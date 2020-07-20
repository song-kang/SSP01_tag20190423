/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rq_initupl.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the requester portion of initiate upload	*/
/*	sequence.  It decodes the initiate upload sequence response	*/
/*	(confirm) & encodes the initiate upload sequence request.	*/
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
#include "mms_pdom.h"
#include "asn1defs.h"

/************************************************************************/
/* variables global to the DECODE portion : NONE			*/

static  INITUPL_RESP_INFO	*rsp_info;

static ST_CHAR		       **cap_ptr;
static ST_INT  max_capabilities;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

static ST_VOID initupl_get_cap (ASN1_DEC_CTXT *aCtx);
static ST_VOID initupl_cap_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID initupl_cap_cstr (ASN1_DEC_CTXT *aCtx);
static ST_VOID initupl_get_ulsmid (ASN1_DEC_CTXT *aCtx);


/************************************************************************/
/************************************************************************/
/*			mms_init_upload_rsp				*/
/* This function is called from MMSDEC when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_init_upload_rsp (ASN1_DEC_CTXT *aCtx)
  {
ST_INT rsp_size;

  MLOG_CDEC0 ("Initiate Upload Sequence Response");

#if defined(FIXED_DECBUF)
  max_capabilities = m_cl_max_initupl_cap;
  rsp_size = mmsl_dec_info_size;
#else
  max_capabilities = _mmsdec_msglen/2;	     /* determine maximum size	*/
  if (m_cl_max_initupl_cap && (max_capabilities > m_cl_max_initupl_cap))
    max_capabilities = m_cl_max_initupl_cap; 
  rsp_size = sizeof (INITUPL_RESP_INFO)
		+ (max_capabilities * (sizeof (ST_CHAR *)));
#endif

/* get storage for request information	*/
  rsp_info = (INITUPL_RESP_INFO *) _m_get_dec_buf (aCtx, rsp_size);
  cap_ptr  = (ST_CHAR **) (rsp_info + 1);
  ASN1R_TAG_ADD (aCtx, CTX, 0, initupl_get_ulsmid); 	/* ulsm id is next	*/
  }

/************************************************************************/
/*			initupl_get_ulsmid				*/
/* initiate upload sequence RESPONSE (confirm) is being decoded:	*/
/* decode the upload state machine id.					*/
/************************************************************************/

static ST_VOID initupl_get_ulsmid (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("initupl_get_ulsmid");

  if (asn1r_get_i32 (aCtx, &(rsp_info->ulsmid)))	/* get upload state machine id	*/
    {
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);
    return;
    }

  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 1, initupl_cap_cstr);
  }

/************************************************************************/
/*			initupl_cap_cstr				*/
/* initiate upload sequence RESPONSE (confirm) is being decoded:	*/
/* capability list constructor was encountered. 			*/
/************************************************************************/

static ST_VOID initupl_cap_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("initupl_cap_cstr");

  ASN1R_TAG_ADD (aCtx, UNI, VISTR_CODE, initupl_get_cap);
  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = initupl_cap_cstr_done;
  }

/************************************************************************/
/*			initupl_get_cap 				*/
/* initiate upload sequence RESPONSE (confirm) is being decoded:	*/
/* capability was encountered.						*/
/************************************************************************/

static ST_VOID initupl_get_cap (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("initupl_get_cap");

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

  ASN1R_TAG_ADD (aCtx, UNI, VISTR_CODE, initupl_get_cap);
  }

/************************************************************************/
/*			initupl_cap_cstr_done				*/
/* initiate upload sequence RESPONSE (confirm) is being decoded:	*/
/* capability list constructor done was encountered.			*/
/************************************************************************/

static ST_VOID initupl_cap_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
ST_INT rsp_size;

/* OK to be done, or may have upload detail				*/

				/* find actual size for response info	*/
  rsp_size = sizeof (INITUPL_RESP_INFO)
		+ (sizeof (ST_CHAR *) * rsp_info->num_of_capab);

				/* give back unused storage		*/
  aCtx->mmsdec_rslt->data_ptr = (*m_realloc_os_fun) (aCtx->mmsdec_rslt->data_ptr,rsp_size);
  rsp_info = (INITUPL_RESP_INFO *) aCtx->mmsdec_rslt->data_ptr;
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_initupl					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE and send the initiate upload sequence.		*/
/************************************************************************/

MMSREQ_PEND *mp_initupl (ST_INT chan, INITUPL_REQ_INFO *info_ptr)
  {
  return (_mms_req_send (MMSOP_INIT_UPLOAD,
			M_CAST_MK_FUN (_ms_mk_initupl_req),
			chan,(ST_CHAR *)info_ptr));
  } 

#else
/************************************************************************/
/************************************************************************/
/*			mpl_initupl					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the initiate upload sequence.			*/
/************************************************************************/

ST_RET  mpl_initupl (INITUPL_REQ_INFO *info_ptr)
  {
  return (_mms_req_fin (MMSOP_INIT_UPLOAD,
			M_CAST_MK_FUN (_ms_mk_initupl_req),
			(ST_CHAR *)info_ptr));
  } 
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_initupl_req					*/
/* ENCODE an initiate upload sequence REQUEST:				*/
/************************************************************************/

ST_VOID _ms_mk_initupl_req (ASN1_ENC_CTXT *aCtx, INITUPL_REQ_INFO *info)
  {
  asn1r_wr_vstr (aCtx, info->dname);		/* write the domain name	*/

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_prim (aCtx, MMSOP_INIT_UPLOAD,CTX);		/* tag = opcode, context	*/
  }
