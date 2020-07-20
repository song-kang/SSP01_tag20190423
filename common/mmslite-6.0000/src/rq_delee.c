/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rq_delee.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the requester portion of delete event	*/
/*	enrollment.  It decodes the delete event enrollment response	*/
/*	(confirm) & encodes the delete event enrollment request.	*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 03/11/04  GLB     06    Remove "thisFileName"                    	*/
/* 12/20/01  JRB     05    Converted to use ASN1R (re-entrant ASN1)	*/
/* 09/13/99  MDE     04    Added SD_CONST modifiers			*/
/* 03/23/99  MDE     03    Changes to decode buffer allocation scheme	*/
/* 06/15/98  MDE     02    Changes to allow compile under C++		*/
/* 07/03/97  MDE     01    Op-specific info buffer handling changes	*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"

#include "mmsdefs.h"
#include "mms_pevn.h"
#include "asn1defs.h"

/************************************************************************/
/* variables global to the DECODE portion : 				*/

static  DELEE_RESP_INFO	*rsp_info;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/


/************************************************************************/
/************************************************************************/
/*			mms_delee_rsp 					*/
/* This function is called from mmsdec when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_delee_rsp (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("Delete Event Enrollment Response");

/* get storage for request information	*/
  rsp_info = (DELEE_RESP_INFO *) _m_get_dec_buf (aCtx, sizeof (DELEE_RESP_INFO));
  if (asn1r_get_u32 (aCtx, &rsp_info->cand_not_deleted))
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);
  else
    {
    _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
    }
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_delee					*/
/* Create and send a delete event enrollment request.    		*/
/************************************************************************/

MMSREQ_PEND *mp_delee (ST_INT chan, DELEE_REQ_INFO *info_ptr)
  {
  return (_mms_req_send (MMSOP_DEL_EE,
			M_CAST_MK_FUN (_ms_mk_delee_req),
			chan,(ST_CHAR *)info_ptr));
  } 

#else
/************************************************************************/
/************************************************************************/
/*			mpl_delee					*/
/* Create a delete event enrollment request.    			*/
/************************************************************************/

ST_RET  mpl_delee (DELEE_REQ_INFO *info_ptr)
  {
  return (_mms_req_fin (MMSOP_DEL_EE,
			M_CAST_MK_FUN (_ms_mk_delee_req),
			(ST_CHAR *)info_ptr));
  } 
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_delee_req					*/
/* construct a delete event enrollment request 	 			*/
/************************************************************************/

ST_VOID _ms_mk_delee_req (ASN1_ENC_CTXT *aCtx, DELEE_REQ_INFO *info_ptr)
  {
ST_INT	i;
OBJECT_NAME *obj_ptr;

  switch (info_ptr->req_tag)
    {
    case 2 :	   			/* event action			*/
      asn1r_strt_constr (aCtx);			/* start context tag 2 cstr	*/
      _ms_wr_mms_objname (aCtx, &info_ptr->sod.evact_name);
      asn1r_fin_constr (aCtx, 2,CTX,DEF);		/* finish context tag 2 cstr	*/
    break;

    case 1 : 				/* event condition		*/
      asn1r_strt_constr (aCtx);			/* start context tag 1 cstr	*/
      _ms_wr_mms_objname (aCtx, &info_ptr->sod.evcon_name);
      asn1r_fin_constr (aCtx, 1,CTX,DEF);		/* finish context tag 1 cstr	*/
    break;

    case 0 :				/* specific			*/
      asn1r_strt_constr (aCtx);			/* start context tag 0 cstr	*/
      obj_ptr  = (OBJECT_NAME *) (info_ptr + 1);
      					/* point to last object name	*/
      obj_ptr +=info_ptr->sod.num_of_names-1;
      for (i = 0; i < info_ptr->sod.num_of_names; i++)
        {
        _ms_wr_mms_objname (aCtx, obj_ptr);
        obj_ptr--;
        }
      asn1r_fin_constr (aCtx, 0,CTX,DEF);		/* finish context tag 0 cstr	*/
    break;
    }

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_constr (aCtx, MMSOP_DEL_EE,CTX,DEF); 		/* tag = opcode, ctx constr	*/
  }
