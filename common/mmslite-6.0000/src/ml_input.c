/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER	*****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*		       1997 All Rights Reserved				*/
/*									*/
/* MODULE NAME : ml_input.c						*/
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
/* m_log_input_req                                                      */
/************************************************************************/


ST_VOID m_log_input_req (INPUT_REQ_INFO *info)
  {
ST_CHAR **pd_ptr;
ST_INT i;

  pd_ptr = (ST_CHAR **) (info + 1);
  MLOG_CALWAYS1 ("Input Station Name : %s",info->station_name);
  if ( info->echo )
    {
    MLOG_CALWAYS0 ("Echo : On ");
    }
  else
    {
    MLOG_CALWAYS0 ("Echo : Off ");
    }
  if ( info->timeout_pres )
    {
    MLOG_CALWAYS1 ("Input Timeout : %lu.", (ST_ULONG)info->timeout);
    }
  else
    {
    MLOG_CALWAYS0 ("Input Timeout : Not Present.");
    }
  if ( info->prompt_pres )
    {
    MLOG_CALWAYS1 ("Number of Prompt Data Strings : %d ", info->prompt_count);
    for (i = 0; i < info->prompt_count; i++, pd_ptr++)
      {
      MLOG_CALWAYS2 ("Prompt Data %3d : %s.", i+1, *pd_ptr);
      }
    }
  else
    {
    MLOG_CALWAYS0 ("Prompt Data : Not Present.");
    }


  }



/************************************************************************/
/* m_log_input_resp                                                  */
/************************************************************************/


ST_VOID m_log_input_resp (INPUT_RESP_INFO *info)
  {
  MLOG_CALWAYS1 ("Input Response : %s.", info->input_resp);
  }





