/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	1994 - 2005, All Rights Reserved		        */
/*									*/
/* MODULE NAME : mvl_serv.c    						*/
/* PRODUCT(S)  : MMSEASE-LITE						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*	NONE								*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 07/19/10  MDE           Persist reject info for u_mvl_reject_ind call*/
/* 04/14/10  JRB	   Fix memory leak when MMSCNCLERR received.	*/
/* 04/13/10  JRB	   Decrement numpend_req ONLY in _mvl_set_req_done*/
/*			   & only if decr_numpend_req flag is set.	*/
/* 04/12/10  JRB	   mvl_reject_rcvd: free event if req not found.*/
/* 04/09/10  JRB	   Fix memory leak when decode of MMSRESP fails.*/
/*			   Del _mvl_send_reject_net_info (now unused).	*/
/* 03/03/10  JRB	   Replace "ml_log_*" with "mlogl_info" struct.	*/
/* 03/02/10  JRB	   Del synchronous client functions.		*/
/* 02/24/10  JRB	   Del _mms_dec_info, decoder sets rslt->data_ptr*/
/*			   Log Invoke ID on reject.			*/
/* 02/03/10  NAV     64    OpenVMS port					*/
/* 06/24/08  MDE     63    Added _mvl_persist_aa_objs			*/
/* 05/20/08  JRB     62    Fix handling of MMSCNCLERR (Conclude ERROR).	*/
/* 04/22/08  JRB     61    Move "active" chk inside semaphore lock.	*/
/*			   Del unused MVLULOG_TIMING.			*/
/* 03/05/08  JRB     60    Call _mvl_clear_aa_objs in ..net_req_done	*/
/* 10/04/07  MDE     59    Tweaked LOGCFG_VALUE_GROUP/LOGCFGX_VALUE_MAP	*/
/* 04/09/07  MDE     58    Enhanced filtered logging 			*/
/* 10/30/06  JRB     57	   Del _mvl_curr_net_info, _mvl_curr_usr_ind_ctrl.*/
/* 08/03/07  NAV     56    mvl_msg_rcvd - return after sending reject   */
/* 01/30/06  GLB     55    Integrated porting changes for VMS           */
/* 09/08/05  JRB     54    _mvl_send_reject: set rej_info->pdu_type,	*/
/*			   send right class/code on request decode err.	*/
/*			   Remove duplicate log msg.			*/
/* 07/13/05  MDE     52    Improved reject handlng, add u_mvl_reject_ind*/
/* 08/08/05  JRB     52    Init mvl_debug_sel = MVLLOG_ERR.     	*/
/* 06/02/05  JRB     51    Fix _mvl_send_reject for INVALID-PDU.	*/
/* 05/24/05  EJV     50    Add mvlLogMaskMapCtrl for parsing logcfg.xml	*/
/* 09/07/04  JRB     49    _mvl_get_req_ctrl: ret NULL if net_info==NULL*/
/* 07/28/03  MDE     48    Yet another tweak to mvl_ureq_bufs_avail	*/
/* 05/05/03  MDE     47    Yet another tweak to mvl_ureq_bufs_avail	*/
/* 05/01/03  MDE     46    Added high water logging to (u)req_bufs_avail*/
/* 04/28/03  JRB     45    Fix computations in mvl_(u)req_bufs_avail.	*/
/* 07/09/02  JRB     44    Add mvl_(u)req_bufs_avail functions.		*/
/* 07/09/02  MDE     43    Add maxpend_ind support			*/
/* 05/21/02  MDE     42    Check PDU len before decoding		*/
/* 05/13/02  MDE     41    Added 'skip' arg to _mvl_set_net_req_done 	*/
/* 05/01/02  MDE     40    Fixed conclude logging			*/
/* 04/05/02  MDE     39    Added MLOG logging				*/
/* 03/25/02  MDE     38	   Moved error/reject elements outside union	*/
/* 11/07/01  JRB     37    mvl_wait_req_done return "req->result".	*/
/* 09/21/01  JRB     36    Alloc global bufs only once at startup.	*/
/* 07/31/01  MDE     35    Added mvl_conn_filtered_logging		*/
/* 02/22/01  GLB     34    Added: MVLULOG_DEBUG, MVLULOG_TIMING         */
/* 11/20/00  MDE     33    Fixed lockup - abort with requests out	*/
/* 10/25/00  JRB     32    Del global link list of MVL_REQ_PEND. Add one*/
/*			   for each conn. Save list ptr in MVL_NET_INFO.*/
/*			   Del _mvl_free_req_pend function.		*/
/* 10/25/00  JRB     31    Del ind & conf funct ptrs. Call directly.	*/
/*			   Mv mvl_ind_rcvd, mvl_conf_rcvd to mvlop_en.c	*/
/*			   so "conditional compilation" can be used.	*/
/* 05/15/00  MDE     30    Added statistics				*/
/* 05/12/00  RKR     29    Worked on mvl_send_reject            	*/
/* 04/14/00  JRB     28    Lint cleanup.				*/
/* 03/08/00  JRB     27    Del #ifdef MVL_REQ_BUF_ENABLED: not used.	*/
/*			   mvl_free_req_ctrl chk result before freeing.	*/
/* 01/21/00  MDE     26    Now use MEM_SMEM for dynamic memory		*/
/* 11/24/99  MDE     25    Del indCtrl from link list for info reports	*/
/* 11/19/99  NAV     24	   Add #ifdef MVL_REQ_BUF_ENABLED		*/
/* 10/20/99  NAV     23    Additions for maxpend_req support		*/
/* 11/03/99  RKR     22    Changed reject and error confirm results	*/
/* 11/02/99  RKR     21    Fixed a leak when a reject is received	*/
/* 10/28/99  RKR     20    Added mvl_clr_ind_que 			*/
/* 10/06/99  GLB     19    Added changes to accomodate file rename      */
/* 09/13/99  MDE     18    Added SD_CONST modifiers			*/
/* 04/01/99  MDE     17    Changes to decode buffer allocation scheme   */
/* 03/03/99  MDE     16    Changed 'mvl_max_req_pend' to 10000		*/
/* 12/08/98  MDE     15    Cleanup on request control handling		*/
/* 11/16/98  MDE     14    Renamed internal functions (prefix '_')	*/
/* 11/16/98  MDE     13    Cleanup, conclude changes, removed simple	*/
/*			   request control, renamed concat_filename	*/
/* 10/08/98  MDE     12    Migrated to updated SLOG interface		*/
/* 10/01/98  DWL     11    Added get domain attributes client		*/
/*			   Added get name list client			*/
/*			   Added get status client			*/
/* 09/21/98  MDE     10    Minor lint cleanup				*/
/* 06/15/98  MDE     09    Changes to allow compile under C++		*/
/* 05/13/98  JRB     08    Set req->result=CNF_REJ_ERR in mvl_reject_rcvd*/
/*			   Add _mplas_err_resp & _mvl_concat_filename.	*/
/* 04/21/98  MDE     07    Added service logging			*/
/* 04/15/98  JRB     06    mvl_conf_rcvd: do all common processing,	*/
/*			     pass req_pend to all "ml_*_conf_proc".	*/
/*			   Add client support for more services.	*/
/*			   Free comm_event, etc. from mvl_free_req_ctrl */
/*			   Init "result" in _mvl_get_req_ctrl.		*/
/*			   Add _mvl_conf_reject, _mvl_ind_reject.		*/
/* 02/10/98  MDE     05    Added get domain attributes server		*/
/* 12/22/97  MDE     04    Changed 'mvl_comm_serve' to report activity	*/
/* 12/12/97  MDE     03    _mvl_send_reject now returns ST_VOID		*/
/* 12/10/97  MDE     02    Added JREAD/JINIT				*/
/* 09/10/97  MDE     01    MMS-LITE V4.0 Release			*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "glbsem.h"
#include "mmsdefs.h"
#include "mms_err.h"
#include "mvl_defs.h"
#include "mvl_log.h"
#include "mloguser.h"

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/
/************************************************************************/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

ST_UINT mvl_debug_sel = MVLLOG_ERR;
#ifdef DEBUG_SISCO
SD_CONST ST_CHAR *SD_CONST _mvllog_err_logstr = "MVLLOG_ERR";
SD_CONST ST_CHAR *SD_CONST _mvllog_nerr_logstr = "MVLLOG_NERR";
SD_CONST ST_CHAR *SD_CONST _mvllog_acse_logstr = "MVLLOG_ACSE";
SD_CONST ST_CHAR *SD_CONST _mvllog_acsedata_logstr = "MVLLOG_ACSEDATA";
SD_CONST ST_CHAR *SD_CONST _mvlulog_flow_logstr = "MVLULOG_FLOW";
SD_CONST ST_CHAR *SD_CONST _mvlulog_debug_logstr = "MVLULOG_DEBUG";

LOGCFGX_VALUE_MAP mvlLogMaskMaps[] =
  {
    {"MVLLOG_ERR",	MVLLOG_ERR,	  &mvl_debug_sel, _LOGCFG_DATATYPE_UINT_MASK, NULL, "Error"},
    {"MVLLOG_NERR",	MVLLOG_NERR,	  &mvl_debug_sel, _LOGCFG_DATATYPE_UINT_MASK, NULL, "Notice"},
    {"MVLLOG_ACSE",	MVLLOG_ACSE,	  &mvl_debug_sel, _LOGCFG_DATATYPE_UINT_MASK, NULL, "ACSE"},
    {"MVLLOG_ACSEDATA",	MVLLOG_ACSEDATA,  &mvl_debug_sel, _LOGCFG_DATATYPE_UINT_MASK, NULL, "ACSEData"},
    {"MVLULOG_FLOW",	MVLULOG_FLOW,	  &mvl_debug_sel, _LOGCFG_DATATYPE_UINT_MASK, NULL, "Flow"},
    {"MVLULOG_DEBUG",	MVLULOG_DEBUG,	  &mvl_debug_sel, _LOGCFG_DATATYPE_UINT_MASK, NULL, "Debug"}
  };

LOGCFG_VALUE_GROUP mvlLogMaskMapCtrl =
  {
  {NULL,NULL},
  "MvlLogMasks",	  /* Parent Tag */
  sizeof(mvlLogMaskMaps)/sizeof(LOGCFGX_VALUE_MAP),
  mvlLogMaskMaps
  };
#endif /* DEBUG_SISCO */

ST_BOOLEAN mvl_conn_filtered_logging;

ST_BOOLEAN _mvl_persist_aa_objs;

/************************************************************************/
/* These functions are used internally by mvl_comm_serve ()		*/

static ST_VOID mvl_msg_rcvd (MVL_COMM_EVENT *event);
static ST_VOID mvl_error_rcvd (MVL_COMM_EVENT *event);
static ST_VOID mvl_reject_rcvd (MVL_COMM_EVENT *event);

#ifndef WIN32//MEMO:  [2016-11-14 12:03 ÉÛ¿­Ìï]
ST_VOID u_mvl_reject_ind (MVL_NET_INFO *net_info, REJECT_RESP_INFO *rej_info)
{
	if (rej_info->rej_class == MMS_REJ_CLASS_CONFIRMED_REQUEST_PDU &&
		rej_info->rej_code  == MMS_REJ_CODE_CONFIRMED_REQUEST_PDU_UNRECOGNIZED_SERVICE)
	{	/* This is most common case. Provide extra information in log msgs.*/
		if (rej_info->detected_here)
			MVL_LOG_ERR0 ("MMS Reject sent, unrecognized service. Check MMS services enabled here.");
		else
			MVL_LOG_ERR0 ("MMS Reject received, unrecognized service. Check MMS services enabled on remote device.");
	}
	else
	{
		/* NOTE: for all other cases, it is best to abort the connection	*/
		/* but it is not done in this callback function because no good	*/
		/* way to inform main application that abort occurred.		*/
		/* DEBUG: should there be extra handshaking in MVL_NET_INFO struct?	*/
		if (rej_info->detected_here)
			MVL_LOG_ERR0 ("Reject sent.");
		else
			MVL_LOG_ERR0 ("Reject received.");
	}
}
#endif

/************************************************************************/
/************************************************************************/
/*			mvl_comm_serve					*/
/* General MMS communication service					*/
/************************************************************************/

ST_BOOLEAN _mvl_comm_serve_active;

ST_BOOLEAN mvl_comm_serve ()
  {
MVL_COMM_EVENT *event;
ST_BOOLEAN activityFlag;

  S_LOCK_COMMON_RESOURCES ();

  /* Check for re-entrancy. This must be inside the lock so only one	*/
  /* thread is accessing the global "_mvl_comm_serve_active".		*/
  if (_mvl_comm_serve_active)
    {
    MVL_LOG_ERR0 ("Error: mvl_comm_serve re-entered");
    S_UNLOCK_COMMON_RESOURCES ();	/* unlock sem before return	*/
    return (SD_FALSE);
    }
  _mvl_comm_serve_active = SD_TRUE;

/* Call the network service routine, see if it has anything for us	*/
/* to deal with								*/
  activityFlag = _mvl_net_service (&event);
  if (event)
    {
    switch (event->event_type)
      {
      case MMS_MSG_RCVD :
        mvl_msg_rcvd (event);
      break;

      default :
        _mvl_free_comm_event (event);	/* All done with this event	*/
      break;
      }
    }
  _mvl_comm_serve_active = SD_FALSE;
  S_UNLOCK_COMMON_RESOURCES ();
  return (activityFlag);
  }

/************************************************************************/
/*			mvl_msg_rcvd					*/
/* This function is called by mvl_comm_serve when a MMS message	        */
/* has been received.							*/
/************************************************************************/

static ST_VOID mvl_msg_rcvd (MVL_COMM_EVENT *event)
  {
MMSDEC_INFO *rslt;
MVL_REQ_PEND *req_pend;
ST_INT pdu_len;

  rslt = &event->u.mms.dec_rslt;
  pdu_len = event->u.mms.pdu_len;

/* Verify that the message is less than or equal to negiotiated		*/
  if (pdu_len > event->net_info->max_pdu_size)
    {
    MVL_LOG_NERR2 ("MMS PDU too long: %d rxd, %d max", 
    				pdu_len, event->net_info->max_pdu_size);
    _mvl_send_reject (event);
    return;
    }

/* some kind of message was received, need to decode and act on it	*/
  _ms_mms_decode (event->u.mms.pdu, pdu_len, rslt);
/* NOTE: _ms_mms_decode sets rslt->data_ptr & other rslt members.	*/

  if (rslt->err_code == NO_DECODE_ERR)
    {
    switch (rslt->type)
      {
      case MMSREQ :
      /* We can log here if so desired */
        if ((mms_debug_sel & MMS_LOG_IND) && mlogl_info.ind_info_fun != NULL)
	  {
          if (mvl_conn_filtered_logging == SD_FALSE || event->net_info->log_enable)
            (*mlogl_info.ind_info_fun) (rslt->id, rslt->op, rslt->data_ptr);
	  }
        mvl_ind_rcvd (event);
      break;

      case MMSUNREQ :
      /* We can log here if so desired */
        if ((mms_debug_sel & MMS_LOG_IND) && mlogl_info.unsol_ind_info_fun != NULL)
	  {
          if (mvl_conn_filtered_logging == SD_FALSE || event->net_info->log_enable)
            (*mlogl_info.unsol_ind_info_fun) (rslt->op, rslt->data_ptr);
	  }
        mvl_ind_rcvd (event);
      break;
    
      case MMSRESP :
      /* We can log here if so desired */
        if ((mms_debug_sel & MMS_LOG_CONF) && mlogl_info.conf_info_fun != NULL)
	  {
          if (mvl_conn_filtered_logging == SD_FALSE || event->net_info->log_enable)
            (*mlogl_info.conf_info_fun) (rslt->id, rslt->op, rslt->data_ptr);
	  }
        mvl_conf_rcvd (event);
      break;

      
      case MMSREJECT :
      /* We can log here if so desired */
        if ((mms_debug_sel & MMS_LOG_CONF) && mlogl_info.reject_recv_fun != NULL)
	  {
          if (mvl_conn_filtered_logging == SD_FALSE || event->net_info->log_enable)
            (*mlogl_info.reject_recv_fun) (rslt->data_ptr);
	  }
        mvl_reject_rcvd (event);
      break;

      case MMSERROR : 
        if ((mms_debug_sel & MMS_LOG_CONF) && mlogl_info.error_conf_fun != NULL)
	  {
          if (mvl_conn_filtered_logging == SD_FALSE || event->net_info->log_enable)
            (*mlogl_info.error_conf_fun) (rslt->id, rslt->data_ptr);
	  }
        mvl_error_rcvd (event);
      break;

      case MMSCNCLERR :		/* MMS Conclude Error PDU	*/
        if ((mms_debug_sel & MMS_LOG_CONF) && mlogl_info.error_conf_fun != NULL)
	  {
          if (mvl_conn_filtered_logging == SD_FALSE || event->net_info->log_enable)
            (*mlogl_info.error_conf_fun) (rslt->id, rslt->data_ptr);
	  }
        /* Find Conclude request sent on this connection & mark it as failed.*/
        req_pend = _mvl_find_req_ctrl (event->net_info,
                                       MMSOP_CONCLUDE, (ST_UINT32) -1);
        if (!req_pend)
          {	/* this should almost never fail.	*/
          MVL_LOG_ERR0 ("Negative Conclude Confirm received: Error finding request control");
          }
        else
          {
          req_pend->done = SD_TRUE;
          req_pend->result = SD_FAILURE;
          }
        _mvl_free_comm_event (event);	/* All done with this event	*/
      break;

      case MMSCNCLRESP :	/* MMS Conclude Response PDU	*/
      /* We can log here if so desired */
        if ((mms_debug_sel & MMS_LOG_CONF) && mlogl_info.conf_info_fun != NULL)
	  {
          if (mvl_conn_filtered_logging == SD_FALSE || event->net_info->log_enable)
            (*mlogl_info.conf_info_fun) (rslt->id, MMSOP_CONCLUDE, NULL);
          } 
        req_pend = _mvl_find_req_ctrl (event->net_info,
                                       MMSOP_CONCLUDE, (ST_UINT32) -1);
        if (!req_pend)
          {
          MVL_LOG_NERR1 ("%s Confirmation : mvl_find_req_ctrl failed",
              mms_op_string [event->u.mms.dec_rslt.op]);
          _mvl_free_comm_event (event);	/* All done with this event	*/
          return;
          }
        req_pend->event = event;
        mvl_concl_conf_rcvd (req_pend);
      break;

      case MMSCNCLREQ :		/* MMS Conclude Request PDU	*/
      /* We can log here if so desired */
        if ((mms_debug_sel & MMS_LOG_IND) && mlogl_info.ind_info_fun != NULL)
	  {
          if (mvl_conn_filtered_logging == SD_FALSE || event->net_info->log_enable)
            (*mlogl_info.ind_info_fun) (rslt->id, MMSOP_CONCLUDE, NULL);
	  }
        mvl_concl_ind_rcvd (event);
      break;
      
/* Unexpected PDU types */
      case MMSCANREQ :
      /* We can log here if so desired */
        if ((mms_debug_sel & MMS_LOG_IND) && mlogl_info.ind_info_fun != NULL)
          (*mlogl_info.ind_info_fun) (rslt->id, rslt->op, rslt->data_ptr);
        mvl_cancel_ind_rcvd (event);
      break;

      case MMSCANRESP	:
        MVL_LOG_NERR0 ("Unexpected MMS PDU type received: Cancel Response");
        _mvl_send_reject (event);
      break;

      case MMSCANERR :
        MVL_LOG_NERR0 ("Unexpected MMS PDU type received: Cancel Error");
        _mvl_send_reject (event);
      break;

      case MMSINITREQ	:
        MVL_LOG_NERR0 ("Unexpected MMS PDU type received: Initiate Request");
        _mvl_send_reject (event);
      break;

      case MMSINITRESP :
        MVL_LOG_NERR0 ("Unexpected MMS PDU type received: Initiate Response");
        _mvl_send_reject (event);
      break;

      case MMSINITERR	:
        MVL_LOG_NERR0 ("Unexpected MMS PDU type received: Initiate Error");
        _mvl_send_reject (event);
      break;

      case UNKNOWNTYPE :
        MVL_LOG_NERR0 ("Unexpected MMS PDU type received: Unknown");
        _mvl_send_reject (event);
      break;

      default :
        MVL_LOG_NERR1 ("Unexpected MMS PDU type received: %d", rslt->type);
        _mvl_send_reject (event);
      break;
      }   
    }
  else  /* Error in decoding, need to reject the message		*/
    {
    MVL_LOG_NERR0 ("MMS PDU decode error, sending reject");

  /* If this was a confirmed response, try to match the request */
  /* CRITICAL: do this BEFORE calling _mvl_send_reject because it frees event.*/
    if (rslt->dec_level >= 2 && rslt->type == MMSRESP)
      {
      req_pend = _mvl_find_req_ctrl (event->net_info, -1, rslt->id);
      if (req_pend != NULL)
        {
        _mvl_set_req_done (req_pend, MVL_ERR_CNF_REJ_ERR);
        }
      else
        {
        MVL_LOG_CNERR0 ("Could not match confirmed request");
        }
      }
    _mvl_send_reject (event);
    }
  }

/************************************************************************/
ST_VOID mvl_clr_ind_que (MVL_NET_INFO *net_info)
  {
MVL_IND_PEND	 *pend_ind;
MVL_IND_PEND	 *next_ind;

/* look at all the pending indications attached to this MVL_NET_INFO	*/
/* unlink them, and deallocate them.					*/
  
  pend_ind = net_info->pend_ind;
  while(pend_ind!=NULL)
    {
    next_ind = (MVL_IND_PEND *) list_get_next(net_info->pend_ind,pend_ind);
    list_unlink(&(net_info->pend_ind),pend_ind);
    --net_info->numpend_ind;

    _mvl_free_comm_event (pend_ind->event);

    M_FREE (MSMEM_IND_CTRL, pend_ind);
    pend_ind = next_ind;
    }
  }

/************************************************************************/
/*			mvl_error_rcvd					*/
/************************************************************************/

static ST_VOID mvl_error_rcvd (MVL_COMM_EVENT *event)
  {
MVL_REQ_PEND *req;
ST_UINT32 id;
ST_INT op;

  ++mvl_mms_statistics.clientReqErr;

  id = event->u.mms.dec_rslt.id;
  op = event->u.mms.dec_rslt.op;

  req = _mvl_find_req_ctrl (event->net_info, -1, id);
  if (!req)
    {
    MVL_LOG_NERR0 ("Error Received : mvl_find_req_ctrl failed");
    _mvl_free_comm_event (event);	/* All done with this event	*/
    return;
    }
  req->event = event;
  req->error_info = (ERR_INFO *) req->event->u.mms.dec_rslt.data_ptr;
   _mvl_set_req_done (req, MVL_ERR_CNF_ERR_OK);
  }

/************************************************************************/
/*			mvl_reject_rcvd					*/
/************************************************************************/

static ST_VOID mvl_reject_rcvd (MVL_COMM_EVENT *event)
  {
REJECT_RESP_INFO *info;
MVL_REQ_PEND *req;
ST_BOOLEAN freeCommEvent;
MVL_NET_INFO *netInfo;
REJECT_RESP_INFO rej_info_copy;

  ++mvl_mms_statistics.rejectsRcvd;
  netInfo = event->net_info;
  freeCommEvent = SD_TRUE;

  info = (REJECT_RESP_INFO *) event->u.mms.dec_rslt.data_ptr;

/* Save a copy to pass to the reject indication function */
  rej_info_copy = *info;
  
  MVL_LOG_NERR2 ("Reject Received: class %d, code %d", 
  					info->rej_class, info->rej_code);

/* If the other node rejected our confirmed request, need to match it	*/
  if (info->rej_class == MMS_REJ_CLASS_CONFIRMED_REQUEST_PDU)
    {
    if (info->invoke_known)
      {
      MVL_LOG_CNERR1 ("Confirmed Request Invoke ID = %ld", (ST_LONG)info->invoke);
      req = _mvl_find_req_ctrl (netInfo, -1, info->invoke);
      if (!req)
        {
        MVL_LOG_CNERR0 ("Could not match confirmed request");
        /* CRITICAL: Continue so user function called & event freed.	*/
        }
      else
        {
        req->event = event; 
        req->reject_info = 
                 (REJECT_RESP_INFO *) req->event->u.mms.dec_rslt.data_ptr;
        _mvl_set_req_done (req, MVL_ERR_CNF_REJ_ERR);
        freeCommEvent = SD_FALSE;
        }
      } 
    }

/* Let the user know about this */
  u_mvl_reject_ind (netInfo, &rej_info_copy);

  if (freeCommEvent)
    _mvl_free_comm_event (event);	/* All done with this event	*/
  }

/************************************************************************/
/************************************************************************/
/* This table is used to select reject class given MMS pdu type		*/
/* PDU type is the index into this table    				*/
/************************************************************************/

SD_CONST ST_INT16 pdutype_to_reject_class[14] =
  {
  1,    /* confirmed request						*/
  2,    /* confirmed response						*/
  3,    /* confirmed error						*/
  4,    /* unconfirmed							*/
  5,    /* reject							*/
  6,    /* cancel request						*/
  7,    /* cancel response						*/
  8,    /* cancel error							*/
  0,    /* initiate req   - not used					*/
  0,    /* initiate resp  - not used					*/
  0,    /* initiate error - not used					*/
  9,    /* conclude request						*/
  10,   /* conclude response						*/
  11    /* conclude error						*/
  };

/************************************************************************/
/*			_mvl_ind_reject					*/
/* Send reject for Indication.						*/
/************************************************************************/

ST_VOID _mvl_ind_reject (MVL_IND_PEND *ind_pend)
  {
MVL_NET_INFO *net_info; 

  net_info = ind_pend->event->net_info;
  _mvl_send_reject (ind_pend->event);
  list_unlink(&(net_info->pend_ind),ind_pend);
  --net_info->numpend_ind;

  M_FREE (MSMEM_IND_CTRL, ind_pend);
  }

/************************************************************************/
/*			_mvl_conf_reject					*/
/* Send reject for Confirmation.					*/
/************************************************************************/

ST_VOID _mvl_conf_reject (MVL_REQ_PEND *req_pend)
  {
  _mvl_send_reject (req_pend->event);
  req_pend->event = NULL;	/* so mvl_free_req_ctrl doesn't try to	*/
				/* free it again.			*/
  req_pend->result = SD_FAILURE;/* Let user know it failed.		*/ 
  }

/************************************************************************/
/*			_mvl_send_reject					*/
/* This is a simple minded reject send function. If the PDU type isd	*/
/* known, send a reject of the appropriate class with code == OTHER.	*/
/* This is not always exactly right per the MMS service document, but	*/
/* will be OK for now.							*/
/************************************************************************/

ST_VOID _mvl_send_reject (MVL_COMM_EVENT *event)
  {
ST_RET rc;
REJECT_RESP_INFO rej_info;
MMSDEC_INFO *rslt;

  S_LOCK_COMMON_RESOURCES ();
  rej_info.detected_here = SD_TRUE;
  rej_info.invoke_known = SD_FALSE;
  rslt = &event->u.mms.dec_rslt;
  if (rslt->dec_level > 0)      /* If we at least know the type of PDU	*/
    {
    rej_info.pdu_type = rslt->type;
    /* If this was unrecognized confirmed request, set appropriate reject class/code.	*/
    if (rslt->dec_level >= 2 && rslt->type == MMSREQ && rslt->err_code==REQ_UNREC_SERV)
      {
      rej_info.rej_class = MMS_REJ_CLASS_CONFIRMED_REQUEST_PDU;
      rej_info.rej_code  = MMS_REJ_CODE_CONFIRMED_REQUEST_PDU_UNRECOGNIZED_SERVICE;
      }
    else
      {		/* set correct class, but just set code to "OTHER" (0).	*/
      rej_info.rej_class = pdutype_to_reject_class[rslt->type];
      rej_info.rej_code = 0;
      }

    if ((rslt->dec_level >= 2) &&  /* if invoke ID known, use it    */
        (rslt->type == MMSREQ || rslt->type == MMSRESP))
      {						
      rej_info.invoke_known = SD_TRUE;
      rej_info.invoke = rslt->id;
      }
    }
  else
    {
    rej_info.pdu_type = UNKNOWNTYPE;
    rej_info.rej_class = MMS_REJ_CLASS_PDU_ERROR;
    rej_info.rej_code = MMS_REJ_CODE_PDU_ERROR_INVALID_PDU;
    }

  rc = mpl_reject_resp (&rej_info);
  if (rc != SD_SUCCESS)
    {
    MVL_LOG_NERR1 ("Sending Reject : mpl_reject_resp failed, ret = %x", rc);
    }	
  else
    {			    	/* build was OK, send it		*/
    rc = _mvl_send_msg (mmsl_msg_start,mmsl_msg_len, event->net_info);
    if (rc != SD_SUCCESS)
      {
      MVL_LOG_NERR1 ("Sending Reject : _mvl_send_msg failed, ret = %x", rc);
      }
    else
      ++mvl_mms_statistics.rejectsSent;
    }

/* Tell the user all about it ... */
  u_mvl_reject_ind (event->net_info, &rej_info);

  _mvl_free_comm_event (event);	/* All done with this event		*/
  S_UNLOCK_COMMON_RESOURCES ();
  }

/************************************************************************/
/*			_mplas_err_resp					*/
/* This function sends an error response and frees the MVL_IND_PEND	*/
/* and the MVL_COMM_EVENT.						*/
/************************************************************************/

ST_RET _mplas_err_resp (MVL_IND_PEND *indCtrl, ST_INT16 eClass, ST_INT16 code)
  {
ST_RET rc;
MVL_NET_INFO *net_info; 

  net_info = indCtrl->event->net_info;
  rc = _mvl_send_err_f (indCtrl->event, eClass, code);
  list_unlink(&(net_info->pend_ind),indCtrl);
  --net_info->numpend_ind;

  M_FREE (MSMEM_IND_CTRL, indCtrl);
  ++mvl_mms_statistics.serverRespErr;
  return (rc);
  }

/************************************************************************/
/*			_mvl_send_err_f					*/
/* This function sends an error response and frees the MVL_COMM_EVENT.	*/
/************************************************************************/

ST_RET _mvl_send_err_f (MVL_COMM_EVENT *event, ST_INT16 eClass, ST_INT16 code)
  {
ST_RET rc;

  rc = _mvl_send_error (event, eClass, code);
  _mvl_free_comm_event (event);	/* All done with this event		*/
  return (rc);
  }

/************************************************************************/
/*			_mvl_send_error					*/
/* This function is used to send an error response to a confirmed	*/
/* request PDU.								*/
/************************************************************************/

ST_RET _mvl_send_error (MVL_COMM_EVENT *event, ST_INT16 eClass, ST_INT16 code)
  {
ST_RET rc;

  S_LOCK_COMMON_RESOURCES ();
  rc = mpl_err_resp (event->u.mms.dec_rslt.op, 
                     event->u.mms.dec_rslt.id,
                     eClass,code);
  if (rc != SD_SUCCESS)
    {
    MVL_LOG_NERR1 ("Sending Error : mpl_err_resp failed, ret = %x", rc);
    }
  else
    {    			/* build was OK, send it		*/
    rc = _mvl_send_msg (mmsl_msg_start,mmsl_msg_len,event->net_info);
    if (rc != SD_SUCCESS)
      {
      MVL_LOG_NERR1 ("Sending Error : _mvl_send_msg failed, ret = %x", rc);
      }
    else
      ++mvl_mms_statistics.serverRespErr;
    }
  S_UNLOCK_COMMON_RESOURCES ();
  return (rc);
  }

/************************************************************************/
/************************************************************************/
/* Outstanding MMS request control handling functions			*/
/************************************************************************/


/************************************************************************/
/*			_mvl_get_req_ctrl				*/
/************************************************************************/

MVL_REQ_PEND *_mvl_get_req_ctrl (MVL_NET_INFO *net_info, ST_INT16 op)
  {
MVL_REQ_PEND *req;

  if (net_info == NULL)
    {
    MVL_LOG_NERR0 ("_mvl_get_req_ctrl error: connection invalid");
    return (NULL);
    }
  req = (MVL_REQ_PEND *) M_CALLOC (MSMEM_REQ_CTRL, 1, sizeof (MVL_REQ_PEND));
  S_LOCK_COMMON_RESOURCES ();
  list_add_last(&net_info->req_pend_list, req);

  req->invoke_id = mmsl_invoke_id;
  req->op = op;
  req->done = SD_FALSE;
  req->result = SD_SUCCESS;
  req->net_info = net_info;
  req->u_req_done = NULL;
  req->event = NULL;
  S_UNLOCK_COMMON_RESOURCES ();
  return (req);
  }

/************************************************************************/
/*			_mvl_find_req_ctrl 				*/
/************************************************************************/

MVL_REQ_PEND *_mvl_find_req_ctrl (MVL_NET_INFO *net_info, 
				  ST_INT op, ST_UINT32 id)
  {
MVL_REQ_PEND *req;

  S_LOCK_COMMON_RESOURCES ();
  req = net_info->req_pend_list;	/* start at head of linked list	*/
  while (req != NULL)			/* look through whole list	*/
    {
    if (req->net_info == net_info && 
	req->invoke_id == id)
      {
      if (req->op == op || op == -1)
        break;				/* return a pointer to it	*/
      }
    req = (MVL_REQ_PEND *) list_get_next (net_info->req_pend_list, req);
    }

  S_UNLOCK_COMMON_RESOURCES ();
  return (req);
  }

/************************************************************************/
/*			_mvl_set_net_req_done				*/
/************************************************************************/

ST_VOID _mvl_set_net_req_done (MVL_REQ_PEND *skip, 
			       MVL_NET_INFO *net_info, ST_RET result)
  {
MVL_REQ_PEND *req;
MVL_REQ_PEND *next;

  req = net_info->req_pend_list;	/* start at head of linked list	*/
  while (req != NULL)			/* look through whole list	*/
    {
  /* CRITICAL: get NEXT entry BEFORE calling _mvl_set_req_done		*/
  /*           because it may remove THIS entry from the linked list.	*/
    next = (MVL_REQ_PEND *) list_get_next (net_info->req_pend_list, req);
    if (req->done != SD_TRUE)
      {
      if (req != skip)
        _mvl_set_req_done (req, result);
      }
    req = next;
    }

  /* CRITICAL: Destroy all AA_SPEC objects on this connection.		*/
  /*    This is a good place to do this because this function is	*/
  /*    called on Abort.req, Abort.ind, Release.ind, or Release.cnf	*/

  if (!_mvl_persist_aa_objs)
    _mvl_clear_aa_objs (net_info->aa_objs);
  }


/************************************************************************/
/* 			_mvl_set_req_done 				*/
/************************************************************************/

ST_VOID _mvl_set_req_done (MVL_REQ_PEND *req, ST_RET result)
  {
  req->result = result;
  req->done = SD_TRUE;

  /* Decrement pending request count (numpend_req) on this connection,	*/
  /* but only if "req->decr_numpend_req" flag is set indicating count	*/
  /* was incremented when this request was sent.			*/
  if (req->decr_numpend_req)
    {
    if (--req->net_info->numpend_req < 0)
      {		/* this error should NEVER happen	*/
      MVL_LOG_ERR1 ("Pending request count invalid (%d). Resetting it to 0.",
          req->net_info->numpend_req);
      req->net_info->numpend_req = 0;
      }
    }

  if (req->u_req_done)		/* Call the user done function		*/
    (*req->u_req_done)(req);
  }

/************************************************************************/
/*			mvl_free_req_ctrl				*/
/************************************************************************/

ST_VOID mvl_free_req_ctrl (MVL_REQ_PEND *pend_req)
  {
  if (pend_req)
    {
    S_LOCK_COMMON_RESOURCES ();

    if (!pend_req->done)
      {
      /* NOTE: This could happen if the user tries to free the request	*/
      /*   before it completes. Treat it like a timeout.		*/
      /*   This will decrement the pending count.			*/
      _mvl_set_req_done (pend_req, MVL_ERR_USR_TIMEOUT);
      }

    if (pend_req->event)
      _mvl_free_comm_event (pend_req->event);

    /* Free up extra stuff for some services.				*/
    /* NOTE: on error or reject the union (pend_req->u) is overwritten	*/
    /*       so "normal" resp_info is not valid. DON'T try to free it.	*/
    if (pend_req->op == MMSOP_FILE_DIR  &&
        pend_req->result != MVL_ERR_CNF_REJ_ERR  &&	/* !reject	*/
        pend_req->result != MVL_ERR_CNF_ERR_OK  &&	/* !error	*/
        pend_req->u.fdir.resp_info != NULL)
      {	/* Only do this if it was allocated (see confirm function)	*/
      M_FREE (MSMEM_GEN, pend_req->u.fdir.resp_info);
      }
    if (pend_req->op == MMSOP_READ_JOURNAL  &&
        pend_req->result != MVL_ERR_CNF_REJ_ERR  &&	/* !reject	*/
        pend_req->result != MVL_ERR_CNF_ERR_OK  &&	/* !error	*/
        pend_req->u.jread.resp_info != NULL)
      {	/* Only do this if it was allocated (see confirm function)	*/
      M_FREE (MSMEM_GEN, pend_req->u.jread.resp_info);
      }

    list_unlink (&pend_req->net_info->req_pend_list, pend_req);
    M_FREE (MSMEM_REQ_CTRL, pend_req);
    S_UNLOCK_COMMON_RESOURCES ();
    }
  }

/************************************************************************/
/************************************************************************/
/*			   mvl_init_glb_vars				*/
/************************************************************************/
/* For use when global variables cannot be initialized at compile time	*/

#if defined(NO_GLB_VAR_INIT)

ST_VOID mvl_init_glb_vars (ST_VOID)
  {
  m_init_glb_vars ();
  _mvlop_en_init ()
  }
#endif

/************************************************************************/
/*			_mvl_concat_filename					*/
/************************************************************************/
ST_RET _mvl_concat_filename (ST_CHAR *good_name, 
	ST_INT num_of_fname,
	FILE_NAME *fname_arr,
	ST_INT max_len)	/* size of array to fill in (i.e. good_name)	*/
  {
ST_INT j;
ST_INT total_len;

  for (j = 0, total_len = 0;  j < num_of_fname;  j++)
    total_len += fname_arr[j].fn_len;

  if (total_len + 1 > max_len)	/* Must leave room for NULL	*/
    return (SD_FAILURE);
  
  for (j = 0, total_len = 0;  j < num_of_fname;  j++)
    {
    memcpy (&good_name [total_len], fname_arr[j].fname, fname_arr[j].fn_len);
    total_len += fname_arr[j].fn_len;
    }
  good_name [total_len] = '\x00';		/* add NULL terminator	*/
  return (SD_SUCCESS);
  }

/************************************************************************/
/************************************************************************/
/************************************************************************/
/*			_mvl_calloc_os 					*/
/************************************************************************/

ST_VOID *_mvl_calloc_os (ST_UINT num, ST_UINT size)
  {
  return (M_CALLOC (MSMEM_DEC_OS_INFO, 1, size));
  }

/************************************************************************/
/*			_mvl_realloc_os 				*/
/************************************************************************/

ST_VOID *_mvl_realloc_os (ST_VOID *old, ST_UINT new_size)
  {
  return (M_REALLOC (MSMEM_DEC_OS_INFO, old,new_size));
  }

/************************************************************************/
/*			_mvl_free_os 					*/
/************************************************************************/

ST_VOID  _mvl_free_os (ST_VOID *buf)
  {
  M_FREE (MSMEM_DEC_OS_INFO, buf);
  }

/************************************************************************/
/*			mvl_req_bufs_avail				*/
/* Return number of buffers available for sending "confirmed" requests.	*/
/************************************************************************/

ST_INT mvlLowWaterAvail = 50;
ST_INT mvlLowWaterReqAvail = 50;
ST_INT mvlLowWaterUReqAvail = 50;

ST_INT mvl_req_bufs_avail (MVL_NET_INFO *net_info)
  {
ST_INT avail;

  avail = a_buffers_avail (net_info->acse_conn_id);
  if (avail < mvlLowWaterAvail)
    {
    mvlLowWaterAvail = avail;
    MVLU_LOG_DEBUG1 ("Low water 'buffers available': %d", mvlLowWaterAvail);
    }

  /* Must have enough buffers avail to respond to max num of indications.*/
  /* Doesn't matter how many indications are currently pending because	*/
  /* at any momemt could get a bunch of indications and must ALWAYS	*/
  /* have buffers available to respond to the maximum.			*/

  avail -= net_info->maxpend_ind;

  if (avail < mvlLowWaterReqAvail)
    {
    mvlLowWaterReqAvail = avail;
    MVLU_LOG_DEBUG1 ("Low water 'Req buffers available': %d", mvlLowWaterReqAvail);
    }
  return (avail);
  }

/************************************************************************/
/*			mvl_ureq_bufs_avail				*/
/* Return number of buffers available for sending "unconfirmed" reqs	*/
/************************************************************************/

ST_INT mvl_ureq_bufs_avail (MVL_NET_INFO *net_info)
  {
ST_INT avail;

  avail = a_buffers_avail (net_info->acse_conn_id);
  if (avail < mvlLowWaterAvail)
    {
    mvlLowWaterAvail = avail;
    MVLU_LOG_DEBUG1 ("Low water 'buffers available': %d", mvlLowWaterAvail);
    }

  /* Must have enough buffers avail to respond to max num of indications.*/
  /* Doesn't matter how many indications are currently pending because	*/
  /* at any momemt could get a bunch of indications and must ALWAYS	*/
  /* have buffers available to respond to the maximum.			*/

  /* Because we don't know when response buffers are release by SUIC, 	*/
  /* we always assume that we must be able to send the max resp at any	*/
  /* time. This means that when there really ARE max resp pend in SUIC	*/
  /* we reserve twice as many as we need ... 				*/

  avail -= (net_info->maxpend_ind * 2);	/* reserve maximum		*/

  /* Must have enough buffers avail to send max num of "confirmed" reqs.*/
  /* Doesn't matter how many requests are currently pending because	*/
  /* at any momemt could get a bunch of confirms and may want to send	*/
  /* new requests. So, we need to have buffers reserved for the max. 	*/
  avail -= (net_info->maxpend_req);

  if (avail < mvlLowWaterUReqAvail)
    {
    mvlLowWaterUReqAvail = avail;
    MVLU_LOG_DEBUG1 ("Low water 'UReq buffers' available': %d", mvlLowWaterUReqAvail);
    }
  return (avail);
  }

