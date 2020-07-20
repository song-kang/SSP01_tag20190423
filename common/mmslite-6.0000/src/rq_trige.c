/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rq_trige.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the requester portion of trigger event.	*/
/*	It decodes the trigger event response (confirm) & encodes the	*/
/*	trigger event request.						*/
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
/*			mms_trige_rsp					*/
/* NOTE : Since the mms_trige_rsp is a NULL PDU, the decode is  	*/
/* continued by '_mms_null_pdu_dec', which simply sets decode done = 1 	*/
/* and sets the decode done function to point to the general decode done*/
/* fun.  See mmsdec.c for the general functions. 			*/
/************************************************************************/


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_trige					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE and send the trigger event request.		*/
/************************************************************************/

MMSREQ_PEND *mp_trige (ST_INT chan, TRIGE_REQ_INFO *info_ptr)
  {
  return (_mms_req_send (MMSOP_TRIGGER_EV,
			M_CAST_MK_FUN (_ms_mk_trige_req),
			chan,(ST_CHAR *)info_ptr));
  } 

#else
/************************************************************************/
/************************************************************************/
/*			mpl_trige					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the trigger event request.				*/
/************************************************************************/

ST_RET  mpl_trige (TRIGE_REQ_INFO *info_ptr)
  {
  return (_mms_req_fin (MMSOP_TRIGGER_EV,
			M_CAST_MK_FUN (_ms_mk_trige_req),
			(ST_CHAR *)info_ptr));
  } 
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_trige_req					*/
/* ENCODE a trigger event REQUEST:					*/
/************************************************************************/

ST_VOID _ms_mk_trige_req (ASN1_ENC_CTXT *aCtx, TRIGE_REQ_INFO *info)
  {
  if (info->priority_pres)
    {			   
    asn1r_wr_u8 (aCtx, info->priority); 		/* write the priority		*/
    asn1r_fin_prim (aCtx, 1, CTX);			/* ctx tag 1			*/
    }

  asn1r_strt_constr (aCtx);			/* start ctx tag 0 constructor	*/
  _ms_wr_mms_objname (aCtx, &info->evcon_name);	/* write event cond mon name	*/
  asn1r_fin_constr (aCtx, 0,CTX,DEF);		/* context specific tag 0	*/

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_constr (aCtx, MMSOP_TRIGGER_EV,CTX,DEF);   	/* tag = opcode, ctx constr	*/
  }
