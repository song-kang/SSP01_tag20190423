/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER	*****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*		       1997 All Rights Reserved				*/
/*									*/
/* MODULE NAME : ml_ustat.c						*/
/* PRODUCT(S)  : MMS-EASE						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION	LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 03/11/04  GLB     04    Remove "thisFileName"                    	*/
/* 10/14/03  JRB     03    Use BSTR_NUMBITS..				*/
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
/* m_log_ustatus_req                                                    */
/************************************************************************/


ST_VOID m_log_ustatus_req (USTATUS_REQ_INFO *info)
  {
  MLOG_CALWAYS1 ("Logical status : %d",info->logical_stat);
  MLOG_CALWAYS1 ("Physical status : %d",info->physical_stat);
  if (info->local_detail_pres)
    {
    MLOG_CALWAYS1 ("%d bits of local detail present (in HEX) : ",
  			                info->local_detail_len);
    MLOG_ALWAYSH (BSTR_NUMBITS_TO_NUMBYTES(info->local_detail_len), info->local_detail);
    }
  else
    {
    MLOG_CALWAYS0 ("No local detail present.");
    }
  }




