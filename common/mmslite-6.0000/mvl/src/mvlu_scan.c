/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	2010-2010 All Rights Reserved					*/
/*									*/
/* MODULE NAME : mvlu_scan.c						*/
/* PRODUCT(S)  : MMSEASE-LITE						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*	Functions to start a scan to read any List of Variables	(LOV)	*/
/*	and call a user callback function when the scan complete.	*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*			mvlu_scan_read_lov_start			*/
/*			mvlu_scan_read_lov_cleanup			*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who	   Comments					*/
/* --------  ---  ------   -------------------------------------------  */
/* 12/08/10  JRB	   Created.					*/
/************************************************************************/
#include "glbtypes.h"
#include "sysincs.h"
#include "mvl_uca.h"
#include "mvl_log.h"

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/
/*			mvlu_scan_read_lov_start			*/
/* This function starts the scan of ALL data for one "List of		*/
/* Variables". It allocates & initializes a temporary MVL_IND_PEND	*/
/* structure for a simulated read indication, then starts the scan.	*/
/* RETURNS:	ST_VOID						 	*/
/*									*/
/* NOTE: similar to "mvlu_setup_scan_read" but for LOV and it also	*/
/*       starts the scan.						*/
/* CRITICAL: Must call mvlu_scan_read_lov_cleanup when scan completes.	*/
/************************************************************************/
ST_VOID mvlu_scan_read_lov_start (
	ST_INT num_of_entries,		/* num of vars in array		*/
	MVL_VAR_ASSOC **varlist,	/* array of variables		*/
	MVL_SCOPE **scopelist,		/* array of scopes		*/
	ST_VOID *usr_info,		/* ptr to any user info		*/
					/* saved in indCtrl->usr	*/
	ST_RET (*scan_done_fun)(MVL_IND_PEND *indCtrl)
	)
  {
MVLAS_RD_VA_CTRL *vaCtrl;
MVL_IND_PEND *indCtrl;
ST_INT j;

/* Allocate space for MVL_IND_PEND & array of MVLAS_RD_VA_CTRL.	*/
  indCtrl = M_CALLOC (MSMEM_GEN, 1,
              sizeof (MVL_IND_PEND) +
              num_of_entries * sizeof (MVLAS_RD_VA_CTRL));

  indCtrl->u.rd.numVar = num_of_entries;
  indCtrl->op = MMSOP_MVLU_RPT_VA;	/* indicates this is not normal read*/

  indCtrl->u.rd.vaCtrlTbl = (MVLAS_RD_VA_CTRL *)(indCtrl+1);/*point after indCtrl*/

  for (j = 0, vaCtrl = indCtrl->u.rd.vaCtrlTbl;
       j < num_of_entries;
       j++, vaCtrl++)
    {
    vaCtrl->va = varlist[j];
    memcpy (&vaCtrl->va_scope, scopelist[j], sizeof(MVL_SCOPE));
    }

  /* Save user pointer to use in callback function (scan_done_fun).	*/
  indCtrl->usr = usr_info;

  /* Save funct ptr. Called from "mvlu_rd_prim_done" when scan complete.*/
  indCtrl->usr_resp_fun = scan_done_fun;	/* CRITICAL:	*/

  u_mvl_read_ind (indCtrl);	/* start the scan */
  }

/************************************************************************/
/*			mvlu_scan_read_lov_cleanup			*/
/* User must call this when scan started by mvlu_scan_read_lov_start	*/
/* completes.								*/
/* RETURNS:	SD_SUCCESS	if all variables successful		*/
/*		SD_FAILURE	is ANY variable failed.			*/
/************************************************************************/
ST_RET mvlu_scan_read_lov_cleanup (MVL_IND_PEND *indCtrl)
  {
MVLAS_RD_VA_CTRL *vaCtrl;
ST_INT j;
ST_RET retcode;
ST_INT numVar;		/* number of variables read	*/

  numVar = indCtrl->u.rd.numVar;

  /* Check result of this recursive scan.				*/
  /* If any variable failed, it's an error.				*/
  retcode = SD_SUCCESS;	/* change this if any variable failed.	*/
  for (j = 0, vaCtrl = indCtrl->u.rd.vaCtrlTbl;
       j < numVar;
       j++, vaCtrl++)
    {
    if (vaCtrl->acc_rslt_tag == ACC_RSLT_FAILURE)
      {
      retcode = SD_FAILURE;
      break;
      }
    }

  /* Free temporary resources allocated for scan.	*/
  for (j = 0, vaCtrl = indCtrl->u.rd.vaCtrlTbl;
       j < numVar;
       j++, vaCtrl++)
    {
    /* NOTE: created by mvlu_flat_var_create. Use companion function to destroy.*/
    mvlu_flat_var_destroy (vaCtrl->va);
    }
  chk_free (indCtrl);
  return (retcode);
  }

