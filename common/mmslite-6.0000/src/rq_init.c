/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2012, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rq_init.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the requester portion of the initiate	*/
/*	operation.  It decodes the initiate response (confirm) &	*/
/*	encodes the initiate request.					*/
/*	This module makes use of common functions in mmsinit.c		*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 01/19/12  EJV           _WIN64 added typecasts to elim warnings.	*/
/* 11/23/11  KCR           Added casting for proper 64bit compile       */
/* 02/28/05  EJV     04    Elim Linux warnings in if (a=b)		*/
/*			   Fixed thisFileName for MMS-EASE   		*/	
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
#ifndef MMS_LITE
#include "mms_ms.h"
#endif
#include "asn1defs.h"

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/

#if defined(DEBUG_SISCO) && (defined(S_MT_SUPPORT) || !defined(MMS_LITE))
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/
/************************************************************************/
/*			mms_init_rsp					*/
/* Function called to decode an Initiate response PDU after the PDU	*/
/* type has been determined.  Sets request to false, and calls the	*/
/* common initiate decode function.					*/
/*									*/
/* NOTE : This functionality has been moved to mmsinit.c, because the	*/
/*   request and response are identical.  The call to mms_init_rsp was	*/
/*   put back to better support MMSEASE-LITE.				*/
/************************************************************************/

ST_VOID mms_init_rsp (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("decode initiate response");
  _mms_init_dec (aCtx, SD_FALSE);
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_init 					*/
/* create and send an initiate request. 				*/
/************************************************************************/

MMSREQ_PEND *mp_init (ST_INT chan, ST_CHAR *partner, INIT_INFO *info_ptr)
  {
ST_UCHAR  *msg_ptr;
MMSREQ_PEND *reqptr;
LLP_ADDR_INFO add_llp_info;
ST_INT  i;
ST_INT  num_ui;
INIT_UI *ui;
INIT_UI *ui_ptr;
ST_UCHAR *buf;
ST_INT16  pend_req_save;
ST_INT  num_cs;
ST_INT  cs_count;
CS_INIT_INFO *cs_info_ptr;
ST_UCHAR *cs_build_bufs[MAX_INIT_CONTEXTS];
ST_INT req_count;
ASN1_ENC_CTXT localEncCtx;	/* For readability, use "aCtx" to access this.*/
ASN1_ENC_CTXT *aCtx = &localEncCtx;

  if (info_ptr->num_cs_init > MAX_INIT_CONTEXTS)
    {
    mms_op_err = ME_TOO_MANY_CONTEXTS;
    return ((MMSREQ_PEND *) 0);
    }

/* Make sure this channel is IDLE					*/
  if (mms_chan_info[chan].ctxt.chan_state != M_IDLE)
    {
    mms_op_err = ME_CHAN_STATE;
    return ((MMSREQ_PEND *) 0);
    }

  S_LOCK_COMMON_RESOURCES ();

/* Flush all pending requests (should be 0 already, check anyway)	*/
  req_count = ms_count_req_pend (chan);
  if (req_count)
    {
    MLOG_ERR2 ("%d Pending Requests on Idle Channel %d ", 
				req_count, 
				(ST_INT) chan);
    }
  ms_disconn_pend_req (chan);


/* Need to save the max pending request count (could be 0) before	*/
/* getting the request control pointer.					*/

  pend_req_save = mms_chan_info[chan].maxpend_req;
  mms_chan_info[chan].maxpend_req = 1;

  reqptr = _ms_get_req_ptr (chan,MMSOP_INITIATE,(ST_CHAR *)info_ptr);

/* restore max pending request for initiate request			*/
  mms_chan_info[chan].maxpend_req = pend_req_save;

  if (reqptr)
    {
    _mms_pend_init[chan].req = reqptr;	/* save INITIATE request ctrl	*/

    num_ui = num_cs = info_ptr->num_cs_init;
    if (info_ptr->mms_p_context_pres)
      num_ui++;

    ui = ui_ptr = (INIT_UI *) chk_calloc (num_ui, sizeof (INIT_UI));
    cs_count = 0;
    cs_info_ptr = (CS_INIT_INFO *) (info_ptr + 1);
    for (i = 0; i < num_ui; ++i, ++ui_ptr)
      {
/* build the core initiate, if used and position reached or if no 	*/
/* companion standard initiates are to be sent				*/

      if ((info_ptr->mms_p_context_pres && i == info_ptr->core_position) ||
           !info_ptr->num_cs_init)
        {
        msg_ptr = _ms_mk_init (aCtx, m_build_buf,mms_max_msgsize,info_ptr,MMSINITREQ,SD_FALSE);
        ui_ptr->context = MMS_PCI;
        ui_ptr->ui_len = (ST_INT) (M_BUILD_BUF_END - msg_ptr);
        ui_ptr->ui_ptr = msg_ptr;
        }
      else	/* build a CS Initiate PDU				*/
        {
        buf = cs_build_bufs[cs_count++] = (ST_UCHAR *)
                                          chk_calloc (1,M_INIT_BUILD_LEN);
        msg_ptr = _ms_mk_init (aCtx, buf,M_INIT_BUILD_LEN,
			(INIT_INFO *) cs_info_ptr,MMSINITREQ,SD_TRUE);
      
        ui_ptr->context = cs_info_ptr->p_context;
        ui_ptr->ui_len = (ST_INT) ((buf + M_INIT_BUILD_LEN) - msg_ptr);
        ui_ptr->ui_ptr = msg_ptr;
        ++cs_info_ptr;
        }
      }

    add_llp_info.i.LLC.use_rem_addr = SD_FALSE;
    add_llp_info.i.LLC.req_ptr = (ST_CHAR *)reqptr;
    add_llp_info.i.LLC.send_type = INITIATE_REQ_PDU;

  /* If a request logging function is installed, call it		*/
    if (m_log_req_info_fun && (mms_debug_sel & MMS_LOG_REQ))
      (*m_log_req_info_fun) (reqptr, info_ptr);

    if ((mms_op_err = _mllp_init_req  (chan, partner, reqptr->context,
				num_ui, ui,&add_llp_info)) != 0)
      {
      _ms_deact_pend_req (reqptr);
      chk_free (ui);
      S_UNLOCK_COMMON_RESOURCES ();
      return ((MMSREQ_PEND *) 0);
      }

    chk_free (ui);
    for (i = 0; i < num_cs; ++i)  	/* free CS build buffers	*/
      chk_free (cs_build_bufs[i]);
    }
  S_UNLOCK_COMMON_RESOURCES ();
  return (reqptr);			/* return request control ptr	*/
  }

#else
/************************************************************************/
/************************************************************************/
/*			mpl_init 					*/
/* create a core initiate request. 					*/
/************************************************************************/

ST_RET mpl_init (INIT_INFO *info_ptr)
  {
ST_RET ret;
ASN1_ENC_CTXT localEncCtx;	/* For readability, use "aCtx" to access this.*/
ASN1_ENC_CTXT *aCtx = &localEncCtx;

  S_LOCK_COMMON_RESOURCES ();
  mmsl_msg_start = _ms_mk_init (aCtx, mmsl_enc_buf, mmsl_enc_buf_size, 
  			info_ptr,MMSINITREQ,SD_FALSE);
  mmsl_msg_len   = (int) ((mmsl_enc_buf + mmsl_enc_buf_size) - mmsl_msg_start);

  if (!aCtx->asn1r_encode_overrun)		/* Check for encode overrun 	*/
    ret = SD_SUCCESS;
  else
    ret = ME_ASN1_ENCODE_OVERRUN;
  S_UNLOCK_COMMON_RESOURCES ();
  return (ret);
  }

#ifdef CS_SUPPORT
/************************************************************************/
/************************************************************************/
/*			mpl_cs_init 					*/
/* create a companion standard initiate request.			*/
/************************************************************************/

ST_INT16 mpl_cs_init (CS_INIT_INFO *info_ptr)
  {
ASN1_ENC_CTXT localEncCtx;	/* For readability, use "aCtx" to access this.*/
ASN1_ENC_CTXT *aCtx = &localEncCtx;

  mmsl_msg_start = _ms_mk_init (aCtx, mmsl_enc_buf, mmsl_enc_buf_size, 
		  	(INIT_INFO *) info_ptr,MMSINITREQ,SD_TRUE);
  mmsl_msg_len   = (mmsl_enc_buf + mmsl_enc_buf_size) - mmsl_msg_start;

  if (aCtx->asn1r_encode_overrun)		/* Check for encode overrun 	*/
    return (ME_ASN1_ENCODE_OVERRUN);
  return (SD_SUCCESS);
  }
#endif

#endif

/************************************************************************/
/*			    mk_initiate					*/
/* Note that there is only one mk_initiate function, and it is listed 	*/
/* in the mmsinit.c module.						*/
/************************************************************************/

