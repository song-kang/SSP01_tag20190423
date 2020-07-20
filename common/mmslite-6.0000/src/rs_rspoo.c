/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rs_rspool.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the responder portion of report pool	*/
/*	semaphore status.  It decodes the report pool semaphroe status	*/
/*	request (indication) and encodes the report pool semaphore	*/
/*	status response.						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 03/11/04  GLB     08    Remove "thisFileName"                    	*/
/* 08/28/03  EJV     07    Corr spelling for 'receive'.			*/
/* 12/20/01  JRB     06    Converted to use ASN1R (re-entrant ASN1)	*/
/* 09/13/99  MDE     05    Added SD_CONST modifiers			*/
/* 03/23/99  MDE     04    Changes to decode buffer allocation scheme	*/
/* 06/15/98  MDE     03    Changes to allow compile under C++		*/
/* 07/03/97  MDE     02    Op-specific info buffer handling changes	*/
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
 
static  RSPOOL_REQ_INFO	*req_info;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

static ST_VOID rspool_get_start_after (ASN1_DEC_CTXT *aCtx);
static ST_VOID rspool_sem_name_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID rspool_objname_done  (ASN1_DEC_CTXT *aCtx);
static ST_VOID rspool_sem_name_cstr (ASN1_DEC_CTXT *aCtx);


/************************************************************************/
/************************************************************************/
/*			mms_rspool_req					*/
/* This function is called from mmsdec when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_rspool_req (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("Report Pool Semaphore Status Request");


/* get storage for request information	*/
  req_info = (RSPOOL_REQ_INFO *) _m_get_dec_buf (aCtx, sizeof (RSPOOL_REQ_INFO));
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 0, rspool_sem_name_cstr);
  }

/************************************************************************/
/*			rspool_sem_name_cstr				*/
/* Decoding report semaphore status request: semaphore name constructor */
/* was encountered.							*/
/************************************************************************/

static ST_VOID rspool_sem_name_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("rspool_sem_name_cstr");

  _ms_get_mms_objname (aCtx, &req_info->sem_name, rspool_objname_done);
  }

/************************************************************************/
/*			rspool_objname_done				*/
/* Decoding Report Semaphore Status Response: Semaphore name has been	*/
/* obtained								*/
/************************************************************************/

static ST_VOID rspool_objname_done (ASN1_DEC_CTXT *aCtx)
  {

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = rspool_sem_name_cstr_done;
  }

/************************************************************************/
/*			rspool_sem_name_cstr_done			*/
/* Decoding report semaphore status request: semaphore name contructor	*/
/* done was encountered.						*/
/************************************************************************/

static ST_VOID rspool_sem_name_cstr_done (ASN1_DEC_CTXT *aCtx)
  {

  ASN1R_TAG_ADD (aCtx, CTX, 1, rspool_get_start_after);
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }

/************************************************************************/
/*			rspool_get_start_after				*/
/* Decoding report semaphore status request: application name to start	*/
/* after was encountered.						*/
/************************************************************************/

static ST_VOID rspool_get_start_after (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("rspool_get_start_after");

  req_info->start_after_pres = SD_TRUE;

			/* read start after name from received message	*/
  if (asn1r_get_identifier (aCtx, req_info->start_after))
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_rspool_resp					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE and send the report pool semaphore status response.*/
/************************************************************************/

ST_RET mp_rspool_resp (MMSREQ_IND *indptr, RSPOOL_RESP_INFO *info_ptr)
  {
  return (_mms_resp_send (MMSOP_REP_SEMPOOL,indptr,
			M_CAST_MK_FUN (_ms_mk_rspool_resp),
			(ST_CHAR *)info_ptr));
  }

#else
/************************************************************************/
/************************************************************************/
/*			mpl_rspool_resp					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the report pool semaphore status response.		*/
/************************************************************************/

ST_RET mpl_rspool_resp (ST_UINT32 invoke_id, RSPOOL_RESP_INFO *info_ptr)
  {
  return (_mms_resp_fin (MMSOP_REP_SEMPOOL,invoke_id,
			M_CAST_MK_FUN (_ms_mk_rspool_resp),
			(ST_CHAR *)info_ptr));
  }
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_rspool_resp					*/
/* Construct a report semaphore status response.			*/
/************************************************************************/

ST_VOID _ms_mk_rspool_resp (ASN1_ENC_CTXT *aCtx, RSPOOL_RESP_INFO *info)
  {
TOKEN_ID *token_ptr;
ST_INT i;

  asn1r_strt_constr (aCtx);			/* start the sequence constr	*/

  asn1r_wr_bool (aCtx, info->more_follows);		/* write more follows indicator */
  asn1r_fin_prim (aCtx, 1,CTX);

  asn1r_strt_constr (aCtx);			/* start the ctx 0 constr	*/
  token_ptr  = (TOKEN_ID *) (info + 1);
  token_ptr += info->num_of_tokens - 1; /* point to last named token	*/
  for (i = 0; i < info->num_of_tokens; i++)
    {
    asn1r_wr_vstr (aCtx, token_ptr->named_token);	/* write the named token	*/
    asn1r_fin_prim (aCtx, token_ptr->token_tag,CTX);
    token_ptr--;
    }
  asn1r_fin_constr (aCtx, 0,CTX,DEF);		/* finish ctx 0 constructor	*/

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_constr (aCtx, MMSOP_REP_SEMPOOL,CTX,DEF);	/* tag = opcode, ctx constr	*/
  }
