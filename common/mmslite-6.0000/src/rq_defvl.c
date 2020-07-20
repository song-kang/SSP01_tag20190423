/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rq_defvlist.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the requester portion of the define named	*/
/*	variable list.  It decodes the get named variable list		*/
/*	response (confirm) & encodes the get named variable list	*/
/*	request.							*/
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
/*			mms_def_vlist_rsp 				*/
/* NOTE : Since the mms_def_vlist_rsp is a NULL PDU, the decode is  	*/
/* continued by '_mms_null_pdu_dec', which simply sets decode done = 1 	*/
/* and sets the decode done function to point to the general decode done*/
/* fun.  See mmsdec.c for the general functions. 			*/
/************************************************************************/


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_defvlist					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE & send the define named variable list request.	*/
/************************************************************************/

MMSREQ_PEND *mp_defvlist (ST_INT chan, DEFVLIST_REQ_INFO *info_ptr)
  {
  return (_mms_req_send (MMSOP_DEF_VLIST,
			M_CAST_MK_FUN (_ms_mk_defvlist_req),
			chan,(ST_CHAR *)info_ptr));
  } 

#else
/************************************************************************/
/************************************************************************/
/*			mpl_defvlist					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the define named variable list request.		*/
/************************************************************************/

ST_RET  mpl_defvlist (DEFVLIST_REQ_INFO *info_ptr)
  {
  return (_mms_req_fin (MMSOP_DEF_VLIST,
			M_CAST_MK_FUN (_ms_mk_defvlist_req),
			(ST_CHAR *)info_ptr));
  } 
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_defvlist_req					*/
/* construct a define named variable list request			*/
/************************************************************************/

ST_VOID _ms_mk_defvlist_req (ASN1_ENC_CTXT *aCtx, DEFVLIST_REQ_INFO *info)
  {
VARIABLE_LIST *vl_ptr;

  					/* get ptr to list of variables	*/
  vl_ptr  = (VARIABLE_LIST *) (info + 1);
  
  asn1r_strt_constr (aCtx);			/* start context tag 0 cstr	*/
  _ms_wr_va_var_list (aCtx, vl_ptr, info->num_of_variables);
  asn1r_fin_constr (aCtx, 0, CTX, DEF);		/* finish context tag 0 cstr	*/

  _ms_wr_mms_objname (aCtx, &info->vl_name);	/* write the variable list name	*/

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_constr (aCtx, MMSOP_DEF_VLIST,CTX,DEF); 	/* tag = opcode, ctx		*/
  }
