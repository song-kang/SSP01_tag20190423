/*****************************************************************************/
/* SISCO SOFTWARE MODULE HEADER **********************************************/
/*****************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,            */
/*            2005 All Rights Reserved                                       */
/*                                                                           */
/* MODULE NAME : stdtime.h                                                   */
/* PRODUCT(S)  : Standard Time Management Library                            */
/*                                                                           */
/* MODULE DESCRIPTION:                                                       */
/*   Header file for Standard Time Management Library Functions              */
/*                                                                           */
/*   Note:  most functions a produce a return-code value STDTIME_RC.         */
/*   the return code is zero if successful, otherwise it is a structured     */
/*   value in which the low-order 10 bits enumerates an error reason,        */
/*   and the upper 20 bits enumerates the high-level and low-level           */
/*   functions where the error was detected.                                 */
/*                                                                           */
/* MODIFICATION LOG:                                                         */
/*   Date    Who  Rev  Comments                                              */
/* --------  ---  ---  ----------------------------------------------------- */
/* 08/06/10  DSF       Made previous change Windows-specific		     */
/* 07/20/10  DSF       Include tchar.h					     */
/* 04/08/08  JRB   15  Chg () to (ST_VOID) in several prototypes.            */
/* 03/13/08  JRB   14  Chg #error to #warning for STDTIME_UNICODE.           */
/* 02/28/08  JRB   13  Incl "timeb.h" only in "sysincs.h".                   */
/* 02/20/08  JRB   12  Chg STDTIME same as FILETIME (simplifies a lot).      */
/*		       Delete MANY unused functions and definitions.         */
/*		       Simplify TimeTypeEx, StrucTmEx args.                  */
/* 04/03/07  RLH   11  Additional conversion functions to/from FILETIME.     */
/* 03/18/07  RLH   10  Remove tabs from source code inadvertantly added.     */
/* 03/16/07  RLH   09  Normalize struct tm, SYSTEMTIME, and STDTIME_FIELDS   */
/*                     after rounding is applied; no rounding for formatting */
/*                     functions.                                            */
/* 02/16/07  RLH   08  Clean up minor warnings under Linux                   */
/* 01/10/07  RLH   07  Add TruncStdTimeString functions                      */
/* 11/29/06  RLH   06  Correction to STDTIME_TIME_T64_ENABLED setting        */
/* 10/18/06  RLH   05  Enhanced capabilities for timezone rules              */
/* 10/27/06  RLH   04  Change STDTIME_RET, STDTIME_IF, STDTIME_IFNOT macros  */
/* 09/13/06  RLH   03  Ensure local time conversions work when TZ not set    */
/* 06/07/06  RLH   02  Numerous features added, see stdtime.doc              */
/* 01/18/06  RLH   01  Created                                               */
/*****************************************************************************/

#ifndef _STDTIME_H_7583ECB2_DB3F_48B3_8A97_2D1D2A38C84A_
#define _STDTIME_H_7583ECB2_DB3F_48B3_8A97_2D1D2A38C84A_

#define SECONDS_FROM_1601_TO_1970_HIGH	2
#define SECONDS_FROM_1601_TO_1970_LOW	0xB6109100

#define FILETIME_1970_HIGH	0x019DB1DE
#define FILETIME_1970_LOW	0xD53E8000

#undef STDTIME_MSVC6
#undef STDTIME_MSVC7
#undef STDTIME_MSVC8

#ifdef _MSC_VER

#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE
#endif


#if   (_MSC_VER >= 1400)
#define STDTIME_MSVC8
#elif (_MSC_VER >= 1300)
#define STDTIME_MSVC7
#elif (_MSC_VER >= 1200)
#define STDTIME_MSVC6
#endif


#if 0

#if   (_MSC_VER >= 1400)
#pragma comment(lib, "stdtimelib.lib")
#elif (_MSC_VER >= 1300)
#pragma comment(lib, "stdtimelib.lib")
#elif (_MSC_VER >= 1200)
#pragma comment(lib, "stdtimelib.lib")
#endif

#endif /* 0 */


#endif /* _MSC_VER */

/* to test for STDTIME library being used */
#undef  STDTIME_DEFINED
#define STDTIME_DEFINED     1

/*****************************************************************************/
/*  LIBRARY HEADERS                                                          */
/*****************************************************************************/

/* UNICODE or ANSI, but not both */

#if defined(STDTIME_UNICODE) && defined(STDTIME_ANSI)
#error Both STDTIME_UNICODE and STDTIME_ANSI defined
#undef STDTIME_UNICODE
#endif

/* UNICODE or _UNICODE implies STDTIME_UNICODE, but not if ANSI defined */

#if (!defined(STDTIME_UNICODE)) && (!defined(STDTIME_ANSI))
#if defined(UNICODE) || defined(_UNICODE)
#define STDTIME_UNICODE
#endif
#endif

#if defined(STDTIME_UNICODE)
#pragma message("Warning: STDTIME_UNICODE option no longer supported. Try using ANSI functions mbstowcs and wcstombs.")
#undef STDTIME_UNICODE
#endif

/* DEBUG: system include should come from "sysincs.h". Fix that later	*/
#ifdef _WIN32
#include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#ifdef _WIN32
#include <tchar.h>
#endif
#include <sys/types.h>

/*****************************************************************************/
/* <time.t> with support for gmtime_r and localtime_r if possible            */
/*****************************************************************************/

#undef  STDTIME_REENTRANT_GMTIME_LOCALTIME

#undef  STDTIME_DEFINED_LINUX__USE_POSIX
#if defined(linux)
#define STDTIME_REENTRANT_GMTIME_LOCALTIME
#ifndef                      __USE_POSIX
#define                      __USE_POSIX
#define STDTIME_DEFINED_LINUX__USE_POSIX
#endif
#endif

#undef  STDTIME_DEFINED_SOLARIS_REENTRANT
#if defined(sun)
#define STDTIME_REENTRANT_GMTIME_LOCALTIME
#ifndef                        _REENTRANT
#define                        _REENTRANT
#define STDTIME_DEFINED_SOLARIS_REENTRANT
#endif
#endif

#undef  STDTIME_DEFINED_SOLARIS_POSIX_PTHREAD_SEMANTICS
#if defined(sun)
#ifndef                        _POSIX_PTHREAD_SEMANTICS
#define                        _POSIX_PTHREAD_SEMANTICS
#define STDTIME_DEFINED_SOLARIS_POSIX_PTHREAD_SEMANTICS
#endif
#endif

#undef  STDTIME_DEFINED_AIX_THREAD_SAFE
#if defined(_AIX)
#define STDTIME_REENTRANT_GMTIME_LOCALTIME
#ifndef                    _THREAD_SAFE
#define                    _THREAD_SAFE
#define STDTIME_DEFINED_AIX_THREAD_SAFE
#endif
#endif

#undef  STDTIME_DEFINED_DEC_UNIX_REENTRANT
#if defined(__alpha) && !defined(__VMS)
#define STDTIME_REENTRANT_GMTIME_LOCALTIME
#ifndef                         _REENTRANT
#define                         _REENTRANT
#define STDTIME_DEFINED_DEC_UNIX_REENTRANT
#endif
#endif


#include <time.h>


#if defined(sun)
struct tm * gmtime_r    (const time_t * pTimeT, struct tm * pStructTm);
struct tm * localtime_r (const time_t * pTimeT, struct tm * pStructTm);
#endif


/* clean up macro namespace */

#ifdef  STDTIME_DEFINED_LINUX__USE_POSIX
#undef  STDTIME_DEFINED_LINUX__USE_POSIX
#undef                       __USE_POSIX
#endif

#ifdef  STDTIME_DEFINED_SOLARIS_REENTRANT
#undef  STDTIME_DEFINED_SOLARIS_REENTRANT
#undef                         _REENTRANT
#endif

#ifdef  STDTIME_DEFINED_SOLARIS_POSIX_PTHREAD_SEMANTICS
#undef  STDTIME_DEFINED_SOLARIS_POSIX_PTHREAD_SEMANTICS
#undef                         _POSIX_PTHREAD_SEMANTICS
#endif

#ifdef  STDTIME_DEFINED_DEC_UNIX_REENTRANT
#undef  STDTIME_DEFINED_DEC_UNIX_REENTRANT
#undef                          _REENTRANT
#endif

#ifdef  STDTIME_DEFINED_AIX_THREAD_SAFE
#undef  STDTIME_DEFINED_AIX_THREAD_SAFE
#undef                     _THREAD_SAFE
#endif


/*****************************************************************************/
/*  support _T notation on non-Windows platforms                             */
/*****************************************************************************/

#if (!defined(_T)) && (!defined(__T))
#ifdef STDTIME_UNICODE
#define __T(x) L##x
#define _T(x)  __T(x)
#else
#define __T(x) x
#define _T(x)  x
#endif
#endif /* not defined _T and __T */

#include "stdtime_config.h"


/*****************************************************************************/
/*  determine if "stdtime_timezone_csv.h" file is "merged" or not.           */
/*                                                                           */
/*  the csv file gets "merged" if there is no comma separating one line      */
/*  from the next.  a "merged" file is the normal format for a csv data      */
/*  file, but when used as part of a C program, the lack of a comma causes   */
/*  the last string value on one line to get concatenated by the compiler    */
/*  to the first string value on the next line.  we need to know if the      */
/*  file is "merged" or "unmerged" to know how to 'undo' the merging.        */
/*                                                                           */
/*  an unmerged file is easier and faster to process, while a merged file    */
/*  maintains source compatibility with the original Boost.org timezone      */
/*  database file from which it originated.                                  */
/*                                                                           */
/*****************************************************************************/

#if defined(STDTIME_MERGED_CSV) && defined(STDTIME_UNMERGED_CSV)
#error Both STDTIME_MERGED_CSV and STDTIME_UNMERGED_CSV defined
#undef STDTIME_UNMERGED_CSV
#endif

#if !defined(STDTIME_MERGED_CSV) && !defined(STDTIME_UNMERGED_CSV)
#define STDTIME_MERGED_CSV /* default */
#endif


/*****************************************************************************/
/*  determine how GMT is converted to local time, and vice-versa.            */
/*                                                                           */
/*  using a system API for a definition of what "local" is will be faster,   */
/*  but if the SYSTEMTIMEZONE or TZ environment variable does not agree      */
/*  with the OS's understanding of what "local" is, there will be a          */
/*  conflict.  if the time library is likely to be used for a non-local      */
/*  time zone, defining STDTIME_LOCAL_USES_TZDB is necessary.                */
/*  otherwise, define STDTIME_LOCAL_USES_SYSAPI for speed.                   */
/*                                                                           */
/*****************************************************************************/

#if defined(STDTIME_LOCAL_USES_TZDB) && defined(STDTIME_LOCAL_USES_SYSAPI)
#error Both STDTIME_LOCAL_USES_TZDB and STDTIME_LOCAL_USES_SYSAPI defined
#undef STDTIME_LOCAL_USES_SYSAPI
#endif

#if !defined(STDTIME_LOCAL_USES_TZDB) && !defined(STDTIME_LOCAL_USES_SYSAPI)
#define STDTIME_LOCAL_USES_SYSAPI  /* default */
#endif


/*****************************************************************************/
/*  C++ ONLY ENABLED OR DISABLED                                             */
/*                                                                           */
/*  use StdTimeConfig.h to set STDTIME_CPP_ONLY_ENABLED/DISABLED             */
/*****************************************************************************/


#ifdef __cplusplus

#if defined(STDTIME_CPP_ONLY_ENABLED) && defined(STDTIME_CPP_ONLY_DISABLED)
#error Both STDTIME_CPP_ONLY_ENABLED and STDTIME_CPP_ONLY_DISABLED defined
#undef STDTIME_CPP_ONLY_ENABLED
#endif

#if !defined(STDTIME_CPP_ONLY_ENABLED) && !defined(STDTIME_CPP_ONLY_DISABLED)
#define STDTIME_CPP_ONLY_ENABLED  /* default */
#endif

#else /* not __cplusplus */

#undef STDTIME_CPP_ONLY_ENABLED
#undef STDTIME_CPP_ONLY_DISABLED
#define STDTIME_CPP_ONLY_DISABLED

#endif /* __cplusplus */


/* StdTimeW32.h needs the bool definition */


#define STDTIME_BOOL        int
#define STDTIME_TRUE        1
#define STDTIME_FALSE       0

typedef int32_t             STDTIME_RC;

/* STDTIME return codes are divided as follows: */
/*  2 bits unused */
/* 10 bits = highest level function where error detected */
/* 10 bits =  lowest level function where error detected */
/* 10 bits = error reason code (nature of problem) */


/* create error code enumeration from enum_stdtime_func.h */
/* this is done by temporary macro and include file */

#define STDTIME_ENUM_FUNC(x)  STDTIME_FUNC__##x,

enum
  {
  STDTIME_FUNC__0000,       /* dummy 0 entry */
#include "stdtime_enum_func.h"
  STDTIME_FUNC__SIZE
  };

/* temporary macro has served its purpose, so undefine it */
#undef  STDTIME_ENUM_FUNC


/* now redefine macro for main purpose, to declare 'rc' and 'func' */
/* within each function that returns a STDTIME_RC return code. */


#define STDTIME_ENUM_FUNC(x)                                                  \
  STDTIME_RC rc;                                                              \
  STDTIME_RC func;                                                            \
  rc = -1;                                                                    \
  func = (STDTIME_RC) STDTIME_FUNC__##x;


/* create error code enumeration from enum_stdtime_err.h */
/* this is done by temporary macro and include file */

#define STDTIME_ENUM_ERR(x)   STDTIME_ERR__##x,

enum
  {
  STDTIME_ERR__0000,        /* dummy 0 entry */
#include "stdtime_enum_err.h"
  STDTIME_ERR__SIZE
  };

/* temporary macro has served its purpose, so undefine it */
#undef  STDTIME_ENUM_ERR


#define STDTIME_10_BITS     0x3FF
#define STDTIME_20_BITS     0xFFFFF

#define STDTIME_OK          0
#define STDTIME_RET(x)      return StdTimeRet (func, x)

#define STDTIME_RET_RC      STDTIME_RET(rc)
#define STDTIME_RET_OK      return STDTIME_OK

#define STDTIME_EC(x)       ((func << 10) | (STDTIME_ERR__##x))

#define STDTIME_RET_EC(x)   STDTIME_RET(STDTIME_EC(x))

#include "stdtime_quadlib.h"
#include "stdtime_w32.h"


#ifdef  __cplusplus
extern "C" {
#endif


/*****************************************************************************/
/*  TYPES AND STRUCTURES                                                     */
/*****************************************************************************/


#if defined(_MSC_VER) && defined(_WCHAR_T_DEFINED)
typedef wchar_t             STDTIME_WCHAR;
#elif defined(sun) && defined(_WCHAR_T)
typedef wchar_t             STDTIME_WCHAR;
#else
typedef unsigned short      STDTIME_WCHAR;
#endif

#ifndef STDTIME_TIME_T64_WIN
#if defined(_TIME_T_DEFINED) && defined(_WIN64)
#define STDTIME_TIME_T64_WIN
#endif
#endif

#ifndef STDTIME_TIME_T64_WIN
#ifdef _TIME64_T_DEFINED
#define STDTIME_TIME_T64_WIN
#endif
#endif


#ifndef STDTIME_TIME_T64_ENABLED
#ifdef STDTIME_TIME_T64_WIN
#define STDTIME_TIME_T64_ENABLED
#endif
#endif


#ifdef STDTIME_TIME_T64_ENABLED
#ifdef STDTIME_TIME_T64_WIN

#ifdef _TIME64_T_DEFINED
typedef __time64_t          STDTIME_WIN_TIME_T;
#else
typedef time_t              STDTIME_WIN_TIME_T;
#endif /* _TIME64_T_DEFINED */

#else /* not STDTIME_TIME_T64_WIN */
typedef QUADLIB_I64N        STDTIME_WIN_TIME_T;
#endif /* STDTIME_TIME_T64_WIN */




#else /* not STDTIME_TIME_T64_ENABLED */
typedef time_t              STDTIME_WIN_TIME_T;
#endif /* STDTIME_TIME_T64_ENABLED */


#define STDTIME_SCALE_NSEC  100     /* only 1 digit of nsec precision */
#define STDTIME_PREC_NSEC   10      /* only 1 digit of nsec precision */

#define STDTIME_WIN_EPOCH_YEAR      1601
#define STDTIME_UNIX_EPOCH_YEAR     1970
#define STDTIME_DST_CUTOFF_YEAR     1945

#define STDTIME_YEAR_MIN    STDTIME_WIN_EPOCH_YEAR    /* base/epoch year */
#define STDTIME_YEAR_MAX    9999    /* to avoid overflow in STDTIME_STRING */

#define STDTIME_INVALID             {0,0}
#define STDTIME_FIELDS_INVALID      {0,0,0,0,0,0,0,0,0}
#define STDTIME_STRING_INVALID      {0}

#define STDTIME_SPAN_INVALID    QUADLIB_I64_MAX

#define STDTIME_DELTA_DAY_MAX (8400*366)


#define STDTIME_DELTA_INVALID { 0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF,           \
    0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF }                       /**/


/* an invalid span exceeds +/- 2,656,281,600,000,000,000 */
/* which is 8400*366 days in terms of 100-nanosecond intervals */
/* the pos value in hex is 24DD 020A A768 0000 */
/* the neg value in hex is DB22 FDF5 5898 0000 */

#define STDTIME_SPAN_MAX_HI     0x24DD020A
#define STDTIME_SPAN_MAX_LO     0xA7680000

#define STDTIME_SPAN_MIN_HI     0xDB22FDF5
#define STDTIME_SPAN_MIN_LO     0x58980000

#define STDTIME_ERR         (-2)
#define STDTIME_LT          (-1)
#define STDTIME_EQ          0
#define STDTIME_GT          1

/* Main StdTime structure identical to Windows FILETIME.	*/
typedef STDTIME_WIN_FILETIME STDTIME;

typedef struct
  {
  int32_t                   year;
  int32_t                   mon;
  int32_t                   day;
  int32_t                   hour;
  int32_t                   min;
  int32_t                   sec;
  int32_t                   msec;   /* milliseconds     */
  int32_t                   usec;   /* microseconds     */
  int32_t                   nsec;   /* nanoseconds      */
  }
STDTIME_FIELDS;


typedef struct
  {
  int32_t                   day;
  int32_t                   hour;
  int32_t                   min;
  int32_t                   sec;
  int32_t                   msec;
  int32_t                   usec;
  int32_t                   nsec;
  }
STDTIME_DELTA;


typedef struct
  {
  QUADLIB_I64               value;
  }
STDTIME_SPAN;


/*                                1         2         3        */
/*                      0123456789012345678901234567890123456  */
/* STRING format:       yyyy-mm-ddThh:mi:ss.fffffff#           */
/* TZO format:                                     +hh:mm:ss#  */
/*                                                 0123456789  */

#define STDTIME_STRING_DATE_LEN     10
#define STDTIME_STRING_TIME_LEN     8
#define STDTIME_STRING_FRAC_LEN     7

#define STDTIME_STRING_DATE_POS     0
#define STDTIME_STRING_TIME_POS     11
#define STDTIME_STRING_FRAC_POS     20

#define STDTIME_STRING_DLM_D1       4
#define STDTIME_STRING_DLM_D2       7
#define STDTIME_STRING_DLM_S1       10
#define STDTIME_STRING_DLM_T1       13
#define STDTIME_STRING_DLM_T2       16
#define STDTIME_STRING_DLM_F1       19

#define STDTIME_STRING_LEN          27
#define STDTIME_TZO_LEN             9
#define STDTIME_STRING_MAXLEN       (STDTIME_STRING_LEN+STDTIME_TZO_LEN)


typedef struct
  {
  char                      str [STDTIME_TZO_LEN + 1];
  }
STDTIME_TZOA;

/* embTzo represents an 'embedded' timezone offset, a suffix to the time */
/* string 'str' in STDTIME_STRINGA and STDTIME_STRINGW. */
/* embTzo may be a null string, the letter Z or an offset like -05:00 */

/* extTzo represents an 'external' timezone offset, created as an output */
/* value by the STDTIME library.  the library creates, but does not */
/* directly use, the extTzo field. */

typedef struct
  {
  char                      str  [STDTIME_STRING_LEN];
  char                      embTzo [STDTIME_TZO_LEN + 1];
  STDTIME_TZOA              extTzo; /* external TimeZone Offset */
  }
STDTIME_STRINGA;

#ifdef STDTIME_UNICODE
#else
#define STDTIME_STRING      STDTIME_STRINGA
#define STDTIME_TZO         STDTIME_TZOA
#endif /* STDTIME_UNICODE */

typedef struct
  {
  STDTIME_WIN_TIME_T        t;
  }
STDTIME_TIMETYPE;

#define STDTIME_API_MKTIME  mktime

/* Round/Trunc field selectors */

#define STDTIME_FIELD_DAY   0
#define STDTIME_FIELD_HOUR  1
#define STDTIME_FIELD_MIN   2
#define STDTIME_FIELD_SEC   3
#define STDTIME_FIELD_MSEC  4
#define STDTIME_FIELD_USEC  5
#define STDTIME_FIELD_NSEC  6


#define STDTIME_ERRCODE_LEN  27

/* an error code has the form   0xNNNNNNNN = HHHH.LLLL.RRRR#  */
/*                              0123456789012345678901234567  */
/* this holds a raw hex RC value, and the RC as subfields     */

#define STDTIME_ERRMSG_LEN  80

typedef struct
  {
  char                      errcode [STDTIME_ERRCODE_LEN+1];
  char                      hilevel [STDTIME_ERRMSG_LEN+1];
  char                      lolevel [STDTIME_ERRMSG_LEN+1];
  char                      reason  [STDTIME_ERRMSG_LEN+1];
  }
STDTIME_ERRMSGA;

#define STDTIME_IF(x)       ((rc=(x)) == STDTIME_OK)
#define STDTIME_IFNOT(x)    ((rc=(x)) != STDTIME_OK)


/*** helper function to assemble a STDTIME return code ***********************/

STDTIME_RC                  StdTimeRet (
  const STDTIME_RC          /*I*/ func,
  const STDTIME_RC          /*I*/ ec);


/*** StdTime/StdTimeFields conversions ***************************************/


STDTIME_RC                  StdTimeToStdTimeFields (
  const STDTIME *           /*I*/ pStdTime,
  STDTIME_FIELDS *          /*O*/ pStdTimeFields);

STDTIME_RC                  StdTimeFieldsToStdTime (
  const STDTIME_FIELDS *    /*I*/ pStdTimeFields,
  STDTIME *                 /*O*/ pStdTime);


/*** StdTime/external conversions ********************************************/


STDTIME_RC                  StdTimeToStdTimeStringA (
  const STDTIME *           /*I*/ pStdTime,
  STDTIME_STRINGA *         /*O*/ pStdTimeString);

#ifdef STDTIME_UNICODE
#else
#define StdTimeToStdTimeString  StdTimeToStdTimeStringA
#endif /* STDTIME_UNICODE */


/*** external/StdTime conversions ********************************************/


STDTIME_RC                  StdTimeStringAToStdTime (
  const STDTIME_STRINGA *   /*I*/ pStdTimeString,
  STDTIME *                 /*O*/ pStdTime);

#ifdef STDTIME_UNICODE
#else
#define StdTimeStringToStdTime          StdTimeStringAToStdTime
#endif /* STDTIME_UNICODE */


/*** StdTimeFields/external conversions **************************************/


STDTIME_RC                  StdTimeFieldsToStdTimeStringA (
  const STDTIME_FIELDS *    /*I*/ pStdTimeFields,
  STDTIME_STRINGA *         /*O*/ pStdTimeString);

#ifdef STDTIME_UNICODE
#else
#define StdTimeFieldsToStdTimeString StdTimeFieldsToStdTimeStringA
#endif /* STDTIME_UNICODE */


/*** external/StdTimeFields conversions **************************************/


STDTIME_RC                  StdTimeStringAToStdTimeFields (
  const STDTIME_STRINGA *   /*I*/ pStdTimeString,
  STDTIME_FIELDS *          /*O*/ pStdTimeFields);

#ifdef STDTIME_UNICODE
#else
#define StdTimeStringToStdTimeFields    StdTimeStringAToStdTimeFields
#endif /* STDTIME_UNICODE */

/*** StdTime to/from Local ***************************************************/


STDTIME_RC                  StdTimeToLocalStdTime (
  const STDTIME *           /*I*/ pStdTime,
  STDTIME *                 /*O*/ pStdTimeLocal);

STDTIME_RC                  LocalStdTimeToStdTime (
  const STDTIME *           /*I*/ pStdTimeLocal,
  STDTIME *                 /*O*/ pStdTime);


/*** StdTimeFields to/from Local *********************************************/


STDTIME_RC                  StdTimeFieldsToLocalStdTimeFields (
  const STDTIME_FIELDS *    /*I*/ pStdTimeFields,
  STDTIME_FIELDS *          /*O*/ pStdTimeFieldsLocal);

STDTIME_RC                  LocalStdTimeFieldsToStdTimeFields (
  const STDTIME_FIELDS *    /*I*/ pStdTimeFieldsLocal,
  STDTIME_FIELDS *          /*O*/ pStdTimeFields);


/*** StdTimeString to/from Local *********************************************/


STDTIME_RC                  StdTimeStringAToLocalStdTimeStringA (
  const STDTIME_STRINGA *   /*I*/ pStdStdTimeString,
  STDTIME_STRINGA *         /*O*/ pLocStdTimeString);

STDTIME_RC                  LocalStdTimeStringAToStdTimeStringA (
  const STDTIME_STRINGA *   /*I*/ pLocStdTimeString,
  STDTIME_STRINGA *         /*O*/ pStdStdTimeString);

#define StdTimeStringToLocalStdTimeStringA StdTimeStringAToLocalStdTimeStringA
#define LocalStdTimeStringToStdTimeStringA LocalStdTimeStringAToStdTimeStringA


#ifdef STDTIME_UNICODE
#else
#define StdTimeStringToLocalStdTimeString StdTimeStringAToLocalStdTimeStringA
#define LocalStdTimeStringToStdTimeString LocalStdTimeStringAToStdTimeStringA
#endif /* STDTIME_UNICODE */

/*** Get (current UTC/GMT time) functions ************************************/


STDTIME_RC                  GetStdTime (
  STDTIME *                 /*O*/ pStdTime);

STDTIME_RC                  GetStdTimeFields (
  STDTIME_FIELDS *          /*O*/ pStdTimeFields);

STDTIME_RC                  GetStdTimeStringA (
  STDTIME_STRINGA *         /*O*/ pStdTimeString);

#ifdef STDTIME_UNICODE
#else
#define GetStdTimeString    GetStdTimeStringA
#endif /* STDTIME_UNICODE */


/*** GetLocal (current time) functions ***************************************/


STDTIME_RC                  GetLocalStdTime (
  STDTIME *                 /*O*/ pStdTime);

STDTIME_RC                  GetLocalStdTimeFields (
  STDTIME_FIELDS *          /*O*/ pStdTimeFields);

STDTIME_RC                  GetLocalStdTimeStringA (
  STDTIME_STRINGA *         /*O*/ pStdTimeString);

#ifdef STDTIME_UNICODE
#else
#define GetLocalStdTimeString GetLocalStdTimeStringA
#endif /* STDTIME_UNICODE */


/*** validation functions ****************************************************/


STDTIME_RC                  ValidStdTime (
  const STDTIME *           /*I*/ pStdTime);

STDTIME_RC                  ValidStdTimeFields (
  const STDTIME_FIELDS *    /*I*/ pStdTimeFields);

STDTIME_RC                  ValidStdTimeStringA (
  const STDTIME_STRINGA *   /*I*/ pStdTimeString);

STDTIME_RC                  ValidStdTimeDelta (
  const STDTIME_DELTA *     /*I*/ pDelta);

STDTIME_RC                  ValidStdTimeSpan (
  const STDTIME_SPAN *      /*I*/ pSpan);


#ifdef STDTIME_UNICODE
#else
#define ValidStdTimeString  ValidStdTimeStringA
#endif /* STDTIME_UNICODE */


/*** Format functions ********************************************************/


STDTIME_RC                  FormatStdTimeA (
  char *                    /*O*/ pTarget,
  size_t                    /*I*/ nMaxsize,
  const char *              /*I*/ pFormat,
  const STDTIME *           /*I*/ pStdTime);

STDTIME_RC                  FormatStdTimeFieldsA (
  char *                    /*O*/ pTarget,
  size_t                    /*I*/ nMaxsize,
  const char *              /*I*/ pFormat,
  const STDTIME_FIELDS *    /*I*/ pStdTimeFields);

STDTIME_RC                  FormatStdTimeStringA (
  char *                    /*O*/ pTarget,
  size_t                    /*I*/ nMaxsize,
  const char *              /*I*/ pFormat,
  const STDTIME_STRINGA *   /*I*/ pStdTimeString);

STDTIME_RC                  TrimStdTimeStringA (
  STDTIME_STRINGA *         /*IO*/ pStdTimeString,
  int32_t                   /*I*/ nSize);

STDTIME_RC                  TruncStdTimeStringA (
  STDTIME_STRINGA *         /*IO*/ pStdTimeString,
  int32_t                   /*I*/ nSize);

#ifdef STDTIME_UNICODE
#else
#define FormatStdTime       FormatStdTimeA
#define FormatStdTimeFields FormatStdTimeFieldsA
#define FormatStdTimeString FormatStdTimeStringA

#define TrimStdTimeString   TrimStdTimeStringA
#define TruncStdTimeString  TruncStdTimeStringA

#endif /* STDTIME_UNICODE */


/*** delimiter functions *****************************************************/


STDTIME_RC                  SetStdTimeStringDelimitersA (
  const char *              /*I*/ pDelimiters);

STDTIME_RC                  RepairStdTimeStringA (
  STDTIME_STRINGA *         /*O*/ pStdTimeString);

STDTIME_RC                  DelimitStdTimeStringA (
  STDTIME_STRINGA *         /*O*/ pStdTimeString,
  const char *              /*I*/ pDelim);

#ifdef STDTIME_UNICODE
#else
#define SetStdTimeStringDelimiters      SetStdTimeStringDelimitersA
#define RepairStdTimeString             RepairStdTimeStringA
#define DelimitStdTimeString            DelimitStdTimeStringA
#endif /* STDTIME_UNICODE */


/*** AddFraction adjustment function *****************************************/


STDTIME_RC                  StdTimeFields_AddFraction (
  STDTIME_FIELDS *          /*IO*/ pStdTimeFields,
  int32_t                   /*I*/ nMsec,
  int32_t                   /*I*/ nUsec,
  int32_t                   /*I*/ nNsec);


/*** AddSpan functions *******************************************************/


STDTIME_RC                  StdTime_AddSpan (
  STDTIME *                 /*IO*/ pStdTime,
  const STDTIME_SPAN *      /*I*/  pSpan);

STDTIME_RC                  StdTimeFields_AddSpan (
  STDTIME_FIELDS *          /*IO*/ pStdTimeFields,
  const STDTIME_SPAN *      /*I*/  pSpan);

STDTIME_RC                  StdTimeStringA_AddSpan (
  STDTIME_STRINGA *         /*IO*/ pStdTimeString,
  const STDTIME_SPAN *      /*I*/  pSpan);

#ifdef STDTIME_UNICODE
#else
#define StdTimeString_AddSpan       StdTimeStringA_AddSpan
#endif /* STDTIME_UNICODE */


/*** SubSpan functions *******************************************************/


STDTIME_RC                  StdTime_SubSpan (
  STDTIME *                 /*IO*/ pStdTime,
  const STDTIME_SPAN *      /*I*/  pSpan);

STDTIME_RC                  StdTimeFields_SubSpan (
  STDTIME_FIELDS *          /*IO*/ pStdTimeFields,
  const STDTIME_SPAN *      /*I*/  pSpan);

STDTIME_RC                  StdTimeStringA_SubSpan (
  STDTIME_STRINGA *         /*IO*/ pStdTimeString,
  const STDTIME_SPAN *      /*I*/  pSpan);

#ifdef STDTIME_UNICODE
#else
#define StdTimeString_SubSpan       StdTimeStringA_SubSpan
#endif /* STDTIME_UNICODE */


/*** GetSpan functions *******************************************************/


STDTIME_RC                  StdTime_GetSpan (
  const STDTIME *           /*I*/ pStdTimeOne,
  const STDTIME *           /*I*/ pStdTimeTwo,
  STDTIME_SPAN *            /*O*/ pSpan);

STDTIME_RC                  StdTimeFields_GetSpan (
  const STDTIME_FIELDS *    /*I*/ pStdTimeFieldsOne,
  const STDTIME_FIELDS *    /*I*/ pStdTimeFieldsTwo,
  STDTIME_SPAN *            /*O*/ pSpan);

STDTIME_RC                  StdTimeStringA_GetSpan (
  const STDTIME_STRINGA *   /*I*/ pStdTimeStringOne,
  const STDTIME_STRINGA *   /*I*/ pStdTimeStringTwo,
  STDTIME_SPAN *            /*O*/ pSpan);

#ifdef STDTIME_UNICODE
#else
#define StdTimeString_GetSpan       StdTimeStringA_GetSpan
#endif /* STDTIME_UNICODE */


/*** AddDelta functions ******************************************************/

STDTIME_RC                  StdTime_AddDelta (
  STDTIME *                 /*IO*/ pStdTime,
  const STDTIME_DELTA *     /*I*/  pDelta);

STDTIME_RC                  StdTimeFields_AddDelta (
  STDTIME_FIELDS *          /*IO*/ pStdTimeFields,
  const STDTIME_DELTA *     /*I*/  pDelta);

STDTIME_RC                  StdTimeStringA_AddDelta (
  STDTIME_STRINGA *         /*IO*/ pStdTimeString,
  const STDTIME_DELTA *     /*I*/  pDelta);

#ifdef STDTIME_UNICODE
#else
#define StdTimeString_AddDelta      StdTimeStringA_AddDelta
#endif /* STDTIME_UNICODE */


/*** SubDelta functions ******************************************************/


STDTIME_RC                  StdTime_SubDelta (
  STDTIME *                 /*IO*/ pStdTime,
  const STDTIME_DELTA *     /*I*/  pDelta);

STDTIME_RC                  StdTimeFields_SubDelta (
  STDTIME_FIELDS *          /*IO*/ pStdTimeFields,
  const STDTIME_DELTA *     /*I*/  pDelta);

STDTIME_RC                  StdTimeStringA_SubDelta (
  STDTIME_STRINGA *         /*IO*/ pStdTimeString,
  const STDTIME_DELTA *     /*I*/  pDelta);

#ifdef STDTIME_UNICODE
#else
#define StdTimeString_SubDelta      StdTimeStringA_SubDelta
#endif /* STDTIME_UNICODE */


/*** GetDelta functions ******************************************************/


STDTIME_RC                  StdTime_GetDelta (
  const STDTIME *           /*I*/ pStdTimeOne,
  const STDTIME *           /*I*/ pStdTimeTwo,
  STDTIME_DELTA *           /*O*/ pDelta);

STDTIME_RC                  StdTimeFields_GetDelta (
  const STDTIME_FIELDS *    /*I*/ pStdTimeFieldsOne,
  const STDTIME_FIELDS *    /*I*/ pStdTimeFieldsTwo,
  STDTIME_DELTA *           /*O*/ pDelta);

STDTIME_RC                  StdTimeStringA_GetDelta (
  const STDTIME_STRINGA *   /*I*/ pStdTimeStringOne,
  const STDTIME_STRINGA *   /*I*/ pStdTimeStringTwo,
  STDTIME_DELTA *           /*O*/ pDelta);

#ifdef STDTIME_UNICODE
#else
#define StdTimeString_GetDelta      StdTimeStringA_GetDelta
#endif /* STDTIME_UNICODE */


/*** Delta/Span conversion functions *****************************************/


STDTIME_RC                  StdTimeDeltaToStdTimeSpan (
  const STDTIME_DELTA *     /*I*/ pDelta,
  STDTIME_SPAN *            /*O*/ pSpan);

STDTIME_RC                  StdTimeSpanToStdTimeDelta (
  const STDTIME_SPAN *      /*I*/ pSpan,
  STDTIME_DELTA *           /*O*/ pDelta);


/*** Delta/Span sign-related functions ***************************************/


int32_t                     SgnStdTimeDelta (
  const STDTIME_DELTA *     /*I*/ pDelta);

STDTIME_RC                  AbsStdTimeDelta (
  STDTIME_DELTA *           /*IO*/ pDelta);

STDTIME_RC                  NegStdTimeDelta (
  STDTIME_DELTA *           /*IO*/ pDelta);


int32_t                     SgnStdTimeSpan (
  const STDTIME_SPAN *      /*I*/ pSpan);

STDTIME_RC                  AbsStdTimeSpan (
  STDTIME_SPAN *            /*IO*/ pSpan);

STDTIME_RC                  NegStdTimeSpan (
  STDTIME_SPAN *            /*IO*/ pSpan);


/*** Compare functions *******************************************************/


int32_t                     CompareStdTime (
  const STDTIME *           /*I*/ pStdTimeOne,
  const STDTIME *           /*I*/ pStdTimeTwo);

int32_t                     CompareStdTimeFields (
  const STDTIME_FIELDS *    /*I*/ pStdTimeFieldsOne,
  const STDTIME_FIELDS *    /*I*/ pStdTimeFieldsTwo);

int32_t                     CompareStdTimeStringA (
  const STDTIME_STRINGA *   /*I*/ pStdTimeStringOne,
  const STDTIME_STRINGA *   /*I*/ pStdTimeStringTwo);

#ifdef STDTIME_UNICODE
#else
#define CompareStdTimeString CompareStdTimeStringA
#endif /* STDTIME_UNICODE */


/*** FileTime functions ******************************************************/



STDTIME_RC                  StdTimeToFileTime (
  const STDTIME *           /*I*/ pStdTime,
  STDTIME_WIN_FILETIME *    /*O*/ pFileTime);

STDTIME_RC                  FileTimeToStdTime (
  const STDTIME_WIN_FILETIME *  /*I*/ pFileTime,
  STDTIME *                 /*O*/ pStdTime);



STDTIME_RC                  StdTimeFieldsToFileTime (
  const STDTIME_FIELDS *    /*I*/ pStdTimeFields,
  STDTIME_WIN_FILETIME *    /*O*/ pFileTime);

STDTIME_RC                  FileTimeToStdTimeFields (
  const STDTIME_WIN_FILETIME *  /*I*/ pFileTime,
  STDTIME_FIELDS *          /*O*/ pStdTimeFields);



STDTIME_RC                  StdTimeStringAToFileTime (
  const STDTIME_STRINGA *   /*I*/ pStdTimeString,
  STDTIME_WIN_FILETIME *    /*O*/ pFileTime);

STDTIME_RC                  FileTimeToStdTimeStringA (
  const STDTIME_WIN_FILETIME *  /*I*/ pFileTime,
  STDTIME_STRINGA *         /*O*/ pStdTimeString);

#ifdef STDTIME_UNICODE
#else
#define StdTimeStringToFileTime StdTimeStringAToFileTime
#define FileTimeToStdTimeString FileTimeToStdTimeStringA
#endif /* STDTIME_UNICODE */


/*** SystemTimeEx functions **************************************************/


STDTIME_RC                  StdTimeToSystemTimeEx (
  const STDTIME *           /*I*/ pStdTime,
  STDTIME_WIN_SYSTEMTIME *  /*O*/ pSystemTime,
  int32_t *                 /*O*/ pUsec, /* optional */
  int32_t *                 /*O*/ pNsec); /* optional */


STDTIME_RC                  SystemTimeExToStdTime (
  const STDTIME_WIN_SYSTEMTIME * /*I*/ pSystemTime,
  int32_t                   /*I*/ nUsec,
  int32_t                   /*I*/ nNsec,
  STDTIME *                 /*O*/ pStdTime);


STDTIME_RC                  StdTimeFieldsToSystemTimeEx (
  const STDTIME_FIELDS *    /*I*/ pStdTimeFields,
  STDTIME_WIN_SYSTEMTIME *  /*O*/ pSystemTime,
  int32_t *                 /*O*/ pUsec, /* optional */
  int32_t *                 /*O*/ pNsec); /* optional */


STDTIME_RC                  SystemTimeExToStdTimeFields (
  const STDTIME_WIN_SYSTEMTIME * /*I*/ pSystemTime,
  int32_t                   /*I*/ nUsec,
  int32_t                   /*I*/ nNsec,
  STDTIME_FIELDS *          /*O*/ pStdTimeFields);


/*** SystemTime functions ****************************************************/


STDTIME_RC                  StdTimeToSystemTime (
  const STDTIME *           /*I*/ pStdTime,
  STDTIME_WIN_SYSTEMTIME *  /*O*/ pSystemTime);


STDTIME_RC                  SystemTimeToStdTime (
  const STDTIME_WIN_SYSTEMTIME * /*I*/ pSystemTime,
  STDTIME *                 /*O*/ pStdTime);


STDTIME_RC                  StdTimeFieldsToSystemTime (
  const STDTIME_FIELDS *    /*I*/ pStdTimeFields,
  STDTIME_WIN_SYSTEMTIME *  /*O*/ pSystemTime);


STDTIME_RC                  SystemTimeToStdTimeFields (
  const STDTIME_WIN_SYSTEMTIME * /*I*/ pSystemTime,
  STDTIME_FIELDS *          /*O*/ pStdTimeFields);

#ifdef _WIN32	/* DbTimeStamp functions only for _WIN32	*/
/*** DbTimeStamp functions ***************************************************/

/* NOTE: can't include "oledb.h" for DBTIMESTAMP so just define our own struct here.*/
typedef struct
  {
  STDTIME_WIN_SHORT         year;
  STDTIME_WIN_USHORT        month;
  STDTIME_WIN_USHORT        day;
  STDTIME_WIN_USHORT        hour;
  STDTIME_WIN_USHORT        minute;
  STDTIME_WIN_USHORT        second;
  STDTIME_WIN_ULONG         fraction;
  } STDTIME_WIN_DBTIMESTAMP;

STDTIME_WIN_DBTIMESTAMP     StdTimeApiZeroDbTimeStamp (ST_VOID);

STDTIME_RC                  StdTimeToDbTimeStamp (
  const STDTIME *           /*I*/ pStdTime,
  STDTIME_WIN_DBTIMESTAMP * /*O*/ pDbTimeStamp);

STDTIME_RC                  DbTimeStampToStdTime (
  const STDTIME_WIN_DBTIMESTAMP * /*I*/ pDbTimeStamp,
  STDTIME *                 /*O*/ pStdTime);

STDTIME_RC                  StdTimeFieldsToDbTimeStamp (
  const STDTIME_FIELDS *    /*I*/ pStdTimeFields,
  STDTIME_WIN_DBTIMESTAMP * /*O*/ pDbTimeStamp);

STDTIME_RC                  DbTimeStampToStdTimeFields (
  const STDTIME_WIN_DBTIMESTAMP * /*I*/ pDbTimeStamp,
  STDTIME_FIELDS *          /*O*/ pStdTimeFields);
#endif	/* DbTimeStamp functions only for _WIN32	*/


/*** StructTmEx functions ****************************************************/


STDTIME_RC                  StdTimeToStructTmEx (
  const STDTIME *           /*I*/ pStdTime,
  struct tm *               /*O*/ pStructTm,
  int32_t *                 /*O*/ pNsec); /* optional */

STDTIME_RC                  StructTmExToStdTime (
  const struct tm *         /*I*/ pStructTm,
  int32_t                   /*I*/ nNsec,
  STDTIME *                 /*O*/ pStdTime);

/*** TimeTypeEx functions ****************************************************/


STDTIME_RC                  StdTimeToTimeTypeEx (
  const STDTIME *           /*I*/ pStdTime,
  time_t *                  /*O*/ pTimeT,
  int32_t *                 /*O*/ pNsec); /* optional */

STDTIME_RC                  TimeTypeExToStdTime (
  time_t                    /*I*/ timet,
  int32_t                   /*I*/ nsec,
  STDTIME *                 /*O*/ pStdTime);

/*** Constructor-like functions **********************************************/


STDTIME_FIELDS              StdTimeFields (
  int32_t                   /*I*/ year,
  int32_t                   /*I*/ mon,
  int32_t                   /*I*/ day,
  int32_t                   /*I*/ hour,
  int32_t                   /*I*/ min,
  int32_t                   /*I*/ sec,
  int32_t                   /*I*/ msec,
  int32_t                   /*I*/ usec,
  int32_t                   /*I*/ nsec);


STDTIME_STRINGA             StdTimeStringA (
  const char *              /*I*/ str);

#ifdef STDTIME_UNICODE
#else
#define StdTimeString       StdTimeStringA
#endif /* STDTIME_UNICODE */


STDTIME_DELTA               StdTimeDelta (
  int32_t                   /*I*/ day,
  int32_t                   /*I*/ hour,
  int32_t                   /*I*/ min,
  int32_t                   /*I*/ sec,
  int32_t                   /*I*/ msec,
  int32_t                   /*I*/ usec,
  int32_t                   /*I*/ nsec);


STDTIME_SPAN                StdTimeSpan (
  QUADLIB_I64               /*I*/ value);


/*** Initializer functions ***************************************************/


STDTIME                     ZeroStdTime (ST_VOID);
STDTIME_FIELDS              ZeroStdTimeFields (ST_VOID);
STDTIME_DELTA               ZeroStdTimeDelta (ST_VOID);
STDTIME_SPAN                ZeroStdTimeSpan (ST_VOID);

STDTIME_STRINGA             ZeroStdTimeStringA (ST_VOID);

STDTIME_TZOA                ZeroStdTimeTzoA (ST_VOID);

STDTIME_ERRMSGA             ZeroStdTimeErrMsgA (ST_VOID);

#ifdef STDTIME_UNICODE
#else
#define ZeroStdTimeString   ZeroStdTimeStringA
#define ZeroStdTimeTzo      ZeroStdTimeTzoA
#define ZeroStdTimeErrMsg   ZeroStdTimeErrMsgA
#endif /* STDTIME_UNICODE */

/* initializers for outside types use 'StdTimeApi' prefix */

STDTIME_TIMETYPE            StdTimeApiZeroTimeType (ST_VOID);
STDTIME_WIN_FILETIME        StdTimeApiZeroFileTime (ST_VOID);
STDTIME_WIN_SYSTEMTIME      StdTimeApiZeroSystemTime (ST_VOID);
struct tm                   StdTimeApiZeroStructTm (ST_VOID);


/*****************************************************************************/
/*  NORMALIZE, ROUND, TRUNC                                                  */
/*****************************************************************************/


STDTIME_RC                  NormalizeStdTimeDelta (
  STDTIME_DELTA *           /*IO*/ pDelta);


STDTIME_RC                  TruncStdTimeDelta (
  STDTIME_DELTA *           /*IO*/ pDelta,
  int32_t                   /*I*/  field);


STDTIME_RC                  RoundStdTimeDelta (
  STDTIME_DELTA *           /*IO*/ pDelta,
  int32_t                   /*I*/  field);


/*** convenience calendar functions ******************************************/


STDTIME_RC                  StdTimeGetDayofWeekAndYear (
  int32_t                   /*I*/ nYear,
  int32_t                   /*I*/ nMon,
  int32_t                   /*I*/ nDayofMon,
  int32_t *                 /*O*/ pDayofWeek,
  int32_t *                 /*O*/ pDayofYear);


int32_t                     StdTimeYearIsLeap (
  int32_t                   /*I*/ year);


int32_t                     StdTimeDaysInYearMon (
  int32_t                   /*I*/ year,
  int32_t                   /*I*/ mon);


/*** DateEx functions ********************************************************/


STDTIME_RC                  StdTimeToDateEx (
  const STDTIME *           /*I*/ pStdTime,
  STDTIME_WIN_DATE *        /*O*/ pDate,
  int32_t *                 /*O*/ pMsec,  /* optional */
  int32_t *                 /*O*/ pUsec,  /* optional */
  int32_t *                 /*O*/ pNsec); /* optional */


STDTIME_RC                  DateExToStdTime (
  const STDTIME_WIN_DATE *  /*I*/ pDate,
  int32_t                   /*I*/ nMsec,
  int32_t                   /*I*/ nUsec,
  int32_t                   /*I*/ nNsec,
  STDTIME *                 /*O*/ pStdTime);


STDTIME_RC                  StdTimeFieldsToDateEx (
  const STDTIME_FIELDS *    /*I*/ pStdTimeFields,
  STDTIME_WIN_DATE *        /*O*/ pDate,
  int32_t *                 /*O*/ pMsec,  /* optional */
  int32_t *                 /*O*/ pUsec,  /* optional */
  int32_t *                 /*O*/ pNsec); /* optional */


STDTIME_RC                  DateExToStdTimeFields (
  const STDTIME_WIN_DATE *  /*I*/ pDate,
  int32_t                   /*I*/ nMsec,
  int32_t                   /*I*/ nUsec,
  int32_t                   /*I*/ nNsec,
  STDTIME_FIELDS *          /*O*/ pStdTimeFields);


/*** Date functions ***************************************************/


STDTIME_RC                  StdTimeToDate (
  const STDTIME *           /*I*/ pStdTime,
  STDTIME_WIN_DATE *        /*O*/ pDate);


STDTIME_RC                  DateToStdTime (
  const STDTIME_WIN_DATE *  /*I*/ pDate,
  STDTIME *                 /*O*/ pStdTime);


STDTIME_RC                  StdTimeFieldsToDate (
  const STDTIME_FIELDS *    /*I*/ pStdTimeFields,
  STDTIME_WIN_DATE *        /*O*/ pDate);


STDTIME_RC                  DateToStdTimeFields (
  const STDTIME_WIN_DATE *  /*I*/ pDate,
  STDTIME_FIELDS *          /*O*/ pStdTimeFields);


/*** timezone-offset support functions ***************************************/


STDTIME_RC                  StdTimeStringAToTzoA (
  const STDTIME_STRINGA *   /*I*/ pStdTimeString,
  STDTIME_TZOA *            /*O*/ pTzo);

STDTIME_RC                  TzoAToStdTimeStringA (
  const STDTIME_TZOA *      /*I*/ pTzo,
  STDTIME_STRINGA *         /*O*/ pStdTimeString);

STDTIME_RC                  ValidStdTimeTzoA (
  const STDTIME_TZOA *      /*I*/ pTzo);

STDTIME_RC                  NormalizeStdTimeTzoA (
  STDTIME_TZOA *            /*I*/ pTzo);

STDTIME_RC                  GetLocalStdTimeTzoA (
  STDTIME_TZOA *            /*O*/ pTzo);

#define StdTimeStringToTzoA     StdTimeStringAToTzoA
#define TzoToStdTimeStringA     TzoAToStdTimeStringA


#ifdef STDTIME_UNICODE
#else
#define StdTimeStringToTzo      StdTimeStringAToTzoA
#define TzoToStdTimeString      TzoAToStdTimeStringA
#define ValidStdTimeTzo         ValidStdTimeTzoA
#define NormalizeStdTimeTzo     NormalizeStdTimeTzoA
#define GetLocalStdTimeTzo      GetLocalStdTimeTzoA
#endif /* STDTIME_UNICODE */


/*** error message decoding functions ****************************************/


STDTIME_RC                  StdTimeRcToErrMsgA (
  const STDTIME_RC          /*I*/ stdTimeRc,
  STDTIME_ERRMSGA *         /*O*/ pErrMsg);

#ifdef STDTIME_UNICODE
#else
#define StdTimeRcToErrMsg   StdTimeRcToErrMsgA
#endif /* STDTIME_UNICODE */


/*** normalization functions; primarily private functions ********************/


STDTIME_RC                  Rc_NormalizeStdTimeFields (
  STDTIME_FIELDS *          pFields);

STDTIME_RC                  Rc_StdTime_NormalizeSystemTimeEx (
  STDTIME_WIN_SYSTEMTIME *  /*IO*/ pSystemTime,
  int32_t *                 /*IO*/ pUsec,
  int32_t *                 /*IO*/ pNsec);

STDTIME_RC                  Rc_StdTime_NormalizeStructTmEx (
  struct tm *               /*IO*/ pStructTm,
  int32_t *                 /*IO*/ pMsec,
  int32_t *                 /*IO*/ pUsec,
  int32_t *                 /*IO*/ pNsec);


/* Utility functions							*/
STDTIME_BOOL Bool_StdTimeGmTimeR (
  time_t *                  pTimeT,
  struct tm *               pStructTm);

/*****************************************************************************/
/*  END                                                                      */
/*****************************************************************************/


#ifdef  __cplusplus
}
#endif


#endif /* _STDTIME_H_7583ECB2_DB3F_48B3_8A97_2D1D2A38C84A_ */

