/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER	*****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*		       1997 All Rights Reserved				*/
/*									*/
/* MODULE NAME : ml_jinit.c						*/
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
/* m_log_jinit_req                                                      */
/************************************************************************/


ST_VOID m_log_jinit_req (JINIT_REQ_INFO *info)
  {
  MLOG_CALWAYS0 ("Journal Name : ");
  m_log_objname (&(info->jou_name));

  if ( info->limit_spec_pres )
    {
    MLOG_CALWAYS0 ("Limiting Specification (Time) Follows:");
    m_log_evt_tod (&(info->limit_time));

    if (info->limit_entry_pres) 
      {
      MLOG_CALWAYS0 ("Limiting Entry Follows in HEX:");
      MLOG_ALWAYSH (info->limit_entry_len, info->limit_entry);
      }
    else
      {
      MLOG_CALWAYS0 ("Limiting Entry : Not Present");
      }
    }
  else
    {
    MLOG_CALWAYS0 ("Limiting Specification : Not Present");
    }
  }


/************************************************************************/
/* m_log_jinit_resp                                                  */
/************************************************************************/


ST_VOID m_log_jinit_resp (JINIT_RESP_INFO *info)
  {
   MLOG_CALWAYS1 ("Deleted Entries : %lu", (ST_ULONG)info->del_entries);
  }



