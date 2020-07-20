/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : ms_size.c						*/
/* PRODUCT(S)  : MMSEASE						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains support functions for estimating maximum	*/
/*	size of Variable Access service PDUs (Read, Write, and InfoRpt).*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 11/23/11  KCR           Added casting for proper 64bit compile       */
/* 02/25/10  JRB	   Chg arr_loops, arr_loop_level to local.	*/
/* 04/14/03  JRB     09    Eliminate compiler warnings.			*/
/* 04/02/03  JRB     08    Add UTF8string support (see RT_UTF8_STRING).	*/
/* 11/14/01  EJV     07    Added support for new MMS type UtcTime:	*/
/*			   ms_data_size: added case for RT_UTC_TIME	*/
/* 10/18/00  RKR     06    Changed RT_UNSIGNED in ms_data_size		*/
/* 09/15/00  MDE     05    Fixed MMS-EASE compile error			*/
/* 09/07/00  MDE     04    Added MMS-LITE support			*/
/* 02/10/98  MDE     03    Now don't use runtime type loops element	*/
/* 08/15/97  MDE     02    Minor cleanup				*/
/* 06/09/97  MDE     01    Changed Runtime Type, and it's use		*/
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
#if defined(MMS_LITE)
#include "mvl_defs.h"
#endif

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/

#if defined(DEBUG_SISCO) && defined(S_MT_SUPPORT)
static ST_CHAR *thisFileName = __FILE__;
#endif

static ST_LONG address_size (VAR_ACC_ADDR *address, ST_BOOLEAN req);
static ST_LONG obj_name_size (OBJECT_NAME *name, ST_BOOLEAN req);
static ST_LONG ms_data_size (RUNTIME_TYPE *rt_ptr, ST_INT rt_num, 
			     ST_INT alt_len, ST_UCHAR *alt_ptr,	
			     ST_BOOLEAN req);

/************************************************************************/
/************************************************************************/
/* To allow the user to specify either MMS-EASE or UNKNOWN PDU style,	*/
/* the following definitions & data structures are used.		*/
/* See ms_size.doc for information on how these were derived.		*/

ST_INT ms_req_bld_id  = UNKNOWN_BUILDER;
ST_INT ms_resp_bld_id = UNKNOWN_BUILDER;

/* Because the MMS-EASE ASN.1 tools are a bit conservitive, need to 	*/
/* provide for a little extra overhead.					*/
ST_INT ms_size_oh_pad = ASN1_MAX_ELEMENT_OVERHEAD;

ST_LONG large_el_oh[] = 
  {
  6L,			/* MMS-EASE is PDU Builder 			*/
  8L			/* UNKNOWN PDU Builder				*/
  };
ST_LONG med_el_oh[] = 
  {
  4L,			/* MMS-EASE is PDU Builder 			*/
  8L			/* UNKNOWN PDU Builder				*/
  };
ST_LONG small_el_oh[] = 
  {
  2L,			/* MMS-EASE is PDU Builder 			*/
  8L			/* UNKNOWN PDU Builder				*/
  };

#define LARGE_EL_REQ_OH   large_el_oh[ms_req_bld_id]
#define MED_EL_REQ_OH     med_el_oh[ms_req_bld_id]
#define SMALL_EL_REQ_OH   small_el_oh[ms_req_bld_id]

#define LARGE_EL_RESP_OH  large_el_oh[ms_resp_bld_id]
#define MED_EL_RESP_OH    med_el_oh[ms_resp_bld_id]
#define SMALL_EL_RESP_OH  small_el_oh[ms_resp_bld_id]

/************************************************************************/
/************************************************************************/
/* Overhead for various types of MMS ASN.1 data elements		*/


#define CONF_REQ_PDU_OH	  (ms_size_oh_pad+MED_EL_REQ_OH+SMALL_EL_REQ_OH+4)
#define CONF_RESP_PDU_OH  (ms_size_oh_pad+MED_EL_RESP_OH+SMALL_EL_RESP_OH+4)
#define UNCONF_PDU_OH	  (ms_size_oh_pad+MED_EL_REQ_OH)

/* Overhead for empty PDU's of various types				*/
#define READ_REQ_OH	(CONF_REQ_PDU_OH  + (MED_EL_REQ_OH * 2))
#define READ_RESP_OH	(CONF_RESP_PDU_OH + (MED_EL_RESP_OH * 2))
#define WRITE_REQ_OH	(CONF_REQ_PDU_OH  + (MED_EL_REQ_OH * 2))
#define WRITE_RESP_OH	(CONF_RESP_PDU_OH + MED_EL_RESP_OH)
#define INFO_REQ_OH	(UNCONF_PDU_OH    + (MED_EL_REQ_OH * 2))

/* Overhead for various components of interest				*/
#define SPEC_IN_RSLT_SIZE   (MED_EL_REQ_OH + 1) 

/* Object name overhead							*/
#define VMD_OBJ_NAME_REQ_OH	SMALL_EL_REQ_OH
#define DOM_OBJ_NAME_REQ_OH	(MED_EL_REQ_OH + (SMALL_EL_REQ_OH * 2))
#define AA_OBJ_NAME_REQ_OH	SMALL_EL_REQ_OH

#define VMD_OBJ_NAME_RESP_OH	SMALL_EL_RESP_OH
#define DOM_OBJ_NAME_RESP_OH	(MED_EL_RESP_OH + (SMALL_EL_RESP_OH * 2))
#define AA_OBJ_NAME_RESP_OH	SMALL_EL_RESP_OH


#define GENTIME_LEN	 18L

/************************************************************************/
/************************************************************************/

/************************************************************************/
/*			     ms_init_va_size				*/
/* This function is called from the user as a first step in estimating	*/
/* the maximum size of both the request and response PDUs of any of the	*/
/* Variable Access services (Read, Write, and InformationReport).  This	*/
/* function initializes the values pointed to by req_size and resp_size */
/* to the overhead in the PDU (before determining the size of the data 	*/
/* and specification of each variable to be included).  The companion	*/
/* function ms_va_size is then called for each variable to add on the 	*/
/* estimated size attributable to that variable.  This function returns */
/* 0 for success, else an error code.					*/
/************************************************************************/

ST_RET ms_init_va_size (ST_INT op, ST_UCHAR spec_in_rslt, ST_INT16 var_acc_tag,
	OBJECT_NAME *vl_name, ST_LONG *req_size, ST_LONG *resp_size)
  {
ST_RET ret;

  S_LOCK_COMMON_RESOURCES ();
  ret = SD_SUCCESS;
  switch (op)
    {
    case MMSOP_READ:
/* initialize sizes to include all overhead for basic request		*/
      *req_size = READ_REQ_OH;
      *resp_size = READ_RESP_OH;

/* To send request for spec in result takes extra bytes, do does	*/
/* sending the VA spec in the response.					*/
      if (spec_in_rslt)
        {
	*req_size += SPEC_IN_RSLT_SIZE;
	*resp_size += MED_EL_RESP_OH;
	}
    break;

    case MMSOP_WRITE:
/* initialize sizes to include all overhead for basic request		*/
      *req_size = WRITE_REQ_OH;
      *resp_size = WRITE_RESP_OH;
    break;

    case MMSOP_INFO_RPT:
/* initialize sizes to include all overhead for basic request		*/
      *req_size = INFO_REQ_OH;
      *resp_size = 0;
    break;

    default :
      ret = MVE_WRONG_OP;
    }

/* If the variable specification is a named list, add in the name, else	*/
/* add in the sequence of sequence constructor overhead			*/
/* Note that this is done here, NOT in later calls to ms_va_size	*/

  if (var_acc_tag == VAR_ACC_NAMEDLIST) /* list of variables case	*/
    *req_size += MED_EL_REQ_OH + obj_name_size (vl_name,SD_TRUE);
  else
    *req_size += MED_EL_REQ_OH; 			/* seq of seq */

  S_UNLOCK_COMMON_RESOURCES ();
  return (ret);
  }


/************************************************************************/
/*			        ms_va_size				*/
/* This function is called from the user repetitively for each variable	*/
/* expected to be included in a Read, Write, or InfoRpt PDU.  Note that */
/* the function ms_init_va_size should be called first to initialize 	*/
/* the two outputs req_size and resp_size.  The values pointed to by 	*/
/* req_size and resp_size are incremented by the appropriate amount, 	*/
/* so that after this function has been called for all variables, they	*/
/* represent valid estimates of the size of the request and response	*/
/* PDUs for the given service (indicated by op).  In some cases, the	*/
/* type of the variable may be indicated both by the vl_ptr argument 	*/
/* and by the type argument; in this case, both must still be specified */
/* and it is up to the user to verify that they are equal.  This 	*/
/* function returns 0 for success, else an error code.			*/
/*									*/
/* NOTE: In the case of accessing a variable list object (as opposed 	*/
/*	 to a list of variable objects), a variable_list structure must	*/
/*	 be built and filled in appropriately, with var_spec_tag set to	*/
/*	 5.  (Otherwise, the value pointed to by req_size and, in 	*/
/*	 certain cases, the value pointed to by resp_size will be 	*/
/*	 much larger than necessary.					*/
/************************************************************************/

#if !defined(MMS_LITE)
ST_RET ms_va_size (ST_INT op, ST_UCHAR spec_in_rslt, ST_INT16 var_acc_tag,
	VARIABLE_LIST *vl_ptr, NAMED_TYPE *type,
	ST_LONG *req_size, ST_LONG *resp_size)
#else
ST_RET ms_va_size (ST_INT op, ST_UCHAR spec_in_rslt, ST_INT16 var_acc_tag,
	VARIABLE_LIST *vl_ptr, ST_INT type_id,
	ST_LONG *req_size, ST_LONG *resp_size)
#endif
  {
ST_LONG size;
ST_INT alt_acc_len;
RUNTIME_TYPE *rt; 
ST_INT rt_num; 
#if defined(MMS_LITE)
ST_INT rc;
#endif

#if !defined(MMS_LITE)
  rt = type->rt_head;
  rt_num = type->rt_num;
#else
  rc = mvl_get_runtime (type_id, &rt, &rt_num);
  if (rc != SD_SUCCESS)
    return (rc);
#endif

  S_LOCK_COMMON_RESOURCES ();

/* non functional at this time */
  if (vl_ptr->alt_access_pres)
    alt_acc_len = vl_ptr->alt_access.len;
  else
    alt_acc_len = 0;

/* get the size of the data element based on the MMS type and add the 	*/
/* data element size to the target PDU					*/

  if (op == MMSOP_READ)		/* read response carries data		*/
    {
    size = ms_data_size (rt, rt_num, alt_acc_len, 
			 vl_ptr->alt_access.data,SD_FALSE);
    *resp_size += size;		
    }
  else	/* WRITE or INFO */
    {
    size = ms_data_size (rt, rt_num, alt_acc_len,
			 vl_ptr->alt_access.data,SD_TRUE);
    *req_size += size;

    if (op == MMSOP_WRITE)	/* Write response carries result	*/
      *resp_size += SMALL_EL_RESP_OH + 1; /* assume failure (+1)       	*/  
    }
  if (size <= 0)		/* verify the data size was OK		*/
    {
    S_UNLOCK_COMMON_RESOURCES ();
    return (MVE_TYPENAME);
    }

/* Now calculate the size of the variable access spec for this var.	*/
/* Only req'd if var_acc_tag is list of vars (list name added already)	*/
  if (var_acc_tag == VAR_ACC_VARLIST) 
    {
    size = MED_EL_REQ_OH;
    switch (vl_ptr->var_spec.var_spec_tag)
      {
      case VA_SPEC_NAMED :
        size += obj_name_size (&vl_ptr->var_spec.vs.name,SD_TRUE);
      break;
  
#if !defined(MMS_LITE)
      case VA_SPEC_DESCRIBED :	/* described gets address & type	*/
        size += type->asn1len +
                         address_size (&vl_ptr->var_spec.vs.address,SD_TRUE);
      break;
#endif
  
      case VA_SPEC_ADDRESSED :
        size +=	address_size (&vl_ptr->var_spec.vs.address,SD_TRUE);
      break;
  
      case VA_SPEC_SCATTERED :		/* not currently supported 	*/
      case VA_SPEC_INVALIDATED :
      default:
        S_UNLOCK_COMMON_RESOURCES ();
        return (MVE_VM_SERVICE_NOTSUPP);
      }

/* not really supported at this time, leave in anyway		     	*/
    if (vl_ptr->alt_access_pres)
      size += (ST_LONG) alt_acc_len;

/* Now we have the variable specification size, add to the request	*/
    *req_size += size;

/* Now, if spec in result requested, calculate VA spec size for resp	*/
    if (op == MMSOP_READ && spec_in_rslt)
      {
      size = MED_EL_RESP_OH;
      switch (vl_ptr->var_spec.var_spec_tag)
        {
        case VA_SPEC_NAMED :
          size += obj_name_size (&vl_ptr->var_spec.vs.name,SD_FALSE);
        break;
    
#if !defined(MMS_LITE)
        case VA_SPEC_DESCRIBED :	/* described gets address & type	*/
          size += type->asn1len +
                       address_size (&vl_ptr->var_spec.vs.address,SD_FALSE);
    
        break;
#endif
    
        case VA_SPEC_ADDRESSED :
          size += address_size (&vl_ptr->var_spec.vs.address,SD_FALSE);
        break;
    
        case 5:				/* variable list object case	*/
          S_UNLOCK_COMMON_RESOURCES ();
          return (SD_SUCCESS);      	/* No variable spec. req'd	*/
        break;
    
        case VA_SPEC_SCATTERED :		/* not currently supported 	*/
        case VA_SPEC_INVALIDATED :
        default:
          S_UNLOCK_COMMON_RESOURCES ();
          return (MVE_VM_SERVICE_NOTSUPP);
        }
      *resp_size += size;		 	
      }
    } /* if VAR_ACC_VARLIST */

/* Now, if WRITE, need to add write result 				*/

  
  S_UNLOCK_COMMON_RESOURCES ();
  return (SD_SUCCESS);
  }


/************************************************************************/
/*			        obj_name_size				*/
/* This function is called from ms_init_va_size and ms_va_size to	*/
/* calculate (exactly) the size that a particular object name will take	*/
/* up in a request or response PDU for the Read, Write, and InfoReport	*/
/* services.  Returns the size of the object name, or 0 if there is a	*/
/* problem.								*/
/************************************************************************/

static ST_LONG obj_name_size (OBJECT_NAME *name, ST_BOOLEAN req)
  {
ST_LONG oh;

  switch (name->object_tag)
    {
    case VMD_SPEC :
      if (req)	  		/* if calculating for a request		*/
        oh = VMD_OBJ_NAME_REQ_OH;
      else
        oh = VMD_OBJ_NAME_RESP_OH;

      return (oh + (ST_LONG) strlen (name->obj_name.vmd_spec));
    break;

    case DOM_SPEC :
      if (req)			/* if calculating for a request		*/
        oh = DOM_OBJ_NAME_REQ_OH;
      else
        oh = DOM_OBJ_NAME_RESP_OH;

      return (oh + (ST_LONG) (strlen (name->obj_name.item_id) + 
			     		strlen (name->domain_id)));
    break;

    case AA_SPEC :
      if (req)			/* if calculating for a request		*/
        oh = AA_OBJ_NAME_REQ_OH;
      else
        oh = AA_OBJ_NAME_RESP_OH;

      return (oh + (ST_LONG) strlen (name->obj_name.aa_spec));
    break;
    }

  mms_op_err = MVE_OBJECT_SCOPE;
  return (0L);				/* indicates error condition	*/
  }


/************************************************************************/
/*			        address_size				*/
/* This function is called from ms_va_size to calculate (exactly) the 	*/
/* size that a particular address will take up in a request or response */
/* PDU for the Read, Write, and InfoReport services.  Returns the size 	*/
/* of the address, or 0 if there is a problem.				*/
/************************************************************************/

static ST_LONG address_size (VAR_ACC_ADDR *address, ST_BOOLEAN req)
  {
ST_LONG size;
ST_LONG numaddr;

  switch (address->addr_tag)
    {
    case NUM_ADDR :
      if (req)    			/* if calculating for a request		*/
        size = SMALL_EL_REQ_OH;
      else
        size = SMALL_EL_RESP_OH;
      numaddr = address->addr.num_addr;
      if (numaddr < 128L)
	size += 1;
      else if (numaddr < 32768L)
        size += 2;
      else if (numaddr < 8388608L)
        size += 3;
      else
	size += 4;
    break;

    case SYM_ADDR :
      if (req)    			/* if calculating for a request		*/
        size = MED_EL_REQ_OH;
      else
        size = MED_EL_RESP_OH;
      size += (ST_INT) strlen (address->addr.sym_addr);
    break;

    case UNCON_ADDR :
      if (req)    			/* if calculating for a request		*/
        size = MED_EL_REQ_OH;
      else
        size = MED_EL_RESP_OH;
      size += address->addr.unc_addr.unc_len;
    break;

    default:
      mms_op_err = MVE_ADDR;
      return (0);
    }
  return (size);
  }


/************************************************************************/
/************************************************************************/
/************************************************************************/
/*			        ms_data_size				*/
/* This function is called from the user or the MMS-EASE virtual 	*/
/* machine to estimate the size that the data corresponding to a 	*/
/* particular type will take up in a request or response PDU for the 	*/
/* Read, Write, and InfoReport services.  Returns the size of the data, */
/* or 0 if there is a problem.  (A rough error code can be found in	*/
/* mms_op_err; a more detailed one in asn1_pdu_dec_err.)			*/
/*									*/
/* Note: Alternate access may be used to modify a type, thereby		*/
/*	 affecting the size of the corresponding data.  However,	*/
/*	 the alternate access inputs are disregarded at this time.	*/
/*	 This will be fixed in a later revision.			*/
/************************************************************************/

static ST_LONG ms_data_size (RUNTIME_TYPE *rt_ptr, ST_INT rt_num, 
			     ST_INT alt_len, ST_UCHAR *alt_ptr,	
			     ST_BOOLEAN req)
  {
ST_LONG size;
RUNTIME_TYPE *rt_end; 
ST_LONG med_oh;
ST_LONG small_oh;
ST_UINT ellen;
ST_LONG len;
/* NOTE: array was arr_loop_buf & arr_loops pointed to it. Don't need both.*/
ST_INT arr_loops [ASN1_MAX_LEVEL];
ST_INT arr_loop_level;
ST_UINT el_size;	/* num bytes to encode data	*/

  memset (arr_loops, 0, sizeof (arr_loops));	/* CRITICAL: start clean.*/
  arr_loop_level = 0;

  if (req)			/* if calculating for a request		*/
    {
    med_oh = MED_EL_REQ_OH;
    small_oh = SMALL_EL_REQ_OH;
    }
  else				/* calculating for response		*/
    {
    med_oh = MED_EL_RESP_OH;
    small_oh = SMALL_EL_RESP_OH;
    }

  rt_end = rt_ptr + rt_num;  	/* end block			*/

  size = 0;
  while (rt_ptr < rt_end)
    {
    if (rt_ptr->el_tag == RT_ARR_END)	/* treat case of array ending	*/
      {					/* loops set at start of array	*/
      if (--arr_loops[arr_loop_level] > 0) /* if need to do next ar elmnt	*/
       rt_ptr -= rt_ptr->u.arr.num_rt_blks; /* mv rt_ptr to start of arr	*/
      else
        --arr_loop_level;
      }

    ellen = abs (rt_ptr->u.p.el_len); /* make sure positive  	*/
    switch (rt_ptr->el_tag)
      {
      case RT_ARR_START :	/* arrays don't have to be aligned	*/
        ++arr_loop_level;
        arr_loops[arr_loop_level] = rt_ptr->u.arr.num_elmnts;
				/* let fall through to STR_MMSOP_START	*/

      case RT_STR_START :	/* structure starting			*/
        size += med_oh;		/* add in data element overhead		*/
      break;

      case RT_UNSIGNED :		/* unsigned integer		*/
        size += small_oh + ellen + 1;   /* high order bit set needs 0x00*/
      break;

      case RT_UTC_TIME :		/* UTC time			*/
      case RT_BINARY_TIME :		/* binary time			*/
      case RT_INTEGER :			/* integer			*/
      case RT_FLOATING_POINT : 		/* float			*/
        size += small_oh + ellen;
      break;

      case RT_VISIBLE_STRING : 		/* visible string		*/
      case RT_OCTET_STRING :		/* octet string 		*/
        size += ellen;
        if (ellen < 128)
          size += small_oh;
        else
          size += med_oh;
      break;

      case RT_GENERAL_TIME : 		/* generalized time		*/
        size += small_oh + GENTIME_LEN;
      break;

      case RT_BOOL :			/* boolean			*/
        size += small_oh +1;
      break;

      case RT_BCD :			/* bcd				*/
        size += small_oh + ellen;
      break;

      case RT_BIT_STRING :		/* bit string			*/
	len = 1 + CALC_BIT_LEN (ellen);

        if (len < 128) 
          size += small_oh + len; 	/* add in data element overhead	*/
        else
          size += med_oh + len;		/* add in data element overhead	*/

      break;

      case RT_UTF8_STRING :		/* Unicode UTF8 string		*/
        el_size = ellen*4;		/* Could take 4 bytes per char	*/
        size += el_size;
        if (el_size < 128)
          size += small_oh;
        else
          size += med_oh;
      break;

      case RT_ARR_END :		/* array done				*/
      case RT_STR_END :		/* structure done			*/
      break;

      default :			/* should not be any other tag	*/
        return (0);
      break;
      }
    rt_ptr++;			/* point to next block			*/
    }  /* while (rt blocks) */
  return (size);
  }


