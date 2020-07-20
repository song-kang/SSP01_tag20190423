/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2005, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : ard_int.c 						*/
/* PRODUCT(S)  : ASN1DE 						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 08/24/09  JRB     09    Fix signed int out of range error detection.	*/
/*			   Use separate signed & unsigned functions.	*/
/*			   Simplify negative value handling.		*/
/* 10/03/08  JRB     08    Fix crash by testing for asn1r_elmnt_len < 1.*/
/* 06/10/05  JRB     07    Use i64 suffix on WIN32, default to LL suffix*/
/* 03/03/05  EJV     06    Use LL suffix also on linux.			*/
/* 03/16/04  EJV     05    Added ST_(U)LONG typecast to logs,on some sys*/
/*			     ST_(U)INT32 can be (unsigned) long or int.	*/
/*			   asn1r_get_u8 (_u16): fixed potential problem.*/
/* 01/08/04  EJV     04    asn1r_get_int64: added define for sun.	*/
/* 12/20/01  JRB     03    Chg ASN1_CTXT to ASN1_DEC_CTXT.		*/
/* 09/13/99  MDE     02    Added SD_CONST modifiers			*/
/* 07/26/99  MDE     01    New module, derived from ad_int.c		*/
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
/************************************************************************/

static ST_RET asn1r_get_int32  (ASN1_DEC_CTXT *ac, ST_INT32 *ptr);
static ST_RET asn1r_get_uint32 (ASN1_DEC_CTXT *ac, ST_UINT32 *ptr);
#ifdef INT64_SUPPORT
static ST_RET asn1r_get_int64  (ASN1_DEC_CTXT *ac, ST_INT64 *ptr);
static ST_RET asn1r_get_uint64 (ASN1_DEC_CTXT *ac, ST_UINT64 *ptr);
#endif

/************************************************************************/
/*			     get_i8					*/
/* Function to read a short integer from a message being decoded.	*/
/* Assumes asn1_field_ptr points to data (content field), and asn1_elmnt_len	*/
/* equals exactly one byte.  Leaves pointer set to next data element.	*/
/* Returns 0 if OK, non-zero if error.					*/
/************************************************************************/

ST_RET asn1r_get_i8 (ASN1_DEC_CTXT *ac, ST_INT8 *ptr)
  {
ST_INT32 value;
ST_RET ret;

  ret = asn1r_get_int32 (ac, &value);
  if (ret == SD_SUCCESS && (value > 127 || value < -128))
    {
    ALOG_NERR1 ("ASN.1 decode: Signed8 out of range (%ld)", (ST_LONG) value);
    return (SD_FAILURE);
    }

  *ptr = (ST_CHAR) value;
  return (ret);
  }

/************************************************************************/
/*			      get_i16					*/
/* Function to read an integer from a message being decoded.		*/
/* Assume asn1_field_ptr points to data, and asn1_elmnt_len		*/
/* equals one or two bytes.  Leaves pointer set to next data element.	*/
/* Returns 0 if OK, non-zero if error.					*/
/************************************************************************/

ST_RET asn1r_get_i16 (ASN1_DEC_CTXT *ac, ST_INT16 *ptr)
  {
ST_INT32 value;
ST_RET ret;

  ret = asn1r_get_int32 (ac, &value);
  if (ret == SD_SUCCESS && (value > 32767 || value < -32768))
    {
    ALOG_NERR1 ("ASN.1 decode: Signed16 out of range (%ld)", (ST_LONG) value);
    return (SD_FAILURE);
    }

  *ptr = (ST_INT16) value;
  return (ret);
  }

/************************************************************************/
/*			      get_i32					*/
/* Function to read a long integer from a message being decoded.	*/
/* Assumes asn1_field_ptr points to data, and asn1_elmnt_len		*/
/* equals one to four bytes.  Leaves pointer set to next data element.	*/
/* Returns 0 if OK, non-zero if error.					*/
/************************************************************************/

ST_RET asn1r_get_i32 (ASN1_DEC_CTXT *ac, ST_INT32 *ptr)
  {
ST_INT32 value;
ST_RET ret;

  ret = asn1r_get_int32 (ac, &value);
  *ptr = value;
  return (ret);  
  }

/************************************************************************/
/*				get_u8					*/
/* Function to read an ASN.1 INTEGER from a message being decoded	*/
/* into a 1-byte unsigned char.  Assumes asn1_field_ptr points to data	*/
/* field and asn1_elmnt_len has 1 or 2 bytes.  Leaves asn1_field_ptr pointing to	*/
/* next data element.  Returns 0 if OK, non-zero if error.		*/
/************************************************************************/

ST_RET asn1r_get_u8 (ASN1_DEC_CTXT *ac, ST_UCHAR *ptr)
  {
ST_UINT32 value;
ST_RET ret;

  ret = asn1r_get_uint32 (ac, &value);
  if (ret == SD_SUCCESS && value > 0xFF)
    {
    ALOG_NERR1 ("ASN.1 decode: Unsigned16 out of range (%lu)", (ST_ULONG) value);
    return (SD_FAILURE);
    }

  *ptr = (ST_UCHAR) value;
  return (ret);
  }

/************************************************************************/
/*			    get_u16					*/
/* Function to read an ASN.1 INTEGER from a message being decoded	*/
/* into a 2-byte unsigned integer.  Assumes asn1_field_ptr points to data	*/
/* field and asn1_elmnt_len has 1 to 3 bytes.  Leaves asn1_field_ptr pointing to	*/
/* next data element.  Returns 0 if OK, non-zero if error.		*/
/************************************************************************/

ST_RET asn1r_get_u16 (ASN1_DEC_CTXT *ac, ST_UINT16 *ptr)
  {
ST_UINT32 value;
ST_RET ret;

  ret = asn1r_get_uint32 (ac, &value);
  if (ret == SD_SUCCESS && value > 0xFFFF)
    {
    ALOG_NERR1 ("ASN.1 decode: Unsigned16 out of range (%lu)", (ST_ULONG) value);
    return (SD_FAILURE);
    }

  *ptr = (ST_UINT16) value;
  return (ret);  
  }

/************************************************************************/
/*			    get_u32					*/
/* Function to read an ASN.1 INTEGER from a message being decoded	*/
/* into a 4-byte unsigned ST_INT32.  Assumes asn1_field_ptr points to data	*/
/* field and asn1_elmnt_len has 1 to 5 bytes.  Leaves asn1_field_ptr pointing	*/
/* to next data element.  Returns 0 if OK, non-zero if error.		*/
/************************************************************************/

ST_RET asn1r_get_u32 (ASN1_DEC_CTXT *ac, ST_UINT32 *ptr)
  {
ST_UINT32 value;
ST_RET ret;

  ret = asn1r_get_uint32 (ac, &value);
  *ptr = value;
  return (ret);  
  }

/************************************************************************/
/* Use this function ONLY for "signed" integers.			*/
/************************************************************************/
static ST_RET asn1r_get_int32 (ASN1_DEC_CTXT *ac, ST_INT32 *ptr)
  {
/* NOTE: Use all "unsigned" variables so shifting & casting does not	*/
/*       cause sign extension, then cast final result to ST_INT32.	*/
ST_UINT32 value;	/* decoded value	*/
ST_UINT32 tmp;
ST_BOOLEAN positive;
ST_INT i;

#ifdef	DEBUG_ASN1_DECODE
  if (!ptr)
    {
    slogCallStack (sLogCtrl,
                    "get_i32: attempt to reference through a NULL pointer");
    return(SD_FAILURE);
    }
#endif

/* ASN.1 spec says 'integer' must be at least 1 octet	*/
  if (ac->asn1r_elmnt_len < 1)
    {
    ALOG_ERR0 ("ASN.1 decode: length < 1 not allowed for integer");
    return (SD_FAILURE);
    }

/* Check to see if the ASN.1 data value is positive or negative		*/
  if (*ac->asn1r_field_ptr & 0x80)
    positive = SD_FALSE;
  else
    positive = SD_TRUE;

  
/* Large 'unsigned' values may be 5 bytes, with a leading 0.		*/
/* This value is 'signed', so it should NEVER be more than 4 bytes.	*/
  i = ac->asn1r_elmnt_len; 
  if (i > 4)
    {
    ALOG_NERR0 ("ASN.1 decode: invalid 'signed' integer encoding (more than 4 bytes)");
    return (SD_FAILURE);
    }

/* Read the value into a ST_UINT32, shifting as we go	*/
  if (!positive)
    value = 0xFFFFFF00;	/* negative value, start with high bits set	*/
  else
    value = 0;		/* positive value, start with 0			*/
  while (SD_TRUE)
    {
    /* asn1r_field_ptr is (ST_UCHAR *), so this cast should NEVER sign extend.*/
    tmp = (ST_UINT32) *(ac->asn1r_field_ptr++);
    value |= tmp;
    if (--i == 0)
      break;

    value <<= 8;	/* more bytes to decode. Make room for next byte.*/
    }

/* Finally, after all shifting & masking, cast it to 'signed' value.	*/
  *ptr = (ST_INT32) value;
  return (SD_SUCCESS);
  }

/************************************************************************/
/* Use this function ONLY for "unsigned" integers.			*/
/************************************************************************/
static ST_RET asn1r_get_uint32 (ASN1_DEC_CTXT *ac, ST_UINT32 *ptr)
  {
ST_UINT32 value;	/* decoded value	*/
ST_UINT32 tmp;
ST_INT i;

#ifdef	DEBUG_ASN1_DECODE
  if (!ptr)
    {
    slogCallStack (sLogCtrl,
                    "get_u32: attempt to reference through a NULL pointer");
    return(SD_FAILURE);
    }
#endif

/* ASN.1 spec says 'integer' must be at least 1 octet	*/
  if (ac->asn1r_elmnt_len < 1)
    {
    ALOG_ERR0 ("ASN.1 decode: length < 1 not allowed for integer");
    return (SD_FAILURE);
    }

/* ASN.1 data value MUST BE positive.		*/
  if (*ac->asn1r_field_ptr & 0x80)
    {
    ALOG_NERR0 ("ASN.1 decode: Negative number received for unsigned integer");
    return (SD_FAILURE);
    }

/* Large 'unsigned' values may be 5 bytes, with a leading 0.		*/
/* If so, we strip the leading 0.					*/
  i = ac->asn1r_elmnt_len; 
  if (i > 5)
    {
    ALOG_NERR0 ("ASN.1 decode: invalid UINT32 encoding (more than 5 bytes)");
    return (SD_FAILURE);
    }
  if (i == 5)	/* Large positive number, first byte must be 0.	*/
    {
    if (*ac->asn1r_field_ptr != 0)
      {
      ALOG_NERR0 ("ASN.1 decode: invalid UINT32 encoding (5 bytes but first byte != 0)");
      return (SD_FAILURE);
      }
    ++ac->asn1r_field_ptr;	/* skip the leading 0	*/
    --i;
    }

/* Read the value into a ST_UINT32, shifting as we go	*/
  value = 0;
  while (SD_TRUE)
    {
    /* asn1r_field_ptr is (ST_UCHAR *), so this cast should NEVER sign extend.*/
    tmp = (ST_UINT32) *(ac->asn1r_field_ptr++);
    value |= tmp;
    if (--i == 0)
      break;

    value <<= 8;	/* more bytes to decode. Make room for next byte.*/
    }

  *ptr = value;
  return (SD_SUCCESS);
  }

/************************************************************************/
#ifdef INT64_SUPPORT
/************************************************************************/
/************************************************************************/

ST_RET asn1r_get_u64 (ASN1_DEC_CTXT *ac, ST_UINT64 *ptr)
  {
ST_UINT64 value;
ST_RET ret;

  ret = asn1r_get_uint64 (ac, &value);
  *ptr = value;
  return (ret);  
  }

/************************************************************************/

ST_RET asn1r_get_i64 (ASN1_DEC_CTXT *ac, ST_INT64 *ptr)
  {
ST_INT64 value;
ST_RET ret;

  ret = asn1r_get_int64 (ac, &value);
  *ptr = value;
  return (ret);  
  }

/************************************************************************/
/* Use this function ONLY for "signed" integers.			*/
/************************************************************************/
static ST_RET asn1r_get_int64 (ASN1_DEC_CTXT *ac, ST_INT64 *ptr)
  {
/* NOTE: Use all "unsigned" variables so shifting & casting does not	*/
/*       cause sign extension, then cast final result to ST_INT64.	*/
ST_UINT64 value;	/* decoded value	*/
ST_UINT64 tmp;
ST_BOOLEAN positive;
ST_INT i;

#ifdef	DEBUG_ASN1_DECODE
  if (!ptr)
    {
    slogCallStack (sLogCtrl,
                    "get_i64: attempt to reference through a NULL pointer");
    return(SD_FAILURE);
    }
#endif

/* ASN.1 spec says 'integer' must be at least 1 octet	*/
  if (ac->asn1r_elmnt_len < 1)
    {
    ALOG_ERR0 ("ASN.1 decode: length < 1 not allowed for integer");
    return (SD_FAILURE);
    }

/* Check to see if the ASN.1 data value is positive or negative		*/
  if (*ac->asn1r_field_ptr & 0x80)
    positive = SD_FALSE;
  else
    positive = SD_TRUE;

  
/* Large 'unsigned' values may be 9 bytes, with a leading 0.		*/
/* This value is 'signed', so it should NEVER be more than 8 bytes.	*/
  i = ac->asn1r_elmnt_len; 
  if (i > 8)
    {
    ALOG_NERR0 ("ASN.1 decode: invalid 'signed' integer encoding (more than 8 bytes)");
    return (SD_FAILURE);
    }

/* Read the value into a ST_UINT64, shifting as we go	*/
  if (!positive)
    {
#if defined(_WIN32)
    value = 0xffffffffffffff00i64;	/* negative value, start with high bits set*/
#else
    value = 0xffffffffffffff00LL;	/* negative value, start with high bits set*/
#endif
    }
  else
    value = 0;				/* positive value, start with 0	*/
  while (SD_TRUE)
    {
    /* asn1r_field_ptr is (ST_UCHAR *), so this cast should NEVER sign extend.*/
    tmp = (ST_UINT64) *(ac->asn1r_field_ptr++);
    value |= tmp;
    if (--i == 0)
      break;

    value <<= 8;	/* more bytes to decode. Make room for next byte.*/
    }

/* Finally, after all shifting & masking, cast it to 'signed' value.	*/
  *ptr = (ST_INT64) value;
  return (SD_SUCCESS);
  }

/************************************************************************/
/* Use this function ONLY for "unsigned" integers.			*/
/************************************************************************/
static ST_RET asn1r_get_uint64 (ASN1_DEC_CTXT *ac, ST_UINT64 *ptr)
  {
ST_UINT64 value;	/* decoded value	*/
ST_UINT64 tmp;
ST_INT i;

#ifdef	DEBUG_ASN1_DECODE
  if (!ptr)
    {
    slogCallStack (sLogCtrl,
                    "get_u64: attempt to reference through a NULL pointer");
    return(SD_FAILURE);
    }
#endif

/* ASN.1 spec says 'integer' must be at least 1 octet	*/
  if (ac->asn1r_elmnt_len < 1)
    {
    ALOG_ERR0 ("ASN.1 decode: length < 1 not allowed for integer");
    return (SD_FAILURE);
    }

/* ASN.1 data value MUST BE positive.		*/
  if (*ac->asn1r_field_ptr & 0x80)
    {
    ALOG_NERR0 ("ASN.1 decode: Negative number received for unsigned integer");
    return (SD_FAILURE);
    }

/* Large 'unsigned' values may be 9 bytes, with a leading 0.		*/
/* If so, we strip the leading 0.					*/
  i = ac->asn1r_elmnt_len; 
  if (i > 9)
    {
    ALOG_NERR0 ("ASN.1 decode: invalid UINT64 encoding (more than 9 bytes)");
    return (SD_FAILURE);
    }
  if (i == 9)	/* Large positive number, first byte must be 0.	*/
    {
    if (*ac->asn1r_field_ptr != 0)
      {
      ALOG_NERR0 ("ASN.1 decode: invalid UINT64 encoding (9 bytes but first byte != 0)");
      return (SD_FAILURE);
      }
    ++ac->asn1r_field_ptr;	/* skip the leading 0	*/
    --i;
    }

/* Read the value into a ST_UINT64, shifting as we go	*/
  value = 0;
  while (SD_TRUE)
    {
    /* asn1r_field_ptr is (ST_UCHAR *), so this cast should NEVER sign extend.*/
    tmp = (ST_UINT64) *(ac->asn1r_field_ptr++);
    value |= tmp;
    if (--i == 0)
      break;

    value <<= 8;	/* more bytes to decode. Make room for next byte.*/
    }

  *ptr = value;
  return (SD_SUCCESS);
  }

/************************************************************************/
#endif /* #ifdef INT64_SUPPORT */
/************************************************************************/
