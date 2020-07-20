/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	2001-2001, All Rights Reserved					*/
/*									*/
/* MODULE NAME : s_ustat.c   						*/
/* PRODUCT(S)  : MMSEASE-LITE						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	Send "Unsolicited Status" request.				*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*	mvl_ustatus							*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 01/30/06  GLB     04    Integrated porting changes for VMS           */
/* 12/12/02  JRB     03    Use new thread-safe mpl_ustatus,_mvl_send_msg*/
/*			   and skip S_LOCK..				*/
/* 09/21/01  JRB     02    Alloc global bufs only once at startup.	*/
/* 03/15/01  JRB     01    Created.					*/
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
/************************************************************************/

ST_RET mvl_ustatus (MVL_NET_INFO *net_info, USTATUS_REQ_INFO *req_info)
  {
ST_RET rc;
ASN1_ENC_CTXT aCtx;
ST_UCHAR *my_enc_buf;

  my_enc_buf = (ST_UCHAR *) mvl_enc_buf_alloc (mmsl_enc_buf_size);

  /* Build the request	*/
  asn1r_strt_asn1_bld (&aCtx, my_enc_buf,mmsl_enc_buf_size);
  rc = mpl_ustatus (&aCtx, req_info);
  if (rc != SD_SUCCESS)
    {
    MVL_LOG_ERR1 ("mpl_ustatus failed, ret = 0x%X", rc);
    }
  else
    {  
    rc = _mvl_send_msg (ASN1_ENC_PTR(&aCtx),ASN1_ENC_LEN(&aCtx),net_info);
    if (rc != SD_SUCCESS)
      {
      MVL_LOG_ERR1 ("_mvl_send_msg failed, ret = 0x%X", rc);
      }
    else
      ++mvl_mms_statistics.serverUstatus;
    }

  mvl_enc_buf_free (my_enc_buf);
  return(rc);  
  }                                  
