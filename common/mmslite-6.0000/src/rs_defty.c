/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rs_deftype.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the responder portion of define named	*/
/*	type.  It decodes the define named type request (indication)	*/
/*	and encodes the define named type response.			*/
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

static  DEFTYPE_REQ_INFO	*req_info;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

static ST_VOID deftype_typespec_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID deftype_objname_done (ASN1_DEC_CTXT *aCtx);


/************************************************************************/
/************************************************************************/
/*			mms_def_type_req				*/
/* This function is called from mmsdec when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_def_type_req (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("Define Named Type Request");

/* get storage for request information	*/
  req_info = (DEFTYPE_REQ_INFO *) _m_get_dec_buf (aCtx, sizeof (DEFTYPE_REQ_INFO));

  _ms_get_mms_objname (aCtx, &req_info->type_name, deftype_objname_done);
  }

/************************************************************************/
/*			deftype_objname_done				*/
/* define named type request: type name has been decoded.		*/
/************************************************************************/

static ST_VOID deftype_objname_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("deftype_objname_done");

  _ms_get_va_type_spec (aCtx, &req_info->type_spec, deftype_typespec_done);
  }

/************************************************************************/
/*			deftype_typespec_done				*/
/* define named type request: type specification has been decoded.	*/
/************************************************************************/

static ST_VOID deftype_typespec_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("deftype_typespec_done");

  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_deftype_resp 				*/
/* Create and send a define named type response.			*/
/************************************************************************/

ST_RET mp_deftype_resp (MMSREQ_IND *indptr)
  {
  return (_mms_send_null_resp (indptr,MMSOP_DEF_TYPE));
  }

#else
/************************************************************************/
/************************************************************************/
/*			mpl_deftype_resp 				*/
/* Create a define named type response.					*/
/************************************************************************/

ST_RET mpl_deftype_resp (ST_UINT32 invoke_id)
  {
  return (_mms_fin_null_resp (invoke_id,MMSOP_DEF_TYPE));
  }
#endif
