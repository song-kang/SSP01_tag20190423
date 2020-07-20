/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	1998 - 2005, All Rights Reserved		        */
/*									*/
/* MODULE NAME : mvl_obj.c	    					*/
/* PRODUCT(S)  : MMSEASE-LITE						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*	mvl_vmd_dom_add							*/
/*	mvl_vmd_var_add							*/
/*	mvl_vmd_nvl_add							*/
/*	mvl_vmd_jou_add							*/
/*	mvl_vmd_dom_remove						*/
/*	mvl_vmd_var_remove						*/
/*	mvl_vmd_nvl_remove						*/
/*	mvl_vmd_jou_remove						*/
/*	mvl_nvl_create							*/
/*	mvl_nvl_destroy							*/
/*	mvl_vmd_create							*/
/*	mvl_vmd_destroy							*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 01/11/12  JRB	   Add mvl_vmd_nvl_add_aa.			*/
/*			   In mvl_nvl_destroy, free alt access stuff.	*/
/*			   mvl_var_create_derived: chg prim_num from	*/
/*			   ST_RTINT to ST_UINT to support bigger Ed 2 vars.*/
/* 02/02/10  NAV     29    OpenVMS port					*/
/* 02/26/08  JRB     28    mvl_vmd_destroy: do NOT free var->data (do	*/
/*			   not know if it was allocated).		*/
/*			   Check var or NVL is deletable before delete.	*/
/*			   mvl_vmd_dom_remove: remove dom objects first.*/
/*			   mvl_dom_destroy: do NOT remove dom objects.	*/
/*			   Add mvl_var_add_alloc, mvl_var_create_alloc.	*/
/*			   Add mvl_var_create_derived.			*/
/*			   Remove unused functions.			*/
/* 02/21/07  JRB     27    Fix funct names in this module header.	*/
/* 12/04/06  JRB     26    mvl_vmd_destroy: add arg to specify which VMD*/
/*			   and DO NOT destroy all rpt_ctrl (let user).	*/
/* 10/30/06  JRB     25    Add vmd_ctrl arg to several functions & chg	*/
/*			   prefix from "mvl_" to "mvl_vmd_".		*/
/*			   Add vmd_ctrl, net_info args to mvl_nvl_create.*/
/*			   Check that (net_info!=NULL) before using it.	*/
/*			   Move "*_delete" to mvl_objb.c with other	*/
/*			    insert/delete functions.			*/
/*			   Add mvl_vmd_create.				*/
/* 08/09/06  JRB     24    Free "last_data" only in mvl_var_destroy.	*/
/*			   Save type_ctrl in MVL_VAR_ASSOC.		*/
/*			   mvl_vmd_destroy: free mvl61850_rpt_ctrl_list.*/
/* 01/30/06  GLB     23    Integrated porting changes for VMS           */
/* 10/25/05  EJV     22    Fix "last_data" memory leaks: alloc only if	*/
/*			   last_data==NULL, free only if last_data!=NULL.*/
/* 09/22/05  JRB     21    mvl_vmd_destroy: del VMD-spec vars, NVLs too.*/
/* 06/29/05  CRM     20    Add mvl_vmd_destroy.				*/
/* 06/27/05  JRB     19    Alloc "va->last_data" in mvl_nvl_create &	*/
/*			   free it in mvl_nvl_destroy so special NVL	*/
/*			   functs not needed for IEC/UCA reports.	*/
/* 06/27/05  JRB     18	   mvl_var_create: chk type.			*/
/* 04/27/05  CRM     17    Added code to free dom->jou_tbl in           */
/*			   mvl_dom_destroy				*/
/* 06/04/04  JRB     16    mvl_var_create: if (data), set use_static_data.*/
/* 10/03/03  JRB     15    mvl_nvl_add: set nvl_scope in MVL_NVLIST_CTRL*/
/*			   mvl_nvl_add/mvl_var_add: chk for valid dom first*/
/* 02/25/02  MDE     14    Now get max PDU size from mvl_cfg_info	*/
/* 01/23/02  EJV     13    Added journals to domain:			*/
/*			     mvl_init_dom -   added max_num_jou param	*/
/*			     mvl_dom_resize - added max_num_jou param	*/
/*			     mvl_dom_create - added max_num_jou param	*/
/*			     mvl_dom_add -    added max_num_jou param	*/
/*			     mvl_clear_dom_objs -added code for journals*/
/*			     mvl_jou_delete - added DOM_SPEC  case	*/
/* 10/25/00  JRB     12    Del u_mvl funct ptrs. Call functs directly.	*/
/*			   Control with #ifdefs.			*/
/* 04/14/00  JRB     11    Move sort & compare code to mvl_objb.c	*/
/* 04/12/00  MDE     10    Moved AA resize code to mvl_acse.c		*/
/* 01/21/00  MDE     09    Now use MEM_SMEM for dynamic memory		*/
/* 09/15/99  JRB     08    Mv mvl_obj_nosort to mvl_objb.c for DOS link	*/
/* 09/13/99  MDE     07    Added SD_CONST modifiers			*/
/* 09/07/99  MDE     06    Changed MVL_VA_SCOPE to MVL_SCOPE		*/
/* 07/12/99  MDE     05    Fixed problem 'destroying' dom w/NVL or VAR	*/
/* 04/07/99  MDE     04    Logging improvements				*/
/* 03/18/99  MDE     03    Enhanced dynamic object facilities		*/
/* 01/20/99  JRB     02    Reduce # of functions. Chg names and args.	*/
/*			   Call new "mvl_???_insert" functions.		*/
/*			   Add ..nvl_remove, ..nvl_create, ..nvl_destroy*/
/* 11/18/98  MDE     01    Created					*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "mvl_acse.h"
#include "mvl_log.h"
#if defined(MVL_UCA)
#include "mvl_uca.h"	/* need "MVLU_RPT_*"	*/
#endif

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/
/************************************************************************/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

ST_VOID (*u_mvl_dom_destroy) (MVL_DOM_CTRL *dom);
ST_VOID (*u_mvl_var_destroy)(MVL_VAR_ASSOC *va);
ST_VOID (*u_mvl_nvl_destroy) (MVL_NVLIST_CTRL *nvl);
ST_VOID (*u_mvl_jou_destroy) (MVL_JOURNAL_CTRL *jou);

/************************************************************************/
/* SCOPE RESIZING FUNCTIONS 						*/
/************************************************************************/
/************************************************************************/
/*			mvl_vmd_resize 					*/
/************************************************************************/

ST_VOID mvl_vmd_resize (ST_INT max_num_dom, ST_INT max_num_var, 
			ST_INT max_num_nvl, ST_INT max_num_jou)
  {
  if (max_num_dom < mvl_vmd.num_dom)
    {
    MVL_LOG_NERR0 ("Resizing VMD: new max DOM is less than current"); 
    max_num_dom = mvl_vmd.num_dom;
    }
  if (max_num_var < mvl_vmd.num_var_assoc)
    {
    MVL_LOG_NERR0 ("Resizing VMD: new max VA is less than current"); 
    max_num_var = mvl_vmd.num_var_assoc;
    }
  if (max_num_nvl < mvl_vmd.num_nvlist)
    {
    MVL_LOG_NERR0 ("Resizing VMD: new max NVL is less than current"); 
    max_num_nvl = mvl_vmd.num_nvlist;
    }
  if (max_num_jou < mvl_vmd.num_jou)
    {
    MVL_LOG_NERR0 ("Resizing VMD: new max JOU is less than current"); 
    max_num_jou = mvl_vmd.num_jou;
    }

  _mvl_resize_ptr_tbl (&(mvl_vmd.max_num_dom), 
  		         (ST_VOID ***)&(mvl_vmd.dom_tbl), max_num_dom);
  _mvl_resize_ptr_tbl (&(mvl_vmd.max_num_var_assoc), 
  		         (ST_VOID ***)&(mvl_vmd.var_assoc_tbl), max_num_var);
  _mvl_resize_ptr_tbl (&(mvl_vmd.max_num_nvlist), 
  		         (ST_VOID ***)&(mvl_vmd.nvlist_tbl), max_num_nvl);
  _mvl_resize_ptr_tbl (&(mvl_vmd.max_num_jou), 
  		         (ST_VOID ***)&(mvl_vmd.jou_tbl), max_num_jou);
  }

/************************************************************************/
/*			mvl_dom_resize 					*/
/************************************************************************/

ST_VOID mvl_dom_resize (MVL_DOM_CTRL *dom, ST_INT max_num_var, 
			ST_INT max_num_nvl, ST_INT max_num_jou)
  {
  if (max_num_var < dom->num_var_assoc)
    {
    MVL_LOG_NERR0 ("Resizing Domain: new max VA is less than current"); 
    max_num_var = dom->num_var_assoc;
    }
  if (max_num_nvl < dom->num_nvlist)
    {
    MVL_LOG_NERR0 ("Resizing Domain: new max NVL is less than current"); 
    max_num_nvl = dom->num_nvlist;
    }
  if (max_num_jou < dom->num_jou)
    {
    MVL_LOG_NERR0 ("Resizing Domain: new max JOU is less than current"); 
    max_num_jou = dom->num_jou;
    }

  _mvl_resize_ptr_tbl (&(dom->max_num_var_assoc), 
  		         (ST_VOID ***)&(dom->var_assoc_tbl), max_num_var);
  _mvl_resize_ptr_tbl (&(dom->max_num_nvlist), 
  		         (ST_VOID ***)&(dom->nvlist_tbl), max_num_nvl);
  _mvl_resize_ptr_tbl (&(dom->max_num_jou), 
  		         (ST_VOID ***)&(dom->jou_tbl), max_num_jou);
  }

/************************************************************************/
/*			_mvl_resize_ptr_tbl 				*/
/************************************************************************/

ST_VOID _mvl_resize_ptr_tbl (ST_INT *p_max_num, 
			     ST_VOID ***p_ptr_tbl, ST_INT new_size)
  {

/* See if something needs to be done at all */
  if (*p_max_num == new_size)
    return;

/* See if the table is going away altogether */
  if (new_size != 0)
    { 
    if (*p_max_num != 0)
      *p_ptr_tbl = (ST_VOID **) M_REALLOC (MSMEM_GEN, *p_ptr_tbl, (new_size * sizeof (ST_VOID *)));
    else   /* We did not have a table, but need one now */
      *p_ptr_tbl = (ST_VOID **) M_CALLOC (MSMEM_GEN, new_size, sizeof (ST_VOID *));
    }
  else	
    { 	/* If we had a table, we need to free it */
    if (*p_max_num != 0)
      M_FREE (MSMEM_GEN, *p_ptr_tbl); 
    }
  *p_max_num = new_size;
  }

/************************************************************************/
/*			mvl_nvl_remove_all				*/
/* Remove all VMD-Specific NVLs & Domain-Specific NVLs in this VMD.	*/
/* CRITICAL: Use "mvl_vmd_nvl_remove". It makes sure it is safe to	*/
/*           remove each NVL.						*/
/* RETURNS: SD_SUCCESS or SD_FAILURE					*/
/************************************************************************/
ST_RET mvl_nvl_remove_all (MVL_VMD_CTRL *vmd_ctrl)
  {
ST_INT didx;
MVL_DOM_CTRL *dom;

  /* Destroy VMD-Specific NVLs.	*/
  while (vmd_ctrl->num_nvlist)
    {
    MVL_NVLIST_CTRL *nvl;
    OBJECT_NAME obj;
    nvl = vmd_ctrl->nvlist_tbl[vmd_ctrl->num_nvlist-1];	/* point to last nvl*/
    obj.obj_name.vmd_spec = nvl->name;
    obj.object_tag = VMD_SPEC;
    if (mvl_vmd_nvl_remove (vmd_ctrl, &obj, NULL) != SD_SUCCESS)
      {
      MVL_LOG_ERR1 ("Error removing VMD-specific NVL='%s'", nvl->name);
      return (SD_FAILURE);
      }
    }

  /* Destroy Domain-Specific NVLs in all domains.	*/
  for (didx = 0; didx < vmd_ctrl->num_dom; didx++)
    {
    dom = vmd_ctrl->dom_tbl [didx];
    while (dom->num_nvlist)
      {
      MVL_NVLIST_CTRL *nvl;
      OBJECT_NAME obj;
      nvl = dom->nvlist_tbl[dom->num_nvlist-1];	/* point to last nvl*/
      obj.obj_name.item_id = nvl->name;
      obj.domain_id = dom->name;
      obj.object_tag = DOM_SPEC;
      if (mvl_vmd_nvl_remove (vmd_ctrl, &obj, NULL) != SD_SUCCESS)
        {
        MVL_LOG_ERR2 ("Error removing NVL='%s' in domain '%s'", nvl->name, dom->name);
        return (SD_FAILURE);
        }
      }
    }
  return (SD_SUCCESS);
  }

/************************************************************************/
/*			mvl_var_in_nvl					*/
/* Is this var used in this NVL?					*/
/************************************************************************/
ST_BOOLEAN mvl_var_in_nvl (MVL_NVLIST_CTRL *nvl, MVL_VAR_ASSOC *var)
  {
ST_INT vidx;
MVL_VAR_ASSOC *nvl_var;	/* variable used in a NVL	*/

  for (vidx = 0; vidx < nvl->num_of_entries; vidx++)
    {
    nvl_var = nvl->entries[vidx];
    if (nvl_var == var || nvl_var->base_va == var)
      {	/* This variable (or a variable derived from it) is used in the NVL.	*/
      return(SD_TRUE);
      }
    }
  return (SD_FALSE);	/* This variable is NOT used in the NVL.	*/
  }
/************************************************************************/
/*			mvl_var_deletable				*/
/* Is this var deletable (i.e. NOT referenced by other objects)?	*/
/************************************************************************/
ST_BOOLEAN mvl_var_deletable (MVL_VMD_CTRL *vmd_ctrl, MVL_VAR_ASSOC *var)
  {
ST_INT didx, nvlidx;
MVL_DOM_CTRL *dom;
MVL_NVLIST_CTRL *nvl;
  assert (var->base_va == NULL);	/* DEBUG: these are always base variables.*/

  /* Make sure var is not used in any VMD-specific NVL. */
  for (nvlidx = 0; nvlidx < vmd_ctrl->num_nvlist; ++nvlidx)
    {
    nvl = vmd_ctrl->nvlist_tbl[nvlidx];
    if (mvl_var_in_nvl (nvl, var))
      {
      MVL_LOG_ERR2 ("Variable=%s cannot be deleted because it is used in VMD-specific NVL=%s",
                    var->name, nvl->name);
      return(SD_FALSE);
      }
    }

  /* Make sure var is not used in any Domain-specific NVL in any Domain. */
  for (didx = 0; didx < vmd_ctrl->num_dom; didx++)
    {
    dom = vmd_ctrl->dom_tbl [didx];
    for (nvlidx = 0; nvlidx < dom->num_nvlist; nvlidx++)
      {
      nvl = dom->nvlist_tbl[nvlidx];
      if (mvl_var_in_nvl (nvl, var))
        {
        MVL_LOG_ERR2 ("Variable=%s cannot be deleted because it is used in Domain-specific NVL=%s",
                      var->name, nvl->name);
        return(SD_FALSE);
        }
      }
    }
  return (SD_TRUE);	/* ok to delete it	*/
  }
/************************************************************************/
/*			mvl_nvl_deletable				*/
/* Is this NVL deletable (i.e. NOT referenced by other objects)?	*/
/************************************************************************/
ST_BOOLEAN mvl_nvl_deletable (MVL_VMD_CTRL *vmd_ctrl, MVL_NVLIST_CTRL *nvl)
  {
#if defined(MVL_UCA)	/* check for IEC 61850 or UCA RCBs using this NVL*/
MVLU_RPT_CTRL *rptCtrl;

  /* NOTE: the flag "nvl->mms_deletable" is only used by the MMS service*/
  /* DeleteNamedVariableList, not to prevent MVL from deleting this NVL.*/
    
  /* See if NVL is used in any IEC 61850 Report Control Block.	*/
  for (rptCtrl = mvl61850_rpt_ctrl_list;
       rptCtrl != NULL;
       rptCtrl = (MVLU_RPT_CTRL *) list_get_next (mvl61850_rpt_ctrl_list, rptCtrl))
    {
    if (rptCtrl->dsNvl == nvl)
      {
      MVL_LOG_ERR1 ("NVL='%s' cannot be deleted because it is used in a IEC 61850 Report Control Block",
                    nvl->name);
      return (SD_FALSE);
      }
    }
  /* See if NVL is used in any UCA Report Control Block.	*/
  for (rptCtrl = mvlu_rpt_ctrl_list;
       rptCtrl != NULL;
       rptCtrl = (MVLU_RPT_CTRL *) list_get_next (mvlu_rpt_ctrl_list, rptCtrl))
    {
    if (rptCtrl->dsNvl == nvl)
      {
      MVL_LOG_ERR1 ("NVL='%s' cannot be deleted because it is used in a UCA Report Control Block",
                    nvl->name);
      return (SD_FALSE);
      }
    }
#endif	/* defined(MVL_UCA)	*/
  /* NOTE: if not MVL_UCA, nothing to check.	*/
  return (SD_TRUE);	/* ok to delete it	*/
  }

/************************************************************************/
/************************************************************************/
/* DOMAIN								*/
/************************************************************************/
/************************************************************************/
/*			mvl_vmd_dom_add					*/
/* Add a domain. Allocate, fill in, and insert into table.		*/
/************************************************************************/

MVL_DOM_CTRL *mvl_vmd_dom_add (MVL_VMD_CTRL *vmd_ctrl, ST_CHAR *name, ST_INT max_num_var, 
			ST_INT max_num_nvl, ST_INT max_num_jou,
			ST_BOOLEAN copy_name)
  {
MVL_DOM_CTRL *dom;

  dom = mvl_dom_create (name, max_num_var, max_num_nvl, max_num_jou, copy_name);

/* Insert it in table.	*/
  if (mvl_vmd_dom_insert (vmd_ctrl, dom))
    {			   		/* Failed.			*/
    M_FREE (MSMEM_GEN, dom);
    return (NULL);
    }

  return (dom);
  } 

/************************************************************************/
/*			mvl_dom_create 					*/
/************************************************************************/

MVL_DOM_CTRL *mvl_dom_create (ST_CHAR *name, ST_INT max_num_var,
			      ST_INT max_num_nvl, ST_INT max_num_jou,
			      ST_BOOLEAN copy_name)
  {
MVL_DOM_CTRL *dom;

/* Allocate object and fill in "name".	*/
  if (copy_name)
    {
    dom = (MVL_DOM_CTRL *) M_CALLOC (MSMEM_GEN, 1, sizeof (MVL_DOM_CTRL)
                              + strlen (name) + 1);
    dom->name = (ST_CHAR *) (dom+1);
    strcpy (dom->name, name);	/* copy name		*/
    }
  else
    {
    dom = (MVL_DOM_CTRL *) M_CALLOC (MSMEM_GEN, 1, sizeof (MVL_DOM_CTRL));
    dom->name = name;		/* point to user buffer	*/
    }

/* Allocate the pointer tables */
  mvl_dom_resize (dom, max_num_var, max_num_nvl, max_num_jou);
  return (dom);
  }

/************************************************************************/
/*			mvl_dom_cleanup 				*/
/* Remove all objects (NVLs, variables, journals) from this domain.	*/
/* CRITICAL: Use "*_remove" functions. They make sure it is safe to	*/
/*           remove each object.					*/
/************************************************************************/

ST_RET mvl_dom_cleanup (MVL_VMD_CTRL *vmd_ctrl, MVL_DOM_CTRL *dom)
  {
ST_INT i;

  if (dom->foundry_objects)
    {
    MVL_LOG_NERR1 ("Cannot remove objects from domain '%s' because Foundry objects present", dom->name); 
    return (SD_FAILURE);
    }

  /* Remove all NVL from this domain. Stop on first error.	*/
  while (dom->num_nvlist)
    {
    MVL_NVLIST_CTRL *nvl;
    OBJECT_NAME obj;
    nvl = dom->nvlist_tbl[dom->num_nvlist-1];	/* point to last nvl*/
    obj.obj_name.item_id = nvl->name;
    obj.domain_id = dom->name;
    obj.object_tag = DOM_SPEC;
    if (mvl_vmd_nvl_remove (vmd_ctrl, &obj, NULL) != SD_SUCCESS)
      {
      MVL_LOG_ERR2 ("Error removing NVL='%s' from domain='%s'",
          nvl->name, dom->name);
      return (SD_FAILURE);
      }
    }

  /* Remove all variables from this domain. Stop on first error.	*/
  while (dom->num_var_assoc > 0)
    {
    MVL_VAR_ASSOC *var;
    OBJECT_NAME obj;
    var = dom->var_assoc_tbl[dom->num_var_assoc-1];	/* point to last var*/
    obj.obj_name.item_id = var->name;
    obj.domain_id = dom->name;
    obj.object_tag = DOM_SPEC;
    if (mvl_vmd_var_remove (vmd_ctrl, &obj, NULL) != SD_SUCCESS)
      {
      MVL_LOG_ERR2 ("Error removing variable='%s' from domain='%s'",
          var->name, dom->name);
      return (SD_FAILURE);
      }
    }

/* Delete the Journal control table.					*/
/* DEBUG: Should use mvl_vmd_jou_remove here but it can't fail, so	*/
/*        using mvl_jou_destroy is just as good for now.		*/
  if (dom->max_num_jou)
    {
    for (i = 0; i < dom->num_jou; ++i)
      mvl_jou_destroy (dom->jou_tbl[i]);
    }
  dom->num_jou = 0;
  return (SD_SUCCESS);
  }

/************************************************************************/
/*			mvl_vmd_dom_remove 					*/
/************************************************************************/

ST_RET mvl_vmd_dom_remove (MVL_VMD_CTRL *vmd_ctrl, ST_CHAR *dom_name)
  {
MVL_DOM_CTRL *dom;
ST_RET retcode = SD_FAILURE;

  dom = mvl_vmd_find_dom (vmd_ctrl, dom_name);
  if (dom != NULL)
    {
    /* Remove all objects in domain before delete/destroy.*/
    retcode = mvl_dom_cleanup (vmd_ctrl, dom);
    if (retcode)
      MVL_LOG_ERR1 ("Error removing objects from domain, so cannot remove domain '%s'",
                    dom->name);
    else
      {		/* cleanup successful	*/
      /* NOTE: this delete should never fail because we already found it.*/
      dom = mvl_vmd_dom_delete (vmd_ctrl, dom_name);
      if (dom != NULL)
        mvl_dom_destroy (dom);	/* cannot fail. retcode already = SD_SUCCESS*/
      else
        retcode = SD_FAILURE;	/* delete failed. Should never happen	*/
      }
    }
  return (retcode);
  }

/************************************************************************/
/*			mvl_dom_destroy 					*/
/* NOTE: this assumes all objects already removed from domain.		*/
/************************************************************************/

ST_VOID mvl_dom_destroy (MVL_DOM_CTRL *dom)
  {
  if (u_mvl_dom_destroy != NULL)
    (*u_mvl_dom_destroy)(dom);

  if (dom->max_num_var_assoc != 0)
    M_FREE (MSMEM_GEN, dom->var_assoc_tbl);

  if (dom->max_num_nvlist != 0)
    M_FREE (MSMEM_GEN, dom->nvlist_tbl); 

  if (dom->max_num_jou != 0)
    M_FREE (MSMEM_GEN, dom->jou_tbl); 

  M_FREE (MSMEM_GEN, dom);
  }

/************************************************************************/
/**** VARIABLE ASSOCIATIONS						*/
/************************************************************************/
/*			mvl_vmd_var_add					*/
/* Add a variable association. Allocate, fill in, and insert into table.*/
/* NOT RECOMMENDED: use mvl_var_add_alloc.				*/
/************************************************************************/

MVL_VAR_ASSOC *mvl_vmd_var_add (MVL_VMD_CTRL *vmd_ctrl, OBJECT_NAME *obj, MVL_NET_INFO *net_info,
			ST_INT type_id,
			ST_VOID *data, 
			MVL_VAR_PROC *proc,
			ST_BOOLEAN copy_name)
  {
MVL_VAR_ASSOC *va;
MVL_DOM_CTRL *dom = NULL;

/* Find domain, if necessary, before allocating anything.	*/
  if (obj->object_tag==DOM_SPEC && (dom = mvl_vmd_find_dom (vmd_ctrl, obj->domain_id)) == NULL)
    {
    MVL_LOG_ERR2 ("Error creating VAR '%s': domain '%s' not found", 
        obj->obj_name.vmd_spec, obj->domain_id);
    return (NULL);
    }

  va = mvl_var_create (obj, type_id, data, proc, copy_name);

/* Insert it in table.	*/
  if (va)
    {
    if (mvl_vmd_var_insert (vmd_ctrl, va, obj, net_info))
      {			   		/* Failed.			*/
      mvl_var_destroy (va);	/* destroy var just created	*/
      return (NULL);
      }
    }

  return (va);
  }

/************************************************************************/
/*			mvl_var_create 					*/
/* NOT RECOMMENDED: use mvl_var_create_alloc.				*/
/************************************************************************/

MVL_VAR_ASSOC *mvl_var_create (OBJECT_NAME *obj, ST_INT type_id,  
				 ST_VOID *data, MVL_VAR_PROC *proc,
			         ST_BOOLEAN copy_name)
  {
MVL_VAR_ASSOC *va;
MVL_TYPE_CTRL *type_ctrl;

  type_ctrl = mvl_type_ctrl_find (type_id);
  if (type_ctrl == NULL)
    {
    MVL_LOG_ERR0 ("Cannot create variable (type_id invalid).");
    return (NULL);
    }  

/* Allocate object and fill in "name".	*/
  if (copy_name)
    {
    va = (MVL_VAR_ASSOC *) M_CALLOC (MSMEM_GEN, 1, sizeof (MVL_VAR_ASSOC)
                              + strlen (obj->obj_name.vmd_spec) + 1);
    va->name = (ST_CHAR *) (va+1);
    strcpy (va->name, obj->obj_name.vmd_spec);	/* copy name		*/
    }
  else
    {
    va = (MVL_VAR_ASSOC *) M_CALLOC (MSMEM_GEN, 1, sizeof (MVL_VAR_ASSOC));
    va->name = obj->obj_name.vmd_spec;		/* point to user buffer	*/
    }

/* Fill in rest of object.	*/
  va->type_id = type_id;
  va->type_ctrl = type_ctrl;	/* don't need to call mvl_get_runtime	*/
				/* to get type details.			*/
  va->data = data;
#if defined(MVL_UCA)
/* If caller set the data ptr (!=NULL), assume they want to use static data.*/
  if (data!=NULL)
    va->use_static_data = SD_TRUE;
#endif
  va->proc = proc;
  return (va);
  }

/************************************************************************/
/*			mvl_var_add_alloc				*/
/* Add a variable association. Allocate, fill in, and insert into table.*/
/************************************************************************/
MVL_VAR_ASSOC *mvl_var_add_alloc (MVL_VMD_CTRL *vmd_ctrl,
	OBJECT_NAME *obj,
	MVL_NET_INFO *net_info,
	ST_INT type_id,
	MVL_VAR_PROC *proc)
  {
MVL_VAR_ASSOC *va;
MVL_DOM_CTRL *dom = NULL;

/* Find domain, if necessary, before allocating anything.	*/
  if (obj->object_tag==DOM_SPEC && (dom = mvl_vmd_find_dom (vmd_ctrl, obj->domain_id)) == NULL)
    {
    MVL_LOG_ERR2 ("Error creating VAR '%s': domain '%s' not found", 
        obj->obj_name.vmd_spec, obj->domain_id);
    return (NULL);
    }

  va = mvl_var_create_alloc (obj->obj_name.vmd_spec, type_id, proc);

/* Insert it in table.	*/
  if (va)
    {
    if (mvl_vmd_var_insert (vmd_ctrl, va, obj, net_info))
      {			   		/* Failed.			*/
      mvl_var_destroy (va);	/* destroy var just created	*/
      return (NULL);
      }
    }
  return (va);
  }

/************************************************************************/
/*			mvl_var_create_alloc				*/
/* Create a new variable. ALWAYS allocate a buffer for			*/
/* the data (buffer is automatically freed when variable is destroyed).	*/
/* CRITICAL: DO NOT use for "derived" variables. Instead use		*/
/*           mvl_var_create_derived.					*/
/************************************************************************/
MVL_VAR_ASSOC *mvl_var_create_alloc (ST_CHAR *name,
	ST_INT type_id,  
	MVL_VAR_PROC *proc)
  {
MVL_VAR_ASSOC *va;
MVL_TYPE_CTRL *type_ctrl;

  type_ctrl = mvl_type_ctrl_find (type_id);
  if (type_ctrl == NULL)
    {
    MVL_LOG_ERR0 ("Cannot create variable (type_id invalid).");
    return (NULL);
    }  

  /* Allocate one buffer for MVL_VAR_ASSOC plus "data" plus "name".	*/
  va = (MVL_VAR_ASSOC *) chk_calloc (1,
             sizeof (MVL_VAR_ASSOC)	/* space for struct*/
             + type_ctrl->data_size	/* space for "data"*/
             + strlen (name) + 1);	/* space for "name"*/

  /* Set data ptr right after MVL_VAR_ASSOC struct.			*/
  /* NOTE: data is before name so alignment should be good for any data.*/
  va->data = (va+1);
  /* Set name ptr right after data.	*/
  va->name = ((ST_CHAR *) (va->data)) + type_ctrl->data_size;

  strcpy (va->name, name);	/* copy name		*/

  /* Fill in rest of object.	*/
  va->type_id = type_id;
  va->type_ctrl = type_ctrl;	/* save for easy access later.	*/

  va->use_static_data = SD_TRUE;	/* always SD_TRUE	*/

  va->proc = proc;
  return (va);
  }

/************************************************************************/
/*			mvl_var_create_derived				*/
/* Create a "derived" variable.						*/
/* NOTE: Structure member "proc" always set to NULL.			*/
/*       Structure member "data" points inside base variable.		*/
/************************************************************************/

MVL_VAR_ASSOC *mvl_var_create_derived (ST_CHAR *name,
	ST_INT type_id,
	MVL_VAR_ASSOC *baseVa,	/* base variable from which to derive	*/
	ST_INT offset_from_base,
	ST_UINT prim_num)
  {
MVL_VAR_ASSOC *va;
MVL_TYPE_CTRL *type_ctrl;

  type_ctrl = mvl_type_ctrl_find (type_id);
  if (type_ctrl == NULL)
    {
    MVL_LOG_ERR0 ("Cannot create variable (type_id invalid).");
    return (NULL);
    }  

/* Allocate object and fill in "name".	*/
  va = (MVL_VAR_ASSOC *) chk_calloc (1, sizeof (MVL_VAR_ASSOC)
                            + strlen (name) + 1);
  va->name = (ST_CHAR *) (va+1);
  strcpy (va->name, name);	/* copy name		*/

/* Fill in rest of object.	*/
  va->type_id = type_id;
  va->type_ctrl = type_ctrl;	/* save for easy access later.	*/

  /* va->proc = NULL (due to calloc above)	*/

  /* Set special "va" elements only for derived variables.	*/
  va->base_va   = baseVa;
  va->user_info = baseVa->user_info;

  /* Save offset_from_base & prim_num passed as arguments.	*/
  va->offset_from_base = offset_from_base;
  va->prim_num         = prim_num;

  if (va->base_va->use_static_data)
    va->data = (((ST_CHAR *) va->base_va->data) + va->offset_from_base);
  /* else caller must set va->data.	*/

  return (va);
  }

/************************************************************************/
/*			mvl_vmd_var_remove 					*/
/************************************************************************/

ST_RET mvl_vmd_var_remove (MVL_VMD_CTRL *vmd_ctrl, OBJECT_NAME *obj, MVL_NET_INFO *net_info)
  {
MVL_VAR_ASSOC *va;

  /* First find it & make sure it is deletable.	*/
  va = mvl_vmd_find_var (vmd_ctrl, obj, net_info);
  if (va == NULL || mvl_var_deletable (vmd_ctrl, va) == SD_FALSE)
    {
    MVL_LOG_ERR1 ("Error deleting variable='%s': not found or not deletable.",
        obj->obj_name.vmd_spec);
    return (SD_FAILURE);
    }

  va = mvl_vmd_var_delete (vmd_ctrl, obj, net_info);
  if (va == NULL)
    return (SD_FAILURE);

  mvl_var_destroy (va);
  return (SD_SUCCESS);
  }

/************************************************************************/
/*			mvl_var_destroy 					*/
/************************************************************************/

ST_VOID mvl_var_destroy (MVL_VAR_ASSOC *va)
  {
  if (u_mvl_var_destroy != NULL)
    (*u_mvl_var_destroy)(va);

  /* "last_data" may have been allocated if this var used in NVL.	*/
  if (va->last_data)
    chk_free (va->last_data);
  M_FREE (MSMEM_GEN, va);
  }


/************************************************************************/
/**** NAMED VARIABLE LISTS						*/
/************************************************************************/
/*			mvl_vmd_nvl_add					*/
/* Add a Named Variable List. Allocate, fill in, and insert into table.	*/
/************************************************************************/

MVL_NVLIST_CTRL *mvl_vmd_nvl_add (MVL_VMD_CTRL *vmd_ctrl, OBJECT_NAME *nvl_obj, MVL_NET_INFO *net_info,
			ST_INT num_var,
			OBJECT_NAME *var_obj,	/* array of var names	*/
			ST_BOOLEAN copy_name)
  {
MVL_NVLIST_CTRL *nvl;
MVL_DOM_CTRL *dom = NULL;

/* Find domain, if necessary, before allocating anything.	*/
  if (nvl_obj->object_tag==DOM_SPEC && (dom = mvl_vmd_find_dom (vmd_ctrl, nvl_obj->domain_id)) == NULL)
    {
    MVL_LOG_ERR2 ("Error creating NVL '%s': domain '%s' not found", 
        nvl_obj->obj_name.vmd_spec, nvl_obj->domain_id);
    return (NULL);
    }

/* Allocate it AND fill it in.	*/
  nvl = mvl_nvl_create (vmd_ctrl, nvl_obj->obj_name.vmd_spec, num_var, var_obj, net_info, copy_name);
  if (nvl == NULL)
    return (NULL);

/* Add scope info to "nvl".	*/
  nvl->nvl_scope.scope = nvl_obj->object_tag;
  nvl->nvl_scope.dom = dom;	/* NOTE: NULL if not DOM_SPEC (see above)	*/

/* Insert it in table.	*/
  if (mvl_vmd_nvl_insert (vmd_ctrl, nvl, nvl_obj, net_info))
    {			   		/* Failed.			*/
    mvl_nvl_destroy (nvl);
    return (NULL);
    }

  return (nvl);
  }

/************************************************************************/
/*                       mvl_nvl_create					*/
/************************************************************************/
/* Given a list of MMS object names, create a MVL Named Variable List.	*/
/* This can include both configured and MVLU manufactured variables	*/
/* Use 'mvl_nvl_destroy' to free this NVL.				*/

MVL_NVLIST_CTRL *mvl_nvl_create (MVL_VMD_CTRL *vmd_ctrl,
			ST_CHAR *nvlName,
			ST_INT numNames,
			OBJECT_NAME *var_obj,	/* array of var obj names*/
			MVL_NET_INFO *net_info,	/* where to find var_obj*/
						/* if it is AA_SPEC	*/
			ST_BOOLEAN copy_name)
  {
MVL_NVLIST_CTRL *nvl;
ST_INT j;
  
/* Allocate object and fill in "name".	*/
  if (copy_name)
    {
    nvl = (MVL_NVLIST_CTRL *) M_CALLOC (MSMEM_GEN, 1, sizeof (MVL_NVLIST_CTRL)
                              + strlen (nvlName) + 1);
    nvl->name = (ST_CHAR *) (nvl+1);
    strcpy (nvl->name, nvlName);	/* copy name		*/
    }
  else
    {
    nvl = (MVL_NVLIST_CTRL *) M_CALLOC (MSMEM_GEN, 1, sizeof (MVL_NVLIST_CTRL));
    nvl->name = nvlName;		/* point to user buffer	*/
    }

/* Fill in rest of object.	*/

  nvl->entries = (MVL_VAR_ASSOC **) M_CALLOC (MSMEM_GEN, numNames, 
					sizeof (MVL_VAR_ASSOC *));
  nvl->va_scope = (MVL_SCOPE *) M_CALLOC (MSMEM_GEN, numNames, 
					sizeof (MVL_SCOPE));
  nvl->num_of_entries = numNames;

  for (j = 0;  j < nvl->num_of_entries;  ++j, ++var_obj)
    {
    MVL_VAR_ASSOC *var;
    if ((nvl->entries[j] = var = _mvl_objname_to_va (vmd_ctrl, net_info, MMSOP_INFO_RPT,
                      var_obj, &nvl->va_scope[j], SD_FALSE, NULL, NULL)) == NULL)
      {
      MVL_LOG_ERR2 ("Error creating NVL '%s': member variable '%s' not found", 
    					nvlName, var_obj->obj_name.vmd_spec);
      mvl_nvl_destroy (nvl);
      return (NULL);
      }
    else
      {
#if defined(MVL_UCA)
      /* Alloc buffer for last data (used for IEC/UCA reports).	*/
      if (!var->last_data)
        var->last_data = chk_calloc (1, mvl_type_ctrl[var->type_id].data_size);
#endif
      }
    }

  return (nvl);
  }


/************************************************************************/
/*			mvl_vmd_nvl_remove					*/
/* Remove a Named Variable List. Delete it from table and "destroy" it.	*/
/************************************************************************/

ST_RET mvl_vmd_nvl_remove (MVL_VMD_CTRL *vmd_ctrl, OBJECT_NAME *obj, MVL_NET_INFO *net_info)
  {
MVL_NVLIST_CTRL *nvl;

/* First find it & make sure it is deletable.	*/
  nvl = mvl_vmd_find_nvl (vmd_ctrl, obj, net_info);
  if (nvl == NULL || mvl_nvl_deletable (vmd_ctrl, nvl) == SD_FALSE)
    {
    MVL_LOG_ERR1 ("Error deleting NVL='%s': not found or not deletable.",
        obj->obj_name.vmd_spec);
    return (SD_FAILURE);
    }

/* Delete it from table.	*/
  if ((nvl = mvl_vmd_nvl_delete (vmd_ctrl, obj, net_info)) == NULL)
    return (SD_FAILURE);		/* nvl not found	*/

/* Destroy it.			*/
  mvl_nvl_destroy (nvl);
  return (SD_SUCCESS);
  }

/************************************************************************/
/*                       mvl_nvl_destroy				*/
/************************************************************************/
/* Free a MVL Named Variable List created by 'mvl_nvl_create'		*/

ST_VOID mvl_nvl_destroy (MVL_NVLIST_CTRL *nvl)
  {
MVL_VAR_ASSOC *va;
ST_INT i;

  if (u_mvl_nvl_destroy != NULL)
    (*u_mvl_nvl_destroy)(nvl);

/* Free up any manufactured variables */
  for (i = 0; i < nvl->num_of_entries; ++i)
    {
    va = nvl->entries[i];
#if defined(MVL_UCA) || defined(USE_MANUFACTURED_OBJS)
/* NOTE: "va" may be NULL if mvl_nvl_create failed while adding entries.*/
    if (va && va->va_to_free == va)
      {
      u_mvl_free_va (MMSOP_INFO_RPT, va, NULL);
      }
#endif
    }

  /* If Alternate Access is present in this NVL, also free these ptrs.	*/
  if (nvl->altAccArray)
    {
    for (i = 0; i < nvl->num_of_entries; ++i)
      {
      if (nvl->altAccArray[i])
        chk_free (nvl->altAccArray[i]);
      }
    chk_free(nvl->altAccArray);
    }


  M_FREE (MSMEM_GEN, nvl->entries);
  M_FREE (MSMEM_GEN, nvl->va_scope);
  M_FREE (MSMEM_GEN, nvl);
  }

/************************************************************************/
/**** JOURNALS								*/
/************************************************************************/
/*			mvl_vmd_jou_add					*/
/* Add a Journal. Allocate, fill in, and insert into table.		*/
/************************************************************************/

MVL_JOURNAL_CTRL *mvl_vmd_jou_add (MVL_VMD_CTRL *vmd_ctrl, OBJECT_NAME *obj, MVL_NET_INFO *net_info,
			ST_BOOLEAN copy_name)
  {
MVL_JOURNAL_CTRL *jou;

  jou = mvl_jou_create (obj, net_info, copy_name);
  if (mvl_vmd_jou_insert (vmd_ctrl, jou, obj, net_info))
    {			   		/* Failed.			*/
    M_FREE (MSMEM_GEN, jou);
    return (NULL);
    }
  return (jou);
  } 

/************************************************************************/
/*			mvl_jou_create 					*/
/************************************************************************/

MVL_JOURNAL_CTRL *mvl_jou_create (OBJECT_NAME *obj, MVL_NET_INFO *net_info,
			         ST_BOOLEAN copy_name)
  {
MVL_JOURNAL_CTRL *jou;

/* Allocate object and fill in "name".	*/
  if (copy_name)
    {
    jou = (MVL_JOURNAL_CTRL *) M_CALLOC (MSMEM_GEN, 1, sizeof (MVL_JOURNAL_CTRL)
                              + strlen (obj->obj_name.vmd_spec) + 1);
    jou->name = (ST_CHAR *) (jou+1);
    strcpy (jou->name, obj->obj_name.vmd_spec);	/* copy name		*/
    }
  else
    {
    jou = (MVL_JOURNAL_CTRL *) M_CALLOC (MSMEM_GEN, 1, sizeof (MVL_JOURNAL_CTRL));
    jou->name = obj->obj_name.vmd_spec;		/* point to user buffer	*/
    }

  return (jou);
  }

/************************************************************************/
/*			mvl_vmd_jou_remove 					*/
/************************************************************************/

ST_RET mvl_vmd_jou_remove (MVL_VMD_CTRL *vmd_ctrl, OBJECT_NAME *obj, MVL_NET_INFO *net_info)
  {
MVL_JOURNAL_CTRL *jou;

  jou = mvl_vmd_jou_delete (vmd_ctrl, obj, net_info);
  if (jou == NULL)
    return (SD_FAILURE);

  mvl_jou_destroy (jou);
  return (SD_SUCCESS);
  }

/************************************************************************/
/*			mvl_jou_destroy 				*/
/************************************************************************/

ST_VOID mvl_jou_destroy (MVL_JOURNAL_CTRL *jou)
  {
  if (u_mvl_jou_destroy != NULL)
    (*u_mvl_jou_destroy)(jou);

  M_FREE (MSMEM_GEN, jou);
  }

/************************************************************************/
/*			 mvl_vmd_create					*/
/* Create a new VMD with empty object arrays.				*/
/* Use "mvl_vmd_destroy" to destroy the VMD.				*/
/************************************************************************/
MVL_VMD_CTRL *mvl_vmd_create (ST_INT max_num_dom,
	ST_INT max_num_var,
	ST_INT max_num_nvl,
	ST_INT max_num_journal)
  {
MVL_VMD_CTRL *vmd_ctrl = (MVL_VMD_CTRL*) chk_calloc (1, sizeof (MVL_VMD_CTRL));

  vmd_ctrl->max_num_var_assoc = max_num_var;
  if (vmd_ctrl->max_num_var_assoc)
    vmd_ctrl->var_assoc_tbl = (MVL_VAR_ASSOC **) chk_calloc (
                        vmd_ctrl->max_num_var_assoc, sizeof (MVL_VAR_ASSOC *));

  vmd_ctrl->max_num_nvlist = max_num_nvl;
  if (vmd_ctrl->max_num_nvlist)
    vmd_ctrl->nvlist_tbl = (MVL_NVLIST_CTRL **) chk_calloc (
                        vmd_ctrl->max_num_nvlist, sizeof (MVL_NVLIST_CTRL *));

  vmd_ctrl->max_num_dom = max_num_dom;
  if (vmd_ctrl->max_num_dom)
    vmd_ctrl->dom_tbl = (MVL_DOM_CTRL **) chk_calloc (
                        vmd_ctrl->max_num_dom, sizeof (MVL_DOM_CTRL *));

  vmd_ctrl->max_num_jou = max_num_journal;
  if (vmd_ctrl->max_num_jou)
    vmd_ctrl->jou_tbl = (MVL_JOURNAL_CTRL **) chk_calloc (
                        vmd_ctrl->max_num_jou, sizeof (MVL_JOURNAL_CTRL *));

  return (vmd_ctrl);
  };

/************************************************************************/
/*			mvl_vmd_destroy					*/
/* Destroy all objects in this VMD.					*/
/* RETURNS:  SD_SUCCESS or error code					*/
/************************************************************************/
ST_RET mvl_vmd_destroy (MVL_VMD_CTRL *vmd_ctrl)
  {
ST_RET ret;
ST_INT j;
MVL_DOM_CTRL *dom;

  ret = SD_SUCCESS;	/* default ret val: returned if LD linked list empty.*/

  /* Remove all NVL from this VMD and from all Domains before deleting	*/
  /* Domains. NVLs might reference vars in other domains.		*/
  /* CRITICAL: Must be done first, or vars can't be destroyed later.	*/
  ret = mvl_nvl_remove_all (vmd_ctrl);
  if (ret)
    {
    MVL_LOG_ERR0 ("Error destroying VMD: failed removing all NVL.");
    return (ret);
    }
  
  /* Delete VMD-Specific Variables.	*/
  for (j = 0; j < vmd_ctrl->num_var_assoc; ++j)
    mvl_var_destroy (vmd_ctrl->var_assoc_tbl[j]);
  vmd_ctrl->num_var_assoc = 0;

  /* Delete Domains (always VMD-Specific).	*/
  while ((dom = mvl_vmd_dom_find_last (vmd_ctrl)) != NULL)
    {
    ret = mvl_vmd_dom_remove (vmd_ctrl, dom->name);

    if (ret != SD_SUCCESS)
      break;
    }

  /* Destroy all types for this VMD.	*/
  if (ret == SD_SUCCESS)
    ret = mvl_vmd_type_id_destroy_all (vmd_ctrl);

  /* Finally, free the tables and the VMD itself (allocated by mvl_vmd_create).*/
  /* NOTE: can't do this for "mvl_vmd" because it was created by Foundry.*/
  if (vmd_ctrl != &mvl_vmd)
    {
    if (vmd_ctrl->var_assoc_tbl)
      chk_free (vmd_ctrl->var_assoc_tbl);
    if (vmd_ctrl->nvlist_tbl)
      chk_free (vmd_ctrl->nvlist_tbl);
    if (vmd_ctrl->dom_tbl)
      chk_free (vmd_ctrl->dom_tbl);
    if (vmd_ctrl->jou_tbl)
      chk_free (vmd_ctrl->jou_tbl);
    chk_free (vmd_ctrl);
    }

  return (ret);
  }

/************************************************************************/
/*			mvl_nvl_create_aa				*/
/* Create Named Variable List with Alternate Access specified on	*/
/* one or more variables.						*/
/************************************************************************/
MVL_NVLIST_CTRL *mvl_nvl_create_aa (MVL_VMD_CTRL *vmd_ctrl,
	ST_CHAR *nvlName,
	ST_INT numNames,
	OBJECT_NAME *var_obj,	/* array of var obj names*/
	ALT_ACCESS **alt_acc,	/* array of alt acc info for each var	*/
				/* NULL if no alt acc present		*/
				/* each ptr in array is NULL if no alt acc for that var*/
	MVL_NET_INFO *net_info)	/* where to find var_obj*/
				/* if it is AA_SPEC	*/
  {
MVL_NVLIST_CTRL *nvl;
ST_INT j;
ST_INT err,ok;
err = 0;
ok = 0;
  
  /* Allocate object and fill in "name".	*/
  nvl = (MVL_NVLIST_CTRL *) M_CALLOC (MSMEM_GEN, 1, sizeof (MVL_NVLIST_CTRL)
                            + strlen (nvlName) + 1);
  nvl->name = (ST_CHAR *) (nvl+1);
  strcpy (nvl->name, nvlName);	/* copy name		*/

  /* Fill in rest of object.	*/
  nvl->entries = (MVL_VAR_ASSOC **) M_CALLOC (MSMEM_GEN, numNames, 
					sizeof (MVL_VAR_ASSOC *));
  nvl->va_scope = (MVL_SCOPE *) M_CALLOC (MSMEM_GEN, numNames, 
					sizeof (MVL_SCOPE));
  nvl->num_of_entries = numNames;

  /* If alternate access info is present, save it in altAccArray to use in	*/
  /* GetNamedVariableListAttributes response (see mvlas_getvlist_resp).	*/
  /* altAccArray must be freed in mvl_nvl_destroy.				*/
  if (alt_acc)
    {
    nvl->altAccArray = chk_calloc (nvl->num_of_entries, sizeof (ALT_ACCESS *));
    for (j = 0;  j < nvl->num_of_entries;  ++j)
      {
      if (alt_acc[j])
        {
        ALT_ACCESS *tmpAltAcc;
        /* alloc ALT_ACCESS struct plus array of ALT_ACC_EL in one buffer.*/
        tmpAltAcc = chk_malloc (sizeof (ALT_ACCESS) + alt_acc[j]->num_aa * sizeof (ALT_ACC_EL));
        tmpAltAcc->num_aa = alt_acc[j]->num_aa;
        tmpAltAcc->aa = (ALT_ACC_EL *)(tmpAltAcc + 1);  /* point after struct*/
        /* Copy array of ALT_ACC_EL.	*/
        memcpy (tmpAltAcc->aa, alt_acc[j]->aa, alt_acc[j]->num_aa * sizeof (ALT_ACC_EL));
        nvl->altAccArray [j] = tmpAltAcc;	/* save new ptr in array*/
        }
      }
    }
  /* CRITICAL: if (nvl->altAccArray!=NULL), it must be freed later by "mvl_nvl_destroy".*/

  for (j = 0;  j < nvl->num_of_entries;  ++j, ++var_obj)
    {
    MVL_VAR_ASSOC *var;
    /* CRITICAL: create var with alternate access already resolved.	*/
    ST_BOOLEAN alt_acc_present; 	/* _mvl_objname_to_va needs this flag*/
    ALT_ACCESS *this_alt_acc;
    if (alt_acc != NULL && alt_acc[j] != NULL)
      {
      this_alt_acc = alt_acc[j];
      alt_acc_present = SD_TRUE;
      }
    else
      {
      this_alt_acc = NULL;
      alt_acc_present = SD_FALSE;
      }
    if ((nvl->entries[j] = var = _mvl_objname_to_va (vmd_ctrl, net_info, MMSOP_INFO_RPT,
                      var_obj, &nvl->va_scope[j], alt_acc_present, this_alt_acc, NULL)) == NULL)
      {
      MVL_LOG_ERR2 ("Error creating NVL '%s': member variable '%s' not found", 
                    nvlName, var_obj->obj_name.vmd_spec);
	  err ++;
	  //MEMO: 暂时不退出，把所有错误都列出来 [2017-9-13 11:25 邵凯田]
	  /*
      mvl_nvl_destroy (nvl);
      return (NULL);*/
      }
    else
      {
		  ok ++;
#if defined(MVL_UCA)
      /* Alloc buffer for last data (used for IEC/UCA reports).	*/
      if (!var->last_data)
        var->last_data = chk_calloc (1, mvl_type_ctrl[var->type_id].data_size);
#endif
      }
    }
  if(err > 0) 
  {
	  mvl_nvl_destroy (nvl);
	  return (NULL);
  }
  return (nvl);
  }
/************************************************************************/
/*			mvl_vmd_nvl_add_aa				*/
/* Add a Named Variable List with Alternate Access specified on		*/
/* one or more variables.						*/
/* Allocate, fill in, and insert into table.				*/
/************************************************************************/
MVL_NVLIST_CTRL *mvl_vmd_nvl_add_aa (MVL_VMD_CTRL *vmd_ctrl,
	OBJECT_NAME *nvl_obj,
	MVL_NET_INFO *net_info,
	ST_INT num_var,
	OBJECT_NAME *var_obj,	/* array of var names	*/
	ALT_ACCESS **alt_acc)	/* array of alt acc info for each var	*/
				/* NULL if no alt acc present		*/
				/* each ptr in array is NULL if no alt acc for that var*/
  {
MVL_NVLIST_CTRL *nvl;
MVL_DOM_CTRL *dom = NULL;

  /* Find domain, if necessary, before allocating anything.	*/
  if (nvl_obj->object_tag==DOM_SPEC && (dom = mvl_vmd_find_dom (vmd_ctrl, nvl_obj->domain_id)) == NULL)
    {
    MVL_LOG_ERR2 ("Error creating NVL '%s': domain '%s' not found", 
        nvl_obj->obj_name.vmd_spec, nvl_obj->domain_id);
    return (NULL);
    }

  /* Allocate it AND fill it in.	*/
  nvl = mvl_nvl_create_aa (vmd_ctrl, nvl_obj->obj_name.vmd_spec, num_var, var_obj, alt_acc, net_info);
  if (nvl == NULL)
    return (NULL);

  /* Add scope info to "nvl".	*/
  nvl->nvl_scope.scope = nvl_obj->object_tag;
  nvl->nvl_scope.dom = dom;	/* NOTE: NULL if not DOM_SPEC (see above)	*/

  /* Insert it in table.	*/
  if (mvl_vmd_nvl_insert (vmd_ctrl, nvl, nvl_obj, net_info))
    {			   		/* Failed.			*/
    mvl_nvl_destroy (nvl);
    return (NULL);
    }

  return (nvl);
  }


