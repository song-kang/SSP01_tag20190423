/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2004, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rq_info.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the requester portion of information	*/
/*	report.  It encodes the information report request.  There	*/
/*	is no response (confirm).					*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 03/28/05  JRB     05    Incl suicacse if MMS_LITE not defined.	*/
/* 09/17/04  EJV     04    Use new mms_chk_resource.			*/
/* 12/12/02  JRB     03    Add ASN1_ENC_CTXT arg to mpl_info		*/ 
/* 12/20/01  JRB     02    Converted to use ASN1R (re-entrant ASN1)	*/
/* 06/15/98  MDE     01    Changes to allow compile under C++		*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"

#include "mmsdefs.h"
#include "mms_pvar.h"
#include "asn1defs.h"

/************************************************************************/
/* variables global to the DECODE portion : NONE			*/

/************************************************************************/
/* variables global to the ENCODE portion : NONE			*/


#ifndef MMS_LITE
#include "mms_vcon.h"
#include "suicacse.h"	/* for SE_QUE_FULL only	*/

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif
  
/************************************************************************/
/************************************************************************/
/*			mp_info 					*/
/* Create and send an information report request. 		        */
/************************************************************************/

ST_RET mp_info (ST_INT chan, INFO_REQ_INFO *info_ptr)
  {
ST_RET rc;
ST_INT avail;

  /* locking here will assure that _mms_chk_resource will work in	*/
  /* multiple threads when mp_* functions are called to send		*/
  /* unconfirmed requests						*/
  S_LOCK_COMMON_RESOURCES ();

  avail = _mms_chk_resource (chan);
  if (avail > 0)
    rc = _mms_unconf_req_send (MMSOP_INFO_RPT,
			M_CAST_MK_FUN (_ms_mk_info_req),
			chan,(ST_CHAR *)info_ptr);
  else
    rc = SE_QUE_FULL;

  S_UNLOCK_COMMON_RESOURCES ();

  return (rc);
  } 

#else
/************************************************************************/
/************************************************************************/
/*			mpl_info 					*/
/* Create an information report request. 		        	*/
/************************************************************************/

ST_RET  mpl_info (ASN1_ENC_CTXT *aCtx, INFO_REQ_INFO *info_ptr)
  {
  return (_mms_unconf_req_fin (aCtx, MMSOP_INFO_RPT,
			M_CAST_MK_FUN (_ms_mk_info_req),
			(ST_CHAR *)info_ptr));
  } 
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_info_req					*/
/* construct an information report request				*/
/************************************************************************/

ST_VOID _ms_mk_info_req (ASN1_ENC_CTXT *aCtx, INFO_REQ_INFO *info_ptr)
  {
ST_INT i;
ACCESS_RESULT *ar_ptr;

  asn1r_strt_constr (aCtx);			/* start context tag 0 cstr	*/
  ar_ptr  = info_ptr->acc_rslt_list;
  ar_ptr += info_ptr->num_of_acc_result - 1;
  for (i = 0; i < info_ptr->num_of_acc_result; i++)
    {
    _ms_wr_va_access_rslt (aCtx, ar_ptr);
    ar_ptr--;
    }
  asn1r_fin_constr (aCtx, 0, CTX, DEF);		/* finish context tag 0 cstr	*/

  _ms_wr_va_spec (aCtx, &info_ptr->va_spec);

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_constr (aCtx, 0,CTX,DEF);		/* tag = opcode, ctx constr	*/
  }
