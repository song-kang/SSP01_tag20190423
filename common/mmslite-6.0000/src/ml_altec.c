/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER	*****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*		       1997 All Rights Reserved				*/
/*									*/
/* MODULE NAME : ml_altec.c						*/
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
/* m_log_altecm_req                                                     */
/************************************************************************/


ST_VOID m_log_altecm_req (ALTECM_REQ_INFO *info)
  {

  MLOG_CALWAYS0 ("Event Condition Name Follows:");
  m_log_objname ( &(info->evcon_name) );

  if (info->enabled_pres)
    {
    if ( info->enabled )
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
    MLOG_CALWAYS0 ("Enabled : Not Present");
    }

  if (info->priority_pres)
    {
    MLOG_CALWAYS1 ("Priority : %u",info->priority);
    }
  else
    {
    MLOG_CALWAYS0 ("Priority : Not Present");
    }

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
    MLOG_CALWAYS0 ("Alarm Summary Reports : Not Present");
    }

  if (info->eval_int_pres)
    {
    MLOG_CALWAYS1 ("Evaluation Interval : %lu", (ST_ULONG)info->eval_int);
    }
  else
    {
    MLOG_CALWAYS0 ("Evaluation Interval : Not Present");
    }
  }


