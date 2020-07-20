/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rs_termupl.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the responder portion of terminate upload	*/
/*	sequence.  It decodes the terminate upload sequence request	*/
/*	(indication) & encodes the terminate upload sequence response.	*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 03/11/04  GLB     06    Remove "thisFileName"                    	*/
/* 08/28/03  EJV     05    Corr spelling for 'receive'.			*/
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
#include "mms_pdom.h"
#include "asn1defs.h"

/************************************************************************/
/* variables global to the DECODE portion :				*/

static  TERMUPL_REQ_INFO	*req_info;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/


/************************************************************************/
/************************************************************************/
/*			mms_term_upload_req				*/
/* This function is called from MMSDEC when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_term_upload_req (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("Terminate Upload Sequence Request");
			/* get storage for request information		*/
  req_info = (TERMUPL_REQ_INFO *) _m_get_dec_buf (aCtx, sizeof (TERMUPL_REQ_INFO));

		/* read upload state machine id from received message	*/
  if (asn1r_get_i32 (aCtx, &(req_info->ulsmid)))
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);

  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_termupl_resp 				*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE and send the terminate upload sequence response.	*/
/************************************************************************/

ST_RET mp_termupl_resp (MMSREQ_IND *indptr)
  {
  return (_mms_send_null_resp (indptr,MMSOP_TERM_UPLOAD));
  }

#else
/************************************************************************/
/************************************************************************/
/*			mpl_termupl_resp 				*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the terminate upload sequence response.		*/
/************************************************************************/

ST_RET mpl_termupl_resp (ST_UINT32 invoke_id)
  {
  return (_mms_fin_null_resp (invoke_id,MMSOP_TERM_UPLOAD));
  }
#endif
