/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER	*****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*		       1997 All Rights Reserved				*/
/*									*/
/* MODULE NAME : ml_ident.c						*/
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
/************************************************************************/
/* m_log_ident_req                                                      */
/************************************************************************/


ST_VOID m_log_ident_req (ST_VOID)
  {
  }


/************************************************************************/
/* m_log_ident_resp                                                     */
/************************************************************************/
 

ST_VOID m_log_ident_resp (IDENT_RESP_INFO *info)
  {
struct mms_obj_id *obj;
ST_INT i,j;

  MLOG_CALWAYS0 ("System Information Follows:" );
  MLOG_CALWAYS1 ("Vendor   : %s", info->vend );
  MLOG_CALWAYS1 ("Model    : %s", info->model );
  MLOG_CALWAYS1 ("Revision : %s", info->rev );
  if ( info->num_as )
    {
    MLOG_CALWAYS0 ("Abstract Syntax's :");
    obj = (struct mms_obj_id *) (info + 1);    /* point to first   */
    for (i = 0; i < info->num_as; ++i, ++obj)
      {
      for ( j = 0; j < obj->num_comps; ++j )
        {
        MLOG_CALWAYS2 ("%02d) %d.",i+1, obj->comps[j]);
        }
      }
    }
  }




