/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER	*****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*		       1997 All Rights Reserved				*/
/*									*/
/* MODULE NAME : ml_rsent.c						*/
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
/* m_log_rsentry_req                                                    */
/************************************************************************/


ST_VOID m_log_rsentry_req (RSENTRY_REQ_INFO *info)
  {

  m_log_objname ( &(info->sem_name) );
  if (info->state == 0)
    {
    MLOG_CALWAYS0 ("Semaphore State : Queued");
    }
  else if (info->state == 1)
    {
    MLOG_CALWAYS0 ("Semaphore State : Owner");
    }
  else if (info->state == 2)
    {
    MLOG_CALWAYS0 ("Semaphore State : Hung");
    }

  if (info->start_after_pres)
    {
    MLOG_CALWAYS0 ("Entry Id to Start After Follows in HEX:");
    MLOG_ALWAYSH (info->sa_len, info->start_after);
    }
  else
    {
    MLOG_CALWAYS0 ("Entry Id to Start After : Not Present");
    }

  }


/************************************************************************/
/* m_log_rsentry_resp                                                */
/************************************************************************/


ST_VOID m_log_rsentry_resp (RSENTRY_RESP_INFO *info)
  {
SEMAPHORE_ENTRY *sent_ptr;
ST_INT i;

  sent_ptr = (SEMAPHORE_ENTRY *) ( info + 1);
  MLOG_CALWAYS1 ("Number of Semaphore Entries Returned : %d",info->num_of_sent);
  for (i = 0; i < info->num_of_sent; i++, sent_ptr++)
    {
    MLOG_CALWAYS1 ("Semaphore Entry Number %d).", i+1);
    MLOG_CALWAYS0 ("Entry Id :");
    MLOG_ALWAYSH (sent_ptr->ei_len, sent_ptr->entry_id);
    MLOG_CALWAYS1 ("Entry Class : %d ",sent_ptr->entry_class);
    MLOG_CALWAYS0 ("Application Reference : ");
    MLOG_ALWAYSH (sent_ptr->app_ref_len, sent_ptr->entry_id);
    if (sent_ptr->named_token_pres)
      {
      MLOG_CALWAYS1 ("Named Token : %s.", sent_ptr->named_token);
      }
    else
      {
      MLOG_CALWAYS0 ("Named Token : Not Present.");
      }
    MLOG_CALWAYS1 ("Priority : %u ",sent_ptr->priority);
    if (sent_ptr->rem_timeout_pres)
      {
      MLOG_CALWAYS1 ("Remaining Timeout : %lu.", (ST_ULONG)sent_ptr->rem_timeout);
      }
    else
      {
      MLOG_CALWAYS0 ("Remaining Timeout : Not Present.");
      }

    if (sent_ptr->abrt_on_timeout_pres)
      {
      MLOG_CALWAYS1 ("Abort On Timeout : %d.", sent_ptr->abrt_on_timeout);
      }
    else
      {
      MLOG_CALWAYS0 ("Abort On Timeout : Not Present.");
      }

    if (sent_ptr->rel_conn_lost)
      {
      MLOG_CALWAYS0 ("Relinquish if Connection Lost : Yes.");
      }
    else
      {
      MLOG_CALWAYS0 ("Relinquish if Connection Lost : No.");
      }

    }
  if (info->more_follows)
    {
    MLOG_CALWAYS0 ("More Follows.");
    }
  else
    {
    MLOG_CALWAYS0 ("No More Follows.");
    }
  }



