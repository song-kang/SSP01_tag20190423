/*****************************************************************************/
/* SISCO SOFTWARE MODULE HEADER **********************************************/
/*****************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,            */
/*            2005 All Rights Reserved                                       */
/*                                                                           */
/* MODULE NAME : QuadLib.h                                                   */
/* PRODUCT(S)  : Standard Time Management Library                            */
/*                                                                           */
/* MODULE DESCRIPTION:                                                       */
/*   64-bit Math Simulation Library - header file                            */
/*   Provided for platforms without native 64-bit support.                   */
/*                                                                           */
/*   When QUADLIB_ENABLED is defined, definitions relevant to the            */
/*   simulations library are generated.  Otherwise, definitions are          */
/*   selected to develop code on 64-bit enabled platforms without going      */
/*   through the QuadLib library.                                            */
/*                                                                           */
/*   Define QUADLIB_STATIC as 'static' to make the declarations local.       */
/*                                                                           */
/*   Define QUADLIB_I64N as the native signed 64-bit data type.              */
/*   Define QUADLIB_U64N as the native unsigned 64-bit data type.            */
/*   If not defined, an assumption is made based on existence of _WIN32.     */
/*                                                                           */
/*   The include file "host_endian.h" defines the native host endian order.  */
/*                                                                           */
/* MODIFICATION LOG:                                                         */
/*   Date    Who  Rev  Comments                                              */
/* --------  ---  ---  ----------------------------------------------------- */
/* 02/22/08  JRB   03  Use SD_BYTE_ORDER like all other SISCO code.          */
/* 06/07/06  RLH   02  Numerous features added, see stdtime.doc              */
/* 01/18/06  RLH   01  Created                                               */
/*****************************************************************************/


#ifndef _STDTIME_QUADLIB_H_B07CB807_8BFC_41B3_872E_EDD251FD6AEB_
#define _STDTIME_QUADLIB_H_B07CB807_8BFC_41B3_872E_EDD251FD6AEB_


#ifdef  __cplusplus
extern "C" {
#endif

/*****************************************************************************/
/*  ENABLED or DISABLED: SUPPORT FOR 64-BIT EMULATION                        */
/*                                                                           */
/*  use StdTimeConfig.h to set QUADLIB_ENABLED or QUADLIB_DISABLED           */
/*****************************************************************************/

#if defined(QUADLIB_ENABLED) && defined(QUADLIB_DISABLED)
#error Both QUADLIB_ENABLED and QUADLIB_DISABLED defined
#undef QUADLIB_ENABLED
#endif

#if !defined(QUADLIB_ENABLED) && !defined(QUADLIB_DISABLED)
#define QUADLIB_DISABLED            /* default */
#endif

/*****************************************************************************/
/*  ENABLED or DISABLED: EXISTENCE OF 64-BIT SUPPORT IN COMPILER             */
/*                                                                           */
/*  use StdTimeConfig.h to set QUADLIB_NATIVE64_ENABLED or                   */
/*  QUADLIB_NATIVE64_DISABLED                                                */
/*****************************************************************************/

/* these defines are used mainly to avoid compile errors on platforms */
/* that have no support at all for 64-bit types */

#if defined(QUADLIB_NATIVE64_ENABLED) && defined(QUADLIB_NATIVE64_DISABLED)
#error Both QUADLIB_NATIVE64_ENABLED and QUADLIB_NATIVE64_DISABLED defined
#undef QUADLIB_NATIVE64_ENABLED
#undef QUADLIB_NATIVE64_DISABLED
#endif

#if !defined(QUADLIB_NATIVE64_ENABLED) && !defined(QUADLIB_NATIVE64_DISABLED)
#ifdef _WIN32
#define QUADLIB_NATIVE64_ENABLED
#else
#define QUADLIB_NATIVE64_DISABLED
#endif
#endif

/*****************************************************************************/
/*  VALIDATE NATIVE SUPPORT VS. EMULATION SUPPORT                            */
/*****************************************************************************/

/* either the host must support 64-bit mode or we must emulate it */
/* otherwise, the code that manages 64-bit FILETIME values will not work */

#if defined(QUADLIB_DISABLED) && defined(QUADLIB_NATIVE64_DISABLED)
#error QUADLIB_DISABLED and QUADLIB_NATIVE64_DISABLED, cannot generate code
#endif


/*****************************************************************************/
/*  STATIC (LOCAL) FUNCTION QUALIFIER                                        */
/*                                                                           */
/*  use StdTimeConfig.h to set QUADLIB_STATIC                                */
/*****************************************************************************/

#ifndef QUADLIB_STATIC
#define QUADLIB_STATIC
#endif

/* stdtimeconfig.h includes appropriate stdint.h header for 32/64 bit types */

#define QUADLIB_U32_HIGHBIT         0x80000000

/*****************************************************************************/
/*  NATIVE 64 BIT TYPES                                                      */
/*****************************************************************************/

#ifdef QUADLIB_NATIVE64_ENABLED
#define QUADLIB_I64N                int64_t
#define QUADLIB_U64N                uint64_t

#else /* not QUADLIB_NATIVE64_ENABLED */

/* host has NO 64-bit data types */
/* the following defines are just for compatibility */
/* but lack 64-bit capacity. _WIN32 always supports I64 */
/* so this is just a non-Windows issue. */

#ifndef QUADLIB_I64N
#define QUADLIB_I64N                int32_t         /* 32-bit */
#endif

#ifndef QUADLIB_U64N
#define QUADLIB_U64N                uint32_t        /* 32-bit */
#endif

#endif /* QUADLIB_NATIVE64_ENABLED */

/*****************************************************************************/
/*  QUADLIB_I64 - hi/lo member ordering based on ENDIAN definition above     */
/*  QUADLIB_U64 has the same representation as QUADLIB_I64                   */
/*  two names are used mainly to document the intended usage                 */
/*****************************************************************************/

/* Make sure SD_BYTE_ORDER is defined (in glbtypes.h)	*/
#if !defined(SD_BYTE_ORDER)
#error SD_BYTE_ORDER not defined
#endif
#if (SD_BYTE_ORDER==SD_LITTLE_ENDIAN)

typedef struct
  {
  union
    {
    int32_t                         i;
    uint32_t                        u;
    } lo;

  union
    {
    int32_t                         i;
    uint32_t                        u;
    } hi;
  }
  QUADLIB_I64_TYPE;

#else /* BIG_ENDIAN */

typedef struct
  {
  union
    {
    int32_t                         i;
    uint32_t                        u;
    } hi;

  union
    {
    int32_t                         i;
    uint32_t                        u;
    } lo;
  }
  QUADLIB_I64_TYPE;

#endif /* BIG_ENDIAN */

#define QUADLIB_U64_TYPE            QUADLIB_I64_TYPE

#ifdef QUADLIB_ENABLED

/* enabled means 64-bit operations are simulated in software */

#define QUADLIB_I64                 QUADLIB_I64_TYPE
#define QUADLIB_U64                 QUADLIB_I64_TYPE
#define QUADLIB_ZERO                {0,0}
#define QUADLIB_U64_MAX             {0xFFFFFFFF, 0xFFFFFFFF}

#if (SD_BYTE_ORDER==SD_LITTLE_ENDIAN)

#define QUADLIB_I64_MAX             {0xFFFFFFFF, 0x7FFFFFFF}
#define QUADLIB_I64_MIN             {0x00000000, 0x80000000}

#else /* BIG_ENDIAN */

#define QUADLIB_I64_MAX             {0x7FFFFFFF, 0xFFFFFFFF}
#define QUADLIB_I64_MIN             {0x00000000, 0x00000000}

#endif /* BIG_ENDIAN */

#else /* not QUADLIB_ENABLED */

/* use native 64-bit values for a few compatibility functions */

#define QUADLIB_I64                 QUADLIB_I64N
#define QUADLIB_U64                 QUADLIB_U64N
#define QUADLIB_ZERO                0
#define QUADLIB_U64_MAX             (QUADLIB_U64)0xFFFFFFFFFFFFFFFF
#define QUADLIB_I64_MAX             (QUADLIB_I64)0x7FFFFFFFFFFFFFFF
#define QUADLIB_I64_MIN             (QUADLIB_I64)0x8000000000000000

#endif /* QUADLIB_ENABLED */


#ifdef _WCHAR_T_DEFINED
typedef wchar_t             QUADLIB_WCHAR;
#else
typedef unsigned short      QUADLIB_WCHAR;
#endif

#define QUADLIB_STRING_LEN  20


typedef struct
  {
  char                      str [QUADLIB_STRING_LEN+1];
  }
QUADLIB_STRINGA;


typedef struct
  {
  QUADLIB_WCHAR             str [QUADLIB_STRING_LEN+1];
  }
QUADLIB_STRINGW;


/*****************************************************************************/
/*  NOT                                                                      */
/*****************************************************************************/

#ifdef QUADLIB_ENABLED

#define QUADLIB_I64_NOT(x)          QuadLibI64Not(x)
#define QUADLIB_U64_NOT(x)          QuadLibU64Not(x)

#define QuadLibI64Not(x)            QuadLibU64Not(x)

QUADLIB_STATIC QUADLIB_U64          QuadLibU64Not (QUADLIB_U64 x);

#else /* not QUADLIB_ENABLED */

#define QUADLIB_I64_NOT(x)          (~x)
#define QUADLIB_U64_NOT(x)          (~x)

#endif /* QUADLIB_ENABLED */

/*****************************************************************************/
/*  AND                                                                      */
/*****************************************************************************/

#ifdef QUADLIB_ENABLED

#define QUADLIB_I64_AND(x,y)        QuadLibI64And(x,y)
#define QUADLIB_U64_AND(x,y)        QuadLibU64And(x,y)
#define QUADLIB_I64_AND_EQ(x,y)     x = QuadLibI64And(x,y)
#define QUADLIB_U64_AND_EQ(x,y)     x = QuadLibU64And(x,y)

#define QUADLIB_I64_ANDNOT(x,y)     QuadLibI64AndNot(x,y)
#define QUADLIB_U64_ANDNOT(x,y)     QuadLibU64AndNot(x,y)
#define QUADLIB_I64_ANDNOT_EQ(x,y)  x = QuadLibI64AndNot(x,y)
#define QUADLIB_U64_ANDNOT_EQ(x,y)  x = QuadLibU64AndNot(x,y)

#define QuadLibI64And(x,y)          QuadLibU64And(x,y)
#define QuadLibI64AndNot(x)         QuadLibU64AndNot(x)

QUADLIB_STATIC QUADLIB_U64          QuadLibU64And (
  QUADLIB_U64                       /*I*/ one,
  QUADLIB_U64                       /*I*/ two);

QUADLIB_STATIC QUADLIB_U64          QuadLibU64AndNot (
  QUADLIB_U64                       /*I*/ one,
  QUADLIB_U64                       /*I*/ two);

#else /* not QUADLIB_ENABLED */

#define QUADLIB_I64_AND(x,y)        (x & y)
#define QUADLIB_U64_AND(x,y)        (x & y)
#define QUADLIB_I64_AND_EQ(x,y)     x &= y
#define QUADLIB_U64_AND_EQ(x,y)     x &= y

#define QUADLIB_I64_ANDNOT(x,y)     (x & (~y))
#define QUADLIB_U64_ANDNOT(x,y)     (x & (~y))
#define QUADLIB_I64_ANDNOT_EQ(x,y)  x = x & (~y)
#define QUADLIB_U64_ANDNOT_EQ(x,y)  x = x & (~y)

#endif /* QUADLIB_ENABLED */

/*****************************************************************************/
/*  OR                                                                       */
/*****************************************************************************/

#ifdef QUADLIB_ENABLED

#define QUADLIB_I64_OR(x,y)         QuadLibI64Or(x,y)
#define QUADLIB_U64_OR(x,y)         QuadLibU64Or(x,y)
#define QUADLIB_I64_OR_EQ(x,y)      x = QuadLibI64Or(x,y)
#define QUADLIB_U64_OR_EQ(x,y)      x = QuadLibU64Or(x,y)

#define QUADLIB_I64_ORNOT(x,y)      QuadLibI64OrNot(x,y)
#define QUADLIB_U64_ORNOT(x,y)      QuadLibU64OrNot(x,y)
#define QUADLIB_I64_ORNOT_EQ(x,y)   x = QuadLibI64OrNot(x,y)
#define QUADLIB_U64_ORNOT_EQ(x,y)   x = QuadLibU64OrNot(x,y)

#define QuadLibI64Or(x,y)           QuadLibU64Or(x,y)
#define QuadLibI64OrNot(x)          QuadLibU64OrNot(x)

QUADLIB_STATIC QUADLIB_U64          QuadLibU64Or (
  QUADLIB_U64                       /*I*/ one,
  QUADLIB_U64                       /*I*/ two);

QUADLIB_STATIC QUADLIB_U64          QuadLibU64OrNot (
  QUADLIB_U64                       /*I*/ one,
  QUADLIB_U64                       /*I*/ two);

#else /* not QUADLIB_ENABLED */

#define QUADLIB_I64_OR(x,y)         (x | y)
#define QUADLIB_U64_OR(x,y)         (x | y)
#define QUADLIB_I64_OR_EQ(x,y)      x |= y
#define QUADLIB_U64_OR_EQ(x,y)      x |= y

#define QUADLIB_I64_ORNOT(x,y)      (x | (~y))
#define QUADLIB_U64_ORNOT(x,y)      (x | (~y))
#define QUADLIB_I64_ORNOT_EQ(x,y)   x = x | (~y)
#define QUADLIB_U64_ORNOT_EQ(x,y)   x = x | (~y)

#endif /* QUADLIB_ENABLED */

/*****************************************************************************/
/*  XOR                                                                      */
/*****************************************************************************/

#ifdef QUADLIB_ENABLED

#define QUADLIB_I64_XOR(x,y)        QuadLibI64Xor(x,y)
#define QUADLIB_U64_XOR(x,y)        QuadLibU64Xor(x,y)
#define QUADLIB_I64_XOR_EQ(x,y)     x = QuadLibI64Xor(x,y)
#define QUADLIB_U64_XOR_EQ(x,y)     x = QuadLibU64Xor(x,y)

#define QUADLIB_I64_XORNOT(x,y)     QuadLibI64XorNot(x,y)
#define QUADLIB_U64_XORNOT(x,y)     QuadLibU64XorNot(x,y)
#define QUADLIB_I64_XORNOT_EQ(x,y)  x = QuadLibI64XorNot(x,y)
#define QUADLIB_U64_XORNOT_EQ(x,y)  x = QuadLibU64XorNot(x,y)

#define QuadLibI64Xor(x,y)          QuadLibU64Xor(x,y)
#define QuadLibI64XorNot(x)         QuadLibU64XorNot(x)

QUADLIB_STATIC QUADLIB_U64          QuadLibU64Xor (
  QUADLIB_U64                       /*I*/ one,
  QUADLIB_U64                       /*I*/ two);

QUADLIB_STATIC QUADLIB_U64          QuadLibU64XorNot (
  QUADLIB_U64                       /*I*/ one,
  QUADLIB_U64                       /*I*/ two);

#else /* not QUADLIB_ENABLED */

#define QUADLIB_I64_XOR(x,y)        (x ^ y)
#define QUADLIB_U64_XOR(x,y)        (x ^ y)
#define QUADLIB_I64_XOR_EQ(x,y)     x ^= y
#define QUADLIB_U64_XOR_EQ(x,y)     x ^= y

#define QUADLIB_I64_XORNOT(x,y)     (x ^ (~y))
#define QUADLIB_U64_XORNOT(x,y)     (x ^ (~y))
#define QUADLIB_I64_XORNOT_EQ(x,y)  x = x ^ (~y)
#define QUADLIB_U64_XORNOT_EQ(x,y)  x = x ^ (~y)

#endif /* QUADLIB_ENABLED */

/*****************************************************************************/
/*  ADD                                                                      */
/*****************************************************************************/

#ifdef QUADLIB_ENABLED

#define QUADLIB_I64_ADD(x,y)        QuadLibI64Add(x,y)
#define QUADLIB_U64_ADD(x,y)        QuadLibU64Add(x,y)
#define QUADLIB_I64_ADD_EQ(x,y)     x = QuadLibI64Add(x,y)
#define QUADLIB_U64_ADD_EQ(x,y)     x = QuadLibU64Add(x,y)

#define QUADLIB_I64_ADD1(x)         QuadLibI64Add1(x)
#define QUADLIB_U64_ADD1(x)         QuadLibU64Add1(x)
#define QUADLIB_I64_ADD1_EQ(x)      x = QuadLibI64Add1(x)
#define QUADLIB_U64_ADD1_EQ(x)      x = QuadLibU64Add1(x)

#define QuadLibI64Add(x,y)          QuadLibU64Add(x,y)
#define QuadLibI64Add1(x)           QuadLibU64Add1(x)

QUADLIB_STATIC QUADLIB_U64          QuadLibU64Add (
  QUADLIB_U64                       /*I*/ one,
  QUADLIB_U64                       /*I*/ two);

QUADLIB_STATIC QUADLIB_U64          QuadLibU64Add1 (
  QUADLIB_U64                       /*I*/ x);

#else /* not QUADLIB_ENABLED */

#define QUADLIB_I64_ADD(x,y)        (x + y)
#define QUADLIB_U64_ADD(x,y)        (x + y)
#define QUADLIB_I64_ADD_EQ(x,y)     x += y
#define QUADLIB_U64_ADD_EQ(x,y)     x += y

#define QUADLIB_I64_ADD1(x)         (x + y)
#define QUADLIB_U64_ADD1(x)         (x + y)
#define QUADLIB_I64_ADD1_EQ(x)      x++
#define QUADLIB_U64_ADD1_EQ(x)      x++

#endif /* QUADLIB_ENABLED */

/*****************************************************************************/
/*  SUB                                                                      */
/*****************************************************************************/

#ifdef QUADLIB_ENABLED

#define QUADLIB_I64_SUB(x,y)        QuadLibI64Sub(x,y)
#define QUADLIB_U64_SUB(x,y)        QuadLibU64Sub(x,y)
#define QUADLIB_I64_SUB_EQ(x,y)     x = QuadLibI64Sub(x,y)
#define QUADLIB_U64_SUB_EQ(x,y)     x = QuadLibU64Sub(x,y)

#define QUADLIB_I64_SUB1(x)         QuadLibI64Sub1(x)
#define QUADLIB_U64_SUB1(x)         QuadLibU64Sub1(x)
#define QUADLIB_I64_SUB1_EQ(x)      x = QuadLibI64Sub1(x)
#define QUADLIB_U64_SUB1_EQ(x)      x = QuadLibU64Sub1(x)

#define QuadLibI64Sub(x,y)          QuadLibU64Sub(x,y)
#define QuadLibI64Sub1(x)           QuadLibU64Sub1(x)

QUADLIB_STATIC QUADLIB_U64          QuadLibU64Sub (
  QUADLIB_U64                       /*I*/ one,
  QUADLIB_U64                       /*I*/ two);

QUADLIB_STATIC QUADLIB_U64          QuadLibU64Sub1 (
  QUADLIB_U64                       /*I*/ x);

#else /* not QUADLIB_ENABLED */

#define QUADLIB_I64_SUB(x,y)        (x - y)
#define QUADLIB_U64_SUB(x,y)        (x - y)
#define QUADLIB_I64_SUB_EQ(x,y)     x -= y
#define QUADLIB_U64_SUB_EQ(x,y)     x -= y

#define QUADLIB_I64_SUB1(x)         (x - y)
#define QUADLIB_U64_SUB1(x)         (x - y)
#define QUADLIB_I64_SUB1_EQ(x)      x--
#define QUADLIB_U64_SUB1_EQ(x)      x--

#endif /* QUADLIB_ENABLED */

/*****************************************************************************/
/*  NEG                                                                      */
/*****************************************************************************/

#ifdef QUADLIB_ENABLED

#define QUADLIB_I64_NEG(x)          QuadLibI64Neg(x)
#define QUADLIB_U64_NEG(x)          QuadLibU64Neg(x)

QUADLIB_STATIC QUADLIB_U64          QuadLibU64Neg (QUADLIB_U64 x);

#else /* not QUADLIB_ENABLED */

#define QUADLIB_I64_NEG(x)          (-x)
#define QUADLIB_U64_NEG(x)          (-x)

#endif /* QUADLIB_ENABLED */

#define QuadLibI64Neg(x)            QuadLibU64Neg(x)

/*****************************************************************************/
/*  ABS                                                                      */
/*****************************************************************************/

#ifdef QUADLIB_ENABLED

#define QUADLIB_I64_ABS(x)          QuadLibI64Abs(x)
#define QUADLIB_U64_ABS(x)          QuadLibU64Abs(x)

#else /* not QUADLIB_ENABLED */

#define QUADLIB_I64_ABS(x)          ((x) >= 0 ? (x) : -(x))
#define QUADLIB_U64_ABS(x)          ((x) >= 0 ? (x) : -(x))

#endif /* QUADLIB_ENABLED */

#define QuadLibI64Abs(x)            QuadLibU64Abs(x)

/* quad abs function always defined */

QUADLIB_STATIC QUADLIB_U64          QuadLibU64Abs (QUADLIB_U64 x);

/*****************************************************************************/
/*  CMP                                                                      */
/*****************************************************************************/

#ifdef QUADLIB_ENABLED

#define QUADLIB_I64_EQ(x,y)         QuadLibI64EQ(x,y)
#define QUADLIB_I64_NE(x,y)         QuadLibI64NE(x,y)
#define QUADLIB_I64_GT(x,y)         QuadLibI64GT(x,y)
#define QUADLIB_I64_GE(x,y)         QuadLibI64GE(x,y)
#define QUADLIB_I64_LT(x,y)         QuadLibI64LT(x,y)
#define QUADLIB_I64_LE(x,y)         QuadLibI64LE(x,y)

#define QUADLIB_U64_EQ(x,y)         QuadLibU64EQ(x,y)
#define QUADLIB_U64_NE(x,y)         QuadLibU64NE(x,y)
#define QUADLIB_U64_GT(x,y)         QuadLibU64GT(x,y)
#define QUADLIB_U64_GE(x,y)         QuadLibU64GE(x,y)
#define QUADLIB_U64_LT(x,y)         QuadLibU64LT(x,y)
#define QUADLIB_U64_LE(x,y)         QuadLibU64LE(x,y)

#define QuadLibI64EQ(x,y)           (QuadLibI64Cmp(x,y)==0)
#define QuadLibI64NE(x,y)           (QuadLibI64Cmp(x,y)!=0)
#define QuadLibI64GT(x,y)           (QuadLibI64Cmp(x,y)> 0)
#define QuadLibI64GE(x,y)           (QuadLibI64Cmp(x,y)>=0)
#define QuadLibI64LT(x,y)           (QuadLibI64Cmp(x,y)< 0)
#define QuadLibI64LE(x,y)           (QuadLibI64Cmp(x,y)<=0)

#define QuadLibU64EQ(x,y)           (QuadLibU64Cmp(x,y)==0)
#define QuadLibU64NE(x,y)           (QuadLibU64Cmp(x,y)!=0)
#define QuadLibU64GT(x,y)           (QuadLibU64Cmp(x,y)> 0)
#define QuadLibU64GE(x,y)           (QuadLibU64Cmp(x,y)>=0)
#define QuadLibU64LT(x,y)           (QuadLibU64Cmp(x,y)< 0)
#define QuadLibU64LE(x,y)           (QuadLibU64Cmp(x,y)<=0)

QUADLIB_STATIC int32_t              QuadLibU64Cmp (
                                    /*I*/ QUADLIB_U64 x,
                                    /*I*/ QUADLIB_U64 y);

QUADLIB_STATIC int32_t              QuadLibI64Cmp (
                                    /*I*/ QUADLIB_I64 x,
                                    /*I*/ QUADLIB_I64 y);

#define QUADLIB_I64_EQ_0(x)     (! QUADLIB_I64_NE_0(x))
#define QUADLIB_I64_NE_0(x)     (QUADLIB_U64_HI(x) | QUADLIB_U64_LO(x))
#define QUADLIB_I64_GT_0(x)     (QUADLIB_I64_GE_0(x) && QUADLIB_I64_NE_0(x))
#define QUADLIB_I64_GE_0(x)     (QUADLIB_I64_HI(x) >= 0)
#define QUADLIB_I64_LT_0(x)     (QUADLIB_I64_HI(x) < 0)
#define QUADLIB_I64_LE_0(x)     (QUADLIB_I64_LT_0(x) || QUADLIB_I64_EQ_0(x))

#define QUADLIB_U64_EQ_0(x)     (! QUADLIB_U64_NE_0(x))
#define QUADLIB_U64_NE_0(x)     (QUADLIB_U64_HI(x) | QUADLIB_U64_LO(x))
#define QUADLIB_U64_GT_0(x)     (QUADLIB_U64_GE_0(x) && QUADLIB_U64_NE_0(x))
#define QUADLIB_U64_GE_0(x)     (QUADLIB_U64_HI(x) >= 0)

#if 0
#define QUADLIB_U64_LT_0(x)     (QUADLIB_U64_HI(x) < 0)
#define QUADLIB_U64_LE_0(x)     (QUADLIB_U64_LT_0(x) || QUADLIB_U64_EQ_0(x))
#endif

#else /* not QUADLIB_ENABLED */

#define QUADLIB_I64_EQ(x,y)         ((x) == (y))
#define QUADLIB_I64_NE(x,y)         ((x) != (y))
#define QUADLIB_I64_GT(x,y)         ((x) >  (y))
#define QUADLIB_I64_GE(x,y)         ((x) >= (y))
#define QUADLIB_I64_LT(x,y)         ((x) <  (y))
#define QUADLIB_I64_LE(x,y)         ((x) <= (y))

#define QUADLIB_U64_EQ(x,y)         ((x) == (y))
#define QUADLIB_U64_NE(x,y)         ((x) != (y))
#define QUADLIB_U64_GT(x,y)         ((x) >  (y))
#define QUADLIB_U64_GE(x,y)         ((x) >= (y))
#define QUADLIB_U64_LT(x,y)         ((x) <  (y))
#define QUADLIB_U64_LE(x,y)         ((x) <= (y))

#define QUADLIB_I64_EQ_0(x)         ((x) == 0)
#define QUADLIB_I64_NE_0(x)         ((x) != 0)
#define QUADLIB_I64_GT_0(x)         ((x) >  0)
#define QUADLIB_I64_GE_0(x)         ((x) >= 0)
#define QUADLIB_I64_LT_0(x)         ((x) <  0)
#define QUADLIB_I64_LE_0(x)         ((x) <= 0)

#define QUADLIB_U64_EQ_0(x)         ((x) == 0)
#define QUADLIB_U64_NE_0(x)         ((x) != 0)
#define QUADLIB_U64_GT_0(x)         ((x) >  0)
#define QUADLIB_U64_GE_0(x)         ((x) >= 0)

#if 0
#define QUADLIB_U64_LT_0(x)         ((x) <  0)
#define QUADLIB_U64_LE_0(x)         ((x) <= 0)
#endif

#endif /* QUADLIB_ENABLED */


/*****************************************************************************/
/*  I32 OVERFLOW CHECK                                                       */
/*****************************************************************************/


#define QUADLIB_I64_NOT_OVERFLOW_I32(x)                                       \
  (  ((QUADLIB_I64_HI(x) ==  0) && (QUADLIB_I64_LO(x) >= 0))                  \
  || ((QUADLIB_I64_HI(x) == -1) && (QUADLIB_I64_LO(x) <  0)) )             /**/


#define QUADLIB_I64_OVERFLOW_I32(x) (! QUADLIB_I64_NOT_OVERFLOW_I32(x))


#define QUADLIB_U64_NOT_OVERFLOW_I32(x)                                       \
  ((QUADLIB_I64_HI(x) == 0) && (QUADLIB_I64_LO(x) >= 0))                   /**/


#define QUADLIB_U64_OVERFLOW_I32(x) (! QUADLIB_U64_NOT_OVERFLOW_I32(x))


#define QUADLIB_I64_OVERFLOW_U32(x) (QUADLIB_I64_HI(x) != 0)
#define QUADLIB_U64_OVERFLOW_U32(x) (QUADLIB_I64_HI(x) != 0)


/*****************************************************************************/
/*  SHL                                                                      */
/*****************************************************************************/

#ifdef QUADLIB_ENABLED

#define QUADLIB_I64_SHL(x,n)        QuadLibU64SHL(x,n)
#define QUADLIB_U64_SHL(x,n)        QuadLibU64SHL(x,n)

#define QUADLIB_I64_SHL1(x)         QuadLibU64SHL1(x)
#define QUADLIB_U64_SHL1(x)         QuadLibU64SHL1(x)

#define QUADLIB_I64_SHL4(x)         QuadLibU64SHL4(x)
#define QUADLIB_U64_SHL4(x)         QuadLibU64SHL4(x)

#define QUADLIB_I64_SHL_EQ(x,n)     x = QUADLIB_I64_SHL(x,n)
#define QUADLIB_U64_SHL_EQ(x,n)     x = QUADLIB_U64_SHL(x,n)

#define QUADLIB_I64_SHL1_EQ(x)      x = QUADLIB_I64_SHL1(x)
#define QUADLIB_U64_SHL1_EQ(x)      x = QUADLIB_U64_SHL1(x)

#define QUADLIB_I64_SHL4_EQ(x)      x = QUADLIB_I64_SHL4(x)
#define QUADLIB_U64_SHL4_EQ(x)      x = QUADLIB_U64_SHL4(x)

#define QUADLIB_I64_SHL1_EQ_INLINE(x)   QUADLIB_U64_SHL1_EQ_INLINE(x)


#define QUADLIB_U64_SHL1_EQ_INLINE(x)                                         \
  {                                                                           \
  x.hi.u <<= 1;                                                               \
  if (x.lo.i < 0)                                                             \
    {                                                                         \
    x.hi.u++;                                                                 \
    }                                                                         \
  x.lo.u <<= 1;                                                               \
  }                                                                        /**/


#define QUADLIB_I64_SHL4_EQ_INLINE(x)   QUADLIB_U64_SHL4_EQ_INLINE(x)


#define QUADLIB_U64_SHL4_EQ_INLINE(x)                                         \
  {                                                                           \
  x.hi.u = (x.hi.u << 4) | (x.lo.u >> 28);                                    \
  x.lo.u <<= 4;                                                               \
  }                                                                        /**/


QUADLIB_STATIC QUADLIB_U64          QuadLibU64SHL (
  QUADLIB_U64                       /*I*/ value,
  int32_t                           /*I*/ shift);

QUADLIB_STATIC QUADLIB_U64          QuadLibU64SHL1 (
  QUADLIB_U64                       /*I*/ value);

QUADLIB_STATIC QUADLIB_U64          QuadLibU64SHL4 (
  QUADLIB_U64                       /*I*/ value);

#else /* not QUADLIB_ENABLED */

#define QUADLIB_I64_SHL(x,n)        ((x) << n)
#define QUADLIB_U64_SHL(x,n)        ((x) << n)

#define QUADLIB_I64_SHL1(x)         ((x) << 1)
#define QUADLIB_U64_SHL1(x)         ((x) << 1)

#define QUADLIB_I64_SHL4(x)         ((x) << 4)
#define QUADLIB_U64_SHL4(x)         ((x) << 4)

#define QUADLIB_I64_SHL_EQ(x,n)     x = ((x) << n)
#define QUADLIB_U64_SHL_EQ(x,n)     x = ((x) << n)

#define QUADLIB_I64_SHL1_EQ(x)      x <<= 1
#define QUADLIB_U64_SHL1_EQ(x)      x <<= 1

#define QUADLIB_I64_SHL1_EQ_INLINE(x)   QUADLIB_I64_SHL1_EQ(x)
#define QUADLIB_U64_SHL1_EQ_INLINE(x)   QUADLIB_U64_SHL1_EQ(x)

#define QUADLIB_I64_SHL4_EQ(x)      x <<= 4
#define QUADLIB_U64_SHL4_EQ(x)      x <<= 4

#define QUADLIB_I64_SHL4_EQ_INLINE(x)   QUADLIB_I64_SHL4_EQ(x)
#define QUADLIB_U64_SHL4_EQ_INLINE(x)   QUADLIB_U64_SHL4_EQ(x)

#endif /* QUADLIB_ENABLED */

/*****************************************************************************/
/*  SHR                                                                      */
/*****************************************************************************/

#ifdef QUADLIB_ENABLED

#define QUADLIB_I64_SHR(x,n)        QuadLibI64SHR(x,n)
#define QUADLIB_U64_SHR(x,n)        QuadLibU64SHR(x,n)

#define QUADLIB_I64_SHR1(x)         QuadLibI64SHR1(x)
#define QUADLIB_U64_SHR1(x)         QuadLibU64SHR1(x)

#define QUADLIB_I64_SHR4(x)         QuadLibI64SHR4(x)
#define QUADLIB_U64_SHR4(x)         QuadLibU64SHR4(x)

#define QUADLIB_I64_SHR_EQ(x,n)     x = QUADLIB_I64_SHR(x,n)
#define QUADLIB_U64_SHR_EQ(x,n)     x = QUADLIB_U64_SHR(x,n)

#define QUADLIB_I64_SHR1_EQ(x)      x = QUADLIB_I64_SHR1(x)
#define QUADLIB_U64_SHR1_EQ(x)      x = QUADLIB_U64_SHR1(x)

#define QUADLIB_I64_SHR4_EQ(x)      x = QUADLIB_I64_SHR4(x)
#define QUADLIB_U64_SHR4_EQ(x)      x = QUADLIB_U64_SHR4(x)


#define QUADLIB_I64_SHR1_EQ_INLINE(x)                                         \
  {                                                                           \
  x.lo.u >>= 1;                                                               \
  if (x.hi.u & 1)                                                             \
    {                                                                         \
    x.lo.u |= QUADLIB_U32_HIGHBIT;                                            \
    }                                                                         \
  x.hi.i >>= 1;     /* signed */                                              \
  }                                                                        /**/


#define QUADLIB_U64_SHR1_EQ_INLINE(x)                                         \
  {                                                                           \
  x.lo.u >>= 1;                                                               \
  if (x.hi.u & 1)                                                             \
    {                                                                         \
    x.lo.u |= QUADLIB_U32_HIGHBIT;                                            \
    }                                                                         \
  x.hi.u >>= 1;     /* unsigned */                                            \
  }                                                                        /**/


#define QUADLIB_I64_SHR4_EQ_INLINE(x)                                         \
  {                                                                           \
  x.lo.u = (x.lo.u >> 4) | (x.hi.u << 28);                                    \
  x.hi.i >>= 4;     /* signed */                                              \
  }                                                                        /**/


#define QUADLIB_U64_SHR4_EQ_INLINE(x)                                         \
  {                                                                           \
  x.lo.u = (x.lo.u >> 4) | (x.hi.u << 28);                                    \
  x.hi.u >>= 4;     /* unsigned */                                            \
  }                                                                        /**/


QUADLIB_STATIC QUADLIB_I64          QuadLibI64SHR (
  QUADLIB_U64                       /*I*/ value,
  int32_t                           /*I*/ shift);

QUADLIB_STATIC QUADLIB_I64          QuadLibI64SHR1 (
  QUADLIB_U64                       /*I*/ value);

QUADLIB_STATIC QUADLIB_I64          QuadLibI64SHR4 (
  QUADLIB_U64                       /*I*/ value);

QUADLIB_STATIC QUADLIB_U64          QuadLibU64SHR (
  QUADLIB_U64                       /*I*/ value,
  int32_t                           /*I*/ shift);

QUADLIB_STATIC QUADLIB_U64          QuadLibU64SHR1 (
  QUADLIB_U64                       /*I*/ value);

QUADLIB_STATIC QUADLIB_U64          QuadLibU64SHR4 (
  QUADLIB_U64                       /*I*/ value);

#else /* not QUADLIB_ENABLED */

#define QUADLIB_I64_SHR(x,n)        ((QUADLIB_I64)x) >> n
#define QUADLIB_U64_SHR(x,n)        ((QUADLIB_U64)x) >> n

#define QUADLIB_I64_SHR1(x)         ((QUADLIB_I64)x) >> 1
#define QUADLIB_U64_SHR1(x)         ((QUADLIB_U64)x) >> 1

#define QUADLIB_I64_SHR4(x)         ((QUADLIB_I64)x) >> 4
#define QUADLIB_U64_SHR4(x)         ((QUADLIB_U64)x) >> 4

#define QUADLIB_I64_SHR_EQ(x,n)     x = ((QUADLIB_I64)x) >> n
#define QUADLIB_U64_SHR_EQ(x,n)     x = ((QUADLIB_U64)x) >> n

#define QUADLIB_I64_SHR1_EQ(x)      x = ((QUADLIB_I64)x) >> 1
#define QUADLIB_U64_SHR1_EQ(x)      x = ((QUADLIB_U64)x) >> 1

#define QUADLIB_I64_SHR4_EQ(x)      x = ((QUADLIB_I64)x) >> 4
#define QUADLIB_U64_SHR4_EQ(x)      x = ((QUADLIB_U64)x) >> 4

#define QUADLIB_I64_SHR1_EQ_INLINE(x)   QUADLIB_I64_SHR1_EQ(x)
#define QUADLIB_U64_SHR1_EQ_INLINE(x)   QUADLIB_U64_SHR1_EQ(x)

#define QUADLIB_I64_SHR4_EQ_INLINE(x)   QUADLIB_I64_SHR4_EQ(x)
#define QUADLIB_U64_SHR4_EQ_INLINE(x)   QUADLIB_U64_SHR4_EQ(x)

#endif /* QUADLIB_ENABLED */

/*****************************************************************************/
/*  CONVERT                                                                  */
/*****************************************************************************/

#ifdef QUADLIB_ENABLED

#define QUADLIB_I64_CASTI64N(x)     QuadLibI64CastI64N(&x,sizeof(x))
#define QUADLIB_U64_CASTU64N(x)     QuadLibU64CastU64N(&x,sizeof(x))

QUADLIB_STATIC QUADLIB_I64          QuadLibI64CastI64N(void * x, size_t n);
QUADLIB_STATIC QUADLIB_U64          QuadLibU64CastU64N(void * x, size_t n);

#define QUADLIB_I64N_CASTI64(x)     QuadLibI64NCastI64(x)
#define QUADLIB_U64N_CASTU64(x)     QuadLibU64NCastU64(x)

QUADLIB_STATIC QUADLIB_I64N         QuadLibI64NCastI64(QUADLIB_I64 x);
QUADLIB_STATIC QUADLIB_U64N         QuadLibU64NCastU64(QUADLIB_U64 x);


#else /* not QUADLIB_ENABLED */

#define QUADLIB_I64_CASTI64N(x)     ((QUADLIB_I64)x)
#define QUADLIB_U64_CASTU64N(x)     ((QUADLIB_U64)x)
#define QUADLIB_I64N_CASTI64(x)     ((QUADLIB_I64N)x)
#define QUADLIB_U64N_CASTU64(x)     ((QUADLIB_U64N)x)

#endif /* QUADLIB_ENABLED */


/*****************************************************************************/
/*  CAST                                                                     */
/*****************************************************************************/

#ifdef QUADLIB_ENABLED

#define QUADLIB_I64_CASTI32(x)      QuadLibI64CastI32((int32_t)x)
#define QUADLIB_I64_CASTU32(x)      QuadLibI64CastI32((uint32_t)x)

QUADLIB_STATIC QUADLIB_I64          QuadLibI64CastI32 (int32_t x);

#define QUADLIB_U64_CASTU32(x)      QuadLibU64CastU32((uint32_t)x)
#define QUADLIB_U64_CASTI32(x)      QuadLibU64CastU32((int32_t)x)

QUADLIB_STATIC QUADLIB_U64          QuadLibU64CastU32 (uint32_t x);

#define QUADLIB_I32_CASTI64(x)      QuadLibI32CastI64(x)
#define QUADLIB_U32_CASTI64(x)      ((QUADLIB_U64) QuadLibI32CastI64(x))

QUADLIB_STATIC int32_t              QuadLibI32CastI64 (QUADLIB_I64 x);

#define QUADLIB_U32_CASTU64(x)      QuadLibU32CastU64(x)
#define QUADLIB_I32_CASTU64(x)      ((int32_t) QuadLibU32CastU64(x))

QUADLIB_STATIC uint32_t             QuadLibU32CastU64 (QUADLIB_U64 x);

#else /* not QUADLIB_ENABLED */

#define QUADLIB_I64_CASTI32(x)      ((QUADLIB_I64) (x))
#define QUADLIB_I64_CASTU32(x)      ((QUADLIB_I64) (x))

#define QUADLIB_U64_CASTI32(x)      ((QUADLIB_U64) (x))
#define QUADLIB_U64_CASTU32(x)      ((QUADLIB_U64) (x))

#define QUADLIB_I32_CASTI64(x)      ((int32_t) (x))
#define QUADLIB_I32_CASTU64(x)      ((int32_t) (x))

#define QUADLIB_U32_CASTI64(x)      ((uint32_t) (x))
#define QUADLIB_U32_CASTU64(x)      ((uint32_t) (x))

#endif /* QUADLIB_ENABLED */

/*****************************************************************************/
/*  GET/SET                                                                  */
/*****************************************************************************/

#define QUADLIB_I64_HI(x)           (*(QUADLIB_I64_TYPE *)(&x)).hi.i
#define QUADLIB_U64_HI(x)           (*(QUADLIB_U64_TYPE *)(&x)).hi.u
#define QUADLIB_I64_LO(x)           (*(QUADLIB_I64_TYPE *)(&x)).lo.i
#define QUADLIB_U64_LO(x)           (*(QUADLIB_U64_TYPE *)(&x)).lo.u

#define FILETIME_TO_QUADLIB_I64     FILETIME_TO_QUADLIB_U64


#define FILETIME_TO_QUADLIB_U64(f,q)                                          \
  (*(QUADLIB_U64_TYPE *)(q)).lo.u = (f)->dwLowDateTime;                       \
  (*(QUADLIB_U64_TYPE *)(q)).hi.u = (f)->dwHighDateTime                    /**/


#define QUADLIB_I64_TO_FILETIME     QUADLIB_U64_TO_FILETIME


#define QUADLIB_U64_TO_FILETIME(q,f)                                          \
  (f)->dwLowDateTime  = (*(QUADLIB_U64_TYPE *)(q)).lo.u;                      \
  (f)->dwHighDateTime = (*(QUADLIB_U64_TYPE *)(q)).hi.u                    /**/


/*****************************************************************************/
/*  MUL                                                                      */
/*****************************************************************************/

#ifdef QUADLIB_ENABLED

#define QUADLIB_I64_MUL(x,y)        QuadLibI64Mul(x,y)
#define QUADLIB_U64_MUL(x,y)        QuadLibU64Mul(x,y)
#define QUADLIB_I64_MUL_EQ(x,y)     x = QuadLibI64Mul(x,y)
#define QUADLIB_U64_MUL_EQ(x,y)     x = QuadLibU64Mul(x,y)

#define QUADLIB_I64_MUL10(x)        QuadLibI64Mul10(x)
#define QUADLIB_U64_MUL10(x)        QuadLibU64Mul10(x)
#define QUADLIB_I64_MUL10_EQ(x)     x = QuadLibI64Mul10(x)
#define QUADLIB_U64_MUL10_EQ(x)     x = QuadLibU64Mul10(x)

#define QuadLibI64Mul(x,y)          QuadLibU64Mul(x,y)
#define QuadLibI64Mul10(x)          QuadLibU64Mul10(x)

QUADLIB_STATIC QUADLIB_U64          QuadLibU64Mul (
  QUADLIB_U64                       /*I*/ one,
  QUADLIB_U64                       /*I*/ two);

QUADLIB_STATIC QUADLIB_U64          QuadLibU64Mul10 (
  QUADLIB_U64                       /*I*/ x);

#else /* not QUADLIB_ENABLED */

#define QUADLIB_I64_MUL(x,y)        (x * y)
#define QUADLIB_U64_MUL(x,y)        (x * y)
#define QUADLIB_I64_MUL_EQ(x,y)     x *= y
#define QUADLIB_U64_MUL_EQ(x,y)     x *= y

#define QUADLIB_I64_MUL10(x)        (x * ((QUADLIB_I64) 10))
#define QUADLIB_U64_MUL10(x)        (x * ((QUADLIB_U64) 10))
#define QUADLIB_I64_MUL10_EQ(x)     x *= ((QUADLIB_I64) 10)
#define QUADLIB_U64_MUL10_EQ(x)     x *= ((QUADLIB_U64) 10)

#endif /* QUADLIB_ENABLED */

/*****************************************************************************/
/*  STR TO I64/U64                                                           */
/*****************************************************************************/

QUADLIB_STATIC QUADLIB_I64          StrAToQuadLibI64 (
  char *                            /*I*/ str);

QUADLIB_STATIC QUADLIB_U64          StrAToQuadLibU64 (
  char *                            /*I*/ str);

#ifdef QUADLIB_NATIVE64_ENABLED

#define QUADLIB_I64_NUM(x)          (x)
#define QUADLIB_U64_NUM(x)          (x)

#else /* not QUADLIB_NATIVE64_ENABLED */

#define QUADLIB_I64_NUM(x)          StrAToQuadLibI64 (#x)
#define QUADLIB_U64_NUM(x)          StrAToQuadLibU64 (#x)

#endif /* QUADLIB_NATIVE64_ENABLED */


/*****************************************************************************/
/*  I64/U64 TO STR                                                           */
/*****************************************************************************/


QUADLIB_STATIC void         QuadLibU64ToFixedStrA (
  QUADLIB_U64               /*I*/ qValue,
  QUADLIB_STRINGA *         /*O*/ pString);


QUADLIB_STATIC void         QuadLibI64ToFixedStrA (
  QUADLIB_I64               /*I*/ qValue,
  QUADLIB_STRINGA *         /*O*/ pString);


QUADLIB_STATIC void         QuadLibU64ToStrA (
  QUADLIB_U64               /*I*/ qValue,
  QUADLIB_STRINGA *         /*O*/ pString);


QUADLIB_STATIC void         QuadLibI64ToStrA (
  QUADLIB_I64               /*I*/ qValue,
  QUADLIB_STRINGA *         /*O*/ pString);


/*****************************************************************************/
/*  DIV/MOD                                                                  */
/*****************************************************************************/

#ifdef QUADLIB_ENABLED

#define QUADLIB_I64_DIV(x,y)        QuadLibI64Div(x,y)
#define QUADLIB_U64_DIV(x,y)        QuadLibU64Div(x,y)
#define QUADLIB_I64_DIV_EQ(x,y)     x = QuadLibI64Div(x,y)
#define QUADLIB_U64_DIV_EQ(x,y)     x = QuadLibU64Div(x,y)

#define QUADLIB_I64_MOD(x,y)        QuadLibI64Mod(x,y)
#define QUADLIB_U64_MOD(x,y)        QuadLibU64Mod(x,y)
#define QUADLIB_I64_MOD_EQ(x,y)     x = QuadLibI64Mod(x,y)
#define QUADLIB_U64_MOD_EQ(x,y)     x = QuadLibU64Mod(x,y)

QUADLIB_STATIC QUADLIB_U64          QuadLibU64DivMod (
  QUADLIB_U64                       u64Dividend,
  QUADLIB_U64                       u64Divisor,
  QUADLIB_U64                       *pu64Remainder);

QUADLIB_STATIC QUADLIB_U64          QuadLibU64Div (
  QUADLIB_U64                       u64Dividend,
  QUADLIB_U64                       u64Divisor);

QUADLIB_STATIC QUADLIB_U64          QuadLibU64Mod (
  QUADLIB_U64                       u64Dividend,
  QUADLIB_U64                       u64Divisor);

QUADLIB_STATIC QUADLIB_I64          QuadLibI64DivMod (
  QUADLIB_I64                       s64Dividend,
  QUADLIB_I64                       s64Divisor,
  QUADLIB_I64                       *ps64Remainder);

QUADLIB_STATIC QUADLIB_I64          QuadLibI64Div (
  QUADLIB_I64                       s64Dividend,
  QUADLIB_I64                       s64Divisor);

QUADLIB_STATIC QUADLIB_I64          QuadLibI64Mod (
  QUADLIB_I64                       s64Dividend,
  QUADLIB_I64                       s64Divisor);

#else /* not QUADLIB_ENABLED */

#define QUADLIB_I64_DIV(x,y)        (x / y)
#define QUADLIB_U64_DIV(x,y)        (x / y)
#define QUADLIB_I64_DIV_EQ(x,y)     x /= y
#define QUADLIB_U64_DIV_EQ(x,y)     x /= y

#define QUADLIB_I64_MOD(x,y)        (x % y)
#define QUADLIB_U64_MOD(x,y)        (x % y)
#define QUADLIB_I64_MOD_EQ(x,y)     x %= y
#define QUADLIB_U64_MOD_EQ(x,y)     x %= y

QUADLIB_STATIC QUADLIB_U64          QuadLibU64DivMod (
  QUADLIB_U64                       u64Dividend,
  QUADLIB_U64                       u64Divisor,
  QUADLIB_U64                       *pu64Remainder);

QUADLIB_STATIC QUADLIB_I64          QuadLibI64DivMod (
  QUADLIB_I64                       s64Dividend,
  QUADLIB_I64                       s64Divisor,
  QUADLIB_I64                       *ps64Remainder);

#endif /* QUADLIB_ENABLED */

/*****************************************************************************/
/*  END                                                                      */
/*****************************************************************************/

#ifdef  __cplusplus
}
#endif


#endif /* _STDTIME_QUADLIB_H_B07CB807_8BFC_41B3_872E_EDD251FD6AEB_ */

