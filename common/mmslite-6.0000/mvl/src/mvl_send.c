/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	1994 - 2002, All Rights Reserved		        */
/*									*/
/* MODULE NAME : mvl_send.c    						*/
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
/* 04/13/10  JRB	   _mvl_send_req: set new flag decr_numpend_req.*/
/* 01/20/10  JRB     18    Fix log messages.				*/
/* 09/14/06  MDE     17    Log PDU when too large			*/
/* 05/01/03  MDE     16    Added buffer depletion logging _mvl_send_req	*/
/* 10/29/02  JRB     15    _mvl_send_req: on err, set done & result in	*/
/*			   req_pend. Helps if user doesn't chk return.	*/
/* 07/10/02  JRB     14    _mvl_send_req: call mvl_req_bufs_avail.	*/
/* 07/09/02  MDE     13    Add maxpend_ind support			*/
/* 01/30/02  JRB     12    Del use of stack_sel.			*/
/* 09/21/01  JRB     11    Alloc global bufs only once at startup.	*/
/* 05/15/00  MDE     10    Added MMS statistics				*/
/* 03/08/00  JRB     09    _mvl_send_req: chk conn_active,		*/
/*			   chk & set numpend_req, set invoke_id.	*/
/* 01/21/00  MDE     08    Now use MEM_SMEM for dynamic memory		*/
/* 10/28/99  RKR     07    Modified _mvl_send_resp_i 			*/
/* 09/13/99  MDE     06    Added SD_CONST modifiers			*/
/* 12/09/98  JRB     05    Don't exceed negotiated PDU size.		*/
/*			   Change all NERR log macros to ERR.		*/
/*			   Free enc buf before _mvl_send_err_f.		*/
/* 11/16/98  MDE     04    Renamed internal functions (prefix '_')	*/
/* 11/16/98  MDE     03    Don't free request control if send error	*/
/* 05/14/98  JRB     02    Add _mvl_send_resp_i function.		*/
/* 09/10/97  MDE     01    MMS-LITE V4.0 Release			*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "mmsdefs.h"
#include "mms_pvar.h"
#include "mms_vvar.h"
#include "mvl_defs.h"
#include "mvl_log.h"


/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/
/************************************************************************/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/
/*			_mvl_send_resp_i					*/
/* This function sends a response and frees the MVL_IND_PEND.		*/
/************************************************************************/

ST_VOID _mvl_send_resp_i (MVL_IND_PEND *indCtrl, ST_RET build_rc)
  {
MVL_NET_INFO *net_info; 

  net_info = indCtrl->event->net_info;
  _mvl_send_resp (indCtrl->event, build_rc);
  list_unlink(&(net_info->pend_ind),indCtrl);
  --net_info->numpend_ind;
  M_FREE (MSMEM_IND_CTRL, indCtrl);
  }

/************************************************************************/
/*			_mvl_send_resp					*/
/************************************************************************/

ST_VOID _mvl_send_resp (MVL_COMM_EVENT *event, ST_RET build_rc)
  {
ST_RET rc;

  if (build_rc != SD_SUCCESS)
    {	/* Free encode buf before "_mvl_send_err_f" because it allocs new buf.*/
    MVL_LOG_ERR1 ("Response build error: rc = 0x%X", build_rc);
    _mvl_send_err_f (event,7,2);
    ++mvl_mms_statistics.serverRespErr;
    }
  else if (mmsl_msg_len > event->net_info->max_pdu_size)
    {	/* Free encode buf before "_mvl_send_err_f" because it allocs new buf.*/
    MVL_LOG_ERR0 ("Response build error: exceeds negotiated PDU size");
    _mvl_send_err_f (event,4,3);	/* Don't exceed negotiated PDU size.	*/
    ++mvl_mms_statistics.serverRespErr;
    }
  else    
    {
    rc = _mvl_send_msg (mmsl_msg_start,mmsl_msg_len,event->net_info);
    if (rc != SD_SUCCESS)
      {
      MVL_LOG_ERR1 ("_mvl_send_msg error, rc = %x", rc);
      }
    else
      ++mvl_mms_statistics.serverRespOk;

    _mvl_free_comm_event (event);	/* done with this event	*/
    }
  }




/************************************************************************/
/*			_mvl_send_req					*/
/************************************************************************/

ST_RET _mvl_send_req (MVL_NET_INFO *net_info, 
		     MVL_REQ_PEND *req_pend, ST_RET build_rc)
  {
ST_RET rc;
ST_INT bufsAvail;

  if (build_rc != SD_SUCCESS)
    {
    MVL_LOG_ERR1 ("Request build error, rc = %x", build_rc);
    rc = build_rc;
    }
  else if (net_info->conn_active == SD_FALSE)
    {	/* Check conn_active before using net_info	*/
    MVL_LOG_ERR0 ("Request send error: disconnected");
    rc = MVL_ERR_CNF_DISCONNECTED;
    }
  else if (mmsl_msg_len > net_info->max_pdu_size)
    {
    MVL_LOG_ERR0 ("Request build error: exceeds negotiated PDU size");
    rc = ASN1E_PDU_TOO_BIG;
    }
  else if (net_info->numpend_req >= net_info->maxpend_req)
    {
    MVL_LOG_ERR1 ("Too many requests outstanding, connection=" S_FMT_PTR, net_info);
    rc = MVL_ERR_REQ_PEND_COUNT;
    }
  else if ((bufsAvail = mvl_req_bufs_avail (net_info)) <= 0)
    {
    MVL_LOG_ERR2 ("Not enough ACSE bufs to send confirmed request, avail=%d, connection=" S_FMT_PTR, bufsAvail, net_info);
    rc = MVL_ERR_REQ_PEND_COUNT;
    }
  else
    {
    rc = _mvl_send_msg (mmsl_msg_start,mmsl_msg_len,net_info);
    if (rc != SD_SUCCESS)
      {
      MVL_LOG_ERR1 ("_mvl_send_msg error, rc = %x", rc);
      }
    else
      ++mvl_mms_statistics.clientReq;
    }                                  
  if (rc == SD_SUCCESS)
    {
    /* mmsl_invoke_id already incremented on encode, so subtract one.	*/
    req_pend->invoke_id = mmsl_invoke_id - 1;
    net_info->numpend_req++;
    /* this flag indicates we incremented numpend_req for this req, and	*/
    /* we should decrement it when the confirm is received.		*/
    req_pend->decr_numpend_req = SD_TRUE;
    }
  else
    {
    req_pend->done = SD_TRUE;
    req_pend->result = rc;
    }
  return (rc);
  }                                  

