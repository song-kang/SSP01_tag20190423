/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1997, All Rights Reserved.		        	*/
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : smp.h							*/
/* PRODUCT(S)  : UCA Station Management Protocol (UCA SMP).		*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This header file specifies internal defines and function	*/
/*	prototypes for UCA Station Management Protocol (UCA SMP).	*/
/*									*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*									*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 09/09/96  EJV    01     Created					*/
/************************************************************************/

#ifndef SMP_INCLUDED
#define SMP_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

	/*--------------------------------------*/
	/* LLC level encoding values		*/
	/*--------------------------------------*/

	/* (same in clnp_llc.h)			*/
#define LLC_LSAP_SMP	(ST_UCHAR) 0xFB	/* For UCA SMP (Station Mgt Prot) */
#define LLC_CONTROL_SMP	(ST_UCHAR) 0x03	/* For UCA SMP (Station Mgt Prot) */

	/*--------------------------------------*/
	/* Max size of Autorization field	*/
	/*--------------------------------------*/

#define SMP_SIZE_OF_AUTH	8
	
	/*----------------------------------------------*/
	/* Valid codes SMP UCA PDU type			*/
	/*----------------------------------------------*/

#define SMP_UCA_PDU_ERROR		0
#define SMP_UCA_PDU_TIMESYNC		1

#define SMP_PDU_INVALID_TYPE		255

	/*----------------------------------------------*/
	/* Valid codes for TimeSync PDU type		*/
	/*----------------------------------------------*/

#define SMP_TIMESYNC_PDU_MEASURE_REQ	0
#define SMP_TIMESYNC_PDU_MEASURE_RSP	1
#define SMP_TIMESYNC_PDU_SYNC_REQ	2
#define SMP_TIMESYNC_PDU_SYNC_RSP	3
#define SMP_TIMESYNC_PDU_ERROR		4
#define SMP_TIMESYNC_PDU_PREPARE	5


	/*----------------------------------------------*/
	/* Valid error codes in the UCA ErrorPDU	*/
	/*----------------------------------------------*/

#define SMP_ERR_NO_ERROR		0
#define SMP_ERR_UNSUPPORTED		1
#define SMP_ERR_UNRECOGNIZED_SERVICE	2
#define SMP_ERR_PDU_ERROR		3
#define SMP_ERR_OUT_OF_RESOURCES	4

	/*----------------------------------------------*/
	/* Valid error codes in the Sync-ErrorPDU	*/
	/*----------------------------------------------*/

#define SMP_SYNC_ERR_NO_ERROR		0
#define SMP_SYNC_ERR_LOCALLY_SYNCED	1
#define SMP_SYNC_ERR_NOT_AUTHORIZED	2
#define SMP_SYNC_ERR_NOT_MEASURED	3
#define SMP_SYNC_ERR_SEQUENCE_PROBLEM	4

	/*--------------------------------------*/
	/* globals in smp_dec.c module 		*/
	/*--------------------------------------*/

extern ST_UCHAR	smp_uca_pdu_type;
extern ST_UCHAR	smp_err_code;
extern ST_UCHAR	smp_timesync_pdu_type;
extern SMP_TIME	smp_time_req;
extern SMP_TIME	smp_time_rsp;
extern ST_UCHAR	smp_auth [SMP_SIZE_OF_AUTH];	/* authentication */
extern SMP_TIME	smp_delta_time;
extern ST_UCHAR	smp_sync_err_code;

ST_RET smp_dec_uca_pdu (ST_UCHAR *pdu_buf, ST_UINT16  pdu_len);


#ifdef __cplusplus
}
#endif

#endif  /* end of 'already included' 	*/


