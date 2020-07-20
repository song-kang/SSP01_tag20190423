/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	1995 - 2010, All Rights Reserved				*/
/*									*/
/* MODULE NAME : acse2.h						*/
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
/* 07/19/10  EJV	   ACSE_CONN: added auth_info.			*/
/* 11/17/09  JRB    17     Add new ACSE decode functions (acse_dec_*).	*/
/*			   Chg all bind_id & conn_id to (ST_VOID *).	*/
/*			   Del REDUCED_STACK, TRIM7, MAP30_ACSE support.*/
/*			   Make MOSI default (don't need MOSI define).	*/
/*			   Add dib_match_remote_ar_ll.			*/
/*			   Deleted obsolete stuff.			*/
/*			   Use ST_INT8 for all pci values.		*/
/* 05/27/04  EJV    16     ACSE_CONN (MAP30_ACSE): added initiator.	*/
/* 08/25/03  EJV    15     Del old password auth (OBSOLETE_ACSE_AUTH)	*/
/*			   ACSE_CONN: added part_auth_info, encrypt_ctrl*/
/*			   Added security functions protos.		*/
/* 08/01/03  JRB    14     Add lean_a.h for DIB_ENTRY.			*/
/* 06/17/03  EJV    13     Moved DIB_MATCH... from m_match.h.		*/
/* 10/14/02  ASK    12     Added abort diagnostic tag			*/
/* 09/27/02  ASK    11	   Added ASN.1 security tags to AARE APDU, moved*/
/*			   bitstring defines from acse2dec.c		*/
/* 06/20/02  JRB    10     Add TSELs & remote NSAP/IP to ACSE_CONN.	*/
/* 12/11/01  JRB    09     Convert to use ASN1R.			*/
/* 09/10/01  JRB    08     Put back cosp_cn_ptr, cosp_cn_len.		*/
/* 08/01/01  JRB    07     Del cosp_cn_ptr, cosp_cn_len from ACSE_CONN.	*/
/* 04/03/01  JRB    06     Add PSELs & SSELs to ACSE_CONN.		*/
/* 01/05/01  EJV    05     Changes for use over Marben			*/
/* 07/14/99  JRB    04     Add ACSE Password Authentication stuff.	*/
/* 08/13/98  JRB    03     Lint cleanup.				*/
/* 07/23/98  JRB    02     Delete Marben support.			*/
/* 11/10/97  JRB    01     Chg cosp_buf to (ST_UCHAR *).		*/
/* 05/27/97  JRB   7.00    MMSEASE 7.0 release.				*/
/* 03/19/97  JRB    08     MOSI cleanup.				*/
/* 03/07/97  JRB    07     Added MOSI support.				*/
/* 01/02/97  EJV    06     Added ACSE2_INCLUDED and __cplusplus.	*/
/* 06/25/96  JRB    05     Added 'LEAN_T' support.			*/
/* 05/02/96  JRB    04     Add STATE_TP4CONN_WAIT.			*/
/* 04/18/96  JRB    03     Add STATE_AARE_ABANDON.			*/
/* 03/26/96  MDE    02     Added 'REDUCED_STACK' support 		*/
/* 10/30/95  JRB    01     Created					*/
/************************************************************************/

#ifndef ACSE2_INCLUDED
#define ACSE2_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "gen_list.h"
#include "acse2log.h"			/* slog macros for ACSE		*/
#include "acse2usr.h"
#include "lean_a.h"			/* need DIB_ENTRY		*/
#include "dibmatch.h"

/* Defines for bits in bitstring encoded values.			*/
#define BIT_VERSION1		0x80	/* AARQ, AARE protocol_version	*/
#define BIT_AUTHENTICATION	0x80	/* AARQ, AARE acse_requirements	*/

/* ACPM states.								*/
#define STATE_INVALID		-1	/* So user cannot use freed struct*/
#define STATE_IDLE		0
#define STATE_AARE_WAIT		1
#define STATE_AASCRSP_WAIT	2
#define STATE_RLRE_WAIT		3
#define STATE_ARLSRSP_WAIT	4
#define STATE_ASSOCIATED	5
#define STATE_INIT_COLL		6
#define STATE_RESP_COLL		7
#define STATE_AARE_ABANDON	8
#define STATE_TP4CONN_WAIT	9

/* ASN.1 tags for ACSE APDUs						*/
#define TAG_AARQ		0
#define TAG_AARE		1
#define TAG_RLRQ		2
#define TAG_RLRE		3
#define TAG_ABRT		4
#define TAG_ADT			5
/* ASN.1 tags for CONNECTIONLESS ACSE APDUs				*/
#define TAG_AUDT		0

/* ASN.1 tags for elements of AARQ APDU					*/
#define AARQ_protocol_version		0
#define AARQ_ASO_context_name		1
#define AARQ_CALLED_AP_title		2
#define AARQ_CALLED_AE_qual		3
#define AARQ_CALLED_AP_inv_id		4
#define AARQ_CALLED_AE_inv_id		5
#define AARQ_CALLING_AP_title		6
#define AARQ_CALLING_AE_qual		7
#define AARQ_CALLING_AP_inv_id		8
#define AARQ_CALLING_AE_inv_id		9
#define AARQ_acse_requirements		10
#define AARQ_mechanism_name		11
#define AARQ_CALLING_auth_value		12
#define AARQ_user_info			30

/* ASN.1 tags for elements of AUDT APDU (same as AARQ)			*/
#define AUDT_protocol_version		AARQ_protocol_version	
#define AUDT_ASO_context_name		AARQ_ASO_context_name	
#define AUDT_CALLED_AP_title		AARQ_CALLED_AP_title	
#define AUDT_CALLED_AE_qual		AARQ_CALLED_AE_qual	
#define AUDT_CALLED_AP_inv_id		AARQ_CALLED_AP_inv_id	
#define AUDT_CALLED_AE_inv_id		AARQ_CALLED_AE_inv_id	
#define AUDT_CALLING_AP_title		AARQ_CALLING_AP_title	
#define AUDT_CALLING_AE_qual		AARQ_CALLING_AE_qual	
#define AUDT_CALLING_AP_inv_id		AARQ_CALLING_AP_inv_id
#define AUDT_CALLING_AE_inv_id		AARQ_CALLING_AE_inv_id
#define AUDT_user_info			AARQ_user_info		

/* ASN.1 tags for elements of AARE APDU					*/
#define AARE_protocol_version		0
#define AARE_ASO_context_name		1
#define AARE_result			2
#define AARE_result_source_diagnostic	3
#define AARE_RESPONDING_AP_title	4
#define AARE_RESPONDING_AE_qual		5
#define AARE_RESPONDING_AP_inv_id	6
#define AARE_RESPONDING_AE_inv_id	7
#define AARE_RESPONDER_acse_requirements 8 /* See the ACSE Spec for the strange name */
#define AARE_mechanism_name		 9
#define AARE_RESPONDING_auth_value 	 10
#define AARE_user_info			30

/* ASN.1 tags for elements of RLRQ APDU					*/
#define RLRQ_reason			0
#define RLRQ_user_info			30

/* ASN.1 tags for elements of RLRE APDU					*/
#define RLRE_reason			0
#define RLRE_user_info			30

/* ASN.1 tags for elements of ABRT APDU					*/
#define ABRT_source			0
#define ABRT_diagnostic			1
#define ABRT_user_info			30

/* ASN.1 tags for elements of ADT APDU					*/
#define ADT_user_info			30

#define ACSE_SERVICE_USER		0
#define ACSE_SERVICE_PROVIDER		1

/************************************************************************/
/*			ACSE_CONN structure				*/
/************************************************************************/
#define MAX_COSP_COPP_HEADER	(((MAX_PSEL_LEN+MAX_SSEL_LEN)*2)+200)
typedef struct tagACSE_CONN
  {
  /* common part */
  ST_UINT	apdu_len;	/* len of a APDU (also PS-user data len)*/
  ST_UCHAR	       *apdu_ptr;	/* pointer to start of APDU encoding	*/
  ST_UINT	ppdu_len;	/* len of a PPDU (also SS-user data len)*/
  ST_UCHAR	       *ppdu_ptr;	/* pointer to start of PPDU encoding	*/
  ST_INT8 pcimms;
  ST_INT8 pciacse;
  /* ACSE part */
  ST_VOID *user_conn_id;	/* ACSE-user's connection ID.		*/
  ST_INT state;		/* See "STATE_*" defines	*/
  ST_BOOLEAN initiator;	/* Flag: did we initiate this connection?	*/
  /* COPP part */
  ST_INT num_ctxt;
  ST_INT	copp_state;	/* See "COPP_STATE_*" defines		*/
  /* cosp part */
  ST_INT	cosp_state;	/* See "COSP_CSTATE_*" defines		*/
  ST_UINT      cosp_cn_len;
  char	       *cosp_cn_ptr;	/* CONNECT SPDU saved until TCONN.cnf	*/
  ST_VOID *cosp_bind_id;	/* COSP bind id saved until CN arrive	*/
  ST_BOOLEAN       cosp_vtca;      /* SD_TRUE if transport connection acceptor*/
  ST_BOOLEAN       cosp_vcoll;     /* SD_TRUE if collision of FINISH SPDUs	*/
  ST_BOOLEAN       cosp_vdnr;      /* SD_TRUE if DISCONNECT SPDU has been	*/
                                /*      received in STA 09 (following	*/
                                /*      collision of FINISH SPDUs).	*/
  /* TP4 part */
  ST_LONG		tp4_conn_id;	/* TP4 connect id			*/
  ST_BOOLEAN	cr;		/* "collision of release" flag		*/
  ST_BOOLEAN	rl;		/* "release started" flag		*/
  BUFFER save_buf;	/* buffer to save AARQ or RLRE to send later	*/
  ST_UCHAR loc_psel [MAX_PSEL_LEN+1];	/* Local PSEL (1st byte = len)	*/
  ST_UCHAR rem_psel [MAX_PSEL_LEN+1];	/* Remote PSEL (1st byte = len)	*/
  ST_UCHAR loc_ssel [MAX_SSEL_LEN+1];	/* Local SSEL (1st byte = len)	*/
  ST_UCHAR rem_ssel [MAX_SSEL_LEN+1];	/* Remote SSEL (1st byte = len)	*/
  ST_UCHAR loc_tsel [MAX_TSEL_LEN+1];	/* Local TSEL (1st byte = len)	*/
  ST_UCHAR rem_tsel [MAX_TSEL_LEN+1];	/* Remote TSEL (1st byte = len)	*/
  union
    {
    ST_UCHAR nsap [CLNP_MAX_LEN_NSAP+1];/* Remote NSAP (1st byte = len)	*/
    ST_ULONG ip;			/* OR IP Addr (network byte order)*/
    }remNetAddr;
  /* authentication and security */
  ACSE_AUTH_INFO     auth_info;		/* local   authentication info	*/
  ACSE_AUTH_INFO     part_auth_info;	/* partner authentication info	*/
  S_SEC_ENCRYPT_CTRL encrypt_ctrl;	/* connection encryption info	*/ 
  } ACSE_CONN;

/************************************************************************/
/* General address matching defines, control, functions			*/
/*									*/

#if defined(MMS_LITE)
/* Lean-T specific functions */
ST_RET dib_match_remote_ar (DIB_MATCH_CTRL *matchCtrl, AARQ_APDU *aarq, 
			    DIB_ENTRY **dib_entry_out);
ST_RET dib_match_local_ar  (DIB_MATCH_CTRL *matchCtrl, AARQ_APDU *aarq, 
			    DIB_ENTRY **dib_entry_out);
ST_INT dib_cmp_local_addr  (DIB_MATCH_CTRL *matchCtrl, AARQ_APDU *aarq, 
			    DIB_ENTRY *dib_entry);
ST_INT dib_cmp_remote_addr (DIB_MATCH_CTRL *matchCtrl, AARQ_APDU *aarq, 
			    DIB_ENTRY *dib_entry);

/* Functions to use when a global SSC_STACK_CFG structure contains a	*/
/* linked list of DIB_ENTRY structures (local AND remote together).	*/
ST_RET dib_match_remote_ar_ll (DIB_MATCH_CTRL *matchCtrl, 
	AARQ_APDU *aarq,
	DIB_ENTRY **dib_entry_out);
#endif	/* defined(MMS_LITE)	*/

/************************************************************************/
/* Security functions							*/
ST_RET a_get_security_info (ACSE_CONN *acse_conn_id, S_SEC_SPARAMS *secParams);
ST_RET a_free_security_info (ACSE_AUTH_INFO *authInfo);
ST_RET a_free_part_security_info (ACSE_CONN *acse_conn_id);


/************************************************************************/
/* Global variables.							*/
/************************************************************************/
extern MMS_OBJ_ID mms_obj_id;		/* ISO MMS Object ID	*/
extern MMS_OBJ_ID acse_obj_id;		/* ACSE Object ID	*/
extern MMS_OBJ_ID asn1_obj_id;		/* ASN.1 Object ID	*/
extern MMS_OBJ_ID a_auth_password_obj_id;	/* Obj ID for ACSE Password*/
						/* Authentication mech.	*/

extern ST_UINT acse_msgsize;
extern ST_UINT cosp_msgsize;
extern ST_UCHAR *cosp_buf;

/************************************************************************/
/* Prototypes								*/
/************************************************************************/
ST_VOID acse_free_con (ACSE_CONN *con);

/* ACSE Decode functions	*/
ST_RET acse_dec_aarq (
	ST_UCHAR *pdu_ptr,	/* PDU pointer		*/
	ST_INT pdu_len,		/* PDU length		*/
	ST_INT8 pcimms,		/* PCI negotiated for MMS data	*/
	AARQ_APDU *aarq_apdu);	/* out: decoded data	*/
ST_RET acse_dec_aare (
	ST_UCHAR *pdu_ptr,	/* PDU pointer		*/
	ST_INT pdu_len,		/* PDU length		*/
	ST_INT8 pcimms,		/* PCI negotiated for MMS data	*/
	AARE_APDU *aare_apdu);	/* out: decoded data	*/
ST_RET acse_dec_rlrq (
	ST_UCHAR *pdu_ptr,	/* PDU pointer		*/
	ST_INT pdu_len,		/* PDU length		*/
	ST_INT8 pcimms,		/* PCI negotiated for MMS data	*/
	RLRQ_APDU *rlrq_apdu);	/* out: decoded data	*/
ST_RET acse_dec_rlre (
	ST_UCHAR *pdu_ptr,	/* PDU pointer		*/
	ST_INT pdu_len,		/* PDU length		*/
	ST_INT8 pcimms,		/* PCI negotiated for MMS data	*/
	RLRE_APDU *rlre_apdu);	/* out: decoded data	*/
ST_RET acse_dec_abrt (
	ST_UCHAR *pdu_ptr,	/* PDU pointer		*/
	ST_INT pdu_len,		/* PDU length		*/
	ST_INT8 pcimms,		/* PCI negotiated for MMS data	*/
	ABRT_APDU *abrt_apdu);	/* out: decoded data	*/

#ifdef __cplusplus
}
#endif

#endif	/* ACSE2_INCLUDED */

