/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rq_resume.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the requester portion of the resume	*/
/*	operation.  It decodes the resume response (confirm) & encodes	*/
/*	the resume request.						*/
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
/*			mms_resume_rsp					*/
/* NOTE: Since the mms_resume_rsp is a NULL PDU, the decode is continued*/
/* by '_mms_null_pdu_dec', which simply sets decode done = 1 and sets	*/
/* the decode done function to point to the general decode done fun	*/
/* See mmsdec.c for the general functions.				*/
/************************************************************************/


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_resume					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE and send the resume request.			*/
/************************************************************************/

MMSREQ_PEND *mp_resume (ST_INT chan, RESUME_REQ_INFO *info_ptr)
  {
  return (_mms_req_send (MMSOP_RESUME,
			M_CAST_MK_FUN (_ms_mk_resume_req),
			chan,(ST_CHAR *)info_ptr));
  } 

#else
/************************************************************************/
/************************************************************************/
/*			mpl_resume					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the resume request.				*/
/************************************************************************/

ST_RET  mpl_resume (RESUME_REQ_INFO *info_ptr)
  {
  return (_mms_req_fin (MMSOP_RESUME,
			M_CAST_MK_FUN (_ms_mk_resume_req),
			(ST_CHAR *)info_ptr));
  } 
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_resume_req					*/
/* construct a resume request						*/
/************************************************************************/

ST_VOID _ms_mk_resume_req (ASN1_ENC_CTXT *aCtx, RESUME_REQ_INFO *info)
  {
  if (info->resume_arg_pres)
    {
    if (info->resume_arg_type == ARG_TYPE_SIMPLE)
      {
      asn1r_wr_vstr (aCtx, (ST_CHAR *)info->resume_arg); /* write the resume argument	*/
      asn1r_fin_prim (aCtx, 1, CTX);	 	 /* context tag 1		*/
      }
    else    	/* ENCODED data, write as complete ASN.1 EXTERNAL     	*/
      asn1r_wr_delmnt (aCtx, info->resume_arg,info->resume_arg_len);
    }

  asn1r_wr_vstr (aCtx, info->piname);		/* write pgm invocation name	*/
  asn1r_fin_prim (aCtx, 0, CTX);			/* context tag 0		*/

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_constr (aCtx, MMSOP_RESUME,CTX,DEF);		/* tag = opcode, ctx cstr	*/
  }
