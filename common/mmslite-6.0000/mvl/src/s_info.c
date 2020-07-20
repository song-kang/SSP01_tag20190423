/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	1994 - 1996, All Rights Reserved		        */
/*									*/
/* MODULE NAME : s_info.c   						*/
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
/* 11/23/11  KCR           Added casting for proper 64bit compile       */
/* 04/09/07  MDE     20    Enhanced filtered logging 			*/
/* 01/30/06  GLB     19    Integrated porting changes for VMS           */
/* 12/12/02  JRB     18    Add & use mvl_encode_info_rpt funct.		*/
/*			   Use new thread-safe mpl_info,_mvl_send_msg	*/
/*			   and skip S_LOCK..				*/
/*			   Pass net_info to mvl_get_va_asn1_data so	*/
/*			   don't need mvl_curr_net_info global var.	*/
/* 07/10/02  JRB     17    Call mvl_ureq_bufs_avail.			*/
/* 02/25/02  MDE     16    Now get max PDU size from mvl_cfg_info	*/
/* 09/21/01  JRB     15    Alloc global bufs only once at startup.	*/
/* 03/30/01  MDE     14    Changed MSMEM_ENC_INFO to MSMEM_ENC_OS_INFO	*/
/* 10/06/00  MDE     13    Added LOCK/UNLOCK				*/
/* 08/07/00  JRB     12    Set scope & domain name correctly for NVL.	*/
/* 05/15/00  MDE     11    Added MMS statistics				*/
/* 04/14/00  JRB     10    Lint cleanup.				*/
/* 01/21/00  MDE     09    Now use MEM_SMEM for dynamic memory		*/
/* 09/13/99  MDE     08    Added SD_CONST modifiers			*/
/* 09/07/99  MDE     07    Changed MVL_VA_SCOPE to MVL_SCOPE		*/
/* 12/11/98  MDE     06    Removed scope references from VA		*/
/* 11/16/98  MDE     05    Renamed internal functions (prefix '_')	*/
/* 06/15/98  MDE     04    Changes to allow compile under C++		*/
/* 02/10/98  MDE     03    Removed NEST_RT_TYPES, always support AA	*/
/* 09/18/97  MDE     02    Fixed problem when no nvl->altAcc		*/
/* 09/10/97  MDE     01    MMS-LITE V4.0 Release			*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"

#include "mmsdefs.h"
#include "mms_pvar.h"
#include "mms_vvar.h"
#include "mvl_acse.h"
#include "mvl_log.h"
/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/
/************************************************************************/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/
/************************************************************************/

ST_RET mvl_info_variables (MVL_NET_INFO *net_info, MVL_NVLIST_CTRL *nvl, 
			  ST_BOOLEAN listOfVariables)
  {
ST_RET rc;
ASN1_ENC_CTXT aCtx;
ST_UCHAR *my_enc_buf;

  if (mvl_ureq_bufs_avail (net_info) <= 0)
    {
    MVL_LOG_ERR0 ("Not enough ACSE buffers available to send InformationReport");
    return (MVL_ERR_REQ_PEND_COUNT);
    }

  my_enc_buf = (ST_UCHAR *) mvl_enc_buf_alloc (mmsl_enc_buf_size);
  asn1r_strt_asn1_bld (&aCtx, my_enc_buf, mmsl_enc_buf_size);/*CRITICAL*/
  rc = mvl_encode_info_rpt (net_info, nvl, listOfVariables, &aCtx);
  if (rc == SD_SUCCESS)
    {		/* Encode was successful, go ahead and send.	*/
    rc = _mvl_send_msg (ASN1_ENC_PTR(&aCtx),(int) ASN1_ENC_LEN(&aCtx),net_info);
    if (rc == SD_SUCCESS)	/* _mvl_send_msg logs on any error.	*/
      ++mvl_mms_statistics.serverInfoRpt;
    }
  mvl_enc_buf_free (my_enc_buf);
  return(rc);  
  }                                  

/************************************************************************/
/*			mvl_encode_info_rpt				*/
/************************************************************************/
ST_RET mvl_encode_info_rpt (MVL_NET_INFO *net_info,
			MVL_NVLIST_CTRL *nvl, 
			ST_BOOLEAN listOfVariables,
			ASN1_ENC_CTXT *aCtx)
  {
ST_RET rc;
INFO_REQ_INFO *info_info;
ACCESS_RESULT *ar;
ST_INT req_info_size;
MVL_VAR_ASSOC *va;
MVL_SCOPE *va_scope;
MVL_SCOPE vmdVaScope;
ST_UCHAR *asn1_buf;
ST_UCHAR *asn1_start;
ST_INT asn1_len;
ST_INT asn1_buf_left;
VARIABLE_LIST *vl;
ST_INT num_var;
ST_INT i;
ST_BOOLEAN alt_access_pres;
ALT_ACCESS *alt_acc;

  va = *nvl->entries;
  num_var = nvl->num_of_entries;

/* Calculate the required size for  the info report information		*/
  req_info_size = sizeof(INFO_REQ_INFO) + 
		    (num_var * sizeof(ACCESS_RESULT));

/* May need to add some more to handle the variable specification	*/
  if (listOfVariables == SD_TRUE)
    req_info_size += (num_var * sizeof(VARIABLE_LIST));

  info_info = (INFO_REQ_INFO *) M_CALLOC (MSMEM_ENC_OS_INFO, 1, req_info_size);
  info_info->num_of_acc_result = num_var;

/* We need to create the variable specification - either a list of	*/
/* variables or a named variable list					*/
  if (listOfVariables == SD_TRUE)
    {
    info_info->va_spec.var_acc_tag = VAR_ACC_VARLIST;
    info_info->va_spec.num_of_variables = num_var;

    vl = (VARIABLE_LIST *) (info_info + 1);
    for (i = 0; i < nvl->num_of_entries; ++i, ++vl)
      {
      va = nvl->entries[i];

    /* For backwards compatibility only ... */
      if (nvl->va_scope != NULL)
        va_scope = &nvl->va_scope[i];
      else
        {
        MVL_LOG_NERR0 ("Warning: Info Rpt nvl->va_scope is NULL, using VMD scope");
        va_scope = &vmdVaScope;
        va_scope->scope = VMD_SPEC;
        }

     /* We know this is a union so the following line is OK ... */
      vl->var_spec.vs.name.obj_name.vmd_spec = va->name;
      vl->var_spec.vs.name.object_tag = va_scope->scope;
      if (va_scope->scope == DOM_SPEC)
        vl->var_spec.vs.name.domain_id = va_scope->dom->name;

      }	 
    info_info->acc_rslt_list = (ACCESS_RESULT *) (vl);
    }
  else	/* Named Variable List */
    {
    info_info->va_spec.var_acc_tag = VAR_ACC_NAMEDLIST;
    info_info->va_spec.vl_name.object_tag = nvl->nvl_scope.scope;
    if (nvl->nvl_scope.scope == DOM_SPEC)
      info_info->va_spec.vl_name.domain_id = nvl->nvl_scope.dom->name;
    info_info->va_spec.vl_name.obj_name.vmd_spec = nvl->name; 
    info_info->acc_rslt_list = (ACCESS_RESULT *) (info_info + 1);
    }
  ar = info_info->acc_rslt_list;

  asn1_buf = (ST_UCHAR *) M_MALLOC (MSMEM_ASN1_DATA_ENC, mvl_cfg_info->max_msg_size);
  asn1_buf_left = mvl_cfg_info->max_msg_size;

/* Now encode the data, create the access result */
  asn1_start = asn1_buf;

  for (i = 0; i < nvl->num_of_entries; ++i, ++ar)
    {
    va = nvl->entries[i];
    alt_access_pres = SD_FALSE;
    if (nvl->altAcc)
      {
      alt_acc = nvl->altAcc[i];
      if (alt_acc != NULL)
        alt_access_pres = SD_TRUE;
      }
    else
      alt_acc = NULL;	/* Just so it's initialized. Eliminates warning.*/
    rc = mvl_get_va_asn1_data (net_info, va, alt_access_pres, alt_acc, asn1_start, 
				asn1_buf_left, &asn1_len);

    if (rc == SD_SUCCESS)
      {
      ar->acc_rslt_tag = ACC_RSLT_SUCCESS; 
      ar->va_data.len = asn1_len;
      ar->va_data.data = asn1_start;
      asn1_buf_left -= asn1_len;   /* adjust buffer control variables	*/
      asn1_start += asn1_len;
      }
    else
      {
      ar->acc_rslt_tag = ACC_RSLT_FAILURE;
      ar->failure = ARE_TYPE_INCONSISTENT;
      }
    }

  /* Build a info request               */
  rc = mpl_info (aCtx, info_info);

  /* Log the PDU, if enabled */
  mpl_unsol_req_log (net_info, MMSOP_INFO_RPT, info_info);

  M_FREE (MSMEM_ENC_OS_INFO, info_info);
  M_FREE (MSMEM_ASN1_DATA_ENC, asn1_buf);
  if (rc != SD_SUCCESS)
    MVL_LOG_NERR1 ("mvl_encode_info_rpt error = %x", rc);

  return (rc);
  }
