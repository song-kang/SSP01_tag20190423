/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rq_write.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the requester portion of variable write.	*/
/*	It decodes the variable write response (confirm) & encodes	*/
/*	the variable write request.					*/
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

static  WRITE_RESP_INFO	*rsp_info;

static WRITE_RESULT    *rslt_ptr;
static ST_INT  max_rslts;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

static ST_VOID write_get_failure (ASN1_DEC_CTXT *aCtx);
static ST_VOID write_get_success (ASN1_DEC_CTXT *aCtx);
static ST_VOID write_all_done (ASN1_DEC_CTXT *aCtx);

/************************************************************************/
/************************************************************************/
/*			mms_write_rsp					*/
/* This function is called from MMSDEC when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_write_rsp (ASN1_DEC_CTXT *aCtx)
  {
ST_INT	rsp_size;

  MLOG_CDEC0 ("Variable Write Response");

#if defined(FIXED_DECBUF)
  max_rslts = m_cl_max_write_rslts;
  rsp_size = mmsl_dec_info_size;
#else
  max_rslts = _mmsdec_msglen / 2;   /* find maximum storage required	*/
  if (m_cl_max_write_rslts && (max_rslts > m_cl_max_write_rslts))
    max_rslts = m_cl_max_write_rslts;

  rsp_size  = sizeof (WRITE_RESP_INFO) + (sizeof (WRITE_RESULT) * max_rslts);
#endif

/* get storage for request information	*/
  rsp_info = (WRITE_RESP_INFO *) _m_get_dec_buf (aCtx, rsp_size);

  rsp_info->num_of_result = 0;
  rslt_ptr = (WRITE_RESULT *) (rsp_info + 1);

  ASN1R_TAG_ADD (aCtx, CTX, 0, write_get_failure);
  ASN1R_TAG_ADD (aCtx, CTX, 1, write_get_success);
  aCtx->asn1r_decode_done_fun = write_all_done;
  }

/************************************************************************/
/*			write_get_failure				*/
/* Decoding variable write response: failure was encountered.		*/
/************************************************************************/

static ST_VOID write_get_failure (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("write_get_failure");

  if (++rsp_info->num_of_result > max_rslts)
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);
  rslt_ptr->resp_tag = SD_FALSE;		/* set tag to failure		*/

  if (asn1r_get_i16 (aCtx, &(rslt_ptr->failure)))	/* read the failure code	*/
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);

  rslt_ptr++;				/* point at the next element	*/
  ASN1R_TAG_ADD (aCtx, CTX, 0, write_get_failure);
  ASN1R_TAG_ADD (aCtx, CTX, 1, write_get_success);
  _ms_set_cs_check2 (aCtx);		/* check for Companion Standard info	*/
  }

/************************************************************************/
/*			write_get_success				*/
/* Decoding variable write response: success was encountered.		*/
/************************************************************************/

static ST_VOID write_get_success (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("write_get_success");

  if (++rsp_info->num_of_result > max_rslts)
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);
  rslt_ptr->resp_tag = SD_TRUE;		/* set tag to success		*/

  if (aCtx->asn1r_elmnt_len != SD_FALSE) 		/* verify the null		*/
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);

  rslt_ptr++;				/* point at the next element	*/
  ASN1R_TAG_ADD (aCtx, CTX, 0, write_get_failure);
  ASN1R_TAG_ADD (aCtx, CTX, 1, write_get_success);
  _ms_set_cs_check2 (aCtx);		/* check for Companion Standard info	*/
  }

/************************************************************************/
/*			write_all_done					*/
/* All of the write response has been obtained, give back unused 	*/
/* storage.								*/
/************************************************************************/

static ST_VOID write_all_done (ASN1_DEC_CTXT *aCtx)
  {
ST_INT	 rsp_size;

				/* find actual size for request info	*/
  rsp_size = sizeof (WRITE_RESP_INFO)
	     + (sizeof (WRITE_RESULT) * rsp_info->num_of_result);

				/* give back unused storage		*/
  aCtx->mmsdec_rslt->data_ptr = (*m_realloc_os_fun) (aCtx->mmsdec_rslt->data_ptr, rsp_size);
  rsp_info = (WRITE_RESP_INFO *) aCtx->mmsdec_rslt->data_ptr;

  _mms_dec_done_ok (aCtx);
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_write					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE & send the variable write request. 		*/
/************************************************************************/

MMSREQ_PEND *mp_write (ST_INT chan, WRITE_REQ_INFO *info_ptr)
  {
  return (_mms_req_send (MMSOP_WRITE,
			M_CAST_MK_FUN (_ms_mk_write_req),
			chan,(ST_CHAR *)info_ptr));
  } 

#else
/************************************************************************/
/************************************************************************/
/*			mpl_write					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the variable write request. 			*/
/************************************************************************/

ST_RET  mpl_write (WRITE_REQ_INFO *info_ptr)
  {
  return (_mms_req_fin (MMSOP_WRITE,
			M_CAST_MK_FUN (_ms_mk_write_req),
			(ST_CHAR *)info_ptr));
  } 
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_write_req					*/
/* construct a variable write request.					*/
/************************************************************************/

ST_VOID _ms_mk_write_req (ASN1_ENC_CTXT *aCtx, WRITE_REQ_INFO *info)
  {
ST_INT i;
VAR_ACC_DATA *data_ptr;

  asn1r_strt_constr (aCtx);			/* start context tag 0 cstr	*/
  data_ptr  = info->va_data;
  data_ptr += (info->num_of_data - 1);
  for (i = 0; i < info->num_of_data; i++)
    {
    _ms_wr_va_data (aCtx, data_ptr);
    data_ptr--;
    }
  asn1r_fin_constr (aCtx, 0, CTX, DEF);		/* finish context tag 0 cstr	*/

  _ms_wr_va_spec (aCtx, &info->va_spec);

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_constr (aCtx, MMSOP_WRITE,CTX,DEF);        /* tag = opcode, ctx        */
  }
