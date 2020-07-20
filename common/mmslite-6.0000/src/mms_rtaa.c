/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2002, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : mms_rtaa.c						*/
/* PRODUCT(S)  : MMSEASE						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/* Function to decode ASN.1 type definitions into Runtime typedefs	*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*			_ms_mk_rt_aa_all				*/
/*			_ms_mk_rt_aa					*/
/*			_ms_get_rtaa_data_size				*/
/*			_ms_log_runtime_aa				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 02/25/10  JRB	   _ms_mk_rt_aa_all & _ms_calc_rt_aa_size: init	array*/
/*			   Del globals "arr_loop*", use locals.		*/
/* 07/27/05  MDE     06    if m_max_rt_aa_cltr == 0 calc required size  */
/* 05/11/05  MDE     05    Added _ms_calc_rt_aa_size			*/
/* 02/22/05  JRB     04    Make "sp" local to avoid VXWORKS conflict.	*/
/* 04/02/03  JRB     03    Add UTF8string support (see RT_UTF8_STRING).	*/
/* 03/04/02  JRB     02    Add more "SD_CONST RUNTIME_TYPE *".		*/
/*			   Make sure str_end_rtaa, arr_end_rtaa init'd.	*/
/* 02/06/02  JRB     01    New. Code from mms_alta.c moved here.	*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "glbsem.h"

#include "mmsdefs.h"
#include "mms_pvar.h"
#include "mms_vvar.h"
#include "asn1defs.h"

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/
/************************************************************************/

#define COPY_RTAA(dest,src) { dest->rt = src->rt;\
                              dest->offset_to_data = src->offset_to_data; }

/************************************************************************/
/************************************************************************/
/* This code is required for handling real alternate access data 	*/
/* conversion via mmsdataa, and for using the ARB DATA handlers.	*/

/************************************************************************/
/************************************************************************/
/* Runtime AA (RTAA) Handling						*/
/************************************************************************/
/************************************************************************/
static ST_VOID _ms_calc_rt_aa_size (SD_CONST RUNTIME_TYPE *rt_head,  ST_INT rt_num, 
                       	    ST_INT *num_aa_out);

static ST_RET get_struct_comps (AA_APPLY_CTRL *aa_app, RT_AA_CTRL *w_rtaa);
static ST_RET get_arr_elmnts (AA_APPLY_CTRL *aa_app, RT_AA_CTRL *w_rtaa);
static ST_INT count_num_comp (AA_APPLY_CTRL *aa_app, 
                      ALT_ACC_EL *aa, 
                      ST_INT *count_out);

static RT_AA_CTRL *find_end_rtaa (AA_APPLY_CTRL *aa_app,
                                  RT_AA_CTRL *start_rtaa);

static RT_AA_CTRL *find_named_comp (AA_APPLY_CTRL *aa_app, 
                                    ST_CHAR *name, 
                                    RT_AA_CTRL *w_rtaa);

static ST_RET get_component_elmnts (AA_APPLY_CTRL *aa_app, 
                            RT_AA_CTRL *rtaa);

static RT_AA_CTRL *find_arr_el (AA_APPLY_CTRL *aa_app, 
                                RT_AA_CTRL *w_rtaa, 
                                ST_INT el_index);
static ST_VOID ms_rtaa_el_size_calc (RT_AA_CTRL *runtime_aa, ST_INT num_rt_aa);

/************************************************************************/
/* Create a RTAA with all elements for the specified RT type            */
/* This table is allocated via chk_calloc, and is sized for the max     */
/* possible AA size. If desired, this may be realloc'd to the correct   */
/* size by the caller.                                                  */

ST_RET _ms_mk_rt_aa_all (SD_CONST RUNTIME_TYPE *rt_head,  ST_INT rt_num, 
                       RT_AA_CTRL **rt_aa_out, ST_INT *num_aa_out)
  {
RT_AA_CTRL *rtaa;
ST_INT data_offset;
SD_CONST RUNTIME_TYPE *rt_end;
SD_CONST RUNTIME_TYPE *rt;
/* NOTE: array was arr_loop_buf & arr_loops pointed to it. Don't need both.*/
ST_INT arr_loops[ASN1_MAX_LEVEL];
ST_INT arr_loop_level;
ST_INT _max_rt_aa_ctrl;

  memset (arr_loops, 0, sizeof(arr_loops));	/* start clean	*/
  S_LOCK_COMMON_RESOURCES ();

  arr_loop_level = 0;

/* Allocate the output array for max size                               */
  _max_rt_aa_ctrl = m_max_rt_aa_ctrl;
  if (_max_rt_aa_ctrl == 0)
    _ms_calc_rt_aa_size (rt_head,  rt_num, &_max_rt_aa_ctrl);
    
  rtaa = (RT_AA_CTRL *) chk_calloc (_max_rt_aa_ctrl, sizeof (RT_AA_CTRL));
  *rt_aa_out = rtaa;
  *num_aa_out = 0;

  rt = rt_head;		                /* set runtime ptr		*/
  rt_end = rt_head + rt_num;  	        /* end block			*/
  data_offset = 0;

/* Traverse the RT type as though we were doing a ASN.1 to LOCAL, and	*/
/* set the RTAA RT pointer and offset as each element is encountered.	*/

  while (rt < rt_end)
    {
    if (rt->el_tag == RT_ARR_END)	/* treat case of array ending	*/
      {
      if (--arr_loops[arr_loop_level] > 0) /* if need to do next ar elmnt	*/
        {
        rt -= rt->u.arr.num_rt_blks;    /* mv rt to start of arr	*/
        continue; 
        }
      else
        --arr_loop_level;
      }

    (*num_aa_out)++;			/* Count this one		*/
    rtaa->rt = rt;			/* Set the RT pointer		*/
    rtaa->offset_to_data = data_offset; /* Set the offset		*/
    data_offset += rt->el_size;		/* Offset for next		*/

    if (rt->el_tag == RT_ARR_START)	/* Set up array looping		*/
      {
      ++arr_loop_level;
      arr_loops[arr_loop_level] = rt->u.arr.num_elmnts;
      }

    rtaa++;			/* point to next runtime AA element	*/
    rt++;			/* point to next runtime element	*/
    }

/* Post process to calculate the 'rtaa->el_size' values (for packed 	*/
/* local representation).						*/
  ms_rtaa_el_size_calc (*rt_aa_out, *num_aa_out);

  if (mms_debug_sel & MMS_LOG_RTAA)
    _ms_log_runtime_aa (*rt_aa_out, *num_aa_out);

/* save the high water mark						*/
  if (*num_aa_out >  m_hw_rt_aa_ctrl)
    m_hw_rt_aa_ctrl = *num_aa_out;

  S_UNLOCK_COMMON_RESOURCES ();
  return (SD_SUCCESS);
  }

/************************************************************************/
/************************************************************************/
/*			_ms_calc_rt_aa_size				*/
/************************************************************************/

static ST_VOID _ms_calc_rt_aa_size (SD_CONST RUNTIME_TYPE *rt_head,  ST_INT rt_num, 
                       	    ST_INT *num_aa_out)
  {
SD_CONST RUNTIME_TYPE *rt_end;
SD_CONST RUNTIME_TYPE *rt;
/* NOTE: array was arr_loop_buf & arr_loops pointed to it. Don't need both.*/
ST_INT arr_loops[ASN1_MAX_LEVEL];
ST_INT arr_loop_level;
ST_INT num_aa;

  memset (arr_loops, 0, sizeof(arr_loops));	/* start clean	*/
  S_LOCK_COMMON_RESOURCES ();
  arr_loop_level = 0;

  num_aa = 0;
  rt = rt_head;		                /* set runtime ptr		*/
  rt_end = rt_head + rt_num;  	        /* end block			*/

/* Traverse the RT type as though we were doing a ASN.1 to LOCAL, and	*/
/* count the RTAA elements						*/
  while (rt < rt_end)
    {
    if (rt->el_tag == RT_ARR_END)	/* treat case of array ending	*/
      {
      if (--arr_loops[arr_loop_level] > 0) /* if need to do next ar elmnt	*/
        {
        rt -= rt->u.arr.num_rt_blks;    /* mv rt to start of arr	*/
        continue; 
        }
      else
        --arr_loop_level;
      }
    ++num_aa;				/* Count this one		*/

    if (rt->el_tag == RT_ARR_START)	/* Set up array looping		*/
      {
      ++arr_loop_level;
      arr_loops[arr_loop_level] = rt->u.arr.num_elmnts;
      }
    rt++;			/* point to next runtime element	*/
    }
  *num_aa_out = num_aa;
  S_UNLOCK_COMMON_RESOURCES ();
  }

/************************************************************************/
/************************************************************************/
/************************************************************************/
/* Create a RTAA with elements selected by a AA for the specified RT 	*/
/* type            							*/
/* This table is allocated via chk_calloc, and is sized for the max     */
/* possible AA size. If desired, this may be realloc'd to the correct   */
/* size by the caller.                                                  */

ST_RET _ms_mk_rt_aa (SD_CONST RUNTIME_TYPE *rt,  ST_INT rt_num, 
                   ALT_ACCESS *alt_acc,
                   RT_AA_CTRL **rt_aa_out, ST_INT *num_aa_out)
  {
ALT_ACC_EL *aa;
ST_INT num_aa;
ST_RET ret;
ST_INT num_all_rtaa;
RT_AA_CTRL *all_rtaa;
RT_AA_CTRL *rtaa;
AA_APPLY_CTRL aa_app_ctrl;
ST_UINT32 mms_log_mask_save;
ST_INT _max_rt_aa_ctrl;

  S_LOCK_COMMON_RESOURCES ();

  aa = alt_acc->aa;
  num_aa = alt_acc->num_aa;

/* Make the 'all' rtaa - has all elements expanded, with offsets.      	*/
/* This is used with the AA to select the elements for the new RTAA	*/

/* Don't allow logging of the 'all' runtime aa table                  	*/
  mms_log_mask_save = mms_debug_sel;
  mms_debug_sel &= ~MMS_LOG_RTAA;
  ret = _ms_mk_rt_aa_all (rt, rt_num, &all_rtaa, &num_all_rtaa);
  mms_debug_sel = mms_log_mask_save;

  if (ret)
    {
    S_UNLOCK_COMMON_RESOURCES ();
    return (ret);
    }

/* Allocate the output (new) RT AA table                                */
  _max_rt_aa_ctrl = m_max_rt_aa_ctrl;
  if (_max_rt_aa_ctrl == 0)
    _ms_calc_rt_aa_size (rt,  rt_num, &_max_rt_aa_ctrl);
    
  rtaa = (RT_AA_CTRL *) chk_calloc (_max_rt_aa_ctrl, sizeof (RT_AA_CTRL));
  *rt_aa_out = rtaa;
  *num_aa_out = 0;

/* Initialize the AA process control structure components                */

/* Alternate Access     */
  aa_app_ctrl.num_aa = num_aa;
  aa_app_ctrl.aa_head = aa;
  aa_app_ctrl.aa_end = aa + num_aa - 1;
  aa_app_ctrl.curr_aa = aa; 

/* Default RTAA         */
  aa_app_ctrl.num_all_rtaa = num_all_rtaa;
  aa_app_ctrl.all_rtaa_head = all_rtaa;
  aa_app_ctrl.all_rtaa_end = all_rtaa + (num_all_rtaa -1);

/* New RTAA                                                             */
  aa_app_ctrl.num_rtaa = 0;
  aa_app_ctrl.rtaa_head = rtaa;
  aa_app_ctrl.curr_rtaa = rtaa;

/* Nest Level */
  aa_app_ctrl.nest = 0;

/* Get the appropriate rt_aa_ctrl elements                              */
/* Start by getting the first nest level elements, based on the RT.	*/
/* Note that these functions will call recursively to get handle 	*/
/* further nest	levels							*/

  if (rt->el_tag == RT_ARR_START)
    {
    ret = get_arr_elmnts (&aa_app_ctrl, all_rtaa);
    }
  else if (rt->el_tag == RT_STR_START)
    {
    ret = get_struct_comps (&aa_app_ctrl, all_rtaa);
    }
  else
    {
    MLOG_NERR0 ("Runtime Type must be a Structure or Array for AA");
    ret = MVE_RT_TYPE;
    }

/* If successful, set the output AA count and log if enabled		*/
  if (ret == SD_SUCCESS)
    {
    *num_aa_out = aa_app_ctrl.num_rtaa;

/* Post process to calculate the 'rtaa->el_size' values (for packed 	*/
/* local representation).						*/
    ms_rtaa_el_size_calc (*rt_aa_out, *num_aa_out);

    if (mms_debug_sel & MMS_LOG_RTAA)
      _ms_log_runtime_aa (*rt_aa_out, *num_aa_out);

/* save the high water mark						*/
    if (*num_aa_out >  m_hw_rt_aa_ctrl)
      m_hw_rt_aa_ctrl = *num_aa_out;
    }
  else
    {
    chk_free (rtaa);		/* If error, need to free the RTAA	*/
    }

  chk_free (all_rtaa);		/* we are done with the 'all' RTAA	*/
  S_UNLOCK_COMMON_RESOURCES ();

  return (ret);
  }

/************************************************************************/
/* We are working on a structure level, and need to copy the selected   */
/* RTAA components for this nest level. This function can be called     */
/* recursively as further nesting levels are encountered.               */
/* The 'w_rtaa' parameter is the 'working' RTAA, and is used to select 	*/
/* the scope (nest) for the operation					*/

static ST_RET get_struct_comps (AA_APPLY_CTRL *aa_app, RT_AA_CTRL *w_rtaa)
  {
ST_RET ret;
RT_AA_CTRL *o_rtaa;
ALT_ACC_EL *aa;
ST_INT num_str_comp;
RT_AA_CTRL *start_rtaa;
RT_AA_CTRL *str_end_rtaa = NULL;
ST_BOOLEAN done;

/* get working pointers                                                 */
  aa = aa_app->curr_aa;                         /* Current AA           */
  o_rtaa = aa_app->curr_rtaa;                   /* Output RTAA          */

/* Check to see if we need to have the outer structure                  */
/* We need this if there are more than 1 named components selected      */

  if (count_num_comp (aa_app, aa, &num_str_comp))
    return (MVE_AA_SELECT);

  if (num_str_comp != 1)
    {   
    str_end_rtaa = find_end_rtaa (aa_app, w_rtaa);
    if (!str_end_rtaa)
      return (MVE_AA_SELECT);

    /* Write the start component of the output rtaa                     */
    COPY_RTAA (o_rtaa,w_rtaa);
    aa_app->num_rtaa++;
    aa_app->curr_rtaa++;
    }

/* Get all selected named components                                  	*/
  ret = SD_SUCCESS;
  done = SD_FALSE;
  while (!done && ret == SD_SUCCESS)
    {
    aa = aa_app->curr_aa;		/* get current AA pointer	*/
    o_rtaa = aa_app->curr_rtaa;		/* get the output RTAA pointer	*/

  /* This AA sel type can be a named component (w/wo further nesting)	*/
  /* or a 'end nest' element.						*/
    switch (aa->sel_type)
      {
      case  AA_COMP :
	/* This AA selects a entire named component. Just find the 	*/
	/* component name in the ALL RTAA and copy it's RTAA elements	*/

        start_rtaa = find_named_comp (aa_app, aa->u.component, w_rtaa);
        if (!start_rtaa)
          {
          return (MVE_AA_SELECT);
          }
        ret = get_component_elmnts (aa_app, start_rtaa);
        ++aa_app->curr_aa;
      break;
  
      case  AA_COMP_NEST  :
	/* This AA selects a named component, with further selection. 	*/
	/* Find the component name in the ALL RTAA and call recursively	*/
	/* to get the components.					*/

        /* This component must be a structure or an array, so that we 	*/
	/* need to call recursively to get the selected RT AA elements	*/

        start_rtaa = find_named_comp (aa_app, aa->u.component, w_rtaa);
        if (!start_rtaa)
          return (MVE_AA_SELECT);

        ++aa_app->nest;		/* Increment the nest level		*/
				/* Point to the next AA 		*/
        if (aa_app->curr_aa < aa_app->aa_end)
          ++aa_app->curr_aa;
        else
          {
          MLOG_NERR0 ("AA table too small");
          return (MVE_AA_SELECT);
          }

	/* Based on the RT type, get the next level with the scope 	*/
	/* defined by the start RTAA.					*/
        if (start_rtaa->rt->el_tag == RT_ARR_START)
          {
          ret = get_arr_elmnts (aa_app, start_rtaa);
          }
        else if (start_rtaa->rt->el_tag == RT_STR_START)
          {
          ret = get_struct_comps (aa_app, start_rtaa);
          }
        else
          {
          MLOG_NERR0 ("Incorrect RT Type for further nesting");
          return (MVE_RT_TYPE);
          }
        --aa_app->nest;		/* Decrement the nest level		*/

       /* curr_aa should now point to our END_NEST, consume it		*/
        if (aa_app->curr_aa->sel_type != AA_END_NEST)
          {
          MLOG_NERR0 ("Required AA_END_NEST not present");
          return (MVE_AA_SELECT);
          }
        else			/* Consume the AA_END_NEST		*/
          ++aa_app->curr_aa;
      break;

      case  AA_END_NEST :
        done = SD_TRUE;            /* This level is complete               */
      break;

      case  AA_ALL :
      case  AA_ALL_NEST   :
      case  AA_INDEX_NEST :
      case  AA_INDEX_RANGE_NEST :
      case  AA_INDEX :
      case  AA_INDEX_RANGE :
        MLOG_NERR0 ("Incorrect AA sel_type for structure component selection");
        return (MVE_AA_SELECT);
      break;

      default :
        MLOG_NERR0 ("Invalid AA sel_type");
        return (MVE_AA_SELECT);
      break;
      }

  /* Prepare to look at next AA, if this was not the last */    
    if (aa_app->curr_aa > aa_app->aa_end)
      done = SD_TRUE;
    } /* End while getting named components at this level     */


/* Don't continue if an error was detected				*/
  if (ret)
    return (ret);


/* Now copy the structure end element if required                       */
  if (num_str_comp != 1)
    {   
    o_rtaa = aa_app->curr_rtaa;
    COPY_RTAA (o_rtaa, str_end_rtaa);
    aa_app->num_rtaa++;
    aa_app->curr_rtaa++;
    }

  return (ret);
  }

/************************************************************************/
/* We are working on a array selection, and need to copy the selected   */
/* RTAA components for this selection. This function can be called      */
/* recusively as further nesting levels are encountered.                */
/* The 'w_rtaa' parameter is the 'working' RTAA, and is used to select 	*/
/* the scope (nest) for the operation					*/


static ST_RET get_arr_elmnts (AA_APPLY_CTRL *aa_app, RT_AA_CTRL *w_rtaa)
  {
ST_RET ret;
ST_INT i;
RT_AA_CTRL *el_rtaa;
RT_AA_CTRL *o_rtaa;
RT_AA_CTRL *arr_end_rtaa = NULL;
ST_INT nest;
ALT_ACC_EL *aa;
ST_INT num_el;
ST_INT start_el;
ST_INT curr_el;


  ret = SD_SUCCESS;

/* get working pointers                                                 */
  aa = aa_app->curr_aa;                         /* Current AA           */
  o_rtaa = aa_app->curr_rtaa;                   /* Output RTAA          */

  /* This AA sel type must be one of the array selection types, 	*/
  /* (w/wo further nesting)						*/

  /* Start by determining the number of elements and the start element	*/
  switch (aa->sel_type)
    {
    case  AA_INDEX :
    case  AA_INDEX_NEST :
      num_el = 1;
      start_el = (ST_INT) aa->u.index;
    break;
  
    case  AA_INDEX_RANGE :
    case  AA_INDEX_RANGE_NEST :
      num_el = (ST_INT) aa->u.ir.num_elmnts;
      start_el = (ST_INT) aa->u.ir.low_index;
      if (!num_el && !start_el)
        {
        num_el = w_rtaa->rt->u.arr.num_elmnts;
        start_el = 0;
        }
    break;

    case  AA_ALL :
    case  AA_ALL_NEST   :
      num_el = w_rtaa->rt->u.arr.num_elmnts;
      start_el = 0;
    break;
  
    case  AA_COMP :
    case  AA_COMP_NEST  :
      MLOG_NERR0 ("Invalid AA Type");
      return (MVE_AA_SELECT);
    break;
  
    default :
      MLOG_NERR0 ("Bad AA Type");
      return (MVE_AA_SELECT);
    break;
    }

/* num_el is the number of elements to get, start_el is the base index  */
/* w_rtaa points to the start of the array in the default RTAA          */

/* Determine whether the array elements have further AA select ...      */
  if (aa->sel_type == AA_INDEX_NEST ||
      aa->sel_type == AA_INDEX_RANGE_NEST ||
      aa->sel_type == AA_ALL_NEST)
    {
    nest = SD_TRUE;
    }
  else
    nest = SD_FALSE;


/* Check to see if we need to have the outer array                      */
/* We need this if there are more than 1 element selected               */

  if (num_el != 1)
    {   
    arr_end_rtaa = find_end_rtaa (aa_app, w_rtaa);
    if (!arr_end_rtaa)
      return (MVE_AA_SELECT);

    /* Write the start component of the output rtaa                     */
    COPY_RTAA (o_rtaa,w_rtaa);
    aa_app->num_rtaa++;
    aa_app->curr_rtaa++;
    }

/* Now we work through the array elements, getting them one at a time   */
  curr_el = start_el;
  for (i = 0; i < num_el && ret == SD_SUCCESS; ++i, ++curr_el)
    {
    aa_app->curr_aa = aa;            /* Restore the working AA position */
    o_rtaa = aa_app->curr_rtaa;      /* Output RTAA                     */
  
    /* Move the working RTAA pointer to the next element to be selected */
    el_rtaa = find_arr_el (aa_app, w_rtaa, curr_el);
    if (!el_rtaa)
      return (MVE_AA_SELECT);

    /* If further selection is to take place, do it                    	*/
    if (nest)
      {
      /* Set up working parameters for the nest level of nesting    	*/
      if (aa_app->curr_aa < aa_app->aa_end)
        ++aa_app->curr_aa;
      else
        {
        MLOG_NERR0 ("AA table too small");
        return (MVE_AA_SELECT);
        }
      ++aa_app->nest;		/* Increment the nest level		*/

      if (el_rtaa->rt->el_tag == RT_ARR_START)
        {
        ret = get_arr_elmnts (aa_app, el_rtaa);
        }
      else if (el_rtaa->rt->el_tag == RT_STR_START)
        {
        ret = get_struct_comps (aa_app, el_rtaa);
        }
      else 		/* Whoops, internal error			*/
        {
        MLOG_NERR0 ("Bad RT Type for further AA selection");
        return (MVE_RT_TYPE);
        }
      --aa_app->nest;		/* Decrement the nest level		*/
      }
    else	/* No further selection, copy the compoment		*/
      {
      ret = get_component_elmnts (aa_app, el_rtaa);
      }
    } /* end for each selected element */
  

/* Now copy the array end element if required                           */
  if (ret == SD_SUCCESS && num_el != 1)
    {   
    o_rtaa = aa_app->curr_rtaa;
    COPY_RTAA (o_rtaa, arr_end_rtaa);
    aa_app->num_rtaa++;
    aa_app->curr_rtaa++;
    }

  if (nest)
    {
   /* curr_aa should now point to our END_NEST, consume it		*/
    if (aa_app->curr_aa->sel_type != AA_END_NEST)
      {
      MLOG_NERR0 ("Required AA_END_NEST not present");
      ret = MVE_AA_SELECT;
      }
    else 			/* Consume the AA_END_NEST		*/
      ++aa_app->curr_aa;
    }
  else
    ++aa_app->curr_aa; 	/* Point to the next AA 		*/

  return (ret);
  }

/************************************************************************/
/************************************************************************/
/* Given the array or structure start RTAA pointer, return a pointer to */
/* the corresponding end element                                        */

static RT_AA_CTRL *find_end_rtaa (AA_APPLY_CTRL *aa_app,
                                         RT_AA_CTRL *start_rtaa)
  {
SD_CONST RUNTIME_TYPE *start_rt;
SD_CONST RUNTIME_TYPE *end_rt;
ST_INT num_rt;
RT_AA_CTRL *rtaa;

  start_rt = start_rtaa->rt;
  if (start_rt->el_tag == RT_ARR_START)
    {
    num_rt = start_rt->u.arr.num_rt_blks;
    }
  else if (start_rt->el_tag == RT_STR_START)
    {
    num_rt = start_rt->u.str.num_rt_blks;
    }
  else
    {
    MLOG_NERR0 ("Bad RT Type");
    return (NULL);
    }

  end_rt = start_rt + (num_rt + 1);

/* Just a trivial sanity check */
  if (end_rt->el_tag != RT_ARR_END &&
      end_rt->el_tag != RT_STR_END)
    {
    MLOG_NERR0 ("Bad RT Type");
    return (NULL);
    }

/* Now find the RTAA that has a reference to the end structure          */
/* within this scope (we hope!)                                         */

  for (rtaa = start_rtaa; rtaa <= aa_app->all_rtaa_end; ++rtaa)
    {
    if (rtaa->rt == end_rt)
      return (rtaa);
    }

  MLOG_NERR0 ("End RT Not Found");
  return (NULL);
  }

/************************************************************************/
/* Copy the current RTAA element, including any sub-elements            */

static ST_RET get_component_elmnts (AA_APPLY_CTRL *aa_app, RT_AA_CTRL *rtaa)
  {
RT_AA_CTRL *last_rtaa;
RT_AA_CTRL *dest_rtaa;

  if (rtaa->rt->el_tag == RT_ARR_START ||
      rtaa->rt->el_tag == RT_STR_START)
    {
    last_rtaa = find_end_rtaa (aa_app, rtaa);
    if (!last_rtaa)
      return (MVE_AA_SELECT);
    }
  else                          /* Simple element, just copy one        */
    last_rtaa = rtaa;

  dest_rtaa = aa_app->curr_rtaa;
  while (rtaa <= last_rtaa)
    {
    COPY_RTAA (dest_rtaa, rtaa);
    aa_app->num_rtaa++;
    aa_app->curr_rtaa++;
    ++dest_rtaa;
    ++rtaa;
    }
  return (SD_SUCCESS);
  }

/************************************************************************/
/************************************************************************/
/* Count the number of named components selected at this level          */
/* by the alternate access select                                       */

static ST_INT count_num_comp (AA_APPLY_CTRL *aa_app, 
                             ALT_ACC_EL *aa, 
                             ST_INT *count_out)
  {
ST_INT count;
ST_INT nest;
ST_INT done;

  done = SD_FALSE;
  nest = 0;
  count = 0;
  while (done == SD_FALSE)
    {
    switch (aa->sel_type)
      {
      case  AA_COMP :
        if (nest == 0)
          ++count;
      break;

      case  AA_COMP_NEST  :
        if (nest == 0)
          ++count;
        nest++;
      break;

      case  AA_INDEX :
      case  AA_INDEX_RANGE :
      case  AA_ALL :
      break;

      case  AA_INDEX_NEST :
      case  AA_INDEX_RANGE_NEST :
      case  AA_ALL_NEST   :
        nest++;     
      break;
    
      case  AA_END_NEST   :
        if (--nest < 0)
          done = SD_TRUE;
      break;
    
      default :
        MLOG_NERR0 ("Bad AA Type");
        return (MVE_AA_SELECT);
      break;
      }
                        
    if (aa >= aa_app->aa_end)   /* If we are done (level 0 only)        */
      done = SD_TRUE;

    aa++;                       /* Point to next                        */
    }

  *count_out = count;
  return (SD_SUCCESS);
  }


/************************************************************************/
/* Given a RTAA pointer, find a named component at the current nest     */
/* The 'w_rtaa' parameter is the 'working' RTAA, and is used to select 	*/
/* the scope (nest) for the operation					*/

static RT_AA_CTRL *find_named_comp (AA_APPLY_CTRL *aa_app, 
                                           ST_CHAR *name, 
                                           RT_AA_CTRL *w_rtaa)
  {
ST_INT nest;
ST_INT err;

  if (w_rtaa->rt->el_tag != RT_STR_START)
    {
    MLOG_NERR0 ("Bad RT Type - must be structure start");
    return (NULL);
    }
  w_rtaa++;

  nest = 0;
  err = SD_FALSE;
  while (!err)
    {
    if (nest == 0 &&
        !strcmp (name, ms_comp_name_find (w_rtaa->rt)))
      {
      return (w_rtaa);
      }
    switch (w_rtaa->rt->el_tag)
      {
      case  RT_ARR_START      :
      case  RT_STR_START      :
        nest++;
      break;

      case  RT_STR_END 	      :
      case  RT_ARR_END 	      :
        if (--nest < 0)
          break;
      break;

      case  RT_BOOL	      :
      case  RT_BIT_STRING     :
      case  RT_INTEGER	      :
      case  RT_UNSIGNED	      :
      case  RT_FLOATING_POINT :
      case  RT_OCTET_STRING   :
      case  RT_VISIBLE_STRING :
      case  RT_GENERAL_TIME   :
      case  RT_BINARY_TIME    :
      case  RT_BCD	      :
      case  RT_UTC_TIME       :
      case  RT_BOOLEANARRAY   :
      case  RT_UTF8_STRING :
      break;

      default :
        err = SD_TRUE;
        MLOG_NERR0 ("Bad RT Type");
      break;
      }

    w_rtaa++;
    if (w_rtaa >= aa_app->all_rtaa_end)
      break;
    }

  MLOG_NERR1 ("AA Named component (%s) not found", name);
  return (NULL);
  }


/************************************************************************/
/************************************************************************/
/* Given a 'working' RTAA pointer to the RT_ARR_START element and the   */
/* element index, return a pointer to the start RTAA for the element    */
/* The 'w_rtaa' parameter is the 'working' RTAA, and is used to select 	*/
/* the scope (nest) for the operation					*/

static RT_AA_CTRL *find_arr_el (AA_APPLY_CTRL *aa_app, 
                                       RT_AA_CTRL *w_rtaa, 
                                       ST_INT el_index)
  {
RT_AA_CTRL *ret_rtaa; 
SD_CONST RUNTIME_TYPE *arr_rt;
SD_CONST RUNTIME_TYPE *start_rt;
ST_INT el;

  arr_rt = w_rtaa->rt;
  if (arr_rt->el_tag != RT_ARR_START)
    {
    MLOG_NERR0 ("RTAA is not RT_ARR_START");
    return (NULL);
    }
  if (el_index >= arr_rt->u.arr.num_elmnts)
    {
    MLOG_NERR0 ("Index out of range");
    return (NULL);
    }

/* Now we need to locate the el_index'th element. To do this, we first	*/
/* get a pointer to the 0'th element's first RT block, then search for 	*/
/* the el_index'th occurance of the RT within the RTAA 			*/

  w_rtaa++;				 /* 0'th element		*/
  start_rt = w_rtaa->rt;		 /* Start RT block		*/
  el = 0;
  while (w_rtaa <= aa_app->all_rtaa_end) /* while still in RTAA		*/
    {
    if (w_rtaa->rt == start_rt)		 /* if start of an element	*/
      {
      if (el == el_index)		 /* if selected element		*/
        return (w_rtaa);		 /* we found it			*/
      else
        ++el;				 /* maybe next time		*/
      }
    w_rtaa++;				 /* look at next RTAA		*/
    }

  ret_rtaa = (w_rtaa+1) + (el_index * arr_rt->u.arr.num_rt_blks);
  return (ret_rtaa);
  }


/************************************************************************/
/* 			ms_rtaa_el_size_calc 				*/
/************************************************************************/

/* Given a RTAA table, calculate the 'el_size' for each element. This	*/
/* is done by using the alignment control table, and is used only for	*/
/* encoding/decoding AA data that is 'packed' in local memory.		*/

static ST_VOID ms_rtaa_el_size_calc (RT_AA_CTRL *runtime_aa, 
			          ST_INT num_rt_aa)
  {
ST_INT ellen;
SD_CONST RUNTIME_TYPE *rt; 
RT_AA_CTRL *rtaa_above; 
ST_INT curr_offset;
ST_INT pad;
ST_INT algn;
ST_INT i;

  curr_offset = 0;		/* Offset from start of data		*/
  rtaa_above = NULL; 
  for (i = 0; i < num_rt_aa; ++i, ++runtime_aa)
    {
    rt = runtime_aa->rt;

    /* Get 'algnment index' and size of the element (excluding padding)	*/
    _ms_m_get_rt_info (rt, &algn, &ellen);

    /* Set the base element size, before padding   			*/
    runtime_aa->el_size = ellen;

    /* Check to see if padding on the element above is required for 	*/
    /* alignment for this element, add it if so.		  	*/
    /* Increment the offset until no bits are set that shouldn't be	*/
    pad = 0;
    while (curr_offset & algn)
      {
      curr_offset++;
      pad++;
      }

    if (pad)
      {
      if (!rtaa_above) /* First element better not require alignment 	*/
        {
        MLOG_ERR0 ("Alignment table error");
        }
      else	/* add pad to element above */
        rtaa_above->el_size += pad;
      }

  /* Save the pointer to the previous RT element (not this one's name)	*/
    rtaa_above = runtime_aa; 

    /* Point past the current element 		 			*/
    curr_offset += ellen;
    }
  }

/************************************************************************/
/*			_ms_get_rtaa_data_size				*/
/************************************************************************/

ST_INT _ms_get_rtaa_data_size (RT_AA_CTRL *runtime_aa, 
			       ST_INT num_rt_aa)
  {
ST_INT i;
ST_INT size;

  size = 0; 
  for (i = 0; i < num_rt_aa; ++i, ++runtime_aa)
    size += runtime_aa->el_size;
  return (size);
  }

/************************************************************************/
/************************************************************************/
/* LOGGING FUNCTIONS							*/
/************************************************************************/
/************************************************************************/
/************************************************************************/
/*			_ms_log_runtime_aa				*/
/************************************************************************/

ST_VOID _ms_log_runtime_aa (RT_AA_CTRL *rt_aa, ST_INT num_aa)
  {
#ifdef DEBUG_SISCO
SD_CONST RUNTIME_TYPE *rt;

ST_INT i;
ST_INT indent;
SD_CONST ST_CHAR *comp_name;
/* Use these strings to keep logging aligned and indented as desired	*/
#define MAX_INDENT 5
SD_CONST ST_CHAR *sp[MAX_INDENT] =
  {
  "  ",
  "    ",
  "      ",
  "        ",
  "          ",
  };
SD_CONST ST_CHAR *sp2[MAX_INDENT] =
  {
  "        ",
  "      ",
  "    ",
  "  ",
  "",
  };

  S_LOCK_COMMON_RESOURCES ();
  indent = 0;
  MLOG_ALWAYS1 ("RTAA : %d elements", num_aa);

  for (i = 0; i < num_aa; ++i, rt_aa++)
    {
    rt = rt_aa->rt;
    comp_name = ms_comp_name_find (rt);
    if (strlen (comp_name))
      {
      MLOG_CALWAYS3 ("%sNamed comp  %s: %s", 
					sp[indent], 
                                        sp2[indent],
					comp_name);
      }

    switch (rt->el_tag)
      {
      case RT_ARR_START :
	MLOG_CALWAYS5 ("%sArray start %s: size %02d, %02d elmnts, %02d rt", 
                                       sp[indent],
                                       sp2[indent],
                                       rt_aa->el_size,
                                       rt->u.arr.num_elmnts,
                                       rt->u.arr.num_rt_blks);
        if (++indent >= MAX_INDENT)
          indent--;
      break;

      case RT_ARR_END :
        if (--indent < 0)
          indent++;
	MLOG_CALWAYS5 ("%sArray end   %s: size %02d, %02d elmnts, %02d rt", 
                                       sp[indent],
                                       sp2[indent],
                                       rt_aa->el_size,
                                       rt->u.arr.num_elmnts,
                                       rt->u.arr.num_rt_blks);
      break;

      case RT_STR_START :
	MLOG_CALWAYS3 ("%sStruct start%s: size %02d", 
				       	sp[indent],
                                       	sp2[indent],
					rt_aa->el_size);
        if (++indent >= MAX_INDENT)
          indent--;
      break;

      case RT_STR_END :
        if (--indent < 0)
          indent++;
	MLOG_CALWAYS3 ("%sStruct end  %s: size %02d", 
				       	sp[indent],
                                       	sp2[indent],
					rt_aa->el_size);
      break;

      case RT_BOOL :
	MLOG_CALWAYS5 ("%sBoolean     %s: size %02d, precision %02d, offset %d",
                                   sp[indent],
                                   sp2[indent],
                                   rt_aa->el_size,
                                   rt->u.p.el_len,
				   rt_aa->offset_to_data); 
      break;

      case RT_BIT_STRING :
	MLOG_CALWAYS5 ("%sBitstring   %s: size %02d, precision %02d, offset %d",
                                   sp[indent],
                                   sp2[indent],
                                   rt_aa->el_size,
                                   rt->u.p.el_len,
				   rt_aa->offset_to_data); 
      break;

      case RT_INTEGER : 
	MLOG_CALWAYS5 ("%sInteger     %s: size %02d, precision %02d, offset %d",
                                   sp[indent],
                                   sp2[indent],
                                   rt_aa->el_size,
                                   rt->u.p.el_len,
				   rt_aa->offset_to_data); 
      break;

      case RT_UNSIGNED : 
	MLOG_CALWAYS5 ("%sUnsigned    %s: size %02d, precision %02d, offset %d",
                                   sp[indent],
                                   sp2[indent],
                                   rt_aa->el_size,
                                   rt->u.p.el_len,
				   rt_aa->offset_to_data); 
      break;

      case RT_FLOATING_POINT :
	MLOG_CALWAYS5 ("%sFloat       %s: size %02d, precision %02d, offset %d",
                                   sp[indent],
                                   sp2[indent],
                                   rt_aa->el_size,
                                   rt->u.p.el_len,
				   rt_aa->offset_to_data); 
      break;

      case RT_OCTET_STRING :
	MLOG_CALWAYS5  ("%sOctet Str  %s: size %02d, precision %02d, offset %d",
                                   sp[indent],
                                   sp2[indent],
                                   rt_aa->el_size,
                                   rt->u.p.el_len,
				   rt_aa->offset_to_data); 
      break;

      case RT_VISIBLE_STRING :
	MLOG_CALWAYS5 ("%sVisible Str %s: size %02d, precision %02d, offset %d",
                                   sp[indent],
                                   sp2[indent],
                                   rt_aa->el_size,
                                   rt->u.p.el_len,
				   rt_aa->offset_to_data); 
      break;

      case RT_GENERAL_TIME :
	MLOG_CALWAYS5 ("%sGen Time    %s: size %02d, precision %02d, offset %d",
                                   sp[indent],
                                   sp2[indent],
                                   rt_aa->el_size,
                                   rt->u.p.el_len,
				   rt_aa->offset_to_data); 
      break;

      case RT_BINARY_TIME :
	MLOG_CALWAYS5 ("%sBin Time    %s: size %02d, precision %02d, offset %d",
                                   sp[indent],
                                   sp2[indent],
                                   rt_aa->el_size,
                                   rt->u.p.el_len,
				   rt_aa->offset_to_data); 
      break;

      case RT_BCD :
	MLOG_CALWAYS5 ("%sBCD         %s: size %02d, precision %02d, offset %d",
                                   sp[indent],
                                   sp2[indent],
                                   rt_aa->el_size,
                                   rt->u.p.el_len,
				   rt_aa->offset_to_data); 
      break;

      case RT_UTC_TIME :
	MLOG_CALWAYS5 ("%sUtc Time    %s: size %02d, precision %02d, offset %d",
                                   sp[indent],
                                   sp2[indent],
                                   rt_aa->el_size,
                                   rt->u.p.el_len,
				   rt_aa->offset_to_data); 
      break;

      case RT_UTF8_STRING :
	MLOG_CALWAYS5 ("%sUTF8 Str %s: size %02d, precision %02d, offset %d",
                                   sp[indent],
                                   sp2[indent],
                                   rt_aa->el_size,
                                   rt->u.p.el_len,
				   rt_aa->offset_to_data); 
      break;

      default : 			/* should not be any other tag	*/
	MLOG_CALWAYS1 (" **** UNKNOWN ELEMENT %02d ",i);
      break;
      }
    }
  S_UNLOCK_COMMON_RESOURCES ();

#endif
  }

