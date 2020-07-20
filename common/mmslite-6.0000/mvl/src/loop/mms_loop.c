/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	1986 - 1993, All Rights Reserved		        */
/*									*/
/* MODULE NAME : mms_loop.c    						*/
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
/* 12/22/97  MDE     03    Changed 'mvl_net_service' to report activity	*/
/* 11/11/96  MDE     02    Misc minor cleanups				*/
/* 01/21/94  MDE     01    Initial                       		*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "assert.h"

#include "mmsdefs.h"
#include "mms_pvar.h"
#include "mms_vvar.h"
#include "mms_err.h"
#include "mms_pcon.h"
#include "asn1defs.h"

#include "mvl_defs.h"

/************************************************************************/

static ST_INT16  rx_msg_rcvd = SD_FALSE;
static ST_UCHAR *rx_msg_ptr;
static ST_INT16  rx_msg_len;

static MVL_COMM_EVENT *mvl_get_com_event (ST_VOID);

/************************************************************************/
/* Functions to allow loopback testing					*/
/************************************************************************/

/* These functions need to be replaced with the real thing		*/

ST_BOOLEAN mvl_net_service (MVL_COMM_EVENT **eventOut);
  {
  if (rx_msg_rcvd)
    {
    rx_msg_rcvd = SD_FALSE;
    *eventOut = mvl_get_com_event ();
    (*eventOut)->event_type = MMS_MSG_RCVD;
    (*eventOut)->u.mms.pdu = rx_msg_ptr;
    (*eventOut)->u.mms.pdu_len = rx_msg_len;
    return (SD_TRUE);
    }
  
  *eventOut = NULL;
  return (SD_FALSE);
  }

/************************************************************************/
/************************************************************************/
/* Outstanding MVL_COMM_EVENT control buffer functions			*/

/* NOTE : This is a trivial implementation that allows only one		*/
/* outstanding request at a time. These routines can easily be replaced	*/
/* by simple list routines						*/
  
static MVL_COMM_EVENT a_comm_event;
static ST_INT16 a_comm_event_used;

static MVL_COMM_EVENT *mvl_get_com_event (ST_VOID)
  {
  if (!a_comm_event_used)
    {
    a_comm_event_used = SD_TRUE;
    return (&a_comm_event);
    }
  return (NULL);
  }

/************************************************************************/

ST_VOID mvl_free_comm_event (MVL_COMM_EVENT *event)
  {
  a_comm_event_used = SD_FALSE;
  }
  
/************************************************************************/
/************************************************************************/

ST_RET mvl_send_msg (ST_UCHAR *pdu, ST_INT pdu_len, MVL_NET_INFO *net_info)
  {
/* send it to us							*/
  rx_msg_rcvd = SD_TRUE;
  rx_msg_ptr = pdu;
  rx_msg_len = pdu_len;
  return (SD_SUCCESS);
  }

