/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	1997-2002, All Rights Reserved					*/
/*									*/
/* MODULE NAME : tp0_w32.c						*/
/* PRODUCT(S)  : Lean-T Stack						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*	TP0 functions ported for WIN32 operating systems using Winsock.	*/
/*	Also ported for OS/2 because functions very similar.		*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev     Comments					*/
/* --------  ---  ------   -------------------------------------------  */
/* 09/22/04  JRB    35     Change "..Connect error.." log to NERR.	*/
/* 06/20/02  JRB    17     Save remote IP addr in sock_info after accept.*/
/* 05/16/02  JRB    16     spawn_read: Pass IP addr to Read task.	*/
/*			   Log connect_result on err (now holds errno).	*/
/* 08/01/01  JRB    15     WIN/OS2: Log only when pipe msg count	*/
/*			   increases by 100.				*/
/* 07/24/01  JRB    14     Set Read Thread priority lower so it cannot	*/
/*			   flood the pipe.				*/
/* 03/14/01  JRB    13     Use new SMEM allocation functions.		*/
/* 03/14/01  JRB    12     Chg arg to read thread to (SOCK_INFO *).	*/
/* 12/13/00  DWL    11     Added SD_FALSE param to gs_get_event_sem	*/
/* 08/15/00  EJV    10     WIN32: Del pipe_msg_crit_sec, pipe_msg_count.*/
/*			     Caused deadlock when pipe filled up on NT.	*/
/*			     Instead use PeekNamedPipe before ReadFile.	*/
/* 07/25/00  JRB    09     Close socket ONLY from sock_info_free.	*/
/* 05/18/00  JRB    08     Create TCP event sem in sockets_init,	*/
/*			   instead of spawn_listen.			*/
/* 10/28/99  JRB    07     Fix memory leak when abort received. Add	*/
/*			   free in np_event DISCONNECTING state.	*/
/* 09/30/99  JRB    06     Don't use "chk_*" functions for thread_info	*/
/*			   (was causing linked list tearing).		*/
/* 08/05/99  JRB    05     Use hTcpEvent, gs_*_event_sem.		*/
/* 10/22/98  JRB    04     Chg LOG_ALWAYS macros to LOG_REQ.		*/
/* 08/13/98  JRB    03     Lint cleanup.				*/
/* 12/09/97  JRB    02     Added __OS2__ support. OS/2 functions very	*/
/*			   simalar so included here.			*/
/* 08/01/97  JRB    01     Created (compatible with MMS-EASE 7.0).	*/
/************************************************************************/
static char *thisFileName = __FILE__;	/* For TP4_EXCEPT and slog macros*/

#include "glbtypes.h"
#include "sysincs.h"
#include "mem_chk.h"
#include "tp4api.h"		/* User definitions for tp4	*/
#include "tp4.h"		/* Internal definitions for tp4	*/
#include "clnp_log.h"

/************************************************************************/
/* Global variables.							*/
/************************************************************************/
ST_EVENT_SEM hTcpEvent;
ST_EVENT_SEM hEventListenGo;

#if defined (__OS2__)
HFILE  hPipeMainRead;	/* read handle for pipe		*/
HFILE  hPipeMainWrite;	/* write handle for pipe	*/
ST_INT pipe_msg_count;
#else	/* !__OS2__	*/
HANDLE hPipeMainRead;	/* read handle for pipe		*/
HANDLE hPipeMainWrite;	/* write handle for pipe	*/
#endif	/* !__OS2__	*/
ST_INT pipe_msg_count_max;	/* Max # msgs there ever was in pipe.	*/

/************************************************************************/
/*			sockets_init					*/
/* 1. Init Winsock.							*/
/* 2. Create Main Pipe.							*/
/* 3. Create Critical Section.						*/
/* 4. Create "TCP" Event Semaphore.					*/
/************************************************************************/
#if defined (__OS2__)	/* Totally different for OS/2	*/
ST_RET sockets_init (ST_INT max_num_conns)
  {
ST_UINT32 PipeSize;
ST_INT    sock_err;

  /* sockets initialization */
  if ((sock_err = sock_init()) != 0)
    {
    CLNP_LOG_ERR1 ("Can't initialize Sockets interface, err: %d", sock_err);
    return (SD_FAILURE);
    }

  PipeSize = 0;			/* System will use default size.	*/
  if (DosCreatePipe(&hPipeMainRead, &hPipeMainWrite, PipeSize) != 0)
    {
    CLNP_LOG_ERR0 ("Can't create pipe");
    return (SD_FAILURE);
    }

  /* Create "TCP" Event Semaphore.	*/
  if (!(hTcpEvent = gs_get_event_sem ()))
    {
    CLNP_LOG_ERR0 ("Error creating TCP Event Semaphore.");
    return (SD_FAILURE);
    }
  
  return (SD_SUCCESS);
  }
#else	/* !__OS2__	*/
ST_RET sockets_init (ST_INT max_num_conns)
  {
WSADATA	wsaData;	/* WinSock data	*/
DWORD PipeSize;

/* init WinSock interface	*/
  if (WSAStartup(0x0101, &wsaData))
    {
    CLNP_LOG_ERR0 ("Can't initialize WinSock interface");
    return (SD_FAILURE);
    }
  if (wsaData.wVersion != 0x0101)
    {
    CLNP_LOG_ERR0 ("WinSock 1.1 not supported");
    return (SD_FAILURE);
    }
  CLNP_LOG_REQ1 ("Max num sockets = %d", wsaData.iMaxSockets);
  if (wsaData.iMaxSockets < max_num_conns)
    {
    CLNP_LOG_ERR2("ERROR np_init: Max num conns (%d) > Max num sockets (%d)",
                 max_num_conns, wsaData.iMaxSockets);
    return (TP4E_INVAL_NUM_CONNS);
    }

/* create pipes and events we need	*/
  PipeSize = 0;			/* System will use default size.	*/
  if (!CreatePipe(&hPipeMainRead, &hPipeMainWrite, NULL, PipeSize))
    {
    CLNP_LOG_ERR0 ("Can't create pipe");
    return (SD_FAILURE);
    }

  /* Create "TCP" Event Semaphore.	*/
  if (!(hTcpEvent = gs_get_event_sem (SD_FALSE)))
    {
    CLNP_LOG_ERR0 ("Error creating TCP Event Semaphore.");
    return (SD_FAILURE);
    }
  
  return (SD_SUCCESS);
  }
#endif	/* !__OS2__	*/

/************************************************************************/
/*			sockets_end					*/
/************************************************************************/
ST_VOID sockets_end (ST_VOID)
  {
#if 0	/* DEBUG: OS seems to clean up OK. Do we need any of this?	*/
  close (hPipeMainRead);
  close (hPipeMainWrite);
#endif
  }
/************************************************************************/
/*			spawn_listen					*/
/* 2. Spawn "ThreadSockListen" thread.					*/
/* 3. Create "ListenGo" Event for signalling ThreadSockListen.		*/
/************************************************************************/
ST_RET spawn_listen (SOCKET hSock)
  {
#if defined (__OS2__)
  if (_beginthread (ThreadSockListen, NULL, 8192, (void *) hSock) == -1)
#else
  if (_beginthread (ThreadSockListen, 0, (void *) hSock) == -1)
#endif
    {
    CLNP_LOG_ERR0 ("Can't create 'Listen' Thread");
    return (SD_FAILURE);
    }

  /* Create "ListenGo" Event	*/
  if (!(hEventListenGo = gs_get_event_sem (SD_FALSE)))
    {
    CLNP_LOG_ERR0 ("Can't create 'Listen Go' event");
    return (SD_FAILURE);
    }

  return (SD_SUCCESS);
  }
/************************************************************************/
/*			spawn_read					*/
/* If connecting, the IP addr is passed to the thread (in sock_info)	*/
/* and the socket "connect" call is made from the thread.		*/
/************************************************************************/
ST_RET spawn_read (SOCK_INFO *sock_info)
  {
#if defined (__OS2__)
  if (_beginthread (ThreadSockRead, NULL, 8192, (void *) sock_info) == -1)
    {
    CLNP_LOG_ERR0 ("Can't create 'Read' Thread");
    return (SD_FAILURE);
    }
#else
/* DEBUG: _beginthread returns ulong, but SetThreadPriority expects HANDLE.*/
/* Are they the same?	*/
unsigned long threadHandle;

  if ((threadHandle = _beginthread (ThreadSockRead, 0, (void *) sock_info)) == -1)
    {
    CLNP_LOG_ERR0 ("Can't create 'Read' Thread");
    return (SD_FAILURE);
    }
  if (SetThreadPriority ((HANDLE) threadHandle, THREAD_PRIORITY_BELOW_NORMAL)
      == 0)
    {
    CLNP_LOG_ERR0 ("Can't change 'Read' Thread priority");
    }
#endif
  return (SD_SUCCESS);
  }
/************************************************************************/
/*			np_event					*/
/************************************************************************/
ST_BOOLEAN np_event (ST_VOID)
  {
SOCK_INFO *sock_info;
PIPE_MSG pipe_msg;
ST_ULONG BytesRead;
SOCKET hSockConnected;	/* Accepted socket.	*/
SOCKADDR_IN RemoteAddr;	/* remote TCP port and IP addr for accepted sock*/
ST_INT RemoteLen;	/* sizeof RemoteAddr	*/
ST_BOOLEAN do_read;	/* flag to tell if ReadFile should be called	*/
ST_INT pipe_msg_count_chk;	/* Count usable outside CriticalSection	*/
#if defined (_WIN32)
DWORD TotalBytesAvail=0;  /* for PeekNamedPipe */
#endif

  /* This code makes sure Pipe contains full msg before blocking read.	*/
#if defined (__OS2__)
  /* Could use DosPeekNPipe here if pipe is changed to "named" pipe.	*/
  DosEnterCritSec ();	/* Begin Crit Sec	*/
  pipe_msg_count_chk = pipe_msg_count;		/* Save count	*/
  if (pipe_msg_count > 0)
    {
    pipe_msg_count--;
    do_read = SD_TRUE;
    }
  else
    do_read = SD_FALSE;
  DosExitCritSec ();	/* End Crit Sec	*/
#else
  /* Only want to know how many bytes available in pipe, so most	*/
  /* arguments are 0 or NULL. Should be faster because no bytes copied.	*/
  if (PeekNamedPipe(hPipeMainRead,	// handle to pipe to copy from
                    (LPVOID) NULL,	// pointer to data buffer
                    (DWORD) 0,		// size, in bytes, of data buffer
                    (LPDWORD) NULL,	// ptr to number of bytes read
                    &TotalBytesAvail,	// ptr to total number of bytes avail
                    (LPDWORD) NULL)	// ptr to unread bytes in this message
      == 0)
    {
    CLNP_LOG_ERR1 ("ERROR peeking on pipe %d", GetLastError ());
    TotalBytesAvail = 0;	/* in case Peek changed it before failing*/
    }
  pipe_msg_count_chk = TotalBytesAvail/sizeof(pipe_msg);
  if (pipe_msg_count_chk > 0)
    do_read = SD_TRUE;
  else
    do_read = SD_FALSE;
#endif  /* !__OS2__ */

  if (pipe_msg_count_chk > pipe_msg_count_max + 100)
    {
    CLNP_LOG_ERR1 ("INFO: np_event: Num msgs in pipe = %d", pipe_msg_count_chk);
    pipe_msg_count_max = pipe_msg_count_chk;
    }
  if (do_read == SD_FALSE)
    return (SD_FALSE);

#if defined (__OS2__)
  if (DosRead(hPipeMainRead, &pipe_msg, sizeof (pipe_msg), &BytesRead) != 0)
#else
  if (!ReadFile(hPipeMainRead, &pipe_msg, sizeof (pipe_msg), &BytesRead, NULL) )
#endif
    {
    CLNP_LOG_ERR1 ("ERROR: np_event: ReadFile error %d", GetLastError());
    return (SD_FALSE);
    }
  if (BytesRead != sizeof (pipe_msg))
    {
    CLNP_LOG_ERR0 ("ERROR: np_event: Incomplete msg read from pipe.");
    return (SD_FALSE);
    }

  /* If sock_info is NULL, this msg is from ThreadSockListen. It means	*/
  /* a connect was received.  Call "accept" now to accept it, and	*/
  /* allocate new "sock_info".						*/
  if (pipe_msg.sock_info)
    sock_info = pipe_msg.sock_info;
  else
    {		/* sock_info = NULL, means msg from ThreadSockListen	*/
    RemoteLen = sizeof(RemoteAddr);
    hSockConnected = accept(hSockListen, (SOCKADDR *) &RemoteAddr,
                   &RemoteLen);

    gs_signal_event_sem (hEventListenGo);	/* Signal Listen thread.*/

    if (hSockConnected != INVALID_SOCKET)
      {
      sock_info = sock_info_alloc (hSockConnected, SOCK_STATE_ACCEPTED,
                  INVALID_CONN_ID);
      sock_info->ip_addr = RemoteAddr.sin_addr.s_addr;	/* Save remote IP addr*/
      if (spawn_read (sock_info) != SD_SUCCESS)
        {
        CLNP_LOG_ERR0 ("ERROR spawning Read Task/Thread");
        sock_info_free (sock_info);
        return (SD_FALSE);
        }
      }
    else
      {
      CLNP_LOG_ERR0 ("ERROR: Socket 'accept' failed.");
      return (SD_FALSE);
      }
    }

  switch (sock_info->state)
    {
    case SOCK_STATE_ACCEPTED:		/* From ThreadSockListen.	*/
      handle_accepted_conn (sock_info);
      break;
    case SOCK_STATE_CONNECTING:		/* From ThreadSockRead.		*/
      if (pipe_msg.connect_result == SD_SUCCESS)
        handle_connect_success (sock_info);
      else
        {
        struct in_addr sin_addr;	/* inet_ntoa needs this addr format*/
        sin_addr.s_addr = sock_info->ip_addr;
        CLNP_LOG_NERR2 ("Socket 'connect' ERROR %d connecting to host %s",
            pipe_msg.connect_result, inet_ntoa (sin_addr));

        handle_connect_fail (sock_info);
        }
      break;

    case SOCK_STATE_CONNECTED:		/* From ThreadSockRead.		*/
      if (pipe_msg.tpkt_len)
        handle_data (sock_info, pipe_msg.tpkt_ptr, pipe_msg.tpkt_len);
      else	/* No data, must be disconnect	*/
        handle_disconnect (sock_info);

      /* CRITICAL: ThreadSockRead allocated tpkt. MUST FREE IT NOW.	*/
      M_FREE (MSMEM_TPKT, pipe_msg.tpkt_ptr);	/* CRITICAL: must be thread-safe*/
      break;

    case SOCK_STATE_DISCONNECTING:	/* From ThreadSockRead.		*/
      /* Ignore data if received. Free "sock_info" when no more data.	*/
      if (pipe_msg.tpkt_len == 0)
        sock_info_free (sock_info);

      /* CRITICAL: ThreadSockRead allocated tpkt. MUST FREE IT NOW.	*/
      M_FREE (MSMEM_TPKT, pipe_msg.tpkt_ptr);	/* CRITICAL: must be thread-safe*/
      break;

    default:
      TP4_EXCEPT ();	/* should never happen 	*/
      break;
    }	/* end "switch"	*/
  return (SD_TRUE);
  }	/* end "np_event"	*/
