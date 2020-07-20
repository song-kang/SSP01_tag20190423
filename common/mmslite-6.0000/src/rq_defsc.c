/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rq_defscat.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the requester portion of define scattered	*/
/*	access.  It decodes the define scattered access response	*/
/*	(confirm) & encodes the define scattered access request.    	*/
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
#include "mms_pvar.h"
#include "asn1defs.h"

/************************************************************************/
/* variables global to the DECODE portion : NONE			*/

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/


/************************************************************************/
/************************************************************************/
/*			  mms_def_scat_rsp			        */
/* NOTE : Since the mms_def_scat_rsp is a NULL PDU, the decode is	*/
/* continued by '_mms_null_pdu_dec', which simply sets decode done = 1 	*/
/* and sets the decode done function to point to the general decode done*/
/* fun.  See mmsdec.c for the general functions. 			*/
/************************************************************************/


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_defscat					*/
/* Create and send a define scattered access request. 			*/
/************************************************************************/

MMSREQ_PEND *mp_defscat (ST_INT chan, DEFSCAT_REQ_INFO *info_ptr)
  {
  return (_mms_req_send (MMSOP_DEF_SCAT,
			M_CAST_MK_FUN (_ms_mk_defscat_req),
			chan,(ST_CHAR *)info_ptr));
  } 

#else
/************************************************************************/
/************************************************************************/
/*			mpl_defscat					*/
/* Create a define scattered access request. 				*/
/************************************************************************/

ST_RET  mpl_defscat (DEFSCAT_REQ_INFO *info_ptr)
  {
  return (_mms_req_fin (MMSOP_DEF_SCAT,
			M_CAST_MK_FUN (_ms_mk_defscat_req),
			(ST_CHAR *)info_ptr));
  } 
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_defscat_req					*/
/* construct a define scattered access request				*/
/************************************************************************/

ST_VOID _ms_mk_defscat_req (ASN1_ENC_CTXT *aCtx, DEFSCAT_REQ_INFO *info_ptr)
  {
  asn1r_strt_constr (aCtx);			/* start context tag 1 cstr	*/
  _ms_wr_va_scat_access (aCtx, &info_ptr->sa_descr);
  asn1r_fin_constr (aCtx, 1, CTX, DEF);		/* finish context tag 1 cstr	*/

  asn1r_strt_constr (aCtx);			/* start context tag 0 cstr	*/
  _ms_wr_mms_objname (aCtx, &info_ptr->sa_name);
  asn1r_fin_constr (aCtx, 0, CTX, DEF);		/* finish context tag 0 cstr	*/

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_constr (aCtx, MMSOP_DEF_SCAT,CTX,DEF); 	/* tag = opcode, ctx constr	*/
  }
