/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	1994 - 1996, All Rights Reserved		        */
/*									*/
/* MODULE NAME : c_ident.c                                              */
/* PRODUCT(S)  : MMSEASE-LITE						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*	mvla_identify							*/
/*	_mvl_process_ident_conf						*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 03/02/10  JRB	   Del synchronous client functions.		*/
/* 01/14/09  MDE     13    Added test probe to send bad identify	*/
/* 04/09/07  MDE     12    Enhanced filtered logging 			*/
/* 11/01/01  JRB     11    mvl_wait_req_done now returns ST_RET.	*/
/* 09/21/01  JRB     10    Alloc global bufs only once at startup.	*/
/* 03/07/01  JRB     09    Del LOCK on sync functs: caused deadlocks.	*/
/* 01/21/00  MDE     08    Now use MEM_SMEM for dynamic memory		*/
/* 09/13/99  MDE     07    Added SD_CONST modifiers			*/
/* 11/17/98  MDE     06    Now use MVL_ERR_COMM_SERVE_ACTIVE error code	*/
/*			   Use MVL_SYNC_REQ_INTERFACE			*/
/* 11/16/98  MDE     05    Renamed internal functions (prefix '_')	*/
/* 06/15/98  MDE     04    Changes to allow compile under C++		*/
/* 05/22/98  JRB     03    Pass (MVL_REQ_PEND**) to sync function also.	*/
/*			   Do not pass resp info ptr. Confirm sets it.	*/
/* 04/16/98  JRB     02    Conf func: Let caller do "common" stuff.	*/
/*			   Always set (*req_out) so user can always free*/
/* 09/10/97  MDE     01    MMS-LITE V4.0 Release			*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "glbsem.h"

#include "mmsdefs.h"
#include "mms_pvmd.h"
#include "mvl_defs.h"
#include "mvl_log.h"

#if defined(_S_TEST_PROBES)
#include "stestprb.h"
#endif

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/
/************************************************************************/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/
/*			mvla_identify					*/
/************************************************************************/

ST_RET mvla_identify (MVL_NET_INFO  *net_info,
		     MVL_REQ_PEND **req_out)
  {
ST_RET rc;
MVL_REQ_PEND *req;

  *req_out = req = _mvl_get_req_ctrl (net_info, MMSOP_IDENTIFY);
  if (!req)
    return (MVL_ERR_REQ_CONTROL);

  S_LOCK_COMMON_RESOURCES ();
  
  /* Build a identify request               */
  rc = mpl_ident();
  if (rc == SD_SUCCESS)
    mpl_req_log (req, NULL);

#if defined(_S_TEST_PROBES)
 if (_sTestProbeEnabledS("MvlSendBadIdentify"))
    mmsl_msg_start[5] = 0xA2;
#endif

  rc = _mvl_send_req (net_info, req, rc);

  S_UNLOCK_COMMON_RESOURCES ();
  return (rc);
  }                                  

/************************************************************************/
/*			_mvl_process_ident_conf				*/
/************************************************************************/
ST_VOID _mvl_process_ident_conf (MVL_REQ_PEND *req_pend)
  {
  req_pend->u.ident.resp_info = 
      (IDENT_RESP_INFO *) req_pend->event->u.mms.dec_rslt.data_ptr;
  }
