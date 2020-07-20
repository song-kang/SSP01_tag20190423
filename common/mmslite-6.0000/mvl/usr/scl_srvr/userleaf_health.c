/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	2011-2012 All Rights Reserved					*/
/*									*/
/* MODULE NAME : userleaf_health.c					*/
/* PRODUCT(S)  : MMSEASE-LITE						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*	Functions to process read indication for IEC 61850		*/
/*	"Health.stVal", "Health.q", and "Health.t" in LLN0.		*/
/* CRITICAL:								*/
/*	This code assumes the type of "Health.stVal" is INT8 (according	*/
/*	to IEC 61850 Edition 2) or INT32 (according to Edition 1).	*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*			lln0_health_stval_rd_ind			*/
/*			lln0_health_q_rd_ind				*/
/*			lln0_health_t_rd_ind				*/
/*									*/
/* NOTE: Define USE_RECURSIVE_READ in makefile to enable recursive	*/
/*       leaf read functions.						*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who	   Comments					*/
/* --------  ---  ------   -------------------------------------------  */
/* 03/02/12  JRB	   Return defaults if Health found ONLY in LLN0.*/
/* 04/04/11  JRB	   Allow also stVal type = INT32.		*/
/* 02/28/11  JRB	   Created.					*/
/************************************************************************/
#include "glbtypes.h"
#include "sysincs.h"
#include "mvl_uca.h"
#include "mvl_log.h"

#if !defined(MVL_UCA)
#error MVL_UCA must be defined when compiling this file
#endif

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of	*/
/* __FILE__ strings.							*/
/************************************************************************/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/*======================================================================*/
/* Function for "LLN0$ST$Health$stVal" Read Indication.			*/
/*======================================================================*/

/************************************************************************/
/*			lln0_health_stval_rd_finish			*/
/* Function called to finish processing for "lln0_health_stval_rd_ind".	*/
/* Get necessary data from local memory, compute Health$stVal,		*/
/* and finish (i.e. call mvlu_rd_prim_done).				*/
/* NOTE: this function only used for Health in LLN0.			*/
/************************************************************************/
static ST_VOID lln0_health_stval_rd_finish (MVLU_RD_VA_CTRL *mvluRdVaCtrl)
  {
MVL_VAR_ASSOC *base_var;
MVL_VAR_ASSOC *base_var_loop;	/* used in loop through domain vars	*/
ST_INT8 lln0_health_stval;	/* Value to return (LLN0$ST$Health$stVal)*/
ST_INT8 health_stval;		/* Value of Health$stVal in other LN.	*/
/* NOTE: Use these 2 vars instead if stVal is INT32	*/
ST_INT32 lln0_health_stval_i32;	/* Value to return (LLN0$ST$Health$stVal)*/
ST_INT32 health_stval_i32;	/* Value of Health$stVal in other LN.	*/
ST_RET retcode;
ST_INT j;
MVL_DOM_CTRL *dom;

  base_var = mvluRdVaCtrl->rdVaCtrl->va->base_va;

  /* Assume scope is DOM_SPEC	*/
  dom = mvluRdVaCtrl->rdVaCtrl->va_scope.dom;

  if (mvluRdVaCtrl->rt->el_tag != RT_INTEGER)
    {
    MVL_LOG_ERR0 ("LLN0$ST$Health$stVal is not INTEGER");
    retcode = SD_FAILURE;
    }
  else if (mvluRdVaCtrl->rt->u.p.el_len == 1)
    {
    /* Type is INT8. Assume all other Health$stVal are INT8.	*/
    lln0_health_stval = 1;	/* start with normal value	*/
    retcode = SD_SUCCESS;		/* change if error occurs.	*/
    for (j = 0; j < dom->num_var_assoc; j++)
      {
      base_var_loop = dom->var_assoc_tbl[j];

      /* Skip LLN0 because we're computing LLN0 Health from others.	*/
      /* Skip LPHD because it doesn't contain Health.			*/
      /* Use "strstr" because name may have prefix and suffix.		*/
      if (base_var_loop != base_var  &&			/* not LLN0	*/
          strstr (base_var_loop->name, "LPHD") == NULL)	/* not LPHD	*/
        {
        retcode = mvlu_get_leaf_val_int8 (base_var_loop, "ST$Health$stVal", &health_stval);
        if (retcode)
          {
          MVL_LOG_ERR1 ("Cannot find %s$ST$Health$stVal or type is not INT8", base_var_loop->name);
          retcode = SD_FAILURE;
          break;	/* stop on first error	*/
          }
        else
          {
          if (health_stval > lln0_health_stval)
            lln0_health_stval = health_stval;	/* val worse than current val, so update it*/
          }
        }
      }
    if (retcode == SD_SUCCESS)
      {
      *(ST_INT8 *) mvluRdVaCtrl->primData = lln0_health_stval;	/* copy data	*/
      }
    }
  else if (mvluRdVaCtrl->rt->u.p.el_len == 4)
    {
    /* Type is INT32. Assume all other Health$stVal are INT32.	*/
    lln0_health_stval_i32 = 1;	/* start with normal value	*/
    retcode = SD_SUCCESS;		/* change if error occurs.	*/
    for (j = 0; j < dom->num_var_assoc; j++)
      {
      base_var_loop = dom->var_assoc_tbl[j];

      /* Skip LLN0 because we're computing LLN0 Health from others.	*/
      /* Skip LPHD because it doesn't contain Health.			*/
      /* Use "strstr" because name may have prefix and suffix.		*/
      if (base_var_loop != base_var  &&			/* not LLN0	*/
          strstr (base_var_loop->name, "LPHD") == NULL)	/* not LPHD	*/
        {
        retcode = mvlu_get_leaf_val_int32 (base_var_loop, "ST$Health$stVal", &health_stval_i32);
        if (retcode)
          {
          MVL_LOG_ERR1 ("Cannot find %s$ST$Health$stVal or type is not INT32", base_var_loop->name);
          retcode = SD_FAILURE;
          break;	/* stop on first error	*/
          }
        else
          {
          if (health_stval_i32 > lln0_health_stval_i32)
            lln0_health_stval_i32 = health_stval_i32;	/* val worse than current val, so update it*/
          }
        }
      }
    if (retcode == SD_SUCCESS)
      {
      *(ST_INT32 *) mvluRdVaCtrl->primData = lln0_health_stval_i32;	/* copy data	*/
      }
    }
  else
    {
    MVL_LOG_ERR0 ("LLN0$ST$Health$stVal integer size is unsupported");
    retcode = SD_FAILURE;
    }

  mvlu_rd_prim_done (mvluRdVaCtrl, retcode);
  }
/************************************************************************/
/*			lln0_health_stval_rd_scan_done			*/
/* Function called when scan started by "lln0_health_stval_rd_scan_start"*/
/* completes.								*/
/* RETURNS:	SD_SUCCESS (caller ignores return anyway)		*/
/* NOTE: this function only used for Health in LLN0.			*/
/************************************************************************/
static ST_RET lln0_health_stval_rd_scan_done (MVL_IND_PEND *indCtrl)
  {
MVLU_RD_VA_CTRL *mvluRdVaCtrl;	/* pointer to original Read request	*/
ST_RET retcode;

  /* CRITICAL: Get pointer to original request BEFORE freeing indCtrl	*/
  /*           (saved in "indCtrl->usr" when scan started).		*/
  mvluRdVaCtrl = (MVLU_RD_VA_CTRL *) indCtrl->usr;

  /* Get result of scan and free "indCtrl".	*/
  retcode = mvlu_scan_read_lov_cleanup (indCtrl);

  /* Scan is done and cleaned up. Finish processing.		*/
  /* If scan failed, stop now. Else, call finish function.	*/
  if (retcode)
    mvlu_rd_prim_done (mvluRdVaCtrl, retcode);
  else
    lln0_health_stval_rd_finish (mvluRdVaCtrl);

  return (SD_SUCCESS);	/* NOTE: ignored by caller	*/
  }
/************************************************************************/
/*			lln0_health_stval_rd_scan_start			*/
/* Function called to start scan.					*/
/* NOTE: this function only used for Health in LLN0.			*/
/************************************************************************/
static ST_VOID lln0_health_stval_rd_scan_start (MVLU_RD_VA_CTRL *mvluRdVaCtrl)
  {
MVL_DOM_CTRL *dom;		/* domain containing this variable	*/
MVL_VAR_ASSOC *base_var;
MVL_VAR_ASSOC *base_var_loop;	/* used in loop through domain vars	*/

ST_RET retcode = SD_SUCCESS;
MVL_VAR_ASSOC **var_array;
MVL_SCOPE **scope_array;
ST_INT j;	/* index to arrays	*/
ST_INT num_var;
ST_INT array_size;
MVL_VAR_ASSOC *tmp_var;	/* temporary var	*/

  base_var = mvluRdVaCtrl->rdVaCtrl->va->base_va;

  /* Assume scope is DOM_SPEC	*/
  dom = mvluRdVaCtrl->rdVaCtrl->va_scope.dom;

  array_size = dom->num_var_assoc;
  var_array =   (MVL_VAR_ASSOC **) chk_calloc (array_size, sizeof (MVL_VAR_ASSOC *));
  scope_array = (MVL_SCOPE **)     chk_calloc (array_size, sizeof (MVL_SCOPE *));

  /* Make temporary vars.	*/
  num_var = 0;

  for (j = 0; j < dom->num_var_assoc; j++)
    {
    base_var_loop = dom->var_assoc_tbl[j];
    /* Skip LLN0 because we're computing LLN0 Health from others.	*/
    /* Skip LPHD because it doesn't contain Health.			*/
    /* Use "strstr" because name may have prefix and suffix.		*/
    /* Set same scope for all variables.				*/
    if (base_var_loop != base_var  &&			/* not LLN0	*/
        strstr (base_var_loop->name, "LPHD") == NULL)	/* not LPHD	*/
      {	/* this base var is "not" LLN0 or LPHD	*/
      /* If correct attribute found in this LN, add it to arrays.	*/
      tmp_var = mvlu_flat_var_create (base_var_loop, "ST$Health$stVal");
      if (tmp_var)
        {
        scope_array [num_var] = &mvluRdVaCtrl->rdVaCtrl->va_scope;
        var_array [num_var] = tmp_var;
        num_var++;
        }
      }
    }

  /* If temporary vars created, start a scan, else respond immediately.*/
  if (num_var)
    {
    /* Start scan.	*/
    mvlu_scan_read_lov_start (num_var,
        var_array,		/* array of variables	*/
        scope_array,		/* array of scopes	*/
        mvluRdVaCtrl,		/* ptr saved in indCtrl->usr	*/
        lln0_health_stval_rd_scan_done);/* this function is called when scan is done*/
    }
  else
    {
    /* Nothing to scan, so respond immediately.	*/
    if (mvluRdVaCtrl->rt->u.p.el_len == 1)
      *(ST_INT8 *) mvluRdVaCtrl->primData = 1;	/* OK	*/
    else if (mvluRdVaCtrl->rt->u.p.el_len == 4)
      *(ST_INT32 *) mvluRdVaCtrl->primData = 1;	/* OK	*/
    else
      {
      mvluRdVaCtrl->rdVaCtrl->failure = ARE_TYPE_INCONSISTENT;	/* send in rsp*/
      retcode = SD_FAILURE;
      }
    mvlu_rd_prim_done (mvluRdVaCtrl, retcode);
    }
  /* Free temporary arrays created at top of function.	*/
  chk_free (var_array);
  chk_free (scope_array);
  }
/************************************************************************/
/*			lln0_health_stval_rd_ind			*/
/* Leaf function called for "LLN0$ST$Health$stVal" attribute.		*/
/* It combines "Health$stVal" values from all other Logical Nodes.	*/
/************************************************************************/
ST_VOID lln0_health_stval_rd_ind (MVLU_RD_VA_CTRL *mvluRdVaCtrl)
  {
#ifdef USE_RECURSIVE_READ
  /* This starts a scan to get the necessary data.			*/
  /* When scan completes, lln0_health_stval_rd_finish is called to build response.*/
  lln0_health_stval_rd_scan_start (mvluRdVaCtrl);
#else
  /* Assume data in local memory is up to date and use it to build response.*/ 
  lln0_health_stval_rd_finish (mvluRdVaCtrl);
#endif
  }

/*======================================================================*/
/* Function for "LLN0$ST$Health$q" Read Indication.			*/
/*======================================================================*/

/************************************************************************/
/*			lln0_health_q_rd_finish				*/
/* Function called to finish processing for "lln0_health_q_rd_ind".	*/
/* Get necessary data from local memory, compute Health$q,		*/
/* and finish (i.e. call mvlu_rd_prim_done).				*/
/* NOTE: this function only used for Health in LLN0.			*/
/************************************************************************/
/* Variable length bitstring for Quality.	*/
typedef struct
  {
  ST_INT16 len;
  ST_UCHAR data[2];	/* allows for 13 bits of data	*/
  } MMS_BVSTRING_Q;

static ST_VOID lln0_health_q_rd_finish (MVLU_RD_VA_CTRL *mvluRdVaCtrl)
  {
MVL_VAR_ASSOC *base_var;
MVL_VAR_ASSOC *base_var_loop;	/* used in loop through domain vars	*/
MMS_BVSTRING_Q lln0_health_q;	/* Value to return (LLN0$ST$Health$q)*/
MMS_BVSTRING_Q health_q;	/* Value of Health$q in other LN.	*/
ST_RET retcode;
ST_INT j;
MVL_DOM_CTRL *dom;

  base_var = mvluRdVaCtrl->rdVaCtrl->va->base_va;

  /* Assume scope is DOM_SPEC	*/
  dom = mvluRdVaCtrl->rdVaCtrl->va_scope.dom;

  lln0_health_q.len = 13;	/* start with normal value	*/
  lln0_health_q.data[0] = 0;
  lln0_health_q.data[1] = 0;
  retcode = SD_SUCCESS;		/* change if error occurs.	*/
  for (j = 0; j < dom->num_var_assoc; j++)
    {
    base_var_loop = dom->var_assoc_tbl[j];

    /* Skip LLN0 because we're computing LLN0 Health from others.	*/
    /* Skip LPHD because it doesn't contain Health.			*/
    /* Use "strstr" because name may have prefix and suffix.		*/
    if (base_var_loop != base_var  &&			/* not LLN0	*/
        strstr (base_var_loop->name, "LPHD") == NULL)	/* not LPHD	*/
      {
      retcode = mvlu_get_leaf_val_bvstring (base_var_loop, "ST$Health$q", (MMS_BVSTRING *) &health_q, 13);
      if (retcode)
        {
        MVL_LOG_ERR1 ("Cannot find %s$ST$Health$q or type is not Quality", base_var_loop->name);
        retcode = SD_FAILURE;
        break;	/* stop on first error	*/
        }
      else
        {
        /* If any bit is set in Health$q of this LN, then set LLN0 Health to "Questionable".*/
        if (health_q.data[0] != 0 || (health_q.data[1] & 0xF8) != 0)
          lln0_health_q.data [0] = 0xC0;	/* Questionable	*/
        }
      }
    }

  if (retcode == SD_SUCCESS)
    {
    memcpy (mvluRdVaCtrl->primData, &lln0_health_q, sizeof (MMS_BVSTRING_Q));	/* copy data	*/
    }
  mvlu_rd_prim_done (mvluRdVaCtrl, retcode);
  }
/************************************************************************/
/*			lln0_health_q_rd_scan_done			*/
/* Function called when scan started by "lln0_health_q_rd_scan_start"*/
/* completes.								*/
/* RETURNS:	SD_SUCCESS (caller ignores return anyway)		*/
/* NOTE: this function only used for Health in LLN0.			*/
/************************************************************************/
static ST_RET lln0_health_q_rd_scan_done (MVL_IND_PEND *indCtrl)
  {
MVLU_RD_VA_CTRL *mvluRdVaCtrl;	/* pointer to original Read request	*/
ST_RET retcode;

  /* CRITICAL: Get pointer to original request BEFORE freeing indCtrl	*/
  /*           (saved in "indCtrl->usr" when scan started).		*/
  mvluRdVaCtrl = (MVLU_RD_VA_CTRL *) indCtrl->usr;

  /* Get result of scan and free "indCtrl".	*/
  retcode = mvlu_scan_read_lov_cleanup (indCtrl);

  /* Scan is done and cleaned up. Finish processing.		*/
  /* If scan failed, stop now. Else, call finish function.	*/
  if (retcode)
    mvlu_rd_prim_done (mvluRdVaCtrl, retcode);
  else
    lln0_health_q_rd_finish (mvluRdVaCtrl);

  return (SD_SUCCESS);	/* NOTE: ignored by caller	*/
  }
/************************************************************************/
/*			lln0_health_q_rd_scan_start			*/
/* Function called to start scan.					*/
/* NOTE: this function only used for Health in LLN0.			*/
/************************************************************************/
static ST_VOID lln0_health_q_rd_scan_start (MVLU_RD_VA_CTRL *mvluRdVaCtrl)
  {
MVL_DOM_CTRL *dom;		/* domain containing this variable	*/
MVL_VAR_ASSOC *base_var;
MVL_VAR_ASSOC *base_var_loop;	/* used in loop through domain vars	*/

ST_RET retcode = SD_SUCCESS;
MVL_VAR_ASSOC **var_array;
MVL_SCOPE **scope_array;
ST_INT j;	/* index to arrays	*/
ST_INT num_var;
ST_INT array_size;
MVL_VAR_ASSOC *tmp_var;	/* temporary var	*/

  base_var = mvluRdVaCtrl->rdVaCtrl->va->base_va;

  /* Assume scope is DOM_SPEC	*/
  dom = mvluRdVaCtrl->rdVaCtrl->va_scope.dom;

  array_size = dom->num_var_assoc;
  var_array =   (MVL_VAR_ASSOC **) chk_calloc (array_size, sizeof (MVL_VAR_ASSOC *));
  scope_array = (MVL_SCOPE **)     chk_calloc (array_size, sizeof (MVL_SCOPE *));

  /* Make temporary vars.	*/
  num_var = 0;

  for (j = 0; j < dom->num_var_assoc; j++)
    {
    base_var_loop = dom->var_assoc_tbl[j];
    /* Skip LLN0 because we're computing LLN0 Health from others.	*/
    /* Skip LPHD because it doesn't contain Health.			*/
    /* Use "strstr" because name may have prefix and suffix.		*/
    /* Set same scope for all variables.				*/
    if (base_var_loop != base_var  &&			/* not LLN0	*/
        strstr (base_var_loop->name, "LPHD") == NULL)	/* not LPHD	*/
      {	/* this base var is "not" LLN0 or LPHD	*/
      /* If correct attribute found in this LN, add it to arrays.	*/
      tmp_var = mvlu_flat_var_create (base_var_loop, "ST$Health$q");
      if (tmp_var)
        {
        scope_array [num_var] = &mvluRdVaCtrl->rdVaCtrl->va_scope;
        var_array [num_var] = tmp_var;
        num_var++;
        }
      }
    }

  /* If temporary vars created, start a scan, else respond immediately.*/
  if (num_var)
    {
    /* Start scan.	*/
    mvlu_scan_read_lov_start (num_var,
        var_array,		/* array of variables	*/
        scope_array,		/* array of scopes	*/
        mvluRdVaCtrl,		/* ptr saved in indCtrl->usr	*/
        lln0_health_q_rd_scan_done);/* this function is called when scan is done*/
    }
  else
    {
    /* Just return default value for Quality (should be all 0).	*/
    mvlu_rd_prim_done (mvluRdVaCtrl, retcode);
    }
  /* Free temporary arrays created at top of function.	*/
  chk_free (var_array);
  chk_free (scope_array);
  }
/************************************************************************/
/*			lln0_health_q_rd_ind				*/
/* Leaf function called for "LLN0$ST$Health$q" attribute.		*/
/* It combines "Health$q" values from all other Logical Nodes.		*/
/************************************************************************/
ST_VOID lln0_health_q_rd_ind (MVLU_RD_VA_CTRL *mvluRdVaCtrl)
  {
#ifdef USE_RECURSIVE_READ
  /* This starts a scan to get the necessary data.			*/
  /* When scan completes, lln0_health_q_rd_finish is called to build response.*/
  lln0_health_q_rd_scan_start (mvluRdVaCtrl);
#else
  /* Assume data in local memory is up to date and use it to build response.*/ 
  lln0_health_q_rd_finish (mvluRdVaCtrl);
#endif
  }

/*======================================================================*/
/* Function for "LLN0$ST$Health$t" Read Indication.			*/
/*======================================================================*/

/************************************************************************/
/*			lln0_health_t_rd_finish			*/
/* Function called to finish processing for "lln0_health_t_rd_ind".	*/
/* Get necessary data from local memory, compute Health$t,		*/
/* and finish (i.e. call mvlu_rd_prim_done).				*/
/* NOTE: this function only used for Health in LLN0.			*/
/************************************************************************/
static ST_VOID lln0_health_t_rd_finish (MVLU_RD_VA_CTRL *mvluRdVaCtrl)
  {
MVL_VAR_ASSOC *base_var;
MVL_VAR_ASSOC *base_var_loop;	/* used in loop through domain vars	*/
MMS_UTC_TIME *lln0_health_t;	/* Value to return (LLN0$ST$Health$t)*/
MMS_UTC_TIME *health_t;		/* Value of Health$t in other LN.	*/
ST_RET retcode;
ST_INT j;
MVL_DOM_CTRL *dom;
RUNTIME_TYPE *rt_type;

  base_var = mvluRdVaCtrl->rdVaCtrl->va->base_va;

  lln0_health_t = (MMS_UTC_TIME *) mvluRdVaCtrl->primData;	/* set data ptr*/

  /* Assume scope is DOM_SPEC	*/
  dom = mvluRdVaCtrl->rdVaCtrl->va_scope.dom;

  memset (lln0_health_t, 0, sizeof(MMS_UTC_TIME));	/* initial value*/
  retcode = SD_SUCCESS;		/* change if error occurs.	*/
  for (j = 0; j < dom->num_var_assoc; j++)
    {
    base_var_loop = dom->var_assoc_tbl[j];

    /* Skip LLN0 because we're computing LLN0 Health from others.	*/
    /* Skip LPHD because it doesn't contain Health.			*/
    /* Use "strstr" because name may have prefix and suffix.		*/
    if (base_var_loop != base_var  &&			/* not LLN0	*/
        strstr (base_var_loop->name, "LPHD") == NULL)	/* not LPHD	*/
      {
      health_t = (MMS_UTC_TIME *) mvlu_get_leaf_data_ptr (base_var_loop, "ST$Health$t", &rt_type);
      if (health_t == NULL || rt_type->el_tag != RT_UTC_TIME)
        {
        MVL_LOG_ERR1 ("Cannot find %s$ST$Health$t or type is not Timestamp", base_var_loop->name);
        retcode = SD_FAILURE;
        break;	/* stop on first error	*/
        }
      else
        {
        if (health_t->secs > lln0_health_t->secs  ||
            (health_t->secs == lln0_health_t->secs && health_t->fraction > lln0_health_t->fraction))
          memcpy (lln0_health_t, health_t, sizeof(MMS_UTC_TIME));	/* val later than current val, so update it*/
        }
      }
    }
  mvlu_rd_prim_done (mvluRdVaCtrl, retcode);
  }
/************************************************************************/
/*			lln0_health_t_rd_scan_done			*/
/* Function called when scan started by "lln0_health_t_rd_scan_start"*/
/* completes.								*/
/* RETURNS:	SD_SUCCESS (caller ignores return anyway)		*/
/* NOTE: this function only used for Health in LLN0.			*/
/************************************************************************/
static ST_RET lln0_health_t_rd_scan_done (MVL_IND_PEND *indCtrl)
  {
MVLU_RD_VA_CTRL *mvluRdVaCtrl;	/* pointer to original Read request	*/
ST_RET retcode;

  /* CRITICAL: Get pointer to original request BEFORE freeing indCtrl	*/
  /*           (saved in "indCtrl->usr" when scan started).		*/
  mvluRdVaCtrl = (MVLU_RD_VA_CTRL *) indCtrl->usr;

  /* Get result of scan and free "indCtrl".	*/
  retcode = mvlu_scan_read_lov_cleanup (indCtrl);

  /* Scan is done and cleaned up. Finish processing.		*/
  /* If scan failed, stop now. Else, call finish function.	*/
  if (retcode)
    mvlu_rd_prim_done (mvluRdVaCtrl, retcode);
  else
    lln0_health_t_rd_finish (mvluRdVaCtrl);

  return (SD_SUCCESS);	/* NOTE: ignored by caller	*/
  }
/************************************************************************/
/*			lln0_health_t_rd_scan_start			*/
/* Function called to start scan.					*/
/* NOTE: this function only used for Health in LLN0.			*/
/************************************************************************/
static ST_VOID lln0_health_t_rd_scan_start (MVLU_RD_VA_CTRL *mvluRdVaCtrl)
  {
MVL_DOM_CTRL *dom;		/* domain containing this variable	*/
MVL_VAR_ASSOC *base_var;
MVL_VAR_ASSOC *base_var_loop;	/* used in loop through domain vars	*/

ST_RET retcode = SD_SUCCESS;
MVL_VAR_ASSOC **var_array;
MVL_SCOPE **scope_array;
ST_INT j;	/* index to arrays	*/
ST_INT num_var;
ST_INT array_size;
MVL_VAR_ASSOC *tmp_var;	/* temporary var	*/
MMS_UTC_TIME *lln0_health_t;

  base_var = mvluRdVaCtrl->rdVaCtrl->va->base_va;

  /* Assume scope is DOM_SPEC	*/
  dom = mvluRdVaCtrl->rdVaCtrl->va_scope.dom;

  array_size = dom->num_var_assoc;
  var_array =   (MVL_VAR_ASSOC **) chk_calloc (array_size, sizeof (MVL_VAR_ASSOC *));
  scope_array = (MVL_SCOPE **)     chk_calloc (array_size, sizeof (MVL_SCOPE *));

  /* Make temporary vars.	*/
  num_var = 0;

  for (j = 0; j < dom->num_var_assoc; j++)
    {
    base_var_loop = dom->var_assoc_tbl[j];
    /* Skip LLN0 because we're computing LLN0 Health from others.	*/
    /* Skip LPHD because it doesn't contain Health.			*/
    /* Use "strstr" because name may have prefix and suffix.		*/
    /* Set same scope for all variables.				*/
    if (base_var_loop != base_var  &&			/* not LLN0	*/
        strstr (base_var_loop->name, "LPHD") == NULL)	/* not LPHD	*/
      {	/* this base var is "not" LLN0 or LPHD	*/
      /* If correct attribute found in this LN, add it to arrays.	*/
      tmp_var = mvlu_flat_var_create (base_var_loop, "ST$Health$t");
      if (tmp_var)
        {
        scope_array [num_var] = &mvluRdVaCtrl->rdVaCtrl->va_scope;
        var_array [num_var] = tmp_var;
        num_var++;
        }
      }
    }

  /* If temporary vars created, start a scan, else respond immediately.*/
  if (num_var)
    {
    /* Start scan.	*/
    mvlu_scan_read_lov_start (num_var,
        var_array,		/* array of variables	*/
        scope_array,		/* array of scopes	*/
        mvluRdVaCtrl,		/* ptr saved in indCtrl->usr	*/
        lln0_health_t_rd_scan_done);/* this function is called when scan is done*/
    }
  else
    {
    /* Just return current time.	*/
    lln0_health_t = (MMS_UTC_TIME *) mvluRdVaCtrl->primData;	/* set data ptr*/
    lln0_health_t->secs = (ST_UINT32) time(NULL);	/* just set seconds in UTC time.*/
    lln0_health_t->fraction = 0;
    lln0_health_t->qflags = 0;
    mvlu_rd_prim_done (mvluRdVaCtrl, retcode);
    }
  /* Free temporary arrays created at top of function.	*/
  chk_free (var_array);
  chk_free (scope_array);
  }
/************************************************************************/
/*			lln0_health_t_rd_ind				*/
/* Leaf function called for "LLN0$ST$Health$t" attribute.		*/
/* It combines "Health$t" values from all other Logical Nodes.		*/
/************************************************************************/
ST_VOID lln0_health_t_rd_ind (MVLU_RD_VA_CTRL *mvluRdVaCtrl)
  {
#ifdef USE_RECURSIVE_READ
  /* This starts a scan to get the necessary data.			*/
  /* When scan completes, lln0_health_t_rd_finish is called to build response.*/
  lln0_health_t_rd_scan_start (mvluRdVaCtrl);
#else
  /* Assume data in local memory is up to date and use it to build response.*/ 
  lln0_health_t_rd_finish (mvluRdVaCtrl);
#endif
  }

