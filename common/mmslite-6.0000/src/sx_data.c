/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	1999 - 2008, All Rights Reserved		        */
/*									*/
/* MODULE NAME : sx_data.c						*/
/* PRODUCT(S)  : 							*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*	sxd_process_arb_data						*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 11/23/11  KCR           Added casting for proper 64bit compile       */
/* 02/23/10  JRB	   Add "maxsize" arg on "*ValsToString" calls.	*/
/*			   Init arr_loops array.			*/
/*			   Fix buffer size for "ctime" result.		*/
/*			   Chk elName!=NULL before strcmp.		*/
/* 01/30/08  EJV     37    sprintf,sscanf: use S_FMT_* macros for ST_INT64 */
/* 01/30/06  GLB     36    Integrated porting changes for VMS           */
/* 08/01/05  JRB     35    Avoid risky cast to remove const.		*/
/* 09/01/04  EJV     34    Added new include "sx_arb.h"			*/
/* 03/16/04  EJV     33    Added ST_(U)LONG typecast to sprintf,on some	*/
/*			   sys ST_(U)INT32 can be (unsigned) long or int*/
/*			   Use SISCO types in place of C types.		*/
/* 01/23/04  MDE     32    Added type ID validity checking		*/
/* 01/13/04  JRB     31    Add LYNX support.				*/
/* 01/08/04  EJV     30    Ported 64-bit funcs to sun. Corr (__hpux).	*/
/* 10/13/03  EJV     29    Replaced defined(QNX) with defined(__QNX__).	*/
/* 04/29/03  JRB     28    Use MAX_IDENT_LEN define.			*/
/* 04/14/03  JRB     27    Add linux support. Use #error.		*/
/* 04/02/03  JRB     26    Add UTF8string support (see RT_UTF8_STRING).	*/
/* 12/11/02  MDE     25    Now include 'mvl_defs.h'			*/
/* 12/02/02  MDE     24    Fixed elPres handling 			*/
/* 04/24/02  EJV     23    Added DEBUG_SISCO for thisFileName		*/
/* 04/17/02  EJV     22    sxd_process_arb_data: added RT_UTC_TIME case */
/*			   _sxdEncArbFuns: added NULL for 'utc' fun	*/
/* 04/08/02  MDE     21    Added NO_MVL ifdefs to support mmslog	*/
/* 03/06/02  JRB     20    Add more SD_CONST. Comment out unused statics*/
/* 02/26/02  EJV     19    strStart: added SD_CONST for rt_head param.	*/
/*			   Eliminated some warnings on VXWORKS.		*/
/*			   Moved limits.h to sysincs.h and INT_MAX check*/
/* 02/25/02  EJV     18    _AIX: replace itoa, ltoa with ANSI sprintf 	*/
/*			   hex_to_ascii_str: added param hex_no_spaces.	*/
/* 11/26/01  EJV     17    _sxdDecInt64: fixed logging.			*/
/* 09/13/01  JRB     16    Add limits.h	so INT_MAX & LONG_MAX defined.	*/
/* 08/10/01  EJV     15    _sxdDecUint32: eliminated warning on QNX.	*/
/* 06/22/01  EJV     14    Reworked decode of arrays and releated chgs.	*/
/*			   Added rt_head param to _sxdEncStrStart.	*/
/*			   Moved globals _sxdPrefix, _sxdCurrNestLevel	*/
/*			   to SX_END_CTRL (to be thread safe).		*/
/*			   Implemented INT64 funcs: _sxdDecInt64,...	*/
/* 05/21/01  MDE     13    Fixed overwrite problem with bad data	*/
/* 05/21/01  MDE     12    Changed to work with all RUNTIME_TYPE	*/
/* 03/21/01  MDE     11    Fixed bitstring decode for variable len bs	*/
/* 03/21/01  MDE     10    Moved sx arb handling defines to sx_defs.h	*/
/* 02/28/01  EJV     09    _AIX: use itoa, ltoa instead of slow sprintf */
/* 02/22/01  MDE     08    Changes to support SXD style selections	*/
/* 02/22/01  MDE     07    Allow QNX & Win32 to use itoa, ltoa		*/
/* 01/24/01  EJV     06    Replaced non-ANSI itoa, ltoa with sprintf.	*/
/*			   SXD_ARB_DATA_CTRL:replaced bool with booln	*/
/*			   (bool may conflict with C++ compiler).	*/
/*			   Corrected conversion in _sxdEncDbl.		*/
/* 01/08/01  EJV     05    Removed unused _writePos from _sxdDecBs.	*/
/* 01/04/01  MDE     04    Always define MVL_DYN_ASN1_TYPES		*/
/* 12/28/00  KCR     03    Removed unpaired S_LOCK_COMMON_RESOURCES()	*/
/* 11/06/00  DWL     02    Modified sx_write_element and sx_end_element	*/
/*                         calls with added parameter			*/
/* 10/10/00  MDE     01    New						*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "glbsem.h"
#include "mms_def2.h"
#include "mms_mp.h"
#include "mms_pvar.h"
#include "mms_vvar.h"
#include "time_str.h"
#include "str_util.h"
#include "mem_chk.h"

#include "sx_defs.h"
#include "sx_arb.h"
#include "sx_log.h"

#if defined(MMS_LITE) && !defined(NO_MVL)
#include "mvl_defs.h"
#endif

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/
/************************************************************************/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/
/*			sxd_get_runtime					*/
/************************************************************************/

#if defined(MMS_LITE) && !defined(NO_MVL)

ST_RET sxd_get_runtime (ST_INT type_id, 
			RUNTIME_TYPE **rt_out, ST_INT *num_rt_out)
  {
  if (type_id >= 0 && type_id < mvl_num_types && 
      mvl_type_ctrl[type_id].num_rt > 0)
    {
    *rt_out = mvl_type_ctrl[type_id].rt;
    *num_rt_out = mvl_type_ctrl[type_id].num_rt;
    return (SD_SUCCESS);
    }
  return (SD_FAILURE);
  }

#endif

/************************************************************************/
/************************************************************************/
/************************************************************************/
/*			sxd_process_arb_data 				*/
/************************************************************************/

ST_RET sxd_process_arb_data (ST_CHAR *datptr, SD_CONST RUNTIME_TYPE *rt_head, 
			     ST_INT rt_num, ST_VOID *usr, 
			     SXD_ARB_DATA_CTRL *ac, ST_BOOLEAN *elPres)
  {
RUNTIME_TYPE *rt_ptr;
RUNTIME_TYPE *rt_end;
ST_RET uDataRet;
ST_INT arr_loop_level;
ST_INT arr_loops[ASN1_MAX_LEVEL];

  memset (arr_loops, 0, sizeof (arr_loops));	/* CRITICAL: start clean.*/
  arr_loop_level = 0;

  rt_ptr = (RUNTIME_TYPE *) rt_head;  	/* point to head rt_block	*/
  rt_end = rt_ptr + rt_num; 	/* done when pointer is here		*/

  uDataRet = SD_SUCCESS;
  while (rt_ptr < rt_end && uDataRet == SD_SUCCESS)
    {
    if (rt_ptr->el_tag == RT_ARR_END)	/* treat case of array ending	*/
      {
      if (--arr_loops[arr_loop_level] > 0) /* if need to do next ar elmnt	*/
	rt_ptr -= rt_ptr->u.arr.num_rt_blks; /* mv rt_ptr to start of arr	*/
      else
        --arr_loop_level;
      }
    if (rt_ptr->el_tag == RT_ARR_START)	/* treat case of array starting	*/
      {
      /* initialize the loop counter for the array				*/
        ++arr_loop_level;
        arr_loops[arr_loop_level] = rt_ptr->u.arr.num_elmnts;
      }

   if (elPres != NULL)
     {
     if (elPres[rt_ptr - rt_head] != SD_TRUE)
       {
        datptr += rt_ptr->el_size; 	/* Adjust data pointer		*/
        rt_ptr++;  			/* point to next rt element	*/
        continue;
       }
     }

    switch (rt_ptr->el_tag)
      {
      case RT_ARR_START :
        if (ac->arrStart)
          uDataRet = (*ac->arrStart) (usr, rt_ptr);
      break;

      case RT_STR_START :
        if (ac->strStart)
          uDataRet = (*ac->strStart) (usr, rt_ptr, rt_head);
      break;

      case RT_ARR_END :			/* array done			*/
        if (ac->arrEnd)
          uDataRet = (*ac->arrEnd) (usr, rt_ptr);
      break;

      case RT_STR_END :			/* structure done		*/
        if (ac->strEnd)
          uDataRet = (*ac->strEnd) (usr, rt_ptr);
      break;

      case RT_BOOL :
        if (ac->booln)
          uDataRet = (*ac->booln) (usr, (ST_BOOLEAN *) datptr, rt_ptr);
      break;

      case RT_BIT_STRING :			
        if (ac->bs)
          uDataRet = (*ac->bs) (usr, (ST_UCHAR *) datptr, rt_ptr);
      break;

      case RT_INTEGER :			
        switch (rt_ptr->u.p.el_len)	/* determine length		*/
       	  {
          case 1 :			/* one byte int 		*/
            if (ac->int8)
              uDataRet = (*ac->int8) (usr, (ST_INT8 *) datptr, rt_ptr);
     	  break;
     
     	  case 2 :			/* two byte int 		*/
            if (ac->int16)
              uDataRet = (*ac->int16) (usr, (ST_INT16 *) datptr, rt_ptr);
     	  break;
     	  
     	  case 4 :			/* four byte integer		*/
            if (ac->int32)
              uDataRet = (*ac->int32) (usr, (ST_INT32 *) datptr, rt_ptr);
     	  break;
     	  
#ifdef INT64_SUPPORT
     	  case 8 :			/* eight byte integer		*/
            if (ac->int64)
              uDataRet = (*ac->int64) (usr, (ST_INT64 *) datptr, rt_ptr);
#endif
     	  break;
          }
      break;

      case RT_UNSIGNED : 			
        switch (rt_ptr->u.p.el_len)	/* determine length		*/
       	  {
          case 1 :			/* one byte int 		*/
            if (ac->uint8)
              uDataRet = (*ac->uint8) (usr, (ST_UCHAR *) datptr, rt_ptr);
     	  break;
     
     	  case 2 :			/* two byte int 		*/
            if (ac->uint16)
              uDataRet = (*ac->uint16) (usr, (ST_UINT16 *) datptr, rt_ptr);
     	  break;
     	  
     	  case 4 :			/* four byte integer		*/
            if (ac->uint32)
              uDataRet = (*ac->uint32) (usr, (ST_UINT32 *) datptr, rt_ptr);
     	  break;
     	  
#ifdef INT64_SUPPORT
     	  case 8 :			/* eight byte integer		*/
            if (ac->uint64)
              uDataRet = (*ac->uint64) (usr, (ST_UINT64 *) datptr, rt_ptr);
     	  break;
#endif 	/* INT64_SUPPORT	*/
     	  }
      break;

#ifdef FLOAT_DATA_SUPPORT
      case RT_FLOATING_POINT : 		
        if (rt_ptr->u.p.el_len != sizeof (ST_FLOAT))
          {
          if (ac->dbl)
            uDataRet = (*ac->dbl) (usr, (ST_DOUBLE *) datptr, rt_ptr);
          }
        else
          {
          if (ac->flt)
            uDataRet = (*ac->flt) (usr, (ST_FLOAT *) datptr, rt_ptr);
          }
      break;
#endif

      case RT_OCTET_STRING : 		
        if (ac->oct)
          uDataRet = (*ac->oct) (usr, (ST_UCHAR *) datptr, rt_ptr);
      break;

      case RT_VISIBLE_STRING : 		
        if (ac->vis)
          uDataRet = (*ac->vis) (usr, (ST_CHAR *) datptr, rt_ptr);
      break;

#ifdef TIME_DATA_SUPPORT
      case RT_GENERAL_TIME : 		
        if (ac->gt)
          uDataRet = (*ac->gt) (usr, (time_t *) datptr, rt_ptr);
      break;
#endif

#ifdef BTOD_DATA_SUPPORT
      case RT_BINARY_TIME : 		
        switch (rt_ptr->u.p.el_len)	/* determine length		*/
	  {
	  case 4:
	    if (ac->bt4)
              uDataRet = (*ac->bt4) (usr, (ST_INT32 *) datptr, rt_ptr);
	    break;
	  case 6:
	    if (ac->bt6)
              uDataRet = (*ac->bt6) (usr, (ST_INT32 *) datptr, rt_ptr);
	    break;
	  }
      break;
#endif

      case RT_BCD : 			
        if (rt_ptr->u.p.el_len <= 2)
          {
          if (ac->bcd1)
            uDataRet = (*ac->bcd1) (usr, (ST_INT8 *) datptr, rt_ptr);
          }
        else if (rt_ptr->u.p.el_len <= 4)
          {
          if (ac->bcd2)
            uDataRet = (*ac->bcd2) (usr, (ST_INT16 *) datptr, rt_ptr);
          }
        else if (rt_ptr->u.p.el_len <= 8)
          {
          if (ac->bcd4)
            uDataRet = (*ac->bcd4) (usr, (ST_INT32 *) datptr, rt_ptr);
          }
      break;

      case RT_UTC_TIME :
        if (ac->utc)
          uDataRet = (*ac->utc) (usr, (MMS_UTC_TIME *) datptr, rt_ptr);
      break;

      case RT_UTF8_STRING :
        if (ac->utf8)
          uDataRet = (*ac->utf8) (usr, (ST_UCHAR *) datptr, rt_ptr);
      break;

      default : 			/* should not be any other tag	*/
        SXLOG_ERR1 ("Invalid tag: %d", (int) rt_ptr->el_tag);
	return (SD_FAILURE);
      break;
      }

    datptr += rt_ptr->el_size; 	/* Adjust data pointer		*/
    rt_ptr++;  			/* point to next rt element	*/
    }
  return (uDataRet);
  }

/************************************************************************/
/************************************************************************/
/************************************************************************/

static ST_RET _sxdStripPrefix (ST_CHAR *prefix);
static ST_RET _sxdEncElement (SX_ENC_CTRL *ec, RUNTIME_TYPE *rt,
	 		      ST_CHAR *dtString, ST_CHAR *contents);

/************************************************************************/

/* XML data type strings */  
static ST_CHAR _boolDtStr[] 		= "Bool";
static ST_CHAR _visStringDtStr[] 	= "VisString";
static ST_CHAR _bitStringDtStr[] 	= "BitString";
static ST_CHAR _i1DtStr[] 		= "I1";
static ST_CHAR _i2DtStr[] 		= "I2";
static ST_CHAR _i4DtStr[] 		= "I4";
static ST_CHAR _uI1DtStr[] 		= "UI1";
static ST_CHAR _uI2DtStr[] 		= "UI2";
static ST_CHAR _uI4DtStr[] 		= "UI4";
static ST_CHAR _r4DtStr[] 		= "R4";
static ST_CHAR _r8DtStr[] 		= "R8";
static ST_CHAR _dateTimeDtStr[] 	= "DateTime";
static ST_CHAR _binHexDtStr[] 		= "BinHex";
#ifdef INT64_SUPPORT				/* Only if INT64 supported*/
static ST_CHAR _i8DtStr[] 		= "I8";
static ST_CHAR _uI8DtStr[] 		= "UI8";
#endif
#if 0						/* These not used yet.	*/
static ST_CHAR _charDtStr[] 		= "Char";
static ST_CHAR _octStringDtStr[] 	= "OctString";
static ST_CHAR _dateDtStr[] 		= "Date";
static ST_CHAR _dateTimeTzDtStr[] 	= "DateTimeTz";
static ST_CHAR _timeDtStr[] 		= "Time";
static ST_CHAR _timeTzDtStr[] 		= "TimeTz";
static ST_CHAR _binDtStr[] 		= "Bin";
static ST_CHAR _binBase64DtStr[] 	= "BinBase64";
static ST_CHAR _uriDtStr[] 		= "Uri";
static ST_CHAR _udtDtStr[] 		= "Udt";
#endif

ST_INT sxdDefXmlStyle; 	/* Note: 0 is style A */

/************************************************************************/
/*			_sxdStripPrefix					*/
/************************************************************************/
static ST_RET _sxdStripPrefix (ST_CHAR *prefix)
  {
  ST_INT i;

  if (prefix)
    {
    i = (int) strlen (prefix) - 1;
    while (--i > 0)
      {
      if (prefix[i] == '.')
        {
        prefix[i+1] = 0;	/* keep the dot with remaining prefix */
        break;
        }
      }
    /* if i=0 then dot not found, this must be the first nest level */
    if (i == 0)
      prefix[0] = 0;
    }

  return (SD_SUCCESS);
  }

/************************************************************************/
/************************************************************************/
/*			_sxdEncArrStart 					*/
/************************************************************************/

static ST_RET _sxdEncArrStart (ST_VOID *usr, RUNTIME_TYPE *rt)
  {

  /* Nothing to do here, the function _sxdEncElement is encoding	*/
  /* the element<=>array name.						*/
  return (SD_SUCCESS);
  }

/************************************************************************/
/*			_sxdEncArrEnd   					*/
/************************************************************************/

static ST_RET _sxdEncArrEnd   (ST_VOID *usr, RUNTIME_TYPE *rt)
  {

  /* Nothing to do here. */
  return (SD_SUCCESS);
  }

/************************************************************************/
/*			_sxdEncStrStart 					*/
/************************************************************************/

static ST_RET _sxdEncStrStart (ST_VOID *usr, RUNTIME_TYPE *rt,
                               SD_CONST RUNTIME_TYPE *rt_head)
  {
SX_ENC_CTRL *ec;
SD_CONST ST_CHAR *elName = "";			/* init to an empty string	*/
RUNTIME_TYPE *rt_tmp;

  ec = (SX_ENC_CTRL *) usr;
  if (ms_comp_name_pres(rt) == SD_TRUE) 
    elName = ms_comp_name_find(rt);
  else
    {
    /* name is NULL, it must be the outer element of struct within array */
    if (rt == rt_head)
      {
      elName = ec->outerElement;
      if (elName == NULL)
        return (SD_SUCCESS);		/* nothing to encode	*/
      }
    else
      {
      /* check if array of structs (check previous RTs) */
      rt_tmp = rt - 1;
      while (rt_tmp->el_tag == RT_ARR_START)
        {
        if (ms_comp_name_pres(rt_tmp) == SD_TRUE)
          {
          elName = ms_comp_name_find(rt_tmp);
          break;
          }
        --rt_tmp;
        }
      }
    }

  if (ec->sxdXmlStyle == SXD_XML_STYLE_A)
    {
    if (ec->sxdStructNestLevel == 0)
      {
      *(ec->nextWritePos++) = '<';
      sx_add_string (ec, elName, &ec->nextWritePos);
      }
    else	/* Not the outer element */
      {
      strcat (ec->sxdPrefix, elName);
      strcat (ec->sxdPrefix, ".");
      }
    ++ec->sxdStructNestLevel;
    }
  else if (ec->sxdXmlStyle == SXD_XML_STYLE_V ||
           ec->sxdXmlStyle == SXD_XML_STYLE_C)
    {
    sx_start_element ((SX_ENC_CTRL *) usr, elName, 0, NULL);
    }

  return (SD_SUCCESS);
  }

/************************************************************************/
/*			_sxdEncStrEnd   					*/
/************************************************************************/

static ST_RET _sxdEncStrEnd   (ST_VOID *usr, RUNTIME_TYPE *rt)
  {
SX_ENC_CTRL *ec;

  ec = (SX_ENC_CTRL *) usr;

  if (ec->sxdXmlStyle == SXD_XML_STYLE_A)
    {
    if (ms_comp_name_pres(rt) == SD_FALSE && ec->outerElement == NULL)
      return (SD_SUCCESS);

    --ec->sxdStructNestLevel;

    if (ec->sxdStructNestLevel == 0)
      {
      /* this is end of XML */
      *(ec->nextWritePos++) = '/';
      *(ec->nextWritePos++) = '>';
      }
    else
      {
      /* Not the outer element or the first level, remove the last prefix */
      _sxdStripPrefix (ec->sxdPrefix);
      }
    }
  else if (ec->sxdXmlStyle == SXD_XML_STYLE_V ||
           ec->sxdXmlStyle == SXD_XML_STYLE_C)
    {
    /*  encode struct end tag */
    if (ec->currNestLevel > 0)
      sx_end_element ((SX_ENC_CTRL *) usr);
    }
  return (SD_SUCCESS);
  }

/************************************************************************/
/************************************************************************/
/*			_sxdEncInt8  					*/
/************************************************************************/

static ST_RET _sxdEncInt8  (ST_VOID *usr, ST_INT8 *data_ptr, RUNTIME_TYPE *rt)
  {
ST_INT intVal;
ST_CHAR buf[50];

  intVal = (ST_INT) (*data_ptr);
#if defined(_WIN32) || defined(__QNX__)
  itoa (intVal, buf, 10);
#else
  sprintf (buf, "%d", intVal);
#endif
  return (_sxdEncElement ((SX_ENC_CTRL *) usr, rt, _i1DtStr, buf));
  }

/************************************************************************/
/*			_sxdEncInt16 					*/
/************************************************************************/

static ST_RET _sxdEncInt16 (ST_VOID *usr, ST_INT16  *data_ptr, RUNTIME_TYPE *rt)
  {
ST_INT intVal;
ST_CHAR buf[50];

  intVal = (ST_INT) *data_ptr;
#if defined(_WIN32) || defined(__QNX__)
  itoa (intVal, buf, 10);
#else
  sprintf (buf, "%d", intVal);
#endif
  return (_sxdEncElement ((SX_ENC_CTRL *) usr, rt, _i2DtStr, buf));
  }

/************************************************************************/
/*			_sxdEncInt32 					*/
/************************************************************************/

static ST_RET _sxdEncInt32 (ST_VOID *usr, ST_INT32   *data_ptr, RUNTIME_TYPE *rt)
  {
ST_LONG long_val;
ST_CHAR buf[50];

  long_val = (ST_LONG) *data_ptr;
#if defined(_WIN32) || defined(__QNX__)
  ltoa (long_val, buf, 10);
#else
  sprintf (buf, "%ld", long_val);
#endif
  return (_sxdEncElement ((SX_ENC_CTRL *) usr, rt, _i4DtStr, buf));
  }

/************************************************************************/
/*			_sxdEncInt64 					*/
/************************************************************************/
#ifdef INT64_SUPPORT

static ST_RET _sxdEncInt64 (ST_VOID *usr, ST_INT64   *data_ptr, RUNTIME_TYPE *rt)
  {
ST_INT64 i64_val;
ST_CHAR buf[80];

  i64_val = (ST_INT64) *data_ptr;
#ifdef _WIN32
  _i64toa (i64_val, buf, 10);
#else
  sprintf (buf, S_FMT_INT64, i64_val);
#endif
  return (_sxdEncElement (usr, rt, _i8DtStr, buf));
  }
#endif

/************************************************************************/
/*			_sxdEncUint8 					*/
/************************************************************************/

static ST_RET _sxdEncUint8 (ST_VOID *usr, ST_UINT8  *data_ptr, RUNTIME_TYPE *rt)
  {
ST_UINT uintVal;
ST_CHAR buf[50];

  uintVal = (ST_UINT) *data_ptr;
#if defined(_WIN32) || defined(__QNX__)
  itoa ((int) uintVal, buf, 10);
#else
  sprintf (buf, "%u", uintVal);
#endif
  return (_sxdEncElement ((SX_ENC_CTRL *) usr, rt, _uI1DtStr, buf));
  }

/************************************************************************/
/*			_sxdEncUint16					*/
/************************************************************************/

static ST_RET _sxdEncUint16(ST_VOID *usr, ST_UINT16 *data_ptr, RUNTIME_TYPE *rt)
  {
ST_LONG long_val;
ST_CHAR buf[50];

  long_val = (ST_LONG) *data_ptr;
#if defined(_WIN32) || defined(__QNX__)
  ltoa (long_val, buf, 10);
#else
  sprintf (buf, "%u", (ST_UINT) long_val);
#endif
  return (_sxdEncElement ((SX_ENC_CTRL *) usr, rt, _uI2DtStr, buf));
  }

/************************************************************************/
/*			_sxdEncUint32					*/
/************************************************************************/

static ST_RET _sxdEncUint32(ST_VOID *usr, ST_UINT32  *data_ptr, RUNTIME_TYPE *rt)
  {
ST_ULONG ulong_val;
ST_CHAR buf[50];

  ulong_val = (ST_ULONG) *data_ptr;
#if defined(_WIN32) || defined(__QNX__)
  ultoa (ulong_val, buf, 10);
#else
  sprintf (buf, "%lu", ulong_val);
#endif
  return (_sxdEncElement ((SX_ENC_CTRL *) usr, rt, _uI4DtStr, buf));
  }

/************************************************************************/
/*			_sxdEncUint64					*/
/************************************************************************/
#ifdef INT64_SUPPORT

static ST_RET _sxdEncUint64(ST_VOID *usr, ST_UINT64  *data_ptr, RUNTIME_TYPE *rt)
  {
ST_UINT64 u64_val;
ST_CHAR buf[80];

  u64_val = (ST_INT64) *data_ptr;
#ifdef _WIN32
  _ui64toa (u64_val, buf, 10);
#else
  sprintf (buf, S_FMT_UINT64, u64_val);
#endif
  return (_sxdEncElement (usr, rt, _uI8DtStr, buf));
  }
#endif

/************************************************************************/
/*			_sxdEncFlt   					*/
/************************************************************************/

static ST_RET _sxdEncFlt   (ST_VOID *usr, ST_FLOAT  *data_ptr, RUNTIME_TYPE *rt)
  {
ST_DOUBLE  d;
ST_CHAR buf[100];

  d =  (ST_DOUBLE) (*(ST_FLOAT *) data_ptr);
  sprintf (buf,"%.10f", d);
  return (_sxdEncElement ((SX_ENC_CTRL *) usr, rt, _r4DtStr, buf));
  }

/************************************************************************/
/*			_sxdEncDbl   					*/
/************************************************************************/

static ST_RET _sxdEncDbl   (ST_VOID *usr, ST_DOUBLE *data_ptr, RUNTIME_TYPE *rt)
  {
ST_DOUBLE  d;
ST_CHAR buf[100];

  d =  *data_ptr;
  sprintf (buf,"%.16f", d);
  return (_sxdEncElement ((SX_ENC_CTRL *) usr, rt, _r8DtStr, buf));
  }

/************************************************************************/
/*			_sxdEncOct   					*/
/************************************************************************/

static ST_RET _sxdEncOct   (ST_VOID *usr, ST_UCHAR  *data_ptr, RUNTIME_TYPE *rt)
  {
ST_INT16 *sp;
ST_CHAR *buf;
ST_INT dataLen;
ST_RET rc;

  if (rt->u.p.el_len < 0)
    {
    sp = (ST_INT16 *) data_ptr;
    dataLen = (ST_INT) *sp;
    data_ptr += 2;
    }
  else
    dataLen = abs (rt->u.p.el_len);

  buf = (ST_CHAR *) M_MALLOC (NULL, (3 * dataLen) + 1);
  hex_to_ascii_str (buf, data_ptr, dataLen, SD_TRUE);
  rc = _sxdEncElement ((SX_ENC_CTRL *) usr, rt, _binHexDtStr, buf);
  M_FREE (NULL, buf);
  return (rc);
  }

/************************************************************************/
/*			_sxdEncBool  					*/
/************************************************************************/

static ST_RET _sxdEncBool  (ST_VOID *usr, ST_BOOLEAN  *data_ptr, RUNTIME_TYPE *rt)
  {
ST_BOOLEAN *bptr;
ST_CHAR *valStr;

  bptr = (ST_BOOLEAN *) data_ptr;
  if (*bptr == 0)
    valStr = "0";
  else
    valStr = "1";

  return (_sxdEncElement ((SX_ENC_CTRL *) usr, rt, _boolDtStr, valStr));
  }

/************************************************************************/
/*			_sxdEncBcd1  					*/
/************************************************************************/

static ST_RET _sxdEncBcd1  (ST_VOID *usr, ST_INT8   *data_ptr, RUNTIME_TYPE *rt)
  {
  return (_sxdEncInt8 (usr, data_ptr, rt));
  }

/************************************************************************/
/*			_sxdEncBcd2  					*/
/************************************************************************/

static ST_RET _sxdEncBcd2  (ST_VOID *usr, ST_INT16  *data_ptr, RUNTIME_TYPE *rt)
  {
  return (_sxdEncInt16 (usr, data_ptr, rt));
  }

/************************************************************************/
/*			_sxdEncBcd4  					*/
/************************************************************************/

static ST_RET _sxdEncBcd4  (ST_VOID *usr, ST_INT32  *data_ptr, RUNTIME_TYPE *rt)
  {
  return (_sxdEncInt32 (usr, data_ptr, rt));
  }

/************************************************************************/
/*			_sxdEncBs    					*/
/************************************************************************/

static ST_RET _sxdEncBs    (ST_VOID *usr, ST_UCHAR  *data_ptr, RUNTIME_TYPE *rt)
  {
ST_INT i;
ST_INT16 *sp;
ST_CHAR *_writePos;
ST_UINT8 bitMask;
ST_UINT8 *bytePtr;
ST_INT numBits;
ST_CHAR *buf;
ST_RET rc;

  if (rt->u.p.el_len < 0)
    {
    sp = (ST_INT16 *) data_ptr;
    numBits = (ST_INT) *sp;
    data_ptr += 2;
    }
  else
    numBits = rt->u.p.el_len;

  buf = (ST_CHAR *) M_MALLOC (NULL, numBits + 1);

  _writePos = buf;
  bitMask = 0x80;
  bytePtr = data_ptr;
  for (i = 0; i < numBits; ++i)
    {
    if (*bytePtr & bitMask)
      *(_writePos++) = '1';
    else
      *(_writePos++) = '0';
  
    if (bitMask == 0x01)
      {
      bitMask = 0x80;
      ++bytePtr;
      }
    else
      bitMask = bitMask >> 1;
    }  

  rc = _sxdEncElement ((SX_ENC_CTRL *) usr, rt, _bitStringDtStr, buf);
  M_FREE (NULL, buf);
  return (rc);
  }

/************************************************************************/
/************************************************************************/
/*			_sxdEncVis   					*/
/************************************************************************/

static ST_RET _sxdEncVis   (ST_VOID *usr, ST_CHAR  *data_ptr, RUNTIME_TYPE *rt)
  {
  return (_sxdEncElement ((SX_ENC_CTRL *) usr, rt, _visStringDtStr, data_ptr));
  }

/************************************************************************/
/*			_sxdEncBt4   					*/
/************************************************************************/

static ST_RET _sxdEncBt4   (ST_VOID *usr, ST_INT32 *data_ptr, RUNTIME_TYPE *rt)
  {
ST_CHAR buf[100];
ST_RET rc;

  rc = Btime4ValsToString (buf, sizeof(buf),(ST_LONG) *data_ptr);
  if (rc != SD_SUCCESS)
    return (rc);

  return (_sxdEncElement ((SX_ENC_CTRL *) usr, rt, _dateTimeDtStr, buf));
  }

/************************************************************************/
/*			_sxdEncBt6   					*/
/************************************************************************/

static ST_RET _sxdEncBt6   (ST_VOID *usr, ST_INT32 *data_ptr, RUNTIME_TYPE *rt)
  {
ST_CHAR buf[100];
ST_RET rc;

  rc = Btime6ValsToString (buf, sizeof(buf), *(data_ptr+1), (ST_LONG) *data_ptr);
  if (rc != SD_SUCCESS)
    return (rc);

  return (_sxdEncElement ((SX_ENC_CTRL *) usr, rt, _dateTimeDtStr, buf));
  }

/************************************************************************/
/*			_sxdEncGt   					*/
/************************************************************************/

static ST_RET _sxdEncGt   (ST_VOID *usr, time_t *data_ptr, RUNTIME_TYPE *rt)
  {
ST_CHAR timeText[26];	/* ctime generates 26 char including \0	*/

  strcpy (timeText, ctime(data_ptr));
  timeText[24] = 0;	/* squash the \n */

  return (_sxdEncElement ((SX_ENC_CTRL *) usr, rt, _dateTimeDtStr, timeText));
  }

/************************************************************************/
/*			_sxdEncUTF8   					*/
/* Don't know how to display. Just show the hex bytes.			*/
/************************************************************************/

static ST_RET _sxdEncUTF8 (ST_VOID *usr, ST_UCHAR *data_ptr, RUNTIME_TYPE *rt)
  {
ST_CHAR *buf;
ST_INT dataLen;
ST_RET rc;

  dataLen = rt->el_size;

  buf = (ST_CHAR *) M_MALLOC (NULL, (3 * dataLen) + 1);
  hex_to_ascii_str (buf, data_ptr, dataLen, SD_TRUE);
  rc = _sxdEncElement ((SX_ENC_CTRL *) usr, rt, _binHexDtStr, buf);
  M_FREE (NULL, buf);
  return (rc);
  }

/************************************************************************/
/************************************************************************/
/*			_sxdEncElement 					*/
/************************************************************************/

static ST_RET _sxdEncElement (SX_ENC_CTRL *ec, RUNTIME_TYPE *rt,
	 		      ST_CHAR *dtString, ST_CHAR *contents)
  {
SD_CONST ST_CHAR *elName;
SXE_ATTR_PAIR attr[2];
ST_INT numAttr;
RUNTIME_TYPE *rt_tmp;

  if (ms_comp_name_pres(rt) == SD_TRUE) 
    elName = ms_comp_name_find(rt);
  else
    {
    /* must be an array element, get the name from the RT_ARR_START	*/
    elName = "UNNAMED";		/* set, just in case name not specified */
    rt_tmp = rt-1;
    /* this maybe multidimentional array , only the first dim has name in RT tbl */
    while (rt_tmp->el_tag == RT_ARR_START)
      {
      if (ms_comp_name_pres(rt_tmp) == SD_TRUE) 
        {
        elName = ms_comp_name_find(rt_tmp);
        break;
        }
      --rt_tmp;
      }
    }

  if (ec->sxdXmlStyle == SXD_XML_STYLE_A)
    {
    *(ec->nextWritePos++) = ' ';
    sx_add_string (ec, ec->sxdPrefix, &ec->nextWritePos);
    sx_add_string (ec, elName, &ec->nextWritePos);
    *(ec->nextWritePos++) = '=';
    *(ec->nextWritePos++) = '"';
    sx_add_string (ec, contents, &ec->nextWritePos);
    *(ec->nextWritePos++) = '"';
    }
  else if (ec->sxdXmlStyle == SXD_XML_STYLE_C)
    {
    sx_wr_string_el (ec, elName, contents);
    }
  else if (ec->sxdXmlStyle == SXD_XML_STYLE_V)
    {
    if (ec->encDt)
      {
      attr[0].name = "DT";
      attr[0].value = dtString;
      numAttr = 1;
      }
    else
      numAttr = 0;

    attr[numAttr].name = "V";
    attr[numAttr].value = contents;
    ++numAttr;
    sx_write_element (ec, elName, numAttr, attr, SD_FALSE, SD_TRUE);
    }

  return (SD_SUCCESS);
  }



/************************************************************************/
/************************************************************************/

SXD_ARB_DATA_CTRL _sxdEncArbFuns =
  {
  _sxdEncArrStart, 	/* arrStart 	*/
  _sxdEncArrEnd, 	/* arrEnd    	*/
  _sxdEncStrStart, 	/* strStart  	*/
  _sxdEncStrEnd, 	/* strEnd    	*/
  _sxdEncInt8,  	/* int8      	*/
  _sxdEncInt16,		/* int16     	*/
  _sxdEncInt32, 	/* int32     	*/
#ifdef INT64_SUPPORT
  _sxdEncInt64, 	/* int64     	*/
#endif
  _sxdEncUint8, 	/* uint8     	*/
  _sxdEncUint16,	/* uint16    	*/
  _sxdEncUint32,	/* uint32    	*/
#ifdef INT64_SUPPORT
  _sxdEncUint64,	/* uint64    	*/
#endif
  _sxdEncFlt,   	/* flt       	*/
  _sxdEncDbl,   	/* dbl       	*/
  _sxdEncOct, 		/* oct       	*/
  _sxdEncBool, 		/* booln      	*/
  _sxdEncBcd1, 		/* bcd1      	*/
  _sxdEncBcd2, 		/* bcd2      	*/
  _sxdEncBcd4, 		/* bcd4      	*/
  _sxdEncBs, 		/* bs	       	*/
  _sxdEncVis, 		/* vis       	*/
  _sxdEncBt4, 		/* Bin Time    	*/
  _sxdEncBt6, 		/* Bin Time    	*/
  _sxdEncGt, 		/* Gen Time    	*/
  NULL,			/* UTC Time	*/
  _sxdEncUTF8,		/* UTF8string	*/
  };

/************************************************************************/
/*			_sxd_wr_data					*/
/************************************************************************/

#if defined(MMS_LITE) && !defined(NO_MVL)

ST_RET _sxd_wr_data (SX_ENC_CTRL *sxEncCtrl, ST_INT typeId,
		   ST_CHAR *elName, ST_VOID *data, ST_INT sxdXmlStyle,
		   ST_BOOLEAN encodeDt, ST_BOOLEAN *elPres)
  {
RUNTIME_TYPE *rt;
ST_INT numRt;
ST_RET rc;

  rc = sxd_get_runtime (typeId, &rt, &numRt);
  if (rc != SD_SUCCESS)
    return (rc);

  rc = sxd_wr_rtdata (sxEncCtrl, rt, numRt, elName, data, sxdXmlStyle, encodeDt, elPres);
  return (rc);
  }
#endif


/************************************************************************/
/*			sxd_wr_rtdata 					*/
/************************************************************************/

ST_RET sxd_wr_rtdata (SX_ENC_CTRL *sxEncCtrl, RUNTIME_TYPE *rt, ST_INT numRt, 
		   ST_CHAR *elName, ST_VOID *data, ST_INT sxdXmlStyle,
		   ST_BOOLEAN encodeDt, ST_BOOLEAN *elPres)
  {
ST_RET rc;

  sxEncCtrl->sxdXmlStyle = sxdXmlStyle;
  sxEncCtrl->encDt = encodeDt;
  sxEncCtrl->outerElement = elName;
  sxEncCtrl->sxdPrefix[0] = 0;

  rc = sxd_process_arb_data ((ST_CHAR *) data, rt, numRt, sxEncCtrl, 
						&_sxdEncArbFuns, elPres);
  if (rc != SD_SUCCESS)
    return (rc);

  return (rc);
  }


/************************************************************************/
/************************************************************************/
/************************************************************************/
/************************************************************************/

#define _SXD_MAX_VAL_BUF 8000
#define _SXD_MAX_EL_NAME   50
#define _SXD_MAX_DT        50

typedef struct
  {
  ST_INT sxdXmlStyle;	/* See sx_defs.h for values */
  ST_CHAR *xmlDest;
  ST_BOOLEAN checkDt;
  ST_CHAR *xmlPos;
  ST_CHAR *xmlBufEnd;
  ST_RET xmlDecResult;
  SD_CONST ST_CHAR *outerElement;
  ST_CHAR valBuf[_SXD_MAX_VAL_BUF+1];
  ST_CHAR elName[_SXD_MAX_EL_NAME+1];
  ST_CHAR dtBuf[_SXD_MAX_DT+1];
  ST_BOOLEAN endEl; 
  ST_BOOLEAN dataEl; 
  } SXD_DEC_CTRL;

/************************************************************************/
/************************************************************************/

static ST_RET _sxdCheckDt (SXD_DEC_CTRL *dc, ST_CHAR *dtString)
  {
  if (strcmp (dc->dtBuf, dtString) != 0)
    {
    SXLOG_NERR3 ("Type mismatch for elName '%s': expected '%s', got '%s'", 
  			 		dc->elName, dtString, dc->dtBuf);
    dc->xmlDecResult = SD_FAILURE;
    return (SD_FAILURE);
    }
  return (SD_SUCCESS);
  }


/************************************************************************/
/************************************************************************/
/*			_sxdDecArrStart 					*/
/************************************************************************/

static ST_RET _sxdDecArrStart (SXD_DEC_CTRL *dc, RUNTIME_TYPE *rt)
  {
  return (SD_SUCCESS);
  }

/************************************************************************/
/*			_sxdDecArrEnd   					*/
/************************************************************************/

static ST_RET _sxdDecArrEnd   (SXD_DEC_CTRL *dc, RUNTIME_TYPE *rt)
  {
  return (SD_SUCCESS);
  }

/************************************************************************/
/*			_sxdDecStrStart 					*/
/************************************************************************/

static ST_RET _sxdDecStrStart (SXD_DEC_CTRL *dc, RUNTIME_TYPE *rt)
  {
  return (SD_SUCCESS);
  }

/************************************************************************/
/*			_sxdDecStrEnd   				*/
/************************************************************************/

static ST_RET _sxdDecStrEnd (SXD_DEC_CTRL *dc, RUNTIME_TYPE *rt)
  {
  return (SD_SUCCESS);
  }

/************************************************************************/
/************************************************************************/
/*			_sxdDecInt8  					*/
/************************************************************************/

static ST_RET _sxdDecInt8  (SXD_DEC_CTRL *dc, ST_INT8 *data_ptr, RUNTIME_TYPE *rt)
  {
ST_INT intVal;
ST_RET rc;

/* Check data type, if requested ... */
  if (dc->checkDt != SD_FALSE)
    {
    rc = _sxdCheckDt (dc, _i1DtStr);
    if (rc != SD_SUCCESS)
      return (rc);
    }

  intVal = atoi (dc->valBuf);
  *data_ptr = (ST_INT8) intVal;
  SXLOG_CDEC1 ("Integer8: %d",intVal);
  return (SD_SUCCESS);
  }

/***********************************************************************/
/*     		_sxdDecInt16 					*/
/***********************************************************************/

static ST_RET _sxdDecInt16 (SXD_DEC_CTRL *dc, ST_INT16  *data_ptr, RUNTIME_TYPE *rt)
  {
ST_INT intVal;
ST_RET rc;

/* Check data type, if requested ... */
  if (dc->checkDt != SD_FALSE)
    {
    rc = _sxdCheckDt (dc, _i2DtStr);
    if (rc != SD_SUCCESS)
      return (rc);
    }

  intVal = atoi (dc->valBuf);
  *data_ptr = (ST_INT16) intVal;
  SXLOG_CDEC1 ("Integer16: %d",intVal);
  return (SD_SUCCESS);
  }

/************************************************************************/
/*			_sxdDecInt32 					*/
/************************************************************************/

static ST_RET _sxdDecInt32 (SXD_DEC_CTRL *dc, ST_INT32   *data_ptr, RUNTIME_TYPE *rt)
  {
ST_INT32 intVal;
ST_RET rc;

/* Check data type, if requested ... */
  if (dc->checkDt != SD_FALSE)
    {
    rc = _sxdCheckDt (dc, _i4DtStr);
    if (rc != SD_SUCCESS)
      return (rc);
    }

  intVal = atoi (dc->valBuf);
  *data_ptr = (ST_INT32) intVal;
  SXLOG_CDEC1 ("Integer32: %ld", (ST_LONG) intVal);
  return (SD_SUCCESS);
  }

/************************************************************************/
/*			_sxdDecInt64 					*/
/************************************************************************/
#ifdef INT64_SUPPORT

static ST_RET _sxdDecInt64 (SXD_DEC_CTRL *dc, ST_INT64   *data_ptr, RUNTIME_TYPE *rt)
  {
ST_INT64 i64Val;
ST_RET rc;

/* Check data type, if requested ... */
  if (dc->checkDt != SD_FALSE)
    {
    rc = _sxdCheckDt (dc, _i8DtStr);
    if (rc != SD_SUCCESS)
      return (rc);
    }

#if defined(_WIN32)
  i64Val = _atoi64 (dc->valBuf);
#else
  if (sscanf (dc->valBuf, S_FMT_INT64, &i64Val) != 1)
    return (SD_FAILURE);
#endif
  *data_ptr = i64Val;
  SXLOG_CDEC1 ("Integer64: " S_FMT_INT64, i64Val);
  return (SD_SUCCESS);
  }
#endif

/************************************************************************/
/*			_sxdDecUint8 					*/
/************************************************************************/

static ST_RET _sxdDecUint8 (SXD_DEC_CTRL *dc, ST_UINT8  *data_ptr, RUNTIME_TYPE *rt)
  {
ST_INT intVal;
ST_RET rc;

/* Check data type, if requested ... */
  if (dc->checkDt != SD_FALSE)
    {
    rc = _sxdCheckDt (dc, _uI1DtStr);
    if (rc != SD_SUCCESS)
      return (rc);
    }

  intVal = atoi (dc->valBuf);
  *data_ptr = (ST_UINT8) intVal;
  SXLOG_CDEC1 ("Unsigned8: %d",intVal);
  return (SD_SUCCESS);
  }

/************************************************************************/
/*			_sxdDecUint16					*/
/************************************************************************/

static ST_RET _sxdDecUint16 (SXD_DEC_CTRL *dc, ST_UINT16 *data_ptr, RUNTIME_TYPE *rt)
  {
ST_INT intVal;
ST_RET rc;

/* Check data type, if requested ... */
  if (dc->checkDt != SD_FALSE)
    {
    rc = _sxdCheckDt (dc, _uI2DtStr);
    if (rc != SD_SUCCESS)
      return (rc);
    }

  intVal = atoi (dc->valBuf);
  *data_ptr = (ST_UINT16) intVal;
  SXLOG_CDEC1 ("Unsigned16: %d",intVal);
  return (SD_SUCCESS);
  }

/************************************************************************/
/*			_sxdDecUint32					*/
/************************************************************************/

static ST_RET _sxdDecUint32(SXD_DEC_CTRL *dc, ST_UINT32  *data_ptr, RUNTIME_TYPE *rt)
  {
ST_ULONG intVal;
ST_RET rc;

/* Check data type, if requested ... */
  if (dc->checkDt != SD_FALSE)
    {
    rc = _sxdCheckDt (dc, _uI4DtStr);
    if (rc != SD_SUCCESS)
      return (rc);
    }

#if (LONG_MAX > INT_MAX)
    intVal = atol (dc->valBuf);
#else
    if (sscanf (dc->valBuf, "%lu", &intVal) != 1)
      return (SD_FAILURE);
#endif
  *data_ptr = (ST_UINT32) intVal;
  SXLOG_CDEC1 ("Unsigned32: %lu",intVal);
  return (SD_SUCCESS);
  }

/************************************************************************/
/*			_sxdDecUint64					*/
/************************************************************************/
#ifdef INT64_SUPPORT

static ST_RET _sxdDecUint64(SXD_DEC_CTRL *dc, ST_UINT64  *data_ptr, RUNTIME_TYPE *rt)
  {
ST_UINT64 intVal;
ST_RET rc;

/* Check data type, if requested ... */
  if (dc->checkDt != SD_FALSE)
    {
    rc = _sxdCheckDt (dc, _uI8DtStr);
    if (rc != SD_SUCCESS)
      return (rc);
    }

  if (sscanf (dc->valBuf, S_FMT_UINT64, &intVal) != 1)
    return (SD_FAILURE);
  *data_ptr = intVal;
  SXLOG_CDEC1 ("Unsigned64: " S_FMT_UINT64, intVal);
  return (SD_SUCCESS);
  }
#endif

/************************************************************************/
/*			_sxdDecFlt   					*/
/************************************************************************/

static ST_RET _sxdDecFlt   (SXD_DEC_CTRL *dc, ST_FLOAT  *data_ptr, RUNTIME_TYPE *rt)
  {
ST_DOUBLE doubleVal;
ST_RET rc;

/* Check data type, if requested ... */
  if (dc->checkDt != SD_FALSE)
    {
    rc = _sxdCheckDt (dc, _r4DtStr);
    if (rc != SD_SUCCESS)
      return (rc);
    }

  doubleVal = atof (dc->valBuf);
  *data_ptr = (ST_FLOAT) doubleVal;
  SXLOG_CDEC1 ("Float: %f", doubleVal);
  return (SD_SUCCESS);
  }

/************************************************************************/
/*			_sxdDecDbl   					*/
/************************************************************************/

static ST_RET _sxdDecDbl   (SXD_DEC_CTRL *dc, ST_DOUBLE *data_ptr, RUNTIME_TYPE *rt)
  {
ST_DOUBLE doubleVal;
ST_RET rc;

/* Check data type, if requested ... */
  if (dc->checkDt != SD_FALSE)
    {
    rc = _sxdCheckDt (dc, _r8DtStr);
    if (rc != SD_SUCCESS)
      return (rc);
    }

  doubleVal = atof (dc->valBuf);
  *data_ptr = (ST_FLOAT) doubleVal;
  SXLOG_CDEC1 ("Double: %f", doubleVal);
  return (SD_SUCCESS);
  }

/************************************************************************/
/*			_sxdDecOct   					*/
/************************************************************************/

static ST_RET _sxdDecOct (SXD_DEC_CTRL *dc, ST_UCHAR  *data_ptr, RUNTIME_TYPE *rt)
  {
ST_RET rc;
ST_INT16 *sp;
ST_UINT maxOctets;
ST_UINT hexLen;

/* Check data type, if requested ... */
  if (dc->checkDt != SD_FALSE)
    {
    rc = _sxdCheckDt (dc, _binHexDtStr);
    if (rc != SD_SUCCESS)
      return (rc);
    }

  if (rt->u.p.el_len < 0)
    data_ptr += 2;

  maxOctets = abs (rt->u.p.el_len);
  rc = ascii_to_hex_str (data_ptr, &hexLen, maxOctets, dc->valBuf);
  if (rc != SD_SUCCESS)
    return (rc);

  if (rt->u.p.el_len < 0)
    {
    sp = (ST_INT16 *) (data_ptr-2);
    *sp = hexLen;
    }

  return (SD_SUCCESS);
  }

/************************************************************************/
/*			_sxdDecBool  					*/
/************************************************************************/

static ST_RET _sxdDecBool  (SXD_DEC_CTRL *dc, ST_BOOLEAN  *data_ptr, RUNTIME_TYPE *rt)
  {
ST_RET rc;

/* Check data type, if requested ... */
  if (dc->checkDt != SD_FALSE)
    {
    rc = _sxdCheckDt (dc, _boolDtStr);
    if (rc != SD_SUCCESS)
      return (rc);
    }

  *data_ptr = dc->valBuf[0] == '0' ? 0 : 1;
  return (SD_SUCCESS);
  }

/************************************************************************/
/*			_sxdDecBcd1  					*/
/************************************************************************/

static ST_RET _sxdDecBcd1  (SXD_DEC_CTRL *dc, ST_INT8   *data_ptr, RUNTIME_TYPE *rt)
  {
  return (_sxdDecInt8 (dc, data_ptr, rt));
  }

/************************************************************************/
/*			_sxdDecBcd2  					*/
/************************************************************************/

static ST_RET _sxdDecBcd2  (SXD_DEC_CTRL *dc, ST_INT16  *data_ptr, RUNTIME_TYPE *rt)
  {
  return (_sxdDecInt16 (dc, data_ptr, rt));
  }

/************************************************************************/
/*			_sxdDecBcd4  					*/
/************************************************************************/

static ST_RET _sxdDecBcd4  (SXD_DEC_CTRL *dc, ST_INT32  *data_ptr, RUNTIME_TYPE *rt)
  {
  return (_sxdDecInt32 (dc, data_ptr, rt));
  }

/************************************************************************/
/*			_sxdDecBs    					*/
/************************************************************************/

static ST_RET _sxdDecBs (SXD_DEC_CTRL *dc, ST_UCHAR *data_ptr, RUNTIME_TYPE *rt)
  {
ST_INT16 *sp;
ST_UINT8 bitMask;
ST_UINT8 *bytePtr;
ST_RET rc;
ST_CHAR *ascPtr;
ST_INT bitCount;

/* Check data type, if requested ... */
  if (dc->checkDt != SD_FALSE)
    {
    rc = _sxdCheckDt (dc, _bitStringDtStr);
    if (rc != SD_SUCCESS)
      return (rc);
    }

  if (rt->u.p.el_len < 0)
    bytePtr = data_ptr + 2;
  else
    bytePtr = data_ptr;

  bitMask = 0x80;
  ascPtr = dc->valBuf;
  bitCount = 0;
  while (*ascPtr != 0 && bitCount < abs (rt->u.p.el_len))
    {
    if (*ascPtr == '1')
      *bytePtr |= bitMask;
    else
      *bytePtr &= ~bitMask;

    if (bitMask == 0x01)
      {
      bitMask = 0x80;
      ++bytePtr;
      }
    else
      bitMask = bitMask >> 1;

    ++bitCount;
    }  

  if (rt->u.p.el_len < 0)
    {
    sp = (ST_INT16 *) (data_ptr);
    *sp = bitCount;
    }
  return (SD_SUCCESS);
  }

/************************************************************************/
/*			_sxdDecVis   					*/
/************************************************************************/

static ST_RET _sxdDecVis   (SXD_DEC_CTRL *dc, ST_CHAR  *data_ptr, RUNTIME_TYPE *rt)
  {
ST_RET rc;

/* Check data type, if requested ... */
  if (dc->checkDt != SD_FALSE)
    {
    rc = _sxdCheckDt (dc, _visStringDtStr);
    if (rc != SD_SUCCESS)
      return (rc);
    }

  strcpy (data_ptr, dc->valBuf);
  SXLOG_CDEC1 ("Visible string: '%s'", data_ptr);
  return (SD_SUCCESS);
  }

/************************************************************************/
/*			_sxdDecBt4   					*/
/************************************************************************/

static ST_RET _sxdDecBt4   (SXD_DEC_CTRL *dc, ST_INT32 *data_ptr, RUNTIME_TYPE *rt)
  {
ST_RET rc;

/* Check data type, if requested ... */
  if (dc->checkDt != SD_FALSE)
    {
    rc = _sxdCheckDt (dc, _dateTimeDtStr);
    if (rc != SD_SUCCESS)
      return (rc);
    }

  rc = Btime4StringToVals (dc->valBuf, data_ptr);
  return (SD_SUCCESS);
  }

/************************************************************************/
/*			_sxdDecBt6   					*/
/************************************************************************/

static ST_RET _sxdDecBt6   (SXD_DEC_CTRL *dc, ST_INT32 *data_ptr, RUNTIME_TYPE *rt)
  {
ST_RET rc;

/* Check data type, if requested ... */
  if (dc->checkDt != SD_FALSE)
    {
    rc = _sxdCheckDt (dc, _dateTimeDtStr);
    if (rc != SD_SUCCESS)
      return (rc);
    }

  rc = Btime6StringToVals (dc->valBuf, data_ptr+1, data_ptr);
  return (rc);
  }

/************************************************************************/
/*			_sxdDecGt   					*/
/************************************************************************/

static ST_RET _sxdDecGt   (SXD_DEC_CTRL *dc, time_t *data_ptr, RUNTIME_TYPE *rt)
  {
ST_RET rc;

/* Check data type, if requested ... */
  if (dc->checkDt != SD_FALSE)
    {
    rc = _sxdCheckDt (dc, _dateTimeDtStr);
    if (rc != SD_SUCCESS)
      return (rc);
    }

  rc = tstrStringToTime (dc->valBuf, data_ptr);
  return (rc);
  }

/************************************************************************/
/************************************************************************/
/*			_sxdDecElementA 					*/
/************************************************************************/

static ST_RET _sxdDecElementA (SXD_DEC_CTRL *dc)
  {
ST_CHAR *p;
ST_CHAR c;
ST_INT i;

/* Continue decoding the element, which must be of the form		*/
/* <structName el1Name="yyy" el2Name="xxx"/>				*/

  dc->endEl = SD_FALSE;
  dc->dataEl = SD_FALSE;

  c = *(dc->xmlPos++);
  if (c == '<')
    {
  /* OK, get the element name */
    c = *(dc->xmlPos++);
    p = dc->elName;
    i = 0;
    while (c != ' ' && c != '>')
      {
      if (++i > _SXD_MAX_EL_NAME)
        {
        SXLOG_NERR0 ("Could not find ' ' or '>'"); 
        dc->xmlDecResult = SD_FAILURE;
        return (SD_FAILURE);
        }

      *(p++) = c;       
      c = *(dc->xmlPos++);
      if (dc->xmlPos > dc->xmlBufEnd)
        {
        SXLOG_NERR0 ("Could not find ' ' or '>'"); 
        dc->xmlDecResult = SD_FAILURE;
        return (SD_FAILURE);
        }
      }
    *p = 0;  
    return (SD_SUCCESS);
    }

/* OK, not the start of an element, see if is the end of an element	*/
  if (c == '/')
    {
    dc->endEl = SD_TRUE;
    c = *(dc->xmlPos++);	/* Eat the '>' */
    return (SD_SUCCESS);
    }

/* OK, must be the name of a structure element */
  p = dc->elName;
  i = 0;
  while (c != '=')
    {
    if (c != ' ')   	/* Skip leading spaces before attribute name 	*/
      {
      if (++i > _SXD_MAX_EL_NAME)
        {
        SXLOG_NERR0 ("Could not find ' ' or '>'"); 
        dc->xmlDecResult = SD_FAILURE;
        return (SD_FAILURE);
        }
      *(p++) = c;       
      }
    c = *(dc->xmlPos++);
    if (dc->xmlPos > dc->xmlBufEnd)
      {
      SXLOG_NERR0 ("Could not find '='"); 
      dc->xmlDecResult = SD_FAILURE;
      return (SD_FAILURE);
      }
    }
  *p = 0;  

/* Finally, we get to the value */
  c = *(dc->xmlPos++);	/* Eat the '"' */
  p = dc->valBuf;
  c = *(dc->xmlPos++);
  i = 0;
  while (c != '"')
    {
    if (++i > _SXD_MAX_VAL_BUF)
      {
      SXLOG_NERR0 ("Could not find ' ' or '>'"); 
      dc->xmlDecResult = SD_FAILURE;
      return (SD_FAILURE);
      }
    *(p++) = c;       
    c = *(dc->xmlPos++);
    if (dc->xmlPos > dc->xmlBufEnd)
      {
      SXLOG_NERR0 ("Could not find end of value attribute"); 
      dc->xmlDecResult = SD_FAILURE;
      return (SD_FAILURE);
      }
    }
  *p = 0;  
  dc->dataEl = SD_TRUE;
  return (SD_SUCCESS);
  }

/************************************************************************/
/*			_sxdDecElementC					*/
/************************************************************************/

static ST_RET _sxdDecElementC (SXD_DEC_CTRL *dc)
  {
ST_CHAR *p;
ST_CHAR c;

ST_INT i;
/* Decode the XML element, which must be of the form 	*/
/* <elName>xxxx</elName>				*/
/* <elName>< ...					*/
/* </elName>						*/

/* Find the opening '<' */
  while (SD_TRUE)
    {
    c = *(dc->xmlPos++);
    if (c == '<')
      {
      if (*dc->xmlPos != '!')
        break;
      }
    if (dc->xmlPos > dc->xmlBufEnd)
      {
      SXLOG_NERR0 ("Could not find opening '<'"); 
      dc->xmlDecResult = SD_FAILURE;
      return (SD_FAILURE);
      }
    }

  c = *(dc->xmlPos++);	  
  if (c == '/')
    {
    dc->endEl = SD_TRUE;
    c = *(dc->xmlPos++);
    }
  else
    dc->endEl = SD_FALSE;

/* OK, get the element name */
  p = dc->elName;
  i = 0;
  while (c != '>')
    {
    if (++i > _SXD_MAX_EL_NAME)
      {
      SXLOG_NERR0 ("Could not find ' ' or '>'"); 
      dc->xmlDecResult = SD_FAILURE;
      return (SD_FAILURE);
      }

    *(p++) = c;       
    c = *(dc->xmlPos++);
    if (dc->xmlPos > dc->xmlBufEnd)
      {
      SXLOG_NERR0 ("Could not find ' '"); 
      dc->xmlDecResult = SD_FAILURE;
      return (SD_FAILURE);
      }
    }
  *p = 0;  

/* If this is an end element, we are done */
  if (dc->endEl == SD_TRUE)
    return (SD_SUCCESS);

/* Now check to see if this is a nested element */
  p = dc->xmlPos;
  while (SD_TRUE)
    {
    if (*(p++) == '<')	/* The end of this element or start of nest	*/
      {
      if (*p != '!')		/* if this is not a comment */
        {
        if (*p == '/')		/* The end of this element */
          break;

        return (SD_SUCCESS); 	/* Start of nested element */
	}
      }
    if (p > dc->xmlBufEnd)
      {
      SXLOG_NERR0 ("Could not find ' '"); 
      dc->xmlDecResult = SD_FAILURE;
      return (SD_FAILURE);
      }
    }  

/* OK, now we can get the value */
  dc->dataEl = SD_TRUE;

  p = dc->valBuf;
  c = *(dc->xmlPos++);
  i = 0;
  while (c != '<')
    {
    if (++i > _SXD_MAX_VAL_BUF)
      {
      SXLOG_NERR0 ("Could not find ' ' or '>'"); 
      dc->xmlDecResult = SD_FAILURE;
      return (SD_FAILURE);
      }

    *(p++) = c;       
    c = *(dc->xmlPos++);
    if (dc->xmlPos > dc->xmlBufEnd)
      {
      SXLOG_NERR0 ("Could not find end of value"); 
      dc->xmlDecResult = SD_FAILURE;
      return (SD_FAILURE);
      }
    }
  *p = 0;  

/* Now we need to consume the end tag */
  c = *(dc->xmlPos++);
  if (c != '/')
    {
    SXLOG_NERR0 ("Expected end tag"); 
    dc->xmlDecResult = SD_FAILURE;
    return (SD_FAILURE);
    }

  while (c != '>')
    {
    c = *(dc->xmlPos++);
    if (dc->xmlPos > dc->xmlBufEnd)
      {
      SXLOG_NERR0 ("Could not find end of end tag"); 
      dc->xmlDecResult = SD_FAILURE;
      return (SD_FAILURE);
      }
    }
  return (SD_SUCCESS);
  }

/************************************************************************/
/*			_sxdDecElementV 					*/
/************************************************************************/

static ST_RET _sxdDecElementV (SXD_DEC_CTRL *dc)
  {
ST_CHAR *p;
ST_CHAR c;
ST_INT i;

/* Decode the XML element, which must be of the form 	*/
/* <elName DT="yyy" V="xxx"/>				*/
/* <elName>						*/
/* </elName>						*/

  c = *(dc->xmlPos++);
  if (c != '<')
    {
    SXLOG_NERR0 ("Could not find opening '<'"); 
    dc->xmlDecResult = SD_FAILURE;
    return (SD_FAILURE);
    }

  c = *(dc->xmlPos++);
  if (c == '/')
    {
    dc->endEl = SD_TRUE;
    c = *(dc->xmlPos++);
    }
  else
    dc->endEl = SD_FALSE;

/* OK, get the element name */
  p = dc->elName;
  i = 0;
  while (c != ' ' && c != '>')
    {
    if (++i > _SXD_MAX_EL_NAME)
      {
      SXLOG_NERR0 ("Could not find ' ' or '>'"); 
      dc->xmlDecResult = SD_FAILURE;
      return (SD_FAILURE);
      }
    *(p++) = c;       
    c = *(dc->xmlPos++);
    if (dc->xmlPos > dc->xmlBufEnd)
      {
      SXLOG_NERR0 ("Could not find ' '"); 
      dc->xmlDecResult = SD_FAILURE;
      return (SD_FAILURE);
      }
    }
  *p = 0;  

  if (c == ' ')
    {
    dc->dataEl = SD_TRUE;

  /* OK, now see if the data type is included ... */
    p = dc->dtBuf;
    c = *(dc->xmlPos++);
    if (c == 'D')
      {
      c = *(dc->xmlPos++);	/* Eat the 'T'		  */
      c = *(dc->xmlPos++);	/* Eat the '='		  */
      c = *(dc->xmlPos++);	/* Eat the '"'		  */
      c = *(dc->xmlPos++);
      i = 0;
      while (c != '"' && dc->xmlPos <= dc->xmlBufEnd)
        {
        if (++i > _SXD_MAX_DT)
          {
          SXLOG_NERR0 ("Could not find end of DT"); 
          dc->xmlDecResult = SD_FAILURE;
          return (SD_FAILURE);
          }

        *(p++) = c;       
        c = *(dc->xmlPos++);
        if (dc->xmlPos > dc->xmlBufEnd)
          {
          SXLOG_NERR0 ("Could not find ' '"); 
          dc->xmlDecResult = SD_FAILURE;
          return (SD_FAILURE);
          }
        }
      c = *(dc->xmlPos++);	/* Eat the ' '		  */
      c = *(dc->xmlPos++);	/* This should be the 'V' */
      }
    *p = 0;  

  /* Finally, we get to the value, right? */
    if (c != 'V')
      {
      SXLOG_NERR0 ("Expected 'V'"); 
      dc->xmlDecResult = SD_FAILURE;
      return (SD_FAILURE);
      }
    c = *(dc->xmlPos++);	/* Eat the '=' */
    c = *(dc->xmlPos++);	/* Eat the '"' */

    p = dc->valBuf;
    c = *(dc->xmlPos++);
    i = 0;
    while (c != '"')
      {
      if (++i > _SXD_MAX_VAL_BUF)
        {
        SXLOG_NERR0 ("Could not find ' ' or '>'"); 
        dc->xmlDecResult = SD_FAILURE;
        return (SD_FAILURE);
        }
      *(p++) = c;       
      c = *(dc->xmlPos++);
      if (dc->xmlPos > dc->xmlBufEnd)
        {
        SXLOG_NERR0 ("Could not find end of value attribute"); 
        dc->xmlDecResult = SD_FAILURE;
        return (SD_FAILURE);
        }
      }
    *p = 0;  
    c = *(dc->xmlPos++);	/* Eat the '/' */
    c = *(dc->xmlPos++);	/* Eat the '>' */
    }
  else
    dc->dataEl = SD_FALSE;	

  return (SD_SUCCESS);
  }

/************************************************************************/
/************************************************************************/

#if defined(MMS_LITE) && !defined(NO_MVL)

ST_RET _sxd_decode_data_el (ST_CHAR *elName, 
			   ST_CHAR *xml, ST_INT xmlLen, ST_INT *xmlUsed, 
			   ST_VOID *vdp, ST_INT typeId, 
			   ST_BOOLEAN *elPres, ST_INT sxdXmlStyle, ST_BOOLEAN checkDt)
  {
RUNTIME_TYPE *rt;
ST_INT numRt;
ST_RET rc;

  rc = sxd_get_runtime (typeId, &rt, &numRt);
  if (rc != SD_SUCCESS)
    return (rc);

  rc = sxd_decode_rtdata_el (elName, xml, xmlLen, xmlUsed, vdp, rt, numRt, 
			     elPres, sxdXmlStyle, checkDt);

  return (rc);
  }
#endif

/************************************************************************/
/*			sxd_decode_rtdata_el 				*/
/************************************************************************/

ST_RET sxd_decode_rtdata_el (SD_CONST ST_CHAR *elName, 
			   ST_CHAR *xml, ST_INT xmlLen, ST_INT *xmlUsed, 
			   ST_VOID *vdp, SD_CONST RUNTIME_TYPE *rt_head, 
			   ST_INT rt_num, ST_BOOLEAN *elPres, 
			   ST_INT sxdXmlStyle, ST_BOOLEAN checkDt)
  {
RUNTIME_TYPE *rt_ptr;
RUNTIME_TYPE *rt_end;
RUNTIME_TYPE *rt_tmp;
ST_RET uDataRet;
ST_INT arr_loop_level;
ST_INT arr_loops[ASN1_MAX_LEVEL];
ST_RET rc;
SXD_DEC_CTRL *dc;
ST_CHAR *datptr;
ST_CHAR nameBuf[SXD_MAX_PREFIX+MAX_IDENT_LEN+1];
ST_INT j, cnt;
ST_BOOLEAN set_arr_start;
ST_BOOLEAN set_arr_end;
ST_BOOLEAN plain_array;
ST_CHAR structPrefix[SXD_MAX_PREFIX+1];	/* used only for decoding of style A */
ST_INT  structNestLevel;		/* used only for decoding of style A */

/* macro to set the elPres field, update datptr, and advance to next RT */
#define _SXD_ADVANCE_PTR(rt_ptr, pres_flg) { \
              if (elPres != NULL) \
                elPres[(rt_ptr) - rt_head] = pres_flg; \
              datptr += (rt_ptr)->el_size; \
              ++(rt_ptr); }

  datptr = (ST_CHAR *) vdp;
  dc = (SXD_DEC_CTRL *) M_CALLOC (NULL, 1, sizeof (SXD_DEC_CTRL));
  if (!dc)
    {
    SXLOG_ERR1 ("Unable to allocate memory (bytes %u)", sizeof (SX_DEC_CTRL));
    return (SD_FAILURE);
    }
  dc->sxdXmlStyle = sxdXmlStyle;
  if (dc->sxdXmlStyle == SXD_XML_STYLE_V)
    dc->checkDt = checkDt;
  else
    dc->checkDt = SD_FALSE;

  dc->xmlDest = xml;
  dc->xmlPos = xml;
  dc->xmlBufEnd = xml + xmlLen;
  dc->xmlDecResult = SD_SUCCESS;
  dc->outerElement = elName;

  arr_loop_level = 0;
  memset (arr_loops, 0, sizeof (arr_loops));

  rt_ptr = (RUNTIME_TYPE *) rt_head;  	/* point to head rt_block	*/
  rt_end = rt_ptr + rt_num; 	/* done when pointer is here		*/

/* Here we will work our way through the XML and the runtime type  */
  
  uDataRet = SD_SUCCESS;
  structPrefix[0] = 0;
  structNestLevel = 0;

  while ((dc->xmlPos < dc->xmlBufEnd) && (rt_ptr < rt_end) &&
         uDataRet == SD_SUCCESS)
    {
  /* Position to the opening '<' */
    if (dc->sxdXmlStyle == SXD_XML_STYLE_C ||
  	dc->sxdXmlStyle == SXD_XML_STYLE_V)
      {
      while (*dc->xmlPos != '<' && dc->xmlPos < dc->xmlBufEnd)
        ++dc->xmlPos;
      }
    if (dc->xmlPos >= dc->xmlBufEnd)
      {
      M_FREE (NULL, dc);
      return (SD_SUCCESS);
      }

    /* Decode the next XML data element */
    if (dc->sxdXmlStyle == SXD_XML_STYLE_A)
      rc = _sxdDecElementA (dc);
    else if (dc->sxdXmlStyle == SXD_XML_STYLE_C)
      rc = _sxdDecElementC (dc);
    else if (dc->sxdXmlStyle == SXD_XML_STYLE_V)
      rc = _sxdDecElementV (dc);
    else
      rc = SD_FAILURE;				/* invalid style	*/

    if (rc != SD_SUCCESS)
      {
      M_FREE (NULL, dc);
      return (rc);
      }

  /* If we reached the end of the element for style A, we are done */
    if (dc->endEl == SD_TRUE && dc->sxdXmlStyle == SXD_XML_STYLE_A)
      {
      if (dc->outerElement && elPres != NULL)
	elPres[rt_num-1] = SD_TRUE;

      break;
      }

    /* Note1: The processing is different for plain array and different	*/
    /*        for array of structures when they are decoded.		*/
    /*        In the case of plain array the function sxdDecElementX	*/
    /*        will return the decoded array element (no start or end	*/
    /*        array element tag).					*/
    /*        In the case of array of structures the fun sxdDecElementX	*/
    /*        will return first the start of array element tag, then	*/
    /*        the decoded array element (x struct fields), and at the	*/
    /*        end we get the array element end tag.			*/
    /* Note2: Arrays are not supported in SXD_XML_STYLE_A because the	*/
    /*        attributes must have unique names.			*/

    plain_array = SD_FALSE;
    set_arr_start = SD_FALSE;
    set_arr_end = SD_FALSE;

    /* OK, we need to find the RT that has the name */
    while (rt_ptr < rt_end && uDataRet == SD_SUCCESS)
      {
      elName = NULL;

      if (rt_ptr->el_tag == RT_STR_START)	/* treat case of struct start */
        {
        /*--------------------------------------------------------------*/
        if (ms_comp_name_pres(rt_ptr) == SD_TRUE)
          {
          elName = ms_comp_name_find(rt_ptr);
          if (dc->sxdXmlStyle == SXD_XML_STYLE_A)
            {
            strcat (structPrefix, elName);
            strcat (structPrefix, ".");
            if (strncmp (dc->elName, structPrefix, strlen(structPrefix)) == 0)
              {
              ++structNestLevel;
              _SXD_ADVANCE_PTR(rt_ptr, SD_TRUE);
              continue;
              }
            else
              {
              /* wrong struct, need to skip this struct in rt tbl */
              _sxdStripPrefix (structPrefix);	/* restore the prefix	*/
              }
            }
          }
        if (elName != NULL)
          {
          /* Check to see if we found the associated RUNTIME element	*/
          if (strcmp (dc->elName, elName) == 0)
            break;		/* struct name matches, process it	*/
          else
            {
            /* dc->elName does not match, need to skip this struct in rt tbl */
            cnt = rt_ptr->u.str.num_rt_blks + 2;	/* 2 for str start, end	*/
            for (j=0; j<cnt; ++j)
              {
              _SXD_ADVANCE_PTR(rt_ptr, SD_FALSE);
              }
            continue;
            }
          }
        else
          {
          /* elName is NULL only in the case of array of structures	*/
          if (arr_loop_level > 0)
            {
            /* array of structs, advance to next rt */
            uDataRet = _sxdDecStrStart (dc, rt_ptr);/* normally called from switch */
            _SXD_ADVANCE_PTR(rt_ptr, SD_TRUE);
            continue;	/* continue processing of the array element	*/
            }
          else
            {
            /* must be the XML wrapper */
            elName = dc->outerElement;
            if (elName == NULL)
              /* can't compare the wrapper name, assume it is OK */
              break;
            else
              {
              if (strcmp (dc->elName, elName) == 0)
                break;				/* process the wrapper	*/
              else
                {
                /* wrong XML ? */
                SXLOG_NERR2 ("Could not find XML wrapper '%s' (found '%s')", dc->elName, elName); 
                M_FREE (NULL, dc);
                return (SD_FAILURE);
                }
              }
            }
          }
	}
      else if (rt_ptr->el_tag == RT_STR_END)	/* treat case of struct end */
        {
        /*--------------------------------------------------------------*/
        if (dc->sxdXmlStyle == SXD_XML_STYLE_A)
          {
          /* need to strip name of last struct level */
          _sxdStripPrefix (structPrefix);
          --structNestLevel;
          _SXD_ADVANCE_PTR(rt_ptr, SD_TRUE);
          continue;
          }
        else
          {
          /* get the struct name from the RT_STR_START rt */
          rt_tmp = rt_ptr - (rt_ptr->u.str.num_rt_blks + 1);
          if (ms_comp_name_pres(rt_tmp) == SD_TRUE) 
            elName = ms_comp_name_find(rt_tmp);
	  }
        if (elName != NULL)
          {
          /* Check to see if we found the associated RUNTIME element	*/
          if (strcmp (dc->elName, elName) == 0)
            break;		/* struct name matches, process end  	*/
          }
        else
          {
          /* elName maybe NULL in the case of array of structures */
          if (arr_loop_level > 0)
            {
            /* this is an array of structs, advance to next rt (but not to next decoded element)*/
            uDataRet = _sxdDecStrEnd (dc, rt_ptr);/* normally called from switch */
            _SXD_ADVANCE_PTR(rt_ptr, SD_TRUE);
            continue;	/* continue processing of the array element	*/
            }
          else
            {
            /* this may be the wrapper end */
            elName = dc->outerElement;
            if (elName == NULL)
              /* can't compare the wrapper name, assume it is OK */
              break;
            else
              {
              if (strcmp (dc->elName, elName) == 0)
                break;				/* process the wrapper	*/
              else
                {
                /* wrong XML ? */
                SXLOG_NERR2 ("Could not find XML end wrapper tag '%s' (found '%s')", dc->elName, elName); 
                M_FREE (NULL, dc);
                return (SD_FAILURE);
                }
              }
            }
          }
        }
      else if (rt_ptr->el_tag == RT_ARR_START)	/* treat case of array element starting	*/
        {
        /*--------------------------------------------------------------*/
        /* check if the start tag of array element matches dc->elName	*/
        /* before setting loop level 					*/
        if (ms_comp_name_pres(rt_ptr) == SD_TRUE) 
          elName = ms_comp_name_find(rt_ptr);
        if (elName != NULL && strcmp (dc->elName, elName) == 0)
          {
          if (arr_loop_level == 0) /* set only one time per array */
            {
            rt_tmp = rt_ptr;
            while (rt_tmp->el_tag == RT_ARR_START)
              {
              ++arr_loop_level;
              arr_loops[arr_loop_level] = rt_tmp->u.arr.num_elmnts;
              ++rt_tmp;
              }
            }
          /* array may be multidimentional, skip to the last RT_ARR_START */
          while ((rt_ptr+1)->el_tag == RT_ARR_START)
            {
            uDataRet = _sxdDecArrStart (dc, rt_ptr);	/* normally called from switch */
            _SXD_ADVANCE_PTR(rt_ptr, SD_TRUE);
            }

          /* if it is array of structs, processing is different		*/
          if ((rt_ptr+1)->el_tag == RT_STR_START)
            /* array of structs */
            break;
          else
            {
            /* plain array, adjust ptrs and go to next rt */
            plain_array = SD_TRUE;
            uDataRet = _sxdDecArrStart (dc, rt_ptr);/* normally called from switch */
            _SXD_ADVANCE_PTR(rt_ptr, SD_TRUE);
            continue;
            }
          }
        /* dc->elName does not match, need to skip this array in rt tbl */
        cnt = rt_ptr->u.arr.num_rt_blks + 2;	/* 2 for arr start, end	*/
        for (j=0; j<cnt; ++j)
          {
          _SXD_ADVANCE_PTR(rt_ptr, SD_FALSE);
          }
        continue;	/* continue searching for the dc->elName in rt tbl	*/
        }
      else if (rt_ptr->el_tag == RT_ARR_END)	/* treat case of array element ending	*/
        {
        /*--------------------------------------------------------------*/
        /* we get here only in the case of array of structures		*/
        /* get the array name from the first RT_ARR_START rt (maybe nested) */
        rt_tmp = rt_ptr - (rt_ptr->u.arr.num_rt_blks + 1);
        while (ms_comp_name_pres(rt_tmp) != SD_TRUE && rt_tmp->el_tag == RT_ARR_START) 
          --rt_tmp;
        elName = "UNKNOWN";
        if (ms_comp_name_pres(rt_tmp) == SD_TRUE)
          elName = ms_comp_name_find(rt_tmp);

        /* check if the end tag of an array element is correct */
        if (strcmp (dc->elName, elName) != 0)
          {
          /* invalid end tag for the array element, quit decoding */
          SXLOG_NERR2 ("Could not find array element end tag '%s' (found '%s')", elName, dc->elName); 
          M_FREE (NULL, dc);
          return (SD_FAILURE);
          }
        if (--arr_loops[arr_loop_level] > 0)   /* if need next ar elmnt	*/
          {
          /* there should be more array elements to process, need to	*/
          /* reset the rt to the beginning of the array		*/
          set_arr_start = SD_TRUE;
          }
        else
          {
          /* all array elements processed, decrement loop level */
          --arr_loop_level;
          if (arr_loops[arr_loop_level] > 0)
            set_arr_start = SD_TRUE;		/* more elements to process */
          else
            {
            /* advance to last RT_ARR_END (if multidimentional array) */
            while ((rt_ptr+1)->el_tag == RT_ARR_END)
              {
              _SXD_ADVANCE_PTR(rt_ptr, SD_TRUE);
              }
            }
          }
        break;
        }
      else
        {
        /*--------------------------------------------------------------*/
        /* struct field or array element */
        if (ms_comp_name_pres(rt_ptr) == SD_TRUE) 
          {
          elName = ms_comp_name_find(rt_ptr);
          if (dc->sxdXmlStyle == SXD_XML_STYLE_A)
            {
            strcpy (nameBuf, structPrefix);
	    strcat (nameBuf, elName);
            if (strcmp (dc->elName, nameBuf) == 0)
              break;		/* found element name, process it	*/
            else
              {
              /* not name we search for, skip it */
              _SXD_ADVANCE_PTR(rt_ptr, SD_FALSE);
              continue;
              }         
            }
          }
      
        if (elName != NULL)
          {
          if (strcmp (dc->elName, elName) == 0)
            break;		/* found element name, process it	*/
          else
            {
            _SXD_ADVANCE_PTR(rt_ptr, SD_FALSE);
            continue;		/* continue looking for the dc->elName	*/
            }
          }
        else
          {
          /* elName maybe NULL in the case of an array element (plain array) */
          if (plain_array)
            {
            /* since no array element end tag will be decoded, we need	*/
            /* to handle this case here 				*/
            if (--arr_loops[arr_loop_level] > 0)   /* if need next ar elmnt	*/
              {
              /* there should be more array elements to process, need to	*/
              /* reset the rt to the beginning of the array		*/
              set_arr_start = SD_TRUE;
              }
            else
              {
              /* all array elements processed, decrement loop level */
              --arr_loop_level;
              if (arr_loops[arr_loop_level] > 0)
                set_arr_start = SD_TRUE;	/* more elements to process	*/
              else
                set_arr_end = SD_TRUE;		/* end of array			*/
              }
            break;
            }
          else
            {
            SXLOG_NERR1 ("Could not find tag '%s' (found 'NULL')", dc->elName); 
            M_FREE (NULL, dc);
            return (SD_FAILURE);
            }
          }
        }

      /* !!! if we get here some 'if-else' is not handled! return error	*/
      SXLOG_NERR0 ("Unhandled code, returning from function sxd_decode_rtdata_el."); 
      M_FREE (NULL, dc);
      return (SD_FAILURE);
      }  /* end of inner while loop */

    if (rt_ptr >= rt_end || uDataRet != SD_SUCCESS)
      {
      SXLOG_NERR1 ("Could not find RT for element name %s", dc->elName); 
      M_FREE (NULL, dc);
      return (SD_FAILURE);
      }

  /* We found a RUNTIME element that matches the XML name 		*/
	       
    if (1 || dc->dataEl == SD_TRUE)
      {
      switch (rt_ptr->el_tag)
        {
        case RT_ARR_START :
            uDataRet = _sxdDecArrStart (dc, rt_ptr);
        break;
    
        case RT_STR_START :
            uDataRet = _sxdDecStrStart (dc, rt_ptr);
        break;
    
        case RT_ARR_END :			/* array done			*/
            uDataRet = _sxdDecArrEnd (dc, rt_ptr);
        break;
    
        case RT_STR_END :			/* structure done		*/
            uDataRet = _sxdDecStrEnd (dc, rt_ptr);
        break;
    
        case RT_BOOL :
            uDataRet = _sxdDecBool (dc, (ST_BOOLEAN *) datptr, rt_ptr);
        break;
    
        case RT_BIT_STRING :			
            uDataRet = _sxdDecBs (dc, (ST_UCHAR *) datptr, rt_ptr);
        break;
    
        case RT_INTEGER :                     
          switch (rt_ptr->u.p.el_len) /* determine length             */
            {
            case 1 :                  /* one byte int                 */
              uDataRet = _sxdDecInt8 (dc, (ST_INT8 *) datptr, rt_ptr);
            break;
       
            case 2 :                      /* two byte int                 */
              uDataRet = _sxdDecInt16 (dc, (ST_INT16 *) datptr, rt_ptr);
            break;
                
            case 4 :                      /* four byte integer            */
              uDataRet = _sxdDecInt32 (dc, (ST_INT32 *) datptr, rt_ptr);
            break;
        
#ifdef INT64_SUPPORT
            case 8 :                      /* eight byte integer           */
              uDataRet = _sxdDecInt64 (dc, (ST_INT64 *) datptr, rt_ptr);
            break;
#endif
            }
        break;

        case RT_UNSIGNED :                        
          switch (rt_ptr->u.p.el_len)     /* determine length             */
            {
            case 1 :                      /* one byte int                 */
              uDataRet = _sxdDecUint8 (dc, (ST_UCHAR *) datptr, rt_ptr);
            break;
        
            case 2 :                      /* two byte int                 */
              uDataRet = _sxdDecUint16 (dc, (ST_UINT16 *) datptr, rt_ptr);
            break;
            
            case 4 :                      /* four byte integer            */
              uDataRet = _sxdDecUint32 (dc, (ST_UINT32 *) datptr, rt_ptr);
            break;
            
#ifdef INT64_SUPPORT
            case 8 :                      /* eight byte integer           */
              uDataRet = _sxdDecUint64 (dc, (ST_UINT64 *) datptr, rt_ptr);
            break;
#endif  /* INT64_SUPPORT        */
            }
        break;
    
#ifdef FLOAT_DATA_SUPPORT
        case RT_FLOATING_POINT :          
          if (rt_ptr->u.p.el_len != sizeof (ST_FLOAT))
            {
            uDataRet = _sxdDecDbl (dc, (ST_DOUBLE *) datptr, rt_ptr);
            }
          else
            {
            uDataRet = _sxdDecFlt (dc, (ST_FLOAT *) datptr, rt_ptr);
            }
        break;
#endif
    
        case RT_OCTET_STRING :            
            uDataRet = _sxdDecOct (dc, (ST_UCHAR *) datptr, rt_ptr);
        break;
    
        case RT_VISIBLE_STRING :          
          uDataRet = _sxdDecVis (dc, (ST_CHAR *) datptr, rt_ptr);
        break;
    
#ifdef TIME_DATA_SUPPORT
        case RT_GENERAL_TIME :            
          uDataRet = _sxdDecGt (dc, (time_t *) datptr, rt_ptr);
        break;
#endif
    
#ifdef BTOD_DATA_SUPPORT
        case RT_BINARY_TIME :             
          switch (rt_ptr->u.p.el_len)     /* determine length             */
            {
            case 4:
              uDataRet = _sxdDecBt4 (dc, (ST_INT32 *) datptr, rt_ptr);
            break;
            case 6:
              uDataRet = _sxdDecBt6 (dc, (ST_INT32 *) datptr, rt_ptr);
            break;
            }
        break;
#endif
    
        case RT_BCD :                     
          if (rt_ptr->u.p.el_len <= 2)
            {
            uDataRet = _sxdDecBcd1 (dc, (ST_INT8 *) datptr, rt_ptr);
            }
          else if (rt_ptr->u.p.el_len <= 4)
            {
            uDataRet = _sxdDecBcd2 (dc, (ST_INT16 *) datptr, rt_ptr);
            }
          else if (rt_ptr->u.p.el_len <= 8)
            {
            uDataRet = _sxdDecBcd4 (dc, (ST_INT32 *) datptr, rt_ptr);
            }
        break;
    
        default :                         /* should not be any other tag  */
          SXLOG_ERR1 ("Invalid tag: %d", (int) rt_ptr->el_tag);
          M_FREE (NULL, dc);
          return (SD_FAILURE);
        break;
        }
    
      /* Set the element present flag for this element to TRUE		*/
      if (elPres != NULL)
        elPres[rt_ptr - rt_head] = SD_TRUE;
      datptr += rt_ptr->el_size;  		/* Adjust data pointer	*/

      if (set_arr_start)
        {
        if (plain_array)
          --rt_ptr;					/* rt_ptr to arr start	*/
        else
          rt_ptr -= rt_ptr->u.arr.num_rt_blks + 1;	/* rt_ptr to arr start	*/
        while ((rt_ptr-1)->el_tag == RT_ARR_START)	/* if multidimensional	*/
          --rt_ptr;					/* go to first level	*/
        }
      else if (set_arr_end)
        {
        rt_ptr++;
        /* if multidimensional array, pass the last RT_ARR_END */
        while ((rt_ptr)->el_tag == RT_ARR_END)
          {
          _SXD_ADVANCE_PTR(rt_ptr, SD_TRUE);
          }
        }
      else
        rt_ptr++;				/* point to next rt elmt*/
      }  /* if (1 || ...) */
    }  /* end of outher while loop */
  *xmlUsed = (int) (dc->xmlPos - xml);
  M_FREE (NULL, dc);
  return (uDataRet);
  }

