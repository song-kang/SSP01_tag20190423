/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	2005 - 2005, All Rights Reserved		        	*/
/*									*/
/* MODULE NAME : reject.c						*/
/* PRODUCT(S)  : MMSEASE-LITE						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*	Default reject indication function. This module may be linked	*/
/*	to any application, or the function may be copied to a user	*/
/*	module and modified as needed.					*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*			u_mvl_reject_ind				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 09/07/05  JRB     01    Created					*/
/************************************************************************/
#include "glbtypes.h"
#include "sysincs.h"
#include "mvl_defs.h"
#include "mvl_acse.h"
#include "mvl_log.h"


#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/
/************************************************************************/
#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/
/*			u_mvl_reject_ind				*/
/************************************************************************/
ST_VOID u_mvl_reject_ind (MVL_NET_INFO *net_info, REJECT_RESP_INFO *rej_info)
  {
  if (rej_info->rej_class == MMS_REJ_CLASS_CONFIRMED_REQUEST_PDU &&
      rej_info->rej_code  == MMS_REJ_CODE_CONFIRMED_REQUEST_PDU_UNRECOGNIZED_SERVICE)
    {	/* This is most common case. Provide extra information in log msgs.*/
    if (rej_info->detected_here)
      MVL_LOG_ERR0 ("MMS Reject sent, unrecognized service. Check MMS services enabled here.");
    else
      MVL_LOG_ERR0 ("MMS Reject received, unrecognized service. Check MMS services enabled on remote device.");
    }
  else
    {
    /* NOTE: for all other cases, it is best to abort the connection	*/
    /* but it is not done in this callback function because no good	*/
    /* way to inform main application that abort occurred.		*/
    /* DEBUG: should there be extra handshaking in MVL_NET_INFO struct?	*/
    if (rej_info->detected_here)
      MVL_LOG_ERR0 ("Reject sent.");
    else
      MVL_LOG_ERR0 ("Reject received.");
    }
  }


#ifdef __cplusplus
}
#endif
