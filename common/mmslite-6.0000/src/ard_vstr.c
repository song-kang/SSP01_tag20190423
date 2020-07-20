/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2004, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : ard_vstr.c 						*/
/* PRODUCT(S)  : ASN1DE 						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 12/05/05  JRB     09    Add asn1r_get_vstr_maxlen.			*/
/* 03/22/05  JRB     08    Fix cast in log macro.			*/
/* 01/16/04  EJV     07    Added typecast to elim warning on AIX.	*/
/* 09/24/03  JRB     06    Fix to properly decode 0-length UTF8string.	*/
/* 04/22/03  JRB     05    Allow fixed len UTF8 only if local fmt=UTF16.*/
/* 04/02/03  JRB     04    Add UTF8string support (asn1r_get_utf8)	*/
/* 12/20/01  JRB     03    Chg ASN1_CTXT to ASN1_DEC_CTXT.		*/
/* 09/13/99  MDE     02    Added SD_CONST modifiers			*/
/* 07/26/99  MDE     01    New module, derived from ad_vstr.c		*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include <ctype.h>
#include "asn1r.h"
#include "asn1log.h"

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/
/*			asn1r_get_vstr_maxlen				*/
/* Just like asn1r_get_vstr but checks the string length.		*/
/* NOTE: caller's buffer must be 1 byte bigger than max_len to allow	*/
/*       room for NULL terminator.					*/
/************************************************************************/
ST_RET asn1r_get_vstr_maxlen (ASN1_DEC_CTXT *aCtx, ST_CHAR *ptr, ST_INT max_len)
  {
  if (aCtx->asn1r_elmnt_len > max_len)
    {
    ALOG_ERR0("ASN.1 decode: visible string too long");
    return (SD_FAILURE);
    }
  else
    return (asn1r_get_vstr (aCtx, ptr));
  }

/************************************************************************/
/*			     get_vstr					*/
/* Reads visible character data from message into selected buffer. If	*/
/* character is encountered which is not valid for ASN.1 VisibleString	*/
/* type, error code will be returned as function return value. Other-	*/
/* wise, function return value will be zero. The calling function must	*/
/* verify that buffer is big enough. Conversion from ASCII to local	*/
/* character representation is not necessary for MS-DOS.		*/
/************************************************************************/

ST_RET asn1r_get_vstr (ASN1_DEC_CTXT *ac, ST_CHAR *ptr)
  {
ST_INT i;

#ifdef	DEBUG_ASN1_DECODE
  if (!ptr)
    {
    slogCallStack (sLogCtrl,
                    "get_vstr: attempt to reference through a NULL pointer");
    return(SD_FAILURE);
    }
#endif

  for (i = ac->asn1r_elmnt_len; i; --i)
    {
    *ptr = (ST_CHAR) *(ac->asn1r_field_ptr++); /* Move character to user buffer.	*/

    if (*ptr < ' ')     /* Check that character is not a control char.  */
      {
      ALOG_NERR1 ("ASN.1 decode: invalid character (0x%02x) in visible string",
                  (ST_UINT)(ST_UCHAR) *ptr);	/* 2 casts to avoid sign extension*/
      return (SD_FAILURE);
      }
    ptr++;
    }
  *ptr = '\x00';                /* Null terminate the visible string.   */
  return(SD_SUCCESS);			/* If no problelms, return success.	*/
  }

/************************************************************************/
/*			asn1r_get_utf8					*/
/* Reads Unicode UTF8 string data from message into selected buffer. If	*/
/* character is encountered which is not valid for ASN.1 UTF8string	*/
/* type, error code will be returned as function return value. Other-	*/
/* wise, function return value will be zero. The calling function must	*/
/* verify that buffer is big enough.					*/
/************************************************************************/

ST_RET asn1r_get_utf8 (ASN1_DEC_CTXT *aCtx, ST_CHAR *ptr, ST_INT el_len)
  {
ST_INT dst_len, local_len;

#ifdef	DEBUG_ASN1_DECODE
  if (!ptr)
    {
    slogCallStack (sLogCtrl,
                    "get_utf8: attempt to reference through a NULL pointer");
    return(SD_FAILURE);
    }
#endif

/* Convert from UTF8 to local format. Input is NOT NULL terminated.
 * abs (el_len) = maximum number of Unicode "characters".
 * Multiply by the maximum size of each Unicode "character".
 */
#if (UNICODE_LOCAL_FORMAT==UNICODE_UTF16)
  dst_len = abs(el_len)*2;
#else				/* default is UTF8	*/
  dst_len = abs(el_len)*4;
#endif
  local_len = asn1r_utf8_to_local (ptr, dst_len, (ST_CHAR *) aCtx->asn1r_field_ptr, aCtx->asn1r_elmnt_len);  
  if (local_len < 0)
    {
    ALOG_NERR0 ("ASN.1 decode: UTF8 to local conversion failed");
    return(SD_FAILURE);
    }
  /* If this is "fixed length" (el_len>0), length must match.	*/
#if (UNICODE_LOCAL_FORMAT==UNICODE_UTF16)
  if (el_len > 0 && el_len*2 != local_len)	/* Fixed len string	*/
    {
    ALOG_NERR2 ("ASN.1 decode: expected UTF8string%d, received UTF8string%d",
                el_len, local_len/2);
    return (SD_FAILURE);
    }
  *(ST_UINT16*)(ptr+local_len) = 0;	/* add "2 byte" NULL	*/
#else			/* default is UTF8	*/
  if (el_len > 0)	/* Fixed len string	*/
    {
    /* This error should never occur because no way to define fixed len UTF8.*/
    ALOG_ERR0 ("ASN.1 decode: 'FIXED LENGTH' UTF8string not supported on this platform");
    return (SD_FAILURE);
    }
  ptr[local_len] = '\0';		/* add NULL		*/
#endif

  aCtx->asn1r_field_ptr += aCtx->asn1r_elmnt_len;	/* Update decode ptr*/
  return(SD_SUCCESS);
  }

#if (UNICODE_LOCAL_FORMAT==UNICODE_UTF16)
#if defined(_WIN32)
#include <windows.h>
/************************************************************************/
/* On Windows, the local format is UTF16, so this function converts	*/
/* from UTF8 string to UTF16 string.					*/
/* CRITICAL: "dst" is NOT automatically NULL terminated, so calling	*/
/* function must do so if necessary.					*/
/* dst_len = num bytes in dst "UTF16" string.				*/
/* src_len = num bytes in src "UTF8" string.				*/
/************************************************************************/
ST_INT asn1r_utf8_to_local (ST_CHAR *dst, ST_INT dst_len, ST_CHAR *src, ST_INT src_len)
  {
int ret;
ST_UINT16 *dst_utf16 = (ST_UINT16 *) dst;

  /* This function expects the max number of UTF16 chars (2 bytes per char)
   * in the last arg, so divide "dst_len" by 2.
   * It does NOT automatically NULL terminate, so neither does this function. 
   * It returns the number of UTF16 chars converted, so we multiply the
   * return value by 2 to return the number of bytes.
   */
  ret = MultiByteToWideChar (CP_UTF8, 0,
            src, src_len, dst_utf16, dst_len/2);
  if (ret >= 0)
    return (ret*2);	/* success	*/
  return (-1);		/* error	*/
  }
#else	/* All other systems	*/
#error Unicode UTF16 currently only supported on Windows
#endif
#else	/* UNICODE_LOCAL_FORMAT==UNICODE_UTF8	*/
/************************************************************************/
/* This function should work on any system where the local Unicode	*/
/* format is UTF8. The ASN.1 format is also UTF8, so this function	*/
/* basically just copies the string.					*/
/* CRITICAL: dst_len must be >= src_len					*/
/************************************************************************/
ST_INT asn1r_utf8_to_local (ST_CHAR *dst, ST_INT dst_len, ST_CHAR *src, ST_INT src_len)
  {
  if (dst_len >= src_len)	/* make sure dst has room	*/
    {
    if (src_len > 0)
      memcpy (dst, src, src_len);
    return (src_len);	/* success	*/
    }
  return (-1);		/* error	*/
  }
#endif	/* UNICODE_LOCAL_FORMAT==UNICODE_UTF8	*/
