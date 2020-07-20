/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2012, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rq_cancl.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the requester portion of the cancel	*/
/*	operation.  It decodes the cancel response (confirm) & encodes	*/
/*	the cancel request.						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 01/19/12  EJV           _WIN64 added typecasts to elim warnings.	*/
/* 11/23/11  KCR           Added casting for proper 64bit compile       */
/* 02/24/10  JRB	   Del _mmsdec_rslt, use aCtx->mmsdec_rslt	*/
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

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/

#if defined(DEBUG_SISCO) && defined(S_MT_SUPPORT)
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/

/* variables global to the DECODE portion : NONE			*/
/* variables global to the ENCODE portion : NONE			*/


/************************************************************************/
/************************************************************************/
/*			mms_cancel_rsp					*/
/* This function is called from mmsdec when a response is received and	*/
/* the opcode for this operation is decoded. Set up state machine	*/
/* function pointers to end the parse, since the parse is complete.	*/
/************************************************************************/

ST_VOID mms_cancel_rsp (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("Cancel Response");

  aCtx->mmsdec_rslt->data_pres = SD_FALSE;	/* cancel just has invoke id*/
  aCtx->asn1r_decode_done = SD_TRUE;
  asn1r_set_all_cstr_done (aCtx);
  aCtx->asn1r_decode_done_fun = NULL;
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_cancel					*/
/* Send cancel request.  Returns a pointer to the same request control	*/
/* structure that was input unless the cancel request does not get sent.*/
/************************************************************************/

MMSREQ_PEND *mp_cancel (MMSREQ_PEND *reqptr)
  {
ST_UCHAR	*msg_ptr;
ST_INT	msg_len;
LLP_ADDR_INFO add_llp_info;
ASN1_ENC_CTXT localEncCtx;	/* For readability, use "aCtx" to access this.*/
ASN1_ENC_CTXT *aCtx = &localEncCtx;

  if (reqptr -> op > MAX_MMS_OPCODE)
    {
    mms_op_err = MVE_WRONG_OP;
    return ((MMSREQ_PEND *)0);
    }

  S_LOCK_COMMON_RESOURCES ();
  msg_ptr = _ms_mk_cancel (aCtx, SD_TRUE,m_build_buf,mms_max_msgsize,reqptr -> id);
  msg_len = (ST_INT) (M_BUILD_BUF_END - msg_ptr); /* find length of message	*/

  add_llp_info.i.LLC.use_rem_addr = SD_FALSE;
  add_llp_info.i.LLC.send_type = CONFIRMED_REQ_PDU;
  add_llp_info.i.LLC.req_ptr = (ST_CHAR *)reqptr;

  if (aCtx->asn1r_encode_overrun)		/* Check for encode overrun	*/
    {
    mms_op_err = ME_ASN1_ENCODE_OVERRUN;
    S_UNLOCK_COMMON_RESOURCES ();
    return ((MMSREQ_PEND *)NULL);		/* return error code	*/
    }
  else if ((mms_op_err = _mms_send_data (reqptr->chan, msg_len, msg_ptr, 
		reqptr->context, &add_llp_info)) != 0)
    {
    S_UNLOCK_COMMON_RESOURCES ();
    return ((MMSREQ_PEND *)NULL);		/* return error code	*/
    }
  else
    {		/* Adjust state of service request to be cancelled.	*/
    reqptr -> cancl_state = CANCEL_REQUESTED;
    S_UNLOCK_COMMON_RESOURCES ();
    return (reqptr);
    }
  }

#else
/************************************************************************/
/************************************************************************/
/*			mpl_cancel					*/
/* Send cancel request.  Returns a pointer to the same request control	*/
/* structure that was input unless the cancel request does not get sent.*/
/************************************************************************/

ST_RET mpl_cancel (ST_VOID)
  {
ST_RET ret;
ASN1_ENC_CTXT localEncCtx;	/* For readability, use "aCtx" to access this.*/
ASN1_ENC_CTXT *aCtx = &localEncCtx;

  mmsl_msg_start = _ms_mk_cancel (aCtx, SD_TRUE, mmsl_enc_buf, mmsl_enc_buf_size, 
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
