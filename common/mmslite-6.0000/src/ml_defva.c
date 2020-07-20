/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER	*****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*		       1997 All Rights Reserved				*/
/*									*/
/* MODULE NAME : ml_defva.c						*/
/* PRODUCT(S)  : MMS-EASE						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION	LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 03/11/04  GLB     05    Remove "thisFileName"                    	*/
/* 09/13/99  MDE     04    Added SD_CONST modifiers			*/
/* 08/13/98  MDE     03    Added m_log_tdl				*/
/* 06/22/98  MDE     02    Now allow TDL up to 10000 bytes long		*/
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
#include "mem_chk.h"

/************************************************************************/
/************************************************************************/
/* m_log_defvar_req                                                     */
/************************************************************************/


ST_VOID m_log_defvar_req (DEFVAR_REQ_INFO *info)
  {
  m_log_objname (&info->name);
  m_log_address (&info->address);
  if ( info->type_spec_pres )
    m_log_tdl (info->type_spec.data, info->type_spec.len); 
  else
    {
    MLOG_CALWAYS0 ("Type Specification : Not Present ");
    }
  }




