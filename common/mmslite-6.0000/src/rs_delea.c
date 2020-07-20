/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rs_delea.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the responder portion of delete event	*/
/*	action.  It decodes the delete event action request 		*/
/*	(indication) and encodes the delete event action response.	*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 02/24/10  JRB	   Del _mms_dec_info, use aCtx->mmsdec_rslt->data_ptr*/
/* 03/11/04  GLB     06    Remove "thisFileName"                    	*/
/* 12/20/01  JRB     05    Converted to use ASN1R (re-entrant ASN1)	*/
/* 09/13/99  MDE     04    Added SD_CONST modifiers			*/
/* 03/23/99  MDE     03    Changes to decode buffer allocation scheme	*/
/* 06/15/98  MDE     02    Changes to allow compile under C++		*/
/* 06/09/97  MDE     01    Changed op specific buffer allocation	*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"

#include "mmsdefs.h"
#include "mms_pevn.h"
#include "asn1defs.h"

/************************************************************************/
/* variables global to the DECODE portion :				*/

static  DELEA_REQ_INFO	*req_info;

static OBJECT_NAME      *obj_ptr;
static ST_INT max_obj_name;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

static ST_VOID delea_name_list_cstr      (ASN1_DEC_CTXT *aCtx);
static ST_VOID delea_objname_done        (ASN1_DEC_CTXT *aCtx);
static ST_VOID delea_name_list_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID delea_get_aa_spec         (ASN1_DEC_CTXT *aCtx);
static ST_VOID delea_get_domain          (ASN1_DEC_CTXT *aCtx);
static ST_VOID delea_get_vmd             (ASN1_DEC_CTXT *aCtx);
static ST_VOID delea_dec_done            (ASN1_DEC_CTXT *aCtx);


/************************************************************************/
/************************************************************************/
/*			mms_delea_req 					*/
/* This function is called from mmsdec when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_delea_req (ASN1_DEC_CTXT *aCtx)
  {
ST_INT	req_size;

  MLOG_CDEC0 ("Delete Event Action Request");

#if defined(FIXED_DECBUF)
  max_obj_name = m_sv_max_obj_name;
  req_size = mmsl_dec_info_size;
#else
  max_obj_name = _mmsdec_msglen / 2;	/* find maximum size */
  if (m_sv_max_obj_name && (max_obj_name > m_sv_max_obj_name))
    max_obj_name = m_sv_max_obj_name;
  req_size = sizeof (DELEA_REQ_INFO) + (sizeof (OBJECT_NAME) * max_obj_name);
#endif

/* get storage for request information	*/
  req_info = (DELEA_REQ_INFO *) _m_get_dec_buf (aCtx, req_size);

  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 0, delea_name_list_cstr);
  ASN1R_TAG_ADD (aCtx, CTX,          1, delea_get_aa_spec);
  ASN1R_TAG_ADD (aCtx, CTX, 	 	3, delea_get_domain);
  ASN1R_TAG_ADD (aCtx, CTX,          4, delea_get_vmd);
  }

/************************************************************************/
/*			delea_name_list_cstr				*/
/* decoding delete event action request: decode the specific name list	*/
/* constructor.								*/
/************************************************************************/

static ST_VOID delea_name_list_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("delea_name_list_cstr");

  req_info->req_tag = 0;		/* set tag to specific		*/
  obj_ptr  = (OBJECT_NAME *) (req_info + 1);

  _ms_get_mms_objname (aCtx, obj_ptr, delea_objname_done);

					/* set the cstr done function	*/
  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = delea_name_list_cstr_done;
  }

/************************************************************************/
/*			delea_objname_done				*/
/* decoding delete event action request: object name has been	*/
/* decoded.								*/
/************************************************************************/

static ST_VOID delea_objname_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("delea_objname_done");

  req_info->num_of_names++;		/* inc # of variable names	*/
  if (req_info->num_of_names > max_obj_name)
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);
  else
    {
    obj_ptr++;				/* point to where next will go	*/
    _ms_get_mms_objname (aCtx, obj_ptr, delea_objname_done);
    }
  }

/************************************************************************/
/*			delea_name_list_cstr_done			*/
/* decoding delete event action request: specific name list constr	*/
/* done was encountered. 						*/
/************************************************************************/

static ST_VOID delea_name_list_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
  aCtx->asn1r_decode_done_fun = delea_dec_done;	
  _ms_set_cs_check2 (aCtx);
  }

/************************************************************************/
/*			delea_get_aa_spec				*/
/* decoding delete event action request: aa specific was encountered	*/
/************************************************************************/

static ST_VOID delea_get_aa_spec (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("delea_get_aa_spec");

  req_info->req_tag = 1;		/* set tag to aa specific	*/
  aCtx->asn1r_decode_done_fun = delea_dec_done;	
  _ms_set_cs_check2 (aCtx);
  }

/************************************************************************/
/*			delea_get_domain				*/
/* decoding delete event action request: domain was encountered.	*/
/************************************************************************/

static ST_VOID delea_get_domain (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("delea_get_domain");

  req_info->req_tag = 3;		/* set tag to domain		*/

  if (asn1r_get_identifier (aCtx, req_info->dname))/* read the domain name 	*/
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);

  aCtx->asn1r_decode_done_fun = delea_dec_done;	
  _ms_set_cs_check2 (aCtx);
  }

/************************************************************************/
/*			delea_get_vmd					*/
/* decoding delete event action request: vmd was encountered.	*/
/************************************************************************/

static ST_VOID delea_get_vmd (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("delea_get_vmd");

  req_info->req_tag = 4;		/* set tag to vmd		*/
  aCtx->asn1r_decode_done_fun = delea_dec_done;	
  _ms_set_cs_check2 (aCtx);
  }

/************************************************************************/
/*			delea_dec_done			   		*/
/* decode done, realloc the structure to desired size			*/
/************************************************************************/

static ST_VOID delea_dec_done (ASN1_DEC_CTXT *aCtx)
  {  
ST_INT	req_size;
  				/* find actual size for request info	*/
  req_size = sizeof (DELEA_REQ_INFO)
		+ (sizeof (OBJECT_NAME) * req_info->num_of_names);

				/* give back unused storage		*/
  aCtx->mmsdec_rslt->data_ptr = (*m_realloc_os_fun) (aCtx->mmsdec_rslt->data_ptr,req_size);
  req_info = (DELEA_REQ_INFO *) aCtx->mmsdec_rslt->data_ptr;

  _mms_dec_done_ok (aCtx); 	 
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_delea_resp		   			*/
/* Create and send a delete event action response.  			*/
/************************************************************************/

ST_RET mp_delea_resp (MMSREQ_IND *indptr, DELEA_RESP_INFO *info_ptr)
  {
  return (_mms_resp_send (MMSOP_DEL_EA,indptr,
			M_CAST_MK_FUN (_ms_mk_delea_resp),
			(ST_CHAR *)info_ptr));
  }

#else
/************************************************************************/
/************************************************************************/
/*			mpl_delea_resp		   			*/
/* Create a delete event action response.  				*/
/************************************************************************/

ST_RET mpl_delea_resp (ST_UINT32 invoke_id, DELEA_RESP_INFO *info_ptr)
  {
  return (_mms_resp_fin (MMSOP_DEL_EA,invoke_id,
			M_CAST_MK_FUN (_ms_mk_delea_resp),
			(ST_CHAR *)info_ptr));
  }
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_delea_resp  					*/
/* construct a delete event action response. 			*/
/************************************************************************/

ST_VOID _ms_mk_delea_resp (ASN1_ENC_CTXT *aCtx, DELEA_RESP_INFO *info)
  {
  asn1r_wr_u32 (aCtx, info->cand_not_deleted); 	/* canditates not deleted 	*/

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_prim (aCtx, MMSOP_DEL_EA,CTX);     	/* tag = opcode, ctx primitive		*/
  }
