/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	1998 - 2010, All Rights Reserved				*/
/*									*/
/* MODULE NAME : s_defvl.c						*/
/* PRODUCT(S)  : MMSEASE-LITE						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*	_mvl_process_defvlist_ind					*/
/*	mvlas_defvlist_resp						*/
/*	mplas_defvlist_resp						*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev     Comments					*/
/* --------  ---  ------   -------------------------------------------	*/
/* 03/31/11  JRB	   Free tmp_obj_array if NVL already exists.	*/
/* 02/11/11  JRB	   Add checks to satisfy UCA test DsetN1.	*/
/* 09/21/10  JRB	   Chg err resp to pass IEC 61850 certification.*/
/* 03/06/08  JRB     17    Send error resp if any variable not Named.	*/
/* 04/09/07  MDE     16    Enhanced filtered logging 			*/
/* 02/21/07  JRB     15    Fix comment.					*/
/* 10/30/06  JRB     14    Use new mvl_vmd_* object handling functions.	*/
/* 01/30/06  GLB     13    Integrated porting changes for VMS           */
/* 09/21/01  JRB     12    Alloc global bufs only once at startup.	*/
/* 01/21/00  MDE     11    Now use MEM_SMEM for dynamic memory		*/
/* 09/13/99  MDE     10    Added SD_CONST modifiers			*/
/* 01/20/99  JRB     09    Use new mvl_nvl_add (simpler, more efficient)*/
/* 01/14/99  JRB     08    BUG FIX: Increment variable_list in loop.	*/
/* 12/11/98  MDE     07    Removed scope references from VA		*/
/* 11/16/98  MDE     06    Allow use of manufactured variables		*/
/* 11/16/98  MDE     05    Renamed internal functions (prefix '_')	*/
/* 07/09/98  JRB     04    Set altAcc=NULL (no alternate access).	*/
/* 06/15/98  MDE     03    Changes to allow compile under C++		*/
/* 06/05/98  MDE     02    Moved 'mvl_insert_nvlist' elsewhere 		*/
/* 04/14/98  JRB     01    MMS-LITE V4.0 Release			*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "mmsdefs.h"
#include "mms_pfil.h"
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
/*			_mvl_process_defvlist_ind				*/
/* A "defvlist" indication has been received, process it and respond	*/
/************************************************************************/

ST_VOID _mvl_process_defvlist_ind (MVL_IND_PEND *indCtrl)
  {
  indCtrl->u.defvlist.req_info =
        (DEFVLIST_REQ_INFO *) indCtrl->event->u.mms.dec_rslt.data_ptr;

  u_mvl_defvlist_ind (indCtrl);
  }

/************************************************************************/
/*			mvlas_defvlist_resp				*/
/* Most of this code doesn't care whether the Named Variable List is	*/
/* VMD-specific, Dom-specific, etc.					*/
/************************************************************************/
ST_VOID mvlas_defvlist_resp (MVL_IND_PEND *indCtrl)
  {
MVL_NVLIST_CTRL *nvlist_ctrl;	/* alloc, fill in, & then copy to list	*/
ST_INT j;
VARIABLE_LIST *variable_list;
DEFVLIST_REQ_INFO *req_info = indCtrl->u.defvlist.req_info;
OBJECT_NAME *tmp_obj_array;
#ifdef MVL_UCA
OBJECT_NAME tmp_obj_name;
ST_CHAR tmp_vl_name [MAX_IDENT_LEN + 1];
ST_CHAR *dollar_ptr;
#endif

  if (req_info->num_of_variables <= 0)
    {
    _mplas_err_resp (indCtrl,4,0);	/* Service error , other	*/
    return;
    }

#ifdef MVL_UCA
  /* IEC 61850 has special rules for this Object Name.			*/
  /* Do this to satisfy "DsetN1" test (UCA Test Procedures for Server Rev 2.2)*/
  /* NOTE: IEC 61850 ObjectReferences contain '/' or '@' but these are	*/
  /*       NEVER included in the MMS Object Name used here.		*/
  if (req_info->vl_name.object_tag == DOM_SPEC)
    {
    if (mvl_vmd_find_dom (&mvl_vmd, req_info->vl_name.domain_id) == NULL)
      {	/* Domain not found	*/
      _mplas_err_resp (indCtrl, MMS_ERRCLASS_DEFINITION, MMS_ERRCODE_INVALID_ADDRESS);
      return;					/* stop now	*/
      }
    memcpy (&tmp_obj_name, &req_info->vl_name, sizeof (OBJECT_NAME));
    tmp_obj_name.obj_name.item_id = tmp_vl_name;	/* replace name with tmp name constructed here*/
    strcpy (tmp_vl_name, req_info->vl_name.obj_name.item_id);
    dollar_ptr = strchr (tmp_vl_name, '$');
    if (dollar_ptr)
      *dollar_ptr = '\0';	/* replace $ with NULL	*/
    if (dollar_ptr == NULL || mvl_vmd_find_var (&mvl_vmd, &tmp_obj_name, NULL) == NULL)
      {	/* NVL name doesn't start with known LN Name	*/
      _mplas_err_resp (indCtrl, MMS_ERRCLASS_DEFINITION, MMS_ERRCODE_INVALID_ADDRESS);
      return;					/* stop now	*/
      }
    }
#endif /* MVL_UCA	*/

  variable_list = (VARIABLE_LIST *) (req_info + 1);
  
  /* Create temporary obj array from var list.	*/
  tmp_obj_array = (OBJECT_NAME *) M_MALLOC (MSMEM_GEN, req_info->num_of_variables * sizeof (OBJECT_NAME));
  for (j = 0;  j < req_info->num_of_variables;  j++, variable_list++)
    {
    if (variable_list->var_spec.var_spec_tag != VA_SPEC_NAMED)
      {	/* Only "Named" variables supported	*/
      _mplas_err_resp (indCtrl, MMS_ERRCLASS_DEFINITION, MMS_ERRCODE_TYPE_UNSUPPORTED);
      M_FREE (MSMEM_GEN, tmp_obj_array);	/* cleanup	*/
      return;					/* stop now	*/
      }
    memcpy (&tmp_obj_array [j], &variable_list->var_spec.vs.name, sizeof (OBJECT_NAME));
    }
    
  /* Make sure NVL object does not already exist.	*/
  nvlist_ctrl = mvl_vmd_find_nvl (&mvl_vmd, &req_info->vl_name, indCtrl->event->net_info);
  if (nvlist_ctrl)
    {			   		/* This NVL already exists.	*/
    /* return this to pass IEC 61850 certification test DsetN2.	*/
    _mplas_err_resp (indCtrl, MMS_ERRCLASS_DEFINITION, MMS_ERRCODE_OBJ_EXISTS);
    M_FREE (MSMEM_GEN, tmp_obj_array);	/* cleanup	*/
    return;
    }

  /* NOTE: Use "obj_name.vmd_spec" for an object of any scope,		*/
  /* because "obj_name" is a union of identical type objects.		*/
  nvlist_ctrl = mvl_vmd_nvl_add (&mvl_vmd, &req_info->vl_name, indCtrl->event->net_info,
                    req_info->num_of_variables, tmp_obj_array,
                    SD_TRUE);	/* copy name because it is volatile	*/

  M_FREE (MSMEM_GEN, tmp_obj_array);	/* free temporary object array.	*/

  if (nvlist_ctrl == NULL)
    {			   		/* Failed.			*/
    /* return this to pass IEC 61850 certification test DsetN1cd.	*/
    _mplas_err_resp (indCtrl, MMS_ERRCLASS_DEFINITION, MMS_ERRCODE_INVALID_ADDRESS);
    return;
    }

  nvlist_ctrl->mms_deletable = SD_TRUE;	/* MMS spec requires this*/
  nvlist_ctrl->altAcc = NULL;		/* Don't support alternate access*/

  mplas_defvlist_resp (indCtrl);
  }


/************************************************************************/
/*			mplas_defvlist_resp				*/
/************************************************************************/
ST_VOID mplas_defvlist_resp (MVL_IND_PEND *indCtrl)
  {
ST_RET rc;

  /* Build a "defvlist" response (no response info).			*/
  rc = mpl_defvlist_resp (indCtrl->event->u.mms.dec_rslt.id);
  if (rc == SD_SUCCESS)
    mpl_resp_log (indCtrl, NULL);

  _mvl_send_resp_i (indCtrl, rc);
  }

