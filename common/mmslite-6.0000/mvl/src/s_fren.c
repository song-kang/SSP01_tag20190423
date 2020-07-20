/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	1998 - 1998, All Rights Reserved				*/
/*									*/
/* MODULE NAME : s_fren.c						*/
/* PRODUCT(S)  : MMSEASE-LITE						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*	_mvl_process_frename_ind					*/
/*	mplas_frename_resp						*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev     Comments					*/
/* --------  ---  ------   -------------------------------------------	*/
/* 04/09/07  MDE     05    Enhanced filtered logging 			*/
/* 03/11/04  GLB     04    Remove "thisFileName"                    	*/
/* 09/21/01  JRB     03    Alloc global bufs only once at startup.	*/
/* 01/21/00  MDE     02    Now use MEM_SMEM for dynamic memory		*/
/* 10/15/99  GLB     01    Created to acommodate file Rename            */
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "mmsdefs.h"
#include "mms_pfil.h"
#include "mvl_defs.h"
#include "mvl_log.h"

/************************************************************************/
/*			_mvl_process_frename_ind				*/
/* A "frename" indication has been received, process it and respond	*/
/************************************************************************/

ST_VOID _mvl_process_frename_ind (MVL_IND_PEND *indCtrl)
  {
FRENAME_REQ_INFO *req_info;
MVLAS_FRENAME_CTRL *frename;
FILE_NAME *fname_idx;

  frename = &indCtrl->u.frename;

  req_info = (FRENAME_REQ_INFO *) indCtrl->event->u.mms.dec_rslt.data_ptr;
  fname_idx = (FILE_NAME *)(req_info + 1);

  if (_mvl_concat_filename (frename->curfilename, req_info->num_of_cur_fname, 
                            fname_idx, sizeof (frename->curfilename)))
    {
    _mplas_err_resp (indCtrl,11,3);	/* File name syntax error	*/
    return;
    }

  if (_mvl_concat_filename (frename->newfilename, req_info->num_of_new_fname, 
                    (FILE_NAME *)(fname_idx + req_info->num_of_cur_fname), 
                    sizeof (frename->newfilename)))
    {
    _mplas_err_resp (indCtrl,11,3);	/* File name syntax error	*/
    return;
    }

  u_mvl_frename_ind (indCtrl);
  }
    
/************************************************************************/
/*			mplas_frename_resp				*/
/************************************************************************/
ST_VOID mplas_frename_resp (MVL_IND_PEND *indCtrl)
  {
ST_RET rc;

  /* Build a "frename" response 					*/
  rc = mpl_frename_resp (indCtrl->event->u.mms.dec_rslt.id);
  if (rc == SD_SUCCESS)
    mpl_resp_log (indCtrl, NULL);

  _mvl_send_resp_i (indCtrl, rc);
  }


