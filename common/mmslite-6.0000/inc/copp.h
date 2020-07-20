/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	1997-2009, All Rights Reserved					*/
/*									*/
/*			PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : copp.h 						*/
/* PRODUCT(S)  :							*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*	COPP internal header file.					*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev     Comments					*/
/* --------  ---  ------   -------------------------------------------  */
/* 11/17/09  JRB    05     Add new COPP decode functions (copp_dec_*).	*/
/* 12/11/01  JRB    04     Convert to use ASN1R.			*/
/* 02/20/98  JRB    03     Chg abort_reason to ST_INT8 too.		*/
/* 12/22/97  JRB    02     Use ST_INT8.					*/
/* 05/27/97  JRB   7.00    MMSEASE 7.0 release.				*/
/* 03/05/97  JRB    01     Created.					*/
/************************************************************************/

#ifndef COPP_INCLUDED
#define COPP_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "copp_usr.h"
#include "copp_log.h"

	/*======================================================*/
	/*	C O N N E C T I O N   S T A T E S		*/
	/*======================================================*/

/* Valid states, the first IDLE state must be 0) */
#define	COPP_STATE_IDLE			0
#define	COPP_STATE_AWAIT_CPA		1
#define	COPP_STATE_AWAIT_CON_RSP	2
#define	COPP_STATE_CONNECTED		3

	/*======================================================*/
	/*	PRESENTATION STRUCTURES				*/
	/*======================================================*/
typedef struct
  {
  ST_INT8 expected_pci;
  ST_INT8 pci;
  ST_UCHAR *pdv_data_ptr;
  ST_INT pdv_data_len;
  } PDV_INFO;

/* Structure for P-context-def-result-list decode (in CPA or CPR).*/
typedef struct
  {
  ST_INT num_result;		/* num of pc_def_result in PPDU		*/
  ST_BOOLEAN bad_pc_deflist;	/* SD_TRUE if responder didn't like the	*/
				/* pc_deflist it received in CP PDU	*/
  } COPP_DEC_PC_DEF_RESULT_LIST;

/* Structure for tracking CP decode.	*/
typedef struct
  {
  ST_UCHAR version_bit_str;
  ST_UCHAR calling_psel   [MAX_PSEL_LEN+1];
  ST_UCHAR called_psel    [MAX_PSEL_LEN+1];
  ST_INT num_ctxt;
  ST_BOOLEAN bad_pc_deflist;	/* SD_TRUE if decoder doesn't like the	*/
				/* pc_deflist received in CP PDU	*/
  ST_BOOLEAN def_ctxt_proposed;
  ST_INT8 mode;
  ST_BOOLEAN pres_req_present;
  ST_UCHAR pres_req [1];
  ST_BOOLEAN user_ses_req_present;
  ST_UCHAR user_sess_req [2];
  ST_BOOLEAN send_negative_resp;/* if set, send negative response	*/
  ST_INT negative_resp_reason;	/* reason to send in negative response	*/
  PDV_INFO pdv_info;		/* Presentation Data Value decoded	*/
  ST_INT8 pcimms;		/* MMS PCI decoded	*/
  ST_INT8 pciacse;		/* ACSE PCI decoded	*/
  } COPP_DEC_CP;

/* Structure for tracking CPA decode.	*/
typedef struct
  {
  ST_UCHAR version_bit_str;
  ST_UCHAR responding_psel[MAX_PSEL_LEN+1];
  COPP_DEC_PC_DEF_RESULT_LIST pc_def_result_list;
  ST_INT8 mode;
  ST_BOOLEAN pres_req_present;
  ST_UCHAR pres_req [1];
  ST_BOOLEAN user_ses_req_present;
  ST_UCHAR user_sess_req [2];
  PDV_INFO pdv_info;		/* Presentation Data Value decoded	*/
  } COPP_DEC_CPA;

/* Structure for tracking CPR decode.	*/
typedef struct
  {
  ST_UCHAR version_bit_str;
  ST_UCHAR responding_psel[MAX_PSEL_LEN+1];
  COPP_DEC_PC_DEF_RESULT_LIST pc_def_result_list;
  ST_INT8 default_context_result;
  ST_BOOLEAN provider_reason_present;	/* Is provider_reason present in PPDU?*/
  ST_INT8 provider_reason;
  PDV_INFO pdv_info;		/* Presentation Data Value decoded	*/
  } COPP_DEC_CPR;

/* Structure for tracking ARU or ARP decode.	*/
typedef struct
  {
#define COPP_U_ABORT	0	/* Use to set "abort_type"	*/
#define COPP_P_ABORT	1	/* Use to set "abort_type"	*/
  ST_INT8 abort_type;
  ST_INT8 abort_reason;		/* abort reason in ARP PDU		*/
  ST_INT8 event_id;		/* event id in ARP PDU			*/
  ST_INT num_identifier;	/* number of identifiers in ARU PDU   	*/
  PDV_INFO pdv_info;		/* Presentation Data Value decoded	*/
  } COPP_DEC_ARX;

	/*======================================================*/
	/*	PRESENTATION INTERNAL FUNCTIONS			*/
	/*======================================================*/

/* Function to encode and send ARP PPDU.				*/
ST_VOID copp_p_abort_req (ACSE_CONN *acse_conn, ST_INT8 abort_reason, ST_INT8 event_id);

	/*======================================================*/
	/*	PRESENTATION DECODE FUNCTIONS			*/
	/*======================================================*/

ST_RET copp_dec_cp (
	ST_UCHAR *pdu_ptr,		/* PDU pointer			*/
	ST_INT pdu_len,			/* PDU length			*/
	COPP_DEC_CP *cp_info);		/* out: decoded data		*/

ST_RET copp_dec_cpa (
	ST_UCHAR *pdu_ptr,		/* PDU pointer			*/
	ST_INT pdu_len,			/* PDU length			*/
	ST_INT8 expected_pci,		/* expected PCI			*/
	COPP_DEC_CPA *cpa_info);	/* out: decoded data		*/

ST_RET copp_dec_cpr (
	ST_UCHAR *pdu_ptr,		/* PDU pointer			*/
	ST_INT pdu_len,			/* PDU length			*/
	ST_INT8 expected_pci,		/* expected PCI			*/
	COPP_DEC_CPR *cpr_info);	/* out: decoded data		*/

/* Decode ARU or ARP PDU.	*/
ST_RET copp_dec_arx (
	ST_UCHAR *pdu_ptr,		/* PDU pointer			*/
	ST_INT pdu_len,			/* PDU length			*/
	ST_INT8 expected_pci,		/* expected PCI			*/
	COPP_DEC_ARX *arx_info);	/* out: decoded data		*/

ST_RET copp_dec_user_data_only (
	ST_UCHAR *pdu_ptr,		/* PDU pointer			*/
	ST_INT pdu_len,			/* PDU length			*/
	ST_INT8 expected_pci,		/* expected PCI			*/
	PDV_INFO *pdv_info);		/* out: decoded PDV data	*/

/* NOTE: this may be called during COPP or ACSE decode.		*/
ST_RET copp_dec_pdv_entry (
	ASN1_DEC_CTXT *aCtx,	/* ASN.1 decode context		*/
	PDV_INFO *pdv_info);	/* in/out: decoded PDV data	*/
				/* set pdv_info->expected_pci before calling*/

	/*======================================================*/
	/*	PRESENTATION INTERNAL VARIABLES			*/
	/*======================================================*/

extern ST_UCHAR only_loc_psel [];	/* Local len & PSEL	*/

/************************************************************************/
#ifdef __cplusplus
}
#endif

#endif /* COPP_INCLUDED */
/************************************************************************/

