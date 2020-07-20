/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	1994 - 2012, All Rights Reserved				*/
/*									*/
/* MODULE NAME : s_getvla.c    						*/
/* PRODUCT(S)  : MMSEASE-LITE						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*	_mvl_process_getvla_ind						*/
/*	mvlas_getvlist_resp						*/
/*	mplas_getvlist_resp						*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 01/11/12  JRB	   If alt access in NVL, encode it in response.	*/
/* 09/27/10  JRB	   Save net_info to use after indCtrl freed.	*/
/* 04/09/07  MDE     17    Enhanced filtered logging 			*/
/* 11/02/06  JRB     16    mplas_err_resp: fix class/code for IEC 61850.*/
/* 10/30/06  JRB     15    Use new mvl_vmd_* object handling functions.	*/
/*			   Elim use of global var "_mvl_curr_net_info".	*/
/* 08/09/06  JRB     14    For MVL_UCA, call mvl_find_nvl		*/
/*			   (DO NOT call u_mvl_get_nvl, u_mvl_free_nvl).	*/
/* 09/21/01  JRB     13    Alloc global bufs only once at startup.	*/
/* 03/30/01  MDE     12    Changed MSMEM_ENC_INFO to MSMEM_ENC_OS_INFO	*/
/* 10/25/00  JRB     11    Del u_mvl funct ptrs. Call functs directly.	*/
/*			   Control with #ifdefs.			*/
/* 01/21/00  MDE     10    Now use MEM_SMEM for dynamic memory		*/
/* 09/13/99  MDE     09    Added SD_CONST modifiers			*/
/* 09/07/99  MDE     08    Changed MVL_VA_SCOPE to MVL_SCOPE		*/
/* 01/15/99  JRB     07    IF (u_mvl_get_nvl_fun != NULL) call it,	*/
/*			   ELSE call "mvl_find_nvl".			*/
/* 12/11/98  MDE     06    Removed scope references from VA		*/
/* 11/16/98  MDE     05    Renamed internal functions (prefix '_')	*/
/* 09/21/98  MDE     04    Minor lint cleanup				*/
/* 06/15/98  MDE     03    Changes to allow compile under C++		*/
/* 05/14/98  JRB     02    Use MVL_IND_PEND. Add mvlas resp function.	*/
/*			   Use new mms_deletable in MVL_NVLIST_CTRL.	*/
/* 09/10/97  MDE     01    MMS-LITE V4.0 Release			*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
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
/************************************************************************/
/*			_mvl_process_getvla_ind				*/
/* A 'Get Variable List Attributes' indication has been received, 	*/
/* process it and respond						*/
/************************************************************************/

ST_VOID _mvl_process_getvla_ind (MVL_IND_PEND *indCtrl)
  {
  indCtrl->u.getvlist.req_info =
        (GETVLIST_REQ_INFO *) indCtrl->event->u.mms.dec_rslt.data_ptr;

  u_mvl_getvlist_ind (indCtrl);
  }

/************************************************************************/
/*			mvlas_getvlist_resp				*/
/************************************************************************/
ST_VOID mvlas_getvlist_resp (MVL_IND_PEND *indCtrl)
  {
GETVLIST_REQ_INFO  *req_info;
GETVLIST_RESP_INFO  *resp_info;
MVL_NVLIST_CTRL *nvlist;
ST_INT num_var;
ST_INT i;
VARIABLE_LIST *vl;
MVL_VAR_ASSOC *va;
MVL_SCOPE *va_scope;
MVL_SCOPE vmdVaScope;
/* indCtrl will be freed by mplas_getvlist_resp, so save net_info to local.*/
MVL_NET_INFO *net_info = indCtrl->event->net_info;
ST_UCHAR **asn1BufArray;	/* array of ASN.1 pointers for Alt Access*/
				/* one pointer for each var		*/
ST_RET retcode;
  
  asn1BufArray = NULL;	/* Initialize. Changed if Alt Access present.	*/
  retcode = SD_SUCCESS;	/* Initialize. Changed if any error found.	*/

  req_info = indCtrl->u.getvlist.req_info;


/* Now find the named variable list */
#if defined(USE_MANUFACTURED_OBJS)
  nvlist = u_mvl_get_nvl (MMSOP_GET_VLIST, &req_info->vl_name, 
                         net_info);
#else	/* default	*/
  nvlist = mvl_vmd_find_nvl (&mvl_vmd, &req_info->vl_name, net_info);
#endif	/* default	*/

  if (nvlist)
    {
    num_var = nvlist->num_of_entries;
    resp_info = (GETVLIST_RESP_INFO *)
                M_CALLOC (MSMEM_ENC_OS_INFO, 1, sizeof(GETVLIST_RESP_INFO) + 
			             (num_var * sizeof(VARIABLE_LIST)));
    /* If Alternate Access present, allocate array of ASN.1 pointers.	*/
    if (nvlist->altAccArray)
      {
      asn1BufArray = (ST_UCHAR **) chk_calloc (num_var, sizeof (ST_UCHAR *));
      }
    resp_info->mms_deletable = nvlist->mms_deletable;
    resp_info->num_of_variables = num_var;
    vl = (VARIABLE_LIST *) (resp_info + 1);
    for (i = 0; i < num_var; ++i, ++vl)
      {
      va = nvlist->entries[i];

    /* For backwards compatibility only ... */
      if (nvlist->va_scope != NULL)
        va_scope = &nvlist->va_scope[i];
      else
        {
        MVL_LOG_NERR0 ("Warning: Info Rpt nvl->va_scope is NULL, using VMD scope");
        va_scope = &vmdVaScope;
        va_scope->scope = VMD_SPEC;
        }

      /* Encode Alternate Access information, if present.	*/
      if (nvlist->altAccArray && nvlist->altAccArray[i])
        {
        ST_INT buf_len;
    
        ST_UCHAR *asn1_ptr;
        ST_INT asn1_len;

        /* Allow 20 bytes of encoding for each Alt Access element.*/
        buf_len = nvlist->altAccArray[i]->num_aa * 20;
        asn1BufArray [i] = chk_malloc (buf_len);
        /* Convert from ALT_ACCESS structure to ASN.1 encoding.	*/
        retcode = ms_aa_to_asn1_2 (nvlist->altAccArray[i], asn1BufArray[i], buf_len,
              &asn1_ptr, &asn1_len);
        if (retcode)
          {	/* Alt Access encoding failed.	*/
          break;	/* break out of loop	*/
          }
        vl->alt_access_pres = SD_TRUE;
        vl->alt_access.len = asn1_len;
        vl->alt_access.data = asn1_ptr;
        }
      vl->var_spec.var_spec_tag = VA_SPEC_NAMED;

     /* We know this is a union so the following line is OK ... */
      vl->var_spec.vs.name.obj_name.vmd_spec = va->name;
      vl->var_spec.vs.name.object_tag = va_scope->scope;
      if (va_scope->scope == DOM_SPEC)
        vl->var_spec.vs.name.domain_id = va_scope->dom->name;
      }	/* end loop through variables	*/

    if (retcode)
      {	/* something failed in the loop above	*/
      MVL_LOG_ERR0 ("Get Var List Attributes Indication : Alternate Access encoding error");
      _mplas_err_resp (indCtrl,MMS_ERRCLASS_DEFINITION,MMS_ERRCODE_OBJ_INVALIDATED);
      }
    else
      {
      indCtrl->u.getvlist.resp_info = resp_info;

      mplas_getvlist_resp (indCtrl);
      }

    /* If asn1BufArray was allocated, free it now.	*/
    if (asn1BufArray)
      {
      for (i = 0; i < num_var; i++)
        {
        if (asn1BufArray[i])
          chk_free(asn1BufArray[i]);
        }
      chk_free (asn1BufArray);
      }

#if defined(USE_MANUFACTURED_OBJS)
    u_mvl_free_nvl (MMSOP_GET_VLIST, nvlist, net_info);
#endif

    M_FREE (MSMEM_ENC_OS_INFO, resp_info);
    }
  else
    {  
    MVL_LOG_NERR0 ("Get Var List Attributes Indication : could not find the list");
    _mplas_err_resp (indCtrl,MMS_ERRCLASS_DEFINITION,MMS_ERRCODE_OBJ_UNDEFINED);
    }
  }

/************************************************************************/
/*			mplas_getvlist_resp				*/
/************************************************************************/

ST_VOID mplas_getvlist_resp (MVL_IND_PEND *indCtrl)
  {
ST_RET rc;

  rc = mpl_getvlist_resp (indCtrl->event->u.mms.dec_rslt.id,
                          indCtrl->u.getvlist.resp_info);
  if (rc == SD_SUCCESS)
    mpl_resp_log (indCtrl, indCtrl->u.getvlist.resp_info);

  _mvl_send_resp_i (indCtrl, rc);
  }
       
