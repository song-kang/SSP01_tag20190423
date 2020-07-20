/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	1997 All Rights Reserved					*/
/*									*/
/* MODULE NAME : tp0_list.c						*/
/* PRODUCT(S)  : Lean-T Stack						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*	TP0 functions for socket "Listen" Task/Thread. This Task/Thread	*/
/*	is spawned by the "main" Task/Thread ONLY ONCE AT STARTUP.	*/
/*	This module contains different versions of code for		*/
/*	different platforms or operating systems.			*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev     Comments					*/
/* --------  ---  ------   -------------------------------------------  */
/* 10/27/06  JRB    15     Add EBADF check for default system.		*/
/* 05/22/05  JRB    14     slistend (VXWORKS): chg ret from void to int.*/
/* 04/14/03  JRB    13     Chg main return from void to "int".		*/
/* 03/07/02  JRB    12     Use memset to start with clean pipe_msg.	*/
/* 08/15/00  EJV    11     WIN32: Del pipe_msg_crit_sec, pipe_msg_count.*/
/*			     Caused deadlock when pipe filled up on NT.	*/
/* 05/18/00  JRB    10     For "default" OS, ignore SIGINT.		*/
/* 04/11/00  JRB    09     For "default" OS, set pipe_msg.tpkt_len=0.	*/
/* 08/05/99  JRB    08     Use hTcpEvent, gs_*_event_sem for WIN32, OS2.*/
/* 01/27/99  JRB    07     Change default code to use "unnamed" pipes,	*/
/*			   works without file system.			*/
/* 12/08/98  JRB    06     Cast to (char *) for UNIX/VXWORKS write call.*/
/* 04/27/98  JRB    05     Call DosResetEventSem for OS2.		*/
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

#if defined (_WIN32)
/************************************************************************/
/*			ThreadSockListen				*/
/* This thread is spawned ONLY ONCE, at startup, from the main thread.	*/
/* This thread does the following IN AN INFINITE LOOP:			*/
/* 1. Waits for a "blocking" select on listening socket to complete.	*/
/* 2. Sends msg to main thread with "pipe_msg.sock_info = NULL" (main	*/
/*    thread knows that if sock_info==NULL, msg is from this thread).	*/
/************************************************************************/
void ThreadSockListen (void *param)
  {
SOCKET hSock;			/* Listening socket	*/
PIPE_MSG pipe_msg;
fd_set readfds;
int nfds;
DWORD	dwTemp;

  hSock = (SOCKET) param;	/* Listening socket	*/

  /* Main thread is only looking for pipe_msg.sock_info==NULL, but lets	*/
  /* start with a clean struct anyway, to be safe.			*/
  memset (&pipe_msg, 0, sizeof (PIPE_MSG));	/* Start with clean struct*/
  pipe_msg.sock_info = NULL;	/* Set this once. Never changes.	*/
  while (1)
    {
    FD_ZERO (&readfds);
    FD_SET (hSock, &readfds);
    /* Wait forever for incoming connection.	*/
    nfds = hSock+1;    /* hSock is only fds to check, so set "nfds=hSock+1".*/
    nfds = select (nfds, &readfds, NULL, NULL, NULL);
    if (nfds <= 0)
      {
#ifdef DEBUG_CHILD	/* not normally defined	*/
      printf ("ThreadSockListen: 'select' failed (ret = %d).\n", nfds);
#endif	/* DEBUG_CHILD	*/
      }
    else	/* got incoming connnection(s) to process*/
      {		/* Send msg to main thread, so it does "accept".	*/
      /* NOTE: on NT, if the pipe is full, WriteFile may block until	*/
      /*       it is "TOTALLY" empty.					*/
      WriteFile(hPipeMainWrite, &pipe_msg, sizeof(pipe_msg), &dwTemp, NULL);

      gs_signal_event_sem (hTcpEvent);	/* Wake up main thread.		*/

      /* Wait for main thread to do "accept"	*/
      if (WaitForSingleObject (hEventListenGo, INFINITE) != WAIT_OBJECT_0)
        {
#ifdef DEBUG_CHILD	/* not normally defined	*/
        printf ("ThreadSockListen: WaitForSingleObject failed.\n");
#endif	/* DEBUG_CHILD	*/
        }
      }
    }	/* end "while"	*/
  }	/* end "ThreadSockListen"	*/

#elif defined (__OS2__)

/************************************************************************/
/*			ThreadSockListen				*/
/* See description for _WIN32 version of this function above.		*/
/************************************************************************/
void ThreadSockListen (void *param)
  {
SOCKET hSock;			/* Listening socket	*/
PIPE_MSG pipe_msg;
fd_set readfds;
int nfds;
ST_UINT32 dwTemp;

  hSock = (SOCKET) param;	/* Listening socket	*/

  /* Main thread is only looking for pipe_msg.sock_info==NULL, but lets	*/
  /* start with a clean struct anyway, to be safe.			*/
  memset (&pipe_msg, 0, sizeof (PIPE_MSG));	/* Start with clean struct*/
  pipe_msg.sock_info = NULL;	/* Set this once. Never changes.	*/
  while (1)
    {
    FD_ZERO (&readfds);
    FD_SET (hSock, &readfds);
    /* Wait forever for incoming connection.	*/
    nfds = hSock+1;	/* hSock is only fds to check, so set "nfds=hSock+1".*/
    nfds = select (nfds, &readfds, NULL, NULL, NULL);
    if (nfds <= 0)
      {
#ifdef DEBUG_CHILD
      printf ("ThreadSockListen: 'select' failed (ret = %d).\n", nfds);
#endif
      }
    else	/* got incoming connnection(s) to process*/
      {		/* Send msg to main thread, so it does "accept".	*/
      DosWrite(hPipeMainWrite, &pipe_msg, sizeof(pipe_msg), &dwTemp);

      DosEnterCritSec();
      pipe_msg_count++;
      DosExitCritSec();

      gs_signal_event_sem (hTcpEvent);	/* Wake up main thread.		*/

      /* Wait for main thread to do "accept"	*/
      if (DosWaitEventSem (hEventListenGo, SEM_INDEFINITE_WAIT) != 0)
        {
#ifdef DEBUG_CHILD
        printf ("ThreadSockListen: WaitForSingleObject failed.\n");
#endif
        }
      gs_reset_event_sem (hEventListenGo);	/* OS2 requires reset.	*/
      }
    }	/* end "while"			*/
  }	/* end "ThreadSockListen"	*/

#else	/* All other systems	*/

/************************************************************************/
/*				 main					*/
/* This task is spawned ONLY ONCE, at startup, from the main task.	*/
/* This task does the following IN AN INFINITE LOOP:			*/
/* 1. Waits for a "blocking" select on listening socket to complete.	*/
/* 2. Sends msg to main task with "pipe_msg.sock_info = NULL" (main	*/
/*    task knows that if sock_info==NULL, msg is from this task).	*/
/* NOTE: This code should work for most UNIX, or UNIX-like systems.	*/
/************************************************************************/
#if defined (VXWORKS)
int slistend (int arg1)
  {
char *argv[2];
#else	/* !VXWORKS	*/  
int main (argc, argv)
int argc;
char *argv[];
  {
#endif	/* !VXWORKS	*/  
SOCKET hSock;			/* Listening socket	*/
PIPE_MSG pipe_msg;
fd_set readfds;
int nfds;
int pipe_to_listen;
int pipe_to_main;
ST_CHAR dummy_msg [1];

#ifdef VXWORKS
  argv [0] = "slistend";
  hSock = arg1;
#else	/* !VXWORKS	*/
  if (argc == 4)
    {
    /* Parse the command line	*/
    hSock = atoi (argv[1]);
    pipe_to_main = atoi (argv[2]);
    pipe_to_listen = atoi (argv[3]);
    }
  else
    {
    printf ("%s: FATAL ERROR: number of arguments (%d) incorrect.\n",
            argv[0], argc-1);
    exit (99);
    }
#endif	/* !VXWORKS	*/

  signal (SIGINT, SIG_IGN);	/* only main task should catch ctrl-c	*/

  /* Create and open pipe to read.	*/
#if defined (VXWORKS)
  if ( pipeDevCreate (PIPE_TO_LISTEN_NAME, 10, 1) != OK)	/* DEBUG:10 msgs,1 bytes */
    {
    printf ("%s: FATAL ERROR: can't create pipe.\n", argv[0]);
    exit (98);
    }
  if ((pipe_to_listen = open (PIPE_TO_LISTEN_NAME, O_RDONLY, 0)) < 0)
    {
    printf ("%s: FATAL ERROR: can't open pipe to read.\n", argv[0]);
    exit (97);
    }

  /* Open pipe to write.		*/
  if ((pipe_to_main = open (PIPE_TO_MAIN_NAME, O_WRONLY, 0)) < 0)
    {
    printf ("%s: FATAL ERROR: can't open pipe to write.\n", argv[0]);
    exit (96);
    }
#endif	/* VXWORKS	*/

  /* Main task is only looking for pipe_msg.sock_info==NULL, but lets	*/
  /* start with a clean struct anyway, to be safe.			*/
  memset (&pipe_msg, 0, sizeof (PIPE_MSG));	/* Start with clean struct*/
  pipe_msg.sock_info = NULL;	/* Set this once. Never changes.	*/
  pipe_msg.tpkt_len = 0;	/* No "data" with this msg.		*/
  while (1)
    {
    FD_ZERO (&readfds);
    FD_SET (hSock, &readfds);
    /* Wait forever for incoming connection.	*/
    nfds = hSock+1;    /* hSock is only fds to check, so set "nfds=hSock+1".*/
    nfds = select (nfds, &readfds, NULL, NULL, NULL);
    if (nfds <= 0)
      {
#ifdef DEBUG_CHILD	/* not normally defined	*/
      printf ("%s: 'select' failed (ret = %d).\n", argv[0], nfds);
#endif	/* DEBUG_CHILD	*/
      if (nfds < 0 && errno == EBADF)	/* bad socket handle	*/
        break;				/* stop looping and exit*/
      }
    else	/* got incoming connnection(s) to process*/
      {		/* Send msg to main thread, so it does "accept".	*/
#if defined (VXWORKS)
      write (pipe_to_main, (char *) &pipe_msg, sizeof (PIPE_MSG));
#else	/* !VXWORKS	*/  
      write (pipe_to_main, (char *) &pipe_msg, PIPE_MSG_HEAD_LEN);
#endif	/* !VXWORKS	*/  
      /* Wait for main task to write back.				*/
      /* CRITICAL: We assume 'read' blocks until 1 byte is received.	*/
      read (pipe_to_listen, dummy_msg, 1);
      }
    }	/* end "while"	*/
  return (0);
  }	/* end "main"	*/

#endif	/* All other systems	*/
