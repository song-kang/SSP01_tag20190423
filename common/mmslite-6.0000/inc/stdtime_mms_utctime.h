/*****************************************************************************/
/* SISCO SOFTWARE MODULE HEADER **********************************************/
/*****************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,            */
/*            2006 All Rights Reserved                                       */
/*                                                                           */
/* MODULE NAME : stdtime_mms_utctime.h                                       */
/* PRODUCT(S)  : Standard Time Management Library                            */
/*                                                                           */
/* MODULE DESCRIPTION:                                                       */
/*   Support for MMS UTC TIME conversions to/from STDTIME                    */
/*                                                                           */
/* MODIFICATION LOG:                                                         */
/*   Date    Who  Rev  Comments                                              */
/* --------  ---  ---  ----------------------------------------------------- */
/* 04/08/08  JRB   03  Chg () to (ST_VOID) in prototype.                     */
/* 02/20/08  JRB   02  Delete StdTimeFields functions.                       */
/* 10/05/06  RLH   01  Created                                               */
/*****************************************************************************/

#ifdef  __cplusplus
extern "C" {
#endif

#if defined(ASN1R_INCLUDED)
#define STDTIME_MMS_UTC_TIME    MMS_UTC_TIME

#else

typedef struct
  {
  int32_t                   secs;     /* seconds since January 1, 1970 */
  int32_t                   fraction; /* 24-bit binary fraction of second */
  int32_t                   qflags;   /* 8-bit quality flags */
  }
STDTIME_MMS_UTC_TIME;

#endif


/*** MmsUtcTime initialization function **************************************/

STDTIME_MMS_UTC_TIME        StdTimeApiZeroMmsUtcTime (ST_VOID);


/*** MmsUtcTime functions *************************************************/


STDTIME_RC                  StdTimeToMmsUtcTime (
  const STDTIME *           /*I*/ pStdTime,
  STDTIME_MMS_UTC_TIME *    /*O*/ pMmsUtcTime);


STDTIME_RC                  MmsUtcTimeToStdTime (
  const STDTIME_MMS_UTC_TIME * /*I*/ pMmsUtcTime,
  STDTIME *                 /*O*/ pStdTime);

#ifdef  __cplusplus
}
#endif

