/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rq_getcl.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the requester portion of get capability	*/
/*	list.  It decodes the get capability list response (confirm) & 	*/
/*	encodes the get capability list request.			*/
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
#include "mms_pvmd.h"
#include "asn1defs.h"

/************************************************************************/
/* variables global to the DECODE portion : NONE			*/

static  GETCL_RESP_INFO	*rsp_info;

static ST_CHAR		       **cap_ptr;
static ST_INT max_capabilities;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

static ST_VOID getcl_cap_cstr (ASN1_DEC_CTXT *aCtx);
static ST_VOID getcl_get_cap (ASN1_DEC_CTXT *aCtx);
static ST_VOID getcl_cap_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID getcl_get_more_follows (ASN1_DEC_CTXT *aCtx);


/************************************************************************/
/************************************************************************/
/*			mms_getcl_rsp					*/
/* This function is called from MMSDEC when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_getcl_rsp (ASN1_DEC_CTXT *aCtx)
  {
ST_INT rsp_size;

  MLOG_CDEC0 ("Get Capability List Response");

#if defined(FIXED_DECBUF)
  max_capabilities = m_cl_max_getcl_cap;
  rsp_size = mmsl_dec_info_size;
#else
  /* determine maximum size	*/
  max_capabilities = _mmsdec_msglen/2;
  if (m_cl_max_getcl_cap && (max_capabilities > m_cl_max_getcl_cap))
    max_capabilities = m_cl_max_getcl_cap;

/* get storage for resp info	*/
  rsp_size = sizeof (GETCL_RESP_INFO)
		+ (max_capabilities * (sizeof (ST_CHAR *)));
#endif

/* get storage for request information	*/
  rsp_info = (GETCL_RESP_INFO *) _m_get_dec_buf (aCtx, rsp_size);
  cap_ptr  = (ST_CHAR **) (rsp_info + 1);
  rsp_info->more_follows = SD_TRUE;
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 0, getcl_cap_cstr);
  }

/************************************************************************/
/*			getcl_cap_cstr					*/
/* get capability list RESPONSE (confirm) is being decoded:		*/
/* capability list constructor was encountered. 			*/
/************************************************************************/

static ST_VOID getcl_cap_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("getcl_cap_cstr");

  ASN1R_TAG_ADD (aCtx, UNI, VISTR_CODE, getcl_get_cap);
  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = getcl_cap_cstr_done;
  aCtx->asn1r_decode_done_fun        = _mms_dec_done_ok;
  }

/************************************************************************/
/*			getcl_get_cap 					*/
/* get capability list RESPONSE (confirm) is being decoded:		*/
/* capability was encountered.						*/
/************************************************************************/

static ST_VOID getcl_get_cap (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("getcl_get_cap");

  rsp_info->num_of_capab++;		/* increment # of capabilities	*/
  if (rsp_info->num_of_capab > max_capabilities)
    {
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);
    return;
    }

	/* copy capability back onto itself shifted by one character	*/
  *cap_ptr = (ST_CHAR *) aCtx->asn1r_field_ptr - 1;
  if (asn1r_get_vstr (aCtx, *cap_ptr))
    {
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);
    return;
    }
  cap_ptr++;

  ASN1R_TAG_ADD (aCtx, UNI, VISTR_CODE, getcl_get_cap);
  }

/************************************************************************/
/*			getcl_cap_cstr_done				*/
/* get capability list RESPONSE (confirm) is being decoded:		*/
/* capability list constructor done was encountered.			*/
/************************************************************************/

static ST_VOID getcl_cap_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
ST_INT rsp_size;

				/* find actual size for response info	*/
/* OK to be done, or may have more follows indicator			*/
  rsp_size = sizeof (GETCL_RESP_INFO)
		+ (sizeof (ST_CHAR *) * rsp_info->num_of_capab);

				/* give back unused aCtx->mmsdec_rslt->data_ptr;	*/
  aCtx->mmsdec_rslt->data_ptr = (*m_realloc_os_fun) (aCtx->mmsdec_rslt->data_ptr,rsp_size);
  rsp_info = (GETCL_RESP_INFO *) aCtx->mmsdec_rslt->data_ptr;

  ASN1R_TAG_ADD (aCtx, CTX,1,getcl_get_more_follows);
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }

/************************************************************************/
/*			getcl_get_more_follows				*/
/* get capability list RESPONSE (confirm) is being decoded:		*/
/* more follows was encountered.					*/
/************************************************************************/

static ST_VOID getcl_get_more_follows (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("getcl_get_more_follows");

  if (asn1r_get_bool (aCtx, &rsp_info->more_follows))
    {
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);
    return;
    }

  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }

#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_getcl					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE and send the get capability list request.		*/
/************************************************************************/

MMSREQ_PEND *mp_getcl (ST_INT chan, GETCL_REQ_INFO *info_ptr)
  {
  return (_mms_req_send (MMSOP_GET_CAP_LIST,
			M_CAST_MK_FUN (_ms_mk_getcl_req),
			chan,(ST_CHAR *)info_ptr));
  } 

#else
/************************************************************************/
/************************************************************************/
/*			mpl_getcl					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the get capability list request.			*/
/************************************************************************/

ST_RET mpl_getcl (GETCL_REQ_INFO *info_ptr)
  {
  return (_mms_req_fin (MMSOP_GET_CAP_LIST,
			M_CAST_MK_FUN (_ms_mk_getcl_req),
			(ST_CHAR *)info_ptr));
  } 
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_getcl_req					*/
/* ENCODE a get capability list REQUEST:				*/
/************************************************************************/

ST_VOID _ms_mk_getcl_req (ASN1_ENC_CTXT *aCtx, GETCL_REQ_INFO *info)
  {

  if (info->cont_after_pres)
    {
    asn1r_wr_vstr (aCtx, info->continue_after);	/* write the continue after	*/
    asn1r_fin_prim (aCtx, VISTR_CODE,UNI);
    }

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_constr (aCtx, MMSOP_GET_CAP_LIST,CTX,DEF);	/* tag = opcode, context	*/
  }
