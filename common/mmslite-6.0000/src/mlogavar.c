/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*                  1986 - 2008 All Rights Reserved                     */
/*									*/
/* MODULE NAME : mlogavar.c						*/
/* PRODUCT(S)  : MMS-EASE						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 01/20/10  JRB     35    Fix log messages.				*/
/* 06/30/09  MDE     34    Change to allow logging long TDL		*/
/* 01/30/08  EJV     33    Use S_FMT_* macros to log ST_INT64.		*/
/* 03/29/07  LWP     32	   Changed mlogUtc to log the GMT time value	*/
/* 02/13/07  RKR     31    changed mlogBt6 to log the GMT time value */
/* 01/30/06  GLB     30    Integrated porting changes for VMS           */
/* 02/01/04  EJV     29    _WIN32 mlogUint64: corr ST_UINT64 logging.	*/
/* 07/26/05  MDE     28    Fixed memory leak for TDL logging		*/
/* 11/22/04  EJV     27    Use slog_max_msg_size_get not sl_max_msg_size*/
/* 10/19/04  nav     26    add #ifndef SX_NO_ARB_DATA                   */
/* 09/01/04  EJV     25    Added new include "sx_arb.h"			*/
/* 03/16/04  EJV     24    Added ST_(U)LONG typecast to logs,on some sys*/
/*			     ST_(U)INT32 can be (unsigned) long or int.	*/
/*			   Removed (added) typecasting.			*/
/*			   Corr logging for ST_INT64, ST_UINT64.	*/
/* 04/11/04  GLB     23    Added "ifdef DEBUG_SISCO" for "thisFileName" */
/* 01/16/04  EJV     22    Added typecast to elim warning on AIX.	*/
/* 10/31/03  JRB     21    Increase MAX_TDL_LEN so ms_runtime_to_tdl	*/
/*			   less likely to fail, even though tdl		*/
/*			   produced may be too long to log.		*/
/* 10/14/03  JRB     20    Del unnecessary casts.			*/
/* 06/04/03  MDE     19    Made typespec logging safe for namedTypes	*/
/* 04/02/03  JRB     18    Add UTF8string support (see mlogUtf8).	*/
/* 07/03/02  EJV     17    MMS_UTC_TIME: chg name usec to fraction.	*/
/* 04/17/02  EJV     16    Uncommented mlogUtc in sx_proc_arb_data	*/
/*			     Changed proto for mlogUtc()		*/
/* 04/05/02  MDE     15    Now use sx_proc_arb_data			*/
/* 11/15/01  EJV     14    Added support for new MMS type UtcTime:	*/
/*                         mlog_arb_log_ctrl[]: added mlogUtc.		*/
/* 07/07/00  JRB     13    Add #if INT64_SUPP...			*/
/* 03/09/00  DSF     12	   Fixed a memory leak in m_log_var_list ()	*/
/* 09/13/99  MDE     11    Added SD_CONST modifiers			*/
/* 04/07/99  MDE     10    Now log alternate access better		*/
/* 08/13/98  MDE     09    Added m_log_tdl				*/
/* 06/22/98  MDE     08    Now allow TDL up to 10000 bytes long		*/
/* 06/15/98  MDE     07    Changes to allow compile under C++		*/
/* 03/20/98  JRB     06    Don't need mmsop_en.h anymore.		*/
/* 01/30/98  EJV     05    Changed 1 arg to ST_INT32 mlogBt4, mlogBt6	*/
/*			   casted params to ms_asn1_data_to_locl	*/
/* 12/02/97  DSF     04    Changes for floats, INT64			*/
/* 09/29/97  MDE     03    Changed to use new MMS_ARB structure		*/
/* 09/11/97  MDE     02    MMS ARB user funs now return ST_RET		*/
/* 08/04/97  MDE     01    Removed DEBUG_SISCO code			*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"

#ifndef MMS_LITE
#include "mms_usr.h"		/* to access MMS fucntions, variables   */
#else
#include "mmsdefs.h"
#endif
#include "asn1defs.h"
#include "mms_pvar.h"
#include "mms_vvar.h"
#include "mem_chk.h"
#include "mloguser.h"
#include "sx_defs.h"
#include "sx_arb.h"
#include "stdtime.h"
#include "stdtime_mms_btime.h"
#include "stdtime_mms_utctime.h"

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/
/************************************************************************/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif


/************************************************************************/
/* Common user output routines for variable access.			*/
/************************************************************************/
/*			m_log_address					*/
/* This routine is used to print an address.				*/
/************************************************************************/

ST_VOID m_log_address (VAR_ACC_ADDR *addr_ptr)
  {
  switch (addr_ptr->addr_tag)
    {
    case (0)  :
      MLOG_CALWAYS1 ("Numeric Address : %08lX",(ST_ULONG)addr_ptr->addr.num_addr);
      break;

    case (1)  :
      MLOG_CALWAYS1 ("Symbolic Address : %s",addr_ptr->addr.sym_addr);
      break;

    case (2)  :
      MLOG_CALWAYS0 ("Unconstrained Address : ");
      MLOG_ALWAYSH (addr_ptr->addr.unc_addr.unc_len,
                    addr_ptr->addr.unc_addr.unc_ptr);
		  
      break;

    default   :
      MLOG_CALWAYS1 ("Invalid Address Tag : %d ",addr_ptr->addr_tag);
      break;
    }
  }

/************************************************************************/
/*			m_log_var_list					*/
/* This routine is used to print a variable access variable list.	*/
/************************************************************************/

ST_VOID m_log_var_list (VARIABLE_LIST *vl_ptr, ST_INT num_of_vars)
  {
VARIABLE_LIST *vlist_ptr;
ALT_ACCESS alt_acc;
ST_INT i;
ST_RET rc;

  MLOG_CALWAYS1 ("Number of Variables = %d ", num_of_vars);
  vlist_ptr = vl_ptr;
  for (i = 0; i < num_of_vars; i++)
    {
    MLOG_CALWAYS1 ("Variable %d) ", i+1);
    m_log_var_spec (&vlist_ptr->var_spec);
    if ( vlist_ptr->alt_access_pres )
      {
      rc = ms_asn1_to_aa (vlist_ptr->alt_access.data, 
			  vlist_ptr->alt_access.len, 
			  &alt_acc);
      if (rc == SD_SUCCESS)
        {
        ms_log_alt_access (&alt_acc);
        chk_free (alt_acc.aa);
	}
      else
        {
        MLOG_CALWAYS0 ("Could not decode Alternate Access:");
        MLOG_ALWAYSH (vlist_ptr->alt_access.len,
                      vlist_ptr->alt_access.data);
        }
      }
    else
      {
      MLOG_CALWAYS0 ("Alternate Access : Not Present");
      }
    vlist_ptr++;
    }
  }

/************************************************************************/
/*			m_log_vaspec					*/
/* This routine is used to print a variable access specification.	*/
/************************************************************************/

ST_VOID m_log_vaspec (VAR_ACC_SPEC *va_ptr)
  {
VARIABLE_LIST *vlist_ptr;

  MLOG_CALWAYS0 ("Variable Access Specification");
  if ( va_ptr->var_acc_tag == 0 )
    {
    vlist_ptr = (VARIABLE_LIST *) (va_ptr + 1);
    m_log_var_list (vlist_ptr, va_ptr->num_of_variables);
    }
  else
    {
    m_log_objname (&va_ptr->vl_name);
    }
  }

/************************************************************************/
/*			m_log_var_spec					*/
/* This routine is used to print a variable specification.		*/
/************************************************************************/

ST_VOID m_log_var_spec (VARIABLE_SPEC *vs_ptr)
  {
  switch (vs_ptr->var_spec_tag)
    {
    case  0 :
      m_log_objname (&vs_ptr->vs.name);
      break;
    case  1 :
      m_log_address (&vs_ptr->vs.address);
      break;
    case  2 :
      m_log_address (&vs_ptr->vs.var_descr.address);
      m_log_tdl (vs_ptr->vs.var_descr.type.data, 
		 vs_ptr->vs.var_descr.type.len);
      break;
    case  3 :
      MLOG_CALWAYS0 ("Scattered Access Follows in HEX: ");
      MLOG_ALWAYSH (vs_ptr->vs.sa_descr.len,
                    vs_ptr->vs.sa_descr.data );
      break;
    case  4 :
      MLOG_CALWAYS0 ("Invalidated.");
      break;
    }
  }


/************************************************************************/
/************************************************************************/
/************************************************************************/

ST_RET mlogArrStart (ST_VOID *usr, RUNTIME_TYPE *rt)
  {
  MLOG_CALWAYS0 ("ArrayStart");
  return (SD_SUCCESS);
  }

ST_RET mlogArrEnd   (ST_VOID *usr, RUNTIME_TYPE *rt)
  {
  MLOG_CALWAYS0 ("ArrayEnd");
  return (SD_SUCCESS);
  }

ST_RET mlogStrStart (ST_VOID *usr, RUNTIME_TYPE *rt, SD_CONST RUNTIME_TYPE *rt_head)
  {
  MLOG_CALWAYS0 ("StructureStart");
  return (SD_SUCCESS);
  }

ST_RET mlogStrEnd   (ST_VOID *usr, RUNTIME_TYPE *rt)
  {
  MLOG_CALWAYS0 ("StructureEnd");
  return (SD_SUCCESS);
  }

ST_RET mlogInt8  (ST_VOID *usr, ST_INT8    *data_ptr, RUNTIME_TYPE *rt)
  {
ST_INT int_val;

  int_val = (ST_INT) (*data_ptr);
  MLOG_CALWAYS1 ("INTEGER 8 value   : %d",int_val);
  return (SD_SUCCESS);
  }

ST_RET mlogInt16 (ST_VOID *usr, ST_INT16   *data_ptr, RUNTIME_TYPE *rt)
  {
ST_INT i;

  i = (ST_INT) (*data_ptr);
  MLOG_CALWAYS1 ("INTEGER 16 value  : %d", (ST_INT) i);
  return (SD_SUCCESS);
  }

ST_RET mlogInt32 (ST_VOID *usr, ST_INT32   *data_ptr, RUNTIME_TYPE *rt)
  {
  MLOG_CALWAYS1 ("INTEGER 32 value  : %ld", (ST_LONG) *data_ptr);
  return (SD_SUCCESS);
  }

#ifdef INT64_SUPPORT
ST_RET mlogInt64 (ST_VOID *usr, ST_INT64   *data_ptr, RUNTIME_TYPE *rt)
  {
  MLOG_CALWAYS1 ("INTEGER 64 value  : " S_FMT_INT64, *data_ptr);
  return (SD_SUCCESS);
  }
#endif

ST_RET mlogUint8 (ST_VOID *usr, ST_UINT8   *data_ptr, RUNTIME_TYPE *rt)
  {
ST_INT int_val;

  int_val = (ST_INT) (*data_ptr);
  MLOG_CALWAYS1 ("UNSIGNED 8 value   : %d",int_val);
  return (SD_SUCCESS);
  }

ST_RET mlogUint16 (ST_VOID *usr, ST_UINT16  *data_ptr, RUNTIME_TYPE *rt)
  {
ST_INT i;

  i = *data_ptr;
  MLOG_CALWAYS1 ("UNSIGNED 16 value  : %d", (ST_INT) i);
  return (SD_SUCCESS);
  }

ST_RET mlogUint32 (ST_VOID *usr, ST_UINT32  *data_ptr, RUNTIME_TYPE *rt)
  {
  MLOG_CALWAYS1 ("UNSIGNED 32 value  : %lu", (ST_ULONG) *data_ptr);
  return (SD_SUCCESS);
  }

#ifdef INT64_SUPPORT
ST_RET mlogUint64 (ST_VOID *usr, ST_UINT64  *data_ptr, RUNTIME_TYPE *rt)
  {
  MLOG_CALWAYS1 ("UNSIGNED 64 value  : " S_FMT_UINT64, *data_ptr);
  return (SD_SUCCESS);
  }
#endif

ST_RET mlogFlt   (ST_VOID *usr, ST_FLOAT   *data_ptr, RUNTIME_TYPE *rt)
  {
ST_DOUBLE  d;

  d =  (ST_DOUBLE) (*(ST_FLOAT *) data_ptr);
  MLOG_CALWAYS1 ("ST_FLOAT value       : %f", d);
  return (SD_SUCCESS);
  }

ST_RET mlogDbl   (ST_VOID *usr, ST_DOUBLE  *data_ptr, RUNTIME_TYPE *rt)
  {
  MLOG_CALWAYS1 ("ST_DOUBLE value      : %f", *((ST_DOUBLE *) data_ptr));
  return (SD_SUCCESS);
  }

ST_RET mlogOct   (ST_VOID *usr, ST_UCHAR   *data_ptr, RUNTIME_TYPE *rt)
  {
ST_INT i;
ST_INT16 *sp;

  if (rt->u.p.el_len < 0)
    {
    sp = (ST_INT16 *) data_ptr;
    i = *sp;
    MLOG_CALWAYS1 ("OCTET STRING length: %d", i);
    MLOG_CALWAYS0 ("OCTET STRING value: ");
    MLOG_ALWAYSH (i, (data_ptr+2));
    }
  else
    {
    MLOG_CALWAYS0 ("OCTET STRING value: ");
    MLOG_ALWAYSH (abs (rt->u.p.el_len), data_ptr);
    }
  return (SD_SUCCESS);
  }

ST_RET mlogBool  (ST_VOID *usr, ST_BOOLEAN *data_ptr, RUNTIME_TYPE *rt)
  {
ST_BOOLEAN *bptr;
ST_INT int_val;

  bptr = (ST_BOOLEAN *) data_ptr;
  int_val = (ST_INT) (*bptr);
  MLOG_CALWAYS1 ("ST_BOOLEAN value     : %d", int_val);
  return (SD_SUCCESS);
  }

ST_RET mlogBcd1  (ST_VOID *usr, ST_INT8    *data_ptr, RUNTIME_TYPE *rt)
  {
ST_UCHAR *ubyteptr;
ST_INT int_val;

  ubyteptr = (ST_UCHAR *) data_ptr;
  int_val = (ST_INT) (*ubyteptr);
  MLOG_CALWAYS1 ("BCD 8 value       : %d", int_val);
  return (SD_SUCCESS);
  }

ST_RET mlogBcd2  (ST_VOID *usr, ST_INT16   *data_ptr, RUNTIME_TYPE *rt)
  {
ST_INT i;
ST_INT16 *sp;

  sp = (ST_INT16 *) data_ptr;
  i = *sp;
  MLOG_CALWAYS1 ("BCD 16 value  : %d", (ST_INT) i);
  return (SD_SUCCESS);
  }

ST_RET mlogBcd4  (ST_VOID *usr, ST_INT32   *data_ptr, RUNTIME_TYPE *rt)
  {
  MLOG_CALWAYS1 ("BCD 32 value      : %ld", (ST_LONG)*((ST_INT32 *) data_ptr));
  return (SD_SUCCESS);
  }

ST_RET mlogBs    (ST_VOID *usr, ST_UCHAR   *data_ptr, RUNTIME_TYPE *rt)
  {
ST_INT i;
ST_INT abs_len;
ST_INT16 *sp;

  if (rt->u.p.el_len < 0)
    {
    sp = (ST_INT16 *) data_ptr;
    i = *sp;
    abs_len = (i+7) / 8;
    MLOG_CALWAYS1 ("BIT STRING length: %d", i);
    MLOG_CALWAYS0 ("BIT STRING value: ");
    MLOG_ALWAYSH (abs_len, (data_ptr+2));
    }
  else
    {
    MLOG_CALWAYS0 ("BIT STRING value  : ");
    abs_len = (abs (rt->u.p.el_len)+7) / 8;
    MLOG_ALWAYSH (abs_len, data_ptr);
    }
  return (SD_SUCCESS);
  }

ST_RET mlogVis   (ST_VOID *usr, ST_CHAR    *data_ptr, RUNTIME_TYPE *rt)
  {
  MLOG_CALWAYS1 ("VIS STRING value  : %s", data_ptr);
  return (SD_SUCCESS);
  }

ST_RET mlogBt4   (ST_VOID *usr, ST_INT32   *data_ptr, RUNTIME_TYPE *rt)
  {
  MLOG_CALWAYS1 ("BTOD Number ms since midnight: %ld", (ST_LONG) *data_ptr);
  MLOG_CALWAYS0 ("BTOD Number days since 1/1/1984: Not included");
  return (SD_SUCCESS);
  }

ST_RET mlogBt6   (ST_VOID *usr, ST_INT32   *data_ptr, RUNTIME_TYPE *rt)
  {
STDTIME                 st;
STDTIME_STRING          ss;

  MmsTimeOfDay6ToStdTime ((STDTIME_MMS_TIMEOFDAY6 *)data_ptr, &st);
  StdTimeToStdTimeString (&st, &ss);
  MLOG_CALWAYS3 ("BTOD days/ms: %ld / %ld = %s",(ST_LONG) *(data_ptr+1),(ST_LONG) *data_ptr, ss.str);

  return (SD_SUCCESS);
  }

ST_RET mlogGt   (ST_VOID *usr, time_t     *data_ptr, RUNTIME_TYPE *rt)
  {
  MLOG_CALWAYS1 ("TIME value        : %s", ctime (data_ptr));
  return (SD_SUCCESS);
  }

ST_RET mlogUtc   (ST_VOID *usr, MMS_UTC_TIME *data_ptr, RUNTIME_TYPE *rt)
  {
STDTIME                 st;
STDTIME_STRING          ss;

  MmsUtcTimeToStdTime (data_ptr, &st);
  StdTimeToStdTimeString (&st, &ss);
  MLOG_CALWAYS6 ("UTC Time seconds/fraction/quality: %lu (0x%8.8lX) / %lu (0x%8.8lX) / 0x%2.2lX = %s",
		  (ST_ULONG) data_ptr->secs, (ST_ULONG) data_ptr->secs, 
		  (ST_ULONG) data_ptr->fraction, (ST_ULONG) data_ptr->fraction,
		  (ST_ULONG) data_ptr->qflags, ss.str);
  return (SD_SUCCESS);
  }

ST_RET mlogUtf8 (ST_VOID *usr, ST_UCHAR *data_ptr, RUNTIME_TYPE *rt)
  {
  /* Can't log Unicode chars, so just log Hex data.	*/
  MLOG_CALWAYS0 ("UTF8string value: ");
  MLOG_ALWAYSH (rt->el_size, data_ptr);
  return (SD_SUCCESS);
  }

#ifndef SX_NO_ARB_DATA
SXD_ARB_DATA_CTRL mlog_arb_log_ctrl =
  {
  mlogArrStart, /* arrStart 	*/
  mlogArrEnd, 	 /* arrEnd    	*/
  mlogStrStart, /* strStart  	*/
  mlogStrEnd, 	 /* strEnd    	*/
  mlogInt8,  	/* int8      	*/
  mlogInt16,	/* int16     	*/
  mlogInt32, 	/* int32     	*/
#ifdef INT64_SUPPORT
  mlogInt64, 	/* int64     	*/
#endif
  mlogUint8, 	/* uint8     	*/
  mlogUint16,	/* uint16    	*/
  mlogUint32,	/* uint32    	*/
#ifdef INT64_SUPPORT
  mlogUint64,	/* uint64    	*/
#endif
  mlogFlt,   	/* flt       	*/
  mlogDbl,   	/* dbl       	*/
  mlogOct, 	/* oct       	*/
  mlogBool, 	/* bool      	*/
  mlogBcd1, 	/* bcd1      	*/
  mlogBcd2, 	/* bcd2      	*/
  mlogBcd4, 	/* bcd4      	*/
  mlogBs, 	/* bs	       	*/
  mlogVis, 	/* vis       	*/
  mlogBt4, 	/* Bin Time    	*/
  mlogBt6, 	/* Bin Time    	*/
  mlogGt, 	/* Gen Time    	*/
  mlogUtc, 	/* UTC Time    	*/
  mlogUtf8, 	/* UTF8string  	*/
  };
#endif

/************************************************************************/
/*			m_log_data					*/
/* This routine is used to log MMS Data					*/
/************************************************************************/

#define MAX_TDL_LEN  2000

ST_VOID m_log_data (ST_INT asn1_data_len, ST_UCHAR *asn1_data)
  {
ST_CHAR *localData;
ST_INT localDataLen;
RUNTIME_TYPE *derivedRt;
ST_INT numDerivedRt;
ST_CHAR tdl_buf[MAX_TDL_LEN];
ST_RET ret;
ST_INT tdl_len;

/* Now we will to derive a runtime type from the data, and convert it	*/
/* to local format.							*/

  derivedRt = NULL;
  localData = NULL;
  ret = ms_asn1_data_to_locl (asn1_data, asn1_data_len, 
			      (ST_VOID **) &localData, &localDataLen,
			      &derivedRt, &numDerivedRt);
  if (ret != SD_SUCCESS)
    {
    MLOG_CALWAYS1 ("Could not convert ASN.1 data to local, ret = 0x%04x", ret);
    MLOG_CALWAYS0 ("ASN.1 Data:");
    MLOG_ALWAYSH (asn1_data_len, asn1_data);
    return;
    }

/* Derive the TDL from the runtime type here, then log it.		*/
  tdl_len = ms_runtime_to_tdl (derivedRt, numDerivedRt,
			   tdl_buf, MAX_TDL_LEN);

  if (tdl_len == 0)
    {			/* error	*/
    MLOG_CALWAYS0 ("ERROR converting Runtime to TDL");
    }
  else
    {
    if ((ST_INT) strlen (tdl_buf) < slog_max_msg_size_get(sLogCtrl) - 20)
      {
      MLOG_CALWAYS1 ("Derived TDL: %s", tdl_buf);
      }
    else
      {
      MLOG_CALWAYS0 ("Derived TDL: too long to log ...");
      }
    }

/* Now we can use the arb data handler to log the data in it's native	*/
/* format, sort of.							*/
#ifndef SX_NO_ARB_DATA
  sxd_process_arb_data (localData, derivedRt, numDerivedRt,
			NULL,  &mlog_arb_log_ctrl, NULL);
#endif

/* OK, now free all the stuff we allocated along the way.		*/
  chk_free (derivedRt);
  chk_free (localData);
  }

/************************************************************************/
/************************************************************************/
/*			m_log_tdl					*/
/************************************************************************/

ST_VOID m_log_tdl (ST_UCHAR *asn1, ST_INT asn1_len)
  {
ST_CHAR *tdl;
ST_CHAR *asn1Copy;

  asn1Copy = (ST_CHAR *) chk_malloc (asn1_len);
  memcpy (asn1Copy, asn1, asn1_len);
  tdl = ms_asn1_to_tdl ((ST_UCHAR *) asn1Copy, asn1_len, 5 * asn1_len);
  if (tdl)
    {
    if ((ST_INT) strlen (tdl) < slog_max_msg_size_get(sLogCtrl) - 10)
      {
      MLOG_CALWAYS0 ("Type Specification Follows in TDL: ");
      MLOG_CALWAYS1 ("  '%s'", tdl);
      }
    else
      {
      MLOG_CALWAYS0 ("Type Specification TDL too long to log ...");
      MLOG_ALWAYSH ((ST_INT) strlen (tdl), tdl);
      }
    chk_free (tdl);
    }
  else
    {
    MLOG_CALWAYS0 ("  Could not convert following Type Specification to TDL: ");
    MLOG_ALWAYSH (asn1_len, asn1);
    }
  chk_free (asn1Copy);
  }


