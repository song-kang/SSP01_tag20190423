/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rs_getas.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the responder portion of get alarm 	*/
/*	summary.  It decodes the get alarm summary request (indication) */
/*	& encodes the get alarm summary response.			*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 03/11/04  GLB     07    Remove "thisFileName"                    	*/
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
#include "mms_pevn.h"
#include "asn1defs.h"

/************************************************************************/
/* variables global to the DECODE portion :				*/

static  GETAS_REQ_INFO	*req_info;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

static ST_VOID getas_get_enrolls_only     (ASN1_DEC_CTXT *aCtx);
static ST_VOID getas_get_act_alarms_only  (ASN1_DEC_CTXT *aCtx);
static ST_VOID getas_get_ack_filter       (ASN1_DEC_CTXT *aCtx);
static ST_VOID getas_sev_filter_cstr      (ASN1_DEC_CTXT *aCtx);
static ST_VOID getas_get_most_sev         (ASN1_DEC_CTXT *aCtx);
static ST_VOID getas_get_least_sev        (ASN1_DEC_CTXT *aCtx);
static ST_VOID getas_sev_filter_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID getas_ca_name_cstr         (ASN1_DEC_CTXT *aCtx);
static ST_VOID getas_ca_name_done         (ASN1_DEC_CTXT *aCtx);


/************************************************************************/
/************************************************************************/
/*			mms_getas_req					*/
/* This function is called from MMSDEC when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_getas_req (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("Get Alarm Summary Request");


/* get storage for request information	*/
  req_info = (GETAS_REQ_INFO *) _m_get_dec_buf (aCtx, sizeof (GETAS_REQ_INFO));
  req_info->enroll_only = SD_TRUE; 	/* set default values		*/
  req_info->act_alarms_only = SD_TRUE;

/*      Done by calloc		   */
/*  req_info->ack_filter = 0;	   */
/*  req_info->most_sev_filter = 0; */
  req_info->least_sev_filter  = 127;

  ASN1R_TAG_ADD (aCtx, CTX, 		0, getas_get_enrolls_only);
  ASN1R_TAG_ADD (aCtx, CTX, 	 	1, getas_get_act_alarms_only);
  ASN1R_TAG_ADD (aCtx, CTX, 	 	2, getas_get_ack_filter);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 3, getas_sev_filter_cstr);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 5, getas_ca_name_cstr);
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }			    

/************************************************************************/
/*			getas_get_enrolls_only				*/
/* get alarm summary REQUEST (indication) is being decoded:		*/
/* enrollments only was encountered.					*/
/************************************************************************/

static ST_VOID getas_get_enrolls_only (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("getas_get_enrolls_only");

  if (asn1r_get_bool (aCtx, &req_info->enroll_only)) 	/* get enrollments only */
    {
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);
    return;
    }

  ASN1R_TAG_ADD (aCtx, CTX, 	 	1, getas_get_act_alarms_only);
  ASN1R_TAG_ADD (aCtx, CTX, 	 	2, getas_get_ack_filter);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 3, getas_sev_filter_cstr);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 5, getas_ca_name_cstr);
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }

/************************************************************************/
/*			getas_get_act_alarms_only			*/
/* get alarm summary REQUEST is being decoded:	active enrollments only */
/* was encountered.							*/
/************************************************************************/

static ST_VOID getas_get_act_alarms_only (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("getas_get_act_alarms_only");

  if (asn1r_get_bool (aCtx, &req_info->act_alarms_only))	/* active enrollments	*/
    {
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);
    return;
    }

  ASN1R_TAG_ADD (aCtx, CTX, 	 	2, getas_get_ack_filter);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 3, getas_sev_filter_cstr);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 5, getas_ca_name_cstr);
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }

/************************************************************************/
/*			getas_get_ack_filter				*/
/* get alarm summary REQUEST is being decoded: ack filter was		*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID getas_get_ack_filter (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("getas_get_ack_filter");

  if (asn1r_get_i16 (aCtx, &req_info->ack_filter)) 	/* get ack filter	*/
    {
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);
    return;
    }
  else if ((req_info->ack_filter < 0) || (req_info->ack_filter > 2))
    {
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);
    return;
    }

  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 3, getas_sev_filter_cstr);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 5, getas_ca_name_cstr);
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }

/************************************************************************/
/*			getas_sev_filter_cstr				*/
/* get alarm summary REQUEST is being decoded: severity filter cstr was */
/* encountered. 							*/
/************************************************************************/

static ST_VOID getas_sev_filter_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("getas_sev_filter_cstr");

  ASN1R_TAG_ADD (aCtx, CTX, 0, getas_get_most_sev);
  ASN1R_TAG_ADD (aCtx, CTX, 1, getas_get_least_sev);

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = getas_sev_filter_cstr_done;
  }

/************************************************************************/
/*			getas_get_most_sev				*/
/* get alarm summary REQUEST is being decoded: most severe filter was	*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID getas_get_most_sev (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("getas_get_most_sev");

  if (asn1r_get_u8 (aCtx, &req_info->most_sev_filter))	/* most severe filter	*/
    {
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);
    return;
    }

  ASN1R_TAG_ADD (aCtx, CTX, 1, getas_get_least_sev);
  }

/************************************************************************/
/*			getas_get_least_sev				*/
/* get alarm summary REQUEST is being decoded: least severe filter was	*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID getas_get_least_sev (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("getas_get_least_sev");

  if (asn1r_get_u8 (aCtx, &req_info->least_sev_filter))	/* least severe filter	*/
    {
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);
    return;
    }
  }

/************************************************************************/
/*			getas_sev_filter_cstr_done			*/
/* get alarm summary REQUEST is being decoded: severity filter cstr	*/
/* done was encountered.						*/
/************************************************************************/

static ST_VOID getas_sev_filter_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 5, getas_ca_name_cstr);
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }

/************************************************************************/
/*			getas_ca_name_cstr				*/
/* get alarm summary REQUEST is being decoded: continue after name	*/
/* constructor was encountered. 					*/
/************************************************************************/

static ST_VOID getas_ca_name_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("getas_ca_name_cstr");

  req_info->ca_pres = SD_TRUE;
  _ms_get_mms_objname (aCtx, &req_info->ca_name, getas_ca_name_done);
  }

/************************************************************************/
/*			getas_ca_name_done 				*/
/* get alarm summary RESPONSE (confirm) is being decoded:		*/
/* continue after name has been obtained.				*/
/************************************************************************/

static ST_VOID getas_ca_name_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("getas_ca_name_done");

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = NULL;	/* clear cstr done functions	*/
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_getas_resp					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE and send the get alarm summary response.		*/
/************************************************************************/
 
ST_RET mp_getas_resp (MMSREQ_IND *indptr, GETAS_RESP_INFO *info_ptr)
  {
  return (_mms_resp_send (MMSOP_GET_ALARM_SUM,indptr,
			M_CAST_MK_FUN (_ms_mk_getas_resp),
			(ST_CHAR *)info_ptr));
  }

#else
/************************************************************************/
/************************************************************************/
/*			mpl_getas_resp					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the get alarm summary response.			*/
/************************************************************************/
 
ST_RET mpl_getas_resp (ST_UINT32 invoke_id, GETAS_RESP_INFO *info_ptr)
  {
  return (_mms_resp_fin (MMSOP_GET_ALARM_SUM,invoke_id,
			M_CAST_MK_FUN (_ms_mk_getas_resp),
			(ST_CHAR *)info_ptr));
  }
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_getas_resp					*/
/* ENCODE a get alarm summary RESPONSE: 				*/
/************************************************************************/

ST_VOID _ms_mk_getas_resp (ASN1_ENC_CTXT *aCtx, GETAS_RESP_INFO *info)
  {
ST_INT	 i;
ALARM_SUMMARY *as_ptr;

  if (info->more_follows)
    {					/* not the default		*/
    asn1r_wr_bool (aCtx, info->more_follows); 	/* write more follows indicator */
    asn1r_fin_prim (aCtx, 1, CTX);			/* ctx tag 1			*/
    }

  asn1r_strt_constr (aCtx);			/* start ctx tag 0 constructor	*/
  as_ptr  = (ALARM_SUMMARY *) (info + 1);
  as_ptr += (info->num_of_alarm_sum-1); /* point to last alarm summary	*/
  for (i = 0; i < info->num_of_alarm_sum; i++)
    {
    asn1r_strt_constr (aCtx);			/* start universal seq cstr	*/
    if (as_ptr->tti_time_pres)
      { 				/* write tran to idle time	*/
      asn1r_strt_constr (aCtx);			/* start ctx tag 6 constructor	*/
      _ms_wr_mms_evtime (aCtx, &as_ptr->tti_time);
      asn1r_fin_constr (aCtx, 6,CTX,DEF);		/* finish ctx tag 6 constructor */
      }
    if (as_ptr->tta_time_pres)
      { 				/* write tran to active time	*/
      asn1r_strt_constr (aCtx);			/* start ctx tag 5 constructor	*/
      _ms_wr_mms_evtime (aCtx, &as_ptr->tta_time);
      asn1r_fin_constr (aCtx, 5,CTX,DEF);		/* finish ctx tag 5 constructor */
      }
#ifdef CS_SUPPORT
    if (as_ptr->addl_detail_pres)
      { 				/* write additional detail	*/
      asn1r_strt_constr (aCtx);			/* start ctx tag 4 constructor	*/
      asn1r_wr_delmnt (aCtx, as_ptr->addl_detail,as_ptr->addl_detail_len);
      asn1r_fin_constr (aCtx, 4,CTX,DEF);		/* finish ctx tag 4 constructor */
      }
#endif
    asn1r_wr_i16 (aCtx, as_ptr->unack_state);
    asn1r_fin_prim (aCtx, 3, CTX);

    asn1r_wr_i16 (aCtx, as_ptr->cur_state);
    asn1r_fin_prim (aCtx, 2, CTX);

    asn1r_wr_u8 (aCtx, as_ptr->severity);
    asn1r_fin_prim (aCtx, 1, CTX);

    asn1r_strt_constr (aCtx);			/* start ctx tag 0 constructor	*/
    _ms_wr_mms_objname (aCtx, &as_ptr->evcon_name);
    asn1r_fin_constr (aCtx, 0,CTX,DEF);		/* finish ctx tag 0 constructor */

    asn1r_fin_constr (aCtx, SEQ_CODE,UNI,DEF);	/* finish universal seq cstr	*/
    as_ptr--;
    }
  asn1r_fin_constr (aCtx, 0,CTX,DEF);		/* ctx tag 0 constructor	*/

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_constr (aCtx, MMSOP_GET_ALARM_SUM,CTX,DEF);	/* tag = opcode, ctx constr	*/
  }
