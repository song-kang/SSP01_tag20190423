/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rs_evnot.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the responder portion of event		*/
/*	notification.  It decodes the event notification request	*/
/*	(indication).  There is no response.				*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*	ST_VOID  mms_evnot_req ()					*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 03/11/04  GLB     08    Remove "thisFileName"                    	*/
/* 12/20/01  JRB     07    Converted to use ASN1R (re-entrant ASN1)	*/
/* 04/28/00  JRB     06    More Lint					*/
/* 04/27/00  MDE     05    Lint cleanup					*/
/* 09/13/99  MDE     04    Added SD_CONST modifiers			*/
/* 03/23/99  MDE     03    Changes to decode buffer allocation scheme	*/
/* 07/03/97  MDE     02    Op-specific info buffer handling changes	*/
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

static  EVNOT_REQ_INFO	*req_info;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

static ST_VOID evnot_enrol_name_cstr       (ASN1_DEC_CTXT *aCtx);
static ST_VOID evnot_enrol_name_done       (ASN1_DEC_CTXT *aCtx);
static ST_VOID evnot_enrol_name_cstr_done  (ASN1_DEC_CTXT *aCtx);
static ST_VOID evnot_cond_name_cstr        (ASN1_DEC_CTXT *aCtx);
static ST_VOID evnot_cond_name_done        (ASN1_DEC_CTXT *aCtx);
static ST_VOID evnot_cond_name_cstr_done   (ASN1_DEC_CTXT *aCtx);
static ST_VOID evnot_get_severity          (ASN1_DEC_CTXT *aCtx);
static ST_VOID evnot_get_state             (ASN1_DEC_CTXT *aCtx);
static ST_VOID evnot_evtime_cstr           (ASN1_DEC_CTXT *aCtx);
static ST_VOID evnot_evtime_done           (ASN1_DEC_CTXT *aCtx);
static ST_VOID evnot_evtime_cstr_done      (ASN1_DEC_CTXT *aCtx);
static ST_VOID evnot_get_not_lost          (ASN1_DEC_CTXT *aCtx);
static ST_VOID evnot_get_alarm_ack_rule    (ASN1_DEC_CTXT *aCtx);
static ST_VOID evnot_action_result_cstr    (ASN1_DEC_CTXT *aCtx);
static ST_VOID evnot_ar_ean_done           (ASN1_DEC_CTXT *aCtx);
static ST_VOID evnot_ar_get_success        (ASN1_DEC_CTXT *aCtx);
static ST_VOID evnot_ars_csr_done          (ASN1_DEC_CTXT *aCtx);
static ST_VOID evnot_ar_get_failure        (ASN1_DEC_CTXT *aCtx);
#ifdef MOD_SUPPORT
static ST_VOID evnot_arf_get_mod_pos       (ASN1_DEC_CTXT *aCtx);
#endif
static ST_VOID evnot_arf_get_serv_err      (ASN1_DEC_CTXT *aCtx);
#ifdef CS_SUPPORT
static ST_VOID evnot_ars_cs_rdet_cstr      (ASN1_DEC_CTXT *aCtx);
static ST_VOID evnot_ars_cs_rdet_done      (ASN1_DEC_CTXT *aCtx);
static ST_VOID evnot_ars_cs_rdet_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID evnot_action_result_cstr_done (ASN1_DEC_CTXT *aCtx);
#endif

/************************************************************************/
/************************************************************************/
/*			mms_evnot_req					*/
/* This function is called from MMSDEC when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_evnot_req (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("Event Notification Request");

/* get storage for request information	*/
  req_info = (EVNOT_REQ_INFO *) _m_get_dec_buf (aCtx, sizeof (EVNOT_REQ_INFO) + sizeof (ERR_INFO));
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 0, evnot_enrol_name_cstr);
  }

/************************************************************************/
/*			evnot_enrol_name_cstr				*/
/* decoding event notification request: event enrollment name cstr	*/
/* was encountered.							*/
/************************************************************************/

static ST_VOID evnot_enrol_name_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("evnot_enrol_name_cstr");

  _ms_get_mms_objname (aCtx, &req_info->evenroll_name, evnot_enrol_name_done);
  }

/************************************************************************/
/*			evnot_enrol_name_done				*/
/* decoding event notification request: event enrollment name has been	*/
/* obtained.								*/
/************************************************************************/

static ST_VOID evnot_enrol_name_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("evnot_enrol_name_done");

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = evnot_enrol_name_cstr_done;
  }

/************************************************************************/
/*			evnot_enrol_name_cstr_done			*/
/* decoding event notification request: event enrollment name cstr	*/
/* has been decoded OK.							*/
/************************************************************************/

static ST_VOID evnot_enrol_name_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("evnot_enrol_name_cstr_done");

  ASN1R_TAG_ADD (aCtx, CTX|CONSTR, 1, evnot_cond_name_cstr);
  }

/************************************************************************/
/*			evnot_cond_name_cstr				*/
/* decoding event notification request: event condition name cstr	*/
/* was encountered.							*/
/************************************************************************/

static ST_VOID evnot_cond_name_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("evnot_cond_name_cstr");

  _ms_get_mms_objname (aCtx, &req_info->evcon_name, evnot_cond_name_done);
  }

/************************************************************************/
/*			evnot_cond_name_done  				*/
/* decoding event notification request: event condition name has been	*/
/* obtained.								*/
/************************************************************************/

static ST_VOID evnot_cond_name_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("evnot_cond_name_done");

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = evnot_cond_name_cstr_done;
  }

/************************************************************************/
/*			evnot_cond_name_cstr_done			*/
/* decoding event notification request: event condition name cstr	*/
/* has been encountered.						*/
/************************************************************************/

static ST_VOID evnot_cond_name_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("evnot_cond_name_cstr_done");

  ASN1R_TAG_ADD (aCtx, CTX, 2, evnot_get_severity);
  }

/************************************************************************/
/*			evnot_get_severity				*/
/* event notification REQUEST is being decoded: severity was		*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID evnot_get_severity (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("evnot_get_severity");

  if (asn1r_get_u8 (aCtx, &(req_info->severity)))		/* get severity 	*/
    {
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);
    return;
    }

  ASN1R_TAG_ADD (aCtx, CTX, 	      3, evnot_get_state);
  ASN1R_TAG_ADD (aCtx, CTX|CONSTR, 4, evnot_evtime_cstr);
  }

/************************************************************************/
/*			evnot_get_state 				*/
/* event notification REQUEST is being decoded: state was encountered.	*/
/************************************************************************/

static ST_VOID evnot_get_state (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("evnot_get_state");

  if (asn1r_get_i16 (aCtx, &(req_info->cur_state)) ||	/* get current state	*/
      (req_info->cur_state < 0) ||
      (req_info->cur_state > 2))
    {
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);
    return;
    }
  req_info->cur_state_pres = SD_TRUE;    		/* set present flag	*/

  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 4, evnot_evtime_cstr);
  }

/************************************************************************/
/*			evnot_evtime_cstr				*/
/* decoding event notification request: event time constructor	was	*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID evnot_evtime_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("evnot_evtime_cstr");

  _ms_get_mms_evtime (aCtx, &req_info->trans_time, evnot_evtime_done);
  }

/************************************************************************/
/*			evnot_evtime_done 				*/
/* decoding event notification request: event time has been obtained.	*/
/************************************************************************/

static ST_VOID evnot_evtime_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("evnot_evtime_done");

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = evnot_evtime_cstr_done;
  }

/************************************************************************/
/*			evnot_evtime_cstr_done				*/
/* decoding event notification request: event time constructor done	*/
/* has been encountered.						*/
/************************************************************************/

static ST_VOID evnot_evtime_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
  ASN1R_TAG_ADD (aCtx, CTX, 	 	6, evnot_get_not_lost);
  ASN1R_TAG_ADD (aCtx, CTX, 	 	7, evnot_get_alarm_ack_rule);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 8, evnot_action_result_cstr);

  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }

/************************************************************************/
/*			evnot_get_not_lost				*/
/* decoding event notification request: notification lost has been	*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID evnot_get_not_lost (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("evnot_get_not_lost");

  if (asn1r_get_bool (aCtx, &(req_info->not_lost)))   	/* notification lost	*/
    {
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);
    return;
    }

  ASN1R_TAG_ADD (aCtx, CTX, 	 	7, evnot_get_alarm_ack_rule);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 8, evnot_action_result_cstr);
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }

/************************************************************************/
/*			evnot_get_alarm_ack_rule			*/
/* decoding event notification request: alarm ack rule has been 	*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID evnot_get_alarm_ack_rule (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("evnot_get_alarm_ack_rule");

  req_info->alarm_ack_rule_pres = SD_TRUE; /* set alarm ack rult present	*/

  if (asn1r_get_i16 (aCtx, &(req_info->alarm_ack_rule)))	/* get alarm ack rule	*/
    {
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);
    return;
    }
  else if ((req_info->alarm_ack_rule < 0) || (req_info->alarm_ack_rule > 3))
    {
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);
    return;
    }

  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 8, evnot_action_result_cstr);
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }

/************************************************************************/
/*			evnot_action_result_cstr			*/
/* decoding event notification request: action result constructor	*/
/* was encountered.							*/
/************************************************************************/

static ST_VOID evnot_action_result_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("evnot_action_result_cstr");

  req_info->evact_result_pres = SD_TRUE;	/* set action result present	*/

  _ms_get_mms_objname (aCtx, &req_info->evact_name, evnot_ar_ean_done);
  }

/************************************************************************/
/*			evnot_ar_ean_done 				*/
/* decoding event notification request: action result event action name	*/
/* has been obtained.							*/
/************************************************************************/

static ST_VOID evnot_ar_ean_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("evnot_ar_ean_done");

  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 0, evnot_ar_get_success);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 1, evnot_ar_get_failure);
  }

/************************************************************************/
/*			evnot_ar_get_success				*/
/* decoding event notification request: action result success has	*/
/* been encountered, get the confirmed service response.		*/
/************************************************************************/

static ST_VOID evnot_ar_get_success (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("evnot_ar_get_success");

  req_info->evact_result_tag = 0;	/* set success action result	*/

	/* leave confirmed service response in place; just parse it. 	*/
  req_info->conf_serv_resp = aCtx->asn1r_field_ptr;
  asn1r_parse_next (aCtx, evnot_ars_csr_done);
  }

/************************************************************************/
/*			evnot_ars_csr_done 				*/
/* event notification request is being decoded: action result success:	*/
/* confirmed service response has been parsed.  			*/
/************************************************************************/

static ST_VOID evnot_ars_csr_done (ASN1_DEC_CTXT *aCtx)
  {
  req_info->conf_serv_resp_len = aCtx->asn1r_octetcount;	/* save length		*/
#ifdef CS_SUPPORT	/* check for Companion Standard response detail	*/
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 79, evnot_ars_cs_rdet_cstr);
#endif

  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }

#ifdef CS_SUPPORT
/************************************************************************/
/*			evnot_ars_cs_rdet_cstr				*/
/* decoding event notification request: action result success:		*/
/* companion standard response detail constructor has been encountered.	*/
/************************************************************************/

static ST_VOID evnot_ars_cs_rdet_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("evnot_ars_cs_rdet_cstr");

  req_info->cs_rdetail_pres = SD_TRUE;		/* set the present flag	*/

		/* leave CS response detail in place; just parse it. 	*/
  req_info->cs_rdetail = aCtx->asn1r_field_ptr;
  asn1r_parse_next (aCtx, evnot_ars_cs_rdet_done);
  }

/************************************************************************/
/*			evnot_ars_cs_rdet_done				*/
/* event notification request is being decoded: action result success:	*/
/* companion standard response detail has been parsed.  		*/
/************************************************************************/

static ST_VOID evnot_ars_cs_rdet_done (ASN1_DEC_CTXT *aCtx)
  {
  req_info->cs_rdetail_len = aCtx->asn1r_octetcount;	/* save length		*/

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = evnot_ars_cs_rdet_cstr_done;
  }

/************************************************************************/
/*			evnot_ars_cs_rdet_cstr_done			*/
/* event notification request is being decoded: action result success:	*/
/* confirmed service response cstr done has been encountered.		*/
/************************************************************************/

static ST_VOID evnot_ars_cs_rdet_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = evnot_action_result_cstr_done;
  }
#endif

/************************************************************************/
/*			evnot_ar_get_failure				*/
/* decoding event notification request: action result failure has	*/
/* been encountered.							*/
/************************************************************************/

static ST_VOID evnot_ar_get_failure (ASN1_DEC_CTXT *aCtx)
  {

  MLOG_CDEC0 ("evnot_ar_get_failure");

  req_info->evact_result_tag = 1;	/* get failure action result	*/

#ifdef MOD_SUPPORT		/* IS, could have modifier position 	*/
  ASN1R_TAG_ADD (aCtx, CTX, 0, evnot_arf_get_mod_pos);
#endif
  ASN1R_TAG_ADD (aCtx, CTX|CONSTR, 1, evnot_arf_get_serv_err);
  }

#ifdef MOD_SUPPORT
/************************************************************************/
/*			evnot_arf_get_mod_pos				*/
/* decoding event notification request: action result failure: modifier	*/
/* position has been encountered. 					*/
/************************************************************************/

static ST_VOID evnot_arf_get_mod_pos (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("evnot_arf_get_mod_pos");

  req_info->mod_pos_pres = SD_TRUE;	/* set modifier pos present	*/

  if (asn1r_get_u32 (aCtx, &(req_info->mod_pos)))	/* get modifier position	*/
    {
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);
    return;
    }

  ASN1R_TAG_ADD (aCtx, CTX|CONSTR, 1, evnot_arf_get_serv_err);
  }
#endif

/************************************************************************/
/*			evnot_arf_get_serv_err				*/
/* event notification request is being decoded: action result failure:	*/
/* service error constructor has been encountered.		 	*/
/************************************************************************/

static ST_VOID evnot_arf_get_serv_err (ASN1_DEC_CTXT *aCtx)
  {

  MLOG_CDEC0 ("evnot_arf_get_serv_err");

				/* set up pointer (already allocated)	*/
  req_info->serv_err = (ERR_INFO *) (req_info+1);
				/* decode service error contents	*/
  _mms_get_service_err (aCtx, req_info->serv_err);
  }

/************************************************************************/
/*			evnot_action_result_cstr_done			*/
/* decoding event notification request: action_result_cstr_done has	*/
/* been encountered.							*/
/************************************************************************/

#ifdef CS_SUPPORT

static ST_VOID evnot_action_result_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }

#endif
