/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2006, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : are_time.c 						*/
/* PRODUCT(S)  : ASN1DE 						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 03/23/06  EJV     07    Cast time_t var to (ST_ULONG) and use '%lu'.	*/
/* 01/14/03  JRB     06    Avoid redundant logging of encode overrun.	*/
/* 07/29/02  EJV     05    Log tmp_buf instead of ac->asn1r_field_ptr.	*/
/* 01/22/02  JRB     04    Chg asn1r_end_of_buffer to asn1r_buf_start.	*/
/* 12/20/01  JRB     03    Chg ASN1_CTXT to ASN1_ENC_CTXT.		*/
/* 09/13/99  MDE     02    Added SD_CONST modifiers			*/
/* 07/26/99  MDE     01    New module, derived from ae_time.c		*/
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

/* for wr_time the length of the string */
#define ASN1_WR_TIME_ENC_L 15

/************************************************************************/
/*			      wr_time					*/
/*  Function to encode date and time from MS-DOS time (= number of	*/
/*  seconds from 00:00 Jan 1, 1970, GMT) to the ASN.1 Generalized time	*/
/************************************************************************/
ST_VOID asn1r_wr_time (ASN1_ENC_CTXT *ac, time_t toEncodeTime)
  {
struct tm *gmtTime;
ST_UCHAR tmp_buf [ASN1_WR_TIME_ENC_L + 1];

  ALOG_ENC0("Encoding a GMT Zulu based time string");
    
  /* check to see if the ASN1 field pointer will overflow the buffer */
  if (ac->asn1r_field_ptr - ASN1_WR_TIME_ENC_L < ac->asn1r_buf_start)
    {
    ac->asn1r_encode_overrun = SD_TRUE;
    return;
    }

  /* move the asn1 field ptr backward in the buffer */
  ac->asn1r_field_ptr = ac->asn1r_field_ptr - (ASN1_WR_TIME_ENC_L-1);

  ALOG_CENC1("Passed Unix epoche based GMT time is (in) = %lu", (ST_ULONG) toEncodeTime);

  gmtTime = gmtime(&toEncodeTime);
    
  /* encode a local time string */
  strftime ((ST_CHAR *)tmp_buf,ASN1_WR_TIME_ENC_L + 1,"%Y%m%d%H%M%SZ",gmtTime);
  memcpy (ac->asn1r_field_ptr, tmp_buf, ASN1_WR_TIME_ENC_L); /* no 0x00 copied */

  ALOG_CENC1("ASN1 encoded String %s", tmp_buf);  
  
  /* move the field pointer 1 more position back in the encode buffer */
  ac->asn1r_field_ptr = ac->asn1r_field_ptr -1;
  }
