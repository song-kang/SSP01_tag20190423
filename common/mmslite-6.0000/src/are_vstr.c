/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2004, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : are_vstr.c						*/
/* PRODUCT(S)  : ASN1DE 						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 11/23/11  KCR           Added casting for proper 64bit compile       */
/* 08/01/05  JRB     09    Add const on asn1_wr_vstr args.		*/
/* 03/11/04  GLB     08    Added "ifdef DEBUG_SISCO" to "thisFileName"  */
/* 01/16/04  EJV     07    Added typecast to elim warning on AIX.	*/
/* 01/14/03  JRB     06    Avoid redundant logging of encode overrun.	*/
/* 09/24/03  JRB     05    Fix to properly encode 0-length UTF8string.	*/
/* 04/02/03  JRB     04    Add UTF8string support (asn1r_wr_utf8)	*/
/* 12/20/01  JRB     03    Chg ASN1_CTXT to ASN1_ENC_CTXT.		*/
/* 09/13/99  MDE     02    Added SD_CONST modifiers			*/
/* 07/26/99  MDE     01    New module, derived from ae_vstr.c		*/
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
/*			     wr_vstr					*/
/************************************************************************/

ST_VOID asn1r_wr_vstr (ASN1_ENC_CTXT *ac, const ST_CHAR *ptr)
  {
  asn1r_wr_octstr (ac, (ST_UCHAR*)ptr, (signed int) strlen (ptr));
  }

/************************************************************************/
/*			asn1r_wr_utf8					*/
/************************************************************************/

ST_RET asn1r_wr_utf8 (ASN1_ENC_CTXT *aCtx, ST_CHAR *ptr, ST_INT el_len)
  {
ST_CHAR *utf8_ptr;	/* temporary place to encode UTF8string.	*/
ST_INT utf8_len;

/* This code is a little tricky. Since we encode backwards, the data must
 * be written before the current "ASN1_ENC_PTR". Since we don't know
 * the len of the data until after the conversion (asn1r_local_to_utf8),
 * we allow for the maximum converted size, then after the conversion, we move
 * the data forward to just before "ASN1_ENC_PTR".
 */ 

  /* Compute maximum converted size.
   * abs (el_len) = maximum number of Unicode "characters".
   * Multiply by the maximum size of each Unicode "character" (4).
   */
  utf8_len = (abs(el_len)*4);		/* 4 bytes per char	*/

  /* Point to safe spot in ASN.1 encode buffer where maximum size
   * UTF8string can be encoded (including NULL terminator).
   */
  utf8_ptr = (ST_CHAR *) ASN1_ENC_PTR(aCtx) - utf8_len;
  if (utf8_ptr < (ST_CHAR *) aCtx->asn1r_buf_start)
    {
    aCtx->asn1r_encode_overrun = SD_TRUE;
    return (SD_FAILURE);
    }

  /* Convert data to UTF8string (NOT NULL terminated).
   * NOTE: utf8_len is overwritten with the actual number of encoded bytes.
   */
  utf8_len = asn1r_local_to_utf8 (utf8_ptr, utf8_len, ptr);
  if (utf8_len < 0)	/* 0 len is OK.	*/
    {
    ALOG_NERR0 ("wr_utf8: conversion from local to UTF8 failed");
    return (SD_FAILURE);
    }
  /* Move data forward in ASN.1 encode buffer.	*/
  if (utf8_len > 0)
    {
    memmove (ASN1_ENC_PTR(aCtx)-utf8_len, (ST_UCHAR*)utf8_ptr, utf8_len);
    aCtx->asn1r_field_ptr -= utf8_len;	/* update ASN.1 encode ptr	*/
    }
  return (SD_SUCCESS);
  }

#if (UNICODE_LOCAL_FORMAT==UNICODE_UTF16)
#if defined(_WIN32)
#include <windows.h>
/************************************************************************/
/* On Windows, the local format is UTF16, so this function converts	*/
/* from UTF16 string to UTF8 string.					*/
/* The source string (at src_ptr) must be NULL terminated.		*/
/* dst_len = num bytes available in dst "UTF8" string.			*/
/* RETURN: num bytes in dst (not including NULL).			*/
/************************************************************************/
ST_INT asn1r_local_to_utf8 (ST_CHAR *dst_ptr, ST_INT dst_len, ST_CHAR *src_ptr)
  {
ST_INT count;

/* NOTE: pass (src_len==-1) to WideCharToMultiByte. 
 * "src_ptr" must be NULL terminated, and chars converted
 * up to and including the NULL.
 * Return value is len of dst, possibly including NULL terminator.
 */
  count = WideCharToMultiByte (CP_UTF8, 0,
            (ST_UINT16 *) src_ptr, -1, dst_ptr, dst_len, NULL, NULL);
  if (count > 0)
    {
    /* "count" MAY include the NULL terminator. If so, return 1 less.	*/
    if (dst_ptr[count-1]=='\0')
      return (count-1);	/* if count includes NULL, subtract 1	*/
    else
      return (count);	/* if count does not include NULL, return count	*/
    }
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
/* The source string (at src_ptr) must be NULL terminated.		*/
/* This function just copies up to (not including the NULL terminator).	*/
/* dst_len = num bytes available in dst "UTF8" string.			*/
/* CRITICAL: dst_len must be >= src_len					*/
/* RETURN: num bytes in dst (not including NULL).			*/
/************************************************************************/
ST_INT asn1r_local_to_utf8 (ST_CHAR *dst_ptr, ST_INT dst_len, ST_CHAR *src_ptr)
  {
ST_INT ret = -1;	/* assume error	*/
ST_INT src_len;

  src_len = strlen(src_ptr);	/* compute src_len	*/
  if (src_len <= dst_len)
    {
    if (src_len > 0)
      strncpy (dst_ptr, src_ptr, src_len);	/* not including NULL terminator*/
    ret = src_len;			/* success	*/
    }
  return (ret);
  }
#endif	/* UNICODE_LOCAL_FORMAT==UNICODE_UTF8	*/
