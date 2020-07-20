/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	2004-2011 All Rights Reserved					*/
/*									*/
/* MODULE NAME : userleaf.c						*/
/* PRODUCT(S)  : MMS-EASE-LITE						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*	User customized "leaf" functions.				*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*			u_set_all_leaf_functions			*/
/*			u_custom_wr_ind					*/
/*									*/
/* NOTE: Leaf functions for IEC 61850 Controls must check the flag	*/
/*       "is_61850_control" set by "_mvl_process_write_ind".		*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev     Comments					*/
/* --------  ---  ------   -------------------------------------------  */
/* 02/17/12  JRB	   Use strncat_maxstrlen.			*/
/* 01/11/12  JRB	   Add LGOS leaf functions to set attributes	*/
/*			   based on GOOSE Subscriptions.		*/
/* 04/25/11  JRB	   Add "*CheckRange" to check range of Enums.	*/
/* 04/19/11  JRB	   Don't allow write to ST, MX, SG, EX attributes*/
/* 03/23/11  JRB	   Check new is_61850_control flag.		*/
/*			   u_ctl_sbo_rd_ind: add USE_RECURSIVE_READ option*/
/*			   u_set_all..: set special func ptrs for Health*/
/* 12/08/10  JRB	   u_set_all_leaf_functions: add arg.		*/
/* 12/05/08  JRB    15     Move u_custom_rd_ind to "userleaf2.c"	*/
/*			   (easier to customize or replace).		*/
/* 05/08/08  JRB    14     ..oper_other.. do not call ..oper_ctlval..	*/
/* 05/06/08  JRB    13     Don't set Error/AddCause in leaf functions,	*/
/*			   should be set when error occurs.		*/
/* 04/28/08  JRB    12     Simplify u_set_all_leaf_functions & set	*/
/*			   special funct for leafs in setMag struct.	*/
/*			   Improve change detection in set_rpt_reason.	*/
/* 02/21/07  RKR    11     more work on u_custom_rd                     */
/* 02/13/07  RKR    10     changed u_custom_rd_ind to emulate data chngs*/
/* 11/22/06  JRB    09     Fill in LastApplError struct if control fails*/
/* 08/17/06  JRB    08     Add set_rpt_reason to detect data, quality	*/
/*			   changes for reports.				*/
/* 09/20/05  JRB    07     Del assert in u_custom_wr_ind, doesn't work now.*/
/* 09/13/05  JRB    06     Add u_ctl_sbow_comp_wr_ind for writing "SBOw".*/		
/*			   Simplify ..nest_level computation.		*/
/*			   Del unneeded memcpy in "wr_ind" functs.	*/
/* 06/03/05  JRB    05     Add chk for (map_head_sorted == NULL).	*/
/* 11/04/04  JRB    04     Replace u_sbo*, u_direct* with u_ctl_oper_*	*/
/*			   functs that check "Loc$stVal" & "ctlModel" vals.*/
/*			   Del unneeded found_sbo flag.			*/
/* 			   Add u_ctl_sbo_rd_ind leaf function to replace*/
/*			   mvl61850_sbo_select_rd_ind deleted from lib.	*/
/* 09/20/04  JRB    03     Add SBO, Oper, Cancel leaf functions.	*/
/* 08/05/04  JRB    02     Use new user header file.			*/
/* 06/08/04  JRB    01     Initial Revision.				*/
/************************************************************************/
#include "glbtypes.h"
#include "sysincs.h"
#include "usermap.h"	/* for DATA_MAP, etc.	*/
#include "slog.h"
#include "mvl_defs.h"
#include "mvl_uca.h"
#include "mvl_log.h"
#include "stdtime.h"
#if defined(GOOSE_RX_SUPP)
#include "iec_demo.h"
#endif
#include "str_util.h"	/* for strncat_maxstrlen, etc.			*/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

static ST_INT prim_data_compare (ST_VOID *new_data, ST_VOID *old_data, RUNTIME_TYPE *rt_type);

/************************************************************************/
/*			u_set_all_leaf_functions			*/
/* User customizable function to set all leaf functions for a data type.*/
/* NOTE: leaf functions never used for RT_STR_START, RT_ARR_START,	*/
/*       etc. but they're set anyway because it's easier to set all.	*/
/* NOTE: this function may be customized for each application.		*/
/*       It should be placed in a "user" module.			*/
/************************************************************************/
ST_RET u_set_all_leaf_functions (RUNTIME_CTRL *rt_ctrl,
	SCL_LNTYPE *scl_lntype)		/* LNodeType info from SCL file.*/
  {
RUNTIME_TYPE *rt_type;
/* These are indices into the arrays of Read & Write leaf function pointers.*/
ST_RTINT idx_u_custom_rd_ind;
ST_RTINT idx_u_custom_wr_ind;
ST_RTINT idx_u_ctl_sbo_rd_ind;	/* index for SBO Select Read ind funct.	*/
ST_RTINT idx_u_no_write_allowed;	/* index for Write not allowed ind funct.*/
ST_RTINT idx_u_ctl_oper_ctlval_wr_ind;
ST_RTINT idx_u_ctl_oper_other_wr_ind;
ST_RTINT idx_u_cancel_comp_wr_ind;
ST_RTINT idx_u_ctl_sbow_comp_wr_ind;
ST_RTINT idx_mvl61850_beh_stval_rd_ind;
ST_RTINT idx_lln0_health_stval_rd_ind;
ST_RTINT idx_lln0_health_q_rd_ind;
ST_RTINT idx_lln0_health_t_rd_ind;
ST_RTINT idx_u_LGOS_NdsCom_rd_ind;
ST_RTINT idx_u_LGOS_LastStNum_rd_ind;
ST_RTINT idx_u_LGOS_ConfRevNum_rd_ind;
ST_RTINT idx_u_LGOS_St_rd_ind;
ST_RTINT idx_u_LGOS_SimSt_rd_ind;
ST_INT j;
ST_CHAR *comp_name;
ST_CHAR branch_name[MAX_IDENT_LEN+1];	/* use to track struct nesting	*/

  branch_name[0] = '\0';		/* start with empty string	*/

  /* Find the customized rd and wr leaf functions to use for this	*/
  /* application. Stop if any function cannot be found.			*/
  idx_u_custom_rd_ind = mvlu_find_rd_ind_fun_index ("u_custom_rd_ind");
  if (idx_u_custom_rd_ind < 0)
    return (SD_FAILURE);	/* error already logged	*/

  idx_u_custom_wr_ind = mvlu_find_wr_ind_fun_index ("u_custom_wr_ind");
  if (idx_u_custom_wr_ind < 0)
    return (SD_FAILURE);	/* error already logged	*/

  idx_u_ctl_sbo_rd_ind = mvlu_find_rd_ind_fun_index ("u_ctl_sbo_rd_ind");
  if (idx_u_ctl_sbo_rd_ind < 0)
    return (SD_FAILURE);	/* error already logged	*/

  idx_u_no_write_allowed = mvlu_find_wr_ind_fun_index ("u_no_write_allowed");
  if (idx_u_no_write_allowed < 0)
    return (SD_FAILURE);	/* error already logged	*/

  idx_u_ctl_oper_ctlval_wr_ind = mvlu_find_wr_ind_fun_index ("u_ctl_oper_ctlval_wr_ind");
  if (idx_u_ctl_oper_ctlval_wr_ind < 0)
    return (SD_FAILURE);	/* error already logged	*/

  idx_u_ctl_oper_other_wr_ind = mvlu_find_wr_ind_fun_index ("u_ctl_oper_other_wr_ind");
  if (idx_u_ctl_oper_other_wr_ind < 0)
    return (SD_FAILURE);	/* error already logged	*/

  idx_u_cancel_comp_wr_ind = mvlu_find_wr_ind_fun_index ("u_cancel_comp_wr_ind");
  if (idx_u_cancel_comp_wr_ind < 0)
    return (SD_FAILURE);	/* error already logged	*/

  idx_mvl61850_beh_stval_rd_ind = mvlu_find_rd_ind_fun_index ("mvl61850_beh_stval_rd_ind");
  if (idx_mvl61850_beh_stval_rd_ind < 0)
    return (SD_FAILURE);	/* error already logged	*/

  idx_lln0_health_stval_rd_ind = mvlu_find_rd_ind_fun_index ("lln0_health_stval_rd_ind");
  if (idx_lln0_health_stval_rd_ind < 0)
    return (SD_FAILURE);	/* error already logged	*/

  idx_lln0_health_q_rd_ind = mvlu_find_rd_ind_fun_index ("lln0_health_q_rd_ind");
  if (idx_lln0_health_q_rd_ind < 0)
    return (SD_FAILURE);	/* error already logged	*/

  idx_lln0_health_t_rd_ind = mvlu_find_rd_ind_fun_index ("lln0_health_t_rd_ind");
  if (idx_lln0_health_t_rd_ind < 0)
    return (SD_FAILURE);	/* error already logged	*/

  idx_u_ctl_sbow_comp_wr_ind = mvlu_find_wr_ind_fun_index ("u_ctl_sbow_comp_wr_ind");
  if (idx_u_ctl_sbow_comp_wr_ind < 0)
    return (SD_FAILURE);	/* error already logged	*/

  /* Find leaf function indices for LGOS attributes.	*/
  idx_u_LGOS_NdsCom_rd_ind = mvlu_find_rd_ind_fun_index ("u_LGOS_NdsCom_rd_ind");
  if (idx_u_LGOS_NdsCom_rd_ind < 0)
    return (SD_FAILURE);	/* error already logged	*/
  idx_u_LGOS_LastStNum_rd_ind = mvlu_find_rd_ind_fun_index ("u_LGOS_LastStNum_rd_ind");
  if (idx_u_LGOS_LastStNum_rd_ind < 0)
    return (SD_FAILURE);	/* error already logged	*/
  idx_u_LGOS_St_rd_ind = mvlu_find_rd_ind_fun_index ("u_LGOS_St_rd_ind");
  if (idx_u_LGOS_St_rd_ind < 0)
    return (SD_FAILURE);	/* error already logged	*/
  idx_u_LGOS_SimSt_rd_ind = mvlu_find_rd_ind_fun_index ("u_LGOS_SimSt_rd_ind");
  if (idx_u_LGOS_SimSt_rd_ind < 0)
    return (SD_FAILURE);	/* error already logged	*/
  idx_u_LGOS_ConfRevNum_rd_ind = mvlu_find_rd_ind_fun_index ("u_LGOS_ConfRevNum_rd_ind");
  if (idx_u_LGOS_ConfRevNum_rd_ind < 0)
    return (SD_FAILURE);	/* error already logged	*/

  /* Set the leaf function index for every leaf.		*/
  /* KEY: Use "branch_name", which is updated when each structure starts*/
  /*      and ends, to keep track of structure nesting.			*/
  for (j = 0, rt_type = rt_ctrl->rt_first;
       j < rt_ctrl->rt_num;
       j++, rt_type++)
    {
    comp_name = ms_comp_name_find(rt_type);
    if ((rt_type->el_tag == RT_STR_START))
      {
      /* Top level struct doesn't have component name. OK. Don't need it.*/
      if (rt_type > rt_ctrl->rt_first)
        {
        assert (strlen(comp_name) > 0);
        strncat_maxstrlen (branch_name, "$", MAX_IDENT_LEN);
        strncat_maxstrlen (branch_name, comp_name, MAX_IDENT_LEN);
        }
      }
    else if (rt_type->el_tag == RT_STR_END)
      {
      mvlu_trim_branch_name (branch_name);
      }
    else if (strcmp (scl_lntype->lnClass, "LGOS") == 0)
      {		/* handle any LGOS components	*/
      if (strcmp (comp_name, "stVal") == 0 &&
          strcmp (branch_name, "$ST$NdsCom") == 0)
        {
        rt_type->mvluTypeInfo.rdIndFunIndex = idx_u_LGOS_NdsCom_rd_ind;
        rt_type->mvluTypeInfo.wrIndFunIndex = idx_u_no_write_allowed;
        }
      else if (strcmp (comp_name, "stVal") == 0 &&
               strcmp (branch_name, "$ST$LastStNum") == 0)
        {
        rt_type->mvluTypeInfo.rdIndFunIndex = idx_u_LGOS_LastStNum_rd_ind;
        rt_type->mvluTypeInfo.wrIndFunIndex = idx_u_no_write_allowed;
        }
      else if (strcmp (comp_name, "stVal") == 0 &&
               strcmp (branch_name, "$ST$St") == 0)
        {
        rt_type->mvluTypeInfo.rdIndFunIndex = idx_u_LGOS_St_rd_ind;
        rt_type->mvluTypeInfo.wrIndFunIndex = idx_u_no_write_allowed;
        }
      else if (strcmp (comp_name, "stVal") == 0 &&
               strcmp (branch_name, "$ST$SimSt") == 0)
        {
        rt_type->mvluTypeInfo.rdIndFunIndex = idx_u_LGOS_SimSt_rd_ind;
        rt_type->mvluTypeInfo.wrIndFunIndex = idx_u_no_write_allowed;
        }
      else if (strcmp (comp_name, "stVal") == 0 &&
               strcmp (branch_name, "$ST$ConfRevNum") == 0)
        {
        rt_type->mvluTypeInfo.rdIndFunIndex = idx_u_LGOS_ConfRevNum_rd_ind;
        rt_type->mvluTypeInfo.wrIndFunIndex = idx_u_no_write_allowed;
        }
      else
        {			/* for any other LGOS component	*/
        rt_type->mvluTypeInfo.rdIndFunIndex = idx_u_custom_rd_ind;
        rt_type->mvluTypeInfo.wrIndFunIndex = idx_u_custom_wr_ind;
        }
      }
    else if (strcmp (comp_name, "SBO") == 0)
      {
      rt_type->mvluTypeInfo.rdIndFunIndex = idx_u_ctl_sbo_rd_ind;
      rt_type->mvluTypeInfo.wrIndFunIndex = idx_u_no_write_allowed;
      }
    else if (strcmp (comp_name, "ctlModel") == 0)
      {
      rt_type->mvluTypeInfo.rdIndFunIndex = idx_u_custom_rd_ind;	/* use default rd leaf function*/
      rt_type->mvluTypeInfo.wrIndFunIndex = idx_u_no_write_allowed;
      }
    else if (strcmp (comp_name, "sboClass") == 0)
      {
      rt_type->mvluTypeInfo.rdIndFunIndex = idx_u_custom_rd_ind;	/* use default rd leaf function*/
      rt_type->mvluTypeInfo.wrIndFunIndex = idx_u_no_write_allowed;
      }
    else if (strcmp (comp_name, "stVal") == 0 &&
             strstr (branch_name, "$Beh") != NULL)
      {				/* "stVal" inside "Beh" struct	*/
      rt_type->mvluTypeInfo.rdIndFunIndex = idx_mvl61850_beh_stval_rd_ind;	/* special rd for Beh$stVal*/
      rt_type->mvluTypeInfo.wrIndFunIndex = idx_u_no_write_allowed;
      }
    else if (strcmp (comp_name, "stVal") == 0 &&
             strcmp (branch_name, "$ST$Health") == 0 &&
             strcmp (scl_lntype->lnClass, "LLN0") == 0)
      {				/* "stVal" inside "Health" struct in LLN0	*/
      rt_type->mvluTypeInfo.rdIndFunIndex = idx_lln0_health_stval_rd_ind;	/* special rd for LLN0$ST$Health$stVal*/
      rt_type->mvluTypeInfo.wrIndFunIndex = idx_u_no_write_allowed;
      }
    else if (strcmp (comp_name, "q") == 0 &&
             strcmp (branch_name, "$ST$Health") == 0 &&
             strcmp (scl_lntype->lnClass, "LLN0") == 0)
      {				/* "q" inside "Health" struct in LLN0	*/
      rt_type->mvluTypeInfo.rdIndFunIndex = idx_lln0_health_q_rd_ind;	/* special rd for LLN0$ST$Health$q*/
      rt_type->mvluTypeInfo.wrIndFunIndex = idx_u_no_write_allowed;
      }
    else if (strcmp (comp_name, "t") == 0 &&
             strcmp (branch_name, "$ST$Health") == 0 &&
             strcmp (scl_lntype->lnClass, "LLN0") == 0)
      {				/* "t" inside "Health" struct in LLN0	*/
      rt_type->mvluTypeInfo.rdIndFunIndex = idx_lln0_health_t_rd_ind;	/* special rd for LLN0$ST$Health$t*/
      rt_type->mvluTypeInfo.wrIndFunIndex = idx_u_no_write_allowed;
      }
    else if (strcmp (comp_name, "ctlVal") == 0 &&
             strstr (branch_name, "$Oper") != NULL)
      {				/* "ctlVal" inside "Oper" struct	*/
      rt_type->mvluTypeInfo.rdIndFunIndex = idx_u_custom_rd_ind;
      rt_type->mvluTypeInfo.wrIndFunIndex = idx_u_ctl_oper_ctlval_wr_ind;
      }
    else if (strstr (branch_name, "$Oper$setMag") != NULL)
      {				/* any component inside "Oper$setMag" struct*/
      /* NOTE: you may want a new special function for "setMag" components.*/
      rt_type->mvluTypeInfo.rdIndFunIndex = idx_u_custom_rd_ind;
      rt_type->mvluTypeInfo.wrIndFunIndex = idx_u_ctl_oper_ctlval_wr_ind;
      }
    else if (strstr (branch_name, "$Oper") != NULL)
      {	/* any other component inside "Oper" struct (not ctlVal or setMag)*/
      rt_type->mvluTypeInfo.rdIndFunIndex = idx_u_custom_rd_ind;
      rt_type->mvluTypeInfo.wrIndFunIndex = idx_u_ctl_oper_other_wr_ind;
      }
    else if (strstr (branch_name, "$Cancel") != NULL)
      {				/* any component inside "Cancel" struct	*/
      rt_type->mvluTypeInfo.rdIndFunIndex = idx_u_custom_rd_ind;
      rt_type->mvluTypeInfo.wrIndFunIndex = idx_u_cancel_comp_wr_ind;
      }
    else if (strstr (branch_name, "$SBOw") != NULL)
      {				/* any component inside "SBOw" struct	*/
      rt_type->mvluTypeInfo.rdIndFunIndex = idx_u_custom_rd_ind;
      rt_type->mvluTypeInfo.wrIndFunIndex = idx_u_ctl_sbow_comp_wr_ind;
      }
    else
      {				/* default for any other component	*/
      rt_type->mvluTypeInfo.rdIndFunIndex = idx_u_custom_rd_ind;
      rt_type->mvluTypeInfo.wrIndFunIndex = idx_u_custom_wr_ind;
      }

    /* IEC 61850-7-2 Ed 1, Table 18 says Attributes of ST, MX, SG,	*/
    /* and EX are not writable. Use the "read" function set above,	*/
    /* but make sure "write" is not allowed for these FC.		*/
    if (ms_is_rt_prim (rt_type))
      {
      if (strncmp (branch_name, "$ST", 3) == 0 ||
          strncmp (branch_name, "$MX", 3) == 0 ||
          strncmp (branch_name, "$SG", 3) == 0 ||
          strncmp (branch_name, "$EX", 3) == 0)
        {
        rt_type->mvluTypeInfo.wrIndFunIndex = idx_u_no_write_allowed;
        }
      }
    }	/* end "for" loop	*/

  return (SD_SUCCESS);
  }

/************************************************************************/
/*			set_rpt_reason					*/
/* Check if this leaf is part of a scan for Report changes.		*/
/* If so, check for data-change, quality-change, etc & set rpt_reason.	*/
/* CRITICAL: this function should be modified to use the best available	*/
/*   change detection for each device.					*/
/************************************************************************/
ST_VOID set_rpt_reason (MVLU_RD_VA_CTRL *mvluRdVaCtrl)
  {
MVL_VAR_ASSOC *va;
ST_INT prim_offset;
ST_CHAR *old_data;
ST_CHAR *new_data;

  /* If scan_va_done_fun is set, this read is part of a RPT scan	*/
  /* so check for changes & set "rpt_reason".				*/
  /* NOTE: checking scan_va_done_fun is not critical, but it helps avoid*/
  /*   wasting time checking for changes on non-report data.		*/
  if (mvluRdVaCtrl->indCtrl->scan_va_done_fun)
    {
    va = mvluRdVaCtrl->rdVaCtrl->va;
    /* Need offset to find this leaf's data in "last_data" buffer.	*/
    prim_offset = mvluRdVaCtrl->primData - (ST_CHAR *) va->data;
    old_data = (ST_CHAR *) va->last_data + prim_offset;
    new_data = mvluRdVaCtrl->primData;

    /* Compare new data to old data for this primitive.			*/
    /* CRITICAL: DO NOT use "memcmp" here. It may compare padding bits	*/
    /*     and bytes which may contain random data.			*/
    if (prim_data_compare (new_data, old_data, mvluRdVaCtrl->rt) != 0)
      {
      if (strcmp (ms_comp_name_find(mvluRdVaCtrl->rt), "q") == 0)
        {	/* set quality-change bit in rpt_reason	*/
        va->rpt_reason |= MVLU_TRGOPS_QUALITY;		/* quality-change*/
        }
      else
        {	/* set data-change bit in rpt_reason	*/
        va->rpt_reason |= MVLU_TRGOPS_DATA;		/* data-change	*/
        }
      }
    }
  }
    
/************************************************************************/
/*			u_custom_wr_ind					*/
/************************************************************************/
ST_VOID u_custom_wr_ind (MVLU_WR_VA_CTRL *mvluWrVaCtrl)
  {
MVL_VAR_ASSOC *base_var;
DATA_MAP_HEAD *map_head_sorted;	/* head of array of (DATA_MAP *) for this var.*/
DATA_MAP *map_entry;		/* DATA_MAP for this bit of data.	*/
ST_RET retcode = SD_SUCCESS;
ST_CHAR *comp_name;

  /* Use new "prim_num" to find (DATA_MAP *).	*/
  base_var = mvluWrVaCtrl->wrVaCtrl->va->base_va;

  assert (base_var->use_static_data);	/* funct only works in this mode*/

  map_head_sorted = (DATA_MAP_HEAD *) base_var->user_info;	/* it was saved here*/
  if (map_head_sorted == NULL)
    {		/* THIS IS A SERIOUS ERROR	*/
    MVL_LOG_ERR1 ("DATA mapping error: variable '%s' not mapped", base_var->name);
    retcode = SD_FAILURE;
    }
  else if (mvluWrVaCtrl->prim_num >= map_head_sorted->map_count)
    {		/* THIS IS A SERIOUS ERROR	*/
    MVL_LOG_ERR1 ("DATA mapping error: prim_num=%d exceeds limit", mvluWrVaCtrl->prim_num);
    retcode = SD_FAILURE;
    }
  else
    {
    /* NOTE: this example not actually using the map_entry, but real leaf
     * function should use map_entry (especially map_entry->usr_data_info)
     * to determine how/where to write the data.
     */
    map_entry = map_head_sorted->map_arr [mvluWrVaCtrl->prim_num];
    }

  comp_name = ms_comp_name_find (mvluWrVaCtrl->rt);

  /* For some attributes, make sure value is in range.	*/
  if (strcmp (comp_name, "hvRef") == 0)
    {
    if (hvRefCheckRange (*(ST_INT8 *)mvluWrVaCtrl->primData))
      {
      mvluWrVaCtrl->wrVaCtrl->failure = ARE_OBJ_VALUE_INVALID;
      retcode = SD_FAILURE;	/* 'negative' resp*/
      }
    }
  else if (strcmp (comp_name, "cmdQual") == 0)
    {
    if (cmdQualCheckRange (*(ST_INT8 *)mvluWrVaCtrl->primData))
      {
      mvluWrVaCtrl->wrVaCtrl->failure = ARE_OBJ_VALUE_INVALID;
      retcode = SD_FAILURE;	/* 'negative' resp*/
      }
    }
  else if (strcmp (comp_name, "SIUnit") == 0)
    {
    if (SIUnitCheckRange (*(ST_INT8 *)mvluWrVaCtrl->primData))
      {
      mvluWrVaCtrl->wrVaCtrl->failure = ARE_OBJ_VALUE_INVALID;
      retcode = SD_FAILURE;	/* 'negative' resp*/
      }
    }
  else if (strcmp (comp_name, "multiplier") == 0)
    {
    if (multiplierCheckRange (*(ST_INT8 *)mvluWrVaCtrl->primData))
      {
      mvluWrVaCtrl->wrVaCtrl->failure = ARE_OBJ_VALUE_INVALID;
      retcode = SD_FAILURE;	/* 'negative' resp*/
      }
    }
  else if (strcmp (comp_name, "phsRef") == 0)
    {
    if (phsRefCheckRange (*(ST_INT8 *)mvluWrVaCtrl->primData))
      {
      mvluWrVaCtrl->wrVaCtrl->failure = ARE_OBJ_VALUE_INVALID;
      retcode = SD_FAILURE;	/* 'negative' resp*/
      }
    }
  else if (strcmp (comp_name, "setCharact") == 0)
    {
    if (setCharactCheckRange (*(ST_INT8 *)mvluWrVaCtrl->primData))
      {
      mvluWrVaCtrl->wrVaCtrl->failure = ARE_OBJ_VALUE_INVALID;
      retcode = SD_FAILURE;	/* 'negative' resp*/
      }
    }

  /* NOTE: the data for this leaf has already been copied to the variable.*/
  /*       To get a ptr to the data, use (mvluWrVaCtrl->primData) OR	*/
  /*       ((ST_CHAR *) base_var->data + mvluWrVaCtrl->prim_offset_base)*/

  /* retcode is always SD_SUCCESS, unless mapping error detected.*/
  mvlu_wr_prim_done (mvluWrVaCtrl, retcode);
  }

/************************************************************************/
/*			fillLastApplError				*/
/* Fill in LastApplError members just in case write fails.		*/
/************************************************************************/
static ST_VOID fillLastApplError (MVLU_WR_VA_CTRL *mvluWrVaCtrl)
  {
ST_CHAR *comp_name;
  comp_name = ms_comp_name_find (mvluWrVaCtrl->rt);

  if (strcmp (comp_name, "ctlNum") == 0)
    mvluWrVaCtrl->wrVaCtrl->LastApplError.ctlNum = *(ST_UINT8 *) mvluWrVaCtrl->primData;
  else if (strcmp (comp_name, "orCat") == 0)
    mvluWrVaCtrl->wrVaCtrl->LastApplError.Origin.orCat = *(ST_INT8 *) mvluWrVaCtrl->primData;
  else if (strcmp (comp_name, "orIdent") == 0)
    /* NOTE: copying el_size bytes works for any data type.		*/
    memcpy (&mvluWrVaCtrl->wrVaCtrl->LastApplError.Origin.orIdent,
        mvluWrVaCtrl->primData,
        mvluWrVaCtrl->rt->el_size);
  }
/************************************************************************/
/*			u_ctl_oper_ctlval_wr_ind			*/
/* For "controlled" component of "Oper" (ctlVal or setMag).		*/
/* Chk value of "Loc$stVal" to determine if remote control allowed.	*/
/* Chk value of "ctlModel" to determine type of control.		*/
/************************************************************************/
ST_VOID u_ctl_oper_ctlval_wr_ind (MVLU_WR_VA_CTRL *mvluWrVaCtrl)
  {
ST_RET retcode;

  if (!mvluWrVaCtrl->wrVaCtrl->is_61850_control)
    {
    /* ERROR: Not writing whole "Oper" structure.	*/
    mvluWrVaCtrl->wrVaCtrl->failure = ARE_OBJ_ACCESS_DENIED;	/* send in resp*/
    mvlu_wr_prim_done (mvluWrVaCtrl, SD_FAILURE);
    return;
    }

  /* Check ctlModel, etc. to determine if control is right type & in right state.*/
  retcode = mvl61850_ctl_chk_state (mvluWrVaCtrl);

  /* Many ways to fail. retcode==SD_FAILURE if "local" mode (Loc$stVal==FALSE)
   * if ctlModel is not appropriate,
   * if SBO Select never done or timed out,
   * if NOT writing whole "Oper" structure, etc.
   */

  mvlu_wr_prim_done (mvluWrVaCtrl, retcode);
  return;
  }

/************************************************************************/
/*			u_ctl_oper_other_wr_ind				*/
/* For "other" components of "Oper" (origin, ctlNum, etc.).		*/ 
/************************************************************************/
ST_VOID u_ctl_oper_other_wr_ind (MVLU_WR_VA_CTRL *mvluWrVaCtrl)
  {
ST_CHAR *comp_name;
  if (!mvluWrVaCtrl->wrVaCtrl->is_61850_control)
    {
    /* ERROR: Not writing whole "Oper" structure.	*/
    mvluWrVaCtrl->wrVaCtrl->failure = ARE_OBJ_ACCESS_DENIED;	/* send in resp*/
    mvlu_wr_prim_done (mvluWrVaCtrl, SD_FAILURE);
    return;
    }

  comp_name = ms_comp_name_find (mvluWrVaCtrl->rt);
  if (strcmp (comp_name, "orCat") == 0)
    {	/* for "orCat" make sure value is in range	*/
    if (orCatCheckRange (*(ST_INT8 *)mvluWrVaCtrl->primData))
      {
      mvluWrVaCtrl->wrVaCtrl->failure = ARE_OBJ_VALUE_INVALID;
      mvlu_wr_prim_done (mvluWrVaCtrl, SD_FAILURE);
      return;
      }
    }

  /* Fill in LastApplError members just in case write fails.	*/
  fillLastApplError (mvluWrVaCtrl);

  /* For this example, just return SD_SUCCESS.			*/
  mvlu_wr_prim_done (mvluWrVaCtrl, SD_SUCCESS);
  }

/************************************************************************/
/*			u_cancel_comp_wr_ind				*/
/* Write a component of the "Cancel" structure (to cancel SBO control).	*/
/* NOTE: this function just writes the data. The actual operation of	*/
/*	cancelling the SBO Select, is done automatically in		*/
/*	mvlu_wr_prim_done.						*/
/************************************************************************/
ST_VOID u_cancel_comp_wr_ind (MVLU_WR_VA_CTRL *mvluWrVaCtrl)
  {
  if (!mvluWrVaCtrl->wrVaCtrl->is_61850_control)
    {
    /* ERROR: Not writing whole "Cancel" structure.	*/
    mvluWrVaCtrl->wrVaCtrl->failure = ARE_OBJ_ACCESS_DENIED;	/* send in resp*/
    mvlu_wr_prim_done (mvluWrVaCtrl, SD_FAILURE);
    return;
    }

  /* Fill in LastApplError members just in case write fails.	*/
  fillLastApplError (mvluWrVaCtrl);

  /* Chk sboCtrl (set by startElWrites if client writing "Cancel").	*/
  if (mvluWrVaCtrl->wrVaCtrl->sboCtrl)
    {
    mvlu_wr_prim_done (mvluWrVaCtrl, SD_SUCCESS);
    }
  else	/* SBO Select never done or timed out. DO NOT cancel control	*/
    mvlu_wr_prim_done (mvluWrVaCtrl, SD_FAILURE);

  return;      
  }

#if defined(USE_RECURSIVE_READ)
/************************************************************************/
/*			_ctl_sbo_rd_finish				*/
/* Continue normal processing for SBO read.				*/
/************************************************************************/
static ST_VOID _ctl_sbo_rd_finish (MVLU_RD_VA_CTRL *mvluRdVaCtrl)
  {
MVL_SBO_CTRL *sboCtrl;

  /* This library function does all the "required" SBO checks.	*/
  sboCtrl = mvl61850_ctl_chk_sbo (mvluRdVaCtrl);
  if (sboCtrl)
    strcpy (mvluRdVaCtrl->primData, sboCtrl->sbo_var);	/* 'positive' resp*/
  else
    mvluRdVaCtrl->primData [0] = '\0';	/* 'negative' resp (empty string)*/

#if 0	/* enable this code & add additional checks if necessary	*/
  if (sboCtrl)
    {	/* SISCO checks OK.	*/
    ST_BOOLEAN user_detected_error = SD_FALSE;

    /* Add code here to perform additional checks. Set "user_detected_error = SD_TRUE" if problem found.*/

    if (user_detected_error)
      {		/* user found problem, so cleanup.	*/
      mvluRdVaCtrl->primData [0] = '\0';	/* 'negative' resp (empty string)*/
      mvlu_sbo_ctrl_free (sboCtrl);
      }
    }
#endif

  /* Always return SUCCESS. Data is 'empty string' if select not allowed.*/
  mvlu_rd_prim_done (mvluRdVaCtrl, SD_SUCCESS);
  }

/************************************************************************/
/*			_ctl_sbo_rd_continue				*/
/* Called when scan started by "u_ctl_sbo_rd_ind" completes.		*/
/* Finds original Read Indication and calls "_ctl_sbo_rd_finish"	*/
/* to finish processing it.						*/
/* RETURNS:	SD_SUCCESS (caller ignores return anyway)		*/
/************************************************************************/
static ST_RET _ctl_sbo_rd_continue (MVL_IND_PEND *indCtrl)
  {
MVLU_RD_VA_CTRL *mvluRdVaCtrl;	/* pointer to original Read request	*/
ST_RET retcode;

  /* CRITICAL: Get pointer to original request BEFORE freeing indCtrl	*/
  /*           (saved in "indCtrl->usr" when scan started).		*/
  mvluRdVaCtrl = (MVLU_RD_VA_CTRL *) indCtrl->usr;

  /* Get result of scan and free "indCtrl".	*/
  retcode = mvlu_scan_read_lov_cleanup (indCtrl);
 
  /* Scan is done and cleaned up. Finish processing Read Indication.	*/
  if (retcode)
    mvlu_rd_prim_done (mvluRdVaCtrl, retcode);
  else
    _ctl_sbo_rd_finish (mvluRdVaCtrl);	/* finish processing	*/

  return (SD_SUCCESS);	/* NOTE: ignored by caller	*/
  }

/************************************************************************/
/*			_ctl_sbo_rd_scan_start				*/
/* Called by u_ctl_sbo_rd_ind only if USE_RECURSIVE_READ defined.	*/
/* Starts a scan to read important IEC 61850 Control "CF" attributes.	*/
/* If error is returned (!SD_SUCCESS), caller should immediately call	*/
/* mvlu_rd_prim_done or mvlu_wr_prim_done to trigger error response.	*/
/************************************************************************/
static ST_RET _ctl_sbo_rd_scan_start (
	MVLU_RD_VA_CTRL *mvluRdVaCtrl,	/* original read ind control	*/
					/* Passed to (*scan_done_fun)	*/
					/* in "indCtrl->usr".		*/
	ST_RET (*scan_done_fun)(MVL_IND_PEND *indCtrl))
				/* call this when scan completes	*/
  {
ST_RET retcode = SD_SUCCESS;
MVL_VAR_ASSOC *var_new;		/* new var created here. Saved in var_array.*/
MVL_VAR_ASSOC *var_array [4];
MVL_SCOPE *scope_array [4];

ST_INT num_var;			/* number of variables to read	*/
ST_CHAR sboClassName   [MAX_IDENT_LEN+1];		/* "CF$...$sboClass"	*/
ST_CHAR sboTimeoutName [MAX_IDENT_LEN+1];		/* "CF$...$sboTimeout"	*/
ST_CHAR ctlModelName   [MAX_IDENT_LEN+1];		/* "CF$...$ctlModel"	*/
ST_CHAR *LocName = "ST$Loc$stVal";		/* always same		*/

MVL_VAR_ASSOC *ctl_var;	/* "control" var being read or written	*/
MVL_SCOPE     *ctl_var_scope;	/* "control" var scope		*/

  ctl_var       = mvluRdVaCtrl->rdVaCtrl->va;
  ctl_var_scope = &mvluRdVaCtrl->rdVaCtrl->va_scope;

  num_var = 0;
    
  /* Create 4 temporary vars.	*/
  if (mvl61850_mkname_ctlmodel (ctl_var->name, ctlModelName, MAX_IDENT_LEN) == SD_SUCCESS
      && (var_new = mvlu_flat_var_create (ctl_var->base_va, ctlModelName)) != NULL)
    {
    var_array   [num_var] = var_new;
    scope_array [num_var] = ctl_var_scope;	/* tmp var scope always same*/
    num_var++;
    }
  if (mvl61850_mkname_sboclass (ctl_var->name, sboClassName, MAX_IDENT_LEN) == SD_SUCCESS
      && (var_new = mvlu_flat_var_create (ctl_var->base_va, sboClassName)) != NULL)
    {
    var_array   [num_var] = var_new;
    scope_array [num_var] = ctl_var_scope;	/* tmp var scope always same*/
    num_var++;
    }
  if (mvl61850_mkname_sbotimeout (ctl_var->name, sboTimeoutName, MAX_IDENT_LEN) == SD_SUCCESS
      && (var_new = mvlu_flat_var_create (ctl_var->base_va, sboTimeoutName)) != NULL)
    {
    var_array   [num_var] = var_new;
    scope_array [num_var] = ctl_var_scope;	/* tmp var scope always same*/
    num_var++;
    }
  if ((var_new = mvlu_flat_var_create (ctl_var->base_va, LocName)) != NULL)
    {
    var_array   [num_var] = var_new;
    scope_array [num_var] = ctl_var_scope;	/* tmp var scope always same*/
    num_var++;
    }
  
  if (num_var > 0)
    {
    /* Start scan, "scan_done_fun" called when scan is done.*/
    mvlu_scan_read_lov_start (num_var,
	var_array,		/* array of variables	*/
	scope_array,		/* array of scopes	*/
	mvluRdVaCtrl,		/* passed to scan_done_fun in indCtrl->usr*/
	scan_done_fun);		/* callback when scan completes*/
    retcode = SD_SUCCESS;	/* scan started	*/
    }
  else
    retcode = SD_FAILURE;	/* scan NOT started	*/
  return (retcode);
  }
#endif	/* defined(USE_RECURSIVE_READ)	*/

/************************************************************************/
/*			u_ctl_sbo_rd_ind				*/
/* If "Select" allowed, send ObjectReference of control in response.	*/
/* If "Select" NOT allowed, send "empty string" in response.		*/
/* NOTE: If USE_RECURSIVE_READ defined, a recursive scan is started.	*/
/*       When the scan completes, _ctl_sbo_rd_continue is called.	*/
/************************************************************************/
ST_VOID u_ctl_sbo_rd_ind (MVLU_RD_VA_CTRL *mvluRdVaCtrl)
  {
ST_RET retcode;
#if !defined(USE_RECURSIVE_READ)
MVL_SBO_CTRL *sboCtrl;
#endif

  /* Only perform "Select" if SBO is ONLY attribute being read.	*/
  if (mvluRdVaCtrl->rdVaCtrl->va->type_ctrl->num_rt != 1)
    {
    /* Reading structure containing SBO. Don't select, but return SUCCESS.*/
    /* CRITICAL: must do this, or structure read will fail.		*/
    /* Data is 'empty string' indicating select is not allowed.		*/
    mvlu_rd_prim_done (mvluRdVaCtrl, SD_SUCCESS);
    return;	/* all done	*/
    }

#if defined(USE_RECURSIVE_READ)
  /* Start a scan to read the necessary "CF" attributes for this "Control".*/
  /* When scan completes, "_ctl_sbo_rd_continue" is called to continue.*/
  retcode = _ctl_sbo_rd_scan_start (
	mvluRdVaCtrl,		/* ptr to original request struct	*/
        _ctl_sbo_rd_continue);	/* call this when scan completes	*/
  if (retcode)
    {
    /* Can't get Control configuration, so can't process this request.	*/
    /* This will always fail so set error = ARE_OBJ_INVALIDATED.	*/
    mvluRdVaCtrl->rdVaCtrl->failure = ARE_OBJ_INVALIDATED;	/* send in rsp*/
    mvlu_rd_prim_done (mvluRdVaCtrl, retcode);
    }
#else	/* !defined(USE_RECURSIVE_READ)	*/
  /* This library function does all the "required" SBO checks.	*/
  sboCtrl = mvl61850_ctl_chk_sbo (mvluRdVaCtrl);
  if (sboCtrl)
    strcpy (mvluRdVaCtrl->primData, sboCtrl->sbo_var);	/* 'positive' resp*/
  else
    mvluRdVaCtrl->primData [0] = '\0';	/* 'negative' resp (empty string)*/

#if 0	/* enable this code & add additional checks if necessary	*/
  if (sboCtrl)
    {	/* SISCO checks OK.	*/
    ST_BOOLEAN user_detected_error = SD_FALSE;

    /* Add code here to perform additional checks. Set "user_detected_error = SD_TRUE" if problem found.*/

    if (user_detected_error)
      {		/* user found problem, so cleanup.	*/
      mvluRdVaCtrl->primData [0] = '\0';	/* 'negative' resp (empty string)*/
      mvlu_sbo_ctrl_free (sboCtrl);
      }
    }
#endif

  /* Always return SUCCESS. Data is 'empty string' if select not allowed.*/
  mvlu_rd_prim_done (mvluRdVaCtrl, SD_SUCCESS);
#endif	/* !defined(USE_RECURSIVE_READ)	*/
  }

/************************************************************************/
/*			u_ctl_sbow_comp_wr_ind				*/
/* Write a component of the "SBOw" structure.				*/
/* Writing the complete "SBOw" structure performs the "Select" service	*/
/* if the control model is "sbo-with-enhanced-security".		*/
/************************************************************************/
ST_VOID u_ctl_sbow_comp_wr_ind (MVLU_WR_VA_CTRL *mvluWrVaCtrl)
  {
ST_RET retcode;	/* SD_SUCCESS or SD_FAILURE	*/
ST_CHAR *comp_name;	/* name of this component	*/
MVL_SBO_CTRL *sboCtrl;

  if (!mvluWrVaCtrl->wrVaCtrl->is_61850_control)
    {
    /* ERROR: Not writing whole "SBOw" structure.	*/
    mvluWrVaCtrl->wrVaCtrl->failure = ARE_OBJ_ACCESS_DENIED;	/* send in resp*/
    mvlu_wr_prim_done (mvluWrVaCtrl, SD_FAILURE);
    return;
    }

  /* Fill in LastApplError members just in case write fails.	*/
  fillLastApplError (mvluWrVaCtrl);

  /* This function is called for each leaf of "SBOw". To avoid repeating*/
  /* SBO checks, only perform SBO checks for one "mandatory" leaf	*/
  /* within "SBOw" struct (here we look for "Check" attribute).		*/
  comp_name = ms_comp_name_find (mvluWrVaCtrl->rt);
  if (strcmp (comp_name, "Check") == 0)
    {	/* This is "Check" attribute, so do SBO checks.	*/
    /* This library function does all the "required" SBO checks.	*/
    sboCtrl = mvl61850_ctl_chk_sbow (mvluWrVaCtrl);
    if (sboCtrl)
      retcode = SD_SUCCESS;	/* 'positive' resp*/
    else
      retcode = SD_FAILURE;	/* 'negative' resp*/

#if 0	/* enable this code & add additional checks if necessary	*/
    if (sboCtrl)
      {	/* SISCO checks OK.	*/
      ST_BOOLEAN user_detected_error = SD_FALSE;

      /* Add code here to perform additional checks. Set "user_detected_error = SD_TRUE" if problem found.*/

      if (user_detected_error)
        {		/* user found problem, so cleanup.	*/
        retcode = SD_FAILURE;	/* 'negative' resp*/
        mvlu_sbo_ctrl_free (sboCtrl);
        }
      }
#endif
    }
  else if (strcmp (comp_name, "orCat") == 0)
    {	/* for "orCat" make sure value is in range	*/
    if (orCatCheckRange (*(ST_INT8 *)mvluWrVaCtrl->primData))
      {
      mvluWrVaCtrl->wrVaCtrl->failure = ARE_OBJ_VALUE_INVALID;
      retcode = SD_FAILURE;	/* 'negative' resp*/
      }
    else
      retcode = SD_SUCCESS;	/* 'positive' resp*/
    }
  else
    {	/* this is NOT "Check" attribute, so skip SBO checks.	*/
    retcode = SD_SUCCESS;	/* allow the write	*/
    }

  mvlu_wr_prim_done (mvluWrVaCtrl, retcode);

  return;      
  }

/************************************************************************/
/*			prim_data_compare				*/
/* Compare old data to new data based on the primitive type.		*/
/* This function must avoid detecting changes in padding bits or bytes.	*/
/* NOTE: This function ignores changes for certain primitive types	*/
/*       because in IEC 61850, the trigger options 'dchg' or 'qchg' are	*/
/*       never set for attributes of these types.			*/
/* RETURNS: 0 if data identical (or don't care), 1 if different.	*/
/************************************************************************/
static ST_INT prim_data_compare (ST_VOID *new_data, ST_VOID *old_data, RUNTIME_TYPE *rt_type)
  {
ST_INT status = 0;	/* assume data identical	*/
ST_INT16 numbits, oldnumbits;

  switch (rt_type->el_tag)
    {
    case RT_BOOL	  :	  
      if (*(ST_BOOLEAN *)new_data != *(ST_BOOLEAN *)old_data)
        status = 1;	/* different	*/
      break;

    case RT_BIT_STRING  :	  
      if (rt_type->u.p.el_len < 0)
        {	/* variable length	*/
        numbits    = *(ST_INT16 *)new_data;
        oldnumbits = *(ST_INT16 *)old_data;
        if (numbits != oldnumbits ||
            numbits < 0 ||	/* should never happen, but let's be safe*/
            bstrcmp ((ST_UINT8 *)new_data+2, (ST_UINT8 *)old_data+2, numbits))
          status = 1;	/* different	*/
        }
      else
        {	/* fixed length		*/
        if (bstrcmp ((ST_UINT8 *)new_data, (ST_UINT8 *)old_data, rt_type->u.p.el_len))
          status = 1;	/* different	*/
        }
      break;

    case RT_INTEGER	  :
    case RT_UNSIGNED	  :
    case RT_FLOATING_POINT :
      if (memcmp (new_data, old_data, rt_type->u.p.el_len))
        status = 1;	/* different	*/
      break;

    /* NOTE: ignore changes for these primitive types.			*/
    /*       In IEC 61850, the trigger options 'dchg' or 'qchg' are	*/
    /*       never set for attributes of these types.			*/
    case RT_OCTET_STRING   :	  
    case RT_VISIBLE_STRING :
    case RT_GENERAL_TIME   :	  
    case RT_BINARY_TIME    :	  
    case RT_BCD	     :	  
    case RT_BOOLEANARRAY   :	  
    case RT_UTC_TIME:	  
    case RT_UTF8_STRING:	  
    default:
      break;
    }
  return (status);
  }

/************************************************************************/
/*			hvRefCheckRange					*/
/************************************************************************/
ST_RET hvRefCheckRange (ST_INT8 value)
  {
ST_RET retcode;
  if (value < 0 || value > 2)
    retcode = SD_FAILURE;
  else
    retcode = SD_SUCCESS;
  return (retcode);
  }
/************************************************************************/
/*			cmdQualCheckRange				*/
/************************************************************************/
ST_RET cmdQualCheckRange (ST_INT8 value)
  {
ST_RET retcode;
  if (value < 0 || value > 1)
    retcode = SD_FAILURE;
  else
    retcode = SD_SUCCESS;
  return (retcode);
  }
/************************************************************************/
/*			orCatCheckRange					*/
/************************************************************************/
ST_RET orCatCheckRange (ST_INT8 value)
  {
ST_RET retcode;
  if (value < 0 || value > 8)
    retcode = SD_FAILURE;
  else
    retcode = SD_SUCCESS;
  return (retcode);
  }
/************************************************************************/
/*			SIUnitCheckRange				*/
/************************************************************************/
ST_RET SIUnitCheckRange (ST_INT8 value)
  {
ST_RET retcode;
  if (value < 1 || 
      (value > 11 && value < 21) ||
      value == 40 ||
      (value > 54 && value < 61) ||
      value > 74)
    retcode = SD_FAILURE;
  else
    retcode = SD_SUCCESS;
  return (retcode);
  }
/************************************************************************/
/*			multiplierCheckRange				*/
/************************************************************************/
ST_RET multiplierCheckRange (ST_INT8 value)
  {
ST_RET retcode;
  /* Let compiler figure out most efficient way to check.	*/
  switch (value)
    {
    case (-24):
    case (-21):
    case (-18):
    case (-15):
    case (-12):
    case (-9):
    case (-6):
    case (-3):
    case (-2):
    case (-1):
    case 0:
    case 1:
    case 2:
    case 3:
    case 6:
    case 9:
    case 12:
    case 15:
    case 18:
    case 21:
    case 24:
      retcode = SD_SUCCESS;
      break;
    default:
      retcode = SD_FAILURE;
      break;
    }
  return (retcode);
  }
/************************************************************************/
/*			angRefCheckRange				*/
/* Use for angRef in CMV.						*/
/************************************************************************/
ST_RET angRefCheckRange (ST_INT8 value)
  {
ST_RET retcode;
  if (value < 0 || value > 2)
    retcode = SD_FAILURE;
  else
    retcode = SD_SUCCESS;
  return (retcode);
  }
/************************************************************************/
/*			angRefWYECheckRange				*/
/* Use for angRef in WYE, Delta, HWYE, HDEL.				*/
/************************************************************************/
ST_RET angRefWYECheckRange (ST_INT8 value)
  {
ST_RET retcode;
  if (value < 0 || value > 10)
    retcode = SD_FAILURE;
  else
    retcode = SD_SUCCESS;
  return (retcode);
  }
/************************************************************************/
/*			phsRefCheckRange				*/
/************************************************************************/
ST_RET phsRefCheckRange (ST_INT8 value)
  {
ST_RET retcode;
  if (value < 0 || value > 2)
    retcode = SD_FAILURE;
  else
    retcode = SD_SUCCESS;
  return (retcode);
  }
/************************************************************************/
/*			setCharactCheckRange				*/
/************************************************************************/
ST_RET setCharactCheckRange (ST_INT8 value)
  {
ST_RET retcode;
  if (value < 1 || value > 48)
    retcode = SD_FAILURE;
  else
    retcode = SD_SUCCESS;
  return (retcode);
  }

#if defined (GOOSE_RX_SUPP)
/************************************************************************/
/*			LGOSToSubscriber				*/
/* Use "setSrcCB" attribute from LGOS to find the GOOSE Subscriber.	*/
/* "base_va" arg should point to LGOS variable.				*/
/************************************************************************/

IEC_GOOSE_RX_USER_INFO *LGOSToSubscriber (MVL_VAR_ASSOC *base_va)
  {
ST_CHAR *flatname = "SP$GoCBRef$setSrcCB";
ST_CHAR *setSrcCB;
RUNTIME_TYPE *rt_type;
IEC_GOOSE_RX_USER_INFO *user_info;

  /* Find the pointer to the "setSrcCB" data.	*/
  setSrcCB = (ST_CHAR *) mvlu_get_leaf_data_ptr (base_va, flatname, &rt_type);

  /* if string is not empty, see if it is valid.	*/
  if (setSrcCB == NULL ||		/* Could not find setSrcCB	*/
      rt_type->el_tag != RT_VISIBLE_STRING)	/* setSrcCB is wrong type*/
    {
    user_info = NULL;
    }
  else
    {
    user_info = iecGooseSubscriberFind (setSrcCB);
    }
  return (user_info);
  }
#endif	/* defined (GOOSE_RX_SUPP)	*/


/************************************************************************/
/*			u_LGOS_NdsCom_rd_ind				*/
/* This boolean indicates if the GOOSE subscription needs commissioning.*/
/************************************************************************/
ST_VOID u_LGOS_NdsCom_rd_ind (MVLU_RD_VA_CTRL *mvluRdVaCtrl)
  {
#if defined (GOOSE_RX_SUPP)
ST_BOOLEAN NdsCom;	/* value to return	*/
IEC_GOOSE_RX_USER_INFO *user_info;

  /* base_va should be LGOS	*/
  user_info = LGOSToSubscriber (mvluRdVaCtrl->rdVaCtrl->va->base_va);
  if (user_info == NULL)
    NdsCom = SD_TRUE;	/* Could not find GCB (subscription needs commissioning)*/
  else
    NdsCom = SD_FALSE;
  *(ST_BOOLEAN *)mvluRdVaCtrl->primData = NdsCom;
#endif	/* defined (GOOSE_RX_SUPP)	*/

  /* Always return SUCCESS.	*/
  mvlu_rd_prim_done (mvluRdVaCtrl, SD_SUCCESS);
  }

/************************************************************************/
/* Last "StNum" value received in a GOOSE message.			*/
/* NOTE: INT32U is received in GOOSE, but this is INT32S (must cast).	*/
/************************************************************************/
ST_VOID u_LGOS_LastStNum_rd_ind (MVLU_RD_VA_CTRL *mvluRdVaCtrl)
  {
#if defined (GOOSE_RX_SUPP)
ST_INT32 LastStNum;	/* value to return	*/
IEC_GOOSE_RX_USER_INFO *user_info;

  user_info = LGOSToSubscriber (mvluRdVaCtrl->rdVaCtrl->va->base_va);
  if (user_info == NULL)
    LastStNum = 0;	/* Could not find GCB	*/
  else
    LastStNum = (ST_INT32) user_info->gptr->stNum;
  *(ST_INT32 *)mvluRdVaCtrl->primData = LastStNum;
#endif	/* defined (GOOSE_RX_SUPP)	*/

  /* Always return SUCCESS.	*/
  mvlu_rd_prim_done (mvluRdVaCtrl, SD_SUCCESS);
  }

/************************************************************************/
/*			u_LGOS_St_rd_ind				*/
/************************************************************************/
ST_VOID u_LGOS_St_rd_ind (MVLU_RD_VA_CTRL *mvluRdVaCtrl)
  {
#if defined (GOOSE_RX_SUPP)
ST_BOOLEAN St;	/* value to return	*/
IEC_GOOSE_RX_USER_INFO *user_info;
ST_DOUBLE cur_time;	/* current time (milliseconds since startup)  */

  user_info = LGOSToSubscriber (mvluRdVaCtrl->rdVaCtrl->va->base_va);
  if (user_info == NULL)
    St = SD_FALSE;	/* Could not find GCB	*/
  else
    {
    /* Should receive a GOOSE at least every 60 sec (allow 65 sec to be generous)*/
    cur_time = sGetMsTime();
    if (user_info->time_received > 0 && (cur_time < user_info->time_received + 65000))
      St = SD_TRUE;	/* receiving GOOSE	*/
    else
      St = SD_FALSE;	/* not receiving GOOSE	*/
    }
  *(ST_BOOLEAN *)mvluRdVaCtrl->primData = St;
#endif	/* defined (GOOSE_RX_SUPP)	*/

  /* Always return SUCCESS.	*/
  mvlu_rd_prim_done (mvluRdVaCtrl, SD_SUCCESS);
  }

/************************************************************************/
/*			u_LGOS_SimSt_rd_ind				*/
/* NOTE: GOOSE message attribute "Test" in IEC 61850 Edition 1 was	*/
/*       renamed as "Simulation" in Editon 2. "SimSt" is the last	*/
/*       received value of "Simulation".				*/
/************************************************************************/
ST_VOID u_LGOS_SimSt_rd_ind (MVLU_RD_VA_CTRL *mvluRdVaCtrl)
  {
#if defined (GOOSE_RX_SUPP)
ST_BOOLEAN SimSt;	/* value to return	*/
IEC_GOOSE_RX_USER_INFO *user_info;

  user_info = LGOSToSubscriber (mvluRdVaCtrl->rdVaCtrl->va->base_va);
  if (user_info == NULL)
    SimSt = 0;	/* Could not find GCB	*/
  else
    SimSt = user_info->gptr->test;	/* Test = Simulation = SimSt	*/
  *(ST_BOOLEAN *)mvluRdVaCtrl->primData = SimSt;
#endif	/* defined (GOOSE_RX_SUPP)	*/

  /* Always return SUCCESS.	*/
  mvlu_rd_prim_done (mvluRdVaCtrl, SD_SUCCESS);
  }

/************************************************************************/
/*			u_LGOS_ConfRevNum_rd_ind			*/
/* "ConfRevNum" is the value of "ConfRev" expected in received GOOSE	*/
/* messages.								*/
/* NOTE: ConfRev is INT32U, but ConfRevNum is INT32S (must cast).	*/
/************************************************************************/
ST_VOID u_LGOS_ConfRevNum_rd_ind (MVLU_RD_VA_CTRL *mvluRdVaCtrl)
  {
#if defined (GOOSE_RX_SUPP)
ST_INT32 ConfRevNum;	/* value to return	*/
IEC_GOOSE_RX_USER_INFO *user_info;

  user_info = LGOSToSubscriber (mvluRdVaCtrl->rdVaCtrl->va->base_va);
  if (user_info == NULL)
    ConfRevNum = 0;	/* Could not find GCB	*/
  else
    /* ConfRev was saved in GCB at startup. Return that value.	*/
    ConfRevNum = (ST_INT32) user_info->gptr->confRev;
  *(ST_UINT32 *)mvluRdVaCtrl->primData = ConfRevNum;
#endif	/* defined (GOOSE_RX_SUPP)	*/

  /* Always return SUCCESS.	*/
  mvlu_rd_prim_done (mvluRdVaCtrl, SD_SUCCESS);
  }


