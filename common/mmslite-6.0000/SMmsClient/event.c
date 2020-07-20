/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	1998-2006 All Rights Reserved					*/
/*									*/
/* MODULE NAME : event.c						*/
/* PRODUCT(S)  : MMSEASE-LITE						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*	Event handling functions.					*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*	wait_any_event ()						*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev     Comments					*/
/* --------  ---  ------   -------------------------------------------  */
/* 06/22/10  JRB	   Simplify code for WIN32 & default platform 	*/
/*			   (eliminate a_get_event_handle* calls).	*/
/* 11/01/06  JRB    09     Go back to gs_wait_mult_event_sem for WIN32	*/
/*			   (Ethernet interface code fixed so don't	*/
/*			   need to use WaitForMultipleObjectsEx).	*/
/* 04/20/05  JRB    08     Add header files (included indirectly before).*/
/* 07/07/04  JRB    07     Del #ifdef MOSI. May need a_get_event_handles*/
/*			   added to stack lib if non-MOSI stack used.	*/
/* 10/23/03  JRB    06     Call a_get_event_handles_unix for default system.*/
/* 05/17/02  JRB    05     Put back old code for PHARLAP_ETS, PharLap	*/
/* 			   doesn't have "Wait..Ex".			*/
/* 04/10/00  JRB    04     Del "select.h". See "sysincs.h".		*/
/* 04/06/00  JRB    03     Use WaitForMultipleObjectsEx for WIN32:	*/
/*			   CRITICAL for ethernet driver interface.	*/
/* 08/05/99  JRB    02     Use a_get_ev.., gs_wait.. for WIN32.		*/
/*			   Use hTcpEvent for OS2.			*/
/* 11/10/98  JRB    01     Created					*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "mmsdefs.h"
#include "glbsem.h"
#include "acse2usr.h"
#include "clnp_sne.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined (_WIN32)
/************************************************************************/
/*                          wait_any_event				*/
/* Windows version (also works for PHARLAP_ETS).			*/
/* Wait for events on these handles: hMMSEvent, hTcpEvent.		*/
/************************************************************************/

ST_VOID wait_any_event (ST_LONG max_ms_delay)
  {
ST_EVENT_SEM EventArray [2];
ST_INT NumEntries;
ST_BOOLEAN activity [2];	/* We don't chk it, but gs_wait.. needs it*/

  NumEntries = 0;
#if defined(GOOSE_RX_SUPP) || defined(GSSE_RX_SUPP) || defined(GSE_MGMT_RX_SUPP) || defined(SMPVAL_RX_SUPP)
  if (hMMSEvent)
    EventArray [NumEntries++] = hMMSEvent;	/* Subnetwork event	*/
#endif

  if (hTcpEvent)
    EventArray [NumEntries++] = hTcpEvent;	/* TCP/IP sockets event	*/
  
  /* Do wait only if NumEntries is reasonable.	*/
  if (NumEntries > 0 && NumEntries <= 2)
    gs_wait_mult_event_sem (NumEntries, EventArray, activity, max_ms_delay);
  else
    sMsSleep (10);	/* Can't do wait, so sleep a little to avoid hogging CPU.*/

  /* Something woke us up. Don't care if it was event or timeout.	*/
  /* Just return to main loop to attempt processing.			*/
  return;
  }


#elif defined (__OS2__)

/************************************************************************/
/*                          wait_any_event				*/
/* DEBUG: not tested but should work on OS/2.				*/
/************************************************************************/

ST_VOID wait_any_event (ST_LONG max_ms_delay)
  {
#define MMS_EVENT_SEM_ID	0
static SEMRECORD sems[2];
static HMUX hMux;
APIRET apiret;
ST_ULONG post_count;
ST_ULONG semId;

  /* This function only waits on the MMS event. To wait on other events	*/
  /* also, add to the "sems" array.					*/

  if (hMux == NULL)
    {
    sems[0].hsemCur = (void *) hTcpEvent;
    sems[0].ulUser = MMS_EVENT_SEM_ID;
    apiret = DosCreateMuxWaitSem (NULL, &hMux, 1, sems, DCMW_WAIT_ANY);
    if (apiret != NO_ERROR)
      {
      printf ("\n DosCreateMuxWaitSem failed, rc = %d",apiret);
      hMux = NULL;
      }
    return;	/* Just return. New event so no one has signaled it yet.*/
    }

  /* Wait on the MMS Event Semaphore.		*/

  apiret = DosWaitMuxWaitSem (hMux, max_ms_delay, &semId);

  if (apiret == NO_ERROR)
    {
    if (semId == MMS_EVENT_SEM_ID) /* MMS event			*/
      {
      gs_reset_event_sem (hTcpEvent);	/* OS2 requires reset	*/
      }
    }
  else if (apiret != ERROR_TIMEOUT)
    {
    printf ("\n DosWaitMuxWaitSem error, apiret = %d",apiret);
    }
  }

#elif defined(MSDOS) || defined(__MSDOS__)

/************************************************************************/
/*                          wait_any_event				*/
/************************************************************************/

ST_VOID wait_any_event (ST_LONG max_ms_delay)
  {
  return;
  }
#else	/* All other systems	*/

/************************************************************************/
/*                          wait_any_event				*/
/* Wait for events on this handle: hPktSock.				*/
/* NOTE: If this is used with TCP, we may need to add another handle	*/
/*       on which to wait. If no TCP threads, use "event2.c".		*/
/************************************************************************/
ST_VOID wait_any_event (ST_LONG max_ms_delay)
  {
  struct timeval stTimeVal;
  fd_set readfds;
  fd_set writefds;
  fd_set exceptfds;
  int    nfds=0;   
  int fd_array [2];	/* array of handles on which to wait	*/
  int num_entries;	/* num of entries in fd_array.			*/
  int j;		/* loop index	*/

  FD_ZERO (&readfds);
  FD_ZERO (&writefds);
  FD_ZERO (&exceptfds);
  FD_SET (0, &readfds);		/* select on "stdin"	*/

  /* Initialize fd_array and num_entries.	*/
  num_entries = 0;
  /* This was initialized to -1, so if it's not -1, assume it's valid.*/
  if (hPktSock >= 0)
    fd_array[num_entries++] = hPktSock;

  /* Use fd_array to initialize readfds.	*/
  for (j = 0; j < num_entries; j++)
    {
    FD_SET (fd_array[j], &readfds);
    nfds = max (nfds,fd_array[j]);
    }
  nfds++;	/* must be 1 greater than max handle.	*/

  /* If you want to wait for other events too, add more FD_SET calls,	*/
  /* and recompute "nfds" (or just set it to FD_SETSIZE).		*/

  max_ms_delay *= 1000; /*** Convert to micro seconds ***/
  stTimeVal.tv_sec  = max_ms_delay / 1000000;
  stTimeVal.tv_usec = max_ms_delay % 1000000;

  if (num_entries > 0)
    nfds = select (nfds, &readfds, &writefds, &exceptfds, &stTimeVal);
  else
    sMsSleep (10);	/* Can't do wait, so sleep a little to avoid hogging CPU.*/
  }

#endif		/* All other systems		*/

#ifdef __cplusplus
  };
#endif
