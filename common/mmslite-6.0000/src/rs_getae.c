/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rs_getaes.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the responder portion of get alarm 	*/
/*	enrollment summary.  It decodes the get alarm enrollment	*/
/*	summary request (indication) & encodes the get alarm 		*/
/*	enrollment summary response.					*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 04/21/08  JRB     08    Use asn1r_wr_bool to write aes_ptr->not_lost.*/
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

static  GETAES_REQ_INFO	*req_info;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

static ST_VOID getaes_get_enrolls_only     (ASN1_DEC_CTXT *aCtx);
static ST_VOID getaes_get_act_alarms_only  (ASN1_DEC_CTXT *aCtx);
static ST_VOID getaes_get_ack_filter       (ASN1_DEC_CTXT *aCtx);
static ST_VOID getaes_sev_filter_cstr      (ASN1_DEC_CTXT *aCtx);
static ST_VOID getaes_get_most_sev         (ASN1_DEC_CTXT *aCtx);
static ST_VOID getaes_get_least_sev        (ASN1_DEC_CTXT *aCtx);
static ST_VOID getaes_sev_filter_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID getaes_ca_name_cstr         (ASN1_DEC_CTXT *aCtx);
static ST_VOID getaes_ca_name_done         (ASN1_DEC_CTXT *aCtx);


/************************************************************************/
/************************************************************************/
/*			mms_getaes_req					*/
/* This function is called from MMSDEC when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_getaes_req (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("Get Alarm Enrollment Summary Request");


/* get storage for request information	*/
  req_info = (GETAES_REQ_INFO *) _m_get_dec_buf (aCtx, sizeof (GETAES_REQ_INFO));
  req_info->enroll_only = SD_TRUE; 	/* set default values		*/
  req_info->act_alarms_only = SD_TRUE;

/*      Done by calloc		   */
/*  req_info->ack_filter = 0;	   */
/*  req_info->most_sev_filter = 0; */
  req_info->least_sev_filter = 127;

  ASN1R_TAG_ADD (aCtx, CTX, 	 	0, getaes_get_enrolls_only);
  ASN1R_TAG_ADD (aCtx, CTX, 	 	1, getaes_get_act_alarms_only);
  ASN1R_TAG_ADD (aCtx, CTX, 	 	2, getaes_get_ack_filter);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 3, getaes_sev_filter_cstr);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 5, getaes_ca_name_cstr);
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }			    

/************************************************************************/
/*			getaes_get_enrolls_only				*/
/* get alarm enrollment summary REQUEST (indication) is being decoded:	*/
/* enrollments only was encountered.					*/
/************************************************************************/

static ST_VOID getaes_get_enrolls_only (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("getaes_get_enrolls_only");

  if (asn1r_get_bool (aCtx, &req_info->enroll_only)) 	/* get enrollments only */
    {
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);
    return;
    }

  ASN1R_TAG_ADD (aCtx, CTX, 	 	1, getaes_get_act_alarms_only);
  ASN1R_TAG_ADD (aCtx, CTX, 	 	2, getaes_get_ack_filter);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 3, getaes_sev_filter_cstr);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 5, getaes_ca_name_cstr);
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }

/************************************************************************/
/*			getaes_get_act_alarms_only			*/
/* get alarm enrollment summary REQUEST is being decoded: active 	*/
/* enrollments only was encountered.   					*/
/************************************************************************/

static ST_VOID getaes_get_act_alarms_only (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("getaes_get_act_alarms_only");

  if (asn1r_get_bool (aCtx, &req_info->act_alarms_only))	/* active enrollments	*/
    {
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);
    return;
    }

  ASN1R_TAG_ADD (aCtx, CTX, 	 	2, getaes_get_ack_filter);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 3, getaes_sev_filter_cstr);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 5, getaes_ca_name_cstr);
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }

/************************************************************************/
/*			getaes_get_ack_filter				*/
/* get alarm enrollment summary REQUEST is being decoded: ack filter	*/
/* was encountered. 							*/
/************************************************************************/

static ST_VOID getaes_get_ack_filter (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("getaes_get_ack_filter");

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

  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 3, getaes_sev_filter_cstr);
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 5, getaes_ca_name_cstr);
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }

/************************************************************************/
/*			getaes_sev_filter_cstr				*/
/* get alarm enrollment summary REQUEST is being decoded: severity	*/
/* filter constructor was encountered. 	 				*/
/************************************************************************/

static ST_VOID getaes_sev_filter_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("getaes_sev_filter_cstr");

  ASN1R_TAG_ADD (aCtx, CTX, 0, getaes_get_most_sev);
  ASN1R_TAG_ADD (aCtx, CTX, 1, getaes_get_least_sev);

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = getaes_sev_filter_cstr_done;
  }

/************************************************************************/
/*			getaes_get_most_sev				*/
/* get alarm enrollment summary REQUEST is being decoded: most severe	*/
/* filter was encountered. 						*/
/************************************************************************/

static ST_VOID getaes_get_most_sev (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("getaes_get_most_sev");

  if (asn1r_get_u8 (aCtx, &req_info->most_sev_filter))	/* most severe filter	*/
    {
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);
    return;
    }

  ASN1R_TAG_ADD (aCtx, CTX, 1, getaes_get_least_sev);
  }

/************************************************************************/
/*			getaes_get_least_sev				*/
/* get alarm enrollment summary REQUEST is being decoded: least severe	*/
/* filter was encountered.   						*/
/************************************************************************/

static ST_VOID getaes_get_least_sev (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("getaes_get_least_sev");

  if (asn1r_get_u8 (aCtx, &req_info->least_sev_filter))	/* least severe filter	*/
    {
    asn1r_set_dec_err (aCtx, REQ_BAD_VALUE);
    return;
    }
  }

/************************************************************************/
/*			getaes_sev_filter_cstr_done			*/
/* get alarm enrollment summary REQUEST is being decoded: severity	*/
/* filter constructor done was encountered.  				*/
/************************************************************************/

static ST_VOID getaes_sev_filter_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 5, getaes_ca_name_cstr);
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }

/************************************************************************/
/*			getaes_ca_name_cstr				*/
/* get alarm enrollment summary REQUEST is being decoded: continue 	*/
/* after name constructor was encountered.  				*/
/************************************************************************/

static ST_VOID getaes_ca_name_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("getaes_ca_name_cstr");

  req_info->ca_name_pres = SD_TRUE;	/* continue after name present	*/

  _ms_get_mms_objname (aCtx, &req_info->ca_name, getaes_ca_name_done);
  }

/************************************************************************/
/*			getaes_ca_name_done 				*/
/* get alarm enrollment summary RESPONSE (confirm) is being decoded:	*/
/* continue after name has been obtained.				*/
/************************************************************************/

static ST_VOID getaes_ca_name_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("getaes_ca_name_done");
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_getaes_resp					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE and send the get alarm enrollment summary response	*/
/************************************************************************/
 
ST_RET mp_getaes_resp (MMSREQ_IND *indptr, GETAES_RESP_INFO *info_ptr)
  {
  return (_mms_resp_send (MMSOP_GET_ALARM_ESUM,indptr,
			M_CAST_MK_FUN (_ms_mk_getaes_resp),
			(ST_CHAR *)info_ptr));
  }

#else
/************************************************************************/
/************************************************************************/
/*			mpl_getaes_resp					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the get alarm enrollment summary response.		*/
/************************************************************************/
 
ST_RET mpl_getaes_resp (ST_UINT32 invoke_id, GETAES_RESP_INFO *info_ptr)
  {
  return (_mms_resp_fin (MMSOP_GET_ALARM_ESUM,invoke_id,
			M_CAST_MK_FUN (_ms_mk_getaes_resp),
			(ST_CHAR *)info_ptr));
  }
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_getaes_resp					*/
/* ENCODE a get alarm enrollment summary RESPONSE:  			*/
/************************************************************************/

ST_VOID _ms_mk_getaes_resp (ASN1_ENC_CTXT *aCtx, GETAES_RESP_INFO *info)
  {
ST_INT	 i;
ALARM_ENROLL_SUMMARY *aes_ptr;

  if (info->more_follows)
    {					/* not the default		*/
    asn1r_wr_bool (aCtx, info->more_follows); 	/* write more follows indicator */
    asn1r_fin_prim (aCtx, 1, CTX);			/* ctx tag 1			*/
    }

  asn1r_strt_constr (aCtx);			/* start ctx tag 0 constructor	*/
  aes_ptr  = (ALARM_ENROLL_SUMMARY *) (info + 1);
  			/* point to last alarm enrollment summary	*/
  aes_ptr += (info->num_of_alarm_esum-1);
  for (i = 0; i < info->num_of_alarm_esum; i++)
    {
    asn1r_strt_constr (aCtx);			/* start universal seq cstr	*/
    if (aes_ptr->iack_time_pres)
      { 				/* write idle acknowlegment time*/
      asn1r_strt_constr (aCtx);			/* start ctx tag 12 constructor	*/
      _ms_wr_mms_evtime (aCtx, &aes_ptr->iack_time);
      asn1r_fin_constr (aCtx, 12,CTX,DEF);		/* finish ctx tag 12 constructor */
      }
    if (aes_ptr->tti_time_pres)
      { 				/* write tran to idle time	*/
      asn1r_strt_constr (aCtx);			/* start ctx tag 11 constructor	*/
      _ms_wr_mms_evtime (aCtx, &aes_ptr->tti_time);
      asn1r_fin_constr (aCtx, 11,CTX,DEF);		/* finish ctx tag 11 constructor */
      }
    if (aes_ptr->aack_time_pres)
      { 				/* write active acknowledge time*/
      asn1r_strt_constr (aCtx);			/* start ctx tag 10 constructor	*/
      _ms_wr_mms_evtime (aCtx, &aes_ptr->aack_time);
      asn1r_fin_constr (aCtx, 10,CTX,DEF);		/* finish ctx tag 10 constructor */
      }
    if (aes_ptr->tta_time_pres)
      { 				/* write tran to active time	*/
      asn1r_strt_constr (aCtx);			/* start ctx tag 9 constructor	*/
      _ms_wr_mms_evtime (aCtx, &aes_ptr->tta_time);
      asn1r_fin_constr (aCtx, 9,CTX,DEF);		/* finish ctx tag 9 constructor */
      }
    if (aes_ptr->ee_state_pres)
      { 				/* write event enroll state	*/
      asn1r_wr_i16 (aCtx, aes_ptr->ee_state);
      asn1r_fin_prim (aCtx, 8, CTX);
      }
    asn1r_wr_i16 (aCtx, aes_ptr->alarm_ack_rule);
    asn1r_fin_prim (aCtx, 7, CTX);

    if (aes_ptr->not_lost)
      { 				/* not the default of SD_FALSE	*/
      asn1r_wr_bool (aCtx, aes_ptr->not_lost);	/* write notification lost	*/
      asn1r_fin_prim (aCtx, 6, CTX);
      }
#ifdef CS_SUPPORT
    if (aes_ptr->addl_detail_pres)
      { 				/* write additional detail	*/
      asn1r_strt_constr (aCtx);			/* start ctx tag 5 constructor	*/
      asn1r_wr_delmnt (aCtx, aes_ptr->addl_detail,aes_ptr->addl_detail_len);
      asn1r_fin_constr (aCtx, 5,CTX,DEF);		/* finish ctx tag 5 constructor */
      }
#endif
    asn1r_wr_i16 (aCtx, aes_ptr->cur_state);
    asn1r_fin_prim (aCtx, 4, CTX);

    asn1r_wr_u8 (aCtx, aes_ptr->severity);
    asn1r_fin_prim (aCtx, 3, CTX);

    if (aes_ptr->client_app_pres)
      {
      asn1r_strt_constr (aCtx);			/* start ctx tag 2 constructor	*/
      asn1r_wr_delmnt (aCtx, aes_ptr->client_app,aes_ptr->client_app_len);
      asn1r_fin_constr (aCtx, 2,CTX,DEF);		/* finish ctx tag 2 constructor */
      }
    asn1r_strt_constr (aCtx);			/* start ctx tag 0 constructor	*/
    _ms_wr_mms_objname (aCtx, &aes_ptr->evenroll_name);
    asn1r_fin_constr (aCtx, 0,CTX,DEF);		/* finish ctx tag 0 constructor */

    asn1r_fin_constr (aCtx, SEQ_CODE,UNI,DEF);	/* finish universal seq cstr	*/
    aes_ptr--;
    }
  asn1r_fin_constr (aCtx, 0,CTX,DEF);		/* ctx tag 0 constructor	*/

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_constr (aCtx, MMSOP_GET_ALARM_ESUM,CTX,DEF);	/* tag = opcode, ctx constr	*/
  }
