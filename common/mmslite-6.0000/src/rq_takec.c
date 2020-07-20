/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rq_takectrl.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the requester portion of take semahpore	*/
/*	control.  It decodes the take control response (confirm) &	*/
/*	encodes the take control request.				*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 03/11/04  GLB     06    Remove "thisFileName"                    	*/
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

static  TAKECTRL_RESP_INFO	*info;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

static ST_VOID takectrl_get_named_token (ASN1_DEC_CTXT *aCtx);
static ST_VOID takectrl_get_null (ASN1_DEC_CTXT *aCtx);


/************************************************************************/
/************************************************************************/
/*			mms_takectrl_rsp				*/
/* This function is called from mmsdec when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_takectrl_rsp (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("Take Control Response");

/* get storage for request information	*/
  info = (TAKECTRL_RESP_INFO *) _m_get_dec_buf (aCtx, sizeof (TAKECTRL_RESP_INFO));
  ASN1R_TAG_ADD (aCtx, CTX, 0, takectrl_get_null);
  ASN1R_TAG_ADD (aCtx, CTX, 1, takectrl_get_named_token);
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }

/************************************************************************/
/*			takectrl_get_null				*/
/* take control response is being decoded: ctx 0 was encountered.	*/
/************************************************************************/

static ST_VOID takectrl_get_null (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("takectrl_get_null");

  info->resp_tag  = SD_FALSE;		/* set tag to NULL response	*/
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }

/************************************************************************/
/*			takectrl_get_named_token			*/
/* take control response is being decoded: ctx 1 was encountered.	*/
/************************************************************************/

static ST_VOID takectrl_get_named_token (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("takectrl_get_named_token");

  info->resp_tag  = SD_TRUE;		/* set tag to named token resp	*/

  if (asn1r_get_identifier (aCtx, info->named_token))
    {					/* not a visible string 	*/
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);
    return;
    }
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_takectrl					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE and send the take control request. 		*/
/************************************************************************/

MMSREQ_PEND *mp_takectrl (ST_INT chan, TAKECTRL_REQ_INFO *info_ptr)
  {
  return (_mms_req_send (MMSOP_TAKE_CONTROL,
			M_CAST_MK_FUN (_ms_mk_takectrl_req),
			chan,(ST_CHAR *)info_ptr));
  } 

#else
/************************************************************************/
/************************************************************************/
/*			mpl_takectrl					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the take control request. 				*/
/************************************************************************/

ST_RET  mpl_takectrl (TAKECTRL_REQ_INFO *info_ptr)
  {
  return (_mms_req_fin (MMSOP_TAKE_CONTROL,
			M_CAST_MK_FUN (_ms_mk_takectrl_req),
			(ST_CHAR *)info_ptr));
  } 
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_takectrl_req 				*/
/* construct a take control request					*/
/************************************************************************/

ST_VOID _ms_mk_takectrl_req (ASN1_ENC_CTXT *aCtx, TAKECTRL_REQ_INFO *info_ptr)
  {
  if (info_ptr->app_preempt_pres)
    {
    asn1r_wr_delmnt (aCtx, info_ptr->app_preempt, info_ptr->app_len);
    *(aCtx->asn1r_field_ptr+1) = (ST_UCHAR) 0xA7;
    }

  if (!info_ptr->rel_conn_lost)
    {					/* NOT the default		*/
    asn1r_wr_bool (aCtx, info_ptr->rel_conn_lost);	/* write the rel if conn lost	*/
    asn1r_fin_prim (aCtx, 6, CTX);			/* context tag 6		*/
    }

  if (info_ptr->abrt_on_timeout_pres)
    {
    asn1r_wr_bool (aCtx, info_ptr->abrt_on_timeout);/* write the abort on timeout 	*/
    asn1r_fin_prim (aCtx, 5, CTX);			/* context tag 5		*/
    }

  if (info_ptr->ctrl_timeout_pres)
    {
    asn1r_wr_u32 (aCtx, info_ptr->ctrl_timeout); 	/* write the control timeout 	*/
    asn1r_fin_prim (aCtx, 4, CTX);			/* context tag 4		*/
    }

  if (info_ptr->acc_delay_pres)
    {
    asn1r_wr_u32 (aCtx, info_ptr->acc_delay);		/* write the acceptable delay	*/
    asn1r_fin_prim (aCtx, 3, CTX);			/* context tag 3		*/
    }

  if (info_ptr->priority != MMS_NORM_PRIORITY)
    {					/* NOT the default		*/
    asn1r_wr_bool (aCtx, info_ptr->priority);	/* write the priority		*/
    asn1r_fin_prim (aCtx, 2, CTX);			/* context tag 2		*/
    }

  if (info_ptr->named_token_pres)
    {
    asn1r_wr_vstr (aCtx, info_ptr->named_token);	/* write the named token	*/
    asn1r_fin_prim (aCtx, 1, CTX);			/* context tag 1		*/
    }

  asn1r_strt_constr (aCtx);			/* object name constructor	*/

  _ms_wr_mms_objname (aCtx, &info_ptr->sem_name);	/* write the object name	*/

  asn1r_fin_constr (aCtx, 0,CTX,DEF);		/* context tag 0		*/

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_constr (aCtx, MMSOP_TAKE_CONTROL,CTX,DEF);	/* tag = opcode, ctx constr	*/
  }
