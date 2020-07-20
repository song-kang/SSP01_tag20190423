/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER	*****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*		       1997 All Rights Reserved				*/
/*									*/
/* MODULE NAME : ml_initd.c						*/
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
/* m_log_initdown_req                                                   */
/************************************************************************/


ST_VOID m_log_initdown_req (INITDOWN_REQ_INFO *info)
  {
ST_CHAR    **cap_ptr;
ST_INT i;

  cap_ptr = (ST_CHAR **) (info+1);
  MLOG_CALWAYS1 ("Domain Name : %s.", info->dname);
  MLOG_CALWAYS1 ("Number of Capabilities  : %d.", info->num_of_capab);
  for (i = 0; i < info->num_of_capab; i++, cap_ptr++)
    {
    MLOG_CALWAYS2 ("%d) %s ", i+1, *cap_ptr);
    }
  MLOG_CALWAYS1 ("Sharable : %d",info->sharable&0xff);
  }

