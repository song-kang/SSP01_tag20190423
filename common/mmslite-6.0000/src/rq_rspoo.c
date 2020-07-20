/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rq_rspool.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the requester portion of report pool	*/
/*	semaphore status.  It decodes the report pool semaphore status	*/
/*	response (confirm) & encodes the report pool semahpore status	*/
/*	request.							*/
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
/* 06/15/98  MDE     01    Changes to allow compile under C++		*/
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

static  RSPOOL_RESP_INFO	*rsp_info;


static TOKEN_ID		*token_ptr;
static ST_INT	max_named_tokens;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

static ST_VOID rspool_get_named_token (ASN1_DEC_CTXT *aCtx);
static ST_VOID rspool_named_token_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID rspool_named_token_cstr (ASN1_DEC_CTXT *aCtx);
static ST_VOID rspool_get_more_follows (ASN1_DEC_CTXT *aCtx);

/************************************************************************/
/************************************************************************/
/*			mms_rspool_rsp					*/
/* This function is called from mmsdec when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_rspool_rsp (ASN1_DEC_CTXT *aCtx)
  {
ST_INT	rsp_size;

  MLOG_CDEC0 ("Report Pool Semaphore Status Response");
				
#if defined(FIXED_DECBUF)
  max_named_tokens = m_cl_max_named_tokens;
  rsp_size = mmsl_dec_info_size;
#else
  max_named_tokens = _mmsdec_msglen / 2; /* find maximum size */
  if (m_cl_max_named_tokens && (max_named_tokens > m_cl_max_named_tokens))
    max_named_tokens = m_cl_max_named_tokens;
  rsp_size = sizeof (RSPOOL_RESP_INFO)
		+ (sizeof (TOKEN_ID) * max_named_tokens);
#endif

/* get storage for request information	*/
  rsp_info = (RSPOOL_RESP_INFO *) _m_get_dec_buf (aCtx, rsp_size);
  token_ptr = (TOKEN_ID *) (rsp_info + 1);

  rsp_info->num_of_tokens = 0;	/* initialize number of named tokens	*/

  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 0, rspool_named_token_cstr);
  }

/************************************************************************/
/*			rspool_named_token_cstr 			*/
/* Decoding report pool semaphore status request: named token list	*/
/* constructor was encountered. 					*/
/************************************************************************/

static ST_VOID rspool_named_token_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("rspool_named_token_cstr");

  ASN1R_TAG_ADD (aCtx, CTX, 0, rspool_get_named_token);
  ASN1R_TAG_ADD (aCtx, CTX, 1, rspool_get_named_token);
  ASN1R_TAG_ADD (aCtx, CTX, 2, rspool_get_named_token);

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = rspool_named_token_cstr_done;
  }

/************************************************************************/
/*			rspool_get_named_token				*/
/* Decoding report pool semaphore status request: named token was	*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID rspool_get_named_token (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("rspool_get_named_token");

  rsp_info->num_of_tokens++;
  if (rsp_info->num_of_tokens > max_named_tokens)
    {			/* too many named tokens			*/
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);
    return;
    }

  token_ptr->token_tag	 = aCtx->asn1r_elmnt_id;

#if defined(USE_COMPACT_MMS_STRUCTS)
  token_ptr->named_token = (ST_CHAR *) (aCtx->asn1r_field_ptr - 1);
#endif

			/* read free token from the received message	*/
  if (asn1r_get_identifier (aCtx, token_ptr->named_token))
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);

  token_ptr++;		/* point to where next named token will go	*/
  asn1r_tag_restore (aCtx);
  }

/************************************************************************/
/*			rspool_named_token_cstr_done			*/
/* Decoding report pool semaphore status request: named token constr	*/
/* done was encountered.						*/
/************************************************************************/

static ST_VOID rspool_named_token_cstr_done (ASN1_DEC_CTXT *aCtx)
  {

  ASN1R_TAG_ADD (aCtx, CTX, 1, rspool_get_more_follows);
  }

/************************************************************************/
/*			rspool_get_more_follows 			*/
/* Decoding report pool semaphore status request: more follows indicator*/
/* was encountered.							*/
/************************************************************************/

static ST_VOID rspool_get_more_follows (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("rspool_get_more_follows");
			/* read more follows indicator from message	*/
  if (asn1r_get_bool (aCtx, &(rsp_info->more_follows)))
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_rspool					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE and send the report pool semaphore status request. */
/************************************************************************/

MMSREQ_PEND *mp_rspool (ST_INT chan, RSPOOL_REQ_INFO *info_ptr)
  {
  return (_mms_req_send (MMSOP_REP_SEMPOOL,
			M_CAST_MK_FUN (_ms_mk_rspool_req),
			chan,(ST_CHAR *)info_ptr));
  } 

#else
/************************************************************************/
/************************************************************************/
/*			mpl_rspool					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the report pool semaphore status request. 		*/
/************************************************************************/

ST_RET  mpl_rspool (RSPOOL_REQ_INFO *info_ptr)
  {
  return (_mms_req_fin (MMSOP_REP_SEMPOOL,
			M_CAST_MK_FUN (_ms_mk_rspool_req),
			(ST_CHAR *)info_ptr));
  } 
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_rspool_req					*/
/* construct a report pool semaphore status request			*/
/************************************************************************/

ST_VOID _ms_mk_rspool_req (ASN1_ENC_CTXT *aCtx, RSPOOL_REQ_INFO *info)
  {

  if (info->start_after_pres)
    {
    asn1r_wr_vstr (aCtx, info->start_after);	/* write app to start after	*/
    asn1r_fin_prim (aCtx, 1, CTX);			/* context tag 1		*/
    }

  asn1r_strt_constr (aCtx);			/* start object name constr	*/

  _ms_wr_mms_objname (aCtx, &info->sem_name);	/* write the object name	*/

  asn1r_fin_constr (aCtx, 0,CTX,DEF);		/* context tag 0		*/

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_constr (aCtx, MMSOP_REP_SEMPOOL,CTX,DEF);	/* tag = opcode, ctx constr	*/
  }
