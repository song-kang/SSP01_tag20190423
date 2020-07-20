/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rq_defee.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the requester portion of define event	*/
/*	enrollment.  It decodes the define event enrollment response	*/
/*	(confirm) & encodes the define event enrollment request. 	*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 12/20/01  JRB     02    Converted to use ASN1R (re-entrant ASN1)	*/
/* 06/15/98  MDE     01    Changes to allow compile under C++		*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"

#include "mmsdefs.h"
#include "mms_pevn.h"
#include "asn1defs.h"

/************************************************************************/
/* variables global to the DECODE portion : NONE			*/

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/
/************************************************************************/
/************************************************************************/
/*			mms_defee_rsp					*/
/* NOTE : Since the mms_defee_rsp is a NULL PDU, the decode is  	*/
/* continued by '_mms_null_pdu_dec', which simply sets decode done = 1 	*/
/* and sets the decode done function to point to the general decode done*/
/* fun.  See mmsdec.c for the general functions. 			*/
/************************************************************************/


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_defee					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE and send the define event enrollment request.   	*/
/************************************************************************/

MMSREQ_PEND *mp_defee (ST_INT chan, DEFEE_REQ_INFO *info_ptr)
  {
  return (_mms_req_send (MMSOP_DEF_EE,
			M_CAST_MK_FUN (_ms_mk_defee_req),
			chan,(ST_CHAR *)info_ptr));
  } 

#else
/************************************************************************/
/************************************************************************/
/*			mpl_defee					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the define event enrollment request.   		*/
/************************************************************************/

ST_RET  mpl_defee (DEFEE_REQ_INFO *info_ptr)
  {
  return (_mms_req_fin (MMSOP_DEF_EE,
			M_CAST_MK_FUN (_ms_mk_defee_req),
			(ST_CHAR *)info_ptr));
  } 
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_defee_req					*/
/* ENCODE a define event enrollment REQUEST:				*/
/************************************************************************/

ST_VOID _ms_mk_defee_req (ASN1_ENC_CTXT *aCtx, DEFEE_REQ_INFO *info)
  {
  if (info->ackec_name_pres)
    {					
    asn1r_strt_constr (aCtx);			/* start ctx tag 6 constructor	*/
    _ms_wr_mms_objname (aCtx, &info->ackec_name);	/* write ack event cond name	*/
    asn1r_fin_constr (aCtx, 6,CTX,DEF);		/* context specific tag 6	*/
    }

  if (info->client_app_pres)
    {
    asn1r_strt_constr (aCtx);			/* start the ctx 5 constr	*/
    asn1r_wr_delmnt (aCtx, info->client_app,info->client_app_len);
    asn1r_fin_constr (aCtx, 5,CTX,DEF);		/* finish ctx 5 constructor	*/
    }

  if (info->evact_name_pres)
    {					
    asn1r_strt_constr (aCtx);			/* start ctx tag 4 constructor	*/
    _ms_wr_mms_objname (aCtx, &info->evact_name);	/* write event action name	*/
    asn1r_fin_constr (aCtx, 4,CTX,DEF);		/* context specific tag 4	*/
    }

  asn1r_wr_i16 (aCtx, info->alarm_ack_rule);
  asn1r_fin_prim (aCtx, 3, CTX);

  asn1r_wr_bitstr (aCtx, (ST_UCHAR *)&info->ec_transitions, 7);
  asn1r_fin_prim (aCtx, 2, CTX);

  asn1r_strt_constr (aCtx);			/* start ctx tag 1 constructor	*/
  _ms_wr_mms_objname (aCtx, &info->evcon_name);	/* write event condition name	*/
  asn1r_fin_constr (aCtx, 1,CTX,DEF);		/* context specific tag 1	*/

  asn1r_strt_constr (aCtx);			/* start ctx tag 0 constructor	*/
  _ms_wr_mms_objname (aCtx, &info->evenroll_name);/* write event enrollment name	*/
  asn1r_fin_constr (aCtx, 0,CTX,DEF);		/* context specific tag 0	*/

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_constr (aCtx, MMSOP_DEF_EE,CTX,DEF);		/* tag = opcode, ctx constr	*/
  }
