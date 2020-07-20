/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rq_download.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the requester portion of the download	*/
/*	segment.  It decodes the download segment response (confirm)	*/
/*	& encodes the download segment request. 			*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 03/11/04  GLB     07    Remove "thisFileName"                    	*/
/* 08/28/03  EJV     06    Corr spelling for 'receive'.			*/
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
/* variables global to the DECODE portion :				*/

static  DOWNLOAD_RESP_INFO	*rsp_info;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

static ST_VOID download_get_more_follows (ASN1_DEC_CTXT *aCtx);
static ST_VOID download_get_load_data (ASN1_DEC_CTXT *aCtx);
static ST_VOID download_load_data_cstr (ASN1_DEC_CTXT *aCtx);
static ST_VOID download_load_data_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID extern_done (ASN1_DEC_CTXT *aCtx);


/************************************************************************/
/************************************************************************/
/*			mms_download_rsp				*/
/* This function is called from MMSDEC when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_download_rsp (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("Download Segment Response");

/* get storage for request information	*/
  rsp_info = (DOWNLOAD_RESP_INFO *) _m_get_dec_buf (aCtx, sizeof (DOWNLOAD_RESP_INFO));
  rsp_info->more_follows = SD_TRUE;		/* set default value	*/

  ASN1R_TAG_ADD (aCtx, CTX, 	 0, download_get_load_data);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 0, download_load_data_cstr);

/* If MMS IS, Check for CODED load data					*/
#ifndef MMS_LITE
  if (mms_chan_info[_mmsdechan].version)		/* IS 	*/
#else
  if (mmsl_version)				/* IS 	*/
#endif
    _ms_chk_for_extern (aCtx, &rsp_info->load_data_len,
    		    &rsp_info->load_data,
		    extern_done);
  }

/************************************************************************/
/*			download_get_load_data				*/
/* Decoding download segment response: load data octets primitive was	*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID download_get_load_data (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("download_get_load_data");

	/* decode load data onto itself, shifted by one character	*/
  rsp_info->load_data = aCtx->asn1r_field_ptr -1;
					/* read load data		*/
  if (asn1r_get_octstr (aCtx, rsp_info->load_data))
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);

  rsp_info->load_data_type = LOAD_DATA_NON_CODED;
  rsp_info->load_data_len = aCtx->asn1r_octetcount; /* save the load data length	*/

  ASN1R_TAG_ADD (aCtx, CTX, 1, download_get_more_follows);
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }

/************************************************************************/
/*			download_load_data_cstr 			*/
/* Decoding download segment response: load data octets constructor was */
/* encountered. 							*/
/************************************************************************/

static ST_VOID download_load_data_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("download_load_data_cstr");

	/* decode entry id onto itself, shifted by one character	*/
  rsp_info->load_data = aCtx->asn1r_field_ptr - 1;

			/* set up where to go when done decoding cstr	*/
  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = download_load_data_cstr_done;

			/* read load data from the received message	*/
  asn1r_get_octstr_cstr (aCtx, _mmsdec_msglen, rsp_info->load_data);
  }

/************************************************************************/
/*			download_load_data_cstr_done			*/
/* Decoding download segment response: load data octets constructor	*/
/* done was encountered.						*/
/************************************************************************/

static ST_VOID download_load_data_cstr_done (ASN1_DEC_CTXT *aCtx)
  {

  rsp_info->load_data_len = aCtx->asn1r_octetcount; /* save the load data length	*/

  ASN1R_TAG_ADD (aCtx, CTX, 1, download_get_more_follows);
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }

/************************************************************************/
/*			   extern_done					*/
/************************************************************************/

static ST_VOID extern_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("extern_done");

  rsp_info->load_data_type = LOAD_DATA_CODED;
  ASN1R_TAG_ADD (aCtx, CTX, 1, download_get_more_follows);
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }

/************************************************************************/
/*			download_get_more_follows			*/
/* Decoding download segment response: more follows was encountered.	*/
/************************************************************************/

static ST_VOID download_get_more_follows (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("download_get_more_follows");
					/* read more follows		*/
  if (asn1r_get_bool (aCtx, &(rsp_info->more_follows)))
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);

  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_download					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE and send the download segment request.		*/
/************************************************************************/

MMSREQ_PEND *mp_download (ST_INT chan, DOWNLOAD_REQ_INFO *info_ptr)
  {
  return (_mms_req_send (MMSOP_DOWN_LOAD,
			M_CAST_MK_FUN (_ms_mk_download_req),
			chan,(ST_CHAR *)info_ptr));
  } 

#else
/************************************************************************/
/************************************************************************/
/*			mpl_download					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the download segment request.			*/
/************************************************************************/

ST_RET  mpl_download (DOWNLOAD_REQ_INFO *info_ptr)
  {
  return (_mms_req_fin (MMSOP_DOWN_LOAD,
			M_CAST_MK_FUN (_ms_mk_download_req),
			(ST_CHAR *)info_ptr));
  } 
#endif


/************************************************************************/
/*			_ms_mk_download_req 				*/
/* construct a download segment request 				*/
/************************************************************************/

ST_VOID _ms_mk_download_req (ASN1_ENC_CTXT *aCtx, DOWNLOAD_REQ_INFO *info)
  {
  asn1r_wr_vstr (aCtx, info->dname);		/* write the domain name	*/

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_prim (aCtx, MMSOP_DOWN_LOAD,CTX);		/* tag = opcode, ctx		*/
  }
