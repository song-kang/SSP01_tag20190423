/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	1994 - 2002, All Rights Reserved		        */
/*									*/
/* MODULE NAME : c_write.c   						*/
/* PRODUCT(S)  : MMSEASE-LITE						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*	mvla_write_variables						*/
/*	_mvl_process_write_conf						*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 11/23/11  KCR           Added casting for proper 64bit compile       */
/* 03/02/10  JRB	   Del synchronous client functions.		*/
/* 09/25/08  MDE     25    Added MMS result info			*/
/* 04/09/07  MDE     24    Enhanced filtered logging 			*/
/* 01/30/06  GLB     23    Integrated porting changes for VMS           */
/* 07/16/02  JRB     22    Skip memmove to front of asn1_buf.		*/
/*			   Make sure num vars matches num data.		*/
/*			   _mvla_send..: Del unused args, make static.	*/
/* 03/06/02  JRB     21    Add S_UNLOCK.. before ALL returns.		*/
/* 02/25/02  MDE     20    Now get max PDU size from mvl_cfg_info	*/
/* 01/02/02  JRB     19    Converted to use ASN1R (re-entrant ASN1)	*/
/* 11/01/01  JRB     18    mvl_wait_req_done now returns ST_RET.	*/
/* 09/21/01  JRB     17    Alloc global bufs only once at startup.	*/
/* 03/07/01  JRB     16    Del LOCK on sync functs: caused deadlocks.	*/
/* 03/08/00  JRB     15    Del #ifdef MVL_REQ_BUF_ENABLED: not used.	*/
/* 			   Del mvl_free_req_ctrl call. User must call.	*/
/*			   Del invoke_id, numpend_req: see mvl_send_req.*/
/* 01/21/00  MDE     14    Now use MEM_SMEM for dynamic memory		*/
/* 01/21/00  MDE     13    Use '_mvl_get_req_buf' for MVL_REQ_BUF_EN..	*/
/* 11/19/99  NAV     12	   Add #ifdef MVL_REQ_BUF_ENABLED		*/
/* 10/20/99  NAV     11    Add maxpend_req support to aynsc functions	*/
/* 09/13/99  MDE     10    Added SD_CONST modifiers			*/
/* 03/05/99  MDE     09    Fixed NVL write pointer arithmatic problem	*/
/* 12/08/98  MDE     08    Added client alternate access support	*/
/* 11/17/98  MDE     07    Now use MVL_ERR_COMM_SERVE_ACTIVE error code	*/
/*			   Use MVL_SYNC_REQ_INTERFACE			*/
/* 11/16/98  MDE     06    Renamed internal functions (prefix '_')	*/
/* 06/15/98  MDE     05    Changes to allow compile under C++		*/
/* 05/22/98  JRB     04    Pass (MVL_REQ_PEND**) to sync function also.	*/
/* 04/16/98  JRB     03    Conf func: Let caller do "common" stuff.	*/
/*			   Always set (*req_out) so user can always free*/
/* 02/10/98  MDE     02    Removed NEST_RT_TYPES support		*/
/* 09/10/97  MDE     01    MMS-LITE V4.0 Release			*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "glbsem.h"

#include "mmsdefs.h"
#include "mms_pvar.h"
#include "mms_vvar.h"
#include "mvl_acse.h"
#include "mvl_log.h"


static ST_RET _mvla_send_write_variables (MVL_NET_INFO *net_info,
			    WRITE_REQ_INFO *write_info,
			    MVL_REQ_PEND *req_pend);
static ST_RET w_get_asn1_data (MVL_WRITE_REQ_INFO *w_info,
                         ST_UCHAR *asn1_dest_buffer,
                         ST_INT asn1_buffer_len,
                         ST_INT *asn1_len_out);


/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/
/************************************************************************/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/
/*			mvla_write_variables				*/
/************************************************************************/

ST_RET mvla_write_variables (MVL_NET_INFO *net_info,
			    WRITE_REQ_INFO *write_info,
                            ST_INT num_data,
                            MVL_WRITE_REQ_INFO *w_info,
			    MVL_REQ_PEND **req_out)
  {
ST_RET rc;
MVL_REQ_PEND *req_pend;
VARIABLE_LIST   *vl_ptr;
VAR_ACC_DATA *va_ptr;
ST_UCHAR *asn1_buf;
ST_INT asn1_len;
ST_INT asn1_buf_left;
ST_INT i;

  *req_out = req_pend = _mvl_get_req_ctrl (net_info, MMSOP_WRITE);
  if (!req_pend)
    return (MVL_ERR_REQ_CONTROL);

  req_pend->u.wr.w_info = w_info;
  req_pend->u.wr.num_data = num_data;

  S_LOCK_COMMON_RESOURCES ();

/* Make the write request operation specific data structure		*/
/* NOTE: Number of vars MUST match number of data.  To be sure,		*/
/*       overwrite values user entered.					*/
  write_info->num_of_data = num_data;
  write_info->va_spec.num_of_variables = num_data;	/* only used if VARLIST*/

  vl_ptr    = (VARIABLE_LIST *) (write_info+1);
  if (write_info->va_spec.var_acc_tag == VAR_ACC_NAMEDLIST)
    write_info->va_data = (VAR_ACC_DATA  *) (write_info + 1);
  else
    write_info->va_data = (VAR_ACC_DATA  *) (vl_ptr + 
				write_info->va_spec.num_of_variables);
  va_ptr  = write_info->va_data;

/* encode the data						      */
  asn1_buf = (ST_UCHAR *) M_MALLOC (MSMEM_ASN1_DATA_ENC, mvl_cfg_info->max_msg_size);
  asn1_buf_left = mvl_cfg_info->max_msg_size;
  for (i = 0; i < num_data; ++i, vl_ptr++, va_ptr++, w_info++)
    {
    rc = w_get_asn1_data(w_info,asn1_buf,asn1_buf_left,&asn1_len);
    if (rc != SD_SUCCESS)
      {
      MVL_LOG_NERR1 ("Write : w_get_asn1_data failed, ret = %x", rc);
      S_UNLOCK_COMMON_RESOURCES ();
      return(rc);  
      }
    else
      {
      /* Data encoded at end of asn1_buf. Adjust len so next data	*/
      /* encodes before this data. Save ptr and len.			*/
      asn1_buf_left -= asn1_len;        /* adjust buffer control vars	*/
      va_ptr->len = asn1_len;
      va_ptr->data = asn1_buf + asn1_buf_left;
#if defined(MVL_AA_SUPP)
      if (w_info->alt_acc_pres)
        {
	rc = _mvl_get_asn1_aa (w_info[i].alt_acc, vl_ptr);
	if (rc != SD_SUCCESS)
          {
          MVL_LOG_NERR0 ("Error creating ASN.1 encoded Alternate Access");
          S_UNLOCK_COMMON_RESOURCES ();
          return (SD_FAILURE);
          }
        }
#endif 
      }
    }

  rc = _mvla_send_write_variables (net_info, write_info, req_pend);
  M_FREE (MSMEM_ASN1_DATA_ENC, asn1_buf);

  S_UNLOCK_COMMON_RESOURCES ();
  return (rc);
  }                                  

/************************************************************************/
/*			_mvla_send_write_variables			*/
/************************************************************************/

static ST_RET _mvla_send_write_variables (MVL_NET_INFO *net_info,
			    WRITE_REQ_INFO *write_info,
			    MVL_REQ_PEND *req_pend)
  {
ST_RET rc;

  /* Build a write request               */
  rc = mpl_write (write_info);
  if (rc == SD_SUCCESS)
    mpl_req_log (req_pend, write_info);

#if defined(MVL_AA_SUPP)
  _m_free_aa_asn1_buf ();
#endif

  rc = _mvl_send_req (net_info, req_pend, rc);
  return (rc);
  }

/************************************************************************/
/*			w_get_asn1_data					*/
/************************************************************************/
/* This function is to provide ASN.1 encoded data, given the variable	*/
/* list element.							*/

static ST_RET w_get_asn1_data (MVL_WRITE_REQ_INFO *wr_info,
                         ST_UCHAR *asn1_dest_buffer,
                         ST_INT asn1_buffer_len,
                         ST_INT *asn1_len_out)
  {
RUNTIME_TYPE *rt;
ST_INT num_rt;
ST_RET rc;
ST_UCHAR *asn1_start;
ST_INT asn1_len;
ST_BOOLEAN packed_save;
ASN1_ENC_CTXT localEncCtx;	/* For readability, use "aCtx" to access this.*/
ASN1_ENC_CTXT *aCtx = &localEncCtx;

  rc = mvl_get_runtime (wr_info->type_id,&rt, &num_rt);
  if (rc != SD_SUCCESS)
    {
    MVL_LOG_NERR1 ("Get ASN1 Data : mvl_get_runtime failed, rc = %x", rc);
    return (rc);     		/* bad type id				*/
    }

/* Check to see if this is an array with variable number of elements	*/
/* and modify the size if necessary -					*/

  if (wr_info->arr)
    mvl_mod_arr_size (rt, wr_info->num_el);

/* First we need to initialize the ASN.1 encode tools, then do the 	*/
/* encode							   	*/

  asn1r_strt_asn1_bld (aCtx, asn1_dest_buffer, asn1_buffer_len);

#if defined(MVL_AA_SUPP)
  if (wr_info->alt_acc_pres &&
      wr_info->alt_acc_type_aa == SD_FALSE)
    {
    packed_save = m_alt_acc_packed;
    m_alt_acc_packed = wr_info->alt_acc_data_packed;
    rc = ms_local_to_asn1_aa (aCtx, rt, num_rt, wr_info->alt_acc, 
			    (ST_CHAR *) wr_info->local_data);
    m_alt_acc_packed = packed_save;
    }
  else
    rc = ms_local_to_asn1 (aCtx, rt, num_rt, (ST_CHAR *) wr_info->local_data);
#else
  rc = ms_local_to_asn1 (aCtx, rt, num_rt, (ST_CHAR *) wr_info->local_data);
#endif /* #if defined(MVL_AA_SUPP) */
  if (rc != SD_SUCCESS)
    {
    MVL_LOG_NERR1 ("Get ASN1 Data : ms_local_to_asn1 failed, rc = %x", rc);
    return (rc);     		/* bad type id				*/
    }

/* When the ASN.1 encode is complete, 'asn1_field_ptr' points 1 byte ahead 	*/
/* of the start of the message. The ASN.1 message is build from back to	*/
/* front, so that is now we calculate the length of the PDU		*/

  asn1_start = aCtx->asn1r_field_ptr+1;
  asn1_len = (int) ((asn1_dest_buffer + asn1_buffer_len) - asn1_start);
  *asn1_len_out = asn1_len;
  return (rc);
  }

/************************************************************************/
/*			_mvl_process_write_conf				*/
/************************************************************************/

ST_VOID _mvl_process_write_conf (MVL_REQ_PEND *req)
  {
ST_INT i;
WRITE_RESP_INFO  *write_info;
WRITE_RESULT     *wr_rslt;
ST_INT num_variables;
MVL_WRITE_REQ_INFO *w_info;

  write_info = (WRITE_RESP_INFO *) req->event->u.mms.dec_rslt.data_ptr;
  wr_rslt    = (WRITE_RESULT *) (write_info+1);

  /* retrieve output information					*/
  w_info = req->u.wr.w_info;
  num_variables = req->u.wr.num_data;
  
  /* just a sanity check						*/
  if (num_variables != write_info->num_of_result)
    {
    req->result = SD_FAILURE;
    return;
    }

    /* For each variable returned in the write response ... */

  for (i=0; i < num_variables; i++, wr_rslt++, w_info++)
    {      /* check for successful write of this variable 		*/
    if (wr_rslt->resp_tag == WR_RSLT_SUCCESS)
      w_info->result = SD_SUCCESS;
    else  /* bad type ID						*/
      w_info->result = SD_FAILURE;

  /* Save MMS level result information */
    w_info->resp_tag = wr_rslt->resp_tag;
    w_info->failure  = wr_rslt->failure;
    } /* for each variable						*/
  }

