/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER	*****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*		       1997 All Rights Reserved				*/
/*									*/
/* MODULE NAME : ml_renam.c						*/
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
/* 12/01/97  DSF     02    Use textual descriptions for object class	*/
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
/* m_log_rename_req                                                     */
/************************************************************************/


ST_VOID m_log_rename_req (RENAME_REQ_INFO *info)
  {
  if (!info->cs_objclass_pres)
    {
    switch (info->obj.mms_class)
      {
      case MMS_CLASS_VAR:
        MLOG_CALWAYS0 ("MMS Object class : namedVariable");
      break;
      case MMS_CLASS_SCAT_ACC:
        MLOG_CALWAYS0 ("MMS Object class : scatteredAccess");
      break;
      case MMS_CLASS_VARLIST:
        MLOG_CALWAYS0 ("MMS Object class : namedVariableList");
      break;
      case MMS_CLASS_TYPE:
        MLOG_CALWAYS0 ("MMS Object class : namedType");
      break;
      case MMS_CLASS_SEM:
        MLOG_CALWAYS0 ("MMS Object class : semaphore");
      break;
      case MMS_CLASS_EV_COND:
        MLOG_CALWAYS0 ("MMS Object class : eventCondition");
      break;
      case MMS_CLASS_EV_ACT:
        MLOG_CALWAYS0 ("MMS Object class : eventAction");
      break;
      case MMS_CLASS_EV_ENROLL:
        MLOG_CALWAYS0 ("MMS Object class : eventEnrollment");
      break;
      case MMS_CLASS_JOU:
        MLOG_CALWAYS0 ("MMS Object class : journal");
      break;
      case MMS_CLASS_DOM:
        MLOG_CALWAYS0 ("MMS Object class : domain");
      break;
      case MMS_CLASS_PI:
        MLOG_CALWAYS0 ("MMS Object class : programInvocation");
      break;
      case MMS_CLASS_OPER_STA:
        MLOG_CALWAYS0 ("MMS Object class : operationStation");
      break;
      default:
        MLOG_CALWAYS0 ("MMS Object class : <UNKNOWN>");
      break;
      }
    }
  else
    {
    MLOG_CALWAYS0 ("CS Object Class ASN.1 :");
    MLOG_ALWAYSH (info->obj.cs.len,info->obj.cs.cs_class);
    }

  MLOG_CALWAYS1 ("Current Name     : %s", info->cur_name.obj_name.vmd_spec);
  m_log_objname (&info->cur_name);

  MLOG_CALWAYS1 ("New Identifier   : %s", info->new_ident);
  }



