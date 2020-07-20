/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2005, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rq_termdown.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the requester portion of the terminate	*/
/*	download sequence.  It decodes the terminate download sequence	*/
/*	response (confirm) & encodes the terminate download sequence	*/
/*	request.							*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 08/09/05  EJV     06    MMS_LITE does not need thisFileName.         */
/* 03/17/04  RKR     05    Changed thisFileName                         */
/* 03/11/04  GLB     04    Remove "thisFileName"                    	*/
/* 12/20/01  JRB     03    Converted to use ASN1R (re-entrant ASN1)	*/
/* 09/13/99  MDE     02    Added SD_CONST modifiers			*/
/* 06/15/98  MDE     01    Changes to allow compile under C++		*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "glbsem.h"

#include "mmsdefs.h"
#include "mms_pdom.h"
#include "asn1defs.h"

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/

#if defined(DEBUG_SISCO) && (defined(S_MT_SUPPORT) && !defined(MMS_LITE))
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/* variables global to the DECODE portion : NONE			*/

#ifndef MMS_LITE
/* variables global to the ENCODE portion : 				*/
static ST_INT channel;
#else
/* variables global to the ENCODE portion : NONE			*/
#endif


/************************************************************************/
/************************************************************************/
/*			mms_term_download_rsp			        */
/* NOTE : Since the mms_term_download_rsp is a NULL PDU, the decode is  */
/* continued by '_mms_null_pdu_dec', which simply sets decode done = 1 	*/
/* and sets the decode done function to point to the general decode done*/
/* fun.  See mmsdec.c for the general functions. 			*/
/************************************************************************/


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_termdown					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE and send the terminate download sequence request.	*/
/************************************************************************/

MMSREQ_PEND *mp_termdown (ST_INT chan, TERMDOWN_REQ_INFO *info_ptr)
  {
MMSREQ_PEND *ret;

  S_LOCK_COMMON_RESOURCES ();
  channel = chan;		/* save for mk function to use		*/
  ret = _mms_req_send (MMSOP_TERM_DOWNLOAD, 
			M_CAST_MK_FUN (_ms_mk_termdown_req),
			 
		      chan, (ST_CHAR *)info_ptr);
  S_UNLOCK_COMMON_RESOURCES ();
  return (ret);
  } 

#else
/************************************************************************/
/************************************************************************/
/*			mpl_termdown					*/
/* This function is called from either the application or the virtual	*/
/* machine to ENCODE the terminate download sequence request.		*/
/************************************************************************/

ST_RET  mpl_termdown (TERMDOWN_REQ_INFO *info_ptr)
  {
  return (_mms_req_fin (MMSOP_TERM_DOWNLOAD,
			M_CAST_MK_FUN (_ms_mk_termdown_req),
			(ST_CHAR *)info_ptr));
  } 
#endif


/************************************************************************/
/************************************************************************/
/*			_ms_mk_termdown_req 				*/
/* construct a terminate download sequence request			*/
/************************************************************************/

ST_VOID _ms_mk_termdown_req (ASN1_ENC_CTXT *aCtx, TERMDOWN_REQ_INFO *info)
  {
  asn1r_strt_constr (aCtx);			/* start sequence constr	*/

  if (info->discarded)
    {					/* NOT the default (SD_FALSE)	*/
#ifndef MMS_LITE
    if (mms_chan_info[channel].version)		/* IS 			*/
#else
    if (mmsl_version)				/* IS 			*/
#endif
      {
      _ms_wr_service_err (aCtx, (info->err)->eclass,(info->err)->code,
				&(info->err)->adtnl);
      asn1r_fin_constr (aCtx, 1,CTX,DEF);	
      }
    else					/* DIS			*/
      {
      asn1r_wr_bool (aCtx, info->discarded);		/* write the discard flag	*/
      asn1r_fin_prim (aCtx, 1, CTX);			/* context tag 1		*/
      }
    }

  asn1r_wr_vstr (aCtx, info->dname);		/* write the domain name	*/
  asn1r_fin_prim (aCtx, 0, CTX);			/* context tag 0		*/

/* write the context specific explicit tag for this opcode.		*/
  asn1r_fin_constr (aCtx, MMSOP_TERM_DOWNLOAD,CTX,DEF);	/* tag = opcode, ctx constr	*/
  }

