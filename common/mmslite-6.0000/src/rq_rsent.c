/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rq_rsentry.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the requester portion of report semaphore	*/
/*	entry status.  It decodes the report semaphore entry status	*/
/*	response (confirm) & encodes the report semaphore entry status	*/
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

static  RSENTRY_RESP_INFO	*rsp_info;

static SEMAPHORE_ENTRY	*sent_ptr;
static ST_INT	max_sem_entries;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

static ST_VOID rsentry_get_entry_class     (ASN1_DEC_CTXT *aCtx);
static ST_VOID rsentry_app_ref_cstr        (ASN1_DEC_CTXT *aCtx);
static ST_VOID rsentry_get_priority        (ASN1_DEC_CTXT *aCtx);
static ST_VOID rsentry_get_rem_timeout     (ASN1_DEC_CTXT *aCtx);
static ST_VOID rsentry_get_abrt_timeout    (ASN1_DEC_CTXT *aCtx);
static ST_VOID rsentry_get_rel_conn_lost   (ASN1_DEC_CTXT *aCtx);
static ST_VOID rsentry_entry_seq_cstr      (ASN1_DEC_CTXT *aCtx);
static ST_VOID rsentry_get_named_token     (ASN1_DEC_CTXT *aCtx);
static ST_VOID rsentry_entry_seq_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID app_ref_cstr_done           (ASN1_DEC_CTXT *aCtx);
static ST_VOID app_ref_done                (ASN1_DEC_CTXT *aCtx);
static ST_VOID rsentry_entry_id_cstr_done  (ASN1_DEC_CTXT *aCtx);
static ST_VOID rsentry_get_entry_id        (ASN1_DEC_CTXT *aCtx);
static ST_VOID rsentry_entry_id_cstr       (ASN1_DEC_CTXT *aCtx);
static ST_VOID rsentry_sem_entry_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID rsentry_sem_entry_cstr      (ASN1_DEC_CTXT *aCtx);
static ST_VOID rsentry_get_more_follows    (ASN1_DEC_CTXT *aCtx);


/************************************************************************/
/************************************************************************/
/*			mms_rsentry_rsp 				*/
/* This function is called from mmsdec when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_rsentry_rsp (ASN1_DEC_CTXT *aCtx)
  {
ST_INT rsp_size;

  MLOG_CDEC0 ("Report Semaphore Entry Status Response");
	
#if defined(FIXED_DECBUF)
  max_sem_entries = m_cl_max_semaphore_entries;
  rsp_size = mmsl_dec_info_size;
#else
  max_sem_entries = _mmsdec_msglen / 6; /* find maximum size of response info	*/
  if (m_cl_max_semaphore_entries && (max_sem_entries > m_cl_max_semaphore_entries))
    max_sem_entries = m_cl_max_semaphore_entries;
  rsp_size = sizeof (RSENTRY_RESP_INFO)
		+ (sizeof (SEMAPHORE_ENTRY) * max_sem_entries);
#endif

/* get storage for request information	*/
  rsp_info = (RSENTRY_RESP_INFO *) _m_get_dec_buf (aCtx, rsp_size);
  sent_ptr  = (SEMAPHORE_ENTRY *) (rsp_info + 1);

  rsp_info->more_follows = SD_TRUE;/* set default value for more follows	*/

  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 0, rsentry_sem_entry_cstr);
  }

/************************************************************************/
/*			rsentry_sem_entry_cstr				*/
/* Decoding report semaphore entry status request: semaphore entry list */
/* constructor was encountered. 					*/
/************************************************************************/

static ST_VOID rsentry_sem_entry_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("rsentry_sem_entry_cstr");

  ASN1R_TAG_ADD (aCtx, UNI | CONSTR, SEQ_CODE, rsentry_entry_seq_cstr);
  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = rsentry_sem_entry_cstr_done;
  }

/************************************************************************/
/*			rsentry_entry_seq_cstr				*/
/* Decoding report semaphore entry status request: semaphore entry	*/
/* sequence constructor was encountered.				*/
/************************************************************************/

static ST_VOID rsentry_entry_seq_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("rsentry_entry_seq_cstr");

  rsp_info->num_of_sent++;		/* increment number of entries	*/
  if (rsp_info->num_of_sent > max_sem_entries)
    {			/* too many semaphore entries			*/
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);
    return;
    }
					
  sent_ptr->priority	     = MMS_NORM_PRIORITY;	/* set defaults		*/
  sent_ptr->rel_conn_lost    = SD_TRUE;

  ASN1R_TAG_ADD (aCtx, CTX, 	 0, rsentry_get_entry_id);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 0, rsentry_entry_id_cstr);
  }

/************************************************************************/
/*			rsentry_get_entry_id				*/
/* Decoding report semaphore entry status request: entry id primitive	*/
/* was encountered.							*/
/************************************************************************/

static ST_VOID rsentry_get_entry_id (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("txdwn_get_entry_id");

	/* decode entry id onto itself, shifted by one character	*/
  sent_ptr->entry_id = aCtx->asn1r_field_ptr - 1;

					/* read entry id from message	*/
  if (asn1r_get_octstr (aCtx, sent_ptr->entry_id))
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);

  sent_ptr->ei_len = aCtx->asn1r_octetcount;	/* save the entry id length	*/

  ASN1R_TAG_ADD (aCtx, CTX, 1, rsentry_get_entry_class);
  }

/************************************************************************/
/*			rsentry_entry_id_cstr				*/
/* Decoding report semaphore entry status request: entry id constructor */
/* was encountered.							*/
/************************************************************************/

static ST_VOID rsentry_entry_id_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("rsentry_entry_id_cstr");

	/* decode entry id onto itself, shifted by one character	*/
  sent_ptr->entry_id = aCtx->asn1r_field_ptr - 1;

			/* set up where to go when done decoding cstr	*/
  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = rsentry_entry_id_cstr_done;

			/* read entry id from the received message	*/
  asn1r_get_octstr_cstr (aCtx, _mmsdec_msglen, sent_ptr->entry_id);
  }

/************************************************************************/
/*			rsentry_entry_id_cstr_done			*/
/* Decoding report semaphore entry status request: entry id constructor */
/* done was encountered.						*/
/************************************************************************/

static ST_VOID rsentry_entry_id_cstr_done (ASN1_DEC_CTXT *aCtx)
  {

  sent_ptr->ei_len = aCtx->asn1r_octetcount;	/* save the entry id length	*/

  ASN1R_TAG_ADD (aCtx, CTX, 1, rsentry_get_entry_class);
  }

/************************************************************************/
/*			rsentry_get_entry_class				*/
/* Decoding report semaphore entry status request: entry class		*/
/* was encountered.							*/
/************************************************************************/

static ST_VOID rsentry_get_entry_class (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("rsentry_get_entry_class");

  if (asn1r_get_i8 (aCtx, &sent_ptr->entry_class))
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);

  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 2, rsentry_app_ref_cstr);
  }


/************************************************************************/
/*			rsentry_app_ref_cstr				*/
/* Decoding report semaphore entry status request: application		*/
/* reference constructor was encountered.				*/
/************************************************************************/

static ST_VOID rsentry_app_ref_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("rsentry_app_ref_cstr");

  _ms_get_mms_app_ref (aCtx, &sent_ptr->app_ref,&sent_ptr->app_ref_len,
					 app_ref_done);
  }

/************************************************************************/
/*			app_ref_done					*/
/* Decoding report semaphore entry status request: application		*/
/* reference constructor has ben read, OK for CTX 2 to be done.		*/
/************************************************************************/

static ST_VOID app_ref_done (ASN1_DEC_CTXT *aCtx)
  {

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = app_ref_cstr_done;
  }

/************************************************************************/
/*			app_ref_cstr_done				*/
/* Decoding report semaphore entry status request: application		*/
/* reference constructor done was encountered.				*/
/************************************************************************/

static ST_VOID app_ref_cstr_done (ASN1_DEC_CTXT *aCtx)
  {

  ASN1R_TAG_ADD (aCtx, CTX, 3, rsentry_get_named_token );
  ASN1R_TAG_ADD (aCtx, CTX, 4, rsentry_get_priority);
  ASN1R_TAG_ADD (aCtx, CTX, 5, rsentry_get_rem_timeout);
  ASN1R_TAG_ADD (aCtx, CTX, 6, rsentry_get_abrt_timeout);
  ASN1R_TAG_ADD (aCtx, CTX, 7, rsentry_get_rel_conn_lost);

/* Was last mandatory SemaphoreEntry Parameter, OK for it to be done	*/
  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = rsentry_entry_seq_cstr_done;
  }

/************************************************************************/
/*			rsentry_get_named_token 			*/
/* Decoding report semaphore entry status request: named token		*/
/* primitve was encountered.						*/
/************************************************************************/

static ST_VOID rsentry_get_named_token (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("rsentry_get_named_token");

  if (aCtx->asn1r_elmnt_len > MAX_IDENT_LEN)
    {			/* named token is too ST_INT32			*/
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);
    return;
    }
			/* read named token from the received message	*/
  sent_ptr->named_token_pres = SD_TRUE;
  if (asn1r_get_vstr (aCtx, sent_ptr->named_token))
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);

  ASN1R_TAG_ADD (aCtx, CTX, 4, rsentry_get_priority);
  ASN1R_TAG_ADD (aCtx, CTX, 5, rsentry_get_rem_timeout);
  ASN1R_TAG_ADD (aCtx, CTX, 6, rsentry_get_abrt_timeout);
  ASN1R_TAG_ADD (aCtx, CTX, 7, rsentry_get_rel_conn_lost);
  }

/************************************************************************/
/*			rsentry_get_priority				*/
/* Decoding report semaphore entry status request: priority primitive	*/
/* was encountered.							*/
/************************************************************************/

static ST_VOID rsentry_get_priority (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("rsentry_get_priority");

			/* read priority from the received message	*/
  if (asn1r_get_u8 (aCtx, &(sent_ptr->priority)))
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);

  ASN1R_TAG_ADD (aCtx, CTX, 5, rsentry_get_rem_timeout);
  ASN1R_TAG_ADD (aCtx, CTX, 6, rsentry_get_abrt_timeout);
  ASN1R_TAG_ADD (aCtx, CTX, 7, rsentry_get_rel_conn_lost);
  }

/************************************************************************/
/*			rsentry_get_rem_timeout 			*/
/* Decoding report semaphore entry status request: remaining timeout	*/
/* primitve was encountered.						*/
/************************************************************************/

static ST_VOID rsentry_get_rem_timeout (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("rsentry_get_rem_timeout");
			/* read remaining timeout from received message */
  sent_ptr->rem_timeout_pres = SD_TRUE;
  if (asn1r_get_u32 (aCtx, &(sent_ptr->rem_timeout)))
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);

  ASN1R_TAG_ADD (aCtx, CTX, 6, rsentry_get_abrt_timeout);
  ASN1R_TAG_ADD (aCtx, CTX, 7, rsentry_get_rel_conn_lost);
  }

/************************************************************************/
/*			rsentry_get_abrt_timeout			*/
/* Decoding report semaphore entry status request: relinquish if	*/
/* connection lost primitve was encountered.				*/
/************************************************************************/

static ST_VOID rsentry_get_abrt_timeout (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("rsentry_get_abrt_timeout");
			/* read abort on timeout			*/
  sent_ptr->abrt_on_timeout_pres = SD_TRUE;
  if (asn1r_get_bool (aCtx, &sent_ptr->abrt_on_timeout))
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);
  ASN1R_TAG_ADD (aCtx, CTX, 7, rsentry_get_rel_conn_lost);
  }

/************************************************************************/
/*			rsentry_get_rel_conn_lost			*/
/* Decoding report semaphore entry status request: relinquish if	*/
/* connection lost primitve was encountered.				*/
/************************************************************************/

static ST_VOID rsentry_get_rel_conn_lost (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("rsentry_get_rel_conn_lost");
			/* read relinquish if conn lost from message	*/
  if (asn1r_get_bool (aCtx, &(sent_ptr->rel_conn_lost)))
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);
  }

/************************************************************************/
/*			rsentry_entry_seq_cstr_done			*/
/* Decoding report semaphore entry status request: semaphore entry	*/
/* sequence constructor done was encountered.				*/
/************************************************************************/

static ST_VOID rsentry_entry_seq_cstr_done (ASN1_DEC_CTXT *aCtx)
  {

  sent_ptr++;			/* point to where next will go		*/

  ASN1R_TAG_ADD (aCtx, UNI | CONSTR, SEQ_CODE, rsentry_entry_seq_cstr);
  }

/************************************************************************/
/*			rsentry_sem_entry_cstr_done			*/
/* Decoding report semaphore entry status request: semaphore entry	*/
/* constructor done was encountered.					*/
/************************************************************************/

static ST_VOID rsentry_sem_entry_cstr_done (ASN1_DEC_CTXT *aCtx)
  {

  ASN1R_TAG_ADD (aCtx, CTX, 1, rsentry_get_more_follows);

  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }

/************************************************************************/
/*			rsentry_get_more_follows			*/
/* Decoding report semaphore entry status request: more follows 	*/
/* indicator was encountered.						*/
/************************************************************************/

static ST_VOID rsentry_get_more_follows (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("rsentry_get_more_follows");
			/* read more follows indicator from message	*/
  if (asn1r_get_bool (aCtx, &(rsp_info->more_follows)))
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);

  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_rsentry					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE and send the report semaphore entry status request.*/
/************************************************************************/

MMSREQ_PEND *mp_rsentry (ST_INT chan, RSENTRY_REQ_INFO *info_ptr)
  {
  return (_mms_req_send (MMSOP_REP_SEMENTRY,
			M_CAST_MK_FUN (_ms_mk_rsentry_req),
			chan,(ST_CHAR *)info_ptr));
  } 

#else
/************************************************************************/
/************************************************************************/
/*			mpl_rsentry					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the report semaphore entry status request.		*/
/************************************************************************/

ST_RET  mpl_rsentry (RSENTRY_REQ_INFO *info_ptr)
  {
  return (_mms_req_fin (MMSOP_REP_SEMENTRY,
			M_CAST_MK_FUN (_ms_mk_rsentry_req),
			(ST_CHAR *)info_ptr));
  } 
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_rsentry_req					*/
/* construct a report semaphore entry status request			*/
/************************************************************************/

ST_VOID _ms_mk_rsentry_req (ASN1_ENC_CTXT *aCtx, RSENTRY_REQ_INFO *info)
  {

  asn1r_strt_constr (aCtx);			/* start sequence constructor	*/

  if (info->start_after_pres)
    {					/* write app to start after	*/
    asn1r_wr_octstr (aCtx, info->start_after, info->sa_len);
    asn1r_fin_prim (aCtx, 2, CTX);			/* context tag 2		*/
    }

  asn1r_wr_i16 (aCtx, info->state); 		/* write the semaphore state	*/
  asn1r_fin_prim (aCtx, 1, CTX);			/* context tag 1		*/

  asn1r_strt_constr (aCtx);			/* start object name constr	*/

  _ms_wr_mms_objname (aCtx, &info->sem_name);	/* write the object name	*/

  asn1r_fin_constr (aCtx, 0,CTX,DEF);		/* context tag 0		*/

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_constr (aCtx, MMSOP_REP_SEMENTRY,CTX,DEF);	/* tag = opcode, ctx constr	*/
  }
