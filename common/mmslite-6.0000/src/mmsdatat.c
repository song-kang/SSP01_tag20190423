/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1997 - 2010, All Rights Reserved.			*/
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : mmsdatat.c						*/
/* PRODUCT(S)  : none							*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the functions employed by the virtual	*/
/*	machine for parsing an ASN.1 data string and creating the	*/
/*	run-time type specification from it.				*/
/*	Adapted from mmsdata.c						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 11/23/11  KCR           Added casting for proper 64bit compile       */
/* 02/25/10  JRB	   Del globals & use aCtx->usr_info.		*/
/*			   Now thread safe so delete S_LOCK...		*/
/* 03/17/08  JRB     19    Del m_use_long_ints flag & assume smallest	*/
/*			   integer is 4 bytes.				*/
/* 02/19/07  JRB     18    Add error log message.			*/
/* 03/22/06  EJV     17    Changed RT_GENERAL_TIME st_size and el_len to*/
/*			     sizeof(time_t) (VS 2005 has 64-bit time_t).*/
/* 01/30/06  GLB     16    Integrated porting changes for VMS           */
/* 10/27/03  JRB     15    Assume all UTF8strings are variable-length.	*/
/*			   Add code to adjust len for array of UTF8str..*/
/* 10/16/03  MDE     14    Fixed leak for data conversion error		*/
/* 05/08/03  JRB     13    Fix TAG_ADD for "array" of RT_UTF8_STRING.	*/
/* 04/02/03  JRB     12    Add UTF8string support (see RT_UTF8_STRING).	*/
/* 12/20/01  JRB     11    Converted to use ASN1R (re-entrant ASN1)	*/
/* 11/15/01  EJV     10    Added support for new MMS type UtcTime.	*/
/* 10/06/00  MDE     09    Now set asn1_decode_method			*/
/* 04/20/00  JRB     08    Undo last chg, fix mms_vvar.h instead. Lint.	*/
/* 10/13/99  RKR     07    Added SD_CONST to function headers		*/
/* 09/13/99  MDE     06    Added SD_CONST modifiers			*/
/* 06/03/98  MDE     05    Allow for 0 length variable length items	*/
/* 03/12/98  MDE     04    Fixed 'loops' change - whoops!		*/
/* 02/10/98  MDE     03    No longer use runtime type 'loops' element	*/
/* 08/15/97  MDE     02    BTOD handling changes			*/
/* 07/03/97  MDE     01    Fixed problem w/structures			*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#include "glbtypes.h"

#include "sysincs.h"
#include "glbsem.h"

#include "mmsdefs.h"
#include "mms_pvar.h"
#include "mms_vvar.h"
#include "asn1defs.h"
#include "mem_chk.h"

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/

static ST_VOID assign_tags (ASN1_DEC_CTXT *aCtx);
static int init_new_RT (ASN1_DEC_CTXT *aCtx, ST_UCHAR tag, ST_INT el_len);
#if 0 	/* Constructed bitstrings and octetstrings not supported 	*/
static ST_VOID get_bstr_cstr   (ASN1_DEC_CTXT *aCtx); 	/* for constructed bitstrings	*/
static ST_VOID get_ostr_cstr   (ASN1_DEC_CTXT *aCtx); 	/* for constructed octetstrings */
static ST_VOID bs_cstr_done    (ASN1_DEC_CTXT *aCtx);
static ST_VOID os_cstr_done    (ASN1_DEC_CTXT *aCtx);
#endif
static ST_VOID get_bool_prim   (ASN1_DEC_CTXT *aCtx); 	/* for boolean primitives	*/
static ST_VOID get_bitstr_prim (ASN1_DEC_CTXT *aCtx); 	/* for bitstring primitives	*/
static ST_VOID get_int_prim    (ASN1_DEC_CTXT *aCtx); 	/* for integer primitives	*/
static ST_VOID get_uint_prim   (ASN1_DEC_CTXT *aCtx); 	/* for unsigned integers	*/
static ST_VOID get_float_prim  (ASN1_DEC_CTXT *aCtx); 	/* for float primitives 	*/
static ST_VOID get_octstr_prim (ASN1_DEC_CTXT *aCtx); 	/* for octet string primitives	*/
static ST_VOID get_vstr_prim   (ASN1_DEC_CTXT *aCtx); 	/* for visible string primitives*/
static ST_VOID get_gtime_prim  (ASN1_DEC_CTXT *aCtx); 	/* for generalized time 	*/
static ST_VOID get_btime_prim  (ASN1_DEC_CTXT *aCtx); 	/* for binary time primitives	*/
static ST_VOID get_bcd_prim    (ASN1_DEC_CTXT *aCtx); 	/* for binary coded decimal	*/
static ST_VOID get_utc_time_prim  (ASN1_DEC_CTXT *aCtx); 	/* for UTC time primitives	*/
static ST_VOID get_utf8_prim  (ASN1_DEC_CTXT *aCtx); 	/* for UTF8string primitives	*/
static ST_VOID arr_start	    (ASN1_DEC_CTXT *aCtx);
static ST_VOID str_start	    (ASN1_DEC_CTXT *aCtx);
static ST_VOID arr_done_fun    (ASN1_DEC_CTXT *aCtx);
static ST_VOID str_done_fun    (ASN1_DEC_CTXT *aCtx);
static ST_VOID err_clean_fun   (ASN1_DEC_CTXT *aCtx, ST_RET err_code);


typedef enum { UNDEF, STRUCT, ARRAY } ConstrType; /* Constructor types */

/* User Info needed by ASN.1 decoder.	*/
typedef struct
  {
  RUNTIME_TYPE *rt_base;	/* Base ptr for runtime array	*/
  RUNTIME_TYPE *cur_rt;		/* Current runtime element ptr	*/
  RUNTIME_TYPE *nxt_un_rt;	/* Next unused RT el in buffer	*/
  RUNTIME_TYPE *rt_EOB_ptr;	/* One past end of RT buffer.	*/
  int local_alloc_flag;
  /* Block pointer for arrs & structs */
  RUNTIME_TYPE *rt_blk_start[ASN1_MAX_LEVEL];
  ConstrType constr_type[ASN1_MAX_LEVEL];	/* Constructor type	*/

  ST_INT arr_loop_level;
  ST_INT arr_loops [ASN1_MAX_LEVEL];
  } USR_INFO;

/* The following defines specify the various error codes that are used	*/
/* in ms_asn1_to_locl and its subordinate functions.  These are in ad-	*/
/* dition to the error codes that can be generated by ASN1DE internally,*/
/* and both translate into return values that are one greater than the	*/
/* error codes themselves.						*/

#define 	BAD_DATA	100	/* Data not consistent with	*/
					/* type specification.		*/
#define 	INTERNAL	101	/* Type specif'n bad, not data. */

/************************************************************************/
/************************************************************************/

ST_RET ms_asn1_data_to_locl (ST_UCHAR *asn1_data,	   
		  	    ST_INT asn1_data_len, 
			    ST_VOID **data_dest,	   
			    ST_INT *data_dest_len,
			    RUNTIME_TYPE **rt,     
			    ST_INT *t_len)	   
  {
ST_RET rc;
RUNTIME_TYPE *rt_in;

			/* First, build runtime from data string */
  rt_in = *rt;
  rc = ms_asn1_data_to_runtime (rt, t_len, asn1_data, asn1_data_len);
  if (rc != SD_SUCCESS)
    return (rc);

/* verify that the supplied data buffer is big enough			*/
  if (*data_dest && *data_dest_len < (*rt)->offset_to_last)
    {
    if (!rt_in)		/* RT table was chk_calloc'd			*/
      chk_free (*rt);

    return (MVE_DATA_SPACE);
    }

  if (!*data_dest)
    {
    *data_dest_len = (*rt)->offset_to_last;

  /* Variable length items can have a data length of 0 */
    if (*data_dest_len)
      {
      *data_dest = chk_calloc (1, (*rt)->offset_to_last);

		/* OK, go ahead and do the data conversion	 	*/
      rc = ms_asn1_to_local (*rt, *t_len, asn1_data, asn1_data_len, (ST_CHAR *) *data_dest);
      if (rc)
        {
        chk_free (*data_dest);
        if (!rt_in)		/* RT table was chk_calloc'd			*/
          chk_free (*rt);
	}
      }
    else
      *data_dest = chk_calloc (1, 1);
    }
  else		/* OK, go ahead and do the data conversion		*/
    {

    rc = ms_asn1_to_local (*rt, *t_len, asn1_data, asn1_data_len, (ST_CHAR *) *data_dest);
    if (rc && !rt_in)	/* Error and RT table was chk_calloc'd			*/
      chk_free (*rt);
    }
  return (rc);
  }
	

/************************************************************************/
/*			ms_asn1_data_to_runtime				*/
/* Creates a run-time data defn table from the input ASN.1 data string. */
/* Input parameters:							*/
/* RUNTIME_TYPE **tptr	Pointer^2 to runtime buffer.			*/
/* ST_INT *t_len	Length of runtime buffer, in runtime elements.	*/
/* ST_UCHAR *asn1ptr	Pointer to input ASN.1 string			*/
/* ST_INT asn1len	Length of ASN.1 string, in bytes.		*/
/* Returns:	SD_SUCCESS, SD_FAILURE or INTERNAL.  If SD_FAILURE, aCtx->asn1r_pdu_dec_err	*/
/*		has the specific error code.				*/
/* USR_INFO members set: rt_base, rt_EOB_ptr, cur_rt, nxt_un_rt		*/
/************************************************************************/


ST_RET ms_asn1_data_to_runtime (RUNTIME_TYPE **tptr,
			ST_INT *t_len,
			ST_UCHAR *asn1ptr,
			ST_INT asn1_len)
  {
ST_RET ret;
ASN1_DEC_CTXT localDecCtx;	/* For readability, use "aCtx" to access this.*/
ASN1_DEC_CTXT *aCtx = &localDecCtx;
USR_INFO usr;

  memset (aCtx, 0, sizeof (ASN1_DEC_CTXT));	/* CRITICAL: start clean.	*/
  memset (&usr, 0, sizeof (usr));	/* CRITICAL: start clean.	*/
  /* NOTE: after memset,		*/
  /*  usr.arr_loop_level=0		*/
  /*  usr.local_alloc_flag = SD_FALSE	*/

  if (tptr == NULL)		/* What?  Parameter error, fix that. */
    {
    return (MVE_RT_TYPE);
    }

  if (*tptr != NULL)
    {				/* Type buffer already allocated, use it. */
    usr.rt_base = *tptr;		/* Init pointer to runtime buffer. */
    if (*t_len <= 0)		/* Check size. */
      {
      return (MVE_RT_TYPE);		/* Parameter error. */
      }
    else
      usr.rt_EOB_ptr = usr.rt_base + *t_len;	/* Init end-of-buffer pointer */
    }
  else				/* Allocate a default-size buffer here. */
    {
    usr.rt_base = (RUNTIME_TYPE *) chk_calloc (m_rt_type_limit, sizeof (RUNTIME_TYPE));
    usr.rt_EOB_ptr = usr.rt_base + m_rt_type_limit;
    usr.local_alloc_flag = SD_TRUE;
    }

  usr.cur_rt = usr.nxt_un_rt = usr.rt_base;
  usr.rt_blk_start[0] = usr.rt_base;	/* Starting block for level 0 is first block. */
  usr.constr_type[0] = UNDEF;	/* Constructed type initializes to UNDEF. */
			/* These variables are used by the tag routines */
			/* to build the runtime type table. */

  aCtx->asn1r_decode_method = ASN1_TAG_METHOD;

  /* Pass "usr" through "aCtx->usr_info [0]".			*/
  /* CRITICAL: Must be BEFORE "assign_tags".			*/
  aCtx->usr_info [0] = &usr;

  assign_tags (aCtx);		/* Assign tags.  First type can be anything. */

  aCtx->asn1r_decode_done_fun = NULL;	
  aCtx->asn1r_err_fun = err_clean_fun;

  asn1r_decode_asn1 (aCtx, asn1ptr, asn1_len);  /* Build the type table. */

  *tptr = usr.rt_base;		/* Local buffer de-allocated on error. */
  if (usr.rt_base != NULL)		/* Check for error to prevent doing */
    {				/* arithmetic with NULL pointers. */
    *t_len = (int) (usr.nxt_un_rt - usr.rt_base);
    ms_rt_size_calc (usr.rt_base, *t_len); /* Fix up element offsets in table */
    }
  else
    *t_len = 0;

  ret =  (aCtx->asn1r_pdu_dec_err == NO_DECODE_ERR) ? SD_SUCCESS : MVE_DATA_TO_RT ;
  return (ret);
  }

/************************************************************************/
/*			     assign_tags				*/
/* Function to assign tags to the ASN1DE tools based on what's expected */
/* in the runtime_type definition table.  At the beginning of the 'Data'*/
/* entity, a DataAccessError type is allowed.  WARNING: This function	*/
/* requires that the current rt_block NOT be an end-of-array or end-of- */
/* structure rt_block.	If so, it will return an error. 		*/
/************************************************************************/

static ST_VOID assign_tags (ASN1_DEC_CTXT *aCtx)
  {	/* There are two possible cases here:				*/
	/* 1.)  We are parsing through terra incognita and don't know	*/
	/*	what to expect (yet).  All possible tags are legal.	*/
	/*	Set up to build type definitions for what we find.	*/
	/* 2.)	We are parsing through the 2nd or later element of an	*/
	/*	array and know what tag we should get next.  If the	*/
	/*	array is a sub-array of an enveloping array, we will	*/
	/*	have an element count for it; check that too.  Verify	*/
	/*	tags against what we expect to see.			*/
USR_INFO *usr = (USR_INFO *) aCtx->usr_info [0];
  if (usr->cur_rt < usr->nxt_un_rt)	/* if this is the 2nd or later element	*/
    {				/* of an array & types are defined, 	*/
    if (usr->cur_rt->el_tag == RT_ARR_END)	/* treat case of array ending */
      {			/* Only possible if we've seen this array before as */
      			/* an element of an enveloping array, otherwise	*/
			/* there would be no RT_ARR_END runtime record yet. */
      if (--usr->arr_loops[usr->arr_loop_level] > 0) /* if need to do next ar elmnt */
        usr->cur_rt -= usr->cur_rt->u.arr.num_rt_blks;
      }					/* mv cur_rt to start of arr */

    switch (usr->cur_rt->el_tag)
      {
      case RT_ARR_START :
	ASN1R_TAG_ADD (aCtx, CTX|CONSTR,1,arr_start); /* expecting an array cstr	*/
      break;

      case RT_STR_START :
	ASN1R_TAG_ADD (aCtx, CTX|CONSTR,2,str_start); /* expecting a struct cstr	*/
      break;

      case RT_BOOL :
	ASN1R_TAG_ADD (aCtx, CTX,3,get_bool_prim);	
      break;

      case RT_BIT_STRING :			
	ASN1R_TAG_ADD (aCtx, CTX,4,get_bitstr_prim);
#if 0 	/* Constructed bitstrings and octetstrings not supported 	*/
	ASN1R_TAG_ADD (aCtx, CTX|CONSTR,4,get_bstr_cstr); 
#endif
      break;

      case RT_INTEGER :			
	ASN1R_TAG_ADD (aCtx, CTX,5,get_int_prim);
      break;

      case RT_UNSIGNED : 			
	ASN1R_TAG_ADD (aCtx, CTX,6,get_uint_prim);
      break;

#ifdef FLOAT_DATA_SUPPORT
      case RT_FLOATING_POINT : 		
	ASN1R_TAG_ADD (aCtx, CTX,7,get_float_prim);
      break;
#endif

      case RT_OCTET_STRING : 		
	ASN1R_TAG_ADD (aCtx, CTX,9,get_octstr_prim);
#if 0 	/* Constructed bitstrings and octetstrings not supported 	*/
	ASN1R_TAG_ADD (aCtx, CTX|CONSTR,9,get_ostr_cstr); 
#endif
      break;

      case RT_VISIBLE_STRING : 		
	ASN1R_TAG_ADD (aCtx, CTX,10,get_vstr_prim);
      break;

#ifdef TIME_DATA_SUPPORT
      case RT_GENERAL_TIME : 		
	ASN1R_TAG_ADD (aCtx, CTX,11,get_gtime_prim);
      break;
#endif

#ifdef BTOD_DATA_SUPPORT
      case RT_BINARY_TIME : 		
	ASN1R_TAG_ADD (aCtx, CTX,12,get_btime_prim);
      break;
#endif

      case RT_BCD : 			
	ASN1R_TAG_ADD (aCtx, CTX,13,get_bcd_prim);
      break;

      case RT_UTC_TIME : 		
	ASN1R_TAG_ADD (aCtx, CTX,17,get_utc_time_prim);
      break;

      case RT_UTF8_STRING : 		
	ASN1R_TAG_ADD (aCtx, CTX,RT_UTF8_STRING,get_utf8_prim);
      break;

      case RT_ARR_END :			/* array done			*/
	aCtx->asn1r_c_done_fun[aCtx->asn1r_msg_level] = arr_done_fun; /* the arr cstr must be done*/
      break;

      case RT_STR_END :			/* structure done		*/
	aCtx->asn1r_c_done_fun[aCtx->asn1r_msg_level] = str_done_fun; /* the str cstr must be done*/
      break;

      default : 			/* should not be any other tag	*/
        MLOG_ERR0 ("Bad tag in runtime type");
	asn1r_set_dec_err (aCtx, INTERNAL);
	return;
      break;
      }
    }
  else			/* In terra incognita, next element can be anything */
    {
    /* Allow for any type of data in the data string. */
    ASN1R_TAG_ADD (aCtx, CTX|CONSTR,1,arr_start);
    ASN1R_TAG_ADD (aCtx, CTX|CONSTR,2,str_start);
    ASN1R_TAG_ADD (aCtx, CTX,3,get_bool_prim);	
    ASN1R_TAG_ADD (aCtx, CTX,4,get_bitstr_prim);
#if 0 	/* Constructed bitstrings and octetstrings not supported 	*/
    ASN1R_TAG_ADD (aCtx, CTX|CONSTR,4,get_bstr_cstr);
#endif
    ASN1R_TAG_ADD (aCtx, CTX,5,get_int_prim);
    ASN1R_TAG_ADD (aCtx, CTX,6,get_uint_prim);
    ASN1R_TAG_ADD (aCtx, CTX,7,get_float_prim);
    ASN1R_TAG_ADD (aCtx, CTX,9,get_octstr_prim);
#if 0 	/* Constructed bitstrings and octetstrings not supported 	*/
    ASN1R_TAG_ADD (aCtx, CTX|CONSTR,9,get_ostr_cstr);
#endif
    ASN1R_TAG_ADD (aCtx, CTX,10,get_vstr_prim);
    ASN1R_TAG_ADD (aCtx, CTX,11,get_gtime_prim);
    ASN1R_TAG_ADD (aCtx, CTX,12,get_btime_prim);
    ASN1R_TAG_ADD (aCtx, CTX,13,get_bcd_prim);
    ASN1R_TAG_ADD (aCtx, CTX,17,get_utc_time_prim);
    ASN1R_TAG_ADD (aCtx, CTX,RT_UTF8_STRING,get_utf8_prim);
    }
  }



/************************************************************************/
/*			   init_new_RT					*/
/* This is the function called to check to see if a new RT table entry	*/
/* is required and available, and initializes it if so.			*/
/* Parameters:								*/
/*	tag	Tag value for the field type.				*/
/*	el_len	Element length, in its own units.  0 for constructed	*/
/*		types.  Units are bytes except for bitstrings (length	*/
/*		given in bits).						*/
/* USR_INFO members used as input:					*/
/*	cur_rt, nxt_un_rt, rt_EOB_ptr					*/
/* USR_INFO members used as output:					*/
/*	nxt_un_rt							*/
/* Returns:								*/
/*	0 if OK, 1 if error (table overflow).				*/
/************************************************************************/

static int init_new_RT (ASN1_DEC_CTXT *aCtx, ST_UCHAR tag, ST_INT el_len)
  {
ST_BOOLEAN prim;
ST_INT st_size;
USR_INFO *usr = (USR_INFO *) aCtx->usr_info [0];

  prim = SD_TRUE;
  switch (tag)		/* Calc storage size based on element fundamentals */
    {
    case RT_BOOL:			/* Size must be 1, so ignore it. */
      st_size = 1;
    break;

    case RT_BIT_STRING:			/* el_len was converted to bits */
      st_size = CALC_BIT_LEN (el_len);	/* Calculate byte count. */
    break;

    case RT_INTEGER:			/* Size is in bytes. */
    case RT_UNSIGNED:
      st_size = el_len;			/* already conditioned (1,2,4)	*/
    break;

    case RT_BCD:			/* always put into ST_INT32		*/
      st_size = 4;
      el_len = 8;			/* assume type is 8 BCD digits	*/
    break;

    case RT_FLOATING_POINT:		
	/* Decrement el_len because it's used for storage size later */
      st_size = (--el_len == 4) ? sizeof (float) : sizeof (double);
    break;

    case RT_OCTET_STRING:		/* Size is in bytes. */
      st_size = el_len;			/* No terminating null */
    break;				/* If variable length, size will */
					/* be increased by sizeof (ST_INT16) */

    case RT_VISIBLE_STRING:		/* Size is in bytes. */
      st_size = el_len+1;		/* Add space for terminating null */
    break;				/* No increase for variable length */

    case RT_GENERAL_TIME:		/* Generalized time */
      st_size = sizeof (time_t);
      el_len = sizeof (time_t);
    break;

    case RT_BINARY_TIME:		/* Binary time */
      if (el_len == 6)
        st_size = 2 * sizeof (ST_INT32);
      else
        st_size = sizeof (ST_INT32);
    break;

    case RT_UTC_TIME:			/* UTC time */
      st_size = sizeof (MMS_UTC_TIME);	/* 3 * sizeof (ST_UINT32);	*/
    break;

    case RT_UTF8_STRING:		/* Unicode UTF8string */
#if (UNICODE_LOCAL_FORMAT==UNICODE_UTF8)
      st_size = abs(el_len)*4 + 1;	/* # bytes (4 bytes per char + NULL)*/
#else		/* assume UNICODE_UTF16	*/
      st_size = abs(el_len)*2 + 2;	/* # bytes (2 bytes per char + 2 byte NULL)*/
#endif
    break;

    default:
      prim = SD_FALSE;			/* Not a primitive element. */
      st_size = 0;
    break;
    }

  if (usr->cur_rt == usr->nxt_un_rt)	/* If this element is not already defined, */
    {				/* We need to use another el, do ovrfl check */
    if (usr->nxt_un_rt == usr->rt_EOB_ptr) /* Check against EOB pointer. */
      {
      asn1r_set_dec_err (aCtx, MX_MEMORY_ALLOC); /* Table is full, declare error & return */
      MLOG_ERR1 ("Exceeded %d limit on number of elements in RUNTIME_TYPE array", usr->rt_EOB_ptr - usr->rt_base);
      return SD_FAILURE;		/* Error value. */
      }
    ++usr->nxt_un_rt;			/* Used one more element. */
    				/* Clear the new element. */
    memset ((char *)usr->cur_rt, 0, sizeof (RUNTIME_TYPE));
    usr->cur_rt->el_tag = tag;	/* Set the tag value. */
    usr->cur_rt->el_size = st_size;	/* Set storage size */
    if (prim)
      {
      if (tag == RT_BCD)
        usr->cur_rt->u.p.el_len = 8;
      else
        usr->cur_rt->u.p.el_len = el_len; /* Set initial data size for prim */
      }
    if (tag == RT_ARR_START)
      usr->cur_rt->u.arr.num_rt_blks = -1; /* Flag unfinished constructor */
    else if (tag == RT_STR_START)
      usr->cur_rt->u.str.num_rt_blks = -1; /* Flag unfinished constructor */

    return SD_SUCCESS;			/* New element, done. */
    }

  /* The following code executes if this is an element of an array.	*/
  /* It adjusts sizes if necessary. For example, if the second Vstring	*/
  /* in an array is longer than the first, the el_len and el_size are	*/
  /* increased to handle it.						*/
 
			/* This is an "old" element.  Check data sizes	*/
			/* and update as required.  Check for variable	*/
  switch (tag)		/* length issues in byte and bit strings.	*/
    {
    case RT_BIT_STRING:			/* Size is in bits */
      if (usr->cur_rt->u.p.el_len < 0)	/* Is variable */
        {					/* Re-size if necessary */
        if (-usr->cur_rt->u.p.el_len < el_len)  /* New one is bigger */
          {
          usr->cur_rt->u.p.el_len = -el_len;	/* Set new length */
          usr->cur_rt->el_size = st_size + sizeof (ST_INT16);
          }
        }
      else if (usr->cur_rt->u.p.el_len != el_len)	/* Different length */
        {					/* Make variable length */
        if (usr->cur_rt->u.p.el_len > el_len) /* Size for biggest seen */
          el_len = usr->cur_rt->u.p.el_len;

        usr->cur_rt->u.p.el_len = -el_len;
        usr->cur_rt->el_size = CALC_BIT_LEN (el_len) + sizeof (ST_INT16);
        }
    break;

    case RT_INTEGER:			/* Size is in bytes. */
    case RT_UNSIGNED:
      if (st_size > usr->cur_rt->el_size)	/* If this one is bigger, */
        {					/* set both storage size */
        usr->cur_rt->el_size =			/* and element length. */
          usr->cur_rt->u.p.el_len = st_size;
        }
    break;

    case RT_OCTET_STRING:		/* Size is in bytes. */
      if (usr->cur_rt->u.p.el_len < 0)	/* Variable size */
        {
        if (-usr->cur_rt->u.p.el_len < el_len)	/* New one is longer */
          {
          usr->cur_rt->u.p.el_len = -el_len;	/* Bump size up */
          usr->cur_rt->el_size = el_len + sizeof (ST_INT16);
          }
        }
      else if (usr->cur_rt->u.p.el_len != el_len)	/* Different size */
        {
        if (-usr->cur_rt->u.p.el_len > el_len)	/* If old is bigger, */
          el_len = -usr->cur_rt->u.p.el_len;	/* keep it, otherwise */
        usr->cur_rt->u.p.el_len = -el_len;		/* use this size. */
        usr->cur_rt->el_size = el_len + sizeof (ST_INT16);
        }
    break;

    case RT_VISIBLE_STRING:		/* Size is in bytes. */
      if (usr->cur_rt->u.p.el_len < 0)	/* Variable len */
        {
        if (usr->cur_rt->u.p.el_len > -el_len)  /* New one is bigger. */
          usr->cur_rt->u.p.el_len = -el_len;
        }
      else if (usr->cur_rt->u.p.el_len != el_len)	/* Length different */
        {					/* Make variable length */
        if (usr->cur_rt->u.p.el_len > el_len)
          el_len = usr->cur_rt->u.p.el_len;
        usr->cur_rt->u.p.el_len = -el_len;
        }
      usr->cur_rt->el_size = abs (usr->cur_rt->u.p.el_len) + 1;
    break;

    case RT_UTF8_STRING:		/* Unicode UTF8string */
      /* If new len > old len, force string to be "variable-len" & increase len.*/
      if (abs(el_len) > abs(usr->cur_rt->u.p.el_len))  /* New one is bigger. */
        {
        usr->cur_rt->u.p.el_len = -abs(el_len);	/* increase len	*/
      	/* adjust el_size	*/
#if (UNICODE_LOCAL_FORMAT==UNICODE_UTF8)
        usr->cur_rt->el_size = abs(usr->cur_rt->u.p.el_len)*4 + 1;	/* # bytes (4 bytes per char + NULL)*/
#else		/* assume UNICODE_UTF16	*/
        usr->cur_rt->el_size = abs(usr->cur_rt->u.p.el_len)*2 + 2;	/* # bytes (2 bytes per char + 2 byte NULL)*/
#endif
        }
    break;

    default:				/* Other types are fixed length */
    break;
    }

  return SD_SUCCESS;			/* All OK. */
  }



/************************************************************************/
/*				next_rt_index				*/
/* Sets cur_rt to the correct value for the next data element in	*/
/* the parse.  If the current message level is an unfinished array,	*/
/* it increments the u.arr.num_elmnts member of the header.		*/
/* Parameters:	None.							*/
/* USR_INFO members used as input:  cur_rt, nxt_un_rt, constr_type[]	*/
/* USR_INFO members used as output: cur_rt				*/
/* Other variables changed:  usr->cur_rt->u.arr.num_elmnts		*/
/************************************************************************/

static ST_VOID next_rt_index (ASN1_DEC_CTXT *aCtx)
  {
USR_INFO *usr = (USR_INFO *) aCtx->usr_info [0];
  if (++usr->cur_rt < usr->nxt_un_rt)	/* Next RT element already defined? */
    return;			/* Return, done. */
	/* The current message level isn't complete yet.  Check type. */
  if (usr->constr_type[aCtx->asn1r_msg_level] == ARRAY) /* This is an unfinished array. */
    {
    usr->cur_rt = usr->rt_blk_start[aCtx->asn1r_msg_level]; /* Loop back to array start */
    usr->cur_rt->u.arr.num_elmnts++; /* Inc # of elements. */
    ++usr->cur_rt;				/* Point to contents. */
    }
	/* Done.  No loopback is required for structures, only arrays. */
  }



/************************************************************************/
/*			   arr_start					*/
/* This is the function called when an array cstr is starting		*/
/************************************************************************/

static ST_VOID arr_start (ASN1_DEC_CTXT *aCtx)
  {
USR_INFO *usr = (USR_INFO *) aCtx->usr_info [0];

/* initialize the loop counter for the array				*/
  MLOG_CDEC0 ("pars_arr_start");

  if (init_new_RT (aCtx, RT_ARR_START, 0))	/* Return if error. */
    return;
		/* Check to see if the end-contructor for this array	*/
		/* has been encountered before.				*/

  ++usr->arr_loop_level;
  if (usr->cur_rt->u.arr.num_rt_blks == -1)
    {
    aCtx->asn1r_c_done_fun[aCtx->asn1r_msg_level] = arr_done_fun;	/* Init now if new array */
    }
  else	/* We've seen this array before as an element of a larger array.  We */
	/* know the size.  Initialize the loop count in the RT_ARR_END block. */
    {
    usr->arr_loops[usr->arr_loop_level] = usr->cur_rt->u.arr.num_elmnts;
    }


  usr->rt_blk_start[aCtx->asn1r_msg_level] = usr->cur_rt;	/* Point to initial element */
  usr->constr_type[aCtx->asn1r_msg_level] = ARRAY;
  usr->cur_rt++;			/* Increment index (no loopback) */
  assign_tags (aCtx);		/* Assign tags for next data element. */
  }


/************************************************************************/
/*			   arr_done_fun 				*/
/* This is the cstr done function called when an array cstr is done	*/
/* Note: aCtx->asn1r_msg_level is one less than the value given to array_start ().	*/
/************************************************************************/

static ST_VOID arr_done_fun (ASN1_DEC_CTXT *aCtx)
  {
USR_INFO *usr = (USR_INFO *) aCtx->usr_info [0];

  MLOG_CDEC0 ("pars_arr_done_fun");

  if (usr->rt_blk_start[aCtx->asn1r_msg_level+1]->u.arr.num_rt_blks == -1)
    {			  /* Unfinished array, build RT_ARR_END record. */
    usr->cur_rt = usr->nxt_un_rt;			/* Point to one past end */
    if (init_new_RT (aCtx, RT_ARR_END, 0))	/* Init a new RT block */
      return;
    
    usr->cur_rt->u.arr.num_rt_blks =  /* Calc RT block count for array. */
     usr->rt_blk_start[aCtx->asn1r_msg_level+1]->u.arr.num_rt_blks =
      (int) (usr->cur_rt - usr->rt_blk_start[aCtx->asn1r_msg_level+1] - 1);/* Copy count back to start. */

    usr->cur_rt->u.arr.num_elmnts =		/* Copy element count to */
      usr->rt_blk_start[aCtx->asn1r_msg_level+1]->u.arr.num_elmnts;/* end-array element.*/
    }
  else
    {				/* This is the end of a finished array. */
    if (usr->arr_loops[usr->arr_loop_level]) /* Loop count down to 0? */
      {
      MLOG_NERR0 ("Malformed array data");
      asn1r_set_dec_err (aCtx, BAD_DATA);	/* If not exactly 0, error. */
      }
    }
  --usr->arr_loop_level;

  next_rt_index (aCtx);
  assign_tags (aCtx);		/* setup to get next element		*/
  }

/************************************************************************/
/*			   str_start					*/
/* This is the function called when a struct cstr is starting		*/
/************************************************************************/

static ST_VOID str_start (ASN1_DEC_CTXT *aCtx)
  {
USR_INFO *usr = (USR_INFO *) aCtx->usr_info [0];

  if (init_new_RT (aCtx, RT_STR_START, 0))
    return;

  if (usr->cur_rt->u.str.num_rt_blks == -1)
    aCtx->asn1r_c_done_fun[aCtx->asn1r_msg_level] = str_done_fun;	/* Init now if new struct */

  usr->rt_blk_start[aCtx->asn1r_msg_level] = usr->cur_rt;	/* Point to initial element */
  usr->constr_type[aCtx->asn1r_msg_level] = STRUCT;
  usr->cur_rt++;			/* point to next runtime element always	*/
  assign_tags (aCtx);		/* setup to get next element		*/
  }

/************************************************************************/
/*			   str_done_fun 				*/
/* This is the cstr done function called when a struct cstr is done	*/
/* Note: aCtx->asn1r_msg_level is one less than the value given to str_start ().	*/
/************************************************************************/

static ST_VOID str_done_fun (ASN1_DEC_CTXT *aCtx)
  {
USR_INFO *usr = (USR_INFO *) aCtx->usr_info [0];
  MLOG_CDEC0 ("pars_str_done_fun");

  if (init_new_RT (aCtx, RT_STR_END, 0))
    return;

  usr->cur_rt->u.str.num_rt_blks = 	/* Calc RT block count for struct. */
   usr->rt_blk_start[aCtx->asn1r_msg_level+1]->u.str.num_rt_blks =
    (int) (usr->cur_rt - usr->rt_blk_start[aCtx->asn1r_msg_level+1] - 1); /* Copy count back to start. */

  next_rt_index (aCtx);		/* point to next runtime element	*/
  assign_tags (aCtx);		/* setup to get next element		*/
  }


/************************************************************************/
/************************************************************************/
#if 0 	/* Constructed bitstrings and octetstrings not supported 	*/
/************************************************************************/
/************************************************************************/
/*			   get_bstr_cstr				*/
/* State function to extract a boolean value from an asn1 data element. */
/************************************************************************/


static ST_VOID get_bstr_cstr (ASN1_DEC_CTXT *aCtx)		&&& This code not implemented.
  {
  MLOG_CDEC0 ("pars_get_bstr_cstr");

  if (init_new_RT (aCtx, RT_STR_END, 0))
    return;

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = bs_cstr_done;

  next_rt_index (aCtx);		/* point to next runtime element	*/
  assign_tags (aCtx);		/* setup to get next element		*/
  }


/************************************************************************/
/*			bs_cstr_done					*/
/* This function is called when an bit string constructor is 		*/
/* completed OK. Need to reset the decode state machine.		*/
/* Note: aCtx->asn1r_msg_level is one less than the value given to get_bstr_cstr ().	*/
/************************************************************************/

static ST_VOID bs_cstr_done (ASN1_DEC_CTXT *aCtx)		&&& Not implemented yet
  {
ST_INT abs_len;
ST_INT abs_count;
int i;
ST_CHAR *to_ptr;
ST_CHAR *from_ptr;
USR_INFO *usr = (USR_INFO *) aCtx->usr_info [0];

  abs_count = abs (usr->cur_rt->u.p.el_len);
  abs_len = CALC_BIT_LEN (abs_count);

/* determine whether octet string is fixed or variable length		*/

  if (usr->cur_rt->u.p.el_len >=0)	/* fixed length				*/
    {
    if (aCtx->asn1r_bitcount != abs_count)
      {
      MLOG_NERR0 ("Fixed length bit string mismatch");
      asn1r_set_dec_err (aCtx, BAD_DATA);
      }
    }
  else		/* variable length bit string				*/
    {		/* need to move down to allow for size ST_INT16		*/
    from_ptr = datptr+abs_len-1;
    to_ptr = from_ptr+sizeof (ST_INT16);
    for (i = 0; i < abs_count; ++i)
      *(to_ptr--) = *(from_ptr--);

    *((ST_INT16 *)datptr) = (ST_INT16) aCtx->asn1r_bitcount;
    }

  datptr += usr->cur_rt->el_size;	/* Adjust data pointer    		*/
  usr->cur_rt++;			/* point to next runtime element	*/
  assign_tags (aCtx);		/* Assign tags for next data elt*/
  }


/************************************************************************/
/*			   get_ostr_cstr				*/
/* State function to extract an octet string from an asn1 data element. */
/************************************************************************/

static ST_VOID get_ostr_cstr (ASN1_DEC_CTXT *aCtx)
  {
USR_INFO *usr = (USR_INFO *) aCtx->usr_info [0];
  MLOG_CDEC0 ("pars_get_ostr_cstr");

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = os_cstr_done;
  asn1r_get_octstr_cstr (aCtx, abs (usr->cur_rt->u.p.el_len), datptr);
  }

/************************************************************************/
/*			os_cstr_done					*/
/* This function is called when an octet string constructor is 		*/
/* completed OK. Need to reset the decode state machine.		*/
/************************************************************************/

static ST_VOID os_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
ST_INT abs_len;
int i;
ST_CHAR *to_ptr;
ST_CHAR *from_ptr;
USR_INFO *usr = (USR_INFO *) aCtx->usr_info [0];

  abs_len = abs (usr->cur_rt->u.p.el_len);

/* determine whether octect string is fixed or variable length		*/

  if (usr->cur_rt->u.p.el_len >=0)	/* fixed length				*/
    {
    if (aCtx->asn1r_octetcount != abs_len)
      {
      MLOG_NERR0 ("Fixed length octet string mismatch");
      asn1r_set_dec_err (aCtx, BAD_DATA);
      }
    }
  else		/* variable length octet string				*/
    {		/* need to move down to allow for size ST_INT16		*/
    from_ptr = datptr+abs_len-1;
    to_ptr = from_ptr+sizeof (ST_INT16);
    for (i = 0; i < aCtx->asn1r_octetcount; ++i)
      *(to_ptr--) = *(from_ptr--);

    *((ST_INT16 *)datptr) = (ST_INT16) aCtx->asn1r_octetcount;
    }

  datptr += usr->cur_rt->el_size; /* Adjust data pointer    		*/
  usr->cur_rt++;			/* point to next runtime element	*/
  assign_tags (aCtx);		/* Assign tags for next data elt*/
  }

/************************************************************************/
#endif
/************************************************************************/
/************************************************************************/

/************************************************************************/
/*			   get_bool_prim				*/
/* State function to extract a boolean prim from an asn1 data element.	*/
/************************************************************************/

static ST_VOID get_bool_prim (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("pars_get_bool_prim");

  if (init_new_RT (aCtx, RT_BOOL, aCtx->asn1r_elmnt_len))
    return;

  if (aCtx->asn1r_elmnt_len != 1)
    {
    MLOG_NERR0 ("Malformed boolean data");
    asn1r_set_dec_err (aCtx, BAD_DATA);		/* checked by get function	*/
    }
  aCtx->asn1r_field_ptr += aCtx->asn1r_elmnt_len;	/* Skip over data */

  next_rt_index (aCtx);		/* point to next runtime element	*/
  assign_tags (aCtx);		/* setup to get next element		*/
  }

/************************************************************************/
/*			   get_bitstr_prim				*/
/* State function to extract a bitstring prim from an asn1 data element	*/
/************************************************************************/

static ST_VOID get_bitstr_prim (ASN1_DEC_CTXT *aCtx)
  {
ST_INT bit_count;

  MLOG_CDEC0 ("pars_get_bitstr_prim");

  bit_count = (aCtx->asn1r_elmnt_len > 1) ? ((aCtx->asn1r_elmnt_len-1)*8 - (*aCtx->asn1r_field_ptr & 7)) : 0;
  if (!aCtx->asn1r_elmnt_len || (!bit_count && *aCtx->asn1r_field_ptr))
    {
    MLOG_NERR0 ("Malformed bitstring data");
    asn1r_set_dec_err (aCtx, BAD_DATA);
    }

  if (init_new_RT (aCtx, RT_BIT_STRING, bit_count))
    return;

  aCtx->asn1r_field_ptr += aCtx->asn1r_elmnt_len;

  next_rt_index (aCtx);		/* point to next runtime element	*/
  assign_tags (aCtx);
  }

/************************************************************************/
/*			   get_int_prim 				*/
/* State function to extract an integer prim from an asn1 data element. */
/************************************************************************/

static ST_VOID get_int_prim (ASN1_DEC_CTXT *aCtx)
  {
ST_INT el_len;

  MLOG_CDEC0 ("pars_get_int_prim");

  switch (aCtx->asn1r_elmnt_len)	/* determine internal length	*/
    {
    case 1:				/* one byte int		*/
      el_len = 1;
    break;

    case 2:				/* two byte int		*/
      el_len = 2;
    break;

    case 3:				/* three byte int, expand to 4	*/
    case 4:				/* four byte integer	*/
      el_len = 4;
    break;

#ifdef INT64_SUPPORT
    case 5:
    case 6:
    case 7:
    case 8:
      el_len = 8;
    break;
#endif

    default :
      MLOG_NERR0 ("Malformed integer data");
      asn1r_set_dec_err (aCtx, BAD_DATA);     /* no other lengths are valid   	*/
      return;
    }

/* Assume all integers at least 4 bytes (i.e. ST_INT32).	*/
  if (el_len < 4)
    el_len = 4;

  if (init_new_RT (aCtx, RT_INTEGER, el_len))
    return;

  aCtx->asn1r_field_ptr += aCtx->asn1r_elmnt_len;
  next_rt_index (aCtx);		/* point to next runtime element	*/
  assign_tags (aCtx);
  }

/************************************************************************/
/*			   get_uint_prim				*/
/* State function to extract an unsigned integer prim from an asn1 data */
/* element.								*/
/************************************************************************/

static ST_VOID get_uint_prim (ASN1_DEC_CTXT *aCtx)
  {
ST_INT el_len;
ST_INT asn1ElmntLen;

  MLOG_CDEC0 ("pars_get_uint_prim");

/* Unsigned data may have a leading 0x00 if it otherwise would be neg	*/
  asn1ElmntLen = aCtx->asn1r_elmnt_len;
  if (aCtx->asn1r_elmnt_len > 1 &&
      *aCtx->asn1r_field_ptr == 0 && 
      *(aCtx->asn1r_field_ptr+1) & 0x80)
    {
    --asn1ElmntLen;
    }

  switch (asn1ElmntLen)		/* determine internal length	*/
    {
    case 1:				/* one byte */
      el_len = 1;
    break;

    case 2:				/* two bytes */
      el_len = 2;
    break;

    case 3:				/* three bytes, expand to 4	*/
    case 4:				/* four bytes	*/
      el_len = 4;
    break;

#ifdef INT64_SUPPORT
    case 5:
    case 6:
    case 7:
    case 8:
      el_len = 8;
    break;
#endif

    default :
      MLOG_NERR0 ("Malformed unsigned data");
      asn1r_set_dec_err (aCtx, BAD_DATA);     /* no other lengths are valid   	*/
      return;
    }
     
/* Assume all unsigned integers at least 4 bytes (i.e. ST_UINT32).	*/
  if (el_len < 4)
    el_len = 4;

  if (init_new_RT (aCtx, RT_UNSIGNED, el_len))
    return;

  aCtx->asn1r_field_ptr += aCtx->asn1r_elmnt_len;
  next_rt_index (aCtx);		/* point to next runtime element	*/
  assign_tags (aCtx);
  }

/************************************************************************/
/*			   get_float_prim				*/
/* State function to extract a floating point primitive from an asn1	*/
/* data element.							*/
/************************************************************************/

#ifdef FLOAT_DATA_SUPPORT

static ST_VOID get_float_prim (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("pars_get_float_prim");

  if (init_new_RT (aCtx, RT_FLOATING_POINT, aCtx->asn1r_elmnt_len))
    return;

  aCtx->asn1r_field_ptr += aCtx->asn1r_elmnt_len;
  next_rt_index (aCtx);		/* point to next runtime element	*/
  assign_tags (aCtx);
  }

#endif

/************************************************************************/
/*			   get_octstr_prim				*/
/* State function to extract an octet string primitive from an asn1	*/
/* data element.							*/
/************************************************************************/

static ST_VOID get_octstr_prim (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("pars_get_octstr_prim");

  if (init_new_RT (aCtx, RT_OCTET_STRING, aCtx->asn1r_elmnt_len))
    return;

  aCtx->asn1r_field_ptr += aCtx->asn1r_elmnt_len;
  next_rt_index (aCtx);		/* point to next runtime element	*/
  assign_tags (aCtx);
  }

/************************************************************************/
/*			   get_vstr_prim				*/
/* State function to extract a visible string primitive from an asn1	*/
/* data element.							*/
/************************************************************************/

static ST_VOID get_vstr_prim (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("pars_get_vstr_prim");

  if (init_new_RT (aCtx, RT_VISIBLE_STRING, aCtx->asn1r_elmnt_len))
    return;

  aCtx->asn1r_field_ptr += aCtx->asn1r_elmnt_len;
  next_rt_index (aCtx);		/* point to next runtime element*/
  assign_tags (aCtx);
  }

/************************************************************************/
/*			   get_gtime_prim				*/
/* State function to extract a generalized time primitive from an asn1	*/
/* data element.							*/
/************************************************************************/

#ifdef TIME_DATA_SUPPORT

static ST_VOID get_gtime_prim (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("pars_get_gtime_prim");

  if (init_new_RT (aCtx, RT_GENERAL_TIME, aCtx->asn1r_elmnt_len))
    return;

  aCtx->asn1r_field_ptr += aCtx->asn1r_elmnt_len;
  next_rt_index (aCtx);		/* point to next runtime element	*/
  assign_tags (aCtx);
  }

#endif

/************************************************************************/
/*			   get_btime_prim				*/
/* State function to extract a binary time primitive from an asn1 data	*/
/* element.								*/
/************************************************************************/

#ifdef BTOD_DATA_SUPPORT

static ST_VOID get_btime_prim (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("pars_get_btime_prim");

  if (init_new_RT (aCtx, RT_BINARY_TIME, aCtx->asn1r_elmnt_len))
    return;

  aCtx->asn1r_field_ptr += aCtx->asn1r_elmnt_len;
  next_rt_index (aCtx);		/* point to next runtime element	*/
  assign_tags (aCtx);
  }

#endif

/************************************************************************/
/*			   get_bcd_prim 				*/
/* State function to extract a binary coded decimal primitive from an	*/
/* asn1 data element.							*/
/************************************************************************/

static ST_VOID get_bcd_prim (ASN1_DEC_CTXT *aCtx)
  {

  MLOG_CDEC0 ("pars_get_bcd_prim");

  if (aCtx->asn1r_elmnt_len > 4)
    {
    MLOG_NERR0 ("Malformed bcd data");
    asn1r_set_dec_err (aCtx, BAD_DATA);
    return;
    }

  if (init_new_RT (aCtx, RT_BCD, aCtx->asn1r_elmnt_len))
    return;

  aCtx->asn1r_field_ptr += aCtx->asn1r_elmnt_len;	/* Adjust data pointer    		*/
  next_rt_index (aCtx);		/* point to next runtime element	*/
  assign_tags (aCtx);
  }

/************************************************************************/
/*			   get_utc_time_prim				*/
/* State function to extract a utc time primitive from an asn1 data	*/
/* element.								*/
/************************************************************************/

static ST_VOID get_utc_time_prim (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("pars_get_utc_time_prim");

  if (init_new_RT (aCtx, RT_UTC_TIME, aCtx->asn1r_elmnt_len))
    return;

  aCtx->asn1r_field_ptr += aCtx->asn1r_elmnt_len;
  next_rt_index (aCtx);		/* point to next runtime element	*/
  assign_tags (aCtx);
  }

/************************************************************************/
/*			   get_utf8_prim				*/
/* State function to extract a Unicode UTF8string primitive from an	*/
/* asn1 data element.							*/
/* Assume string is "variable-length", so pass "negative" len. The len	*/
/* is the number of Unicode characters. Each Unicode character takes	*/
/* at least 1 byte, so the number of characters is ALWAYS <=		*/
/* the ASN.1 encoded size.						*/
/************************************************************************/

static ST_VOID get_utf8_prim (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("get_utf8_prim");

  /* Pass "negative" len to indicate "variable-length".		*/
  if (init_new_RT (aCtx, RT_UTF8_STRING, -aCtx->asn1r_elmnt_len))
    return;

  aCtx->asn1r_field_ptr += aCtx->asn1r_elmnt_len;
  next_rt_index (aCtx);		/* point to next runtime element	*/
  assign_tags (aCtx);
  }

/************************************************************************/
/*			err_clean_fun 					*/
/************************************************************************/


static ST_VOID err_clean_fun (ASN1_DEC_CTXT *aCtx, ST_RET err_code)
  {
USR_INFO *usr = (USR_INFO *) aCtx->usr_info [0];
  if (usr->local_alloc_flag)
    {
    chk_free (usr->rt_base);
    usr->rt_base = NULL;
    }
  usr->nxt_un_rt = usr->rt_base;	/* Zero length runtime buffer. */
  }


