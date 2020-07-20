/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	       1998-2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : mvluleaf.c						*/
/* PRODUCT(S)  : MMSEASE						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 03/01/12  JRB	   Fix Klocwork warnings.			*/
/* 01/12/10  JRB     07    Fix log messages.				*/
/* 10/11/06  JRB     06    Add mvlu_set_leaf_param_name2.		*/
/*			   mvlu_load_xml_leaf_file: fix error return.	*/
/* 09/13/05  JRB     05    Add ERR log if mvlu_find_..._fun_index fails.*/
/* 07/08/04  JRB     04    Fix "Reference" log msg.			*/
/* 12/20/02  JRB     03    mvlu_set_leaf_param moved here.		*/
/*			   MVL_LEAF_FUN_LOOKUP_ENABLE MUST be defined	*/
/*			   for all except mvlu_set_leaf_param.		*/
/* 12/11/02  JRB     02    Use new mvl_uca.h				*/
/* 11/12/02  MDE     01    New						*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "glbsem.h"
#include "mmsdefs.h"
#include "mms_pvar.h"
#include "mms_vvar.h"
#include "mvl_uca.h"
#include "mvl_log.h"

#if defined(MVL_UCA)	/* This entire module is only valid for UCA.	*/

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/* Most of the code in this module requires this to be defined.	*/
#if defined(MVLU_LEAF_FUN_LOOKUP_ENABLE)

/************************************************************************/

#define MAX_LEAF_NAME	200
#define MAX_FUN_NAME	200
#define MAX_REF_NAME	200

static ST_VOID _getAttribVal (ST_CHAR *xmlBuf, ST_CHAR *attribName, 
		       ST_CHAR *dest, ST_INT destSize);

/************************************************************************/
/************************************************************************/
/*			mvlu_load_xml_leaf_file				*/
/* NOTE: continue after errors, but save the last error detected and	*/
/*       return it.							*/
/************************************************************************/

ST_RET mvlu_load_xml_leaf_file (ST_CHAR *fileName)
  {
FILE *fp;
ST_CHAR leafName[MAX_LEAF_NAME+1];
ST_CHAR rdIndFunName[MAX_FUN_NAME+1];
ST_CHAR wrIndFunName[MAX_FUN_NAME+1];
ST_CHAR refString[MAX_REF_NAME+1];
ST_CHAR buf[1000];
ST_CHAR *p;
ST_INT setFlags;
ST_RET ret;
ST_RET last_error = SD_SUCCESS;	/* save last error detected	*/

  fp = fopen (fileName, "rt");
  if (fp == NULL)
    {
    MVL_LOG_NERR1 ("Could not open leaf map XML file '%s'", fileName);
    return (SD_FAILURE);
    }

/* Now read in the file and set the leaf parameters */
  MVLU_LOG_FLOW1 ("Setting Leaf Parameters using file '%s'", fileName);
  ret = SD_SUCCESS;
  while (SD_TRUE)
    {
    leafName[0] = 0;
    rdIndFunName[0] = 0;
    wrIndFunName[0] = 0;
    refString[0] = 0;

    if (fgets (buf, sizeof(buf) - 1, fp) == NULL)
      break;
    if (strstr (buf, "<Leafmap>") != NULL)
      continue;
    if (strstr (buf, "</Leafmap>") != NULL)
      break;
    p = strstr (buf, "<Leaf ");
    if (!p)
      continue;

    _getAttribVal (p, "Name", leafName, sizeof (leafName));
    _getAttribVal (p, "RdIndFun", rdIndFunName, sizeof (rdIndFunName));
    _getAttribVal (p, "WrIndFun", wrIndFunName, sizeof (wrIndFunName));
    _getAttribVal (p, "Ref", refString, sizeof (refString));
     
    if (leafName[0] == 0)
      {
      MVL_LOG_NERR1 ("'Name' attribute not present in '%s'", buf);
      ret = SD_FAILURE;
      break;
      }

    setFlags = 0;
    if (rdIndFunName[0] != 0)
      setFlags |= MVLU_SET_RD_FUN;

    if (wrIndFunName[0] != 0)
      setFlags |= MVLU_SET_WR_FUN;

    if (refString[0] != 0)
      setFlags |= MVLU_SET_REF;


    ret = mvlu_set_leaf_param_name (setFlags, leafName, 
    				    rdIndFunName, wrIndFunName, refString);
    if (ret != SD_SUCCESS)
      {
      MVL_LOG_NERR1 ("Could not set leaf %s parameters", leafName);
      last_error = ret;	/* save last error detected	*/
      }
    }

  fclose (fp);
  return (last_error);	/* return last error detected	*/
  }


/************************************************************************/
/*			_getAttribVal					*/
/************************************************************************/

static ST_VOID _getAttribVal (ST_CHAR *xmlBuf, ST_CHAR *attribName, 
		       ST_CHAR *dest, ST_INT destSize)
  {
ST_CHAR *p;
ST_CHAR look[50];

  *dest = 0;
  strcpy (look, attribName);
  strcat (look, "=\"");
  p = strstr (xmlBuf, look);
  if (!p)
    return;

  p += strlen (look);
  while (*p != 0 && *p != '"' && destSize > 0)
    {
    *(dest++) = *(p++);
    --destSize;
    }
  *dest = 0;
  }


/************************************************************************/
/*			mvlu_set_leaf_param_name			*/
/************************************************************************/

ST_RET mvlu_set_leaf_param_name (ST_INT setFlags, ST_CHAR *leafName, 
				 ST_CHAR *rdIndFunName, ST_CHAR *wrIndFunName, 
				 ST_CHAR *refString)
  {
ST_RTINT rdIndex;
ST_RTINT wrIndex;
ST_RTREF ref;
ST_RET ret;

  /* Init rdIndex, wrIndex to avoid warnings.	*/
  rdIndex = 0;
  wrIndex = 0;

  if (setFlags & MVLU_SET_RD_FUN)
    {
    rdIndex = mvlu_find_rd_ind_fun_index (rdIndFunName);
    if (rdIndex < 0)
      {
      /* Error already logged in "mvlu_find_rd.." function.	*/
      return (SD_FAILURE);
      }
    }

  if (setFlags & MVLU_SET_WR_FUN)
    {
    wrIndex = mvlu_find_wr_ind_fun_index (wrIndFunName);
    if (wrIndex < 0)
      {
      /* Error already logged in "mvlu_find_wr.." function.	*/
      return (SD_FAILURE);
      }
    }

  ret = u_mvlu_resolve_leaf_ref (leafName, &setFlags, refString, &ref);
  if (ret != SD_SUCCESS)
    {
    MVL_LOG_NERR1 ("Could not resolve reference '%s'", refString);
    return (SD_FAILURE);
    }

  MVLU_LOG_FLOW1 ("Setting Leaf Parameters for leaf '%s'", leafName);
  if (setFlags & MVLU_SET_RD_FUN)
    {
    MVLU_LOG_CFLOW2 ("  Read Indication: %s\t(%d)", rdIndFunName, (int) rdIndex);
    }
  if (setFlags & MVLU_SET_WR_FUN)
    {
    MVLU_LOG_CFLOW2 ("  Write Indication: %s\t(%d)", wrIndFunName, (int) wrIndex);
    }
  if (setFlags & MVLU_SET_REF)
    {
    MVLU_LOG_CFLOW1 ("  Reference: " S_FMT_PTR, ref);
    }
  
  ret = mvlu_set_leaf_param (setFlags, leafName, rdIndex, wrIndex, (ST_RTREF) ref);
  if (ret != SD_SUCCESS)
    {
    MVL_LOG_NERR1 ("Could not set leaf %s parameters", leafName);
    }
  return (ret);
  }

/************************************************************************/
/*			mvlu_find_rd_ind_fun_index			*/
/************************************************************************/

ST_RTINT mvlu_find_rd_ind_fun_index (ST_CHAR *funName)
  {
ST_RTINT rdIndex;

  for (rdIndex = 0; rdIndex < mvluNumRdFunEntries; ++rdIndex)
    {
    if (!strcmp (funName, mvluRdFunInfoTbl[rdIndex].fun_name))
      break;
    }
  if (rdIndex >= mvluNumRdFunEntries)
    {
    MVL_LOG_ERR1 ("Could not find 'leaf read' function '%s'", funName);
    return (-1);
    }

  return (rdIndex);
  }

/************************************************************************/
/*			mvlu_find_wr_ind_fun_index				*/
/************************************************************************/

ST_RTINT mvlu_find_wr_ind_fun_index (ST_CHAR *funName)
  {
ST_RTINT wrIndex;

  for (wrIndex = 0; wrIndex < mvluNumWrFunEntries; ++wrIndex)
    {
    if (!strcmp (funName, mvluWrFunInfoTbl[wrIndex].fun_name))
      break;
    }
  if (wrIndex >= mvluNumWrFunEntries)
    {
    MVL_LOG_ERR1 ("Could not find 'leaf write' function '%s'", funName);
    return (-1);
    }

  return (wrIndex);
  }
/************************************************************************/
#endif	/* defined(MVLU_LEAF_FUN_LOOKUP_ENABLE)	*/
/************************************************************************/

/************************************************************************/
/*			mvlu_set_leaf_param			*/
/* DEBUG: could change this to call mvlu_set_leaf_param2 after type found.*/
/************************************************************************/

ST_RET mvlu_set_leaf_param (ST_INT setFlags, ST_CHAR *leafName,
  			     ST_RTINT rdIndFunIndex, ST_RTINT wrIndFunIndex, 
  			     ST_RTREF ref)
  {
RUNTIME_TYPE *rt;
ST_INT type_id;
ST_CHAR *leafNameBuf;
ST_CHAR *p;

/* Make a working copy */
  leafNameBuf = chk_strdup (leafName);
  rt = NULL;	/* Flag that we failed */

/* The type name must be the first part of this string */
  p = strstr (leafNameBuf, "$");
  if (p)
    {
    *p = 0;
    type_id = mvl_typename_to_typeid (leafNameBuf);
    if (type_id >= 0)
      {
      ++p;
      rt = mvlu_find_rt_leaf (type_id, p);
      }
    else
      {
      MVL_LOG_NERR1 ("Could not find type '%s'", leafNameBuf);
      }
    }
  else
    {
    MVL_LOG_NERR1 ("Could not find type '%s'", leafNameBuf);
    }

  chk_free (leafNameBuf);
  if (!rt)
    return (SD_FAILURE);

  if (setFlags & MVLU_SET_RD_FUN)
    {
    if (u_mvlu_leaf_rd_ind_fun == NULL && rdIndFunIndex >= mvluNumRdFunEntries)
      {
      MVL_LOG_NERR0 ("Read ind function index out of range for use with default leaf read handler");
      return (SD_FAILURE);
      }
    rt->mvluTypeInfo.rdIndFunIndex = rdIndFunIndex;
    }

  if (setFlags & MVLU_SET_WR_FUN)
    {
    if (u_mvlu_leaf_wr_ind_fun == NULL && wrIndFunIndex >= mvluNumWrFunEntries)
      {
      MVL_LOG_NERR0 ("Write ind function index out of range for use with default leaf write handler");
      return (SD_FAILURE);
      }
    rt->mvluTypeInfo.wrIndFunIndex = wrIndFunIndex;
    }

#if defined(MVLU_USE_REF)
  if (setFlags & MVLU_SET_REF)
    rt->mvluTypeInfo.ref = ref;
#endif	/* MVLU_USE_REF	*/
  return (SD_SUCCESS);
  }

/************************************************************************/
/*			mvlu_set_leaf_param2				*/
/* Similar to mvlu_set_leaf_param but uses additional base_var_type_id arg.*/
/************************************************************************/
static ST_RET mvlu_set_leaf_param2 (ST_INT base_var_type_id,
	ST_INT setFlags,
	ST_CHAR *leafName,
	ST_RTINT rdIndFunIndex,
	ST_RTINT wrIndFunIndex,
	ST_RTREF ref)
  {
RUNTIME_TYPE *rt;
ST_CHAR *p;

  /* The name to find starts after the first '$'.	*/
  p = strstr (leafName, "$");
  if (p)
    {
    ++p;	/* point after '$'	*/
    rt = mvlu_find_rt_leaf (base_var_type_id, p);
    if (!rt)
      MVL_LOG_NERR1 ("Cannot find leaf '%s'", leafName);
    }
  else
    {
    MVL_LOG_NERR1 ("Cannot find leaf. No '$' in leaf name '%s'", leafName);
    rt = NULL;
    }

  if (!rt)
    return (SD_FAILURE);

  if (setFlags & MVLU_SET_RD_FUN)
    {
    if (u_mvlu_leaf_rd_ind_fun == NULL && rdIndFunIndex >= mvluNumRdFunEntries)
      {
      MVL_LOG_NERR0 ("Read ind function index out of range for use with default leaf read handler");
      return (SD_FAILURE);
      }
    rt->mvluTypeInfo.rdIndFunIndex = rdIndFunIndex;
    }

  if (setFlags & MVLU_SET_WR_FUN)
    {
    if (u_mvlu_leaf_wr_ind_fun == NULL && wrIndFunIndex >= mvluNumWrFunEntries)
      {
      MVL_LOG_NERR0 ("Write ind function index out of range for use with default leaf write handler");
      return (SD_FAILURE);
      }
    rt->mvluTypeInfo.wrIndFunIndex = wrIndFunIndex;
    }

#if defined(MVLU_USE_REF)
  if (setFlags & MVLU_SET_REF)
    rt->mvluTypeInfo.ref = ref;
#endif	/* MVLU_USE_REF	*/
  return (SD_SUCCESS);
  }

/************************************************************************/
/*			mvlu_set_leaf_param_name2			*/
/* Similar to mvlu_set_leaf_param_name but uses additional base_var_type_id arg.*/
/************************************************************************/
ST_RET mvlu_set_leaf_param_name2 (ST_INT base_var_type_id,
	ST_INT setFlags,
	ST_CHAR *leafName, 
	ST_CHAR *rdIndFunName,
	ST_CHAR *wrIndFunName, 
	ST_CHAR *refString)
  {
ST_RTINT rdIndex;
ST_RTINT wrIndex;
ST_RTREF ref;
ST_RET ret;

  /* Init rdIndex, wrIndex to avoid warnings.	*/
  rdIndex = 0;
  wrIndex = 0;

  if (setFlags & MVLU_SET_RD_FUN)
    {
    rdIndex = mvlu_find_rd_ind_fun_index (rdIndFunName);
    if (rdIndex < 0)
      {
      /* Error already logged in "mvlu_find_rd.." function.	*/
      return (SD_FAILURE);
      }
    }

  if (setFlags & MVLU_SET_WR_FUN)
    {
    wrIndex = mvlu_find_wr_ind_fun_index (wrIndFunName);
    if (wrIndex < 0)
      {
      /* Error already logged in "mvlu_find_wr.." function.	*/
      return (SD_FAILURE);
      }
    }

  ret = u_mvlu_resolve_leaf_ref (leafName, &setFlags, refString, &ref);
  if (ret != SD_SUCCESS)
    {
    MVL_LOG_NERR1 ("Could not resolve reference '%s'", refString);
    return (SD_FAILURE);
    }

  MVLU_LOG_FLOW1 ("Setting Leaf Parameters for leaf '%s'", leafName);
  if (setFlags & MVLU_SET_RD_FUN)
    {
    MVLU_LOG_CFLOW2 ("  Read Indication: %s\t(%d)", rdIndFunName, (int) rdIndex);
    }
  if (setFlags & MVLU_SET_WR_FUN)
    {
    MVLU_LOG_CFLOW2 ("  Write Indication: %s\t(%d)", wrIndFunName, (int) wrIndex);
    }
  if (setFlags & MVLU_SET_REF)
    {
    MVLU_LOG_CFLOW1 ("  Reference: " S_FMT_PTR, ref);
    }
  
  ret = mvlu_set_leaf_param2 (base_var_type_id, setFlags, leafName, rdIndex, wrIndex, (ST_RTREF) ref);
  if (ret != SD_SUCCESS)
    {
    MVL_LOG_NERR1 ("Could not set leaf %s parameters", leafName);
    }
  return (ret);
  }

/************************************************************************/
#endif	/* defined(MVL_UCA)	*/
/************************************************************************/
