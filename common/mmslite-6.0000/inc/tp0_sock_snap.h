#ifndef TP0_SOCK_SNAP_INCLUDED
#define TP0_SOCK_SNAP_INCLUDED
/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	1997 - 2009 All Rights Reserved					*/
/*									*/
/* MODULE NAME : tp0_sock_snap.h					*/
/* PRODUCT(S)  : Lean-T Stack						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*	TP0/RFC1006 header file for "sockets" interface using SNAP-Lite.*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev     Comments					*/
/* --------  ---  ------   -------------------------------------------  */
/* 08/02/10  JRB	   Chg sockCtx from (GEN_SOCK_CTXT *) to	*/
/*			   (GEN_SOCK_CTXT) so don't need to alloc/free.	*/
/* 11/17/09  JRB    01     Created from EJV code.			*/
/************************************************************************/
#include "gensock2.h"		/* need GEN_SOCK definitions	*/

#ifdef __cplusplus
extern "C" {
#endif

/* This many messages can accumulate in the receiving ind queue 	*/
/* for given socket before we stop receiving, start receiving after	*/
/* we reach low mark.							*/
#define SOCK_RX_QUE_HIGH_MARK	10		/* >= stop  receiving	*/
#define SOCK_RX_QUE_LOW_MARK	 1		/* <= start receiving	*/

extern ST_CHAR *indEventTypeStr[];
extern ST_INT   indEventTypeStrCnt;
#define  IND_EVENT_TYPE_STR(eventType)   ((eventType < indEventTypeStrCnt) ? indEventTypeStr[eventType] : "IND EVENT UNKNOWN")

  /* Linux, UNIX only - start */
#define TCP_EVENT_WAKEUP_PORT_BASE		56000
#define TCP_EVENT_WAKEUP_PORT_RANGE   		100
  /* Linux, UNIX only - end   */

/* NOTE:   If the gensock2 is compiled with GENSOCK_THREAD_SUPPORT then	*/
/*         following handle should be used by the user application to	*/
/*         wait for TCP events.						*/
#if defined(_WIN32)
  /* on Windows the hTpcEvent indicates TCP events (extern in tp4api.h) */
#else  /* LINUX, UNIX */
extern SOCKET mms_event_fd;	/* wakeup socket, indicating TCP events	*/
#endif /* LINUX, UNIX */

/* struct to hold configuration and communication info for secured and	*/
/* non-secured connections.						*/
typedef struct
  {
  ST_UINT	 state;			/* TP0_SOCK_CTX_STATE_xxx	*/
  S_SEC_CONFIG  *secCfg;		/* ptr to global configuration	*/

  /* Linux, UNIX only - start */
  /* Needed only if GENSOCK_THREAD_SUPPORT is defined to indicate socket activity to application*/
  ST_UINT16 tcpEventWakeupPortBase;	/* base listen port, default TCP_EVENT_WAKEUP_PORT_BASE	*/
  ST_UINT   tcpEventWakeupPortRange;	/* range to search for free port, def TCP_EVENT_WAKEUP_PORT_RANGE */
  ST_UINT16 tcpEventWakeupPort;		/* actual wakeup port				*/
  SOCKET    hTcpEventSender;		/* signaled by gensock2 service thread		*/
  /* Linux, UNIX only - end */

  GEN_SOCK_CTXT sockCtx;		/* this is gensock2 context	*/
  } TP0_SOCK_CTX_SNAP;

/************************************************************************/
/* Global variables.							*/
/************************************************************************/
extern TP0_SOCK_CTX_SNAP tp0CtxSnap;

/* tp0_sock_snap.c functions	*/
ST_RET  sockSetDefaultsSnap (GEN_SOCK_CONFIG *sockCfg);
ST_VOID uSockDataAllocSnap  (GEN_SOCK *pSock, ST_INT dataLen, GEN_SOCK_DATA **sockDataOut);
ST_VOID uSockDataFreeSnap   (GEN_SOCK *pSock, GEN_SOCK_DATA *sockData);

#ifdef __cplusplus
}
#endif

#endif	/* !TP0_SOCK_SNAP_INCLUDED	*/

