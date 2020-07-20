/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rq_relctrl.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the requester portion of relinquish	*/
/*	semaphore control.  It decodes the relinquish control response	*/
/*	(confirm) & encodes the relinguish control request.		*/
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
#include "mms_psem.h"
#include "asn1defs.h"

/************************************************************************/
/* variables global to the DECODE portion : NONE			*/

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

/************************************************************************/
/************************************************************************/
/*			mms_relctrl_rsp 				*/
/* NOTE : Since the mms_relctrl_rsp is a NULL PDU, the decode is	*/
/* continued by '_mms_null_pdu_dec', which simply sets decode done = 1 	*/
/* and sets the decode done function to point to the general decode done*/
/* fun.  See mmsdec.c for the general functions. 			*/
/************************************************************************/


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_relctrl					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE and send the relinquish control request.		*/
/************************************************************************/

MMSREQ_PEND *mp_relctrl (ST_INT chan, RELCTRL_REQ_INFO *info_ptr)
  {
  return (_mms_req_send (MMSOP_REL_CONTROL,
			M_CAST_MK_FUN (_ms_mk_relctrl_req),
			chan,(ST_CHAR *)info_ptr));
  } 

#else
/************************************************************************/
/************************************************************************/
/*			mpl_relctrl					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the relinquish control request.			*/
/************************************************************************/

ST_RET  mpl_relctrl (RELCTRL_REQ_INFO *info_ptr)
  {
  return (_mms_req_fin (MMSOP_REL_CONTROL,
			M_CAST_MK_FUN (_ms_mk_relctrl_req),
			(ST_CHAR *)info_ptr));
  } 
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_relctrl_req					*/
/* construct a relinquish control request				*/
/************************************************************************/

ST_VOID _ms_mk_relctrl_req (ASN1_ENC_CTXT *aCtx, RELCTRL_REQ_INFO *info)
  {

  asn1r_strt_constr (aCtx);			/* start sequence constr	*/

  if (info->named_token_pres)
    {
    asn1r_wr_vstr (aCtx, info->named_token);	/* write the named token	*/
    asn1r_fin_prim (aCtx, 1, CTX);			/* context tag 1		*/
    }

  asn1r_strt_constr (aCtx);			/* object name constructor	*/

  _ms_wr_mms_objname (aCtx, &info->sem_name);	/* write the object name	*/

  asn1r_fin_constr (aCtx, 0,CTX,DEF);		/* context tag 0		*/

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_constr (aCtx, MMSOP_REL_CONTROL,CTX,DEF);	/* tag = opcode, ctx constr	*/
  }
