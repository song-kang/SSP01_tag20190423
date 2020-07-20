/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	1998 - 1998, All Rights Reserved				*/
/*									*/
/* MODULE NAME : c_getnam.c						*/
/* PRODUCT(S)  : MMSEASE-LITE						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*	mvla_getnam							*/
/*	_mvl_process_getnam_conf					*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev     Comments					*/
/* --------  ---  ------   -------------------------------------------	*/
/* 03/02/10  JRB	   Del synchronous client functions.		*/
/* 04/09/07  MDE     13    Enhanced filtered logging 			*/
/* 11/01/01  JRB     12    mvl_wait_req_done now returns ST_RET.	*/
/* 09/21/01  JRB     11    Alloc global bufs only once at startup.	*/
/* 03/07/01  JRB     10    Del LOCK on sync functs: caused deadlocks.	*/
/* 03/08/00  JRB     09    Del #ifdef MVL_REQ_BUF_ENABLED: not used.	*/
/* 			   Del mvl_free_req_ctrl call. User must call.	*/
/*			   Del invoke_id, numpend_req: see mvl_send_req.*/
/* 01/21/00  MDE     08    Now use MEM_SMEM for dynamic memory		*/
/* 01/21/00  MDE     07    Use '_mvl_get_req_buf' for MVL_REQ_BUF_EN..	*/
/* 11/19/99  NAV     06	   Add #ifdef MVL_REQ_BUF_ENABLED		*/
/* 10/20/99  NAV     05    Add maxpend_req support to aynsc functions	*/
/* 09/13/99  MDE     04    Added SD_CONST modifiers			*/
/* 11/17/98  MDE     03    Now use MVL_ERR_COMM_SERVE_ACTIVE error code	*/
/*			   Use MVL_SYNC_REQ_INTERFACE			*/
/* 11/16/98  MDE     02    Renamed internal functions (prefix '_')	*/
/* 10/01/98  DWL     01    Created from c_getdom.c			*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "glbsem.h"

#include "mmsdefs.h"
#include "mms_pvmd.h"
#include "mvl_defs.h"
#include "mvl_log.h"

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/
/************************************************************************/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/
/*			mvla_getnam					*/
/************************************************************************/

ST_RET mvla_getnam (
	MVL_NET_INFO  *net_info,
	NAMELIST_REQ_INFO *req_info,
	MVL_REQ_PEND **req_out)
  {
ST_RET rc;
MVL_REQ_PEND *req_pend;

  *req_out = req_pend = _mvl_get_req_ctrl (net_info, MMSOP_GET_NAMLIST);
  if (!req_pend)
    return (MVL_ERR_REQ_CONTROL);

  S_LOCK_COMMON_RESOURCES ();
  rc = _mvla_send_getname (net_info, req_info, req_pend);
  S_UNLOCK_COMMON_RESOURCES ();
  return (rc);
  }                                  

/************************************************************************/
/*			_mvla_send_getname				*/
/************************************************************************/

ST_RET _mvla_send_getname (MVL_NET_INFO  *net_info,
    			   NAMELIST_REQ_INFO *req_info,
			   MVL_REQ_PEND *req_pend)
  {
ST_RET rc;

  /* Build a getvar request               */
  rc = mpl_namelist (req_info);
  if (rc == SD_SUCCESS)
    mpl_req_log (req_pend, req_info);

  rc = _mvl_send_req (net_info, req_pend, rc);
  return (rc);
  }

/************************************************************************/
/*			_mvl_process_getnam_conf				*/
/************************************************************************/

ST_VOID _mvl_process_getnam_conf (MVL_REQ_PEND *req_pend)
  {
  req_pend->u.getnam.resp_info = 
	(NAMELIST_RESP_INFO *) req_pend->event->u.mms.dec_rslt.data_ptr;
  }

