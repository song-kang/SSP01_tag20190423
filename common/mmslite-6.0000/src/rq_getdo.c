/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rq_getdom.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the requester portion of get domain	*/
/*	attribute.  It decodes the get domain attribute response	*/
/*	(confirm) & encodes the get domain attribute request.		*/
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
/* 06/15/98  MDE     01    Changes to allow compile under C++		*/
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

static  GETDOM_RESP_INFO	*rsp_info;


static ST_CHAR		       **cap_ptr;
static ST_CHAR		       **pin_ptr;
static ST_INT  max_capabilities;
static ST_INT  max_pi_names;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

static ST_VOID getdom_get_upl_in_prog (ASN1_DEC_CTXT *aCtx);
static ST_VOID getdom_get_pin (ASN1_DEC_CTXT *aCtx);
static ST_VOID getdom_pin_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID getdom_pin_cstr (ASN1_DEC_CTXT *aCtx);
static ST_VOID getdom_get_sharable (ASN1_DEC_CTXT *aCtx);
static ST_VOID getdom_get_mms_deletable (ASN1_DEC_CTXT *aCtx);
static ST_VOID getdom_get_state (ASN1_DEC_CTXT *aCtx);
static ST_VOID getdom_get_cap (ASN1_DEC_CTXT *aCtx);
static ST_VOID getdom_cap_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID getdom_cap_cstr (ASN1_DEC_CTXT *aCtx);


/************************************************************************/
/************************************************************************/
/*			mms_get_dom_attr_rsp				*/
/* This function is called from MMSDEC when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_get_dom_attr_rsp (ASN1_DEC_CTXT *aCtx)
  {
ST_INT rsp_size;

  MLOG_CDEC0 ("Get Domain Attribute Response");

#if defined(FIXED_DECBUF)
  max_capabilities = _mmsdec_msglen/2;
  max_pi_names	   = _mmsdec_msglen/2;
  rsp_size = mmsl_dec_info_size;
#else
  /* determine maximum size	*/
  max_capabilities = _mmsdec_msglen/2;
  max_pi_names	   = _mmsdec_msglen/2;
  if (m_cl_max_getdom_cap && (max_capabilities > m_cl_max_getdom_cap))
    max_capabilities = m_cl_max_getdom_cap;
  if (m_cl_max_getdom_pi && (max_pi_names > m_cl_max_getdom_pi))
    max_pi_names = m_cl_max_getdom_pi;
  rsp_size = sizeof (GETDOM_RESP_INFO)
		+ (max_capabilities * (sizeof (ST_CHAR *)))
		+ (max_pi_names     * (sizeof (ST_CHAR *)));
					/* get storage for resp info	*/
#endif

/* get storage for request information	*/
  rsp_info = (GETDOM_RESP_INFO *) _m_get_dec_buf (aCtx, rsp_size);
  cap_ptr  = (ST_CHAR **) (rsp_info + 1);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 0, getdom_cap_cstr);/* cap list is next*/
  }

/************************************************************************/
/*			getdom_cap_cstr 				*/
/* get domain attribute RESPONSE (confirm) is being decoded:		*/
/* capability list constructor was encountered. 			*/
/************************************************************************/

static ST_VOID getdom_cap_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("getdom_cap_cstr");

  ASN1R_TAG_ADD (aCtx, UNI, VISTR_CODE, getdom_get_cap);
  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = getdom_cap_cstr_done;
  }

/************************************************************************/
/*			getdom_get_cap					*/
/* get domain attribute RESPONSE (confirm) is being decoded:		*/
/* capability was encountered.						*/
/************************************************************************/

static ST_VOID getdom_get_cap (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("getdom_get_cap");

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

  ASN1R_TAG_ADD (aCtx, UNI, VISTR_CODE, getdom_get_cap);
  }

/************************************************************************/
/*			getdom_cap_cstr_done				*/
/* get domain attribute RESPONSE (confirm) is being decoded:		*/
/* capability list constructor done was encountered.			*/
/************************************************************************/

static ST_VOID getdom_cap_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
  ASN1R_TAG_ADD (aCtx, CTX, 1, getdom_get_state);
  }

/************************************************************************/
/*			getdom_get_state				*/
/* get domain attribute RESPONSE (confirm) is being decoded:		*/
/* state was encountered.						*/
/************************************************************************/

static ST_VOID getdom_get_state (ASN1_DEC_CTXT *aCtx)
  {
ST_INT ver;

  MLOG_CDEC0 ("getdom_get_state");

#ifndef MMS_LITE
  ver = mms_chan_info[_mmsdechan].version;
#else
  ver = mmsl_version;
#endif

  if ((asn1r_get_i16 (aCtx, &(rsp_info->state))) ||	/* get & verify state 	*/
      (rsp_info->state < 0)          ||
      (!ver && rsp_info->state > 5)   ||	/* DIS */
      (ver && rsp_info->state > 15))		/* IS  */
    {
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);
    return;
    }

  ASN1R_TAG_ADD (aCtx, CTX, 2, getdom_get_mms_deletable);
  }

/************************************************************************/
/*			getdom_get_mms_deletable			*/
/* get domain attribute RESPONSE (confirm) is being decoded:		*/
/* mms deletable was encountered.					*/
/************************************************************************/

static ST_VOID getdom_get_mms_deletable (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("getdom_get_mms_deletable");

  if (asn1r_get_bool (aCtx, &(rsp_info->mms_deletable)))	/* get mms deletable	*/
    {
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);
    return;
    }

  ASN1R_TAG_ADD (aCtx, CTX, 3, getdom_get_sharable);
  }

/************************************************************************/
/*			getdom_get_sharable				*/
/* get domain attribute RESPONSE (confirm) is being decoded:		*/
/* sharable was encountered.						*/
/************************************************************************/

static ST_VOID getdom_get_sharable (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("getdom_get_sharable");

  if (asn1r_get_bool (aCtx, &(rsp_info->sharable)))		/* get sharable 	*/
    {
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);
    return;
    }

  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 4, getdom_pin_cstr);
  }


/************************************************************************/
/*			getdom_pin_cstr 				*/
/* get domain attribute RESPONSE (confirm) is being decoded:		*/
/* program invocation name list constructor was encountered.		*/
/************************************************************************/

static ST_VOID getdom_pin_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("getdom_pin_cstr");

  pin_ptr  = (ST_CHAR **) (cap_ptr);

  ASN1R_TAG_ADD (aCtx, UNI, VISTR_CODE, getdom_get_pin);
  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = getdom_pin_cstr_done;
  }

/************************************************************************/
/*			getdom_get_pin					*/
/* get domain attribute RESPONSE (confirm) is being decoded:		*/
/* progran invocation name was encountered.				*/
/************************************************************************/

static ST_VOID getdom_get_pin (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("getdom_get_pin");

  rsp_info->num_of_pinames++;		/* increment # of pi names	*/
  if (rsp_info->num_of_pinames > max_pi_names)
    {
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);
    return;
    }

	/* copy pi name back onto itself shifted by one character	*/
  *pin_ptr = (ST_CHAR *) aCtx->asn1r_field_ptr - 1;
  if (asn1r_get_identifier (aCtx, *pin_ptr))
    {
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);
    return;
    }
  pin_ptr++;

  ASN1R_TAG_ADD (aCtx, UNI, VISTR_CODE, getdom_get_pin);
  }

/************************************************************************/
/*			getdom_pin_cstr_done				*/
/* get domain attribute RESPONSE (confirm) is being decoded:		*/
/* program invocation name list constructor done was encountered.	*/
/************************************************************************/

static ST_VOID getdom_pin_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
  ASN1R_TAG_ADD (aCtx, CTX, 5, getdom_get_upl_in_prog);
  }

/************************************************************************/
/*			getdom_get_upl_in_prog				*/
/* get domain attribute RESPONSE (confirm) is being decoded:		*/
/* upload in progress was encountered.					*/
/************************************************************************/

static ST_VOID getdom_get_upl_in_prog (ASN1_DEC_CTXT *aCtx)
  {
ST_INT rsp_size;

  MLOG_CDEC0 ("getdom_get_upl_in_prog");

  if (asn1r_get_i8 (aCtx, &(rsp_info->upload_in_progress)))
    {
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);
    return;
    }

				/* find actual size for response info	*/
  rsp_size = sizeof (GETDOM_RESP_INFO)
		+ (sizeof (ST_CHAR *) * rsp_info->num_of_capab)
		+ (sizeof (ST_CHAR *) * rsp_info->num_of_pinames);

				/* give back unused storage		*/
  aCtx->mmsdec_rslt->data_ptr = (*m_realloc_os_fun) (aCtx->mmsdec_rslt->data_ptr,rsp_size);
  rsp_info = (GETDOM_RESP_INFO *) aCtx->mmsdec_rslt->data_ptr;
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_getdom					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE and send the get domain attribute request. 	*/
/************************************************************************/

MMSREQ_PEND *mp_getdom (ST_INT chan, GETDOM_REQ_INFO *info_ptr)
  {
  return (_mms_req_send (MMSOP_GET_DOM_ATTR,
			M_CAST_MK_FUN (_ms_mk_getdom_req),
			chan,(ST_CHAR *)info_ptr));
  }

#else
/************************************************************************/
/************************************************************************/
/*			mpl_getdom					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the get domain attribute request. 			*/
/************************************************************************/

ST_RET  mpl_getdom (GETDOM_REQ_INFO *info_ptr)
  {
  return (_mms_req_fin (MMSOP_GET_DOM_ATTR,
			M_CAST_MK_FUN (_ms_mk_getdom_req),
			(ST_CHAR *)info_ptr));
  }
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_getdom_req					*/
/* ENCODE a get domain attribute REQUEST:				*/
/************************************************************************/

ST_VOID _ms_mk_getdom_req (ASN1_ENC_CTXT *aCtx, GETDOM_REQ_INFO *info)
  {
  asn1r_wr_vstr (aCtx, info->dname);		/* write the domain name	*/

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_prim (aCtx, MMSOP_GET_DOM_ATTR,CTX);		/* tag = opcode, context	*/
  }
