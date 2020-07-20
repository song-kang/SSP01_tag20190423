/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER	*****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*		       1997 All Rights Reserved				*/
/*									*/
/* MODULE NAME : ml_takec.c						*/
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
/* TAKE CONTROL                                                         */
/************************************************************************/


ST_VOID m_log_takectrl_req (TAKECTRL_REQ_INFO *info)
  {

  m_log_objname ( &(info->sem_name) );
  if (info->named_token_pres)
    {
    MLOG_CALWAYS1 ("Named Token : %s",info->named_token);
    }
  else
    {
    MLOG_CALWAYS0 ("Named Token : Not Present");
    }
  MLOG_CALWAYS1 ("Priority : %u",info->priority);
  if (info->acc_delay_pres)
    {
    MLOG_CALWAYS1 ("Acceptable Delay Time : %lu",(ST_ULONG)info->acc_delay);
    }
  else
    {
    MLOG_CALWAYS0 ("Acceptable Delay Time : Not Present");
    }

  if (info->ctrl_timeout_pres)
    {
    MLOG_CALWAYS1 ("Control Timeout Time : %lu",(ST_ULONG)info->ctrl_timeout);
    }
  else
    {
    MLOG_CALWAYS0 ("Control Timeout Time : Not Present");
    }

  if (info->abrt_on_timeout_pres)
    {
    if (info->abrt_on_timeout)
      {
      MLOG_CALWAYS0 ("Abort On Timeout : SD_TRUE");
      }
    else
      {
      MLOG_CALWAYS0 ("Abort On Timeout : SD_FALSE");
      }
    }
  else
    {
    MLOG_CALWAYS0 ("Abort On Timeout : Not Present");
    }

  if (info->rel_conn_lost)
    {
    MLOG_CALWAYS0 ("Relinquish if Connection Lost : Yes");
    }
  else
    {
    MLOG_CALWAYS0 ("Relinquish if Connection Lost : No");
    }

  if (info->app_preempt_pres)
    {
    MLOG_CALWAYS0 ("Application Preempt :");
    m_log_asn1_app_ref ( info->app_preempt, info->app_len );
    }
  else
    {
    MLOG_CALWAYS0 ("Application Preempt : Not Present");
    }

  }


/************************************************************************/
/* m_log_takectrl_resp                                               */
/************************************************************************/


ST_VOID m_log_takectrl_resp (TAKECTRL_RESP_INFO *info)
  {

  if (info->resp_tag == 1)                      /* named token  */
   {
   MLOG_CALWAYS1 ("Named Token : %s", info->named_token);
   }
         
  }




