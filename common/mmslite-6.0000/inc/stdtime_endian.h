/*
 * Copyright (c) 1997
 * Silicon Graphics Computer Systems, Inc.
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Silicon Graphics makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 */

/*
 * Copyright notice reproduced from <boost/detail/limits.hpp>, from
 * which this code was originally taken.
 *
 * Modified by Caleb Epstein to use <endian.h> with GNU libc and to
 * define the BOOST_ENDIAN macro.
 */

/*
 * Modified 2006-02-25 By SISCO, Inc. from Boost source.
 * source code simplified, made C compliant, and BOOST_BYTE_ORDER removed
 */


#ifndef _STDTIME_ENDIAN_H_9F8D5A2A_B2F9_4F41_A9A0_D2D3912E3082_
#define _STDTIME_ENDIAN_H_9F8D5A2A_B2F9_4F41_A9A0_D2D3912E3082_


#undef HOST_BIG_ENDIAN
#undef HOST_LITTLE_ENDIAN

/* GNU libc offers the helpful header <endian.h> which defines __BYTE_ORDER */

#ifdef __GLIBC__
#include <endian.h>

#if (__BYTE_ORDER == __LITTLE_ENDIAN)           /* 1234 */
#define HOST_LITTLE_ENDIAN
#elif (__BYTE_ORDER == __BIG_ENDIAN)            /* 4321 */
#define HOST_BIG_ENDIAN
#elif (__BYTE_ORDER == __PDP_ENDIAN)
#error PDP_ENDIAN order not supported
#else
#error unknown machine endian order
#endif


#elif   defined(_MIPSEB)                                                      \
    ||  defined(_POWER)                                                       \
    ||  defined(__hppa)                                                       \
    ||  defined(__powerpc__)                                                  \
    ||  defined(__ppc__)                                                      \
    ||  defined(__s390__)                                                     \
    ||  defined(__sparc)                                                      \
    ||  defined(__sparc__)                                                    \
        ||  defined(sun)                                                      \
        ||  defined(_AIX)                                                     \
        ||  defined(VXWORKS)                                                  \
        ||  defined(__hpux)                                                /**/
#define HOST_BIG_ENDIAN


#elif   defined(_M_ALPHA)                                                     \
    ||  defined(_M_IA64)                                                      \
    ||  defined(_M_IX86)                                                      \
    ||  defined(__alpha__)                                                    \
    ||  defined(__i386__)                                                     \
    ||  defined(__ia64)                                                       \
    ||  defined(__ia64__)                                                     \
        ||  defined(MSDOS)                                                    \
        ||  defined(_WIN32)                                                   \
        ||  defined(__LYNX)                                                   \
        ||  defined(__MSDOS__)                                                \
        ||  defined(__OS2__)                                                  \
        ||  defined(__QNX__)                                                  \
        ||  defined(linux)                                                    \
        || (defined(__ALPHA) &&  defined(__VMS))                              \
        || (defined(__alpha) && !defined(__VMS))                           /**/
#define HOST_LITTLE_ENDIAN


#else
#error the file host_endian.h needs to be configured for your CPU type
#endif

#endif /* _STDTIME_ENDIAN_H_9F8D5A2A_B2F9_4F41_A9A0_D2D3912E3082_ */

