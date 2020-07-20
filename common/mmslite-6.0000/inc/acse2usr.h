/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	1995-2009, All Rights Reserved					*/
/*									*/
/* MODULE NAME : acse2usr.h 						*/
/* PRODUCT(S)  :							*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*	ACSE main header file.						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev     Comments					*/
/* --------  ---  ------   -------------------------------------------  */
/* 07/20/10		   Chg MAX_PSEL_LEN, MAX_SSEL_LEN to 16 for ALL apps.*/
/* 06/16/10  JRB	   Del del clpp_init.. clpp_term.. audt_set_msgsize*/
/*			   Del a_get_event_handles*.			*/
/* 11/17/09  JRB    35     Chg all bind_id & conn_id to (ST_VOID *).	*/
/*			   Del REDUCED_STACK, TRIM7, MAP30_ACSE support.*/
/*			   Make MOSI default (don't need MOSI define).	*/
/*			   a_associate_req: delete bind_id arg.		*/
/*			   Del unused u_cla_except callback function.	*/
/*			   Del a_get_chan (needed only with acse3enc.c).*/
/* 03/13/09  JRB    34     Fix return value on a_get_event_handles.	*/
/* 05/05/05  EJV    33	   PRES_ADDR added port.			*/
/* 03/28/05  EJV    32     MMSEASE_MOSE: add MAX_PSEL_LEN, MAX_SSEL_LEN	*/
/* 10/21/03  JRB    31     Add a_get_event_handles_unix.		*/
/* 08/25/03  EJV    30     Del old password auth (OBSOLETE_ACSE_AUTH)	*/
/*			   Added a_get_chan.				*/
/*			   Renamed encrypt_info to encrypt_ctrl		*/
/* 07/23/03  MDE    29     Removed ACSE_AUTH_ENABLED, S_SEC_ENABLED	*/
/* 06/17/03  EJV    28     Added ACSE_LOG_DIB mask.			*/
/* 06/13/03  MDE    27     Added Security				*/
/* 10/14/02  ASK    26     Added abort diagnostic to ABRT_APDU.		*/
/* 09/30/02  ASK    25     Moved authentication defines / protos into   */
/*			   acseauth.h					*/
/* 09/27/02  ASK    24     Add authentication stuff to AARE_APDU	*/
/* 06/20/02  JRB    23     Add calling_paddr to AARQ_APDU.		*/
/*			   Replace "nsap" in PRES_ADDR with union.	*/
/* 05/02/02  MDE    22     Added _copp_bind				*/
/* 02/14/02  ASK    21     Add a_associate_reqm				*/
/* 01/22/02  JRB    20     Add args to copp_initialize (same as MAP30..)*/
/*			   Del a_set_msgsize (no longer needed).	*/
/* 01/10/02  JRB    19     Add cltp_decode_nsdu_2 proto.		*/
/* 12/20/01  JRB    18     Del clpp_bind functions and bind id args to	*/
/*			   a_unit_data_req & u_a_unit_data_ind.		*/
/* 05/10/01  JRB    17     Chg SSEL_LEN from 16 to 4.			*/
/* 12/18/00  EJV    16     Added remAr param to a_associate_req for	*/
/*			   Lite over Marben stack (MAP30_ACSE).		*/
/* 08/05/99  JRB    15     Add a_get_ev... prototype.			*/
/* 07/14/99  JRB    14     Add ACSE Password Authentication stuff.	*/
/* 01/28/99  JRB    13     Move cltp_decode_nsdu proto from TP4 to here.*/
/* 12/04/98  JRB    12     Del "audt_debug_sel".			*/
/*			   Add "loc_mac" to AUDT_APDU.			*/
/* 08/13/98  JRB    11     Lint cleanup.				*/
/* 08/14/97  JRB    10     Added "tp_type" to PRES_ADDR.		*/
/*			   Del MAX_TSEL_LEN & MAX_NSAP_LEN defines (use	*/
/*			   MAX_TSEL_LEN & MAX_IP_ADDR_LEN in tp4api.h).	*/
/* 05/27/97  JRB   7.00    MMSEASE 7.0 release.				*/
/* 04/10/97  JRB    09     Chg error defs to use base from "glberror.h".*/
/* 03/07/97  JRB    08     Add COPP & COSP logmasks for MOSI.		*/
/* 02/20/97  JRB    07     Move *_debug_sel & logmasks from acse2log.h	*/
/*			   to acse2usr.h.				*/
/* 01/09/97  MDE    06     Allow user to specify destination MAC	*/
/* 01/01/97  RKR    05     Added ACSE2USR_INCLUDED and __cplusplus	*/
/* 10/04/96  JRB    04     Add adlcAddr to PRES_ADDR for MCGW.		*/
/* 08/15/96  JRB    03     Add a_buffers_avail prototype for LEAN_T.	*/
/* 04/18/96  JRB    02     Chg a_associate_req return to ST_LONG.		*/
/* 03/26/96  MDE    01     Added header, 'REDUCED_STACK' support       	*/
/************************************************************************/

#ifndef ACSE2USR_INCLUDED
#define ACSE2USR_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "asn1defs.h"
#include "acseauth.h"
#include "ssec.h"
#include "clnp_usr.h"
#include "tp4api.h"	/* Need TP_TYPE_* defines for "tp_type" values.	*/
			/* Need MAX_TSEL_LEN & MAX_IP_ADDR_LEN defines.	*/


/****************************************************************/
/* Log Control							*/
/****************************************************************/
#define ACSE_LOG_ERR		0x00000001
#define ACSE_LOG_ENC		0x00000002
#define ACSE_LOG_DEC		0x00000004
#define ACSE_LOG_DIB		0x00000008

#define COPP_LOG_ERR		0x00000100L
#define COPP_LOG_DEC		0x00001000L
#define COPP_LOG_DEC_HEX	0x00002000L
#define COPP_LOG_ENC		0x00004000L
#define COPP_LOG_ENC_HEX	0x00008000L

#define COSP_LOG_ERR		0x00010000L
#define COSP_LOG_DEC		0x00100000L
#define COSP_LOG_DEC_HEX	0x00200000L
#define COSP_LOG_ENC		0x00400000L
#define COSP_LOG_ENC_HEX	0x00800000L


extern ST_UINT acse_debug_sel;

/************************************************************************/
/* Error codes (returned from a_* functions).				*/
/* NOTE: Values begin from E_ACSE2 (found in glberror.h).		*/
/* NOTE: The exception codes (EX_ACSE_*) do not have separate range.	*/
/************************************************************************/
#define E_ACSE_ENC_ERR		0x3001
#define E_ACSE_SEND_ERR		0x3002
#define E_ACSE_INVALID_CONN_ID	0x3003
#define E_ACSE_INVALID_STATE	0x3004
#define E_ACSE_INVALID_PARAM	0x3005
#define E_ACSE_BUFFER_OVERFLOW	0x3006
#define E_ACSE_MEMORY_ALLOC	0x3007

/************************************************************************/
/* Exception codes (passed to u_a_except or u_cla_except).		*/
/* Caused by decode or state problems.					*/
/************************************************************************/
#define EX_ACSE_DECODE		0x3081
#define EX_ACSE_INVALID_STATE	0x3082

/************************************************************************/
/* Structure definitions.						*/
/************************************************************************/
/* This structure represents the entire Presentation address		*/
/* NOTE: Using Fastbyte Presentation and Session, there are no P-Sels	*/
/*       and S-Sels, so the P-Address is equivalent to the T-Address	*/

/* Define larger PSEL,SSEL for compatibility with existing applications.*/
#define MAX_PSEL_LEN	16	/* International Std Profile recommends 4*/
#define MAX_SSEL_LEN	16	/* GOSIP Ver2 recommends len of 2	*/

/* MAX_TSEL_LEN defined in "tp4api.h"	*/
/* MAX_IP_ADDR_LEN defined in "tp4api.h"	*/
typedef struct tagPRES_ADDR
  {
  ST_UINT psel_len;
  ST_UCHAR psel [MAX_PSEL_LEN];
  ST_UINT ssel_len;
  ST_UCHAR ssel [MAX_SSEL_LEN];
  ST_INT tp_type;	/* Type of transport: TP_TYPE_TP4 or TP_TYPE_TCP.*/
  ST_UINT tsel_len;
  ST_UCHAR tsel [MAX_TSEL_LEN];
  ST_UINT nsap_len;
  union
    {	/* If TP_TYPE_TP4, use "nsap". If TP_TYPE_TCP, use "ip".	*/
    ST_UCHAR nsap [CLNP_MAX_LEN_NSAP];	/* NSAP OR			*/
    ST_ULONG ip;			/* IP Addr (network byte order)	*/
    }netAddr;
  ST_UINT16  port;
  } PRES_ADDR;

/* This structure is used for any data that must be encoded/decoded	*/
/* by the user instead of the ACSE.					*/
/* If len == 0, or ptr == NULL, it is assumed there is no data.		*/
typedef struct tagBUFFER
  {
  ST_INT len;			/* Length of encoded data.		*/
  ST_UCHAR *ptr;		/* Pointer to encoded data.		*/
  } BUFFER;

/* This structure contains AE_TITLE info.				*/
typedef struct tagAE_TITLE
  {
  ST_BOOLEAN AP_title_pres;          	/* present flag                 */
  MMS_OBJ_ID AP_title;          	/* AP title                     */
  ST_BOOLEAN AE_qual_pres;           	/* present flag                 */
  ST_INT32 AE_qual;                 	/* AE qualifier                 */
  ST_BOOLEAN AP_inv_id_pres;         	/* present flag                 */
  ST_INT32 AP_inv_id;       	     	/* AP invocation ID             */
  ST_BOOLEAN AE_inv_id_pres;         	/* present flag                 */
  ST_INT32 AE_inv_id;	            	/* AE invocation ID             */
  } AE_TITLE;

/* This structure is used for the AARQ_APDU.			*/
typedef struct tagAARQ_APDU
  {
  /* The following entries passed to peer in AARQ-apdu.*/
  /* ACSE sets and checks "protocol-version".  Must be "version1".	*/
  ST_BOOLEAN	ASO_context_name_pres;
  MMS_OBJ_ID	ASO_context_name;
  AE_TITLE	called_ae_title;
  AE_TITLE	calling_ae_title;

  /* authentication and security */
  ACSE_AUTH_INFO     auth_info;
  S_SEC_ENCRYPT_CTRL encrypt_ctrl;
  
  BUFFER	user_info;		/* User must encode/decode	*/

  /* The following entries passed to or received from presentation.	*/
  PRES_ADDR	calling_paddr;
  PRES_ADDR	called_paddr;
  } AARQ_APDU;

/* This structure is used for the AUDT_APDU				*/
/* NOTE: same as AARQ_APDU except add calling_paddr.			*/
typedef struct tagAUDT_APDU
  {
  /* The following entries passed to peer in AUDT-apdu.*/
  /* ACSE sets and checks "protocol-version".  Must be "version1".	*/
  ST_BOOLEAN	ASO_context_name_pres;
  MMS_OBJ_ID	ASO_context_name;
  AE_TITLE	called_ae_title;
  AE_TITLE	calling_ae_title;
  BUFFER	user_info;		/* User must encode/decode	*/
  
  /* The following entries passed to or received from presentation.	*/
  PRES_ADDR	calling_paddr;
  PRES_ADDR	called_paddr;

  /* User doesn't need to set loc_mac before calling a_unit_data_req.	*/
  /* Decode process fills in loc_mac before calling u_a_unit_data_ind.	*/
  ST_UCHAR	  loc_mac  [CLNP_MAX_LEN_MAC];	/* Local MAC addr	*/
  ST_BOOLEAN rem_mac_valid;			/* SD_TRUE if MAC addr valid	*/
  ST_UCHAR	  rem_mac  [CLNP_MAX_LEN_MAC];		/* Remote MAC addr		*/
  } AUDT_APDU;


/* This structure is used for the AARE_APDU.				*/
typedef struct tagAARE_APDU
  {
  /* ACSE sets and checks "protocol-version".  Must be "version1".	*/
  ST_BOOLEAN	ASO_context_name_pres;
  MMS_OBJ_ID	ASO_context_name;
  AE_TITLE	responding_ae_title;
  ST_INT16		result;
  ST_BOOLEAN	result_source_diag_pres;
  ST_UINT16	result_source;		/* Passed as asn1_tag to ASN1 functs	*/
  ST_INT16		result_diag;

  /* authentication and security */
  ACSE_AUTH_INFO     auth_info;
  S_SEC_ENCRYPT_CTRL encrypt_ctrl;
  
  BUFFER	user_info;		/* User must encode/decode	*/
  } AARE_APDU;

/* This structure is used for the RLRQ_APDU.			*/
typedef struct tagRLRQ_APDU
  {
  ST_BOOLEAN reason_pres;
  ST_INT16 reason;
  BUFFER user_info;			/* User must encode/decode	*/
  } RLRQ_APDU;

/* This structure is used for the RLRE_APDU.			*/
typedef struct tagRLRE_APDU
  {
  ST_BOOLEAN reason_pres;
  ST_INT16 reason;
  BUFFER user_info;			/* User must encode/decode	*/
  /* NOTE: With Fastbyte presentation and session, there is no way to	*/
  /*       refuse a release, so the "Result" parameter is assumed to be	*/
  /*       "affirmative", and is therefore not needed in this structure.*/
  } RLRE_APDU;

/* This structure is used for the ABRT_APDU.			*/
typedef struct tagABRT_APDU
  {
  ST_INT16 source;		/* Abort source (0=user, 1=provider)		*/
  ST_BOOLEAN   diagnostic_pres;
  ST_ACSE_AUTH diagnostic;  /* Abort diagnostic; codes in acseauth.h */
  BUFFER user_info;			/* User must encode/decode	*/
  } ABRT_APDU;

/* NOTE: the ASO-association-identifier in the ACSE specification is not*/
/* supported in this implementation.  It will not be sent in outgoing	*/
/* APDUs and will be ignored in incoming APDUs.				*/

/************************************************************************/
/* Prototypes								*/
/************************************************************************/
ST_RET copp_initialize (ST_INT max_conns, ST_UINT msg_size);
ST_RET copp_terminate (ST_VOID);
ST_BOOLEAN copp_event (ST_VOID);
ST_RET copp_bind (ST_VOID *user_bind_id, PRES_ADDR *pres_addr, 
	ST_INT sharable, ST_INT max_conns);
ST_VOID u_copp_bind_cnf (ST_VOID *user_bind_id, ST_VOID *copp_bind_id, ST_RET result);
ST_RET copp_unbind (ST_VOID *copp_bind_id);
ST_VOID u_copp_unbind_cnf (ST_VOID *user_bind_id);
ST_VOID *a_associate_req (
	ST_VOID *user_conn_id,	/* USER's ID for this connection	*/
	AARQ_APDU *info);	/* AARQ_APDU info.			*/
ST_VOID u_a_associate_ind (
	ST_VOID *user_bind_id,	/* USER's Bind ID passed to copp_bind	*/
	ST_VOID *acse_conn_id,	/* ACSE's ID for this connection	*/
	AARQ_APDU *info);	/* AARQ_APDU info.			*/
ST_RET a_associate_rsp (
	ST_VOID *acse_conn_id,	/* ACSE's ID for this connection	*/
	ST_VOID *user_conn_id,	/* USER's ID for this connection	*/
	AARE_APDU *info);	/* AARE_APDU info.			*/
ST_VOID u_a_associate_cnf (
	ST_VOID *acse_conn_id,	/* ACSE's ID for this connection	*/
	ST_VOID *user_conn_id,	/* USER's ID for this connection	*/
	AARE_APDU *info);	/* AARE_APDU info.			*/
ST_RET a_release_req (
	ST_VOID *acse_conn_id,	/* ACSE's ID for this connection	*/
	RLRQ_APDU *info);	/* RLRQ_APDU info.			*/
ST_VOID u_a_release_ind (
	ST_VOID *user_conn_id,	/* USER's ID for this connection	*/
	RLRQ_APDU *info);	/* RLRQ_APDU info.			*/
ST_RET a_release_rsp (
	ST_VOID *acse_conn_id,	/* ACSE's ID for this connection	*/
	RLRE_APDU *info);	/* RLRE_APDU info.			*/
ST_VOID u_a_release_cnf (
	ST_VOID *user_conn_id,	/* user's ID for this connection	*/
	RLRE_APDU *info);	/* RLRE_APDU info.			*/
ST_RET a_abort_req (
	ST_VOID *acse_conn_id,	/* ACSE's ID for this connection	*/
	ABRT_APDU *info);	/* ABORT request info.			*/
ST_VOID u_a_abort_ind (
	ST_VOID *user_conn_id,	/* USER's ID for this connection	*/
	ABRT_APDU *info);	/* ABORT request info.			*/
ST_RET a_data_req (
	ST_VOID *acse_conn_id,	/* ACSE's ID for this connection	*/
	ST_UINT data_len,
	ST_UCHAR *data);
ST_VOID u_a_data_ind (
	ST_VOID *user_conn_id,	/* USER's ID for this connection	*/
	ST_UINT data_len,
	ST_UCHAR *data);
ST_INT a_buffers_avail (
	ST_VOID *acse_conn_id);	/* ACSE's ID for this connection	*/
ST_VOID u_a_except (
	ST_VOID *user_conn_id,	/* USER's ID for this connection	*/
	ST_INT code,		/* See EX_ACSE_* defines		*/
	ST_CHAR *filename,		/* Source code file name		*/
	ST_INT line);		/* source code line number		*/

ST_RET a_unit_data_req (
		AUDT_APDU *info);	/* AUDT_APDU info.			*/
ST_VOID u_a_unit_data_ind (
		AUDT_APDU *info);	/* AUDT_APDU info.			*/
ST_BOOLEAN clpp_event (ST_VOID);

ST_VOID cltp_decode_nsdu (N_UNITDATA *n_unitdata);
ST_RET cltp_decode_nsdu_2 (N_UNITDATA *pNudt, AUDT_APDU *pAudt);

/************************************************************************/
/* Address resolution utility functions.				*/
/************************************************************************/
ST_UCHAR *a_get_rem_nsap (ST_VOID *acse_conn_id);
ST_ULONG a_get_rem_ip_addr (ST_VOID *acse_conn_id);

/************************************************************************/
/* Miscellaneous functions						*/
/************************************************************************/

/************************************************************************/
#ifdef __cplusplus
}
#endif

#endif /* ACSE2USR_INCLUDED */
/************************************************************************/

