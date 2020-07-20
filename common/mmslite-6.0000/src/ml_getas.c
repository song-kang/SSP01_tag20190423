/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER	*****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*		       1997 All Rights Reserved				*/
/*									*/
/* MODULE NAME : ml_getas.c						*/
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
/* m_log_getas_req                                                      */
/************************************************************************/


ST_VOID m_log_getas_req (GETAS_REQ_INFO *info)
  {

  if ( info->enroll_only )
    {
    MLOG_CALWAYS0 ("Enrollments Only : Yes");
    }
  else
    {
    MLOG_CALWAYS0 ("Enrollments Only : No");
    }

  if ( info->act_alarms_only )
    {
    MLOG_CALWAYS0 ("Active Alarms Only : Yes");
    }
  else
    {
    MLOG_CALWAYS0 ("Active Alarms Only : No");
    }

  switch (info->ack_filter)
    {
    case 0 :
      MLOG_CALWAYS1 ("AckNowledgement Filter : %d, Not ACK'ed.",
                                                info->ack_filter);
      break;
    case 1 :
      MLOG_CALWAYS1 ("AckNowledgement Filter : %d, ACK'ed.",
                                                info->ack_filter);
      break;
    case 2 :
      MLOG_CALWAYS1 ("AckNowledgement Filter : %d, All.",
                                                info->ack_filter);
      break;
    default:
      MLOG_CALWAYS1 ("AckNowledgement Filter : %d, Invalid ACK Filter.",
                                                info->ack_filter);
      break;
    }

  MLOG_CALWAYS1 ("Most Severe Filter : %02X ", info->most_sev_filter);
  MLOG_CALWAYS1 ("Least Severe Filter : %02X ", info->least_sev_filter);

  if ( info->ca_pres )
    {
    MLOG_CALWAYS0 ("Continue After Name Follows:");
    m_log_objname ( &(info->ca_name) );
    }
  else
    {
    MLOG_CALWAYS0 ("Continue After Name : Not Present");
    }

  }


/************************************************************************/
/* m_log_getas_resp                                                  */
/************************************************************************/


ST_VOID m_log_getas_resp (GETAS_RESP_INFO *info)
  {
ALARM_SUMMARY   *as_ptr;
ST_INT i;

  as_ptr  = (ALARM_SUMMARY *)  (info + 1);

  MLOG_CALWAYS1 ("Number of Alarm Summaries : %d", info->num_of_alarm_sum);
  if ( info->more_follows )
    {
    MLOG_CALWAYS0 ("More Follows : Yes");
    }
  else
    {
    MLOG_CALWAYS0 ("More Follows : No");
    }

  for (i = 0; i < info->num_of_alarm_sum; i++, as_ptr++)
    {
    MLOG_CALWAYS1 ("Alarm Summary : %d", i+1);

    MLOG_CALWAYS0 ("Event Condition Name Follows: ");
    m_log_objname (&as_ptr->evcon_name);

    MLOG_CALWAYS1 ("Severity : %02X", as_ptr->severity);

    switch ( as_ptr->cur_state )
      {
      case  0 :
        MLOG_CALWAYS1 ("Current State : %d, Disabled.", as_ptr->cur_state);
        break;
      case  1 :
        MLOG_CALWAYS1 ("Current State : %d, Idle.", as_ptr->cur_state);
        break;
      case  2 :
        MLOG_CALWAYS1 ("Current State : %d, Active.", as_ptr->cur_state);
        break;
      default :
        MLOG_CALWAYS1 ("Current State : %d, Invalid.", as_ptr->cur_state);
        break;
      }

    switch ( as_ptr->unack_state )
      {
      case  0 :
        MLOG_CALWAYS1 ("UnackNowledged State : %d, None.",
                                                        as_ptr->unack_state);
        break;
      case  1 :
        MLOG_CALWAYS1 ("UnackNowledged State : %d, Active.",
                                                        as_ptr->unack_state);
        break;
      case  2 :
        MLOG_CALWAYS1 ("UnackNowledged State : %d, Idle.",
                                                        as_ptr->unack_state);
        break;
      case  3 :
        MLOG_CALWAYS1 ("UnackNowledged State : %d, Both.",
                                                        as_ptr->unack_state);
        break;
      default :
        MLOG_CALWAYS1 ("UnackNowledged State : %d, Invalid.",
                                                        as_ptr->unack_state);
        break;
      }

    if (as_ptr->addl_detail_pres)
      {
      MLOG_CALWAYS0 ("Additional Detail Follows in HEX:");
      MLOG_ALWAYSH (as_ptr->addl_detail_len, as_ptr->addl_detail);
      }
    else
      {
      MLOG_CALWAYS0 ("Additional Detail : Not Present.");
      }

    if (as_ptr->tta_time_pres)
      {
      MLOG_CALWAYS0 ("Transition to Active : ");
      m_log_evtime (&as_ptr->tta_time);
      }
    else
      {
      MLOG_CALWAYS0 ("Transition to Active : Not Present");
      }

    if (as_ptr->tti_time_pres)
      {
      MLOG_CALWAYS0 ("Transition to Idle : ");
      m_log_evtime (&as_ptr->tti_time);
      }
    else
      {
      MLOG_CALWAYS0 ("Transition to Idle : Not Present");
      }
    }
  }




