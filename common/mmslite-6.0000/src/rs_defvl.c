/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rs_defvlist.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the responder portion of define named	*/
/*      variable list.  It decodes the define named variable list 	*/
/*	request (indication) & encodes the define named variable list	*/
/*	response.							*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 02/24/10  JRB	   Del _mms_dec_info, use aCtx->mmsdec_rslt->data_ptr*/
/* 03/11/04  GLB     08    Remove "thisFileName"                    	*/
/* 12/20/01  JRB     07    Converted to use ASN1R (re-entrant ASN1)	*/
/* 08/21/01  JRB     06    Don't allow compile if INT_MAX not defined.	*/
/* 06/22/01  RKR     05    Added limits.h				*/
/* 04/27/00  MDE     04    Lint cleanup					*/
/* 09/13/99  MDE     03    Added SD_CONST modifiers			*/
/* 03/23/99  MDE     02    Changes to decode buffer allocation scheme	*/
/* 07/03/97  MDE     01    Op-specific info buffer handling changes	*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include <limits.h>
#ifndef INT_MAX
#error INT_MAX must be defined. Usually defined in limits.h
#endif
#include "mmsdefs.h"
#include "mms_pvar.h"
#include "asn1defs.h"

/************************************************************************/
/* variables global to the DECODE portion :				*/

static VARIABLE_LIST *vlist_ptr;
static ST_INT  max_num_vars;

static DEFVLIST_REQ_INFO *req_info;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

static ST_VOID defvlist_list_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID defvlist_get_varlist (ASN1_DEC_CTXT *aCtx);
static ST_VOID defvlist_name_done (ASN1_DEC_CTXT *aCtx);


/************************************************************************/
/************************************************************************/
/*			mms_def_vlist_req  				*/
/* This function is called from MMSDEC when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

/* This is used to restrict the maximum buffer size to 0xFF00, which is	*/
/* close to the max allowed for the chk_xxx series for 16 bit systems	*/
#define MAX_ALLOWED_VARS  ((0xFF00 - sizeof (DEFVLIST_REQ_INFO)) / sizeof (VARIABLE_LIST))


ST_VOID mms_def_vlist_req (ASN1_DEC_CTXT *aCtx)
  {
ST_INT	req_size;

  MLOG_CDEC0 ("Define Named Variable List Request");

#if defined(FIXED_DECBUF)
  max_num_vars = m_sv_max_num_vars;
  req_size = mmsl_dec_info_size;
#else
  max_num_vars = _mmsdec_msglen / 2;
  if (m_sv_max_num_vars && (max_num_vars > m_sv_max_num_vars))
    max_num_vars = m_sv_max_num_vars;

				/* verify buffer size is allocatable	*/
#if (INT_MAX <= 0x7FFF)
  if (max_num_vars > MAX_ALLOWED_VARS)
    max_num_vars = MAX_ALLOWED_VARS;
#endif

  req_size = sizeof(DEFVLIST_REQ_INFO) + 
	     (sizeof(VARIABLE_LIST) * max_num_vars);
#endif

/* get storage for request information	*/
  req_info = (DEFVLIST_REQ_INFO *) _m_get_dec_buf (aCtx, req_size);
  vlist_ptr = (VARIABLE_LIST *) (req_info + 1);

  _ms_get_mms_objname (aCtx, &req_info->vl_name, defvlist_name_done);
  }

/************************************************************************/
/*			defvlist_name_done				*/
/* decoding define named variable list request: variable list name has	*/
/* been obtained.							*/
/************************************************************************/

static ST_VOID defvlist_name_done (ASN1_DEC_CTXT *aCtx)
  {

  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 0, defvlist_get_varlist);
  }

/************************************************************************/
/*			defvlist_get_varlist				*/
/* decoding define named variable list request: list of variables 	*/
/* constructor has been obtained.    					*/
/************************************************************************/

static ST_VOID defvlist_get_varlist (ASN1_DEC_CTXT *aCtx)
  {

  req_info->num_of_variables = 0;
  _ms_get_va_var_list (aCtx, vlist_ptr, &req_info->num_of_variables, 
  			max_num_vars, defvlist_list_done);
  }

/************************************************************************/
/*			defvlist_list_done				*/
/* decoding define named variable list request: list of variables has	*/
/* been obtained.							*/
/************************************************************************/

static ST_VOID defvlist_list_done (ASN1_DEC_CTXT *aCtx)
  {
ST_INT	req_size;
  
				/* find actual size for request info	*/
  req_size = sizeof (DEFVLIST_REQ_INFO)
		+ (sizeof (VARIABLE_LIST)*req_info->num_of_variables);

				/* give back unused storage		*/
  aCtx->mmsdec_rslt->data_ptr = (*m_realloc_os_fun) (aCtx->mmsdec_rslt->data_ptr, req_size);
  req_info = (DEFVLIST_REQ_INFO *) aCtx->mmsdec_rslt->data_ptr;

  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_defvlist_resp  				*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE and send the define named variable list response.	*/
/************************************************************************/

ST_RET mp_defvlist_resp (MMSREQ_IND *indptr)
  {
  return (_mms_send_null_resp (indptr,MMSOP_DEF_VLIST));
  }

#else
/************************************************************************/
/************************************************************************/
/*			mpl_defvlist_resp  				*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the define named variable list response.		*/
/************************************************************************/

ST_RET  mpl_defvlist_resp (ST_UINT32 invoke_id)
  {
  return (_mms_fin_null_resp (invoke_id,MMSOP_DEF_VLIST));
  }
#endif
