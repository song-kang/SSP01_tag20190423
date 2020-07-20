/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER	*****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*		       1997 All Rights Reserved				*/
/*									*/
/* MODULE NAME : ml_delee.c						*/
/* PRODUCT(S)  : MMS-EASE						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION	LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 01/20/10  JRB     04    Fix log messages.				*/
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
/* DELETE EVENT ENROLLMENT 						*/
/************************************************************************/


ST_VOID m_log_delee_req (DELEE_REQ_INFO *info)
  {
OBJECT_NAME      *obj_ptr;
ST_INT j;

  MLOG_CALWAYS0 ("Scope of Delete : ");
  switch (info->req_tag)
    {
    case 0 :
      MLOG_CALWAYS0 ("specific");
      obj_ptr = (OBJECT_NAME *) (info + 1);
      MLOG_CALWAYS1 (" Object Names    : %d",info->sod.num_of_names);
      for (j = 0; j < info->sod.num_of_names; j++)
        {
        m_log_objname (obj_ptr);
        obj_ptr++;
        }
      break;
    case 1 :
      MLOG_CALWAYS0 ("Event Condition");
      m_log_objname (&info->sod.evcon_name);
    break;
    case 2 :
      MLOG_CALWAYS0 ("Event Action");
      m_log_objname (&info->sod.evact_name);
    break;
    default:
      MLOG_CALWAYS1 ("%d, invalid scope", info->req_tag);
    break;
    }
  }

/************************************************************************/
/* m_log_delee_resp                                                  */
/************************************************************************/


ST_VOID m_log_delee_resp (DELEE_RESP_INFO *info)
  {
  MLOG_CALWAYS1 ("Canditates Not Deleted : %lu",(ST_ULONG)info->cand_not_deleted);
  }



