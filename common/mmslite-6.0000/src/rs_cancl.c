/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2012, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rs_cancl.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the responder portion of the cancel	*/
/*	operation.  It decodes the cancel request (indication) and	*/
/*	encodes the cancel response.					*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 01/19/12  EJV           _WIN64 added typecasts to elim warnings.	*/
/* 11/23/11  KCR           Added casting for proper 64bit compile       */
/* 02/24/10  JRB	   Del _mmsdec_rslt, use aCtx->mmsdec_rslt	*/
/* 11/01/07  EJV     06    Rem obsolete LLC30... code.			*/
/* 03/09/05  EJV     05    Fixed thisFileName for MMS-EASE              */
/* 03/11/04  GLB     04    Cleaned up "thisFileName"                    */
/* 12/20/01  JRB     03    Converted to use ASN1R (re-entrant ASN1)	*/
/* 09/13/99  MDE     02    Added SD_CONST modifiers			*/
/* 05/07/98  MDE     01    Fixed logging				*/
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

#if defined(DEBUG_SISCO) && (defined(S_MT_SUPPORT) || !defined(MMS_LITE))
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/


/* variables global to the DECODE portion : NONE			*/
/* variables global to the ENCODE portion : NONE			*/

/************************************************************************/
/************************************************************************/
/*			mms_cancel_req					*/
/* This function is called from mmsdec when a request is received and	*/
/* the opcode for this operation is decoded.  Set up state machine	*/
/* function pointers to end the parse, since the parse is complete.	*/
/************************************************************************/

ST_VOID mms_cancel_req (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("Cancel Request");

  aCtx->mmsdec_rslt->data_pres = SD_FALSE;	/* cancel just has invoke id*/
  aCtx->asn1r_decode_done = SD_TRUE;
  asn1r_set_all_cstr_done (aCtx);
  aCtx->asn1r_decode_done_fun = NULL;
  }

#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_cancel_resp					*/
/* Format a Cancel response PDU.					*/
/************************************************************************/

ST_RET mp_cancel_resp (MMSREQ_IND *indptr)
  {
ST_UCHAR	*msg_ptr;
ST_INT	msg_len;
ST_RET	ret;
ASN1_ENC_CTXT localEncCtx;	/* For readability, use "aCtx" to access this.*/
ASN1_ENC_CTXT *aCtx = &localEncCtx;

  S_LOCK_COMMON_RESOURCES ();
  if (!(ret = _ms_check_ind_actv (indptr)))
    {
    if (indptr -> cancl_state != CANCEL_REQUESTED)
      {
      S_UNLOCK_COMMON_RESOURCES ();
      return (MVE_CANCEL_STATE);
      }
    msg_ptr = _ms_mk_cancel (aCtx, SD_FALSE,m_build_buf,mms_max_msgsize,indptr->id);
    msg_len = (ST_INT) (M_BUILD_BUF_END - msg_ptr);

    if (aCtx->asn1r_encode_overrun)		/* Check for encode overrun	*/
      ret = ME_ASN1_ENCODE_OVERRUN;
    else
      ret = _mms_send_data (indptr->chan, msg_len, msg_ptr, 
		indptr->context, indptr->add_addr_info);


    MLOG_ALWAYS0 ("ISSUING MMS CANCEL RESPONSE");
    MLOG_CALWAYS1 ("Channel = %d",indptr->chan);
    MLOG_CALWAYS1 ("Context = %02x",indptr->context);
    MLOG_CALWAYS1 ("Original Invoke ID = %lu",indptr->id);

    }			   
  S_UNLOCK_COMMON_RESOURCES ();
  return (ret);
  }

#else
/************************************************************************/
/************************************************************************/
/*			mpl_cancel_resp					*/
/* Create a Cancel response PDU.					*/
/************************************************************************/
ST_RET mpl_cancel_resp (ST_VOID)
  {
ST_RET ret;
ASN1_ENC_CTXT localEncCtx;	/* For readability, use "aCtx" to access this.*/
ASN1_ENC_CTXT *aCtx = &localEncCtx;

  S_LOCK_COMMON_RESOURCES ();
  mmsl_msg_start = _ms_mk_cancel (aCtx, SD_FALSE, mmsl_enc_buf, mmsl_enc_buf_size, 
  			mmsl_invoke_id);
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
/*			    _ms_mk_cancel					*/
/* Note that there is only one _ms_mk_cancel function, and it is listed in	*/
/* the mmscancl.c module.						*/
/************************************************************************/
