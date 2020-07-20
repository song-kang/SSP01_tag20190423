/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER	*****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*		       1997 All Rights Reserved				*/
/*									*/
/* MODULE NAME : ml_crepi.c						*/
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
/************************************************************************/
/* m_log_crepi_req                                                      */
/************************************************************************/


ST_VOID m_log_crepi_req (CREPI_REQ_INFO *info)
  {
ST_CHAR **dn_info;
ST_INT i;

  dn_info = (ST_CHAR **) (info + 1);
  MLOG_CALWAYS1 ("Program Invocation Name : %s", info->piname);
  MLOG_CALWAYS1 ("Number of Domain Names  : %d ", info->num_of_dnames);
  for (i = 0; i < info->num_of_dnames; i++, dn_info++)
    {
    MLOG_CALWAYS2 ("%d)  %s ", i+1, *dn_info);
    }
  if (info->reusable)
    {
    MLOG_CALWAYS0 ("Reusable.");
    }
  else
    {
    MLOG_CALWAYS0 ("Not Reusable.");
    }

  if (info->monitor_pres)
    {
    if (info->monitor)
      {
      MLOG_CALWAYS0 ("Monitor : Permanent.");
      }
    else
      {
      MLOG_CALWAYS0 ("Monitor : Current.");
      }
    }
  else
    {
    MLOG_CALWAYS0 ("Not Present.");
    }

  }



