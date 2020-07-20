/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rq_getvar.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the requester portion of the get variable	*/
/*	access attributes.  It decodes the get variable access		*/
/*	attributes response (confirm) & encodes the get variable access */
/*	attributes request.						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 03/11/04  GLB     06    Remove "thisFileName"                    	*/
/* 12/20/01  JRB     05    Converted to use ASN1R (re-entrant ASN1)	*/
/* 09/13/99  MDE     04    Added SD_CONST modifiers			*/
/* 03/23/99  MDE     03    Changes to decode buffer allocation scheme	*/
/* 06/15/98  MDE     02    Changes to allow compile under C++		*/
/* 07/03/97  MDE     01    Op-specific info buffer handling changes	*/
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

static  GETVAR_RESP_INFO	*rsp_info;


/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

/* static functions with multiple references                            */

static ST_VOID getvar_typespec_cstr (ASN1_DEC_CTXT *aCtx);
static ST_VOID getvar_typespec_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID getvar_address_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID getvar_address_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID getvar_address_cstr (ASN1_DEC_CTXT *aCtx);
static ST_VOID getvar_get_mms_deletable (ASN1_DEC_CTXT *aCtx);
static ST_VOID getvar_typespec_cstr_done (ASN1_DEC_CTXT *aCtx);


/************************************************************************/
/************************************************************************/
/*			mms_get_var_rsp 				*/
/* This function is called from MMSDEC when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_get_var_rsp (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("Get Variable Access Attributes Response");

/* get storage for request information	*/
  rsp_info = (GETVAR_RESP_INFO *) _m_get_dec_buf (aCtx, sizeof (GETVAR_RESP_INFO));
  rsp_info->address_pres = SD_FALSE;		/* set default value	*/

  ASN1R_TAG_ADD (aCtx, CTX, 0, getvar_get_mms_deletable);
  }

/************************************************************************/
/*			getvar_get_mms_deletable			*/
/* Decoding get variable access attributes response: mms deletable was	*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID getvar_get_mms_deletable (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("getvar_get_mms_deletable");
					/* read mms deletable		*/
  if (asn1r_get_bool (aCtx, &(rsp_info->mms_deletable)))
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);

  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 1, getvar_address_cstr);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 2, getvar_typespec_cstr);
  }

/************************************************************************/
/*			getvar_address_cstr				*/
/* get variable access attributes request: address constructor was	*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID getvar_address_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("getvar_address_cstr");

  rsp_info->address_pres = SD_TRUE;		/* set address present	*/

  _ms_get_va_address (aCtx, &rsp_info->address, getvar_address_done);
  }

/************************************************************************/
/*			getvar_address_done				*/
/* get variable access attributes request: address has been decoded	*/
/************************************************************************/

static ST_VOID getvar_address_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("getvar_address_done");

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = getvar_address_cstr_done;
  }

/************************************************************************/
/*			getvar_address_cstr_done			*/
/* get variable access attributes request: address constructor done was */
/* encountered. 							*/
/************************************************************************/

static ST_VOID getvar_address_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 2, getvar_typespec_cstr);
  }

/************************************************************************/
/*			getvar_typespec_cstr				*/
/* get variable access attributes request: type spec constructor was	*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID getvar_typespec_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("getvar_typespec_cstr");

  _ms_get_va_type_spec (aCtx, &rsp_info->type_spec, getvar_typespec_done);
  }

/************************************************************************/
/*			getvar_typespec_done				*/
/* get variable access attributes request: type spec has been decoded	*/
/************************************************************************/

static ST_VOID getvar_typespec_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("getvar_typespec_done");

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = getvar_typespec_cstr_done;
  }

/************************************************************************/
/*			getvar_typespec_cstr_done			*/
/* get variable access attributes request: typespec constructor done was */
/* encountered. 							*/
/************************************************************************/

static ST_VOID getvar_typespec_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_getvar					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE & send the get variable access attributes request. */
/************************************************************************/

MMSREQ_PEND *mp_getvar (ST_INT chan, GETVAR_REQ_INFO *info_ptr)
  {
  return (_mms_req_send (MMSOP_GET_VAR,
			M_CAST_MK_FUN (_ms_mk_getvar_req),
			chan,(ST_CHAR *)info_ptr));
  } 

#else
/************************************************************************/
/************************************************************************/
/*			mpl_getvar					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the get variable access attributes request. 	*/
/************************************************************************/

ST_RET  mpl_getvar (GETVAR_REQ_INFO *info_ptr)
  {
  return (_mms_req_fin (MMSOP_GET_VAR,
			M_CAST_MK_FUN (_ms_mk_getvar_req),
			(ST_CHAR *)info_ptr));
  } 
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_getvar_req					*/
/* construct a get variable access attributes request			*/
/************************************************************************/

ST_VOID _ms_mk_getvar_req (ASN1_ENC_CTXT *aCtx, GETVAR_REQ_INFO *info)
  {
  if (info->req_tag == 1)
    {
    asn1r_strt_constr (aCtx);			/* start context tag 1 cstr	*/
    _ms_wr_va_address (aCtx, &info->address);	/* write the address		*/
    asn1r_fin_constr (aCtx, 1, CTX, DEF);		/* finish context tag 1 cstr	*/
    }
  else
    {
    asn1r_strt_constr (aCtx);			/* start context tag 0 cstr	*/
    _ms_wr_mms_objname (aCtx, &info->name);	/* write the object name	*/
    asn1r_fin_constr (aCtx, 0, CTX, DEF);		/* finish context tag 0 cstr	*/
    }

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_constr (aCtx, MMSOP_GET_VAR,CTX,DEF); 	/* tag = opcode, ctx		*/
  }
