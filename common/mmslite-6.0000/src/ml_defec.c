/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER	*****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*		       1997 All Rights Reserved				*/
/*									*/
/* MODULE NAME : ml_defec.c						*/
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
/* m_log_defec_req                                                      */
/************************************************************************/


ST_VOID m_log_defec_req (DEFEC_REQ_INFO *info)
  {
        
  MLOG_CALWAYS0 ("Event Condition Name Follows:");
  m_log_objname ( &(info->evcon_name) );

  switch (info->eclass)
    {
    case 0 :
      MLOG_CALWAYS0 ("Event Condition Class : 0, Network Triggered");
      break;
    case 1 :
      MLOG_CALWAYS0 ("Event Condition Class : 1, Monitored");
      break;
    default:
      MLOG_CALWAYS1 ("Event Condition Class : %d, Invalid Class", info->eclass);
      break;
    }

  MLOG_CALWAYS1 ("Priority : %u",info->priority);
  MLOG_CALWAYS1 ("Severity : %u",info->severity);

  if ( info->as_reports_pres )
    {
    if ( info->as_reports )
      {
      MLOG_CALWAYS0 ("Alarm Summary Reports : Yes");
      }
    else
      {
      MLOG_CALWAYS0 ("Alarm Summary Reports : No");
      }
    }
  else
    {
    MLOG_CALWAYS0 ("Alarm Summary Reports : Not present");
    }

  if (info->mon_var_pres )
    {
    MLOG_CALWAYS0("Monitored Variable Follows:");
    m_log_var_spec (&info->var_ref);
    }
  else
    {
    MLOG_CALWAYS0("Monitored Variable : Not present");
    }

  if (info->eval_int_pres)
    {
    MLOG_CALWAYS1 ("Evaluation Interval : %lu.", (ST_ULONG)info->eval_interval);
    }
  else
    {
    MLOG_CALWAYS0 ("Evaluation Interval : Not present");
    }

  }

