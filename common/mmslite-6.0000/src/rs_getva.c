/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rs_getvar.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the responder portion of get variable	*/
/*	access attributes.  It decodes the get variable access		*/
/*	attributes request (indication) & encodes the get variable	*/
/*	access attributes response.					*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 03/11/04  GLB     07    Remove "thisFileName"                    	*/
/* 12/20/01  JRB     06    Converted to use ASN1R (re-entrant ASN1)	*/
/* 09/13/99  MDE     05    Added SD_CONST modifiers			*/
/* 03/23/99  MDE     04    Changes to decode buffer allocation scheme	*/
/* 06/15/98  MDE     03    Changes to allow compile under C++		*/
/* 07/03/97  MDE     02    Op-specific info buffer handling changes	*/
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

static  GETVAR_REQ_INFO	*req_info;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

static ST_VOID getvar_na_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID getvar_name_cstr (ASN1_DEC_CTXT *aCtx);
static ST_VOID getvar_addr_cstr (ASN1_DEC_CTXT *aCtx);


/************************************************************************/
/************************************************************************/
/*			mms_get_var_req 				*/
/* This function is called from MMSDEC when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_get_var_req (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("Get Variable Access Attributes Request");

			/* get storage for request information		*/
/* get storage for request information	*/
  req_info = (GETVAR_REQ_INFO *) _m_get_dec_buf (aCtx, sizeof (GETVAR_REQ_INFO));
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 0, getvar_name_cstr);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 1, getvar_addr_cstr);
  }

/************************************************************************/
/*			getvar_name_cstr				*/
/* decoding get variable access attributes request: name constructor	*/
/* was encountered.							*/
/************************************************************************/

static ST_VOID getvar_name_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("getvar_name_cstr");

  req_info->req_tag = SD_SUCCESS;			/* set tag to name	*/

  _ms_get_mms_objname (aCtx, &req_info->name, getvar_na_done);
  }

/************************************************************************/
/*			getvar_addr_cstr				*/
/* decoding get variable access attributes request: address constructor */
/* was encountered.							*/
/************************************************************************/

static ST_VOID getvar_addr_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("getvar_addr_cstr");

  req_info->req_tag = SD_TRUE;			/* set tag to address	*/

  _ms_get_va_address (aCtx, &req_info->address, getvar_na_done);
  }

/************************************************************************/
/*			getvar_na_done					*/
/* decoding get variable access attributes request: name or address has */
/* been obtained.							*/
/************************************************************************/

static ST_VOID getvar_na_done (ASN1_DEC_CTXT *aCtx)
  {
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_getvar_resp					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE & send the get variable access attributes response */
/************************************************************************/

ST_RET mp_getvar_resp (MMSREQ_IND *indptr, GETVAR_RESP_INFO *info_ptr)
  {
  return (_mms_resp_send (MMSOP_GET_VAR,indptr,
			M_CAST_MK_FUN (_ms_mk_getvar_resp),
			(ST_CHAR *)info_ptr));
  }

#else
/************************************************************************/
/************************************************************************/
/*			mpl_getvar_resp					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the get variable access attributes response 	*/
/************************************************************************/

ST_RET mpl_getvar_resp (ST_UINT32 invoke_id, GETVAR_RESP_INFO *info_ptr)
  {
  return (_mms_resp_fin (MMSOP_GET_VAR,invoke_id,
			M_CAST_MK_FUN (_ms_mk_getvar_resp),
			(ST_CHAR *)info_ptr));
  }
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_getvar_resp					*/
/* Construct a get variable access attributes response. 		*/
/************************************************************************/

ST_VOID _ms_mk_getvar_resp (ASN1_ENC_CTXT *aCtx, GETVAR_RESP_INFO *info)
  {
  asn1r_strt_constr (aCtx);			/* start context tag 2 cstr	*/
  _ms_wr_va_type_spec (aCtx, &info->type_spec);	/* write the type specification */
  asn1r_fin_constr (aCtx, 2, CTX, DEF);		/* finish context tag 2 cstr	*/

  if (info->address_pres)
    {
    asn1r_strt_constr (aCtx);			/* start context tag 1 cstr	*/
    _ms_wr_va_address (aCtx, &info->address);	/* write the address		*/
    asn1r_fin_constr (aCtx, 1, CTX, DEF);		/* finish context tag 1 cstr	*/
    }

  asn1r_wr_bool (aCtx, info->mms_deletable); 	/* write mms deletable flag	*/
  asn1r_fin_prim (aCtx, 0,CTX);			/* context tag 1		*/

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_constr (aCtx, MMSOP_GET_VAR,CTX,DEF); 	/* tag = opcode, ctx constr	*/
  }
