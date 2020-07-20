/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc., 	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : ms_tdef.c						*/
/* PRODUCT(S)  : MMS-EASE						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*	This module contains some MMS-EASE virtual machine support	*/
/*	functions that help the user with MMS variable access and	*/
/*	type specifications.		 				*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 12/01/11  JRB	   ms_runtime_to_tdl: remove invalid comp_name	*/
/*			   at start of TDL.				*/
/* 11/23/11  KCR           Added casting for proper 64bit compile       */
/* 06/22/07  JRB     26    Log max on buffer overrun.			*/
/* 01/17/06  NAV     25    ms_runtime_to_tdl: fix tdl_buf overwrite	*/
/* 07/12/04  JRB     24    For RT_UTF8_STRING, chg "t18" to "t16" in	*/
/*			   "tdlTypes" to match latest 61850-8-1.	*/ 
/* 05/03/04  JRB     23    ms_mk_asn1_type: return err if struct empty,	*/
/*			   ignore extra commas, use ASN1_ENC_* macros,	*/
/*			   & log the ASN.1 produced.			*/
/*			   type_subst: remove whitespace in dest string.*/
/* 10/13/03  JRB     22    ms_runtime_to_tdl: Chg assert to err return.	*/
/* 07/14/03  JRB     21    Use strlen(comp_name), was assuming maxlen=32*/
/* 04/22/03  JRB     20    Allow fixed len UTF8 only if local fmt=UTF16.*/
/* 04/02/03  JRB     19    Add UTF8string support (see RT_UTF8_STRING).	*/
/*			   Use "RT_*" defines in "switch (type)".	*/
/*			   DRASTICALLY simplify ms_runtime_to_tdl.	*/
/*			   add_tdl_separator: add args (& fix spelling):*/
/*			   - allows 3 global vars to be eliminated.	*/
/*			   - allows semaphore to be eliminated.		*/
/* 12/15/02  JRB     18    Add ms_tdl_to_runtime function.		*/
/* 11/11/02  JRB     17    Replace ms_asn1_to.. w/ ms_runtime_create.	*/
/* 03/04/02  JRB     16    Add more SD_CONST. Add str_util.h.		*/
/* 12/20/01  JRB     15    Converted to use ASN1R (re-entrant ASN1)	*/
/* 11/13/01  EJV     14    Added support for new MMS type UtcTime:	*/
/*			   tdlTypes[]: added new type "Utctime" [17]	*/
/*			   ms_mk_asn1_type: added case for type 17.	*/
/*			   anticipated[]: added type 17 and moved other.*/
/*			   ms_runtime_to_tdl: added RT_UTC_TIME case.	*/
/* 03/08/01  JRB     13    Chk return val from ms_comp_name_find.	*/
/* 08/24/00  JRB     12    Use ms_comp_name_find, so code works for	*/
/*			   MMS_LITE (i.e. USE_RT_TYPE_2 or USE_RT_TYPE_3*/
/*			   defined).					*/
/* 07/28/00  JRB     11    Use new defines in #ifdef.			*/
/* 09/13/99  MDE     10    Added SD_CONST modifiers			*/
/* 09/21/98  MDE     09    Minor lint cleanup				*/
/* 08/21/98  MDE     08    Eliminated warning in ms_asn1_to_tdl.	*/
/* 06/03/98  MDE     07    Now check 'ms_asn1_to_runtime' return code 	*/
/* 03/11/98  MDE     06    Removed NEST_RT_TYPES			*/
/* 02/10/98  MDE     05    Runtime type changes for LITE		*/
/* 10/30/97  MDE     04    Calculate size of RT table before alloc	*/
/* 09/16/97  MDE     03    Now don't require @VMD for referenced type	*/
/* 06/19/97  MDE     02    Added 64 bit integer support, cleanup	*/
/* 06/09/97  MDE     01    Changed Runtime Type, and it's use		*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "glbsem.h"
#include "mmsdefs.h"
#include "asn1defs.h"
#include "mem_chk.h"
#include "str_util.h"	/* for strnicmp	*/

/************************************************************************/

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of  	*/
/* __FILE__ strings.                            			*/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif


/************************************************************************/
/* TDL DEFINES								*/
/* Defined constants giving the primitive syntax for the keywords:	*/
/* Bool, Byte, Short, Long, Ubyte, Ushort, Ulong, Float, Double,	*/
/* Vstring, Fstring, Ostring, Bstring, Gtime, Btime4, Btime8, respec-	*/
/* tively.								*/
/* These are used in 'ms_mk_asn1_type' for substitutions		*/
/************************************************************************/

/************************************************************************/

static ST_INT type_subst (ST_CHAR *dest_str, ST_CHAR *src_str, ST_INT src_len);
static ST_RET wr_object_name (ASN1_ENC_CTXT *aCtx, ST_CHAR *objname);
static ST_UCHAR *err_rtn (ST_RET error, ST_CHAR *tspec);

#ifdef MMS_LITE
ST_INT  m_rt_type_limit = 50;	/* max number of runtime blocks per def */
#endif

/* These checks catch if a define change breaks the code using "tdlTypes"*/
/* The way "tdlTypes" is used, does not work if any of the RT_* defines change.*/
/* It would be better to modify the code to always use the RT_* defines,*/
/* but that is not so easy now.						*/
#if (RT_BOOL           != 3)
#error define does't match expected. tdlTypes array won't work.
#endif
#if (RT_BIT_STRING     != 4)
#error define does't match expected. tdlTypes array won't work.
#endif
#if (RT_INTEGER        != 5)
#error define does't match expected. tdlTypes array won't work.
#endif
#if (RT_UNSIGNED       != 6)
#error define does't match expected. tdlTypes array won't work.
#endif
#if (RT_FLOATING_POINT != 7)
#error define does't match expected. tdlTypes array won't work.
#endif
#if (RT_OCTET_STRING   != 9)
#error define does't match expected. tdlTypes array won't work.
#endif
#if (RT_VISIBLE_STRING != 10)
#error define does't match expected. tdlTypes array won't work.
#endif
#if (RT_GENERAL_TIME   != 11)
#error define does't match expected. tdlTypes array won't work.
#endif
#if (RT_BINARY_TIME    != 12)
#error define does't match expected. tdlTypes array won't work.
#endif
#if (RT_BCD            != 13)
#error define does't match expected. tdlTypes array won't work.
#endif
#if (RT_BOOLEANARRAY   != 14)
#error define does't match expected. tdlTypes array won't work.
#endif
#if (RT_UTC_TIME       != 17)
#error define does't match expected. tdlTypes array won't work.
#endif
#if (RT_UTF8_STRING    != 16)
#error define does't match expected. tdlTypes array won't work.
#endif

/************************************************************************/

typedef struct 
  {
  ST_CHAR *abbrev;
  ST_CHAR *subType;
  } TDL_TYPES;

/* tdlTypes array
 * CRITICAL: the number after the "t" in the 2nd string of each struct
 * MUST match the ASN.1 tag for that type. The tags are defined in "mms_vvar.h"
 * and they have a prefix of "RT_".
 * For example, the ASN.1 tag for Boolean is 3, so "mms_vvar.h" has the define:
 *   #define RT_BOOL 3
 * and the string in this array for Boolean is:
 *   "t3"
 */

TDL_TYPES tdlTypes[] =
  {
    { "Bool", 		"t3" 		},
    { "Byte", 		"t5+8" 		},
    { "Short", 		"t5+16" 	},
    { "Long", 		"t5+32" 	},
#ifdef INT64_SUPPORT
    { "Int64", 		"t5+64" 	},
#endif
    { "Ubyte", 		"t6+8" 		},
    { "Ushort", 	"t6+16" 	},
    { "Ulong", 		"t6+32" 	},
#ifdef INT64_SUPPORT
    { "Uint64", 	"t6+64" 	},
#endif
    { "Float", 		"t7+2071" 	},
    { "Double", 	"t7+2868" 	},
    { "Vstring",	"t10-" 		},
    { "Fstring",	"t10+" 		},
    { "Ostring",	"t9+" 		},
    { "OVstring",	"t9-" 		},
    { "Bstring", 	"t4+" 		},
    { "BVstring", 	"t4-" 		},
    { "Gtime", 		"t11" 		},
    { "Btime4", 	"t12+0" 	},
    { "Btime6", 	"t12+1" 	},
    { "Bcd", 		"t13+" 		},
    { "Utctime",   	"t17" 		},
    { "UTF8Vstring",   	"t16-" 		},
#if (UNICODE_LOCAL_FORMAT==UNICODE_UTF16)
    { "UTF8string",   	"t16+" 		},
#endif
  }; 
ST_INT numTdlTypes = (sizeof (tdlTypes)/sizeof(TDL_TYPES));

/************************************************************************/
/*			   ms_mk_asn1_type				*/
/*  Function to convert a character string type specification to an	*/
/*  ASN.1 string, as required by the ms_add_named_type function.  It	*/
/*  is assumed that the function strt_asn1 has been called elsewhere	*/
/*  in the user's program.  The input 'tspec' character string must 	*/
/*  conform to the following syntax for specifying a type:		*/
/*									*/
/*	Let N be a number, i.e. a sequence of digits.  Let 		*/
/*	IDENTIFIER be a character string which is a valid MMS 		*/
/*	identifier.  And let OBJECT_NAME be a character string 		*/
/*	which represents a valid MMS object name of the form:  		*/
/*	string1:IDENTIFIER, where string1 is "+" for a VMD-		*/
/*	specific object, "-" for an AA-specific object, or a		*/
/*	valid IDENTIFIER naming a domain for a domain-specific 		*/
/*	object.  Then we have the following Bacchus-Nauer rules		*/
/*	for specifying the syntax:					*/
/*									*/
/*	Type ::= <OBJECT_NAME> | tN+N | tN-N | tN | [N:Type] 		*/
/*		| [pN:Type] | {Component_list} | {p:Component_list}	*/
/*	Component_list ::= Component | Component_list,Component		*/
/*	Component ::= Type | (IDENTIFIER)Type 	 			*/
/*									*/
/*  The above syntax may contain white space (blanks, tabs and new-	*/
/*  lines) to make it more readable.  (Hence, a character string	*/
/*  constant input for 'tspec' may extend over several lines.)  	*/
/*  However, the symbols appearing in the definition above (called	*/
/*  terminal symbols), such as '<', '>', 't', '+', ,'-', '[', ':', ']',	*/
/*  '{', '}', '(', ')', 'p' and ',', must be used exactly as specified.	*/
/*  The meaning of the above syntax is given below:			*/
/*									*/
/*	<OBJECT_NAME>   - Name of a type object that has already been	*/
/*			  declared to MMS-EASE using ms_add_named_type	*/
/*			  or ms_add_std_types.				*/
/*	tN+N, tN-N, tN  - Primitive type, where the first N indicates	*/
/*			  the basic type, and the second N is the	*/
/*			  length of the type (normally in bits, but	*/
/*			  for octet-strings and visible-strings, it	*/
/*			  represents the number of bytes, and floats	*/
/*			  are a special case - see note below).  The 	*/
/*			  basic types are explained below.  If the type */
/*			  length (+N or -N) is left off, it defaults 	*/
/*			  to zero.  Note that char strings may have 	*/
/*			  negative type lengths, indicating variable-	*/
/*			  length (i.e., can be <= N), but all  other	*/
/*			  types must have nonnegative type lengths.	*/
/*	[N:Type]	- Array of N elements of the specified type.	*/
/*     {Component_list} - Structure consisting of one or more compo-	*/
/*			  nents, separated by commas, with or without	*/
/*			  component names.				*/
/*     (IDENTIFIER)Type - A component of a structure identified by a	*/
/*			  component name, in order that it may be 	*/
/*			  referenced in a subsequent alternate access.	*/
/*	p		- Used in arrays or structures to indicate 	*/
/*			  that it is 'packed', i.e. compressed in some 	*/
/*			  implementation-specific way.			*/
/*									*/
/*  The basic types are numbers (first N following the 't' of a primi-	*/
/*  tive type) with the following meanings:				*/
/*									*/
/*	3  - Boolean, type length = 0 (or absent). Ex: "t3".		*/
/*	4  - Bit-string, type length >= 0.  Ex: "t4+27" for a 27-bit	*/
/*		bit-string.						*/
/*	5  - Integer, type length = 16 or 32 for most computers.  Ex:	*/
/*		"t5+16" for a 2-byte integer, "t5+32" for a 4-byte int.	*/
/*	6  - Unsigned, type length = 16 or 32 for most computers.  Ex:	*/
/*		"t6+16" for a 2-byte unsigned integer.			*/
/*	7  - Floating point, type length = 2071 for single precision	*/
/*		or 2868 for double precision on most computers.  See	*/
/*		the note below.						*/
/*	9  - Octet-string, type length >= 0.  Ex: "t9+19" for a 19-	*/
/*		byte octet-string.					*/
/*	10 - Visible-string, any type length.  Ex: "t10+30" specifies 	*/
/*		a fixed-length string of exactly 30 characters, whereas */
/*		"t10-12" specifies a variable-length string of at most 	*/
/*		12 characters.						*/
/*	11 - Generalized-time, type length = 0 (or absent).  Ex: "t11".	*/
/*	12 - Binary-time, type length = 0 or 1 (SD_FALSE or SD_TRUE).  Ex:	*/
/*		"t12+1" for a 6-byte binary-time time of day, including */
/*		date.							*/
/*	13 - BCD, type length >= 0  AND <= 8				*/
/*		computers).  Ex: "t13+8" for a 8-nibble BCD integer.	*/
/*      17 - UTC Time is struct of 3 unsigned integers encoded as	*/
/*		8 bytes octet string (ssssqmmm), where 			*/	
/*              ssss seconds since Jan 1, 1970, 4 bytes padded with	*/
/*		     leading 0's					*/
/*		q    quality flags, 1 byte				*/
/*              mmm  microseconds in a second, 3 bytes in range		*/
/*		     [0,999999], padded with 0's.			*/
/*	     Note: this is new type added to the spec in 2001/2002.	*/
/*	16 - UTF8string, any type length.  Ex: "t16+30" specifies 	*/
/*		a fixed-length string of exactly 30 characters, whereas */
/*		"t16-12" specifies a variable-length string of at most 	*/
/*		12 characters.						*/
/*									*/
/*	NOTE: Floating point is a special case in that its length is	*/
/*	a combination of two numbers: fractional width (say 52 bits) 	*/
/*	and exponent width (say 11 bits).  The length of the floating	*/
/*	point type is computed according to the following algorithm:	*/
/*			N = e*256 + f, 					*/
/*	where f = fractional width and e = exponent width.  For stan-	*/
/*	dard IEEE floating point format, N = 2071 for single-precision 	*/
/*	and N = 2868 for double-precision.  Hence, most users will use 	*/
/*	the following notation for floating point types:		*/
/*	  "t7+2071" for single prec. and "t7+2868" for double prec.	*/
/*	WARNING: This function will not accept type specifications for	*/
/*	floating point exponents with a width greater than 127 bits or	*/
/*	mantissas with a width greater than 255 bits.			*/
/*									*/
/*  The other input arguments of this function are the length of a 	*/
/*  buffer to hold the ASN.1 string, and a pointer to that buffer.  	*/
/*  The 'asn1_len' argument is modified to equal the actual length of	*/
/*  the ASN.1 string produced.  					*/
/*									*/
/*  If there is no error in this function, the return value is a 	*/
/*  pointer to where the ASN.1 string starts in	the buffer.  (The 	*/
/*  ASN.1 string always ends at the end of the buffer, but it begins 	*/
/*  anywhere within the buffer.)  If an error does occur in this 	*/
/*  function, a NULL pointer is returned, and the global variable	*/
/*  'mms_op_err' gives further detail as to the nature of the problem 	*/
/*  that has caused this function to fail.				*/
/************************************************************************/

#define TYPE_NAME 	0
#define BEG_STRUCT 	2
#define PACKED 		0
#define COMPONENT_NAME 	0 
#define BEG_COMP_TYPE 	1 


ST_UCHAR *ms_mk_asn1_type (ST_INT *asn1_len, ST_UCHAR *asn1, ST_CHAR *type_spec)
  {
ST_CHAR *tspec;
ST_INT type;  
ST_BOOLEAN literal;  
ST_BOOLEAN number;
ST_INT type_len;
ST_BOOLEAN name;
ST_BOOLEAN m_packed;
ST_INT i;
ST_LONG num_elts;
ST_UCHAR acc_float_size;
ASN1_ENC_CTXT localEncCtx;	/* For readability, use "aCtx" to access this.*/
ASN1_ENC_CTXT *aCtx = &localEncCtx;


/* Initialize everything first.						*/

  S_LOCK_COMMON_RESOURCES ();
  num_elts = 0L;
  number = SD_FALSE;
  name = SD_FALSE;
  literal = SD_FALSE;
  m_packed = SD_FALSE;
  type_len = 0;
  i = (ST_INT) strlen (type_spec);
  tspec = (ST_CHAR *) chk_malloc (2*i);
  asn1r_strt_asn1_bld (aCtx, asn1,*asn1_len);

/*  Replace abbreviations in the type definition string with the	*/
/*  appropriate syntax and reset new length of type spec string.	*/
  if ((i = type_subst (tspec,type_spec,i)) < 1)
    {
    S_UNLOCK_COMMON_RESOURCES ();
    return (err_rtn (MVE_TYPEDEF_LEN0,tspec));
    }
/*  Loop through the 'tspec' string, from the back to the front.	*/

  while (i > 0)
    {
    switch (tspec[--i])
      {
      case '>' :
	if (number || m_packed)
          {
          S_UNLOCK_COMMON_RESOURCES ();
	  return (err_rtn (MVE_TYPEDEF_SYM_GT,tspec));
          }
	literal = SD_TRUE;
	tspec[i] = '\0';
	asn1r_strt_constr (aCtx);
	break;

      case '<' :
	if (number || m_packed)
          {
          S_UNLOCK_COMMON_RESOURCES ();
	  return (err_rtn (MVE_TYPEDEF_SYM_LT,tspec));
          }
	literal = SD_FALSE;
	if (wr_object_name (aCtx, &tspec[i+1]))
          {
          S_UNLOCK_COMMON_RESOURCES ();
	  return (err_rtn (MVE_TYPEDEF_OBJNAME,tspec));
          }
	asn1r_fin_constr (aCtx, TYPE_NAME, CTX, 0);
	break;

      case '}' :
	if (number || m_packed)
          {
          S_UNLOCK_COMMON_RESOURCES ();
	  return (err_rtn (MVE_TYPEDEF_SYM_RBRACE,tspec));
          }
	asn1r_strt_constr (aCtx);				/* Start structure type.*/
	asn1r_strt_constr (aCtx);				/* Start component part.*/
	asn1r_strt_constr (aCtx);				/* Start last component.*/
	asn1r_strt_constr (aCtx);				/* Start component's 	*/
	break;					/* explicit type spec.	*/

      case '{' :
	if (number)
          {
          S_UNLOCK_COMMON_RESOURCES ();
	  return (err_rtn (MVE_TYPEDEF_SYM_LBRACE,tspec));
          }
        if (tspec[i+1]=='}')	/* Empty struct	*/
          {
          MLOG_ERR2 ("ms_mk_asn1_type: empty struct defined starting at offset %d TDL=%s", i, &tspec[i]);
          S_UNLOCK_COMMON_RESOURCES ();
	  return (err_rtn (MVE_TYPEDEF_SYM_LBRACE,tspec));
          }
	if (!name)
	  asn1r_fin_constr (aCtx, BEG_COMP_TYPE, CTX, 0);
	asn1r_fin_constr (aCtx, SEQ_CODE, UNI, 0);
	name = SD_FALSE;
	asn1r_fin_constr (aCtx, 1, CTX, 0);
	if (m_packed)
	  {
	  asn1r_wr_i16 (aCtx, 1);
	  asn1r_fin_prim (aCtx, PACKED,CTX);
	  }
	asn1r_fin_constr (aCtx, BEG_STRUCT, CTX, 0);
	m_packed = SD_FALSE;
	break;

      case ',' :
	if (number)
          {
          S_UNLOCK_COMMON_RESOURCES ();
	  return (err_rtn (MVE_TYPEDEF_SYM_COMMA,tspec));
          }
        if (tspec[i+1]=='}' || tspec[i+1]==',')	/* Ignore extra commas	*/
          {
          MLOG_ENC0 ("ms_mk_asn1_type: ',' followed by ',' or '}' in TDL string ignored.");
          }
        else
          {
          if (!name)
            asn1r_fin_constr (aCtx, BEG_COMP_TYPE, CTX, 0);
          asn1r_fin_constr (aCtx, SEQ_CODE, UNI, 0);
          name = SD_FALSE;
          asn1r_strt_constr (aCtx);
          asn1r_strt_constr (aCtx);
          }
	break;

      case ')' :
	if (number || m_packed)
          {
          S_UNLOCK_COMMON_RESOURCES ();
	  return (err_rtn (MVE_TYPEDEF_SYM_RPAREN,tspec));
          }
	asn1r_fin_constr (aCtx, BEG_COMP_TYPE, CTX, 0);
	name = SD_TRUE;
	literal = SD_TRUE;
	tspec[i] = '\0';
	break;	

      case '(' :
	if (number || m_packed)
          {
          S_UNLOCK_COMMON_RESOURCES ();
	  return (err_rtn (MVE_TYPEDEF_SYM_LPAREN,tspec));
          }
	literal = SD_FALSE;
	asn1r_wr_vstr (aCtx, &tspec[i+1]);
	asn1r_fin_prim (aCtx, COMPONENT_NAME, CTX);
	break;

      case 'p' :
        if (literal)
          break;
	if (m_packed)
          {
          S_UNLOCK_COMMON_RESOURCES ();
	  return (err_rtn (MVE_TYPEDEF_SYM_P,tspec));
          }
	m_packed = SD_TRUE;
	break;	

      case '+' :
        if (literal)
          break;
	if (!number || m_packed)
          {
          S_UNLOCK_COMMON_RESOURCES ();
	  return (err_rtn (MVE_TYPEDEF_SYM_PLUS,tspec));
          }
	type_len = atoi (&tspec[i+1]);
	tspec[i] = '\0';
	number = SD_FALSE;
	break;

      case '-' :
        if (literal)
          break;
	if (!number || m_packed)
          {
          S_UNLOCK_COMMON_RESOURCES ();
	  return (err_rtn (MVE_TYPEDEF_SYM_MINUS,tspec));
          }
	type_len = -atoi (&tspec[i+1]);
	tspec[i] = '\0';
	number = SD_FALSE;
	break;

      case 't' :
	if (literal)
	  break;
	if (!number || m_packed)
          {
          S_UNLOCK_COMMON_RESOURCES ();
	  return (err_rtn (MVE_TYPEDEF_SYM_T,tspec));
          }
	type = atoi (&tspec[i+1]);
	switch (type)
	  {
	  case RT_BOOL :
	  case RT_GENERAL_TIME :
	  case RT_UTC_TIME :
	    if (type_len)
              {
              S_UNLOCK_COMMON_RESOURCES ();
	      return (err_rtn (MVE_TYPEDEF_BADLEN3,tspec));
              }
	    asn1r_fin_prim (aCtx, (ST_UINT16) type,CTX);
	    break;

	  case RT_BIT_STRING :
	  case RT_OCTET_STRING :
	  case RT_VISIBLE_STRING :
          case RT_UTF8_STRING:
	    asn1r_wr_i32 (aCtx, (ST_INT32) type_len);
	    asn1r_fin_prim (aCtx, (ST_UINT16) type,CTX);
	    break;

	  case RT_INTEGER :
	  case RT_UNSIGNED :
	  case RT_BINARY_TIME :
	    if (type_len > 127)
              {
              S_UNLOCK_COMMON_RESOURCES ();
	      return (err_rtn (MVE_TYPEDEF_BADLEN5,tspec));
              }
	    asn1r_wr_u8 (aCtx, (ST_UCHAR) type_len);
	    asn1r_fin_prim (aCtx, (ST_UINT16) type, CTX);
	    break;
			
	  case RT_BCD :
	    if (type_len > 8 || type_len <= 0)
              {
              S_UNLOCK_COMMON_RESOURCES ();
	      return (err_rtn (MVE_TYPEDEF_BADLEN5,tspec));
              }
	    asn1r_wr_u8 (aCtx, (ST_UCHAR) type_len);
	    asn1r_fin_prim (aCtx, (ST_UINT16) type, CTX);
	    break;

	  case RT_FLOATING_POINT :
	    if (type_len < 256)
              {
              S_UNLOCK_COMMON_RESOURCES ();
	      return (err_rtn (MVE_TYPEDEF_BADLEN7,tspec));
              }
	    asn1r_strt_constr (aCtx);
	    asn1r_wr_u8 (aCtx, (ST_UCHAR) ((type_len>>8) & 0xFF));	  /* write exp	*/
	    asn1r_fin_prim (aCtx, INT_CODE,UNI);

/* For IS MMS, first octet is exponent + sign + mantissa 		*/
/* For DIS MMS, first octet is mantissa 				*/

            if (!m_use_dis_float)
              {
	      acc_float_size =(ST_UCHAR) ((type_len>>8) & 0xFF);	/* exp 	*/
	      acc_float_size += (ST_UCHAR)(type_len & 0xFF);	/* fract*/
              ++acc_float_size;					/* sign */
  	      asn1r_wr_u8 (aCtx, acc_float_size);
              }
            else
              asn1r_wr_u8 (aCtx, (ST_UCHAR)(type_len & 0xFF));		/* write fract	*/

	    asn1r_fin_prim (aCtx, INT_CODE,UNI);
	    asn1r_fin_constr (aCtx, (ST_UINT16) type, CTX, 0);
	    break;

	  default :
            S_UNLOCK_COMMON_RESOURCES ();
     	    return (err_rtn (MVE_TYPEDEF_BADTYPE,tspec));
	  }
	type_len = 0;
	number = SD_FALSE;
	break;

      case ':' :
	if (literal)
	  break;
	if (number || m_packed)
          {
          S_UNLOCK_COMMON_RESOURCES ();
	  return (err_rtn (MVE_TYPEDEF_SYM_COLON,tspec));
          }
	tspec [i] = '\0';
	num_elts = -1L;
	break;

      case ']' :
	if (number || m_packed)
          {
          S_UNLOCK_COMMON_RESOURCES ();
	  return (err_rtn (MVE_TYPEDEF_SYM_RBRACKET,tspec));
          }
	asn1r_strt_constr (aCtx);
	asn1r_strt_constr (aCtx);
	tspec [i] = '\0';
	break;

      case '[' :
	if (!number || !num_elts)
          {
          S_UNLOCK_COMMON_RESOURCES ();
	  return (err_rtn (MVE_TYPEDEF_SYM_LBRACKET,tspec));
          }
	asn1r_fin_constr (aCtx, 2,CTX,0);
	if (m_packed)
	  num_elts = atol (strchr(&tspec[i+1],'p') + 1);
	else
	  num_elts = atol (&tspec[i+1]);
	if (num_elts < 1L)
          {
          S_UNLOCK_COMMON_RESOURCES ();
	  return (err_rtn (MVE_TYPEDEF_NUM_ELTS,tspec));
          }
	asn1r_wr_i32 (aCtx, num_elts);
	asn1r_fin_prim (aCtx, 1, CTX);
	if (m_packed)
	  {
	  asn1r_wr_i16 (aCtx, 1);
	  asn1r_fin_prim (aCtx, PACKED,CTX);
	  }
	asn1r_fin_constr (aCtx, 1, CTX, 0);
	num_elts = 0L;
	number = SD_FALSE;
	m_packed = SD_FALSE;
	break;

      case ' ' :
      case '\t' :
      case '\n' :
	break;

      default :
	if (literal)
	  break;
	if (tspec[i] >= '0' && tspec[i] <= '9')
	  {
	  if (m_packed)
            {
            S_UNLOCK_COMMON_RESOURCES ();
	    return (err_rtn (MVE_TYPEDEF_SYM_DIGIT,tspec));
            }
	  number = SD_TRUE;
	  break;
	  }
	else
          {
          S_UNLOCK_COMMON_RESOURCES ();
	  return (err_rtn (MVE_TYPEDEF_SYM_OTHER,tspec));
          }
      }
    }				/* end of while loop		*/

  
  chk_free (tspec);
  if (aCtx->asn1r_encode_overrun)
    {
    mms_op_err = ME_ASN1_ENCODE_OVERRUN;
    S_UNLOCK_COMMON_RESOURCES ();
    return (NULL);
    }
  S_UNLOCK_COMMON_RESOURCES ();
  MLOG_ENC1 ("ASN.1 type def encoded from TDL=%s", type_spec);
  MLOG_ENCH ((ST_INT) ASN1_ENC_LEN(aCtx), ASN1_ENC_PTR(aCtx));
  *asn1_len = (ST_INT)  ASN1_ENC_LEN(aCtx);
  return (ASN1_ENC_PTR(aCtx));
  }

/************************************************************************/
/*			      type_subst				*/
/*  Function to pre-processes type definition strings.  It substitutes 	*/
/*  primitive syntax for certain abbreviations defined in the 'abbrev' 	*/
/*  array.  The syntax of the resulting string is able to be parsed 	*/
/*  simply by ms_mk_asn1_type.  This function returns the length of the */
/*  destination string.							*/
/************************************************************************/

static ST_INT type_subst (ST_CHAR *dest_str, ST_CHAR *src_str, ST_INT src_len)
  {
ST_BOOLEAN literal1, literal2;	/* booleans indicating we're in a type	*/
      			/* name or component name literal string*/
ST_INT i, j, k, n;		/* indexes and counters			*/
ST_BOOLEAN found;			/* boolean indicating a string starting	*/
				/* with a capital letter matches an	*/
				/* abbreviation and is to be replaced	*/

  literal1 = literal2 = SD_FALSE;
  i = j = 0;

  while (i < src_len)		/* step through the source string	*/
    {
    if (literal1)
      {
      if (src_str[i] == '>')	/* Keep track of when a type name	*/
	literal1 = SD_FALSE;	/* (delimited by '<' and '>') is	*/
      }				/* beginning and ending.  Abbreviations	*/
    else if (literal2)		/* are not substituted inside type	*/
      {				/* names.  Ditto for component names	*/
      if (src_str[i] == ')')	/* (delimited by '(' and ')').		*/
	literal2 = SD_FALSE;
      }
    else if (src_str[i] == '<')
      literal1 = SD_TRUE;
    else if (src_str[i] == '(')
      literal2 = SD_TRUE;
    else if (src_str[i] >= 'A' && src_str[i] <= 'z')
      {
      found = SD_FALSE;
      for (n = 0; n < numTdlTypes; n++)
	{
	if (strnicmp (tdlTypes[n].abbrev, &src_str[i], strlen (tdlTypes[n].abbrev)))
	  continue;
	else
	  {
	  found = SD_TRUE;
	  break;
	  }
	}
      if (found)
	{
	k = (ST_INT) strlen (tdlTypes[n].subType);
	memcpy (&dest_str[j],tdlTypes[n].subType,k);
	j += k;
	i += (ST_INT) strlen (tdlTypes[n].abbrev);
	continue;
	}
      }
    if (!isspace(src_str[i]))	/* do not copy whitespace chars	*/
      dest_str[j++] = src_str[i];
    i++;			/* ALWAYS increment src index	*/
    }
  dest_str[j] = '\0';
  return (j);
  }



/************************************************************************/
/*			   wr_object_name				*/
/*  Function to write an object name string to the ASN.1 stream 	*/
/*  according to the ObjectName production in the MMS protocol 		*/
/*  specification.  The input string takes the form:			*/
/*  string1:IDENTIFIER, where string1 is "@VMD" for a VMD-specific 	*/
/*  object,  "@AA" for an AA-specific object, or a valid IDENTIFIER 	*/
/*  naming a domain for a domain-specific object.  IDENTIFIER is the 	*/
/*  actual name of the object within the domain, VMD, or association-	*/
/*  specific environment.  Returns 0 if successful, nonzero otherwise.	*/
/************************************************************************/

static ST_RET wr_object_name (ASN1_ENC_CTXT *aCtx, ST_CHAR *objname)
  {
ST_CHAR *ptr;

  if ((ptr = strchr (objname,':')) == NULL)	/* check for colon	*/
    {
    asn1r_wr_vstr (aCtx, objname);		/* Default is VMD specific		*/
    asn1r_fin_prim (aCtx, 0,CTX);
    return (SD_SUCCESS);
    }

  *ptr = '\0';				/* replace colon with null char	*/
  ptr++;				/* "ptr" pts to string after ':'*/
  if (!strcmp (objname,"@VMD"))
    {
    asn1r_wr_vstr (aCtx, ptr);
    asn1r_fin_prim (aCtx, 0,CTX);
    }
  else if (!strcmp (objname,"@AA"))
    {
    asn1r_wr_vstr (aCtx, ptr);
    asn1r_fin_prim (aCtx, 2,CTX);
    }
  else
    {
    asn1r_strt_constr (aCtx);			/* Domain-specific when string	*/
    asn1r_wr_vstr (aCtx, ptr);			/* before ':' is anything else	*/
    asn1r_fin_prim (aCtx, VISTR_CODE,UNI);
    asn1r_wr_vstr (aCtx, objname);
    asn1r_fin_prim (aCtx, VISTR_CODE,UNI);
    asn1r_fin_constr (aCtx, 1,CTX,0);
    }    
  return (SD_SUCCESS);
  }


/************************************************************************/
/*			      err_rtn					*/
/*  Function to be called when there is an error in ms_mk_asn1_type.	*/
/************************************************************************/

static ST_UCHAR *err_rtn (ST_RET error, ST_CHAR *tspec)
  {
  mms_op_err = error;
  chk_free (tspec);
  return (NULL);
  }


/************************************************************************/
/*			    ms_asn1_to_tdl				*/
/************************************************************************/

ST_CHAR *ms_asn1_to_tdl (ST_UCHAR *asn1_ptr, ST_INT asn1_len, ST_INT max_tdl_len)
  {
ST_CHAR	*tdl_buf;
ST_INT	tdl_len;
RUNTIME_CTRL *rt_ctrl;

  S_LOCK_COMMON_RESOURCES ();

  /* Create temporary runtime type table.			*/
  /* Last arg allows biggest array "theoretically" possible to allocate.*/
  rt_ctrl = ms_runtime_create (asn1_ptr, asn1_len,
                               (UINT_MAX/sizeof(RUNTIME_TYPE))-100);
  if (!rt_ctrl)			/* if ASN.1 decode failed		*/
    {
    mms_op_err = MVE_ASN1_TO_RT;
    S_UNLOCK_COMMON_RESOURCES ();
    return (NULL);		/* quit 				*/
    }

  tdl_buf = (ST_CHAR *) chk_calloc (1, max_tdl_len);
  tdl_len = ms_runtime_to_tdl (rt_ctrl->rt_first, rt_ctrl->rt_num, tdl_buf, max_tdl_len);

  if (tdl_len == 0)
    {
    mms_op_err = MVE_ASN1_TO_RT;
    chk_free (tdl_buf);
    S_UNLOCK_COMMON_RESOURCES ();
    return (NULL);
    }

  tdl_buf = (ST_CHAR *) chk_realloc (tdl_buf, tdl_len);
  ms_runtime_destroy (rt_ctrl);	/* destroy temp rt type	*/

  S_UNLOCK_COMMON_RESOURCES ();
  return (tdl_buf);
  }

/************************************************************************/
/*			     ms_runtime_to_tdl				*/
/************************************************************************/
static ST_INT add_tdl_separator (ST_CHAR *tdl_buf, ST_INT index,
				ST_BOOLEAN member_found,
				ST_BOOLEAN named_component_found);

ST_INT ms_runtime_to_tdl (SD_CONST RUNTIME_TYPE *rt, ST_INT rt_num,
			  ST_CHAR *tdl_buf, ST_INT max_tdl_len)
  {
ST_INT			i;
ST_INT			total_size;
SD_CONST RUNTIME_TYPE *	rt_ptr;
#define	SIZE_OF_TEMP_BUF	30
ST_CHAR			temp_buf[SIZE_OF_TEMP_BUF];
ST_INT			temp_size;	/* len of string in temp_buf	*/
ST_BOOLEAN		overrun_detected;
SD_CONST ST_CHAR *	comp_name;	/* component name	*/
ST_INT			comp_name_len;	/* len of component name*/

struct	struct_tracking {
  ST_BOOLEAN	member_found;
  };
struct struct_tracking	nest_level[ASN1_MAX_LEVEL];
ST_BOOLEAN named_component_found;

ST_INT 	cur_nest_level;

  for (i = 0; i < ASN1_MAX_LEVEL; i++)
    {
    nest_level[i].member_found = SD_FALSE;
    }

  i=0;
  overrun_detected = SD_FALSE;
  rt_ptr = rt;
  total_size = 0;
  cur_nest_level = 0;
  while ((i < rt_num) && (overrun_detected == SD_FALSE))
    {
    named_component_found = SD_FALSE;	/* assume comp name not found	*/
    if (i == 0)
      comp_name = NULL;	/* DO NOT include comp_name for first RUNTIME_TYPE*/
    else
      comp_name = ms_comp_name_find (rt_ptr);
    if (comp_name != NULL  &&  comp_name[0] != 0)
      {
      comp_name_len = (ST_INT) strlen (comp_name);

      /* Make sure there's room for comp_name + separator + '(' + ')'.	*/
      if ((total_size + comp_name_len + 3) < max_tdl_len)
        {
        /* CRITICAL: named_component_found is not yet set for this call.*/
        total_size += add_tdl_separator (tdl_buf, total_size,
                      nest_level[cur_nest_level].member_found,
                      named_component_found);
        strcpy (&tdl_buf[total_size], "(");
        total_size += 1;
        strcpy (&tdl_buf[total_size],comp_name);
        total_size += comp_name_len;
        strcpy (&tdl_buf[total_size], ")");
        total_size += 1;
        }
      else
        {
        overrun_detected = SD_TRUE;
        }
      named_component_found = SD_TRUE;
      }

    if (rt_ptr->el_tag == RT_ARR_END || rt_ptr->el_tag == RT_STR_END)
      nest_level[cur_nest_level].member_found = SD_FALSE;
    else
      {
      if (total_size + 1 < max_tdl_len)
	{
	total_size += add_tdl_separator (tdl_buf, total_size,
		      nest_level[cur_nest_level].member_found,
		      named_component_found);
	}
      else
	overrun_detected = SD_TRUE;
      nest_level[cur_nest_level].member_found = SD_TRUE;
      }
      
    temp_buf[0] = '\0';		/* start with NULL string	*/
    switch (rt_ptr->el_tag)
      {
      case RT_ARR_START	  :
        sprintf (temp_buf, "[%d:", (ST_INT)rt_ptr->u.arr.num_elmnts);
        cur_nest_level++;
        break;

      case RT_STR_START	  :
        strcpy (temp_buf, "{");
        cur_nest_level++;
        break;

      case RT_BOOL	  :	  
        strcpy (temp_buf, "Bool");
        break;

      case RT_BIT_STRING  :	  
        if (rt_ptr->u.p.el_len < 0)
          sprintf (temp_buf, "BVstring%d", abs (rt_ptr->u.p.el_len));
        else
          sprintf (temp_buf, "Bstring%d", rt_ptr->u.p.el_len);
        break;

      case RT_INTEGER	  :
        switch (rt_ptr->u.p.el_len)
          {
          case 1:
            strcpy (temp_buf, "Byte");
            break;
          case 2:
            strcpy (temp_buf, "Short");
            break;
          case 4:
            strcpy (temp_buf, "Long");
            break;
#ifdef INT64_SUPPORT
          case 8:
            strcpy (temp_buf, "Int64");
            break;
#endif
          }
        break;

      case RT_UNSIGNED	  :
        switch (rt_ptr->u.p.el_len)
          {
          case 1:
            strcpy (temp_buf, "Ubyte");
            break;
          case 2:
            strcpy (temp_buf, "Ushort");
            break;
          case 4:
            strcpy (temp_buf, "Ulong");
            break;
#ifdef INT64_SUPPORT
          case 8:
            strcpy (temp_buf, "Uint64");
            break;
#endif
          }
        break;

      case RT_FLOATING_POINT :
        switch (rt_ptr->u.p.el_len)
          {
          case 4:
            strcpy (temp_buf, "Float");
            break;
          case 8:
            strcpy (temp_buf, "Double");
            break;
          }
        break;

      case RT_OCTET_STRING   :	  
        if (rt_ptr->u.p.el_len < 0)
          sprintf (temp_buf, "OVstring%d", abs (rt_ptr->u.p.el_len));
        else
          sprintf (temp_buf, "Ostring%d", rt_ptr->u.p.el_len);
        break;

      case RT_VISIBLE_STRING :
        if (rt_ptr->u.p.el_len < 0)
          sprintf (temp_buf, "Vstring%d", abs (rt_ptr->u.p.el_len));
        else
          sprintf (temp_buf, "Fstring%d", rt_ptr->u.p.el_len);
        break;

      case RT_GENERAL_TIME   :	  
        strcpy (temp_buf, "Gtime");
        break;

      case RT_BINARY_TIME    :	  
        sprintf (temp_buf, "Btime%d", rt_ptr->u.p.el_len);
        break;

      case RT_BCD	     :	  
        sprintf (temp_buf, "Bcd%d", (ST_INT)rt_ptr->u.p.el_len);
        break;

      case RT_BOOLEANARRAY   :	  
        break;

      case RT_UTC_TIME:	  
        strcpy (temp_buf, "Utctime");
        break;

      case RT_UTF8_STRING:	  
        if (rt_ptr->u.p.el_len < 0)
          sprintf (temp_buf, "UTF8Vstring%d", (ST_INT) abs (rt_ptr->u.p.el_len));
        else
          sprintf (temp_buf, "UTF8string%d", (ST_INT) (rt_ptr->u.p.el_len));
        break;

      case RT_STR_END 	     :
        strcpy (temp_buf, "}");
        cur_nest_level--;
        break;

      case RT_ARR_END 	     :
        strcpy (temp_buf, "]");
        cur_nest_level--;
        break;
      }


    temp_size = (ST_INT) strlen (temp_buf);
    if (temp_size == 0)
      {			/* If buffer empty, rt_ptr contains unrecognized type*/
      /* DEBUG: this should NEVER happen. Chk for memory corruption.	*/
      MLOG_ERR1 ("RUNTIME_TYPE contains illegal el_tag = %d", rt_ptr->el_tag);
      return (0);	/* error	*/
      }
    if (total_size + temp_size < max_tdl_len)
      {
      strcpy (&tdl_buf[total_size], temp_buf);
      total_size += temp_size;
      }
    else
      overrun_detected = SD_TRUE;

    rt_ptr++;				/* point to next runtime element*/
    i++;
    }		/* end main loop*/

  if (overrun_detected == SD_TRUE)
    {
    MLOG_NERR1 ("ms_runtime_to_tdl: tdl buffer overrun (max=%d)", max_tdl_len);
    total_size = 0;
    }
  else
    {
    tdl_buf[total_size] = 0;		/* ensure a Null terminator 	*/
    total_size++;			/* account for a Null terminator*/
    }

  return (total_size);
  }

/************************************************************************/
/*                          add_tdl_separator				*/
/************************************************************************/

static ST_INT add_tdl_separator (ST_CHAR *tdl_buf, ST_INT index,
				ST_BOOLEAN member_found,
				ST_BOOLEAN named_component_found)
  {
  if ((member_found == SD_TRUE) &&
       (named_component_found == SD_FALSE))
    {
    strcpy (&tdl_buf[index], ",");
    return (1);
    }
  else
    {
    return (0);
    }
  }

/************************************************************************/
/*			ms_tdl_to_runtime				*/
/* This function converts TDL to ASN.1 and then ASN.1 to RUNTIME_TYPE.	*/
/* NOTE: this function allocates array of RUNTIME_TYPE structs by	*/
/*       calling "ms_runtime_create". The array must be freed by	*/
/*       calling "ms_runtime_destroy" (DO NOT use M_FREE or chk_free).	*/
/************************************************************************/
RUNTIME_CTRL *ms_tdl_to_runtime (ST_CHAR *tdl,	/* TDL string	*/
			ST_UCHAR *asn1_buf,	/* ASN.1 buffer	*/
			ST_INT asn1_buf_len)	/* buffer len	*/
  {
ST_UCHAR *asn1_start;		/* ptr to ASN.1 encoded type	*/
RUNTIME_CTRL *rt_ctrl = NULL;	/* assume failure	*/

  /* NOTE: ms_mk_.. overwrites asn1_buf_len with len of ASN.1 encoding	*/
  asn1_start = ms_mk_asn1_type (&asn1_buf_len, asn1_buf, tdl);
  if (asn1_start)				/* successful	*/
    rt_ctrl = ms_runtime_create (asn1_start, asn1_buf_len,
                     (UINT_MAX/sizeof(RUNTIME_TYPE))-100);
  return (rt_ctrl);
  }
