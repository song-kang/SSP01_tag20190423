/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rs_rsentry.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the responder portion of report semaphore	*/
/*	entry status.  It decodes the report semaphore entry status	*/
/*	request (indication) and encodes the report semaphore status	*/
/*	response.							*/
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

static  RSENTRY_REQ_INFO	*req_info;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

static ST_VOID rsentry_start_after_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID rsentry_get_start_after       (ASN1_DEC_CTXT *aCtx);
static ST_VOID rsentry_start_after_cstr      (ASN1_DEC_CTXT *aCtx);
static ST_VOID rsentry_get_state             (ASN1_DEC_CTXT *aCtx);
static ST_VOID rsentry_sem_name_cstr_done    (ASN1_DEC_CTXT *aCtx);
static ST_VOID rsentry_objname_done          (ASN1_DEC_CTXT *aCtx);
static ST_VOID rsentry_sem_name_cstr         (ASN1_DEC_CTXT *aCtx);


/************************************************************************/
/************************************************************************/
/*			mms_rsentry_req 				*/
/* This function is called from mmsdec when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_rsentry_req (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("Report Semaphore Entry Status Request");


/* get storage for request information	*/
  req_info = (RSENTRY_REQ_INFO *) _m_get_dec_buf (aCtx, sizeof (RSENTRY_REQ_INFO));
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 0, rsentry_sem_name_cstr);
  }

/************************************************************************/
/*			rsentry_sem_name_cstr				*/
/* Decoding report semaphore entry status request: semaphore name	*/
/* constructor was encountered. 					*/
/************************************************************************/

static ST_VOID rsentry_sem_name_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("rsentry_sem_name_cstr");

  _ms_get_mms_objname (aCtx, &req_info->sem_name, rsentry_objname_done);
  }

/************************************************************************/
/*			 rsentry_objname_done				*/
/* Decoding a Report Semaphore Entry Status Request: Semaphore name has */
/* been obtained.							*/
/************************************************************************/

static ST_VOID rsentry_objname_done (ASN1_DEC_CTXT *aCtx)
  {

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = rsentry_sem_name_cstr_done;
  }

/************************************************************************/
/*			rsentry_sem_name_cstr_done			*/
/* Decoding report semaphore entry status request: semaphore name	*/
/* contructor done was encountered.					*/
/************************************************************************/

static ST_VOID rsentry_sem_name_cstr_done (ASN1_DEC_CTXT *aCtx)
  {

  ASN1R_TAG_ADD (aCtx, CTX, 1, rsentry_get_state);
  }

/************************************************************************/
/*			rsentry_get_state				*/
/* Decoding report semaphore entry status request: semaphore state was	*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID rsentry_get_state (ASN1_DEC_CTXT *aCtx)
  {
ST_INT16	 state;

  MLOG_CDEC0 ("rsentry_get_state");

			/* read semaphore state from received message	*/
  if (asn1r_get_i16 (aCtx, &state))
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);

  if ((state < 0) || (state > 2))    /* validate range of state [0..2]*/
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);
  else
    req_info->state = state;		/* valid semaphore state	*/

  ASN1R_TAG_ADD (aCtx, CTX, 	 2, rsentry_get_start_after);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 2, rsentry_start_after_cstr);
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }

/************************************************************************/
/*			rsentry_get_start_after 			*/
/* Decoding report semaphore entry status request: entry id to start	*/
/* after primitive was encountered.					*/
/************************************************************************/

static ST_VOID rsentry_get_start_after (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("rsentry_get_start_after");

  req_info->start_after_pres = SD_TRUE;	/* set start after name present */

	/* decode entry id onto itself, shifted by one character	*/
  req_info->start_after = aCtx->asn1r_field_ptr - 1;

					/* read entry id from message	*/
  if (asn1r_get_octstr (aCtx, req_info->start_after))
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);

  req_info->sa_len = aCtx->asn1r_octetcount;	/* save entry id length 	*/
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }

/************************************************************************/
/*			rsentry_start_after_cstr			*/
/* Decoding report semaphore entry status request: entry id to start	*/
/* after constructor was encountered.					*/
/************************************************************************/

static ST_VOID rsentry_start_after_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("rsentry_start_after_cstr");

  req_info->start_after_pres = SD_TRUE;	/* set start after name present */

	/* decode entry id onto itself, shifted by one character	*/
  req_info->start_after = aCtx->asn1r_field_ptr - 1;

			/* set up where to go when done decoding cstr	*/
  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = rsentry_start_after_cstr_done;

			/* read entry id from the received message	*/
  asn1r_get_octstr_cstr (aCtx, _mmsdec_msglen, req_info->start_after);
  }

/************************************************************************/
/*			rsentry_start_after_cstr_done			*/
/* Decoding report semaphore entry status request: entry id to start	*/
/* after constructor done was encountered.				*/
/************************************************************************/

static ST_VOID rsentry_start_after_cstr_done (ASN1_DEC_CTXT *aCtx)
  {

  req_info->sa_len = aCtx->asn1r_octetcount;	/* save the start after length	*/
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_rsentry_resp 				*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE and send the report pool semaphore entry status	*/
/* response.								*/
/************************************************************************/

ST_RET mp_rsentry_resp (MMSREQ_IND *indptr, RSENTRY_RESP_INFO *info_ptr)
  {
  return (_mms_resp_send (MMSOP_REP_SEMENTRY,indptr,
			M_CAST_MK_FUN (_ms_mk_rsentry_resp),
			(ST_CHAR *)info_ptr));
  }

#else
/************************************************************************/
/************************************************************************/
/*			mpl_rsentry_resp 				*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the report pool semaphore entry status response.	*/
/************************************************************************/

ST_RET mpl_rsentry_resp (ST_UINT32 invoke_id, RSENTRY_RESP_INFO *info_ptr)
  {
  return (_mms_resp_fin (MMSOP_REP_SEMENTRY,invoke_id,
			M_CAST_MK_FUN (_ms_mk_rsentry_resp),
			(ST_CHAR *)info_ptr));
  }
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_rsentry_resp 				*/
/* Construct a report semaphore entry status response.			*/
/************************************************************************/

ST_VOID _ms_mk_rsentry_resp (ASN1_ENC_CTXT *aCtx, RSENTRY_RESP_INFO *info)
  {
SEMAPHORE_ENTRY *entry_ptr;
ST_INT i;

  asn1r_strt_constr (aCtx);			/* start the sequence constr	*/

  if (!info->more_follows)		/* if not default value		*/
    {
    asn1r_wr_bool (aCtx, info->more_follows); 	/* write more follows indicator */
    asn1r_fin_prim (aCtx, 1,CTX);
    }

  asn1r_strt_constr (aCtx);			/* start the ctx 0 constr	*/

  entry_ptr  = (SEMAPHORE_ENTRY *) (info + 1);
  entry_ptr += info->num_of_sent - 1;	/* point to last sem entry	*/
  for (i = 0; i < info->num_of_sent; i++)
    {
    asn1r_strt_constr (aCtx);			/* start the sequence constr	*/
					/* write the entry id		*/
    if (!entry_ptr->rel_conn_lost)
      { 				/* NOT the default value	*/
      asn1r_wr_bool (aCtx, entry_ptr->rel_conn_lost);
      asn1r_fin_prim (aCtx, 7,CTX);
      }

    if (entry_ptr->abrt_on_timeout_pres)
      {
      asn1r_wr_bool (aCtx, entry_ptr->abrt_on_timeout);
      asn1r_fin_prim (aCtx, 6,CTX);
      }

    if (entry_ptr->rem_timeout_pres)
      {
      asn1r_wr_u32 (aCtx, entry_ptr->rem_timeout);
      asn1r_fin_prim (aCtx, 5,CTX);
      }

    if (entry_ptr->priority != 64)
      { 				/* NOT the default value	*/
      asn1r_wr_u8 (aCtx, entry_ptr->priority);
      asn1r_fin_prim (aCtx, 4,CTX);
      }

    if (entry_ptr->named_token_pres)
      {
      asn1r_wr_vstr (aCtx, entry_ptr->named_token);
      asn1r_fin_prim (aCtx, 3,CTX);
      }

    asn1r_strt_constr (aCtx);			/* start the ctx 1 constr	*/
    asn1r_wr_delmnt (aCtx, entry_ptr->app_ref,entry_ptr->app_ref_len);
    asn1r_fin_constr (aCtx, 2,CTX,DEF);		/* finish ctx 1 constructor	*/

    asn1r_wr_i8 (aCtx, entry_ptr->entry_class);	/* write entry class		*/
    asn1r_fin_prim (aCtx, 1,CTX);

    asn1r_wr_octstr (aCtx, entry_ptr->entry_id, entry_ptr->ei_len);
    asn1r_fin_prim (aCtx, 0,CTX);

    asn1r_fin_constr (aCtx, SEQ_CODE,UNI,DEF);	/* finish sequence constructor	*/
    entry_ptr--;
    }
  asn1r_fin_constr (aCtx, 0,CTX,DEF);		/* finish ctx 0 constructor	*/

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_constr (aCtx, MMSOP_REP_SEMENTRY,CTX,DEF);	/* tag = opcode, ctx constr	*/
  }
