/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rq_altec.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the requester portion of alter event	*/
/*	condition monitoring.  It decodes the alter event condition	*/
/*	monitoring response (confirm) & encodes the alter event		*/
/*	condition monitoring request.				 	*/
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
/*			mms_altecm_rsp					*/
/* NOTE : Since the mms_altecm_rsp is a NULL PDU, the decode is  	*/
/* continued by '_mms_null_pdu_dec', which simply sets decode done = 1 	*/
/* and sets the decode done function to point to the general decode done*/
/* fun.  See mmsdec.c for the general functions. 			*/
/************************************************************************/


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_altecm					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE and send the alter event condition monitoring	*/
/* request. 							  	*/
/************************************************************************/

MMSREQ_PEND *mp_altecm (ST_INT chan, ALTECM_REQ_INFO *info_ptr)
  {
  return (_mms_req_send (MMSOP_ALT_EC_MON,
			M_CAST_MK_FUN (_ms_mk_altecm_req),
			chan,(ST_CHAR *)info_ptr));
  } 

#else
/************************************************************************/
/************************************************************************/
/*			mpl_altecm					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the alter event condition monitoring request. 	*/
/************************************************************************/

ST_RET  mpl_altecm (ALTECM_REQ_INFO *info_ptr)
  {
  return (_mms_req_fin (MMSOP_ALT_EC_MON,
			M_CAST_MK_FUN (_ms_mk_altecm_req),
			(ST_CHAR *)info_ptr));
  } 
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_altecm_req					*/
/* ENCODE a alter event condition monitoring REQUEST:	 		*/
/************************************************************************/

ST_VOID _ms_mk_altecm_req (ASN1_ENC_CTXT *aCtx, ALTECM_REQ_INFO *info)
  {
  if (info->eval_int_pres)
    {			   
    asn1r_wr_u32 (aCtx, info->eval_int); 		/* write evaluation interval	*/
    asn1r_fin_prim (aCtx, 4, CTX);			/* ctx tag 4			*/
    }

  if (info->as_reports_pres)
    {				      
    asn1r_wr_bool (aCtx, info->as_reports);   	/* write alarm summary reports	*/
    asn1r_fin_prim (aCtx, 3, CTX);			/* ctx tag 3			*/
    }

  if (info->priority_pres)
    {			   
    asn1r_wr_u8 (aCtx, info->priority); 		/* write the priority		*/
    asn1r_fin_prim (aCtx, 2, CTX);			/* ctx tag 2			*/
    }

  if (info->enabled_pres)
    {				      
    asn1r_wr_bool (aCtx, info->enabled); 		/* write enabled 		*/
    asn1r_fin_prim (aCtx, 1, CTX);			/* ctx tag 1			*/
    }

  asn1r_strt_constr (aCtx);			/* start ctx tag 0 constructor	*/
  _ms_wr_mms_objname (aCtx, &info->evcon_name);	/* write event cond mon name	*/
  asn1r_fin_constr (aCtx, 0,CTX,DEF);		/* context specific tag 0	*/

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_constr (aCtx, MMSOP_ALT_EC_MON,CTX,DEF);   	/* tag = opcode, ctx constr	*/
  }
