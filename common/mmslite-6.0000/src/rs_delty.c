/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rs_deltype.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the responder portion of delete named	*/
/*	type.  It decodes the delete named type request (indication)	*/
/*	and encodes the delete named type response.			*/
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
#include "mms_pvar.h"
#include "asn1defs.h"

/************************************************************************/
/* variables global to the DECODE portion :				*/

static  DELTYPE_REQ_INFO	*req_info;

static OBJECT_NAME      *obj_ptr;
static ST_INT max_obj_name;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/
/* static functions with multiple references                            */

static ST_VOID deltype_get_dname (ASN1_DEC_CTXT *aCtx);
static ST_VOID deltype_name_list_cstr (ASN1_DEC_CTXT *aCtx);
static ST_VOID deltype_objname_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID deltype_name_list_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID deltype_dec_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID deltype_get_scope (ASN1_DEC_CTXT *aCtx);


/************************************************************************/
/************************************************************************/
/*			mms_del_type_req				*/
/* This function is called from mmsdec when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_del_type_req (ASN1_DEC_CTXT *aCtx)
  {
ST_INT	req_size;

  MLOG_CDEC0 ("Delete Named Type Request");

#if defined(FIXED_DECBUF)
  max_obj_name = m_sv_max_obj_name;
  req_size = mmsl_dec_info_size;
#else
  max_obj_name = _mmsdec_msglen / 2; /* find maximum size  */
  if (m_sv_max_obj_name && (max_obj_name > m_sv_max_obj_name))
    max_obj_name = m_sv_max_obj_name;
  req_size = sizeof (DELTYPE_REQ_INFO) + (sizeof (OBJECT_NAME) * max_obj_name);
#endif

/* get storage for request information	*/
  req_info = (DELTYPE_REQ_INFO *) _m_get_dec_buf (aCtx, req_size);

/*  req_info->tnames_pres   = SD_FALSE;              */
/*  req_info->num_of_tnames = ZERO_ENTRIES;       */
/*  req_info->dname_pres	  = SD_FALSE;        */


  ASN1R_TAG_ADD (aCtx, CTX, 0, deltype_get_scope);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 1, deltype_name_list_cstr);
  ASN1R_TAG_ADD (aCtx, CTX, 	 2, deltype_get_dname);
  _ms_set_cs_check2 (aCtx);		/* check for Companion Standard info	*/

  asn1r_set_all_cstr_done (aCtx); 	/* clear cstr done functions	*/
  aCtx->asn1r_decode_done_fun = deltype_dec_done;	/* ok to be done, set done fun	*/
  }

/************************************************************************/
/*			deltype_get_scope				*/
/* delete named type request: decode the scope. 			*/
/************************************************************************/

static ST_VOID deltype_get_scope (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("deltype_get_scope");
					/* read scope from message	*/
  if (asn1r_get_i16 (aCtx, &req_info->scope))
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);

  if ((req_info->scope < 0) || (req_info->scope > 3))
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);	      /* validate range of scope [0..3] */


  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 1, deltype_name_list_cstr);
  ASN1R_TAG_ADD (aCtx, CTX, 	 2, deltype_get_dname);
  _ms_set_cs_check2 (aCtx);		/* check for Companion Standard info	*/
  }	       

/************************************************************************/
/*			deltype_name_list_cstr				*/
/* delete named type request: decode the type name constructor		*/
/************************************************************************/

static ST_VOID deltype_name_list_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("deltype_name_list_cstr");

  req_info->tnames_pres = SD_TRUE; 	/* set type name list present	*/

  obj_ptr  = (OBJECT_NAME *) (req_info + 1);

  _ms_get_mms_objname (aCtx, obj_ptr, deltype_objname_done);

					/* set the cstr done function	*/
  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = deltype_name_list_cstr_done;
  }

/************************************************************************/
/*			deltype_objname_done				*/
/* delete named type request: type name has been decoded.		*/
/************************************************************************/

static ST_VOID deltype_objname_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("deltype_objname_done");

  req_info->num_of_tnames++;		/* increment # of type names	*/
  if ((req_info->num_of_tnames + 1) <= max_obj_name)
    {
    obj_ptr++;				/* point to where next will go	*/
    _ms_get_mms_objname (aCtx, obj_ptr, deltype_objname_done);
    }
  }

/************************************************************************/
/*			deltype_name_list_cstr_done			*/
/* delete named type request: end of the type name list constructor was */
/* encountered. 							*/
/************************************************************************/

static ST_VOID deltype_name_list_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
  ASN1R_TAG_ADD (aCtx, CTX, 2, deltype_get_dname);
  _ms_set_cs_check2 (aCtx);		/* check for Companion Standard info	*/
  }

/************************************************************************/
/*			deltype_get_dname				*/
/* delete named type request: domain name was encountered.		*/
/************************************************************************/

static ST_VOID deltype_get_dname (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("deltype_get_dname");

  req_info->dname_pres = SD_TRUE;		/* set domain name present	*/

  if (asn1r_get_identifier (aCtx, req_info->dname))/* read the domain name 	*/
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);

  _ms_set_cs_check2 (aCtx);		/* check for Companion Standard info	*/
  }

/************************************************************************/
/*			deltype_dec_done					*/
/* decode done, realloc the structure to desired size			*/
/************************************************************************/

static ST_VOID deltype_dec_done (ASN1_DEC_CTXT *aCtx)
  {  
ST_INT	req_size;

  				/* find actual size for request info	*/
  req_size = sizeof (DELTYPE_REQ_INFO)
		+ (sizeof (OBJECT_NAME) * req_info->num_of_tnames);

				/* give back unused storage		*/
  aCtx->mmsdec_rslt->data_ptr = (*m_realloc_os_fun) (aCtx->mmsdec_rslt->data_ptr,req_size);
  req_info = (DELTYPE_REQ_INFO *) aCtx->mmsdec_rslt->data_ptr;
  _mms_dec_done_ok (aCtx); 		/* decode done, set done, etc		*/
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_deltype_resp					*/
/* Create and send a delete type response.				*/
/************************************************************************/

ST_RET mp_deltype_resp (MMSREQ_IND *indptr, DELTYPE_RESP_INFO *info_ptr)
  {
  return (_mms_resp_send (MMSOP_DEL_TYPE,indptr,
			M_CAST_MK_FUN (_ms_mk_deltype_resp),
			(ST_CHAR *)info_ptr));
  }

#else
/************************************************************************/
/************************************************************************/
/*			mpl_deltype_resp   				*/
/* Create a delete type response.					*/
/************************************************************************/

ST_RET mpl_deltype_resp (ST_UINT32 invoke_id, DELTYPE_RESP_INFO *info_ptr)
  {
  return (_mms_resp_fin (MMSOP_DEL_TYPE,invoke_id,
			M_CAST_MK_FUN (_ms_mk_deltype_resp),
			(ST_CHAR *)info_ptr));
  }
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_deltype_resp					*/
/* construct a delete type response. 					*/
/************************************************************************/

ST_VOID _ms_mk_deltype_resp (ASN1_ENC_CTXT *aCtx, DELTYPE_RESP_INFO *info)
  {
  asn1r_wr_u32 (aCtx, info->num_deleted);		/* write # deleted		*/
  asn1r_fin_prim (aCtx, 1,CTX);

  asn1r_wr_u32 (aCtx, info->num_matched);		/* write # matched		*/
  asn1r_fin_prim (aCtx, 0,CTX);

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_constr (aCtx, MMSOP_DEL_TYPE,CTX,DEF);	/* tag = opcode, ctx constr	*/
  }
