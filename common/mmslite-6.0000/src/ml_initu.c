/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER	*****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*		       1997 All Rights Reserved				*/
/*									*/
/* MODULE NAME : ml_initu.c						*/
/* PRODUCT(S)  : MMS-EASE						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION	LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 01/20/10  JRB     05    Fix log messages.				*/
/* 03/11/04  GLB     04    Remove "thisFileName"                    	*/
/* 02/28/01  RKR     03    Took the period off the Domain name		*/
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
/* m_log_initupl_req                                                    */
/************************************************************************/


ST_VOID m_log_initupl_req (INITUPL_REQ_INFO *info)
  {
  MLOG_CALWAYS1 ("Domain Name : %s", info->dname);
  }



/************************************************************************/
/* m_log_initupl_resp                                                */
/************************************************************************/


ST_VOID m_log_initupl_resp (INITUPL_RESP_INFO *info)
  {
ST_CHAR	**cap_ptr;
ST_INT i;
  
  cap_ptr = (ST_CHAR **) (info + 1);
  MLOG_CALWAYS1 ("Upload State Machine ID : %ld", (ST_LONG)info->ulsmid);
  MLOG_CALWAYS1 ("Number of Capabilities  : %d",  info->num_of_capab);
  for (i = 0; i < info->num_of_capab; i++, cap_ptr++)
    {
    MLOG_CALWAYS2 ("   %d) %s ", i+1, *cap_ptr);
    }
  }



