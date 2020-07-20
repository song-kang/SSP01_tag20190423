/*****************************************************************************/
/* SISCO SOFTWARE MODULE HEADER **********************************************/
/*****************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,            */
/*            2005 All Rights Reserved                                       */
/*                                                                           */
/* MODULE NAME : QuadLib.c                                                   */
/* PRODUCT(S)  : Standard Time Management Library                            */
/*                                                                           */
/* MODULE DESCRIPTION:                                                       */
/*   64-bit Math Simulation Library.                                         */
/*   Provided for platforms without native 64-bit support.                   */
/*                                                                           */
/* MODIFICATION LOG:                                                         */
/*   Date    Who  Rev  Comments                                              */
/* --------  ---  ---  ----------------------------------------------------- */
/* 02/03/10  NAV   03  OpenVMS port					     */
/* 06/07/06  RLH   02  Numerous features added, see stdtime.doc              */
/* 01/18/06  RLH   01  Created                                               */
/*****************************************************************************/

#include "stdtime.h"

#ifdef QUADLIB_ENABLED

int32_t QuadLibU64Cmp (QUADLIB_U64 x, QUADLIB_U64 y)
  {
  if (x.hi.u < y.hi.u) return -1;
  if (x.hi.u > y.hi.u) return 1;

  if (x.lo.u < y.lo.u) return -1;
  if (x.lo.u > y.lo.u) return 1;

  return 0;
  }

int32_t QuadLibI64Cmp (QUADLIB_I64 x, QUADLIB_I64 y)
  {
  if (x.hi.i < y.hi.i) return -1;
  if (x.hi.i > y.hi.i) return 1;

  /* the unsigned compare on lo.u works correctly if x.hi.i == y.hi.i */
  /* in case of negative numbers, when lo increases in unsigned */
  /* magnitude, the 64-bit value is less negative, and so is a */
  /* greater signed value, so nothing special needs to be done */

  if (x.lo.u < y.lo.u) return -1;
  if (x.lo.u > y.lo.u) return 1;

  return 0;
  }

#endif /* QUADLIB_ENABLED */


/*****************************************************************************/
/*  GET/SET                                                                  */
/*****************************************************************************/


#ifndef QUADLIB_ENABLED

/* get hi value of a 64-bit value when host has native support */

QUADLIB_STATIC int32_t      QuadLibI64GetHi (QUADLIB_I64 x)
  {
  return (int32_t) QUADLIB_I64_HI(x);

  } /* QuadLibI64GetHi */

QUADLIB_STATIC uint32_t     QuadLibU64GetHi (QUADLIB_U64 x)
  {
  return (uint32_t) QUADLIB_U64_HI(x);

  } /* QuadLibU64GetHi */

/* get lo value of a 64-bit value when host has native support */

QUADLIB_STATIC int32_t      QuadLibI64GetLo (QUADLIB_I64 x)
  {
  return (int32_t) QUADLIB_I64_LO(x);

  } /* QuadLibI64GetLo */

QUADLIB_STATIC uint32_t     QuadLibU64GetLo (QUADLIB_U64 x)
  {
  return (uint32_t) QUADLIB_U64_LO(x);

  } /* QuadLibU64GetLo */


/* set hi value of a 64-bit value when host has native support */

void QuadLibI64SetHi (QUADLIB_I64 *x, int32_t y)
  {
  (*(QUADLIB_I64_TYPE *)(x)).hi.i = y;

  } /* QuadLibI64SetHi */

void QuadLibU64SetHi (QUADLIB_U64 *x, uint32_t y)
  {
  (*(QUADLIB_I64_TYPE *)(x)).hi.u = y;

  } /* QuadLibU64SetHi */

/* set lo value of a 64-bit value when host has native support */

void QuadLibI64SetLo (QUADLIB_I64 *x, int32_t y)
  {
  (*(QUADLIB_I64_TYPE *)(x)).lo.i = y;

  } /* QuadLibI64SetLo */

void QuadLibU64SetLo (QUADLIB_U64 *x, uint32_t y)
  {
  (*(QUADLIB_I64_TYPE *)(x)).lo.u = y;

  } /* QuadLibU64SetLo */

#endif /* not QUADLIB_ENABLED */


/*****************************************************************************/
/*  CAST                                                                     */
/*****************************************************************************/


#ifdef QUADLIB_ENABLED


/* 64-bit to 64-bit conversions */


QUADLIB_STATIC QUADLIB_I64  QuadLibI64CastI64N(void * x, size_t n)
  {
  QUADLIB_I64               result;

  /* determine type of cast based on sizeof(x) */

  switch (n)
    {
    case 8:
        QUADLIB_I64_HI(result) = QUADLIB_I64_HI(x);
        QUADLIB_I64_LO(result) = QUADLIB_I64_LO(x);
        break;

    case 2:
        QUADLIB_I64_HI(result) =
        QUADLIB_I64_LO(result) = (*(short *)(&x));
        QUADLIB_I64_HI(result) >>= 31;  /* sign propagation */
        break;

    case 1:
        QUADLIB_I64_HI(result) =
        QUADLIB_I64_LO(result) = (*(char *)(&x));
        QUADLIB_I64_HI(result) >>= 31;  /* sign propagation */
        break;

    default:
        /* assume length 4 */
        QUADLIB_I64_HI(result) =
        QUADLIB_I64_LO(result) = (*(long *)(&x));
        QUADLIB_I64_HI(result) >>= 31;  /* sign propagation */
        break;
    }

  return result;

  } /* QuadLibI64CastI64N */


QUADLIB_STATIC QUADLIB_U64  QuadLibU64CastU64N(void * x, size_t n)
  {
  QUADLIB_U64               result;

  /* determine type of cast based on sizeof(x) */

  switch (n)
    {
    case 8:
        QUADLIB_I64_HI(result) = QUADLIB_I64_HI(x);
        QUADLIB_I64_LO(result) = QUADLIB_I64_LO(x);
        break;

    case 2:
        QUADLIB_I64_HI(result) = 0;
        QUADLIB_I64_LO(result) = (*(unsigned short *)(&x));
        break;

    case 1:
        QUADLIB_I64_HI(result) = 0;
        QUADLIB_I64_LO(result) = (*(unsigned char *)(&x));
        break;

    default:
        /* assume length 4 */
        QUADLIB_I64_HI(result) = 0;
        QUADLIB_I64_LO(result) = (*(unsigned long *)(&x));
        break;
    }

  return result;

  } /* QuadLibU64CastU64N */


QUADLIB_STATIC QUADLIB_I64N QuadLibI64NCastI64(QUADLIB_I64 x)
  {
  QUADLIB_I64N              result;

#ifdef QUADLIB_NATIVE64_ENABLED
  QUADLIB_I64_HI(result) = QUADLIB_I64_HI(x);
  QUADLIB_I64_LO(result) = QUADLIB_I64_LO(x);

#else
  /* x is a 32-bit value, so we can only return 32 bits of I64 value */
  result = (QUADLIB_I64N) QUADLIB_I64_LO(x);

#endif
  return result;

  } /* QuadLibI64NCastI64 */


QUADLIB_STATIC QUADLIB_U64N QuadLibU64NCastU64(QUADLIB_U64 x)
  {
  QUADLIB_U64N              result;

#ifdef QUADLIB_NATIVE64_ENABLED
  QUADLIB_U64_HI(result) = QUADLIB_U64_HI(x);
  QUADLIB_U64_LO(result) = QUADLIB_U64_LO(x);

#else
  /* x is a 32-bit value, so we can only return 32 bits of I64 value */
  result = (QUADLIB_U64N) QUADLIB_U64_LO(x);

#endif
  return result;

  } /* QuadLibU64NCastU64 */


/* upsize casts: 32 to 64 bit */


QUADLIB_STATIC QUADLIB_I64  QuadLibI64CastI32 (int32_t x)
  {
  QUADLIB_I64               result;

  result.lo.i = x;
  result.hi.i = x;
  result.hi.i >>= 31;       /* for signed, x propagation */

  return result;
  } /* QuadLibI64CastI32 */


QUADLIB_STATIC QUADLIB_U64  QuadLibU64CastU32 (uint32_t x)
  {
  QUADLIB_U64               result;

  result.hi.u = 0;          /* for unsigned, 0 propagation */
  result.lo.u = x;

  return result;
  } /* QuadLibU64CastU32 */


/* downsize casts: 64 to 32 bit */


QUADLIB_STATIC int32_t      QuadLibI32CastI64 (QUADLIB_I64 x)
  {
  return x.lo.i;
  } /* QuadLibI32CastI64 */


QUADLIB_STATIC uint32_t     QuadLibU32CastU64 (QUADLIB_U64 x)
  {
  return x.lo.u;
  } /* QuadLibU32CastU64 */


#endif /* QUADLIB_ENABLED */


/*****************************************************************************/
/*            QuadLibU64Not                                                  */
/*  form 1's complement of 64-bit value                                      */
/*****************************************************************************/


#ifdef QUADLIB_ENABLED

QUADLIB_STATIC QUADLIB_U64  QuadLibU64Not (QUADLIB_U64 x)
  {
  QUADLIB_U64               result;

  result.hi.u = ~(x.hi.u);
  result.lo.u = ~(x.lo.u);
  return result;
  }

#endif /* QUADLIB_ENABLED */

/*****************************************************************************/
/*            QuadLibU64Neg                                                  */
/*  form 2's complement of 64-bit value                                      */
/*****************************************************************************/

#ifdef QUADLIB_ENABLED

QUADLIB_STATIC QUADLIB_U64  QuadLibU64Neg (QUADLIB_U64 x)
  {
  QUADLIB_U64               result;

  result.hi.u = ~(x.hi.u);
  result.lo.u = (~(x.lo.u)) + 1;

  /* if x.lo.u came in as 0, its complement is 0xFFFFFFFF */
  /* if we then increment it, it will become 0 again */
  /* so, if result.lo.u is zero now, there was a carry */

  if (result.lo.u == 0)
    {
    result.hi.u++;
    }

  return result;
  } /* QuadLibU64Neg */

#endif /* QUADLIB_ENABLED */

/*****************************************************************************/
/*            QuadLibU64Abs                                                  */
/*  form absolute value of 64-bit value                                      */
/*****************************************************************************/

#ifdef QUADLIB_ENABLED

QUADLIB_STATIC QUADLIB_U64  QuadLibU64Abs (QUADLIB_U64 x)
  {
  if (x.hi.i < 0)               /* value is negative */
    {
    return QUADLIB_U64_NEG (x);
    }

  return x;                     /* value is positive, do not negate */

  } /* QuadLibU64Abs */

#else /* not QUADLIB_ENABLED */

QUADLIB_STATIC QUADLIB_U64  QuadLibU64Abs (QUADLIB_U64 x)
  {
#if !defined(__VMS)
  if (x < 0)                    /* value is negative */
    {
    return (QUADLIB_U64) (-(QUADLIB_I64) x);
    /* negate native I64 value */
    }
#endif

  return x;                     /* value is positive, do not negate */

  } /* QuadLibU64Abs */

#endif /* QUADLIB_ENABLED */

/*****************************************************************************/
/*            QuadLibU64Mul                                                  */
/*  multiply two unsigned 64-bit values.                                     */
/*  form partial products from 16-bit values to limit results to 32 bits.    */
/*                                                                           */
/*  method: let ABCD mean four 16-bit values with 'one', and WXYZ in 'two'   */
/*  then, one * two = ABCD * WXYZ.  in "long-hand" this means:               */
/*                                                                           */
/*  (let xxL mean the Left 16 bits of a word, and xxR the Right 16 bits)     */
/*                                                                           */
/*                  A   B   C   D                                            */
/*                  W   X   Y   Z                                            */
/*                  --------------                                           */
/*                  AZ  BZ  CZ  DZ                                           */
/*              AY  BY  CY  DY                                               */
/*          AX  BX  CX  DX                                                   */
/*      AW  BW  CW  DW                                                       */
/*                                                                           */
/*  express the partial sums as split L/R pairs, and discard partial sums    */
/*  that exceed the 64-bit result capacity. ("discard" means discard from    */
/*  the explanation; the discarded values are not actually calculated.)      */
/*                                                                           */
/*                  A   B   C   D                                            */
/*                  W   X   Y   Z                                            */
/*                  --------------                                           */
/*                  AZR BZR CZR DZR                                          */
/*                  BZL CZL DZL                                              */
/*                  BYR CYR DYR                                              */
/*                  CYL DYL                                                  */
/*                  CXR DXR                                                  */
/*                  DXL                                                      */
/*                  DWR                                                      */
/*                                                                           */
/*  since there is no carry out of the left 32-bit value (the AB column),    */
/*  there is no need to split the left-most (AB column) 32-bit values into   */
/*  16/16.  so, we optimize the list of partial sums as follows, (where      */
/*  (xxL:xxR represents an unsplit 32-bit value), and reorganize the         */
/*  diagram to show the left/right relationships better:                     */
/*                                                                           */
/*                  A   B   C   D                                            */
/*                  W   X   Y   Z                                            */
/*                  --------------                                           */
/*                  AZR                                                      */
/*                  bzL:bzR                                                  */
/*                      CZL CZR                                              */
/*                          DZL DZR                                          */
/*                  --------------                                           */
/*                  BYR                                                      */
/*                  cyL:cyR                                                  */
/*                      DYL DYR                                              */
/*                  --------------                                           */
/*                  CXR                                                      */
/*                  dxL:dxR                                                  */
/*                  --------------                                           */
/*                  DWR                                                      */
/*                                                                           */
/*  finally, add up the columns, and store the sums in a 64-bit value        */
/*                                                                           */
/*****************************************************************************/

#ifdef QUADLIB_ENABLED

#define QUADLIB_U32_LWORD(x)        ( ((uint32_t)(x)) >> 16)
#define QUADLIB_U32_RWORD(x)        ((x) & 0xFFFF)

QUADLIB_STATIC QUADLIB_U64  QuadLibU64Mul (
  QUADLIB_U64               /*I*/ one,
  QUADLIB_U64               /*I*/ two)
  {
  QUADLIB_U64               result;

  uint32_t                  a, b, c, d, w, x, y, z;
  uint32_t                  ab, bc, cd;
  uint32_t                  az, bz, cz, dz;
  uint32_t                      by, cy, dy;
  uint32_t                          cx, dx;
  uint32_t                              dw;

  uint32_t                  azr, czr, dzr;
  uint32_t                  czl, dzl;
  uint32_t                  byr, dyr;
  uint32_t                  dyl;
  uint32_t                  cxr;
  uint32_t                  dwr;


  a = QUADLIB_U32_LWORD(one.hi.u);
  b = QUADLIB_U32_RWORD(one.hi.u);
  c = QUADLIB_U32_LWORD(one.lo.u);
  d = QUADLIB_U32_RWORD(one.lo.u);

  w = QUADLIB_U32_LWORD(two.hi.u);
  x = QUADLIB_U32_RWORD(two.hi.u);
  y = QUADLIB_U32_LWORD(two.lo.u);
  z = QUADLIB_U32_RWORD(two.lo.u);


  az = a * z;   bz = b * z;   cz = c * z;   dz = d * z;
                by = b * y;   cy = c * y;   dy = d * y;
                              cx = c * x;   dx = d * x;
                                            dw = d * w;
  azr = QUADLIB_U32_RWORD(az);
  czr = QUADLIB_U32_RWORD(cz);
  dzr = QUADLIB_U32_RWORD(dz);
  czl = QUADLIB_U32_LWORD(cz);
  dzl = QUADLIB_U32_LWORD(dz);
  byr = QUADLIB_U32_RWORD(by);
  dyr = QUADLIB_U32_RWORD(dy);
  dyl = QUADLIB_U32_LWORD(dy);
  cxr = QUADLIB_U32_RWORD(cx);
  dwr = QUADLIB_U32_RWORD(dw);

  bc = czr + dzl + dyr;

  ab = bc >> 16;
  cd = bc << 16;

  result.lo.u = cd  + dzr;

  result.hi.u = ab  + bz  + cy  + dx
              + czl + dyl
            + ((azr + byr + cxr + dwr) << 16);

  return result;

  } /* QuadLibU64Mul */

#undef QUADLIB_U32_LWORD
#undef QUADLIB_U32_RWORD

#endif /* QUADLIB_ENABLED */

/*****************************************************************************/
/*            QuadLibU64Add                                                  */
/*  add two unsigned 64-bit values                                           */
/*****************************************************************************/

#ifdef QUADLIB_ENABLED

QUADLIB_STATIC QUADLIB_U64  QuadLibU64Add (
  QUADLIB_U64               /*I*/ one,
  QUADLIB_U64               /*I*/ two)
  {
  QUADLIB_U64               sum;
  uint32_t                  carry;

  sum.lo.u = one.lo.u + two.lo.u;
  sum.hi.u = one.hi.u + two.hi.u;

  /* calculate carry using boolean algebra */

  carry =
    ( (one.lo.u & two.lo.u) | ((one.lo.u ^ two.lo.u) & (~sum.lo.u)) ) >> 31;

  sum.hi.u += carry;

  return sum;

  } /* QuadLibU64Add */

#endif /* QUADLIB_ENABLED */

/*****************************************************************************/
/*            QuadLibU64Sub                                                  */
/*  subtract two unsigned 64-bit values                                      */
/*  method: negate second operand, and then add values                       */
/*****************************************************************************/

#ifdef QUADLIB_ENABLED

QUADLIB_STATIC QUADLIB_U64  QuadLibU64Sub (
  QUADLIB_U64               /*I*/ one,
  QUADLIB_U64               /*I*/ two)
  {
  return QuadLibU64Add (one, QUADLIB_U64_NEG (two));
  } /* QuadLibU64Sub */

#endif /* QUADLIB_ENABLED */

/*****************************************************************************/
/*            QuadLibU64Add1                                                 */
/*  increment a 64-bit value                                                 */
/*  method: add a constant 1 to a 64-bit value                               */
/*****************************************************************************/

#ifdef QUADLIB_ENABLED

QUADLIB_STATIC QUADLIB_U64  QuadLibU64Add1 (
  QUADLIB_U64               /*I*/ x)
  {
  QUADLIB_U64               result;

  result.hi.u = x.hi.u;
  result.lo.u = x.lo.u + 1;

  if (result.lo.u == 0)
    {
    /* x.lo.u must have had all 1 bits, so a carry occurred */
    result.hi.u++;
    }

  return result;

  } /* QuadLibU64Add1 */

#endif /* QUADLIB_ENABLED */

/*****************************************************************************/
/*            QuadLibU64Sub1                                                 */
/*  decrement a 64-bit value                                                 */
/*  method: add a constant -1 to a 64-bit value                              */
/*****************************************************************************/

#ifdef QUADLIB_ENABLED

QUADLIB_STATIC QUADLIB_U64  QuadLibU64Sub1 (
  QUADLIB_U64               /*I*/ x)
  {
  QUADLIB_U64               result;

  result.hi.u = x.hi.u;
  result.lo.u = x.lo.u - 1;

  if (x.lo.u == 0)
    {
    /* x.lo.u was 0, so a borrow occurred */
    result.hi.u--;
    }

  return result;

  } /* QuadLibU64Sub1 */

#endif /* QUADLIB_ENABLED */

/*****************************************************************************/
/*            QuadLibU64SHR                                                  */
/*  shift an unsigned 64-bit value right by n bits                           */
/*****************************************************************************/

#ifdef QUADLIB_ENABLED

QUADLIB_STATIC QUADLIB_U64  QuadLibU64SHR (
  QUADLIB_U64               /*I*/ value,
  int32_t                   /*I*/ shift)
  {
  QUADLIB_U64               result = QUADLIB_ZERO;

  if (shift == 0)
    {
    return value;
    }

  if (shift < 0)
    {
    return QuadLibU64SHL (value, -shift);
    }

  if (shift > 63)
    {
    return result;          /* all bits shifted out, return 0 */
    }

  for (result = value; shift > 0; shift--)
    {
    result.lo.u >>= 1;

    if (result.hi.u & 1)
      {
      /* hi has low-order 1 about to be shifted out */
      result.lo.u |= QUADLIB_U32_HIGHBIT;
      }

    result.hi.u >>= 1;      /* unsigned shift right */
    }

  return result;

  } /* QuadLibU64SHR */

#endif /* QUADLIB_ENABLED */

/*****************************************************************************/
/*            QuadLibI64SHR                                                  */
/*  shift a signed 64-bit value right by n bits                              */
/*****************************************************************************/

#ifdef QUADLIB_ENABLED

QUADLIB_STATIC QUADLIB_I64  QuadLibI64SHR (
  QUADLIB_I64               /*I*/ value,
  int32_t                   /*I*/ shift)
  {
  QUADLIB_I64               result = QUADLIB_ZERO;

  if (shift == 0)
    {
    return value;
    }

  if (shift < 0)
    {
    return QuadLibU64SHL (value, -shift);
    }

  if (shift > 63)
    {
    return result;          /* all bits shifted out return 0 */
    }

  for (result = value; shift > 0; shift--)
    {
    result.lo.u >>= 1;

    if (result.hi.u & 1)
      {
      /* hi has low-order 1 about to be shifted out */
      result.lo.u |= QUADLIB_U32_HIGHBIT;
      }

    result.hi.i >>= 1;      /* signed shift right */
    }

  return result;

  } /* QuadLibI64SHR */

#endif /* QUADLIB_ENABLED */

/*****************************************************************************/
/*            QuadLibU64SHL                                                  */
/*  shift an unsigned 64-bit value left by n bits                            */
/*****************************************************************************/

#ifdef QUADLIB_ENABLED

QUADLIB_STATIC QUADLIB_U64  QuadLibU64SHL (
  QUADLIB_U64               /*I*/ value,
  int32_t                   /*I*/ shift)
  {
  QUADLIB_U64               result = QUADLIB_ZERO;

  if (shift == 0)
    {
    return value;
    }

  if (shift < 0)
    {
    return QuadLibU64SHR (value, -shift);
    }

  if (shift > 63)
    {
    return result;          /* all bits shifted out return 0 */
    }

  for (result = value; shift > 0; shift--)
    {
    QUADLIB_U64_SHL1_EQ_INLINE (result);
    }

  return result;

  } /* QuadLibU64SHL */

#endif /* QUADLIB_ENABLED */

/*****************************************************************************/
/*            QuadLibI64SHR1                                                 */
/*  shift a signed 64-bit value right by 1 bit                               */
/*****************************************************************************/

#ifdef QUADLIB_ENABLED

QUADLIB_STATIC QUADLIB_I64  QuadLibI64SHR1 (
  QUADLIB_I64               /*I*/ value)
  {
  QUADLIB_I64_SHR1_EQ_INLINE (value);

  return value;

  } /* QuadLibI64SHR1 */

#endif /* QUADLIB_ENABLED */

/*****************************************************************************/
/*            QuadLibU64SHR1                                                 */
/*  shift an unsigned 64-bit value right by 1 bit                            */
/*****************************************************************************/

#ifdef QUADLIB_ENABLED

QUADLIB_STATIC QUADLIB_U64  QuadLibU64SHR1 (
  QUADLIB_U64               /*I*/ value)
  {
  QUADLIB_U64_SHR1_EQ_INLINE (value);

  return value;

  } /* QuadLibU64SHR1 */

#endif /* QUADLIB_ENABLED */

/*****************************************************************************/
/*            QuadLibU64SHL1                                                 */
/*  shift an unsigned 64-bit value left by 1 bit                             */
/*****************************************************************************/

#ifdef QUADLIB_ENABLED

QUADLIB_STATIC QUADLIB_U64  QuadLibU64SHL1 (
  QUADLIB_U64               /*I*/ value)
  {
  QUADLIB_U64_SHL1_EQ_INLINE (value);

  return value;

  } /* QuadLibU64SHL1 */

#endif /* QUADLIB_ENABLED */

/*****************************************************************************/
/*            QuadLibI64SHR4                                                 */
/*  shift a signed 64-bit value right by 1 bit                               */
/*****************************************************************************/

#ifdef QUADLIB_ENABLED

QUADLIB_STATIC QUADLIB_I64  QuadLibI64SHR4 (
  QUADLIB_I64               /*I*/ value)
  {
  QUADLIB_I64_SHR4_EQ_INLINE (value);

  return value;

  } /* QuadLibI64SHR4 */

#endif /* QUADLIB_ENABLED */

/*****************************************************************************/
/*            QuadLibU64SHR4                                                 */
/*  shift an unsigned 64-bit value right by 1 bit                            */
/*****************************************************************************/

#ifdef QUADLIB_ENABLED

QUADLIB_STATIC QUADLIB_U64  QuadLibU64SHR4 (
  QUADLIB_U64               /*I*/ value)
  {
  QUADLIB_U64_SHR4_EQ_INLINE (value);

  return value;

  } /* QuadLibU64SHR4 */

#endif /* QUADLIB_ENABLED */

/*****************************************************************************/
/*            QuadLibU64SHL4                                                 */
/*  shift an unsigned 64-bit value left by 1 bit                             */
/*****************************************************************************/

#ifdef QUADLIB_ENABLED

QUADLIB_STATIC QUADLIB_U64  QuadLibU64SHL4 (
  QUADLIB_U64               /*I*/ value)
  {
  QUADLIB_U64_SHL4_EQ_INLINE (value);

  return value;

  } /* QuadLibU64SHL4 */

#endif /* QUADLIB_ENABLED */

/*****************************************************************************/
/*            QuadLibU64Mul10                                                */
/*  multiple an unsigned 64-bit value by 10                                  */
/*  method: get value*2 and value*8 by shifting, and then add them           */
/*****************************************************************************/

#ifdef QUADLIB_ENABLED

QUADLIB_STATIC QUADLIB_U64  QuadLibU64Mul10 (
  QUADLIB_U64               /*I*/ value)
  {
  QUADLIB_U64               value2;
  QUADLIB_U64               value8;
  QUADLIB_U64               result;

  value2 = value;
  QUADLIB_U64_SHL1_EQ_INLINE (value2);

  value8 = value2;
  QUADLIB_U64_SHL1_EQ_INLINE (value8);
  QUADLIB_U64_SHL1_EQ_INLINE (value8);

  result = QuadLibU64Add (value8, value2);
  return result;

  } /* QuadLibU64Mul10 */

#endif /* QUADLIB_ENABLED */

/*****************************************************************************/
/*            StrAToQuadLibU64                                               */
/*  define a portable 64-bit unsigned atoi convervion                        */
/*****************************************************************************/

/* StrAToQuadLibU64 always defined */

QUADLIB_STATIC QUADLIB_U64  StrAToQuadLibU64 (
  char *                    /*I*/ str)
  {
  /* extract a decimal string and return unsigned int64 value */

  QUADLIB_U64               result = QUADLIB_ZERO;
  QUADLIB_U64               digit = QUADLIB_ZERO;
  int32_t                   n;

  if (str == NULL)
    {
    return result;
    }

  while (isspace (*str))
    {
    str++;                  /* skip over whitespace */
    }

  /* handle hex value if present */

  if ( (str[0] == '0') && ((str[1] == 'x') || (str[1] == 'X')) )
    {
    for (str+=2; *str; str++)
      {
      n = *str;

      if ((n >= '0') && (n <= '9'))
        {
        n -= '0';
        }

      else if ((n >= 'A') && (n <= 'F'))
        {
        n = n - 'A' + 10;
        }

      else if ((n >= 'a') && (n <= 'f'))
        {
        n = n - 'a' + 10;
        }

      else
        {
        break;
        }

      QUADLIB_I64_LO(digit) = n;
      QUADLIB_U64_SHL1_EQ_INLINE (result);
      QUADLIB_U64_ADD_EQ (result, digit);
      } /* for */

    return result;
    }

  for (; *str; str++)
    {
    n = (*str) - '0';

    if ((n < 0) || (n > 9))
      {
      break;
      }

    QUADLIB_I64_LO(digit) = n;
    QUADLIB_U64_MUL10_EQ (result);
    QUADLIB_U64_ADD_EQ (result, digit);
    }

  return result;

  } /* StrAToQuadLibU64 */

/* StrAToQuadLibU64 always defined */

/*****************************************************************************/
/*            StrAToQuadLibI64                                               */
/*  define a portable 64-bit signed atoi convervion                          */
/*****************************************************************************/

/* StrAToQuadLibI64 always defined */

QUADLIB_STATIC QUADLIB_I64  StrAToQuadLibI64 (
  char *                    /*I*/ str)
  {
  /* extract a decimal string and return unsigned int64 value */

  QUADLIB_I64               result = QUADLIB_ZERO;
  QUADLIB_I64               digit = QUADLIB_ZERO;
  char                      sign = ' ';
  int32_t                   n;

  if (str == NULL)
    {
    return result;
    }

  while (isspace (*str))
    {
    str++;                  /* skip over whitespace */
    }

  if ((*str == '+') || (*str == '-'))
    {
    sign = *str;
    str++;                  /* skip over sign */
    }

  /* handle hex value if present */

  if ( (str[0] == '0') && ((str[1] == 'x') || (str[1] == 'X')) )
    {
    for (str+=2; *str; str++)
      {
      n = *str;

      if ((n >= '0') && (n <= '9'))
        {
        n -= '0';
        }

      else if ((n >= 'A') && (n <= 'F'))
        {
        n = n - 'A' + 10;
        }

      else if ((n >= 'a') && (n <= 'f'))
        {
        n = n - 'a' + 10;
        }

      else
        {
        break;
        }

      QUADLIB_I64_LO(digit) = n;
      QUADLIB_U64_SHL1_EQ_INLINE (result);
      QUADLIB_U64_ADD_EQ (result, digit);
      } /* for */

    if (sign == '-')
      {
      result = QUADLIB_I64_NEG (result);
      }

    return result;
    }

  for (; *str; str++)
    {
    n = (*str) - '0';

    if ((n < 0) || (n > 9))
      {
      break;
      }

    QUADLIB_I64_LO(digit) = n;
    QUADLIB_I64_MUL10_EQ (result);
    QUADLIB_I64_ADD_EQ (result, digit);
    }

  if (sign == '-')
    {
    result = QUADLIB_I64_NEG (result);
    }

  return result;

  } /* StrAToQuadLibI64 */

/* StrAToQuadLibI64 always defined */


/*****************************************************************************/
/*            QuadLibU64ToFixedStrA                                          */
/*  define a portable 64-bit unsigned itoa convervion                        */
/*****************************************************************************/

/* QuadLibU64ToFixedStrA always defined */

QUADLIB_STATIC void         QuadLibU64ToFixedStrA (
  QUADLIB_U64               /*I*/ qValue,
  QUADLIB_STRINGA *         /*O*/ pString)
  {
  QUADLIB_U64               value = qValue;
  int32_t                   i;
  int32_t                   n;

  int32_t                   xdigit;
  uint8_t                   sum [QUADLIB_STRING_LEN] = {0};
  uint8_t                   carry;

  char *                    factor;
  static char *             factor_table[16] =
    {
    "00000000000000000001",
    "00000000000000000016",
    "00000000000000000256",
    "00000000000000004096",
    "00000000000000065536",
    "00000000000001048576",
    "00000000000016777216",
    "00000000000268435456",
    "00000000004294967296",
    "00000000068719476736",
    "00000001099511627776",
    "00000017592186044416",
    "00000281474976710656",
    "00004503599627370496",
    "00072057594037927936",
    "01152921504606846976",
    };

  /* grab 16 hex digits one at a time */
  /* obtain the decimal factor for a '1' in that position */
  /* multiply each decimal digit of the factor by the hex digit */
  /* and add to sum */

  for (i=0; i < 16; i++)
    {
    if (QUADLIB_U64_EQ_0 (value))
      {
      break;
      }

    xdigit = QUADLIB_U64_LO (value) & 0x0F;
    QUADLIB_U64_SHR4_EQ (value);

    if (xdigit == 0)
      {
      continue;     /* nothing to do */
      }

    factor = factor_table [i];

    /* highest value in any position is (15*9)+9 = 135+9 = 144 */
    /* with a carry in of 9, the highest value is 153 */
    /* recall that factor is ASCII when calculating the sum */

    carry = 0;

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4244)
/* silence the VC compiler warning message: */
/* "conversion from 'int ' to 'unsigned __int8', possible loss of data" */
#endif

    for (n = QUADLIB_STRING_LEN-1; n >= 0; n--)
      {
      sum[n] += (uint8_t) (((factor[n] & 0x0F) * xdigit) + carry);
      carry = sum[n] / 10;
      sum[n] %= 10;
      }
    }

#ifdef _MSC_VER
#pragma warning(pop)
#endif

  /* copy sum to output, converting to ASCII */

  for (n = 0; n < QUADLIB_STRING_LEN; n++)
    {
    pString->str[n] = (char) ((sum[n] & 0x0F) + '0');
    }

  pString->str[QUADLIB_STRING_LEN] = 0;

  } /* QuadLibU64ToFixedStrA */


/*****************************************************************************/
/*            QuadLibI64ToFixedStrA                                          */
/*  define a portable 64-bit signed itoa convervion                          */
/*****************************************************************************/

/* QuadLibI64ToFixedStrA always defined */

QUADLIB_STATIC void         QuadLibI64ToFixedStrA (
  QUADLIB_I64               /*I*/ qValue,
  QUADLIB_STRINGA *         /*O*/ pString)
  {
  char                      sign = '0';

  if (QUADLIB_I64_LT_0 (qValue))
    {
    qValue = QUADLIB_I64_NEG (qValue);
    sign = '-';
    }

  QuadLibU64ToFixedStrA (* (QUADLIB_U64 *) &qValue, pString);
  pString->str[0] = sign;

  } /* QuadLibI64ToFixedStrA */


/*****************************************************************************/
/*            QuadLibU64ToStrA                                               */
/*  define a portable 64-bit signed itoa convervion, with zero suppression   */
/*****************************************************************************/

/* QuadLibU64ToStrA always defined */

QUADLIB_STATIC void         QuadLibU64ToStrA (
  QUADLIB_U64               /*I*/ qValue,
  QUADLIB_STRINGA *         /*O*/ pString)
  {
  QUADLIB_STRINGA           s;
  int32_t                   source = 0;
  int32_t                   target = 0;

  QuadLibU64ToFixedStrA (qValue, &s);

  /* copy back value, suppressing leading zeros */

  while (s.str [source] == '0')
    {
    source++;
    }

  if (s.str [source] == 0)
    {
    pString->str [target++] = '0';  /* source is all zero */
    }

  while (s.str [source])
    {
    pString->str [target++] = s.str [source++];
    }

  pString->str [target] = 0;

  } /* QuadLibU64ToStrA */


/*****************************************************************************/
/*            QuadLibI64ToStrA                                               */
/*  define a portable 64-bit signed itoa convervion, with zero suppression   */
/*****************************************************************************/

/* QuadLibI64ToStrA always defined */

QUADLIB_STATIC void         QuadLibI64ToStrA (
  QUADLIB_I64               /*I*/ qValue,
  QUADLIB_STRINGA *         /*O*/ pString)
  {
  QUADLIB_STRINGA           s;
  int32_t                   source = 0;
  int32_t                   target = 0;

  QuadLibI64ToFixedStrA (qValue, &s);

  /* copy back value, suppressing leading zeros */

  if (s.str [source] == '-')
    {
    pString->str [target++] = s.str [source++];
    }

  while (s.str [source] == '0')
    {
    source++;
    }

  if (s.str [source] == 0)
    {
    pString->str [target++] = '0';  /* source is all zero */
    }

  while (s.str [source])
    {
    pString->str [target++] = s.str [source++];
    }

  pString->str [target] = 0;

  } /* QuadLibI64ToStrA */


/*****************************************************************************/
/*            QuadLibU64DivMod                                               */
/*  divide two unsigned 64-bit values to produce a quotient and remainder    */
/*****************************************************************************/

#ifdef QUADLIB_ENABLED

QUADLIB_STATIC QUADLIB_U64  QuadLibU64DivMod (
  QUADLIB_U64               u64Dividend,
  QUADLIB_U64               u64Divisor,
  QUADLIB_U64               *pu64Remainder )
  {
  int32_t                   Count;
  QUADLIB_U64               A_restore;

  QUADLIB_U64               A = QUADLIB_ZERO;
  QUADLIB_U64               M = u64Divisor;
  QUADLIB_U64               Q = u64Dividend;


  if ((u64Divisor.hi.u == 0)
  &&  (u64Divisor.lo.u == 0))
    {
    *pu64Remainder = A;         /* 0 */
    return A;                   /* 0 */
    }


  for (Count=64; Count; Count--)
    {
    /* shift A:Q left */

    QUADLIB_U64_SHL1_EQ_INLINE (A);

    if (Q.hi.i < 0)             /* Q has high-order 1 bit */
      {
      A = QuadLibU64Add1(A);    /* A gets low-order 1 bit */
      }

    QUADLIB_U64_SHL1_EQ_INLINE (Q); /* Q gets low-order 0 bit */

    A_restore = A;

    A = QuadLibU64Sub(A, M);

    if (A.hi.i < 0)             /* A has high-order 1 bit */
      {
      A = A_restore;            /* Q has low-order 0 bit */
      }

    else
      {
      Q = QuadLibU64Add1(Q);    /* Q gets low-order 1 bit */
      }

    } /* for */

    *pu64Remainder = A;
    return Q;

  } /* QuadLibU64DivMod */

#endif /* QUADLIB_ENABLED */

/*****************************************************************************/
/*            QuadLibU64Div                                                  */
/*  divide two signed 64-bit values to produce an unsigned quotient          */
/*****************************************************************************/

#ifdef QUADLIB_ENABLED

QUADLIB_STATIC QUADLIB_U64  QuadLibU64Div (
  QUADLIB_U64               u64Dividend,
  QUADLIB_U64               u64Divisor)
  {
  QUADLIB_U64               u64Remainder;      /* dummy argument */

  return QuadLibU64DivMod (u64Dividend, u64Divisor, &u64Remainder);

  } /*QuadLibU64Div */

#endif /* QUADLIB_ENABLED */

/*****************************************************************************/
/*            QuadLibU64Mod                                                  */
/*  divide two signed 64-bit values to produce unsigned remainder (modulus)  */
/*****************************************************************************/

#ifdef QUADLIB_ENABLED

QUADLIB_STATIC QUADLIB_U64  QuadLibU64Mod (
  QUADLIB_U64               u64Dividend,
  QUADLIB_U64               u64Divisor)
  {
  QUADLIB_U64               u64Remainder;

  QuadLibU64DivMod (u64Dividend, u64Divisor, &u64Remainder);

  return u64Remainder;

  } /*QuadLibU64Mod */

#endif /* QUADLIB_ENABLED */

/*****************************************************************************/
/*            QuadLibI64DivMod                                               */
/*  divide two signed 64-bit values to produce a quotient and remainder      */
/*****************************************************************************/

#ifdef QUADLIB_ENABLED

QUADLIB_STATIC QUADLIB_I64  QuadLibI64DivMod (
  QUADLIB_I64               s64Dividend,
  QUADLIB_I64               s64Divisor,
  QUADLIB_I64               *ps64Remainder )
  {
  QUADLIB_I64               s64Quotient;
  QUADLIB_U64               u64Dividend;
  QUADLIB_U64               u64Divisor;
  int32_t                   sign = 0;

  if (s64Dividend.hi.i < 0)
    {
    u64Dividend = QUADLIB_I64_NEG (s64Dividend);
    sign = 1;
    }

  else
    {
    u64Dividend = s64Dividend;
    }

  if (s64Divisor.hi.i < 0)
    {
    u64Divisor = QUADLIB_I64_NEG (s64Divisor);
    sign ^= 1;
    }

  else
    {
    u64Divisor = s64Divisor;
    }

  s64Quotient = QuadLibU64DivMod (u64Dividend, u64Divisor, ps64Remainder);

  if (sign)
    {
    /* signs of dividend and divisor are different, result is neg */
    s64Quotient = QUADLIB_I64_NEG (s64Quotient);
    }

  /* remainder takes sign of dividend */

  if (s64Dividend.hi.i < 0)
    {
    *ps64Remainder = QUADLIB_I64_NEG (*ps64Remainder);
    }

  return s64Quotient;

} /* QuadLibI64DivMod */

#endif /* QUADLIB_ENABLED */

/*****************************************************************************/
/*            QuadLibI64Div                                                  */
/*  divide two signed 64-bit values to produce a signed quotient             */
/*****************************************************************************/

#ifdef QUADLIB_ENABLED

QUADLIB_STATIC QUADLIB_I64  QuadLibI64Div (
  QUADLIB_I64               s64Dividend,
  QUADLIB_I64               s64Divisor)
  {
  QUADLIB_U64               s64Remainder;      /* dummy argument */

  return QuadLibI64DivMod (s64Dividend, s64Divisor, &s64Remainder);

  } /*QuadLibI64Div */

#endif /* QUADLIB_ENABLED */

/*****************************************************************************/
/*            QuadLibI64Mod                                                  */
/*  divide two signed 64-bit values to produce a signed remainder (modulus)  */
/*****************************************************************************/

#ifdef QUADLIB_ENABLED

QUADLIB_STATIC QUADLIB_I64  QuadLibI64Mod (
  QUADLIB_I64               s64Dividend,
  QUADLIB_I64               s64Divisor)
  {
  QUADLIB_I64               s64Remainder;

  QuadLibI64DivMod (s64Dividend, s64Divisor, &s64Remainder);

  return s64Remainder;

  } /*QuadLibI64Mod */

#endif /* QUADLIB_ENABLED */

/*****************************************************************************/
/*            QuadLibU64DivMod - COMPATIBILITY MODE                          */
/*            QuadLibI64DivMod - COMPATIBILITY MODE                          */
/*  native I64 divide/modulus functions with I64 simulation not in effect    */
/*****************************************************************************/

#ifndef QUADLIB_ENABLED

QUADLIB_STATIC QUADLIB_U64          QuadLibU64DivMod (
  QUADLIB_U64                       u64Dividend,
  QUADLIB_U64                       u64Divisor,
  QUADLIB_U64                       *pu64Remainder)
  {
  if (pu64Remainder != NULL)
    {
    *pu64Remainder = u64Dividend % u64Divisor;
    }

  return u64Dividend / u64Divisor;

  } /* QuadLibU64DivMod */

QUADLIB_STATIC QUADLIB_I64          QuadLibI64DivMod (
  QUADLIB_I64                       s64Dividend,
  QUADLIB_I64                       s64Divisor,
  QUADLIB_I64                       *ps64Remainder)
  {
  if (ps64Remainder != NULL)
    {
    *ps64Remainder = s64Dividend % s64Divisor;
    }

  return s64Dividend / s64Divisor;

  } /* QuadLibI64DivMod */

#endif /* not QUADLIB_ENABLED */


/*****************************************************************************/
/*  AND                                                                      */
/*****************************************************************************/

#ifdef QUADLIB_ENABLED

QUADLIB_STATIC QUADLIB_U64          QuadLibU64And (
  QUADLIB_U64                       /*I*/ one,
  QUADLIB_U64                       /*I*/ two)
  {
  QUADLIB_U64                       result;

  result.hi.u = one.hi.u & two.hi.u;
  result.lo.u = one.lo.u & two.lo.u;

  return result;
  }

QUADLIB_STATIC QUADLIB_U64          QuadLibU64AndNot (
  QUADLIB_U64                       /*I*/ one,
  QUADLIB_U64                       /*I*/ two)
  {
  QUADLIB_U64                       result;

  result.hi.u = one.hi.u & (~two.hi.u);
  result.lo.u = one.lo.u & (~two.lo.u);

  return result;
  }

#endif /* QUADLIB_ENABLED */

/*****************************************************************************/
/*  OR                                                                       */
/*****************************************************************************/

#ifdef QUADLIB_ENABLED

QUADLIB_STATIC QUADLIB_U64          QuadLibU64Or (
  QUADLIB_U64                       /*I*/ one,
  QUADLIB_U64                       /*I*/ two)
  {
  QUADLIB_U64                       result;

  result.hi.u = one.hi.u | two.hi.u;
  result.lo.u = one.lo.u | two.lo.u;

  return result;
  }

QUADLIB_STATIC QUADLIB_U64          QuadLibU64OrNot (
  QUADLIB_U64                       /*I*/ one,
  QUADLIB_U64                       /*I*/ two)
  {
  QUADLIB_U64                       result;

  result.hi.u = one.hi.u | (~two.hi.u);
  result.lo.u = one.lo.u | (~two.lo.u);

  return result;
  }

#endif /* QUADLIB_ENABLED */

/*****************************************************************************/
/*  XOR                                                                      */
/*****************************************************************************/

#ifdef QUADLIB_ENABLED

QUADLIB_STATIC QUADLIB_U64          QuadLibU64Xor (
  QUADLIB_U64                       /*I*/ one,
  QUADLIB_U64                       /*I*/ two)
  {
  QUADLIB_U64                       result;

  result.hi.u = one.hi.u ^ two.hi.u;
  result.lo.u = one.lo.u ^ two.lo.u;

  return result;
  }

QUADLIB_STATIC QUADLIB_U64          QuadLibU64XorNot (
  QUADLIB_U64                       /*I*/ one,
  QUADLIB_U64                       /*I*/ two)
  {
  QUADLIB_U64                       result;

  result.hi.u = one.hi.u ^ (~two.hi.u);
  result.lo.u = one.lo.u ^ (~two.lo.u);

  return result;
  }

#endif /* QUADLIB_ENABLED */

