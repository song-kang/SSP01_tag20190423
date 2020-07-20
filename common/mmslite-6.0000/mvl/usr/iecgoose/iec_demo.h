/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	2002-2005, All Rights Reserved					*/
/*								       	*/
/* MODULE NAME : iec_demo.h						*/
/* PRODUCT(S)  : MMSEASE-LITE					       	*/
/*								       	*/
/* MODULE DESCRIPTION : 					       	*/
/*	Module contains sample definitions for sending and receiving	*/
/*	IEC GOOSE							*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*	NONE								*/						
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 02/15/12  JRB	   Add iecGooseSubscribeExtRefAll,		*/
/*			   iecGooseSubscriberFind, iecGooseUnSubscribeAll.*/
/*			   IEC_GOOSE_RX_USER_INFO: add dsNvl,time_received.*/
/*			   Add callback_function.			*/
/* 01/11/12  JRB	   iecGoosePubCreate: add retrans_curve arg.	*/
/*			   Repl curve_pointer w/ retrans_curve in struct.*/
/*			   Del global retrans_curve.			*/
/* 07/14/08  JRB     15    Add more log macros.				*/
/* 07/02/08  JRB     14    Chg all ConfRev to ST_UINT32 to be consistent*/
/* 06/20/08  JRB     13    Add LOG_RX macros.				*/
/* 08/09/06  JRB     12    Add create_subscription_from_nvl proto.	*/
/* 08/12/05  JRB     11    Add "mvl_uca.h" (was coming from "uca_obj.h")*/
/* 08/02/05  JRB     10    Del unneeded #include "uca_obj.h".		*/
/* 02/08/05  JRB     09    Move all Subscriber params out of		*/
/*			    IEC_GOOSE_SEND_USER_INFO to new		*/
/*			    IEC_GOOSE_RX_USER_INFO.			*/
/*			   Chg Subscriber functions to use ..RX_USER_INFO*/
/*			   Del MAC_TO_GOOSE, move params to ..RX_USER_INFO*/
/*			   Add PUBLISHER_CTRL, SUBSCRIBER_CTRL.		*/
/*			   Add iecGooseSubscribe2.			*/
/*			   Add chk_for_goose_msg proto.			*/
/*			   Del unnecessary prototypes.			*/
/*			   Fix GOOSE_CALLBACK_REASON_* defines so that	*/
/*			    each is a unique single bit value, so	*/
/*			    they can be safely or'd together.		*/
/*			   Del GOOSE_SEND_STRUCT_POOL,GOOSE_SEND_INFO_POOL.*/
/*			   Del unused ETHERNET_HEADER struct.		*/
/*			   delete_sending_goose: chg arg.		*/
/* 11/06/03  JRB     08    Chg retrans_goose return value.		*/
/*			   Add retrans_event_sem global.		*/
/* 09/19/03  JRB     07    Shorten log mask and macro names.		*/
/*			   Use new "Base" macros SLOG_*, SLOGC_*, SLOGH.*/
/* 05/08/03  JRB     06    Del IEC...ALWAYS macros. Use SLOGCALWAYS*.	*/
/* 04/17/03  JRB     05    Use MVL_IND_PEND instead of MVLU_RPT_SCAN_CTRL*/
/* 01/08/03  JRB     04    Del dataReferenceArray fr IEC_GOOSE_SEND_USER_INFO*/
/*			   Del unneeded args from iecGooseSubscribe.	*/
/*			   Make function & flag names more consistent.	*/
/* 12/13/02  ASK     03    Added Ethertype params to user info struct	*/
/* 02/20/01  JRB     02    Fix format.					*/
/* 02/14/02  HSF     01    Created					*/
/************************************************************************/
#include "glbtypes.h"
#include "sysincs.h"
#include "glbsem.h"	/* need ST_EVENT_SEM	*/
#include "stime.h"
#include "goose.h"
#include "mvl_acse.h"
#include "mms_mp.h"
#include "mms_pvar.h"
#include "mvl_defs.h"
#include "mvl_uca.h"
#include "scl.h"
#include <assert.h>
#if 0
#define USE_FRAMEWORK_THREADS	/* define this to enable threads.	*/
#endif

#define MAX_IO_SCAN_TIME  0		/*defines the offset for I/O SCAN Time (msec)	*/
					/*used to offset retransmission and hold times	*/
#define MAX_SENDING_GOOSE 16		/*maximum number of sending  GEESE Supported	*/
#define MAX_RXD_GOOSE		32	/*maximum number of receiving GEESE Supported	*/
#define MAX_IEC_REFERENCE_SIZE 65
#define MAC_ADDR_SIZE 6

/*the following allows the definition of a retransmission curve	*/
/*individual retransmission curves can be associated with each	*/
/*GOOSE being sent						*/
#define MAX_NUM_RETRANS	16
typedef struct retrans_curve
  {
  ST_UINT	num_retrans;			
  /*number of retransmissions in this curve	*/
  ST_INT32 retrans[MAX_NUM_RETRANS];	/*storage of retrans time in msec.*/
  } RETRANS_CURVE;



/*structure that allows tracking of IEC GOOSE as an application extension*/
/*application	*/
typedef struct iec_goose_send_user_info
  {
  /* CRITICAL: First 2 parameters used to add this struct to linked	*/
  /* lists using list_add_last, etc.					*/
  struct iec_goose_send_user_info *next;	/* CRITICAL: DON'T MOVE.*/
  struct iec_goose_send_user_info *prev;	/* CRITICAL: DON'T MOVE.*/

/*additional values for goose_state	*/

#define SD_GOOSE_NOT_CONFIGURED 0
#define SD_GOOSE_CONFIGURED 1
#define SD_GOOSE_TRANSMITTING 2
#define SD_GOOSE_STOPPED 3
#define SD_GOOSE_IDLE 4
#define SD_GOOSE_RETRANSMIT 5
  ST_UINT	goose_state;		/*TRUE, ready to send GOOSE	*/
  ST_INT32	retrans_timeout;	/*timeout period for next goose transmission	*/
  ST_UINT	retrans_curve_offset; 	/*offset to current retrans curve	*/
  RETRANS_CURVE retrans_curve;		/*retransmission curve		*/
  GSE_IEC_CTRL *gptr;			/*pointer to GOOSE Control	*/
  MVL_NVLIST_CTRL *nvl;			/*pointer to Data Set associated with GOOSE*/
  MVL_IND_PEND *scan_ind;		/*pointer to structure that allows for polling*/
  					/*of NVL elements.  Provided as part of	*/
    					/*normal reporting.	*/
  ST_UCHAR dstAddress[MAC_ADDR_SIZE]; 	/*destination MAC address to which GOOSE will be sent	*/
  
  ST_UINT16 etype_tci;			/* Ethertype TCI */
  ST_UINT16 etype_id;			/* Ethertype ID */
  ST_UINT16 etype_appID;		/* Ethertype AppID */
  }IEC_GOOSE_SEND_USER_INFO;

/************************************************************************/
/*			iec_goose_rx_user_info				*/
/* Main structure to control one GOOSE Subscriber.			*/
/************************************************************************/
typedef struct iec_goose_rx_user_info
  {
  /* CRITICAL: First 2 parameters used to add this struct to linked	*/
  /* lists using list_add_last, etc.					*/
  struct iec_goose_rx_user_info *next;	/* CRITICAL: DON'T MOVE.*/
  struct iec_goose_rx_user_info *prev;	/* CRITICAL: DON'T MOVE.*/

  ST_UINT32	num_rxd;
  ST_INT32	retrans_timeout;	/*timeout period for next goose transmission	*/
  GSE_IEC_CTRL *gptr;			/*pointer to GOOSE Control	*/
  
  ST_UINT16 etype_tci;			/* Ethertype TCI */
  ST_UINT16 etype_id;			/* Ethertype ID */
  ST_UINT16 etype_appID;		/* Ethertype AppID */
  
  ST_VOID (*usr_fun)(struct iec_goose_rx_user_info *user_info,
					 GSE_IEC_CTRL *gptr,
					 ST_VOID *usr,
#define GOOSE_CALLBACK_REASON_STATECHANGE_DATA_UPDATED		0x01
#define GOOSE_CALLBACK_REASON_TIMEALLOWED_TO_LIVE_TIMEOUT	0x02
#define GOOSE_CALLBACK_REASON_OUT_OF_SEQUENCE_DETECTED		0x04
#define GOOSE_CALLBACK_REASON_CONFREV_MISMATCH			0x08
#define GOOSE_CALLBACK_REASON_NEED_COMMISSIONING		0x10
#define GOOSE_CALLBACK_REASON_TEST_MODE				0x20
#define GOOSE_CALLBACK_REASON_GCREF_MISMATCH			0x40
#define GOOSE_CALLBACK_REASON_APPID_MISMATCH			0x80
#define GOOSE_CALLBACK_REASON_DATSET_MISMATCH			0x100
					 ST_UINT16 reason);
  ST_VOID *usr;						/*storage for user information to be sent in	*/

  ST_UCHAR dst_mac[MAC_ADDR_SIZE];	/* destination MAC address	*/
  ST_UCHAR src_mac[MAC_ADDR_SIZE];	/* source MAC address		*/
  ST_BOOLEAN decode_needed;		/* SD_TRUE if a GOOSE needs decoding*/
#define GOOSE_DEC_MODE_NOT_SET		0
#define GOOSE_DEC_MODE_LAST_RX		1
#define GOOSE_DEC_MODE_IMMEDIATE	2
  ST_INT decode_mode;			/* decode mode. Use GOOSE_DEC_* defines*/
  ETYPE_INFO etype_info;		/* stores Ethertype information	*/
  ST_CHAR *goose_pdu;			/* pointer to the stored goose pdu*/
  ST_INT goose_pdu_len;			/* length of the stored goose pdu*/
  GSE_IEC_HDR *gse_iec_hdr;		/* decoded GOOSE header		*/
  MVL_NVLIST_CTRL *dsNvl;		/* NVL created for GOOSE Dataset*/
  ST_DOUBLE time_received;		/* time of last GOOSE reception	*/
  }IEC_GOOSE_RX_USER_INFO;

typedef struct rt_type_array
  {
  ST_INT num_rts;			/*number of rts	*/
  RUNTIME_TYPE *rt;			/*pointer to type	*/
  }RT_TYPE_ARRAY;

typedef struct
  {
#ifdef USE_FRAMEWORK_THREADS
  ST_MUTEX_SEM pub_mutex;		/* controls access to pub_list	*/
#endif
  IEC_GOOSE_SEND_USER_INFO *pub_list;	/* linked list of publishers	*/
  } PUBLISHER_CTRL;

typedef struct
  {
#ifdef USE_FRAMEWORK_THREADS
ST_MUTEX_SEM sub_mutex;			/* controls access to sub_list	*/
#endif
  IEC_GOOSE_RX_USER_INFO *sub_list;	/* linked list of subscribers	*/
  } SUBSCRIBER_CTRL;

#define MAX_IEC_GOOSE_ENCODE_BUF 1540	/*maximum size of IEC GOOSE encoding  */

/************************************************************************/
/* Global variables.	*/
/************************************************************************/
extern ST_EVENT_SEM retrans_event_sem;	/* retrans thread event semaphore*/
extern PUBLISHER_CTRL pub_ctrl;	/* ctrl struct for all publishers.	*/
extern SUBSCRIBER_CTRL sub_ctrl;/* ctrl struct for all subscribers.	*/

extern ST_VOID init_retrans_curve(RETRANS_CURVE *retrans_curve);
extern IEC_GOOSE_SEND_USER_INFO *add_sending_goose(GSE_IEC_CTRL *gptr);
ST_RET delete_sending_goose (IEC_GOOSE_SEND_USER_INFO *user_info);

extern OBJECT_NAME *convert_reference_to_mms_obj(ST_CHAR *reference, OBJECT_NAME *obj);
extern ST_VOID sisco_move(ST_CHAR *dest, ST_CHAR *source, ST_INT max_len);



/*Logging information  */

#define IEC_GOOSE_LOG_FLOW 0x00000001
#define IEC_GOOSE_LOG_RX 0x00000002
#define IEC_GOOSE_LOG_TX 0x00000004
#define IEC_GOOSE_LOG_RETRANS 0x00000008

/* Log type strings */
extern SD_CONST ST_CHAR *SD_CONST IEC_GOOSE_LOG_FLOW_TEXT;
extern SD_CONST ST_CHAR *SD_CONST IEC_GOOSE_LOG_RX_TEXT;
extern SD_CONST ST_CHAR *SD_CONST IEC_GOOSE_LOG_TX_TEXT;
extern SD_CONST ST_CHAR *SD_CONST IEC_GOOSE_LOG_RETRANS_TEXT;

extern ST_ULONG iec_goose_usr_debug;

/* FLOW log macros	*/
#define IEC_GOOSE_LOG_FLOW0(a) \
	SLOG_0 (iec_goose_usr_debug & IEC_GOOSE_LOG_FLOW,IEC_GOOSE_LOG_FLOW_TEXT,a)
#define IEC_GOOSE_LOG_FLOW1(a,b) \
	SLOG_1 (iec_goose_usr_debug & IEC_GOOSE_LOG_FLOW,IEC_GOOSE_LOG_FLOW_TEXT,a,b)
#define IEC_GOOSE_LOG_FLOW2(a,b,c) \
	SLOG_2 (iec_goose_usr_debug & IEC_GOOSE_LOG_FLOW,IEC_GOOSE_LOG_FLOW_TEXT,a,b,c)

/* FLOW continuation log macros	*/
#define IEC_GOOSE_LOG_CFLOW0(a) \
	SLOGC_0 (iec_goose_usr_debug & IEC_GOOSE_LOG_FLOW,a)
#define IEC_GOOSE_LOG_CFLOW1(a,b) \
	SLOGC_1 (iec_goose_usr_debug & IEC_GOOSE_LOG_FLOW,a,b)
#define IEC_GOOSE_LOG_CFLOW2(a,b,c) \
	SLOGC_2 (iec_goose_usr_debug & IEC_GOOSE_LOG_FLOW,a,b,c)
#define IEC_GOOSE_LOG_CFLOW3(a,b,c,d) \
	SLOGC_3 (iec_goose_usr_debug & IEC_GOOSE_LOG_FLOW,a,b,c,d)
#define IEC_GOOSE_LOG_CFLOW4(a,b,c,d,e) \
	SLOGC_4 (iec_goose_usr_debug & IEC_GOOSE_LOG_FLOW,a,b,c,d,e)

/* TX log macros	*/
#define IEC_GOOSE_LOG_TX7(a,b,c,d,e,f,g,h) \
	SLOG_7 (iec_goose_usr_debug & IEC_GOOSE_LOG_TX,IEC_GOOSE_LOG_TX_TEXT,a,b,c,d,e,f,g,h)

/* TX continuation log macros	*/
#define IEC_GOOSE_LOG_TXC0(a) \
	SLOGC_0 (iec_goose_usr_debug & IEC_GOOSE_LOG_TX,a)
#define IEC_GOOSE_LOG_TXC1(a,b) \
	SLOGC_1 (iec_goose_usr_debug & IEC_GOOSE_LOG_TX,a,b)
#define IEC_GOOSE_LOG_TXC2(a,b,c) \
	SLOGC_2 (iec_goose_usr_debug & IEC_GOOSE_LOG_TX,a,b,c)
#define IEC_GOOSE_LOG_TXC3(a,b,c,d) \
	SLOGC_3 (iec_goose_usr_debug & IEC_GOOSE_LOG_TX,a,b,c,d)
#define IEC_GOOSE_LOG_TXC4(a,b,c,d,e) \
	SLOGC_4 (iec_goose_usr_debug & IEC_GOOSE_LOG_TX,a,b,c,d,e)

/* TX HEX log macro	*/
#define IEC_GOOSE_LOG_TXH(a,b) \
	SLOGH (iec_goose_usr_debug & IEC_GOOSE_LOG_TX,a,b)

/* RX log macros	*/
#define IEC_GOOSE_LOG_RX0(a) \
	SLOG_0 (iec_goose_usr_debug & IEC_GOOSE_LOG_RX,IEC_GOOSE_LOG_RX_TEXT,a)
#define IEC_GOOSE_LOG_RX1(a,b) \
	SLOG_1 (iec_goose_usr_debug & IEC_GOOSE_LOG_RX,IEC_GOOSE_LOG_RX_TEXT,a,b)
#define IEC_GOOSE_LOG_RX2(a,b,c) \
	SLOG_2 (iec_goose_usr_debug & IEC_GOOSE_LOG_RX,IEC_GOOSE_LOG_RX_TEXT,a,b,c)

/* RX continuation log macros	*/
#define IEC_GOOSE_LOG_RXC0(a) \
	SLOGC_0 (iec_goose_usr_debug & IEC_GOOSE_LOG_RX,a)
#define IEC_GOOSE_LOG_RXC1(a,b) \
	SLOGC_1 (iec_goose_usr_debug & IEC_GOOSE_LOG_RX,a,b)
#define IEC_GOOSE_LOG_RXC2(a,b,c) \
	SLOGC_2 (iec_goose_usr_debug & IEC_GOOSE_LOG_RX,a,b,c)
#define IEC_GOOSE_LOG_RXC3(a,b,c,d) \
	SLOGC_3 (iec_goose_usr_debug & IEC_GOOSE_LOG_RX,a,b,c,d)
#define IEC_GOOSE_LOG_RXC4(a,b,c,d,e) \
	SLOGC_4 (iec_goose_usr_debug & IEC_GOOSE_LOG_RX,a,b,c,d,e)

/* RX HEX log macro	*/
#define IEC_GOOSE_LOG_RXH(a,b) \
	SLOGH (iec_goose_usr_debug & IEC_GOOSE_LOG_RX,a,b)

/* RETRANS log macros	*/
#define IEC_GOOSE_LOG_RETRANS7(a,b,c,d,e,f,g,h) \
	SLOG_7 (iec_goose_usr_debug & IEC_GOOSE_LOG_RETRANS,IEC_GOOSE_LOG_RETRANS_TEXT,a,b,c,d,e,f,g,h)

/* RETRANS continuation log macros	*/
#define IEC_GOOSE_LOG_RETRANSC0(a) \
	SLOGC_0 (iec_goose_usr_debug & IEC_GOOSE_LOG_RETRANS,a)
#define IEC_GOOSE_LOG_RETRANSC1(a,b) \
	SLOGC_1 (iec_goose_usr_debug & IEC_GOOSE_LOG_RETRANS,a,b)
#define IEC_GOOSE_LOG_RETRANSC2(a,b,c) \
	SLOGC_2 (iec_goose_usr_debug & IEC_GOOSE_LOG_RETRANS,a,b,c)
#define IEC_GOOSE_LOG_RETRANSC3(a,b,c,d) \
	SLOGC_3 (iec_goose_usr_debug & IEC_GOOSE_LOG_RETRANS,a,b,c,d)
#define IEC_GOOSE_LOG_RETRANSC4(a,b,c,d,e) \
	SLOGC_4 (iec_goose_usr_debug & IEC_GOOSE_LOG_RETRANS,a,b,c,d,e)

/* RETRANS HEX log macro	*/
#define IEC_GOOSE_LOG_RETRANSH(a,b) \
	SLOGH (iec_goose_usr_debug & IEC_GOOSE_LOG_RETRANS,a,b)

/*********************** functions to be used by the user  */
/*to create the demo framework applicaiton  */
/*from iec_goose_sample_rxd:  */
/* function to subscribe for a IEC GOOSE  */
extern IEC_GOOSE_RX_USER_INFO *iecGooseSubscribe( ST_UCHAR *DstAddress,
			 ST_CHAR *GooseRef,
			 ST_CHAR *DataSetRef,
			 ST_CHAR *AppID,
			 ST_UINT32 ConfRev,		/*initial configuration number expected  */
			 ST_INT numDataEntries,		/*number of entries expecting  */
			 RT_TYPE_ARRAY *rt_array,
			 ST_VOID (*usr_fun)(IEC_GOOSE_RX_USER_INFO *,
					 GSE_IEC_CTRL *gptr,
					 ST_VOID *usr,
					 ST_UINT16 reason),
			 ST_INT	 mode_of_decode);	/*GOOSE_DEC_MODE_..*/

IEC_GOOSE_RX_USER_INFO *iecGooseSubscribe2 (
	ST_UCHAR *DstAddress,
	ST_CHAR *GooseRef,
	ST_CHAR *DataSetRef,
	ST_CHAR *AppID,
	ST_UINT32 ConfRev,	/* initial configuration number expected*/
	ST_INT numDataEntries,	/* number of DataSet entries expected	*/
	ST_INT *typeid_array,
	ST_VOID (*usr_fun)(IEC_GOOSE_RX_USER_INFO *info,
		GSE_IEC_CTRL *gptr,
		ST_VOID *usr,
		ST_UINT16 reason),	/* callback function	*/
	ST_INT mode_of_decode		/* GOOSE_DEC_MODE_..	*/
	);

/* Use SCL info to subscribe for GOOSE. Requires IEC 61850 Edition 2.	*/
ST_RET iecGooseSubscribeExtRefAll (SCL_INFO *scl_info,
	ST_CHAR *iedName,	/* IED name of Subscriber		*/
	ST_CHAR *apName,	/* AccessPoint name of Subscriber	*/
	ST_VOID (*usr_fun)(IEC_GOOSE_RX_USER_INFO *info,
		GSE_IEC_CTRL *gptr,
		ST_VOID *usr,
		ST_UINT16 reason));	/* user callback function pointer*/
IEC_GOOSE_RX_USER_INFO *iecGooseSubscriberFind(ST_CHAR *GoCBRef);

/* function to un-subscribe and free up subscribed resources  */
extern ST_RET iecGooseUnSubscribe( IEC_GOOSE_RX_USER_INFO *goosehandle);
ST_RET iecGooseUnSubscribeAll (ST_VOID);	/* Unsubscribe from all GOOSE*/

/*function to get the GOOSE messages and to process them  */
extern ST_RET get_goose_messages(ST_VOID);

/*function to decode the last GOOSE received for each DST MAC.	*/
/*returns SD_SUCCESS if at least one GOOSE decoded.		*/
ST_RET iecGooseLastRxDecode(ST_VOID);

/*function to check for timed-out GEESE  */
ST_VOID chk_iec_goose_timeout( ST_INT32 elapsed_msec);

ST_RET set_multicast_filters(ST_VOID);
/*from iec_goose_sample_publish  */

/* Function to retransmit GOOSE messages	*/
/* Returns: msec to wait before next retrans.	*/
extern ST_LONG retrans_goose (ST_LONG elapsed_msec);

/*function to create a control block and resources for publishing  */
extern IEC_GOOSE_SEND_USER_INFO *iecGoosePubCreate( ST_UCHAR *DstAddress,
	ST_CHAR *GoCBRef,	/* GOOSE Control Block ObjectReference*/
	ST_CHAR *DatSet,	/* Data Set Reference	*/
	ST_CHAR *GoID,		/* GOOSE Identifier	*/
	ST_UINT32 ConfRev,
	ST_BOOLEAN NdsCom,
	ST_UINT16 tci,
	ST_UINT16 etypeID,
	ST_UINT16 appID,
	RETRANS_CURVE *retrans_curve	/* retransmission curve	*/
	);

/*function to destroy resource created by iecGoosePubCreate  */
ST_RET iecGoosePubDestroy (IEC_GOOSE_SEND_USER_INFO *goosehandle);

/*function to publish a GOOSE.  */
extern ST_VOID iecGoosePublish (IEC_GOOSE_SEND_USER_INFO *goosehandle);

ST_RET chk_for_goose_msg (SN_UNITDATA *sn_req);
IEC_GOOSE_RX_USER_INFO *create_subscription_from_nvl(ST_CHAR *dataSetRef, 
	ST_CHAR *gcRef,
	ST_CHAR *AppID,
	ST_UINT32 ConfRev,
	ST_INT  mode_of_decode,
	ST_UCHAR *mac);

/* User callback function to be called when GOOSE received.		*/
/* Pointer to this function may be passed to "iecGooseSubscribe*".	*/
ST_VOID callback_function(IEC_GOOSE_RX_USER_INFO *goosehandle, 
	GSE_IEC_CTRL *gptr,
	ST_VOID *user,
	ST_UINT16 reason);

