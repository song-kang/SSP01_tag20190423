/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER	*****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*		       1997 All Rights Reserved				*/
/*									*/
/* MODULE NAME : ml_write.c						*/
/* PRODUCT(S)  : MMS-EASE						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION	LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 03/11/04  GLB     06    Remove "thisFileName"                    	*/
/* 09/13/99  MDE     05    Added SD_CONST modifiers			*/
/* 06/15/98  MDE     04    Changes to allow compile under C++		*/
/* 01/30/98  EJV     03    Casted param to m_log_data			*/
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
/* m_log_write_req                                                      */
/************************************************************************/


ST_VOID m_log_write_req (WRITE_REQ_INFO *info)
  {
VAR_ACC_DATA *data_ptr;
ST_INT j;

  m_log_vaspec (&info->va_spec);
  data_ptr = info->va_data;
  MLOG_CALWAYS1 ("Number of Data to Write : %d ", info->num_of_data);
  for (j = 0; j < info->num_of_data; j++)
    {
    MLOG_CALWAYS1 ("Write Data %d) ", j+1);
    m_log_data (data_ptr->len, data_ptr->data);

    data_ptr++;
    }
  }


/************************************************************************/
/* m_log_write_resp                                                  	*/
/************************************************************************/


ST_VOID m_log_write_resp (WRITE_RESP_INFO *info)
  {
WRITE_RESULT *temp;              /* index into array of structs  */
ST_INT i;

  MLOG_CALWAYS1 ("Number of Results = %d", info->num_of_result);
  temp = ( WRITE_RESULT *) ( info + 1 );
  for ( i = 0; i < info->num_of_result; i++ )
    {
    if (temp->resp_tag == WR_RSLT_FAILURE)
      {
      MLOG_CALWAYS2 ("Response %3d) : Failure, code = %d.", i+1, temp->failure);
      }
    else
      {
      MLOG_CALWAYS1 ("Response %3d) : Success", i+1);
      }
    temp++;
    }
 }



