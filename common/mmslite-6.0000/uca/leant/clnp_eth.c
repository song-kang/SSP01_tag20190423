/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	1996-2005, All Rights Reserved.					*/
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : clnp_eth.c						*/
/* PRODUCT(S)  : Lean-T Stack						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*		This module provides the interface between the		*/
/*		Connectionless-mode Network Protocol (CLNP) and the	*/
/*		Ethernet sub-network.					*/
/*									*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
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
/*			clnp_snet_create_es_table			*/
/*			clnp_snet_check_mac				*/
/*			clnp_snet_frame_to_udt				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 06/14/10  JRB	   Del obsolete clnp_snet_read_hook_*.		*/
/* 02/05/06  JRB    13     Use new clnp_snet_write_raw.			*/
/* 02/08/05  JRB    12     Add clnp_snet_frame_to_udt.			*/
/* 01/10/05  JRB    11     Add clnp_snet_read_hook_*.			*/
/* 04/27/00  MDE    10     Lint cleanup					*/
/* 09/13/99  MDE    09     Added SD_CONST modifiers			*/
/* 12/02/98  JRB    08     Moved DOS code to clnp_dos.c. This module	*/
/*			   now shared by all Ethernet implementations.	*/
/*			   Added clnp_snet_create_es_table.		*/
/*			   Added clnp_snet_check_mac.			*/
/* 10/25/97  EJV    07     Added clnp_snet_ext_write for UCA_SMP.	*/
/* 05/28/97  EJV    06     Deleted absolete clnp_err.h include.		*/
/* 05/27/97  JRB   7.00    MMSEASE 7.0 release.				*/
/* 03/03/97  EJV    05     Changed __far to far (for MS Compiler v6.00)	*/
/* 11/13/96  JRB    04     Add "sysincs.h".				*/
/* 09/06/96  JRB    03     Fix len calculation in "read" & "write" func.*/
/* 07/22/96  EJV    02     Added func clnp_snet_get_type		*/
/* 07/19/96  EJV    01     Created					*/
/************************************************************************/
#include "glbtypes.h"
#include "sysincs.h"

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;  /* Define for SLOG mechanism    */
#endif

#include "clnpport.h"
#include "clnp_usr.h"
#include "clnp_sne.h"
#include "clnp_llc.h"
#include "clnp_log.h"
#include "ethertyp.h"

/*----------------------------------------------------------------------*/
/* Global variables.							*/
/*----------------------------------------------------------------------*/

/* All ES and all IS addresses */
ST_UCHAR ETHE_ALL_ES [ETHE_MAC_LEN] = {0x09,0x00,0x2B,0x00,0x00,0x04};
ST_UCHAR ETHE_ALL_IS [ETHE_MAC_LEN] = {0x09,0x00,0x2B,0x00,0x00,0x05};


/*----------------------------------------------------------------------*/
/* defines and variables local to this module				*/
/*----------------------------------------------------------------------*/

/* Variables below are used to store the Intermediate System (IS)	*/
/* address to which packets will be directed if remote MAC address of	*/
/* End System is unknown.  Current implementation allows for only 1 IS	*/
/* address to be stored.						*/

static ST_UCHAR  is_mac [ETHE_MAC_LEN];		/* IS System MAC	*/
static ST_UCHAR  is_net [CLNP_MAX_LEN_NSAP + 1];	/* IS Network Entity Title	*/
						/* (table of one address)	*/
static ST_UINT16 is_holding_time = 0; 		/* If 0, IS addr invalid	*/


#if defined(UCA_SMP)
/************************************************************************/
/*			clnp_snet_ext_write				*/
/*----------------------------------------------------------------------*/
/* This function will send a LPDU to an Ethernet sub-network.		*/
/* Created for compatibility with ADLC sub-network.			*/
/*									*/
/* Parameters:								*/
/*	SN_UNITDATA *sn_req	Pointer to Sub-network Unit Data request*/
/*				to be sent.				*/
/*	ST_LONG      user_id	User id for this write request		*/
/*									*/
/* Return:								*/
/*	SD_SUCCESS (0)		if write successful			*/
/*	error code		otherwise				*/
/************************************************************************/
ST_RET clnp_snet_ext_write (SN_UNITDATA *sn_req, ST_LONG user_id)
{
ST_RET  ret;
ST_UCHAR raw_packet [ETHE_MAX_LEN_LSDU];	/* raw packet buffer	*/

  /* user_id arg ignored.	*/
  memcpy (raw_packet, sn_req->rem_mac, CLNP_MAX_LEN_MAC);/* DST MAC*/
  memcpy (&raw_packet[6], sn_req->loc_mac, CLNP_MAX_LEN_MAC);/* SRC MAC*/
  /* Set the 802.3 length/type field (bytes 12 and 13).	*/
  raw_packet[12] = (ST_UCHAR) (sn_req->lpdu_len >> 8);	
  raw_packet[13] = (ST_UCHAR) (sn_req->lpdu_len & 0xff);
  memcpy (&raw_packet[14], sn_req->lpdu, sn_req->lpdu_len);

  ret = clnp_snet_write_raw (raw_packet, sn_req->lpdu_len+14);

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
    memcpy (is_mac, rem_mac, ETHE_MAC_LEN);

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
/*	ST_UCHAR  *rem_mac		Pointer to remote MAC address		*/
/*	ST_UINT16 holding_time	Holding Time for the new address	*/
/*									*/
/* Return:								*/
/*	SD_SUCCESS (0)		if update successful			*/
/*	error code		otherwise				*/
/************************************************************************/
ST_RET clnp_snet_update_es (ST_UCHAR *rem_nsap, ST_UCHAR *rem_mac, 
			      ST_UINT16 holding_time)
{
ST_RET ret;

  ret = SD_SUCCESS;

  /* not implemented */

  return (ret);
}


/************************************************************************/
/*			clnp_snet_lookup_is				*/
/*----------------------------------------------------------------------*/
/* This function will lookup the remote MAC address in the NET-MAC	*/
/* table (currently only one IS address is stored.			*/
/*									*/
/* Parameters:								*/
/*	ST_UCHAR  *rem_mac		Pointer to buffer where to copy the	*/
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
    memcpy (rem_mac, is_mac, ETHE_MAC_LEN);
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
/*	ST_UCHAR  *rem_mac		Pointer to buffer where to copy the	*/
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

  ret = SNET_ERR_MAC_INVALID;

  /* here goes code to lookup the remote NSAP and copy the remote MAC	*/
  /* address to remote_mac buffer.					*/
  /* not implemented */

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

  memcpy (mac_buf, ETHE_ALL_IS, ETHE_MAC_LEN);

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

  memcpy (mac_buf, ETHE_ALL_ES, ETHE_MAC_LEN);

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
ST_CHAR all_zero_mac [6] = {0, 0, 0, 0, 0, 0};

  ret = SD_SUCCESS;
  if (memcmp (clnp_param.loc_mac, all_zero_mac, 6) == 0)
    {
    CLNP_LOG_ERR0 ("Subnetwork not initialized");
    return (SD_FAILURE);
    }    

  /* the MAC addr is read to clnp_param during sub-net initialization	*/
  memcpy (mac_buf, clnp_param.loc_mac, ETHE_MAC_LEN);

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

  max_udata_len = ETHE_MAX_LEN_UDATA;

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
/*	ST_INT			SUBNET_ETHE.				*/
/************************************************************************/
ST_INT clnp_snet_get_type (ST_VOID)
{
  return (SUBNET_ETHE);
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

  /* here goes code to allocate max_count static routing ES_TABLE	*/
  /* not implemented */

  ret = SNET_ERR_INIT;

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
/*	ST_INT			CLNP_MAC_LOCAL or			*/
/*				CLNP_MAC_ALL_ES or			*/
/*				CLNP_MAC_GOOSE or			*/
/*				CLNP_MAC_INVALID			*/
/************************************************************************/
ST_INT clnp_snet_check_mac (ST_UCHAR *mac_addr)
{
ST_INT mac_flag;

  if (memcmp (mac_addr, clnp_param.loc_mac, CLNP_MAX_LEN_MAC) == 0)
    mac_flag = CLNP_MAC_LOCAL;		/* our MAC address	*/
  else if (memcmp (mac_addr, ETHE_ALL_ES, CLNP_MAX_LEN_MAC) == 0)
    mac_flag = CLNP_MAC_ALL_ES;		/* ALL-ES multicast MAC	*/
  else if (memcmp (mac_addr, ETHE_ALL_IS, CLNP_MAX_LEN_MAC) == 0)
    mac_flag = CLNP_MAC_INVALID;	/* ALL-IS multicast MAC	*/
					/* Want to ignore these so set	*/
					/* flag to INVALID.		*/
  else if (mac_addr [0] & 0x01)	/* For ethernet, this bit means multicast.*/
    mac_flag = CLNP_MAC_GOOSE;		/* Other multicast (i.e. GOOSE)	*/
  else
    mac_flag = CLNP_MAC_INVALID;

  return (mac_flag);
}

/************************************************************************/
/*			clnp_snet_frame_to_udt				*/
/* Copy data from raw frame to SN_UNITDATA struct.			*/
/* CRITICAL: caller must init "sn_req->lpdu" to point to an allocated	*/
/*           buffer before calling this function.			*/
/* Parameters:								*/
/*	ST_UINT8    *frame_buf	Pointer to raw frame.			*/
/*	ST_INT       frame_len	Length of raw frame.			*/
/*	SN_UNITDATA *sn_req	Frame info as Subnet API needs it.	*/
/*	ST_INT  udata_max_len	Max len of user data to be stored	*/
/*				in sn_req->lpdu. Caller should pass	*/
/*				the size of the allocated lpdu buffer.	*/
/*									*/
/* Return:								*/
/*	SD_SUCCESS (0)		if successful				*/
/*	error code		otherwise				*/
/************************************************************************/
ST_RET clnp_snet_frame_to_udt (ST_UINT8 *frame_buf,
			ST_INT frame_len,
			SN_UNITDATA *sn_req,
			ST_INT udata_max_len)
  {
ST_UINT16 lpdu_len_type;	/* Len/Type field from frame	*/
ST_UINT16 data_len = 0;	/* num of data bytes in frame	 	*/
			/* Initialize to invalid value of 0	*/
ST_UINT16 field0;	/* first 2 byte field after MACs (converted to short)*/
ST_INT dstart_idx;	/* index to start of data in frame_buf	*/
ST_RET retcode = SD_FAILURE;

  assert (sn_req->lpdu);	/* Make sure caller set this	*/

  /* Get first field after DST/SRC MACs. Normally it is Length/Type field*/
  /* but it could be beginning of QTag Prefix (IEEE 802.1Q).		*/
  field0 = (((ST_UINT16) frame_buf[2*ETHE_MAC_LEN] << 8) |
             (ST_UINT16) frame_buf[2*ETHE_MAC_LEN+1]);
  
  /* If field0 is beginning of QTAG Prefix (IEEE 802.1Q), handle it.	*/
  if(field0 == ETYPE_VLAN_TYPE_ID)
    {	/* frame contains QTAG Prefix. IGNORE IT.	*/
    /* Decode the "MAC Client Length/Type" field immediately AFTER QTag Prefix.	*/
    lpdu_len_type = (((ST_UINT16) frame_buf[2*ETHE_MAC_LEN+ETHE_LEN_QTAG_PREFIX] << 8) |
                      (ST_UINT16) frame_buf[2*ETHE_MAC_LEN+ETHE_LEN_QTAG_PREFIX+1]);
    /* Data starts after QTag Prefix and "MAC Client Length/Type" field	*/
    dstart_idx = (2*ETHE_MAC_LEN) + ETHE_LEN_QTAG_PREFIX + ETHE_LEN_LENTYPE;
    }
  else
    {
    lpdu_len_type = field0;	/* field0 is the "Length/Type" field.*/
    /* Data starts after "MAC Client Length/Type" field	*/
    dstart_idx = (2*ETHE_MAC_LEN) + ETHE_LEN_LENTYPE;
    }

  /* Check "MAC Client Length/Type" field.
   * If >= 0x0600, it indicates "Type".
   * If <  0x0600, it indicates "Length".
   */
  if (lpdu_len_type >= 0x0600)
    {	/* "type" interpretation	*/
    /* Make sure it is protocol we recognize.	*/
    if (lpdu_len_type == ETYPE_TYPE_GOOSE ||		/* GOOSE	*/
	lpdu_len_type == ETYPE_TYPE_GSE ||		/* GSE Management	*/
	lpdu_len_type == ETYPE_TYPE_SV)			/* Sampled Values	*/
      {
      /* For all protocols we recognize, data len is in byte 2 & 3 of data.*/
      /* len includes bytes starting with APPID.			*/
      data_len = (((ST_UINT16) frame_buf[dstart_idx+2] << 8) |
                   (ST_UINT16) frame_buf[dstart_idx+3]);
      }
    }
  else
    {	/* "length" interpretation		*/
    data_len = lpdu_len_type;	/* num of data bytes to copy	*/
    }

  /* data_len was initialized to invalid value of 0.
   * If NOW it is != 0, frame was recognized.
   */
  if (data_len > 0)
    {	/* frame recognized	*/
    if (data_len <= udata_max_len)	/* make sure there's room to copy data	*/
      {
      ST_INT expected_len = data_len + dstart_idx;

      if (expected_len > frame_len)
        CLNP_LOG_ERR2 ("Partial Subnetwork frame received: expected length (%d), received length (%d). Frame ignored.",
                       expected_len, frame_len);
      else
        {	/* everything is GOOD	*/
        /* Fill in sn_req	*/
        /* NOTE: sn_req->lpdu buffer must be allocated before this funct called.*/
        memcpy (sn_req->loc_mac, &frame_buf[0],            ETHE_MAC_LEN);	/* copy DST MAC	*/
        memcpy (sn_req->rem_mac, &frame_buf[ETHE_MAC_LEN], ETHE_MAC_LEN);	/* copy SRC MAC	*/
        memcpy (sn_req->lpdu, &frame_buf[dstart_idx], data_len);	/* copy data from frame*/
        sn_req->lpdu_len = lpdu_len_type;	/* NOTE: this may contain "len" or "type"*/
        retcode = SD_SUCCESS;	/* this is only path to success!!!	*/
        }
      }
    else
      CLNP_LOG_ERR1 ("Subnetwork frame received length (%d) exceeds maximum. Frame ignored.", data_len);
    }
  else
    CLNP_LOG_IND1 ("Unrecognized Subnetwork frame received (Len/Type=0x%04X). Frame Ignored.",
                   lpdu_len_type);
  return (retcode);
  }


