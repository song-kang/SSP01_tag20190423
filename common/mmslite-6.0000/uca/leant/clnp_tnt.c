/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1996 - 2000, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : clnp_tnt.c						*/
/* PRODUCT(S)  : MOSI Stack						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*		This module provides the interface between the		*/
/*		Connectionless-mode Network Protocol (CLNP) and the	*/
/*		PharLap Ethernet driver.				*/
/*									*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/*			clnp_snet_init					*/
/*			clnp_snet_term					*/
/*			clnp_snet_read					*/
/*			clnp_snet_write_raw				*/
/*			clnp_snet_add_multicast_mac			*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 02/05/07  JRB    12     Repl clnp_snet_write w/ clnp_snet_write_raw	*/
/*			   (not compiled or tested).			*/
/* 07/25/00  JRB    11     Fix bug when compiling with UCA_SMP.		*/
/* 04/27/00  MDE    10     Lint cleanup					*/
/* 04/06/00  JRB    09	   Improve efficiency by eliminating memcpy.	*/
/* 04/06/00  JRB    08     Use new subnet function "clnp_snet_free"	*/
/*			   instead of "free_lpdu" flag. More flexible.	*/
/* 01/25/00  EJV    07     Record receive time for UCA SMP frames.	*/
/* 12/27/99  EJV    06     pfXmit has 4 parameters in Phar Lap ver 9.1	*/
/*			   (with NE2K patch)				*/
/*			   Introduced the _PHARLAP91_PATCHED define	*/
/* 09/13/99  MDE    05     Added SD_CONST modifiers			*/
/* 08/05/99  JRB    04     Create hMMSEvent in clnp_snet_init.		*/
/* 12/02/98  JRB    03     Added clnp_snet_add_multicast_mac function.	*/
/* 12/02/98  JRB    02     Del common functs. Instead use "clnp_eth.c".	*/
/* 10/29/98  JRB    01     Created					*/
/************************************************************************/
#include <windows.h>
#include <process.h>
#include "glbtypes.h"
#include "sysincs.h"
#include <stddef.h>		/* for offsetof	*/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;  /* Define for SLOG mechanism    */
#endif
#include "clnp_log.h"

#include "clnp_usr.h"
#include "clnp_sne.h"
#include "clnp_llc.h"
#include <embkern.h>
#include <embtcpip.h>
#include <pcatdefs.h>
#include <tcpdrivr.h>
#include "clnp_tnt.h"

#if defined(UCA_SMP)
#include "smp_usr.h"
#endif

/*----------------------------------------------------------------------*/
/* NOTE: This module interfaces to the PharLap TNT ETS Ethernet driver.	*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* defines and variables local to this module				*/
/*----------------------------------------------------------------------*/

static  ETHE_FRAME	ethe_frame_tx;		/* transmit buffer.	*/

#ifdef _WIN32
HANDLE hMMSEvent;	/* Event Object to be triggered */
ST_CHAR  *MMSEventName;	/* Pointer to the globally unique name of Event Object */
#endif

/*----------------------------------------------------------------------*/
/* Linked list stuff.							*/
/*----------------------------------------------------------------------*/

static CRITICAL_SECTION cs_frame_list;   /* Critical Section object for List */

typedef struct
  {
  ST_VOID	*next;
#if defined(UCA_SMP)
#if defined(_WIN32)
  SYSTEMTIME    rcv_time;	/* For UCA SMP frame record receive time */
#endif
#endif
  ST_UINT	len;		/* len of whole frame */
  ETHE_FRAME	ethe_frame_rx;		/* receive buffer.	*/
  } FRAME_LIST;

static FRAME_LIST *list_head = NULL;
static FRAME_LIST *list_tail = NULL;

static ST_VOID     list_put (FRAME_LIST *frame);
static FRAME_LIST *list_get (ST_VOID);

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/*	Support functions. 						*/
/*	These are useful in any Multithreaded environment when there is	*/
/*	a "receive" thread that reads packets from the Ethernet driver.	*/
/*	The "receive" thread calls "list_put" to put a packet on a	*/
/*	linked list. The "main" thread calls "list_get" to get a packet	*/
/*	from the linked list. They use a CriticalSection to ensure	*/
/*	only ONE thread can access the linked list at a time.		*/
/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/

/************************************************************************/
/*				list_put				*/
/************************************************************************/
static ST_VOID list_put (FRAME_LIST *frame)
{
  frame->next = NULL;		/* set to NULL just in case is not */

  EnterCriticalSection (&cs_frame_list);

  if (list_head == NULL)
    {
    list_head = list_tail = frame;
    }
  else
    {
    list_tail->next = frame;
    list_tail = frame;
    }

  LeaveCriticalSection (&cs_frame_list);

  if (hMMSEvent)
    SetEvent (hMMSEvent);	/* Wake up main thread.	*/
}

/************************************************************************/
/*				list_get				*/
/************************************************************************/
static FRAME_LIST *list_get (ST_VOID)
{
FRAME_LIST *frame = NULL;

  EnterCriticalSection (&cs_frame_list);

  if (list_head)
    {
    frame = list_head;
    list_head = (FRAME_LIST *) list_head->next;
    }

  LeaveCriticalSection (&cs_frame_list);

  return (frame);
}


/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/*	Functions unique to the Pharlap Ethernet Driver Interface.	*/
/*	These functions are almost always customized for a particular	*/
/*	Operating System.						*/
/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/

/* My global device handle.						*/
DEVHANDLE hDeviceSave;
/* My global copies of Driver function pointers.			*/
#if defined(_PHARLAP91_PATCHED)
int (__cdecl *pfXmitSave)(DEVHANDLE hDevice, long *pPacket, int packetLen, void *hMsg);
#else
int (__cdecl *pfXmitSave)(DEVHANDLE hDevice, long *pPacket, int packetLen);
#endif
int (__cdecl *pfIoctlSave)(DEVHANDLE hDevice, int mode);

/************************************************************************/
/*		stubEtsTCPSetDeviceEthernetInfo				*/
/* The PharLap ethernet driver must be modified to call this function	*/
/* instead of the regular function in the PharLap TCP/IP stack.		*/
/* We do what we need and then call the regular function so that	*/
/* the PharLap TCP/IP stack doesn't know we exist.			*/
/************************************************************************/
int    __cdecl stubEtsTCPSetDeviceEthernetInfo(DEVHANDLE hDriver,
					   char *pEnetStationAddr)
  {
  /* Copy Local MAC address to our global variable.			*/
  memcpy (clnp_param.loc_mac, pEnetStationAddr, 6);
  /* Call the regular function in the TCP/IP stack.		*/
  return (EtsTCPSetDeviceEthernetInfo (hDriver, pEnetStationAddr));
  }

/************************************************************************/
/*		stubEtsTCPRegisterDeviceFuncs				*/
/* The PharLap ethernet driver must be modified to call this function	*/
/* instead of the regular function in the PharLap TCP/IP stack.		*/
/* We do what we need and then call the regular function so that	*/
/* the PharLap TCP/IP stack doesn't know we exist.			*/
/************************************************************************/
int    __cdecl stubEtsTCPRegisterDeviceFuncs(DEVHANDLE hDevice,
			int (__cdecl *pfUpDown)(DEVHANDLE hDevice,
						unsigned short goingUp,
						char *options),
			int (__cdecl *pfIoctl)(DEVHANDLE hDevice, int mode),
#if defined(_PHARLAP91_PATCHED)
			int (__cdecl *pfXmit)(DEVHANDLE hDevice,
					      long *pPacket, int packetLen, void *hMsg),
#else
			int (__cdecl *pfXmit)(DEVHANDLE hDevice,
					      long *pPacket, int packetLen),
#endif
			int (__cdecl *pfSend)(void *pPacketHdr))
  {
int ret;
  hDeviceSave = hDevice;	/* Save device handle in my own global.	*/
  pfXmitSave = pfXmit;		/* Save Xmit funct ptr in my own global.*/
  pfIoctlSave = pfIoctl;	/* Save Ioctl funct ptr in my own global.*/

  /* Call the regular function in the TCP/IP stack.		*/
  ret = EtsTCPRegisterDeviceFuncs(hDevice,pfUpDown,pfIoctl,pfXmit,pfSend);
  return (ret);
  }

/************************************************************************/
/*		stubEtsTCPQueueRecvBuffer				*/
/* The PharLap ethernet driver must be modified to call this function	*/
/* instead of the regular function in the PharLap TCP/IP stack.		*/
/* We do what we need and then call the regular function so that	*/
/* the PharLap TCP/IP stack doesn't know we exist.			*/
/************************************************************************/
void __cdecl stubEtsTCPQueueRecvBuffer(DEVHANDLE hDriver, void *hMsg,
                         int pkt_len,
                         char *pRecvBuf)
  {
FRAME_LIST *frame;

  /* Don't bother passing up unless it is OSI or SMP.			*/
  if (((ST_UINT8) pRecvBuf [14] == 0xFE && (ST_UINT8) pRecvBuf [15] == 0xFE) ||
      ((ST_UINT8) pRecvBuf [14] == 0xFB && (ST_UINT8) pRecvBuf [15] == 0xFB))
    {
    frame = (FRAME_LIST *) LocalAlloc (LMEM_FIXED | LMEM_ZEROINIT,
            sizeof (FRAME_LIST));	/* FRAME_LIST big enough for any pkt*/

#if defined(UCA_SMP)
#if defined(_WIN32)
    if ((ST_UINT8) pRecvBuf [14] == 0xFB )
      /* Record the frame arrival time */
      u_smp_record_time ((ST_UCHAR *) &frame->rcv_time, sizeof(SYSTEMTIME));
#endif
#endif

    frame->len = pkt_len;
    memcpy (&frame->ethe_frame_rx, pRecvBuf, pkt_len);
    list_put (frame);	/* Add to linked list in a thread safe way.	*/
    }

  /* Call the regular function in the TCP/IP stack.		*/
  EtsTCPQueueRecvBuffer(hDriver, hMsg);
  }

/************************************************************************/
/*			clnp_snet_init					*/
/*----------------------------------------------------------------------*/
/* This function will initialize the interface between the CLNP and	*/
/* underlying sub-network.						*/
/*									*/
/* Parameters:								*/
/*	CLNP_PARAM	*clnp_param	POinter to CLNP parameters.	*/
/*									*/
/* Return:								*/
/*	SD_SUCCESS (0)		if initialization successful		*/
/*	error code		otherwise				*/
/************************************************************************/
ST_RET clnp_snet_init (CLNP_PARAM *param)
{
  if (hMMSEvent == NULL)
    hMMSEvent = CreateEvent (NULL,	// No Security
                          FALSE,	// Automatic reset.
                          FALSE,	// Initially non-signaled.
                          NULL);

  /*--------------------*/
  /* open driver	*/
  /*--------------------*/
  /* We let the Pharlap TCP/IP stack do most driver initialization.	*/
  /* When it does, the "stubEtsTCPRegisterDeviceFuncs" is called by the	*/
  /* driver. It sets our global copies of the Driver function pointers	*/
  /* "pfXmitSave" and "pfIoctlSave".					*/
  /* We must be sure the function pointers are set before calling them.	*/

  /* The only initialization to do here is to enable multicasts.	*/
  /* CRITICAL: This code only works if the Pharlap TCP/IP stack is	*/
  /*           initialized first. Otherwise, pfIoctlSave will be NULL.	*/

  if (pfIoctlSave == NULL  ||
      (*pfIoctlSave) (hDeviceSave, ENIOADDMULTI) != 0)
    {
    CLNP_LOG_ERR0 ("clnp_snet_init: error enabling multicast reception");
    return (SD_FAILURE);
    }
 
  /*----------------------------------------------------*/
  /* Initialize var to protect received frames list	*/
  /*----------------------------------------------------*/

  InitializeCriticalSection (&cs_frame_list);

  return (SD_SUCCESS);
}

/************************************************************************/
/*			clnp_snet_term					*/
/*----------------------------------------------------------------------*/
/* This function will terminate the interface between the CLNP and	*/
/* undelying sub-network.						*/
/*									*/
/* Parameters:								*/
/*	ST_VOID			none					*/
/*									*/
/* Return:								*/
/*	SD_SUCCESS (0)		if termination successful		*/
/*	error code		otherwise				*/
/************************************************************************/
ST_RET clnp_snet_term (ST_VOID)
{
ST_RET ret;

  ret = SD_SUCCESS;		/* assume termination successful	*/

  /* We let the Pharlap TCP/IP stack terminate the driver so do nothing.*/

  return (ret);
}


/************************************************************************/
/*			clnp_snet_read					*/
/*----------------------------------------------------------------------*/
/* This function will receive a LPDU from a sub-network.		*/
/*									*/
/* Parameters:								*/
/*	SN_UNITDATA *sn_req	Pointer to Sub-network Unit Data request*/
/*				to be received.				*/
/*									*/
/* Return:								*/
/*	SD_SUCCESS (0)		if read successful			*/
/*	error code		otherwise				*/
/************************************************************************/
ST_RET clnp_snet_read (SN_UNITDATA *sn_req)
{
ST_RET     ret= SNET_ERR_READ;	/* assume read failed (nothing arrived)	*/
DWORD      len = 0;
FRAME_LIST *frame;

  frame = list_get (); /* Win NT driver queues received frames */

  if (frame != NULL)
    {    
    len = (DWORD) frame->len;
    if (len > 0  &&  len <= ETHE_MAX_LEN_LSDU)
      {
      /* data received, pass the LPDU to CLNP for decoding		*/
      /* Note the len may be not equal to frame_len because Ethernet	*/
      /* requires at least 60 bytes packet.				*/

      memcpy (sn_req->loc_mac, frame->ethe_frame_rx.dst_addr, CLNP_MAX_LEN_MAC);
      memcpy (sn_req->rem_mac, frame->ethe_frame_rx.src_addr, CLNP_MAX_LEN_MAC);
      sn_req->lpdu_len = (((ST_UINT16) frame->ethe_frame_rx.frame_len [0]) << 8)
                         | (ST_UINT16) frame->ethe_frame_rx.frame_len [1];
      sn_req->lpdu = frame->ethe_frame_rx.data_buf;
#if defined(UCA_SMP)
      /* If this is a UCA SMP frame then append the recorded time at the	*/
      /* end of the local buffer (lpdu). The SMP frames are small and will fit	*/
      /* into the data_buf of the ethe_frame_rx.				*/
      if ((ST_UINT8) frame->ethe_frame_rx.data_buf[0] == 0xFB )
        memcpy( &frame->ethe_frame_rx.data_buf[sn_req->lpdu_len], &frame->rcv_time, sizeof(SYSTEMTIME));
#endif
      ret = SD_SUCCESS;
      }
    else
      ret = SNET_ERR_FRAME_LEN;
    }

  return (ret);
}


/************************************************************************/
/*			clnp_snet_write_raw				*/
/*----------------------------------------------------------------------*/
/* This function will send a LPDU to a sub-network.			*/
/*									*/
/* Return:								*/
/*	SD_SUCCESS (0)		if write successful			*/
/*	error code		otherwise				*/
/************************************************************************/
ST_RET clnp_snet_write_raw (
	ST_UCHAR *framePtr,	/* Pointer to complete frame including MACs*/
	size_t    frameLen)	/* Length of complete frame		*/
{
ST_RET  ret = SD_FAILURE;
ST_UCHAR local_frame [ETHE_MIN_LEN_LSDU];	/* local copy of short frame*/

  ret = SNET_ERR_WRITE;		/* assume write failed */

  /* Must write at least ETHE_MIN_LEN_LSDU bytes.			*/
  /* If frame is shorter, data is copied to local buffer, and framePtr	*/
  /* and frameLen are changed to reference the local buffer.		*/
  if (frameLen < ETHE_MIN_LEN_LSDU)
    {
    memcpy (local_frame, framePtr, frameLen);	/* copy to local buffer	*/
    framePtr = local_frame;			/* change ptr		*/
    frameLen = ETHE_MIN_LEN_LSDU;		/* change len		*/
    }

  if (pfXmitSave)
    {
#if defined(_PHARLAP91_PATCHED)
    (*pfXmitSave) (hDeviceSave, (long *) framePtr, frameLen, 0);
    /* hMsg is unused in NE2K driver, DEBUG: chk ret? */
#else
    (*pfXmitSave) (hDeviceSave, (long *) framePtr, frameLen);	//DEBUG: chk ret?
#endif
      ret = SD_SUCCESS;	
    }

  return (ret);
}

/************************************************************************/
/*			clnp_snet_add_multicast_mac			*/
/*----------------------------------------------------------------------*/
/* This function will add the MAC to the set of multicast addresses	*/
/* on which to accept incoming packets.					*/
/*									*/
/* Parameters:								*/
/*	ST_UCHAR  *mac_buf	Pointer to buffer for MAC address.	*/
/*									*/
/* Return:								*/
/*	SD_SUCCESS (0)		if successfully added			*/
/*	error code		otherwise				*/
/************************************************************************/
ST_RET clnp_snet_add_multicast_mac (ST_UCHAR *mac_buf)
{
  /* TNT ETS does not provide way to tell Ethernet Driver specific	*/
  /* multicast MAC addresses to listen for. This function assumes that 	*/
  /* the Ethernet Driver has been modified to accept ALL multicast	*/
  /* packets.								*/

  /* DEBUG: this function may need to be modified if the Driver		*/
  /* interface changes.							*/

  return (SD_SUCCESS);		/* Always return SUCCESS	*/
}

/************************************************************************/
/*			clnp_snet_free					*/
/* Upper (CLNP) layer calls this function to free anything allocated	*/
/* for the SN-UNITDATA PDU.						*/
/************************************************************************/
ST_VOID clnp_snet_free (SN_UNITDATA *sn_req)
  {
ST_UCHAR *frame = sn_req->lpdu - offsetof (FRAME_LIST, ethe_frame_rx)
                      - offsetof (ETHE_FRAME, data_buf);
  LocalFree ((HANDLE) frame);
  return;
  }
