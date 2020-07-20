/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2002, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : are_float.c						*/
/* PRODUCT(S)  : ASN1DE 						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 03/11/04  GLB     08    Remove "thisFileName"                    	*/
/* 01/14/03  JRB     07    Avoid redundant logging of encode overrun.	*/
/* 02/19/03  JRB     06    Use SD_BYTE_ORDER.				*/
/* 02/19/03  JRB     05    Del VAX VMS support. For ALPHA VMS, assume	*/
/*			   compiler option /FLOAT=IEEE_FLOAT is used	*/
/*			   so it behaves like other systems.		*/
/* 01/22/02  JRB     04    Chg asn1r_end_of_buffer to asn1r_buf_start.	*/
/* 12/20/01  JRB     03    Chg ASN1_CTXT to ASN1_ENC_CTXT.		*/
/* 09/13/99  MDE     02    Added SD_CONST modifiers			*/
/* 07/26/99  MDE     01    New module, derived from ae_float.c		*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "asn1r.h"
#include "asn1log.h"


/************************************************************************/
/*			   wr_float					*/
/* Function to convert a value of the single precision "float" type to  */
/* the MMS floating point type (of length 5 octets) and write it to the */
/* message being constructed.  The encoded floating point value is in	*/
/* the standard IEEE 754 single precision floating point format with	*/
/* the additional exponent width octet. 				*/
/************************************************************************/

ST_VOID asn1r_wr_float (ASN1_ENC_CTXT *ac, ST_FLOAT in)
  {
ST_UCHAR *ptr;
ST_FLOAT f;

  if (ac->asn1r_field_ptr - 5 < ac->asn1r_buf_start)
    {
    ac->asn1r_encode_overrun = SD_TRUE;
    return;
    }

#if defined(__OS2__)
  ptr = (ST_UCHAR *) &in;
#else
  f = (ST_FLOAT) in;
  ptr = (ST_UCHAR *) &f;
#endif

#if SD_BYTE_ORDER==SD_BIG_ENDIAN	/* big-endian (like Motorola)	*/
  f = (ST_FLOAT) in;
  ptr = (ST_UCHAR *) &f;
  ptr += 3;
  *(ac->asn1r_field_ptr--) = *(ptr--);
  *(ac->asn1r_field_ptr--) = *(ptr--);
  *(ac->asn1r_field_ptr--) = *(ptr--);
#else
  *(ac->asn1r_field_ptr--) = *(ptr++);
  *(ac->asn1r_field_ptr--) = *(ptr++);
  *(ac->asn1r_field_ptr--) = *(ptr++);
#endif
  *(ac->asn1r_field_ptr--) = *ptr;
  *(ac->asn1r_field_ptr--) = (ST_UCHAR)'\x08';
  }

/************************************************************************/
/*			      wr_double 				*/
/* Function to convert a value of the double precision "double" type to */
/* the MMS floating point type (of length 9 octets) and write it to the */
/* message being constructed.  The encoded floating point value is in	*/
/* the standard IEEE 754 double precision floating point format with	*/
/* the additional exponent width octet. 				*/
/************************************************************************/

ST_VOID asn1r_wr_double (ASN1_ENC_CTXT *ac, ST_DOUBLE in)
  {
ST_UCHAR *ptr;

  if (ac->asn1r_field_ptr - 9 < ac->asn1r_buf_start)
    {
    ac->asn1r_encode_overrun = SD_TRUE;
    return;
    }

#if SD_BYTE_ORDER==SD_BIG_ENDIAN	/* big-endian (like Motorola)	*/
  ptr = (ST_UCHAR *) &in + 7;
  *(ac->asn1r_field_ptr--) = *(ptr--);
  *(ac->asn1r_field_ptr--) = *(ptr--);
  *(ac->asn1r_field_ptr--) = *(ptr--);
  *(ac->asn1r_field_ptr--) = *(ptr--);
  *(ac->asn1r_field_ptr--) = *(ptr--);
  *(ac->asn1r_field_ptr--) = *(ptr--);
  *(ac->asn1r_field_ptr--) = *(ptr--);
#else
  ptr = (ST_UCHAR *) &in;
  *(ac->asn1r_field_ptr--) = *(ptr++);
  *(ac->asn1r_field_ptr--) = *(ptr++);
  *(ac->asn1r_field_ptr--) = *(ptr++);
  *(ac->asn1r_field_ptr--) = *(ptr++);
  *(ac->asn1r_field_ptr--) = *(ptr++);
  *(ac->asn1r_field_ptr--) = *(ptr++);
  *(ac->asn1r_field_ptr--) = *(ptr++);
#endif
  *(ac->asn1r_field_ptr--) = *ptr;
  *(ac->asn1r_field_ptr--) = (ST_UCHAR)'\x0B';
  }


