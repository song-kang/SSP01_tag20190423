#ifndef DB_61850_INCLUDED
#define DB_61850_INCLUDED
/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	2008-2008 All Rights Reserved					*/
/*									*/
/* MODULE NAME : db_61850.h						*/
/* PRODUCT(S)  : MMS-EASE-LITE						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*	IEC 61850 deadband structures and functions.			*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev     Comments					*/
/* --------  ---  ------   -------------------------------------------  */
/* 12/17/08  JRB    01     Initial Revision.				*/
/************************************************************************/
#include "mvl_defs.h"	/* need MVL_VAR_ASSOC	*/

#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************/
/* "Deadband" structures.						*/
/************************************************************************/
/* Deadband information structure for ST_INT32 values.	*/
typedef struct
  {
  ST_INT32 *instMag_i;
  ST_INT32 *min_i;
  ST_INT32 *max_i;
  ST_UINT32 *db;
  } DEADBAND_INT32;
/* Deadband information structure for ST_FLOAT values.	*/
typedef struct
  {
  ST_FLOAT *instMag_f;
  ST_FLOAT *min_f;
  ST_FLOAT *max_f;
  ST_UINT32 *db;
  } DEADBAND_FLOAT;

/************************************************************************/
/* "Deadband" functions.						*/
/************************************************************************/
/* Values for last arg to deadband_int32_create, deadband_float_create.	*/
#define DEADBAND_TYPE_MAG	1
#define DEADBAND_TYPE_CVAL_MAG	2
#define DEADBAND_TYPE_CVAL_ANG	3

DEADBAND_INT32 *deadband_int32_create_2 (MVL_VAR_ASSOC *base_var,
	ST_CHAR *leaf,
	ST_INT type);	/* DEADBAND_TYPE_MAG      OR	*/
			/* DEADBAND_TYPE_CVAL_MAG OR	*/
			/* DEADBAND_TYPE_CVAL_ANG	*/
DEADBAND_FLOAT *deadband_float_create_2 (MVL_VAR_ASSOC *base_var,
	ST_CHAR *leaf,
	ST_INT type);	/* DEADBAND_TYPE_MAG      OR	*/
			/* DEADBAND_TYPE_CVAL_MAG OR	*/
			/* DEADBAND_TYPE_CVAL_ANG	*/
DEADBAND_INT32 *deadband_int32_create (MVL_VAR_ASSOC *base_var, ST_CHAR *leaf);
DEADBAND_FLOAT *deadband_float_create (MVL_VAR_ASSOC *base_var, ST_CHAR *leaf);
/************************************************************************/
/*				deadband_create				*/
/* If this "leaf" requires deadbanding, this function allocates a	*/
/* DEADBAND_INT32 or DEADBAND_FLOAT struct and returns a ptr to it,	*/
/* casted to (ST_VOID *). Must cast back before using.			*/
/* Returns NULL if deadbanding not required.				*/
/************************************************************************/
ST_VOID *deadband_create (
	MVL_VAR_ASSOC *base_var,	/* base var (Logical Node)	*/
	ST_CHAR *leaf, 			/* flattened name of this leaf	*/
	RUNTIME_TYPE *rt_type);		/* type of this leaf		*/

ST_RET deadband_int32_calculation (ST_INT32 *pMag, DEADBAND_INT32 *deadband);
ST_RET deadband_float_calculation (ST_FLOAT *pMag, DEADBAND_FLOAT *deadband);

#ifdef __cplusplus
}
#endif

#endif	/* !DB_61850_INCLUDED	*/

