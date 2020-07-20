/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	1994 - 1996, All Rights Reserved		        */
/*									*/
/* MODULE NAME : c_concl.c                                              */
/* PRODUCT(S)  : MMSEASE-LITE						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*	mvla_concl							*/
/*	_mvl_process_concl_conf						*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 03/03/10  JRB	   Replace "ml_log_*" with "mlogl_info" struct.	*/
/* 03/02/10  JRB	   Del synchronous client functions.		*/
/* 01/30/06  GLB     16    Integrated porting changes for VMS           */
/* 10/29/02  JRB     15    _mvla_send_concl: on err, set done & result in*/
/*			   req_pend. Helps if user doesn't chk return.	*/
/* 04/25/02  MDE     14    Added MLOG 					*/
/* 11/01/01  JRB     13    mvl_wait_req_done now returns ST_RET.	*/
/* 03/07/01  JRB     12    Del LOCK on sync functs: caused deadlocks.	*/
/* 04/14/00  JRB     11    Lint cleanup.				*/
/* 03/08/00  JRB     10    Del #ifdef MVL_REQ_BUF_ENABLED: not used.	*/
/* 			   Del mvl_free_req_ctrl call. User must call.	*/
/*			   Don't care numpend_req. Conclude is special.	*/
/* 01/21/00  MDE     09    Now use MEM_SMEM for dynamic memory		*/
/* 11/22/99  NAV     08    Add maxpend_req support			*/
/* 09/13/99  MDE     07    Added SD_CONST modifiers			*/
/* 12/08/98  MDE     06    Cleanup on request control handling		*/
/* 11/17/98  MDE     05    Now use MVL_ERR_COMM_SERVE_ACTIVE error code	*/
/*			   Use MVL_SYNC_REQ_INTERFACE			*/
/* 11/16/98  MDE     04    Renamed internal functions (prefix '_')	*/
/* 11/16/98  MDE     03    Added mvl_concl, mvla_concl, call release	*/
/* 06/15/98  MDE     02    Changes to allow compile under C++		*/
/* 09/10/97  MDE     01    MMS-LITE V4.0 Release			*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "glbsem.h"

#include "mmsdefs.h"
#include "mvl_defs.h"
#include "mvl_log.h"
#include "mloguser.h"

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/
/************************************************************************/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/
/*			mvla_concl					*/
/************************************************************************/


ST_RET mvla_concl (MVL_NET_INFO *net_info, MVL_REQ_PEND **req_out)
  {
ST_RET rc;
MVL_REQ_PEND *req;

  *req_out = req = _mvl_get_req_ctrl (net_info, MMSOP_CONCLUDE);
  if (!req)
    return (MVL_ERR_REQ_CONTROL);

  S_LOCK_COMMON_RESOURCES ();
  rc = _mvla_send_concl (net_info, req);
  S_UNLOCK_COMMON_RESOURCES ();
  return (rc);
  }                                  

/************************************************************************/
/*			_mvla_send_concl				*/
/************************************************************************/

ST_RET _mvla_send_concl (MVL_NET_INFO *net_info, 
			 MVL_REQ_PEND *req_pend)
  {
ST_RET rc;
ST_UCHAR _conclude_pdu[2];

/* "\x8B\x00" == "\213\000", for old compilers ...			*/
  _conclude_pdu[0]= (ST_UCHAR) 0x8b;
  _conclude_pdu[1]= (ST_UCHAR) 0x00;

/* We can log here if so desired */
  if ((mms_debug_sel & MMS_LOG_REQ) && mlogl_info.req_info_fun != NULL)
      (*mlogl_info.req_info_fun) (0, MMSOP_CONCLUDE, NULL);

  rc = _mvl_send_msg (_conclude_pdu, 2, net_info);
  if (rc != SD_SUCCESS)
    {
    MVL_LOG_NERR1 ("Conclude : _mvl_send_msg failed, rc = %x", rc);
    req_pend->done = SD_TRUE;	/* Helps if user doesn't chk return	*/
    req_pend->result = rc;
    }
  /* invoke_id not sent on conclude, but set to -1 just to match up resp.*/
  req_pend->invoke_id = (ST_UINT32) -1;

  return (rc);
  }

/************************************************************************/
/*			_mvl_process_concl_conf				*/
/************************************************************************/

ST_VOID _mvl_process_concl_conf (MVL_REQ_PEND *req_pend)
  {
  _mvl_release (req_pend);
  }


