#ifndef TP4_INCLUDED
#define TP4_INCLUDED
/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	1996-2005 All Rights Reserved					*/
/*									*/
/* MODULE NAME : tp4.h							*/
/* PRODUCT(S)  : Lean-T Stack						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*	TP4 main header file.						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev     Comments					*/
/* --------  ---  ------   -------------------------------------------  */
/* 11/17/09  JRB    18	   Chg all bind_id & conn_id to (ST_VOID *).	*/
/*			   Del MMSEASE_MOSI support.			*/
/*			   Add tp0_process_dt2 for SNAP_LITE.		*/
/* 10/22/08  JRB    17     Add TPDU_ER structure.			*/
/* 03/29/05  EJV    16     MMSEASE_MOSI: added tp0_check_... protos.	*/
/* 01/13/04  EJV    15     Changed tp0_send_... funcs to return value.	*/
/* 06/27/02  JRB    14     Add tp_dr_reason to TP_CONN.			*/
/* 10/25/01  JRB    13     Move replacement func macros to tp4api.h	*/
/* 05/07/01  JRB    12     Add loc_tsap, rem_tsap to TP_CONN, TP0_CONN.	*/
/* 07/19/99  RKR    11     Modified a macro so this file will install	*/
/* 06/08/99  JRB    10     Allow up to 1024 connections by using new	*/
/*			   macros to compute loc_ref_offset & loc_ref.	*/
/* 08/13/98  JRB    09     Lint cleanup.				*/
/* 08/14/97  JRB    08     Use MAX_TSEL_LEN defined in tp4api.h		*/
/*			   Add max_tpdu_len (negotiated) to TP_CONN.	*/
/*			   Add TP0/RFC1006 support defines & protos.	*/
/* 05/27/97  JRB   7.00    MMSEASE 7.0 release.				*/
/* 03/21/97  JRB    07     Add tp_free_spdu_all function.		*/
/* 02/20/97  EJV    06     Add session_timer to TP_CONN for MOSI implem.*/
/* 10/04/96  JRB    05     Add global variable max_tpdu_len_enc.	*/
/*			   Chg TPDU_CX param to max_tpdu_len_enc.	*/
/* 10/04/96  JRB    04     Chg send_ak_flag to ak_delay.		*/
/* 09/09/96  JRB    03     Add send_ak_flag to TP_CONN.			*/
/* 09/09/96  JRB    02     Add tp_send_cr, tp_setup_and_send_??,	*/
/*			   tp_send_dt_and_ak & tp_conn_clean prototypes.*/
/* 07/17/96  JRB    01     Created					*/
/************************************************************************/
#include "tp4api.h"		/* need some TP4 user defs.		*/
#include "clnp_usr.h"		/* TP4 uses CLNP user interface.	*/
#include "tp0_sock.h"		/* TP0/RFC1006 sockets interface defs.	*/

#ifdef __cplusplus
extern "C" {
#endif

#define TP_MAX_CONN_UDATA 32
#define TP4_EXCEPT()	tp4_except (thisFileName, __LINE__)

/************************************************************************/
/* The following defines used in computing "loc_ref" (16-bit value):	*/
/*   TP_CONN_ID_BITS:	number of bits to use for conn_id in loc_ref.	*/
/*   TP_MAX_NUM_CONNS:	max number of connections allowed.		*/
/*   TP_CONN_ID_MASK:	mask used to find conn_id in loc_ref.		*/
/*   TP_LOC_REF_MOD:	modulus to use for loc_ref_offset wrap around.	*/
/*			When loc_ref_offset incremented to this value,	*/
/*			go back to 1 (not 0 so loc_ref can never be 0).	*/
/* TP_CONN_ID_BITS, TP_CONN_ID_MASK, TP_MAX_NUM_CONNS & TP_LOC_REF_MOD	*/
/* are all related. If one changes, they must all change.		*/
/************************************************************************/
#define TP_CONN_ID_BITS		10
#define TP_CONN_ID_MASK		0x3ff	/* 0000 0011 1111 1111 binary	*/
#define TP_MAX_NUM_CONNS	0x400	/* conn_id = 0 thru 0x3ff	*/
/* If 10 bits used for conn_id, 6 bits left for loc_ref_offset, so it	*/
/* must wrap around when it reaches 0100 0000 binary (i.e. 0x40).	*/
#define TP_LOC_REF_MOD		0x40	/* modulus for 6-bit value	*/	

/************************************************************************/
/* Structure typedefs.							*/
/************************************************************************/

/* TPDU_CX - Used for CR or CC TPDU info.	*/
typedef struct
  {
  /* NOTE: the following options not supported:				*/
  /*    Transport Expedited Data.					*/
  /*    Non-use of checksum.						*/
  /*    Use of extended format.						*/
  /* When sending CR, don't request any.				*/
  /* When sending CC, don't accept any.					*/
  
  ST_UCHAR loc_tsap [1+MAX_TSEL_LEN];	/* Local len & TSAP addr	*/
  ST_UCHAR rem_tsap [1+MAX_TSEL_LEN];	/* Remote len & TSAP addr	*/
  ST_UINT16 loc_ref;			/* Local Reference for conn.	*/
  ST_UINT16 rem_ref;			/* Remote Reference for conn.	*/
  ST_UCHAR max_tpdu_len_enc;	/* Negotiated Max TPDU len (encoded).	*/
  ST_UCHAR preferred_class;	/* Preferred Transport Class 0-4	*/
  ST_UCHAR cdt;				/* CDT				*/
  ST_UCHAR *rem_mac;			/* Remote MAC addr		*/
  ST_UCHAR *rem_nsap;			/* Remote len & NSAP addr	*/
  ST_UINT16 udata_len;			/* Length of CR/CC User Data.	*/
  ST_CHAR udata_buf [TP_MAX_CONN_UDATA];	/* Buffer for CR/CC User Data.	*/
#if defined (TP0_ENABLED)
  SOCK_INFO *sock_info;		/* socket info for TP0 conns only	*/
#endif	/* TP0_ENABLED	*/
  } TPDU_CX;

typedef struct
  {
  ST_UINT16 loc_ref;			/* Local Reference for conn.	*/
  ST_UINT16 rem_ref;			/* Remote Reference for conn.	*/
  ST_UCHAR reason;				/* Reason for disconnect.	*/
  ST_UCHAR *rem_mac;			/* Remote MAC addr		*/
  ST_UCHAR *rem_nsap;			/* Remote len & NSAP addr	*/
#if defined (TP0_ENABLED)
  SOCK_INFO *sock_info;		/* socket info for TP0 conns only	*/
#endif	/* TP0_ENABLED	*/
  } TPDU_DR;

typedef TPDU_DR TPDU_DC;	/* Same as DR but ignore "reason".	*/

typedef struct
  {
  ST_UINT16 dst_ref;			/* Dest Reference for conn.	*/
  ST_UCHAR sn;
  ST_UCHAR cdt;
  ST_UCHAR *rem_mac;			/* Remote MAC addr		*/
  ST_UCHAR *rem_nsap;			/* Remote len & NSAP addr	*/
  } TPDU_AK;

typedef struct
  {
  ST_UINT16 dst_ref;			/* Dest Reference for conn.	*/
  ST_UCHAR sn;		/* Sequence #.					*/
  ST_BOOLEAN eot;		/* DT TPDU contains or should contain EOT flag	*/
  ST_UCHAR *rem_mac;			/* Remote MAC addr		*/
  ST_UCHAR *rem_nsap;			/* Remote len & NSAP addr	*/
  ST_CHAR *udata_ptr;	/* Pointer to DT User Data.			*/
  ST_UINT16 udata_len;	/* Length of DT User Data.			*/
#if defined (TP0_ENABLED)
  SOCK_INFO *sock_info;		/* socket info for TP0 conns only	*/
#endif	/* TP0_ENABLED	*/
  } TPDU_DT;

typedef struct
  {
  ST_UINT16 dst_ref;		/* Dest Reference for conn.		*/
  ST_UCHAR reject_cause;	/* reason for error (0-3)		*/
  ST_UCHAR *invalid_tpdu_ptr;	/* Pointer to TPDU hdr that caused reject*/
  ST_UCHAR invalid_tpdu_len;	/* Length of TPDU hdr that caused reject*/
				/* NOTE: <= 255 (must fit in 1 byte).	*/
  ST_UCHAR *rem_mac;		/* Remote MAC addr			*/
  ST_UCHAR *rem_nsap;		/* Remote len & NSAP addr		*/
#if defined (TP0_ENABLED)
  SOCK_INFO *sock_info;		/* socket info for TP0 conns only	*/
#endif	/* TP0_ENABLED	*/
  } TPDU_ER;

typedef struct
  {
  ST_CHAR *spdu_ptr;	/* Pointer to SPDU.			*/
  ST_UINT spdu_len;	/* Length of SPDU.			*/
  ST_UINT offset;	/* Offset into SPDU.			*/
  } SPDU_INFO;

typedef struct
  {
  /* NOTE: "state" is first in this struct because it will be		*/
  /* accessed most often (every time connection made must scan array	*/
  /* for a TP_CONN with state == TP_STATE_CLOSED).			*/
  ST_UCHAR state;		/* State of connection (initially CLOSED).	*/
  ST_UCHAR tp_dr_reason;	/* reason for disconnect (0 if connected)*/
  ST_UINT16 loc_ref_offset;	/* Used to compute loc_ref (see calc_loc_ref)*/
  ST_UINT16 rem_ref;	/* Peer's reference # for this connection.	*/
  ST_UINT16 window_time;	/* Window Time					*/
  ST_UINT16 inact_time;	/* Inactivity Time				*/
  ST_UINT16 retrans_time;	/* Retransmission Time				*/
  ST_UCHAR lower_tx_sn;	/* Sequence # for peer's lower window edge.	*/
  ST_UCHAR adj_rem_cdt;	/* Last CDT received from peer.			*/
  			/* Adjusted to stay within our limits.		*/
  ST_UCHAR next_tx_sn;	/* Sequence # for next TPDU to send to peer.	*/
  ST_UCHAR next_rx_sn;	/* Sequence # for next TPDU to receive from peer*/
  TPDU_DT *tpdu_dt_tx;	/* ptr to array of "max_rem_cdt" structs	*/
  			/* Allocated by tp4_initialize ().	*/
  
  ST_UCHAR num_spdu_outst;	/* # of SPDUs outstanding			*/
  ST_UCHAR spdu_cnt_qued;	/* Count of SPDUs queued. Goes to 0 on overflow	*/
  ST_UCHAR spdu_cnt_sent;	/* Count of SPDUs sent. Goes to 0 on overflow	*/
  ST_UCHAR spdu_cnt_done;	/* Count of SPDUs done. Goes to 0 on overflow	*/
  SPDU_INFO *spdu_tx;	/* ptr to array of "max_spdu_outst" structs	*/
  SPDU_INFO spdu_rx;		/* Struct for one received SPDU		*/
  ST_BOOLEAN spdu_rx_start;	/* Next TPDU rcvd is start of SPDU.	*/
  /* Save rem_mac & rem_nsap received from CLNP with CR or CC.	*/
  ST_UCHAR rem_mac  [CLNP_MAX_LEN_MAC];	/* Remote MAC addr		*/
  ST_UCHAR rem_nsap [1+CLNP_MAX_LEN_NSAP];	/* Remote len & NSAP addr	*/
  ST_UCHAR num_trans;	/* (TX ONLY) # of times a TPDU transmitted	*/
  ST_VOID *user_conn_id;		/* User's ID for this conn.	*/
  TPDU_CX tpdu_cx;		/* CR or CC Info to Send/Receive.	*/
  ST_UCHAR ak_delay;	/* # of loops to delay sending AK.		*/

  ST_UINT16 session_timer;	/* Session timer for MOSI implementation	*/
  ST_UINT16 max_tpdu_len;	/* Negotiated TPDU len on this conn.	*/
  ST_UCHAR loc_tsap [1+MAX_TSEL_LEN];	/* Local len & TSAP addr	*/
  ST_UCHAR rem_tsap [1+MAX_TSEL_LEN];	/* Remote len & TSAP addr	*/
  } TP_CONN;

typedef struct
  {
  /* NOTE: "state" is first in this struct because it will be		*/
  /* accessed most often (every time connection made must scan array	*/
  /* for a TP_CONN with state == TP_STATE_CLOSED).			*/
  ST_UCHAR state;		/* State of connection (initially CLOSED).	*/
  ST_UINT16 loc_ref_offset;	/* Used to compute loc_ref (see calc_loc_ref)*/
  ST_UINT16 rem_ref;	/* Peer's reference # for this connection.	*/
  
  SPDU_INFO spdu_rx;		/* Struct for one received SPDU		*/
  ST_BOOLEAN spdu_rx_start;	/* Next TPDU rcvd is start of SPDU.	*/
  ST_VOID *user_conn_id;		/* User's ID for this conn.	*/
  TPDU_CX tpdu_cx;		/* CR or CC Info to Send/Receive.	*/

  ST_UINT16 session_timer;	/* Session timer for MOSI implementation	*/
  ST_UINT16 max_tpdu_len;	/* Negotiated TPDU len on this conn.	*/
#if defined (TP0_ENABLED)
  SOCK_INFO *sock_info;		/* socket info for TP0 conns only	*/
#endif	/* TP0_ENABLED	*/
  ST_UCHAR loc_tsap [1+MAX_TSEL_LEN];	/* Local len & TSAP addr	*/
  ST_UCHAR rem_tsap [1+MAX_TSEL_LEN];	/* Remote len & TSAP addr	*/
  } TP0_CONN;

/************************************************************************/
/* Miscellaneous Defines.						*/
/************************************************************************/
#define MODULUS_SN	128	/* Sequence # Modulus (2**7)		*/

/************************************************************************/
/* Defines for TP4 connection states.					*/
/************************************************************************/
#define TP_STATE_CLOSED		0
#define TP_STATE_WFCC		1
#define TP_STATE_WBCL		2
#define TP_STATE_OPEN		3
#define TP_STATE_WFTRESP	4
#define TP_STATE_AKWAIT		5
#define TP_STATE_CLOSING	6
#define TP_STATE_WFNC		7	/* for TP0/RFC1006 only		*/

/************************************************************************/
/* Defines for types of TPDUs.						*/
/************************************************************************/
#define	TPDU_TYPE_CR		0
#define	TPDU_TYPE_CC		1
#define	TPDU_TYPE_DR		2
#define	TPDU_TYPE_DC		3
#define	TPDU_TYPE_DT		4
#define	TPDU_TYPE_AK		5

/************************************************************************/
/* Miscellaneous defines.						*/
/************************************************************************/
#define MIN_TP0_CONN_ID		10000	/* min tp_conn_id for TP0 so	*/
					/* don't overlap with TP4 id's	*/

/* WARNING: We assume TP_MAX_NUM_CONNS <= MIN_TP0_CONN_ID so TP0 conn	*/
/*          id's can't overlap with TP4 conn id's.			*/
/*          Let's just make sure this assumption is valid.		*/
#if (TP_MAX_NUM_CONNS > MIN_TP0_CONN_ID)
#error TP_MAX_NUM_CONNS > MIN_TP0_CONN_ID. Definitions incompatible.
#endif

/************************************************************************/
/* Global variables.							*/
/************************************************************************/
extern TP_CONN *tp_conn_arr;	/* ptr to array of "max_num_conns" structs	*/
extern ST_VOID *only_tp4_bind_id;	/* Only one tp4_bind_id.		*/
extern ST_VOID *only_user_bind_id;	/* Only one user_bind_id.		*/
extern ST_UCHAR only_loc_tsap [];	/* Local len & TSAP addr	*/
extern ST_UCHAR max_tpdu_len_enc;	/* Binary encoded MAX TPDU len.		*/

extern TP0_CONN *tp0_conn_arr;	/* ptr to array of "max_num_conns" structs	*/
extern ST_VOID *only_tp0_bind_id;	/* Only one tp0_bind_id.	*/
extern ST_VOID *only_tp0_user_bind_id;	/* Only one tp0_user_bind_id.	*/
extern ST_UCHAR only_tp0_loc_tsap [];	/* Local len & TSAP addr	*/

/************************************************************************/
/* Prototypes								*/
/************************************************************************/
ST_VOID tp_setup_and_send_ak (TP_CONN *tp_conn);
ST_VOID tp_setup_and_send_dr (TP_CONN *tp_conn);

ST_VOID tp_send_cr (TPDU_CX *tpdu_cr);
ST_VOID tp_send_cc (TPDU_CX *tpdu_cc);
ST_VOID tp_send_dr (TPDU_DR *tpdu_dr);
ST_VOID tp_send_dc (TPDU_DC *tpdu_dc);
ST_VOID tp_send_dt (TPDU_DT *tpdu_dt);
ST_VOID tp_send_ak (TPDU_AK *tpdu_ak);
ST_VOID tp_send_dt_and_ak (TPDU_DT *tpdu_dt, TPDU_AK *tpdu_ak);

ST_VOID tp_process_cr (TPDU_CX *tpdu_cr_rx);
ST_VOID tp_process_cc (TPDU_CX *tpdu_cc_rx);
ST_VOID tp_process_dr (TPDU_DR *tpdu_dr_rx);
ST_VOID tp_process_dc (TPDU_DC *tpdu_dc_rx);
ST_VOID tp_process_dt (TPDU_DT *tpdu_dt_rx);
ST_VOID tp_process_ak (TPDU_AK *tpdu_ak_rx);
ST_VOID tp_conn_clean (TP_CONN *tp_conn);
ST_VOID tp_free_spdu_all (TP_CONN *tp_conn);

ST_RET tp0_send_cr (TPDU_CX *tpdu_cr);
ST_RET tp0_send_cc (TPDU_CX *tpdu_cc);
ST_RET tp0_send_dr (TPDU_DR *tpdu_dr);
ST_RET tp0_send_dt (TPDU_DT *tpdu_dt);

ST_VOID tp0_process_cr (TPDU_CX *tpdu_cr_rx);
ST_VOID tp0_process_cc (TPDU_CX *tpdu_cc_rx);
ST_VOID tp0_process_dr (TPDU_DR *tpdu_dr_rx);
ST_VOID tp0_process_dt (TPDU_DT *tpdu_dt_rx);
/* modified function is used if conns are passed from SNAP_LITE	*/
ST_VOID tp0_process_dt2 (SOCK_INFO *sock_info, ST_UCHAR *spdu_ptr, ST_UINT spdu_len);

ST_VOID tp0_conn_clean (TP0_CONN *tp_conn);

/************************************************************************/
/* Macros.								*/
/************************************************************************/

/*----------------------------------------------------------------------*/
/* inc_loc_ref_offset							*/
/* Increment "loc_ref_offset". When the value reaches TP_LOC_REF_MOD	*/
/* (defined above), wrap around back to 1.				*/
/* NOTE: We do not wrap to 0, so "loc_ref_offet" will never be 0.	*/
/*       "loc_ref_offset" is used to compute "loc_ref", so "loc_ref"	*/
/*       will also never be 0.						*/
/*   Arg #1: value (i.e. loc_ref_offset) to be incremented		*/
/* Example of usage:							*/
/*   inc_loc_ref_offset (tp_conn->loc_ref_offset);			*/
/*----------------------------------------------------------------------*/
#define inc_loc_ref_offset(value) {\
          if (++(value) >= TP_LOC_REF_MOD) \
            (value) = 1; } /* comment necessary for DDB installation tool */

/*----------------------------------------------------------------------*/
/*			calc_loc_ref					*/
/* Calculate loc_ref from conn_id and loc_ref_offset.			*/
/* IMPORTANT: loc_ref_offset MUST NEVER be 0, so loc_ref will never be 0.*/
/*           This is accomplished by using ONLY the "inc_loc_ref_offset"*/
/*           macro above to change the value of "loc_ref_offset".	*/ 
/*----------------------------------------------------------------------*/
#define calc_loc_ref(conn_id,loc_ref_offset) \
            ((conn_id) | ((loc_ref_offset) << TP_CONN_ID_BITS))

/*----------------------------------------------------------------------*/
/*			calc_conn_id					*/
/* Calculate conn_id from loc_ref.					*/
/*----------------------------------------------------------------------*/
#define calc_conn_id(loc_ref) \
            ((loc_ref) & TP_CONN_ID_MASK)

#ifdef __cplusplus
}
#endif

#endif	/* !TP4_INCLUDED	*/

