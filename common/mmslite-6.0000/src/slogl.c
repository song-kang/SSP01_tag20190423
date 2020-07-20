/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1993 - 2012, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : slogl.c 						*/
/* PRODUCT(S)  : SLOG							*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 03/15/12  JRB	   Fix to work as replacement for slog lib.	*/
/*			   Fix Klocwork warning (use bigger buffer).	*/
/*			   Fix slogSetTimeText like slog.c.		*/
/* 01/26/05  JRB     24    Move clnp_debug_sel to lean_var.c		*/
/* 10/15/03   JRB     23   Move gs_debug_sel from glbsem.c to here.	*/
/*			   Move clnp_debug_sel to here.			*/
/* 08/18/03   JRB     22   Add "sl_max_msg_size" to match "slog.c".	*/
/* 05/07/01   JRB     21   Increase msgBuf size to avoid overrun.	*/
/* 04/19/00   JRB     20   Lint cleanup.				*/
/* 09/13/99   MDE     19   Added SD_CONST modifiers			*/
/* 12/04/98   MDE     18   Added SLOGL_TIME_STAMP option		*/
/* 12/04/98   MDE     17   Portability and cleanup tweaks 		*/
/* 11/16/98   MDE     16   Added _slogAlwaysLogTypeStr			*/
/* 10/08/98   MDE     15   Migrated to updated SLOG interface		*/
/* 07/02/98   JRB     14   Del system includes, use "sysincs.h".	*/
/* 11/07/97   MDE     13   Changed handling of slogFh			*/
/* 10/31/96   MDE     12   Added 'sLogCtrlDefault'			*/
/* 11/17/93   PLM     11   Changed file to eliminate specific OS 	*/
/*			   defines and to provide consistant interface.	*/
/* 11/03/93   DSF     10   Added ifdefs for _AIX and ultrix.		*/
/* 10/18/83   DSF     09   can't use 'main()' under Windows     	*/
/* 10/18/93   MDE     08   Replaced comments format			*/
/* 10/11/93   MDE     07   Added 'slogCallStack'			*/
/* 10-08-93   DSF     06   Define USE_PCHRT outside of this module	*/ 
/* 09-15-93   MDE     05   Now check 'LOG_CTRL' for NULL, also allow 	*/
/*			   logging to user dynamic function only	*/
/* 06-15-93   MDE     04   Changed time print resolution to 1ms		*/
/* 06-15-93   MDE     03   Added PCHRT support for DOS			*/
/* 06-04-93   MDE     02   Added HRTIMER support for OS/2		*/
/* 05-10-93   MDE     01   Added 'slog_dyn_log_fun'			*/
/* 03-17-93   MDE   1.00   Initial					*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "slog.h"
#include "stime.h"
#include "glbsem.h"	/* need GS_LOG_ERR define, etc.	*/
#include "str_util.h"
/************************************************************************/
/* Uncomment this to use log time stamps 				*/
#define SLOGL_TIME_STAMP

/************************************************************************/
/************************************************************************/
/* USER PORT FUNCTIONS 							*/
/* Modify this function to suit the logging environment - it may	*/
/* send the strings over a serial port, LAN connection, IPC, whatever	*/

ST_VOID u_sloglTxtLogStr (SD_CONST ST_CHAR *str);

/************************************************************************/
/* STATIC FUNCTION DECLARATIONS						*/

static ST_VOID logMsg (SD_CONST ST_INT logType, 
	      SD_CONST ST_CHAR *SD_CONST logTypeStr, 
	      SD_CONST ST_CHAR *SD_CONST sourceFile, 
	      SD_CONST ST_INT lineNum, 
              SD_CONST ST_INT count, SD_CONST ST_CHAR *buf);


static ST_VOID doSlog (LOG_CTRL *lc, 
		       SD_CONST ST_INT logType, 
		       SD_CONST ST_CHAR *SD_CONST logTypeStr, 
		       SD_CONST ST_CHAR *SD_CONST sourceFile,
		       SD_CONST ST_INT lineNum, 
		       SD_CONST ST_CHAR *SD_CONST format, va_list ap);

/************************************************************************/
/************************************************************************/
/* GLOBAL SLOG VARIABLES						*/

LOG_CTRL sLogCtrlDefault;	/* Default - no logging enabled.	*/
LOG_CTRL *sLogCtrl = &sLogCtrlDefault;
ST_CHAR *_slogAlwaysLogTypeStr = "SLOGALWAYS";

ST_CHAR slogTimeText[TIME_BUF_LEN];
ST_INT sl_max_msg_size = MAX_LOG_SIZE;

/************************************************************************/
/************************************************************************/
/* GLOBAL SLOG FUNCTIONS						*/
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

/************************************************************************/
/*			_slog 						*/
/************************************************************************/

ST_VOID _slog (LOG_CTRL *lc, SD_CONST ST_CHAR *SD_CONST logTypeStr, 
	SD_CONST ST_CHAR *SD_CONST sourceFile,
	SD_CONST ST_INT lineNum, SD_CONST ST_CHAR *format, ...)
  {
va_list	ap;

  va_start (ap, format);
  doSlog (lc, SLOG_NORMAL, logTypeStr, sourceFile, lineNum, format, ap);
  va_end(ap);
  }

/************************************************************************/
/*			slog 						*/
/************************************************************************/

ST_VOID slog (LOG_CTRL *lc, SD_CONST ST_INT logType, SD_CONST ST_CHAR *SD_CONST sourceFile,
	      SD_CONST ST_INT lineNum, SD_CONST ST_CHAR *format, ...)
  {
va_list	ap;

  va_start (ap, format);
  doSlog (lc, logType, NULL, sourceFile, lineNum, format, ap);
  va_end(ap);
  }

/************************************************************************/
/************************************************************************/
/*			slogHex 					*/
/************************************************************************/
/* Main HEX logging function, typically called via macro		*/
/* Create the HEX strings && pass to memory & file logging		*/
/* functions if enabled							*/

ST_VOID slogHex (LOG_CTRL *lc, 
		 SD_CONST ST_INT logType, 
		 SD_CONST ST_CHAR *SD_CONST fileName, 
		 SD_CONST ST_INT lineNum,
	  	 SD_CONST ST_INT numBytes, 
		 SD_CONST ST_VOID *hexData)
  {
  _slogHex (lc, numBytes, hexData);
  }

/************************************************************************/
/*			_slogHex 					*/
/************************************************************************/

#define cvt_nibble(a)		((a) > 9 ? (a) + 'A' - 10 : (a) + '0')
#define DEBUG_BUF_LEN 100

ST_VOID _slogHex (LOG_CTRL *lc, 
		  ST_INT numBytes, 
		  SD_CONST ST_VOID *hexData)
  {
ST_INT i;
ST_INT ascii_index;
ST_INT hex_index;
ST_CHAR debug_buf[DEBUG_BUF_LEN+1]; /* output buffer			*/
ST_INT curlen;
ST_UCHAR ch;
ST_UCHAR hex;
ST_INT32 addr;
ST_UCHAR *buf;

/* Make the compiler happy						*/
  addr = 0;
  buf = (ST_UCHAR *) hexData;

/* OK, now start the dump, line by line					*/
  while (numBytes)
    {
    curlen = numBytes > 16 ? 16 : numBytes;
    for (i = 0; i < DEBUG_BUF_LEN; ++i)
      debug_buf[i] = ' ';
    sprintf (debug_buf,"%05lX  ",addr);	/* data offset 			*/
    hex_index = 7;			/* where to put hex data 	*/
    ascii_index = 58;			/* where to put ascii conversion */
    debug_buf[ascii_index++] = '*';

    for (i = 0; i < curlen; i++)	/* for each byte in this line	*/
      {
      ch = *buf++;  			/* get next character		*/
      hex = (ST_UCHAR) ((ch >> 4) & 0x0f);	/* write HEX chars for the byte	*/
      hex = (ST_UCHAR) cvt_nibble(hex);
      debug_buf[hex_index++] = hex;
      hex = (ST_UCHAR) (ch & 0x0f);
      hex = (ST_UCHAR) cvt_nibble(hex);
      debug_buf[hex_index++] = hex;

      debug_buf[hex_index++] = ' ';

      if (isprint(ch))			/* print ASCII portion		*/
	debug_buf[ascii_index] = ch;
      else
	debug_buf[ascii_index] = '.';	/* just put a '.' there 	*/
      ascii_index++;

      if (i == 7)			/* half way through line	*/
	debug_buf[hex_index++] = ' ';   /* put an extra space here 	*/
      } 				/* end of line			*/

    debug_buf[ascii_index++] = '*';
    debug_buf[ascii_index++] = 0;
    for (i = hex_index; i < 58; i++)
      debug_buf[i] = ' ';  /* clear the buffer between hex and ascii data */

    addr += 16;				/* prepare for next line	*/
    numBytes -= curlen;

/* debug_buf now contains the hex/ascii line to be logged		*/
    u_sloglTxtLogStr (debug_buf);
    }
  }

#if defined(SLOGL_TIME_STAMP)
/* NOTE: This funct same as "slog.c" but LOG_ELAPSEDTIME_EN option removed.*/
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

    GetLocalTime (&systime);
    sprintf (tmp_buf, "%04d-%02d-%02d %02d:%02d:%02d.%03d",
             systime.wYear, systime.wMonth, systime.wDay,
             systime.wHour, systime.wMinute, systime.wSecond, systime.wMilliseconds);
    /* If generated string is too long, return empty string	*/
    if (strlen (tmp_buf) < sizeof (slogTimeText))
      strcpy (slogTimeText, tmp_buf);

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
#endif /* #if defined(SLOGL_TIME_STAMP) */

/************************************************************************/
/*			slogTrimFileName 				*/
/************************************************************************/

ST_VOID slogTrimFileName (ST_CHAR *dest, SD_CONST ST_CHAR *fullName)
  {
ST_INT i,s;
ST_CHAR c;

  s = 0;
  i = 0;
  while ((c = fullName[i]))
    {
    if (c == '\\' || c == '/' || c == ':')
      s = i+1;	/* remember the last path seperator character position	*/
    i++; 
    }

  strncpy (dest,&fullName[s], SLOG_MAX_FNAME);
  dest[SLOG_MAX_FNAME] = 0;
  }

/************************************************************************/
/*			slogCallStack 					*/
/************************************************************************/

ST_VOID slogCallStack (LOG_CTRL *lc, SD_CONST ST_CHAR *txt)
  {
  if (txt)
    {
    u_sloglTxtLogStr (txt);
    }
  }

/************************************************************************/
/*			slogCloseFile 					*/
/************************************************************************/

ST_VOID slogCloseFile (LOG_CTRL *lc)
  {
  }


/************************************************************************/
/************************************************************************/
/* INTERNAL FUNCTIONS							*/
/************************************************************************/
/************************************************************************/
/*			doSlog 						*/
/************************************************************************/

static ST_VOID doSlog (LOG_CTRL *lc, 
		       SD_CONST ST_INT logType, 
		       SD_CONST ST_CHAR *SD_CONST logTypeStr, 
		       SD_CONST ST_CHAR *SD_CONST sourceFile,
		       SD_CONST ST_INT lineNum, 
		       SD_CONST ST_CHAR *SD_CONST format, va_list ap)
  {
ST_CHAR buf[MAX_LOG_SIZE];
ST_INT count;

/* It is OK to pass in a NULL format string when using the dynamic 	*/
/* logging functions - no vsprintf if so				*/
  if (format != NULL)
    {
/* OK, now print the string						*/
    count = vsprintf (buf, format, ap);
    count++;	/* allow for null terminator				*/
    if (count > MAX_LOG_SIZE)
      {
      sprintf (buf, "*** LOG BUFFER OVERRUN : %d", count);
      u_sloglTxtLogStr (buf);
      return;
      }
    }
  else		/* make buf a zero length string just in case		*/
    {
    count = 0;
    buf[0] = 0;
    }

#if defined(SLOGL_TIME_STAMP)
/* If time stamping is desired, get the time string from the user	*/
  if (lc->logCtrl & LOG_TIME_EN)
    slogSetTimeText (lc);  
#endif

  if (logType >=0 || logType == SLOG_CONT) /* standard log type range	*/
    {
    logMsg (logType, logTypeStr, sourceFile, lineNum, count, buf);
    }
  else		/* This is a special log format command			*/
    {
    switch (logType)
      {
      case SLOG_DYN_PAUSE  :
      break;

      case SLOG_DYN_LF     :
        u_sloglTxtLogStr ("");
      break;

      case SLOG_DYN_CLRSCR :
      break;
      }
    }
  }

/************************************************************************/
/*			logMsg 						*/
/************************************************************************/

static ST_VOID logMsg (SD_CONST ST_INT logType, 
	      SD_CONST ST_CHAR *SD_CONST logTypeStr, 
	      SD_CONST ST_CHAR *SD_CONST sourceFile, 
	      SD_CONST ST_INT lineNum, 
              SD_CONST ST_INT count, SD_CONST ST_CHAR *buf)
  {
ST_CHAR fname[SLOG_MAX_FNAME+1];
ST_CHAR srcBuf[SLOG_MAX_FNAME+1 + 15];  /* 15 = 5 for punctuation plus	*/
					/* 10 for lineNum	 	*/
ST_CHAR msgBuf[SLOG_MAX_FNAME+1 + 75];	/* Allow 30 more for time	*/
					/* plus 30 for logType		*/

  if (logType != SLOG_CONT)
    {
    if (sourceFile)  	   /* user supplied source file name		*/
      {
      slogTrimFileName (fname, sourceFile);
      sprintf (srcBuf, " (%s, %d)", fname, lineNum);
      }
    else
      srcBuf[0] = 0;

    if (logTypeStr != NULL)
      sprintf (msgBuf, "%s %s%s", slogTimeText, logTypeStr, srcBuf);
    else
      sprintf (msgBuf, "%s %s", slogTimeText, srcBuf);

    if (strlen (msgBuf) > 1)
      {
      u_sloglTxtLogStr ("");
      u_sloglTxtLogStr (msgBuf);
      }
    }
  u_sloglTxtLogStr (buf);
  }

/************************************************************************/
/************************************************************************/
/* ENVIRONMENT SPECIFIC FUNCTIONS					*/
/* Modify this function to suit the logging environment - it may	*/
/* send the strings over a serial port, LAN connection, IPC, whatever	*/
/************************************************************************/
/************************************************************************/
/*			u_sloglTxtLogStr 					*/
/************************************************************************/

ST_VOID u_sloglTxtLogStr (SD_CONST ST_CHAR *str)
  {
  printf ("\n%s", str);
  }

/************************************************************************/
/* Stub functions needed ONLY to be compatible with "slog" library.	*/
/************************************************************************/

/************************************************************************/
/*			slog_start					*/
/************************************************************************/
ST_RET slog_start (LOG_CTRL *lc, ST_INT max_msg_size)
  {
  return (SD_SUCCESS);
  }
/************************************************************************/
/*			slog_end					*/
/************************************************************************/
ST_RET slog_end (LOG_CTRL *lc)
  {
  return (SD_SUCCESS);
  }
/************************************************************************/
/*			slogDeleteFile					*/
/************************************************************************/
ST_VOID slogDeleteFile (LOG_CTRL *lc)
  {
  /* Not supported by 'slogl'. Do nothing.	*/
  }
/************************************************************************/
/*			slogCloneFile					*/
/************************************************************************/
ST_VOID slogCloneFile (LOG_CTRL *lc, SD_CONST ST_CHAR *newFile)
  {
  /* Not supported by 'slogl'. Do nothing.	*/
  }

