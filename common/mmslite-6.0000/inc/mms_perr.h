/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2007, All Rights Reserved.		        */
/*									*/
/* MODULE NAME : mms_perr.h						*/
/* PRODUCT(S)  : MMSEASE						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains structure & function definitions used for	*/
/*	decoding and encoding an error response.			*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 09/05/07  EJV     03    Removed obsolete LLC30_LLP Type code.	*/
/* 07/02/98  NAV     02    Add MVE_JOURNAL_NAME error code			*/
/* 06/09/97  MDE     01    Fixed indenting 				*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#ifndef MMS_PERR_INCLUDED
#define MMS_PERR_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************/
/* Global MMS operation error code, written when error is detected in	*/
/* functions which return pointers.					*/
/************************************************************************/

extern ST_RET mms_op_err;

/************************************************************************/
/* Error Codes								*/
/* MMSEASE error codes start at E_MMS_GEN(found in glberror.h).	*/
/* When the return valus from a user accessable function is a pointer,	*/
/* an error is indicated by a pointer value of 0L. In this case the	*/
/* error code may be found in the global variable 'mms_op_err'.         */
/*									*/
/* Note that SUIC error codes are passed through, and are in the range	*/
/* 1 - 99. Refer to SUIC documentation for specific error definitions.	*/
/*									*/
/*									*/
/* Error codes returned from any request or response function		*/
/************************************************************************/

#define ME_CHAN_STATE 			0x6401 
#define ME_CHAN_NUM 			0x6402 
#define ME_TITLE_LEN 			0x6403
#define ME_SEND_SIZE  			0x6404
#define ME_REQ_PEND_COUNT 		0x6405 
#define ME_INACTIVE_IND 		0x6406 
#define ME_PARTNER_NAME 		0x6407 
#define ME_AR_NAME 			0x6408 
#define ME_OBJ_ID_ERR 			0x6409 
#define ME_SEND_ERROR 			0x640A 
#define ME_ACSE_CHAN_RANGE		0x640B
#define ME_LLC_CHAN_RANGE		0x640D
#define ME_LLP_TYPE			0x640E
#define ME_TRANS_ID_UNAVAILABLE		0x640F
#define ME_CHAN_TYPE			0x6410
#define ME_TOO_MANY_CONTEXTS		0x6411
#define ME_UNKNOWN_P_CONTEXT		0x6412
#define ME_P_CONTEXT_ERROR		0x6413
#define ME_ASN1_ENCODE_OVERRUN		0x6414

/************************************************************************/
/* Error codes returned from mp_xxx_req functions			*/
/* Values begin from E_MMS_PPI(found in glberror.h).                  */
/************************************************************************/

#define ME_QUEFULL 	0x6501

/************************************************************************/
/* Error codes found in resp_err					*/
/* Values begin from E_MMS_RESP(found in glberror.h).                 */
/************************************************************************/

#define CNF_RESP_OK 	 		0
#define CNF_PARSE_ERR			0x6601
#define CNF_REJ_ERR			0x6602
#define CNF_ERR_OK			0x6603
#define CNF_DISCONNECTED		0x6604
#define CNF_CHAN_OP_ERR			0x6605
#define CNF_CANST_ERR			0x6606
#define CNF_ASS_REQ_REJECTED 		0x6607
#define CNF_ASS_RESP_PARAM   		0x6608
#define CNF_ASS_USER_REJ_CONF		0x6609
#define CNF_REM_FOPEN			0x660A
#define CNF_INIT_PARAM			0x660B
#define CNF_REM_FREAD			0x660C
#define CNF_LOC_FWRITE			0x660D
#define CNF_REM_FCLOSE			0x660E
#define CNF_LOC_FCLOSE			0x660F
#define CNF_MVREAD_RESP_PARAM		0x6610
#define CNF_VM_RESP_ERR			0x6611
#define CNF_LLC_SEND_ERROR	  	0x6612
#define	CNF_REQ_NOT_DONE		0x6613

/************************************************************************/
/* Error codes returned from 'mv_xxxx_resp' functions.                  */
/* Values begin from E_MMS_VMI(found in glberror.h).			*/
/************************************************************************/

#define MVE_VARNAME			0x6701 
#define MVE_TYPENAME 			0x6702 
#define MVE_ADDR  			0x6703 
#define MVE_FOPEN  			0x6704 
#define MVE_REM_FILE_COUNT  		0x6705 
#define MVE_LOC_FILE_COUNT  		0x6706 
#define MVE_FILE_REFNUM  		0x6707 
#define MVE_FRENAME  			0x6708 
#define MVE_FDELETE  			0x6709 
#define MVE_FDIR  			0x670A 
#define MVE_LOC_FOPEN  			0x670B 
#define MVE_CANCEL_STATE  		0x670C 
#define MVE_DOM_ERR  			0x670D 
#define MVE_RT_TYPE  			0x670E 
#define MVE_DOM_STATE  			0x670F 
#define MVE_AT_UPPER_LIMIT  		0x6710 
#define MVE_DATA_CONVERT  		0x6711 
#define MVE_VM_SERVICE_NOTSUPP  	0x6712 
#define MVE_MVWRITE_REQ_PARAM  		0x6713 
#define MVE_DOMAIN_NAME  		0x6714 
#define MVE_OBJECT_SCOPE  		0x6715 
#define MVE_VAR_LIST  			0x6716 
#define MVE_VAR_NUMBER  		0x6717
#define MVE_NOT_NAMED_VAR  		0x6718
#define MVE_WRONG_OP  			0x6719 
#define MVE_MVINFO_ACCESS_ERR  		0x671A 
#define MVE_TYPEDEF_LEN0	  	0x671B 
#define MVE_TYPEDEF_SYM_GT  		0x671C 
#define MVE_TYPEDEF_SYM_LT  		0x671D 
#define MVE_TYPEDEF_OBJNAME  		0x671E 
#define MVE_TYPEDEF_SYM_RBRACE  	0x671F 
#define MVE_TYPEDEF_SYM_LBRACE  	0x6720 
#define MVE_TYPEDEF_SYM_COMMA  		0x6721 
#define MVE_TYPEDEF_SYM_RPAREN  	0x6722 
#define MVE_TYPEDEF_SYM_LPAREN  	0x6723 
#define MVE_TYPEDEF_SYM_P  		0x6724 
#define MVE_TYPEDEF_SYM_PLUS  		0x6725 
#define MVE_TYPEDEF_SYM_MINUS  		0x6726 
#define MVE_TYPEDEF_SYM_T  		0x6727 
#define MVE_TYPEDEF_BADLEN3  		0x6728 
#define MVE_TYPEDEF_BADLEN4  		0x6729 
#define MVE_TYPEDEF_BADLEN5  		0x672A 
#define MVE_TYPEDEF_BADLEN7  		0x672B 
#define MVE_TYPEDEF_BADTYPE  		0x672C 
#define MVE_TYPEDEF_SYM_COLON  		0x672D 
#define MVE_TYPEDEF_SYM_RBRACKET  	0x672E 
#define MVE_TYPEDEF_SYM_LBRACKET  	0x672F 
#define MVE_TYPEDEF_NUM_ELTS  		0x6730 
#define MVE_TYPEDEF_SYM_DIGIT  		0x6731 
#define MVE_TYPEDEF_SYM_OTHER  		0x6732 
#define MVE_DATA_SPACE  		0x6733 
#define MVE_VM_REQ_ERR			0x6734
#define MVE_VMD_NOT_EMPTY		0x6735
#define MVE_BAD_APP_REF_FORM		0x6736
#define MVE_ASN1_TO_RT			0x6737
#define MVE_ASN1_ENCODE_ERR		0x6738
#define MVE_ASN1_DECODE_ERR		0x6739
#define MVE_FP_SAVE_REQ_ERR		0x673A
#define MVE_FP_REQ_DONE_ERR		0x673B
#define MVE_LLP_DIB_FOPEN_ERR		0x673C
#define MVE_ADD_APPREF_ERR		0x673D
#define MVE_RESP_NOT_SUPP		0x673E
#define MVE_REQ_NOT_SUPP		0x673F
#define MVE_INVALID_PDU			0x6740
#define MVE_DOM_DISCARDED  		0x6741 
#define MVE_DOM_PROTECTED  		0x6742
#define MVE_DOM_DEL_ERR  		0x6743
#define MVE_DEL_PI_ERR  		0x6744
#define MVE_RTAA_TYPE			0x6745
#define MVE_RTAA_SIZE			0x6746
#define MVE_AA_SELECT			0x6747
#define MVE_INVALID_ADL			0x6748
#define MVE_DATA_TO_RT			0x6749
#define MVE_LOC_FILE_READ  		0x6750 

/************************************************************************/
/* Errors related to VM Program Invocation handling			*/

#define PI_NAME 			0x6770
#define PI_STATE 			0x6771	
#define PI_NOT_REUSABLE 		0x6772
#define PI_NOT_DELETABLE 		0x6773
#define ME_PI_PROTECTION 		0x6774
#define ME_PI_INVALID_ID 		0x6775
				     
/************************************************************************/
/* Errors related to VM Journal handling				*/

#define MVE_JOURNAL_NAME		0x6776
#define MVE_INVALID_DOMAIN_OBJ		0x6777

/************************************************************************/
/* ACSE ERROR CODES							*/
/* The codes below are used in the info.ACSE.err_code in the case where */
/* the event is an ACSE_ERROR. The user defined function 		*/
/* 'u_llp_error_ind' is called with these codes when this occurs.	*/					
/* Values begin from E_LLP_ACSE(found in glberror.h).                 */
/************************************************************************/

#define ACSE_RECEIVE_ERR 	 	0x6901L
#define ACSE_LISTEN_ERR 	 	0x6902L
#define ACSE_DISCONNECT_RCV_ERR  	0x6903L
#define ACSE_STOP_LISTEN_ERR	 	0x6904L
#define ACSE_ASS_REQ_ERR 	 	0x6905L
#define ACSE_ASS_RESP_ERR 	 	0x6906L
#define ACSE_REL_RESP_ERR 	 	0x6907L
#define ACSE_REL_REQ_ERR 	 	0x6908L
#define ACSE_SEND_REQ_ERR 	 	0x6909L
#define ACSE_SET_DEBUG_ERR 	 	0x690AL
#define ACSE_ABORT_ERR	 	 	0x690BL


/************************************************************************/
/* MMS Exception Codes							*/
/* These codes are passed to the user when the user exception function	*/
/* (u_mmsexcept_ind) is called.						*/
/* Values begin from E_MMS_EXCPT(found in glberror.h).                */
/************************************************************************/

#define MX_REJECT 			0x6801
#define MX_BOARD_ERR 			0x6802
#define MX_LLP_EVENT_ERROR  		0x6803
#define MX_MEMORY_ALLOC 		0x6804
#define MX_RUNTIME_TDEF 		0x6805
#define MX_LLP_QUE_OVERFLOW 		0x6806
#define MX_MMSIND_QUE 			0x6807
#define MX_MMSCONF_QUE 			0x6808
#define MX_INTERNAL_ERROR 		0x6809
#define MX_LLP_ADD_INFO_ALLOC  		0x680A


#ifdef __cplusplus
}
#endif

#endif	/* #define MMS_PERR_INCLUDED */



