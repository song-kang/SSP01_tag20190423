/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rs_ackevnot.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the responder portion of the acknowledge	*/
/*	event notification operation.  It decodes the acknowledge event */
/*	notification request (indication) and encodes the acknowledge	*/
/*	event notification response.					*/
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

static  ACKEVNOT_REQ_INFO	*req_info;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

static ST_VOID ackevnot_enroll_name_cstr      (ASN1_DEC_CTXT *aCtx);
static ST_VOID ackevnot_enroll_name_done      (ASN1_DEC_CTXT *aCtx);
static ST_VOID ackevnot_enroll_name_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID ackevnot_get_state             (ASN1_DEC_CTXT *aCtx);
static ST_VOID ackevnot_evtime_cstr           (ASN1_DEC_CTXT *aCtx);
static ST_VOID ackevnot_evtime_done           (ASN1_DEC_CTXT *aCtx);
static ST_VOID ackevnot_evtime_cstr_done      (ASN1_DEC_CTXT *aCtx);
static ST_VOID ackevnot_ack_name_cstr         (ASN1_DEC_CTXT *aCtx);
static ST_VOID ackevnot_ack_name_done         (ASN1_DEC_CTXT *aCtx);


/************************************************************************/
/************************************************************************/
/*			mms_ackevnot_req				*/
/* This function is called from MMSDEC when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_ackevnot_req (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("Acknowledge Event Notification Request");


/* get storage for request information	*/
  req_info = (ACKEVNOT_REQ_INFO *) _m_get_dec_buf (aCtx, sizeof (ACKEVNOT_REQ_INFO));

  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 0, ackevnot_enroll_name_cstr);
  }

/************************************************************************/
/*			ackevnot_enroll_name_cstr 			*/
/* decoding acknowledge event notification request: event enrollment	*/
/* name constructor was encountered.					*/
/************************************************************************/

static ST_VOID ackevnot_enroll_name_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("ackevnot_enroll_name_cstr");

  _ms_get_mms_objname (aCtx, &req_info->evenroll_name, ackevnot_enroll_name_done);
  }

/************************************************************************/
/*			ackevnot_enroll_name_done		    	*/
/* decoding acknowledge event notification request: event enrollment	*/
/* name has been obtained.						*/
/************************************************************************/

static ST_VOID ackevnot_enroll_name_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("ackevnot_enroll_name_done");

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = ackevnot_enroll_name_cstr_done;
  }

/************************************************************************/
/*			ackevnot_enroll_name_cstr_done			*/
/* decoding acknowledge event notification request: event enrollment	*/
/* name constructor done has been encountered.				*/
/************************************************************************/

static ST_VOID ackevnot_enroll_name_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
  ASN1R_TAG_ADD (aCtx, CTX,2, ackevnot_get_state);
  }

/************************************************************************/
/*			ackevnot_get_state				*/
/* acknowledge event notification REQUEST is being decoded: acknowledge */
/* state was encountered.						*/
/************************************************************************/

static ST_VOID ackevnot_get_state (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("ackevnot_get_state");

  if (asn1r_get_i16 (aCtx, &(req_info->ack_state)))	/* acknowledge state	*/
    {
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);
    return;
    }
  else if ((req_info->ack_state < 0) || (req_info->ack_state > 2))
    {
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);
    return;
    }

  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 3, ackevnot_evtime_cstr);
  }

/************************************************************************/
/*			ackevnot_evtime_cstr				*/
/* decoding event notification request: event time constructor	was	*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID ackevnot_evtime_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("ackevnot_evtime_cstr");

  _ms_get_mms_evtime (aCtx, &req_info->evtime, ackevnot_evtime_done);
  }

/************************************************************************/
/*			ackevnot_evtime_done				*/
/* decoding acknowledge event notification request: event time has been */
/* obtained.								*/
/************************************************************************/

static ST_VOID ackevnot_evtime_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("ackevnot_evtime_done");

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = ackevnot_evtime_cstr_done;
  }

/************************************************************************/
/*			ackevnot_evtime_cstr_done			*/
/* decoding acknowledge event notification request: event time		*/
/* constructor done has been encountered.				*/
/************************************************************************/

static ST_VOID ackevnot_evtime_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
  ASN1R_TAG_ADD (aCtx, CTX|CONSTR, 4, ackevnot_ack_name_cstr);

  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }

/************************************************************************/
/*			ackevnot_ack_name_cstr				*/
/* acknowledge event notification REQUEST is being decoded: 		*/
/* acknowledgment event condition constructor was encountered.		*/
/************************************************************************/

static ST_VOID ackevnot_ack_name_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("ackevnot_ack_name_cstr");

  req_info->ackec_name_pres = SD_TRUE;	/* ack event cond name present	*/

  _ms_get_mms_objname (aCtx, &req_info->ackec_name, ackevnot_ack_name_done);
  }

/************************************************************************/
/*			ackevnot_ack_name_done  			*/
/* decoding acknowledge event notification request: acknowledgement 	*/
/* event condition name has been obtained.				*/
/************************************************************************/

static ST_VOID ackevnot_ack_name_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("ackevnot_ack_name_done");

  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_ackevnot_resp				*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE & send the acknowledge event notification response.*/
/************************************************************************/

ST_RET mp_ackevnot_resp (MMSREQ_IND *indptr)
  {
  return (_mms_send_null_resp (indptr,MMSOP_ACK_EVENT_NOT));
  }

#else
/************************************************************************/
/************************************************************************/
/*			mpl_ackevnot_resp				*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the acknowledge event notification response.	*/
/************************************************************************/

ST_RET mpl_ackevnot_resp (ST_UINT32 invoke_id)
  {
  return (_mms_fin_null_resp (invoke_id,MMSOP_ACK_EVENT_NOT));
  }
#endif
