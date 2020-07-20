/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rq_getaes.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the requester portion of get alarm 	*/
/*	enrollment summary.  It decodes the get alarm enrollment 	*/
/*	summary response (confirm) & encodes the get alarm enrollment	*/
/*	summary request.						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 02/24/10  JRB	   Del _mms_dec_info, use aCtx->mmsdec_rslt->data_ptr*/
/* 03/11/04  GLB     06    Remove "thisFileName"                    	*/
/* 12/20/01  JRB     05    Converted to use ASN1R (re-entrant ASN1)	*/
/* 04/28/00  JRB     04    Lint cleanup					*/
/* 09/13/99  MDE     03    Added SD_CONST modifiers			*/
/* 03/23/99  MDE     02    Changes to decode buffer allocation scheme	*/
/* 06/15/98  MDE     01    Changes to allow compile under C++		*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"

#include "mmsdefs.h"
#include "mms_pevn.h"
#include "asn1defs.h"

/************************************************************************/
/* variables global to the DECODE portion : NONE			*/

static  GETAES_RESP_INFO	*rsp_info;

static ALARM_ENROLL_SUMMARY   *aes_ptr;
static ST_INT max_enroll_sums;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

static ST_VOID getaes_aes_list_cstr         (ASN1_DEC_CTXT *aCtx);
static ST_VOID getaes_eename_cstr           (ASN1_DEC_CTXT *aCtx);
static ST_VOID getaes_eename_done           (ASN1_DEC_CTXT *aCtx);
static ST_VOID getaes_eename_cstr_done      (ASN1_DEC_CTXT *aCtx);
static ST_VOID getaes_client_app_cstr       (ASN1_DEC_CTXT *aCtx);
static ST_VOID getaes_client_app_done       (ASN1_DEC_CTXT *aCtx);
static ST_VOID getaes_client_app_cstr_done  (ASN1_DEC_CTXT *aCtx);
static ST_VOID getaes_get_severity          (ASN1_DEC_CTXT *aCtx);
static ST_VOID getaes_get_cur_state         (ASN1_DEC_CTXT *aCtx);
#ifdef CS_SUPPORT
static ST_VOID getaes_addl_detail_cstr      (ASN1_DEC_CTXT *aCtx);
static ST_VOID getaes_addl_detail_done      (ASN1_DEC_CTXT *aCtx);
static ST_VOID getaes_addl_detail_cstr_done (ASN1_DEC_CTXT *aCtx);
#endif
static ST_VOID getaes_get_not_lost          (ASN1_DEC_CTXT *aCtx);
static ST_VOID getaes_get_alarm_ack_rule    (ASN1_DEC_CTXT *aCtx);
static ST_VOID getaes_get_ee_state          (ASN1_DEC_CTXT *aCtx);
static ST_VOID getaes_tta_time_cstr         (ASN1_DEC_CTXT *aCtx);
static ST_VOID getaes_tta_time_done         (ASN1_DEC_CTXT *aCtx);
static ST_VOID getaes_tta_time_cstr_done    (ASN1_DEC_CTXT *aCtx);
static ST_VOID getaes_aack_time_cstr        (ASN1_DEC_CTXT *aCtx);
static ST_VOID getaes_aack_time_done        (ASN1_DEC_CTXT *aCtx);
static ST_VOID getaes_aack_time_cstr_done   (ASN1_DEC_CTXT *aCtx);
static ST_VOID getaes_tti_time_cstr         (ASN1_DEC_CTXT *aCtx);
static ST_VOID getaes_tti_time_done         (ASN1_DEC_CTXT *aCtx);
static ST_VOID getaes_tti_time_cstr_done    (ASN1_DEC_CTXT *aCtx);
static ST_VOID getaes_iack_time_cstr        (ASN1_DEC_CTXT *aCtx);
static ST_VOID getaes_iack_time_done        (ASN1_DEC_CTXT *aCtx);
static ST_VOID getaes_alarm_esum_cstr       (ASN1_DEC_CTXT *aCtx);
static ST_VOID getaes_alarm_esum_cstr_done  (ASN1_DEC_CTXT *aCtx);
static ST_VOID getaes_aes_list_cstr_done    (ASN1_DEC_CTXT *aCtx);
static ST_VOID getaes_get_more_follows      (ASN1_DEC_CTXT *aCtx);


/************************************************************************/
/************************************************************************/
/*			mms_getaes_rsp					*/
/* This function is called from MMSDEC when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_getaes_rsp (ASN1_DEC_CTXT *aCtx)
  {
ST_INT	 rsp_size;

  MLOG_CDEC0 ("Get Alarm Enrollment Summary Response");

#if defined(FIXED_DECBUF)
  max_enroll_sums = m_cl_max_enrollment_summary;
  rsp_size = mmsl_dec_info_size;
#else
  max_enroll_sums = _mmsdec_msglen/8;	/* determine maximum size	*/
  if (m_cl_max_enrollment_summary && (max_enroll_sums > m_cl_max_enrollment_summary))
    max_enroll_sums = m_cl_max_enrollment_summary;
  rsp_size = sizeof (GETAES_RESP_INFO)
		+ (max_enroll_sums * (sizeof (ALARM_ENROLL_SUMMARY)));
					/* get storage for resp info	*/
#endif

/* get storage for request information	*/
  rsp_info = (GETAES_RESP_INFO *) _m_get_dec_buf (aCtx, rsp_size);
  aes_ptr   = (ALARM_ENROLL_SUMMARY *) (rsp_info + 1);

  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 0, getaes_aes_list_cstr);
  }

/************************************************************************/
/*			getaes_aes_list_cstr				*/
/* decoding get alarm enrollment summary RESPONSE (confirm):	 	*/
/* alarm enrollment summary list constructor was encountered.	     	*/
/************************************************************************/

static ST_VOID getaes_aes_list_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("getaes_aes_list_cstr");

  ASN1R_TAG_ADD (aCtx, UNI | CONSTR, SEQ_CODE, getaes_alarm_esum_cstr);

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = getaes_aes_list_cstr_done;
  }

/************************************************************************/
/*			getaes_alarm_esum_cstr				*/
/* decoding get alarm enrollment summary RESPONSE (confirm):		*/
/* alarm enrollment summary was encountered.   				*/
/************************************************************************/

static ST_VOID getaes_alarm_esum_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("getaes_alarm_esum_cstr");

  rsp_info->num_of_alarm_esum++; 	/* inc alarm enrollment sum cnt	*/

  if (rsp_info->num_of_alarm_esum > max_enroll_sums)
    {
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);
    return;
    }

  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 0, getaes_eename_cstr);
  }

/************************************************************************/
/*			getaes_eename_cstr				*/
/* decoding get alarm enrollment summary RESPONSE (confirm):		*/
/* event enrollment name cstr was encountered.				*/
/************************************************************************/

static ST_VOID getaes_eename_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("getaes_eename_cstr");

  _ms_get_mms_objname (aCtx, &aes_ptr->evenroll_name, getaes_eename_done);
  }

/************************************************************************/
/*			getaes_eename_done  				*/
/* decoding get alarm enrollment summary RESPONSE (confirm):		*/
/* event enrollment name has been obtained.				*/
/************************************************************************/

static ST_VOID getaes_eename_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("getaes_eename_done");

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = getaes_eename_cstr_done;
  }

/************************************************************************/
/*			getaes_eename_cstr_done				*/
/* decoding get alarm enrollment summary RESPONSE (confirm):		*/
/* event enrollment name cstr done has been encountered. 		*/
/************************************************************************/

static ST_VOID getaes_eename_cstr_done (ASN1_DEC_CTXT *aCtx)
  {

  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 2, getaes_client_app_cstr);
  ASN1R_TAG_ADD (aCtx, CTX,          3, getaes_get_severity);
  }

/************************************************************************/
/*			getaes_client_app_cstr				*/
/* decoding get alarm enrollment summary RESPONSE (confirm):		*/
/* client application constructor was encountered.	 		*/
/************************************************************************/

static ST_VOID getaes_client_app_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("getaes_client_app_cstr");

  aes_ptr->client_app_pres = SD_TRUE;	/* set client app present	*/

  _ms_get_mms_app_ref (aCtx, &aes_ptr->client_app,&aes_ptr->client_app_len,
					 getaes_client_app_done);
  }

/************************************************************************/
/*			getaes_client_app_done     		       	*/
/* decoding get alarm enrollment summary RESPONSE (confirm):		*/
/* client application has been obtained.    				*/
/************************************************************************/

static ST_VOID getaes_client_app_done (ASN1_DEC_CTXT *aCtx)
  {

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = getaes_client_app_cstr_done;
  }

/************************************************************************/
/*			getaes_client_app_cstr_done 			*/
/* decoding get alarm enrollment summary RESPONSE (confirm):		*/
/* client application constructor done was encountered.	 		*/
/************************************************************************/

static ST_VOID getaes_client_app_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
  ASN1R_TAG_ADD (aCtx, CTX, 3, getaes_get_severity);
  }

/************************************************************************/
/*			getaes_get_severity				*/
/* decoding get alarm enrollment summary RESPONSE (confirm):		*/
/* severity was encountered.						*/
/************************************************************************/

static ST_VOID getaes_get_severity (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("getaes_get_severity");

  if (asn1r_get_u8 (aCtx, &aes_ptr->severity))		/* get the severity	*/
    {
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);
    return;
    }

  ASN1R_TAG_ADD (aCtx, CTX, 4, getaes_get_cur_state);
  }

/************************************************************************/
/*			getaes_get_cur_state 				*/
/* decoding get alarm enrollment summary RESPONSE (confirm):		*/
/* current state was encountered.					*/
/************************************************************************/

static ST_VOID getaes_get_cur_state (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("getaes_get_cur_state");

  if (asn1r_get_i16 (aCtx, &aes_ptr->cur_state))		/* get current state	*/
    {
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);
    return;
    }
  else if ((aes_ptr->cur_state < 0) || (aes_ptr->cur_state > 2))
    {
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);
    return;
    }

#ifdef CS_SUPPORT
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 5, getaes_addl_detail_cstr);
#endif
  ASN1R_TAG_ADD (aCtx, CTX,          6, getaes_get_not_lost);
  ASN1R_TAG_ADD (aCtx, CTX,          7, getaes_get_alarm_ack_rule);
  }
		       
#ifdef CS_SUPPORT
/************************************************************************/
/*			getaes_addl_detail_cstr				*/
/* decoding get alarm enrollment summary RESPONSE (confirm):		*/
/* additional detail constructor was encountered.   			*/
/************************************************************************/

static ST_VOID getaes_addl_detail_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("getaes_addl_detail_cstr");

  aes_ptr->addl_detail_pres = SD_TRUE;		/* set flag		*/
  aes_ptr->addl_detail = aCtx->asn1r_field_ptr;
  aes_ptr->addl_detail_len = aCtx->asn1r_elmnt_len;

  asn1r_parse_next (aCtx, getaes_addl_detail_done);		/* parse the contents	*/
  }

/************************************************************************/
/*			getaes_addl_detail_done				*/
/* decoding get alarm enrollment summary RESPONSE (confirm):		*/
/* additional detail has been obtained.		   			*/
/************************************************************************/

static ST_VOID getaes_addl_detail_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("getaes_addl_detail_done");

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = getaes_addl_detail_cstr_done;
  }

/************************************************************************/
/*			getaes_addl_detail_cstr_done			*/
/* decoding get alarm enrollment summary RESPONSE (confirm):		*/
/* additional detail constructor done was encountered.			*/
/************************************************************************/

static ST_VOID getaes_addl_detail_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
  ASN1R_TAG_ADD (aCtx, CTX, 6, getaes_get_not_lost);
  ASN1R_TAG_ADD (aCtx, CTX, 7, getaes_get_alarm_ack_rule);
  }
#endif

/************************************************************************/
/*			getaes_get_not_lost				*/
/* decoding get alarm enrollment summary RESPONSE (confirm):		*/
/* notification lost was encountered.	  				*/
/************************************************************************/

static ST_VOID getaes_get_not_lost (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("getaes_get_not_lost");

  if (asn1r_get_bool (aCtx, &(aes_ptr->not_lost)))    	/* get notification lost*/
    {
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);
    return;
    }

  ASN1R_TAG_ADD (aCtx, CTX, 7, getaes_get_alarm_ack_rule);
  }

/************************************************************************/
/*			getaes_get_alarm_ack_rule   			*/
/* decoding get alarm enrollment summary RESPONSE (confirm):		*/
/* alarm acknowledgment rule was encountered.  				*/
/************************************************************************/

static ST_VOID getaes_get_alarm_ack_rule (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("getaes_get_alarm_ack_rule");
					/* read alarm ack rule 		*/
  if (asn1r_get_i16 (aCtx, &(aes_ptr->alarm_ack_rule)))
    {
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);
    return;
    }
  else if ((aes_ptr->alarm_ack_rule < 0) || (aes_ptr->alarm_ack_rule > 3))
    {
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);
    return;
    }

  ASN1R_TAG_ADD (aCtx, CTX,          8, getaes_get_ee_state);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 9, getaes_tta_time_cstr);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR,10, getaes_aack_time_cstr);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR,11, getaes_tti_time_cstr);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR,12, getaes_iack_time_cstr);

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = getaes_alarm_esum_cstr_done;
  }

/************************************************************************/
/*			getaes_get_ee_state				*/
/* decoding get alarm enrollment summary RESPONSE (confirm):		*/
/* event enrollment state was encountered.	   			*/
/************************************************************************/

static ST_VOID getaes_get_ee_state (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("getaes_get_ee_state");

  aes_ptr->ee_state_pres = SD_TRUE;	/* set event cond state pres	*/

  if (asn1r_get_i16 (aCtx, &aes_ptr->ee_state))	/* get event cond state		*/
    {
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);
    return;
    }
  else if ((aes_ptr->ee_state < 0) || (aes_ptr->ee_state > 7))
    {
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);
    return;
    }
  
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 9, getaes_tta_time_cstr);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR,10, getaes_aack_time_cstr);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR,11, getaes_tti_time_cstr);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR,12, getaes_iack_time_cstr);
  }

/************************************************************************/
/*			getaes_tta_time_cstr				*/
/* decoding get alarm enrollment summary RESPONSE (confirm):		*/
/* transition to active time cstr was encountered.			*/
/************************************************************************/

static ST_VOID getaes_tta_time_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("getaes_tta_time_cstr");

  aes_ptr->tta_time_pres = SD_TRUE; 	/* set tran to active present	*/

  _ms_get_mms_evtime (aCtx, &aes_ptr->tta_time, getaes_tta_time_done);
  }

/************************************************************************/
/*			getaes_tta_time_done 				*/
/* decoding get alarm enrollment summary RESPONSE (confirm):		*/
/* transition to active time has been obtained. 			*/
/************************************************************************/

static ST_VOID getaes_tta_time_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("getaes_tta_time_done");

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = getaes_tta_time_cstr_done;
  }

/************************************************************************/
/*			getaes_tta_time_cstr_done			*/
/* decoding get alarm enrollment summary RESPONSE (confirm):		*/
/* transition to active time cstr done has been encountered.		*/
/************************************************************************/

static ST_VOID getaes_tta_time_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR,10, getaes_aack_time_cstr);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR,11, getaes_tti_time_cstr);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR,12, getaes_iack_time_cstr);
  }

/************************************************************************/
/*			getaes_aack_time_cstr				*/
/* decoding get alarm enrollment summary RESPONSE (confirm):		*/
/* active acknowledgment time cstr was encountered.			*/
/************************************************************************/

static ST_VOID getaes_aack_time_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("getaes_aack_time_cstr");

  aes_ptr->aack_time_pres = SD_TRUE; 	/* active acknowledgment pres	*/

  _ms_get_mms_evtime (aCtx, &aes_ptr->aack_time, getaes_aack_time_done);
  }

/************************************************************************/
/*			getaes_aack_time_done  				*/
/* decoding get alarm enrollment summary RESPONSE (confirm):		*/
/* active acknowledgment time has been obtained. 			*/
/************************************************************************/

static ST_VOID getaes_aack_time_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("getaes_aack_time_done");

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = getaes_aack_time_cstr_done;
  }

/************************************************************************/
/*			getaes_aack_time_cstr_done			*/
/* decoding get alarm enrollment summary RESPONSE (confirm):		*/
/* active acknowledgment time cstr done was encountered.  		*/
/************************************************************************/

static ST_VOID getaes_aack_time_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR,11, getaes_tti_time_cstr);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR,12, getaes_iack_time_cstr);
  }

/************************************************************************/
/*			getaes_tti_time_cstr				*/
/* decoding get alarm enrollment summary RESPONSE (confirm):		*/
/* transition to idle time cstr was encountered.			*/
/************************************************************************/

static ST_VOID getaes_tti_time_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("getaes_tti_time_cstr");

  aes_ptr->tti_time_pres = SD_TRUE; 	/* set tran to idle present	*/

  _ms_get_mms_evtime (aCtx, &aes_ptr->tti_time, getaes_tti_time_done);
  }

/************************************************************************/
/*			getaes_tti_time_done  				*/
/* decoding get alarm enrollment summary RESPONSE (confirm):		*/
/* transition to idle time has been obtained.				*/
/************************************************************************/

static ST_VOID getaes_tti_time_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("getaes_tti_time_done");

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = getaes_tti_time_cstr_done;
  }

/************************************************************************/
/*			getaes_tti_time_cstr_done			*/
/* decoding get alarm enrollment summary RESPONSE (confirm):		*/
/* transition to idle time cstr done was encountered.			*/
/************************************************************************/

static ST_VOID getaes_tti_time_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR,12, getaes_iack_time_cstr);
  }

/************************************************************************/
/*			getaes_iack_time_cstr				*/
/* decoding get alarm enrollment summary RESPONSE (confirm):		*/
/* idle acknowledgment time cstr was encountered.			*/
/************************************************************************/

static ST_VOID getaes_iack_time_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("getaes_iack_time_cstr");

  aes_ptr->iack_time_pres = SD_TRUE; 	/* idle acknowledgment present	*/

  _ms_get_mms_evtime (aCtx, &aes_ptr->iack_time, getaes_iack_time_done);
  }

/************************************************************************/
/*			getaes_iack_time_done  				*/
/* decoding get alarm enrollment summary RESPONSE (confirm):		*/
/* idle acknowledgment time has been obtained. 				*/
/************************************************************************/

static ST_VOID getaes_iack_time_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("getaes_iack_time_done");

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = NULL;
  }

/************************************************************************/
/*			getaes_alarm_esum_cstr_done			*/
/* decoding get alarm enrollment summary RESPONSE (confirm):  alarm	*/
/* summary constructor done was encountered.				*/
/************************************************************************/

static ST_VOID getaes_alarm_esum_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
  aes_ptr++;				/* point to where next will go	*/

  ASN1R_TAG_ADD (aCtx, UNI | CONSTR, SEQ_CODE, getaes_alarm_esum_cstr);
  }

/************************************************************************/
/*			getaes_aes_list_cstr_done 			*/
/* decoding get alarm enrollment summary RESPONSE (confirm): alarm 	*/
/* summary list constructor done was encountered.			*/
/************************************************************************/

static ST_VOID getaes_aes_list_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
ST_INT rsp_size;
				/* find actual size for response info	*/
  rsp_size = sizeof (GETAES_RESP_INFO)
	   + (sizeof (ALARM_ENROLL_SUMMARY) * rsp_info->num_of_alarm_esum);

				/* give back unused storage		*/
  aCtx->mmsdec_rslt->data_ptr = (*m_realloc_os_fun) (aCtx->mmsdec_rslt->data_ptr,rsp_size);
  rsp_info = (GETAES_RESP_INFO *) aCtx->mmsdec_rslt->data_ptr;

  ASN1R_TAG_ADD (aCtx, CTX, 1, getaes_get_more_follows);
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }

/************************************************************************/
/*			getaes_get_more_follows				*/
/* decoding get alarm enrollment summary RESPONSE (confirm): more		*/
/* follows indicator was encountered.					*/
/************************************************************************/

static ST_VOID getaes_get_more_follows (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("getaes_get_more_follows");

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
/*			mp_getaes					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE and send the decoding get alarm enrollment summary	*/
/* request.								*/
/************************************************************************/

MMSREQ_PEND *mp_getaes (ST_INT chan, GETAES_REQ_INFO *info_ptr)
  {
  return (_mms_req_send (MMSOP_GET_ALARM_ESUM,
			M_CAST_MK_FUN (_ms_mk_getaes_req),
			chan,(ST_CHAR *)info_ptr));
  }

#else
/************************************************************************/
/************************************************************************/
/*			mpl_getaes					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the decoding get alarm enrollment summary request.	*/
/************************************************************************/

ST_RET  mpl_getaes (GETAES_REQ_INFO *info_ptr)
  {
  return (_mms_req_fin (MMSOP_GET_ALARM_ESUM,
			M_CAST_MK_FUN (_ms_mk_getaes_req),
			(ST_CHAR *)info_ptr));
  }
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_getaes_req					*/
/* ENCODE a decoding get alarm enrollment summary REQUEST:					*/
/************************************************************************/

ST_VOID _ms_mk_getaes_req (ASN1_ENC_CTXT *aCtx, GETAES_REQ_INFO *info)
  {
  if (info->ca_name_pres)
    {
    asn1r_strt_constr (aCtx);			/* start ctx tag 5 cstr 	*/
    _ms_wr_mms_objname (aCtx, &info->ca_name);	/* write continue after name	*/
    asn1r_fin_constr (aCtx, 5,CTX,DEF);		/* finish ctx tag 5 cstr	*/
    }

  if (info->least_sev_filter != 127 || /* not the defaults		*/
       info->most_sev_filter != 0) 
    {
    asn1r_strt_constr (aCtx);			/* start ctx tag 3 cstr 	*/
    asn1r_wr_u8 (aCtx, info->least_sev_filter);	/* write least severe filter	*/
    asn1r_fin_prim (aCtx, 1,CTX);

    asn1r_wr_u8 (aCtx, info->most_sev_filter);	/* write most severe filter	*/
    asn1r_fin_prim (aCtx, 0,CTX);
    asn1r_fin_constr (aCtx, 3,CTX,DEF);		/* finish ctx tag 3 cstr	*/
    }

  if (info->ack_filter != SD_FALSE)
    {
    asn1r_wr_i16 (aCtx, info->ack_filter);		/* write acknowledge filter	*/
    asn1r_fin_prim (aCtx, 2,CTX);
    }

  if (!info->act_alarms_only)
    {
    asn1r_wr_bool (aCtx, info->act_alarms_only);	/* write active alarms only	*/
    asn1r_fin_prim (aCtx, 1,CTX);
    }

  if (!info->enroll_only)
    {
    asn1r_wr_bool (aCtx, info->enroll_only);		/* write enrollments only	*/
    asn1r_fin_prim (aCtx, 0,CTX);
    }

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_constr (aCtx, MMSOP_GET_ALARM_ESUM,CTX,DEF);	/* tag = opcode, context	*/
  }
