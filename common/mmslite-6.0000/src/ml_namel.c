/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER	*****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*		       1997 All Rights Reserved				*/
/*									*/
/* MODULE NAME : ml_namel.c						*/
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
/* m_log_namelist_req                                                   */
/************************************************************************/


ST_VOID m_log_namelist_req (NAMELIST_REQ_INFO *info)
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
    MLOG_CALWAYS0 ("CS Object Class ASN.1 : ");
    MLOG_ALWAYSH (info->obj.cs.len,info->obj.cs.cs_class);
    }

  switch (info->objscope)
    {
    case VMD_SPEC:
      MLOG_CALWAYS0 ("Object scope : vmdSpecific");
    break;
    case DOM_SPEC:
      MLOG_CALWAYS0 ("Object scope : domainSpecific");
      MLOG_CALWAYS1 ("Domain name  : %s",info->dname);
    break;
    case AA_SPEC:
      MLOG_CALWAYS0 ("Object scope : aaSpecific");
    break;
    default:
      MLOG_CALWAYS0 ("Object scope : <UNKNOWN>");
    break;
    }

  if (info->cont_after_pres)
    {
    MLOG_CALWAYS1 ("Name to continue after : %s",info->continue_after);
    }
  else
    {
    MLOG_CALWAYS0 ("No name to continue after.");
    }
  }


/************************************************************************/
/* m_log_namelist_resp                                                  */
/************************************************************************/


ST_VOID m_log_namelist_resp (NAMELIST_RESP_INFO *info)
  {
ST_CHAR **nptr;
ST_INT j;

  if (info->more_follows)
    {
    MLOG_CALWAYS0 ("More Follows : SD_TRUE");
    }
  else
    {
    MLOG_CALWAYS0 ("More Follows : SD_FALSE");
    }

  MLOG_CALWAYS1 ("%d Names returned : ",info->num_names);
  MLOG_CALWAYS0 ("Name List Follows:");

  nptr = (ST_CHAR **) (info + 1);

  for (j = 0; j < info->num_names; ++j)
    {
    MLOG_CALWAYS1 ("%s",nptr[j]);
    }
  }





