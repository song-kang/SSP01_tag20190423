/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rq_ackev.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the requester portion of the acknowledge	*/
/*	event notification operation.  It decodes the acknowledge event */
/*	notification response (confirm) & encodes the acknowledge event */
/*	notification request.						*/
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
/*			mms_ackevnot_rsp				*/
/* NOTE : Since the mms_ackevnot_type_rsp is a NULL PDU, the decode is	*/
/* continued by '_mms_null_pdu_dec', which simply sets decode done = 1 	*/
/* and sets the decode done function to point to the general decode done*/
/* fun.  See mmsdec.c for the general functions. 			*/
/************************************************************************/


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_ackevnot					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE & send the acknowledge event notification request. */
/************************************************************************/

MMSREQ_PEND *mp_ackevnot (ST_INT chan, ACKEVNOT_REQ_INFO *info_ptr)
  {
  return (_mms_req_send (MMSOP_ACK_EVENT_NOT,
			M_CAST_MK_FUN (_ms_mk_ackevnot_req),
			chan,(ST_CHAR *)info_ptr));
  } 

#else
/************************************************************************/
/************************************************************************/
/*			mpl_ackevnot					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the acknowledge event notification request. 	*/
/************************************************************************/

ST_RET  mpl_ackevnot (ACKEVNOT_REQ_INFO *info_ptr)
  {
  return (_mms_req_fin (MMSOP_ACK_EVENT_NOT,
			M_CAST_MK_FUN (_ms_mk_ackevnot_req),
			(ST_CHAR *)info_ptr));
  } 
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_ackevnot_req 				*/
/* construct an acknowledge event notification request. 		*/
/************************************************************************/

ST_VOID _ms_mk_ackevnot_req (ASN1_ENC_CTXT *aCtx, ACKEVNOT_REQ_INFO *info)
  {
     		/* check for acknowledgement event condition name	*/
  if (info->ackec_name_pres)
    {					/* ack event cond name present	*/
    asn1r_strt_constr (aCtx);			/* start context tag 1 cstr	*/
    _ms_wr_mms_objname (aCtx, &info->ackec_name);	/* write ack event cond name	*/
    asn1r_fin_constr (aCtx, 4, CTX, DEF);		/* context tag 4		*/
    }

  asn1r_strt_constr (aCtx);			/* start context tag 3 cstr	*/
  _ms_wr_mms_evtime (aCtx, &info->evtime);
  asn1r_fin_constr (aCtx, 3, CTX, DEF);		/* finish context tag 3 	*/

  asn1r_wr_i16 (aCtx, info->ack_state);		/* write ack state		*/
  asn1r_fin_prim (aCtx, 2, CTX);			/* context tag 2		*/

  asn1r_strt_constr (aCtx);			/* start context tag 0 cstr	*/
  _ms_wr_mms_objname (aCtx, &info->evenroll_name);/* write event condition name	*/
  asn1r_fin_constr (aCtx, 0, CTX, DEF);		/* context tag 0		*/

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_constr (aCtx, MMSOP_ACK_EVENT_NOT,CTX,DEF);	/* tag = opcode, ctx cstr	*/
  }
