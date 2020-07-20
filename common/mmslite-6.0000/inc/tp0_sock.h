#ifndef TP0_SOCK_INCLUDED
#define TP0_SOCK_INCLUDED
/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	1997 - 2008 All Rights Reserved					*/
/*									*/
/* MODULE NAME : tp0_sock.h						*/
/* PRODUCT(S)  : Lean-T Stack						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*	TP0/RFC1006 header file for "sockets" interface.		*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev     Comments					*/
/* --------  ---  ------   -------------------------------------------  */
/* 08/02/10  JRB	   Chg sockCtx from (GEN_SOCK_CTXT *) to	*/
/*			   (GEN_SOCK_CTXT) so don't need to alloc/free.	*/
/* 11/17/09  JRB    32     Chg INVALID_CONN_ID to NULL.			*/
/*			   Del MMSEASE_MOSI.				*/
/*			   Add global g_tp0_max_spdu_len (was static).	*/
/* 11/01/09  JRB    31     Merged these changes by EJV:			*/
/*			   Added IND_EVENT_TYPE_STR macro.		*/
/*			   Chg *_conn_id from ST_LONG to (ST_VOID*)	*/
/*			     to support all 64-bit systems.		*/
/*			   Moved TP0_SOCK_CTX from tcp_socks.c to here.	*/
/* 10/26/06  EJV    30     Ported MMSEASE_MOSI code to HP-UX.		*/
/* 06/19/06  EJV    29     Ported MMSEASE_MOSI code to Sun.		*/
/* 08/03/05  EJV    28     SOCK_INFO: added rekeyTime			*/
/* 07/19/05  EJV    27     Added SOCK_STATE_CONNECT_CANCELLED.		*/
/* 06/30/05  EJV    26     SOCK_INFO: added recvCnt (for flow control)	*/
/* 06/06/05  EJV    25     MMSEASE_MOSE: few more chgs to RFC1006_IPC*	*/
/* 05/25/05  EJV    24     Added ssec.h					*/
/* 04/25/05 JRB,EJV 23     Add RFC1006_IPC_MSG, RFC1006_LISTENER_.. defs*/
/* 05/09/05  EJV    22     SOCK_INFO: added port field. sreadd add arg6.*/
/* 05/05/05  EJV    21     np_connect_req: added rem_port param.	*/
/* 03/08/05  JRB    20     Add RFC1006.. define.			*/
/* 02/22/05  JRB    19     slistend,sreadd: chg ret from void to int.	*/
/* 02/21/05  JRB    18     Del ioctlsocket define. Use def in gensock2.h*/
/* 01/14/04  EJV    17     np_data_req: added eot param.		*/
/*			   Added np_get_tx_queue_cnt func.		*/
/* 10/23/03  JRB    16     Move all system includes to sysincs.h	*/
/* 10/16/03  JRB    15     SOCK_INFO: add genSock.			*/
/* 09/11/03  EJV    14	   Del winsock.h (windows.h gets right winsock)	*/
/*			   Added encrypt_ctrl arg to np_connect_req().	*/
/*			   SOCK_INFO: Add DBL_LNK l and encrypt_ctrl.	*/
/* 07/16/02  JRB    13     Add arg to sreadd proto.			*/
/* 06/20/02  JRB    12     Chg np_connect_req 2nd arg to ulong ipAddr.	*/
/* 05/08/02  JRB    11     Chg connect_result to int (to store errno).	*/
/* 05/03/02  JRB    10     Add ip_addr to SOCK_INFO to pass to Read task.*/
/* 02/02/01  EJV    09     _AIX: added forward references for inet.h.	*/
/* 01/02/01  EJV    08     Ported to _AIX.				*/
/* 08/15/00  EJV    07     WIN32: Del pipe_msg_crit_sec, pipe_msg_count.*/
/* 05/18/00  JRB    06     Del "select.h". See "sysincs.h".		*/
/*			   Add <netinet/tcp.h> for VXWORKS.		*/
/*			   Improve "ioctlsocket" macros for non-WIN32.	*/
/* 08/13/98  JRB    05     Lint cleanup.				*/
/*			   Add <netinet/tcp.h> for default system.	*/
/* 12/09/97  JRB    04     Added __OS2__ support.			*/
/* 11/14/97  JRB    03     Make PIPE_MSG for VXWORKS like _WIN32.	*/
/* 11/10/97  JRB    02     Add VXWORKS support.				*/
/* 08/01/97  JRB    01     Created (compatible with MMS-EASE 7.0).	*/
/************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

#include "gensock2.h"		/* need GEN_SOCK definition	*/
#include "ssec.h"		/* need S_SEC_ENCRYPT_CTRL	*/

/* Include sockets defs appropriate for this platform.			*/
#if !defined (_WIN32)
#define closesocket		close
#define WSAEWOULDBLOCK		EWOULDBLOCK
#define WSAEINPROGRESS		EINPROGRESS
#if defined (__OS2__)
#define WSAGetLastError()	sock_errno()
#define GetLastError()		errno
#else	/* !__OS2__	*/
#define WSAGetLastError()	errno
#endif	/* !__OS2__	*/
#define SOCKET			int
#define SOCKADDR		struct sockaddr
#define SOCKADDR_IN		struct sockaddr_in
#define PSOCKADDR		struct sockaddr *
#define HOSTENT			struct hostent
#define INVALID_SOCKET		(-1)

#if defined(sun) || defined(__hpux)
#if !defined(AF_LOCAL)
#define AF_LOCAL		AF_UNIX
#endif
#endif  /* defined(sun) || defined(__hpux) */

#endif	/* !_WIN32	*/

/************************************************************************/
/* Constant definitions.						*/
/************************************************************************/
#define IPPORT_RFC1006		102
#define RFC1006_HEAD_LEN	4
#define RFC1006_VERSION		3	/* first byte of TPKT is version*/
#define INVALID_CONN_ID		NULL	/* conn_id is ptr so NULL is invalid*/
#define LISTEN_TASK_NAME	"slistend"	/* "Listen" task exe name*/
#define READ_TASK_NAME		"sreadd"	/* "Read" task exe name	*/
#define PIPE_TO_MAIN_NAME	"/tmp/pipetp0main"
#define PIPE_TO_LISTEN_NAME	"/tmp/pipetp0list"

/************************************************************************/
/* Variable type definitions.						*/
/************************************************************************/

/* Defines of possible values for "state" in SOCK_INFO struct.		*/
#define SOCK_STATE_ACCEPTED		0
#define SOCK_STATE_CONNECTING		1
#define SOCK_STATE_CONNECTED		2
#define SOCK_STATE_DISCONNECTING	3
#define SOCK_STATE_CONNECT_CANCELLED	4
/* The following errors used only with SNAP_LITE	*/
#define SOCK_STATE_CONNECTING_TO_SNAP	5	/* for SSL connections	*/
#define SOCK_STATE_CONNECTED_WAIT_CR	6
#define SOCK_STATE_CONNECTED_WAIT_P_CON	7

typedef struct
  {
  DBL_LNK l;		/* needed to store this struct on linked list	*/
  GEN_SOCK *genSock;	/* Use with gensock2. Otherwise, use "hSock"	*/
  SOCKET hSock;
  ST_INT state;
  ST_VOID *user_conn_id;/* Received from user on N-CONNECT.req/rsp	*/
  ST_UINT32 ip_addr;	/* IP addr as long int in network byte order.	*/
  ST_UINT16 port;	/* RFC1006 port, defaults to 102		*/
#if !(defined(_WIN32) || defined (__OS2__))
  pid_t pid_read;	/* Save READ process PID			*/
#endif
  S_SEC_ENCRYPT_CTRL encrypt_ctrl;
  ST_UINT recvCnt;	/* num of received (queued) data msgs		*/
  ST_DOUBLE rekeyTime;	/* next time to check rekey if we are idle	*/
  } SOCK_INFO;


/* Info to pass to spawned thread. Can only pass 1 arg to thread, so	*/
/* make it a structure, and put whatever we need in it.			*/
typedef struct
  {
  SOCK_INFO *sock_info;		/* to pass back to main in PIPE_MSG.	*/
  SOCKET hSock;			/* copied from "sock_info"		*/
  ST_INT state;			/* copied from "sock_info"		*/
  ST_UINT16 max_tpkt_len;	/* Max len of RFC1006 TPKT.		*/
  } THREAD_INFO;

typedef struct
  {
  SOCK_INFO *sock_info;
  ST_INT connect_result;	/* errno (WSAGetLastErr() for Windows)*/
  ST_UINT16 tpkt_len;
#if (defined (_WIN32) || defined (VXWORKS) || defined (__OS2__))
  ST_UCHAR *tpkt_ptr;
#else
  ST_UCHAR tpkt_buf [1];	/* Beginning of buffer attached to this struct*/
#endif
  } PIPE_MSG;

/* Use define for Length of PIPE_MSG without tpkt_buf at end.	*/
#define PIPE_MSG_HEAD_LEN	(sizeof(PIPE_MSG)-1)

/* Ctx state (used for "state" in TP0_SOCK_CTX).	*/
#define TP0_SOCK_CTX_STATE_IDLE			0
#define TP0_SOCK_CTX_STATE_ACTIVE		1
#define TP0_SOCK_CTX_STATE_TERMINATING		2

/* struct to hold configuration and communication info for secured and	*/
/* non-secured connections.						*/
/* Used if !defined(SNAP_LITE) (see TP0_SOCK_CTX_SNAP for SNAP_LITE).	*/
typedef struct tag_tp0_sock_ctx
  {
  ST_UINT        state;			/* TP0_SOCK_CTX_STATE_xxx	*/
  S_SEC_CONFIG  *secCfg;		/* ptr to global configuration	*/

  ST_UINT16      rfc1006Port;		/* listen port used for		*/
					/* RFC1006 connections		*/

  GEN_SOCK      *remListenSock;		/* listen socket for non-secured*/
					/* cons from remotes		*/

  GEN_SOCK      *remListenSockSSL[S_SSL_MAX_LISTEN_PORTS]; /* tbl of	*/
					/* secure listen sockets	*/

  SOCK_INFO     *secureSockList;	/* list of secured sockets	*/

  GEN_SOCK_CTXT  sockCtx;		/* gensock2 context		*/
  } TP0_SOCK_CTX;

/************************************************************************/
/* Global variables.							*/
/************************************************************************/
extern ST_UCHAR *g_tpkt_enc_buf;	/* Buffer for encoding TPKTs.	*/
extern ST_UINT   g_tp0_max_spdu_len;	/* Max SPDU size to send or receive*/
extern SOCKET	hSockListen;		/* listen socket handle		*/
extern TP0_SOCK_CTX tp0Ctx;

#if defined _WIN32
extern HANDLE	hPipeMainRead;		/* read handle for pipe		*/
extern HANDLE	hPipeMainWrite;		/* write handle for pipe	*/
extern HANDLE	hEventListenGo;
#endif
#if defined (__OS2__)
extern HFILE   hPipeMainRead;		/* read handle for pipe		*/
extern HFILE   hPipeMainWrite;		/* write handle for pipe	*/
extern HEV     hEventListenGo;
extern ST_INT  pipe_msg_count;
#endif	/* __OS2__	*/

/************************************************************************/
/* Function Prototypes.							*/
/************************************************************************/
ST_RET np_init (ST_INT max_num_conns);
ST_RET np_end (ST_VOID);
ST_BOOLEAN np_event (ST_VOID);

SOCK_INFO *np_connect_req (ST_VOID *user_conn_id, ST_ULONG ipAddr, ST_UINT16 rem_port,
                            S_SEC_ENCRYPT_CTRL *encrypt_ctrl);
ST_VOID np_connect_cnf_pos (SOCK_INFO *sock_info, ST_VOID *user_conn_id);
/* NOTE: np_connect_ind & np_connect_rsp not needed because user only	*/
/*       cares about Transport conn. Doesn't need to know about Network conn.*/

ST_RET np_data_req (SOCK_INFO *sock_info, ST_INT pdu_len, ST_UCHAR *pdu_ptr, ST_BOOLEAN eot);
ST_VOID np_data_ind (SOCK_INFO *sock_info, ST_UCHAR *pdu_ptr, ST_UINT16 pdu_len);
ST_UINT np_get_tx_queue_cnt (SOCK_INFO *sock_info);

ST_RET np_disconnect_req (SOCK_INFO *sock_info);
ST_VOID np_disconnect_ind (SOCK_INFO *sock_info);

ST_RET sockets_init (ST_INT max_num_conns);
ST_VOID sockets_end (ST_VOID);
ST_RET spawn_listen (SOCKET hSock);
ST_RET spawn_read (SOCK_INFO *sock_info);
ST_VOID handle_accepted_conn (SOCK_INFO *sock_info);
ST_VOID handle_connect_success (SOCK_INFO *sock_info);
ST_VOID handle_connect_fail (SOCK_INFO *sock_info);
ST_VOID handle_data (SOCK_INFO *sock_info, ST_UCHAR *tpkt_ptr, ST_UINT16 tpkt_len);
ST_VOID handle_disconnect (SOCK_INFO *sock_info);
SOCK_INFO *sock_info_alloc (SOCKET hSock, ST_INT state, ST_VOID *user_conn_id);
ST_VOID sock_info_free (SOCK_INFO *sock_info);

#if defined (_WIN32)
void ThreadSockListen (void *param);
void ThreadSockRead (void *param);
#endif	/* _WIN32	*/
#if defined (__OS2__)
void _Optlink ThreadSockListen (void *param);
void _Optlink ThreadSockRead (void *param);
#endif	/* __OS2__	*/

int slistend (int arg1);
int sreadd (int arg1, int arg2, int arg3, int arg4, int arg5, int arg6);

#ifdef __cplusplus
}
#endif

#endif	/* !TP0_SOCK_INCLUDED	*/

