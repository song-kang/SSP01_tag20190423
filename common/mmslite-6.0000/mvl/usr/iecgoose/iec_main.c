/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	1986 - 2006, All Rights Reserved		        */
/*									*/
/* MODULE NAME : iec_main.c    						*/
/* PRODUCT(S)  : MMSEASE-LITE						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*	NONE								*/
/*									*/
/* NOTE: define USE_FRAMEWORK_THREADS in makefile or "iec_demo.h" to	*/
/*	enable threads. Must be done from makefile or include files so	*/
/*	that all source files compiled with the same define.		*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 01/11/12  JRB	   Use new scl_info->serverHead created by parser.*/
/*			   Generate RETRANS_CURVE from SCL MinTime,	*/
/*			   MaxTime & pass to iecGoosePubCreate.		*/
/*			   Use strncat_maxstrlen to create gcRef, etc (safer)*/
/* 03/25/11  JRB	   u_set_all_leaf..: add arg.			*/
/* 06/18/10  JRB	   Replace mvl_start_acse & mvl_end_acse with	*/
/*			   clnp_init & clnp_end (avoids unnecessary	*/
/*			   initialization and binding).			*/
/* 06/19/08  GLB     86    Added slog_start                             */
/* 08/01/08  JRB     85    Destroy publishers & subscribers in main.	*/
/*			   Call mvl_vmd_destroy before exit.		*/
/* 07/08/08  JRB     84    Chk VLANPRI, VLANID, then use to compute tci.*/
/* 06/20/08  JRB     83    Use logcfgx to configure IEC_GOOSE log masks.*/
/* 05/14/08  GLB     82    Set slog_ipc_cmd_fun function pointer.	*/
/* 07/20/07  JRB     81    Move scl_gse_find to sclproc.c (to share).	*/
/* 01/16/07  JRB     80    read_scl_file: init scl_info to all 0.	*/
/* 11/14/06  JRB     79    Fix warnings. Del unused global variables.	*/
/* 10/30/06  JRB     78    Chg args to "scl2_*_create_all" functions.	*/
/* 08/09/06  JRB     77    Use SCL to configure.			*/
/*			   Call mvl_end_acse before exit.		*/
/*			   Move mutex create/destroy to main.		*/
/*			   Add u_mvl_scl_set_initial_value callback.	*/
/* 09/09/05  JRB     76    Don't print BUILD_NUM.			*/
/* 08/01/05  JRB     75    Del unused global vars.			*/
/* 07/08/05  JRB     74    Add sample "u_mvl61850_ctl_oper_*" functs.	*/
/*			   Don't print "Check log file". May not be one.*/
/* 06/07/05  EJV     16    Set gsLogMaskMapCtrl if S_MT_SUPPORT defined	*/
/* 05/24/05  EJV     15    Added logCfgAddMaskGroup (&xxxLogMaskMapCtrl)*/
/*			   Use logcfgx_ex to parse logging config file.	*/
/* 02/08/05  JRB     14    Del unneeded call to add_gsse_goose...	*/
/*			   Del unneeded call to set_multicast_filters,	*/
/*			   called during subscribe/unsubscribe.		*/
/*			   Log memory usage at exit.			*/
/* 08/13/04  JRB     13    Del unneeded includes.			*/
/* 07/08/04  JRB     12    Del mvl_local_ar_name, mvl_local_cl_ar_name.	*/
/*			   Del use of COACSE define.			*/
/*			   Del unused indication handling functions.	*/
/*			   Use new asn1_convert_timet_to_btime6.	*/
/* 01/06/04  JRB     11    Del unused vars pbro_di, lun0_di.		*/
/* 12/11/03  JRB     10    DON'T use MVLLOG_REQ/RESP/IND/CONF, they	*/
/*			   don't exist.					*/
/* 11/06/03  JRB     09    Chg main return from void to "int".		*/
/*			   Del unused stuff.				*/
/* 09/23/03  EJV     08    Del old password auth (OBSOLETE_ACSE_AUTH)	*/
/*			   Rpl connect_ind_fun with u_mvl_connect_ind_ex*/
/* 04/04/03  JRB     07    Del unneeded u_mvlu_rpt_scan_done.		*/
/* 01/15/03  ASK     06    Del call to clnp_read_thread_start, del      */
/*			   unused functions. More cleanup.		*/
/* 12/19/02  ASK     05    Update mvlu_create_rpt_ctrl, add 		*/
/*			   u_mvlu_rpt_time_get from uca_srvr		*/
/*			   Del setting of OBSOLETE *_typeId globals.	*/
/* 12/10/02  ASK     04    Misc cleanup					*/
/* 10/18/02  ASK     03    Changed to OBSOLETE_ACSE_AUTH to remove old  */
/*			   authentication sample code.			*/	 
/* 04/12/02  JRB     02    Use global mvl_cfg_info.			*/
/* 02/14/02  HSF     01    Created based on uca_srvr.c			*/
/************************************************************************/

/*#define HARD_CODED_CFG */

/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include <signal.h>

#include "mmsdefs.h"
#include "mms_pvar.h"
#include "mms_vvar.h"
#include "mms_pvmd.h"
#include "mms_pcon.h"

#include "mvl_acse.h"
#include "mvl_log.h"
#include "uca_obj.h"
#include "tp4api.h"

#include "goose.h"
#include "glbsem.h"
#include "iec_demo.h"	/* GOOSE Framework defines.	*/

#ifdef SMEM_ENABLE
#include "smem.h"
#endif

#include "scl.h"
#include "../scl_srvr/startup.h"	/* use startup_cfg_read from	*/
					/* scl_srvr sample		*/
#include "sx_arb.h"			/* for sxaTextToLocal		*/
#include "str_util.h"			/* for strncat_maxstrlen, etc.	*/

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/
/************************************************************************/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

#ifdef DEBUG_SISCO
LOGCFGX_VALUE_MAP iecGooseLogMaskMaps[] =
  {
    {"IEC_GOOSE_LOG_RETRANS",	IEC_GOOSE_LOG_RETRANS,	&iec_goose_usr_debug, _LOGCFG_DATATYPE_UINT_MASK, NULL, "Retransmit"},
    {"IEC_GOOSE_LOG_FLOW",	IEC_GOOSE_LOG_FLOW,	&iec_goose_usr_debug, _LOGCFG_DATATYPE_UINT_MASK, NULL, "Flow"},
    {"IEC_GOOSE_LOG_RX",	IEC_GOOSE_LOG_RX,	&iec_goose_usr_debug, _LOGCFG_DATATYPE_UINT_MASK, NULL, "Receive"},
    {"IEC_GOOSE_LOG_TX",	IEC_GOOSE_LOG_TX,	&iec_goose_usr_debug, _LOGCFG_DATATYPE_UINT_MASK, NULL, "Transmit"}
  };

LOGCFG_VALUE_GROUP iecGooseLogMaskMapCtrl =
  {
  {NULL,NULL},
  "GooseLogMasks",	/* Parent Tag	*/
  sizeof(iecGooseLogMaskMaps)/sizeof(LOGCFGX_VALUE_MAP),
  iecGooseLogMaskMaps
  };
#endif /* DEBUG_SISCO */

extern ST_VOID demo_init(ST_VOID);
/************************************************************************/

ST_RET read_log_cfg_file (ST_CHAR *filename);
ST_VOID wait_any_event (ST_LONG max_ms_delay);

/************************************************************************/
/************************************************************************/
/* Functions in this module						*/

/* Initialization */
static ST_VOID init_acse (ST_VOID);
static ST_VOID init_log_cfg (ST_VOID);
static ST_VOID init_mem (ST_VOID);

/* Connection Management */
static ST_VOID disc_ind_fun (MVL_NET_INFO *cc, ST_INT discType);

/* Misc. */
ST_VOID ctrlCfun (int);

/************************************************************************/

/* General program control variables */
ST_BOOLEAN doIt;

/************************************************************************/
/*			read_scl_file					*/
/* Read SCL file and create all objects.				*/
/************************************************************************/
ST_RET read_scl_file (SCL_INFO *scl_info, ST_CHAR *startup_cfg_filename)
  {
ST_RET ret;
STARTUP_CFG startup_cfg;	/* info from "startup.cfg" file		*/
ST_INT brcb_bufsize=1000000;

  /* Init struct to all 0, in case something fails without filling it in.*/
  memset (scl_info, 0, sizeof (SCL_INFO));

  ret = startup_cfg_read (startup_cfg_filename, &startup_cfg);

  if (ret == SD_SUCCESS)
    ret = scl_parse (startup_cfg.scl_filename,
         startup_cfg.ied_name,			/* IED name	*/
         startup_cfg.access_point_name,		/* AccessPoint	*/
         scl_info);

  /* Create types only if everything successful up to now.	*/
  if (ret == SD_SUCCESS)
    {
    ret = scl2_datatype_create_all (&mvl_vmd, scl_info, 14000,
               SD_FALSE,	/* type names not generated	*/
               NULL);		/* type name prefix (not used)	*/
    }

  /* Create variables only if everything successful up to now.	*/
  if (ret == SD_SUCCESS)
    ret = scl2_ld_create_all (&mvl_vmd, scl_info, startup_cfg.report_scan_rate, brcb_bufsize,
              SD_FALSE);	/* create "server" model	*/

  return (ret);
  }
/************************************************************************/
/*			retrans_curve_init				*/
/************************************************************************/
ST_RET retrans_curve_init (ST_UINT MinTime, ST_UINT MaxTime,
	RETRANS_CURVE *retrans_curve)	/* initialized by this function	*/
  {
ST_UINT tmp;
ST_UINT idx;

  if (MinTime == 0 || MaxTime <= MinTime)
    return (SD_FAILURE);	/* Can't generate curve from these values.	*/

  for (idx = 0, tmp = MinTime;	/* first entry is alway MinTime*/
       idx < MAX_NUM_RETRANS;
       idx++, tmp *= 2)
    {
    if (tmp < MaxTime)
      retrans_curve->retrans [idx] = tmp;
    else
      {
      retrans_curve->retrans [idx] = MaxTime;	/* last entry is always MaxTime*/
      break;	/* should normally stop here	*/
      }
    }
  /* If we looped to the end, the array is too small.	*/
  if (idx >= MAX_NUM_RETRANS)
    return (SD_FAILURE);

  retrans_curve->num_retrans = idx+1;
  return (SD_SUCCESS);	/* retrans_curve is initialized now.	*/
  }

/************************************************************************/
/*			iecGoosePubCreateAll				*/
/* Create all GOOSE publishers from SCL configuration information.	*/
/************************************************************************/
ST_RET iecGoosePubCreateAll (SCL_INFO *scl_info)
  {
SCL_SERVER *scl_server;
RETRANS_CURVE retrans_curve;	/* initialize & pass to iecGoosePubCreate*/
ST_RET retcode;
SCL_LD *scl_ld;
SCL_LN *scl_ln;
SCL_GCB *scl_gcb;	/* GOOSE control block info	*/
SCL_GSE *scl_gse;	/* GOOSE addressing info	*/
/* Temporary buffers to construct gcRef & datasetRef.	*/
ST_CHAR gcRef [MVL61850_MAX_OBJREF_LEN+1];
ST_CHAR datasetRef [MVL61850_MAX_OBJREF_LEN+1];
ST_UINT16 tci;		/* computed from VLAN info.	*/

  /* We loaded CID file, so assume there is exactly one server on the list.*/
  scl_server = scl_info->serverHead;

  for (scl_ld = (SCL_LD *) list_find_last ((DBL_LNK *) scl_server->ldHead);
       scl_ld != NULL;
       scl_ld = (SCL_LD *) list_find_prev ((DBL_LNK *) scl_server->ldHead, (DBL_LNK *) scl_ld))
    {
    for (scl_ln = (SCL_LN *) list_find_last ((DBL_LNK *) scl_ld->lnHead);
         scl_ln != NULL;
         scl_ln = (SCL_LN *) list_find_prev ((DBL_LNK *) scl_ld->lnHead, (DBL_LNK *) scl_ln))
      {
      for (scl_gcb = (SCL_GCB *) list_find_last ((DBL_LNK *) scl_ln->gcbHead);
           scl_gcb != NULL;
           scl_gcb = (SCL_GCB *) list_find_prev ((DBL_LNK *) scl_ln->gcbHead, (DBL_LNK *) scl_gcb))
        {
        if (scl_gcb->isGoose)
          {
          /* Construct gcRef, datasetRef from SCL info.	*/
	  strcpy (gcRef, scl_ld->domName);
          strncat_maxstrlen (gcRef, "/", MVL61850_MAX_OBJREF_LEN);
          strncat_maxstrlen (gcRef, scl_ln->varName, MVL61850_MAX_OBJREF_LEN);
          strncat_maxstrlen (gcRef, "$GO$", MVL61850_MAX_OBJREF_LEN);
          strncat_maxstrlen (gcRef, scl_gcb->name, MVL61850_MAX_OBJREF_LEN);

          strcpy (datasetRef, scl_ld->domName);
          strncat_maxstrlen (datasetRef, "/", MVL61850_MAX_OBJREF_LEN);
          strncat_maxstrlen (datasetRef, scl_ln->varName, MVL61850_MAX_OBJREF_LEN);
          strncat_maxstrlen (datasetRef, "$", MVL61850_MAX_OBJREF_LEN);
          strncat_maxstrlen (datasetRef, scl_gcb->datSet, MVL61850_MAX_OBJREF_LEN);

	  if ((scl_gse = scl_gse_find (scl_info, scl_server->iedName,
                         scl_server->apName, scl_ld->inst, scl_gcb->name)) == NULL)
            {
            SLOGALWAYS3 ("ERROR: unable to find addressing information for GSEControl '%s' in ldInst '%s' in IED '%s'",
                         scl_gcb->name, scl_ld->inst, scl_server->iedName);
            return (SD_FAILURE);
            }

          /* Check VLANPRI, VLANID, then use them to compute tci.	*/
          if (scl_gse->VLANPRI > 7)	/* Must fit in 3 bits	*/
            {
            SLOGALWAYS1 ("ERROR: VLAN-PRIORITY=%d out of range.", scl_gse->VLANPRI);
            return (SD_FAILURE);
            }
          if (scl_gse->VLANID > 4095)	/* Must fit in 12 bits	*/
            {
            SLOGALWAYS1 ("ERROR: VLAN-ID=%d out of range.", scl_gse->VLANID);
            return (SD_FAILURE);
            }
          /* Compute TCI from VLAN-PRIORITY and VLAN-ID.		*/
          /* CFI (bit 12) in TCI is always 0, so do nothing with it.	*/
          tci = scl_gse->VLANPRI << 13;	/* this also forces bit 12 to be 0*/
          tci |= scl_gse->VLANID;

          /* Generate Retransmission Curve using configured MinTime, MaxTime.*/
          retcode = retrans_curve_init (scl_gse->MinTime, scl_gse->MaxTime, &retrans_curve);
	  if (retcode)
	    retrans_curve_init (4, 60000, &retrans_curve);	/* default curve. should never fail*/

          if (iecGoosePubCreate(
                scl_gse->MAC,
                gcRef,
                datasetRef,
                scl_gcb->appID,		/* this is string	*/
                scl_gcb->confRev,
                SD_TRUE,
                tci,			/* computed above */
                ETYPE_TYPE_GOOSE,	/* EtypeID (fixed value for GOOSE)*/
                (ST_UINT16) scl_gse->APPID,
                &retrans_curve)==NULL)
            {
            SLOGALWAYS3 ("ERROR: iecGoosePubCreate failed for GSEControl '%s' in ldInst '%s' in IED '%s'",
                         scl_gcb->name, scl_ld->inst, scl_server->iedName);
            return (SD_FAILURE);	/* stop on first error	*/
            }
          }
        else
          {	/* This is GSSE	*/
          SLOGALWAYS0 ("ERROR: GSSE not supported by this application");
          }
        }
      }
    }
  return (SD_SUCCESS);	/* all were successful	*/	
  }
/************************************************************************/
/*			iecGooseSubscribeAll				*/
/* Create all GOOSE subscribers from SCL configuration information.	*/
/************************************************************************/
ST_RET iecGooseSubscribeAll (SCL_INFO *scl_info)
  {
SCL_SERVER *scl_server;
SCL_LD *scl_ld;
SCL_LN *scl_ln;
SCL_GCB *scl_gcb;	/* GOOSE control block info	*/
SCL_GSE *scl_gse;	/* GOOSE addressing info	*/
/* Temporary buffers to construct gcRef & datasetRef.	*/
ST_CHAR gcRef [MVL61850_MAX_OBJREF_LEN+1];
ST_CHAR datasetRef [MVL61850_MAX_OBJREF_LEN+1];

  /* We loaded CID file, so assume there is exactly one server on the list.*/
  scl_server = scl_info->serverHead;

  for (scl_ld = (SCL_LD *) list_find_last ((DBL_LNK *) scl_server->ldHead);
       scl_ld != NULL;
       scl_ld = (SCL_LD *) list_find_prev ((DBL_LNK *) scl_server->ldHead, (DBL_LNK *) scl_ld))
    {
    for (scl_ln = (SCL_LN *) list_find_last ((DBL_LNK *) scl_ld->lnHead);
         scl_ln != NULL;
         scl_ln = (SCL_LN *) list_find_prev ((DBL_LNK *) scl_ld->lnHead, (DBL_LNK *) scl_ln))
      {
      for (scl_gcb = (SCL_GCB *) list_find_last ((DBL_LNK *) scl_ln->gcbHead);
           scl_gcb != NULL;
           scl_gcb = (SCL_GCB *) list_find_prev ((DBL_LNK *) scl_ln->gcbHead, (DBL_LNK *) scl_gcb))
        {
        if (scl_gcb->isGoose)
          {
          /* Construct gcRef, datasetRef from SCL info.	*/
          strcpy (gcRef, scl_ld->domName);
          strncat_maxstrlen (gcRef, "/", MVL61850_MAX_OBJREF_LEN);
          strncat_maxstrlen (gcRef, scl_ln->varName, MVL61850_MAX_OBJREF_LEN);
          strncat_maxstrlen (gcRef, "$GO$", MVL61850_MAX_OBJREF_LEN);
          strncat_maxstrlen (gcRef, scl_gcb->name, MVL61850_MAX_OBJREF_LEN);

          strcpy (datasetRef, scl_ld->domName);
          strncat_maxstrlen (datasetRef, "/", MVL61850_MAX_OBJREF_LEN);
          strncat_maxstrlen (datasetRef, scl_ln->varName, MVL61850_MAX_OBJREF_LEN);
          strncat_maxstrlen (datasetRef, "$", MVL61850_MAX_OBJREF_LEN);
          strncat_maxstrlen (datasetRef, scl_gcb->datSet, MVL61850_MAX_OBJREF_LEN);

	  if ((scl_gse = scl_gse_find (scl_info, scl_server->iedName,
                         scl_server->apName, scl_ld->inst, scl_gcb->name)) == NULL)
            {
            SLOGALWAYS3 ("ERROR: unable to find addressing information for GSEControl '%s' in ldInst '%s' in IED '%s'",
                         scl_gcb->name, scl_ld->inst, scl_server->iedName);
            return (SD_FAILURE);
            }

          if (create_subscription_from_nvl(
                datasetRef, 
                gcRef,
                scl_gcb->appID,
                scl_gcb->confRev,
                GOOSE_DEC_MODE_IMMEDIATE,
                scl_gse->MAC) == NULL)	/* Use MAC from SCL file	*/
            return (SD_FAILURE);	/* stop on first error	*/
          }
        else
          {	/* This is GSSE	*/
          SLOGALWAYS0 ("ERROR: GSSE not supported by this application");
          }
        }
      }
    }
  return (SD_SUCCESS);	/* all were successful	*/	
  }

/************************************************************************/
/*			u_set_all_leaf_functions			*/
/* Use only mvlu_null_rd_ind & mvlu_null_wr_ind leaf functions.		*/
/************************************************************************/
ST_RET u_set_all_leaf_functions (RUNTIME_CTRL *rt_ctrl, SCL_LNTYPE *scl_lntype)
  {
RUNTIME_TYPE *rt_type;
/* These are indices into the arrays of Read & Write leaf function pointers.*/
ST_RTINT idx_mvlu_null_rd_ind;
ST_RTINT idx_mvlu_null_wr_ind;
ST_INT j;
ST_RET retcode = SD_SUCCESS;

  /* Find the customized rd and wr leaf functions to use for this application.*/
  idx_mvlu_null_rd_ind = mvlu_find_rd_ind_fun_index ("mvlu_null_rd_ind");
  idx_mvlu_null_wr_ind = mvlu_find_wr_ind_fun_index ("mvlu_null_wr_ind");
  if (   idx_mvlu_null_rd_ind >= 0
      && idx_mvlu_null_wr_ind >= 0
      )
    {
    /* Set the leaf function index for every leaf.		*/
    for (j = 0, rt_type = rt_ctrl->rt_first;
         j < rt_ctrl->rt_num;
         j++, rt_type++)
      {
      rt_type->mvluTypeInfo.rdIndFunIndex = idx_mvlu_null_rd_ind;
      rt_type->mvluTypeInfo.wrIndFunIndex = idx_mvlu_null_wr_ind;
      }
    }
  else
    {
    MVL_LOG_ERR0 ("u_set_all_leaf_functions: cannot find one or more read or write leaf functions");
    retcode = SD_FAILURE;
    }

  return (retcode);
  }
/************************************************************************/
/************************************************************************/
ST_RET u_mvlu_resolve_leaf_ref (ST_CHAR *leafName, ST_INT *setFlagsIo, 
				ST_CHAR *refText, ST_RTREF *refOut)
  {
  /* currently never called, but linker needs it.*/
  assert (0);
  return (SD_FAILURE);
  }

/************************************************************************/
/************************************************************************/
/*                       main						*/
/************************************************************************/

#if defined (VXWORKS)	/* Just make this look like "main" call.	*/
int mmsserver ()
  {
#else	/* !VXWORKS	*/  
int main ()
  {
ST_CHAR *marker;	/* last buffer allocated at end of startup	*/
#ifdef SMEM_ENABLE
ST_RET ret;
ST_INT j;
#endif
SCL_INFO scl_info;	/* struct to store all info from SCL file	*/
ST_RET retcode;

#endif	/* !VXWORKS	*/  
  setbuf (stdout, NULL);    /* do not buffer the output to stdout   */
  setbuf (stderr, NULL);    /* do not buffer the output to stderr   */

  printf ("%s Version %s\n", MMSLITE_NAME, MMSLITE_VERSION);
  puts (MMSLITE_COPYRIGHT);

#if defined(NO_GLB_VAR_INIT)
  mvl_init_glb_vars ();
#endif
  init_mem ();		/* Set up memory allocation tools 		*/
  init_log_cfg ();	/* Set up logging subsystem			*/

  SLOGALWAYS2 ("%s Version %s", MMSLITE_NAME, MMSLITE_VERSION);

#ifdef SMEM_ENABLE
  init_mem_chk();	/* MUST call before smemcfgx or any alloc funct.*/
  ret = smemcfgx ("smemcfg.xml");
  if (ret != SD_SUCCESS)
    {
    printf ("\n Parsing of Smem configuration failed, err = 0x%X. Check configuration.\n", ret );
    exit (1);
    }
  smem_debug_sel = SMEM_LOG_ERR | SMEM_LOG_NERR;
#endif

#if defined(UCA_SMP)
  smp_init ();
#endif

  doIt = SD_TRUE;

#if defined(USE_FRAMEWORK_THREADS)
  /* If threads used, create semaphores	*/
  gs_mutex_create (&pub_ctrl.pub_mutex);
  gs_mutex_create (&sub_ctrl.sub_mutex);
#endif

  SLOGALWAYS0 ("Initializing ...");

  mvl_max_dyn.types = 1000;	/* before mvl_init_mms_objs to override default.*/

  /* mvl_init_mms_objs must be called before any dynamic object creation*/
  /* It is called automatically by mvl_start_acse, but it may often be	*/
  /* better to init all objects before mvl_start_acse.			*/
  mvl_init_mms_objs ();

  init_acse ();		/* Start the lower layer subsystem		*/

  marker = chk_malloc (1);		/* Marker			*/

/* Set the ^c catcher */
  signal (SIGINT, ctrlCfun);

  /* Read SCL file and create all objects.	*/
  retcode = read_scl_file (&scl_info, "startup.cfg");

  /* Create GOOSE Publishers and Subscribers based on SCL config.	*/
  /* In "demo_init", user chooses whether to Publish or Subscribe.	*/

  /* Create GOOSE Publishers	*/
  if (retcode == SD_SUCCESS)
    retcode = iecGoosePubCreateAll (&scl_info);

  /* Create GOOSE Subscribers	*/
  if (retcode == SD_SUCCESS)
    retcode = iecGooseSubscribeAll (&scl_info);

  /* SCL_INFO structure should not be needed anymore. This function	*/
  /* frees all memory allocated for this structure.			*/
  scl_info_destroy (&scl_info);

  /* NOTE: there is no special mapping of data in this sample. If mapping*/
  /*       is desired, "datamap_cfg_read" should be called here.	*/

  if (retcode)
    {
    printf ("\n ERROR: Configuration FAILED.");
    exit (1);
    }

  demo_init();				/*call to IEC GOOSE Framework	*/

  /* Destroy GOOSE Publishers	*/
  while (pub_ctrl.pub_list)
    {
    if (iecGoosePubDestroy (pub_ctrl.pub_list) !=SD_SUCCESS)
      printf ("\nError destroying GOOSE publisher.");
    }

  /* Destroy GOOSE Subscribers	*/
  while (sub_ctrl.sub_list)
    {
    if (iecGooseUnSubscribe (sub_ctrl.sub_list) !=SD_SUCCESS)
      printf ("\nError destroying subscriber.");
    }

  /* clnp_init called at startup, so must call clnp_end to clean up.*/
  clnp_end ();

  mvl_vmd_destroy (&mvl_vmd);	/* destroy global VMD	*/

#if defined(USE_FRAMEWORK_THREADS)
  /* If threads used, destroy semaphores	*/
  gs_mutex_destroy (&pub_ctrl.pub_mutex);
  gs_mutex_destroy (&sub_ctrl.sub_mutex);
#endif

  /* Log only buffers allocated AFTER startup & not yet freed.	*/
  dyn_mem_ptr_status2 (marker);
#if 0	/* add this only if problems detected	*/
  dyn_mem_ptr_status ();	/* Log ALL buffers		*/
#endif
  return (0);
  }

/************************************************************************/
/************************************************************************/
/* GOOSE FUNCTIONS							*/

/************************************************************************/
/*			u_mmsl_goose_received 				*/
/************************************************************************/

ST_VOID u_mmsl_goose_received (GOOSE_INFO *goose_info)
  {
static ST_LONG rxCount;

  ++rxCount;
  if ((rxCount % 100) == 0)
    printf ("\r Received Goose Message %ld", rxCount);
  }

/************************************************************************/
/*			u_mvlu_rpt_time_get				*/
/* UCA/IEC reporting callback function to get accurate rpt time.	*/
/* If UCA/IEC Reports are enabled, user must provide this function.	*/
/************************************************************************/
ST_VOID u_mvlu_rpt_time_get (MMS_BTIME6 *TimeOfEntry)
  {
#if 1	/* This is a very simple example using the standard "time" fct.	*/
  time_t curTime;

  curTime = time (NULL);
  asn1_convert_timet_to_btime6 (curTime, TimeOfEntry);

#else	/* This is simplest possible code if user doesn't care about time.*/
  TimeOfEntry->ms = 0;
  TimeOfEntry->day = 0;
#endif
  return;
  }

/************************************************************************/
/************************************************************************/
/* CONNECTION MANAGEMENT FUNCTIONS					*/


/************************************************************************/
/*			u_mvl_connect_ind_ex				*/
/*----------------------------------------------------------------------*/
/* This function is called when a remote node has connected to us. We	*/
/* can look at the assReqInfo to see who it is (assuming that the 	*/
/* AP-Titles are used), or at the cc->rem_init_info to see initiate 	*/
/* request parameters.							*/
/************************************************************************/
extern ST_ACSE_AUTH u_mvl_connect_ind_ex (MVL_NET_INFO *cc, INIT_INFO *init_info,
                   ACSE_AUTH_INFO *req_auth_info, ACSE_AUTH_INFO *rsp_auth_info)
  {
  /* this ind function should never be called */
  assert (0);
  return (ACSE_DIAG_NO_REASON);
  }

/************************************************************************/
/*			disc_ind_fun					*/
/************************************************************************/
/* This function is called when connection is terminated.		*/

static ST_VOID disc_ind_fun (MVL_NET_INFO *cc, ST_INT discType)
  {
  mvlu_clr_pend_sbo (cc);
  }

/************************************************************************/
/************************************************************************/
/* INITIALIZATION FUNCTIONS 						*/
/************************************************************************/

#if defined(HARD_CODED_CFG)

DIB_ENTRY localDibTable[] =
  {
    {
    0, 		/* reserved		*/
    "local1",	/* name[65]		*/
    SD_TRUE,	/* local		*/
    SD_TRUE,	/* AP_title_pres	*/
      {		/* AP_title		*/
      4,	/* num_comps		*/
      1, 3, 9999, 106
      },
    SD_FALSE,	/* AP_inv_id_pres	*/
    0,		/* AP_invoke_id		*/
    SD_TRUE,	/* AE_qual_pres		*/
    33,		/* AE_qual		*/
    SD_FALSE,	/* AE_inv_id_pres	*/
    0,		/* AE_invoke_id		*/
#if defined(REDUCED_STACK)
    106		/* adlcAddr		*/
#else
      {				/* PRES_ADDR				*/
      4,			/* P-selector length			*/
      "\x00\x00\x00\x01",	/* P-selector				*/
      2,			/* S-selector length			*/
      "\x00\x01",		/* S-selector				*/
      TP_TYPE_TP4,		/* Transport type			*/
      2,			/* T-selector length			*/
      "\x00\x01",		/* T-selector				*/
      0,			/* Network address length		*/
      ""			/* Network address (ignored on local)	*/
      }
#endif
    }
  };
#endif

/************************************************************************/
/*				init_acse				*/
/************************************************************************/

static ST_VOID init_acse ()
  {
ST_RET ret;
MVL_CFG_INFO mvlCfg;

/* We want to know about connection activity				*/
  u_mvl_disc_ind_fun     = disc_ind_fun;

#if defined(HARD_CODED_CFG)
  num_loc_dib_entries = sizeof(localDibTable)/sizeof(DIB_ENTRY);
  loc_dib_table = localDibTable;
  num_rem_dib_entries = 0;

/* Only call config functions necessary for stack being used.	*/
#pragma message("Please select configuration functions from set below ...")
Please remove this line when you have selected your config functions ...
#if 0
  adlcConfigure();	/* TRIM-7(over ADLC)/REDUCED STACK	*/
  tp4_config();		/* OSI/RFC1006/TRIM-7			*/
  clnp_config();	/* OSI/TRIM-7				*/
#endif
#else
#if defined(ADLC_SLAVE) || defined (ADLC_MASTER)	/* Must be Reduced Stack or Trim-7*/
  ret = init_dirser ();			/* Use old CFG file.	*/
#else
  ret = osicfgx ("osicfg.xml", &mvlCfg);	/* This fills in mvlCfg	*/
#endif
  if (ret != SD_SUCCESS)
    {
    printf ("Stack configuration failed, err = 0x%X. Check configuration.\n", ret );
    exit (1);
    }
#endif  /* end #if defined(HARD_CODED_CFG) */

  /* MVL services are not needed in this sample, so we can call		*/
  /* clnp_init instead of mvl_start_acse here. This requires much less	*/
  /* code and avoids unnecessary "binding".				*/
  ret = clnp_init (1200);
  if (ret != SD_SUCCESS)
    {
    printf ("clnp_init failed, err = %d.\n", ret );
    exit (1);
    }
  }

/************************************************************************/
/*			init_log_cfg					*/
/************************************************************************/

static ST_VOID init_log_cfg (ST_VOID)
  {
#ifdef DEBUG_SISCO

#if defined (HARD_CODED_CFG)
/* Use File logging							*/
  sLogCtrl->logCtrl = LOG_FILE_EN;

/* Use time/date time log						*/
  sLogCtrl->logCtrl |= LOG_TIME_EN;

/* File Logging Control defaults 					*/
  sLogCtrl->fc.fileName = "uca_srvr.log";
  sLogCtrl->fc.maxSize = 1000000L;
  sLogCtrl->fc.ctrl = (FIL_CTRL_WIPE_EN | 
                 FIL_CTRL_WRAP_EN | 
		 FIL_CTRL_NO_APPEND |
                 FIL_CTRL_MSG_HDR_EN);


#if 1
  asn1_debug_sel |= ASN1_LOG_ERR;
  asn1_debug_sel |= ASN1_LOG_NERR;
  asn1_debug_sel |= ASN1_LOG_DEC;
  asn1_debug_sel |= ASN1_LOG_ENC;
#endif

  mms_debug_sel |= MMS_LOG_NERR;
#if 0
  mms_debug_sel |= MMS_LOG_PDU;
  mms_debug_sel |= MMS_LOG_USR_CONF;
  mms_debug_sel |= MMS_LOG_USR_IND;
#endif

  mvl_debug_sel |= MVLLOG_ERR;
  mvl_debug_sel |= MVLLOG_NERR;
#if 0
  mvl_debug_sel |= MVLLOG_TIMING;
  mvl_debug_sel |= MVLLOG_ACSE;
  mvl_debug_sel |= MVLLOG_ACSEDATA;
  mvl_debug_sel |= MVLULOG_FLOW;
#endif

  acse_debug_sel |= ACSE_LOG_ERR;
#if 0
  acse_debug_sel |= ACSE_LOG_ENC;
  acse_debug_sel |= ACSE_LOG_DEC;
  acse_debug_sel |= COPP_LOG_ERR;
  acse_debug_sel |= COPP_LOG_DEC;
  acse_debug_sel |= COPP_LOG_DEC_HEX;
  acse_debug_sel |= COPP_LOG_ENC;
  acse_debug_sel |= COPP_LOG_ENC_HEX;
  acse_debug_sel |= COSP_LOG_ERR;
  acse_debug_sel |= COSP_LOG_DEC;
  acse_debug_sel |= COSP_LOG_DEC_HEX;
  acse_debug_sel |= COSP_LOG_ENC;
  acse_debug_sel |= COSP_LOG_ENC_HEX;
#endif


  tp4_debug_sel |= TP4_LOG_ERR;
#if 0
  tp4_debug_sel |= TP4_LOG_FLOWUP;
  tp4_debug_sel |= TP4_LOG_FLOWDOWN;
#endif

#if 0
  clnp_debug_sel |= CLNP_LOG_ERR;
  clnp_debug_sel |= CLNP_LOG_NERR;
  clnp_debug_sel |= CLNP_LOG_REQ;
  clnp_debug_sel |= CLNP_LOG_IND;
  clnp_debug_sel |= CLNP_LOG_ENC_DEC;
  clnp_debug_sel |= CLNP_LOG_LLC_ENC_DEC;
  clnp_debug_sel |= CLSNS_LOG_REQ;
  clnp_debug_sel |= CLSNS_LOG_IND;
#endif

#if defined(UCA_SMP)
  smp_debug_sel |= SMP_LOG_ERR;
  smp_debug_sel |= SMP_LOG_IND;
  smp_debug_sel |= SMP_LOG_REQ;
#endif

  chk_debug_en |= MEM_LOG_ERR;

#if 0
  chk_debug_en |= MEM_LOG_CALLOC;
  chk_debug_en |= MEM_LOG_MALLOC;
  chk_debug_en |= MEM_LOG_REALLOC;
  chk_debug_en |= MEM_LOG_FREE;
#endif

#else	/* !defined (HARD_CODED_CFG)	*/
#if defined(ADLC_SLAVE) || defined (ADLC_MASTER)	/* Must be Reduced Stack or Trim-7*/
  if (read_log_cfg_file ("mms_log.cfg"))	/* Use old CFG file.	*/
#else
#if defined(S_SEC_ENABLED)
  logCfgAddMaskGroup (&secLogMaskMapCtrl);
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
  /* Special logging only for this application	*/
  logCfgAddMaskGroup (&iecGooseLogMaskMapCtrl);	/* GOOSE logging	*/
  
  /* At initialization, install a SLOGIPC command handler. The       */
  /* build in SLOGIPC handler just receives the command and put's    */
  /* on a list to be handled by the application at it's leisure ...  */
  sLogCtrl->ipc.slog_ipc_cmd_fun = slog_ipc_std_cmd_fun;

  if (logcfgx_ex (sLogCtrl, "logcfg.xml", NULL, SD_FALSE, SD_FALSE) != SD_SUCCESS)
#endif
    {
    printf ("\n Parsing of 'logging' configuration file failed.");
    if (sLogCtrl->fc.fileName)
      printf ("\n Check log file '%s'.", sLogCtrl->fc.fileName);
    exit (5);
    }
#endif	/* !defined (HARD_CODED_CFG)	*/

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

#ifdef SMEM_ENABLE
/************************************************************************/
/*			u_smem_get_pool_params				*/
/************************************************************************/

ST_VOID  u_smem_get_pool_params ( SMEM_CONTEXT *contextName,
                                  ST_UINT8 *numBuf, ST_UINT *bufSize,
			          ST_BOOLEAN *autoClone,
			          ST_INT *maxClones,
			          ST_CHAR **poolName)
  {
  *bufSize = 9999;
  *numBuf = 9;
  *autoClone = SD_FALSE;
  *maxClones = 0;
  *poolName = NULL;		/* pool name not very useful.	*/
  SLOGALWAYS0 ("CALLBACK: u_smem_get_pool_params");
  }

/************************************************************************/
/*			u_smem_need_buffers				*/
/************************************************************************/

SMEM_POOL_CTRL *u_smem_need_buffers (SMEM_CONTEXT *smemContext, 
				     ST_UINT8 numBuf, ST_UINT bufSize)
  {
SMEM_POOL_CTRL *newSc;

  /* numBuf = # of bufs in last pool created for this bufSize  OR	*/
  /*          0 if this is the first pool created for this bufSize.	*/
  /* If numBuf = 0, then the user must choose an appropriate value.	*/
  if (numBuf == 0)
    numBuf = 10;	/* choose a value	*/
  if (bufSize == 0)
    bufSize = 1000;	/* choose a value	*/

  newSc = smem_add_pool (smemContext, numBuf, bufSize,
                         SD_FALSE,	/* autoClone	*/
                         0,		/* maxClones	*/
                         NULL);		/* poolName	*/
  return (newSc);
  }
#endif	/* SMEM_ENABLE	*/

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
/*			u_mvl61850_ctl_oper_begin			*/
/* Sample user callback function.					*/
/************************************************************************/
ST_VOID u_mvl61850_ctl_oper_begin (ST_CHAR *oper_ref)
  {
  }
/************************************************************************/
/*			u_mvl61850_ctl_oper_end				*/
/* Sample user callback function.					*/
/* NOTE: see scl_srvr.c for a better sample function.			*/
/************************************************************************/
ST_VOID u_mvl61850_ctl_oper_end (MVL_NET_INFO *net_info, ST_CHAR *oper_ref, MVL_VAR_ASSOC *base_var)
  {
  }

/************************************************************************/
/*			u_mvl_scl_set_initial_value			*/
/* Called during SCL processing to convert initial value text to data.	*/
/************************************************************************/
ST_RET u_mvl_scl_set_initial_value (SCL2_IV_TRANSLATE_CTRL *translate)
  {
  /* Just do the basic text conversion.	*/
  return (sxaTextToLocal (translate->valText, translate->dest,
          translate->numRt, translate->rtHead));
  }

