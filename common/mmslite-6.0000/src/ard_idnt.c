/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : ard_idnt.c						*/
/* PRODUCT(S)  : ASN1DE 						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 04/24/03  JRB     04    Del MAX_IDENT_LEN define, get from mms_def2.h*/
/* 12/20/01  JRB     03    Chg ASN1_CTXT to ASN1_DEC_CTXT.		*/
/* 09/13/99  MDE     02    Added SD_CONST modifiers			*/
/* 07/26/99  MDE     01    New module, derived from ad_idnt.c		*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include <ctype.h>
#include "asn1r.h"
#include "asn1log.h"
#include "mms_def2.h"	/* need MAX_IDENT_LEN define	*/

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/
/*			     get_identifier				*/
/* Reads visible character data from message into selected buffer. If	*/
/* character is encountered which is not valid for ASN.1 VisibleString	*/
/* type, error code will be returned as function return value. Other-	*/
/* wise, function return value will be zero. This function will verify	*/
/* that buffer is big enough. Conversion from ASCII to local character	*/
/* representation is not necessary for MS-DOS. 			*/
/************************************************************************/

ST_RET asn1r_get_identifier (ASN1_DEC_CTXT *ac, ST_CHAR *ptr)
  {
ST_INT i;

#ifdef DEBUG_ASN1_DECODE
  if (!ptr)
    {
    slogCallStack (sLogCtrl,
                    "get_identifier: attempt to reference through a NULL pointer");
    return(SD_FAILURE);
    }
#endif

  if (ac->asn1r_elmnt_len > MAX_IDENT_LEN)
    {				/* if the name is to long return error */
    ALOG_NERR1 ("ASN.1 decode: Identifier too long (%u) ", 
					(ST_UINT) ac->asn1r_elmnt_len);
    return (SD_FAILURE);
    }

  if (isdigit(*ac->asn1r_field_ptr))	 /* the first letter may not be a digit  */
    {
    ALOG_NERR0 ("ASN.1 decode: Identifier cannot start with a digit");
    return (SD_FAILURE);
    }

  for (i = ac->asn1r_elmnt_len; i; --i)
    {
    if ((!(isalnum(*ac->asn1r_field_ptr))) &&
	    (!(*ac->asn1r_field_ptr == '_')) &&
	    (!(*ac->asn1r_field_ptr == '$')))
      {
      ALOG_NERR1 ("ASN.1 decode: invalid character (0x%02x) in identifier",
					(int) *ac->asn1r_field_ptr);
      return (SD_FAILURE);
      }

    *ptr = (ST_CHAR) *(ac->asn1r_field_ptr++); /* Move character to user buffer.	*/
    ptr++;
    }
  *ptr = '\x00';                /* Null terminate the visible string.   */
  return(SD_SUCCESS);			/* If no problelms, return success.	*/
  }
