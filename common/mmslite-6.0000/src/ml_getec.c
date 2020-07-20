/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER	*****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*		       1997 All Rights Reserved				*/
/*									*/
/* MODULE NAME : ml_getec.c						*/
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
/* m_log_geteca_req                                                     */
/************************************************************************/


ST_VOID m_log_geteca_req (GETECA_REQ_INFO *info)
  {
  m_log_objname (&(info->evcon_name));
  }



/************************************************************************/
/* m_log_geteca_resp                                                 */
/************************************************************************/


ST_VOID m_log_geteca_resp (GETECA_RESP_INFO *info)
  {
  if (info->mms_deletable)
    {
    MLOG_CALWAYS0 ("MMS Deletable : Yes");
    }
  else
    {
    MLOG_CALWAYS0 ("MMS Deletable : No");
    }

  if (info->eclass == 0)
    {
    MLOG_CALWAYS1 ("Event Condition Class : %d, Network Triggered",
                                                        info->eclass);
    }
  else if (info->eclass == 1)
    {
    MLOG_CALWAYS1 ("Event Condition Class : %d, Monitored", info->eclass);
    }
  else
    {
    MLOG_CALWAYS1 ("Event Condition Class : %d, Not Defined", info->eclass);
    }

  MLOG_CALWAYS1 ("Priority : %d", (info->priority & 0xff));
  MLOG_CALWAYS1 ("Severity : %d", (info->severity & 0xff));

  if (info->as_reports)
    {
    MLOG_CALWAYS0 ("Alarm Summary Reports : Yes");
    }
  else
    {
    MLOG_CALWAYS0 ("Alarm Summary Reports : No");
    }

  if (info->mon_var_pres )
    {
    if (info->mon_var_tag == 0)
      {
      MLOG_CALWAYS0 ("Monitored Variable : ");
      MLOG_CALWAYS0 ("Variable Specification Follows:");
      m_log_var_spec (&info->var_ref);
      }
    else if (info->mon_var_tag == 1)
      {
      MLOG_CALWAYS0 ("Monitored Variable : Undefined");
      }
    else
      {
      MLOG_CALWAYS0 ("Monitored Variable : Invalid Tag");
      }
    }
  else
    {
    MLOG_CALWAYS0("Monitored Variable : Not Present");
    }

  if (info->eval_int_pres)
    {
    MLOG_CALWAYS1 ("Evaluation Interval : %lu ", (ST_ULONG)info->eval_interval);
    }
  else
    {
    MLOG_CALWAYS0 ("Evaluation Interval : Not Present");
    }
  }



