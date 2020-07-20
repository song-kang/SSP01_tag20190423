/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	1998 - 1998, All Rights Reserved				*/
/*									*/
/* MODULE NAME : s_delvl.c						*/
/* PRODUCT(S)  : MMSEASE-LITE						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*	_mvl_process_delvlist_ind					*/
/*	mvlas_delvlist_resp						*/
/*	mplas_delvlist_resp						*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev     Comments					*/
/* --------  ---  ------   -------------------------------------------	*/
/* 04/09/07  MDE     16    Enhanced filtered logging 			*/
/* 10/30/06  JRB     15    Use new mvl_vmd_* object handling functions.	*/
/* 08/21/06  JRB     14    Check mms_deletable flag in DELVL_SPEC case.	*/
/* 09/21/01  JRB     13    Alloc global bufs only once at startup.	*/
/* 07/13/00  JRB     12    Init num_matched, num_deleted for DELVL_SPEC.*/
/* 01/21/00  MDE     11    Now use MEM_SMEM for dynamic memory		*/
/* 09/13/99  MDE     10    Added SD_CONST modifiers			*/
/* 04/07/99  JRB     09    Add support for SPECIFIC scope.		*/
/* 04/07/99  MDE     08    Logging improvements				*/
/* 01/21/99  JRB     07    Add support for AA-specific scope.		*/
/* 01/14/99  JRB     06    Use new "bsearch" object model.		*/
/* 11/16/98  MDE     05    Now use '_mvl_destroy_nvl_entries'		*/
/* 11/16/98  MDE     04    Renamed internal functions (prefix '_')	*/
/* 06/22/98  MDE     03    More changes to allow compile under C++	*/
/* 06/15/98  MDE     02    Changes to allow compile under C++		*/
/* 04/14/98  JRB     01    MMS-LITE V4.0 Release			*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "mmsdefs.h"
#include "mms_pfil.h"
#include "mvl_defs.h"
#include "mvl_log.h"

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/
/************************************************************************/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

static ST_RET delete_nvlist (DELVLIST_RESP_INFO *resp_info,
		ST_INT *num_nvlist,
		MVL_NVLIST_CTRL **nvlist_tbl);
/************************************************************************/
/*			_mvl_process_delvlist_ind			*/
/* A "delvlist" indication has been received, process it and respond	*/
/************************************************************************/

ST_VOID _mvl_process_delvlist_ind (MVL_IND_PEND *indCtrl)
  {
  indCtrl->u.delvlist.req_info = 
      (DELVLIST_REQ_INFO *) indCtrl->event->u.mms.dec_rslt.data_ptr;

  u_mvl_delvlist_ind (indCtrl);
  }

/************************************************************************/
/*			mvlas_delvlist_resp				*/
/************************************************************************/
ST_VOID mvlas_delvlist_resp (MVL_IND_PEND *indCtrl)
  {
DELVLIST_RESP_INFO resp_info;
DELVLIST_REQ_INFO *req_info = indCtrl->u.delvlist.req_info;
MVL_DOM_CTRL *mvl_dom;
MVL_AA_OBJ_CTRL *aa_objs;
ST_INT j;
OBJECT_NAME *obj_name;
MVL_NVLIST_CTRL *nvl;

  if (req_info->scope == DELVL_VMD)
    {
    /* "delete_nvlist" deletes all deletable ("mvl_obj_delete" can't).*/
    if (delete_nvlist (&resp_info, &mvl_vmd.num_nvlist, mvl_vmd.nvlist_tbl))
      {
      _mplas_err_resp (indCtrl,4,0);	/* Service error , other	*/
      return;
      }
    }
  else if (req_info->scope == DELVL_DOM)
    {
    mvl_dom = mvl_vmd_find_dom (&mvl_vmd, req_info->dname);
    if (mvl_dom == NULL)
      {
      MVL_LOG_NERR1 ("Delete NVL ind: Domain '%s' not found", req_info->dname);
      _mplas_err_resp (indCtrl,2,1);	/* Object undefined	*/
      return;
      }
    /* "delete_nvlist" deletes all deletable ("mvl_obj_delete" can't).*/
    if (delete_nvlist (&resp_info, &mvl_dom->num_nvlist, mvl_dom->nvlist_tbl))
      {
      _mplas_err_resp (indCtrl,4,0);	/* Service error , other	*/
      return;
      }
    }
  else if (req_info->scope == DELVL_AA)
    {		
    aa_objs = indCtrl->event->net_info->aa_objs; 
    /* "delete_nvlist" deletes all deletable ("mvl_obj_delete" can't).*/
    if (delete_nvlist (&resp_info, &aa_objs->num_nvlist, aa_objs->nvlist_tbl))
      {
      _mplas_err_resp (indCtrl,4,0);	/* Service error , other	*/
      return;
      }
    }
  else
    {		/* Must be DELVL_SPEC (i.e. delete listed NVLs).	*/
    /* Init "obj_name" to point to array of OBJECT_NAME structs which	*/
    /* MMS decoder puts right after the "req_info" struct.		*/
    resp_info.num_matched = 0;
    resp_info.num_deleted = 0;

    obj_name = (OBJECT_NAME *) (req_info + 1);
    for (j = 0; j < req_info->num_of_vnames; j++, obj_name++)
      {
      if ((nvl = mvl_vmd_find_nvl (&mvl_vmd, obj_name, indCtrl->event->net_info)) != NULL)
        {
        resp_info.num_matched++;
        if (nvl->mms_deletable  &&
            mvl_vmd_nvl_remove (&mvl_vmd, obj_name, indCtrl->event->net_info) == SD_SUCCESS)
          resp_info.num_deleted++;
        }
      }
    }
  indCtrl->u.delvlist.resp_info = &resp_info;
  mplas_delvlist_resp (indCtrl);
  }

/************************************************************************/
/*			mplas_delvlist_resp				*/
/************************************************************************/
ST_VOID mplas_delvlist_resp (MVL_IND_PEND *indCtrl)
  {
ST_RET rc;

  /* Build a "delvlist" response 					*/
  rc = mpl_delvlist_resp (indCtrl->event->u.mms.dec_rslt.id,
                          indCtrl->u.delvlist.resp_info);
  if (rc == SD_SUCCESS)
    mpl_resp_log (indCtrl, indCtrl->u.delvlist.resp_info);

  _mvl_send_resp_i (indCtrl, rc);
  }

/************************************************************************/
/*		delete_nvlist						*/
/* This function deletes all nvlists with "mms_deletable == TRUE".	*/
/* It is more efficient than multiple calls to "mvl_obj_delete"	*/
/* because it deletes multiple entries with one call.			*/
/*    This function fills in "resp_info" and modifies "*num_mvlist" and	*/
/* the array pointed to by "nvlist_tbl".				*/
/************************************************************************/
static ST_RET delete_nvlist (DELVLIST_RESP_INFO *resp_info,
		ST_INT *num_nvlist,
		MVL_NVLIST_CTRL **nvlist_tbl)
  {
ST_INT j, k;

  resp_info->num_matched = (*num_nvlist);
  resp_info->num_deleted = 0;			/* Init count.		*/
  for (j = 0, k = 0;  j < (*num_nvlist);  j++)
    {
    if (nvlist_tbl[j]->mms_deletable)
      {
      /* free up allocated resources for this nvlist	*/
      mvl_nvl_destroy (nvlist_tbl[j]);
      resp_info->num_deleted++;		/* Increment count.	*/
      }
    else
      {				/* move this entry up in array.	*/
      nvlist_tbl[k] = nvlist_tbl[j];
      k++;			/* Increment count of entries left.	*/
      }
    }

  (*num_nvlist) -= (ST_INT) resp_info->num_deleted;
  return (SD_SUCCESS);
  }
