/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	2005 - 2007 All Rights Reserved					*/
/*									*/
/* MODULE NAME : ipc_utl.h						*/
/* PRODUCT(S)  : 							*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*	IPC (using socket or UNIX domain socket) 'utility' functions.	*/ 
/*	The write_fd and recvwait_fd functions allow to pass a socket	*/
/*      handle between processes.					*/
/*	These functions are general enough to be used in other		*/
/*	projects.							*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev     Comments					*/
/* --------  ---  ------   -------------------------------------------  */
/* 02/22/07  EJV    01     Moved from tp0_sock.h			*/
/************************************************************************/
#ifndef SOCK_UTIL_INCLUDED
#define SOCK_UTIL_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

/* these defines  were copied from gensock2.h */
#if defined(_WIN32)

#define SOCKET_ERRORNO  WSAGetLastError()
#define SOCK_EADDRINUSE WSAEADDRINUSE
#define SOCK_WOULDBLOCK WSAEWOULDBLOCK
#define SOCK_INTR       WSAEINTR
#define SOCK_NOBUFS     WSAENOBUFS
#define SOCK_INPROGRESS WSAEINPROGRESS
#define SOCK_TIMEDOUT   WSAETIMEDOUT
#define SOCK_NOTSOCK    WSAENOTSOCK
#define SOCK_ACCESS     WSAEACCES
#define CLOSE_SOCKET(x) closesocket(x)

#define SOCK_OPTLEN     ST_INT		/* in getsockopt	*/
#define SOCK_ADDRLEN    ST_INT		/* in accept 		*/

#else /* UNIX, Linux,... */

#define SOCKET_ERRORNO  errno
#define SOCK_EADDRINUSE EADDRINUSE
#define SOCK_WOULDBLOCK EWOULDBLOCK
#define SOCK_INTR       EINTR
#define SOCK_NOBUFS     ENOBUFS
#define SOCK_INPROGRESS EINPROGRESS
#define SOCK_TIMEDOUT   ETIMEDOUT
#define SOCK_NOTSOCK    EBADF
#define SOCK_ACCESS     EACCES
#define CLOSE_SOCKET(x) close(x)

#define SOCKET		int
#define INVALID_SOCKET	(-1)
#define SOCKADDR	struct sockaddr
#define SOCKADDR_IN	struct sockaddr_in

#if defined (VXWORKS)		/* VXWORKS wants arg to be int	*/
#define ioctlsocket(hSock,opt,valptr)	ioctl(hSock, opt, (int) valptr)
#else	/* all other systems (e.g. UNIX)	*/
#define ioctlsocket	ioctl
#endif

#if defined(_AIX) || defined(sun) || defined(__hpux) || defined(linux)
/* Note: __hpux will need these defines when passing access rights SCM_RIGHTS 	*/
/*       through domain socket is working properly for _XOPEN_SOURCE_EXTENDED.	*/
#define SOCK_OPTLEN     socklen_t	/* in getsockopt	*/
#define SOCK_ADDRLEN    socklen_t	/* in accept 		*/
#elif defined(__VMS)
#define SOCK_OPTLEN     __size_t	/* in getsockopt	*/
#define SOCK_ADDRLEN    __size_t	/* in accept 		*/
#else
/* all other no-Windows platforms not defined above */
#define SOCK_OPTLEN     int		/* in getsockopt	*/
#define SOCK_ADDRLEN    int		/* in accept 		*/
#endif

#endif /* UNIX, Linux,... */



/************************************************************************/
/* Handy socket utility functions (see ipc_utl.c)			*/
/************************************************************************/

#define RET_SOCK_ERR	(-1)	/* ret from recvwait/sendwait for socket error	*/
#define RET_TIMEOUT	(-2)	/* ret from recvwait/sendwait for timeout	*/
#define RET_PROC_ERR	(-3)	/* ret for processing error			*/

ST_INT recvwait (SOCKET fd, ST_VOID *buf, ST_INT numBytes, int flags, ST_CHAR *sockName,
                 ST_INT timeout, ST_DOUBLE *startTm);
ST_INT sendwait (SOCKET fd, ST_VOID *buf, ST_INT numBytes, int flags, ST_CHAR *sockName,
                 ST_INT timeout, ST_DOUBLE *startTm);

#if defined(_WIN32)
ST_INT write_fd    (SOCKET fd, SOCKET hTcpSock, DWORD pid, ST_CHAR *sockName,
                    ST_INT timeout, ST_DOUBLE *startTm);
#else /* Linux, UNIX */
ST_RET write_fd    (SOCKET fd, SOCKET hTcpSock, ST_CHAR *sockName,
                    ST_INT timeout, ST_DOUBLE *startTm);
#endif /* Linux, UNIX */
ST_INT recvwait_fd (SOCKET fd, SOCKET *hTcpSock, ST_CHAR *sockName,
                    ST_INT timeout, ST_DOUBLE *startTm);

#define SOCK_EVENT_NONE		0
#define SOCK_EVENT_READ		1
#define SOCK_EVENT_WRITE	2

ST_INT sock_event_pending (SOCKET fd, ST_BOOLEAN chWrite, ST_INT timeout);



/*----------------------------------------------------------------------*/
/*			I P C 	L O G G I N G				*/
/*----------------------------------------------------------------------*/

#define IPC_LOG_ERR	0x0001
#define IPC_LOG_NERR	0x0002
#define IPC_LOG_FLOW	0x0004
#define IPC_LOG_DEBUG	0x0008
#define IPC_LOG_RX	0x0010
#define IPC_LOG_TX	0x0020

extern ST_UINT  ipc_debug_sel;

extern LOGCFG_VALUE_GROUP ipcLogMaskMapCtrl;

/* Log type strings */
extern SD_CONST ST_CHAR *SD_CONST  IPC_LOG_ERR_TEXT;
extern SD_CONST ST_CHAR *SD_CONST  IPC_LOG_NERR_TEXT;
extern SD_CONST ST_CHAR *SD_CONST  IPC_LOG_FLOW_TEXT;
extern SD_CONST ST_CHAR *SD_CONST  IPC_LOG_DEBUG_TEXT;
extern SD_CONST ST_CHAR *SD_CONST  IPC_LOG_RX_TEXT;
extern SD_CONST ST_CHAR *SD_CONST  IPC_LOG_TX_TEXT;

/*********************** IPC_LOG_ERR Macros *****************************/

#define IPC_LOG_ERR0(a) \
	SLOG_0 (ipc_debug_sel & IPC_LOG_ERR,IPC_LOG_ERR_TEXT,a)
#define IPC_LOG_ERR1(a,b) \
	SLOG_1 (ipc_debug_sel & IPC_LOG_ERR,IPC_LOG_ERR_TEXT,a,b)
#define IPC_LOG_ERR2(a,b,c) \
	SLOG_2 (ipc_debug_sel & IPC_LOG_ERR,IPC_LOG_ERR_TEXT,a,b,c)
#define IPC_LOG_ERR3(a,b,c,d) \
	SLOG_3 (ipc_debug_sel & IPC_LOG_ERR,IPC_LOG_ERR_TEXT,a,b,c,d)
#define IPC_LOG_ERR4(a,b,c,d,e) \
	SLOG_4 (ipc_debug_sel & IPC_LOG_ERR,IPC_LOG_ERR_TEXT,a,b,c,d,e)
#define IPC_LOG_ERR5(a,b,c,d,e,f) \
	SLOG_5 (ipc_debug_sel & IPC_LOG_ERR,IPC_LOG_ERR_TEXT,a,b,c,d,e,f)
#define IPC_LOG_ERR6(a,b,c,d,e,f,g) \
	SLOG_6 (ipc_debug_sel & IPC_LOG_ERR,IPC_LOG_ERR_TEXT,a,b,c,d,e,f,g)

/* Continuation log macros	*/
#define IPC_LOG_ERRC0(a) \
	SLOGC_0 (ipc_debug_sel & IPC_LOG_ERR,a)
#define IPC_LOG_ERRC1(a,b) \
	SLOGC_1 (ipc_debug_sel & IPC_LOG_ERR,a,b)
#define IPC_LOG_ERRC2(a,b,c) \
	SLOGC_2 (ipc_debug_sel & IPC_LOG_ERR,a,b,c)
#define IPC_LOG_ERRC3(a,b,c,d) \
	SLOGC_3 (ipc_debug_sel & IPC_LOG_ERR,a,b,c,d)
#define IPC_LOG_ERRC4(a,b,c,d,e) \
	SLOGC_4 (ipc_debug_sel & IPC_LOG_ERR,a,b,c,d,e)
#define IPC_LOG_ERRC5(a,b,c,d,e,f) \
	SLOGC_5 (ipc_debug_sel & IPC_LOG_ERR,a,b,c,d,e,f)
#define IPC_LOG_ERRC6(a,b,c,d,e,f,g) \
	SLOGC_6 (ipc_debug_sel & IPC_LOG_ERR,a,b,c,d,e,f,g)

/* Hex log macro	*/
#define IPC_LOG_ERRH(a,b) \
	SLOGH (ipc_debug_sel & IPC_LOG_ERR,a,b)

/*********************** IPC_LOG_NERR Macros *****************************/

#define IPC_LOG_NERR0(a) \
	SLOG_0 (ipc_debug_sel & IPC_LOG_NERR,IPC_LOG_NERR_TEXT,a)
#define IPC_LOG_NERR1(a,b) \
	SLOG_1 (ipc_debug_sel & IPC_LOG_NERR,IPC_LOG_NERR_TEXT,a,b)
#define IPC_LOG_NERR2(a,b,c) \
	SLOG_2 (ipc_debug_sel & IPC_LOG_NERR,IPC_LOG_NERR_TEXT,a,b,c)
#define IPC_LOG_NERR3(a,b,c,d) \
	SLOG_3 (ipc_debug_sel & IPC_LOG_NERR,IPC_LOG_NERR_TEXT,a,b,c,d)
#define IPC_LOG_NERR4(a,b,c,d,e) \
	SLOG_4 (ipc_debug_sel & IPC_LOG_NERR,IPC_LOG_NERR_TEXT,a,b,c,d,e)
#define IPC_LOG_NERR5(a,b,c,d,e,f) \
	SLOG_5 (ipc_debug_sel & IPC_LOG_NERR,IPC_LOG_NERR_TEXT,a,b,c,d,e,f)
#define IPC_LOG_NERR6(a,b,c,d,e,f,g) \
	SLOG_6 (ipc_debug_sel & IPC_LOG_NERR,IPC_LOG_NERR_TEXT,a,b,c,d,e,f,g)

/* Continuation log macros	*/
#define IPC_LOG_NERRC0(a) \
	SLOGC_0 (ipc_debug_sel & IPC_LOG_NERR,a)
#define IPC_LOG_NERRC1(a,b) \
	SLOGC_1 (ipc_debug_sel & IPC_LOG_NERR,a,b)
#define IPC_LOG_NERRC2(a,b,c) \
	SLOGC_2 (ipc_debug_sel & IPC_LOG_NERR,a,b,c)
#define IPC_LOG_NERRC3(a,b,c,d) \
	SLOGC_3 (ipc_debug_sel & IPC_LOG_NERR,a,b,c,d)
#define IPC_LOG_NERRC4(a,b,c,d,e) \
	SLOGC_4 (ipc_debug_sel & IPC_LOG_NERR,a,b,c,d,e)
#define IPC_LOG_NERRC5(a,b,c,d,e,f) \
	SLOGC_5 (ipc_debug_sel & IPC_LOG_NERR,a,b,c,d,e,f)
#define IPC_LOG_NERRC6(a,b,c,d,e,f,g) \
	SLOGC_6 (ipc_debug_sel & IPC_LOG_NERR,a,b,c,d,e,f,g)

/* Hex log macro	*/
#define IPC_LOG_NERRH(a,b) \
	SLOGH (ipc_debug_sel & IPC_LOG_NERR,a,b)

/*********************** IPC_LOG_FLOW Macros *****************************/

#define IPC_LOG_FLOW0(a) \
	SLOG_0 (ipc_debug_sel & IPC_LOG_FLOW,IPC_LOG_FLOW_TEXT,a)
#define IPC_LOG_FLOW1(a,b) \
	SLOG_1 (ipc_debug_sel & IPC_LOG_FLOW,IPC_LOG_FLOW_TEXT,a,b)
#define IPC_LOG_FLOW2(a,b,c) \
	SLOG_2 (ipc_debug_sel & IPC_LOG_FLOW,IPC_LOG_FLOW_TEXT,a,b,c)
#define IPC_LOG_FLOW3(a,b,c,d) \
	SLOG_3 (ipc_debug_sel & IPC_LOG_FLOW,IPC_LOG_FLOW_TEXT,a,b,c,d)
#define IPC_LOG_FLOW4(a,b,c,d,e) \
	SLOG_4 (ipc_debug_sel & IPC_LOG_FLOW,IPC_LOG_FLOW_TEXT,a,b,c,d,e)
#define IPC_LOG_FLOW5(a,b,c,d,e,f) \
	SLOG_5 (ipc_debug_sel & IPC_LOG_FLOW,IPC_LOG_FLOW_TEXT,a,b,c,d,e,f)
#define IPC_LOG_FLOW6(a,b,c,d,e,f,g) \
	SLOG_6 (ipc_debug_sel & IPC_LOG_FLOW,IPC_LOG_FLOW_TEXT,a,b,c,d,e,f,g)

/* Continuation log macros	*/
#define IPC_LOG_FLOWC0(a) \
	SLOGC_0 (ipc_debug_sel & IPC_LOG_FLOW,a)
#define IPC_LOG_FLOWC1(a,b) \
	SLOGC_1 (ipc_debug_sel & IPC_LOG_FLOW,a,b)
#define IPC_LOG_FLOWC2(a,b,c) \
	SLOGC_2 (ipc_debug_sel & IPC_LOG_FLOW,a,b,c)
#define IPC_LOG_FLOWC3(a,b,c,d) \
	SLOGC_3 (ipc_debug_sel & IPC_LOG_FLOW,a,b,c,d)
#define IPC_LOG_FLOWC4(a,b,c,d,e) \
	SLOGC_4 (ipc_debug_sel & IPC_LOG_FLOW,a,b,c,d,e)
#define IPC_LOG_FLOWC5(a,b,c,d,e,f) \
	SLOGC_5 (ipc_debug_sel & IPC_LOG_FLOW,a,b,c,d,e,f)
#define IPC_LOG_FLOWC6(a,b,c,d,e,f,g) \
	SLOGC_6 (ipc_debug_sel & IPC_LOG_FLOW,a,b,c,d,e,f,g)

/* Hex log macro	*/
#define IPC_LOG_FLOWH(a,b) \
	SLOGH (ipc_debug_sel & IPC_LOG_FLOW,a,b)

/*********************** IPC_LOG_DEBUG Macros *****************************/

#define IPC_LOG_DEBUG0(a) \
	SLOG_0 (ipc_debug_sel & IPC_LOG_DEBUG,IPC_LOG_DEBUG_TEXT,a)
#define IPC_LOG_DEBUG1(a,b) \
	SLOG_1 (ipc_debug_sel & IPC_LOG_DEBUG,IPC_LOG_DEBUG_TEXT,a,b)
#define IPC_LOG_DEBUG2(a,b,c) \
	SLOG_2 (ipc_debug_sel & IPC_LOG_DEBUG,IPC_LOG_DEBUG_TEXT,a,b,c)
#define IPC_LOG_DEBUG3(a,b,c,d) \
	SLOG_3 (ipc_debug_sel & IPC_LOG_DEBUG,IPC_LOG_DEBUG_TEXT,a,b,c,d)
#define IPC_LOG_DEBUG4(a,b,c,d,e) \
	SLOG_4 (ipc_debug_sel & IPC_LOG_DEBUG,IPC_LOG_DEBUG_TEXT,a,b,c,d,e)
#define IPC_LOG_DEBUG5(a,b,c,d,e,f) \
	SLOG_5 (ipc_debug_sel & IPC_LOG_DEBUG,IPC_LOG_DEBUG_TEXT,a,b,c,d,e,f)
#define IPC_LOG_DEBUG6(a,b,c,d,e,f,g) \
	SLOG_6 (ipc_debug_sel & IPC_LOG_DEBUG,IPC_LOG_DEBUG_TEXT,a,b,c,d,e,f,g)

/* Continuation log macros	*/
#define IPC_LOG_DEBUGC0(a) \
	SLOGC_0 (ipc_debug_sel & IPC_LOG_DEBUG,a)
#define IPC_LOG_DEBUGC1(a,b) \
	SLOGC_1 (ipc_debug_sel & IPC_LOG_DEBUG,a,b)
#define IPC_LOG_DEBUGC2(a,b,c) \
	SLOGC_2 (ipc_debug_sel & IPC_LOG_DEBUG,a,b,c)
#define IPC_LOG_DEBUGC3(a,b,c,d) \
	SLOGC_3 (ipc_debug_sel & IPC_LOG_DEBUG,a,b,c,d)
#define IPC_LOG_DEBUGC4(a,b,c,d,e) \
	SLOGC_4 (ipc_debug_sel & IPC_LOG_DEBUG,a,b,c,d,e)
#define IPC_LOG_DEBUGC5(a,b,c,d,e,f) \
	SLOGC_5 (ipc_debug_sel & IPC_LOG_DEBUG,a,b,c,d,e,f)
#define IPC_LOG_DEBUGC6(a,b,c,d,e,f,g) \
	SLOGC_6 (ipc_debug_sel & IPC_LOG_DEBUG,a,b,c,d,e,f,g)

/* Hex log macro	*/
#define IPC_LOG_DEBUGH(a,b) \
	SLOGH (ipc_debug_sel & IPC_LOG_DEBUG,a,b)

/*********************** IPC_LOG_RX Macros *****************************/

#define IPC_LOG_RX0(a) \
	SLOG_0 (ipc_debug_sel & IPC_LOG_RX,IPC_LOG_RX_TEXT,a)
#define IPC_LOG_RX1(a,b) \
	SLOG_1 (ipc_debug_sel & IPC_LOG_RX,IPC_LOG_RX_TEXT,a,b)
#define IPC_LOG_RX2(a,b,c) \
	SLOG_2 (ipc_debug_sel & IPC_LOG_RX,IPC_LOG_RX_TEXT,a,b,c)
#define IPC_LOG_RX3(a,b,c,d) \
	SLOG_3 (ipc_debug_sel & IPC_LOG_RX,IPC_LOG_RX_TEXT,a,b,c,d)
#define IPC_LOG_RX4(a,b,c,d,e) \
	SLOG_4 (ipc_debug_sel & IPC_LOG_RX,IPC_LOG_RX_TEXT,a,b,c,d,e)
#define IPC_LOG_RX5(a,b,c,d,e,f) \
	SLOG_5 (ipc_debug_sel & IPC_LOG_RX,IPC_LOG_RX_TEXT,a,b,c,d,e,f)

/* Continuation log macros	*/
#define IPC_LOG_RXC0(a) \
	SLOGC_0 (ipc_debug_sel & IPC_LOG_RX,a)
#define IPC_LOG_RXC1(a,b) \
	SLOGC_1 (ipc_debug_sel & IPC_LOG_RX,a,b)
#define IPC_LOG_RXC2(a,b,c) \
	SLOGC_2 (ipc_debug_sel & IPC_LOG_RX,a,b,c)
#define IPC_LOG_RXC3(a,b,c,d) \
	SLOGC_3 (ipc_debug_sel & IPC_LOG_RX,a,b,c,d)
#define IPC_LOG_RXC4(a,b,c,d,e) \
	SLOGC_4 (ipc_debug_sel & IPC_LOG_RX,a,b,c,d,e)
#define IPC_LOG_RXC5(a,b,c,d,e,f) \
	SLOGC_5 (ipc_debug_sel & IPC_LOG_RX,a,b,c,d,e,f)

/* Hex log macro	*/
#define IPC_LOG_RXH(a,b) \
	SLOGH (ipc_debug_sel & IPC_LOG_RX,a,b)

/*********************** IPC_LOG_TX Macros *****************************/

#define IPC_LOG_TX0(a) \
	SLOG_0 (ipc_debug_sel & IPC_LOG_TX,IPC_LOG_TX_TEXT,a)
#define IPC_LOG_TX1(a,b) \
	SLOG_1 (ipc_debug_sel & IPC_LOG_TX,IPC_LOG_TX_TEXT,a,b)
#define IPC_LOG_TX2(a,b,c) \
	SLOG_2 (ipc_debug_sel & IPC_LOG_TX,IPC_LOG_TX_TEXT,a,b,c)
#define IPC_LOG_TX3(a,b,c,d) \
	SLOG_3 (ipc_debug_sel & IPC_LOG_TX,IPC_LOG_TX_TEXT,a,b,c,d)
#define IPC_LOG_TX4(a,b,c,d,e) \
	SLOG_4 (ipc_debug_sel & IPC_LOG_TX,IPC_LOG_TX_TEXT,a,b,c,d,e)
#define IPC_LOG_TX5(a,b,c,d,e,f) \
	SLOG_5 (ipc_debug_sel & IPC_LOG_TX,IPC_LOG_TX_TEXT,a,b,c,d,e,f)

/* Continuation log macros	*/
#define IPC_LOG_TXC0(a) \
	SLOGC_0 (ipc_debug_sel & IPC_LOG_TX,a)
#define IPC_LOG_TXC1(a,b) \
	SLOGC_1 (ipc_debug_sel & IPC_LOG_TX,a,b)
#define IPC_LOG_TXC2(a,b,c) \
	SLOGC_2 (ipc_debug_sel & IPC_LOG_TX,a,b,c)
#define IPC_LOG_TXC3(a,b,c,d) \
	SLOGC_3 (ipc_debug_sel & IPC_LOG_TX,a,b,c,d)
#define IPC_LOG_TXC4(a,b,c,d,e) \
	SLOGC_4 (ipc_debug_sel & IPC_LOG_TX,a,b,c,d,e)
#define IPC_LOG_TXC5(a,b,c,d,e,f) \
	SLOGC_5 (ipc_debug_sel & IPC_LOG_TX,a,b,c,d,e,f)

/* Hex log macro	*/
#define IPC_LOG_TXH(a,b) \
	SLOGH (ipc_debug_sel & IPC_LOG_TX,a,b)


#ifdef __cplusplus
}
#endif

#endif /* !SOCK_UTIL_INCLUDED */

