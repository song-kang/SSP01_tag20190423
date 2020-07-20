/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	2003 - 2003, All Rights Reserved.				*/
/*									*/
/*		PROPRIETARY AND CONFIDENTIAL				*/
/*									*/
/* MODULE NAME : ms_tdef4.c						*/
/* PRODUCT(S)  : MMSEASE-LITE						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/* Function called by "ms_runtime_create" to find base types when	*/
/* building complex types.						*/
/* NOTE: This module should be included in a library & should contain	*/
/*	ONLY "u_ml_get_rt_type", so that the user can easily replace	*/
/* 	this function with a customized function.			*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*			u_ml_get_rt_type				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 02/21/07  JRB     02    Just log & return error (removes dependence	*/
/*			   on "mvl" library). If base types needed,	*/
/*			   user app should create customized funct.	*/
/* 04/24/03  JRB     01    New module.					*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "mmsdefs.h"
#include "mms_vvar.h"

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/
/*			u_ml_get_rt_type				*/
/* Called by "ms_runtime_create" to find base types (INT32S, etc.).	*/
/************************************************************************/
ST_RET u_ml_get_rt_type (ASN1_DEC_CTXT *aCtx, OBJECT_NAME *type_name,
			RUNTIME_TYPE **rt_out, ST_INT *num_rt_out)
  {
  /* DEBUG: if this error occurs, create customized function in user	*/
  /*        application (see sample applications).			*/
  MLOG_ERR0 ("u_ml_get_rt_type: Finding base types not supported.");
  return (SD_FAILURE);
  }
