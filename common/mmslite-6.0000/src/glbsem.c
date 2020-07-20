/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*                  1997-2005, All Rights Reserved                   	*/
/*									*/
/* MODULE NAME : glbsem.c						*/
/* PRODUCT(S)  : 							*/
/*									*/
/* MODULE DESCRIPTION :  Multi-thread support. 				*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 03/12/08  JRB     77    gs_free_semx: make sure fname initialized.	*/
/* 10/04/07  MDE     76    Tweaked LOGCFG_VALUE_GROUP/LOGCFGX_VALUE_MAP	*/
/* 05/23/07  NAV     75    gs_get_semx: conditionally call sGetMsTime   */
/* 02/13/06  DSF     74    Migrate to VS.NET 2005			*/
/* 06/17/05  EJV     73    UNIX, Linux: need str_util.h			*/
/* 06/10/05  DSF     72    Added an array to track calls to gs_free_semx*/
/* 06/07/05  EJV     71    Added S_MT_SUPPORT to avoid Linux errors.	*/
/* 05/23/05  EJV     70    Add gsLogMaskMapCtrl for parsing logcfg.xml  */
/*			   Moved gs_debug_sel from slog.c.		*/
/* 05/23/05  JRB     69    Move all gs_timer* functions to glbtimer.c	*/
/* 02/22/05  EJV     68    gs_timer_worker_thread: eliminated warnings  */
/* 01/10/05  DSF     67    Signal timer event after timer object is	*/
/*			   unlinked					*/
/*			   Don't call user callback if timer is 	*/
/*			   terminated					*/
/* 08/23/04  EJV     66    GS_TIMER: rpl elapsed w/ ST_DOUBLE expiration*/
/* 07/23/04  DSF     65    Set names of timer threads			*/
/* 07/22/04  DSF     64    Set timer resolution back to 10		*/
/* 07/02/04  EJV     63    gs_timer_thread: chg sleep from 10 to 0 ms.	*/
/* 06/17/04  DSF     62    Reset gs_timer_thread_stop in gs_timer_init()*/
/* 05/05/04  EJV     61    gs_timer_cleanup: added timeout parameter.	*/
/*			   Added timer worker threads to proc callbacks.*/
/*			   gs_timer_init: added min_workers, max_workers*/
/* 04/22/04  DSF     60    Initialized timer resolution to 1000 ms	*/
/* 01/21/04  EJV     59    Win Timers: changed to use UNIX code.	*/
/*			   Win impl preserved with _WIN32_timers define	*/
/* 12/01/03  EJV     58    gs_is_win_ver: chk for corr ret 1, 0, -1.	*/
/* 11/17/03  EJV     57    gs_timer_end: reverted moved gs_timer_remove.*/
/*			   _WIN32: reworked periodic timers >430000ms;	*/
/*			     All timer_id will go on delayed delete list*/
/* 10/15/03  JRB     56    Move gs_debug_sel to slog.c, slogl.c.	*/
/* 10/09/03  JRB     55    Enable gs_timer* functions for ALL systems.	*/
/* 07/29/03  DSF,EJV 54    gs_timer_end: moved gs_timer_remove fun up.	*/
/* 06/20/03  EJV     53    Del param from gs_mutex_get, gs_get_semx.	*/
/* 06/12/03  EJV     52    Redesigned mutex sems implementation.	*/
/* 06/06/03  JRB     51    Move _WIN32 code to new glbsem_w32.c		*/
/*			   Move UNIX code to new glbsem_unix.c		*/
/*			   Del OS2 code & "default" code.		*/
/*			   Del unused gs_*_sig functions for DEC UNIX.	*/
/*			   Add gs_util_mutex, init it in gs_init.	*/
/*			   Chg GLBSEM_LOG_ALWAYS* to SLOGALWAYS*.	*/
/* 06/03/03  EJV     50    _WIN32: changed for 64-bit compiler:		*/
/*			     last param cast in RaiseException		*/
/*			     gs_timer_callback last 3 parms to DWORD_PTR*/
/* 04/14/03  JRB     49	   Del intermediate functions for mutexes,	*/
/*			   macros in glbsem.h use gs_mutex_get/free.	*/
/*			   assert if gs_init fails (no way to recover).	*/
/* 04/04/03  JRB     48    Chg _ASSERTE calls to assert, otherwise must	*/
/*			   link DebugMultithreaded libs on Windows.	*/
/* 03/24/03  EJV     47    Removed logging from some MUTEX functions.	*/
/*			   gs_get_sem, gs_get_semx: added ASSERTE.	*/
/* 02/03/03  EJV     46    _WIN32: added gs_is_win_ver()		*/
/*                                 added gs_get_named_event_sem()	*/
/* 01/24/03  EJV     45    _WIN32 gs_start_thread: added comment.	*/
/* 01/14/03  EJV     44    Added gs_sleep().				*/
/* 11/01/02  EJV     43	   Use SISCO's link list with timers.		*/
/* 10/30/02  EJV     42    gs_timer_check_list: limit delay to 1 sec.	*/
/*			     Check if callback pending before deleting.	*/
/* 10/31/02  EJV     41    gs_wait_mult_event_sem: corr activity memset	*/
/* 07/29/02  EJV     40    Added gs_timer_get_resolution(),		*/
/*			   Corrected timer termination wait.		*/
/*			   Reverted timer _WIN32, added ptr wait list	*/
/* 02/06/02  EJV     39    Compile this module if S_MT_SUPPORT defined	*/
/* 12/04/01  KCR     38    cleared activity[] in gs_wait_mult_event_sem	*/
/* 10/31/01  EJV     37    _WIN32: changed to call _beginthreadex;	*/
/*			   Added gs_close_thread, gs_pulse_event_sem.	*/
/*			   Replaced _ALWAYS macros with _ERR or _NERR.	*/
/*			   _WIN32: gs_wait_thread now returns SD_FAILURE*/
/*			           on timeout.				*/
/*			   UNIX: gs_reset_event_sem now clears predicate*/
/* 10/18/01  JRB     36    Eliminate warning.				*/
/* 09/20/01  EJV     35    _WIN32 gs_timer_callback: check if pending	*/
/* 08/01/01  JRB     34    gs_get_semx fill in fname even if NOT logging*/
/* 07/30/01  EJV     33    Added gs_timer_set_resolution() fun.		*/
/*			   _WIN32: reworked gs_timer functions.		*/
/* 07/25/01  EJV     32    Changed gs_timer_resolution from 1000 to 10ms*/
/* 07/24/01  EJV     31    _WIN32: added gs_timer support.		*/
/* 05/17/01  EJV     30    _WIN32: eliminated compile warnings.		*/
/* 04/04/01  DSF     29    Added gs_set_thread_name for WIN32		*/
/* 03/27/01  EJV     28    UNIX: gs_wait_event_sem, mutex the whole fun	*/
/* 03/16/01  EJV     27    UNIX: added check to gs_free_event_sem.	*/
/*			   UNIX: set thread attribute to joinable.	*/
/*			   Removed #undef NDEBUG (now in make file)	*/
/* 03/12/01  EJV     26    UNIX: added/changed few FLOW slogs.		*/
/*                         UNIX: corr gs_wait_event_sem, predicate=0	*/
/*		           Removed <assert.h>, already in sysincs.h	*/
/*			   Changed assert to _ASSERTE - def in sysincs.h*/
/* 03/08/01  EJV     25    UNIX: Corrected type to ST_BOOLEAN for	*/
/*			   static var gs_timer_thread_stop. Corr logs.	*/
/* 03/07/01  EJV     24    UNIX: init attr before calling create thread.*/
/*                         In gs_wait_event_sem check for predicate=1	*/
/*                         before entering the wait.			*/
/* 02/19/01  EJV     23    _AIX: eliminated need to call gs_timer_init.	*/	
/* 02/14/01  EJV     22    Removed some DEBUG_SISCO around slog macros	*/
/*			   AIX: added timer functions gs_timer_xxx.	*/
/* 01/16/01  EJV     21    Moved GET_THREAD_ID define to glbsem.h	*/
/* 12/27/00  EJV     20    _WIN32: Renamed glbCritSetion for easy calls.*/
/*			   Added check for gs_already_inited.		*/
/* 12/20/00  EJV     19    Ported to AIX. Revised DEC UNIX.		*/
/* 12/13/00  EJV     18    Added gs_mutex_... user mutex functions.	*/
/*			   Added gs_track for faster SISCO MUTEX.	*/
/*			   Removed currSemOwner, semCount		*/
/*			   Added few DEBUG_SISCO for __alpha.		*/
/* 12/06/00  EJV     17    Changed gs_get_event_sem to accept arg.	*/
/*                         USE_MANUAL_RESET_SEM define not necessary.	*/
/* 10/25/00  JRB     16    Del gs_chk_thread_id. Found better way to	*/
/*			   prevent errors (see "_MT" chk in glbsem.h).	*/
/* 10/06/00  EJV     15    Ported to DEC UNIX (__alpha) pthread funcs.	*/
/*			   Deleted unused: myThreadId,...		*/
/* 10/06/00  EJV     14    _WIN32: corrected logging statements		*/
/* 04/27/00  MDE     13    Lint cleanup					*/
/* 03/14/00  JRB     12    Chg to allow breakpoint before assert.	*/
/* 01/21/00  JRB     11    Del gs_install. Del function pointers.	*/
/*                         gs_get_sem call gs_init if not already done.	*/
/*			   Add "gs_chk_thread_id".			*/
/* 12/16/99  NAV     10    in gs_free_semx watch for GS_LOG_FLOW	*/
/* 09/13/99  MDE     09    Added SD_CONST modifiers			*/
/* 08/05/99  JRB     08    Chged "gs_get_event_sem" to "auto-reset"	*/
/*			   unless USE_MANUAL_RESET_SEM defined.		*/
/*			   #ifdef'd out "gs_reset_event_sem".		*/
/*			   Added partial support for __OS2__.		*/
/* 03/01/99  DSF     07    Corrected #define releaseMutexSem for	*/
/*			   non-DEBUG_SISCO version			*/
/* 01/22/99  DSF     06    Use Critical Section instead of Mutex (faster)*/
/* 10/08/98  MDE     05    Migrated to updated SLOG interface		*/
/* 06/05/98  MDE     04    Fixed uninitialized 'rc'			*/
/* 05/05/98  DSF     03    Added SD_TIMEOUT				*/
/* 12/11/97  KCR     02    Added gs_wait_mult_event_sem for _WIN32	*/
/* 10/27/97  EJV     01    For !_WIN32 implementations:			*/
/*			     Added typecast to define GET_THREAD_ID();	*/
/*			     Changed func gs_get_event_sem return from	*/
/*			     from SD_FAILURE to (ST_EVENT_SEM) 0.	*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#if defined (_WIN32)
#pragma warning(disable : 4996)
#endif

#include "glbtypes.h"
#include "sysincs.h"
#include "mem_chk.h"
#include "slog.h"
#include "glbsem.h"
#include "str_util.h"

#if defined(S_MT_SUPPORT)

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/
/************************************************************************/
/* GLOBAL VARIABLES							*/

ST_UINT gs_debug_sel = GS_LOG_ERR | GS_LOG_NERR;

#ifdef DEBUG_SISCO
SD_CONST ST_CHAR *SD_CONST _glbem_flow_logstr = "GS_LOG_FLOW";
SD_CONST ST_CHAR *SD_CONST _glbem_err_logstr = "GS_LOG_ERR";
SD_CONST ST_CHAR *SD_CONST _glbem_nerr_logstr = "GS_LOG_NERR";

LOGCFGX_VALUE_MAP gsLogMaskMaps[] =
  {
    {"GS_LOG_ERR",	GS_LOG_ERR,	&gs_debug_sel, _LOGCFG_DATATYPE_UINT_MASK, NULL, "Error"},
    {"GS_LOG_NERR",	GS_LOG_NERR,	&gs_debug_sel, _LOGCFG_DATATYPE_UINT_MASK, NULL, "Notice"},
    {"GS_LOG_FLOW",	GS_LOG_FLOW,	&gs_debug_sel, _LOGCFG_DATATYPE_UINT_MASK, NULL, "Flow"},
  };

LOGCFG_VALUE_GROUP gsLogMaskMapCtrl =
  {
  {NULL,NULL},
  "SemaphoreLogMasks",	  /* Parent Tag */
  sizeof(gsLogMaskMaps)/sizeof(LOGCFGX_VALUE_MAP),
  gsLogMaskMaps
  };
#endif /* DEBUG_SISCO */

/* variables to evalueate semaphores locking performance		*/
ST_DOUBLE gs_hwMutexTime;
ST_DOUBLE gs_hwEventTime;

ST_MUTEX_SEM gs_glb_mutex;		/* global SISCO MUTEX semaphore	*/
ST_MUTEX_SEM gs_util_mutex;	/* UTILITY MUTEX semaphore: for "low-level"*/
				/* util functs (slog,mem_chk,stime,etc.)*/

ST_BOOLEAN gs_already_inited = SD_FALSE;

#if defined(DEBUG_SISCO) 	
/* The debug version keeps a stack of sem owners call locations		*/
/* which can be logged to aid in debugging.				*/
/* These variables are only changed by a thread that owns the sem, but	*/
/* can be read (for logging) by 'gs_log_sem_state', possibly resulting	*/
/* in a 'torn' log if the called is not careful.			*/

#define MAX_SEM_NEST_TRACK  20 
ST_UINT  gs_track;		/* This variable when set to value >0	*/
				/* during an application startup allows	*/
				/* tracking of MUTEX or EVENT sem.	*/
ST_INT   gs_currSemOwnerIndex;
ST_CHAR  gs_currSemOwnerFile[MAX_SEM_NEST_TRACK][SLOG_MAX_FNAME+1];
ST_INT   gs_currSemOwnerLine[MAX_SEM_NEST_TRACK];
ST_CHAR  gs_currSemFreeFile[MAX_SEM_NEST_TRACK][SLOG_MAX_FNAME+1];
ST_INT   gs_currSemFreeLine[MAX_SEM_NEST_TRACK];

#endif	/* DEBUG_SISCO */


	/*---*---*---*---*---*---*---*---*---*---*---*---*---*--*/
	/*							*/
	/*	SISCO's GLOBAL MUTEX SEMAPHORE FUNCTIONS	*/
	/*							*/
	/*---*---*---*---*---*---*---*---*---*---*---*---*---*--*/

/************************************************************************/
/*                       gs_init					*/
/*----------------------------------------------------------------------*/
/* Initialize global SISCO MUTEX semaphore.				*/
/* This function will be called from the gs_get_sem or gs_get_semx,	*/
/* it does not have to be called from user application.			*/
/* Parameters:								*/
/*	none								*/
/* Return values:							*/
/*	SD_SUCCESS	function successful				*/
/*	SD_FAILURE	error occurred					*/
/************************************************************************/

ST_RET gs_init (ST_VOID)
  {
ST_RET rc;

/* !!! To avoid infinite loop with SLOG do not log from this function.	*/

  if (gs_already_inited)
    return (SD_SUCCESS);

/* WARNING: The gs_util_mutex should not be changed to use named mutex	*/
/*          because logging in the code handling named mutex will cause	*/
/*          infinite loop.						*/

/* Initialize the MUTEX semaphores. Both must succeed, so OR the returns.*/
  rc = gs_mutex_create (&gs_glb_mutex);
  rc |= gs_mutex_create (&gs_util_mutex);

/* Set up to track the semaphore owners by nesting level		*/
#if defined(DEBUG_SISCO) 	
  gs_currSemOwnerIndex = -1;
#endif

  if (rc == SD_SUCCESS)
    gs_already_inited = SD_TRUE;
  else
    assert (0);	/* nothing will work properly if this fails.	*/

  return (rc);
  }

/************************************************************************/
/*                       gs_log_sem_state				*/
/*----------------------------------------------------------------------*/
/* Log the global SISCO MUTEX semaphore owner stack.			*/
/* Parameters:								*/
/*	none								*/
/* Return values:							*/
/*	none								*/
/************************************************************************/

ST_VOID gs_log_sem_state (ST_VOID)
  {
#if defined(DEBUG_SISCO) 	
ST_INT i;

  if (!gs_already_inited)	/* Make sure gs is initialized.		*/
    return;

  SLOGALWAYS0 ("GLBSEM: Semaphore owner stack (oldest to latest) :"); 
  for (i = 0; i <= gs_currSemOwnerIndex && i < MAX_SEM_NEST_TRACK; ++i)
    {
    SLOGCALWAYS3 ("  %d) File %s, Line %d", 
  			i+1, gs_currSemOwnerFile[i], gs_currSemOwnerLine[i]);
    }
  if (gs_currSemOwnerIndex >= MAX_SEM_NEST_TRACK)
    SLOGCALWAYS0 ("  Sem's nested too deep to track further");
#endif
  }

/************************************************************************/
/*                       gs_get_semx					*/
/*----------------------------------------------------------------------*/
/* Lock (obtain ownership) the global SISCO MUTEX semaphore.		*/
/* This debug version function keeps track of the sem owner stack.	*/
/* If compiled without DEBUG_SISCO it just falls through to gs_get_sem.	*/
/* If compiled with DEBUG_SISCO the gs_track variable need to be set to	*/
/* value >0 to enable the tracking code. This way logging can be used	*/
/* by an application without degradation of the MUTEX sem performance.	*/
/* In addition the gs_debug_sel need to be set to GS_LOG_FLOW to 	*/
/* log the tracking results.			 			*/
/* Parameters:								*/
/*	srcFile		ptr to source code file name calling this fun	*/
/*      srcLineNum	source code line number				*/
/* Return values:							*/
/*	none								*/
/* CRITICAL: gs_get_semx must NOT be called from any slog or stime	*/
/*	functions. It calls slog and stime functions which would cause	*/
/*	an infinite loop. Slog and stime must call gs_mutex_get.	*/
/************************************************************************/

ST_VOID gs_get_semx (SD_CONST ST_CHAR *srcFile, ST_INT srcLineNum)
  {
#if defined(DEBUG_SISCO) 	
ST_CHAR fname[SLOG_MAX_FNAME+1];
ST_DOUBLE startTime;
ST_DOUBLE endTime;
ST_DOUBLE elapsedTime;

  if (!gs_already_inited)	/* Make sure gs is initialized.		*/
    gs_init ();

  if (gs_track)
    startTime = sGetMsTime ();

  gs_mutex_get (&gs_glb_mutex);

  if (gs_track)
    {
    endTime = sGetMsTime ();

    elapsedTime = endTime - startTime;
    if (elapsedTime > gs_hwMutexTime)
      gs_hwMutexTime = elapsedTime;

    if (srcFile != NULL)
      slogTrimFileName (fname, srcFile);
    else
      strcpy (fname, "Unknown");

    if (gs_debug_sel & GS_LOG_FLOW)
      {
      GLBSEM_LOG_FLOW2 ("GLBSEM: File %s, Line %d has the mutex", 
    			fname, srcLineNum);
      GLBSEM_LOG_CFLOW2 ("        took %.3f sec (hw = %.3f)", 
			elapsedTime/1000, gs_hwMutexTime/1000); 
      }

    ++gs_currSemOwnerIndex;
    if (gs_currSemOwnerIndex < MAX_SEM_NEST_TRACK)
      {
      strcpy (gs_currSemOwnerFile[gs_currSemOwnerIndex], fname);
      gs_currSemOwnerLine[gs_currSemOwnerIndex] = srcLineNum;
      }
    else 
      {
      GLBSEM_LOG_CFLOW0  ("        Nested too deep to track"); 
      }
    }
#else  /* Not DEBUG_SISCO */

  gs_mutex_get (&gs_glb_mutex);

#endif /* DEBUG_SISCO */
  }		        

/************************************************************************/
/*                       gs_free_semx					*/
/*----------------------------------------------------------------------*/
/* Unlock (release ownership) the global SISCO MUTEX semaphore.		*/
/* This debug version function keeps track of the sem owner stack.	*/
/* If compiled without DEBUG_SISCO is just falls through to gs_free_sem.*/
/* If compiled with DEBUG_SISCO the gs_track variable need to be set to	*/
/* value >0 to enable the tracking code. This way logging can be used	*/
/* by an application without degradation of the MUTEX sem performance.	*/
/* In addition the gs_debug_sel need to be set to GS_LOG_FLOW to log	*/
/* the tracking results.			 			*/
/* Parameters:								*/
/*	srcFile		ptr to source code file name calling this fun	*/
/*      srcLineNum	source code line number				*/
/* Return values:							*/
/*	none								*/
/* CRITICAL: gs_free_semx must NOT be called from any slog or stime	*/
/*	functions. It calls slog and stime functions which would cause	*/
/*	an infinite loop. Slog and stime must call gs_mutex_free.	*/
/************************************************************************/

ST_VOID gs_free_semx (SD_CONST ST_CHAR *srcFile, ST_INT srcLineNum)
  {
#if defined(DEBUG_SISCO) 	
ST_CHAR fname[SLOG_MAX_FNAME+1];
ST_INT idx;

  if (!gs_already_inited)	/* Make sure gs is initialized.		*/
    {
    GLBSEM_LOG_ERR0 ("GLBSEM gs_free_semx error: global mutex semaphore not initialized");
    return;
    }

  if (gs_track)
    {
    if (srcFile != NULL)
      slogTrimFileName (fname, srcFile);
    else
      strcpy (fname, "Unknown");

    GLBSEM_LOG_FLOW2 ("GLBSEM: File %s, Line %d freeing the semaphore", 
    			fname, srcLineNum);

    idx = gs_currSemOwnerIndex;
    --gs_currSemOwnerIndex;
    if (gs_currSemOwnerIndex == -1)
      {
      GLBSEM_LOG_CFLOW0 ("        The semaphore should now be free"); 
      }
    else if (gs_currSemOwnerIndex >= 0 && gs_currSemOwnerIndex < MAX_SEM_NEST_TRACK)
      {
      GLBSEM_LOG_CFLOW2 ("        File %s, Line %d now has  the semaphore", 
			gs_currSemOwnerFile[gs_currSemOwnerIndex],
			gs_currSemOwnerLine[gs_currSemOwnerIndex]);
      strcpy (gs_currSemFreeFile[idx], fname);
      gs_currSemFreeLine[idx] = srcLineNum;
      if (strcmpi (gs_currSemFreeFile[idx], gs_currSemOwnerFile[idx]))
        {
        GLBSEM_LOG_ERR2  ("Possible problem: %s (%d)", gs_currSemOwnerFile[idx], 
	                                               gs_currSemOwnerLine[idx]); 
	}
      }
    else if (gs_currSemOwnerIndex >= 0 && gs_currSemOwnerIndex >= MAX_SEM_NEST_TRACK)
      {
      GLBSEM_LOG_CFLOW0  ("        Nested too deep to track"); 
      }
    else if (gs_currSemOwnerIndex < -1)
      {
      GLBSEM_LOG_ERR0 ("GLBSEM gs_free_semx error: Sem track index negative");
      gs_currSemOwnerIndex = -1;
      }
    }
#endif /* DEBUG_SISCO */

  gs_mutex_free (&gs_glb_mutex);       
  }

#endif /* defined(S_MT_SUPPORT) */
