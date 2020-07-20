/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2004, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : are_utc.c 						*/
/* PRODUCT(S)  : ASN1DE 						*/
/*									*/
/* MODULE DESCRIPTION : Encode MMS UtcTime				*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 03/16/04  EJV     07    Added ST_(U)LONG typecast to logs,on some sys*/
/*			     ST_(U)INT32 can be (unsigned) long or int.	*/
/* 01/14/03  JRB     06    Avoid redundant logging of encode overrun.	*/
/* 07/26/02  EJV     05    Time Quality byte chg position to lowest.	*/
/*			   Added logging.				*/
/* 07/03/02  EJV     04    MMS_UTC_TIME: chg name usec to fraction.	*/
/* 01/22/02  JRB     03    Chg asn1r_end_of_buffer to asn1r_buf_start.	*/
/* 12/20/01  JRB     02    Chg ASN1_CTXT to ASN1_ENC_CTXT.		*/
/* 11/08/01  EJV     01    New module, derived from are_btod.c		*/
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
/*			    asn1r_wr_utc_time				*/
/************************************************************************/

ST_VOID asn1r_wr_utc_time (ASN1_ENC_CTXT *ac, MMS_UTC_TIME *data)
  {
#define	_ASN1R_UTCTIME_LEN	8

  ALOG_ENC0("Encoding UtcTime");

  /* check for overun of buffer (UtcTime is encoded on 8 bytes)		*/
  if (ac->asn1r_field_ptr - _ASN1R_UTCTIME_LEN < ac->asn1r_buf_start)
    {
    ac->asn1r_encode_overrun = SD_TRUE;
    return;
    }

  ALOG_CENC1("seconds:   %lu", (ST_ULONG) data->secs);
  ALOG_CENC1("fraction:  %lu", (ST_ULONG) data->fraction);
  ALOG_CENC1("quality:   0x%lX", (ST_ULONG) data->qflags);

  /* write the quality flags */
  *(ac->asn1r_field_ptr--) = (ST_UCHAR) data->qflags;

  /* write fraction of a second (3 bytes) */
  *(ac->asn1r_field_ptr--) = (ST_UCHAR)  (data->fraction        & 0xFF);
  *(ac->asn1r_field_ptr--) = (ST_UCHAR) ((data->fraction >> 8)  & 0xFF);
  *(ac->asn1r_field_ptr--) = (ST_UCHAR) ((data->fraction >> 16) & 0xFF);

  /* write the number of seconds since January 1, 1970			*/
  *(ac->asn1r_field_ptr--) = (ST_UCHAR)  (data->secs        & 0xFF);
  *(ac->asn1r_field_ptr--) = (ST_UCHAR) ((data->secs >> 8)  & 0xFF);
  *(ac->asn1r_field_ptr--) = (ST_UCHAR) ((data->secs >> 16) & 0xFF);
  *(ac->asn1r_field_ptr--) = (ST_UCHAR) ((data->secs >> 24) & 0xFF);

  ALOG_CENC1("Encoded %d bytes:", _ASN1R_UTCTIME_LEN);  
  ALOG_ENCH(_ASN1R_UTCTIME_LEN, ac->asn1r_field_ptr + 1);
  }


