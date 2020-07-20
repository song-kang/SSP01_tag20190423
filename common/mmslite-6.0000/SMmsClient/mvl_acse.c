/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	1986 - 2010, All Rights Reserved		        */
/*									*/
/* MODULE NAME : mvl_acse.c    						*/
/* PRODUCT(S)  : MMSEASE-LITE						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*	NONE								*/
/*									*/
/* NOTE: When "SISCO_STACK_CFG" is defined, there is only one DIB table	*/
/*   with local and remote addresses. It is a Linked List (not array).	*/
/*   The function "find_dib_entry" is used to find entries instead of	*/
/*   "find_loc_dib_entry" and "find_rem_dib_entry".			*/
/*   Initialization is quite different.					*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 02/15/12  JRB	   Add USE_DIB_LIST option.			*/
/*			   Free file_list on Abort indication.		*/
/* 09/09/11  JRB	   Move loc_dib_table, etc globals to lean_var.c*/
/* 08/23/11  EJV           Added mvla_initiate_req_exx with locMaceCert.*/
/*			   Added macro for mvla_initiate_req_ex.	*/
/* 03/31/11  JRB	   mvl_end_acse: free any events on list.	*/
/* 10/28/10  EJV	   Updated S_SEC_ENABLED code.			*/
/* 10/05/10  JRB	   u_a_associate_ind: Init reinfo.user_info.ptr	*/
/* 06/17/10  JRB	   Del calls to clpp_initialize, clpp_terminate	*/
/*			   (now user must call clnp_init, clnp_end).	*/
/*			   find_*_dib_entry: return NULL if arg is NULL.*/
/* 03/03/10  JRB	   Replace "ml_log_*" with "mlogl_info" struct.	*/
/* 02/24/10  JRB	   Del _mms_dec_info, use "rslt.data_ptr"	*/
/* 01/20/10  JRB    112    Chg all bind_id, conn_id to (ST_VOID *).	*/
/*			   Bind/unbind only if "called" conns configured*/
/*			   Add #if defined(SISCO_STACK_CFG).		*/
/*			   mvla_initiate_req_ex: add locAr arg.		*/
/*			   Del mvl_initiate_req, mvla_initiate_req.	*/
/*			   Use modified MVL_CFG_INFO, NET_BIND_CTRL.	*/
/*			   Del MAP30_ACSE, REDUCED_STACK, MMSEASE_MOSI.	*/
/*			   Del _mvla_initiate_locDe, _mvla_initiate_bc.	*/
/*			   Del mvla_set_init_ar function.		*/
/*			   ma_bind: del unused activate arg, add timeout*/
/*			     Do NOT reserve conns for each bind		*/
/*			     (all conns are available for any bind).	*/
/*			   copp_unbind_sync: change args.		*/
/*			   Del unused mvl_get(free)_net_info.		*/
/*			   Del unused ma_clbind_ctrl.			*/
/*			   Chg return to break if bind fails so cleanup done*/
/*			   Del ma_wait_req_done. Use new ma_bind_wait.	*/
/*			   Del bind_id arg to a_associate_req & set	*/
/*			     aarq_apdu.calling_paddr before calling it.	*/
/*			   Added mvl_dib_entry_2_to_app_ref.		*/
/*			   Del mvl_match*, mvl_cmp* (use dib_* directly)*/
/*			   Alloc bind_ctrl, free in mvl_end_acse.	*/
/*			   Fix log messages.				*/
/* 02/13/09  JRB    111    If received PDU len=0, send immediate reject.*/
/* 03/06/08  JRB    110    Del all OBSOLETE_AA_OBJ_INIT code.		*/
/*			   mvl_end_acse: free all conn ctrl structs.	*/
/* 02/20/07  EJV    109    sSecFreeSecParam only for MMS-LITE-SECURE.	*/
/* 02/01/07  EJV    108    u_a_associate_ind: check for S_SEC_ENABLED	*/
/* 01/26/07  EJV    107    u_a_associate_ind fix: free auth (MACE) buff.*/
/* 01/30/06  GLB    106    Integrated porting changes for VMS           */
/* 10/20/05  EJV    105    u_a_associate_ind:free _mms_dec_info on return*/
/* 07/13/05  MDE    104    Fixed crash caused using mvl_init_ind_hold	*/
/* 06/07/05  MDE    103    Fixed deadlock caused by mvl_init_ind_hold	*/
/* 05/10/05  JRB    102    Call new mvl_init_aa_obj_ctrl to init struct	*/
/*			   to control AA-Specific object creation	*/
/*			   (no longer done by Foundry).			*/
/* 05/04/05  MDE    101    Added mvl_init_ind_hold			*/
/* 04/28/05  MDE    100    Added user_conn_id check on abort ind	*/
/* 04/06/05  MDE     99    Added test probes				*/
/* 03/09/05  MDE     98    Set stats startTime				*/
/* 08/11/04  JRB     97    Del global funct ptr u_mvl_connect_ind_fun.	*/
/*			   Fix mvl_abort_req(_ex) returns (ST_RET).	*/
/* 07/14/04  MDE     96    Set mvl_net_info->acse_conn_id on init req	*/
/* 07/08/04  JRB     95    Del unused global var mvl_local_cl_ar_name.	*/
/* 06/07/04  EJV     94	   Removed call to a_free_security_info		*/
/* 04/09/04  MDE     93    Removed call to a_free_security_info		*/
/* 01/21/04  EJV     92    u_a_associate_ind: added a_free_security_info*/
/*			     added reinfo.auth_info.auth_pres = SD_FALSE*/
/* 01/20/04  MDE     91    Added logging				*/
/* 11/07/03  JRB     90    Add mmsop_en_version1.			*/
/* 08/10/03  EJV     89    ACSE_AUTH_ENABLED code wo/defines		*/
/*			   Reworked mvl_net_info_to_chan.		*/
/*			   Removed defs from around find_rem_dib_entry()*/
/*			   For MAP30_ACSE use a_get_chan().		*/
/* 07/02/03  JRB     88    _mvl_net_service set activityFlag if more	*/
/*			   events on list (so user keeps calling).	*/
/* 06/16/03  EJV     87    Removed m_match.h, s_match.h includes.	*/
/*			   Renamed the match funcs & chg arg.		*/
/* 06/10/03  MDE     86    Added address matching			*/
/* 04/14/03  JRB     85    Eliminate compiler warnings.			*/
/* 12/18/02  ASK     84    Only check authentication on POSITIVE connect*/
/*			   confirm					*/
/* 10/30/02  JRB     83    mvl_init_audt_addr: check remote tp_type.	*/
/*			   _mvl_send_msg: do lock around a_data_req	*/
/*			   call to make it thread-safe.			*/
/*			   Chg all connID, bindID to ST_LONG to avoid	*/
/*			   compile errors on 64-bit systems.		*/
/* 10/11/02  ASK     82    Added mvla_initiate_req_ex, 			*/
/*			   u_mvl_connect_ind_ex, and 			*/
/*			   u_mvl_connect_cnf_ex functions for ACSE 	*/
/*			   authentication.				*/
/* 07/19/02  ASK     81    Fix net_info variable in _mvl_send_msg	*/
/* 07/17/02  MDE     80    Added mvl_conn_filtered_logging to send	*/
/* 07/10/02  JRB     79    Add maxpend_ind, numpend_ind.		*/
/* 07/09/02  MDE     78    Fixed bad free for bad decode of init resp	*/
/* 05/24/02  JRB     77    Put back MVL_COMM_EVENT linked list.		*/
/*			   Occasionally copp_event gets 2 events, one	*/
/*			   from TP0 and one from TP4.			*/
/* 05/17/02  JRB     76    mvla_init..: set *req_out=NULL if freed	*/
/*			   so user CAN'T free it again.			*/
/* 05/13/02  MDE     75    Don't scan unbound mvl_net_info		*/
/* 05/13/02  MDE     74    Now clean up requests for release conf	*/
/* 05/03/02  MDE     73    Added mvl_add_bind_ctrl, associated changes	*/
/* 04/29/02  MDE     72    Added MLOG for initiate			*/
/* 04/29/02  JRB     71    DON'T re-use MVL_NET_INFO until req_pend_list*/
/*			   is empty, assert if pend_ind is not empty.	*/
/* 03/07/02  MDE     70    mvla_set_init_ar now takes NULL		*/
/* 03/04/02  JRB     69    Start with clean "temp_bind_ctrl" so garbage	*/
/*			   doesn't get copied. 				*/
/*			   Return error if num_call*>0 but not supported*/
/* 02/25/02  MDE     68    More changes to MVL_CFG_INFO			*/
/* 02/20/02  MDE     67    Changes for new MVL_CFG_INFO, multiple bind	*/
/* 02/14/02  RKR     66    call a_associate_reqm if Marben stack	*/
/* 02/08/02  JRB     65	   Pass MVL_CFG_INFO struct to mvl_start_acse.	*/
/*			   Del mvl_local_ar_name,mvl_set_num_connections*/
/* 01/30/02  JRB     64    Add args to copp_initialize (same as MAP30..)*/
/*			   Del call to a_set_msgsize.			*/
/*			   Make sure mmsl_max_msg_size, mvl_num_calling,*/
/*			   mvl_num_called are set.			*/
/*			   Use MMS_INIT_EN instead of MVL_NUM_CALL*.	*/
/*			   Del use of COACSE define.			*/
/*			   Del mvl_max_comm_event (not used).		*/
/*			   Merge mvl_init, _mvl_init_conn_ctrl into	*/
/*			   mvl_start_acse.				*/
/*			   Replace mvl_init_cl_netinfo funct with	*/
/*			   mvl_init_audt_addr.				*/
/*			   Del u_a_unit_data_ind & all use of stack_sel.*/
/* 01/09/02  JRB     63    Del all use of clpp_bind & bind id.		*/
/*			   Del unused u_cla_except callback function.	*/
/* 12/04/01  MDE     62    Change to work with MAP30_ACSE		*/
/* 11/30/01  GLB     61    changed MVL_LOG_ERR1 to MVL_LOG_ERR0         */
/* 11/28/01  MDE     60    mvl_set_num_connections,mvl_set_max_msg_size */
/* 11/21/01  MDE     59    mvl_loc_ar_name now auto set			*/
/* 11/21/01  MDE     58    Safer num channel operation			*/
/* 11/05/01  JRB     57    mvl_wait_req_done now returns ST_RET.	*/
/* 09/21/01  JRB     56    Alloc global bufs only once at startup.	*/
/*			   Fix M_FREE context.				*/
/* 07/31/01  MDE     55    Added mvl_conn_filtered_logging		*/
/* 05/21/01  MDE     54    Free NET_INFO				*/
/* 03/13/01  JRB     53    Check for (MMS_INIT_EN & REQ_EN).		*/
/* 02/26/01  MDE     52    Moved m_smem_ctxt to mem_chks.c      	*/
/* 11/28/00  EJV     51    Changes to interface with Marben stack SUIC	*/
/*                         (see MAP30_ACSE define):			*/
/*                         Use SUIC find_loc_..., find_rem_dib_entry.	*/
/*			   Added   param  in copp_initialize().		*/
/*			   Added   param  in mvla_initiate_req().	*/
/*			   Changed params in copp_bind().		*/
/* 10/25/00  JRB     50    Del MVL_COMM_EVENT linked list. Only one used*/
/*			   Del call to _mvl_free_req_pend.		*/
/* 09/22/00  MDE     49    Changed index init				*/
/* 08/21/00  RKR     48    Changed prototype of mvl_init_cl_netinfo 	*/
/* 08/23/00  MDE     47    Call '_mvl_set_net_req_done' on aborts too	*/
/* 05/15/00  MDE     46    Added MMS statistics				*/
/* 04/27/00  JRB     45    Fixed calloc (0) problem			*/
/* 04/14/00  JRB     44    Lint cleanup.				*/
/* 04/12/00  MDE     43    Added AA resize, MVL_STATISTICS		*/
/* 03/15/00  JRB     42    Del #ifdef MVL_REQ_BUF_ENABLED: not used.	*/
/* 03/09/00  MDE     41    Added _mvl_init_conn_ctrl			*/
/* 01/21/00  MDE     40    Now use MEM_SMEM for dynamic memory		*/
/* 12/20/99  GLB     39    Remove extra initialization and add asserts  */
/* 11/23/99  MDE     38    Added mvla_check_req_que (from mvl_serv.c)	*/
/* 10/20/99  NAV     37    Add maxpend_req support			*/
/* 11/03/99  RKR     36    Set aborted pend req results to DISCONN	*/
/* 10/29/99  GLB     35    Added check for lack of connection           */
/* 09/07/99  MDE     34    Added _mvlu_rpt_disconnect_rcvd_fun 		*/
/* 07/30/99  JRB     33    Added MVL_GOOSE_SUPPORT define use		*/
/*			   Chg ma_clbind_ctrl from static to global.	*/
/* 07/14/99  JRB     32    On assoc ind, save acse_conn_id first.	*/
/* 04/13/99  MDE     31    Added '_mvla_initiate_locDe'			*/
/* 04/07/99  MDE     30    Fixed mvla_initiate_req return code		*/
/* 04/01/99  MDE     29    Eliminated connection handling memory leaks	*/
/* 04/01/99  MDE     28    Changes to decode buffer allocation scheme   */
/* 03/18/99  MDE     27    Made # connection limits changable runtime 	*/
/* 03/08/99  MDE     26    Added ICCP support				*/
/* 12/08/98  MDE     25    Cleanup on request control handling		*/
/* 12/08/98  MDE     24    Allow user to initiate w/o supplying a 	*/
/*			   'resp_info' buffer.				*/
/* 11/16/98  MDE     23    Conclude/Release cleanup, misc cleanup	*/
/* 10/22/98  JRB     22    mvl_start_acse init mvl_debug_sel. Other lint*/
/* 09/21/98  MDE     21    Minor lint cleanup				*/
/* 08/25/98  MDE     20    Fixed bind/unbind bug for CLACSE		*/
/* 08/14/98  MDE     19    Removed !ALLOW_COMPLEX_NET_EVENTS code	*/
/* 08/14/98  MDE     18    Updated 'u_a_data_ind' parameters as reqd	*/
/* 07/20/98  JRB     17    If MVL_NUM_CALLED not defined, reject all	*/
/*			   associate indications.			*/
/* 07/16/98  MDE     16    Addded 'mvl_dib_entry_to_app_ref'		*/
/* 07/06/98  JRB     15    mvl_init_cl_netinfo use new DIB_ENTRY and	*/
/*			   init rem_mac_valid so it sends to ALL-ES.	*/
/* 06/18/98  JRB     14    Bind to TP4 and TCP if tp_type=TP_TYPE_TPX.	*/
/* 02/10/98  MDE     13    Added 'index' to connection control.		*/
/* 12/29/97  JRB     12    Call c?pp_terminate if bind fails.		*/
/* 12/22/97  MDE     11    Changed '_mvl_net_service' to report activity	*/
/* 09/18/97  MDE     10    Now call 'a_set_msgsize'			*/
/* 02/07/97  MDE     09    Changed to use new MMS-EASE opcode defines	*/
/* 12/13/96  MDE     08    Now support CLACSE, other enhancements	*/
/* 11/18/96  MDE     07    Minor name cleanup, some CLACSE stuff	*/
/* 11/15/96  MDE     06    Global variable work				*/
/* 11/11/96  MDE     05    Added async functions, other cleanup		*/
/* 10/31/96  MDE     04    Added ALLOW_COMPLEX_NET_EVENTS code		*/
/* 08/19/96  MDE     03    Now save all remote node's initiate info	*/
/* 07/22/96  MDE     02    Changes towards A-Unit-Data support		*/
/* 01/21/94  MDE     01    Initial                       		*/
/************************************************************************/
#include <assert.h>

#include "glbtypes.h"
#include "sysincs.h"
#include "glbsem.h"

#include "mmsdefs.h"
#include "mms_pvar.h"
#include "mms_vvar.h"
#include "mms_err.h"
#include "mms_pcon.h"
#include "asn1defs.h"
#include "slog.h"
#include "tp4api.h"
#include "mvl_acse.h"
#include "mvl_defs.h"
#include "mvl_log.h"
#include "mloguser.h"
#include "mmsop_en.h"

#include "acse2.h"

#if defined(SISCO_STACK_CFG)
#include "sstackcfg.h"
#endif

#if defined(_S_TEST_PROBES)
#include "stestprb.h"
#endif

#if defined(OBSOLETE_AA_OBJ_INIT)
#error OBSOLETE_AA_OBJ_INIT must NOT be defined. Code that was enabled by it has been deleted.
/* NOTE: _mvl_clear_aa_objs and other code would never work if this defined.*/
#endif

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/
/************************************************************************/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/

MVL_MMS_STATISTICS mvl_mms_statistics;

/************************************************************************/

#define NEGIOTIATE_INITIATE_PARAM
#define MVL_MMSOP_RELEASE 512	/* Add a MMS opcode define ... */

/************************************************************************/
/* Global variables							*/
/************************************************************************/

/* Our connection controls - we support multiple called and/or calling	*/
/* simultanious connections.						*/

MVL_NET_INFO *mvl_calling_conn_ctrl;
MVL_NET_INFO *mvl_called_conn_ctrl;

/* Other user configurable parameters					*/
#ifndef MMS_INIT_EN
#error MMS_INIT_EN not defined. Must include mmsop_en.h
#endif
#if (MMS_INIT_EN & RESP_EN) 
INIT_INFO *mvl_init_resp_info;
#endif

ST_VOID (*u_mvl_disc_ind_fun) (MVL_NET_INFO *cc, ST_INT discType);
ST_VOID (*_mvlu_rpt_disconnect_rcvd_fun)(MVL_NET_INFO *netInfo);

/************************************************************************/
/* This variable is set SD_TRUE when the ACSE interface is viable		*/
ST_INT mvl_acse_ok;

/************************************************************************/
/* These arrays are created in mmsop_en.c */
extern SD_CONST ST_UCHAR  m_param[2];
extern SD_CONST ST_UCHAR  m_service_resp[11];

/************************************************************************/
/************************************************************************/
/* Bind Control								*/

ST_INT mvl_bind_timeout = 10;	/* bind/unbind timeout in seconds	*/

/************************************************************************/

MVL_CFG_INFO *mvl_cfg_info;

/************************************************************************/
/* MMS COMM EVENT CONTROL MANAGEMENT */

static MVL_COMM_EVENT *mvl_comm_event_up_head;	/* ptr to linked list	*/

/************************************************************************/

typedef struct
  {
  DBL_LNK l;
  ST_VOID *u_bind_id;
  ST_VOID *acse_conn_id; 
  AARQ_APDU *info;
  } MVL_ASSOC_IND;
MVL_ASSOC_IND *mvlAssocIndList;

ST_BOOLEAN mvl_init_ind_hold;

/************************************************************************/

/* Internal functions */
#if (MMS_INIT_EN & RESP_EN)
static MVL_NET_INFO *ma_get_called_conn_ctrl (NET_BIND_CTRL *);
#endif
#if (MMS_INIT_EN & REQ_EN)
static MVL_NET_INFO *ma_get_calling_conn_ctrl (ST_VOID);
#endif
static ST_VOID copp_unbind_sync (NET_BIND_CTRL *bc, ST_INT timeout);
static ST_RET ma_bind (NET_BIND_CTRL *bc, ST_INT timeout);
static ST_RET ma_bind_wait (NET_BIND_CTRL *bc, ST_INT timeout);

static MVL_COMM_EVENT *mvl_get_com_event (ST_VOID);

/************************************************************************/
/************************************************************************/
/*                       _mvl_net_service				*/
/************************************************************************/

ST_BOOLEAN _mvl_net_service (MVL_COMM_EVENT **eventOut)
  {
ST_BOOLEAN activityFlag;
MVL_ASSOC_IND *mvlAssocInd;

  *eventOut = NULL;
  if (mvl_acse_ok != SD_TRUE)
    return (SD_FALSE);

  S_LOCK_COMMON_RESOURCES ();

/* If we have a delayed initiate indication (ICCP Lite redundancy),	*/
/* handle it now 							*/
  if (mvlAssocIndList && !mvl_init_ind_hold)
    {
    mvlAssocInd = (MVL_ASSOC_IND *) list_get_first (&mvlAssocIndList);
    u_a_associate_ind (mvlAssocInd->u_bind_id, 
    		       mvlAssocInd->acse_conn_id, 
    		       mvlAssocInd->info);
    chk_free (mvlAssocInd);
    S_UNLOCK_COMMON_RESOURCES ();
    return (SD_TRUE);
    }

/* May cause the 'acse' user function to be called if something is	*/
/* going on that we need to know about. If so, the functions will	*/
/* get the event and put them onto the 'mvl_comm_event_up_head' list	*/

  activityFlag = copp_event ();
  /* Occasionally copp_event gets 2 events, one from TP0 and one from TP4,*/
  /* so they are put on this linked list. "Usually" only one on list.	*/
  if (mvl_comm_event_up_head)
    *eventOut = (MVL_COMM_EVENT *) list_get_first((ST_VOID **) &mvl_comm_event_up_head);

  if (mvl_comm_event_up_head != NULL)
    {
    /* more events on linked list	*/
    activityFlag=SD_TRUE;	/* force it so user knows to call again	*/
    }

  S_UNLOCK_COMMON_RESOURCES ();
  return (activityFlag);
  }

/************************************************************************/
/*			mvl_get_com_event				*/
/************************************************************************/
/* This function is used to get a MVL_COMM_EVENT control buffer to be	*/
/* passed up to the caller of '_mvl_net_service'. 			*/

static MVL_COMM_EVENT *mvl_get_com_event (ST_VOID)
  {
MVL_COMM_EVENT *commEvent;

  S_LOCK_COMMON_RESOURCES ();
  commEvent = (MVL_COMM_EVENT *) M_CALLOC (MSMEM_COM_EVENT, 1, sizeof (MVL_COMM_EVENT));  
  if (commEvent == NULL)
    {
    MVL_LOG_ERR0 ("Error: No MVL_COMM_EVENT available");
    S_UNLOCK_COMMON_RESOURCES ();
    return (NULL);
    }

  list_add_last((ST_VOID **) &mvl_comm_event_up_head, (ST_VOID *) commEvent);
  S_UNLOCK_COMMON_RESOURCES ();
  return (commEvent);
  }

/************************************************************************/
/*			_mvl_free_comm_event				*/
/************************************************************************/

ST_VOID _mvl_free_comm_event (MVL_COMM_EVENT *event)
  {
  S_LOCK_COMMON_RESOURCES ();
  if (event->event_type == MMS_MSG_RCVD)
    {
    if (event->u.mms.pdu)
      M_FREE (MSMEM_RXPDU, event->u.mms.pdu);

    if (event->u.mms.dec_rslt.data_pres)
      {
      M_FREE (MSMEM_DEC_OS_INFO, event->u.mms.dec_rslt.data_ptr);
      }  
    }

  M_FREE (MSMEM_COM_EVENT, event);
  S_UNLOCK_COMMON_RESOURCES ();
  }

/************************************************************************/
/*		 	mvl_set_max_msg_size 				*/
/************************************************************************/

ST_VOID mvl_set_max_msg_size (ST_INT max_msg_size)
  {
  mvl_cfg_info->max_msg_size = max_msg_size;
  mmsl_enc_buf_size = max_msg_size + ASN1_MAX_ELEMENT_OVERHEAD;
  if (mmsl_enc_buf != NULL)
    M_FREE (MSMEM_STARTUP, mmsl_enc_buf);

  mmsl_enc_buf = (ST_UCHAR *) M_MALLOC (MSMEM_STARTUP, mmsl_enc_buf_size);
  }

/************************************************************************/
/*			mvl_init_aa_obj_ctrl				*/
/* Init struct to control AA-Specific object creation (var and nvl).	*/
/* Init one MVL_AA_OBJ_CTRL struct for one connection.			*/
/************************************************************************/
static MVL_AA_OBJ_CTRL *mvl_init_aa_obj_ctrl ()
  {
MVL_AA_OBJ_CTRL *aa_obj;
  aa_obj = (MVL_AA_OBJ_CTRL *) M_CALLOC (MSMEM_STARTUP, 1, sizeof(MVL_AA_OBJ_CTRL));

  /* Set up for dynamic creation of AA-Specific Variables.	*/
  aa_obj->max_num_var_assoc = mvl_max_dyn.aa_vars;
  aa_obj->num_var_assoc = 0;
  if (aa_obj->max_num_var_assoc)
    aa_obj->var_assoc_tbl = (MVL_VAR_ASSOC **) M_CALLOC (MSMEM_STARTUP, aa_obj->max_num_var_assoc, sizeof (MVL_VAR_ASSOC *));

  /* Set up for dynamic creation of AA-Specific NamedVariableLists (NVL).	*/
  aa_obj->max_num_nvlist = mvl_max_dyn.aa_nvls;
  aa_obj->num_nvlist = 0;
  if (aa_obj->max_num_nvlist)
    aa_obj->nvlist_tbl = (MVL_NVLIST_CTRL **) M_CALLOC (MSMEM_STARTUP, aa_obj->max_num_nvlist, sizeof (MVL_NVLIST_CTRL *));
  return (aa_obj);
  }

/************************************************************************/
/*			mvl_free_aa_obj_ctrl				*/
/* Free MVL_AA_OBJ_CTRL struct allocated by mvl_init_aa_obj_ctrl.	*/
/************************************************************************/
static ST_VOID mvl_free_aa_obj_ctrl (MVL_AA_OBJ_CTRL *aa_obj)
  {
  if (aa_obj->var_assoc_tbl)
    M_FREE (MSMEM_STARTUP, aa_obj->var_assoc_tbl);
  if (aa_obj->nvlist_tbl)
    M_FREE (MSMEM_STARTUP, aa_obj->nvlist_tbl);
  M_FREE (MSMEM_STARTUP, aa_obj);
  }

/************************************************************************/
/*                       mvl_start_acse					*/
/* If (cfgInfo->use_bind_ctrl), caller must also allocate		*/
/* "cfgInfo->bind_ctrl" using "chk_?alloc" and initialize it, else it	*/
/* is allocated here. It will be freed by "mvl_end_acse".		*/
/************************************************************************/

ST_RET mvl_start_acse (MVL_CFG_INFO *cfgInfo)
  {
ST_RET ret;
ST_INT i;
ST_INT index = 0;
extern int mmsop_en_version1;

  mmsop_en_version1=0;	/* value of this var means nothing, but it	*/
			/* causes error if you link to older mmsop_en.c	*/

  MVL_LOG_ACSE0 ("ACSE: Starting");

  mvl_debug_sel |= MVLLOG_ERR;		/* Always enable error logging.	*/

/* Here we will make the MVL configuration information persistant 	*/
  if (!cfgInfo->use_bind_ctrl)
    {	/* Create the required bind control structures, if not supplied */
    cfgInfo->max_bind_ctrl = 1;
    cfgInfo->num_bind_ctrl = 1;
    cfgInfo->bind_ctrl = (NET_BIND_CTRL*)chk_calloc (cfgInfo->max_bind_ctrl, sizeof (NET_BIND_CTRL));
    strcpy (cfgInfo->bind_ctrl[0].ar_name, cfgInfo->local_ar_name);
    }

/* Make sure bind_ctrl was allocated just above OR by caller.		*/
  if (chk_alloc_ptr (cfgInfo->bind_ctrl))
    {
    MVL_LOG_ERR0 ("mvl_start_acse: bind_ctrl must be allocated.");
    return (SD_FAILURE);
    }

/* OK, now copy the MVL configuration info */
  mvl_cfg_info = (MVL_CFG_INFO *) chk_calloc (1, sizeof (MVL_CFG_INFO));
  *mvl_cfg_info = *cfgInfo;	/* copy caller's struct (includes allocated bind_ctrl)*/

/* Make sure max msg size has been set.					*/
  if (mvl_cfg_info->max_msg_size == 0)
    {
    MVL_LOG_ERR0 ("Max MMS message size must be set");
    return (SD_FAILURE);
    }
  mvl_set_max_msg_size (mvl_cfg_info->max_msg_size);	/* save to global & alloc bufs*/

/* Initialize function pointers for MMS allocation.			*/
  m_calloc_os_fun = _mvl_calloc_os;
  m_realloc_os_fun = _mvl_realloc_os;
  m_free_os_fun = _mvl_free_os;

#if !(MMS_INIT_EN & REQ_EN)
  if (mvl_cfg_info->num_calling != 0)
    {
    MVL_LOG_ERR0 ("num_calling > 0 is illegal because Initiate.Req is NOT enabled");
    return (SD_FAILURE);
    }
#endif
#if !(MMS_INIT_EN & RESP_EN)
  if (mvl_cfg_info->num_called != 0)
    {
    MVL_LOG_ERR0 ("num_called > 0 is illegal because Initiate.Resp is NOT enabled");
    return (SD_FAILURE);
    }
#endif

/* Make sure "num_calling" AND/OR "num_called" has been set.		*/
  if (mvl_cfg_info->num_calling == 0  &&  mvl_cfg_info->num_called == 0)
    {
    MVL_LOG_ERR0 ("num_calling AND num_called both 0, must set at least one." );
    return (SD_FAILURE);
    }

/* Alloc & fill in array of "calling" connection control structures.	*/
#if (MMS_INIT_EN & REQ_EN)
  if (mvl_cfg_info->num_calling != 0)

    mvl_calling_conn_ctrl = (MVL_NET_INFO *) M_CALLOC (MSMEM_GEN, mvl_cfg_info->num_calling, sizeof (MVL_NET_INFO));

  for (i = 0; i < mvl_cfg_info->num_calling; ++i)
    {
    mvl_calling_conn_ctrl[i].index = index++;
    mvl_calling_conn_ctrl[i].aa_objs = mvl_init_aa_obj_ctrl ();
    }
#endif

/* Alloc & fill in array of "called" connection control structures.	*/
#if (MMS_INIT_EN & RESP_EN)
  if (mvl_cfg_info->num_called != 0)

    mvl_called_conn_ctrl = (MVL_NET_INFO *) M_CALLOC (MSMEM_GEN, mvl_cfg_info->num_called, sizeof (MVL_NET_INFO));
    
  for (i = 0; i < mvl_cfg_info->num_called; ++i)
    {
    mvl_called_conn_ctrl[i].index = index++;
    mvl_called_conn_ctrl[i].aa_objs = mvl_init_aa_obj_ctrl ();
    }
#endif

/* Initialize Connection Oriented OSI Stack.	*/
  ret = copp_initialize (mvl_cfg_info->num_calling + mvl_cfg_info->num_called, mmsl_enc_buf_size);
  if (ret != SD_SUCCESS)
    {
    MVL_LOG_ERR1 ("Error: copp_initialize returned error '0x%x'", ret);
    return (ret);
    }

/* Bind local AR names */
/* Bind only if "called" connections configured.			*/
/* Bind ID is saved in NET_BIND_CTRL struct.				*/
/* NOTE: Second bind will fail if linked to "single bind" stack.	*/
  if (mvl_cfg_info->num_called)
    {
    for (i = 0; i < mvl_cfg_info->num_bind_ctrl; ++i)
      {  
      ret = ma_bind (&mvl_cfg_info->bind_ctrl[i], mvl_bind_timeout);
      if (ret != SD_SUCCESS)
        break;	/* don't return, so cleanup code will execute	*/
      }
    }	/* end if (mvl_cfg_info->num_called)	*/

/* Binding not necessary for CLACSE.	*/

/* Initialize all mms objects (generated by foundry).			*/
  if (ret == SD_SUCCESS)
    mvl_init_mms_objs ();

  if (ret == SD_SUCCESS)
    mvl_acse_ok = SD_TRUE;
  else
    {
    copp_terminate ();
    }

  MVL_LOG_ACSE1 ("ACSE: Start complete, returning '0x%x'", ret);
  mvl_mms_statistics.startTime = sGetMsTime ();

  return (ret);
  }

/************************************************************************/
/*                       mvl_end_acse					*/
/************************************************************************/

ST_INT mvl_end_acse (ST_VOID)
  {
NET_BIND_CTRL *bc;
ST_INT i;

  MVL_LOG_ACSE0 ("ACSE: Ending");

/* Unbind all */
/* Unbind only if "called" connections configured.			*/
  if (mvl_cfg_info->num_called)
    {
    for (i = 0; i < mvl_cfg_info->num_bind_ctrl; ++i)
      {
      bc = &mvl_cfg_info->bind_ctrl[i];
      copp_unbind_sync (bc, mvl_bind_timeout);
      }
    }

/* Shut down the ACSE interface, as well as it's subsystems		*/

  copp_terminate ();

/* Connection control structures no longer needed.	*/
/* Free all "calling" connection control structures.	*/
  if (mvl_calling_conn_ctrl)
    {
    for (i = 0; i < mvl_cfg_info->num_calling; ++i)
      mvl_free_aa_obj_ctrl (mvl_calling_conn_ctrl[i].aa_objs);
    M_FREE (MSMEM_GEN, mvl_calling_conn_ctrl);
    }
/* Free all "called" connection control structures.	*/
  if (mvl_called_conn_ctrl)
    {
    for (i = 0; i < mvl_cfg_info->num_called; ++i)
      mvl_free_aa_obj_ctrl (mvl_called_conn_ctrl[i].aa_objs);
    M_FREE (MSMEM_GEN, mvl_called_conn_ctrl);
    }

  if (mmsl_enc_buf != NULL)
    {
    M_FREE (MSMEM_STARTUP, mmsl_enc_buf);
    mmsl_enc_buf = NULL;	/* prepare for restart	*/
    }
  chk_free (mvl_cfg_info->bind_ctrl);
  chk_free (mvl_cfg_info);

  /* Free any events on list. These will never be processed.	*/
  while (mvl_comm_event_up_head)
    {
    MVL_COMM_EVENT *event;
    event = (MVL_COMM_EVENT *) list_get_first((ST_VOID **) &mvl_comm_event_up_head);
    _mvl_free_comm_event (event);
    }

  MVL_LOG_ACSE0 ("ACSE: Ended");
  return (SD_SUCCESS);
  }

/************************************************************************/
/*			_mvl_send_msg					*/
/************************************************************************/

ST_INT _mvl_send_msg (ST_UCHAR *pdu, ST_INT pdu_len, MVL_NET_INFO *net_info)
  {
ST_INT ret;

/* See if we are supposed to log ACSE data for this connection		*/
  if (mvl_conn_filtered_logging == SD_FALSE || net_info->log_enable)
    {
    MVL_LOG_ACSEDATA2 ("ACSE: Sending %d bytes on MVL_NET_INFO " S_FMT_PTR, 
						pdu_len, net_info);
    MVL_LOG_ACSEDATAH (pdu_len, pdu);
    }

  ret = SD_FAILURE;

#if (MMS_INIT_EN != REQ_RESP_DIS)
    if (net_info->conn_active == SD_FALSE)
    { /* no connection */
      ret = MVL_ERR_CNF_DISCONNECTED;	/* more precise error code	*/
      MVL_LOG_NERR1 ("ACSE: no connection '0x%x'", ret);
    }
    else
    { /* valid connection */
      S_LOCK_COMMON_RESOURCES ();
      ret = a_data_req (net_info->acse_conn_id, pdu_len, pdu);
      S_UNLOCK_COMMON_RESOURCES ();
      if (ret != SD_SUCCESS)
        {
          MVL_LOG_NERR1 ("ACSE: a_data_req error '0x%x'", ret);
        }
    }
#endif
  return (ret);
  }

/************************************************************************/
/************************************************************************/
/*		DIRECTORY SERVICES FUNCTIONS				*/
/************************************************************************/
#if !defined(SISCO_STACK_CFG)
/************************************************************************/
/*                      find_loc_dib_entry                              */
/************************************************************************/
DIB_ENTRY *find_loc_dib_entry (ST_CHAR *name)
  {
ST_INT i;
DIB_ENTRY *de;

  if (name == NULL)
    return (NULL);	/* "name" arg is not valid	*/

  for (i = 0; i < num_loc_dib_entries; i++)
    {
    de = &loc_dib_table[i];
    if (!strcmp (de->name, name))
      return (de);
    }
  return (NULL);
  }

/************************************************************************/
/*                      find_rem_dib_entry                              */
/************************************************************************/

DIB_ENTRY *find_rem_dib_entry (ST_CHAR *name)
  {
ST_INT i;
DIB_ENTRY *de;

  if (name == NULL)
    return (NULL);	/* "name" arg is not valid	*/

  for (i = 0; i < num_rem_dib_entries; i++)
    {
    de = &rem_dib_table[i];
    if (!strcmp (de->name, name))
      return (de);
    }
  return (NULL);
  }
#endif	/* !defined(SISCO_STACK_CFG)	*/

/************************************************************************/
/*			mvl_dib_entry_to_app_ref			*/
/************************************************************************/

ST_VOID mvl_dib_entry_to_app_ref (DIB_ENTRY *de, APP_REF *appRef)
  {
ST_INT i;

  appRef->form = APP_REF_FORM2;
  appRef->ap_title_pres = de->ae_title.AP_title_pres;
  if (appRef->ap_title_pres == SD_TRUE)
    {
    appRef->ap_title.form_2.num_comps = de->ae_title.AP_title.num_comps;
    for (i = 0; i < de->ae_title.AP_title.num_comps; ++i)
      appRef->ap_title.form_2.comps[i] = de->ae_title.AP_title.comps[i];
    }
  appRef->ap_invoke_pres = de->ae_title.AP_inv_id_pres;
  appRef->ap_invoke = de->ae_title.AP_inv_id;
  appRef->ae_qual_pres = de->ae_title.AE_qual_pres;
  appRef->ae_qual.form_2 = de->ae_title.AE_qual;
  appRef->ae_invoke_pres = de->ae_title.AE_inv_id_pres;
  appRef->ae_invoke = de->ae_title.AE_inv_id;
  }

/************************************************************************/
/************************************************************************/
#if (MMS_INIT_EN != REQ_RESP_DIS)	/* COACSE SPECIFIC FUNCTIONS	*/
/************************************************************************/

#if (MMS_INIT_EN & REQ_EN)

/************************************************************************/
/*			mvla_initiate_req_exx				*/
/************************************************************************/

ST_INT mvla_initiate_req_exx (ST_CHAR *locAr,
			     ST_CHAR *remAr,
			     INIT_INFO *req_info, 
			     INIT_INFO *resp_info,
			     MVL_NET_INFO **net_info_out,
			     MVL_REQ_PEND **req_out,
			     ACSE_AUTH_INFO *auth_info,
			     S_SEC_ENCRYPT_CTRL *encrypt_info,
			     S_CERT_CTRL *locMaceCert)
  {
ST_INT rc;
MVL_NET_INFO *cc;
DIB_ENTRY *locDe;
DIB_ENTRY *remDe;
AARQ_APDU aarq_apdu;
ST_VOID *acse_conn_id;
MVL_REQ_PEND *req;

  *req_out = NULL;			/* make sure it's initialized	*/

  MVL_LOG_ACSE1 ("ACSE: Initiate Request to '%s'", remAr);

  /* Get a calling connection control structure					*/
  cc = ma_get_calling_conn_ctrl ();
  if (!cc)
    {
    MVL_LOG_ERR0 ("Error - Could not get calling MVL_NET_INFO");
    return (MVL_ERR_NO_CONN_CTRL);
    }
  MVL_LOG_CACSE1 ("Using MVL_NET_INFO " S_FMT_PTR, cc);

  req = _mvl_get_req_ctrl (cc, MMSOP_INITIATE);
  if (!req)
    {
    cc->in_use = SD_FALSE;
    return (MVL_ERR_NO_REQ_CTRL);
    }
  *req_out = req;

  req->invoke_id = (ST_UINT32) -1;

  rc = mpl_init (req_info);
  if (rc == SD_SUCCESS)
    {
   /* We can log here if so desired */
    if ((mms_debug_sel & MMS_LOG_REQ) && mlogl_info.req_info_fun != NULL)
        (*mlogl_info.req_info_fun) (0, MMSOP_INITIATE, req_info);

  /* Save the output pointer in the MMS request control element		*/
    req->u.init.resp_info = resp_info;

  /* Save our initiate request information ... */
    memcpy (&cc->locl_init_info, req_info, sizeof (INIT_INFO));

  /* Retrieve the DIB_ENTRY's for local and remote. These are used to set	*/
  /* the AARQ_APDU information						*/
#if defined (SISCO_STACK_CFG) || defined(USE_DIB_LIST)	/* Uses just one list of AR Names.	*/
    locDe = find_dib_entry (locAr);
    remDe = find_dib_entry (remAr);
#else
    locDe = find_loc_dib_entry (locAr);
    remDe = find_rem_dib_entry (remAr);
#endif
    if (locDe == NULL)
      {
      MVL_LOG_NERR1 ("ACSE: Local AR name '%s' not found", locAr ? locAr : "NULL");
      rc = MVLE_LOCAL_NAME;
      }
    else if (remDe == NULL) 
      {
      MVL_LOG_NERR1 ("ACSE: Remote name '%s' not found", remAr);
      rc = MVLE_REMOTE_NAME;
      }
    else
      {  
    /* Now set up all AARQ_APDU information based on DIB_ENTRY information	*/
    
    /* AP Context is MMS */
      aarq_apdu.ASO_context_name_pres = SD_TRUE;
      aarq_apdu.ASO_context_name.num_comps = 5;
      aarq_apdu.ASO_context_name.comps[0] = 1;
      aarq_apdu.ASO_context_name.comps[1] = 0;
      aarq_apdu.ASO_context_name.comps[2] = 9506;
      aarq_apdu.ASO_context_name.comps[3] = 2;
      aarq_apdu.ASO_context_name.comps[4] = 3;

    /* Calling & Called AE Title */
      memcpy (&aarq_apdu.calling_ae_title, &locDe->ae_title, sizeof (AE_TITLE));
      memcpy (&aarq_apdu.called_ae_title,  &remDe->ae_title, sizeof (AE_TITLE));
    /* Calling & Called Presentation Address */
      memcpy (&aarq_apdu.calling_paddr, &locDe->pres_addr, sizeof (PRES_ADDR));
      memcpy (&aarq_apdu.called_paddr,  &remDe->pres_addr, sizeof (PRES_ADDR));

    /* ACSE Authentication */
      if(auth_info != NULL)
      	memcpy (&aarq_apdu.auth_info, auth_info, sizeof (ACSE_AUTH_INFO));
      else
        aarq_apdu.auth_info.auth_pres = SD_FALSE;
    /* copy encryption info */
      if (encrypt_info != NULL)
      	memcpy (&aarq_apdu.encrypt_ctrl, encrypt_info, sizeof (S_SEC_ENCRYPT_CTRL));
      else
        aarq_apdu.encrypt_ctrl.encryptMode = S_SEC_ENCRYPT_NONE;
    
    /* User Data */
      aarq_apdu.user_info.ptr = mmsl_msg_start;
      aarq_apdu.user_info.len = mmsl_msg_len;
  
      MVL_LOG_ACSEDATAH (mmsl_msg_len, mmsl_msg_start);
    
    /* Send the associate request						*/
      acse_conn_id = a_associate_req (cc, &aarq_apdu);
      if (!acse_conn_id)
        {
        MVL_LOG_NERR0 ("ACSE: a_associate_req failed");
        rc = MVL_ERR_ASSOC_REQ;
        }
      else
        ++mvl_mms_statistics.callingConn;
      }
    }
  else
    {
    MVL_LOG_NERR1 ("ACSE: mpl_init error '0x%x'", rc);
    }

  if (rc == SD_SUCCESS)
    {
    *net_info_out = cc;
    cc->acse_conn_id = acse_conn_id;
    cc->loc_mace = locMaceCert;
    }
  else
    {
    cc->in_use = SD_FALSE;
    mvl_free_req_ctrl (req);
    *req_out = NULL;			/* So user CANNOT free it too.	*/
    }

/* All is good, return the connection control too			*/
  return (rc);
  }
/************************************************************************/
#endif /* #if (MMS_INIT_EN & REQ_EN)					*/
/************************************************************************/

/************************************************************************/
/*			mvl_abort_req					*/
/*			mvl_abort_req_ex					*/
/************************************************************************/

ST_RET mvl_abort_req (MVL_NET_INFO *cc)
  {
  return (mvl_abort_req_ex (cc, SD_FALSE, 0));
  }


ST_RET mvl_abort_req_ex (MVL_NET_INFO *cc, 
			 ST_BOOLEAN diagnostic_pres, 
			 ST_ACSE_AUTH diagnostic)
  {
ABRT_APDU abrtinfo;
ST_RET ret;

  MVL_LOG_ACSE1 ("ACSE: sending abort for MVL_NET_INFO " S_FMT_PTR, cc);

  abrtinfo.source = 0;		/* user	*/
  abrtinfo.diagnostic_pres = diagnostic_pres;
  abrtinfo.diagnostic = diagnostic;
  abrtinfo.user_info.len = 0;

  ret = a_abort_req (cc->acse_conn_id, &abrtinfo);
  if (ret == SD_SUCCESS) 
    {
    ++mvl_mms_statistics.localAbort;
    cc->in_use = SD_FALSE;
    cc->conn_active = SD_FALSE;
    if (_mvlu_rpt_disconnect_rcvd_fun != NULL)
       (*_mvlu_rpt_disconnect_rcvd_fun)(cc);
    _mvl_set_net_req_done (NULL, cc, MVL_ERR_CNF_DISCONNECTED);
    }
  else
    {
    MVL_LOG_NERR1 ("ACSE: Abort error '0x%x'", ret);
    }
  return (ret);
  }

/************************************************************************/
/*                       mvl_release_req					*/
/*			 mvla_release_req				*/
/************************************************************************/

ST_INT mvl_release_req (MVL_NET_INFO *cc) 
  {
  MVL_LOG_ERR0 ("ACSE: Using obsolete function 'mvl_release_req'");
  return (SD_FAILURE);
  }

ST_INT mvla_release_req (MVL_NET_INFO *cc, MVL_REQ_PEND **req_out)
  {
  MVL_LOG_ERR0 ("ACSE: Using obsolete function 'mvla_release_req'");
  return (SD_FAILURE);
  }

/************************************************************************/
/*			_mvla_release					*/
/************************************************************************/
/* Called from conclude confirm						*/

ST_VOID _mvl_release (MVL_REQ_PEND *req)
  {
RLRQ_APDU rlinfo;
ST_INT ret;
MVL_NET_INFO *net_info;

  net_info = req->net_info;
  req->op = MVL_MMSOP_RELEASE;	/* Change the opcode in the req ctrl	*/

  MVL_LOG_ACSE1 ("ACSE: sending release for MVL_NET_INFO " S_FMT_PTR, net_info);

  rlinfo.reason_pres = SD_TRUE;
  rlinfo.reason = 1;
  rlinfo.user_info.len = 0;
  ret = a_release_req (net_info->acse_conn_id, &rlinfo);
  if (ret != SD_SUCCESS)
    {
    MVL_LOG_NERR1 ("ACSE: Release error '0x%x'", ret);
    _mvl_set_req_done (req, SD_FAILURE);
    }
  else
    ++mvl_mms_statistics.clientConcl;
  }

/************************************************************************/
/************************************************************************/
/*			mvl_add_bind_ctrl				*/
/************************************************************************/

ST_RET mvl_add_bind_ctrl (NET_BIND_CTRL *bc)
  {
NET_BIND_CTRL *mvlBc;
ST_BOOLEAN activate;
ST_INT i;
ST_RET rc;

/* First let's check to see if we have room for one more */
  if (mvl_cfg_info->num_bind_ctrl >= mvl_cfg_info->max_bind_ctrl)
    {
    MVL_LOG_NERR0 ("Error: no availble bind control");
    return (SD_FAILURE);
    }

/* Let's see if this local AR has already been activated	*/
  activate = SD_TRUE;
  for (i = 0; i < mvl_cfg_info->num_bind_ctrl; ++i)
    {
    if (!strcmp (mvl_cfg_info->bind_ctrl[i].ar_name, bc->ar_name))
      {
      activate = SD_FALSE;
      break;
      }
    }

/* OK, now we can copy the bind control information */
  
  mvlBc = &mvl_cfg_info->bind_ctrl[mvl_cfg_info->num_bind_ctrl];
  *mvlBc = *bc;
  ++mvl_cfg_info->num_bind_ctrl;
  
  rc = ma_bind (mvlBc, mvl_bind_timeout);
  return (rc);
  }

/************************************************************************/
/*			ma_bind					*/
/************************************************************************/

static ST_RET ma_bind (NET_BIND_CTRL *bc, ST_INT timeout)
  {
DIB_ENTRY *de;
ST_INT ret;
ST_INT numConn;

  MVL_LOG_ACSE1 ("ACSE: binding local name '%s'", bc->ar_name);

/* Locate the local DIB entry for this name, so we can find the addr	*/
#if defined(SISCO_STACK_CFG) || defined(USE_DIB_LIST)
  de = find_dib_entry (bc->ar_name);
#else
  de = find_loc_dib_entry (bc->ar_name);
#endif
  if (!de)
    {
    MVL_LOG_ERR1 ("Error: Local AR name '%s' not found", bc->ar_name);
    return (MVLE_LOCAL_NAME);
    }
  if (de->pres_addr.tp_type != TP_TYPE_TCP)
    {
    MVL_LOG_ERR1 ("ma_bind: Unsupported transport type '%d'", de->pres_addr.tp_type);
    return (MVLE_LOCAL_NAME);
    }

  numConn = mvl_cfg_info->num_calling + mvl_cfg_info->num_called;

  bc->tp_type = de->pres_addr.tp_type;

  bc->cmd_done = SD_FALSE;
  bc->cmd_rslt = REQ_INCOMPLETE;
  ret = copp_bind (bc, &de->pres_addr, SD_FALSE, numConn);
  if (ret==SD_SUCCESS)
    ret = ma_bind_wait (bc, timeout);	/* wait for bind confirm	*/
  if (ret != SD_SUCCESS)
    {
    MVL_LOG_ERR2 ("Error: copp_bind of AR '%s' returned '0x%x'", 
    				bc->ar_name, ret);
    return (ret);
    }

  MVL_LOG_ACSE0 ("ACSE: bind OK");

  return (ret);
  }

/************************************************************************/
/*			copp_unbind_sync				*/
/************************************************************************/

static ST_VOID copp_unbind_sync (NET_BIND_CTRL *bc, ST_INT timeout)
  {
ST_RET ret;

  bc->cmd_done = SD_FALSE;
  bc->cmd_rslt = REQ_INCOMPLETE;
  ret = copp_unbind (bc->copp_bind_id);
  if (ret == SD_SUCCESS)
    ret = ma_bind_wait (bc, timeout);	/* wait for unbind confirm	*/

  if (ret != SD_SUCCESS)
    {
    MVL_LOG_ERR1 ("Error: unbind result '0x%x'", ret);
    }
  }

/************************************************************************/
/* 		     COACSE USER FUNCTIONS	       			*/
/************************************************************************/
/************************************************************************/
/*                       u_copp_bind_cnf				*/
/************************************************************************/

ST_VOID u_copp_bind_cnf (ST_VOID *user_bind_id, ST_VOID *copp_bind_id, ST_RET result)
  {
NET_BIND_CTRL *bc;

/* Save the acse interface's bind ID for future use */
  bc = (NET_BIND_CTRL *) user_bind_id;
  bc->copp_bind_id = copp_bind_id;

/* Allow sync binds to complete */
  bc->cmd_done = SD_TRUE;
  bc->cmd_rslt = result;
  }

/************************************************************************/
/*                       u_copp_unbind_cnf				*/
/************************************************************************/

ST_VOID u_copp_unbind_cnf (ST_VOID *user_bind_id)
  {
NET_BIND_CTRL *bc = (NET_BIND_CTRL *) user_bind_id;
/* Allow the request function to complete */
  bc->cmd_done = SD_TRUE;
  bc->cmd_rslt = SD_SUCCESS;
  }

/************************************************************************/
/*                       u_a_associate_ind				*/
/************************************************************************/

ST_VOID u_a_associate_ind (ST_VOID *u_bind_id, ST_VOID *acse_conn_id, 
			   AARQ_APDU *info)
  {
#if (MMS_INIT_EN & RESP_EN)

MMSDEC_INFO rslt;
INIT_INFO *init_req; 
INIT_INFO init_resp; 
MVL_NET_INFO *cc;
#if !defined(ICCP_LITE)
ST_ACSE_AUTH auth_ret;
#endif
ST_INT ret;
AARE_APDU reinfo;
ST_INT accept;
ST_BOOLEAN free_dec_info;
MVL_ASSOC_IND *mvlAssocInd;
AARQ_APDU     *reqinfo; 
ST_UCHAR      *user_info_buf;

  if (mvl_init_ind_hold)
    {
  /* Can't rely on this data being persistent			*/
  /* Allocate buffer big enough for the control structure, 	*/
  /* the AARQ info, and the user data 				*/

    mvlAssocInd = (MVL_ASSOC_IND *) chk_malloc (sizeof (MVL_ASSOC_IND) + 
    			      sizeof (AARQ_APDU) + 
    			      info->user_info.len);

    mvlAssocInd->u_bind_id    = u_bind_id; 
    mvlAssocInd->acse_conn_id = acse_conn_id;

  /* Copy the AARQ data */
    reqinfo = (AARQ_APDU *) (mvlAssocInd + 1); 
    *reqinfo = *info;	
    mvlAssocInd->info = reqinfo;

  /* Copy the user data, set the pointer to it */
    user_info_buf = (ST_UCHAR *) (reqinfo + 1);
    reqinfo->user_info.ptr = user_info_buf;
    memcpy (user_info_buf, info->user_info.ptr, info->user_info.len);

    list_add_last (&mvlAssocIndList, mvlAssocInd);
    return;
    }

#if defined(_S_TEST_PROBES)
  if (_sTestProbeEnabledS("MIInitiateIndNoRespond"))
    {
    SLOGALWAYS0 ("TEST: Initiate indication, do not respond");
    return;
    }
#endif

  ++mvl_mms_statistics.calledConn;

  MVL_LOG_ACSE0 ("ACSE: Associate Indication received");
  MVL_LOG_ACSEDATAH (info->user_info.len, info->user_info.ptr);

  free_dec_info = SD_FALSE;

/* Assume the worst ... */
  accept = SD_FALSE;
  reinfo.user_info.len = 0;
  reinfo.user_info.ptr = NULL;
  reinfo.auth_info.auth_pres = SD_FALSE;

/* Get a 'called' connection control structure, if one is available 	*/
  cc = ma_get_called_conn_ctrl ((NET_BIND_CTRL *) u_bind_id);
  if (cc)
    {
    MVL_LOG_CACSE1 ("Using MVL_NET_INFO " S_FMT_PTR, cc);

    /* Copy the whole AARQ Info struct, for the user to see		*/
    memcpy (&cc->ass_ind_info, info, sizeof (AARQ_APDU));

    /* IMPORTANT: Save the ACSE connection ID before calling user funtions.*/
    cc->acse_conn_id = acse_conn_id;

    /* We need to parse the MMS Initiate PDU 				*/
    free_dec_info = SD_TRUE;
    _ms_mms_decode (info->user_info.ptr, info->user_info.len, &rslt);
    if (rslt.err_code == NO_DECODE_ERR && rslt.type == MMSINITREQ)
      {
      init_req = (INIT_INFO *) rslt.data_ptr;
      init_req->mms_p_context_pres = SD_TRUE;

   /* We can log here if so desired */
      if ((mms_debug_sel & MMS_LOG_IND) && mlogl_info.ind_info_fun != NULL)
          (*mlogl_info.ind_info_fun) (0, MMSOP_INITIATE, init_req);

/* OK, we handle telling the user differently depending on whether this	*/
/* is ICCP Lite, and whether ACSE Authentication is in use or not	*/
#ifdef ICCP_LITE
      ret = mi_connect_ind_fun (cc, init_req, &reinfo.auth_info);
#else
    /* Let the user have a look to see if they want to accept		*/
      auth_ret = u_mvl_connect_ind_ex(cc, init_req, 
      				      &cc->ass_ind_info.auth_info, /* partner authentication	*/
      				      &reinfo.auth_info);	   /* ret rsp authentication	*/
      /* User did not like something about the authentication 		*/
      if (auth_ret != ACSE_AUTH_SUCCESS)
        {
  	MVL_LOG_ACSE1 ("ACSE: ACSE Authentication failure. Reason: %d", auth_ret);
        mvl_abort_req_ex (cc, SD_TRUE, auth_ret);
	M_FREE (MSMEM_DEC_OS_INFO, rslt.data_ptr);
  	return;
  	}
      ret = SD_SUCCESS;	/* User is OK with it */
#endif /* ICCP_LITE */

      if (ret == SD_SUCCESS)
        {
    /* Build an initiate response PDU, negiotiating the calling init	*/
    /* parameters vs. what we support					*/

#ifdef NEGIOTIATE_INITIATE_PARAM
      /* Start with the default initiate response info			*/
        memcpy (&init_resp, mvl_init_resp_info, sizeof (INIT_INFO));
        if (init_req->version < init_resp.version)/* thiers is lower	*/
          init_resp.version = init_req->version;	/* accept lower version */
        if (init_req->max_segsize_pres)
          {
          if (init_req->max_segsize < init_resp.max_segsize)
            init_resp.max_segsize = init_req->max_segsize;  
          }
        if (init_req->maxreq_calling < init_resp.maxreq_calling)
          init_resp.maxreq_calling = init_req->maxreq_calling;
        if (init_req->maxreq_called < init_resp.maxreq_called)
          init_resp.maxreq_called = init_req->maxreq_called;
        if (init_req->max_nest_pres)
          {
          if (init_req->max_nest < init_resp.max_nest)
            init_resp.max_nest = init_req->max_nest;
          }
      /* These 'supported' bitstring parameters are set up in mmsop_en.c	*/
        memcpy (init_resp.serv_supp, m_service_resp, 11);
        init_resp.param_supp[0] = m_param[0] & init_req->param_supp[0];
        init_resp.param_supp[1] = m_param[1] & init_req->param_supp[1];
#else
        /* Just send back what the user wants to send */ 
        memcpy (&init_resp, mvl_init_resp_info, sizeof (INIT_INFO));
#endif

        ret = mpl_init_resp (&init_resp);
        if (ret == SD_SUCCESS)
          {
         /* We can log here if so desired */
          if ((mms_debug_sel & MMS_LOG_RESP) && mlogl_info.resp_info_fun != NULL)
            (*mlogl_info.resp_info_fun) (0, MMSOP_INITIATE, &init_resp);

          accept = SD_TRUE;
      
          cc->conn_active = SD_TRUE;
        
          /* Save the remote node's initiate information			*/
          memcpy (&cc->rem_init_info, init_req, sizeof (INIT_INFO));
          memcpy (&cc->locl_init_info, &init_resp, sizeof (INIT_INFO));
          cc->max_pdu_size = (ST_INT) init_resp.max_segsize;
          /* NOTE: maxreq_calling = num of outstanding indications	*/
          /* allowed at the calling app (similar for maxreq_called).	*/
          cc->maxpend_req = init_resp.maxreq_calling;
          cc->maxpend_ind = init_resp.maxreq_called;
          cc->numpend_req = 0;
          cc->numpend_ind = 0;
          }
        }

      if (ret != SD_SUCCESS)
        {
        MVL_LOG_NERR0 ("ACSE: User does not want connection");
        ret = mpl_init_err (8, 0);
        cc->in_use = SD_FALSE;
        }

      if (ret == SD_SUCCESS)
	{
        reinfo.user_info.len = mmsl_msg_len;
        reinfo.user_info.ptr = mmsl_msg_start;
	}
      }
    else
      {
      MVL_LOG_NERR0 ("ACSE: Initiate indication PDU parse error");
      free_dec_info = SD_FALSE;
      cc->in_use = SD_FALSE;
      }
    }
  else
    {
    MVL_LOG_ERR0 ("Error - Could not get called MVL_NET_INFO");
    }
    
/* Initiate Response (or error) PDU is built, or len set to 0. 		*/
/* Set the remaining ACSE parameters and send the response		*/

/* Use the calling context as the responding, to be friendly		*/  
  reinfo.ASO_context_name_pres = info->ASO_context_name_pres; 
  memcpy (&reinfo.ASO_context_name, &info->ASO_context_name, 
  	  sizeof (MMS_OBJ_ID)); 
 
/* For now, just reflect the ACSE indication information 		*/
/* Note that it may (or may not) be more appropriate to use the 'local'	*/
/* DIB_ENTRY info for this purpose.					*/
  memcpy (&reinfo.responding_ae_title, &info->called_ae_title, 
            sizeof (AE_TITLE));


  if (accept)
    {
    MVL_LOG_ACSE0 ("Accepting the connection");
    reinfo.result = 0;	/* ACCEPT	*/
    reinfo.result_source_diag_pres = SD_FALSE;
    }
  else
    {
    MVL_LOG_ACSE0 ("Rejecting the connection");
    reinfo.result = 1;	/* REJECT	*/
    reinfo.result_source_diag_pres = SD_TRUE;
    reinfo.result_source = 1;
    reinfo.result_diag = 9;
    }

  /* Send ACSE response	*/
  MVL_LOG_ACSEDATAH (reinfo.user_info.len, reinfo.user_info.ptr);
  ret = a_associate_rsp (acse_conn_id, cc, &reinfo);
  if (ret != SD_SUCCESS)
    {
    MVL_LOG_NERR1 ("ACSE: a_associate_rsp error '0x%x", ret);
    if (cc != NULL)
      cc->in_use = SD_FALSE;
    ++mvl_mms_statistics.calledConnFail;
    }
  else
    ++mvl_mms_statistics.calledConnOK;

  if (free_dec_info == SD_TRUE)
    M_FREE (MSMEM_DEC_OS_INFO, rslt.data_ptr);

#else	/* !(MMS_INIT_EN & RESP_EN)	*/

AARE_APDU reinfo;
ST_VOID *user_conn_id;
ST_INT ret;

  ++mvl_mms_statistics.calledConn;

/* Incoming connections not supported, so Reject the association.	*/
  MVL_LOG_ACSE0 ("ACSE: Associate Indication received. Rejecting.");

/* Just use the ASO-context from the request in the response.		*/
  reinfo.ASO_context_name_pres = info->ASO_context_name_pres;
  memcpy (&reinfo.ASO_context_name, &info->ASO_context_name,
  	  sizeof (MMS_OBJ_ID));
 
/* Just use the called AE-Title as the responding AE-Title.		*/
  memcpy (&reinfo.responding_ae_title, &info->called_ae_title,
          sizeof (AE_TITLE));

  /* Reject	*/
  reinfo.result = 1;		/* Rejected-permanent	*/
  reinfo.result_source_diag_pres = SD_TRUE;
  reinfo.result_source = 1;	/* Source = user.	*/
  reinfo.result_diag = 1;	/* no-reason-given	*/
  /* No User Information	*/
  reinfo.user_info.len = 0;
  reinfo.user_info.ptr = NULL;
  reinfo.auth_info.auth_pres = SD_FALSE;

  user_conn_id = 0;	/* id doesn't matter because rejecting	*/
  if ((ret = a_associate_rsp (acse_conn_id, user_conn_id, &reinfo))!=SD_SUCCESS)
    {
    MVL_LOG_ERR1 ("Error sending a_associate_rsp. Error = %d", ret);
    }
  ++mvl_mms_statistics.calledConnFail;
#endif	/* !(MMS_INIT_EN & RESP_EN)	*/
  }


/************************************************************************/
/*			u_a_associate_cnf				*/
/************************************************************************/

ST_VOID u_a_associate_cnf (ST_VOID *acse_conn_id, ST_VOID *user_conn_id,
			AARE_APDU *info)
  {
#if (MMS_INIT_EN & REQ_EN)
MMSDEC_INFO rslt;
MVL_NET_INFO *cc;
INIT_INFO *init_resp; 
MVL_REQ_PEND *req;
ST_BOOLEAN free_dec_info;
ST_RET result;
ST_ACSE_AUTH auth_ret;

/* Need to save off the ACSE interface's connection ID for future use	*/
  cc = (MVL_NET_INFO *) user_conn_id;
  cc->acse_conn_id = acse_conn_id;

  MVL_LOG_ACSE1 ("ACSE: associate confirm for MVL_NET_INFO " S_FMT_PTR, cc); 

  req = _mvl_find_req_ctrl (cc, MMSOP_INITIATE, (ST_UINT32) -1);

  if (!req)
    {
    MVL_LOG_ERR0 ("ACSE: Associate Confirm - no matching req ctrl");
    info->result = SD_FAILURE;		/* Cause disconnect		*/
    cc->in_use = SD_FALSE;
    return;
    }

  /* Only check ACSE authentication if this is a positive connect confirm */
  if (info->result == SD_SUCCESS) 
    {
    auth_ret = u_mvl_connect_cnf_ex(cc, info);
  
    /* User did not like something about the authentication 		*/
    if(auth_ret != ACSE_AUTH_SUCCESS)
      {
      MVL_LOG_ACSE1 ("ACSE: ACSE Authentication failure. Reason: %d", auth_ret);
      mvl_abort_req_ex (cc, SD_TRUE, auth_ret);
      return;
      }
    }

  free_dec_info = SD_FALSE;
  result = info->result;

  if (info->result == SD_SUCCESS) 
    {
    cc->conn_active = SD_TRUE;

    MVL_LOG_ACSEDATAH (info->user_info.len, info->user_info.ptr);

  /* OK, now we need to parse the MMS Initiate PDU 			*/
    free_dec_info = SD_TRUE;
    _ms_mms_decode (info->user_info.ptr, info->user_info.len, &rslt);
    if (rslt.err_code == NO_DECODE_ERR)
      {
      switch (rslt.type)
        {
        case MMSINITRESP :
          init_resp = (INIT_INFO *) rslt.data_ptr;
          init_resp->mms_p_context_pres = SD_TRUE;

         /* We can log here if so desired */
          if ((mms_debug_sel & MMS_LOG_CONF) && mlogl_info.conf_info_fun != NULL)
            (*mlogl_info.conf_info_fun) (0, MMSOP_INITIATE, init_resp);

          if (req->u.init.resp_info != NULL)
            memcpy (req->u.init.resp_info, init_resp, sizeof (INIT_INFO));

          memcpy (&cc->rem_init_info, init_resp, sizeof (INIT_INFO));
          MVL_LOG_CACSE0 ("ACSE: initiate confirm OK"); 
          cc->max_pdu_size = init_resp->max_segsize;
          /* NOTE: maxreq_calling = num of outstanding indications	*/
          /* allowed at the calling app (similar for maxreq_called).	*/
          cc->maxpend_req = init_resp->maxreq_called;
          cc->maxpend_ind = init_resp->maxreq_calling;
          cc->numpend_req = 0;
          cc->numpend_ind = 0;
          ++mvl_mms_statistics.callingConnOk;
        break;
  
        case MMSINITERR	:
          result = SD_FAILURE;
          cc->in_use = SD_FALSE;
          MVL_LOG_NERR0 ("ACSE: initiate confirm error"); 
          ++mvl_mms_statistics.callingConnFail;
        break;
  
        default :
          MVL_LOG_NERR1 ("ACSE: associate confirm PDU wrong type (%d)", 
								rslt.type);
          result = SD_FAILURE;	
          cc->in_use = SD_FALSE;
          info->result = SD_FAILURE;	/* Cause disconnect		*/
          ++mvl_mms_statistics.callingConnFail;
        break;
        }   
      }
    else  /* PDU decode error, abort the connection			*/
      {
      MVL_LOG_NERR0 ("ACSE: associate confirm PDU Decode Error");
      result = SD_FAILURE;
      info->result = SD_FAILURE;	/* Cause disconnect		*/
      cc->in_use = SD_FALSE;
      ++mvl_mms_statistics.callingConnFail;
      }
    }
  else
    {
    MVL_LOG_NERR0 ("ACSE: associate confirm result != SD_SUCCESS");
    cc->in_use = SD_FALSE;
    ++mvl_mms_statistics.callingConnFail;
    }

  _mvl_set_req_done (req, result);

  if (free_dec_info == SD_TRUE && rslt.data_pres)
    M_FREE (MSMEM_DEC_OS_INFO, rslt.data_ptr);
#endif
  }

/************************************************************************/
/*                       u_a_data_ind					*/
/************************************************************************/

ST_VOID u_a_data_ind (ST_VOID *user_conn_id, ST_UINT data_len, ST_UCHAR *data)
  {
MVL_NET_INFO *ni;
MVL_COMM_EVENT *commEvent;

  ni = (MVL_NET_INFO *) user_conn_id;

/* See if we are supposed to log ACSE data for this connection		*/
  if (mvl_conn_filtered_logging == SD_FALSE || ni->log_enable)
    {
    MVL_LOG_ACSEDATA2 ("ACSE: Rx'd %d bytes on MVL_NET_INFO " S_FMT_PTR, 
					data_len, ni);
    MVL_LOG_ACSEDATAH (data_len, data);
    }

  commEvent = mvl_get_com_event ();
  if (!commEvent)
    {
    MVL_LOG_ERR0 ("ACSE: Error - could not get MVL_COMM_EVENT");
    u_a_except (user_conn_id, SD_FAILURE, __FILE__, __LINE__);
    return;
    }

  commEvent->event_type = MMS_MSG_RCVD;
  commEvent->net_info = ni;
  commEvent->u.mms.pdu_len = data_len;

  if (data_len > 0)
    {
    commEvent->u.mms.pdu = (ST_UCHAR *) M_MALLOC (MSMEM_RXPDU,data_len);
    memcpy (commEvent->u.mms.pdu, data, data_len);
    }
  else
    {
    MVL_LOG_ERR1 ("Received MMS PDU len = %d. Sending reject.", data_len);
    /* mvl_get_com_event added it to list. Must immediately remove it.	*/
    list_unlink ((ST_VOID **) &mvl_comm_event_up_head, (ST_VOID *) commEvent);
    _mvl_send_reject (commEvent);	/* this frees "commEvent" too.	*/
    }
  }

/************************************************************************/
/*                       u_a_release_ind			       	*/
/************************************************************************/

ST_VOID u_a_release_ind (ST_VOID *user_conn_id, RLRQ_APDU *info)
  {
RLRE_APDU reinfo;
MVL_NET_INFO *cc;

  cc = (MVL_NET_INFO *) user_conn_id;
  MVL_LOG_ACSE1 ("ACSE: Release Indication received on MVL_NET_INFO " S_FMT_PTR, cc);
  ++mvl_mms_statistics.serverConcl;

  cc->conn_active = SD_FALSE;
  cc->in_use = SD_FALSE;
  if (_mvlu_rpt_disconnect_rcvd_fun != NULL)
     (*_mvlu_rpt_disconnect_rcvd_fun)(cc);

  reinfo.reason_pres = SD_TRUE;
  reinfo.reason = 1;

/* Send no data on the release response */
  reinfo.user_info.len = 0;
  reinfo.user_info.ptr = NULL;
  a_release_rsp (cc->acse_conn_id, &reinfo);

/* Set any outstanding requests to 'done' */
  _mvl_set_net_req_done (NULL, cc, SD_FAILURE);

#ifdef ICCP_LITE
  if (cc->mi_in_use)
    mi_release_ind_fun (cc);
  else
    {
    if (u_mvl_disc_ind_fun)
      (*u_mvl_disc_ind_fun) (cc, MVL_ACSE_RELEASE_IND);
    }
#else
  if (u_mvl_disc_ind_fun)
    (*u_mvl_disc_ind_fun) (cc, MVL_ACSE_RELEASE_IND);
#endif

  cc->maxpend_req = 0;
  cc->numpend_req = 0;
  }

/************************************************************************/
/*                       u_a_abort_ind					*/
/************************************************************************/

ST_VOID u_a_abort_ind (ST_VOID *user_conn_id, ABRT_APDU *info)
  {
MVL_NET_INFO *cc;
MVL_ASSOC_IND *mvlAssocInd;
ST_BOOLEAN conn_active;

  cc = (MVL_NET_INFO *) user_conn_id;
  MVL_LOG_ACSE1 ("ACSE: Abort Indication received on MVL_NET_INFO " S_FMT_PTR, cc);

/* This can happen if we have not responded to an initiate indication	*/
/* In this case, just flush the initiate indication hold list		*/
  if (cc == NULL)
    {
    while (mvlAssocIndList)
      {

      mvlAssocInd = (MVL_ASSOC_IND *) list_get_first (&mvlAssocIndList);

      chk_free (mvlAssocInd);
      }
    return;
    }

  ++mvl_mms_statistics.remoteAbort;

  conn_active = cc->conn_active;
  cc->conn_active = SD_FALSE;
  cc->in_use = SD_FALSE;
  if (_mvlu_rpt_disconnect_rcvd_fun != NULL)
     (*_mvlu_rpt_disconnect_rcvd_fun)(cc);

/* Set any outstanding requests to 'done' */
  _mvl_set_net_req_done (NULL, cc, MVL_ERR_CNF_DISCONNECTED);
  if (conn_active == SD_TRUE)
    {
#ifdef ICCP_LITE
    if (cc->mi_in_use == SD_TRUE)
      mi_abort_ind_fun (cc);
    else
      {
      if (u_mvl_disc_ind_fun)
        (*u_mvl_disc_ind_fun) (cc, MVL_ACSE_ABORT_IND);
      }
#else
    if (u_mvl_disc_ind_fun)
      (*u_mvl_disc_ind_fun) (cc, MVL_ACSE_ABORT_IND);
#endif
    }

  /* If user didn't free file_list, do it here.	*/
  if (cc->file_list)
    {
    MVL_NET_FILE *open_file;
    MVL_LOG_ERR0 ("Received Abort indication with files open. Automatically closing files now.");
    while ((open_file = (MVL_NET_FILE*)list_get_first (&cc->file_list)) != NULL)
      {
      fclose (open_file->fp);
      chk_free (open_file);
      }
    }

  cc->maxpend_req = 0;
  cc->numpend_req = 0;
  }

/************************************************************************/
/*                       u_a_release_cnf				*/
/************************************************************************/

ST_VOID u_a_release_cnf (ST_VOID *user_conn_id, RLRE_APDU *info)
  {
MVL_NET_INFO *cc;
MVL_REQ_PEND *req;


  cc = (MVL_NET_INFO *) user_conn_id;

  MVL_LOG_ACSE1 ("ACSE: release confirm received for MVL_NET_INFO " S_FMT_PTR, cc);

/* Find the conclude request */

  req = _mvl_find_req_ctrl (cc, MVL_MMSOP_RELEASE, (ST_UINT32) -1);

  if (!req)
    {
    MVL_LOG_ERR0 ("ACSE: Release Confirm - no matching req ctrl");
    return;
    }

/* Set any outstanding requests to 'done' */
  _mvl_set_net_req_done (req, cc, MVL_ERR_CNF_DISCONNECTED);

  cc->conn_active = SD_FALSE;
  cc->in_use = SD_FALSE;
  if (_mvlu_rpt_disconnect_rcvd_fun != NULL)
     (*_mvlu_rpt_disconnect_rcvd_fun)(cc);

  _mvl_set_req_done (req, SD_SUCCESS);
  cc->maxpend_req = 0;
  cc->numpend_req = 0;
  }

/************************************************************************/
/*                       u_a_except					*/
/************************************************************************/

ST_VOID u_a_except (ST_VOID *user_conn_id, ST_INT code, 
		 ST_CHAR *filename, ST_INT line)
  {
  MVL_LOG_ERR2 ("ACSE: Error - Exception, code 0x%x, MVL_NET_INFO " S_FMT_PTR, 
			code, user_conn_id);
  MVL_LOG_CERR2 ("File %s, Line %d", filename, line);
  mvl_acse_ok = SD_FALSE;
  }

/************************************************************************/
/*		CONNECTION CONTROL RELATED STUFF			*/
/************************************************************************/
/*			ma_get_called_conn_ctrl				*/
/* Find available "called" conn struct. Save (NET_BIND_CTRL *) there.	*/
/************************************************************************/

#if (MMS_INIT_EN & RESP_EN)

static MVL_NET_INFO *ma_get_called_conn_ctrl (NET_BIND_CTRL *bc)
  {
MVL_NET_INFO *ni;
ST_INT i;

  ni = mvl_called_conn_ctrl;
  for (i = 0; i < mvl_cfg_info->num_called; ++i, ++ni)
    {
    /* CRITICAL: don't reuse 'ni' until req_pend_list cleaned up from	*/
    /* last conn. May be waiting for user to call mvl_free_req_ctrl.	*/
    if (!ni->in_use && ni->req_pend_list==NULL)
      {
      ni->in_use = SD_TRUE;
      ni->conn_active = SD_FALSE;
      /* Save (NET_BIND_CTRL *) in conn struct. Application may use it	*/
      /* to log ARName or other info.					*/
      /* NOTE: "ni->bind_ctrl" only valid when "ni->in_use" flag is set.*/
      ni->bind_ctrl = bc;
      return (ni);
      }
    }
  return (NULL);
  }

#endif

/************************************************************************/
/*			ma_get_calling_conn_ctrl			*/
/* Find available "calling" conn struct.				*/
/* NOTE: Never bind for "calling" conns, so bind_id arg NOT passed here.*/
/************************************************************************/
#if (MMS_INIT_EN & REQ_EN)

static MVL_NET_INFO *ma_get_calling_conn_ctrl (ST_VOID)
  {
MVL_NET_INFO *ni;
ST_INT i;

  ni = mvl_calling_conn_ctrl;
  for (i = 0; i < mvl_cfg_info->num_calling; ++i, ++ni)
    {
    /* CRITICAL: don't reuse 'ni' until req_pend_list cleaned up from	*/
    /* last conn. May be waiting for user to call mvl_free_req_ctrl.	*/
    if (!ni->in_use && ni->req_pend_list==NULL)
      {
      ni->in_use = SD_TRUE;
      ni->conn_active = SD_FALSE;
#if defined(S_SEC_ENABLED)
      /* cleanup secure connection fields */
      ni->loc_ar_sec = NULL;
      ni->rem_ar_sec = NULL;
      ni->loc_mace   = NULL;
      ni->rem_mace   = NULL;
#endif
      return (ni);
      }
    }
  return (NULL);
  }
#endif


/************************************************************************/
#endif	/* End of COACSE specific functions 				*/
/************************************************************************/

/************************************************************************/
/************************************************************************/
#if defined(CLACSE)	/* CONNECTIONLESS ACSE FUNCTIONS					*/
/************************************************************************/

/************************************************************************/
/*			mvl_init_audt_addr				*/
/* User must pass pointer (AUDT_APDU *) to their struct.		*/
/* This function fills in the addressing info in the struct.		*/
/************************************************************************/

ST_RET mvl_init_audt_addr (AUDT_APDU *audt, ST_CHAR *locArName, ST_CHAR *remArName)
  {
DIB_ENTRY *locDe;
DIB_ENTRY *remDe;

/* Retrieve the DIB_ENTRY's for local and remote. These are used to set	*/
/* the AUDT_APDU information						*/

  locDe = find_loc_dib_entry (locArName);
  if (!locDe)
    {
    MVL_LOG_NERR1 ("ACSE: Local name '%s' not found", locArName);
    return (MVLE_LOCAL_NAME);
    }

  remDe = find_rem_dib_entry (remArName);
  if (!remDe)
    {
    MVL_LOG_NERR1 ("ACSE: Remote name '%s' not found", remArName);
    return (MVLE_REMOTE_NAME);
    }

  if (remDe->pres_addr.tp_type != TP_TYPE_TP4)	/* must be TP4 (need NSAP)	*/
    {
    MVL_LOG_NERR1 ("ACSE: Remote name '%s' is not TP4 Transport type", remArName);
    return (MVLE_REMOTE_NAME);
    }

  /* AP Context is MMS */
  audt->ASO_context_name_pres = SD_TRUE;
  audt->ASO_context_name.num_comps = 5;
  audt->ASO_context_name.comps[0] = 1;
  audt->ASO_context_name.comps[1] = 0;
  audt->ASO_context_name.comps[2] = 9506;
  audt->ASO_context_name.comps[3] = 2;
  audt->ASO_context_name.comps[4] = 3;

/* Calling Addressing information */
  memcpy (&audt->calling_ae_title, &locDe->ae_title, sizeof (AE_TITLE));
  memcpy (&audt->calling_paddr, &locDe->pres_addr, sizeof (PRES_ADDR));

/* Called Addressing information */
  memcpy (&audt->called_ae_title, &remDe->ae_title, sizeof (AE_TITLE));
  memcpy (&audt->called_paddr, &remDe->pres_addr, sizeof (PRES_ADDR));

  audt->rem_mac_valid = SD_FALSE;	/* CRITICAL: must send to ALL-ES MAC*/

  return (SD_SUCCESS);
  }

/************************************************************************/
#endif		/* End of CLACSE specific functions			*/
/************************************************************************/

/************************************************************************/
/************************************************************************/
/* UTILITY FUNCTIONS							*/
/************************************************************************/
/*			ma_bind_wait					*/
/* Wait for bind/unbind confirm.					*/
/* NOTE: confirm function must set bc->cmd_done & bc->cmd_rslt.		*/
/************************************************************************/
ST_RET ma_bind_wait (NET_BIND_CTRL *bc, ST_INT timeout)
  {
ST_RET retcode;
time_t endtime;
  /* Bind/Unbind request sent. Wait for bind/unbind confirm.	*/
  endtime = time(NULL)+timeout;
  retcode = MVL_ERR_USR_TIMEOUT;	/* change if bind completes	*/
  do
    {
    sMsSleep (100);
    copp_event ();	/* Process COPP events	*/
    /* Confirm function sets bc->cmd_done & bc->cmd_rslt	*/
    if (bc->cmd_done)
      {
      retcode = bc->cmd_rslt;
      break;	/* bind_id set by confirm function. Bind is complete	*/
      }
    } while (time(NULL) < endtime);
  return (retcode);
  }

/************************************************************************/
/************************************************************************/
/* AA OBJECT HANDLING FUNCTIONS						*/
/* These functions must be here because the sizeof MVL_NET_INFO		*/
/* varies depending on the stack selection.				*/
/************************************************************************/
/************************************************************************/
/*			mvl_aa_resize 					*/
/************************************************************************/

ST_VOID mvl_aa_resize (ST_INT max_num_var, ST_INT max_num_nvl, 
		       ST_INT max_num_jou)
  {
MVL_AA_OBJ_CTRL *aa_objs;
ST_INT i;

/* Do AA specific variables */
  for (i = 0; i < mvl_cfg_info->num_called; ++i)
    {
    aa_objs = (MVL_AA_OBJ_CTRL *) mvl_called_conn_ctrl[i].aa_objs;
    if (aa_objs == NULL)
      { 
      aa_objs = mvl_called_conn_ctrl[i].aa_objs = 
		(MVL_AA_OBJ_CTRL *) M_CALLOC (NULL, 1, sizeof(MVL_AA_OBJ_CTRL));
      }
    mvl_aa_resize_conn (aa_objs, max_num_var, max_num_nvl, max_num_jou);
    }

  for (i = 0; i < mvl_cfg_info->num_calling; ++i)
    {
    aa_objs = (MVL_AA_OBJ_CTRL *) mvl_calling_conn_ctrl[i].aa_objs;
    if (aa_objs == NULL)
      { 
      aa_objs = mvl_calling_conn_ctrl[i].aa_objs = 
		(MVL_AA_OBJ_CTRL *) M_CALLOC (NULL, 1, sizeof(MVL_AA_OBJ_CTRL));
      }
    mvl_aa_resize_conn (aa_objs, max_num_var, max_num_nvl, max_num_jou);
    }
  }

/************************************************************************/
/*			_mvl_clear_aa_objs 				*/
/************************************************************************/

ST_VOID _mvl_clear_aa_objs (MVL_AA_OBJ_CTRL *aa_objs)
  {
ST_INT i;

/* Delete NVL's. We need to delete the 'entries' pointer table for each	*/
/* NVL, then delete the table of NVL's.					*/
  if (aa_objs->max_num_nvlist)
    {
    for (i = 0; i < aa_objs->num_nvlist; ++i)
      mvl_nvl_destroy (aa_objs->nvlist_tbl[i]);
    }    

/* Delete the variable association table.				*/
  if (aa_objs->max_num_var_assoc)
    {
    for (i = 0; i < aa_objs->num_var_assoc; ++i)
      M_FREE (MSMEM_GEN, aa_objs->var_assoc_tbl[i]);
    }

/* Delete the Journal control table.					*/
  if (aa_objs->max_num_jou)
    {
    for (i = 0; i < aa_objs->num_jou; ++i)
      M_FREE (MSMEM_GEN, aa_objs->jou_tbl[i]);
    }

  aa_objs->num_var_assoc = 0;
  aa_objs->num_nvlist = 0;
  aa_objs->num_jou = 0;
  }

/************************************************************************/

ST_VOID mvl_aa_resize_conn (MVL_AA_OBJ_CTRL *aa_objs, ST_INT max_num_var, ST_INT max_num_nvl, 
		       ST_INT max_num_jou)
  {
  if (max_num_var < aa_objs->num_var_assoc)
    {
    MVL_LOG_NERR0 ("Resizing VMD: new max VA is less than current"); 
    max_num_var = aa_objs->num_var_assoc;
    }
  if (max_num_nvl < aa_objs->num_nvlist)
    {
    MVL_LOG_NERR0 ("Resizing VMD: new max NVL is less than current"); 
    max_num_nvl = aa_objs->num_nvlist;
    }
  if (max_num_jou < aa_objs->num_jou)
    {
    MVL_LOG_NERR0 ("Resizing VMD: new max JOU is less than current"); 
    max_num_jou = aa_objs->num_jou;
    }
    
  _mvl_resize_ptr_tbl (&(aa_objs->max_num_var_assoc), 
  		         (ST_VOID ***)&(aa_objs->var_assoc_tbl), max_num_var);
  _mvl_resize_ptr_tbl (&(aa_objs->max_num_nvlist), 
    		         (ST_VOID ***)&(aa_objs->nvlist_tbl), max_num_nvl);
  _mvl_resize_ptr_tbl (&(aa_objs->max_num_jou), 
    		         (ST_VOID ***)&(aa_objs->jou_tbl), max_num_jou);
  }

