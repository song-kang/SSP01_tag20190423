/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	1994 - 1996, All Rights Reserved		        */
/*									*/
/* MODULE NAME : s_cancel.c    						*/
/* PRODUCT(S)  : MMSEASE-LITE						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*	NONE								*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 03/11/04  GLB     09    Remove "thisFileName"                    	*/
/* 09/21/01  JRB     08    Alloc global bufs only once at startup.	*/
/* 01/21/00  MDE     07    Now use MEM_SMEM for dynamic memory		*/
/* 09/13/99  MDE     06    Added SD_CONST modifiers			*/
/* 04/01/99  MDE     05    Changes to decode buffer allocation scheme   */
/* 11/16/98  MDE     04    Renamed internal functions (prefix '_')	*/
/* 09/21/98  MDE     03    Minor lint cleanup				*/
/* 06/15/98  MDE     02    Changes to allow compile under C++		*/
/* 09/10/97  MDE     01    MMS-LITE V4.0 Release			*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "mmsdefs.h"
#include "mms_pvmd.h"

#include "mvl_defs.h"
#include "mvl_log.h"


/************************************************************************/
/************************************************************************/
/*			_mvl_process_cancel_ind				*/
/* A MMS MMSOP_CANCEL request has been received. For now, just send error	*/
/************************************************************************/

ST_VOID _mvl_process_cancel_ind (MVL_COMM_EVENT *event)
  {
ST_RET rc;

/* Build cancel error response message, 				*/
/* class = Cancel, code = 'Cancel Not Possible'				*/
  rc = mpl_cancel_err (event->u.mms.dec_rslt.id, 10, 2);
  _mvl_send_resp (event, rc);
  }

