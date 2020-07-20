/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	1998 - 1998, All Rights Reserved				*/
/*									*/
/* MODULE NAME : c_jread.c						*/
/* PRODUCT(S)  : MMSEASE-LITE						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*	mvla_jread							*/
/*	_mvl_process_jread_conf						*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev     Comments					*/
/* --------  ---  ------   -------------------------------------------	*/
/* 03/02/10  JRB	   Del synchronous client functions.		*/
/* 04/09/07  MDE     15    Enhanced filtered logging 			*/
/* 11/01/01  JRB     14    mvl_wait_req_done now returns ST_RET.	*/
/* 09/21/01  JRB     13    Alloc global bufs only once at startup.	*/
/* 03/07/01  JRB     12    Del LOCK on sync functs: caused deadlocks.	*/
/* 03/08/00  JRB     11    Del #ifdef MVL_REQ_BUF_ENABLED: not used.	*/
/* 			   Del mvl_free_req_ctrl call. User must call.	*/
/*			   Del invoke_id, numpend_req: see mvl_send_req.*/
/* 01/21/00  MDE     10    Now use MEM_SMEM for dynamic memory		*/
/* 01/21/00  MDE     09    Use '_mvl_get_req_buf' for MVL_REQ_BUF_EN..	*/
/* 11/19/99  NAV     08	   Add #ifdef MVL_REQ_BUF_ENABLED		*/
/* 11/03/99  NAV     07    Add maxpend_req support to aynsc functions	*/
/* 09/13/99  MDE     06    Added SD_CONST modifiers			*/
/* 11/17/98  MDE     05    Now use MVL_ERR_COMM_SERVE_ACTIVE error code	*/
/*			   Use MVL_SYNC_REQ_INTERFACE			*/
/* 11/16/98  MDE     04    Renamed internal functions (prefix '_')	*/
/* 10/09/98  JRB     03    Chg MVL_CLI_JOURNAL_ENTRY to MVL_JOURNAL_ENTRY,*/
/*			   just like server (entry_id is now array	*/
/*			   instead of pointer).				*/
/* 06/15/98  MDE     02    Changes to allow compile under C++		*/
/* 03/27/98  JRB     01    MMS-LITE V4.0 Release			*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "glbsem.h"

#include "mmsdefs.h"
#include "mms_pvmd.h"
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
/*			mvla_jread					*/
/************************************************************************/

ST_RET mvla_jread (
	MVL_NET_INFO  *net_info,
	JREAD_REQ_INFO *req_info,
	MVL_REQ_PEND **req_out)
  {
ST_RET rc;
MVL_REQ_PEND *req_pend;


  *req_out = req_pend = _mvl_get_req_ctrl (net_info, MMSOP_READ_JOURNAL);
  if (!req_pend)
    return (MVL_ERR_REQ_CONTROL);
  req_pend->u.jread.resp_info = NULL;	/* Confirm funct must allocate	*/

  S_LOCK_COMMON_RESOURCES ();
  rc = _mvla_send_jread (net_info, req_info, req_pend);
  S_UNLOCK_COMMON_RESOURCES ();
  return (rc);
  }                                  

/************************************************************************/
/*			_mvla_send_jread				*/
/************************************************************************/

ST_RET _mvla_send_jread (MVL_NET_INFO  *net_info,
			 JREAD_REQ_INFO *req_info,
			 MVL_REQ_PEND *req_pend)
  {
ST_RET rc;
  
  /* Build a jread request               */
  rc = mpl_jread (req_info);
  if (rc == SD_SUCCESS)
    mpl_req_log (req_pend, req_info);

  rc = _mvl_send_req (net_info, req_pend, rc);
  return (rc);
  }                                  

/************************************************************************/
/*			_mvl_process_jread_conf				*/
/* Set "req_pend->u.jread.resp_info" to POINT to response info.		*/
/************************************************************************/

ST_VOID _mvl_process_jread_conf (MVL_REQ_PEND *req_pend)
  {
JREAD_RESP_INFO *jread_resp;
JOURNAL_ENTRY *jou_entry;

MVL_JREAD_RESP_INFO *mvl_jread_resp;
MVL_JOURNAL_ENTRY *mvl_jou_entry;
ST_INT j;
ST_INT info_size;

  jread_resp = (JREAD_RESP_INFO *) req_pend->event->u.mms.dec_rslt.data_ptr;

  info_size = sizeof (MVL_JREAD_RESP_INFO) +
     	      jread_resp->num_of_jou_entry * sizeof (MVL_JOURNAL_ENTRY);

  /* Allocate and fill in "req_pend->u.jread.resp_info".		*/
  /* CRITICAL: mvl_free_req_ctrl () must free it.		*/
  req_pend->u.jread.resp_info = 
	(MVL_JREAD_RESP_INFO *) M_CALLOC (MSMEM_GEN, 1, info_size);
  mvl_jread_resp = req_pend->u.jread.resp_info;	/* Local ptr to it.	*/
  mvl_jread_resp->num_of_jou_entry  = jread_resp->num_of_jou_entry;
  mvl_jread_resp->more_follows = jread_resp->more_follows;
  mvl_jread_resp->jou_entry = (MVL_JOURNAL_ENTRY *) (mvl_jread_resp + 1);
  if (mvl_jread_resp->num_of_jou_entry)
    {
    jou_entry = (JOURNAL_ENTRY *)(jread_resp + 1);	/* point to first.*/
    mvl_jou_entry = mvl_jread_resp->jou_entry;		/* point to first.*/
    for (j = 0;  j < jread_resp->num_of_jou_entry;  j++)
      {
      mvl_jou_entry->entry_id_len = jou_entry->entry_id_len;
      memcpy (mvl_jou_entry->entry_id, jou_entry->entry_id, jou_entry->entry_id_len);
      if (ms_asn1_to_appref (&mvl_jou_entry->orig_app, jou_entry->orig_ae,
                             jou_entry->orig_ae_len))
        {
        MVL_LOG_ERR0 ("Error: ReadJournal-Resp: orig_app decode failed");
        }
      memcpy (&mvl_jou_entry->occur_time, &jou_entry->ent_content.occur_time,
              sizeof (MMS_BTOD));
      mvl_jou_entry->entry_form_tag = jou_entry->ent_content.entry_form_tag;
      if (mvl_jou_entry->entry_form_tag == 2)	/* DATA	*/
        {
        mvl_jou_entry->ef.data.event_pres = jou_entry->ent_content.ef.data.event_pres;
        memcpy (&mvl_jou_entry->ef.data.evcon_name,
                &jou_entry->ent_content.ef.data.evcon_name, sizeof (OBJECT_NAME));
        mvl_jou_entry->ef.data.cur_state = jou_entry->ent_content.ef.data.cur_state;
        mvl_jou_entry->ef.data.list_of_var_pres = jou_entry->ent_content.ef.data.list_of_var_pres;
        if (mvl_jou_entry->ef.data.list_of_var_pres)
          {
          mvl_jou_entry->ef.data.num_of_var = jou_entry->ent_content.ef.data.num_of_var;
          mvl_jou_entry->ef.data.list_of_var = (VAR_INFO *)(jou_entry + 1);
          }
        }
      else					/* MUST BE ANNOTATION	*/
        {
        mvl_jou_entry->ef.annotation = jou_entry->ent_content.ef.annotation;
        }
    
      /* Point to next src and dst entry.					*/
      jou_entry++;
      if (mvl_jou_entry->entry_form_tag == 2  &&		/* DATA	*/
          mvl_jou_entry->ef.data.list_of_var_pres)
        jou_entry = (JOURNAL_ENTRY *) ( ((ST_CHAR *)jou_entry) +
                    mvl_jou_entry->ef.data.num_of_var * sizeof (VAR_INFO));
      mvl_jou_entry++;
      }	/* end "loop"	*/
    }	/* end "if num_of_jou_entry"	*/
  }

