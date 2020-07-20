/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	1997-2005, All Rights Reserved					*/
/*									*/
/* MODULE NAME : tp0_read.c						*/
/* PRODUCT(S)  : Lean-T Stack						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*	TP0 functions for socket "Read" Task/Thread.  This Task/Thread	*/
/*	is spawned by the "main" Task/Thread for each connected		*/
/*	socket. This module contains different versions of code for	*/
/*	different platforms or operating systems.  It also contains	*/
/*	common code in static functions which are used by all platforms.*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev     Comments					*/
/* --------  ---  ------   -------------------------------------------  */
/* 12/15/05  JRB    26     Chg exit to break to eliminate QNX warning.	*/
/* 11/28/05  JRB    25     Fix ST_UINT16 spelling in sreadd for VXWORKS.*/
/* 08/09/05  EJV    24     thisFileName needed only on _WIN32.		*/
/* 08/01/05  JRB    23     Avoid risky cast to remove const.		*/
/* 05/09/05  EJV    22     Passed port to read task.			*/
/*			   socket_connect: use port if configured.	*/
/* 03/08/05  JRB    21     If first byte of TPKT != RFC1006_VERSION,	*/
/*			   return error (causes disconnect).		*/
/* 02/22/05  JRB    20     sreadd (VXWORKS): chg ret from void to int.	*/
/* 03/11/04  GLB    19     Cleaned up "thisFileName"                    */
/* 04/14/03  JRB    18     Chg main return from void to "int".		*/
/* 07/16/02  JRB    17     VXWORKS & default sys: Close pipe before exit*/
/* 05/16/02  JRB    16     Call blocking connect from Read task/thread.	*/
/*			   Store "errno" in connect_result so main	*/
/*			   task/thread can examine or log it.		*/
/*			   WIN32: Log if WriteFile fails.		*/
/*			   UNIX: chg M_MALLOC back to malloc to avoid	*/
/*			   needing mem lib in Read task (one per conn).	*/
/* 03/07/02  JRB    15     Use memset to start with clean pipe_msg.	*/
/* 03/14/01  JRB    14     Use new SMEM allocation functions.		*/
/* 11/13/00  JRB    13     Chg arg to read thread to (SOCK_INFO *).	*/
/* 08/15/00  EJV    12     WIN32: Del pipe_msg_crit_sec, pipe_msg_count.*/
/*			     Caused deadlock when pipe filled up on NT.	*/
/* 05/18/00  JRB    11     Lint cleanup for VXWORKS.			*/
/*			   VXWORKS & UNIX exit instead of being killed.	*/
/*			   VXWORKS & UNIX ignore SIGINT.		*/
/*			   Use new ioctlsocket macro.			*/
/* 09/30/99  JRB    10     Don't use "chk_*" functions for thread_info	*/
/*			   (was causing linked list tearing).		*/
/* 08/05/99  JRB    09     Use hTcpEvent, gs_*_event_sem for WIN32, OS2.*/
/* 01/27/99  JRB    08     Change default code to use "unnamed" pipes,	*/
/*			   works without file system.			*/
/* 08/13/98  JRB    07     Lint cleanup.				*/
/* 02/20/98  JRB    06     VXWORKS bug fix: chg chk_malloc to malloc	*/
/*			   for tpkt_ptr (freed in "main" task).		*/
/* 12/10/97  JRB    05     Don't chk for (tpkt_len < 0). It's unsigned.	*/
/* 12/08/97  JRB    04     Added __OS2__ support.			*/
/* 12/08/97  JRB    03     Skip FD_SETSIZE check. Can't happen.		*/
/* 11/14/97  JRB    02     Added VXWORKS support.			*/
/* 08/01/97  JRB    01     Created (compatible with MMS-EASE 7.0).	*/
/************************************************************************/
#include "glbtypes.h"
#include "sysincs.h"
#include "mem_chk.h"
#include "tp4api.h"		/* User definitions for tp4	*/
#include "tp4.h"		/* Internal definitions for tp4	*/
#include "clnp_log.h"

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/

#if defined(DEBUG_SISCO) && defined(_WIN32)
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/
/* Local function Prototypes						*/
/* These functions must only be used by the "Read" Task/Thread.		*/
/************************************************************************/
static ST_INT socket_connect (SOCKET hSock, ST_UINT32 ip_addr, ST_UINT16 port);
static ST_UINT16 read_rfc1006_tpkt (SOCKET hSock, ST_UCHAR *tpkt_ptr,
                                ST_UINT16 max_tpkt_len);

#if defined (_WIN32)

/************************************************************************/
/*			ThreadSockRead					*/
/* This thread is spawned from the main thread once for each active	*/
/* socket (connecting or connected).					*/
/* If "connecting", it waits for connect to complete. Once connected,	*/
/* it forever reads from the socket.					*/
/* If "connected", this thread allocates a buffer, as needed, to	*/
/* store a RFC1006 TPKT received.  When a complete TPKT is received, it	*/
/* sends a pipe_msg to the main thread.  The main thread must free	*/
/* the TPKT (passed in pipe_msg.tpkt_ptr).				*/
/************************************************************************/
void ThreadSockRead (void *param)
  {
SOCK_INFO *sock_info;		/* CRITICAL: do not write to this struct*/
				/* Only Main Thread can write to it.	*/
SOCKET hSock;			/* socket to read from	*/
ST_INT state;			/* Socket state.	*/
PIPE_MSG pipe_msg;
DWORD	dwTemp;
ST_UINT16 max_tpkt_len;
ST_INT err;

  /* Process arguments to thread.	*/
  sock_info = (SOCK_INFO *) param;
  hSock = sock_info->hSock;
  state = sock_info->state;
  max_tpkt_len = tp0_cfg.max_tpdu_len + RFC1006_HEAD_LEN;

  memset (&pipe_msg, 0, sizeof (PIPE_MSG));	/* Start with clean struct*/

  /* this part of pipe_msg never changes.	*/
  pipe_msg.sock_info = sock_info;

  /*--------------------------------------------------------------------*/
  /* This code only for socket that needs to connect.			*/
  /* If connect fails, exit this thread.				*/
  /*--------------------------------------------------------------------*/
  if (state == SOCK_STATE_CONNECTING)
    {
    pipe_msg.connect_result = socket_connect (hSock, sock_info->ip_addr, sock_info->port);

    /* NOTE: on NT, if the pipe is full, WriteFile may block until	*/
    /*       it is "TOTALLY" empty.					*/
    WriteFile(hPipeMainWrite, &pipe_msg, sizeof(pipe_msg), &dwTemp, NULL);

    gs_signal_event_sem (hTcpEvent);	/* Wake up main thread.		*/

    if (pipe_msg.connect_result)
      _endthread ();		/* Connect failed, so we're done.	*/
    }	/* end "if (state == SOCK_STATE_CONNECTING)"	*/

  /*--------------------------------------------------------------------*/
  /* This code for any socket.  At this point socket IS connected.	*/
  /*--------------------------------------------------------------------*/
  while (1)
    {
    /* pipe_msc.tpkt_ptr freed from main thread (see tp0_w32.c).	*/
    /* CRITICAL: allocation function must support multithreading.	*/
    pipe_msg.tpkt_ptr = (ST_UCHAR *) M_MALLOC (MSMEM_TPKT, max_tpkt_len);
    pipe_msg.tpkt_len = read_rfc1006_tpkt (hSock, pipe_msg.tpkt_ptr, max_tpkt_len);

    /* Pass TPKT to main thread for decoding and processing.		*/
    /* NOTE: main thread must free TPKT (i.e. pipe_msg.tpkt_ptr).	*/
    /* NOTE: on NT, if the pipe is full, WriteFile may block until	*/
    /*       it is "TOTALLY" empty.					*/
    if (WriteFile(hPipeMainWrite, &pipe_msg, sizeof(pipe_msg), &dwTemp, NULL)
        == 0)
      {
      err = GetLastError ();
      CLNP_LOG_ERR1 ("tp0_read: WriteFile error %d", err);
      /* Bad news. Do not know how to recover from this.	*/
      }

    gs_signal_event_sem (hTcpEvent);	/* Wake up main thread.		*/

    if (pipe_msg.tpkt_len == 0)		/* This means peer disconnected.*/
      {
      _endthread ();		/* Don't need this thread anymore.	*/
      }
    }
  }	/* End "ThreadSockRead"	*/

#elif defined (__OS2__)

/************************************************************************/
/*			ThreadSockRead					*/
/* See description for _WIN32 version of this function above.		*/
/************************************************************************/
void ThreadSockRead (void *param)
  {
SOCK_INFO *sock_info;		/* CRITICAL: do not write to this struct*/
				/* Only Main Thread can write to it.	*/
SOCKET hSock;			/* socket to read from	*/
ST_INT state;			/* Socket state.	*/
PIPE_MSG   pipe_msg;
ST_UINT32  dwTemp;
ST_UINT16  max_tpkt_len;

  /* Process arguments to thread.	*/
  sock_info = (SOCK_INFO *) param;
  hSock = sock_info->hSock;
  state = sock_info->state;
  max_tpkt_len = tp0_cfg.max_tpdu_len + RFC1006_HEAD_LEN;

  memset (&pipe_msg, 0, sizeof (PIPE_MSG));	/* Start with clean struct*/

  /* this part of pipe_msg never changes.	*/
  pipe_msg.sock_info = sock_info;

  /*--------------------------------------------------------------------*/
  /* This code only for socket that needs to connect.			*/
  /* If connect fails, exit this thread.				*/
  /*--------------------------------------------------------------------*/
  if (state == SOCK_STATE_CONNECTING)
    {
    pipe_msg.connect_result = socket_connect (hSock, sock_info->ip_addr, sock_info->port);

    DosWrite(hPipeMainWrite, &pipe_msg, sizeof(pipe_msg), &dwTemp);

    DosEnterCritSec();
    pipe_msg_count++;
    DosExitCritSec();

    gs_signal_event_sem (hTcpEvent);	/* Wake up main thread.		*/

    if (pipe_msg.connect_result)
      _endthread ();			/* Connect failed, so we're done.*/
    }	/* end "if (state == SOCK_STATE_CONNECTING)"	*/

  /*--------------------------------------------------------------------*/
  /* This code for any socket.  At this point socket IS connected.	*/
  /*--------------------------------------------------------------------*/
  while (1)
    {
    /* pipe_msc.tpkt_ptr freed from main thread (see tp0_w32.c).	*/
    /* CRITICAL: allocation function must support multithreading.	*/
    pipe_msg.tpkt_ptr = (ST_UCHAR *) M_MALLOC (MSMEM_TPKT, max_tpkt_len);
    pipe_msg.tpkt_len = read_rfc1006_tpkt (hSock, pipe_msg.tpkt_ptr, max_tpkt_len);

    /* Pass TPKT to main thread for decoding and processing.		*/
    /* NOTE: main thread must free TPKT (i.e. pipe_msg.tpkt_ptr).	*/
    DosWrite(hPipeMainWrite, &pipe_msg, sizeof(pipe_msg), &dwTemp);

    DosEnterCritSec();
    pipe_msg_count++;
    DosExitCritSec();

    gs_signal_event_sem (hTcpEvent);	/* Wake up main thread.		*/

    if (pipe_msg.tpkt_len == 0)		/* This means peer disconnected.*/
      {
      _endthread ();			/* Don't need this thread anymore.*/
      }
    }
  }	/* End "ThreadSockRead"	for __OS2__	*/

#elif defined (VXWORKS)

/************************************************************************/
/*				 sreadd					*/
/* This task is spawned from the main task once for each active		*/
/* socket (connecting or connected).					*/
/*   If "connecting", it waits for connect to complete. Once connected,	*/
/* it forever reads from the socket.					*/
/*   Because the VXWORKS pipes are "message oriented", and memory is 	*/
/* automatically shared, this works more like the _WIN32 code.		*/
/*   If "connected", this task allocates a buffer, as needed, to	*/
/* store a RFC1006 TPKT received.  When a complete TPKT is received, it	*/
/* sends a pipe_msg to the main task.  The main task must free		*/
/* the TPKT (passed in pipe_msg.tpkt_ptr).				*/
/************************************************************************/
int sreadd (int arg1, int arg2, int arg3, int arg4, int arg5, int arg6)
  {
char *argv[5];		/* Only for faking argv [0]	*/
SOCK_INFO *sock_info;
SOCKET hSock;			/* socket to read from	*/
ST_INT state;			/* Socket state.	*/
PIPE_MSG pipe_msg;
int pipe_to_main;
ST_UINT16 max_tpkt_len;
ST_UINT32 ip_addr;	/* IP addr as long int in network byte order.	*/
ST_UINT16 port;

  /* Parse the command line	*/
  argv [0] = "sreadd";
  sock_info = (SOCK_INFO *) arg1;
  hSock = arg2;
  state = arg3;
  max_tpkt_len = (ST_UINT16) arg4;
  ip_addr = (ST_UINT32) arg5;
  port = (ST_UINT16) arg6;

  signal (SIGINT, SIG_IGN);	/* only main task should catch ctrl-c	*/

  /* Open pipe to write.		*/
  if ((pipe_to_main = open (PIPE_TO_MAIN_NAME, O_WRONLY, 0)) < 0)
    {
    printf ("%s: FATAL ERROR: can't open pipe to write.\n", argv[0]);
    exit (96);
    }

  /* Fill in what we can of "pipe_msg".		*/
  memset (&pipe_msg, 0, sizeof (PIPE_MSG));	/* Start with clean struct*/
  pipe_msg.sock_info = sock_info;	/* this part of msg never changes.*/

  /*--------------------------------------------------------------------*/
  /* This code only for socket that needs to connect.			*/
  /* If connect fails, exit this task.					*/
  /*--------------------------------------------------------------------*/
  if (state == SOCK_STATE_CONNECTING)
    {
    pipe_msg.connect_result = socket_connect (hSock, ip_addr, port);

    pipe_msg.tpkt_len = 0;	/* CRITICAL: or main expects data attached*/
    write (pipe_to_main, (ST_CHAR *) &pipe_msg, sizeof (pipe_msg));
    if (pipe_msg.connect_result)
      {
      close (pipe_to_main);
      exit (95);		/* Connect failed, so we're done.	*/
      }
    }	/* end "if (state == SOCK_STATE_CONNECTING)"	*/

  /*--------------------------------------------------------------------*/
  /* This code for any socket.  At this point socket IS connected.	*/
  /*--------------------------------------------------------------------*/
  while (1)
    {
    pipe_msg.tpkt_ptr = malloc (max_tpkt_len);
    pipe_msg.tpkt_len = read_rfc1006_tpkt (hSock, pipe_msg.tpkt_ptr, max_tpkt_len);
#ifdef DEBUG_CHILD	/* not normally defined	*/
    printf("read_rfc1006_tpkt returned len = %d\n", pipe_msg.tpkt_len);
#endif	/* DEBUG_CHILD	*/

    /* Pass TPKT to main thread for decoding and processing.		*/
    write (pipe_to_main, (ST_CHAR *) &pipe_msg, sizeof (pipe_msg));
    if (pipe_msg.tpkt_len == 0)	/* This means peer disconnected.*/
      {
      close (pipe_to_main);
      break;      /* Don't need this task anymore.	*/
      }
    }
  return (0);
  }

#else		/* All other systems	*/

/************************************************************************/
/*				 main					*/
/* This task is spawned from the main task once for each active	*/
/* socket (connecting or connected).					*/
/* If "connecting", it waits for connect to complete. Once connected,	*/
/* it forever reads from the socket.					*/
/* If "connected", this task allocates a reusable buffer to		*/
/* store a RFC1006 TPKT received.  When a complete TPKT is received, it	*/
/* sends a pipe_msg to the main task including the TPKT received.	*/
/* NOTE: This code should work for most UNIX, or UNIX-like systems.	*/
/************************************************************************/
int main (argc, argv)
int argc;
char *argv[];
  {
SOCK_INFO *sock_info;
SOCKET hSock;			/* socket to read from	*/
ST_INT state;			/* Socket state.	*/
PIPE_MSG *pipe_msg;
int pipe_to_main;
ST_UINT16 max_tpkt_len;
ST_UINT32 ip_addr;	/* IP addr as long int in network byte order.	*/
ST_UINT16 port;

  /* Parse the command line	*/
  if (argc == 8)
    {
    sock_info = (SOCK_INFO *) atol (argv[1]);
    hSock = atoi (argv[2]);
    state = atoi (argv[3]);
    max_tpkt_len = (ST_UINT16) atoi (argv[4]);
    pipe_to_main = atoi (argv[5]);
    ip_addr = (ST_UINT32) atol (argv[6]);
    port = (ST_UINT16) atoi (argv[7]);
    }
  else
    {
    printf ("%s: FATAL ERROR: number of arguments (%d) incorrect.\n",
            argv[0], argc-1);
    exit (99);
    }

  signal (SIGINT, SIG_IGN);	/* only main task should catch ctrl-c	*/

  /* Allocate and fill in what we can of "pipe_msg".		*/
  /* NOTE: malloc used here to avoid needing mem lib. This task is	*/
  /*       spawned for every connection, so the savings add up.		*/
  pipe_msg = malloc (max_tpkt_len + PIPE_MSG_HEAD_LEN);
  pipe_msg->sock_info = sock_info;	/* this part of msg never changes.*/

  /*--------------------------------------------------------------------*/
  /* This code only for socket that needs to connect.			*/
  /* If connect fails, exit this task.					*/
  /*--------------------------------------------------------------------*/
  if (state == SOCK_STATE_CONNECTING)
    {
    pipe_msg->connect_result = socket_connect (hSock, ip_addr, port);

    pipe_msg->tpkt_len = 0;	/* CRITICAL: or main expects data attached*/
    write (pipe_to_main, pipe_msg, PIPE_MSG_HEAD_LEN);
    if (pipe_msg->connect_result)
      {
      close (pipe_to_main);
      exit (95);		/* Connect failed, so we're done.	*/
      }
    }	/* end "if (state == SOCK_STATE_CONNECTING)"	*/

  /*--------------------------------------------------------------------*/
  /* This code for any socket.  At this point socket IS connected.	*/
  /*--------------------------------------------------------------------*/
  while (1)
    {
    pipe_msg->tpkt_len = read_rfc1006_tpkt (hSock, pipe_msg->tpkt_buf, max_tpkt_len);
#ifdef DEBUG_CHILD	/* not normally defined	*/
    printf("read_rfc1006_tpkt returned len = %d\n", pipe_msg->tpkt_len);
#endif	/* DEBUG_CHILD	*/

    /* Pass TPKT to main thread for decoding and processing.		*/
    write (pipe_to_main, pipe_msg, PIPE_MSG_HEAD_LEN + pipe_msg->tpkt_len);
    if (pipe_msg->tpkt_len == 0)	/* This means peer disconnected.*/
      {
      close (pipe_to_main);
      break;      /* Don't need this task anymore. Break out of loop & exit.*/
      }
    }
  return (0);
  }

#endif	/* All other systems.	*/


/************************************************************************/
/*			socket_connect					*/
/* Send "blocking" socket connect request.				*/
/*   hSock:	socket handle						*/
/*   ip_addr:	IP addr as long int in network byte order.		*/
/* RETURNS: SD_SUCCESS or "errno" (WSAGetLastError() on Windows).	*/
/************************************************************************/
static ST_INT socket_connect (SOCKET hSock, ST_UINT32 ip_addr, ST_UINT16 port)
  {
SOCKADDR_IN RemoteAddr;		/* remote TCP port and IP addr		*/
ST_INT retCode;

  RemoteAddr.sin_family = AF_INET;
  if (port)
    RemoteAddr.sin_port = htons(port);
  else
    RemoteAddr.sin_port = htons(IPPORT_RFC1006);
  RemoteAddr.sin_addr.s_addr = ip_addr;

  /* NOTE: This is a "blocking" connect.	*/
  if (connect (hSock, (PSOCKADDR) &RemoteAddr, sizeof(RemoteAddr)) < 0)
    retCode = WSAGetLastError();	/* macroed to errno for non-Win.*/
  else
    retCode = SD_SUCCESS;
  return (retCode);
  }

/************************************************************************/
/*			read_rfc1006_tpkt				*/
/* Read RFC1006 PDU from socket.					*/
/* The Sockets interface does not break up PDUs for us, so we must	*/
/* read the header from a PDU first, get the length, and then read	*/
/* up to that length.							*/
/* RETURN:  tpkt_len (i.e. length of PDU received, in bytes)		*/
/*									*/
/* CRITICAL: only called from "Read" Task/Thread.			*/
/************************************************************************/
static ST_UINT16 read_rfc1006_tpkt (SOCKET hSock, ST_UCHAR *tpkt_ptr,
                                ST_UINT16 max_tpkt_len)
  {
ST_UINT16 tpkt_len;		/* length of PDU received (in bytes)	*/
ST_INT recv_ret;
ST_INT count = 0;	/* counter for # of bytes received. Init to 0.	*/

  /* Read "RFC1006_HEAD_LEN" bytes of tpkt (use blocking recv).	*/
  while (count < RFC1006_HEAD_LEN)
    {
    recv_ret = recv (hSock, (ST_CHAR *) tpkt_ptr+count, RFC1006_HEAD_LEN-count, 0);
    if (recv_ret <= 0)	/* 0 if peer disconnected, negative if error.	*/
      {
      return (0);
      }
    else
      count += recv_ret;	/* add the # of bytes read to "count"	*/
    }	/* end 1st "while" loop	*/

  if (tpkt_ptr[0] != RFC1006_VERSION)	/* first byte of tpkt must be correct version*/
    {
#ifdef DEBUG_CHILD
    printf ("read_rfc1006_tpkt: version = %u illegal\n", tpkt_ptr[0]);
#endif	/* DEBUG_CHILD	*/
    return (0);
    }

  /* CRITICAL:  "tpkt_ptr" must be (ST_UCHAR *) or compiler will	*/
  /* sign extend before casting tpkt_ptr[2] to (ST_UINT16).	*/
  tpkt_len = ( ( (ST_UINT16) tpkt_ptr[2] ) << 8 ) | tpkt_ptr[3];
  if (tpkt_len > max_tpkt_len ||  tpkt_len == 0)
    {				/* PDU size illegal. Possibly bad PDU.	*/
#ifdef DEBUG_CHILD
    printf ("read_rfc1006_tpkt: tpkt_len = %d illegal\n", tpkt_len);
#endif	/* DEBUG_CHILD	*/
    return (0);
    }

  /* Read rest of tpkt (use blocking recv).	*/
  while (count < tpkt_len)
    {			/* keep reading until "tpkt_len" bytes are read	*/
    recv_ret = recv (hSock, (ST_CHAR *) tpkt_ptr+count, tpkt_len-count, 0);
    if (recv_ret <= 0)	/* 0 if peer disconnected, negative if error.	*/
      {
      return (0);
      }
    else
      count += recv_ret;	/* add the # of bytes read to "count"	*/
    }	/* end 2nd "while" loop	*/

  return (tpkt_len);
  }		/* end function	"read_rfc1006_tpkt"	*/
