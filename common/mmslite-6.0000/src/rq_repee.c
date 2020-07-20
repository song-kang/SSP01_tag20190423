/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rq_repees.c						*/
/* PRODUCT(S)  : MMSEASE						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the requester portion of the report event	*/
/*	enrollment status.  It decodes the report event enrollment	*/
/*	status response (confirm) & encodes the report event enrollment	*/
/*	status request.    						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 03/11/04  GLB     07    Remove "thisFileName"                    	*/
/* 03/31/03  JRB     06    asn1r_get_bitstr: add max_bits arg.		*/
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

static  REPEES_RESP_INFO	*rsp_info;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

static ST_VOID repees_get_ect_prim       (ASN1_DEC_CTXT *aCtx);
static ST_VOID repees_get_ect_cstr       (ASN1_DEC_CTXT *aCtx);
static ST_VOID repees_get_ect_done       (ASN1_DEC_CTXT *aCtx);
static ST_VOID repees_get_not_lost       (ASN1_DEC_CTXT *aCtx);
static ST_VOID repees_get_duration       (ASN1_DEC_CTXT *aCtx);
static ST_VOID repees_get_alarm_ack_rule (ASN1_DEC_CTXT *aCtx);
static ST_VOID repees_get_cur_state      (ASN1_DEC_CTXT *aCtx);


/************************************************************************/
/************************************************************************/
/*			mms_repees_rsp 					*/
/* This function is called from MMSDEC when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_repees_rsp (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("Report Event Enrollment Status Response");

/* get storage for request information	*/
  rsp_info = (REPEES_RESP_INFO *) _m_get_dec_buf (aCtx, sizeof (REPEES_RESP_INFO));
  ASN1R_TAG_ADD (aCtx, CTX,          0, repees_get_ect_prim);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 0, repees_get_ect_cstr);
  }

/************************************************************************/
/*			repees_get_ect_prim				*/
/* report event enrollment status response is being decoded: event	*/
/* condition transitions primitive was encountered.   			*/
/************************************************************************/

static ST_VOID repees_get_ect_prim (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("repees_get_ect_prim");
  if (!asn1r_get_bitstr (aCtx, &rsp_info->ec_transitions, 7))
    repees_get_ect_done (aCtx);      	/* set up for the next data element	*/
  else
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);
  }

/************************************************************************/
/*			repees_get_ect_cstr    				*/
/* report event enrollment status response is being decoded: event 	*/
/* condition transitions constructor was encountered.     		*/
/************************************************************************/

static ST_VOID repees_get_ect_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("repees_get_ect_cstr");
	  			/* set up bitstring cstr done function	*/
  aCtx->asn1r_c_done_fun[aCtx->asn1r_msg_level] = repees_get_ect_done;
 
  asn1r_get_bitstr_cstr (aCtx, 7,&rsp_info->ec_transitions);
  }

/************************************************************************/
/*			repees_get_ect_done			      	*/
/* report event enrollment status response is being decoded: event 	*/
/* condition transitions constructor done was encountered. 		*/
/************************************************************************/

static ST_VOID repees_get_ect_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("repees_get_ect_done");
  				/* Check aCtx->asn1r_bitcount for correct # of bits	*/
  if (aCtx->asn1r_bitcount != 7)			/* should be 7 bits		*/
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);
  else					/* now get alarm ack rule 	*/
    {
    ASN1R_TAG_ADD (aCtx, CTX, 1, repees_get_not_lost);
    ASN1R_TAG_ADD (aCtx, CTX, 2, repees_get_duration);
    }
  }

/************************************************************************/
/*			repees_get_not_lost				*/
/* report event enrollment status response is being decoded: 	 	*/
/* notification lost has been encountered.  				*/
/************************************************************************/

static ST_VOID repees_get_not_lost (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("repees_get_not_lost");

  if (asn1r_get_bool (aCtx, &(rsp_info->not_lost)))    	/* get notification lost*/
    {
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);
    return;
    }

  ASN1R_TAG_ADD (aCtx, CTX, 2, repees_get_duration);
  }

/************************************************************************/
/*			repees_get_duration	   			*/
/* Decoding report event enrollment status response: duration was 	*/
/* encountered.								*/
/************************************************************************/

static ST_VOID repees_get_duration (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("repees_get_duration");
					/* read duration  		*/
  if (asn1r_get_i16 (aCtx, &(rsp_info->duration)))
    {
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);
    return;
    }
  else if ((rsp_info->duration < 0) || (rsp_info->duration > 1))
    {
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);
    return;
    }

  ASN1R_TAG_ADD (aCtx, CTX, 3, repees_get_alarm_ack_rule);
  ASN1R_TAG_ADD (aCtx, CTX, 4, repees_get_cur_state);
  }

/************************************************************************/
/*			repees_get_alarm_ack_rule   			*/
/* Decoding report event enrollment status response: alarm ack rule	*/
/* was encountered.							*/
/************************************************************************/

static ST_VOID repees_get_alarm_ack_rule (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("repees_get_alarm_ack_rule");

  rsp_info->alarm_ack_rule_pres = SD_TRUE;
					/* read alarm ack rule 		*/
  if (asn1r_get_i16 (aCtx, &(rsp_info->alarm_ack_rule)))
    {
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);
    return;
    }
  else if ((rsp_info->alarm_ack_rule < 0) || (rsp_info->alarm_ack_rule > 3))
    {
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);
    return;
    }

  ASN1R_TAG_ADD (aCtx, CTX, 4, repees_get_cur_state);
  }

/************************************************************************/
/*			repees_get_cur_state	   			*/
/* Decoding report event enrollment status response: current state was 	*/
/* encountered.								*/
/************************************************************************/

static ST_VOID repees_get_cur_state (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("repees_get_cur_state");
					/* read current state 		*/
  if (asn1r_get_i16 (aCtx, &(rsp_info->cur_state)))
    {
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);
    return;
    }
  else if ((rsp_info->cur_state < 0) || (rsp_info->cur_state > 7))
    {
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);
    return;
    }
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_repees					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE & send the report event enrollment status request	*/
/************************************************************************/

MMSREQ_PEND *mp_repees (ST_INT chan, REPEES_REQ_INFO *info_ptr)
  {
  return (_mms_req_send (MMSOP_REP_EE_STAT,
			M_CAST_MK_FUN (_ms_mk_repees_req),
			chan,(ST_CHAR *)info_ptr));
  } 

#else
/************************************************************************/
/************************************************************************/
/*			mpl_repees					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the report event enrollment status request.	*/
/************************************************************************/

ST_RET  mpl_repees (REPEES_REQ_INFO *info_ptr)
  {
  return (_mms_req_fin (MMSOP_REP_EE_STAT,
			M_CAST_MK_FUN (_ms_mk_repees_req),
			(ST_CHAR *)info_ptr));
  } 
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_repees_req					*/
/* construct a report event enrollment status response			*/
/************************************************************************/

ST_VOID _ms_mk_repees_req (ASN1_ENC_CTXT *aCtx, REPEES_REQ_INFO *info)
  {
  _ms_wr_mms_objname (aCtx, &info->evenroll_name);/* write event enrollment name	*/

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_constr (aCtx, MMSOP_REP_EE_STAT,CTX,DEF); 	/* tag = opcode, ctx		*/
  }
