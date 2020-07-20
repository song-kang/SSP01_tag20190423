/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	1998 - 1999, All Rights Reserved				*/
/*									*/
/* MODULE NAME : c_fren.c						*/
/* PRODUCT(S)  : MMSEASE-LITE						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*	mvla_frename							*/
/*	_mvl_process_frename_conf					*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev     Comments					*/
/* --------  ---  ------   -------------------------------------------	*/
/* 11/23/11  KCR           Added casting for proper 64bit compile       */
/* 03/02/10  JRB	   Del synchronous client functions.		*/
/* 04/09/07  MDE     11    Enhanced filtered logging 			*/
/* 11/01/01  JRB     10    mvl_wait_req_done now returns ST_RET.	*/
/* 09/21/01  JRB     09    Alloc global bufs only once at startup.	*/
/* 03/07/01  JRB     08    Del LOCK on sync functs: caused deadlocks.	*/
/* 01/21/00  MDE     07    Eliminated allocation			*/
/* 10/10/99  GLB     06    Created to accomodate file Rename            */
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
/*			mvla_frename					*/
/************************************************************************/

/* Allocate FRENAME_REQ_INFO with 2 FILE_NAME entries after it.	*/
/* If need multiple FILE_NAME elements, must use "mpl_frename".	*/
#define FRENAME_REQ_INFO_SIZE (sizeof (FRENAME_REQ_INFO) + (2 * sizeof (FILE_NAME)))

ST_RET mvla_frename (
	MVL_NET_INFO  *net_info,
	ST_CHAR *curfilename,
	ST_CHAR *newfilename,
	MVL_REQ_PEND **req_out)
  {
ST_RET rc;
MVL_REQ_PEND *req_pend;
FRENAME_REQ_INFO *req_info;
FILE_NAME       *fname_idx;
ST_CHAR frename_req_info[FRENAME_REQ_INFO_SIZE];

  *req_out = req_pend = _mvl_get_req_ctrl (net_info, MMSOP_FILE_RENAME);
  if (!req_pend)
    return (MVL_ERR_REQ_CONTROL);

  req_info = (FRENAME_REQ_INFO *) frename_req_info;

  req_info->num_of_cur_fname = 1;
  req_info->num_of_new_fname = 1;

  /* Point to the FILE_NAME structure and fill in the current FileName	*/
  fname_idx = (FILE_NAME *)(req_info + 1);
  fname_idx->fn_len = (ST_INT) strlen (curfilename);
  fname_idx->fname = curfilename;
  /* move the pointer by 1 name and fill in the new FileName		*/
  fname_idx++;
  fname_idx->fn_len = (ST_INT) strlen (newfilename);
  fname_idx->fname = newfilename;
  

  S_LOCK_COMMON_RESOURCES ();
  
  /* Build a frename request               */
  rc = mpl_frename (req_info);
  if (rc == SD_SUCCESS)
    mpl_req_log (req_pend, req_info);

  rc = _mvl_send_req (net_info, req_pend, rc);

  S_UNLOCK_COMMON_RESOURCES ();
  return (rc);
  }                                  

/************************************************************************/
/*			_mvl_process_frename_conf			*/
/************************************************************************/

ST_VOID _mvl_process_frename_conf (MVL_REQ_PEND *req_pend)
  {
  /* Nothing to do. Common code takes care of setting req_pend->result.	*/
  }

