/*+***************************************************************************/
/* SISCO SOFTWARE MODULE HEADER **********************************************/
/* ***************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,            */
/*            2005 All Rights Reserved                                       */
/*                                                                           */
/* MODULE NAME : stdtime.c                                                   */
/* PRODUCT(S)  : Standard Time Management Library                            */
/*                                                                           */
/* MODULE DESCRIPTION:                                                       */
/*   Implementation of Standard Time Management Library Functions            */
/*                                                                           */
/*   Note: Most functions a produce a return-code value STDTIME_RC.          */
/*   The return code is zero if successful, otherwise it is a structured     */
/*   value in which the low-order 10 bits enumerates an error reason,        */
/*   and the upper 20 bits enumerates the high-level and low-level           */
/*   functions where the error was detected.                                 */
/*                                                                           */
/* MODIFICATION LOG:                                                         */
/*   Date    Who  Rev  Comments                                              */
/* --------  ---  ---  ----------------------------------------------------- */
/* 02/26/10  JRB	Fix size of sfraction.				     */
/* 02/03/10  NAV   15  OpenVMS port					     */
/* 02/20/08  JRB   14  Make STDTIME same as FILETIME (simplifies a lot).     */
/*		       Delete MANY unused functions.                         */
/*		       Simplified TimeTypeEx & StructTmEx functions.         */
/*		       Del rounding code and PVT_STDTIME_ROUND macro.        */
/*		       Rewrote "Date" functions (simpler)(only for WIN32).   */
/*		       Del all STDTIME_LOCAL_USES_TZDB code.                 */
/*		       Rewrote StdTimeToStdTimeStringA (much simpler).       */
/*		       Several functions ONLY for _WIN32 now.                */
/* 04/03/07  RLH   13  Improve performance of local vs. GMT conversions      */
/* 03/18/07  RLH   12  Correct bug in new normalization code.                */
/* 03/16/07  RLH   11  Normalize struct tm, SYSTEMTIME, and STDTIME_FIELDS   */
/*                     after rounding is applied; no rounding for formatting */
/*                     functions.                                            */
/* 02/16/07  RLH   10  Clean up minor warnings under Linux                   */
/* 01/11/07  RLH   09  Perform generalized rounding when creating outside    */
/*                     data types and extra (Ex) fields are not written.     */
/*                     This supersedes the Rev 08 changes.                   */
/*                     Also, add TruncStdTimeString functions.               */
/* 01/10/07  RLH   08  Perform rounding at msec level when creating HpDate   */
/* 11/22/06  RLH   07  Ensure DelimitStdTimeString does not pad a trimmed    */
/*                     string value with trailing blanks.                    */
/* 11/21/06  RLH   06  Remove unused variable to silence a warning msg       */
/* 10/27/06  RLH   05  Correct rounding in Rc_ArgDateExToStdTime             */
/* 10/18/06  RLH   04  Enhanced capabilities for timezone rules              */
/* 09/13/06  RLH   03  Ensure local time conversions work when TZ not set    */
/* 06/07/06  RLH   02  Numerous features added, see stdtime.doc              */
/* 01/18/06  RLH   01  Created                                               */
/*+***************************************************************************/

/* define list of error-code descriptions */
/* this is done by temporary macro and include file */

#define STDTIME_ENUM_ERR(x) #x ,

static char *               enum_stdtime_err_text[] =
  {
  "",                       /* dummy 0 entry */
#include "stdtime_enum_err.h"
  ""
  };

/* temporary macro has served its purpose, so undefine it */
#undef  STDTIME_ENUM_ERR


/* create function name table from enum_stdtime_func.h */
/* this is done by temporary macro and include file */

#define STDTIME_ENUM_FUNC(x) #x ,

static char *               enum_stdtime_func_text[] =
  {
  "",                       /* dummy 0 entry */
#include "stdtime_enum_func.h"
  ""
  };

/* temporary macro has served its purpose, so undefine it */
#undef  STDTIME_ENUM_FUNC


#include "stdtime.h"
#include "time_str.h"
#include <math.h>
#include <assert.h>

/*** private (helper) macros and static areas ********************************/

#define STDTIME_IF(x)       ((rc=(x)) == STDTIME_OK)
#define STDTIME_IFNOT(x)    ((rc=(x)) != STDTIME_OK)

#define STDTIME_IN_RANGE(x,lo,hi)   (((x) >= (lo)) && ((x) <= (hi)))

#define PVT_STDTIME_FORMAT_BUFSIZE  256
#define PVT_STDTIME_FORMAT_DEFAULT  "%a %b %d %H:%M:%S %Y"

/* standard delimiters for ISO 8601-formatted time strings */
/* allow for user to redefine the standard delimiter string */
/* this change must be done carefully */

#ifndef STDTIME_ISO_DLM_STR
#define STDTIME_ISO_DLM_STR         "-T:."
#endif

#if defined(_WIN32)
  static char             Pvt_StdTime_UserDelimString[5] = STDTIME_ISO_DLM_STR;
#endif

#define PVT_STDTIME_ISO_DLM_DATE    (STDTIME_ISO_DLM_STR[0])
#define PVT_STDTIME_ISO_DLM_SEP     (STDTIME_ISO_DLM_STR[1])
#define PVT_STDTIME_ISO_DLM_TIME    (STDTIME_ISO_DLM_STR[2])
#define PVT_STDTIME_ISO_DLM_FRAC    (STDTIME_ISO_DLM_STR[3])

#define PVT_STDTIME_USR_DLM_DATE    (Pvt_StdTime_UserDelimString[0])
#define PVT_STDTIME_USR_DLM_SEP     (Pvt_StdTime_UserDelimString[1])
#define PVT_STDTIME_USR_DLM_TIME    (Pvt_StdTime_UserDelimString[2])
#define PVT_STDTIME_USR_DLM_FRAC    (Pvt_StdTime_UserDelimString[3])

#define PVT_STDTIME_SCALE_FILETIME  10000000

/* value of 11644473600000000 in hex, split into high/low 32-bit values */
#define PVT_STDTIME_U32_TIMET_BASE_HI 0x019DB1DE
#define PVT_STDTIME_U32_TIMET_BASE_LO 0xD53E8000

/* value of 2650467743999999999 in hex, split into high/low 32-bit values */
#define PVT_STDTIME_MAX_FILETIME_HI 0x24C85A5E
#define PVT_STDTIME_MAX_FILETIME_LO 0xD1C03FFF

/* value of 864000000000 in hex, split into high/low 32-bit values */
#define PVT_STDTIME_100NS_PERDAY_HI 0x000000C9
#define PVT_STDTIME_100NS_PERDAY_LO 0x2A69C000


/* form value of 1899-12-30 00:00:00.0000000 as a FILETIME equivalent */
/* value of 94353120000000000 in hex, split into high/low 32-bit values */
#define PVT_STDTIME_U32_COMDATE_BASE_HI 0x014F35A9
#define PVT_STDTIME_U32_COMDATE_BASE_LO 0xA90CC000


#define PVT_STDTIME_COMPARE(x)                                                \
  if (pOne->x < pTwo->x) return STDTIME_LT;                                   \
  if (pOne->x > pTwo->x) return STDTIME_GT                                 /**/


#define PVT_STDTIME_EXPORT_TZOA(x)                                            \
  {                                                                           \
  if ((x)->embTzo[0] != 0)                                                    \
    {                                                                         \
    strncpy ((x)->extTzo.str, (x)->embTzo, STDTIME_TZO_LEN);                  \
    (x)->extTzo.str[STDTIME_TZO_LEN] = 0;                                     \
    (x)->embTzo[0] = 0;                                                       \
    }                                                                         \
  }                                                                        /**/

#define PVT_STDTIME_IMPORT_TZOA(x)                                            \
  {                                                                           \
  if ((x)->extTzo.str[0] != 0)                                                \
    {                                                                         \
    strncpy ((x)->embTzo, (x)->extTzo.str, STDTIME_TZO_LEN);                  \
    (x)->embTzo[STDTIME_TZO_LEN] = 0;                                         \
    (x)->extTzo.str[0] = 0;                                                   \
    }                                                                         \
  }                                                                        /**/


/*** private (helper) functions **********************************************/


/* NOTE: function names beginning with Rc_, Int_, Char_, Wchar_ and Bool_    */
/* are private functions; the prefix identifies the return type.             */


/*-***************************************************************************/
/*            StdTimeRet                                                     */
/*  assemble a STDTIME return code                                           */
/*  when basic error code is 0, always return 0                              */
/*  (used to be static, but was needed in multiple modules)                  */
/*****************************************************************************/


STDTIME_RC                  StdTimeRet (
  const STDTIME_RC          /*I*/ func,
  const STDTIME_RC          /*I*/ ec)
  {
  if (ec == 0) return 0;
  return ((func & STDTIME_10_BITS) << 20) | (ec & STDTIME_20_BITS);
  }

/*-***************************************************************************/
/*            Bool_StdTime_IsBlankA - PRIVATE FUNCTION                       */
/*  Return STDTIME_TRUE if string is nul or whitespace only                  */
/*****************************************************************************/

static STDTIME_BOOL         Bool_StdTime_IsBlankA (
  const char *              /*I*/ str)
  {
  if (str == NULL)
    {
    return STDTIME_FALSE;
    }

  for (; *str; str++)
    {
    if (*str == 0)
      {
      break;
      }

    if (*str > (char) ' ')
      {
      return STDTIME_FALSE;
      }
    }

  return STDTIME_TRUE;

  } /* Bool_StdTime_IsBlankA */


#if 0	/* OBSOLETE: delete next release	*/
/*-***************************************************************************/
/*            Rc_NormalizeStdTimeFields                                      */
/*  ensure a STDTIME_FIELDS value is normalized, adjusting fields as needed. */
/*****************************************************************************/


#define PVT_STDTIME_NORMALIZE_FIELD(curr,num,next)                            \
  while ((curr) < 0)                                                          \
    {                                                                         \
    (curr) += num;                                                            \
    (next)--;                                                                 \
    }                                                                         \
  if ((curr) >= num)                                                          \
    {                                                                         \
    (next) += (curr) / num;                                                   \
    (curr) =  (curr) % num;                                                   \
    }


STDTIME_RC                  Rc_NormalizeStdTimeFields (
  STDTIME_FIELDS *          pFields)
  {
  STDTIME_DELTA             delta = {0};
  int32_t                   new_day = 0;
  int32_t                   max_day;

  STDTIME_ENUM_FUNC (Rc_NormalizeStdTimeFields)

  if (pFields == NULL)
    {
    STDTIME_RET_EC (null_argument);
    }

  PVT_STDTIME_NORMALIZE_FIELD (pFields->nsec, 1000, pFields->usec)
  PVT_STDTIME_NORMALIZE_FIELD (pFields->usec, 1000, pFields->msec)
  PVT_STDTIME_NORMALIZE_FIELD (pFields->msec, 1000, pFields->sec )
  PVT_STDTIME_NORMALIZE_FIELD (pFields->sec,    60, pFields->min )
  PVT_STDTIME_NORMALIZE_FIELD (pFields->min,    60, pFields->hour)
  PVT_STDTIME_NORMALIZE_FIELD (pFields->hour,   24, delta.day)

  if (delta.day == 0)
    {
    /* did not change the day, so no calendar normalization needed */
    STDTIME_RET_OK;
    }

  /* easy normalization: just day, within current month */

  max_day = StdTimeDaysInYearMon (pFields->year, pFields->mon);
  new_day = pFields->day + delta.day;

  if ((new_day >= 1)
  &&  (new_day <= max_day)
  &&  (pFields->mon >= 1)
  &&  (pFields->mon <= 12))
    {
    /* month was good, and day did not overflow the month */
    pFields->day = new_day;
    STDTIME_RET_OK;
    }

  /* hard normalization: do full AddDelta processing */

  STDTIME_RET (StdTimeFields_AddDelta (pFields, &delta));

  } /* Rc_NormalizeStdTimeFields */

#undef PVT_STDTIME_NORMALIZE_FIELD

/*-***************************************************************************/
/*            Rc_StdTime_NormalizeSystemTimeEx                               */
/*  ensure a SYSTEMTIME value is normalized, adjusting fields as needed.     */
/*  method: convert to STDTIME_FIELDS, normalize that, and convert back.     */
/*****************************************************************************/

STDTIME_RC                  Rc_StdTime_NormalizeSystemTimeEx (
  STDTIME_WIN_SYSTEMTIME *  /*IO*/ pSystemTime,
  int32_t *                 /*IO*/ pUsec,
  int32_t *                 /*IO*/ pNsec)
  {
  STDTIME_FIELDS            fields = {0};
  STDTIME_ENUM_FUNC (Rc_StdTime_NormalizeSystemTimeEx)

  if (pSystemTime == NULL)
    {
    STDTIME_RET_EC (null_argument);
    }

  fields.year = (int32_t) pSystemTime->wYear;
  fields.mon  = (int32_t) pSystemTime->wMonth;
  fields.day  = (int32_t) pSystemTime->wDay;

  fields.hour = (int32_t) pSystemTime->wHour;
  fields.min  = (int32_t) pSystemTime->wMinute;
  fields.sec  = (int32_t) pSystemTime->wSecond;

  fields.msec = (int32_t) pSystemTime->wMilliseconds;

  if (pUsec != NULL)
    {
    fields.usec = *pUsec;
    }

  if (pNsec != NULL)
    {
    fields.nsec = *pNsec;
    }

  rc = Rc_NormalizeStdTimeFields (&fields);

  pSystemTime->wYear         = (STDTIME_WIN_WORD) fields.year;
  pSystemTime->wMonth        = (STDTIME_WIN_WORD) fields.mon ;
  pSystemTime->wDay          = (STDTIME_WIN_WORD) fields.day ;

  pSystemTime->wHour         = (STDTIME_WIN_WORD) fields.hour;
  pSystemTime->wMinute       = (STDTIME_WIN_WORD) fields.min ;
  pSystemTime->wSecond       = (STDTIME_WIN_WORD) fields.sec ;

  pSystemTime->wMilliseconds = (STDTIME_WIN_WORD) fields.msec;

  if (pUsec != NULL)
    {
    *pUsec = fields.usec;
    }

  if (pNsec != NULL)
    {
    *pNsec = fields.nsec;
    }

  return rc;

  } /* Rc_StdTime_NormalizeSystemTimeEx */


/*-***************************************************************************/
/*            Rc_StdTime_NormalizeStructTmEx                                 */
/*  ensure a struct tm value is normalized, adjusting fields as needed.      */
/*  method: convert to STDTIME_FIELDS, normalize that, and convert back.     */
/*****************************************************************************/

STDTIME_RC                  Rc_StdTime_NormalizeStructTmEx (
  struct tm *               /*IO*/ pStructTm,
  int32_t *                 /*IO*/ pMsec,
  int32_t *                 /*IO*/ pUsec,
  int32_t *                 /*IO*/ pNsec)
  {
  STDTIME_FIELDS            fields = {0};

  STDTIME_ENUM_FUNC (Rc_StdTime_NormalizeStructTmEx)

  if (pStructTm == NULL)
    {
    STDTIME_RET_EC (null_argument);
    }


  fields.year = (int32_t) pStructTm->tm_year + 1900;
  fields.mon  = (int32_t) pStructTm->tm_mon + 1;
  fields.day  = (int32_t) pStructTm->tm_mday;

  fields.hour = (int32_t) pStructTm->tm_hour;
  fields.min  = (int32_t) pStructTm->tm_min;
  fields.sec  = (int32_t) pStructTm->tm_sec;

  if (pMsec != NULL)
    {
    fields.msec = *pMsec;
    }

  if (pUsec != NULL)
    {
    fields.usec = *pUsec;
    }

  if (pNsec != NULL)
    {
    fields.nsec = *pNsec;
    }

  rc = Rc_NormalizeStdTimeFields (&fields);

  pStructTm->tm_year = (int) fields.year - 1900;
  pStructTm->tm_mon  = (int) fields.mon - 1;
  pStructTm->tm_mday = (int) fields.day;

  pStructTm->tm_hour = (int) fields.hour;
  pStructTm->tm_min  = (int) fields.min;
  pStructTm->tm_sec  = (int) fields.sec;

  if (pMsec != NULL)
    {
    *pMsec = fields.msec;
    }

  if (pUsec != NULL)
    {
    *pUsec = fields.usec;
    }

  if (pNsec != NULL)
    {
    *pNsec = fields.nsec;
    }

  return rc;

  } /* Rc_StdTime_NormalizeStructTmEx */
#endif	/* OBSOLETE: delete next release	*/

/*-***************************************************************************/
/*            Rc_StdTime_FileTimeToSystemTimeEx - PRIVATE FUNCTION           */
/*  convert FILETIME to SYSTEMTIME, extracting and storing fractional        */
/*  seconds not available in a SYSTEMTIME structure, if pointers provided.   */
/*****************************************************************************/

static STDTIME_RC           Rc_StdTime_FileTimeToSystemTimeEx (
  const STDTIME_WIN_FILETIME * /*I*/ pFileTime,
  STDTIME_WIN_SYSTEMTIME *     /*O*/ pSystemTime,
  int32_t *                    /*O*/ pUsec,
  int32_t *                    /*O*/ pNsec)
  {
  /* helper function to convert FILETIME to SYSTEMTIME and capture */
  /* fractional milliseconds */

  QUADLIB_I64               qFileTime;
  QUADLIB_I64               work;
  int32_t                   frac;
  int32_t                   msec;
  int32_t                   usec;
  int32_t                   nsec;

  STDTIME_ENUM_FUNC (Rc_StdTime_FileTimeToSystemTimeEx)

  if (pSystemTime == NULL)
    {
    STDTIME_RET_EC (null_argument);
    }

  *pSystemTime = StdTimeApiZeroSystemTime();

  if (pFileTime == NULL)
    {
    STDTIME_RET_EC (null_argument);
    }

  /* extract fractional seconds (units of 100 nanoseconds) */

  QUADLIB_U64_HI(qFileTime) = pFileTime->dwHighDateTime;
  QUADLIB_U64_LO(qFileTime) = pFileTime->dwLowDateTime;

  work = QUADLIB_U64_MOD (
    qFileTime, QUADLIB_U64_CASTU32 (PVT_STDTIME_SCALE_FILETIME));

  frac = QUADLIB_U64_LO(work);

  if (! STDTIME_API_FILETIMETOSYSTEMTIME (pFileTime, pSystemTime))
    {
    STDTIME_RET_EC (failure_in_api_FileTimeToSystemTime);
    }

  /* extract 1 digit of nanoseconds, but make it a 3-digit value */

  nsec = (frac % STDTIME_PREC_NSEC) * STDTIME_SCALE_NSEC;
  frac /= STDTIME_PREC_NSEC;
  usec = frac % 1000;
  frac /= 1000;
  msec = (frac % 1000);

  /* If pointers not NULL, copy values there.	*/
  if (pNsec)
    *pNsec = nsec;
  if (pUsec)
    *pUsec = usec;

  pSystemTime->wMilliseconds = (STDTIME_WIN_WORD) msec;

#if 0	/* OBSOLETE: delete next release	*/
  STDTIME_RET (Rc_StdTime_NormalizeSystemTimeEx (pSystemTime, pNsec, pUsec));
#endif
  STDTIME_RET_OK;
  } /* Rc_StdTime_FileTimeToSystemTimeEx */


/*-***************************************************************************/
/*            Rc_StdTime_SystemTimeExToFileTime - PRIVATE FUNCTION           */
/*  convert SYSTEMTIME to FILETIME, incorporating additional parameters      */
/*  to account for fractional seconds not available in a SYSTEMTIME.         */
/*****************************************************************************/

static STDTIME_RC           Rc_StdTime_SystemTimeExToFileTime (
  const STDTIME_WIN_SYSTEMTIME *    /*I*/ pSystemTime,
  int32_t                           /*I*/ nUsec,
  int32_t                           /*I*/ nNsec,
  STDTIME_WIN_FILETIME *            /*O*/ pFileTime)
  {
  STDTIME_ENUM_FUNC (Rc_StdTime_SystemTimeExToFileTime)

  /* helper function to convert SYSTEMTIME to FILETIME and add in */
  /* fractional milliseconds */

  if (pFileTime == NULL)
    {
    STDTIME_RET_EC (null_argument);
    }

  *pFileTime = StdTimeApiZeroFileTime();

  if (pSystemTime == NULL)
    {
    STDTIME_RET_EC (null_argument);
    }

  if (! STDTIME_API_SYSTEMTIMETOFILETIME (pSystemTime, pFileTime))
    {
    STDTIME_RET_EC (failure_in_api_SystemTimeToFileTime);
    }

  if ((nUsec != 0) || (nNsec != 0))
    {
    /* add in fractional seconds */
    STDTIME_DELTA           delta = {0};
    STDTIME_SPAN            span;
    QUADLIB_I64             qFileTime;

    FILETIME_TO_QUADLIB_I64 (pFileTime, &qFileTime);
    delta.usec = nUsec;
    delta.nsec = nNsec;
    StdTimeDeltaToStdTimeSpan (&delta, &span);
    QUADLIB_I64_ADD_EQ (qFileTime, span.value);
    QUADLIB_I64_TO_FILETIME (&qFileTime, pFileTime);
    }

  STDTIME_RET_OK;

  } /* Rc_StdTime_SystemTimeExToFileTime */


#if defined(_WIN32)
/*-***************************************************************************/
/*            Int_StdTime_GetNumField - PRIVATE FUNCTION                     */
/*  extract a fixed-length decimal string and Return int32_t value or -1     */
/*****************************************************************************/

static int32_t              Int_StdTime_GetNumField (
  const char *              /*I*/ str,
  int32_t                   /*I*/ len)
  {
  /* extract a decimal string and Return int32_t value */
  /* an exact number of digits must be present */
  /* string is delimited by length, not by null terminator */
  /* on error, Return -1 */

  int32_t                   result = 0;
  int32_t                   i;
  int32_t                   c;

  if ((str == NULL) || (len < 1))
    {
    return -1;
    }

  for (i=0; i < len; i++)
    {
    c = str[i];

    if ((c < '0') || (c > '9'))
      {
      return -1;
      }

    result = (result * 10) + (c - '0');
    }

  return result;

  } /* Int_StdTime_GetNumField */
#endif	/* defined(_WIN32)	*/

/*-***************************************************************************/
/*            StdTimeYearIsLeap                                              */
/*  Return 1 if year is leapyear, else Return 0                              */
/*****************************************************************************/

int32_t                     StdTimeYearIsLeap (
  int32_t                   /*I*/ year)
  {
  /* no assertions made about invalid years */

  if ((year < STDTIME_YEAR_MIN) || (year > STDTIME_YEAR_MAX))
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

  } /* StdTimeYearIsLeap */


/*-***************************************************************************/
/*            StdTimeDaysInYearMon                                           */
/*  calendar function: get maximum number of days in month for a given year  */
/*****************************************************************************/

static int32_t              StdTimeDaysPerMonTab [13] =
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

int32_t                     StdTimeDaysInYearMon (
  int32_t                   /*I*/ year,
  int32_t                   /*I*/ mon)
  {
  int32_t                   leap = 0;

  if ((year < STDTIME_YEAR_MIN)
  ||  (year > STDTIME_YEAR_MAX)
  ||  (mon  < 1)
  ||  (mon  > 12))
    {
    return 0;       /* argument error */
    }

  if (mon == 2)
    {
    leap = StdTimeYearIsLeap (year);
    }

  return leap + StdTimeDaysPerMonTab [mon];

  } /* StdTimeDaysInYearMon */


#if defined(_WIN32)
/*-***************************************************************************/
/*            Rc_StdTime_FileTimeSplit - PRIVATE FUNCTION                    */
/*  split FILETIME into FILETIME, usec and nsec.                             */
/*****************************************************************************/

static STDTIME_RC           Rc_StdTime_FileTimeSplit (
  STDTIME_WIN_FILETIME *    /*IO*/ pFileTime,
  int32_t *                 /*IO*/ pUsec,
  int32_t *                 /*IO*/ pNsec)
  {
  QUADLIB_I64               qFileTime;
  QUADLIB_I64               qFactor;
  QUADLIB_I64               qResult;
  QUADLIB_I64               qRemainder;

  STDTIME_ENUM_FUNC (Rc_StdTime_FileTimeSplit)

  if ((pFileTime == NULL) || (pUsec == NULL) || (pNsec == NULL))
    {
    STDTIME_RET_EC (null_argument);
    }

  /* divide 64-bit filetime by 10000 to extract usec,nsec */
  /* then Return these values */

  FILETIME_TO_QUADLIB_I64 (pFileTime, &qFileTime);
  QUADLIB_U64_HI (qFactor) = 0;
  QUADLIB_U64_LO (qFactor) = 10000;

  qResult = QuadLibI64DivMod (qFileTime, qFactor, &qRemainder);
  qFileTime = QUADLIB_I64_MUL (qResult, qFactor);
  QUADLIB_I64_TO_FILETIME (&qFileTime, pFileTime);

  *pUsec = QUADLIB_I64_LO (qRemainder) / 10;
  *pNsec = (QUADLIB_I64_LO (qRemainder) % 10) * 100;

  STDTIME_RET_OK;

  } /* Rc_StdTime_FileTimeSplit */
#endif	/* defined(_WIN32)	*/


/*** StdTime/StdTimeFields conversions ***************************************/

#if defined(_WIN32)
/*-***************************************************************************/
/*            StdTimeToStdTimeFields                                         */
/*  convert: StdTime structure to StdTimeFields structure                    */
/*  method: StdTime -> FILETIME -> SYSTEMTIME -> StdTimeFields               */
/*****************************************************************************/

STDTIME_RC                  StdTimeToStdTimeFields (
  const STDTIME *           /*I*/ pStdTime,
  STDTIME_FIELDS *          /*O*/ pStdTimeFields)
  {
  STDTIME_WIN_FILETIME      ft;
  STDTIME_WIN_SYSTEMTIME    st;
  int32_t                   usec;
  int32_t                   nsec;

  STDTIME_ENUM_FUNC (StdTimeToStdTimeField)

  if (pStdTimeFields == NULL)
    {
    STDTIME_RET_EC (null_argument);
    }

  *pStdTimeFields = ZeroStdTimeFields ();

  if (pStdTime == NULL)
    {
    STDTIME_RET_EC (null_argument);
    }

  if STDTIME_IF (StdTimeToFileTime (pStdTime, &ft))
    {
    if STDTIME_IF (Rc_StdTime_FileTimeSplit (&ft, &usec, &nsec))
      {
      if (STDTIME_API_FILETIMETOSYSTEMTIME (&ft, &st))
        {
        STDTIME_RET (SystemTimeExToStdTimeFields (
          &st, usec, nsec, pStdTimeFields));
        }
      else
        {
        STDTIME_RET_EC (failure_in_api_FileTimeToSystemTime);
        }
      }
    }

  STDTIME_RET_RC;

  } /* StdTimeToStdTimeFields */


/*-***************************************************************************/
/*            StdTimeFieldsToStdTime                                         */
/*  convert: StdTimeFields structure to StdTime structure                    */
/*  method:  StdTimeFields -> SYSTEMTIME -> FILETIME -> StdTime              */
/*****************************************************************************/

STDTIME_RC                  StdTimeFieldsToStdTime (
  const STDTIME_FIELDS *    /*I*/ pStdTimeFields,
  STDTIME *                 /*O*/ pStdTime)
  {
  STDTIME_WIN_FILETIME      ft;
  STDTIME_WIN_SYSTEMTIME    st;
  int32_t                   usec;
  int32_t                   nsec;

  STDTIME_ENUM_FUNC (StdTimeFieldsToStdTime)

  if (pStdTime == NULL)
    {
    STDTIME_RET_EC (null_argument);
    }

  *pStdTime = ZeroStdTime ();

  if (pStdTimeFields == NULL)
    {
    STDTIME_RET_EC (null_argument);
    }

  if STDTIME_IF (StdTimeFieldsToSystemTimeEx (
       pStdTimeFields, &st, &usec, &nsec))
    {
    if STDTIME_IF (Rc_StdTime_SystemTimeExToFileTime (&st, usec, nsec, &ft))
      {
      *pStdTime = ft;
      }
    }

  STDTIME_RET_RC;

  } /* StdTimeFieldsToStdTime */
#endif	/* defined(_WIN32)	*/

/*** StdTime/external conversions ********************************************/

/*-***************************************************************************/
/*            StdTimeToStdTimeStringA                                        */
/*  convert: StdTime structure to String formatted like this:		     */
/*           2007-11-19T16:59:59.1234567				     */
/*****************************************************************************/
STDTIME_RC                  StdTimeToStdTimeStringA (
  const STDTIME *           /*I*/ pStdTime,
  STDTIME_STRINGA *         /*O*/ pStdTimeString)
  {
  struct tm                 tms;
  int32_t fraction;	/* number of 100-nanosecond intervals*/
  char sfraction [30];	/* sprintf could generate up to 20 digits on 64-bit*/
			/* system + '.' etc. Allow plenty of room.	*/
  /* DEBUG: would be simpler if arg just changed to (char *)		*/
  char *string = (char *) pStdTimeString;	/* cast arg to (char *)	*/
  size_t maxsize=28;	/* caller buffer must be at least this big	*/
  int32_t nsec;

  STDTIME_ENUM_FUNC (StdTimeToStdTimeStringA)

  if (string == NULL)
    {
    STDTIME_RET_EC (null_argument);
    }
  /* Convert to "struct tm" for strftime.   */
  rc = StdTimeToStructTmEx (pStdTime, &tms, &nsec);
  if (rc)
    return (rc);

  fraction = nsec / 100;	/* reduce to 7 digits	*/
  sprintf (sfraction, ".%07d", fraction);	/* include '.' in this string*/
  
  /* CRITICAL: leave room to add 8 char suffix (fraction of second)	*/
  if (strftime (string, maxsize-8, "%Y-%m-%dT%H:%M:%S", &tms) == 0)
    {
    STDTIME_RET_EC (argument_out_of_range);	/* maxsize too small	*/
    }
  /* sfraction should be <= 8 chars, but use strncat to be sure.	*/
  strncat (string, sfraction, 8);
  assert (strlen (string) < maxsize);
  STDTIME_RET_RC;
  }


/*** external/StdTime conversions ********************************************/

#if defined(_WIN32)
/*-***************************************************************************/
/*            StdTimeStringAToStdTime                                        */
/*  convert: StdTimeStringA structure to StdTime structure                   */
/*****************************************************************************/

STDTIME_RC                  StdTimeStringAToStdTime (
  const STDTIME_STRINGA *   /*I*/ pStdTimeString,
  STDTIME *                 /*O*/ pStdTime)
  {
  STDTIME_FIELDS            f;

  STDTIME_ENUM_FUNC (StdTimeStringAToStdTime)

  if (pStdTime == NULL)
    {
    STDTIME_RET_EC (null_argument);
    }

  *pStdTime = ZeroStdTime ();

  if STDTIME_IF (StdTimeStringAToStdTimeFields (pStdTimeString, &f))
    {
    /* StdTimeStringAToStdTimeFields will apply embTzo to form GMT */

    STDTIME_RET (StdTimeFieldsToStdTime (&f, pStdTime));
    }

  STDTIME_RET_RC;

  } /* StdTimeStringAToStdTime */

/*** external/StdTimeFields conversions **************************************/

/*-***************************************************************************/
/*            StdTimeStringAToStdTimeFields                                  */
/*  convert: StdTimeStringA structure to StdTimeFields structure             */
/*****************************************************************************/

/* string format:           yyyy-mm-ddThh:mi:ss.fffffff     */
/*                          0123456789012345678901234567    */

STDTIME_RC                  StdTimeStringAToStdTimeFields (
  const STDTIME_STRINGA *   /*I*/ pStdTimeString,
  STDTIME_FIELDS *          /*O*/ pStdTimeFields)
  {
  /* method: validate delimiters, then extract numeric values */
  /* into a STDTIME_FIELDS structure.  if a timezone offset is present */
  /* then validate it, and apply its value to the time fields */

  STDTIME_STRINGA           s;
  STDTIME_TZOA              tzo;
  char                      c;

  STDTIME_ENUM_FUNC (StdTimeStringAToStdTimeFields)

  if (pStdTimeFields == NULL)
    {
    STDTIME_RET_EC (null_argument);
    }

  *pStdTimeFields = ZeroStdTimeFields ();

  if (pStdTimeString == NULL)
    {
    STDTIME_RET_EC (null_argument);
    }

  s = *pStdTimeString;

  /* check for some reasonable delimiters */

  c = s.str[STDTIME_STRING_DLM_D1];     /* date delim between yyyy and mm */

  if ( (c != PVT_STDTIME_ISO_DLM_DATE)  /* normally '-' */
  &&   (c != PVT_STDTIME_USR_DLM_DATE)
  &&   (c != '/')                       /* common but nonstandard delimiter */
  &&   (c != ' ')
  &&   (c !=  0 ) )
    {
    STDTIME_RET_EC (invalid_delimiter);
    }

  c = s.str[STDTIME_STRING_DLM_D2];     /* date delim between mo and dd */

  if ( (c != PVT_STDTIME_ISO_DLM_DATE)  /* normally '-' */
  &&   (c != PVT_STDTIME_USR_DLM_DATE)
  &&   (c != '/')                       /* common but nonstandard delimiter */
  &&   (c != ' ')
  &&   (c !=  0 ) )
    {
    STDTIME_RET_EC (invalid_delimiter);
    }

  c = s.str[STDTIME_STRING_DLM_S1];     /* separator between dd and hh */

  if ( (c != PVT_STDTIME_ISO_DLM_SEP)   /* normally 'T' */
  &&   (c != PVT_STDTIME_USR_DLM_SEP)
  &&   (c != ' ')
  &&   (c !=  0 ) )
    {
    STDTIME_RET_EC (invalid_delimiter);
    }

  c = s.str[STDTIME_STRING_DLM_T1];     /* time delim between hh and mi */

  if ( (c != PVT_STDTIME_ISO_DLM_TIME)  /* normally ':' */
  &&   (c != PVT_STDTIME_USR_DLM_TIME)
  &&   (c != ' ')
  &&   (c !=  0 ) )
    {
    STDTIME_RET_EC (invalid_delimiter);
    }

  c = s.str[STDTIME_STRING_DLM_T2];     /* time delim between mi and ss */

  if ( (c != PVT_STDTIME_ISO_DLM_TIME)  /* normally ':' */
  &&   (c != PVT_STDTIME_USR_DLM_TIME)
  &&   (c != ' ')
  &&   (c !=  0 ) )
    {
    STDTIME_RET_EC (invalid_delimiter);
    }

  c = s.str[STDTIME_STRING_DLM_F1]; /* fraction delim between ss and fffffff */

  if ( (c != PVT_STDTIME_ISO_DLM_FRAC)  /* normally '.' */
  &&   (c != PVT_STDTIME_USR_DLM_FRAC)
  &&   (c != ' ')
  &&   (c != ',')                   /* iso 8601 also allows comma here */
  &&   (c !=  0 ) )
    {
    STDTIME_RET_EC (invalid_delimiter);
    }

  /* string may be followed by optional embTzo field */
  /* if so, we need to extract its value and adjust the time accordingly */
  /* it is valid for an embTzo to be empty (null string) */

  strncpy (tzo.str, s.embTzo, STDTIME_TZO_LEN);

  if STDTIME_IFNOT (NormalizeStdTimeTzoA (&tzo)) /* normalize also validates */
    {
    STDTIME_RET_RC;
    }

  /* Int_StdTime_GetNumField Returns -1 on error */
  /* this will be detected via ValidStdTimeFields */

  pStdTimeFields->year = Int_StdTime_GetNumField (s.str+0, 4);
  pStdTimeFields->mon  = Int_StdTime_GetNumField (s.str+5, 2);
  pStdTimeFields->day  = Int_StdTime_GetNumField (s.str+8, 2);
  pStdTimeFields->hour = Int_StdTime_GetNumField (s.str+11, 2);
  pStdTimeFields->min  = Int_StdTime_GetNumField (s.str+14, 2);
  pStdTimeFields->sec  = Int_StdTime_GetNumField (s.str+17, 2);
  pStdTimeFields->msec = Int_StdTime_GetNumField (s.str+20, 3);
  pStdTimeFields->usec = Int_StdTime_GetNumField (s.str+23, 3);

  pStdTimeFields->nsec = Int_StdTime_GetNumField (s.str+26, 1)
                       * STDTIME_SCALE_NSEC;

  if STDTIME_IFNOT (ValidStdTimeFields (pStdTimeFields))
    {
    STDTIME_RET_RC;
    }

  if ((tzo.str[0] == '+') || (tzo.str[0] == '-'))
    {
    /* a valid, non-null, non-Z timezone offset exists */
    /* extract the timezone offset fields and adjust the stdtime fields */
    /* +hh:mm:ss# */
    /* 0123456789 */

    STDTIME_DELTA           delta = {0};

    delta.hour = atoi (tzo.str+1);
    delta.min  = atoi (tzo.str+4);

    if (tzo.str[6] == ':')  /* seconds field is present */
      {
      delta.sec = atoi (tzo.str+7);
      }

    /* an offset like -05:00 means that 5 hours is subtracted from GMT to   */
    /* get local time.  however, the -05:00 appears next to the local time, */
    /* not the GMT time.  so, to get back to GMT, 5 hours would have to be  */
    /* added.  thus, '-' means "add offset to get GMT", and '+' means       */
    /* "subtract offset to get GMT".  we could use the normal meaning of    */
    /* '+' and '-' and use a SubDelta call, but there is overhead to negate */
    /* the delta.  instead, we negate only when '+' is used, and then call  */
    /* AddDelta.  it's counter-intuitive, but more efficient.               */

    if (tzo.str[0] == '+')  /* yes, '+' */
      {
      delta.hour = -(delta.hour);
      delta.min  = -(delta.min);
      delta.sec  = -(delta.sec);
      }

    STDTIME_RET (StdTimeFields_AddDelta (pStdTimeFields, &delta));
    }

  STDTIME_RET_OK;

  } /* StdTimeStringAToStdTimeFields */
#endif	/* defined(_WIN32)	*/

/*** Get (current UTC/GMT time) functions ************************************/

/*-***************************************************************************/
/*            GetStdTime                                                     */
/*  get current UTC time and store in StdTime structure                      */
/*  method: get current UTC time as a FILETIME, then convert to StdTime      */
/*****************************************************************************/

STDTIME_RC                  GetStdTime (
  STDTIME *                 /*O*/ pStdTime)
  {
  STDTIME_WIN_FILETIME      gmtFileTime;

  STDTIME_ENUM_FUNC (GetStdTime)

  if (pStdTime == NULL)
    {
    STDTIME_RET_EC (null_argument);
    }

#if defined(__VMS)
  STDTIME_API_GETSYSTEMTIMEASFILET (&gmtFileTime);
#else
  STDTIME_API_GETSYSTEMTIMEASFILETIME (&gmtFileTime);
#endif

  STDTIME_RET (FileTimeToStdTime (&gmtFileTime, pStdTime));

  } /* GetStdTime */


/*** validation functions ****************************************************/

/*-***************************************************************************/
/*            ValidStdTimeFields                                             */
/*  return STDTIME_OK if StdTimeFields is valid, else STDTIME_FALSE          */
/*****************************************************************************/

STDTIME_RC                  ValidStdTimeFields (
  const STDTIME_FIELDS *    /*I*/ pFields)
  {
  STDTIME_ENUM_FUNC (ValidStdTimeFields)

  if (pFields == NULL)
    {
    STDTIME_RET_EC (null_argument);
    }

  if (! STDTIME_IN_RANGE (pFields->year, STDTIME_YEAR_MIN, STDTIME_YEAR_MAX))
    {
    STDTIME_RET_EC (invalid_year);
    }

  if (! STDTIME_IN_RANGE (pFields->mon,  1, 12))
    {
    STDTIME_RET_EC (invalid_mon);
    }

  if (! STDTIME_IN_RANGE (pFields->day,  1, 31))
    {
    STDTIME_RET_EC (invalid_day);
    }

  if (! STDTIME_IN_RANGE (pFields->hour, 0, 23))
    {
    STDTIME_RET_EC (invalid_hour);
    }

  if (! STDTIME_IN_RANGE (pFields->min,  0, 59))
    {
    STDTIME_RET_EC (invalid_min);
    }

  if (! STDTIME_IN_RANGE (pFields->sec,  0, 59))
    {
    STDTIME_RET_EC (invalid_sec);
    }

  if (! STDTIME_IN_RANGE (pFields->msec, 0, 999))
    {
    STDTIME_RET_EC (invalid_msec);
    }

  if (! STDTIME_IN_RANGE (pFields->usec, 0, 999))
    {
    STDTIME_RET_EC (invalid_usec);
    }

  if (! STDTIME_IN_RANGE (pFields->nsec, 0, 999))
    {
    STDTIME_RET_EC (invalid_nsec);
    }

  /* already checked that day is 0-31, now validate per month */

  if (pFields->day >
        StdTimeDaysInYearMon (pFields->year, pFields->mon))
    {
    STDTIME_RET_EC (invalid_mon);
    }

  STDTIME_RET_OK;

  } /* ValidStdTimeFields */


/*** AddFraction adjustment function *****************************************/

/*-***************************************************************************/
/*            StdTimeFields_AddFraction                                      */
/*  a low-overhead StdTimeFields adjustment without using a delta            */
/*  nMsec, nUsec and nNsec must be positive                                  */
/*****************************************************************************/

STDTIME_RC                  StdTimeFields_AddFraction (
  STDTIME_FIELDS *          /*IO*/ pStdTimeFields,
  int32_t                   /*I*/ nMsec,
  int32_t                   /*I*/ nUsec,
  int32_t                   /*I*/ nNsec)
  {
  STDTIME_FIELDS            f;
  int32_t                   maxday;

  STDTIME_ENUM_FUNC (StdTimeFields_AddFraction)

  if (pStdTimeFields == NULL)
    {
    STDTIME_RET_EC (null_argument);
    }

  if ((nMsec < 0) || (nUsec < 0) || (nNsec < 0))
    {
    STDTIME_RET_EC (argument_out_of_range);
    }

  if STDTIME_IFNOT (ValidStdTimeFields (pStdTimeFields))
    {
    STDTIME_RET_RC;
    }

  if ((nMsec == 0) && (nUsec == 0) && (nNsec == 0))
    {
    STDTIME_RET_OK;         /* nothing to do */
    }

  f.nsec = pStdTimeFields->nsec + nNsec;
  pStdTimeFields->nsec = ((f.nsec / 100) * 100) % 1000;

  f.usec = pStdTimeFields->usec + (f.nsec / 1000) + nUsec;
  pStdTimeFields->usec          = (f.usec % 1000);

  f.msec = pStdTimeFields->msec + (f.usec / 1000) + nMsec;
  pStdTimeFields->msec          = (f.msec % 1000);

  f.sec  = pStdTimeFields->sec  + (f.msec / 1000);
  pStdTimeFields->sec           = (f.sec  % 60);

  f.min  = pStdTimeFields->min  + (f.sec  / 60);
  pStdTimeFields->min           = (f.min  % 60);

  f.hour = pStdTimeFields->hour + (f.min  / 60);
  pStdTimeFields->hour          = (f.hour % 24);

  f.day  = pStdTimeFields->day  + (f.hour / 24);
  f.mon  = pStdTimeFields->mon;
  f.year = pStdTimeFields->year;

  /* adjust days using calander logic */

  for (;;)
    {
    maxday = StdTimeDaysInYearMon (f.year, f.mon);

    if (f.day <= maxday)    /* no (further) calendar adj needed */
      {
      pStdTimeFields->mon  = f.mon;
      pStdTimeFields->year = f.year;
      break;                /* and return true */
      }

    f.day -= maxday;
    f.mon++;

    if (f.mon > 12)
      {
      f.mon = 1;
      f.year++;

      if (f.year > STDTIME_YEAR_MAX)
        {
        STDTIME_RET_EC (year_out_of_range);
        }
      }
    } /* for */

  STDTIME_RET_OK;

  } /* StdTimeFields_AddFraction */


/*** AddSpan functions *******************************************************/


/*-***************************************************************************/
/*            StdTime_AddSpan                                                */
/*  use STDTIME_SPAN parm to increment value in StdTime structure            */
/*****************************************************************************/

STDTIME_RC                  StdTime_AddSpan (
  STDTIME *                 /*IO*/ pStdTime,
  const STDTIME_SPAN *      /*I*/  pSpan)
  {
  QUADLIB_I64               qStdTime;

  STDTIME_ENUM_FUNC (StdTime_AddSpan)

  qStdTime = *(QUADLIB_I64 *)pStdTime;

  QUADLIB_I64_ADD_EQ (qStdTime, pSpan->value);

  *pStdTime = *(STDTIME *)&qStdTime;
  STDTIME_RET_OK;
  } /* StdTime_AddSpan */


#if defined(_WIN32)
/*-***************************************************************************/
/*            StdTimeFields_AddSpan                                          */
/*  use STDTIME_SPAN parm to increment value in StdTimeFields structure      */
/*****************************************************************************/

STDTIME_RC                  StdTimeFields_AddSpan (
  STDTIME_FIELDS *          /*IO*/ pStdTimeFields,
  const STDTIME_SPAN *      /*I*/  pSpan)
  {
  STDTIME                   stdTime;

  STDTIME_ENUM_FUNC (StdTimeFields_AddSpan)

  if STDTIME_IF (StdTimeFieldsToStdTime (pStdTimeFields, &stdTime))
    {
    if STDTIME_IF (StdTime_AddSpan (&stdTime, pSpan))
      {
      STDTIME_RET (StdTimeToStdTimeFields (&stdTime, pStdTimeFields));
      }
    }

  STDTIME_RET_RC;

  } /* StdTimeFields_AddSpan */
#endif	/* defined(_WIN32)	*/


/*** SubSpan functions *******************************************************/


/*-***************************************************************************/
/*            StdTime_SubSpan                                                */
/*  use STDTIME_SPAN parm to decrement value in StdTime structure            */
/*****************************************************************************/

STDTIME_RC                  StdTime_SubSpan (
  STDTIME *                 /*IO*/ pStdTime,
  const STDTIME_SPAN *      /*I*/  pSpan)
  {
  QUADLIB_I64               qStdTime;

  STDTIME_ENUM_FUNC (StdTime_SubSpan)

  qStdTime = *(QUADLIB_I64 *)pStdTime;

  QUADLIB_I64_SUB_EQ (qStdTime, pSpan->value);

  *pStdTime = *(STDTIME *)&qStdTime;
  STDTIME_RET_OK;
  } /* StdTime_SubSpan */


/*** AddDelta functions ******************************************************/


/*-***************************************************************************/
/*            StdTime_AddDelta                                               */
/*  use STDTIME_DELTA parm to increment value in StdTime structure           */
/*****************************************************************************/

STDTIME_RC                  StdTime_AddDelta (
  STDTIME *                 /*IO*/ pStdTime,
  const STDTIME_DELTA *     /*I*/  pDelta)
  {
  STDTIME_SPAN              span;

  STDTIME_ENUM_FUNC (StdTime_AddDelta)

  if STDTIME_IF (StdTimeDeltaToStdTimeSpan (pDelta, &span))
    {
    STDTIME_RET (StdTime_AddSpan (pStdTime, &span));
    }

  STDTIME_RET_RC;

  } /* StdTime_AddDelta */

#if defined(_WIN32)
/*-***************************************************************************/
/*            StdTimeFields_AddDelta                                         */
/*  use STDTIME_DELTA parm to increment value in StdTimeFields structure     */
/*****************************************************************************/

STDTIME_RC                  StdTimeFields_AddDelta (
  STDTIME_FIELDS *          /*IO*/ pStdTimeFields,
  const STDTIME_DELTA *     /*I*/  pDelta)
  {
  STDTIME_SPAN              span;

  STDTIME_ENUM_FUNC (StdTimeFields_AddDelta)

  if STDTIME_IF (StdTimeDeltaToStdTimeSpan (pDelta, &span))
    {
    STDTIME_RET (StdTimeFields_AddSpan (pStdTimeFields, &span));
    }

  STDTIME_RET_RC;

  } /* StdTimeFields_AddDelta */
#endif	/* defined(_WIN32)	*/


/*** SubDelta functions ******************************************************/


/*-***************************************************************************/
/*            StdTime_SubDelta                                               */
/*  use STDTIME_DELTA parm to decrement value in StdTime structure           */
/*****************************************************************************/

STDTIME_RC                  StdTime_SubDelta (
  STDTIME *                 /*IO*/ pStdTime,
  const STDTIME_DELTA *     /*I*/  pDelta)
  {
  STDTIME_SPAN              span;

  STDTIME_ENUM_FUNC (StdTime_SubDelta)

  if STDTIME_IF (StdTimeDeltaToStdTimeSpan (pDelta, &span))
    {
    STDTIME_RET (StdTime_SubSpan (pStdTime, &span));
    }

  STDTIME_RET_RC;

  } /* StdTime_SubDelta */


/*** GetSpan functions *******************************************************/


/*-***************************************************************************/
/*            StdTime_GetSpan                                                */
/*  produce a STDTIME_SPAN value as the delta-t of two STDTIME values        */
/*****************************************************************************/


STDTIME_RC                  StdTime_GetSpan (
  const STDTIME *           /*I*/ pStdTimeOne,
  const STDTIME *           /*I*/ pStdTimeTwo,
  STDTIME_SPAN *            /*O*/ pSpan)
  {
  QUADLIB_I64               qOne;
  QUADLIB_I64               qTwo;

  STDTIME_ENUM_FUNC (StdTime_GetSpan)

  qOne = *(QUADLIB_I64 *)pStdTimeOne;
  qTwo = *(QUADLIB_I64 *)pStdTimeTwo;

  pSpan->value = QUADLIB_I64_SUB (qOne, qTwo);

  STDTIME_RET (ValidStdTimeSpan (pSpan));

  } /* StdTime_GetSpan */


/*** GetDelta functions ******************************************************/


/*-***************************************************************************/
/*            StdTime_GetDelta                                               */
/*  produce a STDTIME_DELTA value as the delta-t of two STDTIME values       */
/*****************************************************************************/

STDTIME_RC                  StdTime_GetDelta (
  const STDTIME *           /*I*/ pStdTimeOne,
  const STDTIME *           /*I*/ pStdTimeTwo,
  STDTIME_DELTA *           /*O*/ pDelta)
  {
  STDTIME_SPAN              span;

  STDTIME_ENUM_FUNC (StdTime_GetDelta)

  if STDTIME_IF (StdTime_GetSpan (pStdTimeOne, pStdTimeTwo, &span))
    {
    STDTIME_RET (StdTimeSpanToStdTimeDelta (&span, pDelta));
    }

  STDTIME_RET_RC;

  } /* StdTime_GetDelta */


/*-***************************************************************************/
/*            NormalizeStdTimeDelta                                          */
/*  normalize a delta value to the nearest number of days                    */
/*  this is the same as truncating its value to the nearest day              */
/*****************************************************************************/


STDTIME_RC                  NormalizeStdTimeDelta (
  STDTIME_DELTA *           /*IO*/ pDelta)
  {
  STDTIME_ENUM_FUNC (NormalizeStdTimeDelta)

  STDTIME_RET (TruncStdTimeDelta (pDelta, STDTIME_FIELD_DAY));

  } /* NormalizeStdTimeDelta */


/*-***************************************************************************/
/*            TruncStdTimeDelta                                              */
/*  form normalized value of delta, such that there are non-zero difference  */
/*  values only as large as the selected units of the specified field.       */
/*  for example, to truncate to 'hour' means that the delta value is         */
/*  expressed in units no larger than hours.  thus, an 'hour' value of 25    */
/*  is truncated to hours because it does not get changed to 1 day and 1     */
/*  'hour'.  fields of smaller units do not exceed the normal maximum value  */
/*  for that field.  for example, a value may be truncated to 25 hours, but  */
/*  the 'min' field will be within the absolute value of 0 to 59, etc.       */
/*                                                                           */
/*  all non-zero values in a truncated delta will have the same sign, plus   */
/*  or minus, regardless of how they were originally defined.                */
/*****************************************************************************/

STDTIME_RC                  TruncStdTimeDelta (
  STDTIME_DELTA *           /*IO*/ pDelta,
  int32_t                   /*I*/  field)
  {
  QUADLIB_I64               qVal;
  int32_t                   sign = 1;
  STDTIME_SPAN              span;
  STDTIME_DELTA             result = {0};

  STDTIME_ENUM_FUNC (TruncStdTimeDelta)

  if (pDelta == NULL)
    {
    STDTIME_RET_EC (null_argument);
    }

  if ((field < STDTIME_FIELD_DAY) || (field > STDTIME_FIELD_NSEC))
    {
    /* field selector is invalid */
    STDTIME_RET_EC (field_selector_out_of_range);
    }

  if STDTIME_IFNOT (StdTimeDeltaToStdTimeSpan (pDelta, &span))
    {
    STDTIME_RET_RC;
    }

  qVal = span.value;

  /* at this point, qVal has a count of 100-nanosecond intervals */
  /* and may be positive or negative */
  /* now, rebuilt the delta value with normalized quantities */

  /***************************************************************************/
  /* nsecs - only 1 digit available, but we store as 3 digit nsec            */
  /***************************************************************************/

  if (QUADLIB_I64_LT_0 (qVal))
    {
    qVal = QUADLIB_I64_NEG (qVal);
    sign = -1;
    }

  /* if truncating to nsec we are done */

  if (field == STDTIME_FIELD_NSEC)
    {
    /* since the 100-nanosecond count must be multiplied by 100 to */
    /* get actual nanoseconds, just checking for overflow is not   */
    /* enough.  we must check to see if overflow WILL occur when   */
    /* the multiply is done, but we have to check before we do it  */

    if ((QUADLIB_I64_HI(qVal) != 0)
    ||  (QUADLIB_U64_LO(qVal) > (0x7FFFFFFF / STDTIME_SCALE_NSEC) ))
      {
      STDTIME_RET_EC (overflow_in_delta_nsec);
      /* cannot hold nsec value in I32 field */
      }

    result.nsec = sign * STDTIME_SCALE_NSEC * QUADLIB_I64_LO(qVal);
    *pDelta = result;

    STDTIME_RET_OK;
    }

  result.nsec =
    sign * STDTIME_SCALE_NSEC * QUADLIB_I32_CASTI64 (
      QUADLIB_I64_MOD (qVal, QUADLIB_I64_CASTI32 (STDTIME_PREC_NSEC)) );

  QUADLIB_I64_DIV_EQ (qVal, QUADLIB_I64_CASTI32 (STDTIME_PREC_NSEC));

  /***************************************************************************/
  /*  qVal now has usecs                                                     */
  /***************************************************************************/

  /* if truncating to usec we are done */

  if (field == STDTIME_FIELD_USEC)
    {
    if (QUADLIB_I64_OVERFLOW_I32 (qVal))
      {
      STDTIME_RET_EC (overflow_in_delta_usec);
      /* cannot hold usec value in I32 field */
      }

    result.usec = sign * QUADLIB_I64_LO(qVal);
    *pDelta = result;
    STDTIME_RET_OK;
    }

  result.usec =
    sign * QUADLIB_I32_CASTI64 (
      QUADLIB_I64_MOD (qVal, QUADLIB_I64_CASTI32 (1000)) );

  QUADLIB_I64_DIV_EQ (qVal, QUADLIB_I64_CASTI32 (1000));    /* qVal / 1000 */


  /***************************************************************************/
  /*  qVal now has msecs                                                     */
  /***************************************************************************/

  /* if truncating to msec we are done */

  if (field == STDTIME_FIELD_MSEC)
    {
    if (QUADLIB_I64_OVERFLOW_I32 (qVal))
      {
      STDTIME_RET_EC (overflow_in_delta_msec);
      /* cannot hold msec value in I32 field */
      }

    result.msec = sign * QUADLIB_I64_LO(qVal);
    *pDelta = result;
    STDTIME_RET_OK;
    }

  result.msec =
    sign * QUADLIB_I32_CASTI64 (
      QUADLIB_I64_MOD (qVal, QUADLIB_I64_CASTI32 (1000)) );

  QUADLIB_I64_DIV_EQ (qVal, QUADLIB_I64_CASTI32 (1000));    /* qVal / 1000 */


  /***************************************************************************/
  /*  qVal now has secs                                                      */
  /***************************************************************************/

  /* if truncating to sec we are done */

  if (field == STDTIME_FIELD_SEC)
    {
    if (QUADLIB_I64_OVERFLOW_I32 (qVal))
      {
      STDTIME_RET_EC (overflow_in_delta_sec);
      /* cannot hold sec value in I32 field */
      }

    result.sec = sign * QUADLIB_I64_LO(qVal);
    *pDelta = result;
    STDTIME_RET_OK;
    }

  result.sec =
    sign * QUADLIB_I32_CASTI64 (
      QUADLIB_I64_MOD (qVal, QUADLIB_I64_CASTI32 (60)) );

  QUADLIB_I64_DIV_EQ (qVal, QUADLIB_I64_CASTI32 (60));      /* qVal / 60 */


  /***************************************************************************/
  /*  qVal now has mins                                                      */
  /***************************************************************************/

  /* if truncating to min we are done */

  if (field == STDTIME_FIELD_MIN)
    {
    if (QUADLIB_I64_OVERFLOW_I32 (qVal))
      {
      STDTIME_RET_EC (overflow_in_delta_min);
      /* cannot hold min value in I32 field */
      }

    result.min = sign * QUADLIB_I64_LO(qVal);
    *pDelta = result;
    STDTIME_RET_OK;
    }

  result.min =
    sign * QUADLIB_I32_CASTI64 (
      QUADLIB_I64_MOD (qVal, QUADLIB_I64_CASTI32 (60)) );

  QUADLIB_I64_DIV_EQ (qVal, QUADLIB_I64_CASTI32 (60));      /* qVal / 60 */


  /***************************************************************************/
  /*  qVal now has hours                                                     */
  /***************************************************************************/

  /* if truncating to hour we are done */

  if (field == STDTIME_FIELD_HOUR)
    {
    if (QUADLIB_I64_OVERFLOW_I32 (qVal))
      {
      STDTIME_RET_EC (overflow_in_delta_hour);
      /* cannot hold hour value in I32 field */
      }

    result.hour = sign * QUADLIB_I64_LO(qVal);
    *pDelta = result;
    STDTIME_RET_OK;
    }

  result.hour =
    sign * QUADLIB_I32_CASTI64 (
      QUADLIB_I64_MOD (qVal, QUADLIB_I64_CASTI32 (24)) );

  QUADLIB_I64_DIV_EQ (qVal, QUADLIB_I64_CASTI32 (24));      /* qVal / 24 */


  /***************************************************************************/
  /*  qVal now has days                                                      */
  /***************************************************************************/

  /* months and years are not linear, so they are left as zero */
  /* field == STDTIME_FIELD_DAY must be true because of prior check */

  if (QUADLIB_I64_OVERFLOW_I32 (qVal))
    {
    STDTIME_RET_EC (overflow_in_delta_day);
    /* cannot hold day value in I32 field */
    }

  result.day = sign * QUADLIB_I64_LO(qVal);
  *pDelta = result;
  STDTIME_RET_OK;

  } /* TruncStdTimeDelta */


/*-***************************************************************************/
/*            RoundStdTimeDelta                                              */
/*  change a delta value to a rounded form.  first, truncate the delta value */
/*  using TruncStdTimeDelta above.  then, based on the field, if the field   */
/*  to its 'right' is greater than or equal to its maximum, the 'round'      */
/*  field is incremented.                                                    */
/*                                                                           */
/*  'greater than' and 'increment' are relative terms, because the delta     */
/*  could be positive or negative; the magnitudes and signs are taken into   */
/*  account.                                                                 */
/*                                                                           */
/*  once the rounding occurs, the fields to the right of the rounded field   */
/*  are zeroed out.  in case the selected field the one to its right are     */
/*  both zero, no increment occurs, but zeroing-out is still done.           */
/*                                                                           */
/*  if the delta is considered invalid by TruncStdTimeDelta, no further      */
/*  processing is done.                                                      */
/*                                                                           */
/*  since nanoseconds (nsec) is the right-most field, a request to round to  */
/*  the nsec level is simply ignored; in effect, it is already rounded to    */
/*  that level, and TruncStdTimeDelta would already have dropped digits of   */
/*  nsec smaller than the 100-nanosecond precision limit anyway.             */
/*                                                                           */
/*  the helper function Bool_StdTime_AbsDelta forms the absolute value of a  */
/*  delta field, and returns STDTIME_TRUE if a sign conflict is found, else  */
/*  STDTIME_FALSE;                                                           */
/*                                                                           */
/*****************************************************************************/

static STDTIME_BOOL         Bool_StdTime_AbsDelta (
  int32_t                   *sign,
  int32_t                   *value)
  {
  int32_t                   newsign = 0;

  if (*value > 0)
    {
    newsign = 1;
    }

  else if (*value < 0)
    {
    newsign = -1;
    *value = -(*value);
    }

  if ((newsign == 0) || (*sign == newsign))
    {
    /* *value is zero, or its sign is the same as the 'old' sign */
    return STDTIME_FALSE;   /* no sign conflict detected */
    }

  if (*sign == 0)           /* sign not defined yet */
    {
    *sign = newsign;
    return STDTIME_FALSE;   /* no sign conflict detected */
    }

  return STDTIME_TRUE;      /* error: delta has conflicting signs */

  } /* Bool_StdTime_AbsDelta */


/*-***************************************************************************/
/*            RoundStdTimeDelta                                              */
/*****************************************************************************/

STDTIME_RC                  RoundStdTimeDelta (
  STDTIME_DELTA *           /*IO*/ pDelta,
  int32_t                   /*I*/  field)
  {
  int32_t                   sign = 0;
  STDTIME_DELTA             f;

  STDTIME_ENUM_FUNC (RoundStdTimeDelta)

  if STDTIME_IFNOT (TruncStdTimeDelta (pDelta, field))
    {
    STDTIME_RET_RC;
    }

  if (field == STDTIME_FIELD_NSEC)
    {
    /* nothing to round - OK - leave value as is */

    STDTIME_RET_OK;
    }

  /* analyze sign of delta fields, and form absolute value of delta */

  f = *pDelta;          /* local copy */

  if ( (Bool_StdTime_AbsDelta (&sign, &f.day ))
  ||   (Bool_StdTime_AbsDelta (&sign, &f.hour))
  ||   (Bool_StdTime_AbsDelta (&sign, &f.min ))
  ||   (Bool_StdTime_AbsDelta (&sign, &f.sec ))
  ||   (Bool_StdTime_AbsDelta (&sign, &f.msec))
  ||   (Bool_StdTime_AbsDelta (&sign, &f.usec))
  ||   (Bool_StdTime_AbsDelta (&sign, &f.nsec)) )
    {
    STDTIME_RET_EC (conflicting_sign_delta);
    }

  /* rounding a field means to increment it if the field to its */
  /* 'right' is >= half the (max+1) value for it */

  /* whether rounds occurs or not, the fields to the right are */
  /* zeroed out to maintain a numerically valid delta value */
  /* to the requested precision of the field being rounded */

  switch (field)
    {
    case STDTIME_FIELD_DAY:
      if (f.hour >= 12)
        {
        f.day++;
        }

      f.hour = f.min = f.sec = f.msec = f.usec = f.nsec = 0;
      break;

    case STDTIME_FIELD_HOUR:
      if (f.min >= 30)
        {
        f.hour++;
        }

      f.min = f.sec = f.msec = f.usec = f.nsec = 0;
      break;

    case STDTIME_FIELD_MIN:
      if (f.sec >= 30)
        {
        f.min++;
        }

      f.sec = f.msec = f.usec = f.nsec = 0;
      break;

    case STDTIME_FIELD_SEC:
      if (f.msec >= 500)
        {
        f.sec++;
        }

      f.msec = f.usec = f.nsec = 0;
      break;

    case STDTIME_FIELD_MSEC:
      if (f.usec >= 500)
        {
        f.msec++;
        }

      f.usec = f.nsec = 0;
      break;

    case STDTIME_FIELD_USEC:
      if (f.nsec >= 500)
        {
        f.usec++;
        }

      f.nsec = 0;
      break;

    default:
     STDTIME_RET_EC (unexpected_condition); /* should not occur */

    } /* switch (field) */

  /* copy back to pDelta parameter, with correct sign */

  pDelta->day  = sign * f.day;
  pDelta->hour = sign * f.hour;
  pDelta->min  = sign * f.min;
  pDelta->sec  = sign * f.sec;
  pDelta->msec = sign * f.msec;
  pDelta->usec = sign * f.usec;
  pDelta->nsec = sign * f.nsec;

  STDTIME_RET_OK;

  } /* RoundStdTimeDelta */


/*** Delta/Span conversion functions *****************************************/


/*-***************************************************************************/
/*            StdTimeDeltaToStdTimeSpan                                      */
/*  convert a StdTimeDelta to a StdTimeSPan value                            */
/*  if the resultant value exceeds the maximum range for a span, return      */
/*  STDTIME_FALSE.  the value range is about +/- 8400 years, in terms of     */
/*  100-nanosecond intervals.                                                */
/*****************************************************************************/


STDTIME_RC                  StdTimeDeltaToStdTimeSpan (
  const STDTIME_DELTA *     /*I*/ pDelta,
  STDTIME_SPAN *            /*O*/ pSpan)
  {
  QUADLIB_I64               qSum;
  QUADLIB_I64               qTest;

  STDTIME_ENUM_FUNC (StdTimeDeltaToStdTimeSpan)

  if ((pDelta == NULL) || (pSpan == NULL))
    {
    STDTIME_RET_EC (null_argument);
    }

  /* accumulate field values, and convert to nanoseconds */

  /* qSum = day */
  qSum = QUADLIB_I64_CASTI32 (pDelta->day);

  /* qSum (day) *= 24 -> (hour)_*/
  QUADLIB_I64_MUL_EQ (qSum, QUADLIB_I64_CASTI32 (24));

  /* qSum += hour */
  QUADLIB_I64_ADD_EQ (qSum, QUADLIB_I64_CASTI32 (pDelta->hour));

  /* qSum (hour) *= 60 -> (min) */
  QUADLIB_I64_MUL_EQ (qSum, QUADLIB_I64_CASTI32 (60));

  /* qSum += min */
  QUADLIB_I64_ADD_EQ (qSum, QUADLIB_I64_CASTI32 (pDelta->min));

  /* qSum (min) *= 60 -> (sec) */
  QUADLIB_I64_MUL_EQ (qSum, QUADLIB_I64_CASTI32 (60));

  /* qSum += sec */
  QUADLIB_I64_ADD_EQ (qSum, QUADLIB_I64_CASTI32 (pDelta->sec));

  /* qSum (sec) *= 1000 -> (msec) */
  QUADLIB_I64_MUL_EQ (qSum, QUADLIB_I64_CASTI32 (1000));

  /* qSum += msec */
  QUADLIB_I64_ADD_EQ (qSum, QUADLIB_I64_CASTI32 (pDelta->msec));

  /* qSum (msec) *= 1000 -> (usec) */
  QUADLIB_I64_MUL_EQ (qSum, QUADLIB_I64_CASTI32 (1000));

  /* qSum += usec */
  QUADLIB_I64_ADD_EQ (qSum, QUADLIB_I64_CASTI32 (pDelta->usec));

  /* qSum (usec) *= 10 -> (nsec/100) */
  QUADLIB_I64_MUL10_EQ (qSum);

  /* qSum += nsec/100 */
  QUADLIB_I64_ADD_EQ (qSum, QUADLIB_I64_CASTI32 ((pDelta->nsec) / 100));

  pSpan->value = qSum;

  if (QUADLIB_I64_LT_0 (qSum))
    {
    /* create negative test value and see if value is less than it */
    QUADLIB_U64_HI (qTest) = STDTIME_SPAN_MIN_HI;
    QUADLIB_U64_LO (qTest) = STDTIME_SPAN_MIN_LO;

    if (QUADLIB_I64_LT (qSum, qTest))   /* qSum < qTest */
      {
      STDTIME_RET_EC (span_out_of_range);
      }
    }

  else  /* qSum >= 0 */
    {
    /* create positive test value and see if value is less than it */
    QUADLIB_U64_HI (qTest) = STDTIME_SPAN_MAX_HI;
    QUADLIB_U64_LO (qTest) = STDTIME_SPAN_MAX_LO;

    if (QUADLIB_I64_GT (qSum, qTest))   /* qSum > qTest */
      {
      STDTIME_RET_EC (span_out_of_range);
      }
    }

  STDTIME_RET_OK;

  } /* StdTimeDeltaToStdTimeSpan */


/*-***************************************************************************/
/*            StdTimeSpanToStdTimeDelta                                      */
/*  convert a StdTimeSpan to a StdTimeDelta value                            */
/*  if the resultant day value exceeds the maximum range for a day, return   */
/*  STDTIME_FALSE.  the value range 8400 years * 366 days, just to keep      */
/*  the definition simple.                                                   */
/*****************************************************************************/

STDTIME_RC                  StdTimeSpanToStdTimeDelta (
  const STDTIME_SPAN *      /*I*/ pSpan,
  STDTIME_DELTA *           /*O*/ pDelta)
  {
  QUADLIB_U64                       uVal;
  QUADLIB_U64                       uMod;
  QUADLIB_U64                       uDiv;

  int32_t                           sign = 1;

  STDTIME_ENUM_FUNC (StdTimeSpanToStdTimeDelta)

  if ((pSpan == NULL) || (pDelta == NULL))
    {
    STDTIME_RET_EC (null_argument);
    }

  if (QUADLIB_I64_LT_0 (pSpan->value))
    {
    uVal = QUADLIB_I64_NEG (pSpan->value);
    sign = -1;
    }

  else
    {
    uVal = pSpan->value;
    }

  /* uVal has count of 100 nsecs intervals */
  /* calculate: nsec = (uVal % 10) * 100; */

  QUADLIB_U64_HI (uDiv) = 0;
  QUADLIB_U64_LO (uDiv) = 10;
  uVal = QuadLibU64DivMod (uVal, uDiv, &uMod);
  pDelta->nsec = sign * QUADLIB_I64_LO (uMod) * 100;

  /* uVal has usecs */
  /* calculate: usec = uVal % 1000; */

  QUADLIB_U64_LO (uDiv) = 1000;
  uVal = QuadLibU64DivMod (uVal, uDiv, &uMod);
  pDelta->usec = sign * QUADLIB_I64_LO (uMod);

  /* uVal has msecs */
  /* calculate: wMilliseconds = uVal % 1000; */
  /* QUADLIB_U64_LO (uDiv) = 1000; ==> still in effect */

  uVal = QuadLibU64DivMod (uVal, uDiv, &uMod);
  pDelta->msec = sign * QUADLIB_U64_LO (uMod);

  /* uVal has secs */
  /* calculate: wSecond = uVal % 60; */

  QUADLIB_U64_LO (uDiv) = 60;
  uVal = QuadLibU64DivMod (uVal, uDiv, &uMod);
  pDelta->sec = sign * QUADLIB_U64_LO (uMod);

  /* uVal has mins */
  /* calculate: wMinute = uVal % 60; */
  /* QUADLIB_U64_LO (uDiv) = 60; ==> still in effect */

  uVal = QuadLibU64DivMod (uVal, uDiv, &uMod);
  pDelta->min = sign * QUADLIB_U64_LO(uMod);

  /* uVal has hours */
  /* calculate: wHour = uVal % 24; */

  QUADLIB_U64_LO (uDiv) = 24;
  uVal = QuadLibU64DivMod (uVal, uDiv, &uMod);

  pDelta->hour = sign * QUADLIB_U64_LO (uMod);

  /* uVal has days */

  pDelta->day = sign * QUADLIB_I64_LO (uVal);

  if (QUADLIB_I64_OVERFLOW_I32 (uVal))
    {
    STDTIME_RET_EC (overflow_in_delta_day);
    }

  /* we check the positive value for valid range, so that +/- cases */
  /* are both covered in one test */

  if (QUADLIB_I64_LO (uVal) > STDTIME_DELTA_DAY_MAX)
    {
    STDTIME_RET_EC (overflow_in_delta_day);
    }

  STDTIME_RET_OK;

  } /* StdTimeSpanToStdTimeDelta */


/*** validation of delta and span ********************************************/


/*-***************************************************************************/
/*            ValidStdTimeDelta                                              */
/*  validate delta by converting to a span and discard the result            */
/*****************************************************************************/

STDTIME_RC                  ValidStdTimeDelta (
  const STDTIME_DELTA *     /*I*/ pDelta)
  {
  STDTIME_SPAN              span;

  STDTIME_ENUM_FUNC (ValidStdTimeDelta)

  STDTIME_RET (StdTimeDeltaToStdTimeSpan (pDelta, &span));

  } /* ValidStdTimeDelta */


/*-***************************************************************************/
/*            ValidStdTimeSpan                                               */
/*  validate span by converting to a delta and discard the result            */
/*****************************************************************************/

STDTIME_RC                  ValidStdTimeSpan (
  const STDTIME_SPAN *      /*I*/ pSpan)
  {
  STDTIME_DELTA             delta;

  STDTIME_ENUM_FUNC (ValidStdTimeSpan)

  STDTIME_RET (StdTimeSpanToStdTimeDelta (pSpan, &delta));

  } /* ValidStdTimeSpan */


/*** Delta/Span sign-related functions ***************************************/


/*-***************************************************************************/
/*            SgnStdTimeDelta                                                */
/*  characterize the sign of a delta as -1, 0 or +1                          */
/*****************************************************************************/

int32_t                     SgnStdTimeDelta (
  const STDTIME_DELTA *     /*I*/ pDelta)
  {
  STDTIME_SPAN              span;

  if (pDelta == NULL)
    {
    return STDTIME_ERR;
    }

  /* test for a normal sign first */

  if ((pDelta->day  == 0)
  &&  (pDelta->hour == 0)
  &&  (pDelta->min  == 0)
  &&  (pDelta->sec  == 0)
  &&  (pDelta->msec == 0)
  &&  (pDelta->usec == 0)
  &&  (pDelta->nsec == 0))
    {
    return STDTIME_EQ;  /* == 0 */
    }

  if ((pDelta->day  >= 0)
  &&  (pDelta->hour >= 0)
  &&  (pDelta->min  >= 0)
  &&  (pDelta->sec  >= 0)
  &&  (pDelta->msec >= 0)
  &&  (pDelta->usec >= 0)
  &&  (pDelta->nsec >= 0))
    {
    return STDTIME_GT;  /* > 0 (not >= 0, because 0 already checked for) */
    }

  if ((pDelta->day  <= 0)
  &&  (pDelta->hour <= 0)
  &&  (pDelta->min  <= 0)
  &&  (pDelta->sec  <= 0)
  &&  (pDelta->msec <= 0)
  &&  (pDelta->usec <= 0)
  &&  (pDelta->nsec <= 0))
    {
    return STDTIME_LT;  /* < 0 (not <= 0, because 0 already checked for) */
    }

  /* delta has a complex sign. convert to a span, and report its sign */

  if (StdTimeDeltaToStdTimeSpan (pDelta, &span) == STDTIME_OK)
    {
    return SgnStdTimeSpan (&span);
    }

  return STDTIME_ERR;

  } /* SgnStdTimeDelta */


/*-***************************************************************************/
/*            AbsStdTimeDelta                                                */
/*  form absolute value of a delta; depends on its sign                      */
/*****************************************************************************/

STDTIME_RC                  AbsStdTimeDelta (
  STDTIME_DELTA *           /*IO*/ pDelta)
  {
  int32_t                   sign;

  STDTIME_ENUM_FUNC (AbsStdTimeDelta)

  sign = SgnStdTimeDelta (pDelta);  /* handles NULL argument */

  if (sign == STDTIME_ERR)
    {
    STDTIME_RET_EC (failure_in_api_SgnStdTimeDelta);
    }

  if (sign == STDTIME_LT)
    {
    STDTIME_RET (NegStdTimeDelta (pDelta));
    }

  STDTIME_RET_OK;

  } /* AbsStdTimeDelta */


/*-***************************************************************************/
/*            NegStdTimeDelta                                                */
/*  form negative value of a delta; negates each element                     */
/*****************************************************************************/

STDTIME_RC                  NegStdTimeDelta (
  STDTIME_DELTA *           /*IO*/ pDelta)
  {
  STDTIME_ENUM_FUNC (NegStdTimeDelta)

  if (pDelta == NULL)
    {
    STDTIME_RET_EC (null_argument);
    }

  pDelta->day  = -(pDelta->day);
  pDelta->hour = -(pDelta->hour);
  pDelta->min  = -(pDelta->min);
  pDelta->sec  = -(pDelta->sec);
  pDelta->msec = -(pDelta->msec);
  pDelta->usec = -(pDelta->usec);
  pDelta->nsec = -(pDelta->nsec);

  STDTIME_RET_OK;

  } /* NegStdTimeDelta */


/*-***************************************************************************/
/*            SgnStdTimeSpan                                                 */
/*  characterize the sign of a span as -1, 0 or +1                           */
/*****************************************************************************/

int32_t                     SgnStdTimeSpan (
  const STDTIME_SPAN *      /*I*/ pSpan)
  {
  if (pSpan == NULL)
    {
    return STDTIME_ERR;
    }

  if (QUADLIB_I64_LT_0 (pSpan->value))
    {
    return STDTIME_LT;
    }

  if (QUADLIB_I64_EQ_0 (pSpan->value))
    {
    return STDTIME_EQ;
    }

  return STDTIME_GT;
  } /* SgnStdTimeSpan */


/*-***************************************************************************/
/*            AbsStdTimeSpan                                                 */
/*  form absolute value of a span, via call to QUADLIB function              */
/*****************************************************************************/

STDTIME_RC                  AbsStdTimeSpan (
  STDTIME_SPAN *            /*IO*/ pSpan)
  {
  STDTIME_ENUM_FUNC (AbsStdTimeSpan)

  if (pSpan == NULL)
    {
    STDTIME_RET_EC (null_argument);
    }

  pSpan->value = QUADLIB_I64_ABS (pSpan->value);

  STDTIME_RET_OK;

  } /* AbsStdTimeSpan */


/*-***************************************************************************/
/*            NegStdTimeSpan                                                 */
/*  form negative value of a span, via call to QUADLIB function              */
/*****************************************************************************/

STDTIME_RC                  NegStdTimeSpan (
  STDTIME_SPAN *            /*IO*/ pSpan)
  {
  STDTIME_ENUM_FUNC (NegStdTimeSpan)

  if (pSpan == NULL)
    {
    STDTIME_RET_EC (null_argument);
    }

  pSpan->value = QUADLIB_I64_NEG (pSpan->value);

  STDTIME_RET_OK;

  } /* NegStdTimeSpan */


/*** Compare functions *******************************************************/


/*-***************************************************************************/
/*            CompareStdTime                                                 */
/*  compare two STDTIME values and return a strcmp-like result               */
/*****************************************************************************/

int32_t                     CompareStdTime (
  const STDTIME *           /*I*/ pOne,
  const STDTIME *           /*I*/ pTwo)
  {
  if ((pOne == NULL) || (pTwo == NULL))
    {
    return STDTIME_ERR;
    }

  PVT_STDTIME_COMPARE(dwHighDateTime);
  PVT_STDTIME_COMPARE(dwLowDateTime);

  return STDTIME_EQ;

  } /* CompareStdTime */

/*-***************************************************************************/
/*            CompareStdTimeFields                                           */
/*  compare two STDTIME_FIELDS values and return a strcmp-like result        */
/*****************************************************************************/

int32_t                     CompareStdTimeFields (
  const STDTIME_FIELDS *    /*I*/ pOne,
  const STDTIME_FIELDS *    /*I*/ pTwo)
  {
  if ((pOne == NULL) || (pTwo == NULL))
    {
    return STDTIME_ERR;
    }

  PVT_STDTIME_COMPARE(year);
  PVT_STDTIME_COMPARE(mon );
  PVT_STDTIME_COMPARE(day );
  PVT_STDTIME_COMPARE(hour);
  PVT_STDTIME_COMPARE(min );
  PVT_STDTIME_COMPARE(sec );
  PVT_STDTIME_COMPARE(msec);
  PVT_STDTIME_COMPARE(usec);
  PVT_STDTIME_COMPARE(nsec);

  return STDTIME_EQ;

  } /* CompareStdTimeFields */


/*** FileTime functions ******************************************************/


/*-***************************************************************************/
/*            StdTimeToFileTime                                              */
/*  convert: StdTime structure to FILETIME structure                         */
/*****************************************************************************/

STDTIME_RC                  StdTimeToFileTime (
  const STDTIME *           /*I*/ pStdTime,
  STDTIME_WIN_FILETIME *    /*O*/ pFileTime)
  {
  STDTIME_ENUM_FUNC (StdTimeToFileTime)

  if (pFileTime == NULL || pStdTime == NULL)
    {
    STDTIME_RET_EC (null_argument);
    }
  /* Structures are now identical, so just copy.	*/
  *pFileTime = *pStdTime;	/* faster than memcpy	*/
  STDTIME_RET_OK;

  } /* StdTimeToFileTime */


/*-***************************************************************************/
/*            FileTimeToStdTime                                              */
/*  convert: FILETIME structure to StdTime structure                         */
/*****************************************************************************/

STDTIME_RC                  FileTimeToStdTime (
  const STDTIME_WIN_FILETIME * /*I*/ pFileTime,
  STDTIME *                    /*O*/ pStdTime)
  {
  STDTIME_ENUM_FUNC (FileTimeToStdTime)

  if (pFileTime == NULL || pStdTime == NULL)
    {
    STDTIME_RET_EC (null_argument);
    }
  /* Structures are now identical, so just copy.	*/
  *pStdTime = *pFileTime;	/* faster than memcpy	*/
  STDTIME_RET_OK;

  } /* FileTimeToStdTime */


#if defined(_WIN32)
/*-***************************************************************************/
/*            StdTimeFieldsToFileTime                                        */
/*  convert: StdTimeFields structure to FILETIME structure                   */
/*****************************************************************************/

STDTIME_RC                  StdTimeFieldsToFileTime (
  const STDTIME_FIELDS *    /*I*/ pStdTimeFields,
  STDTIME_WIN_FILETIME *    /*O*/ pFileTime)
  {
  STDTIME_WIN_SYSTEMTIME        st;
  STDTIME_WIN_FILETIME          ft;
  QUADLIB_U64                   qFileTime;
  int32_t                       omit = 0;
  uint32_t                      incr32;

  STDTIME_ENUM_FUNC (StdTimeFieldsToFileTime)

  if (pFileTime == NULL)
    {
    STDTIME_RET_EC (null_argument);
    }

  *pFileTime = StdTimeApiZeroFileTime();

  if (pStdTimeFields == NULL)
    {
    STDTIME_RET_EC (null_argument);
    }

  /* the 'omit' argument is like NULL, but suppressing rounding */

  if STDTIME_IFNOT (StdTimeFieldsToSystemTimeEx (
       pStdTimeFields, &st, &omit, &omit))
    {
    STDTIME_RET_RC;
    }

  if (! STDTIME_API_SYSTEMTIMETOFILETIME (&st, &ft))
    {
    STDTIME_RET_EC (failure_in_api_SystemTimeToFileTime);
    }

  /* SYSTEMTIME does not have usec and nsec, so add them to FILETIME */

  QUADLIB_U64_HI (qFileTime) = ft.dwHighDateTime;
  QUADLIB_U64_LO (qFileTime) = ft.dwLowDateTime;

  incr32 = (uint32_t) (pStdTimeFields->usec * STDTIME_PREC_NSEC)
         + (uint32_t) (pStdTimeFields->nsec / STDTIME_SCALE_NSEC);

  QUADLIB_U64_ADD_EQ (qFileTime, QUADLIB_U64_CASTU32 (incr32));

  pFileTime->dwHighDateTime = QUADLIB_U64_HI (qFileTime);
  pFileTime->dwLowDateTime  = QUADLIB_U64_LO (qFileTime);

  STDTIME_RET_OK;

  } /* StdTimeFieldsToFileTime */

/*-***************************************************************************/
/*            FileTimeToStdTimeFields                                        */
/*  convert: FILETIME structure to StdTimeFields structure                   */
/*****************************************************************************/

STDTIME_RC                  FileTimeToStdTimeFields (
  const STDTIME_WIN_FILETIME *  /*I*/ pFileTime,
  STDTIME_FIELDS *              /*O*/ pStdTimeFields)
  {
  STDTIME_WIN_SYSTEMTIME    st;
  int32_t                   usec;
  int32_t                   nsec;

  STDTIME_ENUM_FUNC (FileTimeToStdTimeFields)

  if (pStdTimeFields == NULL)
    {
    STDTIME_RET_EC (null_argument);
    }

  *pStdTimeFields = ZeroStdTimeFields ();

  if (pFileTime == NULL)
    {
    STDTIME_RET_EC (null_argument);
    }

  if STDTIME_IFNOT (Rc_StdTime_FileTimeToSystemTimeEx (
       pFileTime, &st, &usec, &nsec))
    {
    STDTIME_RET_RC;
    }

  STDTIME_RET (SystemTimeExToStdTimeFields (&st, usec, nsec, pStdTimeFields));

  } /* FileTimeToStdTimeFields */
#endif	/* defined(_WIN32)	*/


/*** SystemTimeEx functions **************************************************/

/*-***************************************************************************/
/*            StdTimeToSystemTimeEx                                          */
/*  convert: StdTime structure to SYSTEMTIME and extra fields                */
/*  method: StdTime -> FILETIME -> StdTimeFields                             */
/*****************************************************************************/

STDTIME_RC                  StdTimeToSystemTimeEx (
  const STDTIME *           /*I*/ pStdTime,
  STDTIME_WIN_SYSTEMTIME *  /*O*/ pSystemTime,
  int32_t *                 /*O*/ pUsec,
  int32_t *                 /*O*/ pNsec)
  {
  STDTIME_WIN_FILETIME      ft;

  STDTIME_ENUM_FUNC (StdTimeToSystemTimeEx)

  if (pSystemTime == NULL)
    {
    STDTIME_RET_EC (null_argument);
    }

  *pSystemTime = StdTimeApiZeroSystemTime();

  if STDTIME_IF (StdTimeToFileTime (pStdTime, &ft))
    {
    STDTIME_RET (Rc_StdTime_FileTimeToSystemTimeEx (
      &ft, pSystemTime, pUsec, pNsec));
    }

  STDTIME_RET_RC;

  } /* StdTimeToSystemTimeEx */

/*-***************************************************************************/
/*            SystemTimeExToStdTime                                          */
/*  convert: SYSTEMTIME and extra fields to StdTime structure                */
/*  method:  SYSTEMTIME, fields -> FILETIME -> StdTime                       */
/*****************************************************************************/

STDTIME_RC                  SystemTimeExToStdTime (
  const STDTIME_WIN_SYSTEMTIME * /*I*/ pSystemTime,
  int32_t                        /*I*/ nUsec,
  int32_t                        /*I*/ nNsec,
  STDTIME *                      /*O*/ pStdTime)
  {
  STDTIME_WIN_FILETIME      ft;

  STDTIME_ENUM_FUNC (SystemTimeExToStdTime)

  if (pStdTime == NULL)
    {
    STDTIME_RET_EC (null_argument);
    }

  *pStdTime = ZeroStdTime ();

  if STDTIME_IF (Rc_StdTime_SystemTimeExToFileTime (
       pSystemTime, nUsec, nNsec, &ft))
    {
    STDTIME_RET (FileTimeToStdTime (&ft, pStdTime));
    }

  STDTIME_RET_RC;

  } /* SystemTimeExToStdTime */

#if defined(_WIN32)
/*-***************************************************************************/
/*            StdTimeFieldsToSystemTimeEx                                    */
/*  convert: StdTimeFields structure to SYSTEMTIME and extra fields          */
/*****************************************************************************/

STDTIME_RC                  StdTimeFieldsToSystemTimeEx (
  const STDTIME_FIELDS *    /*I*/ pStdTimeFields,
  STDTIME_WIN_SYSTEMTIME *  /*O*/ pSystemTime,
  int32_t *                 /*O*/ pUsec,  /* optional */
  int32_t *                 /*O*/ pNsec)  /* optional */
  {
  int32_t                   wday;
  int32_t                   msec;
  int32_t                   usec;
  int32_t                   nsec;

  STDTIME_ENUM_FUNC (StdTimeFieldsToSystemTimeEx)

  if (pSystemTime == NULL)
    {
    STDTIME_RET_EC (null_argument);
    }

  *pSystemTime = StdTimeApiZeroSystemTime();

  if (pStdTimeFields == NULL)
    {
    STDTIME_RET_EC (null_argument);
    }

  if STDTIME_IFNOT (ValidStdTimeFields (pStdTimeFields))
    {
    STDTIME_RET_RC;
    }

  pSystemTime->wYear         = (STDTIME_WIN_WORD) pStdTimeFields->year;
  pSystemTime->wMonth        = (STDTIME_WIN_WORD) pStdTimeFields->mon;
  pSystemTime->wDay          = (STDTIME_WIN_WORD) pStdTimeFields->day;
  pSystemTime->wHour         = (STDTIME_WIN_WORD) pStdTimeFields->hour;
  pSystemTime->wMinute       = (STDTIME_WIN_WORD) pStdTimeFields->min;
  pSystemTime->wSecond       = (STDTIME_WIN_WORD) pStdTimeFields->sec;

  nsec = pStdTimeFields->nsec;
  usec = pStdTimeFields->usec;
  msec = pStdTimeFields->msec;

  /* If pointers not NULL, copy values there.	*/
  if (pNsec)
    *pNsec = nsec;
  if (pUsec)
    *pUsec = usec;

  pSystemTime->wMilliseconds = (STDTIME_WIN_WORD) msec;

#if 0	/* OBSOLETE: delete next release	*/
  Rc_StdTime_NormalizeSystemTimeEx (pSystemTime, pNsec, pUsec);
#endif

  /* generate wDayOfWeek value */

  /* a conversion to FILETIME and back to SYSTEM could do this */
  /* but that is a lot of overhead for one field */
  /* the 'yday' value, stored in a struct tm, is not contained in */
  /* a SYSTEMTIME struct, so we discard its value with the NULL parm */

  if STDTIME_IFNOT (StdTimeW32GetDayofWeekAndYear (
       pStdTimeFields->year,
       pStdTimeFields->mon,
       pStdTimeFields->day,
       &wday, NULL))
    {
    STDTIME_RET_EC (failure_in_api_StdTimeW32GetDayofWeekAndYear);
    }

  pSystemTime->wDayOfWeek = (STDTIME_WIN_WORD) wday;

  STDTIME_RET_OK;

  } /* StdTimeFieldsToSystemTimeEx */

/*-***************************************************************************/
/*            SystemTimeExToStdTimeFields                                    */
/*  convert: SYSTEMTIME and extra fields to StdTimeFields structure          */
/*****************************************************************************/

STDTIME_RC                  SystemTimeExToStdTimeFields (
  const STDTIME_WIN_SYSTEMTIME * /*I*/ pSystemTime,
  int32_t                   /*I*/ nUsec,
  int32_t                   /*I*/ nNsec,
  STDTIME_FIELDS *          /*O*/ pStdTimeFields)
  {
  STDTIME_ENUM_FUNC (SystemTimeExToStdTimeFields)

  if (pStdTimeFields == NULL)
    {
    STDTIME_RET_EC (null_argument);
    }

  *pStdTimeFields = ZeroStdTimeFields ();

  if (pSystemTime == NULL)
    {
    STDTIME_RET_EC (null_argument);
    }

  pStdTimeFields->year = pSystemTime->wYear;
  pStdTimeFields->mon  = pSystemTime->wMonth;
  pStdTimeFields->day  = pSystemTime->wDay;

  pStdTimeFields->hour = pSystemTime->wHour;
  pStdTimeFields->min  = pSystemTime->wMinute;
  pStdTimeFields->sec  = pSystemTime->wSecond;

  pStdTimeFields->msec = pSystemTime->wMilliseconds;
  pStdTimeFields->usec = 0;
  pStdTimeFields->nsec = 0;

  STDTIME_RET (StdTimeFields_AddFraction (pStdTimeFields, 0, nUsec, nNsec));

  } /* SystemTimeExToStdTimeFields */
#endif	/* defined(_WIN32)	*/


/*** SystemTime functions ****************************************************/

/*-***************************************************************************/
/*            StdTimeToSystemTime                                            */
/*  convert: StdTime structure to SYSTEMTIME (without extra fields)          */
/*****************************************************************************/

STDTIME_RC                  StdTimeToSystemTime (
  const STDTIME *           /*I*/ pStdTime,
  STDTIME_WIN_SYSTEMTIME *  /*O*/ pSystemTime)
  {
  STDTIME_ENUM_FUNC (StdTimeToSystemTime)

  STDTIME_RET (StdTimeToSystemTimeEx (pStdTime, pSystemTime, NULL, NULL));

  } /* StdTimeToSystemTime */

/*-***************************************************************************/
/*            SystemTimeToStdTime                                            */
/*  convert: SYSTEMTIME (without extra fields) to StdTime structure          */
/*****************************************************************************/

STDTIME_RC                  SystemTimeToStdTime (
  const STDTIME_WIN_SYSTEMTIME * /*I*/ pSystemTime,
  STDTIME *                 /*O*/ pStdTime)
  {
  STDTIME_ENUM_FUNC (SystemTimeToStdTime)

  STDTIME_RET (SystemTimeExToStdTime (pSystemTime, 0, 0, pStdTime));

  } /* SystemTimeToStdTime */


#ifdef _WIN32	/* DbTimeStamp functions only for _WIN32	*/
/*** DbTimeStamp functions ***************************************************/

/*-***************************************************************************/
/*            StdTimeApiZeroDbTimeStamp                                      */
/*  return a zero value of type STDTIME_WIN_DBTIMESTAMP                      */
/*****************************************************************************/

STDTIME_WIN_DBTIMESTAMP     StdTimeApiZeroDbTimeStamp ()
  {
  STDTIME_WIN_DBTIMESTAMP   zero = {0};

  return zero;

  } /* StdTimeApiZeroDbTimeStamp */


/*-***************************************************************************/
/*            StdTimeToDbTimeStamp                                           */
/*  convert: StdTime structure to DBTIMESTAMP                                */
/*****************************************************************************/

STDTIME_RC                  StdTimeToDbTimeStamp (
  const STDTIME *           /*I*/ pStdTime,
  STDTIME_WIN_DBTIMESTAMP * /*O*/ pDbTimeStamp)
  {
  STDTIME_FIELDS            stdTimeFields;

  STDTIME_ENUM_FUNC (StdTimeToDbTimeStamp)

  if (pDbTimeStamp == NULL)
    {
    STDTIME_RET_EC (null_argument);
    }

  *pDbTimeStamp = StdTimeApiZeroDbTimeStamp();

  if STDTIME_IF (StdTimeToStdTimeFields (pStdTime, &stdTimeFields))
    {
    STDTIME_RET (StdTimeFieldsToDbTimeStamp (&stdTimeFields, pDbTimeStamp));
    }

  STDTIME_RET_RC;

  } /* StdTimeToDbTimeStamp */


/*-***************************************************************************/
/*            DbTimeStampToStdTime                                           */
/*  convert: DBTIMESTAMP to StdTime structure                                */
/*****************************************************************************/

STDTIME_RC                  DbTimeStampToStdTime (
  const STDTIME_WIN_DBTIMESTAMP * /*I*/ pDbTimeStamp,
  STDTIME *                      /*O*/ pStdTime)
  {
  STDTIME_FIELDS            stdTimeFields;

  STDTIME_ENUM_FUNC (DbTimeStampToStdTime)

  if (pStdTime == NULL)
    {
    STDTIME_RET_EC (null_argument);
    }

  *pStdTime = ZeroStdTime ();

  if STDTIME_IF (DbTimeStampToStdTimeFields (pDbTimeStamp, &stdTimeFields))
    {
    STDTIME_RET (StdTimeFieldsToStdTime (&stdTimeFields, pStdTime));
    }

  STDTIME_RET_RC;

  } /* DbTimeStampToStdTime */


/*-***************************************************************************/
/*            StdTimeFieldsToDbTimeStamp                                     */
/*  convert: StdTimeFields structure to DBTIMESTAMP                          */
/*****************************************************************************/

STDTIME_RC                  StdTimeFieldsToDbTimeStamp (
  const STDTIME_FIELDS *    /*I*/ pStdTimeFields,
  STDTIME_WIN_DBTIMESTAMP * /*O*/ pDbTimeStamp)
  {
  STDTIME_ENUM_FUNC (StdTimeFieldsToDbTimeStamp)

  if (pDbTimeStamp == NULL)
    {
    STDTIME_RET_EC (null_argument);
    }

  *pDbTimeStamp = StdTimeApiZeroDbTimeStamp();

  if (pStdTimeFields == NULL)
    {
    STDTIME_RET_EC (null_argument);
    }

  if STDTIME_IFNOT (ValidStdTimeFields (pStdTimeFields))
    {
    STDTIME_RET_RC;
    }

  pDbTimeStamp->year   = (STDTIME_WIN_SHORT)  pStdTimeFields->year;
  pDbTimeStamp->month  = (STDTIME_WIN_USHORT) pStdTimeFields->mon;
  pDbTimeStamp->day    = (STDTIME_WIN_USHORT) pStdTimeFields->day;
  pDbTimeStamp->hour   = (STDTIME_WIN_USHORT) pStdTimeFields->hour;
  pDbTimeStamp->minute = (STDTIME_WIN_USHORT) pStdTimeFields->min;
  pDbTimeStamp->second = (STDTIME_WIN_USHORT) pStdTimeFields->sec;

  /* a DBTIMESTAMP fraction is a number of nanoseconds, 0 to 999,999,999 */

  pDbTimeStamp->fraction     = (STDTIME_WIN_ULONG)
    (pStdTimeFields->msec * 1000000) +
    (pStdTimeFields->usec * 1000) +
    (pStdTimeFields->nsec);

  STDTIME_RET_OK;

  } /* StdTimeFieldsToDbTimeStamp */


/*-***************************************************************************/
/*            DbTimeStampToStdTimeFields                                     */
/*  convert: DBTIMESTAMP to StdTimeFields structure                          */
/*****************************************************************************/

STDTIME_RC                  DbTimeStampToStdTimeFields (
  const STDTIME_WIN_DBTIMESTAMP * /*I*/ pDbTimeStamp,
  STDTIME_FIELDS *          /*O*/ pStdTimeFields)
  {
  int32_t                   fraction;

  STDTIME_ENUM_FUNC (DbTimeStampToStdTimeFields)

  if (pStdTimeFields == NULL)
    {
    STDTIME_RET_EC (null_argument);
    }

  *pStdTimeFields = ZeroStdTimeFields ();

  if (pDbTimeStamp == NULL)
    {
    STDTIME_RET_EC (null_argument);
    }

  pStdTimeFields->year = (int32_t) pDbTimeStamp->year;
  pStdTimeFields->mon  = (int32_t) pDbTimeStamp->month;
  pStdTimeFields->day  = (int32_t) pDbTimeStamp->day;

  pStdTimeFields->hour = (int32_t) pDbTimeStamp->hour;
  pStdTimeFields->min  = (int32_t) pDbTimeStamp->minute;
  pStdTimeFields->sec  = (int32_t) pDbTimeStamp->second;

  fraction = (int32_t) pDbTimeStamp->fraction;

  /* limit fraction to valid range */

  if (fraction < 0)
    {
    fraction = 0;
    }

  else if (fraction > 999999999)
    {
    fraction = 999999999;
    }

  pStdTimeFields->msec = (fraction / 1000000) % 1000;
  pStdTimeFields->usec = (fraction / 1000) % 1000;
  pStdTimeFields->nsec = (fraction % 1000);

  STDTIME_RET_OK;

  } /* DbTimeStampToStdTimeFields */
#endif	/* DbTimeStamp functions only for _WIN32	*/


/*** StructTmEx functions ****************************************************/

/*-***************************************************************************/
/*            StdTimeToStructTmEx                                            */
/*  convert: StdTime structure to 'struct tm' and extra fields               */
/*****************************************************************************/

STDTIME_RC                  StdTimeToStructTmEx (
  const STDTIME *           /*I*/ pStdTime,
  struct tm *               /*O*/ pStructTm,
  int32_t *                 /*O*/ pNsec)  /* optional */
  {
  time_t timet;
  int32_t nsec;

  STDTIME_ENUM_FUNC (StdTimeToStructTmEx)

  rc = StdTimeToTimeTypeEx (pStdTime, &timet, &nsec);
  if (rc == STDTIME_OK)
    {
    if (Bool_StdTimeGmTimeR (&timet, pStructTm) != STDTIME_TRUE)
      STDTIME_RET_EC (argument_out_of_range);
    }
  if (pNsec)
    *pNsec = nsec;
  STDTIME_RET_RC;

  } /* StdTimeToStructTmEx */

/*-***************************************************************************/
/*            StructTmExToStdTime                                            */
/*  convert: 'struct tm' and extra fields to StdTime structure               */
/*****************************************************************************/

STDTIME_RC                  StructTmExToStdTime (
  const struct tm *         /*I*/ pStructTm,
  int32_t                   /*I*/ nNsec,
  STDTIME *                 /*O*/ pStdTime)
  {
  time_t timet;

  STDTIME_ENUM_FUNC (StructTmExToStdTime)

  /* usr_mkgmtime doesn't accept const arg so must cast first.	*/
#if !defined(__VMS)
  if ((timet = usr_mkgmtime ((struct tm *) pStructTm)) < 0)
    {
    STDTIME_RET_EC (argument_out_of_range);
    }
#endif

  STDTIME_RET (TimeTypeExToStdTime (timet, nNsec, pStdTime));

  } /* StructTmExToStdTime */


/*** TimeTypeEx functions ************************************************   */

/************************************************************************/
/*			StdTimeToTimeTypeEx				*/
/* convert: StdTime struct to time_t plus nanoseconds			*/
/************************************************************************/
STDTIME_RC                  StdTimeToTimeTypeEx (
  const STDTIME *           /*I*/ pStdTime,
  time_t *                  /*O*/ pTimeT,
  int32_t *                 /*O*/ pNsec)
  {
  QUADLIB_U64               qFileTime;
  QUADLIB_U64               qValue, qOffset;
  QUADLIB_U64               qRemainder;
  int32_t                   nsec;

  STDTIME_ENUM_FUNC (StdTimeToTimeTypeEx)

  /* Check that *pStdTime is not before 1970 (illegal).	*/
  if (pStdTime->dwHighDateTime < FILETIME_1970_HIGH ||
      (pStdTime->dwHighDateTime == FILETIME_1970_HIGH &&
       pStdTime->dwLowDateTime < FILETIME_1970_LOW))
    STDTIME_RET_EC (argument_out_of_range);

  FILETIME_TO_QUADLIB_U64 (pStdTime, &qFileTime);

  /* This just divides by 10000000 to get seconds since 1601	*/
  qValue = QUADLIB_I64_DIV (
    qFileTime,
    QUADLIB_I64_CASTI32 (PVT_STDTIME_SCALE_FILETIME) );

  /* Get remainder.	*/
  qRemainder = QUADLIB_I64_MOD (
    qFileTime,
    QUADLIB_I64_CASTI32 (PVT_STDTIME_SCALE_FILETIME) );

  /* Convert remainder to nanoseconds	*/
  nsec = (QUADLIB_I32_CASTI64 (qRemainder)) * 100;
  if (pNsec != NULL)
    *pNsec = nsec;

  /* Subtract number of seconds from 1601 to 1970.	*/
  QUADLIB_U64_HI(qOffset) = SECONDS_FROM_1601_TO_1970_HIGH;
  QUADLIB_U64_LO(qOffset) = SECONDS_FROM_1601_TO_1970_LOW;
  QUADLIB_I64_SUB_EQ (qValue, qOffset);	/* modifies qValue	*/

  /* Cast to time_t.	*/
#ifdef STDTIME_TIME_T64_ENABLED
  *pTimeT = QUADLIB_I64N_CASTI64(qValue);
#else
  /* DEBUG: should check for qValue out of range before this cast.*/
  *pTimeT = QUADLIB_I32_CASTI64 (qValue);
#endif
  return (STDTIME_OK);
  } /* StdTimeToTimeTypeEx */

/************************************************************************/
/*			TimeTypeExToStdTime				*/
/* convert: time_t plus nanoseconds to StdTime struct			*/
/************************************************************************/
STDTIME_RC                  TimeTypeExToStdTime (
  time_t                    /*I*/ timet,
  int32_t                   /*I*/ nsec,
  STDTIME *                 /*O*/ pStdTime)
  {
  QUADLIB_U64               qValue, qOffset;

  STDTIME_ENUM_FUNC (TimeTypeExToStdTime)

  if (pStdTime == NULL)
    {
    STDTIME_RET_EC (null_argument);
    }

#if !defined(__VMS)
  if (timet < 0)
    {
    STDTIME_RET_EC (argument_out_of_range);
    }
#endif

#ifdef STDTIME_TIME_T64_ENABLED
  QUADLIB_U64_HI(qValue) = QUADLIB_U64_HI(timet);
  QUADLIB_U64_LO(qValue) = QUADLIB_U64_LO(timet);
#else
  QUADLIB_U64_HI(qValue) = 0;
  QUADLIB_U64_LO(qValue) = timet;
#endif

  /* Add number of seconds from 1601 to 1970.	*/
  QUADLIB_U64_HI(qOffset) = SECONDS_FROM_1601_TO_1970_HIGH;
  QUADLIB_U64_LO(qOffset) = SECONDS_FROM_1601_TO_1970_LOW;
  QUADLIB_I64_ADD_EQ (qValue, qOffset);	/* modifies qValue	*/

  /* Multiply seconds by 10000000	*/
  qValue = QUADLIB_I64_MUL (
    qValue,
    QUADLIB_I64_CASTI32 (PVT_STDTIME_SCALE_FILETIME) );

  
  /* Divide nsec by 100 to get num of "100-nanoseccond" intervals & add them.*/
  QUADLIB_U64_HI(qOffset) = 0;
  QUADLIB_U64_LO(qOffset) = nsec / 100;
  QUADLIB_I64_ADD_EQ (qValue, qOffset);	/* modifies qValue	*/

  QUADLIB_I64_TO_FILETIME (&qValue, pStdTime);
  return (STDTIME_OK);
  } /* TimeTypeExToStdTime */


/*** Constructor-like functions **********************************************/

/*-***************************************************************************/
/*            StdTimeFields                                                  */
/*  construct a STDTIME_FIELDS structure from parameters                     */
/*****************************************************************************/


STDTIME_FIELDS              StdTimeFields (
  int32_t                   /*I*/ year,
  int32_t                   /*I*/ mon,
  int32_t                   /*I*/ day,
  int32_t                   /*I*/ hour,
  int32_t                   /*I*/ min,
  int32_t                   /*I*/ sec,
  int32_t                   /*I*/ msec,
  int32_t                   /*I*/ usec,
  int32_t                   /*I*/ nsec)
  {
  STDTIME_FIELDS            x;

  x.year = year;
  x.mon  = mon;
  x.day  = day;
  x.hour = hour;
  x.min  = min;
  x.sec  = sec;
  x.msec = msec;
  x.usec = usec;
  x.nsec = nsec;

  return x;

  } /* StdTimeFields */


/*-***************************************************************************/
/*            StdTimeDelta                                                   */
/*  construct a STDTIME_DELTA structure from parameters                      */
/*****************************************************************************/


STDTIME_DELTA               StdTimeDelta (
  int32_t                   /*I*/ day,
  int32_t                   /*I*/ hour,
  int32_t                   /*I*/ min,
  int32_t                   /*I*/ sec,
  int32_t                   /*I*/ msec,
  int32_t                   /*I*/ usec,
  int32_t                   /*I*/ nsec)
  {
  STDTIME_DELTA             x;

  x.day  = day;
  x.hour = hour;
  x.min  = min;
  x.sec  = sec;
  x.msec = msec;
  x.usec = usec;
  x.nsec = nsec;

  return x;

  } /* StdTimeDelta */


/*-***************************************************************************/
/*            StdTimeSpan                                                    */
/*  construct a STDTIME structure from parameters                            */
/*****************************************************************************/


STDTIME_SPAN                StdTimeSpan (
  QUADLIB_I64               /*I*/ value)
  {
  STDTIME_SPAN              x;

  x.value = value;

  return x;

  } /* StdTimeSpan */


/*** Initializer functions ***************************************************/


/*-***************************************************************************/
/*            ZeroStdTime                                                    */
/*  return a zero value of type STDTIME                                      */
/*****************************************************************************/

STDTIME                     ZeroStdTime ()
  {
  STDTIME                   zero = {0};

  return zero;

  } /* ZeroStdTime */


/*-***************************************************************************/
/*            ZeroStdTimeFields                                              */
/*  return a zero value of type STDTIME_FIELDS                               */
/*****************************************************************************/

STDTIME_FIELDS              ZeroStdTimeFields ()
  {
  STDTIME_FIELDS            zero = {0};

  return zero;

  } /* ZeroStdTimeFields */


/*-***************************************************************************/
/*            ZeroStdTimeStringA                                             */
/*  return a zero value of type STDTIME_STRINGA                              */
/*****************************************************************************/

STDTIME_STRINGA             ZeroStdTimeStringA ()
  {
  STDTIME_STRINGA           zero = {{0}};

  return zero;

  } /* ZeroStdTimeStringA */


/*-***************************************************************************/
/*            ZeroStdTimeDelta                                               */
/*  return a zero value of type STDTIME_DELTA                                */
/*****************************************************************************/

STDTIME_DELTA               ZeroStdTimeDelta ()
  {
  STDTIME_DELTA             zero = {0};

  return zero;

  } /* ZeroStdTimeDelta */


/*-***************************************************************************/
/*            ZeroStdTimeSpan                                                */
/*  return a zero value of type STDTIME_SPAN                                 */
/*****************************************************************************/

STDTIME_SPAN                ZeroStdTimeSpan ()
  {
  STDTIME_SPAN              zero = {0};

  return zero;

  } /* ZeroStdTimeSpan */


/*-***************************************************************************/
/*            ZeroStdTimeTzoA                                                */
/*  return a zero value of type STDTIME_TZOA                                 */
/*****************************************************************************/

STDTIME_TZOA                ZeroStdTimeTzoA ()
  {
  STDTIME_TZOA              zero = {{0}};

  return zero;

  } /* ZeroStdTimeTzoA */


/*-***************************************************************************/
/*            ZeroStdTimeErrMsgA                                             */
/*  return a zero value of type STDTIME_ERRMSGA                              */
/*****************************************************************************/

STDTIME_ERRMSGA             ZeroStdTimeErrMsgA ()
  {
  STDTIME_ERRMSGA           zero = {{0}};

  return zero;

  } /* ZeroStdTimeErrMsgA */


/*-***************************************************************************/
/*            ZeroStdTimeErrMsgW                                             */


/*** Initializer functions for 'outside' types *******************************/
/*** names prefixed with 'StdTimeApi' because they are non-STDTIME structs ***/

/*-***************************************************************************/
/*            StdTimeApiZeroFileTime                                         */
/*  return a zero value of type STDTIME_WIN_FILETIME                         */
/*****************************************************************************/

STDTIME_WIN_FILETIME        StdTimeApiZeroFileTime ()
  {
  STDTIME_WIN_FILETIME      zero = {0};

  return zero;

  } /* StdTimeApiZeroFileTime */


/*-***************************************************************************/
/*            StdTimeApiZeroSystemTime                                       */
/*  return a zero value of type STDTIME_WIN_SYSTEMTIME                       */
/*****************************************************************************/

STDTIME_WIN_SYSTEMTIME      StdTimeApiZeroSystemTime ()
  {
  STDTIME_WIN_SYSTEMTIME    zero = {0};

  return zero;

  } /* StdTimeApiZeroSystemTime */


/*-***************************************************************************/
/*            StdTimeApiZeroStructTm                                         */
/*  return a zero value of type struct tm                                    */
/*****************************************************************************/

struct tm                   StdTimeApiZeroStructTm ()
  {
  struct tm                 zero = {0};

  return zero;

  } /* StdTimeApiZeroStructTm */


/*** calandar function interface *********************************************/


/*-***************************************************************************/
/*  OBTAIN DAY OF WEEK AND DAY OF WEEK                                       */
/*  This provides an interface to the W32 layer, which we do not want to     */
/*  expose to the user.                                                      */
/*****************************************************************************/

STDTIME_RC                  StdTimeGetDayofWeekAndYear (
  int32_t                   /*I*/ nYear,
  int32_t                   /*I*/ nMon,
  int32_t                   /*I*/ nDayofMon,
  int32_t *                 /*O*/ pDayofWeek,
  int32_t *                 /*O*/ pDayofYear)
  {
  STDTIME_ENUM_FUNC (StdTimeGetDayofWeekAndYear)

  STDTIME_RET (StdTimeW32GetDayofWeekAndYear (
    nYear, nMon, nDayofMon, pDayofWeek, pDayofYear));
  }


#if defined(_WIN32)
/*** Date functions (COM DATE) ***********************************************/

/************************************************************************/
/*			FileTimeToDate					*/
/* RETURNS: 0=SUCCESS, 1=FAILURE					*/
/************************************************************************/
static int FileTimeToDate(const FILETIME *pft, DATE *pdate)
  {
  /* Don't allow FILETIME before 1899 (causes negative DATE).	*/
  if (*(__int64 *)pft < 0x014f35a9a90cc000)
    {
    *pdate = 0.0;	/* set to bad value in case caller ignores return*/
    return (1);	/* error	*/
    }
  *pdate = (double)((double)(*(__int64 *)pft) / 8.64e11) - (double)(363 + (1899 - 1601) * 365 + (24 + 24 + 24));
  assert (*pdate >= 0.0);
  return (0);	/* success	*/
  }
/************************************************************************/
/*			DateToFileTime					*/
/* RETURNS: 0=SUCCESS, 1=FAILURE					*/
/************************************************************************/
static int DateToFileTime(const DATE *pdate, FILETIME *pft)
  {
  __int64 temp;
  if (*pdate < 0.0)	/* negative date too complicated to deal with	*/
    {
    temp = 0;
    *pft = *(FILETIME *)&temp;	/* set to bad value in case caller ignores return*/
    return (1);	/* error	*/
    }
  /* NOTE: this casts double to _int64 (some precision lost).	*/
  temp = (__int64)((*pdate + (double)(363 + (1899 - 1601) * 365 + (24 + 24 + 24))) * 8.64e11);
  *pft = *(FILETIME *)&temp;
  return (0);	/* success	*/
  }

/*-***************************************************************************/
/*            StdTimeToDate                                                  */
/*  convert: StdTime structure to DATE time value                            */
/*****************************************************************************/

STDTIME_RC                  StdTimeToDate (
  const STDTIME *           /*I*/ pStdTime,
  STDTIME_WIN_DATE *        /*O*/ pDate)
  {
  STDTIME_ENUM_FUNC (StdTimeToDate)

  /* STDTIME same as FILETIME, so this works.	*/
  if (FileTimeToDate (pStdTime, pDate))
    STDTIME_RET_EC (argument_out_of_range);
  STDTIME_RET_OK;
  } /* StdTimeToDate */

/*-***************************************************************************/
/*            DateToStdTime                                                  */
/*  convert: DATE time value to StdTime structure                            */
/*****************************************************************************/

STDTIME_RC                  DateToStdTime (
  const STDTIME_WIN_DATE *  /*I*/ pDate,
  STDTIME *                 /*O*/ pStdTime)
  {
  STDTIME_ENUM_FUNC (DateToStdTime)

  /* STDTIME same as FILETIME, so this works.	*/
  if (DateToFileTime (pDate, pStdTime))
    STDTIME_RET_EC (argument_out_of_range);
  STDTIME_RET_OK;
  } /* DateToStdTime */

/*-***************************************************************************/
/*            StdTimeFieldsToDate                                            */
/*  convert: StdTime structure to DATE time value                            */
/*****************************************************************************/

STDTIME_RC                  StdTimeFieldsToDate (
  const STDTIME_FIELDS *    /*I*/ pStdTimeFields,
  STDTIME_WIN_DATE *        /*O*/ pDate)
  {
  STDTIME StdTime;

  STDTIME_ENUM_FUNC (StdTimeFieldsToDate)

  rc = StdTimeFieldsToStdTime (pStdTimeFields, &StdTime);
  if (rc == STDTIME_OK)
    rc = StdTimeToDate (&StdTime, pDate);
  return (rc);
  } /* StdTimeFieldsToDate */

/*-***************************************************************************/
/*            DateToStdTimeFields                                            */
/*  convert: DATE time value to StdTimeFields structure                      */
/*****************************************************************************/

STDTIME_RC                  DateToStdTimeFields (
  const STDTIME_WIN_DATE *  /*I*/ pDate,
  STDTIME_FIELDS *          /*O*/ pStdTimeFields)
  {
  STDTIME StdTime;

  STDTIME_ENUM_FUNC (DateToStdTimeFields)

  rc = DateToStdTime (pDate, &StdTime);
  if (rc == STDTIME_OK)
    rc = StdTimeToStdTimeFields (&StdTime, pStdTimeFields);
  return (rc);
  } /* DateToStdTimeFields */

#endif	/* defined(_WIN32)	*/

/*-***************************************************************************/
/*            StdTimeToLocalStdTime                                          */
/*  convert: StdTime structure from UTC/GMT time to local time               */
/*  if not using TZDB, use local host to speed up conversion                 */
/*****************************************************************************/

STDTIME_RC                  StdTimeToLocalStdTime (
  const STDTIME *           /*I*/ pStdTime,
  STDTIME *                 /*O*/ pStdTimeLocal)
  {
  STDTIME_ENUM_FUNC (StdTimeToLocalStdTime)
    {
    STDTIME_WIN_FILETIME    gmtFileTime;
    STDTIME_WIN_FILETIME    locFileTime;

    if STDTIME_IFNOT (StdTimeToFileTime (pStdTime, &gmtFileTime))
      {
      STDTIME_RET_RC;
      }

    if (! STDTIME_API_FILETIMETOLOCALFILETIME (&gmtFileTime, &locFileTime))
      {
      STDTIME_RET_EC (failure_in_api_FileTimeToLocalFileTime);
      }

    STDTIME_RET (FileTimeToStdTime (&locFileTime, pStdTimeLocal));
    }
  } /* StdTimeToLocalStdTime */


/*-***************************************************************************/
/*            LocalStdTimeToStdTime                                          */
/*  convert: StdTime structure from local time to UTC/GMT time               */
/*  if not using TZDB, use local host to speed up conversion                 */
/*****************************************************************************/

STDTIME_RC                  LocalStdTimeToStdTime (
  const STDTIME *           /*I*/ pStdTimeLocal,
  STDTIME *                 /*O*/ pStdTime)
  {
  STDTIME_ENUM_FUNC (LocalStdTimeToStdTime)

    {
    STDTIME_WIN_FILETIME    gmtFileTime;
    STDTIME_WIN_FILETIME    locFileTime;

    if STDTIME_IFNOT (StdTimeToFileTime (pStdTimeLocal, &locFileTime))
      {
      STDTIME_RET_RC;
      }

    if (! STDTIME_API_LOCALFILETIMETOFILETIME (&locFileTime, &gmtFileTime))
      {
      STDTIME_RET_EC (failure_in_api_LocalFileTimeToFileTime);
      }

    STDTIME_RET (FileTimeToStdTime (&gmtFileTime, pStdTime));
    }
  } /* LocalStdTimeToStdTime */


/*** timezone-offset support functions ***************************************/


/*-***************************************************************************/
/*            NormalizeStdTimeTzoA                                           */
/*  if timezone offset string is in a valid format, normalize it.            */
/*  normalization involves the following:                                    */
/*                                                                           */
/*  1.  a 'z' code is converted to 'Z'                                       */
/*  2.  if the ss field is zero, it is dropped                               */
/*  3.  if all numeric fields are zero, the offset string becomes Z          */
/*  4.  trailing blanks are removed, and replaced by nuls                    */
/*                                                                           */
/*****************************************************************************/


STDTIME_RC                  NormalizeStdTimeTzoA (
  STDTIME_TZOA *            /*IO*/ pTzo)
  {
  STDTIME_TZOA              tzoA = {{0}};
  int32_t                   hour;
  int32_t                   min;
  int32_t                   sec;

  STDTIME_ENUM_FUNC (NormalizeStdTimeTzoA)

  if (pTzo == NULL)
    {
    STDTIME_RET_EC (null_argument);
    }

  /* limit input tzo string to declared length, jic */

  pTzo->str[STDTIME_TZO_LEN] = 0;

  /* see if timezone offset is omitted */

  if (Bool_StdTime_IsBlankA (pTzo->str))
    {
    *pTzo = tzoA;           /* clear out entire TZO field */
    STDTIME_RET_OK;
    }

  if ((pTzo->str[0] == 'z') || (pTzo->str[0] == 'Z'))
    {
    if (Bool_StdTime_IsBlankA (pTzo->str+1))
      {
      /* normalize Z and pad field with nuls */
      tzoA.str[0] = 'Z';
      *pTzo = tzoA;
      STDTIME_RET_OK;
      }
    }

  if ((pTzo->str[0] != '+') && (pTzo->str[0] != '-'))
    {
    STDTIME_RET_EC (invalid_timezone_offset);
    }

  if ( (! isdigit (pTzo->str[1]))
  ||   (! isdigit (pTzo->str[2]))
  ||   (           pTzo->str[3] != ':')
  ||   (! isdigit (pTzo->str[4]))
  ||   (! isdigit (pTzo->str[5])) )
    {
    STDTIME_RET_EC (invalid_timezone_offset);
    }

  hour = atoi (pTzo->str+1);
  min  = atoi (pTzo->str+4);

  if (! STDTIME_IN_RANGE (hour, 0, 23))
    {
    STDTIME_RET_EC (invalid_hour);
    }

  if (! STDTIME_IN_RANGE (min, 0, 59))
    {
    STDTIME_RET_EC (invalid_min);
    }

  /* determine whether we have short form or long form */
  /* long form: validate 'ss' digits */

  if (pTzo->str[6] == ':')
    {
    if ( (! isdigit (pTzo->str[7]))
    ||   (! isdigit (pTzo->str[8])) )
      {
      STDTIME_RET_EC (invalid_timezone_offset);
      }

    sec = atoi (pTzo->str+7);

    if (! STDTIME_IN_RANGE (sec, 0, 59))
      {
      STDTIME_RET_EC (invalid_sec);
      }

    if (! Bool_StdTime_IsBlankA (pTzo->str+9))
      {
      STDTIME_RET_EC (invalid_timezone_offset);
      }
    }

  /* short form: ensure we are at end of string */

  else
    {
    if (! Bool_StdTime_IsBlankA (pTzo->str+6))
      {
      STDTIME_RET_EC (invalid_timezone_offset);
      }

    sec = 0;
    }

  /* now, normalize the format */

  if ((hour == 0) && (min == 0) && (sec == 0))
    {
    /* if all fields zero, it is GMT time */
    /* normalize Z and pad field with nuls */
    tzoA.str[0] = 'Z';
    *pTzo = tzoA;
    STDTIME_RET_OK;
    }

  if (sec == 0)
    {
    /* +hh:mm or -hh:mm format */
    sprintf (tzoA.str, "%c%02d:%02d", pTzo->str[0], hour, min);
    }

  else
    {
    /* +hh:mm:ss or -hh:mm:ss format */
    sprintf (tzoA.str, "%c%02d:%02d:%02d", pTzo->str[0], hour, min, sec);
    }

  *pTzo = tzoA;           /* copy back reformatted tzoA */
  STDTIME_RET_OK;

  } /* NormalizeStdTimeTzoA */


/*** error message decoding functions ****************************************/


/*-***************************************************************************/
/*            StdTimeRcToErrMsgA                                             */
/*  interpret STDTIME_RC value and convert to a STDTIME_ERRMSGA structure.   */
/*****************************************************************************/

STDTIME_RC                  StdTimeRcToErrMsgA (
  const STDTIME_RC          /*I*/ stdTimeRc,
  STDTIME_ERRMSGA *         /*O*/ pErrMsg)
  {
  STDTIME_ERRMSGA           errMsg = {{0}};
  int32_t                   hilevel;
  int32_t                   lolevel;
  int32_t                   reason;
  int32_t                   i;

  STDTIME_ENUM_FUNC (StdTimeRcToErrMsgA)

  if (pErrMsg == NULL)
    {
    STDTIME_RET_EC (null_argument);
    }

  *pErrMsg = ZeroStdTimeErrMsgA ();

  /* a STDTIME_RC has the following bit layout */
  /* bits 31-30(2)  unused                     */
  /* bits 29-20(10) hi-level function enum     */
  /* bits 19-10(10) lo-level function enum     */
  /* bits 09-00(10) error reason code          */

  hilevel = (stdTimeRc >> 20) & 0x3FF;
  lolevel = (stdTimeRc >> 10) & 0x3FF;
  reason  =  stdTimeRc        & 0x3FF;

  /* an error code has the form   0xNNNNNNNN = HHHH.LLLL.RRRR#      */
  /*                              0123456789012345678901234567      */
  /* this holds a raw hex RC value, and the RC as decimal subfields */

  sprintf (errMsg.errcode, "0x%08X = %04d.%04d.%04d",
    stdTimeRc, hilevel, lolevel, reason);

  if ((stdTimeRc & 0xC0000000) != 0)
    {
    strncpy (errMsg.hilevel, "*** invalid STDTIME_RC value",
      STDTIME_ERRMSG_LEN);

    *pErrMsg = errMsg;          /* pass back result */
    STDTIME_RET_EC (invalid_stdtime_rc_value);
    }

  /* the first entry is at STDTIME_FUNC__0000 with an enum value of 0 */
  /* enum 0 is "valid" but 0 is not used for error return codes, because */
  /* a STDTIME_RC of 0 means there is no error, so 0 is skipped. */
  /* example: suppose there were 10 entries; they would have enum's 1 to 10 */
  /* the last entry is at STDTIME_FUNC__SIZE with (example) enum value of 11 */

  if (hilevel < (STDTIME_RC) STDTIME_FUNC__SIZE)
    {
    strncpy (errMsg.hilevel, enum_stdtime_func_text[hilevel],
      STDTIME_ERRMSG_LEN);
    }

  else
    {
    strncpy (errMsg.hilevel, "*** undefined hilevel function",
      STDTIME_ERRMSG_LEN);
    }

  if (lolevel < (STDTIME_RC) STDTIME_FUNC__SIZE)
    {
    strncpy (errMsg.lolevel, enum_stdtime_func_text[lolevel],
      STDTIME_ERRMSG_LEN);
    }

  else
    {
    strncpy (errMsg.lolevel, "*** undefined lolevel function",
      STDTIME_ERRMSG_LEN);
    }

  if (reason < (STDTIME_RC) STDTIME_ERR__SIZE)
    {
    strncpy (errMsg.reason, enum_stdtime_err_text[reason],
      STDTIME_ERRMSG_LEN);
    }

  else
    {
    strncpy (errMsg.reason, "*** undefined reason code",
      STDTIME_ERRMSG_LEN);
    }

  errMsg.hilevel[STDTIME_ERRMSG_LEN] = 0;
  errMsg.lolevel[STDTIME_ERRMSG_LEN] = 0;
  errMsg.reason [STDTIME_ERRMSG_LEN] = 0;

  /* remove underscores from reason message */
  /* the underscores were necessary for the macro to form an enum name */
  /* but we don't want them literally present in the message */

  for (i=0; errMsg.reason[i]; i++)
    {
    if (errMsg.reason[i] == '_')
      {
      errMsg.reason[i] = ' ';
      }
    }

 *pErrMsg = errMsg;         /* pass back result */

  STDTIME_RET_RC;

  } /* StdTimeRcToErrMsgA */


/*** END - STDTIME.C *********************************************************/


