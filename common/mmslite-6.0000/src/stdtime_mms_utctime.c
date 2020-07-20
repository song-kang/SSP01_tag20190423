/*****************************************************************************/
/* SISCO SOFTWARE MODULE HEADER **********************************************/
/*****************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,            */
/*            2006 All Rights Reserved                                       */
/*                                                                           */
/* MODULE NAME : stdtime_mms_utctime.c                                       */
/* PRODUCT(S)  : Standard Time Management Library                            */
/*                                                                           */
/* MODULE DESCRIPTION:                                                       */
/*   Support for MMS UTC TIME conversions to/from STDTIME                    */
/*                                                                           */
/* MODIFICATION LOG:                                                         */
/*   Date    Who  Rev  Comments                                              */
/* --------  ---  ---  ----------------------------------------------------- */
/* 02/20/08  JRB   02  Simplified StdTimeToMmsUtcTime (fixes rounding bug).  */
/*		       Simplified MmsUtcTimeToStdTime.			     */
/*		       Delete StdTimeFields functions.                       */
/* 10/05/06  RLH   01  Created                                               */
/*****************************************************************************/


#include "stdtime.h"
#include "stdtime_mms_utctime.h"


/*** MmsUtcTime initialization function **************************************/


STDTIME_MMS_UTC_TIME        StdTimeApiZeroMmsUtcTime ()
  {
  STDTIME_MMS_UTC_TIME      zero = {0};

  return zero;

  } /* StdTimeApiZeroMmsUtcTime */


/*** MmsUtcTime functions ****************************************************/


/*-***************************************************************************/
/*            StdTimeToMmsUtcTime                                            */
/*  convert: StdTime structure to MMS_UTC_TIME                               */
/*  method:  convert to TIMETYPE, then calculate binary fraction of second   */
/*****************************************************************************/

STDTIME_RC                  StdTimeToMmsUtcTime (
  const STDTIME *           /*I*/ pStdTime,
  STDTIME_MMS_UTC_TIME *    /*O*/ pMmsUtcTime)
  {
  QUADLIB_I64               qFrac;
  QUADLIB_I64               qRem;
  QUADLIB_I64               q10_7;
  QUADLIB_I64               q2_24;
  time_t timet;
  int32_t nsec;

  STDTIME_ENUM_FUNC (StdTimeToMmsUtcTime)


  if (pMmsUtcTime == NULL)
    {
    STDTIME_RET_EC (null_argument);
    }

  if STDTIME_IFNOT (StdTimeToTimeTypeEx (pStdTime, &timet, &nsec))
    {
    STDTIME_RET_RC;
    }

  /* Init qFrac to number of 100-nsec intervals.	*/
  QUADLIB_I64_HI(qFrac) = 0;
  QUADLIB_I64_LO(qFrac) = nsec / 100;

  QUADLIB_I64_HI(q10_7) = 0;
  QUADLIB_I64_LO(q10_7) = 10000000;

  QUADLIB_I64_HI(q2_24) = 0;
  QUADLIB_I64_LO(q2_24) = 0x1000000;
 
  /* multiply by range of UTC fraction */

  QUADLIB_I64_MUL_EQ (qFrac, q2_24);

  /* divide out number of 100-ns intervals */
  /* as 100-ns intervals approaches 10_7, frac approaches 2_24 */

  qFrac = QuadLibI64DivMod (qFrac, q10_7, &qRem);

  /* assign UTC values to output argument */

  pMmsUtcTime->secs = (int32_t) timet;
  pMmsUtcTime->fraction = (int32_t) QUADLIB_I64_LO(qFrac);
  pMmsUtcTime->qflags = 0;      /* IS THIS CORRECT ? */

  STDTIME_RET_OK;

  } /* StdTimeToMmsUtcTime */


/*-***************************************************************************/
/*            MmsUtcTimeToStdTime                                            */
/*  convert: MMS_UTC_TIME to StdTime structure                               */
/*****************************************************************************/

STDTIME_RC                  MmsUtcTimeToStdTime (
  const STDTIME_MMS_UTC_TIME * /*I*/ pMmsUtcTime,
  STDTIME *                      /*O*/ pStdTime)
  {
  QUADLIB_I64               qFrac;
  QUADLIB_I64               qRem;
  QUADLIB_I64               q10_7;
  QUADLIB_I64               q2_24;
  int32_t                   nsec;	/* nanoseconds	*/

  STDTIME_ENUM_FUNC (MmsUtcTimeToStdTime)

  if (pStdTime == NULL)
    {
    STDTIME_RET_EC (null_argument);
    }

  *pStdTime = ZeroStdTime ();

  if (pMmsUtcTime == NULL)
    {
    STDTIME_RET_EC (null_argument);
    }

  /* Convert fraction to nsec, then call TimeTypeExToStdTime.	*/

  /* obtain number of binary fractional units */

  QUADLIB_I64_HI(q10_7) = 0;
  QUADLIB_I64_LO(q10_7) = 10000000;

  QUADLIB_I64_HI(q2_24) = 0;
  QUADLIB_I64_LO(q2_24) = 0x1000000;

  QUADLIB_I64_HI(qFrac) = 0;
  QUADLIB_I64_LO(qFrac) = pMmsUtcTime->fraction;


  /* multiply by range of 100-ns intervals */

  QUADLIB_I64_MUL_EQ (qFrac, q10_7);

  /* divide out number of binary fractional units */
  /* as fractional units approach 2**24, frac approaches 10**7 */

  qFrac = QuadLibI64DivMod (qFrac, q2_24, &qRem);

  if (QUADLIB_I64_LO(qRem) >= (10000000 / 2))
    {
    QUADLIB_I64_LO(qFrac)++;    /* ROUND UP */
    }

  nsec = (QUADLIB_I32_CASTI64 (qFrac)) * 100;

  /* Convert and put result at pointer pStdTime.	*/
  if STDTIME_IFNOT (TimeTypeExToStdTime (pMmsUtcTime->secs, nsec, pStdTime))
    {
    STDTIME_RET_RC;
    }
  STDTIME_RET_OK;

  } /* MmsUtcTimeToStdTime */

