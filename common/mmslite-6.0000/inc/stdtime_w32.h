/*****************************************************************************/
/* SISCO SOFTWARE MODULE HEADER **********************************************/
/*****************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,            */
/*            2005-2010 All Rights Reserved                                  */
/*                                                                           */
/* MODULE NAME : stdtime_w32.h                                               */
/* PRODUCT(S)  : Standard Time Management Library                            */
/*                                                                           */
/* MODULE DESCRIPTION:                                                       */
/*   Header file for Standard Time Management Library Win32 Compatibility    */
/*   Layer.  This header is compiled with STDTIMEW32_ENABLED on Windows to   */
/*   generate debugging code or to run emulation on Unix.  To run on         */
/*   Windows, compile with STDTIMEW32_DISABLED.  STDTIMEW32_ENABLED is       */
/*   required on Unix/Linux (that is, non-Windows) platforms.                */
/*                                                                           */
/*                                                                           */
/* MODIFICATION LOG:                                                         */
/*   Date    Who  Rev  Comments                                              */
/* --------  ---  ---  ----------------------------------------------------- */
/* 09/13/10  JRB       Fix STDTIME_WIN_FILETIME for SD_BIG_ENDIAN.           */
/* 12/17/07  JRB   04  Del STDTIME_WIN_VARIANT, ..DBTIMESTAMP, ..UDATE       */
/*		       structs (no longer used).                             */
/* 04/03/07  RLH   03  Make UDOT definition compatible with Linux 8.0        */
/* 06/07/06  RLH   02  Numerous features added, see stdtime.doc              */
/* 01/18/06  RLH   01  Created                                               */
/*****************************************************************************/


#ifndef _STDTIME_W32_H_29560654_4DD5_47A5_9B07_559DA09B36A3_
#define _STDTIME_W32_H_29560654_4DD5_47A5_9B07_559DA09B36A3_


#ifdef  __cplusplus
extern "C" {
#endif


/*****************************************************************************/
/*  ENABLED or DISABLED                                                      */
/*****************************************************************************/

#undef STDTIMEW32_DEBUGGING
#undef STDTIMEW32_UNIX

#if defined(STDTIMEW32_ENABLED) && defined(STDTIMEW32_DISABLED)
#error Both STDTIMEW32_ENABLED and STDTIMEW32_DISABLED defined
#undef STDTIMEW32_ENABLED
#endif

#if !defined(STDTIMEW32_ENABLED) && !defined(STDTIMEW32_DISABLED)

#if defined(STDTIMEW32_USE_CLOCKGETTIME)                                      \
 || defined(STDTIMEW32_USE_GETTIMEOFDAY)                                      \
 || defined(STDTIMEW32_USE_TIMEB)                                          /**/
#define STDTIMEW32_ENABLED
#else
#define STDTIMEW32_DISABLED
#endif

#endif

#if defined(STDTIMEW32_DISABLED) && !defined(_WIN32)
#error StdTime Windows Compatibility Layer disabled on non-Windows platform
#endif

#if defined(STDTIMEW32_ENABLED) && defined(_WIN32)
#define STDTIMEW32_DEBUGGING
#endif

#if defined(STDTIMEW32_ENABLED) && !defined(_WIN32)
#define STDTIMEW32_UNIX
#endif


/*****************************************************************************/
/*  STATIC (LOCAL) FUNCTION QUALIFIER                                        */
/*****************************************************************************/

#ifndef STDTIMEW32_STATIC
#define STDTIMEW32_STATIC
#endif


/*****************************************************************************/
/*  PORTABILITY API NAME DEFINITIONS                                         */
/*****************************************************************************/


#if defined(STDTIMEW32_USE_CLOCKGETTIME)
#define STDTIME_API_GETSYSTEMTIMEASFILETIME StdTimeW32ClockGetTimeToFileTime

#elif defined(STDTIMEW32_USE_GETTIMEOFDAY)
#define STDTIME_API_GETSYSTEMTIMEASFILETIME StdTimeW32GetTimeOfDayToFileTime

#elif defined(STDTIMEW32_USE_TIMEB)
#define STDTIME_API_GETSYSTEMTIMEASFILETIME StdTimeW32TimeBToFileTime

#else

#ifdef STDTIMEW32_UNIX
#error No API defined for GetSystemTimeAsFileTime Unix interface
#endif

#endif /* STDTIMEW32_USE_CLOCKGETTIME */


#ifdef STDTIMEW32_ENABLED
#define STDTIME_API_FILETIMETOLOCALFILETIME StdTimeW32FileTimeToLocalFileTime
#define STDTIME_API_LOCALFILETIMETOFILETIME StdTimeW32LocalFileTimeToFileTime
#else
#define STDTIME_API_FILETIMETOLOCALFILETIME FileTimeToLocalFileTime
#define STDTIME_API_LOCALFILETIMETOFILETIME LocalFileTimeToFileTime
#endif /* STDTIMEW32_ENABLED */


#ifdef STDTIMEW32_ENABLED
#define STDTIME_API_FILETIMETOSYSTEMTIME    StdTimeW32FileTimeToSystemTime
#define STDTIME_API_SYSTEMTIMETOFILETIME    StdTimeW32SystemTimeToFileTime
#else
#define STDTIME_API_SYSTEMTIMETOFILETIME    SystemTimeToFileTime
#define STDTIME_API_FILETIMETOSYSTEMTIME    FileTimeToSystemTime
#endif /* STDTIMEW32_ENABLED */


/*****************************************************************************/
/*  BASIC DEFINITIONS                                                        */
/*****************************************************************************/


#define STDTIME_WIN_EPOCH_YEAR      1601
#define STDTIME_UNIX_EPOCH_YEAR     1970

#undef STDTIME_WIN_VT_DATE_DEFINED

#ifdef _WIN32

/* if debugging non-Windows code on Windows, use compatibility typedefs */

typedef SHORT               STDTIME_WIN_SHORT;
typedef USHORT              STDTIME_WIN_USHORT;
typedef WORD                STDTIME_WIN_WORD;
typedef DWORD               STDTIME_WIN_DWORD;
typedef LONG                STDTIME_WIN_LONG;
typedef ULONG               STDTIME_WIN_ULONG;
typedef LONGLONG            STDTIME_WIN_LONGLONG;
typedef ULONGLONG           STDTIME_WIN_ULONGLONG;
typedef BOOL                STDTIME_WIN_BOOL;

#define STDTIME_WIN_TRUE    TRUE
#define STDTIME_WIN_FALSE   FALSE


typedef SYSTEMTIME          STDTIME_WIN_SYSTEMTIME;
typedef FILETIME            STDTIME_WIN_FILETIME;
typedef LARGE_INTEGER       STDTIME_WIN_LARGE_INTEGER;
typedef ULARGE_INTEGER      STDTIME_WIN_ULARGE_INTEGER;

#else /* not _WIN32 */

typedef int16_t             STDTIME_WIN_SHORT;
typedef uint16_t            STDTIME_WIN_USHORT;
typedef uint16_t            STDTIME_WIN_WORD;
typedef uint32_t            STDTIME_WIN_DWORD;
typedef int32_t             STDTIME_WIN_LONG;
typedef uint32_t            STDTIME_WIN_ULONG;
typedef QUADLIB_I64         STDTIME_WIN_LONGLONG;
typedef QUADLIB_U64         STDTIME_WIN_ULONGLONG;
typedef int32_t             STDTIME_WIN_BOOL;

#define STDTIME_WIN_TRUE    1
#define STDTIME_WIN_FALSE   0


#endif /* _WIN32 */


/*****************************************************************************/
/* SYSTEMTIME structure - compatibility definition                           */
/*****************************************************************************/

#ifdef STDTIMEW32_UNIX

typedef struct
  {
  STDTIME_WIN_WORD          wYear;
  STDTIME_WIN_WORD          wMonth;
  STDTIME_WIN_WORD          wDayOfWeek;
  STDTIME_WIN_WORD          wDay;
  STDTIME_WIN_WORD          wHour;
  STDTIME_WIN_WORD          wMinute;
  STDTIME_WIN_WORD          wSecond;
  STDTIME_WIN_WORD          wMilliseconds;
  } STDTIME_WIN_SYSTEMTIME;

#endif /* STDTIMEW32_UNIX */


/*****************************************************************************/
/* VT_DATE compatibility definitions                                         */
/*****************************************************************************/

#ifndef STDTIME_WIN_VT_DATE_DEFINED

typedef uint16_t            STDTIME_WIN_VARTYPE;
typedef double              STDTIME_WIN_DATE;

enum STDTIME_WIN_VARENUM
  {
  STDTIME_WIN_VT_EMPTY      = 0,
  STDTIME_WIN_VT_DATE       = 7,
  STDTIME_WIN_VT_BYREF      = 0x4000
  };

#endif /* STDTIME_WIN_VT_DATE_DEFINED */

/*****************************************************************************/
/* FILETIME structure - compatibility definition                             */
/*****************************************************************************/

#ifdef STDTIMEW32_UNIX

#if (SD_BYTE_ORDER==SD_BIG_ENDIAN)
typedef struct
  {
  STDTIME_WIN_DWORD         dwHighDateTime;
  STDTIME_WIN_DWORD         dwLowDateTime;
  } STDTIME_WIN_FILETIME;
#else	/* SD_LITTLE_ENDIAN	*/
typedef struct
  {
  STDTIME_WIN_DWORD         dwLowDateTime;
  STDTIME_WIN_DWORD         dwHighDateTime;
  } STDTIME_WIN_FILETIME;
#endif	/* SD_LITTLE_ENDIAN	*/

#endif /* STDTIMEW32_UNIX */

/*****************************************************************************/
/* LARGE_INTEGER structure - compatibility definition                        */
/*****************************************************************************/

#ifdef STDTIMEW32_UNIX

typedef union _STDTIME_WIN_LARGE_INTEGER
  {
  /* to maximize compatibility, the anonymous struct is omitted */
  /* references to LARGE_INTEGER fields will require 'u' qualifier */
  /* use of the anonymous struct is a non-portable Microsoft extension. */

  /* the order of LowPart and HighPart should agree with endian */
  /* ordering on the host platform */

  /* if compiled under I64 compatibility, QuadPart is a QuadLib structure */
  /* QUADLIB_I64 and QUADLIB_U64 are the same type, but have different */
  /* names to document their intended usage */

  struct
    {
    STDTIME_WIN_DWORD       LowPart;
    STDTIME_WIN_LONG        HighPart;
    } u;

  QUADLIB_I64               QuadPart;

  } STDTIME_WIN_LARGE_INTEGER;

#endif /* STDTIMEW32_UNIX */

/*****************************************************************************/
/* ULARGE_INTEGER structure - compatibility definition                       */
/*****************************************************************************/

#ifdef STDTIMEW32_UNIX

typedef union _STDTIME_WIN_ULARGE_INTEGER
  {
  /* to maximize compatibility, the anonymous struct is omitted */
  /* references to ULARGE_INTEGER fields will require 'u' qualifier */
  /* use of the anonymous struct is a non-portable Microsoft extension. */

  /* the order of LowPart and HighPart should agree with endian */
  /* ordering on the host platform */

  /* if compiled under I64 compatibility, QuadPart is a QuadLib structure */
  /* QUADLIB_I64 and QUADLIB_U64 are the same type, but have different */
  /* names to document their intended usage */

  struct
    {
    STDTIME_WIN_DWORD       LowPart;
    STDTIME_WIN_DWORD       HighPart;
    } u;

  QUADLIB_U64               QuadPart;

  } STDTIME_WIN_ULARGE_INTEGER;

#endif /* STDTIMEW32_UNIX */


/*****************************************************************************/
/* compatibility-layer API functions                                         */
/*****************************************************************************/

#ifndef STDTIME_API_GETSYSTEMTIMEASFILETIME
#define STDTIME_API_GETSYSTEMTIMEASFILETIME GetSystemTimeAsFileTime

#else
STDTIMEW32_STATIC void              STDTIME_API_GETSYSTEMTIMEASFILETIME (
  STDTIME_WIN_FILETIME *            /*O*/ pFileTime);
#endif


#ifdef STDTIMEW32_ENABLED


STDTIMEW32_STATIC STDTIME_WIN_BOOL  StdTimeW32SystemTimeToFileTime (
  const STDTIME_WIN_SYSTEMTIME *    /*I*/ pSystemTime,
  STDTIME_WIN_FILETIME *            /*O*/ pFileTime);


STDTIMEW32_STATIC STDTIME_WIN_BOOL  StdTimeW32FileTimeToSystemTime (
  const STDTIME_WIN_FILETIME *      /*I*/ pFileTime,
  STDTIME_WIN_SYSTEMTIME *          /*O*/ pSystemTime);


STDTIMEW32_STATIC STDTIME_WIN_BOOL  StdTimeW32FileTimeToLocalFileTime (
  const STDTIME_WIN_FILETIME *      /*I*/ pGmtFileTime,
  STDTIME_WIN_FILETIME *            /*O*/ pLocFileTime);


STDTIMEW32_STATIC STDTIME_WIN_BOOL  StdTimeW32LocalFileTimeToFileTime (
  const STDTIME_WIN_FILETIME *      /*I*/ pLocFileTime,
  STDTIME_WIN_FILETIME *            /*O*/ pGmtFileTime);


#endif /* STDTIMEW32_ENABLED */


/*****************************************************************************/
/*  OBTAIN DAY OF WEEK AND DAY OF WEEK                                       */
/*****************************************************************************/

STDTIME_RC                  StdTimeW32GetDayofWeekAndYear (
  int32_t                   /*I*/ nYear,
  int32_t                   /*I*/ nMon,
  int32_t                   /*I*/ nDayofMon,
  int32_t *                 /*O*/ pDayofWeek,
  int32_t *                 /*O*/ pDayofYear);

/*****************************************************************************/
/*  END                                                                      */
/*****************************************************************************/

#ifdef  __cplusplus
}
#endif


#endif /* _STDTIME_W32_H_29560654_4DD5_47A5_9B07_559DA09B36A3_ */

