/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	2003 All Rights Reserved					*/
/*									*/
/* MODULE NAME : event2.c						*/
/* PRODUCT(S)  : MMSEASE-LITE						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*	Event handling function. This function may be used ONLY if	*/
/*	the the POLLING version of the "gensock2" sockets interface is	*/
/*	used (i.e. gensock2.c is compiled with GENSOCK_THREAD_SUPPORT	*/
/*	NOT defined).							*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*	wait_any_event ()						*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev     Comments					*/
/* --------  ---  ------   -------------------------------------------  */
/* 01/30/07  JRB    04     Shorten sleep to avoid delay detecting connect.*/
/* 12/11/06  JRB    03     Wait for Subnetwork events too.		*/
/* 11/01/06  JRB    02     Call sMsSleep when no connections,		*/
/*			   so we don't hog the CPU.			*/
/* 07/10/03  JRB    01     Created					*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "mmsdefs.h"
#include "gensock2.h"
#include "clnp_sne.h"	/* for "Subnetwork" event stuff	*/

/************************************************************************/
/*                          wait_any_event				*/
/* Wait for "socket" events OR "Subnetwork" events.			*/
/* NOTE: See comments about "PORTING" if this function needs		*/
/*	to wait on "other" handles.					*/
/************************************************************************/
ST_VOID wait_any_event (ST_LONG max_ms_delay)
  {
GENSOCK_FD_SET sockFds;
int nfds;
struct timeval selectTimeout;

  /* This function assumes only one group of sockets to process.	*/
  sockInitAllFds (&sockFds);

  /* If Subnetwork handle "hPktSock" is valid, add it to list.	*/
  if (hPktSock >= 0)
    sockAddReadFds (&sockFds, hPktSock);

  /* PORTING: Call sockAddReadFds, sockAddWriteFds, sockAddExceptFds
   *          to add "other" handles on which to wait.
   */
  if (sockFds.totalfds)
    {
    /* Convert milliseconds to seconds and microseconds	*/
    selectTimeout.tv_sec  = max_ms_delay / 1000;		/* seconds	*/
    selectTimeout.tv_usec = (max_ms_delay % 1000) * 1000;	/* microseconds	*/
    nfds = select (sockFds.selectnfds, &sockFds.readfds, &sockFds.writefds, &sockFds.exceptfds, &selectTimeout);

    /* PORTING: Add code to process "other" handles.	*/
    }
  else
    sMsSleep (10);	/* no sockets to wait on, so sleep a little to avoid hogging CPU.*/

  /* NOTE: socket events are NOT processed here. This function simply returns,
   *       and the caller must call "mvl_comm_serve" to process the events.
   */
  return;
  }

