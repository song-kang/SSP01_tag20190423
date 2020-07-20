/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER	*****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*		       1997 All Rights Reserved				*/
/*									*/
/* MODULE NAME : ml_rssta.c						*/
/* PRODUCT(S)  : MMS-EASE						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION	LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
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
/* REPORT SEMAPHORE STATUS                                              */
/************************************************************************/


ST_VOID m_log_rsstat_req (RSSTAT_REQ_INFO *info)
  {
  m_log_objname ( &(info->sem_name) );
  }


/************************************************************************/
/* m_log_rsstat_resp                                                 */
/************************************************************************/


ST_VOID m_log_rsstat_resp (RSSTAT_RESP_INFO *info)
  {

  if (info->mms_deletable)
    {
    MLOG_CALWAYS0 ("MMS Deletable : Yes.");
    }
  else
    {
    MLOG_CALWAYS0 ("MMS Deletable : No.");
    }

  if (info->tclass == 0)
    {
    MLOG_CALWAYS0 ("Semaphore Class : Token.");
    }
  else if (info->tclass == 1)
    {
    MLOG_CALWAYS0 ("Semaphore Class : Pool.");
    }
  else  /* this should never happen     */
    {
    MLOG_CALWAYS0 ("Error!! This should NEVER EVER Happen!!!");
    }

  MLOG_CALWAYS1 ("Number of Tokens : %u ", info->num_of_tokens );
  MLOG_CALWAYS1 ("Number of Owned Tokens : %u ", info->num_of_owned );
  MLOG_CALWAYS1 ("Number of Hung Tokens : %u ", info->num_of_hung );

  }



