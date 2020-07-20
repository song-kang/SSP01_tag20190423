/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*                  1997-2011, All Rights Reserved                   	*/
/*									*/
/* MODULE NAME : glbsem.h						*/
/* PRODUCT(S)  : 							*/
/*									*/
/* MODULE DESCRIPTION : Multi-thread support defines and prototypes.	*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 02/06/12  JRB	   Make ST_SEMAPHORE functions for _WIN32 only.	*/
/* 07/11/11  DSF           Support for Windows semaphores		*/
/* 04/01/11  EJV           sun, __hpux: let mak define the POSIX level	*/
/*			     (_POSIX_C_SOURCE define). Generate compile	*/
/*			     error if min level not defined.		*/
/* 01/14/11  EJV           Added gs_translate_name for _WIN32.		*/
/* 11/18/10  MDE     63    Added _ex versions with file/line parameters	*/
/* 06/01/08  EJV     61    Ported to Windows Vista.			*/
/* 01/15/07  EJV     60    Chg S_LOCK_RESOURCES to S_LOCK_UTIL_RESOURCES*/
/* 12/15/06  MDE     59    Added S_UNLOCK_CR_AND_RETURN_VAL			*/
/* 10/25/06  EJV     58    Ported to HP-UX.				*/
/* 07/13/06  EJV     57    Sun: changed the test for POSIX define.	*/
/* 03/23/06  EJV     56    Added ( ) in #if a || b || (c && d).		*/
/* 01/30/06  GLB     55    Integrated porting changes for VMS           */
/* 08/04/05  EJV     54    Reworked defs for UNIX and LINUX		*/
/* 04/12/05  DSF     53    Added refCount member to GS_MUTEX (Windows)	*/
/* 01/10/05  DSF     52    Added event member to GS_TIMER		*/
/* 01/06/05  DSF     51    Added owner member to GS_MUTEX (Windows)	*/
/* 01/14/05  EJV     50    Change removed.				*/
/* 06/09/04  EJV     49    Added multithread support for sun.		*/
/* 08/23/04  EJV     48    GS_TIMER: rpl elapsed w/ ST_DOUBLE expiration*/
/* 05/10/04  EJV     47    gs_timer_cleanup: added timeout parameter.	*/
/*			   gs_timer_init: added min_workers, max_workers*/
/*			   Added GS_TIMER_WORKER_... defines.		*/
/*			   GS_TIMER: added/moved fields.		*/
/* 01/21/04  JRB     46    Add LYNX support.				*/
/* 01/21/04  EJV     45    Win Timers: changed to use UNIX code.	*/
/*			   Win impl preserved with _WIN32_timers define	*/
/* 12/03/03  EJV     44    Add SD_WIN_VER_2003_AND_LATER, replaced	*/
/*			   SD_WIN_VER_NET with SD_WIN_VER_2003;		*/
/*			   gs_is_win_ver(): chg return ST_RET to ST_INT	*/
/* 12/01/03  EJV     43    Added SD_WIN_VER_2K_AND_LATER.		*/
/* 11/04/03  JRB     42    Del gs_sleep (use sMsSleep).			*/
/* 10/24/03  JRB     41    Move gs_debug_sel to slog.h.			*/
/* 09/29/03  JRB     40    Add "linux" support.				*/
/*			   Enable gs_timer* functions for ALL systems.	*/
/* 06/20/03  EJV     39    Redesigned mutex sems implementation.	*/
/* 06/11/03  EJV     38    gs_named_mutex_xxx, gs_get_named_event_sem	*/
/*			     defined for all systems.			*/
/*			   __alpha: removed gs_xxx_sig and related code.*/
/* 06/10/03  EJV     37    _WIN32: added gs_named_mutex_xxx functions.	*/
/* 06/06/03  JRB     36    S_LOCK_RESOURCES use new gs_util_mutex.	*/
/*			   Use new "Base" macros SLOG_*, SLOGC_*, SLOGH.*/
/*			   Del GLBSEM_LOG_ALWAYS*, use SLOGALWAYS*.	*/
/* 06/03/03  EJV     35    _WIN32: changed for 64-bit compiler:		*/
/*			     ST_THREAD_HANDLE to uintptr_t		*/
/* 04/14/03  JRB     34    Chg mutex macros to use gs_mutex_get/free.	*/
/*			   Chg gs_get/free_sem functions to macros.	*/
/* 03/03/24  EJV     33    Added _glbem_err_logstr & _glbem_nerr_logstr;*/
/*			   S_LOCK_RESOURCES, S_UNLOCK_RESOURCES call	*/
/*			     now non-logging gs_get_sem & gs_free_sem.	*/
/* 02/03/03  EJV     32    _WIN32: added gs_is_win_ver(), SD_WIN_.. defs*/
/*                                 added gs_get_named_event_sem()	*/
/* 01/14/03  EJV     31    Added gs_sleep().				*/
/* 11/01/02  EJV     30	   Use SISCO's link list in GS_TIMER.		*/
/* 07/29/02  EJV     29    Added gs_timer_get_resolution(),		*/
/*			   GS_TIMER: made more fields common to UNIX,Win*/
/*			     Added field delay_start for WIN32.		*/
/* 04/09/02  EJV     28    __alpha: moved pthread.h up (cleaner code)	*/
/* 02/06/02  EJV     27    _AIX: check for _THREAD_SAFE compiler define	*/
/* 10/31/01  EJV     26    _WIN32: added code to call _beginthreadex;	*/
/*			   Added gs_close_thread, gs_pulse_event_sem;	*/
/*			   Added _ERR, _NERR log macros			*/
/*			   UNIX: chg predicate to ST_INT from ST_BOOLEAN*/
/*                         UNIX: added manualReset to GS_EVENT_SEM.	*/
/* 09/20/01  EJV     25    _WIN32: added pending field to GS_TIMER.	*/
/* 08/01/01  JRB     24    Add comment explaining S_LOCK_RES..		*/
/* 07/30/01  EJV     23    Added gs_timer_set_resolution().Chg GS_TIMER.*/
/*			   Added ST_THREAD_RET, ST_THREAD_RET_VAL macros*/
/*			   and changed gs_start_thread arg.		*/
/* 07/25/01  DSF     22    Use <> when including windows.h		*/
/* 07/24/01  EJV     21    _WIN32: added gs_timer support.		*/
/* 04/04/01  DSF     20    Added gs_set_thread_name for WIN32		*/
/* 03/15/01  EJV     19    UNIX: corrected spelling of REENTRANT.	*/
/* 03/07/01  EJV     18    Added slog macros for !DEBUG_SISCO		*/
/* 02/19/01  EJV     17    _AIX: eliminated need to call gs_timer_init.	*/	
/* 02/16/01  EJV     16	   AIX: added timer functions gs_times_xxx.	*/
/* 01/16/01  EJV     15    Moved GET_THREAD_ID define here from glbsem.c*/
/* 12/20/00  EJV     14    Ported to AIX. Revised DEC UNIX.		*/
/* 12/13/00  EJV     13    Added gs_mutex_... funs for multiple mutex	*/
/*			   semaphores support. Added ST_MUTEX_SEM.	*/
/* 12/13/00  EJV     12    Removed USE_MANUAL_RESET_SEM.		*/
/* 12/06/00  EJV     11    Changed gs_get_event_sem to accept arg.	*/
/* 10/25/00  JRB     10    For _WIN32, define S_MT_SUP.. if _MT defined.*/
/* 10/06/00  EJV     09    Ported to DEC UNIX (__alpha) pthread funcs.	*/
/*			   Added few GLBSEM_LOG_FLOWn macros.		*/
/*			   Deleted gs_chk_thread_id(), check _REENTRANT	*/
/* 01/21/00  JRB     08    Del *sem_fun & *sem_funx function pointers.	*/
/*			   Chg macros to call functions directly.	*/
/*			   S_GS_INSTALL, gs_install, S_GS_INIT obsolete.*/
/*			   Chg macros if !S_MT_SUPPORT to call gs_chk...*/
/* 09/13/99  MDE     07    Added SD_CONST modifiers			*/
/* 08/02/99  JRB     06    Chged "gs_get_event_sem" to "auto-reset" &	*/
/*			   #ifdef'd out "gs_reset_event_sem".		*/
/* 10/08/98  MDE     05    Migrated to updated SLOG interface		*/
/* 05/05/98  DSF     04    Added SD_TIMEOUT				*/
/* 12/11/97  KCR     03    Added gs_wait_mult_event_sem for _WIN32	*/
/* 10/06/97  DSF     02    Modified the macros used by SLOG such that	*/
/*			   they could be used by SLOG in DEBUG_SISCO 	*/
/*			   mode						*/
/* 08/15/97  MDE     01    Changed default ST_xxx to ST_VOID *		*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#ifndef GBLSEM_INCLUDED
#define GBLSEM_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "slog.h"
#include "stime.h"
#include "gen_list.h"

/************************************************************************/
#define SD_TIMEOUT		2

/************************************************************************/
/* Logging masks and variable						*/

#define GS_LOG_FLOW 		0x0001 
#define GS_LOG_ERR 		0x0002
#define GS_LOG_NERR 		0x0004 

extern SD_CONST ST_CHAR *SD_CONST _glbem_flow_logstr;
extern SD_CONST ST_CHAR *SD_CONST _glbem_err_logstr;
extern SD_CONST ST_CHAR *SD_CONST _glbem_nerr_logstr;

/* Mutex types: unnamed (local) can be used to synchronize threads in	*/
/* a process, and named (global) can be used to synchronize processes	*/
/* and threads within a process.					*/
#define GS_MUTEX_UNNAMED		0
#define GS_MUTEX_NAMED			1

/************************************************************************/
/* Just for convenience ... */
#define S_UNLOCK_CR_AND_RETURN()        {S_UNLOCK_COMMON_RESOURCES(); return;}
#define S_UNLOCK_CR_AND_RETURN_VAL(_rc) {S_UNLOCK_COMMON_RESOURCES(); return (_rc);}

/************************************************************************/
/************************************************************************/
/* WINDOWS 95/98/NT/2000						*/
/* These typedefs, defines, and function prototypes are for use with	*/
/* Windows only.							*/
/************************************************************************/

#if defined(_WIN32)				/* VC++, 32-Bit		*/

#include <windows.h>

/* The VC++ compiler automatically defines (_MT) if Multithread libs	*/
/* are selected. The user clearly wants multithread support, so		*/
/* S_MT_SUPPORT "MUST" be enabled.					*/
/* DEBUG: something similar should be done for any system that supports	*/
/*        multithreading (e.g. OS2).					*/

#if defined(_MT) && !defined(S_MT_SUPPORT)
#define S_MT_SUPPORT
#endif

#define ST_THREAD_ID 		unsigned

typedef struct gs_mutex_tag
  {
  ST_INT		mutexType;	/* GS_MUTEX_UNNAMED (_NAMED)	*/
  ST_THREAD_ID		owner;
  ST_THREAD_ID		refCount;
  union
    {
    HANDLE		hMutex;		/* for named mutex & unnamed special	*/
    CRITICAL_SECTION    cs;		/* for unnamed mutex	       		*/
    } u; 
  } GS_MUTEX;

#define GET_THREAD_ID()		GetCurrentThreadId()
#define ST_MUTEX_SEM 		GS_MUTEX
#define ST_EVENT_SEM 		HANDLE
#define ST_SEMAPHORE		HANDLE
#if (_MSC_VER >= 1300)
#define ST_THREAD_HANDLE 	uintptr_t
#else
#define ST_THREAD_HANDLE 	unsigned long
#endif
#define ST_THREAD_ARG 		void *
#define ST_THREAD_CALL_CONV	__stdcall
#define ST_THREAD_RET 		unsigned
#define ST_THREAD_RET_VAL	0

/* defines to distinguish Windows versions passed to gs_is_win_ver()	*/
#define SD_WIN_VER_95_98_Me		1 	/* 95, 98, Me			*/
#define SD_WIN_VER_NT_AND_LATER		2 	/* NT 3.51, NT 4.0 and above	*/
#define SD_WIN_VER_2K_AND_LATER		3 	/* 2000 and above 		*/
#define SD_WIN_VER_XP_AND_LATER		4 	/* XP and above			*/
#define SD_WIN_VER_2003_AND_LATER	5 	/* 2003 and above		*/
#define SD_WIN_VER_VISTA_AND_LATER	6 	/* Vista and above		*/

/* specific Windows version */
#define SD_WIN_VER_95		10
#define SD_WIN_VER_98		20
#define SD_WIN_VER_Me		30
#define SD_WIN_VER_NT_351	40
#define SD_WIN_VER_NT_40	50
#define SD_WIN_VER_2000		60
#define SD_WIN_VER_XP		70
#define SD_WIN_VER_2003		80
#define SD_WIN_VER_VISTA	90

#endif  /* defined(_WIN32) */


/************************************************************************/
/************************************************************************/
/************************************************************************/
/* UNIX, Linux, ...							*/
/* These typedefs, defines, and function prototypes are for use with	*/
/* UNIX, Linux,... only							*/
/* Implemented functions are complying with the "POSIX 1003.1c" standard.*/
/************************************************************************/

#if defined(_AIX) || defined(sun) || defined(__hpux) || \
    defined(linux) || defined(__LYNX) || \
   (defined(__alpha) && !defined(__VMS)) || /* Tru64 alpha   */  \
   (defined(__ALPHA) && defined(__VMS))     /* OpenVMS alpha */  

#if (defined(__alpha) && !defined(__VMS)) || /* Tru64 alpha   */  \
    (defined(__ALPHA) && defined(__VMS))     /* OpenVMS alpha */ 
/* If the code is compiled with -pthread switch then the _REENTRANT	*/
/* symbol is defined. This means that there should be a multithread	*/
/* support for the code (the S_MT_SUPPORT should be defined).		*/

#if defined(_REENTRANT) && !defined(S_MT_SUPPORT)
#define		S_MT_SUPPORT
#endif

/* According to the POSIX standard this define should be included in 	*/
/* every application module. The <standards.h> included in <pthread.h>	*/
/* defines the _POSIX_C_SOURCE to be 199506L.				*/

#define _POSIX_C_SOURCE	199506L
#endif  /* defined(__alpha) && !defined(__VMS) */

#if defined(_AIX)
/* If the compiler defines _THREAD_SAFE we should build the code	*/
/* with multithread support (the S_MT_SUPPORT should be defined).	*/

#if defined(_THREAD_SAFE) && !defined(S_MT_SUPPORT)
#define		S_MT_SUPPORT
#endif
#endif  /* defined(_AIX) */

#if defined(sun) || defined(__hpux)
/* On Solaris 9,10 an application must define _POSIX_C_SOURCE >=199506L	*/
/* IN EVERY MODULE OF AN APPLICATION to get the multithread support.	*/

#if !defined(_POSIX_C_SOURCE)
#error Must define _POSIX_C_SOURCE >= 199506L to compile this module
#endif

#if (_POSIX_C_SOURCE-0 < 199506L)
#error Must define _POSIX_C_SOURCE >= 199506L to compile this module
#endif

#if (_POSIX_C_SOURCE-0 >= 199506L) && !defined(S_MT_SUPPORT)
#define		S_MT_SUPPORT
#endif
#endif  /* defined(sun) || defined(__hpux) */

#if defined(linux) || defined(__LYNX)
/* If the compiler defines _THREAD_SAFE, we should build the code	*/
/* with multithread support (the S_MT_SUPPORT should be defined).	*/

#if (defined(_THREAD_SAFE) || defined(_REENTRANT)) && !defined(S_MT_SUPPORT)
#define		S_MT_SUPPORT
#endif
#endif	/* defined(linux)	*/

#if defined(S_MT_SUPPORT)
#include <pthread.h>
typedef struct gs_event_sem_tag
  {
  ST_INT		predicate;	/* predicate to change value	*/
  pthread_mutex_t  	mutex;		/* mutex sem protecting the cond*/
  pthread_cond_t	cond;		/* condition variable		*/
  ST_BOOLEAN		manualReset;	/* type os event semaphore	*/
  } GS_EVENT_SEM;

#define GET_THREAD_ID()		pthread_self()
#define ST_MUTEX_SEM 		pthread_mutex_t
#define ST_EVENT_SEM 		GS_EVENT_SEM *
#define ST_THREAD_ID 		pthread_t
#define ST_THREAD_HANDLE 	pthread_t
#define ST_THREAD_ARG 		void *
#define ST_THREAD_CALL_CONV
#define ST_THREAD_RET 		void *
#define ST_THREAD_RET_VAL	NULL
#endif /* defined(S_MT_SUPPORT) */

#endif /* defined UNIX, linux,... */


/************************************************************************/
/************************************************************************/
/* If no system has defined the required types, do so now. This allows	*/
/* the source to include variables of these types w/o errors.		*/

#if !defined (ST_EVENT_SEM)

#define GET_THREAD_ID()		(ST_THREAD_ID) 0xFFFFFFFF
#define ST_MUTEX_SEM 		ST_VOID *
#define ST_EVENT_SEM 		ST_VOID *
#define ST_THREAD_ID 		ST_VOID *
#define ST_THREAD_HANDLE 	ST_VOID *
#define ST_THREAD_ARG 		ST_VOID *
#define ST_THREAD_CALL_CONV
#define ST_THREAD_RET 		ST_VOID
#define ST_THREAD_RET_VAL

#endif  /* !defined (ST_EVENT_SEM) */

/************************************************************************/
/************************************************************************/
/* Global functions							*/

/* SISCO Single MUTEX functions */
ST_RET gs_init (ST_VOID);
ST_VOID gs_log_sem_state (ST_VOID);
ST_VOID gs_get_semx (SD_CONST ST_CHAR *srcFile, ST_INT srcLineNum);
ST_VOID gs_free_semx (SD_CONST ST_CHAR *srcFile, ST_INT srcLineNum);

#if defined(_WIN32)
#if defined (UNICODE)
ST_RET gs_translate_name (ST_CHAR *name,   WCHAR *wideName, ST_UINT bufSize);
#else
ST_RET gs_translate_name (ST_CHAR *name, ST_CHAR *newName,  ST_UINT bufSize);
#endif
#endif /* defined(_WIN32) */

/* User's MUTEX Semaphore functions */
ST_RET gs_mutex_create_ex (ST_MUTEX_SEM *ms, ST_CHAR *srcFile, ST_INT srcLine);
#define gs_mutex_create(ms) gs_mutex_create_ex(ms,__FILE__,__LINE__)

ST_RET  gs_named_mutex_create_ex  (ST_MUTEX_SEM *ms, ST_CHAR *name, ST_CHAR *srcFile, ST_INT srcLine);
#define gs_named_mutex_create(ms,name) gs_named_mutex_create_ex(ms,name,__FILE__,__LINE__)

ST_RET gs_mutex_get_tm_ex (ST_MUTEX_SEM *ms, ST_LONG timeout, ST_CHAR *srcFile, ST_INT srcLine);
#define gs_mutex_get_tm(ms,timeout) gs_mutex_get_tm_ex(ms,timeout,__FILE__,__LINE__)
#define gs_mutex_get(ms) gs_mutex_get_tm_ex(ms, -1L,__FILE__,__LINE__)

ST_RET gs_mutex_free_ex (ST_MUTEX_SEM *ms, ST_CHAR *srcFile, ST_INT srcLine);
#define gs_mutex_free(ms) gs_mutex_free_ex(ms,__FILE__,__LINE__)

ST_RET  gs_mutex_destroy (ST_MUTEX_SEM *ms);

/* Event Semaphores */
ST_EVENT_SEM gs_get_named_event_sem_ex (ST_CHAR *name, ST_BOOLEAN manualReset, ST_CHAR *srcFile, ST_INT srcLine);
#define gs_get_named_event_sem(name,manualReset) gs_get_named_event_sem_ex(name,manualReset,__FILE__,__LINE__)

ST_EVENT_SEM gs_get_event_sem_ex (ST_BOOLEAN manualReset, ST_CHAR *srcFile, ST_INT srcLine);
#define gs_get_event_sem(manualReset) gs_get_event_sem_ex(manualReset,__FILE__,__LINE__)

ST_RET gs_wait_event_sem_ex (ST_EVENT_SEM es, ST_LONG timeout, ST_CHAR *srcFile, ST_LONG srcLine);
#define gs_wait_event_sem(es,timeout) gs_wait_event_sem_ex(es,timeout,__FILE__,__LINE__)


ST_RET gs_wait_mult_event_sem_ex (ST_INT numEvents, ST_EVENT_SEM *esTable, 
		ST_BOOLEAN *activity, ST_LONG timeout, ST_CHAR *srcFile, ST_LONG srcLine);
#define gs_wait_mult_event_sem(numEvents,esTable,activity,timeout)\
           gs_wait_mult_event_sem_ex(numEvents,esTable,activity,timeout,__FILE__,__LINE__)

ST_VOID gs_signal_event_sem (ST_EVENT_SEM es);
ST_VOID gs_pulse_event_sem (ST_EVENT_SEM es);
ST_VOID gs_reset_event_sem (ST_EVENT_SEM es);
ST_VOID gs_free_event_sem (ST_EVENT_SEM es);

#if defined(_WIN32)
/* Semaphores */
ST_SEMAPHORE gs_get_named_semaphore_ex (ST_CHAR *name, ST_LONG initCount, ST_LONG maxCount, ST_CHAR *srcFile, ST_INT srcLine);
#define gs_get_named_semaphore(name,initCount,maxCount)	gs_get_named_semaphore_ex (name, initCount, maxCount, __FILE__, __LINE__)

ST_SEMAPHORE gs_get_semaphore_ex (ST_LONG initCount, ST_LONG maxCount, ST_CHAR *srcFile, ST_INT srcLine);
#define gs_get_semaphore(initCount,maxCount)	gs_get_semaphore_ex (initCount, maxCount, __FILE__, __LINE__)

ST_RET gs_wait_semaphore_ex (ST_SEMAPHORE sem, ST_LONG timeout, ST_CHAR *srcFile, ST_LONG srcLine);
#define gs_wait_semaphore(sem,timeout)	gs_wait_semaphore_ex (sem, timeout, __FILE__, __LINE__)

ST_RET gs_release_semaphore_ex (ST_SEMAPHORE sem, ST_LONG releaseCount, ST_LONG *prevCount, ST_CHAR *srcFile, ST_LONG srcLine);
#define gs_release_semaphore(sem,releaseCount,prevCount)	gs_release_semaphore_ex(sem, releaseCount, prevCount, __FILE__, __LINE__)

ST_VOID gs_free_semaphore_ex (ST_SEMAPHORE sem, ST_CHAR *srcFile, ST_LONG srcLine);
#define gs_free_semaphore(sem)	gs_free_semaphore_ex(sem,__FILE__,__LINE__)

#endif	/* defined(_WIN32)	*/

/* Thread */
ST_RET gs_start_thread (ST_THREAD_RET (ST_THREAD_CALL_CONV *threadFunc) (ST_THREAD_ARG),
			ST_THREAD_ARG threadArg, 
			ST_THREAD_HANDLE *threadHandleOut,
			ST_THREAD_ID *threadIdOut);
ST_RET gs_wait_thread (ST_THREAD_HANDLE threadHandle, 
		       ST_THREAD_ID threadId, ST_LONG timeout);
ST_RET gs_close_thread (ST_THREAD_HANDLE threadHandle);

#if defined (_WIN32)
ST_VOID gs_set_thread_name (ST_THREAD_ID threadId, ST_CHAR *pThreadName);
#endif

/* Misc. functions */
#if defined (_WIN32)
ST_INT	gs_is_win_ver (ST_UINT ver);
#endif

/************************************************************************/
/* These are the macro's used to call the glbsem get/free functions.	*/
/* Note that the behavior depends on the defines used at compile time.	*/

#if defined(S_MT_SUPPORT)	/* If code is to have MT support	*/

#if defined(DEBUG_SISCO) 	/* MT support, debug */

#define S_LOCK_COMMON_RESOURCES()    gs_get_semx(thisFileName, __LINE__)
#define S_UNLOCK_COMMON_RESOURCES()  gs_free_semx(thisFileName, __LINE__) 

/* NOTE: because S_LOCK_UTIL_RESOURCES & S_UNLOCK_UTIL_RESOURCES are called from SLOG,	*/
/*      the gs_mutex_get and gs_mutex_free (code handling unnamed mutexes)	*/
/*      should not be logging to avoid infinite loop (i.e. lock funct calling	*/
/*      log funct which calls lock funct again).				*/
#define S_LOCK_UTIL_RESOURCES()		gs_mutex_get (&gs_util_mutex)
#define S_UNLOCK_UTIL_RESOURCES()	gs_mutex_free (&gs_util_mutex)

#else	/* MT support, no debug */

#define S_LOCK_COMMON_RESOURCES()	gs_mutex_get (&gs_glb_mutex)
#define S_UNLOCK_COMMON_RESOURCES()	gs_mutex_free (&gs_glb_mutex)

#define S_LOCK_UTIL_RESOURCES()		gs_mutex_get (&gs_util_mutex)
#define S_UNLOCK_UTIL_RESOURCES()	gs_mutex_free (&gs_util_mutex)

#endif

#else				/* No MT support is compiled in		*/
#define S_LOCK_COMMON_RESOURCES()
#define S_UNLOCK_COMMON_RESOURCES()
#define S_LOCK_UTIL_RESOURCES()
#define S_UNLOCK_UTIL_RESOURCES()
#endif

/* FOR BACKWARD COMPATIBILITY ONLY. Old code may be calling gs_get_sem,	*/
/* gs_free_sem directly. These calls should be replaced with S_LOCK_..	*/
#define gs_get_sem()	S_LOCK_UTIL_RESOURCES()
#define gs_free_sem()	S_UNLOCK_UTIL_RESOURCES()

/************************************************************************/

#define MAX_SEM_NEST_TRACK  20 
extern ST_MUTEX_SEM gs_glb_mutex;	/* the global mutex semaphore	*/
extern ST_MUTEX_SEM gs_util_mutex;/* UTILITY MUTEX semaphore: for "low-level"*/
				/* util functs (slog,mem_chk,stime,etc.)*/
extern ST_BOOLEAN gs_already_inited;
extern ST_UINT  gs_track;	/* This variable when set to value >0	*/
				/* during an application startup allows	*/
				/* tracking of MUTEX or EVENT sem.	*/
extern ST_INT   gs_currSemOwnerIndex;
extern ST_CHAR  gs_currSemOwnerFile[MAX_SEM_NEST_TRACK][SLOG_MAX_FNAME+1];
extern ST_INT   gs_currSemOwnerLine[MAX_SEM_NEST_TRACK];

extern ST_DOUBLE gs_hwMutexTime;
extern ST_DOUBLE gs_hwEventTime;

	/*---*---*---*---*---*---*---*---*---*---*---*---*---*--*/
	/*							*/
	/*		Timer functions				*/
	/*							*/
	/*---*---*---*---*---*---*---*---*---*---*---*---*---*--*/

	/* Note: On AIX the real-time support is not implemented.	*/
	/*	 Functions such as timer_create, sleep, or nanosleep are*/
	/*	 not available (in AIX version 4.3.2 or 4.3.3).		*/
	/*	 This implementation of timers is using the GLBSEM	*/
	/*	 gs_wait_event_sem function implemented with phtreads	*/
	/*	 to support the timer functionality.			*/

/* The timer support will be initialized automatically when the function*/
/* gs_timer_start is called for the first time.				*/

/* If the timer is of type SD_TIMER_ONESHOT then the timer resources	*/
/* will be automatically deleted after the first time the timer	is	*/
/* serviced. For the SD_TIMER_PERIODIC timer the user needs to call	*/
/* the gs_timer_end function to stop the timer and release resources.	*/

/* Before application exits the function gs_timer_cleanup should be	*/
/* called to cleanup all resources associates with timers.		*/

/* NOTE: Since Windows is limiting number of timers to 16 per process 	*/
/*       we have to use the UNIX timer implementation on Windows.	*/
/* 	 If the _WIN32_timers is defined then the Windows implementation*/
/*       of timers will be in effect for Windows.			*/
#if defined _WIN32
/* #define _WIN32_timers */
#endif

#define SD_TIMER_ONESHOT	1
#define SD_TIMER_PERIODIC	2

#define	GS_TIMER_WORKER_MIN	4	/* def min number of timer worker threads	*/
#define	GS_TIMER_WORKER_MAX	10	/* def max number of timer worker threads	*/

struct gs_timer_tag;		/* forward definition */

typedef struct gs_timer_tag
  {
  DBL_LNK	link;		/* link list 				*/
#if defined (_WIN32_timers)
  UINT		timerID;	/* timer id returned from timeSetEvent	*/
  ST_DOUBLE     delay_start;	/* time when delete timer delay started	*/
#endif
  ST_BOOLEAN	terminate;	/* set if timer should be terminated	*/
  ST_BOOLEAN	pending;	/* shows if timer processing is already pending */
  ST_ULONG      interval;	/* milliseconds, time between servicing	*/
  ST_DOUBLE     expiration;	/* milliseconds, next expiration time	*/
  ST_UINT       type;		/* timer type: oneshot or periodic	*/
  ST_VOID       (*callback_fun)(ST_VOID *);
  ST_VOID      *arg;    	/* argument for the callback_fun.	*/
  ST_EVENT_SEM	event;		/* user can set this if it wants to be notified */
  				/* when the timer object is destroyed	*/
  } GS_TIMER;

/* The timer resolution could be set to the lowest value required by an	*/
/* application before calling the gs_timer_start function.		*/
/* On Windows system if the timer resolution is changed while the 	*/
/* application is running, only timers started afterwards will be	*/
/* affected. On UNIX system all timers will be affected.		*/

ST_RET   gs_timer_init (ST_UINT min_workers, ST_UINT max_workers);
ST_RET   gs_timer_set_resolution (ST_ULONG timer_resolution);
ST_ULONG gs_timer_get_resolution (ST_VOID);

GS_TIMER *gs_timer_start (ST_UINT  type,
                          ST_ULONG interval,
                          ST_VOID (*callback_fun)(ST_VOID *),
                          ST_VOID  *arg);
ST_RET  gs_timer_end (GS_TIMER *timer_id);
ST_RET  gs_timer_cleanup (ST_LONG timeout);


/************************************************************************/
/* Logging macro's used internally by glbsem.c				*/

#define GLBSEM_LOG_ERR0(a) \
	SLOG_0 (gs_debug_sel & GS_LOG_ERR,_glbem_err_logstr,a)
#define GLBSEM_LOG_ERR1(a,b) \
	SLOG_1 (gs_debug_sel & GS_LOG_ERR,_glbem_err_logstr,a,b)
#define GLBSEM_LOG_ERR2(a,b,c) \
	SLOG_2 (gs_debug_sel & GS_LOG_ERR,_glbem_err_logstr,a,b,c)
#define GLBSEM_LOG_ERR3(a,b,c,d) \
	SLOG_3 (gs_debug_sel & GS_LOG_ERR,_glbem_err_logstr,a,b,c,d)

#define GLBSEM_LOG_CERR0(a) \
	SLOGC_0 (gs_debug_sel & GS_LOG_ERR,a)
#define GLBSEM_LOG_CERR1(a,b) \
	SLOGC_1 (gs_debug_sel & GS_LOG_ERR,a,b)
#define GLBSEM_LOG_CERR2(a,b,c) \
	SLOGC_2 (gs_debug_sel & GS_LOG_ERR,a,b,c)
#define GLBSEM_LOG_CERR3(a,b,c,d) \
	SLOGC_3 (gs_debug_sel & GS_LOG_ERR,a,b,c,d)

#define GLBSEM_LOG_NERR0(a) \
	SLOG_0 (gs_debug_sel & GS_LOG_NERR,_glbem_nerr_logstr,a)
#define GLBSEM_LOG_NERR1(a,b) \
	SLOG_1 (gs_debug_sel & GS_LOG_NERR,_glbem_nerr_logstr,a,b)
#define GLBSEM_LOG_NERR2(a,b,c) \
	SLOG_2 (gs_debug_sel & GS_LOG_NERR,_glbem_nerr_logstr,a,b,c)
#define GLBSEM_LOG_NERR3(a,b,c,d) \
	SLOG_3 (gs_debug_sel & GS_LOG_NERR,_glbem_nerr_logstr,a,b,c,d)

#define GLBSEM_LOG_FLOW0(a) \
	SLOG_0 (gs_debug_sel & GS_LOG_FLOW,_glbem_flow_logstr,a)
#define GLBSEM_LOG_FLOW1(a,b) \
	SLOG_1 (gs_debug_sel & GS_LOG_FLOW,_glbem_flow_logstr,a,b)
#define GLBSEM_LOG_FLOW2(a,b,c) \
	SLOG_2 (gs_debug_sel & GS_LOG_FLOW,_glbem_flow_logstr,a,b,c)
#define GLBSEM_LOG_FLOW3(a,b,c,d) \
	SLOG_3 (gs_debug_sel & GS_LOG_FLOW,_glbem_flow_logstr,a,b,c,d)

#define GLBSEM_LOG_CFLOW0(a) \
	SLOGC_0 (gs_debug_sel & GS_LOG_FLOW,a)
#define GLBSEM_LOG_CFLOW1(a,b) \
	SLOGC_1 (gs_debug_sel & GS_LOG_FLOW,a,b)
#define GLBSEM_LOG_CFLOW2(a,b,c) \
	SLOGC_2 (gs_debug_sel & GS_LOG_FLOW,a,b,c)
#define GLBSEM_LOG_CFLOW3(a,b,c,d) \
	SLOGC_3 (gs_debug_sel & GS_LOG_FLOW,a,b,c,d)

/************************************************************************/
#define gs_install()		/* OBSOLETE: for backward compat. only	*/
#define S_GS_INSTALL()		/* OBSOLETE: for backward compat. only.	*/
#define S_GS_INIT()		/* OBSOLETE: for backward compat. only.	*/


#ifdef __cplusplus
}
#endif

#endif /* #ifndef GBLSEM_INCLUDED */

