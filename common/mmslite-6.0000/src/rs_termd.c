/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rs_termdown.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the responder portion of the terminate	*/
/*	download sequence.  It decodes the terminate download sequence	*/
/*	request (indication) & encodes the terminate download sequence	*/
/*	response.							*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 03/11/04  GLB     07    Remove "thisFileName"                    	*/
/* 08/28/03  EJV     06    Corr spelling for 'receive'.			*/
/* 12/20/01  JRB     05    Converted to use ASN1R (re-entrant ASN1)	*/
/*			   Del _mms_get_service_err proto(see mmsdefs.h)*/
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
#include "mms_pdom.h"
#include "asn1defs.h"

/************************************************************************/
/* variables global to the DECODE portion :				*/

static  TERMDOWN_REQ_INFO	*req_info;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

static ST_VOID termdown_get_dname (ASN1_DEC_CTXT *aCtx);
static ST_VOID termdown_get_discarded (ASN1_DEC_CTXT *aCtx);
static ST_VOID termdown_get_discarded_err (ASN1_DEC_CTXT *aCtx);


/************************************************************************/
/************************************************************************/
/*			mms_term_download_req				*/
/* This function is called from MMSDEC when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_term_download_req (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("Terminate Download Sequence Request");
/* get storage for the request information				*/
/* make big enough to add service error info if req'd			*/

  req_info = (TERMDOWN_REQ_INFO *) _m_get_dec_buf (aCtx, sizeof (TERMDOWN_REQ_INFO) + sizeof (ERR_INFO));
  req_info->discarded = SD_FALSE;		/* set the default value	*/
			/* read domain name from the received message	*/
  ASN1R_TAG_ADD (aCtx, CTX,0,termdown_get_dname);
  }

/************************************************************************/
/*			termdown_get_dname				*/
/* Terminate download sequence request: decode domain name		*/
/************************************************************************/

static ST_VOID termdown_get_dname (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("termdown_get_dname");

  if (asn1r_get_identifier (aCtx, req_info->dname))
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);

#ifndef MMS_LITE
  if (mms_chan_info[_mmsdechan].version)		/* IS 	*/
#else
  if (mmsl_version)				/* IS 	*/
#endif
    ASN1R_TAG_ADD (aCtx, CTX|CONSTR, 1, termdown_get_discarded_err);
  else						/* DIS	*/
    ASN1R_TAG_ADD (aCtx, CTX, 1, termdown_get_discarded);

  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }

/************************************************************************/
/*			termdown_get_discarded				*/
/* Terminate download sequence request: decode discarded flag		*/
/************************************************************************/

static ST_VOID termdown_get_discarded (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("termdown_get_discarded");

		/* read discarded flag from the received message	*/
  if (asn1r_get_bool (aCtx, &(req_info->discarded)))
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }

/************************************************************************/
/*			termdown_get_discarded_err			*/
/* Terminate download sequence request: decode discarded 'error'	*/
/************************************************************************/

static ST_VOID termdown_get_discarded_err (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("termdown_get_discarded_err");

/* set up pointer (already allocated)					*/
  req_info->err = (ERR_INFO *) (req_info+1);
  req_info->discarded = SD_TRUE;

/* set up to decode error contents, etc..				*/
  _mms_get_service_err (aCtx, req_info->err);
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_termdown_resp				*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE and send the terminate download sequence response. */
/************************************************************************/

ST_RET mp_termdown_resp (MMSREQ_IND *indptr)
  {
  return (_mms_send_null_resp (indptr,MMSOP_TERM_DOWNLOAD));
  }

#else
/************************************************************************/
/************************************************************************/
/*			mpl_termdown_resp				*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the terminate download sequence response. 		*/
/************************************************************************/

ST_RET mpl_termdown_resp (ST_UINT32 invoke_id)
  {
  return (_mms_fin_null_resp (invoke_id,MMSOP_TERM_DOWNLOAD));
  }
#endif
