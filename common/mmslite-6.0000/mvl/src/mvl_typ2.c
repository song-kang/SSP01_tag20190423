/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	2000 - 2000, All Rights Reserved				*/
/*									*/
/* MODULE NAME : mvl_typ2.c						*/
/* PRODUCT(S)  : MMSEASE-LITE						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*	Dynamic type creation/destruction functions.			*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*	mvl_type_id_create						*/
/*	mvl_type_id_destroy						*/
/*	mvl_type_id_create_from_tdl					*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev     Comments					*/
/* --------  ---  ------   -------------------------------------------	*/
/* 11/23/11  KCR           Added casting for proper 64bit compile       */
/* 07/11/07  JRB     12    mvl_type_id_destroy2: Reset struct to all 0.	*/
/* 11/22/06  JRB     11    mvl_type_id_create_from_tdl: clean up on err.*/
/* 10/30/06  JRB     10    Replace static mvl_type_id_create2 with	*/
/*			   extern mvl_vmd_type_id_create which has	*/
/*			   vmd_ctrl arg.				*/
/*			   Add "mvl_vmd_type_id_destroy(_all)".		*/
/*			   Add mvl_type_count.				*/
/* 01/30/06  GLB     09    Integrated porting changes for VMS           */
/* 04/11/05  CRM     08	   Added mvl_type_id_destroy_all_dyn		*/
/* 03/14/05  CRM     07    Replaced _mvlu_num_dyn_types variable with   */
/*                         mvl_max_dyn.types.                           */
/* 06/29/04  JRB     06    Add mvl_type_id_create_from_tdl.		*/
/* 09/18/03  JRB     05    mvl_type_id_create: Allow type_name==NULL.	*/
/* 03/13/03  JRB     04    Use static funcs mvl_type_id_create2/destroy2.*/
/*			   In create2, make sure name not already used.	*/
/*			   Call mvlu_proc_rt_type for UCA types.	*/
/*			   MVL_TYPE_CTRL: type_name is now array, copy to it*/
/* 11/11/02  JRB     03    Use NEW ms_runtime_create/destroy (new args).*/
/* 09/05/00  JRB     02    Use mvl_get_runtime_dyn to find runtime type.*/
/* 09/05/00  JRB     01    Created. Moved code here from "mvl_type.c".	*/
/************************************************************************/
#include "glbtypes.h"
#include "sysincs.h"

#include "mmsdefs.h"
#include "mms_pvar.h"
#include "mms_vvar.h"
#include "mvl_defs.h"
#include "mvl_log.h"
#include "mvl_uca.h"	/* need mvlu_proc_rt_type proto	*/


/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/
/************************************************************************/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/
/* Static function prototypes.						*/
/************************************************************************/
static RUNTIME_CTRL *mvl_type_id_destroy2 (ST_INT typeId);

/************************************************************************/
/*			mvl_type_id_create				*/
/* RETURNS:								*/
/*	Type Id (or -1 on error)					*/
/* NOTE: Use "mvl_type_id_destroy" to destroy this object.		*/
/************************************************************************/
ST_INT mvl_type_id_create (ST_CHAR *type_name, ST_UCHAR *asn1_data, ST_UINT asn1_len)
  {
RUNTIME_CTRL *rt_ctrl;
ST_INT TypeId = -1;	/* assume failure	*/

  rt_ctrl = ms_runtime_create (asn1_data, asn1_len,
                               (UINT_MAX/sizeof(RUNTIME_TYPE))-100);
  if (rt_ctrl)
    {			/* "runtime create" successful, do "step 2".	*/
    TypeId = mvl_vmd_type_id_create (&mvl_vmd,	/* assume global VMD	*/
             type_name, rt_ctrl);
    if (TypeId < 0)	/* "step 2" failed, so destroy runtime just created.*/
      ms_runtime_destroy (rt_ctrl);
    }
  return (TypeId);
  }

/************************************************************************/
/*			mvl_type_id_destroy				*/
/* Destroy object created by "mvl_type_id_create".			*/
/************************************************************************/
ST_VOID mvl_type_id_destroy (ST_INT TypeId)
  {
RUNTIME_CTRL *rt_ctrl;
  rt_ctrl = mvl_type_id_destroy2 (TypeId);	/* remove it from list	*/
  if (rt_ctrl)
    ms_runtime_destroy (rt_ctrl);	/* destroy it	*/
  else
    MVL_LOG_ERR1 ("mvl_type_id_destroy error: TypeId %d not dynamically created", TypeId);
  }

/************************************************************************/
/*			mvl_type_id_destroy_all_dyn			*/
/* Destroy dynamic type ids.						*/
/************************************************************************/
ST_RET mvl_type_id_destroy_all_dyn ()
  {
ST_INT j;
ST_INT type_id_first_dyn;	/* type_id of first dynamic type	*/

  /* Loop through all dynamic types.	*/
  type_id_first_dyn = mvl_num_types - mvl_max_dyn.types;
  for (j = type_id_first_dyn; j < mvl_num_types; j++)
    {
    if (mvl_type_ctrl[j].num_rt != 0)	/* != 0 indicates this type_id used.*/
      mvl_type_id_destroy (j);		/* destroy it.	*/
    }
  return (SD_SUCCESS);	/* at present, no way to fail.	*/
  }

/************************************************************************/
/*			mvl_vmd_type_id_create				*/
/* Second step of creating type_id.					*/
/* RETURNS:								*/
/*	Type Id (or -1 on error)					*/
/************************************************************************/
ST_INT mvl_vmd_type_id_create (MVL_VMD_CTRL *vmd_ctrl, ST_CHAR *type_name, RUNTIME_CTRL *rt_ctrl)
  {
ST_INT typeId;
MVL_TYPE_CTRL *type_ctrl;	/* ptr to current array element	*/

  assert (vmd_ctrl);		/* VMD must be valid	*/

  /* This assertion fails if the global array mvl_type_ctrl is not initialized
   * To avoid this, create dynamic types ONLY AFTER "mvl_init_type_ctrl"
   * (in Foundry output "c" file) is called.
   */
  assert (mvl_type_ctrl);	/* make sure type control initialized	*/

  /* If named, make sure name not too long & name not already used.	*/
  if (type_name!=NULL)
    {
    if (strlen (type_name) >= sizeof (type_ctrl->type_name))
      return (-1);	/* type_name exceeds max allowed size	*/

    for (typeId = 0; typeId < mvl_num_types; ++typeId)
      {
      type_ctrl = &mvl_type_ctrl[typeId];	/* ptr to array elem*/
      if (type_ctrl->num_rt!=0  &&  type_ctrl->type_name!=NULL
          &&  strcmp (type_ctrl->type_name, type_name) == 0)
        {
        MVL_LOG_ERR1 ("Type creation error: name '%s' already in use", type_name);
        return (-1);	/* FAILED: name in use		*/
        }
      }
    }

#if defined (MVL_UCA)	/* do post-processing for UCA types.	*/
  if (mvlu_proc_rt_type (type_name, rt_ctrl, NULL))
    return (-1);	/* FAILED: return invalid typeId		*/
#endif	/* MVL_UCA	*/

  /* Just search for a unused slot */
  for (typeId = mvl_num_types - mvl_max_dyn.types; typeId < mvl_num_types; ++typeId)
    {
    type_ctrl = &mvl_type_ctrl[typeId];	/* ptr to array elem*/
    if (type_ctrl->num_rt == 0)
      {
      if (type_name)
        strcpy (type_ctrl->type_name, type_name);	/* size checked above	*/
      else
        type_ctrl->type_name[0] = '\0';			/* unnamed type		*/
      type_ctrl->rt_ctrl = rt_ctrl;	/* no other func sets this*/
      type_ctrl->rt = rt_ctrl->rt_first;
      type_ctrl->num_rt = rt_ctrl->rt_num;
      type_ctrl->data_size = rt_ctrl->rt_first->offset_to_last;
      type_ctrl->vmd_ctrl = vmd_ctrl;	/* save ptr to VMD using this type*/
      return (typeId);
      }
    }
  MVL_LOG_ERR0 ("mvl_type_id_create error: type array full");
  return (-1);	/* error, no slots available. Return invalid typeId.	*/
  }

/************************************************************************/
/*			mvl_type_id_destroy2				*/
/* Second step of destroying type_id.					*/
/* Remove type from array, return pointer to it.			*/
/* NOTE: uses mvl_type_ctrl_find_dyn to make sure it's "dynamic" type.	*/
/************************************************************************/
static RUNTIME_CTRL *mvl_type_id_destroy2 (ST_INT typeId)
  {
MVL_TYPE_CTRL *type_ctrl;
RUNTIME_CTRL *rt_ctrl = NULL;	/* assume failure	*/

  type_ctrl = mvl_type_ctrl_find_dyn (typeId);
  if (type_ctrl)
    {		/* save ptr to type, then remove it from array	*/
    rt_ctrl = type_ctrl->rt_ctrl;
    /* Reset the structure to all 0 so it is ready to be reused.	*/
    /* NOTE: this should be a type created by mvl_vmd_type_id_create 	*/
    /*       so rt_ctrl and vmd_ctrl should be set (not NULL).		*/
    /* NOTE: this clears num_rt which indicates unused array entry.	*/
    memset (type_ctrl, 0, sizeof (MVL_TYPE_CTRL));
    }
  return (rt_ctrl);
  }

/************************************************************************/
/*			mvl_type_id_create_from_tdl			*/
/************************************************************************/
ST_INT mvl_type_id_create_from_tdl (ST_CHAR *type_name, ST_CHAR *tdl)
  {
ST_UCHAR *asn1_buf;	/* tmp ASN.1 buffer. Alloc & free here.	*/
ST_INT asn1_len;
ST_INT type_id;
RUNTIME_CTRL *rt_ctrl;

  /* Allocate minimum of 2000 bytes, more for long TDL.			*/
  /* NOTE: may want to increase mininum to reduce memory fragmentation.	*/
  asn1_len = max (2000, (ST_INT) (2 * strlen (tdl)));	/* initial len. Changed by ms_mk_asn1_type*/
  asn1_buf = (ST_UCHAR *) chk_malloc (asn1_len);

  if ((rt_ctrl = ms_tdl_to_runtime (tdl, asn1_buf, asn1_len)) == NULL)
    {
    MVL_LOG_ERR0 ("ms_tdl_to_runtime error");
    type_id = -1;	/* error	*/
    }
  else
    {
    /* Create MVL type_id. This function also performs post-processing	*/
    /* on "rt_ctrl".							*/
    type_id = mvl_vmd_type_id_create (&mvl_vmd,	/* assume global VMD	*/
             type_name, rt_ctrl);
    if (type_id < 0)
      ms_runtime_destroy (rt_ctrl);	/* clean up on error	*/
    }

  if (type_id < 0)
    MVL_LOG_ERR2 ("mvl_type_id_create_from_tdl: type creation failed for '%s', TDL=%s.", type_name, tdl);

  chk_free (asn1_buf);
  return (type_id);
  }

/************************************************************************/
/*			mvl_vmd_type_id_destroy				*/
/* Destroy type id, but only if it belongs to this VMD.			*/
/************************************************************************/
ST_RET mvl_vmd_type_id_destroy (MVL_VMD_CTRL *vmd_ctrl, ST_INT type_id)
  {
  assert (vmd_ctrl);		/* VMD must be valid	*/
  if (mvl_type_ctrl[type_id].num_rt != 0	/* != 0 indicates this type_id used.*/
      && mvl_type_ctrl[type_id].vmd_ctrl == vmd_ctrl)	/* VMD matches	*/
    {
    mvl_type_id_destroy (type_id);		/* destroy it.	*/
    return (SD_SUCCESS);
    }
  else
    return (SD_FAILURE);
  }
/************************************************************************/
/*			mvl_vmd_type_id_destroy_all			*/
/* Destroy all type ids for this VMD.					*/
/************************************************************************/
ST_RET mvl_vmd_type_id_destroy_all (MVL_VMD_CTRL *vmd_ctrl)
  {
ST_INT type_id;
ST_INT num_destroyed = 0;

  /* Try to destroy all types. If VMD doesn't match, destroy fails.	*/
  /* Count how many times destroy succeeds.				*/
  for (type_id = mvl_num_types - mvl_max_dyn.types; type_id < mvl_num_types; type_id++)
    {
    if (mvl_vmd_type_id_destroy (vmd_ctrl, type_id) == SD_SUCCESS)
      num_destroyed++;
    }
  MVLU_LOG_FLOW1 ("Number of types destroyed for this VMD=%d", num_destroyed);
  return (SD_SUCCESS);	/* at present, no way to fail.	*/
  }

/************************************************************************/
/*			mvl_type_count					*/
/* Return the current number of types in use.				*/
/************************************************************************/
ST_INT mvl_type_count ()
  {
ST_INT type_id;
ST_INT count = 0;

  /* Try to destroy all types. If VMD doesn't match, destroy fails.	*/
  /* Count how many times destroy succeeds.				*/
  for (type_id = 0; type_id < mvl_num_types; type_id++)
    {
    if (mvl_type_ctrl[type_id].num_rt != 0)	/* != 0 indicates this type_id used.*/
      count++;
    }
  return (count);
  }
/************************************************************************/
/*			mvl_vmd_type_ctrl_find				*/
/* Find type_ctrl for this type id, but only if it belongs to this VMD.	*/
/************************************************************************/
MVL_TYPE_CTRL *mvl_vmd_type_ctrl_find (MVL_VMD_CTRL *vmd_ctrl, ST_INT type_id)
  {
  assert (vmd_ctrl);		/* VMD must be valid	*/
  if (mvl_type_ctrl[type_id].num_rt != 0	/* != 0 indicates this type_id used.*/
      && mvl_type_ctrl[type_id].vmd_ctrl == vmd_ctrl)	/* VMD matches	*/
    {
    return (&mvl_type_ctrl[type_id]);
    }
  else
    return (NULL);	/* not found or doesn't belong to this VMD.	*/
  }

