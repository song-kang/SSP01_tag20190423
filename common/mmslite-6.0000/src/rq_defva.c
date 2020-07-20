/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rq_defvar.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the requester portion of define named	*/
/*	variable.  It decodes the define named variable response	*/
/*	(confirm) & encodes the define named variable request.		*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*	ST_VOID	     mms_def_var_rsp () 				*/
/*	MMSREQ_PEND *mp_defvar (chan,info_ptr)	 			*/
/*	ST_INT16        mpl_defvar (info_ptr)	- MMSEASE-LITE		*/
/*	ST_VOID	     _ms_mk_defvar_req (info)				*/
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
/* variables global to the ENCODE portion : NONE			*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_defvar					*/
/* Create and send a define named variable request.			*/
/************************************************************************/

MMSREQ_PEND *mp_defvar (ST_INT chan, DEFVAR_REQ_INFO *info_ptr)
  {
  return (_mms_req_send (MMSOP_DEF_VAR,
			M_CAST_MK_FUN (_ms_mk_defvar_req),
			chan,(ST_CHAR *)info_ptr));
  } 

#else
/************************************************************************/
/************************************************************************/
/*			mpl_defvar					*/
/* Create a define named variable request.				*/
/************************************************************************/

ST_RET  mpl_defvar (DEFVAR_REQ_INFO *info_ptr)
  {
  return (_mms_req_fin (MMSOP_DEF_VAR,
			M_CAST_MK_FUN (_ms_mk_defvar_req),
			(ST_CHAR *)info_ptr));
  } 
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_defvar_req					*/
/* construct a define named variable request				*/
/************************************************************************/

ST_VOID _ms_mk_defvar_req (ASN1_ENC_CTXT *aCtx, DEFVAR_REQ_INFO *info_ptr)
  {
  if (info_ptr->type_spec_pres)
    {
    asn1r_strt_constr (aCtx);			/* start context tag 2 cstr	*/
    _ms_wr_va_type_spec (aCtx, &info_ptr->type_spec);
    asn1r_fin_constr (aCtx, 2, CTX, DEF);		/* finish context tag 2 cstr	*/
    }

  asn1r_strt_constr (aCtx);			/* start context tag 1 cstr	*/
  _ms_wr_va_address (aCtx, &info_ptr->address);
  asn1r_fin_constr (aCtx, 1, CTX, DEF);		/* finish context tag 1 cstr	*/

  asn1r_strt_constr (aCtx);			/* start context tag 0 cstr	*/
  _ms_wr_mms_objname (aCtx, &info_ptr->name);
  asn1r_fin_constr (aCtx, 0, CTX, DEF);		/* finish context tag 0 cstr	*/

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_constr (aCtx, MMSOP_DEF_VAR,CTX,DEF); 	/* tag = opcode, ctx constr	*/
  }
