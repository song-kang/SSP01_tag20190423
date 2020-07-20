/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rq_defec.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the requester portion of define event	*/
/*	condition.  It decodes the define event condition response	*/
/*	(confirm) & encodes the define event condition request. 	*/
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
/*			mms_defec_rsp					*/
/* NOTE : Since the mms_defec_rsp is a NULL PDU, the decode is  	*/
/* continued by '_mms_null_pdu_dec', which simply sets decode done = 1 	*/
/* and sets the decode done function to point to the general decode done*/
/* fun.  See mmsdec.c for the general functions. 			*/
/************************************************************************/


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_defec					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE and send the define event condition request.   	*/
/************************************************************************/

MMSREQ_PEND *mp_defec (ST_INT chan, DEFEC_REQ_INFO *info_ptr)
  {
  return (_mms_req_send (MMSOP_DEF_EC,
			M_CAST_MK_FUN (_ms_mk_defec_req),
			chan,(ST_CHAR *)info_ptr));
  } 

#else
/************************************************************************/
/************************************************************************/
/*			mpl_defec					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the define event condition request.   		*/
/************************************************************************/

ST_RET mpl_defec (DEFEC_REQ_INFO *info_ptr)
  {
  return (_mms_req_fin (MMSOP_DEF_EC,
			M_CAST_MK_FUN (_ms_mk_defec_req),
			(ST_CHAR *)info_ptr));
  } 
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_defec_req					*/
/* ENCODE a define event condition REQUEST:				*/
/************************************************************************/

ST_VOID _ms_mk_defec_req (ASN1_ENC_CTXT *aCtx, DEFEC_REQ_INFO *info)
  {
  if (info->eval_int_pres)
    {
    asn1r_wr_u32 (aCtx, info->eval_interval); 	/* write evaluation interval	*/
    asn1r_fin_prim (aCtx, 7, CTX);			/* ctx tag 7			*/
    }

  if (info->mon_var_pres)
    {					/* write variable specification	*/
    asn1r_strt_constr (aCtx);			/* start ctx tag 6 constructor	*/
    _ms_wr_va_var_spec (aCtx, &info->var_ref);
    asn1r_fin_constr (aCtx, 6, CTX, DEF);		/* finish ctx tag 6		*/
    }

  if (info->as_reports_pres)
    {				      
    asn1r_wr_bool (aCtx, info->as_reports);  	/* write alarm summary reports	*/
    asn1r_fin_prim (aCtx, 4, CTX);			/* ctx tag 4			*/
    }

  if (info->severity != MMS_NORM_SEVERITY)
    {					/* not the default		*/
    asn1r_wr_u8 (aCtx, info->severity);
    asn1r_fin_prim (aCtx, 3, CTX);
    }

  if (info->priority != MMS_NORM_PRIORITY)
    {					/* not the default		*/
    asn1r_wr_u8 (aCtx, info->priority);
    asn1r_fin_prim (aCtx, 2, CTX);
    }

  asn1r_wr_i16 (aCtx, info->eclass);
  asn1r_fin_prim (aCtx, 1, CTX);

  asn1r_strt_constr (aCtx);			/* start ctx tag 0 constructor	*/
  _ms_wr_mms_objname (aCtx, &info->evcon_name);	/* write event condition name	*/
  asn1r_fin_constr (aCtx, 0,CTX,DEF);		/* context specific tag 0	*/

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_constr (aCtx, MMSOP_DEF_EC,CTX,DEF);		/* tag = opcode, ctx constr	*/
  }
