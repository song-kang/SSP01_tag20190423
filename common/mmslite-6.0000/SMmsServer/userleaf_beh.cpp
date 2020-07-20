/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	2010-2010 All Rights Reserved					*/
/*									*/
/* MODULE NAME : userleaf_beh.c						*/
/* PRODUCT(S)  : MMSEASE-LITE						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*	Code to process read indication for IEC 61850 "Beh.stVal".	*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*			mvl61850_beh_stval_rd_ind			*/
/*									*/
/* NOTE: Define USE_RECURSIVE_READ in makefile to enable recursive	*/
/*       leaf read functions.						*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who	   Comments					*/
/* --------  ---  ------   -------------------------------------------  */
/* 03/19/12  JRB	   Fix Klocwork warning: uninitialized local.	*/
/* 12/08/10  JRB	   Created. Moved code here from mvl61850.c	*/
/************************************************************************/
#include "glbtypes.h"
#include "sysincs.h"
#include "mvl_uca.h"
#include "mvl_log.h"
#include "mvl_acse.h"	/* need mvl_cfg_info	*/

#if !defined(MVL_UCA)
#error MVL_UCA must be defined when compiling this file
#endif

#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of	*/
/* __FILE__ strings.							*/
/************************************************************************/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/
/*			_beh_stval_rd_finish				*/
/* Function called to finish processing for "mvl61850_beh_stval_rd_ind".*/
/* Get necessary data from local memory, compute Beh$stVal, and	finish	*/
/* (i.e. call mvlu_rd_prim_done).					*/
/************************************************************************/
static ST_VOID _beh_stval_rd_finish (MVLU_RD_VA_CTRL *mvluRdVaCtrl)
  {
MVL_VAR_ASSOC *base_var;
MVL_VAR_ASSOC *lln0_var;

ST_INT32 mod_stval;	/* ST$Mod$stVal from this LN	*/
ST_INT32 lln0mod_stval;	/* ST$Mod$stVal from LLN0	*/
ST_INT32 beh_stval;	/* ST$Beh$stVal from this LN (computed)	*/
ST_RET retcode = SD_SUCCESS;

  base_var = mvluRdVaCtrl->rdVaCtrl->va->base_va;

  /* Use lln0_var saved in dom at startup (see scl2_ld_create_part1 in sclproc.c)*/
  lln0_var = mvluRdVaCtrl->rdVaCtrl->va_scope.dom->lln0_var;
  if (lln0_var == NULL)
    {
    MVL_LOG_ERR0 ("Cannot compute 'Beh$stVal' because LLN0 not found.");
    retcode = SD_FAILURE;	/* Can't find LLN0. No way to compute Beh.*/
    }
 
  if (retcode == SD_SUCCESS)
    {
    /* Get value of "ST$Mod$stVal" in this LN.	*/
    retcode = mvlu_get_leaf_val_int_any (base_var, "ST$Mod$stVal", &mod_stval);
    }

  if (retcode == SD_SUCCESS)
    {
    /* Get value of "ST$Mod$stVal" in LLN0 of the same domain.	*/
    retcode = mvlu_get_leaf_val_int_any (lln0_var, "ST$Mod$stVal", &lln0mod_stval);
    }

  if (retcode == SD_SUCCESS)
    {	/* both successful. combine results.	*/
    switch (lln0mod_stval)
      {
      case 1:
        if (mod_stval > 0 && mod_stval <= 5)
          beh_stval = mod_stval;
        else
          beh_stval = 0;	/* Mod is illegal, so set Beh to illegal value too*/
        break;

      case 2:
        switch (mod_stval)
          {
          case 1:
          case 2:
            beh_stval = 2; break;
          case 3:
          case 4:
            beh_stval = 4; break;
          case 5:
            beh_stval = 5; break;
          default:
            beh_stval = 0; break;	/* Mod is illegal, so set Beh to illegal value too*/
          }
        break;

      case 3:
        switch (mod_stval)
          {
          case 1:
            beh_stval = 3; break;
          case 2:
            beh_stval = 4; break;
          case 3:
            beh_stval = 3; break;
          case 4:
            beh_stval = 4; break;
          case 5:
            beh_stval = 5; break;
          default:
            beh_stval = 0; break;	/* Mod is illegal, so set Beh to illegal value too*/
          }
        break;

      case 4:
        switch (mod_stval)
          {
          case 1:
          case 2:
          case 3:
          case 4:
            beh_stval = 4; break;
          case 5:
            beh_stval = 5; break;
          default:
            beh_stval = 0; break;	/* Mod is illegal, so set Beh to illegal value too*/
          }
        break;

      case 5:
        if (mod_stval > 0 && mod_stval <= 5)
          beh_stval = 5;
        else
          beh_stval = 0;	/* Mod is illegal, so set Beh to illegal value too*/
        break;

      default:
        beh_stval = 0;	/* LLNO Mod is illegal, so set Beh to illegal value too*/
        break;
      }	/* end outer switch	*/

    /* Convert data to configured type (INT8, INT16, or INT32).		*/
    if (mvluRdVaCtrl->rt->u.p.el_len == 1)
      *(ST_INT8 *) mvluRdVaCtrl->primData = (ST_INT8) beh_stval;	/* copy data	*/
    else if (mvluRdVaCtrl->rt->u.p.el_len == 2)
      *(ST_INT16 *) mvluRdVaCtrl->primData = (ST_INT16) beh_stval;	/* copy data	*/
    else if (mvluRdVaCtrl->rt->u.p.el_len == 4)
      *(ST_INT32 *) mvluRdVaCtrl->primData = beh_stval;	/* copy data	*/
    else
      retcode = SD_FAILURE;	/* unsupported len. Should never happen.*/
    }
    
  mvlu_rd_prim_done (mvluRdVaCtrl, retcode);
  }
/************************************************************************/
/*			_beh_stval_rd_scan_done				*/
/* Function called when scan started by "_beh_stval_rd_scan_start"	*/
/* completes.								*/
/* RETURNS:	SD_SUCCESS (caller ignores return anyway)		*/
/************************************************************************/
static ST_RET _beh_stval_rd_scan_done (MVL_IND_PEND *indCtrl)
  {
MVLU_RD_VA_CTRL *mvluRdVaCtrl;	/* pointer to original Read request	*/
ST_RET retcode;

  /* CRITICAL: Get pointer to original request BEFORE freeing indCtrl	*/
  /*           (saved in "indCtrl->usr" when scan started).		*/
  mvluRdVaCtrl = (MVLU_RD_VA_CTRL *) indCtrl->usr;

  /* Get result of scan and free "indCtrl".	*/
  retcode = mvlu_scan_read_lov_cleanup (indCtrl);
 
  /* Scan is done and cleaned up. Finish processing.	*/
  /* If scan failed, stop now. Else, call finish function.	*/
  if (retcode)
    mvlu_rd_prim_done (mvluRdVaCtrl, retcode);
  else
    _beh_stval_rd_finish (mvluRdVaCtrl);

  return (SD_SUCCESS);	/* NOTE: ignored by caller	*/
  }
/************************************************************************/
/*			_beh_stval_rd_scan_start			*/
/* Function called to start scan.					*/
/************************************************************************/
static ST_VOID _beh_stval_rd_scan_start (MVLU_RD_VA_CTRL *mvluRdVaCtrl)
  {
MVL_DOM_CTRL *dom;		/* domain containing this variable	*/
MVL_VAR_ASSOC *base_var;

ST_RET retcode = SD_SUCCESS;
MVL_VAR_ASSOC *var_array [2];
MVL_SCOPE *scope_array [2];
ST_INT j;	/* index to arrays	*/
ST_INT num_var = sizeof(var_array)/sizeof(MVL_VAR_ASSOC *);

  memset (var_array, 0, sizeof(var_array)); /* init var_array */

  /* Assume scope is DOM_SPEC	*/

  base_var = mvluRdVaCtrl->rdVaCtrl->va->base_va;

  dom = mvluRdVaCtrl->rdVaCtrl->va_scope.dom;

  /* Use lln0_var saved in dom at startup (see scl2_ld_create_part1 in sclproc.c)*/
  if (dom->lln0_var == NULL)
    {
    MVL_LOG_ERR0 ("Cannot compute 'Beh$stVal' because LLN0 not found.");
    retcode = SD_FAILURE;	/* Can't find LLN0. No way to compute Beh.*/
    }

  /* Make temporary vars.	*/
  if (retcode == SD_SUCCESS)
    {
    /* Create temp vars. If one fails, don't create next.	*/
    /* Set scope and var pointer for each.			*/
    /* Set all tmp var scopes same as this var.			*/
    j = 0;
    scope_array [j] = &mvluRdVaCtrl->rdVaCtrl->va_scope;
    var_array [j] = mvlu_flat_var_create (dom->lln0_var, "ST$Mod$stVal");
    j++;
    scope_array [j] = &mvluRdVaCtrl->rdVaCtrl->va_scope;
    var_array [j] = mvlu_flat_var_create (base_var, "ST$Mod$stVal");
    j++;
    assert (j == num_var);	/* make sure we counted correctly*/

    /* If any tmp var fails, can't continue. Clean up at end.	*/
    for (j = 0; j < num_var; j++)
      {
      if (var_array [j] == NULL)
        {
        retcode = SD_FAILURE;
        break;
        }
      }
    }
 
  if (retcode == SD_SUCCESS)
    {
    /* Start scan, "_beh_stval_rd_scan_done" called when scan is done.*/
    mvlu_scan_read_lov_start (num_var,
        var_array,		/* array of variables	*/
        scope_array,		/* array of scopes	*/
        mvluRdVaCtrl,		/* ptr saved in indCtrl->usr	*/
        _beh_stval_rd_scan_done);/* scan_done_fun	*/
    }

  /* If anything failed, scan was not started, so must cleanup & finish here.*/
  if (retcode)
    {
    /* Destroy variables if they were created.	*/
    for (j = 0; j < num_var; j++)
      {
      if (var_array [j] != NULL)
        {
        /* NOTE: created by mvlu_flat_var_create. Use companion function to destroy.*/
        mvlu_flat_var_destroy (var_array[j]);
        }
      }
    mvlu_rd_prim_done (mvluRdVaCtrl, retcode);	/* immediate error	*/
    }
  }
/************************************************************************/
/*			mvl61850_beh_stval_rd_ind			*/
/************************************************************************/
ST_VOID mvl61850_beh_stval_rd_ind (MVLU_RD_VA_CTRL *mvluRdVaCtrl)
  {
#ifdef USE_RECURSIVE_READ
  /* This starts a scan to get the necessary data.			*/
  /* When scan completes, _beh_stval_rd_finish is called to build response.*/
  _beh_stval_rd_scan_start (mvluRdVaCtrl);
#else
  /* Assume data in local memory is up to date and use it to build response.*/ 
  _beh_stval_rd_finish (mvluRdVaCtrl);
#endif
  }


#ifdef __cplusplus
};
#endif
