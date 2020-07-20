/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER	*****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*		       1997 - 2006 All Rights Reserved			*/
/*									*/
/* MODULE NAME : ml_fdir.c						*/
/* PRODUCT(S)  : MMS-EASE						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION	LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 01/20/10  JRB     08    Fix log messages.				*/
/* 03/28/06  EJV     07    m_log_fdir_resp: corr copy to last_name.	*/
/* 03/03/06  EJV     06    Use strncpy_safe. Added str_util.h.		*/
/* 03/11/04  GLB     05    Remove "thisFileName"                    	*/
/* 10/14/03  JRB     04    Del unnecessary casts.			*/
/* 09/13/99  MDE     03    Added SD_CONST modifiers			*/
/* 10/29/98  EJV     02    Extended last_name buffer in m_log_fdir_resp	*/
/* 06/09/97  MDE     01    Created from existing MLOG source		*/
/************************************************************************/


#include "glbtypes.h"
#include "sysincs.h"
#include "str_util.h"

#ifndef MMS_LITE
#include "mms_usr.h"		/* to access MMS fucntions, variables   */
#else
#include "mmsdefs.h"
#endif
#include "mloguser.h"

/************************************************************************/
/************************************************************************/
/* m_log_fdir_req                                                       */
/************************************************************************/


ST_VOID m_log_fdir_req (FDIR_REQ_INFO *info)
  {
FILE_NAME *fname_idx;
ST_INT i;

  MLOG_CALWAYS0 ("FileName Specification : ");
  if (info->filespec_pres)
    {
    fname_idx = ( FILE_NAME * )( info + 1 );
    MLOG_CALWAYS1 ("Number FileName Specifications : %d.",
                                        info->num_of_fs_fname);
    for (i = 0; i < info->num_of_fs_fname; i++, fname_idx++)
      {
      MLOG_CALWAYS1 ("%d).", i+1 );
      MLOG_ALWAYSH (fname_idx->fn_len, fname_idx->fname);
      }
    }
  else
    {
    MLOG_CALWAYS0 ("Not Present");
    }
  if (info->cont_after_pres)
    {
    MLOG_CALWAYS0 ("FileName to continue after : ");
    MLOG_CALWAYS1 ("Number of FileNames to continue after : %d.", 
                                              info->num_of_ca_fname);
    fname_idx = ( FILE_NAME * )( info + 1 );
    fname_idx += info -> num_of_fs_fname;
    for (i = 0; i < info->num_of_ca_fname; i++, fname_idx++)
      {
      MLOG_CALWAYS1 ("%d).", i+1 );
      MLOG_ALWAYSH (fname_idx->fn_len, fname_idx->fname);
      }
    }
  else
    {
    MLOG_CALWAYS0 ("FileName to continue after : Not Present.");
    }
  }


/************************************************************************/
/* m_log_fdir_resp                                                   */
/************************************************************************/


ST_VOID m_log_fdir_resp (FDIR_RESP_INFO *info)
  {
ST_CHAR last_name[MAX_FILE_NAME+1];
FDIR_DIR_ENT *dir_ptr;
FILE_NAME *fname_idx;
ST_INT i,j;
ST_INT fname_count;

  fname_count = 0;
  last_name[0] = '\x00';
  dir_ptr = (FDIR_DIR_ENT *) (info + 1);
  MLOG_CALWAYS1 ("Number of Directory Entries Returned : %d",info->num_dir_ent);
  for (i = 0; i < info->num_dir_ent; i++ )
    {
    MLOG_CALWAYS1 ("Directory Entry : %d ", i+1);
    MLOG_CALWAYS0 ("File Name : ");
    fname_idx = ( FILE_NAME * )( dir_ptr + 1 );
    fname_count = 0;
    for ( j=0; j < dir_ptr -> num_of_fname; j++, fname_idx++ )
      {
      memcpy (last_name, fname_idx->fname, min(fname_idx->fn_len , sizeof(last_name)-1));
      last_name[min(fname_idx->fn_len, (sizeof(last_name)-1))] = '\0';
      fname_count++;
      MLOG_CALWAYS0 (last_name);
      }
    MLOG_CALWAYS1 ("File Size : %lu ", (ST_ULONG)dir_ptr->fsize);
    if (dir_ptr->mtimpres)
      {
      MLOG_CALWAYS1 ("Time Modified : %s ",ctime (&dir_ptr->mtime));
      }
    else
      {
      MLOG_CALWAYS0 ("Time Modified : Not Present.");
      }

    dir_ptr = ( FDIR_DIR_ENT * ) 
              ( ( (FILE_NAME *)(dir_ptr + 1) ) + fname_count );
    }
  if (info->more_follows)
    {
    MLOG_CALWAYS1 ("More Follows After :%s ", last_name );
    }
  else
    {
    MLOG_CALWAYS0 ("No More Follows ");
    }

  }









