/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	1986 - 2005, All Rights Reserved		        */
/*									*/
/* MODULE NAME : scl_srvr.c    						*/
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
/* 02/16/12  JRB	   Many Ed 2 changes (see V6.0000 release notes).*/
/* 04/25/11  JRB	   Add File Management indication functions.	*/
/* 03/31/11  JRB	   Add mvl_free_mms_objs. Call it and call	*/
/*			   osicfgx_unconf* & logcfgx_unconf* before exit.*/
/* 03/23/11  JRB	   Set u_mvl_write_ind_custom.			*/
/* 09/23/10  JRB           Call clnp_init, clnp_end for SMPVAL_SUPPORT	*/
/*			   (previously called automatically by TP4).	*/
/*			   Chk return from smpval_msg_send.		*/
/* 11/17/09  JRB    26	   Del HARD_CODED_CFG example.			*/
/*			   Del refs to secManCfgXmlFile (obsolete).	*/
/* 06/30/08  JRB    25     Fix u_mvl61850_ctl_oper_end to NOT send	*/
/*			   CommandTermination (add send_cmd_term to	*/
/*			   send it AFTER write resp).			*/
/* 05/21/08  JRB    24     Don't allow conclude if requests pending or	*/
/*			   if any files are open.			*/
/* 04/28/08  JRB    23     Disable dyn_mem.. call at startup (too much).*/
/* 03/31/08  GLB    22     Added slog_ipc_std_cmd_service		*/
/* 03/15/07  JRB    21     Del mvlu_rpt_service call (for UCA only).	*/
/* 02/13/07  RKR    20     added ms res to WIN32 u_mvlu_rpt_time_get    */
/* 01/10/07  JRB    19     Del unused local vars.			*/
/* 12/04/06  JRB    18     Fix mvl_vmd_destroy call (new arg).		*/
/*			   Add call to mvl61850_rpt_ctrl_destroy_all.	*/
/* 11/13/06  JRB    17     Use new mvl_vmd_* object handling functions.	*/
/* 10/30/06  JRB    16     Chg args to "scl2_*_create_all" functions.	*/
/*			   u_mvl_scl_set_initial_value: return error.	*/
/*			   Del slog_max_msg_size_set call, see slog_start*/
/* 08/17/06  JRB    15     Add call to mvl61850_rpt_service (61850 rpt	*/
/*			   servicing is now separate from UCA).		*/
/*			   Shorten wait_any_event delay.		*/
/* 08/09/06  JRB    14	   Add u_mvl_scl_set_initial_value callback.	*/
/* 03/27/06  JRB    13     Add SMPVAL test code (see SMPVAL_SUPPORT).	*/
/*			   all_obj_create do not call scl_info_free	*/
/*			   (let user call it if scl_info not needed).	*/
/* 10/25/05  EJV    12     S_SEC_ENABLED: added identify_response_info.	*/
/*			   Call slog_start, slog_end, slogIpcEvent.	*/
/* 09/09/05  JRB    11     Don't print BUILD_NUM.			*/
/* 09/06/05  EJV    10     Added more code for S_SEC_ENABLED.		*/
/* 07/25/05  JRB    09     scl2_ld_create_all: Add brcb_bufsize arg.	*/
/* 07/13/05  JRB    08     Add sample "u_mvl61850_ctl_oper_*" functs.	*/
/* 07/13/05  JRB    07     Chg datamap_cfg_read args.			*/
/*			   Don't exit on err in SCL or DATAMAP config.	*/
/*			   Init all objs before ACSE (see mvl_init_mms_objs)*/
/*			   Call scl_info_destroy when no longer needed.	*/
/*			   Use new all_obj_create to parse SCL & create	*/
/*			   objects.					*/
/*			   Use new all_obj_destroy to cleanup on exit.	*/
/*			   Don't print "Check log file". May not be one.*/
/*			   Set "failure" code in u_no_write_allowed.	*/
/*			   Use new slog_max_msg_size_set.		*/
/* 06/07/05  EJV    06     Set gsLogMaskMapCtrl if S_MT_SUPPORT defined	*/
/* 06/01/05  GLB    05     Removed secLogMaskMapCtrl & usrLogMaskMapCtrl*/
/* 05/24/05  EJV    04     Added logCfgAddMaskGroup (&xxxLogMaskMapCtrl)*/
/*			   Use logcfgx_ex to parse logging config file.	*/
/* 08/10/04  EJV    03     u_mvl_connect_ind_ex: typecasted ret		*/
/* 08/06/04  JRB    02     Del "sclparse.h", use "scl.h".		*/
/*			   Use new user header file.			*/
/* 07/19/04  JRB    01     New app using SCL parser.			*/
/************************************************************************/

#if 0
  #define ACSE_AUTH_ENABLED		/* for password authentication	*/
					/* without security		*/
/*  S_SEC_ENABLED		for certificate based authentication &	*/
/*				SSL encryption, put this define in	*/
/*				application's DSP/MAK file 		*/
#endif



/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include <signal.h>

#include "mmsdefs.h"
#include "mms_pvar.h"
#include "mms_vvar.h"
#include "mms_pvmd.h"
#include "mms_err.h"
#include "mms_pcon.h"
#include "asn1defs.h"
#include "stime.h"

#include "mvl_acse.h"
#include "mvl_uca.h"	/* UCA/IEC defs	*/
#include "mvl_log.h"
#include "uca_obj.h"
#include "tp4api.h"

#include "stdtime.h"
#include "stdtime_mms_btime.h"
#include "uca_srvr.h"
#include "mmsop_en.h"	/* need MMS_FOPEN_EN, etc. defines	*/

#if defined (MVL_GOOSE_SUPPORT)
#include "goose.h"
#include "glbsem.h"
#include <process.h>
#endif

#ifdef SMEM_ENABLE
#include "smem.h"
#endif
#include "scl.h"	/* SCL file processing structs & functions	*/
#include "startup.h"	/* struct & function to parse "startup.cfg"	*/
#include "usermap.h"	/* for DATA_MAP_HEAD, datamap_cfg_read, etc.	*/
#include "sx_arb.h"			/* for sxaTextToLocal		*/
#include "str_util.h"	/* for strncat_maxstrlen, etc.			*/
#if defined(GOOSE_RX_SUPP) || defined(GSSE_RX_SUPP) || defined(GSE_MGMT_RX_SUPP) || defined(SMPVAL_RX_SUPP)
#include "subnet.h"
#endif
#if defined(GOOSE_RX_SUPP)
#include "iec_demo.h"	/* for GOOSE RX support taken from "iecgoose" sample*/
#endif

/*----------------------------------------------------------------------*/
/* NOTE: The MMS-EASE Lite Secured applications (Client, Server) 	*/
/*	 are designed to work with Security Toolkit for MMS-EASE Lite	*/
/*	 (MMS-LITE-SECURE-80x-001).						*/
/*       The S_SEC_ENABLED delimits the security related code.		*/
/*----------------------------------------------------------------------*/
#if defined(S_SEC_ENABLED) && defined(ACSE_AUTH_ENABLED)
#error Only one S_SEC_ENABLED or ACSE_AUTH_ENABLED may be defined
#endif

#if defined(S_SEC_ENABLED)
#include "mmslusec.h"
#endif

#include "smpval.h"
#include <vld.h>
#define SCL_PARSE_MODE_CID	0	/* default SCL parse mode	*/
#define SCL_PARSE_MODE_SCD	1

/************************************************************************/
/* struct for creating list of direct-with-enhanced-security controls	*/
/* that are currently in progress.					*/
/************************************************************************/
typedef struct mvl61850_ctl_direct_enhanced
  {
  /* CRITICAL: First 2 parameters used to add this struct to linked	*/
  /* lists using list_add_last, etc.					*/
  struct mvl61850_ctl_direct_enhanced *next;	/* CRITICAL: DON'T MOVE.*/
  struct mvl61850_ctl_direct_enhanced *prev;	/* CRITICAL: DON'T MOVE.*/
  ST_CHAR oper_ref [MVL61850_MAX_OBJREF_LEN+1];
  MVL_NET_INFO *net_info;	/* connection used for this control	*/
  ST_INT ctlState;		/* Control state (MVL61850_CTLSTATE_*)	*/
  } MVL61850_CTL_DIRECT_ENHANCED;

MVL61850_CTL_DIRECT_ENHANCED *mvl61850_ctl_direct_enhanced_get (
	MVL_NET_INFO *net_info,
	ST_CHAR *oper_ref);	/* ObjectReference of "Oper" attr	*/
ST_VOID mvl61850_ctl_direct_enhanced_free (MVL61850_CTL_DIRECT_ENHANCED *direct_enh);

ST_RET all_obj_create (ST_CHAR *startup_cfg_filename,
	ST_CHAR *datamap_cfg_filename,
	ST_CHAR *datamap_cfg_out_filename,
	SCL_INFO *scl_info);
ST_RET all_obj_destroy ();
ST_RET all_obj_destroy_scd (MVL_VMD_CTRL **vmd_ctrl_arr, ST_UINT vmd_count);

#if defined(SMPVAL_SUPPORT)
static ST_VOID init_etype_info (SCL_SMV *scl_smv,
	ETYPE_INFO *etypeInfo);
#endif
static ST_VOID mvl61850_rpt_data_init (ST_VOID);

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/
/************************************************************************/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/
/* Global variables.							*/
/************************************************************************/
ST_INT32 SampleData [16];	/* Data to demonstrate SMPVAL messages	*/

/* Head of list of direct-with-enhanced-security controls in progress	*/
MVL61850_CTL_DIRECT_ENHANCED *mvl61850_ctl_direct_enhanced_list;

/************************************************************************/
/************************************************************************/
/* MMS IDENTIFY AND STATUS RESPONSE INFORMATION				*/

/* Identify server information						*/
#if defined(S_SEC_ENABLED)
IDENT_RESP_INFO identify_response_info =
  {
  "SISCO",  				/* Vendor 	*/
  S_SEC_LITESECURE_NAME,  		/* Model  	*/
  S_SEC_LITESECURE_VERSION_STR	,  0,	/* Version 	*/
  };
#else
static IDENT_RESP_INFO identify_response_info =
  {
  "SISCO",  		/* Vendor 	*/
  MMSLITE_NAME,  	/* Model  	*/
  MMSLITE_VERSION,  0,	/* Version 	*/
  };
#endif

/* Status server information						*/
static STATUS_RESP_INFO status_resp_info =
  {
  0,		/* logical_stat 	*/
  0, 		/* physical_stat 	*/
  0, 		/* local_detail_pres 	*/
  };

/************************************************************************/
/************************************************************************/
/* Functions in this module						*/

/* Initialization */
#if defined(USE_DIB_LIST)	/* Use new DIB_ENTRY linked list	*/
static ST_RET init_acse_dib_list (ST_CHAR *iedName, ST_CHAR *apName);
#else
static ST_VOID init_acse (ST_VOID);
#endif
static ST_VOID init_log_cfg (ST_VOID);
static ST_VOID init_mem (ST_VOID);

/* Connection Management */
static ST_INT connect_ind_fun (MVL_NET_INFO *cc, INIT_INFO *init_info);
static ST_VOID disc_ind_fun (MVL_NET_INFO *cc, ST_INT discType);

/* Misc. */
ST_VOID ctrlCfun (int);
static ST_VOID send_cmd_term (ST_VOID);

/************************************************************************/

/* General program control variables */
static ST_BOOLEAN doIt = SD_TRUE;

#ifdef SMPVAL_SUPPORT
/************************************************************************/
/*			test_smpval_msg_send				*/
/************************************************************************/
ST_RET test_smpval_msg_send (SMPVAL_MSG *smpvalMsg, SCL_SVCB *scl_svcb,
                      ST_INT *SmpCntPtr, ETYPE_INFO *etypeInfo)
  {
MMS_UTC_TIME utcTime;
ST_UINT asduIdx;
ST_UCHAR dstMac [] = {1,1,1,1,1,1};
#if (SD_BYTE_ORDER==SD_LITTLE_ENDIAN)
ST_INT32 SampleDataConverted [16];	/* SMPVAL Data converted to BigEndian*/
#endif
ST_BOOLEAN SmpRatePres;		/* extracted from OptFlds	*/
ST_BOOLEAN RefrTmPres;		/* extracted from OptFlds	*/
ST_BOOLEAN SmpSynch;		/* extracted from OptFlds	*/
ST_RET retcode = SD_FAILURE;

  utcTime.secs = (ST_UINT32) time(NULL);	/* just set seconds in UTC time.	*/
  if (scl_svcb)
    {
    SmpRatePres  = BSTR_BIT_GET (scl_svcb->OptFlds, SVOPT_BITNUM_SMPRATE);
    RefrTmPres   = BSTR_BIT_GET (scl_svcb->OptFlds, SVOPT_BITNUM_REFRTM);
    SmpSynch = BSTR_BIT_GET (scl_svcb->OptFlds, SVOPT_BITNUM_SMPSYNCH);

    /* For this sample, put same data in each ASDU, but increment "SmpCnt".*/
    for (asduIdx = 0; asduIdx < smpvalMsg->numASDU; asduIdx++)
      {
      (*SmpCntPtr)++;
      retcode = smpval_asdu_data_update (smpvalMsg,
          asduIdx,
#if (SD_BYTE_ORDER==SD_LITTLE_ENDIAN)
          (ST_UINT8 *) SampleDataConverted,
#else
          (ST_UINT8 *) SampleData,	/* for BigEndian, use original unconverted data	*/
#endif
          sizeof (SampleData),
          scl_svcb->smvID,
          *SmpCntPtr,
          scl_svcb->dataRefPres,	/* DatSetPres flag	*/
          scl_svcb->datSet,		/* ST_CHAR *DatSet	*/
          scl_svcb->confRev,		/* ST_UINT32 ConfRev	*/
          SmpSynch,			/* ST_BOOLEAN SmpSynch	*/
          RefrTmPres,			/* RefrTmPres flag	*/
          &utcTime,			/* MMS_UTC_TIME *RefrTm	*/
          SmpRatePres,			/* SmpRatePres flag	*/
          (ST_UINT16)scl_svcb->smpRate);/* ST_UINT16 SmpRate	*/
      if (retcode)
        {
        printf ("smpval_asdu_data_update error\n");
        break;
        }
      }	/* end loop	*/
    }
  if (retcode == SD_SUCCESS)
    {
#if 0	/* DEBUG: enable this to test "security" encode/decode in SMPVAL msg.*/
    /* WARNING: do not exceed max len of securityBuf.	*/
    strcpy (smpvalMsg->securityBuf, "test_security");
    smpvalMsg->securityLen = strlen ("test_security");
#endif

    retcode = smpval_msg_send (smpvalMsg, etypeInfo, dstMac);
    if (retcode)
      printf ("Error sending SampledValue message = 0x%X.\n", retcode);
    else
      {
      printf ("SampledValue message sent, numASDU = %d, smpCnt values:", smpvalMsg->numASDU);
      for (asduIdx = 0; asduIdx < smpvalMsg->numASDU; asduIdx++)
        printf (" %d", smpvalMsg->asduArray[asduIdx].SmpCnt);
      printf ("\n  securityLen = %u\n", smpvalMsg->securityLen);
      }
    }
  return (retcode);
}
/************************************************************************/
/*			test_smpval_msg_send_ed2			*/
/* Small differences for 61850 Edition 2.				*/
/************************************************************************/
ST_RET test_smpval_msg_send_ed2 (SMPVAL_MSG *smpvalMsg,
	SCL_SVCB *scl_svcb,
	ST_INT *SmpCntPtr,
	ETYPE_INFO *etypeInfo,
	ST_UINT8 SmpSynch,	/* 0, 1, 2, or 5-254 (see 61850-9-2)	*/
	ST_BOOLEAN SmpModPres)	/* If yes, send val from SVCB		*/
  {
MMS_UTC_TIME utcTime;
ST_UINT asduIdx;
ST_UCHAR dstMac [] = {1,1,1,1,1,1};
#if (SD_BYTE_ORDER==SD_LITTLE_ENDIAN)
ST_INT32 SampleDataConverted [16];	/* SMPVAL Data converted to BigEndian*/
#endif
ST_BOOLEAN DatSetPres;		/* extracted from OptFlds	*/
ST_BOOLEAN RefrTmPres;		/* extracted from OptFlds	*/
ST_BOOLEAN SmpRatePres;		/* extracted from OptFlds	*/
ST_UINT16 SmpMod;

ST_RET retcode = SD_FAILURE;

  utcTime.secs = (ST_UINT32) time(NULL);	/* just set seconds in UTC time.	*/
  if (scl_svcb)
    {
    DatSetPres   = BSTR_BIT_GET (scl_svcb->OptFlds, SVOPT_BITNUM_DATSET);
    RefrTmPres   = BSTR_BIT_GET (scl_svcb->OptFlds, SVOPT_BITNUM_REFRTM);
    SmpRatePres  = BSTR_BIT_GET (scl_svcb->OptFlds, SVOPT_BITNUM_SMPRATE);

    SmpMod = scl_svcb->smpMod;

    /* For this sample, put same data in each ASDU, but increment "SmpCnt".*/
    for (asduIdx = 0; asduIdx < smpvalMsg->numASDU; asduIdx++)
      {
      (*SmpCntPtr)++;
      /* Call different function for Edition 2.	*/
      retcode = smpval_asdu_data_update_ed2 (smpvalMsg,
          asduIdx,
#if (SD_BYTE_ORDER==SD_LITTLE_ENDIAN)
          (ST_UINT8 *) SampleDataConverted,
#else
          (ST_UINT8 *) SampleData,	/* for BigEndian, use original unconverted data	*/
#endif
          sizeof (SampleData),
          scl_svcb->smvID,
          *SmpCntPtr,
          DatSetPres,			/* DatSetPres flag	*/
          scl_svcb->datSet,		/* DatSet	*/
          scl_svcb->confRev,		/* ConfRev	*/
          SmpSynch,			/* SmpSynch	*/
          RefrTmPres,			/* RefrTmPres flag	*/
          &utcTime,			/* RefrTm	*/
          SmpRatePres,			/* SmpRatePres flag	*/
          (ST_UINT16)scl_svcb->smpRate,	/* SmpRate	*/
	  SmpModPres,
	  SmpMod);
      if (retcode)
        { 
        printf ("smpval_asdu_data_update_ed2 error\n");
        break;
        }
      }	/* end loop	*/
    }
  if (retcode == SD_SUCCESS)
    {
#if 0	/* DEBUG: enable this to test "security" encode/decode in SMPVAL msg.*/
    /* WARNING: do not exceed max len of securityBuf.	*/
    strcpy (smpvalMsg->securityBuf, "test_security");
    smpvalMsg->securityLen = strlen ("test_security");
#endif

    smpvalMsg->edition = 2;	/* Tell encoder to encode Edition 2.*/
    retcode = smpval_msg_send (smpvalMsg, etypeInfo, dstMac);
    printf ("SampledValue (Edition 2) message sent, numASDU = %d, smpCnt values:", smpvalMsg->numASDU);
    for (asduIdx = 0; asduIdx < smpvalMsg->numASDU; asduIdx++)
      printf (" %d", smpvalMsg->asduArray[asduIdx].SmpCnt);
    printf ("\n  securityLen = %u\n", smpvalMsg->securityLen);
    }
  return (retcode);
  }
#endif	/* SMPVAL_SUPPORT	*/

/************************************************************************/
/*			mvl_free_mms_objs				*/
/* Free objects allocated by "mvl_init_mms_objs".			*/
/* CAUTION: This function only works for this sample. If objects are	*/
/*          allocated and initialized by Foundry, as in other samples,	*/
/*          it is very difficult to free them.				*/
/************************************************************************/
ST_VOID mvl_free_mms_objs (ST_VOID)
  {
  /* Free buffers allocated at startup by "mvl_init_mms_objs".	*/
  if (mvl_vmd.var_assoc_tbl)
    M_FREE (MSMEM_STARTUP, mvl_vmd.var_assoc_tbl);
  if (mvl_vmd.dom_tbl)
    M_FREE (MSMEM_STARTUP, mvl_vmd.dom_tbl);
  if (mvl_vmd.nvlist_tbl)
    M_FREE (MSMEM_STARTUP, mvl_vmd.nvlist_tbl);
  if (mvl_vmd.jou_tbl)
    M_FREE (MSMEM_STARTUP, mvl_vmd.jou_tbl);
  if (mvl_type_ctrl)
    M_FREE (MSMEM_STARTUP, mvl_type_ctrl);
  }

/************************************************************************/
/************************************************************************/
/*                       main						*/
/************************************************************************/

int main (int argc, char *argv[])
  {
ST_CHAR *usageString = "usage: %s -m scl_parse_mode\n\twhere 'scl_parse_mode' is the SCL parse mode.\n\tIt must be 'scd' or 'cid' (default)";
ST_RET ret;
SCL_INFO scl_info;
STARTUP_CFG startup_cfg;
ST_BOOLEAN event;
SCL_OPTIONS options;			/* for "scl_parse_scd_all"	*/
MVL_VMD_CTRL **vmd_ctrl_arr = NULL;	/* for "scl2_vmd_create_all"	*/
ST_UINT vmd_count;			/* for "scl2_vmd_create_all"	*/
SCL_SERV_OPT serv_opt = {0};		/* for "scl2_vmd_create_all"	*/
ST_INT scl_parse_mode = SCL_PARSE_MODE_CID;	/* SCL parse mode	*/
int argnum;				/* cmd line argument index	*/

#ifdef SMPVAL_SUPPORT
SCL_LD *scl_ld;
SCL_LN *scl_ln;
SCL_SVCB *scl_svcb;
ST_UINT nofASDU;	/* Num of ASDU concatenated into one APDU.	*/
			/* Get from SCL configuration.			*/
ST_DOUBLE currTime;
ST_DOUBLE lastTime = 0;
SMPVAL_MSG *smpvalMsg;
ETYPE_INFO etypeInfo;
ST_INT SmpCnt = 0;	/* incremented with each entry	*/
ST_BOOLEAN *SvEnaPtr;	/* Ptr to SvEna leaf data.	*/
RUNTIME_TYPE *SvEnaRtType;
#if (SD_BYTE_ORDER==SD_LITTLE_ENDIAN)
ST_INT32 SampleDataConverted [16];	/* SMPVAL Data converted to BigEndian*/
#endif
MVL_VAR_ASSOC *var_with_ms;	/* var containing FC=MS	*/
OBJECT_NAME baseVarName;
SCL_SERVER *scl_server;
SCL_SMV *scl_smv = NULL;	/* addressing info	*/
#endif	/* SMPVAL_SUPPORT	*/

  /* Process command line arguments.	*/
  for(argnum=1;argnum<argc;  )
    {
    if (strcmp (argv[argnum], "-m") == 0)
      {
      if (argnum + 2 > argc)
        {
        printf ("Not enough arguments\n"); 
        printf (usageString, argv[0]);
        exit (1);
        }
      if (strcmp (argv[argnum+1], "scd") == 0)
        scl_parse_mode = SCL_PARSE_MODE_SCD;
      else if (strcmp (argv[argnum+1], "cid") == 0)
        scl_parse_mode = SCL_PARSE_MODE_CID;
      else
        {
        printf ("Unrecognized value for '-m' option\n"); 
        printf (usageString, argv[0]);
        exit (1);
        }
      argnum += 2;
      }
    else
      {
      printf ("Unrecognized command line argument '%s'\n", argv[argnum]);
      printf (usageString, argv[0]);
      exit (1);
      }
    }	/* end loop checking command line args	*/

#ifdef SMPVAL_SUPPORT
  /* Initialize SMPVAL data with dummy values.	*/
  SampleData [0]  = 256;
  SampleData [1]  = 0x0000f0f0;	/* Quality in first 2 bytes (low bytes on INTEL)*/
  SampleData [2]  = 257;
  SampleData [3]  = 0x00000f0f;
  SampleData [4]  = 258;
  SampleData [5]  = 0x0000f0f0;
  SampleData [6]  = 259;
  SampleData [7]  = 0x0000f0f0;
  SampleData [8]  = 260;
  SampleData [9]  = 0x0000f0f0;
  SampleData [10] = 261;
  SampleData [11] = 0x0000f0f0;
  SampleData [12] = 262;
  SampleData [13] = 0x0000f0f0;
  SampleData [14] = 263;
  SampleData [15] = 0x0000f0f0;
#endif	/* SMPVAL_SUPPORT	*/

  /* Set global pointer to customized function in userwrite.c	*/
  u_mvl_write_ind_custom = u_mvl_write_ind_61850;

  setbuf (stdout, NULL);    /* do not buffer the output to stdout   */
  setbuf (stderr, NULL);    /* do not buffer the output to stderr   */

  printf ("%s Version %s\n", MMSLITE_NAME, MMSLITE_VERSION);
#if defined(S_SEC_ENABLED)
  printf ("%s Version %s\n", S_SEC_LITESECURE_NAME, S_SEC_LITESECURE_VERSION_STR);
#endif
  puts (MMSLITE_COPYRIGHT);

#if defined(NO_GLB_VAR_INIT)
  mvl_init_glb_vars ();
#endif
  init_mem ();		/* Set up memory allocation tools 		*/
  init_log_cfg ();	/* Set up logging subsystem			*/

  SLOGALWAYS2 ("%s Version %s", MMSLITE_NAME, MMSLITE_VERSION);

  SLOGALWAYS0 ("Initializing ...");
  ret = startup_cfg_read ("startup.cfg", &startup_cfg);
  if (ret)
    {
    printf ("Error reading 'startup.cfg'. Exiting.");
    exit (1);
    }
  mvlu_install ();	/* Install UCA object handler			*/

  /* Must set "mvl_max_dyn" members BEFORE mvl_init_mms_objs to		*/
  /* override default maximum values.					*/
  mvl_max_dyn.types = 5000;
  mvl_max_dyn.dom_vars = 200;
  mvl_max_dyn.dom_nvls = 500;

  /* mvl_init_mms_objs must be called before any dynamic object creation*/
  /* It is called automatically by mvl_start_acse, but it may often be	*/
  /* better to init all objects before mvl_start_acse.			*/
  mvl_init_mms_objs ();

  if (scl_parse_mode == SCL_PARSE_MODE_SCD)
    {
    /* Set parser options and call SCL parser.	*/
    options.forceEdition = 2;	/* Treat as Edition 2 file no matter what*/
    options.includeOwner = SD_FALSE;
    ret = scl_parse_scd_all (startup_cfg.scl_filename, &options, &scl_info);
    if (ret)
      {
      printf ("SCL parse failed. Exiting.\n");
      exit (1);
      }

    /* Set some Server options, then configure ALL IED from the SCL info.	*/
    serv_opt.reportScanRateMs = startup_cfg.report_scan_rate;
    serv_opt.brcb_bufsize     = startup_cfg.BRCBBufferSize;
    serv_opt.logScanRateMs    = startup_cfg.LogScanRateMs;
    serv_opt.logMaxEntries    = startup_cfg.LogMaxEntries;
    vmd_ctrl_arr = scl2_vmd_create_all (&scl_info, startup_cfg.ied_name, startup_cfg.access_point_name,
                   &serv_opt, &vmd_count);
    if (vmd_ctrl_arr)
      {
#ifdef GOOSE_RX_SUPP
      /* Subscribe for GOOSE based on any ExtRef configured in the SCL file.*/
      ret = iecGooseSubscribeExtRefAll (&scl_info, "E1Q1SB1", "S1", callback_function);
      if (ret)
        printf ("iecGooseSubscribeExtRefAll failed\n");
#endif	/* GOOSE_RX_SUPP*/
      /* Do data mapping.	*/
      ret = datamap_cfg_read ("datamap.cfg", "datamapout.cfg");
      if (ret)
        printf ("datamap_cfg_read failed\n");
      }
    else
      printf ("scl2_vmd_create_all failed\n");
    /* NOTE: scl_info freed on exit (see scl_info_destroy).	*/
    }	/* scl_parse_mode == SCL_PARSE_MODE_SCD	*/
  else
    {	/* scl_parse_mode == SCL_PARSE_MODE_CID (old way)	*/
    /* Read all object config files (especially SCL) and create all objects.*/
    ret = all_obj_create ("startup.cfg", "datamap.cfg", "datamapout.cfg", &scl_info);
    if (ret != SD_SUCCESS)
      printf ("\n ERROR: Something in SCL or DATAMAP configuration FAILED.");
    }

  /* NOTE: if data in scl_info is not needed, scl_info_destroy may be	*/
  /*   called now to free up resources.					*/

#if defined(USE_DIB_LIST)	/* Use new DIB_ENTRY linked list	*/
  printf ("\n\nNOTICE: USE_DIB_LIST ENABLED: Using local address from"
            "\n        <ConnectedAP iedName='%s' apName='%s'> element"
	    "\n        of SCL file='%s'.\n",
          startup_cfg.ied_name, startup_cfg.access_point_name, startup_cfg.scl_filename);
  /* Save addresses from SCL file in new global DIB_ENTRY list.		*/
  scl_dib_entry_save (&scl_info);
  ret = init_acse_dib_list (startup_cfg.ied_name, startup_cfg.access_point_name);
  if (ret != SD_SUCCESS)
    {
    printf ("init_acse_dib_list error = 0x%X. Exiting now.\n", ret );
    exit (1);	/* cannot continue	*/
    }
#else
  init_acse ();		/* Start the lower layer subsystem		*/
#endif

  SLOGALWAYS0 ("Initialization complete, entering service loop");

#ifdef SMPVAL_SUPPORT
  /* MSVCB "Volt" configured in sample scl.xml. Try to find it. */
  /* Then find the "SvEna" attribute to use later.		*/
  SvEnaPtr = NULL;	/* NULL indicates "SvEna" not found	*/
  /* Find the right server, then find the SVCB for that server.*/
  scl_server = scl_server_find (&scl_info, startup_cfg.ied_name,
               startup_cfg.access_point_name);
  if (scl_server)
    scl_svcb = scl_svcb_find (scl_server, "C1", "LLN0",  /* always LLN0*/
               "Volt",		/* find this SVCB	*/
               &scl_ld,		/* funct sets scl_ld	*/
               &scl_ln);	/* funct sets scl_ln	*/
  else
    scl_svcb = NULL;	/* can't find server, so can't find SVCB	*/
  if (scl_svcb == NULL)
    {
    printf ("Can't find SVCB in SCL configuration. SampledValue messages will not be sent.\n");
    var_with_ms = NULL;
    }
  else
    {
    baseVarName.object_tag = DOM_SPEC;
    baseVarName.domain_id = scl_ld->domName;
    baseVarName.obj_name.item_id = scl_ln->varName;
    var_with_ms = mvl_vmd_find_var (&mvl_vmd, &baseVarName, NULL);
    if (var_with_ms == NULL)
      {
      printf ("Can't find var '%s' containing SVCB. SampledValue messages will not be sent.\n", scl_ln->varName);
      /* NOTE: main loop chks var_with_ms before sending SampledValue msgs.*/
      }
    else
      SvEnaPtr = mvlu_get_leaf_data_ptr (var_with_ms, "MS$Volt$SvEna", &SvEnaRtType);

    /* Find the SMV for this SVCB (i.e. addressing info).*/
    scl_smv = scl_smv_find (&scl_info, scl_server->iedName, scl_server->apName, scl_ld->inst, scl_svcb->name);
    if (scl_smv == NULL)
      {
      SLOGALWAYS3 ("ERROR: unable to find addressing information for SMVControl '%s' in ldInst '%s' in IED '%s'",
                   scl_svcb->name, scl_ld->inst, scl_server->iedName);
      }
    else
      /* Use info from the SMV element to initialize ETYPE_INFO structure.*/
      init_etype_info (scl_smv, &etypeInfo);
    }
  /* Automatically enable the the SampledValueControlBlock.	*/
  /* Client may disable it by writing to "SvEna" attribute.	*/
  if (SvEnaPtr)
    *SvEnaPtr = 1;

  if (scl_svcb)
    nofASDU = scl_svcb->nofASDU;
  else
    nofASDU = 1;	/* couldn't find configured num, so assume 1	*/
  smpvalMsg = smpval_msg_create (nofASDU);

#if (SD_BYTE_ORDER==SD_LITTLE_ENDIAN)
  {
  ST_INT j;
  for (j = 0; j < 16; j++)
    {	/* Odd entries are Quality. No need to reverse bytes.	*/
    if (j % 2)
      SampleDataConverted [j] = SampleData [j];	/* Quality	*/
    else
      reverse_bytes ((ST_UINT8 *) &SampleDataConverted[j],
                     (ST_UINT8 *) &SampleData[j], 4);
    }
  }
#endif

#endif	/* SMPVAL_SUPPORT	*/

#if defined(SMPVAL_SUPPORT) || defined(GOOSE_RX_SUPP) || defined(SMPVAL_RX_SUPP) \
    || defined(GSSE_RX_SUPP) || defined(GSE_MGMT_RX_SUPP)
  /* NOTE: clnp_init required for GSSE. It also calls clnp_snet_init	*/
  /*       which is required for GOOSE, GSE_MGMT, SMPVAL.		*/
  ret = clnp_init (1200);	/* big enough for any GSSE msg	*/
  if (ret)
    {
    printf ("clnp_init error 0x%X", ret);
    exit (49);
    }
#endif

#if defined(GOOSE_RX_SUPP) || defined(SMPVAL_RX_SUPP) \
    || defined(GSSE_RX_SUPP) || defined(GSE_MGMT_RX_SUPP)
  /* CRITICAL: This must be called AFTER all subscriptions because it	*/
  /*     sets up the filter on the Multicast MAC from each subscription.*/
  if (set_multicast_filters () != SD_SUCCESS)
    {	/* DEBUG: should never happen.	*/
    printf ("Error setting multicast filters. Cannot receive GOOSE, SampledValue messages.\n");
    exit (50);
    }
#endif

  /* CRITICAL: Initialize all Report data after all other initialization.*/
  /*           This must be AFTER data mapping by "datamap_cfg_read".	*/
  mvl61850_rpt_data_init ();

/* Set the ^c catcher */
  signal (SIGINT, ctrlCfun);

/* We are all initialized, just service communications			*/
  printf ("\n Entering 'server' mode, hit ^c to exit ... \n");

#if 0	/* DEBUG: enable this to see memory usage after initialization	*/
  dyn_mem_ptr_status ();	/* Log memory allocation usage		*/
#endif

  /* We want to know about connection activity */
  u_mvl_disc_ind_fun = disc_ind_fun;

  while (doIt)
    {
    wait_any_event (100);	/* keep this short so reporting is fast	*/
    do
      {
      /* CRITICAL: do like this so both functions called each time through loop.*/
      event =  mvl_comm_serve ();	/* Perform communications service 	*/
#if  defined(GOOSE_RX_SUPP) || defined(GSSE_RX_SUPP) || defined(GSE_MGMT_RX_SUPP) || defined(SMPVAL_RX_SUPP)
      /* NOTE: subnet_serve is called only if an appropriate protocol is enabled.*/
      /*       Need this to receive GOOSE, SampledValue, etc.		*/
      event |= subnet_serve ();
#endif
      } while (event);		/* Loop until BOTH functions return 0*/
    mvl61850_rpt_service (SD_FALSE);	/* process IEC 61850 reports	*/
    mvl61850_log_service ();	/* process IEC 61850 logs	*/

#ifdef SMPVAL_SUPPORT
    /* If enabled (i.e. SvEna=TRUE), send SMPVAL message when smpRate expires.*/
    /* NOTE: lastTime started at 0, so first msg sent immediately.	*/
    /* NOTE: timing will not exactly match smpRate because of wait_any_event.*/

    if (SvEnaPtr && *SvEnaPtr != 0)	/* got data ptr earlier. Just chk value*/
      {
      currTime = sGetMsTime();
      if (currTime > lastTime + scl_svcb->smpRate)
        {
        lastTime = currTime;
        if (scl_info.edition == 2)
          test_smpval_msg_send_ed2 (smpvalMsg, scl_svcb, &SmpCnt, &etypeInfo,
                254,		/* SmpSynch	*/
                SD_TRUE);	/* SmpModPres	*/
        else
          test_smpval_msg_send (smpvalMsg, scl_svcb, &SmpCnt, &etypeInfo);
        }
      }
#endif	/* SMPVAL_SUPPORT	*/

#if defined(S_SEC_ENABLED)
    /* check for security configuration updates */
    if (ulCheckSecurityConfiguration () != SD_SUCCESS)
      printf("\n Security Reconfiguration failed\n");
#endif /* defined(S_SEC_ENABLED) */

#if defined(DEBUG_SISCO)
    if (sLogCtrl->logCtrl & LOG_IPC_EN)
      slogIpcEvent ();	/* required for IPC Logging if gensock2.c is	*/
    			/* not compiled with GENSOCK_THREAD_SUPPORT	*/
    /* At runtime, periodically need to service SLOG commands and calling connections.   */
    /* The timing of this service is not critical, but to be responsive a default of     */
    /* 100ms works well.                                                                 */
    slog_ipc_std_cmd_service ("logcfg.xml", NULL, NULL, SD_TRUE,  NULL, NULL);
#endif

    /* Send all IEC 61850 CommandTermination messages.	*/
    send_cmd_term ();
    }

#ifdef SMPVAL_SUPPORT
  smpval_msg_destroy (smpvalMsg);
  /* clnp_init called at startup, so must call clnp_end to clean up.*/
  clnp_end ();
#endif

  ret = mvl_end_acse ();	/* Stop the lower layer subsystem	*/

#if defined(USE_DIB_LIST)	/* Use new DIB_ENTRY linked list	*/
  /* Free up DIB_ENTRY linked list.	*/
  dib_entry_destroy_all ();
#endif

  scl_info_destroy (&scl_info);	/* Destroy all stored SCL info		*/
				/* could be done earlier if SCL info not needed*/

  if (scl_parse_mode == SCL_PARSE_MODE_SCD)
    {
#if defined(GOOSE_RX_SUPP)
    /* If we subscribed to any GOOSE, must unsubscribe now.	*/
    if (iecGooseUnSubscribeAll())
      printf ("Error unsubscribing from all GOOSE messages\n");
#endif

    /* CRITICAL: need different code to destroy ALL IED.*/
    if (vmd_ctrl_arr)
      all_obj_destroy_scd (vmd_ctrl_arr, vmd_count);	/* Destroy all MMS objects for array of VMD.*/
    }	/* scl_parse_mode == SCL_PARSE_MODE_SCD	*/
  else
    all_obj_destroy ();		/* Destroy all MMS objects.		*/

  /* If cleanup worked, number of types logged here should be 0.	*/
  SLOGALWAYS1 ("DEBUG: Number of types in use after cleanup=%d", mvl_type_count ());

  /* Free buffers allocated by "mvl_init_mms_objs".	*/
  mvl_free_mms_objs ();
  /* Free buffers allocated by "osicfgx".	*/
  osicfgx_unconfigure ();

  /* This should log only a few buffers which are freed by slog_end and	*/
  /* logcfgx_unconfigure below.						*/
  dyn_mem_ptr_status ();	/* Log memory allocation usage		*/

#if defined(DEBUG_SISCO)
  /* terminate logging services and save current log file position */
  slog_end (sLogCtrl);

  /* Free buffers allocated by "logcfgx".				*/
  /* NOTE: this is AFTER slog_end, so it can't screw up logging.	*/
  logcfgx_unconfigure (sLogCtrl);
#endif

  printf ("\n\n");

  /* Should have 0 buffers allocated now.	*/
  if (chk_curr_mem_count != 0)
    printf ("Note: %lu buffers not freed.\n",
            (ST_ULONG) chk_curr_mem_count);
    
    
  return (0);
  }

/* #if clauses below only work if 'mmsop_en.h' is included. This verifies that.*/
#ifndef MMS_IDENT_EN
#error 'mmsop_en.h' must be included
#endif
/************************************************************************/
/************************************************************************/
/* INDICATION HANDLING FUNCTIONS					*/
/************************************************************************/
/*			u_mvl_ident_ind					*/
/************************************************************************/
#if (MMS_IDENT_EN & RESP_EN)
ST_VOID u_mvl_ident_ind (MVL_IND_PEND *indCtrl)
  {
  indCtrl->u.ident.resp_info = &identify_response_info;
  mplas_ident_resp (indCtrl);
  }
#endif
/************************************************************************/
/*			u_mvl_status_ind				*/
/************************************************************************/
#if (MMS_STATUS_EN & RESP_EN)
ST_VOID u_mvl_status_ind (MVL_IND_PEND *indCtrl)
  {
  indCtrl->u.status.resp_info = &status_resp_info;
  mplas_status_resp (indCtrl);
  }
#endif
/************************************************************************/
/*			u_mvl_getdom_ind				*/
/************************************************************************/
#if (MMS_GET_DOM_EN & RESP_EN)
ST_VOID u_mvl_getdom_ind (MVL_IND_PEND *indCtrl)
  {
  mvlas_getdom_resp (indCtrl);
  }
#endif
/************************************************************************/
/*			u_mvl_getvlist_ind				*/
/************************************************************************/
#if (MMS_GETVLIST_EN & RESP_EN)
ST_VOID u_mvl_getvlist_ind (MVL_IND_PEND *indCtrl)
  {
  mvlas_getvlist_resp (indCtrl);
  }
#endif
/************************************************************************/
/*			u_mvl_getvar_ind				*/
/************************************************************************/
#if (MMS_GETVAR_EN & RESP_EN)
ST_VOID u_mvl_getvar_ind (MVL_IND_PEND *indCtrl)
  {
  mvlas_getvar_resp (indCtrl);
  }
#endif
/************************************************************************/
/*			u_mvl_namelist_ind				*/
/************************************************************************/
#if (MMS_GETNAMES_EN & RESP_EN)
ST_VOID u_mvl_namelist_ind (MVL_IND_PEND *indCtrl)
  {
  mvlas_namelist_resp (indCtrl);
  }
#endif
/************************************************************************/
/*			u_mvl_defvlist_ind				*/
/************************************************************************/
#if (MMS_DEFVLIST_EN & RESP_EN)
ST_VOID u_mvl_defvlist_ind (MVL_IND_PEND *indCtrl)
  {
  mvlas_defvlist_resp (indCtrl);
  }
#endif
/************************************************************************/
/*			u_mvl_delvlist_ind				*/
/************************************************************************/
#if (MMS_DELVLIST_EN & RESP_EN)
ST_VOID u_mvl_delvlist_ind (MVL_IND_PEND *indCtrl)
  {
  mvlas_delvlist_resp (indCtrl);
  }
#endif
/************************************************************************/
/*			u_mvl_getcl_ind					*/
/************************************************************************/
#if (MMS_GETCL_EN & RESP_EN)
ST_VOID u_mvl_getcl_ind (MVL_IND_PEND *ind)
  {
ST_INT16 num_of_capab;
GETCL_RESP_INFO *info;	/* Same as "ind->u.getcl.resp_info".	*/
			/* Only used to simpify filling it in.	*/
ST_CHAR **capab_list;

  num_of_capab = 4;
  /* Allocate space for Resp struct AND array of pointers to capabilities.*/
  info = ind->u.getcl.resp_info = chk_malloc (sizeof(GETCL_RESP_INFO) +
                                         num_of_capab * sizeof(ST_CHAR *));

  /* Fill in response structure.		*/
  info->more_follows = SD_FALSE;
  info->num_of_capab = num_of_capab;

  /* Fill in array of ptrs to capabilities.	*/
  capab_list = (ST_CHAR **) (info + 1);	/* Point after struct.	*/
  if (num_of_capab > 0)
    capab_list [0] = "SampleCapability_0";
  if (num_of_capab > 1)
    capab_list [1] = "SampleCapability_1";
  if (num_of_capab > 2)
    capab_list [2] = "SampleCapability_2";
  if (num_of_capab > 3)
    capab_list [3] = "SampleCapability_3";

  mplas_getcl_resp (ind);	/* Primitive		*/
  chk_free (info);		/* Done with struct, so free it.	*/
  }
#endif
/************************************************************************/
/*			u_mvl_concl_ind					*/
/************************************************************************/
#if (MMS_CONCLUDE_EN & RESP_EN)
ST_VOID u_mvl_concl_ind (MVL_COMM_EVENT *event)
  {
  /* This function should do all appropriate cleanup before sending the	*/
  /* Conclude response.							*/

  /* CRITICAL: If any requests or indications are pending, or if any	*/
  /* files are open, don't allow conclude.				*/
  if (event->net_info->numpend_ind > 0 || event->net_info->numpend_req > 0
      || event->net_info->file_list != NULL)
    mplas_concl_err (event, MMS_ERRCLASS_CONCLUDE,
                     MMS_ERRCODE_FURTHER_COMM_REQUIRED);
  else
    mplas_concl_resp (event);	/* Send response.	*/
  }
#endif
/************************************************************************/
/************************************************************************/
/* CONNECTION MANAGEMENT FUNCTIONS					*/
/************************************************************************/
/*			connect_ind_fun					*/
/************************************************************************/
/* This function is called when a remote node has connected to us. We	*/
/* can look at the assReqInfo to see who it is (assuming that the 	*/
/* AE-Titles are used), or at the cc->rem_init_info to see initiate 	*/
/* request parameters.							*/

static ST_INT  connect_ind_fun (MVL_NET_INFO *cc, INIT_INFO *init_info)
  {
static INIT_INFO initRespInfo;

  initRespInfo.mms_p_context_pres = SD_TRUE;
  initRespInfo.max_segsize_pres = SD_TRUE;
  initRespInfo.max_segsize = mvl_cfg_info->max_msg_size;
  initRespInfo.maxreq_calling = 1;
  initRespInfo.maxreq_called = 3;
  initRespInfo.max_nest_pres = SD_TRUE;
  initRespInfo.max_nest = 5;
  initRespInfo.mms_detail_pres = SD_TRUE;
  initRespInfo.version = 1;
  initRespInfo.num_cs_init = 0;
  initRespInfo.core_position = 0;
  initRespInfo.param_supp[0] = m_param[0];
  initRespInfo.param_supp[1] = m_param[1];
  memcpy (initRespInfo.serv_supp, m_service_resp,11);

  mvl_init_resp_info = &initRespInfo;
  return (SD_SUCCESS);
  }

/************************************************************************/
/*			u_mvl_connect_ind_ex				*/
/************************************************************************/
/* This function is called when a remote node has connected to us and	*/
/* requires us to look at the authentication info sent to us in the	*/
/* req_auth_info pointer. After we have verified ACSE authentication, we*/
/* can fill out an authentication structure of our own to send to our   */
/* partner and continue processing the connection normally.		*/

ST_ACSE_AUTH u_mvl_connect_ind_ex (MVL_NET_INFO *cc, INIT_INFO *init_info,
            ACSE_AUTH_INFO *req_auth_info, ACSE_AUTH_INFO *rsp_auth_info)
  {
ST_RET ret;

#if defined(S_SEC_ENABLED)
  ret = ulCheckSecureAssocInd (cc, rsp_auth_info);
  if (ret != ACSE_AUTH_SUCCESS)
    {
    printf ("\n Connection not authenticated");
    return ((ST_ACSE_AUTH) ret);
    }
#elif defined(ACSE_AUTH_ENABLED)
  if(req_auth_info->auth_pres == SD_TRUE)
    {
    /* Looks like we have some authentication to look at, simply print  */
    /* the password and continue as normal.				*/
    if(req_auth_info->mech_type == ACSE_AUTH_MECH_PASSWORD)
      {
      printf("\nPassword received from peer: %s\n", req_auth_info->u.pw_auth.password);
      }
    /* We were sent a mechanism we don't support, let's reject the 	*/
    /* the connection with the appropriate diagnostic.			*/
    else
      {
      return ACSE_DIAG_AUTH_MECH_NAME_NOT_RECOGNIZED;
      }
    }
  else
    {
    /* Hmm... looks like we weren't sent any authentication even though */
    /* we require it. Let's reject with the appropriate diagnostic.	*/
    return ACSE_DIAG_AUTH_REQUIRED;
    }
  
  /* Fill out a response authentication structure, must use the pointer */
  /* provided to us here.						*/
  rsp_auth_info->auth_pres = SD_TRUE;
  rsp_auth_info->mech_type = ACSE_AUTH_MECH_PASSWORD;
  /* this is the password */
  strcpy(rsp_auth_info->u.pw_auth.password, "secret");
#endif /* defined(ACSE_AUTH_ENABLED) */
  
  /* Now proceed processing the connection as normal */   
  ret = connect_ind_fun(cc, init_info);
  
  if (ret == SD_SUCCESS)
    return ACSE_AUTH_SUCCESS;
  else
    return ACSE_DIAG_NO_REASON;
  }

/************************************************************************/
/*			disc_ind_fun					*/
/************************************************************************/
/* This function is called when connection is terminated.		*/

static ST_VOID disc_ind_fun (MVL_NET_INFO *cc, ST_INT discType)
  {
  printf ("disconnect indication received.\n");
  mvlu_clr_pend_sbo (cc);
  }

/************************************************************************/
/************************************************************************/
/* INITIALIZATION FUNCTIONS 						*/
/************************************************************************/

#if defined(USE_DIB_LIST)	/* Use new DIB_ENTRY linked list	*/
/************************************************************************/
/*				init_acse_dib_list			*/
/* Initialize ACSE using addresses from SCL. Use iedName and apName	*/
/* from "startup.cfg" to generate Local AR Name.			*/
/* IMPORTANT: addresses from "osicfg.xml" are ignored, but other params	*/
/*            are still needed.						*/
/************************************************************************/
static ST_RET init_acse_dib_list (ST_CHAR *iedName, ST_CHAR *apName)
  {
ST_RET ret;
MVL_CFG_INFO mvlCfg;

  /* Set the disconnect indication function pointer.	*/
  u_mvl_disc_ind_fun     = disc_ind_fun;
  
  ret = osicfgx ("osicfg.xml", &mvlCfg);	/* This fills in mvlCfg	*/
  if (ret != SD_SUCCESS)
    {
    printf ("Stack configuration failed, err = 0x%X. Check configuration.\n", ret );
    return (ret);
    }

  /* Generate Local AR Name by concatenating iedName, "/", and apName	*/
  /* (to match AR Name generated from SCL).				*/
  /* NOTE: this overwrites the Local AR Name set by "osicfgx" above.	*/
  strncpy_safe      (mvlCfg.local_ar_name, iedName, sizeof(mvlCfg.local_ar_name)-1);
  strncat_maxstrlen (mvlCfg.local_ar_name, "/",     sizeof(mvlCfg.local_ar_name)-1);
  strncat_maxstrlen (mvlCfg.local_ar_name, apName,  sizeof(mvlCfg.local_ar_name)-1);

  ret = mvl_start_acse (&mvlCfg);	/* MAKE SURE mvlCfg is filled in*/
  if (ret != SD_SUCCESS)
    {
    printf ("mvl_start_acse () failed, err = 0x%X.\n", ret );
    }
  return (ret);
  }

#else	/* !defined(USE_DIB_LIST)	*/

/************************************************************************/
/*				init_acse				*/
/************************************************************************/

static ST_VOID init_acse ()
  {
ST_RET ret;
MVL_CFG_INFO mvlCfg;

/* We want to know about connection activity				*/
  u_mvl_disc_ind_fun     = disc_ind_fun;
  
  ret = osicfgx ("osicfg.xml", &mvlCfg);	/* This fills in mvlCfg	*/
  if (ret != SD_SUCCESS)
    {
    printf ("Stack configuration failed, err = 0x%X. Check configuration.\n", ret );
    exit (1);
    }

  ret = mvl_start_acse (&mvlCfg);	/* MAKE SURE mvlCfg is filled in*/
  if (ret != SD_SUCCESS)
    {
    printf ("mvl_start_acse () failed, err = %d.\n", ret );
    exit (1);
    }
  }
#endif	/* !defined(USE_DIB_LIST)	*/

/************************************************************************/
/*			init_log_cfg					*/
/************************************************************************/
static ST_VOID init_log_cfg (ST_VOID)
  {
#ifdef DEBUG_SISCO

#if defined(S_SEC_ENABLED)
  logCfgAddMaskGroup (&secLogMaskMapCtrl);
  logCfgAddMaskGroup (&ssleLogMaskMapCtrl);
#endif
  logCfgAddMaskGroup (&mvlLogMaskMapCtrl);
  logCfgAddMaskGroup (&mmsLogMaskMapCtrl);
  logCfgAddMaskGroup (&acseLogMaskMapCtrl);
  logCfgAddMaskGroup (&tp4LogMaskMapCtrl);
  logCfgAddMaskGroup (&clnpLogMaskMapCtrl);
  logCfgAddMaskGroup (&asn1LogMaskMapCtrl);
  logCfgAddMaskGroup (&sxLogMaskMapCtrl);
#if defined(S_MT_SUPPORT)
  logCfgAddMaskGroup (&gsLogMaskMapCtrl);
#endif
  logCfgAddMaskGroup (&sockLogMaskMapCtrl);
  logCfgAddMaskGroup (&memLogMaskMapCtrl);
  logCfgAddMaskGroup (&memDebugMapCtrl);
  
  /* At initialization, install a SLOGIPC command handler. The       */
  /* build in SLOGIPC handler just receives the command and put's    */
  /* on a list to be handled by the application at it's leisure ...  */
  sLogCtrl->ipc.slog_ipc_cmd_fun = slog_ipc_std_cmd_fun;

  if (logcfgx_ex (sLogCtrl, "logcfg.xml", NULL, SD_FALSE, SD_FALSE) != SD_SUCCESS)
    {
    printf ("\n Parsing of 'logging' configuration file failed.");
    if (sLogCtrl->fc.fileName)
      printf ("\n Check log file '%s'.", sLogCtrl->fc.fileName);
    exit (5);
    }

  slog_start (sLogCtrl, MAX_LOG_SIZE);  /* call after logging parameters are configured	*/

#endif  /* DEBUG_SISCO */
  }


/************************************************************************/
/************************************************************************/
/*				init_mem				*/
/************************************************************************/

static ST_VOID mem_chk_error_detected (ST_VOID);
static ST_VOID *my_malloc_err (ST_UINT size);
static ST_VOID *my_calloc_err (ST_UINT num, ST_UINT size);
static ST_VOID *my_realloc_err (ST_VOID *old, ST_UINT size);

static ST_CHAR *spareMem;

static ST_VOID init_mem ()
  {

/* Allocate spare memory to allow logging/printing memory errors	*/
  spareMem = malloc (500);

/* trap mem_chk errors							*/
  mem_chk_err = mem_chk_error_detected;
  m_memerr_fun = my_malloc_err;
  c_memerr_fun = my_calloc_err;
  r_memerr_fun = my_realloc_err;

#if 0
  m_heap_check_enable = SD_TRUE;
  m_check_list_enable = SD_TRUE;
  m_no_realloc_smaller = SD_TRUE;
  m_fill_en = SD_TRUE;
#endif
  m_mem_debug = SD_TRUE;

  }

/************************************************************************/
/* This function is called from the DEBUG version of the mem library	*/
/* when an error of any type is detected.				*/
/************************************************************************/

static ST_INT memErrDetected;

static ST_VOID mem_chk_error_detected (ST_VOID)
  {
  if (!memErrDetected)
    {
    free (spareMem);
    memErrDetected = SD_TRUE;
    printf ("\n Memory Error Detected!");
    dyn_mem_ptr_status ();
    }
  }

/************************************************************************/
/* Memory Allocation Error Handling Functions.				*/
/* These functions are called from mem_chk when it is unable to 	*/
/* perform the requested operation. These functions must either return 	*/
/* a valid buffer or not return at all.					*/
/************************************************************************/

static ST_VOID *my_malloc_err (ST_UINT size)
  {
  mem_chk_error_detected ();
  printf ("\n Malloc");
  exit (2);
  return (NULL);   
  }

static ST_VOID *my_calloc_err (ST_UINT num, ST_UINT size)
  {
  mem_chk_error_detected ();
  exit (3);
  return (NULL);   
  }

static ST_VOID *my_realloc_err (ST_VOID *old, ST_UINT size)
  {
  mem_chk_error_detected ();
  exit (4);
  return (NULL);   
  }

/************************************************************************/
/* MISC. FUNCTIONS							*/
/************************************************************************/
/************************************************************************/
/*				ctrlCfun				*/
/************************************************************************/
/* This function handles the ^c, and allows us to cleanup on exit	*/

ST_VOID ctrlCfun (int i)
  {
  doIt = SD_FALSE;
  }

/************************************************************************/
/************************************************************************/
#if defined MVLU_LEAF_FUN_LOOKUP_ENABLE
/************************************************************************/
/* DYNAMIC LEAF ACCESS PARAMETER MANIPULATION				*/
/************************************************************************/
/*			u_mvlu_resolve_leaf_ref				*/
/************************************************************************/
/* This callback function is invoked from within the MVLU LAP file	*/
/* load function mvlu_load_xml_leaf_file from mvlu_set_leaf_param_name.	*/

/* We are to provide the leaf reference value for the named leaf via	*/
/* the outparameter refOut, nominally by looking at the reference text	*/
/* that is passed to us.						*/

/* The bit masked input/output parameter setFlagsIo tells us whether	*/
/* the reference text was present in the LAP file, and we can set or	*/
/* clear the bit to control whether the reference value is written.	*/

ST_RET u_mvlu_resolve_leaf_ref (ST_CHAR *leafName, ST_INT *setFlagsIo, 
				ST_CHAR *refText, ST_RTREF *refOut)
  {
ST_CHAR *p;

/* Note: In this example  we won't really set the reference, because 	*/
/* the references we need are already set in the leaf at compile time 	*/
/* by the Foundry LAP input file					*/

/* First let's see if the reference text was provided 			*/
  if (*setFlagsIo & MVLU_SET_REF)
    {
  /* The refText text was provided in the LAP file.			*/
  /* We need to check for a special PREFIX (used especially for SBO).	*/
    if (!strncmp (refText, MVLU_STRING_REF_PREFIX, strlen (MVLU_STRING_REF_PREFIX)))
      {		/* Write everything after PREFIX as quoted string.	*/
      p = refText + strlen(MVLU_STRING_REF_PREFIX);
      *refOut = (ST_RTREF) chk_strdup (p);
      }
    else	/* If no PREFIX, we don't handle it.			*/
      *setFlagsIo &= ~MVLU_SET_REF;
    }
  else
    {
  /* The refText text was NOT provided in the LAP file.			*/
  /* If we wanted, we could still provide a reference to be used ...	*/
#if 0
    *refOut = (ST_RTREF) 0;
    *setFlagsIo |= MVLU_SET_REF;
#endif
    }

/* We always return success, otherwise the LAP will not be set		*/
  return (SD_SUCCESS);
  }
#endif

/************************************************************************/
/*			u_mvlu_rpt_time_get				*/
/* UCA/IEC reporting callback function to get accurate rpt time.	*/
/* If UCA/IEC Reports are enabled, user must provide this function.	*/
/************************************************************************/
ST_VOID u_mvlu_rpt_time_get (MMS_BTIME6 *TimeOfEntry)
  {
#if 1	/* This is a very simple example using the standard "time" fct.	*/
STDTIME                  stdTime;   

/* This should be GMT time.  61850 now says all Btime6 values must be   */
/* based on GMT.                                                        */

   GetStdTime (&stdTime);          /* if GMT time needed  */
   StdTimeToMmsTimeOfDay6 (&stdTime, TimeOfEntry);  

#else	/* This is simplest possible code if user doesn't care about time.*/
  TimeOfEntry->ms = 0;
  TimeOfEntry->day = 0;
#endif
  return;
  }

/* NOTE: these leaf functions copied from rdwrind.c. Can't use whole file.*/

/************************************************************************/
/* 		'NO WRITE ALLOWED' WRITE HANDLER			*/
/************************************************************************/
ST_VOID u_no_write_allowed (MVLU_WR_VA_CTRL *mvluWrVaCtrl)
  {
  mvluWrVaCtrl->wrVaCtrl->failure = ARE_OBJ_ACCESS_DENIED;	/* send in rsp*/
  mvlu_wr_prim_done (mvluWrVaCtrl, SD_FAILURE);
  }
/************************************************************************/
/*			u_mvl_sbo_operate				*/
/* NOTE: Only called for UCA controls but linker needs it.		*/
/************************************************************************/
ST_VOID  u_mvl_sbo_operate (MVL_SBO_CTRL *sboSelect, 
			    MVLU_WR_VA_CTRL *mvluWrVaCtrl)
  {
MVL_VAR_ASSOC *va;
ST_CHAR *src;

  /* The select is in place, go ahead and operate ... 			*/
  va = mvluWrVaCtrl->wrVaCtrl->va;
  src = mvluWrVaCtrl->primData;

  mvlu_wr_prim_done (mvluWrVaCtrl, SD_SUCCESS);
  }

/************************************************************************/
/*			datamap_cfg_log					*/
/* Log all mappings created by datamap_cfg_read.			*/
/* FOR DIAGNOSTICS ONLY.						*/
/************************************************************************/
ST_VOID datamap_cfg_log ()
  {
MVL_DOM_CTRL *dom;
MVL_VAR_ASSOC *var;
ST_INT didx, vidx;	/* indices to domain & variable arrays	*/
DATA_MAP_HEAD *map_head_sorted;
ST_UINT j;

  /* Loop through all "Logical Devices" (domains).	*/
  for (didx = 0; didx < mvl_vmd.num_dom; didx++)
    {
    dom = mvl_vmd.dom_tbl [didx];

    /* Loop through all "Logical Nodes" (variables).	*/
    for (vidx = 0; vidx < dom->num_var_assoc; vidx++)
      {
      var = dom->var_assoc_tbl[vidx];
      map_head_sorted = (DATA_MAP_HEAD *) var->user_info;

      SLOGALWAYS2 ("Number of primitive data elements in LN '%s' = %d", var->name, map_head_sorted->map_count);
      for (j = 0; j < map_head_sorted->map_count; j++)
        {
        DATA_MAP *map_entry;
        map_entry = map_head_sorted->map_arr[j];
        if (map_entry)
          {
          /* Leafs auto-mapped for "deadband" logged here but not written	*/
          /* to output file (DO NOT want user to try to map them).	*/
          if (map_entry->deadband_info)
            SLOGCALWAYS2   ("leaf[%d]: auto-mapped for deadband only.  Name: %s", j, 
                        map_entry->leaf);
          else
            SLOGCALWAYS3   ("leaf[%d]: mapped.   Name: %s.   User: %s", j, 
                        map_entry->leaf, map_entry->usr_data_info);
          }
        else
          SLOGCALWAYS1   ("leaf[%d]: not mapped", j);
        }
      }
    }
  }

/************************************************************************/
/*			all_obj_create					*/
/* Read all object configuration files (especially SCL) and create all	*/
/* objects.								*/
/* NOTE: this uses the ReportScanRate and BRCBBufferSize entries from	*/
/*       "startup.cfg".							*/
/************************************************************************/
ST_RET all_obj_create (ST_CHAR *startup_cfg_filename,
	ST_CHAR *datamap_cfg_filename,
	ST_CHAR *datamap_cfg_out_filename,
	SCL_INFO *scl_info)
  {
ST_RET ret;
STARTUP_CFG startup_cfg;	/* info from "startup.cfg" file		*/
ST_DOUBLE mstime;

  ret = startup_cfg_read (startup_cfg_filename, &startup_cfg);

  mstime =sGetMsTime ();
  if (ret == SD_SUCCESS)
    ret = scl_parse (startup_cfg.scl_filename,
         startup_cfg.ied_name,			/* IED name	*/
         startup_cfg.access_point_name,		/* AccessPoint	*/
         scl_info);
  printf ("scl_parse time = %.0f ms\n", sGetMsTime() - mstime);	/* in milliseconds*/

  /* Create types only if everything successful up to now.	*/
  if (ret == SD_SUCCESS)
    {
    mstime = sGetMsTime ();
    /* CRITICAL: This saves the type_id in each SCL_LN struct.	*/
    /*           Must not call again with same "scl_info".	*/
    ret = scl2_datatype_create_all (&mvl_vmd, scl_info,
               0,		/* no limit on rt_count		*/
               SD_FALSE,	/* type names not generated	*/
               NULL);		/* type name prefix (not used)	*/
    printf ("scl2_datatype_create_all time = %.0f ms\n", sGetMsTime() - mstime);	/* in milliseconds*/
    }

  /* Create variables only if everything successful up to now.	*/
  if (ret == SD_SUCCESS)
    ret = scl2_ld_create_all (&mvl_vmd, scl_info, startup_cfg.report_scan_rate,
              startup_cfg.BRCBBufferSize,
              SD_FALSE);	/* create "server" model	*/


  if (ret == SD_SUCCESS)
    {
    /* Log all SCL info stored.				*/
    /* NOTE: this only logs if SXLOG_DEC enabled.	*/
    scl_log_all (scl_info);
    }

  /* Map MMS data to user data by reading user data mapping file.	*/
  if (ret == SD_SUCCESS)
    {
    mstime = sGetMsTime ();
    ret = datamap_cfg_read (datamap_cfg_filename, datamap_cfg_out_filename);
    printf ("datamap_cfg_read time = %.0f ms", sGetMsTime() - mstime);	/* in milliseconds*/
    }

#if 0	/* DEBUG: enable this to see what data mapping was done.	*/
  if (ret == SD_SUCCESS)
    datamap_cfg_log ();
#endif

  return (ret);
  }

/************************************************************************/
/*			all_obj_destroy					*/
/* Destroy all objects created by all_obj_create.			*/
/************************************************************************/
ST_RET all_obj_destroy ()
  {
ST_RET ret;
  /* CRITICAL: datamap_cfg_destroy must be called BEFORE mvl_vmd_destroy*/
  /*           if datamap_cfg_read was called at startup.		*/
  datamap_cfg_destroy ();
  mvl61850_rpt_ctrl_destroy_all ();	/* destroy rpt_ctrls before VMD	*/
  mvl61850_log_ctrl_destroy_all ();	/* destroy log_ctrls before VMD	*/
  ret = mvl_vmd_destroy (&mvl_vmd);	/* destroy global VMD	*/
  return (ret);
  }

/************************************************************************/
/*			all_obj_destroy_scd				*/
/* Destroy all objects created by all_obj_create.			*/
/************************************************************************/
ST_RET all_obj_destroy_scd (MVL_VMD_CTRL **vmd_ctrl_arr, ST_UINT vmd_count)
  {
ST_RET ret = SD_SUCCESS;
ST_UINT j;
  /* CRITICAL: datamap_cfg_destroy must be called BEFORE mvl_vmd_destroy*/
  /*           if datamap_cfg_read was called at startup.		*/
  datamap_cfg_destroy ();
  mvl61850_rpt_ctrl_destroy_all ();	/* destroy rpt_ctrls before VMD	*/
  mvl61850_log_ctrl_destroy_all ();	/* destroy log_ctrls before VMD	*/

  /* Destroy ALL VMDs.	*/
  /* NOTE: One of these VMDs may be the global VMD "mvl_vmd".		*/
  for (j = 0; j < vmd_count; j++)
    {
    ret = mvl_vmd_destroy (vmd_ctrl_arr[j]);
    if (ret)
      {
      printf ("Error destroying VMD %p", vmd_ctrl_arr[j]);
      }
    }
  chk_free (vmd_ctrl_arr);
  return (ret);
  }

/************************************************************************/
/*			u_mvl61850_ctl_oper_begin			*/
/* Sample user callback function.					*/
/************************************************************************/
ST_VOID u_mvl61850_ctl_oper_begin (ST_CHAR *oper_ref)
  {
  }
/************************************************************************/
/*			u_mvl61850_ctl_oper_end				*/
/* Sample user callback function.					*/
/* For controls with enhanced security, need to send "CommandTermination"*/
/* when operation completes. Save information needed for that.		*/
/************************************************************************/
ST_VOID u_mvl61850_ctl_oper_end (MVL_NET_INFO *net_info, ST_CHAR *oper_ref, MVL_VAR_ASSOC *base_var)
  {
ST_CHAR ctlmodel_name [MAX_IDENT_LEN + 1];
ST_INT8 ctlModel;
MVL_SBO_CTRL *sboCtrl;

  /* Read "ctlModel" and do what is appropriate for each model.	*/
  if (mvl61850_mkname_ctlmodel (oper_ref, ctlmodel_name, MAX_IDENT_LEN) == SD_SUCCESS
      && mvlu_get_leaf_val_int8 (base_var, ctlmodel_name, &ctlModel) == SD_SUCCESS)
    {
    /* NOTE: if (ctlModel == MVL61850_CTLMODEL_SBO_ENHANCED), all info	*/
    /* needed for CommandTermination is already stored in global "sbo_pool".*/
    /* Just need to change the state (maybe should do this AFTER write	*/
    /* response is sent, but we know we will send response right away).	*/
    if (ctlModel == MVL61850_CTLMODEL_SBO_ENHANCED)
      {
      /* This funct just finds the MVL_SBO_CTRL struct for "oper_ref".	*/
      sboCtrl = mvlu_sbo_chk_state (oper_ref, net_info);
      if (sboCtrl && sboCtrl->ctlState == MVL61850_CTLSTATE_READY)
        sboCtrl->ctlState = MVL61850_CTLSTATE_WAIT_CHANGE;
      }

    /* For MVL61850_CTLMODEL_DIRECT_ENHANCED, save necessary info now.	*/
    if (ctlModel == MVL61850_CTLMODEL_DIRECT_ENHANCED)
      {
      /* Get new control struct now (added to global list).		*/
      /* If this fails, CommandTermination will never get sent.		*/
      mvl61850_ctl_direct_enhanced_get (net_info, oper_ref);
      }
    }
  }

/************************************************************************/
/*			u_mvl_scl_set_initial_value			*/
/* Called during SCL processing to convert initial value text to data.	*/
/* NOTE: only called if MVL library conversion fails.			*/
/************************************************************************/
ST_RET u_mvl_scl_set_initial_value (SCL2_IV_TRANSLATE_CTRL *translate)
  {
#if 0
  /* The MVL library already calls sxaTextToLocal for most data.	*/
  /* If that fails, user might want to try different conversion		*/
  /* function, but the call might be similar to this.			*/
  return (sxaTextToLocal (translate->valText, translate->dest,
          translate->numRt, translate->rtHead));
#else
  SLOGALWAYS1 ("Error converting Val '%s' from SCL file to data.", translate->valText);
  return (SD_FAILURE);	/* this will cause SCL processing to stop	*/
#endif
  }

/************************************************************************/
/*			mvl61850_ctl_direct_enhanced_get		*/
/* RETURNS: pointer to allocated structure, NULL on error.		*/
/************************************************************************/
MVL61850_CTL_DIRECT_ENHANCED *mvl61850_ctl_direct_enhanced_get (
	MVL_NET_INFO *net_info,
	ST_CHAR *oper_ref)	/* ObjectReference of "Oper" attr	*/
  {
MVL61850_CTL_DIRECT_ENHANCED *direct_enhanced;

  if (strlen (oper_ref) >= sizeof (direct_enhanced->oper_ref))
    {	/* should NEVER fail	*/
    MVL_LOG_ERR1 ("IEC 61850 Control object '%s' name too long",
                  oper_ref);
    direct_enhanced = NULL;	/* error	*/
    }
  else
    {
    direct_enhanced = (MVL61850_CTL_DIRECT_ENHANCED *) chk_calloc (1, sizeof (MVL61850_CTL_DIRECT_ENHANCED));
    strcpy (direct_enhanced->oper_ref, oper_ref);
    direct_enhanced->net_info = net_info;
    direct_enhanced->ctlState = MVL61850_CTLSTATE_WAIT_CHANGE;
    /* Add to end of global linked list.	*/
    list_add_last (&mvl61850_ctl_direct_enhanced_list, direct_enhanced);
    }
  return (direct_enhanced);
  }
/************************************************************************/
/************************************************************************/
ST_VOID mvl61850_ctl_direct_enhanced_free (MVL61850_CTL_DIRECT_ENHANCED *direct_enh)
  {
  /* Remove entry from the linked list and free it.	*/
  list_unlink (&mvl61850_ctl_direct_enhanced_list, direct_enh);
  chk_free (direct_enh);
  }

/************************************************************************/
/*			send_cmd_term					*/
/* Send IEC 61850 CommandTermination for any "enhanced" control.	*/
/* CommandTermination should always be sent AFTER "Oper" write resp.	*/
/* NOTE: this function assumes all control commands are done.		*/
/************************************************************************/
static ST_VOID send_cmd_term (ST_VOID)
  {
  ST_INT sboIdx;
  MVL61850_CTL_DIRECT_ENHANCED *direct_enhanced;
  MVL61850_LAST_APPL_ERROR last_appl_error;
  /* DEBUG: use statics here so value can be changed on each call*/
  static ST_RET completion_status = SD_SUCCESS;
  static ST_INT8 dummyError = 0;
  static ST_INT8 dummyAddCause = 10;

  /* Send CommandTermination for any "sbo-with-enhanced-security"	*/
  /* control. Loop through global "sbo_pool" array.			*/
  for (sboIdx = 0; sboIdx < MAX_NUM_SBO_PEND; ++sboIdx)
    {
    MVL_SBO_CTRL *sboCtrl;
    sboCtrl = &sbo_pool[sboIdx];	/* point to global array entry	*/

    if (sboCtrl->in_use &&
        sboCtrl->ctlState == MVL61850_CTLSTATE_WAIT_CHANGE)
      {		/* must be sbo-with-enhanced-security to be in this state*/
      /* DEBUG: For testing purposes only, change status for each CommandTermination.*/
      /* Real application should get status from control operation.	*/
      completion_status = (completion_status == SD_SUCCESS) ? SD_FAILURE : SD_SUCCESS;

      if (completion_status!=SD_SUCCESS)
        {
        /* last_appl_error must contain valid data. Set appropriate values here.*/
        memset (&last_appl_error, 0, sizeof (MVL61850_LAST_APPL_ERROR));
        strcpy (last_appl_error.CntrlObj, sboCtrl->sbo_var);
        last_appl_error.Error = dummyError++;	/* DEBUG: For testing, chg on each call*/
        last_appl_error.AddCause = dummyAddCause++;
        }

      mvl61850_ctl_command_termination (sboCtrl->net_info, sboCtrl->sbo_var,
              completion_status,
              &last_appl_error);
      }
    }

  /* Send CommandTermination for any "direct-with-enhanced-security" control.*/
  /* Loop through global list "mvl61850_ctl_direct_enhanced_list".	*/
  /* This removes each entry from list after sending CmdTerm.		*/
  /* NOTE: entries added to this list by "u_mvl61850_ctl_oper_end".	*/
  while ((direct_enhanced = mvl61850_ctl_direct_enhanced_list) != NULL)
    {
    /* DEBUG: For testing purposes only, change status for each CommandTermination.*/
    /* Real application should get status from control operation.	*/
    completion_status = (completion_status == SD_SUCCESS) ? SD_FAILURE : SD_SUCCESS;

    if (completion_status!=SD_SUCCESS)
      {
      /* last_appl_error must contain valid data. Set appropriate values here.*/
      memset (&last_appl_error, 0, sizeof (MVL61850_LAST_APPL_ERROR));
      strcpy (last_appl_error.CntrlObj, direct_enhanced->oper_ref);
      last_appl_error.Error = dummyError++;	/* DEBUG: For testing, chg on each call*/
      last_appl_error.AddCause = dummyAddCause++;
      }

    mvl61850_ctl_command_termination (direct_enhanced->net_info, direct_enhanced->oper_ref,
            completion_status,
            &last_appl_error);
    /* Remove entry from the linked list and free it.	*/
    mvl61850_ctl_direct_enhanced_free (direct_enhanced);
    }
  }

#if (MMS_FOPEN_EN & RESP_EN)
/************************************************************************/
/*			u_mvl_fopen_ind					*/
/************************************************************************/
ST_VOID u_mvl_fopen_ind (MVL_IND_PEND *indCtrl)
  {
FILE *fp;
FOPEN_RESP_INFO resp_info;
struct stat  stat_buf;
MVL_NET_FILE *open_file;

  fp = fopen (indCtrl->u.fopen.filename, "rb");	/* CRITICAL: use "b" flag for binary transfer*/
  if (fp == NULL)
    {
    _mplas_err_resp (indCtrl,11,6);		/* File-access denied	*/
    return;
    }
  if (fseek (fp, indCtrl->u.fopen.init_pos, SEEK_SET))
    {
    _mplas_err_resp (indCtrl,11,5);		/* Position invalid	*/
    fclose (fp);  /* don't want open file hanging around  */
    return;
    }

  /* Allocate MVL_NET_FILE struct and add it to linked list.	*/
  /* NOTE: this must be before resp so "event" still valid.	*/
  open_file = chk_malloc (sizeof (MVL_NET_FILE));
  open_file->fp = fp;
  list_add_last (&indCtrl->event->net_info->file_list, open_file);

  /* Use (MVL_NET_FILE *) as frsmid.				*/
  /* WARNING: this only works if pointers are 32 bits.		*/
  resp_info.frsmid = (ST_INT32) open_file;

  if (fstat (fileno (fp), &stat_buf))
    {					/* Can't get file size or time	*/
    _mplas_err_resp (indCtrl,11,0);	/* File Problem, Other		*/
    return;
    }
  else
    {
    resp_info.ent.fsize    = stat_buf.st_size;
    resp_info.ent.mtimpres = SD_TRUE;
    resp_info.ent.mtime    = stat_buf.st_mtime;
    }

  indCtrl->u.fopen.resp_info = &resp_info;
  mplas_fopen_resp (indCtrl);
  }
#endif 	/* MMS_FOPEN_EN & RESP_EN	*/


#if (MMS_FREAD_EN & RESP_EN)
/************************************************************************/
/*			u_mvl_fread_ind					*/
/************************************************************************/
ST_VOID u_mvl_fread_ind (MVL_IND_PEND *indCtrl)
  {
FILE *fp;
ST_UCHAR *tmp_buf;
MVLAS_FREAD_CTRL *fread_ctrl = &indCtrl->u.fread;
FREAD_RESP_INFO resp_info;
MVL_NET_FILE *open_file;

  /* Use frsmid to find (FILE *).	*/
  open_file = (MVL_NET_FILE *) fread_ctrl->req_info->frsmid;
  fp = open_file->fp;
  /* Do NOT read more than "max_size".				*/
  tmp_buf = (ST_UCHAR *) chk_malloc (fread_ctrl->max_size);

  resp_info.fd_len = fread (tmp_buf, 1, fread_ctrl->max_size, fp);
  if (resp_info.fd_len == 0  &&  ferror (fp))
    {
    _mplas_err_resp (indCtrl, 3, 0);
    return;
    }

  resp_info.filedata = tmp_buf;
  if (resp_info.fd_len == fread_ctrl->max_size)
    resp_info.more_follows = SD_TRUE;
  else
    resp_info.more_follows = SD_FALSE;

  fread_ctrl->resp_info = &resp_info;
  mplas_fread_resp (indCtrl);
  chk_free (tmp_buf);		/* Temporary buffer	*/
  }
#endif	/* #if (MMS_FREAD_EN & RESP_EN)	*/

#if (MMS_FCLOSE_EN & RESP_EN)
/************************************************************************/
/*			u_mvl_fclose_ind					*/
/************************************************************************/
ST_VOID u_mvl_fclose_ind (MVL_IND_PEND *indCtrl)
  {
FILE *fp;
MVLAS_FCLOSE_CTRL *fclose_ctrl = &indCtrl->u.fclose;
MVL_NET_FILE *open_file;

  /* Use frsmid to find (FILE *).	*/
  open_file = (MVL_NET_FILE *) fclose_ctrl->req_info->frsmid;
  fp = open_file->fp;

  if (fclose (fp))
    _mplas_err_resp (indCtrl, 11, 0);	/* File problem, other	*/
  else
    {
    /* Remove "open_file" from list and free it.	*/
    /* NOTE: do this before resp so event still valid.	*/
    list_unlink (&indCtrl->event->net_info->file_list, open_file);
    chk_free (open_file);
    /* Send response.	*/
    mplas_fclose_resp (indCtrl);
    }
  }
#endif	/* #if (MMS_FCLOSE_EN & RESP_EN)	*/

#if (MMS_FDIR_EN & RESP_EN)
/************************************************************************/
/*			u_mvl_fdir_ind					*/
/************************************************************************/
ST_VOID u_mvl_fdir_ind (MVL_IND_PEND *indCtrl)
  {
  /* This function works for _WIN32, linux, and other Posix systems.	*/
  /* It may need to be ported for other systems.			*/
  mvlas_fdir_resp (indCtrl);
  }
#endif	/* #if (MMS_FDIR_EN & RESP_EN)	*/

#if (MMS_FDELETE_EN & RESP_EN)
/************************************************************************/
/*			u_mvl_fdelete_ind				*/
/************************************************************************/
ST_VOID u_mvl_fdelete_ind (MVL_IND_PEND *indCtrl)
  {
MVLAS_FDELETE_CTRL *fdelete_ctrl = &indCtrl->u.fdelete;

  /* Use the ANSI "remove" function if available on your OS.		*/
  /* Otherwise, use the appropriate function for your OS.		*/
  if (remove (fdelete_ctrl->filename))
    _mplas_err_resp (indCtrl, 11, 0);	/* File problem, other	*/
  else
    mplas_fdelete_resp (indCtrl);
  }
#endif	/* #if (MMS_FDELETE_EN & RESP_EN)	*/

#if (MMS_FRENAME_EN & RESP_EN)
/************************************************************************/
/*			u_mvl_frename_ind       		   	*/
/************************************************************************/
ST_VOID u_mvl_frename_ind (MVL_IND_PEND *indCtrl)
  {
MVLAS_FRENAME_CTRL *frename_ctrl = &indCtrl->u.frename;

  /* Use the ANSI "rename" function if available on your OS.		*/
  /* Otherwise, use the appropriate function for your OS.		*/
  if (rename (frename_ctrl->curfilename, frename_ctrl->newfilename))
    {
    _mplas_err_resp (indCtrl, 11, 0);	/* File problem, other	*/
    }
  else
    mplas_frename_resp (indCtrl);    /* create response to confirm completion */
  }
#endif	/* #if (MMS_FRENAME_EN & RESP_EN)	*/
 
#if (MMS_OBTAINFILE_EN & RESP_EN)
/************************************************************************/
/*			u_mvl_obtfile_ind				*/
/************************************************************************/
ST_VOID u_mvl_obtfile_ind (MVL_IND_PEND *indCtrl)
  {
  mvlas_obtfile_resp (indCtrl);      /* indicate state machine starting */
  }
#endif	/* #if (MMS_OBTAINFILE_EN & RESP_EN)	*/

#if (MMS_JINIT_EN & RESP_EN)
/************************************************************************/
/*			u_mvl_jinit_ind					*/
/************************************************************************/
ST_VOID u_mvl_jinit_ind (MVL_IND_PEND *indCtrl)
  {
  mvl61850_log_jinit_resp (indCtrl);	/* Call Virtual Machine response funct.*/
  }
#endif	/* #if (MMS_JINIT_EN & RESP_EN)	*/

#if (MMS_JREAD_EN & RESP_EN)
/************************************************************************/
/*			u_mvl_jread_ind					*/
/* For the linked list Journal model, this function can simply call	*/
/* "mvlas_jread_resp" to automatically process the indication and send	*/
/* the response. If a different Journal model is used, the MVL code	*/
/* must be modified or replaced (see "mvl_ijou.c").			*/
/************************************************************************/
ST_VOID u_mvl_jread_ind (MVL_IND_PEND *indCtrl)
  {
  mvlas_jread_resp (indCtrl);	/* Call Virtual Machine response funct.	*/
  }
#endif	/* #if (MMS_JREAD_EN & RESP_EN)	*/

#if (MMS_JSTAT_EN & RESP_EN)
/************************************************************************/
/*			u_mvl_jstat_ind					*/
/************************************************************************/
ST_VOID u_mvl_jstat_ind (MVL_IND_PEND *indCtrl)
  {
JSTAT_REQ_INFO *req_info = indCtrl->u.jstat.req_info;
JSTAT_RESP_INFO resp_info;
MVL_JOURNAL_CTRL *journal;

  /* Find the Journal.	*/
  journal = mvl_vmd_find_jou (&mvl_vmd, &req_info->jou_name, indCtrl->event->net_info);

  if (journal == NULL)
    {
    /* NOTE: Must log BEFORE _mplas_err_resp because it frees indCtrl.	*/
    MVL_LOG_NERR1 ("ReportJournalStatus - could not find journal '%s'", 
		req_info->jou_name.obj_name.vmd_spec);
    _mplas_err_resp (indCtrl, MMS_ERRCLASS_DEFINITION, MMS_ERRCODE_OBJ_UNDEFINED);
    return;
    }  

  resp_info.cur_entries = journal->numEntries;
  resp_info.mms_deletable = journal->mms_deletable;

  indCtrl->u.jstat.resp_info = &resp_info;
  mplas_jstat_resp (indCtrl);
  }
#endif	/* #if (MMS_JSTAT_EN & RESP_EN)	*/

#if defined(SMPVAL_SUPPORT)
/************************************************************************/
/*			init_etype_info					*/
/* Use info from the SMV element to initialize ETYPE_INFO structure.	*/
/************************************************************************/
static ST_VOID init_etype_info (SCL_SMV *scl_smv,
	ETYPE_INFO *etypeInfo)
  {
  if (scl_smv)
    {
    /* Check VLANPRI, VLANID, then use them to compute tci.	*/
    if (scl_smv->VLANPRI > 7)	/* Must fit in 3 bits	*/
      {
      SLOGALWAYS1 ("ERROR: VLAN-PRIORITY=%d out of range. Using 0.", scl_smv->VLANPRI);
      scl_smv->VLANPRI = 0;
      }
    if (scl_smv->VLANID > 4095)	/* Must fit in 12 bits	*/
      {
      SLOGALWAYS1 ("ERROR: VLAN-ID=%d out of range. Using 1.", scl_smv->VLANID);
      scl_smv->VLANID = 1;
      }
    etypeInfo->appID = scl_smv->APPID;	/* should be 0x4000-0x7FFF	*/
    /* Compute TCI from VLAN-PRIORITY and VLAN-ID.		*/
    /* CFI (bit 12) in TCI is always 0, so do nothing with it.	*/
    etypeInfo->tci =  scl_smv->VLANPRI << 13;	/* this also forces bit 12 to be 0*/
    etypeInfo->tci |= scl_smv->VLANID;
    }
  else
    {
    /* SMV not found. Set default values.	*/
    /* according to 61850-9-2, APPID must be between 0x4000 and 0x7FFF	*/
    /* Default value if not configured is 0x4000.			*/
    etypeInfo->appID   = 0x4000;
    etypeInfo->tci     = ETYPE_TCI_SV;
    }
  etypeInfo->etypeID = ETYPE_TYPE_SV;	/* always this value for SV	*/
  }
#endif

/************************************************************************/
/*			mvl61850_rpt_init_scan_done			*/
/* Callback function called when scan completes. Scan was started by	*/
/* "mvl61850_rpt_data_init" (BELOW).					*/
/* This does NOT send a report. It just frees up the scan resources.	*/
/************************************************************************/
static ST_RET mvl61850_rpt_init_scan_done (MVL_IND_PEND *indCtrl)
  {
  mvlu_integrity_scan_destroy (indCtrl);	/* destroy temporary struct*/
  return (SD_SUCCESS);
  }
/************************************************************************/
/*			mvl61850_rpt_data_init				*/
/* Loop through all Report Control Blocks and start a scan to get	*/
/* initial values of all Report dataset members.			*/
/* NOTE: without this function, all data is initially 0, so the first	*/
/*       report scan usually generates an invalid data change report.	*/
/************************************************************************/
static ST_VOID mvl61850_rpt_data_init (ST_VOID)
  {
MVLU_RPT_CTRL *rptCtrl;
  /* Loop through list of IEC 61850 RCBs (URCB or BRCB).	*/
  for (rptCtrl = mvl61850_rpt_ctrl_list;
       rptCtrl != NULL;
       rptCtrl = (MVLU_RPT_CTRL *) list_get_next (mvl61850_rpt_ctrl_list, rptCtrl))
    {
    /* Start a scan to initialize "last_data" for each variable.	*/
    /* This is NOT a GI scan but "mvlu_gi_scan_va_done" (in library)	*/
    /* does what we need.						*/
    mvlu_integrity_scan_read (&rptCtrl->only_client,
         mvlu_gi_scan_va_done,	/* this sets "last_data"	*/
         mvl61850_rpt_init_scan_done);	/* this just cleans up	*/
    }
  }

