/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rs_defvar.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the responder portion of define named	*/
/*	variable.  It decodes the define named variable request 	*/
/*	(indication) and encodes the define named variable response.	*/
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

static  DEFVAR_REQ_INFO	*req_info;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

static ST_VOID defvar_typespec_cstr (ASN1_DEC_CTXT *aCtx);
static ST_VOID defvar_address_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID defvar_address_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID defvar_address_cstr (ASN1_DEC_CTXT *aCtx);
static ST_VOID defvar_name_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID defvar_objname_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID defvar_name_cstr (ASN1_DEC_CTXT *aCtx);
static ST_VOID defvar_typespec_done (ASN1_DEC_CTXT *aCtx);


/************************************************************************/
/************************************************************************/
/*			mms_def_var_req 				*/
/* This function is called from mmsdec when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_def_var_req (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("Define Named Variable Request");

/* get storage for request information	*/
  req_info = (DEFVAR_REQ_INFO *) _m_get_dec_buf (aCtx, sizeof (DEFVAR_REQ_INFO));
  req_info->type_spec_pres = SD_FALSE;		/* set default value	*/

  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 0, defvar_name_cstr);
  }

/************************************************************************/
/*			defvar_name_cstr				*/
/* define named variable request: variable name constructor was 	*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID defvar_name_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("defvar_name_cstr");

  _ms_get_mms_objname (aCtx, &req_info->name, defvar_objname_done);
  }

/************************************************************************/
/*			defvar_objname_done				*/
/* define named variable request: variable name has been decoded.	*/
/************************************************************************/

static ST_VOID defvar_objname_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("defvar_objname_done");

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = defvar_name_cstr_done;
  }

/************************************************************************/
/*			defvar_name_cstr_done				*/
/* define named variable request: variable name constructor done was	*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID defvar_name_cstr_done (ASN1_DEC_CTXT *aCtx)
  {

  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 1, defvar_address_cstr);
  }

/************************************************************************/
/*			defvar_address_cstr				*/
/* define named variable request: address constructor was encountered.	*/
/************************************************************************/

static ST_VOID defvar_address_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("defvar_address_cstr");

  _ms_get_va_address (aCtx, &req_info->address, defvar_address_done);
  }

/************************************************************************/
/*			defvar_address_done				*/
/* define named variable request: address has been decoded.		*/
/************************************************************************/

static ST_VOID defvar_address_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("defvar_address_done");

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = defvar_address_cstr_done;
  }

/************************************************************************/
/*			defvar_address_cstr_done			*/
/* define named variable request: address constructor done was		*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID defvar_address_cstr_done (ASN1_DEC_CTXT *aCtx)
  {

  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 2, defvar_typespec_cstr);
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }

/************************************************************************/
/*			defvar_typespec_cstr				*/
/* define named variable request: type spec constructor was encountered */
/************************************************************************/

static ST_VOID defvar_typespec_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("defvar_typespec_cstr");

  req_info->type_spec_pres = SD_TRUE;	/* set type specification pres	*/

  _ms_get_va_type_spec (aCtx, &req_info->type_spec, defvar_typespec_done);
  }

/************************************************************************/
/*			defvar_typespec_done				*/
/* define named type request: type specification has been decoded.	*/
/************************************************************************/

static ST_VOID defvar_typespec_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("defvar_typespec_done");
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_defvar_resp					*/
/* Create and send a define named variable response.			*/
/************************************************************************/

ST_RET mp_defvar_resp (MMSREQ_IND *indptr)
  {
  return (_mms_send_null_resp (indptr,MMSOP_DEF_VAR));
  }

#else
/************************************************************************/
/************************************************************************/
/*			mpl_defvar_resp					*/
/* Create a define named variable response.				*/
/************************************************************************/

ST_RET mpl_defvar_resp (ST_UINT32 invoke_id)
  {
  return (_mms_fin_null_resp (invoke_id,MMSOP_DEF_VAR));
  }
#endif
