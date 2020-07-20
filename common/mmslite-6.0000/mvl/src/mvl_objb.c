/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	1999 - 2002, All Rights Reserved				*/
/*									*/
/* MODULE NAME : mvl_objb.c						*/
/* PRODUCT(S)  : MMSEASE-LITE						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*	Functions for maintaining arrays of "generic objects"		*/
/*	using "bsearch".						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*	mvl_obj_insert							*/
/*	mvl_obj_delete							*/
/*	mvl_obj_find							*/
/*	mvl_obj_find_key						*/
/*	mvl_vmd_dom_insert						*/
/*	mvl_vmd_var_insert						*/
/*	mvl_vmd_nvl_insert						*/
/*	mvl_vmd_jou_insert						*/
/*	mvl_vmd_dom_delete						*/
/*	mvl_vmd_var_delete						*/
/*	mvl_vmd_nvl_delete						*/
/*	mvl_vmd_jou_delete						*/
/*	mvl_vmd_find_dom						*/
/*	mvl_vmd_find_var						*/
/*	mvl_vmd_find_nvl						*/
/*	mvl_vmd_find_jou						*/
/*	mvl_vmd_dom_find_last						*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev     Comments					*/
/* --------  ---  ------   -------------------------------------------  */
/* 02/01/07  JRB     14    Fix function names in module header.		*/
/* 10/30/06  JRB     13    Add vmd_ctrl arg to several functions & chg	*/
/*			   prefix from "mvl_" to "mvl_vmd_".		*/
/*			   Also add net_info arg to "mvl_vmd_find_*"	*/
/*			   so global _mvl_curr_net_info not needed.	*/
/*			   Move all obj delete functs from mvl_obj.c to here.*/
/* 01/30/06  GLB     12    Integrated porting changes for VMS           */
/* 04/11/05  CRM     11	   Added mvl_dom_find_last			*/
/* 11/11/04  MDE     10    Check for 0 elements before calling bsearch	*/
/* 01/23/02  EJV     09    Added journals to domain:			*/
/*			     mvl_jou_insert - added DOM_SPEC case	*/
/*			     mvl_find_jou -   added DOM_SPEC case	*/
/*			     mvl_sort_objs -  added qsort dom->jou_tbl	*/ 
/* 04/14/00  JRB     08    Move sort & compare code FROM mvl_obj.c to	*/
/*			   here and make it ANSI compatible.		*/
/* 04/10/00  JRB     07    Fix usage of bsearch to be ANSI compatible.	*/
/* 09/15/99  JRB     06    Move mvl_obj_nosort here for DOS link	*/
/* 09/13/99  MDE     05    Added SD_CONST modifiers			*/
/* 09/07/99  MDE     04    Changed MVL_VA_SCOPE to MVL_SCOPE, added	*/
/*			   scope to MVL_NVLIST_CTRL			*/
/* 04/07/99  MDE     03    Logging improvements				*/
/* 03/18/99  MDE     02    Enhanced dynamic object facilities		*/
/* 01/15/99  JRB     01    Created					*/
/************************************************************************/
#include "glbtypes.h"
#include "sysincs.h"
#include "mvl_defs.h"
#include "mvl_log.h"


/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/
/************************************************************************/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

ST_BOOLEAN mvl_obj_nosort;

/************************************************************************/
/* Local functions.							*/
/************************************************************************/
int _mvl_obj_compare (const void *obj1, const void *obj2);

/************************************************************************/
/* Static variables.							*/
/************************************************************************/
/* NONE	*/

/************************************************************************/
/*			mvl_obj_insert					*/
/* NOTE: newElement should point to a "real" structure that has already	*/
/*       been allocated. This allows this function to work without	*/
/*       caring about the size of the "real" structure.			*/
/*       This function just adds the ptr to the array of ptrs.		*/
/************************************************************************/

ST_RET mvl_obj_insert (MVL_OBJ **elementTable, 
			ST_INT *numElement,
			ST_INT maxElement,
			MVL_OBJ *newElement)
  {
MVL_OBJ **key;

  if (*numElement >= maxElement)
    {
    MVL_LOG_ERR2 ("ERROR inserting '%s' object. Max num %d exceeded.",
                  newElement->name, maxElement);
    return (SD_FAILURE);		/* No more room in array.	*/
    }

/* Check to see if we are bulk updating, and put at end if so */
  if (mvl_obj_nosort  ||  (*numElement == 0))
    {
    elementTable [(*numElement)++] = newElement;	/* Add to end of array	*/
    }
  else
    {
    /* Make sure it's not already in array. Can't have duplicates.	*/
    if (*numElement)
      key = (MVL_OBJ **) bsearch (&newElement, elementTable, *numElement,
                 sizeof (MVL_OBJ *), _mvl_obj_compare);
    else
      key = NULL;

    if (key != NULL)
      {
      MVL_LOG_ERR1 ("ERROR inserting '%s' object. Duplicate.",
                  newElement->name);
      return (SD_FAILURE);		/* Already in array.		*/
      }
    /* Add to array and resort.	*/
    elementTable [(*numElement)++] = newElement;	/* Add to end of array	*/
    qsort (elementTable, *numElement, sizeof (MVL_OBJ *), _mvl_obj_compare);
    }

  return (SD_SUCCESS);
  }


/************************************************************************/
/*			mvl_obj_delete					*/
/************************************************************************/

MVL_OBJ *mvl_obj_delete (MVL_OBJ **elementTable, 
		    		ST_INT *numElement, 
	            		ST_CHAR *itemName)
  {
MVL_OBJ *el;
MVL_OBJ **foundElement;
MVL_OBJ **destElement;
MVL_OBJ **endElement;

/* Locate the element of interest. We get a pointer into the array of	*/
/* element pointers if it is found.					*/

  foundElement = mvl_obj_find_key (elementTable, *numElement, itemName);
  if (!foundElement)
    return (NULL);
  el = *foundElement;

/* OK, now just move up the remaining pointers		*/
/* Array was sorted before, so it will still be sorted.	*/
  destElement = foundElement;
  endElement = elementTable + *numElement;
  while (++destElement < endElement)
    *(destElement-1) = *destElement;

  --(*numElement);

/* Return the element for further work					*/
  return (el);
  }

/************************************************************************/
/*			mvl_obj_find					*/
/* This function is used to locate an element that is referenced in a 	*/
/* sorted table of element pointers. 					*/
/************************************************************************/

MVL_OBJ *mvl_obj_find (MVL_OBJ **elementTable, 
		    		ST_INT numElement, 
	            		ST_CHAR *itemName)
  {
MVL_OBJ **key;

  /* First find just the "key".				*/
  key = mvl_obj_find_key (elementTable, numElement, itemName);

  /* If key != NULL, return "*key", else return NULL.	*/
  if (key)		/* "key" is pointer to pointer to structure.	*/
    return (*key);	/* "*key" is pointer to structure.		*/

  return (NULL);	/* Not found!!	*/
  }

/************************************************************************/
/*			mvl_obj_find_key				*/
/* This function is used to locate an element that is referenced in a 	*/
/* sorted table of element pointers. 					*/
/*    This function only finds the key. Convenient for delete function.	*/
/************************************************************************/

MVL_OBJ **mvl_obj_find_key (MVL_OBJ **elementTable, 
		    		ST_INT numElement, 
	            		ST_CHAR *itemName)
  {
MVL_OBJ **key;
MVL_OBJ TmpObj;		/* Temporary object to search for.	*/
MVL_OBJ *pTmpObj;	/* ptr to temp object.	*/

  if (numElement == 0)
    return (NULL);

/* Create a element from the itemName, such that a pointer to the 	*/
/* element pointer can be passed to the search function.		*/
  TmpObj.name = itemName;
  pTmpObj = &TmpObj;

  if (numElement) 
    key = (MVL_OBJ **) bsearch (&pTmpObj, elementTable, numElement, 
		 sizeof (MVL_OBJ *), _mvl_obj_compare);
  else
    key = NULL;

  return (key);
  }

/************************************************************************/
/*			mvl_vmd_dom_insert				*/
/************************************************************************/
ST_RET mvl_vmd_dom_insert (MVL_VMD_CTRL *vmd_ctrl, MVL_DOM_CTRL *dom)
  {
/* Domain can only be VMD_SPEC, so this one is easy.			*/
  return (mvl_obj_insert ((MVL_OBJ **) vmd_ctrl->dom_tbl,
                       &vmd_ctrl->num_dom, vmd_ctrl->max_num_dom,
                       (MVL_OBJ *) dom));
  }

/************************************************************************/
/*			mvl_vmd_var_insert				*/
/************************************************************************/
ST_RET mvl_vmd_var_insert (MVL_VMD_CTRL *vmd_ctrl, MVL_VAR_ASSOC *va, OBJECT_NAME *obj,
                       MVL_NET_INFO *net_info)	/* Only used if AA_SPEC	*/
  {
ST_RET retcode = SD_FAILURE;
MVL_DOM_CTRL *dom;
MVL_AA_OBJ_CTRL *aa_objs;

  switch (obj->object_tag)
    {
    case VMD_SPEC:
      retcode = mvl_obj_insert ((MVL_OBJ **) vmd_ctrl->var_assoc_tbl,
                       &vmd_ctrl->num_var_assoc, vmd_ctrl->max_num_var_assoc,
                       (MVL_OBJ *) va);
      break;

    case DOM_SPEC:
      if ((dom = mvl_vmd_find_dom (vmd_ctrl, obj->domain_id)))
        retcode = mvl_obj_insert ((MVL_OBJ **) dom->var_assoc_tbl,
                       &dom->num_var_assoc, dom->max_num_var_assoc,
                       (MVL_OBJ *) va);
      else
        {
        MVL_LOG_NERR1 ("Insert var: Domain '%s' not found", obj->domain_id);
	}
      break;

    case AA_SPEC:
      if (net_info)
        {
        aa_objs = net_info->aa_objs;
        retcode = mvl_obj_insert ((MVL_OBJ **) aa_objs->var_assoc_tbl,
                       &aa_objs->num_var_assoc, aa_objs->max_num_var_assoc,
                       (MVL_OBJ *) va);
        }
      else
        MVL_LOG_ERR0 ("Association not known. Cannot insert Application Association-specific variable.");
      break;

    default:
      MVL_LOG_ERR1 ("Illegal scope for '%s'", obj->obj_name.vmd_spec);
      break;
    }
  return (retcode);
  }

/************************************************************************/
/*			mvl_vmd_nvl_insert				*/
/************************************************************************/
ST_RET mvl_vmd_nvl_insert (MVL_VMD_CTRL *vmd_ctrl, MVL_NVLIST_CTRL *nvl, OBJECT_NAME *obj,
                       MVL_NET_INFO *net_info)	/* Only used if AA_SPEC	*/
  {
ST_RET retcode = SD_FAILURE;
MVL_DOM_CTRL *dom;
MVL_AA_OBJ_CTRL *aa_objs;

  switch (obj->object_tag)
    {
    case VMD_SPEC:
      retcode = mvl_obj_insert ((MVL_OBJ **) vmd_ctrl->nvlist_tbl,
                       &vmd_ctrl->num_nvlist, vmd_ctrl->max_num_nvlist,
                       (MVL_OBJ *) nvl);
      nvl->nvl_scope.scope = VMD_SPEC;
    break;

    case DOM_SPEC:
      if ((dom = mvl_vmd_find_dom (vmd_ctrl, obj->domain_id)))
        retcode = mvl_obj_insert ((MVL_OBJ **) dom->nvlist_tbl,
                       &dom->num_nvlist, dom->max_num_nvlist,
                       (MVL_OBJ *) nvl);
      else
        {
        MVL_LOG_NERR1 ("Insert NVL: Domain '%s' not found", obj->domain_id);
	}
      nvl->nvl_scope.scope = DOM_SPEC;
      nvl->nvl_scope.dom = dom;
    break;

    case AA_SPEC:
      if (net_info)
        {
        aa_objs = net_info->aa_objs;
        retcode = mvl_obj_insert ((MVL_OBJ **) aa_objs->nvlist_tbl,
                       &aa_objs->num_nvlist, aa_objs->max_num_nvlist,
                       (MVL_OBJ *) nvl);
        nvl->nvl_scope.scope = AA_SPEC;
        }
      else
        MVL_LOG_ERR0 ("Association not known. Cannot insert Application Association-specific NVL.");
    break;

    default:
      MVL_LOG_ERR1 ("Illegal scope for '%s'", obj->obj_name.vmd_spec);
      break;
    }
  return (retcode);
  }

/************************************************************************/
/*			mvl_vmd_jou_insert				*/
/************************************************************************/
ST_RET mvl_vmd_jou_insert (MVL_VMD_CTRL *vmd_ctrl, MVL_JOURNAL_CTRL *jou, OBJECT_NAME *obj,
                       MVL_NET_INFO *net_info)	/* Only used if AA_SPEC	*/
  {
ST_RET retcode = SD_FAILURE;
MVL_AA_OBJ_CTRL *aa_objs;
MVL_DOM_CTRL *dom;

  switch (obj->object_tag)
    {
    case VMD_SPEC:
      retcode = mvl_obj_insert ((MVL_OBJ **) vmd_ctrl->jou_tbl,
                       &vmd_ctrl->num_jou, vmd_ctrl->max_num_jou,
                       (MVL_OBJ *) jou);
      break;

    case AA_SPEC:
      if (net_info)
        {
        aa_objs = net_info->aa_objs;
        retcode = mvl_obj_insert ((MVL_OBJ **) aa_objs->jou_tbl,
                       &aa_objs->num_jou, aa_objs->max_num_jou,
                       (MVL_OBJ *) jou);
        }
      else
        MVL_LOG_ERR0 ("Association not known. Cannot insert Application Association-specific journal.");
      break;

    case DOM_SPEC:
      dom = mvl_vmd_find_dom (vmd_ctrl, obj->domain_id);
      if (dom)
        retcode = mvl_obj_insert ((MVL_OBJ **) dom->jou_tbl,
                         &dom->num_jou, dom->max_num_jou, (MVL_OBJ *) jou);
      else
        {
        MVL_LOG_NERR1 ("Insert journal: Domain '%s' not found", obj->domain_id);
	}
      break;

    default:
      /* NOTE: DOM_SPEC illegal for journals.	*/
      MVL_LOG_ERR1 ("Illegal scope for '%s'", obj->obj_name.vmd_spec);
      break;
    }
  return (retcode);
  }

/************************************************************************/
/*			mvl_vmd_dom_delete 				*/
/************************************************************************/
MVL_DOM_CTRL *mvl_vmd_dom_delete (MVL_VMD_CTRL *vmd_ctrl, ST_CHAR *dom_name)
  {
MVL_DOM_CTRL *dom;

  dom = (MVL_DOM_CTRL *) mvl_obj_delete ((MVL_OBJ **) vmd_ctrl->dom_tbl,
                                    &vmd_ctrl->num_dom, dom_name);
  return (dom);
  }


/************************************************************************/
/*			mvl_vmd_var_delete 				*/
/************************************************************************/
MVL_VAR_ASSOC *mvl_vmd_var_delete (MVL_VMD_CTRL *vmd_ctrl, OBJECT_NAME *obj,
	MVL_NET_INFO *net_info)	/* Only used if AA_SPEC	*/
  {
MVL_VAR_ASSOC *va;
MVL_DOM_CTRL *dom;
MVL_AA_OBJ_CTRL *aa_objs;
ST_CHAR *name;

  va = NULL;
  name = obj->obj_name.vmd_spec;	/* We know this is a union ...	*/
  switch (obj->object_tag)
    {
    case VMD_SPEC:
      va = (MVL_VAR_ASSOC *) mvl_obj_delete ((MVL_OBJ **) vmd_ctrl->var_assoc_tbl,
                                    &vmd_ctrl->num_var_assoc, name);
      break;

    case DOM_SPEC:
      if ((dom = mvl_vmd_find_dom (vmd_ctrl, obj->domain_id)))
        va = (MVL_VAR_ASSOC *) mvl_obj_delete ((MVL_OBJ **) dom->var_assoc_tbl,
                                    &dom->num_var_assoc, name);
      else
        {
        MVL_LOG_NERR1 ("Delete var: Domain '%s' not found", obj->domain_id);
	}
      break;

    case AA_SPEC:
      if (net_info)
        {
        aa_objs = net_info->aa_objs;
        va = (MVL_VAR_ASSOC *) mvl_obj_delete ((MVL_OBJ **) aa_objs->var_assoc_tbl,
                                    &aa_objs->num_var_assoc, name);
        }
      else
        MVL_LOG_ERR0 ("Association not known. Cannot delete Application Association-specific variable.");
      break;

    default:
      MVL_LOG_ERR1 ("Illegal scope for '%s'", name);
      break;
    }
  return (va);
  }

/************************************************************************/
/*			mvl_vmd_nvl_delete				*/
/************************************************************************/
MVL_NVLIST_CTRL *mvl_vmd_nvl_delete (MVL_VMD_CTRL *vmd_ctrl, OBJECT_NAME *obj,
                       MVL_NET_INFO *net_info)	/* Only used if AA_SPEC	*/
  {
MVL_NVLIST_CTRL *nvl = NULL;
MVL_DOM_CTRL *dom;
MVL_AA_OBJ_CTRL *aa_objs;
ST_CHAR *name;

  name = obj->obj_name.vmd_spec;	/* We know this is a union ...	*/

  switch (obj->object_tag)
    {
    case VMD_SPEC:
      nvl = (MVL_NVLIST_CTRL *) mvl_obj_delete ((MVL_OBJ **) vmd_ctrl->nvlist_tbl,
                                    &vmd_ctrl->num_nvlist, name);
      break;

    case DOM_SPEC:
      if ((dom = mvl_vmd_find_dom (vmd_ctrl, obj->domain_id)))
        nvl = (MVL_NVLIST_CTRL *) mvl_obj_delete ((MVL_OBJ **) dom->nvlist_tbl,
                                    &dom->num_nvlist, name);
      else
        {
        MVL_LOG_NERR1 ("Delete NVL: Domain '%s' not found", obj->domain_id);
	}
      break;

    case AA_SPEC:
      if (net_info)
        {
        aa_objs = net_info->aa_objs;
        nvl = (MVL_NVLIST_CTRL *) mvl_obj_delete ((MVL_OBJ **) aa_objs->nvlist_tbl,
                                    &aa_objs->num_nvlist, name);
        }
      else
        MVL_LOG_ERR0 ("Association not known. Cannot delete Application Association-specific NVL.");
      break;

    default:
      MVL_LOG_ERR1 ("Illegal scope for '%s'", name);
      break;
    }
  return (nvl);
  }

/************************************************************************/
/*			mvl_vmd_jou_delete 				*/
/************************************************************************/
MVL_JOURNAL_CTRL *mvl_vmd_jou_delete (MVL_VMD_CTRL *vmd_ctrl, OBJECT_NAME *obj,
	MVL_NET_INFO *net_info)	/* Only used if AA_SPEC	*/
  {
MVL_JOURNAL_CTRL *jou;
MVL_AA_OBJ_CTRL *aa_objs;
MVL_DOM_CTRL *dom;
ST_CHAR *name;

  jou = NULL;
  name = obj->obj_name.vmd_spec;	/* We know this is a union ...	*/
  switch (obj->object_tag)
    {
    case VMD_SPEC:
      jou = (MVL_JOURNAL_CTRL *) mvl_obj_delete ((MVL_OBJ **) vmd_ctrl->jou_tbl,
                                    &vmd_ctrl->num_jou, name);
      break;

    case AA_SPEC:
      if (net_info)
        {
        aa_objs = net_info->aa_objs;
        jou = (MVL_JOURNAL_CTRL *) mvl_obj_delete ((MVL_OBJ **) aa_objs->jou_tbl,
                                    &aa_objs->num_jou, name);
        }
      else
        MVL_LOG_ERR0 ("Association not known. Cannot delete Application Association-specific journal.");
      break;

    case DOM_SPEC:
      dom = mvl_vmd_find_dom (vmd_ctrl, obj->domain_id);
      if (dom)
        jou = (MVL_JOURNAL_CTRL *) mvl_obj_delete ((MVL_OBJ **) dom->jou_tbl,
                                    &dom->num_jou, name);
      else
        {
        MVL_LOG_NERR1 ("Delete journal: Domain '%s' not found", obj->domain_id);
	}
      break;

    default:
      MVL_LOG_ERR1 ("Illegal scope for '%s'", name);
      break;
    }
  return (jou);
  }

/************************************************************************/
/*			mvl_vmd_find_dom				*/
/************************************************************************/

MVL_DOM_CTRL *mvl_vmd_find_dom (MVL_VMD_CTRL *vmd_ctrl, ST_CHAR *name)
  {
MVL_DOM_CTRL *result;

  result = (MVL_DOM_CTRL *) mvl_obj_find ((MVL_OBJ **) vmd_ctrl->dom_tbl,
                       vmd_ctrl->num_dom, name);
  return(result);
  }

/************************************************************************/
/*			mvl_vmd_dom_find_last				*/
/* Find the last domain.						*/
/************************************************************************/
MVL_DOM_CTRL *mvl_vmd_dom_find_last (MVL_VMD_CTRL *vmd_ctrl)
  {
MVL_DOM_CTRL *dom;
  
  if (vmd_ctrl->num_dom > 0)
    dom = vmd_ctrl->dom_tbl [vmd_ctrl->num_dom-1];
  else
    dom = NULL;	/* no more dynamically created domains.	*/
  return (dom);
  }

/************************************************************************/
/*			mvl_vmd_find_var				*/
/* This function finds the 'variable association' for a particular var	*/
/* name.								*/
/************************************************************************/

MVL_VAR_ASSOC *mvl_vmd_find_var (MVL_VMD_CTRL *vmd_ctrl,
	OBJECT_NAME *obj,
	MVL_NET_INFO *net_info)	/* Only used if AA_SPEC	*/
  {
MVL_DOM_CTRL *dom;
MVL_AA_OBJ_CTRL *aa;
ST_CHAR *name;

  name = obj->obj_name.vmd_spec;	/* We know this is a union ...	*/

  switch (obj->object_tag)
    {
    case VMD_SPEC :
      return ((MVL_VAR_ASSOC *) mvl_obj_find ((MVL_OBJ **) vmd_ctrl->var_assoc_tbl,
                       vmd_ctrl->num_var_assoc, name));
    case DOM_SPEC :
      dom = mvl_vmd_find_dom (vmd_ctrl, obj->domain_id);
      if (dom)
        return ((MVL_VAR_ASSOC *) mvl_obj_find ((MVL_OBJ **) dom->var_assoc_tbl,
                       dom->num_var_assoc, name));
      else
        {
        MVL_LOG_NERR1 ("Find Va: Domain '%s' not found", obj->domain_id);
	}
    break;

    case AA_SPEC :
      if (net_info)
        {
        aa = (MVL_AA_OBJ_CTRL *) net_info->aa_objs;
        return ((MVL_VAR_ASSOC *) mvl_obj_find ((MVL_OBJ **) aa->var_assoc_tbl,
                       aa->num_var_assoc, name));
        }
      else
        MVL_LOG_ERR0 ("Association not known. Cannot find Application Association-specific variable.");
    break;

    default:
      MVL_LOG_ERR1 ("Illegal scope for '%s'", name);
    break;
    }
  return (NULL);
  }

/************************************************************************/
/*			mvl_vmd_find_nvl				*/
/************************************************************************/

MVL_NVLIST_CTRL *mvl_vmd_find_nvl (MVL_VMD_CTRL *vmd_ctrl,
	OBJECT_NAME *obj,
	MVL_NET_INFO *net_info)	/* Only used if AA_SPEC	*/
  {
MVL_DOM_CTRL *dom;
MVL_AA_OBJ_CTRL *aa;
ST_CHAR *name;

/* Get the NVL name. We know this is a union ...	*/
  name = obj->obj_name.vmd_spec;

  switch (obj->object_tag)
    {
    case VMD_SPEC :
      return ((MVL_NVLIST_CTRL *) mvl_obj_find ((MVL_OBJ **) vmd_ctrl->nvlist_tbl,
                       vmd_ctrl->num_nvlist, name));
    case DOM_SPEC :
      dom = mvl_vmd_find_dom (vmd_ctrl, obj->domain_id);
      if (dom)
        return ((MVL_NVLIST_CTRL *) mvl_obj_find ((MVL_OBJ **) dom->nvlist_tbl,
                       dom->num_nvlist, name));
      else
        {
        MVL_LOG_NERR1 ("Find NVL: Domain '%s' not found", obj->domain_id);
	}
    break;

    case AA_SPEC :
      if (net_info)
        {
        aa = (MVL_AA_OBJ_CTRL *) net_info->aa_objs;
        return ((MVL_NVLIST_CTRL *) mvl_obj_find ((MVL_OBJ **) aa->nvlist_tbl,
                       aa->num_nvlist, name));
        }
      else
        MVL_LOG_ERR0 ("Association not known. Cannot find Application Association-specific NVL.");
    break;

    default:
      MVL_LOG_ERR1 ("Illegal scope for '%s'", name);
    break;
    }
  return (NULL);
  }

/************************************************************************/
/*			mvl_vmd_find_jou				*/
/* This function finds the 'journal control' corresponding to 'obj'.	*/
/************************************************************************/

MVL_JOURNAL_CTRL *mvl_vmd_find_jou (MVL_VMD_CTRL *vmd_ctrl,
	OBJECT_NAME *obj,
	MVL_NET_INFO *net_info)	/* Only used if AA_SPEC	*/
  {
MVL_AA_OBJ_CTRL *aa_objs;
MVL_DOM_CTRL *dom;
ST_CHAR *name;

  name = obj->obj_name.vmd_spec;	/* We know this is a union ...	*/

  switch (obj->object_tag)
    {
    case VMD_SPEC :
      return ((MVL_JOURNAL_CTRL *) mvl_obj_find ((MVL_OBJ **) vmd_ctrl->jou_tbl,
                       vmd_ctrl->num_jou, name));
    break;

    case AA_SPEC :
      if (net_info)
        {
        aa_objs = (MVL_AA_OBJ_CTRL *) net_info->aa_objs;
        return ((MVL_JOURNAL_CTRL *) mvl_obj_find ((MVL_OBJ **) aa_objs->jou_tbl,
                       aa_objs->num_jou, name));
        }
      else
        MVL_LOG_ERR0 ("Association not known. Cannot find Application Association-specific journal.");
    break;

    case DOM_SPEC :
      dom = mvl_vmd_find_dom (vmd_ctrl, obj->domain_id);
      if (dom)
        return ((MVL_JOURNAL_CTRL *) mvl_obj_find ((MVL_OBJ **) dom->jou_tbl,
                       dom->num_jou, name));
      else
        {
        MVL_LOG_NERR1 ("Find journal: Domain '%s' not found", obj->domain_id);
	}
    break;

    default:
      /* NOTE: DOM_SPEC is illegal for Journals.			*/
      MVL_LOG_ERR1 ("Illegal scope for '%s'", name);
    break;
    }
  return (NULL);
  }

/************************************************************************/
/*			mvl_sort_objs					*/
/************************************************************************/
ST_VOID mvl_sort_objs (ST_VOID)  
  {
ST_INT i;
MVL_DOM_CTRL *dom;

  mvl_obj_nosort = SD_FALSE;

  qsort (mvl_vmd.dom_tbl, mvl_vmd.num_dom, sizeof (MVL_OBJ *), _mvl_obj_compare);
  qsort (mvl_vmd.var_assoc_tbl, mvl_vmd.num_var_assoc, sizeof (MVL_OBJ *), _mvl_obj_compare);
  qsort (mvl_vmd.nvlist_tbl, mvl_vmd.num_nvlist, sizeof (MVL_OBJ *), _mvl_obj_compare);
  qsort (mvl_vmd.jou_tbl, mvl_vmd.num_jou, sizeof (MVL_OBJ *), _mvl_obj_compare);

  for (i = 0; i < mvl_vmd.num_dom; ++i)
    {
    dom = mvl_vmd.dom_tbl[i];
    qsort (dom->var_assoc_tbl, dom->num_var_assoc, sizeof (MVL_OBJ *), _mvl_obj_compare);
    qsort (dom->nvlist_tbl, dom->num_nvlist, sizeof (MVL_OBJ *), _mvl_obj_compare);
    qsort (dom->jou_tbl, dom->num_jou, sizeof (MVL_OBJ *), _mvl_obj_compare);
    }
  }

/************************************************************************/
/*			_mvl_obj_compare				*/
/************************************************************************/
int _mvl_obj_compare (const void *obj1, const void *obj2)
  {
const MVL_OBJ **mvl_obj1 = (const MVL_OBJ **) obj1;
const MVL_OBJ **mvl_obj2 = (const MVL_OBJ **) obj2;
  return (strcmp ((*mvl_obj1)->name, (*mvl_obj2)->name));
  }
