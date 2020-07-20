/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	1998 - 1998, All Rights Reserved				*/
/*									*/
/* MODULE NAME : c_fopen.c						*/
/* PRODUCT(S)  : MMSEASE-LITE						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*	mvla_fopen							*/
/*	_mvl_process_fopen_conf						*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev     Comments					*/
/* --------  ---  ------   -------------------------------------------	*/
/* 11/23/11  KCR           Added casting for proper 64bit compile       */
/* 03/02/10  JRB	   Del synchronous client functions.		*/
/* 04/09/07  MDE     14    Enhanced filtered logging 			*/
/* 11/01/01  JRB     13    mvl_wait_req_done now returns ST_RET.	*/
/* 09/21/01  JRB     12    Alloc global bufs only once at startup.	*/
/* 03/07/01  JRB     11    Del LOCK on sync functs: caused deadlocks.	*/
/* 03/08/00  JRB     10    Del #ifdef MVL_REQ_BUF_ENABLED: not used.	*/
/* 			   Del mvl_free_req_ctrl call. User must call.	*/
/*			   Del invoke_id, numpend_req: see mvl_send_req.*/
/*			   Add missing S_LOCK_...			*/
/* 01/21/00  MDE     09    Now use MEM_SMEM for dynamic memory		*/
/* 01/21/00  MDE     08    Use '_mvl_get_req_buf' for MVL_REQ_BUF_EN..	*/
/* 11/19/99  NAV     07	   Add #ifdef MVL_REQ_BUF_ENABLED		*/
/* 10/26/99  NAV     06    Add maxpend_req support to aynsc functions	*/
/* 09/13/99  MDE     05    Added SD_CONST modifiers			*/
/* 11/17/98  MDE     04    Now use MVL_ERR_COMM_SERVE_ACTIVE error code	*/
/*			   Use MVL_SYNC_REQ_INTERFACE			*/
/* 11/16/98  MDE     03    Renamed internal functions (prefix '_')	*/
/* 06/15/98  MDE     02    Changes to allow compile under C++		*/
/* 03/27/98  JRB     01    MMS-LITE V4.0 Release			*/
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
/*			mvla_fopen					*/
/************************************************************************/

ST_RET mvla_fopen (
	MVL_NET_INFO  *net_info,
	ST_CHAR *filename,
	ST_UINT32 init_pos,
	MVL_REQ_PEND **req_out)
  {
ST_RET rc;
MVL_REQ_PEND *req_pend;

  *req_out = req_pend = _mvl_get_req_ctrl (net_info, MMSOP_FILE_OPEN);
  if (!req_pend)
    return (MVL_ERR_REQ_CONTROL);

  S_LOCK_COMMON_RESOURCES ();
  rc = _mvla_send_fopen (net_info, filename, init_pos, req_pend);
  S_UNLOCK_COMMON_RESOURCES ();

  return (rc);
  }                                  

/************************************************************************/
/*			_mvla_send_fopen				*/
/************************************************************************/
  /* Allocate FOPEN_REQ_INFO with "one" FILE_NAME entry after it.	*/
  /* If need more than one FILE_NAME, must use "mpl_fopen".		*/
#define FOPEN_REQ_INFO_SIZE (sizeof (FOPEN_REQ_INFO) + (1 * sizeof (FILE_NAME)))

ST_RET _mvla_send_fopen (MVL_NET_INFO  *net_info,
		  	 ST_CHAR *filename,
			 ST_UINT32 init_pos,
			 MVL_REQ_PEND *req_pend)
  {
ST_RET rc;
FOPEN_REQ_INFO *req_info;
FILE_NAME       *fname_idx;
ST_CHAR fopen_req_info[FOPEN_REQ_INFO_SIZE];

  /* Allocate FOPEN_REQ_INFO with "one" FILE_NAME entry after it.	*/
  /* If need more than one FILE_NAME, must use "mpl_fopen".		*/
  req_info = (FOPEN_REQ_INFO *) fopen_req_info;

  req_info->init_pos = init_pos;
  req_info->num_of_fname = 1;

  /* Point to the FILE_NAME structure and fill it in.			*/
  fname_idx = (FILE_NAME *)(req_info + 1);
  fname_idx->fn_len = (ST_INT) strlen (filename);
  fname_idx->fname = filename;

  /* Build a fopen request               */
  rc = mpl_fopen (req_info);
  if (rc == SD_SUCCESS)
    mpl_req_log (req_pend, req_info);

  rc = _mvl_send_req (net_info, req_pend, rc);
  return (rc);
  }

/************************************************************************/
/*			_mvl_process_fopen_conf				*/
/************************************************************************/

ST_VOID _mvl_process_fopen_conf (MVL_REQ_PEND *req_pend)
  {
  req_pend->u.fopen.resp_info = 
	(FOPEN_RESP_INFO *) req_pend->event->u.mms.dec_rslt.data_ptr;
  }

