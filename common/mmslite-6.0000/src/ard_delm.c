/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : ard_delmn.c						*/
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
/* 12/20/01  JRB     03    Chg ASN1_CTXT to ASN1_DEC_CTXT.		*/
/* 09/13/99  MDE     02    Added SD_CONST modifiers			*/
/* 07/26/99  MDE     01    New module, derived from ad_delmn.c		*/
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
/* Variable used in skipping over entire data elements. 		*/


static ST_VOID  _get_delmnt_all (ASN1_DEC_CTXT *ac, ST_UINT16 id);
static ST_VOID  _get_delmnt_done (ASN1_DEC_CTXT *ac);
static ST_VOID  _get_delmnt_start (ASN1_DEC_CTXT *ac, ST_UINT16 id);
static ST_VOID  _parse_next_done (ASN1_DEC_CTXT *ac);
static ST_VOID  _parse_next_all (ASN1_DEC_CTXT *ac, ST_UINT16 id);
static ST_VOID  _parse_next_start (ASN1_DEC_CTXT *ac, ST_UINT16 id);
static ST_VOID  _parse_cstr_start(ASN1_DEC_CTXT *ac);

/************************************************************************/
/************************************************************************/
/*			get_delmnt submodule				*/
/* The code below makes up the 'get_delmnt' and 'parse_next' functions, */
/* which are used to simply ignore (possibly for later parsing) whatever*/
/* data element is next.  A user function is called at the end of the	*/
/* data element to continue the parse as necessary.  Both functions are */
/* useful for data elements that require another 'layer' of software to */
/* parse the data element.  The difference is that get_delmnt moves	*/
/* the data to a user-designated buffer, whereas parse_next leaves it	*/
/* in place.								*/
/************************************************************************/

/************************************************************************/
/*			    get_delmnt					*/
/* This function is used to parse the next ASN.1 data element, regard-	*/
/* less of type, then call the selected done function.	Inputs to	*/
/* get_delmnt are the pointer to the user buffer where the data element */
/* is to be put (as unparsed ASN.1-encoded data), the maximum length of */
/* the buffer, and the user done function to be called when the end of	*/
/* the data element is encountered.  The actual length of the data ele- */
/* ment is available to the user's done function through the global     */
/* variable asn1_octetcount.  If there was not enough room in the user	*/
/* buffer, a protocol error (OCTCSTR_TOO_BIG) will result.		*/
/************************************************************************/

ST_VOID asn1r_get_delmnt (ASN1_DEC_CTXT *ac, ST_INT buflen, ST_UCHAR *bufptr, 
                          ST_VOID (*done_fun)(ASN1_DEC_CTXT *ac))
  {
  ac->_asn1_maxoctets = buflen;			/* save buflen in global var.	*/
  ac->_asn1_octetptr = bufptr;			/* initialize _asn1_octetptr		*/
  ac->asn1r_octetcount = 0;			/* initialize asn1_octetcount	*/
  ac->_parse_begin_ptr = ac->asn1r_field_ptr;	/* save ptr to start of element */

  ac->_ad_parse_asn1r_fun_save = done_fun;		/* save the selected done fun	*/
  ac->_ad_parse_method_save = ac->asn1r_decode_method;	/* save method; restore later	*/
  ac->asn1r_decode_method = ASN1_CLASS_METHOD;

  ac->asn1r_c_id_fun = _get_delmnt_start;		/* select CTX function		*/
  ac->asn1r_u_id_fun = _get_delmnt_start;		/* select UNI function		*/
  ac->asn1r_p_id_fun = _get_delmnt_start;		/* select PRV function		*/
  ac->asn1r_a_id_fun = _get_delmnt_start;		/* select APP function		*/
  }

/************************************************************************/
/*			_get_delmnt_start				*/
/* This is the first function executed for the get_delmnt operation.	*/
/************************************************************************/

static ST_VOID _get_delmnt_start (ASN1_DEC_CTXT *ac, ST_UINT16 id)
  {

  ALOG_DEC0 ("_get_delmnt_start");

  _get_delmnt_all (ac, id);		/* move current elmnt to user buffer	*/

  if (ac->asn1r_constr_elmnt)		/* if this is a constructor elment -	*/
    {				/* reset done fun for this msg level	*/
    ac->asn1r_c_done_fun[ac->asn1r_msg_level] = _get_delmnt_done;
    ac->asn1r_c_id_fun = _get_delmnt_all;		/* select CTX function		*/
    ac->asn1r_u_id_fun = _get_delmnt_all;		/* select UNI function		*/
    ac->asn1r_p_id_fun = _get_delmnt_all;		/* select PRV function		*/
    ac->asn1r_a_id_fun = _get_delmnt_all;		/* select APP function		*/
    }
  else				/* if this is a primitive element -	*/
    _get_delmnt_done (ac); 	/* all done with the parse		*/
  }

/************************************************************************/
/*			_get_delmnt_all					*/
/* Moves the head of the current data element to the user buffer, and	*/
/* moves the contents also in the case of a primitive data element.	*/
/* Updates asn1_octetcount and leaves asn1_field_ptr pointing to the next elmnt.	*/
/************************************************************************/

static ST_VOID _get_delmnt_all (ASN1_DEC_CTXT *ac, ST_UINT16 id)
  {
ST_INT head_len;
ST_INT i;

  ALOG_DEC0 ("_get_delmnt_all");

  head_len = (int) (ac->asn1r_field_ptr - ac->asn1r_field_start);/* check for room      	*/
  ac->asn1r_octetcount += head_len;		/* in the user buffer for	*/
  if (ac->asn1r_octetcount > ac->_asn1_maxoctets)		/* the head of this data elmnt	*/
    {
    ALOG_NERR0 ("ASN.1 decode: constructed octetstring too long");
    asn1r_set_dec_err (ac, ASN1E_OCTSTR_TOO_BIG);
    return;
    }
  ac->asn1r_field_ptr = ac->asn1r_field_start;
  for (i=0; i<head_len; i++)		/* move the head of this elmnt	*/
    *(ac->_asn1_octetptr++) = *(ac->asn1r_field_ptr++);	/* to the user buffer		*/

  if (ac->asn1r_constr_elmnt)		/* if a constructor elment -	*/
    ac->asn1r_c_done_fun[ac->asn1r_msg_level] = NULL;
  else					/* if a primitive data element	*/
    {
    ac->asn1r_octetcount += ac->asn1r_elmnt_len;		/* check that the user buffer	*/
    if (ac->asn1r_octetcount > ac->_asn1_maxoctets) 	/* is big enough for the con-	*/
      { 				/* tents of this primitive elmnt*/
      ALOG_NERR0 ("ASN.1 decode: constructed octetstring too long");
      asn1r_set_dec_err (ac, ASN1E_OCTSTR_TOO_BIG);
      return;
      }
    for (i=0; i<ac->asn1r_elmnt_len; i++) 	/* move the primitive contents	*/
      *(ac->_asn1_octetptr++) = *(ac->asn1r_field_ptr++);	/* to the user buffer		*/
    }
  }

/************************************************************************/
/*			_get_delmnt_done 				*/
/* This function is called when the data element to be parsed is done.	*/
/* This is either via a constructor done call or from the start fun.	*/
/************************************************************************/

static ST_VOID _get_delmnt_done (ASN1_DEC_CTXT *ac)
  {
  ac->asn1r_decode_method = ac->_ad_parse_method_save;	/* restore method		*/
  (*ac->_ad_parse_asn1r_fun_save)(ac);			/* invoke the user function	*/
  }


/************************************************************************/
/*			    parse_cstr_contents				*/
/************************************************************************/

ST_VOID asn1r_parse_cstr_contents (ASN1_DEC_CTXT *ac, ST_VOID (*done_fun)(ASN1_DEC_CTXT *ac))
  {
  ac->asn1r_c_done_fun[ac->asn1r_msg_level] = done_fun;
  if (ac->asn1r_elmnt_len)
    {
    ac->_contents_done = ac->asn1r_field_ptr + ac->asn1r_elmnt_len;
    _parse_cstr_start(ac);
    }
  }

/************************************************************************/
/*			    _parse_cstr_start				*/
/*  Function will indirectly call itself until the original asn1_c_done_fun	*/
/*  set by parse_cstr_contents expires.					*/
/************************************************************************/

static ST_VOID _parse_cstr_start (ASN1_DEC_CTXT *ac)
  {
  ALOG_DEC0 ("_parse_next_start");
  if (ac->asn1r_field_ptr < ac->_contents_done)
    asn1r_parse_next (ac, _parse_cstr_start);
  }

/************************************************************************/
/*			    parse_next					*/
/* This function is used to parse the next ASN.1 data element, regard-	*/
/* less of type, then call the selected done function.	In the user's   */
/* done function, the length of the data element is given in the global */
/* variable 'asn1_octetcount', and the pointer to the beginning of the data  */
/* element is 'parse_begin_ptr'.                                        */
/************************************************************************/

ST_VOID asn1r_parse_next (ASN1_DEC_CTXT *ac, ST_VOID (*done_fun)(ASN1_DEC_CTXT *ac))
  {

  ac->_ad_parse_asn1r_fun_save = done_fun;		/* save the selected done fun	*/
  ac->_ad_parse_method_save = ac->asn1r_decode_method;	/* save method			*/
  ac->asn1r_decode_method = ASN1_CLASS_METHOD;
  ac->_parse_begin_ptr = ac->asn1r_field_ptr;		/* save ptr to start of element */

  ac->asn1r_c_id_fun = _parse_next_start;		/* select CTX function		*/
  ac->asn1r_u_id_fun = _parse_next_start;		/* select UNI function		*/
  ac->asn1r_p_id_fun = _parse_next_start;		/* select PRV function		*/
  ac->asn1r_a_id_fun = _parse_next_start;		/* select APP function		*/
  }

/************************************************************************/
/*			_parse_next_start				*/
/* First function executed for the parse_next operation.		*/
/************************************************************************/

static ST_VOID _parse_next_start (ASN1_DEC_CTXT *ac, ST_UINT16 id)
  {
  ALOG_DEC0 ("_parse_next_start");

  if (ac->asn1r_constr_elmnt)		/* if this is a constructor elment -	*/
    {
    ac->asn1r_c_done_fun[ac->asn1r_msg_level] = _parse_next_done;
    ac->asn1r_c_id_fun = _parse_next_all;		/* select CTX function		*/
    ac->asn1r_u_id_fun = _parse_next_all;		/* select UNI function		*/
    ac->asn1r_p_id_fun = _parse_next_all;		/* select PRV function		*/
    ac->asn1r_a_id_fun = _parse_next_all;		/* select APP function		*/
    }
  else				/* primitive element			*/
    {
    ac->asn1r_field_ptr += ac->asn1r_elmnt_len;	/* skip the contents			*/
    _parse_next_done (ac); 	/* all done with the parse		*/
    }
  }

/************************************************************************/
/*			_parse_next_all					*/
/* This function is used as the only state function when parsing a	*/
/* constructor element.  It just lets the ASN.1 tools look it over.	*/
/************************************************************************/

static ST_VOID _parse_next_all (ASN1_DEC_CTXT *ac, ST_UINT16 id)
  {
  ALOG_DEC0 ("_parse_next_all");

  if (ac->asn1r_constr_elmnt)	/* if this is a constructor elment -	*/
    ac->asn1r_c_done_fun[ac->asn1r_msg_level] = NULL;
  else					/* primitive element		*/
    ac->asn1r_field_ptr += ac->asn1r_elmnt_len;	/* skip the contents		*/
  }

/************************************************************************/
/*			_parse_next_done 				*/
/* This function is called when the data element to be parsed is done.	*/
/* This is either via a constructor done call or from the start fun.	*/
/************************************************************************/

static ST_VOID _parse_next_done (ASN1_DEC_CTXT *ac)
  {
  ac->asn1r_octetcount = (ST_INT16) (ac->asn1r_field_ptr - ac->_parse_begin_ptr);
  ac->asn1r_decode_method = ac->_ad_parse_method_save;	/* restore method		*/
  (*ac->_ad_parse_asn1r_fun_save)(ac);			/* invoke the user function	*/
  }

