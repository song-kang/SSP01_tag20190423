/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rs_write.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the responder portion of variable write.	*/
/*	It decodes the variable write request (indication) & encodes	*/
/*	the variable write response.					*/
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

static  WRITE_REQ_INFO	*req_info;

static VAR_ACC_DATA   *data_ptr;
static ST_INT	 max_vars;
static ST_INT	 max_data_count;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

static ST_VOID write_data_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID write_data_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID write_data_cstr (ASN1_DEC_CTXT *aCtx);
static ST_VOID write_va_spec_done (ASN1_DEC_CTXT *aCtx);


/************************************************************************/
/************************************************************************/
/*			mms_write_req					*/
/* This function is called from MMSDEC when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_write_req (ASN1_DEC_CTXT *aCtx)
  {
ST_INT	 req_size;
ST_INT	 req_size1;
ST_INT	 req_size2;

  MLOG_CDEC0 ("Variable Write Request");

#if defined(FIXED_DECBUF)
  max_vars = m_sv_max_write_var_spec;
  max_data_count = m_sv_max_write_data;
  req_size = mmsl_dec_info_size;
#else
  max_vars	 = _mmsdec_msglen / 7;
  max_data_count = _mmsdec_msglen / 3;

  if (m_sv_max_write_var_spec && (max_vars > m_sv_max_write_var_spec))
    max_vars = m_sv_max_write_var_spec;

  if (m_sv_max_write_data && (max_data_count > m_sv_max_write_data))
    max_data_count = m_sv_max_write_data;

/* Case of list of variables */
  req_size1 = (sizeof(VARIABLE_LIST) + sizeof(VAR_ACC_DATA)) * max_vars;

/* Case of named variable list */
  req_size2 =  sizeof (VAR_ACC_DATA) * max_data_count;

/* Choose the largest */
  req_size = sizeof (WRITE_REQ_INFO) + max (req_size1, req_size2);
#endif

/* get storage for request information	*/
  req_info = (WRITE_REQ_INFO *) _m_get_dec_buf (aCtx, req_size);
  _ms_get_va_spec (aCtx, &req_info->va_spec, max_vars, write_va_spec_done);
  }

/************************************************************************/
/*			write_va_spec_done				*/
/* decoding variable write request: variable access specification	*/
/* has been decoded.							*/
/************************************************************************/

static ST_VOID write_va_spec_done (ASN1_DEC_CTXT *aCtx)
  {
VARIABLE_LIST *temp;

  MLOG_CDEC0 ("write_va_spec_done");

  temp	= (VARIABLE_LIST *) (req_info + 1);
  temp += req_info->va_spec.num_of_variables;

  req_info->va_data = (VAR_ACC_DATA *) temp;

  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 0, write_data_cstr);
  }

/************************************************************************/
/*			write_data_cstr 				*/
/* decoding variable write request: write data constructor was		*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID write_data_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("write_data_cstr");

  data_ptr = req_info->va_data;

  _ms_get_va_data (aCtx, data_ptr, write_data_done);
  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = write_data_cstr_done;
  }

/************************************************************************/
/*			write_data_done 				*/
/* decoding variable write request: write data has been decoded.	*/
/************************************************************************/

static ST_VOID write_data_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("write_data_done");

  req_info->num_of_data++;
  if ((req_info->num_of_data + 1) <= max_data_count)
    {
    data_ptr++;
    _ms_get_va_data (aCtx, data_ptr, write_data_done);
    }
  }

/************************************************************************/
/*			write_data_cstr_done				*/
/* decoding variable write request: write data constructor done was	*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID write_data_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
ST_INT	 req_size;
VARIABLE_LIST *temp;

  MLOG_CDEC0 ("write_data_cstr_done");
				/* find actual size for request info	*/
  req_size = sizeof (WRITE_REQ_INFO)
	     + (sizeof (VARIABLE_LIST)*req_info->va_spec.num_of_variables)
	     + (sizeof (VAR_ACC_DATA) *req_info->num_of_data);

				/* give back unused storage		*/
  aCtx->mmsdec_rslt->data_ptr = (*m_realloc_os_fun) (aCtx->mmsdec_rslt->data_ptr,req_size);
  req_info = (WRITE_REQ_INFO *) aCtx->mmsdec_rslt->data_ptr;

/* some C run time library mem mgmt. functions will always give back a  */
/* new pointer when reallocating smaller.  In the case of write_req_info */
/* we have to update the va_data to point to this new mem loc */
  temp	= (VARIABLE_LIST *) (req_info + 1);
  temp += req_info->va_spec.num_of_variables;

  req_info->va_data = (VAR_ACC_DATA *) temp;

  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_write_resp					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE & send the variable write response.		*/
/************************************************************************/

ST_RET mp_write_resp (MMSREQ_IND *indptr, WRITE_RESP_INFO *info_ptr)
  {
  return (_mms_resp_send (MMSOP_WRITE,indptr,
			M_CAST_MK_FUN (_ms_mk_write_resp),
			(ST_CHAR *) info_ptr));
  }

#else
/************************************************************************/
/************************************************************************/
/*			mpl_write_resp					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE & send the variable write response.		*/
/************************************************************************/

ST_RET mpl_write_resp (ST_UINT32 invoke_id, WRITE_RESP_INFO *info_ptr)
  {
  return (_mms_resp_fin (MMSOP_WRITE,invoke_id,
			M_CAST_MK_FUN (_ms_mk_write_resp),
			(ST_CHAR *) info_ptr));
  }
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_write_resp					*/
/* Construct a variable write response. 				*/
/************************************************************************/

ST_VOID _ms_mk_write_resp (ASN1_ENC_CTXT *aCtx, WRITE_RESP_INFO *info)
  {
ST_INT i;
WRITE_RESULT	*temp;		/* index into array of structs	*/

  temp = (WRITE_RESULT *) (info + 1);
  temp += (info->num_of_result - 1);
  for (i = info->num_of_result; i ; --i)
    {
    if (temp->resp_tag == SD_TRUE)
      { 				  /* success, null response	  */
      asn1r_fin_prim (aCtx, 1, CTX);		  /* finish context tag 1 prim	  */
      }
    else
      { 				  /* failure, write failure code  */
      asn1r_wr_i16 (aCtx, temp->failure);
      asn1r_fin_prim (aCtx, 0, CTX);		  /* finish context tag 0 prim	  */
      }
    temp--;
    }

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_constr (aCtx, MMSOP_WRITE,CTX,DEF);   	/* tag = opcode, ctx constr	*/
  }
