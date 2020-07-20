/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER	*****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*		       1997 All Rights Reserved				*/
/*									*/
/* MODULE NAME : ml_start.c						*/
/* PRODUCT(S)  : MMS-EASE						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION	LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
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
/* m_log_start_req                                                      */
/************************************************************************/


ST_VOID m_log_start_req (START_REQ_INFO *info)
  {
EXTERN_INFO ext;

  MLOG_CALWAYS1 ("Program Invocation Name : %s", info->piname);
  if (info->start_arg_pres)
    {
    if (info->start_arg_type == ARG_TYPE_ENCODED)
      {
      MLOG_CALWAYS0 ("ENCODED START ARG");
      MLOG_CALWAYS0 ("Start Argument : ");
#if 1
      if (!ms_decode_extern (info->start_arg, info->start_arg_len,&ext))
        {
        m_log_external (&ext);
        }
      else
        {
        MLOG_CALWAYS0 ("EXTERNAL decode error");
        MLOG_ALWAYSH (info->start_arg_len,info->start_arg);
        }
#else
      MLOG_ALWAYSH (info->start_arg_len,info->start_arg);
#endif
      }
    else
      {
      MLOG_CALWAYS0 ("Start Argument : ");
      MLOG_CALWAYS1 ("%s", info->start_arg);
      }
    }
  else
    {
    MLOG_CALWAYS0 ("Start Argument : Not Present.");
    }

  }




