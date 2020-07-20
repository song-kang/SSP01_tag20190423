/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rs_relctrl.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the responder portion of relinquish	*/
/*	semaphore control.  It decodes the relinquish control request	*/
/*	(indication) and encodes the relinquish control response.	*/
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

static  RELCTRL_REQ_INFO	*req_info;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

static ST_VOID relctrl_sem_name_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID relctrl_objname_done       (ASN1_DEC_CTXT *aCtx);
static ST_VOID relctrl_sem_name_cstr      (ASN1_DEC_CTXT *aCtx);
static ST_VOID relctrl_get_named_token    (ASN1_DEC_CTXT *aCtx);


/************************************************************************/
/************************************************************************/
/*			mms_relctrl_req 				*/
/* This function is called from mmsdec when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_relctrl_req (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("Relinquish Control Request");
  req_info = (RELCTRL_REQ_INFO *) _m_get_dec_buf (aCtx, sizeof (RELCTRL_REQ_INFO));
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 0, relctrl_sem_name_cstr);
  }

/************************************************************************/
/*			      relctrl_sem_name_cstr			*/
/* Decoding relinquish control request: semaphore name constructor was	*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID relctrl_sem_name_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("relctrl_sem_name_cstr");
  req_info->named_token_pres = SD_FALSE;	/* set default value	*/
  _ms_get_mms_objname (aCtx, &req_info->sem_name, relctrl_objname_done);
  }

/************************************************************************/
/*			relctrl_objname_done				*/
/* Decoding take control request: Semaphore name has been obtained	*/
/************************************************************************/

static ST_VOID relctrl_objname_done (ASN1_DEC_CTXT *aCtx)
  {

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = relctrl_sem_name_cstr_done;
  }

/************************************************************************/
/*			relctrl_sem_name_cstr_done			*/
/* Decoding relinquish control request: semaphore name contructor done	*/
/* was encountered.							*/
/************************************************************************/

static ST_VOID relctrl_sem_name_cstr_done (ASN1_DEC_CTXT *aCtx)
  {

  ASN1R_TAG_ADD (aCtx, CTX, 1, relctrl_get_named_token  );
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }

/************************************************************************/
/*			relctrl_get_named_token 			*/
/* Decoding relinquish control request: named token was encountered.	*/
/************************************************************************/

static ST_VOID relctrl_get_named_token (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("relctrl_get_named_token");

  req_info->named_token_pres = SD_TRUE;	/* set named token present	*/

			/* read named token from received message	*/
  if (asn1r_get_identifier (aCtx, req_info->named_token))
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_relctrl_resp 				*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE and send the relinquish control response.		*/
/************************************************************************/

ST_RET mp_relctrl_resp (MMSREQ_IND *indptr)
  {
  return (_mms_send_null_resp (indptr,MMSOP_REL_CONTROL));
  }

#else
/************************************************************************/
/************************************************************************/
/*			mpl_relctrl_resp 				*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the relinquish control response.			*/
/************************************************************************/

ST_RET mpl_relctrl_resp (ST_UINT32 invoke_id)
  {
  return (_mms_fin_null_resp (invoke_id,MMSOP_REL_CONTROL));
  }
#endif
