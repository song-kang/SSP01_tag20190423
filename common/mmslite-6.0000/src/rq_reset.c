/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rq_reset.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the requester portion of the reset 	*/
/*	operation.  It decodes the reset response (confirm) & encodes	*/
/*	the reset request.						*/
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
#include "mms_pprg.h"
#include "asn1defs.h"

/************************************************************************/
/* variables global to the DECODE portion : NONE			*/

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

/************************************************************************/
/************************************************************************/
/*			mms_reset_rsp					*/
/* NOTE: Since the mms_reset_ rsp is a NULL PDU, the decode is continued*/
/* by '_mms_null_pdu_dec', which simply sets decode done = 1 and sets	*/
/* the decode done function to point to the general decode done fun	*/
/* See mmsdec.c for the general functions.				*/
/************************************************************************/


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_reset					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE and send the reset request.			*/
/************************************************************************/

MMSREQ_PEND *mp_reset (ST_INT chan, RESET_REQ_INFO *info_ptr)
  {
  return (_mms_req_send (MMSOP_RESET,
			M_CAST_MK_FUN (_ms_mk_reset_req),
			chan,(ST_CHAR *)info_ptr));
  } 

#else
/************************************************************************/
/************************************************************************/
/*			mpl_reset					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the reset request.					*/
/************************************************************************/

ST_RET  mpl_reset (RESET_REQ_INFO *info_ptr)
  {
  return (_mms_req_fin (MMSOP_RESET,
			M_CAST_MK_FUN (_ms_mk_reset_req),
			(ST_CHAR *)info_ptr));
  } 
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_reset_req					*/
/* construct a reset request						*/
/************************************************************************/

ST_VOID _ms_mk_reset_req (ASN1_ENC_CTXT *aCtx, RESET_REQ_INFO *info)
  {
  asn1r_wr_vstr (aCtx, info->piname);		/* write pgm invocation name	*/
  asn1r_fin_prim (aCtx, 0, CTX);			/* context tag 0		*/

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_constr (aCtx, MMSOP_RESET,CTX,DEF);	/* tag = opcode, ctx cstr	*/
  }
