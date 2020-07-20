/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rq_start.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the requester portion of the start 	*/
/*	operation.  It decodes the start response (confirm) & encodes	*/
/*	the start request.						*/
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
/*			mms_start_resp					*/
/* NOTE : Since the start resp is a NULL PDU, the decode is continued	*/
/* by '_mms_null_pdu_dec', which simply sets decode done = 1 and sets	*/
/* the decode done function to point to the general decode done fun	*/
/* See mmsdec.c for the general functions.				*/
/************************************************************************/


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_start					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE and send the start request.			*/
/************************************************************************/

MMSREQ_PEND *mp_start (ST_INT chan, START_REQ_INFO *info_ptr)
  {
  return (_mms_req_send (MMSOP_START,
			M_CAST_MK_FUN (_ms_mk_start_req),
			chan,(ST_CHAR *)info_ptr));
  } 

#else
/************************************************************************/
/************************************************************************/
/*			mpl_start					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the start request.					*/
/************************************************************************/

ST_RET  mpl_start (START_REQ_INFO *info_ptr)
  {
  return (_mms_req_fin (MMSOP_START,
			M_CAST_MK_FUN (_ms_mk_start_req),
			(ST_CHAR *)info_ptr));
  } 
#endif
   

/************************************************************************/
/************************************************************************/
/*			_ms_mk_start_req					*/
/* construct a start request						*/
/************************************************************************/

ST_VOID _ms_mk_start_req (ASN1_ENC_CTXT *aCtx, START_REQ_INFO *info)
  {

  if (info->start_arg_pres)
    {
    if (info->start_arg_type == ARG_TYPE_SIMPLE)
      {
      asn1r_wr_vstr (aCtx, (ST_CHAR *)info->start_arg); /* write the start argument	*/
      asn1r_fin_prim (aCtx, 1, CTX);	 	 /* context tag 1		*/
      }
    else    	/* ENCODED data, write as complete ASN.1 EXTERNAL     	*/
      asn1r_wr_delmnt (aCtx, info->start_arg,info->start_arg_len);
    }

  asn1r_wr_vstr (aCtx, info->piname);		/* write pgm invocation name	*/
  asn1r_fin_prim (aCtx, 0, CTX);			/* context tag 0		*/

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_constr (aCtx, MMSOP_START,CTX,DEF);		/* tag = opcode, ctx cstr	*/
  }
