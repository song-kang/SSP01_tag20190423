/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER	*****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*		       1997 All Rights Reserved				*/
/*									*/
/* MODULE NAME : ml_repea.c						*/
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
/* m_log_repeas_req                                                     */
/************************************************************************/


ST_VOID m_log_repeas_req (REPEAS_REQ_INFO *info)
  {
  MLOG_CALWAYS0 ("Event Action Name Follows:");
  m_log_objname (&(info->evact_name));
  }



/************************************************************************/
/* m_log_repeas_resp                                                 */
/************************************************************************/


ST_VOID m_log_repeas_resp (REPEAS_RESP_INFO *info)
  {
  MLOG_CALWAYS1 ("Number of Event Enrollments : %lu",(ST_ULONG)info->num_of_ev_enroll);
  }

