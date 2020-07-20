/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1993 - 1997, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : mms_err2.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains the MMS error functions for decoding	*/
/*	& encoding the error response PDUs :				*/
/*		initiate-ErrorPDU 					*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 03/09/05  EJV     06    Fixed thisFileName for MMS-EASE              */
/* 03/17/04  RKR     05    Changed thisFileName                         */
/* 03/11/04  GLB     04    Remove "thisFileName"                    	*/
/* 09/13/99  MDE     02    Added SD_CONST modifiers			*/
/* 06/15/98  MDE     01    Changes to allow compile under C++		*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "glbsem.h"

#include "mmsdefs.h"
#include "mms_pcon.h"

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/

#if defined(DEBUG_SISCO) && !defined(MMS_LITE)
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

#ifndef MMS_LITE

#include "mms_vcon.h"

/************************************************************************/
/*			mp_init_err					*/
/* Create and send an initiate error response				*/
/************************************************************************/

ST_RET mp_init_err (MMSREQ_IND *indptr, ST_INT16 class, ST_INT16 code)
  {
INIT_INFO *init_req;
INIT_INFO *init_resp;
CS_INIT_INFO *cs_init_resp;
ST_INT size;
ST_INT i;
ST_INT ret;

  S_LOCK_COMMON_RESOURCES ();
  init_req = (INIT_INFO *) indptr->req_info_ptr;

/* Allocate the INITIATE response info, make big enough for all CS init	*/
  size = sizeof (INIT_INFO) + 
		(MAX_INIT_CONTEXTS * sizeof (CS_INIT_INFO));
  init_resp = (INIT_INFO *) chk_calloc (1, size);
  cs_init_resp = (CS_INIT_INFO *) (init_resp+1);

  if (!(ret = _ms_check_ind_actv (indptr)))
    {
    if (init_req->mms_p_context_pres)
      {	
      init_resp->rslt.accept_context = SD_FALSE;
      init_resp->rslt.err_info_pres = SD_TRUE;
      init_resp->rslt.err.eclass = class;
      init_resp->rslt.err.code = code;
      }

    for (i = 0; i < init_req->num_cs_init; ++i, ++cs_init_resp)
      {
      cs_init_resp->rslt.accept_context = SD_FALSE;
      cs_init_resp->rslt.err_info_pres = SD_TRUE;
      cs_init_resp->rslt.err.eclass = class;
      cs_init_resp->rslt.err.code = code;
      }	/* for each CS context */

    mv_chk_init_resp_param (indptr,init_resp);
    ret = mp_init_resp (indptr,init_resp);
    }

  chk_free (init_resp);
  S_UNLOCK_COMMON_RESOURCES ();
  return (ret);
  }

#else
/************************************************************************/
/*			mpl_init_err					*/
/* Create an initiate error response					*/
/************************************************************************/

ST_RET mpl_init_err (ST_INT16 err_class, ST_INT16 code)
  {
  return (mpl_err_resp (MMSOP_INITIATE,0L,err_class,code));
  }
#endif
