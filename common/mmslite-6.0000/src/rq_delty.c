/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rq_deltype.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the requester portion of delete named	*/
/*	type.  It decodes the delete named type response (confirm) &	*/
/*	encodes the delete named type request.				*/
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
#include "mms_pvar.h"
#include "asn1defs.h"

/************************************************************************/
/* variables global to the DECODE portion : 				*/

static  DELTYPE_RESP_INFO	*rsp_info;


/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

static ST_VOID deltyp_num_matched (ASN1_DEC_CTXT *aCtx);
static ST_VOID deltyp_num_deleted (ASN1_DEC_CTXT *aCtx);


/************************************************************************/
/************************************************************************/
/*			mms_del_type_rsp 				*/
/* This function is called from mmsdec when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_del_type_rsp (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("Delete Type Response");

/* get storage for request information	*/
  rsp_info = (DELTYPE_RESP_INFO *) _m_get_dec_buf (aCtx, sizeof (DELTYPE_RESP_INFO));
  ASN1R_TAG_ADD (aCtx, CTX, 0, deltyp_num_matched);
  }

/************************************************************************/
/*			deltyp_num_matched 				*/
/* number matched element encountered, read into info structure.	*/
/************************************************************************/

static ST_VOID deltyp_num_matched (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("deltyp_num_matched");

  if (asn1r_get_u32 (aCtx, &rsp_info->num_matched))
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);
  else
    ASN1R_TAG_ADD (aCtx, CTX, 1, deltyp_num_deleted);
  }

/************************************************************************/
/*			deltyp_num_deleted 				*/
/* number deleted element encountered, read into info structure.	*/
/************************************************************************/

static ST_VOID deltyp_num_deleted (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("deltyp_num_delete");

  if (asn1r_get_u32 (aCtx, &rsp_info->num_deleted))
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);
  else
    {
    _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
    }
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_deltype					*/
/* Create and send a delete named type request. 			*/
/************************************************************************/

MMSREQ_PEND *mp_deltype (ST_INT chan, DELTYPE_REQ_INFO *info_ptr)
  {
  return (_mms_req_send (MMSOP_DEL_TYPE,
			M_CAST_MK_FUN (_ms_mk_deltype_req),
			chan,(ST_CHAR *)info_ptr));
  } 

#else
/************************************************************************/
/************************************************************************/
/*			mpl_deltype					*/
/* Create and send a delete named type request. 			*/
/************************************************************************/

ST_RET   mpl_deltype (DELTYPE_REQ_INFO *info_ptr)
  {
  return (_mms_req_fin (MMSOP_DEL_TYPE,
			M_CAST_MK_FUN (_ms_mk_deltype_req),
			(ST_CHAR *)info_ptr));
  } 
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_deltype_req					*/
/* construct a delete type definition request				*/
/************************************************************************/

ST_VOID _ms_mk_deltype_req (ASN1_ENC_CTXT *aCtx, DELTYPE_REQ_INFO *info_ptr)
  {
ST_INT i;
OBJECT_NAME *obj_ptr;

  if (info_ptr->dname_pres)
    {
    asn1r_wr_vstr (aCtx, info_ptr->dname);		/* write the domain name	*/
    asn1r_fin_prim (aCtx, 2,CTX);			/* context tag 2		*/
    }

  if (info_ptr->tnames_pres)
    {
    asn1r_strt_constr (aCtx);			/* start context tag 1 cstr	*/
    obj_ptr  = (OBJECT_NAME *) (info_ptr + 1);
    obj_ptr +=info_ptr->num_of_tnames-1;/* point to last type name	*/
    for (i = 0; i < info_ptr->num_of_tnames; i++)
      {
      _ms_wr_mms_objname (aCtx, obj_ptr);
      obj_ptr--;
      }
    asn1r_fin_constr (aCtx, 1,CTX,DEF);		/* finish context tag 1 cstr	*/
    }

  if (info_ptr->scope != 0)		/* default is 0-specific	*/
    {					/* not the default, so		*/
    asn1r_wr_i16 (aCtx, info_ptr->scope);		/* write the scope of delete	*/
    asn1r_fin_prim (aCtx, 0,CTX);			/* context tag 0		*/
    }

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_constr (aCtx, MMSOP_DEL_TYPE,CTX,DEF);	/* tag = opcode, ctx constr	*/
  }
