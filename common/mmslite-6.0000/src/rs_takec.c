/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rs_takectrl.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the responder portion of take semaphore	*/
/*	control.  It decodes the take control request (indication) and	*/
/*	encodes the take control response.				*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 11/23/11  KCR           Added casting for proper 64bit compile       */
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

static  TAKECTRL_REQ_INFO	*req_info;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

static ST_VOID takectrl_get_priority      (ASN1_DEC_CTXT *aCtx);
static ST_VOID takectrl_get_acc_delay     (ASN1_DEC_CTXT *aCtx);
static ST_VOID takectrl_get_ctrl_timeout  (ASN1_DEC_CTXT *aCtx);
static ST_VOID takectrl_get_abrt_timeout  (ASN1_DEC_CTXT *aCtx);
static ST_VOID takectrl_get_rel_conn_lost (ASN1_DEC_CTXT *aCtx);
static ST_VOID takectrl_app_preempt_cstr  (ASN1_DEC_CTXT *aCtx);
static ST_VOID takectrl_get_named_token   (ASN1_DEC_CTXT *aCtx);
static ST_VOID takectrl_sem_name_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID takectrl_objname_done  (ASN1_DEC_CTXT *aCtx);
static ST_VOID takectrl_sem_name_cstr (ASN1_DEC_CTXT *aCtx);
static ST_VOID app_preempt_cstr_done  (ASN1_DEC_CTXT *aCtx);

static ST_UCHAR *ar_start;


/************************************************************************/
/************************************************************************/
/*			mms_takectrl_req				*/
/* This function is called from mmsdec when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_takectrl_req (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("Take Control Request");

  req_info = (TAKECTRL_REQ_INFO *) _m_get_dec_buf (aCtx, sizeof (TAKECTRL_REQ_INFO));
  ASN1R_TAG_ADD (aCtx, CTX |CONSTR, 0, takectrl_sem_name_cstr);
  }

/************************************************************************/
/*			takectrl_sem_name_cstr				*/
/* Decoding take control request: semaphore name constructor was	*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID takectrl_sem_name_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("takectrl_sem_name_cstr");

				/* get storage for request information	*/
  req_info->priority	     = MMS_NORM_PRIORITY;
  req_info->rel_conn_lost    = SD_TRUE;
  _ms_get_mms_objname (aCtx, &req_info->sem_name, takectrl_objname_done);
  }

/************************************************************************/
/*			takectrl_objname_done				*/
/* Decoding take control request: Semaphore name has been obtained	*/
/************************************************************************/

static ST_VOID takectrl_objname_done (ASN1_DEC_CTXT *aCtx)
  {

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = takectrl_sem_name_cstr_done;
  }

/************************************************************************/
/*			takectrl_sem_name_cstr_done			*/
/* Decoding take control request: semaphore name contructor done was	*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID takectrl_sem_name_cstr_done (ASN1_DEC_CTXT *aCtx)
  {

  ASN1R_TAG_ADD (aCtx, CTX, 	 1, takectrl_get_named_token  );
  ASN1R_TAG_ADD (aCtx, CTX, 	 2, takectrl_get_priority     );
  ASN1R_TAG_ADD (aCtx, CTX, 	 3, takectrl_get_acc_delay    );
  ASN1R_TAG_ADD (aCtx, CTX, 	 4, takectrl_get_ctrl_timeout  );
  ASN1R_TAG_ADD (aCtx, CTX, 	 5, takectrl_get_abrt_timeout);
  ASN1R_TAG_ADD (aCtx, CTX, 	 6, takectrl_get_rel_conn_lost);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 7, takectrl_app_preempt_cstr );

  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  ar_start = aCtx->asn1r_field_ptr;
  }

/************************************************************************/
/*			takectrl_get_named_token			*/
/* Decoding take control request: named token name was encountered.	*/
/************************************************************************/

static ST_VOID takectrl_get_named_token (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("takectrl_get_named_token");

  req_info->named_token_pres = SD_TRUE;	/* set named token present	*/

			/* read named token from received message	*/
  if (asn1r_get_identifier (aCtx, req_info->named_token))
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);

  ASN1R_TAG_ADD (aCtx, CTX, 	 2, takectrl_get_priority     );
  ASN1R_TAG_ADD (aCtx, CTX, 	 3, takectrl_get_acc_delay    );
  ASN1R_TAG_ADD (aCtx, CTX, 	 4, takectrl_get_ctrl_timeout  );
  ASN1R_TAG_ADD (aCtx, CTX, 	 5, takectrl_get_abrt_timeout);
  ASN1R_TAG_ADD (aCtx, CTX, 	 6, takectrl_get_rel_conn_lost);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 7, takectrl_app_preempt_cstr );
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  ar_start = aCtx->asn1r_field_ptr;
  }

/************************************************************************/
/*			takectrl_get_priority				*/
/* Take control request: decode the priority				*/
/************************************************************************/

static ST_VOID takectrl_get_priority (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("takectrl_get_priorit");

/* read the priority from the received message				*/
  if (asn1r_get_u8 (aCtx, &(req_info->priority)))
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);

  ASN1R_TAG_ADD (aCtx, CTX, 	 3, takectrl_get_acc_delay    );
  ASN1R_TAG_ADD (aCtx, CTX, 	 4, takectrl_get_ctrl_timeout  );
  ASN1R_TAG_ADD (aCtx, CTX, 	 5, takectrl_get_abrt_timeout);
  ASN1R_TAG_ADD (aCtx, CTX, 	 6, takectrl_get_rel_conn_lost);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 7, takectrl_app_preempt_cstr );
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  ar_start = aCtx->asn1r_field_ptr;
  }

/************************************************************************/
/*			takectrl_get_acc_delay				*/
/* Take control request: decode acceptable delay			*/
/************************************************************************/

static ST_VOID takectrl_get_acc_delay (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("takectrl_get_acc_dela");

  req_info->acc_delay_pres = SD_TRUE; 	/* set acceptable delay present */

				/* read acceptable delay from message	*/
  if (asn1r_get_u32 (aCtx, &(req_info->acc_delay)))
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);

  ASN1R_TAG_ADD (aCtx, CTX, 	 4, takectrl_get_ctrl_timeout  );
  ASN1R_TAG_ADD (aCtx, CTX, 	 5, takectrl_get_abrt_timeout);
  ASN1R_TAG_ADD (aCtx, CTX, 	 6, takectrl_get_rel_conn_lost);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 7, takectrl_app_preempt_cstr );
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  ar_start = aCtx->asn1r_field_ptr;
  }

/************************************************************************/
/*			takectrl_get_ctrl_timeout			*/
/* Take control request: decode the relinquish timeout			*/
/************************************************************************/

static ST_VOID takectrl_get_ctrl_timeout (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("takectrl_get_ctrl_timeou");

  req_info->ctrl_timeout_pres = SD_TRUE;	/* set relinquish timeout pres	*/

				/* read relinquish timeout from message */
  if (asn1r_get_u32 (aCtx, &(req_info->ctrl_timeout)))
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);

  ASN1R_TAG_ADD (aCtx, CTX, 	 5, takectrl_get_abrt_timeout);
  ASN1R_TAG_ADD (aCtx, CTX, 	 6, takectrl_get_rel_conn_lost);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 7, takectrl_app_preempt_cstr );
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  ar_start = aCtx->asn1r_field_ptr;
  }

/************************************************************************/
/*			takectrl_get_abrt_timeout			*/
/* Take control request: decode the relinquish if connection lost	*/
/************************************************************************/

static ST_VOID takectrl_get_abrt_timeout (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("takectrl_get_abrt_timeout");
					/* read access key from message */
  req_info->abrt_on_timeout_pres = SD_TRUE;
  if (asn1r_get_bool (aCtx, &(req_info->abrt_on_timeout)))
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);

  ASN1R_TAG_ADD (aCtx, CTX, 	 6, takectrl_get_rel_conn_lost);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 7, takectrl_app_preempt_cstr );
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  ar_start = aCtx->asn1r_field_ptr;
  }

/************************************************************************/
/*			takectrl_get_rel_conn_lost			*/
/* Take control request: decode the relinquish if connection lost	*/
/************************************************************************/

static ST_VOID takectrl_get_rel_conn_lost (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("takectrl_get_rel_conn_lost");
					/* read access key from message */
  if (asn1r_get_bool (aCtx, &(req_info->rel_conn_lost)))
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);

  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 7, takectrl_app_preempt_cstr );
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  ar_start = aCtx->asn1r_field_ptr;
  }

/************************************************************************/
/*			takectrl_app_preempt_cstr			*/
/* Decoding take control request: application to preempt constr was	*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID takectrl_app_preempt_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("takectrl_app_preempt_cstr");

  req_info->app_preempt_pres = SD_TRUE;	/* set app preempt present	*/
  req_info->app_preempt = ar_start;		/* save_start		*/
  *ar_start = '\x30';				/* change tag		*/

  if (aCtx->asn1r_elmnt_len)
    asn1r_parse_cstr_contents (aCtx, app_preempt_cstr_done);
  else
    aCtx->asn1r_c_done_fun[ aCtx->asn1r_msg_level ] = app_preempt_cstr_done;
  }

/************************************************************************/
/*			app_preempt_cstr_done				*/
/* Decoding take control request: application preempt was encountered.	*/
/************************************************************************/

static ST_VOID app_preempt_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("app_preempt_cstr_done");

  req_info->app_len = (int) (aCtx->asn1r_field_ptr - ar_start);
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_takectrl_resp				*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE and send the take control response.		*/
/************************************************************************/

ST_RET mp_takectrl_resp (MMSREQ_IND *indptr, TAKECTRL_RESP_INFO *info_ptr)
  {
  return (_mms_resp_send (MMSOP_TAKE_CONTROL,indptr,
			M_CAST_MK_FUN (_ms_mk_takectrl_resp),
			(ST_CHAR *)info_ptr));
  }

#else
/************************************************************************/
/************************************************************************/
/*			mpl_takectrl_resp				*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the take control response.				*/
/************************************************************************/

ST_RET mpl_takectrl_resp (ST_UINT32 invoke_id, 
	TAKECTRL_RESP_INFO *info_ptr)
  {
  return (_mms_resp_fin (MMSOP_TAKE_CONTROL,invoke_id,
			M_CAST_MK_FUN (_ms_mk_takectrl_resp),
			(ST_CHAR *)info_ptr));
  }
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_takectrl_resp				*/
/* Construct a take control response.					*/
/************************************************************************/

ST_VOID _ms_mk_takectrl_resp (ASN1_ENC_CTXT *aCtx, TAKECTRL_RESP_INFO *info)
  {
  if (info->resp_tag == SD_FALSE)
    {					/* response is NULL		*/
    asn1r_fin_prim (aCtx, 0,CTX);
    }
  else if (info->resp_tag == SD_TRUE)
    {					/* response is a named token	*/
    asn1r_wr_vstr (aCtx, info->named_token);	/* write the named token	*/
    asn1r_fin_prim (aCtx, 1,CTX);
    }

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_constr (aCtx, MMSOP_TAKE_CONTROL,CTX,DEF);	/* tag = opcode, ctx constr	*/
  }
