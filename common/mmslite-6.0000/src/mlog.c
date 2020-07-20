/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2010, All Rights Reserved.		        */
/*									*/
/* MODULE NAME : mlog.c							*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*									*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 04/13/09  EJV           Added indents in some slogs.			*/
/* 11/01/07  EJV     07    Substituted multiple slogs with one.		*/
/* 10/14/03  JRB     06    Del unnecessary casts.			*/
/* 11/05/02  EJV     05    m_log_conf_err_info: cor log for Initiate Err*/
/* 09/13/99  MDE     04    Added SD_CONST modifiers			*/
/* 03/20/98  JRB     03    Don't need mmsop_en.h anymore.		*/
/* 08/04/97  MDE     02    Moved table initialization to mmsop_en.h	*/
/* 05/16/97  DSF     01    Cleaned up compilation warnings on HP-UX	*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "mms_usr.h"
#include "mloguser.h"


/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/

SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;

/************************************************************************/
/************************************************************************/

ST_VOID m_log_conf_err_info (MMSREQ_PEND *conf);

/************************************************************************/
/************************************************************************/
/************************************************************************/

ST_VOID m_log_ureq_info (ST_INT chan, ST_INT op, ST_VOID *info)
  {
  MLOG_ALWAYS2 ("ISSUING MMS UNCONFIRMED REQUEST \n"
                "  Channel = %d \n"
                "  Opcode  = %u", chan, op);

  if (op <= MAX_MMSOP_DIS)
    {
    MLOG_CALWAYS1 ("Operation is : %s",mms_op_string[op]);
    (*m_req_log_fun_tbl [op]) (info);
    }
  }

/************************************************************************/

ST_VOID m_log_req_info (MMSREQ_PEND *req, ST_VOID *info)
  {
MODIFIER *mod_ptr;
ST_INT i;

  MLOG_ALWAYS4 ("ISSUING MMS REQUEST \n"
                "  Channel   = %d   \n"
                "  Context   = %02x \n"
                "  Invoke ID = %lu  \n"
                "  Opcode    = %u",
                req->chan, req->context, req->id, req->op);
  if (req->mods.info_pres)
    {
    for (i = 0; i < req->mods.num_of_mods; i++)
      {
      mod_ptr = req->mods.mod_list_ptr + i;
      MLOG_CALWAYS1 ("Modifier %d",i);
      m_log_modifier (mod_ptr);
      }
    }      
  if (req->cs.cs_pres)
    {
    MLOG_CALWAYS0 ("Companion Standard Information Present :");
    MLOG_ALWAYSH (req->cs.cs_len, req->cs.cs_ptr);
    }      

  if (req->op <= MAX_MMSOP_DIS)
    {
    MLOG_CALWAYS1 ("Operation is : %s",mms_op_string[req->op]);
    (*m_req_log_fun_tbl [req->op]) (req->req_info_ptr);
    }
  }

/************************************************************************/

ST_VOID m_log_ind_info (MMSREQ_IND *ind)
  {
MODIFIER *mod_ptr;
ST_INT i;

  MLOG_ALWAYS4 ("MMS INDICATION RECEIVED \n"
                "  Channel   = %d   \n"
		"  Context   = %02x \n"
		"  Invoke ID = %lu  \n"
		"  Opcode    = %u",
                ind->chan, ind->context, ind->id, ind->op);
  if (ind->mods.info_pres)
    {
    for (i = 0; i < ind->mods.num_of_mods; i++)
      {
      mod_ptr = ind->mods.mod_list_ptr + i;
      MLOG_CALWAYS1 ("Modifier %d",i);
      m_log_modifier (mod_ptr);
      }
    }      
  if (ind->cs.cs_pres)
    {
    MLOG_CALWAYS0 ("Companion Standard Information Present :");
    MLOG_ALWAYSH (ind->cs.cs_len, ind->cs.cs_ptr);
    }      

  if (ind->op <= MAX_MMSOP_DIS)
    {
    MLOG_CALWAYS1 ("Operation is : %s",mms_op_string[ind->op]);
    (*m_req_log_fun_tbl [ind->op]) (ind->req_info_ptr);
    }
  }

/************************************************************************/

ST_VOID m_log_resp_info (MMSREQ_IND *resp, ST_VOID *info)
  {
  MLOG_ALWAYS4 ("ISSUING MMS RESPONSE \n"
                "  Channel   = %d   \n"
		"  Context   = %02x \n"
		"  Invoke ID = %lu  \n"
		"  Opcode    = %u",
                resp->chan, resp->context, resp->id, resp->op);
  if (resp->cs.cs_pres)
    {
    MLOG_CALWAYS0 ("Companion Standard Information Present :");
    MLOG_ALWAYSH (resp->cs.cs_len, resp->cs.cs_ptr);
    }      

  if (resp->op <= MAX_MMSOP_DIS)
    {
    MLOG_CALWAYS1 ("Operation is : %s",mms_op_string[resp->op]);
    (*m_resp_log_fun_tbl [resp->op]) (info);
    }   
  }

/************************************************************************/

ST_VOID m_log_conf_info (MMSREQ_PEND *conf)
  {
  MLOG_ALWAYS4 ("MMS CONFIRM RECEIVED \n"
                "  Channel   = %d   \n"
		"  Context   = %02x \n"
		"  Invoke ID = %lu  \n"
		"  Opcode    = %u",
                conf->chan, conf->context, conf->id, conf->op);
  if (conf->cs.cs_pres)
    {
    MLOG_CALWAYS0 ("Companion Standard Information Present :");
    MLOG_ALWAYSH (conf->cs.cs_len, conf->cs.cs_ptr);
    }      

  MLOG_CALWAYS1 ("Elapsed Time : %ld Seconds",conf->resp_time - conf->req_time);

  if (conf->op <= MAX_MMSOP_DIS)
    {
    MLOG_CALWAYS1 ("Operation is : %s",mms_op_string[conf->op]);

    if (!conf->resp_err)
      {
      (*m_resp_log_fun_tbl [conf->op]) (conf->resp_info_ptr);
      }
    else
      {
      m_log_conf_err_info (conf);
      }
    }
  }

/************************************************************************/
/*			m_log_error_resp 				*/
/************************************************************************/

ST_VOID m_log_error_resp (MMSREQ_IND *resp, ST_INT16 eclass, ST_INT16 code)
  {
ERR_INFO err_info;
ST_CHAR buf[100];

  MLOG_ALWAYS4 ("ISSUING MMS ERROR RESPONSE \n"
                "  Channel   = %d   \n"
		"  Context   = %02x \n"
		"  Invoke ID = %lu  \n"
		"  Opcode    = %u",
                resp->chan, resp->context, resp->id, resp->op);
  if (resp->cs.cs_pres)
    {
    MLOG_CALWAYS0 ("Companion Standard Information Present :");
    MLOG_ALWAYSH (resp->cs.cs_len, resp->cs.cs_ptr);
    }      

  if (resp->op <= MAX_MMSOP_DIS)
    {
    MLOG_CALWAYS1 ("Operation is : %s",mms_op_string[resp->op]);
    }   

/* Translate the error code and class to text				*/
  m_get_mms_err_text (eclass, code, buf, sizeof (buf)-1);
  MLOG_CALWAYS1 ("Error : %s", buf);

  err_info.eclass = eclass;
  err_info.code = code;
  memcpy (&err_info.adtnl, &adtnl_err_info, 
		sizeof (ADTNL_ERR_RESP_INFO));
  m_log_error_info (&err_info);
  }


/************************************************************************/
/*			m_log_conf_err_info 				*/
/************************************************************************/
/* Called when a confirm with an error of any type has been received	*/

ST_VOID m_log_conf_err_info (MMSREQ_PEND *req_ptr)
  {
ERR_INFO 	*err_ptr;
REJECT_RESP_INFO *rej_ptr;
ST_CHAR buf[200];

  MLOG_CALWAYS0 ("RESPONSE ERROR : ");
  switch (req_ptr->resp_err)
    {
    case CNF_ERR_OK :				/* error response	*/
      MLOG_CALWAYS1 ("  Opcode : %u", req_ptr->op);
      MLOG_CALWAYS1 ("  Invoke ID : %lu", req_ptr->id);
      if (req_ptr->op == MMSOP_INITIATE)
        {
        INIT_INFO *init_info = (INIT_INFO *) req_ptr->resp_info_ptr; 
        /* Note: Initiate error response needs different pointer to ERR_INFO */
        if (init_info->rslt.err_info_pres)
          err_ptr = (ERR_INFO *) &init_info->rslt.err;
        else
          {
          MLOG_CALWAYS0 ("Error Info not present");
          break;
          }
        }
      else
        err_ptr = (ERR_INFO *) req_ptr->resp_info_ptr;

      m_get_mms_err_text (err_ptr->eclass, err_ptr->code, buf, sizeof (buf)-1);
      MLOG_CALWAYS1 ("Service Error : %s", buf);
      m_log_error_info (err_ptr);
    break;

    case CNF_PARSE_ERR :				/* parse error		*/
      MLOG_CALWAYS0 ("Response Parse Error");
    break;

    case CNF_REJ_ERR :				/* REJECT		*/
      rej_ptr = (REJECT_RESP_INFO *) req_ptr->resp_info_ptr;
      m_log_reject_info (rej_ptr);
    break;

    case CNF_DISCONNECTED :			/* channel terminated	*/
      MLOG_CALWAYS0 ("Connection Terminated");
    break;

    default : 				/* other error		*/
      MLOG_CALWAYS1 ("Error Code = 0x%04x", req_ptr->resp_err);
    break;
    }
  }

/************************************************************************/
/************************************************************************/

ST_RET m_mlog_install ()
  {
  m_log_ureq_info_fun  = m_log_ureq_info; 
  m_log_req_info_fun   = m_log_req_info; 
  m_log_ind_info_fun   = m_log_ind_info;
  m_log_resp_info_fun  = m_log_resp_info;
  m_log_conf_info_fun  = m_log_conf_info;
  m_log_error_resp_fun = m_log_error_resp;

  return (SD_SUCCESS);
  }



