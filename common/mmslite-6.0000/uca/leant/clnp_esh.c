/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1996, All Rights Reserved.		        	*/
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : clnp_esh.c						*/
/* PRODUCT(S)  : Lean-T Stack						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*		This module implements function to manage the End System*/
/*		Hello (ESH).						*/
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
/*			clnpl_send_esh	 				*/
/*									*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 03/20/06  JRB    08     Use new clnp_snet_write_raw.			*/
/* 04/27/00  MDE    07     Lint cleanup					*/
/* 09/13/99  MDE    06     Added SD_CONST modifiers			*/
/* 07/17/97  EJV    05     Disable sending ESH if esh_cfg_timer=0	*/
/* 05/28/97  EJV    04     Deleted absolete clnp_err.h include.		*/
/* 05/27/97  JRB   7.00    MMSEASE 7.0 release.				*/
/* 11/13/96  JRB    03     Don't init globals unless they are constant.	*/
/* 11/13/96  JRB    02     Add "sysincs.h".				*/
/* 05/30/96  EJV    01     Created					*/
/************************************************************************/
#include "glbtypes.h"
#include "sysincs.h"

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;  /* Define for SLOG mechanism    */
#endif

#include "mem_chk.h"
#include "clnp_usr.h"
#include "clnp_log.h"
#include "clnp_sta.h"
#include "clnp_llc.h"
#include "clnp_sne.h"
#include "clnp.h"



/*----------------------------------------------------------------------*/
/* The ESH PDU is encoded in the first call to clnpl_send_esh() and	*/
/* sent each time the Configuration Timer expires or in response to	*/
/* Configuration Query by another End System.				*/
/* The ESH PDU is stored in the clnpl_esh buffer defined below.		*/

/* The max estimated ESH PDU length is:					*/
/*	  ESH PDU header fixed part					*/
/*	+ 1				(one byte for src addr count)	*/
/*	+ 1 + CLNP_MAX_NSAP_LEN		(only one src NSAP & len)	*/
/*					(no options part)		*/

#define CLNP_MAX_ESH_LEN (ST_UINT16)(CLNP_9542_FHDR_LEN + 1 + 1 + CLNP_MAX_LEN_NSAP)


static ST_UCHAR	clnpl_esh [LLC_HDR_LEN + CLNP_MAX_ESH_LEN];
				/* Buffer for encoded ESH PDU		*/

static ST_UINT16	clnpl_esh_len;	/* Actual length of the ESH PDU in the	*/
				/* buffer above. The len includes the 	*/
				/* the LLC encoding.			*/


static ST_UINT16	clnpl_cfg_timer;
				/* Configuration Timer for ES Hello.	*/
				/* Initialized to 0 to send first ESH	*/
				/* PDU soonest the system comes up,	*/
				/* unless a delay is requested in	*/
				/* clnp_param.				*/

/************************************************************************/
/*			clnpl_init_esh	 				*/
/*----------------------------------------------------------------------*/
/* This function calls encoding func for the ESH and sets the		*/
/* Configuration Timer to requested delay before first ESH is be sent.	*/
/*									*/
/* Parameters:								*/
/*	ST_UINT16	delay_time	Delay time in second for first ESH.	*/
/*				Use 0 if no delay for first ESH required*/
/*									*/
/* Return:								*/
/*	SD_SUCCESS (0)		if initialization of ESH successful	*/
/*	error code		otherwise				*/
/************************************************************************/
ST_RET clnpl_init_esh (ST_UINT16 delay_time)
{ 
ST_RET	    ret;

  ret = SD_SUCCESS;		/* assume ESH sent successfully */

  /* The ESH will be encoded only once in static buffer and retained	*/
  /* for the rest of CLNP operation.					*/

  ret = clnpl_encode_esh (clnpl_esh, &clnpl_esh_len);

  clnpl_cfg_timer = delay_time;

  return (ret);
}


/************************************************************************/
/*			clnpl_check_cfg_timer 				*/
/*----------------------------------------------------------------------*/
/* This functions will decrement the Configuration Timer and check	*/
/* if ESH should be sent.						*/
/*									*/
/* Parameters:								*/
/*	ST_VOID			none					*/
/*									*/
/* Return:								*/
/*	ST_VOID			none					*/
/************************************************************************/
ST_VOID clnpl_check_cfg_timer (ST_VOID)
{
  /* decrement the Configuration Timer and if expired send ESH to	*/
  /* "All Intermediate Systems".					*/

  if (clnpl_cfg_timer != 0)
    --clnpl_cfg_timer;

  if (clnpl_cfg_timer == 0)
    {
    /* Note that we added additional option: if the esh_cfg_timer=0 then*/
    /* sending of ESHs will be disabled.			       	*/

    if (clnp_param.esh_cfg_timer != 0)
      {
      clnpl_send_esh (NULL);	/* send to All IS */
      clnpl_cfg_timer = clnp_param.esh_cfg_timer; /* reset the Cfg Timer */
      }
    }
}


/************************************************************************/
/*			clnpl_send_esh	 				*/
/*----------------------------------------------------------------------*/
/* This functions sends ESH.						*/
/*									*/
/* Parameters:								*/
/*	ST_UCHAR	*rem_mac	Pointer to remote MAC addr. If NULL the	*/
/*				"All IS" MAC address will be used.	*/
/*									*/
/* Return:								*/
/*	SD_SUCCESS (0)		if ESH sent				*/	
/*	error code		otherwise				*/
/************************************************************************/
ST_RET clnpl_send_esh (ST_UCHAR *rem_mac)
{ 
ST_RET	    ret;
ST_UCHAR tmp_rem_mac [CLNP_MAX_LEN_MAC];
ST_UCHAR raw_packet [200];

  ret = SD_SUCCESS;		/* assume ESH sent successfully */

  if (rem_mac != NULL)
    memcpy (tmp_rem_mac, rem_mac, CLNP_MAX_LEN_MAC);
  else
    /* send ESH to All IS */
    clnp_snet_get_all_is_mac (tmp_rem_mac);

  /* Build raw packet and send it.	*/
  assert (clnpl_esh_len + 14 <= sizeof (raw_packet));
  /* Set the DST MAC address	*/
  memcpy (raw_packet, tmp_rem_mac, CLNP_MAX_LEN_MAC);
  /* Set the SRC MAC address	*/
  memcpy (&raw_packet[6], clnp_param.loc_mac, CLNP_MAX_LEN_MAC);
  /* Set the 802.3 length/type field (bytes 12 and 13).	*/
  raw_packet[12] = (ST_UCHAR) (clnpl_esh_len >> 8);	
  raw_packet[13] = (ST_UCHAR) (clnpl_esh_len & 0xff);
  /* Copy data part of ESH packet.	*/
  memcpy (&raw_packet[14], clnpl_esh, clnpl_esh_len);

  ret = clnp_snet_write_raw (raw_packet, clnpl_esh_len+14);
  
  if (ret == SD_SUCCESS)
    {
    CLNP_STAT_INC_PDU_SENT;
    }
  else
    CLNP_LOG_ERR0 ("CLNP-ERROR: Send ESH failed");

  return (ret);
}
