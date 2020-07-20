/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	1996-2005, All Rights Reserved.					*/
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : clnp_sne.h						*/
/* PRODUCT(S)  : Lean-T Stack						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*		This header file defines the interface between the	*/
/*		Connectionless-mode Network Protocol (CLNP) and the	*/
/*		underlying sub-network.					*/
/*									*/
/* GLOBAL STRUCTURES DEFINED IN THIS MODULE :				*/
/*									*/
/*			SN_UNITDATA struct				*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/*			clnp_snet_init					*/
/*			clnp_snet_term					*/
/*			clnp_snet_read					*/
/*			clnp_snet_write					*/
/*			clnp_snet_free					*/
/*			clnp_snet_add_multicast_mac			*/
/*			clnp_snet_ext_write				*/
/*			clnp_snet_timer_tick 				*/
/*			clnp_snet_update_is				*/
/*			clnp_snet_update_es				*/
/*			clnp_snet_lookup_mac				*/
/*			clnp_snet_get_all_is_mac			*/
/*			clnp_snet_get_all_es_mac			*/
/*			clnp_snet_get_local_mac				*/
/*			clnp_snet_get_max_udata_len			*/
/*			clnp_snet_get_type				*/
/*			clnp_snet_create_es_table			*/
/*			clnp_snet_check_mac				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 09/06/11  JRB	   Add clnp_snet_set_multicast_only_filter.	*/
/* 06/14/10  JRB	   Del obsolete clnp_snet_read_hook_*.		*/
/*			   SN_UNITDATA: add linked list pointers.	*/
/* 12/12/06  JRB    15     Add hPktSock extern.				*/
/* 03/20/06  JRB    14     Add clnp_snet_write_raw.			*/
/*			   Include clnp_usr.h (need some defs from it).	*/
/* 07/25/05  JRB    13     Add comment explaining "lpdu_len".		*/
/* 02/08/05  JRB    12     Increase ETHE_MAX_LEN_LSDU to 1518 to allow	*/
/*			   for 802.1Q QTag Prefix.			*/
/*			   Del ETYPE_UNITDATA, code no longer uses it.	*/
/*			   Add clnp_snet_frame_to_udt.			*/
/*			   Add clnp_snet_read_hook_*.			*/
/* 11/20/02  ASK    11     Added ETYPE_UNITDATA struct, clnp_etype_write*/
/*			   proto					*/
/* 10/24/02  NAV    10     Add clnp_snet_rx_multicast_stop proto.       */
/* 11/15/01  JRB    09     Add more "multicast" functions.		*/
/*			   Add clnpl_log_snsdu proto.			*/
/* 02/21/00  JRB    08     Del "free_lpdu" flag from SN_UNITDATA, and	*/
/*			   use portable "clnp_snet_free" instead.	*/
/* 01/28/99  MDE    07     Added size to ETHE_ALL_ES, ETHE_ALL_IS	*/
/* 11/30/98  JRB    06     Added Ethernet defines.			*/
/*			   Added clnp_snet_add_multicast_mac prototype.	*/
/*			   Added clnp_snet_check_mac prototype.		*/
/* 09/23/97  EJV    05     Added clnp_snet_ext_write for UCA_SMP.	*/
/*			   Aligned fields in SN_UNITDATA struct.	*/
/* 06/19/97  EJV    04     Added clnp_snet_create_es_table prototype.	*/
/* 05/27/97  JRB   7.00    MMSEASE 7.0 release.				*/
/* 10/17/96  EJV    03     Added define SUBNET_PROFI.			*/
/* 07/22/96  EJV    02     Added func clnp_snet_get_type () and defines	*/
/*			   for implemented sub-networks SUBNET_ADLC and	*/
/*			   SUBNET_ETHE.					*/
/* 05/23/96  EJV    01     Created					*/
/************************************************************************/

#ifndef CLNP_SNE_INCLUDED
#define CLNP_SNE_INCLUDED

#include "clnp_usr.h"	/* need CLNP_MAX_LEN_MAC & CLNP_PARAM	*/

#ifdef __cplusplus
extern "C" {
#endif

/* implemented sub-networks types */
#define SUBNET_ADLC	1
#define SUBNET_ETHE	2
#define SUBNET_PROFI	3

/*----------------------------------------------------------------------*/
/* Structure below is used to pass SN_UNITDATA to/from read/write	*/
/* functions between the CLNP and CLNP-Subnetwork interface.		*/

typedef struct sn_unitdata
  {
  /* CRITICAL: First 2 parameters used to add this struct to linked	*/
  /* lists using list_add_last, etc.					*/
  struct sn_unitdata *next;	/* CRITICAL: DON'T MOVE.	*/
  struct sn_unitdata *prev;	/* CRITICAL: DON'T MOVE.	*/
  ST_UCHAR   loc_mac [CLNP_MAX_LEN_MAC]; /* Buffer for local MAC addr	*/
  ST_UCHAR   rem_mac [CLNP_MAX_LEN_MAC]; /* Buffer for remote MAC addr	*/
  /* WARNING: The "lpdu_len" param does NOT always contain the PDU length.*/
  /* It contains the "Length/Type" field of the MAC frame as defined in	*/
  /* IEEE 802.3. Any value greater than or equal to 0x600 must be	*/
  /* interpreted as the "Type" of the MAC frame.			*/
  /* Renaming this parameter would clarify the code, but too much	*/
  /* existing code is already using it.					*/
  ST_UINT16  lpdu_len;		/* IEEE 802.3 "Length/Type" field.	*/
  ST_UCHAR  *lpdu;		/* Pointer to LPDU buffer to send.	*/
  }SN_UNITDATA;

#define SN_UNITDATA_LEN	sizeof (SN_UNITDATA)

/*----------------------------------------------------------------------*/
/* Defines for type of MAC address in received PDU.			*/
/* Returned by "clnp_snet_check_mac".					*/
/*----------------------------------------------------------------------*/
#define CLNP_MAC_INVALID	0	/* Invalid MAC address (not one	*/
					/* of addresses def below)	*/
#define CLNP_MAC_LOCAL		1	/* Our MAC address		*/
#define CLNP_MAC_ALL_ES		2	/* All-ES (End Systems)	address	*/
#define CLNP_MAC_ALL_IS		3	/* All-IS (Intermediate Systems)*/
#define CLNP_MAC_GOOSE		4	/* Possible GOOSE address	*/

/*----------------------------------------------------------------------*/
/* Defines and externs for Ethernet Subnetwork (SUBNET_ETHE) only.	*/
/*----------------------------------------------------------------------*/
#define ETHE_MAC_LEN		6

#define ETHE_LEN_HEAD		(2*ETHE_MAC_LEN + 2)
#define ETHE_MIN_LEN_LSDU	60
/* Max len for normal frame is 1514, but with QTag Prefix it could be 1518*/
#define ETHE_MAX_LEN_LSDU	1518
#define ETHE_MAX_LEN_UDATA	1500	/* max. MAC User data length	*/

#define ETHE_LEN_QTAG_PREFIX	4	/* 802.1Q (VLAN) header length	*/
#define ETHE_LEN_LENTYPE	2	/* MAC Len/type field length	*/

/* Ethernet frame structure */
/* WARNING: This structure is not convenient for representing frames	*/
/* containing the IEEE 802.1Q QTag Prefix. The QTag Prefix would come	*/
/* between src_addr and frame_len in this structure.			*/
/* Most code using this structure was written before QTag existed.	*/
/* New code should not use this structure.				*/
typedef struct
  {
  ST_UCHAR dst_addr [ETHE_MAC_LEN];	/* destination MAC address	*/
  ST_UCHAR src_addr [ETHE_MAC_LEN];	/* source MAC address		*/
  ST_UCHAR frame_len[2];			/* total frame length		*/
  ST_UCHAR data_buf [ETHE_MAX_LEN_UDATA];	/* data buffer			*/
  } ETHE_FRAME;

#define ETHE_FRAME_LEN	sizeof (ETHE_FRAME)

/*----------------------------------------------------------------------*/
/* Global variables.							*/
/*----------------------------------------------------------------------*/
/* All ES and all IS addresses */
extern ST_UCHAR ETHE_ALL_ES [ETHE_MAC_LEN];
extern ST_UCHAR ETHE_ALL_IS [ETHE_MAC_LEN];
extern int hPktSock;	/* Packet Socket handle (for LINUX, etc.)	*/
  
/*----------------------------------------------------------------------*/
/* Interface functions to underlying sub-network			*/
/*----------------------------------------------------------------------*/

ST_RET clnp_snet_init (CLNP_PARAM *clnp_param);
ST_RET clnp_snet_term (ST_VOID);
ST_RET clnp_snet_read  (SN_UNITDATA *sn_req);
ST_RET clnp_snet_write (SN_UNITDATA *sn_req);
ST_RET clnp_etype_write (SN_UNITDATA *sn_req);
/* clnp_snet_write_raw replaces OBSOLETE clnp_snet_write and clnp_etype_write.*/
ST_RET clnp_snet_write_raw (
	ST_UCHAR *framePtr,	/* Pointer to complete frame including MACs*/
	size_t frameLen);	/* Length of complete frame		*/
ST_VOID clnp_snet_free (SN_UNITDATA *sn_req);
ST_VOID	clnpl_log_snsdu (SN_UNITDATA *sn_req, ST_ULONG log_mask);
ST_RET clnp_snet_add_multicast_mac (ST_UCHAR *mac_buf);
ST_RET clnp_snet_set_multicast_filter (ST_UCHAR *mac_list, ST_INT num_macs);
ST_RET clnp_snet_set_multicast_only_filter (ST_UCHAR *mac_list, ST_INT num_macs);
ST_RET clnp_snet_rx_all_multicast_start (ST_VOID);
ST_RET clnp_snet_rx_all_multicast_stop (ST_VOID);
ST_RET clnp_snet_rx_multicast_stop (ST_VOID);

ST_RET clnp_snet_ext_write (SN_UNITDATA *sn_req, ST_LONG user_id);
ST_VOID    clnp_snet_timer_tick (ST_VOID);

ST_RET clnp_snet_update_is (ST_UCHAR *rem_nsap, ST_UCHAR *rem_mac, ST_UINT16 holding_time);
ST_RET clnp_snet_update_es (ST_UCHAR *rem_nsap, ST_UCHAR *rem_mac, ST_UINT16 holding_time);

ST_RET clnp_snet_lookup_is (ST_UCHAR *rem_mac);
ST_RET clnp_snet_lookup_es (ST_UCHAR *rem_nsap, ST_UCHAR *rem_mac);

ST_RET clnp_snet_get_all_is_mac (ST_UCHAR *mac_buf);
ST_RET clnp_snet_get_all_es_mac (ST_UCHAR *mac_buf);
ST_RET clnp_snet_get_local_mac  (ST_UCHAR *mac_buf);
ST_UINT16  clnp_snet_get_max_udata_len (ST_VOID);
ST_INT clnp_snet_get_type (ST_VOID);

ST_RET clnp_snet_create_es_table (ST_UINT max_count);
ST_INT clnp_snet_check_mac (ST_UCHAR *mac_addr);

/* Copy data from raw frame to SN_UNITDATA struct.			*/
ST_RET clnp_snet_frame_to_udt (ST_UINT8 *frame_buf,	/* ptr to raw frame*/
			ST_INT frame_len,		/* len of raw frame*/
			SN_UNITDATA *sn_req,
			ST_INT udata_max_len);	/* max user data len	*/

#ifdef __cplusplus
}
#endif

#endif  /* end of 'already included' 	*/







