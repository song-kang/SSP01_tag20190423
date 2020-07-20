/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : ard_bstr.c 						*/
/* PRODUCT(S)  : ASN1DE 						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 07/21/09  JRB     12    Set asn1r_bitstr_truncated flag for primitive*/
/*			   OR constructed bitstring.			*/
/* 07/21/09  JRB     11    Constructed bitstr code: enable it ALWAYS,	*/
/*			   overhaul to make sure segments are multiples	*/
/*			   of 8 bits, fix it to truncate.		*/
/* 07/07/09  JRB     10    asn1r_get_bitstr: fix bitcount if truncated.	*/
/* 04/21/08  JRB     09    Add cast to fix warning.			*/
/* 03/17/04  RKR     08    Changed thisFileName                         */
/* 03/11/04  GLB     07    Remove "thisFileName"                    	*/
/* 03/31/03  JRB     06	   asn1r_get_bitstr: add max_bits arg & truncate*/
/*			   bitstr if max_bits exceeded.			*/
/* 03/05/02  JRB     05    Eliminate warnings.				*/
/* 12/20/01  JRB     04    Chg ASN1_CTXT to ASN1_DEC_CTXT.		*/
/* 12/12/01  JRB     03    Changes to compile MMS-EASE			*/
/* 09/13/99  MDE     02    Added SD_CONST modifiers			*/
/* 07/26/99  MDE     01    New module, derived from ard_bstr.c		*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "asn1r.h"
#include "asn1log.h"

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/

#if defined(DEBUG_SISCO)
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/
/************************************************************************/

static ST_VOID  _dec_bitstr_cstr (ASN1_DEC_CTXT *ac, ST_UINT16 id_code);

/************************************************************************/
/*			    get_bitstr					*/
/*  Function to read a primitive bitstring from an ASN.1 message.	*/
/*  Truncate bitstring if it exceeds "max_bits".			*/
/************************************************************************/

ST_RET asn1r_get_bitstr (ASN1_DEC_CTXT *ac, ST_UCHAR *bit_ptr, ST_INT max_bits)
 {
ST_INT i;

#ifdef	DEBUG_ASN1_DECODE
  if (!bit_ptr)
    {
    slogCallStack (sLogCtrl,
                    "get_: attempt to reference through a NULL pointer");
    return(SD_FAILURE);
    }
#endif

  ac->asn1r_bitstr_truncated = SD_FALSE;	/* CRITICAL: init "truncated" flag*/

  if (ac->asn1r_elmnt_len < 1)   /* For element length of 0, return error code.	*/
    return (SD_FAILURE);

  i = *(ac->asn1r_field_ptr++);
  if (i>7 || i<0)
    return (2);        /* Return error code for bad unused-bits value.	*/

  if (ac->asn1r_elmnt_len == 1)  /* When there is only the unused-bits octet,	*/
    {		       /* it must = 0, and there are no data bits.	*/
    if (i!=0)
      return (2);
    ac->asn1r_bitcount = 0;
    }

  else		       /* Treat normal case where there are data bits.	*/
    {
    ac->asn1r_bitcount = ((ac->asn1r_elmnt_len-1)*8 - i); /* Compute # data bits.	*/

    /* If caller imposed a limit, truncate. */
    if (max_bits != 0  &&  ac->asn1r_bitcount > max_bits)
      {
      ac->asn1r_bitcount = max_bits;	/* truncated	*/
      ac->asn1r_bitstr_truncated = SD_TRUE;
      }

    /* Copy bitstring to user buffer	*/
    bstrcpy (bit_ptr, ac->asn1r_field_ptr, ac->asn1r_bitcount);
    ac->asn1r_field_ptr += (ac->asn1r_elmnt_len - 1);	/* point to next*/
    }
  return (SD_SUCCESS);
  }

/************************************************************************/
/*			get_bitstr_cstr 				*/
/*  Function to setup to get a constructed bitstring.			*/
/************************************************************************/

ST_VOID asn1r_get_bitstr_cstr (ASN1_DEC_CTXT *ac, ST_INT bits, ST_UCHAR *ptr)
 {
#ifdef	DEBUG_ASN1_DECODE
  if (!ptr)
    {
    slogCallStack (sLogCtrl,
         "get_bitstr_cstr: attempt to reference through a NULL pointer");
    return;
    }
#endif

  ac->asn1r_max_bits = bits;	/* max allowed # bits				*/
  ac->_ad_bitptr = ptr; 	/* pointer to bit storage area			*/
  *ac->_ad_bitptr = 0;		/* zero first byte of user's storage area       */
  ac->asn1r_bitcount = 0; 	/* running bit count				*/
  ac->asn1r_bitstr_truncated = SD_FALSE;	/* CRITICAL: init "truncated" flag*/

  ac->asn1r_u_id_fun = _dec_bitstr_cstr;		/* universal until bitstr done	*/
  ac->asn1r_c_id_fun = asn1r_class_err;		/* no more context		*/
  ac->asn1r_a_id_fun = asn1r_class_err;		/* or application		*/
  ac->asn1r_p_id_fun = asn1r_class_err;		/* or private			*/

  ac->asn1r_save_method = ac->asn1r_decode_method;
  ac->asn1r_decode_method = ASN1_CLASS_METHOD;
  ac->_asn1r_fun_save = ac->asn1r_decode_done_fun; /* save the previous done fun	*/
  ac->asn1r_decode_done_fun = asn1r_done_err;	/* can't be done for now        */
  ac->_asn1r_cstr_done_save = ac->asn1r_c_done_fun[ac->asn1r_msg_level];
  ac->asn1r_c_done_fun[ac->asn1r_msg_level] = asn1r_chk_getcstr_done;
  }

/************************************************************************/
/*			_dec_bitstr_cstr 				*/
/*	Function to decode asn.1 constructor bitstrings.		*/
/* NOTE: last segment may be any length but other segments must be	*/
/*   multiple of 8 bits. We don't know which segment is last until the	*/
/*   decode completes. We assume any segment that is NOT a multiple of 	*/
/*   8 bits must be the last, so if any segment is received after that,	*/
/*   it is a protocol error.						*/
/************************************************************************/

static ST_VOID _dec_bitstr_cstr (ASN1_DEC_CTXT *ac, ST_UINT16 id_code)
  {
ST_INT numbytes;
ST_INT numbits;
ST_UCHAR unused_bits;	/* # of unused bits in last byte	*/

  ALOG_DEC0 ("_dec_bitstr_cstr");

  if ((id_code & BITS_CODE) != BITS_CODE)
    {				/*  Protocol error - invalid ID Code.  */
    ALOG_NERR0 ("ASN.1 decode: unexpected tag");
    asn1r_set_dec_err(ac, ASN1E_UNEXPECTED_TAG);
    return;
    }

  if (ac->asn1r_constr_elmnt)		/* Don't do anything for constructors   */
    {
    ac->asn1r_c_done_fun[ac->asn1r_msg_level] = NULL;
    return;
    }

  /* Handle primitive bitstrings   */

  if (ac->asn1r_bitstr_truncated)
    {
    /* Already truncated when earlier segment decoded. Just skip over this data.*/
    ac->asn1r_field_ptr += ac->asn1r_elmnt_len;
    return;
    }

  /* If bitcount before adding this segment is not multiple of 8, the	*/
  /* previous segment length was illegal.				*/
  if (ac->asn1r_bitcount % 8)
    {	     /*  Protocol error */
    ALOG_NERR0 ("ASN.1 decode: constructed bitstring previous segment length not multiple of 8");
    asn1r_set_dec_err(ac, ASN1E_INVALID_BITSTR);
    return;
    }

  /* check length. Handle most common case first.	*/
  if (ac->asn1r_elmnt_len > 1)
    {
    unused_bits = *ac->asn1r_field_ptr;	    /* read the unused-bits byte. */
    if (unused_bits > 7)
      {	     /*  Protocol error */
      ALOG_NERR0 ("ASN.1 decode: constructed bitstring unused bits > 7");
      asn1r_set_dec_err(ac, ASN1E_INVALID_BITSTR);
      return;
      }
   
    /* # of bytes of data does not include the "unused bits" byte.	*/
    numbytes = (ST_INT16) (ac->asn1r_elmnt_len - 1);
    numbits = numbytes*8 - unused_bits;
    if (ac->asn1r_bitcount + numbits > ac->asn1r_max_bits)
      {
      /* Truncate. Recalculate numbits, numbytes.	*/
      numbits = ac->asn1r_max_bits - ac->asn1r_bitcount;
      numbytes = numbits / 8;
      if (numbits % 8)
        numbytes++;
      ac->asn1r_bitstr_truncated = SD_TRUE;	/* set "truncated" flag	*/
      }
    /* Copy bitstring data (at ac->asn1r_field_ptr + 1) to user buffer	*/
    if (numbits)
      bstrcpy (ac->_ad_bitptr, ac->asn1r_field_ptr + 1, numbits);
    ac->_ad_bitptr += numbytes;		/* adjust pointer	*/
    ac->asn1r_bitcount += numbits;	/* adjust bit count	*/
    }
  else if (ac->asn1r_elmnt_len == 1)	/* ONLY unused-bits byte. Must be 0.*/
    {
    unused_bits = *ac->asn1r_field_ptr;	    /* read the unused-bits byte. */
    if (unused_bits != 0)
      {	     /*  Protocol error */
      ALOG_NERR0 ("ASN.1 decode: bitstring no data. unused bits != 0");
      asn1r_set_dec_err(ac, ASN1E_INVALID_BITSTR);
      return;
      }
    }
  else	/* (ac->asn1r_elmnt_len < 1)	*/
    {	     /*  Protocol error */
    ALOG_NERR1 ("ASN.1 decode: bitstring element length = %d illegal",
        ac->asn1r_elmnt_len);
    asn1r_set_dec_err(ac, ASN1E_INVALID_BITSTR);
    return;
    }
  /* CRITICAL: update field_ptr only here.	*/
  ac->asn1r_field_ptr += ac->asn1r_elmnt_len;
  return;
  }

