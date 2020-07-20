/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1993 - 2012, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : mms_err4.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the MMS error functions for decoding	*/
/*	& encoding the error response PDUs :				*/
/*		cancel-ErrorPDU						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 01/19/12  EJV           _WIN64 added typecasts to elim warnings.	*/
/* 11/01/07  EJV     07    Rem obsolete LLC30... code.			*/
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
#include "mms_pcon.h"
#include "asn1defs.h"

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/

#if defined(DEBUG_SISCO) && (defined(S_MT_SUPPORT) && !defined(MMS_LITE))
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

#ifndef MMS_LITE
/************************************************************************/
/*			 mp_cancel_err					*/
/* Create and send a cancel error response				*/
/************************************************************************/

ST_RET mp_cancel_err (MMSREQ_IND *indptr, ST_INT16 err_class, ST_INT16 code)
  {
ST_UCHAR *msg_ptr;			/* pointer to start of message	*/
ST_INT	msg_len;			/* length of message built	*/
ST_RET	ret;				/* function return value	*/
ASN1_ENC_CTXT localEncCtx;	/* For readability, use "aCtx" to access this.*/
ASN1_ENC_CTXT *aCtx = &localEncCtx;

  S_LOCK_COMMON_RESOURCES ();
  if (!(ret = _ms_check_ind_actv (indptr)))
    {
    if (indptr->cancl_state != CANCELING)
      {
      S_UNLOCK_COMMON_RESOURCES ();
      return (MVE_CANCEL_STATE);
      }
    msg_ptr = _ms_mk_err_resp (aCtx, m_build_buf,mms_max_msgsize,MMSOP_CANCEL,indptr->id,
				err_class,code);
    msg_len = (ST_INT) (M_BUILD_BUF_END - msg_ptr); 		/* find length		*/

    if (aCtx->asn1r_encode_overrun)		/* Check for encode overrun	*/
      ret = ME_ASN1_ENCODE_OVERRUN;
    else
      ret = _mms_send_data (indptr->chan, msg_len, msg_ptr, 
		indptr->context, indptr->add_addr_info);
    }
  S_UNLOCK_COMMON_RESOURCES ();
  return (ret);
  }

#else
/************************************************************************/
/*			 mpl_cancel_err					*/
/* Create a cancel error response					*/
/************************************************************************/

ST_RET mpl_cancel_err (ST_UINT32 invoke, ST_INT16 err_class, ST_INT16 code)
  {
  return (mpl_err_resp (MMSOP_CANCEL,invoke,err_class,code));
  }

#endif
