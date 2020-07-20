/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER	*****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*		       1997 All Rights Reserved				*/
/*									*/
/* MODULE NAME : ml_fread.c						*/
/* PRODUCT(S)  : MMS-EASE						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION	LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 01/20/10  JRB     05    Fix log messages.				*/
/* 03/11/04  GLB     04    Remove "thisFileName"                    	*/
/* 10/14/03  JRB     03    Del unnecessary casts.			*/
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
/* m_log_fread_req                                                      */
/************************************************************************/


ST_VOID m_log_fread_req (FREAD_REQ_INFO *info)
  {
  MLOG_CALWAYS1 ("File Read State Machine Id = %ld.",(ST_LONG)info->frsmid);
  }


/************************************************************************/
/* m_log_fread_resp                                                  */
/************************************************************************/


ST_VOID m_log_fread_resp (FREAD_RESP_INFO *info)
  {

  if (info->more_follows)
    {
    MLOG_CALWAYS0 ("More Follows : YES.");
    }
  else
    {
    MLOG_CALWAYS0 ("More Follows : NO.");
    }
  MLOG_CALWAYS1 ("File Data Length : %d.", info->fd_len);
  MLOG_ALWAYSH (info->fd_len, info->filedata);

  }




