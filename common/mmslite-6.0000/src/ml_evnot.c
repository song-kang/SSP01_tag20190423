/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER	*****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*		       1997 All Rights Reserved				*/
/*									*/
/* MODULE NAME : ml_evnot.c						*/
/* PRODUCT(S)  : MMS-EASE						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION	LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 01/20/10  JRB     05    Fix log messages.				*/
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
/* m_log_evnot_req                                                      */
/************************************************************************/


ST_VOID m_log_evnot_req (EVNOT_REQ_INFO *info)
  {

  MLOG_CALWAYS0 ("Event Enrollment Name Follows:");
  m_log_objname ( &(info->evenroll_name) );

  MLOG_CALWAYS0 ("Event Condition Name Follows:");
  m_log_objname ( &(info->evcon_name) );

  MLOG_CALWAYS1 ("Severity : %d", info->severity);

  if (info->cur_state_pres)
    {
    switch (info->cur_state)
      {
      case 0 :
        MLOG_CALWAYS1 ("Current EC State : %d, Disabled.", info->cur_state);
      break;
      case 1 :
        MLOG_CALWAYS1 ("Current EC State : %d, Idle.", info->cur_state);
      break;
      case 2 :
        MLOG_CALWAYS1 ("Current EC State : %d, Active.", info->cur_state);
      break;
      default:
        MLOG_CALWAYS1 ("Current EC State : %d, Invalid State.", info->cur_state);
      break;
      }
    }
  else
    {
    MLOG_CALWAYS0 ("Current EC State : Not Present");
    }

  MLOG_CALWAYS0 ("Event Transition Time Follows:");
  m_log_evtime (&(info->trans_time));

  if ( info->not_lost )
    {
    MLOG_CALWAYS0 ("Notification Lost : Yes");
    }
  else
    {
    MLOG_CALWAYS0 ("Notification Lost : No");
    }

  if ( info->alarm_ack_rule_pres )
    {
    switch (info->alarm_ack_rule)
      {
      case 0 :
        MLOG_CALWAYS1 ("Alarm Ack Rule : %d, None.",
                                                info->alarm_ack_rule);
        break;
      case 1 :
        MLOG_CALWAYS1 ("Alarm Ack Rule : %d, Simple.",
                                                info->alarm_ack_rule);
        MLOG_CALWAYS0 ("simple");
        break;
      case 2 :
        MLOG_CALWAYS1 ("Alarm Ack Rule : %d, ACK Active.",
                                                info->alarm_ack_rule);
        break;
      case 3 :
        MLOG_CALWAYS1 ("Alarm Ack Rule : %d, ACK All.",
                                                info->alarm_ack_rule);
        break;
      default:
        MLOG_CALWAYS1 ("Alarm Ack Rule : %d, Invalid ACK Rule.",
                                                info->alarm_ack_rule);
        break;
      }
    }
  else
    {
    MLOG_CALWAYS0 ("Alarm Ack Rule : Not Present");
    }

  if ( info->evact_result_pres )
    {
    MLOG_CALWAYS0 ("Event Action Result Follows: ");
    m_log_objname ( &(info->evact_name) );

    if ( info->evact_result_tag == 0 )
      {
      MLOG_CALWAYS0 ("Success, Conf Serv Resp: ");
      MLOG_ALWAYSH (info->conf_serv_resp_len, info->conf_serv_resp);
      if ( info->cs_rdetail_pres )
        {
        MLOG_CALWAYS0 ("CS Resp Detail Follows in HEX:");
        MLOG_ALWAYSH (info->cs_rdetail_len, info->cs_rdetail);
        }
      else
        {
        MLOG_CALWAYS0 ("CS Resp Detail : Not Present");
        }
      }
    else
      {
      if ( info->mod_pos_pres )
        {
        MLOG_CALWAYS1 ("Failure, Modifier Pos : %lu.", (ST_ULONG)info->mod_pos);
        }
      else
        {
        MLOG_CALWAYS0 ("Failure, Modifier Pos : Not Present");
        }
      m_log_error_info (info->serv_err);
      }
    }
  else
    {
    MLOG_CALWAYS0 ("Event Action Result : Not Present");
    }
  }




