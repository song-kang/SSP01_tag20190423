/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : ard_float.c 						*/
/* PRODUCT(S)  : ASN1DE 						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 10/03/08  JRB     08    Make sure elmnt_len is legal.		*/
/* 03/11/04  GLB     07    Remove "thisFileName"                    	*/
/* 02/19/03  JRB     06    Use SD_BYTE_ORDER.				*/
/* 02/19/03  JRB     05    Del VAX VMS support. For ALPHA VMS, assume	*/
/*			   compiler option /FLOAT=IEEE_FLOAT is used	*/
/*			   so it behaves like other systems.		*/
/* 03/05/02  JRB     04    Eliminate warnings.				*/
/* 12/20/01  JRB     03    Chg ASN1_CTXT to ASN1_DEC_CTXT.		*/
/* 09/13/99  MDE     02    Added SD_CONST modifiers			*/
/* 07/26/99  MDE     01    New module, derived from ad_float.c		*/
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

#ifdef ASN1_ARB_FLOAT
static ST_INT16 _fp_common (ASN1_DEC_CTXT *ac, ST_INT16 *sign, ST_UINT16 *exp, ST_UCHAR *lptr, 
			    ST_INT16 *m, ST_UCHAR *next, ST_INT16 doubl_prec);
#endif
/************************************************************************/
/************************************************************************/
/*			    get_float					*/
/* Function to convert the MMS floating point type in the ASN.1 message */
/* stream to the single precision "float" type of the C programming     */
/* language.  This function will handle any size mantissa (which is	*/
/* rounded to fit into the local single precision floating point format)*/
/* and any size exponent width up to 15 bits.  Exponents too large or	*/
/* small for the local representation translate into + or - infinity or */
/* zero, as the case may be.  The return codes are:			*/
/*									*/
/*		0 - finite valid floating pt # decoded successfully	*/
/*		1 - invalid floating point number			*/
/*		2 - exponent length greater than 15			*/
/*	       -1 - floating point value = NaN				*/
/*	       -2 - floating point value = + infinity			*/
/*	       -3 - floating point value = - infinity			*/
/*									*/
/************************************************************************/

ST_RET asn1r_get_float (ASN1_DEC_CTXT *ac, ST_FLOAT *out)
 {
ST_UCHAR n;			/* length of the exponent part		*/
ST_UCHAR *ptr;			/* char ptr to output float variable out*/
ST_UCHAR *next;			/* points to next data element		*/
ST_UCHAR *original_asn1_field_ptr;
ST_BOOLEAN float_is_zero;

#ifdef ASN1_ARB_FLOAT
ST_UINT16 exp;			/* local variable to hold exponent part */
ST_INT i;			/* utility variable			*/
ST_INT16 sign;			/* sign of the floating point quantity	*/
ST_INT32 signexp;		/* contains sign and exp parts of out	*/
ST_INT16 m;			/* # of fract bits in 1st 2 exp octets	*/
ST_UINT32 *fract;		/* contains fraction part of out	*/
#endif

#ifdef	DEBUG_ASN1_DECODE
  if (!out)
    {
    slogCallStack (sLogCtrl,
                    "get_float: attempt to reference through a NULL pointer");
    return(SD_FAILURE);
    }
#endif

/* MMS spec says FloatingPoint must be at least 2 octets	*/
  if (ac->asn1r_elmnt_len < 2)
    {
    ALOG_ERR0 ("ASN.1 decode: length < 2 not allowed for FloatingPoint");
    return (SD_FAILURE);
    }

  next = ac->asn1r_field_ptr + ac->asn1r_elmnt_len;
  n = (ST_UCHAR) *(ac->asn1r_field_ptr++);			/* read exponent width	*/

/* check to see if all the octets are 0					*/

  original_asn1_field_ptr = ac->asn1r_field_ptr;
  float_is_zero = SD_TRUE;				/* assume the float = 0	*/
  while(ac->asn1r_field_ptr < next)
    {
    if (*(ac->asn1r_field_ptr++) != 0)
      {     
      float_is_zero = SD_FALSE;
      break;
      }
    }
  if (float_is_zero)
    {
    ac->asn1r_field_ptr = next;
    *out = (ST_FLOAT)0;				/* this float = 0	*/
    return(SD_SUCCESS);				/* return success	*/
    }
  else
    ac->asn1r_field_ptr = original_asn1_field_ptr;

  ptr = (ST_UCHAR *) out;

/* Treat case where MMS floating point format coincides with IEEE 754.	*/
  if (ac->asn1r_elmnt_len == 5 && n == 8)
    {
#if SD_BYTE_ORDER==SD_BIG_ENDIAN	/* big-endian (like Motorola)	*/
    *(ptr++) = (ST_UCHAR) *(ac->asn1r_field_ptr++);
    *(ptr++) = (ST_UCHAR) *(ac->asn1r_field_ptr++);
    *(ptr++) = (ST_UCHAR) *(ac->asn1r_field_ptr++);
#else
    ac->asn1r_field_ptr += 3;
    *(ptr++) = (ST_UCHAR) *(ac->asn1r_field_ptr--);
    *(ptr++) = (ST_UCHAR) *(ac->asn1r_field_ptr--);
    *(ptr++) = (ST_UCHAR) *(ac->asn1r_field_ptr--);
#endif
    *ptr = (ST_UCHAR) *ac->asn1r_field_ptr;

    ac->asn1r_field_ptr = next;
    return (SD_SUCCESS);
    }


#ifdef ASN1_ARB_FLOAT

/* Otherwise, convert arbitrary MMS floating point into local single-	*/
/* precision floating point.						*/

/* Get exponent and sign information, and read in the fraction part of	*/
/* the floating point number.						*/
  m = 15 - (ST_INT16)n;
  if ((i = _fp_common (ac,&sign,&exp,ptr,&m,next,SD_FALSE)) < 3)
    {
    *out = (ST_FLOAT)0;
    return ((ST_INT16) i);
    }

/* Shift fraction part to bottom 3 bytes, add 1, and then shift one	*/
/* more byte.  This rounds fraction to local precision and places it	*/
/* nine bits into the floating point number, where it's supposed to go. */
  fract = (ST_UINT32 *) out;
  *fract = (*fract>>m) & (ST_UINT32)0x00FFFFFFL;
  *fract = (*fract + (ST_UINT32)0x00000001L) >> 1;	/* add 1 and then shift */
  if (*fract & (ST_UINT32)0x00800000L)
    {						/* rounding may cause	*/
    exp++;					/* exponent to increment*/
    *fract &= (ST_UINT32)0x007FFFFFL;
    }

/* OR the sign, exponent and fraction parts together to make a valid	*/
/* local floating point value.						*/
  exp <<= 7;			/* ready to be ORed with other parts	*/
  signexp = ((ST_INT32) exp) << 16;
  if (sign < 0)
    signexp |= 0x80000000L;
  *fract |= (ST_UINT32)signexp;

/* Return to calling routine with success indicated.			*/
  return (SD_SUCCESS);

#else	/* No arbitrary float format support */
  return (SD_FAILURE);
#endif
 }


/************************************************************************/
/*			     get_double 				*/
/* Function to convert the MMS floating point type in the ASN.1 message */
/* stream to the double precision "double" type of the C programming    */
/* language.  This function will handle any size mantissa (which is	*/
/* rounded to fit into the local double precision floating point format)*/
/* and any size exponent width up to 15 bits.  Exponents too large or	*/
/* small for the local representation translate into + or - infinity or */
/* zero, as the case may be.  The return codes are:			*/
/*									*/
/*		0 - finite valid floating pt # decoded successfully	*/
/*		1 - invalid floating point number			*/
/*		2 - exponent length greater than 15			*/
/*	       -1 - floating point value = NaN				*/
/*	       -2 - floating point value = + infinity			*/
/*	       -3 - floating point value = - infinity			*/
/*									*/
/************************************************************************/

ST_RET asn1r_get_double (ASN1_DEC_CTXT *ac, ST_DOUBLE *out)
 {
ST_UCHAR n;			/* length of the exponent part		*/
ST_UCHAR *ptr;			/* char ptr to the out variable 	*/
ST_UCHAR *next;			/* points to next data element		*/
ST_UINT32 *h_ulong;
ST_UINT32 *l_ulong;
ST_UCHAR *original_asn1_field_ptr;
ST_BOOLEAN float_is_zero;

#ifdef ASN1_ARB_FLOAT
ST_INT garbage_bits_in_mantissa;
ST_UCHAR round;			/* flag indicating rounding, not trunc. */
ST_UINT32 temp;			/* utility variable			*/
ST_UINT16 exp;			/* value of exponent (biased)		*/
ST_INT i;			/* utility variable			*/
ST_UINT32 signexp;			/* contains sign and exp parts of out	*/
ST_INT16 sign;			/* sign of the floating point quantity	*/
ST_INT16 m;			/* # of fract bits in 1st fract octet	*/
ST_INT16   local_double_exp_width; /* The local exp width of native double	*/
ST_UINT32 *fract;			/* points to fraction part of out	*/
#endif

#ifdef	DEBUG_ASN1_DECODE
  if (!out)
    {
    slogCallStack (sLogCtrl,
                    "get_double: attempt to reference through a NULL pointer");
    return(SD_FAILURE);
    }
#endif

  next = ac->asn1r_field_ptr + ac->asn1r_elmnt_len;
  n = (ST_UCHAR) *(ac->asn1r_field_ptr++);			/* read exponent width	*/

/* check to see if all the octets are 0					*/
  original_asn1_field_ptr = ac->asn1r_field_ptr;
  float_is_zero = SD_TRUE;				/* assume the float = 0	*/
  while(ac->asn1r_field_ptr < next)
    {
    if (*(ac->asn1r_field_ptr++) != 0)
      {     
      float_is_zero = SD_FALSE;
      break;
      }
    }
  if (float_is_zero)
    {
    ac->asn1r_field_ptr = next;
    *out = (ST_FLOAT)0;				/* this float = 0	*/
    return(SD_SUCCESS);				/* return success	*/
    }
  else
    ac->asn1r_field_ptr = original_asn1_field_ptr;

  ptr = (ST_UCHAR *) out;
  h_ulong = (ST_UINT32 *) out;
  l_ulong = h_ulong + 1;

/* Treat case where MMS floating point format coincides with IEEE 754	*/
/* double precision floating point format.				*/
  if (ac->asn1r_elmnt_len == 9 && n == 11)
    {
#if SD_BYTE_ORDER==SD_BIG_ENDIAN	/* big-endian (like Motorola)	*/
    *(ptr++) = (ST_UCHAR) *(ac->asn1r_field_ptr++);
    *(ptr++) = (ST_UCHAR) *(ac->asn1r_field_ptr++);
    *(ptr++) = (ST_UCHAR) *(ac->asn1r_field_ptr++);
    *(ptr++) = (ST_UCHAR) *(ac->asn1r_field_ptr++);
    *(ptr++) = (ST_UCHAR) *(ac->asn1r_field_ptr++);
    *(ptr++) = (ST_UCHAR) *(ac->asn1r_field_ptr++);
    *(ptr++) = (ST_UCHAR) *(ac->asn1r_field_ptr++);
#else
    ac->asn1r_field_ptr += 7;
    *(ptr++) = (ST_UCHAR) *(ac->asn1r_field_ptr--);
    *(ptr++) = (ST_UCHAR) *(ac->asn1r_field_ptr--);
    *(ptr++) = (ST_UCHAR) *(ac->asn1r_field_ptr--);
    *(ptr++) = (ST_UCHAR) *(ac->asn1r_field_ptr--);
    *(ptr++) = (ST_UCHAR) *(ac->asn1r_field_ptr--);
    *(ptr++) = (ST_UCHAR) *(ac->asn1r_field_ptr--);
    *(ptr++) = (ST_UCHAR) *(ac->asn1r_field_ptr--);
#endif
    *ptr = (ST_UCHAR) *ac->asn1r_field_ptr;

    ac->asn1r_field_ptr = next;
    return (SD_SUCCESS);
    }

#ifdef ASN1_ARB_FLOAT

/* Otherwise, convert arbitrary MMS floating point into local single-	*/
/* precision floating point.						*/
/* Get exponent and sign information, and read in the fraction part of	*/
/* the floating point number.						*/
  m = 15 - (ST_INT16)n;
  if ((i = _fp_common (ac,&sign,&exp,ptr,&m,next,SD_TRUE)) < 3)
    {
    *out = (ST_DOUBLE)0;
    return ((ST_INT16) i);
    }

/* The number of bits used to store the exponent of a double precision 	*/
/* floating point number varies from machine to machine.  The following */
/* piece of code sets this local exponent width.			*/

  /* Assume all systems now use IEEE 754 format	*/
  local_double_exp_width = 11;  

/* The following piece of code calculates the number of bit positions 	*/
/* to shift the fraction bits in the mantissa.				*/
/* (local_exp_width + sign width) - (non_mantisa_bits)		*/

  garbage_bits_in_mantissa = 8 - m; /* num of garb bits in mantissa	*/
  i = (local_double_exp_width + 1) - garbage_bits_in_mantissa;
  /* was i = m + 4	*/

  fract = (ST_UINT32 *) out;

#if SD_BYTE_ORDER==SD_LITTLE_ENDIAN	/* little-endian (like Intel)	*/
  fract++;					/* high half of f.p.	*/
#endif

/* save lowest i bits that will be lost when positioning the fraction	*/

  temp =((0x00000001L << (local_double_exp_width + 1)) - 1L);
  temp = *fract & (temp >> garbage_bits_in_mantissa);    

  *fract <<= garbage_bits_in_mantissa;  	/* put fract at far left*/
  *fract >>= (local_double_exp_width + 1);    /* shift fract in place */

/* grt rid of any sign and exponent		*/
  *fract &= 0x000fffff;				/* other ports have 12  */


						/* pt at low half of fp */
#if SD_BYTE_ORDER==SD_BIG_ENDIAN	/* big-endian (like Motorola)	*/
  fract++;
#else
  fract--;
#endif

  *fract >>= (i-1);				/* low half of f.p.	*/
  if (*fract & (ST_UINT32)0x00000001L)
    round = SD_TRUE;
  else
    round = SD_FALSE;
  temp <<= (32-i);
  *fract = (*fract>>1) | temp;		/* OR with bits from top half	*/

/* Round off the fraction part to fit into the local 8-byte double-	*/
/* precision floating point number.					*/
  if (round)
    {					/* if low half is all 1's, then */
    if (*fract == 0xFFFFFFFF)		/* rounding affects hi half too */
      {
#if SD_BYTE_ORDER==SD_BIG_ENDIAN	/* big-endian (like Motorola)	*/
      *(fract--) = 0;
#else
      *(fract++) = 0;
#endif
      *fract += 1;
      if (*fract & (ST_UINT32)0xFFF00000L)	/* if adding one overflows into */
	{				/* exp part, increment exponent */
	exp += 1;
	*fract &= (ST_UINT32)0x000FFFFFL;
	}
#if SD_BYTE_ORDER==SD_BIG_ENDIAN	/* big-endian (like Motorola)	*/
      fract++;				/* end up pointing to low half	*/
#else
      fract--;				/* end up pointing to low half	*/
#endif
      }
    else
      *fract += 1;
    }

/* OR the sign, exponent and (high) fraction parts together to make a	*/
/* valid local floating point value.					*/
  exp <<= 4;			/* ready to be ORed with other parts	*/
  signexp = (ST_UINT32)exp << 16;
  if (sign < 0)
    signexp |= (ST_UINT32)0x80000000L;
#if SD_BYTE_ORDER==SD_BIG_ENDIAN	/* big-endian (like Motorola)	*/
  *(--fract) |= signexp;
#else
  *(++fract) |= signexp;
#endif

/* Return to calling routine with success indicated.			*/
  return (SD_SUCCESS);
#else
  return (SD_FAILURE);
#endif
 }

/************************************************************************/
/*			      _fp_common 				*/
/* Function to read the sign, exp, and fract parts of an MMS floating	*/
/* point type into the designated arguments.  This function represents	*/
/* the common logic of the two functions: get_float and get_double.	*/
/* Besides sign, exp, and fract, there are three other outputs from	*/
/* this function:							*/
/*	asn1_field_ptr - global variable set to next data element in msg	*/
/*	m - modified in this function to be the number of fraction bits */
/*			in the 1st octet containing any fraction bits	*/
/*	return value -	0 - valid floating pt # with value of 0 	*/
/*			1 - invalid floating point number		*/
/*			2 - exponent length greater than 15		*/
/*			3 - valid, nonzero, finite floating pt #	*/
/*		       -1 - floating point value = NaN			*/
/*		       -2 - floating point value = + infinity		*/
/*		       -3 - floating point value = - infinity		*/
/************************************************************************/

#ifdef ASN1_ARB_FLOAT

static ST_INT16 _fp_common (
	ASN1_DEC_CTXT *ac,		/* ASN1 context pointer			*/
	ST_INT16 *sign,		/* sign 				*/
	ST_UINT16 *exp,		/* exponent translated to local repr'n  */
	ST_UCHAR *lptr,		/* pointer to local f.p. #		*/
	ST_INT16 *m,		/* begins as # of fract bits in 1st two */
				/* octets, modified in this function	*/
	ST_UCHAR *next,		/* pointer to next data element 	*/
	ST_INT16 doubl_prec)	/* local precision - single or double	*/
 {
ST_INT16 lbias;			/* bias of local floating point type	*/
ST_UINT16 lmaxexp; 		/* max exponent of local f.p. type	*/
ST_INT16 lfract;  		/* local precision length in bytes	*/
ST_UINT16 bias;			/* bias for MMS f.p. type being decoded */
ST_UCHAR nonzero; 		/* flag indicating if fract part is !=0 */
ST_INT i;			/* utility variable			*/
ST_UINT16 temp1;  		/* utility variable			*/
ST_UCHAR temp2;			/* utility variable			*/

/* Initialize variables dependent on the precision of the local f.p.	*/
/* representation (either single or double).				*/
  if (doubl_prec)
    {
    lbias = 1023;
    lmaxexp = 2047;
    lfract = 7;
    }
  else
    {
    lbias = 127;
    lmaxexp = 255;
    lfract = 3;
    }

/* First, check size of exponent and fraction parts based on element	*/
/* length and number of fraction bits in 1st two bytes. 		*/
  if (ac->asn1r_elmnt_len < 3)				/* must have minimum of */
    {						/* 2 bytes for f.p. #	*/
    ac->asn1r_field_ptr = next;
    return (SD_FAILURE);
    }
  if (*m < 0)					/* exp part too big	*/
    {
    ac->asn1r_field_ptr = next;
    return (2);
    }
  if (*m >= 15) 				/* exp part too small	*/
    {
    ac->asn1r_field_ptr = next;
    return (SD_FAILURE);
    }

/* Move exponent and sign parts into *exp variable for manipulation.	*/
  temp1 = ((ST_UINT16) *(ac->asn1r_field_ptr++)) << 8;
  *exp = temp1 | ((ST_UINT16) *(ac->asn1r_field_ptr++) & (ST_UINT16)0x00ff);

/* Determine sign, and mask out.					*/
  if (*exp & (ST_UINT16)0x8000)
    {
    *sign = -1;
    *exp = *exp & (ST_UINT16)0x7FFF;		/* zero sign bit	*/
    }
  else
    *sign = 1;

/* Reset asn1_field_ptr to point to 1st octet containing any fraction bits.	*/
  if (*m > 8)
    ac->asn1r_field_ptr -= 2;
  else if (*m > 0)
    ac->asn1r_field_ptr--;

/* Calculate bias of MMS floating point number before m is changed.	*/
  bias = (ST_UINT16)0x7FFF >> (*m+1);		/* bias = 2**(n-1) - 1	*/

/* Determine if exponent part is all 1's. In this case, MMS float = NaN */
/* or infinity, so return with appropriate return code. 		*/

  *exp = *exp >> *m;		   /* get rid of fract bits on right	*/
  temp1 = (ST_UINT16)0x7FFF >> *m;    /* prepare comparison bytes		*/
  *m = *m%8;
  if (*m == 0)
    *m = 8;			   /* m = # fract bits in current octet */
  if (*exp == temp1)		   /* check for all 1's in exponent     */
    {
    nonzero = SD_FALSE;		   /* now check for all 0's in fraction */
    temp2 = (ST_UCHAR) (*(ac->asn1r_field_ptr++) & ~(0xFF<<*m));
    while (ac->asn1r_field_ptr <= next)			/* if all fract bits =	*/
      { 					/* 0, then we have a +	*/
      if (temp2)				/* or - infinity	*/
	{
	nonzero = SD_TRUE;
	break;
	}
      temp2 = (ST_UCHAR) *(ac->asn1r_field_ptr++);
      }
    ac->asn1r_field_ptr = next;
    if (nonzero)
      return (-1);				/* nonzero fract => NaN */
    else if (*sign > 0)
      return (-2);				/* case of + infinity	*/
    else
      return (-3);				/* case of - infinity	*/
    }

/* Now that we know MMS float is a valid finite value, translate its	*/
/* exponent part into the exponent for the local representation.	*/
  if ((i = (ST_INT16)*exp + lbias - (ST_INT16)bias) < 0)
    return (SD_SUCCESS); 		/* Value too small to represent locally.*/

  *exp = (ST_UINT16) i;
  if (*exp > lmaxexp)				/* biased local exp	*/
    {						/* value too big to	*/
    ac->asn1r_field_ptr = next;				/* represent locally -	*/
    if (*sign > 0)				/* treat as + or - inf	*/
      return (-2);
    else
      return (-3);
    }

/* Move fraction part into local variable.				*/
#if SD_BYTE_ORDER==SD_BIG_ENDIAN	/* big-endian (like Motorola)	*/
  for (i=0;i<lfract;i++)
    {
    if (ac->asn1r_field_ptr < next)
      *(lptr++) = (ST_UCHAR) *(ac->asn1r_field_ptr++);
    else
      {
      *(lptr++) = (ST_UCHAR) 0;		/* zero fill if local precision */
      }
    }
#else
  ac->asn1r_field_ptr += lfract;
  for (i=0;i<lfract;i++)
    {
    if (ac->asn1r_field_ptr < next)
      *(lptr++) = (ST_UCHAR) *(ac->asn1r_field_ptr--);
    else
      {
      *(lptr++) = (ST_UCHAR) 0;		/* zero fill if local precision */
      ac->asn1r_field_ptr--;			/* is greater than MMS f.p.	*/
      }
    }
    *lptr = (ST_UCHAR) *ac->asn1r_field_ptr; 	/* we know last byte < next	*/
#endif

  ac->asn1r_field_ptr = next;
  return (3);					/* normal case		*/
  }
#endif	/* end of ARB FLOAT support */
