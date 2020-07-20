/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER	*****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*		       1997-2002, All Rights Reserved			*/
/*									*/
/* MODULE NAME : ml_jread.c						*/
/* PRODUCT(S)  : MMS-EASE						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION	LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 01/20/10  JRB     07    Fix log messages.				*/
/* 11/01/04  JRB     06    Add ; on MLOG* calls to work with new macros.*/
/* 03/11/04  GLB     05    Remove "thisFileName"                    	*/
/* 10/14/03  JRB     04    Del unnecessary casts.			*/
/* 02/01/02  EJV     03    Added NL in slog listing Journal Entries.	*/
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
/* m_log_jread_req                                                      */
/************************************************************************/


ST_VOID m_log_jread_req (JREAD_REQ_INFO *info)
  {
ST_CHAR **var_ptr;
ST_INT j;

  MLOG_CALWAYS0 ("Press return to continue");
  MLOG_CALWAYS0 ("Journal Name Follows:");
  m_log_objname ( &(info->jou_name) );

  j = 0;
  if ( info->range_start_pres )
    {
    MLOG_CALWAYS0 ("Range Start Specs Follows:");
    switch ( info->start_tag )
      {
      case  0 :
        MLOG_CALWAYS0 ("Starting Time Follows:");
        m_log_evt_tod ( &info->start_time );
        break;
      case  1 :
        MLOG_CALWAYS0 ("Starting Entry Follows in HEX:");
        MLOG_ALWAYSH (info->start_entry_len, info->start_entry);
        break;
      default :
        MLOG_CALWAYS0 ("Range Start Specs : Invalid");
        break;
      }
    }
  else
    {
    MLOG_CALWAYS0 ("Range Start Specs : Not Present");
    }

  if ( info->range_stop_pres )
    {
    MLOG_CALWAYS0 ("Range Stop Specs Follow:");
    switch ( info->stop_tag )
      {
      case  0 :
        MLOG_CALWAYS0 ("Ending Time");
        m_log_evt_tod ( &info->end_time );
        break;
      case  1 :
        MLOG_CALWAYS1 ("Number of Entries : %ld", (ST_LONG)info->num_of_entries);
        break;
      default :
        MLOG_CALWAYS0 ("Invalid");
        break;
      }
    }
  else
    {
    MLOG_CALWAYS0 ("Range Stop Specs : Not Present");
    }

  if ( info->sa_entry_pres )
    {
    MLOG_CALWAYS0 ("Start After Entry : ");
    m_log_evt_tod (&(info->time_spec));
    MLOG_CALWAYS0 ("Entry Spec Follows in HEX:");
    MLOG_ALWAYSH (info->entry_spec_len, info->entry_spec);
    }
  else
    {
    MLOG_CALWAYS0 ("Start After Entry : Not Present");
    }

  if ( info->list_of_var_pres )
    {
    MLOG_CALWAYS1 ("Number of Variables : %d", info->num_of_var);
    var_ptr = (ST_CHAR **)(info+1);
    for (j=0; j < info->num_of_var ; j++)
      {
      MLOG_CALWAYS2 ("Variable %2d) %s", j+1, *var_ptr);
      var_ptr++;
      }
    }
  else
    {
    MLOG_CALWAYS0 ("List of Variables : Not Present");
    }
  }


/************************************************************************/
/* m_log_jread_resp                                                  */
/************************************************************************/


ST_VOID m_log_jread_resp (JREAD_RESP_INFO *info)
  {
JOURNAL_ENTRY *je_ptr;
ENTRY_CONTENT *ec_ptr;
VAR_INFO *vi_ptr;
ST_INT  i = 0, j = 0;

  je_ptr = (JOURNAL_ENTRY *) (info + 1);
  
  MLOG_CALWAYS1 ("Number of Journal Entry : %d",  info->num_of_jou_entry);

  if ( info->more_follows )
    {
    MLOG_CALWAYS0 ("More Follows : Yes");
    }
  else
    {
    MLOG_CALWAYS0 ("More Follows : No");
    }

  for (i = 0; i < info->num_of_jou_entry; i++)
    {
    MLOG_CALWAYS1 ("\nJournal Entry %3d) ", i+1);
    
    MLOG_CALWAYS0 ("Entry Identifier Follows in HEX:");
    MLOG_ALWAYSH (je_ptr->entry_id_len, je_ptr->entry_id);
    
    MLOG_CALWAYS0 ("Originating App : ");
    m_log_asn1_app_ref (je_ptr->orig_ae, je_ptr->orig_ae_len );

    ec_ptr = &(je_ptr->ent_content);

    MLOG_CALWAYS0 ("Entry Content Follows:");
    MLOG_CALWAYS0 ("Occurrence Time Follows:");
    m_log_evt_tod ( &ec_ptr->occur_time );

    if ( ec_ptr->addl_detail_pres )
      {
      MLOG_CALWAYS0 ("Additional Detail Follows in HEX:");
      MLOG_ALWAYSH (ec_ptr->addl_detail_len, ec_ptr->addl_detail);
      }
    else
      {
      MLOG_CALWAYS0 ("Additional Detail : Not Present");
      }

    switch ( ec_ptr->entry_form_tag )
      {
      case  2 :
        MLOG_CALWAYS1 ("Entry form : %d, Data.", ec_ptr->entry_form_tag);
        break;
      case  3 :
        MLOG_CALWAYS1 ("Entry form : %d, AnNotation.", ec_ptr->entry_form_tag);
        break;
      default :
        MLOG_CALWAYS1 ("Entry form : %d, Invalid.", ec_ptr->entry_form_tag);
        break;
      }

    if (ec_ptr->entry_form_tag == 3)        /* ANNoTATION           */
      {
      MLOG_CALWAYS1 ("Annotation : %s", ec_ptr->ef.annotation);
      }
    else                                    /* DATA                 */
      {
      if ( ec_ptr->ef.data.event_pres )
        {
        MLOG_CALWAYS0 ("Event Condition Name Follows:");
        m_log_objname ( &(ec_ptr->ef.data.evcon_name) );

        switch ( ec_ptr->ef.data.cur_state )
          {
          case  0 :
            MLOG_CALWAYS1 ("Current State : %d, Disabled.",
                                        ec_ptr->ef.data.cur_state);
            break;
          case  1 :
            MLOG_CALWAYS1 ("Current State : %d, Idle.",
                                        ec_ptr->ef.data.cur_state);
            break;
          case  2 :
            MLOG_CALWAYS1 ("Current State : %d, Active.",
                                        ec_ptr->ef.data.cur_state);
            break;
          }
        }
      else
        {
        MLOG_CALWAYS0 ("Event Condition Name : Not Present");
        }

      if ( ec_ptr->ef.data.list_of_var_pres )
        {
        vi_ptr = (VAR_INFO *) (ec_ptr + 1);
        MLOG_CALWAYS1 ("Number of Variables : %d",
                                        ec_ptr->ef.data.num_of_var);

        for (j=0; j < ec_ptr->ef.data.num_of_var ; j++)
          {
          MLOG_CALWAYS2 ("Variable Tag %d) %s",j+1,vi_ptr->var_tag);
          MLOG_CALWAYS0 ("Value Specification Follows in HEX:");
          MLOG_ALWAYSH (vi_ptr->value_spec.len, vi_ptr->value_spec.data);
          }
        }
      else
        {
        MLOG_CALWAYS0 ("List of Variables : Not Present");
        }
      }
    vi_ptr = (VAR_INFO *) (je_ptr + 1);
    vi_ptr += je_ptr->ent_content.ef.data.num_of_var;
    je_ptr = (JOURNAL_ENTRY *) vi_ptr;
    }
  }



