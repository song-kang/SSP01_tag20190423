/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1996, All Rights Reserved.		        	*/
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : clnp_sne.c						*/
/* PRODUCT(S)  : Lean-T Stack						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*		This module provides the interface between the		*/
/*		Connectionless-mode Network Protocol (CLNP) and the	*/
/*		ADLC sub-network.					*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/*			clnp_snet_init					*/
/*			clnp_snet_term					*/
/*			clnp_snet_read					*/
/*			clnp_snet_write_raw				*/
/*			clnp_snet_ext_write				*/
/*			clnp_snet_timer_tick 				*/
/*			clnp_snet_update_is				*/
/*			clnp_snet_update_es				*/
/*			clnp_snet_lookup_is				*/
/*			clnp_snet_lookup_es				*/
/*			clnp_snet_get_all_is_mac			*/
/*			clnp_snet_get_all_es_mac			*/
/*			clnp_snet_get_local_mac				*/
/*			clnp_snet_get_max_udata_len			*/
/*			clnp_snet_get_type				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 02/05/07  JRB    15     Repl clnp_snet_write w/ clnp_snet_write_raw.	*/
/* 04/27/00  MDE    14     Lint cleanup					*/
/* 04/06/00  JRB    13     Use new subnet function "clnp_snet_free"	*/
/*			   instead of "free_lpdu" flag. More flexible.	*/
/* 09/13/99  MDE    12     Added SD_CONST modifiers			*/
/* 01/05/98  JRB    11	   Added clnp_snet_check_mac.			*/
/* 08/13/98  JRB    10     Lint cleanup.				*/
/* 10/29/97  EJV    09     Added check for max_count=0 in function	*/
/*			   clnp_snet_create_es_table.			*/
/* 10/06/97  EJV    08     Added support for UCA_SMP.			*/
/* 09/23/97  EJV    07     Added clnp_snet_ext_write for UCA_SMP.	*/
/* 06/19/97  EJV    06     Implemented static routing table for WIN32	*/
/* 05/28/97  EJV    05     Deleted absolete clnp_err.h include.		*/
/* 05/27/97  JRB   7.00    MMSEASE 7.0 release.				*/
/* 11/13/96  JRB    04     Don't init globals unless they are constant.	*/
/* 11/13/96  JRB    03     Add "sysincs.h".				*/
/* 07/22/96  EJV    02     Added func clnp_snet_get_type		*/
/* 06/05/96  EJV    01     Created					*/
/************************************************************************/
#include "glbtypes.h"
#include "sysincs.h"

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;  /* Define for SLOG mechanism    */
#endif

#include "mem_chk.h"
#include "clnpport.h"
#include "clnp_usr.h"
#include "clnp_sne.h"
#include "clnp_llc.h"

#include "adlc_usr.h"

#if defined(UCA_SMP)
#include "smp_usr.h"
#endif

/*----------------------------------------------------------------------*/
/* defines and variables local to this module				*/
/*----------------------------------------------------------------------*/

#define ADLC_LEN_HEAD		3
#define ADLC_MAX_LEN_LSDU	32767	/* ADLC limit of data packet	*/
#define ADLC_MAX_LEN_UDATA	(ADLC_MAX_LEN_LSDU - ADLC_LEN_HEAD)

/* Variables below are used to store the Intermediate System (IS)	*/
/* address to which packets will be directed if remote MAC address of	*/
/* End System is unknown.  Current implementation allows for only 1 IS	*/
/* address to be stored.						*/

static ST_UINT16 is_mac;		     		/* IS System MAC	*/
static ST_UCHAR  is_net [CLNP_MAX_LEN_NSAP + 1]; 	/* IS Network Entity Title	*/
						/* (tbl of one address)		*/
static ST_UINT16 is_holding_time;	 		/* If 0, IS addr invalid*/


#if defined(_WIN32)
typedef struct
  {
  ST_UINT16 es_mac;		     		/* End System MAC	*/
  ST_UCHAR  es_nsap [1 + CLNP_MAX_LEN_NSAP]; 	/* ES NSAP, +1 for len	*/
  ST_UINT16 es_holding_time;	 		/* If 0, ES addr invalid*/
  } ES_TABLE;

static ES_TABLE *es_table = NULL;	/* Table to maintain ES addresses */
static ST_UINT   es_table_max_cnt = 0;	/* Max count of ES addresses in table	*/
#endif


/************************************************************************/
/*			clnp_snet_init					*/
/*----------------------------------------------------------------------*/
/* This function will initialize the interface between the CLNP and	*/
/* underlying sub-network.						*/
/*									*/
/* Parameters:								*/
/*	CLNP_PARAM	*clnp_param	POinter to CLNP parameters.	*/
/*									*/
/* Return:								*/
/*	SD_SUCCESS (0)		if initialization successful		*/
/*	error code		otherwise				*/
/************************************************************************/
ST_RET clnp_snet_init (CLNP_PARAM *clnp_param)
{
ST_RET		 ret;
ADLC_CONNECT_ALL stConAll;

  ret = SNET_ERR_INIT;		/* assume initialization failed	*/

  if (adlcInit () == SD_SUCCESS)
    {
    /* Because the CLNP is connection-less we connect to all configured	*/
    /* remote MAC addresses during initialization time so any read/write*/
    /* transaction does not require checking for connections.		*/
    /* If linked to ADLC SLAVE the adlcConnectAll() is ignored.		*/

    stConAll.i.localAddr = *((ST_UINT16 *) clnp_param->loc_mac);
    adlcConnectAll (&stConAll);
    
    ret = SD_SUCCESS;
    }

  return (ret);
}


/************************************************************************/
/*			clnp_snet_term					*/
/*----------------------------------------------------------------------*/
/* This function will terminate the interface between the CLNP and	*/
/* undelying sub-network.						*/
/*									*/
/* Parameters:								*/
/*	ST_VOID			none					*/
/*									*/
/* Return:								*/
/*	SD_SUCCESS (0)		if termination successful		*/
/*	error code		otherwise				*/
/************************************************************************/
ST_RET clnp_snet_term (ST_VOID)
{
ST_RET		    ret;
ADLC_DISCONNECT_ALL stDisconAll;

  ret = SD_SUCCESS;		/* assume termination successful	*/

  /* Because the CLNP is connection-less we connect to all configured	*/
  /* devices during initialization time and disconnect all during	*/
  /* termination.							*/

  stDisconAll.i.localAddr = *((ST_UINT16 *) clnp_param.loc_mac);

  adlcDisconnectAll (&stDisconAll);

  /* We may be need time to properly disconnect before calling adlcExit	*/

  adlcExit ();

#if defined(_WIN32)
  /* clean ES routing table */
  if (es_table != NULL)
    {
    chk_free (es_table);
    es_table = NULL;
    es_table_max_cnt = 0;
    }
#endif

  return (ret);
}


/************************************************************************/
/*			clnp_snet_read					*/
/*----------------------------------------------------------------------*/
/* This function will receive a LPDU from a sub-network.		*/
/*									*/
/* Parameters:								*/
/*	SN_UNITDATA *sn_req	Pointer to Sub-network Unit Data request*/
/*				to be received.				*/
/*									*/
/* Return:								*/
/*	SD_SUCCESS (0)		if read successful			*/
/*	error code		otherwise				*/
/************************************************************************/
ST_RET clnp_snet_read (SN_UNITDATA *sn_req)
{
ST_RET			ret;
ST_INT			opCode;
ST_RET			retCode;
ST_VOID			*adlcCb;

  ret = SNET_ERR_READ;	/* assume read failed (nothing arrived)	*/

  /* request data from sub-network */
  if ((adlcCb = adlcGetEvent (&opCode, &retCode)) != NULL)
    {
    switch (opCode)
      {
      case ADLC_CMD_WRITE_PACKET:
        {
        ADLC_WRITE_PACKET *adlcPacket = (ADLC_WRITE_PACKET *) adlcCb;

        /* data received, pass the LPDU to CLNP for LLC decoding */
        memcpy (sn_req->loc_mac, (ST_UCHAR *) &adlcPacket->i.localAddr,
                sizeof (adlcPacket->i.localAddr));		/* sizeof(ST_UINT16) for ADLC */
        memcpy (sn_req->rem_mac, (ST_UCHAR *) &adlcPacket->i.remoteAddr,
                sizeof (adlcPacket->i.remoteAddr));		/* sizeof(ST_UINT16) for ADLC */
        sn_req->lpdu_len = (ST_UINT16) adlcPacket->i.dataSize;
        sn_req->lpdu = (ST_UCHAR *) adlcPacket->i.data;	/* ptr alloc by ADLC */
        /* lpdu freed in clnp_snet_free.				*/
        ret = SD_SUCCESS;
        }
      break;

#if defined(UCA_SMP)
      case ADLC_CMD_EXT_WRITE_PACKET:
        {
        ADLC_EXT_WRITE_PACKET *adlcExtPacket = (ADLC_EXT_WRITE_PACKET *) adlcCb;

        /* data received, pass the LPDU to CLNP for LLC decoding */
        memcpy (sn_req->loc_mac, (ST_UCHAR *) &adlcExtPacket->i.localAddr,
                sizeof (adlcExtPacket->i.localAddr));		/* sizeof(ST_UINT16) for ADLC */
        memcpy (sn_req->rem_mac, (ST_UCHAR *) &adlcExtPacket->i.remoteAddr,
                sizeof (adlcExtPacket->i.remoteAddr));		/* sizeof(ST_UINT16) for ADLC */
        sn_req->lpdu_len = (ST_UINT16) adlcExtPacket->i.dataSize;
        sn_req->lpdu = (ST_UCHAR *) adlcExtPacket->i.data;	/* ptr alloc by ADLC */
        /* lpdu freed in clnp_snet_free.				*/
        ret = SD_SUCCESS;
        }
      break;
  
      case ADLC_CMD_EXT_WRITE_PACKET_DONE: 
        {
        ADLC_EXT_WRITE_PACKET_DONE *adlcExtPacketDone = (ADLC_EXT_WRITE_PACKET_DONE *) adlcCb;
  
        /* Call directly the smp function to process this event 	*/
        smp_send_pdu_callback (retCode,
                               adlcExtPacketDone->i.userId,
                               (ST_UCHAR *) adlcExtPacketDone->i.recTime,
                               sizeof (adlcExtPacketDone->i.recTime));
        }
      break;
#endif /* UCA_SMP */

      default:
        /* no other event handling implemented, ignoring */
      break;
      }

    /* free mem allocated for msg */
    chk_free (adlcCb);
    }

  return (ret);
}


/************************************************************************/
/*			clnp_snet_write_raw				*/
/*----------------------------------------------------------------------*/
/* This function will send a LPDU to a sub-network.			*/
/* An Ethernet frame is passed and it must be converted to an ADLC packet.*/
/*									*/
/* Return:								*/
/*	SD_SUCCESS (0)		if write successful			*/
/*	error code		otherwise				*/
/************************************************************************/
ST_RET clnp_snet_write_raw (
	ST_UCHAR *framePtr,	/* Pointer to complete frame including MACs*/
	size_t    frameLen)	/* Length of complete frame		*/
{
ST_RET		  ret;
ADLC_WRITE_PACKET adlcPacket;

  ret = SNET_ERR_WRITE;		/* assume write failed */

  /* fill out the ADLC packet struct */
  adlcPacket.i.remoteAddr = *((ST_UINT16 *) framePtr);
  adlcPacket.i.localAddr =  *((ST_UINT16 *) framePtr+6);
  adlcPacket.i.dataSize = (ST_INT) (((ST_UINT16) framePtr[12] << 8) |
                                     (ST_UINT16) framePtr[13]);
  adlcPacket.i.data = (ST_VOID *) (framePtr+14);
  
  /* send the packet */

  if (adlcWritePacket (&adlcPacket) == SD_SUCCESS)
    ret = SD_SUCCESS;

  /* NOTE: do not free the data pointer here because not all packets	*/
  /*	   are allocated (ESH is not) !!!				*/

  return (ret);
}


#if defined(UCA_SMP)
/************************************************************************/
/*			clnp_snet_ext_write				*/
/*----------------------------------------------------------------------*/
/* This function will send a LPDU to a sub-network and set the callback	*/
/* function to be called when the write actually completes.		*/
/*									*/
/* Parameters:								*/
/*	SN_UNITDATA *sn_req	Pointer to Sub-network Unit Data request*/
/*				to be sent.				*/
/*	ST_LONG      user_id	User id for this write request, saved	*/
/*				and passed in a callback function from	*/
/*				adlcGetEvent().				*/
/*									*/
/* Return:								*/
/*	SD_SUCCESS (0)		if write successful			*/
/*	error code		otherwise				*/
/************************************************************************/
ST_RET clnp_snet_ext_write (SN_UNITDATA *sn_req, ST_LONG user_id)
{
ST_RET		      ret;
ADLC_EXT_WRITE_PACKET adlcExtPacket;

  ret = SNET_ERR_WRITE;		/* assume write failed */

  /* fill out the ADLC packet struct */
  adlcExtPacket.i.remoteAddr = *((ST_UINT16 *) sn_req->rem_mac);
  adlcExtPacket.i.localAddr =  *((ST_UINT16 *) sn_req->loc_mac);
  adlcExtPacket.i.dataSize = (ST_INT) sn_req->lpdu_len;
  adlcExtPacket.i.data = (ST_VOID *) sn_req->lpdu;
  adlcExtPacket.i.userId = user_id;
  
  /* send the packet */

  if (adlcExtWritePacket (&adlcExtPacket) == SD_SUCCESS)
    ret = SD_SUCCESS;

  /* NOTE: do not free the data pointer here because not all packets	*/
  /*	   are allocated. !!!						*/

  return (ret);
}
#endif /* UCA_SMP */


/************************************************************************/
/*			clnp_snet_timer_tick 				*/
/*----------------------------------------------------------------------*/
/* This function is called on 1 second intervals by the CLNP.		*/
/* When this function is called, all Holding Timers for IS or ES	*/
/* addresses should CLNP decremented.					*/
/*									*/
/* Parameters:								*/
/*	ST_VOID			none				       	*/
/*									*/
/* Return:								*/
/*	ST_VOID			none					*/
/************************************************************************/
ST_VOID clnp_snet_timer_tick (ST_VOID)
{
  /* decrement the Holding Time for IS address */
  if (is_holding_time != 0)
    --is_holding_time;

  /* decrement the Holding Time for ES addresses */
  /* not implemented */
}


/************************************************************************/
/*			clnp_snet_update_is				*/
/*----------------------------------------------------------------------*/
/* This function will update the Intermediate System address and is's	*/
/* Holding Time.							*/
/* Note that currently we store only 1 IS address.			*/
/*									*/
/* Parameters:								*/
/*	ST_UCHAR  *rem_nsap	Pointer to remote NSAP address		*/
/*	ST_UCHAR  *rem_mac		Pointer to remote MAC address		*/
/*	ST_UINT16 holding_time	Holding Time for the new address	*/
/*									*/
/* Return:								*/
/*	SD_SUCCESS (0)		if update successful			*/
/*	error code		otherwise				*/
/************************************************************************/
ST_RET clnp_snet_update_is (ST_UCHAR *rem_nsap, ST_UCHAR *rem_mac,
			      ST_UINT16 holding_time)
{
ST_RET ret;

  ret = SD_SUCCESS;

  if (rem_mac != NULL && rem_nsap != NULL)
    {
    /* copy the IS MAC to our global var */
    is_mac = *((ST_UINT16 *) rem_mac);

    /* copy the IS Network Entity Title (NET) to our global var */
    memcpy (is_net, rem_nsap, rem_nsap [0] + 1);

    /* set the Holding Time */
    is_holding_time = holding_time;
    }
  else
    ret = CLNP_ERR_NULL_PTR;

  return (ret);
}


/************************************************************************/
/*			clnp_snet_update_es				*/
/*----------------------------------------------------------------------*/
/* This function will update the End System addresses table.		*/
/* Note that currently we do not maintain the table of ES addresses.	*/
/*									*/
/* Parameters:								*/
/*	ST_UCHAR  *rem_nsap	Pointer to remote NSAP address		*/
/*	ST_UCHAR  *rem_mac	Pointer to remote MAC address		*/
/*	ST_UINT16 holding_time	Holding Time for the new address	*/
/*									*/
/* Return:								*/
/*	SD_SUCCESS (0)		if update successful			*/
/*	error code		otherwise				*/
/************************************************************************/
ST_RET clnp_snet_update_es (ST_UCHAR *rem_nsap, ST_UCHAR *rem_mac, 
			      ST_UINT16 holding_time)
{
ST_RET   ret;
#if defined (_WIN32)
ST_BOOLEAN es_updated = SD_FALSE;
ES_TABLE *es_ptr = es_table;
ST_UINT  i;
#endif

  ret = SD_SUCCESS;

#if defined (_WIN32)
  if (es_table != NULL)
    if ((ST_UINT) rem_nsap [0] <= CLNP_MAX_LEN_NSAP)
      {
      for (i=0; i<es_table_max_cnt; ++i)
        {
        if (es_ptr->es_holding_time == 0)	/* if not used */
          {
          es_ptr->es_mac = *((ST_UINT16 *) rem_mac);
          memcpy (es_ptr->es_nsap, rem_nsap, 1 + (ST_UINT) rem_nsap[0]);
          es_ptr->es_holding_time = 1; 		/* If 0, ES addr invalid */

          es_updated = SD_TRUE;
          break;
    	  }
        ++es_ptr;
        }
      }
  if (!es_updated)
    ret = SNET_ERR_WRITE;			/* update failed */
#endif /* end _WIN32 */

  return (ret);
}


/************************************************************************/
/*			clnp_snet_lookup_is				*/
/*----------------------------------------------------------------------*/
/* This function will lookup the remote MAC address in the NET-MAC	*/
/* table (currently only one IS address is stored.			*/
/*									*/
/* Parameters:								*/
/*	ST_UCHAR  *rem_mac	Pointer to buffer where to copy the	*/
/*				remote MAC address for the IS.		*/
/*				The buffer is at least CLNP_MAX_LEN_MAC	*/
/*				bytes long.				*/
/*									*/
/* Return:								*/
/*	SD_SUCCESS (0)		if remote MAC address copied		*/
/*	error code		otherwise				*/
/************************************************************************/
ST_RET clnp_snet_lookup_is (ST_UCHAR *rem_mac)
{
ST_RET ret;

  ret = SNET_ERR_MAC_INVALID;

  if (is_holding_time > 0)
    {
    *((ST_UINT16 *) rem_mac) = is_mac;
    ret = SD_SUCCESS;
    }

  return (ret);
}


/************************************************************************/
/*			clnp_snet_lookup_es				*/
/*----------------------------------------------------------------------*/
/* This function will lookup the remote MAC address in the NSAP-MAC	*/
/* table.								*/
/*									*/
/* Parameters:								*/
/*	ST_UCHAR  *rem_nsap	Pointer to remote NSAP address. First	*/
/*				byte is the NSAP length.		*/
/*	ST_UCHAR  *rem_mac	Pointer to buffer where to copy the	*/
/*				remote MAC address.			*/
/*				The buffer is at least CLNP_MAX_LEN_MAC	*/
/*				bytes long.				*/
/*									*/
/* Return:								*/
/*	SD_SUCCESS (0)		if remote MAC address copied		*/
/*	error code		otherwise				*/
/************************************************************************/
ST_RET clnp_snet_lookup_es (ST_UCHAR *rem_nsap, ST_UCHAR *rem_mac)
{
ST_RET ret;
#if defined (_WIN32)
ES_TABLE *es_ptr = es_table;
ST_UINT  i;
#endif

  ret = SNET_ERR_MAC_INVALID;

#if defined (_WIN32)
  if (es_table != NULL)
    {
    for (i=0; i<es_table_max_cnt; ++i)
      {
      if (es_ptr->es_holding_time != 0)		/* if entry used */
        if (memcmp(rem_nsap, es_ptr->es_nsap, 1+ (ST_UINT) rem_nsap[0]) == 0)
          {
          /* found match */
          *((ST_UINT16 *) rem_mac) = es_ptr->es_mac;
          ret = SD_SUCCESS;
          break;
    	  }
      ++es_ptr;
      }
    }
#endif /* end _WIN32 */

  return (ret);
}


/************************************************************************/
/*			clnp_snet_get_all_is_mac			*/
/*----------------------------------------------------------------------*/
/* This function will copy to the buffer mac_buf the "All Intermediate	*/
/* Systems" MAC	address for given sub-network.				*/
/*									*/
/* Parameters:								*/
/*	ST_UCHAR  *mac_buf		Pointer to buffer for MAC address.	*/
/*				THe buffer is at least CLNP_MAX_LEN_MAC	*/
/*				bytes long.				*/
/*									*/
/* Return:								*/
/*	SD_SUCCESS (0)		if IS address copied			*/
/*	error code		otherwise				*/
/************************************************************************/
ST_RET clnp_snet_get_all_is_mac (ST_UCHAR *mac_buf)
{
ST_RET ret;

  ret = SD_SUCCESS;

  *((ST_UINT16 *) mac_buf) = ADLC_ALL_IS;

  return (ret);
}


/************************************************************************/
/*			clnp_snet_get_all_es_mac			*/
/*----------------------------------------------------------------------*/
/* This function will copy to the buffer mac_buf the "All End Systems"	*/
/* MAC address for given sub-network.					*/
/*									*/
/* Parameters:								*/
/*	ST_UCHAR  *mac_buf		Pointer to buffer for MAC address.	*/
/*				The buffer is at least CLNP_MAX_LEN_MAC	*/
/*				bytes long.				*/
/*									*/
/* Return:								*/
/*	SD_SUCCESS (0)		if ES address copied			*/
/*	error code		otherwise				*/
/************************************************************************/
ST_RET clnp_snet_get_all_es_mac (ST_UCHAR *mac_buf)
{
ST_RET ret;

  ret = SD_SUCCESS;

  *((ST_UINT16 *) mac_buf) = ADLC_ALL_ES;

  return (ret);
}


/************************************************************************/
/*			clnp_snet_get_local_mac				*/
/*----------------------------------------------------------------------*/
/* This function will copy to the buffer mac_buf the local MAC address	*/
/* for given sub-network.						*/
/*									*/
/* Parameters:								*/
/*	ST_UCHAR  *mac_buf		Pointer to buffer for MAC address.	*/
/*				The buffer is at least CLNP_MAX_LEN_MAC	*/
/*				bytes long.				*/
/*									*/
/* Return:								*/
/*	SD_SUCCESS (0)		if local MAC address copied		*/
/*	error code		otherwise				*/
/************************************************************************/
ST_RET clnp_snet_get_local_mac (ST_UCHAR *mac_buf)
{
ST_RET ret;

  ret = SD_SUCCESS;

  /* for ADLC sub-network the MAC address is configured by N-USER	*/
  *((ST_UINT16 *) mac_buf) = *((ST_UINT16 *)clnp_param.loc_mac);

  return (ret);
}

/************************************************************************/
/*			clnp_snet_get_max_udata_len			*/
/*----------------------------------------------------------------------*/
/* This function will return max length of user data for given sub-net.	*/
/*									*/
/* Parameters:								*/
/*	ST_VOID			none					*/
/*									*/
/* Return:								*/
/*	ST_UINT16			max len of user data			*/
/************************************************************************/
ST_UINT16 clnp_snet_get_max_udata_len (ST_VOID)
{
ST_UINT16 max_udata_len;

  max_udata_len = ADLC_MAX_LEN_UDATA;

  return (max_udata_len);
}

/************************************************************************/
/*			clnp_snet_get_type				*/
/*----------------------------------------------------------------------*/
/* This function will return the sub-network type.			*/
/*									*/
/* Parameters:								*/
/*	ST_VOID			none					*/
/*									*/
/* Return:								*/
/*	ST_INT			SUBNET_ADLC.				*/
/************************************************************************/
ST_INT clnp_snet_get_type (ST_VOID)
{
  return (SUBNET_ADLC);
}

/************************************************************************/
/*			clnp_snet_create_es_table			*/
/*----------------------------------------------------------------------*/
/* This function will create the End System addresses table.		*/
/*									*/
/* Parameters:								*/
/*	ST_UINT max_count	Max number of addresses in table	*/
/*									*/
/* Return:								*/
/*	SD_SUCCESS (0)		if create successful			*/
/*	error code		otherwise				*/
/************************************************************************/
ST_RET clnp_snet_create_es_table (ST_UINT max_count)
{
ST_RET ret;

  ret = SD_SUCCESS;
  
#if defined(_WIN32)
  if (es_table == NULL && max_count > 0)
    {
    es_table = (ES_TABLE *) chk_calloc (max_count, sizeof (ES_TABLE));
    es_table_max_cnt = max_count;
    }
#endif

  return (ret);
}

/************************************************************************/
/*			clnp_snet_check_mac				*/
/*----------------------------------------------------------------------*/
/* This function will compare the mac_addr with	local MAC and All ES MAC*/
/* and return proper flag.						*/
/*									*/
/* Parameters:								*/
/*	ST_UCHAR    *mac_addr  	Ptr to MAC address buffer.		*/
/*									*/
/* Return:								*/
/*	ST_INT			CLNP_MAC_INVALID or			*/
/*				CLNP_MAC_LOCAL or			*/
/*				CLNP_MAC_ALL_ES				*/
/************************************************************************/
ST_INT clnp_snet_check_mac (ST_UCHAR *mac_addr)
{
ST_INT mac_flag;
ST_UINT16 adlc_mac;		/* MAC in ADLC format (UINT)		*/
ST_UINT16 loc_adlc_mac;		/* Local MAC in ADLC format (UINT)	*/

  /* Convert to unsigned 16-bit integers for comparison.		*/
  adlc_mac     = *((ST_UINT16 *) mac_addr);
  loc_adlc_mac = *((ST_UINT16 *) clnp_param.loc_mac);

  if (adlc_mac == loc_adlc_mac)
    mac_flag = CLNP_MAC_LOCAL;		/* our MAC address	*/
  else if (adlc_mac == ADLC_ALL_ES)
    mac_flag = CLNP_MAC_ALL_ES;		/* ALL-ES multicast MAC	*/
  else
    mac_flag = CLNP_MAC_INVALID;

  return (mac_flag);
}

/************************************************************************/
/*			clnp_snet_free					*/
/* Upper (CLNP) layer calls this function to free anything allocated	*/
/* for the SN-UNITDATA PDU.						*/
/************************************************************************/
ST_VOID clnp_snet_free (SN_UNITDATA *sn_req)
  {
  chk_free (sn_req->lpdu);	/* lpdu was allocated by ADLC.		*/
  return;
  }
