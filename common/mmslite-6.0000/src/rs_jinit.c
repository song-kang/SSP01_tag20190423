/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rs_jinit.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the responder portion of initialize	*/
/*	journal. It decodes the initialize journal request (indication) */
/*	& encodes the initialize journal response.			*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 03/11/04  GLB     09    Remove "thisFileName"                    	*/
/* 08/28/03  EJV     08    Corr spelling for 'receive'.			*/
/* 12/20/01  JRB     07    Converted to use ASN1R (re-entrant ASN1)	*/
/* 09/13/99  MDE     06    Added SD_CONST modifiers			*/
/* 03/23/99  MDE     05    Changes to decode buffer allocation scheme	*/
/* 06/15/98  MDE     04    Changes to allow compile under C++		*/
/* 08/15/97  MDE     03    BTOD handling changes			*/
/* 07/03/97  MDE     02    Op-specific info buffer handling changes	*/
/* 06/09/97  MDE     01    Changed op specific buffer allocation	*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"

#include "mmsdefs.h"
#include "mms_pjou.h"
#include "asn1defs.h"

/************************************************************************/
/* variables global to the DECODE portion :				*/

static  JINIT_REQ_INFO	*req_info;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

static ST_VOID jinit_limit_spec_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID jinit_le_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID jinit_get_le_prim (ASN1_DEC_CTXT *aCtx);
static ST_VOID jinit_get_le_cstr (ASN1_DEC_CTXT *aCtx);
static ST_VOID jinit_get_limit_time (ASN1_DEC_CTXT *aCtx);
static ST_VOID jinit_limit_spec_cstr (ASN1_DEC_CTXT *aCtx);
static ST_VOID jinit_jou_name_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID jinit_jn_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID jinit_jou_name_cstr (ASN1_DEC_CTXT *aCtx);


/************************************************************************/
/************************************************************************/
/*			mms_jinit_req					*/
/* This function is called from mmsdec when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_jinit_req (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("Initialize Journal Request");

/* get storage for request information	*/
  req_info = (JINIT_REQ_INFO *) _m_get_dec_buf (aCtx, sizeof (JINIT_REQ_INFO));
  req_info->limit_spec_pres = SD_FALSE;
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 0, jinit_jou_name_cstr);
  }

/************************************************************************/
/*			jinit_jou_name_cstr				*/
/* initialize journal request is being decoded: journal name cstr	*/
/* was encountered.							*/
/************************************************************************/

static ST_VOID jinit_jou_name_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("jinit_jou_name_cstr");
  _ms_get_mms_objname (aCtx, &req_info->jou_name, jinit_jn_done);
  }

/************************************************************************/
/*			jinit_jn_done					*/
/* initialize journal request is being decoded: journal name has been	*/
/* obtained.								*/
/************************************************************************/

static ST_VOID jinit_jn_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("jinit_jn_done");

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = jinit_jou_name_cstr_done;
  }

/************************************************************************/
/*			jinit_jou_name_cstr_done			*/
/* initialize journal request is being decoded: journal name cstr done	*/
/* was encountered.							*/
/************************************************************************/

static ST_VOID jinit_jou_name_cstr_done (ASN1_DEC_CTXT *aCtx)
  {

  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 1, jinit_limit_spec_cstr);
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }

/************************************************************************/
/*			jinit_limit_spec_cstr				*/
/* initialize journal request is being decoded: limit specification	*/
/* was encountered.							*/
/************************************************************************/

static ST_VOID jinit_limit_spec_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("jinit_limit_spec_cstr");

  req_info->limit_spec_pres = SD_TRUE;

  ASN1R_TAG_ADD (aCtx, CTX, 0, jinit_get_limit_time);
  }

/************************************************************************/
/*			jinit_get_limit_time				*/
/* initialize journal request is being decoded: limit time was		*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID jinit_get_limit_time (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("jinit_get_limit_tim");

  if (asn1r_get_btod (aCtx, &req_info->limit_time))		/* get limiting time	*/
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);

  ASN1R_TAG_ADD (aCtx, CTX, 	 1, jinit_get_le_prim);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 1, jinit_get_le_cstr);
  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = jinit_limit_spec_cstr_done;
  }

/************************************************************************/
/*			jinit_get_le_prim				*/
/* initialize journal request is being decoded: limit entry primitive	*/
/* was encountered.							*/
/************************************************************************/

static ST_VOID jinit_get_le_prim (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("jinit_get_le_prim");
				/* decode limit entry back onto itself	*/

  req_info->limit_entry_pres = SD_TRUE;
  req_info->limit_entry = aCtx->asn1r_field_ptr;

  if (asn1r_get_octstr (aCtx, req_info->limit_entry))	/* read limit entry	*/
    asn1r_set_dec_err (aCtx, REQ_UNSPECIFIED);

  req_info->limit_entry_len = aCtx->asn1r_octetcount;

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = jinit_limit_spec_cstr_done;
  }

/************************************************************************/
/*			jinit_get_le_cstr				*/
/* initialize journal request is being decoded: limit entry cstr	*/
/* was encountered.							*/
/************************************************************************/

static ST_VOID jinit_get_le_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("jinit_get_le_cstr");

			/* decode limit entry back onto itself		*/
  req_info->limit_entry_pres = SD_TRUE;
  req_info->limit_entry = aCtx->asn1r_field_ptr;

			/* set up done decoding octet cstr destination	*/
  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = jinit_le_cstr_done;

			/* get limit entry from the received message	*/
			/* set maximum length to accept entire message	*/
  asn1r_get_octstr_cstr (aCtx, _mmsdec_msglen, req_info->limit_entry);
  }

/************************************************************************/
/*			jinit_le_cstr_done				*/
/* initialize journal request is being decoded: limit entry cstr done	*/
/* was encountered.							*/
/************************************************************************/

static ST_VOID jinit_le_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
  req_info->limit_entry_len = aCtx->asn1r_octetcount;	/* save limit entry len */
						/* cstr done already set*/
  }

/************************************************************************/
/*			jinit_limit_spec_cstr_done			*/
/* initialize journal request is being decoded: limit specification	*/
/* cstr done was encountered.						*/
/************************************************************************/

static ST_VOID jinit_limit_spec_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_jinit_resp					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE and send the initialize journal response.		*/
/************************************************************************/

ST_RET mp_jinit_resp (MMSREQ_IND *indptr, JINIT_RESP_INFO *info_ptr)
  {
  return (_mms_resp_send (MMSOP_INIT_JOURNAL,indptr,
			M_CAST_MK_FUN (_ms_mk_jinit_resp),
			(ST_CHAR *)info_ptr));
  }

#else
/************************************************************************/
/************************************************************************/
/*			mpl_jinit_resp					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the initialize journal response.			*/
/************************************************************************/

ST_RET mpl_jinit_resp (ST_UINT32 invoke_id, JINIT_RESP_INFO *info_ptr)
  {
  return (_mms_resp_fin (MMSOP_INIT_JOURNAL,invoke_id,
			M_CAST_MK_FUN (_ms_mk_jinit_resp),
			(ST_CHAR *)info_ptr));
  }
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_jinit_resp					*/
/* ENCODE an initialize journal RESPONSE:				*/
/************************************************************************/

ST_VOID _ms_mk_jinit_resp (ASN1_ENC_CTXT *aCtx, JINIT_RESP_INFO *info_ptr)
  {
  asn1r_wr_u32 (aCtx, info_ptr->del_entries);	/* encode deleted entries	*/
/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_prim (aCtx, MMSOP_INIT_JOURNAL,CTX);		/* tag = opcode, ctx		*/
  }
