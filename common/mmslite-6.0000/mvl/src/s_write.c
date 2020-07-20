/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	1994 - 1996, All Rights Reserved		        */
/*									*/
/* MODULE NAME : s_write.c    						*/
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
/* 03/18/11  JRB           Set vaCtrl->is_61850_control if var is Control.*/
/*			   Add funct ptr u_mvl_write_ind_custom.	*/
/*			   Make sure num_of_variables = num_of_data.	*/
/* 02/26/10  JRB	   Don't call mvl_type_ctrl_find, use va->type_ctrl*/
/*			   Don't call mvl_get_runtime, use va->type_ctrl*/
/* 02/02/09  JRB     32    Chk va_to_free before ...free_va.		*/
/* 01/23/09  MDE     31    Use 0x%X for hex values in log msgs.		*/
/* 01/13/09  MDE     30    Free VA (and temp type) in case of errors 	*/
/* 05/01/08  JRB     29    Chk ms_asn1_to_local return & set		*/
/*			   WRITE_RESULT "failure" appropriately.	*/
/* 04/09/07  MDE     28    Enhanced filtered logging 			*/
/* 10/30/06  JRB     27    Use new mvl_vmd_* object handling functions.	*/
/*			   Add args on call to _mvl_objname_to_va.	*/
/*			   Elim use of global "_mvl_curr_net_info".	*/
/*			   Elim use of global "_mvl_curr_usr_ind_ctrl".	*/
/* 08/09/06  JRB     26    ALWAYS set resp_tag & failure in wr_rslt	*/
/*			   same as in vaCtrl. In put_asn1_data, set	*/
/*			   vaCtrl->failure to appropriate value.	*/
/* 01/30/06  GLB     25    Integrated porting changes for VMS           */
/* 05/16/05  JRB     24    Send appropriate failure codes in write resp.*/
/*			   Move statistics code so it is always executed*/
/* 03/13/03  JRB     23    Use mvl_type_ctrl_find.			*/
/* 07/24/02  JRB     22    Use mvl_send_resp_i like ALL responses.	*/
/* 07/09/02  MDE     21    Add maxpend_ind support			*/
/* 09/21/01  JRB     20    Alloc global bufs only once at startup.	*/
/* 05/25/01  MDE     19    Moved where the old data is saved		*/
/* 03/30/01  MDE     18    Changed MSMEM_ENC_INFO to MSMEM_ENC_OS_INFO	*/
/* 10/25/00  JRB     17    Del u_mvl funct ptrs. Call functs directly.	*/
/*			   Control with #ifdefs.			*/
/* 10/06/00  MDE     16    Now restore old data for write failure	*/
/* 05/15/00  MDE     15    Added MMS statistics				*/
/* 01/21/00  MDE     14    Now use MEM_SMEM for dynamic memory		*/
/* 11/22/99  JRB     13    mvlas_write_resp: del indCtrl from link list	*/
/* 09/13/99  MDE     12    Added SD_CONST modifiers			*/
/* 04/07/99  MDE     11    Logging improvements				*/
/* 12/11/98  MDE     10    Removed scope references from VA		*/
/* 12/08/98  MDE     09    Fixed write handling for alternate access 	*/
/* 11/16/98  MDE     08    Renamed internal functions (prefix '_')	*/
/* 09/21/98  MDE     07    Minor lint cleanup				*/
/* 06/15/98  MDE     06    Changes to allow compile under C++		*/
/* 05/15/98  JRB     05    Chg mvl_process.. arg to (MVL_IND_PEND *).	*/
/* 05/15/98  JRB     04    Call u_mvl_write_ind direct (no funct ptr).	*/
/* 02/20/98  MDE     03    Fixed usrVa set problem			*/
/* 02/10/98  MDE     02    Removed NEST_RT_TYPES, always support AA,	*/
/*			   added 'u_mvl_wr_ind_start' and 		*/
/*         		   'u_mvl_wr_resp_sent', other			*/
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

ST_RET (*u_mvl_wr_ind_start) (MVL_IND_PEND *indCtrl);
ST_VOID (*u_mvl_wr_resp_sent) (MVL_IND_PEND *indCtrl);
ST_VOID (*u_mvl_write_ind_custom) (MVL_IND_PEND *indCtrl);

/************************************************************************/
/* Internal functions							*/

static ST_VOID put_asn1_data (MVL_IND_PEND *indCtrl, VARIABLE_LIST *vl,
                             ST_UCHAR *asn1_buffer, ST_INT asn1_len,
			     MVLAS_WR_VA_CTRL *vaCtrl);

/************************************************************************/
/************************************************************************/
/*			_mvl_process_write_ind				*/
/* A write indication has been received, process it and respond		*/
/************************************************************************/

ST_VOID _mvl_process_write_ind (MVL_IND_PEND *indCtrl)
  {
MVLAS_WRITE_CTRL *wrCtrl;
MVLAS_WR_VA_CTRL *vaCtrl;
WRITE_REQ_INFO   *write_info;
ST_INT num_var;
VARIABLE_LIST *vl_ptr;
VAR_ACC_DATA  *va_ptr;
ST_INT i;   
ST_RET rc;
RUNTIME_TYPE *rt_first;
ST_CHAR *comp_name;

  write_info = (WRITE_REQ_INFO *) indCtrl->event->u.mms.dec_rslt.data_ptr;
  num_var = write_info->va_spec.num_of_variables;

  /* verify that this is a list of variables				*/
  if (write_info->va_spec.var_acc_tag != VAR_ACC_VARLIST)
    {
    _mplas_err_resp (indCtrl,7,2); 	/* access, non-existent	*/
    return;
    }

  /* If this is List Of Variables, num of var must equal num of data.	*/
  if (write_info->va_spec.var_acc_tag == VAR_ACC_VARLIST &&
      write_info->va_spec.num_of_variables != write_info->num_of_data)
    {
    _mplas_err_resp (indCtrl, MMS_ERRCLASS_ACCESS, MMS_ERRCODE_OBJ_ACCESS_DENIED);
    return;
    }

  if (u_mvl_wr_ind_start)
    {
    /* This might set optional user pointer "indCtrl->usr_ind_ctrl".	*/
    rc = (*u_mvl_wr_ind_start) (indCtrl);
    if (rc != SD_SUCCESS)
      {
      _mplas_err_resp (indCtrl,7,2); 	/* access, non-existent	*/
      return;
      }
    }

  wrCtrl = &indCtrl->u.wr;
  wrCtrl->vaCtrlTbl = (MVLAS_WR_VA_CTRL *) M_CALLOC (MSMEM_GEN, num_var, 
					sizeof(MVLAS_WR_VA_CTRL));
  vaCtrl = wrCtrl->vaCtrlTbl;
  wrCtrl->numVar = num_var;  

/* Looks OK, prepare to process each variable in the list		*/
  vl_ptr = (VARIABLE_LIST *) (write_info+1);
  va_ptr = write_info->va_data;

  for (i = 0; i < num_var; ++i, ++vl_ptr, ++va_ptr, ++vaCtrl)
    {
    put_asn1_data (indCtrl, vl_ptr, va_ptr->data, va_ptr->len, vaCtrl);

    /* CRITICAL: must set is_61850_control flag BEFORE calling u_mvl_write_ind*/
    if (vaCtrl->va && vaCtrl->va->base_va)
      {
      /* This is IEC 61850 variable. Check if it is a "Control".	*/
      rt_first = vaCtrl->va->type_ctrl->rt;
  
      /* If var is struct named "Oper", "SBOw" , or "Cancel", it is	*/
      /* IEC 61850 Control object.					*/
      if (rt_first->el_tag == RT_STR_START)
        {
        comp_name = ms_comp_name_find(rt_first);	/* macro never returns NULL*/
        if (strcmp (comp_name, "Oper")   == 0 ||
            strcmp (comp_name, "SBOw")   == 0 ||
            strcmp (comp_name, "Cancel") == 0)
          {	/* Set flag to check in leaf functions.	*/
          vaCtrl->is_61850_control = SD_TRUE;
          }
        }
      }
    }

  /* If user function ptr set, call it. User function must call	*/
  /* u_mvl_write_ind when it completes.				*/
  if (u_mvl_write_ind_custom)
    (*u_mvl_write_ind_custom) (indCtrl);
  else				/* do default processing	*/
    u_mvl_write_ind (indCtrl);
  }


/************************************************************************/
/*				mvlas_write_resp			*/
/************************************************************************/

ST_VOID mvlas_write_resp (MVL_IND_PEND *indCtrl)
  {
MVLAS_WRITE_CTRL *wrCtrl;
MVLAS_WR_VA_CTRL *vaCtrl;
MVL_COMM_EVENT *event;
MVL_NET_INFO *net_info; 
ST_RET rc;
ST_CHAR *resp_buf;
ST_INT num_var;
ST_INT va_data_size;
WRITE_RESP_INFO  *resp_info;
WRITE_RESULT     *wr_rslt;
ST_INT i;   

  net_info = indCtrl->event->net_info;
  event = indCtrl->event;
  wrCtrl = &indCtrl->u.wr;
  vaCtrl = wrCtrl->vaCtrlTbl;
  num_var = wrCtrl->numVar;  

  resp_buf = (ST_CHAR *) M_CALLOC (MSMEM_ENC_OS_INFO, 1, sizeof(WRITE_RESP_INFO) + 
				   (num_var * sizeof (WRITE_RESULT)));

    /* Create a write response info struct	*/
  resp_info = (WRITE_RESP_INFO *) resp_buf;
  resp_info->num_of_result = num_var;
  wr_rslt = (WRITE_RESULT *) (resp_info +1);
  
  for (i = 0; i < num_var; ++i, ++wr_rslt, ++vaCtrl)
    {
    wr_rslt->resp_tag = vaCtrl->resp_tag;
    wr_rslt->failure = vaCtrl->failure;
    if (vaCtrl->va != NULL)
      {
      if (wr_rslt->resp_tag == WR_RSLT_FAILURE)
        {	/* On a failure, restore the previous data */
        va_data_size = vaCtrl->va->type_ctrl->data_size;	/* always valid*/
        memcpy (vaCtrl->va->data, vaCtrl->oldData, va_data_size);
        }
      M_FREE (MSMEM_GEN, vaCtrl->oldData);
#if defined(MVL_UCA) || defined(USE_MANUFACTURED_OBJS)
      if (vaCtrl->va->va_to_free != NULL)
        u_mvl_free_va (MMSOP_WRITE, vaCtrl->va, event->net_info);
#endif	/* defined(MVL_UCA) || defined(USE_MANUFACTURED_OBJS)	*/
      }

    if (wr_rslt->resp_tag == WR_RSLT_SUCCESS)
      ++mvl_mms_statistics.serverVarWrOk;
    else
      ++mvl_mms_statistics.serverVarWrErr;
    }	/* end loop	*/

/* Build a write response 						*/
  rc = mpl_write_resp (event->u.mms.dec_rslt.id, resp_info);
  if (rc == SD_SUCCESS)
    mpl_resp_log (indCtrl, resp_info);

  if (u_mvl_wr_resp_sent)
    (*u_mvl_wr_resp_sent) (indCtrl);

  M_FREE (MSMEM_ENC_OS_INFO, resp_buf);
  M_FREE (MSMEM_GEN, wrCtrl->vaCtrlTbl);

  /* NOTE: This funct frees indCtrl. DO NOT use after this.	*/
  /* NOTE: if rc != SD_SUCCESS, this funct sends error resp.	*/
  _mvl_send_resp_i (indCtrl, rc);
  }


/************************************************************************/
/*			put_asn1_data					*/
/* This function is to provide ASN.1 encoded data, given the variable	*/
/* list element.							*/
/************************************************************************/

static ST_VOID put_asn1_data (MVL_IND_PEND *indCtrl, VARIABLE_LIST *vl,
                             ST_UCHAR *asn1_buffer, ST_INT asn1_len,
			     MVLAS_WR_VA_CTRL *vaCtrl)
  {
MVL_VAR_ASSOC *va;
RUNTIME_TYPE 	*rt;
ST_INT	num_rt;
ST_RET 	rc;
ST_CHAR *temp_data_buf;
ST_CHAR *va_data;
ST_INT  va_data_size;
ST_INT aa_mode;
ALT_ACCESS alt_acc; 	/* Alternate Access specification	*/
ST_BOOLEAN alt_access_pres;
ST_BOOLEAN alt_access_done;

/* There are more ways to fail than to succeed! */
/* Assume failure=ARE_TYPE_INCONSISTENT, but may be changed in some cases.*/
  vaCtrl->resp_tag = WR_RSLT_FAILURE; 
  vaCtrl->failure = ARE_TYPE_INCONSISTENT; 

  alt_access_pres = vl->alt_access_pres;
  if (alt_access_pres)  /* no alternate access support now	*/
    {
#if !defined(MVL_AA_SUPP)
    MVL_LOG_NERR0 ("Put ASN1 Data : Alternate Access not supported");
    return;
#else
  /* now create the AA, given the ASN.1 spec				*/
    if (ms_asn1_to_aa (vl->alt_access.data, vl->alt_access.len, 
           	       &alt_acc))
      {
      MVL_LOG_NERR0 ("Write Indication : ASN.1 -> AA failure");
      return;
      }
#endif
    }

  switch (vl->var_spec.var_spec_tag)
    {
    case VA_SPEC_NAMED :
      va = _mvl_objname_to_va (&mvl_vmd, indCtrl->event->net_info, MMSOP_WRITE, &vl->var_spec.vs.name, 
	    		       &vaCtrl->va_scope,
	      		       alt_access_pres,
          		       &alt_acc,
          		       &alt_access_done);

      if (alt_access_pres && alt_access_done)
        {
        M_FREE (MSMEM_GEN, alt_acc.aa);
        alt_access_pres = SD_FALSE;
        }

      if (!va)  /* could not find variable association	*/
        {
        MVL_LOG_NERR1 ("Write ind : Variable Association '%s' not found", 
				vl->var_spec.vs.name.obj_name.vmd_spec);
        vaCtrl->failure = ARE_OBJ_NONEXISTENT; 
        return;
        }

      vaCtrl->va = va;
      va->usr_ind_ctrl = indCtrl->usr_ind_ctrl;	/* save optional user pointer in va*/

    /* Save the current data in case the user rejects the write */

      va_data_size = va->type_ctrl->data_size;	/* always valid*/
      vaCtrl->oldData = (ST_UCHAR *) M_MALLOC (MSMEM_GEN, va_data_size);
      memcpy (vaCtrl->oldData, va->data, va_data_size);

    /* We have obtained a variable association				*/
    /* Now do the alternate access preprocess function if appropriate 	*/

#if defined(MVL_AA_SUPP)
      aa_mode = MVL_USE_UNPACKED_AA;
      if (alt_access_pres && va->proc && va->proc->proc_write_aa)
        {
        rc = (*va->proc->proc_write_aa)(&va, &alt_acc, 
                            indCtrl->event->net_info, &aa_mode);
      
        if (rc != SD_SUCCESS)
          {
          MVL_LOG_NERR1 ("Write : proc_write_aa returned %d", rc);
          return;
          }
        }
#endif


    /* Now take care of the data conversion (asn1 to local)		*/
      rt     = va->type_ctrl->rt;	/* used by ms_asn1_to_* below	*/
      num_rt = va->type_ctrl->num_rt;	/* used by ms_asn1_to_* below	*/

  /* Sanity check, because foundry can generate configured vars w/o	*/
  /* the data pointer initialized.					*/
      if (va->data == NULL)
        { 
        M_FREE (MSMEM_GEN, vaCtrl->oldData);
        MVL_LOG_NERR1 ("Write Indication : va '%s' data is NULL", va->name);
        vaCtrl->va = NULL;
        return;
        }

      va_data = (ST_CHAR *) va->data;
      /* "va_data_size" already set above	*/
    break;
  
    case VA_SPEC_DESCRIBED :
    default :
      MVL_LOG_NERR0 ("Put ASN1 Data : Invalid Variable Specification Tag");
      vaCtrl->failure = ARE_OBJ_ACC_UNSUPPORTED;
      return;
    break;
    }

/* If we got here, we have a valid runtime and have resolved local	*/
/* data pointer and size. Go ahead and convert the ASN.1 data element to*/
/* local data format							*/

  temp_data_buf = (ST_CHAR *) M_MALLOC (MSMEM_WR_DATA_DEC_BUF, va_data_size);


#if defined(MVL_AA_SUPP)
  if (alt_access_pres == SD_FALSE || aa_mode == MVL_USE_NO_AA)
    rc = ms_asn1_to_local (rt, num_rt, asn1_buffer, asn1_len, temp_data_buf);
  else
    {
  /* This is an alternate access write, we must first copy the data 	*/
  /* contents to to the temp buffer, then only the selected elements	*/
  /* will be written by the data conversion.				*/
    memcpy (temp_data_buf, va_data, va_data_size);

    if (aa_mode == MVL_USE_PACKED_AA)
      m_alt_acc_packed = SD_TRUE;
    else
      m_alt_acc_packed = SD_FALSE;

    rc = ms_asn1_to_local_aa (rt, num_rt, &alt_acc, 
			 asn1_buffer, asn1_len, temp_data_buf);
    }
#else
    rc = ms_asn1_to_local (rt, num_rt, asn1_buffer, asn1_len, temp_data_buf);
#endif

  if (rc != SD_SUCCESS)
    {
    MVL_LOG_NERR1 ("Put ASN1 Data : ms_asn1_to_local failed, rc = 0x%X", rc);
    M_FREE (MSMEM_GEN, vaCtrl->oldData);
    M_FREE (MSMEM_WR_DATA_DEC_BUF, temp_data_buf);

#if defined(MVL_UCA) || defined(USE_MANUFACTURED_OBJS)
    /* Need to free VA (caller will not if vaCtrl->va == NULL) */
    if (vaCtrl->va->va_to_free != NULL)
      u_mvl_free_va (MMSOP_WRITE, vaCtrl->va, NULL);
#endif
    vaCtrl->va = NULL;
    if (rc == ASN1E_UNEXPECTED_TAG)
      /* Tag mismatch in this context indicates MMS type mismatch.	*/
      vaCtrl->failure = ARE_TYPE_INCONSISTENT;
    else
      vaCtrl->failure = ARE_OBJ_VALUE_INVALID;
    return;
    }

  /* Call the preprocessing function for this variable, if it exists	*/
  if (va->proc && va->proc->pre_write_aa)
    {
    rc = (*va->proc->pre_write_aa)(va, alt_access_pres, &alt_acc, 
  			      indCtrl->event->net_info, 
  			      temp_data_buf, va_data_size);

    if (rc != SD_SUCCESS)
      {
      MVL_LOG_NERR1 ("Put ASN1 Data : call to Write pre-processor failed, rc = 0x%X", rc);
      M_FREE (MSMEM_GEN, vaCtrl->oldData);
#if defined(MVL_UCA) || defined(USE_MANUFACTURED_OBJS)
      /* Need to free VA (caller will not if vaCtrl->va == NULL) */
      if (vaCtrl->va->va_to_free != NULL)
        u_mvl_free_va (MMSOP_WRITE, vaCtrl->va, NULL);
#endif
      vaCtrl->va = NULL;
      }
    }

/* OK, almost done. The data has been converted OK and the user has had	*/
/* a chance to look at it. If rc == SD_SUCCESS, we are OK.		*/

  if (rc == SD_SUCCESS)
    {
    vaCtrl->resp_tag = WR_RSLT_SUCCESS; 
    memcpy (va_data, temp_data_buf, va_data_size);

  /* Call the postprocessing function for this variable, if it exists	*/
    if (va->proc && va->proc->post_write_aa)
      (*va->proc->post_write_aa)(va, alt_access_pres, &alt_acc, indCtrl->event->net_info);
    }

#if defined(MVL_AA_SUPP)
  if (alt_access_pres)		/* if we calloc'd this AA, free it	*/
    M_FREE (MSMEM_GEN, alt_acc.aa);
#endif
  M_FREE (MSMEM_WR_DATA_DEC_BUF, temp_data_buf);
  }
