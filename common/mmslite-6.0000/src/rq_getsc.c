/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rq_getscat.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the requester portion of the get scattered	*/
/*	access attributes.  It decodes the get scattered access		*/
/*	attributes response (confirm) & encodes the get scattered 	*/
/*	access attributes request. 					*/
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
/* variables global to the DECODE portion :				*/

static  GETSCAT_RESP_INFO	*rsp_info;

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/

static ST_VOID getscat_scat_acc_cstr_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID getscat_scat_acc_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID getscat_scat_acc_cstr (ASN1_DEC_CTXT *aCtx);
static ST_VOID getscat_get_mms_deletable (ASN1_DEC_CTXT *aCtx);


/************************************************************************/
/************************************************************************/
/*			mms_get_scat_rsp 				*/
/* This function is called from MMSDEC when the opcode for this 	*/
/* operation is decoded. Setup state machine function pointers, etc.	*/
/************************************************************************/

ST_VOID mms_get_scat_rsp (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("Get Scattered Access Attributes Response");

/* get storage for request information	*/
  rsp_info = (GETSCAT_RESP_INFO *) _m_get_dec_buf (aCtx, sizeof (GETSCAT_RESP_INFO));
  ASN1R_TAG_ADD (aCtx, CTX, 0, getscat_get_mms_deletable);
  }

/************************************************************************/
/*			getscat_get_mms_deletable			*/
/* Decoding get scattered access attributes response: mms deletable was	*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID getscat_get_mms_deletable (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("getscat_get_mms_deletable");
					/* read mms deletable		*/
  if (asn1r_get_bool (aCtx, &(rsp_info->mms_deletable)))
    asn1r_set_dec_err (aCtx, RESP_BAD_VALUE);

  ASN1R_TAG_ADD (aCtx, CTX | CONSTR, 1, getscat_scat_acc_cstr);
  }

/************************************************************************/
/*			getscat_scat_acc_cstr				*/
/* get scattered access attributes request: scattered access constr was	*/
/* encountered. 							*/
/************************************************************************/

static ST_VOID getscat_scat_acc_cstr (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("getscat_scat_acc_cstr");

  _ms_get_va_scat_access (aCtx, &rsp_info->sa_descr, getscat_scat_acc_done);
  }

/************************************************************************/
/*			getscat_scat_acc_done				*/
/* get scattered access attributes request: scattered access has been	*/
/* decoded.								*/
/************************************************************************/

static ST_VOID getscat_scat_acc_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("getscat_scat_acc_done");

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = getscat_scat_acc_cstr_done;
  }

/************************************************************************/
/*			getscat_scat_acc_cstr_done			*/
/* get scattered access attributes request: scattered access constr	*/
/* done was encountered.   						*/
/************************************************************************/

static ST_VOID getscat_scat_acc_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
  _ms_set_cs_check (aCtx);		/* check for Companion Standard info	*/
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_getscat					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE & send the get scattered access attributes request */
/************************************************************************/

MMSREQ_PEND *mp_getscat (ST_INT chan, GETSCAT_REQ_INFO *info_ptr)
  {
  return (_mms_req_send (MMSOP_GET_SCAT,
			M_CAST_MK_FUN (_ms_mk_getscat_req),
			chan,(ST_CHAR *)info_ptr));
  } 

#else
/************************************************************************/
/************************************************************************/
/*			mpl_getscat					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the get scattered access attributes request. 	*/
/************************************************************************/

ST_RET  mpl_getscat (GETSCAT_REQ_INFO *info_ptr)
  {
  return (_mms_req_fin (MMSOP_GET_SCAT,
			M_CAST_MK_FUN (_ms_mk_getscat_req),
			(ST_CHAR *)info_ptr));
  } 
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_getscat_req					*/
/* construct a get scattered access attributes request			*/
/************************************************************************/

ST_VOID _ms_mk_getscat_req (ASN1_ENC_CTXT *aCtx, GETSCAT_REQ_INFO *info)
  {
  _ms_wr_mms_objname (aCtx, &info->sa_name);	/* write scattered access name	*/

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_constr (aCtx, MMSOP_GET_SCAT,CTX,DEF); 	/* tag = opcode, ctx		*/
  }
