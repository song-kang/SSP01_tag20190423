/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1996 - 2006, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : clnp_linux.c						*/
/* PRODUCT(S)  : MOSI Stack						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*		This module provides the interface between the		*/
/*		Connectionless-mode Network Protocol (CLNP) and the	*/
/*		LINUX OSI Layer 2.					*/
/*									*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/*			clnp_snet_init					*/
/*			clnp_snet_term					*/
/*			clnp_snet_read					*/
/*			clnp_snet_write_raw				*/
/*			clnp_snet_set_multicast_filter			*/
/*			clnp_snet_add_multicast_mac			*/
/*			clnp_snet_rx_all_multicast_start		*/
/*			clnp_snet_rx_all_multicast_stop			*/
/*			clnp_snet_rx_multicast_stop			*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 09/13/10  JRB	   Use "param->network_device" if it is set.	*/
/* 07/15/08  JRB    16     Don't log if packet received is too long	*/
/*			   (happens too often on RedHat Linux).		*/
/* 06/13/07  RKR    15     ported to RHEL5                              */
/* 03/16/07  JRB    14     If sendto fails, sleep and try again.	*/
/* 02/16/07  JRB    13     Make sure ..write_raw safe for short frames	*/
/* 12/12/06  JRB    12     Move hPktSock to lean_var.c (avail on any sys).*/
/* 			   Ignore packets if too long or all 0 DST MAC.	*/
/*			   Chg "device" to local var.			*/
/* 12/01/06  JRB    12     ..write_raw: log frame being sent.		*/
/* 11/16/06  CRM    11     Fixed "ifr.ifr_ifindex" memory overwrite bug.*/
/*			   Chg global var "from" to local var "toAddr".	*/
/*			   Deleted unused global var "sll".		*/
/*			   Add missing multicast functions.		*/
/* 11/10/06  JRB    10     Del unused global var.			*/
/*			   Chg global etherFrameRx to local var.	*/
/* 03/20/06  JRB    09     Del clnp_snet_write, clnp_etype_write.	*/
/*			   Add clnp_snet_write_raw.			*/
/* 			   Get clnp_param.loc_mac from OS using ioctl.	*/
/* 02/08/05  JRB    08     Fix 802.1Q decoding by chging clnp_snet_read	*/
/*			    to call new clnp_snet_frame_to_udt.		*/
/*			   Use MSG_TRUNC flag to receive complete frame.*/
/*			   Chg etherFrameRx to simple ST_UINT8 buffer.	*/
/* 07/20/04  JRB    07     Del perror calls. Misc cleanup.		*/
/* 06/16/04  JRB    06     Add clnp_snet_set_multicast_filter.		*/
/* 05/18/04  JRB    05     Use sendLen in sendto call in clnp_snet_write*/
/*			   Add clnp_etype_write.			*/
/* 02/23/04  JRB    04     BUG FIX: cast &etherFrameRx to (ST_CHAR *)	*/
/*			   before adding count.				*/
/* 12/12/03  JRB    03     clnp_snet_read: chk for valid packet len.	*/
/* 10/20/03  JRB    02     Del "gensock.h" & chg SOCKET to int. Cleanup.*/
/* 03/03/03  CRM    01     Created					*/
/************************************************************************/
#include "glbtypes.h"
#include "sysincs.h"
#include "mem_chk.h"

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;  /* Define for SLOG mechanism    */
#endif

#include "clnp_log.h"
#include "clnp_usr.h"
#include "clnp_sne.h"
#include "ethertyp.h"
#include <linux/types.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <linux/if.h>
#include <linux/if_arp.h>


/*----------------------------------------------------------------------*/
/* Global variables							*/
/*----------------------------------------------------------------------*/

int save_ifindex;	/* set by init funct, used by write funct	*/

/************************************************************************/
/*			clnp_snet_init					*/
/*----------------------------------------------------------------------*/
/* This function will initialize the interface between the CLNP and	*/
/* underlying sub-network.						*/
/*									*/
/* Parameters:								*/
/*	CLNP_PARAM	*clnp_param	Pointer to CLNP parameters.	*/
/*									*/
/* Return:								*/
/*	SD_SUCCESS (0)		if initialization successful		*/
/*	error code		otherwise				*/
/************************************************************************/

ST_RET clnp_snet_init (CLNP_PARAM *param)
{                    
struct  ifreq ifr;
ST_INT  ret;
ST_LONG arg = 0;
ST_CHAR *device;	/* name of ethernet device	*/

  /* Use ethernet device name stored in "param->network_device".	*/
  if (param->network_device == NULL ||
      strlen (param->network_device) == 0)
    {
    CLNP_LOG_ERR0 ("Subnetwork initialization: 'network_device' not set. Using default 'eth0'.");
    device = "eth0";
    }
  else
    device = param->network_device;

  memset(&ifr, 0, sizeof(ifr));

  if ((hPktSock = socket (PF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 0)
    {
    CLNP_LOG_ERR0 ("clnp_snet_init: init packet socket error");
    return (SD_FAILURE);
    }

  arg |= O_NONBLOCK;
  if((ret  = fcntl (hPktSock, F_SETFL, arg)) < 0)
    {
    CLNP_LOG_ERR0 ("clnp_snet_init: error setting non-blocking flag");
    return (SD_FAILURE);
    }
  
  strcpy ( ifr.ifr_name, device);

  if (ioctl (hPktSock, SIOCGIFFLAGS, &ifr) < 0)
    {
    CLNP_LOG_ERR0 ("clnp_snet_init: error getting interface flags");
    return (SD_FAILURE);
    }

  strcpy ( ifr.ifr_name, device);
  ifr.ifr_flags |= IFF_PROMISC;

  if (ioctl (hPktSock, SIOCSIFFLAGS, &ifr) < 0)
    {
    CLNP_LOG_ERR0 ("clnp_snet_init: error setting promiscuous mode");
    return (SD_FAILURE);
    }

  ifr.ifr_ifindex = 0;
  strcpy ( ifr.ifr_name, device);

  if (ioctl (hPktSock, SIOGIFINDEX, &ifr) < 0)
    {
    CLNP_LOG_ERR0 ("clnp_snet_init: error get index");
    return (SD_FAILURE);
    }

  save_ifindex = ifr.ifr_ifindex;	/* save in global to use in clnp_snet_write_raw*/

  /* Get local MAC addr from the OS. Save it in "clnp_param.loc_mac".	*/
  strcpy (ifr.ifr_name, device);

  if (ioctl (hPktSock, SIOCGIFHWADDR, &ifr) < 0)
    {
    CLNP_LOG_ERR0 ("clnp_snet_init: error getting Hardware address");
    return (SD_FAILURE);
    }
  else
    memcpy (clnp_param.loc_mac, ifr.ifr_hwaddr.sa_data, CLNP_MAX_LEN_MAC);

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
  close(hPktSock);
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
ST_RET    ret = SNET_ERR_READ;	/* assume read failed (nothing arrived)	*/
ST_INT    recvLen;
ST_UINT8 etherFrameRx[ETHE_MAX_LEN_LSDU];	/* receive buffer	*/
ST_CHAR all_zero_mac [6] = {0, 0, 0, 0, 0, 0};

  do
    {
    /* CRITICAL: The MSG_TRUNC flag causes recvfrom to return a complete
     * frame every time. The documentation for MSG_TRUNC says nothing about
     * this, but it certainly behaves that way.
     */  
    recvLen = recvfrom (hPktSock, etherFrameRx, sizeof(etherFrameRx),
                        MSG_TRUNC, NULL, NULL);
    } while (recvLen == -1 && errno == EINTR);

  if (recvLen == -1)
    {	/* recvfrom error	*/
    /* EAGAIN is normal error when no data avaliable. Log other errors.*/
    if (errno!=EAGAIN)
      CLNP_LOG_ERR1 ("clnp_snet_read: recvfrom error (%d)", errno);
    }
  else if (memcmp (etherFrameRx, all_zero_mac, 6) == 0)
    {
    /* SUSE Linux returns many frames with a DST MAC of all 0 that	*/
    /* appear to be "loopback" frames. Since all 0 is never a normal	*/
    /* MAC, these can all be ignored. DO NOTHING HERE.			*/
    }
  else if (recvLen > ETHE_MAX_LEN_LSDU)
    {
    /* This should never happen, but somehow it does. This is bad packet.*/
    /* IGNORE THIS PACKET.						*/
#if 0	/* DEBUG: add this logging if you need to debug this problem.	*/
    CLNP_LOG_IND1 ("Received Raw Ethernet frame (len = %d). FRAME IS TOO LONG. DISCARDING.", recvLen);
    CLNP_LOGH_IND(recvLen, etherFrameRx);
#endif
    }
  else
    {
    CLSNS_LOG_IND1 ("Received Raw Ethernet frame (len = %d)", recvLen);
    CLSNS_LOGH_IND (recvLen, etherFrameRx);

    /* At this point, a complete frame has been received.	*/
    /* Loop above makes sure it can't be too long, so go ahead and fill in "sn_req".*/
    sn_req->lpdu = chk_malloc (ETHE_MAX_LEN_UDATA);
    ret = clnp_snet_frame_to_udt (etherFrameRx, recvLen, sn_req, ETHE_MAX_LEN_UDATA);
    if (ret != SD_SUCCESS)
      chk_free (sn_req->lpdu);
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
struct sockaddr_ll toAddr;
ST_RET    ret= SNET_ERR_WRITE;
socklen_t toAddrLen;    
ST_UCHAR local_frame [ETHE_MIN_LEN_LSDU];	/* local copy of short frame*/

  CLSNS_LOG_REQ1 ("Sending Raw Ethernet frame (len = %d)", frameLen);
  CLSNS_LOGH_REQ (frameLen, framePtr);

  memset (&toAddr, 0, sizeof(toAddr));
  toAddr.sll_family  = AF_PACKET;
  toAddr.sll_ifindex = save_ifindex;	/* use global set by clnp_snet_init*/
  toAddr.sll_protocol = 0;
  toAddrLen = sizeof(toAddr);
  
  /* Must write at least ETHE_MIN_LEN_LSDU bytes.			*/
  /* If frame is shorter, data is copied to local buffer, and framePtr	*/
  /* and frameLen are changed to reference the local buffer.		*/
  if (frameLen < ETHE_MIN_LEN_LSDU)
    {
    memcpy (local_frame, framePtr, frameLen);	/* copy to local buffer	*/
    framePtr = local_frame;			/* change ptr		*/
    frameLen = ETHE_MIN_LEN_LSDU;		/* change len		*/
    }

  ret = sendto (hPktSock, framePtr, frameLen,
                     0, (struct sockaddr *)&toAddr, toAddrLen);

  
  /* If this would block, sleep a few milliseconds, and try again.	*/
  /* We don't have a queue to save this packet for sending later.	*/
  if (ret < 0 && errno==EAGAIN)
    {
    sMsSleep (4);
    ret = sendto (hPktSock, framePtr, frameLen,
                     0, (struct sockaddr *)&toAddr, toAddrLen);
    }

  /* If sendto still fails, just log error. TP4 retransmission code will*/
  /* try to send it again later.					*/
  if (ret < 0)
    CLNP_LOG_ERR2 ("clnp_snet_write_raw: sendto error %d: %s", errno, strerror(errno));
  else
    ret = SD_SUCCESS;

  return (ret);
  }

/************************************************************************/
/*			clnp_snet_free					*/
/* Upper (CLNP) layer calls this function to free anything allocated	*/
/* for the SN-UNITDATA PDU.						*/
/************************************************************************/
ST_VOID clnp_snet_free (SN_UNITDATA *sn_req)
  {
  chk_free (sn_req->lpdu);	/* allocated in clnp_snet_read.	*/
  }

/************************************************************************/
/*			clnp_snet_set_multicast_filter			*/
/*----------------------------------------------------------------------*/
/* This function sets the multicast MAC addresses on which to accept	*/
/* incoming packets.							*/
/* IMPORTANT: If the ES-IS protocol is needed (e.g. for 7-Layer OSI	*/
/*	stack), the ALL-ES & ALL-IS MACs must be included in the list	*/
/*	of MAC addresses.						*/
/*									*/
/* Parameters:								*/
/*	ST_UCHAR  *mac_list	Pointer to list of MAC addresses.	*/
/*	ST_INT     num_macs	Number of MAC addresses in list.	*/
/*									*/
/* Return:								*/
/*	SD_SUCCESS (0)		if successfully set			*/
/*	error code		otherwise				*/
/************************************************************************/
ST_RET clnp_snet_set_multicast_filter (ST_UCHAR *mac_list,
				ST_INT num_macs)
  {
  /* Don't know how to set multicast filters on LINUX, but we are already*/
  /* running in promiscuous mode (see IFF_PROMISC), so OK.		*/
  return (SD_SUCCESS);
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
  /* Don't know how to set multicast filters on LINUX, but we are already*/
  /* running in promiscuous mode (see IFF_PROMISC), so OK.		*/
  return (SD_SUCCESS);
  }

/************************************************************************/
/*			clnp_snet_rx_all_multicast_start		*/
/*----------------------------------------------------------------------*/
/* This function tells the driver to BEGIN accepting ALL incoming	*/
/* multicast packets.							*/
/*									*/
/* Parameters:								*/
/*	NONE								*/
/*									*/
/* Return:								*/
/*	SD_SUCCESS (0)		if successfully STARTED discovery mode	*/
/*	error code		otherwise				*/
/************************************************************************/
ST_RET clnp_snet_rx_all_multicast_start (ST_VOID)
  {
  /* Don't know how to set multicast filters on LINUX, but we are already*/
  /* running in promiscuous mode (see IFF_PROMISC), so OK.		*/
  return (SD_SUCCESS);
  }

/************************************************************************/
/*			clnp_snet_rx_all_multicast_stop			*/
/*----------------------------------------------------------------------*/
/* This function tells the driver to STOP accepting ALL incoming	*/
/* multicast packets.							*/
/*									*/
/* Parameters:								*/
/*	NONE								*/
/*									*/
/* Return:								*/
/*	SD_SUCCESS (0)		if successfully STOPPED discovery mode	*/
/*	error code		otherwise				*/
/************************************************************************/
ST_RET clnp_snet_rx_all_multicast_stop (ST_VOID)
  {
  /* Don't know how to set multicast filters on LINUX, but we are already*/
  /* running in promiscuous mode (see IFF_PROMISC), so OK.		*/
  return (SD_SUCCESS);
  }

/************************************************************************/
/*			clnp_snet_rx_multicast_stop			*/
/*----------------------------------------------------------------------*/
/* This function tells the driver to STOP accepting incoming	        */
/* multicast packets.							*/
/*									*/
/* Parameters:								*/
/*	NONE								*/
/*									*/
/* Return:								*/
/*	SD_SUCCESS (0)		if successfully STOPPED filter mode	*/
/*	error code		otherwise				*/
/************************************************************************/
ST_RET clnp_snet_rx_multicast_stop (ST_VOID)
  {
  /* Don't know how to set multicast filters on LINUX, but we are already*/
  /* running in promiscuous mode (see IFF_PROMISC), so OK.		*/
  return (SD_SUCCESS);
  }


