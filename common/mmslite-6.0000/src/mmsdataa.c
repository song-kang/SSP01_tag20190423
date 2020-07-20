/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2006, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : mmsdataa.c						*/
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
/* 08/24/09  JRB     22    Avoid moving constructed bstr, ostr data.	*/
/*			   Del undocumented BAD_DATA, INTERNAL errcodes.*/
/* 07/23/09  JRB     21    Chk new asn1r_bitstr_truncated flag.		*/
/* 07/23/09  JRB     20    Fixed octet string overwrite hole.		*/
/* 03/22/06  EJV     19    RT_GENERAL_TIME: cast DATA_SRC to (time_t *).*/
/* 10/29/04  EJV     18    do_local_to_asn1_aa: corr rt range check,	*/
/*			     use new safer ms_is_rt_prim.		*/
/* 03/31/03  JRB     17    Add UTF8string support (see RT_UTF8_STRING).	*/
/*			   asn1r_get_bitstr: add max_bits arg.		*/
/* 03/04/02  JRB     16    Use SD_CONST on ALL "RUNTIME_TYPE *" vars.	*/
/* 12/20/01  JRB     15    Converted to use ASN1R (re-entrant ASN1)	*/
/* 11/14/01  EJV     14    Added support for new MMS type UtcTime:	*/
/*			   Added get_utc_time_prim_aa func;		*/
/*			   assign_tags_aa: added case for RT_UTC_TIME;	*/
/*			   wr_prim_dat_aa: added case for TR_UTC_TIME;	*/
/* 09/26/00  JRB     13    Add SD_CONST on ms_local_to*, ms_asn1_to*	*/
/* 09/22/00  DSF     12    Truncate extended prim bitstrings		*/
/* 09/13/99  MDE     10    Added SD_CONST modifiers			*/
/* 03/11/98  MDE     09    Removed NEST_RT_TYPES			*/
/* 12/10/97  MDE     08    Cast for call to get_i8			*/
/* 09/05/97  MDE     07    Minor logging changes			*/
/* 08/15/97  MDE     06    BTOD handling changes			*/
/* 08/14/97  MDE     05    Fixed get_bcd_prim (ST_CHAR -> ST_INT8)	*/
/* 07/02/97  MDE     04    Check for ASN.1 encode overrun		*/
/* 06/19/97  MDE     03    Added 64 bit integer support			*/
/* 06/10/97  MDE     02    Removed non-aa functions (use mmsdata.c too)	*/
/* 06/09/97  MDE     01    Modified Runtime Type handling, other minor	*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "glbsem.h"
#include "mmsdefs.h"
#include "mms_pvar.h"
#include "mms_vvar.h"
#include "mmslog.h"
#include "asn1defs.h"

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/
/************************************************************************/
/* ALTERNATE ACCESS SUPPORT FUNCTIONS					*/
/************************************************************************/
/************************************************************************/
/*	    DATA CONVERSION TO AND FROM LOCAL REPRESENTATION		*/
/************************************************************************/

static ST_VOID assign_tags_aa (ASN1_DEC_CTXT *ac);
static ST_RET wr_prim_dat_aa (ASN1_ENC_CTXT *ac);
static ST_VOID get_bstr_cstr_aa   (ASN1_DEC_CTXT *ac); 	/* for constructed bitstrings	*/
static ST_VOID bs_cstr_done_aa    (ASN1_DEC_CTXT *ac);
static ST_VOID get_ostr_cstr_aa   (ASN1_DEC_CTXT *ac); 	/* for constructed octetstrings */
static ST_VOID os_cstr_done_aa    (ASN1_DEC_CTXT *ac);
static ST_VOID get_bool_prim_aa   (ASN1_DEC_CTXT *ac); 	/* for boolean primitives	*/
static ST_VOID get_bitstr_prim_aa (ASN1_DEC_CTXT *ac); 	/* for bitstring primitives	*/
static ST_VOID get_int_prim_aa    (ASN1_DEC_CTXT *ac); 	/* for integer primitives	*/
static ST_VOID get_uint_prim_aa   (ASN1_DEC_CTXT *ac); 	/* for unsigned integers	*/
static ST_VOID get_float_prim_aa  (ASN1_DEC_CTXT *ac); 	/* for float primitives 	*/
static ST_VOID get_octstr_prim_aa (ASN1_DEC_CTXT *ac); 	/* for octet string primitives	*/
static ST_VOID get_vstr_prim_aa   (ASN1_DEC_CTXT *ac); 	/* for visible string primitives*/
static ST_VOID get_gtime_prim_aa  (ASN1_DEC_CTXT *ac); 	/* for generalized time 	*/
static ST_VOID get_btime_prim_aa  (ASN1_DEC_CTXT *ac); 	/* for binary time primitives	*/
static ST_VOID get_bcd_prim_aa    (ASN1_DEC_CTXT *ac); 	/* for binary coded decimal	*/
static ST_VOID get_utc_time_prim_aa (ASN1_DEC_CTXT *ac);	/* for UTC time primitives	*/
static ST_VOID get_utf8_prim_aa (ASN1_DEC_CTXT *ac);	/* for UTF8string primitives	*/
static ST_VOID arr_start_aa	    (ASN1_DEC_CTXT *ac);
static ST_VOID str_start_aa	    (ASN1_DEC_CTXT *ac);
static ST_VOID arr_done_fun_aa    (ASN1_DEC_CTXT *ac);
static ST_VOID str_done_fun_aa    (ASN1_DEC_CTXT *ac);
static ST_VOID data_asn1_decode_done_aa (ASN1_DEC_CTXT *ac);
static ST_VOID data_dec_err_aa (ASN1_DEC_CTXT *aCtx, ST_RET); 	/* for type definition error	*/


static ST_RET do_asn1_to_local_aa (RT_AA_CTRL *runtime_aa,
			       ST_INT num_rt_aa,
			       ST_UCHAR *asn1ptr,
			       ST_INT asn1len,
			       ST_CHAR *dptr);

static ST_RET do_local_to_asn1_aa (ASN1_ENC_CTXT *aCtx, RT_AA_CTRL *runtime_aa,
			       ST_INT num_rt_aa,
			       ST_CHAR *dptr);


/************************************************************************/
/************************************************************************/

static SD_CONST RUNTIME_TYPE *g_rt_ptr; 
static RT_AA_CTRL *g_rt_aa;
static RT_AA_CTRL *g_rt_aa_head;
static RT_AA_CTRL *g_rt_aa_end;
static ST_CHAR *g_datptr;		    



/* This variable is used to keep track of the current offset for encode	*/
/* and decode when the alternate access is packed.			*/
static ST_INT g_current_offset;


/* When not packed, the offset to the destination is simply the offset	*/
/* to the data as calculated when the runtime type is 'unrolled'.	*/
/* When packed, we keep a running destination offset that is added to 	*/
/* the base address to calculate the destination			*/

#define DATA_DEST (m_alt_acc_packed ?\
                     (g_datptr + g_current_offset) :\
                     (g_datptr + g_rt_aa->offset_to_data))

/* When a data element has been decoded into local format, the running 	*/
/* offset is incremented.						*/
/* Before a data element is encoded, the running offset is decremented.	*/
/* Note that this is used only when local data is 'packed', and that	*/
/* the 'el_size' is that which is calculated when the RTAA is created.	*/

#define INC_CURR_OFFSET (g_current_offset += g_rt_aa->el_size)
#define DEC_CURR_OFFSET (g_current_offset -= g_rt_aa->el_size)

/* When not packed, the offset to the source data is simply the offset	*/
/* to the data as calculated when the runtime type is 'unrolled'.	*/
/* When packed, we keep a running destination offset that is added to 	*/
/* the base address to calculate the address of the data element	*/

#define DATA_SRC  (m_alt_acc_packed ?\
		     (g_datptr + g_current_offset) :\
		     (g_datptr + g_rt_aa->offset_to_data))


/************************************************************************/
/************************************************************************/
/************************************************************************/
/*			ms_asn1_to_locl_aa 				*/
/* Similar to ms_asn1_to_locl, except that this one handles alternate	*/
/* access. The assumption is that local data is layed out per the 	*/
/* original type, and only the components specified by the AA are	*/
/* updated.								*/
/************************************************************************/

#ifndef MMS_LITE

ST_RET ms_asn1_to_locl_aa (NAMED_TYPE *tptr,
                          ALT_ACCESS *alt_acc,
                          ST_UCHAR *asn1ptr, /* pointer to ASN.1 'Data'    */
                          ST_INT asn1len,    /* length of ASN.1 'Data'     */
                          ST_CHAR *dptr)     /* local data buffer pointer  */
  {
  if (!tptr->rt_num)		/* check to make sure some components	*/
    {
    MLOG_NERR0 ("Named type has no runtime elements");
    return (MVE_RT_TYPE);		/* 0 length typedef is error		*/
    }

  return (ms_asn1_to_local_aa (tptr->rt_head,tptr->rt_num, alt_acc,
                                             asn1ptr,asn1len,dptr));
  }

#endif

/************************************************************************/
/*			ms_asn1_to_local_aa				*/
/************************************************************************/

ST_RET ms_asn1_to_local_aa (SD_CONST RUNTIME_TYPE *rt_head,
                           ST_INT rt_num,
                           ALT_ACCESS *alt_acc,
                           ST_UCHAR *asn1ptr,
                           ST_INT asn1len,
                           ST_CHAR *dptr)
  {
ST_INT num_rt_aa;
RT_AA_CTRL *runtime_aa;
ST_RET ret;

/* Create an RTAA that selects the appropriate elements			*/
  ret = _ms_mk_rt_aa (rt_head, rt_num, alt_acc, &runtime_aa, &num_rt_aa);
  if (ret)
    {
    return (ret);
    }
/* Do the conversion!							*/
  S_LOCK_COMMON_RESOURCES ();
  ret = do_asn1_to_local_aa (runtime_aa, num_rt_aa, asn1ptr, asn1len, dptr);
  chk_free (runtime_aa);
  S_UNLOCK_COMMON_RESOURCES ();
  return (ret);
  }

/************************************************************************/
/*			do_asn1_to_local_aa 				*/
/************************************************************************/

static ST_RET do_asn1_to_local_aa (RT_AA_CTRL *runtime_aa,
                               ST_INT num_rt_aa,
                               ST_UCHAR *asn1ptr,
                               ST_INT asn1len,
                               ST_CHAR *dptr)
  {
ASN1_DEC_CTXT localDecCtx;	/* For readability, use "aCtx" to access this.*/
ASN1_DEC_CTXT *aCtx = &localDecCtx;

  memset (aCtx, 0, sizeof (ASN1_DEC_CTXT));	/* CRITICAL: start clean.	*/

/* Set global working variables	   					*/
  g_rt_aa = runtime_aa;
  g_rt_aa_head = g_rt_aa;
  g_rt_aa_end = g_rt_aa + num_rt_aa;

/* Used for 'packed' decode only					*/
  g_current_offset = 0;

/* Get working data conversion pointers					*/
  g_datptr = dptr; 

/* Set up ASN.1 decode tools						*/
  aCtx->asn1r_decode_method = ASN1_TAG_METHOD;		/* select tag method			*/
  aCtx->asn1r_decode_done_fun = asn1r_done_err;/* not legal to be done at this time	*/
  aCtx->asn1r_err_fun = data_dec_err_aa;	/* parse error => call to data_dec_err_aa	*/

  assign_tags_aa (aCtx);		/* set up tags allowed by 1st rt_block	*/
  asn1r_decode_asn1 (aCtx, asn1ptr,asn1len);/* decode entire ASN.1 'Data' entity    */

  if (aCtx->asn1r_pdu_dec_err)
    MLOG_NERR0 ("ASN.1 to Local data conversion error");

  return (aCtx->asn1r_pdu_dec_err);	/* Just return ASN.1 decode error*/
  }

/************************************************************************/
/*			     assign_tags_aa				*/
/* Function to assign tags to the ASN1DE tools based on what's expected */
/* in the runtime_type definition table.  At the beginning of the 'Data'*/
/* entity, a DataAccessError type is allowed.  WARNING: This function	*/
/* requires that the current rt_block NOT be an end-of-array or end-of- */
/* structure rt_block.	If so, it will return an error. 		*/
/************************************************************************/

static ST_VOID assign_tags_aa (ASN1_DEC_CTXT *aCtx)
  {
/* Initialize the runtime type pointer					*/
  g_rt_ptr = g_rt_aa->rt;  

  if (g_rt_aa < g_rt_aa_end)		/* if elements to look at	*/
    {
    switch (g_rt_ptr->el_tag)
      {
      case RT_ARR_START :
	ASN1R_TAG_ADD (aCtx, CTX|CONSTR,1,arr_start_aa); /* expecting an array cstr	*/
      break;

      case RT_STR_START :
	ASN1R_TAG_ADD (aCtx, CTX|CONSTR,2,str_start_aa); /* expecting an struct cstr	*/
      break;

      case RT_BOOL :
	ASN1R_TAG_ADD (aCtx, CTX,3,get_bool_prim_aa);	
      break;

      case RT_BIT_STRING :			
	ASN1R_TAG_ADD (aCtx, CTX,4,get_bitstr_prim_aa);
	ASN1R_TAG_ADD (aCtx, CTX|CONSTR,4,get_bstr_cstr_aa);
      break;

      case RT_INTEGER :			
	ASN1R_TAG_ADD (aCtx, CTX,5,get_int_prim_aa);
      break;

      case RT_UNSIGNED : 			
	ASN1R_TAG_ADD (aCtx, CTX,6,get_uint_prim_aa);
      break;

#ifdef FLOAT_DATA_SUPPORT
      case RT_FLOATING_POINT : 		
	ASN1R_TAG_ADD (aCtx, CTX,7,get_float_prim_aa);
      break;
#endif

      case RT_OCTET_STRING : 		
	ASN1R_TAG_ADD (aCtx, CTX,9,get_octstr_prim_aa);
	ASN1R_TAG_ADD (aCtx, CTX|CONSTR,9,get_ostr_cstr_aa);
      break;

      case RT_VISIBLE_STRING : 		
	ASN1R_TAG_ADD (aCtx, CTX,10,get_vstr_prim_aa);
      break;

#ifdef TIME_DATA_SUPPORT
      case RT_GENERAL_TIME : 		
	ASN1R_TAG_ADD (aCtx, CTX,11,get_gtime_prim_aa);
      break;
#endif

#ifdef BTOD_DATA_SUPPORT
      case RT_BINARY_TIME : 		
	ASN1R_TAG_ADD (aCtx, CTX,12,get_btime_prim_aa);
      break;
#endif

      case RT_BCD : 			
	ASN1R_TAG_ADD (aCtx, CTX,13,get_bcd_prim_aa);
      break;

      case RT_UTC_TIME :
	ASN1R_TAG_ADD (aCtx, CTX,17,get_utc_time_prim_aa);
      break;

      case RT_UTF8_STRING :
	ASN1R_TAG_ADD (aCtx, CTX,g_rt_ptr->el_tag,get_utf8_prim_aa);
      break;

      case RT_ARR_END :			/* array done			*/
	aCtx->asn1r_c_done_fun[aCtx->asn1r_msg_level] = arr_done_fun_aa; /* the arr cstr must be done*/
      break;

      case RT_STR_END :			/* structure done		*/
	aCtx->asn1r_c_done_fun[aCtx->asn1r_msg_level] = str_done_fun_aa; /* the str cstr must be done*/
      break;

      default : 			/* should not be any other tag	*/
        MLOG_NERR0 ("Bad tag in runtime type");
	asn1r_set_dec_err (aCtx, MVE_DATA_CONVERT);
      break;
      }
    }
  else					/* no more elements to do	*/
    {
    aCtx->asn1r_decode_done = SD_TRUE;			/* terminate decode		*/
    aCtx->asn1r_decode_done_fun = data_asn1_decode_done_aa; /* ok to be done now		*/
    }
  }


/************************************************************************/
/*			   arr_start_aa					*/
/* This is the function called when an array cstr is starting		*/
/************************************************************************/

static ST_VOID arr_start_aa (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("arr_start_aa");

  INC_CURR_OFFSET;
  g_rt_aa++;			/* point to next runtime AA element	*/
  assign_tags_aa (aCtx);		/* setup to get next element		*/
  }


/************************************************************************/
/*			   arr_done_fun_aa 				*/
/* This is the cstr done function called when an array cstr is done	*/
/************************************************************************/

static ST_VOID arr_done_fun_aa (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("arr_done_fun_aa");

  INC_CURR_OFFSET;
  g_rt_aa++;			/* point to next runtime AA element	*/
  assign_tags_aa (aCtx);		/* setup to get next element		*/
  }


/************************************************************************/
/*			   str_start_aa					*/
/* This is the function called when an struct cstr is starting		*/
/************************************************************************/

static ST_VOID str_start_aa (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("str_start_aa");

  INC_CURR_OFFSET;
  g_rt_aa++;			/* point to next runtime AA element	*/
  assign_tags_aa (aCtx);		/* setup to get next element		*/
  }

/************************************************************************/
/*			   str_done_fun_aa 				*/
/* This is the cstr done function called when an struct cstr is done	*/
/************************************************************************/

static ST_VOID str_done_fun_aa (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("str_done_fun_aa");

  INC_CURR_OFFSET;
  g_rt_aa++;			/* point to next runtime AA element	*/
  assign_tags_aa (aCtx);		/* setup to get next element		*/
  }


/************************************************************************/
/*			   get_bstr_cstr_aa				*/
/* State function to extract a boolean value from an asn1 data element. */
/************************************************************************/

static ST_VOID get_bstr_cstr_aa (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("get_bstr_cstr_aa");

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = bs_cstr_done_aa;
  /* Pass different data pointer for fixed len or variable len bitstring.*/
  if (g_rt_ptr->u.p.el_len >= 0)	/* fixed length		*/
    asn1r_get_bitstr_cstr (aCtx,abs (g_rt_ptr->u.p.el_len), (ST_UCHAR *) DATA_DEST);
  else					/* variable length	*/
    asn1r_get_bitstr_cstr (aCtx,abs (g_rt_ptr->u.p.el_len), (ST_UCHAR *) DATA_DEST + sizeof (ST_INT16));
  }

/************************************************************************/
/*			bs_cstr_done_aa					*/
/* This function is called when an bit string constructor is 		*/
/* completed OK. Need to reset the decode state machine.		*/
/************************************************************************/

static ST_VOID bs_cstr_done_aa (ASN1_DEC_CTXT *aCtx)
  {
ST_INT abs_len;
ST_INT abs_count;

  abs_count = abs (g_rt_ptr->u.p.el_len);
  abs_len = CALC_BIT_LEN (abs_count);

/* determine whether octect string is fixed or variable length		*/

  if (g_rt_ptr->u.p.el_len >=0)	/* fixed length				*/
    {
    if (aCtx->asn1r_bitstr_truncated || aCtx->asn1r_bitcount != abs_count)
      {
      MLOG_NERR0 ("Fixed length bit string mismatch");
      asn1r_set_dec_err (aCtx, MVE_DATA_CONVERT);
      }
    }
  else		/* variable length bit string				*/
    {
    /* Set length.	*/
    *((ST_INT16 *)DATA_DEST) = (ST_INT16) aCtx->asn1r_bitcount;
    }

  INC_CURR_OFFSET;
  g_rt_aa++;			/* point to next runtime AA element	*/
  assign_tags_aa (aCtx);		/* Assign tags for next data elt	*/
  }


/************************************************************************/
/*			   get_ostr_cstr_aa				*/
/* State function to extract an octet string from an asn1 data element. */
/************************************************************************/

static ST_VOID get_ostr_cstr_aa (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("get_ostr_cstr_aa");

  aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = os_cstr_done_aa;
  /* Pass different data pointer for fixed len or variable len octetstring.*/
  if (g_rt_ptr->u.p.el_len >= 0)	/* fixed length		*/
    asn1r_get_octstr_cstr (aCtx, abs (g_rt_ptr->u.p.el_len), (ST_UCHAR *) DATA_DEST);
  else					/* variable length	*/
    asn1r_get_octstr_cstr (aCtx, abs (g_rt_ptr->u.p.el_len), (ST_UCHAR *) DATA_DEST + sizeof (ST_INT16));
  }

/************************************************************************/
/*			os_cstr_done_aa					*/
/* This function is called when an octet string constructor is 		*/
/* completed OK. Need to reset the decode state machine.		*/
/************************************************************************/

static ST_VOID os_cstr_done_aa (ASN1_DEC_CTXT *aCtx)
  {
ST_INT abs_len;

  abs_len = abs (g_rt_ptr->u.p.el_len);

/* determine whether octect string is fixed or variable length		*/

  if (g_rt_ptr->u.p.el_len >=0)	/* fixed length				*/
    {
    if (aCtx->asn1r_octetcount != abs_len)
      {
      MLOG_NERR0 ("Fixed length octet string mismatch");
      asn1r_set_dec_err (aCtx, MVE_DATA_CONVERT);
      }
    }
  else		/* variable length octet string				*/
    {
    /* Set length.	*/
    *((ST_INT16 *)DATA_DEST) = (ST_INT16) aCtx->asn1r_octetcount;
    }

  INC_CURR_OFFSET;
  g_rt_aa++;			/* point to next runtime AA element	*/
  assign_tags_aa (aCtx);		/* Assign tags for next data elt	*/
  }


/************************************************************************/
/*			   get_bool_prim_aa				*/
/* State function to extract a boolean prim from an asn1 data element.	*/
/************************************************************************/

static ST_VOID get_bool_prim_aa   (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("get_bool_prim_aa");

  if (asn1r_get_bool (aCtx, (ST_UCHAR *)DATA_DEST))	/* Get the boolean length	*/
    asn1r_set_dec_err (aCtx, MVE_DATA_CONVERT);		/* checked by get function	*/

  INC_CURR_OFFSET;
  g_rt_aa++;			/* point to next runtime AA element	*/
  assign_tags_aa (aCtx);		/* Assign tags for next data elt	*/
  }

/************************************************************************/
/*			   get_bitstr_prim_aa				*/
/* State function to extract an bitstring prim from an asn1 data element*/
/************************************************************************/

static ST_VOID get_bitstr_prim_aa    (ASN1_DEC_CTXT *aCtx)
  {
ST_INT abs_len;
ST_INT abs_count;

  MLOG_CDEC0 ("get_bitstr_prim_aa");

  abs_count = abs (g_rt_ptr->u.p.el_len);
  abs_len = CALC_BIT_LEN (abs_count);

/* determine whether bitstring is fixed or variable length		*/
  if (g_rt_ptr->u.p.el_len >= 0)
    {
    if (asn1r_get_bitstr (aCtx, (ST_UCHAR *)DATA_DEST, abs_count))
      {
      MLOG_CDEC0 ("Bitstring decode failed");
      asn1r_set_dec_err (aCtx,MVE_DATA_CONVERT);
      }
    else if (aCtx->asn1r_bitstr_truncated || aCtx->asn1r_bitcount != abs_count)
      {
      /* For 'fixed length' bitstr, length must match expected length.	*/
      /* If bitstr truncated, this implies length DID NOT match.	*/
      MLOG_CDEC0 ("Bitstring length incorrect");
      asn1r_set_dec_err (aCtx, MVE_DATA_CONVERT);
      }
    }
  else		/* variable length octet string				*/
    {
    if (asn1r_get_bitstr (aCtx, (ST_UCHAR *)DATA_DEST + sizeof (ST_INT16), abs_count))
      asn1r_set_dec_err (aCtx, MVE_DATA_CONVERT);
    else
      {
      *((ST_INT16 *)DATA_DEST) = (ST_INT16) aCtx->asn1r_bitcount;
      }
    }

  INC_CURR_OFFSET;
  g_rt_aa++;			/* point to next runtime AA element	*/
  assign_tags_aa (aCtx);		/* Assign tags for next data elt	*/
  }

/************************************************************************/
/*			   get_int_prim_aa 				*/
/* State function to extract an integer prim from an asn1 data element. */
/************************************************************************/

static ST_VOID get_int_prim_aa    (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("get_int_prim_aa");

   switch (g_rt_ptr -> u.p.el_len)	/* determine length	       */
     {
     case 1 :			       /* one byte int		       */
       if (asn1r_get_i8 (aCtx, (ST_INT8 *)(DATA_DEST))) /* write a single byte     */
	 asn1r_set_dec_err (aCtx, MVE_DATA_CONVERT);
     break;

     case 2 :			       /* two byte int		       */
       if (asn1r_get_i16 (aCtx, (ST_INT16 *)DATA_DEST)) /* read 16 bit integer	       */
	 asn1r_set_dec_err (aCtx, MVE_DATA_CONVERT);
     break;

     case 4 :			      /* four byte integer	       */
       if (asn1r_get_i32 (aCtx, (ST_INT32 *)DATA_DEST))/* read 4 bytes		       */
	 asn1r_set_dec_err (aCtx, MVE_DATA_CONVERT);
     break;

#ifdef INT64_SUPPORT
     case 8 :			      /* eight byte integer	       */
       if (asn1r_get_i64 (aCtx, (ST_INT64 *)DATA_DEST)) /* read 8 bytes		       */
	 asn1r_set_dec_err (aCtx, MVE_DATA_CONVERT);
     break;
#endif

     default :
       MLOG_NERR0 ("Invalid integer size in RT");
       asn1r_set_dec_err (aCtx, MVE_DATA_CONVERT);	     /* no other lengths are valid   	*/
     }

  INC_CURR_OFFSET;
  g_rt_aa++;			/* point to next runtime AA element	*/
  assign_tags_aa (aCtx);		/* Assign tags for next data elt	*/
  }

/************************************************************************/
/*			   get_uint_prim_aa				*/
/* State function to extract an unsigned integer prim from an asn1 data */
/* element.								*/
/************************************************************************/

static ST_VOID get_uint_prim_aa   (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("get_uint_prim_aa");

   switch (g_rt_ptr -> u.p.el_len) 	/* determine length	       */
     {
     case 1 :			       /* one byte int		       */
       if (asn1r_get_u8 (aCtx, (ST_UCHAR *)DATA_DEST)) /* write a single byte	       */
	    asn1r_set_dec_err (aCtx, MVE_DATA_CONVERT);
     break;

     case 2 :			       /* two byte int		       */
       if (asn1r_get_u16 (aCtx, (ST_UINT16 *)DATA_DEST)) /* read 16 bit integer 	*/
         {
	 asn1r_set_dec_err (aCtx, MVE_DATA_CONVERT);
         }   
     break;

     case 4 :			      /* four byte integer	       */
       if (asn1r_get_u32 (aCtx, (ST_UINT32 *)DATA_DEST))/* read 4 bytes		       */
	 asn1r_set_dec_err (aCtx, MVE_DATA_CONVERT);
     break;

#ifdef INT64_SUPPORT
     case 8 :			      /* eight byte integer	       */
       if (asn1r_get_u64 (aCtx, (ST_UINT64 *)DATA_DEST))/* read 8 bytes		       */
	 asn1r_set_dec_err (aCtx, MVE_DATA_CONVERT);
     break;
#endif
     default :
       MLOG_NERR0 ("Invalid integer size in RT");
       asn1r_set_dec_err (aCtx, MVE_DATA_CONVERT);	     /* no other lengths are valid   */
     }

  INC_CURR_OFFSET;
  g_rt_aa++;			/* point to next runtime AA element	*/
  assign_tags_aa (aCtx);		/* Assign tags for next data elt	*/
  }

/************************************************************************/
/*			   get_float_prim_aa				*/
/* State function to extract a floating point primitive from an asn1	*/
/* data element.							*/
/************************************************************************/

#ifdef FLOAT_DATA_SUPPORT

static ST_VOID get_float_prim_aa  (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("get_float_prim_aa");

  if (g_rt_ptr -> u.p.el_len == 8)
    {
    if (asn1r_get_double (aCtx, (ST_DOUBLE *)DATA_DEST))
      asn1r_set_dec_err (aCtx, MVE_DATA_CONVERT);
    }
  else if (g_rt_ptr -> u.p.el_len == 4)
    {
    if (asn1r_get_float (aCtx, (ST_FLOAT *)DATA_DEST))
      asn1r_set_dec_err (aCtx, MVE_DATA_CONVERT);
    }
  else
    {
    MLOG_NERR0 ("Bad float size in RT");
    asn1r_set_dec_err (aCtx, MVE_DATA_CONVERT);
    }

  INC_CURR_OFFSET;
  g_rt_aa++;			/* point to next runtime AA element	*/
  assign_tags_aa (aCtx);		/* Assign tags for next data elt	*/
  }

#endif

/************************************************************************/
/*			   get_octstr_prim_aa				*/
/* State function to extract an octet string primitive from an asn1	*/
/* data element.							*/
/************************************************************************/

static ST_VOID get_octstr_prim_aa (ASN1_DEC_CTXT *aCtx)
  {
ST_INT abs_len;
ST_INT var_len;	/* variable length octetstring decoded length	*/

  MLOG_CDEC0 ("get_octstr_prim_aa");

  abs_len = abs (g_rt_ptr->u.p.el_len);

  if (g_rt_ptr->u.p.el_len >= 0)	/* fixed length	*/
    {
    if (asn1r_get_octstr_fixlen (aCtx, (ST_UCHAR *) DATA_DEST, abs_len))
      {
      MLOG_NERR0 ("Fixed length octet string decode error");
      asn1r_set_dec_err (aCtx, MVE_DATA_CONVERT);
      return;
      }
    }
  else					/* variable length	*/
    {
    if (asn1r_get_octstr_varlen (aCtx,(ST_UCHAR *) (DATA_DEST + sizeof (ST_INT16)), abs_len, &var_len))
      {
      MLOG_NERR0 ("Variable length octet string decode error");
      asn1r_set_dec_err (aCtx,MVE_DATA_CONVERT);
      return;
      }
    else
      {
      *((ST_INT16 *)DATA_DEST) = (ST_INT16) var_len;
      }
    }

  INC_CURR_OFFSET;
  g_rt_aa++;			/* point to next runtime AA element	*/
  assign_tags_aa (aCtx);		/* Assign tags for next data elt	*/
  }

/************************************************************************/
/*			   get_vstr_prim_aa				*/
/* State function to extract a visible string primitive from an asn1	*/
/* data element.							*/
/************************************************************************/

static ST_VOID get_vstr_prim_aa   (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("get_vstr_prim_aa");

/* When the u.p.el_len is positive the v-string is of fixed length 	*/
/* meaning that the aCtx->asn1r_elmnt_len has to be equal.  It is ok for the 	*/
/* u.p.el_len to be negative.  When this is encountered it means that 	*/
/* the v_string	is of variable length, the aCtx->asn1r_elmnt_len has to be <= 	*/
/* u.p.el_len to be valid.						*/

  if (((g_rt_ptr -> u.p.el_len > 0) && 
          (aCtx->asn1r_elmnt_len != g_rt_ptr->u.p.el_len)) ||
          (aCtx->asn1r_elmnt_len > abs (g_rt_ptr -> u.p.el_len)) || 
          (asn1r_get_vstr (aCtx, DATA_DEST)))
    {
    MLOG_NERR0 ("Vstring size or content problem");
    asn1r_set_dec_err (aCtx, MVE_DATA_CONVERT);
    }
					/* v-strings are null term'd	*/

  INC_CURR_OFFSET;
  g_rt_aa++;			/* point to next runtime AA element	*/
  assign_tags_aa (aCtx);		/* Assign tags for next data elt	*/
  }

/************************************************************************/
/*			   get_utf8_prim_aa				*/
/* State function to extract a Unicode UTF8string primitive from an asn1*/
/* data element.							*/
/************************************************************************/

static ST_VOID get_utf8_prim_aa   (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("get_utf8_prim_aa");

  if (asn1r_get_utf8 (aCtx,DATA_DEST,g_rt_ptr->u.p.el_len))
    {
    MLOG_NERR0 ("Unicode UTF8string size or content problem");
    asn1r_set_dec_err (aCtx,MVE_DATA_CONVERT);
    }

  INC_CURR_OFFSET;
  g_rt_aa++;			/* point to next runtime AA element	*/
  assign_tags_aa (aCtx);		/* Assign tags for next data elt	*/
  }

/************************************************************************/
/*			   get_gtime_prim_aa				*/
/* State function to extract a generalized time primitive from an asn1	*/
/* data element.							*/
/************************************************************************/

#ifdef TIME_DATA_SUPPORT

static ST_VOID get_gtime_prim_aa  (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("get_gtime_prim_aa");

  if (asn1r_get_time (aCtx, (time_t *)DATA_DEST))
    asn1r_set_dec_err (aCtx, MVE_DATA_CONVERT);

  INC_CURR_OFFSET;
  g_rt_aa++;			/* point to next runtime AA element	*/
  assign_tags_aa (aCtx);		/* Assign tags for next data elt	*/
  }

#endif

/************************************************************************/
/*			   get_btime_prim_aa				*/
/* State function to extract a binary time primitive from an asn1 data	*/
/* element.								*/
/************************************************************************/

#ifdef BTOD_DATA_SUPPORT

static ST_VOID get_btime_prim_aa  (ASN1_DEC_CTXT *aCtx)
  {
MMS_BTOD btod_data;
ST_INT32 *i32_ptr;

  MLOG_CDEC0 ("get_btime_prim_aa");

  if (asn1r_get_btod (aCtx, &btod_data))
    asn1r_set_dec_err (aCtx, MVE_DATA_CONVERT);
  else if (btod_data.form != g_rt_ptr->u.p.el_len)
    {
    MLOG_NERR0 ("BTOD form mismatch");
    asn1r_set_dec_err (aCtx, MVE_DATA_CONVERT);
    }
  else
    {
    i32_ptr = (ST_INT32 *) DATA_DEST;
    *i32_ptr = btod_data.ms;
    if (btod_data.form == MMS_BTOD6)
      {
      ++i32_ptr;
      *i32_ptr = btod_data.day;
      }
    }
  INC_CURR_OFFSET;
  g_rt_aa++;			/* point to next runtime AA element	*/
  assign_tags_aa (aCtx);		/* Assign tags for next data elt	*/
  }

#endif

/************************************************************************/
/*			   get_bcd_prim_aa 				*/
/* State function to extract a binary coded decimal primitive from an	*/
/* asn1 data element.							*/
/************************************************************************/

static ST_VOID get_bcd_prim_aa  (ASN1_DEC_CTXT *aCtx)
  {
ST_INT32 range;
ST_INT8	 btemp;
ST_INT16 stemp;
ST_INT32 ltemp;
ST_INT   i;

  MLOG_CDEC0 ("get_bcd_prim_aa");
  range = 0;

/* we have to calculate the legal range of digits this bcd type will    */
/* fit in.                                                              */

   for (i = 0; i < g_rt_ptr->u.p.el_len; i++)
      range = (range * 10) + 9;

   if (g_rt_ptr -> u.p.el_len <= 2)	/* determine length	       */
     {
     if (asn1r_get_i8 (aCtx, &btemp))	        /* get a two nibble value       */
       asn1r_set_dec_err (aCtx, MVE_DATA_CONVERT);
     if ((btemp > (ST_CHAR)range) ||  /* is it in range		*/
          (btemp < 0))
       {
       MLOG_NERR0 ("BCD value out of range");
       asn1r_set_dec_err (aCtx, MVE_DATA_CONVERT);
       }
     else
       memcpy (DATA_DEST, &btemp, sizeof (ST_CHAR)); /* copy good data	*/
     }
   else if (g_rt_ptr -> u.p.el_len <= 4)
     {
     if (asn1r_get_i16 (aCtx, &stemp)) 		/* read 4 nibble BCD	       */
       asn1r_set_dec_err (aCtx, MVE_DATA_CONVERT);
     if ((stemp > (ST_INT16) range) || (stemp < 0))
       {
       MLOG_NERR0 ("BCD value out of range");
       asn1r_set_dec_err (aCtx, MVE_DATA_CONVERT);
       }
     else
       memcpy (DATA_DEST, &stemp, sizeof (ST_INT16)); /* copy good data	*/
     }
   else if (g_rt_ptr -> u.p.el_len <= 8)
     {
     if (asn1r_get_i32 (aCtx, &ltemp))		/* read 4 bytes		       */
       asn1r_set_dec_err (aCtx, MVE_DATA_CONVERT);
     if ((ltemp > range) || (ltemp < 0))
       {
       MLOG_NERR0 ("BCD value out of range");
       asn1r_set_dec_err (aCtx, MVE_DATA_CONVERT);
       }
     else
       memcpy (DATA_DEST, &ltemp, sizeof (ST_INT32)); /* copy good data	*/
     }
   else
    asn1r_set_dec_err (aCtx, MVE_DATA_CONVERT);	     /* no other lengths are valid   	*/

  INC_CURR_OFFSET;
  g_rt_aa++;			/* point to next runtime AA element	*/
  assign_tags_aa (aCtx);		/* Assign tags for next data elt	*/
  }

/************************************************************************/
/*			   get_utc_time_prim_aa				*/
/* State function to extract a UTC time primitive from an asn1 data	*/
/* element.								*/
/************************************************************************/

static ST_VOID get_utc_time_prim_aa  (ASN1_DEC_CTXT *aCtx)
  {
MMS_UTC_TIME utc_time;

  MLOG_CDEC0 ("get_utc_time_prim_aa");

  if (asn1r_get_utc_time (aCtx, &utc_time))
    asn1r_set_dec_err (aCtx, MVE_DATA_CONVERT);
  else
    /* just copy the whole struct */
    memcpy (DATA_DEST, (ST_CHAR *)&utc_time, sizeof (MMS_UTC_TIME));

  INC_CURR_OFFSET;
  g_rt_aa++;			/* point to next runtime AA element	*/
  assign_tags_aa (aCtx);		/* Assign tags for next data elt	*/
  }

/************************************************************************/
/*			    data_asn1_decode_done_aa				*/
/* Function called when the end of the ASN.1 'Data' entity is           */
/* detected.  Checks this against the runtime type table.		*/
/************************************************************************/

static ST_VOID data_asn1_decode_done_aa (ASN1_DEC_CTXT *aCtx)
  {
  MLOG_CDEC0 ("data_asn1_decode_done_aa");
  }

/************************************************************************/
/*			     data_dec_err_aa				*/
/* Function called when an invalid tag is encountered in the ASN.1	*/
/* 'Data' entity.  Stops the parse with a fatal error.                  */
/************************************************************************/
static ST_VOID data_dec_err_aa (ASN1_DEC_CTXT *aCtx, ST_RET err)
  {
  MLOG_CDEC0 ("data_dec_err_aa");

  asn1r_set_dec_err (aCtx, MVE_DATA_CONVERT);
  }


/************************************************************************/
/************************************************************************/
/************************************************************************/
/* Block data to ASN.1 data section					*/
/* Assumes that the ASN.1 tools have been initialized.			*/

/************************************************************************/
/*			ms_locl_to_asn1_aa 				*/
/* Similar to ms_locl_to_asn1, except that this one handles alternate	*/
/* access. The assumption is that local data is layed out per the 	*/
/* original type, and only the components specified by the AA are	*/
/* encoded into ASN.1							*/
/* Assume the build tools have been initialized, use ASN.1 build tools	*/
/************************************************************************/

#ifndef MMS_LITE

ST_RET ms_locl_to_asn1_aa (ASN1_ENC_CTXT *aCtx, NAMED_TYPE *tptr, 
                          ALT_ACCESS *alt_acc,
			  ST_CHAR *dptr)
  {
  if (!tptr->rt_num)			/* if len == 0			*/
    {
    MLOG_NERR0 ("Named type has no runtime elements");
    return (MVE_RT_TYPE);		/* 0 length typedef is error		*/
    }

  return (ms_local_to_asn1_aa (aCtx, tptr->rt_head, tptr->rt_num, 
			       alt_acc, dptr));
  }

#endif
       
/************************************************************************/
/*			ms_local_to_asn1_aa 				*/
/************************************************************************/

ST_RET ms_local_to_asn1_aa (ASN1_ENC_CTXT *aCtx, SD_CONST RUNTIME_TYPE *rt_head,
                          ST_INT rt_num,
                          ALT_ACCESS *alt_acc,
                          ST_CHAR *dptr)
  {
RT_AA_CTRL *runtime_aa;		/* ptr to RTAA table		*/
ST_INT num_rt_aa;
ST_RET ret;

/* Create an RTAA that selects the appropriate elements			*/
  if (_ms_mk_rt_aa (rt_head, rt_num, alt_acc, &runtime_aa, &num_rt_aa))
    {
    MLOG_NERR0 ("Error creating runtime alternate access");
    return (MVE_RTAA_TYPE);
    }

  S_LOCK_COMMON_RESOURCES ();

/* Do the conversion!							*/
  ret = do_local_to_asn1_aa (aCtx, runtime_aa, num_rt_aa, dptr);
  S_UNLOCK_COMMON_RESOURCES ();

  if (ret == SD_SUCCESS)
    {
    if (aCtx->asn1r_encode_overrun)		/* Check for encode overrun 	*/
      ret = ME_ASN1_ENCODE_OVERRUN;
    }
  return (ret);
  }

/************************************************************************/
/*			do_local_to_asn1_aa 				*/
/************************************************************************/

static ST_RET do_local_to_asn1_aa (ASN1_ENC_CTXT *aCtx, RT_AA_CTRL *runtime_aa,
                               ST_INT num_rt_aa,
                               ST_CHAR *dptr)
  {
ST_INT done;
ST_RET   ret_val;

/* Set up global working variables					*/
  g_rt_aa = runtime_aa;
  g_rt_aa_head = g_rt_aa;
  g_rt_aa_end = g_rt_aa;

  g_rt_aa += num_rt_aa-1;  	/* point to bottom rt AA element   	*/


/* Set working data pointer						*/
  g_datptr = dptr;

/* Used for 'packed' encode only					*/
  g_current_offset = _ms_get_rtaa_data_size (runtime_aa, num_rt_aa);

  done = SD_FALSE;
  ret_val = SD_SUCCESS;  
  while (!done && !ret_val)  		/* while not done		*/
    {
    g_rt_ptr = g_rt_aa->rt;
    if (ms_is_rt_prim(g_rt_ptr))
      {
      DEC_CURR_OFFSET;
      ret_val = wr_prim_dat_aa (aCtx);	   /* Write the primitive data		*/
      }
    else				/* not a primitive element	*/
      {
      switch (g_rt_ptr->el_tag)
	{
	case RT_ARR_END :			/* array end, start constructor */
	  asn1r_strt_constr (aCtx);
          DEC_CURR_OFFSET;
	break;

	case RT_ARR_START :     		/* array start			*/
          asn1r_fin_constr (aCtx, 1,CTX,DEF);	/* array done, fin the cstr	*/
          DEC_CURR_OFFSET;
	break;

	case RT_STR_END :			/* array end, start constructor */
	  asn1r_strt_constr (aCtx);
          DEC_CURR_OFFSET;
	break;

	case RT_STR_START :
	  asn1r_fin_constr (aCtx, 2,CTX,DEF);  	/* struct done, fin the cstr	*/
          DEC_CURR_OFFSET;
	break;

	default :			/* unknown typedef tag		*/
          MLOG_NERR0 ("Unknown RT type tag");
	  ret_val = MVE_RT_TYPE;
	}
      } /* end if not primitive element */

/* All tags (EXCEPT array start when loopback) come here. 		*/

/* Move the g_rt_ptr to the next element, check for done		       	*/
    if (g_rt_aa == g_rt_aa_end)		/* If this was last element 	*/
      done = SD_TRUE;	
    else
      --g_rt_aa;  			/* move pointer to next 	*/
    }

  chk_free (g_rt_aa_head);	/* all done with the RT AA table	*/
  return (ret_val);
  }

/************************************************************************/
/*			wr_prim_dat_aa					*/
/* Write a primitive data element from the user data to the message	*/
/* under construction.							*/
/************************************************************************/

static ST_RET wr_prim_dat_aa (ASN1_ENC_CTXT *aCtx)
  {
ST_RET   ret_val;
ST_INT16 len;
MMS_BTOD btod_data;
ST_INT32 *i32_ptr;

  ret_val = SD_SUCCESS;

/* use the runtime type def table element and type to select storage	*/

  switch (g_rt_ptr->el_tag)
    {
    case RT_BOOL :				/* boolean			*/
      asn1r_wr_bool (aCtx, *((ST_UCHAR *)DATA_SRC));	/* write a single byte		*/
    break;

    case RT_BIT_STRING : 
      if (g_rt_ptr->u.p.el_len >= 0)
        asn1r_wr_bitstr (aCtx, (ST_UCHAR *)DATA_SRC,g_rt_ptr->u.p.el_len); /* bits */
      else
	{
        len = *((ST_INT16 *) DATA_SRC);
        if (len > abs(g_rt_ptr->u.p.el_len))
          {
          MLOG_NERR1 ("Invalid variable length bitstring length: %d", 
						(ST_INT) len);
          ret_val = MVE_RT_TYPE;
	  }
        else
          asn1r_wr_bitstr (aCtx, (ST_UCHAR *) (DATA_SRC + sizeof(ST_INT16)), len);
	}
    break;

    case RT_INTEGER :
      switch (g_rt_ptr -> u.p.el_len)	/* determine length		*/
	{
	case 1 :			/* one byte int 		*/
	  asn1r_wr_i8 (aCtx, *DATA_SRC);		/* write a single byte	*/
	break;

	case 2 :			/* two byte int 		*/
	  asn1r_wr_i16 (aCtx, *((ST_INT16 *)DATA_SRC));
	break;

	case 4 :			/* four byte integer		*/
	  asn1r_wr_i32 (aCtx, *((ST_INT32 *) DATA_SRC));
	break;

#ifdef INT64_SUPPORT
	case 8 :			/* eight byte integer		*/
	  asn1r_wr_i64 (aCtx, *((ST_INT64 *) DATA_SRC));
	break;
#endif

	default :
          MLOG_NERR0 ("Invalid integer length");
          ret_val = MVE_RT_TYPE;
	}
    break;

    case RT_UNSIGNED :
      switch (g_rt_ptr -> u.p.el_len)	/* determine length		*/
	{
	case 1 :			/* one byte unsigned int	*/
	  asn1r_wr_u8 (aCtx, *((ST_UCHAR *)DATA_SRC));
	break;

	case 2 :			/* two byte unsigned int	*/
	  asn1r_wr_u16 (aCtx, *((ST_UINT16 *) DATA_SRC));
	break;

	case 4 :			/* four byte unsigned integer	*/
	  asn1r_wr_u32 (aCtx, *((ST_UINT32 *) DATA_SRC));
	break;

#ifdef INT64_SUPPORT
	case 8 :			/* eight byte unsigned integer	*/
	  asn1r_wr_u64 (aCtx, *((ST_UINT64 *) DATA_SRC));
	break;
#endif

	default :
          MLOG_NERR0 ("Invalid integer length");
          ret_val = MVE_RT_TYPE;
	}
    break;

#ifdef FLOAT_DATA_SUPPORT
    case RT_FLOATING_POINT :
      if (g_rt_ptr -> u.p.el_len != sizeof (ST_FLOAT))
	asn1r_wr_double (aCtx, *((ST_DOUBLE *)DATA_SRC));
      else
	asn1r_wr_float (aCtx, *((ST_FLOAT *)DATA_SRC));
    break;
#endif

    case RT_OCTET_STRING :	
      if (g_rt_ptr->u.p.el_len >= 0)
        asn1r_wr_octstr (aCtx, (ST_UCHAR *) DATA_SRC, g_rt_ptr->u.p.el_len);
      else
	{
        len = *((ST_INT16 *) DATA_SRC);
        if (len > abs(g_rt_ptr->u.p.el_len))
          {
          MLOG_NERR1 ("Invalid variable length octetstring length: %d", 
						(ST_INT) len);
          ret_val = MVE_RT_TYPE;
	  }
        else
          asn1r_wr_octstr (aCtx, (ST_UCHAR *) (DATA_SRC + sizeof(ST_INT16)), len);
	}
    break;

    case RT_VISIBLE_STRING :
      if (((g_rt_ptr -> u.p.el_len > 0) && 
            (strlen (DATA_SRC) != (ST_UINT) g_rt_ptr->u.p.el_len)) ||
            (strlen (DATA_SRC) >  (ST_UINT) abs (g_rt_ptr -> u.p.el_len)))
        {
        MLOG_ENC0 ("Data is illegal for VISIBLE_STRING type.");
        ret_val = MVE_ASN1_ENCODE_ERR;		/* say data is illegal for type	*/
        }
      else
        asn1r_wr_vstr (aCtx, DATA_SRC);
    break;

#ifdef TIME_DATA_SUPPORT
    case RT_GENERAL_TIME :
      asn1r_wr_time (aCtx, *((time_t *)DATA_SRC));
    break;
#endif

#ifdef BTOD_DATA_SUPPORT
    case RT_BINARY_TIME :
      i32_ptr = (ST_INT32 *) DATA_SRC;
      btod_data.ms = *i32_ptr;
      if (g_rt_ptr->u.p.el_len == 6)
        {
        btod_data.form = MMS_BTOD6;
        ++i32_ptr;
        btod_data.day = *i32_ptr;
        }
      else
        btod_data.form = MMS_BTOD4;

      asn1r_wr_btod (aCtx, &btod_data);
    break;
#endif

/* The u.p.el_len for BCD indicates the number of digits in the value.  */
/* Digits are represented as [0..9] requiring one nibble of storage     */
/* each.  Because all BCD values are sent over the wire as integers and */
/* the C programming language has no native BCD type, BCD values are    */
/* handled by MMS-EASE as integers.                                     */

    case RT_BCD:
      if (g_rt_ptr->u.p.el_len <= 2)
	asn1r_wr_i8 (aCtx, *DATA_SRC);		        /* write a single byte	        */
      else if (g_rt_ptr->u.p.el_len <= 4)
        asn1r_wr_i16 (aCtx, *((ST_INT16 *)DATA_SRC));
      else if (g_rt_ptr->u.p.el_len <= 8)
        asn1r_wr_i32 (aCtx, *((ST_INT32 *) DATA_SRC));
    break;

    case RT_UTC_TIME :
      {
      MMS_UTC_TIME *utc_time = (MMS_UTC_TIME *) DATA_SRC;
      asn1r_wr_utc_time (aCtx, utc_time);
      }
    break;

    case RT_UTF8_STRING :
      if (asn1r_wr_utf8 (aCtx,DATA_SRC,g_rt_ptr->u.p.el_len))
        {
        MLOG_NERR1 ("Data or len is illegal for UTF8(V)string%d.",
                    abs(g_rt_ptr->u.p.el_len));
        ret_val = MVE_ASN1_ENCODE_ERR;
        }
    break;

    default :
      MLOG_NERR0 ("Bad RT tag");
      ret_val = MVE_RT_TYPE;
    }

  asn1r_fin_prim (aCtx, g_rt_ptr->el_tag,CTX);	/* finish the primitive */
  return (ret_val);
  }



