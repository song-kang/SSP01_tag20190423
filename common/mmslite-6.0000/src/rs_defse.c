/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rs_defsem.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the responder portion of the define	*/
/*	semaphore operation.  It decodes the define semaphore request	*/
/*	(indication) and encodes the define semaphore response. 	*/
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
#include "mms_psem.h"
#include "asn1defs.h"

/************************************************************************/
/* variables global to the DECODE portion :				*/

static  DEFSEM_REQ_INFO	*req_info;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

static ST_VOID defsem_sem_name_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID defsem_objname_done  (ASN1_DEC_CTXT *aCtx);
static ST_VOID defsem_sem_name_cstr (ASN1_DEC_CTXT *aCtx);
static ST_VOID defsem_get_num_of_tokens (ASN1_DEC_CTXT *aCtx);


/************************************************************************/
/************************************************************************/
/*			mms_defsem_req					*/
/* This function is called from mmsdec when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_defsem_req (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("Define Semaphore Request");

/* get storage for request information	*/
  req_info = (DEFSEM_REQ_INFO *) _m_get_dec_buf (aCtx, sizeof (DEFSEM_REQ_INFO));
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 0, defsem_sem_name_cstr);
  }

/************************************************************************/
/*			defsem_sem_name_cstr				*/
/* Decoding define semaphore request: semaphore name constructor was	*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID defsem_sem_name_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("Define Semaphore Request");
  _ms_get_mms_objname (aCtx, &req_info->sem_name, defsem_objname_done);
  }

/************************************************************************/
/*			  defsem_objname_done				*/
/* Decoding Define Semaphore Request: Semaphore name has been obtained. */
/************************************************************************/

static ST_VOID defsem_objname_done (ASN1_DEC_CTXT *aCtx)
  {

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = defsem_sem_name_cstr_done;
  }

/************************************************************************/
/*			defsem_sem_name_cstr_done			*/
/* Decoding define semaphore request: semaphore name contructor done	*/
/* was encountered.							*/
/************************************************************************/

static ST_VOID defsem_sem_name_cstr_done (ASN1_DEC_CTXT *aCtx)
  {

  ASN1R_TAG_ADD (aCtx, CTX, 1, defsem_get_num_of_tokens);
  }

/************************************************************************/
/*			defsem_get_num_of_tokens			*/
/* Decoding define semaphore request: number of tokens name was 	*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID defsem_get_num_of_tokens (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("defsem_get_num_of_tokens");

			/* read number of tokens from received message	*/
  if (asn1r_get_u16 (aCtx, &(req_info->num_of_tokens)))
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_defsem_resp					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE and send the define semaphore response.		*/
/************************************************************************/

ST_RET mp_defsem_resp (MMSREQ_IND *indptr)
  {
  return (_mms_send_null_resp (indptr,MMSOP_DEFINE_SEM));
  }

#else
/************************************************************************/
/************************************************************************/
/*			mpl_defsem_resp					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the define semaphore response.			*/
/************************************************************************/

ST_RET mpl_defsem_resp (ST_UINT32 invoke_id)
  {
  return (_mms_fin_null_resp (invoke_id,MMSOP_DEFINE_SEM));
  }
#endif
