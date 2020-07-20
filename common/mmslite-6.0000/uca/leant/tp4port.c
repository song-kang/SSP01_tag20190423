/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	1996-2003 All Rights Reserved					*/
/*									*/
/* MODULE NAME : tp4port.c						*/
/* PRODUCT(S)  : Lean-T Stack						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*	Functions for porting to new operating systems.			*/
/*	The purpose of these functions is to cause "tp4_timer_tick"	*/
/*	to be called on 1 second intervals.  These functions are	*/
/*	appropriate for MS-DOS or any other single tasking OS.		*/
/*	They may be replaced by any Operating System appropriate code	*/
/*	which causes the same result.					*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*	tp4_init_timer ()						*/
/*	tp4_check_timer ()						*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev     Comments					*/
/* --------  ---  ------   -------------------------------------------  */
/* 12/19/03  JRB    04     Use sGetMsTime instead of "time" function to	*/
/*			   avoid problems when system time changed.	*/
/* 09/11/00  JRB    03     Make sure tp4_check_timer called at least	*/
/*			     once a second. If NOT, write msg to log.	*/
/* 08/13/98  JRB    02     Lint cleanup.				*/
/* 05/27/97  JRB   7.00    MMSEASE 7.0 release.				*/
/* 07/17/96  JRB    01     Created					*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "tp4api.h"		/* User definitions for tp4	*/
#include "tp4_log.h"

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

static ST_DOUBLE oldtime;	/* milliseconds	*/
static ST_DOUBLE newtime;	/* milliseconds	*/

/************************************************************************/
/* tp4_init_timer							*/
/************************************************************************/
ST_VOID tp4_init_timer (ST_VOID)
  {
  oldtime = sGetMsTime ();
  }

/************************************************************************/
/* tp4_check_timer							*/
/************************************************************************/
ST_VOID tp4_check_timer (ST_VOID)
  {
#ifdef DEBUG_SISCO
static ST_BOOLEAN time_problem;	/* Timing problem detected.		*/
#endif

  newtime = sGetMsTime ();

#ifdef DEBUG_SISCO	/* Don't check for problem if can't log it.	*/
  /* Check time since last call.					*/
  /* This function should be called at least every second, so time diff	*/
  /* should be < 1000 ms. If difference is > 3000 ms, service loop may	*/
  /* be too slow.							*/
  if (newtime - oldtime > 3000.0)
    {
    TP_LOG_ERR1 ("WARNING: stack not serviced for %.3f seconds.", (newtime-oldtime)/1000.0);
    if (!time_problem)
      {
      /* Log this message only the first time it happens.	*/
      TP_LOGC_ERR0 ("MAY CAUSE UNEXPECTED TIMEOUTS, DISCONNECTS, ABORTS.");
      time_problem = SD_TRUE;
      }
    }
#endif

  /* Increment "oldtime" by 1000 ms increments as much as possible	*/
  /* without exceeding "newtime".					*/
  /* Call "tp4_timer_tick" once for each increment.			*/
  while (oldtime + 1000.0 < newtime)
    {
    tp4_timer_tick ();		/* Call function to process timer tick.	*/
    oldtime += 1000.0;
    }
  }

