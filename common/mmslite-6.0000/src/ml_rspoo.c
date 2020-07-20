/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER	*****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*		       1997 All Rights Reserved				*/
/*									*/
/* MODULE NAME : ml_rspoo.c						*/
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
/* REPORT POOL SEMAPHORE STATUS                                         */
/************************************************************************/


ST_VOID m_log_rspool_req (RSPOOL_REQ_INFO *info)
  {

  m_log_objname ( &(info->sem_name) );
  if (info->start_after_pres)
    {
    MLOG_CALWAYS1 ("Application to Start After : %s",info->start_after);
    }
  else
    {
    MLOG_CALWAYS0 ("Application to Start After : Not Present");
    }

  }


/************************************************************************/
/* m_log_rspool_resp                                                 */
/************************************************************************/


ST_VOID m_log_rspool_resp (RSPOOL_RESP_INFO *info)
  {
TOKEN_ID *token_ptr;
ST_INT i;

  token_ptr = (TOKEN_ID *) (info + 1);
  MLOG_CALWAYS1 ("Number of Named Tokens Returned : %d",info->num_of_tokens);
  for (i = 0; i < info->num_of_tokens; i++, token_ptr++)
    {
    switch (token_ptr->token_tag)
      {
      case (0)   :
        MLOG_CALWAYS1 ("Free Token  : %s.",token_ptr->named_token);
      break;

      case (1)   :
        MLOG_CALWAYS1 ("Owned Token : %s.",token_ptr->named_token);
      break;

      case (2)   :
        MLOG_CALWAYS1 ("Hung Token  : %s.",token_ptr->named_token);
      break;
      }
    }
  if (info->more_follows)
    {
    MLOG_CALWAYS0 ("More Follows.");
    }
  else
    {
    MLOG_CALWAYS0 ("No More Follows.");
    }
  }




