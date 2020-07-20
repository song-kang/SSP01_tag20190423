/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER	*****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*		       1997 All Rights Reserved				*/
/*									*/
/* MODULE NAME : ml_delec.c						*/
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
/************************************************************************/
/* m_log_delec_req                                                      */
/************************************************************************/


ST_VOID m_log_delec_req (DELEC_REQ_INFO *info)
  {
OBJECT_NAME *obj_ptr;
ST_INT j;

  j = 0;
  switch (info->req_tag)
    {
    case 0 :
      MLOG_CALWAYS0 ("Scope of Delete : Specific");
      obj_ptr = (OBJECT_NAME *) (info + 1);
      MLOG_CALWAYS1 ("Number of Object Names : %d", info->num_of_names);
      for (j = 0; j < info->num_of_names; j++)
        {
        m_log_objname (obj_ptr);
        obj_ptr++;
        }
      break;
    case 1 :
      MLOG_CALWAYS0 ("Scope of Delete : AA-Specific");
      break;
    case 2 :
      MLOG_CALWAYS0 ("Scope of Delete : Domain");
      MLOG_CALWAYS1 ("Domain Name : %s ", info->dname);
      break;
    case 3 :
      MLOG_CALWAYS0 ("Scope of Delete : VMD");
      break;
    default:
      MLOG_CALWAYS0 ("Scope of Delete : Invalid");
      break;
    }
  }




/************************************************************************/
/* m_log_delec_resp                                                  */
/************************************************************************/


ST_VOID m_log_delec_resp (DELEC_RESP_INFO *info)
  {
  MLOG_CALWAYS1 ("Canditates Not Deleted : %lu",(ST_ULONG)info->cand_not_deleted);
  }

