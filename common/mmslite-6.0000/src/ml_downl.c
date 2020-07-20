/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER	*****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*		       1997 All Rights Reserved				*/
/*									*/
/* MODULE NAME : ml_downl.c						*/
/* PRODUCT(S)  : MMS-EASE						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION	LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 03/11/04  GLB     04    Remove "thisFileName"                    	*/
/* 10/14/03  JRB     03    Fix EXTERN_INFO log, call m_log_external.	*/
/*			   Del unnecessary casts.			*/
/* 09/13/99  MDE     02    Added SD_CONST modifiers			*/
/* 06/09/97  MDE     01    Created from existing MLOG source		*/
/************************************************************************/


#include "glbtypes.h"
#include "sysincs.h"

#ifndef MMS_LITE
#include "mms_usr.h"		/* to access MMS fucntions, variables   */
#else
#include "mmsdefs.h"
#endif
#include "mloguser.h"

/************************************************************************/
/************************************************************************/
/* m_log_download_req                                                   */
/************************************************************************/


ST_VOID m_log_download_req (DOWNLOAD_REQ_INFO *info)
  {
  MLOG_CALWAYS1 ("Domain Name : %s.", info->dname);
  }




/************************************************************************/
/* m_log_download_resp                                               */
/************************************************************************/


ST_VOID m_log_download_resp (DOWNLOAD_RESP_INFO *info)
  {
EXTERN_INFO ext;

  if (info->more_follows)
    {
    MLOG_CALWAYS0 ("More Follows : YES");
    }
  else
    {
    MLOG_CALWAYS0 ("More Follows : NO");
    }

  MLOG_CALWAYS1 ("Load Data Length : %d", info->load_data_len);
  if (info->load_data_type == LOAD_DATA_CODED)
    {
    MLOG_CALWAYS0 ("CODED LOAD DATA");
    if (!ms_decode_extern (info->load_data, info->load_data_len,&ext))
      {
      m_log_external (&ext);
      }
    else
      {
      MLOG_CALWAYS0 ("EXTERNAL decode error");
      }
    MLOG_ALWAYSH (info->load_data_len, info->load_data);
    }

  else
    {
    MLOG_CALWAYS0 ("NON-CODED LOAD DATA");

    if ( info->load_data_len > 0 )
      {
      MLOG_ALWAYSH (info->load_data_len, info->load_data);
      }
    } /* non-coded */
  }

