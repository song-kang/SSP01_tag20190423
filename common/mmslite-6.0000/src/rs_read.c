/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rs_read.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the responder portion of variable read.	*/
/*	It decodes the variable read request (indication) & encodes	*/
/*	the variable read response.					*/
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
/* 03/23/99  MDE     03    Changes to decode buffer allocation scheme   */
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

static  READ_REQ_INFO	*req_info;
static ST_INT	max_vars;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

static ST_VOID read_va_spec_cstr (ASN1_DEC_CTXT *aCtx);
static ST_VOID read_va_spec_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID read_va_spec_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID read_get_spec_in_rslt (ASN1_DEC_CTXT *aCtx);

/************************************************************************/
/************************************************************************/
/*			mms_read_req					*/
/* This function is called from MMSDEC when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_read_req (ASN1_DEC_CTXT *aCtx)
  {
ST_INT	 req_size;

  MLOG_CDEC0 ("Variable Read Request");

#if defined(FIXED_DECBUF)
  max_vars = m_sv_max_read_var_spec;
  req_size = mmsl_dec_info_size;
#else
  max_vars = _mmsdec_msglen / 4; /* find maximum storage required	*/
  if (m_sv_max_read_var_spec && (max_vars > m_sv_max_read_var_spec))
    max_vars = m_sv_max_read_var_spec;
  req_size = sizeof (READ_REQ_INFO) + (sizeof (VARIABLE_LIST) * max_vars);
#endif

/* get storage for request information	*/
  req_info = (READ_REQ_INFO *) _m_get_dec_buf (aCtx, req_size);
  req_info->spec_in_result = SD_FALSE;		/* set default value	*/
  ASN1R_TAG_ADD (aCtx, CTX, 	 0, read_get_spec_in_rslt);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 1, read_va_spec_cstr);
  }

/************************************************************************/
/*			read_get_spec_in_rslt				*/
/* decoding variable read request: specification in result was		*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID read_get_spec_in_rslt (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("read_get_spec_in_rslt");

  req_info->spec_in_result = SD_TRUE;	/* specification in rslt pres	*/
  if (asn1r_get_bool (aCtx, &req_info->spec_in_result))
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);

  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 1, read_va_spec_cstr);
  }

/************************************************************************/
/*			read_va_spec_cstr				*/
/* decoding variable read request: variable access specification	*/
/* constructor was encountered. 					*/
/************************************************************************/

static ST_VOID read_va_spec_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("read_va_spec_cstr");

  _ms_get_va_spec (aCtx, &req_info->va_spec, max_vars, read_va_spec_done);
  }

/************************************************************************/
/*			read_va_spec_done				*/
/* decoding variable read request: variable access specification	*/
/* has been decoded.							*/
/************************************************************************/

static ST_VOID read_va_spec_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("read_va_spec_done");

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = read_va_spec_cstr_done;
  }

/************************************************************************/
/*			read_va_spec_cstr_done				*/
/* decoding variable read request: variable access specification	*/
/* constructor done was encountered.					*/
/************************************************************************/

static ST_VOID read_va_spec_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
ST_INT	 req_size;

				/* find actual size for request info	*/
  req_size = sizeof (READ_REQ_INFO)
	     + (sizeof (VARIABLE_LIST)*req_info->va_spec.num_of_variables);

				/* give back unused storage		*/
  aCtx->mmsdec_rslt->data_ptr = (*m_realloc_os_fun) (aCtx->mmsdec_rslt->data_ptr,req_size);
  req_info = (READ_REQ_INFO *) aCtx->mmsdec_rslt->data_ptr;
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_read_resp					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE & send the variable read response. 		*/
/************************************************************************/

ST_RET mp_read_resp (MMSREQ_IND *indptr, READ_RESP_INFO *info_ptr)
  {
  return (_mms_resp_send (MMSOP_READ,indptr,
			M_CAST_MK_FUN (_ms_mk_read_resp),
			(ST_CHAR *)info_ptr));
  }

#else
/************************************************************************/
/************************************************************************/
/*			mpl_read_resp					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the variable read response. 			*/
/************************************************************************/

ST_RET mpl_read_resp (ST_UINT32 invoke_id, READ_RESP_INFO *info_ptr)
  {
  return (_mms_resp_fin (MMSOP_READ,invoke_id,
			M_CAST_MK_FUN (_ms_mk_read_resp),
			(ST_CHAR *)info_ptr));
  }
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_read_resp					*/
/* Construct a variable read response.					*/
/************************************************************************/

ST_VOID _ms_mk_read_resp (ASN1_ENC_CTXT *aCtx, READ_RESP_INFO *info)
  {
ST_INT i;
ACCESS_RESULT *ar_ptr;

  asn1r_strt_constr (aCtx);			/* start context tag 1 cstr	*/
  ar_ptr  = info->acc_rslt_list;
  ar_ptr += info->num_of_acc_result - 1;
  for (i = 0; i < info->num_of_acc_result; i++)
    {
    _ms_wr_va_access_rslt (aCtx, ar_ptr);
    ar_ptr--;
    }
  asn1r_fin_constr (aCtx, 1,CTX, DEF);		/* finish context tag 1 cstr	*/

  if (info->va_spec_pres)
    {
    asn1r_strt_constr (aCtx);			/* start context tag 0 cstr	*/
    _ms_wr_va_spec (aCtx, &info->va_spec);
    asn1r_fin_constr (aCtx, 0, CTX, DEF);		/* finish context tag 0 cstr	*/
    }

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_constr (aCtx, MMSOP_READ,CTX,DEF);     /* tag = opcode, ctx constr */
  }
