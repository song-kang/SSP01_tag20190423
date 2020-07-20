/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER	*****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*		       1997 All Rights Reserved				*/
/*									*/
/* MODULE NAME : ml_getvl.c						*/
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
/* 09/13/99  MDE     03    Added SD_CONST modifiers			*/
/* 09/29/97  MDE     02    Now log variable specs			*/
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
/* m_log_getvlist_req                                                   */
/************************************************************************/


ST_VOID m_log_getvlist_req (GETVLIST_REQ_INFO *info)
  {
  m_log_objname (&info->vl_name);
  }


/************************************************************************/
/* m_log_getvlist_resp                                               */
/************************************************************************/


ST_VOID m_log_getvlist_resp ( GETVLIST_RESP_INFO *info )
  {
VARIABLE_LIST *vl;

  if (info->mms_deletable)
    {
    MLOG_CALWAYS0 ("MMS Deletable : YES");
    }
  else
    {
    MLOG_CALWAYS0 ("MMS Deletable : NO");
    }

  vl = (VARIABLE_LIST *)(info + 1);
  m_log_var_list (vl, info->num_of_variables);
  }



