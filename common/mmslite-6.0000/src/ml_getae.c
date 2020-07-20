/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER	*****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*		       1997 All Rights Reserved				*/
/*									*/
/* MODULE NAME : ml_getae.c						*/
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
/* m_log_getaes_req                                                     */
/************************************************************************/


ST_VOID m_log_getaes_req (GETAES_REQ_INFO *info)
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

  MLOG_CALWAYS1 ("Most Severe Filter : %02X", info->most_sev_filter);
  MLOG_CALWAYS1 ("Least Severe Filter : %02X", info->least_sev_filter);

  if (info->ca_name_pres)
    {
    MLOG_CALWAYS0 ("Continue After Name Follows:");
    m_log_objname ( &(info->ca_name) );
    }
  else
    {
    MLOG_CALWAYS0 ("Continue After Name : Not present");
    }
  
  }






/************************************************************************/
/* m_log_getaes_resp                                                 */
/************************************************************************/


ST_VOID m_log_getaes_resp (GETAES_RESP_INFO *info)
  {
ALARM_ENROLL_SUMMARY *aes_ptr;
ST_INT i;

  aes_ptr = (ALARM_ENROLL_SUMMARY *)(info + 1);

  MLOG_CALWAYS1 (" Alarm Enroll Summaries : %d",info->num_of_alarm_esum);

  if ( info->more_follows )
    {
    MLOG_CALWAYS0 ("More Follows : Yes");
    }
  else
    {
    MLOG_CALWAYS0 ("More Follows : No");
    }

  for (i = 0; i < info->num_of_alarm_esum; i++, aes_ptr++)
    {
    MLOG_CALWAYS0 ("Event Enrollment Name Follows:");
    m_log_objname (&aes_ptr->evenroll_name);

    if (aes_ptr->client_app_pres)
      {
      MLOG_CALWAYS0 ("Client Application Reference Follows:");
      m_log_asn1_app_ref ( aes_ptr->client_app, aes_ptr->client_app_len );
      }
    else
      {
      MLOG_CALWAYS0 ("Client Application : Not Present");
      }

    MLOG_CALWAYS1 ("Severity : %02X ", aes_ptr->severity);

    switch ( aes_ptr->cur_state )
      {
      case  0 :
        MLOG_CALWAYS1 ("Current State : %d, Disabled.", aes_ptr->cur_state);
        break;
      case  1 :
        MLOG_CALWAYS1 ("Current State : %d, Idle.", aes_ptr->cur_state);
        break;
      case  2 :
        MLOG_CALWAYS1 ("Current State : %d, Active.", aes_ptr->cur_state);
        break;
      default :
        MLOG_CALWAYS1 ("Current State : %d, Invalid.", aes_ptr->cur_state);
        break;
      }

    if (aes_ptr->addl_detail_pres)
      {
      MLOG_CALWAYS0 ("Additional Detail Follows in HEX:");
      MLOG_ALWAYSH (aes_ptr->addl_detail_len, aes_ptr->addl_detail);
      }
    else
      {
      MLOG_CALWAYS0 ("Additional Detail : Not Present");
      }

    if ( aes_ptr->not_lost )
      {
      MLOG_CALWAYS0 ("Notification Lost : Yes");
      }
    else
      {
      MLOG_CALWAYS0 ("Notification Lost : No");
      }

    switch (aes_ptr->alarm_ack_rule)
      {
      case 0 :
        MLOG_CALWAYS1 ("Alarm Ack Rule : %d, None.", 
                                                aes_ptr->alarm_ack_rule);
        break;
      case 1 :
        MLOG_CALWAYS1 ("Alarm Ack Rule : %d, Simple.",
                                                aes_ptr->alarm_ack_rule);
        break;
      case 2 :
        MLOG_CALWAYS1 ("Alarm Ack Rule : %d, ACK Active.",
                                                aes_ptr->alarm_ack_rule);
        break;
      case 3 :
        MLOG_CALWAYS1 ("Alarm Ack Rule : %d, ACK All.",
                                                aes_ptr->alarm_ack_rule);
        break;
      default:
        MLOG_CALWAYS1 ("Alarm Ack Rule : %d, Invalid ACK Rule.",
                                                aes_ptr->alarm_ack_rule);
        break;
      }

    if ( aes_ptr->ee_state_pres )
      {
      switch (aes_ptr->ee_state)
        {
        case        0 :
          MLOG_CALWAYS1 ("Enrollment State : %d, Disabled.",
                                                        aes_ptr->ee_state);
          break;
        case        1 :
          MLOG_CALWAYS1 ("Enrollment State : %d, Idle.",
                                                        aes_ptr->ee_state);
          break;
        case        2 :
          MLOG_CALWAYS1 ("Enrollment State : %d, Active.",
                                                        aes_ptr->ee_state);
          break;
        case        3 :
          MLOG_CALWAYS1 ("Enrollment State : %d, Active NO ACK a.",
                                                        aes_ptr->ee_state);
          break;
        case        4 :
          MLOG_CALWAYS1 ("Enrollment State : %d, Idle NO ACK i.",
                                                        aes_ptr->ee_state);
          break;
        case        5 :
          MLOG_CALWAYS1 ("Enrollment State : %d, Idle NO ACK a.",
                                                        aes_ptr->ee_state);
          break;
        case        6 :
          MLOG_CALWAYS1 ("Enrollment State : %d, Idle-ACK.",
                                                        aes_ptr->ee_state);
          break;
        case        7 :
          MLOG_CALWAYS1 ("Enrollment State : %d, Active-ACK.",
                                                        aes_ptr->ee_state);
          break;
        default :
          MLOG_CALWAYS1 ("Enrollment State : %d, Invalid State Code.",
                                                        aes_ptr->ee_state);
          break;
        }
      }
    else
      {
      MLOG_CALWAYS0 ("Enrollment State : Not Present");
      }

    if (aes_ptr->tta_time_pres)
      {
      MLOG_CALWAYS0 ("Trans to Active Time Follows:");
      m_log_evtime (&aes_ptr->tta_time);
      }
    else
      {
      MLOG_CALWAYS0 ("Trans to Active Time : Not Present");
      }

    if (aes_ptr->aack_time_pres)
      {
      MLOG_CALWAYS0 ("Active Acked Time Follows:");
      m_log_evtime (&aes_ptr->aack_time);
      }
    else
      {
      MLOG_CALWAYS0 ("Active Acked Time : Not Present");
      }

    if (aes_ptr->tti_time_pres)
      {
      MLOG_CALWAYS0 ("Trans to Idle Time Follows:");
      m_log_evtime (&aes_ptr->tti_time);
      }
    else
      {
      MLOG_CALWAYS0 ("Trans to Idle Time : Not Present");
      }

    MLOG_CALWAYS0 ("   Idle Acked Time       : ");
    if (aes_ptr->iack_time_pres)
      {
      m_log_evtime (&aes_ptr->iack_time);
      }
    else
      {
      MLOG_CALWAYS0 ("Not Present");
      }
    }
  }




