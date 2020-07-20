/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rs_initdown.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the responder portion of initiate download */
/*	sequence.  It decodes the initiate download sequence request	*/
/*	(indication) & encodes the initiate download sequence response. */
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 02/24/10  JRB	   Del _mms_dec_info, use aCtx->mmsdec_rslt->data_ptr*/
/* 03/11/04  GLB     06    Remove "thisFileName"                    	*/
/* 12/20/01  JRB     05    Converted to use ASN1R (re-entrant ASN1)	*/
/*			   _ms_mk_initdown_resp:added ST_VOID *dummyInfo*/
/* 09/13/99  MDE     04    Added SD_CONST modifiers			*/
/* 03/23/99  MDE     03    Changes to decode buffer allocation scheme	*/
/* 06/15/98  MDE     02    Changes to allow compile under C++		*/
/* 06/09/97  MDE     01    Changed op specific buffer allocation	*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"

#include "mmsdefs.h"
#include "mms_pdom.h"
#include "asn1defs.h"

/************************************************************************/
/* variables global to the DECODE portion :				*/

static  INITDOWN_REQ_INFO	*req_info;

static	ST_CHAR	**cap_ptr;
static	ST_INT	max_capabilities;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

static ST_VOID initdown_get_sharable (ASN1_DEC_CTXT *aCtx);
static ST_VOID initdown_get_cap  (ASN1_DEC_CTXT *aCtx);
static ST_VOID initdown_cap_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID initdown_cap_cstr (ASN1_DEC_CTXT *aCtx);
static ST_VOID initdown_get_dname	(ASN1_DEC_CTXT *aCtx);


/************************************************************************/
/************************************************************************/
/*			mms_init_download_req				*/
/* This function is called from MMSDEC when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_init_download_req (ASN1_DEC_CTXT *aCtx)
  {
ST_INT	req_size;

  MLOG_CDEC0 ("Initiate Download Sequence Request");

#if defined(FIXED_DECBUF)
  max_capabilities = m_sv_max_initdnld_cap;
  req_size = mmsl_dec_info_size;
#else
  max_capabilities = (_mmsdec_msglen/2);
  if (m_sv_max_initdnld_cap && (max_capabilities > m_sv_max_initdnld_cap))
    max_capabilities = m_sv_max_initdnld_cap;
  req_size = sizeof (INITDOWN_REQ_INFO) + (max_capabilities * sizeof (ST_CHAR *));
#endif

/* get storage for request information	*/
  req_info = (INITDOWN_REQ_INFO *) _m_get_dec_buf (aCtx, req_size);
  cap_ptr  = (ST_CHAR **) (req_info + 1);
  aCtx->asn1r_err_fun  = _mms_dec_buf_free;
  ASN1R_TAG_ADD (aCtx, CTX, 0,initdown_get_dname);
  }

/************************************************************************/
/*			initdown_get_dname				*/
/* initiate download sequence request is being decoded: domain name	*/
/* was encountered.							*/
/************************************************************************/

static ST_VOID initdown_get_dname (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("initdown_get_dname");

  if (asn1r_get_identifier (aCtx, req_info->dname))       /* read the domain name	      */
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);

  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 1, initdown_cap_cstr);
  }

/************************************************************************/
/*			initdown_cap_cstr				*/
/* initiate download sequence request is being decoded: capability	*/
/* list constructor was encountered.					*/
/************************************************************************/

static ST_VOID initdown_cap_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("initdown_cap_cstr");

  ASN1R_TAG_ADD (aCtx, UNI,VISTR_CODE,initdown_get_cap);
  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = initdown_cap_cstr_done;
  }

/************************************************************************/
/*			initdown_get_cap				*/
/* initiate download sequence request is being decoded: capability	*/
/* was encountered.							*/
/************************************************************************/

static ST_VOID initdown_get_cap (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("initdown_get_cap");

  req_info->num_of_capab++;		/* increment # of capabilities	*/
  if (req_info->num_of_capab > max_capabilities)
    {
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);
    return;
    }

	/* copy capability back onto itself shifted by one character	*/
  *cap_ptr = (ST_CHAR *) aCtx->asn1r_field_ptr - 1;
  if (asn1r_get_vstr (aCtx, *cap_ptr))
    {					/* capability is too ST_INT32	*/
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);
    return;
    }
  cap_ptr++;				/* point to where next will go	*/

  ASN1R_TAG_ADD (aCtx, UNI,VISTR_CODE,initdown_get_cap);
  }

/************************************************************************/
/*			initdown_cap_cstr_done				*/
/* initiate download sequence request is being decoded: capability	*/
/* list constructor done was encountered.				*/
/************************************************************************/

static ST_VOID initdown_cap_cstr_done (ASN1_DEC_CTXT *aCtx)
  {

  ASN1R_TAG_ADD (aCtx, CTX,2,initdown_get_sharable);
  }

/************************************************************************/
/*			initdown_get_sharable				*/
/* initiate download sequence request is being decoded: sharable	*/
/* was encountered.							*/
/************************************************************************/

static ST_VOID initdown_get_sharable (ASN1_DEC_CTXT *aCtx)
  {
ST_INT	 req_size;

  MLOG_CDEC0 ("initdown_get_sharable");

			/* read sharable from the received message	*/
  if (asn1r_get_bool (aCtx, &(req_info->sharable)))
    {					/* sharable is too ST_INT32 	*/
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);
    return;
    }

  				/* find actual size for request info	*/
  req_size = sizeof (INITDOWN_REQ_INFO)
		+ (sizeof (ST_CHAR *) * req_info->num_of_capab);

				/* give back unused storage		*/
  aCtx->mmsdec_rslt->data_ptr = (*m_realloc_os_fun) (aCtx->mmsdec_rslt->data_ptr,req_size);
  req_info = (INITDOWN_REQ_INFO *) aCtx->mmsdec_rslt->data_ptr;

  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_initdown_resp				*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE and send the initiate download sequence response.	*/
/************************************************************************/

ST_RET mp_initdown_resp (MMSREQ_IND *indptr)
  {
  if (mms_chan_info[_mmsdechan].version)		/* IS 	*/
    return (_mms_send_null_resp (indptr,MMSOP_INIT_DOWNLOAD));
  else						/* DIS 	*/
    return (_mms_resp_send (MMSOP_INIT_DOWNLOAD,indptr,
			M_CAST_MK_FUN (_ms_mk_initdown_resp),
			NULL));
  }

#else
/************************************************************************/
/************************************************************************/
/*			mpl_initdown_resp				*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the initiate download sequence response.		*/
/************************************************************************/

ST_RET mpl_initdown_resp (ST_UINT32 invoke_id)
  {
  if (mmsl_version)				/* IS 	*/
    return (_mms_fin_null_resp (invoke_id,MMSOP_INIT_DOWNLOAD));
  else						/* DIS 	*/
    return (_mms_resp_fin (MMSOP_INIT_DOWNLOAD,invoke_id,
			M_CAST_MK_FUN (_ms_mk_initdown_resp),
			NULL));
  }
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_initdown_resp				*/
/* ENCODE an initiate download RESPONSE:				*/
/************************************************************************/

ST_VOID _ms_mk_initdown_resp (ASN1_ENC_CTXT *aCtx, ST_VOID *dummyInfo)
  {
/* write the context specific explicit tag for this opcode.		*/

  asn1r_fin_prim (aCtx, NULL_CODE,0);		/* Write NULL field		*/
  asn1r_fin_constr (aCtx, MMSOP_INIT_DOWNLOAD,CTX,DEF);	/* tag = opcode, ctx cstr	*/
  }
