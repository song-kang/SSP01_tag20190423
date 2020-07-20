/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	1994 - 1996, All Rights Reserved		        */
/*									*/
/* MODULE NAME : mvl_type.c    						*/
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
/* 07/11/07  JRB     17    mvlu_free_rt_type: Reset structure to all 0.	*/
/* 02/07/06  JRB     16    Log if max types exceeded.			*/
/* 03/14/05  CRM     15    Add mvl_max_dyn global struct & replace	*/
/*			   _mvlu_num_dyn_types with "mvl_max_dyn.types".*/
/* 09/18/03  JRB     14    mvl_get_runtime: chk for valid num_rt.	*/
/* 03/13/03  JRB     13    Add mvl_type_ctrl_find_dyn.			*/
/*			   Del mvlu_add_rt_type_x (not needed).		*/
/* 11/27/02  MDE     12    Added mvlu_add_rt_type_x,			*/
/* 			   mvl_typeid_to_typename, 			*/
/*			   mvl_typename_to_typeid			*/
/* 10/28/02  JRB     11    Del mvl_mk_rt_from_asn1, use ms_runtime_create*/
/* 10/06/00  MDE     10    Added _mvlu_num_dyn_types			*/
/* 09/05/00  JRB     09    Move mvl_type_id_* to new module.		*/
/*			   Add mvl_get_runtime_dyn.			*/
/*			   mvl_type_ctrl_find chk for "in use".		*/
/* 07/13/00  JRB     08    Add Dynamic type creation functions:		*/
/*			     mvl_type_id_create, mvl_type_id_destroy,	*/
/*			     mvl_type_ctrl_find.			*/
/*			   Move these functs from mvl_uca.c to here:	*/
/*			     mvlu_add_rt_type, mvlu_free_rt_type.	*/
/* 04/14/00  JRB     07    Del SD_CONST from mvl_get_runtime.		*/
/* 01/21/00  MDE     06    Now use MEM_SMEM for dynamic memory		*/
/* 09/13/99  MDE     05    Added SD_CONST modifiers			*/
/* 11/16/98  MDE     04    Renamed internal functions (prefix '_')	*/
/* 09/21/98  MDE     03    Minor lint cleanup				*/
/* 06/15/98  MDE     02    Changes to allow compile under C++		*/
/* 09/10/97  MDE     01    MMS-LITE V4.0 Release			*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"

#include "mmsdefs.h"
#include "mms_pvar.h"
#include "mms_vvar.h"
#include "mvl_defs.h"
#include "mvl_log.h"


/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/
/************************************************************************/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/
/* The user may set this to a positive value prior to calling 		*/
/* 'mvl_init_mms_objs' to control the number allowed 			*/

MVL_MAX_DYN mvl_max_dyn;

/************************************************************************/
/************************************************************************/
/*			mvl_mod_arr_size				*/
/* TYPE HANDLING							*/
/* This function modifies the size of an array runtime element		*/
/************************************************************************/

ST_VOID mvl_mod_arr_size (RUNTIME_TYPE *rt, ST_RTINT num_elmnts)
  {
ST_INT size_per_el;

/* Adjust the total size of the type, using scaling 			*/
  size_per_el = rt->offset_to_last/rt->u.arr.num_elmnts;
  rt->offset_to_last = size_per_el * num_elmnts;

/* Now set the number of elements as desired				*/
  rt->u.arr.num_elmnts = num_elmnts;
  rt += (rt->u.arr.num_rt_blks + 1);
  rt->u.arr.num_elmnts = num_elmnts;
  }

/************************************************************************/
/*			mvl_get_runtime					*/
/* This function returns runtime type information given the type id	*/
/************************************************************************/

ST_RET mvl_get_runtime (ST_INT type_id, 
                       RUNTIME_TYPE **rt_out,
                       ST_INT *num_rt_out)
  {
MVL_TYPE_CTRL *type_ctrl = &mvl_type_ctrl[type_id];	/* ptr to array elem*/

  if (type_id >= 0 &&
      type_id < mvl_num_types &&
      type_ctrl->num_rt > 0)
    {
    /* Cast "rt" to remove SD_CONST.	*/
    *rt_out = (RUNTIME_TYPE *) type_ctrl->rt;
    *num_rt_out = type_ctrl->num_rt;
    return (SD_SUCCESS);
    }
  return (MVL_ERR_RUNTIME_TYPE_ID);
  }

/************************************************************************/
/*			mvl_get_runtime_dyn				*/
/* This function returns runtime type information given the type id.	*/
/* It is the same as "mvl_get_runtime" except it only SUCCEEDS if the	*/
/* type was dynamically created.					*/
/************************************************************************/

ST_RET mvl_get_runtime_dyn (ST_INT type_id, 
                       RUNTIME_TYPE **rt_out,
                       ST_INT *num_rt_out)
  {
MVL_TYPE_CTRL *type_ctrl = &mvl_type_ctrl[type_id];	/* ptr to array elem*/

  if (type_id >= mvl_num_types - mvl_max_dyn.types &&	/* dynamic type	*/
      type_id < mvl_num_types &&
      type_ctrl->num_rt > 0)
    {
    /* Cast "rt" to remove SD_CONST.	*/
    *rt_out = (RUNTIME_TYPE *) type_ctrl->rt;
    *num_rt_out = type_ctrl->num_rt;
    return (SD_SUCCESS);
    }
  return (MVL_ERR_RUNTIME_TYPE_ID);
  }

/************************************************************************/
/*			mvl_type_ctrl_find				*/
/* Find the type ctrl structure corresponding to the type id.		*/
/* RETURNS: ptr to type ctrl structure (NULL on error)			*/
/* NOTE: Use "mvl_get_runtime*" if only the runtime type is needed.	*/
/************************************************************************/
MVL_TYPE_CTRL *mvl_type_ctrl_find (ST_INT TypeId)
  {
MVL_TYPE_CTRL *type_ctrl;

  if (TypeId < 0  ||
      TypeId >= mvl_num_types  ||
      mvl_type_ctrl[TypeId].num_rt == 0)	/* Array entry not in use.*/
    type_ctrl = NULL;
  else
    type_ctrl = &mvl_type_ctrl [TypeId];
  return (type_ctrl);
  }

/************************************************************************/
/*			mvl_type_ctrl_find_dyn				*/
/* Find the type ctrl structure corresponding to the type id.		*/
/* For "dynamic" types ONLY.						*/
/* RETURNS: ptr to type ctrl structure (NULL on error)			*/
/************************************************************************/
MVL_TYPE_CTRL *mvl_type_ctrl_find_dyn (ST_INT TypeId)
  {
MVL_TYPE_CTRL *type_ctrl;

  if (TypeId < mvl_num_types - mvl_max_dyn.types)
    type_ctrl = NULL;				/* NOT dynamic. Return error*/
  else
    type_ctrl = mvl_type_ctrl_find (TypeId);	/* COULD BE dynamic, find it*/
  return (type_ctrl);
  }


/************************************************************************/
/*			mvl_typeid_to_typename				*/
/************************************************************************/
ST_CHAR *mvl_typeid_to_typename (ST_INT type_id)		    
  {								    
  if (type_id >= 0 && type_id < mvl_num_types)		    
    return (mvl_type_ctrl[type_id].type_name);		    
								    
  return ("unknown");					    
  } 								    

/************************************************************************/
/*			mvl_typename_to_typeid				*/
/************************************************************************/

ST_INT mvl_typename_to_typeid (ST_CHAR *type_id_string)	    
  {								    
ST_INT i;							    
								    
  for (i = 0; i < mvl_num_types; ++i)		  		    
    {								    
    if (mvl_type_ctrl[i].num_rt != 0 &&			    
        mvl_type_ctrl[i].type_name != NULL &&		            
        !strcmp (mvl_type_ctrl[i].type_name, type_id_string))     
      {							    
      return (i);			  			    
      }							    
    }								    
  return (-1);						    
  } 								    


/************************************************************************/
/************************************************************************/
/* MVLU UTILITY FUNCTIONS						*/
/************************************************************************/
/************************************************************************/
/*                       mvlu_add_rt_type				*/
/************************************************************************/
/* This function adds a RT type to the 'mvl_type_ctrl' table and 	*/
/* returns ther type id for the new entry. 				*/

ST_RET mvlu_add_rt_type (RUNTIME_TYPE *rt, ST_INT numRt, ST_INT *typeIdOut)
  {
ST_INT i;
ST_INT mvlu_type_id_base;

  /* Just search for a unused slot */
  mvlu_type_id_base = mvl_num_types - mvl_max_dyn.types;
  for (i = mvlu_type_id_base; i < mvl_num_types; ++i)
    {
    if (mvl_type_ctrl[i].num_rt == 0)
      {
      mvl_type_ctrl[i].rt = rt;
      mvl_type_ctrl[i].num_rt = numRt;
      mvl_type_ctrl[i].data_size = rt->offset_to_last;
      mvl_type_ctrl[i].type_name[0] = '\0';	/* name unknown	*/
      *typeIdOut = i;
      return (SD_SUCCESS);
      }
    }
  MVL_LOG_ERR1 ("Cannot add Runtime Type. Maximum number '%d' exceeded", mvl_num_types);
  return (SD_FAILURE);
  }

/************************************************************************/
/*                       mvlu_free_rt_type			       	*/
/************************************************************************/
/* This function free's slot in the 'mvl_type_ctrl' table, so that it 	*/
/* will be available for future use by mvlu_add_rt_type			*/

ST_VOID mvlu_free_rt_type (ST_INT typeId)
  {
MVL_TYPE_CTRL *type_ctrl;
  if (typeId >= mvl_num_types - mvl_max_dyn.types)
    {
    /* Reset the structure to all 0 so it is ready to be reused.	*/
    /* NOTE: this should be a type created by mvlu_add_rt_type	 	*/
    /*       so rt_ctrl and vmd_ctrl should be NULL.			*/
    /* NOTE: this clears num_rt which indicates unused array entry.	*/
    type_ctrl = &mvl_type_ctrl[typeId];
    memset (type_ctrl, 0, sizeof (MVL_TYPE_CTRL));
    }
  }


