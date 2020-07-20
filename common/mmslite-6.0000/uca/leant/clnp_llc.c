/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1996, All Rights Reserved.		        	*/
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : clnp_llc.c						*/
/* PRODUCT(S)  : Lean-T Stack						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*		This moduleimplements the encoding and decoding of	*/
/*		Logical Link Control (LLC) which is implemented with	*/
/*		the CLNP layer.						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*			clnpl_llc_encode 				*/
/*			clnpl_llc_decode 				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 02/08/05  JRB    08     clnpl_llc_decode: don't check LSAP, caller	*/
/*			   should check LSAP.				*/
/* 04/27/00  MDE    07     Lint cleanup					*/
/* 09/13/99  MDE    06     Added SD_CONST modifiers			*/
/* 08/13/98  JRB    05     Lint cleanup.				*/
/* 09/23/97  EJV    04     Added support for UCA_SMP.			*/
/*			   Corrected logs.				*/
/* 05/28/97  EJV    03     Deleted absolete clnp_err.h include.		*/
/* 05/27/97  JRB   7.00    MMSEASE 7.0 release.				*/
/* 11/13/96  JRB    02     Add "sysincs.h".				*/
/* 06/04/96  EJV    01     Created					*/
/************************************************************************/
#include "glbtypes.h"
#include "sysincs.h"

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;  /* Define for SLOG mechanism    */
#endif

#include "clnp_usr.h"
#include "clnp_log.h"
#include "clnp_llc.h"


/************************************************************************/
/*			clnpl_llc_encode 				*/
/*----------------------------------------------------------------------*/
/* This function is used to encode the LLC layer header in LPDU.	*/
/*									*/
/* Parameters:								*/
/*	ST_UCHAR  *enc_buf 	Pointer to buffer where to encode the	*/
/*				LLC header.				*/
/*	ST_UCHAR  src_lsap 	Source LSAP to encode			*/
/*	ST_UCHAR  dest_lsap 	Destination LSAP to encode		*/
/*									*/
/* Return:								*/
/*	SD_SUCCESS (0)		if encoding successful			*/
/*	error code		otherwise				*/
/************************************************************************/
ST_RET clnpl_llc_encode (ST_UCHAR *enc_buf,
			 ST_UCHAR src_lsap, ST_UCHAR dest_lsap)
{
ST_RET	 ret;
LLC_HDR	*hdr;

  ret = SD_SUCCESS;			/* assume successful encoding	*/

  if (enc_buf != NULL)
    {
    hdr = (LLC_HDR *) enc_buf;
    hdr->src = src_lsap;
    hdr->dest = dest_lsap;
    hdr->control = LLC_CONTROL;

    /* log the LLC encoding only */
    CLNP_LOG_LLC_ENC_DEC3 ("LLC encode:\n  Source:  0x%2.2X\n  Dest:    0x%2.2X\n  Control: 0x%2.2X",
      (ST_UINT) hdr->src, (ST_UINT) hdr->dest, (ST_UINT) hdr->control);
    }
  else
    {
    ret = CLNP_ERR_NULL_PTR;
    CLNP_LOG_ERR0 ("CLNP-ERROR: LLC encode failed (enc_buf=NULL)");
    }

  return (ret);
}


/************************************************************************/
/*			clnpl_llc_decode				*/
/*----------------------------------------------------------------------*/
/* This function is used to decode the LLC layer header from a LPDU.	*/
/*									*/
/* Parameters:								*/
/*	ST_UCHAR  *dec_buf	Pointer to LPDU buffer			*/
/*	ST_UCHAR  *dest_lsap	Pointer to dest LSAP to be returned	*/
/*									*/
/* Return:								*/
/*	SD_SUCCESS (0)		if decoding successful			*/
/*	error code		otherwise				*/
/************************************************************************/
ST_RET clnpl_llc_decode (ST_UCHAR *dec_buf, ST_UCHAR *dest_lsap)
{
ST_RET ret;
LLC_HDR	*hdr = (LLC_HDR *) dec_buf;

  ret = SD_SUCCESS;			/* assume successful decoding	*/

  /* just check if all parameters are OK (supported) */

  if (hdr != NULL)
    {
    *dest_lsap = hdr->dest;		/* return the dest LSAP */
#if 0	/* DEBUG: add this code if caller needs src_lsap and control	*/
    *src_lsap = hdr->src;		/* we always expect src_lsap=dst_lsap	*/
    *llc_control = hdr->control;	/* we always expect LLC_CONTROL (0x03)	*/
#endif

    /* log the LLC decoding only */
    CLNP_LOG_LLC_ENC_DEC3 ("LLC decode:\n  Source:  0x%2.2X\n  Dest:    0x%2.2X\n  Control: 0x%2.2X",
      (ST_UINT) hdr->src, (ST_UINT) hdr->dest, (ST_UINT) hdr->control);
    }
  else
    {
    ret = CLNP_ERR_NULL_PTR;
    CLNP_LOG_ERR0 ("CLNP-ERROR: LLC decode failed (dec_buf=NULL)");
    }

  return (ret);
}





