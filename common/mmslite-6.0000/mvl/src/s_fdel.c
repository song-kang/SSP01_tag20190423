/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	1998 - 1998, All Rights Reserved				*/
/*									*/
/* MODULE NAME : s_fdel.c						*/
/* PRODUCT(S)  : MMSEASE-LITE						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*	_mvl_process_fdelete_ind						*/
/*	mplas_fdelete_resp						*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev     Comments					*/
/* --------  ---  ------   -------------------------------------------	*/
/* 04/09/07  MDE     10    Enhanced filtered logging 			*/
/* 03/11/04  GLB     09    Remove "thisFileName"                    	*/
/* 09/21/01  JRB     08    Alloc global bufs only once at startup.	*/
/* 01/21/00  MDE     07    Now use MEM_SMEM for dynamic memory		*/
/* 09/13/99  MDE     06    Added SD_CONST modifiers			*/
/* 04/01/99  MDE     05    Changes to decode buffer allocation scheme   */
/* 11/16/98  MDE     04    Renamed internal functions (prefix '_')	*/
/* 09/21/98  MDE     03    Minor lint cleanup				*/
/* 06/15/98  MDE     02    Changes to allow compile under C++		*/
/* 04/14/98  JRB     01    MMS-LITE V4.0 Release			*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "mmsdefs.h"
#include "mms_pfil.h"
#include "mvl_defs.h"
#include "mvl_log.h"

/************************************************************************/
/*			_mvl_process_fdelete_ind				*/
/* A "fdelete" indication has been received, process it and respond	*/
/************************************************************************/

ST_VOID _mvl_process_fdelete_ind (MVL_IND_PEND *indCtrl)
  {
FDELETE_REQ_INFO *req_info;
MVLAS_FDELETE_CTRL *fdelete;

  fdelete = &indCtrl->u.fdelete;

  req_info = (FDELETE_REQ_INFO *) indCtrl->event->u.mms.dec_rslt.data_ptr;

  if (_mvl_concat_filename (fdelete->filename, req_info->num_of_fname, 
                    (FILE_NAME *)(req_info + 1), sizeof (fdelete->filename)))
    {
    _mplas_err_resp (indCtrl,11,3);	/* File name syntax error	*/
    return;
    }

  u_mvl_fdelete_ind (indCtrl);
  }
    
/************************************************************************/
/*			mplas_fdelete_resp				*/
/************************************************************************/
ST_VOID mplas_fdelete_resp (MVL_IND_PEND *indCtrl)
  {
ST_RET rc;

  /* Build a "fdelete" response 					*/
  rc = mpl_fdelete_resp (indCtrl->event->u.mms.dec_rslt.id);
  if (rc == SD_SUCCESS)
    mpl_resp_log (indCtrl, NULL);

  _mvl_send_resp_i (indCtrl, rc);
  }


