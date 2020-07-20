/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	2011-2011 All Rights Reserved					*/
/*									*/
/* MODULE NAME : userwrite.c						*/
/* PRODUCT(S)  : MMS-EASE-LITE						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*	User code for processing MMS Write Indications for IEC 61850	*/
/*	objects.							*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*			u_mvl_write_ind_61850				*/
/*									*/
/* NOTE: This code only executes if the user sets the global function	*/
/*       pointer like this:						*/
/*		u_mvl_write_ind_custom = u_mvl_write_ind_61850;		*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev     Comments					*/
/* --------  ---  ------   -------------------------------------------  */
/* 02/24/11  JRB	   Created.					*/
/************************************************************************/
#include "glbtypes.h"
#include "sysincs.h"
#include "mvl_uca.h"

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

#if defined(USE_RECURSIVE_READ)
/************************************************************************/
/*			_write_prescan_start				*/
/* Starts a scan to read data needed to process a Write indication.	*/
/* For IEC 61850 Control objects, it reads a few "CF" attributes.	*/
/* NOTE: You may need to add code to handle other special cases.	*/
/* RETURNS:	SD_SUCCESS if prescan started				*/
/*		SD_FAILURE otherwise					*/
/************************************************************************/
ST_RET _write_prescan_start (
	MVL_IND_PEND *indCtrl,	/* Write indication requiring this scan	*/
				/* Passed to (*scan_done_fun) as "usr" ptr*/
	ST_RET (*scan_done_fun)(MVL_IND_PEND *indCtrl))
				/* call this when scan completes	*/
  {
ST_RET retcode;
MVLAS_WRITE_CTRL *wrCtrl;	/* Write Indication info		*/
MVLAS_WR_VA_CTRL *wrVaCtrl;	/* Write Indication info for one variable*/
ST_INT num_ctl_var;		/* number of control variables found	*/
ST_INT num_var_alloc;		/* number of variables allocated	*/
ST_INT num_var;			/* number of variables to read		*/
MVL_VAR_ASSOC *ctl_var;		/* "control" var being written (if found)*/
MVL_SCOPE *ctl_var_scope;	/* "control" var scope			*/

MVL_VAR_ASSOC *var_new;		/* new var created here. Saved in var_array.*/
MVL_VAR_ASSOC **var_array;	/* array of pointers (allocated)	*/
MVL_SCOPE **scope_array;	/* array of pointers (allocated)	*/
ST_INT j;			/* loop index	*/
ST_CHAR sboClassName   [MAX_IDENT_LEN+1];	/* "CF$...$sboClass"	*/
ST_CHAR sboTimeoutName [MAX_IDENT_LEN+1];	/* "CF$...$sboTimeout"	*/
ST_CHAR ctlModelName   [MAX_IDENT_LEN+1];	/* "CF$...$ctlModel"	*/
ST_CHAR *LocName = "ST$Loc$stVal";		/* always same		*/

  wrCtrl = &indCtrl->u.wr;

  /* Loop through variables & count number of IEC 61850 Control vars.	*/
  /* NOTE: is_61850_control flag was set by _mvl_process_write_ind.	*/
  num_ctl_var = 0;
  for (j = 0, wrVaCtrl = wrCtrl->vaCtrlTbl;
       j < wrCtrl->numVar;
       ++j, ++wrVaCtrl)
    {
    if (wrVaCtrl->is_61850_control)
      num_ctl_var++;
    }

  if (num_ctl_var == 0)
    return (SD_FAILURE);	/* nothing to scan. prescan NOT started	*/

  /* Need to read 4 variables for each control.				*/
  /* NOTE: other variables may be added to this prescan, but if so,	*/
  /*       num_var_alloc must be increased.				*/
  num_var_alloc = num_ctl_var * 4;
  num_var = 0;	/* init num var to read. Increment as vars created.	*/

  /* Allocate variable and scope arrays.	*/
  var_array   = chk_calloc (num_var_alloc, sizeof(MVL_VAR_ASSOC *));
  scope_array = chk_calloc (num_var_alloc, sizeof(MVL_SCOPE *));

  /* Loop through all vars and set up "prescan" when needed.		*/
  /* If we don't find an attribute, just ignore it (we should get the	*/
  /* same error later when we continue the write).			*/
  for (j = 0, wrVaCtrl = wrCtrl->vaCtrlTbl;
       j < wrCtrl->numVar;
       ++j, ++wrVaCtrl)
    {
    /* If this is Control var, we need to "prescan" some "CF" attributes.*/
    if (wrVaCtrl->is_61850_control)
      {
      ctl_var = wrVaCtrl->va;
      ctl_var_scope = &wrVaCtrl->va_scope;

      if (ctl_var_scope->scope != DOM_SPEC)	/* must be DOM_SPEC*/
        {
        continue;	/* skip to next variable. None of this will work.*/
        }
    
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
      }	/* end "if (wrVaCtrl->is_61850_control)"	*/
    }	/* end "for" loop	*/
 
  assert (num_var <= num_var_alloc);	/* <= number we allocated	*/
  if (num_var > 0)
    {
    /* Start scan, "scan_done_fun" called when scan is done.*/
    mvlu_scan_read_lov_start (num_var,
        var_array,		/* array of variables	*/
        scope_array,		/* array of scopes	*/
        indCtrl,		/* usr ptr passed to scan_done_fun	*/
        scan_done_fun);		/* callback when scan completes		*/
    retcode = SD_SUCCESS;	/* scan started	*/
    }
  else
    retcode = SD_FAILURE;	/* scan NOT started	*/

  /* Always free these temporary arrays.	*/
  chk_free (var_array);
  chk_free (scope_array);
  return (retcode);
  }

/************************************************************************/
/*			_write_continue					*/
/* Callback function called when scan started by u_mvl_write_ind_61850	*/
/* completes. Continue processing of Write indication.			*/
/* RETURNS:	SD_SUCCESS (caller ignores return anyway)		*/
/************************************************************************/
static ST_RET _write_continue (MVL_IND_PEND *indCtrl)
  {
ST_RET retcode;
MVL_IND_PEND *indCtrlOrig;

  /* CRITICAL: Get pointer to original request BEFORE freeing indCtrl	*/
  /*           (saved in "indCtrl->usr" when scan started).		*/
  indCtrlOrig = (MVL_IND_PEND *) indCtrl->usr;  

  /* Get result of scan and free "indCtrl".	*/
  retcode = mvlu_scan_read_lov_cleanup (indCtrl);
 
  /* Continue with normal Write indication processing.	*/
  u_mvl_write_ind (indCtrlOrig);

  return (SD_SUCCESS);	/* NOTE: ignored by caller	*/
  }
#endif	/* defined(USE_RECURSIVE_READ)	*/

/************************************************************************/
/*			u_mvl_write_ind_61850				*/
/* User function to process a Write Indication.				*/
/* Calls _write_prescan_start to start special processing (if needed).	*/
/* If prescan started successfully, "u_mvl_write_ind" will be called	*/
/* when the prescan completes.						*/
/* NOTE: User may add special processing for other variables.		*/
/************************************************************************/
ST_VOID u_mvl_write_ind_61850 (MVL_IND_PEND *indCtrl)
  {
#if defined(USE_RECURSIVE_READ)
ST_RET retcode;

  /* Attempt to start prescan (returns SD_FAILURE if nothing to scan)	*/
  retcode = _write_prescan_start (indCtrl,
            _write_continue);	/* call this when scan completes	*/

  if (retcode)
    /* prescan failed OR not started. Must continue processing now.	*/
    u_mvl_write_ind (indCtrl);	/* do default processing now.	*/
#else	/* !USE_RECURSIVE_READ	*/
  u_mvl_write_ind (indCtrl);	/* do default processing now.	*/
#endif	/* !USE_RECURSIVE_READ	*/
  }

