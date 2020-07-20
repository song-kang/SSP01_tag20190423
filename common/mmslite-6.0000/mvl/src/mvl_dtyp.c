/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	1994 - 2005, All Rights Reserved		        */
/*									*/
/* MODULE NAME : mvl_dtyp.c   						*/
/* PRODUCT(S)  : MMSEASE-LITE						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*	NONE								*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 11/23/11  KCR           Added casting for proper 64bit compile       */
/* 02/03/10  NAV     09    Open VMS Port				*/
/* 01/19/05  JRB     08    Use mvlu_trim_branch_name from mvl_uca.c	*/
/* 12/19/02  JRB     07    Use ms_comp_name_find, ms_comp_name_pres.	*/
/* 11/19/02  RKR     06    Corrected size in cardinality function	*/
/* 02/27/02  JRB     05    Del unneeded prototypes & includes.		*/
/* 12/04/01  RKR     04    ported to QNX cleaned up errors and warnings */
/* 11/09/01  RKR     03    resorted the num_rt_blks			*/
/* 01/21/00  MDE     02    Now use MEM_SMEM for dynamic memory		*/
/* 10/05/99  RKR     01    Created					*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"

#include "mmsdefs.h"
#include "mms_pvar.h"
#include "mms_vvar.h"
#include "mms_pvmd.h"
#include "mms_err.h"
#include "mms_pcon.h"
#include "mvl_log.h"
#include "mvl_defs.h"
#include "mvl_uca.h"	/* need mvlu_trim_branch_name	*/
#include "asn1defs.h"
#include "stime.h"

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/
/************************************************************************/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

static ST_UCHAR  *create_bitstring (ST_INT num);
static ST_VOID   set_bit ( ST_UCHAR *bptr, ST_INT i);
static ST_BOOLEAN ismember( ST_UCHAR	*bptr,
		     ST_INT	n);
static ST_VOID reSortRT (RUNTIME_TYPE *rt, ST_INT numRt);
static ST_VOID  add_to_branch_name (ST_CHAR *branch_name, SD_CONST ST_CHAR *SD_CONST sub_branch_name);

static ST_INT cardinality (ST_UCHAR *bptr, ST_INT size);
static ST_VOID calc_offset_to_last (RUNTIME_TYPE *rt);

ST_VOID mvl_print_type( ST_CHAR *base_name, ST_INT typeIdIn);
ST_BOOLEAN u_mvl_rt_element_supported (RUNTIME_TYPE *rt, 
				       ST_CHAR *element_name,
				       ST_RTREF *ui, ST_CHAR *handle);

ST_VOID adjRtBlks( RUNTIME_TYPE *derived_type_table, ST_INT derived_count);

/************************************************************************/
/*                           mvl_derive_new_type                        */
/************************************************************************/

ST_RET  mvl_derive_new_type (ST_CHAR *base_name, ST_INT typeIdIn, ST_INT *typeIdOut, ST_CHAR *handle)
  {
ST_RET  ret;
ST_RTREF rtrefs;
ST_UCHAR *membership;

  ret = mvl_derive_type_members (base_name, typeIdIn, &membership, &rtrefs, handle );
  mvl_print_derived_type (base_name, typeIdIn, membership, (ST_UCHAR **) rtrefs);
  ret = mvl_add_derived_type (base_name, typeIdIn, membership, (ST_RTREF *) rtrefs, typeIdOut);
  return (ret);  
  }

/************************************************************************/
/*                     mvl_derive_type_members                          */
/************************************************************************/

ST_RET  mvl_derive_type_members (ST_CHAR *base_name, ST_INT typeIdIn, 
				 ST_UCHAR **membership, ST_RTREF *rtrefs, ST_CHAR *handle)
  {
ST_RET          ret = SD_FAILURE;
RUNTIME_TYPE    *rt;
ST_INT          num_rt;
ST_BOOLEAN      exists;
ST_CHAR         element_name[MAX_IDENT_LEN+1];  
ST_CHAR         branch_name[MAX_IDENT_LEN+1];  
ST_RTREF        *user_info;
ST_INT          i;
ST_RTREF        ui;    /* let the user pass back widget of unknown purpose */
ST_UCHAR        *bptr;

  ret = mvl_get_runtime (typeIdIn, &rt, &num_rt);
  if (ret != SD_SUCCESS)
    {
    MVL_LOG_NERR1 ("TypeId %d not found",typeIdIn);	     
    return (ret);
    }

  bptr = create_bitstring (num_rt);
  *membership = bptr;
  user_info = (ST_RTREF *) M_CALLOC (MSMEM_GEN, num_rt, sizeof(ST_RTREF *));
  *rtrefs = user_info;

  for (i=0; i<num_rt; i++)
    {
    exists = SD_FALSE;

    if ((rt[i].el_tag == RT_STR_START) && (ms_comp_name_pres(&rt[i])))
      {
      add_to_branch_name (branch_name, ms_comp_name_find(&rt[i]));
      strcpy (element_name, branch_name);
      }
    else
      {
      if (ms_comp_name_pres(&rt[i]))
	sprintf (element_name, "%s$%s", branch_name, ms_comp_name_find(&rt[i]));
      else
	{
	if (i==0)       /* This is the start of the entire RuntimeType  */
	  {
	  strcpy (branch_name, base_name);
	  strcpy (element_name, branch_name);
	  }
	else
	  element_name[0]=0;    /* nobody home */
	}
      }

    if (rt[i].el_tag == RT_STR_END)
      {
      strcpy( element_name, branch_name);
      mvlu_trim_branch_name (branch_name);
      }

    ui = rt[i].mvluTypeInfo.ref;
    exists = u_mvl_rt_element_supported(&rt[i], element_name, &ui, handle);
    if (exists)
      {
      set_bit (bptr, i);
      user_info[i] = ui;
      }
    }

  return (ret);
  }


/************************************************************************/
/*                          mvl_add_derived_type                        */
/************************************************************************/

ST_RET  mvl_add_derived_type( ST_CHAR *base_name, ST_INT typeIdIn, 
			      ST_UCHAR *membership, ST_RTREF *rtrefs,
			      ST_INT *typeIdOut)
  {
ST_RET          ret;
ST_INT          derived_elements;
RUNTIME_TYPE    *derived_type_table;
ST_INT          derived_count;
RUNTIME_TYPE    *rt;
ST_INT          num_rt;
ST_INT          i;


  ret = mvl_get_runtime (typeIdIn, &rt, &num_rt);
  if (ret != SD_SUCCESS)
    {
    MVL_LOG_NERR1 ("TypeId %d not found",typeIdIn);	     
    return (ret);
    }

  /* count how many elements are in the new type                        */
  derived_elements = cardinality( membership, num_rt);

  /* allocate a new RT type table about that large                      */
  derived_type_table = (RUNTIME_TYPE *)M_CALLOC (MSMEM_GEN, derived_elements,
						   sizeof(RUNTIME_TYPE));

  /* copy the elements marked as members of the new type into the new   */
  /* RT type table                                                      */
  derived_count = 0;
  for(i=0; i<num_rt; i++)
    {
    if (ismember (membership, i))
      {
      memcpy (&derived_type_table[derived_count], &rt[i], sizeof(RUNTIME_TYPE));
      derived_type_table[derived_count].mvluTypeInfo.ref = rtrefs[i];
      derived_count++;
      }
    }

  M_FREE (MSMEM_GEN, membership);
  M_FREE (MSMEM_GEN, rtrefs);

  /* fix up the offsets.                                                */
  for (i=0; i<derived_count; i++)
    {
    calc_offset_to_last(&derived_type_table[i]);
    }

  /* fix up the number of RuntimeBlocks contained in arrays and structures */
  adjRtBlks(derived_type_table,derived_count);

  /* resort the UCA names so GNL will return sorted strings */
  reSortRT (derived_type_table, derived_count);

  /* we get to finally add a MVL type!                                  */
  ret = mvlu_add_rt_type (derived_type_table, derived_count, typeIdOut);
 
  /*mvl_print_type (base_name, *typeIdOut);*/
  return (ret);
  }

/************************************************************************/
/*                          mvl_print_type                      */
/************************************************************************/
#if 0
ST_VOID mvl_print_type( ST_CHAR *base_name, ST_INT typeIdIn)
  {
ST_RET          ret;
ST_INT          i;
RUNTIME_TYPE    *rt;
ST_INT          num_rt;
ST_CHAR         element_name[MAX_IDENT_LEN+1];  
ST_CHAR         branch_name[MAX_IDENT_LEN+1];  

  ret = mvl_get_runtime (typeIdIn, &rt, &num_rt);
  if (ret != SD_SUCCESS)
    {
    MVL_LOG_NERR1 ("TypeId %d not found",typeIdIn);	     
    }

  for (i=0; i<num_rt; i++)
    {
    if ((rt[i].el_tag == RT_STR_START) && (ms_comp_name_pres(&rt[i])))
      {
      add_to_branch_name (branch_name, ms_comp_name_find(&rt[i]));
      strcpy( element_name, branch_name);
      }
    else
      {
      if (ms_comp_name_pres(&rt[i]))
	sprintf( element_name, "%s$%s", branch_name, ms_comp_name_find(&rt[i]));
      else
	{
	if ( i==0 )     /* This is the start of the entire RuntimeType  */
	  {
	  strcpy( branch_name, base_name);
	  strcpy( element_name, branch_name);
	  }
	else
	  element_name[0]=0;    /* nobody home */
	}
      }

    if (rt[i].el_tag == RT_STR_END)
      {
      strcpy( element_name, branch_name);
      mvlu_trim_branch_name (branch_name);
      }

    _slogc (sLogCtrl,"RT %d '%s' - sortedNum %d",
            i,element_name,rt[i].mvluTypeInfo.sortedNum);
    }
  }
#endif
/************************************************************************/
/*                          mvl_print_derived_type                      */
/************************************************************************/

ST_VOID mvl_print_derived_type( ST_CHAR *base_name, ST_INT typeIdIn, 
				ST_UCHAR *membership, ST_UCHAR **labels)
  {
ST_RET          ret;
ST_INT          i;
RUNTIME_TYPE    *rt;
ST_INT          num_rt;
ST_CHAR         element_name[MAX_IDENT_LEN+1];  
ST_CHAR         branch_name[MAX_IDENT_LEN+1];  

  ret = mvl_get_runtime (typeIdIn, &rt, &num_rt);
  if (ret != SD_SUCCESS)
    {
    MVL_LOG_NERR1 ("TypeId %d not found",typeIdIn);	     
    }

  for (i=0; i<num_rt; i++)
    {
    if ((rt[i].el_tag == RT_STR_START) && (ms_comp_name_pres(&rt[i])))
      {
      add_to_branch_name (branch_name, ms_comp_name_find(&rt[i]));
      strcpy( element_name, branch_name);
      }
    else
      {
      if (ms_comp_name_pres(&rt[i]))
	sprintf( element_name, "%s$%s", branch_name, ms_comp_name_find(&rt[i]));
      else
	{
	if ( i==0 )     /* This is the start of the entire RuntimeType  */
	  {
	  strcpy( branch_name, base_name);
	  strcpy( element_name, branch_name);
	  }
	else
	  element_name[0]=0;    /* nobody home */
	}
      }

    if (rt[i].el_tag == RT_STR_END)
      {
      strcpy( element_name, branch_name);
      mvlu_trim_branch_name (branch_name);
      }

    /*if ( ismember( membership, i))
      {
      printf ( "\n RT %d  element name '%s'",i,element_name);
      _slogc (sLogCtrl,"RT %d '%s' - sortedNum %d",
		     i,element_name,rt[i].mvluTypeInfo.sortedNum);
      }*/
    }
  }

/************************************************************************/
/*                            reSortRT                                  */
/************************************************************************/

static ST_VOID reSortRT (RUNTIME_TYPE *rt, ST_INT numRt)
  {
ST_INT  highestSortNum;
ST_INT  i;
ST_INT  prevSortNum;
ST_INT  newSortNum;

  /* First find the highest 'used' sort number */

  highestSortNum = 0;
  for (i = 0; i < numRt; ++i)
    {
    if (rt[i].mvluTypeInfo.sortedNum > highestSortNum)
      highestSortNum = rt[i].mvluTypeInfo.sortedNum;
    }

  newSortNum = 1;

/* compress the sort numbers */

  for (prevSortNum = 1; prevSortNum <= highestSortNum; ++prevSortNum)
    {
    for (i = 0; i < numRt; ++i)
      {
      if (rt[i].mvluTypeInfo.sortedNum == prevSortNum)
        {
        rt[i].mvluTypeInfo.sortedNum = newSortNum;
        ++newSortNum;
        break;
        }
      }
    }    
  }
/************************************************************************/
/*		                adjRtBlks				*/
/************************************************************************/

ST_VOID adjRtBlks( RUNTIME_TYPE *derived_type_table, ST_INT derived_count)
  {
RUNTIME_TYPE *nested[ASN1_MAX_LEVEL];
ST_INT i;
ST_INT curNestLevel;
RUNTIME_TYPE *start_rt_blk;
RUNTIME_TYPE *rt_ptr;
ST_INT	num_rt;

  curNestLevel = 0;

  rt_ptr = derived_type_table;
  for (i=0; i<derived_count; i++,rt_ptr++)
    {
    if ((rt_ptr->el_tag == RT_STR_START) ||
          (rt_ptr->el_tag == RT_ARR_START))
      {
      nested[curNestLevel] = rt_ptr;  /* remember where we started */
      curNestLevel++;
      }
    else if ((rt_ptr->el_tag == RT_STR_END) ||
              (rt_ptr->el_tag == RT_ARR_END))
      {
      curNestLevel--;
      start_rt_blk = nested[curNestLevel];
      num_rt = (int) (rt_ptr - start_rt_blk - 1); /* figure out how many in between */

      /*if (start_rt_blk->u.str.num_rt_blks != num_rt)
        {
	printf ("\n adjusting %s, was %d, is now %d",
                   ms_comp_name_find(start_rt_blk),
	           start_rt_blk->u.str.num_rt_blks,
		   num_rt);
	}*/

      start_rt_blk->u.str.num_rt_blks = num_rt;
      rt_ptr->u.str.num_rt_blks = num_rt;
      }
    }
  }

/************************************************************************/
/*                            add_to_branch_name                        */
/************************************************************************/

static ST_VOID  add_to_branch_name (ST_CHAR *branch_name, SD_CONST ST_CHAR *SD_CONST sub_branch_name)
  {
  sprintf( branch_name, "%s$%s", branch_name, sub_branch_name);
  }

/************************************************************************/
/*                          create_bitstring                            */
/************************************************************************/
static ST_UCHAR *create_bitstring (ST_INT n)
  {
ST_INT i,j;
ST_UCHAR *s;

  i = n / ( sizeof( ST_UCHAR ) * 8 );
  j = n % ( sizeof( ST_UCHAR ) * 8 );

  if (j) 
    i++;  /* allocate an extra byte if string len ! a multiple of 8 */
  
  s = (ST_UCHAR *)M_CALLOC (MSMEM_GEN, i, sizeof( ST_UCHAR));
  return(s);

  }

/************************************************************************/
/*                              set_bit                                 */
/************************************************************************/
static ST_VOID set_bit (ST_UCHAR *s, ST_INT n)
  {
ST_INT          i,j;
ST_UCHAR        k = 0x01;  /* mask */

  i = n / ( sizeof( ST_UCHAR ) * 8 );
  j = n % ( sizeof( ST_UCHAR ) * 8 );
  s[ i ] = s[ i ] | ( k << j );
  }

/************************************************************************/
/*                              ismember                                        */
/* This function returns 1 if element n belongs to Set s, 0 otherwise.  */
/************************************************************************/

static ST_BOOLEAN ismember( ST_UCHAR   *bptr,
		     ST_INT     n)
  {
ST_INT           i,j;
ST_UCHAR         k = 0x01;  /* mask */

  i = n / ( sizeof( ST_UCHAR ) * 8 );
  j = n % ( sizeof( ST_UCHAR ) * 8 );
  return( bptr[ i ] & ( k << j ));
  }

/************************************************************************/
/*                          cardinality                                 */
/************************************************************************/

static ST_INT cardinality (ST_UCHAR *bptr, ST_INT size)
  {
ST_INT          i, j, s, r, card;
ST_CHAR         temp;

  s = size / ( sizeof( ST_UCHAR ) * 8 );
  r = size % ( sizeof( ST_UCHAR ) * 8 );

  if (r) 
    s++; 
  card=0;
  for ( i = 0; i < s; i++ ) 
    {
    for ( j = 0; j < 8 * sizeof( ST_UCHAR ); j++ ) 
      {
      temp = 0x01;
      temp = temp << j;
      if ( temp & bptr[ i ] ) 
	card++;
      }
    }
  return( card );
  }

/************************************************************************/
/*                          calc_offset_to_last                         */
/************************************************************************/
/* This function would not be needed except that the UCA structure      */
/* handling needs to know the size of each subelement too ...           */

static ST_VOID calc_offset_to_last (RUNTIME_TYPE *rt) 
  {
ST_INT numUcaRt;

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
  rt->offset_to_last = ms_get_blocked_length (rt, numUcaRt);
  }


