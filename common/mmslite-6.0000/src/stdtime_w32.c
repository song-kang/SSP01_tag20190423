/*****************************************************************************/
/* SISCO SOFTWARE MODULE HEADER **********************************************/
/*****************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,            */
/*            2005 All Rights Reserved                                       */
/*                                                                           */
/* MODULE NAME : StdTimeW32.h                                                */
/* PRODUCT(S)  : Standard Time Management Library                            */
/*                                                                           */
/* MODULE DESCRIPTION:                                                       */
/*   Implementation of Standard Time Management Library Win32 Compatibility  */
/*   Layer.  This module is compiled with STDTIMEW32_ENABLED on Windows to   */
/*   generate debugging code or to run emulation on Unix.  To run on         */
/*   Windows, compile with STDTIMEW32_DISABLED.  STDTIMEW32_ENABLED is       */
/*   required on Unix/Linux (that is, non-Windows) platforms.                */
/*                                                                           */
/* MODIFICATION LOG:                                                         */
/*   Date    Who  Rev  Comments                                              */
/* --------  ---  ---  ----------------------------------------------------- */
/* 02/03/10  NAV   08  Open VMS port					     */
/* 05/16/08  JRB   07  Fix DST adjustment in StdTimeW32LocalFileTimeToFileTime*/
/* 04/08/08  JRB   06  Chg FILETIME to portable STDTIME_WIN_FILETIME.        */
/* 02/20/08  JRB   05  Use new simpler "TimeTypeEx", "StructTmEx" functions. */
/* 03/18/07  RLH   04  Enable GMT vs. LOC conversions to work on QNX;        */
/*                     fix bug in Bool_StdTimeGmTimeR.                       */
/* 11/21/06  RLH   03  StdTimeRet no longer static, now in stdtime.c         */
/* 06/07/06  RLH   02  Numerous features added, see stdtime.doc              */
/* 01/18/06  RLH   01  Created                                               */
/*****************************************************************************/

#include "stdtime.h"

#define STDTIMEW32_FT_TIMET_SCALE   10000000
#define STDTIMEW32_FT_TIMET_BASE_HI 0x019DB1DE
#define STDTIMEW32_FT_TIMET_BASE_LO 0xD53E8000


/*-***************************************************************************/
/*            Bool_StdTimeGmTimeR - PRIVATE FUNCTION                         */
/*  provide interface for gmtime_r where possible, otherwise gmtime.         */
/*  we provide thread safety for this function, if the operating system and  */
/*  runtime library allows for it.                                           */
/*****************************************************************************/

STDTIME_BOOL Bool_StdTimeGmTimeR (
  time_t *                  pTimeT,
  struct tm *               pStructTm)
  {

  if ((pTimeT == NULL) || (pStructTm == NULL))
    {
    return STDTIME_FALSE;
    }


#ifdef STDTIME_REENTRANT_GMTIME_LOCALTIME

  if (gmtime_r (pTimeT, pStructTm) != 0)
    {
    return STDTIME_TRUE;
    }

  return STDTIME_FALSE;


#elif defined(STDTIME_MSVC8)


#ifdef _USE_64BIT_TIME_T
  if (_gmtime64_s (pStructTm, (__time64_t *) pTimeT) == 0)
    {
    return STDTIME_TRUE;
    }

  return STDTIME_FALSE;


#else /* not _USE_64BIT_TIME_T */

  if (_gmtime32_s (pStructTm, (__time32_t *) pTimeT) == 0)
    {
    return STDTIME_TRUE;
    }

  return STDTIME_FALSE;

#endif /* _USE_64BIT_TIME_T */


#else /* no reentrant gmtime available */

  {
  struct tm *               pTempTm = NULL;

  pTempTm = gmtime (pTimeT);

  if (pTempTm == NULL)
    {
    return STDTIME_FALSE;
    }

  /* copy back to caller */
  *pStructTm = *pTempTm;

  return STDTIME_TRUE;
  }

#endif

  } /* Bool_StdTimeGmTimeR */


/*-***************************************************************************/
/*            Bool_StdTimeLocalTimeR - PRIVATE FUNCTION                      */
/*  provide interface for localtime_r where possible, otherwise localtime.   */
/*  we provide thread safety for this function, if the operating system and  */
/*  runtime library allows for it.                                           */
/*****************************************************************************/

STDTIME_BOOL Bool_StdTimeLocalTimeR (
  time_t *                   pTimeT,
  struct tm *                pStructTm)
  {

  if ((pTimeT == NULL) || (pStructTm == NULL))
    {
    return STDTIME_FALSE;
    }


#ifdef STDTIME_REENTRANT_GMTIME_LOCALTIME

  if (localtime_r (pTimeT, pStructTm) != 0)
    {
    return STDTIME_TRUE;
    }

  return STDTIME_FALSE;


#elif defined(STDTIME_MSVC8)


#ifdef _USE_64BIT_TIME_T
  if (_localtime64_s (pStructTm, (__time64_t *) pTimeT) == 0)
    {
    return STDTIME_TRUE;
    }

  return STDTIME_FALSE;


#else /* not _USE_64BIT_TIME_T */

  if (_localtime32_s (pStructTm, (__time32_t *) pTimeT) == 0)
    {
    return STDTIME_TRUE;
    }

  return STDTIME_FALSE;

#endif /* _USE_64BIT_TIME_T */


#else /* no reentrant localtime available */

  {
  struct tm *               pTempTm = NULL;

  pTempTm = localtime (pTimeT);

  if (pTempTm == NULL)
    {
    return STDTIME_FALSE;
    }

  /* copy back to caller */
  *pStructTm = *pTempTm;

  return STDTIME_TRUE;
  }

#endif

  } /* Bool_StdTimeLocalTimeR */


#if 0 /* Int_StdTimeLocalDiff is obsolete */

/* function is obsolete as of 2007-03-18, will be removed in future release. */

/*-***************************************************************************/
/*            Int_StdTimeLocalDiff - PRIVATE FUNCTION                        */
/*  get difference in minutes of (GMT time) - (local time)                   */
/*  in case gmtime() or localtime() fails, return 0                          */
/*****************************************************************************/

static int32_t              Int_StdTimeLocalDiff ()
  {
  time_t                    gmt_time_t;
  struct tm                 locTm;
  struct tm                 gmtTm;

  int32_t                   locDay;
  int32_t                   gmtDay;
  int32_t                   locMin;
  int32_t                   gmtMin;
  int32_t                   diff;

  /* problem with this logic is that 'time(NULL)' returns current time */
  /* so calculation of LOC vs. GMT is based on current time instead of */
  /* the time value being converted. */

  gmt_time_t = time(NULL);

  if (! Bool_StdTimeLocalTimeR (&gmt_time_t, &locTm))
    {
    return 0;
    }

  locDay = ((locTm.tm_year) * 10000) + ((locTm.tm_mon) * 100)
            +(locTm.tm_mday);

  locMin = ((locTm.tm_hour) * 60) + locTm.tm_min;

  if (! Bool_StdTimeGmTimeR (&gmt_time_t, &gmtTm))
    {
    return 0;
    }

  gmtDay = ((gmtTm.tm_year) * 10000) + ((gmtTm.tm_mon) * 100)
            +(gmtTm.tm_mday);

  gmtMin = ((gmtTm.tm_hour) * 60) + gmtTm.tm_min;

  if (locDay == gmtDay)
    {
    /* loc and gmt are in same day */
    diff = gmtMin - locMin;
    }

  else if (gmtDay > locDay)
    {
    /* gmtDay > locDay, so gmtMin < locMin */

    /* example: 2006-01-01 23:00 EST = 2006-01-02 04:00 GMT */
    /* 04:00 GMT - 23:00 EST = difference of -19:00 but that is wrong. */
    /* it should be a difference of +05:00 */

    /* to correct, calculate (04:00+24:00) GMT - 23:00 EST = +05:00 */
    /* note that adjustment is made in minutes */

    diff = (gmtMin + 1440) - locMin;
    }

  else
    {
    /* gmtDay < locDay, so gmtMin > locMin */

    /* example: 2006-01-02 01:00 MSK = 2006-01-01 22:00 GMT */
    /* 22:00 GMT - 01:00 MSK = difference of +21:00 but that is wrong. */
    /* it should be a difference of -03:00 */

    /* to correct, calculate 22:00 GMT - (01:00+24:00) MSK = -03:00 */
    /* note that adjustment is made in minutes */

    diff = gmtMin - (locMin + 1440);
    }

  /* calculated difference value is in minutes */
  /* for EST, diff = GMT-EST = 300 [ 5 hours ] */
  /* for EDT, diff = GMT-EDT = 240 [ 4 hours ] */

  return diff;

  } /* Int_StdTimeLocalDiff */

#endif /* Int_StdTimeLocalDiff is obsolete */


/*****************************************************************************/
/*  StdTimeW32FileTimeToLocalFileTime                                        */
/*  portable implementation of WIN32 API FileTimeToLocalFileTime             */
/*****************************************************************************/


#if defined(__VMS)
STDTIMEW32_STATIC STDTIME_WIN_BOOL  StdTimeW32FileTimeToLocalFileTi (
#else
STDTIMEW32_STATIC STDTIME_WIN_BOOL  StdTimeW32FileTimeToLocalFileTime (
#endif
  const STDTIME_WIN_FILETIME *      /*I*/ pGmtFileTime,
  STDTIME_WIN_FILETIME *            /*O*/ pLocFileTime)
  {
  STDTIME                   gmtStdTime;
  STDTIME                   locStdTime;
  STDTIME_RC                rc;
  time_t                    gmtTimeT;

  struct tm *               pStructTm;
  struct tm                 locStructTm;
  int32_t                   nsec;

  if ((pGmtFileTime == NULL) || (pLocFileTime == NULL))
    {
    return STDTIME_WIN_FALSE;
    }

  rc = FileTimeToStdTime (pGmtFileTime, &gmtStdTime);

  if (rc != STDTIME_OK)
    return STDTIME_WIN_FALSE;

  rc = StdTimeToTimeTypeEx (&gmtStdTime, &gmtTimeT, &nsec);

  if (rc != STDTIME_OK)
    return STDTIME_WIN_FALSE;

  pStructTm = localtime (&gmtTimeT);

  if (pStructTm == NULL)
    return STDTIME_WIN_FALSE;

  locStructTm = *pStructTm;

  rc = StructTmExToStdTime (&locStructTm, nsec, &locStdTime);

  if (rc != STDTIME_OK)
    return STDTIME_WIN_FALSE;

  rc = StdTimeToFileTime (&locStdTime, pLocFileTime);

  if (rc != STDTIME_OK)
    return STDTIME_WIN_FALSE;

  return STDTIME_WIN_TRUE;

  } /* StdTimeW32FileTimeToLocalFileTime */


/*****************************************************************************/
/*  StdTimeW32LocalFileTimeToFileTime                                        */
/*  portable implementation of WIN32 API LocalFileTimeToFileTime             */
/*****************************************************************************/

#if defined(__VMS)
STDTIMEW32_STATIC STDTIME_WIN_BOOL  StdTimeW32LocalFileTimeToFileTi (
#else
STDTIMEW32_STATIC STDTIME_WIN_BOOL  StdTimeW32LocalFileTimeToFileTime (
#endif
  const STDTIME_WIN_FILETIME *      /*I*/ pLocFileTime,
  STDTIME_WIN_FILETIME *            /*O*/ pGmtFileTime)
  {
  STDTIME                   gmtStdTime;
  STDTIME                   locStdTime;
  STDTIME_RC                rc;
  time_t                    gmtTimeT;

  struct tm                 locStructTm;
  int32_t                   nsec;


  if ((pGmtFileTime == NULL) || (pLocFileTime == NULL))
    {
    return STDTIME_WIN_FALSE;
    }

  rc = FileTimeToStdTime (pLocFileTime, &locStdTime);

  if (rc != STDTIME_OK)
    return STDTIME_WIN_FALSE;

  rc = StdTimeToStructTmEx (&locStdTime, &locStructTm, &nsec);

  if (rc != STDTIME_OK)
    return STDTIME_WIN_FALSE;

  locStructTm.tm_isdst = -1;	/* CRITICAL: make OS figure out if DST	*/
  gmtTimeT = mktime (&locStructTm);

  if (gmtTimeT == (time_t) (-1))
    return STDTIME_WIN_FALSE;

  rc = TimeTypeExToStdTime (gmtTimeT, nsec, &gmtStdTime);

  if (rc != STDTIME_OK)
    return STDTIME_WIN_FALSE;

  rc = StdTimeToFileTime (&gmtStdTime, pGmtFileTime);

  if (rc != STDTIME_OK)
    return STDTIME_WIN_FALSE;

  return STDTIME_WIN_TRUE;

  } /* StdTimeW32LocalFileTimeToFileTime */


/*****************************************************************************/
/*  StdTimeW32_IsLeap - return 1 if leap year, else 0                        */
/*****************************************************************************/


STDTIMEW32_STATIC int       StdTimeW32_IsLeap (int year)
  {
  if ((year < 0 ) || (year > 32767))
    {
    return 0;
    }

  if ((year % 4000) == 0)
    {
    return 0;               /* multiples of 4000 are not leap years */
    }

  if ((year % 400) == 0)
    {
    return 1;               /* multiples of 400 are leap years */
    }

  if ((year % 100) == 0)
    {
    return 0;               /* multiples of 100 are not leap years */
    }

  if ((year % 4) == 0)
    {
    return 1;               /* multiples of 4 are leap years */
    }

  return 0;                 /* all others are not leap years */

  } /* StdTimeW32_IsLeap */


/*****************************************************************************/
/*  StdTimeW32_LeapYearDays - number leap-year days based on year y          */
/*****************************************************************************/


STDTIMEW32_STATIC int       StdTimeW32_LeapYearDays (int y)
  {
  /* number of 4000-year multiples */
  int n4000 = (y / 4000);

  /* number of 400-year multiples in excess of 4000 */
  int n400  = (y % 4000) / 400;

  /* number of 100-year multiples in excess of 400 */
  int n100  = (y % 400)  / 100;

  /* number of 4-year multiples in excess of 100 */
  int n4    = (y % 100)  / 4;

  return
    ( 969 * n4000 )
  + (  97 * n400  )
  + (  24 * n100  )
  + (   1 * n4    );

  } /* StdTimeW32_LeapYearDays */


/*****************************************************************************/
/*  days-per-month table                                                     */
/*****************************************************************************/


static int                  StdTimeW32_DaysPerMon [13] =
  { 00,
    31,   /* JAN */
    28,   /* FEB */
    31,   /* MAR */
    30,   /* APR */
    31,   /* MAY */
    30,   /* JUN */
    31,   /* JUL */
    31,   /* AUG */
    30,   /* SEP */
    31,   /* OCT */
    30,   /* NOV */
    31    /* DEC */
  };


/*****************************************************************************/
/*  StdTimeW32SystemTimeToFileTimeEx                                         */
/*  portable implementation of WIN32 API SystemTimeToFileTime                */
/*****************************************************************************/


STDTIMEW32_STATIC STDTIME_WIN_BOOL  StdTimeW32SystemTimeToFileTimeEx (
  const STDTIME_WIN_SYSTEMTIME *    /*I*/ pSystemTime,
  STDTIME_WIN_FILETIME *            /*O*/ pFileTime,
  int                               /*I*/ nsec,
  int                               /*I*/ usec)
  {
  QUADLIB_U64                       uVal;
  QUADLIB_U64                       uNum;

  STDTIME_WIN_SYSTEMTIME            st;
  STDTIME_WIN_FILETIME              ft;


  int                               i;
  int                               d;
  int                               basedays;

  if ((pSystemTime == NULL) || (pFileTime == NULL))
    {
    return STDTIME_WIN_FALSE;
    }

  st = *pSystemTime;

  /* form # of days for year - will not overflow uVal.lo */

  QUADLIB_U64_HI(uVal) = 0;
  QUADLIB_I64_LO(uVal) = (365 * st.wYear) + StdTimeW32_LeapYearDays (st.wYear);

  /* convert year/mon to days */

  d = st.wDay;

  for (i=1; i < st.wMonth; i++)
    {
    d += StdTimeW32_DaysPerMon [i];
    }

  if (st.wMonth > 2)
  {
      d += StdTimeW32_IsLeap (st.wYear);
  }

  /* calculate: uVal += (d - basedays - 1) */
  /* == uVal = uVal + d - basedays - 1 == uVal + d - (basedays + 1); */

  /* use the equivalence above to avoid one QUADLIB operation, since */
  /* there is an overhead to using them under simulation. */

  QUADLIB_U64_HI(uNum) = 0;

  QUADLIB_I64_LO(uNum) = d;
  QUADLIB_U64_ADD_EQ (uVal, uNum);

  basedays = (STDTIME_WIN_EPOCH_YEAR * 365)
           + StdTimeW32_LeapYearDays(STDTIME_WIN_EPOCH_YEAR);

  QUADLIB_I64_LO(uNum) = basedays + 1;
  QUADLIB_U64_SUB_EQ (uVal, uNum);

  /* calculate: uVal = (uVal * 24) + st.wHour; */

  QUADLIB_U64_LO(uNum) = 24;
  QUADLIB_U64_MUL_EQ (uVal, uNum);

  QUADLIB_U64_LO(uNum) = st.wHour;
  QUADLIB_U64_ADD_EQ (uVal, uNum);

  /* calculate: uVal = (uVal * 60) + st.wMinute; */

  QUADLIB_U64_LO(uNum) = 60;
  QUADLIB_U64_MUL_EQ (uVal, uNum);

  QUADLIB_U64_LO(uNum) = st.wMinute;
  QUADLIB_U64_ADD_EQ (uVal, uNum);

  /* calculate: uVal = (uVal * 60) + st.wSecond; */

  QUADLIB_U64_LO(uNum) = 60;
  QUADLIB_U64_MUL_EQ (uVal, uNum);

  QUADLIB_U64_LO(uNum) = st.wSecond;
  QUADLIB_U64_ADD_EQ (uVal, uNum);

  /* calculate: uVal = (uVal * 1000) + st.wMilliseconds; */

  QUADLIB_U64_LO(uNum) = 1000;
  QUADLIB_U64_MUL_EQ (uVal, uNum);

  QUADLIB_U64_LO(uNum) = st.wMilliseconds;
  QUADLIB_U64_ADD_EQ (uVal, uNum);

  /* calculate: uVal = (uVal * 1000) + usec; */

  QUADLIB_U64_LO(uNum) = 1000;
  QUADLIB_U64_MUL_EQ (uVal, uNum);

  QUADLIB_U64_LO(uNum) = usec;
  QUADLIB_U64_ADD_EQ (uVal, uNum);

  /* calculate: uVal = (uVal * 10) + (nsec/100); */

  QUADLIB_U64_LO(uNum) = 10;
  QUADLIB_U64_MUL_EQ (uVal, uNum);

  QUADLIB_U64_LO(uNum) = (nsec/100);
  QUADLIB_U64_ADD_EQ (uVal, uNum);

  ft.dwLowDateTime  = QUADLIB_U64_LO(uVal);
  ft.dwHighDateTime = QUADLIB_U64_HI(uVal);

  *pFileTime = ft;
  return STDTIME_WIN_TRUE;

  } /* StdTimeW32SystemTimeToFileTimeEx */


/*****************************************************************************/
/*  StdTimeW32SystemTimeToFileTime                                           */
/*  portable implementation of WIN32 API SystemTimeToFileTime                */
/*  convert SYSTEMTIME to FILETIME; usec and nsec values default to zero     */
/*****************************************************************************/


STDTIMEW32_STATIC STDTIME_WIN_BOOL  StdTimeW32SystemTimeToFileTime (
  const STDTIME_WIN_SYSTEMTIME *    /*I*/ pSystemTime,
  STDTIME_WIN_FILETIME *            /*O*/ pFileTime)
  {
  return StdTimeW32SystemTimeToFileTimeEx (pSystemTime, pFileTime, 0, 0);

  } /* StdTimeW32SystemTimeToFileTime */


/*****************************************************************************/
/*  StdTimeW32FileTimeToSystemTime                                           */
/*  portable implementation of WIN32 API FileTimeToSystemTime                */
/*****************************************************************************/


STDTIMEW32_STATIC STDTIME_WIN_BOOL  StdTimeW32FileTimeToSystemTime (
  const STDTIME_WIN_FILETIME *      /*I*/ pFileTime,
  STDTIME_WIN_SYSTEMTIME *          /*O*/ pSystemTime)
  {
  QUADLIB_U64                       uVal;
  QUADLIB_U64                       uMod;
  QUADLIB_U64                       uDiv;

  STDTIME_WIN_SYSTEMTIME            st;
  STDTIME_WIN_FILETIME              ft;

  int                               yday;
  int                               max_yday;
  int                               day;
  int                               mon;
  int                               nsec;
  int                               usec;
  int                               days;
  int                               year;
  int                               year_hi;
  int                               year_lo;
  int                               leap;
  int                               dpm;
  int                               basedays;
  int                               workdays;


#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4244)
/* silence the VC compiler warning message: */
/* "conversion from 'int' to 'WORD', possible loss of data" */
#endif


  if ((pSystemTime == NULL) || (pFileTime == NULL))
    {
    return STDTIME_WIN_FALSE;
    }

  ft = *pFileTime;

  QUADLIB_U64_HI(uVal) = ft.dwHighDateTime;
  QUADLIB_U64_LO(uVal) = ft.dwLowDateTime;

  /* uVal has count of 100 nsecs intervals */
  /* calculate: nsec = (uVal % 10) * 100; */

  QUADLIB_U64_HI(uDiv) = 0;
  QUADLIB_U64_LO(uDiv) = 10;
  uVal = QuadLibU64DivMod (uVal, uDiv, &uMod);
  nsec = QUADLIB_I64_LO(uMod) * 100;

  /* uVal has usecs */
  /* calculate: usec = uVal % 1000; */

  QUADLIB_U64_LO(uDiv) = 1000;
  uVal = QuadLibU64DivMod (uVal, uDiv, &uMod);
  usec = QUADLIB_I64_LO(uMod);

  /* uVal has msecs */
  /* calculate: wMilliseconds = uVal % 1000; */
  /* QUADLIB_U64_LO(uDiv) = 1000; ==> still in effect */

  uVal = QuadLibU64DivMod (uVal, uDiv, &uMod);
  st.wMilliseconds = QUADLIB_U64_LO(uMod);

  /* uVal has secs */
  /* calculate: wSecond = uVal % 60; */

  QUADLIB_U64_LO(uDiv) = 60;
  uVal = QuadLibU64DivMod (uVal, uDiv, &uMod);
  st.wSecond = QUADLIB_U64_LO(uMod);

  /* uVal has mins */
  /* calculate: wMinute = uVal % 60; */
  /* QUADLIB_U64_LO(uDiv) = 60; ==> still in effect */

  uVal = QuadLibU64DivMod (uVal, uDiv, &uMod);
  st.wMinute = QUADLIB_U64_LO(uMod);

  /* uVal has hours */
  /* calculate: wHour = uVal % 24; */

  QUADLIB_U64_LO(uDiv) = 24;
  uVal = QuadLibU64DivMod (uVal, uDiv, &uMod);

  st.wHour = QUADLIB_U64_LO(uMod);

  /* uVal has days */

  days = QUADLIB_I64_LO(uVal);

  /* Jan 1 1601 was a Monday (day number 1) */
  /* determine day of week by adding base day number to relative 0 day */
  /* modulus 7 is the day number */

  st.wDayOfWeek = (days + 1) % 7;

  /* FT Epoch starts on Jan 1, 1601 */
  /* calculate the number of days prior to FT Epoch */
  /* treating year 0 as a year = start of year 0 to end of year 1600 */
  /* which is 1601 years = 584753 */

  basedays = (STDTIME_WIN_EPOCH_YEAR * 365)
           + StdTimeW32_LeapYearDays(STDTIME_WIN_EPOCH_YEAR);

  workdays = days + basedays;

  /* if we assumed every year were 365 days long, */
  /* then workdays / 365 would calculate too high a year, whereas */
  /* workdays / 366 would calculate too low a year */
  /* determine year by forming an estimated year, then */
  /* calculating its total days until we get the right number */

  year_hi = (workdays / 365) + 1;
  year_lo = (workdays / 366) - 1;

  for (year = year_hi; year > year_lo; year--)
    {
    days = (year * 365) + StdTimeW32_LeapYearDays (year);

    if (days <= workdays) break;
    }

  if (year == year_lo)
    {
    /* failed to determine year */
    return STDTIME_WIN_FALSE;
    }

  st.wYear = year;

  /* if 'days' == 6 (for example), then 6 complete days have */
  /* elapsed, plus the number of fractional days (sec, msec, etc.) */
  /* once fractional days exist, we are into the next day */
  /* from a calendar point of view, the day is 'days+1' even if a */
  /* day isn't over yet. */

  /* however, at midnight (fractional days are exactly zero) */
  /* it is assumed to be the start of the next day. */
  /* so regardless, the day of year is days+1 always. */

  yday = workdays - days + 1;
  leap = StdTimeW32_IsLeap (year);
  max_yday = 365 + leap;

  if ((yday < 1) || (yday > max_yday))
    {
    /* yday is invalid for the given year */
    return STDTIME_WIN_FALSE;
    }

  day = yday;

  for (mon=1; mon <= 12; mon++)
    {
    dpm = StdTimeW32_DaysPerMon [mon];

    if (leap && (mon == 2)) dpm++;

    if (day <= dpm) break;
    day -= dpm;
    }

  if (mon > 12)
    {
    /* failed to determine month */
    return STDTIME_WIN_FALSE;
    }

  st.wMonth = mon;
  st.wDay = day;

  *pSystemTime = st;

  return STDTIME_WIN_TRUE;


#ifdef _MSC_VER
#pragma warning(pop)
#endif


  } /* StdTimeW32FileTimeToSystemTime */


/*****************************************************************************/
/*  StdTimeW32GetDayofWeekAndYear                                            */
/*  get day of week, day of year from year, mon, day of month                */
/*  the maximum day number in the STDTIME system is about 3,652,500          */
/*  so, there is no need for 64-bit math in this function, which only        */
/*  deals with days as the finest resolution.                                */
/*****************************************************************************/

STDTIME_RC                  StdTimeW32GetDayofWeekAndYear (
  int32_t                   /*I*/ nYear,
  int32_t                   /*I*/ nMon,
  int32_t                   /*I*/ nDayofMon,
  int32_t *                 /*O*/ pDayofWeek,
  int32_t *                 /*O*/ pDayofYear)
  {
  int32_t                   n;
  int32_t                   yeardays;
  int32_t                   basedays;
  int32_t                   leap;

  int32_t                   nDayofWeek;
  int32_t                   nDayofYear;

  STDTIME_ENUM_FUNC (StdTimeW32GetDayofWeekAndYear)

  /* output fields are optional, but at least one must be present */
  /* for the function to be considered successful */

  if ((pDayofWeek == NULL) && (pDayofYear == NULL))
    {
    STDTIME_RET_EC (null_argument);
    }

  if ((nYear < STDTIME_YEAR_MIN)
  ||  (nYear > STDTIME_YEAR_MAX))
    {
    STDTIME_RET_EC (invalid_year);
    }

  if ((nMon < 1) || (nMon > 12))
    {
    STDTIME_RET_EC (invalid_mon);
    }

  if (nDayofMon < 1)
    {
    STDTIME_RET_EC (invalid_day);
    }

  leap = StdTimeW32_IsLeap (nYear);
  n = StdTimeW32_DaysPerMon [nMon];

  if (nMon == 2)
  {
      n += leap;
  }

  if (nDayofMon > n)
    {
    STDTIME_RET_EC (invalid_day);
    }

  /* form # of days for year - will not overflow 32 bits */

  yeardays = (365 * nYear) + StdTimeW32_LeapYearDays (nYear-1);

  /* convert year/mon to days */

  nDayofYear = nDayofMon;

  for (n=1; n < nMon; n++)
    {
    nDayofYear += StdTimeW32_DaysPerMon [n];
    }

  if (nMon > 2)
  {
      nDayofYear += leap;
  }

  if (pDayofYear != NULL)
    {
    *pDayofYear = nDayofYear;
    }

  if (pDayofWeek == NULL)
    {
    STDTIME_RET_OK;             /* nothing else to do */
    }

  basedays = (STDTIME_WIN_EPOCH_YEAR * 365)
           + StdTimeW32_LeapYearDays(STDTIME_WIN_EPOCH_YEAR);

  yeardays += (nDayofYear - basedays - 1);

  /* uNum now contains a day number relative to January 1, 1601. */
  /* it is known that this day was a Monday (day number == 1) */
  /* so, to get day of week, form mod 7 of the day number + 1 */
  /* and add 1 */

  nDayofWeek = (yeardays + 1) % 7;
  *pDayofWeek = nDayofWeek;

  STDTIME_RET_OK;

  } /* StdTimeW32GetDayofWeekAndYear */



/*****************************************************************************/
/*  gettimeofday() simulation on Win32 systems                               */
/*****************************************************************************/

#define STDTIMEW32_GETTIMEOFDAY_SUCCESS 0
#define STDTIMEW32_GETTIMEOFDAY_FAILURE (-1)

#ifdef STDTIMEW32_USE_GETTIMEOFDAY
#ifdef STDTIMEW32_DEBUGGING

/* on Unix/Linux systems, gettimeofday() is defined in */
/* <time.h> or <sys/time.h> */

#ifndef _WINSOCKAPI_
struct timeval
  {
  long    tv_sec;    /* seconds since Jan 1 1970 */
  long    tv_usec;   /* microseconds (seconds / 1_000_000) */
  };
#endif

STDTIMEW32_STATIC int gettimeofday (
  struct timeval *pTimeVal, void *pObsolete);

/*****************************************************************************/
/*                                                                           */
/*  pObsolete as an argument is obsolete on most Unix/Linux systems, in      */
/*  which it is an error to specify a non-NULL value.                        */
/*                                                                           */
/*  gettimeofday() return codes                                              */
/*  rc 0: success                                                            */
/*  rc -1: failure, errno set.                                               */
/*  errno: EFAULT pTimeVal points outside the accessible address space.      */
/*  errno: EINVAL The structure pointed to by 'pTimeVal' specifies an        */
/*         invalid time (not supported here)                                 */
/*  errno: EINVAL pObsolete argument is not NULL                             */
/*  errno: EOVERFLOW: The system time is greater than 2038, cannot be stored.*/
/*         Windows does not have EOVERFLOW in errno.h; ERANGE could be used, */
/*         but for simulation purposes, this condition is not supported.     */
/*                                                                           */
/*****************************************************************************/


#endif /* STDTIMEW32_DEBUGGING */
#endif /* STDTIMEW32_USE_GETTIMEOFDAY */


/*****************************************************************************/
/*  gettimeofday - simulate Unix/Linux function gettimeofday()               */
/*  function is used for debugging purposes only                             */
/*****************************************************************************/


#ifdef STDTIMEW32_USE_GETTIMEOFDAY
#ifdef STDTIMEW32_DEBUGGING


STDTIMEW32_STATIC int gettimeofday (
  struct timeval *pTimeVal, void *pObsolete)
  {
  STDTIME_WIN_FILETIME       ft;
  QUADLIB_U64                qFileTime;
  QUADLIB_U64                qSec;
  QUADLIB_U64                qNsec100;
  QUADLIB_U64                qNum;

  if (pTimeVal == NULL)
    {
    errno = EFAULT;
    return STDTIMEW32_GETTIMEOFDAY_FAILURE;
    }

  if (pObsolete != NULL)
    {
    /* the obsolete argument is not supposed to be used */
    errno = EINVAL;
    return STDTIMEW32_GETTIMEOFDAY_FAILURE;
    }

  GetSystemTimeAsFileTime (&ft);

  QUADLIB_U64_HI(qFileTime) = ft.dwHighDateTime;
  QUADLIB_U64_LO(qFileTime) = ft.dwLowDateTime;

  /* conversion code adapted from MSDN KB Q167296 */
  /* subtract epoch factor and divide by 10_000_000  */

  QUADLIB_U64_HI(qNum) = STDTIMEW32_FT_TIMET_BASE_HI;
  QUADLIB_U64_LO(qNum) = STDTIMEW32_FT_TIMET_BASE_LO;

  QUADLIB_I64_SUB_EQ (qFileTime, qNum);

  QUADLIB_U64_HI(qNum) = 0;
  QUADLIB_U64_LO(qNum) = STDTIMEW32_FT_TIMET_SCALE;

  qSec = QuadLibU64DivMod (qFileTime, qNum, &qNsec100);

  pTimeVal->tv_sec = (long) QUADLIB_U64_LO(qSec);

  /* qNsec100 has multiples of 100 nanoseconds, or 1/10 microseconds */
  /* to get whole microseconds, divide by 10 */

  pTimeVal->tv_usec = (long) QUADLIB_U64_LO(qNsec100) / 10;

  return STDTIMEW32_GETTIMEOFDAY_SUCCESS;

  } /* gettimeofday */


#endif /* STDTIMEW32_DEBUGGING */
#endif /* STDTIMEW32_USE_GETTIMEOFDAY */


/*****************************************************************************/
/*  StdTimeW32GetTimeOfDayToFileTime - interface Unix to Windows time        */
/*  function is used to replace WIN32 API GetSystemTimeAsFileTime            */
/*****************************************************************************/


#ifdef STDTIMEW32_USE_GETTIMEOFDAY


STDTIMEW32_STATIC void StdTimeW32GetTimeOfDayToFileTime (STDTIME_WIN_FILETIME *pFileTime)
{
  QUADLIB_U64               qResult;
  QUADLIB_U64               qNum;
  struct timeval            timeVal;
  STDTIME_WIN_FILETIME      ft = {0};

  /* since there is no return-code mechanism for the Windows API */
  /* GetSystemTimeAsFileTime, we have none here.  if the input   */
  /* parameter is NULL, the caller accepts the consequences.     */
  /* a call to GetSystemTimeAsFileTime(NULL) crashes on Windows  */

  if (gettimeofday (&timeVal, NULL) != STDTIMEW32_GETTIMEOFDAY_SUCCESS)
    {
    *pFileTime = ft;
    return;
    }

  /* multiply seconds result by 10_000_000 */
  QUADLIB_U64_HI(qResult) = 0;
  QUADLIB_U64_LO(qResult) = timeVal.tv_sec;

  QUADLIB_U64_HI(qNum) = 0;
  QUADLIB_U64_LO(qNum) = STDTIMEW32_FT_TIMET_SCALE;

  QUADLIB_U64_MUL_EQ (qResult, qNum);

  /* add 100nsec intervals by converting tv_usec to 100nsec */
  /* note that 100nsec units = 10 * usec units */
  /* there is no way to know if multiply by 10 will cause overflow */
  /* in 32-bit mode, so quad lib used for this */

  QUADLIB_U64_LO(qNum) = timeVal.tv_usec;
  QUADLIB_U64_MUL10_EQ (qNum);
  QUADLIB_U64_ADD_EQ (qResult, qNum);

  /* add constant difference in epoch values */

  QUADLIB_U64_HI(qNum) = STDTIMEW32_FT_TIMET_BASE_HI;
  QUADLIB_U64_LO(qNum) = STDTIMEW32_FT_TIMET_BASE_LO;

  QUADLIB_U64_ADD_EQ (qResult, qNum);

  ft.dwHighDateTime = QUADLIB_U64_HI(qResult);
  ft.dwLowDateTime  = QUADLIB_U64_LO(qResult);

  *pFileTime = ft;

} /* StdTimeW32GetTimeOfDayToFileTime */


#endif /* STDTIMEW32_USE_GETTIMEOFDAY */


/*****************************************************************************/
/*  clock_gettime() simulation on Win32 systems                              */
/*  StdTimeW32ClockGetTimeToFileTime - interface Unix to Windows time        */
/*****************************************************************************/

#define STDTIMEW32_CLOCKGETTIME_SUCCESS 0
#define STDTIMEW32_CLOCKGETTIME_FAILURE (-1)

#ifdef STDTIMEW32_USE_CLOCKGETTIME
#ifdef STDTIMEW32_DEBUGGING

/* clock_gettime is defined in Unix <time.h> */

#ifndef _CLOCKID_T
#define _CLOCKID_T
typedef int clockid_t;
#endif

#ifndef _TIME_T_DEFINED
typedef long time_t;
#define _TIME_T_DEFINED
#endif

#define CLOCK_REALTIME              0

/* only CLOCK_REALTIME is supported in this simulation */

/*efine CLOCK_MONOTONIC             1 */
/*efine CLOCK_PROCESS_CPUTIME_ID    2 */
/*efine CLOCK_THREAD_CPUTIME_ID     3 */

struct timespec
  {
  time_t  tv_sec;   /* Seconds */
  long    tv_nsec;  /* Nanoseconds */
  };

STDTIMEW32_STATIC int clock_gettime (clockid_t clock_id, struct timespec *tp);

/*****************************************************************************/
/* rc 0: success                                                             */
/* rc -1: failure, errno set.                                                */
/* errno: EFAULT tp points outside the accessible address space.             */
/* errno: EINVAL The clk_id specified is not supported on this system.       */
/*****************************************************************************/


#endif /* STDTIMEW32_DEBUGGING */
#endif /* STDTIMEW32_USE_CLOCKGETTIME */


/*****************************************************************************/
/*  clock_gettime - simulate Unix/Linux function clock_gettime()             */
/*****************************************************************************/

#ifdef STDTIMEW32_USE_CLOCKGETTIME
#ifdef STDTIMEW32_DEBUGGING

STDTIMEW32_STATIC int clock_gettime (
  clockid_t clock_id, struct timespec *pTimeSpec)
  {
  STDTIME_WIN_FILETIME       ft;
  QUADLIB_U64                qFileTime;
  QUADLIB_U64                qSec;
  QUADLIB_U64                qNsec100;
  QUADLIB_U64                qNum;

  if (pTimeSpec == NULL)
    {
    errno = EFAULT;
    return STDTIMEW32_CLOCKGETTIME_FAILURE;
    }

  if (clock_id != CLOCK_REALTIME)
    {
    errno = EINVAL;
    return STDTIMEW32_CLOCKGETTIME_FAILURE;
    }

  GetSystemTimeAsFileTime (&ft);

  QUADLIB_U64_HI(qFileTime) = ft.dwHighDateTime;
  QUADLIB_U64_LO(qFileTime) = ft.dwLowDateTime;

  /* conversion code adapted from MSDN KB Q167296 */
  /* subtract epoch factor and divide by 10_000_000  */

  QUADLIB_U64_HI(qNum) = STDTIMEW32_FT_TIMET_BASE_HI;
  QUADLIB_U64_LO(qNum) = STDTIMEW32_FT_TIMET_BASE_LO;

  QUADLIB_I64_SUB_EQ (qFileTime, qNum);

  QUADLIB_U64_HI(qNum) = 0;
  QUADLIB_U64_LO(qNum) = STDTIMEW32_FT_TIMET_SCALE;

  qSec = QuadLibU64DivMod (qFileTime, qNum, &qNsec100);

  pTimeSpec->tv_sec = (long) QUADLIB_U64_LO(qSec);

  /* qNsec100 has multiples of 100 nanoseconds, or 1/10 microseconds */
  /* to get whole nanoseconds, multiply by 100 */
  /* since this value was calculated as a remainder, it will never */
  /* exceed 10_000_000, so multiplying by 100 will always work */

  pTimeSpec->tv_nsec = (long) QUADLIB_U64_LO(qNsec100) * 100;

  return 0;

  } /* clock_gettime */


#endif /* STDTIMEW32_DEBUGGING */
#endif /* STDTIMEW32_USE_CLOCKGETTIME */


/*****************************************************************************/
/*  StdTimeW32ClockGetTimeToFileTime - interface Unix to Windows time        */
/*  function is used to replace WIN32 API GetSystemTimeAsFileTime            */
/*****************************************************************************/


#ifdef STDTIMEW32_USE_CLOCKGETTIME

#if defined(__VMS)
STDTIMEW32_STATIC void      StdTimeW32ClockGetTimeToFileTim (
#else
STDTIMEW32_STATIC void      StdTimeW32ClockGetTimeToFileTime (
#endif
  STDTIME_WIN_FILETIME      *pFileTime)
  {
  QUADLIB_U64               qResult;
  QUADLIB_U64               qNum;
  struct timespec           timeSpec;
  STDTIME_WIN_FILETIME      ft = {0};

  /* since there is no return-code mechanism for the Windows API */
  /* GetSystemTimeAsFileTime, we have none here.  if the input   */
  /* parameter is NULL, the caller accepts the consequences.     */
  /* a call to GetSystemTimeAsFileTime(NULL) crashes on Windows  */

  if (clock_gettime (CLOCK_REALTIME, &timeSpec) !=
      STDTIMEW32_CLOCKGETTIME_SUCCESS)
    {
    *pFileTime = ft;
    return;
    }

  /* multiply seconds result by 10_000_000 */
#if STDTIME_TIME_T64_ENABLED
  QUADLIB_U64_HI(qResult) = QUADLIB_U64_HI(timeSpec.tv_sec);
  QUADLIB_U64_LO(qResult) = QUADLIB_U64_LO(timeSpec.tv_sec);
#else
  QUADLIB_U64_HI(qResult) = 0;
  QUADLIB_U64_LO(qResult) = (*(uint32_t *)(&(timeSpec.tv_sec)));
#endif

  QUADLIB_U64_HI(qNum) = 0;
  QUADLIB_U64_LO(qNum) = STDTIMEW32_FT_TIMET_SCALE;

  QUADLIB_U64_MUL_EQ (qResult, qNum);

  /* add 100nsec intervals by converting tv_nsec to 100nsec */

  QUADLIB_U64_LO(qNum) = timeSpec.tv_nsec / 100;
  QUADLIB_U64_ADD_EQ (qResult, qNum);

  /* add constant difference in epoch values */

  QUADLIB_U64_HI(qNum) = STDTIMEW32_FT_TIMET_BASE_HI;
  QUADLIB_U64_LO(qNum) = STDTIMEW32_FT_TIMET_BASE_LO;

  QUADLIB_U64_ADD_EQ (qResult, qNum);

  ft.dwHighDateTime = QUADLIB_U64_HI(qResult);
  ft.dwLowDateTime  = QUADLIB_U64_LO(qResult);

  *pFileTime = ft;

  } /* StdTimeW32ClockGetTimeToFileTime */


#endif /* STDTIMEW32_USE_CLOCKGETTIME */


/*****************************************************************************/
/*  StdTimeW32TimeBToFileTime - interface Unix to Windows time               */
/*  function is used to replace WIN32 API GetSystemTimeAsFileTime            */
/*                                                                           */
/*  ftime() and struct timeb exist on Windows and Unix                       */
/*  however, a struct timeb has only millisecond precision, so there is      */
/*  no reason to use it on Windows, and would be used on Unix/Linux only     */
/*  if no better alternatives existed.                                       */
/*****************************************************************************/


#ifdef STDTIMEW32_USE_TIMEB


STDTIMEW32_STATIC void      StdTimeW32TimeBToFileTime (
  STDTIME_WIN_FILETIME      *pFileTime)
  {
  QUADLIB_U64               qResult;
  QUADLIB_U64               qNum;
  struct timeb              timeBuffer;
  STDTIME_WIN_FILETIME      ft = {0};

  /* since there is no return-code mechanism for the Windows API */
  /* GetSystemTimeAsFileTime, we have none here.  if the input   */
  /* parameter is NULL, the caller accepts the consequences.     */
  /* a call to GetSystemTimeAsFileTime(NULL) crashes on Windows  */

  ftime (&timeBuffer);

  /* ftime returns a time_t and a millisecond count */
  /* since there is no further precision, the resulting FILETIME */
  /* value will have zero for microseconds and nanoseconds */

  /* multiply seconds result by 10_000_000 */

#if STDTIME_TIME_T64_ENABLED
  QUADLIB_U64_HI(qResult) = QUADLIB_U64_HI(timeBuffer.time);
  QUADLIB_U64_LO(qResult) = QUADLIB_U64_LO(timeBuffer.time);
#else
  QUADLIB_U64_HI(qResult) = 0;
  QUADLIB_U64_LO(qResult) = (*(uint32_t *)(&(timeBuffer.time)));
#endif

  QUADLIB_U64_HI(qNum) = 0;
  QUADLIB_U64_LO(qNum) = STDTIMEW32_FT_TIMET_SCALE;

  QUADLIB_U64_MUL_EQ (qResult, qNum);

  /* add milliseconds after converting to 100-nanosecond intervals */
  /* this requires multiplying an unsigned short by 10,000 */
  /* the maximum value will not overflow an unsigned 32-bit value */

  QUADLIB_U64_LO(qNum) = 10000 * (unsigned long) timeBuffer.millitm;
  QUADLIB_U64_ADD_EQ (qResult, qNum);

  /* add constant difference in epoch values */

  QUADLIB_U64_HI(qNum) = STDTIMEW32_FT_TIMET_BASE_HI;
  QUADLIB_U64_LO(qNum) = STDTIMEW32_FT_TIMET_BASE_LO;

  QUADLIB_U64_ADD_EQ (qResult, qNum);

  ft.dwHighDateTime = QUADLIB_U64_HI(qResult);
  ft.dwLowDateTime  = QUADLIB_U64_LO(qResult);

  *pFileTime = ft;

  } /* StdTimeW32TimeBToFileTime */


#endif /* STDTIMEW32_USE_TIMEB */


