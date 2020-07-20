/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2007, All Rights Reserved.		        */
/*									*/
/* MODULE NAME : mms_pcon.h						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This file has the data structures and function definitions	*/
/*	required to interface with MMS context management services	*/
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
/* 12/08/07  EJV     08	   ms_arname_to_appref: chg **appref_out to	*/
/*			     *appref.					*/ 
/* 07/13/05  MDE     07    Added defines for reject class and code	*/
/* 03/23/05  JRB     06    Include asn1r instead of suicacse because	*/
/*			   APP_REF moved to asn1r.			*/
/* 07/10/02  JRB     05    Clarify description of maxreq_calling/called.*/
/* 12/10/97  MDE     04    Changed max_nest to ST_INT8			*/
/* 11/06/97  MDE     03    Added suicacse.h, use APP_REF typedef	*/
/* 08/13/97  RKR     02    Format changes				*/
/* 07/29/97  DSF     01    Moved APP_REF to SUIC			*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#ifndef MMS_PCON_INCLUDED
#define MMS_PCON_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "mms_err.h"
#include "mms_mp.h"
#include "asn1r.h"	/* need APP_REF	*/

/************************************************************************/
/************************************************************************/
/* CONTEXT MANAGEMENT SERVICES						*/
/************************************************************************/
/* The structures below are used to pass CONTEXT MANAGEMENT information */
/* to/from the MMS primitives.						*/
/************************************************************************/

/************************************************************************/
/* INITIATE								*/
/************************************************************************/

struct init_rslt
  {
  ST_BOOLEAN accept_context;	/* SD_TRUE if not error			*/
  ST_BOOLEAN err_info_pres;	/* error information available		*/
  ERR_INFO   err;		/* otherwise look here 			*/
  };
typedef struct init_rslt INIT_RSLT;


/* This structure is used for non-core MMS-EASE P-Context's.		*/
/* It is attached to the primary initiate info structure as an array	*/

struct cs_init_info 
  {
  ST_UINT    p_context;		/* P-Context for this init		*/
  ST_BOOLEAN local_detail_pres;	/* flag if local_detail is present	*/
  ST_INT32   local_detail;	/* local detail				*/
  ST_INT16   maxreq_calling;	/* max outst. "ind" allowed at "calling" app*/
				/* DEBUG: chg name to max_ind_calling?	*/
  ST_INT16   maxreq_called;	/* max outst. "ind" allowed at "called" app*/
				/* DEBUG: chg name to max_ind_called?	*/
  ST_BOOLEAN max_nest_pres;  	/* indicates if max_nest is present	*/
  ST_INT8    max_nest;		/* maximum data nesting 		*/
  ST_INT     init_detail_len;	/* init detail 				*/
  ST_UCHAR   *init_detail;	/* this is an ASN.1 encoded sequence	*/
  INIT_RSLT  rslt;
  };
typedef struct cs_init_info  CS_INIT_INFO;


/* REQUEST & RESPONSE : 						*/

struct init_info
  {				/* Proposed (req) / Negiotiated (resp)	*/
  ST_BOOLEAN mms_p_context_pres;/* MMS Core context used		*/
  ST_BOOLEAN max_segsize_pres;	/* flag if max_segsize is present	*/
  ST_INT32   max_segsize;	/* maximum segment size 		*/
  ST_INT16   maxreq_calling;	/* max outst. "ind" allowed at "calling" app*/
				/* DEBUG: chg name to max_ind_calling?	*/
  ST_INT16   maxreq_called;	/* max outst. "ind" allowed at "called" app*/
				/* DEBUG: chg name to max_ind_called?	*/
  ST_BOOLEAN max_nest_pres;  	/* indicates if max_nest is present	*/
  ST_INT8    max_nest;		/* maximum data nesting 		*/
  ST_BOOLEAN mms_detail_pres;	/* mms init detail enable 		*/
  ST_INT16   version;		/* version				*/
  ST_UCHAR   param_supp[2];	/* parameter support options: bitstring	*/
  ST_UCHAR   serv_supp[11];	/* service support options: bitstring	*/
  INIT_RSLT  rslt;		/* success/failure & error info		*/
  ST_INT     num_cs_init;	/* number of CS init info's attached	*/
  ST_INT     core_position;	/* Position for CORE CONTEXT in list	*/
				/* (0 = first, num_cs_init = last, etc.	*/
/* followed by    CS_INIT_INFO cs_init[num_cs_init]			*/
  SD_END_STRUCT
  };		     
typedef struct init_info INIT_INFO;

#ifndef MMS_LITE
MMSREQ_PEND *mp_init (ST_INT chan, ST_CHAR *partner, INIT_INFO *info_ptr);
ST_RET mp_init_resp (MMSREQ_IND *indptr, INIT_INFO *info);
#else
ST_RET mpl_init         (INIT_INFO *info);
ST_RET mpl_cs_init      (CS_INIT_INFO *info);
ST_RET mpl_init_resp    (INIT_INFO *info);
ST_RET mpl_cs_init_resp (CS_INIT_INFO *info);
#endif

/************************************************************************/
/* CONCLUDE								*/
/************************************************************************/

/* REQUEST : NULL							*/
/* RESPONSE : NULL							*/

#ifndef MMS_LITE
MMSREQ_PEND *mp_conclude (ST_INT chan);
ST_RET mp_conclude_resp (MMSREQ_IND *ind);
#else
ST_RET mpl_conclude (ST_VOID);
ST_RET mpl_conclude_resp (ST_VOID);
#endif

/************************************************************************/
/* CANCEL								*/
/************************************************************************/

/* REQUEST : NULL							*/
/* RESPONSE : NULL							*/

#ifndef MMS_LITE
MMSREQ_PEND *mp_cancel (MMSREQ_PEND *req);
ST_RET mp_cancel_resp   (MMSREQ_IND *ind);
#else
ST_RET mpl_cancel        (ST_VOID);
ST_RET mpl_cancel_resp   (ST_VOID);
#endif

/************************************************************************/
/* REJECT								*/
/************************************************************************/


#define MMS_REJ_CLASS_CONFIRMED_REQUEST_PDU 				  1
#define MMS_REJ_CODE_CONFIRMED_REQUEST_PDU_OTHER			  0
#define MMS_REJ_CODE_CONFIRMED_REQUEST_PDU_UNRECOGNIZED_SERVICE		  1
#define MMS_REJ_CODE_CONFIRMED_REQUEST_PDU_UNRECOGNIZED_MODIFIER	  2
#define MMS_REJ_CODE_CONFIRMED_REQUEST_PDU_INVALID_INVOKEID		  3
#define MMS_REJ_CODE_CONFIRMED_REQUEST_PDU_INVALID_ARGUMENT		  4
#define MMS_REJ_CODE_CONFIRMED_REQUEST_PDU_INVALID_MODIFIER		  5
#define MMS_REJ_CODE_CONFIRMED_REQUEST_PDU_MAX_SERV_OUTSTANDING_EXCEEDED  6
#define MMS_REJ_CODE_CONFIRMED_REQUEST_PDU_MAX_RECURSION_EXCEEDED	  8
#define MMS_REJ_CODE_CONFIRMED_REQUEST_PDU_VALUE_OUT_OF_RANGE		  9

#define MMS_REJ_CLASS_CONFIRMED_RESPONSE_PDU 				  2
#define MMS_REJ_CODE_CONFIRMED_RESPONSE_PDU_OTHER			  0
#define MMS_REJ_CODE_CONFIRMED_RESPONSE_PDU_UNRECOGNIZED_SERVICE	  1
#define MMS_REJ_CODE_CONFIRMED_RESPONSE_PDU_INVALID_INVOKEID		  2
#define MMS_REJ_CODE_CONFIRMED_RESPONSE_PDU_INVALID_RESULT		  3
#define MMS_REJ_CODE_CONFIRMED_RESPONSE_PDU_MAX_RECURSION_EXCEEDED	  5
#define MMS_REJ_CODE_CONFIRMED_RESPONSE_PDU_VALUE_OUT_OF_RANGE		  6

#define MMS_REJ_CLASS_CONFIRMED_ERROR_PDU 				  3
#define MMS_REJ_CODE_CONFIRMED_ERROR_PDU_OTHER				  0
#define MMS_REJ_CODE_CONFIRMED_ERROR_PDU_UNRECOGNIZED_SERVICE		  1
#define MMS_REJ_CODE_CONFIRMED_ERROR_PDU_INVALID_INVOKEID		  2
#define MMS_REJ_CODE_CONFIRMED_ERROR_PDU_INVALID_SERVICEERROR		  3
#define MMS_REJ_CODE_CONFIRMED_ERROR_PDU_VALUE_OUT_OF_RANGE		  4

#define MMS_REJ_CLASS_UNCONFIRMED_PDU 					  4
#define MMS_REJ_CODE_UNCONFIRMED_PDU_OTHER				  0
#define MMS_REJ_CODE_UNCONFIRMED_PDU_UNRECOGNIZED_SERVICE		  1
#define MMS_REJ_CODE_UNCONFIRMED_PDU_INVALID_ARGUMENT			  2
#define MMS_REJ_CODE_UNCONFIRMED_PDU_MAX_RECURSION_EXCEEDED		  3
#define MMS_REJ_CODE_UNCONFIRMED_PDU_VALUE_OUT_OF_RANGE			  4

#define MMS_REJ_CLASS_PDU_ERROR 					  5
#define MMS_REJ_CODE_PDU_ERROR_UNKNOWN_PDU_TYPE				  0
#define MMS_REJ_CODE_PDU_ERROR_INVALID_PDU				  1
#define MMS_REJ_CODE_PDU_ERROR_ILLEGAL_ACSE_MAPPING			  2

#define MMS_REJ_CLASS_CANCEL_REQUEST_PDU 				  6
#define MMS_REJ_CODE_CANCEL_REQUEST_PDU_OTHER				  0
#define MMS_REJ_CODE_CANCEL_REQUEST_PDU_INVALID_INVOKEID		  1

#define MMS_REJ_CLASS_CANCEL_RESPONSE_PDU 				  7
#define MMS_REJ_CODE_CANCEL_RESPONSE_PDU_OTHER				  0
#define MMS_REJ_CODE_CANCEL_RESPONSE_PDU_INVALID_INVOKEID		  1

#define MMS_REJ_CLASS_CANCEL_ERROR_PDU 					  8
#define MMS_REJ_CODE_CANCEL_ERROR_PDU_OTHER				  0
#define MMS_REJ_CODE_CANCEL_ERROR_PDU_INVALID_INVOKEID			  1
#define MMS_REJ_CODE_CANCEL_ERROR_PDU_INVALID_SERVICEERROR		  2
#define MMS_REJ_CODE_CANCEL_ERROR_PDU_VALUE_OUT_OF_RANGE		  3

#define MMS_REJ_CLASS_CONCLUDE_REQUEST_PDU 				  9
#define MMS_REJ_CODE_CONCLUDE_REQUEST_PDU_OTHER				  0
#define MMS_REJ_CODE_CONCLUDE_REQUEST_PDU_INVALID_ARGUMENT		  1
									  
#define MMS_REJ_CLASS_CONCLUDE_RESPONSE_PDU 			         10
#define MMS_REJ_CODE_CONCLUDE_RESPONSE_PDU_OTHER		          0
#define MMS_REJ_CODE_CONCLUDE_RESPONSE_PDU_INVALID_RESULT	          1

#define MMS_REJ_CLASS_CONCLUDE_ERROR_PDU 			         11
#define MMS_REJ_CODE_CONCLUDE_ERROR_PDU_OTHER				  0
#define MMS_REJ_CODE_CONCLUDE_ERROR_PDU_INVALID_SERVICEERROR		  1
#define MMS_REJ_CODE_CONCLUDE_ERROR_PDU_VALUE_OUT_OF_RANGE		  2


struct reject_resp_info
  {
  ST_BOOLEAN detected_here; 	/* flag indicating where error occurred */
  ST_BOOLEAN invoke_known;	/* flag indicating if invoke ID known	*/
  ST_UINT32  invoke;		/* invoke ID of service rejected	*/
  ST_INT     pdu_type;		/* pdu type of service rejected 	*/
  ST_INT16   rej_class;		/* reject class 			*/
  ST_INT16   rej_code;		/* reject code				*/
  };
typedef struct reject_resp_info	REJECT_RESP_INFO;

#ifndef MMS_LITE
ST_RET mp_reject_resp (ST_INT chan, REJECT_RESP_INFO *info);
ST_RET mp_reject_ind  (MMSREQ_IND *ind,REJECT_RESP_INFO *info);
ST_RET mp_reject_conf (MMSREQ_PEND *req,REJECT_RESP_INFO *info);
#else
ST_RET mpl_reject_resp (REJECT_RESP_INFO *info);
#endif

/************************************************************************/
/* ABORT REQUEST FUNCTION						*/
/************************************************************************/

ST_RET mp_abort (ST_INT chan, ST_INT reason);

/************************************************************************/
/************************************************************************/
/* This function is used to convert convert an AR Name into an ASN.1    */
/* encoded ApplicationReference based on the TPY DIB information        */
/************************************************************************/

ST_RET ms_arname_to_asn1 (ST_CHAR *arname, ST_UCHAR *asn1_buf, ST_INT asn1_buf_len, 
				ST_UCHAR **asn1_out, ST_INT *asn1_len_out); 

/* This function is used to convert an ASN.1 encoded                    */
/* ApplicationReference	to the AR Name                                   */
ST_RET ms_asn1_to_arname (ST_CHAR *arname, ST_UCHAR *asn1, ST_INT asn1len);

/* This function is used to find an AR Name based on the app_ref        */
/* information.                                                         */
ST_RET ms_appref_to_arname (ST_CHAR *arname, APP_REF *appref);

/* This function is used to transition between an AR Name and its'      */
/* corresponding app_ref information.                                   */
ST_RET ms_arname_to_appref (ST_CHAR *arname, APP_REF *appref);

/* This function is used to convert to an ApplicationReference to ASN.1	*/
ST_RET ms_appref_to_asn1 (APP_REF *appref, ST_UCHAR *dest, ST_INT dest_len, 
				ST_UCHAR **asn1_out,  ST_INT *asn1_len_out);

/* This function is used to convert ASN.1 encoded ApplicationReference  */
/* data to an intermediate form.					*/
ST_RET ms_asn1_to_appref (APP_REF *appref, ST_UCHAR *asn1, ST_INT asn1len);


#ifdef __cplusplus
}
#endif

#endif	/* MMS_PCON_INCLUDED */

