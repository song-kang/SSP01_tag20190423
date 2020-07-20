/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	1986 - 2012, All Rights Reserved		        */
/*									*/
/* MODULE NAME : client.c                                              	*/
/* PRODUCT(S)  : MMSEASE-LITE						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*									*/
/*   This module shows an example of client doing identify, read and 	*/
/* write with a server.							*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*	NONE								*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 03/02/12  JRB	   test_iec_control: fix oper_data_buf size.	*/
/* 02/17/12  JRB	   Add USE_DIB_LIST option.			*/
/*			   Move scl_info to main for more flexibility.	*/
/*			   Assume all reports are IEC 61850 reports.	*/
/* 			   Use strncat_maxstrlen.			*/
/* 04/29/11  JRB	   Change to new SCL file for iecremote option.	*/
/*			   Check for remote_vmd_ctrl == NULL.		*/
/* 03/31/11  JRB	   Add mvl_free_mms_objs. Call it and call	*/
/*			   osicfgx_unconf* & logcfgx_unconf* before exit.*/
/* 03/25/11  JRB	   u_set_all_leaf..: add arg.			*/
/* 06/23/10  JRB           Call subnet_serve (NEW), clnp_init, clnp_end.*/
/*			   Del call to obsolete clnp_snet_read_hook_add	*/
/*			   and related code (handled by new subnet_serve).*/
/*			   Use new defines GOOSE_RX_SUPP, GSSE_RX_SUPP,	*/
/*			   GSE_MGMT_RX_SUPP, SMPVAL_RX_SUPP, SUBNET_THREADS*/
/*			   Del #ifdef MOSI.				*/
/* 11/17/09  JRB     66    mvla_initiate_req_ex: add arg clientARName.	*/
/*			   Fix HARD_CODED_CFG for new DIB_ENTRY.	*/
/*			   Del MAP30_ACSE support.			*/
/*			   Del refs to secManCfgXmlFile (obsolete).	*/
/*			   Add casts on some printfs.			*/
/* 06/04/08  JRB     65	   Retry connectToServer.			*/
/*			   connectToServer: abort on timeout (critical).*/
/*			   disconnectFromServer: return result.		*/
/*			   Add code to test mvla_fget.			*/
/* 05/14/08  GLB     64    Added slog_ipc_std_cmd_service		*/
/* 10/15/07  JRB     63    Add rcb to linked list BEFORE rcb_enable.	*/
/* 12/04/06  JRB     62    Fix ..vmd_destroy calls.			*/
/* 11/21/06  JRB     61    Chk mode before destroying remote_vmd_ctrl.	*/
/* 11/07/06  JRB     60    Add command line option "iecremote" to test	*/
/*			   configuring with SCL.			*/
/*			   Add '01' suffix in PosReport, MeaReport.	*/
/* 06/23/06  MDE     59    Added time_t cast				*/
/* 03/20/06  JRB     58    Add code to receive 61850 Sampled Value msgs.*/
/*			   Set up callback funct clnp_snet_etype_process*/
/*			   to process all received Ethertype packets.	*/
/* 12/14/05  EJV     57    init_log_cfg: have fun proto for all options.*/
/* 10/25/05  EJV     56    S_SEC_ENABLED: added identify_response_info.	*/
/*			   Call slog_start, slog_end, slogIpcEvent.	*/
/*			   init_mem: del dupl mvl_init_glb_vars call.	*/
/* 09/09/05  JRB     55    Don't print BUILD_NUM.			*/
/* 08/05/05  EJV     54    Added ssleLogMaskMapCtrl.			*/
/* 07/22/05  JRB     53    Allow one conn to control multiple RCBs	*/
/*		 	   (user_info must point to ALL_RCB_INFO).	*/
/*			   Del test_uca_rpt, use test_iec_rpt for UCA too.*/
/*			   Exit if 'x' key is hit.			*/
/* 07/15/05  JRB     52    Add test_iec_control.			*/
/*			   Add #ifdef MVL_GOOSE_SUPPORT.		*/
/*			   test_iec_rpt: pass dom & RCB name as args.	*/
/*			   Fix mvla_status call.			*/
/*			   Ignore rpts received containing NVL.		*/
/* 06/07/05  EJV     51    Set gsLogMaskMapCtrl if S_MT_SUPPORT defined	*/
/* 06/06/05  EJV     50    gsLogMaskMapCtrl not on QNX.			*/
/* 05/24/05  EJV     49    Added logCfgAddMaskGroup (&xxxLogMaskMapCtrl)*/
/*			   Use logcfgx_ex to parse logging config file.	*/
/* 05/20/05  JRB     48    Don't print "Check log file". May not be one.*/
/* 05/20/05  JRB     47    Fix keyboard input: Use getch on WIN32, QNX.	*/
/*			   Use term_init, term_rest, getchar elsewhere.	*/
/* 08/10/04  EJV     46    secManCfgXmlFile: all lowcase "secmancfg.xml"*/
/*			   u_mvl_connect_cnf(ind)_ex: chg/typecasted ret*/
/* 08/06/04  ASK     45    Change path to secManCfg.xml for !WIN32	*/
/* 01/22/04  EJV     44    waitReqDone: replaced time() w/ sGetMsTime()	*/
/*			   Don't call ulFreeAssocSecurity on disconnect.*/
/*			   named_var_write: added chk_free()		*/
/*			   Deleted mvl_local_cl_ar_name.		*/
/* 01/06/04  EJV     43    Added security support (S_SEC_ENABLED code).	*/
/*			   Added LITESECURE version printf.		*/
/* 01/09/03  JRB     42    Fix connectToServer return value on errors.	*/
/* 12/17/03  JRB     41    61850-8-1 FDIS changes:			*/
/*			   Chg OptFlds from bvstring9 to bvstring10.	*/
/*			   Set OPTFLD_BITNUM_CONFREV bit in OptFlds.	*/			   
/* 12/15/03  JRB     40    Clear MVL_READ_RESP_PARSE_INFO before each read*/
/* 12/11/03  JRB     39    DON'T use MVLLOG_REQ/RESP/IND/CONF, they	*/
/*			   don't exist.					*/
/* 11/17/03  JRB     38    Test new IEC rpt code: Call rpt_typeids_find,*/
/*			   rcb_info_create/destroy, u_iec_rpt_ind.	*/
/*			   Include client.h. Make waitReqDone global.	*/
/*			   Use new named_var_read, named_var_write.	*/
/*			   Add '-m' cmd line arg.			*/
/*			   Pass (MVL_NET_INFO *) to functions.		*/
/* 09/30/03  EJV     37    Repl connect_ind_fun w/u_mvl_connect_ind_ex  */
/* 07/17/03  EJV     36    Added param to mvla_initiate_req_ex		*/
/* 04/23/03  JRB     35    Read sample UTF8Vstring var (UTF8Vstring13test).*/
/* 04/14/03  JRB     34    Chg main return from void to "int".		*/
/*			   Eliminate compiler warnings.			*/
/* 10/17/02  ASK     33    Added ACSE authentication sample code.	*/
/* 03/06/02  JRB     32    Del ADLC code.				*/
/* 02/25/02  MDE     31    Now get max PDU size from mvl_cfg_info	*/
/* 02/25/02  MDE     30    MAP30_ACSE cleanup				*/
/* 02/12/02  JRB     29    Fix QNX warning.				*/
/* 02/08/02  JRB     28    Pass MVL_CFG.. to osicfgx & mvl_start_acse,	*/
/*			   del mvl_init_mms.. call & mvl_local_ar.. var.*/
/* 01/24/02  EJV     27    Added sample for DOM_SPEC journals		*/
/* 11/06/01  JRB     26    Replace "synchronous" client calls with	*/
/*			   calls to "async" functs, then waitReqDone.	*/
/*			   Del _WIN32 Sleep in doCommService.		*/
/* 10/09/01  JRB     25    Use new MMSLITE_* defines for product ID.	*/ 
/* 09/21/01  JRB     24    Chg serverName to match sample cfg files.	*/
/* 05/25/01  RKR     23    Updated version to 4.01P12			*/
/* 01/04/01  EJV     22    Removed "conio.h"				*/
/* 01/05/00  MDE     21    Added MAP30_ACSE code			*/
/* 11/27/00  JRB     20    Use old CFG files for Reduced Stack & Trim-7.*/
/* 11/07/00  GLB     19    Added XML configuration files                */
/* 07/17/00  RKR     18    Updated version to 4.01P6			*/
/* 03/13/00  JRB     17    Print all errors in Hex.			*/
/* 12/15/99  GLB     16    Print informative message when program exits */
/* 10/15/99  GLB     15    Added file Rename and Obtain FIle            */
/* 03/19/99  RKR     14    Changed client & server to local1 & remote1	*/
/* 03/03/99  MDE     13    Removed reference to local_data_size 	*/
/* 11/16/98  MDE     12    Cleanup, use new conclude, add mem_chk stuff	*/
/* 11/13/98  JRB     11    Wait for event before calling mvl_comm_serve	*/
/*			   if stack is MOSI (i.e. osi???e.lib used).	*/
/* 10/13/98  JRB     10    Use MVL_JOURNAL_ENTRY, just like server.	*/
/*			   Call read_log_cfg_file to set up logging.	*/
/* 10/12/98  MDE     09    Added GetVar, GetDom, NameList services	*/
/* 09/21/98  MDE     08    Minor lint cleanup				*/
/* 08/12/98  MDE     07    Don't include mvl_defs.h (in mvl_acse.h)	*/
/* 07/16/98  MDE     06    Changed frsmid to ST_INT32			*/
/* 06/30/98  MDE     05    Use new Client Info Rpt code			*/
/* 05/22/98  JRB     04    Use new "Identify" request interface.	*/
/* 05/15/98  JRB     03    Added several new MMS Request tests.		*/
/*			   Reset doIt before disconnect so it finishes.	*/
/*			   Add u_mvl_ident_ind. Del mvl_ident_resp_info.*/
/* 12/29/97  MDE     02    Added ^c handler, removed keyboard stuff	*/
/* 09/10/97  MDE     01    MMS-LITE V4.0 Release			*/
/************************************************************************/

#if 0
  /* move outside #if to enable	*/
  #define HARD_CODED_CFG		
  #define ACSE_AUTH_ENABLED	/* for password authentication without	*/
				/* security				*/
/*  S_SEC_ENABLED		for certificate based authentication &	*/
/*				SSL encryption, put this define in	*/
/*				application's DSP/MAK file 		*/
#endif

