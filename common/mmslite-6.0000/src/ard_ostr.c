/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2006, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : ard_ostr.c						*/
/* PRODUCT(S)  : ASN1DE 						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 07/20/09  JRB     10    Enable constructed octetstring code ALWAYS.	*/
/* 07/02/07  JRB     09    Chg ..fixlen, ..varlen arg to (ST_UCHAR *).	*/
/* 07/10/06  EJV     08    Add casting to elim warnings on Sun.		*/
/* 12/05/05  JRB     07    Add asn1r_get_octstr_fixlen,asn1r_get_octstr_varlen*/
/* 03/17/04  RKR     06    Changed thisFileName                         */
/* 03/11/04  GLB     05    Remove "thisFileName"                    	*/
/* 03/05/02  JRB     04    Eliminate warnings.				*/
/* 12/20/01  JRB     03    Chg ASN1_CTXT to ASN1_DEC_CTXT.		*/
/* 09/13/99  MDE     02    Added SD_CONST modifiers			*/
/* 07/26/99  MDE     01    New module, derived from ard_ostr.c		*/
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

static ST_VOID  _dec_octstr_cstr (ASN1_DEC_CTXT *ac, ST_UINT16 id_code);

/************************************************************************/
/*			asn1r_get_octstr_fixlen				*/
/* Just like asn1r_get_octstr but checks the length for a		*/
/* "fixed length" octet string (len must exactly match expected len).	*/
/************************************************************************/
ST_RET asn1r_get_octstr_fixlen (ASN1_DEC_CTXT *aCtx, ST_UCHAR *ptr, ST_INT len)
  {
ST_RET retcode;
  if (aCtx->asn1r_elmnt_len != len)
    {
    ALOG_ERR2("ASN.1 decode: octet string len = %d does not match expected len = %d",
        aCtx->asn1r_elmnt_len, len);
    retcode = SD_FAILURE;
    }
  else
    retcode = asn1r_get_octstr (aCtx, (ST_UCHAR *) ptr);
  return (retcode);
  }

/************************************************************************/
/*			asn1r_get_octstr_varlen				*/
/* Just like asn1r_get_octstr but checks the length for a		*/
/* "variable length" octet string (len must be "<=" max len).		*/
/************************************************************************/
ST_RET asn1r_get_octstr_varlen (ASN1_DEC_CTXT *aCtx, ST_UCHAR *ptr, ST_INT maxlen, ST_INT *lenout)
  {
ST_RET retcode;
  if (aCtx->asn1r_elmnt_len > maxlen)
    {
    ALOG_ERR2("ASN.1 decode: octet string len = %d exceeds max len = %d",
        aCtx->asn1r_elmnt_len, maxlen);
    retcode = SD_FAILURE;
    }
  else
    {
    *lenout = aCtx->asn1r_elmnt_len;
    retcode = asn1r_get_octstr (aCtx, (ST_UCHAR *) ptr);
    }
  return (retcode);
  }

/************************************************************************/
/*			    get_octstr					*/
/*  Function to read a primitive octet string from an ASN.1 message.	*/
/************************************************************************/

ST_RET asn1r_get_octstr (ASN1_DEC_CTXT *ac, ST_UCHAR *octptr)
  {
ST_INT i;

#ifdef	DEBUG_ASN1_DECODE
  if (!octptr)
    {
    slogCallStack (sLogCtrl,
                    "get_octstr: attempt to reference through a NULL pointer");
    return(SD_FAILURE);
    }
#endif


  if (ac->asn1r_elmnt_len)      /* Treat normal case where there are data octets. */
    {
    for (i = ac->asn1r_elmnt_len; i; --i)    /* Move octets to user storage area. */
      *(octptr++) = *(ac->asn1r_field_ptr++);
    }
  ac->asn1r_octetcount = ac->asn1r_elmnt_len;	   /* Compute number of data octets.	*/
  return (SD_SUCCESS);
  }

/************************************************************************/
/*			get_octstr_cstr 				*/
/*  Function to setup to get a constructed octet string.		*/
/************************************************************************/

ST_VOID asn1r_get_octstr_cstr (ASN1_DEC_CTXT *ac, ST_INT numoctets, ST_UCHAR *octptr)
 {
#ifdef	DEBUG_ASN1_DECODE
  if (!octptr)
    {
    slogCallStack (sLogCtrl,
                    "get_octstr_cstr: attempt to reference through a NULL pointer");
    return;
    }
#endif


  ac->_asn1_maxoctets = numoctets;	/* max allowed # octets 		*/
  ac->_asn1_octetptr = octptr;		/* pointer to user's storage area       */
  ac->asn1r_octetcount = 0;		/* running octet count			*/

  ac->asn1r_u_id_fun = _dec_octstr_cstr;		/* universal until octstr done	*/
  ac->asn1r_c_id_fun = asn1r_class_err;		/* no more context		*/
  ac->asn1r_a_id_fun = asn1r_class_err;		/* or application		*/
  ac->asn1r_p_id_fun = asn1r_class_err;		/* or private			*/

  ac->asn1r_save_method = ac->asn1r_decode_method;
  ac->asn1r_decode_method = ASN1_CLASS_METHOD;
  ac->_asn1r_fun_save = ac->asn1r_decode_done_fun;		/* save the previous done fun	*/
  ac->asn1r_decode_done_fun = asn1r_done_err;	/* can't be done for now        */
  ac->_asn1r_cstr_done_save = ac->asn1r_c_done_fun[ac->asn1r_msg_level];
  ac->asn1r_c_done_fun[ac->asn1r_msg_level] = asn1r_chk_getcstr_done;
  }

/************************************************************************/
/*			_dec_octstr_cstr 				*/
/*	Function to decode asn.1 constructor octet strings		*/
/************************************************************************/

static ST_VOID _dec_octstr_cstr (ASN1_DEC_CTXT *ac, ST_UINT16 id_code)
 {
ST_INT i;

  ALOG_DEC0 ("_dec_octstr_cstr");

  if ((id_code & OCT_CODE) != OCT_CODE)
    {				 /*  Protocol error - invalid ID Code.	*/
    ALOG_NERR0 ("ASN.1 decode: constructed octetstring, invalid tag");
    asn1r_set_dec_err(ac, ASN1E_UNEXPECTED_TAG);
    return;
    }

  if (ac->asn1r_constr_elmnt)		 /* Don't do anything for constructors. */
    {
    ac->asn1r_c_done_fun[ac->asn1r_msg_level] = NULL;
    return;
    }

  ac->asn1r_octetcount += ac->asn1r_elmnt_len;
  if (ac->asn1r_octetcount > ac->_asn1_maxoctets)
    {			  /* User error - overflowed user storage area. */
    ALOG_NERR0 ("ASN.1 decode: constructed octetstring too long");
    asn1r_set_dec_err(ac, ASN1E_OCTSTR_TOO_BIG);
    return;
    }

  if (ac->asn1r_elmnt_len)      /* Treat normal case where there are data octets. */
    {
    for (i = ac->asn1r_elmnt_len; i; --i)    /* Move octets to user storage area. */
      *(ac->_asn1_octetptr++) = *(ac->asn1r_field_ptr++);
    }
  return;
  }

