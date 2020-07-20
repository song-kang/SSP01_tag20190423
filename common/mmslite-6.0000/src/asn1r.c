/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2009, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : asn1r.c 						*/
/* PRODUCT(S)  : ASN1DE 						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the main ASN.1 decode and encode tools.	*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 11/23/11  KCR           Added casting for proper 64bit compile       */
/* 11/17/09  JRB     33    Add asn1r_chk_tag, asn1r_*_contents.		*/
/* 07/21/09  JRB     32    Cleanup new asn1r_get_tag.			*/
/* 05/29/09  JRB     31    Add asn1r_decode_init, asn1r_get_tag functs.	*/
/*			   For indefinite len constr, increase initial	*/
/*			   byte_count (couldn't handle big constr).	*/
/*			   Add asn1r_strt_constr_indef (FOR TESTING).	*/
/* 05/28/09  JRB     30	   On decode, don't log constr content(redundant)*/
/* 01/15/09  MDE     29    Added asn1_peek,made asn1r_head_decode static*/
/* 11/12/08  MDE     28    Added overreach checks to  asn1r_head_decode	*/
/* 11/12/08  MDE     27    Removed assert to improve robustness		*/
/* 10/03/08  JRB     28    Prevent crashes while decoding by improving	*/
/*			   len checks & doing it BEFORE logging.	*/
/* 10/04/07  MDE     27    Tweaked LOGCFG_VALUE_GROUP/LOGCFGX_VALUE_MAP	*/
/* 12/20/06  JRB     26    Add bstrcmp.					*/
/* 03/17/06  JRB     25    Add asn1r_anytag_fun, ASN1_ANYTAG_METHOD.	*/
/* 03/15/06  JRB     24    Add asn1r_skip_elmnt.			*/
/* 02/28/06  EJV     23    asn1_convert_...: added typecast from time_t */
/* 12/19/05  JRB     22    Chg cr to cr_asn1, del static to fix warning.*/
/* 05/23/05  EJV     21    Add asn1LogMaskMapCtrl for parsing logcfg.xml*/
/* 07/08/04  JRB     20    Use define TIME_T_1984_JAN_1 instead of mktime*/
/*			   Use define SECONDS_PER_DAY from asn1r.h	*/
/*			   Add asn1_convert_timet_to_btime6.		*/
/*			   Use 0x01000000 (2**24) in fraction computations*/
/* 01/14/03  JRB     19    Avoid redundant logging of encode overrun.	*/
/*			   fin_constr decrement msg_level on encode overrun*/
/* 11/05/03  EJV     18    Fixed logging.				*/
/* 10/13/03  JRB     17    asn1r_decode_asn1: Chg assert to err return.	*/
/* 04/28/03  JRB     16    Add assert to asn1r_decode_asn1.		*/
/* 04/02/03  JRB     15    On unexpected tag error, log the tag.	*/
/* 02/07/03  JRB     14    Add bstrcpy, bvstrcpy.			*/
/* 10/29/02  JRB     13    Use new asn1r_magic in ASN1_ENC_CTXT.	*/
/* 07/03/02  EJV     12    MMS_UTC_TIME: chg name usec to fraction.	*/
/* 03/05/02  JRB     11    Eliminate compiler warnings.			*/
/* 01/25/02  JRB     10    Move INT_MAX chk to #if, avoids unreachable	*/
/*			   code. Make SD_CONST match header on *logstr.	*/
/* 01/22/02  JRB     09    asn1r_strt_asn1_bld set new "asn1r_buf_end".	*/
/*			   Chg asn1r_end_of_buffer to asn1r_buf_start	*/
/*			   & asn1r_field_start to asn1r_field_end	*/
/*			   & _asn1_constr_start to asn1r_constr_end	*/
/*			   in ENC_CTX (start means start, end means end)*/
/* 01/07/02  EJV     08    Added asn1_ prefix to convert_... functions.	*/
/* 01/04/02  EJV     07    Add convert_btod_to_utc, convert_utc_to_btod	*/
/*			   Cleanup few function header comments.	*/
/*			   Updated cr[] to 2002.			*/
/* 12/20/01  JRB     06    Chg ASN1_CTXT to ASN1_DEC_CTXT, ASN1_ENC_CTXT*/
/*			   Delete #define USE_WITH_ASN1.		*/
/*			   Del asn1_old_err_fun & *_old_id_fun.		*/
/*			   asn1r_objidcmp chged to asn1_objidcmp (same	*/
/*			   as function in asn1.c).			*/
/*			   Del unused strt_asn1r, asn1r_end_asn_bld.	*/
/* 11/28/01  EJV     05    fin_prim: corrected len for logging.		*/
/* 11/12/01  EJV     04    asn1r_decode_asn1: removed static variables	*/
/* 09/13/99  MDE     03    Added SD_CONST modifiers			*/
/* 07/29/99  MDE     02    Added TABLE method				*/
/* 07/26/99  MDE     01    New module, derived from asn1.c		*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include <limits.h>
#include "asn1r.h"
#include "asn1log.h"
#include "mem_chk.h"

/************************************************************************/

const ST_CHAR cr_asn1[] =
 "(c) COPYRIGHT SYSTEMS INTEGRATION SPECIALISTS COMPANY INC., 1986 - 2005.  All Rights Reserved.";

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

#if !defined(NO_GLB_VAR_INIT)
ST_UINT asn1_debug_sel = ASN1_LOG_ERR;
#else
ST_UINT asn1_debug_sel;
#endif

#ifdef DEBUG_SISCO
SD_CONST ST_CHAR *SD_CONST _asn1_log_dec_logstr = "ASN1_LOG_DEC";
SD_CONST ST_CHAR *SD_CONST _asn1_log_enc_logstr = "ASN1_LOG_ENC";
SD_CONST ST_CHAR *SD_CONST _asn1_log_err_logstr = "ASN1_LOG_ERR";
SD_CONST ST_CHAR *SD_CONST _asn1_log_nerr_logstr = "ASN1_LOG_NERR";

LOGCFGX_VALUE_MAP asn1LogMaskMaps[] =
  {
    {"ASN1_LOG_ERR",	ASN1_LOG_ERR,	&asn1_debug_sel, _LOGCFG_DATATYPE_UINT_MASK, NULL, "Error"},
    {"ASN1_LOG_NERR",	ASN1_LOG_NERR,	&asn1_debug_sel, _LOGCFG_DATATYPE_UINT_MASK, NULL, "Notice"},
    {"ASN1_LOG_DEC",	ASN1_LOG_DEC,	&asn1_debug_sel, _LOGCFG_DATATYPE_UINT_MASK, NULL, "Decode"},
    {"ASN1_LOG_ENC",	ASN1_LOG_ENC,	&asn1_debug_sel, _LOGCFG_DATATYPE_UINT_MASK, NULL, "Encode"}
  };

LOGCFG_VALUE_GROUP asn1LogMaskMapCtrl =
  {
  {NULL,NULL},
  "Asn1LogMasks",      /* Parent Tag */
  sizeof(asn1LogMaskMaps)/sizeof(LOGCFGX_VALUE_MAP),
  asn1LogMaskMaps
  };
#endif /* DEBUG_SISCO */

/************************************************************************/
/* General variables and pointers used by the ASN1 system.		*/
/************************************************************************/
/************************************************************************/
/* VMS Float Format select						*/

/* VMS has different forms of float format; select the default here	*/
#if !defined(NO_GLB_VAR_INIT) && defined(VAXC)
ST_INT	asn1_vax_double_type	= A_D_FLOAT;
#endif
#if !defined(NO_GLB_VAR_INIT) && defined( __ALPHA )
ST_INT	asn1_vax_double_type	= A_G_FLOAT;
#endif
#if defined(NO_GLB_VAR_INIT) && (defined( VAXC ) || defined( __ALPHA ))
ST_INT	asn1_vax_double_type;
#endif

/************************************************************************/
/* Static functions in this module					*/

static ST_RET     _asn1r_head_decode (ASN1_DEC_CTXT *ac);
static ST_VOID    _do_decode_asn1 (ASN1_DEC_CTXT *ac, ST_UCHAR *ptr, ST_INT len);
static ST_BOOLEAN _indef_eoc (ASN1_DEC_CTXT *ac);
static ST_VOID    _check_level_done (ASN1_DEC_CTXT *ac);
static ST_VOID    _call_user (ASN1_DEC_CTXT *ac);
static ST_VOID    _wr_asn1_len (ASN1_ENC_CTXT *ac, ST_INT len);
static ST_VOID    _wr_ident (ASN1_ENC_CTXT *ac, ST_UINT16 id_code, ST_UINT16 tag);
#ifdef DEBUG_SISCO
static ST_VOID    _list_elmnt (ASN1_DEC_CTXT *ac);
#endif

/************************************************************************/
/************************************************************************/
/* ASN.1 MESSAGE DECODE							*/
/************************************************************************/
/*			    asn1r_decode_asn1_seq			*/
/* This function is used to decode a ASN.1 data element when the outer	*/
/* constructor has been stripped.					*/
/************************************************************************/

ST_VOID asn1r_decode_asn1_seq (ASN1_DEC_CTXT *ac, ST_UCHAR *ptr, ST_INT len)
  {
  ac->_asn1_indef_track[0] = SD_FALSE;	/* level 0 is always definite		*/
  ac->_asn1_indef_track[1] = SD_FALSE;	/* so is level 1 in this case		*/

  ac->_asn1_byte_count[0] = 0;		/* assumes definite			*/
  ac->_asn1_byte_count[1] = len;		/* contents of missing outer cstr	*/

  ac->asn1r_field_ptr = ptr;		/* point to first element 		*/
  ac->asn1r_msg_level = 1;		/* start at message level 1		*/
  ac->asn1r_c_done_fun[ac->asn1r_msg_level] = NULL; 
	      
  _do_decode_asn1 (ac, ptr,len);
  }

/************************************************************************/
/*			    asn1r_decode_asn1 				*/
/* Function to drive the decoding of an ASN.1 encoded message.		*/
/* The user must set up the class or tag function ptrs, and the error	*/
/* and done function ptrs before calling this function. Pass the length */
/* and location of the message as inputs.				*/
/************************************************************************/

ST_VOID asn1r_decode_asn1 (ASN1_DEC_CTXT *ac, ST_UCHAR *ptr, ST_INT len)
  {
#if !defined (MMS_LITE)
ST_UCHAR *save_asn1r_ptr = NULL;
#endif

/* The caller MUST clear ASN1_DEC_CTXT struct before calling this funct.*/
/* After clearing, caller may set a few params, so chk a param that	*/
/* caller would NEVER set.						*/
  if (ac->asn1r_msg_level != 0)
    {
    ALOG_ERR0 ("ASN.1 decode context not initialized");
    ac->asn1r_pdu_dec_err = ASN1E_DECODE_OTHER;	/* set error code	*/
    return;
    }

/* initialize flags, counters, pointers for decode			*/

  ac->asn1r_indef_flag = SD_FALSE;	/* clear flag				*/
  ac->_asn1_indef_track[0] = SD_FALSE;	/* level 0 is always definite		*/
  ac->_asn1_byte_count[0] = len;	/* set # bytes left in level 0		*/
  ac->asn1r_msg_level = 0;		/* start at message level 0		*/
  ac->asn1r_c_done_fun[0] = NULL;

#if !defined (MMS_LITE)
  /* save the data in case we need to log it if decode fails		*/
  if ( (asn1_debug_sel & ASN1_LOG_NERR) &&
      !(asn1_debug_sel & ASN1_LOG_DEC))
    {
    save_asn1r_ptr = chk_malloc (len);
    memcpy (save_asn1r_ptr, ptr, len);
    }
#endif  

  _do_decode_asn1 (ac, ptr,len);

#if !defined (MMS_LITE)
  if (ac->asn1r_pdu_dec_err != NO_DECODE_ERR) /* if an error was present	*/
    {
    if (save_asn1r_ptr)
      {
      ALOG_NERR0 ("ASN.1 decode: badly formed PDU:");
      ALOG_NERRH (len, save_asn1r_ptr);
      }
    }
  if (save_asn1r_ptr)
    chk_free (save_asn1r_ptr);
#endif  
  }

/************************************************************************/
/*			_do_decode_asn1					*/
/************************************************************************/

static ST_VOID _do_decode_asn1 (ASN1_DEC_CTXT *ac, ST_UCHAR *ptr, ST_INT len)
  {
ST_INT  log_len;
ST_INT  gone_len;
ST_INT i;
ST_RET rc;

  ac->asn1r_pdu_dec_err = NO_DECODE_ERR;    /* reset error flag			*/
  ac->asn1r_decode_done = SD_FALSE;	    /* initialize flag			*/
  ac->asn1r_field_ptr = ptr;		    /* initialize asn1_field_ptr 	*/
  ac->asn1r_done_ptr = ac->asn1r_field_ptr + len;

/* Validate length to avoid possible problems */
  if (len <= 0)
    {
    ALOG_NERR0 ("ASN.1 decode: Length zero or negative");
    asn1r_set_dec_err(ac, ASN1E_END_O_BUFFER);
    if (ac->asn1r_err_fun != NULL)
      (*ac->asn1r_err_fun)(ac, ac->asn1r_pdu_dec_err); /* call error function	*/
    ac->_asn1r_ntag = 0;			/* set # tags = 0 for next time	*/
    return;
    }

  ALOG_DECLF ();
  ALOG_DEC1 ("ASN.1 MESSAGE TO DECODE : len = %d",len);
  ALOG_DECH (len,ptr);

/* Enter the message parse loop.					*/
/* Parse the message until all done or asn1_decode_done flag is set. 	*/
/* asn1_field_ptr points to the start of the data element to be decoded.	*/

  while (ac->asn1r_field_ptr < ac->asn1r_done_ptr && !ac->asn1r_decode_done)
    {
    ALOG_DEC1 ("Current decode offset: 0x%05x", ac->asn1r_field_ptr - ptr);
    ac->asn1r_field_start = ac->asn1r_field_ptr;	/* save field start address		*/
    rc = _asn1r_head_decode (ac); /* decodes the data element header	*/
    if (rc != SD_SUCCESS)
      {
      ALOG_NERR0 ("ASN.1 decode: header decode error");
      asn1r_set_dec_err(ac, rc);
      break;
      }

  /* OK, the header looks good and has been validated */
    log_len = (int) (ac->asn1r_field_ptr - ac->asn1r_field_start); 
    /* For primitive, log the whole element	*/
    if (!ac->asn1r_constr_elmnt)
      log_len += ac->asn1r_elmnt_len;

    ALOG_DECH (log_len, ac->asn1r_field_start);
    ALOG_PAUSEDEC ("");

#ifdef DEBUG_SISCO
    _list_elmnt (ac);		/* print element attributes		*/
#endif

    if (_indef_eoc (ac)) 	/* check for indefinite level EOC elmnt */
      { 			/* EOC is a primitive			*/
      _check_level_done (ac);	/* primitive complete			*/
      continue; 		/* service next field if not done	*/
      }
				/* not indefinite length EOC		*/
    if (ac->asn1r_decode_done)	/* if errors detected in decode 	*/
      break;

    if (ac->asn1r_constr_elmnt)		/* if constructor, bump asn1_msg_level	*/
      {
      if (++ac->asn1r_msg_level >= ASN1_MAX_LEVEL)
	{
        ALOG_ERR0 ("Error : ASN.1 decode exceeded nesting level");
	asn1r_set_dec_err(ac, ASN1E_NEST_TOO_DEEP);
	break;
	}
      ac->asn1r_c_done_fun[ac->asn1r_msg_level] = asn1r_cstr_done_err;  
      }

/* Call the user function specified for the given class or tag.  The	*/
/* user function is expected to parse the contents of this data element */
/* (when it's primitive) and move asn1_field_ptr to the start of the next    */
/* data element.							*/
    _call_user (ac);

/* The user function has been invoked, must set the asn1_decode_done flag	*/
/* if the message is complete or in error. The user function must move	*/
/* asn1_field_ptr to the start of the next data element.			*/

    gone_len = (int) (ac->asn1r_field_ptr - ac->asn1r_field_start);/* number of bytes gone */
						 /* adjust count 	 */

/* For primitive data elements, decrement _asn1_byte_count for current	*/
/* message level and jump directly down to _check_level_done at the end	*/
/* of the while loop.							*/
    if (!ac->asn1r_constr_elmnt)
      {
      if (ac->_asn1_byte_count[ac->asn1r_msg_level] < gone_len)
	{
        ALOG_NERR0 ("ASN.1 decode: invalid constructor length");
	asn1r_set_dec_err (ac, ASN1E_CSTR_INVALID_LEN);	/* call error function		*/
	break;				/* exit parse loop		*/
	}
      ac->_asn1_byte_count[ac->asn1r_msg_level] -= gone_len;
      }

/* Constructors are much more involved. First decrement the _asn1_byte_count	*/
/* for the previous message level (the one containing the new constr.), */
/* then set _asn1_byte_count for this message level.				*/
    else
      {
      if (ac->_asn1_byte_count[ac->asn1r_msg_level-1] < gone_len)
	{
        ALOG_NERR0 ("ASN.1 decode: invalid constructor length");
	asn1r_set_dec_err (ac, ASN1E_CSTR_INVALID_LEN);	/* call error function		*/
	break;				/* exit parse loop		*/
	}
      ac->_asn1_byte_count[ac->asn1r_msg_level-1] -= gone_len;

						/* save length type	*/
      if ((ac->_asn1_indef_track[ac->asn1r_msg_level] = ac->asn1r_indef_flag)
          != SD_FALSE)				/* if indefinite	*/
	{					/* (INDEF = 0xFF)	*/
	ac->_asn1_constr_start[ac->asn1r_msg_level] = ac->asn1r_field_ptr;	/* save start pointer	*/
	ac->_asn1_byte_count[ac->asn1r_msg_level] = INT_MAX;
	ac->asn1r_indef_flag  = SD_FALSE; 		/* clear flag		*/
	}
      else				/* definite length constructor	*/
	{				/* byte count in element known	*/
	ac->_asn1_byte_count[ac->asn1r_msg_level] = ac->asn1r_elmnt_len;
	if (ac->_asn1_byte_count[ac->asn1r_msg_level-1] < (ST_INT) ac->asn1r_elmnt_len)
	  {
          ALOG_NERR0 ("ASN.1 decode: invalid constructor length");
	  asn1r_set_dec_err (ac, ASN1E_CSTR_INVALID_LEN); /* call error function		*/
	  break;			/* exit parse loop		*/
	  }
	ac->_asn1_byte_count[ac->asn1r_msg_level-1] -= ac->asn1r_elmnt_len;
	}
      }

/* After decrementing _asn1_byte_count, check to see if this message level is */
/* done (and if so, check the next msg level, etc...).			*/
    _check_level_done (ac);
    }				/* Now return to top of parse loop.	*/

/* End of outer while loop.  Message parse is complete. 		*/
/* At this point the asn1_field_ptr should match the done_ptr, and the */
/* asn1_pdu_dec_err variable should be = NO_DECODE_ERR, and asn1_msg_level  == -1.*/
/* If any of these conditions are not true, the user error function	*/
/* will be executed (typically will return error message).  If no	*/
/* decode error, the user function pointed to by (*asn1_decode_done_fun)	*/
/* is executed. 							*/

  ALOG_CDEC0 ("Message parse complete");
  ALOG_PAUSEDEC ("");
					/* don't overwrite previous err */
  if (ac->asn1r_pdu_dec_err == NO_DECODE_ERR && 
      (ac->asn1r_field_ptr != ac->asn1r_done_ptr || ac->asn1r_msg_level != -1))
    {  					/* Check for indef cstr done	*/
    for (i = ac->asn1r_msg_level; i >= 0; --i)
      {
      if (ac->_asn1_indef_track[i] == SD_TRUE)	/* if indef length cstr		*/
        {				/* check for cstr done (00 00)	*/
	if ((ac->asn1r_field_ptr+2 > ac->asn1r_done_ptr) ||
	    *(ac->asn1r_field_ptr++) || *(ac->asn1r_field_ptr++))
          {
          ALOG_NERR0 ("ASN.1 decode: indefinite length constructor not terminated");
	  asn1r_set_dec_err (ac, ASN1E_END_O_BUFFER);   	/* call error function		*/
	  break;
	  }
	}
      }
    if (ac->asn1r_field_ptr != ac->asn1r_done_ptr)
      {
      ALOG_NERR0 ("ASN.1 decode: PDU decode terminated before complete");
      asn1r_set_dec_err (ac, ASN1E_END_O_BUFFER);       	/* call error function		*/
      }
    }

  if (ac->asn1r_pdu_dec_err != NO_DECODE_ERR) /* if an error was present	*/
    {
    if (ac->asn1r_err_fun != NULL)
      (*ac->asn1r_err_fun)(ac, ac->asn1r_pdu_dec_err); /* call error function		*/
    }
  else					 /* if not -			*/
    {
    if (ac->asn1r_decode_done_fun != NULL)
      (*ac->asn1r_decode_done_fun)(ac); /* invoke the done function	*/
    }

  ac->_asn1r_ntag = 0;			/* set # tags = 0 for next time	*/
  }


/************************************************************************/
/*			_check_level_done				*/
/* primitive field service complete, check to see if this level is	*/
/* complete, and if so, what the next level to service is.		*/
/************************************************************************/

static ST_VOID _check_level_done (ASN1_DEC_CTXT *ac)
  {
ST_INT i;

  while (ac->asn1r_msg_level >= 0 && !ac->_asn1_byte_count[ac->asn1r_msg_level])
    {		/* Search for first lower level with bytes left -	*/
    ALOG_CDEC1 ("Level %d parse complete",ac->asn1r_msg_level);
    if (!ac->asn1r_decode_done)	/* if user has not terminated		*/
      {
      ac->_asn1r_ntag = 0; 		/* clear tags, and call user		*/
      i = ac->asn1r_msg_level--;
      if (ac->asn1r_c_done_fun[i] != NULL)
        (*ac->asn1r_c_done_fun[i])(ac); /* done fcn for this asn1_msg_level	*/
      }
    else			/* decode terminated, just dec. 	*/
      ac->asn1r_msg_level--;
    }

  if (ac->asn1r_msg_level < 0)		/* if no bytes left at any lvl, */
    ac->asn1r_decode_done = SD_TRUE;		/* stop decode			*/
  }


/************************************************************************/
/*			_indef_eoc					*/
/* Function to check for universal EOC type, and perform level fixup if */
/* so. The fix consists of determining the number of bytes that were in */
/* the element and subtracting this number from the previous level byte */
/* count.								*/
/* Returns 0 if NOT EOC, else != 0					*/
/************************************************************************/

static ST_BOOLEAN _indef_eoc (ASN1_DEC_CTXT *ac)
  {
ST_INT len;

  if (ac->asn1r_constr_elmnt)		/* can only be a primitive	*/
    return (SD_FALSE);

  if (ac->asn1r_elmnt_class != UNI)		/* must be universal		*/
    return (SD_FALSE);

  if (ac->asn1r_elmnt_id != 0)		/* EOC is id_code 0		*/
    return (SD_FALSE);

/* This looks like the EOC from a indefinite length constructor 	*/
  if (ac->_asn1_indef_track[ac->asn1r_msg_level] == SD_TRUE) /* if indef len	*/
    {
    ALOG_CDEC1 ("EOC detected for level %d",ac->asn1r_msg_level);
    len = (int) (ac->asn1r_field_ptr - ac->_asn1_constr_start[ac->asn1r_msg_level]);
    if (ac->_asn1_byte_count[ac->asn1r_msg_level - 1] < len)
      {
      ALOG_NERR0 ("ASN.1 decode: misplaced end of indefinite length constructor");
      asn1r_set_dec_err(ac, ASN1E_CSTR_INVALID_LEN);	/* call error function		*/
      }
    ac->_asn1_byte_count[ac->asn1r_msg_level-1] -= len;
    ac->_asn1_byte_count[ac->asn1r_msg_level] = 0;		/* set this level count = 0	*/
    return (SD_TRUE); 			/* indicate EOC found		*/
    }
  else
    {
    ALOG_NERR0 ("ASN.1 decode: unexpected end of indefinite length constructor");
    asn1r_set_dec_err(ac, ASN1E_UNEXPECTED_TAG);		/* EOC not allowed here 	*/
    }
  return (SD_FALSE);
  }


/************************************************************************/
/*			asn1r_head_decode				*/
/* function to decode the ASN.1 data element identified and length	*/
/* components and put the result into the component variables.		*/
/* The character ptr asn1r_field_ptr points to the data element start,	*/
/* and is left pointing to the start of the contents.			*/
/************************************************************************/

#define _CHECK_ASN1_LEFT(x) {if((ac->asn1r_field_ptr+x) > ac->asn1r_done_ptr)return(ASN1E_END_O_BUFFER);}

static ST_RET _asn1r_head_decode (ASN1_DEC_CTXT *ac)
  {
ST_UCHAR c;
ST_UINT16 ui16;
ST_INT i, i2, i3;

/* We don't want to read past end of buffer */
  _CHECK_ASN1_LEFT(1);
  c = *(ac->asn1r_field_ptr++);	/* read the first byte of the element	*/
				/* pointer points to next byte		*/

  ac->asn1r_constr_elmnt = (ST_UCHAR) (c & CONSTR);/* isolate the constr bit	*/
  ac->asn1r_elmnt_class = (ST_UCHAR) (c & 0xC0);   /* get the class bits	*/

  if ((c &= 0x1F) == 0x1F)		/* if ID extender -		*/
    {
    _CHECK_ASN1_LEFT(1);
    if ((c = *(ac->asn1r_field_ptr++)) & 0x80) /* if next byte is not last -	*/
      {
      _CHECK_ASN1_LEFT(1);
      if (*ac->asn1r_field_ptr & 0x80)	/* if more than two extention	*/
	{				/* ID octets, reject		*/
        ALOG_NERR0 ("ASN.1 decode: element id too long");
	return (ASN1E_ID_TOO_BIG);
	}
      ui16 = (ST_UINT16) (c & 0x7F) << 7;  /* support 2 extention bytes */

      _CHECK_ASN1_LEFT(1);
      ui16 |= (*(ac->asn1r_field_ptr++) & 0x7F);/* merge the 14 ID bits (only*/
      ac->asn1r_elmnt_id = ui16;		   /* 13 usable in this impl'n).*/
      }
    else				/* one octet extended		*/
      ac->asn1r_elmnt_id = (ST_UINT16) c;	/* write masked ID code 	*/
    }
  else					/* not extended ID code 	*/
    ac->asn1r_elmnt_id = (ST_UINT16) c;	/* write masked ID code 	*/

/* asn1_field_ptr points to contents length field start.			*/
/* Support all length forms : short, long, and indefinite.		*/

  _CHECK_ASN1_LEFT(1);
  if ((c = *(ac->asn1r_field_ptr++)) & 0x80)	/* check for long,indef forms	*/
    {
    if (c &= 0x7F)			/* if long form 		*/
      {
      _CHECK_ASN1_LEFT((int) c);
      switch (c)			/* switch on # bytes in length	*/
	{
	case 1 :			/* one byte length		*/
	  ac->asn1r_elmnt_len = *(ac->asn1r_field_ptr++) & 0xFF;
	break;

	case 2 :			/* two byte length		*/
	  i =  (ST_INT) ((ST_UINT) *(ac->asn1r_field_ptr++) << 8);
	  ac->asn1r_elmnt_len = (ST_INT) 
	    (((ST_UINT) *(ac->asn1r_field_ptr++) & 0xFF) | i);
	break;

	case 3 :	 		/* three byte length		*/
	  i =  (ST_INT) ((ST_UINT) *(ac->asn1r_field_ptr++) << 16);
#if (INT_MAX <= 0x7FFF)
	  if (i != 0)
            {
            ALOG_NERR0 ("ASN.1 decode: element length too long");
            return(ASN1E_INVALID_LENGTH);	/* other lengths not supported	*/
            }
#endif
	  i2 =  (ST_INT) (((ST_UINT) *(ac->asn1r_field_ptr++) << 8) | i);

	  ac->asn1r_elmnt_len = (ST_INT) 
	    (((ST_UINT) *(ac->asn1r_field_ptr++) & 0xFF) | i2);
	break;

	case 4 :	 		/* four byte length		*/
	  i =  (ST_INT) ((ST_UINT) *(ac->asn1r_field_ptr++) << 24);
	  i2 =  (ST_INT) (((ST_UINT) *(ac->asn1r_field_ptr++) << 16) | i);
#if (INT_MAX <= 0x7FFF)
	  if (i != 0 || i2 != 0)
            {
            ALOG_NERR0 ("ASN.1 decode: element length too long");
            return (ASN1E_INVALID_LENGTH);	/* other lengths not supported	*/
            }
#endif
	  i3 =  (ST_INT) (((ST_UINT) *(ac->asn1r_field_ptr++) << 8) | i2);
	  ac->asn1r_elmnt_len = (ST_INT) 
	    (((ST_UINT) *(ac->asn1r_field_ptr++) & 0xFF) | i3);
	break;

	default :
           ALOG_NERR0 ("ASN.1 decode: length length > 4");
	   return (ASN1E_INVALID_LENGTH); /* others not supported	*/
	break;
	}
      }
    else				/* indefinite length		*/
      {
      if (ac->asn1r_constr_elmnt) 		/* valid only for constructors	*/
	ac->asn1r_indef_flag = SD_TRUE;	/* set 'indefinite len' flag    */
      else
        {
        ALOG_NERR0 ("ASN.1 decode: indefinite length primitive");
	return (ASN1E_INVALID_LENGTH);	/* Bad PDU structure		*/
        }
      }
    }
  else					/* short form length		*/
    ac->asn1r_elmnt_len = (ST_INT) c;  	/* write length 		*/

  /* If NOT indefinite length, make sure length is legal.	*/
  if (!ac->asn1r_indef_flag)
    {
    if ((ac->asn1r_elmnt_len < 0) || ((ac->asn1r_field_ptr+ac->asn1r_elmnt_len) > ac->asn1r_done_ptr))
      {
      ALOG_NERR2 ("ASN.1 decode element length error: ASN.1 length=%d, remaining ASN.1 buffer %d ",
                  ac->asn1r_elmnt_len, ac->asn1r_done_ptr - ac->asn1r_field_ptr);
      return (ASN1E_INVALID_LENGTH);
      }
    }

  return (SD_SUCCESS);
  }					/* data element decode complete */

/************************************************************************/
/*			_call_user					*/
/* Function to call the user function for the particular ID class.	*/
/* Passes the id code for the data element to the selected function	*/
/* For Universal class, also checks for EOC.				*/
/*									*/
/* The user functions are responsible for ensuring that the asn1_field_ptr	*/
/* is left pointing to the start of the next field.			*/
/************************************************************************/

static ST_VOID _call_user (ASN1_DEC_CTXT *ac)
 {
ST_UINT16 curtag;
ST_INT i;
ST_INT num_tags;
ASN1R_TAG_CTRL_1 *tag_ctrl;
ASN1R_TAG_PAIR *tags;

/* When parsing is being done on the basis of tag, compare the current	*/
/* tag with the list of allowable tags and call the corresponding fcn	*/
/* pointer. Error checking was done previously when tag_add was called. */

  if (ac->asn1r_decode_method == ASN1_TABLE_METHOD)
    {
    curtag =  (ST_UINT16) (ac->asn1r_elmnt_class | ac->asn1r_constr_elmnt) << 8;
    curtag |= ac->asn1r_elmnt_id;

    tag_ctrl = (ASN1R_TAG_CTRL_1 *) ac->asn1r_tag_table;
    tags = tag_ctrl->tags;
    num_tags = tag_ctrl->num_tags;
    for (i = 0; i < num_tags; ++i, ++tags)
     {
     if (curtag == tags->_asn1r_tag)
       {
       (*tags->_asn1r_tag_fun)(ac);		 /* Call user-defined function.	*/
       return;
       }
     }
    ALOG_NERR1 ("ASN.1 decode: unexpected tag 0x%X", (ST_UINT) curtag);
    asn1r_set_dec_err(ac, ASN1E_UNEXPECTED_TAG);
    return;
    }

  if (ac->asn1r_decode_method == ASN1_TAG_METHOD)
    {
    curtag =  (ST_UINT16) (ac->asn1r_elmnt_class | ac->asn1r_constr_elmnt) << 8;
    curtag |= ac->asn1r_elmnt_id;
    for (i = 0; i < ac->_asn1r_ntag; i++)
     {
      if (curtag == ac->_asn1r_valid_tags[i])
       {
       ac->_asn1r_old_ntag = ac->_asn1r_ntag;	 /* Tags are deleted once used,	*/
       ac->_asn1r_ntag = 0;			 /* but are recoverable. 	*/
       (*ac->_asn1r_tag_fun[i])(ac);		 /* Call user-defined function.	*/
       return;
       }
     }
    ALOG_NERR1 ("ASN.1 decode: unexpected tag 0x%X", (ST_UINT) curtag);
    asn1r_set_dec_err(ac, ASN1E_UNEXPECTED_TAG);
    return;
    }

  if (ac->asn1r_decode_method == ASN1_ANYTAG_METHOD)
    {
    curtag =  (ST_UINT16) (ac->asn1r_elmnt_class | ac->asn1r_constr_elmnt) << 8;
    curtag |= ac->asn1r_elmnt_id;
    (*ac->asn1r_anytag_fun)(ac, curtag);
    return;
    }
    
  ac->_asn1r_ntag = 0;				/* class method, no valid tags	*/

/* When parsing is being done on the basis of class, switch on the	*/
/* class of the current data element and call the corresponding fcn	*/
/* pointer. Do error checking where possible.				*/
  switch (ac->asn1r_elmnt_class)
    {
    case CTX : 			/* class = context specific	*/
      (*ac->asn1r_c_id_fun)(ac, ac->asn1r_elmnt_id);
    break;

    case UNI : 			/* class = universal		*/					/* Perform additional checks	*/
      switch (ac->asn1r_elmnt_id) 		/* for constr/prim allowed	*/
	{				/* check selected types 	*/
	case INT_CODE : 		/* integer must be primitive	*/
	   if (ac->asn1r_constr_elmnt)
             {
             ALOG_NERR0 ("ASN.1 decode: invalid INTEGER element");
	     asn1r_set_dec_err (ac, ASN1E_UNEXPECTED_FORM);
             }
	   else
	     {
             (*ac->asn1r_u_id_fun)(ac, ac->asn1r_elmnt_id);
	     }
	break;

	case SEQ_CODE : 		/* sequence valid in constr	*/
	   if (!ac->asn1r_constr_elmnt)
             {
             ALOG_NERR0 ("ASN.1 decode: invalid SEQ element");
	     asn1r_set_dec_err (ac, ASN1E_UNEXPECTED_FORM);
             }
	   else
	     {
             (*ac->asn1r_u_id_fun)(ac, ac->asn1r_elmnt_id);
	     }
	break;

	case SET_CODE : 		/* set valid in constructor	*/
	   if (!ac->asn1r_constr_elmnt)
             {
             ALOG_NERR0 ("ASN.1 decode: invalid SET element");
	     asn1r_set_dec_err (ac, ASN1E_UNEXPECTED_FORM);
             }
	   else
	     {
             (*ac->asn1r_u_id_fun)(ac, ac->asn1r_elmnt_id);
	     }
	break;

	case BOOL_CODE :		/* boolean must be primitive,	*/
	   if (ac->asn1r_constr_elmnt || ac->asn1r_elmnt_len != 1)	/* 1 octet		*/
             {
             ALOG_NERR0 ("ASN.1 decode: invalid BOOL element");
	     asn1r_set_dec_err (ac, ASN1E_INVALID_BOOLEAN);
             }
	   else
	     {
             (*ac->asn1r_u_id_fun)(ac, ac->asn1r_elmnt_id);
	     }
	break;

	case NULL_CODE :		/* null must be primitive	*/
	   if (ac->asn1r_constr_elmnt || ac->asn1r_elmnt_len)
             {
             ALOG_NERR0 ("ASN.1 decode: invalid NULL element");
	     asn1r_set_dec_err (ac, ASN1E_UNEXPECTED_TAG);
             }
	   else
	     {
             (*ac->asn1r_u_id_fun)(ac, ac->asn1r_elmnt_id);
	     }
	break;

	default :			/* all others can be either	*/
	  {
          (*ac->asn1r_u_id_fun)(ac, ac->asn1r_elmnt_id);
	  }
	break;
	}
    break;				/* end of universal		*/

    case APP : 				/* class = application wide	*/
      (*ac->asn1r_a_id_fun)(ac, ac->asn1r_elmnt_id);
    break;

    case PRV : 				/* class = private use		*/
      (*ac->asn1r_p_id_fun)(ac, ac->asn1r_elmnt_id);
    break;
    }
  }

/************************************************************************/
/*			asn1r_set_all_cstr_done				*/
/************************************************************************/

ST_VOID asn1r_set_all_cstr_done (ASN1_DEC_CTXT *ac)
 {
ST_INT i;

  for (i = 0; i < ASN1_MAX_LEVEL; ++i)
    ac->asn1r_c_done_fun[i] = NULL;
  }

/************************************************************************/
/*			asn1r_tag_add 					*/
/************************************************************************/

ST_VOID asn1r_tag_add (ASN1_DEC_CTXT *ac, ST_UINT16 tag, 
		       ST_VOID (*fcn_ptr)(ASN1_DEC_CTXT *ac))
  {
#ifdef DEBUG_SISCO
  if (ac->_asn1r_ntag >= MAX_TAG_FUN)  	/* Array is MAX_TAG_FUN deep */
    {
    ALOG_ERR0 ("ASN.1 Decode Internal Error : Tag Table Buffer Overflow");
    return;
    }
#endif

  ac->_asn1r_valid_tags[ac->_asn1r_ntag] = tag;
  ac->_asn1r_tag_fun[ac->_asn1r_ntag] = (ST_VOID (*)(ASN1_DEC_CTXT *)) fcn_ptr;
  ac->_asn1r_ntag++;
  return;
  }

/************************************************************************/
/*			asn1r_tag_restore				*/
/* This fun restores the el'ts of the tag[] and _asn1_tag_fun[] arrays. */
/************************************************************************/

ST_VOID asn1r_tag_restore (ASN1_DEC_CTXT *ac)
 {
 ac->_asn1r_ntag = ac->_asn1r_old_ntag;
 }

/************************************************************************/
/************************************************************************/
/*				asn1_peek 				*/
/************************************************************************/

ST_RET asn1_peek (ASN1_DEC_CTXT *ac, ST_UCHAR *ptr, ST_INT len)
  {
ST_RET rc;

  ac->asn1r_pdu_dec_err = NO_DECODE_ERR;    /* reset error flag			*/
  ac->asn1r_field_ptr = ptr;		    /* initialize asn1_field_ptr 	*/
  ac->asn1r_done_ptr = ac->asn1r_field_ptr + len;

/* Validate length to avoid possible problems */
  if (len <= 0)
    {
    ALOG_NERR0 ("ASN.1 peek: Length zero or negative");
    asn1r_set_dec_err(ac, ASN1E_END_O_BUFFER);
    return (SD_FAILURE);
    }

  rc = _asn1r_head_decode (ac); /* decodes the data element header	*/
  if (rc != SD_SUCCESS)
    {
    ALOG_NERR0 ("ASN.1 decode: header decode error");
    return (rc);
    }
  return (SD_SUCCESS);
  }

/************************************************************************/
/************************************************************************/
/* ASN.1 message construct tools.					*/
/************************************************************************/
/************************************************************************/
/*			asn1r_strt_asn1_bld				*/
/* Function to initialize the ASN.1 build parameters. Should be called	*/
/* before starting construction of each message.			*/
/************************************************************************/

ST_VOID asn1r_strt_asn1_bld (ASN1_ENC_CTXT *ac, ST_UCHAR *bufptr, ST_INT buflen)
 {
  ALOG_ENC0 ("Starting ASN.1 message construct");

/* Initialize variables in the encode context structure.		*/

  ac->asn1r_encode_overrun = SD_FALSE;
  ac->asn1r_buf_start = bufptr;

  /* Initialize a bunch of variables to the "end" of buffer.		*/
  ac->asn1r_constr_end[0] = ac->asn1r_field_ptr = ac->asn1r_field_end =
    ac->asn1r_buf_end = bufptr + buflen - 1;

  ac->asn1r_msg_level = 0;			     /* init current level	*/
/* Chk this later to verify that "asn1r_strt_asn1_bld" was called.	*/
  ac->asn1r_magic = ASN1_ENC_MAGIC_NUMBER;
  }

/************************************************************************/
/*			asn1r_strt_constr				*/
/* Function to start a constructor build (from back).			*/
/* Simply increments the message level (if notmax) sets the start	*/
/* pointer for the level at the current position			*/
/************************************************************************/

ST_VOID asn1r_strt_constr (ASN1_ENC_CTXT *ac)
  {
  if (++ac->asn1r_msg_level >= ASN1_MAX_LEVEL) /* increment the level			*/
    {
    ALOG_ERR0 ("Error : ASN.1 encode exceeded nesting level");
    --ac->asn1r_msg_level;		/* if at top level			*/
    }

  ALOG_CENC1 ("Starting constructor @ level %d",ac->asn1r_msg_level);
  ac->asn1r_constr_end[ac->asn1r_msg_level] = ac->asn1r_field_ptr; /* save msg ptr	*/
  }

/************************************************************************/
/*			asn1r_strt_constr_indef				*/
/* FOR TEST APPLICATIONS ONLY.						*/
/* SISCO applications never encode Indefinite Length constructors (slow)*/
/* but this function may be needed by test applications to generate	*/
/* ASN.1 messages with Indefinite Length constructors.			*/
/************************************************************************/

ST_VOID asn1r_strt_constr_indef (ASN1_ENC_CTXT *ac)
  {
  if (++ac->asn1r_msg_level >= ASN1_MAX_LEVEL)	/* increment the level	*/
    {
    ALOG_ERR0 ("Error : ASN.1 encode exceeded nesting level");
    --ac->asn1r_msg_level;		/* if at top level		*/
    }

  ALOG_CENC1 ("Starting 'indefinite length' constructor @ level %d",ac->asn1r_msg_level);
  *ac->asn1r_field_ptr-- = 0;	/* write 2 zeros for 'end of constructor'*/
  *ac->asn1r_field_ptr-- = 0;
  ac->asn1r_constr_end[ac->asn1r_msg_level] = ac->asn1r_field_ptr;
  /* CRITICAL: set ..field_end so length of last prim can be computed later.*/
  ac->asn1r_field_end = ac->asn1r_field_ptr;
  }

/************************************************************************/
/*			asn1r_fin_constr				*/
/* Function to finish a constructor.					*/
/* Writes the constructor length, then the identifier as requested.	*/
/*									*/
/* Pass the required id_code, the tag bits (upper 3 bits), and flag to	*/
/* indicate when the indefinite length is required (indef != 0) 	*/
/************************************************************************/

ST_VOID asn1r_fin_constr (ASN1_ENC_CTXT *ac, ST_UINT16 id_code, ST_UINT16 el_tag, ST_BOOLEAN indef)
  {
ST_INT len;

  if (ac->asn1r_field_ptr - ASN1_MAX_ELEMENT_OVERHEAD < ac->asn1r_buf_start)
    {
    /* Do NOT encode anything, but decrement msg_level to avoid nesting	*/
    /* errors if user continues trying to encode.			*/
    --ac->asn1r_msg_level;
    ac->asn1r_encode_overrun = SD_TRUE;	/* May already be SD_TRUE.	*/
    return;
    }


  if (!indef)			/* write the length field (moves ptr)	*/
    {
    len = (int) (ac->asn1r_constr_end[ac->asn1r_msg_level] - ac->asn1r_field_ptr);
    _wr_asn1_len (ac, len);
    }
  else			  	/* if not indefinite length -		*/
    *(ac->asn1r_field_ptr--) = 0x80;

				/* write identifier (moves ptr)		*/
  _wr_ident (ac, id_code, (ST_UINT16) (el_tag | CONSTR)); 

#ifdef DEBUG_SISCO
  ALOG_CENC1 ("Finish level %d constructor",ac->asn1r_msg_level);
  if (indef)
    {
    ALOG_CENC0 ("Indefinite length");
    }
  else
    {
    ALOG_CENC1 ("Length = %d",ac->asn1r_constr_end[ac->asn1r_msg_level] - ac->asn1r_field_ptr);
    }
  ALOG_ENCH ((int) (ac->asn1r_constr_end[ac->asn1r_msg_level] - ac->asn1r_field_ptr), ac->asn1r_field_ptr + 1);
#endif

  if (--ac->asn1r_msg_level < 0)	/* if was level 0 already		*/
    ac->asn1r_msg_level = 0;		/* set = 0				*/

  ac->asn1r_field_end = ac->asn1r_field_ptr;	/* reset the field end pointer*/
  }

/************************************************************************/
/*			asn1r_fin_prim					*/
/* Function to finish writing a primitive data element. 		*/
/* Pass the constructor type (set,sequence) and data element type.	*/
/************************************************************************/

ST_VOID asn1r_fin_prim (ASN1_ENC_CTXT *ac, ST_UINT16 id_code, ST_UINT16 el_tag)
  {
ST_INT len;

  if (ac->asn1r_field_ptr - ASN1_MAX_ELEMENT_OVERHEAD < ac->asn1r_buf_start)
    {
    ac->asn1r_encode_overrun = SD_TRUE;	/* May already be SD_TRUE.	*/
    return;
    }

  len = (int) (ac->asn1r_field_end - ac->asn1r_field_ptr);

				/* write the length field (moves ptr)	*/
  _wr_asn1_len (ac, len);
  _wr_ident (ac, id_code, el_tag);	/* write the identifier (moves ptr)	*/

  len = (int) (ac->asn1r_field_end - ac->asn1r_field_ptr);

  ALOG_CENC0 ("Finish primitive");
  ALOG_ENCH (len, ac->asn1r_field_ptr + 1);

  ac->asn1r_field_end = ac->asn1r_field_ptr;	/* reset the field end pointer*/
  }

/************************************************************************/
/*			_wr_ident					*/
/* Function to write an ASN.1 identifier field. 			*/
/* Pass the id code (0 to 0x3FFF), and the tag (class and constr bits)	*/
/************************************************************************/

static ST_VOID _wr_ident (ASN1_ENC_CTXT *ac, ST_UINT16 id_code, ST_UINT16 el_tag)
  {
  if (id_code < 0x1F)		/* if no extension octets required	*/
    *(ac->asn1r_field_ptr--) = (ST_UCHAR)(id_code | el_tag);	/* one byte identifier	*/
  else
    {
    if (id_code < 0x80) 	/* if just one extension byte - 	*/
      *(ac->asn1r_field_ptr--) = (ST_UCHAR)id_code;		/* write id_code	*/
    else
      { 			/* need two extension bytes		*/
      *(ac->asn1r_field_ptr--) = (ST_UCHAR)(id_code & 0x7F); /* write low 7 bits	*/
      *(ac->asn1r_field_ptr--) = (ST_UCHAR)(((id_code >> 7) & 0x7F) | 0x80);
      }
    *(ac->asn1r_field_ptr--) = (ST_UCHAR)(el_tag | 0x1F); /* indicte extnd'r	*/
    }
  }

/************************************************************************/
/*			_wr_asn1_len					*/
/* Function to write an ASN.1 length field				*/
/* Lengths from 0 to INT_MAX are supported.				*/
/************************************************************************/

static ST_VOID _wr_asn1_len (ASN1_ENC_CTXT *ac, ST_INT len)
  {
  if (len <= 0x7F)			/* see if short form will do	*/
    *(ac->asn1r_field_ptr--) = (ST_UCHAR)len;
  else
    {					/* need long form		*/
    if (len <= 0xFF)			/* if one byte will do		*/
      {
      *(ac->asn1r_field_ptr--) = (ST_UCHAR)len;   /* write length 		*/
      *(ac->asn1r_field_ptr--) = (ST_UCHAR) 0x81; /* write # bytes in len    */
      }
    else if (len <= 0xFFFF)
      {
      *(ac->asn1r_field_ptr--) = (ST_UCHAR)(len & 0xFF);    	   /* low byte	*/
      *(ac->asn1r_field_ptr--) = (ST_UCHAR)((len >> 8) & 0xFF); /* high byte	*/
      *(ac->asn1r_field_ptr--) = (ST_UCHAR) 0x82;               /* #len bytes*/
      }
    else if (len <= (ST_INT) 0xFFFFFF)
      {
      *(ac->asn1r_field_ptr--) = (ST_UCHAR)(len & 0xFF);    	   /* low byte	*/
      *(ac->asn1r_field_ptr--) = (ST_UCHAR)((len >> 8) & 0xFF); /* mid byte	*/
      *(ac->asn1r_field_ptr--) = (ST_UCHAR)((len >> 16) & 0xFF); /* high byte	*/
      *(ac->asn1r_field_ptr--) = (ST_UCHAR) 0x83;               /* #len bytes*/
      }
    else
      {
      *(ac->asn1r_field_ptr--) = (ST_UCHAR)(len & 0xFF);    	   /* low byte	*/
      *(ac->asn1r_field_ptr--) = (ST_UCHAR)((len >> 8) & 0xFF); /* mid log byte	*/
      *(ac->asn1r_field_ptr--) = (ST_UCHAR)((len >> 16) & 0xFF); /* mid high byte	*/
      *(ac->asn1r_field_ptr--) = (ST_UCHAR)((len >> 24) & 0xFF); /* high byte	*/
      *(ac->asn1r_field_ptr--) = (ST_UCHAR) 0x84;               /* #len bytes*/
      }
    }
  }				      



/************************************************************************/
/*			asn1r_set_dec_err					*/
/* Function called from anywhere when an error is detected. Sets the	*/
/* passed error code = asn1_pdu_dec_err, and sets asn1_decode_done.		*/
/************************************************************************/

ST_VOID asn1r_set_dec_err (ASN1_DEC_CTXT *ac, ST_RET err_code)
  {
  ALOG_NERR1 ("ASN.1 decode error : 0x%X", err_code);
  ac->asn1r_pdu_dec_err = err_code;	/* set the problem type 		*/
  ac->asn1r_decode_done = SD_TRUE;	/* quit parse				*/
  }

/************************************************************************/
/*			asn1r_done_err					*/
/* function to eliminate premature decode complete, allow release of	*/
/* allocated resources by invoking the error function.			*/
/* Use as a asn1_decode_done_fun.					*/
/************************************************************************/

ST_VOID asn1r_done_err (ASN1_DEC_CTXT *ac)
  {
  ALOG_NERR0 ("ASN1 decode: PDU ended before valid");
  asn1r_set_dec_err (ac, ASN1E_END_OF_MESSAGE); /* set the error			*/
  if (ac->asn1r_err_fun != NULL)
    (*ac->asn1r_err_fun)(ac, ASN1E_END_OF_MESSAGE); /* call the error function	*/
  }


/************************************************************************/
/*			asn1r_class_err					*/
/* This function is a nominal function to be invoked when a undesired	*/
/* class data element is found during the message parse.		*/
/* Just set an error code, stop parse.					*/
/************************************************************************/

ST_VOID asn1r_class_err (ASN1_DEC_CTXT *ac, ST_UINT16 id_code)
  {
  ALOG_NERR0 ("CLASS ERROR");
  asn1r_set_dec_err (ac, ASN1E_UNEXPECTED_CLASS); /* just set the error flag	*/
  }

/************************************************************************/
/*			asn1_cstr_done_err					*/
/* Function to prevent premature constructor completion.		*/
/************************************************************************/

ST_VOID asn1r_cstr_done_err (ASN1_DEC_CTXT *ac)
  {
  ALOG_NERR0 ("CONSTRUCTOR DONE ERROR");
  asn1r_set_dec_err(ac, ASN1E_END_OF_CSTR);
  }

/************************************************************************/
/*			asn1r_chk_getcstr_done				*/
/* General constructor done function to be used with various pieces	*/
/* of code to get information from a constructor inside a message.	*/
/* To use, the variables _asn1_save_method and _asn1r_fun_save MUST be	*/
/* setup and the ASN.1 function pointer cstr_done_fun must point	*/
/* to this function.							*/
/************************************************************************/

ST_VOID asn1r_chk_getcstr_done (ASN1_DEC_CTXT *ac)
  {
  ac->asn1r_decode_method = ac->asn1r_save_method; /* restore the decode method	*/
  ac->asn1r_decode_done_fun = ac->_asn1r_fun_save;  /* restore the decode done fun*/
  (*ac->_asn1r_cstr_done_save)(ac);		  /* call the user selected fun	*/
  }

/************************************************************************/
/*			asn1_objidcmp					*/
/* Function to compare to OBJECT IDENTIFIERs				*/
/* RETURN:	0 if same						*/
/*		1 if different						*/
/************************************************************************/

ST_BOOLEAN asn1_objidcmp (MMS_OBJ_ID *obj1, MMS_OBJ_ID *obj2)
  {
ST_INT i;

  if (obj1->num_comps != obj2->num_comps)
    return (1);

  for (i = 0; i < obj1->num_comps; i++)
    {
    if (obj1->comps[i] != obj2->comps[i])
      return (SD_TRUE);
    }

  return (SD_FALSE);
  }

/************************************************************************/
/*			asn1r_init_glb_vars				*/
/************************************************************************/

#if defined(NO_GLB_VAR_INIT)

ST_VOID asn1r_init_glb_vars (ST_VOID)
  {
  asn1_debug_sel = ASN1_LOG_ERR;
#if defined( __ALPHA )
  asn1_vax_double_type	= A_G_FLOAT;
#endif
#if defined( VAXC )
  asn1_vax_double_type	= A_D_FLOAT;
#endif
  }

#endif
/************************************************************************/
#ifdef DEBUG_SISCO
/************************************************************************/
/************************************************************************/
/*			_list_elmnt					*/
/* Function to print the attributes of a data element			*/
/************************************************************************/

static ST_VOID _list_elmnt (ASN1_DEC_CTXT *ac)
  {
ST_CHAR *ctxt;
ST_CHAR *pc;
ST_CHAR len[50];

  ctxt ="Bogus";
  if ((asn1_debug_sel & ASN1_LOG_DEC) == 0)
    return;

  switch (ac->asn1r_elmnt_class)
    {
    case (0x00) :
      ctxt = "Univ";
    break;
    case (0x40) :
      ctxt = "App ";
    break;
    case (0x80) :
      ctxt = "Ctxt";
    break;
    case (0xC0) :
      ctxt = "Priv";
    break;
     }
 
  if (ac->asn1r_constr_elmnt)
    pc = "Cstr";
  else
    pc = "Prim";

  if (!ac->asn1r_indef_flag)
    sprintf (len,"%u", (unsigned int) ac->asn1r_elmnt_len);
  else
    strcpy (len,"Indef");

  ALOG_DECLF ();
  ALOG_CDEC4 ("Element : %s, %s, ID = %d, Len = %s",
			ctxt, pc, (int) ac->asn1r_elmnt_id, len);
  }

/************************************************************************/
#endif	/* end ifdef DEBUG_SISCO						*/
/************************************************************************/


	/*------------------------------------------------------*/
	/* MMS_BTOD to/from MMS_UTC_TIME conversion functions	*/
	/*------------------------------------------------------*/

/************************************************************************/
/*			asn1_convert_btod_to_utc			*/
/* This function converts MMS_BTOD (time relative to 1/1/1984) to the	*/
/* MMS_UTC_TIME (time relative to 1/1/1970).				*/
/* The qflags field in the MMS_UTC_TIME need to be set by the calling	*/
/* function.								*/
/* Only the MMS_BTOD6 form of the MMS_BTOD struct can be converted to	*/
/* the MMS_UTC_TIME.							*/ 
/* Parameters:								*/
/*	btod	pointer to MMS_BTOD struct that should be converted to	*/
/*		the MMS_UTC_TIME					*/
/*	utc	pointer to MMS_UTC_TIME struct where the result of the	*/
/*		conversion will be placed				*/
/* Return:								*/
/*	SD_SUCCESS	if function successful				*/
/*	SD_FAILURE	otherwise					*/
/************************************************************************/
ST_RET asn1_convert_btod_to_utc (MMS_BTOD *btod, MMS_UTC_TIME *utc)
  {
time_t tJan84 = TIME_T_1984_JAN_1;

  if (btod->form != MMS_BTOD6)
    {
    ALOG_NERR0 ("convert_btod_to_utc: MMS_BTOD4 can't be converted to UTC time");
    return (SD_FAILURE);
    }

  /* Now compute the MMS_UTC_TIME */
  utc->secs = (ST_UINT32) (tJan84 +
                           (time_t) (btod->day * SECONDS_PER_DAY) + (time_t) (btod->ms / 1000));
					/* num of seconds since Jan 1, 1970	*/
  /* NOTE: use 0x01000000 (2**24) in fraction computations.	*/
  utc->fraction = (ST_UINT32) ((ST_DOUBLE)(btod->ms % 1000) / 1000.0 * (ST_DOUBLE)0x01000000);
					/* fraction of a second	on 24-bits */

  return (SD_SUCCESS);
  }

/************************************************************************/
/*			asn1_convert_utc_to_btod			*/
/* This function converts MMS_UTC_TIME (time relative to 1/1/1970) to	*/
/* the MMS_BTOD (time relative to 1/1/1984).				*/
/* The form field in the MMS_BTOD is set to MMS_BTOD6 by this function.	*/
/* Parameters:								*/
/*	utc	pointer to MMS_UTC_TIME struct that should be converted	*/
/*		to the MMS_BTOD						*/
/*	btod	pointer to MMS_BTOD struct where the result of the	*/
/*		conversion will be placed				*/
/* Return:								*/
/*	SD_SUCCESS	if function successful				*/
/*	SD_FAILURE	otherwise					*/
/************************************************************************/
ST_RET asn1_convert_utc_to_btod (MMS_UTC_TIME *utc, MMS_BTOD *btod)
  {
time_t tJan84 = TIME_T_1984_JAN_1;

  /* Now compute the MMS_BTOD time	*/
  btod->day = (ST_INT32) (utc->secs - tJan84) / SECONDS_PER_DAY;	  /* num of days since 1/1/1984		*/
  btod->ms = (ST_INT32) ((utc->secs - tJan84) % SECONDS_PER_DAY) * 1000; /* num milliseconds since midnight	*/
  /* NOTE: use 0x01000000 (2**24) in fraction computations.	*/
  btod->ms += (ST_INT32) ((ST_DOUBLE) utc->fraction * 1000.0/(ST_DOUBLE)0x01000000);
							/* add the milliseconds left in a sec	*/
  btod->form = MMS_BTOD6;

  return (SD_SUCCESS);
  }

/************************************************************************/
/*			asn1_convert_timet_to_btime6			*/
/* Convert time_t value to MMS_BTIME6.					*/
/* Input "time_t" value is always based on UTC (GMT). It is compared	*/
/* to the define TIME_T_1984_JAN_1 which is also based on UTC (GMT).	*/
/* The difference is used to compute the number of seconds elapsed	*/
/* since 1984. Then seconds is converted to days & milliseconds.	*/
/************************************************************************/
ST_RET asn1_convert_timet_to_btime6 (time_t tThis, MMS_BTIME6 *btime6)
  {
time_t tJan84 = TIME_T_1984_JAN_1;
long elapsed;		/* elapsed time since 1984-Jan-1 (in seconds)	*/
ldiv_t divResult;
ST_RET retcode;

  /* Find the number of seconds since "1984-Jan-1" and convert to days & msec.*/
  /* NOTE: difftime returns "double" (don't know why), but ldiv needs "long".*/
  elapsed = (long) difftime (tThis, tJan84);

  if (elapsed < 0)
    {				/* "negative" elapsed time NOT ALLOWED	*/
    btime6->day = 0;		/* days since 1984-Jan-1		*/
    btime6->ms = 0;		/* milliseconds this day		*/
    retcode = SD_FAILURE;
    }
  else
    {
    divResult = ldiv (elapsed, SECONDS_PER_DAY);

    btime6->day = divResult.quot;		/* days since 1984		*/
    btime6->ms = (divResult.rem * 1000);	/* milliseconds this day	*/
    retcode = SD_SUCCESS;
    }

  return (retcode);
  }

/************************************************************************/
/*			bstrcpy						*/
/************************************************************************/
ST_VOID bstrcpy (ST_UCHAR *dstptr, ST_UCHAR *srcptr, ST_INT numbits)
  {
ST_INT num_bytes = numbits/8;	/* Number of complete bytes	*/
ST_INT extra_bits = numbits%8;	/* Number of extra bits		*/ 
ST_UCHAR src_mask;
ST_UCHAR dst_mask;

  if (num_bytes)
    memcpy (dstptr, srcptr, num_bytes);	
  
  if (extra_bits)
    {
    src_mask = 0xFF << (8-extra_bits);	/* Use high bits from src	*/
    dst_mask = 0xFF >> extra_bits;	/* Use low bits from dst	*/

    dstptr[num_bytes] &= dst_mask;
    dstptr[num_bytes] |= (srcptr[num_bytes] & src_mask);
    }
  }

/************************************************************************/
/*			bvstrcpy					*/
/* Copy one "variable length bitstring" to another.			*/
/************************************************************************/
ST_VOID bvstrcpy (MMS_BVSTRING *dstptr, MMS_BVSTRING *srcptr)
  {
  dstptr->len = srcptr->len;
  bstrcpy (dstptr->data, srcptr->data, srcptr->len);
  }

/************************************************************************/
/*			bstrcmp						*/
/* Compare bitstrings.							*/
/* RETURNS: 0 if bitstrings identical, 1 if different.			*/
/************************************************************************/
ST_INT bstrcmp (ST_UCHAR *dstptr, ST_UCHAR *srcptr, ST_INT numbits)
  {
ST_INT num_bytes = numbits/8;	/* Number of complete bytes	*/
ST_INT extra_bits = numbits%8;	/* Number of extra bits		*/ 
ST_UCHAR mask;
  
  if (num_bytes)
    {
    if (memcmp (dstptr, srcptr, num_bytes))
      return (1);	/* different	*/
    }
  
  if (extra_bits)
    {
    /* Mask off extra (low order) bits in last byte.	*/
    mask = 0xFF << (8-extra_bits);	/* Use high order bits	*/
    if ((dstptr[num_bytes] & mask) != (srcptr[num_bytes] & mask))
      return (1);	/* different	*/
    }
  return (0);	/* same	*/
  }

/************************************************************************/
/*			asn1r_skip_elmnt				*/
/* Skip over current ASN.1 element without decoding.			*/
/* Does not work for indefinite length encoding (returns SD_FAILURE).	*/
/* DEBUG: If indefinite len support needed, try using asn1r_parse_next.	*/
/************************************************************************/
ST_RET asn1r_skip_elmnt (ASN1_DEC_CTXT *aCtx)
  {
ST_RET retcode = SD_SUCCESS;

  if (aCtx->asn1r_constr_elmnt)
    {
    if (aCtx->asn1r_indef_flag)
      {
      ALOG_ERR0 ("asn1r_skip_elmnt: indefinite length encoding not supported");
      asn1r_set_dec_err (aCtx, ASN1E_UNEXPECTED_FORM); 
      retcode = SD_FAILURE;
      }
    else
      {
      aCtx->asn1r_constr_elmnt = 0;	/* Make ASN.1 treat it like primitive.	*/
      --aCtx->asn1r_msg_level;	/* Already incremented for constructor	*/
				/* so decrement to act like primitive.	*/
      aCtx->asn1r_field_ptr += aCtx->asn1r_elmnt_len;
      }
    }
  else
    {	/* primitive: just update field_ptr	*/
    aCtx->asn1r_field_ptr += aCtx->asn1r_elmnt_len;
    }
  return (retcode);
  }

/*======================================================================*/
/*======================================================================*/
/* The following functions are for a new decode method that does	*/
/* NOT use any function pointers:					*/
/*			asn1r_decode_init 				*/
/*			asn1r_byte_count_decrement (static)		*/
/*			asn1r_get_tag					*/
/*======================================================================*/
/*======================================================================*/

/************************************************************************/
/*			asn1r_decode_init 				*/
/* Initialize the ASN.1 decode context.					*/
/* IMPORTANT:  Use ONLY with "asn1r_get_tag".				*/
/* NOTE: This function does NOT use any of the function pointers in	*/
/*     ASN1_DEC_CTXT (_asn1r_tag_fun, asn1r_c_done_fun, etc.).		*/
/*     Also, several other parameters in ASN1_DEC_CTXT are NOT used.	*/
/* NOTE: To avoid a slow "memset" call, this function only sets the	*/
/*	necessary parameters in ASN1_DEC_CTXT (other parameters contain	*/
/*	random data).							*/
/************************************************************************/
ST_RET asn1r_decode_init (ASN1_DEC_CTXT *ac,
	ST_UCHAR *ptr,		/* ASN.1 message to decode	*/
	ST_INT len)		/* length of ASN.1 message	*/
  {
  /* Validate length to avoid possible problems	*/
  if (len <= 0)
    {
    ALOG_NERR0 ("ASN.1 decode: Length zero or negative");
    return (ASN1E_END_O_BUFFER);
    }

/* initialize flags, counters, pointers for decode			*/

  ac->asn1r_indef_flag = SD_FALSE;	/* clear flag			*/
  ac->_asn1_indef_track[0] = SD_FALSE;	/* level 0 is always definite	*/
  ac->_asn1_byte_count[0] = len;	/* set # bytes left in level 0	*/
  ac->asn1r_msg_level = 0;		/* start at message level 0	*/

  ac->asn1r_field_ptr = ptr;
  ac->asn1r_done_ptr  = ptr + len;
  ac->asn1r_msg_start = ptr;		/* save start ptr to compute	*/
					/* offset for logging.		*/

  ALOG_DEC1 ("ASN.1 MESSAGE TO DECODE : len = %d",len);
  ALOG_DECH (len,ptr);
  return (SD_SUCCESS);
  }

/************************************************************************/
/*			asn1r_byte_count_decrement			*/
/* If this is a "definite length" constructor, decrement its byte count.*/
/* NOTE: For "indefinite length" constructor, DO NOTHING.		*/
/************************************************************************/
static ST_RET asn1r_byte_count_decrement (ASN1_DEC_CTXT *ac,
	ST_INT msg_level,	/* constructor nesting level		*/
	ST_INT byte_count)	/* number of bytes processed		*/
  {
  if ( ! ac->_asn1_indef_track[msg_level])
    {
    /* This is "definite length" constructor. Decrement its byte count.	*/
    if (ac->_asn1_byte_count[msg_level] < byte_count)
      {
      if (msg_level == 0)
        ALOG_ERR0 ("ASN.1 decode: data exceeds message size");
      else 
        ALOG_ERR1 ("ASN.1 decode: data exceeds constructor length at message level %d", msg_level);
      return (ASN1E_CSTR_INVALID_LEN);
      }
    ac->_asn1_byte_count[msg_level] -= byte_count;
    }
  return (SD_SUCCESS);
  }

/************************************************************************/
/*			asn1r_get_tag					*/
/* Decode the next ASN.1 tag.						*/
/* IMPORTANT:  Use ONLY with "asn1r_decode_init".			*/
/* Must call 'asn1r_decode_init' once before any calls to this function.*/
/* NOTE: This function does NOT use any of the function pointers in	*/
/*     ASN1_DEC_CTXT (_asn1r_tag_fun, asn1r_c_done_fun, etc.).		*/
/*     Also does NOT use several other parameters in ASN1_DEC_CTXT.	*/
/* RETURNS: SD_SUCCESS or error code. Caller must check return value	*/
/*          instead of checking "ac->asn1r_pdu_dec_err".		*/
/************************************************************************/
ST_RET asn1r_get_tag (ASN1_DEC_CTXT *ac,
	ST_UINT16 *tag_out)	/* out: decoded tag		*/
  {
ST_INT head_len;	/* num bytes processed by asn1r_head_decode	*/
ST_RET rc;

  /* Before decoding the next tag, if current constructor is 		*/
  /* "definite length", check if it is finished.			*/
  /* NOTE: end of "indefinite length" constructor is a real tag, so	*/
  /*       it is processed AFTER the tag is decoded.			*/
  if (   ac->asn1r_msg_level > 0
      && ac->_asn1_indef_track[ac->asn1r_msg_level] == SD_FALSE	/* definite length*/
      && ac->_asn1_byte_count[ac->asn1r_msg_level] == 0)	/* constr done	*/
    {
    /* 0 bytes left at this constructor level (i.e. this constr done).	*/
    ALOG_CDEC1 ("Level %d parse complete",ac->asn1r_msg_level);
    ac->asn1r_msg_level--;
    *tag_out = 0;	/* 0 always indicates constructor done	*/
    return (SD_SUCCESS);
    }
    
  if (ac->asn1r_field_ptr >= ac->asn1r_done_ptr)
    {
    ALOG_ERR0 ("ASN.1 decode: past end of buffer");
    return (ASN1E_END_O_BUFFER);
    }

  ALOG_DEC1 ("Current decode offset: 0x%X",
             ac->asn1r_field_ptr - ac->asn1r_msg_start);
  ac->asn1r_field_start = ac->asn1r_field_ptr;	/* save field start address*/

  /* Clear old asn1r_indef_flag. May be set again by _asn1r_head_decode.*/
  ac->asn1r_indef_flag = SD_FALSE; 		/* clear flag	*/

  /* Decode the ASN.1 element header.	*/
  rc = _asn1r_head_decode (ac);
  if (rc != SD_SUCCESS)
    {
    ALOG_NERR0 ("ASN.1 decode: header decode error");
    return(rc);
    }

  /* Log this element (if enabled).	*/
#ifdef DEBUG_SISCO
  if (asn1_debug_sel & ASN1_LOG_DEC)
    {
    ST_INT log_len;
    log_len = (int) (ac->asn1r_field_ptr - ac->asn1r_field_start);
    /* For primitive, log the whole element	*/
    if (!ac->asn1r_constr_elmnt)
      log_len += ac->asn1r_elmnt_len;

    ALOG_DECH (log_len, ac->asn1r_field_start);	/* log bytes just processed*/

    _list_elmnt (ac);		/* log element attributes		*/
    }
#endif

  /* Generate tag to return to caller (*tag_out).			*/
  /* Our storage of tags in ST_UINT16 can only handle elmnt_id of	*/
  /* 13 bits (high 3 bits indicate Class and Constructor).		*/
  /* This should be good enough for anything we need to parse.		*/
  if (ac->asn1r_elmnt_id > 0x1fff)
    {
    ALOG_ERR1 ("ASN.1 element id = %u unsupported", ac->asn1r_elmnt_id);
    return (SD_FAILURE);		/* should NEVER get here	*/
    }
  *tag_out = ((ST_UINT16) (ac->asn1r_elmnt_class | ac->asn1r_constr_elmnt) << 8)
             | ac->asn1r_elmnt_id;

  /* Check for Indefinite Length End of Constructor (EOC)(i.e. tag==0)	*/
  if (*tag_out == 0)
    {
    /* EOC found. Was constr at this level "indefinite length"?		*/
    if (ac->_asn1_indef_track[ac->asn1r_msg_level])
      {
      ST_INT indef_len;	/* Length of this indefinite length constructor	*/
      ALOG_CDEC1 ("EOC detected for level %d",ac->asn1r_msg_level);
      indef_len = (int) (ac->asn1r_field_ptr - ac->_asn1_constr_start[ac->asn1r_msg_level]);

      /* Decrement the byte count of the "higher level" constr.		*/
      rc = asn1r_byte_count_decrement (ac, ac->asn1r_msg_level - 1, indef_len);
      if (rc)
        return (rc);

      ALOG_CDEC1 ("Level %d parse complete",ac->asn1r_msg_level);
      ac->asn1r_msg_level--;
      }
    else
      {
      ALOG_NERR0 ("ASN.1 decode: unexpected end of indefinite length constructor");
      return (ASN1E_UNEXPECTED_TAG);	/* EOC not allowed here 	*/
      }
    /* ALWAYS return when EOC found.					*/
    /* NOTE: "*tag_out" already set to 0 to indicate constructor done.	*/
    return (SD_SUCCESS);
    }	/* end "if (*tag_out == 0)"	*/

  /* Compute number of bytes processed by asn1r_head_decode.		*/
  /* Need this to decrement constructor byte counts below.		*/
  head_len = (int) (ac->asn1r_field_ptr - ac->asn1r_field_start);

  if (ac->asn1r_constr_elmnt)
    {
    /* Process constructor.	*/
    if (++ac->asn1r_msg_level >= ASN1_MAX_LEVEL)
      {
      ALOG_ERR0 ("Error : ASN.1 decode exceeded nesting level");
      return (ASN1E_NEST_TOO_DEEP);
      }

    /* Save "indefinite length" flag for this constructor level.	*/
    ac->_asn1_indef_track[ac->asn1r_msg_level] = ac->asn1r_indef_flag;

    if (ac->asn1r_indef_flag)
      {
      /* Save start ptr for this constructor. Use to compute len later.	*/
      ac->_asn1_constr_start[ac->asn1r_msg_level] = ac->asn1r_field_ptr;
      /* _asn1_byte_count NOT USED for indef len constructor. Set to 0,	*/
      /* just so it's easier to see in the debugger.			*/
      ac->_asn1_byte_count[ac->asn1r_msg_level] = 0;	/* NOT USED for indef*/

      /* Decrement the byte count of the "higher level" constr.		*/
      /* Use "head_len". Adjust for "data len" later when EOC is found.	*/
      rc = asn1r_byte_count_decrement (ac, ac->asn1r_msg_level - 1, head_len);
      if (rc)
        return (rc);
      }
    else
      {
      /* definite length constructor. Initialize byte count for it.	*/
      ac->_asn1_byte_count[ac->asn1r_msg_level] = ac->asn1r_elmnt_len;
      /* Decrement the byte count of the "higher level" constr.		*/
      rc = asn1r_byte_count_decrement (ac, ac->asn1r_msg_level - 1,
                head_len+ac->asn1r_elmnt_len);
      if (rc)
        return (rc);
      }
    }	/* end constructor processing	*/
  else
    {
    /* This is a primitive.	*/
    /* Decrement the byte count of the "current" constr.		*/
    rc = asn1r_byte_count_decrement (ac, ac->asn1r_msg_level,
              head_len+ac->asn1r_elmnt_len);
    if (rc)
      return (rc);
    }

  return (SD_SUCCESS);
  }

/************************************************************************/
/*			asn1r_chk_tag					*/
/* Make sure tag returned by "asn1r_get_tag" matches one in "expected	*/
/* tag" array. If it doesn't match a "mandatory" tag, return error.	*/
/************************************************************************/
ST_RET asn1r_chk_tag (
	ST_UINT16 tag_num,	/* tag to check				*/
				/* returned by "asn1r_get_tag"		*/
	TAG_INFO *tag_arr,	/* array of expected tags		*/
	ST_INT tag_count,	/* num elem in tag_arr			*/
	ST_INT *p_tag_idx)	/* in/out: next in tag_arr to check	*/
  {
ST_RET retcode = ASN1E_UNEXPECTED_TAG;
TAG_INFO *tag_info;	/* ptr to current array entry	*/
ST_INT tag_idx = *p_tag_idx;

  for (tag_idx = *p_tag_idx;  tag_idx < tag_count;  tag_idx++)
    {
    tag_info = &tag_arr[tag_idx];
    
    if (tag_info->tag_num == tag_num)
      {
      *p_tag_idx = tag_idx+1;	/* point to next expected tag	*/
      retcode = SD_SUCCESS;
      break;
      }
    else
      {
      /* no match. Is it mandatory?	*/
      if (tag_info->mandatory)
        {
        ALOG_ERR2 ("ASN.1 decode: unexpected tag 0x%X, expecting 0x%X",
             tag_num, tag_info->tag_num);
        break;
        }
      }
    }
  return (retcode);
  }

/************************************************************************/
/*			asn1r_skip_contents_constr			*/
/* Skip over the contents of a constructor. All data ignored.		*/
/* This function loops calling "asn1r_get_tag" until an error occurs or	*/
/* the end of this constructor.						*/
/* NOTE: This is the slow way. It should only be used for "indefinite	*/
/*       length" constructors (no other way).				*/
/************************************************************************/
ST_RET asn1r_skip_contents_constr (ASN1_DEC_CTXT *ac)
  {
ST_RET rc = SD_SUCCESS;
ST_UINT16 tagnum;

  /* Loop calling "asn1r_get_tag" until error or end of this constructor.*/
  while ((rc = asn1r_get_tag (ac, &tagnum)) == SD_SUCCESS &&
         tagnum != 0)	/* 0 indicates end of constructor	*/
    {
    if (ac->asn1r_constr_elmnt)
      {
      /* Start of new constructor. Recursively skip over it.	*/
      rc = asn1r_skip_contents_constr (ac);	/* skip "nested" constr*/
      if (rc)
        return (rc);	/* return on first error	*/
      }
    else	/* primitive	*/
      {
      ac->asn1r_field_ptr += ac->asn1r_elmnt_len;
      }
    }
  return (rc);
  }

/************************************************************************/
/*			asn1r_skip_contents				*/
/* Skip over the ASN.1 contents octets (constructor or primitive).	*/
/* Must call "asn1r_get_tag" first to process the tag and length octets.*/
/* CRITICAL: this funct only works with the "asn1r_get_tag" decode mode.*/
/************************************************************************/
ST_RET asn1r_skip_contents (ASN1_DEC_CTXT *ac)
  {
ST_RET rc;

  rc = SD_SUCCESS;		/* assume success. This changes only if	*/
				/* asn1r_skip_contents_constr fails.	*/

  if (ac->asn1r_indef_flag)
    {
    /* Indefinite length constructor. Call function to skip over it.	*/
    /* This is much slower but the only way with Indefinite Length.	*/
    rc = asn1r_skip_contents_constr (ac);
    }
  else
    {
    /* Definite length constructor or primitive.	*/
    /* Just adjust ptr to skip it. Much faster than decoding.	*/
    if (ac->asn1r_constr_elmnt)
      {		/* constructor	*/
      ac->asn1r_constr_elmnt = 0;	/* treat it like primitive.	*/
      --ac->asn1r_msg_level;	/* Already incremented for constructor	*/
				/* so decrement to act like primitive.	*/
      }
    ac->asn1r_field_ptr += ac->asn1r_elmnt_len;
    }
  return (rc);
  }

/************************************************************************/
/*			asn1r_get_contents				*/
/* Get the ASN.1 contents.						*/
/* CRITICAL: Must call "asn1r_get_tag" right before this to decode the	*/
/*           tag and length octets.					*/
/************************************************************************/
ST_RET asn1r_get_contents (ASN1_DEC_CTXT *ac,
	ST_UCHAR **p_contents_ptr,	/* out: ptr to contents	*/
	ST_INT *p_contents_len)		/* out: len of contents	*/
  {
ST_RET rc;
ST_BOOLEAN indef_flag;
ST_UCHAR *start_ptr;
ST_INT len;

  indef_flag = ac->asn1r_indef_flag;	/* save initial flag	*/
  start_ptr = ac->asn1r_field_ptr;	/* save initial ptr	*/

  rc = asn1r_skip_contents (ac);	/* skip over contents	*/

  if (rc == SD_SUCCESS)
    {
    /* Subtract initial ptr from current ptr to get the length.	*/
    /* If this was indef len constr, current ptr points after	*/
    /* terminating "00 00" so must subtract an extra 2.		*/
    if (indef_flag)
      len = (int) (ac->asn1r_field_ptr - start_ptr - 2);
    else
      len = (int) (ac->asn1r_field_ptr - start_ptr);

    *p_contents_ptr = start_ptr;
    *p_contents_len = len;
    }
    
  return (rc);
  }


