/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rs_altee.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the responder portion of alter event	*/
/*	enrollment.  It decodes the alter event enrollment request	*/
/*	(indication) & encodes the alter event enrollment response.   	*/
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

static  ALTEE_REQ_INFO	*req_info;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

static ST_VOID altee_eename_cstr        (ASN1_DEC_CTXT *aCtx);
static ST_VOID altee_eename_done        (ASN1_DEC_CTXT *aCtx);
static ST_VOID altee_eename_cstr_done   (ASN1_DEC_CTXT *aCtx);
static ST_VOID altee_get_ect_prim       (ASN1_DEC_CTXT *aCtx);
static ST_VOID altee_get_ect_cstr       (ASN1_DEC_CTXT *aCtx);
static ST_VOID altee_get_ect_done       (ASN1_DEC_CTXT *aCtx);
static ST_VOID altee_get_alarm_ack_rule (ASN1_DEC_CTXT *aCtx);


/************************************************************************/
/************************************************************************/
/*			mms_altee_req					*/
/* This function is called from MMSDEC when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_altee_req (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("Alter Event Enrollment Request");


/* get storage for request information	*/
  req_info = (ALTEE_REQ_INFO *) _m_get_dec_buf (aCtx, sizeof (ALTEE_REQ_INFO));

/*  req_info->ec_transitions_pres = SD_FALSE;		*/
/*  req_info->alarm_ack_rule_pres = SD_FALSE;		*/

  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 0, altee_eename_cstr);
  }

/************************************************************************/
/*			altee_eename_cstr 				*/
/* alter event enrollment request is being decoded: event enrollment	*/
/* name constructor was encountered. 					*/
/************************************************************************/

static ST_VOID altee_eename_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("altee_eename_cstr");

  _ms_get_mms_objname (aCtx, &req_info->evenroll_name, altee_eename_done);
  }

/************************************************************************/
/*			altee_eename_done	     			*/
/* alter event enrollment request is being decoded: event enrollment	*/
/* name has been obtained.	 					*/
/************************************************************************/

static ST_VOID altee_eename_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("altee_eename_done");

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = altee_eename_cstr_done;
  }

/************************************************************************/
/*			altee_eename_cstr_done				*/
/* alter event enrollment request is being decoded: event enrollment 	*/
/* name constructor done was encountered. 				*/
/************************************************************************/

static ST_VOID altee_eename_cstr_done (ASN1_DEC_CTXT *aCtx)
  {

  ASN1R_TAG_ADD (aCtx, CTX,          1, altee_get_ect_prim);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 1, altee_get_ect_cstr);
  ASN1R_TAG_ADD (aCtx, CTX,          2, altee_get_alarm_ack_rule);
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }

/************************************************************************/
/*			altee_get_ect_prim				*/
/* alter event enrollment request is being decoded: event condition 	*/
/* transitions primitive was encountered.     				*/
/************************************************************************/

static ST_VOID altee_get_ect_prim (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("altee_get_ect_prim");
				/* check length before moving bitstring	*/
  if (!asn1r_get_bitstr (aCtx, &req_info->ec_transitions, 7))
    altee_get_ect_done (aCtx);      	/* set up for the next data element	*/
  else
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);
  }

/************************************************************************/
/*			altee_get_ect_cstr    				*/
/* alter event enrollment request is being decoded: event condition 	*/
/* transitions constructor was encountered.     			*/
/************************************************************************/

static ST_VOID altee_get_ect_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("altee_get_ect_cstr");
	  			/* set up bitstring cstr done function	*/
  aCtx->asn1r_c_done_fun[aCtx->asn1r_msg_level] = altee_get_ect_done;
 
  asn1r_get_bitstr_cstr (aCtx, 7,&req_info->ec_transitions);
  }

/************************************************************************/
/*			altee_get_ect_done			      	*/
/* alter event enrollment request is being decoded: event condition 	*/
/* transitions constructor done was encountered. 			*/
/************************************************************************/

static ST_VOID altee_get_ect_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("altee_get_ect_done");
 
  req_info->ec_transitions_pres = SD_TRUE;		/* set present flag	*/
  
  				/* Check aCtx->asn1r_bitcount for correct # of bits	*/
  if (aCtx->asn1r_bitcount != 7)			/* should be 7 bits		*/
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);
  else					/* now get alarm ack rule 	*/
    {
    ASN1R_TAG_ADD (aCtx, CTX,2,altee_get_alarm_ack_rule);
    _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
    }
  }

/************************************************************************/
/*			altee_get_alarm_ack_rule   			*/
/* Decoding alter event enrollment request: alarm acknowledgment rule	*/
/* was encountered.							*/
/************************************************************************/

static ST_VOID altee_get_alarm_ack_rule (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("altee_get_alarm_ack_rule");

  req_info->alarm_ack_rule_pres = SD_TRUE;
					/* read alarm ack rule 		*/
  if (asn1r_get_i16 (aCtx, &(req_info->alarm_ack_rule)))
    {
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);
    return;
    }
  else if ((req_info->alarm_ack_rule < 0) || (req_info->alarm_ack_rule > 3))
    {
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);
    return;
    }

  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_altee_resp					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE and send the alter event enrollment response.    	*/
/************************************************************************/

ST_RET mp_altee_resp (MMSREQ_IND *indptr, ALTEE_RESP_INFO *info_ptr)
  {
  return (_mms_resp_send (MMSOP_ALT_EE,indptr,
			M_CAST_MK_FUN (_ms_mk_altee_resp),
			(ST_CHAR *)info_ptr));
  }

#else
/************************************************************************/
/************************************************************************/
/*			mpl_altee_resp					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the alter event enrollment response.    		*/
/************************************************************************/

ST_RET mpl_altee_resp (ST_UINT32 invoke_id, ALTEE_RESP_INFO *info_ptr)
  {
  return (_mms_resp_fin (MMSOP_ALT_EE,invoke_id,
			M_CAST_MK_FUN (_ms_mk_altee_resp),
			(ST_CHAR *)info_ptr));
  }
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_altee_resp					*/
/* ENCODE an alter event enrollment RESPONSE: 				*/
/************************************************************************/

ST_VOID _ms_mk_altee_resp (ASN1_ENC_CTXT *aCtx, ALTEE_RESP_INFO *info)
  {
  asn1r_strt_constr (aCtx);			/* start ctx tag 1 constructor	*/
  _ms_wr_mms_evtime (aCtx, &info->trans_time);
  asn1r_fin_constr (aCtx, 1,CTX,DEF);		/* finish ctx tag 1 constructor */
  
  asn1r_strt_constr (aCtx);			/* start ctx tag 0 constructor	*/
  if (info->cur_state_tag == 1)
    asn1r_fin_prim (aCtx, 1, CTX);			/* write undefined		*/
  else
    {
    asn1r_wr_i16 (aCtx, info->state); 		/* write state			*/
    asn1r_fin_prim (aCtx, 0, CTX);
    }
  asn1r_fin_constr (aCtx, 0,CTX,DEF);		/* finish ctx tag 0 constructor */

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_constr (aCtx, MMSOP_ALT_EE,CTX,DEF);		/* tag = opcode, ctx constr	*/
  }
