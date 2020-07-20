/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,       */
/*               1997-2009, All Rights Reserved                         */
/*                                                                      */
/* MODULE NAME : mvlu_rt.c                                              */
/* PRODUCT(S)  :                                                        */
/*                                                                      */
/* MODULE DESCRIPTION :                                                 */
/*	RUNTIME_TYPE post-processing function for UCA and IEC 61850	*/
/*	data models.							*/
/*                                                                      */
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :                            */
/*			mvlu_proc_rt_type				*/
/*                                                                      */
/* MODIFICATION LOG :                                                   */
/*  Date     Who   Rev                  Comments                        */
/* --------  ---  ------   -------------------------------------------  */
/* 01/11/12  JRB	   Check for NULL return from strchr.		*/
/*			   Check for arr_loop_level out of range.	*/
/* 02/26/10  JRB	   _mvlu_set_rt_offset: init arrays.		*/
/* 05/29/09  JRB     12    Skip type name when computing flatname len.	*/
/* 01/18/08  JRB     11    We allow up to 15 char LNName (sclparse.c),	*/
/*			   so max flatname must be MAX_IDENT_LEN-15.	*/
/* 07/20/07  JRB     10    Flatnames must be shorter to add LNName later*/
/* 11/22/06  JRB     09    Free bufs if _mvlu_build_uca_name_tbl fails.	*/
/* 01/30/06  GLB     09    Integrated porting changes for VMS           */
/* 03/11/04  GLB     08    Added "#ifdef DEBUG_SISCO" for "thisFileName"*/
/* 09/18/03  JRB     07    Allow array of structures.			*/
/*			   DON'T allow array of arrays (return error).	*/
/* 05/02/03  JRB     06    switch(rt->el_tag): Use default for most cases*/
/* 04/29/03  JRB     05    Chg several functions to return ST_RET.	*/
/* 04/24/03  JRB     04    Use MAX_IDENT_LEN define.			*/
/* 03/13/03  JRB     03    mvlu_proc_rt_type: Chg to use RUNTIME_CTRL.	*/
/*			   MVLU_UCA_NAME_CTRL: Chg ucaName member from	*/
/*			   ptr to array (saves allocs).			*/
/* 12/11/02  JRB     02    Use new mvl_uca.h				*/
/* 11/14/02  MDE     01    New module, extracted fo_uca.c		*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "gen_list.h"

#include "mmsdefs.h"
#include "mms_pvar.h"
#include "mms_vvar.h"
#include "mem_chk.h"
#include "mvl_uca.h"

/************************************************************************/
/************************************************************************/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/
/************************************************************************/
/* STATIC VARIABLES							*/

#define MAX_PREFIX_LEN  200
#define MAX_NEST_LEVEL  50
static ST_CHAR *namePrefix[MAX_NEST_LEVEL];
static ST_INT nestLevel;


/************************************************************************/
/* STATIC FUNCTIONS							*/

static ST_RET _mvlu_set_rt_sort_num (ST_CHAR *typeName, 
				      RUNTIME_TYPE *rt, ST_INT numRt, 
				      MVLU_UCA_NAME_CTRL **ucaNamesOut);
static ST_VOID _mvlu_set_rt_size (RUNTIME_TYPE *rt) ;
static ST_VOID _mvlu_set_rt_offset (RUNTIME_TYPE *rt, ST_INT numRt);
static ST_RET _mvlu_build_uca_name_tbl (RUNTIME_TYPE *rt, ST_INT rt_num, 
 		       MVLU_UCA_NAME_CTRL *mvluFoundryInfo);
static ST_RET _mvlu_save_rt_uca_name (ST_INT nestLevel, RUNTIME_TYPE *rt,
 			 MVLU_UCA_NAME_CTRL *mvluFoundryInfo);
static ST_VOID _mvlu_sort_uca_names (MVLU_UCA_NAME_CTRL *ucaNames, 
				RUNTIME_TYPE *rt, ST_INT numRt);

/************************************************************************/
/************************************************************************/
/*			mvlu_proc_rt_type				*/
/************************************************************************/

ST_RET mvlu_proc_rt_type (ST_CHAR *typeName, RUNTIME_CTRL *rt_ctrl, 
			  MVLU_UCA_NAME_CTRL **ucaNamesOut)
  {
RUNTIME_TYPE *rt;
ST_INT i;
ST_RET ret;

/* Set the sortedNum for each RT element */
  ret = _mvlu_set_rt_sort_num (typeName, rt_ctrl->rt_first, rt_ctrl->rt_num, ucaNamesOut);
  if (ret)
    return(ret);	/* don't bother continuing	*/

/* Find the offset of the data for each RT within the type		*/
  _mvlu_set_rt_offset (rt_ctrl->rt_first, rt_ctrl->rt_num);

/* Find the size of each RT within the type				*/
  rt = rt_ctrl->rt_first;
  for (i = 0; i < rt_ctrl->rt_num; ++i, ++rt)
    _mvlu_set_rt_size (rt);

  return (ret);
  }

/************************************************************************/
/************************************************************************/
/*				_mvlu_set_rt_size			*/
/************************************************************************/
/* This function would not be needed except that the UCA structure 	*/
/* handling needs to know the size of each subelement too ...		*/

static ST_VOID _mvlu_set_rt_size (RUNTIME_TYPE *rt) 
  {
ST_INT numUcaRt;
ST_INT blockedLen;

  if (rt->el_tag == RT_ARR_END ||  rt->el_tag == RT_STR_END)
    {
    return;
    }

  if (rt->el_tag == RT_STR_START)
    numUcaRt = rt->u.str.num_rt_blks+2;
  else if (rt->el_tag == RT_ARR_START)
    numUcaRt = rt->u.arr.num_rt_blks+2;
  else
    numUcaRt = 1;

/* Now we can find the size of this element and it's associates */
  blockedLen = ms_get_blocked_length (rt, numUcaRt);
  rt->offset_to_last = blockedLen;
  }

/************************************************************************/
/************************************************************************/
/*                       _mvlu_set_rt_offset                            */
/************************************************************************/

static ST_VOID _mvlu_set_rt_offset (RUNTIME_TYPE *rt, ST_INT numRt)
  {
ST_INT data_offset;
SD_CONST RUNTIME_TYPE *rt_end;
ST_INT arr_loops[ASN1_MAX_LEVEL];
ST_BOOLEAN arr_el_set[ASN1_MAX_LEVEL];
ST_INT arr_loop_level;

  /* Initialize arrays.	*/
  memset (arr_loops, 0, sizeof (arr_loops));
  memset (arr_el_set, 0, sizeof (arr_el_set));

  arr_loop_level = 0;
  rt_end = rt + numRt;  	        /* end block			*/
  data_offset = 0;

/* Traverse the RT type as though we were doing a ASN.1 to LOCAL	*/
  while (rt < rt_end)
    {
    if (rt->el_tag == RT_ARR_END)	/* treat case of array ending	*/
      {
      if (--arr_loops[arr_loop_level] > 0)/* if need to do next ar elmnt*/
        {
        rt -= rt->u.arr.num_rt_blks;    /* mv rt to start of arr	*/
        continue; 
        }
      else
	{
        --arr_loop_level;
        if (arr_loop_level < 0)
          {	/* should NEVER happen	*/
          SLOGALWAYS1 ("Critical internal error: array loop level = %d illegal", arr_loop_level);
          return;
          }
	}
      }


    if (arr_loop_level == 0 || arr_el_set[arr_loop_level] == SD_FALSE)
      {
      rt->mvluTypeInfo.offSet = data_offset; /* Set the offset		*/
      arr_el_set[arr_loop_level] = SD_TRUE;
      }

    data_offset += rt->el_size;		/* Offset for next		*/

    if (rt->el_tag == RT_ARR_START)	/* Set up array looping		*/
      {
      ++arr_loop_level;
      if (arr_loop_level >= ASN1_MAX_LEVEL)
        {	/* should NEVER happen	*/
        SLOGALWAYS1 ("Critical internal error: array loop level = %d illegal", arr_loop_level);
        return;
        }
      arr_loops[arr_loop_level] = rt->u.arr.num_elmnts;
      arr_el_set[arr_loop_level] = SD_FALSE;
      }
    rt++;			/* point to next runtime element	*/
    }
  }


/************************************************************************/
/************************************************************************/
/*                   _mvlu_set_rt_sort_num                              */
/************************************************************************/

static ST_RET _mvlu_set_rt_sort_num (ST_CHAR *typeName, 
				      RUNTIME_TYPE *rt, ST_INT numRt, 
				      MVLU_UCA_NAME_CTRL **ucaNamesOut)
  {
MVLU_UCA_NAME_CTRL *ucaNames;
ST_INT i;

  ucaNames = (MVLU_UCA_NAME_CTRL *) chk_calloc (numRt, sizeof (MVLU_UCA_NAME_CTRL));
  nestLevel = 0;

/* Derive the UCA names within this type */
  namePrefix[0] = (ST_CHAR *) chk_calloc (1, MAX_PREFIX_LEN);
  if (typeName != NULL)
    strcpy (namePrefix[0], typeName);

  if (_mvlu_build_uca_name_tbl (rt, numRt, ucaNames))
    {
    chk_free(ucaNames);
    chk_free(namePrefix[0]);
    return (SD_FAILURE);	/* don't bother continuing	*/
    }

/* Now sort the UCA names within the type	     			*/
  _mvlu_sort_uca_names (ucaNames, rt, numRt);    

  for (i = 0; i < MAX_NEST_LEVEL; ++i)
    {
    if (namePrefix[i] != NULL)
      {
      chk_free (namePrefix[i]);
      namePrefix[i] = NULL;
      }
    }
  if (ucaNamesOut == NULL)
    chk_free (ucaNames);
  else
    *ucaNamesOut = ucaNames;
  return (SD_SUCCESS);
  }

/************************************************************************/
/*                       _mvlu_build_uca_name_tbl                       */
/* Start with retCode=SD_SUCCESS. If any call to _mvlu_save_rt_uca_name	*/
/* fails, set retCode=SD_FAILURE. Do not return immediately because we	*/
/* want to log as many problems as possible first.			*/
/************************************************************************/

static ST_RET _mvlu_build_uca_name_tbl (RUNTIME_TYPE *rt, ST_INT rt_num, 
			      MVLU_UCA_NAME_CTRL *ucaNames)
  {
ST_INT i;
ST_RET retCode=SD_SUCCESS;

  nestLevel = 0;
  for (i = 0; i < rt_num; ++i, ++rt)
    {
    ucaNames[i].rtIndex = i;
    ucaNames[i].rt = rt;
    switch (rt->el_tag)
      {
      case RT_STR_START :
        if (ms_comp_name_pres(rt))
          {
          if (_mvlu_save_rt_uca_name (nestLevel, rt, &ucaNames[i]))
            retCode = SD_FAILURE;	/*continue but eventually return this*/
      
          ++nestLevel;
          if (namePrefix[nestLevel] == NULL)
            namePrefix[nestLevel] = (ST_CHAR *) chk_calloc (1, MAX_PREFIX_LEN);
      
          strcpy (namePrefix[nestLevel], namePrefix[nestLevel-1]);
          strcat (namePrefix[nestLevel], "$");
          strcat (namePrefix[nestLevel], ms_comp_name_find(rt));
          }
      break;

      case RT_STR_END :
        --nestLevel;
      break;

      case RT_ARR_START :
        if (_mvlu_save_rt_uca_name (nestLevel, rt, &ucaNames[i]))
          retCode = SD_FAILURE;	/*continue but eventually return this*/

      /* We let the array element inherit our name so we will generate 	*/
      /* rd/wr ind handler functions and references for the contained 	*/
      /* primitive elements						*/
      /* This causes duplicate names (name of array same as name of	*/
      /* object IN array). That's OK because names of anything inside	*/
      /* array are dropped when sorted (see _mvlu_sort_uca_names).	*/
      /* The "sorted" array of names is used in GetNameList response.	*/

        if (ms_is_rt_prim (rt+1) || (rt+1)->el_tag==RT_STR_START)
	  {

#if defined(USE_RT_TYPE_2)
          (rt+1)->comp_name_ptr = rt->comp_name_ptr;
#elif defined(USE_RT_TYPE_3)
          (rt+1)->name_index = rt->name_index;
#else
          strcpy ((rt+1)->name, rt->name);
#endif
	  }
	else
	  {
	  SLOGALWAYS1 ("Warning: Arrays of arrays not supported for UCA (%s)", 
	  				ucaNames[i].ucaName);
          retCode = SD_FAILURE;	/*continue but eventually return this*/
	  }
      break;

      case RT_ARR_END :
      break;

      default :
        if (_mvlu_save_rt_uca_name (nestLevel, rt, &ucaNames[i]))
          retCode = SD_FAILURE;	/*continue but eventually return this*/
      break;
      }
    }
  return (retCode);
  }

/************************************************************************/
/*                      _mvlu_save_rt_uca_name                          */
/* Start with retCode=SD_SUCCESS. If anything fails, set retCode=SD_FAILURE.*/
/************************************************************************/
static ST_RET _mvlu_save_rt_uca_name (ST_INT nestLevel, 
				RUNTIME_TYPE *rt, 
				MVLU_UCA_NAME_CTRL *nameDest)
  {
ST_CHAR nameBuf[256];
ST_CHAR *firstdollar;			/* ptr to first "$" in nameBuf	*/
ST_RET retCode = SD_SUCCESS;

  nameDest->ucaName [0] = '\0';		/* start with empty string	*/
  nameDest->rt = rt;
  if (ms_comp_name_pres(rt))
    {
    sprintf (nameBuf, "%s$%s", namePrefix[nestLevel], ms_comp_name_find(rt));
    /* Make sure there is room left for longest allowed Logical Node Name*/
    /* (11 characters according to IEC 61850-7-2 but we allow 15).	*/
    /* NOTE: this allows 5 or 6 more char than 61850-7-2 specifies, so */
    /*       it is more forgiving, but it prevents illegal MMS names.	*/


    /* Skip over type name, if present (gets replaced by LNName).	*/
    /* strchr can't fail here because sprintf above adds '$'.		*/
    firstdollar = strchr (nameBuf, '$');	/* find first '$'	*/
    if (firstdollar == NULL)
      retCode = SD_FAILURE; /* should NEVER happen  */
    else if (strlen (firstdollar) > (MAX_IDENT_LEN-15))
      {
      SLOGALWAYS2 ("Error: Flattened IEC 61850 variable name '%s' > %d characters. Illegal.",
                   firstdollar, (MAX_IDENT_LEN-15));
      SLOGCALWAYS0 ("Must allow space for LNName up to 15 characters");
      retCode = SD_FAILURE;
      }
    else if (strlen (nameBuf) >= sizeof (nameDest->ucaName))
      {
      /* Suffix length is OK, but type name must be too long.		*/
      /* User may increase size of "ucaName" in MVLU_UCA_NAME_CTRL to	*/
      /* allow longer type name (or just use shorter type name).	*/
      SLOGALWAYS1 ("Error: Flattened IEC 61850 type name '%s' too long to save.",
                   nameBuf);
      retCode = SD_FAILURE;
      }
    else
      strcpy (nameDest->ucaName, nameBuf);
    }
  return (retCode);
  }

/************************************************************************/
/************************************************************************/
/* SORTING UCA NAMES WITHIN A TYPE					*/
/************************************************************************/
/*                       elementCompare                                 */
/************************************************************************/
/* This function is sorting the namelist table                          */

static ST_INT elementCompare (const MVLU_UCA_NAME_CTRL *a, 
			      const MVLU_UCA_NAME_CTRL *b)
  {
  return (strcmp (a->ucaName, b->ucaName));
  }

/************************************************************************/
/*                       _mvlu_sort_uca_names				*/
/************************************************************************/

static ST_VOID _mvlu_sort_uca_names (MVLU_UCA_NAME_CTRL *ucaNames, 
				RUNTIME_TYPE *rt, ST_INT numRt)
  {
ST_INT i;
ST_INT numNames;
MVLU_UCA_NAME_CTRL *sortedNames;	/* compressed/sorted array of names*/

  sortedNames = (MVLU_UCA_NAME_CTRL *) M_MALLOC (MSMEM_GEN, numRt*sizeof(MVLU_UCA_NAME_CTRL));

/* Compress the name table */  
  numNames = 0;
  for (i = 0; i < numRt; ++i, ++rt)
    {
    if (ucaNames[i].ucaName[0] != '\0')
      {
      sortedNames[numNames] = ucaNames[i];
      ++numNames;
/* CRITICAL:
 * The "ucaNames" array contains names for objects inside arrays.
 * They must be kept in the original array, because they are needed
 * for generating leaf function names.
 * However, they are NOT legal variable names, so they are NOT
 * copied to the SORTED array (used for GetNameList response).
 * This is done by skipping over the runtime type for the array.
 */
      if (rt->el_tag==RT_ARR_START)
        {
        rt += (rt->u.arr.num_rt_blks + 1);
        i += (rt->u.arr.num_rt_blks + 1);
        }
      }
    }

/* OK, we now have a raw table of names, go ahead and sort them */
  qsort (sortedNames, numNames, sizeof (MVLU_UCA_NAME_CTRL), 
	 (int (*)(const void *,const void *)) elementCompare);

#if 0	/* DEBUG: enable this code to debug sorting algorithm	*/
  if (numNames)
    {
    ST_INT j;
    SLOGALWAYS0 ("Array of UCA variable names (Uncompressed/Unsorted)");
    for (j = 0; j<numRt; j++)
      SLOGCALWAYS1 ("%s", ucaNames[j].ucaName);

    SLOGALWAYS0 ("Array of UCA variable names (Compressed/Sorted)");
    for (j = 0; j<numNames; j++)
      SLOGCALWAYS1 ("%s", sortedNames[j].ucaName);
    }
#endif

/* Now assign sort numbers to the RT elements */
  for (i = 0; i < numNames; ++i)
    {
    sortedNames[i].rt->mvluTypeInfo.sortedNum = i+1;
    }
  M_FREE (MSMEM_GEN, sortedNames);
  }

