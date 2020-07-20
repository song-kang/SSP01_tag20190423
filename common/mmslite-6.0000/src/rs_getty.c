/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rs_gettype.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the responder portion of get named type	*/
/*	attributes.  It decodes the get named type attributes request	*/
/*	(indication) & encodes the get named type attributes response.	*/
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

static  GETTYPE_REQ_INFO	*req_info;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

static ST_VOID gettype_type_name_done (ASN1_DEC_CTXT *aCtx);


/************************************************************************/
/************************************************************************/
/*			mms_get_type_req				*/
/* This function is called from MMSDEC when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_get_type_req (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("Get Named Type Attributes Request");

/* get storage for request information		*/
  req_info = (GETTYPE_REQ_INFO *) _m_get_dec_buf (aCtx, sizeof (GETTYPE_REQ_INFO));
  _ms_get_mms_objname (aCtx, &req_info->type_name, gettype_type_name_done);
  }

/************************************************************************/
/*			gettype_type_name_done				*/
/* decoding get named type attributes request: type name has been	*/
/* obtained.								*/
/************************************************************************/

static ST_VOID gettype_type_name_done (ASN1_DEC_CTXT *aCtx)
  {
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_gettype_resp 				*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE and send the get named type attributes response.	*/
/************************************************************************/

ST_RET mp_gettype_resp (MMSREQ_IND *indptr, GETTYPE_RESP_INFO *info_ptr)
  {
  return (_mms_resp_send (MMSOP_GET_TYPE,indptr,
			M_CAST_MK_FUN (_ms_mk_gettype_resp),
			(ST_CHAR *)info_ptr));
  }

#else
/************************************************************************/
/************************************************************************/
/*			mpl_gettype_resp 				*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the get named type attributes response.		*/
/************************************************************************/

ST_RET mpl_gettype_resp (ST_UINT32 invoke_id, GETTYPE_RESP_INFO *info_ptr)
  {
  return (_mms_resp_fin (MMSOP_GET_TYPE,invoke_id,
			M_CAST_MK_FUN (_ms_mk_gettype_resp),
			(ST_CHAR *)info_ptr));
  }
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_gettype_resp 				*/
/* Construct a get named type attributes response.			*/
/************************************************************************/

ST_VOID _ms_mk_gettype_resp (ASN1_ENC_CTXT *aCtx, GETTYPE_RESP_INFO *info)
  {
  _ms_wr_va_type_spec (aCtx, &info->type_spec);	/* write the type specification */

  asn1r_wr_bool (aCtx , info->mms_deletable);  	/* write mms deletable flag	*/
  asn1r_fin_prim (aCtx , 0,CTX);			/* context tag 1		*/

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_constr (aCtx , MMSOP_GET_TYPE,CTX,DEF);	/* tag = opcode, ctx constr	*/
  }
