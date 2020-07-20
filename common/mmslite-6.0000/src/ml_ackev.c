/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER	*****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*		       1997 All Rights Reserved				*/
/*									*/
/* MODULE NAME : ml_ackev.c						*/
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
/* m_log_ackevnot_req                                                   */
/************************************************************************/

ST_VOID m_log_ackevnot_req (ACKEVNOT_REQ_INFO *info)
  {
  MLOG_CALWAYS0 ("Event Enrollment Name Follows:");
  m_log_objname (&(info->evenroll_name));

  switch (info->ack_state)
    {
    case 0 :
      MLOG_CALWAYS1 ("Acknowledge State : %d, Disabled.",
                                                info->ack_state);
      break;
    case 1 :
      MLOG_CALWAYS1 ("Acknowledge State : %d, Idle.",
                                                info->ack_state);
      break;
    case 2 :
      MLOG_CALWAYS1 ("Acknowledge State : %d, Active.",
                                                info->ack_state);
      break;
    default:
      MLOG_CALWAYS1 ("Acknowledge State : %d, Invalid ACK State.",
                                                info->ack_state);
      break;
    }
  MLOG_CALWAYS0 ("Event Time Follows:");
  m_log_evtime ( &(info->evtime) );

  if (info->ackec_name_pres)
    {
    MLOG_CALWAYS0 ("Ack Event Condition Name Follows:");
    m_log_objname ( &(info->ackec_name) );
    }
  else
    {
    MLOG_CALWAYS0 ("Ack Event Condition Name : Not present");
    }

  }



