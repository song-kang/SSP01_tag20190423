/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*		1993-2012, All Rights Reserved 				*/
/*									*/
/* MODULE NAME : slog.c							*/
/* PRODUCT(S)  : SLOG							*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 03/12/12  EJV           slogSetHdr: added slogHdrMaxSize parameter	*/ 
/*			    and improve length checks.			*/
/*			   Linux,sun: log ThreadId in slogSetHdr.	*/
/* 02/07/12  JRB	   Display Thread ID ONLY for _WIN32.		*/
/* 12/15/11  DSF           Display Thread ID in hex and decimal		*/
/* 11/23/11  KCR           Added casting for proper 64bit compile       */
/* 12/28/10  MDE           Added ThreadID in header (TID)		*/
/* 08/02/10  JRB	   slogSetHdr: use strncat to append lineBreak.	*/
/*			   slog_end: init rc.				*/
/* 02/24/10  JRB	   slogSetTimeText: chk localtime return & len.	*/
/*			   slogSetHdr: chk lineBreak len.		*/
/* 06/01/09  MDE     63    Added SlogClient features			*/
/* 06/01/09  MDE     64    Fixed crash when file logging not enabled	*/
/* 09/22/08  LWP     62	   changed to remove errors from QNX		*/
/* 03/27/08  EJV     61    Use S_MAX_PATH instead of MAX_PATH.		*/
/*                         slog_get_index_file_name: added destLen param*/
/*                         and changed to return result.		*/
/* 02/06/08  EJV     60    Moved MAX_PATH define to sysincs.h		*/
/*			   _slogSetTimeTextElapsed _WIN32 only.		*/
/* 12/14/07  DSF     59    Added sNonStandardLogMode to support .NET	*/
/*			   Logger class					*/
/* 11/16/07  MDE     58    Fixed LINUX compile problems			*/
/* 10/23/07  MDE     57    Added ElapsedTime option for Windows		*/
/* 01/15/07  EJV     56    Chg S_LOCK_RESOURCES to S_LOCK_UTIL_RESOURCES*/
/* 01/08/07  EJV     55    slogIpcStop: added lc param.			*/
/* 09/13/06  DSF     54    No calls to ExceptionReport			*/
/* 08/18/06  JRB     53    Chk _slog_remote_fun along with other ptrs.	*/
/* 08/02/06  EJV     52    Corr: chg logMissed to ST_INT to avoid	*/
/*                           repeated logging when IPC_LOG_EN is set.	*/
/* 06/20/06  RLH     51    Added parameter checks and other robustness  */
/*                         features to _slogXML                         */
/* 03/14/06  CRM     50    Added _slogXML to log XML strings		*/
/* 02/13/06  DSF     49    Migrate to VS.NET 2005			*/
/* 01/23/06  EJV     48    doSlog: clarified buf overrun log for WIN32.	*/
/* 11/01/05  EJV     47    slog_end: added S_LOCK_RESOURCES.		*/
/* 10/31/05  MDE     46    Tweaked slog_end to stop IPC, disable	*/
/* 10/31/05  MDE     45    Default logging now disabled, log missed	*/
/* 10/21/05  MDE     44    Fixed slog_end path size			*/
/* 08/10/05  MDE     43    Added slog_start, slog_end			*/
/* 08/02/05  MDE     42    Fixed compile warning			*/
/* 05/23/05  EJV     40    doSlog corr: replaced LOG_IPC_EN with 	*/
/*			     LOG_IPC_LISTEN_EN || LOG_IPC_CALL_EN	*/
/*			   Moved sock_debug_sel to gensock2.c		*/
/*			   Moved   gs_debug_sel to glbsem.c		*/
/* 04/21/05  EJV     39    doSlog: use vsnprintf on Linux;		*/
/*			    Reworked the code assembling log in msg_buf.*/
/* 02/23/05  JRB     38    slogSetTimeText: fix for VXWORKS.		*/
/* 01/26/05  JRB     37    Move clnp_debug_sel to lean_var.c		*/
/* 01/24/05  MDE     36    Fixed tweak					*/
/* 01/20/05  MDE     35    Minor tweak for fileName & logtype checks	*/
/* 11/22/04  JRB     34    Add & use slog_max_msg_size_set funct.	*/
/*			   Ignore sl_max_msg_size if chged after first log.*/
/*			   Del static slog_buf, use new lc->msg_buf.	*/
/*			   slogDelBuf: add (LOG_CTRL *) arg.		*/
/* 08/04/04  EJV     33    Del slogTime, slogMs global variables.	*/
/* 07/09/04  EJV     32    All systems: one time/date format in slog hdr*/
/* 06/24/04  DSF     31    For Windows, log milliseconds		*/
/* 05/18/04  MDE     30    Removed LOG_IPC_SUPPORT #ifdef's		*/
/* 05/13/04  EJV     29    Added slogSetHdr, slogHdr.			*/
/* 03/11/04  GLB     28    Remove "thisFileName"                    	*/
/* 02/10/04  KCR     27    Added slogDelBuf()                           */
/* 01/12/04  EJV     26    Moved sock_debug_sel from gensock2.c		*/
/* 10/24/03  JRB     25    Move gs_debug_sel from glbsem.c to here.	*/
/*			   Move clnp_debug_sel to here.			*/
/* 10/13/03  EJV     24    Del MSOS2 (old), _WINDOWS.			*/
/* 05/07/03  DSF     23    Added support for sErrLogCtrl		*/
/* 03/28/02  EJV     22    vnsprintf: use it on Tru64 UNIX v5.0 and up	*/
/* 02/11/02  DSF     21    Call ExpRaiseDebugException () to log the 	*/
/*			   call stack					*/
/* 10/18/01  JRB     20    Fix sprintf calls.				*/
/* 09/28/01  EJV     19    Added vnsprintf for systems that support it.	*/
/* 05/18/00  JRB     18    More Lint cleanup.				*/
/* 04/19/00  JRB     17    Lint cleanup.				*/
/* 09/24/99  JRB     16    added:  #include "sysincs.h"                 */
/* 09/13/99  MDE     15    Added SD_CONST modifiers			*/
/* 07/15/99  RKR     14    16-bit version needs stdarg.h		*/
/* 04/14/99  MDE     13    Removed unnecessary include files		*/
/* 11/11/98  DSF     12    Minor changes to _slog_dyn_log_fun		*/
/* 10/08/98  MDE     11    Migrated to updated SLOG interface		*/
/* 08/25/98  IKE     10    Prevent buffer overrun by vsprintf for some	*/
/*			   platforms					*/
/* 08/13/98  MDE     09    Now log buffer overruns			*/
/* 06/15/98  MDE     08    Changes to allow compile under C++		*/
/* 06/02/98  DSF     07    LOG_TIME_EN on by default            	*/
/* 01/09/98  EJV     06    SUN Solaris 2.5 uses now ANSI vsprintf	*/
/* 11/05/97  DSF     05    Added SYSTIME_EN				*/
/* 10/06/97  DSF     04    Added thisFileName				*/
/* 09/12/97  DSF     03    Expose slogSetTimeText ()			*/
/* 08/19/97  DSF     02    Initialize sLogCtrlDefault to default 	*/
/*			   settings					*/
/* 05/27/97  DSF     01    Added IPC logging capability			*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#if defined(__OS2__)
#pragma data_seg(alldata)
#define INCL_BASE
#define INCL_DOS
#define INCL_DOSMISC
#define INCL_ERRORS
#define INCL_DOSPROCESS
#define INCL_DOSQUEUES
#define INCL_DOSSEMAPHORES
#define INCL_DOSMEMMGR
#define INCL_DOSFILEMGR
#define INCL_DOSDATETIME
#define INCL_DOSDEVICES
#include <os2.h>
#include <stddef.h>
#endif

#include "glbtypes.h"
#include "sysincs.h"
#include "glbsem.h"
#include "slog.h"
#include "stime.h"
#include "str_util.h"
#include "sx_defs.h"

#if defined (_WIN32)
#include <windows.h>
#endif

#define XML_NO_TAG	      0
#define XML_DOCUMENT	      1
#define XML_COMMENT	      2
#define XML_START	      3
#define XML_END		      4
#define XML_EMPTY	      5

ST_CHAR *_slogXMLLogTypeStr = "SLOGXML";

/************************************************************************/
/* Other prototypes.							*/
/************************************************************************/

static ST_VOID doSlog (LOG_CTRL *lc, 
		       SD_CONST ST_INT logType, 
		       SD_CONST ST_CHAR *SD_CONST logTypeStr, 
		       SD_CONST ST_CHAR *SD_CONST sourceFile,
		       SD_CONST ST_INT lineNum, 
		       SD_CONST ST_CHAR *SD_CONST format, va_list ap);
ST_VOID slogSetTimeText (LOG_CTRL *lc);
#if defined (_WIN32)
static ST_VOID _slogSetTimeTextElapsed (LOG_CTRL *lc);
#endif

/************************************************************************/
/* Global variables used by the SLOG library.				*/ 
/************************************************************************/


LOG_CTRL *sErrLogCtrl = NULL;

LOG_CTRL sLogCtrlDefault = {LOG_TIME_EN,
			    {1000000, "mms.log", 
    			     FIL_CTRL_WIPE_EN | 
                   	     FIL_CTRL_WRAP_EN | 
                   	     FIL_CTRL_MSG_HDR_EN,
			     0, 0, NULL}};

LOG_CTRL *sLogCtrl = &sLogCtrlDefault;
ST_CHAR slogTimeText[TIME_BUF_LEN];
ST_INT sl_max_msg_size = MAX_LOG_SIZE;
ST_UINT32 slogRemoteFlags;
ST_BOOLEAN sNonStandardLogMode = SD_FALSE;

ST_VOID (*slog_service_fun) (ST_VOID);

ST_CHAR *_slogAlwaysLogTypeStr = "SLOGALWAYS";

/* dynamic logging function                                              */
ST_VOID (*slog_dyn_log_fun) (LOG_CTRL *lc, 
				SD_CONST ST_INT logType, 
                             	SD_CONST ST_CHAR *SD_CONST sourceFile, 
				SD_CONST ST_INT lineNum,
                             	SD_CONST ST_INT bufLen, 
				SD_CONST ST_CHAR *buf);
ST_VOID (*_slog_dyn_log_fun) (LOG_CTRL *lc, 
				SD_CONST ST_CHAR *timeStr, 
			     	SD_CONST ST_INT logType, 
				SD_CONST ST_CHAR *SD_CONST logTypeStr,
                             	SD_CONST ST_CHAR *SD_CONST sourceFile, 
				SD_CONST ST_INT lineNum,
                             	SD_CONST ST_INT bufLen, 
				SD_CONST ST_CHAR *buf);

/* remote logging function                                              */
/* Assign to this fun pointer a function which handles the logging to	*/
/* remote log file.  For the slog_remote_flags parameter reference the	*/
/* description for slog_remote_fun function.				*/

ST_VOID (*slog_remote_fun) (ST_UINT32 slog_remote_flags,
			  LOG_CTRL *lc, 
			SD_CONST ST_INT logType, 
                        SD_CONST ST_CHAR *SD_CONST sourceFile, 
			SD_CONST ST_INT lineNum,
                        SD_CONST ST_INT bufLen, 
			SD_CONST ST_CHAR *buf);

ST_VOID (*_slog_remote_fun) (ST_UINT32 slog_remote_flags,
			LOG_CTRL *lc, 
			SD_CONST ST_INT logType, 
                        SD_CONST ST_CHAR *SD_CONST logTypeStr, 
                        SD_CONST ST_CHAR *SD_CONST sourceFile, 
			SD_CONST ST_INT lineNum,
                        SD_CONST ST_INT bufLen, 
			SD_CONST ST_CHAR *buf);

/************************************************************************/
/*                               _slog                                  */
/* Main general message logging function, typically called via macro    */
/* Just print the message to be logged, pass to memory & file logging	*/
/* functions if enabled							*/
/************************************************************************/

ST_VOID _slogc (LOG_CTRL *lc, SD_CONST ST_CHAR *format, ...)
  {
va_list	ap;

  va_start (ap, format);
  doSlog (lc, SLOG_CONT, NULL, NULL, 0, format, ap);
  va_end(ap);
  }


ST_VOID _slog (LOG_CTRL *lc, 
	       SD_CONST ST_CHAR *SD_CONST logTypeStr, 
	       SD_CONST ST_CHAR *SD_CONST sourceFile, 
	       SD_CONST ST_INT	lineNum, 
	       SD_CONST ST_CHAR *format, ...)
  {
va_list	ap;

  va_start (ap, format);
  doSlog (lc, SLOG_NORMAL, logTypeStr, sourceFile, lineNum, format, ap);
  va_end(ap);
  }

/************************************************************************/
/*                               slog                                   */
/************************************************************************/

ST_VOID slog (LOG_CTRL *lc, 
	      SD_CONST ST_INT logType, 
	      SD_CONST ST_CHAR *SD_CONST sourceFile, 
	      SD_CONST ST_INT lineNum, 
	      SD_CONST ST_CHAR *format, ...)
  {
va_list	ap;

  va_start (ap, format);
  doSlog (lc, logType, NULL, sourceFile, lineNum, format, ap);
  va_end(ap);
  }

/************************************************************************/
/*                               slogx                                  */
/* Extended message logging function.					*/
/* Same as slog, except one more argument to determine if this type of	*/
/* logging is enabled.  This function allows one macro to be used	*/
/* for any number of arguments.						*/
/* This function is designed so that the first 5 arguments are passed	*/
/* to this function via macro.  For example:				*/
/* #define SLOG_ACSE_IND s_debug_sel & ACSE_IND_PRINT, s_sLogCtrl,\	*/
/*			 ACSE_IND_PRINT_TYPE,THISFILE,__LINE__		*/
/* slogx (SLOG_ACSE_IND, "Indication PDU ptr=" S_FMT_PTR ", len=%d", ptr, len);	*/
/************************************************************************/

ST_VOID slogx (ST_UINT32 doit, LOG_CTRL *lc, 
	       SD_CONST  ST_INT logType, 
	       SD_CONST ST_CHAR *SD_CONST sourceFile, 
	       SD_CONST ST_INT lineNum, 
	       SD_CONST ST_CHAR *format, ...)
  {
va_list	ap;

/* Make sure "doit" flag is set.					*/
  if (!doit)
    return;

  va_start (ap, format);
  doSlog (lc, logType, NULL, sourceFile, lineNum, format, ap);
  va_end(ap);
  }


/************************************************************************/
/*                               doSlog                                 */
/* Main logging function, called from slog or slogx.			*/
/* Just print the message to be logged, pass to memory & file logging	*/
/* functions if enabled							*/
/************************************************************************/

#define SLOG_MISSED_LOG_MSG  "Warning: SLOG Log messages missed"

static ST_VOID doSlog (LOG_CTRL *lc, 
		       SD_CONST ST_INT logType, 
		       SD_CONST ST_CHAR *SD_CONST logTypeStr, 
		       SD_CONST ST_CHAR *SD_CONST sourceFile,
		       SD_CONST ST_INT lineNum, 
		       SD_CONST ST_CHAR *SD_CONST format, va_list ap)
  {
ST_INT 	count;
ST_CHAR tmpBuf[128];
static ST_INT     logMissed = 0;
static ST_CHAR   *missedSourceFile;
static ST_INT     missedLineNum;
	    
  S_GS_INIT ();
  S_LOCK_UTIL_RESOURCES ();

/* Make sure the LOG_CTRL pointer is not NULL				*/
  if (!lc)
    {
    S_UNLOCK_UTIL_RESOURCES ();
    return;
    }

/* Check to see if any logging is enabled AND dynamic user logging	*/
/* function pointer not set						*/

  if (!(lc->logCtrl & (LOG_FILE_EN | LOG_MEM_EN | LOG_IPC_EN)) && 
      !slog_dyn_log_fun && !_slog_dyn_log_fun &&
      !slog_remote_fun && !_slog_remote_fun)	   
    {
    if (logMissed == 0)
      {
      logMissed        = 1;
      missedSourceFile = sourceFile;
      missedLineNum    = lineNum; 
      }
    S_UNLOCK_UTIL_RESOURCES ();
    return;
    }

/* Allocate a slog_buffer the first time this function is called.  This */
/* allows us to make the size of the slog message buffer larger than    */
/* the default.                                                         */

  if (lc->msg_buf == NULL)
    {
    /* NOTE: this code only for backward compatibility with apps that	*/
    /* control buffer size by setting the global var "sl_max_msg_size".	*/
    /* New apps should call "slog_max_msg_size_set" directly.		*/
    /* Use value of sl_max_msg_size NOW. Don't care if user changes it later.*/
    if (slog_max_msg_size_set (lc, sl_max_msg_size) != SD_SUCCESS)
      {
      fprintf(stderr, "\nslog: error setting max msg size");
      S_UNLOCK_UTIL_RESOURCES ();
      return;
      }
    } 
  
/* It is OK to pass in a NULL format string when using the dynamic 	*/
/* logging functions - no vsprintf if so				*/

  if (format == NULL)
    {
    /* make buf a zero length string just in case			*/
    count = 0;
    lc->msg_buf[0] = 0;
    }
  else
    {
#if defined(_WIN32)
    count = _vsnprintf(lc->msg_buf,lc->max_msg_size,format,ap);
#elif  defined(__QNX__) && defined(__WATCOMC__)
    count = _vbprintf(lc->msg_buf,lc->max_msg_size,format,ap);
#elif defined(_AIX) || defined(sun) || defined(_hpux) || defined(__alpha) || defined(linux)
    count = vsnprintf(lc->msg_buf,lc->max_msg_size,format,ap);
#else  /* other systems: VXWORKS, ... 					*/
    count = vsprintf (lc->msg_buf, format, ap);
#endif
    lc->msg_buf[lc->max_msg_size-1] = 0; 	/* terminate the buffer, Win and UNIX 	*/
						/* functions don't behave the same 	*/
    /* NOTE: On _WIN32 count could be negative because of error or too small buffer. 	*/
    /*       On other systems count is negative because of error, too small buffer is	*/
    /*       indicated by return of count larger than buf_size supplied to the function.*/
#if defined(_WIN32)
    if (count < 0)
      {
      sprintf (tmpBuf,"*** LOG ERROR: LOG BUFFER OVERRUN (lc->max_msg_size=%d bytes) or _vsnprintf function error",
               lc->max_msg_size);
      strncpy_safe (lc->msg_buf, tmpBuf, lc->max_msg_size-1);
      count = (int) strlen (lc->msg_buf);		/* count = len of this log message	*/
      }
#else  /* !defined(_WIN32) */
    if (count < 0)
      {
      sprintf (tmpBuf,"*** LOG ERROR: _vbprintf(QNX), vsnprintf(UNIX,LINUX), or vsprintf(other sys) function failed");
      strncpy_safe (lc->msg_buf, tmpBuf, lc->max_msg_size-1);
      count = strlen (lc->msg_buf);		/* count = len of this log message	*/
      }
    else if (count >= lc->max_msg_size)
      {
      sprintf (tmpBuf,"*** LOG ERROR: LOG BUFFER OVERRUN: message len=%d bytes (lc->max_msg_size=%d bytes)",
               count, lc->max_msg_size);
      strncpy_safe (lc->msg_buf, tmpBuf, lc->max_msg_size-1);
      count = strlen (lc->msg_buf);		/* set count to len of this log message */
      }
#endif  /* !defined(_WIN32) */

    count++;					/* allow for null terminator		*/
    }

/* If time stamping is desired, get the time string from the user	*/
  if (lc->logCtrl & LOG_TIME_EN)
    slogSetTimeText (lc);  
  else
    slogTimeText[0] = 0;	/* Init to empty time/date string.	*/

/* Check for type for special logging ('continuation' or dynamic	*/
/* logging format commands) -	    					*/
/* 	All standard log types are >= 0					*/
/*	Continuation logging is SLOG_CONT (-1)	  			*/
/*	Dynamic logging format commands are other negative numbers,	*/
/*	reserved are - 							*/
/*		SLOG_DYN_PAUSE  = -10	 				*/
/*		SLOG_DYN_LF     = -11	 				*/
/*		SLOG_DYN_CLRSCR = -12	 				*/

  if (logType >=0 || logType == SLOG_CONT)
    {
    if (lc->logCtrl & LOG_FILE_EN)	/* File Logging enabled		*/
      {
      if (logMissed == 1)
        {
        logMissed = 2;          /* to prevent logging this msg again	*/
                                /* when slogIpc turns off all masks 	*/
        slogFile (lc, 0, "INTERNAL_SLOG", missedSourceFile, missedLineNum, (int) strlen (SLOG_MISSED_LOG_MSG), SLOG_MISSED_LOG_MSG);
        }
      slogFile (lc, logType, logTypeStr, sourceFile, lineNum, count, lc->msg_buf);
      }
    if (lc->logCtrl & LOG_MEM_EN)	/* File Logging enabled		*/
      slogMem (lc, logType, logTypeStr, sourceFile, lineNum, count, lc->msg_buf);
    }
  if ((lc->logCtrl & LOG_IPC_LISTEN_EN) || (lc->logCtrl & LOG_IPC_CALL_EN))
    /* IPC Logging enabled (listen, calling, or both modes) */
    slogIpc (lc, logType, logTypeStr, sourceFile, lineNum, count, lc->msg_buf);

/* If the user has set up a dynamic log display function, call it	*/
  if (slog_dyn_log_fun)
    (*slog_dyn_log_fun)(lc, logType, sourceFile, lineNum, count, lc->msg_buf);
  if (_slog_dyn_log_fun)
    (*_slog_dyn_log_fun)(lc, slogTimeText, logType, logTypeStr, sourceFile, 
                         lineNum, count, lc->msg_buf);

 /* If the user has set up a remote logging function, call it		*/
  if (slog_remote_fun)
    {
    (*slog_remote_fun)(slogRemoteFlags, lc, logType, sourceFile, lineNum, 
			count, lc->msg_buf);
    }
  if (_slog_remote_fun)
    {
    (*_slog_remote_fun)(slogRemoteFlags, lc, logType, logTypeStr, 
		       sourceFile, lineNum, count, lc->msg_buf);
    }

  S_UNLOCK_UTIL_RESOURCES ();
  }

/************************************************************************/
/*                         slogSetTimeText				*/
/* Set the time string per the lc control information			*/
/************************************************************************/
ST_VOID slogSetTimeText (LOG_CTRL *lc)
  {
ST_CHAR tmp_buf [100];	/* sprintf may generate longer than expected string*/
  slogTimeText[0] = 0;	/* Init to empty.	*/

  if (lc->logCtrl & LOG_TIME_EN)
    {
#if defined (_WIN32)
    SYSTEMTIME systime;

    if ((lc->logCtrl & LOG_ELAPSEDTIME_EN) == 0)
      {  
      GetLocalTime (&systime);
      sprintf (tmp_buf, "%04d-%02d-%02d %02d:%02d:%02d.%03d",
             systime.wYear, systime.wMonth, systime.wDay,
             systime.wHour, systime.wMinute, systime.wSecond, systime.wMilliseconds);
      /* If generated string is too long, return empty string	*/
      if (strlen (tmp_buf) < sizeof (slogTimeText))
        strcpy (slogTimeText, tmp_buf);
      }
    else
      _slogSetTimeTextElapsed (lc);

#elif defined (VXWORKS)
    /* Does not support gettimeofday.	*/
    time_t         curTime;
    struct tm     *locTime;

    curTime = time (NULL);
    locTime = localtime (&curTime);
    if (locTime==NULL)
      return;	/* error, return with empty string	*/

    sprintf (tmp_buf, "%04d-%02d-%02d  %02d:%02d:%02d",
             locTime->tm_year+1900, locTime->tm_mon+1, locTime->tm_mday,
             locTime->tm_hour, locTime->tm_min, locTime->tm_sec);
    /* If generated string is too long, return empty string	*/
    if (strlen (tmp_buf) < sizeof (slogTimeText))
      strcpy (slogTimeText, tmp_buf);
#else
    /* UNIX, Linux, QNX,... */
    struct timeval tp;
    time_t         curTime;
    struct tm     *locTime;

    gettimeofday (&tp, NULL);
    curTime = (time_t) tp.tv_sec;
    locTime = localtime (&curTime);
    if (locTime==NULL)
      return;	/* error, return empty string	*/

    /* tv_usec is "int" on some platforms, so we cast to long to avoid warning*/
    sprintf (tmp_buf, "%04d-%02d-%02d  %02d:%02d:%02d.%03ld",
             locTime->tm_year+1900, locTime->tm_mon+1, locTime->tm_mday,
             locTime->tm_hour, locTime->tm_min, locTime->tm_sec, (long) tp.tv_usec/1000);
    /* If generated string is too long, return empty string	*/
    if (strlen (tmp_buf) < sizeof (slogTimeText))
      strcpy (slogTimeText, tmp_buf);
#endif  /* UNIX, Linux, QNX,... */	
    }
  }  


/************************************************************************/

#if defined (_WIN32)
static ST_VOID _slogSetTimeTextElapsed (LOG_CTRL *lc)
{
static ST_BOOLEAN firstTime = SD_TRUE;
static LARGE_INTEGER ticksPerSec;
static LARGE_INTEGER startTicks;
static double dTicksPerSec;
LARGE_INTEGER ticks;
double dDeltaTicks;
double dSec;

  if (firstTime == SD_TRUE)
    {
  /* See if supported, and if so how many ticks per second */
    if (QueryPerformanceFrequency(&ticksPerSec))
      {
    /* Mark the start of time for us  */
      QueryPerformanceCounter (&startTicks);
  
    /* We want to work with double precision floating point */
      dTicksPerSec = (double) ticksPerSec.QuadPart;
      }
    else
      {
      strncpy_safe (slogTimeText, "Elapsed Time Not Available", sizeof (slogTimeText) - 1);
      return;
      }
    firstTime = SD_FALSE;
    }


/* Get the current tick count */
  QueryPerformanceCounter (&ticks);

/* Get delta from start of logging */
  dDeltaTicks = (double) (ticks.QuadPart - startTicks.QuadPart);

/* Calculate the elapsed time */
  dSec = dDeltaTicks/dTicksPerSec;

  sprintf (slogTimeText, "%.6f", dSec);
  }
#endif /* defined (_WIN32) */



/************************************************************************/
/*		               slogTrimFileName				*/
/************************************************************************/

ST_VOID slogTrimFileName (ST_CHAR *dest, SD_CONST ST_CHAR *fullName)
  {
ST_INT i,s;
ST_CHAR c;

  s = 0;
  i = 0;
  while ((c = fullName[i]))
    {
    if (c == '\\' || c == '/' || c == ':' || c == ']' )
      s = i+1;	/* remember the last path seperator character position	*/
    i++; 
    }
  strncpy (dest,&fullName[s], SLOG_MAX_FNAME);
  dest[SLOG_MAX_FNAME] = 0;
  }

/************************************************************************/
/*                         slogSetHdr					*/
/* Set the header string per the lc control information.		*/
/* If slogHdrMaxSize < SLOG_MAX_HDR, the string written to "slogHdr"	*/
/* may be truncated.							*/
/* NOTE: If this function is changed, the value of SLOG_MAX_HDR may	*/
/*       also need to be changed.					*/
/************************************************************************/

ST_VOID slogSetHdr (LOG_CTRL *lc, SD_CONST ST_INT logType, 
                    SD_CONST ST_CHAR *SD_CONST logTypeStr, 
                    SD_CONST ST_CHAR *SD_CONST sourceFile, 
                    SD_CONST ST_INT lineNum, 
                    ST_CHAR *slogHdr, size_t slogHdrMaxSize,
                    ST_CHAR *lineBreak)
  {
ST_CHAR fname[SLOG_MAX_FNAME+1];
ST_CHAR tmp[SLOG_MAX_FNAME+14+SLOG_MAX_THREADID+1];	/* 14 for line number & ()*/
ST_CHAR tmpHdr [SLOG_MAX_HDR];	/* construct Hdr here, then copy to slogHdr*/
#if defined(_WIN32) || defined(sun) || defined(linux)
ST_THREAD_ID   threadId;
#endif

  tmpHdr[0] = 0;	/* initialize tmpHdr (empty)	*/
  strncat (tmpHdr, lineBreak, 2);
   
  if ((lc->fc.ctrl & FIL_CTRL_MSG_HDR_EN) && (logType != SLOG_CONT))
    {
    if (!(lc->logCtrl & LOG_NO_HEADER_CR))
      strncat (tmpHdr, lineBreak, 2);

    if (lc->logCtrl & LOG_TIME_EN)  /* user wants time stamp		*/
      {
      strcat (tmpHdr, slogTimeText);
      strcat (tmpHdr, " ");
      }

    if (!(lc->logCtrl & LOG_LOGTYPE_SUPPRESS))
      {
      if (logTypeStr != NULL && logTypeStr[0] != 0)
        {
        strncat (tmpHdr, logTypeStr, SLOG_MAX_LOGTYPESTR);
        strcat (tmpHdr, " ");
        }
      else
        {
        sprintf (tmp, "LogType:% 2d ", logType);
        strcat (tmpHdr, tmp);
        }
      }

    if ((sourceFile != NULL && sourceFile[0] != 0) && !(lc->logCtrl & LOG_FILENAME_SUPPRESS))
      {
      slogTrimFileName (fname, sourceFile);
#if defined(_WIN32) || defined(sun) || defined(linux)
      threadId = GET_THREAD_ID();
      sprintf (tmp, "(%s %d ThreadId=" S_FMT_THREAD_IDX " ("S_FMT_THREAD_ID")) ",
                    fname, lineNum, threadId, threadId);
#else
      /* Don't print thread ID because threads not supported on some systems.*/
      sprintf (tmp, "(%s %d) ", fname, lineNum);
#endif
      strcat (tmpHdr, tmp);
      }

    if (!(lc->logCtrl & LOG_NO_HEADER_CR))
      {
      strncat (tmpHdr, lineBreak, 2);
      strcat (tmpHdr,"  ");
      }
    }
  if (logType == SLOG_CONT)
    strcat (tmpHdr,"  ");

  /* Caller buffer must be at least 1 byte to allow copy.	*/
  if (slogHdrMaxSize >= 1)
    strncpy_safe (slogHdr, tmpHdr, slogHdrMaxSize-1); 
  }

/************************************************************************/
/*                       dumpCallingStack				*/
/************************************************************************/

#if defined(MSDOS) && !defined(_WINDOWS)
ST_VOID main (ST_VOID);
#endif

ST_VOID slogCallStack (LOG_CTRL *lc, SD_CONST ST_CHAR *txt)
  {
#if !defined(CODAN)
#if defined(MSDOS) && !defined(TC)
static ST_UINT32 ptr_to_abs (ST_VOID *ptr);
ST_UINT16 os,sg;
ST_UINT16 os2,sg2;
ST_UINT16 os3,sg3;
ST_UINT16 os4,sg4;
ST_UINT16 os5,sg5;
#endif

/* For DOS we can save the caller's return address		 	*/
#if defined(MSDOS) && !defined(TC)
  _asm {
    push si                     ; save SI
    
    mov si, bp                  ; first BP frame
    mov ax, ss:[si+2]		; get return address "offset" from stack
    mov os, ax			; store in "offset" variable
    mov ax, ss:[si+4]		; get return address "segment" from stack
    mov sg, ax			; store in "segment" variable

    mov si, ss:[si]             ; second BP frame
    mov ax, ss:[si+2]		; get return address "offset" from stack
    mov os2, ax			; store in "offset" variable
    mov ax, ss:[si+4]		; get return address "segment" from stack
    mov sg2, ax			; store in "segment" variable
    
    mov si, ss:[si]             ; third BP frame
    mov ax, ss:[si+2]		; get return address "offset" from stack
    mov os3, ax			; store in "offset" variable
    mov ax, ss:[si+4]		; get return address "segment" from stack
    mov sg3, ax			; store in "segment" variable

    mov si, ss:[si]             ; fourth BP frame
    mov ax, ss:[si+2]		; get return address "offset" from stack
    mov os4, ax			; store in "offset" variable
    mov ax, ss:[si+4]		; get return address "segment" from stack
    mov sg4, ax			; store in "segment" variable
    
    mov si, ss:[si]             ; fifth BP frame
    mov ax, ss:[si+2]		; get return address "offset" from stack
    mov os5, ax			; store in "offset" variable
    mov ax, ss:[si+4]		; get return address "segment" from stack
    mov sg5, ax			; store in "segment" variable
    
    pop si                      ; restore SI
    }
#endif

#endif

  if (txt != NULL)
    slog(lc, 0, NULL, 0, "%s", txt);
    
#if defined(MSDOS) && !defined(TC)
  slog(lc,SLOG_CONT,NULL,0,"Calling Address (1st frame): 0x%04X:%04X (0x%lX)",
  	    sg,os,((ST_UINT32)sg << 4) + (ST_UINT32)os);
  slog(lc,SLOG_CONT,NULL,0,"Calling Address (2nd frame): 0x%04X:%04X (0x%lX)",
  	    sg2,os2,((ST_UINT32)sg2 << 4) + (ST_UINT32)os2);
  slog(lc,SLOG_CONT,NULL,0,"Calling Address (3rd frame): 0x%04X:%04X (0x%lX)",
  	    sg3,os3,((ST_UINT32)sg3 << 4) + (ST_UINT32)os3);
  slog(lc,SLOG_CONT,NULL,0,"Calling Address (4th frame): 0x%04X:%04X (0x%lX)",
  	    sg4,os4,((ST_UINT32)sg4 << 4) + (ST_UINT32)os4);
  slog(lc,SLOG_CONT,NULL,0,"Calling Address (5th frame): 0x%04X:%04X (0x%lX)",
  	    sg5,os5,((ST_UINT32)sg5 << 4) + (ST_UINT32)os5);
  slog(lc,SLOG_CONT,NULL,0,"Main Address : %08lx (0x%lX)",
                                                main,ptr_to_abs ((ST_VOID *)main));

#endif

#if !defined(CODAN)
#endif
  }

/************************************************************************/
/*			 ptr_to_abs (ptr)				*/
/* Convert buffer address to long real address				*/
/************************************************************************/

#if defined(MSDOS) && !defined(TC)
static ST_UINT32 ptr_to_abs (ST_VOID *ptr)
  {
ST_UINT32 temp1;
ST_UINT32 temp2;

  temp1 =  (ST_UINT32) ptr;
  temp2 = temp1 >> 12;			/* Microsoft/Xenix use seg/off	*/
  temp1 &= 0xFFFF;
  temp2 &= 0xFFFF0;
  return (temp1 + temp2);               /* find real address         */
  }
#endif


/************************************************************************/
/*                               slog                                   */
/************************************************************************/

ST_VOID slogDelBuf (LOG_CTRL *lc)
  {
  S_LOCK_UTIL_RESOURCES ();
  if (lc->msg_buf)
    free (lc->msg_buf);
  lc->msg_buf = NULL;
  S_UNLOCK_UTIL_RESOURCES ();
  }

/************************************************************************/
/*			slog_max_msg_size_set				*/
/* Set the maximum message size and allocate a buffer of this size.	*/
/* This function sets "max_msg_size" and "msg_buf" in LOG_CTRL.		*/
/************************************************************************/
ST_RET slog_max_msg_size_set (LOG_CTRL *lc, ST_INT max_msg_size)
  {
ST_RET retcode = SD_SUCCESS;

  S_LOCK_UTIL_RESOURCES ();
  lc->max_msg_size = max_msg_size;	/* save size in LOG_CTRL	*/

  if (lc->msg_buf != NULL)
    free (lc->msg_buf);	/* buf already allocated by previous call, so free it first*/

  /* Alloc "lc->msg_buf". chk_malloc calls slog so use malloc here!	*/
  /* This is the only place "lc->msg_buf" is allocated.			*/
  lc->msg_buf = (ST_CHAR *) malloc (lc->max_msg_size + 1); 
  if (lc->msg_buf == NULL)
    {	/* this should almost never happen	*/
    lc->max_msg_size = 0;	/* no buffer so can't log anything	*/
    retcode = SD_FAILURE;
    }
  S_UNLOCK_UTIL_RESOURCES ();
  return (retcode);
  }

/************************************************************************/
/*			slog_start					*/
/************************************************************************/

ST_RET slog_start (LOG_CTRL *lc, ST_INT max_msg_size)
  {
SLOG_INIT_PARAM ip;
ST_RET rc;

  memset (&ip, 0, sizeof (SLOG_INIT_PARAM));
  ip.lc = lc;
  ip.max_msg_size = max_msg_size;
  rc = slog_start_ex (&ip);
  return (rc);
  }


ST_RET slog_start_ex (SLOG_INIT_PARAM *ip)
  {
LOG_CTRL *lc;
ST_RET rc;

  lc = ip->lc;
  lc->ipc.slog_ipc_connect_ind = ip->slog_ipc_connect_ind;
  lc->ipc.slog_ipc_rx_ind = ip->slog_ipc_rx_ind;
  lc->ipc.slog_ipc_disconnect_ind = ip->slog_ipc_disconnect_ind;
  lc->ipc.client = ip->client;

  rc = slog_max_msg_size_set (lc, ip->max_msg_size);
  rc = slogIpcInit (lc);
  return (rc);
  }

/************************************************************************/
/*			slog_end					*/
/************************************************************************/

ST_RET slog_end (LOG_CTRL *lc)
  {
ST_CHAR idxFileName[S_MAX_PATH];
FILE *fh;
ST_RET rc = SD_SUCCESS;

  slogIpcStop (lc);
  
  S_LOCK_UTIL_RESOURCES ();
  if (lc->logCtrl & LOG_FILE_EN)
    {
    rc = slog_get_index_file_name (lc, idxFileName, sizeof(idxFileName));

    if (rc == SD_SUCCESS)
      {    
      fh = fopen (idxFileName,"w+");
      if (fh)
        {
        fprintf (fh,"%ld", lc->fc.wipeFilePos);
        fclose (fh);
        rc = SD_SUCCESS;
        }
      else
        rc = SD_FAILURE;
      }
    } 

/* No more file or IPC logging after end ... */
  lc->logCtrl &= ~(LOG_FILE_EN | LOG_IPC_EN);
  S_UNLOCK_UTIL_RESOURCES ();

  //MEMO: ½â¾öÄÚ´æÐ¹Â© [2015-9-22 19:12 ÉÛ¿­Ìï]
  if (lc->msg_buf != NULL)
  {
	  free (lc->msg_buf);	/* buf already allocated by previous call, so free it first*/
	  lc->msg_buf = NULL;
  }
  return (rc);
  }

/************************************************************************/
/*                             _slogXML 				*/
/* Formats XML strings and logs them if file logging is enabled.	*/
/************************************************************************/
ST_VOID _slogXML (LOG_CTRL *lc, 
    	      SD_CONST ST_CHAR *SD_CONST logTypeStr, 
    	      SD_CONST ST_CHAR *SD_CONST sourceFile, 
    	      SD_CONST ST_INT lineNum, 
    	      ST_UINT  numBytes,
    	      SD_CONST ST_CHAR *textData)
  {
ST_UINT filCtrlSave;
ST_UINT tabLevel = 0;
ST_UINT index, i, j;
ST_CHAR xmlBuffer[SX_MAX_ELEM_LEN+1];
ST_INT  eleLength[SX_MAX_XML_NEST];
ST_INT  prevTag;
ST_INT  tag = XML_NO_TAG;

 

  S_GS_INIT ();
  S_LOCK_UTIL_RESOURCES ();

  /* Make sure the LOG_CTRL and textData pointers are not NULL          */
  if ((!lc) || (!textData))
    {
    S_UNLOCK_UTIL_RESOURCES ();
    return;
    }

  /* Check to see if any logging is enabled AND dynamic user logging	*/
  /* function pointer not set						*/
  if (!(lc->logCtrl & LOG_FILE_EN) && 
      !slog_dyn_log_fun && !_slog_dyn_log_fun &&
      !slog_remote_fun && !_slog_remote_fun)
    {
    S_UNLOCK_UTIL_RESOURCES ();
    return;
    }

  /* Remember the state of the wipe/wrap/header enable flags		*/
  /* so that we can restore them later					*/
  filCtrlSave = lc->fc.ctrl & (FIL_CTRL_WIPE_EN | 
    			       FIL_CTRL_WRAP_EN |
			       FIL_CTRL_MSG_HDR_EN);

  if (!numBytes)
    {
    lc->fc.ctrl |= (filCtrlSave & FIL_CTRL_WIPE_EN);
    }

  if (lc->logCtrl & LOG_FILE_EN)	/* File Logging enabled				  */
    {
    for (i=0; i<SX_MAX_XML_NEST; i++)
      eleLength[i] = 0;
    
    xmlBuffer[0] = '\0';
   
    index=0;

    /* process at most numBytes of data, or until end of textData string */
  
    for (i=0; (i < numBytes) && (textData[i] != 0); i++)
      {
      if (tabLevel >= SX_MAX_XML_NEST)  /* Log error if maximum nesting level is exceeded */
	{
	slogFile (lc, SLOG_NORMAL, logTypeStr, sourceFile, lineNum, 0, "XML nesting exceeds maximum level");
	break;
	}

      if (eleLength[tabLevel] >= SX_MAX_ELEM_LEN)   /* Log error if maximum element length is exceeded */
	{
	slogFile (lc, SLOG_NORMAL, logTypeStr, sourceFile, lineNum, 0, "XML element exceeds maximum length");
	break;
	}

      eleLength[tabLevel]++;

      /* Skip control characters */
      /* cast the chars as ST_UCHAR so data > 0x7F will not look negative */

      if (iscntrl((ST_UCHAR) textData[i]) || (textData[i] == ' ' && index == 0))
	{
	index = 0;
	continue;
	}

      /* Determine tag value */
      if (textData[i] == '/' && textData[i+1] == '>')
	tag = XML_EMPTY;

      if (textData[i] == '<')
	{
	prevTag = tag;

	if (textData[i] == '<' && textData[i+1] == '?')
	  tag = XML_DOCUMENT;
	else if (textData[i] == '<' && textData[i+1] == '!')
	  tag = XML_COMMENT;
	else if (textData[i] == '<' && textData[i+1] == '/') 
	  tag = XML_END;
	else if(textData[i] == '<')
	  tag = XML_START;

	/* Increment/decrement tabLevel */
	if (prevTag == XML_START && (tag == XML_START  || tag == XML_COMMENT))
	  tabLevel++;
	else if (tag == XML_END && (prevTag == XML_END || prevTag == XML_EMPTY))
	  tabLevel--;

	/* Format tab level */
	if (index == 0)
	  {
	  xmlBuffer[0] = '\0';
      
	  if (tabLevel)
  	    {
  	    for (j=0; j<tabLevel; j++, index+=2)
  	      strcat (xmlBuffer, "  ");
  	    }
	  }
	}
  
      xmlBuffer[index++] = textData[i];
      xmlBuffer[index] = '\0';

      /* Log XML buffer */
      if (textData[i] == '>' && (  
          textData[i+1] == '<' || 
          textData[i+1] == ' ' || 
          iscntrl ((ST_UCHAR) textData[i+1]) || 
          i == numBytes-1 || 
          tag != XML_START))
	{
	slogFile (lc, SLOG_CONT, logTypeStr, sourceFile, lineNum, (int) strlen(xmlBuffer), xmlBuffer);
	index = 0;
	
	/* If the user has set up a dynamic log display function, call it	*/
	if (slog_dyn_log_fun)
	  {
	  (*slog_dyn_log_fun)(lc, SLOG_CONT, NULL, 0, (int) strlen(xmlBuffer), xmlBuffer);
          }
        if (_slog_dyn_log_fun)
          {
          (*_slog_dyn_log_fun)(lc, slogTimeText, SLOG_CONT, NULL, NULL, 0, (int) strlen(xmlBuffer), xmlBuffer);
          }

        /* If the user has set up a remote logging function, call it		*/
	if (slog_remote_fun)
          {
          (*slog_remote_fun)(slogRemoteFlags, lc, SLOG_CONT, NULL, 0, (int) strlen(xmlBuffer), xmlBuffer);
          }
        if (_slog_remote_fun)
          {
          (*_slog_remote_fun)(slogRemoteFlags, lc, SLOG_CONT, NULL, NULL, 0, (int) strlen(xmlBuffer), xmlBuffer);
	  }
	}
      }
    }

  /* All done, restore the flag state bits */
  lc->fc.ctrl |= filCtrlSave;

  S_UNLOCK_UTIL_RESOURCES ();
  }


