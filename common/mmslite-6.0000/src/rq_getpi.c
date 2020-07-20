/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rq_getpi.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the requester portion of get program	*/
/*	invocation attribute.  It decodes the get program invocation	*/
/*	attribute response (confirm) & encodes the get program		*/
/*	invocation attribute request.					*/
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
#include "mms_pprg.h"
#include "asn1defs.h"

/************************************************************************/
/* variables global to the DECODE portion :				*/

static  GETPI_RESP_INFO	*rsp_info;

static ST_CHAR		     **dom_ptr;
static ST_INT  max_domain_names;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

static ST_VOID getpi_get_monitor (ASN1_DEC_CTXT *aCtx);
static ST_VOID getpi_get_start_arg (ASN1_DEC_CTXT *aCtx);
static ST_VOID getpi_domain_cstr (ASN1_DEC_CTXT *aCtx);
static ST_VOID getpi_get_reusable (ASN1_DEC_CTXT *aCtx);
static ST_VOID getpi_get_mms_deletable (ASN1_DEC_CTXT *aCtx);
static ST_VOID getpi_get_domain_name (ASN1_DEC_CTXT *aCtx);
static ST_VOID getpi_domain_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID getpi_get_state (ASN1_DEC_CTXT *aCtx);
static ST_VOID extern_done (ASN1_DEC_CTXT *aCtx);


/************************************************************************/
/************************************************************************/
/*			mms_get_pi_rsp					*/
/* This function is called from MMSDEC when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_get_pi_rsp (ASN1_DEC_CTXT *aCtx)
  {
ST_INT	 rsp_size;

  MLOG_CDEC0 ("Get Program Invocation Attribute Response");

#if defined(FIXED_DECBUF)
  max_domain_names = m_cl_max_getpi_doms;
  rsp_size = mmsl_dec_info_size;
#else
  /* determine maximum size	*/
  max_domain_names = _mmsdec_msglen/2;
  if (m_cl_max_getpi_doms && (max_domain_names > m_cl_max_getpi_doms))
    max_domain_names = m_cl_max_getpi_doms;

  /* get storage for resp info	*/
  rsp_size = sizeof (GETPI_RESP_INFO) + (max_domain_names * (sizeof (ST_CHAR *)));
#endif

/* get storage for request information	*/
  rsp_info = (GETPI_RESP_INFO *) _m_get_dec_buf (aCtx, rsp_size);
  dom_ptr  = (ST_CHAR **) (rsp_info + 1);
  ASN1R_TAG_ADD (aCtx, CTX, 0, getpi_get_state);  	/* state is next	*/
  }

/************************************************************************/
/*			getpi_get_state 				*/
/* get program invocation attribute RESPONSE (confirm) is being decoded:*/
/* decode the state.							*/
/************************************************************************/

static ST_VOID getpi_get_state (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("getpi_get_state");

  if (asn1r_get_i16 (aCtx, &(rsp_info->state))) 	/* read state from the message	*/
    {
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);
    return;
    }
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 1, getpi_domain_cstr);
  }

/************************************************************************/
/*			getpi_domain_cstr				*/
/* get program invocation attribute RESPONSE (confirm) is being decoded:*/
/* domain name list constructor was encountered.			*/
/************************************************************************/

static ST_VOID getpi_domain_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("getpi_domain_cstr");

  ASN1R_TAG_ADD (aCtx, UNI, VISTR_CODE, getpi_get_domain_name);
  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = getpi_domain_cstr_done;
  }

/************************************************************************/
/*			getpi_get_domain_name				*/
/* get program invocation attribute RESPONSE (confirm) is being decoded:*/
/* domain name was encountered. 					*/
/************************************************************************/

static ST_VOID getpi_get_domain_name (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("getpi_get_domain_name");

  rsp_info->num_of_dnames++;		/* increment # of domain names	*/
  if (rsp_info->num_of_dnames > max_domain_names)
    {					/* too many domain names	*/
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);
    return;
    }

  if (aCtx->asn1r_elmnt_len > MAX_IDENT_LEN)
    {					/* domain name is too ST_INT32	*/
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);
    return;
    }

	/* copy domain name back onto itself shifted by one character	*/
  *dom_ptr = (ST_CHAR *) aCtx->asn1r_field_ptr - 1;
  if (asn1r_get_identifier (aCtx, *dom_ptr))
    {
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);
    return;
    }
  dom_ptr++;

  ASN1R_TAG_ADD (aCtx, UNI, VISTR_CODE, getpi_get_domain_name);
  }

/************************************************************************/
/*			getpi_domain_cstr_done				*/
/* get program invocation attribute RESPONSE (confirm) is being decoded:*/
/* domain name list constructor done was encountered.			*/
/************************************************************************/

static ST_VOID getpi_domain_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
ST_INT rsp_size;

  rsp_size = sizeof (GETPI_RESP_INFO)
		+ (sizeof (ST_CHAR *) * rsp_info->num_of_dnames);

				/* give back unused storage		*/
  aCtx->mmsdec_rslt->data_ptr = (*m_realloc_os_fun) (aCtx->mmsdec_rslt->data_ptr,rsp_size);
  rsp_info = (GETPI_RESP_INFO *) aCtx->mmsdec_rslt->data_ptr;


  ASN1R_TAG_ADD (aCtx, CTX, 2, getpi_get_mms_deletable);
  }

/************************************************************************/
/*			getpi_get_mms_deletable 			*/
/* get program invocation attribute RESPONSE (confirm) is being decoded:*/
/* decode the mms deletable flag.					*/
/************************************************************************/

static ST_VOID getpi_get_mms_deletable (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("getpi_get_mms_deletable");

  if (asn1r_get_bool (aCtx, &(rsp_info->mms_deletable)))	/* read mms deletable	*/
    {
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);
    return;
    }
  ASN1R_TAG_ADD (aCtx, CTX, 3, getpi_get_reusable);
  }

/************************************************************************/
/*			getpi_get_reusable 				*/
/* get program invocation attribute RESPONSE (confirm) is being decoded:*/
/* decode the mms reusable flag.					*/
/************************************************************************/

static ST_VOID getpi_get_reusable (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("getpi_get_reusable");

  if (asn1r_get_bool (aCtx, &(rsp_info->reusable)))	/* read mms deletable	*/
    {
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);
    return;
    }
  ASN1R_TAG_ADD (aCtx, CTX, 4, getpi_get_monitor);
  }

/************************************************************************/
/*			getpi_get_monitor				*/
/* get program invocation attribute RESPONSE (confirm) is being decoded:*/
/* decode the monitor flag.						*/
/************************************************************************/

static ST_VOID getpi_get_monitor (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("getpi_get_monitor");

  if (asn1r_get_bool (aCtx, &(rsp_info->monitor)))	/* read monitor from message	*/
    {
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);
    return;
    }
  ASN1R_TAG_ADD (aCtx, CTX, 5, getpi_get_start_arg);

/* If MMS IS, Check for ENCODED string					*/
#ifndef MMS_LITE
  if (mms_chan_info[_mmsdechan].version)		/* IS 	*/
#else
  if (mmsl_version)				/* IS 	*/
#endif
    _ms_chk_for_extern (aCtx, &rsp_info->start_arg_len,
    		    &rsp_info->start_arg,
		    extern_done);
  }

/************************************************************************/
/*			getpi_get_start_arg				*/
/* get program invocation attribute RESPONSE (confirm) is being decoded:*/
/* decode the start argument.						*/
/************************************************************************/

static ST_VOID getpi_get_start_arg (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("getpi_get_start_arg");

  rsp_info->start_arg = (ST_UCHAR *) aCtx->asn1r_field_ptr - 1;
  rsp_info->start_arg_type = ARG_TYPE_SIMPLE;

  if (asn1r_get_vstr (aCtx, (ST_CHAR *) rsp_info->start_arg))
    {
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);
    return;
    }

  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }

/************************************************************************/
/*			   extern_done					*/
/************************************************************************/

static ST_VOID extern_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("extern_done");

  rsp_info->start_arg_type = ARG_TYPE_ENCODED;
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_getpi					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE and send the get program invocation attrubute	*/
/* request.								*/
/************************************************************************/

MMSREQ_PEND *mp_getpi (ST_INT chan, GETPI_REQ_INFO *info_ptr)
  {
  return (_mms_req_send (MMSOP_GET_PI_ATTR,
			M_CAST_MK_FUN (_ms_mk_getpi_req),
			chan,(ST_CHAR *)info_ptr));
  } 

#else
/************************************************************************/
/************************************************************************/
/*			mpl_getpi					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the get program invocation attrubute request.	*/
/************************************************************************/

ST_RET  mpl_getpi (GETPI_REQ_INFO *info_ptr)
  {
  return (_mms_req_fin (MMSOP_GET_PI_ATTR,
			M_CAST_MK_FUN (_ms_mk_getpi_req),
			(ST_CHAR *)info_ptr));
  } 
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_getpi_req					*/
/* ENCODE a get program invocation attribute REQUEST:			*/
/************************************************************************/

ST_VOID _ms_mk_getpi_req (ASN1_ENC_CTXT *aCtx, GETPI_REQ_INFO *info)
  {

  asn1r_wr_vstr (aCtx, info->piname);		/* write pgm invocation name	*/

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_prim (aCtx, MMSOP_GET_PI_ATTR,CTX);		/* tag = opcode, context	*/
  }
