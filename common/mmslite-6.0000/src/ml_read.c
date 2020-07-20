/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER	*****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*		       1997 All Rights Reserved				*/
/*									*/
/* MODULE NAME : ml_read.c						*/
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
/* 06/15/98  MDE     03    Changes to allow compile under C++		*/
/* 09/05/97  MDE     02    Now log data in 'real' form			*/
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
/* m_log_read_req                                                       */
/************************************************************************/


ST_VOID m_log_read_req (READ_REQ_INFO *info)
  {
  if ( info->spec_in_result )
    {
    MLOG_CALWAYS0 ("Specification with Result : YES");
    }
  else
    {
    MLOG_CALWAYS0 ("Specification with Result : NO");
    }
  m_log_vaspec (&info->va_spec);
  }


/************************************************************************/
/* m_log_read_resp                                                   */
/************************************************************************/


ST_VOID m_log_read_resp (READ_RESP_INFO *info)
   {
ACCESS_RESULT  *ar_ptr;
ST_INT j;

   if (info->va_spec_pres)
     {
     m_log_vaspec (&info->va_spec);
     }
   else
     {
     MLOG_CALWAYS0 ("Variable Access Specification : Not Present.");
     }

   ar_ptr = info->acc_rslt_list;
   MLOG_CALWAYS1 ("Number of Access Results = %d ",info->num_of_acc_result);

   for (j = 0; j < info->num_of_acc_result; j++)
     {
     if ( ar_ptr->acc_rslt_tag == ACC_RSLT_SUCCESS)
       {
       MLOG_CALWAYS1 ("Result %3d) : Success", j+1);
       m_log_data ((ST_INT)ar_ptr->va_data.len, ar_ptr->va_data.data);
       }
     else
       {
       MLOG_CALWAYS2 ("Result %3d) : Failure : %d.", j+1, ar_ptr->failure);
       }
     ar_ptr++;
     }
  }



