/*****************************************************************************/
/* SISCO SOFTWARE MODULE HEADER **********************************************/
/*****************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,            */
/*            2005 All Rights Reserved                                       */
/*                                                                           */
/* MODULE NAME : stdint.h                                                    */
/* PLATFORM    : Microsoft Windows W32 platform                              */
/* PRODUCT(S)  : general usage                                               */
/*                                                                           */
/* MODULE DESCRIPTION:                                                       */
/*  Implementation of C99 standard header file "stdint.h"                    */
/*                                                                           */
/*  Implementation is based on ISO/IEC 9899:1999 TC2 Programming Language C  */
/*                                                                           */
/* MODIFICATION LOG:                                                         */
/*   Date    Who  Rev  Comments                                              */
/* --------  ---  ---  ----------------------------------------------------- */
/* 06/07/06  RLH   02  Numerous features added, see stdtime.doc              */
/* 02/25/06  RLH   01  Created                                               */
/*****************************************************************************/

/* STDINT.H for Microsoft Windows */


#ifndef _STDTIME_STDINT_H_1BA49070_CC19_4880_8283_69F38C4F8F59_
#define _STDTIME_STDINT_H_1BA49070_CC19_4880_8283_69F38C4F8F59_


#include <limits.h>
#include <stddef.h>
#include <wchar.h>

#ifdef  __cplusplus
extern "C" {
#endif

/* exact-wide integer types */

typedef          __int8     int8_t;
typedef          __int16    int16_t;
typedef          __int32    int32_t;
typedef          __int64    int64_t;

typedef unsigned __int8     uint8_t;
typedef unsigned __int16    uint16_t;
typedef unsigned __int32    uint32_t;
typedef unsigned __int64    uint64_t;

/* minimum-wide integer types */

typedef          __int8     int_least8_t;
typedef          __int16    int_least16_t;
typedef          __int32    int_least32_t;
typedef          __int64    int_least64_t;

typedef unsigned __int8     uint_least8_t;
typedef unsigned __int16    uint_least16_t;
typedef unsigned __int32    uint_least32_t;
typedef unsigned __int64    uint_least64_t;

/* fastest minimum-wide integer types */

typedef          __int8     int_fast8_t;
typedef          __int16    int_fast16_t;
typedef          __int32    int_fast32_t;
typedef          __int64    int_fast64_t;

typedef unsigned __int8     uint_fast8_t;
typedef unsigned __int16    uint_fast16_t;
typedef unsigned __int32    uint_fast32_t;
typedef unsigned __int64    uint_fast64_t;

/* integer types capable of holding object pointers */

/* intptr_t, uintptr_t : see stddef.h */

/* greatest-width integer types */

typedef          __int64    intmax_t;
typedef unsigned __int64    uintmax_t;


/* macros for limits of exact-width integer types */
/* only define in C++ if user-requested per C99 standard */

#if (!defined(__cplusplus)) || defined(__STDC_LIMIT_MACROS)

/* fundamental limits : see limits.h */

#define INT8_MIN            ((int8_t)(SCHAR_MIN))
#define INT8_MAX            ((int8_t)(SCHAR_MAX))
#define UINT8_MAX           ((uint8_t)(UCHAR_MAX))

#define INT16_MIN           ((int16_t)(SHRT_MIN))
#define INT16_MAX           ((int16_t)(SHRT_MAX))
#define UINT16_MAX          ((uint16_t)(USHRT_MAX))

#define INT32_MIN           ((int32_t)(INT_MIN))
#define INT32_MAX           ((int32_t)(INT_MAX))
#define UINT32_MAX          ((uint32_t)(UINT_MAX))

#define INT64_MIN           ((int64_t)(_I64_MIN))
#define INT64_MAX           ((int64_t)(_I64_MAX))
#define UINT64_MAX          ((uint64_t)(_UI64_MAX))

/* limits of minimum-width integer types */

#define INT_LEAST8_MIN      INT8_MIN
#define INT_LEAST8_MAX      INT8_MAX
#define UINT_LEAST8_MAX     UINT8_MAX

#define INT_LEAST16_MIN     INT16_MIN
#define INT_LEAST16_MAX     INT16_MAX
#define UINT_LEAST16_MAX    UINT16_MAX

#define INT_LEAST32_MIN     INT32_MIN
#define INT_LEAST32_MAX     INT32_MAX
#define UINT_LEAST32_MAX    UINT32_MAX

#define INT_LEAST64_MIN     INT64_MIN
#define INT_LEAST64_MAX     INT64_MAX
#define UINT_LEAST64_MAX    UINT64_MAX

/* limits of fastest minimum-width integer types */

#define INT_FAST8_MIN       INT8_MIN
#define INT_FAST8_MAX       INT8_MAX
#define UINT_FAST8_MAX      UINT8_MAX

#define INT_FAST16_MIN      INT16_MIN
#define INT_FAST16_MAX      INT16_MAX
#define UINT_FAST16_MAX     UINT16_MAX

#define INT_FAST32_MIN      INT32_MIN
#define INT_FAST32_MAX      INT32_MAX
#define UINT_FAST32_MAX     UINT32_MAX

#define INT_FAST64_MIN      INT64_MIN
#define INT_FAST64_MAX      INT64_MAX
#define UINT_FAST64_MAX     UINT64_MAX

/* limits of greatest-width integer types */

#define INTMAX_MIN          INT64_MIN
#define INTMAX_MAX          INT64_MAX
#define UINTMAX_MAX         UINT64_MAX

/* limits of other integer types */

#ifndef PTRDIFF_MIN
#ifdef  _WIN64
#define PTRDIFF_MIN         INT64_MIN
#else
#define PTRDIFF_MIN         INT32_MIN
#endif
#endif

#ifndef PTRDIFF_MAX
#ifdef  _WIN64
#define PTRDIFF_MAX         INT64_MAX
#else
#define PTRDIFF_MAX         INT32_MAX
#endif
#endif

#ifndef SIG_ATOMIC_MIN
#define SIG_ATOMIC_MIN      INT32_MIN
#endif

#ifndef SIG_ATOMIC_MAX
#define SIG_ATOMIC_MAX      INT32_MAX
#endif

#ifndef SIZE_MAX
#ifdef  _WIN64
#define SIZE_MAX            UINT64_MAX
#else
#define SIZE_MAX            UINT32_MAX
#endif
#endif

#ifndef WCHAR_MIN
#define WCHAR_MIN           ((wchar_t)(0))
#endif

#ifndef WCHAR_MAX
#define WCHAR_MAX           ((wchar_t)(USHRT_MAX))
#endif

#ifndef WINT_MIN
#define WINT_MIN            ((wint_t)(0))
#endif

#ifndef WINT_MAX
#define WINT_MAX            ((wint_t)(USHRT_MAX))
#endif

#endif /* (!defined(__cplusplus)) || defined(__STDC_LIMIT_MACROS) */


/* macros for minimum-width integer constants */
/* only define in C++ if user-requested per C99 standard */

#if (!defined(__cplusplus)) || defined(__STDC_CONSTANT_MACROS)

#define INT8_C(x)           (int8_t)(x)
#define INT16_C(x)          (int16_t)(x)
#define INT32_C(x)          (int32_t)(x)
#define INT64_C(x)          (int64_t)(x##i64)

#define UINT8_C(x)          (uint8_t)(x##u)
#define UINT16_C(x)         (uint16_t)(x##u)
#define UINT32_C(x)         (uint32_t)(x##u)
#define UINT64_C(x)         (uint64_t)(x##ui64)

#define INTMAX_C(x)         INT64_C(x)
#define UINTMAX_C(x)        UINT64_C(x)

#endif /* (!defined(__cplusplus)) || defined(__STDC_CONSTANT_MACROS) */


#ifdef  __cplusplus
}
#endif

#endif /* _STDTIME_STDINT_H_1BA49070_CC19_4880_8283_69F38C4F8F59_ */

