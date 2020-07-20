/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 1997, All Rights Reserved.		        */
/*									*/
/* MODULE NAME : mms_pevn.h						*/
/* PRODUCT(S)  : MMSEASE						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This file has the data structures and function definitions	*/
/*	required to interface with MMS event management services	*/
/*	at the primitive level.	     					*/
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
/* 12/12/02  JRB     03    Add ASN1_ENC_CTXT arg to mpl_evnot		*/ 
/* 07/15/97  MDE     02    Added cur_state defines			*/
/* 08/14/97  RKR     01    Format changes				*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#ifndef MMS_PEVN_INCLUDED
#define MMS_PEVN_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "mms_pvar.h"	/* needed for variable specification defs	*/
#include "mms_err.h"	/* needed for service error definitions		*/
#include "mms_mp.h"


/************************************************************************/
/* Event cur_state defines */

#define EC_STATE_DISABLED		0
#define EC_STATE_IDLE			1
#define EC_STATE_ACTIVE			2

#define EC_STATE_ACTIVE_NOACK_A  	3 
#define EC_STATE_IDLE_NOACK_I 		4 
#define EC_STATE_IDLE_NOACK_A		5 
#define EC_STATE_IDLE_ACKED     	6 
#define EC_STATE_ACTIVE_ACKED 		7 

/************************************************************************/
/* EVENT MANAGEMENT FUNCTIONS						*/
/************************************************************************/
/* The structures below are used to pass EVENT MANAGEMENT operation	*/
/* information to/from the MMS primitives.				*/
/************************************************************************/

struct event_enrollment
  {
  OBJECT_NAME evenroll_name;		/* event enrollment name 	*/
  ST_CHAR     evcon_name_tag;		/* 0 : event condition name	*/
					/* 1 : undefined		*/
  OBJECT_NAME evcon_name;		/* event condition name - for 	*/
					/* evcon_name_tag = 0 only	*/
  ST_BOOLEAN  evact_name_pres;		/* event action name present	*/
  ST_CHAR     evact_name_tag;		/* 0 : event action name	*/
					/* 1 : undefined		*/
  OBJECT_NAME evact_name;		/* event action name - for 	*/
	      				/* evact_name_tag = 0 only	*/
  ST_BOOLEAN  client_app_pres;		/* client application present	*/
  ST_INT      client_app_len;		/* client application length	*/
  ST_UCHAR    *client_app;		/* client application reference	*/
  ST_BOOLEAN  mms_deletable;		/* mms deletable (def = false)	*/
  ST_INT16    ee_class;			/* event enrollment class:	*/
    					/*   0 - modifier		*/
    					/*   1 - notification		*/
  ST_INT16    duration;			/* duration:			*/
    					/*   0 - current (default)	*/
    					/*   1 - permanent		*/
  ST_BOOLEAN  invoke_id_pres;		/* invoke id present		*/
  ST_UINT32   invoke_id;		/* invoke id 			*/
  ST_BOOLEAN  rem_acc_delay_pres;	/* rem acceptable delay present	*/
  ST_UINT32   rem_acc_delay;		/* remaining acceptable delay	*/
  ST_BOOLEAN  addl_detail_pres;		/* additional detail present	*/
  ST_INT      addl_detail_len;		/* length of additional detail	*/
  ST_UCHAR    *addl_detail;		/* pointer to additional detail */
  ST_BOOLEAN  ackec_name_pres;		/* ack event cond name present	*/
  ST_CHAR     ackec_name_tag;		/* 0 : ack event condition name	*/
					/* 1 : undefined		*/
  OBJECT_NAME ackec_name;		/* ack event cond name - for 	*/
					/* ackec_name_tag = 0 only	*/
  SD_END_STRUCT
  };
typedef struct event_enrollment	EVENT_ENROLLMENT;

struct alarm_summary
  {
  OBJECT_NAME evcon_name;		/* event condition name 	*/
  ST_UCHAR    severity;			/* severity			*/
  ST_INT16    cur_state;		/* current state		*/
					/*  0 : disabled		*/
					/*  1 : idle			*/
					/*  2 : active			*/
  ST_INT16    unack_state;		/* unacknowledged state 	*/
					/*  0 : none			*/
					/*  1 : active			*/
					/*  2 : idle			*/
					/*  3 : both			*/
  ST_BOOLEAN  addl_detail_pres;		/* additional detail present	*/
  ST_INT      addl_detail_len;		/* length of additional detail	*/
  ST_UCHAR    *addl_detail;		/* pointer to additional detail */
  ST_BOOLEAN  tta_time_pres;		/* transition to active time	*/
					/*   present			*/
  EVENT_TIME  tta_time;			/* transition to active time	*/
  ST_BOOLEAN  tti_time_pres;		/* transition to idle time	*/
					/*   present			*/
  EVENT_TIME  tti_time;			/* transition to idle time	*/
  SD_END_STRUCT
  };
typedef struct alarm_summary ALARM_SUMMARY;

struct alarm_enroll_summary
  {
  OBJECT_NAME evenroll_name;		/* event enrollment name 	*/
  ST_BOOLEAN  client_app_pres;		/* client application present	*/
  ST_INT      client_app_len;		/* client application length	*/
  ST_UCHAR    *client_app;		/* client application reference	*/
  ST_UCHAR    severity;			/* severity 			*/
  ST_INT16    cur_state;		/* current state		*/
					/*  0 : disabled		*/
					/*  1 : idle			*/
					/*  2 : active			*/
  ST_BOOLEAN  addl_detail_pres;		/* additional detail present	*/
  ST_INT      addl_detail_len;		/* length of additional detail	*/
  ST_UCHAR    *addl_detail;		/* pointer to additional detail */
  ST_BOOLEAN  not_lost;			/* notification lost		*/
					/*   (default = false)		*/
  ST_INT16    alarm_ack_rule;		/* alarm acknowledgment rule	*/
					/*  0 : none			*/
					/*  1 : simple			*/
					/*  2 : ack active		*/
					/*  3 : ack all			*/
  ST_BOOLEAN  ee_state_pres; 		/* enrollment state present	*/
  ST_INT16    ee_state;			/* enrollment state		*/
					/*  0 : disabled		*/
					/*  1 : idle			*/
					/*  2 : active			*/
					/*  3 : active, no ack a   	*/
					/*  4 : idle, no ack i 		*/
					/*  5 : idle, no ack a		*/
					/*  6 : idle, acked		*/
					/*  7 : active, acked 		*/
  ST_BOOLEAN  tta_time_pres;		/* transition to active time	*/
					/*   present			*/
  EVENT_TIME  tta_time;			/* transition to active time	*/
  ST_BOOLEAN  aack_time_pres;		/* active acknowledgment time	*/
					/*   present			*/
  EVENT_TIME  aack_time;		/* active acknowledgment time	*/
  ST_BOOLEAN  tti_time_pres;		/* transition to idle time	*/
					/*   present			*/
  EVENT_TIME  tti_time;			/* transition to idle time	*/
  ST_BOOLEAN  iack_time_pres;		/* idle acknowledgment time	*/
					/*   present			*/
  EVENT_TIME  iack_time;		/* idle acknowledgment time	*/
  SD_END_STRUCT
  };
typedef struct alarm_enroll_summary ALARM_ENROLL_SUMMARY;


/************************************************************************/
/************************************************************************/
/* DEFINE EVENT CONDITION						*/
/************************************************************************/

/* REQUEST :								*/

struct defec_req_info
  {
  OBJECT_NAME   evcon_name;		/* event condition name 	*/
  ST_INT16      eclass;  		/* event condition class:	*/
  ST_UCHAR      priority;		/* priority			*/
					/*   0 - highest		*/
					/*  64 - normal (default)	*/
					/* 127 - lowest 		*/
  ST_UCHAR      severity;		/* severity  (default = 64)	*/
  ST_BOOLEAN    as_reports_pres;	/* alarm summaray rpts present	*/
  ST_BOOLEAN    as_reports;		/* alarm summaray reports	*/
  ST_BOOLEAN    mon_var_pres;		/* monitored variable present	*/
  VARIABLE_SPEC var_ref;		/* variable reference		*/
  ST_BOOLEAN    eval_int_pres;		/* evaluation interval present	*/
  ST_UINT32     eval_interval;		/* evaluation interval		*/
  SD_END_STRUCT
  };
typedef struct defec_req_info DEFEC_REQ_INFO;

/* RESPONSE : NULL							*/

#ifndef MMS_LITE
MMSREQ_PEND *mp_defec (ST_INT chan, DEFEC_REQ_INFO *info);
ST_RET mp_defec_resp (MMSREQ_IND *ind);
#else
ST_RET mpl_defec (DEFEC_REQ_INFO *info);
ST_RET mpl_defec_resp (ST_UINT32 invoke);
#endif

/************************************************************************/
/* DELETE EVENT CONDITION 						*/
/************************************************************************/

/* REQUEST :								*/

struct delec_req_info
  {
  ST_INT16  req_tag;			/* request tag			*/
					/*   0 : specific		*/
					/*   1 : aa specific		*/
					/*   2 : domain			*/
					/*   3 : vmd			*/
  ST_CHAR   dname [MAX_IDENT_LEN+1]; 	/* domain name, if req_tag = 2	*/
  ST_INT    num_of_names;  		/* number of names, if tag = 0	*/
/*					   list of object names		*/
/*  OBJECT_NAME  name_list [num_of_names];			*/
  SD_END_STRUCT
  };
typedef struct delec_req_info DELEC_REQ_INFO;

/* RESPONSE :								*/

struct delec_resp_info
  {
  ST_UINT32   cand_not_deleted;		/* canditates not deleted	*/
  };
typedef struct delec_resp_info DELEC_RESP_INFO;
 
#ifndef MMS_LITE
MMSREQ_PEND *mp_delec (ST_INT chan, DELEC_REQ_INFO *info);
ST_RET mp_delec_resp (MMSREQ_IND *ind, DELEC_RESP_INFO *info);
#else
ST_RET mpl_delec (DELEC_REQ_INFO *info);
ST_RET mpl_delec_resp (ST_UINT32 invoke, DELEC_RESP_INFO *info);
#endif

/************************************************************************/
/* GET EVENT CONDITION ATTRIBUTES					*/
/************************************************************************/

/* REQUEST :								*/

struct geteca_req_info
  {
  OBJECT_NAME  evcon_name;		/* event condition name 	*/
  };
typedef struct geteca_req_info GETECA_REQ_INFO;

/* RESPONSE :								*/

struct geteca_resp_info
  {
  ST_BOOLEAN    mms_deletable;		/* mms deletable		*/
  ST_INT16      eclass;  		/* event condition class:	*/
  ST_UCHAR      priority;		/* priority			*/
					/*   0 - highest		*/
					/*  64 - normal (default)	*/
					/* 127 - lowest 		*/
  ST_UCHAR      severity;		/* severity			*/
					/*   0 - highest		*/
					/*  64 - normal (default)	*/
					/* 127 - lowest 		*/
  ST_BOOLEAN    as_reports;		/* alarm summaray reports	*/
  ST_BOOLEAN    mon_var_pres;		/* monitored variable present	*/
  ST_INT16      mon_var_tag;		/* monitored variable tag	*/
					/*   0 : variable reference	*/
					/*   1 : undefined (NULL)	*/
  VARIABLE_SPEC var_ref;		/* variable reference		*/
  ST_BOOLEAN    eval_int_pres;		/* evaluation interval present	*/
  ST_UINT32     eval_interval;		/* evaluation interval		*/
  SD_END_STRUCT
  };
typedef struct geteca_resp_info	GETECA_RESP_INFO;
 
#ifndef MMS_LITE
MMSREQ_PEND *mp_geteca (ST_INT chan, GETECA_REQ_INFO *info);
ST_RET mp_geteca_resp (MMSREQ_IND *ind, GETECA_RESP_INFO *info);
#else
ST_RET mpl_geteca (GETECA_REQ_INFO *info);
ST_RET mpl_geteca_resp (ST_UINT32 invoke, GETECA_RESP_INFO *info);
#endif

/************************************************************************/
/* REPORT EVENT CONDITION STATUS					*/
/************************************************************************/

/* REQUEST :								*/

struct repecs_req_info
  {
  OBJECT_NAME  evcon_name;		/* event condition name 	*/
  };
typedef struct repecs_req_info REPECS_REQ_INFO;

/* RESPONSE :								*/

struct repecs_resp_info
  {
  ST_INT16   cur_state;			/* event condition state:	*/
    					/*   0 - disabled		*/
    					/*   1 - idle			*/
    					/*   2 - active			*/
  ST_UINT32  num_of_ev_enroll;		/* number of event enrollments	*/
  ST_BOOLEAN enabled_pres;		/* enabled indicator present	*/
  ST_BOOLEAN enabled;			/* enabled indicator		*/
  ST_BOOLEAN tta_time_pres;		/* transition to active present	*/
  EVENT_TIME tta_time;			/* transition to active time	*/
  ST_BOOLEAN tti_time_pres;		/* transition to idle present	*/
  EVENT_TIME tti_time;			/* transition to idle time	*/
  SD_END_STRUCT
  };
typedef struct repecs_resp_info	REPECS_RESP_INFO;

#ifndef MMS_LITE
MMSREQ_PEND *mp_repecs (ST_INT chan, REPECS_REQ_INFO *info);
ST_RET mp_repecs_resp (MMSREQ_IND *ind, REPECS_RESP_INFO *info);
#else
ST_RET mpl_repecs (REPECS_REQ_INFO *info);
ST_RET mpl_repecs_resp (ST_UINT32 invoke, REPECS_RESP_INFO *info);
#endif

/************************************************************************/
/* ALTER EVENT CONDITION MONITORING					*/
/************************************************************************/

/* REQUEST :								*/

struct altecm_req_info
  {
  OBJECT_NAME evcon_name;		/* event condition name 	*/
  ST_BOOLEAN  enabled_pres;		/* enabled present		*/
  ST_BOOLEAN  enabled;			/* enabled			*/
  ST_BOOLEAN  priority_pres;		/* priority present		*/
  ST_UCHAR    priority;			/* priority			*/
					/*   0 - highest		*/
					/*  64 - normal 		*/
					/* 127 - lowest 		*/
  ST_BOOLEAN  as_reports_pres;		/* alarm summaray rpts present	*/
  ST_BOOLEAN  as_reports;		/* alarm summaray reports	*/
  ST_BOOLEAN  eval_int_pres;		/* evaluation interval present 	*/
  ST_UINT32   eval_int;			/* evaluation interval		*/
  SD_END_STRUCT
  };
typedef struct altecm_req_info ALTECM_REQ_INFO;

/* RESPONSE : NULL							*/

#ifndef MMS_LITE
MMSREQ_PEND *mp_altecm (ST_INT chan, ALTECM_REQ_INFO *info);
ST_RET mp_altecm_resp (MMSREQ_IND *ind);
#else
ST_RET mpl_altecm (ALTECM_REQ_INFO *info);
ST_RET mpl_altecm_resp (ST_UINT32 invoke);
#endif

/************************************************************************/
/* TRIGGER EVENT							*/
/************************************************************************/

/* REQUEST :								*/

struct trige_req_info
  {
  OBJECT_NAME evcon_name;		/* event condition name 	*/
  ST_BOOLEAN  priority_pres;		/* priority present		*/
  ST_UCHAR    priority;			/* priority			*/
					/*   0 - highest		*/
					/*  64 - normal 		*/
					/* 127 - lowest 		*/
  SD_END_STRUCT
  };
typedef struct trige_req_info TRIGE_REQ_INFO;

/* RESPONSE : NULL							*/

#ifndef MMS_LITE
MMSREQ_PEND *mp_trige (ST_INT chan, TRIGE_REQ_INFO *info);
ST_RET mp_trige_resp (MMSREQ_IND *info);
#else
ST_RET mpl_trige (TRIGE_REQ_INFO *info);
ST_RET mpl_trige_resp (ST_UINT32 invoke);
#endif

/************************************************************************/
/* DEFINE EVENT	ACTION							*/
/************************************************************************/

/* REQUEST :								*/

struct defea_req_info
  {
  OBJECT_NAME evact_name;		/* event action name	 	*/
  ST_INT      conf_serv_req_len;  	/* confirmed service req length	*/
  ST_UCHAR    *conf_serv_req;		/* confirmed service request 	*/
  ST_BOOLEAN  modlist_pres;  		/* list of modifiers present	*/
  ST_INT      num_of_modifiers;  	/* number of modifiers		*/
  ST_BOOLEAN  cs_rdetail_pres;		/* CS request detail present	*/
  ST_INT      cs_rdetail_len;		/* CS request detail length	*/
  ST_UCHAR    *cs_rdetail;		/* CS request detail pointer	*/
/*					   list of modifiers		*/
/*  MODIFIER   mod_list [num_of_modifiers];				*/
  SD_END_STRUCT
  };
typedef struct defea_req_info DEFEA_REQ_INFO;

/* RESPONSE : NULL							*/

#ifndef MMS_LITE
MMSREQ_PEND *mp_defea (ST_INT chan, DEFEA_REQ_INFO *info);
ST_RET mp_defea_resp (MMSREQ_IND *ind);
#else
ST_RET mpl_defea (DEFEA_REQ_INFO *info);
ST_RET mpl_defea_resp (ST_UINT32 invoke);
#endif

/************************************************************************/
/* DELETE EVENT ACTION	 						*/
/************************************************************************/

/* REQUEST :								*/

struct delea_req_info
  {
  ST_INT16    req_tag;			/* request tag			*/
					/*   0 : specific		*/
					/*   1 : aa specific		*/
					/*   2 : domain			*/
					/*   3 : vmd			*/
  ST_CHAR     dname [MAX_IDENT_LEN+1]; 	/* domain name, if req_tag = 2	*/
  ST_INT      num_of_names;  		/* number of names, if tag = 0	*/
/*					   list of object names		*/
/*  OBJECT_NAME  name_list [num_of_names];				*/
  SD_END_STRUCT
  };
typedef struct delea_req_info DELEA_REQ_INFO;

/* RESPONSE :								*/

struct delea_resp_info
  {
  ST_UINT32   cand_not_deleted;		/* canditates not deleted	*/
  };
typedef struct delea_resp_info DELEA_RESP_INFO;
 
#ifndef MMS_LITE
MMSREQ_PEND *mp_delea (ST_INT chan, DELEA_REQ_INFO *info);
ST_RET mp_delea_resp (MMSREQ_IND *ind, DELEA_RESP_INFO *info);
#else
ST_RET mpl_delea (DELEA_REQ_INFO *info);
ST_RET mpl_delea_resp (ST_UINT32 invoke, DELEA_RESP_INFO *info);
#endif

/************************************************************************/
/* GET EVENT ACTION ATTRIBUTES 						*/
/************************************************************************/

/* REQUEST :								*/

struct geteaa_req_info
  {
  OBJECT_NAME  evact_name;		/* event action name	 	*/
  };
typedef struct geteaa_req_info GETEAA_REQ_INFO;

/* RESPONSE :								*/

struct geteaa_resp_info
  {
  ST_BOOLEAN mms_deletable;		/* mms deletable (def = false)	*/
  ST_INT     conf_serv_req_len;  	/* confirmed service req length	*/
  ST_UCHAR   *conf_serv_req;		/* confirmed service request 	*/
  ST_BOOLEAN cs_rdetail_pres;		/* CS request detail present	*/
  ST_INT     cs_rdetail_len;		/* CS request detail length	*/
  ST_UCHAR   *cs_rdetail;		/* CS request detail pointer	*/
  ST_INT     num_of_modifiers;  	/* number of modifiers		*/
/*					   list of modifiers		*/
/*  MODIFIER   mod_list [num_of_modifiers];				*/
  SD_END_STRUCT
  };
typedef struct geteaa_resp_info	GETEAA_RESP_INFO;
 
#ifndef MMS_LITE
MMSREQ_PEND *mp_geteaa (ST_INT chan, GETEAA_REQ_INFO *info);
ST_RET mp_geteaa_resp (MMSREQ_IND *ind, GETEAA_RESP_INFO *info);
#else
ST_RET mpl_geteaa (GETEAA_REQ_INFO *info);
ST_RET mpl_geteaa_resp (ST_UINT32 invoke, GETEAA_RESP_INFO *info);
#endif

/************************************************************************/
/* REPORT EVENT ACTION STATUS 						*/
/************************************************************************/

/* REQUEST :								*/

struct repeas_req_info
  {
  OBJECT_NAME  evact_name;		/* event action name	 	*/
  };
typedef struct repeas_req_info REPEAS_REQ_INFO;

/* RESPONSE :								*/

struct repeas_resp_info
  {
  ST_UINT32   num_of_ev_enroll;		/* number of event enrollments	*/
  };
typedef struct repeas_resp_info	REPEAS_RESP_INFO;
 
#ifndef MMS_LITE
MMSREQ_PEND *mp_repeas (ST_INT chan, REPEAS_REQ_INFO *info);
ST_RET mp_repeas_resp (MMSREQ_IND *ind, REPEAS_RESP_INFO *info);
#else
ST_RET mpl_repeas (REPEAS_REQ_INFO *info);
ST_RET mpl_repeas_resp (ST_UINT32 invoke, REPEAS_RESP_INFO *info);
#endif

/************************************************************************/
/* DEFINE EVENT	ENROLLMENT						*/
/************************************************************************/

/* REQUEST :								*/

struct defee_req_info
  {
  OBJECT_NAME evenroll_name;		/* event enrollment name 	*/
  OBJECT_NAME evcon_name;		/* event condition name	 	*/
  ST_UCHAR    ec_transitions;		/* causing transitions:		*/
  					/* bitstring			*/
					/*  0 : idle to disabled	*/
					/*  1 : active to disabled	*/
					/*  2 : disabled to idle	*/
					/*  3 : active to idle		*/
					/*  4 : disabled to active	*/
					/*  5 : idle to active		*/
					/*  6 : any to deleted		*/
  ST_INT16    alarm_ack_rule;		/* alarm acknowledgment rule	*/
					/*  0 : none			*/
					/*  1 : simple			*/
					/*  2 : ack active		*/
					/*  3 : ack all			*/
  ST_BOOLEAN  evact_name_pres;  	/* event action name present	*/
  OBJECT_NAME evact_name;		/* event action name	 	*/
  ST_BOOLEAN  client_app_pres;		/* client application present	*/
  ST_INT      client_app_len;		/* client application length	*/
  ST_UCHAR    *client_app;		/* client application reference	*/
  ST_BOOLEAN  ackec_name_pres;		/* ack event cond name present	*/
  OBJECT_NAME ackec_name;		/* acknowledge event cond name	*/
  SD_END_STRUCT
  };
typedef struct defee_req_info DEFEE_REQ_INFO;

/* RESPONSE : NULL							*/

#ifndef MMS_LITE
MMSREQ_PEND *mp_defee (ST_INT chan, DEFEE_REQ_INFO *info);
ST_RET mp_defee_resp (MMSREQ_IND *ind);
#else
ST_RET mpl_defee (DEFEE_REQ_INFO *info);
ST_RET mpl_defee_resp (ST_UINT32 invoke);
#endif

/************************************************************************/
/* DELETE EVENT ENROLLMENT 						*/
/************************************************************************/

/* REQUEST :								*/

struct delee_req_info
  {
  ST_INT16    req_tag;			/* request tag			*/
					/*   0 : specific		*/
					/*   1 : event condition	*/
					/*   2 : event action		*/
  union
    {
    OBJECT_NAME	evcon_name;		/* event condition name  	*/
    OBJECT_NAME	evact_name;		/* event action name 	 	*/
    ST_INT      num_of_names;  		/* number of object names	*/
    } sod;
/*					   list of object names		*/
/*  OBJECT_NAME   name_list [num_of_names];				*/
  SD_END_STRUCT
  };
typedef struct delee_req_info DELEE_REQ_INFO;

/* RESPONSE :								*/

struct delee_resp_info
  {
  ST_UINT32   cand_not_deleted;		/* canditates not deleted	*/
  };
typedef struct delee_resp_info DELEE_RESP_INFO;
 
#ifndef MMS_LITE
MMSREQ_PEND *mp_delee (ST_INT chan, DELEE_REQ_INFO *info);
ST_RET mp_delee_resp (MMSREQ_IND *ind, DELEE_RESP_INFO *info);
#else
ST_RET mpl_delee (DELEE_REQ_INFO *info);
ST_RET mpl_delee_resp (ST_UINT32 invoke, DELEE_RESP_INFO *info);
#endif

/************************************************************************/
/* GET EVENT ENROLLMENT ATTRIBUTES					*/
/************************************************************************/

/* REQUEST :								*/

struct geteea_req_info
  {
  ST_INT16    scope_of_req;		/* scope of request		*/
					/*  0 : specific		*/
					/*  1 : client (default)	*/
					/*  2 : event condition		*/
					/*  3 : event action		*/
  ST_BOOLEAN  client_app_pres;		/* client application present	*/
  ST_INT      client_app_len;		/* client application length	*/
  ST_UCHAR    *client_app;		/* client application reference	*/
  ST_BOOLEAN  evcon_name_pres;  	/* event condition name present	*/
  OBJECT_NAME evcon_name;		/* event condition name  	*/
  ST_BOOLEAN  evact_name_pres;  	/* event action name present	*/
  OBJECT_NAME evact_name;		/* event action name	 	*/
  ST_BOOLEAN  ca_name_pres;   		/* continue after name present	*/
  OBJECT_NAME ca_name;			/* continue after name	 	*/
  ST_BOOLEAN  eenames_pres; 	  	/* event enroll names present	*/
  ST_INT      num_of_eenames;		/* number of event enroll names	*/
/*					   list of event enroll names	*/
/*  OBJECT_NAME   name_list [num_of_eenames];				*/
  SD_END_STRUCT
  };
typedef struct geteea_req_info GETEEA_REQ_INFO;

/* RESPONSE :								*/

struct geteea_resp_info
  {
  ST_BOOLEAN  more_follows;  		/* default = false		*/
  ST_INT      num_of_evenroll;		/* number of event enrollments	*/
/*					   list of event enrollments	*/
/*  EVENT_ENROLLMENT  evenroll_list [num_of_evenroll];			*/
  SD_END_STRUCT
  };
typedef struct geteea_resp_info	GETEEA_RESP_INFO;
 
#ifndef MMS_LITE
MMSREQ_PEND *mp_geteea (ST_INT chan, GETEEA_REQ_INFO *info);
ST_RET mp_geteea_resp (MMSREQ_IND *ind, GETEEA_RESP_INFO *info);
#else
ST_RET mpl_geteea (GETEEA_REQ_INFO *info);
ST_RET mpl_geteea_resp (ST_UINT32 invoke, GETEEA_RESP_INFO *info);
#endif

/************************************************************************/
/* REPORT EVENT ENROLLMENT STATUS					*/
/************************************************************************/

/* REQUEST :								*/

struct repees_req_info
  {
  OBJECT_NAME evenroll_name;		/* event enrollment name  	*/
  };
typedef struct repees_req_info REPEES_REQ_INFO;

/* RESPONSE :								*/

struct repees_resp_info
  {
  ST_UCHAR   ec_transitions;		/* event condition transitions:	*/
  					/* bitstring			*/
					/*  0 : idle to disabled	*/
					/*  1 : active to disabled	*/
					/*  2 : disabled to idle	*/
					/*  3 : active to idle		*/
					/*  4 : disabled to active	*/
					/*  5 : idle to active		*/
					/*  6 : any to deleted		*/
  ST_UCHAR   not_lost;			/* notification lost		*/
					/*   (default = false)		*/
  ST_INT16   duration;			/* duration:			*/
    					/*   0 - current (default)	*/
    					/*   1 - permanent		*/
  ST_BOOLEAN alarm_ack_rule_pres;	/* alarm ack rule present	*/
  ST_INT16   alarm_ack_rule;		/* alarm acknowledgment rule	*/
					/*  0 : none			*/
					/*  1 : simple			*/
					/*  2 : ack active		*/
					/*  3 : ack all			*/
  ST_INT16   cur_state;			/* current state		*/
					/*  0 : disabled		*/
					/*  1 : idle			*/
					/*  2 : active			*/
					/*  3 : active, no ack a   	*/
					/*  4 : idle, no ack i 		*/
					/*  5 : idle, no ack a		*/
					/*  6 : idle, acked		*/
					/*  7 : active, acked 		*/
  };
typedef struct repees_resp_info	REPEES_RESP_INFO;
 
#ifndef MMS_LITE
MMSREQ_PEND *mp_repees (ST_INT chan, REPEES_REQ_INFO *info);
ST_RET mp_repees_resp (MMSREQ_IND *ind, REPEES_RESP_INFO *info);
#else
ST_RET mpl_repees (REPEES_REQ_INFO *info);
ST_RET mpl_repees_resp (ST_UINT32 invoke, REPEES_RESP_INFO *info);
#endif

/************************************************************************/
/* ALTER EVENT ENROLLMENT 						*/
/************************************************************************/

/* REQUEST :								*/

struct altee_req_info
  {
  OBJECT_NAME evenroll_name;		/* event enrollment name    	*/
  ST_BOOLEAN  ec_transitions_pres;	/* ec transitions present	*/
  ST_UCHAR    ec_transitions;		/* event condition transitions:	*/
  					/* bitstring			*/
					/*  0 : idle to disabled	*/
					/*  1 : active to disabled	*/
					/*  2 : disabled to idle	*/
					/*  3 : active to idle		*/
					/*  4 : disabled to active	*/
					/*  5 : idle to active		*/
					/*  6 : any to deleted		*/
  ST_BOOLEAN  alarm_ack_rule_pres;	/* alarm ack rule present	*/
  ST_INT16    alarm_ack_rule;		/* alarm acknowledgment rule	*/
					/*  0 : none			*/
					/*  1 : simple			*/
					/*  2 : ack active		*/
					/*  3 : ack all			*/
  SD_END_STRUCT
  };
typedef struct altee_req_info ALTEE_REQ_INFO;

/* RESPONSE :								*/

struct altee_resp_info
  {
  ST_INT16    cur_state_tag; 		/* current state tag		*/
					/*  0 : state			*/
					/*  1 : undefined		*/
  ST_INT16    state;			/* state, if cur_state_tag = 0	*/
					/*  0 : disabled		*/
					/*  1 : idle			*/
					/*  2 : active			*/
					/*  3 : active, no ack a   	*/
					/*  4 : idle, no ack i 		*/
					/*  5 : idle, no ack a		*/
					/*  6 : idle, acked		*/
					/*  7 : active, acked 		*/
  EVENT_TIME  trans_time;		/* transition time		*/
  SD_END_STRUCT
  };
typedef struct altee_resp_info ALTEE_RESP_INFO;
 
#ifndef MMS_LITE
MMSREQ_PEND *mp_altee (ST_INT chan, ALTEE_REQ_INFO *info);
ST_RET mp_altee_resp (MMSREQ_IND *ind, ALTEE_RESP_INFO *info);
#else
ST_RET mpl_altee (ALTEE_REQ_INFO *info);
ST_RET mpl_altee_resp (ST_UINT32 invoke, ALTEE_RESP_INFO *info);
#endif

/************************************************************************/
/* EVENT NOTIFICATION, Conformance: MCW1 (EVN3) 			*/
/************************************************************************/

/* REQUEST :								*/

struct evnot_req_info
  {
  OBJECT_NAME evenroll_name;		/* event enrollment name	*/
  OBJECT_NAME evcon_name;		/* event condition name  	*/
  ST_UCHAR    severity;			/* severity			*/
  ST_BOOLEAN  cur_state_pres;		/* current state enable		*/
  ST_INT16    cur_state;		/* current state		*/
					/*  0 : disabled		*/
					/*  1 : idle			*/
					/*  2 : active			*/
  EVENT_TIME  trans_time;    		/* transition time		*/
  ST_UCHAR    not_lost;			/* notification lost		*/
					/*   (default = false)		*/
  ST_BOOLEAN  alarm_ack_rule_pres;	/* alarm acknowledge rule pres	*/
  ST_INT16    alarm_ack_rule;		/* alarm acknowledgment rule	*/
					/*  0 : none			*/
					/*  1 : simple			*/
					/*  2 : ack-active		*/
					/*  3 : ack-all 		*/
  ST_BOOLEAN  evact_result_pres;	/* action result present	*/
  OBJECT_NAME evact_name;		/* event action name		*/
  ST_INT16    evact_result_tag;		/* event action result tag	*/
					/*  0 : success 		*/
					/*  1 : failure 		*/
  ST_INT      conf_serv_resp_len;	/* success: conf serv resp len	*/
  ST_UCHAR    *conf_serv_resp;		/* success: conf serv resp ptr	*/
  ST_BOOLEAN  cs_rdetail_pres;		/* success: CS resp detail pres	*/
  ST_INT      cs_rdetail_len;		/* success: CS resp detail len	*/
  ST_UCHAR    *cs_rdetail;		/* success: CS resp detail ptr	*/

  ST_BOOLEAN  mod_pos_pres;		/* failure: mod pos present	*/
  ST_UINT32   mod_pos;			/* failure: modifier position	*/
  ERR_INFO    *serv_err;		/* failure: service error ptr	*/

  SD_END_STRUCT
  };
typedef struct evnot_req_info EVNOT_REQ_INFO;

/* RESPONSE : NONE (unconfirmed)					*/

#ifndef MMS_LITE
ST_RET mp_evnot (ST_INT chan, EVNOT_REQ_INFO *info);
#else
ST_RET mpl_evnot (ASN1_ENC_CTXT *aCtx, EVNOT_REQ_INFO *info);
#endif

/************************************************************************/
/* ACKNOWLEDGE EVENT NOTIFICATION, Conformance: MCW1 (EVN3)		*/
/************************************************************************/

/* REQUEST :								*/

struct ackevnot_req_info
  {
  OBJECT_NAME evenroll_name;		/* event enrollment name 	*/
  ST_INT16    ack_state;		/* acknowledge state		*/
  EVENT_TIME  evtime;			/* event time			*/
  ST_BOOLEAN  ackec_name_pres;		/* ack event cond name present	*/
  OBJECT_NAME ackec_name;		/* acknowledge event cond name	*/
  };
typedef struct ackevnot_req_info ACKEVNOT_REQ_INFO;

/* RESPONSE : NULL							*/

#ifndef MMS_LITE
MMSREQ_PEND *mp_ackevnot (ST_INT chan, ACKEVNOT_REQ_INFO *info);
ST_RET mp_ackevnot_resp (MMSREQ_IND *info);
#else
ST_RET mpl_ackevnot (ACKEVNOT_REQ_INFO *info);
ST_RET mpl_ackevnot_resp (ST_UINT32 invoke);
#endif

/************************************************************************/
/* GET ALARM SUMMARY, Conformance: EVN5 				*/
/************************************************************************/

/* REQUEST :								*/

struct getas_req_info
  {
  ST_BOOLEAN  enroll_only;		/* enrollments only		*/
					/*   (default = true)		*/
  ST_BOOLEAN  act_alarms_only;		/* active alarms only		*/
					/*   (default = true)		*/
  ST_INT16    ack_filter;		/* acknowledgement filter	*/
					/*  0 : not acked (default)	*/
					/*  1 : acked			*/
					/*  2 : all			*/
					/* severity filter		*/
  ST_UCHAR    most_sev_filter;		/*   most severe (default=0)	*/
  ST_UCHAR    least_sev_filter;		/*   least severe (default=127) */
  ST_BOOLEAN  ca_pres;			/* continue after name present	*/
  OBJECT_NAME ca_name; 			/* continue after name		*/
  };
typedef struct getas_req_info GETAS_REQ_INFO;

/* RESPONSE :								*/

struct getas_resp_info
  {
  ST_BOOLEAN  more_follows;		/* default = false		*/
  ST_INT      num_of_alarm_sum;		/* number of alarm summary	*/
/*					   list of alarm summary data	*/
/*  ALARM_SUMMARY alarm_sum [num_of_alarm_sum];				*/
  SD_END_STRUCT
  };
typedef struct getas_resp_info GETAS_RESP_INFO;

#ifndef MMS_LITE
MMSREQ_PEND *mp_getas (ST_INT chan, GETAS_REQ_INFO *info);
ST_RET mp_getas_resp (MMSREQ_IND *ind, GETAS_RESP_INFO *info);
#else
ST_RET mpl_getas (GETAS_REQ_INFO *info);
ST_RET mpl_getas_resp (ST_UINT32 invoke, GETAS_RESP_INFO *info);
#endif

/************************************************************************/
/* GET ALARM ENROLLMENT SUMMARY						*/
/************************************************************************/

/* REQUEST :								*/

struct getaes_req_info
  {
  ST_BOOLEAN  enroll_only;		/* enrollments only		*/
					/*   (default = true)		*/
  ST_BOOLEAN  act_alarms_only;		/* active alarms only		*/
					/*   (default = true)		*/
  ST_INT16    ack_filter;		/* acknowledgement filter	*/
					/*  0 : not acked (default)	*/
					/*  1 : acked			*/
					/*  2 : all			*/
					/* severity filter		*/
  ST_UCHAR    most_sev_filter;		/*   most severe (default=0)	*/
  ST_UCHAR    least_sev_filter;		/*   least severe (default=127) */
  ST_BOOLEAN  ca_name_pres; 		/* continue after name present	*/
  OBJECT_NAME ca_name; 			/* continue after name		*/
  };
typedef struct getaes_req_info GETAES_REQ_INFO;

/* RESPONSE :								*/

struct getaes_resp_info
  {
  ST_BOOLEAN  more_follows;  		/* default = false		*/
  ST_INT      num_of_alarm_esum;	/* # of alarm enroll summary	*/
/*					   list of alarm enroll summary	*/
/*  ALARM_ENROLL_SUMMARY  alarm_enroll_sum [num_of_alarm_esum];		*/
  SD_END_STRUCT
  };
typedef struct getaes_resp_info	GETAES_RESP_INFO;
 
#ifndef MMS_LITE
MMSREQ_PEND *mp_getaes (ST_INT chan, GETAES_REQ_INFO *info);
ST_RET mp_getaes_resp (MMSREQ_IND *ind, GETAES_RESP_INFO *info);
#else
ST_RET mpl_getaes (GETAES_REQ_INFO *info);
ST_RET mpl_getaes_resp (ST_UINT32 invoke, GETAES_RESP_INFO *info);
#endif

#ifdef __cplusplus
}
#endif

#endif	/* #define MMS_PEVN_INCLUDED */
