/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,       */
/*          2000 - 2009, All Rights Reserved                            */
/*                                                                      */
/* MODULE NAME : osicfgx.c                                              */
/* PRODUCT(S)  :                                                        */
/*                                                                      */
/* MODULE DESCRIPTION : This routine parses the osicfg.xml              */
/*                      configuration file for the Lean-T Stack         */
/*                                                                      */
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :                            */
/*                                                                      */
/* MODIFICATION LOG :                                                   */
/*  Date     Who   Rev          Comments                                */
/* --------  ---  ------   -------------------------------------------  */
/* 03/30/11  JRB	   Added osicfgx_unconfigure.			*/
/* 07/22/10  JRB	   _transportType_EFun: chk init_dirser.. to	*/
/*			   avoid writing to invalid pointer on reconfig.*/
/*			   Move globals to local struct & initialize	*/
/*			   before each parse.				*/
/*			   Chg default tp_type to TCP.			*/
/*			   Set/clear rem_section_flag at start/end of	*/
/*			   "RemoteAddressList" section.			*/
/* 06/16/10  JRB	   Del ifdef TP4_ENABLED around CLNP params.	*/
/* 11/17/09  JRB     41    Fix for modified DIB_ENTRY ("name" is now	*/
/*			   array, not ptr). Fix some strlen checks.	*/
/*			   Fix incorrect format logging "port".		*/
/*			   Del MMSEASE_MOSI support.			*/
/* 02/18/08  JRB     40    Parse Network_Device.			*/
/* 02/07/08  JRB     39    Chg strtok to thread-safe get_next_string.	*/
/*			   Ignore obsolete Max_Num_Connections &	*/
/*			   Rfc1006_Max_Num_Conns.			*/
/* 01/12/07  JRB     38    If RemoteAddress NetAddr Type is illegal,	*/
/*			   ignore addr, log error, but continue parsing.*/
/* 07/08/05  EJV     37    MMSEASE_MOSI: allow only TCP tp_type.	*/
/*			   _netAddr_EFun: re-arranged NSAP with IPADDR	*/
/* 05/23/05  JRB     36    Del CFLOW logging (sx_dec logs similar info).*/
/* 05/18/05  DWL     35    Move "SX_PUSH (networkAddr..)" inside "if".	*/
/* 05/05/05  EJV     34    Added Rfc1006_Listen_Port, remote Port	*/
/* 04/06/05  JRB     33    MMSEASE_MOSI: set def tp_type to TP_TYPE_TCP	*/
/* 09/20/04  JRB     32    Always allow session_cfg.disconnect_timeout	*/
/*			   to be configured (not just for TP4).		*/
/* 08/13/04  JRB     31    Use thread-safe sx_parseExx_mt.		*/
/*			   Del unneeded system includes.		*/
/* 04/31/04  MDE     30    Now use sx_parseExx				*/
/* 02/02/04  JRB     29    Set err flag if IPADDR entered & !TP0_ENABLED*/
/*			   OR NSAP entered & !TP4_ENABLED. Clarify logs.*/
/* 01/14/04  EJV     28    Added rfc1006MaxSpduOutstanding.		*/
/* 12/09/03  JRB     27    Init all 5 members of SX_ELEMENT structs.	*/
/* 10/24/03  JRB     26	   Lint.					*/
/* 09/09/03  EJV     25	   Use the XML element names in ERR, FLOW logs.	*/
/* 12/16/02  ASK     24	   Wrap ifdef TP0_ENABLED around inet_ntoa calls*/
/* 07/18/02  JRB     23    Move ifdefs to skip more unneeded code.	*/
/* 06/27/02  JRB     22    Add tp0_sock.h.				*/
/* 06/20/02  JRB     21    Chg to use new netAddr union in PRES_ADDR.	*/
/* 05/03/02  MDE     20    Now set max_bind_ctrl			*/
/* 02/27/02  GLB     19    Changed ascii_t_.. to asciiTo..              */
/* 02/25/02  MDE     18    More changes to MVL_CFG_INFO			*/
/* 02/20/02  MDE     17    Changes for new MVL_CFG_INFO			*/
/* 02/11/02  JRB     16    Don't reparse w/ logging. Let user do it.	*/
/* 02/08/02  JRB     15    Add MVL_CFG.. arg & pass along to sx_parseEx.*/
/* 02/05/02  GLB     14    Made optional: Rfc1006_Max_Num_Conns,       	*/
/*                          LocalAddressList, RemoteAddressList         */
/* 01/22/02  JRB     13    Move mvl_set_num_connections call so it does	*/
/*			   not depend on order of params in XML file.	*/
/* 01/10/02  GLB     12    Update optional/mandatory flags              */
/*                         Add sx_pop to allow check for mandatory      */
/*                          elements                                    */
/*                         Make Session optional                        */
/* 11/28/01  MDE     11    Use new mvl_acse functions			*/
/* 11/28/01  MDE     10    Allocate mmsl_enc_buf			*/
/* 11/06/01  GLB     09    Added MMS section                            */
/* 05/25/01  GLB     08    Removed getFileSize                          */
/* 05/11/01  JRB     07    Use new sx_parseEx. Del osiCfgParse &        */
/*                         LOG_DEC_CTRL (not used for anything useful). */
/*                         Save "called" flag to use on 2nd pass so same*/
/*                         data parsed on 2nd pass. Chg MVL_LOG to      */
/*                         SXLOG, mvl log mask not initialized yet.     */
/*                         Use correct defines for SSEL TSEL len.       */
/* 03/15/01  JRB     06    Added Session group w/ Disconnect_Timeout.   */
/* 01/25/01  JRB     05    Use stricmp.                                 */
/*                         Chg sx_deb.. again: do NOT set SX_LOG_DEBUG. */
/* 01/15/01  EJV     04    Chnaged sx_debug_sel in cfg parsing.         */
/* 01/09/01  GLB     03    Changed logBufParse to osiCfgParse           */
/* 01/03/01  EJV     02    Don't use O_TEXT in open() on non _WIN32 sys.*/
/*                         Changed _getFileSize to use stat().          */
/*                         Changed sx_debug_sel to 'or' flags.          */
/*                         Deleted duplicate code in _netAddr_EFun.     */
/* 08/31/00  GLB     01    Module created from existing lean_cfg.c      */
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "mem_chk.h"
#include "mvl_acse.h"

#include "sx_defs.h"
#include "sx_log.h"

#include "str_util.h"
#include "slog.h"
#include "acse2log.h"
#include "lean_a.h"
#include "tp0_sock.h"	/* portable "sockets" interface	*/

#include "mmsldefs.h"      /* for MMS section */
#include "mvl_defs.h"		   /* for MMS section */

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of      */
/* __FILE__ strings.                                                    */

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/
#define REMOTE_DIB_TABLE 0
#define LOCAL_DIB_TABLE  1

#define CFG_MAX_AP_TITLE    80
#define CFG_MAX_AE_QUAL     32

typedef struct _tagTransDevInfo
  {
  DBL_LNK l;
  ST_CHAR name[MAX_AR_LEN +1];	/* must be same size as "name" in DIB_ENTRY*/
  ST_CHAR apTitle[CFG_MAX_AP_TITLE +1];
  ST_CHAR aeQual[CFG_MAX_AE_QUAL +1];
  PRES_ADDR pres_addr;
  } TRANS_DEV_INFO;

typedef struct
  {
  TRANS_DEV_INFO *localTransList;
  TRANS_DEV_INFO *remoteTransList;
  TRANS_DEV_INFO *currTransEl;
  MVL_CFG_INFO *mvlCfg;	/* copy osicfgx arg here to pass to parser.	*/
  ST_BOOLEAN rem_section_flag;	/* SD_TRUE if in remote address section	*/
  } SX_USR_INFO;

static ST_BOOLEAN init_dirser_called;

/************************************************************************/

static ST_VOID _logStart_SEFun (SX_DEC_CTRL *sxDecCtrl);

static ST_VOID _mms_SFun (SX_DEC_CTRL *sxDecCtrl);

static ST_VOID _maxMmsPduLength_EFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _maxCallingConnections_EFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _maxCalledConnections_EFun (SX_DEC_CTRL *sxDecCtrl);

static ST_VOID _network_SFun (SX_DEC_CTRL *sxDecCtrl);

static ST_VOID _networkDevice_EFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _clnp_SFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _lifetime_EFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _lifetimeDecrement_EFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _cfgTimer_EFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _eshDelay_EFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _localNsap_EFun (SX_DEC_CTRL *sxDecCtrl);

static ST_VOID _transport_SFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _session_SFun (SX_DEC_CTRL *sxDecCtrl);

static ST_VOID _tp4_SFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _maxTpduLength_EFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _maxRemoteCdt_EFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _localCdt_EFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _maxNumConnections_EFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _maxSpduOutstanding_EFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _windowTime_EFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _inactivityTime_EFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _retransmissionTime_EFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _maxTransmissions_EFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _akDelay_EFun (SX_DEC_CTRL *sxDecCtrl);

static ST_VOID _tcp_SFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _rfc1006MaxTpduLen_EFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _rfc1006MaxNumConns_EFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _rfc1006MaxSpduOutstanding_EFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _rfc1006ListenPort_EFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _disconnectTimeout_EFun (SX_DEC_CTRL *sxDecCtrl);

static ST_VOID _networkAddressing_SFun (SX_DEC_CTRL *sxDecCtrl);

static ST_VOID _localAddresses_SFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _remoteAddresses_SFun (SX_DEC_CTRL *sxDecCtrl);

static ST_VOID _localArName_SFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _arname_EFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _apTitle_EFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _aeQualifier_EFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _psel_EFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _ssel_EFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _tsel_EFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _transportType_EFun (SX_DEC_CTRL *sxDecCtrl);

static ST_VOID _remoteArName_SFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _netAddr_EFun (SX_DEC_CTRL *sxDecCtrl);
static ST_VOID _port_EFun (SX_DEC_CTRL *sxDecCtrl);

static ST_RET save_loc_rem_dib_tables (SX_USR_INFO *usr);
static ST_VOID save_dib_table (TRANS_DEV_INFO *, DIB_ENTRY *);
static ST_RET parseObjId (MMS_OBJ_ID *o, ST_CHAR *buff);

static ST_VOID printDibEntry (ST_VOID);     
static ST_VOID printDibStruct (DIB_ENTRY *de,ST_INT table_type);    

/************************************************************************/
/************************************************************************/
/************************************************************************/

SX_ELEMENT cfgStartElements[] = 
{
  {"STACK_CFG", SX_ELF_CSTART | SX_ELF_CEND,               _logStart_SEFun, NULL, 0}
};

SX_ELEMENT cfgElements[] = 
{
  {"MMS",                     SX_ELF_CSTART|SX_ELF_OPT,    _mms_SFun, NULL, 0},
  {"Network",                 SX_ELF_CSTART|SX_ELF_OPT,    _network_SFun, NULL, 0},
  {"Transport",               SX_ELF_CSTART,               _transport_SFun, NULL, 0},
  {"Session",                 SX_ELF_CSTART|SX_ELF_OPT,    _session_SFun, NULL, 0},
  {"NetworkAddressing",       SX_ELF_CSTARTEND,            _networkAddressing_SFun, NULL, 0}
};

SX_ELEMENT mmsElements[] = 
{
  {"Max_Mms_Pdu_Length",      SX_ELF_CEND,                 _maxMmsPduLength_EFun, NULL, 0},
  {"Max_Calling_Connections", SX_ELF_CEND,                 _maxCallingConnections_EFun, NULL, 0},
  {"Max_Called_Connections",  SX_ELF_CEND,                 _maxCalledConnections_EFun, NULL, 0},
};

SX_ELEMENT networkElements[] = 
{
  {"Network_Device",          SX_ELF_CEND|SX_ELF_OPT,      _networkDevice_EFun, NULL, 0},
  {"Clnp",                    SX_ELF_CSTART|SX_ELF_OPT,    _clnp_SFun, NULL, 0}
};

SX_ELEMENT clnpElements[] = 
{
  {"Lifetime",                SX_ELF_CEND|SX_ELF_OPT,      _lifetime_EFun, NULL, 0},
  {"Lifetime_Decrement",      SX_ELF_CEND|SX_ELF_OPT,      _lifetimeDecrement_EFun, NULL, 0},
  {"Cfg_Timer",               SX_ELF_CEND|SX_ELF_OPT,      _cfgTimer_EFun, NULL, 0},
  {"Esh_Delay",               SX_ELF_CEND|SX_ELF_OPT,      _eshDelay_EFun, NULL, 0},
  {"Local_NSAP",              SX_ELF_CEND|SX_ELF_OPT,      _localNsap_EFun, NULL, 0}
};


SX_ELEMENT transportElements[] = 
{
  {"Tp4",                     SX_ELF_CSTART|SX_ELF_OPT,    _tp4_SFun, NULL, 0},
  {"Tcp",                     SX_ELF_CSTART|SX_ELF_OPT,    _tcp_SFun, NULL, 0}
};

SX_ELEMENT Tp4Elements[] = 
{
  {"Max_Tpdu_Length",         SX_ELF_CEND|SX_ELF_OPT,      _maxTpduLength_EFun, NULL, 0},
  {"Max_Remote_Cdt",          SX_ELF_CEND|SX_ELF_OPT,      _maxRemoteCdt_EFun, NULL, 0},
  {"Local_Cdt",               SX_ELF_CEND|SX_ELF_OPT,      _localCdt_EFun, NULL, 0},
  {"Max_Num_Connections",     SX_ELF_CEND|SX_ELF_OPT,      _maxNumConnections_EFun, NULL, 0},
  {"Max_Spdu_Outstanding",    SX_ELF_CEND|SX_ELF_OPT,      _maxSpduOutstanding_EFun, NULL, 0},
  {"Window_Time",             SX_ELF_CEND|SX_ELF_OPT,      _windowTime_EFun, NULL, 0},
  {"Inactivity_Time",         SX_ELF_CEND|SX_ELF_OPT,      _inactivityTime_EFun, NULL, 0},
  {"Retransmission_Time",     SX_ELF_CEND|SX_ELF_OPT,      _retransmissionTime_EFun, NULL, 0},
  {"Max_Transmissions",       SX_ELF_CEND|SX_ELF_OPT,      _maxTransmissions_EFun, NULL, 0},
  {"Ak_Delay",                SX_ELF_CEND|SX_ELF_OPT,      _akDelay_EFun, NULL, 0}
};

SX_ELEMENT TcpElements[] = 
{
  {"Rfc1006_Max_Tpdu_Len",    SX_ELF_CEND|SX_ELF_OPT,      _rfc1006MaxTpduLen_EFun, NULL, 0},
  {"Rfc1006_Max_Num_Conns",   SX_ELF_CEND|SX_ELF_OPT,      _rfc1006MaxNumConns_EFun, NULL, 0},
  {"Rfc1006_Max_Spdu_Outstanding", SX_ELF_CEND|SX_ELF_OPT, _rfc1006MaxSpduOutstanding_EFun, NULL, 0},
  {"Rfc1006_Listen_Port",     SX_ELF_CEND|SX_ELF_OPT,      _rfc1006ListenPort_EFun, NULL, 0}
};


SX_ELEMENT sessionElements[] = 
{
  {"Disconnect_Timeout",      SX_ELF_CEND|SX_ELF_OPT,      _disconnectTimeout_EFun, NULL, 0}
};

SX_ELEMENT networkAddressingElements[] = 
{
  {"LocalAddressList",        SX_ELF_CSTART|SX_ELF_OPT,    _localAddresses_SFun, NULL, 0},
  {"RemoteAddressList",       SX_ELF_CSTARTEND|SX_ELF_OPT, _remoteAddresses_SFun, NULL, 0}
};

SX_ELEMENT LocalAddressesElements[] = 
{
  {"LocalAddress",            SX_ELF_CSTARTEND|SX_ELF_RPT, _localArName_SFun, NULL, 0}
};

SX_ELEMENT LocalArNameElements[] = 
{
  {"AR_Name",                 SX_ELF_CEND,                 _arname_EFun, NULL, 0},
  {"AP_Title",                SX_ELF_CEND|SX_ELF_OPT,      _apTitle_EFun, NULL, 0},
  {"AE_Qualifier",            SX_ELF_CEND|SX_ELF_OPT,      _aeQualifier_EFun, NULL, 0},
  {"Psel",                    SX_ELF_CEND,                 _psel_EFun, NULL, 0},
  {"Ssel",                    SX_ELF_CEND,                 _ssel_EFun, NULL, 0},
  {"Tsel",                    SX_ELF_CEND,                 _tsel_EFun, NULL, 0},
  {"TransportType",           SX_ELF_CEND,                 _transportType_EFun, NULL, 0}
};

SX_ELEMENT RemoteAddressesElements[] = 
{
  {"RemoteAddress",           SX_ELF_CSTARTEND|SX_ELF_RPT, _remoteArName_SFun, NULL, 0}
};

SX_ELEMENT RemoteArNameElements[] = 
{
  {"AR_Name",                 SX_ELF_CEND,                 _arname_EFun, NULL, 0},
  {"AP_Title",                SX_ELF_CEND|SX_ELF_OPT,      _apTitle_EFun, NULL, 0},
  {"AE_Qualifier",            SX_ELF_CEND|SX_ELF_OPT,      _aeQualifier_EFun, NULL, 0},
  {"Psel",                    SX_ELF_CEND,                 _psel_EFun, NULL, 0},
  {"Ssel",                    SX_ELF_CEND,                 _ssel_EFun, NULL, 0},
  {"Tsel",                    SX_ELF_CEND,                 _tsel_EFun, NULL, 0},
  {"NetAddr",                 SX_ELF_CEND,                 _netAddr_EFun, NULL, 0},
  {"Port",                    SX_ELF_CEND|SX_ELF_OPT,      _port_EFun, NULL, 0}
};

/************************************************************************/
/*                                   osicfgx                            */
/************************************************************************/

ST_RET osicfgx (ST_CHAR *logFileName, MVL_CFG_INFO *mvlCfg)
  {
ST_RET rc;
SX_USR_INFO usr;

  /* Initialize info to pass to parser.	*/
  memset (&usr, 0, sizeof (SX_USR_INFO));	/* CRITICAL: start clean*/
  usr.mvlCfg = mvlCfg;

  memset (mvlCfg, 0, sizeof (MVL_CFG_INFO));	/* CRITICAL: start clean*/

  rc = sx_parseExx_mt (logFileName, 
          sizeof (cfgStartElements)/sizeof(SX_ELEMENT), cfgStartElements,
          &usr, NULL, NULL);

 /* Here we save the bind information */
  if (loc_dib_table != NULL)
    {
    strcpy (mvlCfg->local_ar_name, loc_dib_table->name);
    }

  if (rc == SD_SUCCESS)
    init_dirser_called = SD_TRUE;

  return (rc);
  }

/************************************************************************/
/************************************************************************/
/*                               _logStart_SEFun                        */
/************************************************************************/

static ST_VOID _logStart_SEFun (SX_DEC_CTRL *sxDecCtrl)
  {
  if (sxDecCtrl->reason == SX_ELEMENT_START)
    {
    SX_PUSH (cfgElements);
    }
  else
    {
    while (sxDecCtrl->itemStackLevel > 0)
      sx_pop (sxDecCtrl);
    }
  }
/************************************************************************/
/*                                 _mms_SFun                            */
/************************************************************************/

static ST_VOID _mms_SFun (SX_DEC_CTRL *sxDecCtrl)
  {
  SX_PUSH (mmsElements);
  }

/************************************************************************/
/*                        _maxMmsPduLength_EFun                         */
/************************************************************************/

static ST_VOID _maxMmsPduLength_EFun (SX_DEC_CTRL *sxDecCtrl)
  {
ST_BOOLEAN rc;
ST_CHAR *str;
ST_INT strLen;
SX_USR_INFO *usr = (SX_USR_INFO *) sxDecCtrl->usr;

ST_INT sInt;

  rc = sx_get_string_ptr (sxDecCtrl, &str, &strLen);
  if (rc != SD_SUCCESS)
    return;

  if (!init_dirser_called)
    {

    sInt = 0;
    if (strlen(str) == 0)
      {
      SXLOG_ERR0 ("ERROR: Max_Mms_Pdu_Length required but not present ");
      sxDecCtrl->errCode = SX_USER_ERROR;
      }
    else if ((asciiToSint(str,&sInt) != SD_SUCCESS) || (strchr(str,'-')) )
      {
      SXLOG_ERR1 ("ERROR: Max_Mms_Pdu_Length Data Conversion Error on value: %s", str);
      sxDecCtrl->errCode = SX_USER_ERROR;
      }
    else						 
      usr->mvlCfg->max_msg_size = sInt;
    }
  }
  
/************************************************************************/
/*                      _maxCallingConnections_EFun                     */
/************************************************************************/

static ST_VOID _maxCallingConnections_EFun (SX_DEC_CTRL *sxDecCtrl)
  {
ST_BOOLEAN rc;
ST_CHAR *str;
ST_INT strLen;
ST_INT sInt;
SX_USR_INFO *usr = (SX_USR_INFO *) sxDecCtrl->usr;

  rc = sx_get_string_ptr (sxDecCtrl, &str, &strLen);
  if (rc != SD_SUCCESS)
    return;

  if (!init_dirser_called)
    {

    sInt = 0;

    if (strlen(str) == 0)
      {
      SXLOG_ERR0 ("ERROR: Max_Calling_Connections required but not present ");
      sxDecCtrl->errCode = SX_USER_ERROR;
      }
    else if ((asciiToSint(str,&sInt) != SD_SUCCESS) || (strchr(str,'-')) )
      {
      SXLOG_ERR1 ("ERROR: Max_Calling_Connections Data Conversion Error on value: %s", str);
      sxDecCtrl->errCode = SX_USER_ERROR;
      }
    else						 
      { 
      usr->mvlCfg->num_calling = sInt;
      }

    }
  }
  
/************************************************************************/
/*                    _maxCalledConnections_EFun                        */
/************************************************************************/

static ST_VOID _maxCalledConnections_EFun (SX_DEC_CTRL *sxDecCtrl)
  {
ST_BOOLEAN rc;
ST_CHAR *str;
ST_INT strLen;
ST_INT sInt;
SX_USR_INFO *usr = (SX_USR_INFO *) sxDecCtrl->usr;

  rc = sx_get_string_ptr (sxDecCtrl, &str, &strLen);
  if (rc != SD_SUCCESS)
    return;

  if (!init_dirser_called)
    {
    sInt = 0;
    if (strlen(str) == 0)
      {
      SXLOG_ERR0 ("ERROR: Max_Called_Connections required but not present ");
      sxDecCtrl->errCode = SX_USER_ERROR;
      }
    else if ((asciiToSint(str,&sInt) != SD_SUCCESS) || (strchr(str,'-')) )
      {
      SXLOG_ERR1 ("ERROR: Max_Called_Connections Data Conversion Error on value: %s", str);
      sxDecCtrl->errCode = SX_USER_ERROR;
      }
    else						 
      { 
      usr->mvlCfg->num_called = sInt;
      }
    }
  }
  
/************************************************************************/
/************************************************************************/
/*                               _network_SFun                          */
/************************************************************************/

static ST_VOID _network_SFun (SX_DEC_CTRL *sxDecCtrl)
  {
  SX_PUSH (networkElements);
  }

/************************************************************************/
/*				_networkDevice_EFun			*/
/************************************************************************/
static ST_VOID _networkDevice_EFun (SX_DEC_CTRL *sxDecCtrl)
  {
ST_BOOLEAN rc;
ST_CHAR *str;
ST_INT strLen;

  rc = sx_get_string_ptr (sxDecCtrl, &str, &strLen);
  if (rc != SD_SUCCESS)
    return;

  if (!init_dirser_called)
    {
    clnp_param.network_device = chk_malloc (strLen + 1);
    strcpy (clnp_param.network_device, str);
    }
  }

/************************************************************************/
/************************************************************************/
/*                                    _clnp_SFun                        */
/************************************************************************/

static ST_VOID _clnp_SFun (SX_DEC_CTRL *sxDecCtrl)
  {
  SX_PUSH (clnpElements);
  }

/************************************************************************/
/*                          _lifetime_EFun                              */
/************************************************************************/

static ST_VOID _lifetime_EFun (SX_DEC_CTRL *sxDecCtrl)
  {
ST_BOOLEAN rc;
ST_CHAR *str;
ST_INT strLen;

ST_UINT16 uShortInt = 0;

  rc = sx_get_string_ptr (sxDecCtrl, &str, &strLen);
  if (rc != SD_SUCCESS)
    return;

  if (!init_dirser_called)
    {
    if (asciiToUint16(str,&uShortInt) != SD_SUCCESS)
      {
      SXLOG_ERR0 ("ERROR: Lifetime Data Conversion Error");
      sxDecCtrl->errCode = SX_ERR_CONVERT;
      }
    else
      {
      clnp_param.pdu_lifetime = (ST_UCHAR) uShortInt;
      }
    }
  }
  
/************************************************************************/
/*                       _lifetimeDecrement_EFun                        */
/************************************************************************/

static ST_VOID _lifetimeDecrement_EFun (SX_DEC_CTRL *sxDecCtrl)
  {
ST_BOOLEAN rc;
ST_CHAR *str;
ST_INT strLen;

ST_UINT16 uShortInt = 0;

  rc = sx_get_string_ptr (sxDecCtrl, &str, &strLen);
  if (rc != SD_SUCCESS)
    return;

  if (!init_dirser_called)
    {
    if (asciiToUint16(str,&uShortInt) != SD_SUCCESS)
      {
      SXLOG_ERR0 ("ERROR: Lifetime_Decrement Data Conversion Error");
      sxDecCtrl->errCode = SX_ERR_CONVERT;
      }
    else
      {
      clnp_param.pdu_lifetime_dec = (ST_UCHAR) uShortInt;
      }
    }
  }

/************************************************************************/
/*                             _cfgTimer_EFun                           */
/************************************************************************/

static ST_VOID _cfgTimer_EFun (SX_DEC_CTRL *sxDecCtrl)
  {
ST_BOOLEAN rc;
ST_CHAR *str;
ST_INT strLen;

ST_UINT16 uShortInt = 0;

  rc = sx_get_string_ptr (sxDecCtrl, &str, &strLen);
  if (rc != SD_SUCCESS)
    return;

  if (!init_dirser_called)
    {
    if (asciiToUint16(str,&uShortInt) != SD_SUCCESS)
      {
      SXLOG_ERR0 ("ERROR: Cfg_Timer Data Conversion Error");
      sxDecCtrl->errCode = SX_ERR_CONVERT;
      }
    else
      {
      clnp_param.esh_cfg_timer = uShortInt;
      }
    }
  }

/************************************************************************/
/*                             _eshDelay_EFun                           */
/************************************************************************/

static ST_VOID _eshDelay_EFun (SX_DEC_CTRL *sxDecCtrl)
  {
ST_BOOLEAN rc;
ST_CHAR *str;
ST_INT strLen;

ST_UINT16 uShortInt = 0;

  rc = sx_get_string_ptr (sxDecCtrl, &str, &strLen);
  if (rc != SD_SUCCESS)
    return;

  if (!init_dirser_called)
    {
    if (asciiToUint16(str,&uShortInt) != SD_SUCCESS)
      {
      SXLOG_ERR0 ("ERROR: Esh_Delay Data Conversion Error");
      sxDecCtrl->errCode = SX_ERR_CONVERT;
      }
    else
      {
      clnp_param.esh_delay = uShortInt;
      }
    }
  }

/************************************************************************/
/*                              _localNsap_EFun                         */
/************************************************************************/

static ST_VOID _localNsap_EFun (SX_DEC_CTRL *sxDecCtrl)
  {
ST_BOOLEAN rc;
ST_CHAR *str;
ST_INT strLen;
ST_UINT hexStrLen = 0;

  rc = sx_get_string_ptr (sxDecCtrl, &str, &strLen);
  if (rc != SD_SUCCESS)
    return;

  if (!init_dirser_called)
    {
    if (ascii_to_hex_str (&clnp_param.loc_nsap[1],&hexStrLen,CLNP_MAX_LEN_NSAP,str)
        == SD_SUCCESS)
      {
      if (hexStrLen > 0)
        clnp_param.loc_nsap[0] = (ST_UCHAR) hexStrLen;
      }
    else
      {
      /*cfg_set_config_err();*/
      SXLOG_ERR0 ("ERROR: Clnp Local_NSAP Data Conversion Error");
      sxDecCtrl->errCode = SX_ERR_CONVERT;
      }
    }
  }

/************************************************************************/
/************************************************************************/
/*                              _transport_SFun                         */
/************************************************************************/

static ST_VOID _transport_SFun (SX_DEC_CTRL *sxDecCtrl)
  {
  SX_PUSH (transportElements);
  }

/************************************************************************/
/************************************************************************/
/*                               _tp4_SFun                              */
/************************************************************************/

static ST_VOID _tp4_SFun (SX_DEC_CTRL *sxDecCtrl)
  {
  SX_PUSH (Tp4Elements);
  }

/************************************************************************/
/*                           _maxTpduLength_EFun                        */
/************************************************************************/

static ST_VOID _maxTpduLength_EFun (SX_DEC_CTRL *sxDecCtrl)
  {
#if defined (TP4_ENABLED)   /* Just ignore if disabled.     */
ST_BOOLEAN rc;
ST_CHAR *str;
ST_INT strLen;

ST_UINT16 uShortInt = 0;

  rc = sx_get_string_ptr (sxDecCtrl, &str, &strLen);
  if (rc != SD_SUCCESS)
    return;

  if (!init_dirser_called)
    {
    if (asciiToUint16(str,&uShortInt) != SD_SUCCESS)
      {
      SXLOG_ERR0 ("ERROR: Max_Tpdu_Length Data Conversion Error");
      sxDecCtrl->errCode = SX_ERR_CONVERT;
      }
    else
      {
      tp_cfg.max_tpdu_len = uShortInt;
      }
    }
#endif  /* TP4_ENABLED  */
  }

/************************************************************************/
/*                           _maxRemoteCdt_EFun                         */
/************************************************************************/

static ST_VOID _maxRemoteCdt_EFun (SX_DEC_CTRL *sxDecCtrl)
  {
#if defined (TP4_ENABLED)   /* Just ignore if disabled.     */
ST_BOOLEAN rc;
ST_CHAR *str;
ST_INT strLen;

ST_UINT16 uShortInt = 0;

  rc = sx_get_string_ptr (sxDecCtrl, &str, &strLen);
  if (rc != SD_SUCCESS)
    return;

  if (!init_dirser_called)
    {
    if (asciiToUint16(str,&uShortInt) != SD_SUCCESS)
      {
      SXLOG_ERR0 ("ERROR: Max_Remote_Cdt Data Conversion Error");
      sxDecCtrl->errCode = SX_ERR_CONVERT;
      }
    else
      {
      tp_cfg.max_rem_cdt = (ST_UCHAR) uShortInt;
      }
    }
#endif  /* TP4_ENABLED  */
  }

/************************************************************************/
/*                             _localCdt_EFun                           */
/************************************************************************/

static ST_VOID _localCdt_EFun (SX_DEC_CTRL *sxDecCtrl)
  {
#if defined (TP4_ENABLED)   /* Just ignore if disabled.     */
ST_BOOLEAN rc;
ST_CHAR *str;
ST_INT strLen;

ST_UINT16 uShortInt = 0;

  rc = sx_get_string_ptr (sxDecCtrl, &str, &strLen);
  if (rc != SD_SUCCESS)
    return;

  if (!init_dirser_called)
    {
    if (asciiToUint16(str,&uShortInt) != SD_SUCCESS)
      {
      SXLOG_ERR0 ("ERROR: Local_Cdt Data Conversion Error");
      sxDecCtrl->errCode = SX_ERR_CONVERT;
      }
    else
      {
      tp_cfg.loc_cdt = (ST_UCHAR) uShortInt;
      }
    }
#endif  /* TP4_ENABLED  */
  }

/************************************************************************/
/*                        _maxNumConnections_EFun                       */
/************************************************************************/

static ST_VOID _maxNumConnections_EFun (SX_DEC_CTRL *sxDecCtrl)
  {
  SXLOG_ERR0 ("Max_Num_Connections element is OBSOLETE. Ignored.");
  return;
  }

/************************************************************************/
/*                         _maxSpduOutstanding_EFun                     */
/************************************************************************/

static ST_VOID _maxSpduOutstanding_EFun (SX_DEC_CTRL *sxDecCtrl)
  {
#if defined (TP4_ENABLED)   /* Just ignore if disabled.     */
ST_BOOLEAN rc;
ST_CHAR *str;
ST_INT strLen;

ST_UINT16 uShortInt = 0;

  rc = sx_get_string_ptr (sxDecCtrl, &str, &strLen);
  if (rc != SD_SUCCESS)
    return;

  if (!init_dirser_called)
    {
    if (asciiToUint16(str,&uShortInt) != SD_SUCCESS)
      {
      SXLOG_ERR0 ("ERROR: Max_Spdu_Outstanding Data Conversion Error");
      sxDecCtrl->errCode = SX_ERR_CONVERT;
      }
    else
      {
      tp_cfg.max_spdu_outst = (ST_UCHAR) uShortInt;
      }
    }
#endif  /* TP4_ENABLED  */
  }

/************************************************************************/
/*                            _windowTime_EFun                          */
/************************************************************************/

static ST_VOID _windowTime_EFun (SX_DEC_CTRL *sxDecCtrl)
  {
#if defined (TP4_ENABLED)   /* Just ignore if disabled.     */
ST_BOOLEAN rc;
ST_CHAR *str;
ST_INT strLen;

ST_UINT16 uShortInt = 0;

  rc = sx_get_string_ptr (sxDecCtrl, &str, &strLen);
  if (rc != SD_SUCCESS)
    return;

  if (!init_dirser_called)
    {
    if (asciiToUint16(str,&uShortInt) != SD_SUCCESS)
      {
      SXLOG_ERR0 ("ERROR: Window_Time Data Conversion Error");
      sxDecCtrl->errCode = SX_ERR_CONVERT;
      }
    else
      {
      tp_cfg.window_time = uShortInt;
      }
    }
#endif  /* TP4_ENABLED  */
  }

/************************************************************************/
/*                            _inactivityTime_EFun                      */
/************************************************************************/

static ST_VOID _inactivityTime_EFun (SX_DEC_CTRL *sxDecCtrl)
  {
#if defined (TP4_ENABLED)   /* Just ignore if disabled.     */
ST_BOOLEAN rc;
ST_CHAR *str;
ST_INT strLen;

ST_UINT16 uShortInt = 0;

  rc = sx_get_string_ptr (sxDecCtrl, &str, &strLen);
  if (rc != SD_SUCCESS)
    return;

  if (!init_dirser_called)
    {
    if (asciiToUint16(str,&uShortInt) != SD_SUCCESS)
      {
      SXLOG_ERR0 ("ERROR: Inactivity_Time Data Conversion Error");
      sxDecCtrl->errCode = SX_ERR_CONVERT;
      }
    else
      {
      tp_cfg.inact_time = uShortInt;
      }
    }
#endif  /* TP4_ENABLED  */
  }

/************************************************************************/
/*                          _retransmissionTime_EFun                    */
/************************************************************************/

static ST_VOID _retransmissionTime_EFun (SX_DEC_CTRL *sxDecCtrl)
  {
#if defined (TP4_ENABLED)   /* Just ignore if disabled.     */
ST_BOOLEAN rc;
ST_CHAR *str;
ST_INT strLen;

ST_UINT16 uShortInt = 0;

  rc = sx_get_string_ptr (sxDecCtrl, &str, &strLen);
  if (rc != SD_SUCCESS)
    return;

  if (!init_dirser_called)
    {
    if (asciiToUint16(str,&uShortInt) != SD_SUCCESS)
      {
      SXLOG_ERR0 ("ERROR: Retransmission_Time Data Conversion Error");
      sxDecCtrl->errCode = SX_ERR_CONVERT;
      }
    else
      {
      tp_cfg.retrans_time = uShortInt;
      }
    }
#endif  /* TP4_ENABLED  */
  }

/************************************************************************/
/*                          _maxTransmissions_EFun                      */
/************************************************************************/

static ST_VOID _maxTransmissions_EFun (SX_DEC_CTRL *sxDecCtrl)
  {
#if defined (TP4_ENABLED)   /* Just ignore if disabled.     */
ST_BOOLEAN rc;
ST_CHAR *str;
ST_INT strLen;

ST_UINT16 uShortInt = 0;

  rc = sx_get_string_ptr (sxDecCtrl, &str, &strLen);
  if (rc != SD_SUCCESS)
    return;

  if (!init_dirser_called)
    {
    if (asciiToUint16(str,&uShortInt) != SD_SUCCESS)
      {
      SXLOG_ERR0 ("ERROR: Max_Transmissions Data Conversion Error");
      sxDecCtrl->errCode = SX_ERR_CONVERT;
      }
    else
      {
      tp_cfg.max_trans = (ST_UCHAR) uShortInt;
      }
    }
#endif  /* TP4_ENABLED  */
  }

/************************************************************************/
/*                             _akDelay_EFun                            */
/************************************************************************/

static ST_VOID _akDelay_EFun (SX_DEC_CTRL *sxDecCtrl)
  {
#if defined (TP4_ENABLED)   /* Just ignore if disabled.     */
ST_BOOLEAN rc;
ST_CHAR *str;
ST_INT strLen;

ST_UINT16 uShortInt = 0;

  rc = sx_get_string_ptr (sxDecCtrl, &str, &strLen);
  if (rc != SD_SUCCESS)
    return;

  if (!init_dirser_called)
    {
    if (asciiToUint16(str,&uShortInt) != SD_SUCCESS)
      {
      SXLOG_ERR0 ("ERROR: Ak_Delay Data Conversion Error");
      sxDecCtrl->errCode = SX_ERR_CONVERT;
      }
    else
      {
      tp_cfg.ak_delay = (ST_UCHAR) uShortInt;
      }
    }
#endif  /* TP4_ENABLED  */
  }

/************************************************************************/
/************************************************************************/
/*                              _tcp_SFun                               */
/************************************************************************/

static ST_VOID _tcp_SFun (SX_DEC_CTRL *sxDecCtrl)
  {
  SX_PUSH (TcpElements);
  }

/************************************************************************/
/*                         _rfc1006MaxTpduLen_EFun                      */
/************************************************************************/

static ST_VOID _rfc1006MaxTpduLen_EFun (SX_DEC_CTRL *sxDecCtrl)
  {
ST_BOOLEAN rc;
ST_CHAR *str;
ST_INT strLen;

  rc = sx_get_string_ptr (sxDecCtrl, &str, &strLen);
  if (rc != SD_SUCCESS)
    return;

#if defined (TP0_ENABLED)   /* Just ignore if disabled.     */
  if (!init_dirser_called)
    {
    ST_UINT16 uShortInt = 0;

    if (asciiToUint16(str,&uShortInt) != SD_SUCCESS)
      {
      SXLOG_ERR0 ("ERROR: Rfc1006_Max_Tpdu_Len Data Conversion Error");
      sxDecCtrl->errCode = SX_ERR_CONVERT;
      }
    else
      {
      tp0_cfg.max_tpdu_len = uShortInt;
      }
    }
#endif  /* TP0_ENABLED  */
  }

/************************************************************************/
/*                         _rfc1006MaxNumConns_EFun                     */
/************************************************************************/

static ST_VOID _rfc1006MaxNumConns_EFun (SX_DEC_CTRL *sxDecCtrl)
  {
  SXLOG_ERR0 ("Rfc1006_Max_Num_Conns element is OBSOLETE. Ignored.");
  return;
  }

/************************************************************************/
/*                         _rfc1006MaxSpduOutstanding_EFun              */
/************************************************************************/

static ST_VOID _rfc1006MaxSpduOutstanding_EFun (SX_DEC_CTRL *sxDecCtrl)
  {
ST_BOOLEAN rc;
ST_CHAR *str;
ST_INT strLen;

  rc = sx_get_string_ptr (sxDecCtrl, &str, &strLen);
  if (rc != SD_SUCCESS)
    return;

#if defined (TP0_ENABLED)   /* Just ignore if disabled.     */
  if (!init_dirser_called)
    {
    ST_UINT uInt = 0;

    if (asciiToUint(str,&uInt) != SD_SUCCESS)
      {
      SXLOG_ERR0 ("ERROR: Rfc1006_Max_Spdu_Outstanding Data Conversion Error");
      sxDecCtrl->errCode = SX_ERR_CONVERT;
      }
    else
      {
      tp0_cfg.max_spdu_outst = uInt;
      }
    }
#endif  /* TP0_ENABLED  */
  }

/************************************************************************/
/*                         _rfc1006ListenPort_EFun                      */
/************************************************************************/

static ST_VOID _rfc1006ListenPort_EFun (SX_DEC_CTRL *sxDecCtrl)
  {
ST_BOOLEAN rc;
ST_CHAR *str;
ST_INT strLen;

  rc = sx_get_string_ptr (sxDecCtrl, &str, &strLen);
  if (rc != SD_SUCCESS)
    return;

#if defined (TP0_ENABLED)   /* Just ignore if disabled.     */
  if (!init_dirser_called)
    {
    ST_UINT16 uShortInt = 0;

    if (asciiToUint16(str,&uShortInt) != SD_SUCCESS)
      {
      SXLOG_ERR0 ("ERROR: Rfc1006_Listen_Port Data Conversion Error");
      sxDecCtrl->errCode = SX_ERR_CONVERT;
      }
    else
      {
      tp0_cfg.rfc1006_listen_port = uShortInt;
      }
    }
#endif  /* TP0_ENABLED  */
  }

/************************************************************************/
/************************************************************************/
/*                             _session_SFun                            */
/************************************************************************/

static ST_VOID _session_SFun (SX_DEC_CTRL *sxDecCtrl)
  {
  SX_PUSH (sessionElements);
  }

/************************************************************************/
/*                          _disconnectTimeout_EFun                     */
/************************************************************************/

static ST_VOID _disconnectTimeout_EFun (SX_DEC_CTRL *sxDecCtrl)
  {
ST_BOOLEAN rc;
ST_CHAR *str;
ST_INT strLen;

ST_UINT16 uShortInt = 0;

  rc = sx_get_string_ptr (sxDecCtrl, &str, &strLen);
  if (rc != SD_SUCCESS)
    return;

  if (!init_dirser_called)
    {
    if (asciiToUint16(str,&uShortInt) != SD_SUCCESS)
      {
      SXLOG_ERR0 ("ERROR: Session Disconnect_Timeout Data Conversion Error");
      sxDecCtrl->errCode = SX_ERR_CONVERT;
      }
    else
      {
      session_cfg.disconnect_timeout = uShortInt;
      }
    }
  }

/************************************************************************/
/************************************************************************/
/*                     _networkAddressing_SFun                          */
/************************************************************************/

static ST_VOID _networkAddressing_SFun (SX_DEC_CTRL *sxDecCtrl)
  {
SX_USR_INFO *usr = (SX_USR_INFO *) sxDecCtrl->usr;
  if (sxDecCtrl->reason == SX_ELEMENT_START)
    {
    /* usr->localTransList and usr->RemoteTransList should be NULL.	*/
    /* Initialized in "osicfgx".					*/
    SX_PUSH (networkAddressingElements);
    }

  if (sxDecCtrl->reason == SX_ELEMENT_END)
    {
    /* save link list data from configuartion file parse in tables */
    save_loc_rem_dib_tables (usr);
    printDibEntry (); 
    }
  }

/************************************************************************/
/*                        _localAddresses_SFun                          */
/************************************************************************/

static ST_VOID _localAddresses_SFun (SX_DEC_CTRL *sxDecCtrl)
  {
  SX_PUSH (LocalAddressesElements);
  }

/************************************************************************/
/************************************************************************/
/*                           _localArName_SFun                          */
/************************************************************************/

static ST_VOID _localArName_SFun (SX_DEC_CTRL *sxDecCtrl)
  {
SX_USR_INFO *usr = (SX_USR_INFO *) sxDecCtrl->usr;
  if (sxDecCtrl->reason == SX_ELEMENT_START)
    {
    SX_PUSH (LocalArNameElements);

    if (!init_dirser_called)
      {           /* Don't alloc if reconfiguring.        */
      usr->currTransEl = (TRANS_DEV_INFO *) chk_calloc (1, sizeof(TRANS_DEV_INFO));
      usr->currTransEl->pres_addr.tp_type = TP_TYPE_TCP;	/* Default=TCP	*/
      list_add_last (&usr->localTransList, usr->currTransEl);
      }
    }
  else	/* reason==SX_ELEMENT_END	*/
    usr->currTransEl = NULL;	/* done with temporary pointer	*/
  }

/************************************************************************/
/*                              _arname_EFun                            */
/************************************************************************/

static ST_VOID _arname_EFun (SX_DEC_CTRL *sxDecCtrl)
  {
ST_BOOLEAN rc;
ST_CHAR *str;
ST_INT strLen;
SX_USR_INFO *usr = (SX_USR_INFO *) sxDecCtrl->usr;

  rc = sx_get_string_ptr (sxDecCtrl, &str, &strLen);
  if (rc != SD_SUCCESS)
    return;


  if ((!init_dirser_called) || 
      (init_dirser_called && usr->rem_section_flag == SD_TRUE))
    {
    if (!str)
      {
      SXLOG_ERR0 ("ERROR: AR_Name Error");
      }
    else if (strlen (str) >= sizeof(usr->currTransEl->name))
      {
      SXLOG_ERR0 ("ERROR: AR_Name Error");
      sxDecCtrl->errCode = SX_ERR_CONVERT;
      }
    else
      strcpy (usr->currTransEl->name, str);
    }
  }

/************************************************************************/
/*                            _apTitle_EFun                             */
/************************************************************************/

static ST_VOID _apTitle_EFun (SX_DEC_CTRL *sxDecCtrl)
  {
ST_BOOLEAN rc;
ST_CHAR *str;
ST_INT strLen;
SX_USR_INFO *usr = (SX_USR_INFO *) sxDecCtrl->usr;

  rc = sx_get_string_ptr (sxDecCtrl, &str, &strLen);
  if (rc != SD_SUCCESS)
    return;

  if ((!init_dirser_called) || 
      (init_dirser_called && usr->rem_section_flag == SD_TRUE))
    {
    if (!str)
      {
      SXLOG_ERR0 ("ERROR: AP_Title Error");
      }
    else if (strlen (str) >= sizeof(usr->currTransEl->apTitle))
      {
      SXLOG_ERR0 ("ERROR: AP_Title Error");
      sxDecCtrl->errCode = SX_ERR_CONVERT;
      }
    else
      strcpy (usr->currTransEl->apTitle, str);
    }
  }

/************************************************************************/
/*                             _aeQualifier_EFun                        */
/************************************************************************/

static ST_VOID _aeQualifier_EFun (SX_DEC_CTRL *sxDecCtrl)
{
ST_BOOLEAN rc;
ST_CHAR *str;
ST_INT strLen;
SX_USR_INFO *usr = (SX_USR_INFO *) sxDecCtrl->usr;

  rc = sx_get_string_ptr (sxDecCtrl, &str, &strLen);
  if (rc != SD_SUCCESS)
    return;

 
  if ((!init_dirser_called) || 
      (init_dirser_called && usr->rem_section_flag == SD_TRUE))
    {
      if (!str)
      {
      SXLOG_ERR0 ("ERROR: AE_Qualifier Error");
      }
    else if (strlen (str) >= sizeof(usr->currTransEl->aeQual))
      {
      SXLOG_ERR0 ("ERROR: AE_Qualifier Error");
      sxDecCtrl->errCode = SX_ERR_CONVERT;
      }
    else
      strcpy (usr->currTransEl->aeQual, str);
    }
  }

/************************************************************************/
/*                              _psel_EFun                              */
/************************************************************************/

static ST_VOID _psel_EFun (SX_DEC_CTRL *sxDecCtrl)
  {
ST_BOOLEAN rc;
ST_CHAR *str;
ST_INT strLen;

ST_UINT hexStrLen = 0;
SX_USR_INFO *usr = (SX_USR_INFO *) sxDecCtrl->usr;

  rc = sx_get_string_ptr (sxDecCtrl, &str, &strLen);
  if (rc != SD_SUCCESS)
    return;

  if ((!init_dirser_called) || 
      (init_dirser_called && usr->rem_section_flag == SD_TRUE))
    {
    if (ascii_to_hex_str (usr->currTransEl->pres_addr.psel,&hexStrLen,MAX_PSEL_LEN,str)
        == SD_SUCCESS)
      {
      usr->currTransEl->pres_addr.psel_len = hexStrLen;
      }
    else
      {
      /*cfg_set_config_err();*/
      SXLOG_ERR0 ("ERROR: Psel Data Conversion Error");
      sxDecCtrl->errCode = SX_ERR_CONVERT;
      }
    }
  }

/************************************************************************/
/*                              _ssel_EFun                              */
/************************************************************************/

static ST_VOID _ssel_EFun (SX_DEC_CTRL *sxDecCtrl)
  {
ST_BOOLEAN rc;
ST_CHAR *str;
ST_INT strLen;

ST_UINT hexStrLen = 0;
SX_USR_INFO *usr = (SX_USR_INFO *) sxDecCtrl->usr;

  rc = sx_get_string_ptr (sxDecCtrl, &str, &strLen);
  if (rc != SD_SUCCESS)
    return;

  if ((!init_dirser_called) || 
      (init_dirser_called && usr->rem_section_flag == SD_TRUE))
    {
    if (ascii_to_hex_str (usr->currTransEl->pres_addr.ssel,&hexStrLen,MAX_SSEL_LEN,str)
        == SD_SUCCESS)
      {
      usr->currTransEl->pres_addr.ssel_len = hexStrLen;
      }
    else
      {
      SXLOG_ERR0 ("ERROR: Ssel Data Conversion Error");
      sxDecCtrl->errCode = SX_ERR_CONVERT;
      }
    }
  }

/************************************************************************/
/*                              _tsel_EFun                              */
/************************************************************************/

static ST_VOID _tsel_EFun (SX_DEC_CTRL *sxDecCtrl)
  {
ST_BOOLEAN rc;
ST_CHAR *str;
ST_INT strLen;

ST_UINT hexStrLen = 0;
SX_USR_INFO *usr = (SX_USR_INFO *) sxDecCtrl->usr;

  rc = sx_get_string_ptr (sxDecCtrl, &str, &strLen);
  if (rc != SD_SUCCESS)
    return;

  if ((!init_dirser_called) || 
      (init_dirser_called && usr->rem_section_flag == SD_TRUE))
    {
    if (ascii_to_hex_str (usr->currTransEl->pres_addr.tsel,&hexStrLen,MAX_TSEL_LEN,str)
        == SD_SUCCESS)
      {
      usr->currTransEl->pres_addr.tsel_len = hexStrLen;
      }
    else
      {
      SXLOG_ERR0 ("ERROR: Tsel Data Conversion Error");
      sxDecCtrl->errCode = SX_ERR_CONVERT;
      }
    }
  }

/************************************************************************/
/*                            _transportType_EFun                       */
/************************************************************************/

static ST_VOID _transportType_EFun (SX_DEC_CTRL *sxDecCtrl)
  {
ST_BOOLEAN rc;
ST_CHAR *str;
ST_INT strLen;
SX_USR_INFO *usr = (SX_USR_INFO *) sxDecCtrl->usr;

  rc = sx_get_string_ptr (sxDecCtrl, &str, &strLen);
  if (rc != SD_SUCCESS)
    return;

  /* NOTE: this is ONLY in local section so don't need to check rem_section_flag.*/
  if (!init_dirser_called)
    {
    if (!str)
      {
      SXLOG_ERR0 ("ERROR: TransportType Data Conversion Error");
      }
    else
      {
      if (stricmp (str, "TCP") == 0)
        usr->currTransEl->pres_addr.tp_type = TP_TYPE_TCP;
      else if (stricmp (str, "TP4") == 0)
        usr->currTransEl->pres_addr.tp_type = TP_TYPE_TP4;
      else if (stricmp (str, "TPX") == 0)
        {
        usr->currTransEl->pres_addr.tp_type = TP_TYPE_TPX;
        }
      else
        {
        SXLOG_ERR0 ("ERROR: Invalid TransportType Data Conversion Error");
        sxDecCtrl->errCode = SX_ERR_CONVERT;
        }
      }  
    }
  }

/************************************************************************/
/************************************************************************/
/*                       _remoteAddresses_SFun                          */
/************************************************************************/

static ST_VOID _remoteAddresses_SFun (SX_DEC_CTRL *sxDecCtrl)
  {
SX_USR_INFO *usr = (SX_USR_INFO *) sxDecCtrl->usr;
  if (sxDecCtrl->reason == SX_ELEMENT_START)
    {
    SX_PUSH (RemoteAddressesElements);
    usr->rem_section_flag = SD_TRUE;
    }
  else
    usr->rem_section_flag = SD_FALSE;
  }

/************************************************************************/
/*                          _remoteArName_SFun                          */
/************************************************************************/

static ST_VOID _remoteArName_SFun (SX_DEC_CTRL *sxDecCtrl)
  {
SX_USR_INFO *usr = (SX_USR_INFO *) sxDecCtrl->usr;
  if (sxDecCtrl->reason == SX_ELEMENT_START)
    {
    SX_PUSH (RemoteArNameElements);

    usr->currTransEl = (TRANS_DEV_INFO *) chk_calloc (1, sizeof(TRANS_DEV_INFO));
    usr->currTransEl->pres_addr.tp_type = TP_TYPE_TCP;	/* Default=TCP	*/
    list_add_last (&usr->remoteTransList, usr->currTransEl);
    }
  else	/* reason==SX_ELEMENT_END	*/
    usr->currTransEl = NULL;	/* done with temporary pointer	*/
  }

/************************************************************************/
/*                            _netAddr_EFun                             */
/************************************************************************/

static ST_VOID _netAddr_EFun (SX_DEC_CTRL *sxDecCtrl)
  {
ST_BOOLEAN rc;
ST_CHAR *str;
ST_INT strLen;
SX_USR_INFO *usr = (SX_USR_INFO *) sxDecCtrl->usr;

  rc = sx_get_string_ptr (sxDecCtrl, &str, &strLen);
  if (rc != SD_SUCCESS)
    return;
    
  if (stricmp(sxDecCtrl->sxDecElInfo.attr[0].value, "IPADDR")==0)
    {
#if defined (TP0_ENABLED)   /* Just ignore if disabled.     */
    if (!str)
      {
      SXLOG_ERR0 ("NetAddr Type=IPADDR Data Conversion Error: null string");
      sxDecCtrl->errCode = SX_ERR_CONVERT;
      }
    else
      {
      usr->currTransEl->pres_addr.tp_type = TP_TYPE_TCP;
      if ((usr->currTransEl->pres_addr.netAddr.ip = tp0_convert_ip (str))
          == htonl(INADDR_NONE))
        {
        SXLOG_ERR1 ("NetAddr Type=IPADDR '%s' invalid", str);
        sxDecCtrl->errCode = SX_ERR_CONVERT;
        }
      }
#else
    SXLOG_ERR1 ("NetAddr Type=IPADDR not allowed in RemoteAddress='%s' (TCP not supported). This entry ignored.", usr->currTransEl->name);
    list_unlink (&usr->remoteTransList, usr->currTransEl);	/* discard this address*/
#endif
    }

  else if (stricmp(sxDecCtrl->sxDecElInfo.attr[0].value, "NSAP")==0)
    {
ST_UINT hexStrLen=0;
    /* NSAP valid only for remote DIB */
    /* NOTE: array size is MAX_IP_ADDR_LEN. Must be >= CLNP_MAX_LEN_NSAP.*/
    if (ascii_to_hex_str (usr->currTransEl->pres_addr.netAddr.nsap,&hexStrLen,CLNP_MAX_LEN_NSAP,str)
        == SD_SUCCESS)
      {
      usr->currTransEl->pres_addr.nsap_len = hexStrLen;
      usr->currTransEl->pres_addr.tp_type = TP_TYPE_TP4;
      }
    else
      {
      /*cfg_set_config_err();*/
      SXLOG_ERR0 ("NetAddr Type=NSAP Data Conversion Error");
      }
    }

  else
    {
    SXLOG_ERR1 ("NetAddr Type='%s' invalid", sxDecCtrl->sxDecElInfo.attr[0].value);
    sxDecCtrl->errCode = SX_ERR_CONVERT;
    }
  }

/************************************************************************/
/*                         _port_EFun                      		*/
/************************************************************************/

static ST_VOID _port_EFun (SX_DEC_CTRL *sxDecCtrl)
  {
ST_BOOLEAN rc;
ST_CHAR *str;
ST_INT strLen;
SX_USR_INFO *usr = (SX_USR_INFO *) sxDecCtrl->usr;

  rc = sx_get_string_ptr (sxDecCtrl, &str, &strLen);
  if (rc != SD_SUCCESS)
    return;

#if defined (TP0_ENABLED)   /* Just ignore if disabled.     */
  if (!init_dirser_called)
    {
    ST_UINT16 uShortInt = 0;

    if (asciiToUint16(str,&uShortInt) != SD_SUCCESS)
      {
      SXLOG_ERR0 ("ERROR: Port Data Conversion Error");
      sxDecCtrl->errCode = SX_ERR_CONVERT;
      }
    else
      {
      usr->currTransEl->pres_addr.port = uShortInt;
      }
    }
#endif  /* TP0_ENABLED  */
  }


/************************************************************************/
/*                       save_loc_rem_dib_tables                        */
/*----------------------------------------------------------------------*/
/* This function will initialize local and remote dib entries tables.   */
/*                                                                      */
/* Parameters:                                                          */
/*  SX_USR_INFO *usr							*/
/*                                                                      */
/* Return:                                                              */
/*  SD_SUCCESS (0)      if initialization successful.                   */
/*  SD_FAILURE (1)      otherwise                                       */
/* NOTES:  Does not allow reconfiguration of loc_dib_table.             */
/*         Allows reconfiguration of rem_dib_table, but only after      */
/*         freeing the old table.                                       */
/************************************************************************/

static ST_RET save_loc_rem_dib_tables (SX_USR_INFO *usr)
  {
ST_RET ret;

  ret = SD_SUCCESS;

  if (!init_dirser_called)      /* Don't allow reconfiguring.   */
    {                  
    num_loc_dib_entries = list_get_sizeof (usr->localTransList);
    if (num_loc_dib_entries)
      {
      loc_dib_table = (DIB_ENTRY *) chk_calloc (num_loc_dib_entries, sizeof (DIB_ENTRY));
      save_dib_table (usr->localTransList, loc_dib_table);
      usr->localTransList=NULL;	/* save_dib_table freed this so clear ptr.*/
      }
    }                             

  /* If Remote DIB table already configured, FREE OLD BEFORE ALLOCATING NEW!*/
  if (init_dirser_called)
    {    
    chk_free (rem_dib_table);
    }                                      

  num_rem_dib_entries = list_get_sizeof (usr->remoteTransList);
  if (num_rem_dib_entries)
    {
    rem_dib_table = (DIB_ENTRY *) chk_calloc (num_rem_dib_entries, sizeof (DIB_ENTRY));
    save_dib_table (usr->remoteTransList, rem_dib_table);
    usr->remoteTransList=NULL;	/* save_dib_table freed this so clear ptr.*/
    }
  return (ret);
  }

/************************************************************************/
/*                       save_dib_table                                 */
/*----------------------------------------------------------------------*/
/* This function copies the temporary DIB Table to the permanent        */
/* DIB Table.  It may be used for the Local as well as the Remote DIB   */
/* Tables.                                                              */
/*                                                                      */
/* Parameters:                                                          */
/*  TRANS_DEV_INFO *    temporary table (linked list of structs)        */
/*  DIB_ENTRY *     permanent table (array of structs)                  */
/*                                                                      */
/* NOTE: This function frees "TransList". The caller should not access	*/
/*       TransList after this function returns.				*/
/************************************************************************/

static ST_VOID save_dib_table (TRANS_DEV_INFO *TransList, DIB_ENTRY *de)
  {
TRANS_DEV_INFO *toFree;
TRANS_DEV_INFO *d;

  d = TransList;    /* Point to first in linked list    */
  while (d)
    {
    /* Copy to de->name (array). Len checked earlier, so this strcpy is safe.*/
    strcpy (de->name, d->name);
    if (strlen (d->apTitle))
      {
      if (parseObjId (&de->ae_title.AP_title, d->apTitle))
        {
        ACSELOG_ERR1 ("Invalid AP Title '%s'. Ignored.", d->apTitle);
        }
      else
        de->ae_title.AP_title_pres = SD_TRUE;    /* defaults to FALSE    */
      }

    if (strlen (d->aeQual))
      {
      de->ae_title.AE_qual_pres = SD_TRUE;   /* defaults to FALSE    */
      de->ae_title.AE_qual = atol (d->aeQual);
      }

    de->ae_title.AP_inv_id_pres = SD_FALSE;
    de->ae_title.AE_inv_id_pres = SD_FALSE;

    memcpy (&de->pres_addr, &d->pres_addr, sizeof (PRES_ADDR));

    toFree = d;
    d = (TRANS_DEV_INFO *) list_get_next (TransList, d);    /* Point to next TRANS_DEV_INFO */
    chk_free (toFree);
    ++de;               /* Point to next DIB_ENTRY  */
    }       /* End "while"  */
  }

/************************************************************************/
/*                               parseObjId                             */
/*----------------------------------------------------------------------*/
/* Parses components of the 'ApTitle'.                                  */
/************************************************************************/

static ST_RET parseObjId (MMS_OBJ_ID *objId, ST_CHAR *buff)
  {
char    *numStr;
ST_CHAR *seps = " \t";	/* delimiters to search for	*/

  /* Found keyword 'ApTitle', read the components                       */
  objId->num_comps = 0;
  numStr = get_next_string (&buff, seps);	/* "buff" ptr changes	*/
  while (numStr && strlen (numStr) > 0)
    {
    if (objId->num_comps >= MAX_OBJID_COMPONENTS)
      {
      return (SD_FAILURE);
      }
    objId->comps[objId->num_comps] = (ST_INT16) atoi (numStr);
    numStr = get_next_string (&buff, seps);	/* "buff" ptr changes	*/
    ++objId->num_comps;
    }
  if (objId->num_comps == 0)    /* couldn't parse anything  */
    return (SD_FAILURE);
  return (SD_SUCCESS);
  }
  
/************************************************************************/
/*                               printDibEntry                          */
/************************************************************************/
static ST_VOID printDibEntry (ST_VOID)
{
DIB_ENTRY *de;
ST_INT i;
ST_INT table_type;
                  
  SXLOG_DEBUG0 (" LOCAL DIB TABLE");

  for (i = 0; i < num_loc_dib_entries; i++)
    {
    table_type = LOCAL_DIB_TABLE;
    de = &loc_dib_table[i];
    printDibStruct(de,table_type);
  }

  SXLOG_DEBUG0 (" REMOTE DIB TABLE");

  for (i = 0; i < num_rem_dib_entries; i++)
    {
    table_type = REMOTE_DIB_TABLE;
    de = &rem_dib_table[i];
    printDibStruct(de,table_type);
    }

  }

/************************************************************************/
/*                            printDibStruct                            */
/************************************************************************/
static ST_VOID printDibStruct (DIB_ENTRY *de,ST_INT table_type)
  {
ST_INT i;
ST_UINT k;

  SXLOG_CDEBUG0 ("                                              ");
  SXLOG_CDEBUG1 (" Common_Name = %s",de->name);
  SXLOG_CDEBUG1 ("  local = %s ",de->local ? "SD_TRUE":"SD_FALSE");
  SXLOG_CDEBUG1 ("  AP_title_pres = %s", de->ae_title.AP_title_pres ? "SD_TRUE":"SD_FALSE");
  for (i = 0; i < de->ae_title.AP_title.num_comps; ++i)
    SXLOG_CDEBUG2 ("  AP_title.comps[%d] = %d",i,de->ae_title.AP_title.comps[i]);
  SXLOG_CDEBUG2 ("  AE_qual_pres = %s AE_qual = %ld",
        de->ae_title.AE_qual_pres ? "SD_TRUE":"SD_FALSE",(ST_LONG)de->ae_title.AE_qual);
  SXLOG_CDEBUG2 ("  AP_inv_id_pres = %s AP_invoke_id = %ld",
        de->ae_title.AP_inv_id_pres ? "SD_TRUE":"SD_FALSE",(ST_LONG)de->ae_title.AP_inv_id);
  SXLOG_CDEBUG2 ("  AE_inv_id_pres = %s AE_invoke_id = %ld",
        de->ae_title.AE_inv_id_pres ? "SD_TRUE":"SD_FALSE",(ST_LONG)de->ae_title.AE_inv_id);

  SXLOG_CDEBUG1 ("  psel_len = %d psel =",de->pres_addr.psel_len);
  for (k = 0; k < de->pres_addr.psel_len; k++)
    SXLOG_CDEBUG1 ("    %02x",de->pres_addr.psel[k]);

  SXLOG_CDEBUG1 ("  ssel_len = %d ssel =",de->pres_addr.ssel_len);
  for (k = 0; k < de->pres_addr.ssel_len; k++)
    SXLOG_CDEBUG1 ("    %02x",de->pres_addr.ssel[k]);

  SXLOG_CDEBUG1 ("  tsel_len = %d tsel =",de->pres_addr.tsel_len);
  for (k = 0; k < de->pres_addr.tsel_len; k++)
    SXLOG_CDEBUG1 ("    %02x",de->pres_addr.tsel[k]);

  if (de->pres_addr.tp_type == TP_TYPE_TCP) 
  {
    SXLOG_CDEBUG1 ("  tp_type = %s ","TCP");
  }
  else if (de->pres_addr.tp_type == TP_TYPE_TP4)
  {
    SXLOG_CDEBUG1 ("  tp_type = %s ","TP4");
  }
  else if (de->pres_addr.tp_type == TP_TYPE_TPX)
  {
    SXLOG_CDEBUG1 ("  tp_type = %s ","TPX");
  }


  if (table_type == REMOTE_DIB_TABLE)
    {
    if (de->pres_addr.tp_type == TP_TYPE_TP4)
      {
      SXLOG_CDEBUG1 ("  nsap_len = %d nsap =",de->pres_addr.nsap_len);
      for (k = 0; k < de->pres_addr.nsap_len; k++)
        SXLOG_CDEBUG1 ("    %02x",de->pres_addr.netAddr.nsap[k]);
      }

#if defined(TP0_ENABLED)
    if (de->pres_addr.tp_type == TP_TYPE_TCP)
      {
      struct in_addr sin_addr;	/* inet_ntoa needs this addr format*/
      sin_addr.s_addr = de->pres_addr.netAddr.ip;
      SXLOG_CDEBUG1 ("  ip_addr (s) = %s",inet_ntoa (sin_addr));
      SXLOG_CDEBUG1 ("  port = %d",de->pres_addr.port);
      }
#endif
    }
           
  }

/************************************************************************/
/*			osicfgx_unconfigure				*/
/* Free all buffers allocated by osicfgx.				*/
/* CAUTION: This should only be called when the stack is NOT running,	*/
/*          so the stack does not run without configuration.		*/
/************************************************************************/
void osicfgx_unconfigure ()
  {
  if (clnp_param.network_device)
    {
    chk_free (clnp_param.network_device);
    clnp_param.network_device = NULL;	/* prevent access to freed mem	*/
    }
  if (loc_dib_table)
    {
    chk_free (loc_dib_table);
    loc_dib_table = NULL;		/* prevent access to freed mem	*/
    }
  if (rem_dib_table)
    {
    chk_free (rem_dib_table);
    rem_dib_table = NULL;		/* prevent access to freed mem	*/
    }
  init_dirser_called = SD_FALSE;    /* CRITICAL to allow complete reconfiguration*/
  }

