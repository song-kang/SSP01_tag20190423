/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	1998 - 1999, All Rights Reserved				*/
/*									*/
/* MODULE NAME : c_obtfi.c						*/
/* PRODUCT(S)  : MMSEASE-LITE						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*	mvla_obtfile							*/
/*	_mvl_process_obtfile_conf					*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev     Comments					*/
/* --------  ---  ------   -------------------------------------------	*/
/* 11/23/11  KCR           Added casting for proper 64bit compile       */
/* 03/02/10  JRB	   Del synchronous client functions.		*/
/* 04/09/07  MDE     06    Enhanced filtered logging 			*/
/* 11/01/01  JRB     05    mvl_wait_req_done now returns ST_RET.	*/
/* 09/21/01  JRB     04    Alloc global bufs only once at startup.	*/
/* 03/07/01  JRB     03    Del LOCK on sync functs: caused deadlocks.	*/
/* 01/21/00  MDE     02    Eliminated an allocation			*/
/* 10/14/99  GLB     01    Created to add Obtain File function          */					
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
/*			mvla_obtfile					*/
/************************************************************************/

  /* Allocate OBTFILE_REQ_INFO with "two" FILE_NAME entries after it.	*/
  /* If need multiple FILE_NAME elements, must use "mpl_frename".	*/
#define OBTFILE_REQ_INFO_SIZE (sizeof (OBTFILE_REQ_INFO) + (2 * sizeof (FILE_NAME)))

ST_RET mvla_obtfile (
	MVL_NET_INFO  *net_info,
	ST_CHAR *srcfilename,
	ST_CHAR *destfilename,
	MVL_REQ_PEND **req_out)
  {
ST_RET rc;
MVL_REQ_PEND *req_pend;
OBTFILE_REQ_INFO *req_info;
FILE_NAME       *fname_idx;
ST_CHAR obtfile_req_info[OBTFILE_REQ_INFO_SIZE];

  *req_out = req_pend = _mvl_get_req_ctrl (net_info, MMSOP_OBTAIN_FILE);
  if (!req_pend)
    return (MVL_ERR_REQ_CONTROL);

  req_info = (OBTFILE_REQ_INFO *) obtfile_req_info;

  req_info->ar_title_pres = SD_FALSE;
  req_info->num_of_src_fname = 1;
  req_info->num_of_dest_fname = 1;

  /* Point to the FILE_NAME structure and fill in the current FileName	*/
  fname_idx = (FILE_NAME *)(req_info + 1);
  fname_idx->fn_len = (ST_INT) strlen (srcfilename);
  fname_idx->fname = srcfilename;
  /* move the pointer by 1 name and fill in the new FileName		*/
  fname_idx++;
  fname_idx->fn_len = (ST_INT) strlen (destfilename);
  fname_idx->fname = destfilename;
  

  S_LOCK_COMMON_RESOURCES ();
  
  /* Build a frename request               */
  rc = mpl_obtfile (req_info);
  if (rc == SD_SUCCESS)
    mpl_req_log (req_pend, req_info);

  rc = _mvl_send_req (net_info, req_pend, rc);

  S_UNLOCK_COMMON_RESOURCES ();
  return (rc);
  }                                  

/************************************************************************/
/*			_mvl_process_obtfile_conf			*/
/************************************************************************/

ST_VOID _mvl_process_obtfile_conf (MVL_REQ_PEND *req_pend)
  {
  /* Nothing to do. Common code takes care of setting req_pend->result.	*/
  }

