/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	2005 - 2012, All Rights Reserved				*/
/*									*/
/* MODULE NAME : ipc_utl.c						*/
/* PRODUCT(S)  : 		 					*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*	IPC (using socket or UNIX domain socket) 'utility' functions.	*/ 
/*	The write_fd and recvwait_fd functions allow to pass a socket	*/
/*      handle between processes.					*/
/*	These functions are general enough to be used in any project.	*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*			recvwait					*/
/*			sendwait					*/
/*			write_fd					*/
/*			recvwait_fd					*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev     Comments					*/
/* --------  ---  ------   -------------------------------------------  */
/* 01/11/12 EJV            Rpl SOCKET_ERROR with SOCKET_ERRORNO macro.	*/
/* 01/20/10 JRB     04     Fix log messages.				*/
/* 01/05/09 EJV     03	   Elim warnings on Linux.			*/
/* 02/09/08 EJV     02	   Redesigned func allow partial send/recv.	*/
/* 02/23/07 EJV     01	   Created from other modules.			*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"

#include "glbsem.h"
#include "stime.h"
#include "ipc_utl.h"

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/
/************************************************************************/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/
/* Global logging variables */

ST_UINT ipc_debug_sel = IPC_LOG_ERR;

#ifdef DEBUG_SISCO
LOGCFGX_VALUE_MAP ipcLogMaskMaps[] =
  {
    {"IPC_LOG_ERR",	IPC_LOG_ERR,	&ipc_debug_sel, _LOGCFG_DATATYPE_UINT_MASK, NULL, "Error"},
    {"IPC_LOG_NERR",	IPC_LOG_NERR,	&ipc_debug_sel, _LOGCFG_DATATYPE_UINT_MASK, NULL, "Notice"},
    {"IPC_LOG_FLOW",	IPC_LOG_FLOW,	&ipc_debug_sel, _LOGCFG_DATATYPE_UINT_MASK, NULL, "Flow"},
    {"IPC_LOG_DEBUG",	IPC_LOG_DEBUG,	&ipc_debug_sel, _LOGCFG_DATATYPE_UINT_MASK, NULL, "Debug"},
    {"IPC_LOG_RX",	IPC_LOG_RX,	&ipc_debug_sel, _LOGCFG_DATATYPE_UINT_MASK, NULL, "Receive"},
    {"IPC_LOG_TX",	IPC_LOG_TX,	&ipc_debug_sel, _LOGCFG_DATATYPE_UINT_MASK, NULL, "Transmit"}
  };

LOGCFG_VALUE_GROUP ipcLogMaskMapCtrl =
  {
  {NULL,NULL},
  "IpcLogMasks",
  sizeof(ipcLogMaskMaps)/sizeof(LOGCFGX_VALUE_MAP),
  ipcLogMaskMaps
  };
#endif /* DEBUG_SISCO */

SD_CONST ST_CHAR *SD_CONST  IPC_LOG_NERR_TEXT  = "IPC_LOG_NERR";
SD_CONST ST_CHAR *SD_CONST  IPC_LOG_ERR_TEXT   = "IPC_LOG_ERR";
SD_CONST ST_CHAR *SD_CONST  IPC_LOG_FLOW_TEXT  = "IPC_LOG_FLOW";
SD_CONST ST_CHAR *SD_CONST  IPC_LOG_DEBUG_TEXT = "IPC_LOG_DEBUG";
SD_CONST ST_CHAR *SD_CONST  IPC_LOG_RX_TEXT    = "IPC_LOG_RX";
SD_CONST ST_CHAR *SD_CONST  IPC_LOG_TX_TEXT    = "IPC_LOG_TX";


/************************************************************************/
/*			recvwait					*/
/*----------------------------------------------------------------------*/
/* The recv() doesn't always return the number of bytes requested.	*/
/* Depending on timeout and startTm parameters this fun can loop until:	*/
/* all bytes are received, timeout occurs or socket error. 		*/
/* The startTm tells if the start time is kept locally or is passed to	*/
/* this function.							*/
/*									*/
/* Parameters:								*/
/*   SOCKET 	fd		Handle to socket			*/
/*   ST_VOID   *recvbuf		pointer to receive buffer		*/
/*   ST_INT     numbytes	num of bytes to receive			*/
/*   int        flags		see man for flags param to recv() funct	*/
/*   ST_CHAR   *sockName	for uniform logging, if NULL passed the	*/
/*				log will use "Socket %d:..." format.	*/
/*   ST_INT     timeout		in ms,					*/
/*				if -1 loop FOREVER until numBytes recvd,*/
/*				if  0 try to receive one time & return,	*/
/*				if >0 try to recv until timeout reached.*/
/*   ST_DOUBLE *startTm		Ptr to start time (when timeout>0),	*/
/*				if  NULL, loop receiving until timeout	*/
/*				if !NULL, try receive, check timeout and*/
/*				  return (use passed startTime)		*/
/* RETURNS:								*/
/*	>0			number of bytes received		*/
/*	=0			0 bytes received & timeout not reached yet*/
/*	-1 (RET_SOCK_ERR)	if peer disconnected or socket error	*/
/*	-2 (RET_TIMEOUT)	if timeout				*/
/************************************************************************/

ST_INT recvwait (SOCKET fd, ST_VOID *buf, ST_INT numBytes, int flags, ST_CHAR *sockName,
                 ST_INT timeout, ST_DOUBLE *startTm)
  {
#ifdef DEBUG_SISCO
ST_CHAR *sock_name = (sockName == NULL ? "Socket" : sockName);
#endif
ST_INT   totalRecv = 0;	/* counter for # of bytes received. Init to 0.	*/
int      numRecv;
int      err;
ST_DOUBLE startTime = 0.0;

  if (timeout > 0)
    {
    if (startTm)
      startTime = *startTm;		/* use passed start time	*/
    else
      startTime = sGetMsTime();		/* we wait in this fun		*/
    }

  /* NOTES:								*/
  /*  recv() will block if the socket is in a blocking mode.		*/
  /* if timeout<0 loop FOREVER !					*/

  while (totalRecv < numBytes)
    {
    /* NOTE: Cast needed on buf because can't add "totalRecv" to void ptr.	*/
    numRecv = (int) recv (fd, ((ST_CHAR *) buf) + totalRecv, numBytes - totalRecv, flags);
    if (numRecv > 0)
      {
      totalRecv += numRecv;	/* add received bytes to totalRecv	*/
      /* will try again if numBytes was not received */
      }
    else if (numRecv == 0)	/* 0 if peer disconnected		*/
      {
      IPC_LOG_FLOW2 ("%s %d: recv() failed (peer closed socket).", sock_name, fd);
      totalRecv = RET_SOCK_ERR;	/* return -1	*/
      break;			/* stop looping				*/
      }
    else  /* numRecv < 0 (-1), some kind of error	*/
      {
      err = SOCKET_ERRORNO;
      if (err == SOCK_WOULDBLOCK || err == SOCK_INTR ||
          err == SOCK_TIMEDOUT   || err == SOCK_INPROGRESS)
        {
        if (timeout == 0)	 /* no wait requested			*/
	  {
          IPC_LOG_NERR3 ("%s %d: recv() failed errno=%d", sock_name, fd, err);
          break;	/* will return current totalRecv >=0 */
	  }
        else if (timeout > 0)
	  {
	  /* check for timeout */
          if (sGetMsTime() - startTime > timeout)
	    {
            IPC_LOG_FLOW4 ("%s %d: timed out calling recv(), errno=%d, timeout=%d ms",
                           sock_name, fd, err, timeout);
            totalRecv = RET_TIMEOUT;	/* ret code for timeout		*/
	    break;			/* we waited long enough	*/
            }
          else 
            {
            /* check if we should return to calling function or wait */
            if (startTm)
	      break;		/* return current totalRecv (bytes received until now >= 0) */
            else		/* caller wants to wait until timeout	*/
              sMsSleep(5);	/* give up the rest of time slice, and try to receive more bytes again	*/
            }
	  }
        else			/* timeout<0, INFINITE LOOP until send successful or error occurs */
          sMsSleep(5);		/* give up the rest of time slice, have to wait for more bytes	*/
	}
      else  /* non-recoverable error */
        {
        IPC_LOG_FLOW3 ("%s %d: recv() failed errno=%d", sock_name, fd, err);
        totalRecv = numRecv;	/* return -1 to close socket	*/
        break;			/* stop looping			*/
        }
      }
    }	/* end loop	*/

  if (totalRecv > 0)
    {
    S_LOCK_UTIL_RESOURCES ();
    IPC_LOG_RX4 ("%s %d: received %d bytes out of %d expected", sock_name, fd, totalRecv, numBytes);
    IPC_LOG_RXH (totalRecv, buf);
    S_UNLOCK_UTIL_RESOURCES ();
    }

  return (totalRecv);
  }

/************************************************************************/
/*			sendwait					*/
/*----------------------------------------------------------------------*/
/* The send() doesn't always return the number of bytes requested.	*/
/* This function can loops until all bytes are sent or non-recoverable	*/
/* socket error. The startTm indicates if the start time is passed in	*/
/* from calling funtion.						*/
/* Parameters:								*/
/*   SOCKET 	fd		Handle to socket			*/
/*   ST_VOID   *buf		pointer to send buffer			*/
/*   ST_INT     numBytes	num of bytes to send			*/
/*   int        flags		see man for flags param to send() funct	*/
/*   ST_CHAR   *sockName	for uniform logging, if NULL passed the	*/
/*				log will use "Socket %d:..." format.	*/
/*   ST_INT     timeout		in ms,					*/
/*				if -1 loop FOREVER until numBytes sent,	*/
/*				if  0 try to send one time & return,	*/
/*				if >0 try to send until timeout reached.*/
/*   ST_DOUBLE *startTm		Ptr to start time (valid when timeout>0)*/
/*				if  NULL, loop sending until timeout	*/
/*				if !NULL, try sending, check timeout and*/
/*				  return (use passed startTime)		*/
/* RETURNS:								*/
/*	>0			number of bytes sent			*/
/*	=0			0 bytes sent & timeout not reached yet	*/
/*	-1 (RET_SOCK_ERR)	if peer disconnected or socket error	*/
/*	-2 (RET_TIMEOUT)	if timeout				*/
/************************************************************************/

ST_INT sendwait (SOCKET fd, ST_VOID *buf, ST_INT numBytes, int flags, ST_CHAR *sockName,
                 ST_INT timeout, ST_DOUBLE *startTm)
  {
#ifdef DEBUG_SISCO
ST_CHAR  *sock_name = (sockName == NULL ? "Socket" : sockName);
#endif
ST_INT    totalSent = 0;	/* counter for # of bytes sent. Init to 0.	*/
int       numSent;
int       err;
ST_DOUBLE startTime = 0.0;
ST_CHAR  *sendPtr;
ST_INT    sendLen;

  if (timeout > 0)
    {
    if (startTm)
      startTime = *startTm;		/* use passed start time	*/
    else
      startTime = sGetMsTime();		/* we wait in this fun		*/
    }

  /* NOTES:								*/
  /* ! send() will block if the socket is in a blocking mode.		*/
  /* if timeout<0 loop FOREVER !					*/

  while (totalSent < numBytes)
    {
    sendPtr = ((ST_CHAR *) buf) + totalSent;
    sendLen = numBytes - totalSent;
    /* NOTE: Cast needed on buf because can't add "totalSent" to void ptr.	*/
    numSent = (int) send (fd, sendPtr, sendLen, flags);
    if (numSent > 0)
      {
      totalSent += numSent;	/* add sent bytes to totalSent		*/
      /* try again if numBytes was not sent */
      }
    else  /* numSent <= 0   some kind of error	*/
      {
      err = SOCKET_ERRORNO;
      if (numSent == 0 ||	/* 0 bytes, handle here to avoid endless loop */
          err == SOCK_WOULDBLOCK || err == SOCK_INTR || err == SOCK_NOBUFS ||
          err == SOCK_TIMEDOUT   || err == SOCK_INPROGRESS)
        {
        if (timeout == 0)	 /* no wait requested			*/
	  {
          IPC_LOG_NERR3 ("%s %d: send() failed errno=%d", sock_name, fd, err);
          break;	/* will return current totalSent >=0 */
	  }
        else if (timeout > 0)
          {
	  /* check for timeout */
          if (sGetMsTime() - startTime > timeout)
            {
            IPC_LOG_NERR4 ("%s %d: timed out calling send(), errno=%d, timeout=%d ms",
                           sock_name, fd, err, timeout);
            totalSent = RET_TIMEOUT;	/* ret code for timeout		*/
            break;			/* we waited long enough	*/
            }
          else
            {
            /* check if we should return to calling function */
            if (startTm)
              break;		/* return current totalSent (bytes sent until now) */
            else		/* caller wants to wait until timeout	*/
              sMsSleep(5);	/* give up the rest of time slice, and try to send more bytes	*/
            }
          }
        else			/* timeout<0, INFINITE LOOP until send successful or error occurs	*/
          sMsSleep(5);		/* give up the rest of time slice, have to send more bytes	*/
        }
      else  /* non-recoverable error */
        {
        IPC_LOG_FLOW4 ("%s %d: socket send() failed numSent=%d, errno=%d",
                        sock_name, fd, numSent, err);
        totalSent = RET_SOCK_ERR;	/* return -1 to close socket	*/
        break;	/* stop looping	*/
        }
      }
    }	/* end loop	*/

  if (totalSent > 0)
    {
    S_LOCK_UTIL_RESOURCES ();
    IPC_LOG_TX4 ("%s %d: sent %d bytes out of %d", sock_name, fd, totalSent, numBytes);
    IPC_LOG_TXH (totalSent, buf);
    S_UNLOCK_UTIL_RESOURCES ();
    }

  return (totalSent);
  }

#if defined(_WIN32)
/************************************************************************/
/*			write_fd					*/
/*----------------------------------------------------------------------*/
/* This function passes the TCP socket handle to the user application.	*/
/* This function can loops until all bytes are sent or non-recoverable	*/
/* socket error. The startTm indicates if the start time is passed in	*/
/* from calling funtion.						*/
/*									*/
/* Paramaters:								*/
/*   SOCKET 	fd		Handle to socket			*/
/*   SOCKET 	hTcpSock	TCP socket handle to pass to another	*/
/*				process.				*/
/*   DWORD	pid		PID needed for handle passing on Win.	*/
/*   ST_CHAR   *sockName	for uniform logging, if NULL passed the	*/
/*				log will use "Socket %d:..." format.	*/
/*   ST_INT     timeout		in ms, see sendwait			*/
/*   ST_DOUBLE *startTm		see sendwait				*/
/* RETURN:								*/
/*	>0			number of bytes sent, for successful	*/
/*				handle passing the rtn needs to be 	*/
/*				rtn=sizeof (WSAPROTOCOL_INFO)		*/
/*	=0			0 bytes sent & timeout not reached yet	*/
/*	-1 (RET_SOCK_ERR)	if peer disconnected or socket error	*/
/*	-2 (RET_TIMEOUT)	if timeout				*/
/************************************************************************/

/* NOTE: this function is not used by SNAP-Lite (WSAPROTOCOL_INFO stored in tcp_con) */

ST_INT write_fd (SOCKET fd, SOCKET hTcpSock, DWORD pid, ST_CHAR *sockName,
                 ST_INT timeout, ST_DOUBLE *startTm)
  {
ST_CHAR         *sock_name = (sockName == NULL ? "Socket" : sockName);
ST_INT           totalSent = RET_SOCK_ERR;
WSAPROTOCOL_INFO protocolInfo;

  IPC_LOG_FLOW3 ("%s %d:  in write_fd, sizeof(WSAPROTOCOL_INFO)=%u", sock_name, fd,
                 sizeof (WSAPROTOCOL_INFO));

  memset (&protocolInfo, 0,  sizeof (WSAPROTOCOL_INFO));
  if (WSADuplicateSocket (hTcpSock, pid, &protocolInfo) == 0)
    {
    totalSent = sendwait (fd, &protocolInfo, sizeof (WSAPROTOCOL_INFO), 0, sockName,
                          timeout, startTm);
    if (totalSent == sizeof (WSAPROTOCOL_INFO))
      IPC_LOG_FLOW3 ("%s %d: TCP socket handle=%d passed suceesfully.",
                     sock_name, fd, hTcpSock);
    }
  else
    {
    IPC_LOG_NERR4 ("%s %d:  WSADuplicateSocket for TCP socket handle=%d failed error=%d.",
		    sock_name, fd, hTcpSock, SOCKET_ERRORNO);
    totalSent = RET_SOCK_ERR;
    }

  return (totalSent);
  }

#else /* Linux, UNIX */

/************************************************************************/
/*			write_fd					*/
/*----------------------------------------------------------------------*/
/* This function passes the TCP socket handle to the user application.	*/
/* NOTE: there is a different msghdr that includes "msg_accrights"	*/
/*    on some older systems. See "UNIX Network Programming" book	*/
/*    if we ever need it.						*/
/*									*/
/* Paramaters:								*/
/*   SOCKET 	fd		Handle to socket			*/
/*   SOCKET 	hTcpSock	TCP socket handle to pass to another	*/
/*				process.				*/
/*   ST_CHAR   *sockName	for uniform logging, if NULL passed the	*/
/*				log will use "Socket %d:..." format.	*/
/*   ST_INT     timeout		in ms,					*/
/*				if -1 loop FOREVER until numBytes sent,	*/
/*				if  0 try to send one time & return,	*/
/*				if >0 try to send until timeout reached.*/
/*   ST_DOUBLE *startTm		Ptr to start time (valid when timeout>0)*/
/*				if  NULL, loop sending until timeout	*/
/*				if !NULL, try sending, check timeout and*/
/*				  return (use passed startTime)		*/
/* RETURN:								*/
/*	=1			1 byte sent and handle passed OK	*/
/*	=0			0 bytes sent & timeout not reached yet	*/
/*	-1 (RET_SOCK_ERR)	if peer disconnected or socket error	*/
/*	-2 (RET_TIMEOUT)	if timeout				*/
/************************************************************************/

ST_RET write_fd (SOCKET fd, SOCKET hTcpSock, ST_CHAR *sockName,
                 ST_INT timeout, ST_DOUBLE *startTm)
  {
#ifdef DEBUG_SISCO
ST_CHAR      *sock_name = (sockName == NULL ? "Socket" : sockName);
#endif
ST_INT        totalSent = 0;
ssize_t       numSent;
struct msghdr msg;
struct iovec  iov[1];
int           err;
ST_CHAR       dummy_byte = 7;  /* 1 byte msg to write with write_fd, any value */
ST_DOUBLE     startTime = 0.0;

#if defined(OLD_WAY)
  /* NOTE: On HP-UX to use the msg_accrights fields compile without	*/
  /*       defining the -D_XOPEN_SOURCE_EXTENDED, -D_OPEN_SOURCE and	*/
  /*       without linking the -lxnet library).				*/
#else
  /* NOTE: On HP-UX to use the SCM_RIGHTS msg type compile with the	*/
  /*       -D_XOPEN_SOURCE_EXTENDED, -D_OPEN_SOURCE and link with the	*/
  /*       -lxnet library. Without linking the libxnet.* library the	*/
  /*       sendmsg() fails with EBADF error.				*/
  /*       See Richard Stevens "UNIX Network Programming" book.		*/
/* this just helps get alignment right	*/
union
  {
  struct cmsghdr cm;
  char control [CMSG_SPACE(sizeof(int))];
  } control_un;
struct cmsghdr *cmptr;
#endif

  IPC_LOG_FLOW2 ("%s %d:  in write_fd", sock_name, fd);

#if defined(OLD_WAY)
  msg.msg_accrights = (caddr_t) &hTcpSock;
  msg.msg_accrightslen = sizeof (int);
#else
  msg.msg_control = control_un.control;
  msg.msg_controllen = sizeof (control_un.control);
  cmptr = CMSG_FIRSTHDR(&msg);
  cmptr->cmsg_len = CMSG_LEN(sizeof(SOCKET));
  cmptr->cmsg_level = SOL_SOCKET;
  cmptr->cmsg_type = SCM_RIGHTS;
  *((SOCKET *) CMSG_DATA(cmptr)) = hTcpSock;
#endif
  msg.msg_name = NULL;
  msg.msg_namelen = 0;
  
  iov[0].iov_base = &dummy_byte;
  iov[0].iov_len = sizeof (dummy_byte);
  msg.msg_iov = iov;
  msg.msg_iovlen = 1;
  
  if (timeout > 0)
    {
    if (startTm)
      startTime = *startTm;		/* use passed start time	*/
    else
      startTime = sGetMsTime();		/* we wait in this fun		*/
    }

  IPC_LOG_FLOW3 ("%s %d: calling sendmsg() to pass TCP handle hTcpSock=%d.",
                 sock_name, fd, hTcpSock);

  /* we sending only 1 byte with this special message */
  while (totalSent < sizeof(dummy_byte))
    {
    numSent = sendmsg (fd, &msg, 0);
    if (numSent > 0)
      {
      totalSent += numSent;
      }
    else  /* numSent <= 0   some kind of error	*/
      {
      err = SOCKET_ERRORNO;
      if (numSent == 0 ||	/* 0 bytes, handle here to avoid endless loop */
          err == SOCK_WOULDBLOCK || err == SOCK_INTR || err == SOCK_NOBUFS ||
          err == SOCK_TIMEDOUT   || err == SOCK_INPROGRESS)
        {
        if (timeout == 0)	 /* no wait requested			*/
	  {
          IPC_LOG_NERR3 ("%s %d: sendmsg() failed errno=%d", sock_name, fd, err);
          break;	/* will return current totalSent >=0 */
	  }
        else if (timeout > 0)
          {
	  /* check for timeout */
          if (sGetMsTime() - startTime > timeout)
            {
            IPC_LOG_NERR4 ("%s %d: timed out calling sendmsg(), errno=%d, timeout=%d ms",
                           sock_name, fd, err, timeout);
            totalSent = RET_TIMEOUT;	/* ret code for timeout		*/
            break;			/* we waited long enough	*/
            }
          else
            {
            /* check if we should return to calling function */
            if (startTm)
              break;		/* return current totalSent (bytes sent until now) */
            else		/* caller wants to wait until timeout	*/
              sMsSleep(5);	/* give up the rest of time slice, and try to send more bytes	*/
            }
          }
        else			/* timeout<0, INFINITE LOOP until send successful or error occurs	*/
          sMsSleep(5);		/* give up the rest of time slice, have to send more bytes	*/
        }
      else  /* non-recoverable error */
        {
        IPC_LOG_NERR4 ("%s %d: failed to pass TCP socket handle=%d to user process, sendmsg() errno=%d.",
                        sock_name, fd, hTcpSock, err);
        totalSent = RET_SOCK_ERR;	/* return -1 to close socket	*/
        break;	/* stop looping	*/
	}
      }
    }	/* end loop	*/

  return (totalSent);	/* 1 if handle passed OK */
  }
#endif /* Linux, UNIX */


#if defined(_WIN32)
/************************************************************************/
/*			recvwait_fd					*/
/*----------------------------------------------------------------------*/
/* This is a special recv function only for receiving the socket handle	*/
/* from another process. 						*/
/* Depending on timeout and startTm parameters this fun can loop until:	*/
/* all bytes are received, timeout occurs or socket error. 		*/
/* The startTm tells if the start time is kept locally or is passed to	*/
/* this function.							*/
/*									*/
/* Paramaters:								*/
/*   SOCKET 	fd		Handle to socket			*/
/*   SOCKET    *hTcpSock	Pointer where to store received TCP	*/
/*				socket handle.				*/
/*   ST_CHAR   *sockName	for uniform logging, if NULL passed the	*/
/*				log will use "Socket %d:..." format.	*/
/*   ST_INT     timeout		in ms,					*/
/*				if -1 loop until numBytes received,	*/
/*				if  0 try to receive one time & return,	*/
/*				if >0 try to recv until timeout reached.*/
/*   ST_DOUBLE *startTm		Ptr to start time (when timeout>0),	*/
/*				if  NULL, loop receiving until timeout	*/
/*				if !NULL, try receive, check timeout and*/
/*				  return (use passed startTime)		*/
/* Return (same as from recvwait):					*/
/*	>0			number of bytes received		*/
/*	=0			0 bytes received & timeout not reached yet */
/*	-1 (RET_SOCK_ERR)	if peer disconnected or socket error	*/
/*	-2 (RET_TIMEOUT)	if timeout				*/
/************************************************************************/

/* NOTE: this function is not used by SNAP-Lite (WSAPROTOCOL_INFO stored in tcp_con) */

ST_INT recvwait_fd (SOCKET fd, SOCKET *hTcpSock, ST_CHAR *sockName,
                    ST_INT timeout, ST_DOUBLE *startTm)
  {
ST_CHAR         *sock_name = (sockName == NULL ? "Socket" : sockName);
ST_INT           numRecv;
WSAPROTOCOL_INFO protocolInfo;

  IPC_LOG_FLOW3 ("%s %d:  in recvwait_fd, sizeof(WSAPROTOCOL_INFO)=%u",
                  sock_name, fd, sizeof (WSAPROTOCOL_INFO));

  *hTcpSock = INVALID_SOCKET;

  numRecv = recvwait (fd, &protocolInfo, sizeof(WSAPROTOCOL_INFO), 0, sockName,
                      timeout, startTm);
  if (numRecv == sizeof(WSAPROTOCOL_INFO))
    {
    *hTcpSock = WSASocket (
			FROM_PROTOCOL_INFO,
			FROM_PROTOCOL_INFO,
			FROM_PROTOCOL_INFO,
			&protocolInfo,
			0,
			0);
    if (*hTcpSock == INVALID_SOCKET)
      {
      IPC_LOG_NERR3 ("%s %d: WSASocket() failed errno=%d", sock_name, fd, SOCKET_ERRORNO);
      numRecv = RET_SOCK_ERR;
      }
    else
      IPC_LOG_FLOW3 ("%s %d: received TCP handle=%d", sock_name, fd, (*hTcpSock));
    }

  return (numRecv);
  }

#else  /* UNIX, Linux */

/************************************************************************/
/*			recvwait_fd					*/
/*----------------------------------------------------------------------*/
/* This a special recv function only for receiving the socket handle	*/
/* from another process. It can wait untile the msg is received.		*/
/* DBLOCK. On any other error, it stops waiting and returns.	*/
/* Paramaters:								*/
/*   SOCKET 	fd		Handle to socket			*/
/*   SOCKET    *hTcpSock	Pointer where to store received TCP	*/
/*				socket handle.				*/
/*   ST_CHAR   *sockName	for uniform logging, if NULL passed the	*/
/*				log will use "Socket %d:..." format.	*/
/*   ST_INT     timeout		in ms,					*/
/*				if -1 loop until numBytes received,	*/
/*				if  0 try to receive one time & return,	*/
/*				if >0 try to recv until timeout reached.*/
/*   ST_DOUBLE *startTm		Ptr to start time (when timeout>0),	*/
/*				if  NULL, loop receiving until timeout	*/
/*				if !NULL, try receive, check timeout and*/
/*				  return (use passed startTime)		*/
/* Return:								*/
/*	>0			# of bytes received (currently 1)	*/
/*				(& received valid TCP handle)		*/
/*	=0			0 bytes received, but not timed out yet	*/
/*	-1 (RET_SOCK_ERR)	if peer disconnected or socket error	*/
/*	-2 (RET_TIMEOUT)	if timeout				*/
/************************************************************************/
ST_INT recvwait_fd (SOCKET fd, SOCKET *hTcpSock, ST_CHAR *sockName,
                    ST_INT timeout, ST_DOUBLE *startTm)
  {
#ifdef DEBUG_SISCO
ST_CHAR      *sock_name = (sockName == NULL ? "Socket" : sockName);
#endif
struct msghdr msg;
struct iovec  iov[1];
ssize_t       totalRecv = 0;	/* must be init to 0 !!!		*/
int           err;
char          dummy_byte; /* 1 byte msg to read, doesn't contain real info */
ST_DOUBLE     startTime = 0.0;

#if defined (OLD_WAY)
  /* NOTE: On HP-UX to use the msg_accrights fields compile without	*/
  /*       defining the -D_XOPEN_SOURCE_EXTENDED, -D_OPEN_SOURCE and	*/
  /*       without linking the -lxnet library).				*/
#else
  /* NOTE: On HP-UX to use the SCM_RIGHTS msg type compile with the	*/
  /*       -D_XOPEN_SOURCE_EXTENDED, -D_OPEN_SOURCE and link with the	*/
  /*       -lxnet library. Without linking the libxnet.* library the	*/
  /*       sendmsg() fails with EBADF error.				*/
  /*       See Richard Stevens "UNIX Network Programming" book.		*/

/* this just helps get alignment right	*/
union
  {
  struct cmsghdr cm;
  char control [CMSG_SPACE(sizeof(int))];
  } control_un;
struct cmsghdr *cmptr;
#endif
  
  IPC_LOG_FLOW2 ("%s %d: in recvwait_fd()", sock_name, fd);

  *hTcpSock = INVALID_SOCKET;  /* init to invalid handle in case the recvmsg fails	*/

#if defined (OLD_WAY)
  msg.msg_accrights = (caddr_t) recvfd;
  msg.msg_accrightslen = sizeof (int);		/* must be int			*/
#else
  msg.msg_control = control_un.control;
  msg.msg_controllen = sizeof (control_un.control);
#endif

  msg.msg_name = NULL;
  msg.msg_namelen = 0;
  
  iov[0].iov_base = &dummy_byte;
  iov[0].iov_len =  sizeof(dummy_byte);
  msg.msg_iov = iov;
  msg.msg_iovlen = 1;
  
  if (timeout > 0)
    {
    if (startTm)
      startTime = *startTm;		/* use passed start time	*/
    else
      startTime = sGetMsTime();		/* we wait in this fun		*/
    }

  /* Wait for data as long as error is SOCK_WOULDBLOCK. Stop on any other error.*/
  /* NOTE: at this time there is only 1 byte of dummy data receive with this	*/
  /*       special message, but if this is changed to more bytes then the	*/
  /*       non-blocking recvmsg function may return with partial bytes.		*/

  /* Open Source Group:								*/
  /* " Upon successful completion, recvmsg() shall return the length of the	*/
  /* message in bytes. If no messages are available to be received and the	*/
  /* peer has performed an orderly shutdown, recvmsg() shall return 0.		*/
  /* Otherwise, -1 shall be returned and errno set to indicate the error."	*/

  while (totalRecv < sizeof(dummy_byte))
    {
    totalRecv = recvmsg (fd, &msg, 0);
    if (totalRecv > 0)
      {
      break;  /* looks that we got the handle */
      }
    else if (totalRecv == 0)
      {
      IPC_LOG_NERR2 ("%s %d: recvmsg() failed, peer closed the socket.", sock_name, fd);
      totalRecv = RET_SOCK_ERR;	/* return -1 to close socket */
      break;
      }
    else /* totalRecv < 0 (-1), some kind of error	*/
      {
      err = SOCKET_ERRORNO;
      if (err == SOCK_WOULDBLOCK || err == SOCK_INTR ||
          err == SOCK_TIMEDOUT   || err == SOCK_INPROGRESS)
        {
        IPC_LOG_DEBUG3 ("%s %d: recvmsg() errno=%d", sock_name, fd, err);
        /* check is we want to still wait longer for the msg			*/
        if (timeout == 0)	 /* no wait requested			*/
          {
          IPC_LOG_NERR3 ("%s %d: recvmsg() failed errno=%d", sock_name, fd, err);
          break;
          }
        else if (timeout > 0)
          {
          if ((sGetMsTime() - startTime) >= timeout)
            {
            IPC_LOG_NERR4 ("%s %d: timed out calling recvmsg(), errno = %d, timeout=%d ms",
                           sock_name, fd, err, timeout);
            totalRecv = RET_TIMEOUT;	/* timeout, other side failed to send the socket handle	*/
	    break;
            }
          else
            {
            /* check if we should return to calling function or wait */
            if (startTm)
	      {
              totalRecv = 0;	/* 0 bytes received but timeout not elapsed	*/
	      break;
	      }
            else		/* caller wants to wait until timeout	*/
              {
              IPC_LOG_DEBUG2 ("%s %d: recvwait_fd  sleeping 5 ms", sock_name, fd);
              sMsSleep(5);	/* give up the rest of time slice, and try to receive more bytes again	*/
              }
            }
          }
        else			/* timeout<0, INFINITE LOOP until send successful or error occurs */
          sMsSleep(5);		/* give up the rest of time slice, have to wait for more bytes	*/
        }
      else  /* non-recoverable error */
        {
        IPC_LOG_NERR3 ("%s %d: recvmsg() failed errno=%d", sock_name, fd, err);
        break;	/* some non-recoverable socket error */
        }
      }
    }  /* end while loop */
  if (totalRecv < 0)
    {
    IPC_LOG_DEBUG3 ("%s %d: recvwait_fd  failed, returning totalRecv =%d", sock_name, fd, totalRecv);
    return (totalRecv);	/* error that we can't recover from	*/
    }

#if defined (OLD_WAY)

#else
  if ((cmptr = CMSG_FIRSTHDR(&msg)) != NULL &&
       cmptr->cmsg_len == CMSG_LEN(sizeof(int)))
    {
    if (cmptr->cmsg_level != SOL_SOCKET)
      {
      IPC_LOG_NERR2 ("%s %d: cmsg_level != SOL_SOCKET", sock_name, fd);
      /* DEBUG: error. Book uses err_quit.*/
      }
    else if (cmptr->cmsg_type != SCM_RIGHTS)
      {
      IPC_LOG_NERR2 ("%s %d: cmsg_type != SCM_RIGHTS", sock_name, fd);
      /* DEBUG: error. Book uses err_quit. */
      }
    else
      *hTcpSock = *((int *) CMSG_DATA(cmptr));
    }
  else
    *hTcpSock = INVALID_SOCKET;	/* descriptor was not passed	*/
#endif

  if (*hTcpSock == INVALID_SOCKET)
    {
    /* totalRecv = RET_SOCK_ERR; return 1 instead and let the calling fun handle this problem */
    IPC_LOG_NERR2 ("%s %d: FAILED to receive valid TCP socket handle", sock_name, fd);
    }
  else
    IPC_LOG_FLOW3 ("%s %d: received TCP socket handle=%d", sock_name, fd, *hTcpSock);

  return (totalRecv);  /* 1 byte if handle received */
  }
#endif  /* UNIX, Linux */


/************************************************************************/
/*			sock_event_pending				*/
/*----------------------------------------------------------------------*/
/* This function checks if the there are any events pending on a socket.*/
/* Note that this function does non-blocking select if timeout=0	*/
/* Paramaters:								*/
/*   SOCKET 	fd		Handle to socket			*/
/*   ST_INT     timeout		in ms,					*/
/*									*/
/* Return:								*/
/*  SOCK_EVENT_WRITE | SOCK_EVENT_READ (3)				*/
/*				socket is writable and read is pending	*/
/*  SOCK_EVENT_WRITE (2)	if socket writable			*/
/*  SOCK_EVENT_READ  (1)	if there are bytes to receive		*/
/*  SOCK_EVENT_NONE  (0)	no event				*/
/*  RET_SOCK_ERR    (-1) 	socket error				*/
/************************************************************************/
ST_INT sock_event_pending (SOCKET fd, ST_BOOLEAN chWrite, ST_INT timeout)
  {
ST_RET rtn = RET_SOCK_ERR;
fd_set readfds;
fd_set writefds;
fd_set exceptfds;
int    nready;
int    maxfd = 0;
struct timeval stTimeVal;
ST_INT err;

  if (fd == INVALID_SOCKET)
    {
    IPC_LOG_ERR0 ("Socket: INVALID_SOCKET passed to sock_event_pending");
    return (RET_SOCK_ERR);
    }

#if !defined(_WIN32)
  if (fd >= FD_SETSIZE)
    {	/* Can't use this socket because illegal to use in "select" call*/
    IPC_LOG_ERR1 ("Socket: socket num=%d > FD_SETSIZE passed to sock_event_pending", fd);
    return (RET_SOCK_ERR);	/* error */
    }
#endif

  /* CRITICAL: start with clean Fds.	*/
  FD_ZERO (&readfds);
  FD_ZERO (&writefds);
  FD_ZERO (&exceptfds);
  FD_SET (fd, &readfds );
  if (chWrite)
    FD_SET (fd, &writefds );

  maxfd = max (maxfd, (int) fd);
  
  /* 0 timeout on select. Don't want to wait here.	*/
  stTimeVal.tv_sec  =  timeout / 1000;
  stTimeVal.tv_usec = (timeout % 1000) * 1000;

  nready = select (maxfd+1, &readfds, &writefds, &exceptfds, &stTimeVal);
  if (nready > 0)
    {
    rtn = SOCK_EVENT_NONE;
    if (FD_ISSET (fd, &readfds))
      rtn |= SOCK_EVENT_READ;
    if (chWrite)
      {
      if (FD_ISSET (fd, &writefds))
        /* this one would be set most of the times unless there is some congestion */
        rtn |= SOCK_EVENT_WRITE;
      }
    return (rtn);
    }
  else if (nready == 0)
    return (SOCK_EVENT_NONE);	/* no events pending */
  else  /* nready < 0 */
    {
    err = SOCKET_ERRORNO;
    if (err == SOCK_INPROGRESS || err == SOCK_INTR)
      return (SOCK_EVENT_NONE);	/* no events pending */
  
    IPC_LOG_NERR2 ("Socket %d: select() returned errno=%d in sock_event_pending",
                   fd, err);
    rtn = RET_SOCK_ERR;
    }
  return (rtn);	/* all cases should be already handled above */
  }


