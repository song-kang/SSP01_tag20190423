/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	1994 - 2011, All Rights Reserved		        */
/*									*/
/* MODULE NAME : mvl_defs.h   						*/
/* PRODUCT(S)  : MMSEASE-LITE						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*	NONE								*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 01/11/12  JRB	   Add mvl_vmd_nvl_add_aa function.		*/
/*			   MVL_NVLIST_CTRL: add altAccArray.		*/
/*			   mvl_var_create_derived & MVL_VAR_ASSOC: chg	*/
/*			   prim_num to ST_UINT to support bigger vars.	*/
/* 10/14/11  JRB	   Del unnecessary ICCP_LITE_SUPP define.	*/
/* 05/18/11  EJV           MVL_NET_INFO: chg loc_ar_sec,rem_ar_sec to	*/
/*			     ST_VOID *.					*/
/* 03/18/11  JRB	   MVLAS_WR_VA_CTRL: add is_61850_control.	*/
/*			   Add u_mvl_write_ind_custom.			*/
/* 12/08/10  JRB	   MVL_DOM_CTRL: add lln0_var.			*/
/* 07/08/10  EJV	   MVL_NET_INFO: add loc_mace, rem_mace,	*/
/*			     certExpirationLastCheckTime.		*/
/* 04/13/10  JRB	   Add decr_numpend_req.			*/
/* 03/02/10  JRB	   Del synchronous client functions,		*/
/*			   MVL_SYNC_REQ_INTERFACE, & u_mvl_check_timeout*/
/* 11/17/09  JRB    133    Chg acse_conn_id to (ST_VOID *).		*/
/*			   Replace S_SEC_LOC_AR & S_SEC_REM_AR with	*/
/*			   S_SEC_LOC_REM_AR.				*/
/* 01/27/09  JRB    132    MVLAS_WRITE_CTRL: add ind_funct_done.	*/
/* 09/25/08  MDE    131    Added MMS result info for READ/WRITE		*/
/* 07/23/08  JRB    130    Move defs from mvl_uca.h to here (need earlier).*/
/*			   Fix CntrlObj size.				*/
/* 06/24/08  MDE    129    Added _mvl_persist_aa_objs			*/
/* 06/04/08  JRB    128    Add file_list to MVL_NET_INFO.		*/
/*			   Add mplas_concl_err.				*/
/*			   Del mvl_type_ctrl from typedef (global var	*/
/*			   with same name confused some debuggers).	*/
/* 04/08/08  JRB    127    Chg () to (ST_VOID) in 2 prototypes.		*/
/* 02/26/08  JRB    126    Add mvl_var_add_alloc, mvl_var_create_alloc.	*/
/*			   Add mvl_var_create_derived.			*/
/*			   mvl_dom_destroy: chg ret to void (can't fail)*/
/*			   Add _mvl_clear_aa_objs.			*/
/* 11/12/07  MDE    125    Added _mvl_getpath, changed _mvl_tmpnam	*/
/* 04/16/07  JRB    124    Add prim_num to MVL_VAR_ASSOC.		*/
/* 03/19/07  JRB    123    Del obsolete "mvl_vmd_*" macros.		*/
/* 12/04/06  JRB    122    mvl_vmd_destroy: add arg to specify which VMD.*/
/* 11/21/06  JRB    121    Move ..LAST_APPL_ERROR struct def to here	*/
/*			   and add it to MVLAS_WR_VA_CTRL.		*/
/* 10/30/06  JRB    120    Del #ifdef in structs so they do not	change	*/
/*			    with compiler options.			*/
/*			   Del MVL_DYN_ASN1_TYPES option, always use	*/
/*			    dynamic ASN1. Del asn1 from MVL_TYPE_CTRL.	*/
/* 10/30/06  JRB    119    New object handling functions take VMD arg	*/
/*			    & have "mvl_vmd_" prefix. Macros provided	*/
/*			    for backward compatibility only.		*/
/*			   mvl_nvl_create: change args.			*/
/*			   Add "mvl_vmd_type_id_*".			*/
/*			   Add mvl_vmd_create,				*/
/*			    mvlu_rpt_ctrl_destroy_all,			*/
/*			    mvl61850_rpt_ctrl_destroy_all.		*/
/*			   u_mvl_get_va_aa: add vmd_ctrl arg.		*/
/*			   u_gnl_ind_*: add net_info arg.		*/
/*			   _mvl_objname_to_va: add vmd_ctrl, net_info args*/
/*			   Del global var _mvl_curr_net_info.		*/
/*			   Del global var _mvl_curr_usr_ind_ctrl.	*/
/* 10/17/06  MDE    118    Added mvlas_obtfile_resp_ex			*/
/* 07/21/06  JRB    117    Add rpt_reason & type_ctrl to MVL_VAR_ASSOC.	*/
/*			   Add already_scanned to MVL_NVLIST_CTRL.	*/
/* 07/13/05  MDE    116    Added u_mvl_reject_ind			*/
/* 06/29/05  CRM    115    Add mvl_vmd_destroy.				*/
/* 06/27/05  JRB    114    MVL_VAR_ASSOC: del mvl_internal, add last_data.*/
/* 06/27/05  CRM    113    Added mvl_dom_find_last and			*/
/*			   mvl_type_id_destroy_all_dyn.			*/
/* 05/04/05  MDE    112    Added mvl_init_ind_hold			*/
/* 03/28/05  JRB    111    Del suicacse header.				*/
/* 03/14/05  CRM    110    Add mvl_max_dyn global variable.		*/
/*			   Del _mvlu_num_dyn_types, add backward compat def*/
/* 03/08/05  CRM    109    Add mvlas_fdir_resp(MVL_IND_PEND *ind)	*/
/* 03/07/05  MDE    108    Added startTime to stats			*/
/* 09/20/04  JRB    107    Add comments.				*/
/* 08/30/04  JRB    106    Add sboCtrl to MVLAS_WR_VA_CTRL.		*/
/* 06/29/04  JRB    105    Add mvl_type_id_create_from_tdl proto.	*/
/*			   Add use_static_data flag to MVL_VAR_ASSOC.	*/
/*			   Increase MAX_JREAD_RESP_ENTRIES define.	*/
/* 07/10/03  EJV    104    Chg loc_ar_sec/rem_ar_sec to (S_SEC_LOC_AR *)*/
/* 04/11/03  MDE    103    Added security elements			*/
/* 04/11/03  MDE    102    Added _mvl_tmpnam				*/
/* 04/04/03  JRB    101    MVL_IND_PEND: Add scan_va_done_fun for	*/
/*			   IEC/UCA integrity scan.			*/
/* 03/13/03  JRB    100    mvl_type_id_create: Add type_name arg.	*/
/*			   MVL_TYPE_CTRL: Chg type_name to array.	*/
/*			   MVL_TYPE_CTRL: Add rt_ctrl for dynamic types.*/
/*			   Add mvl_type_ctrl_find_dyn.			*/
/*			   Add mvl_scope_set proto.			*/
/*			   Del mvlu_add_rt_type_x (not needed).		*/
/* 12/12/02  JRB     99    Add usr_resp_fun to MVL_IND_PEND		*/
/*			   Add mvl_internal to MVL_VAR_ASSOC		*/
/*			   Chg acse_conn_id to ST_LONG to match ACSE	*/
/*			   Add net_info arg to mvl_get_va_asn1_data	*/
/*			   Add mvl_encode_info_rpt proto		*/
/*			   Add enc_buf allocation macros.		*/
/* 12/10/02  JRB     98    Moved UCA defs to new mvl_uca.h		*/
/* 12/09/02  MDE     96    Added mvlu_find_uca_var			*/
/* 12/09/02  MDE     96    Changed SBO reference handling		*/
/* 11/27/02  MDE     95    Addded runtime leaf parameter manipulation	*/
/* 			   Added type_name to MVL_TYPE_CTRL		*/
/*     		   	   Added mvlu_add_rt_type_x			*/
/*     		   	   Added mvlu_proc_rt_type			*/
/* 07/17/02  JRB     94    Del _mvla_send_write.. proto. Use static fct.*/
/* 07/09/02  JRB     93    Add mvl_(u)req_bufs_avail functions.		*/
/* 07/09/02  MDE     92    Add maxpend_ind support			*/
/* 05/13/02  MDE     91    Added 'skip' arg to _mvl_set_net_req_done 	*/
/* 03/25/02  MDE     90	   Moved error/reject elements outside union	*/
/* 02/27/02  JRB     89    Fix mvl_print... proto.			*/
/* 02/25/02  MDE     88    Deleted mvl_num_call* vars			*/
/* 01/29/02  JRB     87    Del use of MVL_NUM_CALL* & COACSE.		*/
/*			   Del a_unit_data_info,stack_sel from MVL_NET_INFO.*/
/* 01/23/02  EJV     86	   Added journals to MVL_DOM_CTRL		*/
/*			     mvl_init_dom -   added max_jou param	*/
/*			     mvl_dom_resize - added max_jou param	*/
/*			     mvl_dom_create - added max_num_jou param	*/
/*			     mvl_dom_add -    added max_num_jou param	*/
/* 12/20/01  HSF     85	   Added usr field to MVLU_RPT_SCAN_CTRL	*/
/* 12/12/01  MDE     84    Moved acse_conn_id outside ifdef		*/
/* 11/31/01  MDE     83    Added GOOSE scan support			*/
/* 11/01/01  JRB     82    Chg mvl_wait_req_done return to ST_RET.	*/
/*			   Add wait_any_event proto (user must supply).	*/
/* 07/31/01  MDE     81    Added mvl_conn_filtered_logging support	*/
/* 03/19/01  JRB     80    Move SMEM context defs to "smem.h".		*/
/* 03/15/01  JRB     79    Add clientUstatus, serverUstatus statistics.	*/
/* 03/13/01  JRB     78    Add mvl_ustatus, _mvl_process_ustatus_ind,	*/
/*			   u_mvl_ustatus_ind prototypes.		*/
/* 01/19/00  MDE     77    Removed SMEM idx defines (now in mem_chk.h)	*/
/* 01/05/01  MDE     76    Changes for use over Marben          	*/
/* 10/25/00  JRB     75    Del u_mvl*, u_gnl* & _ml_* funct ptrs.	*/
/*			   Add u_mvl_concl_ind, mplas_concl_resp.	*/
/*			   Add req_pend_list to mvl_net_info.		*/
/*			   Del usrNvl from MVLAS_READ_CTRL (not needed).*/
/*			   Add "Server" support for Upl/Downl.		*/
/* 10/06/00  MDE     74    Added _mvlu_num_dyn_types			*/
/* 09/05/00  JRB     73    Add mvl_get_runtime_dyn.			*/
/* 08/18/00  JRB     72    Fixed mvl_type_id_cre.. proto.		*/
/* 08/18/00  RKR     71    Added rt field to MVLU_ typedefs 		*/
/* 07/26/00  MDE     70    Added user field to MVL_FGET_REQ_INFO	*/
/* 07/12/00  JRB     69    Add dynamic type creation prototypes.	*/
/* 07/11/00  MDE     68    Changed seqNum to sqNum, made INT8U		*/
/* 06/26/00  JRB     67    Move "#define COACSE" from mvl_acse.h to	*/
/*			     here (avoids dependence on mvl_acse.h).	*/
/* 06/12/00  GLB     66    Added info for asynchronous File Get		*/
/* 06/01/00  RKR     65    Removed some prototypes redundant in MMS-EASE*/
/* 05/01/00  JRB     64    Del mvl_init_type_ctrl proto; now it's static*/
/* 04/14/00  JRB     63    Fix SD_CONST for ??IndFunTbl, mvl_get_run...	*/
/*			   Delete remnants of MVL_REQ_BUF_ENABLED.	*/
/* 04/12/00  MDE     62    Added MVL_STATISTICS, _mvl_resize_ptr_tbl	*/
/* 04/05/00  RKR     61    Made MVL_XNAME a compile time option		*/
/* 04/04/00  RKR     60    Added expanded UCA variable name to RD and WR*/
/* 03/10/00  JRB     59    Add scan_va_scope to MVLU_RPT_SCAN_CTRL.	*/
/* 03/07/00  MDE     58    Added type_id to string declarations		*/
/* 01/21/00  MDE     57    Now use MEM_SMEM for dynamic memory		*/
/* 10/20/99  NAV     56    Add maxpend_req support			*/
/* 09/28/99  NAV     55    Modified UCA Report REASON Bits		*/
/* 09/13/99  MDE     54    Added SD_CONST modifiers			*/
/* 11/04/99  RKR     53    Removed action_required from MVL_NET_INFO	*/
/* 11/03/99  RKR     52    Added MVL error constants			*/
/* 10/28/99  RKR     51    Added pending indications to MVL_NET_INFO	*/
/* 10/07/99  GLB     50    Added support for file rename & obtain file.	*/
/* 10/05/99  RKR     49    Added prototypes for deriving types		*/
/* 09/30/99  JRB     48    Added forward declaration to reduce warnings.*/
/* 09/30/99  RKR     47    Changed the Reason for Inclusion bit strings */
/* 09/24/99  JRB     46    Added extern to mvlu_rpt_scan_list		*/
/* 09/13/99  MDE     45    Added SD_CONST modifiers			*/
/* 09/07/99  MDE     44    Revised and enhanced the UCA report system	*/
/* 09/07/99  MDE     43    Changed MVL_VA_SCOPE to MVL_SCOPE, added	*/
/*			   scope to MVL_NVLIST_CTRL			*/
/* 07/30/99  MDE     42    Removed unused defines			*/
/* 04/01/99  MDE     41    Changes to decode buffer allocation scheme   */
/* 03/18/99  MDE     40    Enhanced dynamic object facilities		*/
/* 03/18/99  MDE     39    Changes to support runtime connection limits */
/* 03/09/99  MDE     38    Added ICCP Lite support			*/
/* 01/22/99  JRB     37    Chg all objects to array of ptrs so bsearch	*/
/*			   can be used. Add bsearch functs (mvl_obj_*).	*/
/*			   Use "mvl_nvl" prefix on all NVL obj functs.	*/
/*			   Add "flags" to MVL_VAR_ASSOC for UCA.	*/
/* 12/08/98  MDE     36    Added client alternate access support	*/
/* 12/08/98  MDE     35    Added dynamic object handling		*/
/* 11/17/98  MDE     34    Added _mvl_destroy_nvl_entries		*/
/* 11/16/98  MDE     33    Cleanup and rearrange for readability	*/
/* 11/16/98  MDE     32    Added 'locl_init_info' to MVL_NET_CTRL'	*/
/* 11/16/98  MDE     30    Renamed MVL interal functions ('_'prefix)	*/
/* 11/16/98  MDE     29    Added mvl_concl, mvla_concl			*/
/* 11/16/98  MDE     28    Removed 'wait_any_event', 'read_log_cfg_file'*/
/* 11/13/98  JRB     27    Add wait_any_event proto.			*/
/* 10/12/98  MDE     26    Merged JRB's and MDE's changes		*/
/* 10/09/98  JRB     25    Chg MVL_JOURNAL_ENTRY to use VAR_INFO.	*/
/*			   Del MVL_CLI_JOURNAL_ENTRY, use only		*/
/*			   MVL_JOURNAL_ENTRY everywhere.		*/
/*			   Add 'read_log_cfg_file' prototype.		*/
/* 10/09/98  DWL     24    Added get_dom stuff				*/
/*			   Added get_nam stuff				*/
/*			   Added status stuff				*/
/* 10/09/98  DWL     23    Added get_var stuff				*/
/* 10/01/98  MDE     22    Changed 'SD_CONST' define 			*/
/* 10/01/98  MDE     21    Changed rcb to ptr, removed MVL_NUM_CALLED	*/
/* 10/01/98  MDE     20    Changed MVL_JOURNAL_ENTRY entry_id to 	*/
/*			   ST_UCHAR[8]					*/
/* 09/21/98  MDE     19    Minor lint cleanup				*/
/* 09/16/98  MDE     18    Fixed possible UCA report 'reason' overwrite	*/
/*			   Added 'mvlu_rpt_ctrl_ptrs[MVLU_NUM_RPT_CTRL]'*/
/*			   Added SBO support				*/
/* 09/10/98  MDE     17    Added 'SD_CONST' declaration			*/
/* 08/13/98  MDE     16    Removed USE_RT_DATA stuff			*/
/* 08/11/98  MDE     15    Added UCA variable array support		*/
/* 08/10/98  MDE     14    Changes to UCA report handling		*/
/* 07/28/98  MDE     13    Added USE_RT_DATA define, related changes	*/
/* 07/16/98  MDE     12    More journal work				*/
/* 07/15/98  MDE     11    Changed Journal Read user functions		*/
/* 07/13/98  MDE     10    Report and Journal work			*/
/* 06/30/98  MDE     09    Added more flexable CLient Info Rpt code	*/
/* 06/22/98  MDE     08    SeqNum now ST_UINT16				*/
/* 06/15/98  MDE     07    Changes to allow compile under C++		*/
/* 05/14/98  JRB     06    Add support for more services.		*/
/*			   Add u_mvl_read_ind, u_mvl_write_ind, and	*/
/*			     delete corresponding function ptrs.	*/
/*			   Del mvl_ident_resp_info, mvl_status_resp_info*/
/*			   Chg "Conf" function arg to (MVL_REQ_PEND *).	*/
/*			   Chg "Ind" function arg to (MVL_IND_PEND *).	*/
/*			   Add (MVL_REQ_PEND**) to sync client functs.	*/
/* 03/15/98  MDE     05    Added support for user loaded Runtime Types,	*/
/*			   dynamic NVL's				*/
/* 02/10/98  MDE     04    Always support AA, added 'index' to conn	*/
/*			   control, added UCA support, more.		*/
/* 12/22/97  MDE     03    Changed '_mvl_net_service', 'mvl_comm_serv'	*/
/* 12/29/97  MDE     02    Added Journal stuff, general cleanup		*/
/* 09/10/97  MDE     01    MMS-LITE V4.0 Release			*/
/************************************************************************/

#ifndef MVL_DEFS_INCLUDED
#define MVL_DEFS_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "mmsdefs.h"
#include "gen_list.h"

#include "acse2usr.h"

/************************************************************************/
/* COMPILE TIME OPTIONS							*/

/* These values are used in the MVL object initialization code to allow	*/
/* the user to dynamically add additional objects.			*/

#define MVL_NUM_DYN_DOMS	10

#if !defined(MVL_NUM_DYN_VMD_VARS)
#define MVL_NUM_DYN_VMD_VARS	0
#endif

#define MVL_NUM_DYN_VMD_NVLS	10
#define MVL_NUM_DYN_JOUS	10
#define MVL_NUM_DYN_DOM_VARS	10
#define MVL_NUM_DYN_DOM_NVLS	10
#define MVL_NUM_DYN_AA_VARS	10
#define MVL_NUM_DYN_AA_NVLS	10

/* MVL_UCA requires dynamic types to function */
#define MVLU_NUM_DYN_TYPES	100 

typedef struct
  {
  ST_INT aa_nvls;
  ST_INT aa_vars;
  ST_INT doms;
  ST_INT dom_nvls;
  ST_INT dom_vars;
  ST_INT journals;
  ST_INT types;
  ST_INT vmd_nvls;
  ST_INT vmd_vars;
  } MVL_MAX_DYN; 

extern MVL_MAX_DYN mvl_max_dyn;

/* OBSOLETE: For backward compatibility only	*/
#define _mvlu_num_dyn_types      mvl_max_dyn.types

/* Comment this out to use statically allocated buffers			*/
/* Note that dynamic buffers are required for async server responses.	*/
#define MVL_DYN_MEM 

/* This define can be uncomented in order to use binary search for 	*/
/* MMS-LITE object searches (variables, named variable lists, etc.).	*/ 
/* #define MVL_USE_BSEARCH */

/* Define this to support receiving Information Reports			*/
#define MVL_INFO_RPT_CLIENT

/* Uncomment this to enable support for described access as server	*/
/* #define MVL_DESCR_SUPP */ /* Warning, not currently supported	*/

/* Uncomment this to enable support for expanded UCA variable names	*/
/* This will save some processing and a small bit of code.		*/
/* #define MVL_XNAME  */

/* Comment this to disable Alternate Access support for as server	*/
/* This will save a small bit of code.					*/
#define MVL_AA_SUPP 

/************************************************************************/
/* Miscellaneous defines.						*/
/************************************************************************/
/* Max size of some IEC 61850 objects.	*/
#define MVL61850_MAX_OBJREF_LEN	129	/* Value specified by Tissue 141*/
#define MVL61850_MAX_RPTID_LEN	65

/************************************************************************/
/* MMS OBJECT INITIALIZATION						*/
/* This function must be called to initialize all MVL types and other	*/
/* MMS objects.								*/

ST_VOID mvl_init (ST_VOID);
ST_VOID mvl_init_mms_objs (ST_VOID);

/* COMMUNICATION SERVICE						*/
/* This is the high level function to be called periodicly by the 	*/
/* application. It will check for communications events and act on them,*/
/* which will include decoding MMS PDU's and calling service functions.	*/
/* The mechanism used to determine when this function should be called	*/
/* will depend on the lower layer service provider.			*/

ST_BOOLEAN mvl_comm_serve (ST_VOID);

/* For use when global variables cannot be initialized at compile time	*/
#if defined(NO_GLB_VAR_INIT)
ST_VOID mvl_init_glb_vars (ST_VOID);

/* Internal use only */
ST_VOID _mvlop_en_init (ST_VOID);
ST_VOID _s_getnam_init_glb_vars (ST_VOID);
#endif

/* If set == SD_TRUE, MVL will not delete AA objects when connection 	*/
/* terminates 								*/
extern ST_BOOLEAN _mvl_persist_aa_objs;

/************************************************************************/
/* CAPACITY/STACK USAGE CONTROL DEFINES					*/
/* These defines work along with the MMS PDU buffer sizes to control	*/
/* the maximum number of MMS objects and the size of the objects that	*/
/* can be handled per MMS PDU.						*/

#define MAX_JREAD_RESP_ENTRIES	20
#define MAX_JREAD_RESP_VARS	100

#define MVL_MAX_GNL_NAMES	100

/************************************************************************/
/* MVL ERROR CODES							*/

/* #define E_MMS_LITE	0x6A00 */
#define MVL_ERR_USR_TIMEOUT           0x6A01
#define MVL_ERR_REQ_CONTROL           0x6A02
#define MVL_ERR_UNKNOWN_PDU_TYPE      0x6A03
#define MVL_ERR_RUNTIME_TYPE_ID       0x6A04
#define MVL_ERR_ASN1_TO_RUNTIME       0x6A05
#define MVL_ERR_NOT_SYM_ADDR	      0x6A06
#define MVL_ERR_ARRAY_ELEMENT_CNT     0x6A07
#define MVL_ERR_LOCAL_ADDRESS	      0x6A08
#define MVL_ERR_BUFFER_SIZE  	      0x6A09
#define MVL_ERR_DOM_CONTROL   	      0x6A0A
#define MVL_ERR_AA_CONTROL   	      0x6A0B
#define MVL_ERR_AA_SPECIFIC   	      0x6A0C
#define MVL_ERR_NVL_NOT_FOUND  	      0x6A0D
#define MVL_ERR_ALT_ACCESS  	      0x6A0E
#define MVL_ERR_VA_NOT_FOUND  	      0x6A0F
#define MVL_ERR_VA_SPEC		      0x6A10
#define MVL_ERR_NO_REQ_CTRL 	      0x6A11
#define MVL_ERR_NO_CONN_CTRL	      0x6A12
#define MVL_ERR_ASSOC_REQ	      0x6A13
#define MVL_ERR_COMM_SERVE_ACTIVE     0x6A14
#define MVL_ERR_REQ_PEND_COUNT	      0x6A15
#define MVL_ERR_CNF_REJ_ERR           0x6A16
#define MVL_ERR_CNF_ERR_OK	      0x6A17
#define MVL_ERR_CNF_DISCONNECTED      0x6A18
#define MVL_ERR_BAD_TYPE	      0x6A19
#define MVL_ERR_RESOURCE_NOT_AVAIL    0x6A1A

/************************************************************************/
/* Structure used to create linked list of all open files for one	*/
/* connection (see connection structure MVL_NET_INFO).			*/
typedef struct mvl_net_file
  {
  /* CRITICAL: First 2 parameters used to add this struct to linked	*/
  /* lists using list_add_last, etc.					*/
  struct mvl_net_file *next;		/* CRITICAL: DON'T MOVE.	*/
  struct mvl_net_file *prev;		/* CRITICAL: DON'T MOVE.	*/
  FILE *fp;				/* file pointer from fopen	*/
  } MVL_NET_FILE;
  
/************************************************************************/
/************************************************************************/
/* NETWORK CONNECTION CONTROL						*/
/* MVL_NET_INFO is used to specify the remote node to which the PDU is  */
/* to be sent, or from which the PDU was received. This element and 	*/
/* it's use are implementation specific.				*/

struct mvl_ind_pend;	/* Forward declare this to eliminate warnings.	*/
struct mvl_req_pend;	/* Forward declare this to eliminate warnings.	*/

typedef struct mvl_net_info
  {
  struct mvl_aa_obj_ctrl *aa_objs; /* AA object ctrl			*/
  struct mvl_vmd_ctrl    *rem_vmd; /* Remote VMD			*/
  struct mvl_ind_pend	 *pend_ind;
  struct mvl_req_pend	 *req_pend_list;	/* link list of pending reqs*/

  ST_BOOLEAN conn_active; 	/* Set SD_TRUE when the connection is up	*/
  ST_INT max_pdu_size;
  ST_INT index;			/* NET_INFO table index for this elmnt	*/

  ST_INT	      maxpend_req;    /* num outstanding reqs negotiated  */
  ST_INT	      numpend_req;    /* num reqs currently outstanding	  */

  ST_INT	      maxpend_ind;    /* num outstanding inds negotiated  */
  ST_INT	      numpend_ind;    /* num inds currently outstanding	  */

  ST_BOOLEAN log_enable;	/* For use w/mvl_conn_filtered_logging	*/
  
  ST_VOID *acse_conn_id;	/* ACSE's connection ID, internal	*/

  ST_BOOLEAN mi_in_use;		/* used only for ICCP-LITE product	*/
  struct _mi_conn *mi_conn;	/* used only for ICCP-LITE product	*/

  INIT_INFO rem_init_info;	/* Services supported by remote device	*/
  INIT_INFO locl_init_info;	/* Initiate info we sent 		*/
  AARQ_APDU ass_ind_info;	
				/* Items below are used by MVL only	*/
  ST_BOOLEAN in_use;		/* Flag that this 'NET_INFO' is in use	*/
  struct net_bind_ctrl *bind_ctrl;

  ST_VOID      *loc_ar_sec;
  ST_VOID      *rem_ar_sec;

  S_CERT_CTRL      *loc_mace;	/* loc MACE cert used in this connection*/
  S_CERT_CTRL      *rem_mace;	/* rem MACE cert used in this connection*/
  ST_DOUBLE         certExpirationLastCheckTime;

  ST_VOID *user_info;		/* MVL user can use this for 'whatever'	*/
  MVL_NET_FILE *file_list;	/* linked list of open files on this conn*/
  } MVL_NET_INFO;

/* Our connection control structures */
extern MVL_NET_INFO *mvl_calling_conn_ctrl;
extern MVL_NET_INFO *mvl_called_conn_ctrl;

/************************************************************************/
/* MMS COMMUNICATIONS EVENT						*/
/* MMS_COMM_EVENT is the data structure used to pass a communication	*/
/* event from the lower layer provider to the MVL communication service	*/
/* subsystem. This structure may be expanded to fit the needs of the	*/
/* application.								*/

#define MMS_MSG_RCVD      	1
#define RECEIVE_ERROR     	2
#define COMM_LINK_UP      	3
#define COMM_LINK_DOWN       	4
#define MVLU_RPT_COMM_EVENT  	5

typedef struct mvl_comm_event
  {
  DBL_LNK l;			/* For linked list			*/
  MVL_NET_INFO *net_info;
  ST_INT event_type;
  union
    {
    struct       /* used for MMS_MSG_RCVD event types			*/
      {
      ST_UCHAR *pdu;
      ST_INT pdu_len;
      MMSDEC_INFO dec_rslt;
      } mms;
    struct      /* possibly add more structs to handle other event types*/
      {
      ST_RET code;
      } error;
    }u;
  } MVL_COMM_EVENT;

/************************************************************************/
/************************************************************************/
/* MVL TYPE HANDLING */
/* Runtime type handling functions and structures			*/
/* Note that the source code to create this table can be generated	*/
/* using the 'mktypes' utility						*/

typedef struct
  {
  RUNTIME_CTRL *rt_ctrl;	/* For dyn types ONLY, otherwise = NULL	*/
  ST_INT num_rt;		/* Runtime type table			*/
  SD_CONST RUNTIME_TYPE *rt;
  ST_INT data_size;             /* size of data element			*/
  ST_CHAR type_name [MAX_IDENT_LEN+1];		/* name passed to Foundry (1st after :T)*/
  struct mvl_vmd_ctrl *vmd_ctrl;	/* ptr to VMD which uses this type*/
  } MVL_TYPE_CTRL;

/* These variables can be provided via the source code generated by 	*/
/* the 'foundry' utility program					*/
extern MVL_TYPE_CTRL *mvl_type_ctrl;
extern ST_INT mvl_num_types;

/* This function takes the type ID and provides a pointer to the runtime*/
/* type and it's size as output. It returns SD_SUCCESS for success.	*/
/* The type information is currently stored internally as ASN.1 encoded */
/* types.								*/

ST_RET mvl_get_runtime (ST_INT type_id, 
                       RUNTIME_TYPE **rt_ptr_out,
                       ST_INT *num_rt_out);

/* Same function but only succeeds if type was dynamically created.	*/
ST_RET mvl_get_runtime_dyn (ST_INT type_id, 
                       RUNTIME_TYPE **rt_ptr_out,
                       ST_INT *num_rt_out);



/* If USR_SUPPLIED_RT is defined (and not USE_RT_DATA) then the user	*/
/* will supply the runtime table data structures using local means.	*/
ST_VOID u_mvl_start_init_rt_tbl (ST_INT numTypes, ST_INT totalNumRt);
ST_VOID u_mvl_end_init_rt_tbl (ST_VOID);
RUNTIME_TYPE *u_mvl_get_rt_tbl (ST_INT typeId, ST_INT numRt);

/* global variables in Foundry Object Realization File */
extern SD_CONST RUNTIME_TYPE * SD_CONST mvl_rt_tables[];
extern ST_INT rt_table_index;

/************************************************************************/
/* This function can be used to modify the number of elements		*/
/* in an array runtime type.						*/

ST_VOID mvl_mod_arr_size (RUNTIME_TYPE *rt, ST_RTINT num_elmnts);

/* This function converts an ASN.1 type definition to a runtime type.	*/
/* The runtime type is allocated by chk_calloc and the user must free	*/
/* the runtime type when done with it (using chk_free).			*/

ST_RET mvl_mk_rt_from_asn1 (ST_UCHAR *asn1ptr, ST_INT asn1len, 
                      RUNTIME_TYPE **rt_out, ST_INT *num_rt_out);

/************************************************************************/
/************************************************************************/
/* MMS OBJECT CONTROL ELEMENTS						*/
/* Functions and structures used to map MMS objects to local elements	*/
/************************************************************************/
/* VARIABLE ASSOCIATION							*/
/************************************************************************/

/* This structure is used in the Variable Association for use by 	*/
/* MVL_UCA in handling array alternate access				*/

typedef struct mvl_arr_ctrl
  {
  ST_BOOLEAN arrAltAccPres;
  ST_BOOLEAN nested;		/* Furter nesting is done on this arr	*/
  ST_RTINT low_index;
  ST_RTINT num_elmnts;
  ST_RTINT curr_index;
  ALT_ACCESS alt_acc;		/* Nested AA on the array		*/
  } MVL_ARR_CTRL;


/* MVL_VAR_PROC								*/
/* The functions selected by these pointers will be called during 	*/
/* read/write indication processing (NULL pointers are not called, of	*/
/* course!). The 'pre' funs must return SD_SUCCESS or SD_FAILURE, which	*/
/* will be reflected in the MMS response.				*/

/* Values for 'aa_mode_out' */
#define MVL_USE_NO_AA		1
#define MVL_USE_PACKED_AA	2
#define MVL_USE_UNPACKED_AA	3

struct mvl_var_assoc;	/* Forward declare this to eliminate warnings.	*/

typedef struct mvl_var_proc
  {
  ST_RET (*pre_read_aa)(struct mvl_var_assoc **va, 
			ST_BOOLEAN alt_acc_pres,
		      	ALT_ACCESS *alt_acc,
		      	MVL_NET_INFO *ni,
			ST_INT *aa_mode_out);
  ST_VOID (*post_read_aa)(struct mvl_var_assoc *va, 
			ST_BOOLEAN alt_acc_pres,
		      	ALT_ACCESS *alt_acc,
		      	MVL_NET_INFO *ni);

  ST_RET (*proc_write_aa)(struct mvl_var_assoc **va, 
		      	ALT_ACCESS *alt_acc,
		      	MVL_NET_INFO *ni,
			ST_INT *aa_mode_out);

  ST_RET (*pre_write_aa) (struct mvl_var_assoc *va, 
			ST_BOOLEAN alt_acc_pres,
		      	ALT_ACCESS *alt_acc,
			MVL_NET_INFO *ni, 
		      	ST_CHAR *write_data, 
			ST_INT write_data_size);
  ST_VOID (*post_write_aa)(struct mvl_var_assoc *va, 
			ST_BOOLEAN alt_acc_pres,
		      	ALT_ACCESS *alt_acc,
			MVL_NET_INFO *ni);

  ST_RET (*pre_info)  (struct mvl_var_assoc *va, MVL_NET_INFO *ni);
  ST_VOID  (*post_info) (struct mvl_var_assoc *va, MVL_NET_INFO *ni);
  } MVL_VAR_PROC;

#define MVL_VAR_FLAG_UCA	0x01	/* for "flags" of MVL_VAR_ASSOC	*/

/* MVL_VAR_ASSOC							*/
typedef struct mvl_var_assoc
  {
  ST_CHAR *name;                /* variable name			*/
  ST_VOID *data;                /* pointer to local data		*/
  ST_VOID *last_data;		/* to save last value of this variable	*/
				/* only allocated if va is in nvl	*/
				/* used by IEC/UCA rpt code.		*/
  ST_INT type_id;               /* type of variable			*/
  MVL_TYPE_CTRL *type_ctrl;	/* type of variable details		*/
				/* set by mvl_var_create		*/
  ST_UCHAR flags;		/* MVL_VAR_FLAG_UCA, etc.		*/
  MVL_VAR_PROC *proc;		/* User defined pre/post processing	*/
  ST_VOID *user_info;		/* MVL user can use this for 'whatever'	*/
  ST_VOID *usr_ind_ctrl;

  struct mvl_var_assoc *va_to_free; /* Used in NVL processing		*/

  struct mvl_var_assoc *base_va;    /* VA from which this was derived	*/
  ST_INT offset_from_base;	    /* Used only for static data buffer	*/
  ST_UINT prim_num;		/* Index to first prim in RUNTIME_TYPE	*/
				/* (used only when MVL_UCA defined)	*/
  ST_RTREF ref;
  MVL_ARR_CTRL arrCtrl;

  ST_BOOLEAN use_static_data;	/* "data" in this struct points to	*/
				/* permanent data.			*/
  ST_UCHAR rpt_reason;		/* 61850 RPT reason bitstring		*/
  ST_RET result;		/* Used only if MVL_INFO_RPT_CLIENT	*/
  } MVL_VAR_ASSOC;


ST_RET mvl_get_va_asn1_data (MVL_NET_INFO *net_info,
			 MVL_VAR_ASSOC *va,
			 ST_BOOLEAN alt_acc_pres,
			 ALT_ACCESS *alt_acc,
                         ST_UCHAR *asn1_dest_buffer,
                         ST_INT asn1_buffer_len,
                         ST_INT *asn1_len_out);


/************************************************************************/
/* DESCRIBED VARIABLE SUPPORT: Warning, not currently supported		*/

#if defined (MVL_DESCR_SUPP)
typedef struct mvl_descr_addr_assoc
  {
  ST_CHAR *base_name;                /* base part of address		*/
  ST_VOID *data;                     /* pointer to local data base		*/
  ST_INT data_size;                /* size of data element		*/
  ST_INT max_elmnts;
  ST_VOID *user_info;		/* MVL user can use this for 'whatever'	*/
  } MVL_DESCR_ADDR_ASSOC;

ST_RET mvl_get_descr_local_addr (VAR_ACC_ADDR *addr,
                            RUNTIME_TYPE *rt,
                            ST_INT mum_rt,
                            ST_CHAR **data_out,
                            ST_INT *data_size_out);

ST_RET mvl_get_descr_asn1_data (VARIABLE_LIST *vl,
                         ST_UCHAR *asn1_dest_buffer,
                         ST_INT asn1_buffer_len,
                         ST_INT *asn1_len_out);
#endif

/************************************************************************/
/* NAMED VARIABLE LISTS        						*/

typedef struct mvl_scope
  {
  ST_INT16 scope;
  struct mvl_dom_ctrl *dom;
  } MVL_SCOPE;

/* For backward compatibility only */
#define MVL_VA_SCOPE MVL_SCOPE

typedef struct mvl_nvlist_ctrl
  {
  ST_CHAR *name;                /* name of the named variable list	*/
  ST_INT num_of_entries;        /* number of variables in the list	*/
  MVL_VAR_ASSOC **entries;
  MVL_SCOPE nvl_scope;		/* scope of this NVL			*/
  MVL_SCOPE *va_scope;
  ALT_ACCESS **altAcc;		/* Alternate Access array for var's	*/
  /* NOTE: "altAccArray" is similar to "altAcc", but it is only used	*/
  /* to save Alternate Access info for GetNamedVariableListAttributes	*/
  /* response.								*/
  ALT_ACCESS **altAccArray;	/* Alternate Access array, 1 for each var*/
  ST_BOOLEAN mms_deletable;
  ST_VOID *user_info;		/* MVL user can use this for 'whatever'	*/
  ST_BOOLEAN already_scanned;	/* flag to indicate this NVL already	*/
				/* scanned for rpt changes. Avoids	*/
				/* scanning same data multiple times.	*/
  } MVL_NVLIST_CTRL;


/************************************************************************/
/* JOURNALS 								*/

typedef struct  mvlJouVar
  {
  ST_CHAR      *varTag;		/* variable tag 		*/
  MVL_VAR_ASSOC *va;	
  } MVL_JOU_VAR;

/* entry_form_tag value defines	are in mms_pjou.h			*/
/* #define JE_FORM_DATA 	2 */
/* #define JE_FORM_ANNOTATION 	3 */

/* cur_state value defines are in mms_pevn.h			*/
/* #define EC_STATE_DISABLED	0 */
/* #define EC_STATE_IDLE	1 */
/* #define EC_STATE_ACTIVE	2 */

typedef struct mvlJournalEntry
  {
  DBL_LNK l;				/* for user, not used by MVL	*/

  ST_INT   entry_id_len;		/* Octet string ID, size 1-8 	*/
  ST_UCHAR entry_id[8];

  APP_REF  orig_app;
  MMS_BTOD occur_time;			/* occurrence time		*/

  ST_INT16    entry_form_tag;		/* entry form tag		*/
  union
    {
    struct				/* entry form is DATA		*/
     {
      ST_BOOLEAN  event_pres;		/* event present		*/
      OBJECT_NAME evcon_name;		/* event condition name 	*/
      ST_INT16    cur_state;		/* current state		*/
      ST_BOOLEAN  list_of_var_pres;	/* list of variables present	*/
      ST_INT      num_of_var;		/* number of variables		*/
      VAR_INFO    *list_of_var;		/* ptr to array			*/
      } data;
    ST_CHAR	  *annotation;		/* pointer to annotation	*/
    }ef;
  } MVL_JOURNAL_ENTRY;

/* MVL Journal control structure. Used by MVL for GNL response		*/
typedef struct mvlJournal
  {
  ST_CHAR *name;			/* journal name 		*/
  ST_BOOLEAN mms_deletable;
  ST_INT numEntries;
  MVL_JOURNAL_ENTRY *entries;
  } MVL_JOURNAL_CTRL;

/************************************************************************/
/* MMS OBJECT CONTROL							*/
/************************************************************************/

/* Application Association Scope Objects				*/
typedef struct mvl_aa_obj_ctrl
  {
  ST_INT max_num_var_assoc;
  ST_INT num_var_assoc;
  MVL_VAR_ASSOC **var_assoc_tbl;

  ST_INT max_num_nvlist;
  ST_INT num_nvlist;
  MVL_NVLIST_CTRL **nvlist_tbl;   

  ST_INT max_num_jou;
  ST_INT num_jou;
  MVL_JOURNAL_CTRL **jou_tbl;

  ST_BOOLEAN foundry_objects;	/* Flag for internal use */
  ST_VOID *user_info;		/* MVL user can use this for 'whatever'	*/
  } MVL_AA_OBJ_CTRL;


/* Domain Scope Objects							*/
typedef struct mvl_dom_ctrl
  {
  ST_CHAR *name;

  ST_INT max_num_var_assoc;
  ST_INT num_var_assoc;
  MVL_VAR_ASSOC **var_assoc_tbl;

  ST_INT max_num_nvlist;
  ST_INT num_nvlist;
  MVL_NVLIST_CTRL **nvlist_tbl;   

  ST_INT max_num_jou;
  ST_INT num_jou;
  MVL_JOURNAL_CTRL **jou_tbl;

  GETDOM_RESP_INFO  *get_dom_resp_info; /* Optional			*/

  ST_BOOLEAN foundry_objects;	/* Flag for internal use */
  MVL_VAR_ASSOC *lln0_var;	/* used for computing Beh$stVal		*/
  ST_VOID *user_info;		/* MVL user can use this for 'whatever'	*/
  } MVL_DOM_CTRL;


/* VMD Scope Objects							*/
typedef struct mvl_vmd_ctrl
  {
  ST_INT max_num_var_assoc;
  ST_INT num_var_assoc;
  MVL_VAR_ASSOC **var_assoc_tbl;

#if defined (MVL_DESCR_SUPP)
  ST_INT num_descr_addr;
  MVL_DESCR_ADDR_ASSOC *descr_addr_assoc_tbl;
#endif

  ST_INT max_num_nvlist;
  ST_INT num_nvlist;
  MVL_NVLIST_CTRL **nvlist_tbl;   

  ST_INT max_num_dom;
  ST_INT num_dom;
  MVL_DOM_CTRL **dom_tbl;

  ST_INT max_num_jou;
  ST_INT num_jou;
  MVL_JOURNAL_CTRL **jou_tbl;

  ST_BOOLEAN foundry_objects;	/* Flag for internal use */

  ST_VOID *user_info;		/* MVL user can use this for 'whatever'	*/
  } MVL_VMD_CTRL;
extern MVL_VMD_CTRL mvl_vmd;


/************************************************************************/
/************************************************************************/
/* MMS OBJECT CONTROL MANIPULUATION 					*/
/************************************************************************/
/************************************************************************/
/* Naming Conventions							*/
/* 									*/
/* Find:	Locate the object, given the object name		*/
/*									*/
/* Init:	Delete all sub objects and resize tables		*/
/* Clear:	Delete all sub objects					*/
/* Resize:	Adjust table sizes					*/
/* 									*/
/* Add: 	Create and Insert 					*/
/* Create:	Allocate & initialize the object			*/
/* Insert:	Insert into tables					*/
/* 									*/
/* Remove:	Delete and Destroy					*/
/* Delete:	Delete from tables					*/
/* Destroy:	Free object and sub objects as approptiate		*/
/* 									*/
/* Pairs -	Add/Remove, Insert/Delete, Create/Destroy		*/
/* 									*/
/* Usages:								*/
/*   All dynamic, startup and reconfigure, online object changes	*/
/* 	* Use Foundry for types only					*/
/* 	* Use 'mvl_init_vmd' for startup/reconfigure			*/
/* 	* Use Add/Insert to construct object set			*/
/* 	* Use Remove/Delete to remove unwanted objects			*/
/* 	* Use Resize to modify storage as appropriate			*/
/* 									*/
/*   Both Foundry & Dynamic, startup & reconfigure, online changes 	*/
/* 	* Use Foundry for types and other objects			*/
/* 	* Use Resize to modify storage for dynamic objects		*/
/* 	* Use Add/Insert to add to object set				*/
/* 	* Use Remove/Delete to remove unwanted objects			*/
/* 									*/
/*   Renaming an object							*/
/* 	* Delete, change the name, and insert. Be careful of the name 	*/
/*           buffer handling.						*/
/* 									*/
/************************************************************************/

/**** OBJECT BULK UPDATE						*/

/* These defines can be used to speed loading of multiple objects.	*/
/* After the 'start' call, MVL stops sorting objects as they are 	*/
/* inserted. When the 'end' call is made, all objects are sorted.	*/

/* NOTE: The 'end' call must be made before NVL's are added, so that	*/
/* the variable associations can be resolved. This rule can be ignored	*/
/* if domains and variable associations are added in sorted order.	*/

extern ST_BOOLEAN mvl_obj_nosort;
ST_VOID mvl_sort_objs (ST_VOID);
#define MVL_START_BULK_OBJ_UPDATE()  {mvl_obj_nosort = SD_TRUE;}
#define MVL_END_BULK_OBJ_UPDATE()  {if (mvl_obj_nosort == SD_TRUE){\
				      mvl_obj_nosort = SD_FALSE;\
                                      mvl_sort_objs ();\
                                      }}\

/**** SCOPE LEVEL - VMD, DOMAIN						*/

/* Deletes all objects in the scope, allocates new object ctrl tables	*/
/* NOTE: Works only on scopes with NO foundry objects.			*/
ST_RET mvl_init_vmd (ST_INT max_dom, ST_INT max_var, 
		        ST_INT max_nvl, ST_INT max_jou);
ST_RET mvl_init_dom (MVL_DOM_CTRL *dom, ST_INT max_var,
			ST_INT max_nvl, ST_INT max_jou);
ST_RET mvl_init_aa (ST_INT max_var, ST_INT max_nvl, ST_INT max_jou);

/* Deletes all objects in the scope					*/
/* NOTE: Works only on scopes with NO foundry objects.			*/
ST_RET mvl_clear_vmd_objs (ST_VOID);
ST_RET mvl_clear_dom_objs (MVL_DOM_CTRL *dom);
ST_RET mvl_clear_aa_objs (ST_VOID);
ST_VOID _mvl_clear_aa_objs (MVL_AA_OBJ_CTRL *aa_objs);

/* Resize scope capacities, w/o affecting objects			*/
ST_VOID mvl_vmd_resize (ST_INT max_dom, ST_INT max_var,
			  ST_INT max_nvl, ST_INT max_jou);
ST_VOID mvl_dom_resize (MVL_DOM_CTRL *dom, ST_INT max_var,
			  ST_INT max_nvl, ST_INT max_jou);
ST_VOID mvl_aa_resize (ST_INT max_var, ST_INT max_nvl, ST_INT max_jou);
ST_VOID mvl_aa_resize_conn (MVL_AA_OBJ_CTRL *aa_objs, ST_INT max_num_var, ST_INT max_num_nvl, 
		       ST_INT max_num_jou);

/**** DOMAINS 								*/

MVL_DOM_CTRL *mvl_dom_create (ST_CHAR *name, ST_INT max_var,
			     ST_INT max_nvl, ST_INT max_jou, ST_BOOLEAN copy_name);
ST_VOID mvl_dom_destroy (MVL_DOM_CTRL *dom);

/* User can be notified when domain is being destroyed */
extern ST_VOID (*u_mvl_dom_destroy) (MVL_DOM_CTRL *dom);


/**** VARIABLE ASSOCIATIONS						*/

MVL_VAR_ASSOC *mvl_var_create (OBJECT_NAME *obj, ST_INT type_id,  /* NEW */
				 ST_VOID *data, MVL_VAR_PROC *proc,
			         ST_BOOLEAN copy_name);
MVL_VAR_ASSOC *mvl_var_create_alloc (ST_CHAR *name,
	ST_INT type_id,  
	MVL_VAR_PROC *proc);
MVL_VAR_ASSOC *mvl_var_create_derived (ST_CHAR *name,
	ST_INT type_id,
	MVL_VAR_ASSOC *baseVa,	/* base variable from which to derive	*/
	ST_INT offset_from_base,
	ST_UINT prim_num);
ST_VOID mvl_var_destroy (MVL_VAR_ASSOC *va);

/* User can be notified when VA is being destroyed */
extern ST_VOID (*u_mvl_var_destroy)(MVL_VAR_ASSOC *va);

/**** NAMED VARIABLE LISTS						*/

MVL_NVLIST_CTRL *mvl_nvl_create (MVL_VMD_CTRL *vmd_ctrl,
	ST_CHAR *nvlName,
	ST_INT numNames,
	OBJECT_NAME *var_obj,	/* array of var obj names*/
	MVL_NET_INFO *net_info,	/* where to find var_obj*/
				/* if it is AA_SPEC	*/
	ST_BOOLEAN copy_name);
ST_VOID mvl_nvl_destroy (MVL_NVLIST_CTRL *nvl);

/* User can be notified when NVL is being destroyed */
extern ST_VOID (*u_mvl_nvl_destroy) (MVL_NVLIST_CTRL *nvl);


/**** JOURNALS								*/

MVL_JOURNAL_CTRL *mvl_jou_create (OBJECT_NAME *obj, MVL_NET_INFO *net_info,
			         ST_BOOLEAN copy_name);
ST_VOID mvl_jou_destroy (MVL_JOURNAL_CTRL *jou);

/* User can be notified when journal is being destroyed */
extern ST_VOID (*u_mvl_jou_destroy) (MVL_JOURNAL_CTRL *jou);

MVL_VMD_CTRL *mvl_vmd_create (ST_INT max_num_dom, ST_INT max_num_var, ST_INT max_num_nvl, ST_INT max_num_journal);
/* NOTE: old mvl_vmd_destroy function destroyed the global VMD and all	*/
/* report controls. If that's what you want, you must now do like this:	*/
/*   mvlu_rpt_ctrl_destroy_all ();					*/
/*   mvl61850_rpt_ctrl_destroy_all ();					*/
/*   mvl_vmd_destroy (&mvl_vmd);					*/
ST_RET mvl_vmd_destroy (MVL_VMD_CTRL *vmd_ctrl);
ST_VOID mvlu_rpt_ctrl_destroy_all (ST_VOID);
ST_VOID mvl61850_rpt_ctrl_destroy_all (ST_VOID);

/************************************************************************/
/************************************************************************/
/* MANUFACTURED VARIABLE SUPPORT					*/
/* Handlers for manufactured server variable names && lists 		*/
/************************************************************************/

/* This function will be called when an					*/
/* undefined object is being accessed using alternate access. 		*/
/* If '*alt_access_done_out' is set SD_TRUE, MVL will assume that the	*/
/* alternate access operation has been addressed by the called function	*/

MVL_VAR_ASSOC *u_mvl_get_va_aa (struct mvl_vmd_ctrl *vmd_ctrl,
	ST_INT service,
	OBJECT_NAME *obj,
	MVL_NET_INFO *netInfo,
	ST_BOOLEAN alt_access_pres,
	ALT_ACCESS *alt_acc,
	ST_BOOLEAN *alt_access_done_out);
ST_VOID u_mvl_free_va (ST_INT service, 
				     MVL_VAR_ASSOC *va, 
				     MVL_NET_INFO *netInfo);

MVL_NVLIST_CTRL *u_mvl_get_nvl (ST_INT service, 
					OBJECT_NAME *obj, 
					MVL_NET_INFO *netInfo);
ST_VOID u_mvl_free_nvl (ST_INT service, 
				      MVL_NVLIST_CTRL *nvl, 
			   	      MVL_NET_INFO *netInfo);

/************************************************************************/
/************************************************************************/
/* GET NAME LIST INDICATION HANDLERS					*/
/* Default versions of these functs are found in s_getnam.c. They may	*/
/* be replaced with customized functs (e.g. for UCA, see mvl_uca.c).	*/ 

ST_INT u_gnl_ind_vars(MVL_NET_INFO *net_info, NAMELIST_REQ_INFO *req_info, 
			     ST_CHAR **ptr, ST_BOOLEAN *moreFollowsOut, 
			     ST_INT maxNames);

ST_INT u_gnl_ind_nvls(MVL_NET_INFO *net_info, NAMELIST_REQ_INFO *req_info, 
			     ST_CHAR **ptr, ST_BOOLEAN *moreFollowsOut, 
			     ST_INT maxNames);

ST_INT u_gnl_ind_doms(NAMELIST_REQ_INFO *req_info, 
			     ST_CHAR **ptr, ST_BOOLEAN *moreFollowsOut, 
			     ST_INT maxNames);

ST_INT u_gnl_ind_jous(MVL_NET_INFO *net_info, NAMELIST_REQ_INFO *req_info, 
			     ST_CHAR **ptr, ST_BOOLEAN *moreFollowsOut, 
			     ST_INT maxNames);

ST_VOID u_gnl_done(ST_INT16 mms_class, 
			     NAMELIST_RESP_INFO *resp_info);


/************************************************************************/
/************************************************************************/
/* MVL SERVER SERVICE SUPPORT						*/
/************************************************************************/
/************************************************************************/
/* Type for IEC 61850 "LastApplError" var (sent in report when control fails).*/
/* NOTE: must be before MVLAS_WR_VA_CTRL.				*/
/************************************************************************/
typedef struct
  {
  ST_CHAR CntrlObj [MVL61850_MAX_OBJREF_LEN+1];
  ST_INT8 Error;
  struct
    {
    ST_INT8 orCat;
    struct
      {
      ST_INT16 len;
      ST_UINT8 data[64];
      } orIdent;	/* OVstring64	*/
    } Origin;
  ST_UINT8 ctlNum;
  ST_INT8 AddCause;
  } MVL61850_LAST_APPL_ERROR;

/************************************************************************/
/* INDICATION CONTROL							*/
/************************************************************************/
/* The data structure 'MVL_IND_PEND' is used to keep track of MMS 	*/
/* indications pending.							*/

/* Identify */
typedef struct
  {
  IDENT_RESP_INFO *resp_info;
  } MVLAS_IDENT_CTRL;

/* Status */
typedef struct
  {
  STATUS_REQ_INFO *req_info;
  STATUS_RESP_INFO *resp_info;
  } MVLAS_STATUS_CTRL;

/* GetNameList */
typedef struct
  {
  NAMELIST_REQ_INFO *req_info;
  NAMELIST_RESP_INFO *resp_info;
  } MVLAS_NAMELIST_CTRL;

/* Read */
/* This structure represents one Variable Association being read. The	*/
/* user can set the 'acc_rslt_tag' to 'ACC_RSLT_FAILURE' if the read	*/
/* does not succeed. 							*/

typedef struct mvlas_rd_va_ctrl
  {
  MVL_VAR_ASSOC *va;
  MVL_SCOPE va_scope;

  ST_INT16 acc_rslt_tag;  /* ACC_RSLT_SUCCESS or ACC_RSLT_FAILURE	*/
  ST_INT16 failure;	  /* DataAccessError code for failure		*/
  ST_BOOLEAN alt_access_pres;
  ALT_ACCESS alt_acc;

  ST_INT numPrimData;		/* Total primitive elements for var	*/
  ST_INT numPrimDataDone;	/* Number complete			*/

  ST_VOID *usr;		  /* For user to use as she sees fit		*/
  } MVLAS_RD_VA_CTRL;

typedef struct mvlas_read_ctrl
  {
  ST_INT16 var_acc_tag;		/* VAR_ACC_NAMEDLIST or VAR_ACC_VARLIST	*/

  ST_INT numVar;		/* Variables being read			*/
  MVLAS_RD_VA_CTRL *vaCtrlTbl;

  MVL_NVLIST_CTRL *nvList;
  } MVLAS_READ_CTRL;


/* Write */
/* This structure represents one Variable Assoc being written. The	*/
/* user can set the 'acc_rslt_tag' to 'ACC_RSLT_FAILURE' if the read	*/
/* does not succeed. 							*/
struct mvl_sbo_ctrl;	/* Forward declaration (defined in mvl_uca.h)	*/
typedef struct mvlas_wr_va_ctrl
  {
  MVL_VAR_ASSOC *va;
  MVL_SCOPE va_scope;
  ST_INT16 resp_tag;	  /* WR_RSLT_FAILURE or WR_RSLT_SUCCESS		*/
  ST_INT16 failure;	  /* DataAccessError code for failure		*/
  ST_BOOLEAN alt_access_pres;
  ALT_ACCESS alt_acc; 	/* Alternate Access specification	*/

  ST_INT numPrimData;		/* Total primitive elements for var	*/
  ST_INT numPrimDataDone;	/* Number complete			*/
  struct mvl_sbo_ctrl *sboCtrl;	/* SBO Control Info.			*/
  /* NOTE: "sboCtrl" used when writing IEC 61850 "Oper" or "Cancel".	*/
  /*       Not used (i.e. NULL) for all other writes.			*/

  ST_UCHAR *oldData;

  /* Used only when writing Oper, Cancel, SBOw (for IEC 61850).		*/
  MVL61850_LAST_APPL_ERROR LastApplError;

  /* is_61850_control is initialized SD_FALSE. The user code should	*/
  /* change it to SD_TRUE when writing Oper, Cancel, SBOw (for IEC 61850).*/
  ST_BOOLEAN is_61850_control;

  ST_VOID *usr;		  /* For user to use as she sees fit		*/
  } MVLAS_WR_VA_CTRL;

typedef struct mvlas_write_ctrl
  {
  ST_INT numVar;		/* Variables being written		*/
  MVLAS_WR_VA_CTRL *vaCtrlTbl;
  ST_BOOLEAN ind_funct_done;	/* indication function completed?	*/
  } MVLAS_WRITE_CTRL;


/* GetVariableAccessAttributes */
typedef struct
  {
  GETVAR_REQ_INFO *req_info;
  GETVAR_RESP_INFO *resp_info;
  } MVLAS_GETVAR_CTRL;

/* DefineNamedVariableList */
typedef struct
  {
  DEFVLIST_REQ_INFO *req_info;
  } MVLAS_DEFVLIST_CTRL;

/* GetNamedVariableListAttributes */
typedef struct
  {
  GETVLIST_REQ_INFO *req_info;
  GETVLIST_RESP_INFO *resp_info;	/* allocate appropriate size	*/
  } MVLAS_GETVLIST_CTRL;

/* DeleteNamedVariableList */
typedef struct
  {
  DELVLIST_REQ_INFO *req_info;
  DELVLIST_RESP_INFO *resp_info;
  } MVLAS_DELVLIST_CTRL;

/* GetDomainAttributes */
typedef struct
  {
  GETDOM_REQ_INFO *req_info;
  GETDOM_RESP_INFO *resp_info;
  } MVLAS_GETDOM_CTRL;

/* File Open */
typedef struct
  {
  ST_CHAR filename [MAX_FILE_NAME+1];	/* array (not ptr) so always valid*/
  ST_UINT32 init_pos;
  FOPEN_RESP_INFO *resp_info;
  } MVLAS_FOPEN_CTRL;

/* File Read */
typedef struct
  {
  FREAD_REQ_INFO *req_info;
  ST_INT max_size;
  FREAD_RESP_INFO *resp_info;
  } MVLAS_FREAD_CTRL;

/* File Close */
typedef struct
  {
  FCLOSE_REQ_INFO *req_info;
  } MVLAS_FCLOSE_CTRL;

/* File Directory */
typedef struct
  {
  ST_UINT32  fsize; 			/* file size (# bytes)		*/
  ST_BOOLEAN mtimpres;			/* last modified time present	*/
  time_t     mtime; 			/* last modified time		*/
  ST_CHAR filename [MAX_FILE_NAME+1];
  } MVL_DIR_ENT;

typedef struct
  {
  ST_INT     num_dir_ent;		/* number of directory entries	*/
  ST_BOOLEAN more_follows;		/* more dir entries follow	*/
					/*   default: SD_FALSE		*/
  MVL_DIR_ENT *dir_ent;			/* ptr to array of dir entries	*/
  } MVL_FDIR_RESP_INFO;

typedef struct
  {
  ST_CHAR fs_filename[MAX_FILE_NAME+1];	/* array (not ptr) so always valid*/
  ST_CHAR ca_filename[MAX_FILE_NAME+1];	/* array (not ptr) so always valid*/
  MVL_FDIR_RESP_INFO *resp_info;
  } MVLAS_FDIR_CTRL;

/* ObtainFile */
typedef struct
  {
  ST_CHAR srcfilename [MAX_FILE_NAME+1];	/* array (not ptr) so always valid*/
  ST_CHAR destfilename [MAX_FILE_NAME+1];	/* array (not ptr) so always valid*/
  } MVLAS_OBTFILE_CTRL;

/* File Delete */
typedef struct
  {
  ST_CHAR filename [MAX_FILE_NAME+1];	/* array (not ptr) so always valid*/
  } MVLAS_FDELETE_CTRL;

/* File Rename */
typedef struct
  {
  ST_CHAR curfilename [MAX_FILE_NAME+1];	/* array (not ptr) so always valid*/
  ST_CHAR newfilename [MAX_FILE_NAME+1];	/* array (not ptr) so always valid*/
  } MVLAS_FRENAME_CTRL;

/* InitializeJournal */
typedef struct
  {
  JINIT_REQ_INFO *req_info;
  JINIT_RESP_INFO *resp_info;
  } MVLAS_JINIT_CTRL;

/* ReadJournal */
typedef struct
  {
  JREAD_REQ_INFO *req_info;
  JREAD_RESP_INFO *resp_info;	/* Variable size. User or mvlas_* must alloc.*/
  } MVLAS_JREAD_CTRL;

/* JournalStatus */
typedef struct
  {
  JSTAT_REQ_INFO *req_info;
  JSTAT_RESP_INFO *resp_info;
  } MVLAS_JSTAT_CTRL;

typedef struct
  {
  INITUPL_REQ_INFO *req_info;
  INITUPL_RESP_INFO *resp_info;
  } MVLAS_INITUPL_CTRL;

typedef struct
  {
  UPLOAD_REQ_INFO *req_info;
  UPLOAD_RESP_INFO *resp_info;
  } MVLAS_UPLOAD_CTRL;

typedef struct
  {
  TERMUPL_REQ_INFO *req_info;
  } MVLAS_TERMUPL_CTRL;

typedef struct
  {
  INITDOWN_REQ_INFO *req_info;
  } MVLAS_INITDOWN_CTRL;

typedef struct
  {
  DELDOM_REQ_INFO *req_info;
  } MVLAS_DELDOM_CTRL;

typedef struct
  {
  CREPI_REQ_INFO *req_info;
  } MVLAS_CREPI_CTRL;

typedef struct
  {
  DELPI_REQ_INFO *req_info;
  } MVLAS_DELPI_CTRL;

typedef struct
  {
  GETPI_REQ_INFO *req_info;
  GETPI_RESP_INFO *resp_info;
  } MVLAS_GETPI_CTRL;

typedef struct
  {
  START_REQ_INFO *req_info;
  } MVLAS_START_CTRL;

typedef struct
  {
  STOP_REQ_INFO *req_info;
  } MVLAS_STOP_CTRL;

typedef struct
  {
  RESUME_REQ_INFO *req_info;
  } MVLAS_RESUME_CTRL;

typedef struct
  {
  RESET_REQ_INFO *req_info;
  } MVLAS_RESET_CTRL;

typedef struct
  {
  GETCL_REQ_INFO *req_info;
  GETCL_RESP_INFO *resp_info;
  } MVLAS_GETCL_CTRL;

/* MVL Indication Control						*/
typedef struct mvl_ind_pend
  {
  DBL_LNK l;
  MVL_COMM_EVENT *event;
  ST_INT op;	       		/* MMS Opcode (MMSOP_READ, etc.)	*/
  union
    {
    MVLAS_READ_CTRL rd;
    MVLAS_WRITE_CTRL wr;
    MVLAS_IDENT_CTRL ident;
    MVLAS_STATUS_CTRL status;
    MVLAS_NAMELIST_CTRL namelist;
    MVLAS_GETVAR_CTRL getvar;
    MVLAS_GETDOM_CTRL getdom;
    MVLAS_FOPEN_CTRL fopen;
    MVLAS_FREAD_CTRL fread;
    MVLAS_FCLOSE_CTRL fclose;
    MVLAS_FDIR_CTRL fdir;
    MVLAS_OBTFILE_CTRL obtfile;
    MVLAS_FDELETE_CTRL fdelete;
    MVLAS_FRENAME_CTRL frename;
    MVLAS_DEFVLIST_CTRL defvlist;
    MVLAS_GETVLIST_CTRL getvlist;
    MVLAS_DELVLIST_CTRL delvlist;
    MVLAS_JINIT_CTRL jinit;
    MVLAS_JREAD_CTRL jread;
    MVLAS_JSTAT_CTRL jstat;
    MVLAS_INITUPL_CTRL initupl;
    MVLAS_UPLOAD_CTRL upload;
    MVLAS_TERMUPL_CTRL termupl;
    MVLAS_INITDOWN_CTRL initdown;
    MVLAS_DELDOM_CTRL deldom;
    MVLAS_CREPI_CTRL crepi;
    MVLAS_DELPI_CTRL delpi;
    MVLAS_GETPI_CTRL getpi;
    MVLAS_START_CTRL start;
    MVLAS_STOP_CTRL stop;
    MVLAS_RESUME_CTRL resume;
    MVLAS_RESET_CTRL reset;
    MVLAS_GETCL_CTRL getcl;
    } u;  
  /* For IEC/UCA Report scanning:					*/
  /* This function is called when scan completes for "one" variable	*/
  /* (i.e. all primitive reads complete for "one" variable of rpt).	*/
  ST_VOID (*scan_va_done_fun)(struct mvl_ind_pend *ind, MVL_VAR_ASSOC *va);

  /* User function to call INSTEAD of default Response function.	*/
  /* Currently only allowed for Read service (i.e. replaces call to	*/
  /* mvlas_read_resp).							*/
  ST_RET (*usr_resp_fun) (struct mvl_ind_pend *ind_pend);

  ST_VOID *usr_ind_ctrl;
  ST_VOID *usr;			/* For user to use as she sees fit		*/
  } MVL_IND_PEND;

/************************************************************************/
/* USER INDICATION and MVL RESPONSE FUNCTIONS				*/
/* MVL calls the 'u_mvl_xxx_ind' function from within 'mvl_comm_serve'	*/
/* to notify the user of the indication. The user must call a response 	*/
/* function ('mvlas_xxx_resp or 'mplas_xxx_resp') either from within 	*/
/* the user indication function or some time later.			*/

/* Identify */
ST_VOID u_mvl_ident_ind (MVL_IND_PEND *ind);
ST_VOID mplas_ident_resp (MVL_IND_PEND *ind);	/* Primitive		*/

/* Status */
ST_VOID u_mvl_status_ind (MVL_IND_PEND *ind);
ST_VOID mplas_status_resp (MVL_IND_PEND *ind);	/* Primitive		*/

/* Unsolicited Status */
ST_VOID u_mvl_ustatus_ind (MVL_COMM_EVENT *event);	/* NOTE: different arg*/

/* GetNameList */
ST_VOID u_mvl_namelist_ind (MVL_IND_PEND *ind);
ST_VOID mplas_namelist_resp (MVL_IND_PEND *ind);/* Primitive		*/
ST_VOID mvlas_namelist_resp (MVL_IND_PEND *ind);/* Virtual Machine	*/

/* Read */
ST_VOID u_mvl_read_ind (MVL_IND_PEND *indCtrl);
ST_RET mvlas_read_resp (MVL_IND_PEND *indCtrl);

/* Write */
ST_VOID u_mvl_write_ind (MVL_IND_PEND *indCtrl);
ST_VOID mvlas_write_resp (MVL_IND_PEND *indCtrl);

/* GetVariableAccessAttributes */
ST_VOID u_mvl_getvar_ind (MVL_IND_PEND *ind);
ST_VOID mplas_getvar_resp (MVL_IND_PEND *ind);	/* Primitive		*/
ST_VOID mvlas_getvar_resp (MVL_IND_PEND *ind);	/* Virtual Machine	*/

/* DefineNamedVariableList */
ST_VOID u_mvl_defvlist_ind (MVL_IND_PEND *ind);
ST_VOID mplas_defvlist_resp (MVL_IND_PEND *ind);/* Primitive		*/
ST_VOID mvlas_defvlist_resp (MVL_IND_PEND *ind);/* Virtual Machine	*/

/* GetNamedVariableListAttributes */
ST_VOID u_mvl_getvlist_ind (MVL_IND_PEND *ind);
ST_VOID mplas_getvlist_resp (MVL_IND_PEND *ind);/* Primitive		*/
ST_VOID mvlas_getvlist_resp (MVL_IND_PEND *ind);/* Virtual Machine	*/

/* DeleteNamedVariableList */
ST_VOID u_mvl_delvlist_ind (MVL_IND_PEND *ind);
ST_VOID mplas_delvlist_resp (MVL_IND_PEND *ind);/* Primitive		*/
ST_VOID mvlas_delvlist_resp (MVL_IND_PEND *ind);/* Virtual Machine	*/

/* GetDomainAttributes */
ST_VOID u_mvl_getdom_ind (MVL_IND_PEND *ind);
ST_VOID mplas_getdom_resp (MVL_IND_PEND *ind);	/* Primitive		*/
ST_VOID mvlas_getdom_resp (MVL_IND_PEND *ind);	/* Virtual Machine	*/

/* FileOpen */
ST_VOID u_mvl_fopen_ind (MVL_IND_PEND *ind);
ST_VOID mplas_fopen_resp (MVL_IND_PEND *ind);	/* Primitive		*/

/* FileRead */
ST_VOID u_mvl_fread_ind (MVL_IND_PEND *ind);
ST_VOID mplas_fread_resp (MVL_IND_PEND *ind);	/* Primitive		*/

/* FileClose */
ST_VOID u_mvl_fclose_ind (MVL_IND_PEND *ind);
ST_VOID mplas_fclose_resp (MVL_IND_PEND *ind);	/* Primitive		*/

/* FileDirectory */
ST_VOID u_mvl_fdir_ind (MVL_IND_PEND *ind);
ST_VOID mvlas_fdir_resp (MVL_IND_PEND *ind);
ST_VOID mplas_fdir_resp (MVL_IND_PEND *ind);	/* Primitive		*/

/* ObtainFile */
ST_VOID u_mvl_obtfile_ind (MVL_IND_PEND *ind);
ST_VOID mplas_obtfile_resp (MVL_IND_PEND *ind);	/* Primitive		*/
ST_VOID mvlas_obtfile_resp (MVL_IND_PEND *ind);	/* VirtualMachine	*/
ST_VOID mvlas_obtfile_resp_ex (MVL_IND_PEND *indCtrl, ST_UINT32 fileSizeLimit);

/* FileDelete */
ST_VOID u_mvl_fdelete_ind (MVL_IND_PEND *ind);
ST_VOID mplas_fdelete_resp (MVL_IND_PEND *ind);	/* Primitive		*/

/* FileRename */
ST_VOID u_mvl_frename_ind (MVL_IND_PEND *ind);
ST_VOID mplas_frename_resp (MVL_IND_PEND *ind);	/* Primitive		*/

/* InitializeJournal */
ST_VOID u_mvl_jinit_ind (MVL_IND_PEND *ind);
ST_VOID mplas_jinit_resp (MVL_IND_PEND *ind);	/* Primitive		*/

/* ReadJournal */
ST_VOID u_mvl_jread_ind (MVL_IND_PEND *ind);
ST_VOID mplas_jread_resp (MVL_IND_PEND *ind);	/* Primitive		*/
ST_VOID mvlas_jread_resp (MVL_IND_PEND *ind);	/* Virtual Machine	*/

/* ReportJournalStatus */
ST_VOID u_mvl_jstat_ind (MVL_IND_PEND *ind);
ST_VOID mplas_jstat_resp (MVL_IND_PEND *ind);	/* Primitive		*/

/* Conclude */
ST_VOID u_mvl_concl_ind (MVL_COMM_EVENT *event);
ST_VOID mplas_concl_resp (MVL_COMM_EVENT *event);	/* Primitive	*/
ST_RET mplas_concl_err (MVL_COMM_EVENT *event, ST_INT16 err_class, ST_INT16 err_code);

ST_VOID u_mvl_initupl_ind (MVL_IND_PEND *ind);
ST_VOID mplas_initupl_resp (MVL_IND_PEND *ind);	/* Primitive		*/

ST_VOID u_mvl_upload_ind (MVL_IND_PEND *ind);
ST_VOID mplas_upload_resp (MVL_IND_PEND *ind);	/* Primitive		*/

ST_VOID u_mvl_termupl_ind (MVL_IND_PEND *ind);
ST_VOID mplas_termupl_resp (MVL_IND_PEND *ind);	/* Primitive		*/

ST_VOID u_mvl_initdown_ind (MVL_IND_PEND *ind);
ST_VOID mplas_initdown_resp (MVL_IND_PEND *ind);/* Primitive		*/

ST_VOID u_mvl_deldom_ind (MVL_IND_PEND *ind);
ST_VOID mplas_deldom_resp (MVL_IND_PEND *ind);	/* Primitive		*/

ST_VOID u_mvl_crepi_ind (MVL_IND_PEND *ind);
ST_VOID mplas_crepi_resp (MVL_IND_PEND *ind);	/* Primitive		*/

ST_VOID u_mvl_delpi_ind (MVL_IND_PEND *ind);
ST_VOID mplas_delpi_resp (MVL_IND_PEND *ind);	/* Primitive		*/

ST_VOID u_mvl_getpi_ind (MVL_IND_PEND *ind);
ST_VOID mplas_getpi_resp (MVL_IND_PEND *ind);	/* Primitive		*/

ST_VOID u_mvl_start_ind (MVL_IND_PEND *ind);
ST_VOID mplas_start_resp (MVL_IND_PEND *ind);	/* Primitive		*/

ST_VOID u_mvl_stop_ind (MVL_IND_PEND *ind);
ST_VOID mplas_stop_resp (MVL_IND_PEND *ind);	/* Primitive		*/

ST_VOID u_mvl_reset_ind (MVL_IND_PEND *ind);
ST_VOID mplas_reset_resp (MVL_IND_PEND *ind);	/* Primitive		*/

ST_VOID u_mvl_resume_ind (MVL_IND_PEND *ind);
ST_VOID mplas_resume_resp (MVL_IND_PEND *ind);	/* Primitive		*/

ST_VOID u_mvl_getcl_ind (MVL_IND_PEND *ind);
ST_VOID mplas_getcl_resp (MVL_IND_PEND *ind);	/* Primitive		*/

/************************************************************************/
/* Server Read/Write Start and End Function Pointers			*/

/* These function pointers are invoked to allow the user application 	*/
/* to prepare for handling the indication.				*/
extern ST_RET (*u_mvl_rd_ind_start) (MVL_IND_PEND *indCtrl);
extern ST_RET (*u_mvl_wr_ind_start) (MVL_IND_PEND *indCtrl);

/* These function pointers are invoked to allow the user application 	*/
/* to free resources used in VA data buffer management.			*/
extern ST_VOID (*u_mvl_rd_resp_sent) (MVL_IND_PEND *indCtrl);
extern ST_VOID (*u_mvl_wr_resp_sent) (MVL_IND_PEND *indCtrl);


/************************************************************************/
/* Server Information Report						*/
/* This function is used to send an information report.			*/

ST_RET mvl_info_variables (MVL_NET_INFO *net_info, MVL_NVLIST_CTRL *nvl, 
			  ST_BOOLEAN listOfVariables);

/************************************************************************/
/* Server Journal Read							*/

/* Jread User Interaction						*/
/* 1. A Journal Read indication is received by MVL.			  */
/* 2. MVL calls 'u_mvl_start_jread'. User applies filter to it's journal  */
/*    data and returns the number of entries and variables that meet	  */
/*    the filter as well as JREAD buffer control information and a 	  */
/*    user handle.							  */
/*    Note that the defines:						  */
/* 		MAX_JREAD_RESP_ENTRIES					  */
/* 		MAX_JREAD_RESP_VARS					  */
/*    are used to limit the amount of information that MVL will return 	  */
/*    in the MMS response.						  */
/*    The buffer control allows the user to supply MVL with data buffers  */
/*    to be used to save the volitile Journal information (Entry ID, 	  */
/*    Variable Tag, Annotation).					  */
/* 3. MVL calls 'u_mvl_get_next_jread_entry' and 'u_mvl_free_jread_entry' */
/*    repetitively until the max MMS PDU size is reached or the number of */
/*    user entries is reached.						  */
/* 4. MVL sends the JREAD response.					  */
/* 5. MVL calls 'u_mvl_end_jread' and the user may then free resources	  */
/*    as necessary.							  */
   
/* These defines control the way that MVL handles the potentially	*/
/* volitile information presented by the user via MVL_JOURNAL_ENTRY.	*/
/* If these defines are defined, MVL will assume that the pointers	*/
/* in MVL_JOURNAL_ENTRY will become invalid after the free call and	*/
/* so it must copy the data into a buffer. These buffers are to be 	*/
/* supplied by the user, and must be large enough for the data.		*/

/* #define MVL_JREAD_ENTRYID_DYNAMIC		*/
/* #define MVL_JREAD_VARTAG_DYNAMIC		*/
/* #define MVL_JREAD_ANNOTATION_DYNAMIC		*/

typedef struct mvlJreadBufCtrl 
  {
  ST_INT numUsrEntries;		/* User must supply		*/
  ST_INT numVarInfo;		/* User must supply		*/

#if defined (MVL_JREAD_ENTRYID_DYNAMIC)
  ST_UCHAR  *entryIdBuf;	/* User must supply		*/
  ST_UCHAR  *nextEntryId;	/* MVL use only			*/
#endif

#if defined (MVL_JREAD_VARTAG_DYNAMIC)
  ST_CHAR *varTagBuf;		/* User must supply		*/
  ST_CHAR  *nextVarTag;		/* MVL use only			*/
#endif

#if defined (MVL_JREAD_ANNOTATION_DYNAMIC)
  ST_CHAR *annotationBuf;	/* User must supply		*/
  ST_CHAR *nextAnnotation;	/* MVL use only			*/
#endif

  } MVL_JREAD_BUF_CTRL;


/* User defined Journal Read response functions. These functions are	*/
/* to be supplied by the application in order to get the information	*/
/* required for a Journal Read response.				*/

/* This function should be used to apply the filter and returns the 	*/
/* total number of Journal Entries and Journal Variables that meet	*/
/* the filter criteria.							*/
ST_RET u_mvl_start_jread (MVL_JOURNAL_CTRL *jCtrl,
			  JREAD_REQ_INFO *reqInfo, 
			  MVL_JREAD_BUF_CTRL *bufCtrl,
			  ST_VOID **usrHandleOut);

/* MVL calls this function repetitively to get entries from the app.	*/
/* MVL will use the entry then will call the free function below to 	*/
/* allow the application free the resources as required.		*/
ST_RET u_mvl_get_next_jread_entry (MVL_JOURNAL_CTRL *jCtrl,
				   ST_VOID *usrHandle, 
				   ST_BOOLEAN *moreFollowsOut, 
				   MVL_JOURNAL_ENTRY **mvlJeDest);
ST_VOID u_mvl_free_jread_entry (MVL_JOURNAL_CTRL *jCtrl,
				ST_VOID *usrHandle, 
				MVL_JOURNAL_ENTRY *mvlJe);

/* When complete, MVL will call this function.				*/
ST_VOID u_mvl_end_jread (MVL_JOURNAL_CTRL *jCtrl,
			 ST_VOID *usrHandle, 
			 MVL_JREAD_BUF_CTRL *bufCtrl,
			 JREAD_REQ_INFO *reqInfo);

/************************************************************************/
/************************************************************************/
/* Temporary file name 							*/

/* User can supply path for temp files, prefix for file name		*/
/* Typical forms are:							*/
/*	"/temp/"							*/
/*	"S:/temp/"							*/
/*	"//mike/temp/"							*/
/*	"X:"								*/

#define MVL_MAX_TMPFILE_PATH  MAX_FILE_NAME 
extern ST_CHAR *mvl_tmpfile_path;

/* Users should have a buffer at least this big, unless they can	*/
/* control the temp filename path to be used.				*/ 
#define MVL_MAX_TMPNAM_LEN    (MVL_MAX_TMPFILE_PATH + 12)
ST_RET _mvl_tmpnam (ST_CHAR *dest, ST_CHAR *tmpfile_path);
ST_VOID _mvl_getpath (ST_CHAR *file_spec, ST_CHAR *dest);


/************************************************************************/
/************************************************************************/
/************************************************************************/
/* MVL CLIENT SERVICE SUPPORT						*/
/************************************************************************/
/* conf_process_mode defines */
/* When the confirm is received (and ICCP support is enabled) this flag	*/
/* is used to tell MVL how to process the confirm.			*/
#define MI_CONF_ICCP_DV	0x0001


/* Read */
typedef struct mvl_read_resp_parse_info
  {
  ST_RET result;                /* SD_SUCCESS for OK			*/

				/* MMS level result information		*/
  ST_INT16 acc_rslt_tag; 	/* access result tag			*/
  ST_INT16 failure;		/* data access error			*/

  ST_VOID *dest;                /* Where data is to be put		*/
  ST_INT type_id;          	/* type of variable			*/

  ST_BOOLEAN alt_acc_pres;
  ALT_ACCESS *alt_acc;
  ST_BOOLEAN alt_acc_type_aa;	/* type_id is derived type		*/
  ST_BOOLEAN alt_acc_data_packed;

  ST_BOOLEAN descr_arr;         /* for described read of array		*/
  ST_RTINT arr_size;            /*   number of elements			*/
  } MVL_READ_RESP_PARSE_INFO;

/* Write */
typedef struct mvl_write_req_info
  {
  ST_RET result;               /* SD_SUCCESS for OK			*/

			       /* MMS level result detail		*/		
  ST_INT16    resp_tag;	       /* response tag 				*/
  ST_INT16    failure;	       /* data access error			*/

  ST_VOID *local_data;         /* Source of local data			*/
  ST_INT type_id;              /* type of variable			*/

  ST_BOOLEAN alt_acc_pres;
  ALT_ACCESS *alt_acc;
  ST_BOOLEAN alt_acc_type_aa;	/* type_id is derived type		*/
  ST_BOOLEAN alt_acc_data_packed;

  ST_BOOLEAN arr; 	       /* SD_TRUE if type is array and the #    */
  ST_RTINT num_el;             /* elements needs to be set		*/
  } MVL_WRITE_REQ_INFO ;

/* JournalRead */
typedef struct
  {
  ST_INT     num_of_jou_entry;		/* number of journal entries	*/
  ST_BOOLEAN more_follows; 		/* default = false		*/
  MVL_JOURNAL_ENTRY *jou_entry;		/* ptr to array of entries	*/
  } MVL_JREAD_RESP_INFO;

/* Outstanding Request Control */
typedef struct mvl_req_pend
  {
  DBL_LNK l;			 /* For linked list			*/
  ST_UINT32 invoke_id;             
  time_t request_time;
  MVL_NET_INFO *net_info;        /* Who it was sent to			*/
  ST_INT op;                     /* MMS Opcode				*/
  union
    {
    struct
      {
      MVL_READ_RESP_PARSE_INFO *parse_info;
      ST_INT num_data;
      } rd;
    struct
      {
      MVL_WRITE_REQ_INFO *w_info;
      ST_INT num_data;
      } wr;
    struct
      {
      IDENT_RESP_INFO *resp_info;
      } ident;
    struct
      {
      INIT_INFO *resp_info;
      } init;
    struct
      {
      FOPEN_RESP_INFO *resp_info;
      } fopen;
    struct
      {
      FREAD_RESP_INFO *resp_info;
      } fread;
    struct
      {
      MVL_FDIR_RESP_INFO *resp_info;
      } fdir;
    struct
      {
      JINIT_RESP_INFO *resp_info;
      } jinit;
    struct
      {
      JSTAT_RESP_INFO *resp_info;
      } jstat;
    struct
      {
      MVL_JREAD_RESP_INFO *resp_info;
      } jread;
    struct
      {
      GETVLIST_RESP_INFO *resp_info;
      } getvlist;
    struct
      {
      DELVLIST_RESP_INFO *resp_info;
      } delvlist;
    struct
      {
      GETVAR_RESP_INFO *resp_info;
      } getvar;
    struct
      {
      GETDOM_RESP_INFO *resp_info;
      } getdom;
    struct
      {
      NAMELIST_RESP_INFO *resp_info;
      } getnam;
    struct
      {
      STATUS_RESP_INFO *resp_info;
      } status;
    struct
      {
      DOWNLOAD_RESP_INFO *resp_info;
      } download;			/* DownloadSegment resp info	*/
    } u;
  MVL_COMM_EVENT *event;	/* save event ptr to free later.*/
  ST_BOOLEAN done;
  ST_BOOLEAN decr_numpend_req;	/* if TRUE, pending count was incremented*/
				/* when req sent, must be decremented	*/
				/* when confirm received		*/
  ST_RET result;         /* SD_SUCCESS or error code			*/
  ERR_INFO         *error_info;	 /* When result == MVL_ERR_CNF_ERR_OK	*/
  REJECT_RESP_INFO *reject_info; /* When result == MVL_ERR_CNF_REJ_ERR	*/


			/* User done function for async functions	*/ 
  ST_VOID (*u_req_done) (struct mvl_req_pend *req);
  ST_VOID *v;			/* For MVL user's use			*/

  struct _mi_req_ctrl *mi_req;	/* used only for ICCP-LITE product	*/
  } MVL_REQ_PEND;


struct mvl_fget_req_info;   /* forward declaration */

typedef struct mvl_fget_req_info
  {
  ST_BOOLEAN    fget_done;
  ST_INT        fget_error;
  /* pointer to user's fget confirm function */
  ST_VOID (*fget_cnf_ptr)(struct mvl_fget_req_info *state);
  ST_CHAR	srcfilename[MAX_FILE_NAME+1];
  ST_CHAR	destfilename[MAX_FILE_NAME+1];

  ST_VOID *v;			/* For MVL user's use			*/

  /* The rest of this structure is not normally accessed by the user.	*/  
  char  	tempfilename[MVL_MAX_TMPNAM_LEN+1];
  FILE		*fp;
  ST_INT32	frsmid;
  ST_UINT32	fsize;
  } MVL_FGET_REQ_INFO;


ST_RET _mvla_send_status (MVL_NET_INFO *net_info, 
			  STATUS_REQ_INFO *req_info,
			  MVL_REQ_PEND *req_pend);
ST_RET _mvla_send_getname (MVL_NET_INFO  *net_info,
    			   NAMELIST_REQ_INFO *req_info,
			   MVL_REQ_PEND *req_pend);
ST_RET _mvla_send_read_variables (MVL_NET_INFO  *net_info,
                          READ_REQ_INFO *read_info,
			  ST_INT num_data,
			  MVL_READ_RESP_PARSE_INFO *parse_info,
			  MVL_REQ_PEND *req_pend);
ST_RET _mvla_send_getvar (MVL_NET_INFO  *net_info,
			  GETVAR_REQ_INFO *req_info,
			  MVL_REQ_PEND *req_pend);
ST_RET _mvla_send_defvlist (MVL_NET_INFO  *net_info,
			    DEFVLIST_REQ_INFO *req_info,
			    MVL_REQ_PEND *req_pend);
ST_RET _mvla_send_getvlist (MVL_NET_INFO  *net_info,
			    GETVLIST_REQ_INFO *req_info,
			    MVL_REQ_PEND *req_pend);
ST_RET _mvla_send_delvlist (MVL_NET_INFO  *net_info,
			    DELVLIST_REQ_INFO *req_info,
			    MVL_REQ_PEND *req_pend);
ST_RET _mvla_send_getdom (MVL_NET_INFO  *net_info,
			  GETDOM_REQ_INFO *req_info,
			  MVL_REQ_PEND *req_pend);
ST_RET _mvla_send_fopen (MVL_NET_INFO  *net_info,
		  	 ST_CHAR *filename,
			 ST_UINT32 init_pos,
			 MVL_REQ_PEND *req_pend);
ST_RET _mvla_send_fread (MVL_NET_INFO  *net_info,
			 FREAD_REQ_INFO *req_info,
			 MVL_REQ_PEND *req_pend);
ST_RET _mvla_send_fclose (MVL_NET_INFO  *net_info,
			  FCLOSE_REQ_INFO *req_info,
			  MVL_REQ_PEND *req_pend);
ST_RET _mvla_send_fdir (MVL_NET_INFO  *net_info,
			ST_CHAR *filename,
			ST_CHAR *ca_filename,
			MVL_REQ_PEND *req_pend);
ST_RET _mvla_send_fdelete (MVL_NET_INFO  *net_info,
			   ST_CHAR *filename,
			   MVL_REQ_PEND *req_pend);
ST_RET _mvla_send_jinit (MVL_NET_INFO  *net_info,
			 JINIT_REQ_INFO *req_info,
			 MVL_REQ_PEND *req_pend);
ST_RET _mvla_send_jread (MVL_NET_INFO  *net_info,
			 JREAD_REQ_INFO *req_info,
			 MVL_REQ_PEND *req_pend);
ST_RET _mvla_send_jstat (MVL_NET_INFO  *net_info,
			 JSTAT_REQ_INFO *req_info,
			 MVL_REQ_PEND *req_pend);

/* Function to make async function sync					*/
ST_RET mvl_wait_req_done (MVL_REQ_PEND *req);

/* Use this to free a request contrl after the confirm has been rx'd	*/
ST_VOID mvl_free_req_ctrl (MVL_REQ_PEND *pend_req);

/* Use this to free any pending Indication Ctrl structures		*/
ST_VOID mvl_clr_ind_que (MVL_NET_INFO *net_info);

/************************************************************************/
/* CLIENT REQUEST FUNCTIONS						*/
/* All client request functions are asynchronous. They return as soon	*/
/* as the request is queued for sending and the caller must use		*/
/* MVL_REQ_PEND to determine when the Confirm has been received.	*/

/* Identify */
ST_RET mvla_identify (MVL_NET_INFO  *net_info, MVL_REQ_PEND **req_out);

/* Status */
ST_RET mvla_status (MVL_NET_INFO  *net_info, STATUS_REQ_INFO *req_info,
		    MVL_REQ_PEND **req_out);

/* Unsolicited Status */
ST_RET mvl_ustatus (MVL_NET_INFO *net_info, USTATUS_REQ_INFO *req_info);

/* GetNameList */
ST_RET mvla_getnam (MVL_NET_INFO  *net_info, NAMELIST_REQ_INFO *req_info,
		    MVL_REQ_PEND **req_out);


/* Read */
ST_RET mvla_read_variables (MVL_NET_INFO  *net_info, READ_REQ_INFO *read_info,
                        ST_INT num_data,
                        MVL_READ_RESP_PARSE_INFO *parse_info,
			MVL_REQ_PEND **req_out);


/* Write */
ST_RET mvla_write_variables (MVL_NET_INFO *net_info, WRITE_REQ_INFO *write_info,
                            ST_INT num_data, MVL_WRITE_REQ_INFO *w_info,
 			    MVL_REQ_PEND **req_out);

/* GetVariableAccessAttributes */
ST_RET mvla_getvar (MVL_NET_INFO  *net_info, GETVAR_REQ_INFO *req_info,
		    MVL_REQ_PEND **req_out);

/* DefineNamedVariableList */
ST_RET mvla_defvlist (MVL_NET_INFO  *net_info, DEFVLIST_REQ_INFO *req_info,
		      MVL_REQ_PEND **req_out);

/* GetNamedVariableListAttributes */
ST_RET mvla_getvlist (MVL_NET_INFO  *net_info, GETVLIST_REQ_INFO *req_info,
		      MVL_REQ_PEND **req_out);

/* DeleteNamedVariableList */
ST_RET mvla_delvlist (MVL_NET_INFO  *net_info, DELVLIST_REQ_INFO *req_info,
		      MVL_REQ_PEND **req_out);

/* GetDomainAttributes */
ST_RET mvla_getdom (MVL_NET_INFO  *net_info, GETDOM_REQ_INFO *req_info,
		    MVL_REQ_PEND **req_out);

/* File Open */
ST_RET mvla_fopen (MVL_NET_INFO  *net_info, ST_CHAR *filename,
		   ST_UINT32 init_pos, MVL_REQ_PEND **req_out);

/* File Read */
ST_RET mvla_fread (MVL_NET_INFO *net_info, FREAD_REQ_INFO *req_info,
		   MVL_REQ_PEND **req_out);

/* File Close */
ST_RET mvla_fclose (MVL_NET_INFO  *net_info, FCLOSE_REQ_INFO *req_info,
		    MVL_REQ_PEND **req_out);

/* File Directory */
ST_RET mvla_fdir (MVL_NET_INFO  *net_info, ST_CHAR *filename,
		  ST_CHAR *ca_filename,	MVL_REQ_PEND **req_out);

/* File Delete */
ST_RET mvla_fdelete (MVL_NET_INFO  *net_info, ST_CHAR *filename,
		     MVL_REQ_PEND **req_out);

/* File Rename */
ST_RET mvla_frename (MVL_NET_INFO  *net_info, ST_CHAR *curfilename,
		    ST_CHAR *newfilename, MVL_REQ_PEND **req_out);

/* Obtainfile */
ST_RET mvla_obtfile (MVL_NET_INFO  *net_info, ST_CHAR *srcfilename,
	    	     ST_CHAR *destfilename, MVL_REQ_PEND **req_out);

/* File Get */
/* No synchronous File Get function "mvl_fget". Requires multiple reqs,	*/
/* so cannot be synchronous.						*/
ST_RET mvla_fget (MVL_NET_INFO  *net_info, ST_CHAR *srcfilename,
	    	     ST_CHAR *destfilename, MVL_FGET_REQ_INFO *fget_req_info);

/* InitializeJournal */
ST_RET mvla_jinit (MVL_NET_INFO  *net_info, JINIT_REQ_INFO *req_info,
		   MVL_REQ_PEND **req_out);

/* ReadJournal */
ST_RET mvla_jread (MVL_NET_INFO  *net_info, JREAD_REQ_INFO *req_info,
		   MVL_REQ_PEND **req_out);

/* WriteJournal */
ST_RET mvla_jwrite (MVL_NET_INFO  *net_info, JWRITE_REQ_INFO *req_info,
		    MVL_REQ_PEND **req_out);

/* JournalStatus */
ST_RET mvla_jstat (MVL_NET_INFO  *net_info, JSTAT_REQ_INFO *req_info,
		   MVL_REQ_PEND **req_out);

/* InitiateDownload	*/
/* NOT YET IMPLEMENTED! ONLY NEEDED FOR CLIENT	*/

/* DownloadSegment	*/
ST_RET mvla_download (MVL_NET_INFO  *net_info, DOWNLOAD_REQ_INFO *req_info,
		   MVL_REQ_PEND **req_out);

/* TerminateDownload	*/
ST_RET mvla_termdown (MVL_NET_INFO  *net_info, TERMDOWN_REQ_INFO *req_info,
		   MVL_REQ_PEND **req_out);

/* Conclude */
ST_RET mvla_concl (MVL_NET_INFO *net_info, MVL_REQ_PEND **req_out);
ST_RET _mvla_send_concl (MVL_NET_INFO *net_info, MVL_REQ_PEND *req_pend);

/************************************************************************/
/* ENCODE FUNCTIONS - each encodes a req or resp but does NOT send it.	*/
/************************************************************************/
/* Information Report Request	*/ 
ST_RET mvl_encode_info_rpt (MVL_NET_INFO *net_info,
			MVL_NVLIST_CTRL *nvl, 
			ST_BOOLEAN listOfVariables,
			ASN1_ENC_CTXT *aCtx);

/* NOTE: other encode functions not currently needed.	*/

/************************************************************************/
/* Client Info Rpt indication						*/

/* This function is called when thje client application received an	*/
/* information report 							*/
ST_VOID u_mvl_info_rpt_ind (MVL_COMM_EVENT *event);

/* This function is used to convert Info Report data to local format	*/
/* The user supplies a table of variable associations, which speify the	*/
/* data type, destination, and pre/post processing functions.		*/
/* This function free's the info report resources when the conversion 	*/
/* are complete.							*/
 
ST_VOID mvl_info_data_to_local (MVL_COMM_EVENT *event,
				ST_INT num_va,
				MVL_VAR_ASSOC **info_va);

#if defined(MVL_REM_VMD_SUPPORT)
extern MVL_VMD_CTRL *(*u_mvl_get_rem_vmd_fun) (MVL_NET_INFO *net_info);
ST_VOID mvl_info_rpt_rem_process (MVL_COMM_EVENT *event);
#endif

ST_RET mvlu_add_rt_type (RUNTIME_TYPE *rt, ST_INT numRt, ST_INT *typeIdOut);
ST_VOID mvlu_free_rt_type (ST_INT typeId);

/* Type to String and visa-versa */
ST_INT mvl_typename_to_typeid (ST_CHAR *type_id_string);
ST_CHAR *mvl_typeid_to_typename (ST_INT type_id);


/************************************************************************/
/************************************************************************/

typedef struct
  {
  ST_DOUBLE     startTime;
  ST_INT32	callingConn;		/* Calling Connections		*/
  ST_INT32	callingConnOk;		/* Calling Connections Succesful*/
  ST_INT32	callingConnFail;	/* Calling Connections Failed	*/
  ST_INT32	calledConn;		/* Called Connections		*/
  ST_INT32	calledConnOK;		/* Called Connections Succesful	*/
  ST_INT32	calledConnFail;		/* Called Connections Failed	*/
  ST_INT32	clientConcl;		/* Client Concludes		*/
  ST_INT32	serverConcl;		/* Server Concludes		*/
  ST_INT32	localAbort;		/* Local Aborts			*/
  ST_INT32	remoteAbort;		/* Remote Aborts		*/
  ST_INT32	rejectsSent;		/* Rejects Sent			*/
  ST_INT32	rejectsRcvd;		/* Rejects Received		*/
  ST_INT32	clientReq;		/* Client Requests		*/
  ST_INT32	clientReqOk;		/* Client Requests OK		*/
  ST_INT32	clientReqErr;		/* Client Requests Error	*/
  ST_INT32	clientVarRdOk;		/* Client Variables Read OK	*/
  ST_INT32	clientVarRdFail;	/* Client Variables Read Error	*/
  ST_INT32	clientVarWrOk;		/* Client Variables Write OK	*/
  ST_INT32	clientVarWrErr;		/* Client Variables Write Error	*/
  ST_INT32	clientInfoRpt;		/* Client Information Reports	*/
  ST_INT32	clientUstatus;		/* Client Unsolicited Status	*/
  ST_INT32	serverInd;		/* Server Indications		*/
  ST_INT32	serverRespOk;		/* Server Responses OK		*/
  ST_INT32	serverRespErr;		/* Server Responses Error	*/
  ST_INT32	serverVarRdOk;		/* Server Variables Read OK	*/
  ST_INT32	serverVarRdErr;		/* Server Variables Read Error	*/
  ST_INT32	serverVarWrOk;		/* Server Variables Write OK	*/
  ST_INT32	serverVarWrErr;		/* Server Variables Write Error	*/
  ST_INT32	serverInfoRpt;		/* Server Information Reports	*/
  ST_INT32	serverUstatus;		/* Server Unsolicited Status	*/
  } MVL_MMS_STATISTICS;

extern MVL_MMS_STATISTICS mvl_mms_statistics;

/************************************************************************/
/************************************************************************/
/************************************************************************/
/* MVL INTERNAL DECLARATIONS						*/
/************************************************************************/
/************************************************************************/

/* Built in MVL Indication and Confirm functions, typically called 	*/
/* from "mvlop_en.c" if service is enabled.				*/

ST_VOID _mvl_process_read_ind (MVL_IND_PEND *ind_pend);
ST_VOID _mvl_process_write_ind (MVL_IND_PEND *ind_pend);
ST_VOID _mvl_process_ident_ind (MVL_IND_PEND *ind_pend);
ST_VOID _mvl_process_info_ind (MVL_COMM_EVENT *event);
ST_VOID _mvl_process_status_ind (MVL_IND_PEND *ind_pend);
ST_VOID _mvl_process_ustatus_ind (MVL_COMM_EVENT *event);
ST_VOID _mvl_process_getnam_ind (MVL_IND_PEND *ind_pend);
ST_VOID _mvl_process_getvaa_ind (MVL_IND_PEND *ind_pend);
ST_VOID _mvl_process_getvla_ind (MVL_IND_PEND *ind_pend);
ST_VOID _mvl_process_getdom_ind (MVL_IND_PEND *ind_pend);
ST_VOID _mvl_process_fopen_ind (MVL_IND_PEND *ind_pend);
ST_VOID _mvl_process_fread_ind (MVL_IND_PEND *ind_pend);
ST_VOID _mvl_process_fclose_ind (MVL_IND_PEND *ind_pend);
ST_VOID _mvl_process_fdir_ind (MVL_IND_PEND *ind_pend);
ST_VOID _mvl_process_obtfile_ind (MVL_IND_PEND *ind_pend);
ST_VOID _mvl_process_fdelete_ind (MVL_IND_PEND *ind_pend);
ST_VOID _mvl_process_frename_ind (MVL_IND_PEND *ind_pend);
ST_VOID _mvl_process_jinit_ind (MVL_IND_PEND *ind_pend);
ST_VOID _mvl_process_jread_ind (MVL_IND_PEND *ind_pend);
ST_VOID _mvl_process_concl_ind (MVL_COMM_EVENT *event);
ST_VOID _mvl_process_cancel_ind (MVL_COMM_EVENT *event);
ST_VOID _mvl_process_defvlist_ind (MVL_IND_PEND *ind_pend);
ST_VOID _mvl_process_delvlist_ind (MVL_IND_PEND *ind_pend);
ST_VOID _mvl_process_jstat_ind (MVL_IND_PEND *ind_pend);
ST_VOID _mvl_process_initdown_ind (MVL_IND_PEND *ind_pend);
ST_VOID _mvl_process_download_ind (MVL_IND_PEND *ind_pend);
ST_VOID _mvl_process_termdown_ind (MVL_IND_PEND *ind_pend);
ST_VOID _mvl_process_initupl_ind (MVL_IND_PEND *ind_pend);
ST_VOID _mvl_process_upload_ind (MVL_IND_PEND *ind_pend);
ST_VOID _mvl_process_termupl_ind (MVL_IND_PEND *ind_pend);
ST_VOID _mvl_process_deldom_ind (MVL_IND_PEND *ind_pend);
ST_VOID _mvl_process_crepi_ind (MVL_IND_PEND *ind_pend);
ST_VOID _mvl_process_delpi_ind (MVL_IND_PEND *ind_pend);
ST_VOID _mvl_process_start_ind (MVL_IND_PEND *ind_pend);
ST_VOID _mvl_process_stop_ind (MVL_IND_PEND *ind_pend);
ST_VOID _mvl_process_resume_ind (MVL_IND_PEND *ind_pend);
ST_VOID _mvl_process_reset_ind (MVL_IND_PEND *ind_pend);
ST_VOID _mvl_process_kill_ind (MVL_IND_PEND *ind_pend);
ST_VOID _mvl_process_getpi_ind (MVL_IND_PEND *ind_pend);
ST_VOID _mvl_process_getcl_ind (MVL_IND_PEND *ind_pend);

ST_VOID _mvl_process_read_conf (MVL_REQ_PEND *req);
ST_VOID _mvl_process_write_conf (MVL_REQ_PEND *req);
ST_VOID _mvl_process_ident_conf (MVL_REQ_PEND *req);
ST_VOID _mvl_process_getvar_conf (MVL_REQ_PEND *req);
ST_VOID _mvl_process_getnam_conf (MVL_REQ_PEND *req);
ST_VOID _mvl_process_status_conf (MVL_REQ_PEND *req);
ST_VOID _mvl_process_getdom_conf (MVL_REQ_PEND *req);
ST_VOID _mvl_process_fopen_conf (MVL_REQ_PEND *req);
ST_VOID _mvl_process_fread_conf (MVL_REQ_PEND *req);
ST_VOID _mvl_process_fclose_conf (MVL_REQ_PEND *req);
ST_VOID _mvl_process_fdir_conf (MVL_REQ_PEND *req);
ST_VOID _mvl_process_obtfile_conf (MVL_REQ_PEND *req);
ST_VOID _mvl_process_fdelete_conf (MVL_REQ_PEND *req);
ST_VOID _mvl_process_frename_conf (MVL_REQ_PEND *req);
ST_VOID _mvl_process_jinit_conf (MVL_REQ_PEND *req);
ST_VOID _mvl_process_jread_conf (MVL_REQ_PEND *req);
ST_VOID _mvl_process_jwrite_conf (MVL_REQ_PEND *req);
ST_VOID _mvl_process_jstat_conf (MVL_REQ_PEND *req);
ST_VOID _mvl_process_defvlist_conf (MVL_REQ_PEND *req);
ST_VOID _mvl_process_getvlist_conf (MVL_REQ_PEND *req);
ST_VOID _mvl_process_delvlist_conf (MVL_REQ_PEND *req);
ST_VOID _mvl_process_initdown_conf (MVL_REQ_PEND *req);
ST_VOID _mvl_process_download_conf (MVL_REQ_PEND *req);
ST_VOID _mvl_process_termdown_conf (MVL_REQ_PEND *req);
ST_VOID _mvl_process_initupl_conf (MVL_REQ_PEND *req);
ST_VOID _mvl_process_upload_conf (MVL_REQ_PEND *req);
ST_VOID _mvl_process_termupl_conf (MVL_REQ_PEND *req);
ST_VOID _mvl_process_deldom_conf (MVL_REQ_PEND *req);
ST_VOID _mvl_process_crepi_conf (MVL_REQ_PEND *req);
ST_VOID _mvl_process_delpi_conf (MVL_REQ_PEND *req);
ST_VOID _mvl_process_start_conf (MVL_REQ_PEND *req);
ST_VOID _mvl_process_stop_conf (MVL_REQ_PEND *req);
ST_VOID _mvl_process_resume_conf (MVL_REQ_PEND *req);
ST_VOID _mvl_process_reset_conf (MVL_REQ_PEND *req);
ST_VOID _mvl_process_kill_conf (MVL_REQ_PEND *req);
ST_VOID _mvl_process_getpi_conf (MVL_REQ_PEND *req);
ST_VOID _mvl_process_getcl_conf (MVL_REQ_PEND *req);
ST_VOID _mvl_process_concl_conf (MVL_REQ_PEND *req);
ST_VOID _mvl_release (MVL_REQ_PEND *req);
				
/************************************************************************/

ST_VOID mvl_ind_rcvd (MVL_COMM_EVENT *event);
ST_VOID mvl_conf_rcvd (MVL_COMM_EVENT *event);
ST_VOID mvl_cancel_ind_rcvd (MVL_COMM_EVENT *event);
ST_VOID mvl_concl_ind_rcvd (MVL_COMM_EVENT *event);
ST_VOID mvl_concl_conf_rcvd (MVL_REQ_PEND *req_pend);

ST_VOID _mvl_ind_reject (MVL_IND_PEND *ind);
ST_VOID _mvl_conf_reject (MVL_REQ_PEND *req);
ST_VOID _mvl_send_reject (MVL_COMM_EVENT *event);
ST_RET _mvl_send_error (MVL_COMM_EVENT *event, ST_INT16 eClass, ST_INT16 code);
ST_RET _mvl_send_err_f (MVL_COMM_EVENT *event, ST_INT16 eClass, ST_INT16 code);

#define mplas_err_resp(indCtrl, eClass, code) _mplas_err_resp(indCtrl, eClass, code)
ST_RET _mplas_err_resp (MVL_IND_PEND *indCtrl, ST_INT16 eClass, ST_INT16 code);

ST_VOID _mvl_send_resp (MVL_COMM_EVENT *event, ST_RET build_rc);
ST_VOID _mvl_send_resp_i (MVL_IND_PEND *indCtrl, ST_RET build_rc);
ST_RET _mvl_send_req (MVL_NET_INFO *net_info, 
		     MVL_REQ_PEND *req, ST_RET build_rc);

MVL_VAR_ASSOC *_mvl_objname_to_va (MVL_VMD_CTRL *vmd_ctrl,
                                 MVL_NET_INFO *net_info,
                                 ST_INT service,
				 OBJECT_NAME *obj, 
				 MVL_SCOPE *va_scope_out,
		      		 ST_BOOLEAN alt_access_pres,
		      		 ALT_ACCESS *alt_acc,
		      		 ST_BOOLEAN *alt_access_done_out);

/* This function does everything required to send a MMS message -	*/
/* adds CLNP/CLTP/HDLC components then does whatever is required to send*/
/* the complete message. Does not return until done, returns SD_SUCCESS	*/
/* or an error code.							*/
ST_RET _mvl_send_msg (ST_UCHAR *mms_msg, ST_INT mms_msg_len, MVL_NET_INFO *net_info);


/* This function is called to get MMS PDU's that have been received,	*/
/* as well as other communications events. Returns a pointer to a 	*/
/* MMS Comm Event control structure that must be free'd via the		*/
ST_BOOLEAN _mvl_net_service (MVL_COMM_EVENT **eventOut);
ST_VOID _mvl_free_comm_event (MVL_COMM_EVENT *event);

extern ST_BOOLEAN mvl_init_ind_hold;

/* Functions to manipulate request control list				*/
MVL_REQ_PEND *_mvl_get_req_ctrl (MVL_NET_INFO *net_info, ST_INT16 op);
MVL_REQ_PEND *_mvl_find_req_ctrl (MVL_NET_INFO *net_info, 
				  ST_INT op, ST_UINT32 id);
ST_VOID _mvl_set_net_req_done (MVL_REQ_PEND *skip, 
			       MVL_NET_INFO *net_info, ST_RET result);
ST_VOID _mvl_set_req_done (MVL_REQ_PEND *req, ST_RET result);

/* Alternate Access creation buffer management */
ST_RET _mvl_get_asn1_aa (ALT_ACCESS *alt_acc, VARIABLE_LIST *vl);

ST_RET _mvl_concat_filename (ST_CHAR *good_name, ST_INT num_of_fname,
			     FILE_NAME *fname_arr, ST_INT max_len);

ST_VOID mvl_log_req (ST_INT opcode, ST_VOID *info);
ST_VOID mvl_log_resp (ST_INT opcode, ST_VOID *info);

ST_VOID mvl_scope_set (OBJECT_NAME *obj, MVL_SCOPE *mvl_scope_out);


/************************************************************************/

extern ST_BOOLEAN _mvl_comm_serve_active;

/************************************************************************/
/* 			MVL_OBJ (generic MVL object)			*/
/* This structure is used by all the MVL object manipulation functions	*/
/* (mvl_obj_insert, mvl_obj_delete, mvl_obj_find, etc.) to represent	*/
/* any named object (i.e. Named Variable, Named Variable List,		*/
/* Domain, Journal, etc.).						*/
/* A pointer to a "real" structure must be cast to (MVL_OBJ *)		*/
/* for any call to the functions.					*/
/* WARNING: It is assumed that the first item in the "real" structure	*/
/* is a pointer to the "name" of the object.				*/
/************************************************************************/

typedef struct
  {
  ST_CHAR *name;
  } MVL_OBJ;

/************************************************************************/

ST_VOID *_mvl_calloc_os (ST_UINT num, ST_UINT size);
ST_VOID *_mvl_realloc_os (ST_VOID *old, ST_UINT new_size);
ST_VOID  _mvl_free_os (ST_VOID *buf);

/************************************************************************/
/* MVL object manipulation function prototypes.				*/
/************************************************************************/

ST_RET mvl_obj_insert (MVL_OBJ **ObjTable, ST_INT *numObj, ST_INT maxObj,
		MVL_OBJ *newObj);
MVL_OBJ *mvl_obj_delete    (MVL_OBJ **ObjTable, ST_INT *numObj,
		ST_CHAR *ObjName);
MVL_OBJ *mvl_obj_find      (MVL_OBJ **ObjTable, ST_INT numObj,
		ST_CHAR *ObjName);
MVL_OBJ ** mvl_obj_find_key (MVL_OBJ **ObjTable, ST_INT numObj,
		ST_CHAR *ObjName);
ST_VOID _mvl_resize_ptr_tbl (ST_INT *p_max_num, 
			     ST_VOID ***p_ptr_tbl, ST_INT new_size);
/************************************************************************/
/* MVL type derivation functions					*/
/************************************************************************/

ST_RET  mvl_derive_new_type (ST_CHAR *base_name, ST_INT typeIdIn, 
		ST_INT *typeIdOut, ST_CHAR *handle);
ST_RET	mvl_derive_type_members (ST_CHAR *base_name, ST_INT typeIdIn, 
		ST_UCHAR **membership, ST_RTREF *refs, ST_CHAR *handle);
ST_VOID	mvl_print_derived_type( ST_CHAR *base_name, ST_INT typeIdIn, 
                                ST_UCHAR *membership, ST_UCHAR **labels);
ST_RET	mvl_add_derived_type( ST_CHAR *base_name, ST_INT typeIdIn, 
                              ST_UCHAR *membership, ST_RTREF *refs,
			      ST_INT *typeIdOut);

ST_BOOLEAN u_mvl_rt_element_supported( RUNTIME_TYPE *rt, 
                                       ST_CHAR *element_name, 
				       ST_RTREF *ui, ST_CHAR *handle);

/************************************************************************/

ST_INT mi_connect_ind_fun (MVL_NET_INFO *net_info, 
			   INIT_INFO *init_ind_info,
			   ACSE_AUTH_INFO *rsp_auth_info);
ST_VOID mi_release_ind_fun (MVL_NET_INFO *mvl_net_info);
ST_VOID mi_abort_ind_fun (MVL_NET_INFO *mvl_net_info);

ST_VOID _mvl_free_req_pend (ST_VOID);
ST_VOID u_mvl_reject_ind (MVL_NET_INFO *net_info, REJECT_RESP_INFO *rej_info);

/************************************************************************/
/* Dynamic type creation functions.					*/
/************************************************************************/
MVL_TYPE_CTRL *mvl_type_ctrl_find (ST_INT TypeId);
MVL_TYPE_CTRL *mvl_type_ctrl_find_dyn (ST_INT TypeId);
ST_INT mvl_type_id_create (ST_CHAR *type_name, ST_UCHAR *asn1_data, ST_UINT asn1_len);
ST_INT mvl_type_id_create_from_tdl (ST_CHAR *type_name, ST_CHAR *tdl);
ST_VOID mvl_type_id_destroy (ST_INT TypeId);
ST_RET mvl_type_id_destroy_all_dyn (ST_VOID);
ST_INT mvl_type_count (ST_VOID);

/* These take additional "vmd_ctrl" argument.	*/
ST_INT mvl_vmd_type_id_create (MVL_VMD_CTRL *vmd_ctrl, ST_CHAR *type_name, RUNTIME_CTRL *rt_ctrl);
ST_RET mvl_vmd_type_id_destroy (MVL_VMD_CTRL *vmd_ctrl, ST_INT TypeId);
ST_RET mvl_vmd_type_id_destroy_all (MVL_VMD_CTRL *vmd_ctrl);
MVL_TYPE_CTRL *mvl_vmd_type_ctrl_find (MVL_VMD_CTRL *vmd_ctrl, ST_INT type_id);

#define mvlu_install()	/* OBSOLETE: For backward compatibility only	*/

/************************************************************************/
/* Allocation functions/macros.						*/
/************************************************************************/
#define mvl_enc_buf_alloc(size)	M_MALLOC (MSMEM_PDU_ENC, size)
#define mvl_enc_buf_free(ptr)	M_FREE (MSMEM_PDU_ENC, ptr)

/************************************************************************/
/* Miscellaneous functions.						*/
/************************************************************************/
ST_VOID wait_any_event (ST_LONG max_ms_delay);	/* called by sync fcts	*/
						/* user MUST supply this fct*/
ST_INT mvl_req_bufs_avail (MVL_NET_INFO *net_info);
ST_INT mvl_ureq_bufs_avail (MVL_NET_INFO *net_info);

/************************************************************************/
/* Object add/remove functions.						*/
/************************************************************************/
MVL_DOM_CTRL *mvl_vmd_dom_add (MVL_VMD_CTRL *vmd_ctrl,
	ST_CHAR *name,
	ST_INT max_num_var, 
	ST_INT max_num_nvl,
	ST_INT max_num_jou,
	ST_BOOLEAN copy_name);
ST_RET mvl_vmd_dom_remove (MVL_VMD_CTRL *vmd_ctrl, ST_CHAR *dom_name);

MVL_VAR_ASSOC *mvl_vmd_var_add (MVL_VMD_CTRL *vmd_ctrl,
	OBJECT_NAME *obj,
	MVL_NET_INFO *net_info,
	ST_INT type_id,
	ST_VOID *data, 
	MVL_VAR_PROC *proc,
	ST_BOOLEAN copy_name);
MVL_VAR_ASSOC *mvl_var_add_alloc (MVL_VMD_CTRL *vmd_ctrl,
	OBJECT_NAME *obj,
	MVL_NET_INFO *net_info,
	ST_INT type_id,
	MVL_VAR_PROC *proc);
ST_RET mvl_vmd_var_remove (MVL_VMD_CTRL *vmd_ctrl,
	OBJECT_NAME *obj,
	MVL_NET_INFO *net_info);

MVL_NVLIST_CTRL *mvl_vmd_nvl_add (MVL_VMD_CTRL *vmd_ctrl,
	OBJECT_NAME *nvl_obj,
	MVL_NET_INFO *net_info,
	ST_INT num_var,
	OBJECT_NAME *var_obj,	/* array of var names	*/
	ST_BOOLEAN copy_name);
MVL_NVLIST_CTRL *mvl_vmd_nvl_add_aa (MVL_VMD_CTRL *vmd_ctrl,
	OBJECT_NAME *nvl_obj,
	MVL_NET_INFO *net_info,
	ST_INT num_var,
	OBJECT_NAME *var_obj,	/* array of var names	*/
	ALT_ACCESS **alt_acc);	/* array of alt acc info for each var*/
				/* NULL if not present	*/
ST_RET mvl_vmd_nvl_remove (MVL_VMD_CTRL *vmd_ctrl,
	OBJECT_NAME *obj,
	MVL_NET_INFO *net_info);

MVL_JOURNAL_CTRL *mvl_vmd_jou_add (MVL_VMD_CTRL *vmd_ctrl,
	OBJECT_NAME *obj,
	MVL_NET_INFO *net_info,
	ST_BOOLEAN copy_name);
ST_RET mvl_vmd_jou_remove (MVL_VMD_CTRL *vmd_ctrl,
	OBJECT_NAME *obj,
	MVL_NET_INFO *net_info);

/************************************************************************/
/* Object insert/delete functions.					*/
/************************************************************************/
ST_RET mvl_vmd_dom_insert (MVL_VMD_CTRL *vmd_ctrl,
	MVL_DOM_CTRL *dom);
MVL_DOM_CTRL *mvl_vmd_dom_delete (MVL_VMD_CTRL *vmd_ctrl, ST_CHAR *dom_name);

ST_RET mvl_vmd_var_insert (MVL_VMD_CTRL *vmd_ctrl,
	MVL_VAR_ASSOC *va,
	OBJECT_NAME *obj,
	MVL_NET_INFO *net_info);	/* Only used if AA_SPEC	*/
MVL_VAR_ASSOC *mvl_vmd_var_delete (MVL_VMD_CTRL *vmd_ctrl,
	OBJECT_NAME *obj,
	MVL_NET_INFO *net_info);

ST_RET mvl_vmd_nvl_insert (MVL_VMD_CTRL *vmd_ctrl,
	MVL_NVLIST_CTRL *nvl,
	OBJECT_NAME *obj,
	MVL_NET_INFO *net_info);	/* Only used if AA_SPEC	*/
MVL_NVLIST_CTRL *mvl_vmd_nvl_delete (MVL_VMD_CTRL *vmd_ctrl,
	OBJECT_NAME *obj,
	MVL_NET_INFO *net_info);	/* Only used if AA_SPEC	*/

ST_RET mvl_vmd_jou_insert (MVL_VMD_CTRL *vmd_ctrl,
	MVL_JOURNAL_CTRL *jou,
	OBJECT_NAME *obj,
	MVL_NET_INFO *net_info);	/* Only used if AA_SPEC	*/
MVL_JOURNAL_CTRL *mvl_vmd_jou_delete (MVL_VMD_CTRL *vmd_ctrl,
	OBJECT_NAME *obj,
	MVL_NET_INFO *net_info);


/************************************************************************/
/* Object "find" functions.						*/
/************************************************************************/
MVL_DOM_CTRL *mvl_vmd_find_dom (MVL_VMD_CTRL *vmd_ctrl,
	ST_CHAR *name);
MVL_DOM_CTRL *mvl_vmd_dom_find_last (MVL_VMD_CTRL *vmd_ctrl);
MVL_VAR_ASSOC *mvl_vmd_find_var (MVL_VMD_CTRL *vmd_ctrl,
	OBJECT_NAME *obj,
	MVL_NET_INFO *net_info);	/* Only used if AA_SPEC	*/
MVL_NVLIST_CTRL *mvl_vmd_find_nvl (MVL_VMD_CTRL *vmd_ctrl,
	OBJECT_NAME *obj,
	MVL_NET_INFO *net_info);	/* Only used if AA_SPEC	*/
MVL_JOURNAL_CTRL *mvl_vmd_find_jou (MVL_VMD_CTRL *vmd_ctrl,
	OBJECT_NAME *obj,
	MVL_NET_INFO *net_info);	/* Only used if AA_SPEC	*/

/************************************************************************/
/* Global variables.							*/
/************************************************************************/

/* This function pointer called to customize Write Indication processing.*/
extern ST_VOID (*u_mvl_write_ind_custom) (MVL_IND_PEND *indCtrl);

/************************************************************************/
#ifdef __cplusplus
}
#endif

#endif /* MVL_DEFS_INCLUDED */
/************************************************************************/

