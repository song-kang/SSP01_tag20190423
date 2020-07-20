/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	1994 - 1996, All Rights Reserved		        */
/*									*/
/* MODULE NAME : mvlop_en.c    						*/
/* PRODUCT(S)  : MMSEASE-LITE						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*			mvl_cancel_ind_rcvd				*/
/*			mvl_concl_ind_rcvd				*/
/*			mvl_concl_conf_rcvd				*/
/*			mvl_ind_rcvd					*/
/*			mvl_conf_rcvd					*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   ------------------------------------------	*/
/* 04/13/10  JRB	   Don't decrement numpend_req in mvl_conf_rcvd	*/
/*			   (now decremented ONLY in _mvl_set_req_done).	*/
/* 11/12/07  MDE     17    Added _mvl_getpath, changed _mvl_tmpnam	*/
/* 08/12/05  JRB     16    Add #error if unsupported service enabled.	*/
/* 10/20/03  JRB     15    Fix print formats.				*/
/* 02/04/03  MDE     14    Added _mvl_tmpnam				*/
/* 07/09/02  MDE     13    Add maxpend_ind support			*/
/* 05/21/01  MDE     12    Now check return from indication ctrl alloc	*/
/* 03/15/01  JRB     11    Added call to _mvl_process_ustatus_ind.	*/
/* 10/25/00  JRB     10    Added #ifdef ICCP_LITE.			*/
/* 09/25/00  JRB     09    Added server support for Domain & PI services*/
/* 09/25/00  JRB     08    Total rewrite without function pointers.	*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"

#include "mmsdefs.h"
#include "mvl_defs.h"
#include "mmsop_en.h"

#include "mvl_log.h"

#if defined(ICCP_LITE)
#include "mi.h"
#endif

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/
/************************************************************************/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/
/*			mvl_cancel_ind_rcvd				*/
/* A MMS Cancel Indication has been received. Process it.		*/
/************************************************************************/
ST_VOID mvl_cancel_ind_rcvd (MVL_COMM_EVENT *event)
  {
#if (MMS_CANCEL_EN & RESP_EN)
        _mvl_process_cancel_ind (event);
#else
        _mvl_send_reject (event);
#endif
  }

/************************************************************************/
/*			mvl_concl_ind_rcvd				*/
/* A MMS Conclude Indication has been received. Process it.		*/
/************************************************************************/
ST_VOID mvl_concl_ind_rcvd (MVL_COMM_EVENT *event)
  {
#if (MMS_CONCLUDE_EN & RESP_EN)
        _mvl_process_concl_ind (event);
#else
        _mvl_send_reject (event);
#endif
  }

/************************************************************************/
/*			mvl_concl_conf_rcvd				*/
/* A MMS Conclude Confirm has been received. Process it.		*/
/************************************************************************/
ST_VOID mvl_concl_conf_rcvd (MVL_REQ_PEND *req_pend)
  {
#if (MMS_CONCLUDE_EN & REQ_EN)
        _mvl_process_concl_conf (req_pend);
#else
        _mvl_conf_reject (req_pend);
#endif
  }

/************************************************************************/
/*			mvl_ind_rcvd					*/
/* A MMS request has been received. This is where the real action 	*/
/* takes place, and the response is generated				*/
/* NOTE: if a service is NOT enabled, the "switch" falls through to the	*/
/*       default case which sends a reject.				*/
/************************************************************************/

ST_VOID mvl_ind_rcvd (MVL_COMM_EVENT *event)
  {
MVL_IND_PEND *indCtrl;

  /* Information Reports & Unsol Status are special cases. Process first.*/
  /*   1. "event" used instead of "indCtrl".			*/
  /*   2. Keep different statistics.				*/
  if (event->u.mms.dec_rslt.op == MMSOP_INFO_RPT)
    {
#if (MMS_INFO_EN & RESP_EN)
#if defined(ICCP_LITE)
    _mic_process_info_ind (event);
#else
    _mvl_process_info_ind (event);
#endif
#else
    _mvl_send_reject (event);
#endif
    ++mvl_mms_statistics.clientInfoRpt;
    return;	/* do not continue	*/
    }
  else if (event->u.mms.dec_rslt.op == MMSOP_USTATUS)
    {
#if (MMS_USTATUS_EN & RESP_EN)
    _mvl_process_ustatus_ind (event);
#else
    _mvl_send_reject (event);
#endif
    ++mvl_mms_statistics.clientUstatus;
    return;	/* do not continue	*/
    }

  ++mvl_mms_statistics.serverInd;
  indCtrl = (MVL_IND_PEND *) M_CALLOC (MSMEM_IND_CTRL, 1, sizeof (MVL_IND_PEND));
  if (indCtrl == NULL)
    {
    MVL_LOG_NERR0 ("Could not allocate indication control, rejecting");
    _mvl_send_reject (event);
    return;
    }

  indCtrl->op = event->u.mms.dec_rslt.op;
  indCtrl->event = event;
  list_add_first(&(event->net_info->pend_ind),indCtrl);
  ++event->net_info->numpend_ind;

  switch (event->u.mms.dec_rslt.op)
    {
#if (MMS_READ_EN & RESP_EN)
    case MMSOP_READ :
#if defined(ICCP_LITE)
      _mis_process_read_ind (indCtrl);
#else
      _mvl_process_read_ind (indCtrl);
#endif
    break;
#endif
#if (MMS_WRITE_EN & RESP_EN)
    case MMSOP_WRITE :
#if defined(ICCP_LITE)
      _mis_process_write_ind (indCtrl);
#else
      _mvl_process_write_ind (indCtrl);
#endif
    break;
#endif

/*  case MMSOP_INFO_RPT :	handled before switch.	*/

#if (MMS_IDENT_EN & RESP_EN)
    case MMSOP_IDENTIFY :
      _mvl_process_ident_ind (indCtrl);
    break;
#endif
#if (MMS_STATUS_EN & RESP_EN)
    case MMSOP_STATUS :
      _mvl_process_status_ind (indCtrl);
    break;
#endif
#if (MMS_GETNAMES_EN & RESP_EN)
    case MMSOP_GET_NAMLIST:
#if defined(ICCP_LITE)
      _mis_process_getnam_ind (indCtrl);
#else
      _mvl_process_getnam_ind (indCtrl);
#endif
    break;
#endif
#if (MMS_GETVAR_EN & RESP_EN)
    case MMSOP_GET_VAR:
#if defined(ICCP_LITE)
      _mis_process_getvaa_ind (indCtrl);
#else
      _mvl_process_getvaa_ind (indCtrl);
#endif
    break;
#endif
#if (MMS_GETVLIST_EN & RESP_EN)
    case MMSOP_GET_VLIST:
#if defined(ICCP_LITE)
      _mis_process_getvla_ind (indCtrl);
#else
      _mvl_process_getvla_ind (indCtrl);
#endif
    break;
#endif
#if (MMS_FDIR_EN & RESP_EN)
    case MMSOP_FILE_DIR:
      _mvl_process_fdir_ind (indCtrl);
    break;
#endif
#if (MMS_FOPEN_EN & RESP_EN)
    case MMSOP_FILE_OPEN:
      _mvl_process_fopen_ind (indCtrl);
    break;
#endif
#if (MMS_FREAD_EN & RESP_EN)
    case MMSOP_FILE_READ:
      _mvl_process_fread_ind (indCtrl);
    break;
#endif
#if (MMS_FCLOSE_EN & RESP_EN)
    case MMSOP_FILE_CLOSE:
      _mvl_process_fclose_ind (indCtrl);
    break;
#endif
#if (MMS_FDELETE_EN & RESP_EN)
    case MMSOP_FILE_DELETE:
      _mvl_process_fdelete_ind (indCtrl);
    break;
#endif
#if (MMS_OBTAINFILE_EN & RESP_EN)
    case MMSOP_OBTAIN_FILE:
      _mvl_process_obtfile_ind (indCtrl);
    break;
#endif
#if (MMS_FRENAME_EN & RESP_EN)
    case MMSOP_FILE_RENAME:
      _mvl_process_frename_ind (indCtrl);
    break;
#endif
#if (MMS_DEFVLIST_EN & RESP_EN)
    case MMSOP_DEF_VLIST:
#if defined(ICCP_LITE)
      _mis_process_defvl_ind (indCtrl);
#else
      _mvl_process_defvlist_ind (indCtrl);
#endif
    break;
#endif
#if (MMS_DELVLIST_EN & RESP_EN)
    case MMSOP_DEL_VLIST:
#if defined(ICCP_LITE)
      _mis_process_delvlist_ind (indCtrl);
#else
      _mvl_process_delvlist_ind (indCtrl);
#endif
    break;
#endif
#if (MMS_JINIT_EN & RESP_EN)
    case MMSOP_INIT_JOURNAL:
      _mvl_process_jinit_ind (indCtrl);
    break;
#endif
#if (MMS_JREAD_EN & RESP_EN)
    case MMSOP_READ_JOURNAL:
      _mvl_process_jread_ind (indCtrl);
    break;
#endif
#if (MMS_JSTAT_EN & RESP_EN)
    case MMSOP_STAT_JOURNAL:
      _mvl_process_jstat_ind (indCtrl);
    break;
#endif
#if (MMS_INIT_DWN_EN & RESP_EN)
    case MMSOP_INIT_DOWNLOAD:	/* initiate download sequence	*/
      _mvl_process_initdown_ind (indCtrl);
    break;
#endif
#if (MMS_DWN_LOAD_EN & RESP_EN)
    case MMSOP_DOWN_LOAD:	/* download segment			*/
      _mvl_process_download_ind (indCtrl);
    break;
#endif
#if (MMS_TERM_DWN_EN & RESP_EN)
    case MMSOP_TERM_DOWNLOAD:	/* terminate download sequence	*/
      _mvl_process_termdown_ind (indCtrl);
    break;
#endif
#if (MMS_INIT_UPL_EN & RESP_EN)
    case MMSOP_INIT_UPLOAD:	/* initiate upload sequence		*/
      _mvl_process_initupl_ind (indCtrl);
    break;
#endif
#if (MMS_UP_LOAD_EN & RESP_EN)
    case MMSOP_UP_LOAD:		/* upload segment			*/
      _mvl_process_upload_ind (indCtrl);
    break;
#endif
#if (MMS_TERM_UPL_EN & RESP_EN)
    case MMSOP_TERM_UPLOAD:	/* terminate upload sequence		*/
      _mvl_process_termupl_ind (indCtrl);
    break;
#endif
#if (MMS_DEL_DOM_EN & RESP_EN)
    case MMSOP_DELETE_DOMAIN:	/* delete domain			*/
      _mvl_process_deldom_ind (indCtrl);
    break;
#endif
#if (MMS_GET_DOM_EN & RESP_EN)
    case MMSOP_GET_DOM_ATTR:	/* get domain attribute 		*/
      _mvl_process_getdom_ind (indCtrl);
    break;
#endif
#if (MMS_CRE_PI_EN & RESP_EN)
    case MMSOP_CREATE_PI:	/* create program invocation		*/
      _mvl_process_crepi_ind (indCtrl);
    break;
#endif
#if (MMS_DEL_PI_EN & RESP_EN)
    case MMSOP_DELETE_PI:	/* delete program invocation		*/
      _mvl_process_delpi_ind (indCtrl);
    break;
#endif
#if (MMS_START_EN & RESP_EN)
    case MMSOP_START:		/* start				*/
      _mvl_process_start_ind (indCtrl);
    break;
#endif
#if (MMS_STOP_EN & RESP_EN)
    case MMSOP_STOP:		/* stop 				*/
      _mvl_process_stop_ind (indCtrl);
    break;
#endif
#if (MMS_RESUME_EN & RESP_EN)
    case MMSOP_RESUME:		/* resume				*/
      _mvl_process_resume_ind (indCtrl);
    break;
#endif
#if (MMS_RESET_EN & RESP_EN)
    case MMSOP_RESET:		/* reset				*/
      _mvl_process_reset_ind (indCtrl);
    break;
#endif
#if (MMS_KILL_EN & RESP_EN)
    case MMSOP_KILL:		/* kill 				*/
      _mvl_process_kill_ind (indCtrl);
    break;
#endif
#if (MMS_GET_PI_EN & RESP_EN)
    case MMSOP_GET_PI_ATTR:	/* get program invocation attribute	*/
      _mvl_process_getpi_ind (indCtrl);
    break;
#endif
#if (MMS_GETCL_EN & RESP_EN)
    case MMSOP_GET_CAP_LIST:	/* get capability list	*/
      _mvl_process_getcl_ind (indCtrl);
    break;
#endif
    default :
      _mvl_ind_reject (indCtrl);
    break;
    }
  }

/************************************************************************/
/*			mvl_conf_rcvd					*/
/* A MMS response has been received. This is where the real action 	*/
/* takes place, and the response data is interpreted			*/
/* NOTE: if a service is NOT enabled, the "switch" falls through to the	*/
/*       default case which sends a reject.				*/
/************************************************************************/

ST_VOID mvl_conf_rcvd (MVL_COMM_EVENT *event)
  {
MVL_REQ_PEND *req_pend;
ST_UINT32 id;
ST_INT op;

  ++mvl_mms_statistics.clientReqOk;

  id = event->u.mms.dec_rslt.id;
  op = event->u.mms.dec_rslt.op;

/* First find the associated request control structure			*/
  req_pend = _mvl_find_req_ctrl (event->net_info, op, id);

  if (!req_pend)
    {
    MVL_LOG_NERR1 ("%s Confirmation : mvl_find_req_ctrl failed",
        mms_op_string [event->u.mms.dec_rslt.op]);
    _mvl_free_comm_event (event);	/* All done with this event	*/
    return;
    }

  /* Save "event" in req_pend. Free from mvl_free_req_ctrl.		*/
  /* This allows user code to use any data in event without copying it.	*/
  req_pend->event = event;

  switch (event->u.mms.dec_rslt.op)
    {
#if (MMS_READ_EN & REQ_EN)
    case MMSOP_READ :
#if defined(ICCP_LITE)
      _mic_process_read_conf (req_pend);
#else
      _mvl_process_read_conf (req_pend);
#endif
    break;
#endif
#if (MMS_WRITE_EN & REQ_EN)
    case MMSOP_WRITE :
      _mvl_process_write_conf (req_pend);
    break;
#endif
#if (MMS_IDENT_EN & REQ_EN)
    case MMSOP_IDENTIFY :
      _mvl_process_ident_conf (req_pend);
    break;
#endif
#if (MMS_FOPEN_EN & REQ_EN)
    case MMSOP_FILE_OPEN :
      _mvl_process_fopen_conf (req_pend);
    break;
#endif
#if (MMS_FREAD_EN & REQ_EN)
    case MMSOP_FILE_READ :
      _mvl_process_fread_conf (req_pend);
    break;
#endif
#if (MMS_FCLOSE_EN & REQ_EN)
    case MMSOP_FILE_CLOSE :
      _mvl_process_fclose_conf (req_pend);
    break;
#endif
#if (MMS_FDIR_EN & REQ_EN)
    case MMSOP_FILE_DIR :
      _mvl_process_fdir_conf (req_pend);
    break;
#endif
#if (MMS_FDELETE_EN & REQ_EN)
    case MMSOP_FILE_DELETE :
      _mvl_process_fdelete_conf (req_pend);
    break;
#endif
#if (MMS_FRENAME_EN & REQ_EN)
    case MMSOP_FILE_RENAME :
      _mvl_process_frename_conf (req_pend);
    break;
#endif
#if (MMS_OBTAINFILE_EN & REQ_EN)
    case MMSOP_OBTAIN_FILE :
      _mvl_process_obtfile_conf (req_pend);
    break;
#endif
#if (MMS_JINIT_EN & REQ_EN)
    case MMSOP_INIT_JOURNAL :
      _mvl_process_jinit_conf (req_pend);
    break;
#endif
#if (MMS_JREAD_EN & REQ_EN)
    case MMSOP_READ_JOURNAL :
      _mvl_process_jread_conf (req_pend);
    break;
#endif
#if (MMS_JWRITE_EN & REQ_EN)
    case MMSOP_WRITE_JOURNAL :
      _mvl_process_jwrite_conf (req_pend);
    break;
#endif
#if (MMS_JSTAT_EN & REQ_EN)
    case MMSOP_STAT_JOURNAL :
      _mvl_process_jstat_conf (req_pend);
    break;
#endif
#if (MMS_DEFVLIST_EN & REQ_EN)
    case MMSOP_DEF_VLIST :
      _mvl_process_defvlist_conf (req_pend);
    break;
#endif
#if (MMS_GETVLIST_EN & REQ_EN)
    case MMSOP_GET_VLIST :
      _mvl_process_getvlist_conf (req_pend);
    break;
#endif
#if (MMS_DELVLIST_EN & REQ_EN)
    case MMSOP_DEL_VLIST :
      _mvl_process_delvlist_conf (req_pend);
    break;
#endif
#if (MMS_GETVAR_EN & REQ_EN)
    case MMSOP_GET_VAR :
      _mvl_process_getvar_conf (req_pend);
    break;
#endif
#if (MMS_GETNAMES_EN & REQ_EN)
    case MMSOP_GET_NAMLIST :
      _mvl_process_getnam_conf (req_pend);
    break;
#endif
#if (MMS_STATUS_EN & REQ_EN)
    case MMSOP_STATUS :
      _mvl_process_status_conf (req_pend);
    break;
#endif
#if (MMS_INIT_DWN_EN & REQ_EN)
    case MMSOP_INIT_DOWNLOAD:	/* initiate download sequence	*/
      _mvl_process_initdown_conf (req_pend);
    break;
#endif
#if (MMS_DWN_LOAD_EN & REQ_EN)
    case MMSOP_DOWN_LOAD:	/* download segment			*/
      _mvl_process_download_conf (req_pend);
    break;
#endif
#if (MMS_TERM_DWN_EN & REQ_EN)
    case MMSOP_TERM_DOWNLOAD:	/* terminate download sequence	*/
      _mvl_process_termdown_conf (req_pend);
    break;
#endif
#if (MMS_INIT_UPL_EN & REQ_EN)
    case MMSOP_INIT_UPLOAD:	/* initiate upload sequence		*/
      _mvl_process_initupl_conf (req_pend);
    break;
#endif
#if (MMS_UP_LOAD_EN & REQ_EN)
    case MMSOP_UP_LOAD:		/* upload segment			*/
      _mvl_process_upload_conf (req_pend);
    break;
#endif
#if (MMS_TERM_UPL_EN & REQ_EN)
    case MMSOP_TERM_UPLOAD:	/* terminate upload sequence		*/
      _mvl_process_termupl_conf (req_pend);
    break;
#endif
#if (MMS_DEL_DOM_EN & REQ_EN)
    case MMSOP_DELETE_DOMAIN:	/* delete domain			*/
      _mvl_process_deldom_conf (req_pend);
    break;
#endif
#if (MMS_GET_DOM_EN & REQ_EN)
    case MMSOP_GET_DOM_ATTR:	/* get domain attributes 		*/
      _mvl_process_getdom_conf (req_pend);
    break;
#endif
#if (MMS_CRE_PI_EN & REQ_EN)
    case MMSOP_CREATE_PI:	/* create program invocation		*/
      _mvl_process_crepi_conf (req_pend);
    break;
#endif
#if (MMS_DEL_PI_EN & REQ_EN)
    case MMSOP_DELETE_PI:	/* delete program invocation		*/
      _mvl_process_delpi_conf (req_pend);
    break;
#endif
#if (MMS_START_EN & REQ_EN)
    case MMSOP_START:		/* start				*/
      _mvl_process_start_conf (req_pend);
    break;
#endif
#if (MMS_STOP_EN & REQ_EN)
    case MMSOP_STOP:		/* stop 				*/
      _mvl_process_stop_conf (req_pend);
    break;
#endif
#if (MMS_RESUME_EN & REQ_EN)
    case MMSOP_RESUME:		/* resume				*/
      _mvl_process_resume_conf (req_pend);
    break;
#endif
#if (MMS_RESET_EN & REQ_EN)
    case MMSOP_RESET:		/* reset				*/
      _mvl_process_reset_conf (req_pend);
    break;
#endif
#if (MMS_KILL_EN & REQ_EN)
    case MMSOP_KILL:		/* kill 				*/
      _mvl_process_kill_conf (req_pend);
    break;
#endif
#if (MMS_GET_PI_EN & REQ_EN)
    case MMSOP_GET_PI_ATTR:	/* get program invocation attribute	*/
      _mvl_process_getpi_conf (req_pend);
    break;
#endif
#if (MMS_GETCL_EN & REQ_EN)
    case MMSOP_GET_CAP_LIST:	/* get capability list	*/
      _mvl_process_getcl_conf (req_pend);
    break;
#endif
    default :
      /* This should never happen. It means we sent a "Request" but we	*/
      /* don't support the "Confirm" for it.				*/
      MVL_LOG_ERR1 ("Error: Confirm received for unsupported service '%s'",
          mms_op_string [event->u.mms.dec_rslt.op]);
      _mvl_conf_reject (req_pend);
    break;
    }

  _mvl_set_req_done (req_pend, req_pend->result);
  }


/************************************************************************/
/************************************************************************/
/*			_mvl_tmpnam					*/
/************************************************************************/
/* For applications that do file activity, we need this function	*/

#if (MMS_FOPEN_EN & REQ_EN) || (MMS_FOPEN_EN & RESP_EN)


#define MAX_TEMP_FILE_NUM 	0xFFFF
#define MAX_TEMP_FILE_TRIES     0xFFFF

ST_CHAR *mvl_tmpfile_path;

ST_RET _mvl_tmpnam (ST_CHAR *dest, ST_CHAR *tmpfile_path)
  {
static ST_INT nextId = 0; 
ST_INT i;
FILE *fp;
ST_CHAR *p;
ST_INT pathLen;

  if (dest == NULL)
    return (SD_FAILURE);

/* If the user did not pass in a path, use the global path */
  if (tmpfile_path == NULL)
    tmpfile_path = mvl_tmpfile_path;

/* Default is to use the current working directory 	*/
  dest[0] = 0;
  p = dest;

/* See if user has path they want to use */
  if (tmpfile_path != NULL)
    {
    pathLen = strlen (tmpfile_path);
    if (pathLen > MVL_MAX_TMPFILE_PATH)
      {
      MVL_LOG_NERR0 ("Error: temp file path too long, using working dir");
      }
    else
      {
      strcpy (dest, tmpfile_path);
      p = dest + pathLen;
      }
    }

  for (i = 0; i < MAX_TEMP_FILE_TRIES; ++i)
    {
    sprintf (p, "TMP%04x.TMP", nextId);
    if (++nextId >= MAX_TEMP_FILE_NUM)
      nextId = 0;

    fp = fopen (dest,"r");		/* See if already present	*/
    if (!fp)				/* nope.			*/
      return (SD_SUCCESS);
    else				/* file is present, try again	*/
      fclose (fp);
    }
  return (SD_FAILURE);
  }


/************************************************************************/
/*			_mvl_getpath					*/
/************************************************************************/

ST_VOID _mvl_getpath (ST_CHAR *file_spec, ST_CHAR *dest)
  {
ST_INT i;

  strcpy (dest, file_spec);
  i = strlen (dest);
  while (i > 0)
    {
    if ((dest[i-1] == '/') ||
        (dest[i-1] == '\\') ||
        (dest[i-1] == ':'))
      {
      dest[i] = 0;
      break;
      }
    --i;
    }
  if (i == 0)
    dest[0] = 0;
  }

#endif /* #if (MMS_FOPEN_EN & REQ_EN) || (MMS_FOPEN_EN & RESP_EN) */

/************************************************************************/
/* These checks make sure that user doesn't try to enable a MMS		*/
/* service that is not supported.					*/
/************************************************************************/
#if (MMS_CANCEL_EN & REQ_EN)
#error Unsupported service must not be enabled.
#endif

#if (MMS_RENAME_EN	!= REQ_RESP_DIS)
#error Unsupported service must not be enabled.
#endif
#if (MMS_MV_DWN_EN	!= REQ_RESP_DIS)	/* VM Upload		*/
#error Unsupported service must not be enabled.
#endif
#if (MMS_INIT_DWN_EN 	!= REQ_RESP_DIS)
#error Unsupported service must not be enabled.
#endif
#if (MMS_DWN_LOAD_EN 	!= REQ_RESP_DIS)
#error Unsupported service must not be enabled.
#endif
#if (MMS_TERM_DWN_EN 	!= REQ_RESP_DIS)
#error Unsupported service must not be enabled.
#endif
#if (MMS_MV_UPL_EN	!= REQ_RESP_DIS)	/* VM Download		*/
#error Unsupported service must not be enabled.
#endif
#if (MMS_INIT_UPL_EN 	!= REQ_RESP_DIS)
#error Unsupported service must not be enabled.
#endif
#if (MMS_UP_LOAD_EN	!= REQ_RESP_DIS)
#error Unsupported service must not be enabled.
#endif
#if (MMS_TERM_UPL_EN 	!= REQ_RESP_DIS)
#error Unsupported service must not be enabled.
#endif
#if (MMS_RDDWN_EN 	!= REQ_RESP_DIS)
#error Unsupported service must not be enabled.
#endif
#if (MMS_RDUPL_EN 	!= REQ_RESP_DIS)
#error Unsupported service must not be enabled.
#endif
#if (MMS_LOAD_DOM_EN 	!= REQ_RESP_DIS)
#error Unsupported service must not be enabled.
#endif
#if (MMS_STR_DOM_EN	!= REQ_RESP_DIS)
#error Unsupported service must not be enabled.
#endif
#if (MMS_DEL_DOM_EN	!= REQ_RESP_DIS)
#error Unsupported service must not be enabled.
#endif

#if (MMS_CRE_PI_EN	!= REQ_RESP_DIS)
#error Unsupported service must not be enabled.
#endif
#if (MMS_DEL_PI_EN	!= REQ_RESP_DIS)
#error Unsupported service must not be enabled.
#endif
#if (MMS_START_EN	!= REQ_RESP_DIS)
#error Unsupported service must not be enabled.
#endif
#if (MMS_STOP_EN	!= REQ_RESP_DIS)
#error Unsupported service must not be enabled.
#endif
#if (MMS_RESUME_EN	!= REQ_RESP_DIS)
#error Unsupported service must not be enabled.
#endif
#if (MMS_RESET_EN	!= REQ_RESP_DIS)
#error Unsupported service must not be enabled.
#endif
#if (MMS_KILL_EN	!= REQ_RESP_DIS)
#error Unsupported service must not be enabled.
#endif
#if (MMS_GET_PI_EN	!= REQ_RESP_DIS)
#error Unsupported service must not be enabled.
#endif

#if (MMS_MV_READ_EN	!= REQ_RESP_DIS)	/* VM Read variable(s)	*/
#error Unsupported service must not be enabled.
#endif
#if (MMS_MV_RDVARS_EN 	!= REQ_RESP_DIS)	/* General VM Rd var's	*/
#error Unsupported service must not be enabled.
#endif
#if (MMS_MV_WRITE_EN	!= REQ_RESP_DIS)	/* VM Write variable(s)	*/
#error Unsupported service must not be enabled.
#endif
#if (MMS_MV_WRVARS_EN 	!= REQ_RESP_DIS)	/* General VM WR var's	*/
#error Unsupported service must not be enabled.
#endif
#if (MMS_DEFVAR_EN	!= REQ_RESP_DIS)
#error Unsupported service must not be enabled.
#endif
#if (MMS_DEFSCAT_EN	!= REQ_RESP_DIS)
#error Unsupported service must not be enabled.
#endif
#if (MMS_GETSCAT_EN	!= REQ_RESP_DIS)
#error Unsupported service must not be enabled.
#endif
#if (MMS_DELVAR_EN	!= REQ_RESP_DIS)
#error Unsupported service must not be enabled.
#endif
#if (MMS_MV_DEFTYPE_EN	!= REQ_RESP_DIS)	/* VM DefineType	*/
#error Unsupported service must not be enabled.
#endif
#if (MMS_DEFTYPE_EN	!= REQ_RESP_DIS)
#error Unsupported service must not be enabled.
#endif
#if (MMS_GETTYPE_EN	!= REQ_RESP_DIS)
#error Unsupported service must not be enabled.
#endif
#if (MMS_DELTYPE_EN	!= REQ_RESP_DIS)
#error Unsupported service must not be enabled.
#endif

#if (MMS_TAKECTRL_EN 	!= REQ_RESP_DIS)
#error Unsupported service must not be enabled.
#endif
#if (MMS_RELCTRL_EN	!= REQ_RESP_DIS)
#error Unsupported service must not be enabled.
#endif
#if (MMS_DEFINE_SEM_EN	!= REQ_RESP_DIS)
#error Unsupported service must not be enabled.
#endif
#if (MMS_DELETE_SEM_EN	!= REQ_RESP_DIS)
#error Unsupported service must not be enabled.
#endif
#if (MMS_REP_SEMSTAT_EN	!= REQ_RESP_DIS)
#error Unsupported service must not be enabled.
#endif
#if (MMS_REP_SEMPOOL_EN	!= REQ_RESP_DIS)
#error Unsupported service must not be enabled.
#endif
#if (MMS_REP_SEMENTRY_EN!= REQ_RESP_DIS)
#error Unsupported service must not be enabled.
#endif

#if (MMS_JWRITE_EN	!= REQ_RESP_DIS)
#error Unsupported service must not be enabled.
#endif
#if (MMS_JCREATE_EN	!= REQ_RESP_DIS)
#error Unsupported service must not be enabled.
#endif
#if (MMS_JDELETE_EN	!= REQ_RESP_DIS)
#error Unsupported service must not be enabled.
#endif
	
#if (MMS_DEFEC_EN	!= REQ_RESP_DIS)
#error Unsupported service must not be enabled.
#endif
#if (MMS_DELEC_EN	!= REQ_RESP_DIS)	
#error Unsupported service must not be enabled.
#endif
#if (MMS_GETECA_EN	!= REQ_RESP_DIS)
#error Unsupported service must not be enabled.
#endif
#if (MMS_REPECS_EN	!= REQ_RESP_DIS)
#error Unsupported service must not be enabled.
#endif
#if (MMS_ALTECM_EN	!= REQ_RESP_DIS)	
#error Unsupported service must not be enabled.
#endif
#if (MMS_TRIGE_EN	!= REQ_RESP_DIS)	
#error Unsupported service must not be enabled.
#endif
#if (MMS_DEFEA_EN	!= REQ_RESP_DIS)	
#error Unsupported service must not be enabled.
#endif
#if (MMS_DELEA_EN	!= REQ_RESP_DIS)	
#error Unsupported service must not be enabled.
#endif
#if (MMS_GETEAA_EN	!= REQ_RESP_DIS)	
#error Unsupported service must not be enabled.
#endif
#if (MMS_REPEAS_EN	!= REQ_RESP_DIS)	
#error Unsupported service must not be enabled.
#endif
#if (MMS_DEFEE_EN	!= REQ_RESP_DIS)	
#error Unsupported service must not be enabled.
#endif
#if (MMS_DELEE_EN	!= REQ_RESP_DIS)	
#error Unsupported service must not be enabled.
#endif
#if (MMS_GETEEA_EN	!= REQ_RESP_DIS)	
#error Unsupported service must not be enabled.
#endif
#if (MMS_REPEES_EN	!= REQ_RESP_DIS)	
#error Unsupported service must not be enabled.
#endif
#if (MMS_ALTEE_EN	!= REQ_RESP_DIS)	
#error Unsupported service must not be enabled.
#endif
#if (MMS_EVNOT_EN	!= REQ_RESP_DIS)
#error Unsupported service must not be enabled.
#endif
#if (MMS_ACKEVNOT_EN 	!= REQ_RESP_DIS)
#error Unsupported service must not be enabled.
#endif
#if (MMS_GETAS_EN	!= REQ_RESP_DIS)
#error Unsupported service must not be enabled.
#endif
#if (MMS_GETAES_EN	!= REQ_RESP_DIS)
#error Unsupported service must not be enabled.
#endif

#if (MMS_INPUT_EN	!= REQ_RESP_DIS)
#error Unsupported service must not be enabled.
#endif
#if (MMS_OUTPUT_EN	!= REQ_RESP_DIS)
#error Unsupported service must not be enabled.
#endif
				     
#if (MMS_MV_FOPEN_EN	!= REQ_RESP_DIS)	/* VM FileOpen		*/
#error Unsupported service must not be enabled.
#endif
#if (MMS_MV_FREAD_EN	!= REQ_RESP_DIS)	/* VM FileRead		*/
#error Unsupported service must not be enabled.
#endif
#if (MMS_MV_FCLOSE_EN	!= REQ_RESP_DIS)	/* VM FileClose		*/
#error Unsupported service must not be enabled.
#endif
#if (MMS_MV_FCOPY_EN	!= REQ_RESP_DIS)	/* VM FileCopy		*/
#error Unsupported service must not be enabled.
#endif
