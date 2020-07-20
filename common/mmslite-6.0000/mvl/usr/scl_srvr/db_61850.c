/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	2009-2009 All Rights Reserved					*/
/*									*/
/* MODULE NAME : db_61850.c						*/
/* PRODUCT(S)  : MMS-EASE-LITE						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*	Functions to configure & compute deadband for IEC 61850.	*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev     Comments					*/
/* --------  ---  ------   -------------------------------------------  */
/* 02/09/09  JRB    02	   Handle (max<min), (max==min), or (db==0).	*/
/* 01/05/09  JRB    01     Initial Revision.				*/
/************************************************************************/
#include "glbtypes.h"
#include "sysincs.h"
#include "db_61850.h"
#include "slog.h"
#include "mvl_defs.h"
#include "mvl_uca.h"
#include "mvl_log.h"

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/
/*			deadband_int32_create_2				*/
/* Get Deadband information for "integer" attribute (...$i).		*/
/* This function allocates a DEADBAND_INT32 structure, fills it in, 	*/
/* and returns a pointer to it.						*/
/* NOTE: "dbAng" and "rangeAngC" added to CMV for "ang" deadband	*/
/*       calculations in IEC 61850-7-3 Ed 2. This function assumes Ed 2.*/
/************************************************************************/
DEADBAND_INT32 *deadband_int32_create_2 (MVL_VAR_ASSOC *base_var,
	ST_CHAR *leaf,
	ST_INT type)	/* DEADBAND_TYPE_MAG      OR	*/
			/* DEADBAND_TYPE_CVAL_MAG OR	*/
			/* DEADBAND_TYPE_CVAL_ANG	*/
  {
DEADBAND_INT32 *deadband;
ST_CHAR *firstdollar;	/* ptr to first '$' in var name	*/
/* Allow extra space in "tmp_buf" so string manipulation is always safe.*/
ST_CHAR tmp_buf [MAX_IDENT_LEN+20+1];	/* allow extra space	*/
ST_CHAR *tmp_ptr;	/* ptr where "critical text" found in tmp_buf	*/
RUNTIME_TYPE *rt_type;	/* type of each deadband parameter found	*/
ST_BOOLEAN done = SD_FALSE;
ST_CHAR *text_to_find;

  switch (type)
    {
    case DEADBAND_TYPE_MAG:
      text_to_find = "mag$i";
      break;
    case DEADBAND_TYPE_CVAL_MAG:
      text_to_find = "cVal$mag$i";
      break;
    case DEADBAND_TYPE_CVAL_ANG:
      text_to_find = "cVal$ang$i";
      break;
    default:
      assert (0);	/* For now, caller must be careful	*/
    }
     
  deadband = (DEADBAND_INT32 *) chk_calloc (1, sizeof (DEADBAND_INT32));
  do
    {	/* loop just to make it easier to break out on any error	*/
    firstdollar = strchr (leaf, '$');		/* find first '$'	*/
    if (firstdollar==NULL)	/* should never happen.	*/
      {
      MVL_LOG_ERR1 ("No '$' in leaf name '%s'", leaf);
      break;
      }

    strcpy (tmp_buf, firstdollar+1);	/* copy name after first '$'	*/
    /* Find position of "critical text" in temporary buf.	*/
    tmp_ptr = strstr (tmp_buf, text_to_find);
    if (tmp_ptr==NULL)		/* should never happen.	*/
      {
      MVL_LOG_ERR2 ("'%s' not found in leaf name '%s'", text_to_find, leaf);
      break;
      }

    switch (type)
      {
      case DEADBAND_TYPE_MAG:
        strcpy (tmp_ptr, "instMag$i");		/* replace "critical text"*/
        break;
      case DEADBAND_TYPE_CVAL_MAG:
        strcpy (tmp_ptr, "instCVal$mag$i");	/* replace "critical text"*/
        break;
      case DEADBAND_TYPE_CVAL_ANG:
        strcpy (tmp_ptr, "instCVal$ang$i");	/* replace "critical text"*/
        break;
      default:
        assert (0);	/* For now, caller must be careful	*/
      }
    deadband->instMag_i = (ST_INT32 *) mvlu_get_leaf_data_ptr (base_var, tmp_buf, &rt_type);
    if (deadband->instMag_i==NULL)
      {
      MVL_LOG_ERR2 ("Deadband param '%s' not found for leaf '%s'", tmp_buf, leaf);
      break;
      }
    else if (rt_type->el_tag!=RT_INTEGER || rt_type->u.p.el_len != 4)
      {
      MVL_LOG_ERR2 ("Incorrect type for Deadband param '%s' for leaf '%s'",
                   tmp_buf, leaf);
      break;
      }

    /* NOTE: this applies to the rest of the parameters.	*/
    strncpy (tmp_buf, "CF", 2);	/* replace "MX"  with "CF"	*/

    if (type == DEADBAND_TYPE_CVAL_ANG)	/* "ang" uses different attr*/
      strcpy (tmp_ptr, "dbAng");	/* replace "critical text"*/
    else
      strcpy (tmp_ptr, "db");		/* replace "critical text"*/
    deadband->db = (ST_UINT32 *) mvlu_get_leaf_data_ptr (base_var, tmp_buf, &rt_type);
    if (deadband->db==NULL)
      {
      MVL_LOG_ERR2 ("Deadband param '%s' not found for leaf '%s'", tmp_buf, leaf);
      break;
      }
    else if (rt_type->el_tag!=RT_UNSIGNED || rt_type->u.p.el_len != 4)
      {
      MVL_LOG_ERR2 ("Incorrect type for Deadband param '%s' for leaf '%s'",
                   tmp_buf, leaf);
      break;
      }

    if (type == DEADBAND_TYPE_CVAL_ANG)		/* "ang" uses different attr*/
      strcpy (tmp_ptr, "rangeAngC$min$i");	/* replace "critical text"*/
    else
      strcpy (tmp_ptr, "rangeC$min$i");		/* replace "critical text"*/
    deadband->min_i = (ST_INT32 *) mvlu_get_leaf_data_ptr (base_var, tmp_buf, &rt_type);
    if (deadband->min_i==NULL)
      {
      MVL_LOG_ERR2 ("Deadband param '%s' not found for leaf '%s'", tmp_buf, leaf);
      break;
      }
    else if (rt_type->el_tag!=RT_INTEGER || rt_type->u.p.el_len != 4)
      {
      MVL_LOG_ERR2 ("Incorrect type for Deadband param '%s' for leaf '%s'",
                   tmp_buf, leaf);
      break;
      }

    if (type == DEADBAND_TYPE_CVAL_ANG)		/* "ang" uses different attr*/
      strcpy (tmp_ptr, "rangeAngC$max$i");	/* replace "critical text"*/
    else
      strcpy (tmp_ptr, "rangeC$max$i");		/* replace "critical text"*/
    deadband->max_i = (ST_INT32 *) mvlu_get_leaf_data_ptr (base_var, tmp_buf, &rt_type);
    if (deadband->max_i==NULL)
      {
      MVL_LOG_ERR2 ("Deadband param '%s' not found for leaf '%s'", tmp_buf, leaf);
      break;
      }
    else if (rt_type->el_tag!=RT_INTEGER || rt_type->u.p.el_len != 4)
      {
      MVL_LOG_ERR2 ("Incorrect type for Deadband param '%s' for leaf '%s'",
                   tmp_buf, leaf);
      break;
      }

    done = SD_TRUE;
    } while (0);	/* end do-while loop	*/

  /* If anything failed in do-while loop, "done" will be SD_FALSE.	*/
  if (!done)
    {
    /* NOTE: already logged specific reason for error. Add this note.	*/
    MVL_LOG_CERR0 ("Deadband calculation will not be performed.");
    chk_free(deadband);
    deadband = NULL;
    }
  else
    MVLU_LOG_FLOW1 ("Deadband mapped for leaf '%s'", leaf);

  return (deadband);
  }
/************************************************************************/
/*			deadband_float_create_2				*/
/* Get Deadband information for "float" attribute (...$f).		*/
/* This function allocates a DEADBAND_FLOAT structure, fills it in, 	*/
/* and returns a pointer to it.						*/
/* NOTE: "dbAng" and "rangeAngC" added to CMV for "ang" deadband	*/
/*       calculations in IEC 61850-7-3 Ed 2. This function assumes Ed 2.*/
/************************************************************************/
DEADBAND_FLOAT *deadband_float_create_2 (MVL_VAR_ASSOC *base_var,
	ST_CHAR *leaf,
	ST_INT type)	/* DEADBAND_TYPE_MAG      OR	*/
			/* DEADBAND_TYPE_CVAL_MAG OR	*/
			/* DEADBAND_TYPE_CVAL_ANG	*/
  {
DEADBAND_FLOAT *deadband;
ST_CHAR *firstdollar;	/* ptr to first '$' in var name	*/
/* Allow extra space in "tmp_buf" so string manipulation is always safe.*/
ST_CHAR tmp_buf [MAX_IDENT_LEN+20+1];	/* allow extra space	*/
ST_CHAR *tmp_ptr;	/* ptr where "critical text" found in tmp_buf	*/
RUNTIME_TYPE *rt_type;	/* type of each deadband parameter found	*/
ST_BOOLEAN done = SD_FALSE;
ST_CHAR *text_to_find;

  switch (type)
    {
    case DEADBAND_TYPE_MAG:
      text_to_find = "mag$f";
      break;
    case DEADBAND_TYPE_CVAL_MAG:
      text_to_find = "cVal$mag$f";
      break;
    case DEADBAND_TYPE_CVAL_ANG:
      text_to_find = "cVal$ang$f";
      break;
    default:
      assert (0);	/* For now, caller must be careful	*/
    }
     
  deadband = (DEADBAND_FLOAT *) chk_calloc (1, sizeof (DEADBAND_FLOAT));
  do
    {	/* loop just to make it easier to break out on any error	*/
    firstdollar = strchr (leaf, '$');		/* find first '$'	*/
    if (firstdollar==NULL)	/* should never happen.	*/
      {
      MVL_LOG_ERR1 ("No '$' in leaf name '%s'", leaf);
      break;
      }

    strcpy (tmp_buf, firstdollar+1);	/* copy name after first '$'	*/
    /* Find position of "critical text" in temporary buf.	*/
    tmp_ptr = strstr (tmp_buf, text_to_find);
    if (tmp_ptr==NULL)		/* should never happen.	*/
      {
      MVL_LOG_ERR2 ("'%s' not found in leaf name '%s'", text_to_find, leaf);
      break;
      }

    switch (type)
      {
      case DEADBAND_TYPE_MAG:
        strcpy (tmp_ptr, "instMag$f");		/* replace "critical text"*/
        break;
      case DEADBAND_TYPE_CVAL_MAG:
        strcpy (tmp_ptr, "instCVal$mag$f");	/* replace "critical text"*/
        break;
      case DEADBAND_TYPE_CVAL_ANG:
        strcpy (tmp_ptr, "instCVal$ang$f");	/* replace "critical text"*/
        break;
      default:
        assert (0);	/* For now, caller must be careful	*/
      }
    deadband->instMag_f = (ST_FLOAT *) mvlu_get_leaf_data_ptr (base_var, tmp_buf, &rt_type);
    if (deadband->instMag_f==NULL)
      {
      MVL_LOG_ERR2 ("Deadband param '%s' not found for leaf '%s'", tmp_buf, leaf);
      break;
      }
    else if (rt_type->el_tag!=RT_FLOATING_POINT || rt_type->u.p.el_len != 4)
      {
      MVL_LOG_ERR2 ("Incorrect type for Deadband param '%s' for leaf '%s'",
                   tmp_buf, leaf);
      break;
      }

    /* NOTE: this applies to the rest of the parameters.	*/
    strncpy (tmp_buf, "CF", 2);	/* replace "MX"  with "CF"	*/

    if (type == DEADBAND_TYPE_CVAL_ANG)	/* "ang" uses different attr*/
      strcpy (tmp_ptr, "dbAng");	/* replace "critical text"*/
    else
      strcpy (tmp_ptr, "db");		/* replace "critical text"*/
    deadband->db = (ST_UINT32 *) mvlu_get_leaf_data_ptr (base_var, tmp_buf, &rt_type);
    if (deadband->db==NULL)
      {
      MVL_LOG_ERR2 ("Deadband param '%s' not found for leaf '%s'", tmp_buf, leaf);
      break;
      }
    else if (rt_type->el_tag!=RT_UNSIGNED || rt_type->u.p.el_len != 4)
      {
      MVL_LOG_ERR2 ("Incorrect type for Deadband param '%s' for leaf '%s'",
                   tmp_buf, leaf);
      break;
      }

    if (type == DEADBAND_TYPE_CVAL_ANG)		/* "ang" uses different attr*/
      strcpy (tmp_ptr, "rangeAngC$min$f");	/* replace "critical text"*/
    else
      strcpy (tmp_ptr, "rangeC$min$f");		/* replace "critical text"*/
    deadband->min_f = (ST_FLOAT *) mvlu_get_leaf_data_ptr (base_var, tmp_buf, &rt_type);
    if (deadband->min_f==NULL)
      {
      MVL_LOG_ERR2 ("Deadband param '%s' not found for leaf '%s'", tmp_buf, leaf);
      break;
      }
    else if (rt_type->el_tag!=RT_FLOATING_POINT || rt_type->u.p.el_len != 4)
      {
      MVL_LOG_ERR2 ("Incorrect type for Deadband param '%s' for leaf '%s'",
                   tmp_buf, leaf);
      break;
      }

    if (type == DEADBAND_TYPE_CVAL_ANG)		/* "ang" uses different attr*/
      strcpy (tmp_ptr, "rangeAngC$max$f");	/* replace "critical text"*/
    else
      strcpy (tmp_ptr, "rangeC$max$f");		/* replace "critical text"*/
    deadband->max_f = (ST_FLOAT *) mvlu_get_leaf_data_ptr (base_var, tmp_buf, &rt_type);
    if (deadband->max_f==NULL)
      {
      MVL_LOG_ERR2 ("Deadband param '%s' not found for leaf '%s'", tmp_buf, leaf);
      break;
      }
    else if (rt_type->el_tag!=RT_FLOATING_POINT || rt_type->u.p.el_len != 4)
      {
      MVL_LOG_ERR2 ("Incorrect type for Deadband param '%s' for leaf '%s'",
                   tmp_buf, leaf);
      break;
      }

    done = SD_TRUE;
    } while (0);	/* end do-while loop	*/

  /* If anything failed in do-while loop, "done" will be SD_FALSE.	*/
  if (!done)
    {
    /* NOTE: already logged specific reason for error. Add this note.	*/
    MVL_LOG_CERR0 ("Deadband calculation will not be performed.");
    chk_free(deadband);
    deadband = NULL;
    }
  else
    MVLU_LOG_FLOW1 ("Deadband mapped for leaf '%s'", leaf);

  return (deadband);
  }

/************************************************************************/
/*			deadband_int32_create				*/
/* Map Deadband information for attributes of CDCs MV and CMV		*/
/* by allocating and initializing appropriate "deadband" structure	*/
/* (either DEADBAND_FLOAT or DEADBAND_INT32).				*/
/*									*/
/* Find "i" or "f" inside "mag" and "ang" structures.			*/
/* If "mag" or "ang" is inside "cVal", do mapping.			*/
/* If "mag" is inside "normal" MV (TotW, Hz, etc), do mapping.		*/
/* But ignore "mag" inside "instCVal" or "subCVal" or array (deadband	*/
/* does not apply to these attributes).					*/
/*									*/
/* RETURNS:	ptr to allocated structure cast to (ST_VOID *) OR	*/
/*		NULL if structure not allocated.			*/
/*									*/
/* CRITICAL: chk for cVal$mag$i before mag$i (processed differently).	*/
/************************************************************************/
DEADBAND_INT32 *deadband_int32_create (MVL_VAR_ASSOC *base_var, ST_CHAR *leaf)
  {
ST_CHAR *lastdollar;	/* ptr to last '$' in leaf name	*/
DEADBAND_INT32 *deadband_info = NULL;	/* return value	(NULL if deadband not created)*/

  lastdollar = strrchr (leaf, '$');		/* find last '$'*/
  if (lastdollar == NULL)	/* should never fail	*/
    {
    MVL_LOG_ERR1 ("No '$' in leaf name '%s'", leaf);
    return (NULL);
    }
  /* Perform multiple string checks only if leaf is "i" or "f". */
  if (strcmp (lastdollar, "$i") == 0)
    {
    /* Get deadband information for "integers".				*/
    /* NOTE: deadband_info is (ST_VOID *).				*/
    /*       Must cast back to (DEADBAND_INT32 *) later to use it.	*/
    if      (strstr (leaf, "cVal$mag$i") != NULL)
      deadband_info = deadband_int32_create_2 (base_var, leaf,
                                 DEADBAND_TYPE_CVAL_MAG);
    else if (strstr (leaf, "cVal$ang$i") != NULL)
      deadband_info = deadband_int32_create_2 (base_var, leaf,
                                 DEADBAND_TYPE_CVAL_ANG);
    else if (strstr (leaf, "$mag$i") != NULL &&
             strstr (leaf, "instCVal$mag$i") == NULL &&
             strstr (leaf, "subCVal$mag$i") == NULL &&
             strstr (leaf, "]$mag$i") == NULL)	/* don't want mag inside array*/
      deadband_info = deadband_int32_create_2 (base_var, leaf,
                                 DEADBAND_TYPE_MAG);
    }
  return (deadband_info);
  }
/************************************************************************/
/*			deadband_float_create				*/
/* Map Deadband information for attributes of CDCs MV and CMV		*/
/* by allocating and initializing appropriate "deadband" structure	*/
/* (either DEADBAND_FLOAT or DEADBAND_INT32).				*/
/*									*/
/* Find "i" or "f" inside "mag" and "ang" structures.			*/
/* If "mag" or "ang" is inside "cVal", do mapping.			*/
/* If "mag" is inside "normal" MV (TotW, Hz, etc), do mapping.		*/
/* But ignore "mag" inside "instCVal" or "subCVal" or array (deadband	*/
/* does not apply to these attributes).					*/
/*									*/
/* RETURNS:	ptr to allocated structure cast to (ST_VOID *) OR	*/
/*		NULL if structure not allocated.			*/
/*									*/
/* CRITICAL: chk for cVal$mag$i before mag$i (processed differently).	*/
/************************************************************************/
DEADBAND_FLOAT *deadband_float_create (MVL_VAR_ASSOC *base_var, ST_CHAR *leaf)
  {
ST_CHAR *lastdollar;	/* ptr to last '$' in leaf name	*/
DEADBAND_FLOAT *deadband_info = NULL;	/* return value	(NULL if deadband not created)*/

  lastdollar = strrchr (leaf, '$');		/* find last '$'*/
  if (lastdollar == NULL)	/* should never fail	*/
    {
    MVL_LOG_ERR1 ("No '$' in leaf name '%s'", leaf);
    return (NULL);
    }
  /* Perform multiple string checks only if leaf is "f". */
  if (strcmp (lastdollar, "$f") == 0)
    {
    /* Get deadband information for "floats".				*/
    /* NOTE: deadband_info is (ST_VOID *).				*/
    /*       Must cast back to (DEADBAND_FLOAT *) later to use it.	*/
    if      (strstr (leaf, "cVal$mag$f") != NULL)
      deadband_info = deadband_float_create_2 (base_var, leaf,
                                 DEADBAND_TYPE_CVAL_MAG);
    else if (strstr (leaf, "cVal$ang$f") != NULL)
      deadband_info = deadband_float_create_2 (base_var, leaf,
                                 DEADBAND_TYPE_CVAL_ANG);
    else if (strstr (leaf, "$mag$f") != NULL &&
             strstr (leaf, "instCVal$mag$f") == NULL &&
             strstr (leaf, "subCVal$mag$f") == NULL &&
             strstr (leaf, "]$mag$f") == NULL)	/* don't want mag inside array*/
      deadband_info = deadband_float_create_2 (base_var, leaf,
                                 DEADBAND_TYPE_MAG);
    }
  return (deadband_info);
  }

/************************************************************************/
/*				deadband_create				*/
/* If this "leaf" requires deadbanding, this function allocates a	*/
/* DEADBAND_INT32 or DEADBAND_FLOAT struct and returns a ptr to it,	*/
/* casted to (ST_VOID *). Must cast back before using.			*/
/* Returns NULL if deadbanding not required.				*/
/************************************************************************/
ST_VOID *deadband_create (
	MVL_VAR_ASSOC *base_var,	/* base var (Logical Node)	*/
	ST_CHAR *leaf_name,		/* flattened name of this leaf	*/
	RUNTIME_TYPE *rt_type)		/* type of this leaf		*/
  {
ST_VOID *ptr=NULL;	/* ptr to be returned	*/

  if (rt_type->el_tag == RT_INTEGER)
    ptr = deadband_int32_create (base_var, leaf_name);
  else if (rt_type->el_tag == RT_FLOATING_POINT)
    ptr = deadband_float_create (base_var, leaf_name);
  return (ptr);		/* NULL for most leafs	*/
  }

/************************************************************************/
/*			deadband_int32_calculation			*/
/************************************************************************/
ST_RET deadband_int32_calculation (ST_INT32 *pMag, DEADBAND_INT32 *deadband)
  {
/* Local variables for faster processing.	*/
ST_INT32 instMag_i;
ST_INT32 mag;		/* old/new value	*/
ST_INT32 min_i, max_i, diff_i;
ST_UINT32 db;
double step_size;
int num_steps;

  /* Copy data to local variables.	*/
  instMag_i = *deadband->instMag_i;
  db        = *deadband->db;
  min_i     = *deadband->min_i;
  max_i     = *deadband->max_i;

  if (max_i < min_i)
    {
    /* Invalid range. Can't do deadband calculation.	*/
    MVL_LOG_ERR0 ("max$i < min$i so can't compute deadband");
    return (SD_FAILURE);
    }
  else if (max_i == min_i)
    {
    /* Skip db calculation & return "min" (same as "max").	*/
    *pMag = min_i;	/* never changes	*/
    return (SD_SUCCESS);
    }
  else if (db == 0)	/* (max > min) (normal) but (db == 0) */
    {
    /* Skip db calculation & return instantaneous value (within range).	*/
    if (instMag_i > max_i)
      mag = max_i;
    else if (instMag_i < min_i)
      mag = min_i;
    else
      mag = instMag_i;	/* instantaneous value.	*/
    *pMag = mag;
    return (SD_SUCCESS);
    }
  /* If we get to here, all is normal. Do normal calculations.	*/

  /* Get current "mag" value.		*/
  mag = *pMag;	/* get current value	*/
  
  /* Do deadband calculations.	*/
  if (mag < min_i)
    mag = min_i;	/* make sure mag always starts within range	*/
  else if (mag > max_i)
    mag = max_i;	/* make sure mag always starts within range	*/

  if (instMag_i > max_i)
    mag = max_i;
  else if (instMag_i < min_i)
    mag = min_i;
  else if (db > 100000)	/* db > 100% not allowed	*/
    mag = instMag_i;
  else
    {	/* do deadband calculation	*/
    diff_i = instMag_i - mag;
    step_size = (max_i-min_i)*0.00001*db;	/* db increment=0.001%=0.00001	*/
    /* CRITICAL: min, mag, db chks above should assure (step_size!=0)	*/
    num_steps = (int)(diff_i/step_size);
    if (num_steps)
      mag += (ST_INT32)(num_steps * step_size);	/* adjust deadbanded val*/
    }

  *pMag = mag;	/* modify caller's value	*/

  return (SD_SUCCESS);
  }

/************************************************************************/
/*			deadband_float_calculation			*/
/************************************************************************/
ST_RET deadband_float_calculation (ST_FLOAT *pMag, DEADBAND_FLOAT *deadband)
  {
/* Local variables for faster processing.	*/
ST_FLOAT instMag_f;
ST_FLOAT mag;		/* old/new value	*/
ST_FLOAT min_f, max_f, diff_f;
ST_UINT32 db;
double step_size;
int num_steps;

  /* Copy data to local variables.	*/
  instMag_f = *deadband->instMag_f;
  db        = *deadband->db;
  min_f     = *deadband->min_f;
  max_f     = *deadband->max_f;

  if (max_f < min_f)
    {
    /* Invalid range. Can't do deadband calculation.	*/
    MVL_LOG_ERR0 ("max$f < min$f so can't compute deadband");
    return (SD_FAILURE);
    }
  else if (max_f == min_f)
    {
    /* Skip db calculation & return "min" (same as "max").	*/
    *pMag = min_f;	/* never changes	*/
    return (SD_SUCCESS);
    }
  else if (db == 0)	/* (max > min) (normal) but (db == 0) */
    {
    /* Skip db calculation & return instantaneous value (within range).	*/
    if (instMag_f > max_f)
      mag = max_f;
    else if (instMag_f < min_f)
      mag = min_f;
    else
      mag = instMag_f;	/* instantaneous value.	*/
    *pMag = mag;
    }
  /* If we get to here, all is normal. Do normal calculations.	*/

  /* Get current "mag" value.		*/
  mag = *pMag;	/* get current value	*/
  
  /* Do deadband calculations.	*/
  if (mag < min_f)
    mag = min_f;	/* make sure mag always starts within range	*/
  else if (mag > max_f)
    mag = max_f;	/* make sure mag always starts within range	*/

  if (instMag_f > max_f)
    mag = max_f;
  else if (instMag_f < min_f)
    mag = min_f;
  else if (db > 100000)	/* db > 100% not allowed	*/
    mag = instMag_f;
  else
    {	/* do deadband calculation	*/
    diff_f = instMag_f - mag;
    step_size = (max_f-min_f)*0.00001*db;	/* db increment=0.001%=0.00001	*/
    /* CRITICAL: min, mag, db chks above should assure (step_size!=0)	*/
    num_steps = (int)(diff_f/step_size);
    if (num_steps)
      mag += (ST_FLOAT)(num_steps * step_size);	/* adjust deadbanded val*/
    }

  *pMag = mag;	/* modify caller's value	*/

  return (SD_SUCCESS);
  }

