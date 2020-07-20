/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rq_delec.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the requester portion of delete event	*/
/*	condition.  It decodes the delete event condition response	*/
/*	(confirm) & encodes the delete event condition request.		*/
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

static  DELEC_RESP_INFO	*rsp_info;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/


/************************************************************************/
/************************************************************************/
/*			mms_delec_rsp 					*/
/* This function is called from mmsdec when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_delec_rsp (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("Decoding : Delete Event Condition Response");

/* get storage for request information	*/
  rsp_info = (DELEC_RESP_INFO *) _m_get_dec_buf (aCtx, sizeof (DELEC_RESP_INFO));

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
/*			mp_delec					*/
/* Create and send a delete event condition request.    		*/
/************************************************************************/

MMSREQ_PEND *mp_delec (ST_INT chan, DELEC_REQ_INFO *info_ptr)
  {
  return (_mms_req_send (MMSOP_DEL_EC,
			M_CAST_MK_FUN (_ms_mk_delec_req),
			chan,(ST_CHAR *)info_ptr));
  } 

#else
/************************************************************************/
/************************************************************************/
/*			mpl_delec					*/
/* Create a delete event condition request.    				*/
/************************************************************************/

ST_RET  mpl_delec (DELEC_REQ_INFO *info_ptr)
  {
  return (_mms_req_fin (MMSOP_DEL_EC,
			M_CAST_MK_FUN (_ms_mk_delec_req),
			(ST_CHAR *)info_ptr));
  } 
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_delec_req					*/
/* construct a delete event condition request 	 			*/
/************************************************************************/

ST_VOID _ms_mk_delec_req (ASN1_ENC_CTXT *aCtx, DELEC_REQ_INFO *info_ptr)
  {
ST_INT	i;
OBJECT_NAME *obj_ptr;

  switch (info_ptr->req_tag)
    {
    case 3 :
      asn1r_fin_prim (aCtx, 3, CTX);		/* ctx tag 3, vmd is null	*/
    break;

    case 2 :
        asn1r_wr_vstr (aCtx, info_ptr->dname); 	/* write the domain name	*/
        asn1r_fin_prim (aCtx, 2,CTX);	   	/* context tag 2		*/
    break;

    case 1 : 
      asn1r_fin_prim (aCtx, 1, CTX);		/* ctx tag 1, aa spec is null	*/
    break;

    case 0 :
      asn1r_strt_constr (aCtx);			/* start context tag 0 cstr	*/
      obj_ptr  = (OBJECT_NAME *) (info_ptr + 1);
      obj_ptr +=info_ptr->num_of_names-1;/* point to last object name	*/
      for (i = 0; i < info_ptr->num_of_names; i++)
        {
        _ms_wr_mms_objname (aCtx, obj_ptr);
        obj_ptr--;
        }
      asn1r_fin_constr (aCtx, 0,CTX,DEF);		/* finish context tag 0 cstr	*/
    break;
    }

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_constr (aCtx, MMSOP_DEL_EC,CTX,DEF); 		/* tag = opcode, ctx constr	*/
  }
