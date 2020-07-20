/***********************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,       */
/*                  1986 - 2006 All Rights Reserved                     */
/*                                                                      */
/* MODULE NAME : mlogmisc.c                                             */
/* PRODUCT(S)  : MMS-EASE                                               */
/*                                                                      */
/* MODULE DESCRIPTION :                                                 */
/*      This module contains user defined logging functions for         */
/*      u_misc.c functions that require logging.                        */
/*                                                                      */
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :                            */
/*                                                                      */
/* MODIFICATION LOG :                                                   */
/*  Date     Who   Rev                  Comments                        */
/* --------  ---  ------   -------------------------------------------  */
/* 01/20/10  JRB     15    Fix log messages.				*/
/* 05/20/08  GLB     14    Created missing error and reject messages    */
/* 03/21/08  JRB     13    Fix crash on unknown error/reject class/code.*/
/* 03/03/06  EJV     12    Use strncpy_safe. Added str_util.h		*/
/* 03/11/04  GLB     11    Remove "thisFileName"                    	*/
/* 10/14/03  JRB     10    Del unnecessary casts.			*/
/* 09/13/99  MDE     09    Added SD_CONST modifiers			*/
/* 11/04/98  DSF     08    Added m_get_mms_rej_text ()			*/
/* 11/04/98  DSF     07    Fixed spelling				*/
/* 10/05/98  EJV     06    Don't need "suicacse.h" (if def MAP30_ACSE)	*/
/* 03/20/98  JRB     05    Don't need mmsop_en.h anymore.		*/
/* 12/09/97  DSF     04    Fixed spelling				*/
/* 08/05/97  MDE     03    Made 'no log' functions non-static		*/
/* 08/04/97  MDE     02    Removed DEBUG_SISCO code, added err, more	*/
/* 06/09/97  MDE     01    Added MMS-LITE ifdefs			*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "str_util.h"

#ifndef MMS_LITE
#include "mms_usr.h"		/* to access MMS fucntions, variables   */
#else
#include "mmsdefs.h"
#endif

#include "mms_pevn.h"		/* to access modifiers			*/
#include "asn1defs.h"           /* to access ASN.1 variables            */
#include "mem_chk.h"
#include "slog.h"

#include "mloguser.h"

/************************************************************************/
/*                      m_log_external                                  */
/* This function is used to log the contents of the extern_info         */
/* structure that has been filled in by a parse routine                 */
/************************************************************************/

ST_VOID m_log_external (EXTERN_INFO *info)
  {
struct mms_obj_id *obj;
ST_INT i;

  if (info->dir_ref_pres)
    {
    obj = &(info->dir_ref);
    for (i = 0; i < obj->num_comps; ++i)
      {
      MLOG_CALWAYS1 ("Direct Reference : %d",obj->comps[i]);
      }
    }
  else
    {
    MLOG_CALWAYS0 ("Direct Reference : Not Present");
    }

  if (info->indir_ref_pres)
    {
    MLOG_CALWAYS1 ("Indirect Reference : %ld",(ST_LONG)info->indir_ref);
    }
  else
    {
    MLOG_CALWAYS0 ("Indirect Reference : Not Present");
    }

  if (info->dv_descr_pres)
    {
    MLOG_CALWAYS0 ("Data Value Descripter :");
    MLOG_ALWAYSH (info->dv_descr_len,info->dv_descr);
    }
  else
    {
    MLOG_CALWAYS0 ("Data Value Descripter : Not Present");
    }

  MLOG_CALWAYS1 ("Encoding Tag : %d",info->encoding_tag);

  if (info->encoding_tag == 2)
    {
    MLOG_CALWAYS1 ("Data Length = %d bits",info->num_bits);
    }
  else
    {
    MLOG_CALWAYS1 ("Data Length = %d octects",info->data_len);
    }

  MLOG_CALWAYS0 ("Data : ");
  MLOG_ALWAYSH (info->data_len,info->data_ptr);
  }


/************************************************************************/
/************************************************************************/
/************************************************************************/
/*			m_log_objname					*/
/* This routine is used here in variable access management and in	*/
/* semaphore management, to print an object name.			*/
/************************************************************************/

ST_VOID m_log_objname (OBJECT_NAME *info)
  {
  switch (info->object_tag)
    {
    case 0  :
      MLOG_CALWAYS1 ("VMD-Specific Name : %s",info->obj_name.vmd_spec);
      break;

    case 1  :
      MLOG_CALWAYS1 ("DOM-Specific Domain Id: %s",info->domain_id);
      MLOG_CALWAYS1 ("Item Id: %s",info->obj_name.item_id);
      break;

    case 2  :
      MLOG_CALWAYS1 ("AA -Specific Name : %s",info->obj_name.aa_spec);
      break;

    default   :
      MLOG_CALWAYS1 ("Invalid Object Tag : %d ",info->object_tag);
      break;
    }
  }

/************************************************************************/
/*			m_log_modifier					*/
/* This routine is used to slog a modifier.	 			*/
/************************************************************************/

ST_VOID m_log_modifier (MODIFIER *info)
  {
  switch (info->modifier_tag)
    {
    case (0)  :
      MLOG_CALWAYS0 ("Attach to Event Condition");

      MLOG_CALWAYS0 ("Event Enrollment Name Follows:");
      m_log_objname (&(info->mod.atec.evenroll_name));

      MLOG_CALWAYS0 ("Event Condition Name Follows:");
      m_log_objname (&(info->mod.atec.evcon_name));

      MLOG_CALWAYS1 ("Causing Transitions : %02x",
      				info->mod.atec.causing_transitions);

      if (info->mod.atec.causing_transitions & 0x01)
        {
        MLOG_CALWAYS0 ("Idle to Disabled : Set");
        }
      else
        {
        MLOG_CALWAYS0 ("Idle to Disabled : Not Set");	        
        }

      if (info->mod.atec.causing_transitions & 0x02)
        {
        MLOG_CALWAYS0 ("Active to Disabled : Set");
        }
      else 
        {
        MLOG_CALWAYS0 ("Active to Disabled : Not Set");
        }

      if (info->mod.atec.causing_transitions & 0x04)
        {
        MLOG_CALWAYS0 ("Disabled to Idle : Set");
        }
      else
        {
        MLOG_CALWAYS0 ("Disabled to Idle : Not Set");
        }

      if (info->mod.atec.causing_transitions & 0x08)
        {
        MLOG_CALWAYS0 ("Active to Idle : Set");
        }
      else
        {
        MLOG_CALWAYS0 ("Active to Idle : Not Set");
        }

      if (info->mod.atec.causing_transitions & 0x10)
        {
        MLOG_CALWAYS0 ("Disabled to Active : Set");
        }
      else
        {
        MLOG_CALWAYS0 ("Disabled to Active : Not Set");
        }

      if (info->mod.atec.causing_transitions & 0x20)
        {
        MLOG_CALWAYS0 ("Idle to Active : Set");
        }
      else
        {
        MLOG_CALWAYS0 ("Idle to Active : Not Set");
        }

      if (info->mod.atec.causing_transitions & 0x40)
        {
        MLOG_CALWAYS0 ("Any to Deleted : Set");
        }
      else
        {
        MLOG_CALWAYS0 ("Any to Deleted : Not Set");
        }

      if (info->mod.atec.acc_delay_pres)
        {
        MLOG_CALWAYS1 ("Acceptable Delay : %lu",(ST_ULONG)info->mod.atec.acc_delay);
        }
      else
        {
        MLOG_CALWAYS0 ("Acceptable Delay : Not Present ");
        }
      break;

    case (1)  :
      MLOG_CALWAYS0 ("Attach to Semaphore");
      MLOG_CALWAYS0 ("Semaphore Name Follows:");
      m_log_objname (&(info->mod.atsem.sem_name));

      if (info->mod.atsem.named_token_pres)
        {
        MLOG_CALWAYS1 ("Named Token : %s",info->mod.atsem.named_token);
        }
      else
        {
        MLOG_CALWAYS0 ("Named Token : ot Present");
        }

      MLOG_CALWAYS1 ("Priority : %u",info->mod.atsem.priority);
      if (info->mod.atsem.acc_delay_pres)
        {
        MLOG_CALWAYS1 ("Acceptable Delay : %lu",(ST_ULONG)info->mod.atsem.acc_delay);
        }
      else
        {
        MLOG_CALWAYS0 ("Acceptable Delay : Not Present");
        }

      if (info->mod.atsem.ctrl_timeout_pres)
        {
        MLOG_CALWAYS1 ("Control Timeout : %lu",(ST_ULONG)info->mod.atsem.ctrl_timeout);
        }
      else
        {
        MLOG_CALWAYS0 ("Control Timeout : Not Present");
        }

      if (info->mod.atsem.abrt_on_timeout_pres)
        {
	if (info->mod.atsem.abrt_on_timeout)
          {
          MLOG_CALWAYS0 ("Abort On Timeout : Yes");
          }
	else
          {
          MLOG_CALWAYS0 ("Abort On Timeout : No");
          }
	}
      else
        {
        MLOG_CALWAYS0 ("Abort On Timeout : Not Present ");
        }

      if (info->mod.atsem.rel_conn_lost)
        {
        MLOG_CALWAYS0 ("Relinquish if Conn Lost : Yes");
        }
      else
        {
        MLOG_CALWAYS0 ("Relinquish if Conn Lost : No");
        }
      break;

    default   :
      MLOG_CALWAYS1 ("Invalid Tag : %d",info->modifier_tag);
      break;
    }
  }


/************************************************************************/
/************************************************************************/
/************************************************************************/
/************************************************************************/

static ST_CHAR *err_class_msg[] = 
  {									
  "VMD State",				/* Class = 0			*/
  "Application Reference",		/* Class = 1			*/
  "Definition",				/* Class = 2			*/
  "Resource",				/* Class = 3			*/
  "Service",				/* Class = 4			*/
  "Service Preempt",			/* Class = 5			*/
  "Time Resolution",			/* Class = 6			*/
  "Access",				/* Class = 7			*/
  "Initiate",				/* Class = 8			*/
  "Conclude",				/* Class = 9			*/
  "Cancel",				/* Class = 10			*/
  "File"				/* Class = 11			*/
  };

static ST_CHAR *vmd_err_code_msg[] = 
  {
  "Other",
  "VMD State Conflict",
  "VMD Operational Problem",
  "Domain Transfer Problem",
  "State Machine ID Invalid"
  };

static ST_CHAR *app_ref_err_code_msg[] = 
  {
  "Other",
  "Aplication Unreachable",
  "Connection Lost",
  "Application Reference Invalid",
  "Context Unsupported"
  };

static ST_CHAR *def_err_code_msg[] = 
  {
  "Other",
  "Object Undefined",
  "Invalid Address",
  "Type Unsupported",
  "Type Inconsistent",
  "Object Exists",
  "Object Attribute Inconsistent"
  };

static ST_CHAR *res_err_code_msg[] = 
  {
  "Other",
  "Memory Unavailable",
  "Processor Resource Unavailable",
  "Mass Storage Unavailable",
  "Capability Unavailable",
  "Capability Unknown"
  };

static ST_CHAR *ser_err_code_msg[] = 
  {
  "Other",
  "Primitive Out Of Sequence",
  "Object State Conflict",
  "PDU Size",
  "Continuation Invalid",
  "Object Constraint Conflict"
  };

static ST_CHAR *ser_pre_err_code_msg[] = 
  {
  "Other",
  "Timeout",
  "Deadlock",
  "Cancel"
  };

static ST_CHAR *tim_res_err_code_msg[] = 
  {
  "Other",
  "Unsupportable Time Resolution"
  };

static ST_CHAR *acc_err_code_msg[] = 
  {
  "Other",
  "Object Access Unsupported",
  "Object Non Existent",
  "Object Access Denied",
  "Object Invalidated"
  };

static ST_CHAR *init_err_code_msg[] = 
  {
  "Other",
  "Version Incompatible",
  "Max Segment Insufficient",
  "Oustanding Calling Insufficient",
  "Oustanding Called Insufficent",
  "Service CBB Insufficient",
  "Parameter CBB Insufficient",
  "Nesting Level Insufficient"
  };

static ST_CHAR *concl_err_code_msg[] = 
  {
  "Other",
  "Further Communication Req'd"
  };

static ST_CHAR *cancl_err_code_msg[] = 
  {
  "Other",
  "Invoke Id Unknown",
  "Cancel Not Possible"
  };

static ST_CHAR *file_err_code_msg[] = 
  {
  "Other",
  "FileName Ambiguous",
  "File Busy",
  "FileName Syntax Error",
  "Content Type Invalid",
  "Position Invalid",
  "File Access Denied",
  "File Non Existent",
  "Duplicate Filename",
  "Insufficient Space In Filestore"
 };

/************************************************************************/


static ST_CHAR *rej_class_msg[] = 
  {
  "",
  "Request PDU",
  "Response PDU",
  "Error PDU",
  "Unconfirmed PDU",
  "PDU Error",
  "Cancel Request",
  "Cancel Response",
  "Cancel Error",
  "Conclude Request",
  "Conclude Response",
  "Conclude Error"
  };

static ST_CHAR *class1_rej_code_msg[] = 
  {
  "Other",
  "Unrecognized Service",
  "Unrecognized Modifier",
  "Invalid Invoke ID",
  "Invalid Argument",
  "Invalid Modifier",
  "Max Serv Outstanding Exceeded",
  "Reserved",
  "Max Recursion Exceeded",
  "Value Out Of Range"
  };	

static ST_CHAR *class2_rej_code_msg[] = 
  {
  "Other",
  "Unrecognized Service",
  "Invalid InvokeID",
  "Invalid Result",
  "Reserved",
  "Max Recursion Exceeded",
  "Value Out Of Range"
  };
static ST_CHAR *class3_rej_code_msg[] = 
  {
  "Other",
  "Unrecognized Service",
  "Invalid InvokeID",
  "Invalid Service Error",
  "Value Out Of Range"
  };
static ST_CHAR *class4_rej_code_msg[] = 
  {
  "Other",
  "Unrecognized Service",
  "Invalid Argument",
  "Max Recursion Exceeded",
  "Value Out Of Range"
  };
static ST_CHAR *class5_rej_code_msg[] = 
  {
  "Unknown PDU Type",
  "Invalid PDU",
  "Illegal ACSE Mapping"
  };
static ST_CHAR *class6_rej_code_msg[] = 
  {
  "Other",
  "Invalid InvokeID"
  };
static ST_CHAR *class7_rej_code_msg[] = 
  {
  "Other",
  "Invalid InvokeID"
  };
static ST_CHAR *class8_rej_code_msg[] = 
  {
  "Other",
  "Invalid InvokeID",
  "Invalid ServiceError",
  "Value Out Of Range"
  };
static ST_CHAR *class9_rej_code_msg[] = 
  {
  "Other",
  "Invalid Argument"
  };
static ST_CHAR *class10_rej_code_msg[] = 
  {
  "Other",
  "Invalid Result"
  };
static ST_CHAR *class11_rej_code_msg[] = 
  {
  "Other",
  "Invalid ServiceError",
  "Value Out Of Range"
  };

/************************************************************************/
/************************************************************************/
/*			m_log_error_info 				*/
/************************************************************************/
/* Called to log an 'err_info' structure				*/

ST_VOID m_log_error_info (ERR_INFO *err_ptr)
  {
  if (err_ptr->adtnl.mod_pos_pres)
    {
    MLOG_CALWAYS1 ("  Modifier position : %ld", (ST_LONG)err_ptr->adtnl.mod_pos);
    }

  MLOG_CALWAYS1 ("  Problem Type : %d", err_ptr->eclass);
  MLOG_CALWAYS1 ("  Problem Code : %d", err_ptr->code);
  if (err_ptr->adtnl.info_pres)
    {
    MLOG_CALWAYS0 ("  Additional Information : ");
    if (err_ptr->adtnl.code_pres)
      {
      MLOG_CALWAYS1 ("    Code : %ld", (ST_LONG)err_ptr->adtnl.code);
      }
    if (err_ptr->adtnl.descr_pres)
      {
      MLOG_CALWAYS1 ("    Description : %s", err_ptr->adtnl.descr);
      }
    if (err_ptr->adtnl.ssi_pres)
      {
      MLOG_CALWAYS0 ("    Additional Service Specific Information Follows.");
      MLOG_CALWAYS1 ("    Service : %d", err_ptr->adtnl.service);
      if (err_ptr->adtnl.service == 10)
        {
        MLOG_CALWAYS0("    Additional Error Data Follows in HEX:");
        MLOG_ALWAYSH (err_ptr->adtnl.ss_error_len, 
                      err_ptr->adtnl.ss_error_data); 
        }
      else
        {
        MLOG_CALWAYS1 ("    Error : %d", err_ptr->adtnl.ss_error_val);
        }
      }
    }
  }

/************************************************************************/
/************************************************************************/

ST_VOID m_get_mms_err_text (ST_INT eclass, ST_INT code, 
	ST_CHAR *dest, ST_INT dest_len)
  {
ST_CHAR buf[200];
ST_CHAR buf2[100];
ST_CHAR **msg_tbl;
ST_BOOLEAN code_exceeds_max = SD_FALSE;	/* set SD_TRUE if max exceeded	*/

  if (eclass > 11 || eclass < 0)
    {
    strncpy_safe (dest, "Bad Error Class", dest_len-1);
    return;
    }  
  if (code < 0)
    {	/* Negative error code. Yikes!	*/
    strncpy_safe (dest, "Bad Error Code", dest_len-1);
    return;
    }  

  sprintf (buf,"%s, ", err_class_msg[eclass]);

  /* NOTE: "code" used as index into each array. If index exceeds array	*/
  /*       size OR we didn't find msg array, we just write the code	*/
  /*       number in the message.					*/
  switch (eclass)
    {
    case 0 :
      msg_tbl = vmd_err_code_msg; 
      if (code >= (sizeof(vmd_err_code_msg)/sizeof(ST_CHAR *)))
        code_exceeds_max = SD_TRUE;
    break;

    case 1 :
      msg_tbl = app_ref_err_code_msg; 
      if (code >= (sizeof(app_ref_err_code_msg)/sizeof(ST_CHAR *)))
        code_exceeds_max = SD_TRUE;
    break;

    case 2 :
      msg_tbl = def_err_code_msg; 
      if (code >= (sizeof(def_err_code_msg)/sizeof(ST_CHAR *)))
        code_exceeds_max = SD_TRUE;
    break;

    case 3 :
      msg_tbl = res_err_code_msg; 
      if (code >= (sizeof(res_err_code_msg)/sizeof(ST_CHAR *)))
        code_exceeds_max = SD_TRUE;
    break;

    case 4 :
      msg_tbl = ser_err_code_msg; 
      if (code >= (sizeof(ser_err_code_msg)/sizeof(ST_CHAR *)))
        code_exceeds_max = SD_TRUE;
    break;

    case 5 :
      msg_tbl = ser_pre_err_code_msg; 
      if (code >= (sizeof(ser_pre_err_code_msg)/sizeof(ST_CHAR *)))
        code_exceeds_max = SD_TRUE;
    break;

     case 6 :
      msg_tbl = tim_res_err_code_msg; 
      if (code >= (sizeof(tim_res_err_code_msg)/sizeof(ST_CHAR *)))
        code_exceeds_max = SD_TRUE;
    break;

   case 7 :
      msg_tbl = acc_err_code_msg; 
      if (code >= (sizeof(acc_err_code_msg)/sizeof(ST_CHAR *)))
        code_exceeds_max = SD_TRUE;
    break;

    case 8 :
      msg_tbl = init_err_code_msg; 
      if (code >= (sizeof(init_err_code_msg)/sizeof(ST_CHAR *)))
        code_exceeds_max = SD_TRUE;
    break;

    case 9 :
      msg_tbl = concl_err_code_msg; 
      if (code >= (sizeof(concl_err_code_msg)/sizeof(ST_CHAR *)))
        code_exceeds_max = SD_TRUE;
    break;

     case 10 :
      msg_tbl = cancl_err_code_msg; 
      if (code >= (sizeof(cancl_err_code_msg)/sizeof(ST_CHAR *)))
        code_exceeds_max = SD_TRUE;
    break;

      case 11 :
      msg_tbl = file_err_code_msg; 
      if (code >= (sizeof(file_err_code_msg)/sizeof(ST_CHAR *)))
        code_exceeds_max = SD_TRUE;
    break;

    default :
      msg_tbl = NULL; 
    break;
    }
  if (msg_tbl == NULL || code_exceeds_max)
    {	/* didn't find msg array or code exceeds array max	*/
    sprintf (buf2,"Code %d",code);
    strcat (buf,buf2);
    }
  else
    strcat (buf,msg_tbl[code]);

  strncpy_safe (dest, buf, dest_len-1);
  }

/************************************************************************/
/************************************************************************/

ST_VOID m_get_mms_rej_text (ST_INT rclass, ST_INT code, 
	ST_CHAR *dest, ST_INT dest_len)
  {
ST_CHAR buf[200];
ST_CHAR buf2[100];
ST_CHAR **msg_tbl;
ST_BOOLEAN code_exceeds_max = SD_FALSE;	/* set SD_TRUE if max exceeded	*/

  if (rclass > 11 || rclass < 1)
    {
    strncpy_safe (dest, "Bad Reject Class", dest_len-1);
    return;
    }  
  if (code < 0)
    {	/* Negative reject code. Yikes!	*/
    strncpy_safe (dest, "Bad Reject Code", dest_len-1);
    return;
    }  

  sprintf (buf,"%s, ", rej_class_msg[rclass]);

  /* NOTE: "code" used as index into each array. If index exceeds array	*/
  /*       size OR we didn't find msg array, we just write the code	*/
  /*       number in the message.					*/
  switch (rclass)
    {
    case 1 :
      msg_tbl = class1_rej_code_msg; 
      if (code >= (sizeof(class1_rej_code_msg)/sizeof(ST_CHAR *)))
        code_exceeds_max = SD_TRUE;
    break;

    case 2 :
      msg_tbl = class2_rej_code_msg;
      if (code >= (sizeof(class2_rej_code_msg)/sizeof(ST_CHAR *)))
        code_exceeds_max = SD_TRUE;
    break;

    case 3 :
      msg_tbl = class3_rej_code_msg;
      if (code >= (sizeof(class3_rej_code_msg)/sizeof(ST_CHAR *)))
        code_exceeds_max = SD_TRUE;
    break;

    case 4 :
      msg_tbl = class4_rej_code_msg;
      if (code >= (sizeof(class4_rej_code_msg)/sizeof(ST_CHAR *)))
        code_exceeds_max = SD_TRUE;
    break;

    case 5 :
      msg_tbl = class5_rej_code_msg;
      if (code >= (sizeof(class5_rej_code_msg)/sizeof(ST_CHAR *)))
        code_exceeds_max = SD_TRUE;
    break;

    case 6 :
      msg_tbl = class6_rej_code_msg;
      if (code >= (sizeof(class6_rej_code_msg)/sizeof(ST_CHAR *)))
        code_exceeds_max = SD_TRUE;
    break;

     case 7 :
      msg_tbl = class7_rej_code_msg;
      if (code >= (sizeof(class7_rej_code_msg)/sizeof(ST_CHAR *)))
        code_exceeds_max = SD_TRUE;
    break;

    case 8 :
      msg_tbl = class8_rej_code_msg;
      if (code >= (sizeof(class8_rej_code_msg)/sizeof(ST_CHAR *)))
        code_exceeds_max = SD_TRUE;
    break;

   case 9 :
      msg_tbl = class9_rej_code_msg;
      if (code >= (sizeof(class9_rej_code_msg)/sizeof(ST_CHAR *)))
        code_exceeds_max = SD_TRUE;
    break;

    case 10 :
      msg_tbl = class10_rej_code_msg;
      if (code >= (sizeof(class10_rej_code_msg)/sizeof(ST_CHAR *)))
        code_exceeds_max = SD_TRUE;
    break;

    case 11 :
      msg_tbl = class11_rej_code_msg;
      if (code >= (sizeof(class11_rej_code_msg)/sizeof(ST_CHAR *)))
        code_exceeds_max = SD_TRUE;
    break;

    default :
      msg_tbl = NULL; 
    break;
    }
  if (msg_tbl == NULL || code_exceeds_max)
    {	/* didn't find msg array or code exceeds array max	*/
    sprintf (buf2,"Code %d",code);
    strcat (buf,buf2);
    }
  else
    strcat (buf,msg_tbl[code]);

  strncpy_safe (dest, buf, dest_len-1);
  }

/************************************************************************/
/************************************************************************/

ST_VOID m_log_reject_info (REJECT_RESP_INFO *rej_ptr)
  {
ST_CHAR msg_buf [100];

  if (rej_ptr->detected_here)
    {
    MLOG_CALWAYS0 ("Reject Detected Here.");
    }
  else
    {
    MLOG_CALWAYS0 ("Reject Detected At Peer.");
    }
  if (rej_ptr->invoke_known)
    {
    MLOG_CALWAYS1 ("Invoke ID : %lu", (ST_ULONG)rej_ptr->invoke);
    }
  else
    {
    MLOG_CALWAYS0 ("Invoke ID NOT KNOWN");
    }
  MLOG_CALWAYS1 ("Reject Class : %d", rej_ptr->rej_class);
  MLOG_CALWAYS1 ("Reject Code : %d", rej_ptr->rej_code);
  
  /* Convert Class and Code to text.	*/
  m_get_mms_rej_text (rej_ptr->rej_class, rej_ptr->rej_code,
        msg_buf, sizeof(msg_buf)-1);
  MLOG_CALWAYS1 ("Rejected : %s", msg_buf);
  }

/************************************************************************/
/************************************************************************/

ST_VOID m_no_log_resp (ST_VOID *resp)
  {
  }

ST_VOID m_no_log_req  (ST_VOID *req)
  {
  }



