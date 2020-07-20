/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1993 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : mmscancl.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the common function to encode a conclude	*/
/*	operation.  Note that the cancel request and the cancel 	*/
/*	response are the same.						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 11/23/11  KCR           Added casting for proper 64bit compile       */
/* 12/20/01  JRB     02    Converted to use ASN1R (re-entrant ASN1)	*/
/* 09/13/99  MDE     01    Added SD_CONST modifiers			*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"

#include "mmsdefs.h"
#include "asn1defs.h"

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/
/*			_ms_mk_cancel					*/
/* Construct a cancel response or request PDU.				*/
/************************************************************************/

ST_UCHAR *_ms_mk_cancel (ASN1_ENC_CTXT *aCtx, ST_BOOLEAN request, ST_UCHAR *msg_ptr, ST_INT buf_len, ST_UINT32 id)
  {
ST_UCHAR	*msg_start;			
#ifdef DEBUG_SISCO
ST_INT	msg_len;			/* Used for debug print only 	*/
#endif

  asn1r_strt_asn1_bld (aCtx, msg_ptr,buf_len);	/* init the ASN.1 tools 	*/

  asn1r_wr_u32 (aCtx, id);				/* write the invoke id		*/

/* Finish the PDU with the correct context tag. 			*/
  if (request)
    asn1r_fin_prim (aCtx, MMSCANREQ,CTX);
  else
    asn1r_fin_prim (aCtx, MMSCANRESP,CTX);

  msg_start = aCtx->asn1r_field_ptr + 1;
#ifdef DEBUG_SISCO
  msg_len = (int) ((msg_ptr + buf_len) - msg_start);
  MLOG_ENC1 ("CANCEL response built, len = %d", msg_len);
  MLOG_ENCH (msg_len,msg_start);
  MLOG_PAUSEENC (NULL);
#endif

  return (msg_start);
  }
