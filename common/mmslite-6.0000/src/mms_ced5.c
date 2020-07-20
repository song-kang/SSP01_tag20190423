/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : mms_ced5.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains common primitive encode/decode functions	*/
/*	particular application references.				*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 03/11/04  GLB     03    Remove "thisFileName"                    	*/
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

static ST_VOID app_ref_get_seq (ASN1_DEC_CTXT *aCtx);

/************************************************************************/
/*			global structures & functions			*/
/************************************************************************/

static ST_VOID (*get_mms_app_ref_done) (ASN1_DEC_CTXT *aCtx);

/************************************************************************/
/************************************************************************/
/*			_ms_get_mms_app_ref 				*/
/* Function to get an Application Reference from the received message.	*/
/* Takes two destination element pointers because of op-specific struct */
/* differences. 							*/
/************************************************************************/

ST_VOID _ms_get_mms_app_ref (ASN1_DEC_CTXT *aCtx, ST_UCHAR **ptr_ptr, ST_INT *len_ptr,
			 ST_VOID (*done_fun) (ASN1_DEC_CTXT *aCtx))
  {

  get_mms_app_ref_done = done_fun;

  *ptr_ptr = aCtx->asn1r_field_ptr; 			/* save pointer 	*/
  *len_ptr = aCtx->asn1r_elmnt_len; 			/* save length of data	*/

  ASN1R_TAG_ADD (aCtx, UNI|CONSTR, SEQ_CODE, app_ref_get_seq);
  }

/************************************************************************/
/*			app_ref_get_seq					*/
/* Sequence was encountered, contents are 4 optional ASN.1 ANY's.	*/
/************************************************************************/

static ST_VOID app_ref_get_seq (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("app_ref_get_seq");
  if (aCtx->asn1r_elmnt_len)		/* if any contents, parse them		*/
    asn1r_parse_cstr_contents (aCtx, get_mms_app_ref_done);
  else				/* no contents, call user done function	*/
    aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = get_mms_app_ref_done;
  }

