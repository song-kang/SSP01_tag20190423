/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 1997, All Rights Reserved.		        */
/*									*/
/* MODULE NAME : mms_pprg.h						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This file has the data structures and function definitions	*/
/*	required to interface with MMS program invocation management	*/
/*	services at the primitive level.   				*/
/*									*/
/*	Parameters : In general, the first parameter is the channel	*/
/*		     that the request or response is to be sent over.	*/
/*		     For Responses, the second parameter is the Invoke	*/
/*		     ID to be used. The last parameter is used to pass	*/
/*		     operation specific information by pointer. 	*/
/*									*/
/*	Return								*/
/*	    Values : Request functions return a pointer to the pending	*/
/*		     request tracking structure, of type MMSREQ_PEND.	*/
/*		     In case of error, the pointer is returned == 0	*/
/*		     and mms_op_err is written with the err code.	*/
/*		     Response functions return 0 if OK, else an error	*/
/*		     code.						*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 08/14/97  RKR     02    Format changes				*/
/* 06/09/97  MDE     01    Added PI State defines (from mms_vprg.h)	*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#ifndef MMS_PPRG_INCLUDED
#define MMS_PPRG_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************/
/************************************************************************/
/* PROGRAM INVOCATION MANAGEMENT SERVICES				*/
/************************************************************************/
/* The structures below are used to pass PROGRAM INVOCATION MANAGEMENT	*/
/* information to/from the MMS primitives.				*/
/************************************************************************/
/* use these defines to determine whether string is simple or encoded   */
/* If encoded, the data pointer points to a complete ASN.1 EXTERNAL	*/

#define ARG_TYPE_SIMPLE		0
#define ARG_TYPE_ENCODED 	1

/************************************************************************/
/* CREATE PROGRAM INVOCATION						*/
/************************************************************************/

/* REQUEST :								*/

struct crepi_req_info
  {
  ST_CHAR    piname [MAX_IDENT_LEN+1];	/* program invocation name	*/
  ST_INT     num_of_dnames;		/* number of domain names	*/
  ST_BOOLEAN reusable;			
  ST_BOOLEAN monitor_pres;		/* monitoring present		*/
  ST_BOOLEAN monitor;			/* SD_TRUE : permanent monitoring	*/
					/* SD_FALSE: current monitoring	*/

/*					   list of ptrs to domain names */
/* ST_CHAR   *dnames_list [num_of_dnames]; 				*/
  SD_END_STRUCT
  };
typedef struct crepi_req_info CREPI_REQ_INFO;

/* RESPONSE : NULL							*/

#ifndef MMS_LITE
MMSREQ_PEND *mp_crepi (ST_INT chan, CREPI_REQ_INFO *info);
ST_RET mp_crepi_resp (MMSREQ_IND *ind);
#else
ST_RET mpl_crepi (CREPI_REQ_INFO *info);
ST_RET mpl_crepi_resp (ST_UINT32 invoke);
#endif

/************************************************************************/
/* DELETE PROGRAM INVOCATION						*/
/************************************************************************/

/* REQUEST :								*/

struct delpi_req_info
  {
  ST_CHAR piname [MAX_IDENT_LEN+1];	/* program invocation name	*/
  };
typedef struct delpi_req_info DELPI_REQ_INFO;

/* RESPONSE : NULL							*/

#ifndef MMS_LITE
MMSREQ_PEND *mp_delpi (ST_INT chan, DELPI_REQ_INFO *info);
ST_RET mp_delpi_resp (MMSREQ_IND *ind);
#else
ST_RET mpl_delpi (DELPI_REQ_INFO *info);
ST_RET mpl_delpi_resp (ST_UINT32 invoke);
#endif

/************************************************************************/
/* START								*/
/************************************************************************/

/* REQUEST :								*/

struct start_req_info
  {
  ST_CHAR    piname [MAX_IDENT_LEN+1];	/* program invocation name	*/

  ST_INT16   start_arg_type; 		/* start argument present	*/
  ST_BOOLEAN start_arg_pres; 		/* start argument present	*/
  ST_INT     start_arg_len;			/* Only used for ENCODED	*/
  ST_UCHAR   *start_arg;			/* pointer to start argument	*/
  SD_END_STRUCT
  };
typedef struct start_req_info START_REQ_INFO;

/* RESPONSE : NULL							*/

#ifndef MMS_LITE
MMSREQ_PEND *mp_start (ST_INT chan, START_REQ_INFO *info);
ST_RET mp_start_resp (MMSREQ_IND *ind);
#else
ST_RET mpl_start (START_REQ_INFO *info);
ST_RET mpl_start_resp (ST_UINT32 invoke);
#endif

/************************************************************************/
/* STOP 								*/
/************************************************************************/

/* REQUEST :								*/

struct stop_req_info
  {
  ST_CHAR piname [MAX_IDENT_LEN+1];	/* program invocation name	*/
  };
typedef struct stop_req_info STOP_REQ_INFO;

/* RESPONSE : NULL							*/

#ifndef MMS_LITE
MMSREQ_PEND *mp_stop (ST_INT chan, STOP_REQ_INFO *info);
ST_RET mp_stop_resp (MMSREQ_IND *ind);
#else
ST_RET mpl_stop (STOP_REQ_INFO *info);
ST_RET mpl_stop_resp (ST_UINT32 invoke);
#endif

/************************************************************************/
/* RESUME								*/
/************************************************************************/

/* REQUEST :								*/

struct resume_req_info
  {
  ST_CHAR    piname [MAX_IDENT_LEN+1];	/* program invocation name	*/

  ST_INT16   resume_arg_type; 		/* resume argument present	*/
  ST_BOOLEAN resume_arg_pres; 		/* resume argument present	*/
  ST_INT     resume_arg_len;		/* Only used for ENCODED	*/
  ST_UCHAR   *resume_arg;		/* pointer to resume argument	*/
  };
typedef struct resume_req_info RESUME_REQ_INFO;

/* RESPONSE : NULL							*/

#ifndef MMS_LITE
MMSREQ_PEND *mp_resume (ST_INT chan, RESUME_REQ_INFO *info);
ST_RET mp_resume_resp (MMSREQ_IND *ind);
#else
ST_RET mpl_resume (RESUME_REQ_INFO *info);
ST_RET mpl_resume_resp (ST_UINT32 invoke);
#endif

/************************************************************************/
/* RESET								*/
/************************************************************************/

/* REQUEST :								*/

struct reset_req_info
  {
  ST_CHAR piname [MAX_IDENT_LEN+1];	/* program invocation name	*/
  };
typedef struct reset_req_info RESET_REQ_INFO;

/* RESPONSE : NULL							*/

#ifndef MMS_LITE
MMSREQ_PEND *mp_reset (ST_INT chan, RESET_REQ_INFO *info);
ST_RET mp_reset_resp (MMSREQ_IND *ind);
#else
ST_RET mpl_reset (RESET_REQ_INFO *info);
ST_RET mpl_reset_resp (ST_UINT32 invoke);
#endif

/************************************************************************/
/* KILL 								*/
/************************************************************************/

/* REQUEST :								*/

struct kill_req_info
  {
  ST_CHAR piname [MAX_IDENT_LEN+1];	/* program invocation name	*/
  };
typedef struct kill_req_info KILL_REQ_INFO;

/* RESPONSE : NULL							*/
 
#ifndef MMS_LITE
MMSREQ_PEND *mp_kill (ST_INT chan, KILL_REQ_INFO *info);
ST_RET mp_kill_resp (MMSREQ_IND *ind);
#else
ST_RET mpl_kill (KILL_REQ_INFO *info);
ST_RET mpl_kill_resp (ST_UINT32 invoke);
#endif

/************************************************************************/
/* GET PROGRAM INVOCATION ATTRIBUTE					*/
/************************************************************************/

#define PI_NON_EXISTENT	0
#define PI_UNRUNNABLE	1
#define PI_IDLE		2
#define PI_RUNNING	3
#define PI_STOPPED	4
#define PI_STARTING	5
#define PI_STOPPING	6
#define PI_RESUMING	7
#define PI_RESETTING	8

/* REQUEST :								*/

struct getpi_req_info
  {
  ST_CHAR piname [MAX_IDENT_LEN+1];	/* program invocation name	*/
  };
typedef struct getpi_req_info GETPI_REQ_INFO;

/* RESPONSE :								*/

struct getpi_resp_info
  {
  ST_INT16   state; 			/* program invocation state	*/
  ST_BOOLEAN mms_deletable; 		/* MMS deletable		*/
  ST_BOOLEAN reusable;			/* Reusable			*/
  ST_BOOLEAN monitor;			/* SD_TRUE : permanent monitoring	*/
					/* SD_FALSE: current monitoring	*/

  ST_INT16   start_arg_type; 		/* start argument present	*/
  ST_INT     start_arg_len;		/* Only used for ENCODED	*/
  ST_UCHAR   *start_arg;		/* pointer to start argument	*/

  ST_INT     num_of_dnames;		/* number of domain names	*/
/*					   list of ptrs to domain names */
/*  ST_CHAR  *dnames_list [num_of_dnames]; 				*/
  SD_END_STRUCT
  };
typedef struct getpi_resp_info GETPI_RESP_INFO;

#ifndef MMS_LITE
MMSREQ_PEND *mp_getpi (ST_INT chan, GETPI_REQ_INFO *info);
ST_RET mp_getpi_resp (MMSREQ_IND *ind, GETPI_RESP_INFO *info);
#else
ST_RET mpl_getpi (GETPI_REQ_INFO *info);
ST_RET mpl_getpi_resp (ST_UINT32 invoke, GETPI_RESP_INFO *info);
#endif		

#ifdef __cplusplus
}		
#endif

#endif	/* #define MMS_PPRG_INCLUDED */
