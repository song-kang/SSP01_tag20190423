/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	1996-2007, All Rights Reserved.					*/
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : clnp_pcap.c						*/
/* PRODUCT(S)  : MOSI Stack						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*		This module provides the interface between the		*/
/*		Connectionless-mode Network Protocol (CLNP) and the	*/
/*		Ethernet Windows 95/98/NT driver.			*/
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
/* 11/23/11  KCR           Added casting for proper 64bit compile       */
/* 09/06/11  JRB	   Add clnp_snet_set_multicast_only_filter.	*/
/* 09/23/10  JRB	   Log error and return if Pcap interface is not*/ 
/*			   initialized (check "adhandle" before using).	*/
/* 08/02/10  JRB	   Del chk of header.len < 0 (useless on unsigned)*/
/*			   Log also descriptions if dev not found.	*/
/* 08/01/08  JRB    07     Fix max len passed to clnp_snet_frame_to_udt.*/
/* 05/08/08  JRB    06     Log device list if device not found.		*/
/* 03/12/08  JRB    05     Add WPCAP define so don't need it in vcproj.	*/
/* 10/18/07  JRB    04     Fix send error log message.			*/
/* 09/10/07  JRB    03     Get device name from 'param->network_device'	*/
/*			   (more flexible than using Registry).		*/
/*			   Log frames being sent/received.		*/
/* 08/30/07  JRB    02     Get device name from Registry.		*/
/* 09/20/06  CRM    01     All new functions to use WinPcap libraries.	*/
/************************************************************************/
#include "glbtypes.h"
#include "sysincs.h"

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;  /* Define for SLOG mechanism */
#endif
#include "clnp_log.h"

#include "clnp_usr.h"
#include "clnp_sne.h"

#include "ethertyp.h"

#include "mem_chk.h"

#ifndef WPCAP
#define WPCAP	/* need Windows-specific functions like pcap_getevent	*/
#endif

#include "pcap.h"
#include "packet32.h"
#include "ntddndis.h"

#if 0	/* DEBUG: add this for debugging if necessary.	*/
ST_UINT packet_rx_count;	/* Count of received packets.	*/
#endif

/*----------------------------------------------------------------------*/
/* NOTE: This module interfaces to the Ethernet driver for Win95/98/NT.	*/
/*----------------------------------------------------------------------*/


/*----------------------------------------------------------------------*/
/* defines and variables local to this module				*/
/*----------------------------------------------------------------------*/
#define MAX_PDU_LEN	65536

HANDLE hMMSEvent;	/* Event Object to be triggered */

/* NOTE:  may want to add these globals to CLNP_PARAM and pass to read, */
/* write, and filter functions.						*/
ST_UINT	  netmask;
pcap_t	 *adhandle;

/************************************************************************/
/*			clnp_snet_set_llc				*/
/*----------------------------------------------------------------------*/
/* This function will set the local MAC address and multicast filters.	*/
/*									*/
/* Return:								*/
/*	SD_SUCCESS (0)		if initialization successful		*/
/*	error code		otherwise				*/
/************************************************************************/
ST_RET  clnp_snet_set_llc (pcap_if_t *device)
  {
LPADAPTER	  lpAdapter = 0;
DWORD		  dwErrorCode;
PPACKET_OID_DATA  oidData;
BOOLEAN		  status;

  /* Open the selected adapter */
  lpAdapter = PacketOpenAdapter (device->name);
	
  if (!lpAdapter || (lpAdapter->hFile == INVALID_HANDLE_VALUE))
    {
    dwErrorCode = GetLastError();
    CLNP_LOG_ERR1 ("Unable to open the adapter, Error Code : %lx", dwErrorCode);
    return (SD_FALSE);
    }	

  /* Allocate a buffer to get the MAC adress */
  oidData = (PPACKET_OID_DATA) chk_calloc (1, 6 + sizeof (PACKET_OID_DATA));
	
  /* Retrieve the adapter MAC querying the NIC driver */
  oidData->Oid = OID_802_3_CURRENT_ADDRESS;
  oidData->Length = 6;
  ZeroMemory (oidData->Data, 6);
	
  status = PacketRequest (lpAdapter, FALSE, oidData);
  PacketCloseAdapter (lpAdapter);
	
  if (status)
    {
    /* Save the local MAC address */
    memcpy (clnp_param.loc_mac, oidData->Data, CLNP_MAX_LEN_MAC);
    chk_free (oidData);
    
    /* Add ES and IS Multicast */
    if (clnp_snet_rx_all_multicast_stop () != SD_SUCCESS)
      {
      CLNP_LOG_ERR0 ("Error setting multicast filters.");
      return (SD_FALSE);
      }
    }
  else
    {
    CLNP_LOG_ERR0 ("Error retrieving the MAC address of the adapter.");
    return (SD_FALSE);
    }
 
  return (SD_SUCCESS);
  }

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
pcap_if_t *alldevs;
pcap_if_t *device;
ST_CHAR errbuf[PCAP_ERRBUF_SIZE];
ST_RET retcode = SD_SUCCESS;	/* funct ret value. Init to success.*/

  if (param->network_device == NULL)	/* Caller must set this.	*/
    {
    CLNP_LOG_ERR0 ("Subnetwork initialization error: 'network_device' not set.");
    return (SD_FAILURE);
    }

  /* Retrieve the device list */
  if (pcap_findalldevs (&alldevs, errbuf) == -1)
    {
    CLNP_LOG_ERR1 ("Error in pcap_findalldevs: %s.", errbuf);
    return (SNET_ERR_DRV_OPEN);
    }
  /* Make sure pcap_freealldevs is called before returning.	*/

  /* Loop through list to find matching name. Set "device" ptr.	*/
  for (device = alldevs; device != NULL; device = device->next)
    {
    if (strcmp (device->name, param->network_device) == 0)
      break;	/* found a match. device now points to correct entry*/
    }
  if (device == NULL)
    {
    CLNP_LOG_ERR1 ("Device '%s' not found in the following device list:", param->network_device);
    for (device = alldevs; device != NULL; device = device->next)
      CLNP_LOG_ERR2 ("  %s\t(%s)", device->name, device->description);
    retcode = SNET_ERR_DRV_OPEN;
    }

  if (retcode == SD_SUCCESS)
    {
    /* Set the netmask of the interface */
    if (device->addresses != NULL)
      /* Retrieve the mask of the first address of the interface */
      netmask = ((struct sockaddr_in *)(device->addresses->netmask))->sin_addr.S_un.S_addr;
    else
      /* If the interface is without an address we are supposed to be in a C class network */
      netmask = 0xffffff; 

    /* Open the adapter */
    if ((adhandle = pcap_open_live (
                    device->name,     /* name of the device						  */
                    MAX_PDU_LEN,      /* portion of the packet to capture. It doesn't matter in this case */
                    1,		      /* promiscuous mode (nonzero means promiscuous)			  */
                    1000,	      /* read timeout in milli-seconds					  */
                    errbuf)) == NULL) /* error buffer							  */
      {
      CLNP_LOG_ERR1 ("Unable to open the adapter. %s is not supported by WinPcap.", device->name);
      retcode = SNET_ERR_DRV_OPEN;
      }
    }

  if (retcode == SD_SUCCESS)
    {
    /* adapter is open. Do additional initialization.	*/
    /* Set local MAC and multicast filters */
    if (clnp_snet_set_llc (device) != SD_SUCCESS)
      retcode = SNET_ERR_DRV_OPEN;
    }

  /* All done with device list. Free it now.	*/
  pcap_freealldevs (alldevs);

  if (retcode == SD_SUCCESS)
    {
    /* Set nonblocking mode */
    if (pcap_setnonblock (adhandle, 1, errbuf) != SD_SUCCESS)
      {
      CLNP_LOG_ERR0 ("Unable to set non-blocking mode for WinPcap.");
      retcode = SNET_ERR_DRV_OPEN;
      }

    hMMSEvent = pcap_getevent (adhandle);
    pcap_setmintocopy (adhandle, 1);
    }
  return (retcode);
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

  return (SD_SUCCESS);
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
ST_RET ret = SNET_ERR_READ;
struct pcap_pkthdr header;
const ST_UCHAR *pkt_data;

  if (adhandle == NULL)
    {
    CLNP_LOG_ERR0 ("Subnetwork read error. Not initialized.");
    return (SNET_ERR_DRV_OPEN);
    }

  /* Retrieve the packets					      */
  /* CRITICAL: this loop must break after ONE good packet is received.*/
  while((pkt_data = pcap_next (adhandle, &header)) != NULL)
    {
    if (header.len > MAX_PDU_LEN)
      break;
    
    if (memcmp (clnp_param.loc_mac, &pkt_data[CLNP_MAX_LEN_MAC], CLNP_MAX_LEN_MAC) == 0)
      continue;			

    CLSNS_LOG_IND1 ("Received Raw Ethernet frame (len = %d)", header.len);
    CLSNS_LOGH_IND (header.len, (ST_UCHAR *) pkt_data);

    sn_req->lpdu = (ST_UCHAR *) chk_calloc (ETHE_MAX_LEN_UDATA, 1);
    ret = clnp_snet_frame_to_udt ((ST_UCHAR *) pkt_data, header.len, sn_req, ETHE_MAX_LEN_UDATA);

    /* CRITICAL: if decode failed, free lpdu now. Caller will not free it.*/
    if (ret)
      chk_free (sn_req->lpdu);
#if 0	/* DEBUG: add this for debugging if necessary.	*/
    packet_rx_count++;
#endif
    break;
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

  return (SD_SUCCESS);
}

/************************************************************************/
/*			clnp_snet_free					*/
/* Upper (CLNP) layer calls this function to free anything allocated	*/
/* for the SN-UNITDATA PDU.						*/
/************************************************************************/
ST_VOID clnp_snet_free (SN_UNITDATA *sn_req)
  {

  chk_free (sn_req->lpdu);
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
#ifdef AXS4GOOSE
#define MAX_MULTICAST_FILTER	14	
#else
#define MAX_MULTICAST_FILTER	6	/*DEBUG: what can Win handle?? */
#endif
ST_RET clnp_snet_set_multicast_filter (ST_UCHAR *mac_list,
				ST_INT num_macs)
  {
ST_CHAR filter[50 + (31 * MAX_MULTICAST_FILTER)];
ST_INT i, j, index;
struct bpf_program fcode;

  if (adhandle == NULL)
    {
    CLNP_LOG_ERR0 ("Subnetwork multicast filter error. Not initialized.");
    return (SNET_ERR_DRV_OPEN);
    }

  if (num_macs > MAX_MULTICAST_FILTER)	/* don't exceed max*/
    return (SNET_ERR_DRV_ADD_ES_ADDR);

  index = sprintf (filter, "ether dst ");

  for (i=0, j=0; i<num_macs; i++, j+=6)
    {
    if (i > 0)
      index += sprintf (&filter[index], " or ether dst ");

    index += sprintf (&filter[index], "%x:%x:%x:%x:%x:%x", mac_list[j],
  						  mac_list[j+1],
						  mac_list[j+2],
						  mac_list[j+3],
						  mac_list[j+4],
						  mac_list[j+5]);
    }

  sprintf (&filter[index], " or ether dst %x:%x:%x:%x:%x:%x", clnp_param.loc_mac[0],
  							      clnp_param.loc_mac[1],
							      clnp_param.loc_mac[2],
							      clnp_param.loc_mac[3],
							      clnp_param.loc_mac[4],
							      clnp_param.loc_mac[5]);
  /* Compile the filter */
  if (pcap_compile (adhandle, &fcode, filter, 1, netmask) < 0)
    return (SNET_ERR_DRV_ADD_ES_ADDR);

  /* set the filter */
  if (pcap_setfilter (adhandle, &fcode) < 0)
    return (SNET_ERR_DRV_ADD_ES_ADDR);

  return (SD_SUCCESS);
  }

/************************************************************************/
/*			clnp_snet_set_multicast_only_filter		*/
/*----------------------------------------------------------------------*/
/* This function sets the multicast MAC addresses on which to accept	*/
/* incoming packets.							*/
/* NOTE: This function is the same as clnp_snet_set_multicast_filter	*/
/*       except it captures ONLY multicast packets.			*/
/*       Unicast packets sent to the Local MAC address are ignored.	*/
/*									*/
/* Parameters:								*/
/*	ST_UCHAR  *mac_list	Pointer to list of MAC addresses.	*/
/*	ST_INT     num_macs	Number of MAC addresses in list.	*/
/*									*/
/* Return:								*/
/*	SD_SUCCESS (0)		if successfully set			*/
/*	error code		otherwise				*/
/************************************************************************/
ST_RET clnp_snet_set_multicast_only_filter (ST_UCHAR *mac_list, ST_INT num_macs)
  {
ST_CHAR filter[50 + (31 * MAX_MULTICAST_FILTER)];
ST_INT i, j, index;
struct bpf_program fcode;

  if (adhandle == NULL)
    {
    CLNP_LOG_ERR0 ("Subnetwork multicast filter error. Not initialized.");
    return (SNET_ERR_DRV_OPEN);
    }

  if (num_macs > MAX_MULTICAST_FILTER)	/* don't exceed max*/
    return (SNET_ERR_DRV_ADD_ES_ADDR);

  index = sprintf (filter, "ether dst ");

  for (i=0, j=0; i<num_macs; i++, j+=6)
    {
    if (i > 0)
      index += sprintf (&filter[index], " or ether dst ");

    index += sprintf (&filter[index], "%x:%x:%x:%x:%x:%x", mac_list[j],
             mac_list[j+1],
             mac_list[j+2],
             mac_list[j+3],
             mac_list[j+4],
             mac_list[j+5]);
    }

  /* Compile the filter */
  if (pcap_compile (adhandle, &fcode, filter, 1, netmask) < 0)
    return (SNET_ERR_DRV_ADD_ES_ADDR);

  /* set the filter */
  if (pcap_setfilter (adhandle, &fcode) < 0)
    return (SNET_ERR_DRV_ADD_ES_ADDR);

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
ST_CHAR filter[50];
struct  bpf_program fcode;
  
  if (adhandle == NULL)
    {
    CLNP_LOG_ERR0 ("Subnetwork multicast filter error. Not initialized.");
    return (SNET_ERR_DRV_OPEN);
    }

  sprintf (filter, "ether multicast or ether dst %x:%x:%x:%x:%x:%x",  clnp_param.loc_mac[0],
  					      clnp_param.loc_mac[1],
					      clnp_param.loc_mac[2],
					      clnp_param.loc_mac[3],
					      clnp_param.loc_mac[4],
					      clnp_param.loc_mac[5]);
  /* Compile the filter */
  if (pcap_compile (adhandle, &fcode, filter, 1, netmask) < 0)
    return (SNET_ERR_DRV_ADD_ES_ADDR);

  /* set the filter */
  if (pcap_setfilter (adhandle, &fcode) < 0)
    return (SNET_ERR_DRV_ADD_ES_ADDR);

  return (SD_SUCCESS);
  }
/************************************************************************/
/*			clnp_snet_rx_all_multicast_stop			*/
/*----------------------------------------------------------------------*/
/* This function tells the driver to STOP accepting ALL incoming	*/
/* multicast packets.							*/
/* NOTE: If multicast filters were previously set up by calling		*/
/*	gse_set_multicast_filter, they will automatically be active	*/
/*	again when this function returns.				*/
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
ST_CHAR filter[100];
struct  bpf_program fcode;
  
  if (adhandle == NULL)
    {
    CLNP_LOG_ERR0 ("Subnetwork multicast filter error. Not initialized.");
    return (SNET_ERR_DRV_OPEN);
    }

  sprintf (filter, "ether dst 09:00:2b:00:00:04 or ether dst 09:00:2b:00:00:05 or ether dst %x:%x:%x:%x:%x:%x",  
					      clnp_param.loc_mac[0],
  					      clnp_param.loc_mac[1],
					      clnp_param.loc_mac[2],
					      clnp_param.loc_mac[3],
					      clnp_param.loc_mac[4],
					      clnp_param.loc_mac[5]);
  /* Compile the filter */
  if (pcap_compile (adhandle, &fcode, filter, 1, netmask) < 0)
    return (SNET_ERR_DRV_ADD_ES_ADDR);

  /* set the filter */
  if (pcap_setfilter (adhandle, &fcode) < 0)
    return (SNET_ERR_DRV_ADD_ES_ADDR);

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
ST_CHAR filter[30];
struct  bpf_program fcode;
  
  if (adhandle == NULL)
    {
    CLNP_LOG_ERR0 ("Subnetwork multicast filter error. Not initialized.");
    return (SNET_ERR_DRV_OPEN);
    }

  sprintf (filter, "ether dst %x:%x:%x:%x:%x:%x",  
					      clnp_param.loc_mac[0],
  					      clnp_param.loc_mac[1],
					      clnp_param.loc_mac[2],
					      clnp_param.loc_mac[3],
					      clnp_param.loc_mac[4],
					      clnp_param.loc_mac[5]);
  /* Compile the filter */
  if (pcap_compile (adhandle, &fcode, filter, 1, netmask) < 0)
    return (SNET_ERR_DRV_ADD_ES_ADDR);

  /* set the filter */
  if (pcap_setfilter (adhandle, &fcode) < 0)
    return (SNET_ERR_DRV_ADD_ES_ADDR);

  return (SD_SUCCESS);
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
	ST_UCHAR *framePtr,	/* Pointer to complete frame including MACs */
	size_t    frameLen)	/* Length of complete frame		    */
  {
ST_RET retcode = SD_SUCCESS;		  /* assume success		    */
int pcap_ret;				  /* value returned from pcap_send..*/
ST_UCHAR local_frame [ETHE_MIN_LEN_LSDU]; /* local copy of short frame	    */

  if (adhandle == NULL)
    {
    CLNP_LOG_ERR0 ("Subnetwork write error. Not initialized.");
    return (SNET_ERR_DRV_OPEN);
    }

  CLSNS_LOG_REQ1 ("Sending Raw Ethernet frame (len = %d)", frameLen);
  CLSNS_LOGH_REQ (frameLen, framePtr);

  /* Must write at least ETHE_MIN_LEN_LSDU bytes.			  */
  /* If frame is shorter, data is copied to local buffer, and framePtr	  */
  /* and frameLen are changed to reference the local buffer.		  */
  if (frameLen < ETHE_MIN_LEN_LSDU)
    {
    memcpy (local_frame, framePtr, frameLen);	/* copy to local buffer	  */
    framePtr = local_frame;			/* change ptr		  */
    frameLen = ETHE_MIN_LEN_LSDU;		/* change len		  */
    }
    
  pcap_ret = pcap_sendpacket (adhandle,		/* Adapter		  */
                   framePtr,			/* buffer with the packet */
		   (int) frameLen);			/* size			  */
  if (pcap_ret != 0)
    {
    CLNP_LOG_ERR1 ("%s", pcap_geterr (adhandle));
    retcode = SNET_ERR_WRITE;			/* write failed		  */
    }

  return (retcode);
  }
