/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 1997, All Rights Reserved.		        */
/*									*/
/* MODULE NAME : mms_pvmd.h						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This file has the data structures and function definitions	*/
/*	required to interface with MMS vmd management services 		*/
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
/* 12/12/02  JRB     02    Add ASN1_ENC_CTXT arg to mpl_ustatus		*/ 
/* 09/09/97  MDE     02    Added 'mms_class' defines			*/
/* 08/14/97  RKR     01    Format changes				*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#ifndef MMS_PVMD_INCLUDED
#define MMS_PVMD_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "mms_mp.h"

/************************************************************************/
/************************************************************************/
/* VMD MANAGEMENT SERVICES						*/
/************************************************************************/
/* The structures below are used to pass VMD MANAGEMENT information	*/
/* to/from the MMS primitives.						*/
/************************************************************************/

/* Defines for 'mms_class' for GNL and RENAME */
#define MMS_CLASS_VAR		0
#define MMS_CLASS_SCAT_ACC	1
#define MMS_CLASS_VARLIST	2
#define MMS_CLASS_TYPE		3
#define MMS_CLASS_SEM		4
#define MMS_CLASS_EV_COND	5
#define MMS_CLASS_EV_ACT	6
#define MMS_CLASS_EV_ENROLL	7
#define MMS_CLASS_JOU		8
#define MMS_CLASS_DOM		9
#define MMS_CLASS_PI	       10
#define MMS_CLASS_OPER_STA     11

/************************************************************************/
/************************************************************************/
/* STATUS 								*/
/************************************************************************/

/* REQUEST :								*/

#define MAX_STAT_DTL_LEN        16

struct status_req_info
  {
  ST_BOOLEAN extended;		/* flag indicating if extended status	*/
  };				/* is desired				*/
typedef struct status_req_info STATUS_REQ_INFO;

/* RESPONSE :								*/

struct status_resp_info
  {
  ST_INT16   logical_stat; 	/* logical status - not optional	*/
  ST_INT16   physical_stat;	/* physical status - not optional	*/
  ST_BOOLEAN local_detail_pres;	/* flag if logical detail is present	*/
  ST_INT     local_detail_len;	/* length of logical detail (in bits)	*/
  ST_UCHAR   local_detail[MAX_STAT_DTL_LEN]; /* vendor-specific detailed info*/
  };
typedef struct status_resp_info	STATUS_RESP_INFO;

#ifndef MMS_LITE
MMSREQ_PEND *mp_status (ST_INT chan,STATUS_REQ_INFO *info);
ST_RET mp_status_resp (MMSREQ_IND *ind,STATUS_RESP_INFO *info);
#else
ST_RET mpl_status (STATUS_REQ_INFO *info);
ST_RET mpl_status_resp (ST_UINT32 invoke, STATUS_RESP_INFO *info);
#endif

/************************************************************************/
/* UNSOLICITED STATUS							*/
/************************************************************************/

/* REQUEST :								*/

struct ustatus_req_info
  {
  ST_INT16   logical_stat;	/* logical status - not optional	*/
  ST_INT16   physical_stat;	/* physical status - not optional	*/
  ST_BOOLEAN local_detail_pres;	/* flag if logical detail is present	*/
  ST_INT     local_detail_len;	/* length of logical detail (in bits)	*/
  ST_UCHAR   local_detail[MAX_STAT_DTL_LEN];	/* vendor-specific detailed info*/
  };
typedef struct ustatus_req_info	USTATUS_REQ_INFO;

/* RESPONSE : No response (unsolicited)					*/

#ifndef MMS_LITE
ST_RET mp_ustatus (ST_INT chan,USTATUS_REQ_INFO *info);
#else
ST_RET mpl_ustatus (ASN1_ENC_CTXT *aCtx, USTATUS_REQ_INFO *info);
#endif

/************************************************************************/
/* GET NAME LIST							*/
/************************************************************************/

/* REQUEST :								*/

struct namelist_req_info
  {
  ST_BOOLEAN cs_objclass_pres;		/* set for use CS object class	*/
  union
    {
    ST_INT16 mms_class;			/* object class 		*/
    struct				/* CS context object sel	*/
      {
      ST_INT   len;			/* length of ASN.1 CS class	*/
      ST_UCHAR *cs_class;  		/* CS object class ASN.1	*/
      } cs;
    } obj;

  ST_INT16   objscope;			/* object scope			*/
  ST_CHAR    dname[MAX_IDENT_LEN+1]; 	/* domain name, for scope = dom	*/
  ST_BOOLEAN cont_after_pres;		/* flag if continue param pres. */
  ST_CHAR    continue_after[MAX_IDENT_LEN+1]; /* continue-after name	*/
  SD_END_STRUCT
  };
typedef struct namelist_req_info NAMELIST_REQ_INFO;

/* RESPONSE :								*/

struct namelist_resp_info
  {
  ST_BOOLEAN more_follows;	/* end of list boolean value		*/
  ST_INT     num_names;		/* number of names			*/
  SD_END_STRUCT
  };
/* ST_CHAR *name_list[]; */	/* array of pointers to object names	*/

/* NOTE: Immediately below this structure (contiguous in memory) is a	*/
/* list of character pointers, one for each name in the name list.	*/
/* Essentially the structure and name pointers are allocated in a	*/
/* single call to malloc of size: (sizeof(NAMELIST_RESP_INFO)	*/
/* + num_names * sizeof(ST_CHAR *)).  The requesting user makes use of	*/
/* the contiguous list of pointers as appropriate.  The responding	*/
/* user must pass the mp_namelist_resp function a pointer to a contig-	*/
/* uous block of memory containing the namelist_resp_info structure at	*/
/* the top and the list of character pointers below it. 		*/
typedef struct namelist_resp_info NAMELIST_RESP_INFO;

#ifndef MMS_LITE
MMSREQ_PEND *mp_namelist (ST_INT chan,NAMELIST_REQ_INFO *info);
ST_RET mp_namelist_resp (MMSREQ_IND *ind,NAMELIST_RESP_INFO *info);
#else
ST_RET mpl_namelist (NAMELIST_REQ_INFO *info);
ST_RET mpl_namelist_resp (ST_UINT32 invoke, NAMELIST_RESP_INFO *info);
#endif

/************************************************************************/
/* IDENTIFY								*/
/************************************************************************/

/* REQUEST : NULL							*/
/* RESPONSE :								*/

/* This is the structure used for the info pass to/from the MMS prims	*/

#define MAX_VEND_LEN	64	/* vendor name max len			*/
#define MAX_REV_LEN 	16	/* revision max len			*/
#define MAX_MOD_LEN 	16	/* model max len			*/

struct ident_resp_info
  {
  ST_CHAR vend[MAX_VEND_LEN+1];	/* vendor				*/
  ST_CHAR model[MAX_MOD_LEN+1];	/* model				*/
  ST_CHAR rev[MAX_REV_LEN+1];		/* revision				*/
  ST_INT  num_as;			/* number abstract syntax's		*/
/* this structure may be followed by the abstract syntax's		*/
/*  MMS_OBJ_ID as[num_as];						*/
  SD_END_STRUCT
  };
typedef struct ident_resp_info IDENT_RESP_INFO;

#ifndef MMS_LITE
MMSREQ_PEND *mp_ident (ST_INT chan);
ST_RET mp_ident_resp (MMSREQ_IND *ind,IDENT_RESP_INFO *info);
#else
ST_RET mpl_ident (ST_VOID);
ST_RET mpl_ident_resp (ST_UINT32 invoke, IDENT_RESP_INFO *info);
#endif

/************************************************************************/
/* RENAME								*/
/************************************************************************/

/* REQUEST :								*/

struct rename_req_info
  {
  ST_BOOLEAN  cs_objclass_pres;		/* set for use CS object class	*/
  union
    {
    ST_INT16  mms_class;		/* object class 		*/
    struct				/* CS context object sel	*/
      {
      ST_INT   len;			/* length of ASN.1 CS class	*/
      ST_UCHAR *cs_class; 		/* CS object class ASN.1	*/
      } cs;
    } obj;

  OBJECT_NAME cur_name;			/* current name 		*/
  ST_CHAR     new_ident [MAX_IDENT_LEN+1];	/* new identifier		*/
  SD_END_STRUCT
  };
typedef struct rename_req_info RENAME_REQ_INFO;

/* RESPONSE : NULL							*/

#ifndef MMS_LITE
MMSREQ_PEND *mp_rename (ST_INT chan,RENAME_REQ_INFO *info);
ST_RET mp_rename_resp (MMSREQ_IND *ind);
#else
ST_RET mpl_rename (RENAME_REQ_INFO *info);
ST_RET mpl_rename_resp (ST_UINT32 invoke);
#endif

/************************************************************************/
/* GET CAPABILITY LIST							*/
/************************************************************************/

/* REQUEST :								*/

struct getcl_req_info
  {
  ST_BOOLEAN cont_after_pres;		/* flag if continue param pres. */
  ST_CHAR    *continue_after; 		/* pointer continue-after name	*/
  };
typedef struct getcl_req_info GETCL_REQ_INFO;

/* RESPONSE :								*/

struct getcl_resp_info
  {
  ST_BOOLEAN more_follows;		/* end of list boolean value	*/
  ST_INT     num_of_capab; 		/* number of capabilities	*/
/*					   list of capabilities 	*/
/*  ST_CHAR * capab_list [num_of_capab];				*/
  SD_END_STRUCT
  };
typedef struct getcl_resp_info GETCL_RESP_INFO;

#ifndef MMS_LITE
MMSREQ_PEND *mp_getcl (ST_INT chan,GETCL_REQ_INFO *info);
ST_RET mp_getcl_resp (MMSREQ_IND *ind,GETCL_RESP_INFO *info);
#else
ST_RET mpl_getcl (GETCL_REQ_INFO *info);
ST_RET mpl_getcl_resp (ST_UINT32 invoke, GETCL_RESP_INFO *info);
#endif

#ifdef __cplusplus
}
#endif

#endif	/* #define MMS_PVMD_INCLUDED */

