/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	1997-2005 All Rights Reserved					*/
/*									*/
/* MODULE NAME : tp0_vxw.c						*/
/* PRODUCT(S)  : Lean-T Stack						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*	TP0 functions ported to VXWORKS using Berkley Sockets API.	*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev     Comments					*/
/* --------  ---  ------   -------------------------------------------  */
/* 05/09/05  EJV    04     spawn_read: Pass port to Read task.		*/
/* 05/16/02  JRB    03     spawn_read: Pass IP addr to Read task.	*/
/*			   Log connect_result on err (now holds errno).	*/
/* 07/25/00  JRB    02     Close socket ONLY from sock_info_free.	*/
/* 05/18/00  JRB    01     Created from code removed from "tp0_unix.c".	*/
/*			   Don't alloc pipe_msg; point to global struct.*/
/*			   Chg "sp" calls to "taskSpawn".		*/
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
pid_t listen_tid;			/* Task id for "Listen" task	*/

/* Use "named" pipes	*/
ST_CHAR pipe_to_listen_name [] = PIPE_TO_LISTEN_NAME;
ST_CHAR pipe_to_main_name [] = PIPE_TO_MAIN_NAME;

ST_INT pipe_to_listen;
ST_INT pipe_to_main;

/* On VXWORKS, the pipe msg does NOT contain data. The data buffer is	*/
/* allocated by the "read" task, a pointer to the buffer is passed in	*/
/* the pipe msg, and the buffer is freed by this task.			*/
PIPE_MSG only_pipe_msg;		/* One global struct for received pipe msgs.*/
PIPE_MSG *g_pipe_msg = &only_pipe_msg;

int g_priority_main;		/* Task priority for "Main" task.	*/

/************************************************************************/
/*			sockets_init					*/
/* - Create Main Pipe.							*/
/* - Set signal for child processes to SIG_IGN (avoid zombie processes)	*/
/* - Set signal for pipes to SIG_IGN.					*/
/************************************************************************/
ST_RET sockets_init (ST_INT max_num_conns)
  {
int nMessages = max_num_conns * 2;
int pipe_msg_len = sizeof (PIPE_MSG);

  /* Init pipe handles so can tell in "sockets_end" if they were opened.*/
  pipe_to_listen = -1;
  pipe_to_main = -1;

  /* Create and open pipe to read.	*/
  if ( pipeDevCreate (pipe_to_main_name, nMessages, pipe_msg_len) != OK)
    {
    CLNP_LOG_ERR0 ("ERROR creating pipe to 'Main' task");
    return (SD_FAILURE);
    }
  /* Open pipe for BLOCKING read.  Can't do NONBLOCKING on VXWORKS	*/
  if ((pipe_to_main = open (pipe_to_main_name, O_RDONLY, 0)) < 0)
    {
    CLNP_LOG_ERR0 ("ERROR opening pipe to 'Main' task");
    return (SD_FAILURE);
    }

  /* Ignore signals when children die or they will become "zombies".	*/
  signal (SIGCHLD, SIG_IGN);

  /* Ignore signal from Pipe Manager. On QNX, we get 'Broken pipe' error*/
  /* and client application is terminated if connection to remote can	*/
  /* not be established because remote app is not running.		*/
  /* The socket connect is successful, but the first send on the socket	*/
  /* causes the signal.							*/
  /* WARNING: by ignoring this signal we may mask our pipes problems.	*/
  signal (SIGPIPE, SIG_IGN);

  /* Get the task priority of this (Main) task. Use it later to set the	*/
  /* priority of the spawned tasks (Listen & Read).			*/
  if (taskPriorityGet (taskIdSelf (), &g_priority_main) != OK)
    g_priority_main = 100;	/* assume default priority	*/

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

  /* Destroy the "named" pipes.		*/
  if (unlink (pipe_to_listen_name))
    {
    CLNP_LOG_ERR1 ("ERROR deleting pipe '%s'", pipe_to_listen_name);
    }
  if (unlink (pipe_to_main_name))
    {
    CLNP_LOG_ERR1 ("ERROR deleting pipe '%s'", pipe_to_main_name);
    }
  }
/************************************************************************/
/*			spawn_listen					*/
/* 1. Spawn "Socket Listen" task.					*/
/* 2. Open pipe to "Socket Listen" task to signal when "accept" done.	*/
/************************************************************************/
ST_RET spawn_listen (SOCKET hSock)
  {
  listen_tid = taskSpawn (
        "t_slistend",		/* task name for debugging	*/
        g_priority_main,	/* priority - same as this task.	*/
        0,			/* no task options supported	*/
        2000,			/* stack size			*/
        (FUNCPTR) slistend,	/* task function entry point	*/
        (int) hSock,		/* arg1 = socket		*/
        0,0,0,  0,0,0, 0,0,0);	/* 9 more dummy args		*/
  if (listen_tid < 0)
    {
    CLNP_LOG_ERR1 ("Error %d spawning Listen Task", listen_tid);
    return (SD_FAILURE);
    }

  /* Give child (slistend) time to create pipe.	*/
  taskDelay (sysClkRateGet ());	/* sleep for 1 second	*/

  if ((pipe_to_listen = open (pipe_to_listen_name, O_WRONLY, 0)) < 0)
    {
    CLNP_LOG_ERR0 ("ERROR opening pipe to listen task");
    return (SD_FAILURE);
    }
  return (SD_SUCCESS);
  }
/************************************************************************/
/*			spawn_read					*/
/* 1. Spawn "Socket Read" task.						*/
/************************************************************************/
ST_RET spawn_read (SOCK_INFO *sock_info)
  {
pid_t spawn_tid;
ST_UINT16 max_tpkt_len = tp0_cfg.max_tpdu_len +
                       RFC1006_HEAD_LEN;	/* Max len of RFC1006 TPKT.*/

  spawn_tid = taskSpawn (
        "t_sreadd",		/* task name for debugging	*/
        g_priority_main,	/* priority - same as this task.	*/
        0,			/* no task options supported	*/
        2000,			/* stack size			*/
        (FUNCPTR) sreadd,	/* task function entry point	*/
        (int) sock_info,	/* arg1	*/
        (int) sock_info->hSock,	/* arg2	*/
        (int) sock_info->state,	/* arg3	*/
        (int) max_tpkt_len,	/* arg4	*/
        (int) sock_info->ip_addr,	/* arg5	*/
        (int) (unsigned) sock_info->port, /* arg6 */
        0, 0, 0, 0);		/* 4 more dummy args		*/
  if (spawn_tid < 0)
    {
    CLNP_LOG_ERR1 ("Error %d spawning 'Read' task", spawn_tid);
    return (SD_FAILURE);
    }
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
int	RemoteLen;	/* sizeof RemoteAddr	*/
ssize_t read_ret;
ST_CHAR dummy_msg [1];

fd_set readfds;
int nfds;
struct timeval TimeOut;

  /* "read" is always BLOCKING on VXWORKS, so must do a select.		*/
  FD_ZERO (&readfds);
  FD_SET (pipe_to_main, &readfds);
  /* pipe_to_main is only fds to check, so set nfds one greater.	*/
  nfds = pipe_to_main+1;

  TimeOut.tv_sec = 0;		/* Don't block.	*/
  TimeOut.tv_usec = 0;
  nfds = select (nfds, &readfds, NULL, NULL, &TimeOut);
  if (nfds < 0)
    {			/* Error	*/
    CLNP_LOG_ERR1 ("ERROR 'select' on pipe_to_main failed (ret = %d).\n", nfds);
    return (SD_FALSE);
    }
  else if (nfds == 0)
    {			/* Timeout	*/
    return (SD_FALSE);
    }
  else
    {			/* Got pipe_msg	*/
    read_ret = read (pipe_to_main, (ST_CHAR *)g_pipe_msg, sizeof (PIPE_MSG));
    if (read_ret != sizeof (PIPE_MSG))
      {		/* Pipes are "message oriented", so this can't happen.	*/
      TP4_EXCEPT ();
      return (SD_FALSE);
      }
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
        sock_info_free (sock_info);
        return (SD_FALSE);
        }
      }
    else
      {
      CLNP_LOG_ERR0 ("Socket 'accept' error");
      return (SD_FALSE);
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
        handle_data (sock_info, g_pipe_msg->tpkt_ptr, g_pipe_msg->tpkt_len);
        }
      else			/* No data, must be disconnect	*/
        handle_disconnect (sock_info);

      /* CRITICAL: "read" thread allocated tpkt. MUST FREE IT NOW.	*/
      free (g_pipe_msg->tpkt_ptr);	/* CRITICAL: must be thread-safe*/
      break;

    case SOCK_STATE_DISCONNECTING:	/* From "Socket Read" Task.	*/
      /* Ignore data if received. Free "sock_info" when no more data.	*/
      if (g_pipe_msg->tpkt_len == 0)
        sock_info_free (sock_info);

      /* CRITICAL: "read" thread allocated tpkt. MUST FREE IT NOW.	*/
      free (g_pipe_msg->tpkt_ptr);	/* CRITICAL: must be thread-safe*/
      break;

    default:
      TP4_EXCEPT ();	/* should never happen 	*/
      break;
    }	/* end "switch"	*/
  return (SD_TRUE);
  }	/* end "np_event"	*/
