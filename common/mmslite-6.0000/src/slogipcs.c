/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*		2004 - 2009, All Rights Reserved 			*/
/*									*/
/* MODULE NAME : slogipcs.c						*/
/* PRODUCT(S)  : SLOG							*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	Stub functions for SLOG IPC, fully implemented in slogipc.c.	*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 01/07/11  MDE    10     Added slogIpcHeartbeatService		*/
/* 08/25/09  RKR    09     changed stub for slog_ipc_std_cmd_fun        */
/* 05/18/09  EJV    08     Unified funcs with slogipc.c.		*/
/* 08/04/08  MDE    07     Added slogIpcEventEx				*/
/* 07/25/08  MDE    06     Moved slogIpcCtx into LOG_CTRL		*/
/* 05/30/08  JRB    05     Fix ..RawData args.				*/
/* 10/04/07  MDE    04     Added slogIpcCallingEnable			*/
/* 01/08/07  EJV    04     slogIpcStop: added lc param.			*/
/* 11/03/05  EJV    02     Added slogIpcEvent.				*/
/* 02/22/05  JRB    02     slogIpc: add SD_CONST to some args.		*/
/* 05/18/04  MDE    01     Initial Release				*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "stime.h"
#include "slog.h"
#include "gensock2.h"

/************************************************************************/
SLOGIPC_CMD *slogIpcCmdList;
ST_INT 	    slogIpcMaxCmdPend = 1;

/************************************************************************/
/*                              slogIpcInit				*/
/************************************************************************/

ST_RET slogIpcInit (LOG_CTRL *lc)
  {
  return (SD_SUCCESS);
  }

/************************************************************************/
/*                       slogIpcCallingEnable   			*/
/************************************************************************/

ST_VOID slogIpcCallingEnable (ST_BOOLEAN enable)
  {
  }


ST_VOID slogIpcCallingEnableEx (struct log_ctrl *lc, ST_BOOLEAN enable)
  {
  }


/************************************************************************/
/*                              slogIpcStop				*/
/************************************************************************/

ST_RET slogIpcStop (LOG_CTRL *lc)
  {
  return (SD_SUCCESS);
  }

/************************************************************************/
/*                              slogIpcEvent				*/
/************************************************************************/

ST_RET slogIpcEvent (ST_VOID)
{
  return (SD_SUCCESS);
}

ST_RET slogIpcEventEx (LOG_CTRL *lc)
{
  return (SD_SUCCESS);
}

/************************************************************************/
/*                              slogIpc					*/
/************************************************************************/

ST_VOID slogIpc (LOG_CTRL *lc, ST_INT logType,
		 SD_CONST ST_CHAR *SD_CONST logTypeStr, 
		 SD_CONST ST_CHAR *SD_CONST sourceFile,
		 ST_INT lineNum, ST_INT bufLen, 
                 ST_CHAR *buf)
  {
  }



/************************************************************************/
/*			slog_ipc_std_cmd_fun				*/
/************************************************************************/

ST_VOID slog_ipc_std_cmd_fun (SLOGIPC_RX_IND *slogIpcRxInd)
  {
  }


/************************************************************************/
/*			slogIpcSendData					*/
/************************************************************************/

ST_VOID slogIpcSendData (GEN_SOCK *pSock, ST_UINT32 msgType, ST_INT dataLen, ST_UCHAR *data)
  {
  }

/************************************************************************/
/*			slogIpcSendRawData				*/
/************************************************************************/

ST_VOID slogIpcSendRawData (LOG_CTRL *lc, ST_INT bufLen, ST_CHAR *buf, ST_BOOLEAN sendAlways)
  {
  }

/************************************************************************/
/*			slogIpcHeartbeatService				*/
/************************************************************************/

ST_VOID slogIpcHeartbeatService (LOG_CTRL *lc)
  {
  }
