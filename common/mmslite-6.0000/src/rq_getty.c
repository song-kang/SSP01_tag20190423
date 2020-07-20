/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rq_gettype.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the requester portion of the get named	*/
/*	type attributes.  It decodes the get named type attributes	*/
/*	response (confirm) & encodes the get named type attributes	*/
/*	request.							*/
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

static  GETTYPE_RESP_INFO	*rsp_info;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

static ST_VOID gettype_type_spec_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID gettype_get_mms_deletable (ASN1_DEC_CTXT *aCtx);


/************************************************************************/
/************************************************************************/
/*			mms_get_type_rsp				*/
/* This function is called from MMSDEC when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_get_type_rsp (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("Get Named Type Attributes Response");

/* get storage for request information	*/
  rsp_info = (GETTYPE_RESP_INFO *) _m_get_dec_buf (aCtx, sizeof (GETTYPE_RESP_INFO));
  ASN1R_TAG_ADD (aCtx, CTX, 0, gettype_get_mms_deletable);
  }

/************************************************************************/
/*			gettype_get_mms_deletable			*/
/* Decoding get named type attributes response: mms deletable was	*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID gettype_get_mms_deletable (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("gettype_get_mms_deletable");
					/* read mms deletable		*/
  if (asn1r_get_bool (aCtx, &(rsp_info->mms_deletable)))
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);

  _ms_get_va_type_spec (aCtx, &rsp_info->type_spec, gettype_type_spec_done);
  }

/************************************************************************/
/*			gettype_type_spec_done				*/
/* Decoding get named type attributes response: type specification has	*/
/* been decoded.							*/
/************************************************************************/

static ST_VOID gettype_type_spec_done (ASN1_DEC_CTXT *aCtx)
  {

  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_gettype					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE and send the get named type attributes request.	*/
/************************************************************************/

MMSREQ_PEND *mp_gettype (ST_INT chan, GETTYPE_REQ_INFO *info_ptr)
  {
  return (_mms_req_send (MMSOP_GET_TYPE,
			M_CAST_MK_FUN (_ms_mk_gettype_req),
			chan,(ST_CHAR *)info_ptr));
  } 

#else
/************************************************************************/
/************************************************************************/
/*			mpl_gettype					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the get named type attributes request.		*/
/************************************************************************/

ST_RET  mpl_gettype (GETTYPE_REQ_INFO *info_ptr)
  {
  return (_mms_req_fin (MMSOP_GET_TYPE,
			M_CAST_MK_FUN (_ms_mk_gettype_req),
			(ST_CHAR *)info_ptr));
  } 
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_gettype_req					*/
/* construct a get named type attributes request			*/
/************************************************************************/

ST_VOID _ms_mk_gettype_req (ASN1_ENC_CTXT *aCtx, GETTYPE_REQ_INFO *info)
  {
  _ms_wr_mms_objname (aCtx, &info->type_name);	/* write the type name		*/

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_constr (aCtx, MMSOP_GET_TYPE,CTX,DEF);	/* tag = opcode, ctx		*/
  }
