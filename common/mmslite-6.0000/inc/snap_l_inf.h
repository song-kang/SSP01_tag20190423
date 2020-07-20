/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	2005 - 2008, All Rights Reserved				*/
/*									*/
/* MODULE NAME : snap_l_inf.h						*/
/* PRODUCT(S)  : MMS-EASE-142-xxx, MMS-LITE.				*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*	This is the application interface header file for the		*/
/*	Secure Network Access Provider for MMS-LITE Stack (SNAP-Lite).	*/
/*									*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev     Comments					*/
/* --------  ---  ------   -------------------------------------------  */
/* 01/29/08  EJV     02    Implemented event drived IPC with SNAP-Lite.	*/
/* 02/27/07  EJV     01	   Merged rfc1006listenerd and SNAP.		*/
/************************************************************************/
#ifndef SNAP_L_INF_INCLUDED
#define SNAP_L_INF_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "bindsels.h"
#include "tp4.h"		/* for TPDU_CX				*/
#include "tp4_encd.h"		/* for TP0_FHDR_DT_LEN			*/
#include "snap_l_ipc.h"		/* for SNAPL_CON_PARAM 			*/
#include "acse2.h"	/* for MAX_PSEL_LEN, MAX_SSEL_LEN,acse_debug_sel*/
  /* and COPP_LOG_xxx, COSP_LOG_xxx,  asn1_skip_elmnt, xxx_obj_id.	*/

/*----------------------------------------------*/
/* Added from other MMS-LITE FILES - end  	*/
/*----------------------------------------------*/
#include "cosp_usr.h"		/* for COSP_MAX_UDATA_CON, MAX_COSP_COPP_HEADER */
#include "copp.h"		/* for COPP_STATE_AWAIT_CON_RSP		*/


/* IPC states (ipc_state) for passing TCP handle, TPDU-CR, and P-CONNECT to	*/
/* user application (states numerical order is important) 			*/
#define IPC_STATE_IDLE			0	/* no IPC op was done		*/
#define IPC_STATE_PREPARE_IPC_MSG	1	/* IPC op begins		*/
#define IPC_STATE_START_IPC		2	/* IPC op started		*/
#define IPC_STATE_SENT_IPC_MSG		3	/* sent IPC msg 		*/
#define IPC_STATE_SENT_TCP_HANDLE	4	/* sent TCP socket handle	*/
#define IPC_STATE_SENT_CR_TPDU		5	/* sent TPDU-CR			*/
#define IPC_STATE_SENT_PCON_SPDU	6	/* sent P-CONNECT SPDU		*/
#define IPC_STATE_WAIT_IPC_ACK		7	/* wait for IPC ACK msg		*/

typedef struct TCP_CONTROL_tag
  {
  DBL_LNK   	l;			/* add to user's tcp_con_que when P-CON received*/

  /* TCP connectionm info */
  ST_UINT32     uniqueId;		/* unique id given to every TCP connection	*/
  SOCKET        hTcpSock;		/* handle to TCP socket connected		*/
  SOCKADDR_IN   rem_addr;	        /* remote TCP port and IP addr for accepted sock*/
  ST_DOUBLE     con_time;		/* save connect time, limited wait for CR, P_CON*/

  /* Transport info */
  ST_INT        tp_state;	       	/* transport connection state			*/
  ST_UINT       max_tpdu_len;		/* negotiated TPDU len (we accept proposed len)	*/
  ST_UINT       cr_len;			/* CR packet length				*/
  ST_UINT       cr_len_done;            /* # of bytes already sent to user app over IPC	*/
  ST_UCHAR      cr_buf[MAX_TPDU_CR_LEN];/* buf to save TPDU-CR				*/
  TPDU_CX       tpdu_cr;		/* decoded TPDU-CR				*/
  ST_UINT       conn_id;		/* Transport connection ID, idx of tcp_con	*/
  ST_UINT16     loc_ref_offset;		/* used to compute loc_ref (see calc_loc_ref)	*/
  ST_BOOLEAN    sent_DR;		/* if SD_TRUE wait for socket discon by remote	*/

  /* TPDU  buffer */
  ST_UINT       tpkt_len;		/* expected T-DATA packet length		*/
  ST_UINT       tpkt_len_done;		/* number of bytes already received		*/
  ST_UCHAR     *tpkt_buf;		/* ptr to alloc buffer for receiving TPDU	*/
  ST_UCHAR     *enc_buf;		/* ptr to alloc buffer for sending   TPDU	*/
  
  /* Session info */
  ST_INT        cosp_state;		/* session state				*/
  ST_UINT       spdu_len_done;          /* # of bytes already sent to user app over IPC	*/
  SPDU_INFO     spdu_rx;		/* Struct for received SPDU			*/
  ST_UCHAR      ssel[MAX_SSEL_LEN+1];	/* called session selector			*/
  ST_UINT       session_timer;

  /* Presentation info */
  ST_INT        copp_state;		/* presentation state				*/
  ST_UCHAR      psel[MAX_PSEL_LEN+1];	/* called presentation selector			*/
  ST_UINT       ppdu_len;		/* len of a PPDU (also SS-user data len)	*/
  ST_UCHAR     *ppdu_ptr;		/* pointer to start of PPDU encoding		*/

  /* DEBUG: these fields should be copied from the connect decode here */
  ST_CHAR       pciacse;
  ST_CHAR       pcimms;

  /* connection info to pass to user app */
  SNAPL_CON_PARAM con_info;

  /* IPC info (to receive IPC msg with SSL con info on internal TCP connection		*/
  /*           and for passing TCP handle to user application)				*/
  ST_INT        ipc_state;		/* IPC operation state				*/
  SNAPL_IPC_MSG ipc_msg;		/* IPC message to send/receive to/from app	*/
  ST_UINT       ipc_msg_done_cnt;	/* num of bytes sent/received from/to ipc_msg	*/
#if defined(_WIN32)
  WSAPROTOCOL_INFO protocol_info;		/* for passing handle			*/
  ST_UINT          protocol_info_done_cnt;	/* num of bytes sent			*/
#endif
  } TCP_CONTROL;

typedef struct TCP_HEAD_tag
  {
  SOCKET       hLisTcpSock;		/* handle to listen TCP socket			*/
  SOCKET       hLisIntSock;		/* handle to internal listen TCP socket		*/

  /* this is pool of TCP connections waiting for P-CONNECT (before they can be		*/
  /* dispatched to users)								*/
  ST_UINT      tcp_control_cnt;
  TCP_CONTROL *tcp_control_arr;

  /* global transport params */
  ST_UINT      max_tpdu_len;		/* max TPDU len in SNAP-Lite, app len must be <=*/
  ST_UINT      max_con_time;		/* ms, how long to wait for TPDU-CR and P-CONNECT */

  /* global session params */
  ST_UINT      max_spdu_len;		/* max SPDU len, only large enough for S-CONNECT*/
  ST_UCHAR    *cosp_buf;		/* single buffer for COSP msg encoding		*/
  ST_UINT      session_disc_timeout;	/* timeout for session_timer			*/
  } TCP_HEAD;


/* these are default values that can be overwritten in configuration file of by passing	*/
/* parameters to SNAP-Lite 								*/

#define SNAPL_MAX_USERS		64	/* default if not config (or supplied on cmd line)	*/
#define SNAPL_MAX_TCP_CONNS	100	/* This is just number waiting for TP0 CR and P-CONNECT.*/
					/* After that, TCP socket is passed to user app 	*/
					/* (or queued to user).					*/
#define SNAPL_MAX_PIPELINE	10	/* This is number of TCP connections queued to user app	*/

#define SNAPL_MAX_CONN_TIME   (60*1000) /* in ms, this is the total time allowed for TCP connect*/
					/* operation to complete: rcv TPDU-CR, P-CONNECT and	*/
					/* pass the con_info to user (also used to wait for peer*/
					/* to close socket after TPDU-DR was sent)		*/

typedef struct IPC_MSG_QEL_tag
  {
  DBL_LNK   	l;			/* will be queued to user when P-CON received		*/
  SNAPL_IPC_MSG ipc_msg;		/* IPC message to send/receive to/from app		*/
  } IPC_MSG_QEL;

typedef struct USER_CONTROL_tag
  {
  /* hIpcSock=INVALID_SOCKET indicates unused entry */
  SOCKET        hIpcSock;		/* IPC (UNIX domain) socket handle			*/
  BIND_CONTROL *bind_control_arr;	/* SELs binds array, allocated				*/
  ST_UINT       bind_control_arr_cnt;	/* limited to max_user_binds				*/

  ST_BOOLEAN    params_initialized;	/* SD_TRUE if INIT IPC msg was received from user app	*/
  ST_UINT32     max_spdu_len;		/* max SPDU size that app can handle			*/
  ST_UINT32     max_tpdu_len;		/* max TPDU size that app can handle			*/
  ST_UINT32     pid;			/* PID needed for socket duplication on Windows		*/

  ST_UINT       pipe_used;		/* limit to 1 IPC msg or TCP con in transit to/from user*/

  /* TCP connections to be passed to user app (can start when pipe_used=SD_FALSE) 		*/
  ST_UINT       tcp_con_que_cnt;	/* <= max_pipeline					*/
  TCP_CONTROL  *tcp_con_que;		/* TCP connections queued for this user			*/
  TCP_CONTROL  *tcp_con;		/* TCP conn currently processed (in transit to user)	*/

  /* receiving IPC msg */
  ST_DOUBLE     ipc_msg_start_time;	/* start time of IPC operation				*/
  ST_UINT       ipc_msg_done_cnt;	/* num of bytes sent/received from/to ipc_msg		*/
  SNAPL_IPC_MSG ipc_msg;		/* IPC message being send/received to/from user app	*/

  /* responding to IPC msg (can respond when pipe_used=SD_FALSE)				*/
  ST_UINT        ipc_msg_resp_que_cnt;	/* <= max_pipeline					*/
  IPC_MSG_QEL   *ipc_msg_resp_que;	/* IPC response message queue to send to user		*/

  ST_DOUBLE      ipc_msg_resp_start_time; /* start time of IPC operation			*/
  ST_UINT        ipc_msg_resp_done_cnt;	/* num of bytes sent from ipc_msg buffer		*/
  IPC_MSG_QEL   *ipc_msg_resp;		/* IPC response message currently handled		*/
  } USER_CONTROL;

typedef struct USER_HEAD_tag
  {
  SOCKET        hLisIpcSock;		/* UNIX domain listen socket handle			*/			
  ST_UINT       max_user_binds;		/* max binds allowed per user				*/
  ST_UINT	user_control_cnt;	/* num of user processes				*/
  USER_CONTROL *user_control_arr;
  ST_INT        ipcTimeout;		/* ms, max time to recv IPC msg				*/
  ST_UINT       max_pipeline;		/* max num of TCP connections queued to user application*/

  /* Windows only - start */
  ST_UINT16     ipcPort;		/* SNAP-Lite IPC port					*/
  /* Windows only - end   */

  /* Linux, UNIX - start */
  ST_CHAR       domSockFile[S_MAX_PATH];  /* SNAP-Lite UNIX domain socket name for IPC		*/
  /* Linux, UNIX - end */
  } USER_HEAD;


/* GLOBALS: the tcp_head holds general info about TCP connections.				*/
extern TCP_HEAD   *tcp_head;

/* functions in snap_l_inf.c */
USER_CONTROL *find_user_tsel      (ST_UCHAR *tsel);
USER_CONTROL *find_user_tsel_ssel (ST_UCHAR *tsel, ST_UCHAR *ssel);
USER_CONTROL *find_user_sels      (ST_UCHAR *sels, ST_UINT sels_len);
ST_RET        que_con_for_user    (USER_CONTROL *user, TCP_CONTROL *tcp_con);

/* functions in snap_l_tp0.c */
ST_INT  read_rfc1006_tpkt_hdr (TCP_CONTROL *tcp_con);
ST_INT  read_rfc1006_tpkt     (TCP_CONTROL *tcp_con);
ST_INT  drain_tcp_sock        (TCP_CONTROL *tcp_con);
ST_UINT get_max_tpdu_len_enc  (ST_UINT max_tpdu_len);

ST_RET   np_data_IND        (TCP_CONTROL *tcp_con);
ST_RET   tp0_data_REQ       (TCP_CONTROL *tcp_con, ST_INT eot, ST_UINT spdu_len, ST_CHAR *spdu_ptr);
ST_RET   tp0_disconnect_REQ (TCP_CONTROL *tcp_con);
ST_RET   tp0_accept_REQ     (TCP_CONTROL *tcp_con);

ST_RET  _tp0_session_timer_ (TCP_CONTROL *tcp_con, ST_BOOLEAN start);
ST_VOID _tp0_init_timer_    (ST_VOID);
ST_VOID _tp0_check_timer_   (ST_VOID);


/* functions in snap_l_cosp.c */
ST_RET   cosp_u_abort_REQ   (TCP_CONTROL *tcp_con);
ST_RET   cosp_con_RSP_neg   (TCP_CONTROL *tcp_con, ST_INT reason);
ST_RET   tp0_data_IND       (TCP_CONTROL *tcp_con);
ST_RET   tp0_connect_IND    (TCP_CONTROL *tcp_con);
ST_VOID  tp0_disconnect_IND (TCP_CONTROL *tcp_con, ST_INT reason);
ST_VOID _tp0_session_timer_expired_ (TCP_CONTROL *tcp_con);

/* functions in snap_l_copp.c */
ST_RET  u_cosp_con_IND     (TCP_CONTROL *tcp_con);
ST_VOID u_cosp_p_abort_IND (TCP_CONTROL *tcp_con, ST_INT reason);
ST_VOID u_cosp_u_abort_IND (TCP_CONTROL *tcp_con);


#ifdef __cplusplus
}
#endif

#endif /* SNAP_L_INF_INCLUDED */

