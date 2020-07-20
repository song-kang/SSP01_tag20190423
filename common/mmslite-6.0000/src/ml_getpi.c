/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER	*****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*		       1997 All Rights Reserved				*/
/*									*/
/* MODULE NAME : ml_getpi.c						*/
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
/* m_log_getpi_req                                                      */
/************************************************************************/


ST_VOID m_log_getpi_req (GETPI_REQ_INFO *info)
  {
  MLOG_CALWAYS1 ("Program Invocation Name : %s", info->piname);
  }



/************************************************************************/
/* m_log_getpi_resp                                                  */
/************************************************************************/


ST_VOID m_log_getpi_resp (GETPI_RESP_INFO *info)
  {
ST_CHAR    **data;
ST_INT i;
EXTERN_INFO ext;

  data = (ST_CHAR **) (info + 1);
  MLOG_CALWAYS0 ("State : ");
  m_log_pi_state ( info->state );
  if (info->mms_deletable)
    {
    MLOG_CALWAYS0 ("MMS Deletable : YES.");
    }
  else
    {
    MLOG_CALWAYS0 ("MMS Deletable : NO.");
    }
  if (info->reusable)
    {
    MLOG_CALWAYS0 ("Reusable : YES.");
    }
  else
    {
    MLOG_CALWAYS0 ("Reusable : NO.");
    }
  if (info->monitor)
    {
    MLOG_CALWAYS0 ("Monitor : Permanent.");
    }
  else
    {
    MLOG_CALWAYS0 ("Monitor : Current");
    }
  MLOG_CALWAYS0 ("Start Argument : ");
  if (info->start_arg_type == ARG_TYPE_ENCODED)
    {
    MLOG_CALWAYS0 ("ENCODED Start ARG");
#if 1
    if (!ms_decode_extern (info->start_arg, info->start_arg_len,&ext))
      {
      m_log_external (&ext);
      }
    else
      {
      MLOG_CALWAYS0 ("EXTERNAL Decode Error Follows in HEX:");
      MLOG_ALWAYSH (info->start_arg_len, info->start_arg);
      }
#else
      MLOG_ALWAYSH (info->start_arg_len, info->start_arg);
#endif
    }
  else
    {
    MLOG_CALWAYS1 ("%s", info->start_arg);
    }
  MLOG_CALWAYS1 ("Number of Domain Names : %d",  info->num_of_dnames);
  for (i = 0; i < info->num_of_dnames; i++, data++)
    {
    MLOG_CALWAYS2 ("%d) %s.", i+1, *data);
    }

  }






/************************************************************************/
/*                      m_log_pi_state                                  */
/* Debugging pi state changes is the worst.  This logs what the  number */
/* means                                                                */
/************************************************************************/


ST_VOID m_log_pi_state (ST_INT state)
  {
  switch (state)
    {
    case PI_NON_EXISTENT :              
      MLOG_CALWAYS0( "PI_NON_EXISTENT" );
      break;

    case PI_UNRUNNABLE :                
      MLOG_CALWAYS0( "PI_UNRUNNABLE" );
      break;

    case PI_IDLE :              
      MLOG_CALWAYS0( "PI_IDLE" );
      break;

    case PI_RUNNING :           
      MLOG_CALWAYS0( "PI_RUNNING" );
      break;

    case PI_STOPPED :           
      MLOG_CALWAYS0( "PI_STOPPED" );
      break;

    case PI_STARTING :          
      MLOG_CALWAYS0( "PI_STARTING" );
      break;

    case PI_STOPPING :          
      MLOG_CALWAYS0( "PI_STOPPING" );
      break;

    case PI_RESUMING :          
      MLOG_CALWAYS0( "PI_RESUMING" );
      break;

    case PI_RESETTING :         
      MLOG_CALWAYS0( "PI_RESETTING" );
      break;

    default :                   /* any other error reason       */
      MLOG_CALWAYS0( "UNDEFINED PI STATE" );
      break;
    }
  }

