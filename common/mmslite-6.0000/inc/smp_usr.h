/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1997 - 1998, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : smp_usr.h						*/
/* PRODUCT(S)  : UCA Station Management Protocol (UCA SMP).		*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module provides interface function to the SLAVE/MASTER	*/
/*	side of the UCA Station Management Protocol.			*/
/*									*/
/*	Implemented Services:						*/
/*		Time Synchronization					*/
/*									*/
/*									*/
/* MODIFICATION LOG :							*/
/*									*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 02/18/98  EJV    04     Added smp_init function.			*/
/* 11/17/97  EJV    03     Added HEX logging. Added par to smp_hc_config*/
/* 10/13/97  EJV    02     Added defs for DLL.				*/
/* 09/09/97  EJV    01     Created					*/
/************************************************************************/

#ifndef SMP_USR_INCLUDED
#define SMP_USR_INCLUDED


#ifdef __cplusplus
extern "C" {
#endif

	/*--------------------------------------*/
	/* Max size of MAC and NSAP		*/
	/* (same in clnp_llc.h)			*/
	/*--------------------------------------*/

#define SMP_MAX_LEN_MAC		6
#define SMP_MAX_LEN_NSAP	20


	/*--------------------------------------*/
	/* SMP logging types (for debugging)	*/
	/*--------------------------------------*/

#define SMP_LOG_ERR		0x00000001L
#define SMP_LOG_REQ		0x00000010L
#define SMP_LOG_IND		0x00000020L
#define SMP_LOG_DEB		0x00000040L
#define SMP_LOG_HEX		0x00000080L

extern ST_UINT smp_debug_sel;

	/*--------------------------------------*/
	/*	SMP_TIME struct			*/
	/*--------------------------------------*/

#define SMP_SIZE_OF_REC_TIME	32	/* buf size used to record binary time	*/

#define SMP_SIZE_OF_TIME	6	/* TimeOfDay size, see ISO/IEC 9506-2, clause 7.6.1	*/

typedef struct
  {
  ST_INT32 days;	/* number of days since January 1, 1984 	*/
  ST_INT32 msec;	/* milliseconds since midnight for the current date	*/
  ST_INT32 usec;	/* microseconds whithin current msec, if implemented	*/
  } SMP_TIME;

/* This struct is used also to pass the time difference	returned from	*/
/* remote node after time synchronization				*/

	/*--------------------------------------------------------------*/
	/* SLAVE application may disallow Time Synchronization from 	*/
	/* Master by setting the smp_time_locally_sync to SD_TRUE.				*/
	/*--------------------------------------------------------------*/

extern ST_BOOLEAN smp_time_locally_sync;


	/*======================================================*/
	/*	Initialization functions			*/
	/*======================================================*/

/************************************************************************/
/*			smp_init					*/
/*----------------------------------------------------------------------*/
/* This function is called from the user code to initialize all global	*/
/* variables used in SMP modules.					*/
/*									*/
/* Return:								*/
/*	ST_RET		SD_SUCCESS or SD_FAILURE.			*/
/************************************************************************/
ST_RET smp_init (ST_VOID);


	/*======================================================*/
	/*	SMP MASTER functions				*/
	/*======================================================*/

/************************************************************************/
/*			smp_time_sync_req_ar_name			*/
/*----------------------------------------------------------------------*/
/* Function called by Master to request Time Synchronization with	*/
/* remote node. Implemented for TRIM7 stack	.			*/
/* This function will map remote rem_ar_name into a MAC address and call*/
/* the smp_time_sync_req function.					*/
/*									*/
/* Parameters:								*/
/*	ST_LONG   user_id						*/
/*	ST_UCHAR *rem_ar_name						*/
/*	ST_VOID (*callBackFun)						*/
/*		(ST_LONG user_id, ST_RET result, SMP_TIME *delta_time));*/
/*									*/
/* Return:								*/
/*	ST_RET		SD_SUCCESS or SD_FAILURE.			*/
/************************************************************************/
ST_RET smp_time_sync_req_ar_name (
				ST_LONG   user_id,
				ST_UCHAR *rem_ar_name,
	ST_VOID (*callBackFun)(ST_LONG user_id, ST_RET result, SMP_TIME *delta_time));



/************************************************************************/
/*			smp_time_sync_req_chan				*/
/*----------------------------------------------------------------------*/
/* Function called by Master to request Time Synchronization with	*/
/* remote node.	Implemented for REDUCED STACK (only for ADLC subnet).	*/
/* This function will map chan_num into a loc,rem MAC address pair and	*/
/* call the smp_time_sync_req function.					*/
/*									*/
/* Note: A MMS connection must be established before calling this func.	*/
/*									*/
/* Parameters:								*/
/*	ST_LONG   user_id						*/
/*	ST_INT    chan_num						*/
/*	ST_VOID (*callBackFun)						*/
/*		(ST_LONG user_id, ST_RET result, SMP_TIME *delta_time));*/
/*									*/
/* Return:								*/
/*	ST_RET		SD_SUCCESS or SD_FAILURE.			*/
/************************************************************************/
ST_RET smp_time_sync_req_chan (
				ST_LONG   user_id,
				ST_INT    chan_num,
	ST_VOID (*callBackFun)(ST_LONG user_id, ST_RET result, SMP_TIME *delta_time));


/************************************************************************/
/*			smp_time_sync_req				*/
/*----------------------------------------------------------------------*/
/* Function called by Master to request Time Synchronization with	*/
/* remote node.								*/
/* When the time synchronization has been completed or error occurred,	*/
/* then the callBackFun will be used to notify the Master application	*/
/* about the result of time synchronization request.			*/
/*									*/
/* !Note: This function requires that the application establishes	*/
/*	  ADLC connection (for REDUCED_STACK) with given node before	*/
/*	  calling this function. For TRIM7 stack ADLC connections are	*/
/*	  maintained by ADLC_ML.					*/
/*	  On the Ethernet there is no need to establish connection.	*/
/*									*/
/* Parameters:								*/
/*	ST_LONG	  user_id;	for user to use				*/
/*	ST_UCHAR *loc_mac;	Ptr to Local MAC address		*/
/*	ST_UCHAR *rem_mac;	Ptr to Remote MAC address		*/
/*	ST_VOID	(*callBackFun)						*/
/*		(ST_LONG user_id, ST_RET result, SMP_TIME *delta_time);	*/
/*									*/
/* Return:								*/
/*	ST_RET		SD_SUCCESS or SD_FAILURE.			*/
/************************************************************************/
ST_RET smp_time_sync_req (
		ST_LONG	  user_id,	/* for user to use		*/
		ST_UCHAR *loc_mac,	/* Ptr to Local MAC address	*/
		ST_UCHAR *rem_mac,	/* Ptr to Remote MAC address	*/
		ST_VOID	(*callBackFun)(ST_LONG user_id, ST_RET result, SMP_TIME *delta_time));



	/*======================================================*/
	/*  SMP common SLAVE/MASTER functions			*/
	/*======================================================*/

typedef struct
  {
  ST_UCHAR  *pdu_buf;
  ST_UINT16  pdu_len;
  ST_UCHAR  *loc_mac;
  ST_UCHAR  *rem_mac;
  ST_UCHAR  *rec_time;
  } SMP_DATAUNIT;


/************************************************************************/
/*			smp_decode_uca_pdu				*/
/*----------------------------------------------------------------------*/
/* Function called by Slave application to decode received UCA Station	*/
/* Management Protocol PDU after LLC header has been decoded (dest=0xFB)*/
/* If the Slave application is a TRIM7 or Reduced Stack application	*/
/* this function will be called automatically from proper decoding	*/
/* functions.								*/
/*									*/
/* Parameters:								*/
/*									*/
/* Return:								*/
/*	ST_RET		SD_SUCCESS or SD_FAILURE.			*/
/************************************************************************/
ST_RET smp_decode_uca_pdu (SMP_DATAUNIT *smp_du);


/************************************************************************/
/*			smp_send_pdu_callback				*/
/*----------------------------------------------------------------------*/
/* This callback function called by ADLC sub-network layer when a PDU	*/
/* has been sent. Saves time stamp of Measure-Response PDU only.	*/
/*									*/
/* Parameters:								*/
/*									*/
/* Return:								*/
/*	ST_VOID								*/
/************************************************************************/
ST_VOID smp_send_pdu_callback (ST_RET result, ST_LONG user_id,
			       ST_UCHAR *rec_time, ST_UINT rec_time_len);


	/*======================================================*/
	/*  SMP time SLAVE/MASTER functions implemented by user	*/
	/*======================================================*/


#if defined(SMP_DLL)
#include <windows.h>
#define DllImport	__declspec (dllimport)
#define DllExport	__declspec (dllexport)
#endif

/************************************************************************/
/*			u_smp_record_time				*/
/*----------------------------------------------------------------------*/
/* Function implemented by user to record time.				*/
/* The rec_time is a buffer of rec_time_len bytes (SMP_SIZE_OF_REC_TIME)*/
/*									*/
/* Parameters:								*/
/*									*/
/* Return:								*/
/*	ST_RET		SD_SUCCESS or SD_FAILURE.			*/
/************************************************************************/
#if defined(SMP_DLL)
DllExport ST_RET WINAPI u_smp_record_time (ST_UCHAR *rec_time, ST_UINT rec_time_len);
#else
ST_RET u_smp_record_time (ST_UCHAR *rec_time, ST_UINT rec_time_len);
#endif


/************************************************************************/
/*			u_smp_convert_time				*/
/*----------------------------------------------------------------------*/
/* Function implemented by user converting recorded time in rec_time	*/
/* buffer to the SMP_TIME struct the application uses.			*/
/* The rec_time is a buffer of rec_time_len bytes (SMP_SIZE_OF_REC_TIME)*/
/*									*/
/* Parameters:								*/
/*									*/
/* Return:								*/
/*	ST_RET		SD_SUCCESS or SD_FAILURE.			*/
/************************************************************************/
#if defined(SMP_DLL)
DllExport ST_RET WINAPI u_smp_convert_time (ST_UCHAR *rec_time,
                                            ST_UINT   rec_time_len,
                                            SMP_TIME *smp_time);
#else
ST_RET u_smp_convert_time (ST_UCHAR *rec_time,
                           ST_UINT   rec_time_len,
                           SMP_TIME *smp_time);
#endif


	/*======================================================*/
	/*	SMP SLAVE functions implemented by user		*/
	/*======================================================*/


/************************************************************************/
/*			u_smp_time_sync_ind				*/
/*----------------------------------------------------------------------*/
/* SLAVE function implemented by user to synchronize time.		*/
/*									*/
/* Parameters:								*/
/*			SLAVE side:					*/
/*	SMP_TIME *a1	recorded time of Measure-RequestPDU  reception	*/
/*	SMP_TIME *a2	recorded time of Measure-ResponsePDU sending	*/
/*			MASTER side:					*/
/*	SMP_TIME *b1	recorded time of Measure-RequestPDU  sending	*/
/*	SMP_TIME *b2	recorded time of Measure-ResponsePDU arrival	*/
/*	SMP_TIME *delta_time return the local_adjustement.		*/
/*									*/
/* Return:								*/
/*	ST_RET		SD_SUCCESS or SD_FAILURE.			*/
/************************************************************************/
ST_RET u_smp_time_sync_ind (SMP_TIME *a1, SMP_TIME *a2,
			    SMP_TIME *b1, SMP_TIME *b2,
			    SMP_TIME *delta_time);
		



	/*======================================================*/
	/* For applications which do not link with TRIM7 or	*/
	/* Reduced Stack the smp_hc_config function may be	*/
	/* called to initialize needed parameters.		*/
	/*======================================================*/

/************************************************************************/
/*			smp_hc_config					*/
/*----------------------------------------------------------------------*/
/* This USER's func provides hard coded initialization of few parameters*/
/* for applications which DO NOT link to TRIM7 or REDUCED STACK.	*/
/*									*/
/* Parameters:								*/
/*	ST_VOID *loc_addr	Local ADLC address			*/
/*				(ignored for other subnet)		*/
/*	ST_CHAR *smpLogFile	Pointer log file name buffer		*/
/*									*/
/* Return:								*/
/*	SD_SUCCESS (0)		if initialization successful		*/
/*	error code		otherwise				*/
/************************************************************************/
ST_RET smp_hc_config (ST_VOID *loc_addr, ST_CHAR *smpLogFile);


#ifdef __cplusplus
}
#endif

#endif  /* end of 'already included' 	*/


