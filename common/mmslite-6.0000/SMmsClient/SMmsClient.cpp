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

#include "SMmsClient.h"


#ifdef __cplusplus
extern "C" {
#endif

	/************************************************************************/
#include "glbtypes.h"
#include "sysincs.h"
#include "signal.h"
#include "mmsdefs.h"
#include "mms_pvmd.h"
#include "mms_pvar.h"
#include "mms_vvar.h"
#include "mms_err.h"
#include "mms_pcon.h"
#include "asn1defs.h"
#include "mmsop_en.h"

#include "mvl_acse.h"
#include "mvl_log.h"
#include "tp4api.h"
#include "clntobj.h"
#include "client.h"
#include "smpval.h"

#include "lean_a.h"	/* need for DIB_ENTRY if HARD_CODED_CFG	*/
#include "scl.h"
#include "sx_arb.h"	/* needed for sxd_process..	*/
#include "mloguser.h"	/* needed for mlog_arb_log_ctrl	*/
#include "utf2gb.h"

#if defined(GOOSE_RX_SUPP) || defined(GSSE_RX_SUPP) || defined(GSE_MGMT_RX_SUPP) || defined(SMPVAL_RX_SUPP)
#include "subnet.h"
#endif
#include "str_util.h"	/* for strncat_maxstrlen, etc.			*/

	//////////////////////////////////////////////////////////////////////////
	// 名    称:  stuSMmsClient_UserInfo
	// 作    者:  邵凯田
	// 创建时间:  2015-9-22 14:07
	// 描    述:  会话对应的用户信息指针结构定义
	//////////////////////////////////////////////////////////////////////////
	struct stuSMmsClient_UserInfo
	{
		SMmsClient *pMmsClient;
		ALL_RCB_INFO *pALL_RCB_INFO;
	};

	RPT_TYPEIDS *g_pRPT_TYPEIDS = NULL;

	MVL_VMD_CTRL *config_iec_remote_vmd (
		ST_CHAR *scl_filename,
		ST_CHAR *ied_name,
		ST_CHAR *access_point_name,
		SCL_INFO *remote_scl_info);	/* filled in by this function	*/
	ST_VOID log_iec_remote_vmd_var_names (MVL_VMD_CTRL *vmd_ctrl);
	ST_VOID test_iec_remote_vmd (
		MVL_NET_INFO *net_info,
		MVL_VMD_CTRL *vmd_ctrl);

#if defined(MVL_GOOSE_SUPPORT)
	/* Need parts of "iecgoose" sample app	*/
#include "iec_demo.h"	/* definitions from "iecgoose" sample app	*/
	/* Prototype here because it needs struct def from "iec_demo.h".	*/
	IEC_GOOSE_RX_USER_INFO *goose_init (ST_VOID);
#endif

	/*----------------------------------------------------------------------*/
	/* NOTE: The MMS-EASE Lite Secured applications (Client, Server) 	*/
	/*	 are designed to work with Security Toolkit for MMS-EASE Lite	*/
	/*	 (LITESECURE-000-001).						*/
	/*       The S_SEC_ENABLED delimits the security related code.		*/
	/*----------------------------------------------------------------------*/
#if defined(S_SEC_ENABLED) && defined(ACSE_AUTH_ENABLED)
#error Only one S_SEC_ENABLED or ACSE_AUTH_ENABLED may be defined
#endif

#if defined(S_SEC_ENABLED)
#include "mmslusec.h"
#endif

#include "fkeydefs.h"
#ifdef kbhit	/* CRITICAL: fkeydefs may redefine kbhit. DO NOT want that.*/
#undef kbhit
#endif

	/************************************************************************/
	/* For debug version, use a static pointer to avoid duplication of 	*/
	/* __FILE__ strings.							*/
	/************************************************************************/

#ifdef DEBUG_SISCO
	SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

	/************************************************************************/
	/* Identify server information						*/

#if defined(S_SEC_ENABLED)
	IDENT_RESP_INFO identify_response_info =
	{
		"SISCO",  				/* Vendor 	*/
		S_SEC_LITESECURE_NAME,  		/* Model  	*/
		S_SEC_LITESECURE_VERSION_STR	,  0,	/* Version 	*/
	};
#else
	IDENT_RESP_INFO identify_response_info =
	{
		"SISCO",  		/* Vendor 	*/
		MMSLITE_NAME,  	/* Model  	*/
		MMSLITE_VERSION,  0,	/* Version 	*/
	};
#endif


	/************************************************************************/
	void ctrlCfun (int);
	static ST_VOID kbService (ST_VOID);

	static MVL_NET_INFO *connectToServer (ST_CHAR *clientARName, ST_CHAR *serverARName);

	static ST_RET disconnectFromServer (MVL_NET_INFO *clientNetInfo);

	static ST_VOID disc_ind_fun (MVL_NET_INFO *cc, ST_INT discType);

	static ST_RET getFile (MVL_NET_INFO *clientNetInfo, ST_CHAR *loc_file, ST_CHAR *rem_file);

	ST_VOID test_simple_requests (MVL_NET_INFO *clientNetInfo);
	RCB_INFO *test_iec_rpt (MVL_NET_INFO *clientNetInfo, RPT_TYPEIDS *rpt_typeids,
		ST_CHAR *dom_name,	/* domain which contains the RCB*/
		ST_CHAR *rcb_name);	/* RCB (e.g. "LLN0$BR$PosReport")*/

	ST_INT timeOut = 60;	/* timeout (in seconds) passed to waitReqDone	*/
	time_t startTime;

	INIT_INFO callingInitInfo;
	INIT_INFO initRespInfo;
	ST_BOOLEAN doIt = SD_TRUE;
	bool g_bSingleThreadMode = false;

	/************************************************************************/
#if defined(HARD_CODED_CFG)

	DIB_ENTRY localDibTable[] =
	{
		{
			NULL,	/* next (Linked List ptr)*/	/* NOTE: added for new DIB_ENTRY*/
				NULL,	/* prev (Linked List ptr)*/	/* NOTE: added for new DIB_ENTRY*/
				0, 		/* reserved		*/
				"local1",	/* name[65]		*/
				SD_TRUE,	/* local		*/
			{		/* ae_title		*/	/* NOTE: added for new DIB_ENTRY*/
				SD_TRUE,	/* AP_title_pres	*/
				{	/* AP_title		*/
					4,	/* num_comps		*/
						1, 3, 9999, 105
				},
				SD_FALSE,	/* AP_inv_id_pres	*/
				0,	/* AP_invoke_id		*/
				SD_TRUE,	/* AE_qual_pres		*/
				33,	/* AE_qual		*/
				SD_FALSE,	/* AE_inv_id_pres	*/
				0,	/* AE_invoke_id		*/
			},	/* end ae_title		*/	/* NOTE: added for new DIB_ENTRY*/

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
		}
	};

	DIB_ENTRY remoteDibTable[] =
	{
		{
			NULL,	/* next (Linked List ptr)*/	/* NOTE: added for new DIB_ENTRY*/
				NULL,	/* prev (Linked List ptr)*/	/* NOTE: added for new DIB_ENTRY*/
				0, 		/* reserved		*/
				"remote1",	/* name[65]		*/
				SD_TRUE,	/* local		*/
			{		/* ae_title		*/	/* NOTE: added for new DIB_ENTRY*/
				SD_TRUE,	/* AP_title_pres	*/
				{	/* AP_title		*/
					4,	/* num_comps		*/
						1, 3, 9999, 106
				},
				SD_FALSE,	/* AP_inv_id_pres	*/
				0,	/* AP_invoke_id		*/
				SD_TRUE,	/* AE_qual_pres		*/
				33,	/* AE_qual		*/
				SD_FALSE,	/* AE_inv_id_pres	*/
				0,	/* AE_invoke_id		*/
			},	/* end ae_title		*/	/* NOTE: added for new DIB_ENTRY*/

			{				/* PRES_ADDR				*/
				4,			/* P-selector length			*/
					"\x00\x00\x00\x01",	/* P-selector				*/
					2,			/* S-selector length			*/
					"\x00\x01",		/* S-selector				*/
					TP_TYPE_TP4,		/* Transport type			*/
					2,			/* T-selector length			*/
					"\x00\x01",		/* T-selector				*/
					4,			/* Network address length		*/
					"\x49\x00\x11\x11"	/* Network address			*/
					}
		}
	};

#endif	/* defined(HARD_CODED_CFG)	*/

	static ST_VOID init_mem (ST_VOID);
	static ST_VOID init_log_cfg (ST_VOID);

	//处理64位下，FILE*到INT的映射

	ST_INT32 g_iFilePtrIdx=0;
	map<ST_INT32,FILE*> g_mapFilePtrToInt;
	ST_INT32 gen_FilePtrIdx(FILE* fp)
	{
		g_iFilePtrIdx++;
		if(g_iFilePtrIdx > 0x7ffffff0)
			g_iFilePtrIdx = 1;
		int idx = g_iFilePtrIdx;
		g_mapFilePtrToInt[idx] = fp;
		return g_iFilePtrIdx;
	}
	FILE* get_FilePtrByIdx(ST_UINT32 idx)
	{
		return g_mapFilePtrToInt[idx];
	}
	void remove_FilePtrByIdx(ST_UINT32 idx)
	{
		g_mapFilePtrToInt.erase(idx);
	}

	/************************************************************************/
	/* Global variables							*/
	/************************************************************************/
	typedef struct
	{
		ST_INT cmd_term_num_va;	/* num of vars received in CommandTermination*/
		ST_CHAR oper_name [MAX_IDENT_LEN +1];
	} MY_CONTROL_INFO;
	/* NOTE: this global variable can be avoided if a (MVL_CONTROL_INFO *)	*/
	/*   is saved in the "user_info" member of MVL_NET_INFO, but to do so,	*/
	/*   you must make sure "user_info" is not used for something else.	*/
	MY_CONTROL_INFO my_control_info;

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
	/*			domvar_type_id_create				*/
	/* Get the type for any domain-specific var and create a type_id.	*/
	/************************************************************************/
	ST_INT domvar_type_id_create (MVL_NET_INFO *clientNetInfo, ST_CHAR *dom_name, ST_CHAR *var_name)
	{
		MVL_REQ_PEND *reqCtrl;

		GETVAR_REQ_INFO getvar_req;
		ST_INT oper_type_id = -1;	/* start with invalid type id	*/
		ST_RET ret;

		/* Get the type of this "Oper" attribute & create type.	*/
		/* Would be more efficient to do this just once before this function.*/
		getvar_req.req_tag = GETVAR_NAME;
		getvar_req.name.object_tag = DOM_SPEC;	/* always DOM_SPEC for 61850 variables*/
		getvar_req.name.obj_name.vmd_spec = var_name;
		getvar_req.name.domain_id= dom_name;

		ret = mvla_getvar (clientNetInfo, &getvar_req, &reqCtrl);
		if (ret == SD_SUCCESS)
			ret = waitReqDone (reqCtrl, timeOut);
		if (ret != SD_SUCCESS)
			printf ("Error getting type of variable '%s' in domain '%s'", var_name, dom_name);
		else
		{
			/* Don't care about name so pass NULL.			*/
			oper_type_id = mvl_type_id_create (NULL, reqCtrl->u.getvar.resp_info->type_spec.data,
				reqCtrl->u.getvar.resp_info->type_spec.len);
		}
		mvl_free_req_ctrl (reqCtrl);	/* Done with request struct	*/
		return (oper_type_id);
	}

	/************************************************************************/
	/*			test_iec_control				*/
	/* Save control info in global variable (my_control_info). It is needed	*/
	/* later when u_mvl_info_rpt_ind is called (i.e. rpt received).		*/
	/* See earlier note about how avoid using global my_control_info.	*/
	/************************************************************************/
	ST_VOID test_iec_control (MVL_NET_INFO *clientNetInfo, RPT_TYPEIDS *rpt_typeids,
		ST_CHAR *dom_name,
		ST_CHAR *sbo_name,
		ST_CHAR *oper_name,
		ST_INT oper_type_id)	/* type_id for "oper_name" arg	*/
	{
		ST_RET ret;
		ST_INT j;
		ST_CHAR oper_ref [66];	/* Reference returned when SBO read	*/
		ST_CHAR *oper_data_buf;	/* "Oper" data. Get size from RUNTIME_TYPE & alloc*/
		RUNTIME_TYPE *rt_first;	/* first type in array		*/
		RUNTIME_TYPE *rt_type;	/* current type being processed	*/
		ST_INT rt_num;		/* num of types in array	*/

		oper_data_buf = NULL;	/* initialize. Should be allocated below.	*/

		/* Read "SBO"	*/
		/* Type should really be vstring64, but using vstring65 is OK.	*/
		ret = named_var_read (clientNetInfo, sbo_name, DOM_SPEC, dom_name,
			rpt_typeids->vstring65, oper_ref, timeOut);
		if (ret)
			printf ("SBO Read ERROR");
		else
			printf ("SBO read SUCCESS = %s\n", oper_ref);

		/* FOR DEMONSTRATION ONLY: find "ctlVal" & "operTm" components	*/
		/* of "Oper" and write dummy data to them.				*/
		ret = mvl_get_runtime (oper_type_id, &rt_first, &rt_num);
		if (ret == SD_SUCCESS)
		{
			ST_INT data_offset = 0;	/* use to compute offset to components	*/
			/* Allocate "Oper" data buffer. Loop through type to init data.*/
			oper_data_buf = (char*)chk_calloc (1, rt_first->offset_to_last);
			for (j=0; j < rt_num; j++)
			{
				rt_type = &rt_first[j];
				if (strcmp (ms_comp_name_find(rt_type), "ctlVal") == 0)
				{
					if (rt_type->el_tag == RT_INTEGER && rt_type->u.p.el_len == 4)
						*(ST_INT32 *)(oper_data_buf+data_offset) = 9;
				}
				else if (strcmp (ms_comp_name_find(rt_type), "operTm") == 0)
				{
					if (rt_type->el_tag == RT_UTC_TIME)
					{
						MMS_UTC_TIME *utc_time;
						utc_time = (MMS_UTC_TIME *)(oper_data_buf+data_offset);
						utc_time->secs = (ST_UINT32) time(NULL);
						utc_time->fraction = 99;
						utc_time->qflags = 0xFf;
					}
				}
				data_offset += rt_type->el_size;
				assert (data_offset <= rt_first->offset_to_last);
			}	/* end "for"	*/
		}
		else
			printf ("Error getting RUNTIME_TYPE info. Dummy values not written to 'Oper'.\n");

		/* NOTE: -1 indicates "CommandTermination" not received yet.		*/
		/*  This must be set before "named_var_write" because			*/
		/*  CommandTermination may be received before it returns.		*/ 

		my_control_info.cmd_term_num_va = -1;
		strcpy (my_control_info.oper_name, oper_name);	/* save oper_name for later*/

		/* NOTE: if this is SBO control and SBO read failed, this write	*/
		/*   to "Oper" should fail, but try it anyway.			*/
		ret = named_var_write (clientNetInfo, oper_name, DOM_SPEC, dom_name,	/* no domain name*/
			oper_type_id, oper_data_buf, timeOut);
		if (ret != SD_SUCCESS)
			printf ("Control Activation Response received: FAILURE\n");
		else
			printf ("Control Activation Response received: SUCCESS\n");

		/* if buffer was allocated, free it.	*/
		if (oper_data_buf)
			chk_free (oper_data_buf);

		/* Wait here for CommandTermination info rpt before returning to main loop.*/
		if (ret == SD_SUCCESS)
		{
			time_t timeout = time(NULL)+5;	/* DEBUG: chg timeout as needed.*/

			while (doIt)
			{
				doCommService ();
				if (time(NULL)>timeout)
				{
					printf ("Timeout waiting for CommandTermination\n");
					break;
				}
				/* .._num_va is set by u_mvl_info_rpt_ind when report received.	*/
				if (my_control_info.cmd_term_num_va>=0)
				{
					/* If 2 vars in rpt, must be error. If 1 var, must be OK.	*/
					printf ("Command Termination received: %s\n", 
						my_control_info.cmd_term_num_va==1?"SUCCESS":"FAILURE"); 
					break;
				}
			}
		}
		return;
	}

	/************************************************************************/
	/*                       main						*/
	/************************************************************************/

	int main2 (int argc, char *argv[])
	{
#define TEST_MODE_SIMPLE	0
#define TEST_MODE_IEC_RPT	1
#define TEST_MODE_UCA_RPT	2
#define TEST_MODE_IEC_CONTROL	3
#define TEST_MODE_IEC_REMOTE	4
		ST_INT test_mode = TEST_MODE_SIMPLE;	/* test mode. default to simple test	*/
		ST_RET ret;
		MVL_CFG_INFO mvlCfg;
		/* NOTE: serverName gets overwritten if USE_DIB_LIST defined.	*/
		ST_CHAR serverName [MAX_IDENT_LEN+1] = "remote1";	/* */
		MVL_NET_INFO *clientNetInfo;
		RPT_TYPEIDS rpt_typeids;
		ST_CHAR *usageString = "usage: %s -m test_mode\n-m test_mode\tTesting mode (iecrpt, ucarpt, ieccontrol, or iecremote)";
		ST_INT j;
#if defined(MVL_GOOSE_SUPPORT)
		IEC_GOOSE_RX_USER_INFO *goosehandle = NULL;
		ST_DOUBLE TimeOld;
		ST_DOUBLE TimeNew;
		ST_LONG   TimeElapsed;
#endif
		ST_INT oper_type_id;
		ALL_RCB_INFO *all_rcb_info;	/* used to track all RCBs on one conn	*/
		RCB_INFO *rcb_info;
		MVL_VMD_CTRL *remote_vmd_ctrl;
		SCL_INFO remote_scl_info;	/* filled in by config_iec_remote_vmd	*/

		/* Process command line arguments.	*/
		for(j=1;j<argc;  )
		{
			if (strcmp (argv[j], "-m") == 0)
			{
				if (j + 2 > argc)
				{
					printf ("Not enough arguments\n"); 
					printf (usageString, argv[0]);
					exit (1);
				}
				if (strcmp (argv[j+1], "iecrpt") == 0)
					test_mode = TEST_MODE_IEC_RPT;
				else if (strcmp (argv[j+1], "ucarpt") == 0)
					test_mode = TEST_MODE_UCA_RPT;
				else if (strcmp (argv[j+1], "ieccontrol") == 0)
					test_mode = TEST_MODE_IEC_CONTROL;
				else if (strcmp (argv[j+1], "iecremote") == 0)
					test_mode = TEST_MODE_IEC_REMOTE;
				else
				{
					printf ("Unrecognized value for '-m' option\n"); 
					printf (usageString, argv[0]);
					exit (1);
				}
				j += 2;
			}
		}	/* end loop checking command line args	*/

		setbuf (stdout, NULL);    /* do not buffer the output to stdout   */
		setbuf (stderr, NULL);    /* do not buffer the output to stderr   */

		printf ("%s Version %s\n", MMSLITE_NAME, MMSLITE_VERSION);
#if defined(S_SEC_ENABLED)
		printf ("%s Version %s\n", S_SEC_LITESECURE_NAME, S_SEC_LITESECURE_VERSION_STR);
#endif
		puts (MMSLITE_COPYRIGHT);


#ifdef S_MT_SUPPORT
		/* init glbsem explicitly to avoid auto-initialization calls from multiple threads */
		if ((ret = gs_init()) != SD_SUCCESS)
		{
			printf ("gs_init() failed");
			exit (1);
		}
#endif

		/* init stime explicitly to avoid auto-initialization calls from multiple threads */
		if ((ret = stimeInit()) != SD_SUCCESS)
		{
			printf ("stimeInit() failed");
			exit (1);
		}

#if defined(NO_GLB_VAR_INIT)
		mvl_init_glb_vars ();
#endif

		init_mem ();		/* Set up memory allocation tools 		*/

		init_log_cfg ();

		SLOGALWAYS2 ("%s Version %s", MMSLITE_NAME, MMSLITE_VERSION);

		/* We want to know about connection activity				*/
		u_mvl_disc_ind_fun     = disc_ind_fun;

#if defined(HARD_CODED_CFG)
		num_loc_dib_entries = sizeof(localDibTable)/sizeof(DIB_ENTRY);
		loc_dib_table = localDibTable;
		num_rem_dib_entries = sizeof(remoteDibTable)/sizeof(DIB_ENTRY);
		rem_dib_table = remoteDibTable;
#if 0	/* Only call config functions necessary for stack being used.	*/
		tp4_config();		/* see tp4_hc.c		*/
		clnp_config();	/* see clnp_hc.c	*/
		adlcConfigure();	/* see adlc_hc.c	*/
#endif
		/* Fill in mvlCfg.	*/
		mvlCfg.num_calling = 4;
		mvlCfg.num_called = 0;
		mvlCfg.max_msg_size = 8000;
		strcpy (mvlCfg.local_ar_name, "local1");

#else /* #if defined(HARD_CODED_CFG) */

		/* Read the configuration from a file */
		ret = osicfgx ("osicfg.xml", &mvlCfg);	/* This fills in mvlCfg	*/
		if (ret != SD_SUCCESS)
		{
			printf ("\n Stack configuration failed, err = 0x%X. Check configuration.\n", ret );
			exit (1);
		}
#endif /* HARD_CODED_CFG */

		/* Set the ^c catcher */
		signal (SIGINT, ctrlCfun);

		mvl_max_dyn.types = 1000;	/* CRITICAL: must be before mvl_init_mms_objs*/
		/* (called by mvl_start_acse)		*/
#if defined(GOOSE_RX_SUPP) || defined(GSSE_RX_SUPP) || defined(GSE_MGMT_RX_SUPP) || defined(SMPVAL_RX_SUPP)
		/* NOTE: clnp_init required for GSSE. It also calls clnp_snet_init	*/
		/*       which is required for GOOSE, GSE_MGMT, SMPVAL.		*/
		ret = clnp_init (1200);	/* big enough for any GSSE msg	*/
		if (ret)
		{
			printf ("clnp_init error 0x%X", ret);
			exit (49);
		}
#endif

		ret = mvl_start_acse (&mvlCfg);	/* MAKE SURE mvlCfg is filled in*/
		if (ret != SD_SUCCESS)
		{
			printf ("\n mvl_start_acse () failed, err = 0x%X.\n", ret );
			exit (1);
		}

		if ((ret = rpt_typeids_find (&rpt_typeids)) != SD_SUCCESS)
		{
			printf ("rpt_typeids_find () failed = 0x%X.\n", ret );
			exit (1);
		}

#if defined(MVL_GOOSE_SUPPORT)
		/* NOTE: this GOOSE initialization must be AFTER mvl_start_acse so	*/
		/*   that required types can be found.				*/
		goosehandle = goose_init ();
#endif

#if defined(GOOSE_RX_SUPP) || defined(GSSE_RX_SUPP) || defined(GSE_MGMT_RX_SUPP) || defined(SMPVAL_RX_SUPP)

		/* CRITICAL: to receive Multicast packets (for GOOSE, SMPVAL, etc.),	*/
		/* must set multicast filter using clnp_snet_set_multicast_filter	*/
		/* or clnp_snet_rx_all_multicast_start (accept ALL multicast packets).*/
		/* NOTE: this is AFTER goose_init to override filter set by goose_init.*/

		if ((ret = clnp_snet_rx_all_multicast_start()) != SD_SUCCESS)
		{
			SLOGALWAYS1 ("Error 0x%04X setting multicast filter.", ret);
			printf      ("Error 0x%04X setting multicast filter.\n", ret);
		}
#endif

#if !defined(NO_KEYBOARD) && !defined(_WIN32) && !defined(__QNX__)
		term_init ();	/* makes getchar work right	*/
#endif

#if defined(USE_DIB_LIST)
		/* First copy addresses from "osicfg.xml" (stored in "loc_dib_table"	*/
		/* and "rem_dib_table" arrays) to the new DIB list.			*/
		/* NOTE: Client address is probably not in the SCL file, so it should	*/
		/*       come from here.						*/
		for (j = 0; j < num_loc_dib_entries; j++)
		{
			dib_entry_create (&loc_dib_table[j]);	/* add to new DIB list	*/
		}
		for (j = 0; j < num_rem_dib_entries; j++)
		{
			dib_entry_create (&rem_dib_table[j]);	/* add to new DIB list	*/
		}
#endif	/* USE_DIB_LIST	*/

		/* For this test_mode, process SCL before connecting to server.	*/
		if (test_mode == TEST_MODE_IEC_REMOTE)
		{
			/* Use SCL to configure a remote VMD (used later by			*/
			/* "test_iec_remote_vmd" after a connection is established).	*/
			/* NOTE: could configure several VMDs like this using different SCL files.*/
			remote_vmd_ctrl = config_iec_remote_vmd (
				"../scl_srvr/sisco_sample.cid",	/* matches config of "scl_srvr" sample	*/
				"E1Q1SB1",
				"S1",
				&remote_scl_info);
			log_iec_remote_vmd_var_names (remote_vmd_ctrl);

#ifdef USE_DIB_LIST	/* only works with new dib	*/
			/* Add addresses from SCL file to new global linked list	*/
			scl_dib_entry_save (&remote_scl_info);
			/* Construct remote AR Name from IED/AccessPoint above.	*/
			/* DEBUG: It would be convenient to make this configurable.	*/
			printf ("\n\nNOTICE: USE_DIB_LIST ENABLED: Connecting to Remote AR Name 'E1Q1SB1/S1'."
				"\n        Address comes from <ConnectedAP iedName='E1Q1SB1' apName='S1'>"
				"\n        element of SCL file.\n");
			strcpy (serverName, "E1Q1SB1/S1");
			if (find_dib_entry (serverName) == NULL)
			{
				printf ("ERROR: Remote AR Name '%s' not found in DIB. Exiting now.", serverName);
				exit (100);
			}
#endif	/* USE_DIB_LIST	*/
			scl_info_destroy (&remote_scl_info);	/* Don't need SCL info anymore	*/
		}

		/* Try more than once to connect to Server.	*/
		for (j = 0; j < 5; j++)
		{
			printf ("\n Connecting to Server '%s'...", serverName);
			clientNetInfo = connectToServer (mvlCfg.local_ar_name, serverName);
			if (clientNetInfo == NULL)
			{
				printf ("  failed.");
				if (j < 4)
				{	/* Not last loop. try again.	*/
					printf ("  Retry in 5 seconds.");
					sMsSleep (5000);	/* wait 5 seconds & try again	*/
				}
			}
			else
				break;	/* connected. Stop looping.	*/
		}
		if (clientNetInfo != NULL)
		{
			printf ("Connected\n");

			/* Alloc, init "all_rcb_info" & save it in "user_info" member of MVL_NET_INFO
			* (needed for TEST_MODE_IEC_RPT, TEST_MODE_UCA_RPT; ignored for other tests).
			* When a disconnect occurs, RCB resources must be freed (see "disc_ind_fun").
			*/ 
			all_rcb_info = (ALL_RCB_INFO *)chk_calloc (1, sizeof (ALL_RCB_INFO));
			all_rcb_info->rpt_typeids = &rpt_typeids;
			assert (clientNetInfo->user_info==NULL);	/* make sure user_info not already used*/
			clientNetInfo->user_info = all_rcb_info;	/* save ptr in conn struct	*/

			/* Execute Client Services ... */
			switch (test_mode)
			{
			case TEST_MODE_IEC_RPT:
				/* NOTE: Could use mvla_getnam to get domain & RCB name from server.*/
				rcb_info = test_iec_rpt (clientNetInfo, &rpt_typeids, "E1Q1SB1C1", "LLN0$BR$PosReport01");

				/* Create 2nd RCB to test multiples.	*/
				rcb_info = test_iec_rpt (clientNetInfo, &rpt_typeids, "E1Q1SB1C1", "LLN0$RP$MeaReport01");
				break;
			case TEST_MODE_UCA_RPT:
				/* NOTE: "test_iec_rpt" can handle UCA rpts too.	*/
				rcb_info = test_iec_rpt (clientNetInfo, &rpt_typeids, "pbroDev", "GLOBE$RP$brcbST");
				break;
			case TEST_MODE_IEC_CONTROL:
				/* NOTE: Could use mvla_getnam to get these names from the server.*/
				oper_type_id = domvar_type_id_create (clientNetInfo, "E1Q1SB1C1", "CSWI1$CO$Mod$Oper");
				printf ("\nCONTROL TEST #1\n");
				test_iec_control (clientNetInfo, &rpt_typeids,
					"E1Q1SB1C1", "CSWI1$CO$Mod$SBO", "CSWI1$CO$Mod$Oper", oper_type_id);
				printf ("\nCONTROL TEST #2\n");
				test_iec_control (clientNetInfo, &rpt_typeids,
					"E1Q1SB1C1", "CSWI1$CO$Mod$SBO", "CSWI1$CO$Mod$Oper", oper_type_id);
				printf ("\nCONTROL TEST #3\n");
				test_iec_control (clientNetInfo, &rpt_typeids,
					"E1Q1SB1C1", "CSWI1$CO$Mod$SBO", "CSWI1$CO$Mod$Oper", oper_type_id);
				printf ("\nCONTROL TEST #4\n");
				test_iec_control (clientNetInfo, &rpt_typeids,
					"E1Q1SB1C1", "CSWI1$CO$Mod$SBO", "CSWI1$CO$Mod$Oper", oper_type_id);
				mvl_type_id_destroy (oper_type_id);
				break;
			case TEST_MODE_IEC_REMOTE:
				printf ("\nREMOTE VMD TEST\n");
				test_iec_remote_vmd (clientNetInfo, remote_vmd_ctrl);
				break;

			default:
				test_simple_requests (clientNetInfo);
				break;
			}
		}

		printf ("\n Entering 'server' mode, hit ^c or 'x' to exit ... ");
#if defined(MVL_GOOSE_SUPPORT)
		TimeOld = sGetMsTime ();	/* init old time	*/
#endif

		while (doIt)
		{
			doCommService ();
			kbService ();
#if defined(S_SEC_ENABLED)
			/* check for security configuration updates */
			if (ulCheckSecurityConfiguration () != SD_SUCCESS)
				printf("\n Security Reconfiguration failed\n");
#endif /* defined(S_SEC_ENABLED) */
#if defined(MVL_GOOSE_SUPPORT)
			/* Check for GOOSE timeouts on any subscriptions.	*/
			TimeNew = sGetMsTime ();				/* get current time*/
			TimeElapsed = (ST_LONG) (TimeNew - TimeOld);	/* calc elapsed time*/
			TimeOld = TimeNew;					/* update old time*/
			chk_iec_goose_timeout (TimeElapsed);	/* look for expired TAL	*/
#endif
		}

		doIt = SD_TRUE;	/* Turn it on again so disconnect can succeed	*/

		/* If connect succeeded, do cleanup.	*/
		if (clientNetInfo)
		{
			if ((all_rcb_info = (ALL_RCB_INFO *)clientNetInfo->user_info) != NULL)
			{
				/* Free up rcb_info if necessary.	*/
				while ((rcb_info = (RCB_INFO *)list_get_first (&all_rcb_info->rcb_info_list)) != NULL)
					rcb_info_destroy (rcb_info);
				chk_free (all_rcb_info);
			}
			clientNetInfo->user_info=NULL;

			/* Issue Conclude */
			disconnectFromServer (clientNetInfo);
		}

		/* Done with VMDs. Destroy them.	*/
		if (test_mode == TEST_MODE_IEC_REMOTE && remote_vmd_ctrl != NULL)
		{
			mvl_vmd_destroy (remote_vmd_ctrl);	/* only created for this mode*/
		}
		mvl_vmd_destroy (&mvl_vmd);

		/* If cleanup worked, number of types logged here should be the number*/
		/* of types created by Foundry (non-dynamic).				*/
		SLOGALWAYS1 ("DEBUG: Number of types in use after cleanup=%d", mvl_type_count ());

		dyn_mem_ptr_statistics (0);	
#if defined(GOOSE_RX_SUPP) || defined(GSSE_RX_SUPP) || defined(GSE_MGMT_RX_SUPP) || defined(SMPVAL_RX_SUPP)
		/* clnp_init called at startup, so must call clnp_end to clean up.*/
		clnp_end ();
#endif
		mvl_end_acse ();

		/* Free buffers allocated by "mvl_init_mms_objs".	*/
		mvl_free_mms_objs ();
		/* Free buffers allocated by "osicfgx".	*/
		osicfgx_unconfigure ();

#if defined(USE_DIB_LIST)
		/* Destroy addresses created at startup.	*/
		dib_entry_destroy_all ();
#endif

		/* This should log only a few buffers which are freed by slog_end and	*/
		/* logcfgx_unconfigure below.						*/
		dyn_mem_ptr_status ();	/* is everything freed up?	*/

#if defined(DEBUG_SISCO)
		/* terminate logging services and save current log file position */
		slog_end (sLogCtrl);

		/* Free buffers allocated by "logcfgx".				*/
		/* NOTE: this is AFTER slog_end, so it can't screw up logging.	*/
		logcfgx_unconfigure (sLogCtrl);
#endif

		printf ("\n\n");
#if !defined(NO_KEYBOARD) && !defined(_WIN32) && !defined(__QNX__)
		term_rest ();	/* Must be called before exit if term_init used.	*/
#endif
#if defined(MVL_GOOSE_SUPPORT)
		/* Destroy all subscribers.	*/
		if (goosehandle!=NULL && iecGooseUnSubscribe (goosehandle) !=SD_SUCCESS)
			printf("\niecGooseUnsubscribe failed");
#endif

		/* Should have 0 buffers allocated now.	*/
		if (chk_curr_mem_count != 0)
			printf ("Note: %lu buffers not freed.\n",
			(ST_ULONG) chk_curr_mem_count);

		return (0);
	}

	/************************************************************************/
	/* Confirm function for mvla_fget request.				*/
	/************************************************************************/
	ST_VOID fget_cnf (MVL_FGET_REQ_INFO *fget_req)
	{
		printf ("\n mvla_fget completed: result = 0x%X.\n", fget_req->fget_error);
		chk_free (fget_req);
	}

	/************************************************************************/
	/*			 test_simple_requests				*/
	/* This function sends simple requests to the server. It is designed	*/
	/* to communicate with the sample "server" program. It is NOT for use	*/
	/* with an IEC or UCA server.						*/
	/************************************************************************/
	ST_VOID test_simple_requests (MVL_NET_INFO *clientNetInfo)
	{
		ST_INT16 remTemperature;
		ST_UINT16 write_val;
		ST_RET ret;
		ST_INT i, j;
		IDENT_RESP_INFO *ident_resp;
		MVL_FDIR_RESP_INFO *fdir_resp;
		MVL_REQ_PEND *reqCtrl;

		VARIABLE_LIST *variable_list;
		DEFVLIST_REQ_INFO *defvlist_req;

		GETVLIST_REQ_INFO getvlist_req;
		GETVLIST_RESP_INFO *getvlist_resp;	/* set to reqCtrl->u.getvlist_resp_info*/

		DELVLIST_REQ_INFO delvlist_req;
		JINIT_REQ_INFO jinit_req;
		JSTAT_REQ_INFO jstat_req;
		JREAD_REQ_INFO jread_req;
		MVL_JREAD_RESP_INFO *jread_resp;	/* set to reqCtrl->u.jread_resp_info*/
		MVL_JOURNAL_ENTRY *jou_entry;
		ST_CHAR *src_file_name;
		ST_CHAR *dest_file_name;
		ST_CHAR *file_to_delete;
		ST_CHAR *file_to_rename;
		ST_CHAR *new_file_name;
		GETVAR_REQ_INFO getvar_req;
		GETDOM_REQ_INFO getdom_req;
		NAMELIST_REQ_INFO getnam_req;
		NAMELIST_RESP_INFO *getnam_resp;
		ST_CHAR **nptr;
		STATUS_REQ_INFO status_req;
#if (UNICODE_LOCAL_FORMAT==UNICODE_UTF16)
		ST_INT utf8_type_id;
		ST_UINT16 remUTF8string [13+1];
#endif
		MVL_FGET_REQ_INFO *fget_req_info;	/* allocate this	*/
		ST_CHAR *localFile, *remoteFile;

		/* send "Identify" request and wait for reply */
		ret = mvla_identify (clientNetInfo, &reqCtrl);
		if (ret == SD_SUCCESS)
			ret = waitReqDone (reqCtrl, timeOut);
		if (ret == SD_SUCCESS)
		{
			ident_resp = reqCtrl->u.ident.resp_info;
			printf ("\n Vendor = %s, Model = %s, Rev = %s", ident_resp->vend,
				ident_resp->model, ident_resp->rev);
		}
		else
			printf ("\n mvl_identify () Error, ret = 0x%X.", ret);
		mvl_free_req_ctrl (reqCtrl);

#if (MMS_STATUS_EN & REQ_EN)
		status_req.extended = SD_FALSE;
		ret = mvla_status (clientNetInfo, &status_req, &reqCtrl);
		if (ret == SD_SUCCESS)
			ret = waitReqDone (reqCtrl, timeOut);
		if (ret != SD_SUCCESS)
			printf ("\n mvl_status () Error, ret = 0x%x.", ret);
		else
		{
			printf ("\n mvl_status OK");
			printf ("\n   logical status  = %d", reqCtrl->u.status.resp_info->logical_stat);
			printf ("\n   physical status = %d", reqCtrl->u.status.resp_info->physical_stat);
			if (reqCtrl->u.status.resp_info->local_detail_pres == SD_FALSE)
				printf ("\n   No local detail");
			else
			{
				printf ("\n   Local detail present");
				printf ("\n     Local detail len = %d bits", 
					reqCtrl->u.status.resp_info->local_detail_len);
			}
		}
		mvl_free_req_ctrl (reqCtrl);
#endif


#if (MMS_GETVAR_EN & REQ_EN)
		getvar_req.req_tag = GETVAR_NAME;
		getvar_req.name.object_tag = VMD_SPEC;
		getvar_req.name.obj_name.vmd_spec = "Temperature";

		ret = mvla_getvar (clientNetInfo, &getvar_req, &reqCtrl);
		if (ret == SD_SUCCESS)
			ret = waitReqDone (reqCtrl, timeOut);
		if (ret != SD_SUCCESS)
			printf ("\n mvl_getvar () Error, ret = 0x%x.", ret);
		else
		{
			printf ("\n mvl_getvar OK");
			printf ("\n   len = %d", reqCtrl->u.getvar.resp_info->type_spec.len);
		}
		mvl_free_req_ctrl (reqCtrl);
#endif

#if (MMS_GET_DOM_EN & REQ_EN)
		strcpy (getdom_req.dname, "mvlLiteDom");
		ret = mvla_getdom (clientNetInfo, &getdom_req, &reqCtrl);
		if (ret == SD_SUCCESS)
			ret = waitReqDone (reqCtrl, timeOut);
		if (ret != SD_SUCCESS)
			printf ("\n mvl_getdom () Error, ret = 0x%x.", ret);
		else
		{
			printf ("\n mvl_getdom OK");
			printf ("\n   num_of_capab = %d", reqCtrl->u.getdom.resp_info->num_of_capab);
			if (reqCtrl->u.getdom.resp_info->mms_deletable == SD_FALSE)
				printf ("\n   MMS Deletable : NO");
			else
				printf ("\n   MMS Deletable : YES");
			if (reqCtrl->u.getdom.resp_info->sharable == SD_FALSE)
				printf ("\n   Sharable : NO");
			else
				printf ("\n   Sharable : YES");
			printf ("\n   num_of_pinames = %d", reqCtrl->u.getdom.resp_info->num_of_pinames);
			printf ("\n   State = %d", reqCtrl->u.getdom.resp_info->state);
			if (reqCtrl->u.getdom.resp_info->upload_in_progress == SD_FALSE)
				printf ("\n   Upload in Progress : NO");
			else
				printf ("\n   Upload in Progress : YES");
		}
		mvl_free_req_ctrl (reqCtrl);
#endif

#if (MMS_GETNAMES_EN & REQ_EN)
		getnam_req.cs_objclass_pres = SD_FALSE;
		getnam_req.obj.mms_class = 0;
		getnam_req.objscope = VMD_SPEC;
		getnam_req.cont_after_pres = SD_FALSE;

		ret = mvla_getnam (clientNetInfo, &getnam_req, &reqCtrl);
		if (ret == SD_SUCCESS)
			ret = waitReqDone (reqCtrl, timeOut);
		if (ret != SD_SUCCESS)
			printf ("\n mvl_getnam () Error, ret = 0x%x.", ret);
		else
		{
			printf ("\n mvl_getnam OK");
			getnam_resp = (NAMELIST_RESP_INFO *) reqCtrl->u.ident.resp_info;
			if (getnam_resp->more_follows)
				printf ("\n   More Follows : TRUE");
			else
				printf ("\n   More Follows : FALSE");
			printf ("\n   %d Names returned : ",getnam_resp->num_names);
			printf ("\n   Name List :");

			nptr = (ST_CHAR **) (getnam_resp + 1);

			for (i = 0; i < getnam_resp->num_names; ++i)
				printf ("\n     %s ",nptr[i]);
		}
		mvl_free_req_ctrl (reqCtrl);
#endif


		for (i = 0; i < 2; ++i)
		{
			ret = named_var_read (clientNetInfo, "Temperature", VMD_SPEC, NULL,	/* no domain name*/
				I16_TYPEID, &remTemperature, timeOut);
			if (ret == SD_SUCCESS)
				printf ("\nRemote Temperature: %d, count %d", 
				(int) remTemperature, i+1);
			else
				printf ("\n Read Error");
		}

#if (UNICODE_LOCAL_FORMAT==UNICODE_UTF16)
		utf8_type_id = mvl_typename_to_typeid ("UTF8VSTRING13");
		ret = named_var_read (clientNetInfo, "UTF8Vstring13test", VMD_SPEC, NULL,	/* no domain name*/
			utf8_type_id, &remUTF8string, timeOut); 
		if (ret == SD_SUCCESS)
		{
			printf ("\nRemote UTF8string Read SUCCESS");
			SLOGALWAYS0 ("Remote UTF8string Read SUCCESS. Converted to UTF16");
			SLOGALWAYSH (sizeof(remUTF8string), remUTF8string);    
		}
		else
			printf ("\nRemote UTF8string Read FAILED");
#endif

		write_val = 9;
		ret = named_var_write (clientNetInfo, "Temperature", VMD_SPEC, NULL,	/* no domain name*/
			I16_TYPEID, &write_val, timeOut);
		if (ret != SD_SUCCESS)
			printf ("\n Write Error");
		else
			printf ("\n Write OK");

#if (MMS_FRENAME_EN & REQ_EN)
		file_to_rename = "datafile.dat";
		new_file_name = "newfile.dat";
		ret = mvla_frename (clientNetInfo, file_to_rename, new_file_name, &reqCtrl);
		if (ret == SD_SUCCESS)
			ret = waitReqDone (reqCtrl, timeOut);
		if (ret != SD_SUCCESS)
			printf ("\n mvl_frename () Error renaming file '%s', ret = 0x%X.",
			file_to_rename, ret);
		else
			printf ("\n mvl_frename () of '%s' OK.", file_to_rename);
		mvl_free_req_ctrl (reqCtrl);
#endif

#if (MMS_OBTAINFILE_EN & REQ_EN)
		src_file_name = "client.c";
		dest_file_name = "temp.c";
		ret = mvla_obtfile (clientNetInfo, src_file_name, dest_file_name, &reqCtrl);
		if (ret == SD_SUCCESS)
			ret = waitReqDone (reqCtrl, timeOut);
		if (ret != SD_SUCCESS)
			printf ("\n mvl_obtfile () Error obtaining file '%s', ret = 0x%X.",
			src_file_name, ret);
		else
			printf ("\n mvl_obtfile () of '%s' OK.", src_file_name);
		mvl_free_req_ctrl (reqCtrl);
#endif

#if (MMS_FDELETE_EN & REQ_EN)
		file_to_delete = "junk.jnk";
		ret = mvla_fdelete (clientNetInfo, file_to_delete, &reqCtrl);
		if (ret == SD_SUCCESS)
			ret = waitReqDone (reqCtrl, timeOut);
		if (ret != SD_SUCCESS)
			printf ("\n mvl_fdelete () Error deleting file '%s', ret = 0x%X.",
			file_to_delete, ret);
		else
			printf ("\n mvl_fdelete () of '%s' OK.", file_to_delete);
		mvl_free_req_ctrl (reqCtrl);
#endif

#if (MMS_FOPEN_EN & REQ_EN) && (MMS_FREAD_EN & REQ_EN) && (MMS_FCLOSE_EN & REQ_EN)
		remoteFile = "osicfg.xml";
		localFile = "junk.jnk";
		ret = getFile (clientNetInfo, localFile, remoteFile);
		if (ret != SD_SUCCESS)
			printf ("\n getFile () Error getting remote file '%s', ret = 0x%X.",
			remoteFile, ret);
		else
			printf ("\n getFile () of remote file '%s' OK", remoteFile);

		/* Use mvla_fget to transfer the same file.				*/
		/* NOTE: File transfer is not complete until fget_cnf is called.	*/
		/* CRITICAL: allocate fget_req_info (used later by callback functions).*/
		fget_req_info = (MVL_FGET_REQ_INFO*)chk_calloc (1, sizeof (MVL_FGET_REQ_INFO));
		fget_req_info->fget_cnf_ptr = fget_cnf;	/* CRITICAL: must set this ptr*/
		ret = mvla_fget (clientNetInfo, remoteFile, localFile, fget_req_info);
		if (ret != SD_SUCCESS)
			printf ("\n mvla_fget () Error getting remote file '%s', ret = 0x%X.",
			remoteFile, ret);
		else
			printf ("\n mvla_fget () of remote file '%s': request sent OK. WAIT for completion result.", remoteFile);
#endif

#if (MMS_FDIR_EN & REQ_EN)
		ret = mvla_fdir (clientNetInfo, 
			"*.*",	/* fs_name	*/
			NULL,	/* ca_name	*/
			&reqCtrl);
		if (ret == SD_SUCCESS)
			ret = waitReqDone (reqCtrl, timeOut);
		if (ret != SD_SUCCESS)
			printf ("\n mvl_fdir () Error, ret = 0x%X.", ret);
		else
		{
			fdir_resp = reqCtrl->u.fdir.resp_info;
			printf ("\n mvl_fdir results:");
			for (i = 0; i < fdir_resp->num_dir_ent;  i++)
				printf ("\n   File #%d: %s", i, fdir_resp->dir_ent[i].filename);
		}
		mvl_free_req_ctrl (reqCtrl);	/* CRITICAL:		*/
#endif

#if (MMS_DEFVLIST_EN & REQ_EN)
		defvlist_req = 
			(DEFVLIST_REQ_INFO *) chk_calloc (1, sizeof (DEFVLIST_REQ_INFO) + 
			2 * sizeof (VARIABLE_LIST));
		defvlist_req->vl_name.object_tag = VMD_SPEC;
		defvlist_req->vl_name.obj_name.vmd_spec = "NewVlist";
		defvlist_req->num_of_variables = 2;
		variable_list = (VARIABLE_LIST *) (defvlist_req + 1);

		/* Just define 2 copies of Temperature (almost all SISCO apps have it)*/
		variable_list->var_spec.vs.name.object_tag = VMD_SPEC;
		variable_list->var_spec.vs.name.obj_name.vmd_spec = "Temperature";
		variable_list++;
		variable_list->var_spec.vs.name.object_tag = VMD_SPEC;
		variable_list->var_spec.vs.name.obj_name.vmd_spec = "Temperature";
		variable_list++;

		ret = mvla_defvlist (clientNetInfo, defvlist_req, &reqCtrl);
		if (ret == SD_SUCCESS)
			ret = waitReqDone (reqCtrl, timeOut);
		if (ret != SD_SUCCESS)
			printf ("\n mvl_defvlist () Error, ret = 0x%X.", ret);
		else
			printf ("\n mvl_defvlist OK");
		mvl_free_req_ctrl (reqCtrl);
		chk_free (defvlist_req);
#endif

#if (MMS_GETVLIST_EN & REQ_EN)
		getvlist_req.vl_name.object_tag = VMD_SPEC;
		getvlist_req.vl_name.obj_name.vmd_spec = "NewVlist";

		ret = mvla_getvlist (clientNetInfo, &getvlist_req, &reqCtrl);
		if (ret == SD_SUCCESS)
			ret = waitReqDone (reqCtrl, timeOut);
		if (ret != SD_SUCCESS)
			printf ("\n mvl_getvlist () Error, ret = 0x%X.", ret);
		else
		{
			getvlist_resp = reqCtrl->u.getvlist.resp_info;
			printf ("\n mvl_getvlist results:");
			if (getvlist_resp->mms_deletable)
				printf ("    Deletable");
			else
				printf ("    NOT Deletable");
			variable_list = (VARIABLE_LIST *) (getvlist_resp + 1);
			for (i = 0; i < getvlist_resp->num_of_variables;  i++, variable_list++)
				printf ("\n   Var #%d: %s", i, variable_list->var_spec.vs.name.obj_name.vmd_spec);
		}
		mvl_free_req_ctrl (reqCtrl);	/* CRITICAL:		*/
#endif

#if (MMS_DELVLIST_EN & REQ_EN)
		delvlist_req.scope = DELVL_VMD;
		delvlist_req.dname_pres = SD_FALSE;
		delvlist_req.vnames_pres = SD_FALSE;

		ret = mvla_delvlist (clientNetInfo, &delvlist_req, &reqCtrl);
		if (ret == SD_SUCCESS)
			ret = waitReqDone (reqCtrl, timeOut);
		if (ret != SD_SUCCESS)
			printf ("\n mvl_delvlist () Error, ret = 0x%X.", ret);
		else
		{
			printf ("\n mvl_delvlist OK");
			printf ("\n   num_matched = %lu", (ST_ULONG) reqCtrl->u.delvlist.resp_info->num_matched);
			printf ("\n   num_deleted = %lu", (ST_ULONG) reqCtrl->u.delvlist.resp_info->num_deleted);
		}
		mvl_free_req_ctrl (reqCtrl);
#endif

#if (MMS_JINIT_EN & REQ_EN)
		printf ("\n\n Testing VMD_SPEC JournalInit");
		jinit_req.jou_name.object_tag = VMD_SPEC;
		jinit_req.jou_name.obj_name.vmd_spec = "SampleJournal";
		jinit_req.limit_spec_pres = 0;
		jinit_req.limit_entry_pres = 0;
		ret = mvla_jinit (clientNetInfo, &jinit_req, &reqCtrl);
		if (ret == SD_SUCCESS)
			ret = waitReqDone (reqCtrl, timeOut);
		if (ret != SD_SUCCESS)
			printf ("\n mvl_jinit () Error, ret = 0x%X.", ret);
		else
		{
			printf ("\n mvl_jinit OK");
			printf ("\n   del_entries = %lu", (ST_ULONG) reqCtrl->u.jinit.resp_info->del_entries);
		}
		mvl_free_req_ctrl (reqCtrl);
#endif

#if (MMS_JSTAT_EN & REQ_EN)
		printf ("\n\n Testing VMD_SPEC JournalStat");
		jstat_req.jou_name.object_tag = VMD_SPEC;
		jstat_req.jou_name.obj_name.vmd_spec = "SampleJournal";
		ret = mvla_jstat (clientNetInfo, &jstat_req, &reqCtrl);
		if (ret == SD_SUCCESS)
			ret = waitReqDone (reqCtrl, timeOut);
		if (ret != SD_SUCCESS)
			printf ("\n mvl_jstat () Error, ret = 0x%X.", ret);
		else
		{
			printf ("\n mvl_jstat OK");
			printf ("\n   cur_entries = %lu", (ST_ULONG) reqCtrl->u.jstat.resp_info->cur_entries);
			printf ("\n   mms_deletable = %d", reqCtrl->u.jstat.resp_info->mms_deletable);
		}
		mvl_free_req_ctrl (reqCtrl);
#endif

#if (MMS_JREAD_EN & REQ_EN)
		printf ("\n\n Testing VMD_SPEC JournalRead");
		jread_req.jou_name.object_tag = VMD_SPEC;
		jread_req.jou_name.obj_name.vmd_spec = "SampleJournal";
		jread_req.range_start_pres = 0;
		jread_req.range_stop_pres = 0;
		jread_req.list_of_var_pres = 0;
		jread_req.sa_entry_pres = 0;
		ret = mvla_jread (clientNetInfo, &jread_req, &reqCtrl);
		if (ret == SD_SUCCESS)
			ret = waitReqDone (reqCtrl, timeOut);
		if (ret != SD_SUCCESS)
			printf ("\n mvl_jread () Error, ret = 0x%X.", ret);
		else
		{
			printf ("\n mvl_jread OK");
			jread_resp = reqCtrl->u.jread.resp_info;
			printf ("\n   num_entry = %d", jread_resp->num_of_jou_entry);
			printf ("\n   more_follows = %d", jread_resp->more_follows);
			for (i = 0;  i < jread_resp->num_of_jou_entry;  i++)
			{
				jou_entry = &jread_resp->jou_entry[i];
				printf ("\n   Journal Entry # %d:", i);
				printf ("\n     entry_id_len = %d", jou_entry->entry_id_len);
				printf ("\n     occur_time.form = %s", (jou_entry->occur_time.form == MMS_BTOD6 ? "MMS_BTOD6" : "MMS_BTOD4"));
				printf ("\n     occur_time.ms  = %ld", (ST_LONG) jou_entry->occur_time.ms);
				printf ("\n     occur_time.day = %ld", (ST_LONG) jou_entry->occur_time.day);
				printf ("\n     entry_form_tag = %d", jou_entry->entry_form_tag);
				if (jou_entry->entry_form_tag == 2)
				{
					if (jou_entry->ef.data.list_of_var_pres)
					{
						for (j = 0;  j < jou_entry->ef.data.num_of_var;  j++)
						{
							printf ("\n       Var # %d: var_tag = %s", j,
								jou_entry->ef.data.list_of_var[j].var_tag);
							printf ("\n       Var # %d: value_spec.len = %d", j,
								jou_entry->ef.data.list_of_var[j].value_spec.len);
						}
					}
				}
				else
				{
					printf ("\n       annotation = %s",
						jou_entry->ef.annotation);
				}
			}	/* end "loop"	*/
		}
		mvl_free_req_ctrl (reqCtrl);	/* CRITICAL:		*/
#endif


#if (MMS_JINIT_EN & REQ_EN)
		printf ("\n\n Testing DOM_SPEC JournalInit");
		jinit_req.jou_name.object_tag = DOM_SPEC;
		jinit_req.jou_name.domain_id = "mvlLiteDom1";
		jinit_req.jou_name.obj_name.vmd_spec = "MyJournal";
		jinit_req.limit_spec_pres = 0;
		jinit_req.limit_entry_pres = 0;
		ret = mvla_jinit (clientNetInfo, &jinit_req, &reqCtrl);
		if (ret == SD_SUCCESS)
			ret = waitReqDone (reqCtrl, timeOut);
		if (ret != SD_SUCCESS)
			printf ("\n mvl_jinit () Error, ret = 0x%X.", ret);
		else
		{
			printf ("\n mvl_jinit OK");
			printf ("\n   del_entries = %lu", (ST_ULONG) reqCtrl->u.jinit.resp_info->del_entries);
		}
		mvl_free_req_ctrl (reqCtrl);
#endif

#if (MMS_JSTAT_EN & REQ_EN)
		printf ("\n\n Testing DOM_SPEC JournalStat");
		jstat_req.jou_name.object_tag = DOM_SPEC;
		jstat_req.jou_name.domain_id = "mvlLiteDom1";
		jstat_req.jou_name.obj_name.vmd_spec = "MyJournal";
		ret = mvla_jstat (clientNetInfo, &jstat_req, &reqCtrl);
		if (ret == SD_SUCCESS)
			ret = waitReqDone (reqCtrl, timeOut);
		if (ret != SD_SUCCESS)
			printf ("\n mvl_jstat () Error, ret = 0x%X.", ret);
		else
		{
			printf ("\n mvl_jstat OK");
			printf ("\n   cur_entries = %lu", (ST_ULONG) reqCtrl->u.jstat.resp_info->cur_entries);
			printf ("\n   mms_deletable = %d", reqCtrl->u.jstat.resp_info->mms_deletable);
		}
		mvl_free_req_ctrl (reqCtrl);
#endif

#if (MMS_JREAD_EN & REQ_EN)
		printf ("\n\n Testing DOM_SPEC JournalRead");
		jread_req.jou_name.object_tag = DOM_SPEC;
		jread_req.jou_name.domain_id = "mvlLiteDom1";
		jread_req.jou_name.obj_name.vmd_spec = "MyJournal";
		jread_req.range_start_pres = 0;
		jread_req.range_stop_pres = 0;
		jread_req.list_of_var_pres = 0;
		jread_req.sa_entry_pres = 0;
		ret = mvla_jread (clientNetInfo, &jread_req, &reqCtrl);
		if (ret == SD_SUCCESS)
			ret = waitReqDone (reqCtrl, timeOut);
		if (ret != SD_SUCCESS)
			printf ("\n mvl_jread () Error, ret = 0x%X.", ret);
		else
		{
			printf ("\n mvl_jread OK");
			jread_resp = reqCtrl->u.jread.resp_info;
			printf ("\n   num_entry = %d", jread_resp->num_of_jou_entry);
			printf ("\n   more_follows = %d", jread_resp->more_follows);
			for (i = 0;  i < jread_resp->num_of_jou_entry;  i++)
			{
				jou_entry = &jread_resp->jou_entry[i];
				printf ("\n   Journal Entry # %d:", i);
				printf ("\n     entry_id_len = %d", jou_entry->entry_id_len);
				printf ("\n     occur_time.form = %s", (jou_entry->occur_time.form == MMS_BTOD6 ? "MMS_BTOD6" : "MMS_BTOD4"));
				printf ("\n     occur_time.ms  = %ld", (ST_LONG) jou_entry->occur_time.ms);
				printf ("\n     occur_time.day = %ld", (ST_LONG) jou_entry->occur_time.day);
				printf ("\n     entry_form_tag = %d", jou_entry->entry_form_tag);
				if (jou_entry->entry_form_tag == 2)
				{
					if (jou_entry->ef.data.list_of_var_pres)
					{
						for (j = 0;  j < jou_entry->ef.data.num_of_var;  j++)
						{
							printf ("\n       Var # %d: var_tag = %s", j,
								jou_entry->ef.data.list_of_var[j].var_tag);
							printf ("\n       Var # %d: value_spec.len = %d", j,
								jou_entry->ef.data.list_of_var[j].value_spec.len);
						}
					}
				}
				else
				{
					printf ("\n       annotation = %s",
						jou_entry->ef.annotation);
				}
			}	/* end "loop"	*/
		}
		mvl_free_req_ctrl (reqCtrl);	/* CRITICAL:		*/
#endif
		return;
	}
	/************************************************************************/
	/*			 test_iec_rpt					*/
	/* This function sets up to receive IEC-61850 or UCA reports on this connection*/
	/************************************************************************/
	RCB_INFO *test_iec_rpt (MVL_NET_INFO *clientNetInfo, RPT_TYPEIDS *rpt_typeids,
		ST_CHAR *dom_name,	/* domain which contains the RCB*/
		ST_CHAR *rcb_name)	/* RCB (e.g. "LLN0$BR$PosReport")*/
	{
		ST_RET ret;
		RCB_INFO *rcb_info;		/* UCA/IEC Report Control Block info	*/
		ST_UCHAR OptFlds [2];		/* 10 bit bitstring but only allow write of 9 bits*/
		ST_UCHAR TrgOps [1];		/* 8 bit bitstring			*/
		ALL_RCB_INFO *all_rcb_info;

		all_rcb_info = (ALL_RCB_INFO *) clientNetInfo->user_info;

		rcb_info = rcb_info_create (clientNetInfo, dom_name, rcb_name, rpt_typeids, 10);
		if (rcb_info)
		{
			/* Add RCB to list before enabling, so we're ready to receive RPTS immediately.*/
			list_add_last (&all_rcb_info->rcb_info_list, rcb_info);

			printf ("rcb_info->numDsVar = %d\n", rcb_info->numDsVar);
			/* Enable options we want to see in report.	*/
			OptFlds [0] = 0;
			OptFlds [1] = 0;
			/* These options valid for IEC BRCB, IEC URCB, or UCA.	*/
			BSTR_BIT_SET_ON(OptFlds, OPTFLD_BITNUM_SQNUM);
			BSTR_BIT_SET_ON(OptFlds, OPTFLD_BITNUM_TIMESTAMP);
			BSTR_BIT_SET_ON(OptFlds, OPTFLD_BITNUM_REASON);
			BSTR_BIT_SET_ON(OptFlds, OPTFLD_BITNUM_DATSETNAME);
			/* NOTE: these options ONLY available for IEC-61850 (not UCA)
			* SUBSEQNUM is only set by the server, so don't try to set it.
			*/
			if (rcb_info->rcb_type == RCB_TYPE_IEC_BRCB)
			{	/* These only valid for IEC BRCB	*/
				BSTR_BIT_SET_ON(OptFlds, OPTFLD_BITNUM_BUFOVFL);
				BSTR_BIT_SET_ON(OptFlds, OPTFLD_BITNUM_ENTRYID);
			}
			if (rcb_info->rcb_type != RCB_TYPE_UCA)
			{	/* These only valid for IEC BRCB or IEC URCB	*/
				BSTR_BIT_SET_ON(OptFlds, OPTFLD_BITNUM_DATAREF);
				BSTR_BIT_SET_ON(OptFlds, OPTFLD_BITNUM_CONFREV);
			}

			TrgOps [0] = 0xff;	/* enable ALL triggers	*/
			ret = rcb_enable (clientNetInfo, dom_name, rcb_name, OptFlds, TrgOps,
				5000,		/* Integrity Period (ms)	*/
				rpt_typeids,
				10);		/* timeout (s)	*/
			if (ret)
			{
				printf ("rcb_enable error\n");
				/* Remove this RCB from list and destroy it.	*/
				list_unlink (&all_rcb_info->rcb_info_list, rcb_info);
				rcb_info_destroy (rcb_info);
				rcb_info = NULL;
			}
		}
		else
			printf ("rcb_info_create error for dom='%s', rcb='%s'\n", dom_name, rcb_name);
		return (rcb_info);
	}
	/************************************************************************/
	/*				doCommService 				*/
	/************************************************************************/

	ST_VOID doCommService ()
	{
		ST_BOOLEAN event;
		wait_any_event (10);	/* Wait 1000 milliseconds		*/
		do
		{
			/* CRITICAL: do like this so both functions called each time through loop.*/
			event =  mvl_comm_serve ();	/* Perform communications service 	*/
#if  defined(GOOSE_RX_SUPP) || defined(GSSE_RX_SUPP) || defined(GSE_MGMT_RX_SUPP) || defined(SMPVAL_RX_SUPP)
			/* NOTE: this is called only if an appropriate protocol is enabled.*/
			event |= subnet_serve ();
#endif
		} while (event);		/* Loop until BOTH functions return 0*/

#if defined(DEBUG_SISCO)
		if (sLogCtrl->logCtrl & LOG_IPC_EN)
			slogIpcEvent ();	/* required for IPC Logging if gensock2.c is	*/
		/* not compiled with GENSOCK_THREAD_SUPPORT	*/

		/* At runtime, periodically need to service SLOG commands and calling connections. */
		/* The timing of this service is not critical, but to be responsive a default of    */
		/* 100ms works well.                                                                */ 
		slog_ipc_std_cmd_service ("logcfg.xml", NULL, NULL, SD_TRUE,  NULL, NULL);
#endif

#if defined(SUBNET_THREADS)
		/* This code is purely for demonstration purposes. No threads are 	*/
		/* created, but this shows what could be done in a separate thread.	*/
		/* You could also have a separate thread to process each list.		*/
		/* This just processes all packets on 4 separate lists.			*/
		{
			SN_UNITDATA *sn_udt;
			N_UNITDATA *n_udt;
#if defined(GOOSE_RX_SUPP)
			while ((sn_udt = goose_ind_get ()) != NULL)
			{
				goose_ind_process (sn_udt);	/* process it	*/
				chk_free (sn_udt);		/* free it	*/
			}
#endif
#if defined(SMPVAL_RX_SUPP)
			while ((sn_udt = smpval_ind_get ()) != NULL)
			{
				smpval_ind_process (sn_udt);	/* process it	*/
				chk_free (sn_udt);		/* free it	*/
			}
#endif
#if defined(GSE_MGMT_RX_SUPP)
			while ((sn_udt = gse_mgmt_ind_get ()) != NULL)
			{
				gse_mgmt_ind_process (sn_udt);	/* process it	*/
				chk_free (sn_udt);		/* free it	*/
			}
#endif
#if defined(GSSE_RX_SUPP)
			while ((n_udt = cltp_ind_get ()) != NULL)
			{
				cltp_ind_process (n_udt);		/* process it	*/
				clnp_free (n_udt);		/* free it	*/
			}
#endif
		}
#endif	/* SUBNET_THREADS	*/
	}

	/************************************************************************/
	/*				ctrlCfun				*/
	/************************************************************************/
	/* This function handles the ^c, and allows us to cleanup on exit	*/

	void ctrlCfun (int i)
	{
		doIt = SD_FALSE;
	}

	/************************************************************************/
	/*			kbService ()					*/
	/************************************************************************/

	/* NOTE for some UNIX systems:						*/
	/*	This function, if enebled, needs ukey.c to be linked into the	*/
	/*	project (for the kbhit function).				*/
	static ST_VOID kbService (ST_VOID)
	{
#if !defined(NO_KEYBOARD)
		ST_CHAR c;

		if (kbhit ())		/* Report test keyboard input */
		{
#if defined(_WIN32) || (defined(__QNX__) && !defined(__QNXNTO__))
			c = getch ();
#else
			c = (ST_CHAR) getchar ();	/* works only if term_init called first	*/
#endif

#if defined(S_SEC_ENABLED)
			if (c == 'u')
				secManCfgChange();
#endif

			if (c == 'x')
				doIt = SD_FALSE;	/* This triggers graceful exit	*/

			if (c == '?')
			{
#if defined(S_SEC_ENABLED)
				printf ("\n u : Update Security Configuration");
#endif
				printf ("\n x : Exit");
			}
		}
#endif	/* !defined(NO_KEYBOARD)	*/
	}

	/************************************************************************/
	/* 			connectToServer 				*/
	/************************************************************************/

	static MVL_NET_INFO *connectToServer (ST_CHAR *clientARName, ST_CHAR *serverARName)
	{
		ST_RET ret;
		MVL_REQ_PEND *reqCtrl;
		MVL_NET_INFO *clientNetInfo = NULL;
#if defined(S_SEC_ENABLED) || defined(ACSE_AUTH_ENABLED)
		ACSE_AUTH_INFO      authInfoStr = {0};
#endif
#if defined(S_SEC_ENABLED)
		S_SEC_ENCRYPT_CTRL  encryptCtrlStr = {0};
		S_SEC_LOC_AR       *loc_ar_sec = NULL;
		S_SEC_REM_AR       *rem_ar_sec = NULL;
#endif
		ACSE_AUTH_INFO     *authInfo = NULL;	/* conn authentication info	*/
		S_SEC_ENCRYPT_CTRL *encryptCtrl = NULL;	/* conn enctryption info	*/

		callingInitInfo.mms_p_context_pres = SD_TRUE;
		callingInitInfo.max_segsize_pres = SD_TRUE;
		callingInitInfo.max_segsize = mvl_cfg_info->max_msg_size;
		callingInitInfo.maxreq_calling = 1;
		/* NOTE: maxreq_called must be > 1 to allow mvla_fget to work.*/
		callingInitInfo.maxreq_called = 2;
		callingInitInfo.max_nest_pres = SD_TRUE;
		callingInitInfo.max_nest = 5;
		callingInitInfo.mms_detail_pres = SD_TRUE;
		callingInitInfo.version = 1;
		callingInitInfo.num_cs_init = 0;
		callingInitInfo.core_position = 0;
		callingInitInfo.param_supp[0] = m_param[0];
		callingInitInfo.param_supp[1] = m_param[1];
		memcpy (callingInitInfo.serv_supp, m_service_resp,11);

#if defined(S_SEC_ENABLED)
		authInfo = &authInfoStr;
		encryptCtrl = &encryptCtrlStr;
		/* set authentication and encryption infor for this connection */
		ret = ulSetSecurityCalling (clientARName, serverARName,
			&loc_ar_sec, &rem_ar_sec,
			authInfo, encryptCtrl);
		if (ret != SD_SUCCESS)
		{
			printf ("\n Initialization of security info failed");
			return (NULL);	/* error	*/
		}
#elif defined(ACSE_AUTH_ENABLED)
		/* Fill out an authentication structure */
		authInfo = &authInfoStr;
		authInfo->auth_pres = SD_TRUE;
		authInfo->mech_type = ACSE_AUTH_MECH_PASSWORD;
		strcpy (authInfo->u.pw_auth.password, "mypassword");	/* this is the password */
#endif  /* defined(ACSE_AUTH_ENABLED) */

		ret = mvla_initiate_req_ex (clientARName, serverARName, &callingInitInfo, 
			&initRespInfo, &clientNetInfo, &reqCtrl,
			authInfo, encryptCtrl);
		if (ret == SD_SUCCESS)
		{
#if defined(S_SEC_ENABLED)
			/* Save the AR Security pointers, for convenience when the confirm is rxd	*/
			clientNetInfo->loc_ar_sec = loc_ar_sec;
			clientNetInfo->rem_ar_sec = rem_ar_sec;
#endif
			ret = waitReqDone (reqCtrl, 10);
			/* CRITICAL: on timeout, must Abort or resources may not be freed.*/
			if (ret == MVL_ERR_USR_TIMEOUT)
				mvl_abort_req (clientNetInfo);
		}
		if (ret != SD_SUCCESS)
			clientNetInfo = NULL;

#if defined(S_SEC_ENABLED)
		ulFreeAssocSecurity (authInfo); /* free local auth info, not needed anymore	*/
#endif

		mvl_free_req_ctrl (reqCtrl);
		return (clientNetInfo);
	}

	/************************************************************************/
	/*			disconnectFromServer				*/
	/* Send "Conclude" request and check result.				*/
	/************************************************************************/

	static ST_RET disconnectFromServer (MVL_NET_INFO *clientNetInfo)
	{
		MVL_REQ_PEND *reqCtrl;
		ST_RET retcode;

		mvla_concl (clientNetInfo, &reqCtrl);	/* Send "Conclude" request	*/
		waitReqDone (reqCtrl, timeOut);
		retcode = reqCtrl->result;	/* save result before freeing reqCtrl	*/
		mvl_free_req_ctrl (reqCtrl);
		if (retcode)
			printf ("Conclude request failed: error=0x%04X\n", retcode);

		return (retcode);
	}

	/************************************************************************/
	/* 			u_mvl_info_rpt_ind				*/
	/************************************************************************/

	ST_VOID u_mvl_info_rpt_ind (MVL_COMM_EVENT *event)
	{
		/* Assume all reports are IEC 61850 reports and process as such.	*/
		if (u_iec_rpt_ind (event) != SD_SUCCESS)
			SLOGALWAYS0 ("ERROR: recieved report is not a valid IEC 61850 report.");
	}


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
		ST_ACSE_AUTH ret = ACSE_AUTH_SUCCESS;

#if defined(S_SEC_ENABLED)
		/* this Client will reject the connection since there is no security	*/
		/* checking code in place (see Server for sample)			*/
		ret = ACSE_DIAG_NO_REASON;
#elif defined(ACSE_AUTH_ENABLED)
		/* this Client will reject the connection since there is no password	*/
		/* checking code in place (see Server for sample)			*/
		ret = ACSE_DIAG_NO_REASON;
#endif

		return (ret);
	}

	/************************************************************************/
	/*			u_mvl_connect_cnf_ex   				*/
	/*----------------------------------------------------------------------*/
	/* This function is called when we have received an initiate response.	*/
	/* Depending on the server's authentication scheme, we may have been 	*/
	/* sent responding authentication that we can pull out of the response 	*/
	/* PDU.									*/
	/************************************************************************/

	ST_ACSE_AUTH u_mvl_connect_cnf_ex (MVL_NET_INFO *cc, AARE_APDU *ass_rsp_info)
	{
		ST_RET ret = ACSE_AUTH_SUCCESS;

#if defined(S_SEC_ENABLED)
		/* check security parameters of the Associate Confirm			*/
		ret = ulCheckSecureAssocConf (cc);
#elif defined(ACSE_AUTH_ENABLED)
		{
			ACSE_AUTH_INFO *rsp_auth_info;

			rsp_auth_info = &ass_rsp_info->auth_info;


			if(rsp_auth_info->auth_pres == SD_TRUE)
			{
				/* Looks like we have some authentication to look at, simply print  */
				/* the password and continue as normal.				*/
				if(rsp_auth_info->mech_type == ACSE_AUTH_MECH_PASSWORD)
				{
					printf("\nPassword recieved from peer: %s\n", rsp_auth_info->u.pw_auth.password);
				}
				/* We were sent a mechanism we don't support, let's reject the 	*/
				/* the connection with the appropriate diagnostic.			*/
				else
				{
					ret = ACSE_DIAG_AUTH_MECH_NAME_NOT_RECOGNIZED;
				}
			}
			else
			{
				/* Hmm... looks like we weren't sent any authentication even though */
				/* we require it. Let's reject with the appropriate diagnostic.	*/
				ret = ACSE_DIAG_AUTH_REQUIRED;
			}
		}
#endif /* defined(ACSE_AUTH_ENABLED) */

		/* to accept the confirm ACSE_AUTH_SUCCESS need to be returned	*/
		return ((ST_ACSE_AUTH) ret);
	}


	/************************************************************************/
	/*			disc_ind_fun					*/
	/************************************************************************/
	/* This function is called when connection is terminated.		*/

	static ST_VOID disc_ind_fun (MVL_NET_INFO *cc, ST_INT discType)
	{
		ALL_RCB_INFO *all_rcb_info;
		RCB_INFO *rcb_info;
		stuSMmsClient_UserInfo *pUserInfo = (stuSMmsClient_UserInfo*)cc->user_info;
		SMmsClient *pMmsClient = NULL;
		if(pUserInfo != NULL)
			pMmsClient = pUserInfo->pMmsClient;
		printf ("disconnect indication received.\n");
		/* Free up rcb_info if necessary.	*/
		if (pUserInfo != NULL && pUserInfo->pALL_RCB_INFO)
		{
			all_rcb_info = (ALL_RCB_INFO *)pUserInfo->pALL_RCB_INFO;
			while ((rcb_info = (RCB_INFO *)list_get_first (&all_rcb_info->rcb_info_list)) != NULL)
				rcb_info_destroy (rcb_info);
			//chk_free (all_rcb_info);
			delete pUserInfo->pALL_RCB_INFO;
			pUserInfo->pALL_RCB_INFO = NULL;
		}
		if(pMmsClient != NULL && pMmsClient->GetUserInfo_A() == cc->user_info)
		{
			//A网断开事件
			pMmsClient->DisConnect_A();
			if(pMmsClient->GetMVL_NET_INFO_A() == NULL && pMmsClient->GetMVL_NET_INFO_B() == NULL)
				pMmsClient->OnDisConnected();
		}
		else if(pMmsClient != NULL && pMmsClient->GetUserInfo_B() == cc->user_info)
		{
			//B网断开事件
			pMmsClient->DisConnect_B();
			if(pMmsClient->GetMVL_NET_INFO_A() == NULL && pMmsClient->GetMVL_NET_INFO_B() == NULL)
				pMmsClient->OnDisConnected();
		}
		else
		{
			LOGWARN("未知的MMS断开事件!");
		}
		cc->user_info=NULL;
		cc->rem_vmd = NULL;
	}

#if (MMS_IDENT_EN & RESP_EN)
	/************************************************************************/
	/*			u_mvl_ident_ind					*/
	/************************************************************************/
	ST_VOID u_mvl_ident_ind (MVL_IND_PEND *indCtrl)
	{
#if 0	/* enable this to print when Ind received		*/
		printf ("\n Identify Indication received.");
#endif
		indCtrl->u.ident.resp_info = &identify_response_info;
		mplas_ident_resp (indCtrl);
	}
#endif	/* #if (MMS_IDENT_EN & RESP_EN)	*/

	/************************************************************************/
	/*			getFile						*/
	/************************************************************************/
	static ST_RET getFile (MVL_NET_INFO *clientNetInfo, ST_CHAR *loc_file, ST_CHAR *rem_file)
	{
		FREAD_REQ_INFO   fread_req_info;
		FCLOSE_REQ_INFO  fclose_req_info;
		ST_RET ret;
		FILE *loc_fp;
		MVL_REQ_PEND *reqCtrl;
		ST_INT32 frsmid;
		ST_BOOLEAN more_follows;

		ret = mvla_fopen (clientNetInfo,
			rem_file,
			0,		/* init_pos: start at beginning of file	*/
			&reqCtrl);
		if (ret == SD_SUCCESS)
			ret = waitReqDone (reqCtrl, timeOut);

		if (ret == SD_SUCCESS)
			frsmid = reqCtrl->u.fopen.resp_info->frsmid;	/* save frsmid	*/
		mvl_free_req_ctrl (reqCtrl);
		if (ret != SD_SUCCESS)
			return (ret);

		loc_fp = fopen (loc_file, "wb");
		if (loc_fp == NULL)
			return (SD_FAILURE);	/* Can't open local file.	*/

		fread_req_info.frsmid = frsmid;	/* "fread" request doesn't chg.	*/
		do
		{
			ret = mvla_fread (clientNetInfo, &fread_req_info, &reqCtrl);
			if (ret == SD_SUCCESS)
				ret = waitReqDone (reqCtrl, timeOut);
			if (ret == SD_SUCCESS)
			{
				more_follows = reqCtrl->u.fread.resp_info->more_follows;
				fwrite (reqCtrl->u.fread.resp_info->filedata, 1,
					reqCtrl->u.fread.resp_info->fd_len, loc_fp);
			}
			mvl_free_req_ctrl (reqCtrl);
			if (ret != SD_SUCCESS)
				return (ret);
		} while (more_follows);

		fclose_req_info.frsmid = frsmid;
		ret = mvla_fclose (clientNetInfo, &fclose_req_info, &reqCtrl);
		if (ret == SD_SUCCESS)
			ret = waitReqDone (reqCtrl, timeOut);
		if (ret != SD_SUCCESS)
			printf ("\n mvl_fclose failed, ret = 0x%X", ret);
		mvl_free_req_ctrl (reqCtrl);

		fclose (loc_fp);
		return (SD_SUCCESS);
	}

	/************************************************************************/
	/*				init_log_cfg 				*/
	/************************************************************************/

	static ST_VOID init_log_cfg (ST_VOID)
	{
#if defined(DEBUG_SISCO)

#if defined (HARD_CODED_CFG)
		/* Use File logging							*/
		sLogCtrl->logCtrl = LOG_FILE_EN;

		/* Use time/date time log						*/
		sLogCtrl->logCtrl |= LOG_TIME_EN;

		/* File Logging Control defaults 					*/
		sLogCtrl->fc.fileName = "client.log";
		sLogCtrl->fc.maxSize = 1000000L;
		sLogCtrl->fc.ctrl = (FIL_CTRL_WIPE_EN | 
			FIL_CTRL_WRAP_EN | 
			FIL_CTRL_NO_APPEND |
			FIL_CTRL_MSG_HDR_EN);

		mms_debug_sel |= MMS_LOG_NERR;

		mvl_debug_sel |= MVLLOG_ERR;
		mvl_debug_sel |= MVLLOG_NERR;
#if 0
		mvl_debug_sel |= MVLLOG_ACSE;
		mvl_debug_sel |= MVLLOG_ACSEDATA;
		mvl_debug_sel |= MVLLOG_TIMING;
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

#if !defined(REDUCED_STACK)
		clnp_debug_sel |= CLNP_LOG_ERR;
		clnp_debug_sel |= CLNP_LOG_NERR;
#if 0
		clnp_debug_sel |= CLNP_LOG_REQ;
		clnp_debug_sel |= CLNP_LOG_IND;
		clnp_debug_sel |= CLNP_LOG_ENC_DEC;
		clnp_debug_sel |= CLNP_LOG_LLC_ENC_DEC;
		clnp_debug_sel |= CLSNS_LOG_REQ;
		clnp_debug_sel |= CLSNS_LOG_IND;
#endif
#endif

#else	/* !defined (HARD_CODED_CFG)	*/
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

		/* At initialization, install a SLOGIPC command handler. The      */
		/* build in SLOGIPC handler just receives the command and put's   */
		/* on a list to be handled by the application at it's leisure ... */
		sLogCtrl->ipc.slog_ipc_cmd_fun = slog_ipc_std_cmd_fun;

		if (logcfgx_ex (sLogCtrl, "logcfg.xml", NULL, SD_FALSE, SD_FALSE) != SD_SUCCESS)
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
	/*				init_mem				*/
	/************************************************************************/

	static ST_VOID mem_chk_error_detected (ST_VOID);
	static ST_VOID *my_malloc_err (size_t size);
	static ST_VOID *my_calloc_err (size_t num, size_t size);
	static ST_VOID *my_realloc_err (ST_VOID *old, size_t size);

	//static ST_CHAR *spareMem;

	static ST_VOID init_mem ()
	{
		/* Allocate spare memory to allow logging/printing memory errors	*/
		//spareMem = (ST_CHAR *) malloc (500);

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
			//free (spareMem);
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
#if (MMS_FOPEN_EN & RESP_EN)
	/************************************************************************/
	/*			u_mvl_fopen_ind					*/
	/************************************************************************/
	ST_VOID u_mvl_fopen_ind (MVL_IND_PEND *indCtrl)
	{
		FILE *fp;
		FOPEN_RESP_INFO resp_info;
		struct stat  stat_buf;

		fp = fopen (indCtrl->u.fopen.filename, "rb");	/* CRITICAL: use "b" flag for binary transfer*/
		if (fp == NULL)
		{
			_mplas_err_resp (indCtrl,11,6);		/* File-access denied	*/
			return;
		}
		if (fseek (fp, indCtrl->u.fopen.init_pos, SEEK_SET))
		{
			_mplas_err_resp (indCtrl,11,5);		/* Position invalid	*/
			return;
		}

		/* WARNING: this only works if (FILE *) is a 32-bit pointer.		*/
		resp_info.frsmid = (ST_INT32) gen_FilePtrIdx(fp);

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

		fp = (FILE *) get_FilePtrByIdx(fread_ctrl->req_info->frsmid);
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
	/*			u_mvl_fclose_ind				*/
	/************************************************************************/
	ST_VOID u_mvl_fclose_ind (MVL_IND_PEND *indCtrl)
	{
		FILE *fp;
		MVLAS_FCLOSE_CTRL *fclose_ctrl = &indCtrl->u.fclose;

		fp = (FILE *) get_FilePtrByIdx(fclose_ctrl->req_info->frsmid);
		remove_FilePtrByIdx(fclose_ctrl->req_info->frsmid);
		if (fclose (fp))
			_mplas_err_resp (indCtrl, 11, 0);	/* File problem, other	*/
		else
			mplas_fclose_resp (indCtrl);
	}
#endif	/* #if (MMS_FCLOSE_EN & RESP_EN)	*/

	/************************************************************************/
	/*			waitReqDone					*/
	/*----------------------------------------------------------------------*/
	/* Wait for request to complete. Service communications while waiting.	*/
	/* Check for timeout or Ctrl-C (i.e. doIt=SD_FALSE).			*/
	/* NOTE: if other processing should be done while waiting, add it to	*/
	/*       this function.							*/
	/* Parameters:								*/
	/*	req		request to wait for.				*/
	/*	timeout		time to wait (seconds). 0 means wait forever.	*/
	/* RETURNS: SD_SUCCESS, MVL_ERR_USR_TIMEOUT or other MVL error code.	*/
	/************************************************************************/
	ST_RET waitReqDone (MVL_REQ_PEND *req, ST_INT timeout)
	{
		ST_DOUBLE stopTime = sGetMsTime() + (ST_DOUBLE) timeout * 1000.0;

		while (req->done == SD_FALSE)		/* wait until done	*/
		{
			if (timeout != 0  &&  sGetMsTime() > stopTime)
			{
				req->result = MVL_ERR_USR_TIMEOUT;
				MVL_LOG_ERR1 ("MMS request (opcode = %d) timed out", req->op); 
				break;
			}
			if (doIt == SD_FALSE)	/* user hit Ctrl-C, stop waiting	*/
			{
				req->result = SD_FAILURE;
				break;
			}
			doCommService ();	/* wait for events if possible & call mvl_comm_serve*/
		}	/* end loop	*/

		MVL_NET_INFO *pNetInfo = (MVL_NET_INFO *)req->net_info;
		if (pNetInfo)
		{
			stuSMmsClient_UserInfo *pUserInfo = (stuSMmsClient_UserInfo *)pNetInfo->user_info;
			if (pUserInfo)
			{
				if (req->result == SD_FAILURE)	// 当做否定相应触发回调
					pUserInfo->pMmsClient->OnRequestDenial();
				else if (req->result == MVL_ERR_USR_TIMEOUT)	// 超时相应触发回调
					pUserInfo->pMmsClient->OnRequestTimeout();
				else if (req->result != MVL_ERR_USR_TIMEOUT)	// 更新最后一次通信时间
					pUserInfo->pMmsClient->onRecvNewPacket(SDateTime::currentDateTime());
			}
		}
	
		return (req->result);
	}
	/************************************************************************/
	/*                       named_var_read					*/
	/* Read a single named variable.					*/
	/*   scope = VMD_SPEC, DOM_SPEC, or AA_SPEC				*/
	/* CRITICAL: the local variable "parse_info" is written indirectly 	*/
	/*           from waitReqDone when the response is received. The call	*/
	/*           to "waitReqDone" MUST NOT be moved outside this function.	*/
	/************************************************************************/
	ST_RET named_var_read (MVL_NET_INFO *net_info, ST_CHAR *varName,
		ST_INT scope, ST_CHAR *domName,
		ST_INT type_id, ST_VOID *dataDest, ST_INT timeOut)
	{
		ST_INT num_data;
		ST_CHAR buffer[512];
		MVL_READ_RESP_PARSE_INFO parse_info;
		READ_REQ_INFO *req_info;
		VARIABLE_LIST *vl;
		ST_INT ret;
		MVL_REQ_PEND *reqCtrl;

		/* Create a read request info struct  */
		req_info = (READ_REQ_INFO *) buffer;
		req_info->spec_in_result = SD_FALSE;
		req_info->va_spec.var_acc_tag = VAR_ACC_VARLIST;
		req_info->va_spec.num_of_variables = 1;
		vl = (VARIABLE_LIST *) (req_info + 1);

		vl->alt_access_pres = SD_FALSE;
		vl->var_spec.var_spec_tag = VA_SPEC_NAMED;
		vl->var_spec.vs.name.object_tag = scope;	/* set scope	*/
		if (scope == DOM_SPEC)
			vl->var_spec.vs.name.domain_id = domName;	/* set domain name	*/
		vl->var_spec.vs.name.obj_name.vmd_spec = varName;

		num_data = 1;     /* Number of named variables returned		*/

		/* IMPORTANT: start with clean structure, then set appropriate elements.*/
		memset (&parse_info, 0, sizeof (parse_info));
		parse_info.dest = dataDest;
		parse_info.type_id = type_id;
		parse_info.descr_arr = SD_FALSE;

		/* Send read request. */
		ret = mvla_read_variables (net_info,req_info,num_data,
			&parse_info, &reqCtrl);

		/* If request sent successfully, wait for reply.	*/
		if (ret == SD_SUCCESS)
			ret = waitReqDone (reqCtrl, timeOut);

		mvl_free_req_ctrl (reqCtrl);
		if (ret != SD_SUCCESS)
			return (ret);

		return (parse_info.result);	/* Return the single variable result	*/
	}
	/************************************************************************/
	/*			named_var_write					*/
	/* Write a single named variable.					*/
	/*   scope = VMD_SPEC, DOM_SPEC, or AA_SPEC				*/
	/* CRITICAL: the local variable "wr_info" is written indirectly 	*/
	/*           from waitReqDone when the response is received. The call	*/
	/*           to "waitReqDone" MUST NOT be moved outside this function.	*/
	/************************************************************************/
	ST_RET named_var_write (MVL_NET_INFO *netInfo, ST_CHAR *varName,
		ST_INT scope, ST_CHAR *domName,
		ST_INT type_id,	ST_VOID *dataSrc, ST_INT timeOut)
	{
		ST_CHAR buffer[512];
		MVL_WRITE_REQ_INFO *wr_info;
		WRITE_REQ_INFO *req_info;
		VARIABLE_LIST *vl;
		ST_RET ret;
		MVL_REQ_PEND *reqCtrl;

		req_info = (WRITE_REQ_INFO *) buffer;
		req_info->num_of_data = 1;
		req_info->va_spec.var_acc_tag = VAR_ACC_VARLIST;
		req_info->va_spec.num_of_variables = 1;
		vl = (VARIABLE_LIST *) (req_info + 1);

		vl->alt_access_pres = SD_FALSE;
		vl->var_spec.var_spec_tag = VA_SPEC_NAMED;
		vl->var_spec.vs.name.object_tag = scope;	/* set scope	*/
		if (scope == DOM_SPEC)
			vl->var_spec.vs.name.domain_id = domName;	/* set domain name	*/
		vl->var_spec.vs.name.obj_name.vmd_spec = varName;

		/* Alloc array of structs. Here only one var so alloc one struct.	*/
		/* To read multiple vars, allocate more.				*/
		/* CRITICAL: response code writes to this struct. Do not free until	*/
		/*           response received.					*/ 
		wr_info = (MVL_WRITE_REQ_INFO*)chk_calloc (1, sizeof (MVL_WRITE_REQ_INFO));

		wr_info->local_data = dataSrc;
		wr_info->type_id = type_id;
		wr_info->arr = SD_FALSE;

		/* Send write request.	*/
		ret = mvla_write_variables (netInfo, req_info, 1, wr_info, &reqCtrl);

		/* If request sent successfully, wait for response.	*/
		if (ret == SD_SUCCESS)
			ret = waitReqDone (reqCtrl, timeOut);

		mvl_free_req_ctrl (reqCtrl);
		if (ret == SD_SUCCESS)	/* response received OK.	*/
			ret = wr_info->result;	/* return single variable result	*/
		chk_free (wr_info);

		return (ret);
	}


	/************************************************************************/
	/* This function processes the variable data according to the type	*/
	/* defined in var->type_id. The function pointer table			*/
	/* "&mlog_arb_log_ctrl" passed to sxd_process_arb_data causes it	*/
	/* to just log the data (see mlogavar.c).				*/
	/* NOTE: User may pass a different function pointer table to		*/
	/*       "sxd_process..." to perform different processing.		*/ 
	/************************************************************************/
	static ST_VOID process_var_data (MVL_VAR_ASSOC *var, OBJECT_NAME *varObjName)
	{
		MVL_TYPE_CTRL *type_ctrl;

		if (varObjName->object_tag == DOM_SPEC)
			SLOGALWAYS2 ("DATA for variable '%s' in domain '%s':",
			var->name, varObjName->domain_id);
		else
			SLOGALWAYS1 ("DATA for variable '%s':", var->name);

		type_ctrl = mvl_type_ctrl_find (var->type_id);
		if (type_ctrl)
		{
			sxd_process_arb_data((char*)var->data, type_ctrl->rt, type_ctrl->num_rt,
				NULL,	/* pointer to any user data	*/
				&mlog_arb_log_ctrl,	/* function ptr table	*/
				NULL);	/* pointer to "elPres" array. Always NULL.*/
		}
		else
			SLOGCALWAYS2 ("  ERROR: type_id is invalid for variable '%s' in domain '%s'",
			var->name, varObjName->domain_id);
	}
	/************************************************************************/
	/*			getVarNames					*/
	/* This funct based on getGnlVarNames in mvl_uca.c, but much simpler	*/
	/* because it doesn't need to handle "ContinueAfter". 			*/
	/************************************************************************/
#define MAX_NEST_LEVEL	10
	ST_VOID getVarNames (MVL_VAR_ASSOC *va,
		ST_CHAR *allNameBuf,	/* buffer for all names			*/
		ST_CHAR **dest,		/* array of ptrs to names (max=numUcaRt)*/
		RUNTIME_TYPE *ucaRt,	/* array of RUNTIME_TYPE		*/
		ST_INT numUcaRt,	/* num entries in array of RUNTIME_TYPE	*/
		ST_INT *numNames)
	{
		ST_INT i;
		ST_INT nameCount;
		ST_INT sortedNum;
		/* prefix len should never reach MAX_IDENT_LEN but could get very close*/
		ST_CHAR namePrefix[MAX_NEST_LEVEL][MAX_IDENT_LEN+1];
		ST_INT nestLevel;
		ST_INT strLen;
		ST_INT get;
		ST_INT put;
		ST_INT numNewNames;
		ST_BOOLEAN compress;
		ST_CHAR *comp_name;		/* component name		*/
		ST_CHAR *currNamePos;		/* current position in buffer	*/

		currNamePos = allNameBuf;	/* start out pointing to head of buffer	*/

		if ( (va->flags & MVL_VAR_FLAG_UCA) == 0)
		{  
			++(*numNames);
			dest[0] = va->name;
			return;
		}

		memset (namePrefix, 0, sizeof(namePrefix));
		strcpy (namePrefix[0], va->name);

		/* Put base variable name in first entry.	*/
		nameCount = *numNames;
		dest[0] = va->name;
		++nameCount;

		/* OK, now we start doing the real thing. Derive names for this 	*/
		/* type, put them into the dest array.					*/

		compress = SD_FALSE;

		/* This code assumes all UCA vars are structs, so first tag must be RT_STR_START*/
		assert (ucaRt->el_tag == RT_STR_START);

		nestLevel = 0;
		for (i = 0; i < numUcaRt; ++i, ++ucaRt)
		{
			sortedNum = ucaRt->mvluTypeInfo.sortedNum;
			assert (sortedNum < numUcaRt);	/* this should never fail.	*/
			comp_name = (char *)ms_comp_name_find (ucaRt);
			if (strlen (comp_name))
			{
				/* Chk len is legal BEFORE writing (need room for 2 strings + '$'.*/
				if (strlen (namePrefix[nestLevel]) + strlen (comp_name) + 1 <= MAX_IDENT_LEN)
				{
					sprintf (currNamePos, "%s$%s", namePrefix[nestLevel], comp_name);
					strLen = strlen (currNamePos);
					assert (strLen <= MAX_IDENT_LEN);	/* if this fails, len chk in "if" is wrong*/
					/* NOTE: names not stored in order. "dest" array puts "pointers" in order*/
					dest[sortedNum] = currNamePos;
					currNamePos += strLen + 1;	/* point after name just added	*/
					++nameCount;
				}
				else
				{
					SLOGALWAYS2 ("ERROR: Derived variable name '%s$%s' too long to be stored",
						namePrefix[nestLevel], comp_name);
					compress = SD_TRUE;
				}
			}

			if (ucaRt->el_tag == RT_STR_START)
			{
				comp_name = (char *)ms_comp_name_find (ucaRt);
				if (strlen (comp_name))
				{
					++nestLevel;
					assert (nestLevel < MAX_NEST_LEVEL);
					assert (nestLevel > 0);
					strcpy (namePrefix[nestLevel], namePrefix[nestLevel-1]);
					strncat_maxstrlen (namePrefix[nestLevel], "$",       MAX_IDENT_LEN);
					strncat_maxstrlen (namePrefix[nestLevel], comp_name, MAX_IDENT_LEN);
				}
			}
			else if (ucaRt->el_tag == RT_STR_END)
				--nestLevel;
			else if (ucaRt->el_tag == RT_ARR_START)
			{					     /* Skip the array elements	*/
				/* There is no way to create UCA names for objects inside an	*/
				/* array, so skip over the array.					*/
				i += (ucaRt->u.arr.num_rt_blks + 1);
				ucaRt += (ucaRt->u.arr.num_rt_blks + 1);
			}
		}

		/* DEBUG: at this point, 'nestLevel' should equal (-1). This may	*/
		/* sound strange, but nestLevel NOT incremented on first RT_STR_START,*/
		/* so last RT_STR_END makes it -1.					*/
		assert (nestLevel == -1);

		/* If we had to skip one or more names, we need to eliminate the	*/
		/* holes in the name pointer table.					*/
		if (compress == SD_TRUE)
		{
			get = 0;
			put = 0;
			numNewNames = nameCount - *numNames;
			while (put < numNewNames)
			{
				if (dest[get] != NULL)
					dest[put++] = dest[get];
				++get;
			}
		}

		*numNames = nameCount;
	}

	/************************************************************************/
	/*			named_var_find_read_and_log			*/
	/* Find a named variable in the VMD, send a "Read" request to the	*/
	/* Server. If successful, log the variable's data.			*/
	/************************************************************************/
	ST_RET named_var_find_read_and_log (MVL_NET_INFO *net_info,
		MVL_VMD_CTRL *vmd_ctrl,
		OBJECT_NAME *varObjName)
	{
		ST_RET retcode = SD_FAILURE;
		MVL_VAR_ASSOC *var;

		/* Create a temporary variable	*/
		var = u_mvl_get_va_aa (vmd_ctrl,
			MMSOP_INFO_RPT,
			varObjName, 
			net_info,
			SD_FALSE,		/* alt_access_pres	*/
			NULL,		/* alt_acc		*/
			NULL);		/* alt_access_done_out	*/
		if (var==NULL)
		{	/* Cannot find variable. Log it.	*/
			if (varObjName->object_tag == DOM_SPEC)
				SLOGALWAYS2 ("Error: Cannot find variable '%s' in domain '%s'",
				varObjName->obj_name.item_id, varObjName->domain_id);
			else
				SLOGALWAYS1 ("Error: Cannot find variable '%s'", varObjName->obj_name.item_id);
		}
		else
		{	/* Found the variable. Now read it.	*/
			retcode = named_var_read (net_info,
				varObjName->obj_name.item_id,
				varObjName->object_tag,
				varObjName->domain_id,
				var->type_id, var->data, timeOut); 
			if (varObjName->object_tag == DOM_SPEC)
				SLOGALWAYS2 ("Reading variable '%s' in domain '%s'",
				varObjName->obj_name.item_id, varObjName->domain_id);
			else
				SLOGALWAYS1 ("Reading variable '%s'",
				varObjName->obj_name.item_id);
			if (retcode)
				SLOGCALWAYS1 ("RESULT: Error 0x%X", retcode);
			else
			{
				SLOGCALWAYS0 ("RESULT: Success");
				/* log the variable's data	*/
				process_var_data (var, varObjName);
			}

			/* Free the tmp var created by u_mvl_get_va_aa	*/
			u_mvl_free_va (MMSOP_INFO_RPT, var, net_info);
		}

		return (retcode);
	}
	/************************************************************************/
	/*			log_iec_remote_vmd_var_names			*/
	/* Log all DOM_SPEC variable names for this VMD.			*/
	/* NOTE: This function is for demonstration purposes only.		*/
	/************************************************************************/
	ST_VOID log_iec_remote_vmd_var_names (MVL_VMD_CTRL *vmd_ctrl)
	{
		ST_INT dom_idx, var_idx;

		if (vmd_ctrl == NULL)
		{
			printf ("ERROR: Remote VMD not found.\n");
			return; /* cannot continue	*/
		}

		for (dom_idx = 0;  dom_idx < vmd_ctrl->num_dom; dom_idx++)
		{
			MVL_DOM_CTRL *dom = vmd_ctrl->dom_tbl[dom_idx];
			SLOGALWAYS2 ("Domain[%d] = %s", dom_idx, dom->name);
			for (var_idx = 0; var_idx < dom->num_var_assoc; var_idx++)
			{
				MVL_VAR_ASSOC *var = dom->var_assoc_tbl[var_idx];
				ST_CHAR **dest;
				ST_RET rc;
				RUNTIME_TYPE *ucaRt;
				ST_INT numUcaRt;
				ST_CHAR *allNameBuf;
				ST_INT numNames = 0;
				ST_INT j;		/* loop counter	*/
				rc = mvl_get_runtime (var->type_id, &ucaRt, &numUcaRt);
				if (rc == SD_SUCCESS)
				{  
					/* Allocate a name buffer and array of pointers to names.	*/
					allNameBuf = (ST_CHAR *) chk_calloc (1, numUcaRt * (MAX_IDENT_LEN +1));
					dest = (ST_CHAR **) chk_calloc (1, numUcaRt * sizeof (ST_CHAR *));
					getVarNames (var, allNameBuf, dest, ucaRt, numUcaRt, &numNames);
					SLOGCALWAYS2 ("  Variable[%d] = %s (flattened names):", var_idx, var->name);
					for (j = 0; j<numNames; j++)
						SLOGCALWAYS1 ("    %s", dest[j]);	/* log flattened name	*/
					/* Free the name buffer and array of pointers to names.	*/
					chk_free (allNameBuf);
					chk_free (dest);
				}
			}
		}
	}

	/************************************************************************/
	/*			remote_obj_create				*/
	/* Read all object configuration files (especially SCL) and create all	*/
	/* objects.								*/
	/************************************************************************/
	ST_RET remote_obj_create (MVL_VMD_CTRL *vmd_ctrl,
		ST_CHAR *scl_filename,
		ST_CHAR *ied_name,
		ST_CHAR *access_point_name,
		SCL_INFO *scl_info)
	{
		ST_RET ret;
		ST_INT brcb_bufsize=0;	/* never used for remote objects	*/
		ST_UINT report_scan_rate=0;	/* never used for remote objects*/

		ret = scl_parse (scl_filename,
			ied_name,			/* IED name	*/
			access_point_name,		/* AccessPoint	*/
			scl_info);

		/* Create types only if everything successful up to now.	*/
		if (ret == SD_SUCCESS)
		{
			/* This creates types & saves the type_id in each SCL_LN & SCL_LNTYPE.*/
			/* DO NOT call it again with same scl_info.				*/
			/* If it is called again, it will fail, but will not corrupt scl_info.*/
			/* NOTE: arg #4 always SD_FALSE so types not named.		*/
			ret = scl2_datatype_create_all (vmd_ctrl, scl_info, 14000, SD_FALSE, NULL);
		}

		/* Create variables only if everything successful up to now.	*/
		if (ret == SD_SUCCESS)
			ret = scl2_ld_create_all (vmd_ctrl, scl_info, report_scan_rate, brcb_bufsize,
			SD_TRUE);	/* create "client" model (no RCB, etc.)*/
		return (ret);
	}
	/************************************************************************/
	/*			config_iec_remote_vmd				*/
	/************************************************************************/
	MVL_VMD_CTRL *config_iec_remote_vmd (
		ST_CHAR *scl_filename,
		ST_CHAR *ied_name,
		ST_CHAR *access_point_name,
		SCL_INFO *remote_scl_info)	/* filled in by this function	*/
	{
		MVL_VMD_CTRL *vmd_ctrl;
		ST_RET ret;	
		vmd_ctrl = mvl_vmd_create (mvl_max_dyn.doms, mvl_max_dyn.vmd_vars,
			mvl_max_dyn.vmd_nvls, mvl_max_dyn.journals);
		ret = remote_obj_create (vmd_ctrl, 
			scl_filename,
			ied_name,
			access_point_name,
			remote_scl_info);
		if (ret != SD_SUCCESS)
		{
			printf ("\n DEBUG: ERROR: remote_obj_create FAILED.");
			mvl_vmd_destroy (vmd_ctrl);	/* Destroy VMD created above.	*/
			vmd_ctrl = NULL;
		}
		return (vmd_ctrl);
	}
	/************************************************************************/
	/*			test_iec_remote_vmd				*/
	/* Read and log "some" selected remote VMD data (see item_id).		*/
	/* NOTE: this is for demonstration purposes only. It simply shows how	*/
	/*       different variables can be read and processed. It looks for 	*/
	/*       common variables in every domain of the VMD, tries to read	*/
	/*	 the variables, and then logs the data.				*/
	/************************************************************************/
	ST_VOID test_iec_remote_vmd (
		MVL_NET_INFO *net_info,
		MVL_VMD_CTRL *vmd_ctrl)
	{
		ST_INT dom_idx;
		OBJECT_NAME varObjName;
		ST_RET retcode;

		if (vmd_ctrl == NULL)
		{
			printf ("ERROR: Remote VMD not found.\n");
			return; /* cannot continue	*/
		}

		/* Loop through all Domains in this VMD.	*/
		for (dom_idx = 0;  dom_idx < vmd_ctrl->num_dom; dom_idx++)
		{
			MVL_DOM_CTRL *dom = vmd_ctrl->dom_tbl[dom_idx];

			/* Try reading top level variable from this domain	*/
			varObjName.object_tag = DOM_SPEC;
			varObjName.domain_id = dom->name;
			varObjName.obj_name.item_id = "LLN0";
			/* This funct logs on error so may not need to check return.*/
			retcode = named_var_find_read_and_log (net_info, vmd_ctrl, &varObjName);

			/* Try reading lower level variable from this domain	*/
			varObjName.object_tag = DOM_SPEC;
			varObjName.domain_id = dom->name;
			varObjName.obj_name.item_id = "LLN0$DC$NamPlt$vendor";
			retcode = named_var_find_read_and_log (net_info, vmd_ctrl, &varObjName);

			/* Try reading another lower level variable from this domain*/
			varObjName.object_tag = DOM_SPEC;
			varObjName.domain_id = dom->name;
			varObjName.obj_name.item_id = "LLN0$DC$NamPlt$configRev";
			retcode = named_var_find_read_and_log (net_info, vmd_ctrl, &varObjName);
		}
	}

	/************************************************************************/
	/*			u_mvl_scl_set_initial_value			*/
	/* Called during SCL processing to convert initial value text to data.	*/
	/* NOTE: only called if MVL library conversion fails.			*/
	/************************************************************************/
	ST_RET u_mvl_scl_set_initial_value (SCL2_IV_TRANSLATE_CTRL *translate)
	{
		/* NOTE: do nothing but return SD_SUCCESS so SCL processing continues.*/
		/* Initial values not usable in client application.			*/
		return (SD_SUCCESS);
	}
	/************************************************************************/
	/*			u_set_all_leaf_functions			*/
	/* Called during SCL processing to set up leaf functions. The client 	*/
	/* does not need any leaf functions, so this function does nothing, but	*/
	/* it must always return SD_SUCCESS, so SCL processing will continue.	*/
	/************************************************************************/
	ST_RET u_set_all_leaf_functions (RUNTIME_CTRL *rt_ctrl, SCL_LNTYPE *scl_lntype)
	{
		return (SD_SUCCESS);	/* always return SD_SUCCESS	*/
	}
	/************************************************************************/
	/* NOTE: the following global variables and functions are needed to	*/
	/* satisfy the linker when the "mvlu" library is used, but they should	*/
	/* NEVER be used.							*/ 
	/************************************************************************/
	MVLU_RD_FUN_INFO mvluRdFunInfoTbl[] =
	{
		{"", NULL}
	};
	ST_INT mvluNumRdFunEntries = sizeof(mvluRdFunInfoTbl)/sizeof(MVLU_RD_FUN_INFO);

	MVLU_WR_FUN_INFO mvluWrFunInfoTbl[] =
	{
		{"", NULL}
	};
	ST_INT mvluNumWrFunEntries = sizeof(mvluWrFunInfoTbl)/sizeof(MVLU_WR_FUN_INFO);

	ST_VOID u_mvl61850_ctl_oper_begin (ST_CHAR *oper_ref)
	{
		assert (0);	/* should never be called	*/
	}

	ST_VOID u_mvl61850_ctl_oper_end (MVL_NET_INFO *net_info, ST_CHAR *oper_ref, MVL_VAR_ASSOC *base_var)
	{
		assert (0);	/* should never be called	*/
	}

	ST_VOID  u_mvl_sbo_operate (MVL_SBO_CTRL *sboSelect, 
		MVLU_WR_VA_CTRL *mvluWrVaCtrl)
	{
		assert (0);	/* should never be called	*/
	}

	ST_VOID u_mvlu_rpt_time_get (MMS_BTIME6 *TimeOfEntry)
	{
		assert (0);	/* should never be called	*/
	}

	ST_RET u_mvlu_resolve_leaf_ref (ST_CHAR *leafName, ST_INT *setFlagsIo, 
		ST_CHAR *refText, ST_RTREF *refOut)
	{
		assert (0);	/* should never be called	*/
		return (SD_FAILURE);
	}

#ifdef __cplusplus
};
#endif

SPtrList<DIB_ENTRY> g_SMms_Dib_Index_Used;
SLock g_SMms_Global_Lock;
SPtrList<SMmsClient> g_SmmsClients;
bool g_bMmsGlobalExit=false;
SMmsClient::SMmsClient()
{
	m_bQuit = false;
	m_iThreads =  0;
	memset(m_sServerIpA,0,sizeof(m_sServerIpA));
	memset(m_sServerIpB,0,sizeof(m_sServerIpB));
	m_iPort = 0;
	m_pMVL_NET_INFO_A = NULL;//A网连接实例,MVL_NET_INFO*,NULL表示未连接
	m_pMVL_NET_INFO_B = NULL;//B网连接实例,MVL_NET_INFO*,NULL表示未连接
	m_pMVL_NET_INFO_RptEn = NULL;
	m_pDIB_ENTRY_A = NULL;//A网的连接DIB条目指针，DIB_ENTRY*,NULL表示未连接
	m_pDIB_ENTRY_B = NULL;//B网的连接DIB条目指针，DIB_ENTRY*,NULL表示未连接
	m_pUserInfo_A = NULL;//A网的连接用户信息指针
	m_pUserInfo_B = NULL;//B网的连接用户信息指针
	m_ReportInfos.setAutoDelete(true);
	m_Datasets.setAutoDelete(true);
	m_LogicDevices.setAutoDelete(true);
	g_SMms_Dib_Index_Used.setShared(true);
	m_isSystemTaskProcessing = false;
	m_isUserTaskProcessing = false;
}

SMmsClient::~SMmsClient()
{
	Stop();
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  设置MMS服务器地址，必须在启动前进行设置
// 作    者:  邵凯田
// 创建时间:  2015-9-16 19:36
// 参数说明:  @sServerIpA为服务端A网IP地址
//         :  @sServerIpB为服务端B网IP地址
//         :  @iPort为服务TCP端口号，缺省为102
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void SMmsClient::SetMmsServerAddr(char *sServerIpA,char *sServerIpB/*=""*/,int iPort/*=102*/)
{
	memset(m_sServerIpA,0,sizeof(m_sServerIpA));
	strcpy(m_sServerIpA, sServerIpA);
	memset(m_sServerIpB,0,sizeof(m_sServerIpB));
	strcpy(m_sServerIpB, sServerIpB);
	m_iPort = iPort;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  初始化MMS客户端运行环境,一个进程仅运行一次即可
// 作    者:  邵凯田
// 创建时间:  2015-9-21 18:20
// 参数说明:  @bSingleThreadMode表示多个客户端实例时是否采用单线程运行模式，true表示单线程模式，false表示多线程模式
// 返 回 值:  true表示初始化成功，false表示初始化失败
//////////////////////////////////////////////////////////////////////////
bool SMmsClient::InitGlobal(bool bSingleThreadMode/*=false*/)
{
	ST_RET ret;
	MVL_CFG_INFO mvlCfg;
	/* NOTE: serverName gets overwritten if USE_DIB_LIST defined.	*/
	ST_CHAR serverName [MAX_IDENT_LEN+1] = "remote1";	/* */
	static RPT_TYPEIDS rpt_typeids;
	ST_CHAR *usageString = "usage: %s -m test_mode\n-m test_mode\tTesting mode (iecrpt, ucarpt, ieccontrol, or iecremote)";
	g_bSingleThreadMode = bSingleThreadMode;
	static bool bInit = false;
	if(bInit)
	{
		g_SmmsClients.setShared(true);
		return true;
	}
	bInit = true;
	printf ("%s Version %s\n", MMSLITE_NAME, MMSLITE_VERSION);
#if defined(S_SEC_ENABLED)
	printf ("%s Version %s\n", S_SEC_LITESECURE_NAME, S_SEC_LITESECURE_VERSION_STR);
#endif
	puts (MMSLITE_COPYRIGHT);


#ifdef S_MT_SUPPORT
	/* init glbsem explicitly to avoid auto-initialization calls from multiple threads */
	if ((ret = gs_init()) != SD_SUCCESS)
	{
		LOGFAULT("gs_init() failed");
		return false;
	}
#endif

	/* init stime explicitly to avoid auto-initialization calls from multiple threads */
	if ((ret = stimeInit()) != SD_SUCCESS)
	{
		LOGFAULT("stimeInit() failed");
		return false;
	}

#if defined(NO_GLB_VAR_INIT)
	mvl_init_glb_vars ();
#endif

	init_mem ();		/* Set up memory allocation tools 		*/

	init_log_cfg ();

	SLOGALWAYS2 ("%s Version %s", MMSLITE_NAME, MMSLITE_VERSION);

	/* We want to know about connection activity				*/
	u_mvl_disc_ind_fun     = disc_ind_fun;

#if defined(HARD_CODED_CFG)
	num_loc_dib_entries = sizeof(localDibTable)/sizeof(DIB_ENTRY);
	loc_dib_table = localDibTable;
	num_rem_dib_entries = sizeof(remoteDibTable)/sizeof(DIB_ENTRY);
	rem_dib_table = remoteDibTable;
#if 0	/* Only call config functions necessary for stack being used.	*/
	tp4_config();		/* see tp4_hc.c		*/
	clnp_config();	/* see clnp_hc.c	*/
	adlcConfigure();	/* see adlc_hc.c	*/
#endif
	/* Fill in mvlCfg.	*/
	mvlCfg.num_calling = 4;
	mvlCfg.num_called = 0;
	mvlCfg.max_msg_size = 8000;
	strcpy (mvlCfg.local_ar_name, "local1");

#else /* #if defined(HARD_CODED_CFG) */

	/* Read the configuration from a file */
	ret = osicfgx ("cliosicfg.xml"/*"osicfg.xml"*/, &mvlCfg);	/* This fills in mvlCfg	*/
	if (ret != SD_SUCCESS)
	{
		LOGFAULT("Stack configuration failed, err = 0x%X. Check configuration.\n", ret );
		return false;
	}
	tp0_cfg.keepalive = SD_TRUE;
#endif /* HARD_CODED_CFG */

	/* Set the ^c catcher */
	signal (SIGINT, ctrlCfun);

	mvl_max_dyn.types = 100000;	/* CRITICAL: must be before mvl_init_mms_objs*/
	/* (called by mvl_start_acse)		*/
#if defined(GOOSE_RX_SUPP) || defined(GSSE_RX_SUPP) || defined(GSE_MGMT_RX_SUPP) || defined(SMPVAL_RX_SUPP)
	/* NOTE: clnp_init required for GSSE. It also calls clnp_snet_init	*/
	/*       which is required for GOOSE, GSE_MGMT, SMPVAL.		*/
	ret = clnp_init (1200);	/* big enough for any GSSE msg	*/
	if (ret)
	{
		LOGFAULT("clnp_init error 0x%X", ret);
		return false;
	}
#endif

	ret = mvl_start_acse (&mvlCfg);	/* MAKE SURE mvlCfg is filled in*/
	if (ret != SD_SUCCESS)
	{
		LOGFAULT("mvl_start_acse () failed, err = 0x%X.", ret );
		return false;
	}

	if ((ret = rpt_typeids_find (&rpt_typeids)) != SD_SUCCESS)
	{
		LOGFAULT("rpt_typeids_find () failed = 0x%X.", ret );
		return false;
	}
	g_pRPT_TYPEIDS = &rpt_typeids;
#if !defined(NO_KEYBOARD) && !defined(_WIN32) && !defined(__QNX__)
	term_init ();	/* makes getchar work right	*/
#endif
#if defined(USE_DIB_LIST)
	/* First copy addresses from "osicfg.xml" (stored in "loc_dib_table"	*/
	/* and "rem_dib_table" arrays) to the new DIB list.			*/
	/* NOTE: Client address is probably not in the SCL file, so it should	*/
	/*       come from here.						*/
	for (j = 0; j < num_loc_dib_entries; j++)
	{
		dib_entry_create (&loc_dib_table[j]);	/* add to new DIB list	*/
	}
	for (j = 0; j < num_rem_dib_entries; j++)
	{
		dib_entry_create (&rem_dib_table[j]);	/* add to new DIB list	*/
	}
#endif	/* USE_DIB_LIST	*/
	if(!g_bSingleThreadMode)
	{
		SKT_CREATE_THREAD(ThreadGlobalMmsMain,NULL);
		SKT_CREATE_THREAD(ThreadGlobalMmsTimer,NULL);
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  全局退出时的销毁函数
// 作    者:  邵凯田
// 创建时间:  2015-9-22 18:37
// 参数说明:  void
// 返 回 值:  true/false
//////////////////////////////////////////////////////////////////////////
bool SMmsClient::ExitGlobal()
{
	static bool bInit = false;
	if(bInit)
		return true;
	g_bMmsGlobalExit = true;
	bInit = true;
	/* Done with VMDs. Destroy them.	*/
	// 	if (test_mode == TEST_MODE_IEC_REMOTE && remote_vmd_ctrl != NULL)
	// 	{
	// 		mvl_vmd_destroy (remote_vmd_ctrl);	/* only created for this mode*/
	// 	}
	mvl_vmd_destroy (&mvl_vmd);

	/* If cleanup worked, number of types logged here should be the number*/
	/* of types created by Foundry (non-dynamic).				*/
	SLOGALWAYS1 ("DEBUG: Number of types in use after cleanup=%d", mvl_type_count ());

	dyn_mem_ptr_statistics (0);	
#if defined(GOOSE_RX_SUPP) || defined(GSSE_RX_SUPP) || defined(GSE_MGMT_RX_SUPP) || defined(SMPVAL_RX_SUPP)
	/* clnp_init called at startup, so must call clnp_end to clean up.*/
	clnp_end ();
#endif
	mvl_end_acse ();

	/* Free buffers allocated by "mvl_init_mms_objs".	*/
	mvl_free_mms_objs ();
	/* Free buffers allocated by "osicfgx".	*/
	osicfgx_unconfigure ();

#if defined(USE_DIB_LIST)
	/* Destroy addresses created at startup.	*/
	dib_entry_destroy_all ();
#endif

	/* This should log only a few buffers which are freed by slog_end and	*/
	/* logcfgx_unconfigure below.						*/
	dyn_mem_ptr_status ();	/* is everything freed up?	*/

#if defined(DEBUG_SISCO)
	/* terminate logging services and save current log file position */
	slog_end (sLogCtrl);

	/* Free buffers allocated by "logcfgx".				*/
	/* NOTE: this is AFTER slog_end, so it can't screw up logging.	*/
	logcfgx_unconfigure (sLogCtrl);
#endif

	printf ("\n\n");
#if !defined(NO_KEYBOARD) && !defined(_WIN32) && !defined(__QNX__)
	term_rest ();	/* Must be called before exit if term_init used.	*/
#endif
#if defined(MVL_GOOSE_SUPPORT)
	/* Destroy all subscribers.	*/
	if (goosehandle!=NULL && iecGooseUnSubscribe (goosehandle) !=SD_SUCCESS)
		printf("\niecGooseUnsubscribe failed");
#endif

	/* Should have 0 buffers allocated now.	*/
	if (chk_curr_mem_count != 0)
		printf ("Note: %lu buffers not freed.\n",
		(ST_ULONG) chk_curr_mem_count);

	SLog::quitLog();
	return true;
}


////////////////////////////////////////////////////////////////////////
// 描    述:  启动服务,虚函数，派生函数中必须先调用此函数
// 作    者:  邵凯田
// 创建时间:  2015-9-16 19:24
// 参数说明:  void
// 返 回 值:  true表示服务启动成功,false表示服务启动失败
//////////////////////////////////////////////////////////////////////////
extern ST_INT num_loc_dib_entries;
extern ST_INT num_rem_dib_entries;
extern DIB_ENTRY *loc_dib_table;
extern DIB_ENTRY *rem_dib_table;

bool SMmsClient::Start()
{
	if(!InitGlobal(g_bSingleThreadMode))
	{
		LOGFAULT("初始化MMS客户端时失败!");
		return false;
	}
	m_bQuit = false;
	if(g_bSingleThreadMode)
	{
		S_CREATE_THREAD(ThreadMain,this);
	}
	else
	{
		if(!g_SmmsClients.exist(this))
			g_SmmsClients.append(this);
	}
	return true;
}

////////////////////////////////////////////////////////////////////////
// 描    述:  停止服务，派生函数中必须先调用此函数
// 作    者:  邵凯田
// 创建时间:  2015-9-16 19:24
// 参数说明:  void
// 返 回 值:  true表示服务停止成功,false表示服务停止失败
//////////////////////////////////////////////////////////////////////////
bool SMmsClient::Stop()
{
	m_bQuit = true;
	if(!g_bSingleThreadMode)
	{
		g_SmmsClients.remove(this);
	}
	DisConnect();
	while(m_iThreads > 0)
	{
		SApi::UsSleep(1000);
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  取空闲的DIB条目
// 作    者:  邵凯田
// 创建时间:  2015-9-22 13:48
// 参数说明:  void
// 返 回 值:  DIB_ENTRY*
//////////////////////////////////////////////////////////////////////////
DIB_ENTRY* SMmsClient_GetFreeDibEntry()
{
	DIB_ENTRY *pDib = NULL;
	//g_SMms_Global_Lock.lock();
	for(int i = 0; i < num_rem_dib_entries; i++)
	{
		if(!g_SMms_Dib_Index_Used.exist(&rem_dib_table[i]))
		{
			pDib = &rem_dib_table[i];
			break;
		}		
	}
	if(pDib == NULL)
	{
		//g_SMms_Global_Lock.unlock();
		LOGFAULT("%d个客户端实例已经全部用完，无法开启新的通讯实例!",num_rem_dib_entries);
		return NULL;
	}
	g_SMms_Dib_Index_Used.append(pDib);
	//g_SMms_Global_Lock.unlock();
	return pDib;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  释放DIB条目为空闲条目
// 作    者:  邵凯田
// 创建时间:  2015-9-22 13:48
// 参数说明:  pDib为DIB_ENTRY指针
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void SMmsClient_ReleaseDibEntry(DIB_ENTRY *pDib)
{
	//g_SMms_Global_Lock.lock();
	g_SMms_Dib_Index_Used.remove(pDib);
	//g_SMms_Global_Lock.unlock();
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  连接指针地址、端口的MMS服务器
// 作    者:  邵凯田
// 创建时间:  2015-9-16 19:24
// 参数说明:  void
// 返 回 值:  true表示连接成功，false表示连接失败
//////////////////////////////////////////////////////////////////////////
bool SMmsClient::Connect()
{
	DIB_ENTRY *pDib = NULL;
	bool bConnOk = false;

	//连接A网
	if(m_pMVL_NET_INFO_A == NULL && m_sServerIpA[0] != '\0')
	{
		pDib = (DIB_ENTRY *)m_pDIB_ENTRY_A;
		if(pDib == NULL)
			m_pDIB_ENTRY_A = pDib = SMmsClient_GetFreeDibEntry();
		if(pDib == NULL)
		{
			LOGFAULT("没有空闲的DIB条目可用，请扩大cliosicfg.xml中的客户端数量!");
			return false;
		}
		pDib->pres_addr.netAddr.ip = tp0_convert_ip(m_sServerIpA);
		pDib->pres_addr.port = m_iPort;
		m_pMVL_NET_INFO_A = connectToServer("mms_client", pDib->name);
		if(m_pMVL_NET_INFO_A == NULL)
		{
			LOGWARN("MMS客户端连接A网 [%s] 失败!",m_sServerIpA);
			//return false;
		}
		else
		{
			bConnOk = true;
			stuSMmsClient_UserInfo *pUserInfo = (stuSMmsClient_UserInfo*)(((MVL_NET_INFO*)m_pMVL_NET_INFO_A)->user_info);
			if(pUserInfo == NULL)
			{
				pUserInfo = new stuSMmsClient_UserInfo();
				pUserInfo->pMmsClient = this;
				((MVL_NET_INFO*)m_pMVL_NET_INFO_A)->user_info = pUserInfo;
			}
			m_pUserInfo_A = pUserInfo;
			pUserInfo->pALL_RCB_INFO = new ALL_RCB_INFO;
			memset(pUserInfo->pALL_RCB_INFO,0,sizeof(ALL_RCB_INFO));
			LOGDEBUG("MMS客户端连接A网 [%s] 成功!",m_sServerIpA);
			OnConnected_A();
		}
	}

	//连接B网,如果配置了B网IP的话
	if(m_pMVL_NET_INFO_B == NULL && m_sServerIpB[0] != '\0')
	{
		pDib = (DIB_ENTRY *)m_pDIB_ENTRY_B;
		if(pDib == NULL)
			m_pDIB_ENTRY_B = pDib = SMmsClient_GetFreeDibEntry();
		if(pDib == NULL)
		{
			LOGFAULT("没有空闲的DIB条目可用，请扩大cliosicfg.xml中的客户端数量!");
			return false;
		}
		pDib->pres_addr.netAddr.ip = tp0_convert_ip(m_sServerIpB);
		pDib->pres_addr.port = m_iPort;
		m_pMVL_NET_INFO_B = connectToServer("mms_client", pDib->name);
		if(m_pMVL_NET_INFO_B == NULL)
		{
			LOGWARN("MMS客户端连接B网 [%s] 失败!",m_sServerIpB);
			return false;
		}
		else
		{
			bConnOk = true;
			stuSMmsClient_UserInfo *pUserInfo = (stuSMmsClient_UserInfo*)(((MVL_NET_INFO*)m_pMVL_NET_INFO_B)->user_info);
			if(pUserInfo == NULL)
			{
				pUserInfo = new stuSMmsClient_UserInfo();
				pUserInfo->pMmsClient = this;
				((MVL_NET_INFO*)m_pMVL_NET_INFO_B)->user_info = pUserInfo;
			}
			m_pUserInfo_B = pUserInfo;			
			pUserInfo->pALL_RCB_INFO = new ALL_RCB_INFO;
			memset(pUserInfo->pALL_RCB_INFO,0,sizeof(ALL_RCB_INFO));
			LOGDEBUG("MMS客户端连接B网 [%s] 成功!",m_sServerIpB);
			OnConnected_B();
		}
	}
	if(m_pMVL_NET_INFO_A == NULL && m_pMVL_NET_INFO_B == NULL)
		return false;

	// 连接成功后将不使能报告，待所有的客户连接都完成后再使能报告
// 	if(bConnOk && IsUserTaskProcessing() == false)
// 	{
// 		EnableReport();
// 	}

	OnConnected();
	
	return true;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  断开与服务端的连接
// 作    者:  邵凯田
// 创建时间:  2015-9-16 19:26
// 参数说明:  void
// 返 回 值:  true表示断开成功,false表示断开失败
//////////////////////////////////////////////////////////////////////////
bool SMmsClient::DisConnect()
{
	bool bDis = false;
	if(m_pMVL_NET_INFO_A == NULL && m_pMVL_NET_INFO_B == NULL)
		bDis = true;
	LOGDEBUG("断开MMS客户端连接!");
	DisableReport();
	DisConnect_A();
	DisConnect_B();

	if(bDis == true)
		return false;
	OnDisConnected();
	return true;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  断开A网的连接
// 作    者:  邵凯田
// 创建时间:  2015-9-22 14:21
// 参数说明:  void
// 返 回 值:  true表示断开成功,false表示断开失败
//////////////////////////////////////////////////////////////////////////
bool SMmsClient::DisConnect_A()
{
	int ret;
	bool isLock = m_Lock.trylock();
	if(m_pMVL_NET_INFO_A != NULL)
	{
		//A网已连接
		if(m_pMVL_NET_INFO_RptEn == m_pMVL_NET_INFO_A)
		{
			DisableReport(false);
		}
		((MVL_NET_INFO*)m_pMVL_NET_INFO_A)->user_info = NULL;
		ret = disconnectFromServer((MVL_NET_INFO*)m_pMVL_NET_INFO_A);
		if(ret != 0)
		{
			LOGWARN("释放MMS客户端A网连接会话失败! error=0x%04X\n", ret);
			//return false;
		}
		m_pMVL_NET_INFO_A = NULL;
	}
	if(m_pUserInfo_A != NULL)
	{
		stuSMmsClient_UserInfo *pInfo = (stuSMmsClient_UserInfo*)m_pUserInfo_A;
		m_pUserInfo_A = NULL;
		if(pInfo != NULL && pInfo->pALL_RCB_INFO != NULL)
		{
			ALL_RCB_INFO* all_rcb_info = (ALL_RCB_INFO *)pInfo->pALL_RCB_INFO;
			RCB_INFO* rcb_info;
			while ((rcb_info = (RCB_INFO *)list_get_first (&all_rcb_info->rcb_info_list)) != NULL)
				rcb_info_destroy (rcb_info);
		}
		if(pInfo->pALL_RCB_INFO != NULL)
		{
			delete pInfo->pALL_RCB_INFO;
			pInfo->pALL_RCB_INFO = NULL;
		}
		if(pInfo != NULL)
			delete pInfo;
	}
	if(m_pDIB_ENTRY_A != NULL)
	{
		SMmsClient_ReleaseDibEntry((DIB_ENTRY*)m_pDIB_ENTRY_A);
		m_pDIB_ENTRY_A = NULL;
	}
	LOGDEBUG("MMS客户端A网[%s]连接断开!",m_sServerIpA);
	OnDisConnected_A();
	if (isLock)
		unlock();
	return true;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  断开B网的连接
// 作    者:  邵凯田
// 创建时间:  2015-9-22 14:21
// 参数说明:  void
// 返 回 值:  true表示断开成功,false表示断开失败
//////////////////////////////////////////////////////////////////////////
bool SMmsClient::DisConnect_B()
{
	int ret;
	bool isLock = m_Lock.trylock();
	if(m_pMVL_NET_INFO_B != NULL)
	{
		//B网已连接
		if(m_pMVL_NET_INFO_RptEn == m_pMVL_NET_INFO_B)
		{
			DisableReport(false);
		}
		((MVL_NET_INFO*)m_pMVL_NET_INFO_B)->user_info = NULL;
		ret = disconnectFromServer((MVL_NET_INFO*)m_pMVL_NET_INFO_B);
		if(ret != 0)
		{
			LOGWARN("释放MMS客户端B网连接会话失败! error=0x%04X\n", ret);
			//return false;
		}
		m_pMVL_NET_INFO_B = NULL;
	}
	if(m_pUserInfo_B != NULL)
	{
		stuSMmsClient_UserInfo *pInfo = (stuSMmsClient_UserInfo*)m_pUserInfo_B;
		m_pUserInfo_B = NULL;
		if(pInfo != NULL && pInfo->pALL_RCB_INFO != NULL)
		{
			ALL_RCB_INFO* all_rcb_info = (ALL_RCB_INFO *)pInfo->pALL_RCB_INFO;
			RCB_INFO* rcb_info;
			while ((rcb_info = (RCB_INFO *)list_get_first (&all_rcb_info->rcb_info_list)) != NULL)
				rcb_info_destroy (rcb_info);
		}
		if(pInfo->pALL_RCB_INFO != NULL)
		{
			delete pInfo->pALL_RCB_INFO;
			pInfo->pALL_RCB_INFO = NULL;
		}
		if(pInfo != NULL)
			delete pInfo;
	}
	if(m_pDIB_ENTRY_B != NULL)
	{
		SMmsClient_ReleaseDibEntry((DIB_ENTRY*)m_pDIB_ENTRY_B);
		m_pDIB_ENTRY_B = NULL;
	}
	LOGDEBUG("MMS客户端B网[%s]连接断开!",m_sServerIpB);
	OnDisConnected_B();
	if (isLock)
		unlock();
	return true;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  MMS客户端主线程
// 作    者:  邵凯田
// 创建时间:  2015-9-16 19:31
// 参数说明:  this
// 返 回 值:  void*
//////////////////////////////////////////////////////////////////////////
void* SMmsClient::ThreadMain(void* lp)
{
	SMmsClient *pThis = (SMmsClient*)lp;
	pThis->m_iThreads ++;
	int rpt_soc = 0;;
	stuSMmsReportInfo *pRptInfo;
	unsigned long rpt_pos;
	while(!pThis->m_bQuit)
	{
		if(!(pThis->m_pMVL_NET_INFO_A != NULL && (pThis->m_pMVL_NET_INFO_B != NULL || pThis->m_sServerIpB[0] == '\0')))
		{
			if(!pThis->Connect())
			{
				SApi::UsSleep(1000000);
				continue;
			}

			// 如果有用户请求任务正在处理，优先处理，完成后再使能报告
			if (pThis->IsUserTaskProcessing())
			{
				SApi::UsSleep(1000000);
				continue;
			}
		}
		if(abs(::time(NULL)-rpt_soc) >= 3)
		{
			pRptInfo = pThis->m_ReportInfos.FetchFirst(rpt_pos);
			while(pRptInfo)
			{
				// 如果有用户请求任务正在处理，优先处理，完成后再使能报告
				if (pThis->IsUserTaskProcessing())
				{
					break;
				}

				if(pRptInfo->report_en_state != 1)
				{
					pThis->EnableReport();
					break;
				}
				pRptInfo = pThis->m_ReportInfos.FetchNext(rpt_pos);
			}
			rpt_soc = (int)::time(NULL);	//获取当前时间
		}
		//pThis->lock();
		doCommService();
		//pThis->unlock();
		SApi::UsSleep(50000);
	}
	pThis->DisConnect();
	pThis->m_iThreads --;
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  全局MMS客户端主线程，所有通信实例均使用同一个通信实例
// 作    者:  邵凯田
// 创建时间:  2017-5-26 10:17
// 参数说明:  
// 返 回 值:  
//////////////////////////////////////////////////////////////////////////
void* SMmsClient::ThreadGlobalMmsMain(void *lp)
{
	while(!g_bMmsGlobalExit)
	{
		//pThis->lock();
		doCommService();
		//pThis->unlock();
		SApi::UsSleep(1000);
	}
	return NULL;
}


//////////////////////////////////////////////////////////////////////////
// 描    述:  全局MMS客户端定时线程，处理所有设备的连接、重连、全能报告、定时触发等操作
// 作    者:  邵凯田
// 创建时间:  2017-5-26 10:26
// 参数说明:  NULL
// 返 回 值:  NULL
//////////////////////////////////////////////////////////////////////////
void* SMmsClient::ThreadGlobalMmsTimer(void *lp)
{
	SMmsClient *pThis;
	int rpt_soc = 0,i,rpt_soc0;
	stuSMmsReportInfo *pRptInfo;
	unsigned long pos=0,rpt_pos=0;
	while(!g_bMmsGlobalExit)
	{
		for(i=0;!g_bMmsGlobalExit && i<g_SmmsClients.count();i++)
		{
			pThis = g_SmmsClients[i];
			if(pThis == NULL)
				continue;
			if(pThis->m_pMVL_NET_INFO_A == NULL && (pThis->m_pMVL_NET_INFO_B == NULL || pThis->m_sServerIpB[0] == '\0'))
			{
				pThis->Connect();
			}
		}

		rpt_soc0 = 0;
		if(abs(((int)::time(NULL))-rpt_soc) >= 3)
		{
			for(i=0;!g_bMmsGlobalExit && i<g_SmmsClients.count();i++)
			{
				pThis = g_SmmsClients[i];
				if(pThis == NULL)
					continue;

				// 如果有用户请求任务正在处理，优先处理，完成后再使能报告
				if (pThis->IsUserTaskProcessing())
				{
					continue;
				}

				for(int j=0;!g_bMmsGlobalExit && j<pThis->m_ReportInfos.count();j++)
				{
					pRptInfo = pThis->m_ReportInfos[j];
					if(pRptInfo == NULL)
						continue;
							
					if (pThis->IsUserTaskProcessing())
						break;

					if(pRptInfo->report_en_state != 1)
					{
						if(rpt_soc0 == 0)
							rpt_soc0 = (int)::time(NULL);
						pThis->EnableReport();
						break;
					}
				}
			}
		}
		if(rpt_soc0 != 0)
			rpt_soc = rpt_soc0;		
		SApi::UsSleep(1000000);
	}
	return NULL;
}


//////////////////////////////////////////////////////////////////////////
// 描    述:  创建新的报告信息,并添加到报告队列中
// 作    者:  邵凯田
// 创建时间:  2015-9-24 8:24
// 参数说明:  @dom_name表示IED+LD名称
//         :  @rcb_name表示报告控制块名称
//         :  @report_id表示报告序号，从1开始编号，0表示单实例报告（不需要报告号）
//         :  @IntgPd表示完整性周期，单位ms
//         :  @TrgOpt表示触发选项
// 返 回 值:  stuSMmsReportInfo*
//////////////////////////////////////////////////////////////////////////
stuSMmsReportInfo* SMmsClient::AddReportInfo(SString dom_name,SString rptID,SString buffered,
	SString rcb_name,int report_id,SString entry_id,int IntgPd/*=5000*/,unsigned char TrgOpt/*=0xFF*/)
{
	stuSMmsReportInfo* pRpt = new stuSMmsReportInfo();
	pRpt->dom_name = dom_name;
	pRpt->rptID = rptID;
	pRpt->buffered = buffered;
	if(report_id > 0)
		pRpt->rcb_name.sprintf("%s%02d",rcb_name.data(),report_id);
	else
		pRpt->rcb_name = rcb_name;
	pRpt->report_id = report_id;
	pRpt->IntgPd = IntgPd;
	pRpt->TrgOps[0] = TrgOpt;
	pRpt->OptFlds[0] = 0;
	pRpt->OptFlds[1] = 0;
	if (entry_id.size() == 16)
	{
		SString::StrToHex(entry_id,pRpt->EntryID);
	}
	else
	{
		for (int i = 0; i < 8; i++)	
			pRpt->EntryID[i] = 0;
	}
	pRpt->pRCB_INFO = NULL;
	pRpt->report_en_state = 0;

	/* These options valid for IEC BRCB, IEC URCB, or UCA.	*/
	BSTR_BIT_SET_ON(pRpt->OptFlds, OPTFLD_BITNUM_SQNUM);
	BSTR_BIT_SET_ON(pRpt->OptFlds, OPTFLD_BITNUM_TIMESTAMP);
	BSTR_BIT_SET_ON(pRpt->OptFlds, OPTFLD_BITNUM_REASON);
	BSTR_BIT_SET_ON(pRpt->OptFlds, OPTFLD_BITNUM_DATSETNAME);
	/* NOTE: these options ONLY available for IEC-61850 (not UCA)
	* SUBSEQNUM is only set by the server, so don't try to set it.
	*/
	m_ReportInfos.append(pRpt);
	return pRpt;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  删除报告控制块，并从报告列表删除
// 作    者:  邵凯田
// 创建时间:  2015-9-24 9:00
// 参数说明:  @pRptInfo表示报告指针
// 返 回 值:  true/false
//////////////////////////////////////////////////////////////////////////
bool SMmsClient::RemoveReportInfo(stuSMmsReportInfo *pRpt)
{
	ST_RET ret;
	if(m_ReportInfos.exist(pRpt) == false)
		return false;
	lock();
	MVL_NET_INFO* pNetInfo = (MVL_NET_INFO*)m_pMVL_NET_INFO_RptEn;
	ALL_RCB_INFO *all_rcb_info = ((stuSMmsClient_UserInfo *)pNetInfo->user_info)->pALL_RCB_INFO;
	if(pNetInfo == NULL || all_rcb_info == NULL)
	{
		m_ReportInfos.remove(pRpt);
		unlock();
		LOGWARN("DisableReport失败!尚未连接报告!");
		return false;
	}
	RCB_INFO *rcb_info = (RCB_INFO*)pRpt->pRCB_INFO;
	if(pRpt->report_en_state == 1 || rcb_info != NULL)
	{
		ret = rcb_disable(pNetInfo,pRpt->dom_name.data(),pRpt->rcb_name.data(),g_pRPT_TYPEIDS,10);
		list_unlink (&all_rcb_info->rcb_info_list, rcb_info);
		rcb_info_destroy (rcb_info);
		pRpt->pRCB_INFO = NULL;
	}
	m_ReportInfos.remove(pRpt);
	unlock();
	return true;
}


//////////////////////////////////////////////////////////////////////////
// 描    述:  使能所有报告
// 作    者:  邵凯田
// 创建时间:  2015-9-23 16:38
// 参数说明:  @iTimeout_s表示超时的秒数
// 返 回 值:  <=0表示使能失败，>0表示使能成功的数量，=GetReportInfoCount()表示全部使能成功
//////////////////////////////////////////////////////////////////////////
int SMmsClient::EnableReport(int iTimeout_s/*=10*/)
{
	ST_RET ret;
	RCB_INFO *rcb_info;		/* UCA/IEC Report Control Block info	*/
	ALL_RCB_INFO *all_rcb_info;
	unsigned long pos;
	m_isSystemTaskProcessing = true;
	g_SMms_Global_Lock.lock();
	MVL_NET_INFO *pNetInfo = (MVL_NET_INFO*)m_pMVL_NET_INFO_A;
	if(pNetInfo == NULL)
		pNetInfo = (MVL_NET_INFO*)m_pMVL_NET_INFO_B;
	m_pMVL_NET_INFO_RptEn = pNetInfo;
	if(pNetInfo == NULL)
	{
		g_SMms_Global_Lock.unlock();
		m_isSystemTaskProcessing = false;
		return -1;
	}
	int cnt = 0;
	stuSMmsReportInfo *pRpt = m_ReportInfos.FetchFirst(pos);
	while(pRpt)
	{
		if (IsUserTaskProcessing() == true)
		{
			LOGDEBUG("有用户操作,优先响应用户操作命令,稍后再执行使能");
			break;
		}
		if(pRpt->report_en_state == 1)
		{
			pRpt = m_ReportInfos.FetchNext(pos);
			continue;
		}
		if (!(stuSMmsClient_UserInfo *)pNetInfo->user_info)
		{
			pRpt = m_ReportInfos.FetchNext(pos);
			continue;
		}
		all_rcb_info = ((stuSMmsClient_UserInfo *)pNetInfo->user_info)->pALL_RCB_INFO;
		if (!all_rcb_info)
		{
			pRpt = m_ReportInfos.FetchNext(pos);
			continue;
		}
		//enable report control
		rcb_info = (RCB_INFO *)pRpt->pRCB_INFO;
		if(rcb_info != NULL)
			rcb_info_destroy(rcb_info);
		rcb_info = rcb_info_create(pNetInfo, pRpt->dom_name.data(), pRpt->rcb_name.data(), g_pRPT_TYPEIDS, iTimeout_s);
		if(rcb_info != NULL)
		{
			/* Add RCB to list before enabling, so we're ready to receive RPTS immediately.*/
			if (rcb_info->RptID[0] == 0)
				sprintf(rcb_info->RptID,"%s/%s",pRpt->dom_name.data(),pRpt->rcb_name.data());

			list_add_last (&all_rcb_info->rcb_info_list, rcb_info);
			printf ("rcb_info->numDsVar = %d\n", rcb_info->numDsVar);
			if (rcb_info->rcb_type == RCB_TYPE_IEC_BRCB)
			{	/* These only valid for IEC BRCB	*/
				BSTR_BIT_SET_ON(pRpt->OptFlds, OPTFLD_BITNUM_BUFOVFL);
				BSTR_BIT_SET_ON(pRpt->OptFlds, OPTFLD_BITNUM_ENTRYID);
				if (pRpt->buffered == "true" && SString::HexToStr(pRpt->EntryID,8,false) != "0000000000000000")
				{
					SString varName = pRpt->rcb_name + "$EntryID";
					ST_RET ret = named_var_write((MVL_NET_INFO*)m_pMVL_NET_INFO_RptEn,varName.data(),DOM_SPEC,pRpt->dom_name.data(), 
						g_pRPT_TYPEIDS->ostring8,(ST_CHAR *)pRpt->EntryID,iTimeout_s);
					if (ret != 0)
					{
						LOGWARN("设置[%s,%s]EntryID[%s]失败。",pRpt->dom_name.data(),varName.data(),SString::HexToStr(pRpt->EntryID,8).data());
					}
					else
					{
						LOGDEBUG("设置[%s,%s]EntryID[%s]成功。",pRpt->dom_name.data(),varName.data(),SString::HexToStr(pRpt->EntryID,8).data());
					}
				}
			}
			if (rcb_info->rcb_type != RCB_TYPE_UCA)
			{	/* These only valid for IEC BRCB or IEC URCB	*/
				BSTR_BIT_SET_ON(pRpt->OptFlds, OPTFLD_BITNUM_DATAREF);
				BSTR_BIT_SET_ON(pRpt->OptFlds, OPTFLD_BITNUM_CONFREV);
			}

			if (IsUserTaskProcessing() == true)
			{
				LOGDEBUG("有用户操作,优先响应用户操作命令,稍后再执行使能");
				break;
			}

			ret = rcb_enable (pNetInfo, pRpt->dom_name.data(), pRpt->rcb_name.data(), pRpt->OptFlds, pRpt->TrgOps,
				pRpt->IntgPd,		/* Integrity Period (ms) 5000*/
				g_pRPT_TYPEIDS,
				iTimeout_s);		/* timeout (s) 10*/
			if (ret != 0)
			{
				LOGWARN("报告使能(%s,%s)失败!\n",pRpt->dom_name.data(), pRpt->rcb_name.data());
				/* Remove this RCB from list and destroy it.	*/
				list_unlink (&all_rcb_info->rcb_info_list, rcb_info);
				rcb_info_destroy (rcb_info);
				rcb_info = NULL;
				pRpt->report_en_state = 2;
				pRpt->report_en_error = "rcb_enable error";
			}
			else
			{
				LOGDEBUG("报告使能(%s,%s)成功!\n",pRpt->dom_name.data(), pRpt->rcb_name.data());
				cnt++;
				pRpt->pRCB_INFO = rcb_info;
				pRpt->report_en_state = 1;
			}

			stuSMmsReportInfo *pNextRpt = m_ReportInfos.FetchNext(pos);
			OnEnabledRpt(pRpt, ret, pNextRpt == NULL ? true : false);
			pRpt = pNextRpt;
			continue;
		}
		else
		{
			pRpt->report_en_state = 2;
			pRpt->report_en_error = "rcb_info_create error";
			LOGWARN("报告创建(%s,%s)失败!\n",pRpt->dom_name.data(), pRpt->rcb_name.data());
		}

		pRpt = m_ReportInfos.FetchNext(pos);
	}
	g_SMms_Global_Lock.unlock();
	m_isSystemTaskProcessing = false;
	return cnt;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  关闭所有报告
// 作    者:  邵凯田
// 创建时间:  2015-9-23 16:39
// 参数说明:  void
// 返 回 值:  <=0表示关闭失败，>0表示有报告未关闭，=0表示关闭所有报告成功
//////////////////////////////////////////////////////////////////////////
int SMmsClient::DisableReport(bool bNeedLock)
{
	unsigned long pos;
	ALL_RCB_INFO *all_rcb_info;
	RCB_INFO* rcb_info;
	ST_RET ret;
	if(bNeedLock)
		g_SMms_Global_Lock.lock();

	MVL_NET_INFO* pNetInfo = (MVL_NET_INFO*)m_pMVL_NET_INFO_RptEn;
	if(pNetInfo == NULL)
	{
		if(bNeedLock)
			g_SMms_Global_Lock.unlock();
		LOGWARN("DisableReport失败!尚未连接报告!");
		return -1;
	}

	if((stuSMmsClient_UserInfo *)pNetInfo->user_info == NULL)
	{
		if(bNeedLock)
			g_SMms_Global_Lock.unlock();
		m_pMVL_NET_INFO_RptEn = NULL;
		return m_ReportInfos.count();
	}
	
	stuSMmsReportInfo *pRpt = m_ReportInfos.FetchFirst(pos);
	while(pRpt)
	{
		//disable report control
		ret = rcb_disable(pNetInfo,pRpt->dom_name.data(),pRpt->rcb_name.data(),g_pRPT_TYPEIDS,10);
		pRpt->report_en_state = 0;
		pRpt->report_en_error = "";
		pRpt->pRCB_INFO = NULL;
		pRpt = m_ReportInfos.FetchNext(pos);
	}
	all_rcb_info = ((stuSMmsClient_UserInfo *)pNetInfo->user_info)->pALL_RCB_INFO;
	if(all_rcb_info != NULL)
	{
		while ((rcb_info = (RCB_INFO *)list_get_first(&all_rcb_info->rcb_info_list)) != NULL)
			rcb_info_destroy (rcb_info);
		memset(all_rcb_info,0,sizeof(ALL_RCB_INFO));
	}
	if(bNeedLock)
		g_SMms_Global_Lock.unlock();
	m_pMVL_NET_INFO_RptEn = NULL;
	return m_ReportInfos.count();
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  将指定MVL变量转换为字符串并返回
// 作    者:  邵凯田
// 创建时间:  2015-9-24 17:02
// 参数说明:  @pVal为MVL_VAR_ASSOC*
// 返 回 值:  SString
//////////////////////////////////////////////////////////////////////////
SString SMmsClient::GetMvlVarText(void *pVal)
{
	ST_CHAR tdl_buf [500];	/* increase size if complex TDL expected*/
	MVL_TYPE_CTRL *type_ctrl;
	ST_CHAR *data_text;		/* var data converted to text	*/
	ST_CHAR text_buf [30000];	/* increase size if ms_local_to_text fails	*/
	MVL_VAR_ASSOC* var = (MVL_VAR_ASSOC*)pVal;
	if(var == NULL)
		return "";
	type_ctrl = mvl_type_ctrl_find (var->type_id);
	if (type_ctrl)
	{
		/* If the TDL produced is longer than max_tdl_len, this function	*/
		/* "gracefully" fails (i.e. returns 0).					*/
		if (ms_runtime_to_tdl (type_ctrl->rt, type_ctrl->num_rt, tdl_buf, sizeof(tdl_buf))>0)
			;//printf ("  TYPE: %s\n", tdl_buf);
		else
			printf ("  TYPE: unknown\n");

		data_text = ms_local_to_text ((char*)var->data,
			type_ctrl->rt,
			type_ctrl->num_rt, text_buf, sizeof (text_buf));
		if (data_text)
		{
			;//printf ("  DATA: %s\n", data_text);
			return data_text;
		}
		else
			printf ("  DATA: cannot be converted to text\n");
	}
	else
		printf ("  ERR: type_id is invalid\n");
	return "";
}

int parseData(ST_CHAR *datptr,SD_CONST RUNTIME_TYPE *rt_head,ST_INT rt_num,SPtrList<MMSValue> &valueList,char *tdl_buf);
/********************************************  
* @brief 将指定MVL变量转换为值类队列并返回
* @author 宋康
* @date 2015/12/25
* @param void * pVal MVL_VAR_ASSOC*变量
* @param SPtrList<MMSValue> & valueList 值类转换队列
* @return int =0表示转换成功，>0或<0表示转换失败
*********************************************/ 
int SMmsClient::GetMvlVarValueList(void *pVal,SPtrList<MMSValue> &valueList)
{
	ST_CHAR tdl_buf [500];	/* increase size if complex TDL expected*/
	MVL_TYPE_CTRL *type_ctrl;
	MVL_VAR_ASSOC* var = (MVL_VAR_ASSOC*)pVal;
	if(var == NULL)
		return 0;
	type_ctrl = mvl_type_ctrl_find (var->type_id);
	if (type_ctrl)
	{
		/* If the TDL produced is longer than max_tdl_len, this function	*/
		/* "gracefully" fails (i.e. returns 0).					*/
		if (ms_runtime_to_tdl (type_ctrl->rt, type_ctrl->num_rt, tdl_buf, sizeof(tdl_buf))>0)
			;//printf ("  TYPE: %s\n", tdl_buf);
		else
			printf ("  TYPE: unknown\n");

		int ret = parseData((char*)var->data,type_ctrl->rt,type_ctrl->num_rt,valueList,tdl_buf);
		if (!ret)
			return ret;
	}
	else
		printf ("  ERR: type_id is invalid\n");
	return -1;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  取MVL目录内容
// 作    者:  邵凯田
// 创建时间:  2015-9-24 18:43
// 参数说明:  @sPath为目录名称
//         :  @slFiles这目录内容
//         :  @bNeedToSort表示是否需要排序，true排序，false不排序
// 返 回 值:  >0表示内容数量，<0表示失败, =0表示内容为空
//////////////////////////////////////////////////////////////////////////
int SMmsClient::GetMvlDirectory(SString sPath,SPtrList<stuSMmsFileInfo> &slFiles,bool bNeedToSort/*=false*/)
{
	char mca_name[MAX_FILE_NAME+1] = {0};
	ST_RET ret;
	int i = 0;
	int j = 0;
	int mTmpCount = 0;
	ST_BOOLEAN more_follows = SD_FALSE;
	MVL_REQ_PEND *reqCtrl = NULL;
	MVL_FDIR_RESP_INFO *fdir_resp = NULL;
	MVL_DIR_ENT *pDirEnt;
	stuSMmsFileInfo *pFile,*pMin;
	unsigned long pos;
	slFiles.clear();
	slFiles.setAutoDelete(true);

	m_isUserTaskProcessing = true;
	if (!WaitSystemTaskFinish())
	{
		m_isUserTaskProcessing = false;
		return false;
	}

	lock();
	m_isUserTaskProcessing = true;
	MVL_NET_INFO *pNetInfo = (MVL_NET_INFO*)m_pMVL_NET_INFO_A;
	if(pNetInfo == NULL)
		pNetInfo = (MVL_NET_INFO*)m_pMVL_NET_INFO_B;
	if(pNetInfo == NULL)
	{
		m_isUserTaskProcessing = false;
		unlock();
		return -1;
	}
	//如果超过1000还有剩余数据，则不处理
	for (i=0; i<1000; ++i)
	{
		ret = mvla_fdir(pNetInfo,sPath.data(),
			i == 0 ? NULL : mca_name,       // 第一次取波形列表时传输NULL,第二次执行时取上次最后一条记录
			&reqCtrl);

		if (ret == SD_SUCCESS)
		{
			ret = waitReqDone (reqCtrl, timeOut);
		}

		if (ret != SD_SUCCESS)
		{
			m_isUserTaskProcessing = false;
			mvl_free_req_ctrl (reqCtrl);	// CRITICAL:
			unlock();
			return -2;
		}

		fdir_resp = reqCtrl->u.fdir.resp_info;
		if (fdir_resp->num_dir_ent <= 0)
		{
			mvl_free_req_ctrl (reqCtrl);
			break;
		}
		if(bNeedToSort)
		{
			for (j=0; j<fdir_resp->num_dir_ent; ++j)
			{			
				pFile = new stuSMmsFileInfo();
				pDirEnt = &fdir_resp->dir_ent[j];
				pFile->filename = pDirEnt->filename;
				pFile->fsize	= pDirEnt->fsize;
				pFile->mtimpres = (pDirEnt->mtimpres != SD_FALSE);
				pFile->mtime	= pDirEnt->mtime;
				pMin = slFiles.FetchFirst(pos);
				while(pMin)
				{
					if(pMin->filename >= pFile->filename)
						break;
					pMin = slFiles.FetchNext(pos);
				}
				slFiles.insertBefore(pMin,pFile);
			}
		}
		else
		{
			for (j=0; j<fdir_resp->num_dir_ent; ++j)
			{			
				pFile = new stuSMmsFileInfo();
				pDirEnt = &fdir_resp->dir_ent[j];
				pFile->filename = pDirEnt->filename;
				pFile->fsize	= pDirEnt->fsize;
				pFile->mtimpres = (pDirEnt->mtimpres != SD_FALSE);
				pFile->mtime	= pDirEnt->mtime;
				slFiles.append(pFile);
			}
		}

		// 保存最后一个文件名,用户morefollow
		strcpy(mca_name, fdir_resp->dir_ent[fdir_resp->num_dir_ent-1].filename);
		more_follows = fdir_resp->more_follows;
		mvl_free_req_ctrl (reqCtrl);
		if(more_follows == SD_FALSE)
			break;
	}
	m_isUserTaskProcessing = false;
	unlock();
	return slFiles.count();
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  下载MMS服务端指定的文件到文件系统
// 作    者:  邵凯田
// 创建时间:  2015-9-24 18:47
// 参数说明:  @sPathFile表示服务端的源文件名
//         :  @sLocalFile表示目前文件名
// 返 回 值:  true表示成功,false表示失败
//////////////////////////////////////////////////////////////////////////
bool SMmsClient::DownMvlFile(SString sPathFile, SString sLocalFile)
{
	FREAD_REQ_INFO   fread_req_info;
	FCLOSE_REQ_INFO  fclose_req_info;
	ST_RET ret;
	FILE *loc_fp;
	MVL_REQ_PEND *reqCtrl;
	ST_INT32 frsmid;
	ST_BOOLEAN more_follows;

	m_isUserTaskProcessing = true;
	if (!WaitSystemTaskFinish())
	{
		m_isUserTaskProcessing = false;
		return false;
	}

	lock();
	m_isUserTaskProcessing = true;
	MVL_NET_INFO *pNetInfo = (MVL_NET_INFO*)m_pMVL_NET_INFO_A;
	if(pNetInfo == NULL)
		pNetInfo = (MVL_NET_INFO*)m_pMVL_NET_INFO_B;
	if(pNetInfo == NULL)
	{
		m_isUserTaskProcessing = false;
		unlock();
		return false;
	}
	ret = mvla_fopen (pNetInfo,
		sPathFile.data(),
		0,		/* init_pos: start at beginning of file	*/
		&reqCtrl);
	if (ret == SD_SUCCESS)
		ret = waitReqDone (reqCtrl, timeOut);

	if (ret == SD_SUCCESS)
		frsmid = reqCtrl->u.fopen.resp_info->frsmid;	/* save frsmid	*/
	mvl_free_req_ctrl (reqCtrl);
	if (ret != SD_SUCCESS)
	{
		m_isUserTaskProcessing = false;
		unlock();
		return false;
	}

	loc_fp = fopen (sLocalFile.data(), "wb");
	if (loc_fp == NULL)
	{
		m_isUserTaskProcessing = false;
		unlock();
		return false;	/* Can't open local file.	*/
	}

	fread_req_info.frsmid = frsmid;	/* "fread" request doesn't chg.	*/
	do
	{
		ret = mvla_fread (pNetInfo, &fread_req_info, &reqCtrl);
		if (ret == SD_SUCCESS)
			ret = waitReqDone (reqCtrl, timeOut);
		if (ret == SD_SUCCESS)
		{
			more_follows = reqCtrl->u.fread.resp_info->more_follows;
			fwrite (reqCtrl->u.fread.resp_info->filedata, 1,
				reqCtrl->u.fread.resp_info->fd_len, loc_fp);
		}
		mvl_free_req_ctrl (reqCtrl);
		if (ret != SD_SUCCESS)
		{
			m_isUserTaskProcessing = false;
			unlock();
			return false;
		}
	}while (more_follows);

	fclose_req_info.frsmid = frsmid;
	ret = mvla_fclose (pNetInfo, &fclose_req_info, &reqCtrl);
	if (ret == SD_SUCCESS)
		ret = waitReqDone (reqCtrl, timeOut);
	if (ret != SD_SUCCESS)
	{
		LOGWARN("mvl_fclose failed, ret = 0x%X", ret);
	}
	mvl_free_req_ctrl (reqCtrl);

	fclose (loc_fp);
	m_isUserTaskProcessing = false;
	unlock();
	return true;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  下载MMS服务端指定的文件到内存
// 作    者:  邵凯田
// 创建时间:  2015-9-24 18:48
// 参数说明:  @sPathFile表示服务端的源文件名
//         :  @pBuffer表示目标缓冲区，当输入为空时将由内部申请空间并引用返回，调用者负责delete释放
//         :  @iLen表示目标缓冲区长度，当pBuffer非空时表示缓冲区长度，返回时会被置为目标内容的字节数
// 返 回 值:  true表示成功,false表示失败
//////////////////////////////////////////////////////////////////////////
bool SMmsClient::DownMvlFileToMem(SString sPathFile, BYTE* &pBuffer, int &iLen)
{
	FREAD_REQ_INFO   fread_req_info;
	FCLOSE_REQ_INFO  fclose_req_info;
	ST_RET ret;
	MVL_REQ_PEND *reqCtrl;
	ST_INT32 frsmid;
	ST_BOOLEAN more_follows;
	int iBufMaxSize = iLen;

	m_isUserTaskProcessing = true;
	if (!WaitSystemTaskFinish())
	{
		m_isUserTaskProcessing = false;
		return false;
	}

	if(iBufMaxSize <= 0)
	{
		if(pBuffer != NULL)
			delete pBuffer;
		iBufMaxSize = 64*1024;
		pBuffer = new BYTE[iBufMaxSize];
	}
	iLen = 0;
	lock();
	m_isUserTaskProcessing = true;
	MVL_NET_INFO *pNetInfo = (MVL_NET_INFO*)m_pMVL_NET_INFO_A;
	if(pNetInfo == NULL)
		pNetInfo = (MVL_NET_INFO*)m_pMVL_NET_INFO_B;
	if(pNetInfo == NULL)
	{
		m_isUserTaskProcessing = false;
		unlock();
		return false;
	}
	ret = mvla_fopen (pNetInfo,
		sPathFile.data(),
		0,		/* init_pos: start at beginning of file	*/
		&reqCtrl);
	if (ret == SD_SUCCESS)
		ret = waitReqDone (reqCtrl, timeOut);

	if (ret == SD_SUCCESS)
		frsmid = reqCtrl->u.fopen.resp_info->frsmid;	/* save frsmid	*/
	mvl_free_req_ctrl (reqCtrl);
	if (ret != SD_SUCCESS)
	{
		m_isUserTaskProcessing = false;
		unlock();
		return false;
	}

	fread_req_info.frsmid = frsmid;	/* "fread" request doesn't chg.	*/
	do
	{
		ret = mvla_fread (pNetInfo, &fread_req_info, &reqCtrl);
		if (ret == SD_SUCCESS)
			ret = waitReqDone (reqCtrl, timeOut);
		if (ret == SD_SUCCESS)
		{
			more_follows = reqCtrl->u.fread.resp_info->more_follows;
			memcpy(pBuffer+iLen,reqCtrl->u.fread.resp_info->filedata,reqCtrl->u.fread.resp_info->fd_len);
			iLen += reqCtrl->u.fread.resp_info->fd_len;
		}
		mvl_free_req_ctrl (reqCtrl);
		if (ret != SD_SUCCESS)
		{
			m_isUserTaskProcessing = false;
			unlock();
			return false;
		}
	}while (more_follows);

	fclose_req_info.frsmid = frsmid;
	ret = mvla_fclose (pNetInfo, &fclose_req_info, &reqCtrl);
	if (ret == SD_SUCCESS)
		ret = waitReqDone (reqCtrl, timeOut);
	if (ret != SD_SUCCESS)
	{
		LOGWARN("mvl_fclose failed, ret = 0x%X", ret);
	}
	mvl_free_req_ctrl (reqCtrl);

	m_isUserTaskProcessing = false;
	unlock();
	return true;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  处理突发报告的回调函数，在SMmsClient.cpp中实现
// 作    者:  邵凯田
// 创建时间:  2015-9-24 14:18
// 参数说明:  
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
extern "C" ST_VOID SMms_iec_rpt_ind_data (void *pUserInfo, MVL_VAR_ASSOC **info_va,
	ST_UINT8 *OptFldsData,	/* ptr to data part of OptFlds bvstring	*/
	ST_UINT8 *InclusionData,	/* ptr to Inclusion bstring	*/
	RCB_INFO *rcb_info,
	ST_INT va_total)
{
	ST_INT va_num=0;
	ST_INT j;
	stuSMmsClient_UserInfo *pSMmsUserInfo = (stuSMmsClient_UserInfo*)pUserInfo;
	SMmsClient *pThis = pSMmsUserInfo->pMmsClient;
	if(pThis == NULL)
	{
		LOGERROR("SMms_iec_rpt_ind_data pThis is NULL!");
		return;
	}
	pSMmsUserInfo->pMmsClient->onRecvNewPacket(SDateTime::currentDateTime());
	stuSMmsReport *mms_rpt = new stuSMmsReport();
	mms_rpt->sRptID = rcb_info->RptID;
	mms_rpt->RptID = rcb_info->rcb_var.RptID;
	mms_rpt->OptFlds = rcb_info->rcb_var.OptFlds;
	if (BSTR_BIT_GET(OptFldsData, OPTFLD_BITNUM_SQNUM))
		mms_rpt->SqNum = rcb_info->rcb_var.SqNum;
	if (BSTR_BIT_GET(OptFldsData, OPTFLD_BITNUM_TIMESTAMP))
		mms_rpt->TimeOfEntry = rcb_info->rcb_var.TimeOfEntry;
	if (BSTR_BIT_GET(OptFldsData, OPTFLD_BITNUM_DATSETNAME))
		mms_rpt->DatSetNa = rcb_info->rcb_var.DatSetNa;

	/* The following optional vars are supported by IEC-61850 but they are
	* NOT supported by UCA. This client must NOT set these OptFlds bits
	* when connected to a UCA server, and these variables will NOT be
	* included in a report received from a UCA server.
	*/

	if (BSTR_BIT_GET (OptFldsData, OPTFLD_BITNUM_BUFOVFL))
		mms_rpt->BufOvfl = rcb_info->rcb_var.BufOvfl;

	if (BSTR_BIT_GET(OptFldsData, OPTFLD_BITNUM_ENTRYID))
		mms_rpt->EntryID = rcb_info->rcb_var.EntryID;

	if (BSTR_BIT_GET(OptFldsData, OPTFLD_BITNUM_CONFREV))
		mms_rpt->ConfRev = rcb_info->rcb_var.ConfRev;

	if (BSTR_BIT_GET (OptFldsData,OPTFLD_BITNUM_SUBSEQNUM))
	{
		mms_rpt->SubSeqNum = rcb_info->rcb_var.SubSeqNum;
		mms_rpt->MoreSegmentsFollow = rcb_info->rcb_var.MoreSegmentsFollow;
	}

	mms_rpt->Inclusion = rcb_info->rcb_var.Inclusion;

	//当前数据集中的所有数据内容
	for (j = 0; j < rcb_info->numDsVar; ++j)
	{
		if (BSTR_BIT_GET (InclusionData, j))
		{
			mms_rpt->AddItem(BSTR_BIT_GET(OptFldsData, OPTFLD_BITNUM_DATAREF)?rcb_info->rcb_var.dataRefName[j]:NULL,
				rcb_info->rcb_var.dataValue[j],
				BSTR_BIT_GET(OptFldsData, OPTFLD_BITNUM_REASON)?rcb_info->rcb_var.Reason[j]:NULL);
		}
	}  
	pThis->OnMmsReportProcess(mms_rpt);
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  添加新的数据集
// 作    者:  邵凯田
// 创建时间:  2015-10-9 17:42
// 参数说明:  @sDsPath为数据集全路径
//         :  @sDsName为数据集名称
//         :  @iDsType为数据集类型（参见CSMmsDataset::m_iDsType定义)
// 返 回 值:  CSMmsDataset*
//////////////////////////////////////////////////////////////////////////
CSMmsDataset* SMmsClient::AddDataset(SString sIedPath,SString sDsPath,SString sDsName,int iDsType)
{
	CSMmsDataset* p = new CSMmsDataset();
	p->m_sIedName = sIedPath;
	p->m_sDsPath = sDsPath;
	p->m_sDsName = sDsName;
	p->m_iDsType = iDsType;
	m_Datasets.append(p);
	return p;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  删除指定的数据集
// 作    者:  邵凯田
// 创建时间:  2015-10-9 17:45
// 参数说明:  @sDsPath为数据集全路径名
// 返 回 值:  true/false
//////////////////////////////////////////////////////////////////////////
bool SMmsClient::RemoveDataset(SString sDsPath)
{
	unsigned long pos;
	CSMmsDataset *p = m_Datasets.FetchFirst(pos);
	while(p)
	{
		if(p->m_sDsPath == sDsPath)
		{
			m_Datasets.remove(p);
			return true;
		}
		p = m_Datasets.FetchNext(pos);
	}
	return false;
}

CSMmsLogicDevice* SMmsClient::AddLogicDevice(SString sLdPath,SString sLdName)
{
	CSMmsLogicDevice* p = new CSMmsLogicDevice(this);
	p->m_sLdPath = sLdPath;
	p->m_sLdName = sLdName;
	m_LogicDevices.append(p);
	return p;
}

/********************************************  
* @brief 总召唤所有报告控制块数据
* @author 宋康
* @date 2015/12/28
* @return void
*********************************************/ 
void SMmsClient::ProcessGi(int iTimeout_s/*=10*/)
{
	for (int i = 0; i < m_ReportInfos.count(); i++)
	{
		stuSMmsReportInfo * rptInfo = m_ReportInfos.at(i);
		if (rptInfo->report_en_state == 1 && (rptInfo->TrgOps[0] & C_SMMS_TRGOPT_ZZ))
		{
			ProcessReportInfoGi(rptInfo,iTimeout_s);
		}
	}
}

/********************************************  
* @brief 总召唤指定报告控制块数据
* @author 宋康
* @date 2015/12/28
* @param SString rcbName 报告控制块名称
* @return void
*********************************************/
void SMmsClient::ProcessGi(SString rcbName,int iTimeout_s/*=10*/)
{
	for (int i = 0; i < m_ReportInfos.count(); i++)
	{
		stuSMmsReportInfo * rptInfo = m_ReportInfos.at(i);
		if (rptInfo->report_en_state == 1 && (rptInfo->TrgOps[0] & C_SMMS_TRGOPT_ZZ) && rptInfo->rcb_name == rcbName)
		{
			ProcessReportInfoGi(rptInfo,iTimeout_s);
		}
	}
}

/********************************************  
* @brief 总召唤报告控制块数据
* @author 宋康
* @date 2015/12/28
* @param stuSMmsReportInfo * rptInfo 报告控制块
* @return void
*********************************************/
void SMmsClient::ProcessReportInfoGi(stuSMmsReportInfo *rptInfo,int iTimeout_s)
{
	SString varName = "";
	ST_BOOLEAN GI = 1;

	varName = rptInfo->rcb_name + "$GI";
	lock();
	ST_RET ret = named_var_write((MVL_NET_INFO*)m_pMVL_NET_INFO_RptEn,varName.data(),DOM_SPEC,rptInfo->dom_name.data(), 
		g_pRPT_TYPEIDS->mmsbool,(ST_CHAR *)&GI,iTimeout_s);
	unlock();
	if (ret != 0)
	{
		LOGWARN("总召唤[%s]失败。",varName.data());
	}
}

/********************************************  
* @brief 判断原因码是否包含数据变化
* @author 宋康
* @date 2015/12/29
* @param SString reason
* @return -true表示包含 -false表示不包含
*********************************************/ 
bool SMmsClient::ReasonIsDchg(SString reason)
{
	char val = reason[REASON_DATA_CHANGE];
	if (val == '1')
		return true;

	return false;
}

/********************************************  
* @brief 判断原因码是否包含品质变化
* @author 宋康
* @date 2015/12/29
* @param SString reason
* @return -true表示包含 -false表示不包含
*********************************************/ 
bool SMmsClient::ReasonIsQchg(SString reason)
{
	char val = reason[REASON_QUALITY_CHANGE];
	if (val == '1')
		return true;

	return false;
}

/********************************************  
* @brief 判断原因码是否包含数据更新
* @author 宋康
* @date 2015/12/29
* @param SString reason
* @return -true表示包含 -false表示不包含
*********************************************/ 
bool SMmsClient::ReasonIsDupd(SString reason)
{
	char val = reason[REASON_DATA_UPDATE];
	if (val == '1')
		return true;

	return false;
}

/********************************************  
* @brief 判断原因码是否包含周期变化
* @author 宋康
* @date 2015/12/29
* @param SString reason
* @return -true表示包含 -false表示不包含
*********************************************/ 
bool SMmsClient::ReasonIsPeriod(SString reason)
{
	char val = reason[REASON_INTEGRITY];
	if (val == '1')
		return true;

	return false;
}

/********************************************  
* @brief 判断原因码是否包含总召唤变化
* @author 宋康
* @date 2015/12/29
* @param SString reason
* @return -true表示包含 -false表示不包含
*********************************************/ 
bool SMmsClient::ReasonIsGi(SString reason)
{
	char val = reason[REASON_GENERAL_INTERROGATION];
	if (val == '1')
		return true;
	
	return false;
}

/********************************************  
* @brief 设置条目标识符
* @author 宋康
* @date 2015/12/30
* @param SString ied_name 装置MMS名称，如：PT1101
* @param stuSMmsReport * report 回调的报告
* @return void
*********************************************/ 
void SMmsClient::SetReportEntryID(stuSMmsReport *report,stuSMmsReportInfo **rptInfo,SString &entry_id)
{
	unsigned long pos;
	stuSMmsReportInfo *pRpt = m_ReportInfos.FetchFirst(pos);
	while(pRpt)
	{
		SString m_rptID_inst;
		SString m_rptID;
		if (pRpt->rptID.isEmpty() || pRpt->rptID=="NULL")
		{
			m_rptID_inst.sprintf("%s/%s",pRpt->dom_name.data(),pRpt->rcb_name.data());
		}
		else
		{
			m_rptID = SString::GetIdAttribute(2,pRpt->rptID,"/");
			m_rptID_inst.sprintf("%s%02d",m_rptID.data(),pRpt->report_id);
			m_rptID_inst = pRpt->dom_name + "/" + m_rptID_inst;
			m_rptID = pRpt->dom_name + "/" + m_rptID;
		}
		if(m_rptID == report->sRptID || m_rptID_inst == report->sRptID || pRpt->rptID == report->sRptID)
		{
			SString sEntryID = GetMvlVarText(report->EntryID);
			if (SString::GetAttributeCount(sEntryID," ") == 8)	//判断entryID数是否为8
			{
				LOGDEBUG("装置报告控制块[%s]条目标识符EntryID[%s]更新成功。",report->sRptID.data(),sEntryID.data());
				sEntryID = sEntryID.replace(" ","");
				SString::StrToHex(sEntryID,pRpt->EntryID);
				*rptInfo = pRpt;
				entry_id = sEntryID;
				break;
			}
		}
		pRpt = m_ReportInfos.FetchNext(pos);
	}
}

bool SMmsClient::QualityIsGood(unsigned short quality)
{
	mms_quality_t q;
	memcpy(&q,&quality,sizeof(mms_quality_t));
	if (q.byte[0] == 0 && (q.byte[1]&0x78) == 0)
		return true;

	return false;
}

bool SMmsClient::QualityIsInvalid(unsigned short quality)
{
	mms_quality_t q;
	memcpy(&q,&quality,sizeof(mms_quality_t));
	if (q.MQ.validity == QUALITY_INVALID)
		return true;

	return false;
}

bool SMmsClient::QualityIsQuestionable(unsigned short quality)
{
	mms_quality_t q;
	memcpy(&q,&quality,sizeof(mms_quality_t));
	if (q.MQ.validity == QUALITY_QUESTIONABLE)
		return true;

	return false;
}

bool SMmsClient::QualityIsOverFlow(unsigned short quality)
{
	mms_quality_t q;
	memcpy(&q,&quality,sizeof(mms_quality_t));
	if (q.MQ.overFlow)
		return true;

	return false;
}

bool SMmsClient::QualityIsOutOfRange(unsigned short quality)
{
	mms_quality_t q;
	memcpy(&q,&quality,sizeof(mms_quality_t));
	if (q.MQ.outOfRange)
		return true;

	return false;
}

bool SMmsClient::QualityIsBadReference(unsigned short quality)
{
	mms_quality_t q;
	memcpy(&q,&quality,sizeof(mms_quality_t));
	if (q.MQ.badReference)
		return true;

	return false;
}

bool SMmsClient::QualityIsOscillatory(unsigned short quality)
{
	mms_quality_t q;
	memcpy(&q,&quality,sizeof(mms_quality_t));
	if (q.MQ.oscillatory)
		return true;

	return false;
}

bool SMmsClient::QualityIsFailure(unsigned short quality)
{
	mms_quality_t q;
	memcpy(&q,&quality,sizeof(mms_quality_t));
	if (q.MQ.failure)
		return true;

	return false;
}

bool SMmsClient::QualityIsOldData(unsigned short quality)
{
	mms_quality_t q;
	memcpy(&q,&quality,sizeof(mms_quality_t));
	if (q.MQ.oldData)
		return true;

	return false;
}

bool SMmsClient::QualityIsInconsistent(unsigned short quality)
{
	mms_quality_t q;
	memcpy(&q,&quality,sizeof(mms_quality_t));
	if (q.MQ.inconsistent)
		return true;

	return false;
}

bool SMmsClient::QualityIsInaccurate(unsigned short quality)
{
	mms_quality_t q;
	memcpy(&q,&quality,sizeof(mms_quality_t));
	if (q.MQ.inaccurate)
		return true;

	return false;
}

bool SMmsClient::QualityIsSubstituted(unsigned short quality)
{
	mms_quality_t q;
	memcpy(&q,&quality,sizeof(mms_quality_t));
	if (q.MQ.substituted)
		return true;

	return false;
}

bool SMmsClient::QualityIsTest(unsigned short quality)
{
	mms_quality_t q;
	memcpy(&q,&quality,sizeof(mms_quality_t));
	if (q.MQ.test)
		return true;

	return false;
}

bool SMmsClient::QualityIsOperatorBlocked(unsigned short quality)
{
	mms_quality_t q;
	memcpy(&q,&quality,sizeof(mms_quality_t));
	if (q.MQ.operatorBlocked)
		return true;

	return false;
}

/********************************************  
* @brief 读取定值区个数
* @author 宋康
* @date 2016/01/05
* @param char * dom_name 域名（PT1101LD0）
* @param int & areaNum 解析后的返回数值
* @param int timeOut 超时时间，默认10秒
* @return -true表示读取成功 -false表示读取失败
*********************************************/ 
bool SMmsClient::ReadSettingAreaNumber(char *dom_name,int &areaNum,int timeOut/*=10*/)
{
	ST_RET	ret = SD_FAILURE;
	ST_CHAR tdl_buf[64] = {'\0'};
	ST_CHAR data[64] = {'\0'};
	char var_name[64] = "LLN0$SP$SGCB$NumOfSG";
	SPtrList<MMSValue> vlist;
	vlist.setAutoDelete(true);

	lock();
	MVL_NET_INFO *m_mvl_net_info = (MVL_NET_INFO*)m_pMVL_NET_INFO_A;
	if (!m_mvl_net_info)
		m_mvl_net_info = (MVL_NET_INFO*)m_pMVL_NET_INFO_B;
	if (!m_mvl_net_info)
	{
		unlock();
		LOGFAULT("读定值区数目,MVL_NET_INFO[0x%x] 错误!",m_mvl_net_info);
		return false;
	}

	ST_INT type_id = domvar_type_id_create(m_mvl_net_info,dom_name,var_name);
	if (type_id != -1)
	{
		ret = named_var_read(m_mvl_net_info,var_name,DOM_SPEC,dom_name,type_id,data,timeOut);
		if (ret == SD_SUCCESS)
		{
			MVL_TYPE_CTRL *type_ctrl = NULL;
			type_ctrl = mvl_type_ctrl_find (type_id);
			if (type_ctrl)
			{
				if (ms_runtime_to_tdl(type_ctrl->rt, type_ctrl->num_rt, tdl_buf, sizeof(tdl_buf))>0)
				{
					ret = parseData(data, type_ctrl->rt, type_ctrl->num_rt,vlist,tdl_buf);
					if (ret == SD_SUCCESS)
						areaNum = vlist.at(0)->getUint8Value();
					else
					{
						LOGFAULT("读数值[%s,%s]解析返回错误! 无法解析数值.",dom_name,var_name);
					}
				}
				else
				{
					LOGFAULT("读定值区数目[%s,%s] ms_runtime_to_tdl 返回错误! 无法解析数值.",dom_name,var_name);
				}
			}
		}
		else
		{
			LOGFAULT("读定值区数目[%s,%s] named_var_read 返回错误! 无法解析数值.",dom_name,var_name);
		}

		mvl_type_id_destroy(type_id);
	}
	else
	{
		LOGFAULT("读定值区数目[%s,%s] domvar_type_id_create 返回错误! 无法解析数值.",dom_name,var_name);
	}

	unlock();
	if (ret == SD_SUCCESS)
		return true;
	return false;
}

/********************************************  
* @brief 读取当前定值区区号
* @author 宋康
* @date 2016/01/11
* @param char * dom_name 域名（PT1101LD0）
* @param int & currentArea 解析后返回的当前定值区区号
* @param int timeOut 超时时间，默认10秒
* @return -true表示读取成功 -false表示读取失败 
*********************************************/ 
bool SMmsClient::ReadCurrentSettingArea(char *dom_name,int &currentArea,int timeOut/*=10*/)
{
	ST_RET	ret = SD_FAILURE;
	ST_CHAR tdl_buf[64] = {'\0'};
	ST_CHAR data[64] = {'\0'};
	char var_name[64] = "LLN0$SP$SGCB$ActSG";
	SPtrList<MMSValue> vlist;
	vlist.setAutoDelete(true);

	lock();
	MVL_NET_INFO *m_mvl_net_info = (MVL_NET_INFO*)m_pMVL_NET_INFO_A;
	if (!m_mvl_net_info)
		m_mvl_net_info = (MVL_NET_INFO*)m_pMVL_NET_INFO_B;
	if (!m_mvl_net_info)
	{
		unlock();
		LOGFAULT("读当前定值区区号,MVL_NET_INFO[0x%x] 错误!",m_mvl_net_info);
		return false;
	}

	ST_INT type_id = domvar_type_id_create(m_mvl_net_info,dom_name,var_name);
	if (type_id != -1)
	{
		ret = named_var_read(m_mvl_net_info,var_name,DOM_SPEC,dom_name,type_id,data,timeOut);
		if (ret == SD_SUCCESS)
		{
			MVL_TYPE_CTRL *type_ctrl = NULL;
			type_ctrl = mvl_type_ctrl_find (type_id);
			if (type_ctrl)
			{
				if (ms_runtime_to_tdl(type_ctrl->rt, type_ctrl->num_rt, tdl_buf, sizeof(tdl_buf))>0)
				{
					ret = parseData(data, type_ctrl->rt, type_ctrl->num_rt,vlist,tdl_buf);
					if (ret == SD_SUCCESS)
						currentArea = vlist.at(0)->getUint8Value();
					else
					{
						LOGFAULT("读当前定值区区号[%s,%s]解析返回错误! 无法解析数值.",dom_name,var_name);
					}
				}
				else
				{
					LOGFAULT("读当前定值区区号[%s,%s] ms_runtime_to_tdl 返回错误! 无法解析数值.",dom_name,var_name);
				}
			}
		}
		else
		{
			LOGFAULT("读当前定值区区号[%s,%s] named_var_read 返回错误! 无法解析数值.",dom_name,var_name);
		}

		mvl_type_id_destroy(type_id);
	}
	else
	{
		LOGFAULT("读当前定值区区号[%s,%s] domvar_type_id_create 返回错误! 无法解析数值.",dom_name,var_name);
	}

	unlock();
	if (ret == SD_SUCCESS)
		return true;
	return false;
}

/********************************************  
* @brief 读取编辑定值区区号
* @author 宋康
* @date 2016/01/11
* @param char * dom_name 域名（PT1101LD0）
* @param int & currentArea 解析后返回的编辑定值区区号
* @param int timeOut 超时时间，默认10秒
* @return -true表示读取成功 -false表示读取失败 
*********************************************/ 
bool SMmsClient::ReadEditSettingArea(char *dom_name,int &editArea,int timeOut/*=10*/)
{
	ST_RET	ret = SD_FAILURE;
	ST_CHAR tdl_buf[64] = {'\0'};
	ST_CHAR data[64] = {'\0'};
	char var_name[64] = "LLN0$SP$SGCB$EditSG";
	SPtrList<MMSValue> vlist;
	vlist.setAutoDelete(true);

	lock();
	MVL_NET_INFO *m_mvl_net_info = (MVL_NET_INFO*)m_pMVL_NET_INFO_A;
	if (!m_mvl_net_info)
		m_mvl_net_info = (MVL_NET_INFO*)m_pMVL_NET_INFO_B;
	if (!m_mvl_net_info)
	{
		unlock();
		LOGFAULT("读编辑定值区区号,MVL_NET_INFO[0x%x] 错误!",m_mvl_net_info);
		return false;
	}

	ST_INT type_id = domvar_type_id_create(m_mvl_net_info,dom_name,var_name);
	if (type_id != -1)
	{
		ret = named_var_read(m_mvl_net_info,var_name,DOM_SPEC,dom_name,type_id,data,timeOut);
		if (ret == SD_SUCCESS)
		{
			MVL_TYPE_CTRL *type_ctrl = NULL;
			type_ctrl = mvl_type_ctrl_find (type_id);
			if (type_ctrl)
			{
				if (ms_runtime_to_tdl(type_ctrl->rt, type_ctrl->num_rt, tdl_buf, sizeof(tdl_buf))>0)
				{
					ret = parseData(data, type_ctrl->rt, type_ctrl->num_rt,vlist,tdl_buf);
					if (ret == SD_SUCCESS)
						editArea = vlist.at(0)->getUint8Value();
					else
					{
						LOGFAULT("读编辑定值区区号[%s,%s]解析返回错误! 无法解析数值.",dom_name,var_name);
					}
				}
				else
				{
					LOGFAULT("读编辑定值区区号[%s,%s] ms_runtime_to_tdl 返回错误! 无法解析数值.",dom_name,var_name);
				}
			}
		}
		else
		{
			LOGFAULT("读编辑定值区区号[%s,%s] named_var_read 返回错误! 无法解析数值.",dom_name,var_name);
		}

		mvl_type_id_destroy(type_id);
	}
	else
	{
		LOGFAULT("读编辑定值区区号[%s,%s] domvar_type_id_create 返回错误! 无法解析数值.",dom_name,var_name);
	}

	unlock();
	if (ret == SD_SUCCESS)
		return true;
	return false;
}

/********************************************  
* @brief 修改当前定值区区号
* @author 宋康
* @date 2016/02/02
* @param char * dom_name 域名（PT1101LD0）
* @param int currentArea 需修改的定值区区号
* @param int timeOut 超时时间，默认10秒
* @return -true表示修改成功 -false表示修改失败 
*********************************************/ 
bool SMmsClient::WriteCurrentSettingArea(char *dom_name,int currentArea,int timeOut/*=10*/)
{
	ST_RET	ret = SD_FAILURE;
	ST_CHAR area = currentArea;
	char var_name[64] = "LLN0$SP$SGCB$ActSG";

	lock();
	MVL_NET_INFO *m_mvl_net_info = (MVL_NET_INFO*)m_pMVL_NET_INFO_A;
	if (!m_mvl_net_info)
		m_mvl_net_info = (MVL_NET_INFO*)m_pMVL_NET_INFO_B;
	if (!m_mvl_net_info)
	{
		unlock();
		LOGFAULT("读数值，MVL_NET_INFO[0x%x] 错误!",m_mvl_net_info);
		return false;
	}

	ret = named_var_write(m_mvl_net_info,var_name,DOM_SPEC,dom_name,g_pRPT_TYPEIDS->int8u,&area,timeOut);
	if (ret != SD_SUCCESS)	
	{
		LOGFAULT("修改当前定值[%s,%s] named_var_write 写操作，返回错误!",dom_name,var_name);
		unlock();
		return	false;
	}

	unlock();
	return true;
}

/********************************************  
* @brief 修改编辑定值区区号
* @author 宋康
* @date 2016/02/02
* @param char * dom_name 域名（PT1101LD0）
* @param int currentArea 需修改的定值区区号
* @param int timeOut 超时时间，默认10秒
* @return -true表示修改成功 -false表示修改失败 
*********************************************/ 
bool SMmsClient::WriteEditSettingArea(char *dom_name,int editArea,int timeOut/*=10*/)
{
	ST_RET	ret = SD_FAILURE;
	ST_CHAR area = editArea;
	char var_name[64] = "LLN0$SP$SGCB$EditSG";

	lock();
	MVL_NET_INFO *m_mvl_net_info = (MVL_NET_INFO*)m_pMVL_NET_INFO_A;
	if (!m_mvl_net_info)
		m_mvl_net_info = (MVL_NET_INFO*)m_pMVL_NET_INFO_B;
	if (!m_mvl_net_info)
	{
		unlock();
		LOGFAULT("读数值，MVL_NET_INFO[0x%x] 错误!",m_mvl_net_info);
		return false;
	}

	ret = named_var_write(m_mvl_net_info,var_name,DOM_SPEC,dom_name,g_pRPT_TYPEIDS->int8u,&area,timeOut);
	if (ret != SD_SUCCESS)	
	{
		LOGFAULT("修改当前定值[%s,%s] named_var_write 写操作，返回错误!",dom_name,var_name);
		unlock();
		return	false;
	}

	unlock();
	return true;
}

/********************************************  
* @brief 读取单个数值
* @author 宋康
* @date 2016/01/05
* @param char * dom_name 域名（PT1101LD0）
* @param char * var_name 值名（LLN0$$SG$Enable$setVal）
* @param SPtrList<MMSValue> & lstValue 解析后的返回数值队列
* @param int timeOut 超时时间，默认10秒
* @return -true表示读取成功 -false表示读取失败
*********************************************/ 
bool SMmsClient::ReadValue(char *dom_name,char *var_name,SPtrList<MMSValue> &lstValue,int timeOut/*=10*/)
{
	ST_RET	ret = SD_FAILURE;
	ST_CHAR tdl_buf[1024] = {'\0'};
	ST_CHAR data[1024] = {'\0'};

	lock();
	MVL_NET_INFO *m_mvl_net_info = (MVL_NET_INFO*)m_pMVL_NET_INFO_A;
	if (!m_mvl_net_info)
		m_mvl_net_info = (MVL_NET_INFO*)m_pMVL_NET_INFO_B;
	if (!m_mvl_net_info)
	{
		unlock();
		LOGFAULT("读数值，MVL_NET_INFO[0x%x] 错误!",m_mvl_net_info);
		return false;
	}

	ST_INT type_id = domvar_type_id_create(m_mvl_net_info,dom_name,var_name);
	if (type_id != -1)
	{
		ret = named_var_read(m_mvl_net_info,var_name,DOM_SPEC,dom_name,type_id,data,timeOut);
		if (ret == SD_SUCCESS)
		{
			MVL_TYPE_CTRL *type_ctrl = NULL;
			type_ctrl = mvl_type_ctrl_find (type_id);
			if (type_ctrl)
			{
				if (ms_runtime_to_tdl(type_ctrl->rt, type_ctrl->num_rt, tdl_buf, sizeof(tdl_buf))>0)
				{
					ret = parseData(data, type_ctrl->rt, type_ctrl->num_rt,lstValue,tdl_buf);
					if (ret != SD_SUCCESS)
					{
						LOGFAULT("读数值[%s,%s]解析返回错误! 无法解析数值.",dom_name,var_name);
					}
				}
				else
				{
					LOGFAULT("读数值[%s,%s] ms_runtime_to_tdl 返回错误! 无法解析数值.",dom_name,var_name);
				}
			}
		}
		else
		{
			LOGFAULT("读数值[%s,%s] named_var_read 返回错误! 无法解析数值.",dom_name,var_name);
		}

		mvl_type_id_destroy(type_id);
	}
	else
	{
		LOGFAULT("读数值[%s,%s] domvar_type_id_create 返回错误! 无法解析数值.",dom_name,var_name);
	}

	unlock();
	if (ret == SD_SUCCESS)
		return true;
	return false;
}

typedef struct {
	int type_id;
	char data[256];
} var_dt_t;
ST_RET named_vars_read (MVL_NET_INFO *net_info,SPtrList<SString> &var_name,ST_INT scope,
	SPtrList<SString> &dom_name,SPtrList<var_dt_t> &vdt,ST_INT timeOut);
/********************************************  
* @brief 读取多个数值
* @author 宋康
* @date 2016/01/06
* @param SPtrList<SString> & dom_name 域名队列（PT1101LD0）
* @param SPtrList<SString> & var_name 值名队列（LLN0$$SG$Enable$setVal）
* @param SPtrList<SPtrList<MMSValue>> & lstValue 解析后的返回数值队列的队列
* @param int timeOut 超时时间，默认10秒
* @return -true表示读取成功 -false表示读取失败
*********************************************/ 
bool SMmsClient::ReadValues(SPtrList<SString> &dom_name,SPtrList<SString> &var_name,
	SPtrList< SPtrList<MMSValue> > &lstValue,int timeOut)
{
	ST_RET	ret = SD_FAILURE;
	ST_CHAR tdl_buf[256] = {'\0'};
	int i,cnt = dom_name.count();

	m_isUserTaskProcessing = true;
	if (!WaitSystemTaskFinish())
	{
		m_isUserTaskProcessing = false;
		return false;
	}

	lock();
	m_isUserTaskProcessing = true;
	if (cnt <= 0 || cnt > MAX_READ_VALUES_NUMBER)
	{
		m_isUserTaskProcessing = false;
		unlock();
		LOGFAULT("读多数值，数值项个数[%d]异常!",cnt);
		return false;
	}

	MVL_NET_INFO *m_mvl_net_info = (MVL_NET_INFO*)m_pMVL_NET_INFO_A;
	if (!m_mvl_net_info)
		m_mvl_net_info = (MVL_NET_INFO*)m_pMVL_NET_INFO_B;
	if (!m_mvl_net_info)
	{
		m_isUserTaskProcessing = false;
		unlock();
		LOGFAULT("读多数值，MVL_NET_INFO[0x%x] 错误!",m_mvl_net_info);
		return false;
	}

	SPtrList<var_dt_t> vdt;
	vdt.setAutoDelete(true);
	for (i = 0; i < cnt; i++)
	{
		var_dt_t *dt = new var_dt_t;
		dt->type_id = domvar_type_id_create(m_mvl_net_info,dom_name.at(i)->data(),var_name.at(i)->data());
		if (dt->type_id != -1)
		{
			vdt.append(dt);
		}
		else
		{
			delete dt;
			m_isUserTaskProcessing = false;
			unlock();
			LOGFAULT("读多数值[%s,%s] domvar_type_id_create 返回错误!",dom_name.at(i)->data(),var_name.at(i)->data());
			return false;
		}
	}

	ret = named_vars_read(m_mvl_net_info,var_name,DOM_SPEC,dom_name,vdt,timeOut);
	if (ret == SD_SUCCESS)
	{
		for (i = 0; i < cnt; i++)
		{
			MVL_TYPE_CTRL *type_ctrl = NULL;
			type_ctrl = mvl_type_ctrl_find (vdt.at(i)->type_id);
			if (type_ctrl)
			{
				if (ms_runtime_to_tdl(type_ctrl->rt, type_ctrl->num_rt, tdl_buf, sizeof(tdl_buf))>0)
				{
					ret = parseData(vdt.at(i)->data, type_ctrl->rt, type_ctrl->num_rt,*lstValue.at(i),tdl_buf);
					if (ret != SD_SUCCESS)
					{
						LOGFAULT("读多数值中的[%s,%s]解析返回错误! 无法解析数值.",dom_name.at(i),var_name.at(i));
					}
				}
				else
				{		
					LOGFAULT("读多数值中的[%s,%s] ms_runtime_to_tdl 返回错误! 无法解析数值.",dom_name.at(i),var_name.at(i));
				}
			}
		}
	}
	else
	{
		LOGFAULT("读多数值, named_vars_read 返回错误! 无法解析数值.");
		LOGFAULT("读多数值, var_name个数%d，dom_name个数%d，vdt个数%d.",var_name.count(),dom_name.count(),vdt.count());
		for (i = 0; i < var_name.count(); i++)
		{
			LOGFAULT("读多数值, var_name[%d]=%s，dom_name[%d]=%s，vdt[%d].type_id=%d.",
				i,var_name.at(i)->data(),
				i,dom_name.at(i)->data(),
				i,vdt.at(i)->type_id);
		}
	}

	for (i = 0; i < cnt; i++)
		mvl_type_id_destroy(vdt.at(i)->type_id);

	m_isUserTaskProcessing = false;
	unlock();

	if (ret == SD_SUCCESS)
		return true;
	return false;
}

/********************************************  
* @brief 写单个数值
* @author 宋康
* @date 2016/02/03
* @param char * dom_name 域名（PT1101LD0）
* @param char * var_name 值名（LLN0$SG$Enable$setVal）
* @param char * data 写的数值
* @param int timeOut 超时时间，默认10秒
* @return -true表示写值成功 -false表示写值失败
*********************************************/ 
bool SMmsClient::WriteValue(char *dom_name,char *var_name,char *data,int timeOut/*=10*/)
{
	ST_RET	ret = SD_FAILURE;

	m_isUserTaskProcessing = true;
	if (!WaitSystemTaskFinish())
	{
		m_isUserTaskProcessing = false;
		return false;
	}

	lock();
	m_isUserTaskProcessing = true;

	MVL_NET_INFO *m_mvl_net_info = (MVL_NET_INFO*)m_pMVL_NET_INFO_A;
	if (!m_mvl_net_info)
		m_mvl_net_info = (MVL_NET_INFO*)m_pMVL_NET_INFO_B;
	if (!m_mvl_net_info)
	{
		m_isUserTaskProcessing = false;
		unlock();
		LOGFAULT("写数值[%s,%s]，MVL_NET_INFO[0x%x] 错误!",dom_name,var_name,m_mvl_net_info);
		return false;
	}

	ST_INT type_id = domvar_type_id_create(m_mvl_net_info,dom_name,var_name);
	if (type_id != -1)
	{
		ret = named_var_write(m_mvl_net_info,var_name,DOM_SPEC,dom_name,type_id,data,timeOut);
		if (ret != SD_SUCCESS)
		{
			LOGFAULT("写数值[%s,%s] named_var_write 返回错误! 写数值失败.",dom_name,var_name);
		}

		mvl_type_id_destroy(type_id);
	}
	else
	{
		LOGFAULT("写数值[%s,%s] domvar_type_id_create 返回错误! 写数值失败.",dom_name,var_name);
	}

	m_isUserTaskProcessing = false;
	unlock();
	if (ret == SD_SUCCESS)
		return true;
	return false;
}

/********************************************  
* @brief 固化定值
* @author 宋康
* @date 2016/02/03
* @param char * dom_name 域名（PT1101LD0）
* @param int timeOut 超时时间，默认10秒
* @return -true表示固化成功 -false表示固化失败
*********************************************/ 
bool SMmsClient::CureSetting(char *dom_name,int timeOut/*=10*/)
{
	ST_RET	ret = SD_FAILURE;
	char    var_name[64] = "LLN0$SP$SGCB$CnfEdit";
	int		data = 1;

	m_isUserTaskProcessing = true;
	if (!WaitSystemTaskFinish())
	{
		m_isUserTaskProcessing = false;
		return false;
	}

	lock();
	m_isUserTaskProcessing = true;
	MVL_NET_INFO *m_mvl_net_info = (MVL_NET_INFO*)m_pMVL_NET_INFO_A;
	if (!m_mvl_net_info)
		m_mvl_net_info = (MVL_NET_INFO*)m_pMVL_NET_INFO_B;
	if (!m_mvl_net_info)
	{
		m_isUserTaskProcessing = false;
		unlock();
		LOGFAULT("固化定值[%s]，MVL_NET_INFO[0x%x] 错误!",dom_name,m_mvl_net_info);
		return false;
	}

	ret = named_var_write(m_mvl_net_info,var_name,DOM_SPEC,dom_name,g_pRPT_TYPEIDS->mmsbool,&data,timeOut);
	if (ret != SD_SUCCESS)	
	{
		LOGFAULT("固化定值[%s] named_var_write 写操作失败，返回错误[0X%x]!",dom_name,ret);
		m_isUserTaskProcessing = false;
		unlock();
		return	false;
	}

	m_isUserTaskProcessing = false;
	unlock();
	return true;
}

/********************************************  
* @brief 遥控操作，包括选控和直控
* @author 宋康
* @date 2016/02/01
* @param char * dom_name 域名（PT1101LD0）
* @param char * var_name 值名（CSWI1$CO$Pos）
* @param int ctrlValue 遥控值（0分，1合）
* @param bool check 遥控检查
* @param int timeOut 超时时间，默认10秒
* @return -true表示遥控成功 -false表示遥控失败
*********************************************/ 
bool SMmsClient::ProcessControl(char *dom_name,char *var_name,int ctrlValue,bool check,int timeOut/*=10*/)
{
	ST_RET	ret = SD_FAILURE;
	ST_CHAR oper_data_buf[256] = {'\0'};	/* "Oper" data	*/
	RUNTIME_CTRL rt_ctrl;

	MVL_NET_INFO *m_mvl_net_info = (MVL_NET_INFO*)m_pMVL_NET_INFO_A;
	if (!m_mvl_net_info)
		m_mvl_net_info = (MVL_NET_INFO*)m_pMVL_NET_INFO_B;
	if (!m_mvl_net_info)
	{
		LOGFAULT("遥控操作，MVL_NET_INFO[0x%x] 错误!",m_mvl_net_info);
		return false;
	}

	ST_INT type_id = domvar_type_id_create(m_mvl_net_info,dom_name,var_name);
	if (type_id != -1)
	{
		ret = mvl_get_runtime(type_id, &rt_ctrl.rt_first,&rt_ctrl.rt_num);
		if (ret == SD_SUCCESS)
		{
			ST_INT data_offset = 0;	/* use to compute offset to components	*/
			memset(oper_data_buf, 0, sizeof (oper_data_buf));	/* start with "Oper" data all 0*/
			for (int j = 0; j < rt_ctrl.rt_num; j++)
			{
				RUNTIME_TYPE *rt_type = &rt_ctrl.rt_first[j];
				if (strcmp (ms_comp_name_find(rt_type), "ctlVal") == 0)
				{
					if (rt_type->el_tag == RT_INTEGER && rt_type->u.p.el_len == 4)
						*(ST_INT32 *)(oper_data_buf+data_offset) = ctrlValue;
					if (rt_type->el_tag == RT_BOOL && rt_type->u.p.el_len == 1)
						*(ST_BOOLEAN *)(oper_data_buf+data_offset) = (ST_BOOLEAN)ctrlValue;
				}
				else if ((strcmp (ms_comp_name_find(rt_type), "operTm") == 0) || 
					(strcmp (ms_comp_name_find(rt_type), "T") == 0) )
				{
					if (rt_type->el_tag == RT_UTC_TIME)
					{
						MMS_UTC_TIME *utc_time;
						utc_time = (MMS_UTC_TIME *)(oper_data_buf+data_offset);
						utc_time->secs = (ST_UINT32) time(NULL);
						utc_time->fraction = 99;
						utc_time->qflags = 0;
					}
				}
				else if (check && strcmp (ms_comp_name_find(rt_type), "Check") == 0)
				{
					if (rt_type->el_tag == RT_BIT_STRING && abs(rt_type->u.p.el_len)==2 ) 
					{
						rt_type->u.p.el_len = 2;
						*(ST_UINT8 *)(oper_data_buf+data_offset) = 0x00;
					}
				}
				else if (strcmp (ms_comp_name_find(rt_type), "orCat") == 0) 
				{
					if (rt_type->el_tag == RT_INTEGER && rt_type->u.p.el_len == 1 )
						*(ST_UINT8 *)(oper_data_buf+data_offset) = 3;//remote-control
				}
				else if (strcmp (ms_comp_name_find(rt_type), "orIdent") == 0) 
				{
					if (rt_type->el_tag == RT_OCTET_STRING && rt_type->u.p.el_len == -64 ) 
					{
						rt_type->u.p.el_len = strlen(identify_response_info.vend);
						memcpy(oper_data_buf+data_offset, identify_response_info.vend, rt_type->u.p.el_len );
					}
				}
				data_offset += rt_type->el_size;
			}	/* end "for"	*/
		}
		else
		{
			LOGFAULT("遥控操作[%s,%s] mvl_get_runtime 获取遥控参数，返回错误!",dom_name,var_name);
			mvl_type_id_destroy(type_id);
			return false;
		}

		/* NOTE: -1 indicates "CommandTermination" not received yet.		*/
		/*  This must be set before "mms_named_var_write" because			*/
		/*  CommandTermination may be received before it returns.		*/ 
		if (strstr(var_name,"$Oper"))
		{
			my_control_info.cmd_term_num_va = 1;
			strcpy (my_control_info.oper_name, var_name);	/* save ctrl_name as oper_name for later*/
		}

		/* NOTE: if this is SBO control and SBO read failed, this write	*/
		/*   to "Oper" should fail, but try it anyway.			*/
		lock();
		ret = named_var_write(m_mvl_net_info,var_name,DOM_SPEC,dom_name,type_id,oper_data_buf,timeOut);
		mvl_type_id_destroy(type_id);
		if (ret != SD_SUCCESS)	
		{
			LOGFAULT("控制操作[%s,%s] named_var_write 写操作，返回错误!",dom_name,var_name);
			unlock();
			return	false;
		}

		/* Wait here for CommandTermination info rpt before returning to main loop.*/ 
		if (strstr(var_name,"$Oper"))
		{   
			time_t timeout = time(NULL)+timeOut;	/* DEBUG: chg timeout as needed.*/
			while (SD_TRUE)
			{
				doCommService (); 
				/* .._num_va is set by u_mvl_info_rpt_ind when report received.	*/
				if (my_control_info.cmd_term_num_va>=0)
				{
					/* If 2 vars in rpt, must be error. If 1 var, must be OK.	*/
					LOGDEBUG("控制操作[%s,%s]成功，cmd_term_num_va=%d。",dom_name,var_name,my_control_info.cmd_term_num_va);
					break;
				}
				if (time(NULL) > timeout)
				{
					LOGFAULT("控制操作[%s,%s] 超时[%d]秒!",dom_name,var_name,timeOut);
					unlock();
					return false;
				}      
			}
		}  

		unlock();
	}
	else
	{
		LOGFAULT("遥控操作[%s,%s] domvar_type_id_create 返回错误!",dom_name,var_name);
	}

	if (ret == SD_SUCCESS)
		return true;
	return false;
}

/********************************************  
* @brief 信号复归/设备复位
* @author 宋康
* @date 2016/02/01
* @param char * dom_name 域名（PT1101LD0）
* @param char * var_name 值名（LLN0$CO$LEDRs）
* @return -true表示复归成功 -false表示复归失败
*********************************************/ 
bool SMmsClient::Reset(char *dom_name,char *var_name)
{
	return ProcessControl(dom_name,var_name,1,false);
}

/********************************************  
* @brief 获取装置确认，用于定期测试连接
* @author 宋康
* @date 2016/01/26
* @param int iTimeout_s 超时时间，默认10秒
* @return -true表示读取成功 -false表示读取失败
*********************************************/ 
bool SMmsClient::GetIdentify(int iTimeout_s/*=10*/)
{
	ST_RET			ret;
	MVL_REQ_PEND	*req_pend;

	m_isUserTaskProcessing = true;
	if (!WaitSystemTaskFinish())
	{
		m_isUserTaskProcessing = false;
		return false;
	}

	lock();
	m_isUserTaskProcessing = true;
	MVL_NET_INFO *m_mvl_net_info = (MVL_NET_INFO*)m_pMVL_NET_INFO_A;
	if (!m_mvl_net_info)
		m_mvl_net_info = (MVL_NET_INFO*)m_pMVL_NET_INFO_B;
	if (!m_mvl_net_info)
	{
		m_isUserTaskProcessing = false;
		unlock();
		LOGFAULT("测试连接，MVL_NET_INFO[0x%x] 错误!",m_mvl_net_info);
		return false;
	}

	ret = mvla_identify(m_mvl_net_info, &req_pend);
	if (ret == SD_SUCCESS)
		ret = waitReqDone(req_pend,iTimeout_s);
	else
	{
		LOGFAULT("测试连接 mvla_identify 返回错误! ret=0x%x。",ret);
	}

	if (ret != SD_SUCCESS)
	{
		LOGFAULT("测试连接 waitReqDone 返回错误! ret=0x%x。",ret);
	}

	mvl_free_req_ctrl(req_pend);

	m_isUserTaskProcessing = false;
	unlock();
	if (ret == SD_SUCCESS)
		return true;
	return false;
}

bool SMmsClient::InitJournal(char *dom_name,char *var_name,unsigned long &delEntries,int timeOut/*=10*/)
{
	ST_RET			ret;
	MVL_REQ_PEND	*req_pend;
	JINIT_REQ_INFO	jinit_req;

	lock();
	MVL_NET_INFO *m_mvl_net_info = (MVL_NET_INFO*)m_pMVL_NET_INFO_A;
	if (!m_mvl_net_info)
		m_mvl_net_info = (MVL_NET_INFO*)m_pMVL_NET_INFO_B;
	if (!m_mvl_net_info)
	{
		unlock();
		LOGFAULT("初始化日志，MVL_NET_INFO[0x%x] 错误!",m_mvl_net_info);
		return false;
	}

	jinit_req.jou_name.object_tag = DOM_SPEC;
	jinit_req.jou_name.domain_id = dom_name;
	jinit_req.jou_name.obj_name.vmd_spec = var_name;
	jinit_req.limit_spec_pres = SD_FALSE;
	jinit_req.limit_entry_pres = SD_FALSE;

	ret = mvla_jinit (m_mvl_net_info, &jinit_req, &req_pend);
	if (ret == SD_SUCCESS)
		ret = waitReqDone(req_pend, timeOut);
	else
	{
		LOGFAULT("初始化日志，mvla_jinit 返回错误! ret=0x%x。",ret);
	}

	if (ret != SD_SUCCESS)
	{
		LOGFAULT("初始化日志，waitReqDone 返回错误! ret=0x%x。",ret);
	}
	else
		delEntries = req_pend->u.jinit.resp_info->del_entries;

	mvl_free_req_ctrl (req_pend);

	unlock();
	if (ret == SD_SUCCESS)
		return true;
	return false;
}

bool SMmsClient::GetJournalState(char *dom_name,char *var_name,unsigned long &entries,bool &deletable,int timeOut/*=10*/)
{
	ST_RET			ret;
	MVL_REQ_PEND	*req_pend;
	JSTAT_REQ_INFO  jstat_req;

	lock();
	MVL_NET_INFO *m_mvl_net_info = (MVL_NET_INFO*)m_pMVL_NET_INFO_A;
	if (!m_mvl_net_info)
		m_mvl_net_info = (MVL_NET_INFO*)m_pMVL_NET_INFO_B;
	if (!m_mvl_net_info)
	{
		unlock();
		LOGFAULT("读取日志状态，MVL_NET_INFO[0x%x] 错误!",m_mvl_net_info);
		return false;
	}

	jstat_req.jou_name.object_tag = DOM_SPEC;
	jstat_req.jou_name.domain_id = dom_name;
	jstat_req.jou_name.obj_name.vmd_spec = var_name;
	ret = mvla_jstat (m_mvl_net_info, &jstat_req, &req_pend);
	if (ret == SD_SUCCESS)
		ret = waitReqDone (req_pend, timeOut);
	else
	{
		LOGFAULT("读取日志状态，mvla_jstat 返回错误! ret=0x%x。",ret);
	}

	if (ret != SD_SUCCESS)
	{
		LOGFAULT("读取日志状态，waitReqDone 返回错误! ret=0x%x。",ret);
	}
	else
	{
		entries = req_pend->u.jstat.resp_info->cur_entries;
		deletable = req_pend->u.jstat.resp_info->mms_deletable == 0 ? false : true;
	}

	mvl_free_req_ctrl (req_pend);

	unlock();
	if (ret == SD_SUCCESS)
		return true;
	return false;
}

bool SMmsClient::ReadJournal(char *dom_name,char *var_name,SDateTime &start,SDateTime &end,unsigned char *entryID,
	SPtrList<stuJournalData> &lstJData,bool &follow,int timeOut/*=10*/)
{
	ST_RET				ret;
	MVL_REQ_PEND		*req_pend;
	JREAD_REQ_INFO		jread_req;
	MVL_JREAD_RESP_INFO *jread_resp;
	MVL_JOURNAL_ENTRY	*jou_entry = 0;

	lock();
	MVL_NET_INFO *m_mvl_net_info = (MVL_NET_INFO*)m_pMVL_NET_INFO_A;
	if (!m_mvl_net_info)
		m_mvl_net_info = (MVL_NET_INFO*)m_pMVL_NET_INFO_B;
	if (!m_mvl_net_info)
	{
		unlock();
		LOGFAULT("读取日志，MVL_NET_INFO[0x%x] 错误!",m_mvl_net_info);
		return false;
	}

	static MMS_BTOD btod;
	static ST_INT	entry_id_len;
	static ST_UCHAR entry_id[8];
	
	jread_req.jou_name.object_tag = DOM_SPEC;
	jread_req.jou_name.domain_id = dom_name;
	jread_req.jou_name.obj_name.vmd_spec = var_name;
	if (SString::HexToStr(entryID,8,false) == "0000000000000000")
	{
		jread_req.range_start_pres = 1;
		jread_req.range_stop_pres = 1;
		jread_req.start_tag = 0;
		jread_req.stop_tag = 0;
		SDateTimeToMmsBtod(&start,&jread_req.start_time);
		SDateTimeToMmsBtod(&end,&jread_req.end_time);
		jread_req.list_of_var_pres = 0;
		jread_req.sa_entry_pres = follow;
	}
	else
	{
		jread_req.range_start_pres = 0;
		jread_req.range_stop_pres = 0;
		jread_req.start_tag = 1;
		jread_req.stop_tag = 1;
		jread_req.num_of_entries = 100;
		SDateTimeToMmsBtod(&start,&jread_req.start_time);
		jread_req.start_entry_len = 8;
		jread_req.start_entry = (unsigned char*)entryID;
		jread_req.list_of_var_pres = 0;
		//jread_req.sa_entry_pres = follow;

		jread_req.sa_entry_pres = true;
		SDateTimeToMmsBtod(&start,&jread_req.time_spec);
		jread_req.entry_spec_len = 8;
		jread_req.entry_spec = (unsigned char*)entryID;
	}
	
	if (follow)
	{
		jread_req.time_spec = btod;
		jread_req.entry_spec_len = entry_id_len;
		jread_req.entry_spec = entry_id;
	}
	
	ret = mvla_jread (m_mvl_net_info, &jread_req, &req_pend);
	if (ret == SD_SUCCESS)
		ret = waitReqDone (req_pend,timeOut);
	else
	{
		LOGFAULT("读取日志，mvla_jread 返回错误! ret=0x%x。",ret);
	}

	if (ret != SD_SUCCESS)
	{
		LOGFAULT("读取日志，waitReqDone 返回错误! ret=0x%x。",ret);
	}
	else
	{
		jread_resp = req_pend->u.jread.resp_info;
		follow = jread_resp->more_follows;
		for (int i = 0; i < jread_resp->num_of_jou_entry; i++)
		{
			jou_entry = &jread_resp->jou_entry[i];
			//printf ("\n   Journal Entry # %d:", i);
			//printf ("\n     entry_id_len = %d", jou_entry->entry_id_len);
			//printf ("\n     occur_time.form = %s", (jou_entry->occur_time.form == MMS_BTOD6 ? "MMS_BTOD6" : "MMS_BTOD4"));
			//printf ("\n     occur_time.ms  = %ld", (ST_LONG) jou_entry->occur_time.ms);
			//printf ("\n     occur_time.day = %ld", (ST_LONG) jou_entry->occur_time.day);
			//printf ("\n     entry_form_tag = %d", jou_entry->entry_form_tag);
			if (jou_entry->entry_form_tag == 2)
			{
				if (jou_entry->ef.data.list_of_var_pres)
				{
					for (int j = 0; j < jou_entry->ef.data.num_of_var; j++)
					{
						SString path = jou_entry->ef.data.list_of_var[j].var_tag;
						int	num = SString::GetAttributeCount(path,"/");
						if (num < 2)
							continue;
						SString dn = SString::GetIdAttribute(1,path,"/");
						SString vn = SString::GetIdAttribute(2,path,"/");
						if (vn.Find("reasonCode") > 0 || vn.Find("ReasonCode") > 0)
							continue;

						int type_id = domvar_type_id_create(m_mvl_net_info,dn.data(),vn.data());
						if (type_id == -1)
							continue;

						SPtrList<MMSValue> value;
						RUNTIME_TYPE *rt;
						int	rt_num;
						char dest[512] = {'\0'};
						int ret = mvl_get_runtime(type_id,&rt,&rt_num);
						if (ret == SD_SUCCESS)
						{
							ret = ms_asn1_to_local(rt,rt_num,jou_entry->ef.data.list_of_var[j].value_spec.data,
								jou_entry->ef.data.list_of_var[j].value_spec.len,dest);
							if (ret == SD_SUCCESS)
							{
								ST_CHAR tdl_buf[64] = {'\0'};
								MVL_TYPE_CTRL *type_ctrl = NULL;
								type_ctrl = mvl_type_ctrl_find(type_id);
								if (type_ctrl)
								{
									if (ms_runtime_to_tdl(type_ctrl->rt, type_ctrl->num_rt, tdl_buf, sizeof(tdl_buf))>0)
									{
										ret = parseData(dest, type_ctrl->rt, type_ctrl->num_rt,value,tdl_buf);
										if (ret == SD_SUCCESS)
										{
											stuJournalData *jd = new stuJournalData;
											jd->value.setAutoDelete(true);
											jd->ref = path;
											value.copyto(jd->value);
											memset(jd->entry_id,0,sizeof(jd->entry_id));
											memcpy(jd->entry_id,jou_entry->entry_id,jou_entry->entry_id_len);
											lstJData.append(jd);
										}
										else
										{
											//delete jd;
											LOGFAULT("读日志[%s,%s]解析返回错误! 无法解析数值.", dn.data(), vn.data());
										}
									}
								}
							}
						}
						
						mvl_type_id_destroy(type_id);
					}
				}
			}
			else
			{
				printf ("\n       annotation = %s",jou_entry->ef.annotation);
			}
		}	/* end "loop"	*/

		if (jou_entry)
		{
			entry_id_len = jou_entry->entry_id_len;
			btod = jou_entry->occur_time;
			memcpy(entry_id,jou_entry->entry_id,8);
		}
	}

	mvl_free_req_ctrl (req_pend);	/* CRITICAL:		*/

	unlock();
	if (ret == SD_SUCCESS)
		return true;
	return false;
}

/********************************************  
* @brief 设置装置内全部LD的数据条目名
* @author 宋康
* @date 2016/01/05
* @return -true表示成功 -false表示失败
*********************************************/
bool SMmsClient::SetIedVarName()
{
	bool ret = false;
	unsigned long pos;

	CSMmsLogicDevice *p = m_LogicDevices.FetchFirst(pos);
	while(p)
	{
		ret = p->SetVarNameList();
		if (ret)
			p = m_LogicDevices.FetchNext(pos);
		else
			break;
	}

	return ret;
}

void SMmsClient::SetMultiThreadMode(bool b)
{
	g_bSingleThreadMode = b;
}

void SMmsClient::SDateTimeToMmsBtod(SDateTime *dateTime,void *btod)
{
	MMS_BTIME6 bt6;
	MMS_BTOD *bd = (MMS_BTOD*)btod;
	time_t t = SDateTime::makeSOC(dateTime->year(),dateTime->month(),dateTime->day(),
		dateTime->hour(),dateTime->minute(),dateTime->second());
	
	asn1_convert_timet_to_btime6(t,&bt6);
	bd->form = MMS_BTOD6;
	bd->day = bt6.day;
	bd->ms = bt6.ms;
}

/****************  stuSMmsDataNode  ****************/
stuSMmsDataNode::stuSMmsDataNode()
{
	pValue = NULL;
	iMmsOperId = 0;
	user = NULL;
}

stuSMmsDataNode::~stuSMmsDataNode()
{
	MVL_VAR_ASSOC *var = (MVL_VAR_ASSOC*)pValue;
	if(var != NULL)
	{
		u_mvl_free_va(iMmsOperId,var,NULL);
		var = NULL;
	}
}

/****************  CSMmsDataset  ****************/
CSMmsDataset::CSMmsDataset()
{
	m_Items.setAutoDelete(true);
	user = NULL;
}

CSMmsDataset::~CSMmsDataset()
{
	m_Items.clear();
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  添加一条新的数据项条目
// 作    者:  邵凯田
// 创建时间:  2015-10-9 17:29
// 参数说明:  @mmspath为节点全路径(如:PL2202APROT/LLN0$BR$brcbWarning01$RptID)
// 返 回 值:  stuSMmsDataNode*
//////////////////////////////////////////////////////////////////////////
stuSMmsDataNode* CSMmsDataset::AddItem(SString desc,SString mmspath)
{
	stuSMmsDataNode *p = new stuSMmsDataNode();
	p->sMmsPath = mmspath;
	m_Items.append(p);
	return p;
}

/****************  CSMmsLogicDevice  ****************/
CSMmsLogicDevice::CSMmsLogicDevice(SMmsClient *client)
{
	m_client = client;
	m_VarNames.setAutoDelete(true);
	user = NULL;
}

CSMmsLogicDevice::~CSMmsLogicDevice()
{
	m_VarNames.clear();
}

/********************************************  
* @brief 设置数据名称条目列表
* @author 宋康
* @date 2016/01/04
* @param int iTimeout_s 超时时间（默认10秒）
* @return true表示设置成功,false表示设置失败
*********************************************/ 
bool CSMmsLogicDevice::SetVarNameList(int iTimeout_s)
{
	ST_RET				ret;
	NAMELIST_REQ_INFO	getnam_req;
	NAMELIST_RESP_INFO	resp_info;

	getnam_req.cs_objclass_pres = SD_FALSE;
	getnam_req.obj.mms_class = MMS_CLASS_VAR;
	getnam_req.objscope = DOM_SPEC;
	getnam_req.cont_after_pres = SD_FALSE;
	strcpy(getnam_req.dname,m_sLdPath.data());

	ret = getVarName(&getnam_req,&resp_info,iTimeout_s);
	while (ret == SD_SUCCESS && resp_info.more_follows)
	{
		getnam_req.cont_after_pres = SD_TRUE;
		strcpy(getnam_req.continue_after,m_VarNames.at(m_VarNames.count()-1)->data());
		ret = getVarName(&getnam_req,&resp_info,iTimeout_s);
	}

	return true;
}

/********************************************  
* @brief 获取数据条目列表
* @author 宋康
* @date 2016/01/04
* @param void * req_info 请求信息，NAMELIST_REQ_INFO*
* @param void * resp_info 回复信息，NAMELIST_RESP_INFO*
* @param int iTimeout_s 超时时间
* @return int 是否获取成功，0表示成功，非0表示不成功
*********************************************/ 
ST_RET CSMmsLogicDevice::getVarName(void *req_info,void *resp_info,int iTimeout_s)
{
	ST_RET			ret;
	ST_CHAR			**nptr;
	MVL_REQ_PEND	*req_pend;
	MVL_NET_INFO	*m_mvl_net_info = (MVL_NET_INFO*)m_client->GetMVL_NET_INFO_A();

	if (!m_mvl_net_info)
		m_mvl_net_info = (MVL_NET_INFO*)m_client->GetMVL_NET_INFO_B();

	if (!m_mvl_net_info)
	{
		LOGFAULT("MVL_NET_INFO[0x%x] 错误!",m_mvl_net_info);
		return SD_FAILURE;
	}

	ret = mvla_getnam(m_mvl_net_info,(NAMELIST_REQ_INFO*)req_info,&req_pend);
	if (ret == SD_SUCCESS)
		ret = waitReqDone(req_pend,iTimeout_s);

	if (ret != SD_SUCCESS)
	{
		LOGFAULT("mvla_getnam error! ret = %d.",ret);
	}
	else
	{
		nptr = (ST_CHAR **)(req_pend->u.getnam.resp_info + 1);
		for (int i = 0; i < req_pend->u.getnam.resp_info->num_names; i++)
		{
			SString *s = new SString(nptr[i]);
			m_VarNames.append(s);
		}

		memcpy(resp_info,req_pend->u.getnam.resp_info,sizeof(NAMELIST_RESP_INFO));
	}

	mvl_free_req_ctrl(req_pend);

	return ret;
}

//数值转换附加函数
ST_RET parseRtArrStart(RUNTIME_TYPE *rt,SPtrList<MMSValue> &valueList)
{
	MMSValue * mmsVal = new MMSValue();

	mmsVal->setType(TYPE_ARRAY);
	mmsVal->setStrValue("[");
	valueList.append(mmsVal);

	return SD_SUCCESS;
}

ST_RET parseRtArrEnd(RUNTIME_TYPE *rt,SPtrList<MMSValue> &valueList)
{
	MMSValue * mmsVal = new MMSValue();

	mmsVal->setType(TYPE_ARRAY);
	mmsVal->setStrValue("]");
	valueList.append(mmsVal);

	return SD_SUCCESS;
}

ST_RET parseRtStrStart(RUNTIME_TYPE *rt,SD_CONST RUNTIME_TYPE *rt_head,SPtrList<MMSValue> &valueList)
{
	MMSValue * mmsVal = new MMSValue();

	mmsVal->setType(TYPE_STRUCT);
	mmsVal->setStrValue("{");
	valueList.append(mmsVal);

	return SD_SUCCESS;
}

ST_RET parseRtStrEnd(RUNTIME_TYPE *rt,SPtrList<MMSValue> &valueList)
{
	MMSValue * mmsVal = new MMSValue();

	mmsVal->setType(TYPE_STRUCT);
	mmsVal->setStrValue("}");
	valueList.append(mmsVal);

	return SD_SUCCESS;
}

ST_RET parseRtBool(ST_BOOLEAN *data_ptr,RUNTIME_TYPE *rt,SPtrList<MMSValue> &valueList,SString tdl,int &pos)
{
	ST_BOOLEAN *bptr = (ST_BOOLEAN *)data_ptr;
	ST_INT int_val = (ST_INT)(*bptr);

	MMSValue * mmsVal = new MMSValue();
	mmsVal->setType(TYPE_BOOL);
	if (int_val)
	{
		mmsVal->setStrValue("1");
		mmsVal->setBoolValue(true);
		mmsVal->setUint8Value(1);
	}
	else
	{
		mmsVal->setStrValue("0");
		mmsVal->setBoolValue(false);
		mmsVal->setUint8Value(0);
	}
	SString name = SString::GetIdAttribute(pos++,tdl,",");
	name = name.GetIdAttribute(1,name,")");
	name = name.GetIdAttribute(2,name,"(");
	mmsVal->setName(name);
	valueList.append(mmsVal);

	return SD_SUCCESS;
}

#define MMS_DATA_BUF_SIZE 20000
ST_RET parseRtBitString(ST_UCHAR *data_ptr,RUNTIME_TYPE *rt,SPtrList<MMSValue> &valueList,SString tdl,int &pos)
{
	int i,j,k,m = 0;
	int numBits = rt->u.p.el_len;
	ST_UCHAR mask;
	ST_CHAR text[MMS_DATA_BUF_SIZE] = {'\0'};

	/* We take 1 dest byte per bit, make sure it fits				*/
	if (numBits > MMS_DATA_BUF_SIZE-1)
	{
		printf("Bit String (%d bits) too long to encode", numBits);
		return (SD_FAILURE);
	}

	if (numBits < 0)		/* a variable length bit string	*/
	{
		numBits = *(ST_INT16 *)data_ptr;
		k = 2;
		m = 2;
	}
	else
		k = 0;

	for (i = 0; i < numBits; ++k)	/* for each byte, while bits remain	*/
	{
		mask = 0x80;
		for (j = 0; j < 8 && i < numBits; ++i, ++j)
		{
			if (data_ptr[k] & mask)
				text[i] = '1';
			else
				text[i] = '0';
			mask >>= 1;
		}
	}
	text[i] = 0;

	MMSValue * mmsVal = new MMSValue();
	mmsVal->setType(TYPE_BIT_STRING);
	mmsVal->setStrValue(SString::toFormat("%s",text));
	unsigned short q = *(ST_INT16*)(data_ptr+m);
	mmsVal->setUint16Value(q & 0x78ff);	//取13位
	SString name = SString::GetIdAttribute(pos++,tdl,",");
	name = name.GetIdAttribute(1,name,")");
	name = name.GetIdAttribute(2,name,"(");
	mmsVal->setName(name);
	valueList.append(mmsVal);

	return (SD_SUCCESS);
}

ST_RET parseRtInterge8(ST_INT8 *data_ptr, RUNTIME_TYPE *rt,SPtrList<MMSValue> &valueList,SString tdl,int &pos)
{
	MMSValue * mmsVal = new MMSValue();

	ST_INT8 val = (ST_INT8)(*data_ptr);
	mmsVal->setType(TYPE_INT8);
	mmsVal->setInt8Value(val);
	mmsVal->setStrValue(SString::toFormat("%d",val));
	SString name = SString::GetIdAttribute(pos++,tdl,",");
	name = name.GetIdAttribute(1,name,")");
	name = name.GetIdAttribute(2,name,"(");
	mmsVal->setName(name);
	valueList.append(mmsVal);

	return (SD_SUCCESS);
}

ST_RET parseRtInterge16(ST_INT16 *data_ptr, RUNTIME_TYPE *rt,SPtrList<MMSValue> &valueList,SString tdl,int &pos)
{
	MMSValue * mmsVal = new MMSValue();

	ST_INT16 val = (ST_INT16)(*data_ptr);
	mmsVal->setType(TYPE_INT16);
	mmsVal->setInt16Value(val);
	mmsVal->setStrValue(SString::toFormat("%d",val));
	SString name = SString::GetIdAttribute(pos++,tdl,",");
	name = name.GetIdAttribute(1,name,")");
	name = name.GetIdAttribute(2,name,"(");
	mmsVal->setName(name);
	valueList.append(mmsVal);

	return (SD_SUCCESS);
}

ST_RET parseRtInterge32(ST_INT32 *data_ptr, RUNTIME_TYPE *rt,SPtrList<MMSValue> &valueList,SString tdl,int &pos)
{
	MMSValue * mmsVal = new MMSValue();

	ST_INT32 val = (ST_INT32)(*data_ptr);
	mmsVal->setType(TYPE_INT32);
	mmsVal->setInt32Value(val);
	mmsVal->setStrValue(SString::toFormat("%d",val));
	SString name = SString::GetIdAttribute(pos++,tdl,",");
	name = name.GetIdAttribute(1,name,")");
	name = name.GetIdAttribute(2,name,"(");
	mmsVal->setName(name);
	valueList.append(mmsVal);

	return (SD_SUCCESS);
}

ST_RET parseRtInterge64(ST_INT64 *data_ptr, RUNTIME_TYPE *rt,SPtrList<MMSValue> &valueList,SString tdl,int &pos)
{
	MMSValue * mmsVal = new MMSValue();

	ST_INT64 val = (ST_INT64)(*data_ptr);
	mmsVal->setType(TYPE_INT64);
	mmsVal->setInt64Value(val);
#ifdef _WIN32
	mmsVal->setStrValue(SString::toFormat("%l64d",val));
#else
	mmsVal->setStrValue(SString::toFormat("%lld",val));
#endif
	SString name = SString::GetIdAttribute(pos++,tdl,",");
	name = name.GetIdAttribute(1,name,")");
	name = name.GetIdAttribute(2,name,"(");
	mmsVal->setName(name);
	valueList.append(mmsVal);

	return (SD_SUCCESS);
}

ST_RET parseRtUnsigned8(ST_UINT8 *data_ptr, RUNTIME_TYPE *rt,SPtrList<MMSValue> &valueList,SString tdl,int &pos)
{
	MMSValue * mmsVal = new MMSValue();

	ST_UINT8 val = (ST_UINT8)(*data_ptr);
	mmsVal->setType(TYPE_UINT8);
	mmsVal->setUint8Value(val);
	mmsVal->setStrValue(SString::toFormat("%d",val));
	SString name = SString::GetIdAttribute(pos++,tdl,",");
	name = name.GetIdAttribute(1,name,")");
	name = name.GetIdAttribute(2,name,"(");
	mmsVal->setName(name);
	valueList.append(mmsVal);

	return (SD_SUCCESS);
}

ST_RET parseRtUnsigned16(ST_UINT16 *data_ptr, RUNTIME_TYPE *rt,SPtrList<MMSValue> &valueList,SString tdl,int &pos)
{
	MMSValue * mmsVal = new MMSValue();

	ST_UINT16 val = (ST_UINT16)(*data_ptr);
	mmsVal->setType(TYPE_UINT16);
	mmsVal->setUint16Value(val);
	mmsVal->setStrValue(SString::toFormat("%d",val));
	SString name = SString::GetIdAttribute(pos++,tdl,",");
	name = name.GetIdAttribute(1,name,")");
	name = name.GetIdAttribute(2,name,"(");
	mmsVal->setName(name);
	valueList.append(mmsVal);

	return (SD_SUCCESS);
}

ST_RET parseRtUnsigned32(ST_UINT32 *data_ptr, RUNTIME_TYPE *rt,SPtrList<MMSValue> &valueList,SString tdl,int &pos)
{
	MMSValue * mmsVal = new MMSValue();

	ST_UINT32 val = (ST_UINT32)(*data_ptr);
	mmsVal->setType(TYPE_UINT32);
	mmsVal->setUint32Value(val);
	mmsVal->setStrValue(SString::toFormat("%d",val));
	SString name = SString::GetIdAttribute(pos++,tdl,",");
	name = name.GetIdAttribute(1,name,")");
	name = name.GetIdAttribute(2,name,"(");
	mmsVal->setName(name);
	valueList.append(mmsVal);

	return (SD_SUCCESS);
}

ST_RET parseRtUnsigned64(ST_UINT64 *data_ptr, RUNTIME_TYPE *rt,SPtrList<MMSValue> &valueList,SString tdl,int &pos)
{
	MMSValue * mmsVal = new MMSValue();

	ST_UINT64 val = (ST_UINT64)(*data_ptr);
	mmsVal->setType(TYPE_UINT64);
	mmsVal->setUint64Value(val);
#ifdef _WIN32
	mmsVal->setStrValue(SString::toFormat("%l64u",val));
#else
	mmsVal->setStrValue(SString::toFormat("%llu",val));
#endif
	SString name = SString::GetIdAttribute(pos++,tdl,",");
	name = name.GetIdAttribute(1,name,")");
	name = name.GetIdAttribute(2,name,"(");
	mmsVal->setName(name);
	valueList.append(mmsVal);

	return (SD_SUCCESS);
}

ST_RET parseRtFloat(ST_FLOAT *data_ptr, RUNTIME_TYPE *rt,SPtrList<MMSValue> &valueList,SString tdl,int &pos)
{
	MMSValue * mmsVal = new MMSValue();

	ST_FLOAT val = (ST_FLOAT)(*data_ptr);
	mmsVal->setType(TYPE_FLOAT);
	mmsVal->setFloatValue(val);
	mmsVal->setStrValue(SString::toFormat("%.3f",val));
	SString name = SString::GetIdAttribute(pos++,tdl,",");
	name = name.GetIdAttribute(1,name,")");
	name = name.GetIdAttribute(2,name,"(");
	mmsVal->setName(name);
	valueList.append(mmsVal);

	return (SD_SUCCESS);
}

ST_RET parseRtDouble(ST_DOUBLE *data_ptr, RUNTIME_TYPE *rt,SPtrList<MMSValue> &valueList,SString tdl,int &pos)
{
	MMSValue * mmsVal = new MMSValue();

	ST_DOUBLE val = (ST_DOUBLE)(*data_ptr);
	mmsVal->setType(TYPE_DOUBLE);
	mmsVal->setDoubleValue(val);
	mmsVal->setStrValue(SString::toFormat("%.5f",val));
	SString name = SString::GetIdAttribute(pos++,tdl,",");
	name = name.GetIdAttribute(1,name,")");
	name = name.GetIdAttribute(2,name,"(");
	mmsVal->setName(name);
	valueList.append(mmsVal);

	return (SD_SUCCESS);
}

ST_RET parseRtBCD1(ST_INT8 *data_ptr,RUNTIME_TYPE *rt,SPtrList<MMSValue> &valueList,SString tdl,int &pos)
{
	ST_UCHAR *ubyteptr;
	ST_INT int_val;

	ubyteptr = (ST_UCHAR *) data_ptr;
	int_val = (ST_INT) (*ubyteptr);

	MMSValue * mmsVal = new MMSValue();
	mmsVal->setStrValue(SString::toFormat("%d",int_val));
	SString name = SString::GetIdAttribute(pos++,tdl,",");
	name = name.GetIdAttribute(1,name,")");
	name = name.GetIdAttribute(2,name,"(");
	mmsVal->setName(name);
	valueList.append(mmsVal);

	return (SD_SUCCESS);
}

ST_RET parseRtBCD2(ST_INT16 *data_ptr,RUNTIME_TYPE *rt,SPtrList<MMSValue> &valueList,SString tdl,int &pos)
{
	ST_INT i;
	ST_INT16 *sp;

	sp = (ST_INT16 *) data_ptr;
	i = *sp;

	MMSValue * mmsVal = new MMSValue();
	mmsVal->setStrValue(SString::toFormat("%d",i));
	SString name = SString::GetIdAttribute(pos++,tdl,",");
	name = name.GetIdAttribute(1,name,")");
	name = name.GetIdAttribute(2,name,"(");
	mmsVal->setName(name);
	valueList.append(mmsVal);

	return (SD_SUCCESS);
}

ST_RET parseRtBCD4(ST_INT32 *data_ptr,RUNTIME_TYPE *rt,SPtrList<MMSValue> &valueList,SString tdl,int &pos)
{
	MMSValue * mmsVal = new MMSValue();
	mmsVal->setStrValue(SString::toFormat("%d",*((ST_INT32 *)data_ptr)));
	SString name = SString::GetIdAttribute(pos++,tdl,",");
	name = name.GetIdAttribute(1,name,")");
	name = name.GetIdAttribute(2,name,"(");
	mmsVal->setName(name);
	valueList.append(mmsVal);

	return (SD_SUCCESS);
}

ST_RET parseRtVisibleString(ST_CHAR *datptr,RUNTIME_TYPE *rt,SPtrList<MMSValue> &valueList,SString tdl,int &pos)
{
	MMSValue * mmsVal = new MMSValue();
	SString str = SString::toFormat("%s",datptr);
	mmsVal->setStrValue(str);
	SString name = SString::GetIdAttribute(pos++,tdl,",");
	name = name.GetIdAttribute(1,name,")");
	name = name.GetIdAttribute(2,name,"(");
	mmsVal->setName(name);
	valueList.append(mmsVal);

	return (SD_SUCCESS);
}

ST_RET parseRtGeneralTime(time_t *data_ptr, RUNTIME_TYPE *rt,SPtrList<MMSValue> &valueList,SString tdl,int &pos)
{
	MMSValue * mmsVal = new MMSValue();
	SString str = SString::toFormat("%s",ctime(data_ptr));
	mmsVal->setStrValue(str);
	SString name = SString::GetIdAttribute(pos++,tdl,",");
	name = name.GetIdAttribute(1,name,")");
	name = name.GetIdAttribute(2,name,"(");
	mmsVal->setName(name);
	valueList.append(mmsVal);

	return (SD_SUCCESS);
}

ST_RET parseRtBTime(MMS_BTIME6 *data_ptr, RUNTIME_TYPE *rt,SPtrList<MMSValue> &valueList,SString tdl,int &pos)
{
	MMSValue * mmsVal = new MMSValue();
	SString str;
	str.sprintf("BINARY TIME day=%lu, ms=%lu",
		(long) data_ptr->day, (long) data_ptr->ms);

	mmsVal->setUint32Value((long)data_ptr->day);
	mmsVal->setInt32Value((long)data_ptr->ms);
	mmsVal->setType(TYPE_BINARY_TIME);
	SString name = SString::GetIdAttribute(pos++,tdl,",");
	name = name.GetIdAttribute(1,name,")");
	name = name.GetIdAttribute(2,name,"(");
	mmsVal->setName(name);
	valueList.append(mmsVal);

	return SD_SUCCESS;
}

ST_RET parseRtUtcTime(MMS_UTC_TIME *data_ptr, RUNTIME_TYPE *rt,SPtrList<MMSValue> &valueList,SString tdl,int &pos)
{
	MMSValue * mmsVal = new MMSValue();
	SString str;
	str.sprintf("UTC TIME seconds=%lu, fraction=%lu, qflags=%lu",
		(long) data_ptr->secs, (long) data_ptr->fraction, (long) data_ptr->qflags);
	mmsVal->setStrValue(str);
	
	int usec = (int)((((double)data_ptr->fraction)/0x01000000) * 1000000);
	usec = ((usec+500)/1000)*1000;	//29999微秒变为30000微秒
	mmsVal->setUint32Value((long)data_ptr->secs);
	mmsVal->setInt32Value(usec);
	mmsVal->setUint8Value((unsigned char)data_ptr->qflags);
	mmsVal->setType(TYPE_UTC_TIME);
	SString name = SString::GetIdAttribute(pos++,tdl,",");
	name = name.GetIdAttribute(1,name,")");
	name = name.GetIdAttribute(2,name,"(");
	mmsVal->setName(name);
	valueList.append(mmsVal);

	return SD_SUCCESS;
}

ST_RET parseRtUtf8String(ST_CHAR *data_ptr,RUNTIME_TYPE *rt,SPtrList<MMSValue> &valueList,SString tdl,int &pos)
{
	char text[512] = {'\0'};

#ifdef WIN32
	UTF16ToGB2312(data_ptr,text);
#else
	int strLen = 0;
	for (int i = 0; i < 512; ++i)
	{
		++strLen;
		if (data_ptr[i] == 0x00)
			break;
	}

	// 传入第二个参数一定要是字符串长度，否则转换错误
	convert_utf8_gb2312_ex(data_ptr, strLen, text, 512);
#endif

	MMSValue * mmsVal = new MMSValue();
	SString str = SString::toFormat("%s",text);
	mmsVal->setStrValue(str);
	mmsVal->setType(TYPE_UTF8_STRING);
	SString name = SString::GetIdAttribute(pos++,tdl,",");
	name = name.GetIdAttribute(1,name,")");
	name = name.GetIdAttribute(2,name,"(");
	mmsVal->setName(name);
	valueList.append(mmsVal);

	return SD_SUCCESS;
}

int parseData(ST_CHAR *datptr,SD_CONST RUNTIME_TYPE *rt_head,ST_INT rt_num,SPtrList<MMSValue> &valueList,char *tdl)
{
	RUNTIME_TYPE *rt_ptr;
	RUNTIME_TYPE *rt_end;
	ST_RET uDataRet;
	ST_INT arr_loop_level;
	ST_INT arr_loops[ASN1_MAX_LEVEL];
	SString m_tdl = tdl;
	int	tdl_pos = 1;

	arr_loop_level = 0;

	rt_ptr = (RUNTIME_TYPE *) rt_head;  	/* point to head rt_block	*/
	rt_end = rt_ptr + rt_num; 	/* done when pointer is here		*/

	uDataRet = SD_SUCCESS;
	while (rt_ptr < rt_end && uDataRet == SD_SUCCESS)
	{
		if (rt_ptr->el_tag == RT_ARR_END)	/* treat case of array ending	*/
		{
			if (--arr_loops[arr_loop_level] > 0) /* if need to do next ar elmnt	*/
				rt_ptr -= rt_ptr->u.arr.num_rt_blks; /* mv rt_ptr to start of arr	*/
			else
				--arr_loop_level;
		}
		if (rt_ptr->el_tag == RT_ARR_START)	/* treat case of array starting	*/
		{
			/* initialize the loop counter for the array				*/
			++arr_loop_level;
			arr_loops[arr_loop_level] = rt_ptr->u.arr.num_elmnts;
		}

		switch (rt_ptr->el_tag)
		{
		case RT_ARR_START:
			uDataRet = parseRtArrStart(rt_ptr,valueList);
			break;
		case RT_STR_START:
			uDataRet = parseRtStrStart(rt_ptr,rt_head,valueList);
			break;
		case RT_ARR_END:
			uDataRet = parseRtArrEnd(rt_ptr,valueList);
			break;
		case RT_STR_END:
			uDataRet = parseRtStrEnd(rt_ptr,valueList);
			break;
		case RT_BOOL:
			uDataRet = parseRtBool((ST_BOOLEAN *)datptr,rt_ptr,valueList,m_tdl,tdl_pos);
			break;
		case RT_BIT_STRING :
			uDataRet = parseRtBitString((ST_UCHAR *)datptr,rt_ptr,valueList,m_tdl,tdl_pos);
			break;
		case RT_INTEGER:			
			switch (rt_ptr->u.p.el_len)	/* determine length		*/
			{
			case 1 :			/* one byte int 		*/
				uDataRet = parseRtInterge8((ST_INT8 *)datptr,rt_ptr,valueList,m_tdl,tdl_pos);
				break;
			case 2 :			/* two byte int 		*/
				uDataRet = parseRtInterge16((ST_INT16 *)datptr,rt_ptr,valueList,m_tdl,tdl_pos);
				break;
			case 4 :			/* four byte integer		*/
				uDataRet = parseRtInterge32((ST_INT32 *)datptr,rt_ptr,valueList,m_tdl,tdl_pos);
				break;
#ifdef INT64_SUPPORT
			case 8 :			/* eight byte integer		*/
				uDataRet = parseRtInterge64((ST_INT64 *)datptr,rt_ptr,valueList,m_tdl,tdl_pos);
				break;
#endif	
			}
			break;
		case RT_UNSIGNED: 			
			switch (rt_ptr->u.p.el_len)	/* determine length		*/
			{
			case 1 :			/* one byte int 		*/
				uDataRet = parseRtUnsigned8((ST_UINT8 *) datptr,rt_ptr,valueList,m_tdl,tdl_pos);
				break;
			case 2 :			/* two byte int 		*/
				uDataRet = parseRtUnsigned16((ST_UINT16 *) datptr,rt_ptr,valueList,m_tdl,tdl_pos);
				break;
			case 4 :			/* four byte integer		*/
				uDataRet = parseRtUnsigned32((ST_UINT32 *) datptr,rt_ptr,valueList,m_tdl,tdl_pos);
				break;
#ifdef INT64_SUPPORT
			case 8 :			/* eight byte integer		*/
				uDataRet = parseRtUnsigned64((ST_UINT64 *) datptr,rt_ptr,valueList,m_tdl,tdl_pos);
				break;
#endif
			}
			break;
#ifdef FLOAT_DATA_SUPPORT
		case RT_FLOATING_POINT: 
			if (rt_ptr->u.p.el_len != sizeof (ST_FLOAT))
				uDataRet = parseRtDouble((ST_DOUBLE *) datptr,rt_ptr,valueList,m_tdl,tdl_pos);
			else
				uDataRet = parseRtFloat((ST_FLOAT *) datptr,rt_ptr,valueList,m_tdl,tdl_pos);
			break;
#endif
		case RT_OCTET_STRING : 		
			//			if (ac->oct)
			//				uDataRet = (*ac->oct) (usr, (ST_UCHAR *) datptr, rt_ptr);
			break;
		case RT_VISIBLE_STRING:
			uDataRet = parseRtVisibleString((ST_CHAR *)datptr,rt_ptr,valueList,m_tdl,tdl_pos);
			break;
#ifdef TIME_DATA_SUPPORT
		case RT_GENERAL_TIME:	
			uDataRet = parseRtGeneralTime((time_t *)datptr,rt_ptr,valueList,m_tdl,tdl_pos);
			break;
#endif
#ifdef BTOD_DATA_SUPPORT
		case RT_BINARY_TIME : 		
			//			switch (rt_ptr->u.p.el_len)	/* determine length		*/
			//			{
			//			case 4:
			//				if (ac->bt4)
			//					uDataRet = (*ac->bt4) (usr, (ST_INT32 *) datptr, rt_ptr);
			//				break;
			//			case 6:
			//				if (ac->bt6)
			//					uDataRet = (*ac->bt6) (usr, (ST_INT32 *) datptr, rt_ptr);
			//				break;
			//			}
			uDataRet = parseRtBTime((MMS_BTIME6 *)datptr,rt_ptr,valueList,m_tdl,tdl_pos);	// add by qiancl 20180518
			break;
#endif
		case RT_BCD : 			
			if (rt_ptr->u.p.el_len <= 2)
				uDataRet = parseRtBCD1((ST_INT8 *)datptr,rt_ptr,valueList,m_tdl,tdl_pos);
			else if (rt_ptr->u.p.el_len <= 4)
				uDataRet = parseRtBCD2((ST_INT16 *)datptr,rt_ptr,valueList,m_tdl,tdl_pos);
			else if (rt_ptr->u.p.el_len <= 8)
				uDataRet = parseRtBCD4((ST_INT32 *)datptr,rt_ptr,valueList,m_tdl,tdl_pos);
			break;
		case RT_UTC_TIME:
			uDataRet = parseRtUtcTime((MMS_UTC_TIME *)datptr,rt_ptr,valueList,m_tdl,tdl_pos);
			break;
		case RT_UTF8_STRING :
			uDataRet = parseRtUtf8String((ST_CHAR *)datptr,rt_ptr,valueList,m_tdl,tdl_pos);
			break;
		default:
			uDataRet = -1;
			break;
		}

		datptr += rt_ptr->el_size; 	/* Adjust data pointer		*/
		rt_ptr++;  					/* point to next rt element	*/
	}

	return (uDataRet);
}

ST_RET named_vars_read (MVL_NET_INFO *net_info,SPtrList<SString> &var_name,ST_INT scope,
	SPtrList<SString> &dom_name,SPtrList<var_dt_t> &vdt,ST_INT timeOut)
{
	ST_INT ret = SD_SUCCESS;
	READ_REQ_INFO *req_info;
	VARIABLE_LIST *vl;
	MVL_REQ_PEND *reqCtrl;
	ST_INT i,num_data = dom_name.count();
	ST_CHAR *buffer = new ST_CHAR[512*num_data];
	//MVL_READ_RESP_PARSE_INFO *parse_info = new MVL_READ_RESP_PARSE_INFO[num_data];
	MVL_READ_RESP_PARSE_INFO parse_info[MAX_READ_VALUES_NUMBER] = {0};

	/* Create a read request info struct  */
	req_info = (READ_REQ_INFO *) buffer;
	req_info->spec_in_result = SD_FALSE;
	req_info->va_spec.var_acc_tag = VAR_ACC_VARLIST;
	req_info->va_spec.num_of_variables = num_data;

	memset (parse_info, 0, sizeof (MVL_READ_RESP_PARSE_INFO) * MAX_READ_VALUES_NUMBER);
	for (i = 0;i < num_data; i++)
	{
		vl = (VARIABLE_LIST *) (req_info + 1);
		vl += i;
		vl->alt_access_pres = SD_FALSE;
		vl->var_spec.var_spec_tag = VA_SPEC_NAMED;
		vl->var_spec.vs.name.object_tag = scope;	/* set scope	*/
		if (scope == DOM_SPEC)
			vl->var_spec.vs.name.domain_id = dom_name.at(i)->data();	/* set domain name	*/
		vl->var_spec.vs.name.obj_name.vmd_spec = var_name.at(i)->data();

		//memset (&parse_info[i],0,sizeof(parse_info));
		parse_info[i].dest = vdt.at(i)->data;
		parse_info[i].type_id = vdt.at(i)->type_id;
		parse_info[i].descr_arr = SD_FALSE;
	}

	/* Send read request. */
	ret = mvla_read_variables (net_info,req_info,num_data,parse_info, &reqCtrl);

	/* If request sent successfully, wait for reply.	*/
	if (ret == SD_SUCCESS)
		ret = waitReqDone (reqCtrl, timeOut);
	else
	{
		LOGFAULT("读多数值，mvla_read_variables返回错误，ret=0x%x!",ret);
	}

	mvl_free_req_ctrl (reqCtrl);
	if (ret != SD_SUCCESS)
	{
		delete [] buffer;
		//delete [] parse_info;
		LOGFAULT("读多数值，waitReqDone返回错误，ret=0x%x!",ret);
		return (ret);
	}
	
	for (i = 0;i < num_data; i++)
	{
		if (parse_info[i].result != SD_SUCCESS)
		{
			LOGFAULT("读多数值，parse_info[%d].type_id=%d,返回[result=0x%x]异常。",
				i,parse_info[i].type_id,parse_info[i].result);
			ret = parse_info[i].result;
			break;
		}
	}
	delete [] buffer;
	//delete [] parse_info;

	return ret;
}

bool SMmsClient::WaitSystemTaskFinish(int timeout)
{
	int times = timeout / 500;

	for (int i = 0; i < times; ++i)
	{
		if (!m_isSystemTaskProcessing)
			return true;
		SApi::UsSleep(500000);
	}
	
	return false;
}

bool SMmsClient::IsUserTaskProcessing()
{ 
	return m_isUserTaskProcessing;
}