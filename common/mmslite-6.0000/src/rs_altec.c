/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rs_altecm.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the responder portion of alter event	*/
/*	condition monitoring.  It decodes the alter event condition	*/
/*	monitoring request (indication) & encodes the alter event	*/
/*	condition monitoring response.  			 	*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   ------------------------------------------	*/
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

static  ALTECM_REQ_INFO	*req_info;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

static ST_VOID altecm_name_cstr      (ASN1_DEC_CTXT *aCtx);
static ST_VOID altecm_name_done      (ASN1_DEC_CTXT *aCtx);
static ST_VOID altecm_name_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID altecm_get_enabled    (ASN1_DEC_CTXT *aCtx);
static ST_VOID altecm_get_priority   (ASN1_DEC_CTXT *aCtx);
static ST_VOID altecm_get_as_reports (ASN1_DEC_CTXT *aCtx);
static ST_VOID altecm_get_eval_int   (ASN1_DEC_CTXT *aCtx);


/************************************************************************/
/************************************************************************/
/*			mms_altecm_req					*/
/* This function is called from MMSDEC when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_altecm_req (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("Alter Event Condition Monitoring Request");


/* get storage for request information	*/
  req_info = (ALTECM_REQ_INFO *) _m_get_dec_buf (aCtx, sizeof (ALTECM_REQ_INFO));

/*  req_info->enabled_pres    = SD_FALSE;		set default values	*/
/*  req_info->priority_pres   = SD_FALSE;					*/
/*  req_info->as_reports_pres = SD_FALSE;					*/

  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 0, altecm_name_cstr);
  }

/************************************************************************/
/*			altecm_name_cstr		   		*/
/* alter event condition monitoring request is being decoded: event	*/
/* condition name constructor was encountered.		  		*/
/************************************************************************/

static ST_VOID altecm_name_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("altecm_name_cstr");

  _ms_get_mms_objname (aCtx, &req_info->evcon_name, altecm_name_done);
  }

/************************************************************************/
/*			altecm_name_done			   	*/
/* alter event condition monitoring request is being decoded: event	*/
/* condition name has been obtained.  					*/
/************************************************************************/

static ST_VOID altecm_name_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("altecm_name_done");

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = altecm_name_cstr_done;
  }

/************************************************************************/
/*			altecm_name_cstr_done				*/
/* alter event condition monitoring request is being decoded: event	*/
/* condition name constructor done was encountered. 	  		*/
/************************************************************************/

static ST_VOID altecm_name_cstr_done (ASN1_DEC_CTXT *aCtx)
  {

  ASN1R_TAG_ADD (aCtx, CTX, 1, altecm_get_enabled);
  ASN1R_TAG_ADD (aCtx, CTX, 2, altecm_get_priority);
  ASN1R_TAG_ADD (aCtx, CTX, 3, altecm_get_as_reports);
  ASN1R_TAG_ADD (aCtx, CTX, 4, altecm_get_eval_int);
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }

/************************************************************************/
/*			altecm_get_enabled				*/
/* Decoding alter event condition monitoring request: enabled was	*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID altecm_get_enabled (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("altecm_get_enabled");

  req_info->enabled_pres = SD_TRUE;

  if (asn1r_get_bool (aCtx, &(req_info->enabled)))		/* read enabled		*/
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);

  ASN1R_TAG_ADD (aCtx, CTX, 2, altecm_get_priority);
  ASN1R_TAG_ADD (aCtx, CTX, 3, altecm_get_as_reports);
  ASN1R_TAG_ADD (aCtx, CTX, 4, altecm_get_eval_int);
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }

/************************************************************************/
/*			altecm_get_priority				*/
/* Decoding alter event condition monitoring request: priority was	*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID altecm_get_priority (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("altecm_get_priority");

  req_info->priority_pres = SD_TRUE;

  if (asn1r_get_u8 (aCtx, &(req_info->priority)))		/* read priority	*/
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);

  ASN1R_TAG_ADD (aCtx, CTX, 3, altecm_get_as_reports);
  ASN1R_TAG_ADD (aCtx, CTX, 4, altecm_get_eval_int);
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }

/************************************************************************/
/*			altecm_get_as_reports				*/
/* Decoding alter event condition monitoring request: alarm summary	*/
/* reports was encountered. 						*/
/************************************************************************/

static ST_VOID altecm_get_as_reports (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("altecm_get_as_reports");
  
  req_info->as_reports_pres = SD_TRUE;	/* set present flag		*/
					/* read alarm summary reports	*/
  if (asn1r_get_bool (aCtx, &(req_info->as_reports)))
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);

  ASN1R_TAG_ADD (aCtx, CTX, 4, altecm_get_eval_int);
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }

/************************************************************************/
/*			altecm_get_eval_int				*/
/* Decoding alter event condition monitoring request: evaluation	*/
/* interval was encountered. 						*/
/************************************************************************/

static ST_VOID altecm_get_eval_int (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("altecm_get_eval_int");

  req_info->eval_int_pres = SD_TRUE;

  if (asn1r_get_u32 (aCtx, &(req_info->eval_int)))		/* read evaluation int	*/
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);

  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_altecm_resp					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE and send the alter event condition monitoring	*/
/* response.							    	*/
/************************************************************************/

ST_RET mp_altecm_resp (MMSREQ_IND *indptr)
  {
  return (_mms_send_null_resp (indptr,MMSOP_ALT_EC_MON));
  }

#else
/************************************************************************/
/************************************************************************/
/*			mpl_altecm_resp					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the alter event condition monitoring response.	*/
/************************************************************************/

ST_RET mpl_altecm_resp (ST_UINT32 invoke_id)
  {
  return (_mms_fin_null_resp (invoke_id,MMSOP_ALT_EC_MON));
  }
#endif
