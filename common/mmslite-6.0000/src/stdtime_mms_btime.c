/*****************************************************************************/
/* SISCO SOFTWARE MODULE HEADER **********************************************/
/*****************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,            */
/*            2006 All Rights Reserved                                       */
/*                                                                           */
/* MODULE NAME : stdtime_mms_btime.c                                         */
/* PRODUCT(S)  : Standard Time Management Library                            */
/*                                                                           */
/* MODULE DESCRIPTION:                                                       */
/*   Support for MMS Btime4, BTime6 and BTOD conversions to/from STDTIME     */
/*                                                                           */
/* MODIFICATION LOG:                                                         */
/*   Date    Who  Rev  Comments                                              */
/* --------  ---  ---  ----------------------------------------------------- */
/* 02/20/08  JRB   02  Make STDTIME same as FILETIME (simplifies a lot).     */
/*		       Move T84_* defines to header file.		     */
/*		       Del TimeOfDay4, Btod, and StdTimeFields functions.    */
/* 10/05/06  RLH   01  Created                                               */
/*****************************************************************************/


#include "stdtime.h"
#include "stdtime_mms_btime.h"

/*** MmsTimeOfDay6 initialization functions ***************************/


STDTIME_MMS_TIMEOFDAY6      StdTimeApiZeroMmsTimeOfDay6 ()
  {
  STDTIME_MMS_TIMEOFDAY6    zero = {0};

  return zero;

  } /* StdTimeApiZeroMmsTimeOfDay6 */


/*** MmsTimeOfDay6Ex functions ***********************************************/


/*-***************************************************************************/
/*            StdTimeToMmsTimeOfDay6Ex                                       */
/*  convert: StdTime struct to MMS_TIMEOFDAY6 and extra fields               */
/*  method:  subtract 1984-01-01 from stdTime to form delta                  */
/*****************************************************************************/

STDTIME_RC                  StdTimeToMmsTimeOfDay6Ex (
  const STDTIME *           /*I*/ pStdTime,
  STDTIME_MMS_TIMEOFDAY6 *  /*O*/ pMmsTimeOfDay6,
  int32_t *                 /*O*/ pUsec,
  int32_t *                 /*O*/ pNsec)
  {
  /* pre-calculated STDTIME value of 1984-01-01 00:00:00.0000000 */
  STDTIME t84;	/* initialized below	*/

  STDTIME_DELTA             mms;

  STDTIME_ENUM_FUNC (StdTimeToMmsTimeOfDay6Ex)

  if (pStdTime->dwHighDateTime < T84_HIGHDATETIME ||
      (pStdTime->dwHighDateTime == T84_HIGHDATETIME &&
       pStdTime->dwLowDateTime < T84_LOWDATETIME))
    {
    STDTIME_RET_EC (MMS_year_LT_1984);
    }

  t84.dwHighDateTime = T84_HIGHDATETIME;	/* initialize	*/
  t84.dwLowDateTime  = T84_LOWDATETIME;

  if (pMmsTimeOfDay6 == NULL)
    {
    STDTIME_RET_EC (null_argument);
    }

  *pMmsTimeOfDay6 = StdTimeApiZeroMmsTimeOfDay6();

  /* form difference between supplied date and 1984 */

  if STDTIME_IFNOT (StdTime_GetDelta (pStdTime, &t84, &mms))
    {
    STDTIME_RET_RC;
    }

  pMmsTimeOfDay6->day = mms.day;

  pMmsTimeOfDay6->ms =
    (mms.hour * 60 * 60 * 1000) +
    (mms.min       * 60 * 1000) +
    (mms.sec            * 1000) + mms.msec;

  if (pUsec != NULL)
    {
    *pUsec = mms.usec;
    }

  if (pNsec != NULL)
    {
    *pNsec = mms.nsec;
    }

  STDTIME_RET_OK;

  } /* StdTimeToMmsTimeOfDay6Ex */


/*-***************************************************************************/
/*            MmsTimeOfDay6ExToStdTime                                       */
/*  convert: MMS_TIMEOFDAY6 and extra fields to StdTime struct               */
/*  method:  add MMS days and msec to base value of 1984-01-01               */
/*****************************************************************************/

STDTIME_RC                  MmsTimeOfDay6ExToStdTime (
  const STDTIME_MMS_TIMEOFDAY6 * /*I*/ pMmsTimeOfDay6,
  int32_t                        /*I*/ nUsec,
  int32_t                        /*I*/ nNsec,
  STDTIME *                      /*O*/ pStdTime)
  {
  /* pre-calculated STDTIME value of 1984-01-01 00:00:00.0000000 */
  STDTIME t84;	/* initialized below	*/
  STDTIME_DELTA             mms = {0};

  STDTIME_ENUM_FUNC (MmsTimeOfDay6ExToStdTime)

  t84.dwHighDateTime = T84_HIGHDATETIME;	/* initialize	*/
  t84.dwLowDateTime  = T84_LOWDATETIME;

  if (pStdTime == NULL)
    {
    STDTIME_RET_EC (null_argument);
    }

  *pStdTime = ZeroStdTime ();

  if (pMmsTimeOfDay6 == NULL)
    {
    STDTIME_RET_EC (null_argument);
    }

  *pStdTime = ZeroStdTime();

  /* create delta from 1984-01-01 */

  mms.day  = pMmsTimeOfDay6->day;
  mms.msec = pMmsTimeOfDay6->ms;
  mms.usec = nUsec;
  mms.nsec = nNsec;

  /* add mms displacement to mms epoch date of Jan 1 1984 */

  if STDTIME_IFNOT (StdTime_AddDelta (&t84, &mms))
    {
    STDTIME_RET_RC;
    }

  *pStdTime = t84;

  STDTIME_RET_OK;

  } /* MmsTimeOfDay6ExToStdTime */


/*** MmsTimeOfDay6 functions *************************************************/


/*-***************************************************************************/
/*            StdTimeToMmsTimeOfDay6                                         */
/*  convert: StdTime struct to MMS_TIMEOFDAY6 (without extra fields)         */
/*****************************************************************************/

STDTIME_RC                  StdTimeToMmsTimeOfDay6 (
  const STDTIME *           /*I*/ pStdTime,
  STDTIME_MMS_TIMEOFDAY6 *  /*O*/ pMmsTimeOfDay6)
  {
  STDTIME_ENUM_FUNC (StdTimeToMmsTimeOfDay6)

  STDTIME_RET (StdTimeToMmsTimeOfDay6Ex (pStdTime, pMmsTimeOfDay6, NULL, NULL));

  } /* StdTimeToMmsTimeOfDay6 */


/*-***************************************************************************/
/*            MmsTimeOfDay6ToStdTime                                         */
/*  convert: MMS_TIMEOFDAY6 (without extra fields) to StdTime struct         */
/*****************************************************************************/

STDTIME_RC                  MmsTimeOfDay6ToStdTime (
  const STDTIME_MMS_TIMEOFDAY6 * /*I*/ pMmsTimeOfDay6,
  STDTIME *                 /*O*/ pStdTime)
  {
  STDTIME_ENUM_FUNC (MmsTimeOfDay6ToStdTime)

  STDTIME_RET (MmsTimeOfDay6ExToStdTime (pMmsTimeOfDay6, 0, 0, pStdTime));

  } /* MmsTimeOfDay6ToStdTime */
