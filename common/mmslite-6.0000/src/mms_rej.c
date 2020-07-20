/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2012, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : mms_rej.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the functions to decode/encode a reject	*/
/*	operation.							*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 01/19/12  EJV           _WIN64 added typecasts to elim warnings.	*/
/* 11/23/11  KCR           Added casting for proper 64bit compile       */
/* 02/24/10  JRB	   Del _mmsdec_rslt, use aCtx->mmsdec_rslt	*/
/* 03/11/04  GLB     06    Cleaned up "thisFileName"                    */
/* 01/18/02  JRB     05    Fix encoding if !DEBUG_SISCO (use new fcts	*/
/*			   _ms_fin_pdu & _ms_fin_pdu_log.		*/
/* 12/20/01  JRB     04    Converted to use ASN1R (re-entrant ASN1)	*/
/* 09/13/99  MDE     03    Added SD_CONST modifiers			*/
/* 03/23/99  MDE     02    Changes to decode buffer allocation scheme	*/
/* 07/03/97  MDE     01    Op-specific info buffer handling changes	*/
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

static  REJECT_RESP_INFO	*rej_info_ptr;

static ST_VOID rej_get_problem (ASN1_DEC_CTXT *aCtx, ST_UINT16 id_code);


/************************************************************************/
/************************************************************************/
/*			mms_reject					*/
/* Reject PDU is being decoded, and this is a context-specific element. */
/* It should be an integer.						*/
/************************************************************************/

ST_VOID mms_reject (ASN1_DEC_CTXT *aCtx, ST_UINT16 id_code)
  {
  MLOG_CDEC0 ("mms_reject");

  if (aCtx->asn1r_constr_elmnt)
    {
    asn1r_set_dec_err (aCtx, PDU_INVALID);
    return;
    }

  rej_info_ptr = (REJECT_RESP_INFO *) 	_m_get_dec_buf (aCtx, sizeof (REJECT_RESP_INFO));
  aCtx->asn1r_decode_method = ASN1_CLASS_METHOD; 
  rej_info_ptr -> detected_here = SD_FALSE;   /* reject detected remotely	*/
  rej_info_ptr -> pdu_type = UNKNOWNTYPE;  /* in this case, no pdu type */

  if (!id_code)				/* if this is invoke id -	*/
    {
    if (asn1r_get_u32 (aCtx, &rej_info_ptr -> invoke)) /* read invoke id 	*/
      asn1r_set_dec_err (aCtx, PDU_INVALID);
    else
      {
      rej_info_ptr -> invoke_known = SD_TRUE;	/* set flag 		*/
      aCtx->mmsdec_rslt->dec_level = 2;
      aCtx->mmsdec_rslt->id = rej_info_ptr->invoke;	/* store invoke ID*/
      aCtx->asn1r_c_id_fun = rej_get_problem;
      }
    }
  else
    {
    rej_info_ptr -> invoke_known = SD_FALSE;	/* clear		*/
    rej_get_problem (aCtx, id_code);		/* call rejectreason fun direct	*/
    }
  }

/************************************************************************/
/*			rej_get_problem					*/
/* Reject PDU is being decoded, and this is a context-specific element. */
/* It should be an integer in which the tag is the error class and the	*/
/* value is the error code.						*/
/************************************************************************/

static ST_VOID rej_get_problem (ASN1_DEC_CTXT *aCtx, ST_UINT16 id_code)
  {
  MLOG_CDEC0 ("rej_get_problem");

  if (aCtx->asn1r_constr_elmnt || id_code < 1 || id_code > 11 || aCtx->asn1r_elmnt_len > 1)
    {					/* better not be constructor	*/
    asn1r_set_dec_err (aCtx, PDU_INVALID);		/* better be context tagged 1-4 */
    return;				/* max value = 8		*/
    }

/* Information looks OK; extract it.					*/

  rej_info_ptr -> rej_class = id_code;
  asn1r_get_i16 (aCtx, &rej_info_ptr -> rej_code);	/* get the problem value	*/

  aCtx->asn1r_decode_done_fun = _mms_dec_done_ok;	/* all done now 		*/
  aCtx->asn1r_decode_done = SD_TRUE;
  }

#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_reject_resp					*/
/* Sends reject message.  Returns integer value indicating success	*/
/* (value = 0) or failure (value <> 0) to send the reject message.	*/
/************************************************************************/

ST_RET mp_reject_resp (ST_INT chan, REJECT_RESP_INFO *info_ptr)
  {
ST_UCHAR	*msg_ptr;
ST_INT		msg_len;
LLP_ADDR_INFO add_llp_info;
ST_RET ret;
ASN1_ENC_CTXT localEncCtx;	/* For readability, use "aCtx" to access this.*/
ASN1_ENC_CTXT *aCtx = &localEncCtx;

  S_LOCK_COMMON_RESOURCES ();
  msg_ptr = _ms_mk_reject (aCtx, m_build_buf,mms_max_msgsize,info_ptr);
  msg_len = (ST_INT) (M_BUILD_BUF_END - msg_ptr); 	/* find length		*/

  add_llp_info.i.LLC.use_rem_addr = SD_FALSE;
  add_llp_info.i.LLC.send_type = REJECT_PDU;
  add_llp_info.i.LLC.req_ptr = (ST_CHAR *)NULL;

/* Check for encode overrun */
  if (aCtx->asn1r_encode_overrun)
    ret = ME_ASN1_ENCODE_OVERRUN;
  else
    ret = _mms_send_data (chan, msg_len, msg_ptr, P_context_sel[chan], &add_llp_info);

  S_UNLOCK_COMMON_RESOURCES ();
  return (ret);
  }

#else
/************************************************************************/
/************************************************************************/
/*			mpl_reject_resp					*/
/* Sends reject message.  Returns integer value indicating success	*/
/* (value = 0) or failure (value <> 0) to send the reject message.	*/
/************************************************************************/

ST_RET mpl_reject_resp (REJECT_RESP_INFO *info_ptr)
  {
ST_RET ret;
ASN1_ENC_CTXT localEncCtx;	/* For readability, use "aCtx" to access this.*/
ASN1_ENC_CTXT *aCtx = &localEncCtx;

  S_LOCK_COMMON_RESOURCES ();
  mmsl_msg_start = _ms_mk_reject (aCtx, mmsl_enc_buf,mmsl_enc_buf_size,info_ptr);
  						/* find length		*/
  mmsl_msg_len = (int) ((mmsl_enc_buf + mmsl_enc_buf_size) - mmsl_msg_start);

  if (aCtx->asn1r_encode_overrun)		/* Check for encode overrun	*/
    ret = ME_ASN1_ENCODE_OVERRUN;
  else
    ret = SD_SUCCESS;
  S_UNLOCK_COMMON_RESOURCES ();
  return (ret);
  }
#endif

/************************************************************************/
/*			_ms_mk_reject					*/
/* Construct a reject PDU.						*/
/************************************************************************/

ST_UCHAR *_ms_mk_reject (ASN1_ENC_CTXT *aCtx, ST_UCHAR *msg_ptr, ST_INT buf_len,
	REJECT_RESP_INFO *info_ptr)
  {
ST_UCHAR *pduStart;
ST_INT pduLen;

  asn1r_strt_asn1_bld (aCtx, msg_ptr,buf_len);	/* init the ASN.1 tools 	*/

  asn1r_wr_i16 (aCtx, info_ptr -> rej_code);
  asn1r_fin_prim (aCtx, info_ptr -> rej_class,CTX);

#ifndef MMS_LITE
  if (info_ptr->invoke_known && info_ptr->invoke <= MAX_INVOKE)
#else
  if (info_ptr->invoke_known)
#endif
    {
    asn1r_wr_u32 (aCtx, info_ptr -> invoke);	/* if the invoke ID is known &	*/
    asn1r_fin_prim (aCtx, 0,CTX);			/* is not artificial, write it	*/
    }

  pduStart = _ms_fin_pdu (aCtx, MMSREJECT, &pduLen);
#ifdef DEBUG_SISCO
  _ms_fin_pdu_log (MMSREJECT, "REJECT", pduStart, pduLen);
#endif

  return (pduStart);
  }

#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			   _ms_reject_resp				*/
/* Function to reject an MMS confirmed response pdu, given the request	*/
/* control structure. This must be a valid response, parsed OK. 	*/
/* Called by the user or virtual machine after the confirm is matched.	*/
/************************************************************************/

ST_VOID _ms_reject_resp (MMSREQ_PEND *reqptr, ST_INT16 code)
  {
REJECT_RESP_INFO info;		/* reject input info structure	*/

  info.detected_here = SD_TRUE;		/* parse error detected locally */

  info.invoke_known = SD_TRUE;
  info.invoke = reqptr->id;

  info.pdu_type = MMSRESP;		/* confirmed response		*/
  info.rej_code = code; 		/* include error code		*/
  info.rej_class = 2;			/* invalid response class	*/

  if (mp_reject_resp (reqptr->chan,&info)) /* send the reject pdu	*/
    (*u_mmsexcept_ind_fun)(reqptr->chan,MX_REJECT);
  (*u_reject_ind_fun) (reqptr->chan,&info);
  }

/************************************************************************/
/*			   mp_reject_ind				*/
/************************************************************************/

ST_RET mp_reject_ind (MMSREQ_IND *req_ptr, REJECT_RESP_INFO *info_ptr)
  {
ST_RET	ret;				/* function return value	*/

  S_LOCK_COMMON_RESOURCES ();
  if (!(ret = _ms_check_ind_actv (req_ptr)))
    {
    if (!(ret = mp_reject_resp (req_ptr->chan, info_ptr)))
      _ms_ind_serve_done (req_ptr);
    }
  S_UNLOCK_COMMON_RESOURCES ();
  return (ret);
  }

/************************************************************************/
/*			   mp_reject_conf				*/
/************************************************************************/

ST_RET mp_reject_conf (MMSREQ_PEND *req_ptr, REJECT_RESP_INFO *info_ptr)
  {
ST_RET	ret;				/* function return value	*/

  S_LOCK_COMMON_RESOURCES ();
  if (!(ret = _ms_check_req_actv (req_ptr)))
    {
    ret = mp_reject_resp (req_ptr->chan, info_ptr);
    }
  S_UNLOCK_COMMON_RESOURCES ();
  return (ret);
  }

#endif
