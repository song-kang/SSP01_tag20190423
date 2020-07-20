/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rs_defee.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the responder portion of define event	*/
/*	enrollment.  It decodes the define event enrollment request	*/
/*	(indication) & encodes the define event enrollment response.   	*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 03/11/04  GLB     06    Remove "thisFileName"                    	*/
/* 03/31/03  JRB     05    asn1r_get_bitstr: add max_bits arg.		*/
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

static  DEFEE_REQ_INFO	*req_info;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

static ST_VOID defee_eename_cstr          (ASN1_DEC_CTXT *aCtx);
static ST_VOID defee_eename_done          (ASN1_DEC_CTXT *aCtx);
static ST_VOID defee_eename_cstr_done     (ASN1_DEC_CTXT *aCtx);
static ST_VOID defee_ecname_cstr          (ASN1_DEC_CTXT *aCtx);
static ST_VOID defee_ecname_done          (ASN1_DEC_CTXT *aCtx);
static ST_VOID defee_ecname_cstr_done     (ASN1_DEC_CTXT *aCtx);
static ST_VOID defee_get_ect_prim         (ASN1_DEC_CTXT *aCtx);
static ST_VOID defee_get_ect_cstr         (ASN1_DEC_CTXT *aCtx);
static ST_VOID defee_get_ect_done         (ASN1_DEC_CTXT *aCtx);
static ST_VOID defee_get_alarm_ack_rule   (ASN1_DEC_CTXT *aCtx);
static ST_VOID defee_eaname_cstr          (ASN1_DEC_CTXT *aCtx);
static ST_VOID defee_eaname_done          (ASN1_DEC_CTXT *aCtx);
static ST_VOID defee_eaname_cstr_done     (ASN1_DEC_CTXT *aCtx);
static ST_VOID defee_client_app_cstr      (ASN1_DEC_CTXT *aCtx);
static ST_VOID defee_app_ref_done         (ASN1_DEC_CTXT *aCtx);
static ST_VOID defee_client_app_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID defee_aecname_cstr         (ASN1_DEC_CTXT *aCtx);
static ST_VOID defee_aecname_done         (ASN1_DEC_CTXT *aCtx);
static ST_VOID defee_aecname_cstr_done    (ASN1_DEC_CTXT *aCtx);


/************************************************************************/
/************************************************************************/
/*			mms_defee_req					*/
/* This function is called from MMSDEC when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_defee_req (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("Define Event Enrollment Request");


/* get storage for request information	*/
  req_info = (DEFEE_REQ_INFO *) _m_get_dec_buf (aCtx, sizeof (DEFEE_REQ_INFO));

/*  req_info->evact_name_pres = SD_FALSE;		*/
/*  req_info->client_app_pres = SD_FALSE;		*/

  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 0, defee_eename_cstr);
  }

/************************************************************************/
/*			defee_eename_cstr 				*/
/* define event enrollment request is being decoded: event enrollment	*/
/* name constructor was encountered. 					*/
/************************************************************************/

static ST_VOID defee_eename_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("defee_eename_cstr");

  _ms_get_mms_objname (aCtx, &req_info->evenroll_name, defee_eename_done);
  }

/************************************************************************/
/*			defee_eename_done	     			*/
/* define event enrollment request is being decoded: event enrollment	*/
/* name has been obtained.	 					*/
/************************************************************************/

static ST_VOID defee_eename_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("defee_eename_done");

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = defee_eename_cstr_done;
  }

/************************************************************************/
/*			defee_eename_cstr_done				*/
/* define event enrollment request is being decoded: event enrollment 	*/
/* name constructor done was encountered. 				*/
/************************************************************************/

static ST_VOID defee_eename_cstr_done (ASN1_DEC_CTXT *aCtx)
  {

  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 1, defee_ecname_cstr);
  }

/************************************************************************/
/*			defee_ecname_cstr 				*/
/* define event enrollment request is being decoded: event condition	*/
/* name constructor was encountered. 					*/
/************************************************************************/

static ST_VOID defee_ecname_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("defee_ecname_cstr");

  _ms_get_mms_objname (aCtx, &req_info->evcon_name, defee_ecname_done);
  }

/************************************************************************/
/*			defee_ecname_done	     			*/
/* define event enrollment request is being decoded: event condition	*/
/* name has been obtained.	 					*/
/************************************************************************/

static ST_VOID defee_ecname_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("defee_ecname_done");

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = defee_ecname_cstr_done;
  }

/************************************************************************/
/*			defee_ecname_cstr_done				*/
/* define event enrollment request is being decoded: event condition 	*/
/* name constructor done was encountered. 				*/
/************************************************************************/

static ST_VOID defee_ecname_cstr_done (ASN1_DEC_CTXT *aCtx)
  {

  ASN1R_TAG_ADD (aCtx, CTX,          2, defee_get_ect_prim);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 2, defee_get_ect_cstr);
  }

/************************************************************************/
/*			defee_get_ect_prim				*/
/* define event enrollment request is being decoded: event condition 	*/
/* transitions primitive was encountered.     				*/
/************************************************************************/

static ST_VOID defee_get_ect_prim (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("defee_get_ect_prim");
				/* check length before moving bitstring	*/
  if (!asn1r_get_bitstr (aCtx, &req_info->ec_transitions, 7))
    defee_get_ect_done (aCtx);      	/* set up for the next data element	*/
  else
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);
  }

/************************************************************************/
/*			defee_get_ect_cstr    				*/
/* define event enrollment request is being decoded: event condition 	*/
/* transitions constructor was encountered.     			*/
/************************************************************************/

static ST_VOID defee_get_ect_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("defee_get_ect_cstr");
	  			/* set up bitstring cstr done function	*/
  aCtx->asn1r_c_done_fun[aCtx->asn1r_msg_level] = defee_get_ect_done;

  asn1r_get_bitstr_cstr (aCtx, 7,&req_info->ec_transitions);
  }

/************************************************************************/
/*			defee_get_ect_done			      	*/
/* define event enrollment request is being decoded: event condition 	*/
/* transitions constructor done was encountered. 			*/
/************************************************************************/

static ST_VOID defee_get_ect_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("defee_get_ect_done");
				/* Check aCtx->asn1r_bitcount for correct # of bits	*/
  if (aCtx->asn1r_bitcount != 7)			/* should be 7 bits		*/
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);
  else					/* now get alarm ack rule 	*/
    ASN1R_TAG_ADD (aCtx, CTX,3,defee_get_alarm_ack_rule);
  }

/************************************************************************/
/*			defee_get_alarm_ack_rule   			*/
/* Decoding define event enrollment request: alarm acknowledgment rule	*/
/* was encountered.							*/
/************************************************************************/

static ST_VOID defee_get_alarm_ack_rule (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("defee_get_alarm_ack_rule");
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

  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 4, defee_eaname_cstr);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 5, defee_client_app_cstr);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 6, defee_aecname_cstr);
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }

/************************************************************************/
/*			defee_eaname_cstr 				*/
/* define event enrollment request is being decoded: event action 	*/
/* name constructor was encountered. 					*/
/************************************************************************/

static ST_VOID defee_eaname_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("defee_eaname_cstr");

  req_info->evact_name_pres = SD_TRUE;

  _ms_get_mms_objname (aCtx, &req_info->evact_name, defee_eaname_done);
  }

/************************************************************************/
/*			defee_eaname_done	     			*/
/* define event enrollment request is being decoded: event action 	*/
/* name has been obtained.	 					*/
/************************************************************************/

static ST_VOID defee_eaname_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("defee_eaname_done");

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = defee_eaname_cstr_done;
  }

/************************************************************************/
/*			defee_eaname_cstr_done				*/
/* define event enrollment request is being decoded: event action 	*/
/* name constructor done was encountered. 				*/
/************************************************************************/

static ST_VOID defee_eaname_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 5, defee_client_app_cstr);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 6, defee_aecname_cstr);
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }

/************************************************************************/
/*			defee_client_app_cstr				*/
/* Decoding define event enrollment request: client application cstr 	*/
/* was encountered.							*/
/************************************************************************/

static ST_VOID defee_client_app_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("defee_client_app_cstr");

  req_info->client_app_pres = SD_TRUE;

  _ms_get_mms_app_ref (aCtx, &req_info->client_app,&req_info->client_app_len,
					 defee_app_ref_done);
  }

/************************************************************************/
/*			defee_app_ref_done     				*/
/* Decoding define event enrollment request: client application has 	*/
/* been obtained.							*/
/************************************************************************/

static ST_VOID defee_app_ref_done (ASN1_DEC_CTXT *aCtx)
  {

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = defee_client_app_cstr_done;
  }

/************************************************************************/
/*			defee_client_app_cstr_done 			*/
/* Decoding define event enrollment request: client application cstr 	*/
/* done was encountered.    						*/
/************************************************************************/

static ST_VOID defee_client_app_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 6, defee_aecname_cstr);
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }

/************************************************************************/
/*			defee_aecname_cstr 				*/
/* define event enrollment request is being decoded: acknowledgement	*/
/* event condition name constructor was encountered. 			*/
/************************************************************************/

static ST_VOID defee_aecname_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("defee_aecname_cstr");

  req_info->ackec_name_pres = SD_TRUE;

  _ms_get_mms_objname (aCtx, &req_info->ackec_name, defee_aecname_done);
  }

/************************************************************************/
/*			defee_aecname_done	     			*/
/* define event enrollment request is being decoded: event action 	*/
/* name has been obtained.	 					*/
/************************************************************************/

static ST_VOID defee_aecname_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("defee_aecname_done");

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = defee_aecname_cstr_done;
  }

/************************************************************************/
/*			defee_aecname_cstr_done				*/
/* define event enrollment request is being decoded: event action 	*/
/* name constructor done was encountered. 				*/
/************************************************************************/

static ST_VOID defee_aecname_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_defee_resp					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE and send the define event enrollment response.    	*/
/************************************************************************/

ST_RET mp_defee_resp (MMSREQ_IND *indptr)
  {
  return (_mms_send_null_resp (indptr,MMSOP_DEF_EE));
  }

#else
/************************************************************************/
/************************************************************************/
/*			mpl_defee_resp					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the define event enrollment response.    		*/
/************************************************************************/

ST_RET mpl_defee_resp (ST_UINT32 invoke_id)
  {
  return (_mms_fin_null_resp (invoke_id,MMSOP_DEF_EE));
  }
#endif
