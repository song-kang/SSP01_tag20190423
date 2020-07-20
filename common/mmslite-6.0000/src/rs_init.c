/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2012, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : rs_init.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the responder portion of the initiate	*/
/*	operation.  It decodes the initiate request (indication) and	*/
/*	encodes the initiate response.					*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 01/19/12  EJV           _WIN64 added typecasts to elim warnings.	*/
/* 11/23/11  KCR           Added casting for proper 64bit compile       */
/* 11/01/07  EJV     09    Rem obsolete LLC30... code.			*/
/* 03/09/05  EJV     08    Fixed thisFileName for MMS-EASE              */
/* 03/11/04  GLB     07    Cleaned up "thisFileName"                    */
/* 12/20/01  JRB     06    Converted to use ASN1R (re-entrant ASN1)	*/
/* 09/08/00  RKR     05    Log error response for negative initiate resp*/
/*                         instead of calling m_log_resp_info_fun.	*/
/* 09/13/99  MDE     04    Added SD_CONST modifiers			*/
/* 04/07/98  MDE     01    Minor restructure to eliminate warnings	*/
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
/************************************************************************/
/*			mms_init_req					*/
/* Function called to decode an Initiate request PDU after the PDU type */
/* has been determined.  Sets up the parse of the next data element and */
/* returns, letting the ASN.1 tools take over.				*/
/*									*/
/* NOTE : This functionality has been moved to mmsinit.c, because the	*/
/*   request and response are identical.  The call to mms_init_req was	*/
/*   put back to better support MMSEASE-LITE.				*/
/************************************************************************/

ST_VOID mms_init_req (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("decode initiate request");
  _mms_init_dec (aCtx, SD_TRUE);
  }


#ifndef MMS_LITE
/************************************************************************/
/************************************************************************/
/*			mp_init_resp					*/
/* create and send an initiate response 				*/
/************************************************************************/

ST_RET mp_init_resp (MMSREQ_IND *indptr, INIT_INFO *info)
  {
ST_UCHAR *msg_ptr;
ST_RET  ret;
ST_INT  i;
ST_INT  num_ui;
INIT_UI *ui;
INIT_UI *ui_ptr;
ST_INT  cs_count;
CS_INIT_INFO *cs_info_ptr;
ST_UCHAR *cs_build_bufs[MAX_INIT_CONTEXTS];
ST_UCHAR *buf;
ST_BOOLEAN  context_accepted;
ST_RET  result;
ST_INT16  reason;
ST_INT16  eclass;
ST_INT16  code;
ASN1_ENC_CTXT localEncCtx;	/* For readability, use "aCtx" to access this.*/
ASN1_ENC_CTXT *aCtx = &localEncCtx;

  if (info->num_cs_init > MAX_INIT_CONTEXTS)
    return (ME_TOO_MANY_CONTEXTS);

  S_LOCK_COMMON_RESOURCES ();
  context_accepted = SD_FALSE;			/* clear flag		*/

  if (!(ret = _ms_check_ind_actv (indptr)))
    {
    num_ui = info->num_cs_init;
    if (info->mms_p_context_pres)
      num_ui++;

    if (num_ui)
      ui = ui_ptr = (INIT_UI *) chk_calloc (num_ui, sizeof (INIT_UI));
    else 
      ui = NULL;

    cs_count = 0;
    cs_info_ptr = (CS_INIT_INFO *) (info + 1);
    for (i = 0; i < num_ui; ++i, ++ui_ptr)
      {
/* build the core initiate, if used and position reached or if no 	*/
/* companion standard initiates are to be sent				*/

      if ((info->mms_p_context_pres && i == info->core_position) ||
           !info->num_cs_init)
        {
        if (info->rslt.accept_context)
          {
          context_accepted = SD_TRUE; 		/* set flag		*/
          msg_ptr = _ms_mk_init (aCtx, m_build_buf,mms_max_msgsize,info,MMSINITRESP,SD_FALSE);
          }  
        else
          {
          msg_ptr = _ms_mk_err_resp (aCtx, m_build_buf,mms_max_msgsize,indptr->op,indptr->id,
		info->rslt.err.eclass,info->rslt.err.code);
          eclass = info->rslt.err.eclass;
          code = info->rslt.err.code;
          }
        ui_ptr->context = MMS_PCI;
        if (info->rslt.accept_context)
          ui_ptr->reject = SD_FALSE;
        else
          ui_ptr->reject = SD_TRUE;
        ui_ptr->ui_len = (ST_INT) (M_BUILD_BUF_END - msg_ptr);
        }
      else
        {
        buf = cs_build_bufs[cs_count++] = (ST_UCHAR *)
                                          chk_calloc (1,M_INIT_BUILD_LEN);
        if (cs_info_ptr->rslt.accept_context)
          {
          context_accepted = SD_TRUE; 		/* set flag		*/
          msg_ptr = _ms_mk_init (aCtx, buf,M_INIT_BUILD_LEN, 
	    	(INIT_INFO *) cs_info_ptr,MMSINITRESP,SD_TRUE);
          }
        else
          {
          msg_ptr = _ms_mk_err_resp (aCtx, buf,M_INIT_BUILD_LEN,indptr->op,indptr->id,
		cs_info_ptr->rslt.err.eclass,cs_info_ptr->rslt.err.code);
          eclass = cs_info_ptr->rslt.err.eclass;
          code = cs_info_ptr->rslt.err.code;
          }

        ui_ptr->context = cs_info_ptr->p_context;
        if (cs_info_ptr->rslt.accept_context)
          ui_ptr->reject = SD_FALSE;
        else
          ui_ptr->reject = SD_TRUE;
        ui_ptr->ui_len = (ST_INT) ((buf + M_INIT_BUILD_LEN) - msg_ptr);
        }
      ui_ptr->ui_ptr = msg_ptr;
      }

/* Now set up the ACSE result & reason parameteds			*/
    if (context_accepted)	/* if any context accepted		*/
      {
      result = 0;
      reason = 0;
      }
    else			/* no context was accepted		*/
      {
      result = 1;
      reason = 1;
      }

  /* If a response logging function is installed, call it		*/
    
    if (result==0)
      {
      if (m_log_resp_info_fun && (mms_debug_sel & MMS_LOG_RESP))
        (*m_log_resp_info_fun) (indptr, info);
      }
    else
      {
      if (m_log_error_resp_fun && (mms_debug_sel & MMS_LOG_RESP))
        (*m_log_error_resp_fun) (indptr, eclass, code);
      }

    ret = _mllp_init_resp (indptr->chan,result,reason,
		num_ui, ui,indptr->add_addr_info);

    _ms_ind_serve_done (indptr);

    if (ui)
      chk_free (ui);

    for (i = 0; i < info->num_cs_init; ++i) /* free CS build buffers	*/
      chk_free (cs_build_bufs[i]);
    }
  S_UNLOCK_COMMON_RESOURCES ();
  return (ret);
  }

#else
/************************************************************************/
/************************************************************************/
/*			mpl_init_resp					*/
/* create an initiate response 						*/
/************************************************************************/

ST_RET mpl_init_resp (INIT_INFO *info_ptr)
  {
ST_RET ret;
ASN1_ENC_CTXT localEncCtx;	/* For readability, use "aCtx" to access this.*/
ASN1_ENC_CTXT *aCtx = &localEncCtx;

  S_LOCK_COMMON_RESOURCES ();

  mmsl_msg_start = _ms_mk_init (aCtx, mmsl_enc_buf, mmsl_enc_buf_size, 
		  	info_ptr,MMSINITRESP,SD_FALSE);
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
/*			mpl_cs_init_resp				*/
/* create a companion standard initiate response			*/
/************************************************************************/

ST_RET mpl_cs_init_resp (CS_INIT_INFO *info_ptr)
  {
ASN1_ENC_CTXT localEncCtx;	/* For readability, use "aCtx" to access this.*/
ASN1_ENC_CTXT *aCtx = &localEncCtx;

  mmsl_msg_start = _ms_mk_init (aCtx, mmsl_enc_buf, mmsl_enc_buf_size, 
		  	(INIT_INFO *) info_ptr,MMSINITRESP,SD_TRUE);
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
