/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,       */
/*              1994 - 1998 All Rights Reserved                         */
/*                                                                      */
/* MODULE NAME : rsevent.c                                              */
/* PRODUCT(S)  : TP4/RS API event handling module			*/
/*                                                                      */
/* MODULE DESCRIPTION : 						*/
/*                                                                      */
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :                            */
/*                                                                      */
/* MODIFICATION LOG :                                                   */
/*  Date     Who   Rev                  Comments                        */
/* --------  ---  ------   -------------------------------------------  */
/* 09/13/99  MDE    08     Added SD_CONST modifiers			*/
/* 02/05/99  JRB    07     Make get_rs_event static, ret BOOLEAN, and	*/
/*			   only ret FALSE if adlcGetEvent ret NULL.	*/
/*			   tp4_event ret same as get_rs_event.		*/
/*			   This way, even if event thrown out, caller	*/
/*			   can always tell if there was one or not.	*/
/* 12/21/98  EJV    06     Rearranged code & fixed missing events problem*/
/* 10/08/98  MDE    05     Minor logging cleanup			*/
/* 08/13/98  JRB    04     Lint cleanup.				*/
/* 06/22/98  JRB    03     Allow ADLC_CMD_WRITE_PACKET_DONE & ignore it.*/
/*			   Fix illegal log msg.				*/
/* 10/09/97  EJV    02     Added UCA SMP TimeSync support.		*/
/* 06/04/97  JRB    01     Free data for WRITE_PACKET after processing.	*/
/* 05/27/97  JRB   7.00    MMSEASE 7.0 release.				*/
/* 07/22/96  MDE    04     Changes towards supporting A-Unit Data	*/
/* 06/20/96  KCR    03     free TP4 connection after tp4_disconnect_ind */
/* 05/21/96  MDE    02     Check for duplicate connections		*/
/* 03/28/96  MDE    01     Created from tp4event.c                      */
/************************************************************************/

static char *thisFileName = __FILE__;
#define THISFILE thisFileName



#ifdef __OS2__
#pragma data_seg( alldata )
#define INCL_BASE
#define INCL_DOS
#define INCL_DOSMEMMGR
#define INCL_ERRORS
#define INCL_DOSPROCESS
#define INCL_DOSQUEUES
#define INCL_DOSSEMAPHORES

/* We may need to define others as we determine what we need */
#include <os2.h>
#endif									  

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <process.h>
#include "glbtypes.h"
#include "glberror.h"
#include "gen_list.h"
#include "mem_chk.h"
#include "slog.h"

#ifdef __OS2__
#include "os2_err.h"
#include "rli_ipc.h"
#include "rli_ipcl.h"
#include "rsgw_log.h"
#include "adlcapi.h"
#endif
#include "adlc_usr.h"
#include "adlc_log.h"
#include "tp4api.h"
#include "rs_defs.h"

#if defined(UCA_SMP)
#include "smp_usr.h"
#endif

/************************************************************************/

static ST_VOID handleAdlcConnectInd (ST_VOID *vp, ST_INT op, ST_RET rslt);
static ST_VOID handleAdlcWriteInd (ST_VOID *vp, ST_INT op, ST_RET rslt);
static ST_VOID handleAdlcExtWriteInd (ST_VOID *vp, ST_INT op, ST_RET rslt);
static ST_VOID handleAdlcExtWriteDone (ST_VOID *vp, ST_INT op, ST_RET rslt);
static ST_VOID handleAdlcDisconnectInd (ST_VOID *vp, ST_INT op, ST_RET rslt);
static ST_VOID handleAdlcConnectDone (ST_VOID *vp, ST_INT op, ST_RET rslt);
static ST_VOID handleAdlcDisconnectDone (ST_VOID *vp, ST_INT op, ST_RET rslt);
static REGINFO *findRegInfo (ST_UINT16 adlcAddr);
static CONNINFO *adlcAddrToConn (ST_UINT16 remote, ST_UINT16 local);
static ST_BOOLEAN get_rs_event (ST_VOID);

/************************************************************************/
/*			tp4_event					*/
/* The user calls this function to retrieve TP4 events from the stack	*/
/* The function returns SD_TRUE if an event was retrieved and SD_FALSE	*/
/* otherwise.								*/
/************************************************************************/

ST_BOOLEAN tp4_event ()
  {
ADLC_WRITE_PACKET    *adlcWriteInfo;
#if defined(UCA_SMP)
ADLC_EXT_WRITE_PACKET *adlcExtWriteInfo;
#endif
ST_BOOLEAN retcode;	/* Return same value as get_rs_event	*/

/* If not one waiting, try to get one frm ADLC 				*/
  if (!co_rs_event)
    retcode = get_rs_event ();

/* Check to see if we have a CO event waiting for us			*/
  if (!co_rs_event)
    return (retcode);

  switch (co_rs_op)
    {
/* Indications								*/
    case ADLC_CMD_WRITE_PACKET :
      handleAdlcWriteInd (co_rs_event, co_rs_op, co_rs_rslt);
      adlcWriteInfo = (ADLC_WRITE_PACKET *) co_rs_event;
      chk_free (adlcWriteInfo->i.data);
    break;

    case ADLC_CMD_CONNECT_NODE :
      handleAdlcConnectInd (co_rs_event, co_rs_op, co_rs_rslt);
    break;

    case ADLC_CMD_DISCONNECT_NODE :
      handleAdlcDisconnectInd (co_rs_event, co_rs_op, co_rs_rslt);
    break;

/* Command Completions							*/
    case ADLC_CMD_CONNECT_NODE_DONE :
      handleAdlcConnectDone (co_rs_event, co_rs_op, co_rs_rslt);
    break;

    case ADLC_CMD_DISCONNECT_NODE_DONE :
      handleAdlcDisconnectDone (co_rs_event, co_rs_op, co_rs_rslt);
    break;

#if defined(UCA_SMP)
    case ADLC_CMD_EXT_WRITE_PACKET :
      handleAdlcExtWriteInd (co_rs_event, co_rs_op, co_rs_rslt);
      adlcExtWriteInfo = (ADLC_EXT_WRITE_PACKET *) co_rs_event;
      chk_free (adlcExtWriteInfo->i.data);
    break;

    case ADLC_CMD_EXT_WRITE_PACKET_DONE :
      handleAdlcExtWriteDone (co_rs_event, co_rs_op, co_rs_rslt);
    break;
#endif  /* UCA_SMP */
    }

  chk_free (co_rs_event);
  co_rs_event = NULL;
  return (retcode);
  }

/************************************************************************/
/************************************************************************/
/*			 handleAdlcConnectInd 				*/
/************************************************************************/

static ST_VOID handleAdlcConnectInd (ST_VOID *vp, ST_INT op, ST_RET rslt)
  {
ADLC_CONNECT_NODE    *adlcConnectInfo;
CONNINFO *conn;
REGINFO *reg;
ADLC_DISCONNECT_NODE adlcDiscInfo;
TP4_ADDR tp4_addr;
ST_RET ret;

  adlcConnectInfo = (ADLC_CONNECT_NODE *) vp;
  SFLOWUP0 ("Connect Indication received");
  SFLOWUP2C ("Local ADLC address 0x%X, remote ADLC address 0x%X",
  			adlcConnectInfo->i.localAddr,
 			adlcConnectInfo->i.remoteAddr);


  reg = findRegInfo (adlcConnectInfo->i.localAddr);
  if (!reg)
    {
    SERR1 ("No bind for ADLC address 0x%X, connect indication rejected",
			adlcConnectInfo->i.localAddr);
    adlcDiscInfo.i.localAddr = adlcConnectInfo->i.localAddr;
    adlcDiscInfo.i.remoteAddr = adlcConnectInfo->i.remoteAddr;
    ret = adlcDisconnectNode(&adlcDiscInfo);
    if (ret != SD_SUCCESS)
      {
      SERR1("adlcDisconnectNode Error 0x%04x", ret);
      }
    return;
    }

/* Make sure this source/dest address pair is not already in use	*/
  conn = adlcAddrToConn (adlcConnectInfo->i.remoteAddr, 
			 adlcConnectInfo->i.localAddr);
  if (conn)
    {
    SERR0 ("Error : Connect Ind for existing Local/Remote addr pair");
    SERR2C ("Local ADLC address 0x%X, remote ADLC address 0x%X",
  			adlcConnectInfo->i.localAddr,
 			adlcConnectInfo->i.remoteAddr);

    adlcDiscInfo.i.localAddr = adlcConnectInfo->i.localAddr;
    adlcDiscInfo.i.remoteAddr = adlcConnectInfo->i.remoteAddr;
    ret = adlcDisconnectNode(&adlcDiscInfo);
    if (ret != SD_SUCCESS)
      {
      SERR1("adlcDisconnectNode Error 0x%04x", ret);
      }
    return;
    }

/* OK, create a new Connection Control element				*/
  conn = (CONNINFO *) chk_calloc (1, sizeof (CONNINFO));
  list_add_last ((ST_VOID**) &conn_list, conn);
  conn->reginfo = reg;
  conn->localAddr = adlcConnectInfo->i.localAddr;
  conn->remAddr = adlcConnectInfo->i.remoteAddr;

  tp4_addr.adlcAddr = conn->remAddr;
  tp4_connect_ind (reg->user_bind_id, (ST_LONG) conn, &tp4_addr, 0, NULL);
  }

/************************************************************************/
/*			handleAdlcWriteInd 				*/
/************************************************************************/

static ST_VOID handleAdlcWriteInd (ST_VOID *vp, ST_INT op, ST_RET rslt)
  {
ADLC_WRITE_PACKET    *adlcWriteInfo;
CONNINFO *conn;
ST_UCHAR *data;

  adlcWriteInfo = (ADLC_WRITE_PACKET *) vp;
  SFLOWUP2 ("CO ADLC Write Ind rcvd for Local ADLC address 0x%X, remote ADLC address 0x%X",
  			adlcWriteInfo->i.localAddr,
 			adlcWriteInfo->i.remoteAddr);

  conn = adlcAddrToConn (adlcWriteInfo->i.remoteAddr, 
  		 adlcWriteInfo->i.localAddr);
  if (!conn)
    {
    SERR2 ("Connection not found for remote = 0x%X, local = 0x%X",
  				adlcWriteInfo->i.remoteAddr, 
  				adlcWriteInfo->i.localAddr);
    return;
    }
  
  data = (ST_UCHAR *) adlcWriteInfo->i.data;
  if (adlcWriteInfo->i.dataSize < 3 || data[0] != rs_co_dest || 
      data[1] != rs_co_src || data[2] != rs_co_qos)
    {
    SERR0 ("Network Address Error, dropping data ...");
    }
  
  SFLOWUP1C ("Connect ID %ld", conn->user_conn_id);
  SFLOWUPH (adlcWriteInfo->i.dataSize, adlcWriteInfo->i.data);
  
/* Pass the data up to the TP4 user					*/
  SFLOWUP0C ("Pass up as tp4_data_ind");
  tp4_data_ind (conn->user_conn_id, SD_TRUE, 
  	    adlcWriteInfo->i.dataSize -3, (ST_CHAR *) data +3);
  }

#if defined(UCA_SMP)
/************************************************************************/
/*			handleAdlcExtWriteInd 				*/
/************************************************************************/
static ST_VOID handleAdlcExtWriteInd (ST_VOID *vp, ST_INT op, ST_RET rslt)
  {
ADLC_EXT_WRITE_PACKET *adlcExtWriteInfo;
ST_UCHAR              *data;   
CONNINFO              *conn;
SMP_DATAUNIT           smp_du;


  adlcExtWriteInfo = (ADLC_EXT_WRITE_PACKET *) vp;
  SFLOWUP2 ("CO ADLC Ext Write Ind rcvd for Local ADLC address 0x%X, remote ADLC address 0x%X",
  			adlcExtWriteInfo->i.localAddr,
 			adlcExtWriteInfo->i.remoteAddr);

  conn = adlcAddrToConn (adlcExtWriteInfo->i.remoteAddr, 
	  		 adlcExtWriteInfo->i.localAddr);
  if (!conn)
    {
    SERR2 ("Connection not found for remote = 0x%X, local = 0x%X, ignoring data...",
  				adlcExtWriteInfo->i.remoteAddr, 
  				adlcExtWriteInfo->i.localAddr);
    return;
    }

  data = (ST_UCHAR *) adlcExtWriteInfo->i.data;

  SFLOWUP1C ("Connect ID %ld", conn->user_conn_id);
  SFLOWUPH (adlcExtWriteInfo->i.dataSize, adlcExtWriteInfo->i.data);

  smp_du.pdu_buf = &data[3];
  smp_du.pdu_len = adlcExtWriteInfo->i.dataSize-3;
  smp_du.loc_mac = (ST_UCHAR *) &adlcExtWriteInfo->i.localAddr;  /* passing pointer */
  smp_du.rem_mac = (ST_UCHAR *) &adlcExtWriteInfo->i.remoteAddr; /* passing pointer */
  /* the rec_time buf is appended on the end of data buf */
  smp_du.pdu_len = smp_du.pdu_len - SMP_SIZE_OF_REC_TIME; /* true PDU len */
  smp_du.rec_time = &smp_du.pdu_buf [smp_du.pdu_len];

  /* Pass the data up to the SMP module */
  SFLOWUP0C ("Pass up as smp_decode_uca_pdu function.");
  smp_decode_uca_pdu (&smp_du);
  }

/************************************************************************/
/*			handleAdlcExtWriteDone 				*/
/************************************************************************/
static ST_VOID handleAdlcExtWriteDone (ST_VOID *vp, ST_INT op, ST_RET rslt)
  {
ADLC_EXT_WRITE_PACKET_DONE *adlcExtWriteDoneInfo;

  adlcExtWriteDoneInfo = (ADLC_EXT_WRITE_PACKET_DONE *) vp;
  SFLOWUP2 ("CO ADLC Ext Write Done rcvd for Local ADLC address 0x%X, remote ADLC address 0x%X",
  			adlcExtWriteDoneInfo->i.localAddr,
 			adlcExtWriteDoneInfo->i.remoteAddr);

  /* Call directly the SMP function to process this event 	*/
  smp_send_pdu_callback (rslt,
                         adlcExtWriteDoneInfo->i.userId,
                         (ST_UCHAR *) adlcExtWriteDoneInfo->i.recTime,
                         sizeof (adlcExtWriteDoneInfo->i.recTime));
  }
#endif  /* UCA_SMP */

/************************************************************************/
/*			handleAdlcDisconnectInd 			*/
/************************************************************************/

static ST_VOID handleAdlcDisconnectInd (ST_VOID *vp, ST_INT op, ST_RET rslt)
  {
ADLC_DISCONNECT_NODE *adlcDisconnectInfo;
CONNINFO *conn;

  adlcDisconnectInfo = (ADLC_DISCONNECT_NODE *) vp;
  SFLOWUP2("ADLC Disconnect Ind rcvd for Local ADLC address 0x%X, remote ADLC address 0x%X",
 			adlcDisconnectInfo->i.localAddr,
  			adlcDisconnectInfo->i.remoteAddr);

  conn = adlcAddrToConn (adlcDisconnectInfo->i.remoteAddr, 
			 adlcDisconnectInfo->i.localAddr);
  if (!conn)		/* May already have been removed */
    {
    SFLOWUP0C ("Connection not found");
    return;
    }
  SFLOWUP1C ("User connect ID %ld", conn->user_conn_id);

  tp4_disconnect_ind (conn->user_conn_id, 1, 0, NULL);
  list_unlink ((ST_VOID**) &conn_list, conn);
  chk_free (conn);
  }

/************************************************************************/
/*			handleAdlcConnectDone 				*/
/************************************************************************/

static ST_VOID handleAdlcConnectDone (ST_VOID *vp, ST_INT op, ST_RET rslt)
  {
ADLC_CONNECT_NODE    *adlcConnectInfo;
CONNINFO *conn;

  adlcConnectInfo = (ADLC_CONNECT_NODE *) vp;
  SFLOWUP2 ("ADLC Connect Done rcvd for Local ADLC address 0x%X, remote ADLC address 0x%X",
  			adlcConnectInfo->i.localAddr,
 			adlcConnectInfo->i.remoteAddr);

  conn = adlcAddrToConn (adlcConnectInfo->i.remoteAddr, 
			 adlcConnectInfo->i.localAddr);
  if (!conn)
    {
    SERR2 ("Connection not found for remote = 0x%X, local = 0x%X",
 					adlcConnectInfo->i.remoteAddr, 
					adlcConnectInfo->i.localAddr);
    return;
    }
  else if (conn->connFlag == SD_TRUE)
    {
    SERR2 ("Duplicate Connect Done Received for remote = 0x%X, local = 0x%X",
 					adlcConnectInfo->i.remoteAddr, 
					adlcConnectInfo->i.localAddr);
    return;
    }

  SFLOWUP1C ("User connect ID %ld", conn->user_conn_id);
  if (rslt == SD_SUCCESS)
    {
    conn->connFlag = SD_TRUE;   /* Connect Done Processed Flag */
    tp4_connect_cnf (conn->user_conn_id, (ST_LONG) conn, 0, NULL);
    }
  else
    {
    tp4_disconnect_ind (conn->user_conn_id, 1, 0, NULL);
    list_unlink ((ST_VOID**) &conn_list, conn);
    chk_free (conn);
    }
  }

/************************************************************************/
/*			handleAdlcDisconnectDone 			*/
/************************************************************************/

static ST_VOID handleAdlcDisconnectDone (ST_VOID *vp, ST_INT op, ST_RET rslt)
  {
ADLC_DISCONNECT_NODE *adlcDisconnectInfo;

  adlcDisconnectInfo = (ADLC_DISCONNECT_NODE *) vp;
  SFLOWUP2 ("ADLC Disconnect Done rcvd for Local ADLC address 0x%X, remote ADLC address 0x%X",
  			adlcDisconnectInfo->i.localAddr,
 			adlcDisconnectInfo->i.remoteAddr);
  }

/************************************************************************/
/*				findRegInfo				*/
/************************************************************************/

static REGINFO *findRegInfo (ST_UINT16 adlcAddr)
  {
REGINFO *reg;

  /* Verify that this address is not used				*/
  reg = reg_list;
  while (reg)
    {
    if (reg->localTp4Addr.adlcAddr == adlcAddr)
      break;
    reg = (REGINFO *) list_get_next (reg_list, reg);
    }
  return (reg);  
  }

/************************************************************************/
/*			adlcAddrToConn					*/
/************************************************************************/

static CONNINFO *adlcAddrToConn (ST_UINT16 remote, ST_UINT16 local)
  {
CONNINFO *conn;

  /* Verify that this address is not used				*/
  conn = conn_list;
  while (conn)
    {
    if (conn->localAddr == local && conn->remAddr == remote)
      break;
    conn = (CONNINFO *) list_get_next (conn_list, conn);
    }

  return (conn);  
  }

/************************************************************************/
/************************************************************************/

SD_CONST ST_UCHAR rs_co_dest = 0xF7;
SD_CONST ST_UCHAR rs_co_src  = 0xF7;
SD_CONST ST_UCHAR rs_co_qos  = 0x03;

SD_CONST ST_UCHAR rs_cl_dest = 0xF5;
SD_CONST ST_UCHAR rs_cl_src  = 0xF5;
SD_CONST ST_UCHAR rs_cl_qos  = 0x03;

ST_VOID   *co_rs_event;
ST_INT co_rs_op;
ST_RET co_rs_rslt;

ST_VOID   *cl_rs_event;
ST_INT cl_rs_op;
ST_RET cl_rs_rslt;

/************************************************************************/
/************************************************************************/

static ST_BOOLEAN get_rs_event (ST_VOID)
  {
ST_VOID *vp;
ST_INT op;
ST_RET rslt;
ADLC_WRITE_PACKET    *adlcWriteInfo;
#if defined(UCA_SMP)
ADLC_EXT_WRITE_PACKET *adlcExtWriteInfo;
#endif
ST_UCHAR *data;

/* See if ADLC has something for us, give it some processing time	*/
  vp = adlcGetEvent (&op, &rslt);
  if (!vp)
    return (SD_FALSE);

  SFLOWUP2 ("ADLC Event received, opcode %d, result %d", op, rslt);

  switch (op)
    {
/* Indications								*/
    case ADLC_CMD_WRITE_PACKET :
      adlcWriteInfo = (ADLC_WRITE_PACKET *) vp;
      data = (ST_UCHAR *) adlcWriteInfo->i.data;

      if (adlcWriteInfo->i.dataSize > 3 && data[0] == rs_co_dest)
        {
        if (!co_rs_event)
          {
          co_rs_event = vp;
          co_rs_op = op;
          co_rs_rslt = rslt;
          }
        else
          {
          SERR0 ("CO RS EVENT Lost, must service CO/CL sequentially");
          }
        }
      else if (adlcWriteInfo->i.dataSize > 3 && data[0] == rs_cl_dest)
        {
        if (!cl_rs_event)
          {
          cl_rs_event = vp;
          cl_rs_op = op;
          cl_rs_rslt = rslt;
          }
        else
          {
          SERR0 ("CL RS EVENT Lost, must service CO/CL sequentially");
          }
        }
      else
        {
        SFLOWUP1 ("Network Address Error, discarding (0x%02x)", 
						(unsigned int) data[0]);
        chk_free (data);
        chk_free (vp);
        }
    break;

    case ADLC_CMD_CONNECT_NODE :
    case ADLC_CMD_DISCONNECT_NODE :
    case ADLC_CMD_CONNECT_NODE_DONE :
    case ADLC_CMD_DISCONNECT_NODE_DONE :
    case ADLC_CMD_WRITE_PACKET_DONE :
      co_rs_event = vp;
      co_rs_op = op;
      co_rs_rslt = rslt;
    break;

#if defined(UCA_SMP)
    case ADLC_CMD_EXT_WRITE_PACKET :
      adlcExtWriteInfo = (ADLC_EXT_WRITE_PACKET *) vp;
      data = (ST_UCHAR *) adlcExtWriteInfo->i.data;

      if (adlcExtWriteInfo->i.dataSize > 3 &&
          data[0] == 0xFB && data[1] == 0xFB) /* check dest and source addr */
        {
        if (!co_rs_event)
          {
          co_rs_event = vp;
          co_rs_op = op;
          co_rs_rslt = rslt;
          }
        else
          {
          SERR0 ("CO RS EVENT Lost (Ext Write Ind), must service CO/CL sequentially");
          }
        }
      else
        {
        SERR1 ("Network Address Error, discarding (0x%02x)", 
						(unsigned int) data[0]);
        chk_free (data);
        chk_free (vp);
        }
    break;

    case ADLC_CMD_EXT_WRITE_PACKET_DONE :
      if (!co_rs_event)
        {
        co_rs_event = vp;
        co_rs_op = op;
        co_rs_rslt = rslt;
        }
      else
        {
        SERR0 ("CO RS EVENT Lost (Ext Write Done), must service CO/CL sequentially");
        }
    break;
#endif  /* UCA_SMP */

    default :
      SERR1 ("Unknown ADLC OPCODE %d", op);
      chk_free (vp);
    break;
    }
  return (SD_TRUE);
  }
