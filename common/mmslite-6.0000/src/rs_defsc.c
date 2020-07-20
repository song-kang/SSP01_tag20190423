/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rs_defscat.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the responder portion of define scattered	*/
/*	access.  It decodes the define scattered access request 	*/
/*	(indication) and encodes the define scattered access response.	*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 03/11/04  GLB     05    Remove "thisFileName"                    	*/
/* 12/20/01  JRB     04    Converted to use ASN1R (re-entrant ASN1)	*/
/* 09/13/99  MDE     03    Added SD_CONST modifiers			*/
/* 03/23/99  MDE     02    Changes to decode buffer allocation scheme	*/
/* 07/03/97  MDE     01    Op-specific info buffer handling changes	*/
/* 06/09/97  MDE     01    Changed op specific buffer allocation	*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"

#include "mmsdefs.h"
#include "mms_pvar.h"
#include "asn1defs.h"

/************************************************************************/
/* variables global to the DECODE portion :				*/

static  DEFSCAT_REQ_INFO	*req_info;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

static ST_VOID defscat_scat_acc_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID defscat_scat_acc_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID defscat_scat_acc_cstr (ASN1_DEC_CTXT *aCtx);
static ST_VOID defscat_name_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID defscat_objname_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID defscat_name_cstr (ASN1_DEC_CTXT *aCtx);


/************************************************************************/
/************************************************************************/
/*			mms_def_scat_req 				*/
/* This function is called from mmsdec when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_def_scat_req (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("Define Scattered Access Request");

/* get storage for request information	*/
  req_info = (DEFSCAT_REQ_INFO *) _m_get_dec_buf (aCtx, sizeof (DEFSCAT_REQ_INFO));
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 0, defscat_name_cstr);
  }

/************************************************************************/
/*			defscat_name_cstr				*/
/* define scattered access request: scattered access name constr was 	*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID defscat_name_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("defscat_name_cstr");

  _ms_get_mms_objname (aCtx, &req_info->sa_name, defscat_objname_done);
  }

/************************************************************************/
/*			defscat_objname_done				*/
/* define scattered access request: scattered access name has been	*/
/* decoded.								*/
/************************************************************************/

static ST_VOID defscat_objname_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("defscat_objname_done");

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = defscat_name_cstr_done;
  }

/************************************************************************/
/*			defscat_name_cstr_done				*/
/* define scattered access request: scattered access name constructor	*/
/* done was encountered.     						*/
/************************************************************************/

static ST_VOID defscat_name_cstr_done (ASN1_DEC_CTXT *aCtx)
  {

  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 1, defscat_scat_acc_cstr);
  }

/************************************************************************/
/*			defscat_address_cstr				*/
/* define scattered access request: scattered access constructor was	*/
/* encountered.								*/
/************************************************************************/

static ST_VOID defscat_scat_acc_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("defscat_scat_acc_cstr");

  _ms_get_va_scat_access (aCtx, &req_info->sa_descr, defscat_scat_acc_done);
  }

/************************************************************************/
/*			defscat_scat_acc_done				*/
/* define scattered access request: scattered access has been decoded.	*/
/************************************************************************/

static ST_VOID defscat_scat_acc_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("defscat_scat_acc_done");

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = defscat_scat_acc_cstr_done;
  }

/************************************************************************/
/*			defscat_scat_acc_cstr_done			*/
/* define scattered access request: scattered access constr done was	*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID defscat_scat_acc_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_defscat_resp					*/
/* Create and send a define scattered access response.			*/
/************************************************************************/

ST_RET mp_defscat_resp (MMSREQ_IND *indptr)
  {
  return (_mms_send_null_resp (indptr,MMSOP_DEF_SCAT));
  }

#else
/************************************************************************/
/************************************************************************/
/*			mpl_defscat_resp				*/
/* Create a define scattered access response.				*/
/************************************************************************/

ST_RET  mpl_defscat_resp (ST_UINT32 invoke_id)
  {
  return (_mms_fin_null_resp (invoke_id,MMSOP_DEF_SCAT));
  }
#endif
