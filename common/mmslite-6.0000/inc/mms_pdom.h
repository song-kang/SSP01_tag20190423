/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 1997, All Rights Reserved.		        */
/*									*/
/* MODULE NAME : mms_pdom.h						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This file has the data structures and function definitions	*/
/*	required to interface with MMS domain management services	*/
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
/* 08/13/97  RKR     01    Format changes				*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#ifndef MMS_PDOM_INCLUDED
#define MMS_PDOM_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "mms_err.h"		/* need for service error definition	*/
#include "mms_mp.h"

/************************************************************************/
/************************************************************************/
/* DOMAIN MANAGEMENT SERVICES						*/
/************************************************************************/
/* The structures below are used to pass DOMAIN MANAGEMENT information	*/
/* to/from the MMS primitives.						*/
/************************************************************************/

/* use these defines to determine whether load data is coded or not	*/
/* If coded, the data pointer points to a complete ASN.1 EXTERNAL	*/

#define LOAD_DATA_NON_CODED		0
#define LOAD_DATA_CODED    		1


/************************************************************************/
/* INITIATE DOWNLOAD SEQUENCE						*/
/************************************************************************/

/* REQUEST :								*/

struct initdown_req_info
  {
  ST_CHAR    dname [MAX_IDENT_LEN +1];	/* domain name			*/
  ST_BOOLEAN sharable;			/* boolean, no default		*/
  ST_INT     num_of_capab; 		/* number of capabilities	*/
/*					   list of capabilities 	*/
/*  ST_CHAR * capab_list [num_of_capab];					*/
  SD_END_STRUCT
  };
typedef struct initdown_req_info INITDOWN_REQ_INFO;

/* RESPONSE : NULL							*/

#ifndef MMS_LITE
MMSREQ_PEND *mp_initdown (ST_INT chan, INITDOWN_REQ_INFO *info);
ST_RET mp_initdown_resp (MMSREQ_IND *ind);
#else
ST_RET mpl_initdown (INITDOWN_REQ_INFO *info);
ST_RET mpl_initdown_resp (ST_UINT32 invoke);
#endif

/************************************************************************/
/* DOWNLOAD SEGMENT							*/
/************************************************************************/

/* REQUEST :								*/

struct download_req_info
  {
  ST_CHAR dname [MAX_IDENT_LEN +1];	/* domain name			*/
  };
typedef struct download_req_info DOWNLOAD_REQ_INFO;

/* RESPONSE :								*/


struct download_resp_info
  {
  ST_INT     load_data_type;		/* coded/non-coded		*/
  ST_INT     load_data_len;		/* number of load data octets	*/
  ST_UCHAR   *load_data;			/* pointer to load data octets	*/
  ST_BOOLEAN more_follows;		/* boolean, default to true	*/
  };
typedef struct download_resp_info DOWNLOAD_RESP_INFO;
  
#ifndef MMS_LITE
MMSREQ_PEND *mp_download (ST_INT chan, DOWNLOAD_REQ_INFO *info);
ST_RET mp_download_resp (MMSREQ_IND *ind, DOWNLOAD_RESP_INFO *info);
#else
ST_RET mpl_download (DOWNLOAD_REQ_INFO *info);
ST_RET mpl_download_resp (ST_UINT32 invoke, DOWNLOAD_RESP_INFO *info);
#endif

/************************************************************************/
/* TERMINATE DOWNLOAD SEQUENCE						*/
/************************************************************************/

/* REQUEST :								*/

struct termdown_req_info
  {
  ST_CHAR    dname [MAX_IDENT_LEN +1];	/* domain name			*/
  ST_BOOLEAN discarded;			/* boolean, default to false	*/
  ERR_INFO   *err;			/* points to service error	*/
  };					/* (IS only, for discarded != 0	*/
typedef struct termdown_req_info TERMDOWN_REQ_INFO;

/* RESPONSE : NULL							*/

#ifndef MMS_LITE
MMSREQ_PEND *mp_termdown (ST_INT chan, TERMDOWN_REQ_INFO *info);
ST_RET mp_termdown_resp (MMSREQ_IND *ind);
#else
ST_RET mpl_termdown (TERMDOWN_REQ_INFO *info);
ST_RET mpl_termdown_resp (ST_UINT32 invoke);
#endif

/************************************************************************/
/* INITIATE UPLOAD SEQUENCE						*/
/************************************************************************/

/* REQUEST :								*/

struct initupl_req_info
  {
  ST_CHAR dname [MAX_IDENT_LEN +1];	/* domain name			*/
  };
typedef struct initupl_req_info	INITUPL_REQ_INFO;

/* RESPONSE :								*/

struct initupl_resp_info
  {
  ST_INT32 ulsmid; 			/* upload state machine id	*/
  ST_INT   num_of_capab; 		/* number of capabilities	*/
/*					   list of capabilities 	*/
/*  ST_CHAR *capab_list [num_of_capab];					*/
  SD_END_STRUCT
  };
typedef struct initupl_resp_info INITUPL_RESP_INFO;

#ifndef MMS_LITE
MMSREQ_PEND *mp_initupl (ST_INT chan, INITUPL_REQ_INFO *info);
ST_RET mp_initupl_resp (MMSREQ_IND *ind, INITUPL_RESP_INFO *info);
#else
ST_RET mpl_initupl (INITUPL_REQ_INFO *info);
ST_RET mpl_initupl_resp (ST_UINT32 invoke, INITUPL_RESP_INFO *info);
#endif

/************************************************************************/
/* UPLOAD SEGMENT							*/
/************************************************************************/

/* REQUEST :								*/

struct upload_req_info
  {
  ST_INT32	ulsmid;    		/* upload state machine id	*/
  };
typedef struct upload_req_info UPLOAD_REQ_INFO;

/* RESPONSE :								*/

struct upload_resp_info
  {
  ST_INT     load_data_type;		/* coded/non-coded		*/
  ST_INT     load_data_len;		/* number of load data octets	*/
  ST_UCHAR   *load_data;   		/* pointer to load data octets	*/
  ST_BOOLEAN more_follows;		/* boolean, default to true	*/
  };
typedef struct upload_resp_info	UPLOAD_RESP_INFO;

#ifndef MMS_LITE
MMSREQ_PEND *mp_upload (ST_INT chan, UPLOAD_REQ_INFO *info);
ST_RET mp_upload_resp (MMSREQ_IND *ind, UPLOAD_RESP_INFO *info);
#else
ST_RET mpl_upload (UPLOAD_REQ_INFO *info);
ST_RET mpl_upload_resp (ST_UINT32 invoke, UPLOAD_RESP_INFO *info);
#endif

/************************************************************************/
/* TERMINATE UPLOAD SEQUENCE						*/
/************************************************************************/

/* REQUEST :								*/

struct termupl_req_info
  {
  ST_INT32	ulsmid; 			/* upload state machine id	*/
  };
typedef struct termupl_req_info	TERMUPL_REQ_INFO;

/* RESPONSE : NULL							*/

#ifndef MMS_LITE
MMSREQ_PEND *mp_termupl (ST_INT chan, TERMUPL_REQ_INFO *info);
ST_RET mp_termupl_resp (MMSREQ_IND *ind);
#else
ST_RET mpl_termupl (TERMUPL_REQ_INFO *info);
ST_RET mpl_termupl_resp (ST_UINT32 invoke);
#endif

/************************************************************************/
/* REQUEST DOMAIN DOWNLOAD						*/
/************************************************************************/

/* REQUEST :								*/

struct rddwn_req_info
  {
  ST_CHAR    dname [MAX_IDENT_LEN +1];	/* domain name			*/
  ST_BOOLEAN sharable;			/* boolean, no default		*/
  ST_INT     num_of_capab;		/* number of capabilities	*/
  ST_INT     num_of_fname;		/* num of fname elements	*/
/*					   list of capabilities 	*/
/*  ST_CHAR * capab_list [num_of_capab];					*/
/*					   list of file names		*/
/*  FILE_NAME fname_list[ num_of_fname ];				*/
  SD_END_STRUCT
  };
typedef struct rddwn_req_info RDDWN_REQ_INFO;

/* RESPONSE : NULL							*/

#ifndef MMS_LITE
MMSREQ_PEND *mp_rddwn (ST_INT chan, RDDWN_REQ_INFO *info);
ST_RET mp_rddwn_resp (MMSREQ_IND *ind);
#else
ST_RET mpl_rddwn (RDDWN_REQ_INFO *info);
ST_RET mpl_rddwn_resp (ST_UINT32 invoke);
#endif

/************************************************************************/
/* REQUEST DOMAIN UPLOAD						*/
/************************************************************************/

/* REQUEST :								*/

struct rdupl_req_info
  {
  ST_CHAR dname [MAX_IDENT_LEN +1];	/* domain name			*/
  ST_INT  num_of_fname; 		/* num of fname elements	*/
/*					   list of file names		*/
/*  FILE_NAME fname_list[ num_of_fname ];				*/
  SD_END_STRUCT
  };
typedef struct rdupl_req_info RDUPL_REQ_INFO;

/* RESPONSE : NULL							*/

#ifndef MMS_LITE
MMSREQ_PEND *mp_rdupl (ST_INT chan, RDUPL_REQ_INFO *info);
ST_RET mp_rdupl_resp (MMSREQ_IND *ind);
#else
ST_RET mpl_rdupl (RDUPL_REQ_INFO *info);
ST_RET mpl_rdupl_resp (ST_UINT32 invoke);
#endif

/************************************************************************/
/* LOAD DOMAIN CONTENT							*/
/************************************************************************/

/* REQUEST :								*/

struct loaddom_req_info
  {
  ST_CHAR    dname [MAX_IDENT_LEN +1];	/* domain name			*/
  ST_BOOLEAN sharable;			/* boolean, no default		*/
  ST_BOOLEAN third_pty_pres;   		/* third party used		*/
  ST_INT     third_pty_len;		/* the length of third party	*/
  ST_UCHAR   *third_pty;		/* ptr to third party ASN.1	*/
  ST_INT     num_of_capab;		/* number of capabilities	*/
  ST_INT     num_of_fname; 		/* num of fname elements	*/
/*					   list of capabilities 	*/
/*  ST_CHAR *capab_list [num_of_capab];					*/
/*					   list of file names		*/
/*  FILE_NAME fname_list[ num_of_fname ];			*/
  SD_END_STRUCT
  };
typedef struct loaddom_req_info	LOADDOM_REQ_INFO;

/* RESPONSE : NULL							*/

#ifndef MMS_LITE
MMSREQ_PEND *mp_loaddom (ST_INT chan, LOADDOM_REQ_INFO *info);
ST_RET mp_loaddom_resp (MMSREQ_IND *ind);
#else
ST_RET mpl_loaddom (LOADDOM_REQ_INFO *info);
ST_RET mpl_loaddom_resp (ST_UINT32 invoke);
#endif

/************************************************************************/
/* STORE DOMAIN CONTENT 						*/
/************************************************************************/

/* REQUEST :								*/

struct storedom_req_info
  {
  ST_CHAR    dname [MAX_IDENT_LEN +1];	/* domain name			*/
  ST_INT     num_of_fname; 		/* num of fname elements	*/
  ST_BOOLEAN third_pty_pres; 		/* third party used		*/
  ST_INT     third_pty_len;		/* the length of third party	*/
  ST_UCHAR   *third_pty;		/* ptr to third party ASN.1	*/
/*					   list of file names		*/
/*  FILE_NAME fname_list[ num_of_fname ];				*/
  SD_END_STRUCT
  };
typedef struct storedom_req_info STOREDOM_REQ_INFO;

/* RESPONSE : NULL							*/

#ifndef MMS_LITE
MMSREQ_PEND *mp_storedom (ST_INT chan, STOREDOM_REQ_INFO *info);
ST_RET mp_storedom_resp (MMSREQ_IND *ind);
#else
ST_RET mpl_storedom (STOREDOM_REQ_INFO *info);
ST_RET mpl_storedom_resp (ST_UINT32 invoke);
#endif

/************************************************************************/
/* DELETE DOMAIN							*/
/************************************************************************/

/* REQUEST :								*/

struct deldom_req_info
  {
  ST_CHAR dname [MAX_IDENT_LEN +1];	/* domain name			*/
  };
typedef struct deldom_req_info DELDOM_REQ_INFO;

/* RESPONSE : NULL							*/

#ifndef MMS_LITE
MMSREQ_PEND *mp_deldom (ST_INT chan, DELDOM_REQ_INFO *info);
ST_RET mp_deldom_resp (MMSREQ_IND *ind);
#else
ST_RET mpl_deldom (DELDOM_REQ_INFO *info);
ST_RET mpl_deldom_resp (ST_UINT32 invoke);
#endif

/************************************************************************/
/* GET DOMAIN ATTRIBUTE 						*/
/************************************************************************/

/* REQUEST :								*/

struct getdom_req_info
  {
  ST_CHAR dname [MAX_IDENT_LEN +1];	/* domain name			*/
  };
typedef struct getdom_req_info GETDOM_REQ_INFO;

/* RESPONSE :								*/

struct getdom_resp_info
  {
  ST_INT     num_of_capab; 		/* number of capabilities	*/
  ST_BOOLEAN mms_deletable;		/* MMS deletable, no default	*/
  ST_BOOLEAN sharable;			/* boolean, no default		*/
  ST_INT     num_of_pinames;		/* number of pi names		*/
  ST_INT16   state;			/* 0 : non-existent		*/
					/* 1 : loaded			*/
					/* 2 : ready			*/
					/* 3 : in use			*/
					/* 4 : complete 		*/
					/* 5 : incomplete		*/
					/* 6-15 : d1-d8	(IS)		*/
  ST_INT8  upload_in_progress;		/* upload in progress		*/
/*			list of pointers to capabilities		*/
/*  ST_CHAR *capab_list [num_of_capab];					*/
/*			list of pointers to program invocation names	*/
/*  ST_CHAR *pinames_list [num_of_pinames];				*/
  SD_END_STRUCT
  };
typedef struct getdom_resp_info	GETDOM_RESP_INFO;

#ifndef MMS_LITE
MMSREQ_PEND *mp_getdom (ST_INT chan, GETDOM_REQ_INFO *info);
ST_RET mp_getdom_resp (MMSREQ_IND *ind, GETDOM_RESP_INFO *info);
#else
ST_RET mpl_getdom (GETDOM_REQ_INFO *info);
ST_RET mpl_getdom_resp (ST_UINT32 invoke, GETDOM_RESP_INFO *info);
#endif

#ifdef __cplusplus
}
#endif

#endif	/* #define MMS_PDOM_INCLUDED */

