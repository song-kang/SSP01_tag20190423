/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2012, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rq_concl.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the requester portion of the conclude	*/
/*	operation.  It decodes the conclude response (confirm) &	*/
/*	encodes the conclude request.					*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 01/19/12  EJV           _WIN64 added typecasts to elim warnings.	*/
/* 11/23/11  KCR           Added casting for proper 64bit compile       */
/* 11/01/07  EJV     05    Rem obsolete LLC30... code.			*/
/* 02/28/05  EJV     04    Elim Linux warnings in if (a=b)		*/
/* 03/11/04  GLB     03    Cleaned up "thisFileName"                    */
/* 12/20/01  JRB     02    Converted to use ASN1R (re-entrant ASN1)	*/
/* 09/13/99  MDE     01    Added SD_CONST modifiers			*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "glbsem.h"

#include "mmsdefs.h"
#include "mms_pcon.h"
#include "asn1defs.h"
#include "slog.h"

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/

#if defined(DEBUG_SISCO) && defined(S_MT_SUPPORT)
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/* External functions (see mmsconcl) :					*/
/* ST_CHAR *_ms_mk_conclude (buf_ptr,buf_len,pdutype)				*/

/************************************************************************/
/************************************************************************/
/*			mms_conclude_rsp				*/
/* NOTE : Since a CONCLUDE is a NULL PDU, the decode is continued	*/
/* by '_mms_null_pdu_dec', which simply sets decode done = 1 and sets	*/
/* the decode done function to point to the general decode done fun	*/
/* See mmsdec.c for the general functions.				*/
/************************************************************************/


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_conclude					*/
/* create and send a conclude request.					*/
/************************************************************************/

MMSREQ_PEND *mp_conclude (ST_INT chan)
  {
ST_UCHAR	*msg_ptr;
ST_INT	msg_len;
MMSREQ_PEND *reqptr;
LLP_ADDR_INFO add_llp_info;
ASN1_ENC_CTXT localEncCtx;	/* For readability, use "aCtx" to access this.*/
ASN1_ENC_CTXT *aCtx = &localEncCtx;

  S_LOCK_COMMON_RESOURCES ();

  reqptr = _ms_get_req_ptr (chan,MMSOP_CONCLUDE,(ST_CHAR *) 0);
  if (!reqptr)
    {
    S_UNLOCK_COMMON_RESOURCES ();
    return ((MMSREQ_PEND *) 0);
    }

  msg_ptr = _ms_mk_conclude (aCtx,m_build_buf,mms_max_msgsize,MMSCNCLREQ);
  msg_len = (ST_INT) (M_BUILD_BUF_END - msg_ptr);		/* find length		*/
  
  add_llp_info.i.LLC.use_rem_addr = SD_FALSE;
  add_llp_info.i.LLC.send_type = CONCLUDE_REQ_PDU;
  add_llp_info.i.LLC.req_ptr = (ST_CHAR *)reqptr;

  if (aCtx->asn1r_encode_overrun)		/* Check for encode overrun	*/
    {
    mms_op_err = ME_ASN1_ENCODE_OVERRUN;
    _ms_deact_pend_req (reqptr);		/* move from pend to avail	*/
    S_UNLOCK_COMMON_RESOURCES ();
    return ((MMSREQ_PEND *) 0); 	/* return error code		*/
    }
  else if ((mms_op_err = _mms_send_data (chan, msg_len, msg_ptr, 
		reqptr->context, &add_llp_info)) != 0)
    {					/* need to recover req ctrl	*/
    _ms_deact_pend_req (reqptr);		/* move from pend to avail	*/
    S_UNLOCK_COMMON_RESOURCES ();
    return ((MMSREQ_PEND *) 0); 	/* return error code		*/
    }
  else
    {
      mms_chan_info[chan].ctxt.chan_state |= M_CONCL_REQ_PEND;

  /* If a request logging function is installed, call it		*/
    if (m_log_req_info_fun && (mms_debug_sel & MMS_LOG_REQ))
      (*m_log_req_info_fun) (reqptr, NULL);

    S_UNLOCK_COMMON_RESOURCES ();
    return (reqptr);
    }
  }
#else
/************************************************************************/
/************************************************************************/
/*			mpl_conclude					*/
/* create a conclude request.						*/
/************************************************************************/

ST_RET mpl_conclude (ST_VOID)
  {
ST_RET ret;
ASN1_ENC_CTXT localEncCtx;	/* For readability, use "aCtx" to access this.*/
ASN1_ENC_CTXT *aCtx = &localEncCtx;

  S_LOCK_COMMON_RESOURCES ();
  mmsl_msg_start = _ms_mk_conclude (aCtx, mmsl_enc_buf, mmsl_enc_buf_size, MMSCNCLREQ);
  mmsl_msg_len   = (int) ((mmsl_enc_buf + mmsl_enc_buf_size) - mmsl_msg_start);

  if (!aCtx->asn1r_encode_overrun)		/* Check for encode overrun 	*/
    ret = SD_SUCCESS;
  else
    ret = ME_ASN1_ENCODE_OVERRUN;

  S_UNLOCK_COMMON_RESOURCES ();
  return (ret);
  }
#endif

/************************************************************************/
/*			    _ms_mk_conclude					*/
/* Note that there is only one _ms_mk_conclude function, and it is listed 	*/
/* in the mmsconcl.c module.						*/
/************************************************************************/

