/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*     		2003 - 2011, All Rights Reserved		        */
/*									*/
/* MODULE NAME : sStackCfgXml.c						*/
/* PRODUCT(S)  : XXX-142-XXX						*/
/*									*/
/* MODULE DESCRIPTION : SISCO Stack Configuration (SSC)			*/
/*									*/
/*	This file provides user application interface to read		*/
/*	the SISCO Stack	and Security parameters from the		*/
/*      siscoStackCfg.xml file.						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments    			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 03/09/11  EJV	   _ssc_AR_Name_TSEL_EFun: chg to SX_USER_ERROR.*/
/* 08/17/10  JRB	   Allow PSEL, SSEL, or TSEL length = 0.	*/
/* 08/02/10  JRB	   Chk size before saving secEventLogFileName.	*/
/* 02/24/10  JRB	   Use sx_parseExx_mt.				*/
/* 02/05/10  JRB    09     Move most S_SEC_ENABLED code to new file.	*/
/* 12/16/09  EJV    08     SSC_APP_NETWORK: added MMS params (max*).	*/
/* 11/17/09  JRB    07     Put ifdef S_SEC_ENABLED around secure code	*/
/*			   (don't need to link ssec_l.lib or DLL).	*/
/* 05/14/09  EJV    06	   COrr: parse CPU_Affinity as hex.		*/
/* 01/28/09  EJV    04	   Added Sec_Events_Log_File.			*/
/* 12/19/08  EJV    04     Added Non_Secure_Fallback,			*/
/* 04/25/08  EJV    03     CORR parsing MACE Certificate Name with	*/
/*                         special characters (`,',',<,>), must use 	*/
/*			   sx_get_string not the  sx_get_string_ptr!	*/
/* 01/29/08  EJV    02     Added Tcp_Max_Idle_Time in LeanT		*/
/*			   Implemented "_ALL_" cfg for Remote AR Names.	*/
/* 07/31/07  EJV    01     Merged osill2.cfg and secManCfg.xml for V2.00*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "mem_chk.h"
#include "sx_defs.h"
#include "str_util.h"
#include "ssec.h"
#include "ssec_int.h"
#include "sstackcfg.h"

/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/
#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

static ST_RET _ssc_skip_el_start (SX_DEC_CTRL *sxDecCtrl, ST_CHAR *tag);
static ST_RET _ssc_skip_el_end   (SX_DEC_CTRL *sxDecCtrl, ST_CHAR *tag);

static ST_VOID _ssc_StartEnd_SEFun			             (SX_DEC_CTRL *sxDecCtrl);

static ST_VOID _ssc_Network_Adapter_List_SEFun			     (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _ssc_LeanT_Profile_List_SEFun			     (SX_DEC_CTRL *sxDecCtrl);

static ST_VOID _ssc_App_SNAP_SEFun				     (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _ssc_AR_Name_List_SEFun				     (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _ssc_AR_Matching_Profile_List_SEFun                   (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _ssc_Security_Profile_List_SEFun		             (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _ssc_App_Network_List_SEFun			     (SX_DEC_CTRL *sxDecCtrl);

static ST_VOID _ssc_Network_Adapter_SEFun			     (SX_DEC_CTRL *sxDecCtrl);

static ST_VOID _ssc_Network_Adapter_Name_EFun			     (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _ssc_Network_Adapter_Description_EFun		     (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _ssc_Network_Adapter_MAC_EFun			     (SX_DEC_CTRL *sxDecCtrl);

static ST_VOID _ssc_LeanT_Profile_SEFun			             (SX_DEC_CTRL *sxDecCtrl);

static ST_VOID _ssc_LeanT_Profile_Name_EFun		             (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _ssc_LeanT_Profile_Session_Disconnect_Timeout_EFun    (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _ssc_LeanT_Profile_Tcp_Enable_EFun	             (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _ssc_LeanT_Profile_Tcp_Max_Tpdu_Len_EFun              (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _ssc_LeanT_Profile_Tcp_Max_Spdu_Outstanding_EFun      (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _ssc_LeanT_Profile_Tcp_Listen_Port_EFun               (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _ssc_LeanT_Profile_Tcp_Max_Idle_Time_EFun             (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _ssc_LeanT_Profile_Tcp_Event_Wakeup_Port_Base_EFun    (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _ssc_LeanT_Profile_Tcp_Event_Wakeup_Port_Range_EFun   (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _ssc_LeanT_Profile_Gensock_Wakeup_Port_Base_EFun      (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _ssc_LeanT_Profile_Gensock_Wakeup_Port_Range_EFun     (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _ssc_LeanT_Profile_Tp4_Enable_EFun	             (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _ssc_LeanT_Profile_Tp4_Max_Tpdu_Len_EFun              (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _ssc_LeanT_Profile_Tp4_Max_Spdu_Outstanding_EFun      (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _ssc_LeanT_Profile_Tp4_Local_Credits_EFun             (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _ssc_LeanT_Profile_Tp4_Max_Remote_Credits_EFun        (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _ssc_LeanT_Profile_Tp4_Window_Time_EFun	             (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _ssc_LeanT_Profile_Tp4_Inactivity_Time_EFun	     (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _ssc_LeanT_Profile_Tp4_Retransmission_Time_EFun       (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _ssc_LeanT_Profile_Tp4_Max_Transmissions_EFun	     (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _ssc_LeanT_Profile_Tp4_Ack_Delay_EFun		     (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _ssc_LeanT_Profile_Clnp_Lifetime_EFun		     (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _ssc_LeanT_Profile_Clnp_Lifetime_Decrement_EFun       (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _ssc_LeanT_Profile_Clnp_Cfg_Timer_EFun		     (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _ssc_LeanT_Profile_Clnp_Esh_Delay_EFun		     (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _ssc_LeanT_Profile_Adapter_Name_EFun		     (SX_DEC_CTRL *sxDecCtrl);

static ST_VOID _ssc_App_SNAP_Max_Users_EFun			     (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _ssc_App_SNAP_Max_Binds_Per_User_EFun		     (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _ssc_App_SNAP_IPC_Port_EFun			     (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _ssc_App_SNAP_IPC_Domain_Socket_Path_EFun	     (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _ssc_App_SNAP_IPC_Timeout_EFun			     (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _ssc_App_SNAP_CPU_Affinity_EFun			     (SX_DEC_CTRL *sxDecCtrl);

static ST_VOID _ssc_AR_Name_SEFun			             (SX_DEC_CTRL *sxDecCtrl);

static ST_VOID _ssc_AR_Name_Name_EFun				     (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _ssc_AR_Name_Description_EFun			     (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _ssc_AR_Name_AP_Title_EFun			     (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _ssc_AR_Name_AE_Qualifier_EFun			     (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _ssc_AR_Name_PSEL_EFun				     (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _ssc_AR_Name_SSEL_EFun				     (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _ssc_AR_Name_TSEL_EFun				     (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _ssc_AR_Name_IP_Address_EFun			     (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _ssc_AR_Name_Tcp_Port_EFun			     (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _ssc_AR_Name_Authentication_Mode_EFun		     (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _ssc_AR_Name_NSAP_EFun				     (SX_DEC_CTRL *sxDecCtrl);

static ST_VOID _ssc_AR_Matching_Profile_SEFun			     (SX_DEC_CTRL *sxDecCtrl);

static ST_VOID _ssc_AR_Matching_Profile_Name_EFun		     (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _ssc_AR_Matching_Profile_Allow_Missing_AE_EFun	     (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _ssc_AR_Matching_Profile_Allow_Extra_AE_EFun	     (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _ssc_AR_Matching_Profile_Match_AP_Title_EFun	     (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _ssc_AR_Matching_Profile_Match_AE_Qualifier_EFun      (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _ssc_AR_Matching_Profile_Match_AP_Invoke_EFun	     (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _ssc_AR_Matching_Profile_Match_AE_Invoke_EFun         (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _ssc_AR_Matching_Profile_Match_PSEL_EFun	             (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _ssc_AR_Matching_Profile_Match_SSEL_EFun	             (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _ssc_AR_Matching_Profile_Match_TSEL_EFun	             (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _ssc_AR_Matching_Profile_Match_IP_Address_EFun	     (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _ssc_AR_Matching_Profile_Host_Name_Support_EFun	     (SX_DEC_CTRL *sxDecCtrl);

static ST_VOID _ssc_Security_Profile_SEFun			     (SX_DEC_CTRL *sxDecCtrl);

static ST_VOID _ssc_Security_Profile_Name_EFun			     (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _ssc_Security_Profile_Secure_EFun		     (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _ssc_Security_Profile_Non_Secure_Fallback_EFun        (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _ssc_Security_Profile_MACE_Time_Seal_Window_EFun      (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _ssc_Security_Profile_Encrypt_Req_Calling_EFun	     (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _ssc_Security_Profile_Encrypt_Req_Called_EFun	     (SX_DEC_CTRL *sxDecCtrl);

static ST_VOID _ssc_App_Network_SEFun				     (SX_DEC_CTRL *sxDecCtrl);

static ST_VOID _ssc_App_Network_App_Name_EFun			     (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _ssc_App_Network_LeanT_Profile_EFun		     (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _ssc_App_Network_Security_Profile_EFun		     (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _ssc_App_Network_AR_Matching_Profile_EFun	     (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _ssc_App_Network_App_AR_Name_EFun		     (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _ssc_App_Network_Max_Mms_Pdu_Length_EFun              (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _ssc_App_Network_Max_Connections_EFun                 (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _ssc_App_Network_Max_Calling_Connections_EFun         (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _ssc_App_Network_Max_Called_Connections_EFun          (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _ssc_App_Network_Sec_Events_Log_File_EFun	     (SX_DEC_CTRL *sxDecCtrl);

	/* =============================================================*/
	/* 	SISCO's Stack Configuration XML file parsing elements	*/
	/* =============================================================*/

static SX_ELEMENT _sscStartEnd[] = 
  {
  {"STACK_CFG",        SX_ELF_CSTARTEND,               _ssc_StartEnd_SEFun,                 NULL, 0}
  };

#if defined(S_SEC_ENABLED)
static SX_ELEMENT _ssc_Sections_With_Security_Elements[] = 
  {
  {"Network_Adapter_List",    SX_ELF_CSTARTEND | SX_ELF_OPT,    _ssc_Network_Adapter_List_SEFun,     NULL, 0},
  {"LeanT_Profile_List",      SX_ELF_CSTARTEND,                 _ssc_LeanT_Profile_List_SEFun,       NULL, 0},
  {"Cert_Parameters",         SX_ELF_CSTARTEND | SX_ELF_OPT,    _ssc_Cert_Parameters_SEFun,          NULL, 0},
  {"Trusted_CA_Cert_List",    SX_ELF_CSTARTEND | SX_ELF_OPT,    _ssc_Trusted_CA_Cert_List_SEFun,     NULL, 0},
  {"Certificate_List",        SX_ELF_CSTARTEND | SX_ELF_OPT,    _ssc_Certificate_List_SEFun,         NULL, 0},
  {"App_SNAP",                SX_ELF_CSTARTEND | SX_ELF_OPT,    _ssc_App_SNAP_SEFun,                 NULL, 0},
  {"AR_Name_List",            SX_ELF_CSTARTEND | SX_ELF_OPT,    _ssc_AR_Name_List_SEFun,             NULL, 0},
  {"AR_Matching_Profile_List",SX_ELF_CSTARTEND | SX_ELF_OPT,    _ssc_AR_Matching_Profile_List_SEFun, NULL, 0},
  {"Security_Profile_List",   SX_ELF_CSTARTEND,                 _ssc_Security_Profile_List_SEFun,    NULL, 0},
  {"App_Network_List",        SX_ELF_CSTARTEND,                 _ssc_App_Network_List_SEFun,         NULL, 0}
  };
#endif	/* defined(S_SEC_ENABLED)	*/

static SX_ELEMENT _ssc_Sections_Without_Security_Elements[] = 
  {
  {"Network_Adapter_List",    SX_ELF_CSTARTEND | SX_ELF_OPT,    _ssc_Network_Adapter_List_SEFun,     NULL, 0},
  {"LeanT_Profile_List",      SX_ELF_CSTARTEND,                 _ssc_LeanT_Profile_List_SEFun,       NULL, 0},
  {"App_SNAP",                SX_ELF_CSTARTEND | SX_ELF_OPT,    _ssc_App_SNAP_SEFun,                 NULL, 0},
  {"AR_Name_List",            SX_ELF_CSTARTEND | SX_ELF_OPT,    _ssc_AR_Name_List_SEFun,             NULL, 0},
  {"AR_Matching_Profile_List",SX_ELF_CSTARTEND | SX_ELF_OPT,    _ssc_AR_Matching_Profile_List_SEFun, NULL, 0},
  {"Security_Profile_List",   SX_ELF_CSTARTEND,                 _ssc_Security_Profile_List_SEFun,    NULL, 0},
  {"App_Network_List",        SX_ELF_CSTARTEND,                 _ssc_App_Network_List_SEFun,         NULL, 0}
  };

static SX_ELEMENT _ssc_Network_Adapter_List_Elements[] = 
  {
  {"Network_Adapter",         SX_ELF_CSTARTEND | SX_ELF_OPTRPT, _ssc_Network_Adapter_SEFun,             NULL, 0}
  };
static SX_ELEMENT _ssc_Network_Adapter_Elements[] = 
  {
  {"Name",                    SX_ELF_CEND,                      _ssc_Network_Adapter_Name_EFun,         NULL, 0},
  {"Description",             SX_ELF_CEND | SX_ELF_OPT,         _ssc_Network_Adapter_Description_EFun,  NULL, 0},
  {"MAC",                     SX_ELF_CEND,                      _ssc_Network_Adapter_MAC_EFun,          NULL, 0}
  };

static SX_ELEMENT _ssc_LeanT_Profile_List_Elements[] = 
  {
  {"LeanT_Profile",             SX_ELF_CSTARTEND | SX_ELF_RPT,  _ssc_LeanT_Profile_SEFun,                           NULL, 0}
  };
static SX_ELEMENT _ssc_LeanT_Profile_Elements[] = 
  {
  {"Name",                      SX_ELF_CEND,                    _ssc_LeanT_Profile_Name_EFun,                       NULL, 0},
  {"Session_Disconnect_Timeout",SX_ELF_CEND | SX_ELF_OPT,       _ssc_LeanT_Profile_Session_Disconnect_Timeout_EFun, NULL, 0},
  {"Tcp_Enable",                SX_ELF_CEND,                    _ssc_LeanT_Profile_Tcp_Enable_EFun,                 NULL, 0},
  {"Tcp_Max_Tpdu_Len",          SX_ELF_CEND | SX_ELF_OPT,       _ssc_LeanT_Profile_Tcp_Max_Tpdu_Len_EFun,           NULL, 0},
  {"Tcp_Max_Spdu_Outstanding",  SX_ELF_CEND | SX_ELF_OPT,       _ssc_LeanT_Profile_Tcp_Max_Spdu_Outstanding_EFun,   NULL, 0},
  {"Tcp_Listen_Port",           SX_ELF_CEND | SX_ELF_OPT,       _ssc_LeanT_Profile_Tcp_Listen_Port_EFun,            NULL, 0},
  {"Tcp_Max_Idle_Time",         SX_ELF_CEND | SX_ELF_OPT,       _ssc_LeanT_Profile_Tcp_Max_Idle_Time_EFun,          NULL, 0},
  {"Tcp_Event_Wakeup_Port_Base",  SX_ELF_CEND | SX_ELF_OPT,     _ssc_LeanT_Profile_Tcp_Event_Wakeup_Port_Base_EFun, NULL, 0},
  {"Tcp_Event_Wakeup_Port_Range", SX_ELF_CEND | SX_ELF_OPT,     _ssc_LeanT_Profile_Tcp_Event_Wakeup_Port_Range_EFun,NULL, 0},
  {"Gensock_Wakeup_Port_Base",    SX_ELF_CEND | SX_ELF_OPT,     _ssc_LeanT_Profile_Gensock_Wakeup_Port_Base_EFun,   NULL, 0},
  {"Gensock_Wakeup_Port_Range",   SX_ELF_CEND | SX_ELF_OPT,     _ssc_LeanT_Profile_Gensock_Wakeup_Port_Range_EFun,  NULL, 0},
  {"Tp4_Enable",                SX_ELF_CEND,                    _ssc_LeanT_Profile_Tp4_Enable_EFun,                 NULL, 0},
  {"Tp4_Max_Tpdu_Len",          SX_ELF_CEND | SX_ELF_OPT,       _ssc_LeanT_Profile_Tp4_Max_Tpdu_Len_EFun,           NULL, 0},
  {"Tp4_Max_Spdu_Outstanding",  SX_ELF_CEND | SX_ELF_OPT,       _ssc_LeanT_Profile_Tp4_Max_Spdu_Outstanding_EFun,   NULL, 0},
  {"Tp4_Local_Credits",         SX_ELF_CEND | SX_ELF_OPT,       _ssc_LeanT_Profile_Tp4_Local_Credits_EFun,          NULL, 0},
  {"Tp4_Max_Remote_Credits",    SX_ELF_CEND | SX_ELF_OPT,       _ssc_LeanT_Profile_Tp4_Max_Remote_Credits_EFun,     NULL, 0},
  {"Tp4_Window_Time",           SX_ELF_CEND | SX_ELF_OPT,       _ssc_LeanT_Profile_Tp4_Window_Time_EFun,            NULL, 0},
  {"Tp4_Inactivity_Time",       SX_ELF_CEND | SX_ELF_OPT,       _ssc_LeanT_Profile_Tp4_Inactivity_Time_EFun,        NULL, 0},
  {"Tp4_Retransmission_Time",   SX_ELF_CEND | SX_ELF_OPT,       _ssc_LeanT_Profile_Tp4_Retransmission_Time_EFun,    NULL, 0},
  {"Tp4_Max_Transmissions",     SX_ELF_CEND | SX_ELF_OPT,       _ssc_LeanT_Profile_Tp4_Max_Transmissions_EFun,      NULL, 0},
  {"Tp4_Ack_Delay",             SX_ELF_CEND | SX_ELF_OPT,       _ssc_LeanT_Profile_Tp4_Ack_Delay_EFun,              NULL, 0},
  {"Clnp_Lifetime",             SX_ELF_CEND | SX_ELF_OPT,       _ssc_LeanT_Profile_Clnp_Lifetime_EFun,              NULL, 0},
  {"Clnp_Lifetime_Decrement",   SX_ELF_CEND | SX_ELF_OPT,       _ssc_LeanT_Profile_Clnp_Lifetime_Decrement_EFun,    NULL, 0},
  {"Clnp_Cfg_Timer",            SX_ELF_CEND | SX_ELF_OPT,       _ssc_LeanT_Profile_Clnp_Cfg_Timer_EFun,             NULL, 0},
  {"Clnp_Esh_Delay",            SX_ELF_CEND | SX_ELF_OPT,       _ssc_LeanT_Profile_Clnp_Esh_Delay_EFun,             NULL, 0},
  {"Adapter_Name",              SX_ELF_CEND | SX_ELF_OPT,       _ssc_LeanT_Profile_Adapter_Name_EFun,               NULL, 0}
  };

static SX_ELEMENT _ssc_App_SNAP_Elements[] =
  {
#if defined(S_SEC_ENABLED)
  /* SSL connection parameters */
  {"Local_SSL_Certificate",        SX_ELF_CEND      | SX_ELF_OPT,  _ssc_App_SNAP_Local_SSL_Certificate_EFun,             NULL, 0}, /* for backward compatibility		*/
  {"Local_SSL_Certificate_List",   SX_ELF_CSTARTEND | SX_ELF_OPT,  _ssc_App_SNAP_Local_SSL_Certificate_List_SEFun,       NULL, 0}, /* now up to 2 certs are allowed in list	*/
  {"SSL_Called_Port",              SX_ELF_CEND      | SX_ELF_OPT,  _ssc_App_SNAP_SSL_Called_Port_EFun,                   NULL, 0},
  {"SSL_Calling_Port",             SX_ELF_CEND      | SX_ELF_OPT,  _ssc_App_SNAP_SSL_Calling_Port_EFun,                  NULL, 0},
  {"SSL_Called_Peer_Cert_Known",   SX_ELF_CEND      | SX_ELF_OPT,  _ssc_App_SNAP_SSL_Called_Peer_Cert_Known_EFun,        NULL, 0},
  {"SSL_Calling_Peer_Cert_Known",  SX_ELF_CEND      | SX_ELF_OPT,  _ssc_App_SNAP_SSL_Calling_Peer_Cert_Known_EFun,       NULL, 0},
  {"Max_Tran_Per_Key",             SX_ELF_CEND      | SX_ELF_OPT,  _ssc_App_SNAP_Max_Tran_Per_Key_EFun,                  NULL, 0},
  {"Max_Time_Per_Key",   	   SX_ELF_CEND      | SX_ELF_OPT,  _ssc_App_SNAP_Max_Time_Per_Key_EFun,                  NULL, 0},
  {"Rekey_Timeout",                SX_ELF_CEND      | SX_ELF_OPT,  _ssc_App_SNAP_Rekey_Timeout_EFun,                     NULL, 0},
  {"Cipher_List",                  SX_ELF_CSTARTEND | SX_ELF_OPT,  _ssc_App_SNAP_Cipher_List_SEFun,                      NULL, 0},

  /* SNAP-Lite parameters for SSL connections */
  {"SSL_Max_Called",               SX_ELF_CEND      | SX_ELF_OPT,  _ssc_App_SNAP_SSL_Max_Called_EFun,          NULL, 0},
  {"SSL_Max_Calling",              SX_ELF_CEND      | SX_ELF_OPT,  _ssc_App_SNAP_SSL_Max_Calling_EFun,         NULL, 0},
  {"SSL_Proxy_Called_Port",        SX_ELF_CEND      | SX_ELF_OPT,  _ssc_App_SNAP_SSL_Proxy_Called_Port_EFun,   NULL, 0},
  {"SSL_Proxy_Calling_Port",       SX_ELF_CEND      | SX_ELF_OPT,  _ssc_App_SNAP_SSL_Proxy_Calling_Port_EFun,  NULL, 0},
  {"SSL_Connect_Timeout",          SX_ELF_CEND      | SX_ELF_OPT,  _ssc_App_SNAP_SSL_Connect_Timeout_EFun,     NULL, 0},
#endif	/* defined(S_SEC_ENABLED)	*/

  /* SNAP-Lite parameters for IPC and non-secure connections */
  {"Max_Users",                    SX_ELF_CEND      | SX_ELF_OPT,  _ssc_App_SNAP_Max_Users_EFun,               NULL, 0},
  {"Max_Binds_Per_User",           SX_ELF_CEND      | SX_ELF_OPT,  _ssc_App_SNAP_Max_Binds_Per_User_EFun,      NULL, 0},
  {"IPC_Port",                     SX_ELF_CEND      | SX_ELF_OPT,  _ssc_App_SNAP_IPC_Port_EFun,                NULL, 0},
  {"IPC_Domain_Socket_Path",       SX_ELF_CEND      | SX_ELF_OPT,  _ssc_App_SNAP_IPC_Domain_Socket_Path_EFun,  NULL, 0},
  {"IPC_Timeout",                  SX_ELF_CEND      | SX_ELF_OPT,  _ssc_App_SNAP_IPC_Timeout_EFun,             NULL, 0},
  {"CPU_Affinity",                 SX_ELF_CEND      | SX_ELF_OPT,  _ssc_App_SNAP_CPU_Affinity_EFun,            NULL, 0}
  };								  

static SX_ELEMENT _ssc_AR_Name_List_Elements[] = 
  {
  {"AR_Name",                      SX_ELF_CSTARTEND | SX_ELF_OPTRPT,  _ssc_AR_Name_SEFun,                      NULL, 0}
  };

static SX_ELEMENT _ssc_AR_Name_Elements[] = 
  {
  {"Name",                         SX_ELF_CEND,                  _ssc_AR_Name_Name_EFun,                       NULL, 0},
  {"Description",                  SX_ELF_CEND | SX_ELF_OPT,     _ssc_AR_Name_Description_EFun,                NULL, 0},
  {"AP_Title",                     SX_ELF_CEND | SX_ELF_OPT,     _ssc_AR_Name_AP_Title_EFun,                   NULL, 0},
  {"AE_Qualifier",                 SX_ELF_CEND | SX_ELF_OPT,     _ssc_AR_Name_AE_Qualifier_EFun,               NULL, 0},
  {"PSEL",                         SX_ELF_CEND | SX_ELF_OPT,     _ssc_AR_Name_PSEL_EFun,                       NULL, 0},
  {"SSEL",                         SX_ELF_CEND | SX_ELF_OPT,     _ssc_AR_Name_SSEL_EFun,                       NULL, 0},
  {"TSEL",                         SX_ELF_CEND | SX_ELF_OPT,     _ssc_AR_Name_TSEL_EFun,                       NULL, 0},
  {"IP_Address",                   SX_ELF_CEND | SX_ELF_OPT,     _ssc_AR_Name_IP_Address_EFun,                 NULL, 0},
  {"Tcp_Port",                     SX_ELF_CEND | SX_ELF_OPT,     _ssc_AR_Name_Tcp_Port_EFun,                   NULL, 0},
  {"Authentication_Mode",          SX_ELF_CEND | SX_ELF_OPT,     _ssc_AR_Name_Authentication_Mode_EFun,        NULL, 0},
#if defined(S_SEC_ENABLED)
  {"Authentication_Certificate",       SX_ELF_CEND      | SX_ELF_OPTRPT,  _ssc_AR_Name_Authentication_Certificate_EFun,       NULL, 0},  /* for backward compatibility	*/
  {"Authentication_Certificate_List",  SX_ELF_CSTARTEND | SX_ELF_OPT,     _ssc_AR_Name_Authentication_Certificate_List_SEFun, NULL, 0},  /* now put all certs in list	*/
  {"Encryption",                   SX_ELF_CEND | SX_ELF_OPT,     _ssc_AR_Name_Encryption_EFun,                 NULL, 0},
  {"SSL_Port",                     SX_ELF_CEND | SX_ELF_OPT,     _ssc_AR_Name_SSL_Port_EFun,                   NULL, 0},
#endif	/* defined(S_SEC_ENABLED)	*/
  {"NSAP",                         SX_ELF_CEND | SX_ELF_OPT,     _ssc_AR_Name_NSAP_EFun,                       NULL, 0}
  };

static SX_ELEMENT _ssc_AR_Matching_Profile_List_Elements[] = 
  {
  {"AR_Matching_Profile",         SX_ELF_CSTARTEND | SX_ELF_OPTRPT,  _ssc_AR_Matching_Profile_SEFun, NULL, 0}
  };

static SX_ELEMENT _ssc_AR_Matching_Profile_Elements[] = 
  {
  {"Name",                         SX_ELF_CEND,               _ssc_AR_Matching_Profile_Name_EFun,               NULL, 0},
  {"Allow_Missing_AE",             SX_ELF_CEND | SX_ELF_OPT,  _ssc_AR_Matching_Profile_Allow_Missing_AE_EFun,   NULL, 0},
  {"Allow_Extra_AE",               SX_ELF_CEND | SX_ELF_OPT,  _ssc_AR_Matching_Profile_Allow_Extra_AE_EFun,     NULL, 0},
  {"Match_AP_Title",               SX_ELF_CEND | SX_ELF_OPT,  _ssc_AR_Matching_Profile_Match_AP_Title_EFun,     NULL, 0},
  {"Match_AE_Qualifier",           SX_ELF_CEND | SX_ELF_OPT,  _ssc_AR_Matching_Profile_Match_AE_Qualifier_EFun, NULL, 0},
  {"Match_AP_Invoke",              SX_ELF_CEND | SX_ELF_OPT,  _ssc_AR_Matching_Profile_Match_AP_Invoke_EFun,    NULL, 0},
  {"Match_AE_Invoke",              SX_ELF_CEND | SX_ELF_OPT,  _ssc_AR_Matching_Profile_Match_AE_Invoke_EFun,    NULL, 0},
  {"Match_PSEL",                   SX_ELF_CEND | SX_ELF_OPT,  _ssc_AR_Matching_Profile_Match_PSEL_EFun,         NULL, 0},
  {"Match_SSEL",                   SX_ELF_CEND | SX_ELF_OPT,  _ssc_AR_Matching_Profile_Match_SSEL_EFun,         NULL, 0},
  {"Match_TSEL",                   SX_ELF_CEND | SX_ELF_OPT,  _ssc_AR_Matching_Profile_Match_TSEL_EFun,         NULL, 0},
  {"Match_IP_Address",             SX_ELF_CEND | SX_ELF_OPT,  _ssc_AR_Matching_Profile_Match_IP_Address_EFun,   NULL, 0},
  {"Host_Name_Support",            SX_ELF_CEND | SX_ELF_OPT,  _ssc_AR_Matching_Profile_Host_Name_Support_EFun,  NULL, 0}
  };

static SX_ELEMENT _ssc_Security_Profile_List_Elements[] = 
  {
  {"Security_Profile",             SX_ELF_CSTARTEND | SX_ELF_RPT,  _ssc_Security_Profile_SEFun,    NULL, 0}
  };

static SX_ELEMENT _ssc_Security_Profile_Elements[] = 
  {
  {"Name",                         SX_ELF_CEND,               _ssc_Security_Profile_Name_EFun,                  NULL, 0},
  {"Secure",                       SX_ELF_CEND,               _ssc_Security_Profile_Secure_EFun,                NULL, 0},
  {"Non_Secure_Fallback",          SX_ELF_CEND | SX_ELF_OPT,  _ssc_Security_Profile_Non_Secure_Fallback_EFun,   NULL, 0},
  {"MACE_Time_Seal_Window",        SX_ELF_CEND | SX_ELF_OPT,  _ssc_Security_Profile_MACE_Time_Seal_Window_EFun, NULL, 0},
  {"Encrypt_Req_Calling",          SX_ELF_CEND | SX_ELF_OPT,  _ssc_Security_Profile_Encrypt_Req_Calling_EFun,   NULL, 0},
  {"Encrypt_Req_Called",           SX_ELF_CEND | SX_ELF_OPT,  _ssc_Security_Profile_Encrypt_Req_Called_EFun,    NULL, 0}
  };

static SX_ELEMENT _ssc_App_Network_List_Elements[] = 
  {
  {"App_Network",                  SX_ELF_CSTARTEND | SX_ELF_RPT,  _ssc_App_Network_SEFun,         NULL, 0}
  };

static SX_ELEMENT _ssc_App_Network_Elements[] = 
  {
  {"App_Name",                     SX_ELF_CEND,                 _ssc_App_Network_App_Name_EFun,            NULL, 0},
  {"LeanT_Profile",                SX_ELF_CEND,                 _ssc_App_Network_LeanT_Profile_EFun,       NULL, 0},
  {"Security_Profile",             SX_ELF_CEND,                 _ssc_App_Network_Security_Profile_EFun,    NULL, 0},
  {"AR_Matching_Profile",          SX_ELF_CEND | SX_ELF_OPT,    _ssc_App_Network_AR_Matching_Profile_EFun, NULL, 0},
  {"App_AR_Name",                  SX_ELF_CEND | SX_ELF_OPTRPT, _ssc_App_Network_App_AR_Name_EFun,         NULL, 0},
  {"Max_Mms_Pdu_Length",           SX_ELF_CEND | SX_ELF_OPT,    _ssc_App_Network_Max_Mms_Pdu_Length_EFun,        NULL, 0},
  {"Max_Connections",              SX_ELF_CEND | SX_ELF_OPT,    _ssc_App_Network_Max_Connections_EFun,           NULL, 0},
  {"Max_Calling_Connections",      SX_ELF_CEND | SX_ELF_OPT,    _ssc_App_Network_Max_Calling_Connections_EFun,   NULL, 0},
  {"Max_Called_Connections",       SX_ELF_CEND | SX_ELF_OPT,    _ssc_App_Network_Max_Called_Connections_EFun,    NULL, 0},
  {"Sec_Events_Log_File",          SX_ELF_CEND | SX_ELF_OPT,    _ssc_App_Network_Sec_Events_Log_File_EFun,  NULL, 0}
  };


 	/* =============================================================*/
	/* 	SISCO's Stack Configuration user interface functions	*/
	/* =============================================================*/


/************************************************************************/
/*			sscReadStackCfgFromXmlFile 			*/
/*----------------------------------------------------------------------*/
/* Allocates the SSC_STACK_CFG pointer and loads the SISCO Stack	*/
/* configuration from a XML file.					*/
/* Return:								*/
/*	SD_SUCCESS			if function successful,		*/
/*	SX_xxx error or SD_FAILURE	otherwise			*/
/************************************************************************/
ST_RET sscReadStackCfgFromXmlFile (ST_CHAR *fileName, SSC_STACK_CFG **sscStackCfgOut)
{
ST_RET         rtn = SD_FAILURE;
SSC_STACK_CFG  *sscStackCfg = NULL;

  SSC_LOG_FLOW1 ("SSC PARSE INFO: decoding SISCO Stack Configuration file: '%s'.", fileName);

  sscStackCfg = (SSC_STACK_CFG *) chk_calloc (1, sizeof (SSC_STACK_CFG));

  sscStackCfg->cfgVersion = SSC_VERSION_1;
  /*sscStackCfg->cfgVersionTime;*/	/* DEBUG: what is the time representation ?	*/

  /* set the secureDllLib to skip parsing security parameters when	*/
  /* NULL Security DLL (or shared lib on UNIX/Linux) is present.	*/
#if defined(S_SEC_ENABLED)
  /* This function is in DLL.	*/
  sscStackCfg->secureDllLib = ssleSecuritySupported ();
#else
  sscStackCfg->secureDllLib = SD_FALSE;	/* no DLL, so can't be secure	*/
#endif
  if (!sscStackCfg->secureDllLib)
    SSC_LOG_FLOW0 ("SSC PARSE INFO: security extension not installed.");

  /* NOTE: the parsing function has limited checking of configuration parameters.	*/
  /*        More checking will be done in the sscConfigAppNetwork.			*/
  rtn = sx_parseExx_mt (fileName, sizeof (_sscStartEnd)/sizeof (SX_ELEMENT),
                       _sscStartEnd, sscStackCfg, _ssc_skip_el_start, _ssc_skip_el_end);

  if (rtn == SD_SUCCESS)
    *sscStackCfgOut = sscStackCfg;
  else
    {
    sscFreeStackCfgPtr (sscStackCfg);

    if (rtn == SX_FILE_NOT_FOUND)
      SSC_LOG_ERR1 ("SSC ERROR: SISCO Stack Configuration file '%s' Open Error (or file not found).", fileName);
    else if (rtn == SX_STRUCT_NOT_FOUND)
      SSC_LOG_ERR3 ("SSC ERROR: SISCO Stack Configuration file '%s'\n"
                    "  parsing error rtn=%d (%s) - unknown section name.",
                    fileName, rtn, SX_ERR_STR(rtn));
    else
      SSC_LOG_ERR3 ("SSC ERROR: SISCO Stack Configuration file '%s'\n"
                    "  parsing error rtn=%d (%s).",
                    fileName, rtn, SX_ERR_STR(rtn));
    }

  return (rtn);
}



	/* =============================================================*/
	/* 	SISCO's Stack Configuration parsing functions		*/
	/* =============================================================*/


/************************************************************************/
/*			_ssc_skip_el_start				*/
/* SX called function to ignore some sections in the XML file when	*/
/* the sscStackCfg->secureDllLib=SD_FALSE.				*/
/************************************************************************/

static ST_RET _ssc_skip_el_start (SX_DEC_CTRL *sxDecCtrl, ST_CHAR *tag)
  {
ST_RET         rtn = SD_FAILURE;	/* if secured DDL, don't skip elements	*/
SSC_STACK_CFG *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;

  if (!sscStackCfg->secureDllLib)
    {
    SSC_LOG_CFLOW1 ("SSC PARSE INFO: not secured app, skipping security element '%s'", tag);
    rtn = SD_SUCCESS;
    }
  return (rtn);
  }

/************************************************************************/
/*			_ssc_skip_el_end				*/
/************************************************************************/

static ST_RET _ssc_skip_el_end (SX_DEC_CTRL *sxDecCtrl, ST_CHAR *tag)
  {
  return (SD_SUCCESS);
  }


/************************************************************************/
/*			_ssc_StartEnd_SEFun				*/
/************************************************************************/
static ST_VOID _ssc_StartEnd_SEFun (SX_DEC_CTRL *sxDecCtrl)
{
#if defined(S_SEC_ENABLED)
SSC_STACK_CFG *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;
#endif
  if (sxDecCtrl->reason == SX_ELEMENT_START)
    {
    SSC_LOG_CFLOW0 ("SSC PARSE: 'STACK_CFG' start\n");
#if defined(S_SEC_ENABLED)
    if (sscStackCfg->secureDllLib)
     SX_PUSH (_ssc_Sections_With_Security_Elements);
    else
#endif
     SX_PUSH (_ssc_Sections_Without_Security_Elements);
    }
  else
    SSC_LOG_CFLOW0 ("SSC PARSE: 'STACK_CFG' end\n");
}

/************************************************************************/
/*			_ssc_Network_Adapter_List_SEFun			*/
/************************************************************************/
static ST_VOID _ssc_Network_Adapter_List_SEFun (SX_DEC_CTRL *sxDecCtrl)
{
  if (sxDecCtrl->reason == SX_ELEMENT_START)
    {
    SSC_LOG_CFLOW0 ("SSC PARSE:  'Network_Adapter_List' start\n");
    SX_PUSH (_ssc_Network_Adapter_List_Elements);
    }
  else
    SSC_LOG_CFLOW0 ("SSC PARSE:  'Network_Adapter_List' end\n");
}

/************************************************************************/
/*			_ssc_LeanT_Profile_List_SEFun			*/
/************************************************************************/
static ST_VOID _ssc_LeanT_Profile_List_SEFun (SX_DEC_CTRL *sxDecCtrl)
{
  if (sxDecCtrl->reason == SX_ELEMENT_START)
    {
    SSC_LOG_CFLOW0 ("SSC PARSE:  'LeanT_Profile_List' start\n");
    SX_PUSH (_ssc_LeanT_Profile_List_Elements);
    }
  else
    SSC_LOG_CFLOW0 ("SSC PARSE:  'LeanT_Profile_List' end\n");
}

/************************************************************************/
/*			_ssc_App_SNAP_SEFun				*/
/************************************************************************/
static ST_VOID _ssc_App_SNAP_SEFun (SX_DEC_CTRL *sxDecCtrl)
{
SSC_STACK_CFG        *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;

  if (sxDecCtrl->reason == SX_ELEMENT_START)
    {
    /* keep sslParams and snapParam pointers independent in case we	*/
    /* need to separate SSL params for !SNAP_LITE case.		*/
    if (sscAddSnapParam (sscStackCfg))
      {
      SSC_LOG_CFLOW0 ("SSC PARSE:  'App_SNAP' start");
      SX_PUSH (_ssc_App_SNAP_Elements);
      }
    else
      {
      SSC_LOG_ERR0 ("SSC ERROR: 'Add SNAP Param failed.");
      sxDecCtrl->errCode = SX_USER_ERROR;
      return;
      }
    if (sscAddSslParam (sscStackCfg))
      {
      SX_PUSH (_ssc_App_SNAP_Elements);
      }
    else
      {
      SSC_LOG_ERR0 ("SSC ERROR: 'Add SSL SNAP Param failed.");
      sxDecCtrl->errCode = SX_USER_ERROR;
      return;
      }
    }
  else
    SSC_LOG_CFLOW0 ("SSC PARSE:  'App_SNAP' end\n");
}

/************************************************************************/
/*			_ssc_AR_Name_List_SEFun				*/
/************************************************************************/
static ST_VOID _ssc_AR_Name_List_SEFun (SX_DEC_CTRL *sxDecCtrl)
{
  if (sxDecCtrl->reason == SX_ELEMENT_START)
    {
    SSC_LOG_CFLOW0 ("SSC PARSE:  'AR_Name_List' start\n");
    SX_PUSH (_ssc_AR_Name_List_Elements);
    }
  else
    SSC_LOG_CFLOW0 ("SSC PARSE:  'AR_Name_List' end\n");
}

/************************************************************************/
/*			_ssc_AR_Matching_Profile_List_SEFun		*/
/************************************************************************/
static ST_VOID _ssc_AR_Matching_Profile_List_SEFun (SX_DEC_CTRL *sxDecCtrl)
{
  if (sxDecCtrl->reason == SX_ELEMENT_START)
    {
    SSC_LOG_CFLOW0 ("SSC PARSE:  'AR_Matching_Profile_List' start\n");
    SX_PUSH (_ssc_AR_Matching_Profile_List_Elements);
    }
  else
    SSC_LOG_CFLOW0 ("SSC PARSE:  'AR_Matching_Profile_List' end\n");
}

/************************************************************************/
/*			_ssc_Security_Profile_List_SEFun		*/
/************************************************************************/
static ST_VOID _ssc_Security_Profile_List_SEFun (SX_DEC_CTRL *sxDecCtrl)
{
  if (sxDecCtrl->reason == SX_ELEMENT_START)
    {
    SSC_LOG_CFLOW0 ("SSC PARSE:  'Security_Profile_List' start\n");
    SX_PUSH (_ssc_Security_Profile_List_Elements);
    }
  else
    SSC_LOG_CFLOW0 ("SSC PARSE:  'Security_Profile_List' end\n");
}

/************************************************************************/
/*			_ssc_App_Network_List_SEFun			*/
/************************************************************************/
static ST_VOID _ssc_App_Network_List_SEFun (SX_DEC_CTRL *sxDecCtrl)
{
  if (sxDecCtrl->reason == SX_ELEMENT_START)
    {
    SSC_LOG_CFLOW0 ("SSC PARSE:  'App_Network_List' start\n");
    SX_PUSH (_ssc_App_Network_List_Elements);
    }
  else
    SSC_LOG_CFLOW0 ("SSC PARSE:  'App_Network_List' end\n");
}


/************************************************************************/
/*			_ssc_Network_Adapter_SEFun			*/
/************************************************************************/
static ST_VOID _ssc_Network_Adapter_SEFun (SX_DEC_CTRL *sxDecCtrl)
{
SSC_STACK_CFG *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;

  if (sxDecCtrl->reason == SX_ELEMENT_START)
    {
    if (sscAddNetworkAdapter (sscStackCfg))
      {
      SSC_LOG_CFLOW0 ("SSC PARSE:    'Network_Adapter' start");
      SX_PUSH (_ssc_Network_Adapter_Elements);
      }
    else
      {
      SSC_LOG_ERR0 ("SSC ERROR: 'Add Network_Adapter failed.");
      sxDecCtrl->errCode = SX_USER_ERROR;
      }
    }
  else
    SSC_LOG_CFLOW0 ("SSC PARSE:    'Network_Adapter' end\n");
}

/************************************************************************/
/*			_ssc_Network_Adapter_Name_EFun 			*/
/************************************************************************/
static ST_VOID _ssc_Network_Adapter_Name_EFun (SX_DEC_CTRL *sxDecCtrl)
{
ST_RET                rtn;
SSC_STACK_CFG        *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;
SSC_NETWORK_ADAPTER  *networkAdapter;
ST_CHAR              *networkAdapterName;
ST_INT                len;

  networkAdapter = (SSC_NETWORK_ADAPTER *) list_find_last ((DBL_LNK *) sscStackCfg->networkAdapterList);
  if (!networkAdapter)
    {
    SSC_LOG_ERR0 ("SSC ERROR: 'Network Adapter.Name' processing failed (networkAdapter=NULL).");
    sxDecCtrl->errCode = SX_USER_ERROR;
    return;
    }

  len = sizeof (networkAdapter->name) - 1; /* -1 for string terminator	*/
  rtn = sx_get_string_ptr (sxDecCtrl, &networkAdapterName, &len);
  if (rtn == SD_SUCCESS)
    {
    if (len > 0)
      {
      /* prevent duplicate names */
      if (!sscFindNetworkAdapter (sscStackCfg, networkAdapterName))
        {
	strcpy (networkAdapter->name, networkAdapterName);
        SSC_LOG_CFLOW1 ("SSC PARSE:      'Name'        ='%s'", networkAdapter->name);
	}
      else
        {
        SSC_LOG_ERR1 ("SSC ERROR: duplicate 'Network_Adapter.Name'='%s' found.", networkAdapterName);
        sxDecCtrl->errCode = SX_USER_ERROR;
	}
      }
    else
      {
      SSC_LOG_ERR0 ("SSC ERROR: 'Network_Adapter.Name' not supplied.");
      sxDecCtrl->errCode = SX_USER_ERROR;
      }
    }
  else
    SSC_LOG_ERR0 ("SSC ERROR: 'Network Adapter.Name' failed to parse from XML.");
}

/************************************************************************/
/*			_ssc_Network_Adapter_Description_EFun		*/
/************************************************************************/
static ST_VOID _ssc_Network_Adapter_Description_EFun (SX_DEC_CTRL *sxDecCtrl)
{
ST_RET                rtn;
SSC_STACK_CFG        *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;
SSC_NETWORK_ADAPTER  *networkAdapter;
ST_INT                len;

  networkAdapter = (SSC_NETWORK_ADAPTER *) list_find_last ((DBL_LNK *) sscStackCfg->networkAdapterList);
  if (!networkAdapter)
    {
    SSC_LOG_ERR0 ("SSC ERROR: 'Network Adapter.Description' processing failed (networkAdapter=NULL).");
    sxDecCtrl->errCode = SX_USER_ERROR;
    return;
    }

  len = sizeof (networkAdapter->description) - 1; /* -1 for string terminator	*/
  rtn = sx_get_string (sxDecCtrl, networkAdapter->description, &len);
  if (rtn == SD_SUCCESS)
    {
    if (len > 0)
      SSC_LOG_CFLOW1 ("SSC PARSE:      'Description' ='%s'", networkAdapter->description);
    }
  else
    SSC_LOG_ERR0 ("SSC ERROR: 'Network Adapter.Description' failed to parse from XML.");
}

/************************************************************************/
/*			_ssc_Network_Adapter_MAC_EFun			*/
/************************************************************************/
static ST_VOID _ssc_Network_Adapter_MAC_EFun (SX_DEC_CTRL *sxDecCtrl)
{
ST_RET                rtn;
SSC_STACK_CFG        *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;
SSC_NETWORK_ADAPTER  *networkAdapter;
ST_UCHAR              macBuf[SSC_MAX_LEN_MAC*3+1];
ST_INT                len;

  networkAdapter = (SSC_NETWORK_ADAPTER *) list_find_last ((DBL_LNK *) sscStackCfg->networkAdapterList);
  if (!networkAdapter)
    {
    SSC_LOG_ERR0 ("SSC ERROR: 'Network Adapter.MAC' processing failed (networkAdapter=NULL).");
    sxDecCtrl->errCode = SX_USER_ERROR;
    return;
    }

  len = sizeof (macBuf) - 1; /* -1 for string terminator	*/
  rtn = sx_get_string (sxDecCtrl, (ST_CHAR *) macBuf, &len);
  if (rtn == SD_SUCCESS)
    {
    if (len > 0)
      {
      ST_UINT   macHexLen = 0;
      ST_UCHAR *tmpPtr = macBuf;
      while (tmpPtr)	/* replace ':' with ' ' (space) in the macStr */
        {
        if (*tmpPtr == ':')
          *tmpPtr = ' ';
        ++tmpPtr;
        if (tmpPtr == macBuf + len)
          break;
        }
      rtn = ascii_to_hex_str (networkAdapter->mac, &macHexLen, SSC_MAX_LEN_MAC, (ST_CHAR *) macBuf);
      if (rtn == SD_SUCCESS)
        {
        if (macHexLen == SSC_MAX_LEN_MAC)
          SSC_LOG_CFLOW1 ("SSC PARSE:      'MAC'         ='%s'", macBuf);
        else
          {
          SSC_LOG_ERR1 ("SSC ERROR: 'Network_Adapter.MAC' too short, expecting %d hex values.", SSC_MAX_LEN_MAC);
          sxDecCtrl->errCode = SX_USER_ERROR;
          }
        }
      else
        {
        SSC_LOG_ERR0 ("SSC ERROR: 'Network_Adapter.MAC' conversion to hex failed.");
        sxDecCtrl->errCode = SX_USER_ERROR;
        }
      }
    else
      {
      SSC_LOG_ERR0 ("SSC ERROR: 'Network_Adapter.MAC' not supplied.");
      sxDecCtrl->errCode = SX_USER_ERROR;
      }
    }
  else
    SSC_LOG_ERR0 ("SSC ERROR: 'Network Adapter.MAC' failed to parse from XML.");
}

/************************************************************************/
/*			_ssc_LeanT_Profile_SEFun			*/
/************************************************************************/
static ST_VOID _ssc_LeanT_Profile_SEFun (SX_DEC_CTRL *sxDecCtrl)
{
SSC_STACK_CFG     *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;

  if (sxDecCtrl->reason == SX_ELEMENT_START)
    {
    if (sscAddLeantProfile (sscStackCfg))
      {
      SSC_LOG_CFLOW0 ("SSC PARSE:    'LeanT_Profile' start");
      SX_PUSH (_ssc_LeanT_Profile_Elements);
      }
    else
      {
      SSC_LOG_ERR0 ("SSC ERROR: 'Add LeanT_Profile failed.");
      sxDecCtrl->errCode = SX_USER_ERROR;
      }
    }
  else
    SSC_LOG_CFLOW0 ("SSC PARSE:    'LeanT_Profile' end\n");
}

/************************************************************************/
/*		_ssc_LeanT_Profile_Name_EFun				*/
/************************************************************************/
static ST_VOID _ssc_LeanT_Profile_Name_EFun (SX_DEC_CTRL *sxDecCtrl)
{
ST_RET             rtn;
SSC_STACK_CFG     *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;
SSC_LEANT_PROFILE *leantProfile;
ST_CHAR           *leantProfileName;
ST_INT             len;

  leantProfile = (SSC_LEANT_PROFILE *) list_find_last ((DBL_LNK *) sscStackCfg->leantProfileList);
  if (!leantProfile)
    {
    SSC_LOG_ERR0 ("SSC ERROR: 'LeanT_Profile.Name' processing failed (leantProfile=NULL).");
    sxDecCtrl->errCode = SX_USER_ERROR;
    return;
    }

  len = sizeof (leantProfile->name) - 1; /* -1 for string terminator	*/
  rtn = sx_get_string_ptr (sxDecCtrl, &leantProfileName, &len);
  if (rtn == SD_SUCCESS)
    {
    if (len > 0)
      {
      /* check for duplicates */
      if (!sscFindLeantProfile (sscStackCfg, leantProfileName))
        {
	strcpy (leantProfile->name, leantProfileName);
        SSC_LOG_CFLOW1 ("SSC PARSE:      'Name'                       ='%s'", leantProfileName);
	}
      else
        {
        SSC_LOG_ERR1 ("SSC ERROR: duplicate 'LeanT_Profile.Name'='%s' found.", leantProfileName);
        sxDecCtrl->errCode = SX_USER_ERROR;
	}
      }
    else
      {
      SSC_LOG_ERR0 ("SSC ERROR: 'LeanT_Profile.Name' not supplied.");
      sxDecCtrl->errCode = SX_USER_ERROR;
      }
    }
  else
    SSC_LOG_ERR0 ("SSC ERROR: 'LeanT_Profile.Name' failed to parse from XML.");
}

/************************************************************************/
/*		_ssc_LeanT_Profile_Session_Disconnect_Timeout_EFun	*/
/************************************************************************/
static ST_VOID _ssc_LeanT_Profile_Session_Disconnect_Timeout_EFun (SX_DEC_CTRL *sxDecCtrl)
{
ST_RET             rtn;
SSC_STACK_CFG     *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;
SSC_LEANT_PROFILE *leantProfile;

  leantProfile = (SSC_LEANT_PROFILE *) list_find_last ((DBL_LNK *) sscStackCfg->leantProfileList);
  if (!leantProfile)
    {
    SSC_LOG_ERR0 ("SSC ERROR: 'LeanT_Profile.Session_Disconnect_Timeout' processing failed (leantProfile=NULL).");
    sxDecCtrl->errCode = SX_USER_ERROR;
    return;
    }

  rtn = sx_get_uint16 (sxDecCtrl, &leantProfile->sessionDisconnectTimeout);
  if (rtn == SD_SUCCESS)
    SSC_LOG_CFLOW1 ("SSC PARSE:      'Session_Disconnect_Timeout' = %u", (ST_UINT) leantProfile->sessionDisconnectTimeout);
  else
    SSC_LOG_ERR0 ("SSC ERROR: 'LeanT_Profile.Session_Disconnect_Timeout' failed to parse from XML.");
}

/************************************************************************/
/*		_ssc_LeanT_Profile_Tcp_Enable_EFun			*/
/************************************************************************/
static ST_VOID _ssc_LeanT_Profile_Tcp_Enable_EFun (SX_DEC_CTRL *sxDecCtrl)
{
ST_RET             rtn;
SSC_STACK_CFG     *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;
SSC_LEANT_PROFILE *leantProfile;

  leantProfile = (SSC_LEANT_PROFILE *) list_find_last ((DBL_LNK *) sscStackCfg->leantProfileList);
  if (!leantProfile)
    {
    SSC_LOG_ERR0 ("SSC ERROR: 'LeanT_Profile.Tcp_Enable' processing failed (leantProfile=NULL).");
    sxDecCtrl->errCode = SX_USER_ERROR;
    return;
    }

  rtn = sx_get_string_YesNo (sxDecCtrl, &leantProfile->tcpEnable, SX_USER_ERROR);
  if (rtn == SD_SUCCESS)
    SSC_LOG_CFLOW1 ("SSC PARSE:      'Tcp_Enable'                 = %s", leantProfile->tcpEnable ? "Yes" : "No");
  else
    SSC_LOG_ERR0 ("SSC ERROR: 'LeanT_Profile.Tcp_Enable' failed to parse from XML.");
}

/************************************************************************/
/*		_ssc_LeanT_Profile_Tcp_Max_Tpdu_Len_EFun		*/
/************************************************************************/
static ST_VOID _ssc_LeanT_Profile_Tcp_Max_Tpdu_Len_EFun (SX_DEC_CTRL *sxDecCtrl)
{
ST_RET             rtn;
SSC_STACK_CFG     *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;
SSC_LEANT_PROFILE *leantProfile;

  leantProfile = (SSC_LEANT_PROFILE *) list_find_last ((DBL_LNK *) sscStackCfg->leantProfileList);
  if (!leantProfile)
    {
    SSC_LOG_ERR0 ("SSC ERROR: 'LeanT_Profile.Tcp_Max_Tpdu_Len' processing failed (leantProfile=NULL).");
    sxDecCtrl->errCode = SX_USER_ERROR;
    return;
    }

  rtn = sx_get_uint16 (sxDecCtrl, &leantProfile->tcpMaxTpduLen);
  if (rtn == SD_SUCCESS)
    SSC_LOG_CFLOW1 ("SSC PARSE:      'Tcp_Max_Tpdu_Len'           = %u", (ST_UINT) leantProfile->tcpMaxTpduLen);
  else
    SSC_LOG_ERR0 ("SSC ERROR: 'LeanT_Profile.Tcp_Max_Tpdu_Len' failed to parse from XML.");
}

/************************************************************************/
/*		_ssc_LeanT_Profile_Tcp_Max_Spdu_Outstanding_EFun	*/
/************************************************************************/
static ST_VOID _ssc_LeanT_Profile_Tcp_Max_Spdu_Outstanding_EFun (SX_DEC_CTRL *sxDecCtrl)
{
ST_RET             rtn;
SSC_STACK_CFG     *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;
SSC_LEANT_PROFILE *leantProfile;

  leantProfile = (SSC_LEANT_PROFILE *) list_find_last ((DBL_LNK *) sscStackCfg->leantProfileList);
  if (!leantProfile)
    {
    SSC_LOG_ERR0 ("SSC ERROR: 'LeanT_Profile.Tcp_Max_Spdu_Outstanding' processing failed (leantProfile=NULL).");
    sxDecCtrl->errCode = SX_USER_ERROR;
    return;
    }

  rtn = sx_get_uint (sxDecCtrl, &leantProfile->tcpMaxSpduOutstanding);
  if (rtn == SD_SUCCESS)
    SSC_LOG_CFLOW1 ("SSC PARSE:      'Tcp_Max_Spdu_Outstanding'   = %u", leantProfile->tcpMaxSpduOutstanding);
  else
    SSC_LOG_ERR0 ("SSC ERROR: 'LeanT_Profile.Tcp_Max_Spdu_Outstanding' failed to parse from XML.");
}

/************************************************************************/
/*		_ssc_LeanT_Profile_Tcp_Listen_Port_EFun			*/
/************************************************************************/
static ST_VOID _ssc_LeanT_Profile_Tcp_Listen_Port_EFun (SX_DEC_CTRL *sxDecCtrl)
{
ST_RET             rtn;
SSC_STACK_CFG     *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;
SSC_LEANT_PROFILE *leantProfile;

  leantProfile = (SSC_LEANT_PROFILE *) list_find_last ((DBL_LNK *) sscStackCfg->leantProfileList);
  if (!leantProfile)
    {
    SSC_LOG_ERR0 ("SSC ERROR: 'LeanT_Profile.Tcp_Listen_Port' processing failed (leantProfile=NULL).");
    sxDecCtrl->errCode = SX_USER_ERROR;
    return;
    }

  rtn = sx_get_uint16 (sxDecCtrl, &leantProfile->tcpListenPort);
  if (rtn == SD_SUCCESS)
    SSC_LOG_CFLOW1 ("SSC PARSE:      'Tcp_Listen_Port'            = %u", (ST_UINT) leantProfile->tcpListenPort);
  else
    SSC_LOG_ERR0 ("SSC ERROR: 'LeanT_Profile.Tcp_Listen_Port' failed to parse from XML.");
}

/************************************************************************/
/*		_ssc_LeanT_Profile_Tcp_Max_Idle_Time_EFun		*/
/************************************************************************/
static ST_VOID _ssc_LeanT_Profile_Tcp_Max_Idle_Time_EFun (SX_DEC_CTRL *sxDecCtrl)
{
ST_RET             rtn;
SSC_STACK_CFG     *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;
SSC_LEANT_PROFILE *leantProfile;

  leantProfile = (SSC_LEANT_PROFILE *) list_find_last ((DBL_LNK *) sscStackCfg->leantProfileList);
  if (!leantProfile)
    {
    SSC_LOG_ERR0 ("SSC ERROR: 'LeanT_Profile.Tcp_Max_Idle_Time' processing failed (leantProfile=NULL).");
    sxDecCtrl->errCode = SX_USER_ERROR;
    return;
    }

  rtn = sx_get_uint (sxDecCtrl, &leantProfile->tcpMaxIdleTime);
  if (rtn == SD_SUCCESS)
    SSC_LOG_CFLOW1 ("SSC PARSE:      'Tcp_Max_Idle_Time'          = %u", (ST_UINT) leantProfile->tcpMaxIdleTime);
  else
    SSC_LOG_ERR0 ("SSC ERROR: 'LeanT_Profile.Tcp_Max_Idle_Time' failed to parse from XML.");
}

/************************************************************************/
/*		_ssc_LeanT_Profile_Tcp_Event_Wakeup_Port_Base_EFun	*/
/************************************************************************/
static ST_VOID _ssc_LeanT_Profile_Tcp_Event_Wakeup_Port_Base_EFun (SX_DEC_CTRL *sxDecCtrl)
{
ST_RET             rtn;
SSC_STACK_CFG     *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;
SSC_LEANT_PROFILE *leantProfile;

  leantProfile = (SSC_LEANT_PROFILE *) list_find_last ((DBL_LNK *) sscStackCfg->leantProfileList);
  if (!leantProfile)
    {
    SSC_LOG_ERR0 ("SSC ERROR: 'LeanT_Profile.Tcp_Event_Wakeup_Port_Base' processing failed (leantProfile=NULL).");
    sxDecCtrl->errCode = SX_USER_ERROR;
    return;
    }

  rtn = sx_get_uint16 (sxDecCtrl, &leantProfile->tcpEventWakeupPortBase);
  if (rtn == SD_SUCCESS)
    SSC_LOG_CFLOW1 ("SSC PARSE:      'Tcp_Event_Wakeup_Port_Base' = %u", (ST_UINT) leantProfile->tcpEventWakeupPortBase);
  else
    SSC_LOG_ERR0 ("SSC ERROR: 'LeanT_Profile.Tcp_Event_Wakeup_Port_Base' failed to parse from XML.");
}

/************************************************************************/
/*		_ssc_LeanT_Profile_Tcp_Event_Wakeup_Port_Range_EFun	*/
/************************************************************************/
static ST_VOID _ssc_LeanT_Profile_Tcp_Event_Wakeup_Port_Range_EFun (SX_DEC_CTRL *sxDecCtrl)
{
ST_RET             rtn;
SSC_STACK_CFG     *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;
SSC_LEANT_PROFILE *leantProfile;

  leantProfile = (SSC_LEANT_PROFILE *) list_find_last ((DBL_LNK *) sscStackCfg->leantProfileList);
  if (!leantProfile)
    {
    SSC_LOG_ERR0 ("SSC ERROR: 'LeanT_Profile.Tcp_Event_Wakeup_Port_Range' processing failed (leantProfile=NULL).");
    sxDecCtrl->errCode = SX_USER_ERROR;
    return;
    }

  rtn = sx_get_uint (sxDecCtrl, &leantProfile->tcpEventWakeupPortRange);
  if (rtn == SD_SUCCESS)
    SSC_LOG_CFLOW1 ("SSC PARSE:      'Tcp_Event_Wakeup_Port_Range'= %u", (ST_UINT) leantProfile->tcpEventWakeupPortRange);
  else
    SSC_LOG_ERR0 ("SSC ERROR: 'LeanT_Profile.Tcp_Event_Wakeup_Port_Range' failed to parse from XML.");
}

/************************************************************************/
/*		_ssc_LeanT_Profile_Gensock_Wakeup_Port_Base_EFun	*/
/************************************************************************/
static ST_VOID _ssc_LeanT_Profile_Gensock_Wakeup_Port_Base_EFun (SX_DEC_CTRL *sxDecCtrl)
{
ST_RET             rtn;
SSC_STACK_CFG     *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;
SSC_LEANT_PROFILE *leantProfile;

  leantProfile = (SSC_LEANT_PROFILE *) list_find_last ((DBL_LNK *) sscStackCfg->leantProfileList);
  if (!leantProfile)
    {
    SSC_LOG_ERR0 ("SSC ERROR: 'LeanT_Profile.Gensock_Wakeup_Port_Base' processing failed (leantProfile=NULL).");
    sxDecCtrl->errCode = SX_USER_ERROR;
    return;
    }

  rtn = sx_get_uint16 (sxDecCtrl, &leantProfile->gsWakeupPortBase);
  if (rtn == SD_SUCCESS)
    SSC_LOG_CFLOW1 ("SSC PARSE:      'Gensock_Wakeup_Port_Base'   = %u", (ST_UINT) leantProfile->gsWakeupPortBase);
  else
    SSC_LOG_ERR0 ("SSC ERROR: 'LeanT_Profile.Gensock_Wakeup_Port_Base' failed to parse from XML.");
}

/************************************************************************/
/*		_ssc_LeanT_Profile_Gensock_Wakeup_Port_Range_EFun	*/
/************************************************************************/
static ST_VOID _ssc_LeanT_Profile_Gensock_Wakeup_Port_Range_EFun (SX_DEC_CTRL *sxDecCtrl)
{
ST_RET             rtn;
SSC_STACK_CFG     *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;
SSC_LEANT_PROFILE *leantProfile;

  leantProfile = (SSC_LEANT_PROFILE *) list_find_last ((DBL_LNK *) sscStackCfg->leantProfileList);
  if (!leantProfile)
    {
    SSC_LOG_ERR0 ("SSC ERROR: 'LeanT_Profile.Gensock_Wakeup_Port_Range' processing failed (leantProfile=NULL).");
    sxDecCtrl->errCode = SX_USER_ERROR;
    return;
    }

  rtn = sx_get_uint (sxDecCtrl, &leantProfile->gsWakeupPortRange);
  if (rtn == SD_SUCCESS)
    SSC_LOG_CFLOW1 ("SSC PARSE:      'Gensock_Wakeup_Port_Range'  = %u", (ST_UINT) leantProfile->gsWakeupPortRange);
  else
    SSC_LOG_ERR0 ("SSC ERROR: 'LeanT_Profile.Gensock_Wakeup_Port_Range' failed to parse from XML.");
}

/************************************************************************/
/*		_ssc_LeanT_Profile_Tp4_Enable_EFun			*/
/************************************************************************/
static ST_VOID _ssc_LeanT_Profile_Tp4_Enable_EFun (SX_DEC_CTRL *sxDecCtrl)
{
ST_RET             rtn;
SSC_STACK_CFG     *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;
SSC_LEANT_PROFILE *leantProfile;

  leantProfile = (SSC_LEANT_PROFILE *) list_find_last ((DBL_LNK *) sscStackCfg->leantProfileList);
  if (!leantProfile)
    {
    SSC_LOG_ERR0 ("SSC ERROR: 'LeanT_Profile.Tp4_Enable' processing failed (leantProfile=NULL).");
    sxDecCtrl->errCode = SX_USER_ERROR;
    return;
    }

  rtn = sx_get_string_YesNo (sxDecCtrl, &leantProfile->tp4Enable, SX_USER_ERROR);
  if (rtn == SD_SUCCESS)
    SSC_LOG_CFLOW1 ("SSC PARSE:      'Tp4_Enable'                 = %s", leantProfile->tp4Enable ? "Yes" : "No");
  else
    SSC_LOG_ERR0 ("SSC ERROR: 'LeanT_Profile.Tp4_Enable' failed to parse from XML.");
}

/************************************************************************/
/*		_ssc_LeanT_Profile_Tp4_Max_Tpdu_Len_EFun		*/
/************************************************************************/
static ST_VOID _ssc_LeanT_Profile_Tp4_Max_Tpdu_Len_EFun (SX_DEC_CTRL *sxDecCtrl)
{
ST_RET             rtn;
SSC_STACK_CFG     *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;
SSC_LEANT_PROFILE *leantProfile;

  leantProfile = (SSC_LEANT_PROFILE *) list_find_last ((DBL_LNK *) sscStackCfg->leantProfileList);
  if (!leantProfile)
    {
    SSC_LOG_ERR0 ("SSC ERROR: 'LeanT_Profile.Tp4_Max_Tpdu_Len' processing failed (leantProfile=NULL).");
    sxDecCtrl->errCode = SX_USER_ERROR;
    return;
    }

  rtn = sx_get_uint16 (sxDecCtrl, &leantProfile->tp4MaxTpduLen);
  if (rtn == SD_SUCCESS)
    SSC_LOG_CFLOW1 ("SSC PARSE:      'Tp4_Max_Tpdu_Len'           = %u", (ST_UINT) leantProfile->tp4MaxTpduLen);
  else
    SSC_LOG_ERR0 ("SSC ERROR: 'LeanT_Profile.Tp4_Max_Tpdu_Len' failed to parse from XML.");
}

/************************************************************************/
/*		_ssc_LeanT_Profile_Tp4_Max_Spdu_Outstanding_EFun	*/
/************************************************************************/
static ST_VOID _ssc_LeanT_Profile_Tp4_Max_Spdu_Outstanding_EFun (SX_DEC_CTRL *sxDecCtrl)
{
ST_RET             rtn;
SSC_STACK_CFG     *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;
SSC_LEANT_PROFILE *leantProfile;

  leantProfile = (SSC_LEANT_PROFILE *) list_find_last ((DBL_LNK *) sscStackCfg->leantProfileList);
  if (!leantProfile)
    {
    SSC_LOG_ERR0 ("SSC ERROR: 'LeanT_Profile.Tp4_Max_Spdu_Outstanding' processing failed (leantProfile=NULL).");
    sxDecCtrl->errCode = SX_USER_ERROR;
    return;
    }

  rtn = sx_get_uint (sxDecCtrl, &leantProfile->tp4MaxSpduOutstanding);
  if (rtn == SD_SUCCESS)
    SSC_LOG_CFLOW1 ("SSC PARSE:      'Tp4_Max_Spdu_Outstanding'   = %u", leantProfile->tp4MaxSpduOutstanding);
  else
    SSC_LOG_ERR0 ("SSC ERROR: 'LeanT_Profile.Tp4_Max_Spdu_Outstanding' failed to parse from XML.");
}

/************************************************************************/
/*			_ssc_LeanT_Profile_Tp4_Local_Credits_EFun	*/
/************************************************************************/
static ST_VOID _ssc_LeanT_Profile_Tp4_Local_Credits_EFun (SX_DEC_CTRL *sxDecCtrl)
{
ST_RET             rtn;
SSC_STACK_CFG     *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;
SSC_LEANT_PROFILE *leantProfile;
ST_UINT16          tmpU16;

  leantProfile = (SSC_LEANT_PROFILE *) list_find_last ((DBL_LNK *) sscStackCfg->leantProfileList);
  if (!leantProfile)
    {
    SSC_LOG_ERR0 ("SSC ERROR: 'LeanT_Profile.Tp4_Local_Credits' processing failed (leantProfile=NULL).");
    sxDecCtrl->errCode = SX_USER_ERROR;
    return;
    }

  rtn = sx_get_uint16 (sxDecCtrl, &tmpU16);
  if (rtn == SD_SUCCESS)
    {
    leantProfile->tp4LocalCredits = (ST_UINT8) tmpU16;
    SSC_LOG_CFLOW1 ("SSC PARSE:      'Tp4_Local_Credits'          = %u", (ST_UINT) leantProfile->tp4LocalCredits);
    }
  else
    SSC_LOG_ERR0 ("SSC ERROR: 'LeanT_Profile.Tp4_Local_Credits' failed to parse from XML.");
}

/************************************************************************/
/*			_ssc_LeanT_Profile_Tp4_Max_Remote_Credits_EFun	*/
/************************************************************************/
static ST_VOID _ssc_LeanT_Profile_Tp4_Max_Remote_Credits_EFun	(SX_DEC_CTRL *sxDecCtrl)
{
ST_RET             rtn;
SSC_STACK_CFG     *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;
SSC_LEANT_PROFILE *leantProfile;
ST_UINT16          tmpU16;

  leantProfile = (SSC_LEANT_PROFILE *) list_find_last ((DBL_LNK *) sscStackCfg->leantProfileList);
  if (!leantProfile)
    {
    SSC_LOG_ERR0 ("SSC ERROR: 'LeanT_Profile.Tp4_Max_Remote_Credits' processing failed (leantProfile=NULL).");
    sxDecCtrl->errCode = SX_USER_ERROR;
    return;
    }

  rtn = sx_get_uint16 (sxDecCtrl, &tmpU16);
  if (rtn == SD_SUCCESS)
    {
    leantProfile->tp4MaxRemoteCredits = (ST_UINT8) tmpU16;
    SSC_LOG_CFLOW1 ("SSC PARSE:      'Tp4_Max_Remote_Credits'     = %u", (ST_UINT) leantProfile->tp4MaxRemoteCredits);
    }
  else
    SSC_LOG_ERR0 ("SSC ERROR: 'LeanT_Profile.Tp4_Max_Remote_Credits' failed to parse from XML.");
}

/************************************************************************/
/*		_ssc_LeanT_Profile_Tp4_Window_Time_EFun			*/
/************************************************************************/
static ST_VOID _ssc_LeanT_Profile_Tp4_Window_Time_EFun (SX_DEC_CTRL *sxDecCtrl)
{
ST_RET             rtn;
SSC_STACK_CFG     *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;
SSC_LEANT_PROFILE *leantProfile;

  leantProfile = (SSC_LEANT_PROFILE *) list_find_last ((DBL_LNK *) sscStackCfg->leantProfileList);
  if (!leantProfile)
    {
    SSC_LOG_ERR0 ("SSC ERROR: 'LeanT_Profile.Tp4_Window_Time' processing failed (leantProfile=NULL).");
    sxDecCtrl->errCode = SX_USER_ERROR;
    return;
    }

  rtn = sx_get_uint16 (sxDecCtrl, &leantProfile->tp4WindowTime);
  if (rtn == SD_SUCCESS)
    SSC_LOG_CFLOW1 ("SSC PARSE:      'Tp4_Window_Time'            = %u", (ST_UINT) leantProfile->tp4WindowTime);
  else
    SSC_LOG_ERR0 ("SSC ERROR: 'LeanT_Profile.Tp4_Window_Time' failed to parse from XML.");
}

/************************************************************************/
/*		_ssc_LeanT_Profile_Tp4_Inactivity_Time_EFun		*/
/************************************************************************/
static ST_VOID _ssc_LeanT_Profile_Tp4_Inactivity_Time_EFun (SX_DEC_CTRL *sxDecCtrl)
{
ST_RET             rtn;
SSC_STACK_CFG     *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;
SSC_LEANT_PROFILE *leantProfile;

  leantProfile = (SSC_LEANT_PROFILE *) list_find_last ((DBL_LNK *) sscStackCfg->leantProfileList);
  if (!leantProfile)
    {
    SSC_LOG_ERR0 ("SSC ERROR: 'LeanT_Profile.Tp4_Inactivity_Time' processing failed (leantProfile=NULL).");
    sxDecCtrl->errCode = SX_USER_ERROR;
    return;
    }

  rtn = sx_get_uint16 (sxDecCtrl, &leantProfile->tp4InactivityTime);
  if (rtn == SD_SUCCESS)
    SSC_LOG_CFLOW1 ("SSC PARSE:      'Tp4_Inactivity_Time'        = %u", (ST_UINT) leantProfile->tp4InactivityTime);
  else
    SSC_LOG_ERR0 ("SSC ERROR: 'LeanT_Profile.Tp4_Inactivity_Time' failed to parse from XML.");
}

/************************************************************************/
/*		_ssc_LeanT_Profile_Tp4_Retransmission_Time_EFun		*/
/************************************************************************/
static ST_VOID _ssc_LeanT_Profile_Tp4_Retransmission_Time_EFun (SX_DEC_CTRL *sxDecCtrl)
{
ST_RET             rtn;
SSC_STACK_CFG     *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;
SSC_LEANT_PROFILE *leantProfile;

  leantProfile = (SSC_LEANT_PROFILE *) list_find_last ((DBL_LNK *) sscStackCfg->leantProfileList);
  if (!leantProfile)
    {
    SSC_LOG_ERR0 ("SSC ERROR: 'LeanT_Profile.Tp4_Retransmission_Time' processing failed (leantProfile=NULL).");
    sxDecCtrl->errCode = SX_USER_ERROR;
    return;
    }

  rtn = sx_get_uint16 (sxDecCtrl, &leantProfile->tp4RetransmissionTime);
  if (rtn == SD_SUCCESS)
    SSC_LOG_CFLOW1 ("SSC PARSE:      'Tp4_Retransmission_Time'    = %u", (ST_UINT) leantProfile->tp4RetransmissionTime);
  else
    SSC_LOG_ERR0 ("SSC ERROR: 'LeanT_Profile.Tp4_Retransmission_Time' failed to parse from XML.");
}

/************************************************************************/
/*		_ssc_LeanT_Profile_Tp4_Max_Transmissions_EFun		*/
/************************************************************************/
static ST_VOID _ssc_LeanT_Profile_Tp4_Max_Transmissions_EFun (SX_DEC_CTRL *sxDecCtrl)
{
ST_RET             rtn;
SSC_STACK_CFG     *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;
SSC_LEANT_PROFILE *leantProfile;
ST_UINT16          tmpU16;

  leantProfile = (SSC_LEANT_PROFILE *) list_find_last ((DBL_LNK *) sscStackCfg->leantProfileList);
  if (!leantProfile)
    {
    SSC_LOG_ERR0 ("SSC ERROR: 'LeanT_Profile.Tp4_Max_Transmissions' processing failed (leantProfile=NULL).");
    sxDecCtrl->errCode = SX_USER_ERROR;
    return;
    }

  rtn = sx_get_uint16 (sxDecCtrl, &tmpU16);
  if (rtn == SD_SUCCESS)
    {
    leantProfile->tp4MaxTransmissions = (ST_UINT8) tmpU16;
    SSC_LOG_CFLOW1 ("SSC PARSE:      'Tp4_Max_Transmissions'      = %u", (ST_UINT) leantProfile->tp4MaxTransmissions);
    }
  else
    SSC_LOG_ERR0 ("SSC ERROR: 'LeanT_Profile.Tp4_Max_Transmissions' failed to parse from XML.");
}

/************************************************************************/
/*		_ssc_LeanT_Profile_Tp4_Ack_Delay_EFun			*/
/************************************************************************/
static ST_VOID _ssc_LeanT_Profile_Tp4_Ack_Delay_EFun (SX_DEC_CTRL *sxDecCtrl)
{
ST_RET             rtn;
SSC_STACK_CFG     *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;
SSC_LEANT_PROFILE *leantProfile;
ST_UINT16          tmpU16;

  leantProfile = (SSC_LEANT_PROFILE *) list_find_last ((DBL_LNK *) sscStackCfg->leantProfileList);
  if (!leantProfile)
    {
    SSC_LOG_ERR0 ("SSC ERROR: 'LeanT_Profile.Tp4_Ack_Delay' processing failed (leantProfile=NULL).");
    sxDecCtrl->errCode = SX_USER_ERROR;
    return;
    }

  rtn = sx_get_uint16 (sxDecCtrl, &tmpU16);
  if (rtn == SD_SUCCESS)
    {
    leantProfile->tp4AckDelay = (ST_UINT8) tmpU16;
    SSC_LOG_CFLOW1 ("SSC PARSE:      'Tp4_Ack_Delay'              = %u", (ST_UINT) leantProfile->tp4AckDelay);
    }
  else
    SSC_LOG_ERR0 ("SSC ERROR: 'LeanT_Profile.Tp4_Ack_Delay' failed to parse from XML.");
}

/************************************************************************/
/*		_ssc_LeanT_Profile_Clnp_Lifetime_EFun			*/
/************************************************************************/
static ST_VOID _ssc_LeanT_Profile_Clnp_Lifetime_EFun (SX_DEC_CTRL *sxDecCtrl)
{
ST_RET             rtn;
SSC_STACK_CFG     *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;
SSC_LEANT_PROFILE *leantProfile;
ST_UINT16          tmpU16;

  leantProfile = (SSC_LEANT_PROFILE *) list_find_last ((DBL_LNK *) sscStackCfg->leantProfileList);
  if (!leantProfile)
    {
    SSC_LOG_ERR0 ("SSC ERROR: 'LeanT_Profile.Clnp_Lifetime' processing failed (leantProfile=NULL).");
    sxDecCtrl->errCode = SX_USER_ERROR;
    return;
    }

  rtn = sx_get_uint16 (sxDecCtrl, &tmpU16);
  if (rtn == SD_SUCCESS)
    {
    leantProfile->clnpLifetime = (ST_UINT8) tmpU16;
    SSC_LOG_CFLOW1 ("SSC PARSE:      'Clnp_Lifetime'              = %u", (ST_UINT) leantProfile->clnpLifetime);
    }
  else
    SSC_LOG_ERR0 ("SSC ERROR: 'LeanT_Profile.Clnp_Lifetime' failed to parse from XML.");
}

/************************************************************************/
/*		_ssc_LeanT_Profile_Clnp_Lifetime_Decrement_EFun		*/
/************************************************************************/
static ST_VOID _ssc_LeanT_Profile_Clnp_Lifetime_Decrement_EFun (SX_DEC_CTRL *sxDecCtrl)
{
ST_RET             rtn;
SSC_STACK_CFG     *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;
SSC_LEANT_PROFILE *leantProfile;
ST_UINT16          tmpU16;

  leantProfile = (SSC_LEANT_PROFILE *) list_find_last ((DBL_LNK *) sscStackCfg->leantProfileList);
  if (!leantProfile)
    {
    SSC_LOG_ERR0 ("SSC ERROR: 'LeanT_Profile.Clnp_Lifetime_Decrement' processing failed (leantProfile=NULL).");
    sxDecCtrl->errCode = SX_USER_ERROR;
    return;
    }

  rtn = sx_get_uint16 (sxDecCtrl, &tmpU16);
  if (rtn == SD_SUCCESS)
    {
    leantProfile->clnpLifetimeDecrement = (ST_UINT8) tmpU16;
    SSC_LOG_CFLOW1 ("SSC PARSE:      'Clnp_Lifetime_Decrement'    = %u", (ST_UINT) leantProfile->clnpLifetimeDecrement);
    }
  else
    SSC_LOG_ERR0 ("SSC ERROR: 'LeanT_Profile.Clnp_Lifetime_Decrement' failed to parse from XML.");
}

/************************************************************************/
/*		_ssc_LeanT_Profile_Clnp_Cfg_Timer_EFun			*/
/************************************************************************/
static ST_VOID _ssc_LeanT_Profile_Clnp_Cfg_Timer_EFun	(SX_DEC_CTRL *sxDecCtrl)
{
ST_RET             rtn;
SSC_STACK_CFG     *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;
SSC_LEANT_PROFILE *leantProfile;

  leantProfile = (SSC_LEANT_PROFILE *) list_find_last ((DBL_LNK *) sscStackCfg->leantProfileList);
  if (!leantProfile)
    {
    SSC_LOG_ERR0 ("SSC ERROR: 'LeanT_Profile.Clnp_Cfg_Timer' processing failed (leantProfile=NULL).");
    sxDecCtrl->errCode = SX_USER_ERROR;
    return;
    }

  rtn = sx_get_uint16 (sxDecCtrl, &leantProfile->clnpCfgTimer);
  if (rtn == SD_SUCCESS)
    SSC_LOG_CFLOW1 ("SSC PARSE:      'Clnp_Cfg_Timer'             = %u", (ST_UINT) leantProfile->clnpCfgTimer);
  else
    SSC_LOG_ERR0 ("SSC ERROR: 'LeanT_Profile.Clnp_Cfg_Timer' failed to parse from XML.");
}

/************************************************************************/
/*		_ssc_LeanT_Profile_Clnp_Esh_Delay_EFun			*/
/************************************************************************/
static ST_VOID _ssc_LeanT_Profile_Clnp_Esh_Delay_EFun	(SX_DEC_CTRL *sxDecCtrl)
{
ST_RET             rtn;
SSC_STACK_CFG     *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;
SSC_LEANT_PROFILE *leantProfile;

  leantProfile = (SSC_LEANT_PROFILE *) list_find_last ((DBL_LNK *) sscStackCfg->leantProfileList);
  if (!leantProfile)
    {
    SSC_LOG_ERR0 ("SSC ERROR: 'LeanT_Profile.Clnp_Esh_Delay' processing failed (leantProfile=NULL).");
    sxDecCtrl->errCode = SX_USER_ERROR;
    return;
    }

  rtn = sx_get_uint16 (sxDecCtrl, &leantProfile->clnpEshDelay);
  if (rtn == SD_SUCCESS)
    SSC_LOG_CFLOW1 ("SSC PARSE:      'Clnp_Esh_Delay'             = %u",  (ST_UINT) leantProfile->clnpEshDelay);
  else
    SSC_LOG_ERR0 ("SSC ERROR: 'LeanT_Profile.Clnp_Esh_Delay' failed to parse from XML.");
}

/************************************************************************/
/*		_ssc_LeanT_Profile_Adapter_Name_EFun			*/
/************************************************************************/
static ST_VOID _ssc_LeanT_Profile_Adapter_Name_EFun (SX_DEC_CTRL *sxDecCtrl)
{
ST_RET             rtn;
SSC_STACK_CFG     *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;
SSC_LEANT_PROFILE *leantProfile;
ST_INT             len;

  leantProfile = (SSC_LEANT_PROFILE *) list_find_last ((DBL_LNK *) sscStackCfg->leantProfileList);
  if (!leantProfile)
    {
    SSC_LOG_ERR0 ("SSC ERROR: 'LeanT_Profile.Adapter_Name' processing failed (leantProfile=NULL).");
    sxDecCtrl->errCode = SX_USER_ERROR;
    return;
    }

  len = sizeof (leantProfile->adapterName) - 1; /* -1 for string terminator	*/
  rtn = sx_get_string (sxDecCtrl, leantProfile->adapterName, &len);
  if (rtn == SD_SUCCESS)
    {
    if (len > 0)
      SSC_LOG_CFLOW1 ("SSC PARSE:      'Adapter_Name'               ='%s'", leantProfile->adapterName);
    else
      {
      SSC_LOG_ERR0 ("SSC ERROR: 'LeanT_Profile.Adapter_Name' not supplied.");
      sxDecCtrl->errCode = SX_USER_ERROR;
      }
    }
  else
    SSC_LOG_ERR0 ("SSC ERROR: 'LeanT_Profile.Adapter_Name' failed to parse from XML.");
}

/************************************************************************/
/*		_ssc_App_SNAP_Max_Users_EFun				*/
/************************************************************************/
static ST_VOID _ssc_App_SNAP_Max_Users_EFun (SX_DEC_CTRL *sxDecCtrl)
{
ST_RET             rtn;
SSC_STACK_CFG     *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;
SSC_SNAP_PARAM    *snapParam;

  snapParam = sscStackCfg->snapParam;
  if (!snapParam)
    {
    SSC_LOG_ERR0 ("SSC ERROR: 'App_SNAP.Max_Users' processing failed (snapParam=NULL).");
    sxDecCtrl->errCode = SX_USER_ERROR;
    return;
    }

  rtn = sx_get_uint (sxDecCtrl, &snapParam->maxUsers);
  if (rtn == SD_SUCCESS)
    SSC_LOG_CFLOW1 ("SSC PARSE:      'Max_Users'                       = %u", snapParam->maxUsers);
  else
    SSC_LOG_ERR0 ("SSC ERROR: 'App_SNAP.Max_Users' failed to parse from XML.");
}

/************************************************************************/
/*		_ssc_App_SNAP_Max_Binds_Per_User_EFun			*/
/************************************************************************/
static ST_VOID _ssc_App_SNAP_Max_Binds_Per_User_EFun (SX_DEC_CTRL *sxDecCtrl)
{
ST_RET             rtn;
SSC_STACK_CFG     *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;
SSC_SNAP_PARAM    *snapParam;

  snapParam = sscStackCfg->snapParam;
  if (!snapParam)
    {
    SSC_LOG_ERR0 ("SSC ERROR: 'App_SNAP.Max_Binds_Per_User' processing failed (snapParam=NULL).");
    sxDecCtrl->errCode = SX_USER_ERROR;
    return;
    }

  rtn = sx_get_uint (sxDecCtrl, &snapParam->maxBindsPerUser);
  if (rtn == SD_SUCCESS)
    SSC_LOG_CFLOW1 ("SSC PARSE:      'Max_Binds_Per_User'              = %u", snapParam->maxBindsPerUser);
  else
    SSC_LOG_ERR0 ("SSC ERROR: 'App_SNAP.Max_Binds_Per_User' failed to parse from XML.");
}

/************************************************************************/
/*		_ssc_App_SNAP_IPC_Port_EFun				*/
/************************************************************************/
static ST_VOID _ssc_App_SNAP_IPC_Port_EFun (SX_DEC_CTRL *sxDecCtrl)
{
#if defined(_WIN32)
ST_RET             rtn;
SSC_STACK_CFG     *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;
SSC_SNAP_PARAM    *snapParam;

  snapParam = sscStackCfg->snapParam;
  if (!snapParam)
    {
    SSC_LOG_ERR0 ("SSC ERROR: 'App_SNAP.IPC_Port' processing failed (snapParam=NULL).");
    sxDecCtrl->errCode = SX_USER_ERROR;
    return;
    }

  rtn = sx_get_uint16 (sxDecCtrl, &snapParam->ipcPort);
  if (rtn == SD_SUCCESS)
    SSC_LOG_CFLOW1 ("SSC PARSE:      'IPC_Port'                        = %u", (ST_UINT) snapParam->ipcPort);
  else
    SSC_LOG_ERR0 ("SSC ERROR: 'App_SNAP.IPC_Port' failed to parse from XML.");
#endif /* defined(_WIN32) */
}

/************************************************************************/
/*		_ssc_App_SNAP_IPC_Domain_Socket_Path_EFun		*/
/************************************************************************/
static ST_VOID _ssc_App_SNAP_IPC_Domain_Socket_Path_EFun (SX_DEC_CTRL *sxDecCtrl)
{
#if !defined(_WIN32)  /* UNIX / Linux */
ST_RET             rtn;
SSC_STACK_CFG     *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;
SSC_SNAP_PARAM    *snapParam;
ST_INT             len;

  snapParam = sscStackCfg->snapParam;
  if (!snapParam)
    {
    SSC_LOG_ERR0 ("SSC ERROR: 'App_SNAP.IPC_Domain_Socket_Path' processing failed (snapParam=NULL).");
    sxDecCtrl->errCode = SX_USER_ERROR;
    return;
    }

  len = sizeof (snapParam->ipcDir) - 1; 	/* -1 for string terminator	*/
  rtn = sx_get_string (sxDecCtrl, snapParam->ipcDir, &len);
  if (rtn == SD_SUCCESS)
    {
    if (len > 0)
      SSC_LOG_CFLOW1 ("SSC PARSE:      'IPC_Domain_Socket_Path'          ='%s'", snapParam->ipcDir);
    else
      {
      SSC_LOG_ERR0 ("SSC ERROR: 'App_SNAP.IPC_Domain_Socket_Path' not supplied.");
      sxDecCtrl->errCode = SX_USER_ERROR;
      }
    }
  else
    SSC_LOG_ERR0 ("SSC ERROR: 'App_SNAP.IPC_Domain_Socket_Path' failed to parse from XML.");
#endif /* !defined(_WIN32) */
}

/************************************************************************/
/*		_ssc_App_SNAP_IPC_Timeout_EFun				*/
/************************************************************************/
static ST_VOID _ssc_App_SNAP_IPC_Timeout_EFun (SX_DEC_CTRL *sxDecCtrl)
{
ST_RET             rtn;
SSC_STACK_CFG     *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;
SSC_SNAP_PARAM    *snapParam;

  snapParam = sscStackCfg->snapParam;
  if (!snapParam)
    {
    SSC_LOG_ERR0 ("SSC ERROR: 'App_SNAP.IPC_Timeout' processing failed (snapParam=NULL).");
    sxDecCtrl->errCode = SX_USER_ERROR;
    return;
    }

  rtn = sx_get_int (sxDecCtrl, &snapParam->ipcTimeout);
  if (rtn == SD_SUCCESS)
    SSC_LOG_CFLOW1 ("SSC PARSE:      'IPC_Timeout'                     = %d", snapParam->ipcTimeout);
  else
    SSC_LOG_ERR0 ("SSC ERROR: 'App_SNAP.IPC_Timeout' failed to parse from XML.");
}

/************************************************************************/
/*		_ssc_App_SNAP_CPU_Affinity_EFun				*/
/************************************************************************/
static ST_VOID _ssc_App_SNAP_CPU_Affinity_EFun	(SX_DEC_CTRL *sxDecCtrl)
{
SSC_STACK_CFG     *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;
SSC_SNAP_PARAM    *snapParam;
ST_INT             strLen = 0;
ST_CHAR            tmpBuf[20];

  snapParam = sscStackCfg->snapParam;
  if (!snapParam)
    {
    SSC_LOG_ERR0 ("SSC ERROR: 'App_SNAP.CPU_Affinity' processing failed (snapParam=NULL).");
    sxDecCtrl->errCode = SX_USER_ERROR;
    return;
    }

  strLen = sizeof(tmpBuf) - 1;
  if (sx_get_string (sxDecCtrl, tmpBuf, &strLen) != SD_SUCCESS)
    {
    SSC_LOG_ERR0 ("SSC ERROR: 'App_SNAP.CPU_Affinity' failed to parse from XML.");
    return;
    }  
  if ((strlen (tmpBuf) < 3) || (strncmp (tmpBuf, "0x", 2) != 0))
    {
    SSC_LOG_ERR0 ("SSC ERROR: invalid CpuAffinity value (hex value expected)");
    sxDecCtrl->errCode = SX_USER_ERROR;
    return;
    }
  
  sscanf (&tmpBuf[2], "%x", &snapParam->cpuAffinity);
  SSC_LOG_CFLOW1 ("SSC PARSE:      'CPU_Affinity'                    = %u", snapParam->cpuAffinity);
}

/************************************************************************/
/*		_ssc_AR_Name_SEFun					*/
/************************************************************************/
static ST_VOID _ssc_AR_Name_SEFun (SX_DEC_CTRL *sxDecCtrl)
{
SSC_STACK_CFG *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;

  if (sxDecCtrl->reason == SX_ELEMENT_START)
    {
    if (sscAddArName (sscStackCfg))
      {
      SSC_LOG_CFLOW0 ("SSC PARSE:    'AR_Name' start");
      SX_PUSH (_ssc_AR_Name_Elements);
      }
    else
      {
      SSC_LOG_ERR0 ("SSC ERROR: 'Add AR_Name failed.");
      sxDecCtrl->errCode = SX_USER_ERROR;
      }
    }
  else
    SSC_LOG_CFLOW0 ("SSC PARSE:    'AR_Name' end\n");
}

/************************************************************************/
/*		_ssc_AR_Name_Name_EFun					*/
/************************************************************************/
static ST_VOID _ssc_AR_Name_Name_EFun (SX_DEC_CTRL *sxDecCtrl)
{
ST_RET             rtn;
SSC_STACK_CFG     *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;
SSC_AR_NAME       *sscArName;
DIB_ENTRY        *arDib;
S_SEC_LOC_REM_AR  *arSec;
ST_CHAR           *str;
ST_INT             len = 0;

  sscArName = (SSC_AR_NAME *) list_find_last ((DBL_LNK *) sscStackCfg->arNameList);
  if (!sscArName)
    {
    SSC_LOG_ERR0 ("SSC ERROR: 'AR_Name.Name' processing failed (sscArName=NULL).");
    sxDecCtrl->errCode = SX_USER_ERROR;
    return;
    }
  arDib = sscArName->arDib;
  arSec = sscArName->arSec;

  rtn = sx_get_string_ptr (sxDecCtrl, &str, &len);
  if (rtn == SD_SUCCESS)
    {
    if (len > 0 && len < SSC_MAX_LEN_AR_NAME)
      {
      /* check for duplicates */
      if (!sscFindArName (sscStackCfg, str))
        {
        SSC_LOG_CFLOW1 ("SSC PARSE:      'Name'                ='%s'", str);
        strcpy (arDib->name, str);
        strcpy (arSec->arName, str);
	}
      else
        {
        SSC_LOG_ERR1 ("SSC ERROR: duplicate 'AR_Name.Name'='%s' found", str);
        sxDecCtrl->errCode = SX_USER_ERROR;
	}
      }
    else
      {
      SSC_LOG_ERR2 ("SSC ERROR: 'AR_Name.Name' %s (max AR Name length is %u).",
                    (len == 0 ? "not supplied" : "too long"), SSC_MAX_LEN_AR_NAME);
      sxDecCtrl->errCode = SX_USER_ERROR;
      }
    }
  else
    SSC_LOG_ERR0 ("SSC ERROR: 'AR_Name.Name' failed to parse from XML.");
}

/************************************************************************/
/*		_ssc_AR_Name_Description_EFun				*/
/************************************************************************/
static ST_VOID _ssc_AR_Name_Description_EFun (SX_DEC_CTRL *sxDecCtrl)
{
ST_RET             rtn;
SSC_STACK_CFG     *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;
SSC_AR_NAME       *sscArName;
DIB_ENTRY        *arDib;
ST_CHAR           *str;
ST_INT             len = 0;

  sscArName = (SSC_AR_NAME *) list_find_last ((DBL_LNK *) sscStackCfg->arNameList);
  if (!sscArName)
    {
    SSC_LOG_ERR0 ("SSC ERROR: 'AR_Name.Name' processing failed (sscArName=NULL).");
    sxDecCtrl->errCode = SX_USER_ERROR;
    return;
    }
  arDib = sscArName->arDib;

  rtn = sx_get_string_ptr (sxDecCtrl, &str, &len);
  if (rtn == SD_SUCCESS)
    {
    if (len > 0)
      SSC_LOG_CFLOW1 ("SSC PARSE:      'Description'         ='%s'", str);
      /* we do not save the description anywhere */
    }
  else
    SSC_LOG_ERR1 ("SSC ERROR: 'AR_Name.Description' failed to parse from XML\n"
                  "  for 'AR_Name.Name'='%s'.", (strlen(arDib->name) ? arDib->name : "?"));
}

/************************************************************************/
/*		_ssc_AR_Name_AP_Title_EFun				*/
/************************************************************************/
static ST_VOID _ssc_AR_Name_AP_Title_EFun (SX_DEC_CTRL *sxDecCtrl)
{
ST_RET             rtn;
SSC_STACK_CFG     *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;
SSC_AR_NAME       *sscArName;
DIB_ENTRY        *arDib;
ST_CHAR            apTitle[SSC_MAX_LEN_AP_TITLE+1];
ST_INT             len;

  sscArName = (SSC_AR_NAME *) list_find_last ((DBL_LNK *) sscStackCfg->arNameList);
  if (!sscArName)
    {
    SSC_LOG_ERR0 ("SSC ERROR: 'AR_Name.AP_Title' processing failed (sscArName=NULL).");
    sxDecCtrl->errCode = SX_USER_ERROR;
    return;
    }
  arDib = sscArName->arDib;

  len = sizeof (apTitle) - 1;	/* -1 for string terminator	*/
  rtn = sx_get_string (sxDecCtrl, apTitle, &len);
  if (rtn == SD_SUCCESS)
    {
    if (len > 0)
      {
      SSC_LOG_CFLOW1 ("SSC PARSE:      'AP_Title'            ='%s'", apTitle);
      /* note that parseObjId puts NULL chars into the string when parsing	*/
      if (sscParseObjId (&arDib->ae_title.AP_title, apTitle) == SD_SUCCESS)
        arDib->ae_title.AP_title_pres = SD_TRUE;    /* defaults to FALSE    */
      else
        sxDecCtrl->errCode = SX_USER_ERROR;
      }
    else
      {
      SSC_LOG_ERR1 ("SSC ERROR: 'AR_Name.AP_Title' not supplied\n"
                    "  for 'AR_Name.Name'='%s'.", (strlen(arDib->name) ? arDib->name : "?"));
      sxDecCtrl->errCode = SX_USER_ERROR;
      }
    }
  else
    SSC_LOG_ERR1 ("SSC ERROR: 'AR_Name.AP_Title' failed to parse from XML\n"
                  "  for 'AR_Name.Name'='%s'.", (strlen(arDib->name) ? arDib->name : "?"));
}

/************************************************************************/
/*		_ssc_AR_Name_AE_Qualifier_EFun				*/
/************************************************************************/
static ST_VOID _ssc_AR_Name_AE_Qualifier_EFun (SX_DEC_CTRL *sxDecCtrl)
{
ST_RET             rtn;
SSC_STACK_CFG     *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;
SSC_AR_NAME       *sscArName;
DIB_ENTRY        *arDib;

  sscArName = (SSC_AR_NAME *) list_find_last ((DBL_LNK *) sscStackCfg->arNameList);
  if (!sscArName)
    {
    SSC_LOG_ERR0 ("SSC ERROR: 'AR_Name.AE_Qualifier' processing failed (sscArName=NULL).");
    sxDecCtrl->errCode = SX_USER_ERROR;
    return;
    }
  arDib = sscArName->arDib;

  rtn = sx_get_int32 (sxDecCtrl, &arDib->ae_title.AE_qual);
  if (rtn == SD_SUCCESS)
    {
    arDib->ae_title.AE_qual_pres = SD_TRUE;   /* defaults to FALSE if AE_Qualifier not present   */
    SSC_LOG_CFLOW1 ("SSC PARSE:      'AE_Qualifier'        = %u", arDib->ae_title.AE_qual);
    }
  else
    SSC_LOG_ERR1 ("SSC ERROR: 'AR_Name.AE_Qualifier' failed to parse from XML\n"
                  "  for 'AR_Name.Name'='%s'.", (arDib->name ? arDib->name : "?"));

}

/************************************************************************/
/*		_ssc_AR_Name_PSEL_EFun					*/
/************************************************************************/
static ST_VOID _ssc_AR_Name_PSEL_EFun (SX_DEC_CTRL *sxDecCtrl)
{
ST_RET             rtn;
SSC_STACK_CFG     *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;
SSC_AR_NAME       *sscArName;
DIB_ENTRY        *arDib;
ST_CHAR           *str;
ST_INT             len = 0;
ST_UINT            hexStrLen = 0;

  sscArName = (SSC_AR_NAME *) list_find_last ((DBL_LNK *) sscStackCfg->arNameList);
  if (!sscArName)
    {
    SSC_LOG_ERR0 ("SSC ERROR: 'AR_Name.PSEL' processing failed (sscArName=NULL).");
    sxDecCtrl->errCode = SX_USER_ERROR;
    return;
    }
  arDib = sscArName->arDib;

  rtn = sx_get_string_ptr (sxDecCtrl, &str, &len);
  if (rtn == SD_SUCCESS)
    {
    SSC_LOG_CFLOW1 ("SSC PARSE:      'PSEL'                ='%s'", str);
    /* NOTE: ascii_to_hex_str can handle length = 0.	*/
    if (ascii_to_hex_str (arDib->pres_addr.psel, &hexStrLen, MAX_PSEL_LEN, str) == SD_SUCCESS)
      arDib->pres_addr.psel_len = hexStrLen;
    else
      {
      SSC_LOG_ERR1 ("SSC ERROR: unable to parse hex digits 'AR_Name.PSEL'\n"
                    "  for 'AR_Name.Name'='%s'.", (strlen(arDib->name) ? arDib->name : "?"));
      sxDecCtrl->errCode = SX_USER_ERROR;
      }
    }
  else
    SSC_LOG_ERR1 ("SSC ERROR: 'AR_Name.PSEL' failed to parse from XML\n"
                  "  for 'AR_Name.Name'='%s'.", (strlen(arDib->name) ? arDib->name : "?"));
}

/************************************************************************/
/*		_ssc_AR_Name_SSEL_EFun					*/
/************************************************************************/
static ST_VOID _ssc_AR_Name_SSEL_EFun (SX_DEC_CTRL *sxDecCtrl)
{
ST_RET             rtn;
SSC_STACK_CFG     *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;
SSC_AR_NAME       *sscArName;
DIB_ENTRY        *arDib;
ST_CHAR           *str;
ST_INT             len = 0;
ST_UINT            hexStrLen = 0;

  sscArName = (SSC_AR_NAME *) list_find_last ((DBL_LNK *) sscStackCfg->arNameList);
  if (!sscArName)
    {
    SSC_LOG_ERR0 ("SSC ERROR: 'AR_Name.SSEL' processing failed (sscArName=NULL).");
    sxDecCtrl->errCode = SX_USER_ERROR;
    return;
    }
  arDib = sscArName->arDib;

  rtn = sx_get_string_ptr (sxDecCtrl, &str, &len);
  if (rtn == SD_SUCCESS)
    {
    SSC_LOG_CFLOW1 ("SSC PARSE:      'SSEL'                ='%s'", str);
    /* NOTE: ascii_to_hex_str can handle length = 0.	*/
    if (ascii_to_hex_str (arDib->pres_addr.ssel, &hexStrLen, MAX_SSEL_LEN, str) == SD_SUCCESS)
      arDib->pres_addr.ssel_len = hexStrLen;
    else
      {
      SSC_LOG_ERR1 ("SSC ERROR: unable to parse hex digits 'AR_Name.SSEL'\n"
                    "  for 'AR_Name.Name'='%s'.", (strlen(arDib->name) ? arDib->name : "?"));
      sxDecCtrl->errCode = SX_USER_ERROR;
      }
    }
  else
    SSC_LOG_ERR1 ("SSC ERROR: 'AR_Name.SSEL' failed to parse from XML\n"
                  "  for 'AR_Name.Name'='%s'.", (strlen(arDib->name) ? arDib->name : "?"));
}

/************************************************************************/
/*		_ssc_AR_Name_TSEL_EFun					*/
/************************************************************************/
static ST_VOID _ssc_AR_Name_TSEL_EFun (SX_DEC_CTRL *sxDecCtrl)
{
ST_RET             rtn;
SSC_STACK_CFG     *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;
SSC_AR_NAME       *sscArName;
DIB_ENTRY        *arDib;
ST_CHAR           *str;
ST_INT             len = 0;
ST_UINT            hexStrLen = 0;

  sscArName = (SSC_AR_NAME *) list_find_last ((DBL_LNK *) sscStackCfg->arNameList);
  if (!sscArName)
    {
    SSC_LOG_ERR0 ("SSC ERROR: 'AR_Name.TSEL' processing failed (sscArName=NULL).");
    sxDecCtrl->errCode = SX_USER_ERROR;
    return;
    }
  arDib = sscArName->arDib;

  rtn = sx_get_string_ptr (sxDecCtrl, &str, &len);
  if (rtn == SD_SUCCESS)
    {
    SSC_LOG_CFLOW1 ("SSC PARSE:      'TSEL'                ='%s'", str);
    /* NOTE: ascii_to_hex_str can handle length = 0.	*/
    if (ascii_to_hex_str (arDib->pres_addr.tsel, &hexStrLen, MAX_TSEL_LEN, str) == SD_SUCCESS)
      arDib->pres_addr.tsel_len = hexStrLen;
    else
      {
      SSC_LOG_ERR1 ("SSC ERROR: unable to parse hex digits 'AR_Name.TSEL'\n"
                    "  for 'AR_Name.Name'='%s'.", (strlen(arDib->name) ? arDib->name : "?"));
      sxDecCtrl->errCode = SX_USER_ERROR;
      }
    }
  else
    SSC_LOG_ERR1 ("SSC ERROR: 'AR_Name.TSEL' failed to parse from XML\n"
                  "  for 'AR_Name.Name'='%s'.", (strlen(arDib->name) ? arDib->name : "?"));
}

/************************************************************************/
/*		_ssc_AR_Name_IP_Address_EFun				*/
/************************************************************************/
static ST_VOID _ssc_AR_Name_IP_Address_EFun (SX_DEC_CTRL *sxDecCtrl)
{
SSC_STACK_CFG     *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;
SSC_AR_NAME       *sscArName;
DIB_ENTRY        *arDib;
ST_RET             rtn;
ST_CHAR           *str;
ST_INT             len = 0;
ST_BOOLEAN         useGetHostByName = SD_TRUE;	/* should we ? */


  sscArName = (SSC_AR_NAME *) list_find_last ((DBL_LNK *) sscStackCfg->arNameList);
  if (!sscArName)
    {
    SSC_LOG_ERR0 ("SSC ERROR: 'AR_Name.IP_Address' processing failed (sscArName=NULL).");
    sxDecCtrl->errCode = SX_USER_ERROR;
    return;
    }
  arDib = sscArName->arDib;
  if (arDib->pres_addr.tp_type == TP_TYPE_TP4 && arDib->pres_addr.nsap_len > 0)
    {
    SSC_LOG_ERR1 ("SSC ERROR: 'AR_Name.IP_Address' processing failed\n"
                  "  (NSAP and IP_Address can not configured at the same time) in 'AR_Name.Name'='%s'.",
                  (strlen(arDib->name) ? arDib->name : "?"));
    sxDecCtrl->errCode = SX_USER_ERROR;
    return;
    }

  rtn = sx_get_string_ptr (sxDecCtrl, &str, &len);
  if (rtn == SD_SUCCESS)
    {
    if (len > 0)
      {
      SSC_LOG_CFLOW1 ("SSC PARSE:      'IP_Address'          ='%s'", str);
      arDib->pres_addr.netAddr.ip = convertIPAddr (str, useGetHostByName);
      if (arDib->pres_addr.netAddr.ip == htonl(INADDR_NONE) ||
          arDib->pres_addr.netAddr.ip == 0)
        {
        SSC_LOG_ERR1 ("SSC ERROR: unable to convert 'AR_Name.IP_Address'\n"
                      "  for 'AR_Name.Name'='%s'.", (strlen(arDib->name) ? arDib->name : "?"));
        sxDecCtrl->errCode = SX_ERR_CONVERT;
        }
      else
        arDib->pres_addr.tp_type = TP_TYPE_TCP;
      }
    else
      {
      SSC_LOG_ERR1 ("SSC ERROR: 'AR_Name.IP_Address' not supplied for 'AR_Name.Name'='%s'.",
                    (strlen(arDib->name) ? arDib->name : "?"));
      sxDecCtrl->errCode = SX_USER_ERROR;
      }
    }
  else
    SSC_LOG_ERR1 ("SSC ERROR: 'AR_Name.IP_Address' failed to parse from XML\n"
                  "  for 'AR_Name.Name'='%s'.", (strlen(arDib->name) ? arDib->name : "?"));
}

/************************************************************************/
/*			_ssc_AR_Name_Tcp_Port_EFun			*/
/************************************************************************/
static ST_VOID _ssc_AR_Name_Tcp_Port_EFun (SX_DEC_CTRL *sxDecCtrl)
{
ST_RET          rtn;
SSC_STACK_CFG  *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;
SSC_AR_NAME    *sscArName;
DIB_ENTRY     *arDib;

  sscArName = (SSC_AR_NAME *) list_find_last ((DBL_LNK *) sscStackCfg->arNameList);
  if (!sscArName)
    {
    SSC_LOG_ERR0 ("SSC ERROR: 'AR_Name.Tcp_Port' processing failed (sscArName=NULL).");
    sxDecCtrl->errCode = SX_USER_ERROR;
    return;
    }
  arDib = sscArName->arDib;

  rtn = sx_get_uint16 (sxDecCtrl, &arDib->pres_addr.port);
  if (rtn == SD_SUCCESS)
    SSC_LOG_CFLOW1 ("SSC PARSE:      'Tcp_Port'            = %u", (ST_UINT) arDib->pres_addr.port);
  else
    SSC_LOG_ERR1 ("SSC ERROR: 'AR_Name.Tcp_Port' failed to parse from XML\n"
                  "  for 'AR_Name.Name'='%s'.", (strlen(arDib->name) ? arDib->name : "?"));
}

/************************************************************************/
/*		_ssc_AR_Name_NSAP_EFun					*/
/************************************************************************/
static ST_VOID _ssc_AR_Name_NSAP_EFun (SX_DEC_CTRL *sxDecCtrl)
{
SSC_STACK_CFG     *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;
SSC_AR_NAME       *sscArName;
DIB_ENTRY        *arDib;
ST_RET             rtn;
ST_CHAR           *str;
ST_INT             len = 0;
ST_UINT            hexStrLen = 0;

  sscArName = (SSC_AR_NAME *) list_find_last ((DBL_LNK *) sscStackCfg->arNameList);
  if (!sscArName)
    {
    SSC_LOG_ERR0 ("SSC ERROR: 'AR_Name.NSAP' processing failed (sscArName=NULL).");
    sxDecCtrl->errCode = SX_USER_ERROR;
    return;
    }
  arDib = sscArName->arDib;
  if (arDib->pres_addr.tp_type == TP_TYPE_TCP && arDib->pres_addr.netAddr.ip != 0)
    {
    SSC_LOG_ERR1 ("SSC ERROR: 'AR_Name.NSAP' processing failed\n"
                  "  (NSAP and IP_Address can not configured at the same time) in 'AR_Name.Name'='%s'.",
                  (strlen(arDib->name) ? arDib->name : "?"));
    sxDecCtrl->errCode = SX_USER_ERROR;
    return;
    }

  rtn = sx_get_string_ptr (sxDecCtrl, &str, &len);
  if (rtn == SD_SUCCESS)
    {
    if (len > 0)
      {
      SSC_LOG_CFLOW1 ("SSC PARSE:      'NSAP'                ='%s'", str);
      if (ascii_to_hex_str (arDib->pres_addr.netAddr.nsap, &hexStrLen, CLNP_MAX_LEN_NSAP, str) == SD_SUCCESS)
        {
        arDib->pres_addr.tp_type = TP_TYPE_TP4;
        arDib->pres_addr.nsap_len = hexStrLen;
	}
      else
        {
        SSC_LOG_ERR1 ("SSC ERROR: unable to parse hex digits 'AR_Name.NSAP'\n"
                      "  for 'AR_Name.Name'='%s'.", (strlen(arDib->name) ? arDib->name : "?"));
        sxDecCtrl->errCode = SX_USER_ERROR;
        }
      }
    else
      {
      SSC_LOG_ERR1 ("SSC ERROR: 'AR_Name.NSAP' not supplied for 'AR_Name.Name'='%s'.",
                    (strlen(arDib->name) ? arDib->name : "?"));
      sxDecCtrl->errCode = SX_USER_ERROR;
      }
    }
  else
    SSC_LOG_ERR1 ("SSC ERROR: 'AR_Name.NSAP' failed to parse from XML\n"
                  "  for 'AR_Name.Name'='%s'.", (strlen(arDib->name) ? arDib->name : "?"));
}

/************************************************************************/
/*		_ssc_AR_Name_Authentication_Mode_EFun			*/
/* NOTE: the S_SEC_LOC_REM_AR is optional so we wait with calloc until	*/
/*       we parse security related fields. 				*/
/************************************************************************/
static ST_VOID _ssc_AR_Name_Authentication_Mode_EFun (SX_DEC_CTRL *sxDecCtrl)
{
ST_RET             rtn;
SSC_STACK_CFG     *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;
SSC_AR_NAME       *sscArName;
S_SEC_LOC_REM_AR  *arSec;
ST_CHAR           *str;
ST_INT             len = 0;

  sscArName = (SSC_AR_NAME *) list_find_last ((DBL_LNK *) sscStackCfg->arNameList);
  if (!sscArName)
    {
    SSC_LOG_ERR0 ("SSC ERROR: 'AR_Name.Authentication_Mode' processing failed (sscArName=NULL).");
    sxDecCtrl->errCode = SX_USER_ERROR;
    return;
    }
  arSec = sscArName->arSec;

  rtn = sx_get_string_ptr (sxDecCtrl, &str, &len);
  if (rtn == SD_SUCCESS)
    {
    rtn = secCfgStrToInt (arAuthModeTypeStrings, str, &arSec->arAuthMode);
    if (rtn == SD_SUCCESS)
      {
      SSC_LOG_CFLOW1 ("SSC PARSE:      'Authentication_Mode' ='%s'", str);
      if (!sscStackCfg->secureDllLib && 
          (arSec->arAuthMode == S_AR_AUTH_MODE_SSL || arSec->arAuthMode == S_AR_AUTH_MODE_MACE))
        {
        SSC_LOG_ERR2 ("SSC ERROR: NULL Security does not support 'AR_Name.Authentication_Mode' value '%s'\n"
                      "  for 'AR_Name.Name'='%s'.", str, (strlen(arSec->arName) ? arSec->arName : "?"));
        sxDecCtrl->errCode = SD_FAILURE;
        }
      }
    else
      {
      SSC_LOG_ERR2 ("SSC ERROR: invalid 'AR_Name.Authentication_Mode' value '%s'\n"
                    "  for 'AR_Name.Name'='%s'.", str, (strlen(arSec->arName) ? arSec->arName : "?"));
      sxDecCtrl->errCode = SD_FAILURE;
      }
    }
  else
    SSC_LOG_ERR1 ("SSC ERROR: 'AR_Name.Authentication_Mode' failed to parse from XML\n"
                  "  for 'AR_Name.Name'='%s'.", (strlen(arSec->arName) ? arSec->arName : "?"));
}

/************************************************************************/
/*		_ssc_AR_Matching_Profile_SEFun				*/
/************************************************************************/
static ST_VOID _ssc_AR_Matching_Profile_SEFun (SX_DEC_CTRL *sxDecCtrl)
{
SSC_STACK_CFG           *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;

  if (sxDecCtrl->reason == SX_ELEMENT_START)
    {
    if (sscAddArMatchingProfile (sscStackCfg))
      {
      SSC_LOG_CFLOW0 ("SSC PARSE:    'AR_Matching_Profile' start");
      SX_PUSH (_ssc_AR_Matching_Profile_Elements);
      }
    else
      {
      SSC_LOG_ERR0 ("SSC PARSE: add AR_Matching_Profile failed.");
      sxDecCtrl->errCode = SX_USER_ERROR;
      }
    }
  else
    SSC_LOG_CFLOW0 ("SSC PARSE:    'AR_Matching_Profile' end\n");
}

/************************************************************************/
/*		_ssc_AR_Matching_Profile_Name_EFun			*/
/************************************************************************/
static ST_VOID _ssc_AR_Matching_Profile_Name_EFun (SX_DEC_CTRL *sxDecCtrl)
{
ST_RET                   rtn;
SSC_STACK_CFG           *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;
SSC_AR_MATCHING_PROFILE *arMatchingProfile;
ST_CHAR                 *arMatchingProfileName;
ST_INT                   len;

  arMatchingProfile = (SSC_AR_MATCHING_PROFILE *) list_find_last ((DBL_LNK *) sscStackCfg->arMatchingProfileList);
  if (!arMatchingProfile)
    {
    SSC_LOG_ERR0 ("SSC ERROR: 'AR_Matching_Profile.Name' processing failed (arMatchingProfile=NULL).");
    sxDecCtrl->errCode = SX_USER_ERROR;
    return;
    }

  len = sizeof (arMatchingProfile->name) - 1;	/* -1 for string terminator	*/
  rtn = sx_get_string_ptr (sxDecCtrl, &arMatchingProfileName, &len);
  if (rtn == SD_SUCCESS)
    {
    if (len > 0)
      {
      /* check for duplicates */
      if (!sscFindArMatchingProfile (sscStackCfg, arMatchingProfileName))
        {
	strcpy  (arMatchingProfile->name, arMatchingProfileName);
        SSC_LOG_CFLOW1 ("SSC PARSE:      'Name'               ='%s'", arMatchingProfileName);

	}
      else
        {
        SSC_LOG_ERR1 ("SSC ERROR: duplicate 'AR_Matching_Profile.Name'='%s' found.", arMatchingProfileName);
        sxDecCtrl->errCode = SX_USER_ERROR;
        }
      }
    else
      {
      SSC_LOG_ERR0 ("SSC ERROR: 'AR_Matching_Profile.Name' not supplied.");
      sxDecCtrl->errCode = SX_USER_ERROR;
      }
    }
  else
    SSC_LOG_ERR0 ("SSC ERROR: 'AR_Matching_Profile.Name' failed to parse from XML.");
}

/************************************************************************/
/*		_ssc_AR_Matching_Profile_Allow_Missing_AE_EFun		*/
/************************************************************************/
static ST_VOID _ssc_AR_Matching_Profile_Allow_Missing_AE_EFun (SX_DEC_CTRL *sxDecCtrl)
{
ST_RET                   rtn;
SSC_STACK_CFG           *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;
SSC_AR_MATCHING_PROFILE *arMatchingProfile;
DIB_MATCH_CTRL          *dibMatch;

  arMatchingProfile = (SSC_AR_MATCHING_PROFILE *) list_find_last ((DBL_LNK *) sscStackCfg->arMatchingProfileList);
  if (!arMatchingProfile)
    {
    SSC_LOG_ERR0 ("SSC ERROR: 'AR_Matching_Profile.Allow_Missing_AE' processing failed (arMatchingProfile=NULL).");
    sxDecCtrl->errCode = SX_USER_ERROR;
    return;
    }
  dibMatch = arMatchingProfile->dibMatch;

  rtn = sx_get_string_YesNo (sxDecCtrl, &dibMatch->match_allow_missing_ae_elements, SX_USER_ERROR);
  if (rtn == SD_SUCCESS)
    SSC_LOG_CFLOW1 ("SSC PARSE:      'Allow_Missing_AE'   = %s", (dibMatch->match_allow_missing_ae_elements ? "Yes" : "No"));
  else
    SSC_LOG_ERR0 ("SSC ERROR: 'AR_Matching_Profile.Allow_Missing_AE' failed to parse from XML.");
}

/************************************************************************/
/*		_ssc_AR_Matching_Profile_Allow_Extra_AE_EFun		*/
/************************************************************************/
static ST_VOID _ssc_AR_Matching_Profile_Allow_Extra_AE_EFun (SX_DEC_CTRL *sxDecCtrl)
{
ST_RET                   rtn;
SSC_STACK_CFG           *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;
SSC_AR_MATCHING_PROFILE *arMatchingProfile;
DIB_MATCH_CTRL          *dibMatch;

  arMatchingProfile = (SSC_AR_MATCHING_PROFILE *) list_find_last ((DBL_LNK *) sscStackCfg->arMatchingProfileList);
  if (!arMatchingProfile)
    {
    SSC_LOG_ERR0 ("SSC ERROR: 'AR_Matching_Profile.Allow_Extra_AE' processing failed (arMatchingProfile=NULL).");
    sxDecCtrl->errCode = SX_USER_ERROR;
    return;
    }
  dibMatch = arMatchingProfile->dibMatch;

  rtn = sx_get_string_YesNo (sxDecCtrl, &dibMatch->match_allow_extra_ae_elements, SX_USER_ERROR);
  if (rtn == SD_SUCCESS)
    SSC_LOG_CFLOW1 ("SSC PARSE:      'Allow_Extra_AE'     = %s", (dibMatch->match_allow_extra_ae_elements ? "Yes" : "No"));
  else
    SSC_LOG_ERR0 ("SSC ERROR: 'AR_Matching_Profile.Allow_Extra_AE' failed to parse from XML.");
}

/************************************************************************/
/*		_ssc_AR_Matching_Profile_Match_AP_Title_EFun		*/
/************************************************************************/
static ST_VOID _ssc_AR_Matching_Profile_Match_AP_Title_EFun (SX_DEC_CTRL *sxDecCtrl)
{
ST_RET                   rtn;
SSC_STACK_CFG           *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;
SSC_AR_MATCHING_PROFILE *arMatchingProfile;
DIB_MATCH_CTRL          *dibMatch;

  arMatchingProfile = (SSC_AR_MATCHING_PROFILE *) list_find_last ((DBL_LNK *) sscStackCfg->arMatchingProfileList);
  if (!arMatchingProfile)
    {
    SSC_LOG_ERR0 ("SSC ERROR: 'AR_Matching_Profile.Match_AP_Title' processing failed (arMatchingProfile=NULL).");
    sxDecCtrl->errCode = SX_USER_ERROR;
    return;
    }
  dibMatch = arMatchingProfile->dibMatch;

  rtn = sx_get_string_YesNo (sxDecCtrl, &dibMatch->match_ap_title, SX_USER_ERROR);
  if (rtn == SD_SUCCESS)
    SSC_LOG_CFLOW1 ("SSC PARSE:      'Match_AP_Title'     = %s", (dibMatch->match_ap_title ? "Yes" : "No"));
  else
    SSC_LOG_ERR0 ("SSC ERROR: 'AR_Matching_Profile.Match_AP_Title' failed to parse from XML.");
}

/************************************************************************/
/*		_ssc_AR_Matching_Profile_Match_AE_Qualifier_EFun	*/
/************************************************************************/
static ST_VOID _ssc_AR_Matching_Profile_Match_AE_Qualifier_EFun (SX_DEC_CTRL *sxDecCtrl)
{
ST_RET                   rtn;
SSC_STACK_CFG           *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;
SSC_AR_MATCHING_PROFILE *arMatchingProfile;
DIB_MATCH_CTRL          *dibMatch;

  arMatchingProfile = (SSC_AR_MATCHING_PROFILE *) list_find_last ((DBL_LNK *) sscStackCfg->arMatchingProfileList);
  if (!arMatchingProfile)
    {
    SSC_LOG_ERR0 ("SSC ERROR: 'AR_Matching_Profile.Match_AE_Qualifier' processing failed (arMatchingProfile=NULL).");
    sxDecCtrl->errCode = SX_USER_ERROR;
    return;
    }
  dibMatch = arMatchingProfile->dibMatch;

  rtn = sx_get_string_YesNo (sxDecCtrl, &dibMatch->match_ae_qualifier, SX_USER_ERROR);
  if (rtn == SD_SUCCESS)
    SSC_LOG_CFLOW1 ("SSC PARSE:      'Match_AE_Qualifier' = %s", (dibMatch->match_ae_qualifier ? "Yes" : "No"));
  else
    SSC_LOG_ERR0 ("SSC ERROR: 'AR_Matching_Profile.Match_AE_Qualifier' failed to parse from XML.");
}

/************************************************************************/
/*		_ssc_AR_Matching_Profile_Match_AP_Invoke_EFun		*/
/************************************************************************/
static ST_VOID _ssc_AR_Matching_Profile_Match_AP_Invoke_EFun (SX_DEC_CTRL *sxDecCtrl)
{
ST_RET                   rtn;
SSC_STACK_CFG           *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;
SSC_AR_MATCHING_PROFILE *arMatchingProfile;
DIB_MATCH_CTRL          *dibMatch;

  arMatchingProfile = (SSC_AR_MATCHING_PROFILE *) list_find_last ((DBL_LNK *) sscStackCfg->arMatchingProfileList);
  if (!arMatchingProfile)
    {
    SSC_LOG_ERR0 ("SSC ERROR: 'AR_Matching_Profile.Match_AP_Invoke' processing failed (arMatchingProfile=NULL).");
    sxDecCtrl->errCode = SX_USER_ERROR;
    return;
    }
  dibMatch = arMatchingProfile->dibMatch;

  rtn = sx_get_string_YesNo (sxDecCtrl, &dibMatch->match_ap_invoke, SX_USER_ERROR);
  if (rtn == SD_SUCCESS)
    SSC_LOG_CFLOW1 ("SSC PARSE:      'Match_AP_Invoke'    = %s", (dibMatch->match_ap_invoke ? "Yes" : "No"));
  else
    SSC_LOG_ERR0 ("SSC ERROR: 'AR_Matching_Profile.Match_AP_Invoke' failed to parse from XML.");
}

/************************************************************************/
/*		_ssc_AR_Matching_Profile_Match_AE_Invoke_EFun		*/
/************************************************************************/
static ST_VOID _ssc_AR_Matching_Profile_Match_AE_Invoke_EFun (SX_DEC_CTRL *sxDecCtrl)
{
ST_RET                   rtn;
SSC_STACK_CFG           *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;
SSC_AR_MATCHING_PROFILE *arMatchingProfile;
DIB_MATCH_CTRL          *dibMatch;

  arMatchingProfile = (SSC_AR_MATCHING_PROFILE *) list_find_last ((DBL_LNK *) sscStackCfg->arMatchingProfileList);
  if (!arMatchingProfile)
    {
    SSC_LOG_ERR0 ("SSC ERROR: 'AR_Matching_Profile.Match_AE_Invoke' processing failed (arMatchingProfile=NULL).");
    sxDecCtrl->errCode = SX_USER_ERROR;
    return;
    }
  dibMatch = arMatchingProfile->dibMatch;

  rtn = sx_get_string_YesNo (sxDecCtrl, &dibMatch->match_ae_invoke, SX_USER_ERROR);
  if (rtn == SD_SUCCESS)
    SSC_LOG_CFLOW1 ("SSC PARSE:      'Match_AE_Invoke'    = %s", (dibMatch->match_ae_invoke ? "Yes" : "No"));
  else
    SSC_LOG_ERR0 ("SSC ERROR: 'AR_Matching_Profile.Match_AE_Invoke' failed to parse from XML.");
}

/************************************************************************/
/*		_ssc_AR_Matching_Profile_Match_PSEL_EFun		*/
/************************************************************************/
static ST_VOID _ssc_AR_Matching_Profile_Match_PSEL_EFun (SX_DEC_CTRL *sxDecCtrl)
{
ST_RET                   rtn;
SSC_STACK_CFG           *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;
SSC_AR_MATCHING_PROFILE *arMatchingProfile;
DIB_MATCH_CTRL          *dibMatch;

  arMatchingProfile = (SSC_AR_MATCHING_PROFILE *) list_find_last ((DBL_LNK *) sscStackCfg->arMatchingProfileList);
  if (!arMatchingProfile)
    {
    SSC_LOG_ERR0 ("SSC ERROR: 'AR_Matching_Profile.Match_PSEL' processing failed (arMatchingProfile=NULL).");
    sxDecCtrl->errCode = SX_USER_ERROR;
    return;
    }
  dibMatch = arMatchingProfile->dibMatch;

  rtn = sx_get_string_YesNo (sxDecCtrl, &dibMatch->match_psel, SX_USER_ERROR);
  if (rtn == SD_SUCCESS)
    SSC_LOG_CFLOW1 ("SSC PARSE:      'Match_PSEL'         = %s", (dibMatch->match_psel ? "Yes" : "No"));
  else
    SSC_LOG_ERR0 ("SSC ERROR: 'AR_Matching_Profile.Match_PSEL' failed to parse from XML.");
}


/************************************************************************/
/*		_ssc_AR_Matching_Profile_Match_SSEL_EFun		*/
/************************************************************************/
static ST_VOID _ssc_AR_Matching_Profile_Match_SSEL_EFun (SX_DEC_CTRL *sxDecCtrl)
{
ST_RET                   rtn;
SSC_STACK_CFG           *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;
SSC_AR_MATCHING_PROFILE *arMatchingProfile;
DIB_MATCH_CTRL          *dibMatch;

  arMatchingProfile = (SSC_AR_MATCHING_PROFILE *) list_find_last ((DBL_LNK *) sscStackCfg->arMatchingProfileList);
  if (!arMatchingProfile)
    {
    SSC_LOG_ERR0 ("SSC ERROR: 'AR_Matching_Profile.Match_SSEL' processing failed (arMatchingProfile=NULL).");
    sxDecCtrl->errCode = SX_USER_ERROR;
    return;
    }
  dibMatch = arMatchingProfile->dibMatch;

  rtn = sx_get_string_YesNo (sxDecCtrl, &dibMatch->match_ssel, SX_USER_ERROR);
  if (rtn == SD_SUCCESS)
    SSC_LOG_CFLOW1 ("SSC PARSE:      'Match_SSEL'         = %s", (dibMatch->match_ssel ? "Yes" : "No"));
  else
    SSC_LOG_ERR0 ("SSC ERROR: 'AR_Matching_Profile.Match_SSEL' failed to parse from XML.");
}

/************************************************************************/
/*		_ssc_AR_Matching_Profile_Match_TSEL_EFun		*/
/************************************************************************/
static ST_VOID _ssc_AR_Matching_Profile_Match_TSEL_EFun (SX_DEC_CTRL *sxDecCtrl)
{
ST_RET                   rtn;
SSC_STACK_CFG           *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;
SSC_AR_MATCHING_PROFILE *arMatchingProfile;
DIB_MATCH_CTRL          *dibMatch;

  arMatchingProfile = (SSC_AR_MATCHING_PROFILE *) list_find_last ((DBL_LNK *) sscStackCfg->arMatchingProfileList);
  if (!arMatchingProfile)
    {
    SSC_LOG_ERR0 ("SSC ERROR: 'AR_Matching_Profile.Match_TSEL' processing failed (arMatchingProfile=NULL).");
    sxDecCtrl->errCode = SX_USER_ERROR;
    return;
    }
  dibMatch = arMatchingProfile->dibMatch;

  rtn = sx_get_string_YesNo (sxDecCtrl, &dibMatch->match_tsel, SX_USER_ERROR);
  if (rtn == SD_SUCCESS)
    SSC_LOG_CFLOW1 ("SSC PARSE:      'Match_TSEL'         = %s", (dibMatch->match_tsel ? "Yes" : "No"));
  else
    SSC_LOG_ERR0 ("SSC ERROR: 'AR_Matching_Profile.Match_TSEL' failed to parse from XML.");
}

/************************************************************************/
/*		_ssc_AR_Matching_Profile_Match_IP_Address_EFun		*/
/************************************************************************/
static ST_VOID _ssc_AR_Matching_Profile_Match_IP_Address_EFun (SX_DEC_CTRL *sxDecCtrl)
{
ST_RET                   rtn;
SSC_STACK_CFG           *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;
SSC_AR_MATCHING_PROFILE *arMatchingProfile;
DIB_MATCH_CTRL          *dibMatch;

  arMatchingProfile = (SSC_AR_MATCHING_PROFILE *) list_find_last ((DBL_LNK *) sscStackCfg->arMatchingProfileList);
  if (!arMatchingProfile)
    {
    SSC_LOG_ERR0 ("SSC ERROR: 'AR_Matching_Profile.Match_IP_Address' processing failed (arMatchingProfile=NULL).");
    sxDecCtrl->errCode = SX_USER_ERROR;
    return;
    }
  dibMatch = arMatchingProfile->dibMatch;

  rtn = sx_get_string_YesNo (sxDecCtrl, &dibMatch->match_net_addr, SX_USER_ERROR);
  if (rtn == SD_SUCCESS)
    SSC_LOG_CFLOW1 ("SSC PARSE:      'Match_IP_Address'   = %s", (dibMatch->match_net_addr ? "Yes" : "No"));
  else
    SSC_LOG_ERR0 ("SSC ERROR: 'AR_Matching_Profile.Match_IP_Address' failed to parse from XML.");
}

/************************************************************************/
/*		_ssc_AR_Matching_Profile_Host_Name_Support_EFun		*/
/************************************************************************/
static ST_VOID _ssc_AR_Matching_Profile_Host_Name_Support_EFun	(SX_DEC_CTRL *sxDecCtrl)
{
ST_RET                   rtn;
SSC_STACK_CFG           *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;
SSC_AR_MATCHING_PROFILE *arMatchingProfile;
DIB_MATCH_CTRL          *dibMatch;

  arMatchingProfile = (SSC_AR_MATCHING_PROFILE *) list_find_last ((DBL_LNK *) sscStackCfg->arMatchingProfileList);
  if (!arMatchingProfile)
    {
    SSC_LOG_ERR0 ("SSC ERROR: 'AR_Matching_Profile.Host_Name_Support' processing failed (arMatchingProfile=NULL).");
    sxDecCtrl->errCode = SX_USER_ERROR;
    return;
    }
  dibMatch = arMatchingProfile->dibMatch;

  rtn = sx_get_string_YesNo (sxDecCtrl, &dibMatch->use_gethostbyname, SX_USER_ERROR);
  if (rtn == SD_SUCCESS)
    SSC_LOG_CFLOW1 ("SSC PARSE:      'Host_Name_Support'  = %s", (dibMatch->use_gethostbyname ? "Yes" : "No"));
  else
    SSC_LOG_ERR0 ("SSC ERROR: 'AR_Matching_Profile.Host_Name_Support' failed to parse from XML.");
}


/************************************************************************/
/*		_ssc_Security_Profile_SEFun				*/
/************************************************************************/
static ST_VOID _ssc_Security_Profile_SEFun (SX_DEC_CTRL *sxDecCtrl)
{
SSC_STACK_CFG           *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;

  if (sxDecCtrl->reason == SX_ELEMENT_START)
    {
    if (sscAddSecurityProfile (sscStackCfg))
      {
      SSC_LOG_CFLOW0 ("SSC PARSE:    'Security_Profile' start");
      SX_PUSH (_ssc_Security_Profile_Elements);
      }
    else
      {
      SSC_LOG_ERR0 ("SSC PARSE:    Add Security_Profile failed");
      sxDecCtrl->errCode = SX_USER_ERROR;
      }
    }
  else
    SSC_LOG_CFLOW0 ("SSC PARSE:    'Security_Profile' end\n");
}

/************************************************************************/
/*		_ssc_Security_Profile_Name_EFun				*/
/************************************************************************/
static ST_VOID _ssc_Security_Profile_Name_EFun	(SX_DEC_CTRL *sxDecCtrl)
{
ST_RET                   rtn;
SSC_STACK_CFG           *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;
SSC_SECURITY_PROFILE    *securityProfile;
ST_CHAR                 *securityProfileName;
ST_INT                   len;

  securityProfile = (SSC_SECURITY_PROFILE *) list_find_last ((DBL_LNK *) sscStackCfg->securityProfileList);
  if (!securityProfile)
    {
    SSC_LOG_ERR0 ("SSC ERROR: 'Security_Profile.Name' processing failed (securityProfile=NULL).");
    sxDecCtrl->errCode = SX_USER_ERROR;
    return;
    }

  len = sizeof (securityProfile->name) - 1;	/* -1 for string terminator	*/
  rtn = sx_get_string_ptr (sxDecCtrl, &securityProfileName, &len);
  if (rtn == SD_SUCCESS)
    {
    if (len > 0)
      {
      /* check for duplicates */
      if (!sscFindSecurityProfile (sscStackCfg, securityProfileName))
        {
	strcpy (securityProfile->name, securityProfileName);
        SSC_LOG_CFLOW1 ("SSC PARSE:      'Name'                  ='%s'", securityProfileName);
	}
      else
        {
        SSC_LOG_ERR1 ("SSC ERROR: duplicate 'Security_Profile.Name'='%s' found.", securityProfileName);
        sxDecCtrl->errCode = SX_USER_ERROR;
        }
      }
    else
      {
      SSC_LOG_ERR0 ("SSC ERROR: 'Security_Profile.Name' not supplied.");
      sxDecCtrl->errCode = SX_USER_ERROR;
      }
    }
  else
    SSC_LOG_ERR0 ("SSC ERROR: 'Security_Profile.Name' failed to parse from XML.");
}

/************************************************************************/
/*		_ssc_Security_Profile_Secure_EFun			*/
/************************************************************************/
static ST_VOID _ssc_Security_Profile_Secure_EFun (SX_DEC_CTRL *sxDecCtrl)
{
ST_RET                   rtn;
SSC_STACK_CFG           *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;
SSC_SECURITY_PROFILE    *securityProfile;

  securityProfile = (SSC_SECURITY_PROFILE *) list_find_last ((DBL_LNK *) sscStackCfg->securityProfileList);
  if (!securityProfile)
    {
    SSC_LOG_ERR0 ("SSC ERROR: 'Security_Profile.Secure' processing failed (securityProfile=NULL).");
    sxDecCtrl->errCode = SX_USER_ERROR;
    return;
    }

  rtn = sx_get_string_YesNo (sxDecCtrl, &securityProfile->secure, SX_USER_ERROR);
  if (rtn == SD_SUCCESS)
    SSC_LOG_CFLOW1 ("SSC PARSE:      'Secure'                = %s", (securityProfile->secure ? "Yes" : "No"));
  else
    SSC_LOG_ERR0 ("SSC ERROR: 'Security_Profile.Secure' failed to parse from XML.");
}

/************************************************************************/
/*		_ssc_Security_Profile_Non_Secure_Fallback_EFun		*/
/************************************************************************/
static ST_VOID _ssc_Security_Profile_Non_Secure_Fallback_EFun (SX_DEC_CTRL *sxDecCtrl)
{
ST_RET                   rtn;
SSC_STACK_CFG           *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;
SSC_SECURITY_PROFILE    *securityProfile;

  securityProfile = (SSC_SECURITY_PROFILE *) list_find_last ((DBL_LNK *) sscStackCfg->securityProfileList);
  if (!securityProfile)
    {
    SSC_LOG_ERR0 ("SSC ERROR: 'Security_Profile.Non_Secure_Fallback' processing failed (securityProfile=NULL).");
    sxDecCtrl->errCode = SX_USER_ERROR;
    return;
    }

  rtn = sx_get_string_YesNo (sxDecCtrl, &securityProfile->nonSecureFallbackEnabled, SX_USER_ERROR);
  if (rtn == SD_SUCCESS)
    SSC_LOG_CFLOW1 ("SSC PARSE:      'Non_Secure_Fallback'   = %s", (securityProfile->nonSecureFallbackEnabled ? "Yes" : "No"));
  else
    SSC_LOG_ERR0 ("SSC ERROR: 'Security_Profile.Non_Secure_Fallback' failed to parse from XML.");
}

/************************************************************************/
/*		_ssc_Security_Profile_MACE_Time_Seal_Window_EFun	*/
/************************************************************************/
static ST_VOID _ssc_Security_Profile_MACE_Time_Seal_Window_EFun (SX_DEC_CTRL *sxDecCtrl)
{
ST_RET                   rtn;
SSC_STACK_CFG           *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;
SSC_SECURITY_PROFILE    *securityProfile;

  securityProfile = (SSC_SECURITY_PROFILE *) list_find_last ((DBL_LNK *) sscStackCfg->securityProfileList);
  if (!securityProfile)
    {
    SSC_LOG_ERR0 ("SSC ERROR: 'Security_Profile.MACE_Time_Seal_Window' processing failed (securityProfile=NULL).");
    sxDecCtrl->errCode = SX_USER_ERROR;
    return;
    }

  rtn = sx_get_uint32 (sxDecCtrl, &securityProfile->maceTimeSealWindow);
  if (rtn == SD_SUCCESS)
    SSC_LOG_CFLOW1 ("SSC PARSE:      'MACE_Time_Seal_Window' = %u sec", securityProfile->maceTimeSealWindow);
  else
    SSC_LOG_ERR0 ("SSC ERROR: 'Security_Profile.MACE_Time_Seal_Window' failed to parse from XML.");
}

/************************************************************************/
/*		_ssc_Security_Profile_Encrypt_Req_Calling_EFun		*/
/************************************************************************/
static ST_VOID _ssc_Security_Profile_Encrypt_Req_Calling_EFun (SX_DEC_CTRL *sxDecCtrl)
{
ST_RET                   rtn;
SSC_STACK_CFG           *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;
SSC_SECURITY_PROFILE    *securityProfile;

  securityProfile = (SSC_SECURITY_PROFILE *) list_find_last ((DBL_LNK *) sscStackCfg->securityProfileList);
  if (!securityProfile)
    {
    SSC_LOG_ERR0 ("SSC ERROR: 'Security_Profile.Encrypt_Req_Calling' processing failed (securityProfile=NULL).");
    sxDecCtrl->errCode = SX_USER_ERROR;
    return;
    }

  rtn = sx_get_string_YesNo (sxDecCtrl, &securityProfile->encryptReqCalling, SX_USER_ERROR);
  if (rtn == SD_SUCCESS)
    SSC_LOG_CFLOW1 ("SSC PARSE:      'Encrypt_Req_Calling'   = %s", (securityProfile->encryptReqCalling ? "Yes" : "No"));
  else
    SSC_LOG_ERR0 ("SSC ERROR: 'Security_Profile.Encrypt_Req_Calling' failed to parse from XML.");
}

/************************************************************************/
/*		_ssc_Security_Profile_Encrypt_Req_Called_EFun		*/
/************************************************************************/
static ST_VOID _ssc_Security_Profile_Encrypt_Req_Called_EFun (SX_DEC_CTRL *sxDecCtrl)
{
ST_RET                   rtn;
SSC_STACK_CFG           *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;
SSC_SECURITY_PROFILE    *securityProfile;

  securityProfile = (SSC_SECURITY_PROFILE *) list_find_last ((DBL_LNK *) sscStackCfg->securityProfileList);
  if (!securityProfile)
    {
    SSC_LOG_ERR0 ("SSC ERROR: 'Security_Profile.Encrypt_Req_Called' processing failed (securityProfile=NULL).");
    sxDecCtrl->errCode = SX_USER_ERROR;
    return;
    }

  rtn = sx_get_string_YesNo (sxDecCtrl, &securityProfile->encryptReqCalled, SX_USER_ERROR);
  if (rtn == SD_SUCCESS)
    SSC_LOG_CFLOW1 ("SSC PARSE:      'Encrypt_Req_Called'    = %s", (securityProfile->encryptReqCalled ? "Yes" : "No"));
  else
    SSC_LOG_ERR0 ("SSC ERROR: 'Security_Profile.Encrypt_Req_Called' failed to parse from XML.");
}

/************************************************************************/
/*		_ssc_App_Network_SEFun					*/
/************************************************************************/
static ST_VOID _ssc_App_Network_SEFun (SX_DEC_CTRL *sxDecCtrl)
{
SSC_STACK_CFG      *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;

  if (sxDecCtrl->reason == SX_ELEMENT_START)
    {
    if (sscAddAppNetwork (sscStackCfg))
      {
      SSC_LOG_CFLOW0 ("SSC PARSE:    'App_Network' start");
      SX_PUSH (_ssc_App_Network_Elements);
      }
    else
      {
      SSC_LOG_ERR0 ("SSC PARSE: add 'App_Network' failed");
      sxDecCtrl->errCode = SX_USER_ERROR;
      }
    }
  else
    SSC_LOG_CFLOW0 ("SSC PARSE:    'App_Network' end\n");
}

/************************************************************************/
/*		_ssc_App_Network_App_Name_EFun				*/
/************************************************************************/
static ST_VOID _ssc_App_Network_App_Name_EFun (SX_DEC_CTRL *sxDecCtrl)
{
ST_RET              rtn;
SSC_STACK_CFG      *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;
SSC_APP_NETWORK    *appNetwork;
ST_CHAR            *appNetworkName;
ST_INT              len;

  appNetwork = (SSC_APP_NETWORK *) list_find_last ((DBL_LNK *) sscStackCfg->appNetworkList);
  if (!appNetwork)
    {
    SSC_LOG_ERR0 ("SSC ERROR: 'App_Network.App_Name' processing failed (appNetwork=NULL).");
    sxDecCtrl->errCode = SX_USER_ERROR;
    return;
    }

  len = sizeof (appNetwork->appName) - 1;	/* -1 for string terminator	*/
  rtn = sx_get_string_ptr (sxDecCtrl, &appNetworkName, &len);
  if (rtn == SD_SUCCESS)
    {
    if (len > 0)
      {
      /* check for duplicates */
      if (!sscFindAppNetwork (sscStackCfg, appNetworkName))
        {
	strcpy (appNetwork->appName ,appNetworkName);
        SSC_LOG_CFLOW1 ("SSC PARSE:      'App_Name'            ='%s'", appNetworkName);
	}
      else
        {
        SSC_LOG_ERR1("SSC ERROR: duplicate 'App_Network.App_Name'='%s' found.", appNetworkName);
        sxDecCtrl->errCode = SX_USER_ERROR;
        }
      }
    else
      {
      SSC_LOG_ERR0 ("SSC ERROR: 'App_Network.App_Name' not supplied.");
      sxDecCtrl->errCode = SX_USER_ERROR;
      }
    }
  else
    SSC_LOG_ERR0 ("SSC ERROR: 'App_Network.App_Name' failed to parse from XML.");
}

/************************************************************************/
/*		_ssc_App_Network_LeanT_Profile_EFun			*/
/************************************************************************/
static ST_VOID _ssc_App_Network_LeanT_Profile_EFun (SX_DEC_CTRL *sxDecCtrl)
{
ST_RET              rtn;
SSC_STACK_CFG      *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;
SSC_APP_NETWORK    *appNetwork;
SSC_LEANT_PROFILE  *leantProfile;
ST_CHAR            *leantProfileName;
ST_INT              len = 0;

  appNetwork = (SSC_APP_NETWORK *) list_find_last ((DBL_LNK *) sscStackCfg->appNetworkList);
  if (!appNetwork)
    {
    SSC_LOG_ERR0 ("SSC ERROR: 'App_Network.LeanT_Profile' processing failed (appNetwork=NULL).");
    sxDecCtrl->errCode = SX_USER_ERROR;
    return;
    }

  rtn = sx_get_string_ptr (sxDecCtrl, &leantProfileName, &len);
  if (rtn == SD_SUCCESS)
    {
    if (len > 0)
      {
      leantProfile = sscFindLeantProfile (sscStackCfg, leantProfileName);
      if (leantProfile)
        {
	appNetwork->leantProfile = leantProfile;
        SSC_LOG_CFLOW1 ("SSC PARSE:      'LeanT_Profile'       ='%s'", leantProfileName);
	}
      else
        {
        SSC_LOG_ERR1 ("SSC ERROR: 'App_Network.LeanT_Profile' '%s' not configured in XML.",
                      leantProfileName);
        sxDecCtrl->errCode = SX_USER_ERROR;
        }
      }
    else
      {
      SSC_LOG_ERR0 ("SSC ERROR: 'App_Network.LeanT_Profile' name not supplied.");
      sxDecCtrl->errCode = SX_USER_ERROR;
      }
    }
  else
    SSC_LOG_ERR0 ("SSC ERROR: 'App_Network.LeanT_Profile' name failed to parse from XML.");
}

/************************************************************************/
/*		_ssc_App_Network_Security_Profile_EFun			*/
/************************************************************************/
static ST_VOID _ssc_App_Network_Security_Profile_EFun (SX_DEC_CTRL *sxDecCtrl)
{
ST_RET                 rtn;
SSC_STACK_CFG         *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;
SSC_APP_NETWORK       *appNetwork;
SSC_SECURITY_PROFILE  *securityProfile;
ST_CHAR               *securityProfileName;
ST_INT                 len = 0;

  appNetwork = (SSC_APP_NETWORK *) list_find_last ((DBL_LNK *) sscStackCfg->appNetworkList);
  if (!appNetwork)
    {
    SSC_LOG_ERR0 ("SSC ERROR: 'App_Network.Security_Profile' processing failed (appNetwork=NULL).");
    sxDecCtrl->errCode = SX_USER_ERROR;
    return;
    }

  rtn = sx_get_string_ptr (sxDecCtrl, &securityProfileName, &len);
  if (rtn == SD_SUCCESS)
    {
    if (len > 0)
      {
      securityProfile = sscFindSecurityProfile (sscStackCfg, securityProfileName);
      if (securityProfile)
        {
	appNetwork->securityProfile = securityProfile;
        SSC_LOG_CFLOW1 ("SSC PARSE:      'Security_Profile'    ='%s'", securityProfileName);
	}
      else
        {
        SSC_LOG_ERR1 ("SSC ERROR: 'App_Network.Security_Profile' '%s' not configured in XML.",
                      securityProfileName);
        sxDecCtrl->errCode = SX_USER_ERROR;
        }
      }
    else
      {
      SSC_LOG_ERR0 ("SSC ERROR: 'App_Network.Security_Profile' name not supplied.");
      sxDecCtrl->errCode = SX_USER_ERROR;
      }
    }
  else
    SSC_LOG_ERR0 ("SSC ERROR: 'App_Network.Security_Profile' name failed to parse from XML.");
}

/************************************************************************/
/*		_ssc_App_Network_AR_Matching_Profile_EFun		*/
/************************************************************************/
static ST_VOID _ssc_App_Network_AR_Matching_Profile_EFun (SX_DEC_CTRL *sxDecCtrl)
{
ST_RET                   rtn;
SSC_STACK_CFG           *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;
SSC_APP_NETWORK         *appNetwork;
SSC_AR_MATCHING_PROFILE *arMatchingProfile;
ST_CHAR                 *arMatchingProfileName;
ST_INT                   len = 0;

  appNetwork = (SSC_APP_NETWORK *) list_find_last ((DBL_LNK *) sscStackCfg->appNetworkList);
  if (!appNetwork)
    {
    SSC_LOG_ERR0 ("SSC ERROR: 'App_Network.AR_Matching_Profile' processing failed (appNetwork=NULL).");
    sxDecCtrl->errCode = SX_USER_ERROR;
    return;
    }

  rtn = sx_get_string_ptr (sxDecCtrl, &arMatchingProfileName, &len);
  if (rtn == SD_SUCCESS)
    {
    if (len > 0)
      {
      arMatchingProfile = sscFindArMatchingProfile (sscStackCfg, arMatchingProfileName);
      if (arMatchingProfile)
        {
	appNetwork->arMatchingProfile = arMatchingProfile;
        SSC_LOG_CFLOW1 ("SSC PARSE:      'AR_Matching_Profile' ='%s'", arMatchingProfileName);
	}
      else
        {
        SSC_LOG_ERR1 ("SSC ERROR: 'App_Network.AR_Matching_Profile' '%s' not configured in XML.",
                      arMatchingProfileName);
        sxDecCtrl->errCode = SX_USER_ERROR;
        }
      }
    else
      {
      SSC_LOG_ERR0 ("SSC ERROR: 'App_Network.AR_Matching_Profile' name not supplied.");
      sxDecCtrl->errCode = SX_USER_ERROR;
      }
    }
  else
    SSC_LOG_ERR0 ("SSC ERROR: 'App_Network.AR_Matching_Profile' name failed to parse from XML.");
}

/************************************************************************/
/*		_ssc_App_Network_App_AR_Name_EFun			*/
/* NOTE: AR Name "...ALL..." is special AR Name and includes all	*/
/*       remaining AR Names configured above.				*/
/************************************************************************/
static ST_VOID _ssc_App_Network_App_AR_Name_EFun (SX_DEC_CTRL *sxDecCtrl)
{
ST_RET             rtn;
SSC_STACK_CFG     *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;
SSC_AR_NAME       *sscArName;
SSC_APP_NETWORK   *appNetwork;
SSC_APP_AR_NAME   *sscAppArName;
ST_CHAR           *arName;
SXD_ATTR_PAIR     *arNameAttr;
ST_INT             len = 0;
ST_UINT            role = SSC_AR_NAME_ROLE_NONE;

  appNetwork = (SSC_APP_NETWORK *) list_find_last ((DBL_LNK *) sscStackCfg->appNetworkList);
  if (!appNetwork)
    {
    SSC_LOG_ERR0 ("SSC ERROR: 'App_Network.App_AR_Name' processing failed (appNetwork=NULL).");
    sxDecCtrl->errCode = SX_USER_ERROR;
    return;
    }

  /* we require to have a 'Role' attribute */
  if (sxDecCtrl->sxDecElInfo.attrCount != 1)
    {
    SSC_LOG_ERR0 ("SSC ERROR: 'App_Network.App_AR_Name' too many/few attributes\n"
                  "  (expecting only one attribute 'Role').");
    sxDecCtrl->errCode = SD_FAILURE;
    return;
    }
  arNameAttr = &sxDecCtrl->sxDecElInfo.attr[0];
  if (stricmp (arNameAttr->name, "Role") != 0)
    {
    SSC_LOG_ERR1 ("SSC ERROR: 'App_Network.App_AR_Name' invalid attribute '%s'\n"
                  "  (expecting 'Role').", arNameAttr->name);
    sxDecCtrl->errCode = SD_FAILURE;
    return;
    }
  if (strlen (arNameAttr->value) == 0)
    {
    SSC_LOG_ERR0 ("SSC ERROR: 'App_Network.App_AR_Name' value missing.");
    sxDecCtrl->errCode = SD_FAILURE;
    return;
    }
  if (stricmp (arNameAttr->value, "Local") == 0)
    role = SSC_AR_NAME_ROLE_LOCAL;
  else if (stricmp (arNameAttr->value, "Remote") == 0)
    role = SSC_AR_NAME_ROLE_REMOTE;
  else
    {
    SSC_LOG_ERR1 ("SSC ERROR: 'App_Network.App_AR_Name' invalid 'Role' attribute '%s'\n"
                  "  (expecting 'Local' or 'Remote').", arNameAttr->value);
    sxDecCtrl->errCode = SD_FAILURE;
    return;
    }

  rtn = sx_get_string_ptr (sxDecCtrl, &arName, &len);
  if (rtn != SD_SUCCESS)
    {
    SSC_LOG_ERR0 ("SSC ERROR: 'App_Network.App_AR_Name' failed to parse from XML.");
    return;
    }

    /* is this special AR Name to indicate all remaining configured AR Names? */
  /* DISABLE for now - maybe too complicated to sort the names */
  if (strcmp (arName, SSC_APP_AR_NAME_ALL) == 0)
    {
    if (role == SSC_AR_NAME_ROLE_REMOTE)
      {
      SSC_AR_NAME *sscArName;
      sscArName = sscStackCfg->arNameList;
      /* all remaining AR Names are to be added with Role=Remote	*/
      SSC_LOG_CFLOW0 ("SSC PARSE:      Adding ALL remaining AR Names as Remote");
      while (sscArName)
        {
        if (!sscFindAppArName (appNetwork, sscArName->arDib->name))
	  {
          sscAppArName = sscAddAppArName (appNetwork);
          if (sscAppArName)
            {
            SSC_LOG_CFLOW2 ("SSC PARSE:      'AR_Name' %6.6s      ='%s'",
                            arNameAttr->value, sscArName->arDib->name );
            strcpy (sscAppArName->arName, sscArName->arDib->name);
            sscAppArName->role = role;
            /* Note: in DIB_ENTRY, by default, the local=0 (meaning it is a remote AR Name) */
            }
          else
            {
            SSC_LOG_ERR1 ("SSC ERROR: add 'App_Network.App_AR_Name'='%s' failed.", sscArName->arDib->name);
            sxDecCtrl->errCode = SD_FAILURE;
	    break;
            }
	  }
        sscArName = list_get_next (sscStackCfg->arNameList, sscArName);
	}  /* end while */
      }
    else
      {
      SSC_LOG_ERR1 ("SSC PARSE:      'App_Network.App_AR_Name' '%s' valid only for Role=Remote", SSC_APP_AR_NAME_ALL);
      sxDecCtrl->errCode = SD_FAILURE;
      }
    return;	/* added ALL remaining AR Names woth Role=Remote */
    }

  if (len > 0 && len <= SSC_MAX_LEN_AR_NAME)
    {
    /* check for duplicates */
    if (!sscFindAppArName (appNetwork, arName))
      {
      /* check if AR Name configured */
       
      if ((sscArName = sscFindArName (sscStackCfg, arName)))
        {
        sscAppArName = sscAddAppArName (appNetwork);
        if (sscAppArName)
          {
          SSC_LOG_CFLOW2 ("SSC PARSE:      'AR_Name' %6.6s      ='%s'",
                          arNameAttr->value, arName );
          strcpy (sscAppArName->arName, arName);
          sscAppArName->role = role;
	  /* set also the role in DIB_ENTRY */
	  if (role == SSC_AR_NAME_ROLE_LOCAL)
            sscArName->arDib->local = 1;
          }
        else
          {
          SSC_LOG_ERR1 ("SSC ERROR: add 'App_Network.App_AR_Name'='%s' failed.", arName);
          sxDecCtrl->errCode = SD_FAILURE;
          }
        }
      else
        {
        SSC_LOG_ERR1 ("SSC ERROR: 'App_Network.App_AR_Name'='%s' not configured in AR_Name_List section", arName);
        sxDecCtrl->errCode = SX_USER_ERROR;
        }
      }
    else
      {
      SSC_LOG_ERR1 ("SSC ERROR: duplicate 'App_Network.App_AR_Name'='%s' found", arName);
      sxDecCtrl->errCode = SX_USER_ERROR;
      }
    }
  else
    {
    SSC_LOG_ERR2 ("SSC ERROR: 'App_Network.AR_Name' %s (max AR Name length is %u).",
                  len == 0 ? "not supplied" : "too long", SSC_MAX_LEN_AR_NAME);
    sxDecCtrl->errCode = SX_USER_ERROR;
    }
}

/************************************************************************/
/*		_ssc_App_Network_Max_Mms_Pdu_Length_EFun		*/
/************************************************************************/
static ST_VOID _ssc_App_Network_Max_Mms_Pdu_Length_EFun (SX_DEC_CTRL *sxDecCtrl)
{
ST_RET                   rtn;
SSC_STACK_CFG           *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;
SSC_APP_NETWORK         *appNetwork;

  appNetwork = (SSC_APP_NETWORK *) list_find_last ((DBL_LNK *) sscStackCfg->appNetworkList);
  if (!appNetwork)
    {
    SSC_LOG_ERR0 ("SSC ERROR: 'App_Network.Max_Mms_Pdu_Length' processing failed (appNetwork=NULL).");
    sxDecCtrl->errCode = SX_USER_ERROR;
    return;
    }

  rtn = sx_get_uint (sxDecCtrl, &appNetwork->maxMmsPduLength);
  if (rtn == SD_SUCCESS)
    SSC_LOG_CFLOW1 ("SSC PARSE:      'Max_Mms_Pdu_Length' = %u", appNetwork->maxMmsPduLength);
  else
    SSC_LOG_ERR0 ("SSC ERROR: 'App_Network.Max_Mms_Pdu_Length' failed to parse from XML.");
}

/************************************************************************/
/*		_ssc_App_Network_Max_Connections_EFun			*/
/* NOTE: used by MMS-EASE.						*/
/************************************************************************/
static ST_VOID _ssc_App_Network_Max_Connections_EFun (SX_DEC_CTRL *sxDecCtrl)
{
ST_RET                   rtn;
SSC_STACK_CFG           *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;
SSC_APP_NETWORK         *appNetwork;

  appNetwork = (SSC_APP_NETWORK *) list_find_last ((DBL_LNK *) sscStackCfg->appNetworkList);
  if (!appNetwork)
    {
    SSC_LOG_ERR0 ("SSC ERROR: 'App_Network.Max_Connections' processing failed (appNetwork=NULL).");
    sxDecCtrl->errCode = SX_USER_ERROR;
    return;
    }

  rtn = sx_get_uint (sxDecCtrl, &appNetwork->maxConnections);
  if (rtn == SD_SUCCESS)
    SSC_LOG_CFLOW1 ("SSC PARSE:      'Max_Connections' = %u", appNetwork->maxConnections);
  else
    SSC_LOG_ERR0 ("SSC ERROR: 'App_Network.Max_Connections' failed to parse from XML.");
}

/************************************************************************/
/*		_ssc_App_Network_Max_Calling_Connections_EFun		*/
/* NOTE: used by MMS-LITE.						*/
/************************************************************************/
static ST_VOID _ssc_App_Network_Max_Calling_Connections_EFun (SX_DEC_CTRL *sxDecCtrl)
{
ST_RET                   rtn;
SSC_STACK_CFG           *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;
SSC_APP_NETWORK         *appNetwork;

  appNetwork = (SSC_APP_NETWORK *) list_find_last ((DBL_LNK *) sscStackCfg->appNetworkList);
  if (!appNetwork)
    {
    SSC_LOG_ERR0 ("SSC ERROR: 'App_Network.Max_Calling_Connections' processing failed (appNetwork=NULL).");
    sxDecCtrl->errCode = SX_USER_ERROR;
    return;
    }

  rtn = sx_get_uint (sxDecCtrl, &appNetwork->maxCallingConns);
  if (rtn == SD_SUCCESS)
    SSC_LOG_CFLOW1 ("SSC PARSE:      'Max_Calling_Connections' = %u", appNetwork->maxCallingConns);
  else
    SSC_LOG_ERR0 ("SSC ERROR: 'App_Network.Max_Calling_Connections' failed to parse from XML.");
}

/************************************************************************/
/*		_ssc_App_Network_Max_Called_Connections_EFun		*/
/* NOTE: used by MMS-LITE.						*/
/************************************************************************/
static ST_VOID _ssc_App_Network_Max_Called_Connections_EFun (SX_DEC_CTRL *sxDecCtrl)
{
ST_RET                   rtn;
SSC_STACK_CFG           *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;
SSC_APP_NETWORK         *appNetwork;

  appNetwork = (SSC_APP_NETWORK *) list_find_last ((DBL_LNK *) sscStackCfg->appNetworkList);
  if (!appNetwork)
    {
    SSC_LOG_ERR0 ("SSC ERROR: 'App_Network.Max_Called_Connections' processing failed (appNetwork=NULL).");
    sxDecCtrl->errCode = SX_USER_ERROR;
    return;
    }

  rtn = sx_get_uint (sxDecCtrl, &appNetwork->maxCalledConns);
  if (rtn == SD_SUCCESS)
    SSC_LOG_CFLOW1 ("SSC PARSE:      'Max_Called_Connections' = %u", appNetwork->maxCalledConns);
  else
    SSC_LOG_ERR0 ("SSC ERROR: 'App_Network.Max_Called_Connections' failed to parse from XML.");
}

/************************************************************************/
/*		_ssc_App_Network_Sec_Event_Log_EFun			*/
/************************************************************************/
static ST_VOID _ssc_App_Network_Sec_Events_Log_File_EFun (SX_DEC_CTRL *sxDecCtrl)
{
ST_RET            rtn;
SSC_STACK_CFG    *sscStackCfg = (SSC_STACK_CFG *) sxDecCtrl->usr;
SXD_ATTR_PAIR    *attrPair;
SSC_APP_NETWORK  *appNetwork;
ST_UINT           i;

  appNetwork = (SSC_APP_NETWORK *) list_find_last ((DBL_LNK *) sscStackCfg->appNetworkList);
  if (!appNetwork)
    {
    SSC_LOG_ERR0 ("SSC ERROR: 'App_Network.Sec_Events_Log_File' processing failed (appNetwork=NULL).");
    sxDecCtrl->errCode = SX_USER_ERROR;
    return;
    }
  
  if (sxDecCtrl->sxDecElInfo.attrCount <= 1)
    {
    SSC_LOG_ERR1 ("SSC ERROR: 'App_Network.Sec_Events_Log_File' attributes missing\n"
                  "  in 'App_Network.App_Name'='%s' (expecting Enable, Name, and/or Size) ",
                  appNetwork->appName);
    sxDecCtrl->errCode = SX_USER_ERROR;
    return;
    }

/* DEBUG: we probably should allow Enable attribute anywhere ...
  TO-DO
 */
   
  attrPair = &sxDecCtrl->sxDecElInfo.attr[0];
  if (stricmp (attrPair->name, "Enable") != 0)
    {
    SSC_LOG_ERR1 ("SSC ERROR: 'App_Network.Sec_Events_Log_File': 'Enable' attribute missing \n"
                  "  in 'App_Network.App_Name'='%s' (expecting Enable, Name, and/or Size) ",
                  appNetwork->appName);
    sxDecCtrl->errCode = SD_FAILURE;
    return;
    }
  /* skip if disabled */
  if (stricmp (attrPair->value, "Yes") != 0)
    return;	/* not used */
  
  for (i = 1; i < sxDecCtrl->sxDecElInfo.attrCount; i++)
    {
    attrPair = &sxDecCtrl->sxDecElInfo.attr[i];
  
    if (strcmp (attrPair->name, "Name") == 0)
      {
      if (strlen (attrPair->value) < sizeof (appNetwork->secEventLogFileName))
        {
        strcpy (appNetwork->secEventLogFileName, attrPair->value);
        SSC_LOG_CFLOW1 ("SSC PARSE:      'Sec_Events_Log_File.Name' ='%s'", appNetwork->secEventLogFileName);
        }
      else
        {
        SSC_LOG_ERR1 ("SSC ERROR: 'Sec_Events_Log_File.Name' ='%s' is too long", attrPair->value);
        sxDecCtrl->errCode = SD_FAILURE;
        return;
        }
      }
    else if (strcmp (attrPair->name, "Size") == 0)
      {
      rtn = asciiToUlong (attrPair->value, &appNetwork->secEventLogFileSize);
      if (rtn != SD_SUCCESS)
        {
        appNetwork->secEventLogFileSize = 2000000;	/* set default */
        SSC_LOG_NERR2 ("SSC ERROR: 'App_Network.Sec_Events_Log_File' failed to parse attribute Size='%s'\n"
                       "  from XML in 'App_Network.App_Name'='%s'.", attrPair->value, appNetwork->appName);
        /* OK to continue */
        }
      SSC_LOG_CFLOW1 ("SSC PARSE:      'Sec_Events_Log_File.Size' ='%lu'", appNetwork->secEventLogFileSize);
      }
    else
      {
      SSC_LOG_ERR1 ("SSC ERROR: 'App_Network.Sec_Events_Log_File': invalid attribute '%s'\n"
                    " (expecting Enable, Name, and/or Size).", attrPair->name);
      sxDecCtrl->errCode = SD_FAILURE;
      return;
      }
    }
}

