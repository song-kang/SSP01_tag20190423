/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER	*****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*		       1997 All Rights Reserved				*/
/*									*/
/* MODULE NAME : ml_frena.c						*/
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
/* m_log_frename_req                                                    */
/************************************************************************/


ST_VOID m_log_frename_req (FRENAME_REQ_INFO *info)
  {
FILE_NAME *fname_idx;
ST_INT i;

  fname_idx = ( FILE_NAME * )( info + 1 );
  MLOG_CALWAYS1 ("Number of Current FileNames : %d.", 
                                     info->num_of_cur_fname);
  for (i = 0; i < info->num_of_cur_fname; i++, fname_idx++)
    {
    MLOG_CALWAYS1 ( "%d) ", i+1 );
    MLOG_ALWAYSH (fname_idx->fn_len, fname_idx->fname);
    }
  MLOG_CALWAYS1 ("Number of New FileNames : %d.",info->num_of_new_fname);
  for (i = 0; i < info->num_of_new_fname; i++, fname_idx++)
    {
    MLOG_CALWAYS1 ("%d).", i+1 );
    MLOG_ALWAYSH (fname_idx->fn_len, fname_idx->fname);
    }

  }


