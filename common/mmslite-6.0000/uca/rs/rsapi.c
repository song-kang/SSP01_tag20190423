/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,       */
/*              1994-2003 All Rights Reserved                           */
/*                                                                      */
/* MODULE NAME : rsapi.c                                             	*/
/* PRODUCT(S)  : TP4/RS API main module					*/
/*                                                                      */
/* MODULE DESCRIPTION : 						*/
/*                                                                      */
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :                            */
/*                                                                      */
/* MODIFICATION LOG :                                                   */
/*  Date     Who   Rev                  Comments                        */
/* --------  ---  ------   -------------------------------------------  */
/* 01/24/05  JRB    09     Move tp4_debug_sel & "*_logstr" to lean_var.c*/
/* 07/30/03  EJV    08     Added encrypt_ctrl to tp_connect.		*/
/* 01/22/02  JRB    07     Add max_conns arg to tp4_initialize.		*/
/* 09/13/99  MDE    06     Added SD_CONST modifiers			*/
/* 10/08/98  MDE    05     Migrated to updated SLOG interface		*/
/* 08/13/98  JRB    04     Lint cleanup.				*/
/* 09/04/97  JRB    03     Add max_spdu_len arg to tp4_initialize.	*/
/*			   Not used by this stack but other stacks use.	*/
/* 08/26/97  MDE    02     Removed tp4_reldata 				*/
/* 06/04/97  JRB    01     tp4_reldata do nothing (just like LEAN-T).	*/
/*			   Freed in tp4_event right after processing.	*/
/* 05/27/97  JRB  7.00     MMSEASE 7.0 release.				*/
/* 01/23/97  JRB    06     Move tp4_debug_sel init to tp4_initialize.	*/
/*			   Del tp4_sLogCtrl, use sLogCtrl (slog.c) now.	*/
/* 01/09/97  RKR    05     Put ifdef DEBUG_SISCO around sLogCtrl	*/
/* 11/15/96  MDE    04     Global variable work				*/
/* 07/22/96  MDE    03     Changes towards supporting A-Unit Data	*/
/* 04/17/96  KCR    02     Fixed error recovery in tp4_connect to remove*/
/*                           structure from linked list if conn. failed */
/* 03/28/96  MDE    01     Created from tp4api.c                        */
/************************************************************************/


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


/************************************************************************/
SD_CONST static char *thisFileName = __FILE__;
#define THISFILE thisFileName

static ST_RET getNewSrcAdlcAddr (ST_UINT16 *addrDest, ST_UINT16 remAddr);

CONNINFO *conn_list;		/* pointer to list of conn structs */
REGINFO *reg_list;		/* pointer to list of REG structs */

/* Local ADLC Address control variables */
#if !defined(NO_GLB_VAR_INIT)
ST_UINT16 rsBaseSrcAdlcAddr = 0x2000;
ST_UINT16 rsNumSrcAdlcAddr = 0x200;
#else
ST_UINT16 rsBaseSrcAdlcAddr;
ST_UINT16 rsNumSrcAdlcAddr;
#endif

static ST_UINT16 nextSrcAdlcAddrOffset;

/************************************************************************/
/*			tp4_initialize					*/
/* This function is called by the user to get TP4 interface initialized	*/
/* and started								*/
/* NOTE: the max_conns arg is not used, but is included to be		*/
/*       compatible with the "standard" version of this function.	*/
/************************************************************************/

ST_RET tp4_initialize (ST_UINT max_conns, ST_UINT max_spdu_len)
  {
ST_RET ret;

#if defined(DEBUG_SISCO)
  tp4_debug_sel |= TP4_LOG_ERR;		/* Always enable error logging.	*/
#endif

#if defined(NO_GLB_VAR_INIT)
  if (!rsBaseSrcAdlcAddr)
    rsBaseSrcAdlcAddr = 0x2000;
  if (!rsNumSrcAdlcAddr)
    rsNumSrcAdlcAddr = 0x200;
#endif

/* Initialize the ADLC subsystem					*/
  ret = adlcInit ();
  return (ret);
  }


/************************************************************************/
/*			tp4_terminate					*/
/* This function is called by the user to terminate TP4 interface 	*/
/************************************************************************/

ST_RET tp4_terminate ()
  {

/* Shut down ADLC							*/
  adlcExit ();
  return (SD_SUCCESS);
  }

/************************************************************************/
/************************************************************************/
/*			tp4_bind					*/
/* This function is called by the user to bind to local TSAP.		*/
/* This is done by passing to the stack local transport address --	*/
/* transport selector + network address (NSAP).				*/
/************************************************************************/

ST_RET tp4_bind (ST_LONG user_bind_id, 
		  TP4_ADDR *tp4_addr, 
		  ST_INT sharable,
		  ST_INT max_conns)
  {
REGINFO *reg;

  SFLOWDOWN1 ("Bind Request issued for Bind ID %ld", user_bind_id);
  SFLOWDOWN1 ("  ADLC Address: %04x", tp4_addr->adlcAddr);


/* A 'bind' ties the user_bind_id to a local TP4 address, which in	*/
/* this case boils down to the local NSAP.				*/

  /* Allocate and fill in Reg Info structure */
  reg = (REGINFO *) chk_calloc (1, sizeof (REGINFO));
  list_add_last ((ST_VOID**) &reg_list, reg);

  reg->user_bind_id = user_bind_id;
  memcpy (&reg->localTp4Addr, tp4_addr, sizeof(TP4_ADDR));

  tp4_bind_cnf (user_bind_id, (ST_LONG)reg, SD_SUCCESS);
  return (SD_SUCCESS);
  }

/************************************************************************/
/*			tp4_unbind					*/
/************************************************************************/

ST_RET tp4_unbind (ST_LONG tp4_bind_id)
  {
REGINFO *reg;

  SFLOWDOWN1 ("Unbind for TP4 Bind ID %ld", tp4_bind_id);

  reg = (REGINFO *) tp4_bind_id;
  tp4_unbind_cnf (reg->user_bind_id);
  list_unlink ((ST_VOID**) &reg_list, reg);
  chk_free (reg);

  return (SD_SUCCESS);
  }

/************************************************************************/
/*			tp4_connect					*/
/* This function is called by the user to initiate a connect request to	*/
/* a remote node							*/
/************************************************************************/

ST_RET tp4_connect (ST_LONG tp4_bind_id, ST_LONG user_conn_id,
		     TP4_ADDR *rem_tp4_addr,
		     ST_INT conndata_len,
		     char *conndata,
                     S_SEC_ENCRYPT_CTRL encrypt_ctrl)
  {
REGINFO *reg;
CONNINFO *conn;
ST_RET ret;
ST_UINT16 localAddr;
ADLC_CONNECT_NODE adlcConnectInfo;

  reg = (REGINFO *) tp4_bind_id;

  SFLOWDOWN2 ("Connect Req on Bind ID %ld, User Connect ID %ld",
              reg->user_bind_id, user_conn_id);
  SFLOWDOWN1C ("%d bytes user data", conndata_len);

/* Get the ADLC address components */
  ret = getNewSrcAdlcAddr (&localAddr, rem_tp4_addr->adlcAddr);
  if (ret != SD_SUCCESS)
    {
    SERR0 ("Could not get new source address");
    return (ret);
    }

  SFLOWDOWN2C ("Local ADLC address 0x%X, Remote ADLC address 0x%X", 
			localAddr, rem_tp4_addr->adlcAddr);

  conn = (CONNINFO *) chk_calloc (1, sizeof (CONNINFO));
  list_add_last ((ST_VOID**) &conn_list, conn);
  conn->reginfo = reg;
  conn->user_conn_id = user_conn_id;

/* Record the ADLC address info for future use				*/
  conn->localAddr = localAddr;
  conn->remAddr = rem_tp4_addr->adlcAddr;

/* Issue the ADLC connect */
  adlcConnectInfo.i.localAddr = localAddr;
  adlcConnectInfo.i.remoteAddr = rem_tp4_addr->adlcAddr;

  ret = adlcConnectNode(&adlcConnectInfo);
  if (ret)
    {
    SERR1("adlcConnectNode Error 0x%04x", ret);
    list_unlink ((ST_VOID**) &conn_list, conn);
    chk_free (conn);
    }
  
  return (ret);
  }

/************************************************************************/
/*			tp4_accept					*/
/* This function is called by the user to accept a connect request from	*/
/* a remote node							*/
/************************************************************************/

ST_RET tp4_accept (ST_LONG tp4_conn_id, ST_LONG user_conn_id,
		    ST_INT conndata_len, char *conndata)
  {
REGINFO *reg;
CONNINFO *conn;

  conn = (CONNINFO *) tp4_conn_id;
  if (list_find_node (conn_list, conn) == SD_FAILURE)
    {
    SERR1 ("Connection ID %ld not found", tp4_conn_id);
    return (TP4E_BADCONN);
    }

  reg = conn->reginfo;
  SFLOWDOWN2 ("Accept issued on bind ID %ld using user connect ID %ld",
              reg->user_bind_id, user_conn_id);
  SFLOWDOWN2C ("Local ADLC address 0x%X, Remote ADLC address 0x%X", 
			conn->localAddr, conn->remAddr);
  SFLOWDOWN1C ("%d bytes user data", conndata_len);

  conn->user_conn_id = user_conn_id;
  return (SD_SUCCESS);
  }

/************************************************************************/
/*			tp4_disconnect					*/
/* This function is called by the user to break connection		*/
/************************************************************************/

ST_RET tp4_disconnect (ST_LONG tp4_conn_id,
		        ST_INT conndata_len,
		        char *conndata)
  {
CONNINFO *conn;
ADLC_DISCONNECT_NODE adlcDiscInfo;
ST_RET ret;

  conn = (CONNINFO *) tp4_conn_id;
  if (list_find_node (conn_list, conn) == SD_FAILURE)
    {
    SERR1 ("User Connection ID %ld not found", tp4_conn_id);
    return (TP4E_BADCONN);
    }

  SFLOWDOWN1 ("Tp4 Disconnect issued on user connect ID %ld", conn->user_conn_id);
  SFLOWDOWN1C ("TP4 connect ID %ld", tp4_conn_id);
  SFLOWDOWN2C ("Local ADLC address 0x%X, remote ADLC address 0x%X",
			conn->localAddr, conn->remAddr);

/*  - Disconnect the ADLC connection					*/
  adlcDiscInfo.i.localAddr = conn->localAddr;
  adlcDiscInfo.i.remoteAddr = conn->remAddr;
  ret = adlcDisconnectNode(&adlcDiscInfo);
  if (ret == SD_SUCCESS)
    {
    list_unlink ((ST_VOID**) &conn_list, conn);
    chk_free (conn);
    }
  else
    {
    SERR1("adlcDisconnectNode Error 0x%04x", ret);
    }
  return (ret);
  }

/************************************************************************/
/*			tp4_data					*/
/* This function is called by the user to send data			*/
/************************************************************************/

ST_RET tp4_data (ST_LONG tp4_conn_id, ST_INT eot, 
		  ST_UINT data_len, char *data)
  {
CONNINFO *conn;
ADLC_WRITE_PACKET adlcWriteInfo;
ST_RET ret;
ST_CHAR *buf;

  conn = (CONNINFO *) tp4_conn_id;
  if (list_find_node (conn_list, conn) == SD_FAILURE)
    {
    SERR1 ("Connection ID %ld not found", tp4_conn_id);
    return (TP4E_BADCONN);
    }

  SFLOWDOWN1 ("Data issued on Connect ID %ld", conn->user_conn_id);
  SFLOWDOWN2C ("Local ADLC address 0x%X, remote ADLC address 0x%X",
			conn->localAddr, conn->remAddr);
  SFLOWDOWNH (data_len, data);

  buf = (ST_CHAR *) chk_malloc (data_len +3);

   /* Add LSAP's, QOS				*/
  buf[0] = rs_co_dest;
  buf[1] = rs_co_src;
  buf[2] = rs_co_qos;
  memcpy (&buf[3], data, data_len);

/* Send the data over the ADLC connection				*/
  adlcWriteInfo.i.localAddr = conn->localAddr;
  adlcWriteInfo.i.remoteAddr = conn->remAddr;
  adlcWriteInfo.i.dataSize = data_len + 3;
  adlcWriteInfo.i.data = buf;

  ret = adlcWritePacket (&adlcWriteInfo);
  if (ret != SD_SUCCESS)
    {
    SERR1("adlcWritePacket Error 0x%04x", ret);
    }

  chk_free (buf);
  return (ret);
  }


/************************************************************************/
/*			tp4_expdata					*/
/* This function is called by the user to send expedited data		*/
/************************************************************************/

ST_RET tp4_expdata (ST_LONG tp4_conn_id,
		     ST_UINT data_len,
		     char *data)
  {
CONNINFO *conn;
ADLC_WRITE_PACKET adlcWriteInfo;
ST_RET ret;

  conn = (CONNINFO *) tp4_conn_id;
  if (list_find_node (conn_list, conn) == SD_FAILURE)
    {
    SERR1 ("Connection ID %ld not found", tp4_conn_id);
    return (TP4E_BADCONN);
    }

  SFLOWDOWN1 ("Expedited Data issued on Connect ID %ld", conn->user_conn_id);
  SFLOWDOWN2C ("Local ADLC address 0x%X, remote ADLC address 0x%X",
			conn->localAddr, conn->remAddr);
  SFLOWDOWNH (data_len, data);

/* Send the data over the ADLC connection				*/
  adlcWriteInfo.i.localAddr = conn->localAddr;
  adlcWriteInfo.i.remoteAddr = conn->remAddr;
  adlcWriteInfo.i.dataSize = data_len;
  adlcWriteInfo.i.data = data;

  ret = adlcWritePacket (&adlcWriteInfo);
  if (ret != SD_SUCCESS)
    {
    SERR1("adlcWritePacket Error 0x%04x", ret);
    }

  return (ret);
  }

/************************************************************************/
/************************************************************************/

static ST_RET getNewSrcAdlcAddr (ST_UINT16 *addrDest, ST_UINT16 remAddr)
  {
CONNINFO *conn;
ST_UINT16 i;
ST_UINT16 localAddr;

  i = 0;
  while (SD_TRUE && i < rsNumSrcAdlcAddr)
    {
    localAddr = rsBaseSrcAdlcAddr + i;
  
  /* Verify that this src/dest address pair is not used			*/
    conn = conn_list;
    while (conn)
      {
      if (conn->localAddr == localAddr && conn->remAddr == remAddr)
        break;		/* Address pair is in use */

      conn = (CONNINFO *) list_get_next (conn_list, conn);
      }
    if (!conn)
      break;

    ++i;		/* loop escape */
    }

  if (i >= rsNumSrcAdlcAddr)
    return (SD_FAILURE);

  *addrDest = localAddr;
  return (SD_SUCCESS);  
  }

