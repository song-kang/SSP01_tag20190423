/*****************************************************************************/
/* SISCO SOFTWARE MODULE HEADER **********************************************/
/*****************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,            */
/*            2005 All Rights Reserved                                       */
/*                                                                           */
/* MODULE NAME : stdtimeconfig.h                                             */
/* PRODUCT(S)  : Standard Time Management Library                            */
/*                                                                           */
/* MODULE DESCRIPTION:                                                       */
/*   Configuration of Standard Time Management Library Functions             */
/*   This file is used to set the required symbols to generate a STDTIME     */
/*   Library for a desired environment                                       */
/*                                                                           */
/* MODIFICATION LOG:                                                         */
/*   Date    Who  Rev  Comments                                              */
/* --------  ---  ---  ----------------------------------------------------- */
/* 02/03/10  NAV   07  OpenVMS port					     */
/* 09/16/08  LWP   06  Ported to QNX Neutrino				     */
/* 02/22/08  JRB   05  Add glbtypes.h for SD_BYTE_ORDER & del stdtime_endian.h*/
/* 03/18/07  RLH   04  Add QNX support.                                      */
/* 01/10/07  RLH   03  Add STDTIME_HPDATE_MSEC_ROUNDED define                */
/* 06/07/06  RLH   02  Numerous features added, see stdtime.doc              */
/* 01/18/06  RLH   01  Created                                               */
/*****************************************************************************/


#ifndef _STDTIME_CONFIG_H_7BF1A3F7_495B_4EEE_A2B3_02D65FDF88B6_
#define _STDTIME_CONFIG_H_7BF1A3F7_495B_4EEE_A2B3_02D65FDF88B6_

#include "glbtypes.h"	/* Need this for SD_BYTE_ORDER	*/

#if 0  /* put disabled option inside the #if/#endif block                    */


#define STDTIME_UNICODE                 /* default time strings are UNICODE  */
#define STDTIME_UNICODE_SUBSTITUTE_CHAR '\x1A' /* a value instead of '\x1A'  */
#define STDTIME_TIME_T64_ENABLED        /* if system time_t is 64 bits       */
#define STDTIME_CPP_ONLY_DISABLED       /* if library may be compiled as C   */
#define STDTIME_CPP_ONLY_ENABLED        /* if library only compiled as C++   */
#define STDTIME_MERGED_CSV              /* csv has NO commas between records */
#define STDTIME_LOCAL_USES_SYSAPI       /* how to get local time from GMT    */


#define STDTIME_ISO_DLM_STR  "-T:."     /* default STDTIME_STRING delimiters */
/*      STDTIME_ISO_DLM_STR  must be exactly 4 Ansi characters               */


#define QUADLIB_ENABLED                 /* if 64-bit math is simulated       */
#define QUADLIB_DISABLED                /* if 64-bit math done in hardware   */
#define QUADLIB_NATIVE64_DISABLED       /* if compiler has no 64-bit support */
#define QUADLIB_STATIC  static          /* to make QUADLIB functions local   */
#define QUADLIB_I64N  myI64Type         /* explicit 64-bit signed int type   */
#define QUADLIB_U64N  myUnsignedI64Type /* explicit 64-bit unsigned int type */


#define STDTIMEW32_STATIC               /* makes STDTIMEW32 functions local  */
#define STDTIMEW32_USE_CLOCKGETTIME     /* to use clock_gettime() on Unix    */
#define STDTIMEW32_USE_GETTIMEOFDAY     /* to use gettimeofday() on Unix     */
#define STDTIMEW32_USE_TIMEB            /* to use ftime() and timeb on Unix  */

#endif  /* disabled */

/* put enabled options after this point */

#define STDTIME_TM_ISDST_DEFAULT  (-1)  /* struct tm_isdst default setting   */

#define STDTIME_UNMERGED_CSV            /* csv HAS commas between records    */

#undef  STDTIME_LOCAL_USES_TZDB         /* how to get local time from GMT    */

#define QUADLIB_NATIVE64_ENABLED        /* if compiler has 64-bit support    */
#define STDTIME_HPDATE_MSEC_ROUNDED     /* if HpDate msec are rounded        */

/* include appropriate stdint.h header for target system */

#ifdef _AIX
#define STDTIMEW32_USE_CLOCKGETTIME     /* to use clock_gettime() on Unix    */
#define STDTIMEW32_ENABLED              /* enable WIN32 compatibility layer  */
#define QUADLIB_DISABLED                /* if 64-bit math done in hardware   */
#include "stdint.h"

#elif defined(linux)
#define STDTIMEW32_USE_CLOCKGETTIME     /* to use clock_gettime() on Unix    */
#define STDTIMEW32_ENABLED              /* enable WIN32 compatibility layer  */
#define QUADLIB_DISABLED                /* if 64-bit math done in hardware   */
#include "stdint.h"

#elif defined(sun)
#define STDTIMEW32_USE_CLOCKGETTIME     /* to use clock_gettime() on Unix    */
#define STDTIMEW32_ENABLED              /* enable WIN32 compatibility layer  */
#define QUADLIB_DISABLED                /* if 64-bit math done in hardware   */
#include <sys/int_types.h>

#elif defined(__alpha) && !defined(__VMS)  /* digital unix                   */
#define STDTIMEW32_USE_CLOCKGETTIME     /* to use clock_gettime() on Unix    */
#define STDTIMEW32_ENABLED              /* enable WIN32 compatibility layer  */
#define QUADLIB_DISABLED                /* if 64-bit math done in hardware   */
#include <inttypes.h>

#elif defined(__VMS)
#define STDTIMEW32_USE_CLOCKGETTIME     /* to use clock_gettime() on Unix    */
#define STDTIMEW32_ENABLED              /* enable WIN32 compatibility layer  */
#define QUADLIB_DISABLED                /* if 64-bit math done in hardware   */
#include <inttypes.h>

#elif defined(_WIN32)
#define STDTIMEW32_DISABLED             /* disable WIN32 compatibility layer */
#define QUADLIB_DISABLED                /* if 64-bit math done in hardware   */
#include "stdtime_stdint.h"

#elif (defined(__QNX__) && !defined(__QNXNTO__))
#define STDTIMEW32_USE_CLOCKGETTIME     /* to use clock_gettime() on QNX     */
#define STDTIMEW32_ENABLED              /* enable WIN32 compatibility layer  */
#undef  QUADLIB_DISABLED                /* if 64-bit math done in hardware   */
#define QUADLIB_ENABLED                 /* if 64-bit math is simulated       */
#undef  QUADLIB_NATIVE64_ENABLED        /* if compiler has 64-bit support    */
#define QUADLIB_NATIVE64_DISABLED       /* if compiler has no 64-bit support */

/* QNX has no stdint.h or equivalent */
/* bare minimum definitions located here */

typedef char                int8_t;
typedef short               int16_t;
typedef int                 int32_t;

typedef unsigned char       uint8_t;
typedef unsigned short      uint16_t;
typedef unsigned int        uint32_t;

#elif defined(__QNXNTO__)
#define STDTIMEW32_USE_CLOCKGETTIME     /* to use clock_gettime() on Unix    */
#define STDTIMEW32_ENABLED              /* enable WIN32 compatibility layer  */
#define QUADLIB_DISABLED                /* if 64-bit math done in hardware   */
#include <stdint.h>

#else
#define STDTIMEW32_DISABLED             /* disable WIN32 compatibility layer */
#include "stdtime_stdint.h"
#endif

#endif /* _STDTIME_CONFIG_H_7BF1A3F7_495B_4EEE_A2B3_02D65FDF88B6_ */

