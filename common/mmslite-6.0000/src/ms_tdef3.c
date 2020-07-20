/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2012, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : ms_tdef3.c						*/
/* PRODUCT(S)  : MMSEASE						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*   Functions to assist in using NESTED RUNTIME TYPES			*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 03/01/12  EJV           Revised code to eliminate Klocwork warnings. */
/* 01/19/12  EJV           _WIN64 added typecasts to elim warnings.	*/
/* 03/11/08  JRB     21    Del get_utcTime, get_binary_time (handle	*/
/*			   these like other types in get_general). This	*/
/*			   fixes Foundry typedefs generated for arrays.	*/
/* 01/22/04  JRB     20	   Replace (rt_ptr-1) with rt_prev, & (rt_ptr+1)*/
/*			   with rt_next (chk for out of bounds).	*/
/* 11/04/03  JRB     19    Add RT_UTF8_STRING to "if" in get_arr_end	*/
/*			   to create correct tdef for array of UTF8string.*/
/* 04/14/03  JRB     18    Eliminate compiler warnings.			*/
/* 04/02/03  JRB     17    Add UTF8string support (see RT_UTF8_STRING).	*/
/* 07/03/02  EJV     16    MMS_UTC_TIME: chg name usec to fraction.	*/
/* 05/02/02  DWL     15    Btime4 fix					*/
/* 11/14/01  EJV     14    Added support for new MMS type UtcTime:	*/
/*			   added get_utcTime func;			*/
/*			   ms_rt_to_typedef: added case for RT_UTC_TIME;*/
/* 08/23/00  DSF     13    Fixed get_bit_string for fixed length strings*/
/* 11/23/99  DWL     12    Fixed btime4 & btime6 array bug		*/
/* 09/13/99  MDE     11    Added SD_CONST modifiers			*/
/* 07/14/99  DWL     10	   Carry over name if array of vstring is named	*/
/* 02/02/99  DWL     09	   Corrected array of vstring/bstring/ostring	*/
/* 07/29/98  MDE     08	   Corrected btime6 problem			*/
/* 07/01/98  MDE     07	   Now use 'el_len' instead of 'el_size'	*/
/* 06/04/98  MDE     06	   Fixed overwrite problem			*/
/* 04/30/98  DWL     05	   Fixed Bitstring problem			*/
/* 03/16/98  DWL     04	   Fixed 'get_arr_end' problem			*/
/* 03/16/98  EJV     03	   Substituted itoa with ANSI sprintf.		*/
/* 02/03/98  DWL     02	   Added 'ms_rt_to_typedef'			*/
/* 08/14/97  MDE     01    New						*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "glbsem.h"
#include "mmsdefs.h"
#include "mms_pvar.h"
#include "mms_vvar.h"
#include "asn1defs.h"
#include "str_util.h"

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/


#define BUF_LEN			50
#define NUM_NEST_LEVELS_ALLOWED 50
#define COPY 			SD_TRUE
#define CAT 			SD_FALSE

/* This structure keeps track of nested structures			*/
typedef struct nest
  {
  ST_CHAR struct_name[BUF_LEN];
  ST_CHAR name_type[BUF_LEN];
  } NEST;
static NEST struct_nest[NUM_NEST_LEVELS_ALLOWED];

/* This structure keeps track of nested arrays				*/
typedef struct arr
  {
  ST_CHAR arr_name[BUF_LEN];
  ST_INT arr_num;
  ST_CHAR arr_type[BUF_LEN];
  }  ARR;
static ARR array_nest[NUM_NEST_LEVELS_ALLOWED];


/************************************************************************/

static ST_RET str_concat (ST_CHAR *dest, ST_CHAR *str, 
		   ST_INT destSize, ST_BOOLEAN copycat);

static ST_RET get_str_start (RUNTIME_TYPE *rt_head, RUNTIME_TYPE *rt_ptr, NEST *s, 
			     ST_INT struct_nest_level, ST_INT *strnum,
			     ST_CHAR *spaces, ST_CHAR *dest, 
			     ST_INT destSize);

static ST_RET get_str_end (RUNTIME_TYPE *rt_head, RUNTIME_TYPE *rt_ptr, ST_INT numRt, NEST *s,
			   ST_INT struct_nest_level, ST_CHAR *spaces,
			   ST_INT i, ST_CHAR *dest, ST_INT destSize);

static ST_VOID get_arr_start (RUNTIME_TYPE *rt_head, RUNTIME_TYPE *rt_ptr, ARR *a, 
			      ST_INT array_nest_level, ST_INT *strnum,
			      ST_INT i);

static ST_RET get_arr_end (RUNTIME_TYPE *rt, RUNTIME_TYPE *rt_ptr, ARR *a, 
			   ST_INT array_nest_level, ST_CHAR *spaces,
			   ST_INT struct_nest_level, 
			   ST_INT i, ST_INT numRt, ST_INT *num_array_nest,
			   ST_BOOLEAN *typedef_name_displayed, 
			   ST_CHAR *typedefName, ST_CHAR *dest,
			   ST_INT destSize);

static ST_RET get_bcd (RUNTIME_TYPE *rt_ptr, ST_INT struct_nest_level,
		       ST_CHAR *var_name, ST_CHAR *spaces, ST_INT *strnum,
		       ST_CHAR *dest, ST_INT destSize);

static ST_RET get_visible_string (RUNTIME_TYPE *rt_head, RUNTIME_TYPE *rt_ptr, ST_INT numRt,
				   ST_INT *strnum, ST_CHAR *str_buffer, 
				   ST_CHAR *typedefName, ST_CHAR *spaces,
				   ST_BOOLEAN *typedef_name_displayed,
				   ST_CHAR *dest, ST_INT destSize);

static ST_RET get_bit_string (RUNTIME_TYPE *rt_head, RUNTIME_TYPE *rt_ptr, ST_INT numRt,
			       ST_CHAR *spaces, ST_INT *strnum, 
			       ST_CHAR *str_buffer, 
			       ST_BOOLEAN *typedef_name_displayed,
			       ST_CHAR *typedefName,
			       ST_CHAR *dest, ST_INT destSize);

static ST_RET get_general (RUNTIME_TYPE *rt_ptr, ST_INT numRt,
			   ST_INT struct_nest_level, ST_CHAR *spaces,
			   ST_CHAR *var_name, ST_INT *strnum, NEST *s,
			   ST_CHAR *dest, ST_INT destSize);

static ST_RET get_integer (RUNTIME_TYPE *rt_ptr, ST_CHAR *var_name,
			    NEST *s, ST_INT *strnum);

static ST_RET get_unsigned (RUNTIME_TYPE *rt_ptr, ST_CHAR *var_name,
			    NEST *s, ST_INT *strnum);

static ST_RET get_floating_point (RUNTIME_TYPE *rt_ptr, ST_CHAR *var_name,
			          NEST *s, ST_INT *strnum);

/************************************************************************/
/************************************************************************/

/************************************************************************/
/*			ms_rt_to_typedef				*/
/* This function converts Runtime type to a C structure.		*/
/************************************************************************/

ST_RET ms_rt_to_typedef (ST_CHAR *typedefName, SD_CONST RUNTIME_TYPE *rt_head,
				ST_INT numRt, ST_CHAR *dest, ST_INT destSize)
  {
RUNTIME_TYPE *rt_ptr;

ST_CHAR var_name[BUF_LEN];
ST_INT strnum;		      /* Keep track of numbers concatenated to	*/
ST_CHAR str_buffer[16];	      /* variable names				*/
ST_INT i;
ST_INT num_array_nest;
ST_CHAR spaces[80];	  /* Keeps track of spacing in structure	*/
NEST *s;
ARR *a;
ST_INT struct_nest_level; /* Keeps track of structure nesting level	*/
ST_INT array_nest_level;  /* Keeps track of array nesting level		*/
ST_BOOLEAN typedef_name_displayed;
ST_RET ret;

/* Initialize variables						    */
  strnum = 1;	      /* Keep track of numbers concatenated to	*/
  var_name[0] = 0;
  ret = SD_SUCCESS;
  rt_ptr = rt_head;
  strcpy (spaces, "");
  struct_nest_level = 0;
  array_nest_level = 0;
  typedef_name_displayed = SD_FALSE;
  s = &struct_nest[struct_nest_level];
  a = &array_nest[array_nest_level];

/* Now we parse the Runtime type and fill a C typedef statement	    */
  for (i = 0; i < numRt; ++i, ++rt_ptr)
    {
    switch (rt_ptr->el_tag)
      {
      case RT_STR_START:
	++struct_nest_level;
	++s;
	ret = get_str_start (rt_head, rt_ptr, s, struct_nest_level, &strnum, spaces,
	               dest, destSize);
	if (ret == SD_FAILURE)
	  return (SD_FAILURE);
	break;
	
      case RT_STR_END:
        ret = get_str_end (rt_head, rt_ptr, numRt, s, struct_nest_level, spaces,
	             i, dest, destSize);
	--struct_nest_level;
	--s;
	if (ret == SD_FAILURE)
	  return (SD_FAILURE);
	break;

      case RT_ARR_START:
	++array_nest_level;
	++a;
	get_arr_start (rt_head, rt_ptr, a, array_nest_level, &strnum, i);
	break;
	
      case RT_ARR_END:
	ret = get_arr_end (rt_head, rt_ptr, a, array_nest_level, spaces,
	                   struct_nest_level, i, numRt,
			   &num_array_nest, &typedef_name_displayed,
			   typedefName, dest, destSize);
	--array_nest_level;
	--a;
	if (ret == SD_FAILURE)
	  return (SD_FAILURE);
	break;

      case RT_BCD:
	ret = get_bcd (rt_ptr, struct_nest_level, var_name, spaces, &strnum,
		 dest, destSize);
	if (ret == SD_FAILURE)
	  return (SD_FAILURE);
	break;

      case RT_VISIBLE_STRING:
      case RT_UTF8_STRING:	/* same as visible str except len	*/
        /* get_visible_string must chk el_tag to differentiate.	*/
	ret = get_visible_string (rt_head, rt_ptr, numRt, &strnum, str_buffer, 
			    typedefName, spaces, &typedef_name_displayed,
			    dest, destSize);
	if (ret == SD_FAILURE)
	  return (SD_FAILURE);
	break;

      case RT_BIT_STRING:
      case RT_OCTET_STRING:
	ret = get_bit_string (rt_head, rt_ptr, numRt, spaces, &strnum, str_buffer, 
			&typedef_name_displayed, typedefName,
			dest, destSize);
	if (ret == SD_FAILURE)
	  return (SD_FAILURE);
	break;

      case RT_BOOL:
      case RT_INTEGER:
      case RT_UNSIGNED:
      case RT_FLOATING_POINT:
      case RT_GENERAL_TIME:
      case RT_BINARY_TIME:
      case RT_UTC_TIME:
	ret = get_general (rt_ptr, numRt, struct_nest_level, spaces,
	                   var_name, &strnum, s, dest, destSize);
	if (ret == SD_FAILURE)
	  return (SD_FAILURE);
	break;

      default:
	MLOG_ERR1 ("MS_RT_TO_TYPEDEF: Invalid element tag (%d)",
    				      rt_ptr->el_tag);
	return (SD_FAILURE);
	break;
      }
    }
  /* If everything went correctly, these values should both be 0	*/
  if (struct_nest_level == 0 && array_nest_level == 0)
    {
    if (!typedef_name_displayed)
      {
      ret = str_concat (dest, " ", destSize, CAT) || ret;
      ret = str_concat (dest, typedefName, destSize, CAT) || ret;
      ret = str_concat (dest, ";", destSize, CAT) || ret;
      }

    if (numRt == 1)
      {		/* simple type. "rt_head" points to the 1 RUNTIME_TYPE struct*/
      if (rt_head->el_tag == RT_BINARY_TIME && rt_head->u.p.el_len == 4)
        ret = str_concat (dest, "  /* Btime4 is one ST_INT32 */", destSize, CAT) || ret;
      else if (rt_head->el_tag == RT_VISIBLE_STRING)
        ret = str_concat (dest, "  /* Visible String */", destSize, CAT) || ret;
      else if (rt_head->el_tag == RT_BCD)
        ret = str_concat (dest, "  /* BCD */", destSize, CAT) || ret;
      else if (rt_head->el_tag == RT_BIT_STRING)
        ret = str_concat (dest, "  /* Bitstring */", destSize, CAT) || ret;
      else if (rt_head->el_tag == RT_OCTET_STRING)
        ret = str_concat (dest, "  /* Octetstring */", destSize, CAT) || ret;
      }	/* end (numRt == 1)	*/
    }
  else
    {
    if (struct_nest_level != 0)
      {
      MLOG_ERR0 ("MS_RT_TO_TYPEDEF: Uneven nesting in Runtime structure");
      }
    else
      {
      MLOG_ERR0 ("MS_RT_TO_TYPEDEF: Uneven nesting in Runtime array");
      }
    return (SD_FAILURE);
    }
  if (ret == SD_FAILURE)
    return (SD_FAILURE);

  return (SD_SUCCESS);
  }


/************************************************************************/
/*			str_concat					*/
/* This function checks buffer to see if there is enough room left to	*/
/* copy or concat to the string, then if yes, does so.			*/
/************************************************************************/

static ST_RET str_concat (ST_CHAR *dest, ST_CHAR *str, 
		   ST_INT destSize, ST_BOOLEAN copycat)
  {
ST_INT dest_len;
ST_INT str_len;

  dest_len = (ST_INT) strlen (dest);
  str_len = (ST_INT) strlen (str);
  if (dest_len + str_len > (destSize - 1))  /* Allow for termination */
    {
    MLOG_ERR1 ("MS_RT_TO_TYPEDEF: Maximum buffer size (%d) exceeded.", destSize);
    return (SD_FAILURE);
    }
  else
    {
    if (copycat == COPY)
      strcpy(dest, str);
    else
      strcat (dest, str);
    }
  return (SD_SUCCESS);
  }

/************************************************************************/
/*			get_str_start					*/
/************************************************************************/
static ST_RET get_str_start (RUNTIME_TYPE *rt_head, RUNTIME_TYPE *rt_ptr, NEST *s, 
			     ST_INT struct_nest_level, ST_INT *strnum,
			     ST_CHAR *spaces, ST_CHAR *dest, 
			     ST_INT destSize)
  {
ST_RET ret;
RUNTIME_TYPE *rt_prev;		/* ptr to previous in RUNTIME_TYPE array*/

  rt_prev = rt_ptr - 1;
  if (rt_prev < rt_head)
    rt_prev = NULL;

  ret = SD_SUCCESS;
  s->struct_name[0] = 0;
  strcat (spaces, "  ");
  if (struct_nest_level > 1)
    {
    if (rt_ptr->name[0] == 0)
      {
      sprintf (s->struct_name,"struct_%d",*strnum);
      if (rt_prev!=NULL && rt_prev->el_tag != RT_ARR_START)
	++(*strnum);
      }
    else
      {
      strncpy_safe(s->struct_name, rt_ptr->name, sizeof(s->struct_name)-1);
      }
    }
  if (struct_nest_level == 1)
    ret = str_concat (dest, "typedef struct", destSize, COPY) || ret;
  else
    ret = str_concat (dest, "struct", destSize, CAT) || ret;
  ret = str_concat (dest, "\n", destSize, CAT) || ret;
  ret = str_concat (dest, spaces, destSize, CAT) || ret;
  ret = str_concat (dest, "{\n", destSize, CAT) || ret;
  ret = str_concat (dest, spaces, destSize, CAT) || ret;
  return (ret);
  } 

/************************************************************************/
/*			get_str_end					*/
/************************************************************************/
static ST_RET get_str_end (RUNTIME_TYPE *rt_head, RUNTIME_TYPE *rt_ptr, ST_INT numRt, NEST *s,
			   ST_INT struct_nest_level, ST_CHAR *spaces,
			   ST_INT i, ST_CHAR *dest, ST_INT destSize)
  {
ST_RET ret;
RUNTIME_TYPE *rt_next;		/* ptr to next in RUNTIME_TYPE array*/

  rt_next = rt_ptr + 1;
  if (rt_next - rt_head >= numRt)
    rt_next = NULL;

  assert (struct_nest_level > 0);	/* must never hit 0 in this funct	*/
  ret = SD_SUCCESS;

  ret = str_concat (dest, "}", destSize, CAT) || ret;
  if (struct_nest_level > 1 && rt_next!=NULL && rt_next->el_tag != RT_ARR_END)
    {
    ret = str_concat (dest, " ", destSize, CAT) || ret;
    ret = str_concat (dest, s->struct_name, destSize, CAT) || ret;
    }
  spaces[strlen (spaces) - 1] = '\x00';
  spaces[strlen (spaces) - 1] = '\x00';

  if (rt_next!=NULL && rt_next->el_tag != RT_ARR_END)
    {
    ret = str_concat (dest, ";", destSize, CAT) || ret;
    }
  if (struct_nest_level != 1 && rt_next!=NULL && rt_next->el_tag != RT_ARR_END)
    {
    ret = str_concat (dest, "\n", destSize, CAT) || ret;
    ret = str_concat (dest, spaces, destSize, CAT) || ret;
    }
  return (ret);
  }

/************************************************************************/
/*			get_arr_start					*/
/************************************************************************/
static ST_VOID get_arr_start (RUNTIME_TYPE *rt_head, RUNTIME_TYPE *rt_ptr, ARR *a, 
			      ST_INT array_nest_level, ST_INT *strnum,
			      ST_INT i)
  {
RUNTIME_TYPE *rt_prev;		/* ptr to previous in RUNTIME_TYPE array*/

  rt_prev = rt_ptr - 1;
  if (rt_prev < rt_head)
    rt_prev = NULL;

  /* If array is multi-dimensional, carry over its name		*/
  if (array_nest_level > 1 && rt_prev && rt_prev->el_tag == RT_ARR_START)
    strcpy (a->arr_name, (a-1)->arr_name);
  else if (rt_ptr->name[0] == 0)
    {
    sprintf (a->arr_name, "arr_%d", *strnum);
    if (rt_ptr->u.arr.num_rt_blks != 1 && i != 0)
      ++(*strnum);
    }	       
  else
    strncpy_safe (a->arr_name, rt_ptr->name, sizeof(a->arr_name)-1);
  a->arr_num = rt_ptr->u.arr.num_elmnts;
  }

/************************************************************************/
/*			get_arr_end					*/
/************************************************************************/
static ST_RET get_arr_end (RUNTIME_TYPE *rt, RUNTIME_TYPE *rt_ptr, ARR *a, 
			   ST_INT array_nest_level, ST_CHAR *spaces,
			   ST_INT struct_nest_level, 
			   ST_INT i, ST_INT numRt, ST_INT *num_array_nest,
			   ST_BOOLEAN *typedef_name_displayed, 
			   ST_CHAR *typedefName, ST_CHAR *dest, 
			   ST_INT destSize)
  {
ST_INT j;
ST_CHAR temp_buffer[32];
ST_RET ret;

  ret = SD_SUCCESS;

  assert (rt_ptr-2 >= rt);	/* (rt_ptr-2) must be legal	*/
  if ((rt_ptr-2)->el_tag == RT_ARR_START && ((rt_ptr-1)->el_tag == RT_VISIBLE_STRING ||
                                             (rt_ptr-1)->el_tag == RT_UTF8_STRING ||
                                             (rt_ptr-1)->el_tag == RT_BIT_STRING ||
                                             (rt_ptr-1)->el_tag == RT_OCTET_STRING)) 
    return (ret);

  if (array_nest_level == 0)
    {
    MLOG_ERR0 ("MS_RT_TO_TYPEDEF: Uneven array nesting in Runtime type.");
    return (SD_FAILURE);
    }

  /* If we have a multi-dimensional array, we must display them	*/
  /* in the order they came in, not in the order they go out	*/
  if ((rt_ptr-1)->el_tag != RT_ARR_END)
    {
    j = 1;
    while ((rt_ptr+j)->el_tag == RT_ARR_END)
      {
      ++j;
      }
    /* DEBUG: check out this code. What's happening?	*/
    if (numRt > 4 &&			/* rt+4 must be legal	*/
        (i != numRt - 1 && (rt_ptr-1)->el_tag != RT_ARR_END) &&
	        (rt->el_tag != RT_ARR_START || 
		(rt+1)->el_tag != RT_ARR_START ||
		(rt+3)->el_tag != RT_ARR_END || 
		(rt+4)->el_tag != RT_ARR_END))
      {
      ret = str_concat (dest, " ", destSize, CAT) || ret;
      ret = str_concat (dest, a->arr_name, destSize, CAT) || ret;
      }
    /* Display array parameters in the correct order		*/
    if (!(*typedef_name_displayed) && struct_nest_level == 0)
      {
      ret = str_concat (dest, " ", destSize, CAT) || ret;
      ret = str_concat (dest, typedefName, destSize, CAT) || ret;
      *typedef_name_displayed = SD_TRUE;
      }
    for (*num_array_nest = 1; *num_array_nest <= j; ++(*num_array_nest))
      {
      ret = str_concat (dest, "[", destSize, CAT) || ret;
      if (j > 1)
	{
        sprintf (temp_buffer, "%d", array_nest[*num_array_nest].arr_num);
	ret = str_concat (dest, temp_buffer, destSize, CAT) || ret;
	}
      else
	{
        sprintf (temp_buffer, "%d", array_nest[array_nest_level].arr_num);
	ret = str_concat (dest, temp_buffer, destSize, CAT) || ret;
	}
      ret = str_concat (dest, "]", destSize, CAT) || ret;
      }
    if (*typedef_name_displayed && struct_nest_level == 0)
      {
      ret = str_concat (dest, ";", destSize, CAT) || ret;
      }
    }
  if ((i+1) != numRt && (rt_ptr+1)->el_tag != RT_ARR_END)
    {
    ret = str_concat (dest, ";\n", destSize, CAT) || ret;
    ret = str_concat (dest, spaces, destSize, CAT) || ret;
    }
  a->arr_name[0] = 0;
  return (ret);
  }

/************************************************************************/
/*			get_bcd						*/
/************************************************************************/
static ST_RET get_bcd (RUNTIME_TYPE *rt_ptr, ST_INT struct_nest_level,
		       ST_CHAR *var_name, ST_CHAR *spaces, ST_INT *strnum,
		       ST_CHAR *dest, ST_INT destSize)

  {
ST_RET ret;

  ret = SD_SUCCESS;

  if (struct_nest_level == 0)
    ret = str_concat (dest, "typedef ", destSize, COPY) || ret;
  if (rt_ptr->name[0] == 0)
    {
    sprintf (var_name, "bcd_%d", *strnum);
    ++(*strnum);
    }
  else
    strcpy (var_name, rt_ptr->name);
  switch (rt_ptr->u.p.el_len)
    {
    case 1:
    case 2:
      ret = str_concat (dest, "ST_INT8 ", destSize, CAT) || ret;
      break;
    case 3:
    case 4:
      ret = str_concat (dest, "ST_INT16 ", destSize, CAT) || ret;
      break;
    case 5:
    case 6:
    case 7:
    case 8:
      ret = str_concat (dest, "ST_INT32 ", destSize, CAT) || ret;
      break;
    default:
      {			  
      MLOG_ERR2 ("MS_RT_TO_TYPEDEF: Invalid element len (%d) for type (%d)", 
	          rt_ptr->u.p.el_len, rt_ptr->el_tag);
      return (SD_FAILURE);
      }
    }
  if (struct_nest_level != 0)
    {
    ret = str_concat (dest, var_name, destSize, CAT) || ret;
    ret = str_concat (dest, ";   /* BCD number */\n", destSize, CAT) || ret;
    ret = str_concat (dest, spaces, destSize, CAT) || ret;
    }
  return (ret);
  }

/************************************************************************/
/*			get_visible_string				*/
/************************************************************************/
static ST_RET get_visible_string (RUNTIME_TYPE *rt_head, RUNTIME_TYPE *rt_ptr, ST_INT numRt,
				  ST_INT *strnum, ST_CHAR *str_buffer, 
				  ST_CHAR *typedefName, ST_CHAR *spaces,
				  ST_BOOLEAN *typedef_name_displayed,
				  ST_CHAR *dest, ST_INT destSize)
  {
ST_CHAR temp_buffer[32];
ST_RET ret;
RUNTIME_TYPE *rt_prev;		/* ptr to previous in RUNTIME_TYPE array*/
RUNTIME_TYPE *rt_next;		/* ptr to next     in RUNTIME_TYPE array*/

  rt_prev = rt_ptr - 1;
  if (rt_prev < rt_head)
    rt_prev = NULL;		/* past beginning of array	*/

  rt_next = rt_ptr + 1;
  if (rt_next - rt_head >= numRt)
    rt_next = NULL;		/* past end of array	*/

  ret = SD_SUCCESS;

  if (numRt == 1 || (numRt == 3 && rt_prev && rt_prev->el_tag == RT_ARR_START))
    ret = str_concat (dest, "typedef ", destSize, COPY) || ret;
  if (rt_ptr->name[0] == 0)
    {
    ret = str_concat (dest, "ST_CHAR", destSize, CAT) || ret;
    if (numRt != 1 && !(numRt == 3 && rt_prev && rt_prev->el_tag == RT_ARR_START))
      {
      if (rt_prev && rt_prev->el_tag == RT_ARR_START && rt_prev->name[0] != 0)
	{  /* Array is named */
	ret = str_concat (dest, " ", destSize, CAT) || ret;
	ret = str_concat (dest, rt_prev->name, destSize, CAT) || ret;
	}
      else
	{
        ret = str_concat (dest, " vstr_", destSize, CAT) || ret;
        sprintf (str_buffer, "%d", *strnum);
        ret = str_concat (dest, str_buffer, destSize, CAT) || ret;
        ++(*strnum);
	}
      }
    }
  else
    {
    ret = str_concat (dest, "ST_CHAR", destSize, CAT) || ret;
    if (numRt != 1 && !(rt_next && rt_next->el_tag == RT_ARR_END && 
                        rt_prev && rt_prev->el_tag == RT_ARR_START))
      {
      ret = str_concat (dest, " ", destSize, CAT) || ret;
      ret = str_concat (dest, rt_ptr->name, destSize, CAT) || ret;
      }
    /* Special case - named Vstring inside a named array, we carry over array name */
    else if (rt_prev && rt_prev->el_tag == RT_ARR_START &&
             rt_next && rt_next->el_tag == RT_ARR_END &&
             rt_ptr->name[0] != 0 && rt_prev->name[0] != 0)
      {
      ret = str_concat (dest, " ", destSize, CAT) || ret;
      ret = str_concat (dest, rt_prev->name, destSize, CAT) || ret;
      }
    }
  if ((numRt == 1 || (numRt == 3 && 
                      rt_prev && rt_prev->el_tag == RT_ARR_START)) && 
                      !*typedef_name_displayed)
    {
    ret = str_concat (dest, " ", destSize, CAT) || ret;
    ret = str_concat (dest, typedefName, destSize, CAT) || ret;
    *typedef_name_displayed = SD_TRUE;
    }
  ret = str_concat (dest, "[", destSize, CAT) || ret;
  if (rt_prev!=NULL && rt_next!=NULL && rt_prev->el_tag==RT_ARR_START && rt_next->el_tag==RT_ARR_END)
    {
    sprintf (temp_buffer, "%d", rt_prev->u.arr.num_elmnts);
    ret = str_concat (dest, temp_buffer, destSize, CAT) || ret;
    ret = str_concat (dest, "]", destSize, CAT) || ret;

    ret = str_concat (dest, "[", destSize, CAT) || ret;
    if (rt_ptr->el_tag == RT_VISIBLE_STRING)
      sprintf (temp_buffer, "%d", abs (rt_ptr->u.p.el_len) + 1);
    else if (rt_ptr->el_tag == RT_UTF8_STRING)
      sprintf (temp_buffer, "%d", rt_ptr->el_size);
    else
      assert (0);	/* wrong tag. should not be here	*/
    ret = str_concat (dest, temp_buffer, destSize, CAT) || ret;
    }
  else
    {
    temp_buffer[0] = '\0';
    if (rt_ptr->el_tag == RT_VISIBLE_STRING)
      sprintf (temp_buffer, "%d", abs (rt_ptr->u.p.el_len) + 1);
    else if (rt_ptr->el_tag == RT_UTF8_STRING)
      sprintf (temp_buffer, "%d", rt_ptr->el_size);
    else
      assert (0);	/* wrong tag. should not be here	*/
    ret = str_concat (dest, temp_buffer, destSize, CAT) || ret;
    }
  ret = str_concat (dest, "];", destSize, CAT) || ret;
  if (numRt != 1 && !(numRt == 3 && rt_prev && rt_prev->el_tag == RT_ARR_START))
    {
    ret = str_concat (dest, "\n", destSize, CAT) || ret;
    ret = str_concat (dest, spaces, destSize, CAT) || ret;
    }
  return (ret);
  }

/************************************************************************/
/*			get_bit_string					*/
/************************************************************************/
static ST_RET get_bit_string (RUNTIME_TYPE *rt_head, RUNTIME_TYPE *rt_ptr, ST_INT numRt,
			      ST_CHAR *spaces, ST_INT *strnum, 
			      ST_CHAR *str_buffer, 
			      ST_BOOLEAN *typedef_name_displayed,
			      ST_CHAR *typedefName,
			      ST_CHAR *dest, ST_INT destSize)
  {
ST_CHAR temp_buffer[32];
ST_RET ret;
RUNTIME_TYPE *rt_prev;		/* ptr to previous in RUNTIME_TYPE array*/
RUNTIME_TYPE *rt_next;		/* ptr to next     in RUNTIME_TYPE array*/

  rt_prev = rt_ptr - 1;
  if (rt_prev < rt_head)
    rt_prev = NULL;		/* past beginning of array	*/

  rt_next = rt_ptr + 1;
  if (rt_next - rt_head >= numRt)
    rt_next = NULL;		/* past end of array	*/

  ret = SD_SUCCESS;

  if (rt_ptr->u.p.el_len < 0)	    /* This is Variable-length	*/
    {
    if (numRt == 1 || (numRt == 3 && rt_prev && rt_prev->el_tag == RT_ARR_START))
      ret = str_concat (dest, "typedef ", destSize, COPY) || ret;
    ret = str_concat (dest, "struct", destSize, CAT) || ret;
    if (rt_ptr->el_tag == RT_BIT_STRING)
      ret = str_concat (dest, "     /* BVstring */\n", destSize, CAT) || ret;
    else
      ret = str_concat (dest, "     /* OVstring */\n", destSize, CAT) || ret;
    strcat (spaces, "  ");
    ret = str_concat (dest, spaces, destSize, CAT) || ret;
    ret = str_concat (dest, "{\n", destSize, CAT) || ret;
    ret = str_concat (dest, spaces, destSize, CAT) || ret;
    ret = str_concat (dest, "ST_INT16 len_", destSize, CAT) || ret;
    sprintf (str_buffer, "%d", *strnum);
    ret = str_concat (dest, str_buffer, destSize, CAT) || ret;
    ret = str_concat (dest, ";\n", destSize, CAT) || ret;
    ret = str_concat (dest, spaces, destSize, CAT) || ret;
    ret = str_concat (dest, "ST_UCHAR data_", destSize, CAT) || ret;
    ret = str_concat (dest, str_buffer, destSize, CAT) || ret;
    ret = str_concat (dest, "[", destSize, CAT) || ret;
    if (rt_ptr->el_tag == RT_BIT_STRING)
      sprintf (temp_buffer, "%d", (abs (rt_ptr->u.p.el_len) + 7) / 8);
    else
      sprintf (temp_buffer, "%d", abs (rt_ptr->u.p.el_len));
    
    ret = str_concat (dest, temp_buffer, destSize, CAT) || ret;
    ret = str_concat (dest, "]", destSize, CAT) || ret;
    ret = str_concat (dest, ";\n", destSize, CAT) || ret;
    ret = str_concat (dest, spaces, destSize, CAT) || ret;
    ret = str_concat (dest, "} ", destSize, CAT) || ret;
    if (rt_ptr->name[0] == 0 && numRt != 1 && !(numRt == 3 && rt_prev && rt_prev->el_tag == RT_ARR_START))
      {
      if (rt_ptr->el_tag == RT_BIT_STRING)
	{
	ret = str_concat (dest, "bvstr_", destSize, CAT) || ret;
	sprintf (str_buffer, "%d", *strnum);
	ret = str_concat (dest, str_buffer, destSize, CAT) || ret;
	++(*strnum);
	}
      else
	{
	ret = str_concat (dest, "ovstr_", destSize, CAT) || ret;
	sprintf (str_buffer, "%d", *strnum);
	ret = str_concat (dest, str_buffer, destSize, CAT) || ret;
	++(*strnum);
	}
      }
    else if ((numRt != 1) && !(rt_next && rt_next->el_tag == RT_ARR_END &&
                               rt_prev && rt_prev->el_tag == RT_ARR_START))
      {
      ret = str_concat (dest, rt_ptr->name, destSize, CAT) || ret;
      ret = str_concat (dest, ";", destSize, CAT) || ret;
      }

    if (rt_prev!=NULL && rt_next!=NULL && rt_prev->el_tag==RT_ARR_START && rt_next->el_tag==RT_ARR_END)
      {
      if (numRt == 3)
        {
	ret = str_concat (dest, " ", destSize, CAT) || ret;
	ret = str_concat (dest, typedefName, destSize, CAT) || ret;
	*typedef_name_displayed = SD_TRUE;
	}

      /* DEBUG: if component name is empty, dummy name already added	*/
      /*        earlier. Confusing but works. Simplify later.		*/
      /* If component name is valid, add it now.	*/
      if (rt_ptr->name[0]!=0)
        ret = str_concat (dest, rt_ptr->name, destSize, CAT) || ret;
   
      ret = str_concat (dest, "[", destSize, CAT) || ret;
      sprintf (temp_buffer, "%d", rt_prev->u.arr.num_elmnts);
      ret = str_concat (dest, temp_buffer, destSize, CAT) || ret;
      ret = str_concat (dest, "];", destSize, CAT) || ret;
      }

    spaces[strlen (spaces) - 1] = '\x00';
    spaces[strlen (spaces) - 1] = '\x00';
    }
  else	      /* This is fixed-length	  */
    {
    if (numRt == 1 || (numRt == 3 && rt_prev && rt_prev->el_tag == RT_ARR_START))
      ret = str_concat (dest, "typedef ", destSize, COPY) || ret;
    if (rt_ptr->name[0] == 0)
      {
      ret = str_concat (dest, "ST_UCHAR", destSize, CAT) || ret;
      if (numRt != 1 && !(numRt == 3 && rt_prev && rt_prev->el_tag == RT_ARR_START))
	{
	if (rt_ptr->el_tag == RT_BIT_STRING)
	  ret = str_concat (dest, " bstr_", destSize, CAT) || ret;
	else
	  ret = str_concat (dest, " ostr_", destSize, CAT) || ret;
	sprintf (str_buffer, "%d", *strnum);
	ret = str_concat (dest, str_buffer, destSize, CAT) || ret;
	++(*strnum);
	}
      }
    else
      {
      ret = str_concat (dest, "ST_UCHAR", destSize, CAT) || ret;
      if (numRt != 1 && !(numRt == 3 && rt_prev && rt_prev->el_tag == RT_ARR_START))
	{
	ret = str_concat (dest, " ", destSize, CAT) || ret;
	ret = str_concat (dest, rt_ptr->name, destSize, CAT) || ret;
	}
      }
    if ((numRt == 1 || (numRt == 3 && 
                        rt_prev && rt_prev->el_tag == RT_ARR_START)) && 
                        !*typedef_name_displayed)
      {
      ret = str_concat (dest, " ", destSize, CAT) || ret;
      ret = str_concat (dest, typedefName, destSize, CAT) || ret;
      *typedef_name_displayed = SD_TRUE;
      }
    ret = str_concat (dest, "[", destSize, CAT) || ret;
    if (rt_prev!=NULL && rt_next!=NULL && rt_prev->el_tag==RT_ARR_START && rt_next->el_tag==RT_ARR_END)
      {
      sprintf (temp_buffer, "%d", rt_prev->u.arr.num_elmnts);
      ret = str_concat (dest, temp_buffer, destSize, CAT) || ret;
      ret = str_concat (dest, "]", destSize, CAT) || ret;

      ret = str_concat (dest, "[", destSize, CAT) || ret;
      }

    if (rt_ptr->el_tag == RT_BIT_STRING)
      if (rt_ptr->u.p.el_len / 8 == 0)
	strcpy (temp_buffer, "1");
      else
	sprintf (temp_buffer, "%d", (rt_ptr->u.p.el_len + 7) / 8);
    else
      sprintf (temp_buffer, "%d", rt_ptr->u.p.el_len);
    ret = str_concat (dest, temp_buffer, destSize, CAT) || ret;
    ret = str_concat (dest, "];", destSize, CAT) || ret;
    }
  if (numRt != 1 && !(numRt == 3 && rt_prev && rt_prev->el_tag == RT_ARR_START))
    {
    ret = str_concat (dest, "\n", destSize, CAT) || ret;
    ret = str_concat (dest, spaces, destSize, CAT) || ret;
    }
  return (ret);
  }

/************************************************************************/
/*			get_general					*/
/************************************************************************/
static ST_RET get_general (RUNTIME_TYPE *rt_ptr, ST_INT numRt,
			   ST_INT struct_nest_level, ST_CHAR *spaces,
			   ST_CHAR *var_name, ST_INT *strnum, NEST *s,
			   ST_CHAR *dest, ST_INT destSize)
  {
ST_RET ret;

  ret = SD_SUCCESS;

  if (struct_nest_level == 0)
    ret = str_concat (dest, "typedef ", destSize, COPY) || ret;
  
  switch (rt_ptr->el_tag)
    {
    case RT_BOOL:
      sprintf (var_name, "bool_%d", (*strnum)++);
      strcpy (s->name_type, "ST_BOOLEAN");
      break;
    case RT_INTEGER:
      ret = get_integer (rt_ptr, var_name, s, &(*strnum));
      if (ret == SD_FAILURE)
	return (SD_FAILURE);
      break;
    case RT_UNSIGNED:
      ret = get_unsigned (rt_ptr, var_name, s, &(*strnum));
      if (ret == SD_FAILURE)
	return (SD_FAILURE);
      break;
    case RT_FLOATING_POINT:
	ret = get_floating_point (rt_ptr, var_name, s, &(*strnum));
	if (ret == SD_FAILURE)
	  return (SD_FAILURE);
      break;
    case RT_GENERAL_TIME:
	sprintf (var_name, "gtime_%d", (*strnum)++);
	strcpy (s->name_type, "time_t");
	break;
    case RT_BINARY_TIME:
	sprintf (var_name, "btime_%d", (*strnum)++);
        if (rt_ptr->u.p.el_len==6)		/* This is Btime6	*/
          strcpy (s->name_type, "MMS_BTIME6");
        else if (rt_ptr->u.p.el_len == 4)	/* This is Btime4	*/
          strcpy (s->name_type, "ST_INT32");
        else
          {
          MLOG_ERR1 ("MS_RT_TO_TYPEDEF: Invalid Btime len (%d)", rt_ptr->u.p.el_len);
          return (SD_FAILURE);
          }
	break;
    case RT_UTC_TIME:
	sprintf (var_name, "utc_%d", (*strnum)++);
	strcpy (s->name_type, "MMS_UTC_TIME");
	break;
    default:
	{
	MLOG_ERR1 ("MS_RT_TO_TYPEDEF: Invalid element tag (%d)",
    				  rt_ptr->el_tag);
	return (SD_FAILURE);
	}
      break;
    }
  if (!rt_ptr->name[0] == 0)
    {
    strcpy (var_name, rt_ptr->name);
    --(*strnum);
    }
  ret = str_concat (dest, s->name_type, destSize, CAT) || ret;
  if (numRt != 1 &&
     ((rt_ptr-1)->el_tag != RT_ARR_START && 
      (rt_ptr+1)->el_tag != RT_ARR_END) && struct_nest_level > 0)
    {
    ret = str_concat (dest, " ", destSize, CAT) || ret;
    ret = str_concat (dest, var_name, destSize, CAT) || ret;
    if ((rt_ptr+1)->el_tag != RT_ARR_END)
      {
      ret = str_concat (dest, ";\n", destSize, CAT) || ret;
      ret = str_concat (dest, spaces, destSize, CAT) || ret;
      }
    }
  return (ret);
  } 

/************************************************************************/
/*			get_integer					*/
/************************************************************************/
static ST_RET get_integer (RUNTIME_TYPE *rt_ptr, ST_CHAR *var_name,
			    NEST *s, ST_INT *strnum)
  {
  switch (rt_ptr->u.p.el_len)
    {
    case 1:
      sprintf (var_name, "i8_%d", (*strnum)++);
      strcpy (s->name_type, "ST_INT8");
      break;
    case 2:
      sprintf (var_name, "i16_%d", (*strnum)++);
      strcpy (s->name_type, "ST_INT16");
      break;
    case 4:
      sprintf (var_name, "i32_%d", (*strnum)++);
      strcpy (s->name_type, "ST_INT32");
      break;
    case 8:
      sprintf (var_name, "i64_%d", (*strnum)++);
      strcpy (s->name_type, "ST_INT64");
      break;
    default:
      { 
      MLOG_ERR2 ("MS_RT_TO_TYPEDEF: Invalid element size (%d) for type (%d)", 
		  rt_ptr->u.p.el_len, rt_ptr->el_tag);
      return (SD_FAILURE);
      }
    }
  return (SD_SUCCESS);
  }

/************************************************************************/
/*			get_unsigned					*/
/************************************************************************/
static ST_RET get_unsigned (RUNTIME_TYPE *rt_ptr, ST_CHAR *var_name,
			    NEST *s, ST_INT *strnum)
  {
  switch (rt_ptr->u.p.el_len)
    {
    case 1:
      sprintf (var_name, "u8_%d", (*strnum)++);
      strcpy (s->name_type, "ST_UINT8");
      break;
    case 2:
      sprintf (var_name, "u16_%d", (*strnum)++);
      strcpy (s->name_type, "ST_UINT16");
      break;
    case 4:
      sprintf (var_name, "u32_%d", (*strnum)++);
      strcpy (s->name_type, "ST_UINT32");
      break;
    case 8:
      sprintf (var_name, "u64_%d", (*strnum)++);
      strcpy (s->name_type, "ST_UINT64");
      break;
    default:
      {
      MLOG_ERR2 ("MS_RT_TO_TYPEDEF: Invalid element size (%d) for type (%d)", 
		  rt_ptr->u.p.el_len, rt_ptr->el_tag);
      return (SD_FAILURE);
      }
    }
  return (SD_SUCCESS);
  }

/************************************************************************/
/*			get_floating_point				*/
/************************************************************************/
static ST_RET get_floating_point (RUNTIME_TYPE *rt_ptr, ST_CHAR *var_name,
			    NEST *s, ST_INT *strnum)
  {
  switch (rt_ptr->u.p.el_len)
    {
    case 4:
      sprintf (var_name, "flt_%d", (*strnum)++);
      strcpy (s->name_type, "ST_FLOAT");
      break;
    case 8:
      sprintf (var_name, "dbl_%d", (*strnum)++);
      strcpy (s->name_type, "ST_DOUBLE");
      break;
    default:
      {
      MLOG_ERR2 ("MS_RT_TO_TYPEDEF: Invalid element len (%d) for type (%d)", 
		  rt_ptr->u.p.el_len, rt_ptr->el_tag);
      return (SD_FAILURE);
      }
    }
  return (SD_SUCCESS);
  }

