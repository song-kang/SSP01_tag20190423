/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER	*****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*		       1997 All Rights Reserved				*/
/*									*/
/* MODULE NAME : ml_getsc.c						*/
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
/* 10/14/03  JRB     03    Del unnecessary casts.			*/
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
/* m_log_getscat_req                                                    */
/************************************************************************/


ST_VOID m_log_getscat_req (GETSCAT_REQ_INFO *info)
  {
  m_log_objname (&info->sa_name);
 }



/************************************************************************/
/* m_log_getscat_resp                                                */
/************************************************************************/


ST_VOID m_log_getscat_resp (GETSCAT_RESP_INFO *info)
  {
  if ( info->mms_deletable )
    {
    MLOG_CALWAYS0 ("MMS Deletable : YES");
    }
  else
    {
    MLOG_CALWAYS0 ("MMS Deletable : NO");
    }
  MLOG_CALWAYS0 ("Scattered Access : ");
  MLOG_ALWAYSH (info->sa_descr.len, info->sa_descr.data);
  }

