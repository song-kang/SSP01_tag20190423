/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	1996 - 1996, All Rights Reserved		        */
/*									*/
/* MODULE NAME : s_jinit.c    						*/
/* PRODUCT(S)  : MMSEASE-LITE						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*	_mvl_process_jinit_ind						*/
/*	mplas_jinit_resp						*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 04/09/07  MDE     11    Enhanced filtered logging 			*/
/* 03/11/04  GLB     10    Remove "thisFileName"                    	*/
/* 09/21/01  JRB     09    Alloc global bufs only once at startup.	*/
/* 01/21/00  MDE     08    Now use MEM_SMEM for dynamic memory		*/
/* 09/13/99  MDE     07    Added SD_CONST modifiers			*/
/* 04/01/99  MDE     06    Changes to decode buffer allocation scheme   */
/* 11/16/98  MDE     05    Renamed internal functions (prefix '_')	*/
/* 09/21/98  MDE     04    Minor lint cleanup				*/
/* 06/15/98  MDE     03    Changes to allow compile under C++		*/
/* 05/14/98  JRB     02    Use MVL_IND_PEND. Add mvlas resp function.	*/
/* 11/04/97  MDE     01    Created					*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "mmsdefs.h"
#include "mms_pvmd.h"
#include "mvl_defs.h"
#include "mvl_log.h"


/************************************************************************/
/************************************************************************/
/*			_mvl_process_jinit_ind				*/
/* A Initialize Journal indication has been rcved, process and respond	*/
/************************************************************************/

ST_VOID _mvl_process_jinit_ind (MVL_IND_PEND *indCtrl)
  {
  indCtrl->u.jinit.req_info =
        (JINIT_REQ_INFO *) indCtrl->event->u.mms.dec_rslt.data_ptr;

  u_mvl_jinit_ind (indCtrl);
  }

/************************************************************************/
/*			mplas_jinit_resp				*/
/************************************************************************/
ST_VOID mplas_jinit_resp (MVL_IND_PEND *indCtrl)
  {
ST_RET rc;

  /* Build a jinit response 						*/
  rc = mpl_jinit_resp (indCtrl->event->u.mms.dec_rslt.id,
                       indCtrl->u.jinit.resp_info);
  if (rc == SD_SUCCESS)
    mpl_resp_log (indCtrl, indCtrl->u.jinit.resp_info);

  _mvl_send_resp_i (indCtrl, rc);
  }

