/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER	*****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*		       1997 All Rights Reserved				*/
/*									*/
/* MODULE NAME : ml_jwrit.c						*/
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
/* m_log_jwrite_req                                                     */
/************************************************************************/


ST_VOID m_log_jwrite_req (JWRITE_REQ_INFO *info)
  {
ENTRY_CONTENT *ec_info;
VAR_INFO *vi_info;
ST_INT i;
ST_INT j;

    i = 0;
    j = 0;
    ec_info = (ENTRY_CONTENT *) (info + 1);

    MLOG_CALWAYS0 ("Journal Name Follows:");
    m_log_objname (&(info->jou_name));

    MLOG_CALWAYS1 ("Number of Journal Entries : %d ", info->num_of_jou_entry);

    for (i=0; i < info->num_of_jou_entry; i++)
      {
      MLOG_CALWAYS0 ("Occurrence Time Follows : ");
      m_log_evt_tod ( &ec_info->occur_time );

      if ( ec_info->addl_detail_pres )
        {
        MLOG_CALWAYS0 ("Additional Detail Follows in HEX:");
        MLOG_ALWAYSH (ec_info->addl_detail_len, ec_info->addl_detail);
        }
      else
        {
        MLOG_CALWAYS0 ("Additional Detail : Not Present");
        }

      switch ( ec_info->entry_form_tag )
        {
        case  2 :
          MLOG_CALWAYS1 ("Entry form tag : %d, Data.",
                                                ec_info->entry_form_tag);
          break;
        case  3 :
          MLOG_CALWAYS1 ("Entry form tag : %d, Annotation.", 
                                                ec_info->entry_form_tag);
          break;
        }

      if (ec_info->entry_form_tag == 3)          /* form = annotation    */
        {
        MLOG_CALWAYS1 ("Form == Annotation : %s", ec_info->ef.annotation);
        }
      else                                      /* form == data         */
        {
        MLOG_CALWAYS0 ("Form == Data");
        if ( ec_info->ef.data.event_pres )
          {
          MLOG_CALWAYS0 ("Event Condition Name Follows:");
          m_log_objname (&(ec_info->ef.data.evcon_name));

          switch ( ec_info->ef.data.cur_state )
            {
            case  0 :
              MLOG_CALWAYS1 ("Current State : %d, Disabled.",
                                                ec_info->ef.data.cur_state);
              break;
            case  1 :
              MLOG_CALWAYS1 ("Current State : %d, Idle.",
                                                ec_info->ef.data.cur_state);
              break;
            case  2 :
              MLOG_CALWAYS1 ("Current State : %d, Active.",
                                                ec_info->ef.data.cur_state);
              break;
            }
          }
        else
          {
          MLOG_CALWAYS0 ("Event Condition Name : Not Present");
          }

        if ( ec_info->ef.data.list_of_var_pres )
          {
          vi_info = (VAR_INFO *) (ec_info + 1);
          
          MLOG_CALWAYS1 ("Number of Variables : %d", ec_info->ef.data.num_of_var);

          for (j=0; j < ec_info->ef.data.num_of_var ; j++)
            {
            MLOG_CALWAYS2 ("Variable Tag %d)  %s", j+1, vi_info->var_tag);
            MLOG_CALWAYS0 ("Value Specification : ");
            MLOG_ALWAYSH (vi_info->value_spec.len, vi_info->value_spec.data);
            }
          }
        else
          {
          MLOG_CALWAYS0 ("List of Variables : Not Present");
          }
        }

      vi_info = (VAR_INFO *) (ec_info + 1);
      vi_info += ec_info->ef.data.num_of_var;
      ec_info  = (ENTRY_CONTENT *) vi_info;
      }
  }



