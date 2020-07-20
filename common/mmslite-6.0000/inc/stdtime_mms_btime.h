/*****************************************************************************/
/* SISCO SOFTWARE MODULE HEADER **********************************************/
/*****************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,            */
/*            2006 All Rights Reserved                                       */
/*                                                                           */
/* MODULE NAME : stdtime_mms_btime.h                                         */
/* PRODUCT(S)  : Standard Time Management Library                            */
/*                                                                           */
/* MODULE DESCRIPTION:                                                       */
/*   Support for MMS Btime4, BTime6 and BTOD conversions to/from STDTIME     */
/*                                                                           */
/* MODIFICATION LOG:                                                         */
/*   Date    Who  Rev  Comments                                              */
/* --------  ---  ---  ----------------------------------------------------- */
/* 04/08/08  JRB   03  Chg () to (ST_VOID) in prototype.                     */
/* 02/20/08  JRB   02  Add T84_* defines.				     */
/*		       Del TimeOfDay4, Btod, and StdTimeFields functions.    */
/* 10/05/06  RLH   01  Created                                               */
/*****************************************************************************/

#ifdef  __cplusplus
extern "C" {
#endif

#define T84_HIGHDATETIME	0x01ad63ae	/* high word of 1984 FILETIME*/
#define T84_LOWDATETIME		0xef5a4000	/* low word of 1984 FILETIME*/

#if defined(ASN1R_INCLUDED) && defined(MMS_BTOD4) && defined(MMS_BTOD6)
#define STDTIME_MMS_TIMEOFDAY6  MMS_BTIME6

#else

typedef struct
  {
  int32_t                   ms;     /* milliseconds since midnight */
  int32_t                   day;    /* days since January 1, 1984 */
  }
STDTIME_MMS_TIMEOFDAY6;

#endif



/*** MmsTimeOfDay4/6 initialization functions ********************************/

STDTIME_MMS_TIMEOFDAY6      StdTimeApiZeroMmsTimeOfDay6 (ST_VOID);

/*** MmsTimeOfDay6Ex functions ***********************************************/


STDTIME_RC                  StdTimeToMmsTimeOfDay6Ex (
  const STDTIME *           /*I*/ pStdTime,
  STDTIME_MMS_TIMEOFDAY6 *  /*O*/ pMmsTimeOfDay6,
  int32_t *                 /*O*/ pUsec, /* optional */
  int32_t *                 /*O*/ pNsec); /* optional */


STDTIME_RC                  MmsTimeOfDay6ExToStdTime (
  const STDTIME_MMS_TIMEOFDAY6 * /*I*/ pMmsTimeOfDay6,
  int32_t                   /*I*/ nUsec,
  int32_t                   /*I*/ nNsec,
  STDTIME *                 /*O*/ pStdTime);


/*** MmsTimeOfDay6 functions *************************************************/


STDTIME_RC                  StdTimeToMmsTimeOfDay6 (
  const STDTIME *           /*I*/ pStdTime,
  STDTIME_MMS_TIMEOFDAY6 *  /*O*/ pMmsTimeOfDay6);


STDTIME_RC                  MmsTimeOfDay6ToStdTime (
  const STDTIME_MMS_TIMEOFDAY6 * /*I*/ pMmsTimeOfDay6,
  STDTIME *                 /*O*/ pStdTime);

#ifdef  __cplusplus
}
#endif

