/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2007, All Rights Reserved.		        */
/*									*/
/* MODULE NAME : mmsop_en.c						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module is used to initialize the tables used in defining	*/
/*	the supported operation set, and MUST be re-compiled if the 	*/
/*	supported command set is reduced (or expanded) via the subset 	*/
/*	creation mechanism.  This module also defines and initializes	*/
/*	the supported services parameters.				*/
/*									*/
/*	The following tables are initialized:				*/
/*									*/
/*     *operation enable table (mmsop_en [MAX_MMSOP_DIS+1]) :		*/
/*		used to check if an MMS operation is to be supported	*/
/*		as requestor and/or responder, and if the data element	*/
/*		containing the opcode is of the proper form.  		*/
/*									*/
/*     *operation name table (mms_op_string [MAX_MMSOP_DIS+1]) :	*/
/*		used to print the name of the MMS operation.		*/
/*									*/
/*     *request decode function table 					*/
/*		(mms_req_decode_fun [MAX_MMSOP_DIS+1]) :		*/
/*		used to define which function will be executed to	*/
/*		decode a request.					*/
/*									*/
/*     *response decode function table					*/
/*		(mms_rsp_decode_fun [MAX_MMSOP_DIS+1]) :		*/
/*		used to define which function will be executed to	*/
/*		decode a response.					*/
/*									*/
/*    **indication service function table 				*/
/*		(mms_ind_serve_fun  [MAX_IND_SFUN+1]) :			*/
/*		used to define which function will be executed to	*/
/*		service an indication.					*/
/*									*/
/*    **confirmation service function table 				*/
/*		(mms_conf_serve_fun [MAX_CONF_SFUN+1]) :		*/
/*		used to define which function will be executed to	*/
/*		service a confirmation.					*/
/*									*/
/*     * - these tables are not normally modified by the user.		*/
/*    ** - these tables may be modified by the user.			*/
/*									*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* NOTE: If this module changes the mmsop_en_versionN variable need	*/
/*       to change to (N+1) to prevent linking of old versions of this	*/
/*       file to user applications. The strt_MMS() in mmsutil.c should	*/
/*	 reference the new name. 					*/
/* 03/03/10  JRB	   Replace "ml_log_*" with "mlogl_info" struct.	*/
/* 11/06/07  EJV     20    Rem obsolete MAP30_LLC code.			*/
/* 11/02/06  JRB     19    Del #if around some m_sv_* for linker problem*/
/* 05/07/03  EJV     18    Add mmsop_en_version1 global & comment above.*/
/* 07/11/02  EJV     17    mms_conf_serve_fun: corrected few entries.	*/
/* 07/09/02  MDE     16    Fixed up opcode defines			*/
/* 12/18/01  JRB     15    Convert to use ASN1R.			*/
/* 04/19/00  JRB     14    Fix SD_CONST for decode funct ptr arrays.	*/
/* 09/13/99  MDE     13    Added SD_CONST modifiers			*/
/* 10/05/98  EJV     12    Replace struct acse_assinfo with ACSE_ASSINFO*/
/*			   Chg u_mllp_l_assoc_ind_fun return to ST_RET.	*/
/* 05/27/98  MDE     11    Added MLOG_DISABLE define use		*/
/* 04/13/98  MDE     10    MMS-LITE MLOG logging, unconfirmed logging	*/
/* 12/30/97  EJV     09    Added typecast to all functions in tables:	*/
/*			   m_req_log_fun_tbl and m_resp_log_fun_tbl	*/
/* 10/13/97  DSF     08    no more MAP21_CASE!!!			*/
/* 09/05/97  DSF     07    Fixed m_cl_max_getvla_vars #ifdef problem	*/
/* 09/02/97  DSF     06    Always define mms_op_string if not MMS-LITE	*/
/* 08/18/97  MDE     05    Moved parameter support def's to mmsop_en.h	*/
/* 08/14/97  DSF     04    define MLOG_ENABLE if not MMS-LITE		*/
/* 08/05/97  MDE     03    MLOG integration				*/
/* 07/03/97  MDE     02    Minor tweaks to decode size control vars	*/
/* 06/09/97  MDE     01    Added decode size control variables		*/
/* 04/15/97  DSF   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "mmsop_en.h"
#include "mms_dfun.h"

#if !defined(MMS_LITE) && !defined(MLOG_DISABLE)
#define MLOG_ENABLE 
#endif

#if !defined(DEBUG_SISCO)
/* MLOG_ENABLE doesn't enable anything useful without DEBUG_SISCO,	*/
/* so undefine it if DEBUG_SISCO is not defined.			*/
#undef MLOG_ENABLE
#endif

#if !defined(MMS_LITE)
#include "mms_usr.h"
#else
#include "mms_def2.h"
#endif

#include "mloguser.h"

/************************************************************************/
/************************************************************************/
/* NOTE: This variable will prevent linking of old version of the	*/
/*       mmsop_en.c file to user application. If this file changes this	*/
/*       variable's name should be changed to mmsop_en_version(N+1) and	*/
/*       the strt_MMS() in mmsutil.c should reference the new name.	*/
ST_INT  mmsop_en_version1;


/************************************************************************/
/************************************************************************/
/* The following defines are used in initializing the mmsop_en array.	*/
/* Each entry of the array consists of 8 bits: 00ijklmn, where the	*/
/* bottom 6 bits mean the following:					*/
/*									*/
/*	i - if 1, then response is NULL 				*/
/*	j - if 1, then request is NULL					*/
/*	k - if 1, then response is constructor; otherwise, primitive.	*/
/*	l - if 1, then request is constructor; otherwise, primitive.	*/
/*	m - if 1, then response is supported; otherwise not.		*/
/*	n - if 1, then request is supported; otherwise not.		*/
/*									*/
/* By "supported" is meant that the executable program contains code to */
/* handle that op code. 						*/
/************************************************************************/
/*									*/
/*	resp req							*/
#define PRIM_PRIM	0x00		/* primitive resp and req	*/

#define PRIM_CSTR	0x04		/* prim resp, constructor req	*/
#define CSTR_PRIM	0x08		/* constructor resp, prim req	*/
#define CSTR_CSTR	0x0C		/* cstr resp, cstr req		*/
#define NULL_NULL	0x30		/* primitive resp and req	*/
#define NULL_PRIM	0x20		/* prim resp, constructor req	*/
#define PRIM_NULL	0x10		/* constructor resp, prim req	*/
#define NULL_CSTR	0x24		/* prim resp, constructor req	*/
#define CSTR_NULL	0x18		/* constructor resp, prim req	*/

/************************************************************************/
/* initialize the operation enable table, opcode is index into table	*/
/************************************************************************/
SD_CONST ST_UCHAR mmsop_en [MAX_MMSOP_DIS+1] =
  {
  MMS_STATUS_EN | CSTR_PRIM,		/*  00 STATUS			*/
  MMS_GETNAMES_EN | CSTR_CSTR,		/*  01 GET_NAMLIST		*/
  MMS_IDENT_EN | CSTR_NULL,		/*  02 IDENTIFY 		*/
  MMS_RENAME_EN | NULL_CSTR,		/*  03 RENAME			*/
  MMS_READ_EN | CSTR_CSTR,		/*  04 READ			*/
  MMS_WRITE_EN | CSTR_CSTR,		/*  05 WRITE			*/
  MMS_GETVAR_EN | CSTR_CSTR,		/*  06 GET_VARDEF		*/
  MMS_DEFVAR_EN | NULL_CSTR,		/*  07 DEF_VARNAM		*/
  MMS_DEFSCAT_EN | NULL_CSTR,		/*  08 DEF_SCATTERED		*/
  MMS_GETSCAT_EN | CSTR_CSTR,		/*  09 GET_SCATTERED		*/
  MMS_DELVAR_EN | CSTR_CSTR,		/*  10 DEL_VARNAM		*/
  MMS_DEFVLIST_EN | NULL_CSTR,		/*  11 DEF_VARLIST		*/
  MMS_GETVLIST_EN | CSTR_CSTR,		/*  12 GET_VARLIST		*/
  MMS_DELVLIST_EN | CSTR_CSTR,		/*  13 DEL_VARLIST		*/
  MMS_DEFTYPE_EN | NULL_CSTR,		/*  14 DEF_TYPENAM		*/
  MMS_GETTYPE_EN | CSTR_CSTR,		/*  15 GET_TYPEDEF		*/
  MMS_DELTYPE_EN | CSTR_CSTR,		/*  16 DEL_TYPENAM		*/
  MMS_INPUT_EN | PRIM_CSTR,		/*  17 INPUT			*/
  MMS_OUTPUT_EN | NULL_CSTR,		/*  18 OUTPUT			*/
  MMS_TAKECTRL_EN | CSTR_CSTR,		/*  19 TAKE_CONTROL		*/
  MMS_RELCTRL_EN | NULL_CSTR,		/*  20 REL_CONTROL		*/
  MMS_DEFINE_SEM_EN | NULL_CSTR,	/*  21 DEF_SEMAPHORE		*/
  MMS_DELETE_SEM_EN | NULL_CSTR,	/*  22 DEL_SEMAPHORE		*/
  MMS_REP_SEMSTAT_EN | CSTR_CSTR,	/*  23 REP_SEM_STATUS		*/
  MMS_REP_SEMPOOL_EN | CSTR_CSTR,	/*  24 REP_SEM_POOL_STATUS	*/
  MMS_REP_SEMENTRY_EN | CSTR_CSTR,	/*  25 REP_SEM_ENTRY_STATUS	*/
  MMS_INIT_DWN_EN | CSTR_CSTR,		/*  26 INIT_DOWNLOAD		*/
  MMS_DWN_LOAD_EN | CSTR_PRIM,		/*  27 DOWN_LOAD		*/
  MMS_TERM_DWN_EN | NULL_CSTR,		/*  28 TERM_DOWNLOAD		*/
  MMS_INIT_UPL_EN | CSTR_PRIM,		/*  29 INIT_UPLOAD		*/
  MMS_UP_LOAD_EN | CSTR_PRIM,		/*  30 UP_LOAD			*/
  MMS_TERM_UPL_EN | NULL_PRIM,		/*  31 TERM_UPLOAD		*/
  MMS_RDDWN_EN | NULL_CSTR,		/*  32 REQ_DOWNLOAD		*/
  MMS_RDUPL_EN | NULL_CSTR,		/*  33 REQ_UPLOAD		*/
  MMS_LOAD_DOM_EN | NULL_CSTR,		/*  34 LOAD_DOMAIN		*/
  MMS_STR_DOM_EN | NULL_CSTR,		/*  35 STORE_DOMAIN		*/
  MMS_DEL_DOM_EN | NULL_PRIM,		/*  36 DELETE_DOMAIN		*/
  MMS_GET_DOM_EN | CSTR_PRIM,		/*  37 GET_DOMAIN		*/
  MMS_CRE_PI_EN | NULL_CSTR,		/*  38 CREATE_PROGRAM_INVOCATION*/
  MMS_DEL_PI_EN | NULL_PRIM,		/*  39 DEL_PROGRAM_INVOCATION	*/
  MMS_START_EN | NULL_CSTR,		/*  40 START			*/
  MMS_STOP_EN | NULL_CSTR,		/*  41 STOP			*/
  MMS_RESUME_EN | NULL_CSTR,		/*  42 RESUME			*/
  MMS_RESET_EN | NULL_CSTR,		/*  43 RESET			*/
  MMS_KILL_EN | NULL_CSTR,		/*  44 KILL			*/
  MMS_GET_PI_EN | CSTR_PRIM,		/*  45 GET_PROGRAM_INVOCATION	*/
  MMS_OBTAINFILE_EN | NULL_CSTR,	/*  46 OBTAIN_FILE		*/
  MMS_DEFEC_EN | NULL_CSTR,  		/*  47 DEFINE_EVENT_CONDITION	*/
  MMS_DELEC_EN | PRIM_CSTR,		/*  48 DELETE_EVENT_CONDITION	*/
  MMS_GETECA_EN | CSTR_CSTR,		/*  49 GET EVENT CONDITION ATTR	*/
  MMS_REPECS_EN | CSTR_CSTR,		/*  50 REPORT EVENT COND STATUS	*/
  MMS_ALTECM_EN | NULL_CSTR,   		/*  51 ALTER_EVENT_COND_MON	*/
  MMS_TRIGE_EN | NULL_CSTR,   		/*  52 TRIGGER_EVENT		*/
  MMS_DEFEA_EN | NULL_CSTR,   		/*  53 DEFINE_EVENT_ACTION	*/
  MMS_DELEA_EN | PRIM_CSTR,   		/*  54 DELETE_EVENT_ACTION	*/
  MMS_GETEAA_EN | CSTR_CSTR,   		/*  55 GET_EVENT_ACTION_ATTRIB	*/
  MMS_REPEAS_EN | PRIM_CSTR,   		/*  56 REPORT_EVENT_ACTION_STAT	*/
  MMS_DEFEE_EN | NULL_CSTR,   		/*  57 DEFINE_EVENT_ENROLLMENT	*/
  MMS_DELEE_EN | PRIM_CSTR,   		/*  58 DELETE_EVENT_ENROLLMENT	*/
  MMS_ALTEE_EN | CSTR_CSTR,   		/*  59 ALTER_EVENT_ENROLLMENT	*/
  MMS_REPEES_EN | CSTR_CSTR, 		/*  60 REPORT_EVENT_ENROLL_STAT	*/
  MMS_GETEEA_EN | CSTR_CSTR,   		/*  61 GET_EVENT_ENROLL_ATTRIB	*/
  MMS_ACKEVNOT_EN | NULL_CSTR,		/*  62 ACK_EVENT_NOTIFICATION	*/
  MMS_GETAS_EN | CSTR_CSTR,		/*  63 GET_ALARM_SUMMARY	*/
  MMS_GETAES_EN | CSTR_CSTR,   		/*  64 GET_ALARM_ENROLL_SUMMARY	*/
  MMS_JREAD_EN | CSTR_CSTR,		/*  65 READ_JOURNAL		*/
  MMS_JWRITE_EN | NULL_CSTR,		/*  66 WRITE_JOURNAL		*/
  MMS_JINIT_EN | PRIM_CSTR,		/*  67 INITIALIZE_JOURNAL	*/
  MMS_JSTAT_EN | CSTR_CSTR,		/*  68 REPORT_JOURNAL_STATUS	*/
  MMS_JCREATE_EN | NULL_CSTR,		/*  69 CREATE JOURNAL		*/
  MMS_JDELETE_EN | NULL_CSTR,		/*  70 DELETE JOURNAL		*/
  MMS_GETCL_EN | CSTR_CSTR,		/*  71 GET CAPABILITY LIST 	*/
  MMS_FOPEN_EN | CSTR_CSTR,		/*  72 FILE_OPEN		*/
  MMS_FREAD_EN | CSTR_PRIM,		/*  73 FILE_READ		*/
  MMS_FCLOSE_EN | NULL_PRIM,		/*  74 FILE_CLOSE		*/
  MMS_FRENAME_EN | NULL_CSTR,		/*  75 FILE_RENAME		*/
  MMS_FDELETE_EN | NULL_CSTR,		/*  76 FILE_DELETE		*/
  MMS_FDIR_EN | CSTR_CSTR,		/*  77 FILE_DIR 		*/
  MMS_USTATUS_EN | CSTR_CSTR,		/*  78 UNSOLICITED_STATUS	*/
  MMS_INFO_EN | CSTR_CSTR,		/*  79 INFO_RPT 		*/
  MMS_EVNOT_EN | CSTR_CSTR,		/*  80 EVENT NOTIFICATION	*/
  0,					/*  81 ATTACH TO EVENT COND	*/
  0,					/*  82 ATTACH TO SEMAPHORE	*/
  MMS_CONCLUDE_EN | NULL_NULL,		/*  83 CONCLUDE 		*/
  MMS_CANCEL_EN | PRIM_PRIM,		/*  84 CANCEL			*/
  MMS_INIT_EN | CSTR_CSTR		/*  85 INITIATE 		*/
  };					    

#if !defined (MMS_LITE) || defined (DEBUG_SISCO)
/************************************************************************/
/************************************************************************/
/* INITIALIZE THE OPCODE PRINT STRINGS					*/
/************************************************************************/



ST_CHAR *mms_op_string [] = 	  /* opcode is index into table	*/
  {
  "STATUS",			  /*  00 STATUS			*/
  "GET_NAMLIST",		  /*  01 GET_NAMLIST		*/
  "IDENTIFY",			  /*  02 IDENTIFY 		*/
  "RENAME",			  /*  03 RENAME			*/
  "READ",			  /*  04 READ			*/
  "WRITE",			  /*  05 WRITE			*/
  "GET_VARDEF",			  /*  06 GET_VARDEF		*/
  "DEF_VARNAM",			  /*  07 DEF_VARNAM		*/
  "DEF_SCATTERED",		  /*  08 DEF_SCATTERED		*/
  "GET_SCATTERED",		  /*  09 GET_SCATTERED		*/
  "DEL_VARNAM",			  /*  10 DEL_VARNAM		*/
  "DEF_VARLIST",		  /*  11 DEF_VARLIST		*/
  "GET_VARLIST",		  /*  12 GET_VARLIST		*/
  "DEL_VARLIST",		  /*  13 DEL_VARLIST		*/
  "DEF_TYPENAM",		  /*  14 DEF_TYPENAM		*/
  "GET_TYPEDEF",		  /*  15 GET_TYPEDEF		*/
  "DEL_TYPENAM",		  /*  16 DEL_TYPENAM		*/
  "INPUT",			  /*  17 INPUT			*/
  "OUTPUT",			  /*  18 OUTPUT			*/
  "TAKE_CONTROL",		  /*  19 TAKE_CONTROL		*/
  "REL_CONTROL",		  /*  20 REL_CONTROL		*/
  "DEF_SEMAPHORE",		  /*  21 DEF_SEMAPHORE		*/
  "DEL_SEMAPHORE",		  /*  22 DEL_SEMAPHORE		*/
  "REP_SEM_STATUS",		  /*  23 REP_SEM_STATUS		*/
  "REP_SEM_POOL_STATUS",	  /*  24 REP_SEM_POOL_STATUS	*/
  "REP_SEM_ENTRY_STATUS",	  /*  25 REP_SEM_ENTRY_STATUS	*/
  "INIT_DOWNLOAD",		  /*  26 INIT_DOWNLOAD		*/
  "DOWN_LOAD",			  /*  27 DOWN_LOAD		*/
  "TERM_DOWNLOAD",		  /*  28 TERM_DOWNLOAD		*/
  "INIT_UPLOAD",		  /*  29 INIT_UPLOAD		*/
  "UP_LOAD",			  /*  30 UP_LOAD		*/
  "TERM_UPLOAD",		  /*  31 TERM_UPLOAD		*/
  "REQ_DOWNLOAD",		  /*  32 REQ_DOWNLOAD		*/
  "REQ_UPLOAD",			  /*  33 REQ_UPLOAD		*/
  "LOAD_DOMAIN",		  /*  34 LOAD_DOMAIN		*/
  "STORE_DOMAIN",		  /*  35 STORE_DOMAIN		*/
  "DELETE_DOMAIN",		  /*  36 DELETE_DOMAIN		*/
  "GET_DOMAIN",			  /*  37 GET_DOMAIN		*/
  "CREATE_PROGRAM_INVOCATION",	  /*  38 CREATE_PROG_INVOCATION	*/
  "DEL_PROGRAM_INVOCATION",	  /*  39 DEL_PROGRAM_INVOCATION	*/
  "START",			  /*  40 START			*/
  "STOP",			  /*  41 STOP			*/
  "RESUME",			  /*  42 RESUME			*/
  "RESET",			  /*  43 RESET			*/
  "KILL",			  /*  44 KILL			*/
  "GET_PROGRAM_INVOCATION",	  /*  45 GET_PROGRAM_INVOCATION	*/
  "OBTAIN_FILE",		  /*  46 OBTAIN_FILE		*/
  "DEFINE_EVENT_COND",		  /*  47 DEFINE_EVENT_COND	*/
  "DELETE_EVENT_COND",		  /*  48 DELETE_EVENT_COND	*/
  "GET_EVENT_CONDITION_ATTR",	  /*  49 GET_EVENT_COND_ATTR	*/
  "REPORT_EVENT_COND_STATUS",	  /*  50 REPORT_EVENT_COND_STAT	*/
  "ALTER_EVENT_COND_MON", 	  /*  51 ALTER_EVENT_COND_MON  	*/
  "TRIGGER_EVENT",		  /*  52 TRIGGER_EVENT	       	*/
  "DEFINE_EVEVT_ACTION",  	  /*  53 DEFINE_EVENT_ACTION   	*/
  "DELETE_EVENT_ACTION",  	  /*  54 DELETE_EVENT_ACTION   	*/
  "GET_EVENT_ACTION_ATTR",	  /*  55 GET_EVENT_ACT_ATTR 	*/
  "REPORT_EVENT_ACTION_STAT", 	  /*  56 REPORT_EVENT_ACT_STAT	*/
  "DEFINE_EVENT_ENROLLMENT", 	  /*  57 DEFINE_EVENT_ENROLL	*/
  "DELETE_EVENT_ENROLLMENT", 	  /*  58 DELETE_EVENT_ENROLL	*/
  "ALTER_EVENT_ENROLLMENT", 	  /*  59 ALTER_EVENT_ENROLL	*/
  "REPORT_EVENT_ENROLL_STAT",	  /*  60 REPORT_EV_ENROLL_STAT	*/
  "GET_EVENT_ENROLL_ATTR",  	  /*  61 GET_EVENT_ENROLL_ATTR	*/
  "ACK_EVENT_NOTIFICATION",	  /*  62 ACK_EVENT_NOTIFICATION	*/
  "GET_ALARM_SUMMARY",		  /*  63 GET_ALARM_SUMMARY     	*/
  "GET_ALARM_ENROLL_SUMMARY",  	  /*  64 GET_ALARM_ENROLL_SUM	*/
  "READ_JOURNAL",		  /*  65 READ_JOURNAL	     	*/
  "WRITE_JOURNAL",		  /*  66 WRITE_JOURNAL		*/
  "INITIALIZE_JOURNAL",		  /*  67 INITIALIZE_JOURNAL	*/
  "REPORT_JOURNAL_STATUS",	  /*  68 REPORT_JOURNAL_STATUS	*/
  "CREATE JOURNAL",		  /*  69 CREATE JOURNAL		*/
  "DELETE JOURNAL",		  /*  70 DELETE JOURNAL		*/
  "GET CAPABILITY LIST",	  /*  71 GET CAPABILITY LIST 	*/
  "FILE_OPEN",			  /*  72 FILE_OPEN		*/
  "FILE_READ",			  /*  73 FILE_READ		*/
  "FILE_CLOSE",			  /*  74 FILE_CLOSE		*/
  "FILE_RENAME",		  /*  75 FILE_RENAME		*/
  "FILE_DELETE",		  /*  76 FILE_DELETE		*/
  "FILE_DIR",			  /*  77 FILE_DIR 		*/
  "UNSOLICITED_STATUS",		  /*  78 UNSOLICITED_STATUS	*/
  "INFO_RPT",			  /*  79 INFO_RPT 		*/
  "EVENT NOTIFICATION",		  /*  80 EVENT NOTIFICATION	*/
  "ATTACH TO EVENT CONDITION",	  /*  81 ATTACH TO EVENT COND	*/
  "ATTACH TO SEMAPHORE",	  /*  82 ATTACH TO SEMAPHORE	*/
  "CONCLUDE",			  /*  83 CONCLUDE 		*/
  "CANCEL",			  /*  84 CANCEL			*/
  "INITIATE",			  /*  85 INITIATE	 	*/
  "",				  /*  86 NOT A MMS OPERATION 	*/
  "",				  /*  87 NOT A MMS OPERATION 	*/
  "",				  /*  88 NOT A MMS OPERATION 	*/
  "",				  /*  89 NOT A MMS OPERATION 	*/
  "MV READ",			  /*  90 NAMED READ	 	*/
  "MV WRITE",			  /*  91 NAMED WRITE	 	*/
  "",				  /*  92 NOT A MMS OPERATION	*/
  "MV MVE_FOPEN",		  /*  93 REMOTE FILE OPEN 	*/
  "MV FREAD",			  /*  94 REMOTE FILE READ 	*/
  "MV FCLOSE",			  /*  95 REMOTE FILE CLOSE 	*/
  "MV INIT",			  /*  96 INITIATE 		*/
  "MV DEFTYPE",			  /*  97 TYPE DEFINITION	*/
  "MV FCOPY",			  /*  98 REMOTE FILE COPY 	*/
  "MV DOWNLOAD",		  /*  99 VM DOMAIN DOWNLOAD	*/
  "MV UPLOAD", 			  /* 100 VM DOMAIN UPLOAD	*/
  "MV READ VARS",		  /* 101 GENERAL READ	 	*/
  "MV WRITE VARS",		  /* 102 GENERAL WRITE	 	*/
  "",				  /* 103 NOT A MMS OPERATION 	*/
  "",				  /* 104 NOT A MMS OPERATION 	*/
  ""				  /* 105 NOT A MMS OPERATION 	*/
 };

ST_INT m_num_mms_op_string = sizeof (mms_op_string)/sizeof (ST_CHAR *);

#endif

/************************************************************************/
/************************************************************************/
/* The following defines are used in initializing the request and	*/
/* response decode functions, the indication and confirmation service	*/
/* funtions, and the supported service indicators.			*/
/************************************************************************/

/************************************************************************/
/************************************************************************/
/* VMD SUPPORT - CONFIRMED SERVICES   					*/
/************************************************************************/
#if (MMS_STATUS_EN & REQ_EN)
#define MMS_STATUS_RSP_DFUN		mms_status_rsp
#define MMS_STATUS_CONF_SFUN 		u_mp_status_conf
#else
#define MMS_STATUS_RSP_DFUN		mms_rsp_not_supp
#define MMS_STATUS_CONF_SFUN 		u_conf_not_supp
#endif
#if (MMS_STATUS_EN & RESP_EN)
#define MMS_STATUS_REQ_DFUN		mms_status_req
#define MMS_STATUS_IND_SFUN 		u_status_ind
#define STATUS_SSI			0x80
#else
#define MMS_STATUS_REQ_DFUN		mms_req_not_supp
#define MMS_STATUS_IND_SFUN 		u_ind_not_supp
#define STATUS_SSI			0x00
#endif

#if (MMS_GETNAMES_EN & REQ_EN)
#define MMS_GETNAMES_RSP_DFUN		mms_namelist_rsp
#define MMS_GETNAMES_CONF_SFUN 		u_mp_namelist_conf
#else
#define MMS_GETNAMES_RSP_DFUN		mms_rsp_not_supp
#define MMS_GETNAMES_CONF_SFUN 		u_conf_not_supp
#endif
#if (MMS_GETNAMES_EN & RESP_EN)
#define MMS_GETNAMES_REQ_DFUN		mms_namelist_req
#define MMS_GETNAMES_IND_SFUN 		u_namelist_ind
#define GETNAMES_SSI			0x40
#else
#define MMS_GETNAMES_REQ_DFUN		mms_req_not_supp
#define MMS_GETNAMES_IND_SFUN 		u_ind_not_supp
#define GETNAMES_SSI			0x00
#endif

#if (MMS_IDENT_EN & REQ_EN)
#define MMS_IDENT_RSP_DFUN		mms_identify_rsp
#define MMS_IDENT_CONF_SFUN 		u_mp_ident_conf
#else
#define MMS_IDENT_RSP_DFUN		mms_rsp_not_supp
#define MMS_IDENT_CONF_SFUN 		u_conf_not_supp
#endif
#if (MMS_IDENT_EN & RESP_EN)
#define MMS_IDENT_REQ_DFUN		_mms_null_pdu_dec
#define MMS_IDENT_IND_SFUN 		u_ident_ind
#define IDENT_SSI			0x20
#else
#define MMS_IDENT_REQ_DFUN		mms_req_not_supp
#define MMS_IDENT_IND_SFUN 		u_ind_not_supp
#define IDENT_SSI			0x00
#endif

#if (MMS_RENAME_EN & REQ_EN)
#define MMS_RENAME_RSP_DFUN		_mms_null_pdu_dec
#define MMS_RENAME_CONF_SFUN 		u_mp_rename_conf
#else
#define MMS_RENAME_RSP_DFUN		mms_rsp_not_supp
#define MMS_RENAME_CONF_SFUN 		u_conf_not_supp
#endif
#if (MMS_RENAME_EN & RESP_EN)
#define MMS_RENAME_REQ_DFUN		mms_rename_req
#define MMS_RENAME_IND_SFUN 		u_rename_ind
#define RENAME_SSI			0x10
#else
#define MMS_RENAME_REQ_DFUN		mms_req_not_supp
#define MMS_RENAME_IND_SFUN 		u_ind_not_supp
#define RENAME_SSI			0x00
#endif

#if (MMS_GETCL_EN & REQ_EN)
#define MMS_GETCL_RSP_DFUN		mms_getcl_rsp
#define MMS_GETCL_CONF_SFUN 		u_mp_getcl_conf
#else
#define MMS_GETCL_RSP_DFUN		mms_rsp_not_supp
#define MMS_GETCL_CONF_SFUN 		u_conf_not_supp
#endif
#if (MMS_GETCL_EN & RESP_EN)
#define MMS_GETCL_REQ_DFUN		mms_getcl_req
#define MMS_GETCL_IND_SFUN 		u_getcl_ind
#define GETCL_SSI			0x01
#else
#define MMS_GETCL_REQ_DFUN		mms_req_not_supp
#define MMS_GETCL_IND_SFUN 		u_ind_not_supp
#define GETCL_SSI			0x00
#endif

/************************************************************************/
/* VARIABLE ACCESS - CONFIRMED SERVICES					*/
/************************************************************************/

/* VM named read */
#if (MMS_MV_READ_EN & REQ_EN)	
#define MMS_MV_READ_CONF_SFUN 		u_mv_read_conf
#else
#define MMS_MV_READ_CONF_SFUN 		u_conf_not_supp
#endif

/* VM general read */
#if (MMS_MV_RDVARS_EN & REQ_EN)
#define MMS_MV_RDVARS_CONF_SFUN   	u_mv_read_vars_conf
#else
#define MMS_MV_RDVARS_CONF_SFUN  	u_conf_not_supp
#endif

#if (MMS_READ_EN & REQ_EN)
#define MMS_READ_RSP_DFUN		mms_read_rsp
#define MMS_READ_CONF_SFUN 		u_mp_read_conf
#else
#define MMS_READ_RSP_DFUN		mms_rsp_not_supp
#define MMS_READ_CONF_SFUN 		u_conf_not_supp
#endif
#if (MMS_READ_EN & RESP_EN)
#define MMS_READ_REQ_DFUN		mms_read_req
#define MMS_READ_IND_SFUN 		u_read_ind
#define READ_SSI			0x08
#else
#define MMS_READ_REQ_DFUN		mms_req_not_supp
#define MMS_READ_IND_SFUN 		u_ind_not_supp
#define READ_SSI			0x00
#endif

/* VM named write */
#if (MMS_MV_WRITE_EN & REQ_EN)
#define MMS_MV_WRITE_CONF_SFUN 		u_mv_write_conf
#else
#define MMS_MV_WRITE_CONF_SFUN 		u_conf_not_supp
#endif

/* VM general write */
#if (MMS_MV_WRVARS_EN & REQ_EN)
#define MMS_MV_WRVARS_CONF_SFUN   	u_mv_write_vars_conf
#else
#define MMS_MV_WRVARS_CONF_SFUN  	u_conf_not_supp
#endif

#if (MMS_WRITE_EN & REQ_EN)
#define MMS_WRITE_RSP_DFUN		mms_write_rsp
#define MMS_WRITE_CONF_SFUN 		u_mp_write_conf
#else
#define MMS_WRITE_RSP_DFUN		mms_rsp_not_supp
#define MMS_WRITE_CONF_SFUN 		u_conf_not_supp
#endif
#if (MMS_WRITE_EN & RESP_EN)
#define MMS_WRITE_REQ_DFUN		mms_write_req
#define MMS_WRITE_IND_SFUN 		u_write_ind
#define WRITE_SSI			0x04
#else
#define MMS_WRITE_REQ_DFUN		mms_req_not_supp
#define MMS_WRITE_IND_SFUN 		u_ind_not_supp
#define WRITE_SSI			0x00
#endif

#if (MMS_GETVAR_EN & REQ_EN)
#define MMS_GETVAR_RSP_DFUN		mms_get_var_rsp
#define MMS_GETVAR_CONF_SFUN 		u_mp_getvar_conf
#else
#define MMS_GETVAR_RSP_DFUN		mms_rsp_not_supp
#define MMS_GETVAR_CONF_SFUN 		u_conf_not_supp
#endif
#if (MMS_GETVAR_EN & RESP_EN)
#define MMS_GETVAR_REQ_DFUN		mms_get_var_req
#define MMS_GETVAR_IND_SFUN 		u_getvar_ind
#define GETVAR_SSI			0x02
#else
#define MMS_GETVAR_REQ_DFUN		mms_req_not_supp
#define MMS_GETVAR_IND_SFUN 		u_ind_not_supp
#define GETVAR_SSI			0x00
#endif

#if (MMS_DEFVAR_EN & REQ_EN)
#define MMS_DEFVAR_RSP_DFUN		_mms_null_pdu_dec
#define MMS_DEFVAR_CONF_SFUN 		u_mp_defvar_conf
#else
#define MMS_DEFVAR_RSP_DFUN		mms_rsp_not_supp
#define MMS_DEFVAR_CONF_SFUN 		u_conf_not_supp
#endif
#if (MMS_DEFVAR_EN & RESP_EN)
#define MMS_DEFVAR_REQ_DFUN		mms_def_var_req
#define MMS_DEFVAR_IND_SFUN 		u_defvar_ind
#define DEFVAR_SSI			0x01
#else
#define MMS_DEFVAR_REQ_DFUN		mms_req_not_supp
#define MMS_DEFVAR_IND_SFUN 		u_ind_not_supp
#define DEFVAR_SSI			0x00
#endif

#if (MMS_DEFSCAT_EN & REQ_EN)
#define MMS_DEFSCAT_RSP_DFUN		_mms_null_pdu_dec
#define MMS_DEFSCAT_CONF_SFUN 		u_mp_defscat_conf
#else
#define MMS_DEFSCAT_RSP_DFUN		mms_rsp_not_supp
#define MMS_DEFSCAT_CONF_SFUN 		u_conf_not_supp
#endif
#if (MMS_DEFSCAT_EN & RESP_EN)
#define MMS_DEFSCAT_REQ_DFUN		mms_def_scat_req
#define MMS_DEFSCAT_IND_SFUN 		u_defscat_ind
#define DEFSCAT_SSI			0x80
#else
#define MMS_DEFSCAT_REQ_DFUN		mms_req_not_supp
#define MMS_DEFSCAT_IND_SFUN 		u_ind_not_supp
#define DEFSCAT_SSI			0x00
#endif

#if (MMS_GETSCAT_EN & REQ_EN)
#define MMS_GETSCAT_RSP_DFUN		mms_get_scat_rsp
#define MMS_GETSCAT_CONF_SFUN 		u_mp_getscat_conf
#else
#define MMS_GETSCAT_RSP_DFUN		mms_rsp_not_supp
#define MMS_GETSCAT_CONF_SFUN 		u_conf_not_supp
#endif
#if (MMS_GETSCAT_EN & RESP_EN)
#define MMS_GETSCAT_REQ_DFUN		mms_get_scat_req
#define MMS_GETSCAT_IND_SFUN 		u_getscat_ind
#define GETSCAT_SSI			0x40
#else
#define MMS_GETSCAT_REQ_DFUN		mms_req_not_supp
#define MMS_GETSCAT_IND_SFUN 		u_ind_not_supp
#define GETSCAT_SSI			0x00
#endif

#if (MMS_DELVAR_EN & REQ_EN)
#define MMS_DELVAR_RSP_DFUN		mms_del_var_rsp
#define MMS_DELVAR_CONF_SFUN 		u_mp_delvar_conf
#else
#define MMS_DELVAR_RSP_DFUN		mms_rsp_not_supp
#define MMS_DELVAR_CONF_SFUN 		u_conf_not_supp
#endif
#if (MMS_DELVAR_EN & RESP_EN)
#define MMS_DELVAR_REQ_DFUN		mms_del_var_req
#define MMS_DELVAR_IND_SFUN 		u_delvar_ind
#define DELVAR_SSI			0x20
#else
#define MMS_DELVAR_REQ_DFUN		mms_req_not_supp
#define MMS_DELVAR_IND_SFUN 		u_ind_not_supp
#define DELVAR_SSI			0x00
#endif

#if (MMS_DEFVLIST_EN & REQ_EN)
#define MMS_DEFVLIST_RSP_DFUN		_mms_null_pdu_dec
#define MMS_DEFVLIST_CONF_SFUN 		u_mp_defvlist_conf
#else
#define MMS_DEFVLIST_RSP_DFUN		mms_rsp_not_supp
#define MMS_DEFVLIST_CONF_SFUN 		u_conf_not_supp
#endif
#if (MMS_DEFVLIST_EN & RESP_EN)
#define MMS_DEFVLIST_REQ_DFUN		mms_def_vlist_req
#define MMS_DEFVLIST_IND_SFUN 		u_defvlist_ind
#define DEFVLIST_SSI			0x10
#else
#define MMS_DEFVLIST_REQ_DFUN		mms_req_not_supp
#define MMS_DEFVLIST_IND_SFUN 		u_ind_not_supp
#define DEFVLIST_SSI			0x00
#endif

#if (MMS_GETVLIST_EN & REQ_EN)
#define MMS_GETVLIST_RSP_DFUN		mms_get_vlist_rsp
#define MMS_GETVLIST_CONF_SFUN 		u_mp_getvlist_conf
#else
#define MMS_GETVLIST_RSP_DFUN		mms_rsp_not_supp
#define MMS_GETVLIST_CONF_SFUN 		u_conf_not_supp
#endif
#if (MMS_GETVLIST_EN & RESP_EN)
#define MMS_GETVLIST_REQ_DFUN		mms_get_vlist_req
#define MMS_GETVLIST_IND_SFUN 		u_getvlist_ind
#define GETVLIST_SSI			0x08
#else
#define MMS_GETVLIST_REQ_DFUN		mms_req_not_supp
#define MMS_GETVLIST_IND_SFUN 		u_ind_not_supp
#define GETVLIST_SSI			0x00
#endif

#if (MMS_DELVLIST_EN & REQ_EN)
#define MMS_DELVLIST_RSP_DFUN		mms_del_vlist_rsp
#define MMS_DELVLIST_CONF_SFUN 		u_mp_delvlist_conf
#else
#define MMS_DELVLIST_RSP_DFUN		mms_rsp_not_supp
#define MMS_DELVLIST_CONF_SFUN 		u_conf_not_supp
#endif
#if (MMS_DELVLIST_EN & RESP_EN)
#define MMS_DELVLIST_REQ_DFUN		mms_del_vlist_req
#define MMS_DELVLIST_IND_SFUN 		u_delvlist_ind
#define DELVLIST_SSI			0x04
#else
#define MMS_DELVLIST_REQ_DFUN		mms_req_not_supp
#define MMS_DELVLIST_IND_SFUN 		u_ind_not_supp
#define DELVLIST_SSI			0x00
#endif

#if (MMS_MV_DEFTYPE_EN & REQ_EN)
#define MMS_MV_DEFTYPE_CONF_SFUN 	u_mv_deftype_conf
#else
#define MMS_MV_DEFTYPE_CONF_SFUN 	u_conf_not_supp
#endif

#if (MMS_DEFTYPE_EN & REQ_EN)
#define MMS_DEFTYPE_RSP_DFUN		_mms_null_pdu_dec
#define MMS_DEFTYPE_CONF_SFUN 		u_mp_deftype_conf
#else
#define MMS_DEFTYPE_RSP_DFUN		mms_rsp_not_supp
#define MMS_DEFTYPE_CONF_SFUN 		u_conf_not_supp
#endif
#if (MMS_DEFTYPE_EN & RESP_EN)
#define MMS_DEFTYPE_REQ_DFUN		mms_def_type_req
#define MMS_DEFTYPE_IND_SFUN 		u_deftype_ind
#define DEFTYPE_SSI			0x02
#else
#define MMS_DEFTYPE_REQ_DFUN		mms_req_not_supp
#define MMS_DEFTYPE_IND_SFUN 		u_ind_not_supp
#define DEFTYPE_SSI			0x00
#endif

#if (MMS_GETTYPE_EN & REQ_EN)
#define MMS_GETTYPE_RSP_DFUN		mms_get_type_rsp
#define MMS_GETTYPE_CONF_SFUN 		u_mp_gettype_conf
#else
#define MMS_GETTYPE_RSP_DFUN		mms_rsp_not_supp
#define MMS_GETTYPE_CONF_SFUN 		u_conf_not_supp
#endif
#if (MMS_GETTYPE_EN & RESP_EN)
#define MMS_GETTYPE_REQ_DFUN		mms_get_type_req
#define MMS_GETTYPE_IND_SFUN 		u_gettype_ind
#define GETTYPE_SSI			0x01
#else
#define MMS_GETTYPE_REQ_DFUN		mms_req_not_supp
#define MMS_GETTYPE_IND_SFUN 		u_ind_not_supp
#define GETTYPE_SSI			0x00
#endif

#if (MMS_DELTYPE_EN & REQ_EN)
#define MMS_DELTYPE_RSP_DFUN		mms_del_type_rsp
#define MMS_DELTYPE_CONF_SFUN 		u_mp_deltype_conf
#else
#define MMS_DELTYPE_RSP_DFUN		mms_rsp_not_supp
#define MMS_DELTYPE_CONF_SFUN 		u_conf_not_supp
#endif
#if (MMS_DELTYPE_EN & RESP_EN)
#define MMS_DELTYPE_REQ_DFUN		mms_del_type_req
#define MMS_DELTYPE_IND_SFUN 		u_deltype_ind
#define DELTYPE_SSI			0x80
#else
#define MMS_DELTYPE_REQ_DFUN		mms_req_not_supp
#define MMS_DELTYPE_IND_SFUN 		u_ind_not_supp
#define DELTYPE_SSI			0x00
#endif

/************************************************************************/
/* OPERATOR COMMUNICATION SERVICES   					*/
/************************************************************************/

#if (MMS_INPUT_EN & REQ_EN)
#define MMS_INPUT_RSP_DFUN		mms_input_rsp
#define MMS_INPUT_CONF_SFUN 		u_mp_input_conf
#else
#define MMS_INPUT_RSP_DFUN		mms_rsp_not_supp
#define MMS_INPUT_CONF_SFUN 		u_conf_not_supp
#endif
#if (MMS_INPUT_EN & RESP_EN)
#define MMS_INPUT_REQ_DFUN		mms_input_req
#define MMS_INPUT_IND_SFUN 		u_input_ind
#define INPUT_SSI			0x40
#else
#define MMS_INPUT_REQ_DFUN		mms_req_not_supp
#define MMS_INPUT_IND_SFUN 		u_ind_not_supp
#define INPUT_SSI			0x00
#endif

#if (MMS_OUTPUT_EN & REQ_EN)
#define MMS_OUTPUT_RSP_DFUN		_mms_null_pdu_dec
#define MMS_OUTPUT_CONF_SFUN 		u_mp_output_conf
#else
#define MMS_OUTPUT_RSP_DFUN		mms_rsp_not_supp
#define MMS_OUTPUT_CONF_SFUN 		u_conf_not_supp
#endif
#if (MMS_OUTPUT_EN & RESP_EN)
#define MMS_OUTPUT_REQ_DFUN		mms_output_req
#define MMS_OUTPUT_IND_SFUN 		u_output_ind
#define OUTPUT_SSI			0x20
#else
#define MMS_OUTPUT_REQ_DFUN		mms_req_not_supp
#define MMS_OUTPUT_IND_SFUN 		u_ind_not_supp
#define OUTPUT_SSI			0x00
#endif

/************************************************************************/
/* SEMAPHORE MANAGEMENT SERVICES 					*/
/************************************************************************/

#if (MMS_TAKECTRL_EN & REQ_EN)
#define MMS_TAKECTRL_RSP_DFUN		mms_takectrl_rsp
#define MMS_TAKECTRL_CONF_SFUN 		u_mp_takectrl_conf
#else
#define MMS_TAKECTRL_RSP_DFUN		mms_rsp_not_supp
#define MMS_TAKECTRL_CONF_SFUN 		u_conf_not_supp
#endif
#if (MMS_TAKECTRL_EN & RESP_EN)
#define MMS_TAKECTRL_REQ_DFUN		mms_takectrl_req
#define MMS_TAKECTRL_IND_SFUN 		u_takectrl_ind
#define TAKECTRL_SSI			0x10
#else
#define MMS_TAKECTRL_REQ_DFUN		mms_req_not_supp
#define MMS_TAKECTRL_IND_SFUN 		u_ind_not_supp
#define TAKECTRL_SSI			0x00
#endif

#if (MMS_RELCTRL_EN & REQ_EN)
#define MMS_RELCTRL_RSP_DFUN		_mms_null_pdu_dec
#define MMS_RELCTRL_CONF_SFUN 		u_mp_relctrl_conf
#else
#define MMS_RELCTRL_RSP_DFUN		mms_rsp_not_supp
#define MMS_RELCTRL_CONF_SFUN 		u_conf_not_supp
#endif
#if (MMS_RELCTRL_EN & RESP_EN)
#define MMS_RELCTRL_REQ_DFUN		mms_relctrl_req
#define MMS_RELCTRL_IND_SFUN 		u_relctrl_ind
#define RELCTRL_SSI			0x08
#else
#define MMS_RELCTRL_REQ_DFUN		mms_req_not_supp
#define MMS_RELCTRL_IND_SFUN 		u_ind_not_supp
#define RELCTRL_SSI			0x00
#endif

#if (MMS_DEFINE_SEM_EN & REQ_EN)
#define MMS_DEFINE_SEM_RSP_DFUN		_mms_null_pdu_dec
#define MMS_DEFINE_SEM_CONF_SFUN 	u_mp_defsem_conf
#else
#define MMS_DEFINE_SEM_RSP_DFUN		mms_rsp_not_supp
#define MMS_DEFINE_SEM_CONF_SFUN 	u_conf_not_supp
#endif

#if (MMS_DEFINE_SEM_EN & RESP_EN)
#define MMS_DEFINE_SEM_REQ_DFUN		mms_defsem_req
#define MMS_DEFINE_SEM_IND_SFUN 	u_defsem_ind
#define DEFSEM_SSI			0x04
#else
#define MMS_DEFINE_SEM_REQ_DFUN		mms_req_not_supp
#define MMS_DEFINE_SEM_IND_SFUN 	u_ind_not_supp
#define DEFSEM_SSI			0x00
#endif

#if (MMS_DELETE_SEM_EN & REQ_EN)
#define MMS_DELETE_SEM_RSP_DFUN		_mms_null_pdu_dec
#define MMS_DELETE_SEM_CONF_SFUN 	u_mp_delsem_conf
#else
#define MMS_DELETE_SEM_RSP_DFUN		mms_rsp_not_supp
#define MMS_DELETE_SEM_CONF_SFUN 	u_conf_not_supp
#endif

#if (MMS_DELETE_SEM_EN & RESP_EN)
#define MMS_DELETE_SEM_REQ_DFUN		mms_delsem_req
#define MMS_DELETE_SEM_IND_SFUN 	u_delsem_ind
#define DELSEM_SSI			0x02
#else
#define MMS_DELETE_SEM_REQ_DFUN		mms_req_not_supp
#define MMS_DELETE_SEM_IND_SFUN 	u_ind_not_supp
#define DELSEM_SSI			0x00
#endif

#if (MMS_REP_SEMSTAT_EN & REQ_EN)
#define MMS_REP_SEMSTAT_RSP_DFUN	mms_rsstat_rsp
#define MMS_REP_SEMSTAT_CONF_SFUN 	u_mp_rsstat_conf
#else
#define MMS_REP_SEMSTAT_RSP_DFUN	mms_rsp_not_supp
#define MMS_REP_SEMSTAT_CONF_SFUN 	u_conf_not_supp
#endif
#if (MMS_REP_SEMSTAT_EN & RESP_EN)
#define MMS_REP_SEMSTAT_REQ_DFUN	mms_rsstat_req
#define MMS_REP_SEMSTAT_IND_SFUN 	u_rsstat_ind
#define RSSTAT_SSI			0x01
#else
#define MMS_REP_SEMSTAT_REQ_DFUN	mms_req_not_supp
#define MMS_REP_SEMSTAT_IND_SFUN 	u_ind_not_supp
#define RSSTAT_SSI			0x00
#endif

#if (MMS_REP_SEMPOOL_EN & REQ_EN)
#define MMS_REP_SEMPOOL_RSP_DFUN	mms_rspool_rsp
#define MMS_REP_SEMPOOL_CONF_SFUN 	u_mp_rspool_conf
#else
#define MMS_REP_SEMPOOL_RSP_DFUN	mms_rsp_not_supp
#define MMS_REP_SEMPOOL_CONF_SFUN 	u_conf_not_supp
#endif

#if (MMS_REP_SEMPOOL_EN & RESP_EN)
#define MMS_REP_SEMPOOL_REQ_DFUN	mms_rspool_req
#define MMS_REP_SEMPOOL_IND_SFUN 	u_rspool_ind
#define RSPOOL_SSI			0x80
#else
#define MMS_REP_SEMPOOL_REQ_DFUN	mms_req_not_supp
#define MMS_REP_SEMPOOL_IND_SFUN 	u_ind_not_supp
#define RSPOOL_SSI			0x00
#endif

#if (MMS_REP_SEMENTRY_EN & REQ_EN)
#define MMS_REP_SEMENTRY_RSP_DFUN	mms_rsentry_rsp
#define MMS_REP_SEMENTRY_CONF_SFUN 	u_mp_rsentry_conf
#else
#define MMS_REP_SEMENTRY_RSP_DFUN	mms_rsp_not_supp
#define MMS_REP_SEMENTRY_CONF_SFUN 	u_conf_not_supp
#endif

#if (MMS_REP_SEMENTRY_EN & RESP_EN)
#define MMS_REP_SEMENTRY_REQ_DFUN	mms_rsentry_req
#define MMS_REP_SEMENTRY_IND_SFUN 	u_rsentry_ind
#define RSENTRY_SSI			0x40
#else
#define MMS_REP_SEMENTRY_REQ_DFUN	mms_req_not_supp
#define MMS_REP_SEMENTRY_IND_SFUN 	u_ind_not_supp
#define RSENTRY_SSI			0x00
#endif

/************************************************************************/
/* DOMAIN MANAGEMENT SERVICES 						*/
/************************************************************************/

#if (MMS_INIT_DWN_EN & REQ_EN)
#define MMS_INIT_DWN_RSP_DFUN		mms_init_download_rsp
#define MMS_INIT_DWN_CONF_SFUN 		u_mp_initdown_conf
#else
#define MMS_INIT_DWN_RSP_DFUN		mms_rsp_not_supp
#define MMS_INIT_DWN_CONF_SFUN 		u_conf_not_supp
#endif

#if (MMS_INIT_DWN_EN & RESP_EN)
#define MMS_INIT_DWN_REQ_DFUN		mms_init_download_req
#define MMS_INIT_DWN_IND_SFUN 		u_initdown_ind
#define INITDWN_SSI			0x20
#else
#define MMS_INIT_DWN_REQ_DFUN		mms_req_not_supp
#define MMS_INIT_DWN_IND_SFUN 		u_ind_not_supp
#define INITDWN_SSI			0x00
#endif

#if (MMS_DWN_LOAD_EN & REQ_EN)
#define MMS_DWN_LOAD_RSP_DFUN		mms_download_rsp
#define MMS_DWN_LOAD_CONF_SFUN 		u_mp_download_conf
#else
#define MMS_DWN_LOAD_RSP_DFUN		mms_rsp_not_supp
#define MMS_DWN_LOAD_CONF_SFUN 		u_conf_not_supp
#endif

#if (MMS_DWN_LOAD_EN & RESP_EN)
#define MMS_DWN_LOAD_REQ_DFUN		mms_download_req
#define MMS_DWN_LOAD_IND_SFUN 		u_download_ind
#define DOWNLOAD_SSI			0x10
#else
#define MMS_DWN_LOAD_REQ_DFUN		mms_req_not_supp
#define MMS_DWN_LOAD_IND_SFUN 		u_ind_not_supp
#define DOWNLOAD_SSI			0x00
#endif

#if (MMS_TERM_DWN_EN & REQ_EN)
#define MMS_TERM_DWN_RSP_DFUN		_mms_null_pdu_dec
#define MMS_TERM_DWN_CONF_SFUN 		u_mp_termdown_conf
#else
#define MMS_TERM_DWN_RSP_DFUN		mms_rsp_not_supp
#define MMS_TERM_DWN_CONF_SFUN 		u_conf_not_supp
#endif

#if (MMS_TERM_DWN_EN & RESP_EN)
#define MMS_TERM_DWN_REQ_DFUN		mms_term_download_req
#define MMS_TERM_DWN_IND_SFUN 		u_termdown_ind
#define TERMDOWN_SSI			0x08
#else
#define MMS_TERM_DWN_REQ_DFUN		mms_req_not_supp
#define MMS_TERM_DWN_IND_SFUN 		u_ind_not_supp
#define TERMDOWN_SSI			0x00
#endif

#if (MMS_MV_DWN_EN & REQ_EN)
#define MMS_MV_DOWNLOAD_CONF_SFUN 	u_mv_download_conf
#else
#define MMS_MV_DOWNLOAD_CONF_SFUN 	u_conf_not_supp
#endif

#if (MMS_INIT_UPL_EN & REQ_EN)
#define MMS_INIT_UPL_RSP_DFUN		mms_init_upload_rsp
#define MMS_INIT_UPL_CONF_SFUN 		u_mp_initupl_conf
#else
#define MMS_INIT_UPL_RSP_DFUN		mms_rsp_not_supp
#define MMS_INIT_UPL_CONF_SFUN 		u_conf_not_supp
#endif

#if (MMS_INIT_UPL_EN & RESP_EN)
#define MMS_INIT_UPL_REQ_DFUN		mms_init_upload_req
#define MMS_INIT_UPL_IND_SFUN 		u_initupl_ind
#define INITUPL_SSI			0x04
#else
#define MMS_INIT_UPL_REQ_DFUN		mms_req_not_supp
#define MMS_INIT_UPL_IND_SFUN 		u_ind_not_supp
#define INITUPL_SSI			0x00
#endif

#if (MMS_UP_LOAD_EN & REQ_EN)
#define MMS_UP_LOAD_RSP_DFUN		mms_upload_rsp
#define MMS_UP_LOAD_CONF_SFUN 		u_mp_upload_conf
#else
#define MMS_UP_LOAD_RSP_DFUN		mms_rsp_not_supp
#define MMS_UP_LOAD_CONF_SFUN 		u_conf_not_supp
#endif

#if (MMS_UP_LOAD_EN & RESP_EN)
#define MMS_UP_LOAD_REQ_DFUN		mms_upload_req
#define MMS_UP_LOAD_IND_SFUN 		u_upload_ind
#define UPLOAD_SSI			0x02
#else
#define MMS_UP_LOAD_REQ_DFUN		mms_req_not_supp
#define MMS_UP_LOAD_IND_SFUN 		u_ind_not_supp
#define UPLOAD_SSI			0x00
#endif

#if (MMS_TERM_UPL_EN & REQ_EN)
#define MMS_TERM_UPL_RSP_DFUN		_mms_null_pdu_dec
#define MMS_TERM_UPL_CONF_SFUN 		u_mp_termupl_conf
#else
#define MMS_TERM_UPL_RSP_DFUN		mms_rsp_not_supp
#define MMS_TERM_UPL_CONF_SFUN 		u_conf_not_supp
#endif

#if (MMS_TERM_UPL_EN & RESP_EN)
#define MMS_TERM_UPL_REQ_DFUN		mms_term_upload_req
#define MMS_TERM_UPL_IND_SFUN 		u_termupl_ind
#define TERMUPL_SSI			0x01
#else
#define MMS_TERM_UPL_REQ_DFUN		mms_req_not_supp
#define MMS_TERM_UPL_IND_SFUN 		u_ind_not_supp
#define TERMUPL_SSI			0x00
#endif

#if (MMS_MV_UPL_EN & REQ_EN)
#define MMS_MV_UPLOAD_CONF_SFUN 	u_mv_upload_conf
#else
#define MMS_MV_UPLOAD_CONF_SFUN 	u_conf_not_supp
#endif

#if (MMS_RDDWN_EN & REQ_EN)
#define MMS_RDDWN_RSP_DFUN		_mms_null_pdu_dec
#define MMS_RDDWN_CONF_SFUN 		u_mp_rddwn_conf
#else
#define MMS_RDDWN_RSP_DFUN		mms_rsp_not_supp
#define MMS_RDDWN_CONF_SFUN 		u_conf_not_supp
#endif

#if (MMS_RDDWN_EN & RESP_EN)
#define MMS_RDDWN_REQ_DFUN		mms_rddwn_req
#define MMS_RDDWN_IND_SFUN 		u_rddwn_ind
#define RDDWN_SSI			0x80
#else
#define MMS_RDDWN_REQ_DFUN		mms_req_not_supp
#define MMS_RDDWN_IND_SFUN 		u_ind_not_supp
#define RDDWN_SSI			0x00
#endif

#if (MMS_RDUPL_EN & REQ_EN)
#define MMS_RDUPL_RSP_DFUN		_mms_null_pdu_dec
#define MMS_RDUPL_CONF_SFUN 		u_mp_rdupl_conf
#else
#define MMS_RDUPL_RSP_DFUN		mms_rsp_not_supp
#define MMS_RDUPL_CONF_SFUN 		u_conf_not_supp
#endif

#if (MMS_RDUPL_EN & RESP_EN)
#define MMS_RDUPL_REQ_DFUN		mms_rdupl_req
#define MMS_RDUPL_IND_SFUN 		u_rdupl_ind
#define RDUPL_SSI			0x40
#else
#define MMS_RDUPL_REQ_DFUN		mms_req_not_supp
#define MMS_RDUPL_IND_SFUN 		u_ind_not_supp
#define RDUPL_SSI			0x00
#endif

#if (MMS_LOAD_DOM_EN & REQ_EN)
#define MMS_LOAD_DOM_RSP_DFUN		_mms_null_pdu_dec
#define MMS_LOAD_DOM_CONF_SFUN 		u_mp_loaddom_conf
#else
#define MMS_LOAD_DOM_RSP_DFUN		mms_rsp_not_supp
#define MMS_LOAD_DOM_CONF_SFUN 		u_conf_not_supp
#endif

#if (MMS_LOAD_DOM_EN & RESP_EN)
#define MMS_LOAD_DOM_REQ_DFUN		mms_load_domain_req
#define MMS_LOAD_DOM_IND_SFUN 		u_loaddom_ind
#define LOADDOM_SSI			0x20
#else
#define MMS_LOAD_DOM_REQ_DFUN		mms_req_not_supp
#define MMS_LOAD_DOM_IND_SFUN 		u_ind_not_supp
#define LOADDOM_SSI			0x00
#endif

#if (MMS_STR_DOM_EN & REQ_EN)
#define MMS_STR_DOM_RSP_DFUN		_mms_null_pdu_dec
#define MMS_STR_DOM_CONF_SFUN 		u_mp_storedom_conf
#else
#define MMS_STR_DOM_RSP_DFUN		mms_rsp_not_supp
#define MMS_STR_DOM_CONF_SFUN 		u_conf_not_supp
#endif

#if (MMS_STR_DOM_EN & RESP_EN)
#define MMS_STR_DOM_REQ_DFUN		mms_store_domain_req
#define MMS_STR_DOM_IND_SFUN 		u_storedom_ind
#define STRDOM_SSI			0x10
#else
#define MMS_STR_DOM_REQ_DFUN		mms_req_not_supp
#define MMS_STR_DOM_IND_SFUN 		u_ind_not_supp
#define STRDOM_SSI			0x00
#endif

#if (MMS_DEL_DOM_EN & REQ_EN)
#define MMS_DEL_DOM_RSP_DFUN		_mms_null_pdu_dec
#define MMS_DEL_DOM_CONF_SFUN 		u_mp_deldom_conf
#else
#define MMS_DEL_DOM_RSP_DFUN		mms_rsp_not_supp
#define MMS_DEL_DOM_CONF_SFUN 		u_conf_not_supp
#endif
#if (MMS_DEL_DOM_EN & RESP_EN)
#define MMS_DEL_DOM_REQ_DFUN		mms_delete_domain_req
#define MMS_DEL_DOM_IND_SFUN 		u_deldom_ind
#define DELDOM_SSI			0x08
#else
#define MMS_DEL_DOM_REQ_DFUN		mms_req_not_supp
#define MMS_DEL_DOM_IND_SFUN 		u_ind_not_supp
#define DELDOM_SSI			0x00
#endif

#if (MMS_GET_DOM_EN & REQ_EN)
#define MMS_GET_DOM_RSP_DFUN		mms_get_dom_attr_rsp
#define MMS_GET_DOM_CONF_SFUN 		u_mp_getdom_conf
#else
#define MMS_GET_DOM_RSP_DFUN		mms_rsp_not_supp
#define MMS_GET_DOM_CONF_SFUN 		u_conf_not_supp
#endif
#if (MMS_GET_DOM_EN & RESP_EN)
#define MMS_GET_DOM_REQ_DFUN		mms_get_dom_attr_req
#define MMS_GET_DOM_IND_SFUN 		u_getdom_ind
#define GETDOM_SSI			0x04
#else
#define MMS_GET_DOM_REQ_DFUN		mms_req_not_supp
#define MMS_GET_DOM_IND_SFUN 		u_ind_not_supp
#define GETDOM_SSI			0x00
#endif

/************************************************************************/
/* PROGRAM INVOCATION MANAGEMENT SERVICES 				*/
/************************************************************************/

#if (MMS_CRE_PI_EN & REQ_EN)
#define MMS_CRE_PI_RSP_DFUN		_mms_null_pdu_dec
#define MMS_CRE_PI_CONF_SFUN 		u_mp_crepi_conf
#else
#define MMS_CRE_PI_RSP_DFUN		mms_rsp_not_supp
#define MMS_CRE_PI_CONF_SFUN 		u_conf_not_supp
#endif
#if (MMS_CRE_PI_EN & RESP_EN)
#define MMS_CRE_PI_REQ_DFUN		mms_create_pi_req
#define MMS_CRE_PI_IND_SFUN 		u_crepi_ind
#define CREPI_SSI			0x02
#else
#define MMS_CRE_PI_REQ_DFUN		mms_req_not_supp
#define MMS_CRE_PI_IND_SFUN 		u_ind_not_supp
#define CREPI_SSI			0x00
#endif

#if (MMS_DEL_PI_EN & REQ_EN)
#define MMS_DEL_PI_RSP_DFUN		_mms_null_pdu_dec
#define MMS_DEL_PI_CONF_SFUN 		u_mp_delpi_conf
#else
#define MMS_DEL_PI_RSP_DFUN		mms_rsp_not_supp
#define MMS_DEL_PI_CONF_SFUN 		u_conf_not_supp
#endif
#if (MMS_DEL_PI_EN & RESP_EN)
#define MMS_DEL_PI_REQ_DFUN		mms_delete_pi_req
#define MMS_DEL_PI_IND_SFUN 		u_delpi_ind
#define DELPI_SSI			0x01
#else
#define MMS_DEL_PI_REQ_DFUN		mms_req_not_supp
#define MMS_DEL_PI_IND_SFUN 		u_ind_not_supp
#define DELPI_SSI			0x00
#endif

#if (MMS_START_EN & REQ_EN)
#define MMS_START_RSP_DFUN		_mms_null_pdu_dec
#define MMS_START_CONF_SFUN 		u_mp_start_conf
#else
#define MMS_START_RSP_DFUN		mms_rsp_not_supp
#define MMS_START_CONF_SFUN 		u_conf_not_supp
#endif
#if (MMS_START_EN & RESP_EN)
#define MMS_START_REQ_DFUN		mms_start_req
#define MMS_START_IND_SFUN 		u_start_ind
#define START_SSI			0x80
#else
#define MMS_START_REQ_DFUN		mms_req_not_supp
#define MMS_START_IND_SFUN 		u_ind_not_supp
#define START_SSI			0x00
#endif

#if (MMS_STOP_EN & REQ_EN)
#define MMS_STOP_RSP_DFUN		_mms_null_pdu_dec
#define MMS_STOP_CONF_SFUN 		u_mp_stop_conf
#else
#define MMS_STOP_RSP_DFUN		mms_rsp_not_supp
#define MMS_STOP_CONF_SFUN 		u_conf_not_supp
#endif
#if (MMS_STOP_EN & RESP_EN)
#define MMS_STOP_REQ_DFUN		mms_stop_req
#define MMS_STOP_IND_SFUN 		u_stop_ind
#define STOP_SSI			0x40
#else
#define MMS_STOP_REQ_DFUN		mms_req_not_supp
#define MMS_STOP_IND_SFUN 		u_ind_not_supp
#define STOP_SSI			0x00
#endif

#if (MMS_RESUME_EN & REQ_EN)
#define MMS_RESUME_RSP_DFUN		_mms_null_pdu_dec
#define MMS_RESUME_CONF_SFUN 		u_mp_resume_conf
#else
#define MMS_RESUME_RSP_DFUN		mms_rsp_not_supp
#define MMS_RESUME_CONF_SFUN 		u_conf_not_supp
#endif
#if (MMS_RESUME_EN & RESP_EN)
#define MMS_RESUME_REQ_DFUN		mms_resume_req
#define MMS_RESUME_IND_SFUN 		u_resume_ind
#define RESUME_SSI			0x20
#else
#define MMS_RESUME_REQ_DFUN		mms_req_not_supp
#define MMS_RESUME_IND_SFUN 		u_ind_not_supp
#define RESUME_SSI			0x00
#endif

#if (MMS_RESET_EN & REQ_EN)
#define MMS_RESET_RSP_DFUN		_mms_null_pdu_dec
#define MMS_RESET_CONF_SFUN 		u_mp_reset_conf
#else
#define MMS_RESET_RSP_DFUN		mms_rsp_not_supp
#define MMS_RESET_CONF_SFUN 		u_conf_not_supp
#endif
#if (MMS_RESET_EN & RESP_EN)
#define MMS_RESET_REQ_DFUN		mms_reset_req
#define MMS_RESET_IND_SFUN 		u_reset_ind
#define RESET_SSI			0x10
#else
#define MMS_RESET_REQ_DFUN		mms_req_not_supp
#define MMS_RESET_IND_SFUN 		u_ind_not_supp
#define RESET_SSI			0x00
#endif

#if (MMS_KILL_EN & REQ_EN)
#define MMS_KILL_RSP_DFUN		_mms_null_pdu_dec
#define MMS_KILL_CONF_SFUN 		u_mp_kill_conf
#else
#define MMS_KILL_RSP_DFUN		mms_rsp_not_supp
#define MMS_KILL_CONF_SFUN 		u_conf_not_supp
#endif
#if (MMS_KILL_EN & RESP_EN)
#define MMS_KILL_REQ_DFUN		mms_kill_req
#define MMS_KILL_IND_SFUN 		u_kill_ind
#define KILL_SSI			0x08
#else
#define MMS_KILL_REQ_DFUN		mms_req_not_supp
#define MMS_KILL_IND_SFUN 		u_ind_not_supp
#define KILL_SSI			0x00
#endif

#if (MMS_GET_PI_EN & REQ_EN)
#define MMS_GET_PI_RSP_DFUN		mms_get_pi_rsp
#define MMS_GET_PI_CONF_SFUN 		u_mp_getpi_conf
#else
#define MMS_GET_PI_RSP_DFUN		mms_rsp_not_supp
#define MMS_GET_PI_CONF_SFUN 		u_conf_not_supp
#endif
#if (MMS_GET_PI_EN & RESP_EN)
#define MMS_GET_PI_REQ_DFUN		mms_get_pi_req
#define MMS_GET_PI_IND_SFUN 		u_getpi_ind
#define GETPI_SSI			0x04
#else
#define MMS_GET_PI_REQ_DFUN		mms_req_not_supp
#define MMS_GET_PI_IND_SFUN 		u_ind_not_supp
#define GETPI_SSI			0x00
#endif

/************************************************************************/
/* EVENT MANAGEMENT - CONFIRMED SERVICES 				*/
/************************************************************************/

#if (MMS_DEFEC_EN & REQ_EN)
#define MMS_DEFEC_RSP_DFUN		_mms_null_pdu_dec
#define MMS_DEFEC_CONF_SFUN 		u_mp_defec_conf
#else
#define MMS_DEFEC_RSP_DFUN		mms_rsp_not_supp
#define MMS_DEFEC_CONF_SFUN 		u_conf_not_supp
#endif
#if (MMS_DEFEC_EN & RESP_EN)
#define MMS_DEFEC_REQ_DFUN		mms_defec_req
#define MMS_DEFEC_IND_SFUN 		u_defec_ind
#define DEFEC_SSI			0x01
#else
#define MMS_DEFEC_REQ_DFUN		mms_req_not_supp
#define MMS_DEFEC_IND_SFUN 		u_ind_not_supp
#define DEFEC_SSI			0x00
#endif

#if (MMS_DELEC_EN & REQ_EN)
#define MMS_DELEC_RSP_DFUN		mms_delec_rsp
#define MMS_DELEC_CONF_SFUN 		u_mp_delec_conf
#else
#define MMS_DELEC_RSP_DFUN		mms_rsp_not_supp
#define MMS_DELEC_CONF_SFUN 		u_conf_not_supp
#endif
#if (MMS_DELEC_EN & RESP_EN)
#define MMS_DELEC_REQ_DFUN		mms_delec_req
#define MMS_DELEC_IND_SFUN 		u_delec_ind
#define DELEC_SSI			0x80
#else
#define MMS_DELEC_REQ_DFUN		mms_req_not_supp
#define MMS_DELEC_IND_SFUN 		u_ind_not_supp
#define DELEC_SSI			0x00
#endif

#if (MMS_GETECA_EN & REQ_EN)
#define MMS_GETECA_RSP_DFUN		mms_geteca_rsp
#define MMS_GETECA_CONF_SFUN 		u_mp_geteca_conf
#else
#define MMS_GETECA_RSP_DFUN		mms_rsp_not_supp
#define MMS_GETECA_CONF_SFUN 		u_conf_not_supp
#endif
#if (MMS_GETECA_EN & RESP_EN)
#define MMS_GETECA_REQ_DFUN		mms_geteca_req
#define MMS_GETECA_IND_SFUN 		u_geteca_ind
#define GETECA_SSI			0x40
#else
#define MMS_GETECA_REQ_DFUN		mms_req_not_supp
#define MMS_GETECA_IND_SFUN 		u_ind_not_supp
#define GETECA_SSI			0x00
#endif

#if (MMS_REPECS_EN & REQ_EN)
#define MMS_REPECS_RSP_DFUN		mms_repecs_rsp
#define MMS_REPECS_CONF_SFUN 		u_mp_repecs_conf
#else
#define MMS_REPECS_RSP_DFUN		mms_rsp_not_supp
#define MMS_REPECS_CONF_SFUN 		u_conf_not_supp
#endif
#if (MMS_REPECS_EN & RESP_EN)
#define MMS_REPECS_REQ_DFUN		mms_repecs_req
#define MMS_REPECS_IND_SFUN 		u_repecs_ind
#define REPECS_SSI			0x20
#else
#define MMS_REPECS_REQ_DFUN		mms_req_not_supp
#define MMS_REPECS_IND_SFUN 		u_ind_not_supp
#define REPECS_SSI			0x00
#endif

#if (MMS_ALTECM_EN & REQ_EN)
#define MMS_ALTECM_RSP_DFUN		_mms_null_pdu_dec
#define MMS_ALTECM_CONF_SFUN 		u_mp_altecm_conf
#else
#define MMS_ALTECM_RSP_DFUN		mms_rsp_not_supp
#define MMS_ALTECM_CONF_SFUN 		u_conf_not_supp
#endif
#if (MMS_ALTECM_EN & RESP_EN)
#define MMS_ALTECM_REQ_DFUN		mms_altecm_req
#define MMS_ALTECM_IND_SFUN 		u_altecm_ind
#define ALTECM_SSI			0x10
#else
#define MMS_ALTECM_REQ_DFUN		mms_req_not_supp
#define MMS_ALTECM_IND_SFUN 		u_ind_not_supp
#define ALTECM_SSI			0x00
#endif

#if (MMS_TRIGE_EN & REQ_EN)
#define MMS_TRIGE_RSP_DFUN		_mms_null_pdu_dec
#define MMS_TRIGE_CONF_SFUN 		u_mp_trige_conf
#else
#define MMS_TRIGE_RSP_DFUN		mms_rsp_not_supp
#define MMS_TRIGE_CONF_SFUN 		u_conf_not_supp
#endif
#if (MMS_TRIGE_EN & RESP_EN)
#define MMS_TRIGE_REQ_DFUN		mms_trige_req
#define MMS_TRIGE_IND_SFUN 		u_trige_ind
#define TRIGE_SSI			0x08
#else
#define MMS_TRIGE_REQ_DFUN		mms_req_not_supp
#define MMS_TRIGE_IND_SFUN 		u_ind_not_supp
#define TRIGE_SSI			0x00
#endif

#if (MMS_DEFEA_EN & REQ_EN)
#define MMS_DEFEA_RSP_DFUN		_mms_null_pdu_dec
#define MMS_DEFEA_CONF_SFUN 		u_mp_defea_conf
#else
#define MMS_DEFEA_RSP_DFUN		mms_rsp_not_supp
#define MMS_DEFEA_CONF_SFUN 		u_conf_not_supp
#endif
#if (MMS_DEFEA_EN & RESP_EN)
#define MMS_DEFEA_REQ_DFUN		mms_defea_req
#define MMS_DEFEA_IND_SFUN 		u_defea_ind
#define DEFEA_SSI			0x04
#else
#define MMS_DEFEA_REQ_DFUN		mms_req_not_supp
#define MMS_DEFEA_IND_SFUN 		u_ind_not_supp
#define DEFEA_SSI			0x00
#endif

#if (MMS_DELEA_EN & REQ_EN)
#define MMS_DELEA_RSP_DFUN		mms_delea_rsp
#define MMS_DELEA_CONF_SFUN 		u_mp_delea_conf
#else
#define MMS_DELEA_RSP_DFUN		mms_rsp_not_supp
#define MMS_DELEA_CONF_SFUN 		u_conf_not_supp
#endif
#if (MMS_DELEA_EN & RESP_EN)
#define MMS_DELEA_REQ_DFUN		mms_delea_req
#define MMS_DELEA_IND_SFUN 		u_delea_ind
#define DELEA_SSI			0x02
#else
#define MMS_DELEA_REQ_DFUN		mms_req_not_supp
#define MMS_DELEA_IND_SFUN 		u_ind_not_supp
#define DELEA_SSI			0x00
#endif

#if (MMS_GETEAA_EN & REQ_EN)
#define MMS_GETEAA_RSP_DFUN		mms_geteaa_rsp
#define MMS_GETEAA_CONF_SFUN 		u_mp_geteaa_conf
#else
#define MMS_GETEAA_RSP_DFUN		mms_rsp_not_supp
#define MMS_GETEAA_CONF_SFUN 		u_conf_not_supp
#endif
#if (MMS_GETEAA_EN & RESP_EN)
#define MMS_GETEAA_REQ_DFUN		mms_geteaa_req
#define MMS_GETEAA_IND_SFUN 		u_geteaa_ind
#define GETEAA_SSI			0x01
#else
#define MMS_GETEAA_REQ_DFUN		mms_req_not_supp
#define MMS_GETEAA_IND_SFUN 		u_ind_not_supp
#define GETEAA_SSI			0x00
#endif

#if (MMS_REPEAS_EN & REQ_EN)
#define MMS_REPEAS_RSP_DFUN		mms_repeas_rsp
#define MMS_REPEAS_CONF_SFUN 		u_mp_repeas_conf
#else
#define MMS_REPEAS_RSP_DFUN		mms_rsp_not_supp
#define MMS_REPEAS_CONF_SFUN 		u_conf_not_supp
#endif
#if (MMS_REPEAS_EN & RESP_EN)
#define MMS_REPEAS_REQ_DFUN		mms_repeas_req
#define MMS_REPEAS_IND_SFUN 		u_repeas_ind
#define REPEAS_SSI			0x80
#else
#define MMS_REPEAS_REQ_DFUN		mms_req_not_supp
#define MMS_REPEAS_IND_SFUN 		u_ind_not_supp
#define REPEAS_SSI			0x00
#endif

#if (MMS_DEFEE_EN & REQ_EN)
#define MMS_DEFEE_RSP_DFUN		_mms_null_pdu_dec
#define MMS_DEFEE_CONF_SFUN 		u_mp_defee_conf
#else
#define MMS_DEFEE_RSP_DFUN		mms_rsp_not_supp
#define MMS_DEFEE_CONF_SFUN 		u_conf_not_supp
#endif
#if (MMS_DEFEE_EN & RESP_EN)
#define MMS_DEFEE_REQ_DFUN		mms_defee_req
#define MMS_DEFEE_IND_SFUN 		u_defee_ind
#define DEFEE_SSI			0x40
#else
#define MMS_DEFEE_REQ_DFUN		mms_req_not_supp
#define MMS_DEFEE_IND_SFUN 		u_ind_not_supp
#define DEFEE_SSI			0x00
#endif

#if (MMS_DELEE_EN & REQ_EN)
#define MMS_DELEE_RSP_DFUN		mms_delee_rsp
#define MMS_DELEE_CONF_SFUN 		u_mp_delee_conf
#else
#define MMS_DELEE_RSP_DFUN		mms_rsp_not_supp
#define MMS_DELEE_CONF_SFUN 		u_conf_not_supp
#endif
#if (MMS_DELEE_EN & RESP_EN)
#define MMS_DELEE_REQ_DFUN		mms_delee_req
#define MMS_DELEE_IND_SFUN 		u_delee_ind
#define DELEE_SSI			0x20
#else
#define MMS_DELEE_REQ_DFUN		mms_req_not_supp
#define MMS_DELEE_IND_SFUN 		u_ind_not_supp
#define DELEE_SSI			0x00
#endif

#if (MMS_ALTEE_EN & REQ_EN)
#define MMS_ALTEE_RSP_DFUN		mms_altee_rsp
#define MMS_ALTEE_CONF_SFUN 		u_mp_altee_conf
#else
#define MMS_ALTEE_RSP_DFUN		mms_rsp_not_supp
#define MMS_ALTEE_CONF_SFUN 		u_conf_not_supp
#endif
#if (MMS_ALTEE_EN & RESP_EN)
#define MMS_ALTEE_REQ_DFUN		mms_altee_req
#define MMS_ALTEE_IND_SFUN 		u_altee_ind
#define ALTEE_SSI			0x10
#else
#define MMS_ALTEE_REQ_DFUN		mms_req_not_supp
#define MMS_ALTEE_IND_SFUN 		u_ind_not_supp
#define ALTEE_SSI			0x00
#endif

#if (MMS_REPEES_EN & REQ_EN)
#define MMS_REPEES_RSP_DFUN		mms_repees_rsp
#define MMS_REPEES_CONF_SFUN 		u_mp_repees_conf
#else
#define MMS_REPEES_RSP_DFUN		mms_rsp_not_supp
#define MMS_REPEES_CONF_SFUN 		u_conf_not_supp
#endif
#if (MMS_REPEES_EN & RESP_EN)
#define MMS_REPEES_REQ_DFUN		mms_repees_req
#define MMS_REPEES_IND_SFUN 		u_repees_ind
#define REPEES_SSI			0x08
#else
#define MMS_REPEES_REQ_DFUN		mms_req_not_supp
#define MMS_REPEES_IND_SFUN 		u_ind_not_supp
#define REPEES_SSI			0x00
#endif

#if (MMS_GETEEA_EN & REQ_EN)
#define MMS_GETEEA_RSP_DFUN		mms_geteea_rsp
#define MMS_GETEEA_CONF_SFUN 		u_mp_geteea_conf
#else
#define MMS_GETEEA_RSP_DFUN		mms_rsp_not_supp
#define MMS_GETEEA_CONF_SFUN 		u_conf_not_supp
#endif
#if (MMS_GETEEA_EN & RESP_EN)
#define MMS_GETEEA_REQ_DFUN		mms_geteea_req
#define MMS_GETEEA_IND_SFUN 		u_geteea_ind
#define GETEEA_SSI			0x04
#else
#define MMS_GETEEA_REQ_DFUN		mms_req_not_supp
#define MMS_GETEEA_IND_SFUN 		u_ind_not_supp
#define GETEEA_SSI			0x00
#endif

#if (MMS_ACKEVNOT_EN & REQ_EN)
#define MMS_ACKEVNOT_RSP_DFUN		_mms_null_pdu_dec
#define MMS_ACKEVNOT_CONF_SFUN 		u_mp_ackevnot_conf
#else
#define MMS_ACKEVNOT_RSP_DFUN		mms_rsp_not_supp
#define MMS_ACKEVNOT_CONF_SFUN 		u_conf_not_supp
#endif
#if (MMS_ACKEVNOT_EN & RESP_EN)
#define MMS_ACKEVNOT_REQ_DFUN		mms_ackevnot_req
#define MMS_ACKEVNOT_IND_SFUN 		u_ackevnot_ind
#define ACKEVNOT_SSI			0x02
#else
#define MMS_ACKEVNOT_REQ_DFUN		mms_req_not_supp
#define MMS_ACKEVNOT_IND_SFUN 		u_ind_not_supp
#define ACKEVNOT_SSI			0x00
#endif

#if (MMS_GETAS_EN & REQ_EN)
#define MMS_GETAS_RSP_DFUN		mms_getas_rsp
#define MMS_GETAS_CONF_SFUN 		u_mp_getas_conf
#else
#define MMS_GETAS_RSP_DFUN		mms_rsp_not_supp
#define MMS_GETAS_CONF_SFUN 		u_conf_not_supp
#endif
#if (MMS_GETAS_EN & RESP_EN)
#define MMS_GETAS_REQ_DFUN		mms_getas_req
#define MMS_GETAS_IND_SFUN 		u_getas_ind
#define GETAS_SSI			0x01
#else
#define MMS_GETAS_REQ_DFUN		mms_req_not_supp
#define MMS_GETAS_IND_SFUN 		u_ind_not_supp
#define GETAS_SSI			0x00
#endif

#if (MMS_GETAES_EN & REQ_EN)
#define MMS_GETAES_RSP_DFUN		mms_getaes_rsp
#define MMS_GETAES_CONF_SFUN 		u_mp_getaes_conf
#else
#define MMS_GETAES_RSP_DFUN		mms_rsp_not_supp
#define MMS_GETAES_CONF_SFUN 		u_conf_not_supp
#endif
#if (MMS_GETAES_EN & RESP_EN)
#define MMS_GETAES_REQ_DFUN		mms_getaes_req
#define MMS_GETAES_IND_SFUN 		u_getaes_ind
#define GETAES_SSI			0x80
#else
#define MMS_GETAES_REQ_DFUN		mms_req_not_supp
#define MMS_GETAES_IND_SFUN 		u_ind_not_supp
#define GETAES_SSI			0x00
#endif

/************************************************************************/
/* JOURNAL MANAGEMENT SERVICES 						*/
/************************************************************************/

#if (MMS_JREAD_EN & REQ_EN)
#define MMS_JREAD_RSP_DFUN		mms_jread_rsp
#define MMS_JREAD_CONF_SFUN 		u_mp_jread_conf
#else
#define MMS_JREAD_RSP_DFUN		mms_rsp_not_supp
#define MMS_JREAD_CONF_SFUN 		u_conf_not_supp
#endif
#if (MMS_JREAD_EN & RESP_EN)
#define MMS_JREAD_REQ_DFUN		mms_jread_req
#define MMS_JREAD_IND_SFUN 		u_jread_ind
#define JREAD_SSI			0x40
#else
#define MMS_JREAD_REQ_DFUN		mms_req_not_supp
#define MMS_JREAD_IND_SFUN 		u_ind_not_supp
#define JREAD_SSI			0x00
#endif

#if (MMS_JWRITE_EN & REQ_EN)
#define MMS_JWRITE_RSP_DFUN		_mms_null_pdu_dec
#define MMS_JWRITE_CONF_SFUN 		u_mp_jwrite_conf
#else
#define MMS_JWRITE_RSP_DFUN		mms_rsp_not_supp
#define MMS_JWRITE_CONF_SFUN 		u_conf_not_supp
#endif
#if (MMS_JWRITE_EN & RESP_EN)
#define MMS_JWRITE_REQ_DFUN		mms_jwrite_req
#define MMS_JWRITE_IND_SFUN 		u_jwrite_ind
#define JWRITE_SSI			0x20
#else
#define MMS_JWRITE_REQ_DFUN		mms_req_not_supp
#define MMS_JWRITE_IND_SFUN 		u_ind_not_supp
#define JWRITE_SSI			0x00
#endif

#if (MMS_JINIT_EN & REQ_EN)
#define MMS_JINIT_RSP_DFUN		mms_jinit_rsp
#define MMS_JINIT_CONF_SFUN 		u_mp_jinit_conf
#else
#define MMS_JINIT_RSP_DFUN		mms_rsp_not_supp
#define MMS_JINIT_CONF_SFUN 		u_conf_not_supp
#endif
#if (MMS_JINIT_EN & RESP_EN)
#define MMS_JINIT_REQ_DFUN		mms_jinit_req
#define MMS_JINIT_IND_SFUN 		u_jinit_ind
#define JINIT_SSI			0x10
#else
#define MMS_JINIT_REQ_DFUN		mms_req_not_supp
#define MMS_JINIT_IND_SFUN 		u_ind_not_supp
#define JINIT_SSI			0x00
#endif

#if (MMS_JSTAT_EN & REQ_EN)
#define MMS_JSTAT_RSP_DFUN		mms_jstat_rsp
#define MMS_JSTAT_CONF_SFUN 		u_mp_jstat_conf
#else
#define MMS_JSTAT_RSP_DFUN		mms_rsp_not_supp
#define MMS_JSTAT_CONF_SFUN 		u_conf_not_supp
#endif
#if (MMS_JSTAT_EN & RESP_EN)
#define MMS_JSTAT_REQ_DFUN		mms_jstat_req
#define MMS_JSTAT_IND_SFUN 		u_jstat_ind
#define JSTAT_SSI			0x08
#else
#define MMS_JSTAT_REQ_DFUN		mms_req_not_supp
#define MMS_JSTAT_IND_SFUN 		u_ind_not_supp
#define JSTAT_SSI			0x00
#endif

#if (MMS_JCREATE_EN & REQ_EN)
#define MMS_JCREATE_RSP_DFUN		_mms_null_pdu_dec
#define MMS_JCREATE_CONF_SFUN 		u_mp_jcreate_conf
#else
#define MMS_JCREATE_RSP_DFUN		mms_rsp_not_supp
#define MMS_JCREATE_CONF_SFUN 		u_conf_not_supp
#endif
#if (MMS_JCREATE_EN & RESP_EN)
#define MMS_JCREATE_REQ_DFUN		mms_jcreate_req
#define MMS_JCREATE_IND_SFUN 		u_jcreate_ind
#define JCREATE_SSI			0x04
#else
#define MMS_JCREATE_REQ_DFUN		mms_req_not_supp
#define MMS_JCREATE_IND_SFUN 		u_ind_not_supp
#define JCREATE_SSI			0x00
#endif

#if (MMS_JDELETE_EN & REQ_EN)
#define MMS_JDELETE_RSP_DFUN		_mms_null_pdu_dec
#define MMS_JDELETE_CONF_SFUN 		u_mp_jdelete_conf
#else
#define MMS_JDELETE_RSP_DFUN		mms_rsp_not_supp
#define MMS_JDELETE_CONF_SFUN 		u_conf_not_supp
#endif
#if (MMS_JDELETE_EN & RESP_EN)
#define MMS_JDELETE_REQ_DFUN		mms_jdelete_req
#define MMS_JDELETE_IND_SFUN 		u_jdelete_ind
#define JDELETE_SSI			0x02
#else
#define MMS_JDELETE_REQ_DFUN		mms_req_not_supp
#define MMS_JDELETE_IND_SFUN 		u_ind_not_supp
#define JDELETE_SSI			0x00
#endif

/************************************************************************/
/* FILE MANAGEMENT SERVICES 						*/
/************************************************************************/

#if (MMS_OBTAINFILE_EN & REQ_EN)
#define MMS_OBTAINFILE_RSP_DFUN		_mms_null_pdu_dec
#define MMS_OBTAINFILE_CONF_SFUN 	u_mp_obtfile_conf
#else
#define MMS_OBTAINFILE_RSP_DFUN		mms_rsp_not_supp
#define MMS_OBTAINFILE_CONF_SFUN 	u_conf_not_supp
#endif
#if (MMS_OBTAINFILE_EN & RESP_EN)
#define MMS_OBTAINFILE_REQ_DFUN		mms_obtain_file_req
#define MMS_OBTAINFILE_IND_SFUN 	u_obtfile_ind
#define OBTFILE_SSI			0x02
#else
#define MMS_OBTAINFILE_REQ_DFUN		mms_req_not_supp
#define MMS_OBTAINFILE_IND_SFUN 	u_ind_not_supp
#define OBTFILE_SSI			0x00
#endif

#if (MMS_MV_FOPEN_EN & REQ_EN)
#define MMS_MV_FOPEN_CONF_SFUN 		u_mv_fopen_conf
#else
#define MMS_MV_FOPEN_CONF_SFUN 		u_conf_not_supp
#endif

#if (MMS_FOPEN_EN & REQ_EN)
#define MMS_FOPEN_RSP_DFUN		mms_file_open_rsp
#define MMS_FOPEN_CONF_SFUN 		u_mp_fopen_conf
#else
#define MMS_FOPEN_RSP_DFUN		mms_rsp_not_supp
#define MMS_FOPEN_CONF_SFUN 		u_conf_not_supp
#endif
#if (MMS_FOPEN_EN & RESP_EN)
#define MMS_FOPEN_REQ_DFUN		mms_file_open_req
#define MMS_FOPEN_IND_SFUN 		u_fopen_ind
#define FOPEN_SSI			0x80
#else
#define MMS_FOPEN_REQ_DFUN		mms_req_not_supp
#define MMS_FOPEN_IND_SFUN 		u_ind_not_supp
#define FOPEN_SSI			0x00
#endif

#if (MMS_MV_FREAD_EN & REQ_EN)
#define MMS_MV_FREAD_CONF_SFUN 		u_mv_fread_conf
#else
#define MMS_MV_FREAD_CONF_SFUN 		u_conf_not_supp
#endif

#if (MMS_FREAD_EN & REQ_EN)
#define MMS_FREAD_RSP_DFUN		mms_file_read_rsp
#define MMS_FREAD_CONF_SFUN 		u_mp_fread_conf
#else
#define MMS_FREAD_RSP_DFUN		mms_rsp_not_supp
#define MMS_FREAD_CONF_SFUN 		u_conf_not_supp
#endif
#if (MMS_FREAD_EN & RESP_EN)
#define MMS_FREAD_REQ_DFUN		mms_file_read_req
#define MMS_FREAD_IND_SFUN 		u_fread_ind
#define FREAD_SSI			0x40
#else
#define MMS_FREAD_REQ_DFUN		mms_req_not_supp
#define MMS_FREAD_IND_SFUN 		u_ind_not_supp
#define FREAD_SSI			0x00
#endif

#if (MMS_MV_FCLOSE_EN & REQ_EN)
#define MMS_MV_FCLOSE_CONF_SFUN 	u_mv_fclose_conf
#else
#define MMS_MV_FCLOSE_CONF_SFUN 	u_conf_not_supp
#endif

#if (MMS_FCLOSE_EN & REQ_EN)
#define MMS_FCLOSE_RSP_DFUN		_mms_null_pdu_dec
#define MMS_FCLOSE_CONF_SFUN 		u_mp_fclose_conf
#else
#define MMS_FCLOSE_RSP_DFUN		mms_rsp_not_supp
#define MMS_FCLOSE_CONF_SFUN 		u_conf_not_supp
#endif
#if (MMS_FCLOSE_EN & RESP_EN)
#define MMS_FCLOSE_REQ_DFUN		mms_file_close_req
#define MMS_FCLOSE_IND_SFUN 		u_fclose_ind
#define FCLOSE_SSI			0x20
#else
#define MMS_FCLOSE_REQ_DFUN		mms_req_not_supp
#define MMS_FCLOSE_IND_SFUN 		u_ind_not_supp
#define FCLOSE_SSI			0x00
#endif

#if (MMS_MV_FCOPY_EN & REQ_EN)
#define MMS_MV_FCOPY_CONF_SFUN 		u_mv_fcopy_conf
#else
#define MMS_MV_FCOPY_CONF_SFUN 		u_conf_not_supp
#endif

#if (MMS_FRENAME_EN & REQ_EN)
#define MMS_FRENAME_RSP_DFUN		_mms_null_pdu_dec
#define MMS_FRENAME_CONF_SFUN 		u_mp_frename_conf
#else
#define MMS_FRENAME_RSP_DFUN		mms_rsp_not_supp
#define MMS_FRENAME_CONF_SFUN 		u_conf_not_supp
#endif
#if (MMS_FRENAME_EN & RESP_EN)
#define MMS_FRENAME_REQ_DFUN		mms_file_rename_req
#define MMS_FRENAME_IND_SFUN 		u_frename_ind
#define FRENAME_SSI			0x10
#else
#define MMS_FRENAME_REQ_DFUN		mms_req_not_supp
#define MMS_FRENAME_IND_SFUN 		u_ind_not_supp
#define FRENAME_SSI			0x00
#endif

#if (MMS_FDELETE_EN & REQ_EN)
#define MMS_FDELETE_RSP_DFUN		_mms_null_pdu_dec
#define MMS_FDELETE_CONF_SFUN 		u_mp_fdelete_conf
#else
#define MMS_FDELETE_RSP_DFUN		mms_rsp_not_supp
#define MMS_FDELETE_CONF_SFUN 		u_conf_not_supp
#endif
#if (MMS_FDELETE_EN & RESP_EN)
#define MMS_FDELETE_REQ_DFUN		mms_file_delete_req
#define MMS_FDELETE_IND_SFUN 		u_fdelete_ind
#define FDELETE_SSI			0x08
#else
#define MMS_FDELETE_REQ_DFUN		mms_req_not_supp
#define MMS_FDELETE_IND_SFUN 		u_ind_not_supp
#define FDELETE_SSI			0x00
#endif

#if (MMS_FDIR_EN & REQ_EN)
#define MMS_FDIR_RSP_DFUN		mms_file_dir_rsp
#define MMS_FDIR_CONF_SFUN 		u_mp_fdir_conf
#else
#define MMS_FDIR_RSP_DFUN		mms_rsp_not_supp
#define MMS_FDIR_CONF_SFUN 		u_conf_not_supp
#endif
#if (MMS_FDIR_EN & RESP_EN)
#define MMS_FDIR_REQ_DFUN		mms_file_dir_req
#define MMS_FDIR_IND_SFUN 		u_fdir_ind
#define FDIR_SSI			0x04
#else
#define MMS_FDIR_REQ_DFUN		mms_req_not_supp
#define MMS_FDIR_IND_SFUN 		u_ind_not_supp
#define FDIR_SSI			0x00
#endif

/************************************************************************/
/************************************************************************/
/* UNCONFIRMED SERVICES FROM VARIABLE ACCESS, VMD SUPPORT AND EVENT	*/
/* MANAGEMENT SERVICES							*/
/************************************************************************/
#if (MMS_INFO_EN & RESP_EN)
#define MMS_INFO_REQ_DFUN		mms_info_rpt_req
#define MMS_INFO_IND_SFUN 		u_info_ind
#define INFO_SSI			0x01
#else
#define MMS_INFO_REQ_DFUN		mms_req_not_supp
#define MMS_INFO_IND_SFUN 		u_ind_not_supp
#define INFO_SSI			0x00
#endif

#if (MMS_USTATUS_EN & RESP_EN)
#define MMS_USTATUS_REQ_DFUN		mms_ustatus_req
#define MMS_USTATUS_IND_SFUN 		u_ustatus_ind
#define USTATUS_SSI			0x02
#else
#define MMS_USTATUS_REQ_DFUN		mms_req_not_supp
#define MMS_USTATUS_IND_SFUN 		u_ind_not_supp
#define USTATUS_SSI			0x00
#endif

#if (MMS_EVNOT_EN & RESP_EN)
#define MMS_EVNOT_REQ_DFUN		mms_evnot_req
#define MMS_EVNOT_IND_SFUN 		u_evnot_ind
#define EVNOT_SSI			0x80
#else
#define MMS_EVNOT_REQ_DFUN		mms_req_not_supp
#define MMS_EVNOT_IND_SFUN 		u_ind_not_supp
#define EVNOT_SSI			0x00
#endif

/************************************************************************/
/************************************************************************/
/* ENVIRONMENT & GENERAL MANAGEMENT					*/
/************************************************************************/
#if (MMS_CONCLUDE_EN & REQ_EN)
#define MMS_CONCLUDE_RSP_DFUN		_mms_null_pdu_dec
#define MMS_CONCLUDE_CONF_SFUN 		u_mp_conclude_conf
#else
#define MMS_CONCLUDE_RSP_DFUN		mms_rsp_not_supp
#define MMS_CONCLUDE_CONF_SFUN 		u_conf_not_supp
#endif
#if (MMS_CONCLUDE_EN & RESP_EN)
#define MMS_CONCLUDE_REQ_DFUN		_mms_null_pdu_dec
#define MMS_CONCLUDE_IND_SFUN 		u_conclude_ind
#define CONCLUDE_SSI			0x10
#else
#define MMS_CONCLUDE_REQ_DFUN		mms_req_not_supp
#define MMS_CONCLUDE_IND_SFUN 		u_ind_not_supp
#define CONCLUDE_SSI			0x00
#endif

#if (MMS_CANCEL_EN & REQ_EN)
#define MMS_CANCEL_RSP_DFUN		mms_cancel_rsp
#define MMS_CANCEL_CONF_SFUN 		u_mp_cancel_conf
#else
#define MMS_CANCEL_RSP_DFUN		mms_rsp_not_supp
#define MMS_CANCEL_CONF_SFUN 		u_conf_not_supp
#endif
#if (MMS_CANCEL_EN & RESP_EN)
#define MMS_CANCEL_REQ_DFUN		mms_cancel_req
#define MMS_CANCEL_IND_SFUN 		u_cancel_ind
#define CANCEL_SSI			0x08
#else
#define MMS_CANCEL_REQ_DFUN		mms_req_not_supp
#define MMS_CANCEL_IND_SFUN 		u_ind_not_supp
#define CANCEL_SSI			0x00
#endif

#if (MMS_INIT_EN & REQ_EN)
#define MMS_INIT_RSP_DFUN		mms_init_rsp
#define MMS_INIT_CONF_SFUN 		u_conf_not_supp
#define MMS_MV_INIT_CONF_SFUN 		u_mv_init_conf
#else
#define MMS_INIT_RSP_DFUN		mms_rsp_not_supp
#define MMS_INIT_CONF_SFUN 		u_conf_not_supp
#define MMS_MV_INIT_CONF_SFUN 		u_conf_not_supp
#endif
#if (MMS_INIT_EN & RESP_EN)
#define MMS_INIT_REQ_DFUN		mms_init_req
#define MMS_INIT_IND_SFUN 		u_init_ind
#define INIT_SSI			0x10
#else
#define MMS_INIT_REQ_DFUN		mms_req_not_supp
#define MMS_INIT_IND_SFUN 		u_ind_not_supp
#define INIT_SSI			0x00
#endif


/************************************************************************/
/************************************************************************/
/* initialize the REQUEST DECODE table, opcode is index into table	*/
/************************************************************************/
ST_VOID (* SD_CONST mms_req_decode_fun [MAX_MMSOP_DIS+1]) (ASN1_DEC_CTXT *) =
  {
  MMS_STATUS_REQ_DFUN,			/*  00 STATUS			*/
  MMS_GETNAMES_REQ_DFUN, 		/*  01 GET_NAMLIST		*/
  MMS_IDENT_REQ_DFUN,			/*  02 IDENTIFY 		*/
  MMS_RENAME_REQ_DFUN,			/*  03 RENAME			*/
  MMS_READ_REQ_DFUN,			/*  04 READ			*/
  MMS_WRITE_REQ_DFUN,			/*  05 WRITE			*/
  MMS_GETVAR_REQ_DFUN,			/*  06 GET_VARDEF		*/
  MMS_DEFVAR_REQ_DFUN,			/*  07 DEF_VARNAM		*/
  MMS_DEFSCAT_REQ_DFUN,			/*  08 DEF_SCATTERED		*/
  MMS_GETSCAT_REQ_DFUN,			/*  09 GET_SCATTERED		*/
  MMS_DELVAR_REQ_DFUN,			/*  10 DEL_VARNAM		*/
  MMS_DEFVLIST_REQ_DFUN,		/*  11 DEF_VARLIST		*/
  MMS_GETVLIST_REQ_DFUN,		/*  12 GET_VARLIST		*/
  MMS_DELVLIST_REQ_DFUN,		/*  13 DEL_VARLIST		*/
  MMS_DEFTYPE_REQ_DFUN,			/*  14 DEF_TYPENAM		*/
  MMS_GETTYPE_REQ_DFUN,			/*  15 GET_TYPEDEF		*/
  MMS_DELTYPE_REQ_DFUN,			/*  16 DEL_TYPENAM		*/
  MMS_INPUT_REQ_DFUN,			/*  17 INPUT			*/
  MMS_OUTPUT_REQ_DFUN,			/*  18 OUTPUT			*/
  MMS_TAKECTRL_REQ_DFUN,		/*  19 TAKE_CONTROL		*/
  MMS_RELCTRL_REQ_DFUN,			/*  20 REL_CONTROL		*/
  MMS_DEFINE_SEM_REQ_DFUN,		/*  21 DEF_SEMAPHORE		*/
  MMS_DELETE_SEM_REQ_DFUN,		/*  22 DEL_SEMAPHORE		*/
  MMS_REP_SEMSTAT_REQ_DFUN,		/*  23 REP_SEM_STATUS		*/
  MMS_REP_SEMPOOL_REQ_DFUN,		/*  24 REP_SEM_POOL_STATUS	*/
  MMS_REP_SEMENTRY_REQ_DFUN,		/*  25 REP_SEM_ENTRY_STATUS	*/
  MMS_INIT_DWN_REQ_DFUN,		/*  26 INIT_DOWNLOAD		*/
  MMS_DWN_LOAD_REQ_DFUN,		/*  27 DOWN_LOAD		*/
  MMS_TERM_DWN_REQ_DFUN,		/*  28 TERM_DOWNLOAD		*/
  MMS_INIT_UPL_REQ_DFUN,		/*  29 INIT_UPLOAD		*/
  MMS_UP_LOAD_REQ_DFUN,			/*  30 UP_LOAD			*/
  MMS_TERM_UPL_REQ_DFUN,		/*  31 TERM_UPLOAD		*/
  MMS_RDDWN_REQ_DFUN,			/*  32 REQ_DOWNLOAD		*/
  MMS_RDUPL_REQ_DFUN,			/*  33 REQ_UPLOAD		*/
  MMS_LOAD_DOM_REQ_DFUN,		/*  34 LOAD_DOMAIN		*/
  MMS_STR_DOM_REQ_DFUN,			/*  35 STORE_DOMAIN		*/
  MMS_DEL_DOM_REQ_DFUN,			/*  36 DELETE_DOMAIN		*/
  MMS_GET_DOM_REQ_DFUN,			/*  37 GET_DOMAIN		*/
  MMS_CRE_PI_REQ_DFUN,			/*  38 CREATE_PROGRAM_INVOCATION*/
  MMS_DEL_PI_REQ_DFUN,			/*  39 DEL_PROGRAM_INVOCATION	*/
  MMS_START_REQ_DFUN,			/*  40 START			*/
  MMS_STOP_REQ_DFUN,			/*  41 STOP			*/
  MMS_RESUME_REQ_DFUN,			/*  42 RESUME			*/
  MMS_RESET_REQ_DFUN,			/*  43 RESET			*/
  MMS_KILL_REQ_DFUN,			/*  44 KILL			*/
  MMS_GET_PI_REQ_DFUN,			/*  45 GET_PROGRAM_INVOCATION	*/
  MMS_OBTAINFILE_REQ_DFUN,		/*  46 OBTAIN_FILE		*/
  MMS_DEFEC_REQ_DFUN,			/*  47 DEFINE EVENT CONDITION	*/
  MMS_DELEC_REQ_DFUN,			/*  48 DELETE EVENT CONDITION	*/
  MMS_GETECA_REQ_DFUN,			/*  49 GET EVENT CONDITION ATTR	*/
  MMS_REPECS_REQ_DFUN,			/*  50 REPORT EVENT COND STATUS	*/
  MMS_ALTECM_REQ_DFUN,			/*  51 ALTER EV COND MONITORING	*/
  MMS_TRIGE_REQ_DFUN,			/*  52 TRIGGER EVENT		*/
  MMS_DEFEA_REQ_DFUN,			/*  53 DEFINE EVENT ACTION	*/
  MMS_DELEA_REQ_DFUN,			/*  54 DELETE EVENT ACTION	*/
  MMS_GETEAA_REQ_DFUN,			/*  55 GET EV ACTION ATTRIBUTES	*/
  MMS_REPEAS_REQ_DFUN,			/*  56 REPORT EV ACTION STATUS	*/
  MMS_DEFEE_REQ_DFUN,			/*  57 DEFINE EVENT ENROLLMENT	*/
  MMS_DELEE_REQ_DFUN,			/*  58 DELETE EVENT ENROLLMENT	*/
  MMS_ALTEE_REQ_DFUN,			/*  59 ALTER EVENT ENROLLMENT	*/
  MMS_REPEES_REQ_DFUN,			/*  60 REPORT ENROLL ATTRIBUTES	*/
  MMS_GETEEA_REQ_DFUN,			/*  61 GET ENROLLMENT STATUS	*/
  MMS_ACKEVNOT_REQ_DFUN,		/*  62 ACK_EVENT_NOTIFICATION	*/
  MMS_GETAS_REQ_DFUN,			/*  63 GET_ALARM_SUMMARY	*/
  MMS_GETAES_REQ_DFUN,			/*  64 GET ALARM ENROLLMENT SUM	*/
  MMS_JREAD_REQ_DFUN,			/*  65 READ_JOURNAL		*/
  MMS_JWRITE_REQ_DFUN,			/*  66 WRITE_JOURNAL		*/
  MMS_JINIT_REQ_DFUN,			/*  67 INITIALIZE_JOURNAL	*/
  MMS_JSTAT_REQ_DFUN,			/*  68 REPORT_JOURNAL_STATUS	*/
  MMS_JCREATE_REQ_DFUN,			/*  69 CREATE JOURNAL		*/
  MMS_JDELETE_REQ_DFUN,			/*  70 DELETE JOURNAL		*/
  MMS_GETCL_REQ_DFUN,			/*  71 GET CAPABILITY LIST 	*/
  MMS_FOPEN_REQ_DFUN,			/*  72 FILE_OPEN		*/
  MMS_FREAD_REQ_DFUN,			/*  73 FILE_READ		*/
  MMS_FCLOSE_REQ_DFUN,			/*  74 FILE_CLOSE		*/
  MMS_FRENAME_REQ_DFUN,			/*  75 FILE_RENAME		*/
  MMS_FDELETE_REQ_DFUN,			/*  76 FILE_DELETE		*/
  MMS_FDIR_REQ_DFUN,			/*  77 FILE_DIR 		*/
  MMS_USTATUS_REQ_DFUN,			/*  78 UNSOLICITED_STATUS	*/
  MMS_INFO_REQ_DFUN,			/*  79 INFO_RPT 		*/
  MMS_EVNOT_REQ_DFUN,			/*  80 EVENT NOTIFICATION	*/
  mms_req_not_supp,	  		/*  81 ATTACH TO EVENT COND	*/
  mms_req_not_supp,	  		/*  82 ATTACH TO SEMAPHORE	*/
  MMS_CONCLUDE_REQ_DFUN,		/*  83 CONCLUDE 		*/
  MMS_CANCEL_REQ_DFUN,			/*  84 CANCEL			*/
  MMS_INIT_REQ_DFUN			/*  85 INITIATE 		*/
  };

/************************************************************************/
/* initialize the RESPONSE DECODE table, opcode is index into table	*/
/************************************************************************/
ST_VOID (* SD_CONST mms_rsp_decode_fun [MAX_MMSOP_DIS+1]) (ASN1_DEC_CTXT *) =
  {
  MMS_STATUS_RSP_DFUN,			/*  00 STATUS			*/
  MMS_GETNAMES_RSP_DFUN, 		/*  01 GET_NAMLIST		*/
  MMS_IDENT_RSP_DFUN,			/*  02 IDENTIFY 		*/
  MMS_RENAME_RSP_DFUN,			/*  03 RENAME			*/
  MMS_READ_RSP_DFUN,			/*  04 READ			*/
  MMS_WRITE_RSP_DFUN,			/*  05 WRITE			*/
  MMS_GETVAR_RSP_DFUN,			/*  06 GET_VARDEF		*/
  MMS_DEFVAR_RSP_DFUN,			/*  07 DEF_VARNAM		*/
  MMS_DEFSCAT_RSP_DFUN,			/*  08 DEF_SCATTERED		*/
  MMS_GETSCAT_RSP_DFUN,			/*  09 GET_SCATTERED		*/
  MMS_DELVAR_RSP_DFUN,			/*  10 DEL_VARNAM		*/
  MMS_DEFVLIST_RSP_DFUN,		/*  11 DEF_VARLIST		*/
  MMS_GETVLIST_RSP_DFUN,		/*  12 GET_VARLIST		*/
  MMS_DELVLIST_RSP_DFUN,		/*  13 DEL_VARLIST		*/
  MMS_DEFTYPE_RSP_DFUN,			/*  14 DEF_TYPENAM		*/
  MMS_GETTYPE_RSP_DFUN,			/*  15 GET_TYPEDEF		*/
  MMS_DELTYPE_RSP_DFUN,			/*  16 DEL_TYPENAM		*/
  MMS_INPUT_RSP_DFUN,			/*  17 INPUT			*/
  MMS_OUTPUT_RSP_DFUN,			/*  18 OUTPUT			*/
  MMS_TAKECTRL_RSP_DFUN,		/*  19 TAKE_CONTROL		*/
  MMS_RELCTRL_RSP_DFUN,			/*  20 REL_CONTROL		*/
  MMS_DEFINE_SEM_RSP_DFUN,		/*  21 DEF_SEMAPHORE		*/
  MMS_DELETE_SEM_RSP_DFUN,		/*  22 DEL_SEMAPHORE		*/
  MMS_REP_SEMSTAT_RSP_DFUN,		/*  23 REP_SEM_STATUS		*/
  MMS_REP_SEMPOOL_RSP_DFUN,		/*  24 REP_SEM_POOL_STATUS	*/
  MMS_REP_SEMENTRY_RSP_DFUN,		/*  25 REP_SEM_ENTRY_STATUS	*/
  MMS_INIT_DWN_RSP_DFUN,		/*  26 INIT_DOWNLOAD		*/
  MMS_DWN_LOAD_RSP_DFUN,		/*  27 DOWN_LOAD		*/
  MMS_TERM_DWN_RSP_DFUN,		/*  28 TERM_DOWNLOAD		*/
  MMS_INIT_UPL_RSP_DFUN,		/*  29 INIT_UPLOAD		*/
  MMS_UP_LOAD_RSP_DFUN,			/*  30 UP_LOAD			*/
  MMS_TERM_UPL_RSP_DFUN,		/*  31 TERM_UPLOAD		*/
  MMS_RDDWN_RSP_DFUN,			/*  32 REQ_DOWNLOAD		*/
  MMS_RDUPL_RSP_DFUN,			/*  33 REQ_UPLOAD		*/
  MMS_LOAD_DOM_RSP_DFUN,		/*  34 LOAD_DOMAIN		*/
  MMS_STR_DOM_RSP_DFUN,			/*  35 STORE_DOMAIN		*/
  MMS_DEL_DOM_RSP_DFUN,			/*  36 DELETE_DOMAIN		*/
  MMS_GET_DOM_RSP_DFUN,			/*  37 GET_DOMAIN		*/
  MMS_CRE_PI_RSP_DFUN,			/*  38 CREATE_PROGRAM_INVOCATION*/
  MMS_DEL_PI_RSP_DFUN,			/*  39 DEL_PROGRAM_INVOCATION	*/
  MMS_START_RSP_DFUN,			/*  40 START			*/
  MMS_STOP_RSP_DFUN,			/*  41 STOP			*/
  MMS_RESUME_RSP_DFUN,			/*  42 RESUME			*/
  MMS_RESET_RSP_DFUN,			/*  43 RESET			*/
  MMS_KILL_RSP_DFUN,			/*  44 KILL			*/
  MMS_GET_PI_RSP_DFUN,			/*  45 GET_PROGRAM_INVOCATION	*/
  MMS_OBTAINFILE_RSP_DFUN,		/*  46 OBTAIN_FILE		*/
  MMS_DEFEC_RSP_DFUN,			/*  47 DEFINE EVENT CONDITION	*/
  MMS_DELEC_RSP_DFUN,			/*  48 DELETE EVENT CONDITION	*/
  MMS_GETECA_RSP_DFUN,			/*  49 GET EVENT CONDITION ATTR	*/
  MMS_REPECS_RSP_DFUN,			/*  50 REPORT EVENT COND STATUS	*/
  MMS_ALTECM_RSP_DFUN,			/*  51 ALTER EV COND MONITORING	*/
  MMS_TRIGE_RSP_DFUN,			/*  52 TRIGGER EVENT		*/
  MMS_DEFEA_RSP_DFUN,			/*  53 DEFINE EVENT ACTION	*/
  MMS_DELEA_RSP_DFUN,			/*  54 DELETE EVENT ACTION	*/
  MMS_GETEAA_RSP_DFUN,			/*  55 GET EV ACTION ATTRIBUTES	*/
  MMS_REPEAS_RSP_DFUN,			/*  56 REPORT EV ACTION STATUS	*/
  MMS_DEFEE_RSP_DFUN,			/*  57 DEFINE EVENT ENROLLMENT	*/
  MMS_DELEE_RSP_DFUN,			/*  58 DELETE EVENT ENROLLMENT	*/
  MMS_ALTEE_RSP_DFUN,			/*  59 ALTER EVENT ENROLLMENT	*/
  MMS_REPEES_RSP_DFUN,			/*  60 REPORT ENROLL ATTRIBUTES	*/
  MMS_GETEEA_RSP_DFUN,			/*  61 GET ENROLLMENT STATUS	*/
  MMS_ACKEVNOT_RSP_DFUN,		/*  62 ACK_EVENT_NOTIFICATION	*/
  MMS_GETAS_RSP_DFUN,			/*  63 GET_ALARM_SUMMARY	*/
  MMS_GETAES_RSP_DFUN,			/*  64 GET ALARM ENROLLMENT SUM	*/
  MMS_JREAD_RSP_DFUN,			/*  65 READ_JOURNAL		*/
  MMS_JWRITE_RSP_DFUN,			/*  66 WRITE_JOURNAL		*/
  MMS_JINIT_RSP_DFUN,			/*  67 INITIALIZE_JOURNAL	*/
  MMS_JSTAT_RSP_DFUN,			/*  68 REPORT_JOURNAL_STATUS	*/
  MMS_JCREATE_RSP_DFUN,			/*  69 CREATE JOURNAL		*/
  MMS_JDELETE_RSP_DFUN,			/*  70 DELETE JOURNAL		*/
  MMS_GETCL_RSP_DFUN,			/*  71 GET CAPABILITY LIST 	*/
  MMS_FOPEN_RSP_DFUN,			/*  72 FILE_OPEN		*/
  MMS_FREAD_RSP_DFUN,			/*  73 FILE_READ		*/
  MMS_FCLOSE_RSP_DFUN,			/*  74 FILE_CLOSE		*/
  MMS_FRENAME_RSP_DFUN,			/*  75 FILE_RENAME		*/
  MMS_FDELETE_RSP_DFUN,			/*  76 FILE_DELETE		*/
  MMS_FDIR_RSP_DFUN,			/*  77 FILE_DIR 		*/


  mms_rsp_not_supp,			/*  78 UNSOLICITED_STATUS	*/
  mms_rsp_not_supp,			/*  79 INFO_RPT 		*/
  mms_rsp_not_supp,			/*  80 EVENT NOTIFICATION	*/
  mms_rsp_not_supp,	  		/*  81 ATTACH TO EVENT COND	*/
  mms_rsp_not_supp,	  		/*  82 ATTACH TO SEMAPHORE	*/
  MMS_CONCLUDE_RSP_DFUN,		/*  83 CONCLUDE 		*/
  MMS_CANCEL_RSP_DFUN,			/*  84 CANCEL			*/
  MMS_INIT_RSP_DFUN			/*  85 INITIATE 		*/
  };

#ifndef MMS_LITE
/************************************************************************/
/* initialize the INDICATION SERVICE table, opcode is index into table	*/
/************************************************************************/

ST_VOID (*mms_ind_serve_fun  [MAX_IND_SFUN+1]) (MMSREQ_IND *) =   
  {
  MMS_STATUS_IND_SFUN,			/*  00 STATUS			*/
  MMS_GETNAMES_IND_SFUN, 		/*  01 GET_NAMLIST		*/
  MMS_IDENT_IND_SFUN,			/*  02 IDENTIFY 		*/
  MMS_RENAME_IND_SFUN,			/*  03 RENAME			*/
  MMS_READ_IND_SFUN,			/*  04 READ			*/
  MMS_WRITE_IND_SFUN,			/*  05 WRITE			*/
  MMS_GETVAR_IND_SFUN,			/*  06 GET_VARDEF		*/
  MMS_DEFVAR_IND_SFUN,			/*  07 DEF_VARNAM		*/
  MMS_DEFSCAT_IND_SFUN,			/*  08 DEF_SCATTERED		*/
  MMS_GETSCAT_IND_SFUN,			/*  09 GET_SCATTERED		*/
  MMS_DELVAR_IND_SFUN,			/*  10 DEL_VARNAM		*/
  MMS_DEFVLIST_IND_SFUN,		/*  11 DEF_VARLIST		*/
  MMS_GETVLIST_IND_SFUN,		/*  12 GET_VARLIST		*/
  MMS_DELVLIST_IND_SFUN,		/*  13 DEL_VARLIST		*/
  MMS_DEFTYPE_IND_SFUN,			/*  14 DEF_TYPENAM		*/
  MMS_GETTYPE_IND_SFUN,			/*  15 GET_TYPEDEF		*/
  MMS_DELTYPE_IND_SFUN,			/*  16 DEL_TYPENAM		*/
  MMS_INPUT_IND_SFUN,			/*  17 INPUT			*/
  MMS_OUTPUT_IND_SFUN,			/*  18 OUTPUT			*/
  MMS_TAKECTRL_IND_SFUN,		/*  19 TAKE_CONTROL		*/
  MMS_RELCTRL_IND_SFUN,			/*  20 REL_CONTROL		*/
  MMS_DEFINE_SEM_IND_SFUN,		/*  21 DEF_SEMAPHORE		*/
  MMS_DELETE_SEM_IND_SFUN,		/*  22 DEL_SEMAPHORE		*/
  MMS_REP_SEMSTAT_IND_SFUN,		/*  23 REP_SEM_STATUS		*/
  MMS_REP_SEMPOOL_IND_SFUN,		/*  24 REP_SEM_POOL_STATUS	*/
  MMS_REP_SEMENTRY_IND_SFUN,		/*  25 REP_SEM_ENTRY_STATUS	*/
  MMS_INIT_DWN_IND_SFUN,		/*  26 INIT_DOWNLOAD		*/
  MMS_DWN_LOAD_IND_SFUN,		/*  27 DOWN_LOAD		*/
  MMS_TERM_DWN_IND_SFUN,		/*  28 TERM_DOWNLOAD		*/
  MMS_INIT_UPL_IND_SFUN,		/*  29 INIT_UPLOAD		*/
  MMS_UP_LOAD_IND_SFUN,			/*  30 UP_LOAD			*/
  MMS_TERM_UPL_IND_SFUN,		/*  31 TERM_UPLOAD		*/
  MMS_RDDWN_IND_SFUN,			/*  32 REQ_DOWNLOAD		*/
  MMS_RDUPL_IND_SFUN,			/*  33 REQ_UPLOAD		*/
  MMS_LOAD_DOM_IND_SFUN,		/*  34 LOAD_DOMAIN		*/
  MMS_STR_DOM_IND_SFUN,			/*  35 STORE_DOMAIN		*/
  MMS_DEL_DOM_IND_SFUN,			/*  36 DELETE_DOMAIN		*/
  MMS_GET_DOM_IND_SFUN,			/*  37 GET_DOMAIN		*/
  MMS_CRE_PI_IND_SFUN,			/*  38 CREATE_PROGRAM_INVOCATION*/
  MMS_DEL_PI_IND_SFUN,			/*  39 DEL_PROGRAM_INVOCATION	*/
  MMS_START_IND_SFUN,			/*  40 START			*/
  MMS_STOP_IND_SFUN,			/*  41 STOP			*/
  MMS_RESUME_IND_SFUN,			/*  42 RESUME			*/
  MMS_RESET_IND_SFUN,			/*  43 RESET			*/
  MMS_KILL_IND_SFUN,			/*  44 KILL			*/
  MMS_GET_PI_IND_SFUN,			/*  45 GET_PROGRAM_INVOCATION	*/
  MMS_OBTAINFILE_IND_SFUN,		/*  46 OBTAIN_FILE		*/
  MMS_DEFEC_IND_SFUN,			/*  47 DEFINE EVENT CONDITION	*/
  MMS_DELEC_IND_SFUN,			/*  48 DELETE EVENT CONDITION	*/
  MMS_GETECA_IND_SFUN,			/*  49 GET EVENT CONDITION ATTR	*/
  MMS_REPECS_IND_SFUN,			/*  50 REPORT EVENT COND STATUS	*/
  MMS_ALTECM_IND_SFUN,			/*  51 ALTER EV COND MONITORING	*/
  MMS_TRIGE_IND_SFUN,			/*  52 TRIGGER EVENT		*/
  MMS_DEFEA_IND_SFUN,			/*  53 DEFINE EVENT ACTION	*/
  MMS_DELEA_IND_SFUN,			/*  54 DELETE EVENT ACTION	*/
  MMS_GETEAA_IND_SFUN,			/*  55 GET EV ACTION ATTRIBUTES	*/
  MMS_REPEAS_IND_SFUN,			/*  56 REPORT EV ACTION STATUS	*/
  MMS_DEFEE_IND_SFUN,			/*  57 DEFINE EVENT ENROLLMENT	*/
  MMS_DELEE_IND_SFUN,			/*  58 DELETE EVENT ENROLLMENT	*/
  MMS_ALTEE_IND_SFUN,			/*  59 ALTER EVENT ENROLLMENT	*/
  MMS_REPEES_IND_SFUN,			/*  60 REPORT ENROLL ATTRIBUTES	*/
  MMS_GETEEA_IND_SFUN,			/*  61 GET ENROLLMENT STATUS	*/
  MMS_ACKEVNOT_IND_SFUN,		/*  62 ACK_EVENT_NOTIFICATION	*/
  MMS_GETAS_IND_SFUN,			/*  63 GET_ALARM_SUMMARY	*/
  MMS_GETAES_IND_SFUN,			/*  64 GET ALARM ENROLLMENT SUM	*/
  MMS_JREAD_IND_SFUN,			/*  65 READ_JOURNAL		*/
  MMS_JWRITE_IND_SFUN,			/*  66 WRITE_JOURNAL		*/
  MMS_JINIT_IND_SFUN,			/*  67 INITIALIZE_JOURNAL	*/
  MMS_JSTAT_IND_SFUN,			/*  68 REPORT_JOURNAL_STATUS	*/
  MMS_JCREATE_IND_SFUN,			/*  69 CREATE JOURNAL		*/
  MMS_JDELETE_IND_SFUN,			/*  70 DELETE JOURNAL		*/
  MMS_GETCL_IND_SFUN,			/*  71 GET CAPABILITY LIST 	*/
  MMS_FOPEN_IND_SFUN,			/*  72 FILE_OPEN		*/
  MMS_FREAD_IND_SFUN,			/*  73 FILE_READ		*/
  MMS_FCLOSE_IND_SFUN,			/*  74 FILE_CLOSE		*/
  MMS_FRENAME_IND_SFUN,			/*  75 FILE_RENAME		*/
  MMS_FDELETE_IND_SFUN,			/*  76 FILE_DELETE		*/
  MMS_FDIR_IND_SFUN,			/*  77 FILE_DIR 		*/
  MMS_USTATUS_IND_SFUN,			/*  78 UNSOLICITED_STATUS	*/
  MMS_INFO_IND_SFUN,			/*  79 INFO_RPT 		*/
  MMS_EVNOT_IND_SFUN,			/*  80 EVENT NOTIFICATION	*/
  u_ind_not_supp,	  		/*  81 ATTACH TO EVENT COND	*/
  u_ind_not_supp,	  		/*  82 ATTACH TO SEMAPHORE	*/
  MMS_CONCLUDE_IND_SFUN,		/*  83 CONCLUDE 		*/
  MMS_CANCEL_IND_SFUN,			/*  84 CANCEL			*/
  MMS_INIT_IND_SFUN			/*  85 INITIATE 		*/
  };

/************************************************************************/
/* initialize the CONFIRMATION SERVICE table, opcode is index into table*/
/************************************************************************/
ST_VOID (*mms_conf_serve_fun [MAX_CONF_SFUN+1]) (MMSREQ_PEND *) =
  {
  MMS_STATUS_CONF_SFUN,			/*  00 STATUS			*/
  MMS_GETNAMES_CONF_SFUN, 		/*  01 GET_NAMLIST		*/
  MMS_IDENT_CONF_SFUN,			/*  02 IDENTIFY 		*/
  MMS_RENAME_CONF_SFUN,			/*  03 RENAME			*/
  MMS_READ_CONF_SFUN,			/*  04 READ			*/
  MMS_WRITE_CONF_SFUN,			/*  05 WRITE			*/
  MMS_GETVAR_CONF_SFUN,			/*  06 GET_VARDEF		*/
  MMS_DEFVAR_CONF_SFUN,			/*  07 DEF_VARNAM		*/
  MMS_DEFSCAT_CONF_SFUN,		/*  08 DEF_SCATTERED		*/
  MMS_GETSCAT_CONF_SFUN,		/*  09 GET_SCATTERED		*/
  MMS_DELVAR_CONF_SFUN,			/*  10 DEL_VARNAM		*/
  MMS_DEFVLIST_CONF_SFUN,		/*  11 DEF_VARLIST		*/
  MMS_GETVLIST_CONF_SFUN,		/*  12 GET_VARLIST		*/
  MMS_DELVLIST_CONF_SFUN,		/*  13 DEL_VARLIST		*/
  MMS_DEFTYPE_CONF_SFUN,		/*  14 DEF_TYPENAM		*/
  MMS_GETTYPE_CONF_SFUN,		/*  15 GET_TYPEDEF		*/
  MMS_DELTYPE_CONF_SFUN,		/*  16 DEL_TYPENAM		*/
  MMS_INPUT_CONF_SFUN,			/*  17 INPUT			*/
  MMS_OUTPUT_CONF_SFUN,			/*  18 OUTPUT			*/
  MMS_TAKECTRL_CONF_SFUN,		/*  19 TAKE_CONTROL		*/
  MMS_RELCTRL_CONF_SFUN,		/*  20 REL_CONTROL		*/
  MMS_DEFINE_SEM_CONF_SFUN,		/*  21 DEF_SEMAPHORE		*/
  MMS_DELETE_SEM_CONF_SFUN,		/*  22 DEL_SEMAPHORE		*/
  MMS_REP_SEMSTAT_CONF_SFUN,		/*  23 REP_SEM_STATUS		*/
  MMS_REP_SEMPOOL_CONF_SFUN,		/*  24 REP_SEM_POOL_STATUS	*/
  MMS_REP_SEMENTRY_CONF_SFUN,		/*  25 REP_SEM_ENTRY_STATUS	*/
  MMS_INIT_DWN_CONF_SFUN,		/*  26 INIT_DOWNLOAD		*/
  MMS_DWN_LOAD_CONF_SFUN,		/*  27 DOWN_LOAD		*/
  MMS_TERM_DWN_CONF_SFUN,		/*  28 TERM_DOWNLOAD		*/
  MMS_INIT_UPL_CONF_SFUN,		/*  29 INIT_UPLOAD		*/
  MMS_UP_LOAD_CONF_SFUN,		/*  30 UP_LOAD			*/
  MMS_TERM_UPL_CONF_SFUN,		/*  31 TERM_UPLOAD		*/
  MMS_RDDWN_CONF_SFUN,			/*  32 REQ_DOWNLOAD		*/
  MMS_RDUPL_CONF_SFUN,			/*  33 REQ_UPLOAD		*/
  MMS_LOAD_DOM_CONF_SFUN,		/*  34 LOAD_DOMAIN		*/
  MMS_STR_DOM_CONF_SFUN,		/*  35 STORE_DOMAIN		*/
  MMS_DEL_DOM_CONF_SFUN,		/*  36 DELETE_DOMAIN		*/
  MMS_GET_DOM_CONF_SFUN,		/*  37 GET_DOMAIN		*/
  MMS_CRE_PI_CONF_SFUN,			/*  38 CREATE_PROGRAM_INVOCATION*/
  MMS_DEL_PI_CONF_SFUN,			/*  39 DEL_PROGRAM_INVOCATION	*/
  MMS_START_CONF_SFUN,			/*  40 START			*/
  MMS_STOP_CONF_SFUN,			/*  41 STOP			*/
  MMS_RESUME_CONF_SFUN,			/*  42 RESUME			*/
  MMS_RESET_CONF_SFUN,			/*  43 RESET			*/
  MMS_KILL_CONF_SFUN,			/*  44 KILL			*/
  MMS_GET_PI_CONF_SFUN,			/*  45 GET_PROGRAM_INVOCATION	*/
  MMS_OBTAINFILE_CONF_SFUN,		/*  46 OBTAIN_FILE		*/
  MMS_DEFEC_CONF_SFUN,			/*  47 DEFINE EVENT CONDITION	*/
  MMS_DELEC_CONF_SFUN,			/*  48 DELETE EVENT CONDITION	*/
  MMS_GETECA_CONF_SFUN,			/*  49 GET EVENT CONDITION ATTR	*/
  MMS_REPECS_CONF_SFUN,			/*  50 REPORT EVENT COND STATUS	*/
  MMS_ALTECM_CONF_SFUN,			/*  51 ALTER EV COND MONITORING	*/
  MMS_TRIGE_CONF_SFUN,			/*  52 TRIGGER EVENT		*/
  MMS_DEFEA_CONF_SFUN,			/*  53 DEFINE EVENT ACTION	*/
  MMS_DELEA_CONF_SFUN,			/*  54 DELETE EVENT ACTION	*/
  MMS_GETEAA_CONF_SFUN,			/*  55 GET EV ACTION ATTRIBUTES	*/
  MMS_REPEAS_CONF_SFUN,			/*  56 REPORT EV ACTION STATUS	*/
  MMS_DEFEE_CONF_SFUN,			/*  57 DEFINE EVENT ENROLLMENT	*/
  MMS_DELEE_CONF_SFUN,			/*  58 DELETE EVENT ENROLLMENT	*/
  MMS_ALTEE_CONF_SFUN,			/*  59 ALTER EVENT ENROLLMENT	*/
  MMS_REPEES_CONF_SFUN,			/*  60 REPORT ENROLL ATTRIBUTES	*/
  MMS_GETEEA_CONF_SFUN,			/*  61 GET ENROLLMENT STATUS	*/
  MMS_ACKEVNOT_CONF_SFUN,		/*  62 ACK_EVENT_NOTIFICATION	*/
  MMS_GETAS_CONF_SFUN,			/*  63 GET_ALARM_SUMMARY	*/
  MMS_GETAES_CONF_SFUN,			/*  64 GET ALARM ENROLLMENT SUM	*/
  MMS_JREAD_CONF_SFUN,			/*  65 READ_JOURNAL		*/
  MMS_JWRITE_CONF_SFUN,			/*  66 WRITE_JOURNAL		*/
  MMS_JINIT_CONF_SFUN,			/*  67 INITIALIZE_JOURNAL	*/
  MMS_JSTAT_CONF_SFUN,			/*  68 REPORT_JOURNAL_STATUS	*/
  MMS_JCREATE_CONF_SFUN,		/*  69 CREATE JOURNAL		*/
  MMS_JDELETE_CONF_SFUN,		/*  70 DELETE JOURNAL		*/
  MMS_GETCL_CONF_SFUN,			/*  71 GET CAPABILITY LIST 	*/
  MMS_FOPEN_CONF_SFUN,			/*  72 FILE_OPEN		*/
  MMS_FREAD_CONF_SFUN,			/*  73 FILE_READ		*/
  MMS_FCLOSE_CONF_SFUN,			/*  74 FILE_CLOSE		*/
  MMS_FRENAME_CONF_SFUN,		/*  75 FILE_RENAME		*/
  MMS_FDELETE_CONF_SFUN,		/*  76 FILE_DELETE		*/
  MMS_FDIR_CONF_SFUN,			/*  77 FILE_DIR 		*/
  u_conf_not_supp,			/*  78 UNSOLICITED_STATUS	*/
  u_conf_not_supp,			/*  79 INFO_RPT 		*/
  u_conf_not_supp,			/*  80 EVENT NOTIFICATION	*/
  u_conf_not_supp,	  		/*  81 ATTACH TO EVENT COND	*/
  u_conf_not_supp,	  		/*  82 ATTACH TO SEMAPHORE	*/
  MMS_CONCLUDE_CONF_SFUN,		/*  83 CONCLUDE 		*/
  u_conf_not_supp,			/*  84 CANCEL; handled elsewhere*/
  MMS_INIT_CONF_SFUN,			/*  85 INITIATE 		*/
  u_conf_not_supp,			/*  86 NOT A MMS OPERATION 	*/
  u_conf_not_supp,			/*  87 NOT A MMS OPERATION 	*/
  u_conf_not_supp,			/*  88 NOT A MMS OPERATION 	*/
  u_conf_not_supp,			/*  89 NOT A MMS OPERATION 	*/
  MMS_MV_READ_CONF_SFUN,		/*  90 NAMED READ	 	*/
  MMS_MV_WRITE_CONF_SFUN,		/*  91 NAMED WRITE	 	*/
  u_conf_not_supp,			/*  92 NOT A MMS OPERATION	*/
  MMS_MV_FOPEN_CONF_SFUN,		/*  93 REMOTE FILE OPEN 	*/
  MMS_MV_FREAD_CONF_SFUN,		/*  94 REMOTE FILE READ 	*/
  MMS_MV_FCLOSE_CONF_SFUN,		/*  95 REMOTE FILE CLOSE 	*/
  MMS_MV_INIT_CONF_SFUN,		/*  96 INITIATE 		*/
  MMS_MV_DEFTYPE_CONF_SFUN,		/*  97 TYPE DEFINITION	 	*/
  MMS_MV_FCOPY_CONF_SFUN,		/*  98 REMOTE FILE COPY 	*/
  MMS_MV_DOWNLOAD_CONF_SFUN,		/*  99 VM DOMAIN DOWNLOAD	*/
  MMS_MV_UPLOAD_CONF_SFUN, 		/* 100 VM DOMAIN UPLOAD		*/
  MMS_MV_RDVARS_CONF_SFUN,		/* 101 GENERAL READ	 	*/
  MMS_MV_WRVARS_CONF_SFUN,		/* 102 GENERAL WRITE	 	*/
  u_conf_not_supp,			/* 103 NOT A MMS OPERATION 	*/
  u_conf_not_supp,			/* 104 NOT A MMS OPERATION 	*/
  u_conf_not_supp			/* 105 NOT A MMS OPERATION 	*/
  };
#endif

/************************************************************************/
/************************************************************************/
/* MODIFIERS								*/
/************************************************************************/
#define MODAEC_SSI			0x00
#define MODAS_SSI			0x00


/************************************************************************/

SD_CONST ST_UCHAR  m_param[2] = { MPARAM0, MPARAM1 }; /* parameter support	*/


/************************************************************************/
/************************************************************************/
/* initialize the supported services variable for the INITIATE service.	*/
/************************************************************************/
#define NOTUSED_SSI	0x00


/*                 0x80      |    0x40      |     0x20     |    0x10      |
	           0x08      |    0x04      |     0x02     |    0x01	*/
#define SERV0	STATUS_SSI   | GETNAMES_SSI | IDENT_SSI    | RENAME_SSI   | \
	        READ_SSI     | WRITE_SSI    | GETVAR_SSI   | DEFVAR_SSI

#define SERV1	DEFSCAT_SSI  | GETSCAT_SSI  | DELVAR_SSI   | DEFVLIST_SSI | \
		GETVLIST_SSI | DELVLIST_SSI | DEFTYPE_SSI  | GETTYPE_SSI

#define SERV2	DELTYPE_SSI  | INPUT_SSI    | OUTPUT_SSI   | TAKECTRL_SSI | \
		RELCTRL_SSI  | DEFSEM_SSI   | DELSEM_SSI   | RSSTAT_SSI

#define SERV3	RSPOOL_SSI   | RSENTRY_SSI  | INITDWN_SSI  | DOWNLOAD_SSI | \
		TERMDOWN_SSI | INITUPL_SSI  | UPLOAD_SSI   | TERMUPL_SSI

#define SERV4	RDDWN_SSI    | RDUPL_SSI    | LOADDOM_SSI  | STRDOM_SSI   | \
		DELDOM_SSI   | GETDOM_SSI   | CREPI_SSI    | DELPI_SSI

#define SERV5	START_SSI    | STOP_SSI     | RESUME_SSI   | RESET_SSI    | \
		KILL_SSI     | GETPI_SSI    | OBTFILE_SSI  | DEFEC_SSI

#define SERV6	DELEC_SSI    | GETECA_SSI   | REPECS_SSI   | ALTECM_SSI   | \
		TRIGE_SSI    | DEFEA_SSI    | DELEA_SSI    | GETEAA_SSI

#define SERV7	REPEAS_SSI   | DEFEE_SSI    | DELEE_SSI    | ALTEE_SSI    | \
		REPEES_SSI   | GETEEA_SSI   | ACKEVNOT_SSI | GETAS_SSI

#define SERV8	GETAES_SSI   | JREAD_SSI    | JWRITE_SSI   | JINIT_SSI    | \
		JSTAT_SSI    | JCREATE_SSI  | JDELETE_SSI  | GETCL_SSI

#define SERV9	FOPEN_SSI    | FREAD_SSI    | FCLOSE_SSI   | FRENAME_SSI  | \
		FDELETE_SSI  | FDIR_SSI     | USTATUS_SSI  | INFO_SSI

#define SERV10	EVNOT_SSI    | MODAEC_SSI   | MODAS_SSI    | CONCLUDE_SSI | \
		CANCEL_SSI   | NOTUSED_SSI  | NOTUSED_SSI  | NOTUSED_SSI       

			        /* services supported as responder	*/
SD_CONST ST_UCHAR  m_service_resp[SERVICE_RESP_SIZE] =
                             {SERV0,SERV1,SERV2,SERV3,SERV4,SERV5,SERV6,
			     SERV7,SERV8,SERV9,SERV10};


/************************************************************************/
/************************************************************************/
/* These variables are used help control decode operation specific data */
/* structure allocation. For MMS-LITE (M_USR_DEC_BUF), these variables 	*/
/* are used to tell the decode system how many objects the allocated 	*/
/* buffer (_mms_dec_info). For MMS-EASE, these are normally not used;	*/
/* if they are set non-zero, they will be used in calculating the size	*/
/* of the dec info buffer to be allocated.				*/

#if (MMS_READ_EN & REQ_EN)
ST_INT m_cl_read_spec_in_result = SD_TRUE;
ST_INT m_cl_max_read_var_spec;
ST_INT m_cl_max_read_acc_rslt;
#endif

#if (MMS_INFO_EN & RESP_EN)
ST_INT m_cl_max_info_var_spec;
ST_INT m_cl_max_info_acc_rslt;
#endif

#if (MMS_GETVLIST_EN & REQ_EN)
ST_INT m_cl_max_getvla_vars;
#endif

#if (MMS_WRITE_EN & REQ_EN)
ST_INT m_cl_max_write_rslts;
#endif

#if (MMS_GETNAMES_EN & REQ_EN)
ST_INT m_cl_max_namel_names;
#endif

#if (MMS_GETCL_EN & REQ_EN)
ST_INT m_cl_max_getcl_cap;
#endif

#if (MMS_GET_PI_EN & REQ_EN)
ST_INT m_cl_max_getpi_doms;
#endif

#if (MMS_GET_DOM_EN & REQ_EN)
ST_INT m_cl_max_getdom_cap;
ST_INT m_cl_max_getdom_pi;
#endif

#if (MMS_INIT_UPL_EN & REQ_EN)
ST_INT m_cl_max_initupl_cap;
#endif

#if (MMS_GETAS_EN & REQ_EN)
ST_INT m_cl_max_alarm_summary;
#endif

#if (MMS_JREAD_EN & REQ_EN)
ST_INT m_cl_max_journal_entries;
ST_INT m_cl_max_entry_content;
#endif

#if (MMS_REP_SEMSTAT_EN & REQ_EN)
ST_INT m_cl_max_semaphore_entries;
#endif

#if (MMS_REP_SEMPOOL_EN & REQ_EN)
ST_INT m_cl_max_named_tokens;
#endif

#if (MMS_FDIR_EN & REQ_EN)
ST_INT m_cl_max_file_directory;
#endif

#if (MMS_GETEEA_EN & REQ_EN)
ST_INT m_cl_max_event_enrollment;
#endif

#if (MMS_GETAES_EN & REQ_EN)
ST_INT m_cl_max_enrollment_summary;
#endif

#if (MMS_LOAD_DOM_EN & RESP_EN)
ST_INT m_sv_max_loadd_cap;
#endif

ST_INT m_sv_max_read_var_spec;

ST_INT m_sv_max_write_var_spec;
ST_INT m_sv_max_write_data;

#if (MMS_RDDWN_EN & RESP_EN)
ST_INT m_sv_max_rqdlnl_cap;
#endif

#if (MMS_INIT_DWN_EN & RESP_EN)
ST_INT m_sv_max_initdnld_cap;
#endif

#if (MMS_CRE_PI_EN & RESP_EN)
ST_INT m_sv_max_vstr;
#endif

ST_INT m_sv_max_file_names;
ST_INT m_sv_max_obj_name;

#if (MMS_INPUT_EN & RESP_EN)
ST_INT m_sv_max_prompt_count;
#endif

#if (MMS_DEFVLIST_EN & RESP_EN)
ST_INT m_sv_max_num_vars;
#endif

#if (MMS_OUTPUT_EN & RESP_EN)
ST_INT m_sv_max_data_count;
#endif

#if (MMS_JWRITE_EN & RESP_EN)
ST_INT m_sv_max_entry_content;
#endif

#if (MMS_JWRITE_EN & RESP_EN)
ST_INT m_sv_max_data_vars;
#endif

#if (MMS_JREAD_EN & RESP_EN)
ST_INT m_sv_max_jread_vars;
#endif

/************************************************************************/
#ifndef MMS_LITE
/************************************************************************/
/* LLP indication function pointers					*/

ST_VOID (*u_abort_ind_fun)(ST_INT chan, ST_INT reason, ST_BOOLEAN au_flag) = 
	u_abort_ind;

ST_VOID (*u_llp_error_ind_fun)(ST_INT chan, ST_LONG code) = 
	u_llp_error_ind;

ST_VOID (*u_release_ind_fun)(ST_INT chan) = 
	u_release_ind;

ST_VOID (*u_mmsexcept_ind_fun)(ST_INT code, ST_INT chan) = 
	u_mmsexcept_ind;

ST_VOID (*u_cancel_ind_fun)(MMSREQ_IND *req_info) = 
	MMS_CANCEL_IND_SFUN;

ST_VOID (*u_reject_ind_fun)(ST_INT chan, struct  reject_resp_info *rej_ptr) = 
	u_reject_ind;

/************************************************************************/
/* LLP confirmation function pointers					*/

ST_VOID (*u_mp_conclude_done_fun)(ST_INT chan, ST_RET ret_code) = 
	u_mp_conclude_done;

ST_VOID (*u_mp_cancel_conf_fun)(MMSREQ_PEND *req_ptr, ST_BOOLEAN errdata_pres, 
			     struct err_info *err_ptr) = 
	u_mp_cancel_conf;

ST_VOID (*u_mp_abort_done_fun)(ST_INT chan) = 
	u_mp_abort_done;

ST_VOID (*u_init_resp_done_fun)(ST_INT chan) = 
	u_init_resp_done;

/************************************************************************/
/* LLP association function pointers					*/

ST_RET (*u_mllp_a_assoc_ind_fun)(ST_INT chan, ACSE_ASSINFO *assinfo) = 
	u_mllp_a_assoc_ind;

ST_RET (*u_mllp_a_assoc_conf_fun)(ST_INT chan, ACSE_ASSINFO *assinfo) = 
	u_mllp_a_assoc_conf;

/************************************************************************/
#endif	/* #ifndef MMS_LITE */
/************************************************************************/
/************************************************************************/
/************************************************************************/
/* MLOG LOGGING FUNCTION POINTER INITIALIZATION				*/
/************************************************************************/

#ifdef MLOG_ENABLE 

/************************************************************************/
/* VMD SUPPORT - CONFIRMED SERVICES   					*/
/************************************************************************/

#if (MMS_STATUS_EN & (REQ_EN | RESP_EN))
#define MMS_STATUS_RSP_LFUN		m_log_status_resp
#define MMS_STATUS_REQ_LFUN 		m_log_status_req
#else
#define MMS_STATUS_RSP_LFUN		m_no_log_resp
#define MMS_STATUS_REQ_LFUN 		m_no_log_req
#endif

#if (MMS_GETNAMES_EN & (REQ_EN | RESP_EN))
#define MMS_GETNAMES_RSP_LFUN		m_log_namelist_resp
#define MMS_GETNAMES_REQ_LFUN 		m_log_namelist_req
#else
#define MMS_GETNAMES_RSP_LFUN		m_no_log_resp
#define MMS_GETNAMES_REQ_LFUN 		m_no_log_req
#endif

#if (MMS_IDENT_EN & (REQ_EN | RESP_EN))
#define MMS_IDENT_RSP_LFUN		m_log_ident_resp
#define MMS_IDENT_REQ_LFUN 		m_no_log_req
#else
#define MMS_IDENT_RSP_LFUN		m_no_log_resp
#define MMS_IDENT_REQ_LFUN 		m_no_log_req
#endif

#if (MMS_RENAME_EN & (REQ_EN | RESP_EN))
#define MMS_RENAME_RSP_LFUN		m_no_log_resp
#define MMS_RENAME_REQ_LFUN 		m_log_rename_req
#else
#define MMS_RENAME_RSP_LFUN		m_no_log_resp
#define MMS_RENAME_REQ_LFUN 		m_no_log_req
#endif

#if (MMS_GETCL_EN & (REQ_EN | RESP_EN))
#define MMS_GETCL_RSP_LFUN		m_log_getcl_resp
#define MMS_GETCL_REQ_LFUN 		m_log_getcl_req
#else
#define MMS_GETCL_RSP_LFUN		m_no_log_resp
#define MMS_GETCL_REQ_LFUN 		m_no_log_req
#endif

/************************************************************************/
/* VARIABLE ACCESS - CONFIRMED SERVICES					*/
/************************************************************************/

#if (MMS_READ_EN & (REQ_EN | RESP_EN))
#define MMS_READ_RSP_LFUN		m_log_read_resp
#define MMS_READ_REQ_LFUN 		m_log_read_req
#else
#define MMS_READ_RSP_LFUN		m_no_log_resp
#define MMS_READ_REQ_LFUN 		m_no_log_req
#endif

#if (MMS_WRITE_EN & (REQ_EN | RESP_EN))
#define MMS_WRITE_RSP_LFUN		m_log_write_resp
#define MMS_WRITE_REQ_LFUN 		m_log_write_req
#else
#define MMS_WRITE_RSP_LFUN		m_no_log_resp
#define MMS_WRITE_REQ_LFUN 		m_no_log_req
#endif

#if (MMS_GETVAR_EN & (REQ_EN | RESP_EN))
#define MMS_GETVAR_RSP_LFUN		m_log_getvar_resp
#define MMS_GETVAR_REQ_LFUN 		m_log_getvar_req
#else
#define MMS_GETVAR_RSP_LFUN		m_no_log_resp
#define MMS_GETVAR_REQ_LFUN 		m_no_log_req
#endif

#if (MMS_DEFVAR_EN & (REQ_EN | RESP_EN))
#define MMS_DEFVAR_RSP_LFUN		m_no_log_resp
#define MMS_DEFVAR_REQ_LFUN 		m_log_defvar_req
#else
#define MMS_DEFVAR_RSP_LFUN		m_no_log_resp
#define MMS_DEFVAR_REQ_LFUN 		m_no_log_req
#endif

#if (MMS_DEFSCAT_EN & (REQ_EN | RESP_EN))
#define MMS_DEFSCAT_RSP_LFUN		m_no_log_resp
#define MMS_DEFSCAT_REQ_LFUN 		m_log_defscat_req
#else
#define MMS_DEFSCAT_RSP_LFUN		m_no_log_resp
#define MMS_DEFSCAT_REQ_LFUN 		m_no_log_req
#endif

#if (MMS_GETSCAT_EN & (REQ_EN | RESP_EN))
#define MMS_GETSCAT_RSP_LFUN		m_log_getscat_resp
#define MMS_GETSCAT_REQ_LFUN 		m_log_getscat_req
#else
#define MMS_GETSCAT_RSP_LFUN		m_no_log_resp
#define MMS_GETSCAT_REQ_LFUN 		m_no_log_req
#endif

#if (MMS_DELVAR_EN & (REQ_EN | RESP_EN))
#define MMS_DELVAR_RSP_LFUN		m_log_delvar_resp
#define MMS_DELVAR_REQ_LFUN 		m_log_delvar_req
#else
#define MMS_DELVAR_RSP_LFUN		m_no_log_resp
#define MMS_DELVAR_REQ_LFUN 		m_no_log_req
#endif

#if (MMS_DEFVLIST_EN & (REQ_EN | RESP_EN))
#define MMS_DEFVLIST_RSP_LFUN		m_no_log_resp
#define MMS_DEFVLIST_REQ_LFUN 		m_log_defvlist_req
#else
#define MMS_DEFVLIST_RSP_LFUN		m_no_log_resp
#define MMS_DEFVLIST_REQ_LFUN 		m_no_log_req
#endif

#if (MMS_GETVLIST_EN & (REQ_EN | RESP_EN))
#define MMS_GETVLIST_RSP_LFUN		m_log_getvlist_resp
#define MMS_GETVLIST_REQ_LFUN 		m_log_getvlist_req
#else
#define MMS_GETVLIST_RSP_LFUN		m_no_log_resp
#define MMS_GETVLIST_REQ_LFUN 		m_no_log_req
#endif

#if (MMS_DELVLIST_EN & (REQ_EN | RESP_EN))
#define MMS_DELVLIST_RSP_LFUN		m_log_delvlist_resp
#define MMS_DELVLIST_REQ_LFUN 		m_log_delvlist_req
#else
#define MMS_DELVLIST_RSP_LFUN		m_no_log_resp
#define MMS_DELVLIST_REQ_LFUN 		m_no_log_req
#endif


#if (MMS_DEFTYPE_EN & (REQ_EN | RESP_EN))
#define MMS_DEFTYPE_RSP_LFUN		m_no_log_resp
#define MMS_DEFTYPE_REQ_LFUN 		m_log_deftype_req
#else
#define MMS_DEFTYPE_RSP_LFUN		m_no_log_resp
#define MMS_DEFTYPE_REQ_LFUN 		m_no_log_req
#endif

#if (MMS_GETTYPE_EN & (REQ_EN | RESP_EN))
#define MMS_GETTYPE_RSP_LFUN		m_log_gettype_resp
#define MMS_GETTYPE_REQ_LFUN 		m_log_gettype_req
#else
#define MMS_GETTYPE_RSP_LFUN		m_no_log_resp
#define MMS_GETTYPE_REQ_LFUN 		m_no_log_req
#endif

#if (MMS_DELTYPE_EN & (REQ_EN | RESP_EN))
#define MMS_DELTYPE_RSP_LFUN		m_log_deltype_resp
#define MMS_DELTYPE_REQ_LFUN 		m_log_deltype_req
#else
#define MMS_DELTYPE_RSP_LFUN		m_no_log_resp
#define MMS_DELTYPE_REQ_LFUN 		m_no_log_req
#endif

/************************************************************************/
/* OPERATOR COMMUNICATION SERVICES   					*/
/************************************************************************/

#if (MMS_INPUT_EN & (REQ_EN | RESP_EN))
#define MMS_INPUT_RSP_LFUN		m_log_input_resp
#define MMS_INPUT_REQ_LFUN 		m_log_input_req
#else
#define MMS_INPUT_RSP_LFUN		m_no_log_resp
#define MMS_INPUT_REQ_LFUN 		m_no_log_req
#endif

#if (MMS_OUTPUT_EN & (REQ_EN | RESP_EN))
#define MMS_OUTPUT_RSP_LFUN		m_no_log_resp
#define MMS_OUTPUT_REQ_LFUN 		m_log_output_req
#else
#define MMS_OUTPUT_RSP_LFUN		m_no_log_resp
#define MMS_OUTPUT_REQ_LFUN 		m_no_log_req
#endif

/************************************************************************/
/* SEMAPHORE MANAGEMENT SERVICES 					*/
/************************************************************************/

#if (MMS_TAKECTRL_EN & (REQ_EN | RESP_EN))
#define MMS_TAKECTRL_RSP_LFUN		m_log_takectrl_resp
#define MMS_TAKECTRL_REQ_LFUN 		m_log_takectrl_req
#else
#define MMS_TAKECTRL_RSP_LFUN		m_no_log_resp
#define MMS_TAKECTRL_REQ_LFUN 		m_no_log_req
#endif

#if (MMS_RELCTRL_EN & (REQ_EN | RESP_EN))
#define MMS_RELCTRL_RSP_LFUN		m_no_log_resp
#define MMS_RELCTRL_REQ_LFUN 		m_log_relctrl_req
#else
#define MMS_RELCTRL_RSP_LFUN		m_no_log_resp
#define MMS_RELCTRL_REQ_LFUN 		m_no_log_req
#endif

#if (MMS_DEFINE_SEM_EN & (REQ_EN | RESP_EN))
#define MMS_DEFINE_SEM_RSP_LFUN		m_no_log_resp
#define MMS_DEFINE_SEM_REQ_LFUN 	m_log_defsem_req
#else
#define MMS_DEFINE_SEM_RSP_LFUN		m_no_log_resp
#define MMS_DEFINE_SEM_REQ_LFUN 	m_no_log_req
#endif


#if (MMS_DELETE_SEM_EN & (REQ_EN | RESP_EN))
#define MMS_DELETE_SEM_RSP_LFUN		m_no_log_resp
#define MMS_DELETE_SEM_REQ_LFUN 	m_log_delsem_req
#else
#define MMS_DELETE_SEM_RSP_LFUN		m_no_log_resp
#define MMS_DELETE_SEM_REQ_LFUN 	m_no_log_req
#endif


#if (MMS_REP_SEMSTAT_EN & (REQ_EN | RESP_EN))
#define MMS_REP_SEMSTAT_RSP_LFUN	m_log_rsstat_resp
#define MMS_REP_SEMSTAT_REQ_LFUN 	m_log_rsstat_req
#else
#define MMS_REP_SEMSTAT_RSP_LFUN	m_no_log_resp
#define MMS_REP_SEMSTAT_REQ_LFUN 	m_no_log_req
#endif

#if (MMS_REP_SEMPOOL_EN & (REQ_EN | RESP_EN))
#define MMS_REP_SEMPOOL_RSP_LFUN	m_log_rspool_resp
#define MMS_REP_SEMPOOL_REQ_LFUN 	m_log_rspool_req
#else
#define MMS_REP_SEMPOOL_RSP_LFUN	m_no_log_resp
#define MMS_REP_SEMPOOL_REQ_LFUN 	m_no_log_req
#endif


#if (MMS_REP_SEMENTRY_EN & (REQ_EN | RESP_EN))
#define MMS_REP_SEMENTRY_RSP_LFUN	m_log_rsentry_resp
#define MMS_REP_SEMENTRY_REQ_LFUN 	m_log_rsentry_req
#else
#define MMS_REP_SEMENTRY_RSP_LFUN	m_no_log_resp
#define MMS_REP_SEMENTRY_REQ_LFUN 	m_no_log_req
#endif


/************************************************************************/
/* DOMAIN MANAGEMENT SERVICES 						*/
/************************************************************************/

#if (MMS_INIT_DWN_EN & (REQ_EN | RESP_EN))
#define MMS_INIT_DWN_RSP_LFUN		m_no_log_resp
#define MMS_INIT_DWN_REQ_LFUN 		m_log_initdown_req
#else
#define MMS_INIT_DWN_RSP_LFUN		m_no_log_resp
#define MMS_INIT_DWN_REQ_LFUN 		m_no_log_req
#endif


#if (MMS_DWN_LOAD_EN & (REQ_EN | RESP_EN))
#define MMS_DWN_LOAD_RSP_LFUN		m_log_download_resp
#define MMS_DWN_LOAD_REQ_LFUN 		m_log_download_req
#else
#define MMS_DWN_LOAD_RSP_LFUN		m_no_log_resp
#define MMS_DWN_LOAD_REQ_LFUN 		m_no_log_req
#endif


#if (MMS_TERM_DWN_EN & (REQ_EN | RESP_EN))
#define MMS_TERM_DWN_RSP_LFUN		m_no_log_resp
#define MMS_TERM_DWN_REQ_LFUN 		m_log_termdown_req
#else
#define MMS_TERM_DWN_RSP_LFUN		m_no_log_resp
#define MMS_TERM_DWN_REQ_LFUN 		m_no_log_req
#endif


#if (MMS_INIT_UPL_EN & (REQ_EN | RESP_EN))
#define MMS_INIT_UPL_RSP_LFUN		m_log_initupl_resp
#define MMS_INIT_UPL_REQ_LFUN 		m_log_initupl_req
#else
#define MMS_INIT_UPL_RSP_LFUN		m_no_log_resp
#define MMS_INIT_UPL_REQ_LFUN 		m_no_log_req
#endif


#if (MMS_UP_LOAD_EN & (REQ_EN | RESP_EN))
#define MMS_UP_LOAD_RSP_LFUN		m_log_upload_resp
#define MMS_UP_LOAD_REQ_LFUN 		m_log_upload_req
#else
#define MMS_UP_LOAD_RSP_LFUN		m_no_log_resp
#define MMS_UP_LOAD_REQ_LFUN 		m_no_log_req
#endif


#if (MMS_TERM_UPL_EN & (REQ_EN | RESP_EN))
#define MMS_TERM_UPL_RSP_LFUN		m_no_log_resp
#define MMS_TERM_UPL_REQ_LFUN 		m_log_termupl_req
#else
#define MMS_TERM_UPL_RSP_LFUN		m_no_log_resp
#define MMS_TERM_UPL_REQ_LFUN 		m_no_log_req
#endif


#if (MMS_RDDWN_EN & (REQ_EN | RESP_EN))
#define MMS_RDDWN_RSP_LFUN		m_no_log_resp
#define MMS_RDDWN_REQ_LFUN 		m_log_rddwn_req
#else
#define MMS_RDDWN_RSP_LFUN		m_no_log_resp
#define MMS_RDDWN_REQ_LFUN 		m_no_log_req
#endif


#if (MMS_RDUPL_EN & (REQ_EN | RESP_EN))
#define MMS_RDUPL_RSP_LFUN		m_no_log_resp
#define MMS_RDUPL_REQ_LFUN 		m_log_rdupl_req
#else
#define MMS_RDUPL_RSP_LFUN		m_no_log_resp
#define MMS_RDUPL_REQ_LFUN 		m_no_log_req
#endif


#if (MMS_LOAD_DOM_EN & (REQ_EN | RESP_EN))
#define MMS_LOAD_DOM_RSP_LFUN		m_no_log_resp
#define MMS_LOAD_DOM_REQ_LFUN 		m_log_loaddom_req
#else
#define MMS_LOAD_DOM_RSP_LFUN		m_no_log_resp
#define MMS_LOAD_DOM_REQ_LFUN 		m_no_log_req
#endif


#if (MMS_STR_DOM_EN & (REQ_EN | RESP_EN))
#define MMS_STR_DOM_RSP_LFUN		m_no_log_resp
#define MMS_STR_DOM_REQ_LFUN 		m_log_storedom_req
#else
#define MMS_STR_DOM_RSP_LFUN		m_no_log_resp
#define MMS_STR_DOM_REQ_LFUN 		m_no_log_req
#endif


#if (MMS_DEL_DOM_EN & (REQ_EN | RESP_EN))
#define MMS_DEL_DOM_RSP_LFUN		m_no_log_resp
#define MMS_DEL_DOM_REQ_LFUN 		m_log_deldom_req
#else
#define MMS_DEL_DOM_RSP_LFUN		m_no_log_resp
#define MMS_DEL_DOM_REQ_LFUN 		m_no_log_req
#endif

#if (MMS_GET_DOM_EN & (REQ_EN | RESP_EN))
#define MMS_GET_DOM_RSP_LFUN		m_log_getdom_resp
#define MMS_GET_DOM_REQ_LFUN 		m_log_getdom_req
#else
#define MMS_GET_DOM_RSP_LFUN		m_no_log_resp
#define MMS_GET_DOM_REQ_LFUN 		m_no_log_req
#endif

/************************************************************************/
/* PROGRAM INVOCATION MANAGEMENT SERVICES 				*/
/************************************************************************/

#if (MMS_CRE_PI_EN & (REQ_EN | RESP_EN))
#define MMS_CRE_PI_RSP_LFUN		m_no_log_resp
#define MMS_CRE_PI_REQ_LFUN 		m_log_crepi_req
#else
#define MMS_CRE_PI_RSP_LFUN		m_no_log_resp
#define MMS_CRE_PI_REQ_LFUN 		m_no_log_req
#endif

#if (MMS_DEL_PI_EN & (REQ_EN | RESP_EN))
#define MMS_DEL_PI_RSP_LFUN		m_no_log_resp
#define MMS_DEL_PI_REQ_LFUN 		m_log_delpi_req
#else
#define MMS_DEL_PI_RSP_LFUN		m_no_log_resp
#define MMS_DEL_PI_REQ_LFUN 		m_no_log_req
#endif

#if (MMS_START_EN & (REQ_EN | RESP_EN))
#define MMS_START_RSP_LFUN		m_no_log_resp
#define MMS_START_REQ_LFUN 		m_log_start_req
#else
#define MMS_START_RSP_LFUN		m_no_log_resp
#define MMS_START_REQ_LFUN 		m_no_log_req
#endif

#if (MMS_STOP_EN & (REQ_EN | RESP_EN))
#define MMS_STOP_RSP_LFUN		m_no_log_resp
#define MMS_STOP_REQ_LFUN 		m_log_stop_req
#else
#define MMS_STOP_RSP_LFUN		m_no_log_resp
#define MMS_STOP_REQ_LFUN 		m_no_log_req
#endif

#if (MMS_RESUME_EN & (REQ_EN | RESP_EN))
#define MMS_RESUME_RSP_LFUN		m_no_log_resp
#define MMS_RESUME_REQ_LFUN 		m_log_resume_req
#else
#define MMS_RESUME_RSP_LFUN		m_no_log_resp
#define MMS_RESUME_REQ_LFUN 		m_no_log_req
#endif

#if (MMS_RESET_EN & (REQ_EN | RESP_EN))
#define MMS_RESET_RSP_LFUN		m_no_log_resp
#define MMS_RESET_REQ_LFUN 		m_log_reset_req
#else
#define MMS_RESET_RSP_LFUN		m_no_log_resp
#define MMS_RESET_REQ_LFUN 		m_no_log_req
#endif

#if (MMS_KILL_EN & (REQ_EN | RESP_EN))
#define MMS_KILL_RSP_LFUN		m_no_log_resp
#define MMS_KILL_REQ_LFUN 		m_log_kill_req
#else
#define MMS_KILL_RSP_LFUN		m_no_log_resp
#define MMS_KILL_REQ_LFUN 		m_no_log_req
#endif

#if (MMS_GET_PI_EN & (REQ_EN | RESP_EN))
#define MMS_GET_PI_RSP_LFUN		m_log_getpi_resp
#define MMS_GET_PI_REQ_LFUN 		m_log_getpi_req
#else
#define MMS_GET_PI_RSP_LFUN		m_no_log_resp
#define MMS_GET_PI_REQ_LFUN 		m_no_log_req
#endif

/************************************************************************/
/* EVENT MANAGEMENT - CONFIRMED SERVICES 				*/
/************************************************************************/

#if (MMS_DEFEC_EN & (REQ_EN | RESP_EN))
#define MMS_DEFEC_RSP_LFUN		m_no_log_resp
#define MMS_DEFEC_REQ_LFUN 		m_log_defec_req
#else
#define MMS_DEFEC_RSP_LFUN		m_no_log_resp
#define MMS_DEFEC_REQ_LFUN 		m_no_log_req
#endif

#if (MMS_DELEC_EN & (REQ_EN | RESP_EN))
#define MMS_DELEC_RSP_LFUN		m_log_delec_resp
#define MMS_DELEC_REQ_LFUN 		m_log_delec_req
#else
#define MMS_DELEC_RSP_LFUN		m_no_log_resp
#define MMS_DELEC_REQ_LFUN 		m_no_log_req
#endif

#if (MMS_GETECA_EN & (REQ_EN | RESP_EN))
#define MMS_GETECA_RSP_LFUN		m_log_geteca_resp
#define MMS_GETECA_REQ_LFUN 		m_log_geteca_req
#else
#define MMS_GETECA_RSP_LFUN		m_no_log_resp
#define MMS_GETECA_REQ_LFUN 		m_no_log_req
#endif

#if (MMS_REPECS_EN & (REQ_EN | RESP_EN))
#define MMS_REPECS_RSP_LFUN		m_log_repecs_resp
#define MMS_REPECS_REQ_LFUN 		m_log_repecs_req
#else
#define MMS_REPECS_RSP_LFUN		m_no_log_resp
#define MMS_REPECS_REQ_LFUN 		m_no_log_req
#endif

#if (MMS_ALTECM_EN & (REQ_EN | RESP_EN))
#define MMS_ALTECM_RSP_LFUN		m_no_log_resp
#define MMS_ALTECM_REQ_LFUN 		m_log_altecm_req
#else
#define MMS_ALTECM_RSP_LFUN		m_no_log_resp
#define MMS_ALTECM_REQ_LFUN 		m_no_log_req
#endif

#if (MMS_TRIGE_EN & (REQ_EN | RESP_EN))
#define MMS_TRIGE_RSP_LFUN		m_no_log_resp
#define MMS_TRIGE_REQ_LFUN 		m_log_trige_req
#else
#define MMS_TRIGE_RSP_LFUN		m_no_log_resp
#define MMS_TRIGE_REQ_LFUN 		m_no_log_req
#endif

#if (MMS_DEFEA_EN & (REQ_EN | RESP_EN))
#define MMS_DEFEA_RSP_LFUN		m_no_log_resp
#define MMS_DEFEA_REQ_LFUN 		m_log_defea_req
#else
#define MMS_DEFEA_RSP_LFUN		m_no_log_resp
#define MMS_DEFEA_REQ_LFUN 		m_no_log_req
#endif

#if (MMS_DELEA_EN & (REQ_EN | RESP_EN))
#define MMS_DELEA_RSP_LFUN		m_log_delea_resp
#define MMS_DELEA_REQ_LFUN 		m_log_delea_req
#else
#define MMS_DELEA_RSP_LFUN		m_no_log_resp
#define MMS_DELEA_REQ_LFUN 		m_no_log_req
#endif

#if (MMS_GETEAA_EN & (REQ_EN | RESP_EN))
#define MMS_GETEAA_RSP_LFUN		m_log_geteaa_resp
#define MMS_GETEAA_REQ_LFUN 		m_log_geteaa_req
#else
#define MMS_GETEAA_RSP_LFUN		m_no_log_resp
#define MMS_GETEAA_REQ_LFUN 		m_no_log_req
#endif

#if (MMS_REPEAS_EN & (REQ_EN | RESP_EN))
#define MMS_REPEAS_RSP_LFUN		m_log_repeas_resp
#define MMS_REPEAS_REQ_LFUN 		m_log_repeas_req
#else
#define MMS_REPEAS_RSP_LFUN		m_no_log_resp
#define MMS_REPEAS_REQ_LFUN 		m_no_log_req
#endif

#if (MMS_DEFEE_EN & (REQ_EN | RESP_EN))
#define MMS_DEFEE_RSP_LFUN		m_no_log_resp
#define MMS_DEFEE_REQ_LFUN 		m_log_defee_req
#else
#define MMS_DEFEE_RSP_LFUN		m_no_log_resp
#define MMS_DEFEE_REQ_LFUN 		m_no_log_req
#endif

#if (MMS_DELEE_EN & (REQ_EN | RESP_EN))
#define MMS_DELEE_RSP_LFUN		m_log_delee_resp
#define MMS_DELEE_REQ_LFUN 		m_log_delee_req
#else
#define MMS_DELEE_RSP_LFUN		m_no_log_resp
#define MMS_DELEE_REQ_LFUN 		m_no_log_req
#endif

#if (MMS_ALTEE_EN & (REQ_EN | RESP_EN))
#define MMS_ALTEE_RSP_LFUN		m_log_altee_resp
#define MMS_ALTEE_REQ_LFUN 		m_log_altee_req
#else
#define MMS_ALTEE_RSP_LFUN		m_no_log_resp
#define MMS_ALTEE_REQ_LFUN 		m_no_log_req
#endif

#if (MMS_REPEES_EN & (REQ_EN | RESP_EN))
#define MMS_REPEES_RSP_LFUN		m_log_repees_resp
#define MMS_REPEES_REQ_LFUN 		m_log_repees_req
#else
#define MMS_REPEES_RSP_LFUN		m_no_log_resp
#define MMS_REPEES_REQ_LFUN 		m_no_log_req
#endif

#if (MMS_GETEEA_EN & (REQ_EN | RESP_EN))
#define MMS_GETEEA_RSP_LFUN		m_log_geteea_resp
#define MMS_GETEEA_REQ_LFUN 		m_log_geteea_req
#else
#define MMS_GETEEA_RSP_LFUN		m_no_log_resp
#define MMS_GETEEA_REQ_LFUN 		m_no_log_req
#endif

#if (MMS_ACKEVNOT_EN & (REQ_EN | RESP_EN))
#define MMS_ACKEVNOT_RSP_LFUN		m_no_log_resp
#define MMS_ACKEVNOT_REQ_LFUN 		m_log_ackevnot_req
#else
#define MMS_ACKEVNOT_RSP_LFUN		m_no_log_resp
#define MMS_ACKEVNOT_REQ_LFUN 		m_no_log_req
#endif

#if (MMS_GETAS_EN & (REQ_EN | RESP_EN))
#define MMS_GETAS_RSP_LFUN		m_log_getas_resp
#define MMS_GETAS_REQ_LFUN 		m_log_getas_req
#else
#define MMS_GETAS_RSP_LFUN		m_no_log_resp
#define MMS_GETAS_REQ_LFUN 		m_no_log_req
#endif

#if (MMS_GETAES_EN & (REQ_EN | RESP_EN))
#define MMS_GETAES_RSP_LFUN		m_log_getaes_resp
#define MMS_GETAES_REQ_LFUN 		m_log_getaes_req
#else
#define MMS_GETAES_RSP_LFUN		m_no_log_resp
#define MMS_GETAES_REQ_LFUN 		m_no_log_req
#endif

/************************************************************************/
/* JOURNAL MANAGEMENT SERVICES 						*/
/************************************************************************/

#if (MMS_JREAD_EN & (REQ_EN | RESP_EN))
#define MMS_JREAD_RSP_LFUN		m_log_jread_resp
#define MMS_JREAD_REQ_LFUN 		m_log_jread_req
#else
#define MMS_JREAD_RSP_LFUN		m_no_log_resp
#define MMS_JREAD_REQ_LFUN 		m_no_log_req
#endif

#if (MMS_JWRITE_EN & (REQ_EN | RESP_EN))
#define MMS_JWRITE_RSP_LFUN		m_no_log_resp
#define MMS_JWRITE_REQ_LFUN 		m_log_jwrite_req
#else
#define MMS_JWRITE_RSP_LFUN		m_no_log_resp
#define MMS_JWRITE_REQ_LFUN 		m_no_log_req
#endif

#if (MMS_JINIT_EN & (REQ_EN | RESP_EN))
#define MMS_JINIT_RSP_LFUN		m_log_jinit_resp
#define MMS_JINIT_REQ_LFUN 		m_log_jinit_req
#else
#define MMS_JINIT_RSP_LFUN		m_no_log_resp
#define MMS_JINIT_REQ_LFUN 		m_no_log_req
#endif

#if (MMS_JSTAT_EN & (REQ_EN | RESP_EN))
#define MMS_JSTAT_RSP_LFUN		m_log_jstat_resp
#define MMS_JSTAT_REQ_LFUN 		m_log_jstat_req
#else
#define MMS_JSTAT_RSP_LFUN		m_no_log_resp
#define MMS_JSTAT_REQ_LFUN 		m_no_log_req
#endif

#if (MMS_JCREATE_EN & (REQ_EN | RESP_EN))
#define MMS_JCREATE_RSP_LFUN		m_no_log_resp
#define MMS_JCREATE_REQ_LFUN 		m_log_jcreate_req
#else
#define MMS_JCREATE_RSP_LFUN		m_no_log_resp
#define MMS_JCREATE_REQ_LFUN 		m_no_log_req
#endif

#if (MMS_JDELETE_EN & (REQ_EN | RESP_EN))
#define MMS_JDELETE_RSP_LFUN		m_no_log_resp
#define MMS_JDELETE_REQ_LFUN 		m_log_jdelete_req
#else
#define MMS_JDELETE_RSP_LFUN		m_no_log_resp
#define MMS_JDELETE_REQ_LFUN 		m_no_log_req
#endif

/************************************************************************/
/* FILE MANAGEMENT SERVICES 						*/
/************************************************************************/

#if (MMS_OBTAINFILE_EN & (REQ_EN | RESP_EN))
#define MMS_OBTAINFILE_RSP_LFUN		m_no_log_resp
#define MMS_OBTAINFILE_REQ_LFUN 	m_log_obtfile_req
#else
#define MMS_OBTAINFILE_RSP_LFUN		m_no_log_resp
#define MMS_OBTAINFILE_REQ_LFUN 	m_no_log_req
#endif

#if (MMS_FOPEN_EN & (REQ_EN | RESP_EN))
#define MMS_FOPEN_RSP_LFUN		m_log_fopen_resp
#define MMS_FOPEN_REQ_LFUN 		m_log_fopen_req
#else
#define MMS_FOPEN_RSP_LFUN		m_no_log_resp
#define MMS_FOPEN_REQ_LFUN 		m_no_log_req
#endif

#if (MMS_FREAD_EN & (REQ_EN | RESP_EN))
#define MMS_FREAD_RSP_LFUN		m_log_fread_resp
#define MMS_FREAD_REQ_LFUN 		m_log_fread_req
#else
#define MMS_FREAD_RSP_LFUN		m_no_log_resp
#define MMS_FREAD_REQ_LFUN 		m_no_log_req
#endif

#if (MMS_FCLOSE_EN & (REQ_EN | RESP_EN))
#define MMS_FCLOSE_RSP_LFUN		m_no_log_resp
#define MMS_FCLOSE_REQ_LFUN 		m_log_fclose_req
#else
#define MMS_FCLOSE_RSP_LFUN		m_no_log_resp
#define MMS_FCLOSE_REQ_LFUN 		m_no_log_req
#endif

#if (MMS_FRENAME_EN & (REQ_EN | RESP_EN))
#define MMS_FRENAME_RSP_LFUN		m_no_log_resp
#define MMS_FRENAME_REQ_LFUN 		m_log_frename_req
#else
#define MMS_FRENAME_RSP_LFUN		m_no_log_resp
#define MMS_FRENAME_REQ_LFUN 		m_no_log_req
#endif

#if (MMS_FDELETE_EN & (REQ_EN | RESP_EN))
#define MMS_FDELETE_RSP_LFUN		m_no_log_resp
#define MMS_FDELETE_REQ_LFUN 		m_log_fdelete_req
#else
#define MMS_FDELETE_RSP_LFUN		m_no_log_resp
#define MMS_FDELETE_REQ_LFUN 		m_no_log_req
#endif

#if (MMS_FDIR_EN & (REQ_EN | RESP_EN))
#define MMS_FDIR_RSP_LFUN		m_log_fdir_resp
#define MMS_FDIR_REQ_LFUN 		m_log_fdir_req
#else
#define MMS_FDIR_RSP_LFUN		m_no_log_resp
#define MMS_FDIR_REQ_LFUN 		m_no_log_req
#endif

/************************************************************************/
/************************************************************************/
/* UNCONFIRMED SERVICES FROM VARIABLE ACCESS, VMD SUPPORT AND EVENT	*/
/* MANAGEMENT SERVICES							*/
/************************************************************************/
#if (MMS_INFO_EN & (REQ_EN | RESP_EN))
#define MMS_INFO_REQ_LFUN		m_log_info_req
#define MMS_INFO_RSP_LFUN		m_no_log_resp
#else
#define MMS_INFO_RSP_LFUN		m_no_log_resp
#define MMS_INFO_REQ_LFUN 		m_no_log_req
#endif

#if (MMS_USTATUS_EN & (REQ_EN | RESP_EN))
#define MMS_USTATUS_REQ_LFUN		m_log_ustatus_req
#define MMS_USTATUS_RSP_LFUN		m_no_log_resp
#else
#define MMS_USTATUS_RSP_LFUN		m_no_log_resp
#define MMS_USTATUS_REQ_LFUN 		m_no_log_req
#endif

#if (MMS_EVNOT_EN & (REQ_EN | RESP_EN))
#define MMS_EVNOT_REQ_LFUN		m_log_evnot_req
#define MMS_EVNOT_RSP_LFUN		m_no_log_resp
#else
#define MMS_EVNOT_RSP_LFUN		m_no_log_resp
#define MMS_EVNOT_REQ_LFUN 		m_no_log_req
#endif

/************************************************************************/
/************************************************************************/
/* ENVIRONMENT & GENERAL MANAGEMENT					*/
/************************************************************************/

#define MMS_CONCLUDE_RSP_LFUN		m_no_log_resp
#define MMS_CONCLUDE_REQ_LFUN 		m_no_log_req

#if (MMS_CANCEL_EN & (REQ_EN | RESP_EN))
#define MMS_CANCEL_RSP_LFUN		m_no_log_resp
#define MMS_CANCEL_REQ_LFUN 		m_no_log_req
#else
#define MMS_CANCEL_RSP_LFUN		m_no_log_resp
#define MMS_CANCEL_REQ_LFUN 		m_no_log_req
#endif

#if (MMS_INIT_EN & (REQ_EN | RESP_EN))
#define MMS_INIT_RSP_LFUN		m_log_init_info
#define MMS_INIT_REQ_LFUN 		m_log_init_info
#else
#define MMS_INIT_RSP_LFUN		m_no_log_resp
#define MMS_INIT_REQ_LFUN 		m_no_log_req
#endif


/************************************************************************/
/************************************************************************/
/* initialize the REQUEST/INDICATION LOG table, opcode is index         */
/************************************************************************/

ST_VOID (*m_req_log_fun_tbl [MAX_MMSOP_DIS+1]) (ST_VOID *) =
  {
  (ST_VOID (*)(ST_VOID *)) MMS_STATUS_REQ_LFUN,		/*  00 STATUS			*/
  (ST_VOID (*)(ST_VOID *)) MMS_GETNAMES_REQ_LFUN, 	/*  01 GET_NAMLIST		*/
  (ST_VOID (*)(ST_VOID *)) MMS_IDENT_REQ_LFUN,		/*  02 IDENTIFY 		*/
  (ST_VOID (*)(ST_VOID *)) MMS_RENAME_REQ_LFUN,		/*  03 RENAME			*/
  (ST_VOID (*)(ST_VOID *)) MMS_READ_REQ_LFUN,		/*  04 READ			*/
  (ST_VOID (*)(ST_VOID *)) MMS_WRITE_REQ_LFUN,		/*  05 WRITE			*/
  (ST_VOID (*)(ST_VOID *)) MMS_GETVAR_REQ_LFUN,		/*  06 GET_VARDEF		*/
  (ST_VOID (*)(ST_VOID *)) MMS_DEFVAR_REQ_LFUN,		/*  07 DEF_VARNAM		*/
  (ST_VOID (*)(ST_VOID *)) MMS_DEFSCAT_REQ_LFUN,	/*  08 DEF_SCATTERED		*/
  (ST_VOID (*)(ST_VOID *)) MMS_GETSCAT_REQ_LFUN,	/*  09 GET_SCATTERED		*/
  (ST_VOID (*)(ST_VOID *)) MMS_DELVAR_REQ_LFUN,		/*  10 DEL_VARNAM		*/
  (ST_VOID (*)(ST_VOID *)) MMS_DEFVLIST_REQ_LFUN,	/*  11 DEF_VARLIST		*/
  (ST_VOID (*)(ST_VOID *)) MMS_GETVLIST_REQ_LFUN,	/*  12 GET_VARLIST		*/
  (ST_VOID (*)(ST_VOID *)) MMS_DELVLIST_REQ_LFUN,	/*  13 DEL_VARLIST		*/
  (ST_VOID (*)(ST_VOID *)) MMS_DEFTYPE_REQ_LFUN,	/*  14 DEF_TYPENAM		*/
  (ST_VOID (*)(ST_VOID *)) MMS_GETTYPE_REQ_LFUN,	/*  15 GET_TYPEDEF		*/
  (ST_VOID (*)(ST_VOID *)) MMS_DELTYPE_REQ_LFUN,	/*  16 DEL_TYPENAM		*/
  (ST_VOID (*)(ST_VOID *)) MMS_INPUT_REQ_LFUN,		/*  17 INPUT			*/
  (ST_VOID (*)(ST_VOID *)) MMS_OUTPUT_REQ_LFUN,		/*  18 OUTPUT			*/
  (ST_VOID (*)(ST_VOID *)) MMS_TAKECTRL_REQ_LFUN,	/*  19 TAKE_CONTROL		*/
  (ST_VOID (*)(ST_VOID *)) MMS_RELCTRL_REQ_LFUN,	/*  20 REL_CONTROL		*/
  (ST_VOID (*)(ST_VOID *)) MMS_DEFINE_SEM_REQ_LFUN,	/*  21 DEF_SEMAPHORE		*/
  (ST_VOID (*)(ST_VOID *)) MMS_DELETE_SEM_REQ_LFUN,	/*  22 DEL_SEMAPHORE		*/
  (ST_VOID (*)(ST_VOID *)) MMS_REP_SEMSTAT_REQ_LFUN,	/*  23 REP_SEM_STATUS		*/
  (ST_VOID (*)(ST_VOID *)) MMS_REP_SEMPOOL_REQ_LFUN,	/*  24 REP_SEM_POOL_STATUS	*/
  (ST_VOID (*)(ST_VOID *)) MMS_REP_SEMENTRY_REQ_LFUN,	/*  25 REP_SEM_ENTRY_STATUS	*/
  (ST_VOID (*)(ST_VOID *)) MMS_INIT_DWN_REQ_LFUN,	/*  26 INIT_DOWNLOAD		*/
  (ST_VOID (*)(ST_VOID *)) MMS_DWN_LOAD_REQ_LFUN,	/*  27 DOWN_LOAD		*/
  (ST_VOID (*)(ST_VOID *)) MMS_TERM_DWN_REQ_LFUN,	/*  28 TERM_DOWNLOAD		*/
  (ST_VOID (*)(ST_VOID *)) MMS_INIT_UPL_REQ_LFUN,	/*  29 INIT_UPLOAD		*/
  (ST_VOID (*)(ST_VOID *)) MMS_UP_LOAD_REQ_LFUN,	/*  30 UP_LOAD			*/
  (ST_VOID (*)(ST_VOID *)) MMS_TERM_UPL_REQ_LFUN,	/*  31 TERM_UPLOAD		*/
  (ST_VOID (*)(ST_VOID *)) MMS_RDDWN_REQ_LFUN,		/*  32 REQ_DOWNLOAD		*/
  (ST_VOID (*)(ST_VOID *)) MMS_RDUPL_REQ_LFUN,		/*  33 REQ_UPLOAD		*/
  (ST_VOID (*)(ST_VOID *)) MMS_LOAD_DOM_REQ_LFUN,	/*  34 LOAD_DOMAIN		*/
  (ST_VOID (*)(ST_VOID *)) MMS_STR_DOM_REQ_LFUN,	/*  35 STORE_DOMAIN		*/
  (ST_VOID (*)(ST_VOID *)) MMS_DEL_DOM_REQ_LFUN,	/*  36 DELETE_DOMAIN		*/
  (ST_VOID (*)(ST_VOID *)) MMS_GET_DOM_REQ_LFUN,	/*  37 GET_DOMAIN		*/
  (ST_VOID (*)(ST_VOID *)) MMS_CRE_PI_REQ_LFUN,		/*  38 CREATE_PROGRAM_INVOCATION*/
  (ST_VOID (*)(ST_VOID *)) MMS_DEL_PI_REQ_LFUN,		/*  39 DEL_PROGRAM_INVOCATION	*/
  (ST_VOID (*)(ST_VOID *)) MMS_START_REQ_LFUN,		/*  40 START			*/
  (ST_VOID (*)(ST_VOID *)) MMS_STOP_REQ_LFUN,		/*  41 STOP			*/
  (ST_VOID (*)(ST_VOID *)) MMS_RESUME_REQ_LFUN,		/*  42 RESUME			*/
  (ST_VOID (*)(ST_VOID *)) MMS_RESET_REQ_LFUN,		/*  43 RESET			*/
  (ST_VOID (*)(ST_VOID *)) MMS_KILL_REQ_LFUN,		/*  44 KILL			*/
  (ST_VOID (*)(ST_VOID *)) MMS_GET_PI_REQ_LFUN,		/*  45 GET_PROGRAM_INVOCATION	*/
  (ST_VOID (*)(ST_VOID *)) MMS_OBTAINFILE_REQ_LFUN,	/*  46 OBTAIN_FILE		*/
  (ST_VOID (*)(ST_VOID *)) MMS_DEFEC_REQ_LFUN,		/*  47 DEFINE EVENT CONDITION	*/
  (ST_VOID (*)(ST_VOID *)) MMS_DELEC_REQ_LFUN,		/*  48 DELETE EVENT CONDITION	*/
  (ST_VOID (*)(ST_VOID *)) MMS_GETECA_REQ_LFUN,		/*  49 GET EVENT CONDITION ATTR	*/
  (ST_VOID (*)(ST_VOID *)) MMS_REPECS_REQ_LFUN,		/*  50 REPORT EVENT COND STATUS	*/
  (ST_VOID (*)(ST_VOID *)) MMS_ALTECM_REQ_LFUN,		/*  51 ALTER EV COND MONITORING	*/
  (ST_VOID (*)(ST_VOID *)) MMS_TRIGE_REQ_LFUN,		/*  52 TRIGGER EVENT		*/
  (ST_VOID (*)(ST_VOID *)) MMS_DEFEA_REQ_LFUN,		/*  53 DEFINE EVENT ACTION	*/
  (ST_VOID (*)(ST_VOID *)) MMS_DELEA_REQ_LFUN,		/*  54 DELETE EVENT ACTION	*/
  (ST_VOID (*)(ST_VOID *)) MMS_GETEAA_REQ_LFUN,		/*  55 GET EV ACTION ATTRIBUTES	*/
  (ST_VOID (*)(ST_VOID *)) MMS_REPEAS_REQ_LFUN,		/*  56 REPORT EV ACTION STATUS	*/
  (ST_VOID (*)(ST_VOID *)) MMS_DEFEE_REQ_LFUN,		/*  57 DEFINE EVENT ENROLLMENT	*/
  (ST_VOID (*)(ST_VOID *)) MMS_DELEE_REQ_LFUN,		/*  58 DELETE EVENT ENROLLMENT	*/
  (ST_VOID (*)(ST_VOID *)) MMS_ALTEE_REQ_LFUN,		/*  59 ALTER EVENT ENROLLMENT	*/
  (ST_VOID (*)(ST_VOID *)) MMS_REPEES_REQ_LFUN,		/*  60 REPORT ENROLL ATTRIBUTES	*/
  (ST_VOID (*)(ST_VOID *)) MMS_GETEEA_REQ_LFUN,		/*  61 GET ENROLLMENT STATUS	*/
  (ST_VOID (*)(ST_VOID *)) MMS_ACKEVNOT_REQ_LFUN,	/*  62 ACK_EVENT_NOTIFICATION	*/
  (ST_VOID (*)(ST_VOID *)) MMS_GETAS_REQ_LFUN,		/*  63 GET_ALARM_SUMMARY	*/
  (ST_VOID (*)(ST_VOID *)) MMS_GETAES_REQ_LFUN,		/*  64 GET ALARM ENROLLMENT SUM	*/
  (ST_VOID (*)(ST_VOID *)) MMS_JREAD_REQ_LFUN,		/*  65 READ_JOURNAL		*/
  (ST_VOID (*)(ST_VOID *)) MMS_JWRITE_REQ_LFUN,		/*  66 WRITE_JOURNAL		*/
  (ST_VOID (*)(ST_VOID *)) MMS_JINIT_REQ_LFUN,		/*  67 INITIALIZE_JOURNAL	*/
  (ST_VOID (*)(ST_VOID *)) MMS_JSTAT_REQ_LFUN,		/*  68 REPORT_JOURNAL_STATUS	*/
  (ST_VOID (*)(ST_VOID *)) MMS_JCREATE_REQ_LFUN,	/*  69 CREATE JOURNAL		*/
  (ST_VOID (*)(ST_VOID *)) MMS_JDELETE_REQ_LFUN,	/*  70 DELETE JOURNAL		*/
  (ST_VOID (*)(ST_VOID *)) MMS_GETCL_REQ_LFUN,		/*  71 GET CAPABILITY LIST 	*/
  (ST_VOID (*)(ST_VOID *)) MMS_FOPEN_REQ_LFUN,		/*  72 FILE_OPEN		*/
  (ST_VOID (*)(ST_VOID *)) MMS_FREAD_REQ_LFUN,		/*  73 FILE_READ		*/
  (ST_VOID (*)(ST_VOID *)) MMS_FCLOSE_REQ_LFUN,		/*  74 FILE_CLOSE		*/
  (ST_VOID (*)(ST_VOID *)) MMS_FRENAME_REQ_LFUN,	/*  75 FILE_RENAME		*/
  (ST_VOID (*)(ST_VOID *)) MMS_FDELETE_REQ_LFUN,	/*  76 FILE_DELETE		*/
  (ST_VOID (*)(ST_VOID *)) MMS_FDIR_REQ_LFUN,		/*  77 FILE_DIR 		*/
  (ST_VOID (*)(ST_VOID *)) MMS_USTATUS_REQ_LFUN,	/*  78 UNSOLICITED_STATUS	*/
  (ST_VOID (*)(ST_VOID *)) MMS_INFO_REQ_LFUN,		/*  79 INFO_RPT 		*/
  (ST_VOID (*)(ST_VOID *)) MMS_EVNOT_REQ_LFUN,		/*  80 EVENT NOTIFICATION	*/
  (ST_VOID (*)(ST_VOID *)) m_no_log_req,	        /*  81 ATTACH TO EVENT COND	*/
  (ST_VOID (*)(ST_VOID *)) m_no_log_req,	        /*  82 ATTACH TO SEMAPHORE	*/
  (ST_VOID (*)(ST_VOID *)) MMS_CONCLUDE_REQ_LFUN,	/*  83 CONCLUDE 		*/
  (ST_VOID (*)(ST_VOID *)) MMS_CANCEL_REQ_LFUN,		/*  84 CANCEL			*/
  (ST_VOID (*)(ST_VOID *)) MMS_INIT_REQ_LFUN		/*  85 INITIATE 		*/
  };

/************************************************************************/
/* initialize the RESPONSE/CONFIRM LOG table, opcode is index 	        */
/************************************************************************/
ST_VOID (*m_resp_log_fun_tbl [MAX_MMSOP_DIS+1]) (ST_VOID *) =
  {
  (ST_VOID (*)(ST_VOID *)) MMS_STATUS_RSP_LFUN,		/*  00 STATUS			*/
  (ST_VOID (*)(ST_VOID *)) MMS_GETNAMES_RSP_LFUN, 	/*  01 GET_NAMLIST		*/
  (ST_VOID (*)(ST_VOID *)) MMS_IDENT_RSP_LFUN,		/*  02 IDENTIFY 		*/
  (ST_VOID (*)(ST_VOID *)) MMS_RENAME_RSP_LFUN,		/*  03 RENAME			*/
  (ST_VOID (*)(ST_VOID *)) MMS_READ_RSP_LFUN,		/*  04 READ			*/
  (ST_VOID (*)(ST_VOID *)) MMS_WRITE_RSP_LFUN,		/*  05 WRITE			*/
  (ST_VOID (*)(ST_VOID *)) MMS_GETVAR_RSP_LFUN,		/*  06 GET_VARDEF		*/
  (ST_VOID (*)(ST_VOID *)) MMS_DEFVAR_RSP_LFUN,		/*  07 DEF_VARNAM		*/
  (ST_VOID (*)(ST_VOID *)) MMS_DEFSCAT_RSP_LFUN,	/*  08 DEF_SCATTERED		*/
  (ST_VOID (*)(ST_VOID *)) MMS_GETSCAT_RSP_LFUN,	/*  09 GET_SCATTERED		*/
  (ST_VOID (*)(ST_VOID *)) MMS_DELVAR_RSP_LFUN,		/*  10 DEL_VARNAM		*/
  (ST_VOID (*)(ST_VOID *)) MMS_DEFVLIST_RSP_LFUN,	/*  11 DEF_VARLIST		*/
  (ST_VOID (*)(ST_VOID *)) MMS_GETVLIST_RSP_LFUN,	/*  12 GET_VARLIST		*/
  (ST_VOID (*)(ST_VOID *)) MMS_DELVLIST_RSP_LFUN,	/*  13 DEL_VARLIST		*/
  (ST_VOID (*)(ST_VOID *)) MMS_DEFTYPE_RSP_LFUN,	/*  14 DEF_TYPENAM		*/
  (ST_VOID (*)(ST_VOID *)) MMS_GETTYPE_RSP_LFUN,	/*  15 GET_TYPEDEF		*/
  (ST_VOID (*)(ST_VOID *)) MMS_DELTYPE_RSP_LFUN,	/*  16 DEL_TYPENAM		*/
  (ST_VOID (*)(ST_VOID *)) MMS_INPUT_RSP_LFUN,		/*  17 INPUT			*/
  (ST_VOID (*)(ST_VOID *)) MMS_OUTPUT_RSP_LFUN,		/*  18 OUTPUT			*/
  (ST_VOID (*)(ST_VOID *)) MMS_TAKECTRL_RSP_LFUN,	/*  19 TAKE_CONTROL		*/
  (ST_VOID (*)(ST_VOID *)) MMS_RELCTRL_RSP_LFUN,	/*  20 REL_CONTROL		*/
  (ST_VOID (*)(ST_VOID *)) MMS_DEFINE_SEM_RSP_LFUN,	/*  21 DEF_SEMAPHORE		*/
  (ST_VOID (*)(ST_VOID *)) MMS_DELETE_SEM_RSP_LFUN,	/*  22 DEL_SEMAPHORE		*/
  (ST_VOID (*)(ST_VOID *)) MMS_REP_SEMSTAT_RSP_LFUN,	/*  23 REP_SEM_STATUS		*/
  (ST_VOID (*)(ST_VOID *)) MMS_REP_SEMPOOL_RSP_LFUN,	/*  24 REP_SEM_POOL_STATUS	*/
  (ST_VOID (*)(ST_VOID *)) MMS_REP_SEMENTRY_RSP_LFUN,	/*  25 REP_SEM_ENTRY_STATUS	*/
  (ST_VOID (*)(ST_VOID *)) MMS_INIT_DWN_RSP_LFUN,	/*  26 INIT_DOWNLOAD		*/
  (ST_VOID (*)(ST_VOID *)) MMS_DWN_LOAD_RSP_LFUN,	/*  27 DOWN_LOAD		*/
  (ST_VOID (*)(ST_VOID *)) MMS_TERM_DWN_RSP_LFUN,	/*  28 TERM_DOWNLOAD		*/
  (ST_VOID (*)(ST_VOID *)) MMS_INIT_UPL_RSP_LFUN,	/*  29 INIT_UPLOAD		*/
  (ST_VOID (*)(ST_VOID *)) MMS_UP_LOAD_RSP_LFUN,	/*  30 UP_LOAD			*/
  (ST_VOID (*)(ST_VOID *)) MMS_TERM_UPL_RSP_LFUN,	/*  31 TERM_UPLOAD		*/
  (ST_VOID (*)(ST_VOID *)) MMS_RDDWN_RSP_LFUN,		/*  32 REQ_DOWNLOAD		*/
  (ST_VOID (*)(ST_VOID *)) MMS_RDUPL_RSP_LFUN,		/*  33 REQ_UPLOAD		*/
  (ST_VOID (*)(ST_VOID *)) MMS_LOAD_DOM_RSP_LFUN,	/*  34 LOAD_DOMAIN		*/
  (ST_VOID (*)(ST_VOID *)) MMS_STR_DOM_RSP_LFUN,	/*  35 STORE_DOMAIN		*/
  (ST_VOID (*)(ST_VOID *)) MMS_DEL_DOM_RSP_LFUN,	/*  36 DELETE_DOMAIN		*/
  (ST_VOID (*)(ST_VOID *)) MMS_GET_DOM_RSP_LFUN,	/*  37 GET_DOMAIN		*/
  (ST_VOID (*)(ST_VOID *)) MMS_CRE_PI_RSP_LFUN,		/*  38 CREATE_PROGRAM_INVOCATION*/
  (ST_VOID (*)(ST_VOID *)) MMS_DEL_PI_RSP_LFUN,		/*  39 DEL_PROGRAM_INVOCATION	*/
  (ST_VOID (*)(ST_VOID *)) MMS_START_RSP_LFUN,		/*  40 START			*/
  (ST_VOID (*)(ST_VOID *)) MMS_STOP_RSP_LFUN,		/*  41 STOP			*/
  (ST_VOID (*)(ST_VOID *)) MMS_RESUME_RSP_LFUN,		/*  42 RESUME			*/
  (ST_VOID (*)(ST_VOID *)) MMS_RESET_RSP_LFUN,		/*  43 RESET			*/
  (ST_VOID (*)(ST_VOID *)) MMS_KILL_RSP_LFUN,		/*  44 KILL			*/
  (ST_VOID (*)(ST_VOID *)) MMS_GET_PI_RSP_LFUN,		/*  45 GET_PROGRAM_INVOCATION	*/
  (ST_VOID (*)(ST_VOID *)) MMS_OBTAINFILE_RSP_LFUN,	/*  46 OBTAIN_FILE		*/
  (ST_VOID (*)(ST_VOID *)) MMS_DEFEC_RSP_LFUN,		/*  47 DEFINE EVENT CONDITION	*/
  (ST_VOID (*)(ST_VOID *)) MMS_DELEC_RSP_LFUN,		/*  48 DELETE EVENT CONDITION	*/
  (ST_VOID (*)(ST_VOID *)) MMS_GETECA_RSP_LFUN,		/*  49 GET EVENT CONDITION ATTR	*/
  (ST_VOID (*)(ST_VOID *)) MMS_REPECS_RSP_LFUN,		/*  50 REPORT EVENT COND STATUS	*/
  (ST_VOID (*)(ST_VOID *)) MMS_ALTECM_RSP_LFUN,		/*  51 ALTER EV COND MONITORING	*/
  (ST_VOID (*)(ST_VOID *)) MMS_TRIGE_RSP_LFUN,		/*  52 TRIGGER EVENT		*/
  (ST_VOID (*)(ST_VOID *)) MMS_DEFEA_RSP_LFUN,		/*  53 DEFINE EVENT ACTION	*/
  (ST_VOID (*)(ST_VOID *)) MMS_DELEA_RSP_LFUN,		/*  54 DELETE EVENT ACTION	*/
  (ST_VOID (*)(ST_VOID *)) MMS_GETEAA_RSP_LFUN,		/*  55 GET EV ACTION ATTRIBUTES	*/
  (ST_VOID (*)(ST_VOID *)) MMS_REPEAS_RSP_LFUN,		/*  56 REPORT EV ACTION STATUS	*/
  (ST_VOID (*)(ST_VOID *)) MMS_DEFEE_RSP_LFUN,		/*  57 DEFINE EVENT ENROLLMENT	*/
  (ST_VOID (*)(ST_VOID *)) MMS_DELEE_RSP_LFUN,		/*  58 DELETE EVENT ENROLLMENT	*/
  (ST_VOID (*)(ST_VOID *)) MMS_ALTEE_RSP_LFUN,		/*  59 ALTER EVENT ENROLLMENT	*/
  (ST_VOID (*)(ST_VOID *)) MMS_REPEES_RSP_LFUN,		/*  60 REPORT ENROLL ATTRIBUTES	*/
  (ST_VOID (*)(ST_VOID *)) MMS_GETEEA_RSP_LFUN,		/*  61 GET ENROLLMENT STATUS	*/
  (ST_VOID (*)(ST_VOID *)) MMS_ACKEVNOT_RSP_LFUN,	/*  62 ACK_EVENT_NOTIFICATION	*/
  (ST_VOID (*)(ST_VOID *)) MMS_GETAS_RSP_LFUN,		/*  63 GET_ALARM_SUMMARY	*/
  (ST_VOID (*)(ST_VOID *)) MMS_GETAES_RSP_LFUN,		/*  64 GET ALARM ENROLLMENT SUM	*/
  (ST_VOID (*)(ST_VOID *)) MMS_JREAD_RSP_LFUN,		/*  65 READ_JOURNAL		*/
  (ST_VOID (*)(ST_VOID *)) MMS_JWRITE_RSP_LFUN,		/*  66 WRITE_JOURNAL		*/
  (ST_VOID (*)(ST_VOID *)) MMS_JINIT_RSP_LFUN,		/*  67 INITIALIZE_JOURNAL	*/
  (ST_VOID (*)(ST_VOID *)) MMS_JSTAT_RSP_LFUN,		/*  68 REPORT_JOURNAL_STATUS	*/
  (ST_VOID (*)(ST_VOID *)) MMS_JCREATE_RSP_LFUN,	/*  69 CREATE JOURNAL		*/
  (ST_VOID (*)(ST_VOID *)) MMS_JDELETE_RSP_LFUN,	/*  70 DELETE JOURNAL		*/
  (ST_VOID (*)(ST_VOID *)) MMS_GETCL_RSP_LFUN,		/*  71 GET CAPABILITY LIST 	*/
  (ST_VOID (*)(ST_VOID *)) MMS_FOPEN_RSP_LFUN,		/*  72 FILE_OPEN		*/
  (ST_VOID (*)(ST_VOID *)) MMS_FREAD_RSP_LFUN,		/*  73 FILE_READ		*/
  (ST_VOID (*)(ST_VOID *)) MMS_FCLOSE_RSP_LFUN,		/*  74 FILE_CLOSE		*/
  (ST_VOID (*)(ST_VOID *)) MMS_FRENAME_RSP_LFUN,	/*  75 FILE_RENAME		*/
  (ST_VOID (*)(ST_VOID *)) MMS_FDELETE_RSP_LFUN,	/*  76 FILE_DELETE		*/
  (ST_VOID (*)(ST_VOID *)) MMS_FDIR_RSP_LFUN,		/*  77 FILE_DIR 		*/
  (ST_VOID (*)(ST_VOID *)) MMS_USTATUS_RSP_LFUN,	/*  78 UNSOLICITED_STATUS	*/
  (ST_VOID (*)(ST_VOID *)) MMS_INFO_RSP_LFUN,		/*  79 INFO_RPT 		*/
  (ST_VOID (*)(ST_VOID *)) MMS_EVNOT_RSP_LFUN,		/*  80 EVENT NOTIFICATION	*/
  (ST_VOID (*)(ST_VOID *)) m_no_log_resp,	        /*  81 ATTACH TO EVENT COND	*/
  (ST_VOID (*)(ST_VOID *)) m_no_log_resp,	        /*  82 ATTACH TO SEMAPHORE	*/
  (ST_VOID (*)(ST_VOID *)) MMS_CONCLUDE_RSP_LFUN,	/*  83 CONCLUDE 		*/
  (ST_VOID (*)(ST_VOID *)) MMS_CANCEL_RSP_LFUN,		/*  84 CANCEL			*/
  (ST_VOID (*)(ST_VOID *)) MMS_INIT_RSP_LFUN		/*  85 INITIATE 		*/
  };


/************************************************************************/
#endif /* MLOG_ENABLE */
/************************************************************************/

/************************************************************************/
/* Initialize the main MLOG function pointers (in global structure).	*/
/************************************************************************/
#if defined(MMS_LITE)
#if defined(MLOG_ENABLE)
  /* Global structure initialized.	*/
  MLOGL_INFO mlogl_info =
    {
    _ml_log_req_info,
    _ml_log_ind_info,

    _ml_log_resp_info,
    _ml_log_conf_info,

    _ml_log_unsol_req_info,
    _ml_log_unsol_ind_info,

    _ml_log_error_conf,
    _ml_log_error_resp,

    _ml_log_reject_send,
    _ml_log_reject_recv
    };
#else	/* !defined(MLOG_ENABLE)	*/
  /* Global structure uninitialized (all function pointers = NULL).	*/
  MLOGL_INFO mlogl_info;
#endif	/* !defined(MLOG_ENABLE)	*/
#endif	/* defined(MMS_LITE)		*/

