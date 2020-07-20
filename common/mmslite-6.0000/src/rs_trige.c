/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rs_trige.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the responder portion of trigger event.	*/
/*	It decodes the trigger event request (indication) & encodes	*/
/*	the trigger event response.					*/
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

static  TRIGE_REQ_INFO	*req_info;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

static ST_VOID trige_get_priority (ASN1_DEC_CTXT *aCtx);
static ST_VOID trige_name_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID trige_name_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID trige_name_cstr  (ASN1_DEC_CTXT *aCtx);


/************************************************************************/
/************************************************************************/
/*			mms_trige_req					*/
/* This function is called from MMSDEC when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_trige_req (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("Trigger Event Request");
  req_info = (TRIGE_REQ_INFO *) _m_get_dec_buf (aCtx, sizeof (TRIGE_REQ_INFO));

/*  req_info->priority_pres    = SD_FALSE;		set default values	*/

  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 0, trige_name_cstr);
  }

/************************************************************************/
/*			trige_name_cstr			   		*/
/* trigger event request is being decoded: event condition name constr	*/
/* was encountered.					  		*/
/************************************************************************/

static ST_VOID trige_name_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("trige_name_cstr");

  _ms_get_mms_objname (aCtx, &req_info->evcon_name, trige_name_done);
  }

/************************************************************************/
/*			trige_name_done				   	*/
/* trigger event request is being decoded: event condition name has	*/
/* been obtained.  							*/
/************************************************************************/

static ST_VOID trige_name_done (ASN1_DEC_CTXT *aCtx)
  {

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = trige_name_cstr_done;
  }

/************************************************************************/
/*			trige_name_cstr_done				*/
/* trigger event request is being decoded: event condition name constr	*/
/* done was encountered.    				  		*/
/************************************************************************/

static ST_VOID trige_name_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("trige_name_done");

  ASN1R_TAG_ADD (aCtx, CTX, 1, trige_get_priority);
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }

/************************************************************************/
/*			trige_get_priority				*/
/* Decoding trigger event request: priority was encountered.  		*/
/************************************************************************/

static ST_VOID trige_get_priority (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("trige_get_priority");

  req_info->priority_pres = SD_TRUE;

  if (asn1r_get_u8 (aCtx, &(req_info->priority)))		/* read priority	*/
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_trige_resp					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE and send the trigger event	response.		*/
/************************************************************************/

ST_RET mp_trige_resp (MMSREQ_IND *indptr)
  {
  return (_mms_send_null_resp (indptr,MMSOP_TRIGGER_EV));
  }

#else
/************************************************************************/
/************************************************************************/
/*			mpl_trige_resp					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the trigger event	response.			*/
/************************************************************************/

ST_RET mpl_trige_resp (ST_UINT32 invoke_id)
  {
  return (_mms_fin_null_resp (invoke_id,MMSOP_TRIGGER_EV));
  }
#endif
