/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rq_altee.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the requester portion of alter event	*/
/*	enrollment.  It decodes the alter event enrollment response	*/
/*	(confirm) & encodes the alter event enrollment request. 	*/
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

static  ALTEE_RESP_INFO	*rsp_info;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

static ST_VOID altee_cur_state_cstr      (ASN1_DEC_CTXT *ac);
static ST_VOID altee_get_state           (ASN1_DEC_CTXT *ac);
static ST_VOID altee_get_undef           (ASN1_DEC_CTXT *ac);
static ST_VOID altee_cur_state_cstr_done (ASN1_DEC_CTXT *ac);
static ST_VOID altee_ttime_cstr          (ASN1_DEC_CTXT *ac);
static ST_VOID altee_ttime_done          (ASN1_DEC_CTXT *ac);
static ST_VOID altee_ttime_cstr_done     (ASN1_DEC_CTXT *ac);


/************************************************************************/
/************************************************************************/
/*			mms_altee_rsp 					*/
/* This function is called from MMSDEC when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_altee_rsp (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("Alter Event Enrollment Response");

/* get storage for request information	*/
  rsp_info = (ALTEE_RESP_INFO *) _m_get_dec_buf (aCtx, sizeof (ALTEE_RESP_INFO));
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 0, altee_cur_state_cstr);
  }

/************************************************************************/
/*			altee_cur_state_cstr				*/
/* Decoding alter event enrollment response: current state constructor	*/
/* was encountered. 							*/
/************************************************************************/

static ST_VOID altee_cur_state_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("altee_cur_state_cstr");

  ASN1R_TAG_ADD (aCtx, CTX, 0, altee_get_state);
  ASN1R_TAG_ADD (aCtx, CTX, 1, altee_get_undef);
  }

/************************************************************************/
/*			altee_get_state					*/
/* Decoding alter event enrollment request: state was encountered.	*/
/************************************************************************/

static ST_VOID altee_get_state (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("altee_get_state");

  rsp_info->cur_state_tag = 0;			/* set tag to state	*/

  if (asn1r_get_i16 (aCtx, &(rsp_info->state)))
    {						/* read the state 	*/
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);
    return;
    }
  else if ((rsp_info->state < 0) || (rsp_info->state > 7))
    {
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);
    return;
    }

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = altee_cur_state_cstr_done;
  }

/************************************************************************/
/*			altee_get_undef					*/
/* Decoding alter event enrollment request: undefined was encountered.	*/
/************************************************************************/

static ST_VOID altee_get_undef (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("altee_get_undef");

  rsp_info->cur_state_tag = 1;			/* set tag to undefined	*/

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = altee_cur_state_cstr_done;
  }

/************************************************************************/
/*			altee_cur_state_cstr_done			*/
/* Decoding alter event enrollment response: current state constructor	*/
/* done was encountered.  						*/
/************************************************************************/

static ST_VOID altee_cur_state_cstr_done (ASN1_DEC_CTXT *aCtx)
  {

  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 1, altee_ttime_cstr);
  }

/************************************************************************/
/*			altee_ttime_cstr				*/
/* Decoding alter event enrollment response: transition time constr	*/
/* was encountered.  							*/
/************************************************************************/

static ST_VOID altee_ttime_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("altee_ttime_cstr");

  _ms_get_mms_evtime (aCtx, &rsp_info->trans_time, altee_ttime_done);
  }

/************************************************************************/
/*			altee_ttime_done  				*/
/* Decoding alter event enrollment response: transition time has been	*/
/* obtained. 								*/
/************************************************************************/

static ST_VOID altee_ttime_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("altee_ttime_done");

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = altee_ttime_cstr_done;
  }

/************************************************************************/
/*			altee_ttime_cstr_done				*/
/* Decoding alter event enrollment response: transition time constr	*/
/* done was encountered.  						*/
/************************************************************************/

static ST_VOID altee_ttime_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_altee					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE and send the alter event enrollment request.   	*/
/************************************************************************/

MMSREQ_PEND *mp_altee (ST_INT chan, ALTEE_REQ_INFO *info_ptr)
  {
  return (_mms_req_send (MMSOP_ALT_EE,
			M_CAST_MK_FUN (_ms_mk_altee_req),
			chan,(ST_CHAR *)info_ptr));
  } 

#else
/************************************************************************/
/************************************************************************/
/*			mpl_altee					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the alter event enrollment request.   		*/
/************************************************************************/

ST_RET  mpl_altee (ALTEE_REQ_INFO *info_ptr)
  {
  return (_mms_req_fin (MMSOP_ALT_EE,
			M_CAST_MK_FUN (_ms_mk_altee_req),
			(ST_CHAR *)info_ptr));
  } 
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_altee_req					*/
/* ENCODE a alter event enrollment REQUEST:				*/
/************************************************************************/

ST_VOID _ms_mk_altee_req (ASN1_ENC_CTXT *aCtx, ALTEE_REQ_INFO *info)
  {
  if (info->alarm_ack_rule_pres)
    {  
    asn1r_wr_i16 (aCtx, info->alarm_ack_rule);
    asn1r_fin_prim (aCtx, 2, CTX);
    }

  if (info->ec_transitions_pres)
    {  
    asn1r_wr_bitstr (aCtx, &info->ec_transitions, 7);
    asn1r_fin_prim (aCtx, 1, CTX);
    }

  asn1r_strt_constr (aCtx);			/* start ctx tag 0 constructor	*/
  _ms_wr_mms_objname (aCtx, &info->evenroll_name);/* write event enrollment name	*/
  asn1r_fin_constr (aCtx, 0,CTX,DEF);		/* context specific tag 0	*/

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_constr (aCtx, MMSOP_ALT_EE,CTX,DEF);		/* tag = opcode, ctx constr	*/
  }
