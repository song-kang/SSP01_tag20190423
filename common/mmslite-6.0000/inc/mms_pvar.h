/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 1997, All Rights Reserved.		        */
/*									*/
/* MODULE NAME : mms_pvar.h						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This file has the data structures and function definitions	*/
/*	required to interface with MMS variable access services		*/
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
/* 12/04/06  JRB     03    Add ARE_OBJ_VALUE_INVALID (new in MMS V2).	*/
/* 12/12/02  JRB     02    Add ASN1_ENC_CTXT arg to mpl_info		*/ 
/* 08/13/97  RKR     01    format changes				*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#ifndef MMS_PVAR_INCLUDED
#define MMS_PVAR_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "mms_mp.h"

/************************************************************************/
/************************************************************************/
/* DEFINES FOR THE VARIABLE ACCESS SERVICES				*/
/************************************************************************/

/* defines to be used for addr_tag					*/
#define NUM_ADDR	0		/* numeric address		*/
#define SYM_ADDR	1		/* symbolic address		*/
#define UNCON_ADDR	2		/* unconstrained address	*/

/* #defines to be used for var_spec_tag					*/
#define VA_SPEC_NAMED		0
#define VA_SPEC_ADDRESSED	1
#define VA_SPEC_DESCRIBED	2
#define VA_SPEC_SCATTERED	3
#define VA_SPEC_INVALIDATED	4

/* #defines to be used for var_acc_tag					*/
#define VAR_ACC_VARLIST		0	/* list of variables		*/
#define VAR_ACC_NAMEDLIST	1	/* variable list name		*/

/* defines to be used for 'access result'				*/
#define ACC_RSLT_SUCCESS	1
#define ACC_RSLT_FAILURE	0

/* defines to be used for DataAccessError codes				*/
#define	ARE_OBJ_INVALIDATED		0
#define	ARE_HW_FAULT			1
#define	ARE_TEMP_UNAVAIL		2
#define	ARE_OBJ_ACCESS_DENIED		3
#define	ARE_OBJ_UNDEFINED		4
#define	ARE_INVAL_ADDR			5
#define	ARE_TYPE_UNSUPPORTED		6
#define	ARE_TYPE_INCONSISTENT		7
#define ARE_OBJ_ATTR_INCONSISTENT	8
#define ARE_OBJ_ACC_UNSUPPORTED		9
#define ARE_OBJ_NONEXISTENT		10
#define ARE_OBJ_VALUE_INVALID		11

/* #defines to be used for 'resp_tag' (write response)			*/
#define WR_RSLT_SUCCESS		1
#define WR_RSLT_FAILURE		0

/* #defines to be used for 'scope' selector (used for delete var, 	*/
/* varlist, type requests)						*/
#define SPECIFIC_SCOPE		0
#define ALL_AA_SCOPE		1
#define ALL_DOM_SCOPE		2
#define ALL_VMD_SCOPE		3


/************************************************************************/
/************************************************************************/
/* The structures below are used to pass VARIABLE ACCESS information	*/
/* to/from the MMS primitives.						*/
/************************************************************************/

struct unconst_addr
  {
  ST_INT   unc_len;			/* unconstrained address length */
  ST_UCHAR *unc_ptr;			/* unconstrained address ptr	*/
  SD_END_STRUCT
  };
typedef struct unconst_addr UNCONST_ADDR;

struct var_acc_addr
  {
  ST_INT16 addr_tag;			/* address tag			*/
  union
    {
    ST_UINT32	  num_addr;		/* numeric address		*/
    ST_CHAR	  *sym_addr;		/* symbolic address		*/
    UNCONST_ADDR  unc_addr;		/* unconstrained address 	*/
    } addr;
  };
typedef struct var_acc_addr VAR_ACC_ADDR;

struct var_acc_data
  {
  ST_INT len;				/* len of variable access data	*/
  ST_UCHAR *data; 			/* ptr to variable access data	*/
  };
typedef struct var_acc_data VAR_ACC_DATA;

struct var_acc_tspec
  {
  ST_INT len;				/* len of type specification	*/
  ST_UCHAR *data;			/* ptr to type specification	*/
  };
typedef struct var_acc_tspec VAR_ACC_TSPEC;

struct scattered_access
  {
  ST_INT   len;				/* len of scattered access descr*/
  ST_UCHAR *data; 			/* ptr to scattered access descr*/
  };
typedef struct scattered_access	SCATTERED_ACCESS;

struct alternate_access
  {
  ST_INT   len;				/* len of alternate access data */
  ST_UCHAR *data; 			/* ptr to alternate access data */
  };
typedef struct alternate_access	ALTERNATE_ACCESS;

struct variable_descr
  {
  VAR_ACC_ADDR	address;		/* variable address		*/
  VAR_ACC_TSPEC	type;			/* variable type		*/
  };
typedef struct variable_descr VARIABLE_DESCR;

struct variable_spec
  {
  ST_INT16 var_spec_tag;		/* variable specification tag	*/
  union
    {
    OBJECT_NAME	     name;		/* name 			*/
    VAR_ACC_ADDR     address;		/* address			*/
    VARIABLE_DESCR   var_descr;		/* variable description 	*/
    SCATTERED_ACCESS sa_descr;		/* scattered access description */
    } vs;
  };
typedef struct variable_spec VARIABLE_SPEC;

struct variable_list
  {
  VARIABLE_SPEC	   var_spec;		/* variable access specification*/
  ST_BOOLEAN 	   alt_access_pres;	/* alternate access present	*/
  ALTERNATE_ACCESS alt_access;		/* alternate access		*/
  };
typedef struct variable_list VARIABLE_LIST;

struct var_acc_spec
  {
  ST_INT16    var_acc_tag; 		/* variable access tag		*/
  OBJECT_NAME vl_name;			/* variable list name		*/
  ST_INT      num_of_variables;		/* number of variables		*/
/*					   list of variables		*/
/*  VARIABLE_LIST var_list [num_of_variables];				*/
  SD_END_STRUCT
  };
typedef struct var_acc_spec VAR_ACC_SPEC;

struct access_result
  {
  ST_INT16     acc_rslt_tag; 		/* access result tag		*/
  ST_INT16     failure;			/* data access error		*/
  VAR_ACC_DATA va_data;			/* success data 		*/
  };
typedef struct access_result ACCESS_RESULT;

struct write_result
  {
  ST_INT16    resp_tag;			/* response tag 		*/
  ST_INT16    failure;			/* data access error		*/
  SD_END_STRUCT
  };
typedef struct write_result WRITE_RESULT;


/************************************************************************/
/************************************************************************/
/* VARIABLE READ							*/
/************************************************************************/

/* REQUEST :								*/

struct read_req_info
  {
  ST_BOOLEAN    spec_in_result;		/* specification with result,	*/
					/*  default to false		*/
  VAR_ACC_SPEC	va_spec;		/* variable access spec 	*/
/*					     which "includes"           */
/*					     a list of variables	*/
/*  VARIABLE_LIST var_list [va_spec.num_of_variables];			*/
/*  SD_END_STRUCT */
  };
typedef struct read_req_info READ_REQ_INFO;

/* RESPONSE :								*/

struct read_resp_info
  {
  ST_BOOLEAN 	va_spec_pres;		/* variable access spec present */
  ST_INT 	num_of_acc_result;	/* number of access result	*/
  ACCESS_RESULT *acc_rslt_list;		/* ptr to list of access result */
  VAR_ACC_SPEC	va_spec;		/* variable access spec 	*/
/*					     which "includes"           */
/*					     a list of variables	*/
/*  VARIABLE_LIST var_list [va_spec.num_of_variables];			*/
/*					   list of access result	*/
/*  ACCESS_RESULT acc_result_list [num_of_acc_result];			*/
/*  SD_END_STRUCT */
  };
typedef struct read_resp_info READ_RESP_INFO;

#ifndef MMS_LITE
MMSREQ_PEND *mp_read (ST_INT chan, READ_REQ_INFO *info);
ST_RET mp_read_resp (MMSREQ_IND *ind, READ_RESP_INFO *info);
#else
ST_RET mpl_read (READ_REQ_INFO *info);
ST_RET mpl_read_resp (ST_UINT32 invoke, READ_RESP_INFO *info);
#endif

/************************************************************************/
/* VARIABLE WRITE							*/
/************************************************************************/

/* REQUEST :								*/

struct write_req_info
  {
  ST_INT 	num_of_data;   		/* number of data		*/
  VAR_ACC_DATA  *va_data;		/* ptr to list of variable data */
  VAR_ACC_SPEC	va_spec;		/* variable access spec 	*/
/*					     which "includes"           */
/*					     a list of variables	*/
/*  VARIABLE_LIST var_list [va_spec.num_of_variables];			*/
/*					   list of variable data	*/
/*  VAR_ACC_DATA  var_data_list [num_of_data];				*/
  };
typedef struct write_req_info WRITE_REQ_INFO;

/* RESPONSE :								*/

struct write_resp_info
  {
  ST_INT num_of_result;
/*	WRITE_RESULT wr_result[num_of_result]				*/
  SD_END_STRUCT
  };
/* Where:								*/
/*	num_of_result	=  The number of members in wr_result. This	*/
/*			   number should match the number of		*/
/*			   variables included in the Write request.	*/
/*									*/
/*	wr_result	=  Array of structures of type write_result	*/
/*			   containing the results of the write		*/
/*			   (success or failure) for each variable	*/
/*			   written.  The result array members should	*/
/*			   align with the var_list members that were	*/
/*			   included in the Write request.		*/
typedef struct write_resp_info WRITE_RESP_INFO;

#ifndef MMS_LITE
MMSREQ_PEND *mp_write (ST_INT chan, WRITE_REQ_INFO *info);
ST_RET mp_write_resp (MMSREQ_IND *ind, WRITE_RESP_INFO *info);
#else
ST_RET mpl_write (WRITE_REQ_INFO *info);
ST_RET mpl_write_resp (ST_UINT32 invoke, WRITE_RESP_INFO *info);
#endif

/************************************************************************/
/* INFORMATION REPORT							*/
/************************************************************************/

/* REQUEST :								*/

struct info_req_info
  {
  ST_INT 	num_of_acc_result;	/* number of access result	*/
  ACCESS_RESULT *acc_rslt_list;		/* ptr to list of access result */
  VAR_ACC_SPEC	va_spec;		/* variable access spec 	*/
/*					     which "includes"           */
/*					     a list of variables	*/
/*  VARIABLE_LIST var_list [va_spec.num_of_variables];			*/
/*					   list of access result	*/
/*  ACCESS_RESULT acc_result_list [num_of_acc_result];			*/
/*  SD_END_STRUCT */
  };
typedef struct info_req_info INFO_REQ_INFO;

/* No Response : (unsolicited)						*/

#ifndef MMS_LITE
ST_RET mp_info (ST_INT chan, INFO_REQ_INFO *info);
#else
ST_RET mpl_info (ASN1_ENC_CTXT *aCtx, INFO_REQ_INFO *info);
#endif

/************************************************************************/
/* DEFINE VARIABLE NAME 						*/
/************************************************************************/

/* REQUEST :								*/

struct defvar_req_info
  {
  OBJECT_NAME   name;			/* object name			*/
  VAR_ACC_ADDR  address; 		/* address			*/
  ST_BOOLEAN 	type_spec_pres;		/* type specification present	*/
  VAR_ACC_TSPEC type_spec;		/* type specification		*/
  };
typedef struct defvar_req_info DEFVAR_REQ_INFO;


/* RESPONSE : NULL							*/

#ifndef MMS_LITE
MMSREQ_PEND *mp_defvar (ST_INT chan, DEFVAR_REQ_INFO *info);
ST_RET mp_defvar_resp (MMSREQ_IND *ind);
#else
ST_RET mpl_defvar (DEFVAR_REQ_INFO *info);
ST_RET mpl_defvar_resp (ST_UINT32 invoke);
#endif

/************************************************************************/
/* GET VARIABLE NAME DEFINITION 					*/
/************************************************************************/

/* REQUEST :								*/

#define GETVAR_NAME 0
#define GETVAR_ADDR 1

struct getvar_req_info
  {
  ST_INT16     req_tag;			/* request tag			*/
					/*   0 : object name request	*/
					/*   1 : address request	*/
  OBJECT_NAME  name;			/* object name			*/
  VAR_ACC_ADDR address;			/* address			*/
  };
typedef struct getvar_req_info GETVAR_REQ_INFO;

/* RESPONSE :								*/

struct getvar_resp_info
  {
  ST_BOOLEAN 	mms_deletable; 		/* MMS deletable		*/
  ST_BOOLEAN 	address_pres;		/* address present		*/
  VAR_ACC_ADDR  address; 		/* address			*/
  VAR_ACC_TSPEC type_spec;		/* type specification		*/
  };
typedef struct getvar_resp_info	GETVAR_RESP_INFO;

#ifndef MMS_LITE
MMSREQ_PEND *mp_getvar (ST_INT chan, GETVAR_REQ_INFO *info);
ST_RET mp_getvar_resp (MMSREQ_IND *ind, GETVAR_RESP_INFO *info);
#else
ST_RET mpl_getvar (GETVAR_REQ_INFO *info);
ST_RET mpl_getvar_resp (ST_UINT32 invoke, GETVAR_RESP_INFO *info);
#endif

/************************************************************************/
/* DELETE VARIABLE NAME 						*/
/************************************************************************/

/* REQUEST :								*/

#define DELVAR_SPEC	0
#define DELVAR_AA	1
#define DELVAR_DOM	2
#define DELVAR_VMD	3

struct delvar_req_info
  {
  ST_INT16   scope;			/* scope of delete (see above)	*/
  ST_BOOLEAN dname_pres;		/* domain name present		*/
  ST_CHAR    dname [MAX_IDENT_LEN+1]; 	/* domain name			*/
  ST_BOOLEAN vnames_pres;		/* variable names present	*/
  ST_INT     num_of_vnames; 		/* number of variable names	*/
/*					   list of variable names	*/
/*  OBJECT_NAME vname_list [num_of_vnames];				*/
  SD_END_STRUCT
  };
typedef struct delvar_req_info DELVAR_REQ_INFO;


/* RESPONSE : 								*/

struct delvar_resp_info
  {
  ST_UINT32 num_matched;			/* Number matched		*/
  ST_UINT32 num_deleted;			/* Number deleted		*/
  };
typedef struct delvar_resp_info	DELVAR_RESP_INFO;

#ifndef MMS_LITE
MMSREQ_PEND *mp_delvar (ST_INT chan, DELVAR_REQ_INFO *info);
ST_RET mp_delvar_resp (MMSREQ_IND *ind,DELVAR_RESP_INFO *info);
#else
ST_RET mpl_delvar (DELVAR_REQ_INFO *info);
ST_RET mpl_delvar_resp (ST_UINT32 invoke, DELVAR_RESP_INFO *info);
#endif

/************************************************************************/
/* DEFINE SCATTERED ACCESS 	 					*/
/************************************************************************/

/* REQUEST :								*/

struct defscat_req_info
  {
  OBJECT_NAME      sa_name; 		/* scattered access name	*/
  SCATTERED_ACCESS sa_descr;		/* scattered access description */
  };
typedef struct defscat_req_info	DEFSCAT_REQ_INFO;

/* RESPONSE :  NULL							*/

#ifndef MMS_LITE
MMSREQ_PEND *mp_defscat (ST_INT chan, DEFSCAT_REQ_INFO *info);
ST_RET mp_defscat_resp (MMSREQ_IND *info);
#else
ST_RET mpl_defscat (DEFSCAT_REQ_INFO *info);
ST_RET mpl_defscat_resp (ST_UINT32 invoke);
#endif

/************************************************************************/
/* GET SCATTERED ACCESS ATTRIBUTES 					*/
/************************************************************************/

/* REQUEST :								*/

struct getscat_req_info
  {
  OBJECT_NAME      sa_name; 		/* scattered access name	*/
  };
typedef struct getscat_req_info	GETSCAT_REQ_INFO;

/* RESPONSE :								*/

struct getscat_resp_info
  {
  ST_BOOLEAN 	   mms_deletable; 	/* MMS deletable		*/
  SCATTERED_ACCESS sa_descr;		/* scattered access description */
  };
typedef struct getscat_resp_info GETSCAT_RESP_INFO;

#ifndef MMS_LITE
MMSREQ_PEND *mp_getscat (ST_INT chan, GETSCAT_REQ_INFO *info);
ST_RET mp_getscat_resp (MMSREQ_IND *ind, GETSCAT_RESP_INFO *info);
#else
ST_RET mpl_getscat (GETSCAT_REQ_INFO *info);
ST_RET mpl_getscat_resp (ST_UINT32 invoke, GETSCAT_RESP_INFO *info);
#endif

/************************************************************************/
/* DEFINE NAMED VARIABLE LIST 	 					*/
/************************************************************************/

/* REQUEST :								*/

struct defvlist_req_info
  {
  OBJECT_NAME vl_name; 			/* variable list name		*/
  ST_INT      num_of_variables;		/* number of variables		*/
/*					   list of variables		*/
/*  VARIABLE_LIST var_list [num_of_variables];				*/
  SD_END_STRUCT
  };
typedef struct defvlist_req_info DEFVLIST_REQ_INFO;

/* RESPONSE :  NULL							*/

#ifndef MMS_LITE
MMSREQ_PEND *mp_defvlist (ST_INT chan, DEFVLIST_REQ_INFO *info);
ST_RET mp_defvlist_resp (MMSREQ_IND *info);
#else
ST_RET mpl_defvlist (DEFVLIST_REQ_INFO *info);
ST_RET mpl_defvlist_resp (ST_UINT32 invoke);
#endif

/************************************************************************/
/* GET NAMED VARIABLE LIST ATTRIBUTES 					*/
/************************************************************************/

/* REQUEST :								*/

struct getvlist_req_info
  {
  OBJECT_NAME vl_name; 			/* variable list name		*/
  };
typedef struct getvlist_req_info GETVLIST_REQ_INFO;

/* RESPONSE :  								*/

struct getvlist_resp_info
  {
  ST_BOOLEAN mms_deletable; 		/* MMS deletable		*/
  ST_INT     num_of_variables;		/* number of variables		*/
/*					   list of variables		*/
/*  VARIABLE_LIST var_list [num_of_variables];				*/
  SD_END_STRUCT
  };
typedef struct getvlist_resp_info GETVLIST_RESP_INFO;

#ifndef MMS_LITE
MMSREQ_PEND *mp_getvlist (ST_INT chan, GETVLIST_REQ_INFO *info);
ST_RET mp_getvlist_resp (MMSREQ_IND *ind, GETVLIST_RESP_INFO *info);
#else
ST_RET mpl_getvlist (GETVLIST_REQ_INFO *info);
ST_RET mpl_getvlist_resp (ST_UINT32 invoke, GETVLIST_RESP_INFO *info);
#endif

/************************************************************************/
/* DELETE NAMED VARIABLE LIST 	 					*/
/************************************************************************/

/* REQUEST :								*/

#define DELVL_SPEC	0
#define DELVL_AA	1
#define DELVL_DOM	2
#define DELVL_VMD	3

struct delvlist_req_info
  {
  ST_INT16   scope;   			/* scope of delete (see above)	*/
  ST_BOOLEAN dname_pres; 		/* domain name present		*/
  ST_CHAR    dname [MAX_IDENT_LEN+1]; 	/* domain name			*/
  ST_BOOLEAN vnames_pres;		/* variable names present	*/
  ST_INT     num_of_vnames;		/* number of var list names	*/
/*					   list of var list names	*/
/*  OBJECT_NAME  vname_list [num_of_vnames];				*/
  SD_END_STRUCT
  };
typedef struct delvlist_req_info DELVLIST_REQ_INFO;

/* RESPONSE :  								*/

struct delvlist_resp_info
  {
  ST_UINT32 num_matched;			/* Number matched		*/
  ST_UINT32 num_deleted;			/* Number deleted		*/
  };
typedef struct delvlist_resp_info DELVLIST_RESP_INFO;

#ifndef MMS_LITE
MMSREQ_PEND *mp_delvlist (ST_INT chan, DELVLIST_REQ_INFO *info);
ST_RET mp_delvlist_resp (MMSREQ_IND *ind, DELVLIST_RESP_INFO *info);
#else
ST_RET mpl_delvlist (DELVLIST_REQ_INFO *info);
ST_RET mpl_delvlist_resp (ST_UINT32 invoke, DELVLIST_RESP_INFO *info);
#endif

/************************************************************************/
/* DEFINE TYPE								*/
/************************************************************************/

/* REQUEST :								*/

struct deftype_req_info
  {
  OBJECT_NAME   type_name;		/* object name			*/
  VAR_ACC_TSPEC type_spec;		/* type specification		*/
  };
typedef struct deftype_req_info	DEFTYPE_REQ_INFO;

/* RESPONSE : NULL							*/

#ifndef MMS_LITE
MMSREQ_PEND *mp_deftype (ST_INT chan, DEFTYPE_REQ_INFO *info);
ST_RET mp_deftype_resp (MMSREQ_IND *ind);
#else
ST_RET mpl_deftype (DEFTYPE_REQ_INFO *info);
ST_RET mpl_deftype_resp (ST_UINT32 invoke);
#endif

/************************************************************************/
/* GET TYPE NAME DEFINITION						*/
/************************************************************************/

/* REQUEST :								*/

struct gettype_req_info
  {
  OBJECT_NAME  type_name;	/* object name			*/
  };
typedef struct gettype_req_info	GETTYPE_REQ_INFO;

/* RESPONSE :								*/

struct gettype_resp_info
  {
  ST_BOOLEAN    mms_deletable; 		/* MMS deletable		*/
  VAR_ACC_TSPEC type_spec;		/* type specification		*/
  };
typedef struct gettype_resp_info GETTYPE_RESP_INFO;
      
#ifndef MMS_LITE
MMSREQ_PEND *mp_gettype (ST_INT chan, GETTYPE_REQ_INFO *info);
ST_RET mp_gettype_resp (MMSREQ_IND *ind, GETTYPE_RESP_INFO *info);
#else
ST_RET mpl_gettype (GETTYPE_REQ_INFO *info);
ST_RET mpl_gettype_resp (ST_UINT32 invoke, GETTYPE_RESP_INFO *info);
#endif

/************************************************************************/
/* DELETE TYPE NAME							*/
/************************************************************************/

/* REQUEST :								*/

#define DELTYPE_SPEC	0
#define DELTYPE_AA	1
#define DELTYPE_DOM	2
#define DELTYPE_VMD	3

struct deltype_req_info
  {
  ST_INT16   scope; 			/* scope of delete (see above)	*/
  ST_BOOLEAN dname_pres; 		/* domain name present		*/
  ST_CHAR    dname [MAX_IDENT_LEN+1]; 	/* domain name			*/
  ST_BOOLEAN tnames_pres;		/* type names present		*/
  ST_INT     num_of_tnames; 		/* number of type names 	*/
/*					   list of type names		*/
/*  OBJECT_NAME tname_list [num_of_tnames];				*/
  SD_END_STRUCT
  };
typedef struct deltype_req_info	DELTYPE_REQ_INFO;

/* RESPONSE :   							*/

struct deltype_resp_info
  {
  ST_UINT32 num_matched;			/* Number matched		*/
  ST_UINT32 num_deleted;			/* Number deleted		*/
  };
typedef struct deltype_resp_info DELTYPE_RESP_INFO;

#ifndef MMS_LITE
MMSREQ_PEND *mp_deltype (ST_INT chan, DELTYPE_REQ_INFO *info);
ST_RET mp_deltype_resp (MMSREQ_IND *ind, DELTYPE_RESP_INFO *info);
#else
ST_RET mpl_deltype (DELTYPE_REQ_INFO *info);
ST_RET mpl_deltype_resp (ST_UINT32 invoke, DELTYPE_RESP_INFO *info);
#endif

#ifdef __cplusplus
}
#endif

#endif	/* #define MMS_PVAR_INCLUDED */
