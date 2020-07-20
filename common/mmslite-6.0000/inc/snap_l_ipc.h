/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	2005 - 2009 All Rights Reserved					*/
/*									*/
/* MODULE NAME : snap_l_ipc.h						*/
/* PRODUCT(S)  : MMS-EASE-142-xxx, MMS-LITE				*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*	Interface to SNAP-Lite task.					*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev     Comments					*/
/* --------  ---  ------   -------------------------------------------  */
/* 05/12/09  EJV    02     SNAPL_CON_PARAM: chg/added fields.		*/
/* 02/15/07  EJV    01     Moved from tp0_sock.h			*/
/************************************************************************/
#ifndef SNAP_L_IPC_INCLUDED
#define SNAP_L_IPC_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "gensock2.h"		/* for SOCKET define			*/
#include "bindsels.h"		/* for MAX_SELS_BUF_LEN			*/

/************************************************************************/
/* Defines, etc. for interfacing to SNAP-Lite process.			*/
/************************************************************************/

#if !defined(_WIN32)
        /* Linux, UNIX */
        /* use the same task name and the UNIX domain listen socket name to 	*/
        /* easy associate each other when viewing for example network stats	*/
#define SNAPL_IPC_PATH		"/tmp"
#define SNAPL_IPC_DOMSOCK	"snap_ld.domsock"
#endif /* !defined(_WIN32) */


/* RFC1006 protocol defines */ 
#define RFC1006_HEAD_LEN	4	/* RFC1006 header length			*/
#define RFC1006_VERSION		3	/* RFC1006 version				*/
#define RFC1006_BYTE2		0x77	/* RFC1006 2-nd byte (internal SISCO code)	*/
#define RFC1006_PORT		102	/* RFC1006 listen port (same as IPPORT_RFC1006)	*/

#define MAX_TPDU_CR_LEN		128 + RFC1006_HEAD_LEN	/* 128 is the protocol max len for TPDU-CR */

/* SNAP-Lite process state (snapCtx->snapState) */
#define SNAPL_STATE_IDLE		0	/* non-functional			*/
#define SNAPL_STATE_STARTING		1	/* Initializing				*/
#define SNAPL_STATE_RUNNING		2	/* Operational				*/
#define SNAPL_STATE_RUNNING_SECURE	3	/* Operational with Security enabled	*/
#define SNAPL_STATE_TERMINATING		4	/* Terminating				*/

/* timeout in IPC messaging */
#define SNAPL_IPC_TIMEOUT      		1000	/* ms, default	*/

/* defines for 'opcode' field in the SNAPL_IPC_MSG.	*/
/* IMPORTANT: the ACK is OP+1.				*/
#define SNAPL_IPC_OP_UNKNOWN		 0	/* invalid opcode			*/
#define SNAPL_IPC_OP_ANY		 1	/* any type of IPC msg			*/
#define SNAPL_IPC_OP_INIT		 2	/* init connection params		*/
#define SNAPL_IPC_OP_INIT_ACK		 3	/* init connection params ACK		*/
#define SNAPL_IPC_OP_BIND		 4	/* bind SELs				*/
#define SNAPL_IPC_OP_BIND_ACK		 5  	/* bind SELs ACK			*/
#define SNAPL_IPC_OP_UNBIND		 6	/* unbind SELs				*/
#define SNAPL_IPC_OP_UNBIND_ACK		 7	/* unbind SELs ACK			*/
#define SNAPL_IPC_OP_CON_IND		 8	/* TP0 conn indication from rem		*/
#define SNAPL_IPC_OP_CON_IND_ACK	 9	/* TP0 conn indication from rem ACK	*/
#define SNAPL_IPC_OP_CON_IND_SSL	10	/* TP0 conn ind from rem (SSL)		*/
#define SNAPL_IPC_OP_CON_IND_SSL_ACK	11	/* TP0 conn ind from rem (SSL) ACK	*/
#define SNAPL_IPC_OP_CON_REQ_SSL	12	/* TP0 conn request to rem (SSL)	*/
#define SNAPL_IPC_OP_CON_REQ_SSL_ACK	13	/* ACK - not utilized at this time	*/
#define SNAPL_IPC_OP_CON_CNF_SSL	14	/* TP0 conn confirm from rem (SSL)	*/
#define SNAPL_IPC_OP_CON_CNF_SSL_ACK	15	/* ACK - not utilized at this time	*/
#define SNAPL_IPC_OP_SET_LOG_MASKS	16	/* Reread log mask from logcfg.xml file	*/
#define SNAPL_IPC_OP_SET_LOG_MASKS_ACK	17	/* Reread log mask from logcfg.xml file	ACK */
#define SNAPL_IPC_OP_GET_STATE		18	/* SNAP-Lite state check		*/
#define SNAPL_IPC_OP_GET_STATE_ACK	19	/* SNAP-Lite state check ACK		*/
/* UNIX / Linux only */
#define SNAPL_IPC_OP_START		20	/* Start SNAP-Lite daemon, just opcode used	*/
#define SNAPL_IPC_OP_START_ACK		21	/* ACK - not utilized at this time	*/
#define SNAPL_IPC_OP_STOP		22	/* Stop SNAP-Lite daemon		*/
#define SNAPL_IPC_OP_STOP_ACK		23	/* Stop SNAP-Lite daemon ACK		*/


/* response codes in 'resp' in SNAPL_IPC_MSG */
#define SNAPL_IPC_SUCCESS              0        /* same as SD_SUCCESS			*/
#define SNAPL_IPC_ERR_FAILED           1	/* general error (SD_FAILURE)		*/
#define SNAPL_IPC_ERR_UNEXPECTED       2	/* unexpected OP msg received		*/
/* next few (4) codes match with bindsels.h error codes */
#define SNAPL_IPC_ERR_MAX_BINDS        3	/* too many binds			*/
#define SNAPL_IPC_ERR_ALREADY_BOUND    4	/* SELs already bound			*/
#define SNAPL_IPC_ERR_USER_BIND_ID     5	/* User bind id already used in another bind */
#define SNAPL_IPC_ERR_NOT_BOUND        6	/* User bind id not bound		*/
#define SNAPL_IPC_ERR_UNBIND_PENDING   7	/* msg not accepted, unbinding		*/
#define SNAPL_IPC_ERR_PARAM_LEN        8	/* param too long, or not as expected in IPC msg */
#define SNAPL_IPC_ERR_PARAM            9	/* unexpected param in IPC msg		*/
#define SNAPL_IPC_ERR_NOT_INIT        10	/* User params not initialized		*/
#define SNAPL_IPC_ERR_MAX_SPDU_LEN    11	/* app max_spdu_len is too large	*/
#define SNAPL_IPC_ERR_MAX_TPDU_LEN    12	/* app max_tpdu_len is too large	*/
#define SNAPL_IPC_ERR_ONLY_SSL_CON    13	/* accepting only secure cons		*/
#define SNAPL_IPC_ERR_MAGIG_NUM       14	/* invalid Magic Num in IPC msg		*/
#define SNAPL_IPC_ERR_VERSION         15	/* invalid IPC msg version		*/
#define SNAPL_IPC_ERR_STRUCT_SIZE     16	/* incompatible SNAP_IPC_MSG struct size*/
#define SNAPL_IPC_ERR_BUF_NOT_AVAIL   17	/* ipc_msg buf not available temporarily */


/*----------------------------------------------------------------------*/
/*   SNAPL_IPC_MSG struct for SNAP-Lite <--> Application IPC messaging	*/
/*----------------------------------------------------------------------*/
/*									*/
/* The 'param' buffer is used with following messages:			*/
/* SNAPL_IPC_OP_INIT:    	SNAPL_INIT_PARAM			*/
/* SNAPL_IPC_OP_BIND:    	SNAPL_BIND_PARAM			*/
/* SNAPL_IPC_OP_UNBIND:  	SNAPL_UNBIND_PARAM			*/
/* SNAPL_IPC_OP_CON_IND_SSL	SNAPL_CON_PARAM				*/
/* SNAPL_IPC_OP_CON_REQ_SSL	SNAPL_CON_PARAM				*/
/* SNAPL_IPC_OP_CON_CNF_SSL	SNAPL_CON_PARAM				*/
/* SNAPL_IPC_OP_CON_IND_ACK	SNAPL_CON_ACK_PARAM			*/
/* SNAPL_IPC_OP_CON_IND_SSL_ACK	SNAPL_CON_ACK_PARAM			*/
/*									*/
/* NOTE: because of the pack(1) pragma be caryful when adding fields	*/
/*       to this structure. On 64-bit Win Itanium fields in this	*/
/*       struct must be aligned on data type size offset, otherwise	*/
/*       an exception occurs.	For example, a 32-bit field must start	*/
/*       in the structure where its offset is divisible by 4.		*/
/*       To avoid the exception an UNALIGNED macros can be used when	*/
/*       accessing such field.						*/
/*       Avoid adding ST_LONG since on Linux long type is 64-bit on 	*/
/*       64-bit system.							*/
/*----------------------------------------------------------------------*/

#define SNAPL_IPC_MAGIC_NUM	((ST_UINT16) 0xBAD2)

#define SNAPL_IPC_VER200	((ST_UINT16) 200)	/* IPC msg version Mmm	*/
/* Does not has to be changed as long as the message structures did not changed.*/

#define SNAPL_IPC_PARAM_MAX	 128   	/* max len of param in SNAPL_IPC_MSG	*/


  /* Note: because of the pack(1) pragma be caryful when adding fields	*/
  /*       to these structs. On 64-bit Win Itanium fields in this	*/
  /*       struct must be aligned on data type size offset, otherwise	*/
  /*       an exception occurs.	For example, a 32-bit field must start	*/
  /*       in the structure where its offset is divisible by 4.		*/
  /*       To avoid the exception an UNALIGNED macros can be used when	*/
  /*       accessing such field.					*/
  /*       Avoid adding ST_LONG since on Linux long type is 64-bit on 	*/
  /*       64-bit system.						*/

/*______________________________________________________________________________*/
/*										*/
/*			PACKING  START						*/
/*______________________________________________________________________________*/
#if 0 /* NOTE: GNU compiler does not support the pragma pack, use following	*/
  #ifdef __GNUC__
  #define SNAPL_PACKED __attribute__ ((packed))
  #else
  #define SNAPL_PACKED
  #endif
#endif /* 0 */

#if defined(_WIN32)
#pragma pack(push,1)	       		/* padding not desired in the IPC msg	*/
#else
#pragma pack(1)		       		/* padding not desired in the IPC msg	*/
#endif
/*______________________________________________________________________________*/

typedef struct SNAPL_IPC_MSG_tag
  {
  ST_UINT16 magic;			/* always = SNAPL_IPC_MAGIC_NUM		*/
  ST_UINT16 version;			/* always = SNAPL_IPC_MSG_VERMmm	*/
  ST_UINT16 struct_size;		/* always = sizeof(SNAPL_IPC_MSG)	*/
  ST_UINT16 opcode;			/* one of SNAPL_IPC_OP.. values		*/
  ST_UINT16 cr_len;			/* length of TPDU-CR        following this msg	*/
  ST_UINT16 spdu_len;			/* length of P-CONNECT SPDU following this msg	*/
  ST_UINT16 resp;			/* result of reqd oper: SNAPL_IPC_RESP..	*/
  /* this msg includes operation related parameters */
  ST_UINT16 param_len;			/* length of parameter(s) passed in the param	*/
  ST_UCHAR  param[SNAPL_IPC_PARAM_MAX];	/* see above for usage explanation		*/
  } SNAPL_IPC_MSG;


typedef struct SNAPL_INIT_PARAM_tag
  {
  ST_UINT32	pid;			/* relevant onr Windows only 		*/
  ST_UINT32	max_spdu_len;		/* max SPDU len that app can handle	*/
  ST_UINT32	max_tpdu_len;		/* max TPDU len that app can handle	*/
  } SNAPL_INIT_PARAM;


/* Note: use constant size for user_bind_id in the IPC messages to allow	*/
/*       64-bit application to talk to 32-bit applications.			*/
typedef struct SNAPL_BIND_PARAM_tag
  {
  ST_UINT64	user_bind_id;		/* unique for each bind			*/
  ST_UINT32     sels_len;		/* actual length of all SELs		*/
  ST_UCHAR      sels[MAX_SELS_BUF_LEN]; /* combined TSEL, SSEL, PSEL		*/
  } SNAPL_BIND_PARAM;

typedef struct SNAPL_BIND_ACK_PARAM_tag
  {
  ST_UINT64	user_bind_id;		/* unique for each bind			*/
  } SNAPL_BIND_ACK_PARAM;

typedef struct SNAPL_UNBIND_PARAM_tag
  {
  ST_UINT64	user_bind_id;		/* unique for each bind			*/
  } SNAPL_UNBIND_PARAM;

typedef struct SNAPL_UNBIND_ACK_PARAM_tag
  {
  ST_UINT64	user_bind_id;		/* unique for each bind			*/
  } SNAPL_UNBIND_ACK_PARAM;

typedef struct SNAPL_CON_PARAM_tag
  {
  ST_UINT32     uniqueId;		/* unique ID for each TCP connection	*/
  ST_UINT16     secure;			/* SD_TRUE if connection secure  	*/
  /* fields to set for SSL connection */
  ST_UINT16     ipPort;			/* remote SSL conn IP port		*/
  ST_UINT32     ipAddr;			/* remote SSL conn IP Addr		*/
  ST_UINT32     cipherSuiteIdx;		/* in conn ind/cnf			*/
  ST_UINT32     locCfgId;		/* in conn ind/cnf,  certificate cfgId	*/
  ST_UINT32     remCfgId;		/* in conn ind/cnf,  certificate cfgId	*/
  } SNAPL_CON_PARAM;

typedef struct SNAPL_CON_ACK_PARAM_tag
  {
  ST_UINT32     uniqueId;		/* unique ID for each TCP connection	*/
  } SNAPL_CON_ACK_PARAM;


typedef struct SNAPL_GET_STATE_ACK_PARAM_tag
  {
  ST_UINT      snapState;
  } SNAPL_GET_STATE_ACK_PARAM;


/*______________________________________________________________________________*/
/*										*/
/*				PACKING  END					*/
/*______________________________________________________________________________*/
#if defined(_WIN32)
#pragma pack(pop)			/* restore default packing		*/
#else
#pragma pack()				/* restore default packing		*/
#endif
/*______________________________________________________________________________*/



/************************************************************************/
/*  Externals & misc defines						*/
/************************************************************************/

/* macros to get the state, opcode, or respose string for logging	*/
extern ST_CHAR  *snapl_state_str[];
extern ST_UINT   snapl_state_str_cnt;
#define  SNAPL_STATE_STR(state)     ((state < snapl_state_str_cnt) ? snapl_state_str[(ST_UINT) state] : "STATE UNKNOWN")

extern ST_CHAR  *snapl_ipc_op_str[];    /* tbl of opcode strings, used in slogs */
extern ST_UINT   snapl_ipc_op_str_cnt;
#define  SNAPL_IPC_OP_STR(opcode)   ((opcode < snapl_ipc_op_str_cnt) ? snapl_ipc_op_str[(ST_UINT) opcode] : "OPCODE UNKNOWN")

extern ST_CHAR  *snapl_ipc_resp_str[];  /* tbl of response code strings, used in slogs */
extern ST_UINT   snapl_ipc_resp_str_cnt;
#define  SNAPL_IPC_RESP_STR(resp)   ((resp < snapl_ipc_resp_str_cnt) ? snapl_ipc_resp_str[(ST_UINT) resp] : "RESP UNKNOWN")

/* defines to log the socket type used for IPC with SNAP-Lite process	*/
#if defined(_WIN32)
#define		IPCSOCK		"IpcSock"
#else
#define		IPCSOCK		"DomSock"
#endif
#define		TCPSOCK		"TcpSock"


/************************************************************************/
/* Prototypes for functions in the snap_l_ipc.c				*/
/************************************************************************/
#if defined(_WIN32)
ST_RET snapl_ipc_connect (ST_UINT16 ipcPort,    ST_BOOLEAN checkStatus, SOCKET *hSockOut);
#else  /* UNIX, Linux */
ST_RET snapl_ipc_connect (ST_CHAR *domSockFile, ST_BOOLEAN checkStatus, SOCKET *hSockOut);
#endif  /* UNIX, Linux */

ST_RET snapl_ipc_set_msg         (SNAPL_IPC_MSG *ipc_msg,          ST_UINT16 opcode, ST_UINT16 param_len, ST_UCHAR *param);
ST_RET snapl_ipc_set_rfc1006_msg (ST_UCHAR *buf, ST_UINT buf_size, ST_UINT16 opcode, ST_UINT16 param_len, ST_UCHAR *param);

ST_RET snapl_ipc_send_msg        (SOCKET hIpcSock,                   SNAPL_IPC_MSG *ipc_msg, ST_UINT *ipc_msg_done_cnt, ST_INT timeout, ST_DOUBLE *start_time);
ST_RET snapl_ipc_wait_msg        (SOCKET hIpcSock, ST_UINT16 opcode, SNAPL_IPC_MSG *ipc_msg, ST_UINT *ipc_msg_done_cnt, ST_INT timeout, ST_DOUBLE *start_time);
ST_RET snapl_ipc_validate_msg    (SNAPL_IPC_MSG *ipc_msg, ST_UINT16 opcode);


#ifdef __cplusplus
}
#endif

#endif /* !SNAP_L_IPC_INCLUDED */

