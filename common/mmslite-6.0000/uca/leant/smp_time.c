#if defined(UCA_SMP)
/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1997, All Rights Reserved.		        	*/
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : smp_time.c						*/
/* PRODUCT(S)  : UCA Station Management Protocol (UCA SMP).		*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This USER's module provides time recording and conversion funs.	*/
/*									*/
/*									*/
/* MODIFICATION LOG :							*/
/*									*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 02/18/98  EJV    04     Removed initialized static variables from fun*/
/* 10/16/97  EJV    03     Corrected code for MSDOS,deleted types redef.*/
/* 10/13/97  EJV    02     Added defs for DLL.				*/
/* 10/09/97  EJV    01     Created					*/
/************************************************************************/
#if defined (_WIN32)
#include <windows.h>
#include <mmsystem.h>
#endif

#include "glbtypes.h"
#include "sysincs.h"
#include "smp_usr.h"

#define SMP_SEC_IN_DAY		(24L*60L*60L)
#if defined(_WIN32)
#define SMP_MSEC_IN_DAY		(24L*60L*60L*1000L)
#define SMP_USEC_IN_DAY		(24L*60L*60L*1000L*1000L)
#endif  /* _WIN32 */


	/*======================================================*/
	/*  SMP SLAVE/MASTER functions implemented by user	*/
	/*======================================================*/


/************************************************************************/
/*			u_smp_record_time				*/
/*----------------------------------------------------------------------*/
/* Function implemented by user to record time.				*/
/* The rec_time is a buffer of rec_time_len bytes (SMP_SIZE_OF_REC_TIME)*/
/*									*/
/* Parameters:								*/
/*									*/
/* Return:								*/
/*	ST_RET		SD_SUCCESS or SD_FAILURE.			*/
/************************************************************************/
#if defined(SMP_DLL)
DllExport ST_RET WINAPI u_smp_record_time (ST_UCHAR *rec_time, ST_UINT rec_time_len)
#else
ST_RET u_smp_record_time (ST_UCHAR *rec_time, ST_UINT rec_time_len)
#endif
{
	/*======================================================*/

#if defined(MSDOS) || defined(__MSDOS__)

  if (rec_time_len < sizeof (time_t))
    return (SD_FAILURE);

  time ((time_t *) rec_time);			/* local time */
  return (SD_SUCCESS);

#endif  /* MSDOS || __MSDOS__ */

	/*======================================================*/

#if defined(_WIN32)

  if (rec_time_len < sizeof (SYSTEMTIME))
    return (SD_FAILURE);

  GetSystemTime ((SYSTEMTIME *) rec_time);	/* UCT time */
  return (SD_SUCCESS);

#endif  /* _WIN32 */

	/*======================================================*/

  /* other systems not implemented */
  return (SD_FAILURE);
}


/************************************************************************/
/*			u_smp_convert_time				*/
/*----------------------------------------------------------------------*/
/* Function implemented by user converting recorded time in rec_time	*/
/* buffer to the SMP_TIME struct the application uses.			*/
/* The rec_time is a buffer of rec_time_len bytes (SMP_SIZE_OF_REC_TIME)*/
/*									*/
/* Parameters:								*/
/*									*/
/* Return:								*/
/*	ST_RET		SD_SUCCESS or SD_FAILURE.			*/
/************************************************************************/
#if defined(SMP_DLL)
DllExport ST_RET WINAPI u_smp_convert_time (ST_UCHAR *rec_time,
                                            ST_UINT   rec_time_len,
                                            SMP_TIME *smp_time)
#else
ST_RET u_smp_convert_time (ST_UCHAR *rec_time,
                           ST_UINT   rec_time_len,
                           SMP_TIME *smp_time)
#endif
{
	/*======================================================*/

#if defined(MSDOS) || defined(__MSDOS__)

time_t		  currTime;
time_t	  	  t84;
struct tm	  tm84;
ST_DOUBLE	  tm_dif; /* time in sec from January 1, 1984, 00:00:00	*/

  if (rec_time_len < sizeof (time_t))
    return (SD_FAILURE);

  /* assemble time_t since 1 Jan, 1984 */
  memset (&tm84, 0, sizeof (struct tm));
  tm84.tm_mday = 1;
  tm84.tm_mon  = 0;		/* January=0	*/
  tm84.tm_year = 84;		/* date/time January 1, 1984, 00:00:00	*/
  tm84.tm_isdst = -1;		/* let C lib detect DST, local time */
  t84 = mktime (&tm84);

  currTime = *((time_t *) rec_time);	/* time since January 1, 1970, 00:00:00	*/

  /* get the diffrence beetween local times */
  tm_dif = difftime (currTime, t84);	/* num of secs from 01/01/84	*/
  tm_dif += _timezone;			/* Universal Coordinated Time	*/

  smp_time->days =  (ST_INT32) tm_dif / SMP_SEC_IN_DAY;
  smp_time->msec =  ((ST_INT32) tm_dif - smp_time->days * SMP_SEC_IN_DAY) * 1000;
  smp_time->usec = 0;  		/* microseconds, not implemented */

  return (SD_SUCCESS);

#endif  /* MSDOS || __MSDOS__ */

	/*======================================================*/

#if defined(_WIN32)
FILETIME	currTime;
ST_INT64	tm_dif;
ST_INT64	tCur;
ST_INT64	t84;
FILETIME	t_84;
SYSTEMTIME	tm84;

  if (rec_time_len < sizeof (SYSTEMTIME))
    return (SD_FAILURE);

  memset (&tm84, 0, sizeof (SYSTEMTIME));
  tm84.wYear = 1984;
  tm84.wMonth= 1;		/* January=1	*/
  tm84.wDay  = 1;		/* date/time January 1, 1984, 00:00:00	*/

  SystemTimeToFileTime (&tm84, &t_84);

  t84  = (((ST_INT64) t_84.dwHighDateTime << 32) & 0xFFFFFFFF00000000) +
           (ST_INT64) t_84.dwLowDateTime;


  SystemTimeToFileTime ((SYSTEMTIME *) rec_time, &currTime);

  tCur = (((ST_INT64) currTime.dwHighDateTime << 32) & 0xFFFFFFFF00000000) +
           (ST_INT64) currTime.dwLowDateTime;
  tm_dif = tCur - t84;

  /* this computed diff is in 100-nanosecond intervals, convert to millisec */
  tm_dif = tm_dif / (10*1000);			/* now in milliseconds	*/

  smp_time->days =  (ST_INT32) (tm_dif / SMP_MSEC_IN_DAY);
  smp_time->msec =  (ST_INT32) (tm_dif - (ST_INT64) smp_time->days * SMP_MSEC_IN_DAY);
  smp_time->usec = 0;  		/* microseconds, not implemented */

  return (SD_SUCCESS);

#endif  /* _WIN32 */

	/*======================================================*/

  /* other systems not implemented */
  return (SD_FAILURE);
}

#endif  /* UCA_SMP */

