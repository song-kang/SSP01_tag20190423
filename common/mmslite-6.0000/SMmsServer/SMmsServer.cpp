/**
*
* 文 件 名 : SMmsClient.cpp
* 创建日期 : 2015-9-16 19:22
* 作    者 : 邵凯田(skt001@163.com)
* 修改日期 : $Date: $
* 当前版本 : $Revision: $
* 功能描述 : sbase-MMS客户端封装类
* 修改记录 : 
*            $Log: $
*
* Ver  Date        Author  Comments
* ---  ----------  ------  -------------------------------------------
* 001	2015-9-16	邵凯田　创建文件
*
**/

#include "SMmsServer.h"
#include "SApplication.h"
#include <vector>


#ifdef __cplusplus
extern "C" {
#endif


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

#ifdef SMEM_ENABLE
#include "smem.h"
#endif
#include "scl.h"	/* SCL file processing structs & functions	*/
#include "startup.h"	/* struct & function to parse "startup.cfg"	*/
#include "usermap.h"	/* for DATA_MAP_HEAD, datamap_cfg_read, etc.	*/
#include "sx_arb.h"			/* for sxaTextToLocal		*/
#include "str_util.h"	/* for strncat_maxstrlen, etc.			*/

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

SMmsServer *g_MmsServer_ptr = NULL;
SString m_sBindIPAddr;

#define SCL_PARSE_MODE_CID	0	/* default SCL parse mode	*/
#define SCL_PARSE_MODE_SCD	1


unsigned int UTF8ToUnicode16(const unsigned char* pszUtf8, unsigned int nSizeUtf8, unsigned short* pszUtf16, unsigned int nSizeUtf16)  
{  
	if (0 == nSizeUtf16)
	{
		return (nSizeUtf8 + 1);
	}
	unsigned int count = 0;
	unsigned int i = 0;
	unsigned short wch;
	unsigned short *p;
	for(i = 0; count < nSizeUtf8 && i < nSizeUtf16; i++)
	{
		p = (unsigned short*)&pszUtf16[i];

		if( pszUtf8[count] < 0x80)
		{
			wch = pszUtf8[count];
			count++;
		}
		else if( (pszUtf8[count] < 0xDF) && (pszUtf8[count] >= 0x80))
		{
			wch = pszUtf8[count] & 0x1F;
			wch = wch << 6;
			wch += pszUtf8[count+1] & 0x3F;
			count += 2;
		}
		else if( (pszUtf8[count] <= 0xEF) && (pszUtf8[count] >= 0xDF))
		{
			wch = pszUtf8[count] & 0x0F;
			wch = wch << 6;
			wch += pszUtf8[count+1] & 0x3F;
			wch = wch << 6;
			wch += pszUtf8[count+2] & 0x3F;
			count += 3;
		}
		else
		{
			printf("error!/n");
		}
		*p = wch;
	}

	if (i < nSizeUtf16)
	{
		pszUtf16[i] = 0;
		return (i + 1);
	}
	else if (nSizeUtf16 > 0)
	{
		pszUtf16[nSizeUtf16-1] = 0;
		return nSizeUtf16;
	}

	return 0;
}


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

	extern bool g_bGlobalCtrl_Select_En;
	extern bool g_bGlobalCtrl_Oper_En;
	extern bool g_bGlobalCtrl_Cancel_En;


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

	int main_old(int argc, char *argv[])
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
		mvl_max_dyn.types	 = 500000;
		mvl_max_dyn.dom_vars = 500000;
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
		info = ind->u.getcl.resp_info = (GETCL_RESP_INFO*)chk_malloc (sizeof(GETCL_RESP_INFO) +
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

		if (!m_sBindIPAddr.isEmpty())
			sk_set_bind_address(m_sBindIPAddr.data());

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
	static ST_VOID *my_malloc_err (size_t size);
	static ST_VOID *my_calloc_err (size_t num, size_t size);
	static ST_VOID *my_realloc_err (ST_VOID *old, size_t size);

	static ST_CHAR *spareMem=NULL;

	static ST_VOID init_mem ()
	{

		/* Allocate spare memory to allow logging/printing memory errors	*/
		spareMem = (char*)malloc (500);

		/* trap mem_chk errors							*/
		mem_chk_err = mem_chk_error_detected;
		m_memerr_fun = my_malloc_err;
		c_memerr_fun = my_calloc_err;
		r_memerr_fun = my_realloc_err;

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

	static ST_VOID *my_malloc_err (size_t size)
	{
		mem_chk_error_detected ();
		printf ("\n Malloc");
		exit (2);
		return (NULL);   
	}

	static ST_VOID *my_calloc_err (size_t num, size_t size)
	{
		mem_chk_error_detected ();
		exit (3);
		return (NULL);   
	}

	static ST_VOID *my_realloc_err (ST_VOID *old, size_t size)
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
		memset(&startup_cfg,0,sizeof(startup_cfg));
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
		FOPEN_RESP_INFO resp_info;
		SMmsServer::CMmsMemFile *pMemFile;

		int fid = g_MmsServer_ptr->m_iMemFileId++;
		if (fid == C_MAX_MMS_SERVER_FILE_OPENED)
		{
			fid = 0;
			g_MmsServer_ptr->m_iMemFileId = 1;
		}
		if(g_MmsServer_ptr->m_MemFilePtr[fid] != NULL)
		{
			delete g_MmsServer_ptr->m_MemFilePtr[fid];
		}
		pMemFile = g_MmsServer_ptr->m_MemFilePtr[fid] = g_MmsServer_ptr->OnReadServerFile(indCtrl->u.fopen.filename);
		if (pMemFile == NULL)
		{
			_mplas_err_resp (indCtrl,11,6);		/* File-access denied	*/
			return;
		}
		pMemFile->m_tOpenTime = (int)SDateTime::getNowSoc();
		/* Allocate MVL_NET_FILE struct and add it to linked list.	*/
		/* NOTE: this must be before resp so "event" still valid.	*/

		/* Use (MVL_NET_FILE *) as frsmid.				*/
		/* WARNING: this only works if pointers are 32 bits.		*/
		resp_info.frsmid = (ST_INT32) fid;
		resp_info.ent.fsize    = pMemFile->m_iLen;
		resp_info.ent.mtimpres = SD_TRUE;
		resp_info.ent.mtime    = pMemFile->m_tFileModifyTime;

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
		MVLAS_FREAD_CTRL *fread_ctrl = &indCtrl->u.fread;
		FREAD_RESP_INFO resp_info;
		SMmsServer::CMmsMemFile *pMemFile = g_MmsServer_ptr->m_MemFilePtr[fread_ctrl->req_info->frsmid];
		if(pMemFile == NULL)
		{
			_mplas_err_resp (indCtrl, 3, 0);
			return;
		}
		resp_info.filedata = pMemFile->m_pBuffer+pMemFile->m_iReadPos;
		resp_info.fd_len = fread_ctrl->max_size;
		if(resp_info.fd_len > pMemFile->m_iLen-pMemFile->m_iReadPos)
			resp_info.fd_len = pMemFile->m_iLen-pMemFile->m_iReadPos;
		if(resp_info.fd_len <= 0)
		{
			_mplas_err_resp (indCtrl, 3, 0);
			return;
		}
		if (pMemFile->m_iReadPos + resp_info.fd_len < pMemFile->m_iLen)
			resp_info.more_follows = SD_TRUE;
		else
			resp_info.more_follows = SD_FALSE;
		pMemFile->m_iReadPos += resp_info.fd_len;
		fread_ctrl->resp_info = &resp_info;
		mplas_fread_resp (indCtrl);
	}
#endif	/* #if (MMS_FREAD_EN & RESP_EN)	*/

#if (MMS_FCLOSE_EN & RESP_EN)
	/************************************************************************/
	/*			u_mvl_fclose_ind					*/
	/************************************************************************/
	ST_VOID u_mvl_fclose_ind (MVL_IND_PEND *indCtrl)
	{
		MVLAS_FCLOSE_CTRL *fclose_ctrl = &indCtrl->u.fclose;
		SMmsServer::CMmsMemFile *pMemFile = g_MmsServer_ptr->m_MemFilePtr[fclose_ctrl->req_info->frsmid];
		if(pMemFile == NULL)
			_mplas_err_resp (indCtrl, 11, 0);	/* File problem, other	*/
		else
		{
			g_MmsServer_ptr->m_MemFilePtr[fclose_ctrl->req_info->frsmid] = NULL;
			delete pMemFile;
			mplas_fclose_resp (indCtrl);
		}
	}
#endif	/* #if (MMS_FCLOSE_EN & RESP_EN)	*/

#if (MMS_FDIR_EN & RESP_EN)
	/************************************************************************/
	/*			u_mvl_fdir_ind					*/
	/************************************************************************/
	//MEMO: move from S_fdir.c [2016-9-5 11:04 邵凯田]
#define SERVICE_OVRHD    20
#define DIR_ENTRY_OVRHD  32
	ST_VOID u_mvl_fdir_ind (MVL_IND_PEND *indCtrl)
	{
		/* This function works for _WIN32, linux, and other Posix systems.	*/
		/* It may need to be ported for other systems.			*/
		//MEMO: del by skt [2016-9-5 11:03 邵凯田]mvlas_fdir_resp (indCtrl);

		//MEMO: add by skt [2016-9-5 11:03 邵凯田]
		ST_INT   status;
		ST_CHAR  fs_name [MAX_FILE_NAME+1];
		ST_CHAR  ca_name [MAX_FILE_NAME+1];
		ST_INT   ca_len;
		FDIR_RESP_INFO *rsp_ptr;
		FDIR_DIR_ENT   *dir_ptr;
		ST_INT rsp_size;
		ST_CHAR    *name_ptr;
		ST_INT  total_len;
		FILE_NAME  *fname_ptr;
		ST_UCHAR   found_ca = SD_FALSE;
		ST_CHAR    *data_buf;
		MVLAS_FDIR_CTRL *fdir_ctrl;
		ST_INT  negotiatedPduSize;
		ST_INT  max_dir_ents;
		ST_RET  rc;


		/* calculate the maximum message size and number of directory entries */
		negotiatedPduSize = (ST_INT) indCtrl->event->net_info->max_pdu_size;
		max_dir_ents = (negotiatedPduSize - SERVICE_OVRHD) / DIR_ENTRY_OVRHD;

		/* set the number of directory entries and allocate memory for response info */
		fdir_ctrl = &indCtrl->u.fdir;

		if(fdir_ctrl->ca_filename[0] == '\0')
		{
			//没有ca_filename时，表示这第一次请求目录内容
			g_MmsServer_ptr->m_sLastDirPath = fdir_ctrl->fs_filename;
			g_MmsServer_ptr->m_LastDirResult.clear();
			g_MmsServer_ptr->OnServerDirectory(fdir_ctrl->fs_filename,g_MmsServer_ptr->m_LastDirResult,true);
		}
		else
		{
			//有ca_filename时，表示这是取目录内容后续帧
			if(SString::equals(g_MmsServer_ptr->m_sLastDirPath.data(),fdir_ctrl->fs_filename))
			{
				//此时看最后一次缓存的结果是否与当前请求目录同名，是则不需要再读取目录，直接回复对应的分片即可
			}
			else
			{
				//不匹配，重新加载
				g_MmsServer_ptr->m_sLastDirPath = fdir_ctrl->fs_filename;
				g_MmsServer_ptr->m_LastDirResult.clear();
				g_MmsServer_ptr->OnServerDirectory(fdir_ctrl->fs_filename,g_MmsServer_ptr->m_LastDirResult,true);
			}
		}
		SPtrList<SMmsServer::stuSMmsFileInfo> *pDirFiles = &g_MmsServer_ptr->m_LastDirResult;

		strcpy (fs_name, fdir_ctrl->fs_filename);
		strcpy (ca_name, fdir_ctrl->ca_filename);

		/* check if ca_filename is present */
		if ((ca_len = (ST_INT) strlen (ca_name)) > 0)
		{
			/* strip off any '\' character at the end of file name */
			if (ca_len && ca_name[ca_len-1] == '\\')
			{
				ca_len--;                        
				ca_name[ca_len] = '\0';
			}
		}
		else
			found_ca = SD_TRUE;

		/*---------------------------------------------------------*/
		/* Find maximum response buffer size, calloc space for it, */
		/* and initialize the response structure variables.        */
		/*---------------------------------------------------------*/
		rsp_size = sizeof (FDIR_RESP_INFO) 
			+ ( max_dir_ents * sizeof (FDIR_DIR_ENT) )
			+ ( max_dir_ents * sizeof (FILE_NAME) );
		rsp_ptr  = (FDIR_RESP_INFO *) chk_calloc (1,rsp_size);

		/* Allocate buffer to hold just names. Will NEVER use whole buffer	*/
		/* because names plus overhead must be less than mms_max_msgsze.	*/
		data_buf = (char*)chk_malloc (negotiatedPduSize);

		rsp_ptr->num_dir_ent  = 0;
		rsp_ptr->more_follows = SD_FALSE;                        /* set default value    */

		/*---------------------------------*/
		/* Initialize some other variables */
		/*---------------------------------*/
		SMmsServer::stuSMmsFileInfo *pFileInfo;
		name_ptr = data_buf;
		total_len = 0;
		status = SD_SUCCESS;
		fname_ptr = ( FILE_NAME * )( fdir_ctrl + 1 );

		dir_ptr   = (FDIR_DIR_ENT *)(rsp_ptr + 1);
		fname_ptr = (FILE_NAME *)(dir_ptr + 1);
		for(int i=0;i<pDirFiles->count();i++)
		{
			pFileInfo = pDirFiles->at(i);
			if(!found_ca)
			{
				if(pFileInfo->filename == ca_name)
					found_ca = SD_TRUE;//找到continue after的位置，下一个开始使用
				continue;
			}
			if (total_len < (negotiatedPduSize - SERVICE_OVRHD) ) 
			{
				rsp_ptr->num_dir_ent++;     

				dir_ptr->fsize = pFileInfo->fsize;
				dir_ptr->mtime = pFileInfo->mtime;
				dir_ptr->mtimpres = pFileInfo->mtimpres;
				dir_ptr->num_of_fname = 1;

				strcpy(name_ptr,pFileInfo->filename.data());
				fname_ptr->fname = name_ptr;
				fname_ptr->fn_len = strlen(fname_ptr->fname);

				//fillFdirEnt (&stFindData, dir_ptr, fname_ptr, name_ptr);
				name_ptr += fname_ptr->fn_len + 1; /* check for end of buffer */
				dir_ptr =  (FDIR_DIR_ENT *) ((FILE_NAME *) (dir_ptr + 1) + 1);
				fname_ptr = (FILE_NAME *)(dir_ptr + 1);
			}
			else/* message size limit           */
			{
				//放不下了
				rsp_ptr->more_follows = SD_TRUE;
				break;
			}
		}
		
		if (status == SD_SUCCESS)                /* directory was successful     */
		{
			/* Build a File Directory response 					*/
			rc = mpl_fdir_resp (indCtrl->event->u.mms.dec_rslt.id, rsp_ptr);
			if (rc == SD_SUCCESS)
				mpl_resp_log (indCtrl, rsp_ptr);

			_mvl_send_resp_i (indCtrl, rc);
		}
		/*------------------------*/
		/* Free allocated storage */
		/*------------------------*/

		chk_free (rsp_ptr);
		chk_free (data_buf);

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
		if(g_MmsServer_ptr->OnDeleteServerFile(fdelete_ctrl->filename) <= 0)
		//if (remove (fdelete_ctrl->filename))
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
		if(g_MmsServer_ptr->OnRenameServerFile(frename_ctrl->curfilename, frename_ctrl->newfilename) <= 0)
		//if (rename (frename_ctrl->curfilename, frename_ctrl->newfilename))
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
	ST_VOID mvlas_obtfile_resp_skt (MVL_IND_PEND *indCtrl);

	ST_VOID u_mvl_obtfile_ind (MVL_IND_PEND *indCtrl)
	{
		mvlas_obtfile_resp_skt (indCtrl);      /* indicate state machine starting */
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

	MVL_VMD_CTRL **scl2_vmd_create_all_new (
		SCL_INFO *scl_info,	/* SCL info from parser		*/
		ST_CHAR *iedName,	/* IED name of Server		*/
		/* If NULL, no Server configured*/
		ST_CHAR *apName,	/* AccessPoint name of Server	*/
		/* If NULL, no Server configured*/
		SCL_SERV_OPT *serv_opt,	/* Server options		*/
		ST_UINT *vmd_count)	/* OUT: number of VMDs created	*/
		/* (i.e. size of array returned)*/
	{
		MVL_VMD_CTRL *vmd_ctrl;
		ST_RET ret;
		SCL_SERVER *scl_server;
		MVL_VMD_CTRL **vmd_ctrl_arr=NULL;	/* array of VMDs for each Server*/
		/* allocated by this function	*/
		ST_UINT server_count, tmp_count;
		ST_BOOLEAN is_client;		/* set this for each VMD	*/
		SCL_LD *scl_ld;
		ST_INT dom_count;

		/* Create ALL types in global "mvl_vmd".	*/
		/* It appears that we need about 25000 in 3rd arg to support MHAI (Edition 2).*/
		ret = scl2_datatype_create_all (&mvl_vmd, scl_info, 25000, SD_FALSE, NULL);
		if (ret)
			return (NULL);	/* error	*/

		/* Parser created "Servers" and saved on "serverHead" list.	*/
		/* Loop through list and create VMD for each Server.		*/
		server_count = 0;
		is_client = SD_FALSE;
		for (scl_server = scl_info->serverHead;
			scl_server != NULL;
			scl_server = (SCL_SERVER *) list_get_next (scl_info->serverHead, scl_server))
		{
			printf("AP%d : %s/%s\n",server_count+1,scl_server->iedName,scl_server->apName);
			/* If iedName and apName are valid, and they match this scl_server,*/
			/* configure this scl_server as the ONE AND ONLY Server VMD.	*/
			/* All other scl_server are configured as Client (i.e. remote) VMDs.*/
			if((iedName == NULL || strcmp (scl_server->iedName, iedName) == 0) &&
				(apName == NULL || strcmp (scl_server->apName, apName) == 0))
// 			if (iedName != NULL && apName != NULL && 
// 				strcmp (scl_server->iedName, iedName) == 0 &&
// 				strcmp (scl_server->apName, apName) == 0)
			{
				/* CRITICAL: Server must use global VMD (mvl_vmd).	*/
				vmd_ctrl = &mvl_vmd;	/* point to global VMD	*/
				//is_client = SD_FALSE;
				/* Create all LD for one Server (one VMD) of the SCD.	*/
				ret = scl2_ld_create_all_scd (vmd_ctrl, scl_info, scl_server, serv_opt, is_client);
				is_client = SD_TRUE;
				if (ret)
				{
					SLOGALWAYS0 ("ERROR creating all LD for Server");
					break;
				}
			}
			else
			{
				/* Create a new VMD for a Client.	*/
				/* First count the number of Domains needed.	*/
				dom_count = 0;
				for (scl_ld = (SCL_LD *) list_find_last ((DBL_LNK *) scl_server->ldHead);
					scl_ld != NULL;
					scl_ld = (SCL_LD *) list_find_prev ((DBL_LNK *) scl_server->ldHead, (DBL_LNK *) scl_ld))
				{
					dom_count++;
				}
				/* Create VMD with exactly the required number of domains.	*/
				/* Other args (VMD-spec vars, nvls, journals) are always 0.	*/
				vmd_ctrl = mvl_vmd_create (dom_count, 0, 0, 0);

				/* Create all LD for one Server (one VMD) of the SCD.	*/
				is_client = SD_TRUE;	/* Make this a "Client" VMD	*/
				ret = scl2_ld_create_all_scd (vmd_ctrl, scl_info, scl_server, serv_opt, is_client);
				if (ret)
				{
					SLOGALWAYS0 ("ERROR creating all LD for Client");
					mvl_vmd_destroy (vmd_ctrl);	/* Destroy VMD created above.	*/
					break;
				}
			}
			server_count++;
		}	/* end "scl_server" loop	*/
		/* CRITICAL: all types created in global "mvl_vmd". Free reserved info in each type.*/
		scl2_reserved_free_all (&mvl_vmd);

		if (ret)
			return (NULL);	/* error	*/

		/* If all succeeded so far, allocate & fill in array of VMD pointers.	*/
		/* NOTE: scl2_ld_create_all_scd saved VMD ptr in SCL_SERVER struct.	*/
		/* Allocate array of VMD pointers (one ptr for each Server).	*/
		/* Copy VMD pointers to this array.					*/
		vmd_ctrl_arr = (MVL_VMD_CTRL**)chk_calloc (server_count, sizeof (MVL_VMD_CTRL *));
		tmp_count = 0;
		for (scl_server = scl_info->serverHead;
			scl_server != NULL;
			scl_server = (SCL_SERVER *) list_get_next (scl_info->serverHead, scl_server))
		{
			vmd_ctrl_arr[tmp_count++] = scl_server->vmd_ctrl;
		}
		assert (tmp_count == server_count);	/* count should always match on 2nd loop*/
		/* Number of VMDs = number of Servers found (i.e. IED/AccessPoint pairs).*/
		*vmd_count = server_count;    

		return (vmd_ctrl_arr);
	}

	//////////////////////////////////////////////////////////////////////////
	// 文件上传
	//////////////////////////////////////////////////////////////////////////

	typedef struct filecopyinfo
	{
		ST_CHAR  	tempfilename[MVL_MAX_TMPNAM_LEN+1];
		ST_CHAR	destfilename[MAX_FILE_NAME+1];
		MVL_IND_PEND  *indCtrl;
		FILE		*fp;
		ST_INT32	frsmid;
		ST_UINT32	fsize;
		ST_UINT32	fileSizeLimit;
	} MVL_OBTFILE_STATE;

	MVL_REQ_PEND *send_file_open_skt (MVL_NET_INFO *NetInfo, ST_CHAR *src);
	MVL_REQ_PEND *send_file_read_skt (MVL_NET_INFO *NetInfo, 
		MVL_OBTFILE_STATE *u_file_info);
	MVL_REQ_PEND *send_file_close_skt (MVL_NET_INFO *NetInfo, 
		MVL_OBTFILE_STATE *u_file_info);
	ST_VOID u_fopen_done_skt (MVL_REQ_PEND *req);
	ST_VOID u_fread_done_skt( MVL_REQ_PEND *req);
	ST_VOID u_fclose_done_skt( MVL_REQ_PEND *req);
	ST_VOID mvlas_obtfile_resp_ex_skt (MVL_IND_PEND *indCtrl, ST_UINT32 fileSizeLimit);
	ST_VOID mplas_obtfile_resp_skt (MVL_IND_PEND *indCtrl);


	/************************************************************************/
	/************************************************************************/
	/*			_mvl_process_obtfile_ind_skt		        */
	/* An Obtain File request has been sent and                             */
	/* An Obtain File indication was received, so process it and respond	*/
	/************************************************************************/

	ST_VOID _mvl_process_obtfile_ind_skt (MVL_IND_PEND *indCtrl)
	{
		OBTFILE_REQ_INFO *req_info;
		MVLAS_OBTFILE_CTRL *obtfile;
		FILE_NAME *fname_idx;


		req_info = (OBTFILE_REQ_INFO *) indCtrl->event->u.mms.dec_rslt.data_ptr;
		fname_idx = (FILE_NAME *)(req_info + 1);

		obtfile = &indCtrl->u.obtfile;

		if (_mvl_concat_filename (obtfile->srcfilename, req_info->num_of_src_fname, 
			fname_idx, sizeof (obtfile->srcfilename)))
		{
			MVL_LOG_ERR0 ("ObtFile error: _mvl_process_obtfile_ind failed (_mvl_concat_filename(srcfilename) failed)");
			_mplas_err_resp (indCtrl,11,3);	/* 3 = File name syntax error */
			return;
		}

		if (_mvl_concat_filename (obtfile->destfilename, req_info->num_of_dest_fname, 
			(FILE_NAME *)(fname_idx + req_info->num_of_src_fname), 
			sizeof (obtfile->destfilename)))
		{
			MVL_LOG_ERR0 ("ObtFile error: _mvl_process_obtfile_ind failed (_mvl_concat_filename(destfilename) failed)");
			_mplas_err_resp (indCtrl,11,3);	/* 3 = File name syntax error */
			return;
		}

		u_mvl_obtfile_ind (indCtrl);
	}

	/************************************************************************/
	/*			mvlas_obtfile_resp				*/
	/* Open a temp file to copy into and save the destination file name     */
	/************************************************************************/
	
	ST_VOID mvlas_obtfile_resp_skt (MVL_IND_PEND *indCtrl)
	{  
		mvlas_obtfile_resp_ex_skt (indCtrl, 0);
	}


	ST_VOID mvlas_obtfile_resp_ex_skt (MVL_IND_PEND *indCtrl, ST_UINT32 fileSizeLimit)
	{
		MVL_OBTFILE_STATE *obtfile_state;
		MVL_REQ_PEND *req;
		MVLAS_OBTFILE_CTRL *obtfile;
		ST_CHAR tmpfile_path[MVL_MAX_TMPFILE_PATH];

		obtfile = &indCtrl->u.obtfile;

		obtfile_state = (MVL_OBTFILE_STATE *) M_CALLOC (MSMEM_GEN, 1, sizeof (MVL_OBTFILE_STATE));

		/* save destination name for rename */
		SString sDescFile = g_MmsServer_ptr->m_sRootPath+obtfile->destfilename;
		//strncpy (obtfile_state->destfilename, obtfile->destfilename, MAX_FILE_NAME+1); 
		strncpy (obtfile_state->destfilename, sDescFile.data(), MAX_FILE_NAME+1); 

		obtfile_state->indCtrl = indCtrl;

		/* Create a temporary file name to copy into. This prevents partially	*/
		/* transferred files from hanging around and being accidently used	*/
		/* Open the created temporary file. Send the file open request.	*/

		_mvl_getpath (obtfile_state->destfilename, tmpfile_path);

		if (_mvl_tmpnam (obtfile_state->tempfilename, tmpfile_path) != SD_SUCCESS  ||
			(obtfile_state->fp = fopen(obtfile_state->tempfilename, "wb")) == NULL ||
			(req = send_file_open_skt (indCtrl->event->net_info,obtfile->srcfilename)) == NULL)
		{
			if ( obtfile_state->fp )
			{
				MVL_LOG_ERR0 ("ObtFile error: mvlas_obtfile_resp failed (fopen failed)");
				fclose (obtfile_state->fp);              /* close temporary file      */
				remove (obtfile_state->tempfilename);    /* get rid of temporary file */
			}
			else
			{
				MVL_LOG_ERR1 ("ObtFile: mvlas_obtfile_resp failed (invalid tmpnam (%s) or send_file_open failed)", 
					obtfile_state->tempfilename);
			}
			_mplas_err_resp (indCtrl,11,0);	/* 0 = other */
			M_FREE (MSMEM_GEN, obtfile_state);
		}
		else
		{
			req->v = obtfile_state;
			obtfile_state->fileSizeLimit = fileSizeLimit;
		}
	}

	/************************************************************************/
	/*		            send_file_open				*/
	/* Send a file open request to open file on the other side              */
	/************************************************************************/

	MVL_REQ_PEND *send_file_open_skt (MVL_NET_INFO *NetInfo, ST_CHAR *src)
	{
		MVL_REQ_PEND *req= NULL;

		if ((mvla_fopen(NetInfo, src, 0, &req)) == SD_SUCCESS)
		{
			req->u_req_done = u_fopen_done_skt;
		}
		else
		{
			/* error, clean up and return NULL	*/
			/* NOTE: "req" may be NULL but "mvl_free_req_ctrl" safely handles that.*/
			mvl_free_req_ctrl(req);
			req = NULL;
		}
		return (req);
	}

	/************************************************************************/
	/*		            u_fopen_done				*/
	/* It was indicated that a file open request was made                   */
	/* Confirm file open response that was received as a result of a request*/                                           
	/************************************************************************/

	ST_VOID u_fopen_done_skt( MVL_REQ_PEND *req)
	{
		ST_RET ret;
		MVL_REQ_PEND  *freadreq= NULL;
		MVL_OBTFILE_STATE  *obtfile_state;
		FOPEN_RESP_INFO  *resp_info;
#if 0
		ERR_INFO 		*err_ptr;
		REJECT_RESP_INFO 	*rej_ptr;
#endif

		obtfile_state = (MVL_OBTFILE_STATE *) req->v;
		ret = req->result;
		if (ret == SD_SUCCESS)
		{
			resp_info = req->u.fopen.resp_info;
			if (obtfile_state->fileSizeLimit > 0 && 
				(resp_info->ent.fsize > obtfile_state->fileSizeLimit))
			{
				MVL_LOG_NERR2 ("ObtFile error: file size %lu exceeds limit %lu", 
					(ST_ULONG)resp_info->ent.fsize, (ST_ULONG)obtfile_state->fileSizeLimit);
				ret = SD_FAILURE;
			}  
			else
			{  

				obtfile_state ->frsmid = resp_info->frsmid;
				obtfile_state ->fsize = resp_info->ent.fsize;
				if ((freadreq = send_file_read_skt (req->net_info, obtfile_state)) != NULL)
				{
					freadreq->v = obtfile_state;
				}
				else
				{
					MVL_LOG_ERR0 ("ObtFile error: u_fopen_done failed (send_file_read failed)");
					ret = SD_FAILURE;
				}
			}
		}
		else
		{
			MVL_LOG_ERR0 ("ObtFile error: u_fopen_done failed (mvla_fopen failed)");
		}
		if ( ret != SD_SUCCESS )
		{ /* file open failed */
			fclose (obtfile_state->fp);	       /* close temporary file      */
			remove (obtfile_state->tempfilename);    /* get rid of temporary file */
			if (req->net_info->conn_active == SD_TRUE)
				_mplas_err_resp (obtfile_state->indCtrl,11,0);	/* 0 = other */
			M_FREE (MSMEM_GEN, obtfile_state);
		} 
		mvl_free_req_ctrl (req);
	}

	/************************************************************************/
	/*		            send_file_read				*/
	/* Send a file read request to obtain a block of the file data          */
	/************************************************************************/

	MVL_REQ_PEND *send_file_read_skt (MVL_NET_INFO *NetInfo, 
		MVL_OBTFILE_STATE *obtfile_state)
	{
		MVL_REQ_PEND *req= NULL;

		FREAD_REQ_INFO fread_req;

		fread_req.frsmid = obtfile_state->frsmid;  
		if ((mvla_fread (NetInfo, &fread_req, &req)) == SD_SUCCESS)
		{
			req->u_req_done = u_fread_done_skt;
		}
		else
		{
			/* error, clean up and return NULL	*/
			/* NOTE: "req" may be NULL but "mvl_free_req_ctrl" safely handles that.*/
			mvl_free_req_ctrl(req);
			req = NULL;
		}
		return (req);
	}


	/************************************************************************/
	/*		            u_fread_done				*/
	/* Confirm the file read response sent to indicate a read request       */
	/************************************************************************/

	ST_VOID u_fread_done_skt( MVL_REQ_PEND *req)
	{
		ST_RET ret;
		MVL_REQ_PEND *freadreq= NULL;
		MVL_REQ_PEND *fclosereq= NULL;
		MVL_OBTFILE_STATE *obtfile_state;
		FREAD_RESP_INFO	*resp_info;

		obtfile_state = (MVL_OBTFILE_STATE *) req->v;

		ret = req->result;
		if (ret == SD_SUCCESS)
		{
			resp_info = req->u.fread.resp_info;

			if ( fwrite (resp_info->filedata, sizeof(ST_CHAR), resp_info->fd_len, obtfile_state->fp) 
				== (unsigned) resp_info->fd_len )
			{
				if ( resp_info->more_follows == SD_TRUE)
				{
					if ((freadreq = send_file_read_skt (req->net_info, obtfile_state)) != NULL)
					{
						freadreq->v = obtfile_state;  /* if more file data request to read another block */
					}
					else
					{
						MVL_LOG_ERR0 ("ObtFile error: u_fread_done failed (send_file_read failed)");
						ret = SD_FAILURE;
					}
				}
				else
				{
					if ((fclosereq = send_file_close_skt (req->net_info, obtfile_state)) != NULL)
					{
						fclosereq->v = obtfile_state;  /* no more file data so send a file close request */
					}
					else
					{
						MVL_LOG_ERR0 ("ObtFile error: u_fread_done failed (send_file_close failed)");
						ret = SD_FAILURE;
					}
				}
			}
			else
			{
				/* write failed */
				MVL_LOG_ERR0 ("ObtFile error: u_fread_done failed (fwrite failed)");
				ret = SD_FAILURE;
			}
		}
		else
		{
			MVL_LOG_ERR0 ("ObtFile error: u_fread_done failed (mvla_fread failed)");
		}

		if (ret != SD_SUCCESS )
		{ /* read failed so close and remove temp file */
			fclose (obtfile_state->fp);
			remove (obtfile_state->tempfilename);  /* may fail if file nonexistent */
			if (req->net_info->conn_active == SD_TRUE)
				_mplas_err_resp (obtfile_state->indCtrl,11,0);	   /* 0 = Other */
			M_FREE (MSMEM_GEN, obtfile_state);
		}

		mvl_free_req_ctrl (req);
	}

	/************************************************************************/
	/*		            send_file_close				*/
	/* No more data to read from file so send a request to close the file   */
	/************************************************************************/

	MVL_REQ_PEND *send_file_close_skt (MVL_NET_INFO *NetInfo, 
		MVL_OBTFILE_STATE *obtfile_state)
	{
		MVL_REQ_PEND *req= NULL;
		FCLOSE_REQ_INFO fclose_req;

		fclose_req.frsmid = obtfile_state->frsmid;
		if ((mvla_fclose (NetInfo, &fclose_req, &req)) == SD_SUCCESS)
		{
			req->u_req_done = u_fclose_done_skt;
		}
		else
		{
			/* error, clean up and return NULL	*/
			/* NOTE: "req" may be NULL but "mvl_free_req_ctrl" safely handles that.*/
			mvl_free_req_ctrl(req);
			req = NULL;
		}
		return (req);
	}

	/************************************************************************/
	/*		            u_fclose_done				*/
	/* Confirm file closed response                                         */
	/************************************************************************/
	
	ST_VOID u_fclose_done_skt( MVL_REQ_PEND *req)
	{

		MVL_OBTFILE_STATE *obtfile_state;

		obtfile_state = (MVL_OBTFILE_STATE *) req->v;

		if (req->result == SD_SUCCESS)    /* close succeeded */
		{

			/* always close transferred file, rename and remove temp file */
			fclose (obtfile_state->fp);	    /* close temp file */
			remove (obtfile_state->destfilename);  /* may fail if file nonexistent */

			/* rename the temporary file to become the requested destination file  */ 
			/* use the ANSI rename function if available on your OS  */
			/* otherwise use other function appropriate for your system */
			if (rename (obtfile_state->tempfilename, obtfile_state->destfilename))
			{
				MVL_LOG_ERR0 ("ObtFile error: u_fclose_done failed (rename failed)");
				remove (obtfile_state->tempfilename);  /* cleanup useless file */
				_mplas_err_resp (obtfile_state->indCtrl, 11, 0);   /* 0 = other file error */
			}
			else	/* send an obtainfile response */
			{
				mplas_obtfile_resp_skt (obtfile_state->indCtrl);  
				g_MmsServer_ptr->OnWriteServerFile(obtfile_state->destfilename);
			}

		}
		else  /* close failed */
		{
			MVL_LOG_ERR0 ("ObtFile error: u_fclose_done failed (mvla_fclose failed)");

			/* always close transferred file, rename and remove temp file */
			fclose (obtfile_state->fp);	    /* close temp file */

			remove (obtfile_state->tempfilename);  /* cleanup useless file */
			/* but don't send an obtainfile response */

			if (req->net_info->conn_active == SD_TRUE)
				_mplas_err_resp (obtfile_state->indCtrl,11,0);	/* 0 = other */
		}

		M_FREE (MSMEM_GEN, obtfile_state);
		mvl_free_req_ctrl (req);
	}


	/************************************************************************/
	/*			mplas_obtfile_resp_skt				*/
	/* Send a response to confirm the obtain file request that prompted the */
	/* reading of file data                                                 */
	/************************************************************************/
	ST_VOID mplas_obtfile_resp_skt (MVL_IND_PEND *indCtrl)
	{
		ST_RET rc;

		/* Build an Obtain File Response with Null to confirm */
		rc = mpl_obtfile_resp (indCtrl->event->u.mms.dec_rslt.id);
		if (rc == SD_SUCCESS)
			mpl_resp_log (indCtrl, NULL);

		_mvl_send_resp_i (indCtrl, rc);


	}

	//////////////////////////////////////////////////////////////////////////
	//
	//////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
};
#endif

unsigned char g_sCScdCommunicationZero[1024];
CScdCommunication::CScdCommunication()
{
	memset(&g_sCScdCommunicationZero,0,sizeof(g_sCScdCommunicationZero));
	//SConstPtrList<stuSCL_Communication> m_Communication;

	SetRootNodeName("SCL");
	SetRootCallbackFun(CB_SCL);
}
CScdCommunication::~CScdCommunication()
{
}
stuSaxChoice* CScdCommunication::CB_SCL(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);
	static const stuSaxChoice pChoice[] = 
	{
		{ "Communication",                  CB_SCL_Communication },
		{ NULL,NULL }
	};

	CScdCommunication *pThis = (CScdCommunication*)cbParam;

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CScdCommunication::CB_SCL_Communication(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);
	static const stuSaxChoice pChoice[] = 
	{
		{ "SubNetwork",                     CB_SCL_Communication_SubNetwork },
		{ NULL,NULL }
	};

	CScdCommunication *pThis = (CScdCommunication*)cbParam;
	pThis->m_p_SCL_Communication = (stuSCL_Communication*)pThis->m_ConstString.CopyToConstBuffer(g_sCScdCommunicationZero,sizeof(stuSCL_Communication));
	pThis->m_p_SCL_Communication->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_Communication.append(pThis->m_ConstString,pThis->m_p_SCL_Communication);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CScdCommunication::CB_SCL_Communication_SubNetwork(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);
	static const stuSaxChoice pChoice[] = 
	{
		{ "ConnectedAP",                    CB_SCL_Communication_SubNetwork_ConnectedAP },
		{ NULL,NULL }
	};

	CScdCommunication *pThis = (CScdCommunication*)cbParam;
	pThis->m_p_SCL_Communication_SubNetwork = (stuSCL_Communication_SubNetwork*)pThis->m_ConstString.CopyToConstBuffer(g_sCScdCommunicationZero,sizeof(stuSCL_Communication_SubNetwork));
	for(int i=0;i<iAttributeCount;i++,pAttribute++)
	{
		if(SString::equals(pAttribute->m_sAttrName.data(), "desc"))
			pThis->m_p_SCL_Communication_SubNetwork->desc = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "type"))
			pThis->m_p_SCL_Communication_SubNetwork->type = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "name"))
			pThis->m_p_SCL_Communication_SubNetwork->name = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
	}
	pThis->m_p_SCL_Communication_SubNetwork->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_Communication->m_SubNetwork.append(pThis->m_ConstString,pThis->m_p_SCL_Communication_SubNetwork);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CScdCommunication::CB_SCL_Communication_SubNetwork_ConnectedAP(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);
	static const stuSaxChoice pChoice[] = 
	{
		{ NULL,NULL }
	};

	CScdCommunication *pThis = (CScdCommunication*)cbParam;
	pThis->m_p_SCL_Communication_SubNetwork_ConnectedAP = (stuSCL_Communication_SubNetwork_ConnectedAP*)pThis->m_ConstString.CopyToConstBuffer(g_sCScdCommunicationZero,sizeof(stuSCL_Communication_SubNetwork_ConnectedAP));
	for(int i=0;i<iAttributeCount;i++,pAttribute++)
	{
		if(SString::equals(pAttribute->m_sAttrName.data(), "iedName"))
			pThis->m_p_SCL_Communication_SubNetwork_ConnectedAP->iedName = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "apName"))
			pThis->m_p_SCL_Communication_SubNetwork_ConnectedAP->apName = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "desc"))
			pThis->m_p_SCL_Communication_SubNetwork_ConnectedAP->desc = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
	}
	pThis->m_p_SCL_Communication_SubNetwork_ConnectedAP->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_Communication_SubNetwork->m_ConnectedAP.append(pThis->m_ConstString,pThis->m_p_SCL_Communication_SubNetwork_ConnectedAP);

	return (stuSaxChoice*)pChoice;
}



SMmsServer::SMmsServer()
{
	if(g_MmsServer_ptr == NULL)
		g_MmsServer_ptr = this;
	m_Ieds.setAutoDelete(true);
	m_LastDirResult.setAutoDelete(true);
	memset(m_MemFilePtr,0,sizeof(m_MemFilePtr));
	m_iMemFileId = 0;
}

SMmsServer::~SMmsServer()
{

}

//////////////////////////////////////////////////////////////////////////
// 描    述:  添加IED信息，添加后由SMmmsServer内部负责释放，应在Start函数之前完成所有IED、AP的添加
// 作    者:  邵凯田
// 创建时间:  2016-8-29 18:39
// 参数说明:  @pIed
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void SMmsServer::AddIed(stuMmsServerIed *pIed)
{
	unsigned long pos;
	stuMmsServerIed *p = m_Ieds.FetchFirst(pos);
	while(p)
	{
		if(p->ied_name == pIed->ied_name && p->ap_name == pIed->ap_name)
		{
			OnProgressText(SString::toFormat("忽略重复的访问点:IED:%s, AP:%s",pIed->ied_name.data(),pIed->ap_name.data()));
			LOGERROR("忽略重复的访问点:IED:%s, AP:%s",pIed->ied_name.data(),pIed->ap_name.data());
			delete pIed;
			return;
		}
		p = m_Ieds.FetchNext(pos);
	}
	m_Ieds.append(pIed);
}


//////////////////////////////////////////////////////////////////////////
// 描    述:  设备控制操作的全局使能
// 作    者:  邵凯田
// 创建时间:  2017-4-7 15:36
// 参数说明:  @bSelectEn表示选择的全局使能
//         :  @bOperEn表示执行的全局使能
//         :  @bCancelEn表示取消的全局使能
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void SMmsServer::SetGlobalCtrl_En(bool bSelectEn,bool bOperEn,bool bCancelEn)
{
	g_bGlobalCtrl_Select_En = bSelectEn;
	g_bGlobalCtrl_Oper_En   = bOperEn;
	g_bGlobalCtrl_Cancel_En = bCancelEn;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  通过SCD文件添加IED访问点，应在Start函数之前完成本函数调用
// 作    者:  邵凯田
// 创建时间:  2016-9-1 15:30
// 参数说明:  @sScdFile为SCD文件全路径 
// 返 回 值:  >0表示解析SCD并添加IED访问点成功，=0表示SCD解析成功但其中没有有效的MMS访问点,<0表示失败
//////////////////////////////////////////////////////////////////////////
int SMmsServer::AddIedByScd(SString sScdFile)
{
	CScdCommunication scd;
	if(!scd.ParseXmlFile(sScdFile.data()))
	{
		return -1;
	}

	int ied_no = 0;
	unsigned long pos,pos2,pos3;
	int i=0,cnt = 0;

	CScdCommunication::stuSCL_Communication *pComm = scd.m_Communication.FetchFirst(pos);
	while(pComm)
	{
		CScdCommunication::stuSCL_Communication_SubNetwork *pSubNetwork = pComm->m_SubNetwork.FetchFirst(pos2);
		while(pSubNetwork)
		{
			if((pSubNetwork->type != NULL && strstr(pSubNetwork->type,"MMS") != NULL) || (pSubNetwork->name != NULL && strstr(pSubNetwork->name,"MMS") != NULL))
			{
				CScdCommunication::stuSCL_Communication_SubNetwork_ConnectedAP *pAP = pSubNetwork->m_ConnectedAP.FetchFirst(pos3);
				while(pAP)
				{
					cnt++;
					pAP = pSubNetwork->m_ConnectedAP.FetchNext(pos3);
				}
			}
			pSubNetwork = pComm->m_SubNetwork.FetchNext(pos2);
		}
		pComm = scd.m_Communication.FetchNext(pos);
	}
	pComm = scd.m_Communication.FetchFirst(pos);
	while(pComm)
	{
		CScdCommunication::stuSCL_Communication_SubNetwork *pSubNetwork = pComm->m_SubNetwork.FetchFirst(pos2);
		while(pSubNetwork)
		{
			if((pSubNetwork->type != NULL && strstr(pSubNetwork->type,"MMS") != NULL) || (pSubNetwork->name != NULL && strstr(pSubNetwork->name,"MMS") != NULL))
			{
				CScdCommunication::stuSCL_Communication_SubNetwork_ConnectedAP *pAP = pSubNetwork->m_ConnectedAP.FetchFirst(pos3);
				while(pAP)
				{
					i++;
					OnProgressText(SString::toFormat("添加MMS访问点[%d/%d]:%s->%s",i,cnt,pAP->iedName,pAP->apName));
					AddIed(stuMmsServerIed::New(sScdFile,++ied_no,pAP->iedName,pAP->apName));
					pAP = pSubNetwork->m_ConnectedAP.FetchNext(pos3);
				}
			}
			pSubNetwork = pComm->m_SubNetwork.FetchNext(pos2);
		}
		pComm = scd.m_Communication.FetchNext(pos);
	}
	return ied_no;
}

SCL_INFO g_SMMS_scl_info;
MVL_VMD_CTRL **g_SMMS_vmd_ctrl_arr = NULL;	/* for "scl2_vmd_create_all"	*/
ST_INT g_SMMS_scl_parse_mode = SCL_PARSE_MODE_SCD;	/* SCL parse mode	*/
ST_UINT g_SMMS_vmd_count;			/* for "scl2_vmd_create_all"	*/


////////////////////////////////////////////////////////////////////////
// 描    述:  启动服务,虚函数，派生函数中必须先调用此函数
// 作    者:  邵凯田
// 创建时间:  2016-7-23 11:11
// 参数说明:  void
// 返 回 值:  true表示服务启动成功,false表示服务启动失败
//////////////////////////////////////////////////////////////////////////
bool SMmsServer::Start()
{
	if(!SService::Start())
		return false;

	bool haveError = false;
	OnProgressText("正在初始化MMS服务运行环境...");
	ST_CHAR *usageString = "usage: %s -m scl_parse_mode\n\twhere 'scl_parse_mode' is the SCL parse mode.\n\tIt must be 'scd' or 'cid' (default)";
	ST_RET ret;
	STARTUP_CFG startup_cfg;
//	ST_BOOLEAN event;
//	SCL_OPTIONS options;			/* for "scl_parse_scd_all"	*/
	SCL_SERV_OPT serv_opt = {0};		/* for "scl2_vmd_create_all"	*/
//	int argnum;				/* cmd line argument index	*/

	g_SMMS_scl_parse_mode = SCL_PARSE_MODE_SCD;

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
	memset(&startup_cfg,0,sizeof(startup_cfg));
	ret = startup_cfg_read ("startup.cfg", &startup_cfg);
	if (ret)
	{
		printf ("Error reading 'startup.cfg'. Exiting.");
		exit (1);
	}
	mvlu_install ();	/* Install UCA object handler			*/

	/* Must set "mvl_max_dyn" members BEFORE mvl_init_mms_objs to		*/
	/* override default maximum values.					*/
	mvl_max_dyn.types	 = 500000;
	mvl_max_dyn.dom_vars = 500000;
	mvl_max_dyn.dom_nvls = 500;
	mvl_max_dyn.aa_nvls = 20;
	mvl_max_dyn.doms = 2000;
	mvl_max_dyn.aa_vars = 20;
	mvl_max_dyn.journals = 2000;
	mvl_max_dyn.vmd_nvls = 20;
	mvl_max_dyn.vmd_vars = 2000;

	/* mvl_init_mms_objs must be called before any dynamic object creation*/
	/* It is called automatically by mvl_start_acse, but it may often be	*/
	/* better to init all objects before mvl_start_acse.			*/
	mvl_init_mms_objs ();

#if 1
	unsigned long pos;
	ST_DOUBLE mstime;
	int cnt = 0;
	stuMmsServerIed *pIed = m_Ieds.FetchFirst(pos);
	while(pIed)
	{
		if(pIed->scl_info == NULL)
		{
			pIed->scl_info = (SCL_INFO*)malloc(sizeof(SCL_INFO));
			memset(pIed->scl_info,0,sizeof(SCL_INFO));
		}
		mstime =sGetMsTime ();
		cnt++;
		OnProgressText(SString::toFormat("正在解释模型[%d/%d]: %s->%s ...",cnt,m_Ieds.count(),pIed->ied_name.data(),pIed->ap_name.data()));

		ret = scl_parse (pIed->scl_pathfile.data(),
			pIed->ied_name.data(),		/* IED name	*/
			pIed->ap_name.data(),		/* AccessPoint	*/
			(SCL_INFO *)pIed->scl_info);
		if (ret != SD_SUCCESS)
			haveError = true;
		LOGDEBUG ("解释SCL %d (file=%s,IED=%d-%s,AP=%s)结果为:%d, 用时:%fms\n", 
			cnt,pIed->scl_pathfile.data(),pIed->ied_no,pIed->ied_name.data(),pIed->ap_name.data(),ret,sGetMsTime() - mstime);
		OnProgressText(SString::toFormat("解释模型[%d/%d]: %s->%s 完毕[%s%d]  正在加载模型...",cnt,m_Ieds.count(),pIed->ied_name.data(),pIed->ap_name.data(),ret==SD_SUCCESS?"成功:":"失败:",ret));

		/* Set some Server options, then configure ALL IED from the SCL info.	*/
		serv_opt.reportScanRateMs = startup_cfg.report_scan_rate;
		serv_opt.brcb_bufsize     = startup_cfg.BRCBBufferSize;
		serv_opt.logScanRateMs    = startup_cfg.LogScanRateMs;
		serv_opt.logMaxEntries    = startup_cfg.LogMaxEntries;

		/* Create types only if everything successful up to now.	*/
		if (ret == SD_SUCCESS)
		{
			/* CRITICAL: This saves the type_id in each SCL_LN struct.	*/
			/*           Must not call again with same "scl_info".	*/
			ret = scl2_datatype_create_all (&mvl_vmd, (SCL_INFO *)pIed->scl_info, 0/*24000*/,
				SD_FALSE,	/* type names not generated	*/
				NULL);		/* type name prefix (not used)	*/
		}

		/* Create variables only if everything successful up to now.	*/
		if (ret == SD_SUCCESS)
		{
			ret = scl2_ld_create_all (&mvl_vmd, (SCL_INFO *)pIed->scl_info, serv_opt.reportScanRateMs/*G_startup_cfg->report_scan_rate*/, serv_opt.brcb_bufsize/*24000*//*brcb_bufsize*/,
				SD_FALSE);	/* create "server" model	*/
		}

		if (ret != SD_SUCCESS)
		{
			LOGERROR("IED:%d-%s/%s模型加载失败!",pIed->ied_no,pIed->ied_name.data(),pIed->ap_name.data());
			haveError = true;
		}

		OnProgressText(SString::toFormat("加载模型[%d/%d]: %s->%s %s  正在提取逻辑节点...",cnt,m_Ieds.count(),pIed->ied_name.data(),pIed->ap_name.data(),ret==SD_SUCCESS?"成功":(SString::toFormat("失败[%d]",ret).data())));
		OnParseIed(pIed);
		scl_info_destroy ((SCL_INFO *)pIed->scl_info);	/* Destroy all stored SCL info		*/
		pIed = m_Ieds.FetchNext(pos);
	}
	if (ret == SD_SUCCESS)
		ret = datamap_cfg_read ("datamap.cfg", "datamap_out.cfg");

	ret = SD_SUCCESS;
#else
	//g_SMMS_scl_parse_mode = SCL_PARSE_MODE_CID;
	if (g_SMMS_scl_parse_mode == SCL_PARSE_MODE_SCD)
	{
		/* Set parser options and call SCL parser.	*/
		options.forceEdition = 1;///2	/* Treat as Edition 2 file no matter what*/
		options.includeOwner = SD_FALSE;
		ret = scl_parse_scd_all (startup_cfg.scl_filename, &options, &g_SMMS_scl_info);
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
		g_SMMS_vmd_ctrl_arr = scl2_vmd_create_all_new (&g_SMMS_scl_info, startup_cfg.ied_name, startup_cfg.access_point_name,
			&serv_opt, &g_SMMS_vmd_count);
		if (g_SMMS_vmd_ctrl_arr)
		{
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
		ret = all_obj_create ("startup.cfg", "datamap.cfg", "datamapout.cfg", &g_SMMS_scl_info);
		if (ret != SD_SUCCESS)
			printf ("\n ERROR: Something in SCL or DATAMAP configuration FAILED.");
	}
#endif
	OnProgressText("正在映射叶节点...");
	MapLeaf();

	/* NOTE: if data in scl_info is not needed, scl_info_destroy may be	*/
	/*   called now to free up resources.					*/

#if defined(USE_DIB_LIST)	/* Use new DIB_ENTRY linked list	*/
	printf ("\n\nNOTICE: USE_DIB_LIST ENABLED: Using local address from"
		"\n        <ConnectedAP iedName='%s' apName='%s'> element"
		"\n        of SCL file='%s'.\n",
		startup_cfg.ied_name, startup_cfg.access_point_name, startup_cfg.scl_filename);
	/* Save addresses from SCL file in new global DIB_ENTRY list.		*/
	scl_dib_entry_save (&g_SMMS_scl_info);
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

	/* CRITICAL: Initialize all Report data after all other initialization.*/
	/*           This must be AFTER data mapping by "datamap_cfg_read".	*/
	mvl61850_rpt_data_init ();

	/* Set the ^c catcher */
	signal (SIGINT, ctrlCfun);

	/* We are all initialized, just service communications			*/
	printf ("\n Entering 'server' mode, hit ^c to exit ... \n");

	/* We want to know about connection activity */
	u_mvl_disc_ind_fun = disc_ind_fun;

	// 	SKT_CREATE_THREAD(ThreadMapLeaf,this);
	SKT_CREATE_THREAD(ThreadMainLoop,this);
	// 	SKT_CREATE_THREAD(ThreadRequest,this);
	//SLOGALWAYS0 ("Initialization complete, entering service loop");
	OnProgressText("MMS服务服务完毕!");
	if (haveError)
		return false;
	return true;
}

////////////////////////////////////////////////////////////////////////
// 描    述:  停止服务，派生函数中必须先调用此函数
// 作    者:  邵凯田
// 创建时间:  2016-7-23 11:11
// 参数说明:  void
// 返 回 值:  true表示服务停止成功,false表示服务停止失败
//////////////////////////////////////////////////////////////////////////
bool SMmsServer::Stop()
{
	SService::Stop();
	while(this->GetThreadCount() > 0)
		SApi::UsSleep(10000);

	ST_RET ret;

	ret = mvl_end_acse ();	/* Stop the lower layer subsystem	*/

#if defined(USE_DIB_LIST)	/* Use new DIB_ENTRY linked list	*/
	/* Free up DIB_ENTRY linked list.	*/
	dib_entry_destroy_all ();
#endif

	scl_info_destroy (&g_SMMS_scl_info);	/* Destroy all stored SCL info		*/
	/* could be done earlier if SCL info not needed*/

	if (g_SMMS_scl_parse_mode == SCL_PARSE_MODE_SCD)
	{
		/* CRITICAL: need different code to destroy ALL IED.*/
		if (g_SMMS_vmd_ctrl_arr)
			all_obj_destroy_scd (g_SMMS_vmd_ctrl_arr, g_SMMS_vmd_count);	/* Destroy all MMS objects for array of VMD.*/
		else
			all_obj_destroy ();		/* Destroy all MMS objects.		*/
	}	/* scl_parse_mode == SCL_PARSE_MODE_SCD	*/
	else
		all_obj_destroy ();		/* Destroy all MMS objects.		*/

	/* If cleanup worked, number of types logged here should be 0.	*/
	SLOGALWAYS1 ("DEBUG: Number of types in use after cleanup=%d", mvl_type_count ());

	/* Free buffers allocated by "mvl_init_mms_objs".	*/
	//MEMO: 停止时不释放MMS对象，否则下次启动会异常 [2017-4-21 17:13 邵凯田] mvl_free_mms_objs ();
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
	if(spareMem != NULL)
	{
		free(spareMem);
		spareMem = NULL;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  映射所有叶子节点
// 作    者:  邵凯田
// 创建时间:  2016-9-2 9:13
// 参数说明:  void
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void SMmsServer::MapLeaf()
{
	int i,didx,vidx;//,retcode;
	MVL_DOM_CTRL *dom;
	MVL_VAR_ASSOC *var;
	DATA_MAP_HEAD *map_head;
	DATA_MAP *data_map;
	char* domName;
	for (didx = 0; didx < mvl_vmd.num_dom; didx++)
	{
		dom = mvl_vmd.dom_tbl [didx];
		/* Loop through all "Logical Nodes" (variables).	*/
		for (vidx = 0; vidx < dom->num_var_assoc; vidx++)
		{
			var = dom->var_assoc_tbl[vidx];
			map_head = (DATA_MAP_HEAD*)var->user_info;
			
			if(map_head == NULL || map_head->map_count <= 0)
				continue;
			printf("MAPLD=%s\n",map_head->ldevice);
			//查找匹配的IED
			domName = dom->name;
			for(i=0;i<(int)map_head->map_count;i++)
			{
				data_map = map_head->map_arr[i];
				if(data_map != NULL)
				{
					//printf("LEAF MAP : %s    %s  %p\n",domName,data_map->leaf,&data_map->user_map);
					if(OnMapLeafToUser(domName,data_map->leaf,&(data_map->user_map)) && data_map->user_map != NULL)
					{
						data_map->user_map->va_ptr = var;//指向LN对应变量
						data_map->user_map->map_head_ptr = map_head;
						data_map->user_map->map_ptr = data_map;
					}
					//OnMapLeafToUser("","",NULL);
				}
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  检索指定的叶子映射指针
// 作    者:  邵凯田
// 创建时间:  2016-9-1 14:09
// 参数说明:  @sIedLdName表示IED+LD名称
//         :  @sLeafPath表示叶子节点路径
// 返 回 值:  stuLeafMap*， NULL表示未找到有效的映射记录
//////////////////////////////////////////////////////////////////////////
stuLeafMap* SMmsServer::SearchLeafMap(const char* sIedLdName, const char* sLeafPath)
{
	int i,didx,vidx;//,retcode;
	MVL_DOM_CTRL *dom;
	MVL_VAR_ASSOC *var;
	DATA_MAP_HEAD *map_head;
	DATA_MAP *data_map;
	char* domName;
	SString path = sLeafPath;
	path.replace(".","$");
	SString sLnName;
	char* sLeafPath2 = path.data();
	for (didx = 0; didx < mvl_vmd.num_dom; didx++)
	{
		dom = mvl_vmd.dom_tbl [didx];
		if(!SString::equals(dom->name,sIedLdName))
			continue;
		for (vidx = 0; vidx < dom->num_var_assoc; vidx++)
		{
			var = dom->var_assoc_tbl[vidx];
			sLnName = var->name;
			sLnName += "$";
			if(strstr(sLeafPath2,sLnName.data()) != sLeafPath2)
				continue;
			map_head = (DATA_MAP_HEAD*)var->user_info;
			if(map_head == NULL || map_head->map_count <= 0)
				break;
			domName = dom->name;
			for(i=0;i<(int)map_head->map_count;i++)
			{
				data_map = map_head->map_arr[i];
				if(data_map != NULL && SString::equals(data_map->leaf,sLeafPath2))
					return data_map->user_map;
			}
			break;
		}
		break;
	}
	return NULL;
}


//////////////////////////////////////////////////////////////////////////
// 描    述:  通过路径检索指定的叶子映射指针
// 作    者:  邵凯田
// 创建时间:  2017-4-20 15:19
// 参数说明:  @sMmsPath表示MMS路径(PL2201APROT/PTOC3$SG$StrVal)
// 返 回 值:  stuLeafMap*， NULL表示未找到有效的映射记录
//////////////////////////////////////////////////////////////////////////
stuLeafMap* SMmsServer::SearchLeafMap(const char* sMmsPath)
{
	SString sPath = sMmsPath;
	SString iedld = SString::GetIdAttribute(1,sPath,"/");
	SString leaf  = SString::GetIdAttribute(2,sPath,"/");
	return SearchLeafMap(iedld.data(),leaf.data());
}


//////////////////////////////////////////////////////////////////////////
// 描    述:  通过路径检索指定的叶子映射指针，如果指定路径找不到，则找其所有子节点
// 作    者:  邵凯田
// 创建时间:  2017-4-21 8:41
// 参数说明:  @sMmsPath表示MMS路径
//         :  @leafs为引用返回的叶子指针引用队列,类型为DATA_MAP*
// 返 回 值:  表示返回节点或子节点指针的数量，0表示找不到，<0表示失败
//////////////////////////////////////////////////////////////////////////
int SMmsServer::SearchLeafMapWithChild(const char* sMmsPath,SPtrList<void> &leafs)
{
	SString sPath = sMmsPath;
	SString iedld = SString::GetIdAttribute(1,sPath,"/");
	SString leaf  = SString::GetIdAttribute(2,sPath,"/");
	leaf.replace(".","$");
	char* sIedLdName = iedld.data();
	char* sLeafPath = leaf.data();
	SString leaf2 = leaf + "$";
	char* sLeafPath2 = leaf2.data();
	int i,didx,vidx;//,retcode;
	MVL_DOM_CTRL *dom;
	MVL_VAR_ASSOC *var;
	DATA_MAP_HEAD *map_head;
	DATA_MAP *data_map;
	char* domName;
	SString sLnName;
	leafs.clear();
	leafs.setAutoDelete(false);
	for (didx = 0; didx < mvl_vmd.num_dom; didx++)
	{
		dom = mvl_vmd.dom_tbl [didx];
		if(!SString::equals(dom->name,sIedLdName))
			continue;
		for (vidx = 0; vidx < dom->num_var_assoc; vidx++)
		{
			var = dom->var_assoc_tbl[vidx];
			sLnName = var->name;
			sLnName += "$";
			if(strstr(sLeafPath,sLnName.data()) != sLeafPath)
				continue;
			map_head = (DATA_MAP_HEAD*)var->user_info;
			if(map_head == NULL || map_head->map_count <= 0)
				break;
			domName = dom->name;
			for(i=0;i<(int)map_head->map_count;i++)
			{
				data_map = map_head->map_arr[i];
				if(data_map != NULL)
				{
					if(SString::equals(data_map->leaf,sLeafPath))
					{
						//完全相等
						leafs.clear();
						leafs.append(data_map);
						return 1;
					}
					else if(strstr(data_map->leaf,sLeafPath2) == data_map->leaf)
					{
						//找到子节点
						leafs.append(data_map);
					}
				}
			}
			break;
		}
		break;
	}
	return leafs.count();
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  MMS服务端主循环线程
// 作    者:  邵凯田
// 创建时间:  2016-7-23 11:11
// 参数说明:  this
// 返 回 值:  void*
//////////////////////////////////////////////////////////////////////////
void* SMmsServer::ThreadMainLoop(void* lp)
{
	ST_BOOLEAN event;
	SMmsServer *pThis = (SMmsServer*)lp;
	pThis->BeginThread();
	while (!pThis->IsQuit())
	{
		SApi::UsSleep(10000);
		wait_any_event (100);	/* keep this short so reporting is fast	*/
		pThis->lock();
		do
		{
			/* CRITICAL: do like this so both functions called each time through loop.*/
			event =  mvl_comm_serve ();	/* Perform communications service 	*/
		} while (event);		/* Loop until BOTH functions return 0*/
		mvl61850_rpt_service (SD_FALSE);	/* process IEC 61850 reports	*/
		mvl61850_log_service ();	/* process IEC 61850 logs	*/

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
		pThis->unlock();
	}
	pThis->EndThread();
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  根据DATA_MAP指针读取节点的值，并以字符串格式返回
// 作    者:  邵凯田
// 创建时间:  2017-4-21 13:44
// 参数说明:  @pDataMap为节点的DATA_MAP*指针
// 返 回 值:  SString表示值内容
//////////////////////////////////////////////////////////////////////////
SString SMmsServer::GetLeafVarValueByDM(void *pDataMap)
{
	DATA_MAP *map = (DATA_MAP*)pDataMap;
	if(map == NULL)
		return "";
	SString val;
	switch (map->dataType->el_tag)
	{
	case RT_BOOL:
		val.sprintf("%d",*(ST_BOOLEAN *) map->dataPtr);
		break;
	case RT_BIT_STRING:
		{
			ST_UCHAR *pSrc = (ST_UCHAR*)map->dataPtr;
			RUNTIME_TYPE *rt = map->dataType;
			ST_CHAR *text = (ST_CHAR*)val.data();
			int i;
			int j;
			int k;
			ST_INT16 *sp;
			ST_CHAR *destBuf;
			ST_UCHAR mask;

			//text[0] = 0;
			int numBits = map->dataType->u.p.el_len;
			if (numBits < 0)		/* a variable length bit string	*/
			{
				sp = (ST_INT16 *) pSrc;
				numBits = *sp;
				k=2;
			}
			else 
				k=0;
			val.SetBuffer(numBits+1);
			text = (ST_CHAR*)val.data();

			destBuf = text;
			for (i = 0; i < numBits; ++k)	/* for each byte, while bits remain	*/
			{
				mask = 0x80;
				for (j = 0; j < 8 && i < numBits; ++i, ++j)
				{
					if (pSrc[k] & mask)
						destBuf[i] = '1';
					else
						destBuf[i] = '0';
					mask >>= 1;
				}
			}
			destBuf[i] = 0;
			val.PrepareStrLen();
		}
		break;
	case RT_INTEGER:
		switch (map->dataType->u.p.el_len)
		{
		case 1:
			val.sprintf("%d",*(ST_INT8 *) map->dataPtr);
			break;
		case 2:
			val.sprintf("%d",*(ST_INT16 *) map->dataPtr);
			break;
		case 4:
			val.sprintf("%d",*(ST_INT32 *) map->dataPtr);
			break;
		}
		break;

	case RT_UNSIGNED:
		switch (map->dataType->u.p.el_len)
		{
		case 1:
			val.sprintf("%d",*(ST_UINT8 *) map->dataPtr);
			break;
		case 2:
			val.sprintf("%d",*(ST_UINT16 *) map->dataPtr);
			break;
		case 4:
			val.sprintf("%d",*(ST_UINT32 *) map->dataPtr);
			break;
		}
		break;

	case RT_FLOATING_POINT:
		switch (map->dataType->u.p.el_len)
		{
		case 4:
			val.sprintf("%f",*(ST_FLOAT *) map->dataPtr);
			break;
		case 8:
			val.sprintf("%f",*(ST_DOUBLE *) map->dataPtr);
			break;
		}
		break;

	case RT_UTC_TIME:
		{
			MMS_UTC_TIME UtcTimeNow;
			memcpy (&UtcTimeNow, map->dataPtr, sizeof (MMS_UTC_TIME));
			int soc = UtcTimeNow.secs;
			int usec = (ST_UINT32)(((double)UtcTimeNow.fraction)/(ST_DOUBLE)0x01000000*1000000.0);
			SDateTime t((time_t)soc);
			val.sprintf("%s.%06d",t.toString("yyyy-MM-dd hh:mm:ss").data(),usec);
		}
		break;
	case RT_VISIBLE_STRING:
		val = (char*)map->dataPtr;
		break;
	case RT_UTF8_STRING:
		val = (char*)map->dataPtr;
		val = val.toGb2312();
		break;
	default :
		return "";
	}
	return val;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  读取映射对应的叶子节点的值，并以字符串格式返回
// 作    者:  邵凯田
// 创建时间:  2016-9-2 9:18
// 参数说明:  @pLeafMap表示映射指针
// 返 回 值:  SString表示值内容
//////////////////////////////////////////////////////////////////////////
SString SMmsServer::GetLeafVarValue(stuLeafMap *pLeafMap)
{
	return GetLeafVarValueByDM(pLeafMap->map_ptr);
}


//////////////////////////////////////////////////////////////////////////
// 描    述:  读取指定路径的节点值，如果是复合对象，返回所有子节点带大括号的嵌套值（子节点顺序以模型中顺序为准）
// 作    者:  邵凯田
// 创建时间:  2017-4-21 8:55
// 参数说明:  @sMmsPath为MMS全路径：CL2223CTRL/GGIO26$ST$Alm2$stVal
// 返 回 值:  SString表示值内容
//////////////////////////////////////////////////////////////////////////
SString SMmsServer::GetLeafVarValueByPath(SString sMmsPath)
{
	SPtrList<void> leafs;
	void *pDataMap;
	unsigned long pos;
	int cnt = SearchLeafMapWithChild(sMmsPath,leafs);
	if(cnt <= 0)
		return "";
	if(cnt == 1 && leafs.count() == 1)
		return GetLeafVarValueByDM(leafs[0]);
	SString sVal = "{ ";
	pDataMap = leafs.FetchFirst(pos);
	while(pDataMap)
	{
		sVal += GetLeafVarValueByDM(pDataMap);
		pDataMap = leafs.FetchNext(pos);
		if(pDataMap != NULL)
			sVal += " , ";
	}
	sVal += " }";
	return sVal;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  读取指定路径的节点值，如果是复合对象，返回所有子节点带大括号的嵌套值（子节点顺序以模型中顺序为准）
// 作    者:  邵凯田
// 创建时间:  2017-4-21 8:57
// 参数说明:  @sIedLd表示IED名+逻辑节点：CL2223CTRL
//         :  @sLeafPath表示叶子节点名：GGIO26$ST$Alm2$stVal
// 返 回 值:  SString表示值内容
//////////////////////////////////////////////////////////////////////////
SString SMmsServer::GetLeafVarValueByPath(SString sIedLd,SString sLeafPath)
{
	return GetLeafVarValueByPath(sIedLd+"/"+sLeafPath);
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  设置DATA_MAP指针对应的叶子节点的值
// 作    者:  邵凯田
// 创建时间:  2017-4-21 13:48
// 参数说明:  @pDataMap为DATA_MAP*指针
//         :  @pValStr为值字符串，多节点通过'{}'和','分隔
// 返 回 值:  true表示赋值成功，false表示赋值失败
//////////////////////////////////////////////////////////////////////////
bool SMmsServer::SetLeafVarValueByDM(void* pDataMap,char *pValStr)
{
	DATA_MAP *map = (DATA_MAP*)pDataMap;
	if(map == NULL)
		return NULL;
	ST_INT32 i32;
	float f32;
	switch (map->dataType->el_tag)
	{
	case RT_BOOL:
		i32 = atoi(pValStr);
		*(ST_BOOLEAN *) map->dataPtr = (ST_BOOLEAN)i32;
		break;
	case RT_BIT_STRING:
		{
			ST_UCHAR *pSrc = (ST_UCHAR*)map->dataPtr;
			RUNTIME_TYPE *rt = map->dataType;
			ST_CHAR *text = (ST_CHAR*)pValStr;
			int i;
			int j;
			int k;
			ST_INT16 *sp;
			ST_CHAR *destBuf;
			ST_UCHAR mask;

			//text[0] = 0;
			int numBits = map->dataType->u.p.el_len;
			if (numBits < 0)		/* a variable length bit string	*/
			{
				sp = (ST_INT16 *) pSrc;
				numBits = *sp;
				k=2;
			}
			else 
				k=0;

			destBuf = text;
			for (i = 0; i < numBits; ++k)	/* for each byte, while bits remain	*/
			{
				mask = 0x80;
				for (j = 0; j < 8 && i < numBits; ++i, ++j)
				{
					if(destBuf[i] == '1')
						pSrc[k] |= mask;
					else
						pSrc[k] &= ~mask;
					mask >>= 1;
				}
			}
		}
		break;
	case RT_INTEGER:
		i32 = atoi(pValStr);
		switch (map->dataType->u.p.el_len)
		{
		case 1:
			*(ST_INT8 *) map->dataPtr = (ST_INT8)i32;
			break;
		case 2:
			*(ST_INT16 *) map->dataPtr = (ST_INT16)i32;
			break;
		case 4:
			*(ST_INT32 *) map->dataPtr = (ST_INT32)i32;
			break;
		}
		break;

	case RT_UNSIGNED:
		i32 = atoi(pValStr);
		switch (map->dataType->u.p.el_len)
		{
		case 1:
			*(ST_UINT8 *) map->dataPtr = (ST_UINT8)i32;
			break;
		case 2:
			*(ST_UINT16 *) map->dataPtr = (ST_UINT16)i32;
			break;
		case 4:
			*(ST_UINT32 *) map->dataPtr = (ST_UINT32)i32;
			break;
		}
		break;

	case RT_FLOATING_POINT:
		f32 = (float)atof(pValStr);
		switch (map->dataType->u.p.el_len)
		{
		case 4:
			*(ST_FLOAT *) map->dataPtr = (ST_FLOAT)f32;
			break;
		case 8:
			*(ST_DOUBLE *) map->dataPtr = (ST_DOUBLE)f32;
			break;
		}
		break;

	case RT_UTC_TIME:
		{
			MMS_UTC_TIME UtcTimeNow;
			SString s = pValStr;
			int soc = SDateTime::makeDateTime("yyyy-MM-dd hh:mm:ss",SString::GetIdAttribute(1,s,".")).soc();
			int usec = SString::GetIdAttributeI(2,s,".");
			UtcTimeNow.secs = soc;
			UtcTimeNow.fraction = (ST_UINT32)(((ST_DOUBLE)usec) / 1000000.0 * (ST_DOUBLE)0x01000000);
			memcpy (map->dataPtr, &UtcTimeNow, sizeof (MMS_UTC_TIME));
		}
		break;
	case RT_VISIBLE_STRING:
		//i32 = mvluRdVaCtrl->rt->u.p.el_len;mvluRdVaCtrl->primData
		//if(i32 > (long)map_entry->raw_user_data.data_len-1)
		//	i32 = map_entry->raw_user_data.data_len-1;
		strcpy((char*)map->dataPtr,(char*)pValStr);
		break;
	case RT_UTF8_STRING:
		{
			SString str = pValStr;
			str = str.toUtf8();
#ifdef WIN32
			unsigned short utf16Str[256] = {0};
			int ret = UTF8ToUnicode16((const unsigned char *)str.data(), str.length(), utf16Str, 256);
			memcpy((char*)map->dataPtr, utf16Str, 256);
#else
			memcpy((char*)map->dataPtr, str.data(), str.length());
#endif
		}
		break;
	default :
		return false;
	}
	return true;
}
//////////////////////////////////////////////////////////////////////////
// 描    述:  设置映射对应的叶子节点的值
// 作    者:  邵凯田
// 创建时间:  2016-9-1 18:41
// 参数说明:  @pLeafMap
// 返 回 值:  true表示赋值成功，false表示赋值失败
//////////////////////////////////////////////////////////////////////////
bool SMmsServer::SetLeafVarValue(stuLeafMap *pLeafMap,char *pValStr)
{
	return SetLeafVarValueByDM(pLeafMap->map_ptr,pValStr);
}

bool SMmsServer::SetLeafVarValue(stuLeafMap *pLeafMap,int val)
{
	DATA_MAP *map = (DATA_MAP*)pLeafMap->map_ptr;
	if(map == NULL)
		return NULL;
	ST_INT32 i32 = val;
	float f32;
	switch (map->dataType->el_tag)
	{
	case RT_BOOL:
		*(ST_BOOLEAN *) map->dataPtr = (ST_BOOLEAN)i32;
		break;
	case RT_INTEGER:
		switch (map->dataType->u.p.el_len)
		{
		case 1:
			*(ST_INT8 *) map->dataPtr = (ST_INT8)i32;
			break;
		case 2:
			*(ST_INT16 *) map->dataPtr = (ST_INT16)i32;
			break;
		case 4:
			*(ST_INT32 *) map->dataPtr = (ST_INT32)i32;
			break;
		}
		break;

	case RT_UNSIGNED:
		switch (map->dataType->u.p.el_len)
		{
		case 1:
			*(ST_UINT8 *) map->dataPtr = (ST_UINT8)i32;
			break;
		case 2:
			*(ST_UINT16 *) map->dataPtr = (ST_UINT16)i32;
			break;
		case 4:
			*(ST_UINT32 *) map->dataPtr = (ST_UINT32)i32;
			break;
		}
		break;

	case RT_FLOATING_POINT:
		f32 = (float)val;
		switch (map->dataType->u.p.el_len)
		{
		case 4:
			*(ST_FLOAT *) map->dataPtr = (ST_FLOAT)f32;
			break;
		case 8:
			*(ST_DOUBLE *) map->dataPtr = (ST_DOUBLE)f32;
			break;
		}
		break;
	default :
		return false;
	}
	return true;
}
bool SMmsServer::SetLeafVarValue(stuLeafMap *pLeafMap,float val)
{
	DATA_MAP *map = (DATA_MAP*)pLeafMap->map_ptr;
	if(map == NULL)
		return NULL;
	ST_INT32 i32;
	float f32;
	switch (map->dataType->el_tag)
	{
	case RT_BOOL:
		i32 = (ST_INT32)val;
		*(ST_BOOLEAN *) map->dataPtr = (ST_BOOLEAN)i32;
		break;
	case RT_INTEGER:
		i32 = (ST_INT32)val;
		switch (map->dataType->u.p.el_len)
		{
		case 1:
			*(ST_INT8 *) map->dataPtr = (ST_INT8)i32;
			break;
		case 2:
			*(ST_INT16 *) map->dataPtr = (ST_INT16)i32;
			break;
		case 4:
			*(ST_INT32 *) map->dataPtr = (ST_INT32)i32;
			break;
		}
		break;

	case RT_UNSIGNED:
		i32 = (ST_UINT32)val;
		switch (map->dataType->u.p.el_len)
		{
		case 1:
			*(ST_UINT8 *) map->dataPtr = (ST_UINT8)i32;
			break;
		case 2:
			*(ST_UINT16 *) map->dataPtr = (ST_UINT16)i32;
			break;
		case 4:
			*(ST_UINT32 *) map->dataPtr = (ST_UINT32)i32;
			break;
		}
		break;

	case RT_FLOATING_POINT:
		f32 = (float)val;
		switch (map->dataType->u.p.el_len)
		{
		case 4:
			*(ST_FLOAT *) map->dataPtr = (ST_FLOAT)f32;
			break;
		case 8:
			*(ST_DOUBLE *) map->dataPtr = (ST_DOUBLE)f32;
			break;
		}
		break;
	default :
		return false;
	}
	return true;
}
bool SMmsServer::SetLeafVarValue(stuLeafMap *pLeafMap,double val)
{
	DATA_MAP *map = (DATA_MAP*)pLeafMap->map_ptr;
	if(map == NULL)
		return NULL;
	ST_INT32 i32;
//	float f32;
	switch (map->dataType->el_tag)
	{
	case RT_BOOL:
		i32 = (ST_INT32)val;
		*(ST_BOOLEAN *) map->dataPtr = (ST_BOOLEAN)i32;
		break;
	case RT_INTEGER:
		i32 = (ST_INT32)val;
		switch (map->dataType->u.p.el_len)
		{
		case 1:
			*(ST_INT8 *) map->dataPtr = (ST_INT8)i32;
			break;
		case 2:
			*(ST_INT16 *) map->dataPtr = (ST_INT16)i32;
			break;
		case 4:
			*(ST_INT32 *) map->dataPtr = (ST_INT32)i32;
			break;
		}
		break;

	case RT_UNSIGNED:
		i32 = (ST_UINT32)val;
		switch (map->dataType->u.p.el_len)
		{
		case 1:
			*(ST_UINT8 *) map->dataPtr = (ST_UINT8)i32;
			break;
		case 2:
			*(ST_UINT16 *) map->dataPtr = (ST_UINT16)i32;
			break;
		case 4:
			*(ST_UINT32 *) map->dataPtr = (ST_UINT32)i32;
			break;
		}
		break;

	case RT_FLOATING_POINT:
		switch (map->dataType->u.p.el_len)
		{
		case 4:
			*(ST_FLOAT *) map->dataPtr = (ST_FLOAT)val;
			break;
		case 8:
			*(ST_DOUBLE *) map->dataPtr = (ST_DOUBLE)val;
			break;
		}
		break;
	default :
		return false;
	}
	return true;
}

bool SMmsServer::SetLeafVarValue(stuLeafMap *pLeafMap,int soc,int usec)
{
	DATA_MAP *map = (DATA_MAP*)pLeafMap->map_ptr;
	if(map == NULL)
		return NULL;
	switch (map->dataType->el_tag)
	{
	case RT_UTC_TIME:
		{
			MMS_UTC_TIME UtcTimeNow;
			UtcTimeNow.secs = soc;
			UtcTimeNow.fraction = (ST_UINT32)(((ST_DOUBLE)usec) / 1000000.0 * (ST_DOUBLE)0x01000000);
			memcpy (map->dataPtr, &UtcTimeNow, sizeof (MMS_UTC_TIME));
		}
		break;
	default :
		return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  根据指定的路径，设置节点的值内容
// 作    者:  邵凯田
// 创建时间:  2017-4-21 9:08
// 参数说明:  @sMmsPath表示MMS节点全路径
//         :  @sVal表示值内容，如果是针对非叶子节点，值为大括号加逗号分隔的值串，顺序应与读取返回的顺序一致
// 返 回 值:  true表示赋值成功，false表示赋值失败
//////////////////////////////////////////////////////////////////////////
bool SMmsServer::SetLeafVarValueByPath(SString sMmsPath, SString sVal)
{
	SPtrList<void> leafs;
	void *pDataMap;
	unsigned long pos;
	int cnt = SearchLeafMapWithChild(sMmsPath,leafs);
	if(cnt <= 0)
		return false;
	SString sLeafPath;
	char *pVal = sVal.data();
	bool bSpace=false;
	if(*pVal == '{')
	{
		pVal++;
		bSpace = true;
	}
	char *pVal2;
	int slen;
	pDataMap = leafs.FetchFirst(pos);
	while(pDataMap)
	{
		if(pVal == NULL)
			return false;
		while(bSpace && *pVal == ' ')
			pVal ++;
		pVal2 = strstr(pVal,",");
		if(pVal2 == NULL)
			pVal2 = strstr(pVal,"}");
		if(pVal2 != NULL)
		{
			*pVal2 = '\0';
			pVal2 ++;
		}
		slen = strlen(pVal);
		while(bSpace && slen>0 && pVal[slen-1]==' ')
		{
			pVal[slen-1] = '\0';
			slen --;
		}
		if(!SetLeafVarValueByDM(pDataMap,pVal))
			return false;
		pVal = pVal2;
		pDataMap = leafs.FetchNext(pos);
	}
	return true;
}

bool SMmsServer::SetLeafVarValueForDgt(SString sMmsPath, int val, SDateTime dt, SString &sErr)
{
	stuLeafMap *leafMap = SearchLeafMap(sMmsPath);
	if (!leafMap)
	{
		sErr = "获取子节点失败";
		return false;
	}
	
	DATA_MAP *map = (DATA_MAP*)leafMap->map_ptr;
	if(map == NULL)
	{
		sErr = "获取子节点MAP指针失败";
		return false;
	}

	switch (map->dataType->el_tag)
	{
	case RT_BOOL:
		{
			if (!SetLeafVarValue(leafMap, val))
			{
				sErr = "设值失败";
				return false;
			}
		}
		break;
	case RT_BIT_STRING:
		{
			char cVal[3] = {'\0'};
			if (val == 0)
				strcpy(cVal,"01");
			else if (val == 1)
				strcpy(cVal,"10");
			else
				strcpy(cVal,"00");
			if (!SetLeafVarValue(leafMap, cVal))
			{
				sErr = "设值失败";
				return false;
			}
		}
		break;
	}

	DATA_MAP_HEAD *map_head = (DATA_MAP_HEAD*)leafMap->map_head_ptr;
	if (!map_head)
	{
		sErr = "获取头节点失败";
		return false;
	}

	SString leaf  = SString::GetIdAttribute(2,sMmsPath,"/").replace(".","$");
	SString leaf2;
	int count = SString::GetAttributeCount(leaf, "$");
	if (count >= 3)
	{
		leaf2 += SString::GetIdAttribute(1,leaf,"$") + "$";
		leaf2 += SString::GetIdAttribute(2,leaf,"$") + "$";
		leaf2 += SString::GetIdAttribute(3,leaf,"$") + "$";
		char* sLeafPath = leaf2.data();
		for(int i = 0; i < (int)map_head->map_count; i++)
		{
			DATA_MAP *data_map = map_head->map_arr[i];
			if (data_map && strstr(data_map->leaf,sLeafPath) == data_map->leaf)
			{
				switch (data_map->dataType->el_tag)
				{
				case RT_UTC_TIME:
					{
						//SDateTime dt = SDateTime::currentDateTime();
						dt.addHours(-8); //UTC时间
						SString sVal = dt.toString("yyyy-MM-dd hh:mm:ss.zzzuuu");
						SetLeafVarValueByDM(data_map,sVal.data());
					}
					break;
				default:
					break;
				}
			}
		}
	}

	return true;
}

bool SMmsServer::SetLeafVarValueForAna(SString sMmsPath, float val, SDateTime dt, SString &sErr)
{
	stuLeafMap *leafMap = SearchLeafMap(sMmsPath);
	if (!leafMap)
	{
		sErr = "获取子节点失败";
		return false;
	}

	if (!SetLeafVarValue(leafMap, val))
	{
		sErr = "设值失败";
		return false;
	}

	DATA_MAP_HEAD *map_head = (DATA_MAP_HEAD*)leafMap->map_head_ptr;
	if (!map_head)
	{
		sErr = "获取头节点失败";
		return false;
	}

	SString leaf  = SString::GetIdAttribute(2,sMmsPath,"/").replace(".","$");
	SString leaf2;
	int count = SString::GetAttributeCount(leaf, "$");
	if (count >= 3)
	{
		leaf2 += SString::GetIdAttribute(1,leaf,"$") + "$";
		leaf2 += SString::GetIdAttribute(2,leaf,"$") + "$";
		leaf2 += SString::GetIdAttribute(3,leaf,"$") + "$";
		char* sLeafPath = leaf2.data();
		for(int i = 0; i < (int)map_head->map_count; i++)
		{
			DATA_MAP *data_map = map_head->map_arr[i];
			if (data_map && strstr(data_map->leaf,sLeafPath) == data_map->leaf)
			{
				switch (data_map->dataType->el_tag)
				{
				case RT_UTC_TIME:
					{
						//SDateTime dt = SDateTime::currentDateTime();
						dt.addHours(-8); //UTC时间
						SString sVal = dt.toString("yyyy-MM-dd hh:mm:ss.zzzuuu");
						SetLeafVarValueByDM(data_map,sVal.data());
					}
					break;
				default:
					break;
				}
			}
		}
	}

	return true;
}

bool SMmsServer::SetLeafVarValueForCtlDgt(SString sMmsPath, int val, SDateTime dt, SString &sErr)
{
	SPtrList<void> leafs;
	unsigned long pos;
	int cnt = SearchLeafMapWithChild(sMmsPath,leafs);
	if(cnt <= 0)
	{
		sErr = "获取子节点失败";
		return false;
	}

	DATA_MAP *pDataMap = (DATA_MAP *)leafs.FetchFirst(pos);
	while(pDataMap)
	{
		switch (pDataMap->dataType->el_tag)
		{
		case RT_BOOL:
			{
				SString sVal = SString::toFormat("%d",val);
				SetLeafVarValueByDM(pDataMap,sVal.data());
			}
			break;
		case RT_BIT_STRING:
			{
				char cVal[3] = {'\0'};
				if (val == 0)
					strcpy(cVal,"01");
				else if (val == 1)
					strcpy(cVal,"10");
				else
					strcpy(cVal,"00");
				SetLeafVarValueByDM(pDataMap,cVal);
			}
			break;
		case RT_UTC_TIME:
			{
				//SDateTime dt = SDateTime::currentDateTime();
				dt.addHours(-8); //UTC时间
				SString sVal = dt.toString("yyyy-MM-dd hh:mm:ss.zzzuuu");
				SetLeafVarValueByDM(pDataMap,sVal.data());
			}
			break;
		default:
			break;
		}
		pDataMap = (DATA_MAP *)leafs.FetchNext(pos);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  写一个新的报告
// 作    者:  邵凯田
// 创建时间:  2016-9-2 9:10
// 参数说明:  @pLeafMap为叶子节点映射指针
//         :  @val表示值
// 返 回 值:  true表示报告节点赋值成功，false表示赋值失败
//////////////////////////////////////////////////////////////////////////
bool SMmsServer::NewReportPush(stuLeafMap *pLeafMap,char *pValStr)
{
	PushNewReport(pLeafMap);
	return SetLeafVarValue(pLeafMap,pValStr);
}
bool SMmsServer::NewReportPush(stuLeafMap *pLeafMap,int val)
{
	PushNewReport(pLeafMap);
	return SetLeafVarValue(pLeafMap,val);
}
bool SMmsServer::NewReportPush(stuLeafMap *pLeafMap,float val)
{
	PushNewReport(pLeafMap);
	return SetLeafVarValue(pLeafMap,val);
}
bool SMmsServer::NewReportPush(stuLeafMap *pLeafMap,double val)
{
	PushNewReport(pLeafMap);
	return SetLeafVarValue(pLeafMap,val);
}
bool SMmsServer::NewReportPush(stuLeafMap *pLeafMap,int soc,int usec)
{
	PushNewReport(pLeafMap);
	return SetLeafVarValue(pLeafMap,soc,usec);
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  处理新的报告，通过扫描报告，使所有NewReportPush更改的报告发出
// 作    者:  邵凯田
// 创建时间:  2016-9-2 9:10
// 参数说明:  void
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void SMmsServer::NewReportProc()
{
	m_NewReportPush.clear();
	lock();
	mvl61850_rpt_service (SD_TRUE);	/* process IEC 61850 reports	*/
	unlock();
}


//////////////////////////////////////////////////////////////////////////
// 描    述:  将叶子加入报告暂存队列，如果已经存在则先处理报告再存入
// 作    者:  邵凯田
// 创建时间:  2016-9-2 9:14
// 参数说明:  @map为报告对应的叶子节点
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void SMmsServer::PushNewReport(stuLeafMap *map)
{
	//return;
	if(m_NewReportPush.exist(map))
	{
		NewReportProc();
	}
	m_NewReportPush.append(map);
}


/*
bool SMmsServer::FillReadVar(void *mvluRdVaCtrl0,char *pValStr)
{
	MVLU_RD_VA_CTRL *mvluRdVaCtrl = (MVLU_RD_VA_CTRL*)mvluRdVaCtrl0;
	ST_INT32 i32;
	float f32;
	
	switch (mvluRdVaCtrl->rt->el_tag)
	{
	case RT_INTEGER:
		//memcpy(&i32,mvluRdVaCtrl->primData.data_ptr,4);
		i32 = atoi(pValStr);
		switch (mvluRdVaCtrl->rt->u.p.el_len)
		{
		case 1:
			*(ST_INT8 *) mvluRdVaCtrl->primData = (ST_INT8)i32;
			break;
		case 2:
			*(ST_INT16 *) mvluRdVaCtrl->primData = (ST_INT16)i32;
			break;
		case 4:
			*(ST_INT32 *) mvluRdVaCtrl->primData = (ST_INT32)i32;
			break;
		}
		break;

	case RT_UNSIGNED:
		//memcpy(&i32,mvluRdVaCtrl->primData,4);
		i32 = atoi(pValStr);
		switch (mvluRdVaCtrl->rt->u.p.el_len)
		{
		case 1:
			*(ST_UINT8 *) mvluRdVaCtrl->primData = (ST_UINT8)i32;
			break;
		case 2:
			*(ST_UINT16 *) mvluRdVaCtrl->primData = (ST_UINT16)i32;
			break;
		case 4:
			*(ST_UINT32 *) mvluRdVaCtrl->primData = (ST_UINT32)i32;
			break;
		}
		break;

	case RT_FLOATING_POINT:
		//memcpy(&f32,mvluRdVaCtrl->primData,4);
		f32 = (float)atof(pValStr);
		switch (mvluRdVaCtrl->rt->u.p.el_len)
		{
		case 4:
			*(ST_FLOAT *) mvluRdVaCtrl->primData = (ST_FLOAT)f32;
			break;
		case 8:
			*(ST_DOUBLE *) mvluRdVaCtrl->primData = (ST_DOUBLE)f32;
			break;
		}
		break;

	case RT_UTC_TIME:
		{
			MMS_UTC_TIME UtcTimeNow;
			SString s = pValStr;
			int soc = SString::GetIdAttributeI(1,s,",");
			int usec = SString::GetIdAttributeI(2,s,",");
			UtcTimeNow.secs = soc;
			UtcTimeNow.fraction = (ST_UINT32)(((ST_DOUBLE)usec) / 1000000.0 * (ST_DOUBLE)0x01000000);
			memcpy (mvluRdVaCtrl->primData, &UtcTimeNow, sizeof (MMS_UTC_TIME));
		}
		break;
	case RT_VISIBLE_STRING:
		//i32 = mvluRdVaCtrl->rt->u.p.el_len;mvluRdVaCtrl->primData
		//if(i32 > (long)map_entry->raw_user_data.data_len-1)
		//	i32 = map_entry->raw_user_data.data_len-1;
		strcpy((char*)mvluRdVaCtrl->primData,(char*)pValStr);
	default :
		return false;
	}
	return true;
}

bool SMmsServer::FillReadVar(void *mvluRdVaCtrl0,int val)
{
	MVLU_RD_VA_CTRL *mvluRdVaCtrl = (MVLU_RD_VA_CTRL*)mvluRdVaCtrl0;
	switch (mvluRdVaCtrl->rt->el_tag)
	{
	case RT_INTEGER:
		switch (mvluRdVaCtrl->rt->u.p.el_len)
		{
		case 1:
			*(ST_INT8 *) mvluRdVaCtrl->primData = (ST_INT8)val;
			break;
		case 2:
			*(ST_INT16 *) mvluRdVaCtrl->primData = (ST_INT16)val;
			break;
		case 4:
			*(ST_INT32 *) mvluRdVaCtrl->primData = (ST_INT32)val;
			break;
		}
		break;

	case RT_UNSIGNED:
		switch (mvluRdVaCtrl->rt->u.p.el_len)
		{
		case 1:
			*(ST_UINT8 *) mvluRdVaCtrl->primData = (ST_UINT8)val;
			break;
		case 2:
			*(ST_UINT16 *) mvluRdVaCtrl->primData = (ST_UINT16)val;
			break;
		case 4:
			*(ST_UINT32 *) mvluRdVaCtrl->primData = (ST_UINT32)val;
			break;
		}
		break;
	default :
		return false;
	}
	return true;
}

bool SMmsServer::FillReadVar(void *mvluRdVaCtrl0,float val)
{
	MVLU_RD_VA_CTRL *mvluRdVaCtrl = (MVLU_RD_VA_CTRL*)mvluRdVaCtrl0;
	switch (mvluRdVaCtrl->rt->el_tag)
	{
	case RT_FLOATING_POINT:
		switch (mvluRdVaCtrl->rt->u.p.el_len)
		{
		case 4:
			*(ST_FLOAT *) mvluRdVaCtrl->primData = (ST_FLOAT)val;
			break;
		case 8:
			*(ST_DOUBLE *) mvluRdVaCtrl->primData = (ST_DOUBLE)val;
			break;
		}
		break;
	default :
		return false;
	}
	return true;
}

bool SMmsServer::FillReadVar(void *mvluRdVaCtrl0,double val)
{
	MVLU_RD_VA_CTRL *mvluRdVaCtrl = (MVLU_RD_VA_CTRL*)mvluRdVaCtrl0;

	switch (mvluRdVaCtrl->rt->el_tag)
	{
	case RT_FLOATING_POINT:
		switch (mvluRdVaCtrl->rt->u.p.el_len)
		{
		case 4:
			*(ST_FLOAT *) mvluRdVaCtrl->primData = (ST_FLOAT)val;
			break;
		case 8:
			*(ST_DOUBLE *) mvluRdVaCtrl->primData = (ST_DOUBLE)val;
			break;
		}
		break;
	default :
		return false;
	}
	return true;
}

bool SMmsServer::FillReadVar(void *mvluRdVaCtrl0,int soc,int usec)
{
	MVLU_RD_VA_CTRL *mvluRdVaCtrl = (MVLU_RD_VA_CTRL*)mvluRdVaCtrl0;
	switch (mvluRdVaCtrl->rt->el_tag)
	{
	case RT_UTC_TIME:
		{
			MMS_UTC_TIME UtcTimeNow;
			UtcTimeNow.secs = soc;
			UtcTimeNow.fraction = (ST_UINT32)(((ST_DOUBLE)usec) / 1000000.0 * (ST_DOUBLE)0x01000000);
			memcpy (mvluRdVaCtrl->primData, &UtcTimeNow, sizeof (MMS_UTC_TIME));
		}
		break;
	default :
		return false;
	}
	return true;
}
*/

//////////////////////////////////////////////////////////////////////////
// 描    述:  设置文件服务的根目录
// 作    者:  邵凯田
// 创建时间:  2016-8-29 18:39
// 参数说明:  @sPath为目录名
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void SMmsServer::SetRootPath(SString sPath)
{
	m_sRootPath = sPath;
	if(m_sRootPath.length() == 0)
		return ;
#ifdef WIN32
	if(m_sRootPath.Right(1) != "\\" && m_sRootPath.Right(1) != "/")
		m_sRootPath += "\\";
#else
	if(m_sRootPath.Right(1) != "/")
		m_sRootPath += "/";
#endif
};


//////////////////////////////////////////////////////////////////////////
// 描    述:  取MVL目录内容
// 作    者:  邵凯田
// 创建时间:  2016-09-02 10:04
// 参数说明:  @sPath为目录名称
//         :  @slFiles这目录内容
//         :  @bNeedToSort表示是否需要排序，true排序，false不排序
// 返 回 值:  >0表示内容数量，<0表示失败, =0表示内容为空
//////////////////////////////////////////////////////////////////////////
int SMmsServer::OnServerDirectory(SString sPathIn,SPtrList<stuSMmsFileInfo> &slFiles,bool bNeedToSort/*=false*/)
{
	int p;
	SString sFileFilter;
	SString sPath = m_sRootPath+sPathIn;

	std::vector<SString > keywordList;
	keywordList.push_back("\\*");
	keywordList.push_back("\\*.*");
	keywordList.push_back("/*");
	keywordList.push_back("/*.*");

	for (std::size_t i = 0; i < keywordList.size(); ++i)
	{
		SString keyword = keywordList.at(i);
		if((p = sPath.find(keyword)) >= 0)
		{
			sFileFilter = sPath.Mid(p+1);
			sPath = sPath.Left(p);
		}
	}
	
	sFileFilter = sFileFilter.replace((char*)"*",(char*)"").toLower();
	SDir dir(sPath);
#ifdef WIN32
	if(sPath.Right(1) != "\\" && sPath.Right(1) != "/")
		sPath += "\\";
#else
	if(sPath.Right(1) != "/")
		sPath += "/";
#endif

	// 去除路径中的结尾
	SString sPathReturn = sPathIn;
	if (sPathReturn.right(2) == "\\*" || sPathReturn.right(2) == "/*")
		sPathReturn = sPathReturn.left(sPathReturn.length() - 2);
	else if (sPathReturn.right(4) == "\\*.*" || sPathReturn.right(4) == "/*.*")
		sPathReturn = sPathReturn.left(sPathReturn.length() - 4);
	else
	{
		while (sPathReturn.length() > 0 && (sPathReturn.right(1) == "\\" || sPathReturn.right(1) == "/"))
			sPathReturn = sPathReturn.left(sPathReturn.length() - 1);
	}

	if(sPathReturn.length() > 0 && sPathReturn.Right(1) != "/")
		sPathReturn += "/";

	int i,cnt = dir.count();
	stuSMmsFileInfo *pInfo;
	for(i=0;i<cnt;i++)
	{
		SString sFile = dir[i];
		if(sFile == "." || sFile == "..")
			continue;
		pInfo = new stuSMmsFileInfo;
		pInfo->filename = sPathReturn + sFile;
		SString attr = dir.attribute(i);
		if(SString::GetAttributeValueI(attr,"isdir") == 1)
		{
			pInfo->fsize = 0;
			pInfo->mtime = 0;
			pInfo->mtimpres = false;
		}
		else
		{
			if(sFileFilter.length() > 0 && sFile.toLower().find(sFileFilter) < 0)
				continue;
			sFile = sPath+sFile;
			pInfo->fsize = SFile::fileSize(sFile);
			pInfo->mtime = SFile::filetime(sFile).soc();
			pInfo->mtimpres = true;
		}
		if(!bNeedToSort)
			slFiles.append(pInfo);
		else
		{
			//sort
			unsigned long pos;
			stuSMmsFileInfo *p = slFiles.FetchFirst(pos);
			while(p)
			{
				if(pInfo->filename <= p->filename)
					break;
				p = slFiles.FetchNext(pos);
			}
			slFiles.insertBefore(p,pInfo,false);
		}
	}
	return slFiles.count();
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  读取服务端文件
// 作    者:  邵凯田
// 创建时间:  2016-09-02 10:10
// 参数说明:  @sFileName表示服务端文件名称
// 返 回 值:  CMmsMemFile*, 表示加载到内存的文件内容，NULL表示文件打开或读取失败
//////////////////////////////////////////////////////////////////////////
SMmsServer::CMmsMemFile* SMmsServer::OnReadServerFile(SString sFileName)
{
	SDir d(m_sRootPath+sFileName);
	if (d.count() > 0 && d.IsDir(d.count() - 1))
	{
		LOGERROR("read directory(%s) as file, refused read", (m_sRootPath+sFileName).data());
		return NULL;
	}
	SFile f(m_sRootPath+sFileName);
	if(!f.open(IO_ReadOnly))
		return NULL;	
	CMmsMemFile *pFile = new CMmsMemFile();
	pFile->m_iLen = f.size();
	pFile->m_pBuffer = new BYTE[pFile->m_iLen+1];
	pFile->m_iReadPos = 0;
	pFile->m_tFileModifyTime = f.filetime(f.filename()).soc();
	pFile->m_tOpenTime = (int)SDateTime::getNowSoc();
	f.seekBegin();
	int ret = pFile->m_iLen==0?0:f.readBlock(pFile->m_pBuffer,pFile->m_iLen);
	if(ret != pFile->m_iLen)
	{
		delete pFile;
		return NULL;
	}
	return pFile;
}


//////////////////////////////////////////////////////////////////////////
// 描    述:  写入服务端文件
// 作    者:  邵凯田
// 创建时间:  2016-09-02 10:12
// 参数说明:  @sFileName表示服务端文件名称
//         :  @pBuf表示文件内容缓冲区
//         :  @iLen表示内容长度（字节）
// 返 回 值:  >0表示写入成功，=0表示写入0，<0表示失败
//////////////////////////////////////////////////////////////////////////
int SMmsServer::OnWriteServerFile(SString sFileName/*,unsigned char* pBuf,int iLen*/)
{
	LOGWARN("服务器收到客户端上传的文件:%s",sFileName.data());
	return 0;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  删除服务端文件
// 作    者:  邵凯田
// 创建时间:  2016-09-02 10:12
// 参数说明:  @sFileName表示服务端文件名称，相对于文件服务根路径，如有子目录应包含子目录
// 返 回 值:  >0表示删除成功，=0表示文件不存在，<0表示删除失败
//////////////////////////////////////////////////////////////////////////
int SMmsServer::OnDeleteServerFile(SString sFileName)
{
	while(1)
	{
		if(sFileName.length() == 0)
			return 0;
		char ch = sFileName.at(0);
		if(ch == '\\' || ch == '/')
			sFileName = sFileName.Mid(1);
		else
			break;
	}
	sFileName = m_sRootPath + sFileName;
	if(!SFile::exists(sFileName))
		return 0;
	if(SFile::remove(sFileName))
		return 1;

	return -100;
}


//////////////////////////////////////////////////////////////////////////
// 描    述:  修改服务端文件名称
// 作    者:  邵凯田
// 创建时间:  2016-9-5 10:30
// 参数说明:  @sOldFileName表示原文件名称，相对于文件服务根路径
//         :  @sNewFileName表示新文件名称，相对于文件服务根路径
// 返 回 值:  >0表示改名成功，=0表示原文件不存在,=-1表示新文件已存在，其它<0表示改名失败
//////////////////////////////////////////////////////////////////////////
int SMmsServer::OnRenameServerFile(SString sOldFileName,SString sNewFileName)
{
	while(1)
	{
		if(sOldFileName.length() == 0)
			return -1;
		char ch = sOldFileName.at(0);
		if(ch == '\\' || ch == '/')
			sOldFileName = sOldFileName.Mid(1);
		else
			break;
	}
	while(1)
	{
		if(sNewFileName.length() == 0)
			return -1;
		char ch = sNewFileName.at(0);
		if(ch == '\\' || ch == '/')
			sNewFileName = sNewFileName.Mid(1);
		else
			break;
	}
	sOldFileName = m_sRootPath + sOldFileName;
	if(!SFile::exists(sOldFileName))
		return 0;
	sNewFileName = m_sRootPath + sNewFileName;
	if(SFile::exists(sNewFileName))
		return -1;
	if(SFile::rename(sOldFileName,sNewFileName))
		return 1;
	return -100;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  取映射节点对应的IED_LD名称
// 作    者:  邵凯田
// 创建时间:  2016-9-3 9:59
// 参数说明:  @map为映射指针
// 返 回 值:  char*, ""表示无效指针
//////////////////////////////////////////////////////////////////////////
char* SMmsServer::GetLeafMapIedLdName(stuLeafMap *map)
{
	if(map == NULL || map->map_head_ptr == NULL)
		return "";
	return ((DATA_MAP_HEAD*)map->map_head_ptr)->ldevice;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  取映射节点对应的节点名称
// 作    者:  邵凯田
// 创建时间:  2016-9-3 9:59
// 参数说明:  @map为映射指针
// 返 回 值:  char*, ""表示无效指针
//////////////////////////////////////////////////////////////////////////
char* SMmsServer::GetLeafMapLeaf(stuLeafMap *map)
{
	if(map == NULL || map->map_ptr == NULL)
		return "";
	return ((DATA_MAP*)map->map_ptr)->leaf;
}

bool SMmsServer::OnParseIed( stuMmsServerIed *ied )
{
	SCL_INFO *scl_info = (SCL_INFO *)ied->scl_info;

	if (scl_info == NULL || scl_info->serverHead == NULL || scl_info->serverHead->ldHead == NULL)
		return false;
	
	SCL_SERVER * serverHead = NULL;
	SCL_LD *ldHead = NULL;
	SCL_LN *lnHead = NULL;
	SCL_DAI *daiHead = NULL;

	for (serverHead = (SCL_SERVER *) list_find_last ((DBL_LNK *) scl_info->serverHead);
		serverHead != NULL;
		serverHead = (SCL_SERVER *) list_find_prev ((DBL_LNK *) scl_info->serverHead, (DBL_LNK *) serverHead))
	{
		// ldHead
		for (ldHead = (SCL_LD *) list_find_last ((DBL_LNK *) serverHead->ldHead);
			ldHead != NULL;
			ldHead = (SCL_LD *) list_find_prev ((DBL_LNK *) serverHead->ldHead, (DBL_LNK *) ldHead))
		{
			// lnHead
			for (lnHead = (SCL_LN *) list_find_last ((DBL_LNK *) ldHead->lnHead);
				lnHead != NULL;
				lnHead = (SCL_LN *) list_find_prev ((DBL_LNK *) ldHead->lnHead, (DBL_LNK *) lnHead))
			{
				// daiHead
				for (daiHead = (SCL_DAI *) list_find_last ((DBL_LNK *) lnHead->daiHead);
					daiHead != NULL;
					daiHead = (SCL_DAI *) list_find_prev ((DBL_LNK *) lnHead->daiHead, (DBL_LNK *) daiHead))
				{
					OnMapDaiToUser(serverHead->iedName, ldHead->inst, lnHead->varName, daiHead->flattened, daiHead->valKind, daiHead->sAddr);
					if (strlen(daiHead->sAddr) > 0)
					{
						SString str = SString::toFormat("%s%s/%s$%s", serverHead->iedName, ldHead->inst, lnHead->varName, daiHead->flattened);
						m_mapAddr[str.data()] = SString(daiHead->sAddr);
					}
				}
			}
		}
	}
	
	return true;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  设置服务绑定的IP地址，默认为0.0.0.0
// 作    者:  邵凯田
// 创建时间:  2016-9-2 9:14
// 参数说明:  @ip为地址
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void SMmsServer::SetBindIPAddr(SString ip)
{
	m_sBindIPAddr = ip;
}
