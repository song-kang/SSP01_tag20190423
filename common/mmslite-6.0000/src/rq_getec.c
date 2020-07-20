/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rq_geteca.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the requester portion of get event		*/
/*	condition attributes.  It decodes the get event condition	*/
/*	attributes response (confirm) and encodes the get event		*/
/*	condition attributes request.					*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
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
#include "mms_pevn.h"
#include "asn1defs.h"

/************************************************************************/
/* variables global to the DECODE portion :				*/

static  GETECA_RESP_INFO	*rsp_info;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

static ST_VOID geteca_var_ref_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID geteca_get_ec_class (ASN1_DEC_CTXT *aCtx);
static ST_VOID geteca_get_severity (ASN1_DEC_CTXT *aCtx);
static ST_VOID geteca_get_as_reports (ASN1_DEC_CTXT *aCtx);
static ST_VOID geteca_monitored_var_cstr (ASN1_DEC_CTXT *aCtx);
static ST_VOID geteca_monitored_var_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID geteca_get_eval_int (ASN1_DEC_CTXT *aCtx);
static ST_VOID geteca_get_var_ref_cstr (ASN1_DEC_CTXT *aCtx);
static ST_VOID geteca_get_undefined (ASN1_DEC_CTXT *aCtx);
static ST_VOID geteca_get_priority (ASN1_DEC_CTXT *aCtx);
static ST_VOID geteca_get_mms_deletable (ASN1_DEC_CTXT *aCtx);


/************************************************************************/
/************************************************************************/
/*			mms_geteca_rsp					*/
/* This function is called from mmsdec when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_geteca_rsp (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("Get Event Condition Attributes Response");


/* get storage for request information	*/
  rsp_info = (GETECA_RESP_INFO *) _m_get_dec_buf (aCtx, sizeof (GETECA_RESP_INFO));
  rsp_info->priority      = MMS_NORM_PRIORITY;	/* set default value	*/
  rsp_info->severity      = MMS_NORM_SEVERITY;
/*  rsp_info->mms_deletable = SD_FALSE;		*/
/*  rsp_info->as_reports    = SD_FALSE;		*/
/*  rsp_info->addl_detail_pres = SD_FALSE;		*/
/*  rsp_info->eval_int_pres = SD_FALSE;		*/

  ASN1R_TAG_ADD (aCtx, CTX, 0, geteca_get_mms_deletable);
  ASN1R_TAG_ADD (aCtx, CTX, 1, geteca_get_ec_class);
  }

/************************************************************************/
/*			geteca_get_mms_deletable			*/
/* Decoding get event condition attributes response: mms deletable was	*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID geteca_get_mms_deletable (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("geteca_get_mms_deletable");
					/* read mms deletable		*/
  if (asn1r_get_u8 (aCtx, &(rsp_info->mms_deletable)))
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);

  ASN1R_TAG_ADD (aCtx, CTX, 1, geteca_get_ec_class);
  }

/************************************************************************/
/*			geteca_get_ec_class				*/
/* Decoding get event condition attributes response: ec class was	*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID geteca_get_ec_class (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("geteca_get_ec_class");
					/* read event condition class	*/
  if (asn1r_get_i16 (aCtx, &(rsp_info->eclass)))
    {
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);
    return;
    }
  else if ((rsp_info->eclass < 0) || (rsp_info->eclass > 1))
    {
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);
    return;
    }

  ASN1R_TAG_ADD (aCtx, CTX, 		2, geteca_get_priority);
  ASN1R_TAG_ADD (aCtx, CTX, 		3, geteca_get_severity);
  ASN1R_TAG_ADD (aCtx, CTX,		4, geteca_get_as_reports);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 6, geteca_monitored_var_cstr);
  ASN1R_TAG_ADD (aCtx, CTX,		7, geteca_get_eval_int);
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }

/************************************************************************/
/*			geteca_get_priority				*/
/* Decoding get event condition attributes response: priority was	*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID geteca_get_priority (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("geteca_get_priority");
					/* read priority		*/
  if (asn1r_get_u8 (aCtx, &(rsp_info->priority)))
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);

  ASN1R_TAG_ADD (aCtx, CTX, 		3, geteca_get_severity);
  ASN1R_TAG_ADD (aCtx, CTX, 		4, geteca_get_as_reports);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 6, geteca_monitored_var_cstr);
  ASN1R_TAG_ADD (aCtx, CTX, 		7, geteca_get_eval_int);
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }

/************************************************************************/
/*			geteca_get_severity				*/
/* Decoding get event condition attributes response: severity was	*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID geteca_get_severity (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("geteca_get_severity");
					/* read severity		*/
  if (asn1r_get_u8 (aCtx, &(rsp_info->severity)))
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);

  ASN1R_TAG_ADD (aCtx, CTX, 		4, geteca_get_as_reports);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 6, geteca_monitored_var_cstr);
  ASN1R_TAG_ADD (aCtx, CTX, 		7, geteca_get_eval_int);
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }

/************************************************************************/
/*			geteca_get_as_reports				*/
/* Decoding get event condition attributes response: alarm summary	*/
/* reports was encountered. 						*/
/************************************************************************/

static ST_VOID geteca_get_as_reports (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("geteca_get_as_reports");
					/* read alarm summary reports	*/
  if (asn1r_get_u8 (aCtx, &(rsp_info->as_reports)))
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);

  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 6, geteca_monitored_var_cstr);
  ASN1R_TAG_ADD (aCtx, CTX, 		7, geteca_get_eval_int);
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }

/************************************************************************/
/*			geteca_monitored_var_cstr			*/
/* Decoding get event condition attributes response: monitored variable	*/
/* was encountered.							*/
/************************************************************************/

static ST_VOID geteca_monitored_var_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("geteca_monitored_var_cstr");

  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 0, geteca_get_var_ref_cstr);
  ASN1R_TAG_ADD (aCtx, CTX,          1, geteca_get_undefined);
  }

/************************************************************************/
/*			geteca_get_var_ref_cstr				*/
/* Decoding get event condition attributes response: variable reference	*/
/* was encountered.							*/
/************************************************************************/

static ST_VOID geteca_get_var_ref_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("geteca_get_var_ref_cstr");

  rsp_info->mon_var_tag = SD_SUCCESS; 	/* set to variable reference	*/

  _ms_get_va_var_spec (aCtx, &rsp_info->var_ref, geteca_var_ref_done);
  }

/************************************************************************/
/*			geteca_var_ref_done				*/
/************************************************************************/

static ST_VOID geteca_var_ref_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("geteca_var_ref_done");

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level]   = NULL;
  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level-1] = geteca_monitored_var_cstr_done;
  }

/************************************************************************/
/*			geteca_get_undefined				*/
/* Decoding get event condition attributes response: undefined was	*/
/* encountered.								*/
/************************************************************************/

static ST_VOID geteca_get_undefined (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("geteca_get_undefined");

  rsp_info->mon_var_tag = SD_FAILURE;	/* set to undefined		*/

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = geteca_monitored_var_cstr_done;
  }

/************************************************************************/
/*			geteca_monitored_var_cstr_done			*/
/* Decoding get event condition attributes response: monitored variable	*/
/* constructor done was encountered.					*/
/************************************************************************/

static ST_VOID geteca_monitored_var_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
  rsp_info->mon_var_pres = SD_TRUE;
  ASN1R_TAG_ADD (aCtx, CTX, 7, geteca_get_eval_int);
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }

/************************************************************************/
/*			geteca_get_eval_int				*/
/* Decoding get event condition attributes response: evaluation		*/
/* interval was encountered. 						*/
/************************************************************************/

static ST_VOID geteca_get_eval_int (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("geteca_get_eval_int");

  rsp_info->eval_int_pres = SD_TRUE;
					/* read evaluation interval	*/
  if (asn1r_get_u32 (aCtx, &(rsp_info->eval_interval)))
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);

  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_geteca					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE and send the get event condition attributes	*/
/* request.								*/
/************************************************************************/

MMSREQ_PEND *mp_geteca (ST_INT chan, GETECA_REQ_INFO *info_ptr)
  {
  return (_mms_req_send (MMSOP_GET_EC_ATTR,
			M_CAST_MK_FUN (_ms_mk_geteca_req),
			chan,(ST_CHAR *)info_ptr));
  } 

#else
/************************************************************************/
/************************************************************************/
/*			mpl_geteca					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the get event condition attributes	request.  	*/
/************************************************************************/

ST_RET  mpl_geteca (GETECA_REQ_INFO *info_ptr)
  {
  return (_mms_req_fin (MMSOP_GET_EC_ATTR,
			M_CAST_MK_FUN (_ms_mk_geteca_req),
			(ST_CHAR *)info_ptr));
  } 
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_geteca_req					*/
/* construct a get event condition attributes request 			*/
/************************************************************************/

ST_VOID _ms_mk_geteca_req (ASN1_ENC_CTXT *aCtx, GETECA_REQ_INFO *info)
  {
  _ms_wr_mms_objname (aCtx, &info->evcon_name);	/* write the object name	*/

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_constr (aCtx, MMSOP_GET_EC_ATTR,CTX,DEF);	/* tag = opcode, ctx constr	*/
  }
