/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER	*****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*		       1997 All Rights Reserved				*/
/*									*/
/* MODULE NAME : ml_repec.c						*/
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
/* m_log_repeca_req                                                     */
/************************************************************************/


ST_VOID m_log_repecs_req (REPECS_REQ_INFO *info)
  {
  m_log_objname (&(info->evcon_name));
  }



/************************************************************************/
/* m_log_repecs_resp                                                 */
/************************************************************************/


ST_VOID m_log_repecs_resp (REPECS_RESP_INFO *info)
  {
  switch (info->cur_state)
    {
    case    0 :
      MLOG_CALWAYS1 ("Current State : %d, Disabled.", info->cur_state);
      break;
    case    1 :
      MLOG_CALWAYS1 ("Current State : %d, Idle.", info->cur_state);
      break;
    case    2 :
      MLOG_CALWAYS1 ("Current State : %d, Active.", info->cur_state);
      break;
    default :
      MLOG_CALWAYS1 ("Current State : %d, Invalid.", info->cur_state);
      break;
    }

  if (info->enabled_pres)
    {
    MLOG_CALWAYS1 ("Number of Event Enrollments : %lu ",
                                        (ST_ULONG)info->num_of_ev_enroll);
    if (info->enabled)
      {
      MLOG_CALWAYS0 ("Enabled : Yes");
      }
    else
      {
      MLOG_CALWAYS0 ("Enabled : No");
      }
    }
  else
    {
    MLOG_CALWAYS0 ("Number of Event Enrollments : Not Present");
    }

  if (info->tta_time_pres)
    {
    MLOG_CALWAYS0 ("Transition to Active Follows:");
    m_log_evtime (&info->tta_time);
    }
  else
    {
    MLOG_CALWAYS0 ("Transition to Active : Not Present");
    }

  if (info->tti_time_pres)
    {
    MLOG_CALWAYS0 ("Transition to Idle Follows:");
    m_log_evtime (&info->tti_time);
    }
  else
    {
    MLOG_CALWAYS0 ("Transition to Idle : Not Present");
    }
  }

