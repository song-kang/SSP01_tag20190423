/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rq_storedom.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the requester portion of store domain	*/
/*	content. It decodes the store domain content response (confirm) */
/*	& encodes the store domain content request.			*/
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
#include "mms_pdom.h"
#include "asn1defs.h"

/************************************************************************/
/* variables global to the DECODE portion : NONE			*/
/* variables global to the ENCODE portion : NONE			*/

/************************************************************************/
/************************************************************************/
/*			mms_store_domain_rsp				*/
/* NOTE : Since the mms_store_domain_rsp is a NULL PDU, the decode is	*/
/* continued by '_mms_null_pdu_dec', which simply sets decode done = 1   */
/* and sets the decode done function to point to the general decode done*/
/* fun.  See mmsdec.c for the general functions. 			*/
/************************************************************************/


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_storedom					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE and send the store domain content request. 	*/
/************************************************************************/

MMSREQ_PEND *mp_storedom (ST_INT chan, STOREDOM_REQ_INFO *info_ptr)
  {
  return (_mms_req_send (MMSOP_STORE_DOMAIN,
			M_CAST_MK_FUN (_ms_mk_storedom_req),
			chan,(ST_CHAR *)info_ptr));
  } 

#else
/************************************************************************/
/************************************************************************/
/*			mpl_storedom					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the store domain content request. 			*/
/************************************************************************/

ST_RET mpl_storedom (STOREDOM_REQ_INFO *info_ptr)
  {
  return (_mms_req_fin (MMSOP_STORE_DOMAIN,
			M_CAST_MK_FUN (_ms_mk_storedom_req),
			(ST_CHAR *)info_ptr));
  } 
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_storedom_req 				*/
/* ENCODE a store domain content REQUEST:				*/
/************************************************************************/

ST_VOID _ms_mk_storedom_req (ASN1_ENC_CTXT *aCtx, STOREDOM_REQ_INFO *info)
  {
  asn1r_strt_constr (aCtx);			/* start sequence constructor	*/

  if (info->third_pty_pres)
    {
    asn1r_wr_delmnt (aCtx, info->third_pty, info->third_pty_len);
    *(aCtx->asn1r_field_ptr+1) = (ST_UCHAR) 0xA2;
    }

  asn1r_strt_constr (aCtx);
  _ms_wr_mms_fname (aCtx, (FILE_NAME *)(info + 1), 
                 info -> num_of_fname);
  asn1r_fin_constr (aCtx, 1,CTX,DEF);		/* context specific tag 1	*/

  asn1r_wr_vstr (aCtx, info->dname);		/* write the domain name	*/
  asn1r_fin_prim (aCtx, 0,CTX);			/* context specific tag 0	*/

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_constr (aCtx, MMSOP_STORE_DOMAIN,CTX,DEF);	/* tag = opcode, ctx constr	*/
  }

