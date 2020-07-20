/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2012, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rs_conclude.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the responder portion of the conclude	*/
/*	operation.  It decodes the conclude request (indication) and	*/
/*	encodes the conclude response.					*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 11/23/11  KCR           Added casting for proper 64bit compile       */
/* 11/01/07  EJV     04    Rem obsolete LLC30... code.			*/
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

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/

#if defined(DEBUG_SISCO) && defined(S_MT_SUPPORT)
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/* External conclude functions (see mmsconcl) : 			*/
/* ST_CHAR    *_ms_mk_conclude (buf_ptr,buf_len,pdutype)			*/

/************************************************************************/
/************************************************************************/
/*			mms_conclude_req				*/
/* NOTE : Since a MMSOP_CONCLUDE is a NULL PDU, the decode is continued	*/
/* by '_mms_null_pdu_dec', which simply sets decode done = 1 and sets	*/
/* the decode done function to point to the general decode done fun	*/
/* See mmsdec.c for the general functions.				*/
/************************************************************************/


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_concl_resp					*/
/* create and send a conclude response. 				*/
/************************************************************************/

ST_RET mp_conclude_resp (MMSREQ_IND *indptr)
  {
ST_UCHAR	*msg_ptr;
ST_INT	msg_len;
ST_RET	ret;
ASN1_ENC_CTXT localEncCtx;	/* For readability, use "aCtx" to access this.*/
ASN1_ENC_CTXT *aCtx = &localEncCtx;

  S_LOCK_COMMON_RESOURCES ();
  if (!(ret = _ms_check_ind_actv (indptr)))
    {
    msg_ptr = _ms_mk_conclude (aCtx, m_build_buf,mms_max_msgsize,MMSCNCLRESP);
    msg_len = (ST_INT) (M_BUILD_BUF_END - msg_ptr);

    if (aCtx->asn1r_encode_overrun)		/* Check for encode overrun	*/
      {
      S_UNLOCK_COMMON_RESOURCES ();
      return (ME_ASN1_ENCODE_OVERRUN);
      }
    else
      ret = _mms_send_data (indptr->chan, msg_len, msg_ptr, 
		indptr->context, indptr->add_addr_info);

    if (!ret)
      {
        mms_chan_info[indptr->chan].ctxt.chan_state |= M_REL_IND_WAIT;

  /* If a response logging function is installed, call it		*/
      if (m_log_resp_info_fun && (mms_debug_sel & MMS_LOG_RESP))
        (*m_log_resp_info_fun) (indptr, NULL);
      }

    _ms_ind_serve_done (indptr);
    }
  S_UNLOCK_COMMON_RESOURCES ();
  return (ret);
  }

#else
/************************************************************************/
/************************************************************************/
/*			mpl_concl_resp					*/
/* create a conclude response. 						*/
/************************************************************************/

ST_RET mpl_conclude_resp (ST_VOID)
  {
ST_RET ret;
ASN1_ENC_CTXT localEncCtx;	/* For readability, use "aCtx" to access this.*/
ASN1_ENC_CTXT *aCtx = &localEncCtx;

  S_LOCK_COMMON_RESOURCES ();
  mmsl_msg_start = _ms_mk_conclude (aCtx, mmsl_enc_buf, mmsl_enc_buf_size, MMSCNCLRESP);
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
/* Note that there is only one _ms_mk_conclude function, and it is listed	*/
/* in the mmsconcl.c module.						*/
/************************************************************************/

