/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rq_repecs.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the requester portion of report event	*/
/*	condition status.  It decodes the report event condition	*/
/*	status response (confirm) and encodes the report event		*/
/*	condition status request.					*/
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

static  REPECS_RESP_INFO	*rsp_info;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

static ST_VOID repecs_tta_time_cstr (ASN1_DEC_CTXT *aCtx);
static ST_VOID repecs_tti_time_cstr (ASN1_DEC_CTXT *aCtx);
static ST_VOID repecs_tta_time_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID repecs_tta_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID repecs_get_enabled (ASN1_DEC_CTXT *aCtx);
static ST_VOID repecs_get_enrolls (ASN1_DEC_CTXT *aCtx);
static ST_VOID repecs_get_cur_state (ASN1_DEC_CTXT *aCtx);
static ST_VOID repecs_tti_done (ASN1_DEC_CTXT *aCtx);


/************************************************************************/
/************************************************************************/
/*			mms_repecs_rsp					*/
/* This function is called from mmsdec when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_repecs_rsp (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("Report Event Condition Status Response");



/* get storage for request information	*/
  rsp_info = (REPECS_RESP_INFO *) _m_get_dec_buf (aCtx, sizeof (REPECS_RESP_INFO));
  rsp_info->enabled_pres  = SD_FALSE;		/* set default value	*/
  rsp_info->tta_time_pres = SD_FALSE;
  rsp_info->tti_time_pres = SD_FALSE;

  ASN1R_TAG_ADD (aCtx, CTX, 0, repecs_get_cur_state);
  }

/************************************************************************/
/*			repecs_get_cur_state				*/
/* Decoding report event condition status response: current state was	*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID repecs_get_cur_state (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("repecs_get_cur_state");
					/* read current state		*/
  if (asn1r_get_i16 (aCtx, &(rsp_info->cur_state)))
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);

  ASN1R_TAG_ADD (aCtx, CTX, 1, repecs_get_enrolls);
  }

/************************************************************************/
/*			repecs_get_enrolls				*/
/* Decoding report event condition status response: number of event	*/
/* enrollments was encountered. 					*/
/************************************************************************/

static ST_VOID repecs_get_enrolls (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("repecs_get_enrolls");
				/* read number of event enrollments	*/
  if (asn1r_get_u32 (aCtx, &(rsp_info->num_of_ev_enroll)))
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);

  ASN1R_TAG_ADD (aCtx, CTX, 2, repecs_get_enabled);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 3, repecs_tta_time_cstr);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 4, repecs_tti_time_cstr);
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }

/************************************************************************/
/*			repecs_get_enabled				*/
/* Decoding report event condition status response: enabled indicator	*/
/* was encountered. 							*/
/************************************************************************/

static ST_VOID repecs_get_enabled (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("repecs_get_enabled");

  rsp_info->enabled_pres  = SD_TRUE;	/* set enabled present		*/
					/* read enabled indicator	*/
  if (asn1r_get_bool (aCtx, &(rsp_info->enabled)))
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);

  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 3, repecs_tta_time_cstr);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 4, repecs_tti_time_cstr);
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }

/************************************************************************/
/*			repecs_tta_time_cstr				*/
/* Decoding report event condition status response: transition to	*/
/* active time cstr was encountered. 					*/
/************************************************************************/

static ST_VOID repecs_tta_time_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("repecs_tta_time_cstr");

  rsp_info->tta_time_pres = SD_TRUE; 	/* set tran to active present	*/

  _ms_get_mms_evtime (aCtx, &rsp_info->tta_time, repecs_tta_done);
  }

/************************************************************************/
/*			repecs_tta_done					*/
/* Decoding report event condition status response: transition to 	*/
/* active time has been obtained. 					*/
/************************************************************************/

static ST_VOID repecs_tta_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("repecs_tta_done");

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = repecs_tta_time_cstr_done;
  }

/************************************************************************/
/*			repecs_tta_time_cstr_done			*/
/* Decoding report event condition status response: transition to 	*/
/* active time cstr done has been encountered.				*/
/************************************************************************/

static ST_VOID repecs_tta_time_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 4, repecs_tti_time_cstr);
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }

/************************************************************************/
/*			repecs_tti_time_cstr				*/
/* Decoding report event condition status response: transition to 	*/
/* idle time cstr has been encountered.					*/
/************************************************************************/

static ST_VOID repecs_tti_time_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("repecs_tti_time_cstr");

  rsp_info->tti_time_pres = SD_TRUE; 	/* set tran to active present	*/

  _ms_get_mms_evtime (aCtx, &rsp_info->tti_time, repecs_tti_done);
  }

/************************************************************************/
/*			repecs_tti_done					*/
/* Decoding report event condition status response: transition to 	*/
/* idle time has been obtained.						*/
/************************************************************************/

static ST_VOID repecs_tti_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("repecs_tti_done");

  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_repecs					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE and send the report event condition status		*/
/* request.								*/
/************************************************************************/

MMSREQ_PEND *mp_repecs (ST_INT chan, REPECS_REQ_INFO *info_ptr)
  {
  return (_mms_req_send (MMSOP_REP_EC_STAT,
			M_CAST_MK_FUN (_ms_mk_repecs_req),
			chan,(ST_CHAR *)info_ptr));
  } 

#else
/************************************************************************/
/************************************************************************/
/*			mpl_repecs					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the report event condition status request.		*/
/************************************************************************/

ST_RET  mpl_repecs (REPECS_REQ_INFO *info_ptr)
  {
  return (_mms_req_fin (MMSOP_REP_EC_STAT,
			M_CAST_MK_FUN (_ms_mk_repecs_req),
			(ST_CHAR *)info_ptr));
  } 
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_repecs_req					*/
/* construct a report event condition status request 			*/
/************************************************************************/

ST_VOID _ms_mk_repecs_req (ASN1_ENC_CTXT *aCtx, REPECS_REQ_INFO *info)
  {
  _ms_wr_mms_objname (aCtx, &info->evcon_name);	/* write the object name	*/

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_constr (aCtx, MMSOP_REP_EC_STAT,CTX,DEF);	/* tag = opcode, ctx constr	*/
  }
