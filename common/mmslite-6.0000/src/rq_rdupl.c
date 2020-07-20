/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rq_rdupl.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the requester portion of request domain	*/
/*	upload.  It decodes the request domain upload response 		*/
/*	(confirm) & encodes the request domain upload request.		*/
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
/* variables global to the ENCODE portion : NONE			*/


/************************************************************************/
/************************************************************************/
/*			mms_rdupl_rsp					*/
/* NOTE : Since the mms_rdupl_rsp is a NULL PDU, the decode is continued*/
/* by '_mms_null_pdu_dec', which simply sets decode done = 1 and sets the*/
/* decode done function to point to the general decode done fun.  See   */
/* mmsdec.c for the general functions. 					*/
/************************************************************************/


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_rdupl					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE and send the request domain upload request.	*/
/************************************************************************/

MMSREQ_PEND *mp_rdupl (ST_INT chan, RDUPL_REQ_INFO *info_ptr)
  {
  return (_mms_req_send (MMSOP_REQ_DOM_UPL,
			M_CAST_MK_FUN (_ms_mk_rdupl_req),
			chan,(ST_CHAR *)info_ptr));
  } 

#else
/************************************************************************/
/************************************************************************/
/*			mpl_rdupl					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the request domain upload request.			*/
/************************************************************************/

ST_RET  mpl_rdupl (RDUPL_REQ_INFO *info_ptr)
  {
  return (_mms_req_fin (MMSOP_REQ_DOM_UPL,
			M_CAST_MK_FUN (_ms_mk_rdupl_req),
			(ST_CHAR *)info_ptr));
  } 
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_rdupl_req 					*/
/* ENCODE a request domain upload REQUEST:				*/
/************************************************************************/

ST_VOID _ms_mk_rdupl_req (ASN1_ENC_CTXT *aCtx, RDUPL_REQ_INFO *info)
  {
  asn1r_strt_constr (aCtx);
  _ms_wr_mms_fname (aCtx, (FILE_NAME *)(info + 1),
                info->num_of_fname);
  asn1r_fin_constr (aCtx, 1,CTX,DEF);		/* context specific tag 1	*/

  asn1r_wr_vstr (aCtx, info->dname);		/* write the domain name	*/
  asn1r_fin_prim (aCtx, 0,CTX);			/* context specific tag 0	*/

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_constr (aCtx, MMSOP_REQ_DOM_UPL,CTX,DEF);	/* tag = opcode, ctx constr	*/
  }

