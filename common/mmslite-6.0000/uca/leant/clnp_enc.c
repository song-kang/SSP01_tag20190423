/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	1996-2001, All Rights Reserved.			        	*/
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : clnp_enc.c						*/
/* PRODUCT(S)  : Lean-T Stack						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*		This module implements the encoding of the DT PDU,	*/
/*		ER PDU, and ES HELLO PDU for the Connectionless-mode	*/
/*		Network Protocol (CLNP).				*/
/*		Note that the CLNP is implemented as non-segmenting	*/
/*		subset of the full protocol. 				*/
/*									*/
/*	NOTE: 	Compile this module with define ER_PDU to set the Error	*/
/*		Report flag in outgoing DT PDU which will allow to	*/
/*		receive	Error Reports in case our DT PDUs are in error.	*/
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
/*			clnpl_encode_dt					*/
/*			clnpl_encode_esh				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 03/12/01  JRB    12     clnpl_encode_dt: caller sets sn_req->lpdu.	*/
/*			    Eliminates unnecessary allocation.		*/
/* 04/27/00  MDE    11     Lint cleanup					*/
/* 04/27/00  MDE    10     Lint cleanup					*/
/* 09/13/99  MDE    09    Added SD_CONST modifiers			*/
/* 12/29/97  JRB    08     Don't check for chk_calloc return value=NULL.*/
/* 09/23/97  EJV    07     Added support for UCA_SMP.			*/
/*			   Alligned back some code and comments.	*/
/* 05/28/97  EJV    06     Deleted absolete clnp_err.h include.		*/
/* 05/27/97  JRB   7.00    MMSEASE 7.0 release.				*/
/* 05/07/97  JRB    05     Changed non-ANSI "%hu" to "%u".		*/
/* 05/06/97  EJV    04     Replaced SWAP_SHORT macro.			*/
/* 11/13/96  JRB    03     Add "sysincs.h".				*/
/* 07/19/96  EJV    02	   Changed the checking of req->data_len.	*/
/* 05/24/96  EJV    01     Created					*/
/************************************************************************/
#include "glbtypes.h"
#include "sysincs.h"

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;  /* Define for SLOG mechanism    */
#endif

#include "mem_chk.h"
#include "clnpport.h"
#include "clnp_llc.h"
#include "clnp_usr.h"
#include "clnp_log.h"
#include "clnp_sne.h"
#include "clnp.h"
#include <assert.h>


/************************************************************************/
/*			clnpl_encode_dt					*/
/*----------------------------------------------------------------------*/
/* This function is used to encode a DT PDU.				*/
/* NOTE: The caller must set sn_req->lpdu before calling this function.	*/
/*       This function sets sn_req->lpdu_len = # of encoded bytes.	*/
/*									*/
/* Parameters:								*/
/*	SN_UNITDATA *sn_req	Pointer to Sub-network Unit Data request*/
/*				request to be decoded.			*/
/*	N_UNITDATA  *req 	Pointer to CLNP-user data struct.	*/
/*				call to this func).			*/
/*									*/
/* Return:								*/
/*	SD_SUCCESS (0)		if encoding successful			*/
/*	error code		otherwise				*/
/************************************************************************/
ST_RET clnpl_encode_dt (SN_UNITDATA *sn_req, N_UNITDATA *req)
{
ST_RET		ret;
ST_UINT16	enc_len;		/* LLC + NPDU len		*/
ST_UCHAR       *enc_buf;		/* buf for LLC and NPDU encoding*/
ST_UINT16	pdu_len;		/* NPDU length			*/
ST_UCHAR       *pdu_buf;		/* buf for NPDU encoding	*/
ST_UCHAR	hdr_len;		/* DT PDU header length		*/
CLNP_8473_FHDR *hdr;			/* pointer to DT PDU header	*/
ST_UINT16	idx;			/* index into pdu_buf		*/
ST_UINT16	snet_udata_len;

  assert (sn_req->lpdu);		/* make sure caller set lpdu	*/

  ret = SD_SUCCESS;			/* assume successful encoding	*/

  enc_buf = NULL;
  pdu_buf = NULL;

  hdr_len = (ST_UCHAR) (CLNP_8473_FHDR_LEN	 	/* PDU header fixed part*/
	   + req->rem_nsap [0] + 1		/* dest addr + len byte	*/
	   + clnp_param.loc_nsap[0] + 1);	/* src  addr + len byte	*/
				/* no segmenting part, no options part	*/

  /* check if we can fit the header & data into one PDU (actual PDU size)*/
  snet_udata_len = clnp_snet_get_max_udata_len ();
  if (req->data_len + hdr_len + LLC_HDR_LEN <= snet_udata_len)
    {
    /* get total len for encoding buffer (LLC encoding + NPDU encoding)	*/

    pdu_len = (ST_UINT16) (hdr_len + req->data_len);

    enc_len = (ST_UINT16) (LLC_HDR_LEN + pdu_len);
    assert (enc_len <= ETHE_MAX_LEN_UDATA);	/* Must NOT exceed buffer*/
    enc_buf = sn_req->lpdu;	/* Use caller's buffer to encode.	*/ 

    /*------------------------------------------*/
    /* encode the DT PDU header fixed part	*/
    /*------------------------------------------*/

    pdu_buf = enc_buf + LLC_HDR_LEN;
    hdr = (CLNP_8473_FHDR *) pdu_buf;

    hdr->protocol_id  = CLNP_8473_PROT_ID;
    hdr->hdr_len      = hdr_len;
    hdr->protocol_ver = CLNP_8473_PROT_VER;
    hdr->pdu_lifetime = clnp_param.pdu_lifetime;

#if defined(ER_PDU)
    hdr->type_flags   = CLNP_8473_TYPE_DT | CLNP_SEG_NOT_PERMITTED | CLNP_ERR_REP_GENERATE;
    	/* skip ORing with 0s,| CLNP_LAST_SEG */
#else
    hdr->type_flags   = CLNP_8473_TYPE_DT | CLNP_SEG_NOT_PERMITTED;
	/* skip ORing with 0s,| CLNP_LAST_SEG | CLNP_ERR_REP_NOT_GEN;*/
#endif

    hdr->seg_len [0] = (ST_UCHAR) (pdu_len >> 8);     /* most significant byte	*/
    hdr->seg_len [1] = (ST_UCHAR) (pdu_len & 0x00FF); /* least significant byte	*/

    hdr->checksum[0] = hdr->checksum[1] = 0;	/* no checksum used */

    /*----------------------------------------*/
    /* encode the DT PDU src & dest addresses	*/
    /*----------------------------------------*/

    idx = CLNP_8473_FHDR_LEN;			/* set idx to dest addr	*/
    memcpy (&pdu_buf [idx], req->rem_nsap, req->rem_nsap [0] + 1);

    idx += (ST_UINT16) (req->rem_nsap [0] + 1);	/* set idx to src addr	*/
    memcpy (&pdu_buf [idx], clnp_param.loc_nsap, clnp_param.loc_nsap [0] + 1);

    /*----------------------------------------*/
    /* copy CLNP-user data to DT PDU		*/
    /*----------------------------------------*/

    idx += (ST_UINT16) (clnp_param.loc_nsap[0] + 1);	/* set idx to data part	*/
    memcpy (&pdu_buf [idx], req->data_buf, req->data_len);

    /* log the encoded DT PDU */
    clnpl_log_npdu (pdu_buf, CLNP_8473_TYPE_DT, "encode");

    /*----------------------------------------*/
    /* add LLC encoding to DT PDU		*/
    /*----------------------------------------*/

    if ((ret = clnpl_llc_encode (enc_buf, LLC_LSAP, LLC_LSAP)) == SD_SUCCESS)
      {
      /* Save length of encoded data.	*/
      sn_req->lpdu_len = enc_len;
      }
    }
  else
    {
    ret = CLNP_ERR_UDATA_LEN;
    /* clnp_except () */
    CLNP_LOG_ERR2 ("CLNP-ERROR encode failed (NS-USERDATA length too big %u (max allowed %u)",
    	req->data_len, snet_udata_len-hdr_len-LLC_HDR_LEN);
    }

  return (ret);
}


/************************************************************************/
/*			clnpl_encode_esh				*/
/*----------------------------------------------------------------------*/
/* This function is used to encode a ESH PDU (End System Hello PDU).	*/
/*									*/
/* Parameters:								*/
/*	ST_UCHAR  *enc_buf	Ptr to encoding buffer			*/
/*	ST_UINT16 *enc_len	Ptr where to return the encoded len	*/
/*									*/
/* Return:								*/
/*	SD_SUCCESS (0)		if encoding successful			*/
/*	error code		otherwise				*/
/************************************************************************/
ST_RET clnpl_encode_esh (ST_UCHAR *enc_buf, ST_UINT16 *enc_len)
{
ST_RET		ret;
ST_UCHAR       *pdu_buf;		/* buf to encode the ESH PDU	*/
ST_UCHAR	hdr_len;		/* length of ESH PDU = len hdr	*/
CLNP_9542_FHDR *hdr;			/* pointer to ESH PDU header	*/
ST_UINT16	holding_time;		/* How long addr info should be keept */
ST_UINT16	idx;			/* index into pdu_buf		*/

  ret = SD_SUCCESS;			/* assume successful encoding	*/

  hdr_len = (ST_UCHAR) (CLNP_9542_FHDR_LEN 	/* PDU header fixed part*/
  	   + 1					/* number of addresses	*/
	   + clnp_param.loc_nsap[0] + 1);	/* src addr + len byte	*/
	   					/* no option part	*/

  pdu_buf = enc_buf + LLC_HDR_LEN;

  /* encode the ESH PDU header fixed part */
  hdr = (CLNP_9542_FHDR *) pdu_buf;

  hdr->protocol_id  = CLNP_9542_PROT_ID;
  hdr->hdr_len      = hdr_len;
  hdr->protocol_ver = CLNP_9542_PROT_VER;
  hdr->rfu          = 0;			/* reserved must be 0	*/
  hdr->type         = CLNP_9542_TYPE_ESH;

  holding_time = clnp_param.esh_cfg_timer * 2;	/* ISO recomendation for holding time */
  hdr->holding_time [0] = (ST_UCHAR) (holding_time >> 8);     /* most significant byte	*/
  hdr->holding_time [1] = (ST_UCHAR) (holding_time & 0x00FF); /* least significant byte	*/

  hdr->checksum[0] = hdr->checksum[1] = 0; /* checksum not used */

  /* encode the PDU local address (NSAP) we servicing			*/

  idx = CLNP_9542_FHDR_LEN;			/* set idx to # of addr	*/
  pdu_buf [idx] = 1; 				/* only one src addr	*/

  ++idx;					/* set idx to src addr	*/
  memcpy (&pdu_buf [idx], clnp_param.loc_nsap, clnp_param.loc_nsap[0] + 1);

  /* add LLC encoding to ESH PDU */

  *enc_len = (ST_UINT16)(LLC_HDR_LEN + hdr_len);
  ret = clnpl_llc_encode (enc_buf, LLC_LSAP, LLC_LSAP);

  return (ret);
}
