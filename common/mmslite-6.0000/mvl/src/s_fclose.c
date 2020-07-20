/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	1994 - 1996, All Rights Reserved		        */
/*									*/
/* MODULE NAME : s_fclose.c    						*/
/* PRODUCT(S)  : MMSEASE-LITE						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*	_mvl_process_fclose_ind						*/
/*	mplas_fclose_resp						*/
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
/* 09/10/97  MDE     01    MMS-LITE V4.0 Release			*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "mmsdefs.h"
#include "mms_pfil.h"
#include "mvl_defs.h"
#include "mvl_log.h"


/************************************************************************/
/************************************************************************/
/*			_mvl_process_fclose_ind				*/
/* A file close indication has been received, process it and respond	*/
/************************************************************************/

ST_VOID _mvl_process_fclose_ind (MVL_IND_PEND *indCtrl)
  {
  indCtrl->u.fclose.req_info =
      (FCLOSE_REQ_INFO *) indCtrl->event->u.mms.dec_rslt.data_ptr;

  u_mvl_fclose_ind (indCtrl);
  }

/************************************************************************/
/*			mplas_fclose_resp				*/
/************************************************************************/
ST_VOID mplas_fclose_resp (MVL_IND_PEND *indCtrl)
  {
ST_RET rc;

  /* Build a File Close response 					*/
  rc = mpl_fclose_resp (indCtrl->event->u.mms.dec_rslt.id);
  if (rc == SD_SUCCESS)
    mpl_resp_log (indCtrl, NULL);

  _mvl_send_resp_i (indCtrl, rc);
  }

