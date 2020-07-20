/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rs_jcreate.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the responder portion of create journal.	*/
/*	It decodes the create journal request (indication) & encodes	*/
/*	the create journal response.					*/
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
#include "mms_pjou.h"
#include "asn1defs.h"

/************************************************************************/
/* variables global to the DECODE portion :				*/

static  JCREATE_REQ_INFO	*req_info;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

static ST_VOID jcreate_jname_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID jcreate_jname_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID jcreate_jname_cstr  (ASN1_DEC_CTXT *aCtx);


/************************************************************************/
/************************************************************************/
/*			mms_jcreate_req					*/
/* This function is called from MMSDEC when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_jcreate_req (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("Create Journal Request");


/* get storage for request information	*/
  req_info = (JCREATE_REQ_INFO *) _m_get_dec_buf (aCtx, sizeof (JCREATE_REQ_INFO));
  aCtx->asn1r_err_fun  = _mms_dec_buf_free;
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 0, jcreate_jname_cstr);
  }

/************************************************************************/
/*			jcreate_jname_cstr				*/
/* create journal request is being decoded: journal name constructor	*/
/* was encountered.							*/
/************************************************************************/

static ST_VOID jcreate_jname_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("jcreate_jname_cstr");

  _ms_get_mms_objname (aCtx, &req_info->jou_name, jcreate_jname_done);
  }

/************************************************************************/
/*			jcreate_jname_done				*/
/* create journal request is being decoded: journal name has been	*/
/* obtained.								*/
/************************************************************************/

static ST_VOID jcreate_jname_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("jcreate_jname_done");

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = jcreate_jname_cstr_done;
  }

/************************************************************************/
/*			jcreate_jname_cstr_done				*/
/* create journal request is being decoded: journal name cstr done was	*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID jcreate_jname_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_jcreate_resp					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE and send the create journal response.		*/
/************************************************************************/

ST_RET mp_jcreate_resp (MMSREQ_IND *indptr)
  {
  return (_mms_send_null_resp (indptr,MMSOP_CREATE_JOURNAL));
  }

#else
/************************************************************************/
/************************************************************************/
/*			mpl_jcreate_resp  				*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the create journal response.			*/
/************************************************************************/

ST_RET mpl_jcreate_resp (ST_UINT32 invoke_id)
  {
  return (_mms_fin_null_resp (invoke_id,MMSOP_CREATE_JOURNAL));
  }
#endif
