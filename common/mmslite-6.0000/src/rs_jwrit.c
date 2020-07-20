/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rs_jwrite.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the responder portion of write journal.	*/
/*	It decodes the write journal request (indication) & encodes	*/
/*	the write journal response.					*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 11/23/11  KCR           Added casting for proper 64bit compile       */
/* 02/24/10  JRB	   Del _mms_dec_info, use aCtx->mmsdec_rslt->data_ptr*/
/* 03/11/04  GLB     05    Remove "thisFileName"                    	*/
/* 12/20/01  JRB     04    Converted to use ASN1R (re-entrant ASN1)	*/
/* 09/13/99  MDE     03    Added SD_CONST modifiers			*/
/* 03/23/99  MDE     02    Changes to decode buffer allocation scheme	*/
/* 06/09/97  MDE     01    Changed op specific buffer allocation	*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"

#include "mmsdefs.h"
#include "mms_pjou.h"
#include "asn1defs.h"

/************************************************************************/
/* variables global to the DECODE portion :				*/

static  JWRITE_REQ_INFO	*req_info;

static ENTRY_CONTENT   *ec_ptr;
static ST_INT	max_entries;
static ST_INT	max_data_vars;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

/************************************************************************/
/* static functions with multiple references                            */

static ST_VOID jwrite_jou_entry_cstr (ASN1_DEC_CTXT *aCtx);
static ST_VOID jwrite_je_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID jwrite_list_of_je_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID jwrite_list_of_je_cstr (ASN1_DEC_CTXT *aCtx);
static ST_VOID jwrite_jname_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID jwrite_jname_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID jwrite_jname_cstr  (ASN1_DEC_CTXT *aCtx);


/************************************************************************/
/************************************************************************/
/*			mms_jwrite_req					*/
/* This function is called from MMSDEC when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_jwrite_req (ASN1_DEC_CTXT *aCtx)
  {
ST_INT	 req_size;

  MLOG_CDEC0 ("Write Journal Request");

#if defined(FIXED_DECBUF)
  max_entries = m_sv_max_entry_content;
  max_data_vars = m_sv_max_data_vars;
  req_size = mmsl_dec_info_size;
#else
  max_entries	= (_mmsdec_msglen/4);
  max_data_vars = (_mmsdec_msglen/6);
  if (m_sv_max_entry_content && (max_entries > m_sv_max_entry_content))
    max_entries = m_sv_max_entry_content;
  if (m_sv_max_data_vars && (max_data_vars > m_sv_max_data_vars))
    max_data_vars = m_sv_max_data_vars;

  req_size = sizeof (JWRITE_REQ_INFO)
		+ (max_entries * sizeof (ENTRY_CONTENT))
		+ (max_data_vars * sizeof (VAR_INFO));
#endif

/* get storage for request information	*/
  req_info = (JWRITE_REQ_INFO *) _m_get_dec_buf (aCtx, req_size);
  ec_ptr   = (ENTRY_CONTENT *) (req_info + 1);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 0, jwrite_jname_cstr);
  }

/************************************************************************/
/*			jwrite_jname_cstr				*/
/* write journal request is being decoded: journal name constructor	*/
/* was encountered.							*/
/************************************************************************/

static ST_VOID jwrite_jname_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("jwrite_jname_cstr");

  _ms_get_mms_objname (aCtx, &req_info->jou_name, jwrite_jname_done);
  }

/************************************************************************/
/*			jwrite_jname_done				*/
/* write journal request is being decoded: journal name has been	*/
/* obtained.								*/
/************************************************************************/

static ST_VOID jwrite_jname_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("jwrite_jname_done");

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = jwrite_jname_cstr_done;
  }

/************************************************************************/
/*			jwrite_jname_cstr_done				*/
/* write journal request is being decoded: journal name cstr done was	*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID jwrite_jname_cstr_done (ASN1_DEC_CTXT *aCtx)
  {

  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 1, jwrite_list_of_je_cstr);
  }

/************************************************************************/
/*			jwrite_list_of_je_cstr				*/
/* write journal request is being decoded: list of journal entry cstr	*/
/* was encountered.							*/
/************************************************************************/

static ST_VOID jwrite_list_of_je_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("jwrite_list_of_je_cstr");

  ASN1R_TAG_ADD (aCtx, UNI | CONSTR, SEQ_CODE, jwrite_jou_entry_cstr);
  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = jwrite_list_of_je_cstr_done;
  }

/************************************************************************/
/*			jwrite_jou_entry_cstr				*/
/* write journal request is being decoded: journal entry cstr was	*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID jwrite_jou_entry_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("jwrite_jou_entry_cstr");

  req_info->num_of_jou_entry++; 	/* increment # of journal entry */
  if (req_info->num_of_jou_entry > max_entries)
    {
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);
    return;
    }

  _ms_get_jou_ent_cont (aCtx, ec_ptr, max_data_vars, jwrite_je_done);
  }

/************************************************************************/
/*			jwrite_je_done					*/
/* write journal request is being decoded: journal entry has been	*/
/* obtained.								*/
/************************************************************************/

static ST_VOID jwrite_je_done (ASN1_DEC_CTXT *aCtx)
  {
VAR_INFO *vi_ptr;

  MLOG_CDEC0 ("jwrite_je_done");

				  /* point to where next will go	*/
  if (ec_ptr->entry_form_tag == 2 && ec_ptr->ef.data.list_of_var_pres)
    {
    vi_ptr = (VAR_INFO *) (ec_ptr + 1);
    ec_ptr = (ENTRY_CONTENT *) (vi_ptr + ec_ptr->ef.data.num_of_var);
    }
  else
    ec_ptr++;

  ASN1R_TAG_ADD (aCtx, UNI | CONSTR, SEQ_CODE, jwrite_jou_entry_cstr);
  }

/************************************************************************/
/*			jwrite_list_of_je_cstr_done			*/
/* write journal request is being decoded: list of journal entry cstr	*/
/* done was encountered.						*/
/************************************************************************/

static ST_VOID jwrite_list_of_je_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
ST_INT	 req_size;

				/* find actual size for request info	*/
  req_size = (int) ((ST_CHAR *) ec_ptr - (ST_CHAR *) req_info);

				/* give back unused storage		*/
  aCtx->mmsdec_rslt->data_ptr = (*m_realloc_os_fun) (aCtx->mmsdec_rslt->data_ptr, req_size);
  req_info = (JWRITE_REQ_INFO *) aCtx->mmsdec_rslt->data_ptr;


  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_jwrite_resp					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE and send the write journal response.		*/
/************************************************************************/

ST_RET mp_jwrite_resp (MMSREQ_IND *indptr)
  {
  return (_mms_send_null_resp (indptr,MMSOP_WRITE_JOURNAL));
  }

#else
/************************************************************************/
/************************************************************************/
/*			mpl_jwrite_resp					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the write journal response.			*/
/************************************************************************/

ST_RET mpl_jwrite_resp (ST_UINT32 invoke_id)
  {
  return (_mms_fin_null_resp (invoke_id,MMSOP_WRITE_JOURNAL));
  }
#endif
