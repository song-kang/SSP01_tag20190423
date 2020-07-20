/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : mmsconcl.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the common function to encode a conclude	*/
/*	operation.  Note that the conclude request and the conclude	*/
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
/************************************************************************/
/*			_ms_mk_conclude					*/
/* Function to construct a conclude message in selected buffer. 	*/
/* This may be used for either request or response by selecting the	*/
/* PDU type desired. Pass a pointer to the end of the build buffer.	*/
/* Returns a pointer to the first byte of the decoded message.		*/
/************************************************************************/

ST_UCHAR *_ms_mk_conclude (ASN1_ENC_CTXT *aCtx, ST_UCHAR *buf_ptr, ST_INT buf_len, ST_INT pdutype)
  {
#ifdef DEBUG_SISCO
ST_UCHAR	*msg_start;			/* These variables used for	*/
ST_INT		msg_len;			/* debug print only		*/
#endif

  asn1r_strt_asn1_bld (aCtx, buf_ptr,buf_len);	/* init the ASN.1 tools 	*/

/* The only data element is a context-tagged NULL.			*/

#ifdef DEBUG_SISCO
  asn1r_fin_prim (aCtx, (ST_UINT16) pdutype, CTX);
  msg_start = aCtx->asn1r_field_ptr + 1;
  msg_len = (int) ( (buf_ptr + buf_len) - msg_start);
  MLOG_ENC2 ("CONCLUDE %s built, len = %d",
             pdutype == MMSCNCLREQ ? "request" : "response",
             msg_len);
  MLOG_ENCH (msg_len,msg_start);
  MLOG_PAUSEENC (NULL);
  return (msg_start);
#else
  asn1r_fin_prim (aCtx, (ST_UINT16) pdutype,CTX);
  return (aCtx->asn1r_field_ptr + 1);
#endif
  }
