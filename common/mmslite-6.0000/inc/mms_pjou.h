/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 1997, All Rights Reserved.		        */
/*									*/
/* MODULE NAME : mms_pjou.h						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This file has the data structures and function definitions	*/
/*	required to interface with MMS journal management services	*/
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
/* 07/15/98  MDE     03    Added entry_form_tag defines			*/
/* 08/15/97  MDE     02    BTOD handling changes			*/
/* 08/14/97  RKR     01    Format changes				*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#ifndef MMS_PJOU_INCLUDED
#define MMS_PJOU_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "mms_pvar.h"
#include "mms_mp.h"

/************************************************************************/
/************************************************************************/
/* JOURNAL MANAGEMENT SERVICES						*/
/************************************************************************/
/* The structures below are used to pass JOURNAL MANAGEMENT information */
/* to/from the MMS primitives.						*/
/************************************************************************/

/* entry_form_tag value defines						*/
#define JE_FORM_DATA 		2
#define JE_FORM_ANNOTATION 	3

struct var_info
  {
  ST_CHAR      *var_tag;		/* variable tag 		*/
  VAR_ACC_DATA value_spec;		/* value specification data	*/
  };
typedef struct var_info	VAR_INFO;

struct entry_content
  {
  MMS_BTOD occur_time;			/* occurrence time		*/
  ST_BOOLEAN  addl_detail_pres;		/* additional detail present	*/
  ST_INT      addl_detail_len;		/* length of additional detail	*/
  ST_UCHAR    *addl_detail;		/* pointer to additional detail */
  ST_INT16    entry_form_tag;		/* entry form tag		*/
  union
    {
    struct				/* entry form is DATA		*/
      {
      ST_BOOLEAN  event_pres;		/* event present		*/
      OBJECT_NAME evcon_name;		/* event condition name 	*/
      ST_INT16    cur_state;		/* current state		*/
					/*  0 : disabled		*/
					/*  1 : idle			*/
					/*  2 : active			*/
      ST_BOOLEAN  list_of_var_pres;	/* list of variables present	*/
      ST_INT      num_of_var;		/* number of variables		*/
      } data;
    ST_CHAR	  *annotation;		/* pointer to annotation	*/
    }ef;
/*					   for form == DATA, need a	*/
/*					   list of variables		*/
/*  VAR_INFO list_of_var [num_of_var];					*/
  SD_END_STRUCT
  };
typedef struct entry_content ENTRY_CONTENT;

struct journal_entry
  {
  ST_INT        entry_id_len;	       	/* length of entry identifier	*/
  ST_UCHAR      *entry_id;	       	/* ptr to entry identifier	*/
  ST_INT        orig_ae_len;	       	/* originating app entity len	*/
  ST_UCHAR      *orig_ae;		/* originating app entity	*/
  ENTRY_CONTENT ent_content;		/* entry content		*/
  };
typedef struct journal_entry JOURNAL_ENTRY;

/************************************************************************/
/* READ JOURNAL, Conformance: JOU2					*/
/************************************************************************/

/* REQUEST :								*/

struct jread_req_info
  {
  OBJECT_NAME jou_name;			/* journal name 		*/
  ST_BOOLEAN  range_start_pres;		/* range start spec present	*/
  ST_INT16    start_tag;		/* start tag			*/
					/*  0 - starting time		*/
					/*  1 - starting entry		*/
  MMS_BTOD start_time;			/* range starting time		*/
  ST_INT      start_entry_len;		/* length of starting entry	*/
  ST_UCHAR    *start_entry;		/* pointer to starting entry	*/
  ST_BOOLEAN  range_stop_pres;		/* range stop spec present	*/
  ST_INT16    stop_tag;			/* stop tag			*/
					/*  0 - ending time		*/
					/*  1 - number of entries	*/
  MMS_BTOD end_time;			/* range ending time		*/
  ST_INT32    num_of_entries;		/* number of entries		*/
  ST_BOOLEAN  list_of_var_pres;		/* list of variables present	*/
  ST_INT      num_of_var;		/* number of variables		*/
  ST_BOOLEAN  sa_entry_pres;		/* start after entry present	*/
  MMS_BTOD time_spec;			/* time specification		*/
  ST_INT      entry_spec_len;		/* entry specification length	*/
  ST_UCHAR    *entry_spec;		/* entry specification		*/
/*					   list of pointers to variables*/
/*  ST_CHAR *list_of_var [num_of_var];					*/
  SD_END_STRUCT
  };
typedef struct jread_req_info JREAD_REQ_INFO;

/* RESPONSE :								*/

struct jread_resp_info
  {
  ST_INT     num_of_jou_entry;		/* number of journal entries	*/
  ST_BOOLEAN more_follows; 		/* default = false		*/
/*					   list of journal entries	*/
/*  JOURNAL_ENTRY list_of_jou_entry [num_of_jou_entry];			*/
  SD_END_STRUCT
  };
typedef struct jread_resp_info JREAD_RESP_INFO;

#ifndef MMS_LITE
MMSREQ_PEND *mp_jread (ST_INT chan, JREAD_REQ_INFO *info);
ST_RET mp_jread_resp (MMSREQ_IND *ind, JREAD_RESP_INFO *info);
#else
ST_RET mpl_jread (JREAD_REQ_INFO *info);
ST_RET mpl_jread_resp (ST_UINT32 invoke, JREAD_RESP_INFO *info);
#endif

/************************************************************************/
/* WRITE JOURNAL, Conformance: JOU1					*/
/************************************************************************/

/* REQUEST :								*/

struct jwrite_req_info
  {
  OBJECT_NAME jou_name;			/* journal name 		*/
  ST_INT      num_of_jou_entry;		/* number of journal entries	*/
/*					   list of journal entries	*/
/*  ENTRY_CONTENT list_of_jou_entry [num_of_jou_entry];			*/
  SD_END_STRUCT
  };
typedef struct jwrite_req_info JWRITE_REQ_INFO;

/* RESPONSE : NULL							*/

#ifndef MMS_LITE
MMSREQ_PEND *mp_jwrite (ST_INT chan, JWRITE_REQ_INFO *info);
ST_RET mp_jwrite_resp (MMSREQ_IND *ind);
#else
ST_RET mpl_jwrite (JWRITE_REQ_INFO *info);
ST_RET mpl_jwrite_resp (ST_UINT32 invoke);
#endif

/************************************************************************/
/* INITIALIZE JOURNAL, Conformance: JOU2				*/
/************************************************************************/

/* REQUEST :								*/

struct jinit_req_info
  {
  OBJECT_NAME jou_name;			/* journal name 		*/
  ST_BOOLEAN  limit_spec_pres;		/* limit specification present	*/
  MMS_BTOD limit_time;			/* limiting time		*/
  ST_BOOLEAN  limit_entry_pres;		/* limiting entry present	*/
  ST_INT      limit_entry_len;		/* length of limiting entry	*/
  ST_UCHAR    *limit_entry;		/* pointer to limiting entry	*/
  SD_END_STRUCT
  };
typedef struct jinit_req_info JINIT_REQ_INFO;

/* RESPONSE :								*/

struct jinit_resp_info
  {
  ST_UINT32  del_entries;		/* deleted entries		*/
  };
typedef struct jinit_resp_info JINIT_RESP_INFO;

#ifndef MMS_LITE
MMSREQ_PEND *mp_jinit (ST_INT chan, JINIT_REQ_INFO *info);
ST_RET mp_jinit_resp (MMSREQ_IND *ind, JINIT_RESP_INFO *info);
#else
ST_RET mpl_jinit (JINIT_REQ_INFO *info);
ST_RET mpl_jinit_resp (ST_UINT32 invoke, JINIT_RESP_INFO *info);
#endif

/************************************************************************/
/* REPORT JOURNAL STATUS, Conformance: JOU2				*/
/************************************************************************/

/* REQUEST :								*/

struct jstat_req_info
  {
  OBJECT_NAME  jou_name; 		/* journal name 		*/
  };
typedef struct jstat_req_info JSTAT_REQ_INFO;

/* RESPONSE :								*/

struct jstat_resp_info
  {
  ST_UINT32  cur_entries;		/* current entries		*/
  ST_BOOLEAN mms_deletable;		/* MMS deletable		*/
  SD_END_STRUCT
  };
typedef struct jstat_resp_info JSTAT_RESP_INFO;

#ifndef MMS_LITE
MMSREQ_PEND *mp_jstat (ST_INT chan, JSTAT_REQ_INFO *info);
ST_RET mp_jstat_resp (MMSREQ_IND *ind, JSTAT_RESP_INFO *info);
#else
ST_RET mpl_jstat (JSTAT_REQ_INFO *info);
ST_RET mpl_jstat_resp (ST_UINT32 invoke, JSTAT_RESP_INFO *info);
#endif

/************************************************************************/
/* CREATE JOURNAL							*/
/************************************************************************/

/* REQUEST :								*/

struct jcreate_req_info
  {
  OBJECT_NAME  jou_name;		/* journal name 		*/
  };
typedef struct jcreate_req_info	JCREATE_REQ_INFO;

/* RESPONSE : NULL							*/

#ifndef MMS_LITE
MMSREQ_PEND *mp_jcreate (ST_INT chan, JCREATE_REQ_INFO *info);
ST_RET mp_jcreate_resp (MMSREQ_IND *ind);
#else
ST_RET mpl_jcreate (JCREATE_REQ_INFO *info);
ST_RET mpl_jcreate_resp (ST_UINT32 invoke);
#endif

/************************************************************************/
/* DELETE JOURNAL 							*/
/************************************************************************/

/* REQUEST :								*/

struct jdelete_req_info
  {
  OBJECT_NAME  jou_name;		/* journal name 		*/
  };
typedef struct jdelete_req_info	JDELETE_REQ_INFO;

/* RESPONSE : NULL							*/

#ifndef MMS_LITE
MMSREQ_PEND *mp_jdelete (ST_INT chan, JDELETE_REQ_INFO *info);
ST_RET mp_jdelete_resp (MMSREQ_IND *ind);
#else
ST_RET mpl_jdelete (JDELETE_REQ_INFO *info);
ST_RET mpl_jdelete_resp (ST_UINT32 invoke);
#endif

#ifdef __cplusplus
}
#endif

#endif	/* #define MMS_PJOU_INCLUDED */
