/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 1997, All Rights Reserved.		        */
/*									*/
/* MODULE NAME : mlogl.c						*/
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
/* 03/03/10  JRB	   Del obsolete ml_mlog_install function.	*/
/* 01/20/10  JRB     07    Chg invokeId to ST_ULONG in "ml_log_*"	*/
/*			   (works with %lu format without casting).	*/
/* 03/11/04  GLB     06    Added "ifdef DEBUG_SISCO" for "thisFileName" */
/* 04/08/02  MDE     05    Moved log function pointers to mmsl_fin.c	*/
/* 04/05/02  MDE     04    Cleaned for runtime use by Lite apps		*/
/* 09/13/99  MDE     03    Added SD_CONST modifiers			*/
/* 03/03/99  MDE     02    Now log invoke ID for error			*/
/* 03/20/98  JRB     01    Don't need mmsop_en.h anymore.		*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "mmsdefs.h"
#include "mloguser.h"


/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/
/************************************************************************/

static ST_VOID _ml_log_reqind_info (ST_CHAR *pduTypeText, 
			    ST_ULONG invokeId, ST_INT op, ST_VOID *info);
static ST_VOID _ml_log_respconf_info (ST_CHAR *pduTypeText,
			      ST_ULONG invokeId, ST_INT op, ST_VOID *info);
static ST_VOID _ml_log_unsol_reqind_ind_info (ST_CHAR *pduTypeText, 
				      ST_INT op, ST_VOID *info);
static ST_VOID _ml_log_error_respconf (ST_CHAR *pduTypeText,
			       ST_ULONG invokeId, ST_VOID *info);
static ST_VOID _ml_log_reject_sendrecv (ST_CHAR *pduTypeText, ST_VOID *info);

/************************************************************************/
/************************************************************************/
/*			ml_log_dec_rslt					*/
/************************************************************************/

ST_RET ml_log_dec_rslt (MMSDEC_INFO *rslt)
  {
ERR_INFO 	*err_ptr;
REJECT_RESP_INFO *rej_ptr;
ST_CHAR buf[200];
INIT_INFO *init_info;

  if (rslt->err_code == NO_DECODE_ERR)
    {
    switch (rslt->type)
      {
      case MMSREQ :
        _ml_log_reqind_info ("Request/Indication", 
			    rslt->id, rslt->op, rslt->data_ptr);
      break;

      case MMSRESP :
        _ml_log_respconf_info ("Response/Confirm", 
			    rslt->id, rslt->op, rslt->data_ptr);
      break;

      case MMSUNREQ :
        _ml_log_unsol_reqind_ind_info ("Request/Indication", 
				      rslt->op, rslt->data_ptr);
      break;

      case MMSINITREQ	:
        MLOG_ALWAYS0 ("MMS Initiate Request/Indication");
	init_info = (INIT_INFO *) rslt->data_ptr;
        init_info->mms_p_context_pres = SD_TRUE;
        (*m_req_log_fun_tbl [MMSOP_INITIATE]) (rslt->data_ptr);
      break;

      case MMSINITRESP :
        MLOG_ALWAYS0 ("MMS Initiate Response/Confirm");
	init_info = (INIT_INFO *) rslt->data_ptr;
        init_info->mms_p_context_pres = SD_TRUE;
        (*m_resp_log_fun_tbl [MMSOP_INITIATE]) (rslt->data_ptr);
      break;

      case MMSINITERR	:
        MLOG_ALWAYS0 ("MMS Initiate Error");
      break;

      case MMSCNCLREQ	:
        MLOG_ALWAYS0 ("MMS Conclude Request/Indication");
        (*m_req_log_fun_tbl [MMSOP_CONCLUDE]) (rslt->data_ptr);
      break;

      case MMSCNCLRESP :
        MLOG_ALWAYS0 ("MMS Conclude Response/Confirm");
        (*m_resp_log_fun_tbl [MMSOP_CONCLUDE]) (rslt->data_ptr);
      break;

      case MMSCNCLERR	:
        MLOG_ALWAYS0 ("MMS Conclude Error");
      break;

      case MMSCANREQ :
        MLOG_ALWAYS0 ("MMS Cancel Request/Indication");
        (*m_req_log_fun_tbl [MMSOP_CANCEL]) (rslt->data_ptr);
      break;

      case MMSCANRESP	:
        MLOG_ALWAYS0 ("MMS Conclude Response/Confirm");
        (*m_resp_log_fun_tbl [MMSOP_CANCEL]) (rslt->data_ptr);
      break;

      case MMSCANERR :
        MLOG_ALWAYS0 ("MMS Cancel Error");
      break;

      case MMSREJECT :
        rej_ptr = (REJECT_RESP_INFO *) rslt->data_ptr;
        m_log_reject_info (rej_ptr);
      break;

      case MMSERROR : 
        MLOG_CALWAYS1 ("Invoke ID = %lu", (ST_ULONG)rslt->id);
        err_ptr = (ERR_INFO *) rslt->data_ptr;
        m_get_mms_err_text (err_ptr->eclass, err_ptr->code, buf, sizeof (buf)-1);
        MLOG_CALWAYS1 ("Service Error : %s", buf);
        m_log_error_info (err_ptr);
      break;

      case UNKNOWNTYPE :
        MLOG_ALWAYS0 ("MMS Unknown PDU Type");
      break;
 
      default :
      break;
      }   
    }
  return (SD_SUCCESS);
  }

/************************************************************************/
/************************************************************************/
/*			_ml_log_req_info				*/
/************************************************************************/

ST_VOID _ml_log_req_info (ST_ULONG invokeId, ST_INT op, ST_VOID *info)
  {
  _ml_log_reqind_info ("Request", invokeId, op, info);
  }

/************************************************************************/
/*			_ml_log_ind_info  				*/
/************************************************************************/

ST_VOID _ml_log_ind_info (ST_ULONG invokeId, ST_INT op, ST_VOID *info)
  {
  _ml_log_reqind_info ("Indication", invokeId, op, info);
  }

/************************************************************************/
/*			_ml_log_reqind_info  				*/
/************************************************************************/

ST_VOID _ml_log_reqind_info (ST_CHAR *pduTypeText, 
			    ST_ULONG invokeId, ST_INT op, ST_VOID *info)
  {
  MLOG_ALWAYS1 ("MMS %s", pduTypeText);
  MLOG_CALWAYS1 ("Invoke ID = %lu", invokeId);
  MLOG_CALWAYS1 ("Opcode = %u", op);
  MLOG_CALWAYS1 ("Operation is : %s",mms_op_string[op]);
  (*m_req_log_fun_tbl [op]) (info);
  }

/************************************************************************/
/************************************************************************/
/*			_ml_log_resp_info				*/
/************************************************************************/

ST_VOID _ml_log_resp_info (ST_ULONG invokeId, ST_INT op, ST_VOID *info)
  {
  _ml_log_respconf_info ("Response", invokeId, op, info);
  }

/************************************************************************/
/*			_ml_log_conf_info  				*/
/************************************************************************/

ST_VOID _ml_log_conf_info (ST_ULONG invokeId, ST_INT op, ST_VOID *info)
  {
  _ml_log_respconf_info ("Confirm", invokeId, op, info);
  }

/************************************************************************/
/*			_ml_log_respconf_info  				*/
/************************************************************************/

ST_VOID _ml_log_respconf_info (ST_CHAR *pduTypeText,
			      ST_ULONG invokeId, ST_INT op, ST_VOID *info)
  {
  MLOG_ALWAYS1 ("MMS %s", pduTypeText);
  MLOG_CALWAYS1 ("Invoke ID = %lu", invokeId);
  MLOG_CALWAYS1 ("Opcode = %u", op);
  MLOG_CALWAYS1 ("Operation is : %s",mms_op_string[op]);
  (*m_resp_log_fun_tbl [op]) (info);
  }


/************************************************************************/
/************************************************************************/
/*			_ml_log_unsol_req_info				*/
/************************************************************************/

ST_VOID _ml_log_unsol_req_info (ST_INT op, ST_VOID *info)
  {
  _ml_log_unsol_reqind_ind_info ("Request", op, info);
  }

/************************************************************************/
/*			_ml_log_unsol_ind_info  			*/
/************************************************************************/

ST_VOID _ml_log_unsol_ind_info (ST_INT op, ST_VOID *info)
  {
  _ml_log_unsol_reqind_ind_info ("Indication", op, info);
  }

/************************************************************************/
/*			_ml_log_unsol_reqind_ind_info  			*/
/************************************************************************/

ST_VOID _ml_log_unsol_reqind_ind_info (ST_CHAR *pduTypeText, 
				      ST_INT op, ST_VOID *info)
  {
  MLOG_ALWAYS1 ("MMS Unconfirmed Service %s", pduTypeText);
  MLOG_CALWAYS1 ("Operation is : %s",mms_op_string[op]);
  (*m_req_log_fun_tbl [op]) (info);
  }

/************************************************************************/
/************************************************************************/
/*			_ml_log_error_resp				*/
/************************************************************************/

ST_VOID _ml_log_error_resp (ST_ULONG invokeId, ST_VOID *info)
  {
  _ml_log_error_respconf ("Response", invokeId, info);
  }

/************************************************************************/
/*			_ml_log_error_conf  				*/
/************************************************************************/

ST_VOID _ml_log_error_conf (ST_ULONG invokeId, ST_VOID *info)
  {
  _ml_log_error_respconf ("Confirm", invokeId, info);
  }

/************************************************************************/
/*			_ml_log_error_respconf  			*/
/************************************************************************/

ST_VOID _ml_log_error_respconf (ST_CHAR *pduTypeText,
			       ST_ULONG invokeId, ST_VOID *info)
  {
ERR_INFO 	*err_ptr;
ST_CHAR buf[200];

  MLOG_ALWAYS1 ("MMS Error %s", pduTypeText);
  MLOG_CALWAYS1 ("Invoke ID = %lu", invokeId);
  err_ptr = (ERR_INFO *) info;
  m_get_mms_err_text (err_ptr->eclass, err_ptr->code, buf, sizeof (buf)-1);
  MLOG_CALWAYS1 ("Service Error : %s", buf);
  m_log_error_info (err_ptr);
  }


/************************************************************************/
/************************************************************************/
/*			_ml_log_reject_send				*/
/************************************************************************/

ST_VOID _ml_log_reject_send (ST_VOID *info)
  {
  _ml_log_reject_sendrecv ("Sent", info);
  }

/************************************************************************/
/*			_ml_log_reject_recv  				*/
/************************************************************************/

ST_VOID _ml_log_reject_recv (ST_VOID *info)
  {
  _ml_log_reject_sendrecv ("Received", info);
  }

/************************************************************************/
/*			_ml_log_reject_sendrecv  			*/
/************************************************************************/

ST_VOID _ml_log_reject_sendrecv (ST_CHAR *pduTypeText, ST_VOID *info)
  {
REJECT_RESP_INFO *rej_ptr;

  MLOG_ALWAYS1 ("MMS Reject %s", pduTypeText);
  rej_ptr = (REJECT_RESP_INFO *) info;
  m_log_reject_info (rej_ptr);
  }

