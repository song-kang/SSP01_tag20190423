/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rs_delsem.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the responder portion of the delete	*/
/*	semaphore. It decodes the delete semaphore request (indication) */
/*	and encodes the delete semaphore response.			*/
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
#include "mms_psem.h"
#include "asn1defs.h"

/************************************************************************/
/* variables global to the DECODE portion :				*/

static  DELSEM_REQ_INFO	*req_info;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

static ST_VOID delsem_objname_done  (ASN1_DEC_CTXT *aCtx);


/************************************************************************/
/************************************************************************/
/*			mms_delsem_req					*/
/* This function is called from mmsdec when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_delsem_req (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("Delete Semaphore Request");

/* get storage for request information	*/
  req_info = (DELSEM_REQ_INFO *) _m_get_dec_buf (aCtx, sizeof (DELSEM_REQ_INFO));
  _ms_get_mms_objname (aCtx, &req_info->sem_name, delsem_objname_done);
  }

/************************************************************************/
/*			delsem_objname_done				*/
/* Decoding delete semaphore request: semaphore name obtained		*/
/************************************************************************/

static ST_VOID delsem_objname_done (ASN1_DEC_CTXT *aCtx)
  {
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_delsem_resp					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE and send the delete semaphore response.		*/
/************************************************************************/

ST_RET mp_delsem_resp (MMSREQ_IND *indptr)
  {
  return (_mms_send_null_resp (indptr,MMSOP_DELETE_SEM));
  }

#else
/************************************************************************/
/************************************************************************/
/*			mpl_delsem_resp					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the delete semaphore response.			*/
/************************************************************************/

ST_RET mpl_delsem_resp (ST_UINT32 invoke_id)
  {
  return (_mms_fin_null_resp (invoke_id,MMSOP_DELETE_SEM));
  }
#endif
