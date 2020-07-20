/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1996, All Rights Reserved.		        	*/
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : clnp_usr.h						*/
/* PRODUCT(S)  : Lean-T Stack						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*		This header file defines the interface between the	*/
/*		Connectionless-mode Network Protocol (CLNP) and the	*/
/*		CLNP-user in order to exchange Network Service Data	*/
/*		Units (NSDUs).						*/
/*		Note that the CLNP is implemented as non-segmenting	*/
/*		subset of the full protocol. 				*/
/*									*/
/*  For information see the:						*/
/*	ISO 8473 "Information processing systems - Data communication -	*/
/*	  Protocol providing the connectionless-mode  network service";	*/
/*	ISO 8348 "Information processing systems - Data communication -	*/
/*	  Network service definition. Addendum 1: Connectionless-mode	*/
/*	  transmission".						*/
/*	ISO 9542 "Information processing systems - Telecommunications	*/
/*	  and information exchange beetween systems - End system to	*/
/*	  Intermediate system routing exchange protocol for use with	*/
/*	  ISO 8473.							*/
/*									*/
/*									*/
/* GLOBAL STRUCTURES DEFINED IN THIS MODULE :				*/
/*									*/
/*			CLNP_PARAM struct				*/
/*			N_UNITDATA struct				*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/*			clnp_init					*/
/*			clnp_end					*/
/*			clnp_alloc					*/
/*			clnp_free					*/
/*			clnp_write					*/
/*			clnp_timer_tick					*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 06/14/10  JRB	   Del clnp_read*, hMainEvent, hGooseEvent	*/
/*			   (use new subnet_serve to receive packets).	*/
/*			   Add clnpl_log_nsdu.				*/
/* 09/10/07  JRB    09     Add network_device to CLNP_PARAM.		*/
/* 10/24/03  JRB    08     Move clnp_debug_sel to slog.h.		*/
/* 02/29/00  JRB    07     Add clnp_status proto.			*/
/* 08/11/99  JRB    06     Add DBL_LNK to N_UNITDATA for multithreading.*/
/* 08/05/99  JRB    05     Clean up multithreading.			*/
/* 08/02/99  JRB    04     Use glbsem for multithreading.		*/
/* 07/30/99  JRB    03     Add "multi-threading" support.		*/
/* 12/04/98  JRB    02     Add "loc_nsap" to N_UNITDATA.		*/
/* 08/13/98  JRB    01     Lint cleanup.				*/
/* 05/27/97  JRB   7.00    MMSEASE 7.0 release.				*/
/* 04/10/97  JRB    06     Moved error defs from clnp_err.h to here	*/
/*			   and changed to use base from "glberror.h".	*/
/* 03/03/97  EJV    05     Removed CLNP_LOG_ALWAYS			*/
/* 10/04/96  JRB    04     Added loc_mac to N_UNITDATA.			*/
/* 07/22/96  EJV    03     Changed CLNP_DEF_ESH_DELAY from 5 to 0.	*/
/* 07/19/96  EJV    02     Removed ns_userdata_len from clnp_param	*/
/*			   Moved CLNP_DEF_LEN_HDR to clnp.h and deleted	*/
/*			   unused defines				*/
/* 05/23/96  EJV    01     Created					*/
/************************************************************************/

#ifndef CLNP_USR_INCLUDED
#define CLNP_USR_INCLUDED

#include "gen_list.h"		/* Need DBL_LNK definition.		*/
#include "glbsem.h"		/* Need ST_EVENT_SEM definition.	*/
#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------*/
/* Error codes returned from the CLNP interface funcs to the CLNP-user	*/
/* NOTE: Values begin from E_CLNP (found in glberror.h).		*/
/*----------------------------------------------------------------------*/
/* CLNP general errors */
#define	CLNP_ERR_CFG_FILE	0x3400	/* Errors found in cfg file (or required*/
					/* params not configured = local MAC and*/
					/* local NSAP)				*/
#define	CLNP_ERR_NOT_INIT 	0x3401	/* CLNP has not been initialized	*/
#define	CLNP_ERR_MEM_ALLOC 	0x3402	/* allocating memory failed		*/
#define	CLNP_ERR_NULL_PTR	0x3403	/* NULL pointer passed to clnp_... func	*/

/* CLNP parsing clnp_param structure errors */
/* -&-&- */
/* CAN NOT recover from this parameter error !!!			*/
#define	CLNP_ERR_NSAP_LEN	0x3404	/* NSAP length is 0 or more then allowed*/
/* CAN recover from parameter errors below				*/
#define	CLNP_ERR_LIFETIME	0x3405	/* Invalid PDU lifetime			*/
#define	CLNP_ERR_LIFETIME_DEC	0x3406	/* Invalid PDU lifetime	decrement value	*/
#define CLNP_ERR_ESH_CFG_TIMER	0x3407	/* Invalid ESH Configuration Timer value*/
#define CLNP_ERR_ESH_DELAY	0x3408	/* Invalid delay value for first ESH	*/
#define CLNP_ERR_MAC_ADDR	0x3409	/* Local MAC address not configured	*/
					/* (for ADLC sub-network)		*/
#define	CLNP_ERR_UDATA_LEN	0x3410	/* CLNP-user data length (too big)	*/
/* last error code is also used for CLNP PDU encoding error */
/* -&-&- */

/* CLNP PDU parsing (decoding) errors */
#define	CLNP_ERR_PDU_MAC_ADDR	0x3420	/* The NPDU MAC address is not local MAC*/
					/* or All ES MAC.			*/
#define	CLNP_ERR_PDU_ID		0x3421	/* invalid/not supported PDU identifier	*/
#define	CLNP_ERR_PDU_VER	0x3422	/* invalid/not supported PDU version	*/
#define	CLNP_ERR_PDU_TYPE	0x3423	/* invalid/not supported PDU type	*/
#define	CLNP_ERR_PDU_LEN	0x3424	/* received PDU len does not match the	*/
					/* len indicated by sub-network		*/
#define	CLNP_ERR_PDU_EXPIRED	0x3425	/* DT or ER PDU's lifetime expired	*/
#define	CLNP_ERR_PDU_NSAP_ADDR	0x3426	/* PDU not addressed to our local NSAP	*/
#define	CLNP_ERR_PDU_SEGMENTING	0x3427	/* Segmented PDUs not supported		*/
#define	CLNP_ERR_PDU_CHECKSUM	0x3428	/* PDU checksum verification failed	*/
#define	CLNP_ERR_PDU_LAST_SEG	0x3429	/* Last seg bit not set (unsegmented PDU)*/
#define	CLNP_ERR_PDU_ER_PDU	0x342A	/* code turned off for ER PDU processing*/

/* LLC encoding/decoding errors */
#define	LLC_ERR_SRC_ADDR	0x3481	/* LLC header Source field invalid	*/
#define	LLC_ERR_DEST_ADDR	0x3482	/* LLC header Dest filed invalid	*/
#define	LLC_ERR_CONTROL		0x3483	/* LLC header Control field invalid	*/

/*----------------------------------------------------------------------*/
/* Error codes returned from the sub-network interface functions.	*/
/* NOTE: Values begin from E_SUBNET (found in glberror.h).		*/
/*----------------------------------------------------------------------*/
#define	SNET_ERR_INIT		0x3501	/* Init sub-net interface failed*/
#define	SNET_ERR_WRITE		0x3502	/* sub-net write func failed	*/
#define	SNET_ERR_READ		0x3503	/* sub-net read func failed or no data	*/
#define	SNET_ERR_MAC_INVALID	0x3504	/* invalid MAC address, unable	*/
					/* to obtain requested ALL ES,	*/
					/* ALL IS, or local MAC address.*/
#define	SNET_ERR_FRAME_LEN	0x3505	/* received more data than	*/
					/* reserved in buffer.		*/
#define	SNET_ERR_UDATA_LEN	0x3506	/* invalid len of data to send (too large)*/

/* sub-net errors specific to the Ethernet driver */
#define	SNET_ERR_DRV_OPEN	0x3520	/* open driver failed		*/
#define	SNET_ERR_DRV_LOC_MAC	0x3521	/* obtain local MAC addr from driver failed*/
#define SNET_ERR_DRV_ADD_ES_ADDR 0x3522	/* add All ES Address failed	*/
#define	SNET_ERR_DRV_BIND_LSAP	0x3523	/* bind to LSAP failed		*/
#define	SNET_ERR_DRV_POST_BUFS	0x3524	/* post buffers to driver failed*/

/*----------------------------------------------------------------------*/
/* CLNP logging types 							*/

#define CLNP_LOG_ERR		0x00000001L
#define CLNP_LOG_NERR		0x00000002L

/* to log CLNP-user indication or request */
#define CLNP_LOG_REQ		0x00000010L
#define CLNP_LOG_IND		0x00000020L

/* to log CLNP encoding/decoding or LLC encoding/decoding */

#define CLNP_LOG_ENC_DEC	0x00000100L
#define CLNP_LOG_LLC_ENC_DEC	0x00000200L

/* to log data read/written to sub-network */
#define CLSNS_LOG_REQ		0x00001000L
#define CLSNS_LOG_IND		0x00002000L

/* CLNP is using clnp_debug_sel variable to turn on/off different	*/
/* types of logging (see slog.h).					*/

/*----------------------------------------------------------------------*/
/* The defines with CLNP_MAX_... specify the maximum values the		*/
/* ISO 8473 or this implementation allows. The CLNP_DEF_... defines	*/
/* specify the default values used to initialize the CLNP_PARAM struct.	*/
/*									*/
/* Note:  The max length of NS-USERDATA is 64512 bytes but this number	*/
/*	  is limited by underlying sub-network to:			*/
/*		for ADLC     -> 32676 - CLNP_DEF_LEN_HDR - LLC_HDR_LEN	*/
/*		for Ethernet ->  1500 - CLNP_DEF_LEN_HDR - LLC_HDR_LEN	*/


#define CLNP_MAX_LEN_MAC	6	/* Max len of MAC addr		*/
#define CLNP_MAX_LEN_NSAP	20	/* Max len of NSAP addr		*/

#define CLNP_MIN_PDU_LIFETIME	  (ST_UCHAR) 1	/* min PDU lifetime in 500msec units		*/
#define CLNP_MAX_PDU_LIFETIME	  (ST_UCHAR) 255	/* max PDU lifetime in 500msec units		*/
#define CLNP_DEF_PDU_LIFETIME	  (ST_UCHAR) 50	/* def PDU lifetime in 500msec units		*/
#define CLNP_DEF_PDU_LIFETIME_DEC (ST_UCHAR) 1	/* PDU lifetime decrement (in 500msec units)	*/


#define CLNP_MAX_ESH_CFG_TIMER	(ST_UINT16) 32767	/* max value for ESH Configuration Timer, it is	*/
						/* related to Holding Time in ESH which is	*/
						/* 2*CfgTimer=2*32767=65534 (must be ST_UINT16)	*/
#define CLNP_DEF_ESH_CFG_TIMER	(ST_UINT16) 120	/* default ESH Configuration Timer (in seconds)	*/
#define CLNP_DEF_ESH_DELAY	(ST_UINT16) 0	/* default delay before first ESH will be sent	*/



/*----------------------------------------------------------------------*/
/*			CLNP_PARAM struct				*/
/*									*/
/* This struct is used to supply actual parameters needed for operation	*/
/* of the CLNP.								*/
/* It is set by the NS-USER during call to clnp_init() function from	*/
/* configuration file or from hard coded module clnp_hc.c.		*/
/*									*/
/* NOTE: The NSAP address is represented as sequence of unsigned bytes.	*/
/*	 The first byte is the length of the NSAP address. This makes	*/
/*	 it easy to put the NSAP to PDU.				*/
/* Note: The pdu_lifetime_dec specifies by how many units the lifetime	*/
/*	 value in received DT or ER PDU should be decremented. It has	*/
/*	 to be at least 1. In case the transit delay plus processing	*/
/*	 time is larger then 500msec the pdu_life_time_dec should be	*/
/*	 adjusted accordingly.						*/

typedef struct
  {
  ST_UCHAR	 pdu_lifetime;		/* PDU lifetime (in 500 msec units) for	*/
				/* outgoing DT PDUs.			*/
  				/*   init to CLNP_DEF_PDU_LIFETIME	*/
  ST_UCHAR	 pdu_lifetime_dec;	/* PDU lifetime decrement (1=500msec)	*/
  				/* for incomming DT or ER PDUs.		*/
  				/*   init to CLNP_DEF_PDU_LIFETIME_DEC	*/
  ST_UINT16 esh_cfg_timer;		/* How often we report our presence to	*/
				/* other network entities (in seconds)	*/
				/*   init to CLNP_DEF_ESH_CFG_TIMER	*/
  ST_UINT16 esh_delay;		/* Delay time before first ESH is sent 	*/
				/*   init to CLNP_DEF_ESH_DELAY		*/
  ST_UCHAR	 loc_mac  [CLNP_MAX_LEN_MAC];	/* Local MAC address		*/
				/* For ADLC the NS-USER sets the loc_mac*/
				/* DEBUG: Now the loc_mac has to match	*/
				/* the address in adlc.cfg !!!		*/
				/* For the Ethernet this param will be	*/
				/* read from the driver during init.	*/
  ST_UCHAR	 loc_nsap [1+CLNP_MAX_LEN_NSAP];/* Local len & NSAP address	*/
  ST_CHAR *network_device;	/* network device name. May be used to	*/
				/* select device on some platforms	*/
  }CLNP_PARAM;

#define CLNP_PARAM_LEN	sizeof (CLNP_PARAM)


extern CLNP_PARAM clnp_param;	/* This struct holds parameters needed	*/
				/* for the CLNP operations.		*/
				/* It is set by the CLNP-user before	*/
				/* calling the clnp_init() function.	*/




/*----------------------------------------------------------------------*/
/*			N_UNITDATA struct				*/
/*									*/
/* This struct is used to pass N_UNITDATA request primitive to CLNP	*/
/* and to receive N_UNITDATA indication from CLNP.			*/
/* This struct uses the following trick to make the size of data buffer	*/
/* configurable:							*/
/*	The last entry in the structure is a tiny (2 bytes) buffer.	*/
/*	To allow for any size data_buf, we can allocate extra bytes	*/
/*	after this struct.						*/
/*	For example if we want data_buf to be 4096 bytes, we could use	*/
/*	the following malloc call:					*/
/*	  N_UNITDATA *req = malloc (sizeof (N_UNITDATA) + 4096 - 2 );	*/
/*	The buffer can then be filled in, for example, as follow:	*/
/*	  memcpy (req->data_buf, raw_data, 4096);			*/
/*									*/
/* NOTE: The "Local NSAP" is not passed back and forth in this struct.	*/
/*	 Only one local NSAP is allowed.				*/
/*									*/
/* NOTE: The NSAP address is represented as sequence of unsigned bytes.	*/
/*	 The first byte is the length of the NSAP address. This makes	*/
/*	 it easy to get a NSAP from a NSDU.				*/


#define CLNP_MIN_LEN_UDATA  2
				/* min length of data_buf defined in	*/
				/* N_UNITDATA struct. Do not use 1	*/
				/* because optimizer may NOT treat this	*/
				/* field as an array.			*/

typedef struct
  {
  DBL_LNK link;		/* for multithreading, must go on linked list	*/
  ST_BOOLEAN rem_mac_valid;			/* SD_TRUE if MAC addr valid	*/
  ST_UCHAR loc_mac  [CLNP_MAX_LEN_MAC];		/* Local MAC addr		*/
  ST_UCHAR rem_mac  [CLNP_MAX_LEN_MAC];		/* Remote MAC addr		*/
  ST_UCHAR loc_nsap [1+CLNP_MAX_LEN_NSAP];	/* Local len & NSAP addr*/
  ST_UCHAR rem_nsap [1+CLNP_MAX_LEN_NSAP];	/* Remote len & NSAP addr	*/
  ST_UINT16 data_len;				/* Data length in bytes		*/
  ST_UCHAR data_buf [CLNP_MIN_LEN_UDATA];	/* Data buffer			*/
  }N_UNITDATA;

#define N_UNITDATA_LEN		sizeof (N_UNITDATA)





/************************************************************************/
/*			clnp_init					*/
/*----------------------------------------------------------------------*/
/* This function will initialize the operation of CLNP and the interface*/
/* to underlying sub-network.						*/
/* The CLNP-user should set the parameters in the clnp_param structure	*/
/* before calling this function.					*/
/*									*/
/* Parameters:								*/
/*	ST_UINT	ns_userdata_len		Max size of NS-USERDATA		*/
/*									*/
/* Return:								*/
/*	SD_SUCCESS (0)		if initialization successful		*/
/*	error code		otherwise				*/
/************************************************************************/
ST_RET clnp_init (ST_UINT ns_userdata_len);


/************************************************************************/
/*			clnp_end					*/
/*----------------------------------------------------------------------*/
/* This function will terminate the operation of the CLNP and cleanup	*/
/* the interface to underlying subnetwork.				*/
/*									*/
/* Parameters:								*/
/*	ST_VOID			none					*/
/*									*/
/* Return:								*/
/*	SD_SUCCESS (0)		if termination successful		*/
/*	error code		otherwise				*/
/************************************************************************/
ST_RET clnp_end (ST_VOID);


/************************************************************************/
/*			clnp_alloc					*/
/*----------------------------------------------------------------------*/
/* This function will allocate a buffer for N_UNITDATA to pass data	*/
/* between the CLNP and the CLNP-user.					*/
/*									*/
/* Parameters:								*/
/*	ST_UINT	data_len	Length of data in bytes. The length	*/
/*				of CLNP-user data has to be:		*/
/*				0 < data_len <= CLNP_MAX_LEN_UDATA	*/
/*									*/
/* Return:								*/
/*	N_UNITDATA *		pointer to allocated memory		*/
/*	NULL			if function fails to alloc memory	*/
/************************************************************************/
N_UNITDATA *clnp_alloc (ST_UINT data_len);


/************************************************************************/
/*			clnp_free					*/
/*----------------------------------------------------------------------*/
/* This function will free buffer allocated for N_UNITDATA passed	*/
/* between the CLNP and the CLNP-user.					*/
/*									*/
/* Parameters:								*/
/*	N_UNITDATA *		memory pointer to free			*/
/*									*/
/* Return:								*/
/*	ST_VOID								*/
/************************************************************************/
ST_VOID clnp_free (N_UNITDATA *req);


/************************************************************************/
/*			clnp_write					*/
/*----------------------------------------------------------------------*/
/* This function is used by the CLNP-user to write N_UNITDATA.request	*/
/* to the CLNP.								*/
/* The CLNP will free the req pointer.					*/
/*									*/
/* Parameters:								*/
/*	N_UNITDATA *req		ptr to N_UNITDATA.request to send	*/
/*				This pointer should be allocated using	*/
/*				the clnp_alloc function.		*/
/*									*/
/* Return:								*/
/*	SD_SUCCESS (0)		if N_UNITDATA.request sent successfully	*/
/*	error code		otherwise				*/
/************************************************************************/
ST_RET clnp_write (N_UNITDATA *req);


/************************************************************************/
/*			clnp_timer_tick					*/
/*----------------------------------------------------------------------*/
/* This function is called on 1 second intervals by the CLNP-user (TP4)	*/
/* When this function is called, CLNP decrements the Configuration	*/
/* Timer (used to trigger sending of ES-Hellos), and the Holding Timers	*/
/* (used to trigger clearing the {NSAP,MAC} table entries).		*/
/*									*/
/* Parameters:								*/
/*	ST_VOID			none				       	*/
/*									*/
/* Return:								*/
/*	ST_VOID			none					*/
/************************************************************************/
ST_VOID clnp_timer_tick (ST_VOID);


/************************************************************************/
/*			clnp_config					*/
/*----------------------------------------------------------------------*/
/* This function will initialize the CLNP parameters.			*/
/*									*/
/* Parameters:								*/
/*	ST_VOID			none					*/
/*									*/
/* Return:								*/
/*	SD_SUCCESS (0)		if configuration successful		*/
/*	error code		otherwise				*/
/************************************************************************/
ST_RET clnp_config (ST_VOID);

/************************************************************************/
/*			clnp_status					*/
/*----------------------------------------------------------------------*/
/* This function checks to see if CLNP is running.			*/
/*									*/
/* Return:								*/
/*	SD_SUCCESS if running, else error code.				*/
/************************************************************************/
ST_RET clnp_status (ST_VOID);

/************************************************************************/
/* Miscellaneous							*/
/************************************************************************/
ST_VOID	clnpl_log_nsdu (N_UNITDATA *req, ST_ULONG log_mask);

/************************************************************************/
/* Multi-threading prototypes and externs.				*/
/************************************************************************/
extern ST_EVENT_SEM hMMSEvent;

#ifdef __cplusplus
}
#endif

#endif  /* end of 'already included' 	*/


