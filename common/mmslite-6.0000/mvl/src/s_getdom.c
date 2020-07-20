/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	1994 - 1996, All Rights Reserved		        */
/*									*/
/* MODULE NAME : s_getdom.c    						*/
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
/* 04/09/07  MDE     09    Enhanced filtered logging 			*/
/* 10/30/06  JRB     08    Use new mvl_vmd_* object handling functions.	*/
/* 09/21/01  JRB     07    Alloc global bufs only once at startup.	*/
/* 01/21/00  MDE     06    Now use MEM_SMEM for dynamic memory		*/
/* 09/13/99  MDE     05    Added SD_CONST modifiers			*/
/* 11/16/98  MDE     04    Renamed internal functions (prefix '_')	*/
/* 06/15/98  MDE     03    Changes to allow compile under C++		*/
/* 05/14/98  JRB     02    Use MVL_IND_PEND. Add mplas resp function.	*/
/* 09/10/97  MDE     01    MMS-LITE V4.0 Release			*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "mmsdefs.h"
#include "mms_pdom.h"
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
/*			_mvl_process_getdom_ind				*/
/* A 'Get Domain Attributes' indication has been received, 		*/
/* process it and respond						*/
/************************************************************************/

ST_VOID _mvl_process_getdom_ind (MVL_IND_PEND *indCtrl)
  {
  indCtrl->u.getdom.req_info =
        (GETDOM_REQ_INFO *) indCtrl->event->u.mms.dec_rslt.data_ptr;

  u_mvl_getdom_ind (indCtrl);
  }

/************************************************************************/
/*			mvlas_getdom_resp				*/
/************************************************************************/
ST_VOID mvlas_getdom_resp (MVL_IND_PEND *indCtrl)
  {
GETDOM_REQ_INFO  *req_info;
GETDOM_RESP_INFO  *resp_info;
GETDOM_RESP_INFO  default_resp_info;
MVL_DOM_CTRL *dom;

  req_info = indCtrl->u.getdom.req_info;
  dom = mvl_vmd_find_dom (&mvl_vmd, req_info->dname);
  if (dom == NULL)
    {
    MVL_LOG_NERR1 ("GetDomAttrib Ind : %s not found", req_info->dname);
    _mplas_err_resp (indCtrl,2,1);	/* definition, object undefined	*/ 
    return;
    }

  resp_info = dom->get_dom_resp_info;
  if (resp_info == NULL)
    {
    resp_info = &default_resp_info;
    resp_info->num_of_capab = 0;
    resp_info->mms_deletable = 0;
    resp_info->sharable = 0;
    resp_info->num_of_pinames = 0;
    resp_info->state = 2;	    	/* 2 : ready			*/
    }
  indCtrl->u.getdom.resp_info = resp_info;
  mplas_getdom_resp (indCtrl);
  }

/************************************************************************/
/*			mplas_getdom_resp				*/
/************************************************************************/
ST_VOID mplas_getdom_resp (MVL_IND_PEND *indCtrl)
  {
ST_RET rc;
  rc = mpl_getdom_resp (indCtrl->event->u.mms.dec_rslt.id,
                        indCtrl->u.getdom.resp_info);
  if (rc == SD_SUCCESS)
    mpl_resp_log (indCtrl, indCtrl->u.getdom.resp_info);

  _mvl_send_resp_i (indCtrl, rc);
  }

