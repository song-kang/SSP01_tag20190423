/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER	*****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*		       1997 All Rights Reserved				*/
/*									*/
/* MODULE NAME : ml_jstat.c						*/
/* PRODUCT(S)  : MMS-EASE						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION	LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 01/20/10  JRB     04    Fix log messages.				*/
/* 03/11/04  GLB     03    Remove "thisFileName"                    	*/
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
/* m_log_jstat_req                                                      */
/************************************************************************/


ST_VOID m_log_jstat_req (JSTAT_REQ_INFO *info)
  {
  MLOG_CALWAYS0 ("Journal Name Follows:");
  m_log_objname (&(info->jou_name));
  }


/************************************************************************/
/* m_log_jstat_resp                                                  */
/************************************************************************/


ST_VOID m_log_jstat_resp (JSTAT_RESP_INFO *info)
  {

  MLOG_CALWAYS1 ("Current Entries : %lu", (ST_ULONG)info->cur_entries);
  if (info->mms_deletable)
    {
    MLOG_CALWAYS0 ("MMS Deletable : Yes");
    }
  else
    {
    MLOG_CALWAYS0 ("MMS Deletable : No");
    }

  }



