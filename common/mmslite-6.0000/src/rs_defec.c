/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rs_defec.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the responder portion of define event	*/
/*	condition.  It decodes the define event condition request	*/
/*	(indication) & encodes the define event condition response.   	*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 03/11/04  GLB     05    Remove "thisFileName"                    	*/
/* 12/20/01  JRB     04    Converted to use ASN1R (re-entrant ASN1)	*/
/* 09/13/99  MDE     03    Added SD_CONST modifiers			*/
/* 03/23/99  MDE     02    Changes to decode buffer allocation scheme	*/
/* 07/03/97  MDE     01    Op-specific info buffer handling changes	*/
/* 06/09/97  MDE     01    Changed op specific buffer allocation	*/
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

static  DEFEC_REQ_INFO	*req_info;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

static ST_VOID defec_get_severity (ASN1_DEC_CTXT *aCtx);
static ST_VOID defec_get_as_reports (ASN1_DEC_CTXT *aCtx);
static ST_VOID defec_monitored_var_cstr (ASN1_DEC_CTXT *aCtx);
static ST_VOID defec_get_eval_int (ASN1_DEC_CTXT *aCtx);
static ST_VOID defec_monitored_var_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID defec_var_spec_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID defec_get_priority (ASN1_DEC_CTXT *aCtx);
static ST_VOID defec_get_ec_class (ASN1_DEC_CTXT *aCtx);
static ST_VOID defec_name_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID defec_name_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID defec_name_cstr  (ASN1_DEC_CTXT *aCtx);


/************************************************************************/
/************************************************************************/
/*			mms_defec_req					*/
/* This function is called from MMSDEC when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_defec_req (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("Define Event Condition Request");


/* get storage for request information	*/
  req_info = (DEFEC_REQ_INFO *) _m_get_dec_buf (aCtx, sizeof (DEFEC_REQ_INFO));

  req_info->priority      = MMS_NORM_PRIORITY;	/* set default value	*/
  req_info->severity      = MMS_NORM_SEVERITY;
/*  req_info->as_reports  = SD_FALSE;		*/
/*  req_info->addl_detail_pres = SD_FALSE;		*/
/*  req_info->mon_var_pres  = SD_FALSE;		*/
/*  req_info->eval_int_pres = SD_FALSE;		*/

  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 0, defec_name_cstr);
  }

/************************************************************************/
/*			defec_name_cstr					*/
/* define event condition request is being decoded: event condition	*/
/* name constructor was encountered. 					*/
/************************************************************************/

static ST_VOID defec_name_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("defec_name_cstr");

  _ms_get_mms_objname (aCtx, &req_info->evcon_name, defec_name_done);
  }

/************************************************************************/
/*			defec_name_done					*/
/* define event condition request is being decoded: event condition	*/
/* name has been obtained.	 					*/
/************************************************************************/

static ST_VOID defec_name_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("defec_name_done");

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = defec_name_cstr_done;
  }

/************************************************************************/
/*			defec_name_cstr_done				*/
/* define event condition request is being decoded: event condition 	*/
/* name constructor done was encountered. 				*/
/************************************************************************/

static ST_VOID defec_name_cstr_done (ASN1_DEC_CTXT *aCtx)
  {

  ASN1R_TAG_ADD (aCtx, CTX, 1, defec_get_ec_class);
  }

/************************************************************************/
/*			defec_get_ec_class				*/
/* Decoding define event condition request: ec class was encountered.	*/
/************************************************************************/

static ST_VOID defec_get_ec_class (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("defec_get_ec_class");
					/* read event condition class	*/
  if (asn1r_get_i16 (aCtx, &(req_info->eclass)))
    {
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);
    return;
    }
  else if ((req_info->eclass < 0) || (req_info->eclass > 1))
    {
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);
    return;
    }

  ASN1R_TAG_ADD (aCtx, CTX,          2, defec_get_priority);
  ASN1R_TAG_ADD (aCtx, CTX,          3, defec_get_severity);
  ASN1R_TAG_ADD (aCtx, CTX,          4, defec_get_as_reports);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 6, defec_monitored_var_cstr);
  ASN1R_TAG_ADD (aCtx, CTX,          7, defec_get_eval_int);
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }

/************************************************************************/
/*			defec_get_priority				*/
/* Decoding define event condition request: priority was encountered. 	*/
/************************************************************************/

static ST_VOID defec_get_priority (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("defec_get_priority");
					/* read priority		*/
  if (asn1r_get_u8 (aCtx, &(req_info->priority)))
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);

  ASN1R_TAG_ADD (aCtx, CTX,          3, defec_get_severity);
  ASN1R_TAG_ADD (aCtx, CTX,          4, defec_get_as_reports);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 6, defec_monitored_var_cstr);
  ASN1R_TAG_ADD (aCtx, CTX,          7, defec_get_eval_int);
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }

/************************************************************************/
/*			defec_get_severity				*/
/* Decoding define event condition request: severity was encountered.	*/
/************************************************************************/

static ST_VOID defec_get_severity (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("defec_get_severity");
					/* read severity		*/
  if (asn1r_get_u8 (aCtx, &(req_info->severity)))
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);

  ASN1R_TAG_ADD (aCtx, CTX,          4, defec_get_as_reports);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 6, defec_monitored_var_cstr);
  ASN1R_TAG_ADD (aCtx, CTX,          7, defec_get_eval_int);
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }

/************************************************************************/
/*			defec_get_as_reports				*/
/* Decoding define event condition request: alarm summary reports was	*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID defec_get_as_reports (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("defec_get_as_reports");
  
  req_info->as_reports_pres = SD_TRUE;	/* set present flag		*/
					/* read alarm summary reports	*/
  if (asn1r_get_bool (aCtx, &(req_info->as_reports)))
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);

  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 6, defec_monitored_var_cstr);
  ASN1R_TAG_ADD (aCtx, CTX,          7, defec_get_eval_int);
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }

/************************************************************************/
/*			defec_monitored_var_cstr			*/
/* Decoding define event condition request: monitored variable was	*/
/* encountered.								*/
/************************************************************************/

static ST_VOID defec_monitored_var_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("defec_monitored_var_cstr");

  req_info->mon_var_pres = SD_TRUE; 	/* set present flag		*/

  _ms_get_va_var_spec (aCtx, &req_info->var_ref, defec_var_spec_done);
  }

/************************************************************************/
/*			defec_var_spec_done				*/
/************************************************************************/

static ST_VOID defec_var_spec_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("defec_var_spec_done");

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = defec_monitored_var_cstr_done;
  }

/************************************************************************/
/*			defec_monitored_var_cstr_done			*/
/* Decoding define event condition request: monitored variable constr	*/
/* done was encountered.						*/
/************************************************************************/

static ST_VOID defec_monitored_var_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
  ASN1R_TAG_ADD (aCtx, CTX, 7, defec_get_eval_int);
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }

/************************************************************************/
/*			defec_get_eval_int				*/
/* Decoding define event condition request: evaluation interval was	*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID defec_get_eval_int (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("defec_get_eval_int");

  req_info->eval_int_pres = SD_TRUE;
					/* read evaluation interval	*/
  if (asn1r_get_u32 (aCtx, &(req_info->eval_interval)))
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);

  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_defec_resp					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE and send the define event condition response.    	*/
/************************************************************************/

ST_RET mp_defec_resp (MMSREQ_IND *indptr)
  {
  return (_mms_send_null_resp (indptr,MMSOP_DEF_EC));
  }

#else
/************************************************************************/
/************************************************************************/
/*			mpl_defec_resp					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the define event condition response.    		*/
/************************************************************************/

ST_RET mpl_defec_resp (ST_UINT32 invoke_id)
  {
  return (_mms_fin_null_resp (invoke_id,MMSOP_DEF_EC));
  }
#endif
