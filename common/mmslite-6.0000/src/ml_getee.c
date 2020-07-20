/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER	*****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*		       1997 All Rights Reserved				*/
/*									*/
/* MODULE NAME : ml_getee.c						*/
/* PRODUCT(S)  : MMS-EASE						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION	LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 01/20/10  JRB     05    Fix log messages.				*/
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
/* m_log_geteea_req                                                     */
/************************************************************************/


ST_VOID m_log_geteea_req (GETEEA_REQ_INFO *info)
  {
OBJECT_NAME *obj_ptr;
ST_INT j;

  switch ( info->scope_of_req )
    {
    case 0 :
      MLOG_CALWAYS1 ("Scope of Request : %d, Specific.",
                                                info->scope_of_req);
      break;
    case 1 :
      MLOG_CALWAYS1 ("Scope of Request : %d, Client.",
                                                info->scope_of_req);
      break;
    case 2 :
      MLOG_CALWAYS1 ("Scope of Request : %d, Event Condition.",
                                                info->scope_of_req);
      break;
    case 3 :
      MLOG_CALWAYS1 ("Scope of Request : %d, Event Action.",
                                                info->scope_of_req);
      break;
    default:
      MLOG_CALWAYS1 ("Scope of Request : %d, Invalid Scope.",
                                                info->scope_of_req);
      break;
    }

  if (info->client_app_pres)
    {
    MLOG_CALWAYS0 ("Client Application Follows:");
    m_log_asn1_app_ref ( info->client_app, info->client_app_len );
    }
  else
    {
    MLOG_CALWAYS0 ("Client Application : Not Present");
    }

  if ( info->evcon_name_pres )
    {
    MLOG_CALWAYS0 ("Event Condition Name Follows:");
    m_log_objname ( &(info->evcon_name) );
    }
  else
    {
    MLOG_CALWAYS0 ("Event Condition Name : Not Present");
    }

  if ( info->evact_name_pres )
    {
    MLOG_CALWAYS0 ("Event Action Name Follows:");
    m_log_objname ( &(info->evact_name) );
    }
  else
    {
    MLOG_CALWAYS0 ("Event Action Name : Not present");
    }

  if ( info->ca_name_pres )
    {
    MLOG_CALWAYS0 ("Continue After Name :");
    m_log_objname ( &(info->ca_name) );
    }
  else
    {
    MLOG_CALWAYS0 ("Continue After Name : Not Present");
    }

  if ( info->eenames_pres )
    {
    MLOG_CALWAYS1 ("Event Enrollment Names : %d", info->num_of_eenames);
    obj_ptr = (OBJECT_NAME *) (info + 1);
    for (j = 0; j < info->num_of_eenames; j++)
      {
      m_log_objname (obj_ptr);
      obj_ptr++;
      }
    }
  else
    {
    MLOG_CALWAYS0 ("Event Enrollment Names : Not present");
    }
  }



/************************************************************************/
/* m_log_geteea_resp                                                 */
/************************************************************************/


ST_VOID m_log_geteea_resp (GETEEA_RESP_INFO *info)
  {
EVENT_ENROLLMENT *ee_ptr;
ST_INT i;

  ee_ptr = (EVENT_ENROLLMENT *) (info + 1);
  MLOG_CALWAYS1 ("Event Enrollments : %d",info->num_of_evenroll);

  if (info->more_follows)
    {
    MLOG_CALWAYS0 ("More Follows : Yes");
    }
  else
    {
    MLOG_CALWAYS0 ("More Follows : No");
    }

  for (i = 0; i < info->num_of_evenroll; i++, ee_ptr++)
    {
    MLOG_CALWAYS0 ("Event Enrollment Name Follows:");
    m_log_objname (&ee_ptr->evenroll_name);

    if ( ee_ptr->evcon_name_tag == 0 )
      {
      MLOG_CALWAYS0 ("Event Condition Name Follows:");
      m_log_objname (&ee_ptr->evcon_name);
      }
    else if ( ee_ptr->evcon_name_tag == 1 )
      {
      MLOG_CALWAYS0 ("Event Condition Name Tag : Undefined.");
      }
    else
      {
      MLOG_CALWAYS0 ("Event Condition Name Tag : Invalid.");
      }

    if ( ee_ptr->evact_name_pres )
      {
      if (ee_ptr->evact_name_tag == 0)
        {
        MLOG_CALWAYS0 ("Event Action Name Follows:");
        m_log_objname (&ee_ptr->evact_name);
        }
      else if ( ee_ptr->evact_name_tag == 1 )
        {
        MLOG_CALWAYS0 ("Event Action Name Tag: Undefined.");
        }
      else
        {
        MLOG_CALWAYS1 ("Event Action Name Tag : %d, Invalid.",
                                                ee_ptr->evact_name_tag);
        }
      }
    else
      {
      MLOG_CALWAYS0 ("Event Action Name : Not Present");
      }

    if (ee_ptr->client_app_pres)
      {
      MLOG_CALWAYS0 ("Client Application Follows:");
      m_log_asn1_app_ref ( ee_ptr->client_app, ee_ptr->client_app_len );
      }
    else
      {
      MLOG_CALWAYS0 ("Client Application : Not Present");
      }

    if (ee_ptr->mms_deletable)
      {
      MLOG_CALWAYS0 ("MMS Deletable : Yes");
      }
    else
      {
      MLOG_CALWAYS0 ("MMS Deletable : No");
      }

    switch (ee_ptr->ee_class)
      {
      case  0 :
        MLOG_CALWAYS1 ("Event Enrollment Class: %d, Modifier.",
                                                        ee_ptr->ee_class);
        break;
      case  1 :
        MLOG_CALWAYS1 ("Event Enrollment Class: %d, Notification.",
                                                        ee_ptr->ee_class);
        break;
      default :
        MLOG_CALWAYS1 ("Event Enrollment Class: %d, Invalid Class.",
                                                        ee_ptr->ee_class);
        break;
      }

    switch (ee_ptr->duration)
      {
      case  0 :
        MLOG_CALWAYS1 ("Duration : %d, Current.", ee_ptr->duration);
        break;
      case  1 :
        MLOG_CALWAYS1 ("Duration : %d, Permanent.", ee_ptr->duration);
        break;
      default :
        MLOG_CALWAYS1 ("Duration : %d, Invalid Duration.", ee_ptr->duration);
        break;
      }

    if (ee_ptr->invoke_id_pres)
      {
      MLOG_CALWAYS1 ("Invoke Id : %lu", (ST_ULONG)ee_ptr->invoke_id);
      }
    else
      {
      MLOG_CALWAYS0 ("Invoke Id : Not present");
      }

    if (ee_ptr->rem_acc_delay_pres)
      {
      MLOG_CALWAYS1 ("Rem Acceptable Delay : %lu", (ST_ULONG)ee_ptr->rem_acc_delay);
      }
    else
      {
      MLOG_CALWAYS0 ("Rem Acceptable Delay : Not Present");
      }

    if (ee_ptr->addl_detail_pres)
      {
      MLOG_CALWAYS0 ("Additional Detail Follows in HEX:");
      MLOG_ALWAYSH (ee_ptr->addl_detail_len, ee_ptr->addl_detail);
      }
    else
      {
      MLOG_CALWAYS0 ("Additional Detail : Not Present");
      }

    if ( ee_ptr->ackec_name_pres )
      {
      if (ee_ptr->ackec_name_tag == 0)
        {
        MLOG_CALWAYS0 ("Ack Event Cond Name Follows:");
        m_log_objname (&ee_ptr->ackec_name);
        }
      else if ( ee_ptr->ackec_name_tag == 1 )
        {
        MLOG_CALWAYS0 ("Ack Event Cond Tag : Undefined.");
        }
      else
        {
        MLOG_CALWAYS0 ("Ack Event Cond Tag : Invalid.");
        }
      }
    else
      {
      MLOG_CALWAYS0 ("Ack Event Cond Name : Not Present.");
      }
    }
  }



