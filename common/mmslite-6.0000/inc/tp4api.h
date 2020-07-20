#ifndef TP4API_INCLUDED
#define TP4API_INCLUDED
/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,       */
/*   1996-2005 All Rights Reserved					*/
/*                                                                      */
/* MODULE NAME : tp4api.h                                             	*/
/* PRODUCT(S)  : TP4 API user include file                          	*/
/*                                                                      */
/* MODULE DESCRIPTION : 						*/
/*                                                                      */
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :                            */
/*                                                                      */
/* MODIFICATION LOG :                                                   */
/*  Date     Who   Rev                  Comments                        */
/* --------  ---  ------   -------------------------------------------  */
/* 07/20/10  JRB	   Chg MAX_TSEL_LEN to 32 for ALL apps.		*/
/* 06/14/10  JRB           Del obsolete cltp_decode_nsdu_fun funct ptr.	*/
/* 09/17/09  JRB    24	   Chg all bind_id & user_conn_id to (ST_VOID *)*/
/*			   (tp?_conn_id unchanged, used as array index).*/
/*			   tp?_connect: pass loc addr instead of bind_id.*/
/*			   Del REDUCED_STACK, MMSEASE_MOSI support.	*/
/* 07/27/05  JRB    23     Del obsolete DS_INFO, tp4_ds_*, odbc_ds_*.	*/
/* 05/05/05  EJV    22	   TP0_CFG: added rfc1006_listen_port		*/
/*			   TP4_ADDR added port.				*/
/* 04/06/05  EJV    21	   MMSEASE_MOSI: use 32 for MAX_TSEL_LEN	*/
/* 09/17/04  JRB    20     Chg tp?_connect to ret ST_LONG (i.e. conn_id)*/
/* 01/14/04  EJV    19	   TP0_CFG: added max_spdu_outst.		*/
/* 08/25/03  EJV    18     Added encrypt_ctrl arg to tp0_connect().	*/
/*			   Added param to tp4_connect macro.		*/
/* 06/20/02  JRB    17     Replace net_addr with union in TP4_ADDR.	*/
/*			   Chg addr args to tp4_connect_ind.		*/
/*			   Add tp0_convert_ip proto.			*/
/* 05/08/02  JRB    16     Add tp0_connect_outstanding_count proto.	*/
/* 01/22/02  JRB    15     Add max_conns arg to tp*_initialize.		*/
/* 05/10/01  JRB    14     Chg TSEL_LEN from 32 to 4.			*/
/* 03/14/01  JRB    13     Add global session_cfg w/ disconnect_timeout.*/
/* 08/05/99  JRB    12     Add hTcp... extern.				*/
/* 06/08/99  JRB    11     Change max_num_conns to ST_UINT16 to allow	*/
/*			   many more connections.			*/
/* 01/28/99  JRB    10     Addec cltp_decode_nsdu_fun funct ptr.	*/
/* 08/13/98  JRB    09     Lint cleanup.				*/
/* 06/18/98  JRB    08     Add TP_TYPE_TPX.				*/
/* 03/11/98  JRB    07     Del max_spdu_len from TP0_CFG & TP_CFG.	*/
/*			   Add keepalive to TP0_CFG.			*/
/* 09/04/07  JRB    06     Add max_spdu_len arg to tp4_initialize.	*/
/* 08/14/97  JRB    05     RFC1006 changes:				*/
/*			   - Add "tp0_*" protos.			*/
/*			   - Add "tp_type" to TP4_ADDR.			*/
/*			   - Add "tp0_cfg" config struct for RFC1006.	*/
/*			   - Chg "tp4_data" arg "data_len" to ST_UINT.	*/
/*			   - Replace tp4_reldata proto with dummy macro.*/
/*			   - Chg MAX_TP4_SEL to MAX_TSEL_LEN (ACSE uses)*/
/*			   - Chg MAX_NET_ADDR to MAX_IP_ADDR_LEN and	*/
/*			     increase to allow IP hostname.		*/ 
/* 08/04/97  JRB    04     Add max_tpdu_len_enc to TP_CFG (use instead	*/
/*			   of global max_tpdu_len_enc).			*/
/* 05/27/97  JRB   7.00    MMSEASE 7.0 release.				*/
/* 05/07/97  JRB    03     Added Error codes.				*/
/* 02/20/97  EJV    02     Added MOSI support: tp4_session_timer and	*/
/*			   tp4_session_timer_expired.			*/
/* 11/14/96  JRB    01     Created for LEAN_T only.			*/
/************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#include "ssec.h"		/* for S_SEC_ENCRYPT_CTRL		*/
#include "clnp_usr.h"		/* Need CLNP_MAX_LEN_NSAP define.	*/

/* Defines for Transport type. Used to set "tp_type" in TP4_ADDR.	*/
#define TP_TYPE_TCP	0
#define TP_TYPE_TP4	1
#define TP_TYPE_TPX	2	/* allow TP4 and TCP at same time.	*/

#define LOCAL_IP_ADDRESS	"127.0.0.1"
/****************************************************************/
/*	Structure definitions					*/
/****************************************************************/

/* Define larger TSEL for compatibility with existing applications.	*/
#define MAX_TSEL_LEN	32	/* GOSIP Ver2 recommends len of 2	*/ 

#define MAX_IP_ADDR_LEN	80	/* Big enough for IP hostname	*/

typedef struct tagTP4_ADDR
  {
  ST_INT tp_type;	/* Type of transport: TP_TYPE_TP4 or TP_TYPE_TCP.*/
  ST_UINT tp4_sel_len;
  char tp4_sel[MAX_TSEL_LEN];
  ST_UINT net_addr_len;
  union
    {	/* If TP_TYPE_TP4, use "nsap". If TP_TYPE_TCP, use "ip".	*/
    ST_UCHAR nsap [CLNP_MAX_LEN_NSAP];	/* NSAP OR			*/
    ST_ULONG ip;			/* IP Addr (network byte order)	*/
    }netAddr;
  ST_UINT16 port;			/* remote port, optional param,	*/
  					/* defaults to 102 if not config*/
  } TP4_ADDR;

/****************************************************************/
/*	API prototypes						*/
/****************************************************************/

ST_RET tp4_initialize (ST_UINT max_conns, ST_UINT max_spdu_len);
ST_RET tp4_terminate (ST_VOID);

ST_BOOLEAN tp4_event (ST_VOID);

ST_RET tp4_bind (ST_VOID *user_bind_id, TP4_ADDR *tp4_addr, 
		  ST_INT sharable, ST_INT max_conns);
ST_RET tp4_unbind (ST_VOID *tp4_bind_id);

ST_LONG tp4_connect (ST_VOID *user_conn_id,
		     TP4_ADDR *loc_tp4_addr,
		     TP4_ADDR *rem_tp4_addr,
		     ST_INT conndata_len,
		     char *conndata,
                     S_SEC_ENCRYPT_CTRL *encrypt_ctrl);	/* connection encryption info	*/ 

ST_RET tp4_accept (ST_LONG tp4_conn_id, ST_VOID *user_conn_id,
		    ST_INT conndata_len,
		    char *conndata);
ST_RET tp4_disconnect (ST_LONG tp4_conn_id,
		        ST_INT conndata_len,
		        char *conndata);
ST_RET tp4_data (ST_LONG tp4_conn_id,
		  ST_INT eot,
		  ST_UINT data_len,
		  char *data);
ST_RET tp4_expdata (ST_LONG tp4_conn_id,
		     ST_UINT data_len,
		     char *data);

/* The "tp4_reldata" function is not needed in this TP4 implementation,	*/
/* so define "tp4_reldata" macro which does nothing.			*/
#define tp4_reldata(databuf)

/* Function for MOSI implementation:	*/
ST_RET tp4_session_timer (ST_LONG tp_conn_id, ST_BOOLEAN start);

ST_RET tp0_initialize (ST_UINT max_conns, ST_UINT max_spdu_len);
ST_RET tp0_terminate (ST_VOID);

ST_BOOLEAN tp0_event (ST_VOID);

ST_RET tp0_bind (ST_VOID *user_bind_id, TP4_ADDR *tp4_addr, 
		  ST_INT sharable, ST_INT max_conns);
ST_RET tp0_unbind (ST_VOID *tp4_bind_id);

ST_LONG tp0_connect (ST_VOID *user_conn_id,
		     TP4_ADDR *loc_tp4_addr,
		     TP4_ADDR *rem_tp4_addr,
		     ST_INT conndata_len,
		     char *conndata,
                     S_SEC_ENCRYPT_CTRL *encrypt_ctrl);
ST_INT tp0_connect_outstanding_count (TP4_ADDR *tp4Addr);
ST_RET tp0_accept (ST_LONG tp4_conn_id, ST_VOID *user_conn_id,
		    ST_INT conndata_len,
		    char *conndata);
ST_RET tp0_disconnect (ST_LONG tp4_conn_id,
		        ST_INT conndata_len,
		        char *conndata);
ST_RET tp0_data (ST_LONG tp4_conn_id,
		  ST_INT eot,
		  ST_UINT data_len,
		  char *data);

/* Function for MOSI implementation:	*/
ST_RET tp0_session_timer (ST_LONG tp_conn_id, ST_BOOLEAN start);

/****************************************************************/
/* Event callback function prototypes				*/
/****************************************************************/

ST_VOID tp4_bind_cnf (ST_VOID *user_bind_id, ST_VOID *tp4_bind_id, ST_RET result);
ST_VOID tp4_unbind_cnf (ST_VOID *user_bind_id);

ST_VOID tp4_connect_ind (ST_VOID *user_bind_id, ST_LONG tp4_conn_id, 
                      ST_UCHAR *loc_tsap, ST_UCHAR *rem_tsap,
                      ST_UCHAR *rem_nsap, ST_ULONG rem_ip,
                      ST_INT conndata_len, char *conndata);
ST_VOID tp4_connect_cnf (ST_VOID *user_conn_id, ST_LONG tp4_conn_id, 
                      ST_INT conndata_len, char *conndata);
ST_VOID tp4_disconnect_ind (ST_VOID *user_conn_id, ST_INT reason,
                         ST_INT conndata_len, char *conndata);
ST_VOID tp4_data_ind (ST_VOID *user_conn_id, ST_INT eot, 
		   ST_UINT data_len, char *data);
ST_VOID tp4_expdata_ind (ST_VOID *user_conn_id,
		      ST_UINT data_len, char *data);

/* Function for MOSI implementation:	*/
ST_VOID tp4_session_timer_expired (ST_VOID *user_conn_id);

			 
/****************************************************************/
/* Log Control							*/
/****************************************************************/

#define TP4_LOG_ERR		0x00000001
#define TP4_LOG_FLOWUP		0x00000002
#define TP4_LOG_FLOWDOWN	0x00000004

extern ST_UINT tp4_debug_sel;

/****************************************************************/
/* Error Codes							*/
/****************************************************************/

#define TP4E_SHMALLOC		0x1201
#define TP4E_BADCONN		0x1202
#define TP4E_QUEUE_FULL		0x1203
#define TP4E_CONN_STATE		0x1204
#define TP4E_INVAL_TPDU_LEN	0x1205
#define TP4E_INVAL_REM_CDT	0x1206
#define TP4E_INVAL_LOC_CDT	0x1207
#define TP4E_INVAL_SPDU_OUTST	0x1208
#define TP4E_INVAL_NUM_CONNS	0x1209
#define TP4E_INVAL_SPDU_LEN	0x120A
#define TP4E_INVAL_WINDOW_TIME	0x120B
#define TP4E_INVAL_INACT_TIME	0x120C
#define TP4E_INVAL_RETRANS_TIME	0x120D
#define TP4E_INVAL_MAX_TRANS	0x120E
#define TP4E_MALLOC		0x120F
#define TP4E_INVAL_TSEL		0x1210
#define TP4E_TOO_MANY_BINDS	0x1211


/************************************************************************/
/* Global Configuration Structure Typedef.				*/
/************************************************************************/
typedef struct
  {
  ST_UINT16 max_tpdu_len;	/* max len of TPDU.  Base on SNPDU size.*/
			/* Use to allocate TPDU buffers.	*/
  ST_UCHAR max_tpdu_len_enc;	/* Binary encoded MAX TPDU len. Computed*/
				/* from max_tpdu_len by tp4_initialize.	*/
  ST_UCHAR max_rem_cdt;	/* Max credits we can handle.		*/
			/* Will allocate this many TPDU_DT	*/
			/* structs.				*/
			/* CRITICAL: MUST BE POWER OF 2.	*/
  ST_UCHAR loc_cdt;	/* CDT value we will ALWAYS send in ACK	*/
			/* We only accept in-sequence TPDUs so	*/
			/* only purpose of this is to		*/
			/* allow peer to send ahead.		*/
  ST_UCHAR max_spdu_outst;	/* Max # of SPDUs outstanding per conn.	*/
			/* Will allocate this many SPDU_INFO	*/
			/* structs for transmit queue.		*/
			/* CRITICAL: MUST BE POWER OF 2.	*/
  ST_UINT16 max_num_conns;	/* Max # Connections			*/
  ST_UINT16 window_time;	/* Window Time				*/
  ST_UINT16 inact_time;	/* Inactivity Time			*/
  ST_UINT16 retrans_time;	/* Retransmission Time			*/
  ST_UCHAR max_trans;	/* Max # of transmissions of a TPDU	*/
  ST_UCHAR ak_delay;	/* # of loops to delay sending AK.	*/
  } TP_CFG;

typedef struct
  {
  ST_UINT16 max_tpdu_len;	/* max len of TPDU.  Base on SNPDU size.*/
			/* Use to allocate TPDU buffers.	*/
  ST_UCHAR max_tpdu_len_enc;	/* Binary encoded MAX TPDU len. Computed*/
				/* from max_tpdu_len by tp0_initialize.	*/
  ST_UINT  max_spdu_outst;	/* Max # of SPDUs outstanding per conn.	*/
				/* that can be queued on the gensock2	*/
  ST_UINT16 max_num_conns;	/* Max # Connections			*/
  ST_BOOLEAN keepalive;		/* Use KEEPALIVE option on Sockets?	*/
  ST_UINT16 rfc1006_listen_port;/* RFC1006 listen port, optional param,	*/
  				/* will default to 102 if not configured*/
  } TP0_CFG;			/* For TP0/RFC1006 only.		*/

typedef struct
  {
  ST_UINT16 disconnect_timeout;	/* time to wait for T-disconnect.	*/
  } SESSION_CFG;

/************************************************************************/
/* Global Configuration Structure					*/
/* This structure must be filled in by user BEFORE tp4_initialize ().	*/
/* It MUST NOT change after tp4_initialize ().				*/
/************************************************************************/
extern TP_CFG tp_cfg;
extern TP0_CFG tp0_cfg;		/* For TP0/RFC1006 only.		*/
extern SESSION_CFG session_cfg;
  
/************************************************************************/
/* Global Variables							*/
/************************************************************************/

/************************************************************************/
/* Additional Prototypes for LEAN_T only.				*/
/************************************************************************/

ST_VOID tp4_except (ST_CHAR *filename, ST_INT line);
ST_VOID tp4_timer_tick (ST_VOID);
ST_RET tp4_config (ST_VOID);
ST_VOID tp4_init_timer (ST_VOID);
ST_VOID tp4_check_timer (ST_VOID);
ST_INT tp4_buffers_avail (ST_LONG tp4_conn_id);

ST_VOID tp0_timer_tick (ST_VOID);
ST_INT tp0_buffers_avail (ST_LONG tp4_conn_id);
ST_ULONG tp0_convert_ip (ST_CHAR *hostname);

/************************************************************************/
/* Event semaphore to indicate TCP events.				*/
/************************************************************************/
extern ST_EVENT_SEM hTcpEvent;

/************************************************************************/
/* Replacement function macros. If no TP4, call TP0 functions.		*/
/************************************************************************/
#if !defined (TP4_ENABLED)
#define tp4_initialize(a,b)	tp0_initialize(a,b)
#define tp4_terminate()		tp0_terminate()
#define tp4_event()		tp0_event()
#define tp4_bind(a,b,c,d)	tp0_bind(a,b,c,d)
#define tp4_unbind(a)		tp0_unbind(a)
#define tp4_connect(a,b,c,d,e,f) tp0_connect(a,b,c,d,e,f)
#define tp4_accept(a,b,c,d)	tp0_accept(a,b,c,d)
#define tp4_disconnect(a,b,c)	tp0_disconnect(a,b,c)
#define tp4_data(a,b,c,d)	tp0_data(a,b,c,d)
#define tp4_session_timer(a,b)	tp0_session_timer(a,b)
#define tp4_timer_tick()	tp0_timer_tick()
#define tp4_buffers_avail(a)	tp0_buffers_avail(a)
#endif	/* !defined (TP4_ENABLED)	*/

#ifdef __cplusplus
}
#endif
#endif	/* !TP4API_INCLUDED	*/
