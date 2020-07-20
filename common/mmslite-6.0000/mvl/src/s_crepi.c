/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	2000 - 2000, All Rights Reserved				*/
/*									*/
/* MODULE NAME : s_crepi.c						*/
/* PRODUCT(S)  : MMSEASE-LITE						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*	_mvl_process_crepi_ind						*/
/*	mplas_crepi_resp						*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev     Comments					*/
/* --------  ---  ------   -------------------------------------------	*/
/* 04/09/07  MDE     03    Enhanced filtered logging 			*/
/* 09/21/01  JRB     02    Alloc global bufs only once at startup.	*/
/* 09/06/00  JRB     01    Created.					*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "mmsdefs.h"
#include "mms_pdom.h"
#include "mvl_defs.h"
#include "mvl_log.h"

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/
/************************************************************************/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/*----------------------------------------------------------------------*/
/* Functions to process CreateProgramInvocation Indication		*/
/* and send CreateProgramInvocation Response.				*/
/*----------------------------------------------------------------------*/

/************************************************************************/
/*			_mvl_process_crepi_ind				*/
/* CreateProgramInvocation Indication received.				*/
/* Process it and pass it up to user.					*/
/************************************************************************/
ST_VOID _mvl_process_crepi_ind (MVL_IND_PEND *indCtrl)
  {
  indCtrl->u.crepi.req_info = 
      (CREPI_REQ_INFO *) indCtrl->event->u.mms.dec_rslt.data_ptr;

  u_mvl_crepi_ind (indCtrl);
  }

/************************************************************************/
/*			mplas_crepi_resp				*/
/* Send CreateProgramInvocation Response.				*/
/* NOTE: no data in this response.					*/
/************************************************************************/
ST_VOID mplas_crepi_resp (MVL_IND_PEND *indCtrl)
  {
ST_RET rc;

  rc = mpl_crepi_resp (indCtrl->event->u.mms.dec_rslt.id);
  if (rc == SD_SUCCESS)
    mpl_resp_log (indCtrl, NULL);

  _mvl_send_resp_i (indCtrl, rc);
  }

