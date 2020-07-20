/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	2004-2008 All Rights Reserved					*/
/*									*/
/* MODULE NAME : userleaf2.c						*/
/* PRODUCT(S)  : MMS-EASE-LITE						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*	User customized "leaf" function.				*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*			u_custom_rd_ind					*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev     Comments					*/
/* --------  ---  ------   -------------------------------------------  */
/* 06/10/10  JRB	   Fix log message.				*/
/* 02/09/09  JRB    03     If db calc fails, log & ret OBJ_VALUE_INVALID.*/
/* 12/18/08  JRB    02     Added deadband calculation & simulation code.*/
/* 12/05/08  JRB    01     Initial Revision. Moved code from userleaf.c	*/
/*			   to here (easier to customize or replace).	*/
/************************************************************************/
#include "glbtypes.h"
#include "sysincs.h"
#include "usermap.h"	/* for DATA_MAP, etc.	*/
#include "slog.h"
#include "mvl_defs.h"
#include "mvl_uca.h"
#include "mvl_log.h"
#include "db_61850.h"	/* for deadband calculations	*/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/* Simulation functions. Called only if SIMULATE_INSTMAG_INCREASING	*/
/* or SIMULATE_INSTMAG_DECREASING defined (do not define both).		*/

#if defined(SIMULATE_INSTMAG_INCREASING)
/************************************************************************/
/* Simulate all instMag$i values "increasing" through entire range.	*/
/************************************************************************/
ST_VOID simulate_instmag_i_increasing (DEADBAND_INT32 *deadband, ST_UCHAR el_tag)
  {
  (*deadband->instMag_i)++;	/* simulate increasing value	*/
  /* If out of range, start at min	*/
  if (*deadband->instMag_i > *deadband->max_i || *deadband->instMag_i < *deadband->min_i)
    *deadband->instMag_i = *deadband->min_i;	/* Make sure val in range*/
  }
/************************************************************************/
/* Simulate all instMag$f values "increasing" through entire range.	*/
/************************************************************************/
ST_VOID simulate_instmag_f_increasing (DEADBAND_FLOAT *deadband, ST_UCHAR el_tag)
  {
  (*deadband->instMag_f)++;	/* simulate increasing value	*/
  /* If out of range, start at min	*/
  if (*deadband->instMag_f > *deadband->max_f || *deadband->instMag_f < *deadband->min_f)
    *deadband->instMag_f = *deadband->min_f;	/* Make sure val in range*/
  }
#endif	/* defined(SIMULATE_INSTMAG_INCREASING)	*/

#if defined(SIMULATE_INSTMAG_DECREASING)
/************************************************************************/
/* Simulate all instMag$i values "decreasing" through entire range.	*/
/************************************************************************/
ST_VOID simulate_instmag_i_decreasing (DEADBAND_INT32 *deadband, ST_UCHAR el_tag)
  {
  (*deadband->instMag_i)--;	/* simulate decreasing value	*/
  /* If out of range, start at max	*/
  if (*deadband->instMag_i > *deadband->max_i || *deadband->instMag_i < *deadband->min_i)
    *deadband->instMag_i = *deadband->max_i;	/* Make sure val in range*/
  }
/************************************************************************/
/* Simulate all instMag$i values "decreasing" through entire range.	*/
/************************************************************************/
ST_VOID simulate_instmag_f_decreasing (DEADBAND_FLOAT *deadband, ST_UCHAR el_tag)
  {
  (*deadband->instMag_f)--;	/* simulate decreasing value	*/
  /* If out of range, start at max	*/
  if (*deadband->instMag_f > *deadband->max_f || *deadband->instMag_f < *deadband->min_f)
    *deadband->instMag_f = *deadband->max_f;	/* Make sure val in range*/
  }
#endif	/* defined(SIMULATE_INSTMAG_DECREASING)	*/

/************************************************************************/
/*			u_custom_rd_ind					*/
/************************************************************************/
ST_VOID u_custom_rd_ind (MVLU_RD_VA_CTRL *mvluRdVaCtrl)
  {
MVL_VAR_ASSOC *base_var;
DATA_MAP_HEAD *map_head_sorted;	/* head of array of (DATA_MAP *) for this var.*/
DATA_MAP *map_entry;		/* DATA_MAP for this bit of data.	*/
ST_RET retcode = SD_SUCCESS;

  /* Use new "prim_num" to find (DATA_MAP *).	*/
  base_var = mvluRdVaCtrl->rdVaCtrl->va->base_va;

  assert (base_var->use_static_data);	/* funct only works in this mode*/

  map_head_sorted = (DATA_MAP_HEAD *) base_var->user_info;	/* it was saved here*/
  if (map_head_sorted == NULL)
    {		/* THIS IS A SERIOUS ERROR	*/
    MVL_LOG_ERR1 ("DATA mapping error: variable '%s' not mapped", base_var->name);
    retcode = SD_FAILURE;
    }
  else if (mvluRdVaCtrl->prim_num >= map_head_sorted->map_count)
    {		/* THIS IS A SERIOUS ERROR	*/
    MVL_LOG_ERR1 ("DATA mapping error: prim_num=%d exceeds limit", mvluRdVaCtrl->prim_num);
    retcode = SD_FAILURE;
    }
  else
    {
    map_entry = map_head_sorted->map_arr [mvluRdVaCtrl->prim_num];

    /* CRITICAL: "map_entry" may be NULL (i.e. this leaf not mapped).	*/
    if (map_entry)
      {
      /* CRITICAL: this chks that all mapping was correct.	*/
      if (map_entry->dataPtr != mvluRdVaCtrl->primData)
        {		/* THIS IS A SERIOUS ERROR	*/
        MVL_LOG_ERR2 ("DATA mapping error: map_entry->dataPtr=" S_FMT_PTR " mvluRdVaCtrl->primData=" S_FMT_PTR,
                  map_entry->dataPtr, mvluRdVaCtrl->primData);
        retcode = SD_FAILURE;
        }
      }
    }

#if defined(SIMULATE_INSTMAG_DECREASING)
  if (retcode == SD_SUCCESS && map_entry && map_entry->deadband_info)
    {
    if (mvluRdVaCtrl->rt->el_tag == RT_INTEGER)
      simulate_instmag_i_decreasing ((DEADBAND_INT32 *) map_entry->deadband_info, mvluRdVaCtrl->rt->el_tag);
    else if (mvluRdVaCtrl->rt->el_tag == RT_FLOATING_POINT)
      simulate_instmag_f_decreasing ((DEADBAND_FLOAT *) map_entry->deadband_info, mvluRdVaCtrl->rt->el_tag);
    }
#endif	/* defined(SIMULATE_INSTMAG_DECREASING)	*/

#if defined(SIMULATE_INSTMAG_INCREASING)
  if (retcode == SD_SUCCESS && map_entry && map_entry->deadband_info)
    {
    if (mvluRdVaCtrl->rt->el_tag == RT_INTEGER)
      simulate_instmag_i_increasing ((DEADBAND_INT32 *) map_entry->deadband_info, mvluRdVaCtrl->rt->el_tag);
    else if (mvluRdVaCtrl->rt->el_tag == RT_FLOATING_POINT)
      simulate_instmag_f_increasing ((DEADBAND_FLOAT *) map_entry->deadband_info, mvluRdVaCtrl->rt->el_tag);
    }
#endif	/* defined(SIMULATE_INSTMAG_INCREASING)	*/
    
  /* Do deadband calculations if necessary.				*/
  /* NOTE: add this in any leaf function where deadbanding needed.	*/
  if (retcode == SD_SUCCESS && map_entry && map_entry->deadband_info)
    {
    /* Call appropriate deadband function for ST_INT32 or ST_FLOAT.	*/
    /* NOTE: these functions compute the deadband and may adjust the value*/
    /*       at mvluRdVaCtrl->primData. Nothing else needs to be done.	*/
    if (map_entry->dataType->el_tag == RT_INTEGER)
      retcode = deadband_int32_calculation ((ST_INT32 *) mvluRdVaCtrl->primData,
                (DEADBAND_INT32 *) map_entry->deadband_info);
    else if (map_entry->dataType->el_tag == RT_FLOATING_POINT)
      retcode = deadband_float_calculation ((ST_FLOAT *) mvluRdVaCtrl->primData,
                (DEADBAND_FLOAT *) map_entry->deadband_info);
    if (retcode)
      {
      MVL_LOG_ERR1 ("Deadband calculation failed for attribute '%s'.", map_entry->leaf);
      mvluRdVaCtrl->rdVaCtrl->failure = ARE_OBJ_VALUE_INVALID;
      }
    }

  /* NOTE: Data is already in static buffer. Nothing else to do.
   * Notice the 2 possible mapping errors above. If one of these errors
   * occurred, retcode==SD_FAILURE. If not, retcode==SD_SUCCESS, and
   * the data in the static buffer is sent in the read response.
   */

  if (retcode==SD_SUCCESS)
    set_rpt_reason (mvluRdVaCtrl);	/* set va->rpt_reason if necessary.*/

  mvlu_rd_prim_done (mvluRdVaCtrl, retcode);
  }

  

