/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	2000 - 2000, All Rights Reserved				*/
/*									*/
/* MODULE NAME : s_getcl.c						*/
/* PRODUCT(S)  : MMSEASE-LITE						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*	_mvl_process_getcl_ind						*/
/*	mplas_getcl_resp						*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev     Comments					*/
/* --------  ---  ------   -------------------------------------------	*/
/* 04/09/07  MDE     04    Enhanced filtered logging 			*/
/* 03/11/04  GLB     03    Remove "thisFileName"                    	*/
/* 09/21/01  JRB     02    Alloc global bufs only once at startup.	*/
/* 10/25/00  JRB     01    Created.					*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "mmsdefs.h"
#include "mms_pdom.h"
#include "mvl_defs.h"
#include "mvl_log.h"

/*----------------------------------------------------------------------*/
/* Functions to process GetProgramInvocationAttributes Indication	*/
/* and send GetProgramInvocationAttributes Response.			*/
/*----------------------------------------------------------------------*/

/************************************************************************/
/*			_mvl_process_getcl_ind				*/
/* GetProgramInvocationAttributes Indication received.			*/
/* Process it and pass it up to user.					*/
/************************************************************************/
ST_VOID _mvl_process_getcl_ind (MVL_IND_PEND *indCtrl)
  {
  indCtrl->u.getcl.req_info = 
      (GETCL_REQ_INFO *) indCtrl->event->u.mms.dec_rslt.data_ptr;

  u_mvl_getcl_ind (indCtrl);
  }

/************************************************************************/
/*			mplas_getcl_resp				*/
/* Send GetProgramInvocationAttributes Response.			*/
/************************************************************************/
ST_VOID mplas_getcl_resp (MVL_IND_PEND *indCtrl)
  {
ST_RET rc;

  rc = mpl_getcl_resp (indCtrl->event->u.mms.dec_rslt.id,
                          indCtrl->u.getcl.resp_info);
  if (rc == SD_SUCCESS)
    mpl_resp_log (indCtrl, indCtrl->u.getcl.resp_info);

  _mvl_send_resp_i (indCtrl, rc);
  }

