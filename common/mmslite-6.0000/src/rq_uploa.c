/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rq_upload.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the requester portion of upload segment.	*/
/*	It decodes the upload segment response (confirm) & encodes the	*/
/*	upload segment request. 					*/
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
/* variables global to the DECODE portion : NONE			*/

static  UPLOAD_RESP_INFO	*rsp_info;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

static ST_VOID upload_get_more_follows (ASN1_DEC_CTXT *aCtx);
static ST_VOID upload_load_data_prim (ASN1_DEC_CTXT *aCtx);
static ST_VOID upload_load_data_cstr (ASN1_DEC_CTXT *aCtx);
static ST_VOID upload_load_data_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID extern_done (ASN1_DEC_CTXT *aCtx);


/************************************************************************/
/************************************************************************/
/*			mms_upload_rsp					*/
/* This function is called from MMSDEC when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_upload_rsp (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("Upload Segment Response");


/* get storage for request information	*/
  rsp_info = (UPLOAD_RESP_INFO *) _m_get_dec_buf (aCtx, sizeof (UPLOAD_RESP_INFO));
  rsp_info->more_follows = SD_TRUE;	/* set default value		*/

  ASN1R_TAG_ADD (aCtx, CTX, 	 0, upload_load_data_prim);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 0, upload_load_data_cstr);

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
/*			upload_load_data_prim				*/
/* upload segment response: decode the load data primitive		*/
/************************************************************************/

static ST_VOID upload_load_data_prim (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("upload_load_data_pri");

	/* decode load data back onto itself, shifted by one character	*/
  rsp_info->load_data = aCtx->asn1r_field_ptr - 1;

					/* read load data from message	*/
  if (asn1r_get_octstr (aCtx, rsp_info->load_data))
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);

  rsp_info->load_data_len = aCtx->asn1r_octetcount;

  ASN1R_TAG_ADD (aCtx, CTX, 1, upload_get_more_follows);
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/

  }

/************************************************************************/
/*			upload_load_data_cstr				*/
/* upload segment response: decode load data constructor		*/
/************************************************************************/

static ST_VOID upload_load_data_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("upload_load_data_cstr");

	/* decode load data back onto itself, shifted by one character	*/
  rsp_info->load_data = aCtx->asn1r_field_ptr - 1;

			/* set up where to go when done decoding cstr	*/
  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = upload_load_data_cstr_done;

			/* get load data from the received message	*/
  asn1r_get_octstr_cstr (aCtx, _mmsdec_msglen, rsp_info->load_data);
  }

/************************************************************************/
/*			upload_load_data_cstr_done			*/
/* upload segment response: load data constructor is done		*/
/************************************************************************/

static ST_VOID upload_load_data_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
  rsp_info->load_data_len = aCtx->asn1r_octetcount; /* save the load data length	*/

  ASN1R_TAG_ADD (aCtx, CTX, 1, upload_get_more_follows);
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }

/************************************************************************/
/*			   extern_done					*/
/************************************************************************/

static ST_VOID extern_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("extern_done");

  rsp_info->load_data_type = LOAD_DATA_CODED;
  ASN1R_TAG_ADD (aCtx, CTX, 1, upload_get_more_follows);
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }

/************************************************************************/
/*			upload_get_more_follows 			*/
/* upload segment response: decode the more follows flag		*/
/************************************************************************/

static ST_VOID upload_get_more_follows (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("upload_get_more_follows");
					/* read end of pgm from message */
  if (asn1r_get_bool (aCtx, &(rsp_info->more_follows)))
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);

  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_upload					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE and send the upload segment request.		*/
/************************************************************************/

MMSREQ_PEND *mp_upload (ST_INT chan, UPLOAD_REQ_INFO *info_ptr)
  {
  return (_mms_req_send (MMSOP_UP_LOAD,
			M_CAST_MK_FUN (_ms_mk_upload_req),
			chan,(ST_CHAR *)info_ptr));
  } 

#else
/************************************************************************/
/************************************************************************/
/*			mpl_upload					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE and send the upload segment request.		*/
/************************************************************************/

ST_RET  mpl_upload (UPLOAD_REQ_INFO *info_ptr)
  {
  return (_mms_req_fin (MMSOP_UP_LOAD,
			M_CAST_MK_FUN (_ms_mk_upload_req),
			(ST_CHAR *)info_ptr));
  } 
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_upload_req					*/
/* construct an upload segment request					*/
/************************************************************************/

ST_VOID _ms_mk_upload_req (ASN1_ENC_CTXT *aCtx, UPLOAD_REQ_INFO *info)
  {
  asn1r_strt_constr (aCtx);			/* start sequence constr	*/

  asn1r_wr_i32 (aCtx, info->ulsmid);		/* write upload state machine id*/

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_prim (aCtx, MMSOP_UP_LOAD,CTX);		/* tag = opcode, ctx		*/
  }
