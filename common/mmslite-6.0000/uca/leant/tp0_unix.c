/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	1997 - 2005 All Rights Reserved					*/
/*									*/
/* MODULE NAME : tp0_unix.c						*/
/* PRODUCT(S)  : Lean-T Stack						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*	TP0 functions ported for UNIX and UNIX-like operating systems	*/
/*	using Berkley Sockets.						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev     Comments					*/
/* --------  ---  ------   -------------------------------------------  */
/* 06/12/07  RKR    18     ported np_event to RHEL5                     */
/* 06/06/05  EJV    17     spawn_read: corr - added port_str.		*/
/* 05/09/05  EJV    16     spawn_read: Pass port to Read task.		*/
/* 01/08/04  EJV    15     Ported to sun.				*/
/* 04/14/03  JRB    14     Eliminate compiler warnings.			*/
/* 05/16/02  JRB    13     spawn_read: Pass IP addr to Read task.	*/
/*			   Log connect_result on err (now holds errno).	*/
/* 03/15/01  JRB    12     Use new SMEM allocation functions.		*/
/* 01/03/01  EJV    11     Ported to _AIX  (accept param in np_event)	*/
/* 07/25/00  JRB    10     Close socket ONLY from sock_info_free.	*/
/* 05/18/00  JRB    09     Del VXWORKS code (see new tp0_vxw.c).	*/
/*			   Free g_pipe_msg in sockets_end.		*/
/* 10/28/99  JRB    08     Fix memory leaks on disconnects for VXWORKS.	*/
/*			   See "free" calls in np_event.		*/
/* 04/12/99  JRB    07     CRITICAL: use fcntl to make read end of	*/
/*			   main pipe NONBLOCKING.			*/
/* 01/27/99  JRB    06     Change default code to use "unnamed" pipes,	*/
/*			   works without file system.			*/
/* 08/13/98  JRB    05     Lint cleanup.				*/
/* 02/20/98  JRB    04     VXWORKS bug fix: chg chk_free to free	*/
/*			   for tpkt_ptr (allocated in "read" task).	*/
/* 11/14/97  JRB    03     Added VXWORKS support.			*/
/* 09/23/97  JRB    02     sockets_end: add checks on handles, etc.	*/
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
ST_CHAR listen_task_name [] = LISTEN_TASK_NAME;
ST_CHAR read_task_name [] = READ_TASK_NAME;
pid_t listen_tid;			/* Task id for "Listen" task	*/
ST_INT pipe_to_main_fds [2];	/* read/write handles for unnamed pipe	*/
ST_INT pipe_to_listen_fds [2];	/* read/write handles for unnamed pipe	*/

ST_INT pipe_to_listen;
ST_INT pipe_to_main;
PIPE_MSG *g_pipe_msg;	/* Alloc big enough for max TPKT + PIPE_MSG	*/

/************************************************************************/
/*			sockets_init					*/
/* 1. Create Main Pipe.							*/
/* 2. Allocate "g_pipe_msg".						*/
/* 3. Set signal for child processes to SIG_IGN (avoid zombie processes)*/
/* 3. Set signal for pipes to SIG_IGN.					*/
/************************************************************************/
ST_RET sockets_init (ST_INT max_num_conns)
  {
int pipe_msg_len = tp0_cfg.max_tpdu_len + RFC1006_HEAD_LEN + PIPE_MSG_HEAD_LEN;
int pipe_flags;

  /* Init pipe handles so can tell in "sockets_end" if they were opened.*/
  pipe_to_listen = -1;
  pipe_to_main = -1;

  /* Create and open pipe to read.	*/
  if (pipe (pipe_to_main_fds) == -1)
    {
    CLNP_LOG_ERR0 ("ERROR: sockets_init: can't create 'main' pipe.\n");
    return (SD_FAILURE);
    }
  /* This task uses the "read" file descriptor "pipe_to_main_fds[0]".	*/
  /* The "write" file descriptor "pipe_to_main_fds[1]" must be passed	*/
  /* to the "read" and "listen" tasks (see spawn_read, spawn_listen).	*/

  pipe_to_main = pipe_to_main_fds [0];		/* save "read" fd	*/

  /* CRITICAL: read end of main pipe must be NONBLOCKING.		*/
  pipe_flags = fcntl (pipe_to_main, F_GETFL);
  fcntl (pipe_to_main, F_SETFL, pipe_flags | O_NONBLOCK);

  if (pipe (pipe_to_listen_fds) == -1)
    {
    CLNP_LOG_ERR0 ("ERROR: sockets_init: can't create 'listen' pipe.\n");
    return (SD_FAILURE);
    }
  /* This task uses the "write" file descriptor "pipe_to_listen_fds[1]".*/
  /* The "read" file descriptor "pipe_to_listen_fds[0]" must be passed	*/
  /* to the "listen" task (see spawn_listen).				*/

  pipe_to_listen = pipe_to_listen_fds [1];	/* save "write" fd	*/

  /* Allocate buffer for receiving "pipe" msgs.				*/
  /* Pipe msg includes pipe msg header + TPKT (i.e. TPDU + RFC1006 header).*/
  g_pipe_msg = (PIPE_MSG *) M_MALLOC (MSMEM_STARTUP, pipe_msg_len);

  /* Ignore signals when children die or they will become "zombies".	*/
  signal (SIGCHLD, SIG_IGN);

  /* Ignore signal from Pipe Manager. On QNX, we get 'Broken pipe' error*/
  /* and client application is terminated if connection to remote can	*/
  /* not be established because remote app is not running.		*/
  /* The socket connect is successful, but the first send on the socket	*/
  /* causes the signal.							*/
  /* WARNING: by ignoring this signal we may mask our pipes problems.	*/
  signal (SIGPIPE, SIG_IGN);

  return (SD_SUCCESS);
  }
/************************************************************************/
/*			sockets_end					*/
/************************************************************************/
ST_VOID sockets_end (ST_VOID)
  {
  if (pipe_to_listen >= 0)
    close (pipe_to_listen);
  if (listen_tid > 0)
    kill (listen_tid, SIGKILL);	/* Kill "Listen" Task	*/
  if (pipe_to_main >= 0)
    close (pipe_to_main);
  M_FREE (MSMEM_STARTUP, g_pipe_msg);
  }
/************************************************************************/
/*			spawn_listen					*/
/* 1. Spawn "Socket Listen" task.					*/
/* 2. Open pipe to "Socket Listen" task to signal when "accept" done.	*/
/************************************************************************/
ST_RET spawn_listen (SOCKET hSock)
  {
ST_CHAR sock_str [20];
ST_CHAR pipe_to_main_str [20];
ST_CHAR pipe_to_listen_str [20];

  if ((listen_tid = fork()) < 0)
    {
    CLNP_LOG_ERR0 ("ERROR: spawn_listen: fork failed.");
    return (SD_FAILURE);
    }
  else if (listen_tid == 0)	/* This is child	*/
    {
    sprintf (sock_str, "%d", hSock);
    sprintf (pipe_to_main_str, "%d", pipe_to_main_fds [1]);	/* write fds*/
    sprintf (pipe_to_listen_str, "%d", pipe_to_listen_fds [0]);	/* read fds*/
    if (execlp (listen_task_name, listen_task_name, sock_str,
                pipe_to_main_str, pipe_to_listen_str, NULL) < 0)
      {
      /* NOTE: we are in child task now, just do printf.	*/
      printf ("execlp of '%s' failed.", listen_task_name);
      exit (102);
      }
    }
  /* parent task just continues	*/
  return (SD_SUCCESS);
  }
/************************************************************************/
/*			spawn_read					*/
/* 1. Spawn "Socket Read" task.						*/
/************************************************************************/
ST_RET spawn_read (SOCK_INFO *sock_info)
  {
pid_t spawn_tid;
ST_CHAR sock_info_str [20];
ST_CHAR hSock_str [20];
ST_CHAR state_str [20];
ST_CHAR pipe_to_main_str [20];
ST_UINT16 max_tpkt_len = tp0_cfg.max_tpdu_len +
                       RFC1006_HEAD_LEN;	/* Max len of RFC1006 TPKT.*/
ST_CHAR max_tpkt_len_str [20];
ST_CHAR ip_addr_str [20];
ST_CHAR port_str [20];

  if ((spawn_tid = fork()) < 0)
    {
    CLNP_LOG_ERR0 ("spawn_read: fork failed.");
    return (SD_FAILURE);
    }
  else if (spawn_tid == 0)	/* This is child	*/
    {
    sprintf (sock_info_str, "%ld", (long) sock_info);	/* convert ptr to long	*/
    sprintf (hSock_str, "%d", sock_info->hSock);
    sprintf (state_str, "%d", sock_info->state);
    sprintf (max_tpkt_len_str, "%d", max_tpkt_len);
    sprintf (pipe_to_main_str, "%d", pipe_to_main_fds [1]);
    sprintf (ip_addr_str, "%ld", sock_info->ip_addr);
    sprintf (port_str, "%u", (ST_UINT) sock_info->port);
    if (execlp (read_task_name, read_task_name, sock_info_str,
                hSock_str, state_str, max_tpkt_len_str,
                pipe_to_main_str, ip_addr_str, port_str, NULL) < 0)
      {
      /* NOTE: we are in child task now, just do printf.	*/
      printf ("execlp of '%s' failed.", read_task_name);
      exit (102);
      }
    }
  /* parent task just continues	*/
  sock_info->pid_read = spawn_tid;

  return (SD_SUCCESS);
  }
/************************************************************************/
/*			np_event					*/
/************************************************************************/
ST_BOOLEAN np_event (ST_VOID)
  {
SOCK_INFO *sock_info;
SOCKET hSockConnected;	/* Accepted socket.	*/
SOCKADDR RemoteAddr;	/* remote TCP port and IP addr for accepted sock*/
#if defined(_AIX) || defined(sun) || defined(linux)
socklen_t RemoteLen;
#else
int	RemoteLen;	/* sizeof RemoteAddr	*/
#endif
ssize_t read_ret;
ST_UINT count = 0;	/* Count of bytes read from pipe.		*/
ST_CHAR dummy_msg [1];

  /* Read PIPE_MSG header. If we get any data, we better get complete msg.*/
  while (count < PIPE_MSG_HEAD_LEN)
    {
    read_ret = read (pipe_to_main, ((ST_CHAR *)g_pipe_msg) + count,
                     PIPE_MSG_HEAD_LEN - count);
    if (read_ret == 0)
      TP4_EXCEPT ();	/* Can't happen. Means nobody connected to pipe.*/
    else if (read_ret == -1)
      {				/* Pipe empty.				*/
      /* If this is first read, just return.  Otherwise, keep looping.	*/
      if (count == 0)		/* This is first read.			*/
        return (SD_FALSE);
      }
    else
      count += (ST_UINT) read_ret;
    }

  /* If sock_info is NULL, this msg is from "Socket Listen" Task. It	*/
  /* means a connect was received. Call "accept" now to accept it, and	*/
  /* allocate new "sock_info".						*/
  if (g_pipe_msg->sock_info)
    sock_info = g_pipe_msg->sock_info;
  else
    {	/* sock_info = NULL, means msg from "Socket Listen" Task	*/
    RemoteLen = sizeof(RemoteAddr);
    hSockConnected = accept(hSockListen, &RemoteAddr,
                   &RemoteLen);
    /* Signal "Socket Listen" Task to continue by writing 1 byte to pipe.*/
    write (pipe_to_listen, dummy_msg, 1);

    if (hSockConnected != INVALID_SOCKET)
      {
      sock_info = sock_info_alloc (hSockConnected, SOCK_STATE_ACCEPTED,
                  INVALID_CONN_ID);
      if (spawn_read (sock_info) != SD_SUCCESS)
        {
        CLNP_LOG_ERR0 ("ERROR spawning Read Task");
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

  /* Read PIPE_MSG data (i.e. TPKT).	*/
  if (g_pipe_msg->tpkt_len)
    {
    while (count < g_pipe_msg->tpkt_len + PIPE_MSG_HEAD_LEN)
      {
      read_ret = read (pipe_to_main, ((ST_CHAR *)g_pipe_msg) + count,
                       g_pipe_msg->tpkt_len + PIPE_MSG_HEAD_LEN - count);
      if (read_ret == 0)
        TP4_EXCEPT ();	/* Can't happen. Means nobody connected to pipe.*/
      else if (read_ret != -1)	/* Pipe NOT empty.			*/
        count += (ST_UINT) read_ret;
      }
    }

  switch (sock_info->state)
    {
    case SOCK_STATE_ACCEPTED:		/* From "Socket Listen" Task.	*/
      handle_accepted_conn (sock_info);
      break;
    case SOCK_STATE_CONNECTING:		/* From "Socket Read" Task.	*/
      if (g_pipe_msg->connect_result == SD_SUCCESS)
        handle_connect_success (sock_info);
      else
        {
        struct in_addr sin_addr;	/* inet_ntoa needs this addr format*/
        sin_addr.s_addr = sock_info->ip_addr;
        CLNP_LOG_REQ2 ("Socket 'connect' ERROR %d connecting to host %s",
            g_pipe_msg->connect_result, inet_ntoa (sin_addr));

        handle_connect_fail (sock_info);
        }
      break;

    case SOCK_STATE_CONNECTED:		/* From "Socket Read" Task.	*/
      if (g_pipe_msg->tpkt_len)
        {
        handle_data (sock_info, g_pipe_msg->tpkt_buf, g_pipe_msg->tpkt_len);
        }
      else			/* No data, must be disconnect	*/
        handle_disconnect (sock_info);
      break;

    case SOCK_STATE_DISCONNECTING:	/* From "Socket Read" Task.	*/
      /* Ignore data if received. Free "sock_info" when no more data.	*/
      if (g_pipe_msg->tpkt_len == 0)
        sock_info_free (sock_info);
      break;

    default:
      TP4_EXCEPT ();	/* should never happen 	*/
      break;
    }	/* end "switch"	*/
  return (SD_TRUE);
  }	/* end "np_event"	*/
