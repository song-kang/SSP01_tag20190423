/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER	*****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*		       1997 All Rights Reserved				*/
/*									*/
/* MODULE NAME : ml_altee.c						*/
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
/* m_log_altee_req                                                      */
/************************************************************************/


ST_VOID m_log_altee_req (ALTEE_REQ_INFO *info)
  {

  MLOG_CALWAYS0 ("Event Enrollment Name Follows:");
  m_log_objname ( &(info->evenroll_name) );

  if (info->ec_transitions_pres)
    {
    MLOG_CALWAYS1 ("Event Cond Transitions : %02x",info->ec_transitions);
    if ( info->ec_transitions & 0x01 )
      {
      MLOG_CALWAYS0 ("Idle to Disabled : Set");
      }
    else
      {
      MLOG_CALWAYS0 ("Idle to Disabled : Not Set");
      }

    if ( info->ec_transitions & 0x02 )
      {
      MLOG_CALWAYS0 ("Active to Disabled : Set");
      }
    else
      {
      MLOG_CALWAYS0 ("Active to Disabled : Not Set");
      }

    if ( info->ec_transitions & 0x04 )
      {
      MLOG_CALWAYS0 ("Disabled to Idle : Set");
      }
    else
      {
      MLOG_CALWAYS0 ("Disabled to Idle : Not Set");
      }

    if ( info->ec_transitions & 0x08 )
      {
      MLOG_CALWAYS0 ("Active to Idle : Set");
      }
    else
      {
      MLOG_CALWAYS0 ("Active to Idle : Not Set");
      }

    if ( info->ec_transitions & 0x10 )
      {
      MLOG_CALWAYS0 ("Disabled to Active : Set");
      }
    else
      {
      MLOG_CALWAYS0 ("Disabled to Active : Not Set");
      }

    if ( info->ec_transitions & 0x20 )
      {
      MLOG_CALWAYS0 ("Idle to Active : Set");
      }
    else
      {
      MLOG_CALWAYS0 ("Idle to Active : Not Set");
      }

    if ( info->ec_transitions & 0x40 )
      {
      MLOG_CALWAYS0 ("Any to Deleted : Set");
      }
    else
      {
      MLOG_CALWAYS0 ("Any to Deleted : Not Set");
      }

    }
  else
    {
    MLOG_CALWAYS0 ("Event Cond Transitions : Not Present");
    }

  if (info->alarm_ack_rule_pres)
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
  }



/************************************************************************/
/* m_log_altee_resp                                                  */
/************************************************************************/


ST_VOID m_log_altee_resp (ALTEE_RESP_INFO *info)
  {

  if ( info->cur_state_tag == 0 )
    {
    switch (info->state)
      {
      case  0 :
        MLOG_CALWAYS1 ("Current State : %d, Disabled.", info->state);
        break;
      case  1 :
        MLOG_CALWAYS1 ("Current State : %d, Idle.", info->state);
        break;
      case  2 :
        MLOG_CALWAYS1 ("Current State : %d, Active.", info->state);
        break;
      case  3 :
        MLOG_CALWAYS1 ("Current State : %d, Active NO ACK a.", info->state);
        break;
      case  4 :
        MLOG_CALWAYS1 ("Current State : %d, Idle NO ACK i.", info->state);
        break;
      case  5 :
        MLOG_CALWAYS1 ("Current State : %d, Idle NO ACK a.", info->state);
        break;
      case  6 :
        MLOG_CALWAYS1 ("Current State : %d, Idle-ACK'ed.", info->state);
        break;
      case  7 :
        MLOG_CALWAYS1 ("Current State : %d, Active-ACK'ed.", info->state);
        break;
      default :
        MLOG_CALWAYS0 ("Current State : Invalid State Code.");
        break;
      }
    }

  else if ( info->cur_state_tag == 1 )
    {
    MLOG_CALWAYS0 ("Current State : Undefined.");
    }
  else
    {
    MLOG_CALWAYS0 ("Current State Tag : Invalid.");
    }

  MLOG_CALWAYS0 ("Transition Time Follows:");
  m_log_evtime (&info->trans_time);

  }



