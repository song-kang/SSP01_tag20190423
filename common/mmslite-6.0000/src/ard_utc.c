/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2004, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : ard_utc.c 						*/
/* PRODUCT(S)  : ASN1DE 						*/
/*									*/
/* MODULE DESCRIPTION : Decode MMS UtcTime				*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 03/16/04  EJV     05    Added ST_(U)LONG typecast to logs,on some sys*/
/*			     ST_(U)INT32 can be (unsigned) long or int.	*/
/* 07/26/02  EJV     04    Time Quality byte chg position to lowest.	*/
/*			   Added logging.				*/
/* 07/03/02  EJV     03    MMS_UTC_TIME: chg name usec to fraction.	*/
/* 12/20/01  JRB     02    Chg ASN1_CTXT to ASN1_DEC_CTXT.		*/
/* 11/09/01  EJV     01    New module, derived from ard_btod.c		*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "asn1r.h"
#include "asn1log.h"

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/
/*			    asn1r_get_utc_time				*/
/************************************************************************/

ST_RET asn1r_get_utc_time (ASN1_DEC_CTXT *ac, MMS_UTC_TIME *dest)
  {
#ifdef	DEBUG_ASN1_DECODE
  if (!dest)
    {
    slogCallStack (sLogCtrl,
                    "asn1r_get_utc_time: attempt to reference through a NULL pointer");
    return(SD_FAILURE);
    }
#endif

  ALOG_DEC0("Decoding UtcTime");

  /* Read the number of seconds since January 1, 1970 (4 bytes)	*/
  dest->secs =  (((ST_UINT32) *(ac->asn1r_field_ptr++)) << 24) & 0xFF000000L;
  dest->secs |= (((ST_UINT32) *(ac->asn1r_field_ptr++)) << 16) & 0x00FF0000L;
  dest->secs |= (((ST_UINT32) *(ac->asn1r_field_ptr++)) << 8)  & 0x0000FF00L;
  dest->secs |=  ((ST_UINT32) *(ac->asn1r_field_ptr++))        & 0x000000FFL;

  /* read fraction of a second (3 bytes) */
  dest->fraction =  (((ST_UINT32) *(ac->asn1r_field_ptr++)) << 16) & 0x00FF0000L;
  dest->fraction |= (((ST_UINT32) *(ac->asn1r_field_ptr++)) << 8)  & 0x0000FF00L;
  dest->fraction |=  ((ST_UINT32) *(ac->asn1r_field_ptr++))        & 0x000000FFL;

  /* read the quality flags (1 byte) */
  dest->qflags =  ((ST_UINT32) *(ac->asn1r_field_ptr++));

  ALOG_CDEC1("seconds:   %lu", (ST_ULONG) dest->secs);
  ALOG_CDEC1("fraction:  %lu", (ST_ULONG) dest->fraction);
  ALOG_CDEC1("quality:   0x%lX", (ST_ULONG) dest->qflags);

  return (SD_SUCCESS);
  }



