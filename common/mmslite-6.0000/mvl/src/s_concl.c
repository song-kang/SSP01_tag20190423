/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	1994 - 1996, All Rights Reserved		        */
/*									*/
/* MODULE NAME : s_concl.c    						*/
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
/* 04/13/10  JRB	   mplas_concl_err: free event when done.	*/
/* 03/03/10  JRB	   Replace "ml_log_*" with "mlogl_info" struct.	*/
/* 05/14/08  JRB     07    Add mplas_concl_err.				*/
/* 05/01/02  MDE     06    Added MLOG logging				*/
/* 10/25/00  JRB     05    Pass ind up to user. User must call resp fct	*/
/* 09/13/99  MDE     04    Added SD_CONST modifiers			*/
/* 11/16/98  MDE     03    Renamed internal functions (prefix '_')	*/
/* 06/15/98  MDE     02    Changes to allow compile under C++		*/
/* 09/10/97  MDE     01    MMS-LITE V4.0 Release			*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "mmsdefs.h"
#include "mvl_defs.h"
#include "mvl_log.h"
#include "mloguser.h"


/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/
/************************************************************************/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif


/************************************************************************/
/************************************************************************/
/*			_mvl_process_concl_ind				*/
/* A Conclude indication has been received. Pass it up to user.		*/
/************************************************************************/

ST_VOID _mvl_process_concl_ind (MVL_COMM_EVENT *event)
  {
  /* Pass up to user for processing. User must call mplas_concl_resp	*/
  /* to send Conclude response.						*/
  u_mvl_concl_ind (event);
  }


/************************************************************************/
/*			_mvl_process_concl_ind				*/
/* Send Conclude response.						*/
/************************************************************************/
ST_VOID mplas_concl_resp (MVL_COMM_EVENT *event)
  {
ST_RET rc;

/* We can log here if so desired */
  if ((mms_debug_sel & MMS_LOG_RESP) && mlogl_info.resp_info_fun != NULL)
      (*mlogl_info.resp_info_fun) (0, MMSOP_CONCLUDE, NULL);

/* Send a Conclude response 						*/
  rc = _mvl_send_msg ((ST_UCHAR *) "\x8C\x00",2,event->net_info);

  if (rc != SD_SUCCESS)
    MVL_LOG_NERR1 ("Conclude Indication : _mvl_send_msg failed, ret = %x", rc);
  _mvl_free_comm_event (event);		/* All done with this event	*/
  }

/************************************************************************/
/*			mplas_concl_err					*/
/* Send Conclude error response.					*/
/* NOTE: err_class should always be 9, and err_code should be 0 or 1.	*/
/************************************************************************/
ST_RET mplas_concl_err (MVL_COMM_EVENT *event, ST_INT16 err_class, ST_INT16 err_code)
  {
ST_RET rc;
  rc = mpl_conclude_err (err_class,err_code);	/* encode Conclude-ErrorPDU*/
  if (rc)
    {	/* Should never happen. No good recovery.	*/
    MVL_LOG_ERR1 ("Error encoding Conclude-ErrorPDU, rc = %x", rc);
    }
  else
    {		/* send it	*/
    rc = _mvl_send_msg (mmsl_msg_start,mmsl_msg_len,event->net_info);
    if (rc != SD_SUCCESS)
      MVL_LOG_ERR1 ("Error sending Conclude-ErrorPDU, rc = %x", rc);
    }
  _mvl_free_comm_event (event);		/* All done with this event	*/
  return (rc);
  }
