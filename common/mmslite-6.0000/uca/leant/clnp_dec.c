/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1996, All Rights Reserved.		        	*/
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : clnp_dec.c						*/
/* PRODUCT(S)  : Lean-T Stack						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*		This module implements the decoding of received NPDUs	*/
/*		for the Connectionless-mode Network Protocol (CLNP).	*/
/*		Note that the CLNP is implemented as non-segmenting	*/
/*		subset of the full protocol. 				*/
/*									*/
/*	NOTE: 	Compile this module with define ER_PDU to enable	*/
/*		decoding of ER PDUs.					*/
/*									*/
/*  For information see the:						*/
/*	ISO 8473 "Information processing systems - Data communication -	*/
/*	  Protocol providing the connectionless-mode  network service";	*/
/*	ISO 8348 "Information processing systems - Data communication -	*/
/*	  Network service definition. Addendum 1: Connectionless-mode	*/
/*	  transmission".						*/
/*	ISO 9542 "Information processing systems - Telecommunications	*/
/*	  and information exchange beetween systems - End system to	*/
/*	  Intermediate system routing exchange protocol for use with	*/
/*	  ISO 8473.							*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/*			clnpl_decode	 				*/
/*									*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 09/28/00  JRB    13     Chg "discarding" log msg from NERR to IND	*/
/* 04/27/00  MDE    12     Lint cleanup					*/
/* 09/13/99  MDE    11     Added SD_CONST modifiers			*/
/* 11/19/98  JRB    10     Del clnpl_check_mac, use clnp_snet_check_mac.*/
/*			   Move "CLNP_MAC_*" defines to include file.	*/
/*			   For GOOSE:					*/
/*			     If receive GOOSE multicast, pass it up.	*/
/*			     On DT-Ind (clnpl_process_dt), fill in new	*/
/*			     "loc_nsap" field in N_UNITDATA.		*/
/* 10/08/98  MDE    09     Minor logging cleanup 			*/
/* 09/23/97  EJV    08     Added support for UCA_SMP.Moved LLC decode.	*/
/*			   Alligned some code and comments.		*/
/* 05/28/97  EJV    07     Deleted absolete clnp_err.h include.		*/
/* 05/27/97  JRB   7.00    MMSEASE 7.0 release.				*/
/* 05/07/97  JRB    06     Changed non-ANSI "%hu" to "%u".		*/
/* 05/06/97  EJV    05     Replaced SWAP_SHORT macro.			*/
/* 12/09/96  JRB    04     Add clnpl_invalid_mac_handler funct ptr.	*/
/*			   and es_is_config_resp_disable flag.		*/
/* 11/13/96  JRB    03     Add "sysincs.h".				*/
/* 10/04/96  JRB    02     #ifdef MCGW, ignore loc_mac. Pass to user.	*/
/* 05/30/96  EJV    01     Created					*/
/************************************************************************/
#include "glbtypes.h"
#include "sysincs.h"

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;  /* Define for SLOG mechanism    */
#endif

#include "mem_chk.h"
#include "checksum.h"
#include "clnpport.h"
#include "clnp_usr.h"
#include "clnp_log.h"
#include "clnp_sta.h"
#include "clnp_llc.h"
#include "clnp_sne.h"
#include "clnp.h"

/*----------------------------------------------------------------------*/
/* Global variables in this module					*/
/*----------------------------------------------------------------------*/
ST_VOID (*clnpl_invalid_mac_handler) (SN_UNITDATA *sn_req);
ST_INT es_is_config_resp_disable;	/* Flag to disable config resp.	*/

/*----------------------------------------------------------------------*/
/* static variables and functions in this module			*/

static ST_RET clnpl_decode_8473_npdu  (ST_UCHAR *pdu_buf, ST_UINT16 pdu_len, SN_UNITDATA *sn_req, ST_INT mac_flag, N_UNITDATA **req);
static ST_RET clnpl_process_dt    (ST_UCHAR *pdu_buf, N_UNITDATA **req);
static ST_RET clnpl_process_er    (ST_UCHAR *pdu_buf);

static ST_RET clnpl_decode_9542_npdu  (ST_UCHAR *pdu_buf, ST_UINT16 pdu_len, ST_UCHAR *rem_mac);
static ST_RET clnpl_process_esh  (ST_UCHAR *pdu_buf, ST_UCHAR *rem_mac);
static ST_RET clnpl_process_ish  (ST_UCHAR *pdu_buf, ST_UCHAR *rem_mac);

static ST_BOOLEAN clnpl_lifetime_expired  (ST_UCHAR pdu_lifetime);
static ST_RET clnpl_last_segment      (ST_UCHAR *pdu_buf);
static ST_RET clnpl_discard_npdu      (ST_UCHAR *pdu_buf, ST_UINT16 pdu_len);


/************************************************************************/
/*			clnpl_decode	 				*/
/*----------------------------------------------------------------------*/
/* This functions decodes the LLC and network layers.			*/
/* This function checks the identifier and protocol version and		*/
/* redirects for specific NPDU decoding.				*/
/* Note that the req pointer may be not be assigned if the received	*/
/* NPDU is not a DT PDU.						*/
/*									*/
/* Parameters:								*/
/*	SN_UNITDATA *sn_req	Pointer to Sub-network Unit Data request*/
/*				request to be decoded.			*/
/*	N_UNITDATA **req 	Address where to return allocated	*/
/*				buffer ptr to N_UNITDATA.indication	*/
/*									*/
/* Return:								*/
/*	SD_SUCCESS (0)		if decoding successful			*/	
/*	error code		otherwise				*/
/************************************************************************/
ST_RET clnpl_decode (SN_UNITDATA *sn_req, N_UNITDATA **req)
{
ST_RET	 	 ret;
ST_UCHAR	*pdu_buf;
ST_UINT16	 pdu_len;
ST_INT	 mac_flag;

  ret = SD_SUCCESS;		/* assume successful decoding		*/

  CLNP_STAT_INC_PDU_RECEIVED;

  /* set ptr and len of NPDU */
  pdu_buf = sn_req->lpdu + LLC_HDR_LEN;
  pdu_len = sn_req->lpdu_len - LLC_HDR_LEN;

  /* get the flag indicating how the received PDU was addressed	*/

#if !defined (MCGW)
  if ((mac_flag = clnp_snet_check_mac (sn_req->loc_mac)) == CLNP_MAC_INVALID)
    {
    /* If user sets function pointer, call function to handle PDU.	*/
    if (clnpl_invalid_mac_handler)
      (*clnpl_invalid_mac_handler) (sn_req);
    ret = CLNP_ERR_PDU_MAC_ADDR;	/* we have to discard the PDU	*/
    }
#endif	/* !defined (MCGW)	*/

  if (ret == SD_SUCCESS)
    {
    /* check the NPDU ISO standard, first byte designates the ISO	*/
    /* standard used							*/

    switch (pdu_buf [0])
      {
      case CLNP_8473_PROT_ID:		/* DT and ER PDUs */
        ret = clnpl_decode_8473_npdu (pdu_buf, pdu_len, sn_req, mac_flag, req);
      break;

      case CLNP_9542_PROT_ID:		/* ES-IS HELLO PDUs */
        ret = clnpl_decode_9542_npdu (pdu_buf, pdu_len, sn_req->rem_mac);
      break;

      default:
        ret = CLNP_ERR_PDU_ID;
      }
    }

  if (ret != SD_SUCCESS)
    {
    /* decoding PDU error, discard it */

    CLNP_LOG_IND1 ("Discarding received NPDU (ret=0x%X)", ret);
    clnpl_discard_npdu (pdu_buf, pdu_len);
    }

  return (ret);
}


/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/*		Decoding functions local to this module			*/
/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/

/************************************************************************/
/*			clnpl_decode_8473_npdu				*/
/*----------------------------------------------------------------------*/
/* This function is used to decode a ISO 8473 (DT or ER PDU).		*/
/*									*/
/* Parameters:								*/
/*	ST_UCHAR    *pdu_buf   	Ptr to received NPDU buffer		*/
/*	ST_UINT16    pdu_len	Data length in pdu_buf (from sub-net)	*/
/*	SN_UNITDATA *sn_req	For local/remote MAC addr (to send ESH)	*/
/*	ST_INT       mac_flag	Flag indicatin how the PDU was addressed*/
/*	N_UNITDATA **req 	Address where to return the acllocated	*/
/*				buf pointer to N_UNITDATA.indication	*/
/*									*/
/* Return:								*/
/*	SD_SUCCESS (0)		if decoding successful			*/	
/*	error code		otherwise				*/
/************************************************************************/
static ST_RET clnpl_decode_8473_npdu ( ST_UCHAR	       *pdu_buf,
					ST_UINT16	pdu_len,
					SN_UNITDATA    *sn_req,
					ST_INT          mac_flag,
					N_UNITDATA    **req)
{
ST_RET		 ret;
CLNP_8473_FHDR	*hdr;
ST_UINT16	 seg_len;
ST_UCHAR 	 pdu_type;

  ret = SD_SUCCESS;		/* assume successful decoding		*/

  hdr = (CLNP_8473_FHDR *) pdu_buf;

    /* check PDU length against the pdu_len received from sub-network	*/

  seg_len = (((ST_UINT16) hdr->seg_len [0]) << 8) | (ST_UINT16) hdr->seg_len [1];

  if (seg_len == pdu_len)
    {
    if (hdr->protocol_ver == CLNP_8473_PROT_VER)	/* check protocol ver	*/
      {
      /* verify the checksum, note that the pos of first checksum byte	*/
      /* is computed as the pointer difference between beginning of	*/
      /* hdr and checksum field 					*/

      if (checksum_verified ((ST_UCHAR *) hdr, hdr->hdr_len,
      		(ST_UINT16) (hdr->checksum - (ST_UCHAR *)hdr), SD_TRUE) == SD_TRUE)
        {
        /* check if the PDU is for us (compare dest addr with local NSAP)	*/

#if !defined (MCGW)	/* For MCGW, don't care what NSAP is.		*/
        if (memcmp (&pdu_buf [CLNP_8473_FHDR_LEN], clnp_param.loc_nsap,
                    (ST_UINT) (pdu_buf [CLNP_8473_FHDR_LEN]+1)) == 0  ||
            mac_flag == CLNP_MAC_GOOSE)
          {
#endif	/* !defined (MCGW)	*/
          /* DT PDU is addressed to our NSAP, process the PDU according to type	*/

          pdu_type = (ST_UCHAR) (hdr->type_flags & CLNP_MASK_FLAGS);

          if (pdu_type == CLNP_8473_TYPE_DT)
            {
            /* DT PDU received, decode it */
            ret = clnpl_process_dt (pdu_buf, req);

            if (ret == SD_SUCCESS)
	      {
              /* copy the remote MAC to N_UNITDATA.indication	*/
              (*req)->rem_mac_valid = SD_TRUE;
              memcpy ((*req)->rem_mac, sn_req->rem_mac, CLNP_MAX_LEN_MAC);

              /* copy the local MAC to N_UNITDATA.indication	*/
              memcpy ((*req)->loc_mac, sn_req->loc_mac, CLNP_MAX_LEN_MAC);

              /* check if received DT PDU was addressed the "All ES Systems,	*/
              /* e.i. should we send ESH to the sender of the DT PDU		*/

	      if (mac_flag == CLNP_MAC_ALL_ES && (!es_is_config_resp_disable))
                clnpl_send_esh (sn_req->rem_mac);
	      }
            }
          else if (pdu_type == CLNP_8473_TYPE_ER)
            {
            /* ER PDU received */
            ret = clnpl_process_er (pdu_buf);
            }
          else
            {
            ret = CLNP_ERR_PDU_TYPE;
            CLNP_LOG_ERR1 ("CLNP-ERROR: not supported ISO 8473 PDU type %u",
	      (ST_UINT) pdu_type);
	    }
#if !defined (MCGW)
	  }
        else
          ret = CLNP_ERR_PDU_NSAP_ADDR;	/* the PDU is not for us	*/
#endif	/* !defined (MCGW)	*/
        }
      else
        {
        ret = CLNP_ERR_PDU_CHECKSUM;
        CLNP_LOG_ERR0 ("CLNP-ERROR: NPDU (DT/ER) checksum verification failed");
        }
      }
    else
      {
      ret = CLNP_ERR_PDU_VER;
      CLNP_LOG_ERR1 ("CLNP-ERROR: not supported ISO 8473 version %d",
        (ST_UINT) hdr->protocol_ver);
      }
    }
  else
    {
    ret = CLNP_ERR_PDU_LEN;
    CLNP_LOG_ERR2 ("CLNP-ERROR: incorrect NPDU length (seg_len=%u, pdu_len=%u)",
      seg_len, pdu_len);
    }

  return (ret);
}


/************************************************************************/
/*			clnpl_process_dt				*/
/*----------------------------------------------------------------------*/
/* This function is used to process info in received DT PDU.		*/
/*									*/
/* Parameters:								*/
/*	ST_UCHAR    *pdu_buf   	Ptr to received DT PDU buffer		*/
/*	N_UNITDATA **req 	Address where to return the allocated	*/
/*				buf pointer to N_UNITDATA.indication	*/
/*									*/
/* Return:								*/
/*	SD_SUCCESS (0)		if processing successful		*/
/*	error code		otherwise				*/
/************************************************************************/
static ST_RET clnpl_process_dt (ST_UCHAR *pdu_buf, N_UNITDATA **req)
{
ST_RET		 ret;
CLNP_8473_FHDR	*hdr;
N_UNITDATA	*req_ind;
ST_UINT16	 seg_len;	/* PDU segment length			*/
ST_UINT16	 udata_len;	/* CLNP-used data length		*/
ST_UINT16	 idx;


  ret = SD_SUCCESS;		/* assume successful decoding 		*/

  CLNP_STAT_INC_DT_RECEIVED;
  clnpl_log_npdu (pdu_buf, CLNP_8473_TYPE_DT, "decode");

  hdr = (CLNP_8473_FHDR *) pdu_buf;

  seg_len = (((ST_UINT16) hdr->seg_len [0]) << 8) | (ST_UINT16) hdr->seg_len [1];

  /* check the PDU's lifetime */
  if (clnpl_lifetime_expired (hdr->pdu_lifetime) == SD_FALSE)
    {
    /* check if last segment received */
    ret = clnpl_last_segment (pdu_buf);

    if (ret == SD_SUCCESS)
      {
      /* we received last segment					*/
      /* alloc the struct for the N_UNITDATA.indication and fill it out	*/

      udata_len = seg_len - (ST_UINT16) hdr->hdr_len;
    
      if ((req_ind = clnp_alloc (udata_len)) != NULL)
        {
        idx = (ST_UINT16) CLNP_8473_FHDR_LEN; 	/* set idx to dest addr */

        /* copy DST addr and len to "loc_nsap".				*/
        memcpy (req_ind->loc_nsap, &pdu_buf [idx], pdu_buf [idx] + 1);

        idx += (ST_UINT16) (pdu_buf [idx]+1);	/* set idx to src addr */

        /* copy SRC addr and len to "rem_nsap".				*/
        memcpy (req_ind->rem_nsap, &pdu_buf [idx], pdu_buf [idx] + 1);
      
        /* SKIP parsing the Option part (if any exists) */
    
	/* copy the data to req.indication */
        req_ind->data_len = udata_len;

        idx = (ST_UINT16) hdr->hdr_len;		/* set idx to data	*/
        memcpy (req_ind->data_buf, &pdu_buf [idx], udata_len);

        *req = req_ind;
        }
      else
        ret = CLNP_ERR_MEM_ALLOC;
      }
    }
  else
    {
    ret = CLNP_ERR_PDU_EXPIRED;	/* PDU lifetime elapsed		*/
    CLNP_STAT_INC_DT_EXPIRED;
    }

  if (ret != SD_SUCCESS)
    {
    CLNP_STAT_INC_DT_DISCARDED;
    CLNP_LOG_NERR1 ("CLNP-NERR: Discarding received DT NPDU (ret=0x%X)", ret);
    }

  return (ret);
}


#if defined(ER_PDU)
/************************************************************************/
/*			clnpl_process_er				*/
/*----------------------------------------------------------------------*/
/* This function is used to decode an ER PDU.				*/
/*									*/
/* Parameters:								*/
/*	ST_UCHAR    *pdu_buf   	Ptr to received NPDU buffer		*/
/*									*/
/* Return:								*/
/*	SD_SUCCESS (0)		if decoding successful			*/	
/*	error code		otherwise				*/
/************************************************************************/
static ST_RET clnpl_process_er (ST_UCHAR *pdu_buf)
{
ST_RET		ret;
CLNP_8473_FHDR *hdr;
ST_UINT16	seg_len;	/* PDU segment length			*/
ST_UINT16	reason_idx;

  ret = SD_SUCCESS;		/* assume successful decoding 		*/

  CLNP_STAT_INC_ER_RECEIVED;
  clnpl_log_npdu (pdu_buf, CLNP_8473_TYPE_ER, "decode");

  hdr = (CLNP_8473_FHDR *) pdu_buf;

  seg_len = (((ST_UINT16) hdr->seg_len [0]) << 8) | (ST_UINT16) hdr->seg_len [1]);

  reason_idx = (ST_UINT16) (seg_len - hdr->hdr_len - 2);

  /* check the PDU's lifetime */
  if (clnpl_lifetime_expired (hdr->pdu_lifetime) == SD_FALSE)
    {
    /* log the reason why our DT PDU has been discarded */
    CLNP_LOG_NERR2 ("CLNP-NERR: Error Report received: DT PDU discard reason=0x%2X, offending octet number=%u", 
      pdu_buf [reason_idx], pdu_buf [reason_idx+1]);
    }
  else
    {
    ret = CLNP_ERR_PDU_EXPIRED;	/* PDU lifetime elapsed		*/
    CLNP_STAT_INC_ER_EXPIRED;
    }

  if (ret != SD_SUCCESS)
    {
    CLNP_STAT_INC_ER_DISCARDED;
    CLNP_LOG_NERR1 ("CLNP-NERR: Discarding received ER PDU (ret=0x%X)", ret);
    }

  return (ret);
}
#else	/* function if ER_PDU not defined */
static ST_RET clnpl_process_er (ST_UCHAR *pdu_buf)
{
ST_RET ret;

  ret = CLNP_ERR_PDU_ER_PDU;

  CLNP_STAT_INC_ER_RECEIVED;

  CLNP_STAT_INC_ER_DISCARDED;
  CLNP_LOG_NERR1 ("CLNP-NERR: Discarding received ER NPDU (ret=0x%X)", ret);

  return (ret);
}
#endif


/************************************************************************/
/*			clnpl_decode_9542_npdu				*/
/*----------------------------------------------------------------------*/
/* This function is used to decode a ISO 9542 (ESH or ISH PDU).		*/
/*									*/
/* Parameters:								*/
/*	ST_UCHAR    *pdu_buf   	Ptr to received NPDU buffer		*/
/*	ST_UINT16    pdu_len	Data length in pdu_buf (from sub-net)	*/
/*	ST_UCHAR    *rem_mac	Pointer to remote MAC addr		*/
/*									*/
/* Return:								*/
/*	SD_SUCCESS (0)		if decoding successful			*/	
/*	error code		otherwise				*/
/************************************************************************/
static ST_RET clnpl_decode_9542_npdu ( ST_UCHAR	       *pdu_buf,
					ST_UINT16	pdu_len,
					ST_UCHAR       *rem_mac)
{
ST_RET		 ret;
CLNP_9542_FHDR	*hdr;

  ret = SD_SUCCESS;		/* assume successful decoding		*/

  hdr = (CLNP_9542_FHDR *) pdu_buf;

  /* check PDU length against the pdu_len received from sub-network	*/
  if ((ST_UINT16) hdr->hdr_len == pdu_len)
    {
    /* check the version of the protocol */
    if (hdr->protocol_ver == CLNP_9542_PROT_VER)
      {
      if (checksum_verified ((ST_UCHAR *) hdr, hdr->hdr_len,
		(ST_UINT16) (hdr->checksum - (ST_UCHAR *)hdr), SD_TRUE) == SD_TRUE)
        {
        /* process the PDU according to it's type */
        if (hdr->type == CLNP_9542_TYPE_ESH)
          {
          /* End System Hello received */
          ret = clnpl_process_esh (pdu_buf, rem_mac);
          }
        else if (hdr->type == CLNP_9542_TYPE_ISH)
          {
          /* Intermediate System Hello received */
          ret = clnpl_process_ish (pdu_buf, rem_mac);
          }
        else
          /* wrong type, RD PDU not supported */
          ret = CLNP_ERR_PDU_TYPE;
	}
      else
        {
        ret = CLNP_ERR_PDU_CHECKSUM;
        CLNP_LOG_ERR0 ("CLNP-ERROR: ESH/ISH PDU checksum verification failed");
	}
      }
    else
      {
      ret = CLNP_ERR_PDU_VER;
      CLNP_LOG_ERR1 ("CLNP-ERROR: not supported ISO 9542 version %d",
        (ST_UINT) hdr->protocol_ver);
      }
    }
  else
    {
    ret = CLNP_ERR_PDU_LEN;		/* PDU length problem	*/
    CLNP_LOG_ERR2 ("CLNP-ERROR: NPDU length error (hdr_len=%u, pdu_len=%u)",
      (ST_UINT16) hdr->hdr_len, pdu_len);
    }

  return (ret);
}


/************************************************************************/
/*			clnpl_process_esh				*/
/*----------------------------------------------------------------------*/
/* This function is used to process info from ESH PDU.			*/
/*									*/
/* Parameters:								*/
/*	ST_UCHAR *pdu_buf   	Ptr to received ESH PDU buffer		*/
/*	ST_UCHAR *rem_mac	Ptr to remote MAC address		*/
/*									*/
/* Return:								*/
/*	SD_SUCCESS (0)		if processing successful		*/
/*	error code		otherwise				*/
/************************************************************************/
static ST_RET clnpl_process_esh (ST_UCHAR *pdu_buf, ST_UCHAR *rem_mac)
{
ST_RET		ret;
CLNP_9542_FHDR *hdr;
ST_UINT16	idx;
ST_UINT16	holding_time;
ST_UCHAR	addr_cnt;

  ret = SD_SUCCESS;			/* assume successful decoding	*/

  hdr = (CLNP_9542_FHDR *) pdu_buf;

  holding_time = (((ST_UINT16) hdr->holding_time [0]) << 8) | (ST_UINT16) hdr->holding_time [1];

  /* decode the remote NSAPs addresses */

  idx = (ST_UINT16) CLNP_9542_FHDR_LEN; 	/* set idx to num of adresses	*/
  addr_cnt = pdu_buf [idx];
  ++idx;				/* set idx to first NSAP address*/
  while (addr_cnt > 0 && ret == SD_SUCCESS)
    {
    ret = clnp_snet_update_es (&pdu_buf [idx], rem_mac, holding_time);

    idx += (ST_UINT16) (pdu_buf [idx] + 1);  /* set idx to next NSAP addr	*/
    --addr_cnt;
    }

  return (ret);
}


/************************************************************************/
/*			clnpl_process_ish				*/
/*----------------------------------------------------------------------*/
/* This function is used to process info from ISH PDU.			*/
/*									*/
/* Parameters:								*/
/*	ST_UCHAR    *pdu_buf   	Ptr to received ISH PDU buffer		*/
/*	ST_UCHAR    *rem_mac	Ptr to remote MAC address		*/
/*									*/
/* Return:								*/
/*	SD_SUCCESS (0)		if processing successful		*/
/*	error code		otherwise				*/
/************************************************************************/
static ST_RET clnpl_process_ish (ST_UCHAR *pdu_buf, ST_UCHAR *rem_mac)
{
ST_RET		ret;
CLNP_9542_FHDR *hdr;
ST_UINT16	holding_time;

  ret = SD_SUCCESS;			/* assume successful decoding	*/

  hdr = (CLNP_9542_FHDR *) pdu_buf;

  holding_time = (((ST_UINT16) hdr->holding_time [0]) << 8) | (ST_UINT16) hdr->holding_time [1];

  /* update the IS Network Entity Title (NET) in our table		*/
  ret = clnp_snet_update_is (&pdu_buf [CLNP_9542_FHDR_LEN], rem_mac, holding_time);

  return (ret);
}


/************************************************************************/
/*			clnpl_lifetime_expired				*/
/*----------------------------------------------------------------------*/
/* This function is used to check if the PDU lifetime has elapsed. In	*/
/* which case the PDU should be discarded.				*/
/* Because we are end system and  we will not forward any PDUs so	*/
/* updating the lifetime in the PDU and recalculation of checksum is	*/
/* not neccessary.							*/
/*									*/
/* Parameters:								*/
/*	ST_UCHAR    pdu_lifetime	From header of received PDU	*/
/*									*/
/* Return:								*/
/*	SD_TRUE (1)			if PDU lifetime expired.	*/
/*	SD_FALSE (0)			otherwise			*/
/************************************************************************/
static ST_BOOLEAN clnpl_lifetime_expired (ST_UCHAR pdu_lifetime)
{
ST_BOOLEAN ret;

  ret = SD_FALSE;

  if (pdu_lifetime < clnp_param.pdu_lifetime_dec)
    ret = SD_TRUE;

  return (ret);
}


/************************************************************************/
/*			clnpl_last_segment				*/
/*----------------------------------------------------------------------*/
/* This function is used to check if last segment arrived.		*/
/*									*/
/* Parameters:								*/
/*	ST_UCHAR *pdu_buf	Pointer to DT PDU in buffer		*/
/*									*/
/* Return:								*/
/*	SD_SUCCESS (0)		if last segment received		*/
/*	error code		otherwise				*/
/************************************************************************/
static ST_RET clnpl_last_segment (ST_UCHAR *pdu_buf)
{
ST_RET			 ret;
CLNP_8473_FHDR		*hdr;
CLNP_8473_HDR_SEG	*hdr_seg;	/* pointer to segmentation part	*/
ST_UINT16		 seg_len;
ST_UINT16		 pdu_total_len;
ST_UINT16		 idx;

  ret = SD_SUCCESS;		/* assume last sement received	*/
  
  hdr = (CLNP_8473_FHDR *) pdu_buf;

  /* Check if segmenting permitted flag set */
  if (hdr->type_flags & CLNP_SEG_PERMITTED)
    {
    /* currently we support only non-segmented PDUs, so check if the	*/
    /* whole PDU arrived in this packet					*/

    if (hdr->type_flags & CLNP_MORE_SEG)
      /* the original PDU is segmented, we do not support segmenting	*/
      ret = CLNP_ERR_PDU_SEGMENTING;
    else
      {
      /* More segments bit not set (it is last segment) */
      idx = (ST_UINT16) CLNP_8473_FHDR_LEN;	/* set idx to dest addr */
      idx += (ST_UINT16) (pdu_buf [idx] + 1);	/* set idx to src addr	*/
      idx += (ST_UINT16) (pdu_buf [idx] + 1);	/* set idx to seg. part	*/

      hdr_seg = (CLNP_8473_HDR_SEG *) &pdu_buf [idx];

      seg_len = *((ST_UINT16 *) hdr->seg_len);
      pdu_total_len = *((ST_UINT16 *) hdr_seg->pdu_total_len);

      if (seg_len == pdu_total_len)	/* not swapped	*/
        {
	/* the original PDU was not segmented so we can process the PDU	*/
	ret = SD_SUCCESS;
	}
      else
	/* the original PDU was segmented, we do not support segmenting	*/
        ret = CLNP_ERR_PDU_SEGMENTING;
      }
    }
  else
    {
    /* segmenting not permitted, make sure that the more segments bit is not set */
    if (hdr->type_flags & CLNP_MORE_SEG)
      ret = CLNP_ERR_PDU_LAST_SEG;
    }

  return (ret);
}


/************************************************************************/
/*			clnpl_discard_npdu				*/
/*----------------------------------------------------------------------*/
/* This function will handle the discarding of a received DT PDU.	*/
/* It will check if an ER PDU should be generated for a PDU which can	*/
/* not be properly process.						*/
/* 									*/
/* Parameters:								*/
/*	ST_UCHAR    *pdu_buf   	Ptr to PDU to discard			*/
/*	USHORTE	     pdu_len   	Length of PDU to discard		*/
/*									*/
/* Return:								*/
/*	SD_SUCCESS (0)			if function successful		*/
/*	error code			otherwise			*/
/************************************************************************/
static ST_RET clnpl_discard_npdu (ST_UCHAR *pdu_buf, ST_UINT16 pdu_len)
{
ST_RET ret;

  ret = SD_SUCCESS;

  CLNP_STAT_INC_PDU_DISCARDED;

  /* !!! log info about discarded PDU */

  return (ret);
}


#if defined(DEBUG_SISCO)
/************************************************************************/
/*			clnpl_log_npdu					*/
/*----------------------------------------------------------------------*/
/* This function is used to log info from DT/ER PDU which is beeing	*/
/* decoded or which has been encoded.					*/
/*									*/
/* Parameters:								*/
/*	ST_UCHAR *pdu_buf   	Ptr to received DT PDU buffer		*/
/*	ST_UCHAR  npdu_type	CLNP_8473_TYPE_DT or CLNP_8473_TYPE_ER	*/
/*	ST_CHAR	 *enc_dec_str	String "encode" or "decode"		*/
/*									*/
/* Return:								*/
/*	ST_VOID			none					*/
/************************************************************************/
ST_VOID clnpl_log_npdu (ST_UCHAR *pdu_buf, ST_UCHAR npdu_type, ST_CHAR *enc_dec_str)
{
CLNP_8473_FHDR	*hdr;
ST_UINT16	 data_len;	/* CLNP-user, or Error Rep data length	*/
ST_UINT16	 idx;
ST_CHAR		 str [10];
ST_UINT16	 seg_len;

  hdr = (CLNP_8473_FHDR *) pdu_buf;

  if (npdu_type == CLNP_8473_TYPE_DT)
    strcpy (str, "DT");
  else if (npdu_type == CLNP_8473_TYPE_ER)
    strcpy (str, "ER");
  else
    return;

  CLNP_LOG_ENC_DEC2  ("CLNP %s %s NPDU:", enc_dec_str, str);

  CLNP_LOGC_ENC_DEC1 ("Header total_len=%u, fixed part:", (ST_UINT) hdr->hdr_len);
  CLNP_LOGH_ENC_DEC ((ST_UINT) CLNP_8473_FHDR_LEN, pdu_buf);

  idx = (ST_UINT16) CLNP_8473_FHDR_LEN; 	/* set idx to dest addr */

  CLNP_LOGC_ENC_DEC1 ("Dst NSAP len=%u", (ST_UINT) pdu_buf [idx]);
  CLNP_LOGH_ENC_DEC ((ST_UINT) pdu_buf [idx], &pdu_buf [idx]+1);

  idx += (ST_UINT16) (pdu_buf [idx]+1);	/* set idx to src addr */

  CLNP_LOGC_ENC_DEC1 ("Src NSAP len=%u", (ST_UINT) pdu_buf [idx]);
  CLNP_LOGH_ENC_DEC ((ST_UINT) pdu_buf [idx], &pdu_buf [idx]+1);

  idx += (ST_UINT16) (pdu_buf [idx]+1);	/* set idx to seg part, or	*/
					/* options part, or data	*/
  if (npdu_type == CLNP_8473_TYPE_DT)
    {
    if (hdr->type_flags & CLNP_SEG_PERMITTED)
      {
      CLNP_LOGC_ENC_DEC1 ("Segmenting part len=%u", (ST_UINT) CLNP_8473_HDR_SEG_LEN);
      CLNP_LOGH_ENC_DEC  ((ST_UINT) CLNP_8473_HDR_SEG_LEN, &pdu_buf [idx]);

      idx += (ST_UINT16) CLNP_8473_HDR_SEG_LEN; /* set idx to options part or data */
      }
    else
      CLNP_LOGC_ENC_DEC0 ("Segmenting part not present");
    }

  if (idx < (ST_UINT16) hdr->hdr_len)
    {
    CLNP_LOGC_ENC_DEC1 ("Options part len=%u", (ST_UINT) (hdr->hdr_len - idx));
    CLNP_LOGH_ENC_DEC ((ST_UINT) (hdr->hdr_len - idx), &pdu_buf [idx]);
    }
  else
    CLNP_LOGC_ENC_DEC0 ("Options part not present");

  seg_len = (((ST_UINT16) hdr->seg_len [0]) << 8) | (ST_UINT16) hdr->seg_len [1];
  data_len = seg_len - (ST_UINT16) hdr->hdr_len;

  idx = (ST_UINT16) hdr->hdr_len;		/* set idx to data	*/

  if (npdu_type == CLNP_8473_TYPE_DT)
    {
    CLNP_LOGC_ENC_DEC1 ("Data len=%u", data_len);
    }
  else
    CLNP_LOGC_ENC_DEC1 ("Error Report Data len=%u", data_len);

  if (data_len > 0)
    {
    CLNP_LOGH_ENC_DEC  ((ST_UINT) data_len, &pdu_buf [idx]);
    }
}
#else	/* function if DEBUG_SISCO not defined */
ST_VOID clnpl_log_npdu (ST_UCHAR *pdu_buf, ST_UCHAR npdu_type, ST_CHAR *enc_dec_str)
{
  return;
}
#endif



