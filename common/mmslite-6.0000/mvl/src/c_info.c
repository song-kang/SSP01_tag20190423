/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	1994 - 2002, All Rights Reserved		        */
/*									*/
/* MODULE NAME : c_info.c    						*/
/* PRODUCT(S)  : MMSEASE-LITE						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*	_mvl_process_info_ind						*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 02/25/04  MDE     16    Tweaked logging				*/
/* 10/28/03  JRB     15    Move mvl_free_comm_event call from		*/
/*			   mvl_info_data.. to _mvl_process_info_ind.	*/
/*			   mvl_info_data..: use num_va arg (ignored before)*/
/* 01/30/02  JRB     14    Del use of stack_sel.			*/
/* 06/22/00  MDE     13    Minor log change				*/
/* 01/21/00  MDE     12    Now use MEM_SMEM for dynamic memory		*/
/* 09/13/99  MDE     11    Added SD_CONST modifiers			*/
/* 04/07/99  MDE     10    Added  '#ifdef MVL_INFO_RPT_CLIENT'		*/
/* 02/22/99  JRB     09    BUG FIX: Don't access "va" if it is NULL.	*/
/* 01/08/99  JRB     08    Use new "bsearch" object model.		*/
/* 12/09/98  MDE     07    'mvl_info_rpt_rem_process' now frees event	*/
/* 11/16/98  MDE     06    Renamed internal functions (prefix '_')	*/
/* 07/10/98  JRB     05    Don't use net_info->rem_vmd for CLACSE, not set.*/
/* 06/30/98  MDE     04    Added more flexable CLient Info Rpt code	*/
/* 06/15/98  MDE     03    Changes to allow compile under C++		*/
/* 02/10/98  MDE     02    Removed NEST_RT_TYPES support		*/
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
#ifdef MVL_INFO_RPT_CLIENT
/************************************************************************/

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/
/************************************************************************/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/
/************************************************************************/
/*			_mvl_process_info_ind				*/
/* A info indication has been received, process it and respond		*/
/************************************************************************/

ST_VOID _mvl_process_info_ind (MVL_COMM_EVENT *event)
  {
INFO_REQ_INFO *info_ptr;
VAR_ACC_SPEC *va_spec;

/* Do a sanity check to make sure the number of data is correct		*/
  info_ptr = (INFO_REQ_INFO *) event->u.mms.dec_rslt.data_ptr;
  va_spec = &info_ptr->va_spec;
  if (va_spec->var_acc_tag == VAR_ACC_VARLIST)
    {
    if (va_spec->num_of_variables != info_ptr->num_of_acc_result) 
      {
      MVL_LOG_NERR2 ("InfoRpt Ind : NumVar (%d) vs. NumAccRslt (%d)mismatch", 
        	va_spec->num_of_variables, info_ptr->num_of_acc_result);
      _mvl_free_comm_event (event);	/* All done with this event	*/
      return;
      }
    }

/* OK, just give the info report to the user.				*/
/* NOTE: User must finish processing the event in u_mvl_info_rpt_ind,	*/
/*       because it is freed immediately after this function returns.	*/
  u_mvl_info_rpt_ind (event);
  _mvl_free_comm_event (event);	/* done with this event, free it	*/
  }

/************************************************************************/
/*			mvl_info_data_to_local				*/
/************************************************************************/
/* This function is used to convert Info Report data to local format	*/
/* The user supplies a table of variable associations, which speify the	*/
/* data type, destination, and pre/post processing functions.		*/
/* This function free's the info report resources when the conversion 	*/
/* are complete.							*/
 
ST_VOID mvl_info_data_to_local (MVL_COMM_EVENT *event,
				ST_INT num_va,
				MVL_VAR_ASSOC **info_va)
  {
INFO_REQ_INFO *info_ptr;
ST_INT j;
ACCESS_RESULT *ar_ptr;
MVL_VAR_ASSOC *va;
RUNTIME_TYPE *rt;
ST_INT rt_len;

  info_ptr = (INFO_REQ_INFO *) event->u.mms.dec_rslt.data_ptr;
  ar_ptr = info_ptr->acc_rslt_list;
  for (j = 0; j < num_va; ++j, ++ar_ptr)
    {
    va = info_va[j];
    if (!va)
      {
      MVLU_LOG_FLOW1 ("InfoRpt Ind : No VA provided for AccessResult %d, ignoring", j);
      continue;
      }

    if (ar_ptr->acc_rslt_tag == ACC_RSLT_SUCCESS)
      {

  /* Call the preprocessing function for this variable, if it exists	*/
      if (va->proc && va->proc->pre_info)
        (*va->proc->pre_info)(va, event->net_info);

    /* convert data in read buffer to local representation */
    /* and store in local buffer                           */
      if (mvl_get_runtime (va->type_id, &rt, &rt_len) == SD_SUCCESS)
        {
        va->result = ms_asn1_to_local (rt,
                                       rt_len,
                                       ar_ptr->va_data.data,
                                       ar_ptr->va_data.len,
                                       (ST_CHAR *) va->data);

        if (va->result != SD_SUCCESS)
          {
          MVL_LOG_NERR1 ("InfoRpt Ind : Data conversion error for variable '%s'", 
						va->name);
          }                        
        }
      else  /* bad type ID					       */
        {
        MVL_LOG_NERR1 ("InfoRpt Ind : Bad Type ID '%d'", va->type_id);
        va->result = SD_FAILURE;
        }
      }
    else /* access result == failure					*/
      {
      MVL_LOG_NERR0 ("InfoRpt Ind : Access Result == SD_FAILURE");
      va->result = SD_FAILURE;
      }

    if ((va->result == SD_SUCCESS) && va->proc && va->proc->post_info)
      {
      (*va->proc->post_info)(va, event->net_info);
      }
    }

  }
    
/************************************************************************/
/*			mvl_info_rpt_rem_process			*/
/************************************************************************/

#if defined(MVL_REM_VMD_SUPPORT)

MVL_VMD_CTRL *(*u_mvl_get_rem_vmd_fun) (MVL_NET_INFO *net_info);

ST_VOID mvl_info_rpt_rem_process (MVL_COMM_EVENT *event)
  {
INFO_REQ_INFO *info_ptr;
ST_INT i;
ST_INT j;
OBJECT_NAME *vobj;
VARIABLE_LIST *vl;
VAR_ACC_SPEC *va_spec;
MVL_VMD_CTRL *rem_vmd;
MVL_VAR_ASSOC **info_va;
ST_INT num_va;
MVL_NVLIST_CTRL *nvlist;
ST_CHAR *name;

/* To handle information reports, the user must have set up a remote	*/
/* VMD object control structure, and the MVL_NET_INFO must reference it.	*/ 
  if (event->net_info->rem_vmd)
    rem_vmd = (MVL_VMD_CTRL *) event->net_info->rem_vmd;
  else
    {
    if (u_mvl_get_rem_vmd_fun)
      rem_vmd = (*u_mvl_get_rem_vmd_fun) (event->net_info);
    }
  if (!rem_vmd)
    {
    MVL_LOG_NERR1 ("InfoRpt Ind : No rem_vmd for MVL_NET_INFO 0x%08lx", 
							event->net_info);
    _mvl_free_comm_event (event);	/* All done with this event	*/
    return;
    }

/* We want to get a table of pointers to VARIABLE_ASSOCIATIONS so we	*/
/* can convert the received data to local format and tell the user 	*/
/* about it.								*/

  info_ptr = (INFO_REQ_INFO *) event->u.mms.dec_rslt.data_ptr;
  va_spec = &info_ptr->va_spec;
  if (va_spec->var_acc_tag == VAR_ACC_NAMEDLIST)
    {
    vobj = &va_spec->vl_name;
    for (i = 0; i < rem_vmd->num_nvlist; ++i)
      {                           /* search for the NVL */
      if (!strcmp(rem_vmd->nvlist_tbl[i].name, vobj->obj_name.vmd_spec))
        {
        /* Found the named variable list, create the response */
        nvlist = rem_vmd->nvlist_tbl[i];
        info_va = nvlist->entries;
        num_va = nvlist->num_of_entries;
        break;
        }
      }
    if ((i >= rem_vmd->num_nvlist) ||
        (num_va != info_ptr->num_of_acc_result))
      {
      MVL_LOG_NERR2 ("InfoRpt Ind : NumVar (%d) vs. NumAccRslt (%d)mismatch", 
        	num_va, info_ptr->num_of_acc_result);

      _mvl_free_comm_event (event);	/* All done with this event	*/
      return;
      }
    }
  else if (va_spec->var_acc_tag == VAR_ACC_VARLIST)
    {
    if (va_spec->num_of_variables != info_ptr->num_of_acc_result) 
      {
      MVL_LOG_NERR2 ("InfoRpt Ind : NumVar (%d) vs. NumAccRslt (%d)mismatch", 
        	va_spec->num_of_variables, info_ptr->num_of_acc_result);
      _mvl_free_comm_event (event);	/* All done with this event	*/
      return;
      }
								 
    num_va = info_ptr->num_of_acc_result;
    info_va = 
       (MVL_VAR_ASSOC **) M_CALLOC (MSEM_GEN, num_va, sizeof (MVL_VAR_ASSOC *));

    vl = (VARIABLE_LIST *) (info_ptr + 1);
    for (j = 0; j < info_ptr->num_of_acc_result; ++j, ++vl)
      {
      if (vl->var_spec.var_spec_tag == VA_SPEC_NAMED)
        {
        vobj = &vl->var_spec.vs.name;
        name = vobj->obj_name.vmd_spec;
        for (i = 0; i < rem_vmd->num_var_assoc; ++i)
          {
          if (!strcmp (rem_vmd->var_assoc_tbl[i].name, name))
            {
  	    info_va[j] = rem_vmd->var_assoc_tbl[i];
            break;
            }
          }
        if (i >= rem_vmd->num_var_assoc)
          {
          MVL_LOG_NERR1 ("InfoRpt Ind : Variable '%s' not found", name);
          M_FREE (MSMEM_GEN, info_va);
          _mvl_free_comm_event (event);	/* All done with this event	*/
          return;
          }
        }
      else
        {
        MVL_LOG_NERR0 ("InfoRpt Ind : VA Spec not named");
        M_FREE (MSMEM_GEN, info_va);
        _mvl_free_comm_event (event);	/* All done with this event	*/
        return;
        }
      }
    }

  mvl_info_data_to_local (event, num_va, info_va);
  M_FREE (MSMEM_GEN, info_va);
  _mvl_free_comm_event (event);	/* All done with this event	*/
  }

#endif /* MVL_REM_VMD_SUPPORT */


/************************************************************************/
#endif /* MVL_INFO_RPT_CLIENT */
/************************************************************************/

