/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rq_defea.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the requester portion of define event	*/
/*	action.  It decodes the define event action response (confirm)	*/
/*	& encodes the define event action request. 			*/
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
#include "mms_pevn.h"
#include "asn1defs.h"

/************************************************************************/
/* variables global to the DECODE portion : NONE			*/

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/


/************************************************************************/
/************************************************************************/
/*			mms_defea_rsp					*/
/* NOTE : Since the mms_defea_rsp is a NULL PDU, the decode is  	*/
/* continued by '_mms_null_pdu_dec', which simply sets decode done = 1 	*/
/* and sets the decode done function to point to the general decode done*/
/* fun.  See mmsdec.c for the general functions. 			*/
/************************************************************************/


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_defea					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE and send the define event action request.   	*/
/************************************************************************/

MMSREQ_PEND *mp_defea (ST_INT chan, DEFEA_REQ_INFO *info_ptr)
  {
  return (_mms_req_send (MMSOP_DEF_EA,
			M_CAST_MK_FUN (_ms_mk_defea_req),
			chan,(ST_CHAR *)info_ptr));
  } 

#else
/************************************************************************/
/************************************************************************/
/*			mpl_defea					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the define event action request.   		*/
/************************************************************************/

ST_RET  mpl_defea (DEFEA_REQ_INFO *info_ptr)
  {
  return (_mms_req_fin (MMSOP_DEF_EA,
			M_CAST_MK_FUN (_ms_mk_defea_req),
			(ST_CHAR *)info_ptr));
  } 
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_defea_req					*/
/* ENCODE a define event action REQUEST:				*/
/************************************************************************/

ST_VOID _ms_mk_defea_req (ASN1_ENC_CTXT *aCtx, DEFEA_REQ_INFO *info)
  {
#ifdef MOD_SUPPORT
MODIFIER *mod_ptr;
#endif

#ifdef CS_SUPPORT		/* if companion standards are supported	*/
  if (info->cs_rdetail_pres)	/*   check for CS request detail	*/
    {		       
    asn1r_strt_constr (aCtx);			/* start ctx tag 79 constructor	*/
    asn1r_wr_delmnt (aCtx, info->cs_rdetail, info->cs_rdetail_len);
    asn1r_fin_constr (aCtx, 79, CTX, DEF);		/* CS is cstr 79		*/
    }
#endif

  asn1r_strt_constr (aCtx);			/* start ctx tag 2 constructor	*/
  asn1r_wr_delmnt (aCtx, info->conf_serv_req,info->conf_serv_req_len);
  asn1r_fin_constr (aCtx, 2,CTX,DEF);		/* finish ctx tag 2 constructor */

#ifdef MOD_SUPPORT
  if (info->modlist_pres)
    {
    mod_ptr = (MODIFIER *) (info + 1);
    asn1r_strt_constr (aCtx);			/* start ctx tag 1 constructor	*/
    _ms_wr_mms_modlist (aCtx, mod_ptr, info->num_of_modifiers);
    asn1r_fin_constr (aCtx, 1,CTX,DEF);		/* finish ctx tag 1 constructor */
    }
#endif

  asn1r_strt_constr (aCtx);			/* start ctx tag 0 constructor	*/
  _ms_wr_mms_objname (aCtx, &info->evact_name);	/* write event action name	*/
  asn1r_fin_constr (aCtx, 0,CTX,DEF);		/* context specific tag 0	*/

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_constr (aCtx, MMSOP_DEF_EA,CTX,DEF);		/* tag = opcode, ctx constr	*/
  }
