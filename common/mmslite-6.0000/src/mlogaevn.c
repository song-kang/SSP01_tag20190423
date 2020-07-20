/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,       */
/*                  1986 - 1994 All Rights Reserved                     */
/*                                                                      */
/* MODULE NAME : mlogaevn.c                                             */
/* PRODUCT(S)  : MMS-EASE 250-001                                       */
/*                                                                      */
/* MODULE DESCRIPTION :                                                 */
/*      This module contains user defined logging functions for         */
/*      mmsaevn.c functions that require logging.                       */
/*                                                                      */
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :                            */
/*                                                                      */
/* MODIFICATION LOG :                                                   */
/*  Date     Who   Rev                  Comments                        */
/* --------  ---  ------   -------------------------------------------  */
/* 01/20/10  JRB     08    Fix log messages.				*/
/* 03/11/04  GLB     07    Remove "thisFileName"                    	*/
/* 09/13/99  MDE     06    Added SD_CONST modifiers			*/
/* 04/14/99  MDE     05    Removed unnecessary include files		*/
/* 03/20/98  JRB     04    Don't need mmsop_en.h anymore.		*/
/* 08/15/97  MDE     03    BTOD handling changes			*/
/* 08/04/97  MDE     02    Removed DEBUG_SISCO code			*/
/* 06/09/97  MDE     01    Added MMS-LITE ifdefs			*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#ifndef MMS_LITE
#include "mms_usr.h"		/* to access MMS fucntions, variables   */
#else
#include "mmsdefs.h"
#endif
#include "mms_pevn.h"
#include "asn1defs.h"

#include "mloguser.h"

/************************************************************************/
/*			m_log_evtime					*/
/* This routine is used to print an event time. 			*/
/************************************************************************/

ST_VOID m_log_evtime (EVENT_TIME *info)
  {
ST_VOID m_log_evt_tod (MMS_BTOD *btod);

  switch (info->evtime_tag)
    {
    case (0)  :
      m_log_evt_tod (&info->evtime.time_of_day );
      break;

    case (1)  :
      MLOG_CALWAYS1 ("Time Sequence Id: %lu", (ST_ULONG)info->evtime.time_seq_id);
      break;

    case (2)  :
      MLOG_CALWAYS0 ("Undefined.");
      break;

    default   :
      MLOG_CALWAYS1 ("Invalid Tag : %d", info->evtime_tag );
      break;
    }
  }

/************************************************************************/
/*			m_log_evt_tod					*/
/* This routine is used to slog an event time time of day.		*/
/************************************************************************/

ST_VOID m_log_evt_tod (MMS_BTOD *info)
  {
  MLOG_CALWAYS0 ("Binary Time of Day : ");
  MLOG_CALWAYS1 ("   Milliseconds since midnight: %ld", (ST_LONG)info->ms);
  if ( info->form == MMS_BTOD6)
    {
    MLOG_CALWAYS1 ("   Days since 1/1/1984        : %ld", (ST_LONG)info->day);
    }
  else
    {
    MLOG_CALWAYS0 ("   Date not present");
    }
  }

