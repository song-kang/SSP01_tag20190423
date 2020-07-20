/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rs_ident.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the responder portion of the Identify	*/
/*	service.  It decodes the Identify request (indication) and	*/
/*	encodes the Identify response.					*/
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
#include "mms_pvmd.h"
#include "asn1defs.h"

/************************************************************************/
/* variables global to the DECODE portion : NONE			*/

/************************************************************************/
/* variables global to the ENCODE portion :				*/
/*			IDENT_RESP_INFO mmse_ident_info; 	*/


/************************************************************************/
/************************************************************************/
/*			mms_identify_req				*/
/* NOTE : Since the identify req is a NULL PDU, the decode is continued	*/
/* by '_mms_null_pdu_dec', which simply sets decode done = 1 and sets	*/
/* the decode done function to point to the general decode done fun	*/
/* See mmsdec.c for the general functions.				*/
/************************************************************************/


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_ident_resp					*/
/* This function is called from the user to ENCODE and send the 	*/
/* Identify response PDU.						*/
/************************************************************************/

ST_RET mp_ident_resp (MMSREQ_IND *indptr, IDENT_RESP_INFO *info_ptr)
  {
  return (_mms_resp_send (MMSOP_IDENTIFY,indptr,
			M_CAST_MK_FUN (_ms_mk_ident_resp),
			(ST_CHAR *)info_ptr));
  }

#else
/************************************************************************/
/************************************************************************/
/*			mpl_ident_resp					*/
/* This function is called from the user to ENCODE the Identify		*/
/* response PDU.							*/
/************************************************************************/

ST_RET mpl_ident_resp (ST_UINT32 invoke_id, IDENT_RESP_INFO *info_ptr)
  {
  return (_mms_resp_fin (MMSOP_IDENTIFY,invoke_id,
			M_CAST_MK_FUN (_ms_mk_ident_resp),
			(ST_CHAR *)info_ptr));
  }
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_ident_resp					*/
/* ENCODE an Identify RESPONSE: 					*/
/************************************************************************/

ST_VOID _ms_mk_ident_resp (ASN1_ENC_CTXT *aCtx, IDENT_RESP_INFO *info)
  {
ST_INT i;
struct mms_obj_id *obj;

  asn1r_strt_constr (aCtx);

  if (info->num_as)			/* if user wants to send AS	*/
    {
    asn1r_strt_constr (aCtx);			    	/* sequence 		*/
    obj = (struct mms_obj_id *) (info +1);	/* point to first AS	*/

    for (i = 0; i < info->num_as; ++i, ++obj)
      {
      asn1r_wr_objid (aCtx, obj->comps, obj->num_comps);
      asn1r_fin_prim (aCtx, OBJ_ID_CODE,UNI);
      }
    asn1r_fin_constr (aCtx, 3,CTX,DEF);	 	/* finish sequence of AS	*/
    }

  asn1r_wr_vstr (aCtx, info->rev);			/* write the revision name	*/
  asn1r_fin_prim (aCtx, 2,CTX);

  asn1r_wr_vstr (aCtx, info->model);		/* write the model name 	*/
  asn1r_fin_prim (aCtx, 1,CTX);

  asn1r_wr_vstr (aCtx, info->vend); 		/* write the vendor name	*/
  asn1r_fin_prim (aCtx, 0,CTX);

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_constr (aCtx, MMSOP_IDENTIFY,CTX,DEF);	 /* tag = opcode, context	*/
  }
