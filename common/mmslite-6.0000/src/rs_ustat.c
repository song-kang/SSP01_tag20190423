/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rs_ustat.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the functions to decode an Unsolicited	*/
/*	status request. 						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 03/11/04  GLB     03    Remove "thisFileName"                    	*/
/* 12/20/01  JRB     02    Converted to use ASN1R (re-entrant ASN1)	*/
/*			   Deleted mms_status proto (see mms_dfun.h)	*/
/* 09/13/99  MDE     01    Added SD_CONST modifiers			*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"

#include "mmsdefs.h"
#include "asn1defs.h"

/************************************************************************/
/************************************************************************/
/*			   mms_ustatus_req				*/
/* This function is called from mmsdec to decode an UnsolicitedStatus	*/
/* request PDU after the PDU type has been determined.	It simply calls */
/* mms_status in the module mmsstat.c.					*/
/************************************************************************/

ST_VOID mms_ustatus_req (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("decode unsolicited status reques");
  mms_status (aCtx);
  }
