/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rs_info.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the responder portion of information	*/
/*	report. It decodes the information report request (indication). */
/*	There is no response.						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 02/24/10  JRB	   Del _mms_dec_info, use aCtx->mmsdec_rslt->data_ptr*/
/* 03/11/04  GLB     05    Remove "thisFileName"                    	*/
/* 12/20/01  JRB     04    Converted to use ASN1R (re-entrant ASN1)	*/
/* 09/13/99  MDE     03    Added SD_CONST modifiers			*/
/* 03/23/99  MDE     02    Changes to decode buffer allocation scheme	*/
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

static  INFO_REQ_INFO	*req_info;


static ACCESS_RESULT *ar_ptr;
static ST_INT	max_vars;
static ST_INT	max_acc_rslt;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

static ST_VOID info_acc_rslt_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID info_acc_rslt_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID info_acc_rslt_cstr (ASN1_DEC_CTXT *aCtx);
static ST_VOID info_va_spec_done (ASN1_DEC_CTXT *aCtx);


/************************************************************************/
/************************************************************************/
/*			mms_info_rpt_req				*/
/* This function is called from mmsdec when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_info_rpt_req (ASN1_DEC_CTXT *aCtx)
  {
ST_INT size1;
ST_INT size2;
ST_INT req_size;

  MLOG_CDEC0 ("Information Report Request");

#if defined(FIXED_DECBUF)
  max_acc_rslt = m_cl_max_info_acc_rslt;
  max_vars = m_cl_max_info_var_spec;
  req_size = mmsl_dec_info_size;
#else

/* First assume a list of variables (each var has a data) */
  max_vars	 = _mmsdec_msglen / 7;
  if (m_cl_max_info_var_spec && (max_vars > m_cl_max_info_var_spec))
    max_vars = m_cl_max_info_var_spec;

  size1 = sizeof(INFO_REQ_INFO) + 
          (max_vars * (sizeof(VARIABLE_LIST) + sizeof(ACCESS_RESULT)));

/* Now assume a named variable list name */
  max_acc_rslt = _mmsdec_msglen / 3;
  if (m_cl_max_info_acc_rslt && (max_acc_rslt > m_cl_max_info_acc_rslt))
    max_acc_rslt = m_cl_max_info_acc_rslt;
  size2 = sizeof(INFO_REQ_INFO) + (sizeof(ACCESS_RESULT) * max_acc_rslt);

  req_size = max(size1, size2);
#endif

/* get storage for request information	*/
  req_info = (INFO_REQ_INFO *) _m_get_dec_buf (aCtx, req_size);
  _ms_get_va_spec (aCtx, &req_info->va_spec, max_vars, info_va_spec_done);
  }

/************************************************************************/
/*			info_va_spec_done				*/
/* decoding information report request: variable access specification	*/
/* has been decoded.							*/
/************************************************************************/

static ST_VOID info_va_spec_done (ASN1_DEC_CTXT *aCtx)
  {

VARIABLE_LIST *temp;

  MLOG_CDEC0 ("info_va_spec_done");

  temp	= (VARIABLE_LIST *) (req_info + 1);
  temp += req_info->va_spec.num_of_variables;

  req_info->acc_rslt_list = (ACCESS_RESULT *) temp;

  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 0, info_acc_rslt_cstr);
  }

/************************************************************************/
/*			info_acc_rslt_cstr				*/
/* decoding information report request: access result constructor was	*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID info_acc_rslt_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("info_acc_rslt_cstr");

  ar_ptr = req_info->acc_rslt_list;

  _ms_get_va_access_rslt (aCtx, ar_ptr, info_acc_rslt_done);
  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = info_acc_rslt_cstr_done;
  }

/************************************************************************/
/*			info_acc_rslt_done				*/
/* decoding information report request: access result has been decoded. */
/************************************************************************/

static ST_VOID info_acc_rslt_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("info_acc_rslt_done");

  req_info->num_of_acc_result++;
  if ((req_info->num_of_acc_result + 1) <= max_acc_rslt)
    {
    ar_ptr++;
    _ms_get_va_access_rslt (aCtx, ar_ptr, info_acc_rslt_done);
    }
  }

/************************************************************************/
/*			info_acc_rslt_cstr_done 			*/
/* decoding information report request: access result constructor done	*/
/* was encountered.							*/
/************************************************************************/

static ST_VOID info_acc_rslt_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
ST_INT	 req_size;
VARIABLE_LIST	*temp;

  MLOG_CDEC0 ("info_acc_rslt_cstr_done");
				/* find actual size for request info	*/
  req_size = sizeof (INFO_REQ_INFO)
	     + (sizeof (VARIABLE_LIST)*req_info->va_spec.num_of_variables)
	     + (sizeof (ACCESS_RESULT)*req_info->num_of_acc_result);

				/* give back unused storage		*/
  aCtx->mmsdec_rslt->data_ptr = (*m_realloc_os_fun) (aCtx->mmsdec_rslt->data_ptr,req_size);
  req_info = (INFO_REQ_INFO *) aCtx->mmsdec_rslt->data_ptr;

/* some C run time library mem mgmt. functions will always give back a	*/
/* new pointer when reallocating smaller.  In the case of info_req_info	*/
/* we have to update the acc_rslt_list ptr to point to this new mem loc	*/

  temp	= (VARIABLE_LIST *) (req_info + 1);
  temp += req_info->va_spec.num_of_variables;
  req_info->acc_rslt_list = (ACCESS_RESULT *) temp;

  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }
