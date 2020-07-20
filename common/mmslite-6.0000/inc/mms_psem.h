/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 1997, All Rights Reserved.		        */
/*									*/
/* MODULE NAME : mms_psem.h						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This file has the data structures and function definitions	*/
/*	required to interface with MMS semaphore management services	*/
/*	at the primitive level.						*/
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
/* 12/10/97  MDE     02    Changed entry_class to ST_INT8		*/
/* 08/14/97  RKR     01    Format changes				*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#ifndef MMS_PSEM_INCLUDED
#define MMS_PSEM_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "mms_mp.h"

/************************************************************************/
/************************************************************************/
/* SEMAPHORE MANAGEMENT FUNCTIONS					*/
/************************************************************************/
/* The structures below are used to pass SEMAPHORE MANAGEMENT operation */
/* information to/from the MMS primitives.				*/
/************************************************************************/

/************************************************************************/
/* TAKE CONTROL, Conformance: SEM1/SEM3 				*/
/************************************************************************/

/* REQUEST :								*/

struct takectrl_req_info
  {
  OBJECT_NAME sem_name; 		/* semaphore name		*/
  ST_BOOLEAN  named_token_pres;		/* named token present ind	*/
  ST_CHAR     named_token[MAX_IDENT_LEN+1]; /* named token			*/
  ST_UCHAR    priority;			/* priority			*/
					/*   0 - highest		*/
					/*  64 - normal (default)	*/
					/* 127 - lowest 		*/
  ST_BOOLEAN  acc_delay_pres; 		/* acceptable delay present ind */
  ST_UINT32   acc_delay;		/* acceptable delay		*/
  ST_BOOLEAN  ctrl_timeout_pres;	/* control timeout present	*/
  ST_UINT32   ctrl_timeout; 		/* control timeout		*/
  ST_BOOLEAN  abrt_on_timeout_pres; 	/* Abort on Timeout present	*/
  ST_BOOLEAN  abrt_on_timeout;		/* Abort on Timeout		*/
  ST_BOOLEAN  rel_conn_lost;		/* relenquish if connection lost*/
  ST_BOOLEAN  app_preempt_pres;		/* app preempt present ind	*/
  ST_INT      app_len;			/* length of app. to preempt	*/
  ST_UCHAR    *app_preempt;  		/* application to preempt	*/
  SD_END_STRUCT
  };
typedef struct takectrl_req_info TAKECTRL_REQ_INFO;

/* RESPONSE :								*/

struct takectrl_resp_info
  {
  ST_INT16 resp_tag;			/* response tag 		*/
					/*   0 : NULL response		*/
					/*   1 : named token response	*/
  ST_CHAR  named_token [MAX_IDENT_LEN+1];	/* named token identifier	*/
  SD_END_STRUCT
  };
typedef struct takectrl_resp_info TAKECTRL_RESP_INFO;

#ifndef MMS_LITE
MMSREQ_PEND *mp_takectrl (ST_INT chan, TAKECTRL_REQ_INFO *info);
ST_RET mp_takectrl_resp (MMSREQ_IND *ind, TAKECTRL_RESP_INFO *info);
#else
ST_RET mpl_takectrl (TAKECTRL_REQ_INFO *info);
ST_RET mpl_takectrl_resp (ST_UINT32 invoke, TAKECTRL_RESP_INFO *info);
#endif

/************************************************************************/
/* RELINQUISH CONTROL, Conformance: SEM1/SEM3				*/
/************************************************************************/

/* REQUEST :								*/

struct relctrl_req_info
  {
  OBJECT_NAME sem_name; 		/* semaphore name		*/
  ST_BOOLEAN  named_token_pres;		/* named token present ind	*/
  ST_CHAR     named_token [MAX_IDENT_LEN+1];/* named token			*/
  SD_END_STRUCT
  };
typedef struct relctrl_req_info	RELCTRL_REQ_INFO;

/* RESPONSE : NULL							*/

#ifndef MMS_LITE
MMSREQ_PEND *mp_relctrl (ST_INT chan, RELCTRL_REQ_INFO *info);
ST_RET mp_relctrl_resp (MMSREQ_IND *ind);
#else
ST_RET mpl_relctrl (RELCTRL_REQ_INFO *info);
ST_RET mpl_relctrl_resp (ST_UINT32 invoke);
#endif

/************************************************************************/
/* REPORT SEMAPHORE STATUS, Conformance: SEM1/SEM3			*/
/************************************************************************/

/* REQUEST :								*/

struct rsstat_req_info
  {
  OBJECT_NAME  sem_name; 	/* semaphore name		*/
  };
typedef struct rsstat_req_info RSSTAT_REQ_INFO;

/* RESPONSE :								*/

struct rsstat_resp_info
  {
  ST_BOOLEAN	mms_deletable;		/* mms deletable		*/
  ST_INT16  	tclass;			/* class: token (0) or pool (1) */
  ST_UINT16	num_of_tokens;		/* number of tokens		*/
  ST_UINT16	num_of_owned;		/* number of owned tokens	*/
  ST_UINT16	num_of_hung;		/* number of hung tokens	*/
  SD_END_STRUCT
  };
typedef struct rsstat_resp_info	RSSTAT_RESP_INFO;

#ifndef MMS_LITE
MMSREQ_PEND *mp_rsstat (ST_INT chan, RSSTAT_REQ_INFO *info);
ST_RET mp_rsstat_resp (MMSREQ_IND *ind, RSSTAT_RESP_INFO *info);
#else
ST_RET mpl_rsstat (RSSTAT_REQ_INFO *info);
ST_RET mpl_rsstat_resp (ST_UINT32 invoke, RSSTAT_RESP_INFO *info);
#endif

/************************************************************************/
/* REPORT POOL SEMAPHORE STATUS, Conformance: SEM1/SEM3 		*/
/************************************************************************/

/* REQUEST :								*/

struct rspool_req_info
  {
  OBJECT_NAME sem_name; 		/* semaphore name		*/
  ST_BOOLEAN  start_after_pres;		/* start after name present ind */
  ST_CHAR     start_after [MAX_IDENT_LEN+1]; /* name to start after		*/
  SD_END_STRUCT
  };
typedef struct rspool_req_info RSPOOL_REQ_INFO;

/* RESPONSE :								*/

#if !defined (USE_COMPACT_MMS_STRUCTS)

struct token_id
  {
  ST_INT16 token_tag;			/* named token tag		*/
					/*   0 : free named token	*/
					/*   1 : owned named token	*/
					/*   2 : hung named token	*/
  ST_CHAR  named_token[MAX_IDENT_LEN+1];/* named token id		*/
  SD_END_STRUCT
  };

#else		/* Use compact form */

struct token_id
  {
  ST_INT16 token_tag;			/* named token tag		*/
					/*   0 : free named token	*/
					/*   1 : owned named token	*/
					/*   2 : hung named token	*/
  ST_CHAR	*named_token;			/* named token id		*/
  SD_END_STRUCT
  };
#endif

typedef struct token_id	TOKEN_ID;

struct rspool_resp_info
  {
  ST_INT16   num_of_tokens;		/* number of named tokens	*/
  ST_BOOLEAN more_follows; 		/* more follows indicator	*/
/*					   list of named tokens 	*/
/*  TOKEN_ID named_token_list [num_of_tokens];			*/
  SD_END_STRUCT
  };
typedef struct rspool_resp_info	RSPOOL_RESP_INFO;

#ifndef MMS_LITE
MMSREQ_PEND *mp_rspool (ST_INT chan, RSPOOL_REQ_INFO *info);
ST_RET mp_rspool_resp (MMSREQ_IND *ind, RSPOOL_RESP_INFO *info);
#else
ST_RET mpl_rspool (RSPOOL_REQ_INFO *info);
ST_RET mpl_rspool_resp (ST_UINT32 invoke, RSPOOL_RESP_INFO *info);
#endif


/************************************************************************/
/* REPORT SEMAPHORE ENTRY STATUS, Conformance: SEM1/SEM3		*/
/************************************************************************/

/* REQUEST :								*/

struct rsentry_req_info
  {
  OBJECT_NAME sem_name; 		/* semaphore name		*/
  ST_INT16    state;	 		/* state: 0 : queued		*/
					/*	  1 : owner		*/
					/*	  2 : hung		*/
  ST_BOOLEAN  start_after_pres;		/* start after id present ind	*/
  ST_INT      sa_len; 			/* length of entry id to start	*/
					/*   after			*/
  ST_UCHAR    *start_after;		/* pointer to entry id to start */
					/*   after			*/
  SD_END_STRUCT
  };
typedef struct rsentry_req_info	RSENTRY_REQ_INFO;

/* RESPONSE :								*/

struct semaphore_entry
  {
  ST_INT     ei_len; 			/* entry id length		*/
  ST_UCHAR   *entry_id;			/* entry id			*/
  ST_INT8    entry_class;		/* entry class			*/
  ST_INT     app_ref_len;		/* app ref length (obj ident)	*/
  ST_UCHAR   *app_ref;			/* application reference	*/
  ST_BOOLEAN named_token_pres;		/* named token present ind	*/
  ST_CHAR    named_token[MAX_IDENT_LEN+1];/* named token		*/
  ST_UCHAR   priority;			/* priority			*/
					/*   0 - highest		*/
					/*  64 - normal (default)	*/
					/* 127 - lowest 		*/
  ST_BOOLEAN rem_timeout_pres;		/* relinquish timeout present	*/
  ST_UINT32  rem_timeout; 		/* relinquish timeout		*/
  ST_BOOLEAN abrt_on_timeout_pres;	/* Abort On Timeout present	*/
  ST_BOOLEAN abrt_on_timeout;		/* Abort On Timeout		*/
  ST_BOOLEAN rel_conn_lost;	 	/* relenquish if connection lost*/
  SD_END_STRUCT
  };
typedef struct semaphore_entry SEMAPHORE_ENTRY;

struct rsentry_resp_info
  {
  ST_INT num_of_sent;			/* number of semaphore entries	*/
  ST_BOOLEAN more_follows;		/* more follows indicator	*/
/*					   list of semaphore entries	*/
/* SEMAPHORE_ENTRY sent_list [num_of_sent];				*/
  SD_END_STRUCT
  };
typedef struct rsentry_resp_info RSENTRY_RESP_INFO;

#ifndef MMS_LITE
MMSREQ_PEND *mp_rsentry (ST_INT chan, RSENTRY_REQ_INFO *info);
ST_RET mp_rsentry_resp (MMSREQ_IND *ind, RSENTRY_RESP_INFO *info);
#else
ST_RET mpl_rsentry (RSENTRY_REQ_INFO *info);
ST_RET mpl_rsentry_resp (ST_UINT32 invoke, RSENTRY_RESP_INFO *info);
#endif

/************************************************************************/
/* DEFINE SEMAPHORE, Conformance: SEM3					*/
/************************************************************************/

/* REQUEST :								*/

struct defsem_req_info
  {
  OBJECT_NAME sem_name; 		/* semaphore name		*/
  ST_UINT16   num_of_tokens;		/* number of tokens		*/
  SD_END_STRUCT
  };
typedef struct defsem_req_info DEFSEM_REQ_INFO;

/* RESPONSE : NULL							*/

#ifndef MMS_LITE
MMSREQ_PEND *mp_defsem (ST_INT chan, DEFSEM_REQ_INFO *info);
ST_RET mp_defsem_resp (MMSREQ_IND *ind);
#else
ST_RET mpl_defsem (DEFSEM_REQ_INFO *info);
ST_RET mpl_defsem_resp (ST_UINT32 invoke);
#endif

/************************************************************************/
/* DELETE SEMAPHORE, Conformance: SEM3					*/
/************************************************************************/

/* REQUEST :								*/

struct delsem_req_info
  {
  OBJECT_NAME  sem_name; 	/* semaphore name		*/
  };
typedef struct delsem_req_info DELSEM_REQ_INFO;

/* RESPONSE : NULL							*/

#ifndef MMS_LITE
MMSREQ_PEND *mp_delsem (ST_INT chan, DELSEM_REQ_INFO *info);
ST_RET mp_delsem_resp (MMSREQ_IND *ind);
#else
ST_RET mpl_delsem (DELSEM_REQ_INFO *info);
ST_RET mpl_delsem_resp (ST_UINT32 invoke);
#endif

#ifdef __cplusplus
}
#endif

#endif	/* #define MMS_PSEM_INCLUDED */
