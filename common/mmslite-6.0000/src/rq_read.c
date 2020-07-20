/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rq_read.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the requester portion of variable read.	*/
/*	It decodes the variable read response (confirm) & encodes	*/
/*	the variable read request.					*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 02/24/10  JRB	   Del _mms_dec_info, use aCtx->mmsdec_rslt->data_ptr*/
/* 03/11/04  GLB     08    Remove "thisFileName"                    	*/
/* 12/20/01  JRB     07    Converted to use ASN1R (re-entrant ASN1)	*/
/* 05/29/01  RKR     06    Corrected max_acc_rslt = _mmsdec_msglen / 2  */
/* 01/21/00  MDE     05    Corrected buffer size calculations for 	*/
/*			   'm_cl_max_read_acc_rslt' != 0		*/
/* 09/13/99  MDE     04    Added SD_CONST modifiers			*/
/* 03/23/99  MDE     03    Changes to decode buffer allocation scheme	*/
/* 06/15/98  MDE     02    Changes to allow compile under C++		*/
/* 06/27/97  MDE     01    Fixed max_acc_rslt bug			*/
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

static  READ_RESP_INFO	*rsp_info;
static ACCESS_RESULT  *ar_ptr;

static ST_INT	max_vars;
static ST_INT	max_acc_rslt;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

static ST_VOID read_acc_rslt_cstr (ASN1_DEC_CTXT *aCtx);
static ST_VOID read_acc_rslt_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID read_acc_rslt_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID read_va_spec_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID read_va_spec_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID read_va_spec_cstr (ASN1_DEC_CTXT *aCtx);


/************************************************************************/
/************************************************************************/
/*			mms_read_rsp					*/
/* This function is called from MMSDEC when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_read_rsp (ASN1_DEC_CTXT *aCtx)
  {
ST_INT	 rsp_size;
ST_INT	 rsp_size2;

  MLOG_CDEC0 ("Variable Read Response");

#if defined(FIXED_DECBUF)
  max_acc_rslt = m_cl_max_read_acc_rslt;
  max_vars = m_cl_max_read_var_spec;
  rsp_size = mmsl_dec_info_size;
#else
  max_vars     = _mmsdec_msglen / 4;
  max_acc_rslt = _mmsdec_msglen / 2;
  if (m_cl_max_read_acc_rslt && (max_acc_rslt > m_cl_max_read_acc_rslt))
    max_acc_rslt = m_cl_max_read_acc_rslt;

  if (m_cl_read_spec_in_result)
    {
    if (m_cl_max_read_var_spec && (max_vars > m_cl_max_read_var_spec))
      max_vars = m_cl_max_read_var_spec;
    rsp_size2 = sizeof(READ_RESP_INFO) + (sizeof(VARIABLE_LIST) * max_vars);
    }
  else
    rsp_size2 = 0;

  rsp_size = sizeof(READ_RESP_INFO) + (sizeof(ACCESS_RESULT) * max_acc_rslt);
  if (rsp_size < rsp_size2)
    rsp_size = rsp_size2;
#endif

/* get storage for request information	*/
  rsp_info = (READ_RESP_INFO *) _m_get_dec_buf (aCtx, rsp_size);
  rsp_info->va_spec_pres = SD_FALSE;		/* set default value	*/

  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 0, read_va_spec_cstr);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 1, read_acc_rslt_cstr);
  }

/************************************************************************/
/*			read_va_spec_cstr				*/
/* decoding variable read request: variable access constructor was	*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID read_va_spec_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("read_va_spec_cstr");

  rsp_info->va_spec_pres = SD_TRUE;		/* set va spec present	*/

  _ms_get_va_spec (aCtx, &rsp_info->va_spec, max_vars, read_va_spec_done);
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
/* decoding variable read request: variable access constructor done	*/
/* was encountered.							*/
/************************************************************************/

static ST_VOID read_va_spec_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 1, read_acc_rslt_cstr);
  }

/************************************************************************/
/*			read_acc_rslt_cstr				*/
/* decoding variable read request: access result constructor was	*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID read_acc_rslt_cstr (ASN1_DEC_CTXT *aCtx)
  {
VARIABLE_LIST *temp;

  MLOG_CDEC0 ("read_acc_rslt_cstr");

  temp	= (VARIABLE_LIST *) (rsp_info + 1);
  temp += rsp_info->va_spec.num_of_variables;

  rsp_info->num_of_acc_result = 0;
  rsp_info->acc_rslt_list = (ACCESS_RESULT *) temp;
  ar_ptr = rsp_info->acc_rslt_list;

  _ms_get_va_access_rslt (aCtx, ar_ptr, read_acc_rslt_done);
  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = read_acc_rslt_cstr_done;
  }

/************************************************************************/
/*			read_acc_rslt_done				*/
/* decoding variable read request: access result has been decoded.	*/
/************************************************************************/

static ST_VOID read_acc_rslt_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("read_acc_rslt_done");

  rsp_info->num_of_acc_result++;
  if ((rsp_info->num_of_acc_result + 1) <= max_acc_rslt)
    {
    ar_ptr++;
    _ms_get_va_access_rslt (aCtx, ar_ptr, read_acc_rslt_done);
    }
  }

/************************************************************************/
/*			read_acc_rslt_cstr_done 			*/
/* decoding variable read request: access result constructor done	*/
/* was encountered.							*/
/************************************************************************/

static ST_VOID read_acc_rslt_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
ST_INT	 rsp_size;
VARIABLE_LIST	*temp;

  MLOG_CDEC0 ("read_acc_rslt_cstr_done");

				/* find actual size for request info	*/
  rsp_size = sizeof (READ_RESP_INFO)
	     + (sizeof (VARIABLE_LIST)*rsp_info->va_spec.num_of_variables)
	     + (sizeof (ACCESS_RESULT)*rsp_info->num_of_acc_result);

				/* give back unused storage		*/
  aCtx->mmsdec_rslt->data_ptr = (*m_realloc_os_fun) (aCtx->mmsdec_rslt->data_ptr,rsp_size);
  rsp_info = (READ_RESP_INFO *) aCtx->mmsdec_rslt->data_ptr;

/* some C run time library mem mgmt. functions will always give back a	*/
/* new pointer when reallocating smaller.  In the case of read_resp_info*/
/* we have to update the acc_rslt_list ptr to point to this new mem loc	*/

  temp	= (VARIABLE_LIST *) (rsp_info + 1);
  temp += rsp_info->va_spec.num_of_variables;
  rsp_info->acc_rslt_list = (ACCESS_RESULT *) temp;

  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_read 					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE & send the variable read request.			*/
/************************************************************************/

MMSREQ_PEND *mp_read (ST_INT chan, READ_REQ_INFO *info_ptr)
  {
  return (_mms_req_send (MMSOP_READ,
		        M_CAST_MK_FUN (_ms_mk_read_req),
			chan,(ST_CHAR *)info_ptr));
  } 

#else
/************************************************************************/
/************************************************************************/
/*			mpl_read 					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the variable read request.				*/
/************************************************************************/


ST_RET  mpl_read (READ_REQ_INFO *info_ptr)
  {
  return (_mms_req_fin (MMSOP_READ,
		        M_CAST_MK_FUN (_ms_mk_read_req),
		        (ST_CHAR *) info_ptr));
  } 
#endif

  

/************************************************************************/
/************************************************************************/
/*			_ms_mk_read_req					*/
/* construct a variable read request.					*/
/************************************************************************/

ST_VOID _ms_mk_read_req (ASN1_ENC_CTXT *aCtx, READ_REQ_INFO *info)
  {

  asn1r_strt_constr (aCtx);			/* start context tag 1 cstr	*/
  _ms_wr_va_spec (aCtx, &info->va_spec);
  asn1r_fin_constr (aCtx, 1, CTX, DEF);		/* finish context tag 1 cstr	*/

  if (info->spec_in_result)
    {					/* not the default (false)	*/
    asn1r_wr_bool (aCtx, info->spec_in_result);	/* write specification in rslt	*/
    asn1r_fin_prim (aCtx, 0, CTX);			/* context tag 0 primitive	*/
    }

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_constr (aCtx, MMSOP_READ,CTX,DEF);     /* tag = opcode, ctx        */
  }
