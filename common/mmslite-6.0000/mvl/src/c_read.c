/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	1994 - 2004, All Rights Reserved		        */
/*									*/
/* MODULE NAME : c_read.c   						*/
/* PRODUCT(S)  : MMSEASE-LITE						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*	mvla_read_variables						*/
/*	_mvl_process_read_conf						*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 03/02/10  JRB	   Del synchronous client functions.		*/
/* 09/25/08  MDE     21    Added MMS result info			*/
/* 04/09/07  MDE     20    Enhanced filtered logging 			*/
/* 08/09/04  JRB     19    Log if type_id invalid.			*/
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
/* 12/08/98  MDE     09    Added client alternate access support	*/
/* 11/17/98  MDE     08    Now use MVL_ERR_COMM_SERVE_ACTIVE error code	*/
/*			   Use MVL_SYNC_REQ_INTERFACE			*/
/* 11/16/98  MDE     07    Renamed internal functions (prefix '_')	*/
/* 09/21/98  MDE     06    Minor lint cleanup				*/
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
/*			mvla_read_variables				*/
/************************************************************************/

ST_RET mvla_read_variables (MVL_NET_INFO  *net_info,
                        READ_REQ_INFO *read_info,
                        ST_INT num_data,
                        MVL_READ_RESP_PARSE_INFO *parse_info,
			MVL_REQ_PEND **req_out)
  {
ST_RET rc;
MVL_REQ_PEND *req_pend;

  *req_out = req_pend = _mvl_get_req_ctrl (net_info, MMSOP_READ);
  if (!req_pend)
    return (MVL_ERR_REQ_CONTROL);
  req_pend->u.rd.parse_info = parse_info;
  req_pend->u.rd.num_data = num_data;
  
  S_LOCK_COMMON_RESOURCES ();
  rc = _mvla_send_read_variables (net_info, read_info, num_data, parse_info, req_pend);
  S_UNLOCK_COMMON_RESOURCES ();
  return (rc);
  }                                  

/************************************************************************/
/*			_mvla_send_read_variables			*/
/************************************************************************/

ST_RET _mvla_send_read_variables (MVL_NET_INFO  *net_info,
                          READ_REQ_INFO *read_info,
			  ST_INT num_data,
			  MVL_READ_RESP_PARSE_INFO *parse_info,
			  MVL_REQ_PEND *req_pend)
  {
ST_RET rc;
VARIABLE_LIST *vl;
ST_INT i;

#if defined(MVL_AA_SUPP)
  if (read_info->va_spec.var_acc_tag == VAR_ACC_VARLIST)
    {
    vl = (VARIABLE_LIST *) (read_info + 1);
    for (i = 0; i < num_data; ++i, ++vl)
      {
      if (parse_info[i].alt_acc_pres == SD_TRUE)
        {
	rc = _mvl_get_asn1_aa (parse_info[i].alt_acc, vl);
	if (rc != SD_SUCCESS)
          {
          MVL_LOG_NERR0 ("Error creating ASN.1 encoded Alternate Access");
          return (SD_FAILURE);
          }
        }
      }
    }
#endif
  
  /* Build a read request               */
  rc = mpl_read (read_info);
  if (rc == SD_SUCCESS)
    mpl_req_log (req_pend, read_info);

#if defined(MVL_AA_SUPP)
  _m_free_aa_asn1_buf ();
#endif

  rc = _mvl_send_req (net_info, req_pend, rc);
  return (rc);
  }

/************************************************************************/

ST_VOID _mvl_process_read_conf (MVL_REQ_PEND *req)
  {
ST_INT i;
RUNTIME_TYPE *rt;
ST_INT rt_len;
READ_RESP_INFO  *read_info;
ACCESS_RESULT   *ar_ptr;
MVL_READ_RESP_PARSE_INFO *parse_table;
ST_INT num_variables;
ST_BOOLEAN packed_save;

  read_info = (READ_RESP_INFO *) req->event->u.mms.dec_rslt.data_ptr;
  ar_ptr    = read_info->acc_rslt_list;

  /* retrieve parse information						*/
  parse_table = req->u.rd.parse_info;
  num_variables = req->u.rd.num_data;
  
  /* just a sanity check						*/
  if (num_variables != read_info->num_of_acc_result)
    {
    MVL_LOG_NERR0 ("Read Confirmation : num var != num result");
    req->result = SD_FAILURE;
    return;
    }

    /* For each variable returned in the read response ... */

  for (i = 0; i < num_variables; ++i, ++ar_ptr, ++parse_table)
    {      /* check for successfull read of this variable */
    if (ar_ptr->acc_rslt_tag == ACC_RSLT_SUCCESS)
      {
        /* convert data in read buffer to local representation */
        /* and store in local buffer                           */
        
      if (mvl_get_runtime (parse_table->type_id,&rt, &rt_len) == SD_SUCCESS)
        {
        if (parse_table->descr_arr)
          {
          mvl_mod_arr_size (rt, parse_table->arr_size);
          }
#if defined(MVL_AA_SUPP)
        if (parse_table->alt_acc_pres == SD_TRUE &&
            parse_table->alt_acc_type_aa == SD_FALSE)
          {
          packed_save = m_alt_acc_packed;
          m_alt_acc_packed = parse_table->alt_acc_data_packed;
          parse_table->result = ms_asn1_to_local_aa (rt, rt_len,
                                      parse_table->alt_acc,
                                      ar_ptr->va_data.data,
                                      ar_ptr->va_data.len,
                                      (ST_CHAR *) parse_table->dest);
          m_alt_acc_packed = packed_save;
          }
        else	/* No alternate access */
          {          
          parse_table->result = ms_asn1_to_local (rt, rt_len,
                                      ar_ptr->va_data.data,
                                      ar_ptr->va_data.len,
                                      (ST_CHAR *) parse_table->dest);

          }                        
#else  /* We don't support alternate access */
        parse_table->result = ms_asn1_to_local (rt, rt_len,
                                      ar_ptr->va_data.data,
                                      ar_ptr->va_data.len,
                                      (ST_CHAR *) parse_table->dest);

#endif  /* #if defined(MVL_AA_SUPP) */
        }                        
      else  /* bad type ID						*/
        {
        MVL_LOG_NERR2 ("Read Confirmation : type_id %d invalid for variable #%d", parse_table->type_id, i);
        parse_table->result = SD_FAILURE;
        }
      }
    else /* access result == failure					*/
      parse_table->result = SD_FAILURE;

  /* Save MMS level result information */
    parse_table->acc_rslt_tag = ar_ptr->acc_rslt_tag;
    parse_table->failure      = ar_ptr->failure;
    } /* for each variable						*/
  }

