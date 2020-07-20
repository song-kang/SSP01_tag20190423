/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1996, All Rights Reserved.		        	*/
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : clnp_hc.c						*/
/* PRODUCT(S)  : Lean-T Stack						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*		This module implements the hard coded initialization	*/
/*		of CLNP parameters (which can be used in absence of	*/
/*		configuration file).					*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*			clnp_config					*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 02/22/05  JRB    04     Del unused "thisFileName".			*/
/* 04/27/00  MDE    03     Lint cleanup					*/
/* 03/07/00  JRB    02     Only set loc_mac if compiled for ADLC.	*/
/* 09/13/99  MDE    01     Added SD_CONST modifiers			*/
/* 05/27/97  JRB   7.00    MMSEASE 7.0 release.				*/
/* 03/03/97  EJV    03     Removed CLNP_LOG_ALWAYS			*/
/* 11/13/96  JRB    02     Add "sysincs.h".				*/
/* 07/18/96  EJV    01     Created					*/
/************************************************************************/
#include "glbtypes.h"
#include "sysincs.h"
#include "clnp_usr.h"


/************************************************************************/
/*			clnp_config					*/
/*----------------------------------------------------------------------*/
/* This function will initialize the CLNP parameters.			*/
/*									*/
/* Parameters:								*/
/*	ST_VOID			none					*/
/*									*/
/* Return:								*/
/*	SD_SUCCESS (0)		if initialization successful		*/
/*	error code		otherwise				*/
/************************************************************************/
ST_RET clnp_config (ST_VOID)
{
  /* Default logging mask, ignored if not compiled with DEBUG_SISCO	*/
  clnp_debug_sel = 	CLNP_LOG_ERR |
  			CLNP_LOG_NERR;

  /* additional log options (add to clnp_debug_sel)*/
  /*			CLNP_LOG_REQ |		*/
  /*			CLNP_LOG_IND |		*/
  /*			CLNP_LOG_ENC_DEC |	*/
  /*			CLNP_LOG_LLC_ENC_DEC |	*/
  /*			CLSNS_LOG_REQ |		*/
  /*			CLSNS_LOG_IND |		*/

  /* TPDU lifetime (in 500 ms units), range from CLNP_MIN_PDU_LIFETIME	*/
  /* to CLNP_MAX_PDU_LIFETIME						*/
  clnp_param.pdu_lifetime = CLNP_DEF_PDU_LIFETIME;

  /* PDU lifetime decrement (in 500msec units)	*/
  clnp_param.pdu_lifetime_dec = CLNP_DEF_PDU_LIFETIME_DEC;

  /* Configuration Timer in seconds (how often ESH will be sent) */
  clnp_param.esh_cfg_timer = CLNP_DEF_ESH_CFG_TIMER;

  /* Delay time in seconds before first ESH is sent */
  clnp_param.esh_delay = CLNP_DEF_ESH_DELAY;

  /* Local MAC address							*/
  /* For the ADLC sub-network the MAC address has to match the address	*/
  /* in adlc.cfg !							*/
  /* For the Ethernet this param will be read from the driver during	*/
  /* initialization.							*/

#if (defined(ADLC_MASTER) || defined(ADLC_SLAVE))
  clnp_param.loc_mac [0] = 0x30;
  clnp_param.loc_mac [1] = 0x00;
#endif

  /* local NSAP (length and address) */
  clnp_param.loc_nsap[0] = (ST_UCHAR) 4;
  clnp_param.loc_nsap[1] = 0x49;
  clnp_param.loc_nsap[2] = 0x00;
  clnp_param.loc_nsap[3] = 0x11;
  clnp_param.loc_nsap[4] = 0x11;

  return (SD_SUCCESS);
}




