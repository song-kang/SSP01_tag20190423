/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rq_getas.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the requester portion of get alarm 	*/
/*	summary.  It decodes the get alarm summary response (confirm)	*/
/*	& encodes the get alarm summary request.			*/
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

static  GETAS_RESP_INFO	*rsp_info;

static ALARM_SUMMARY   *as_ptr;
static ST_INT max_summaries;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

/* static functions with multiple references                            */
static ST_VOID getas_as_list_cstr          (ASN1_DEC_CTXT *aCtx);
static ST_VOID getas_alarm_sum_cstr        (ASN1_DEC_CTXT *aCtx);
static ST_VOID getas_cond_name_cstr        (ASN1_DEC_CTXT *aCtx);
static ST_VOID getas_cond_name_done        (ASN1_DEC_CTXT *aCtx);
static ST_VOID getas_cond_name_cstr_done   (ASN1_DEC_CTXT *aCtx);
static ST_VOID getas_get_severity          (ASN1_DEC_CTXT *aCtx);
static ST_VOID getas_get_state             (ASN1_DEC_CTXT *aCtx);
static ST_VOID getas_get_unack_state       (ASN1_DEC_CTXT *aCtx);
#ifdef CS_SUPPORT
static ST_VOID getas_addl_detail_cstr      (ASN1_DEC_CTXT *aCtx);
static ST_VOID getas_addl_detail_done      (ASN1_DEC_CTXT *aCtx);
static ST_VOID getas_addl_detail_cstr_done (ASN1_DEC_CTXT *aCtx);
#endif
static ST_VOID getas_tta_time_cstr         (ASN1_DEC_CTXT *aCtx);
static ST_VOID getas_tta_time_done         (ASN1_DEC_CTXT *aCtx);
static ST_VOID getas_tta_time_cstr_done    (ASN1_DEC_CTXT *aCtx);
static ST_VOID getas_tti_time_cstr         (ASN1_DEC_CTXT *aCtx);
static ST_VOID getas_tti_time_done         (ASN1_DEC_CTXT *aCtx);
static ST_VOID getas_alarm_sum_cstr_done   (ASN1_DEC_CTXT *aCtx);
static ST_VOID getas_as_list_cstr_done     (ASN1_DEC_CTXT *aCtx);
static ST_VOID getas_get_more_follows      (ASN1_DEC_CTXT *aCtx);


/************************************************************************/
/************************************************************************/
/*			mms_getas_rsp					*/
/* This function is called from MMSDEC when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_getas_rsp (ASN1_DEC_CTXT *aCtx)
  {
ST_INT rsp_size;

  MLOG_CDEC0 ("Get Alarm Summary Response");

#if defined(FIXED_DECBUF)
  max_summaries = m_cl_max_alarm_summary;
  rsp_size = mmsl_dec_info_size;
#else
  max_summaries = _mmsdec_msglen/8; /* determine maximum size	*/
  if (m_cl_max_alarm_summary && (max_summaries > m_cl_max_alarm_summary))
    max_summaries = m_cl_max_alarm_summary;
					
/* get storage for resp info	*/
  rsp_size = sizeof (GETAS_RESP_INFO)
		+ (max_summaries * (sizeof (ALARM_SUMMARY)));
#endif

/* get storage for request information	*/
  rsp_info = (GETAS_RESP_INFO *) _m_get_dec_buf (aCtx, rsp_size);
  as_ptr   = (ALARM_SUMMARY *) (rsp_info + 1);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 0, getas_as_list_cstr);
  }

/************************************************************************/
/*			getas_as_list_cstr				*/
/* get alarm summary RESPONSE (confirm) is being decoded:		*/
/* alarm summary list constructor was encountered.			*/
/************************************************************************/

static ST_VOID getas_as_list_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("getas_as_list_cstr");

  ASN1R_TAG_ADD (aCtx, UNI | CONSTR, SEQ_CODE, getas_alarm_sum_cstr);
  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = getas_as_list_cstr_done;
  }

/************************************************************************/
/*			getas_alarm_sum_cstr				*/
/* get alarm summary RESPONSE (confirm) is being decoded:		*/
/* alarm summary was encountered.					*/
/************************************************************************/

static ST_VOID getas_alarm_sum_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("getas_alarm_sum_cstr");

  rsp_info->num_of_alarm_sum++; 	/* increment alarm summary cnt	*/

  if (rsp_info->num_of_alarm_sum > max_summaries)
    {
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);
    return;
    }

  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 0, getas_cond_name_cstr);
  }

/************************************************************************/
/*			getas_cond_name_cstr				*/
/* get alarm summary RESPONSE (confirm) is being decoded:		*/
/* event condition name cstr was encountered.				*/
/************************************************************************/

static ST_VOID getas_cond_name_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("getas_cond_name_cstr");

  _ms_get_mms_objname (aCtx, &as_ptr->evcon_name, getas_cond_name_done);
  }

/************************************************************************/
/*			getas_cond_name_done				*/
/* get alarm summary RESPONSE (confirm) is being decoded:		*/
/* event condition name has been obtained.				*/
/************************************************************************/

static ST_VOID getas_cond_name_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("getas_cond_name_done");

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = getas_cond_name_cstr_done;
  }

/************************************************************************/
/*			getas_cond_name_cstr_done			*/
/* get alarm summary RESPONSE (confirm) is being decoded:		*/
/* event condition name cstr done has been encountered. 		*/
/************************************************************************/

static ST_VOID getas_cond_name_cstr_done (ASN1_DEC_CTXT *aCtx)
  {

  ASN1R_TAG_ADD (aCtx, CTX, 1, getas_get_severity);
  }

/************************************************************************/
/*			getas_get_severity				*/
/* get alarm summary RESPONSE (confirm) is being decoded:		*/
/* severity was encountered.						*/
/************************************************************************/

static ST_VOID getas_get_severity (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("getas_get_severity");

  if (asn1r_get_u8 (aCtx, &as_ptr->severity))		/* get the severity	*/
    {
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);
    return;
    }

  ASN1R_TAG_ADD (aCtx, CTX, 2, getas_get_state);
  }

/************************************************************************/
/*			getas_get_state 				*/
/* get alarm summary RESPONSE (confirm) is being decoded:		*/
/* current state was encountered.					*/
/************************************************************************/

static ST_VOID getas_get_state (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("getas_get_state");

  if (asn1r_get_i16 (aCtx, &as_ptr->cur_state))		/* get current state	*/
    {
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);
    return;
    }
  else if ((as_ptr->cur_state < 0) || (as_ptr->cur_state > 2))
    {
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);
    return;
    }

  ASN1R_TAG_ADD (aCtx, CTX, 3, getas_get_unack_state);
  }

/************************************************************************/
/*			getas_get_unack_state				*/
/* get alarm summary RESPONSE (confirm) is being decoded:		*/
/* unack state was encountered. 					*/
/************************************************************************/

static ST_VOID getas_get_unack_state (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("getas_get_unack_state");

  if (asn1r_get_i16 (aCtx, &as_ptr->unack_state))		/* get unack state	*/
    {
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);
    return;
    }
  else if ((as_ptr->unack_state < 0) || (as_ptr->unack_state > 3))
    {
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);
    return;
    }

#ifdef CS_SUPPORT
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 4, getas_addl_detail_cstr);
#endif
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 5, getas_tta_time_cstr);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 6, getas_tti_time_cstr);

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = getas_alarm_sum_cstr_done;
  }

#ifdef CS_SUPPORT
/************************************************************************/
/*			getas_addl_detail_cstr				*/
/* get alarm summary RESPONSE (confirm) is being decoded:  additional	*/
/* detail was encountered.						*/
/************************************************************************/

static ST_VOID getas_addl_detail_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("getas_addl_detail_cstr");

  as_ptr->addl_detail_pres = SD_TRUE;	/* set add'nl detail present    */

		/* leave additional detail in place; just parse it.	*/
  as_ptr->addl_detail = aCtx->asn1r_field_ptr;
  asn1r_parse_next (aCtx, getas_addl_detail_done);
  }

/************************************************************************/
/*			getas_addl_detail_done				*/
/* get alarm summary RESPONSE (confirm) is being decoded:  additional	*/
/* detail has been parsed.						*/
/************************************************************************/

static ST_VOID getas_addl_detail_done (ASN1_DEC_CTXT *aCtx)
  {

  as_ptr->addl_detail_len = aCtx->asn1r_octetcount; 	/* save length		*/

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = getas_addl_detail_cstr_done;
  }

/************************************************************************/
/*			getas_addl_detail_cstr_done			*/
/* get alarm summary RESPONSE (confirm) is being decoded:  additional	*/
/* detail cstr done has been encountered.				*/
/************************************************************************/

static ST_VOID getas_addl_detail_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 5, getas_tta_time_cstr);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 6, getas_tti_time_cstr);
  }
#endif

/************************************************************************/
/*			getas_tta_time_cstr				*/
/* get alarm summary RESPONSE (confirm) is being decoded:		*/
/* transition to active time cstr was encountered.			*/
/************************************************************************/

static ST_VOID getas_tta_time_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("getas_tta_time_cstr");

  as_ptr->tta_time_pres = SD_TRUE; 	/* set tran to active present	*/

  _ms_get_mms_evtime (aCtx, &as_ptr->tta_time, getas_tta_time_done);
  }

/************************************************************************/
/*			getas_tta_time_done				*/
/* get alarm summary RESPONSE (confirm) is being decoded:		*/
/* transition to active time has been obtained. 			*/
/************************************************************************/

static ST_VOID getas_tta_time_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("getas_tta_time_done");

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = getas_tta_time_cstr_done;
  }

/************************************************************************/
/*			getas_tta_time_cstr_done			*/
/* get alarm summary RESPONSE (confirm) is being decoded:		*/
/* transition to active time cstr done has been encountered.		*/
/************************************************************************/

static ST_VOID getas_tta_time_cstr_done (ASN1_DEC_CTXT *aCtx)
  {

  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 6, getas_tti_time_cstr);
  }

/************************************************************************/
/*			getas_tti_time_cstr				*/
/* get alarm summary RESPONSE (confirm) is being decoded:		*/
/* transition to idle time cstr was encountered.			*/
/************************************************************************/

static ST_VOID getas_tti_time_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("getas_tti_time_cstr");

  as_ptr->tti_time_pres = SD_TRUE; 	/* set tran to active present	*/

  _ms_get_mms_evtime (aCtx, &as_ptr->tti_time, getas_tti_time_done);
  }

/************************************************************************/
/*			getas_tti_time_done				*/
/* get alarm summary RESPONSE (confirm) is being decoded:		*/
/* transition to idle time has been obtained.				*/
/************************************************************************/

static ST_VOID getas_tti_time_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("getas_tti_time_done");

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = NULL;
  }

/************************************************************************/
/*			getas_alarm_sum_cstr_done			*/
/* get alarm summary RESPONSE (confirm) is being decoded:  alarm	*/
/* summary constructor done was encountered.				*/
/************************************************************************/

static ST_VOID getas_alarm_sum_cstr_done (ASN1_DEC_CTXT *aCtx)
  {

  as_ptr++;				/* point to where next will go	*/

  ASN1R_TAG_ADD (aCtx, UNI | CONSTR, SEQ_CODE, getas_alarm_sum_cstr);
  }

/************************************************************************/
/*			getas_as_list_cstr_done 			*/
/* get alarm summary RESPONSE (confirm) is being decoded: alarm 	*/
/* summary list constructor done was encountered.			*/
/************************************************************************/

static ST_VOID getas_as_list_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
ST_INT rsp_size;

  ASN1R_TAG_ADD (aCtx, CTX, 1, getas_get_more_follows);
				/* find actual size for response info	*/
  rsp_size = sizeof (GETAS_RESP_INFO)
	   + (sizeof (ALARM_SUMMARY) * rsp_info->num_of_alarm_sum);

				/* give back unused storage		*/
  aCtx->mmsdec_rslt->data_ptr = (*m_realloc_os_fun) (aCtx->mmsdec_rslt->data_ptr,rsp_size);
  rsp_info = (GETAS_RESP_INFO *) aCtx->mmsdec_rslt->data_ptr;
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }

/************************************************************************/
/*			getas_get_more_follows				*/
/* get alarm summary RESPONSE (confirm) is being decoded: more		*/
/* follows indicator was encountered.					*/
/************************************************************************/

static ST_VOID getas_get_more_follows (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("getas_get_more_follows");

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
/*			mp_getas					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE and send the get alarm summary request.		*/
/************************************************************************/

MMSREQ_PEND *mp_getas (ST_INT chan, GETAS_REQ_INFO *info_ptr)
  {
  return (_mms_req_send (MMSOP_GET_ALARM_SUM,
			M_CAST_MK_FUN (_ms_mk_getas_req),
			chan,(ST_CHAR *)info_ptr));
  }

#else
/************************************************************************/
/************************************************************************/
/*			mpl_getas					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the get alarm summary request.			*/
/************************************************************************/

ST_RET  mpl_getas (GETAS_REQ_INFO *info_ptr)
  {
  return (_mms_req_fin (MMSOP_GET_ALARM_SUM,
			M_CAST_MK_FUN (_ms_mk_getas_req),
			(ST_CHAR *)info_ptr));
  }
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_getas_req					*/
/* ENCODE a get alarm summary REQUEST:					*/
/************************************************************************/

ST_VOID _ms_mk_getas_req (ASN1_ENC_CTXT *aCtx, GETAS_REQ_INFO *info)
  {
  if (info->ca_pres)
    {
    asn1r_strt_constr (aCtx);			/* start ctx tag 5 cstr 	*/
    _ms_wr_mms_objname (aCtx, &info->ca_name);	/* write continue after name	*/
    asn1r_fin_constr (aCtx, 5,CTX,DEF);		/* finish ctx tag 5 cstr	*/
    }
 
  if (info->least_sev_filter != 127 ||	/* not the defaults		*/ 
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
  asn1r_fin_constr (aCtx, MMSOP_GET_ALARM_SUM,CTX,DEF);	/* tag = opcode, context	*/
  }
