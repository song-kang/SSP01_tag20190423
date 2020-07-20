/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	1998 - 1998, All Rights Reserved				*/
/*									*/
/* MODULE NAME : c_fdir.c						*/
/* PRODUCT(S)  : MMSEASE-LITE						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*	mvla_fdir							*/
/*	_mvl_process_fdir_conf						*/
/*									*/
/* WARNING: these functions were designed to be as easy as possible to	*/
/* use for MOST applications. They will work for any system where	*/
/* filenames can be represented as a single NULL terminated string.	*/
/* They do NOT support filenames with multiple parts or which contain	*/
/* If these features are needed, then the more complicated mpl_fdir	*/
/* must be used directly.						*/
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
/*			mvla_fdir					*/
/************************************************************************/

ST_RET mvla_fdir (
	MVL_NET_INFO  *net_info,
	ST_CHAR *filename,
	ST_CHAR *ca_filename,
	MVL_REQ_PEND **req_out)
  {
ST_RET rc;
MVL_REQ_PEND  *req_pend;

  *req_out = req_pend = _mvl_get_req_ctrl (net_info, MMSOP_FILE_DIR);
  if (!req_pend)
    return (MVL_ERR_REQ_CONTROL);
  req_pend->u.fdir.resp_info = NULL;	/* Confirm funct must allocate	*/

  S_LOCK_COMMON_RESOURCES ();
  rc = _mvla_send_fdir (net_info, filename, ca_filename, req_pend);
  S_UNLOCK_COMMON_RESOURCES ();
  return (rc);
  } 
                                 
/************************************************************************/
/*			_mvla_send_fdir					*/
/************************************************************************/
/* Allocate FDIR_REQ_INFO with "one" FILE_NAME entry after it.	*/
/* If need more than one FILE_NAME, must use "mpl_fdir".		*/
#define FDIR_REQ_INFO_SIZE (sizeof (FDIR_REQ_INFO) + (2 * sizeof (FILE_NAME)))

ST_RET _mvla_send_fdir (MVL_NET_INFO  *net_info,
			ST_CHAR *filename,
			ST_CHAR *ca_filename,
			MVL_REQ_PEND *req_pend)
  {
ST_RET rc;
FDIR_REQ_INFO *req_info;
FILE_NAME       *fname_idx;
ST_CHAR fdir_req_info[FDIR_REQ_INFO_SIZE];

  req_info = (FDIR_REQ_INFO *) fdir_req_info;
  if (filename)
    {
    req_info->filespec_pres = SD_TRUE;
    req_info->num_of_fs_fname = 1;
    }
  else 
    {
    req_info->filespec_pres = SD_FALSE;
    req_info->num_of_fs_fname = 0;
    }

  if (ca_filename)
    {
    req_info->cont_after_pres = SD_TRUE;
    req_info->num_of_ca_fname = 1;
    }
  else
    {
    req_info->cont_after_pres = SD_FALSE;
    req_info->num_of_ca_fname = 0;
    }

  /* Point to first FILE_NAME structure.				*/
  fname_idx = (FILE_NAME *)(req_info + 1);

  if (filename)
    {
    fname_idx->fn_len = (ST_INT) strlen (filename);
    fname_idx->fname = filename;
    fname_idx++;			/* point to next	*/
    }

  if (ca_filename)
    {
    fname_idx->fn_len = (ST_INT) strlen (ca_filename);
    fname_idx->fname = ca_filename;
    fname_idx++;			/* point to next	*/
    }

  /* Build a fdir request               */
  rc = mpl_fdir (req_info);
  if (rc == SD_SUCCESS)
    mpl_req_log (req_pend, req_info);

  rc = _mvl_send_req (net_info, req_pend, rc);
  return (rc);
  }

/************************************************************************/
/*			_mvl_process_fdir_conf				*/
/************************************************************************/

ST_VOID _mvl_process_fdir_conf (MVL_REQ_PEND *req_pend)
  {
FDIR_DIR_ENT *dir_ent;
MVL_DIR_ENT *mvl_dir_ent;
MVL_FDIR_RESP_INFO *mvl_resp_info;
FDIR_RESP_INFO *resp_info;
ST_INT j;
ST_INT info_size;

  resp_info = (FDIR_RESP_INFO *) req_pend->event->u.mms.dec_rslt.data_ptr;

  /* Allocate and fill in "req_pend->u.fdir.resp_info".			*/
/* CRITICAL: mvl_free_req_ctrl () must free it.			*/
  info_size = sizeof (MVL_FDIR_RESP_INFO) + 
	      resp_info->num_dir_ent * sizeof(MVL_DIR_ENT);
  req_pend->u.fdir.resp_info = 
	(MVL_FDIR_RESP_INFO *) M_MALLOC (MSMEM_GEN, info_size);

  mvl_resp_info = req_pend->u.fdir.resp_info;	/* Local ptr to it.	*/
  mvl_resp_info->num_dir_ent  = resp_info->num_dir_ent;
  mvl_resp_info->more_follows = resp_info->more_follows;
  mvl_resp_info->dir_ent = (MVL_DIR_ENT *) (mvl_resp_info + 1);
  if (resp_info->num_dir_ent)
    {
    dir_ent = (FDIR_DIR_ENT *)(resp_info + 1);	/* point to first entry	*/
    mvl_dir_ent = mvl_resp_info->dir_ent;		/* point to first entry	*/
    for (j = 0;  j < resp_info->num_dir_ent;  j++, mvl_dir_ent++)
      {
      mvl_dir_ent->fsize     = dir_ent->fsize;
      mvl_dir_ent->mtimpres = dir_ent->mtimpres;
      mvl_dir_ent->mtime	   = dir_ent->mtime;

      if (_mvl_concat_filename (mvl_dir_ent->filename, dir_ent->num_of_fname, 
                      (FILE_NAME *)(dir_ent + 1), sizeof (mvl_dir_ent->filename)))
        {
        MVL_LOG_ERR0 ("Error: FileDirectory.Conf: can't process filename");
        req_pend->result = SD_FAILURE;
	return;
        }

      /* Point to next dir entry.	*/
      dir_ent = (FDIR_DIR_ENT *) ((ST_CHAR *)dir_ent + sizeof (FDIR_DIR_ENT)
                + dir_ent->num_of_fname * sizeof (FILE_NAME));
      }
    }
  }
