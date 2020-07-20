/***********************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2008, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : mmsdata.c						*/
/* PRODUCT(S)  : MMSEASE						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the functions employed by the virtual	*/
/*	machine for translating ASN.1 data (from the MMS 'Data'         */
/*	production) to local data, and vice versa.			*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 11/23/11  KCR           Added casting for proper 64bit compile       */
/* 08/26/09  JRB     31    Avoid moving constructed bstr, ostr data.	*/
/*			   Del undocumented BAD_DATA, INTERNAL errcodes.*/
/* 07/21/09  JRB     30    Chk new asn1r_bitstr_truncated flag.		*/
/* 06/25/09  MDE     29    Fixed octet string overwrite hole		*/
/* 05/01/08  JRB     28    ms_asn1_to_local: return error from decoder.	*/
/* 04/09/08  JRB     27    wr_prim_dat: fail if BVstring, OVstring len<0*/
/* 01/29/08  EJV     26    Use S_FMT_PTR macro to log pointers.		*/
/* 08/09/06  JRB     25    Add ms_local_to_asn1_2.			*/
/*			   Log TDL & data if wr_prim_dat fails.		*/
/* 03/22/06  EJV     24    RT_GENERAL_TIME: cast datptr to (time_t *).	*/
/* 10/29/04  JRB     23    Use ms_is_rt_prim, safer than el_tag range chk*/
/* 01/20/04  MDE     22    Changed some MLOG_ERR to MLOG_NERR		*/
/* 01/14/04  JRB     21    ms_local_to_asn1: stop encoding if		*/
/*			   asn1r_encode_overrun flag is set.		*/
/* 10/13/03  JRB     20    asn1r_magic error: Chg assert to err return.	*/
/* 04/14/03  JRB     19    Eliminate compiler warnings.			*/
/* 03/31/03  JRB     18    Add UTF8string support (see RT_UTF8_STRING).	*/
/*			   Use new RT_MAX_PRIM_TAG in range chk.	*/
/*			   asn1r_get_bitstr: add max_bits arg.		*/
/* 12/12/02  JRB     17    Verify aCtx using asn1r_magic.		*/
/* 07/12/02  JRB     16    Log Local Data Type (TDL) & maybe ASN.1 data	*/
/*			   if conversion fails.				*/
/* 01/23/02  JRB     15    Simplify use of usr_info (fixes QNX compile)	*/
/* 12/06/01  JRB     14    Convert to use ASN1R (re-entrant ASN1).	*/
/*			   Add ASN1_DEC_CTXT arg to ms_loc*_to_asn1.	*/
/*			   Add function ms_local_to_asn1_easy.		*/
/*			   On decode, replace globals with ..usr_info[].*/
/*			   Use macros to access usr_info[], more readable.*/
/*			   Pass datptr,rt_ptr as args to wr_prim_dat.	*/
/*			   Delete all locks, functions now thread-safe.	*/
/* 11/14/01  EJV     13    Added support for new MMS type UtcTime:	*/
/*			   added get_utc_time_prim func;		*/
/*			   ms_local_to_asn1: replaced RT_BOOLEANARRAY	*/
/*			     with RT_UTC_TIME in range check;		*/
/*			   wr_prim_dat: added case for RT_UTC_TIME;	*/
/* 09/22/00  DSF     12    Truncate extended prim bitstrings		*/
/* 09/13/99  MDE     11    Added SD_CONST modifiers			*/
/* 03/11/98  MDE     10    Removed NEST_RT_TYPES			*/
/* 02/10/98  MDE     09    No longer use runtime type 'loops' element	*/
/* 12/10/97  MDE     08    Cast for call to get_i8			*/
/* 09/05/97  MDE     07    Minor logging changes			*/
/* 08/15/97  MDE     06    BTOD handling changes			*/
/* 08/14/97  MDE     05    Fixed get_bcd_prim (ST_CHAR -> ST_INT8)	*/
/* 07/02/97  MDE     04    Check for ASN.1 encode overrun		*/
/* 06/19/97  MDE     03    Added 64 bit integer support			*/
/* 06/10/97  MDE     02    Removed stub functions (AA, AA Data)		*/
/* 06/09/97  MDE     01    Modified Runtime Type handling		*/
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
#if defined(_S_TEST_PROBES)
#include "stestprb.h"
#endif

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/
/* DEC_INFO - structure to store temporary decode information.		*/
/************************************************************************/
typedef struct
  {
  ST_INT arr_loop_level;
  ST_INT *arr_loops;
  SD_CONST RUNTIME_TYPE *rt_ptr;	/* current RUNTIME_TYPE struct	*/
  SD_CONST RUNTIME_TYPE *rt_end;	/* last RUNTIME_TYPE struct	*/
  ST_CHAR *datptr;			/* decoded data destination	*/
  } DEC_INFO;

/************************************************************************/
/* Local functions for ASN1 decode.					*/
/************************************************************************/
static ST_VOID assign_tags (ASN1_DEC_CTXT *aCtx);
static ST_RET  wr_prim_dat (ASN1_ENC_CTXT *aCtx, ST_CHAR *datptr, SD_CONST RUNTIME_TYPE *rt_ptr);
static ST_VOID get_bstr_cstr   (ASN1_DEC_CTXT *aCtx); 	/* for constructed bitstrings	*/
static ST_VOID bs_cstr_done    (ASN1_DEC_CTXT *aCtx);
static ST_VOID get_ostr_cstr   (ASN1_DEC_CTXT *aCtx); 	/* for constructed octetstrings */
static ST_VOID os_cstr_done    (ASN1_DEC_CTXT *aCtx);
static ST_VOID get_bool_prim   (ASN1_DEC_CTXT *aCtx); 	/* for boolean primitives	*/
static ST_VOID get_bitstr_prim (ASN1_DEC_CTXT *aCtx); 	/* for bitstring primitives	*/
static ST_VOID get_int_prim    (ASN1_DEC_CTXT *aCtx); 	/* for integer primitives	*/
static ST_VOID get_uint_prim   (ASN1_DEC_CTXT *aCtx); 	/* for unsigned integers	*/
static ST_VOID get_float_prim  (ASN1_DEC_CTXT *aCtx); 	/* for float primitives 	*/
static ST_VOID get_octstr_prim (ASN1_DEC_CTXT *aCtx); 	/* for octet string primitives	*/
static ST_VOID get_vstr_prim   (ASN1_DEC_CTXT *aCtx); 	/* for visible string primitives*/
static ST_VOID get_gtime_prim  (ASN1_DEC_CTXT *aCtx); 	/* for generalized time 	*/
static ST_VOID get_btime_prim  (ASN1_DEC_CTXT *aCtx); 	/* for binary time primitives	*/
static ST_VOID get_utc_time_prim (ASN1_DEC_CTXT *aCtx);	/* for UTC time primitives	*/
static ST_VOID get_bcd_prim    (ASN1_DEC_CTXT *aCtx); 	/* for binary coded decimal	*/
static ST_VOID get_utf8_prim   (ASN1_DEC_CTXT *aCtx);	/* for Unicode UTF8string	*/
static ST_VOID arr_start	    (ASN1_DEC_CTXT *aCtx);
static ST_VOID str_start	    (ASN1_DEC_CTXT *aCtx);
static ST_VOID arr_done_fun    (ASN1_DEC_CTXT *aCtx);
static ST_VOID str_done_fun    (ASN1_DEC_CTXT *aCtx);
static ST_VOID data_asn1_decode_done (ASN1_DEC_CTXT *aCtx);
static ST_VOID data_dec_err (ASN1_DEC_CTXT *aCtx,ST_RET); 	/* for type definition error	*/

static ST_VOID _log_data_type (SD_CONST RUNTIME_TYPE *rt_head, ST_INT rt_num);

#if defined(_S_TEST_PROBES)

static ST_RET _wr_constructed_bitstring (ASN1_ENC_CTXT *aCtx, ST_CHAR *datptr, 
					 SD_CONST RUNTIME_TYPE *rt_ptr);

static ST_RET _wr_constructed_octetstring (ASN1_ENC_CTXT *aCtx, ST_CHAR *datptr, 
					 SD_CONST RUNTIME_TYPE *rt_ptr);

#endif

/************************************************************************/
/*			ms_asn1_to_locl 				*/
/* Function to convert an MMS ASN.1 'Data' entity into local blocked    */
/* data.  Uses the ASN1DE decode tools to check the received data	*/
/* against the expected data for the selected type using the runtime	*/
/* type table for that type.  Returns 0 if the decode is OK, else an	*/
/* error code.								*/
/************************************************************************/

#ifndef MMS_LITE

ST_RET ms_asn1_to_locl (NAMED_TYPE *tptr, ST_UCHAR *asn1ptr,
			ST_INT asn1len, ST_CHAR *dptr)
  {
  if (!tptr->rt_num)		/* check to make sure some components	*/
    {
    MLOG_NERR0 ("Named type has no runtime elements");
    return (MVE_RT_TYPE);		/* 0 length typedef is error		*/
    }
  return (ms_asn1_to_local (tptr->rt_head,tptr->rt_num,
                                             asn1ptr,asn1len,dptr));
  }

#endif

/************************************************************************/
/*			ms_asn1_to_local 				*/
/************************************************************************/

ST_RET ms_asn1_to_local (SD_CONST RUNTIME_TYPE *rt_head, ST_INT rt_num, 
			 ST_UCHAR *asn1ptr, ST_INT asn1len, ST_CHAR *dptr)
  {
ST_RET ret;
ST_INT arr_loop_buf[ASN1_MAX_LEVEL];
ASN1_DEC_CTXT aCtx;
DEC_INFO decInfo;	/* temporary storage for decode info.		*/

  /* Set initial values in decInfo.			*/ 
  decInfo.arr_loop_level = 0;
  decInfo.arr_loops = arr_loop_buf;
  decInfo.rt_ptr = rt_head;		/* point to head rt_block	*/
  decInfo.rt_end = decInfo.rt_ptr + rt_num;	/* done when pointer is here*/
  decInfo.datptr = dptr;	/* datptr is the running dest pointer.	*/

  /* Initialize the decode context "aCtx" and call "asn1r_decode_asn1".	*/
  memset (&aCtx, 0, sizeof (ASN1_DEC_CTXT));	/* CRITICAL: start clean.*/
  aCtx.usr_info[0] = &decInfo;	/* save ptr to decInfo in context.	*/
  aCtx.asn1r_decode_method = ASN1_TAG_METHOD;	/* select tag method	*/
  aCtx.asn1r_decode_done_fun = asn1r_done_err;/* not legal to be done at this time	*/
  aCtx.asn1r_err_fun = data_dec_err;	/* parse error => call to data_dec_err	*/

  assign_tags (&aCtx);		/* set up tags allowed by 1st rt_block	*/
  asn1r_decode_asn1 (&aCtx,asn1ptr,asn1len);/* decode entire ASN.1 'Data' entity    */

  if (aCtx.asn1r_pdu_dec_err == NO_DECODE_ERR)/* check for sucess or return error	*/
    ret = SD_SUCCESS;
  else
    {
    MLOG_NERR0 ("ASN.1 to Local data conversion error. ASN.1 data to convert:");
    if (mms_debug_sel & MMS_LOG_NERR)
      {			/* use ALWAYSH but only if NERR enabled*/
      MLOG_ALWAYSH (asn1len, asn1ptr);
      }
    /* Conversion failed, try to log the type used for the conversion.	*/
    _log_data_type (rt_head, rt_num);

    ret = aCtx.asn1r_pdu_dec_err;	/* Just return ASN.1 decode error*/
    }

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
  {
DEC_INFO *decInfo = (DEC_INFO *) aCtx->usr_info[0];	/* get saved ptr*/
  if (decInfo->rt_ptr < decInfo->rt_end)	/* if elements to look at	*/
    {
    if (decInfo->rt_ptr->el_tag == RT_ARR_END)	/* treat case of array ending*/
      {
      if (--decInfo->arr_loops[decInfo->arr_loop_level] > 0) /* if need to do next ar elmnt	*/
	decInfo->rt_ptr -= decInfo->rt_ptr->u.arr.num_rt_blks; /* mv rtPtr to start of arr	*/
      else
        --decInfo->arr_loop_level;
      }

    switch (decInfo->rt_ptr->el_tag)
      {
      case RT_ARR_START :
	ASN1R_TAG_ADD (aCtx,CTX|CONSTR,1,arr_start); /* expecting an array cstr	*/
      break;

      case RT_STR_START :
	ASN1R_TAG_ADD (aCtx,CTX|CONSTR,2,str_start); /* expecting an struct cstr	*/
      break;

      case RT_BOOL :
	ASN1R_TAG_ADD (aCtx,CTX,3,get_bool_prim);	
      break;

      case RT_BIT_STRING :			
	ASN1R_TAG_ADD (aCtx,CTX,4,get_bitstr_prim);
	ASN1R_TAG_ADD (aCtx,CTX|CONSTR,4,get_bstr_cstr);
      break;

      case RT_INTEGER :			
	ASN1R_TAG_ADD (aCtx,CTX,5,get_int_prim);
      break;

      case RT_UNSIGNED : 			
	ASN1R_TAG_ADD (aCtx,CTX,6,get_uint_prim);
      break;

#ifdef FLOAT_DATA_SUPPORT
      case RT_FLOATING_POINT : 		
	ASN1R_TAG_ADD (aCtx,CTX,7,get_float_prim);
      break;
#endif

      case RT_OCTET_STRING : 		
	ASN1R_TAG_ADD (aCtx,CTX,9,get_octstr_prim);
	ASN1R_TAG_ADD (aCtx,CTX|CONSTR,9,get_ostr_cstr);
      break;

      case RT_VISIBLE_STRING : 		
	ASN1R_TAG_ADD (aCtx,CTX,10,get_vstr_prim);
      break;

#ifdef TIME_DATA_SUPPORT
      case RT_GENERAL_TIME : 		
	ASN1R_TAG_ADD (aCtx,CTX,11,get_gtime_prim);
      break;
#endif

#ifdef BTOD_DATA_SUPPORT
      case RT_BINARY_TIME : 		
	ASN1R_TAG_ADD (aCtx,CTX,12,get_btime_prim);
      break;
#endif

      case RT_BCD : 			
	ASN1R_TAG_ADD (aCtx,CTX,13,get_bcd_prim);
      break;

      case RT_UTC_TIME : 		
	ASN1R_TAG_ADD (aCtx,CTX,17,get_utc_time_prim);
      break;

      case RT_UTF8_STRING : 		
	ASN1R_TAG_ADD (aCtx,CTX,RT_UTF8_STRING,get_utf8_prim);
      break;

      case RT_ARR_END :			/* array done			*/
	aCtx->asn1r_c_done_fun[aCtx->asn1r_msg_level] = arr_done_fun; /* the arr cstr must be done*/
      break;

      case RT_STR_END :			/* structure done		*/
	aCtx->asn1r_c_done_fun[aCtx->asn1r_msg_level] = str_done_fun; /* the str cstr must be done*/
      break;

      default : 			/* should not be any other tag	*/
        MLOG_NERR0 ("Bad tag in runtime type");
	asn1r_set_dec_err (aCtx,MVE_DATA_CONVERT);
	return;
      break;
      }
    }
  else					/* no more elements to do	*/
    {
    aCtx->asn1r_decode_done = SD_TRUE;	/* terminate decode		*/
    aCtx->asn1r_decode_done_fun = data_asn1_decode_done; /* ok to be done now		*/
    }
  }


/************************************************************************/
/*			   arr_start					*/
/* This is the function called when an array cstr is starting		*/
/************************************************************************/

static ST_VOID arr_start (ASN1_DEC_CTXT *aCtx)
  {
DEC_INFO *decInfo = (DEC_INFO *) aCtx->usr_info[0];	/* get saved ptr*/
  MLOG_CDEC0 ("arr_start");

/* initialize the loop counter for the array				*/

  ++decInfo->arr_loop_level;
  decInfo->arr_loops[decInfo->arr_loop_level] = decInfo->rt_ptr->u.arr.num_elmnts;

  decInfo->datptr += decInfo->rt_ptr->el_size; /* Adjust data pointer		*/
  decInfo->rt_ptr++;		/* point to next runtime element	*/
  assign_tags (aCtx);		/* setup to get next element		*/
  }


/************************************************************************/
/*			   arr_done_fun 				*/
/* This is the cstr done function called when an array cstr is done	*/
/************************************************************************/

static ST_VOID arr_done_fun (ASN1_DEC_CTXT *aCtx)
  {
DEC_INFO *decInfo = (DEC_INFO *) aCtx->usr_info[0];	/* get saved ptr*/
  MLOG_CDEC0 ("arr_done_fun");

  decInfo->datptr += decInfo->rt_ptr->el_size; /* Adjust data pointer 		*/
  decInfo->rt_ptr++;		/* point to next runtime element	*/
  assign_tags (aCtx);		/* setup to get next element		*/
  }


/************************************************************************/
/*			   str_start					*/
/* This is the function called when an struct cstr is starting		*/
/************************************************************************/

static ST_VOID str_start (ASN1_DEC_CTXT *aCtx)
  {
DEC_INFO *decInfo = (DEC_INFO *) aCtx->usr_info[0];	/* get saved ptr*/
  MLOG_CDEC0 ("str_start");

  decInfo->datptr += decInfo->rt_ptr->el_size; /* Adjust data pointer		*/
  decInfo->rt_ptr++;		/* point to next runtime element	*/
  assign_tags (aCtx);		/* setup to get next element		*/
  }

/************************************************************************/
/*			   str_done_fun 				*/
/* This is the cstr done function called when an struct cstr is done	*/
/************************************************************************/

static ST_VOID str_done_fun (ASN1_DEC_CTXT *aCtx)
  {
DEC_INFO *decInfo = (DEC_INFO *) aCtx->usr_info[0];	/* get saved ptr*/
  MLOG_CDEC0 ("str_done_fun");

  decInfo->datptr += decInfo->rt_ptr->el_size; /* Adjust data pointer		*/
  decInfo->rt_ptr++;		/* point to next runtime element	*/
  assign_tags (aCtx);		/* setup to get next element		*/
  }


/************************************************************************/
/*			   get_bstr_cstr				*/
/* State function to extract a boolean value from an asn1 data element. */
/************************************************************************/

static ST_VOID get_bstr_cstr (ASN1_DEC_CTXT *aCtx)
  {
DEC_INFO *decInfo = (DEC_INFO *) aCtx->usr_info[0];	/* get saved ptr*/
  MLOG_CDEC0 ("get_bstr_cstr");

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = bs_cstr_done;

  /* Pass different data pointer for fixed len or variable len bitstring.*/
  if (decInfo->rt_ptr->u.p.el_len >=0)	/* fixed length		*/
    asn1r_get_bitstr_cstr (aCtx,abs (decInfo->rt_ptr->u.p.el_len), (ST_UCHAR *) decInfo->datptr);
  else					/* variable length	*/
    asn1r_get_bitstr_cstr (aCtx,abs (decInfo->rt_ptr->u.p.el_len), (ST_UCHAR *) decInfo->datptr + sizeof (ST_INT16));
  }                        

/************************************************************************/
/*			bs_cstr_done					*/
/* This function is called when an bit string constructor is 		*/
/* completed OK. Need to reset the decode state machine.		*/
/************************************************************************/

static ST_VOID bs_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
ST_INT abs_len;
ST_INT abs_count;
DEC_INFO *decInfo = (DEC_INFO *) aCtx->usr_info[0];	/* get saved ptr*/

  abs_count = abs (decInfo->rt_ptr->u.p.el_len);
  abs_len = (abs_count+7) / 8;

/* determine whether octect string is fixed or variable length		*/

  if (decInfo->rt_ptr->u.p.el_len >=0)	/* fixed length			*/
    {
    if (aCtx->asn1r_bitstr_truncated || aCtx->asn1r_bitcount != abs_count)
      {
      MLOG_NERR0 ("Fixed length bit string mismatch");
      asn1r_set_dec_err (aCtx,MVE_DATA_CONVERT);
      return;
      }
    }
  else		/* variable length bit string				*/
    {
    /* Set length.	*/
    *((ST_INT16 *)decInfo->datptr) = (ST_INT16) aCtx->asn1r_bitcount;
    }

  decInfo->datptr += decInfo->rt_ptr->el_size; /* Adjust data pointer 		*/
  decInfo->rt_ptr++;		/* point to next runtime element	*/
  assign_tags (aCtx);		/* Assign tags for next data elt*/
  }


/************************************************************************/
/*			   get_ostr_cstr				*/
/* State function to extract an octet string from an asn1 data element. */
/************************************************************************/

static ST_VOID get_ostr_cstr (ASN1_DEC_CTXT *aCtx)
  {
DEC_INFO *decInfo = (DEC_INFO *) aCtx->usr_info[0];	/* get saved ptr*/
  MLOG_CDEC0 ("get_ostr_cstr");

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = os_cstr_done;

  /* Pass different data pointer for fixed len or variable len octetstring.*/
  if (decInfo->rt_ptr->u.p.el_len >=0)	/* fixed length		*/
    asn1r_get_octstr_cstr (aCtx,abs (decInfo->rt_ptr->u.p.el_len), (ST_UCHAR *) decInfo->datptr);
  else					/* variable length	*/
    asn1r_get_octstr_cstr (aCtx,abs (decInfo->rt_ptr->u.p.el_len), (ST_UCHAR *) decInfo->datptr + sizeof (ST_INT16));
  }

/************************************************************************/
/*			os_cstr_done					*/
/* This function is called when an octet string constructor is 		*/
/* completed OK. Need to reset the decode state machine.		*/
/************************************************************************/

static ST_VOID os_cstr_done (ASN1_DEC_CTXT *aCtx)
  {
ST_INT abs_len;
DEC_INFO *decInfo = (DEC_INFO *) aCtx->usr_info[0];	/* get saved ptr*/

  abs_len = abs (decInfo->rt_ptr->u.p.el_len);

/* determine whether octect string is fixed or variable length		*/

  if (decInfo->rt_ptr->u.p.el_len >=0)	/* fixed length			*/
    {
    if (aCtx->asn1r_octetcount != abs_len)
      {
      MLOG_NERR0 ("Fixed length octet string mismatch");
      asn1r_set_dec_err (aCtx,MVE_DATA_CONVERT);
      return;
      }
    }
  else		/* variable length octet string				*/
    {
    /* Set length.	*/
    *((ST_INT16 *)decInfo->datptr) = (ST_INT16) aCtx->asn1r_octetcount;	
    }

  decInfo->datptr += decInfo->rt_ptr->el_size; /* Adjust data pointer    	*/
  decInfo->rt_ptr++;		/* point to next runtime element	*/
  assign_tags (aCtx);		/* Assign tags for next data elt*/
  }


/************************************************************************/
/*			   get_bool_prim				*/
/* State function to extract a boolean prim from an asn1 data element.	*/
/************************************************************************/

static ST_VOID get_bool_prim   (ASN1_DEC_CTXT *aCtx)
  {
DEC_INFO *decInfo = (DEC_INFO *) aCtx->usr_info[0];	/* get saved ptr*/
  MLOG_CDEC0 ("get_bool_prim");

  if (asn1r_get_bool (aCtx,(ST_UCHAR *)decInfo->datptr))	/* Get the boolean length	*/
    asn1r_set_dec_err (aCtx,MVE_DATA_CONVERT);		/* checked by get function	*/

  decInfo->datptr += decInfo->rt_ptr->el_size; /* Adjust data pointer    	*/
  decInfo->rt_ptr++;		/* point to next runtime element	*/
  assign_tags (aCtx);		/* continue the parse		*/
  }

/************************************************************************/
/*			   get_bitstr_prim				*/
/* State function to extract an bitstring prim from an asn1 data element*/
/************************************************************************/

static ST_VOID get_bitstr_prim    (ASN1_DEC_CTXT *aCtx)
  {
ST_INT abs_len;
ST_INT abs_count;
DEC_INFO *decInfo = (DEC_INFO *) aCtx->usr_info[0];	/* get saved ptr*/

  MLOG_CDEC0 ("get_bitstr_prim");

  abs_count = abs (decInfo->rt_ptr->u.p.el_len);
  abs_len = CALC_BIT_LEN (abs_count);

/* determine whether bitstring is fixed or variable length		*/
  if (decInfo->rt_ptr->u.p.el_len >= 0)
    {
    if (asn1r_get_bitstr (aCtx,(ST_UCHAR *)decInfo->datptr, abs_count))
      {
      MLOG_CDEC0 ("Bitstring decode failed");
      asn1r_set_dec_err (aCtx,MVE_DATA_CONVERT);
      }
    else if (aCtx->asn1r_bitstr_truncated || aCtx->asn1r_bitcount != abs_count)
      {
      /* For 'fixed length' bitstr, length must match expected length.	*/
      /* If bitstr truncated, this implies length DID NOT match.	*/
      MLOG_CDEC0 ("Bitstring length incorrect");
      asn1r_set_dec_err (aCtx,MVE_DATA_CONVERT);
      }
    }
  else		/* variable length octet string				*/
    {
    if (asn1r_get_bitstr (aCtx,(ST_UCHAR *)decInfo->datptr + sizeof (ST_INT16), abs_count))
      asn1r_set_dec_err (aCtx,MVE_DATA_CONVERT);
    else
      {
      *((ST_INT16 *)decInfo->datptr) = (ST_INT16) aCtx->asn1r_bitcount;
      }
    }

  decInfo->datptr += decInfo->rt_ptr->el_size; /* Adjust data pointer    	*/
  decInfo->rt_ptr++;		/* point to next runtime element	*/
  assign_tags (aCtx);
  }

/************************************************************************/
/*			   get_int_prim 				*/
/* State function to extract an integer prim from an asn1 data element. */
/************************************************************************/

static ST_VOID get_int_prim    (ASN1_DEC_CTXT *aCtx)
  {
DEC_INFO *decInfo = (DEC_INFO *) aCtx->usr_info[0];	/* get saved ptr*/
  MLOG_CDEC0 ("get_int_prim");

   switch (decInfo->rt_ptr -> u.p.el_len)	/* determine length	       */
     {
     case 1 :			       /* one byte int		       */
       if (asn1r_get_i8 (aCtx,(ST_INT8 *)decInfo->datptr)) /* write a single byte	       */
	 asn1r_set_dec_err (aCtx,MVE_DATA_CONVERT);
     break;

     case 2 :			       /* two byte int		       */
       if (asn1r_get_i16 (aCtx,(ST_INT16 *)decInfo->datptr)) /* read 16 bit integer       */
	 asn1r_set_dec_err (aCtx,MVE_DATA_CONVERT);
     break;

     case 4 :			      /* four byte integer	       */
       if (asn1r_get_i32 (aCtx,(ST_INT32 *)decInfo->datptr))/* read 4 bytes*/
	 asn1r_set_dec_err (aCtx,MVE_DATA_CONVERT);
     break;

#ifdef INT64_SUPPORT
     case 8 :			      /* eight byte integer	       */
       if (asn1r_get_i64 (aCtx,(ST_INT64 *)decInfo->datptr))/* read 8 bytes*/
	 asn1r_set_dec_err (aCtx,MVE_DATA_CONVERT);
     break;
#endif

     default :
       MLOG_NERR0 ("Invalid integer size in RT");
       asn1r_set_dec_err (aCtx,MVE_DATA_CONVERT);	/* no other lengths are valid*/
       return;
     }

  decInfo->datptr += decInfo->rt_ptr->el_size; 	/* Adjust data pointer	*/
  decInfo->rt_ptr++;		/* point to next runtime element	*/
  assign_tags (aCtx);
  }

/************************************************************************/
/*			   get_uint_prim				*/
/* State function to extract an unsigned integer prim from an asn1 data */
/* element.								*/
/************************************************************************/

static ST_VOID get_uint_prim   (ASN1_DEC_CTXT *aCtx)
  {
DEC_INFO *decInfo = (DEC_INFO *) aCtx->usr_info[0];	/* get saved ptr*/
  MLOG_CDEC0 ("get_uint_prim");

   switch (decInfo->rt_ptr -> u.p.el_len) 	/* determine length	       */
     {
     case 1 :			       /* one byte int		       */
       if (asn1r_get_u8 (aCtx,(ST_UCHAR *)decInfo->datptr)) /* write a single byte	       */
	    asn1r_set_dec_err (aCtx,MVE_DATA_CONVERT);
     break;

     case 2 :			       /* two byte int		       */
       if (asn1r_get_u16 (aCtx,(ST_UINT16 *)decInfo->datptr)) /* read 16 bit integer 	*/
         {
	 asn1r_set_dec_err (aCtx,MVE_DATA_CONVERT);
         }   
     break;

     case 4 :			      /* four byte integer	       */
       if (asn1r_get_u32 (aCtx,(ST_UINT32 *)decInfo->datptr))/* read 4 bytes*/
	 asn1r_set_dec_err (aCtx,MVE_DATA_CONVERT);
     break;

#ifdef INT64_SUPPORT
     case 8 :			      /* eight byte integer	       */
       if (asn1r_get_u64 (aCtx,(ST_UINT64 *)decInfo->datptr))/* read 8 bytes*/
	 asn1r_set_dec_err (aCtx,MVE_DATA_CONVERT);
     break;
#endif

     default :
       MLOG_NERR0 ("Invalid integer size in RT");
       asn1r_set_dec_err (aCtx,MVE_DATA_CONVERT);	/* no other lengths are valid*/
       return;
     }
  decInfo->datptr += decInfo->rt_ptr->el_size; /* Adjust data pointer		*/
  decInfo->rt_ptr++;		/* point to next runtime element	*/
  assign_tags (aCtx);
  }

/************************************************************************/
/*			   get_float_prim				*/
/* State function to extract a floating point primitive from an asn1	*/
/* data element.							*/
/************************************************************************/

#ifdef FLOAT_DATA_SUPPORT

static ST_VOID get_float_prim  (ASN1_DEC_CTXT *aCtx)
  {
DEC_INFO *decInfo = (DEC_INFO *) aCtx->usr_info[0];	/* get saved ptr*/
  MLOG_CDEC0 ("get_float_prim");

  if (decInfo->rt_ptr -> u.p.el_len == 8)
    {
    if (asn1r_get_double (aCtx,(ST_DOUBLE *)decInfo->datptr))
      asn1r_set_dec_err (aCtx,MVE_DATA_CONVERT);
    }
  else if (decInfo->rt_ptr -> u.p.el_len == 4)
    {
    if (asn1r_get_float (aCtx,(ST_FLOAT *)decInfo->datptr))
      asn1r_set_dec_err (aCtx,MVE_DATA_CONVERT);
    }
  else
    {
    MLOG_NERR0 ("Bad float size in RT");
    asn1r_set_dec_err (aCtx,MVE_DATA_CONVERT);
    return;
    }

  decInfo->datptr += decInfo->rt_ptr->el_size; /* Adjust data pointer    	*/
  decInfo->rt_ptr++;		/* point to next runtime element	*/
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
ST_INT abs_len;
ST_INT var_len;	/* variable length octetstring decoded length	*/
DEC_INFO *decInfo = (DEC_INFO *) aCtx->usr_info[0];	/* get saved ptr*/

  MLOG_CDEC0 ("get_octstr_prim");

  abs_len = abs (decInfo->rt_ptr->u.p.el_len);

  if (decInfo->rt_ptr->u.p.el_len >= 0)	/* fixed length	*/
    {
    if (asn1r_get_octstr_fixlen (aCtx,(ST_UCHAR *) decInfo->datptr, abs_len))
      {
      MLOG_NERR0 ("Fixed length octet string decode error");
      asn1r_set_dec_err (aCtx,MVE_DATA_CONVERT);
      return;
      }
    }
  else					/* variable length	*/
    {
    if (asn1r_get_octstr_varlen (aCtx,(ST_UCHAR *) decInfo->datptr + sizeof (ST_INT16), abs_len, &var_len))
      {
      MLOG_NERR0 ("Variable length octet string decode error");
      asn1r_set_dec_err (aCtx,MVE_DATA_CONVERT);
      return;
      }
    else
      {
      *((ST_INT16 *)decInfo->datptr) = (ST_INT16) var_len;
      }
    }

  decInfo->datptr += decInfo->rt_ptr->el_size; /* Adjust data pointer    	*/
  decInfo->rt_ptr++;		/* point to next runtime element	*/
  assign_tags (aCtx);
  }

/************************************************************************/
/*			   get_vstr_prim				*/
/* State function to extract a visible string primitive from an asn1	*/
/* data element.							*/
/************************************************************************/

static ST_VOID get_vstr_prim   (ASN1_DEC_CTXT *aCtx)
  {
DEC_INFO *decInfo = (DEC_INFO *) aCtx->usr_info[0];	/* get saved ptr*/
  MLOG_CDEC0 ("get_vstr_prim");

/* When the u.p.el_len is positive the v-string is of fixed length 	*/
/* meaning that the aCtx->asn1r_elmnt_len has to be equal.  It is ok for the 	*/
/* u.p.el_len to be negative.  When this is encountered it means that 	*/
/* the v_string	is of variable length, the aCtx->asn1r_elmnt_len has to be <= 	*/
/* u.p.el_len to be valid.						*/

  if (((decInfo->rt_ptr -> u.p.el_len > 0) && 
       (aCtx->asn1r_elmnt_len != decInfo->rt_ptr->u.p.el_len)) ||
       (aCtx->asn1r_elmnt_len > abs (decInfo->rt_ptr -> u.p.el_len)) || 
       (asn1r_get_vstr (aCtx,decInfo->datptr)))
    {
    MLOG_NERR0 ("Vstring size or content problem");
    asn1r_set_dec_err (aCtx,MVE_DATA_CONVERT);
    return;
    }
					/* v-strings are null term'd	*/
  decInfo->datptr += decInfo->rt_ptr->el_size; /* Adjust data pointer		*/
  decInfo->rt_ptr++;		/* point to next runtime element	*/
  assign_tags (aCtx);
  }

/************************************************************************/
/*			   get_utf8_prim				*/
/* State function to extract a Unicode UTF8string primitive from an	*/
/* ASN.1 data element.							*/
/************************************************************************/

static ST_VOID get_utf8_prim   (ASN1_DEC_CTXT *aCtx)
  {
DEC_INFO *decInfo = (DEC_INFO *) aCtx->usr_info[0];	/* get saved ptr*/
  MLOG_CDEC0 ("get_utf8_prim");

/* If "el_len" is positive, the string is "fixed length" and this is the
 * number of characters.
 * If "el_len" is negative, the string is "variable length" and
 * "abs(el_len)" is the "maximum" number of characters.
 * Pass "el_len" to the ASN.1 function and it will take care of all len chks.
 */

  if (asn1r_get_utf8 (aCtx,decInfo->datptr,decInfo->rt_ptr->u.p.el_len))
    {
    MLOG_NERR0 ("Unicode UTF8(V)string decode error");
    asn1r_set_dec_err (aCtx,MVE_DATA_CONVERT);
    return;
    }
  decInfo->datptr += decInfo->rt_ptr->el_size; /* Adjust data pointer	*/
  decInfo->rt_ptr++;		/* point to next runtime element	*/
  assign_tags (aCtx);
  }

/************************************************************************/
/*			   get_gtime_prim				*/
/* State function to extract a generalized time primitive from an asn1	*/
/* data element.							*/
/************************************************************************/

#ifdef TIME_DATA_SUPPORT

static ST_VOID get_gtime_prim  (ASN1_DEC_CTXT *aCtx)
  {
DEC_INFO *decInfo = (DEC_INFO *) aCtx->usr_info[0];	/* get saved ptr*/
  MLOG_CDEC0 ("get_gtime_prim");

  if (asn1r_get_time (aCtx,(time_t *)decInfo->datptr))
    asn1r_set_dec_err (aCtx,MVE_DATA_CONVERT);
  decInfo->datptr += decInfo->rt_ptr->el_size; /* Adjust data pointer    	*/
  decInfo->rt_ptr++;		/* point to next runtime element	*/
  assign_tags (aCtx);
  }

#endif

/************************************************************************/
/*			   get_btime_prim				*/
/* State function to extract a binary time primitive from an asn1 data	*/
/* element.								*/
/************************************************************************/

#ifdef BTOD_DATA_SUPPORT

static ST_VOID get_btime_prim  (ASN1_DEC_CTXT *aCtx)
  {
MMS_BTOD btod_data;
ST_INT32 *i32_ptr;
DEC_INFO *decInfo = (DEC_INFO *) aCtx->usr_info[0];	/* get saved ptr*/

  MLOG_CDEC0 ("get_btime_prim");

  if (asn1r_get_btod (aCtx,&btod_data))
    asn1r_set_dec_err (aCtx,MVE_DATA_CONVERT);
  else if (btod_data.form != decInfo->rt_ptr->u.p.el_len)
    {
    MLOG_NERR0 ("BTOD form mismatch");
    asn1r_set_dec_err (aCtx,MVE_DATA_CONVERT);
    return;
    }
  else
    {
    i32_ptr = (ST_INT32 *) decInfo->datptr;
    *i32_ptr = btod_data.ms;
    if (btod_data.form == MMS_BTOD6)
      {
      ++i32_ptr;
      *i32_ptr = btod_data.day;
      }
    decInfo->datptr += decInfo->rt_ptr->el_size; /* Adjust data pointer    	*/
    decInfo->rt_ptr++;		/* point to next runtime element	*/
    assign_tags (aCtx);
    }
  }

#endif

/************************************************************************/
/*			   get_bcd_prim 				*/
/* State function to extract a binary coded decimal primitive from an	*/
/* asn1 data element.							*/
/************************************************************************/

static ST_VOID get_bcd_prim    (ASN1_DEC_CTXT *aCtx)
  {
ST_INT32 range;
ST_INT8	 btemp;
ST_INT16 stemp;
ST_INT32 ltemp;
ST_INT   i;
DEC_INFO *decInfo = (DEC_INFO *) aCtx->usr_info[0];	/* get saved ptr*/

  range = 0;

  MLOG_CDEC0 ("get_bcd_prim");

/* we have to calculate the legal range of digits this bcd type will    */
/* fit in.                                                              */

   for (i = 0; i < decInfo->rt_ptr->u.p.el_len; i++)
      range = (range * 10) + 9;

   if (decInfo->rt_ptr -> u.p.el_len <= 2)	/* determine length	       */
     {
     if (asn1r_get_i8 (aCtx,&btemp))	/* get a two nibble value	*/
       asn1r_set_dec_err (aCtx,MVE_DATA_CONVERT);
     if ((btemp > (ST_CHAR)range) ||  /* is it in range		*/
          (btemp < 0))
       {
       MLOG_NERR0 ("BCD value out of range");
       asn1r_set_dec_err (aCtx,MVE_DATA_CONVERT);
       return;
       }
     else
       memcpy (decInfo->datptr, &btemp, sizeof (ST_CHAR)); /* copy good data*/
     }
   else if (decInfo->rt_ptr -> u.p.el_len <= 4)
     {
     if (asn1r_get_i16 (aCtx,&stemp))		/* read 4 nibble BCD	*/
       asn1r_set_dec_err (aCtx,MVE_DATA_CONVERT);
     if ((stemp > (ST_INT16) range) || (stemp < 0))
       {
       MLOG_NERR0 ("BCD value out of range");
       asn1r_set_dec_err (aCtx,MVE_DATA_CONVERT);
       return;
       }
     else
       memcpy (decInfo->datptr, &stemp, sizeof (ST_INT16)); /* copy good data	*/
     }
   else if (decInfo->rt_ptr -> u.p.el_len <= 8)
     {
     if (asn1r_get_i32 (aCtx,&ltemp))		/* read 4 bytes	*/
       asn1r_set_dec_err (aCtx,MVE_DATA_CONVERT);
     if ((ltemp > range) || (ltemp < 0))
       {
       MLOG_NERR0 ("BCD value out of range");
       asn1r_set_dec_err (aCtx,MVE_DATA_CONVERT);
       return;
       }
     else
       memcpy (decInfo->datptr, &ltemp, sizeof (ST_INT32)); /* copy good data*/
     }
   else
    asn1r_set_dec_err (aCtx,MVE_DATA_CONVERT);	     /* no other lengths are valid*/

  decInfo->datptr += decInfo->rt_ptr->el_size; /* Adjust data pointer		*/
  decInfo->rt_ptr++;		/* point to next runtime element	*/
  assign_tags (aCtx);
  }

/************************************************************************/
/*			   get_utc_time_prim				*/
/* State function to extract a UTC time primitive from an asn1 data	*/
/* element.								*/
/************************************************************************/

static ST_VOID get_utc_time_prim  (ASN1_DEC_CTXT *aCtx)
  {
MMS_UTC_TIME utc_time;
DEC_INFO *decInfo = (DEC_INFO *) aCtx->usr_info[0];	/* get saved ptr*/

  MLOG_CDEC0 ("get_utc_time_prim");

  if (asn1r_get_utc_time (aCtx, &utc_time))
    asn1r_set_dec_err (aCtx,MVE_DATA_CONVERT);
  else
    /* just copy the whole struct */
    memcpy (decInfo->datptr, (ST_CHAR *)&utc_time, sizeof (MMS_UTC_TIME));

  decInfo->datptr += decInfo->rt_ptr->el_size;	/* Adjust data pointer		*/
  decInfo->rt_ptr++;		/* point to next runtime element	*/
  assign_tags (aCtx);
  }

/************************************************************************/
/*			    data_asn1_decode_done				*/
/* Function called when the end of the ASN.1 'Data' entity is           */
/* detected.  Checks this against the runtime type table.		*/
/************************************************************************/

static ST_VOID data_asn1_decode_done (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("data_asn1_decode_done");
  }

/************************************************************************/
/*			     data_dec_err				*/
/* Function called when an invalid tag is encountered in the ASN.1	*/
/* 'Data' entity.  Stops the parse with a fatal error.                  */
/************************************************************************/

static ST_VOID data_dec_err (ASN1_DEC_CTXT *aCtx, ST_RET err)
  {
  MLOG_CDEC1 ("data_dec_err %d", err);
  /* NOTE: error already set. DO NOT call asn1r_set_dec_err.		*/
  /*       DO NOT set (aCtx->asn1r_pdu_dec_err).			*/
  }

/************************************************************************/
/************************************************************************/
/************************************************************************/
/* Block data to ASN.1 data section					*/
/* Assumes that the ASN.1 tools have been initialized.			*/
/************************************************************************/
/************************************************************************/
/*			ms_locl_to_asn1 				*/
/* Construct a 'data' element (ASN.1) from blocked data.                */
/* The user passes a pointer to the blocked data and the type def,	*/
/* and the number of elements in the type definition table.		*/
/*									*/
/* Assume the build tools have been initialized, use ASN.1 build tools	*/
/************************************************************************/

#ifndef MMS_LITE

ST_RET ms_locl_to_asn1 (ASN1_ENC_CTXT *aCtx, NAMED_TYPE *tptr, ST_CHAR *dptr)
  {
  if (!tptr->rt_num)			/* if len == 0			*/
    {
    MLOG_NERR0 ("Named type has no runtime elements");
    return (MVE_RT_TYPE);		/* 0 length typedef is error		*/
    }

  return (ms_local_to_asn1 (aCtx, tptr->rt_head,tptr->rt_num, dptr));
  }

#endif
       
/************************************************************************/
/*			ms_local_to_asn1 				*/
/* This function is most convenient when data is encoded in the		*/
/* middle of a larger encode.						*/
/*   If data is encoded by itself, ms_local_to_asn1_2 is more		*/
/* efficient and much easier to use.					*/
/************************************************************************/

ST_RET ms_local_to_asn1 (ASN1_ENC_CTXT *aCtx,
			SD_CONST RUNTIME_TYPE *rt_head,
			ST_INT rt_num,
			ST_CHAR *dptr)
  {
ST_INT done;
ST_RET   ret_val;
ST_INT arr_loop_buf[ASN1_MAX_LEVEL];
ST_CHAR *datptr;
SD_CONST RUNTIME_TYPE *rt_ptr;
SD_CONST RUNTIME_TYPE *rt_end;
ST_INT arr_loop_level;
ST_INT *arr_loops;

/* Make sure that "asn1r_strt_asn1_bld" was called for this context.	*/
  if (aCtx->asn1r_magic != ASN1_ENC_MAGIC_NUMBER)
    {
    MLOG_ERR0 ("ASN1_ENC_CTXT not initialized. MUST call 'asn1r_strt_asn1_bld' first.");
    return (MVE_ASN1_ENCODE_ERR);
    }

  arr_loop_level = 0;
  arr_loops = arr_loop_buf;

  rt_ptr = rt_head + rt_num -1; /* point to bottom rt element 		*/
  rt_end = rt_head;		/* head is end				*/

/* Set working data pointer to point below last data element		*/
  datptr =  dptr + rt_head->offset_to_last;

  done = SD_FALSE;
  ret_val = SD_SUCCESS;  
  while (!done && !ret_val && !aCtx->asn1r_encode_overrun)	/* while not done*/
    {
    if (ms_is_rt_prim (rt_ptr))		/* is this a primitive element?	*/
      {
      datptr -= rt_ptr->el_size;   /* Point to data for this element    */
      ret_val = wr_prim_dat (aCtx, datptr, rt_ptr);	   /* Write the primitive data*/
      }
    else				/* not a primitive element	*/
      {
      switch (rt_ptr->el_tag)
	{
	case RT_ARR_END :		/* array end, start constructor */
					/* initialize loop count	*/
          ++arr_loop_level;
          arr_loops[arr_loop_level] = rt_ptr->u.arr.num_elmnts;
	  asn1r_strt_constr (aCtx);
          datptr -= rt_ptr->el_size; /* Point to data element above	*/
	break;

	case RT_ARR_START :     		/* array start			*/
					/* if more loops, move ptr back */
          if (--arr_loops[arr_loop_level] > 0) /* if need to do next ar elmnt	*/
	    {
	    rt_ptr += rt_ptr->u.arr.num_rt_blks;
	    continue;			/* Back to loop bottom		*/
	    }
	  else
	    {
            --arr_loop_level;
	    asn1r_fin_constr (aCtx,1,CTX,DEF);	/* array done, fin the cstr*/
            datptr -= rt_ptr->el_size;
	    }
	break;

	case RT_STR_END :			/* array end, start constructor */
	  asn1r_strt_constr (aCtx);
          datptr -= rt_ptr->el_size;
	break;

	case RT_STR_START :
	  asn1r_fin_constr (aCtx,2,CTX,DEF);  	/* struct done, fin the cstr*/
          datptr -= rt_ptr->el_size;
	break;

	default :			/* unknown typedef tag		*/
          MLOG_NERR0 ("Unknown RT type tag");
	  ret_val = MVE_RT_TYPE;
	}
      } /* end if not primitive element */

/* All tags (EXCEPT array start when loopback) come here. 		*/

/* Move the rt_ptr to the next element, check for done		       	*/
    if (rt_ptr == rt_end)		/* If this was last element 	*/
      done = SD_TRUE;	
    else
      --rt_ptr;				/* move pointer to next 	*/
    }


  if (ret_val == SD_SUCCESS)
    {
    if (aCtx->asn1r_encode_overrun)	/* Check for encode overrun	*/
      ret_val = ME_ASN1_ENCODE_OVERRUN;
    }

  /* If conversion failed, log the type used for the conversion.	*/
  if (ret_val)
    {
    MLOG_NERR1 ("Local to ASN.1 data conversion error = 0x%X", ret_val);
    _log_data_type (rt_head, rt_num);
    }

  return (ret_val);
  }

/************************************************************************/
/*			wr_prim_dat					*/
/* Write a primitive data element from the user data to the message	*/
/* under construction.							*/
/************************************************************************/

static ST_RET wr_prim_dat (ASN1_ENC_CTXT *aCtx, ST_CHAR *datptr, SD_CONST RUNTIME_TYPE *rt_ptr)
  {
ST_RET ret_val;
ST_INT16 len;
MMS_BTOD btod_data;
ST_INT32 *i32_ptr;

  ret_val = SD_SUCCESS;

/* use the runtime type def table element and type to select storage	*/

  switch (rt_ptr->el_tag)
    {
    case RT_BOOL :				/* boolean			*/
      asn1r_wr_bool (aCtx,*((ST_UCHAR *)datptr));	/* write a single byte*/
    break;

    case RT_BIT_STRING : 
#if defined(_S_TEST_PROBES)
      if (_sTestProbeEnabledS("MmsConstructedBitstrings"))
        {
        ret_val = _wr_constructed_bitstring (aCtx, datptr, rt_ptr);
	goto _CONSTRUCTED_BSOS_WRITTEN;
        }
#endif

      if (rt_ptr->u.p.el_len >= 0)
        asn1r_wr_bitstr (aCtx,(ST_UCHAR *)datptr,rt_ptr->u.p.el_len); /* bits */
      else
        {	/* variable length bitstring	*/
        len = *((ST_INT16 *)datptr);
        if (len < 0 || len > abs(rt_ptr->u.p.el_len))
          {
          MLOG_NERR1 ("Invalid variable length bitstring length: %d", 
						(ST_INT) len);
          ret_val = MVE_RT_TYPE;
	  }
        else
          asn1r_wr_bitstr (aCtx,(ST_UCHAR *) (datptr + sizeof(ST_INT16)), len);
	}
    break;

    case RT_INTEGER :
      switch (rt_ptr -> u.p.el_len)	/* determine length		*/
	{
	case 1 :			/* one byte int 		*/
	  asn1r_wr_i8 (aCtx,*datptr);		/* write a single byte	*/
	break;

	case 2 :			/* two byte int 		*/
	  asn1r_wr_i16 (aCtx,*((ST_INT16 *)datptr));
	break;

	case 4 :			/* four byte integer		*/
	  asn1r_wr_i32 (aCtx,*((ST_INT32 *) datptr));
	break;

#ifdef INT64_SUPPORT
	case 8 :			/* eight byte integer		*/
	  asn1r_wr_i64 (aCtx,*((ST_INT64 *) datptr));
	break;
#endif

	default :
          MLOG_NERR0 ("Invalid integer length");
          ret_val = MVE_RT_TYPE;
	}
    break;

    case RT_UNSIGNED :
      switch (rt_ptr -> u.p.el_len)	/* determine length		*/
	{
	case 1 :			/* one byte unsigned int	*/
	  asn1r_wr_u8 (aCtx,*((ST_UCHAR *)datptr));
	break;

	case 2 :			/* two byte unsigned int	*/
	  asn1r_wr_u16 (aCtx,*((ST_UINT16 *) datptr));
	break;

	case 4 :			/* four byte unsigned integer	*/
	  asn1r_wr_u32 (aCtx,*((ST_UINT32 *) datptr));
	break;

#ifdef INT64_SUPPORT
	case 8 :			/* eight byte unsigned integer	*/
	  asn1r_wr_u64 (aCtx,*((ST_UINT64 *) datptr));
	break;
#endif

	default :
          MLOG_NERR0 ("Invalid integer length");
          ret_val = MVE_RT_TYPE;
	}
    break;

#ifdef FLOAT_DATA_SUPPORT
    case RT_FLOATING_POINT :
      if (rt_ptr -> u.p.el_len != sizeof (ST_FLOAT))
	asn1r_wr_double (aCtx,*((ST_DOUBLE *)datptr));
      else
	asn1r_wr_float (aCtx,*((ST_FLOAT *)datptr));
    break;
#endif

    case RT_OCTET_STRING :	
#if defined(_S_TEST_PROBES)
      if (_sTestProbeEnabledS("MmsConstructedOctetstrings"))
        {
        ret_val = _wr_constructed_octetstring (aCtx, datptr, rt_ptr);
	goto _CONSTRUCTED_BSOS_WRITTEN;
        }
#endif

      if (rt_ptr->u.p.el_len >= 0)
        asn1r_wr_octstr (aCtx,(ST_UCHAR *)datptr, rt_ptr->u.p.el_len);
      else
        {	/* variable length octetstring	*/
        len = *((ST_INT16 *)datptr);
        if (len < 0 || len > abs(rt_ptr->u.p.el_len))
          {
          MLOG_NERR1 ("Invalid variable length octetstring length: %d", 
						(ST_INT) len);
          ret_val = MVE_RT_TYPE;
	  }
        else
          asn1r_wr_octstr (aCtx,(ST_UCHAR *) (datptr + sizeof(ST_INT16)), len);
	}
    break;

    case RT_VISIBLE_STRING :
      if (((rt_ptr -> u.p.el_len > 0) && 
            (strlen (datptr) != (ST_UINT) rt_ptr->u.p.el_len)) ||
            (strlen (datptr) >  (ST_UINT) abs (rt_ptr->u.p.el_len)))
        {
        MLOG_ENC0 ("Data is illegal for VISIBLE_STRING type.");
        ret_val = MVE_ASN1_ENCODE_ERR;		/* say data is illegal for type	*/
        }
      else
        asn1r_wr_vstr (aCtx,datptr);
    break;

#ifdef TIME_DATA_SUPPORT
    case RT_GENERAL_TIME :
      asn1r_wr_time (aCtx,*((time_t *)datptr));
    break;
#endif

#ifdef BTOD_DATA_SUPPORT
    case RT_BINARY_TIME :
      i32_ptr = (ST_INT32 *) datptr;
      btod_data.ms = *i32_ptr;
      if (rt_ptr->u.p.el_len == 6)
        {
        btod_data.form = MMS_BTOD6;
        ++i32_ptr;
        btod_data.day = *i32_ptr;
        }
      else
        btod_data.form = MMS_BTOD4;

      asn1r_wr_btod (aCtx,&btod_data);
    break;
#endif

/* The u.p.el_len for BCD indicates the number of digits in the value.  */
/* Digits are represented as [0..9] requiring one nibble of storage     */
/* each.  Because all BCD values are sent over the wire as integers and */
/* the C programming language has no native BCD type, BCD values are    */
/* handled by MMS-EASE as integers.                                     */

    case RT_BCD:
      if (rt_ptr->u.p.el_len <= 2)
	asn1r_wr_i8 (aCtx,*datptr);		        /* write a single byte	        */
      else if (rt_ptr->u.p.el_len <= 4)
        asn1r_wr_i16 (aCtx,*((ST_INT16 *)datptr));
      else 
        asn1r_wr_i32 (aCtx,*((ST_INT32 *) datptr));
    break;

    case RT_UTC_TIME :
      {
      MMS_UTC_TIME *utc_time = (MMS_UTC_TIME *) datptr;
      asn1r_wr_utc_time (aCtx,utc_time);
      }
    break;

    case RT_UTF8_STRING :
      /* asn1r_wr_utf8 returns SD_SUCCESS or error code, so chk return	*/
      if (asn1r_wr_utf8 (aCtx,datptr,rt_ptr->u.p.el_len))
        {
        MLOG_NERR1 ("Data or len is illegal for UTF8(V)string%d.",
                    abs(rt_ptr->u.p.el_len));
        ret_val = MVE_ASN1_ENCODE_ERR;	/* say data is illegal for type	*/
        }
    break;

    default :
      MLOG_NERR0 ("Bad RT tag");
      ret_val = MVE_RT_TYPE;
    }

  asn1r_fin_prim (aCtx,rt_ptr->el_tag,CTX);	/* finish the primitive */

#if defined(_S_TEST_PROBES)
_CONSTRUCTED_BSOS_WRITTEN:
#endif

  if (ret_val)
    {
    ST_CHAR tmp_tdl [MAX_IDENT_LEN+1];
    MLOG_ERR0 ("Local data could not be converted to ASN.1");
    if (ms_runtime_to_tdl (rt_ptr, 1, tmp_tdl, MAX_IDENT_LEN) > 0)
      MLOG_CERR1 ("Data type TDL=%s", tmp_tdl);
    else
      MLOG_CERR1 ("Data type tag=%d", rt_ptr->el_tag);
    MLOG_CERR0 ("Local Data:");
    MLOG_ERRH (rt_ptr->el_size, datptr);
    }
  return (ret_val);
  }

/************************************************************************/
/************************************************************************/

#if defined(_S_TEST_PROBES)

static ST_RET _wr_constructed_bitstring (ASN1_ENC_CTXT *aCtx, ST_CHAR *datptr, 
					 SD_CONST RUNTIME_TYPE *rt_ptr)
  {
ST_INT16 len;
ST_INT16 el_len;
ST_INT j;
ST_INT extra_bits;
ST_INT numbytes;
ST_UCHAR *bitstr_data;
static ST_BOOLEAN firstTime = SD_TRUE;

  if (firstTime)
    {
    SLOGALWAYS0 ("TEST: Using constructed bitstrings");
    firstTime = SD_FALSE;
    }


  el_len = rt_ptr->u.p.el_len;
  if (el_len > 8 || (el_len < 0 && *(ST_INT16*)datptr > 8))
    {	/* bitstring longer than 8 bits	*/
    if (el_len < 0)
      {
      bitstr_data = datptr + 2;	/* point after ST_INT16 len	*/
      len = *(ST_INT16 *)datptr;
      }
    else
      {
      bitstr_data = datptr;
      len = el_len;
      }
    extra_bits = len % 8;
    numbytes   = len / 8;

    asn1r_strt_constr (aCtx);
    asn1r_strt_constr (aCtx);	/* nested constr. Should be ignored.*/
#if 0	/* DEBUG: enable this to send 1 garbage bit after last real segment.*/
        /* Should cause truncation for "variable length" bitstr		*/
	/* (error for "fixed length" bitstr). 				*/
    asn1r_wr_bitstr (aCtx, bitstr_data, 1);	/* write 1 bit bitstr*/
    asn1r_fin_prim (aCtx,BITS_CODE,UNI);
#endif
    /* encoding backward, so loop backward through bits.*/
    if (extra_bits)
      {
      asn1r_wr_bitstr (aCtx, bitstr_data+numbytes, extra_bits);
      /* NOTE: Tag is UNI class.*/
      asn1r_fin_prim (aCtx,BITS_CODE,UNI);
      }
    for (j = numbytes-1; j >= 0; j--)
      {
      asn1r_wr_bitstr (aCtx, bitstr_data+j, 8);	/* write 8 bit bitstr*/
      asn1r_fin_prim (aCtx,BITS_CODE,UNI);	/* finish the primitive */
      }
#if 0	/* DEBUG: enable this to send 1 garbage bit before first real segment.*/
	/* Should cause decode error. First segment must be multiple of 8 bits*/
    asn1r_wr_bitstr (aCtx, &onebit, 1);	/* write 1 bit bitstr*/
    asn1r_fin_prim (aCtx,BITS_CODE,UNI);
#endif
    asn1r_fin_constr (aCtx, BITS_CODE, UNI,DEF);	/* nested constr. Should be ignored.*/
    asn1r_fin_constr (aCtx, RT_BIT_STRING, CTX,DEF);	/* finish the constructor*/
    }
  else
    {
    asn1r_wr_bitstr (aCtx,(ST_UCHAR *)datptr,rt_ptr->u.p.el_len); /* bits */
    asn1r_fin_prim (aCtx, RT_BIT_STRING, CTX);	/* finish the primitive */
    }
  return (SD_SUCCESS);
  }

/************************************************************************/

static ST_RET _wr_constructed_octetstring (ASN1_ENC_CTXT *aCtx, ST_CHAR *datptr, 
					 SD_CONST RUNTIME_TYPE *rt_ptr)
  {
ST_INT16 len;
ST_INT j;
static ST_BOOLEAN firstTime = SD_TRUE;

  if (firstTime)
    {
    SLOGALWAYS0 ("TEST: Using constructed octetstrings");
    firstTime = SD_FALSE;
    }

  len = rt_ptr->u.p.el_len;
  if (len < 0)
    {
    len = *(ST_INT16 *)datptr;
    datptr += 2;	/* point after ST_INT16 len	*/
    }

  asn1r_strt_constr (aCtx);
  asn1r_strt_constr (aCtx);	/* nested constr. Should be ignored.*/
  /* encoding backward, so loop backward through bytes.*/
  for (j = len-1; j >= 0; j--)
    {
    asn1r_wr_octstr (aCtx, &datptr[j], 1);	/* write 1 byte*/
    /* NOTE: all prim octstr inside constructed octstr, must be UNI.*/
    asn1r_fin_prim (aCtx,OCT_CODE,UNI);	/* finish the primitive */
    }
  asn1r_fin_constr (aCtx,OCT_CODE, UNI,DEF);	/* nested constr. Should be ignored.*/
  asn1r_fin_constr (aCtx,RT_OCTET_STRING,CTX,DEF);	/* finish the constructor*/
  return (SD_SUCCESS);
  }

#endif

/************************************************************************/
/*			ms_local_to_asn1_easy				*/
/* If a data encode is NOT part of a bigger encode, 			*/
/* this function is more efficient and easier to use.			*/
/* NOTE: ms_local_to_asn1_2 is almost the same but its arguments are	*/
/*       more consistent with other encoding functions.			*/
/************************************************************************/
ST_UCHAR *ms_local_to_asn1_easy (SD_CONST RUNTIME_TYPE *rt_head, ST_INT rt_num,
			ST_CHAR *dptr,
                        ST_UCHAR *asn1Buf,
                        ST_INT asn1BufLen,
                        ST_INT *asn1LenOut)
  {
ST_RET ret_val;
ASN1_ENC_CTXT aCtx;
ST_UCHAR *asn1Start = NULL;	/* ptr to start of ASN1 encoded data	*/
				/* init to NULL (i.e. failure)		*/

  asn1r_strt_asn1_bld (&aCtx, asn1Buf, asn1BufLen);

  /* encode everything using new context.	*/
  ret_val = ms_local_to_asn1 (&aCtx, rt_head, rt_num, dptr);

  /* When the ASN.1 encode is complete, 'asn1_field_ptr' points 1 byte	*/
  /* ahead of the start of the message. The ASN.1 message is built from	*/
  /* back to front, so the length = (ptr to end) - (ptr to start)	*/
  if (ret_val == SD_SUCCESS)
    {
    asn1Start = aCtx.asn1r_field_ptr+1;
    *asn1LenOut = (int) ((asn1Buf + asn1BufLen) - asn1Start);
    }
  return (asn1Start);		/* ptr to start (or NULL on error)	*/
  }

/************************************************************************/
/*			ms_local_to_asn1_2				*/
/* If a data encode is NOT part of a bigger encode, 			*/
/* this function is easier to use.					*/
/************************************************************************/
ST_RET ms_local_to_asn1_2 (SD_CONST RUNTIME_TYPE *rt_head, ST_INT rt_num,
	ST_CHAR *dptr,			/* local data to encode		*/
	ST_UCHAR *buf_ptr,		/* buffer in which to encode	*/
	ST_INT buf_len,			/* buffer length		*/
	ST_UCHAR **asn1_ptr_out,	/* ptr to ptr to encoded data	*/
	ST_INT *asn1_len_out)		/* ptr to encoded length	*/
  {
ST_RET ret_val;
ASN1_ENC_CTXT aCtx;

  asn1r_strt_asn1_bld (&aCtx, buf_ptr, buf_len);

  /* encode everything using new context.	*/
  ret_val = ms_local_to_asn1 (&aCtx, rt_head, rt_num, dptr);

  if (ret_val == SD_SUCCESS)
    {
    /* Set output parameters (ptr & len).	*/
    *asn1_ptr_out = ASN1_ENC_PTR(&aCtx);
    *asn1_len_out = (int) ASN1_ENC_LEN(&aCtx);
    }
  return (ret_val);
  }

/************************************************************************/
/*			_log_data_type					*/
/* Log the data type (as TDL so average user can read it).		*/
/************************************************************************/
static ST_VOID _log_data_type (SD_CONST RUNTIME_TYPE *rt_head, ST_INT rt_num)
  {
ST_CHAR tdl_buf [500];
ST_INT tdl_len;

  tdl_len = sizeof (tdl_buf);		/* init to max allowed tdl len	*/
  tdl_len = ms_runtime_to_tdl (rt_head, rt_num, tdl_buf, tdl_len);

  if (tdl_len != 0)	/* conversion to tdl succeeded	*/
    {
    MLOG_CALWAYS1 ("Local Data Type TDL: %s", tdl_buf);	/* should be CERR1?*/
    }
  else
    {		/* conversion failed. Just log what we can.	*/
    MLOG_CALWAYS2 ("Local Runtime Type Array ptr = " S_FMT_PTR ", Num Entries = %d",
                   rt_head, rt_num);
    }
  }

