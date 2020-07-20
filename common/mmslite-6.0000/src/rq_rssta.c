/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rq_rsstat.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the requester portion of report semaphore	*/
/*	status.  It decodes the report semaphore status response	*/
/*	(confirm) and encodes the report semaphore status request.	*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 03/11/04  GLB     07    Remove "thisFileName"                    	*/
/* 09/30/03  EJV     06    Corr spelling for 'receive'			*/
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
#include "mms_psem.h"
#include "asn1defs.h"

/************************************************************************/
/* variables global to the DECODE portion :				*/


static  RSSTAT_RESP_INFO	*rsp_info;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

static ST_VOID rsstat_get_num_of_owned (ASN1_DEC_CTXT *aCtx);
static ST_VOID rsstat_get_num_of_tokens (ASN1_DEC_CTXT *aCtx);
static ST_VOID rsstat_get_class (ASN1_DEC_CTXT *aCtx);
static ST_VOID rsstat_get_mms_deletable (ASN1_DEC_CTXT *aCtx);
static ST_VOID rsstat_get_num_of_hung (ASN1_DEC_CTXT *aCtx);

/************************************************************************/
/************************************************************************/
/*			mms_rsstat_rsp					*/
/* This function is called from mmsdec when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_rsstat_rsp (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("Report Semaphore Status Response");

/* get storage for request information	*/
  rsp_info = (RSSTAT_RESP_INFO *) _m_get_dec_buf (aCtx, sizeof (RSSTAT_RESP_INFO));
  ASN1R_TAG_ADD (aCtx, CTX, 0, rsstat_get_mms_deletable);
  }

/************************************************************************/
/*			rsstat_get_mms_deletable			*/
/* Decoding report semaphore status request: mms deletable name was	*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID rsstat_get_mms_deletable (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("rsstat_get_mms_deletable");
			/* read mms deletable from received message	*/
  if (asn1r_get_bool (aCtx, &(rsp_info->mms_deletable)))
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);

  ASN1R_TAG_ADD (aCtx, CTX, 1, rsstat_get_class);
  }

/************************************************************************/
/*			rsstat_get_class				*/
/* Decoding report semaphore status request: class was encountered.	*/
/************************************************************************/

static ST_VOID rsstat_get_class (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("rsstat_get_class");
			/* read the class from the received message	*/
  if (asn1r_get_i16 (aCtx, &(rsp_info->tclass)))
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);

  if ((rsp_info->tclass < 0) || (rsp_info->tclass > 1))
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);

  ASN1R_TAG_ADD (aCtx, CTX, 2, rsstat_get_num_of_tokens);
  }

/************************************************************************/
/*			rsstat_get_num_of_tokens			*/
/* Decoding report semaphore status request: number of tokens was	*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID rsstat_get_num_of_tokens (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("rsstat_get_num_of_tokens");
			/* read number of tokens from received message	*/
  if (asn1r_get_u16 (aCtx, &(rsp_info->num_of_tokens)))
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);

  ASN1R_TAG_ADD (aCtx, CTX, 3, rsstat_get_num_of_owned);
  }

/************************************************************************/
/*			rsstat_get_num_of_owned 			*/
/* Decoding report semaphore status request: number of owned tokens was */
/* encountered. 							*/
/************************************************************************/

static ST_VOID rsstat_get_num_of_owned (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("rsstat_get_num_of_owned");
			/* read number of owned tokens from message	*/
  if (asn1r_get_u16 (aCtx, &(rsp_info->num_of_owned)))
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);

  ASN1R_TAG_ADD (aCtx, CTX, 4, rsstat_get_num_of_hung);
  }

/************************************************************************/
/*			rsstat_get_num_of_hung				*/
/* Decoding report semaphore status request: number of hung tokens was	*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID rsstat_get_num_of_hung (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("rsstat_get_num_of_hung");
			/* read number of hung tokens from message	*/
  if (asn1r_get_u16 (aCtx, &(rsp_info->num_of_hung)))
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_rsstat					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE and send the report semaphore status request.	*/
/************************************************************************/

MMSREQ_PEND *mp_rsstat (ST_INT chan, RSSTAT_REQ_INFO *info_ptr)
  {
  return (_mms_req_send (MMSOP_REP_SEMSTAT,
			M_CAST_MK_FUN (_ms_mk_rsstat_req),
			chan,(ST_CHAR *)info_ptr));
  } 
									 
#else
/************************************************************************/
/************************************************************************/
/*			mpl_rsstat					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the report semaphore status request.		*/
/************************************************************************/

ST_RET  mpl_rsstat (RSSTAT_REQ_INFO *info_ptr)
  {
  return (_mms_req_fin (MMSOP_REP_SEMSTAT,
			M_CAST_MK_FUN (_ms_mk_rsstat_req),
			(ST_CHAR *)info_ptr));
  } 
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_rsstat_req					*/
/* construct a report semaphore status request				*/
/************************************************************************/

ST_VOID _ms_mk_rsstat_req (ASN1_ENC_CTXT *aCtx, RSSTAT_REQ_INFO *info)
  {

  _ms_wr_mms_objname (aCtx, &info->sem_name);	/* write the object name	*/

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_constr (aCtx, MMSOP_REP_SEMSTAT,CTX,DEF);	/* tag = opcode, ctx constr	*/
  }
