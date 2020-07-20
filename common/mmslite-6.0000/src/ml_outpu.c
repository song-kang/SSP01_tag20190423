/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER	*****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*		       1997 All Rights Reserved				*/
/*									*/
/* MODULE NAME : ml_outpu.c						*/
/* PRODUCT(S)  : MMS-EASE						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION	LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 03/11/04  GLB     03    Remove "thisFileName"                    	*/
/* 09/13/99  MDE     02    Added SD_CONST modifiers			*/
/* 06/09/97  MDE     01    Created from existing MLOG source		*/
/************************************************************************/


#include "glbtypes.h"
#include "sysincs.h"

#ifndef MMS_LITE
#include "mms_usr.h"		/* to access MMS fucntions, variables   */
#else
#include "mmsdefs.h"
#endif
#include "mloguser.h"

/************************************************************************/
/************************************************************************/
/* m_log_output_req                                                     */
/************************************************************************/


ST_VOID m_log_output_req (OUTPUT_REQ_INFO *info)
  {
ST_CHAR **od_info;
ST_INT i;

    od_info = (ST_CHAR **) (info + 1);
    MLOG_CALWAYS1 ("Output Station Name : %s",info->station_name);
    MLOG_CALWAYS1 ("Number of Output Data Strings : %d ", info->data_count);
    for (i = 0; i < info->data_count; i++, od_info++)
      {
      MLOG_CALWAYS2 ("Output Data %3d : %s.", i+1, *od_info);
      }
  }



