/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1993 - 1997, All Rights Reserved.		        */
/*									*/
/* MODULE NAME : mms_def2.h						*/
/* PRODUCT(S)  : MMSEASE, MMSEASE-LITE 					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module contains miscellaneous definitions and variables.	*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 11/12/07  MDE     11    Added MMSOP_RD_USR_HANDLED, etc..		*/
/* 04/24/03  JRB     10    Increase MAX_IDENT_LEN from 32 to 64.	*/
/* 07/09/02  MDE     09    Fixed up opcode defines			*/
/* 02/24/00  DSF     08    Bumped up MAX_FILES_OPEN to 100		*/
/* 10/06/99  GLB     07    Added file Rename and Obtain File            */
/* 09/13/99  MDE     06    Added SD_CONST modifiers			*/
/* 04/14/98  JRB     05    Del MPARAM* defs. Still needed in mmsop_en.h	*/
/* 04/03/98  JRB     04    Add check for obsolete version of mmsop_en.h	*/
/* 03/20/98  JRB     03    Move defs from "mmsop_en.h" to here.		*/
/* 08/15/97  MDE     02    Added xxx_DATA_SUPPORT defines, cleanup	*/
/* 08/14/97  RKR     01    Format changes				*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#ifndef MMS_DEF2_INCLUDED
#define MMS_DEF2_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#if defined (REQ_RESP_DIS) || defined (MAX_MMS_OPCODE)
#error May have obsolete version of "mmsop_en.h". See Release Notes.
#endif

/************************************************************************/
/************************************************************************/
/* define the operation enable switches used by "mmsop_en.h"		*/
/************************************************************************/
#define REQ_RESP_DIS	0x00		/* no support for req or resp	*/
#define REQ_EN		0x01		/* support for request		*/
#define RESP_EN 	0x02		/* support for response 	*/
#define REQ_RESP_EN	0x03		/* support for resp and req	*/

/************************************************************************/
/************************************************************************/
/* The following character array is used to check if a MMS operation is */
/* to be supported as requestor and/or responder, and whether to expect */
/* a constructor or primitive after the invoke ID. The operation opcode */
/* is used as an index into the table.					*/
/************************************************************************/

#define MAX_MMS_OPCODE	77		/* highest MMS opcode		*/
#define MAX_MMSOP_DIS	85		/* highest mmsop_en[] entry	*/

extern SD_CONST ST_UCHAR mmsop_en [MAX_MMSOP_DIS+1];

/************************************************************************/
/************************************************************************/
/* The following array is used to print the opcode description.	 The	*/
/* operation opcode is used as an index into the table.	 		*/
/************************************************************************/

extern ST_CHAR *mms_op_string[106];
extern ST_INT m_num_mms_op_string;

/************************************************************************/
/************************************************************************/
/* Define the MMS operation opcodes					*/
/************************************************************************/
#define MMSOP_STATUS		 0 /* status				*/
#define MMSOP_GET_NAMLIST	 1 /* get name list			*/
#define MMSOP_IDENTIFY	 	 2 /* identify				*/
#define MMSOP_RENAME		 3 /* rename				*/
#define MMSOP_READ		 4 /* read 				*/
#define MMSOP_WRITE	 	 5 /* write				*/
#define MMSOP_GET_VAR 	 	 6 /* get variable name definition 	*/
#define MMSOP_DEF_VAR 	 	 7 /* define variable name 		*/
#define MMSOP_DEF_SCAT 	 	 8 /* define scattered access		*/
#define MMSOP_GET_SCAT 	 	 9 /* get scattered access attributes	*/
#define MMSOP_DEL_VAR 		10 /* delete variable name definition	*/
#define MMSOP_DEF_VLIST 	11 /* define named variable list	*/
#define MMSOP_GET_VLIST 	12 /* get named variable list		*/
#define MMSOP_DEL_VLIST 	13 /* delete named variable list	*/
#define MMSOP_DEF_TYPE		14 /* define type name			*/
#define MMSOP_GET_TYPE		15 /* get type name definition		*/
#define MMSOP_DEL_TYPE		16 /* delete type name			*/

#define MMSOP_INPUT		17 /* input				*/
#define MMSOP_OUTPUT		18 /* output				*/

#define MMSOP_TAKE_CONTROL	19 /* take control 			*/
#define MMSOP_REL_CONTROL	20 /* relinquish control		*/
#define MMSOP_DEFINE_SEM	21 /* define semaphore			*/
#define MMSOP_DELETE_SEM	22 /* delete semaphore			*/
#define MMSOP_REP_SEMSTAT	23 /* report semaphore status		*/
#define MMSOP_REP_SEMPOOL	24 /* report semaphore pool status 	*/
#define MMSOP_REP_SEMENTRY	25 /* report semaphore entry status	*/

#define MMSOP_INIT_DOWNLOAD	26 /* initiate download sequence	*/
#define MMSOP_DOWN_LOAD		27 /* download segment			*/
#define MMSOP_TERM_DOWNLOAD	28 /* terminate download sequence	*/
#define MMSOP_INIT_UPLOAD	29 /* initiate upload sequence		*/
#define MMSOP_UP_LOAD 		30 /* upload segment			*/
#define MMSOP_TERM_UPLOAD	31 /* terminate upload sequence		*/
#define MMSOP_REQ_DOM_DOWN	32 /* request domain download		*/
#define MMSOP_REQ_DOM_UPL	33 /* request domain upload		*/
#define MMSOP_LOAD_DOMAIN	34 /* load domain content		*/
#define MMSOP_STORE_DOMAIN	35 /* store domain content 		*/
#define MMSOP_DELETE_DOMAIN	36 /* delete domain			*/
#define MMSOP_GET_DOM_ATTR	37 /* get domain attribute 		*/

#define MMSOP_CREATE_PI		38 /* create program invocation		*/
#define MMSOP_DELETE_PI		39 /* delete program invocation		*/
#define MMSOP_START		40 /* start				*/
#define MMSOP_STOP		41 /* stop 				*/
#define MMSOP_RESUME		42 /* resume				*/
#define MMSOP_RESET 		43 /* reset				*/
#define MMSOP_KILL		44 /* kill 				*/
#define MMSOP_GET_PI_ATTR	45 /* get program invocation attribute	*/

#define MMSOP_OBTAIN_FILE	46 /* obtain file			*/

#define MMSOP_DEF_EC		47 /* define event condition		*/
#define MMSOP_DEL_EC		48 /* delete event condition 		*/
#define MMSOP_GET_EC_ATTR	49 /* get event condition attributes	*/
#define MMSOP_REP_EC_STAT	50 /* report event condition status	*/
#define MMSOP_ALT_EC_MON	51 /* alter event condition monitoring	*/
#define MMSOP_TRIGGER_EV	52 /* trigger event 			*/
#define MMSOP_DEF_EA		53 /* define event action status	*/
#define MMSOP_DEL_EA		54 /* delete event action		*/
#define MMSOP_GET_EA_ATTR	55 /* get event action attributes	*/
#define MMSOP_REP_EA_STAT	56 /* report event action status	*/
#define MMSOP_DEF_EE		57 /* define event enrollment		*/
#define MMSOP_DEL_EE		58 /* delete event enrollment		*/
#define MMSOP_ALT_EE		59 /* alter event enrollment		*/
#define MMSOP_REP_EE_STAT	60 /* report event enrollment status	*/
#define MMSOP_GET_EE_ATTR	61 /* get event enrollment attributes	*/
#define MMSOP_ACK_EVENT_NOT	62 /* acknowledge event notification	*/
#define MMSOP_GET_ALARM_SUM	63 /* get alarm summary			*/
#define MMSOP_GET_ALARM_ESUM	64 /* get alarm enrollment summary	*/

#define MMSOP_READ_JOURNAL	65 /* read journal 			*/
#define MMSOP_WRITE_JOURNAL	66 /* write journal			*/
#define MMSOP_INIT_JOURNAL	67 /* initialize journal		*/
#define MMSOP_STAT_JOURNAL	68 /* report journal status		*/
#define MMSOP_CREATE_JOURNAL	69 /* create journal 			*/
#define MMSOP_DELETE_JOURNAL	70 /* delete journal 			*/

#define MMSOP_GET_CAP_LIST	71 /* get capability list 		*/

#define MMSOP_FILE_OPEN		72 /* file open				*/
#define MMSOP_FILE_READ		73 /* file read				*/
#define MMSOP_FILE_CLOSE	74 /* file close			*/
#define MMSOP_FILE_RENAME	75 /* file rename			*/
#define MMSOP_FILE_DELETE	76 /* file delete			*/
#define MMSOP_FILE_DIR		77 /* file directory			*/


/************************************************************************/
/* The opcodes below are reserved for the unconfirmed services and are	*/
/* NOT used on the wire.						*/
/************************************************************************/
#define MMSOP_USTATUS 		78 /* unsolicited status, ucs 1		*/
#define MMSOP_INFO_RPT		79 /* information report, ucs 0		*/
#define MMSOP_EVENT_NOT		80 /* unsolicited status, ucs 2		*/


/************************************************************************/
/* These are not really opcodes at all, but simply represent the bit	*/
/* positions in the services supported bitstring			*/

#define _MMSOP_ATTACH_TO_EC	81 /* Attach to event condition		*/
#define _MMSOP_ATTACH_TO_SEM	82 /* Attach to semaphore		*/

/************************************************************************/
/* These opcodes are reserved for Initiate, Conclude, etc. and are	*/
/* NOT used on the wire, since these operations do not take op codes.	*/
/************************************************************************/

#define MMSOP_CONCLUDE		83 /* conclude				*/
#define MMSOP_CANCEL		84 /* cancel				*/


/************************************************************************/
/* MVLU Internal Use */
#define MMSOP_MVLU_RPT_VA	200
#define MMSOP_MVLU_GOOSE_UPDATE	201
#define MMSOP_RD_USR_HANDLED	202

/************************************************************************/
/* The opcodes below are NOT used on the wire, only for completed	*/
/* 'Macro' function complete flags.                                     */
/************************************************************************/
#define MMSOP_INITIATE		85      /* initiate operation		*/

#define MMSOP_NAMED_READ	90	/* Named Read			*/
#define MMSOP_NAMED_WRITE	91	/* Named Write			*/
#define MMSOP_REM_FILE_OPEN	93	/* Remote File Open		*/
#define MMSOP_REM_FILE_READ	94	/* Remote File Read		*/
#define MMSOP_REM_FILE_CLOSE	95	/* Remote File Close		*/
#define MMSOP_INIT_CONN	        96 	/* Initiate			*/
#define MMSOP_REM_FILE_COPY	98	/* Remote file copy		*/
#define MMSOP_MV_DOWNLOAD       99	/* VM Domain Download		*/
#define MMSOP_MV_UPLOAD         100	/* VM Domain Upload		*/
#define MMSOP_VM_VAR_READ       101	/* Var Read			*/
#define MMSOP_VM_VAR_WRITE      102	/* Var Write			*/

/************************************************************************/
/************************************************************************/
/* Various MMS limitations, parameters, etc.				*/
/************************************************************************/

#define SERVICE_RESP_SIZE 11    /* BYTE size of service resp array      */

#define MAX_FILES_OPEN 100	/* maximum files open locally/remotely	*/
#define MAX_FILE_NAME  255	/* maximum length of a file pathname	*/
#define MAX_IDENT_LEN	128	/* length of an Identifier variable	*/
#define MAX_AR_LEN	64	/* length of a AR name			*/

/************************************************************************/
/************************************************************************/
/* This structure is used to support modifier handling.			*/
/************************************************************************/

struct list_of_mods
  {
  ST_BOOLEAN      info_pres;	/* must be set to get info encoded	*/
  ST_INT          num_of_mods;	/* tells how many mods in the list	*/
  struct modifier *mod_list_ptr;/* ptr to array of contiguous modifiers	*/
  };
typedef struct list_of_mods LIST_OF_MODS;

extern struct list_of_mods modifier_list;
extern ST_INT	m_max_mods;

/************************************************************************/
/************************************************************************/
/* COMPANION STANDARD HANDLING						*/
/************************************************************************/
/* This structure is used as a global input parameter to all MMS 	*/
/* requests and responses sent by the application. To send CS info, the	*/
/* 'cs_pres' flag must be set SD_TRUE, the other components refer to the*/
/* ASN.1 CS INFO to be sent. 'cs_pres' is reset after being used.	*/
/************************************************************************/


struct csi 
  {				
  ST_BOOLEAN cs_pres;		/* SD_TRUE if CS info was received	*/
  ST_INT     cs_len;		/* length of ASN.1			*/
  ST_UCHAR   *cs_ptr;		/* pointer to ASN.1			*/
  };
typedef struct csi  CSI ;

extern struct csi cs_send; 	/* SEND CS info structure		*/

/************************************************************************/
/************************************************************************/
/* These defines can be commented out to exclude support for some data	*/
/* types if they are not required.					*/

#ifndef MMS_LITE
#define FLOAT_DATA_SUPPORT
#define TIME_DATA_SUPPORT
#define BTOD_DATA_SUPPORT
#else
#define FLOAT_DATA_SUPPORT
#define TIME_DATA_SUPPORT 
#define BTOD_DATA_SUPPORT
#endif

/************************************************************************/
/************************************************************************/
/* These defines are the values that the domain state attribute 	*/
/* named_dom_ctrl.state can take on.					*/
/************************************************************************/

#define DOM_NON_EXISTENT  0
#define DOM_LOADING	  1
#define DOM_READY	  2
#define DOM_IN_USE	  3
#define DOM_COMPLETE	  4
#define DOM_INCOMPLETE	  5

#define DOM_D1		  7
#define DOM_D2		  8
#define DOM_D3		  9
#define DOM_D4		 10
#define DOM_D5		 11
#define DOM_D6		 12
#define DOM_D7		 13
#define DOM_D8		 14
#define DOM_D9		 15

#ifdef __cplusplus
}
#endif

#endif		/* MMS_DEF2_INCLUDED */


