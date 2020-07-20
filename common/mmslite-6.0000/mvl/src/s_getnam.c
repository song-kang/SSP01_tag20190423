/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	1994 - 2005, All Rights Reserved		        */
/*									*/
/* MODULE NAME : s_getnam.c    						*/
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
/* 04/09/07  MDE     21    Enhanced filtered logging 			*/
/* 10/30/06  JRB     20    Use new mvl_vmd_* object handling functions.	*/
/*			   u_gnl_ind_*: add net_info arg to		*/
/*			   elim use of global var "_mvl_curr_net_info".	*/
/* 01/19/05  JRB     19	   u_gnl_ind_* return (-1) on error.		*/
/*			   mvlas_namelist_resp: send err resp on error	*/
/*			    (instead of empty name list).		*/
/* 08/20/04  JRB     18    Del uninformative "#pragma message".		*/
/* 03/04/02  JRB     17    Avoid uninitialized locals in u_gnl_ind_jous	*/
/* 01/23/02  EJV     16    Added journals to domain:			*/
/*			     u_gnl_ind_jous - added DOM_SPEC case.	*/
/* 09/21/01  JRB     15    Alloc global bufs only once at startup.	*/
/* 03/30/01  MDE     14    Changed MSMEM_ENC_INFO to MSMEM_ENC_OS_INFO	*/
/* 10/25/00  JRB     13    Del GNL funct ptrs. Call functs directly.	*/
/*			   Control with #ifdefs.			*/
/*			   Del NO_GLB_VAR_INIT code (no longer needed).	*/
/* 01/21/00  MDE     12    Now use MEM_SMEM for dynamic memory		*/
/* 09/13/99  MDE     11    Added SD_CONST modifiers			*/
/* 04/07/99  MDE     10    Logging improvements				*/
/* 01/08/99  JRB     09    Use new "bsearch" object model.		*/
/* 11/16/98  MDE     08    Renamed internal functions (prefix '_')	*/
/* 09/21/98  MDE     07    Minor lint cleanup				*/
/* 07/02/98  MDE     06    Added Journal support			*/
/* 06/15/98  MDE     05    Changes to allow compile under C++		*/
/* 05/14/98  JRB     04    Use MVL_IND_PEND. Add mplas resp function.	*/
/* 02/10/98  MDE     03    Added 'u_gnl_done' function pointer		*/
/* 09/18/97  MDE     02    Fixed 'continue after' handling		*/
/* 09/10/97  MDE     01    MMS-LITE V4.0 Release			*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "mmsdefs.h"
#include "mms_pvmd.h"
#include "mvl_defs.h"
#include "mvl_log.h"

/************************************************************************/
/* Approximate worst case PDU overhead					*/

#define MVL_GNL_RESP_OH		25

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/
/************************************************************************/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/
/************************************************************************/

static ST_INT getLocalObjectNames (MVL_OBJ ** obj_tbl, ST_INT num_obj,
			NAMELIST_REQ_INFO *req_info, ST_CHAR **ptr,
			ST_BOOLEAN *moreFollowsOut, ST_INT maxNames);

/************************************************************************/
/************************************************************************/
/*			_mvl_process_getnam_ind				*/
/* A get name indication has been received, process it and respond	*/
/************************************************************************/

#define GETNAM_RESP_BUF_SIZE (sizeof(NAMELIST_RESP_INFO)+ \
			     (MVL_MAX_GNL_NAMES * sizeof(ST_CHAR *)))

ST_VOID _mvl_process_getnam_ind (MVL_IND_PEND *indCtrl)
  {
  indCtrl->u.namelist.req_info =
        (NAMELIST_REQ_INFO *) indCtrl->event->u.mms.dec_rslt.data_ptr;

  u_mvl_namelist_ind (indCtrl);
  }

/************************************************************************/
/*			mvlas_namelist_resp				*/
/* NOTE: u_gnl_ind_* functions return negative value on errors.		*/
/************************************************************************/

ST_VOID mvlas_namelist_resp (MVL_IND_PEND *indCtrl)
  {
NAMELIST_REQ_INFO   *req_info;
NAMELIST_RESP_INFO  *resp_info;
ST_CHAR **ptr;
ST_INT num_obj;
ST_INT i;   
ST_INT max_size;   
ST_INT16 mms_class;

  req_info = indCtrl->u.namelist.req_info;
  mms_class = req_info->obj.mms_class;

/* Now create the response data structure	*/
  resp_info = (NAMELIST_RESP_INFO *) 
		M_CALLOC (MSMEM_ENC_OS_INFO, 1, GETNAM_RESP_BUF_SIZE);
  ptr = (ST_CHAR **)(resp_info + 1);

  num_obj = 0;
  switch (req_info->obj.mms_class)
    {
    case MMS_CLASS_VAR:                		/* NamedVariables 	*/
      num_obj = u_gnl_ind_vars(indCtrl->event->net_info, req_info, ptr, &resp_info->more_follows, 
				MVL_MAX_GNL_NAMES);
    break;

    case MMS_CLASS_VARLIST:                  	/* NamedVariableLists 	*/
      num_obj = u_gnl_ind_nvls(indCtrl->event->net_info, req_info, ptr, &resp_info->more_follows, 
				MVL_MAX_GNL_NAMES);
    break;

    case MMS_CLASS_DOM:                   	/* Domains 		*/
      num_obj = u_gnl_ind_doms(req_info, ptr, &resp_info->more_follows, 
				MVL_MAX_GNL_NAMES);
    break;

    case MMS_CLASS_JOU:	      	/* Journals 		*/
      num_obj = u_gnl_ind_jous(indCtrl->event->net_info, req_info, ptr, &resp_info->more_follows, 
				MVL_MAX_GNL_NAMES);
    break;

    case MMS_CLASS_SCAT_ACC:	/* Scattered Access 	*/
    case MMS_CLASS_TYPE:	/* NamedTypes 		*/
    case MMS_CLASS_SEM:	      	/* Semaphores 		*/
    case MMS_CLASS_EV_COND:     /* Event Conditions 	*/
    case MMS_CLASS_EV_ACT:      /* EventActions 	*/
    case MMS_CLASS_EV_ENROLL:   /* EventEnrollments 	*/
    case MMS_CLASS_PI:	      	/* Program Invocations 	*/
    case MMS_CLASS_OPER_STA:    /* OperatorStations 	*/
    break;

    default:
    break;
    }

  if (num_obj < 0)
    {	/* error. Send error response.	*/
    _mplas_err_resp (indCtrl,7,2); 	/* access, non-existent	*/
    }
  else
    {
    /* Let's make sure they will all fit into the PDU */
    max_size = indCtrl->event->net_info->max_pdu_size - MVL_GNL_RESP_OH;
    resp_info->num_names = 0;  
    for (i = 0; i < num_obj; ++i, ++ptr)
      {
      max_size -= (ST_INT) (2 + strlen (*ptr));
      if (resp_info->num_names > MVL_MAX_GNL_NAMES || max_size <= 0)
        {
        resp_info->more_follows = SD_TRUE;
        break;
        }
      ++resp_info->num_names;
      }

    /* Build a Get Name List response 					*/
    indCtrl->u.namelist.resp_info = resp_info;
    mplas_namelist_resp (indCtrl);
    }

#if defined(MVL_UCA)	/* extra cleanup for UCA	*/
  u_gnl_done (mms_class, resp_info);
#endif			/* defined(MVL_UCA)	*/

  M_FREE (MSMEM_ENC_OS_INFO, resp_info);
  }

/************************************************************************/
/*			mplas_namelist_resp				*/
/************************************************************************/
ST_VOID mplas_namelist_resp (MVL_IND_PEND *indCtrl)
  {
ST_RET rc;

  rc = mpl_namelist_resp (indCtrl->event->u.mms.dec_rslt.id,
                          indCtrl->u.namelist.resp_info);
  if (rc == SD_SUCCESS)
    mpl_resp_log (indCtrl, indCtrl->u.namelist.resp_info);

  _mvl_send_resp_i (indCtrl, rc);
  }

/************************************************************************/
/************************************************************************/

#if defined(MVL_UCA)
  /* This function not used. See customized 'u_gnl_ind_vars' in 'mvl_uca.c'.*/
#elif defined(USE_MANUFACTURED_OBJS)
#pragma message("USE_MANUFACTURED_OBJS defined. Must provide customized 'u_gnl_ind_vars'.")
#else	/* default	*/

ST_INT u_gnl_ind_vars (MVL_NET_INFO *net_info, NAMELIST_REQ_INFO *req_info, 
			     ST_CHAR **ptr, ST_BOOLEAN *moreFollowsOut, 
			     ST_INT maxNames)
  {
ST_INT num_obj;
MVL_VAR_ASSOC **va;
MVL_AA_OBJ_CTRL *aa;
MVL_DOM_CTRL *domCtrl;

/* First get the number of objects and pointer to the object table	*/
  num_obj = 0;
  if (req_info->objscope == VMD_SPEC)
    {
    num_obj = mvl_vmd.num_var_assoc;
    va = mvl_vmd.var_assoc_tbl;
    }
  else if (req_info->objscope == DOM_SPEC)
    {
    domCtrl = mvl_vmd_find_dom (&mvl_vmd, req_info->dname);
    if (domCtrl)
      {
      num_obj = domCtrl->num_var_assoc;
      va = domCtrl->var_assoc_tbl;
      }
    else
      {
      MVL_LOG_NERR1 ("GetNameList variables: Domain '%s' not found", req_info->dname);
      *moreFollowsOut = SD_FALSE;
      return (-1);	/* error. This triggers error response	*/
      }
    }
  else  /* AA_SPEC */
    {
    aa = (MVL_AA_OBJ_CTRL *) net_info->aa_objs;
    if (aa)
      {
      num_obj = aa->num_var_assoc;
      va = aa->var_assoc_tbl;
      }
    else
      {
      *moreFollowsOut = SD_FALSE;
      return (-1);	/* error. This triggers error response	*/
      }
    }

/* Get all the names.	*/
  return (getLocalObjectNames ((MVL_OBJ **) va, num_obj,
                               req_info, ptr, moreFollowsOut, maxNames));
  }
#endif	/* default	*/

/************************************************************************/
/************************************************************************/

#if defined(MVL_UCA)
  /* This function not used. See customized 'u_gnl_ind_nvls' in 'mvl_uca.c'.*/
#elif defined(USE_MANUFACTURED_OBJS)
#pragma message("USE_MANUFACTURED_OBJS defined. Must provide customized 'u_gnl_ind_nvls'.")
#else	/* default	*/

ST_INT u_gnl_ind_nvls (MVL_NET_INFO *net_info, NAMELIST_REQ_INFO *req_info, 
			     ST_CHAR **ptr, ST_BOOLEAN *moreFollowsOut, 
			     ST_INT maxNames)
  {
ST_INT num_obj;
MVL_AA_OBJ_CTRL *aa;
MVL_NVLIST_CTRL **vl;
MVL_DOM_CTRL *domCtrl;

/* First get the number of objects and pointer to the object table	*/
  num_obj = 0;
  if (req_info->objscope == VMD_SPEC)
    {
    num_obj = mvl_vmd.num_nvlist;
    vl = mvl_vmd.nvlist_tbl;
    }
  else if (req_info->objscope == DOM_SPEC)
    {
    domCtrl = mvl_vmd_find_dom (&mvl_vmd, req_info->dname);
    if (domCtrl)
      {
      num_obj = domCtrl->num_nvlist;
      vl = domCtrl->nvlist_tbl;
      }
    else
      {
      MVL_LOG_NERR1 ("GetNameList NVL: Domain '%s' not found", req_info->dname);
      *moreFollowsOut = SD_FALSE;
      return (-1);	/* error. This triggers error response	*/
      }
    }
  else  /* AA_SPEC */
    {
    aa = (MVL_AA_OBJ_CTRL *) net_info->aa_objs;
    if (aa)
      {
      num_obj = aa->num_nvlist;
      vl = aa->nvlist_tbl;
      }
    else
      {
      *moreFollowsOut = SD_FALSE;
      return (-1);	/* error. This triggers error response	*/
      }
    }

/* Get all the names.	*/
  return (getLocalObjectNames ((MVL_OBJ **) vl, num_obj,
                               req_info, ptr, moreFollowsOut, maxNames));
  }
#endif	/* default	*/

/************************************************************************/
/************************************************************************/

#if defined(USE_MANUFACTURED_OBJS)
#pragma message("USE_MANUFACTURED_OBJS defined. Must provide customized 'u_gnl_ind_doms'.")
#else	/* default	*/

ST_INT u_gnl_ind_doms (NAMELIST_REQ_INFO *req_info, 
			     ST_CHAR **ptr, ST_BOOLEAN *moreFollowsOut, 
			     ST_INT maxNames)
  {
/* Get all the names.	*/
  return (getLocalObjectNames ((MVL_OBJ **) mvl_vmd.dom_tbl, mvl_vmd.num_dom,
                               req_info, ptr, moreFollowsOut, maxNames));
  }
#endif	/* default	*/




/************************************************************************/
/************************************************************************/

ST_INT u_gnl_ind_jous (MVL_NET_INFO *net_info, NAMELIST_REQ_INFO *req_info, 
			     ST_CHAR **ptr, ST_BOOLEAN *moreFollowsOut, 
			     ST_INT maxNames)
  {
ST_INT num_obj;
MVL_AA_OBJ_CTRL *aa;
MVL_DOM_CTRL *dom;
MVL_JOURNAL_CTRL **jou;

/* First get the number of objects and pointer to the object table	*/
  num_obj = 0;
  jou = NULL;
  switch (req_info->objscope)
    {
    case VMD_SPEC:
      num_obj = mvl_vmd.num_jou;
      jou = mvl_vmd.jou_tbl;
    break;

    case AA_SPEC:
      aa = (MVL_AA_OBJ_CTRL *) net_info->aa_objs;
      if (aa)
        {
        num_obj = aa->num_jou;
        jou = aa->jou_tbl;
        }
    break;

    case DOM_SPEC:
      dom = mvl_vmd_find_dom (&mvl_vmd, req_info->dname);
      if (dom)
        {
        num_obj = dom->num_jou;
        jou = dom->jou_tbl;
        }
      else
        {
        MVL_LOG_NERR1 ("GetNameList NVL Journal: Domain '%s' not found", req_info->dname);
        *moreFollowsOut = SD_FALSE;
        return (-1);	/* error. This triggers error response	*/
	}
    break;

    default:
      *moreFollowsOut = SD_FALSE;
      return (-1);	/* error. This triggers error response	*/
    }

/* Get all the names.	*/
  if (num_obj)
    return (getLocalObjectNames ((MVL_OBJ **) jou, num_obj,
                               req_info, ptr, moreFollowsOut, maxNames));
  return (0);
  }

/************************************************************************/
/*			getLocalObjectNames				*/
/* Find index into array of pointers for name AFTER continue_after name.*/
/************************************************************************/
static ST_INT getLocalObjectNames (MVL_OBJ ** obj_tbl,
		ST_INT num_obj,
		NAMELIST_REQ_INFO *req_info,
		ST_CHAR **ptr,			/* array of ptrs to names*/
		ST_BOOLEAN *moreFollowsOut,
		ST_INT maxNames)		/* max # of names	*/
  {
ST_INT obj_index;	/* index to array of objects	*/
ST_INT name_index;	/* index to array of names	*/
MVL_OBJ **key;

  if (req_info->cont_after_pres)
    {
    key = mvl_obj_find_key (obj_tbl, num_obj, req_info->continue_after);
    if (key)
      obj_index = (int) (key - obj_tbl + 1);	/* index to next one (i.e. + 1).*/
    else
      {
      MVL_LOG_ERR1 ("GetNameList Error: continue after name '%s' not found",
                    req_info->continue_after);
      obj_index = num_obj;		/* Treat as if there are no more objects.*/
      }
    }
  else
    obj_index = 0;		/* No continue_after. Start with first entry.	*/

/* Now make the list for the response */
  for (name_index = 0; name_index < maxNames && obj_index < num_obj;
       ++obj_index, ++name_index)
    ptr[name_index] = obj_tbl[obj_index]->name;

  if (obj_index < num_obj)
    *moreFollowsOut = SD_TRUE;

  return (name_index);
  }
