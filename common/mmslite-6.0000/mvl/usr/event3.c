/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	2010-2010 All Rights Reserved					*/
/*									*/
/* MODULE NAME : event3.c						*/
/* PRODUCT(S)  : MMSEASE-LITE						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*	Event handling functions.					*/
/*	Use ONLY with "osi_snap" stack library.				*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*	wait_any_event ()						*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev     Comments					*/
/* --------  ---  ------   -------------------------------------------  */
/* 02/02/10  JRB    01     Created					*/
/************************************************************************/
#include "glbtypes.h"
#include "sysincs.h"
#include "mmsdefs.h"
#include "glbsem.h"
#include "tp0_ipc.h"		/* for IPC_SOCK_HANDLE	*/
#include "tp0_sock_snap.h"	/* for mms_event_fd	*/

/************************************************************************/
/*                          wait_any_event				*/
/************************************************************************/
ST_VOID wait_any_event (ST_LONG max_ms_delay)
  {
  struct timeval stTimeVal;
  fd_set readfds;
  fd_set writefds;
  fd_set exceptfds;
  int    nfds=0;
  ST_LONG usec;		/* microseconds to wait	*/

  FD_ZERO (&readfds);
  FD_ZERO (&writefds);
  FD_ZERO (&exceptfds);
  FD_SET (0, &readfds);		/* select on "stdin"	*/

  if (mms_event_fd  != INVALID_SOCKET)
    {
    FD_SET (mms_event_fd, &readfds );    /* select on MMS Event socket    */
    nfds = max (nfds, mms_event_fd);
    }

  if (IPC_SOCK_HANDLE != INVALID_SOCKET)
    {
    /* select on domain socket connected to the SNAP-Lite process.*/
    FD_SET (IPC_SOCK_HANDLE, &readfds);
    if (IPC_SOCK_CHECK_WRITE)
      FD_SET (IPC_SOCK_HANDLE, &writefds);
    nfds = max (nfds, IPC_SOCK_HANDLE);
    }
  else
    printf ("Lost connection with SNAP-Lite (can't receive connections).\n");

  nfds++;	/* must be 1 greater than max handle.	*/

  /* If you want to wait for other events too, add more FD_SET calls,	*/
  /* and recompute "nfds" using "max" as above, then adding 1		*/
  /* (or just set nfds=FD_SETSIZE).					*/

  usec = max_ms_delay * 1000; /*** Convert to micro seconds ***/
  stTimeVal.tv_sec  = usec / 1000000;
  stTimeVal.tv_usec = usec % 1000000;

  nfds = select (nfds, &readfds, &writefds, &exceptfds, &stTimeVal);
  }

