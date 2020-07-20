/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	1986 - 2008, All Rights Reserved		        */
/*									*/
/* MODULE NAME : server.c    						*/
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
/* 02/24/11  RKR           Added code to set type_ctrl in mnfg callbacks*/
/* 11/17/09  JRB     75    Del HARD_CODED_CFG example.			*/
/*			   Del MAP30_ACSE support.			*/
/*			   Del refs to secManCfgXmlFile (obsolete).	*/
/* 05/21/08  JRB     74    Keep track of open files, close files on abort,*/
/*			   don't allow conclude if files open.		*/
/*			   Don't allow conclude if requests pending.	*/
/* 05/14/08  GLB     73    Added slog_ipc_std_cmd_service		*/
/* 01/03/08  LWP     72	   Memchk statistics no longer crash when written*/
/* 03/16/07  JRB     71	   Fix a few more function calls.		*/
/* 03/14/07  JRB     70    Disable printfs in ..frename_ind, ..obtfile_ind.*/
/* 10/31/06  JRB     69    u_gnl_ind*, u_mvl_get_va_aa: add args.	*/
/*			   Use new mvl_vmd_* object handling functions.	*/
/*			   Add #if around read/write indication code.	*/
/* 06/23/06  MDE     68    Added time_t cast				*/
/* 10/25/05  EJV     67    S_SEC_ENABLED: added identify_response_info.	*/
/*			   Call slog_start, slog_end, slogIpcEvent.	*/
/* 09/09/05  JRB     66    Don't print BUILD_NUM.			*/
/* 08/05/05  EJV     65    Added ssleLogMaskMapCtrl.			*/
/* 06/06/05  EJV     64    gsLogMaskMapCtrl not on QNX.			*/
/* 05/24/05  EJV     63    Added logCfgAddMaskGroup (&xxxLogMaskMapCtrl)*/
/*			   Use logcfgx_ex to parse logging config file.	*/
/* 05/20/05  JRB     62    Don't print "Check log file". May not be one.*/
/* 05/20/05  JRB     61    Fix keyboard input: Use getch on WIN32, QNX.	*/
/*			   Use term_init, term_rest, getchar elsewhere.	*/
/* 03/28/05  JRB     60    Add lean_a hdr (indirectly included before).	*/
/*			   Use mvlas_fdir_resp for _WIN32.		*/
/* 08/13/04  JRB     59    Del unneeded includes.			*/
/* 08/10/04  EJV     58    u_mvl_connect_ind_ex: typecasted ret		*/
/*			   secManCfgXmlFile: all lowcase "secmancfg.xml"*/
/* 08/06/04  ASK     57    Change path to secManCfg.xml for !WIN32	*/
/* 07/08/04  JRB     56    Del mvl_local_cl_ar_name.			*/
/* 01/20/04  EJV     55    Don't call ulFreeAssocSecurity on disconnect.*/
/* 01/06/04  EJV     54    Added security support (S_SEC_ENABLED code)	*/
/*			   Added LITESECURE version printf.		*/
/*			   Added kbService func. Deleted Sleep().	*/
/* 12/11/03  JRB     53    DON'T use MVLLOG_REQ/RESP/IND/CONF, they	*/
/*			   don't exist.					*/
/* 08/25/03  EJV     52    Chg u_mvl_connect_ind_ex.  			*/
/*			   Init UTF8Vstring13test in init_mms_objects.	*/
/* 07/14/03  JRB     51    Del dyn_mem_ptr_statistics call in ctrlCfun,	*/
/*			   don't want to log from signal handler.	*/
/* 04/23/03  JRB     50    Add sample UTF8Vstring var (UTF8Vstring13test).*/
/* 04/14/03  JRB     49    Chg main return from void to "int".		*/
/*			   Eliminate compiler warnings.			*/
/* 12/16/02  JRB     48    Add (MVL_NET_INFO*) arg to mvl_get_va_asn1_data*/
/*			   Update UtcTimeNow each time through loop.	*/
/* 10/17/02  ASK     47    Added ACSE authentication sample code.	*/
/* 08/09/02  JRB     46    Fix UTC time sample code. Fix printf.	*/
/* 04/01/02  JRB     45    init_named_vars return SUCCESS or err code.	*/
/* 03/06/02  JRB     44    Del ADLC code.				*/
/* 02/25/02  MDE     43    Now get max PDU size from mvl_cfg_info	*/
/* 02/25/02  MDE     42    MAP30_ACSE cleanup				*/
/* 02/08/02  JRB     41    Pass MVL_CFG.. to osicfgx & mvl_start_acse,	*/
/*			   del mvl_init_mms.. call & mvl_local_ar.. var.*/
/* 02/04/02  JRB     40    Del use of MVL_NUM_CALL*, COACSE defines.	*/
/*			   Del CLACSE info reports (never used).	*/
/* 01/24/02  EJV     39    Added DOM_SPEC journals			*/
/* 11/26/01  EJV     38    Added UtcTimeNow variable.			*/
/* 10/09/01  JRB     37    Use new MMSLITE_* defines for product ID.	*/ 
/* 05/25/01  RKR     36    Updated version to 4.01P12			*/
/* 01/05/00  MDE     35    Added MAP30_ACSE code			*/
/* 11/27/00  JRB     34    Use old CFG files for Reduced Stack & Trim-7.*/
/* 11/07/00  GLB     33    Added XML configuration files                */
/* 10/25/00  JRB     32    Del u_mvl & u_gnl funct ptrs.		*/
/*			   Add u_mvl_getcl_ind & u_mvl_concl_ind.	*/
/* 07/17/00  RKR     31    Updated version to 4.01P6			*/
/* 05/18/00  RKR     30    Took out a //				*/
/* 05/12/00  RKR     29    Added support for info reports		*/
/* 05/02/00  RKR     28    Added va_scope to manufactured NVL		*/
/* 05/01/00  RKR     27    Updated version to 4.02			*/
/* 03/17/00  RKR     26    Added mem_chk heap stats as MMS variables	*/
/* 01/25/00  MDE     25    Use M_MALLOC for asn1 encode buffer		*/
/* 12/20/99  GLB     24    Added setbuf                                 */
/* 10/28/99  RKR     23    Added mvl_clr_ind_que to disc_ind_fun 	*/
/* 10/06/99  GLB     22    Added file Rename and Obtain File            */
/* 03/19/99  RKR     21    Changed server to local1 			*/
/* 03/08/99  EJV     20    Replaced | with || in if statement (line 332)*/
/* 02/04/99  JRB     19    Send correct file info in mplas_fopen_resp.	*/
/* 01/22/99  JRB     18    Use "mvl_find_*" to find objects by name.	*/
/* 11/13/98  JRB     17    Wait for event before calling mvl_comm_serve	*/
/*			   if stack is MOSI (i.e. osi???e.lib used).	*/
/* 10/13/98  JRB     16    Move most Journal code to MVL library.	*/
/*			   Chg journal "entry_id" from ptr to array.	*/
/*			   Call read_log_cfg_file to set up logging.	*/
/* 09/21/98  MDE     15    Minor lint cleanup				*/
/* 09/16/98  MDE     14    Foundry creates Journals			*/
/* 09/11/98  JRB     13    Model Journal as linked list.		*/
/* 08/12/98  MDE     12    Don't include mvl_defs.h (in mvl_acse.h)	*/
/* 07/16/98  MDE     11    More journal work				*/
/* 07/15/98  MDE     10    Changed Journal handling per new MVL API	*/
/* 05/15/98  JRB     09    Added several new Indication functions.	*/
/*			   Make "u_mvl_*_ind" external, always called.	*/
/*			   Del mvl_ident_resp_info, mvl_status_resp_info*/
/* 05/14/98  MDE     08    Removed 'ml_init_buffers' call		*/
/* 04/07/98  MDE     07    Added UCA_SMP ifdef around smp_init call	*/
/* 12/29/97  MDE     06    Now use 'mktypes' for configured MMS objects	*/
/* 12/29/97  MDE     05    Added ^c handler, removed keyboard stuff,	*/
/* 			   removed program arguments			*/
/* 12/10/97  MDE     04    Added VXWORKS, Journal Read/Init		*/
/* 10/09/97  MDE     03    Minor cleanups				*/
/* 09/18/97  MDE     02    Fixed getnamelist code, misc. minor		*/
/* 09/10/97  MDE     01    MMS-LITE V4.0 Release			*/
/************************************************************************/

#if 0
  /* move outside #if to enable	*/
  #define USE_MANUFACTURED_OBJS
  #define ACSE_AUTH_ENABLED	/* for password authentication without	*/
				/* security				*/
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
#include "mem_chk.h"

#include "mvl_acse.h"
#include "mvl_log.h"
#include "srvrobj.h"
#include "tp4api.h"
#include "server.h"
#include "mmsop_en.h"
#include "lean_a.h"	/* for DIB_ENTRY, loc_dib_table, etc.	*/

#include <errno.h>

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

ST_RET read_log_cfg_file (ST_CHAR *filename);
ST_VOID wait_any_event (ST_LONG max_ms_delay);

/************************************************************************/
/* MMS SERVER VARIABLES							*/
/* These are variables that are visible to MMS Clients			*/
/* In this application we have both 'configured' and 'manufactured' 	*/
/* variables and variable lists.					*/
/*									*/
/* Configured objects are those that are configured in the 		*/
/* 'MVL_VMD_CTRL' data structure and which MVL can handle transparently	*/
/* if desired. This is the simplest way to handle variable access.	*/
/*									*/
/* Manufactured objects are those that are NOT configured in the	*/
/* 'MVL_VMD_CTRL' data structure, and so the user must resolve the MMS	*/
/* object to local mapping dynamically. This takes a bit more work than	*/
/* using configured objects, but can be useful under some conditions.	*/

/* It is possible to mix configured and manufactured objects, and this	*/
/* sample application does just that.					*/

/* CONFIGURED OBJECTS							*/
/* VMD Scope */
ST_INT16 arr1[10];	/* ARR1_TYPEID	TDL="[10:Short]"		*/
ST_INT16 Temperature;	/* I16_TYPEID 	TDL="Short"			*/
STRUCT1 struct1;	/* STRUCT1_TYPEID	TDL="{(s)Short,(l)Long}"	*/
MMS_UTC_TIME UtcTimeNow;	/* New MMS Type: Utctime		*/
#if (UNICODE_LOCAL_FORMAT==UNICODE_UTF16)
ST_UINT16 UTF8Vstring13test [13+1];	/* local representation is UTF16*/
#else
ST_CHAR UTF8Vstring13test [13+1];	/* local representation is UTF8*/
#endif
ST_INT localUTF8len;	/* len of UTF8 string in "bytes"	*/

/* Domain Scope */
ST_INT16 domArr1[10];
ST_INT16 domTemperature;
STRUCT1 domStruct1;

/* Note: The individual elements of 'struct1' can also be accessed as:	*/
/* struct1$s   (struct1.s)  		I16_TYPEID			*/
/* struct1$l   (struct1.l)	 	I32_TYPEID			*/
/* The named var list 'nvl1' contains arr1, struct1, and Temperature	*/

MVL_VAR_ASSOC *struct1s_va;
MVL_VAR_ASSOC *struct1l_va;

/* AA SCOPE OBJECTS */
ST_INT16 rptCtrl[20];	/* DEBUG: array size must be > mvl_num_called+mvl_num_called*/

/* MANUFACTURED OBJECTS							*/
/* ST_INT16 arr1_dyn[10];	 ARR1_TYPEID	TDL="[10:Short]"	*/
/* ST_INT16 Temperature_dyn;	 I16_TYPEID 	TDL="Short"		*/
/* STRUCT1 struct1_dyn;		 STRUCT1_TYPEID	TDL="{(s)Short,(l)Long}"*/

/* Note: The individual elements of 'struct1' can also be accessed as:	*/
/* struct1$s_dyn   (struct1.s) 		I16_TYPEID			*/
/* struct1$l_dyn   (struct1.l)	 	I32_TYPEID			*/
/* The named var list 'nvl1_dyn' contains arr1_dyn, struct1_dyn, and 	*/
/* Temperature_dyn.							*/

/* Note that we use the variables below as the source for the 		*/
/* manufactured variable's data values.					*/
#if defined (USE_MANUFACTURED_OBJS)
ST_INT16 lastDynTemp = 32;
ST_INT16 lastArr1StartVal = 30;
ST_INT16 lastStr1S = 30;
ST_INT32 lastStr1L = 300;
#endif

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
IDENT_RESP_INFO identify_response_info =
  {
  "SISCO",  		/* Vendor 	*/
  MMSLITE_NAME,  	/* Model  	*/
  MMSLITE_VERSION,  0,	/* Version 	*/
  };
#endif

/* Status server information						*/
STATUS_RESP_INFO status_resp_info =
  {
  0,		/* logical_stat 	*/
  0, 		/* physical_stat 	*/
  0, 		/* local_detail_pres 	*/
  };

/************************************************************************/
/************************************************************************/
/* Functions in this module						*/

/* Initialization */
static ST_VOID init_acse (ST_VOID);
static ST_VOID init_mms_objects (ST_VOID);
static ST_RET init_named_vars (ST_VOID);
static ST_VOID init_journals (ST_VOID);
static ST_RET fill_journal_var_info (VAR_INFO *var_info, ST_CHAR *var_tag,
                             MVL_VAR_ASSOC *va);
static ST_VOID init_log_cfg (ST_VOID);
static ST_VOID init_mem (ST_VOID);

/* Manufactured Variable Support */
#if defined (USE_MANUFACTURED_OBJS)
static MVL_VAR_ASSOC *get_va_aa (ST_INT service, ST_CHAR *name,
		      	   ST_BOOLEAN alt_access_pres,
		      	   ALT_ACCESS *alt_acc);
static MVL_VAR_ASSOC *get_va (ST_INT service, ST_CHAR *name);
static MVL_NVLIST_CTRL *get_nvl1_nvl (ST_INT service);
static MVL_VAR_ASSOC *get_Temperature_va (ST_INT service);
static MVL_VAR_ASSOC *get_arr1_va (ST_INT service);
static MVL_VAR_ASSOC *get_struct1_va (ST_INT service);
static MVL_VAR_ASSOC *get_struct1_s_va (ST_INT service);
static MVL_VAR_ASSOC *get_struct1_l_va (ST_INT service);
#endif /* USE_MANUFACTURED_OBJS */

/* Async Server Response Support */
static ST_VOID pend_ind_service (ST_VOID);

/* Connection Management */
static ST_INT connect_ind_fun (MVL_NET_INFO *cc, INIT_INFO *init_info);
static ST_VOID disc_ind_fun (MVL_NET_INFO *cc, ST_INT discType);

/* Misc. */
static ST_VOID send_info_reports (ST_INT now);

static ST_VOID kbService (ST_VOID);
void ctrlCfun (int);

/************************************************************************/
/************************************************************************/
/* General program control variables */

ST_BOOLEAN sendInfoRpts = SD_FALSE;
ST_BOOLEAN asyncResponse = SD_FALSE;
ST_LONG infoRptCount;
ST_LONG infoRptPeriod = 10;
time_t serverStartTime;
time_t asyncRespTime = 0;
static ST_BOOLEAN doIt = SD_TRUE;

/************************************************************************/
/************************************************************************/
/*                       main						*/
/************************************************************************/

#if defined (VXWORKS)	/* Just make this look like "main" call.	*/
int mmsserver ()
#else	/* !VXWORKS	*/  
int main ()
#endif	/* !VXWORKS	*/  
  {
ST_RET        ret;

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
#if defined(UCA_SMP)
  smp_init ();
#endif

  init_log_cfg ();	/* Set up logging subsystem			*/

  SLOGALWAYS2 ("%s Version %s", MMSLITE_NAME, MMSLITE_VERSION);
  SLOGALWAYS0 ("Initializing ...");

  init_acse ();		/* Start the lower layer subsystem		*/
  init_mms_objects ();	/* Set up MMS objects				*/
  init_journals ();     /* We use DIB info here so must init_acse 1st   */

  SLOGALWAYS0 ("Initialization complete, entering service loop");

/* Set the ^c catcher */
  signal (SIGINT, ctrlCfun);

/* We are all initialized, just service communications			*/
  serverStartTime = time(NULL);
  printf ("\n Entering 'server' mode, hit ^c to exit ... ");
  chk_malloc(1);
  dyn_mem_ptr_statistics (0);	
#if !defined(NO_KEYBOARD) && !defined(_WIN32) && !defined(__QNX__)
  term_init ();	/* makes getchar work right	*/
#endif
  while (doIt)
    {
#if defined (MOSI)		/* If stack is MOSI, use events		*/
    wait_any_event (1000);	/* Wait 1000 milliseconds		*/
    
    while (mvl_comm_serve ())	/* Perform communications service 	*/
      {				/* Loop until "mvl_comm_serve" returns 0*/
      }
#else
    mvl_comm_serve ();		/* Perform communications service 	*/
#endif
    pend_ind_service ();	/* Service async responses		*/

    if (sendInfoRpts == SD_TRUE)
      send_info_reports (SD_FALSE);

    UtcTimeNow.secs = (ST_UINT32) time(NULL);	/* Update UTC time	*/
					/* leave fraction,qflags alone*/
    kbService ();
#if defined(S_SEC_ENABLED)
    /* check for security configuration updates */
    if (ulCheckSecurityConfiguration () != SD_SUCCESS)
      printf("\n Security Reconfiguration failed\n");
#endif /* defined(S_SEC_ENABLED) */

#if defined(DEBUG_SISCO)
    if (sLogCtrl->logCtrl & LOG_IPC_EN)
      slogIpcEvent ();	/* required for IPC Logging if gensock2.c is	*/
    			/* not compiled with GENSOCK_THREAD_SUPPORT	*/
      
    /* At runtime, periodically need to service SLOG commands and calling connections. */
    /* The timing of this service is not critical, but to be responsive a default of   */
    /* 100ms works well.                                                               */
    slog_ipc_std_cmd_service ("logcfg.xml", NULL, NULL, SD_TRUE,  NULL, NULL);
#endif
    }

  dyn_mem_ptr_statistics (0);	
  mvl_end_acse ();		/* Stop the lower layer subsystem	*/
  dyn_mem_ptr_status ();	/* Log memory allocation usage		*/

#if defined(DEBUG_SISCO)
  /* terminate logging services and save current log file position */
  slog_end (sLogCtrl);
#endif

  printf ("\n\n");
  printf ("\n\n");
#if !defined(NO_KEYBOARD) && !defined(_WIN32) && !defined(__QNX__)
  term_rest ();	/* Must be called before exit if term_init used.	*/
#endif
  return (0);
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
#if defined(S_SEC_ENABLED)
#if !defined(NO_KEYBOARD)
ST_CHAR c;

  if (kbhit ())		/* Report test keyboard input */
    {
#if defined(_WIN32) || defined(__QNX__)
    c = getch ();
#else
    c = (ST_CHAR) getchar ();	/* works only if term_init called first	*/
#endif

    if (c == 'u')
      secManCfgChange();

    if (c == '?')
      {
      printf ("\n u : Update Security Configuration");
      }
    }
#endif	/* !defined(NO_KEYBOARD)	*/
#endif	/* defined(S_SEC_ENABLED) */
  }

/************************************************************************/
/************************************************************************/

#define INFO_SSI			0x01

static ST_VOID send_info_reports (ST_INT now)
  {
static ST_BOOLEAN listOfVariables;
static time_t last_info_time;
time_t time_now;
ST_LONG oldInfoRptCount;
ST_INT i;
ST_RET ret;
OBJECT_NAME obj;
MVL_NVLIST_CTRL *nvl;

  time (&time_now);
  if (now == SD_TRUE || (time_now - last_info_time > infoRptPeriod))
    {
    obj.object_tag = VMD_SPEC;
    obj.obj_name.vmd_spec = "nvl1";
    if ((nvl = mvl_vmd_find_nvl (&mvl_vmd, &obj, NULL)) == NULL)
      {
      printf ("\n Error: cannot find NVL '%s' to send InfoReport",
             obj.obj_name.vmd_spec);
      return;
      }

    oldInfoRptCount = infoRptCount;
    last_info_time = time_now;
    for (i = 0; i < mvl_cfg_info->num_called; ++i)
      {
    /* If there is an active conn and the remote supports info rpt	*/
    /* then we will send our named variable lists as info rpts		*/

      if (mvl_called_conn_ctrl[i].conn_active &&
          (mvl_called_conn_ctrl[i].rem_init_info.serv_supp[9] & INFO_SSI))
        {
        if (a_buffers_avail (mvl_called_conn_ctrl[i].acse_conn_id) >= 2)
          {
          ret = mvl_info_variables (&mvl_called_conn_ctrl[i], nvl,
				listOfVariables);
          if (ret == SD_SUCCESS)
            {
            listOfVariables = (listOfVariables == SD_TRUE) ? SD_FALSE : SD_TRUE;
            ++infoRptCount;
            printf ("\r %ld Info Reports Sent",infoRptCount);
            }
          else
            printf ("\n Info Reports Error : 0x%x",ret);
          }
        else
          {
          SLOGALWAYS0 ("Could not send Info Rpt, no buffers available");
          printf ("\n Could not send Info Rpt, no buffers available ..\n"); 
          }
        }
      }
    if (now == SD_TRUE && (infoRptCount == oldInfoRptCount))
      printf ("\n No active connection (or partner does not support Info Rpt)\n");
    }
  }

/************************************************************************/
/************************************************************************/
/* INITIALIZATION FUNCTIONS 						*/
/************************************************************************/

/************************************************************************/
/************************************************************************/

static ST_VOID init_acse (ST_VOID)
  {
ST_RET ret;
MVL_CFG_INFO  mvlCfg;

/* We want to know about connection activity				*/
  u_mvl_disc_ind_fun     = disc_ind_fun;

/* Read configuration from a file */
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


/************************************************************************/
/************************************************************************/
/************************************************************************/
/*			init_mms_objects				*/
/************************************************************************/

static ST_VOID init_mms_objects (ST_VOID)
  {
ST_INT i;
ST_RET ret;
char hello_str [] = "Hello World!";

  ret = init_named_vars ();
  if (ret != SD_SUCCESS)
    {
    printf ("\ninit_named_vars failed. Exiting.");
    exit (1);
    }
  
/* Just set up some initial values for the configured variables		*/
  for (i = 0; i < 10; ++i)
    {
    arr1[i] = i+1;
    domArr1[i] = i+100;
    }
  Temperature = 71;
  domTemperature = 171;

  struct1.s = 2000;
  struct1.l = 9000;

  UtcTimeNow.secs = (ST_UINT32) time(NULL);
  UtcTimeNow.fraction = 1000;
  UtcTimeNow.qflags = 0x0f;

  domStruct1.s = 12111;
  domStruct1.l = 19111;

  /* Don't have any unicode strings, so create one from normal string	*/
  /* by calling conversion function.					*/
  /* Add 1 to len so that NULL terminator converted too.		*/
  localUTF8len = asn1r_utf8_to_local ((char *) UTF8Vstring13test,
              sizeof (UTF8Vstring13test),
              hello_str, 
              strlen (hello_str)+1
              );
  assert (localUTF8len >= 0);
  }

#if defined(DEBUG_SISCO)
/************************************************************************/
/*			       preReadCurrMemCount                      */
/************************************************************************/

ST_UINT32 tempCurrMemCount;
ST_UINT32 tempMaxDynMemPtrs;
ST_UINT32 tempMaxMemAllocated;
ST_UINT32 tempCurrMemSize;

MVL_VAR_PROC procsCurrMemCount;
MVL_VAR_PROC procsMaxDynMemPtrs;
MVL_VAR_PROC procsMaxMemAllocated;
MVL_VAR_PROC procsCurrMemSize;


ST_RET preReadCurrMemCount (MVL_VAR_ASSOC **va, 
	                        ST_BOOLEAN alt_acc_pres,
	                        ALT_ACCESS *alt_acc,
	                        MVL_NET_INFO *ni,
	                        ST_INT *aa_mode_out)
  {
  if (alt_acc_pres)
    {
    return (SD_FAILURE);
    }

  tempCurrMemCount = chk_curr_mem_count;
  return (SD_SUCCESS);
  }
ST_RET preReadMaxDynMemPtrs (MVL_VAR_ASSOC **va, 
	                        ST_BOOLEAN alt_acc_pres,
	                        ALT_ACCESS *alt_acc,
	                        MVL_NET_INFO *ni,
	                        ST_INT *aa_mode_out)
  {
  if (alt_acc_pres)
    {
    return (SD_FAILURE);
    }

  tempMaxDynMemPtrs = chk_max_dyn_mem_ptrs;
  return (SD_SUCCESS);
  }

ST_RET preReadMaxMemAllocated (MVL_VAR_ASSOC **va, 
	                        ST_BOOLEAN alt_acc_pres,
	                        ALT_ACCESS *alt_acc,
	                        MVL_NET_INFO *ni,
	                        ST_INT *aa_mode_out)
  {
  if (alt_acc_pres)
    {
    return (SD_FAILURE);
    }

  tempMaxMemAllocated = chk_max_mem_allocated;
  return (SD_SUCCESS);
  }

ST_RET preReadCurrMemSize (MVL_VAR_ASSOC **va, 
	                        ST_BOOLEAN alt_acc_pres,
	                        ALT_ACCESS *alt_acc,
	                        MVL_NET_INFO *ni,
	                        ST_INT *aa_mode_out)
  {
  if (alt_acc_pres)
    {
    return (SD_FAILURE);
    }

  tempCurrMemSize = chk_curr_mem_size;
  return (SD_SUCCESS);
  }
#endif
/************************************************************************/
/*                       init_named_vars 				*/
/* Initialize the variable association table				*/
/************************************************************************/

ST_RET init_named_vars (ST_VOID)
  {
OBJECT_NAME obj;
#if defined(DEBUG_SISCO)
MVL_VAR_ASSOC *varptr;
#endif

/* To allow us to resolve alternate access easily, resolve the		*/
/* variable associations now ...					*/
  obj.object_tag = VMD_SPEC;

  obj.obj_name.vmd_spec = "struct1$s";
  struct1s_va = mvl_vmd_find_var (&mvl_vmd, &obj, NULL);

  obj.obj_name.vmd_spec = "struct1$l";
  struct1l_va = mvl_vmd_find_var (&mvl_vmd, &obj, NULL);


/* The mem_chk varaibles						*/

#if defined(DEBUG_SISCO)
  procsCurrMemCount.pre_read_aa = preReadCurrMemCount;
  obj.object_tag = VMD_SPEC;
  obj.obj_name.vmd_spec = "chk_curr_mem_count";
  varptr = mvl_vmd_var_add (&mvl_vmd, &obj, 
			NULL,
			U32_TYPEID,
			&tempCurrMemCount, 
			&procsCurrMemCount,
			SD_TRUE);
  if (varptr == NULL)
    return (SD_FAILURE);

  obj.obj_name.vmd_spec = "chk_max_dyn_mem_ptrs";
  procsMaxDynMemPtrs.pre_read_aa = preReadMaxDynMemPtrs;

  varptr = mvl_vmd_var_add (&mvl_vmd, &obj, 
			NULL,
			U32_TYPEID,
			&tempMaxDynMemPtrs, 
			&procsMaxDynMemPtrs,
			SD_TRUE);
  if (varptr == NULL)
    return (SD_FAILURE);

  obj.obj_name.vmd_spec = "chk_max_mem_allocated";
  procsMaxMemAllocated.pre_read_aa = preReadMaxMemAllocated;

  varptr = mvl_vmd_var_add (&mvl_vmd, &obj, 
			NULL,
			U32_TYPEID,
			&tempMaxMemAllocated, 
			&procsMaxMemAllocated,
			SD_TRUE);
  if (varptr == NULL)
    return (SD_FAILURE);

  obj.obj_name.vmd_spec = "chk_curr_mem_size";
  procsCurrMemSize.pre_read_aa = preReadCurrMemSize;

  varptr = mvl_vmd_var_add (&mvl_vmd, &obj, 
			NULL,
			U32_TYPEID,
			&tempCurrMemSize, 
			&procsCurrMemSize,
			SD_TRUE);
  if (varptr == NULL)
    return (SD_FAILURE);
#endif
  return (SD_SUCCESS);
  }

/************************************************************************/
/*			init_journals					*/
/* This sample code shows how to add entries to a Journal when		*/
/* the Journal is modeled as a linked list of journal entries.		*/
/************************************************************************/
static ST_CHAR vartag0 [] = "vartag0";
static ST_CHAR vartag1 [] = "vartag1";
static ST_CHAR vartag2 [] = "vartag2";

static ST_VOID init_journals (ST_VOID)
  {
#if (MMS_JREAD_EN & RESP_EN)
MVL_JOURNAL_CTRL *jou;
MVL_JOURNAL_ENTRY *je;
OBJECT_NAME obj;

  obj.object_tag = VMD_SPEC;
  obj.obj_name.vmd_spec = "SampleJournal";
  jou = mvl_vmd_find_jou (&mvl_vmd, &obj, NULL);
  if (jou == NULL)
    {
    printf ("\n Error: cannot find journal '%s' to intialize",
            obj.obj_name.vmd_spec);
    return;
    }

  /* Add entry	*/
  je = (MVL_JOURNAL_ENTRY *) chk_calloc (1, sizeof (MVL_JOURNAL_ENTRY));
  je->entry_id[0] = 1;
  je->entry_id_len = 1;
  mvl_dib_entry_to_app_ref (&loc_dib_table[0], &je->orig_app);
  je->occur_time.form = MMS_BTOD6;
  je->occur_time.ms = 1;	/* milliseconds since midnight*/
  je->occur_time.day = 5300;	/* Days since 1-1-1984		*/
  je->entry_form_tag = JE_FORM_DATA;
  je->ef.data.event_pres = SD_FALSE;
  je->ef.data.list_of_var_pres = 1;
  je->ef.data.num_of_var = 1;
  je->ef.data.list_of_var = (VAR_INFO *) chk_calloc (je->ef.data.num_of_var,
                                     sizeof (VAR_INFO));
  /* NOTE: if num_of_var > 1, call "fill_journal_var_info" multiple	*/
  /* times & change first arg to "&je->ef.data.list_of_var [1]", etc.	*/
  if (fill_journal_var_info (&je->ef.data.list_of_var [0],
                             vartag0, struct1s_va))
    {		/* Failed. Just leave var_info out of entry.	*/
    je->ef.data.list_of_var_pres = SD_FALSE;
    chk_free (je->ef.data.list_of_var);
    }
  list_add_last((ST_VOID **) &jou->entries, (ST_VOID *) je);
  jou->numEntries++;

  /* Add entry	*/
  je = (MVL_JOURNAL_ENTRY *) chk_calloc (1, sizeof (MVL_JOURNAL_ENTRY));
  je->entry_id[0] = 2;
  je->entry_id_len = 1;
  mvl_dib_entry_to_app_ref (&loc_dib_table[0], &je->orig_app);
  je->occur_time.form = MMS_BTOD6;
  je->occur_time.ms = 2;	/* milliseconds since midnight*/
  je->occur_time.day = 5300;	/* Days since 1-1-1984		*/
  je->entry_form_tag = JE_FORM_ANNOTATION;
  je->ef.annotation = "This is annotation type journal entry.";
  list_add_last((ST_VOID **) &jou->entries, (ST_VOID *) je);
  jou->numEntries++;
  
  /* Add entry	*/
  je = (MVL_JOURNAL_ENTRY *) chk_calloc (1, sizeof (MVL_JOURNAL_ENTRY));
  je->entry_id[0] = 3;
  je->entry_id_len = 1;
  mvl_dib_entry_to_app_ref (&loc_dib_table[0], &je->orig_app);
  je->occur_time.form = MMS_BTOD6;
  je->occur_time.ms = 3;	/* milliseconds since midnight*/
  je->occur_time.day = 5300;	/* Days since 1-1-1984		*/
  je->entry_form_tag = JE_FORM_DATA;
  je->ef.data.event_pres = SD_FALSE;
  je->ef.data.list_of_var_pres = 1;
  je->ef.data.num_of_var = 1;
  je->ef.data.list_of_var = (VAR_INFO *) chk_calloc (je->ef.data.num_of_var,
                                     sizeof (VAR_INFO));
  if (fill_journal_var_info (&je->ef.data.list_of_var [0],
                             vartag1, struct1l_va))
    {		/* Failed. Just leave var_info out of entry.	*/
    je->ef.data.list_of_var_pres = SD_FALSE;
    chk_free (je->ef.data.list_of_var);
    }
  list_add_last((ST_VOID **) &jou->entries, (ST_VOID *) je);
  jou->numEntries++;

  /* Add entry	*/
  je = (MVL_JOURNAL_ENTRY *) chk_calloc (1, sizeof (MVL_JOURNAL_ENTRY));
  je->entry_id[0] = 4;
  je->entry_id_len = 1;
  mvl_dib_entry_to_app_ref (&loc_dib_table[0], &je->orig_app);
  je->occur_time.form = MMS_BTOD6;
  je->occur_time.ms = 4;	/* milliseconds since midnight*/
  je->occur_time.day = 5300;	/* Days since 1-1-1984		*/
  je->entry_form_tag = JE_FORM_DATA;
  je->ef.data.event_pres = SD_FALSE;
  je->ef.data.list_of_var_pres = 1;
  je->ef.data.num_of_var = 1;
  je->ef.data.list_of_var = (VAR_INFO *) chk_calloc (je->ef.data.num_of_var,
                                     sizeof (VAR_INFO));
  if (fill_journal_var_info (&je->ef.data.list_of_var [0],
                             vartag0, struct1s_va))
    {		/* Failed. Just leave var_info out of entry.	*/
    je->ef.data.list_of_var_pres = SD_FALSE;
    chk_free (je->ef.data.list_of_var);
    }
  list_add_last((ST_VOID **) &jou->entries, (ST_VOID *) je);
  jou->numEntries++;

  /* Add entry	*/
  je = (MVL_JOURNAL_ENTRY *) chk_calloc (1, sizeof (MVL_JOURNAL_ENTRY));
  je->entry_id[0] = 5;
  je->entry_id_len = 1;
  mvl_dib_entry_to_app_ref (&loc_dib_table[0], &je->orig_app);
  je->occur_time.form = MMS_BTOD6;
  je->occur_time.ms = 5;	/* milliseconds since midnight*/
  je->occur_time.day = 5300;	/* Days since 1-1-1984		*/
  je->entry_form_tag = JE_FORM_ANNOTATION;
  je->ef.annotation = "This is annotation type journal entry.";
  list_add_last((ST_VOID **) &jou->entries, (ST_VOID *) je);
  jou->numEntries++;

  /* Add entry	*/
  je = (MVL_JOURNAL_ENTRY *) chk_calloc (1, sizeof (MVL_JOURNAL_ENTRY));
  je->entry_id[0] = 6;
  je->entry_id_len = 1;
  mvl_dib_entry_to_app_ref (&loc_dib_table[0], &je->orig_app);
  je->occur_time.form = MMS_BTOD6;
  je->occur_time.ms = 6;	/* milliseconds since midnight*/
  je->occur_time.day = 5300;	/* Days since 1-1-1984		*/
  je->entry_form_tag = JE_FORM_DATA;
  je->ef.data.event_pres = SD_FALSE;
  je->ef.data.list_of_var_pres = 1;
  je->ef.data.num_of_var = 2;
  je->ef.data.list_of_var = (VAR_INFO *) chk_calloc (je->ef.data.num_of_var,
                                     sizeof (VAR_INFO));
  /* NOTE: this is an example with 2 vars.				*/
  if (fill_journal_var_info (&je->ef.data.list_of_var [0],
                             vartag0, struct1s_va)  ||
      fill_journal_var_info (&je->ef.data.list_of_var [1],
                             vartag1, struct1l_va))
    {		/* Failed. Just leave var_info out of entry.	*/
    je->ef.data.list_of_var_pres = SD_FALSE;
    chk_free (je->ef.data.list_of_var);
    }
  list_add_last((ST_VOID **) &jou->entries, (ST_VOID *) je);
  jou->numEntries++;

  /* -------------------- DOM_SPEC Journal ---------------------*/
  obj.object_tag = DOM_SPEC;
  obj.domain_id = "mvlLiteDom1";
  obj.obj_name.vmd_spec = "MyJournal";
  jou = mvl_vmd_find_jou (&mvl_vmd, &obj, NULL);
  if (jou == NULL)
    {
    printf ("\n Error: cannot find journal '%s:%s' to intialize",
            obj.domain_id, obj.obj_name.vmd_spec);
    return;
    }

  /* Add entry	*/
  je = (MVL_JOURNAL_ENTRY *) chk_calloc (1, sizeof (MVL_JOURNAL_ENTRY));
  je->entry_id[0] = 1;
  je->entry_id_len = 1;
  mvl_dib_entry_to_app_ref (&loc_dib_table[0], &je->orig_app);
  je->occur_time.form = MMS_BTOD6;
  je->occur_time.ms = 1;	/* milliseconds since midnight*/
  je->occur_time.day = 5600;	/* Days since 1-1-1984		*/
  je->entry_form_tag = JE_FORM_DATA;
  je->ef.data.event_pres = SD_FALSE;
  je->ef.data.list_of_var_pres = 1;
  je->ef.data.num_of_var = 1;
  je->ef.data.list_of_var = (VAR_INFO *) chk_calloc (je->ef.data.num_of_var,
                                     sizeof (VAR_INFO));
  /* NOTE: if num_of_var > 1, call "fill_journal_var_info" multiple	*/
  /* times & change first arg to "&je->ef.data.list_of_var [1]", etc.	*/
  if (fill_journal_var_info (&je->ef.data.list_of_var [0],
                             vartag2, struct1s_va))
    {		/* Failed. Just leave var_info out of entry.	*/
    je->ef.data.list_of_var_pres = SD_FALSE;
    chk_free (je->ef.data.list_of_var);
    }
  list_add_last((ST_VOID **) &jou->entries, (ST_VOID *) je);
  jou->numEntries++;

#endif
  }

/************************************************************************/
/* 			fill_journal_var_info				*/
/* This function reads the variable "va" at THIS MOMENT in time,	*/
/* converts it to ASN.1 encoding, and then fills in the VAR_INFO	*/
/* structure appropriately.						*/
/* This function may be replaced by any function that fills in the	*/
/* VAR_INFO structure with appropriate "time stamped" data.		*/
/************************************************************************/
static ST_RET fill_journal_var_info (VAR_INFO *var_info, ST_CHAR *var_tag,
                             MVL_VAR_ASSOC *va)
  {
ST_RET rc;
ST_INT varDataLen;
ST_UCHAR *asn1_buf;

  var_info->var_tag = var_tag;

  asn1_buf = M_MALLOC (MSMEM_ASN1_DATA_ENC, mvl_cfg_info->max_msg_size);
  /* First arg is (MVL_NET_INFO *), used for AA-SPEC variables.		*/
  /* Journal should not contain any AA-SPEC vars, so passing NULL is OK.*/
  rc = mvl_get_va_asn1_data (NULL, va, SD_FALSE, NULL, 
                             asn1_buf, mvl_cfg_info->max_msg_size, &varDataLen);
  if (rc == SD_SUCCESS)
    {
    /* ASN.1 conversion OK. Allocate buffer for ASN.1 data & copy to it.*/
    /* "mvl_get_va_asn1_data" encodes at beginning of buffer, so	*/
    /* "asn1_buf" points to ASN.1 data.					*/

    var_info->value_spec.data = (ST_UCHAR *) chk_malloc (varDataLen);
    memcpy (var_info->value_spec.data, asn1_buf, varDataLen);
    var_info->value_spec.len = varDataLen;
    }
  else	/* Could not get journal data converted to ASN.1 	*/
    {
    printf ("\n ERROR: Illegal variable data for Journal Entry.");
    }
  M_FREE (MSMEM_ASN1_DATA_ENC, asn1_buf);
  return (rc);
  }


/************************************************************************/
/************************************************************************/
/* READ/WRITE PROCESSING FUNCTIONS					*/
/* These functions are attached to the VariableAssociations for the 	*/
/* configured variables. They are used to provide a means for the user	*/
/* to resolve alternate access as well as to perform application tasks.	*/
/************************************************************************/
/************************************************************************/
/*			 preReadAA					*/
/************************************************************************/

ST_RET preReadAA (MVL_VAR_ASSOC **va, 
		ST_BOOLEAN alt_acc_pres,
	      	ALT_ACCESS *alt_acc,
	      	MVL_NET_INFO *ni,
		ST_INT *aa_mode_out)
  {
MVL_VAR_ASSOC *newVa;

/* See if the remote node is reading with alternate access. If so, we	*/
/* will make sure it is on a supported variable then resolve the	*/
/* variable association.						*/

  if (alt_acc_pres)
    {
    if (!strcmp ((*va)->name, "struct1") && 
       alt_acc->num_aa == 1 &&
       alt_acc->aa[0].sel_type == AA_COMP)
      {
      if (!strcmp (alt_acc->aa[0].u.component, "s"))
   	*va = struct1s_va;
      else if (!strcmp (alt_acc->aa[0].u.component, "l"))
   	*va = struct1l_va;
      else
        return (SD_FAILURE);
      }
    else if (!strcmp ((*va)->name, "arr1") && 
       alt_acc->num_aa == 1 &&
       alt_acc->aa[0].sel_type == AA_INDEX)
      {
      newVa = (MVL_VAR_ASSOC *) chk_calloc (1, sizeof (MVL_VAR_ASSOC));
      newVa->name = "arr1_alta";
      newVa->data = &arr1[alt_acc->aa[0].u.index];
      newVa->type_id = I16_TYPEID;
      *va = newVa;
      }
    else
      return (SD_FAILURE);

  /* Since we are resolving the alternate access, MVL does not have to 	*/
  /* deal with it.							*/
    *aa_mode_out = MVL_USE_NO_AA;
    }

  /* Just for fun, change the value of Temperature whenever it is read.	*/
  if (!strcmp ((*va)->name, "Temperature")) 
    ++Temperature;
  if (!strcmp ((*va)->name, "domTemperature")) 
    ++domTemperature;

  return (SD_SUCCESS);
  }

/************************************************************************/
/*			 postReadAA					*/
/************************************************************************/

ST_VOID postReadAA (MVL_VAR_ASSOC *va, 
		ST_BOOLEAN alt_acc_pres,
	      	ALT_ACCESS *alt_acc,
	      	MVL_NET_INFO *ni)
  {

/* For alternate access on arr1, we allocated the VA			*/
  if (alt_acc_pres && !strcmp (va->name, "arr1_alta"))
    chk_free (va);
  }

/************************************************************************/
/*			 procWriteAA					*/
/************************************************************************/

ST_RET procWriteAA (MVL_VAR_ASSOC **va, 
	      	ALT_ACCESS *alt_acc,
	      	MVL_NET_INFO *ni,
		ST_INT *aa_mode_out)
  {
/* The remote node is writing with alternate access. We will make sure 	*/
/* it is on a supported variable then resolve the variable association.	*/

  if (!strcmp ((*va)->name, "struct1") && 
     alt_acc->num_aa == 1 &&
     alt_acc->aa[0].sel_type == AA_COMP)
    {
    if (!strcmp (alt_acc->aa[0].u.component, "s"))
      *va = struct1s_va;
    else if (!strcmp (alt_acc->aa[0].u.component, "l"))
      *va = struct1l_va;
    else
      return (SD_FAILURE);
    }
  else
    return (SD_FAILURE);

  *aa_mode_out = MVL_USE_NO_AA;
  return (SD_SUCCESS);
  }

/************************************************************************/
/*			 preWriteAA					*/
/************************************************************************/

ST_RET preWriteAA (MVL_VAR_ASSOC *va, 
		ST_BOOLEAN alt_acc_pres,
	      	ALT_ACCESS *alt_acc,
		MVL_NET_INFO *ni, 
	      	ST_CHAR *write_data, 
		ST_INT write_data_size)
  {
  return (SD_SUCCESS);
  }

/************************************************************************/
/*			 postWriteAA					*/
/************************************************************************/

ST_VOID postWriteAA (MVL_VAR_ASSOC *va, 
		ST_BOOLEAN alt_acc_pres,
	      	ALT_ACCESS *alt_acc,
		MVL_NET_INFO *ni)
  {
#if defined (USE_MANUFACTURED_OBJS)
STRUCT1 *strPtr;
ST_INT16 *intPtr;
ST_INT32 *longPtr;

/* We need to take care of the write data here. 			*/
  if (!strcmp (va->name, "Temperature_dyn"))
    {
    intPtr = (ST_INT16 *) va->data;
    lastDynTemp = *intPtr;
    }
  else if (!strcmp (va->name, "arr1_dyn"))
    {
    intPtr = (ST_INT16 *) va->data;
    lastArr1StartVal = *intPtr;
    }
  else if (!strcmp (va->name, "struct1_dyn"))
    {
    strPtr = (STRUCT1 *) va->data;
    lastStr1S = strPtr->s;
    lastStr1L = strPtr->l;
    }
  else if (!strcmp (va->name, "struct1$s_dyn"))
    {
    intPtr = (ST_INT16 *) va->data;
    lastStr1S = *intPtr;
    }
  else if (!strcmp (va->name, "struct1$l_dyn"))
    {
    longPtr = (ST_INT32 *) va->data;
    lastStr1L = *longPtr;
    }
#endif /* USE_MANUFACTURED_OBJS */
  }

/************************************************************************/
/************************************************************************/

MVL_VAR_PROC varProcFuns =
  {
  preReadAA,		/* pre_read_aa		*/
  postReadAA,		/* post_read_aa		*/
  procWriteAA,		/* proc_write_aa	*/
  preWriteAA,		/* pre_write_aa		*/
  postWriteAA,		/* post_write_aa	*/
  NULL,			/* pre_info		*/
  NULL,			/* post_info		*/
  };

/************************************************************************/
/************************************************************************/
/* MANUFACTURED VARIABLE/VARIABLELIST RESOLUTION FUNCTIONS			*/
/************************************************************************/

#if defined (USE_MANUFACTURED_OBJS)

#pragma message("These customized functions will only be called if USE_MANUFACTURED_OBJS defined when MVL library compiled.")
#pragma message(" u_mvl_get_va_aa")
#pragma message(" u_mvl_get_nvl")
#pragma message(" u_mvl_free_va")
#pragma message(" u_mvl_free_nvl")
#pragma message(" u_gnl_ind_vars")
#pragma message(" u_gnl_ind_nvls")
#pragma message(" u_gnl_ind_doms")
/************************************************************************/
/*			u_mvl_get_va_aa					*/
/************************************************************************/
/* This function is called from MVL when it is unable to find a 	*/
/* configured MMS server variable for a READ, WRITE, or GET VARIABLE 	*/
/* ACCESS ATTRIBUTES indication. We can 'manufacture' a variable	*/
/* association or return NULL if the MMS object name is unrecognized.	*/
/* In manufacturing the variable association, we can take alternate	*/
/* access into account and resolve it if we so desire, or it can be	*/
/* taken care of in later processing. In this example, we handle it.	*/

MVL_VAR_ASSOC *u_mvl_get_va_aa (MVL_VMD_CTRL *vmd_ctrl, ST_INT service, OBJECT_NAME *obj, 
			   MVL_NET_INFO *netInfo,
		      	   ST_BOOLEAN alt_access_pres,
		      	   ALT_ACCESS *alt_acc,
		      	   ST_BOOLEAN *alt_access_done_out)
  {
MVL_DOM_CTRL *dom;
MVL_AA_OBJ_CTRL *aa;
ST_CHAR *name;

/* We will handle any alternate access here */
  if (alt_access_pres)
    *alt_access_done_out = SD_TRUE;
    
  switch (obj->object_tag)
    {
    case VMD_SPEC :
      name = obj->obj_name.vmd_spec;
      return (get_va_aa (service, name, alt_access_pres, alt_acc));
    break;

    case DOM_SPEC :
      dom = mvl_vmd_find_dom (&mvl_vmd, obj->domain_id);
      if (!dom)
        {
	MVL_LOG_NERR0 ("Find Variable Association : Domain control not found");
        return (NULL);
	}
      name = obj->obj_name.item_id;
      return (get_va_aa (service, name, alt_access_pres, alt_acc));
    break;

    case AA_SPEC :
      aa = (MVL_AA_OBJ_CTRL *) netInfo->aa_objs;
      if (!aa)
	{
	MVL_LOG_NERR0 ("Find Variable Association : AA control not found");
        return (NULL);
	}
      name = obj->obj_name.aa_spec;
      return (get_va_aa (service, name, alt_access_pres, alt_acc));
    break;
    }

  MVL_LOG_NERR0 ("Error: u_mvl_get_va_aa");
  return (NULL);
  }

/************************************************************************/
/*				get_va_aa				*/
/************************************************************************/

static MVL_VAR_ASSOC *get_va_aa (ST_INT service, ST_CHAR *name, 
		      	   ST_BOOLEAN alt_access_pres,
		      	   ALT_ACCESS *alt_acc)
  {
ST_INT16 *sp;
MVL_VAR_ASSOC *va;

  if (alt_access_pres)
    {
    if (!strcmp (name, "struct1_dyn") && 
       alt_acc->num_aa == 1 &&
       alt_acc->aa[0].sel_type == AA_COMP)
      {
      if (!strcmp (alt_acc->aa[0].u.component, "s"))
   	name = "struct1_dyn$s";
      else if (!strcmp (alt_acc->aa[0].u.component, "l"))
   	name = "struct1_dyn$l";
      else
        return (NULL);
      }
    else if (!strcmp (name, "arr1_dyn") && 
       alt_acc->num_aa == 1 &&
       alt_acc->aa[0].sel_type == AA_INDEX)
      {
      va = chk_calloc (1, sizeof (MVL_VAR_ASSOC));
      va->name = "arr1_alta";
      va->data = chk_calloc (1, sizeof (ST_INT16));
      sp = (ST_INT16 *) va->data;
      *sp = lastArr1StartVal + (ST_INT16) alt_acc->aa[0].u.index;
      va->type_id = I16_TYPEID;
      return (va);
      }
    else
      return (NULL);
    }

  return (get_va (service, name));
  }

/************************************************************************/
/*				get_va					*/
/************************************************************************/

static MVL_VAR_ASSOC *get_va (ST_INT service, ST_CHAR *name)
  {
MVL_VAR_ASSOC *va;

  if (!strcmp (name, "Temperature_dyn"))
    va = get_Temperature_va (service);
  else if (!strcmp (name, "arr1_dyn"))
    va = get_arr1_va (service);
  else if (!strcmp (name, "struct1_dyn"))
    va = get_struct1_va (service);
  else if (!strcmp (name, "struct1$s_dyn"))
    va = get_struct1_s_va (service);
  else if (!strcmp (name, "struct1$l_dyn"))
    va = get_struct1_l_va (service);
  else
    va = NULL;

  return (va);
  }

/************************************************************************/
/*				u_mvl_get_nvl				*/
/************************************************************************/
/* This function is called from MVL when it is unable to find a 	*/
/* configured MMS server named variable list for a READ, WRITE, or GET 	*/
/* VARIABLE LIST ATTRIBUTES indication. We can 'manufacture' a variable	*/
/* list or return NULL if the MMS object name is unrecognized.		*/

MVL_NVLIST_CTRL *u_mvl_get_nvl (ST_INT service, 
			      OBJECT_NAME *obj, MVL_NET_INFO *netInfo)
  {
MVL_DOM_CTRL *dom;
MVL_NVLIST_CTRL *nvl;
MVL_AA_OBJ_CTRL *aa;
ST_CHAR *name;

  switch (obj->object_tag)
    {
    case VMD_SPEC :
      name = obj->obj_name.vmd_spec;
      if (!strcmp (name, "nvl1_dyn"))
        nvl = get_nvl1_nvl (service);
      else
        nvl = NULL;

      return (nvl);
    break;

    case DOM_SPEC :
      dom = mvl_vmd_find_dom (&mvl_vmd, obj->domain_id);
      if (!dom)
        {
	MVL_LOG_NERR0 ("Find Variable Association : Domain control not found");
        return (NULL);
	}
      name = obj->obj_name.item_id;
      if (!strcmp (name, "nvl1_dyn"))
        nvl = get_nvl1_nvl (service);
      else
        nvl = NULL;

      return (nvl);
    break;

    case AA_SPEC :
      aa = (MVL_AA_OBJ_CTRL *) netInfo->aa_objs;
      if (!aa)
	{
	MVL_LOG_NERR0 ("Find Variable Association : AA control not found");
        return (NULL);
	}
      name = obj->obj_name.aa_spec;
      if (!strcmp (name, "nvl1_dyn"))
        nvl = get_nvl1_nvl (service);
      else
        nvl = NULL;

      return (nvl);
    break;
    }

  MVL_LOG_NERR0 ("Error: Find NVL failed");
  return (NULL);
  }

/************************************************************************/
/*				get_nvl1_nvl				*/
/************************************************************************/

static MVL_NVLIST_CTRL *get_nvl1_nvl (ST_INT service)
  {
MVL_NVLIST_CTRL *nvl;

  nvl = chk_calloc (1, sizeof (MVL_NVLIST_CTRL));
  nvl->name = "nvl1_dyn";
  nvl->num_of_entries = 3;
  nvl->entries = chk_calloc (nvl->num_of_entries, 
  		   sizeof (MVL_VAR_ASSOC **));
  nvl->va_scope = (MVL_SCOPE *) chk_calloc (3, sizeof (MVL_SCOPE));

  nvl->entries[0] = get_arr1_va (service);
  nvl->va_scope[0].scope = VMD_SPEC;
  nvl->entries[1] = get_struct1_va (service);
  nvl->va_scope[1].scope = VMD_SPEC;
  nvl->entries[2] = get_Temperature_va (service);
  nvl->va_scope[2].scope = VMD_SPEC;
  return (nvl);
  }

/************************************************************************/
/*                       get_Temperature_va				*/
/************************************************************************/

MVL_VAR_ASSOC *get_Temperature_va (ST_INT service)
  {
MVL_VAR_ASSOC *va;
ST_INT16 *intPtr;

  va = chk_calloc (1, sizeof (MVL_VAR_ASSOC));
  va->name = "Temperature_dyn";
  va->data = chk_calloc (1, sizeof (ST_INT16));
  va->type_id = I16_TYPEID;
  va->type_ctrl = mvl_type_ctrl_find (va->type_id);
  if (va->type_ctrl == NULL)
    {
    MVL_LOG_ERR0 ("Cannot create variable (type_id invalid).");
    return (NULL);
    }
  va->proc = &varProcFuns;
  if (service == MMSOP_READ)
    {
    intPtr = (ST_INT16 *) va->data;
    *intPtr = lastDynTemp++;
    }
  return (va);
  }

/************************************************************************/
/*                       get_arr1_va					*/
/************************************************************************/

MVL_VAR_ASSOC *get_arr1_va (ST_INT service)
  {
MVL_VAR_ASSOC *va;
ST_INT i;
ST_INT16 *intPtr;

  va = chk_calloc (1, sizeof (MVL_VAR_ASSOC));
  va->name = "arr1_dyn";
  va->data = chk_calloc (1, 10 * sizeof (ST_INT16));
  va->type_id = ARR1_TYPEID;
  va->type_ctrl = mvl_type_ctrl_find (va->type_id);
  if (va->type_ctrl == NULL)
    {
    MVL_LOG_ERR0 ("Cannot create variable (type_id invalid).");
    return (NULL);
    }
  va->proc = &varProcFuns;
  if (service == MMSOP_READ)
    {
    intPtr = (ST_INT16 *) va->data;
    for (i = 0; i < 10; ++i, ++intPtr)
      *intPtr = lastArr1StartVal+i;
    ++lastArr1StartVal;
    }
  return (va);
  }

/************************************************************************/
/*                       get_struct1_va					*/
/************************************************************************/


MVL_VAR_ASSOC *get_struct1_va (ST_INT service)
  {
MVL_VAR_ASSOC *va;
STRUCT1 *strPtr;

  va = chk_calloc (1, sizeof (MVL_VAR_ASSOC));
  va->data = chk_calloc (1, sizeof (STRUCT1));
  va->name = "struct1_dyn";
  va->type_id = STRUCT1_TYPEID;
  va->type_ctrl = mvl_type_ctrl_find (va->type_id);
  if (va->type_ctrl == NULL)
    {
    MVL_LOG_ERR0 ("Cannot create variable (type_id invalid).");
    return (NULL);
    }
  va->proc = &varProcFuns;
  if (service == MMSOP_READ)
    {
    strPtr = (STRUCT1 *) va->data;
    strPtr->s = lastStr1S++;
    strPtr->l = lastStr1L++;
    }
  return (va);
  }

/************************************************************************/
/*                       get_struct1_s_va 				*/
/************************************************************************/

static MVL_VAR_ASSOC *get_struct1_s_va (ST_INT service)
  {
MVL_VAR_ASSOC *va;
ST_INT16 *intPtr;

  va = chk_calloc (1, sizeof (MVL_VAR_ASSOC));
  va->name = "struct1$s_dyn";
  va->data = chk_calloc (1, sizeof (ST_INT16));
  va->type_id = I16_TYPEID;
  va->type_ctrl = mvl_type_ctrl_find (va->type_id);
  if (va->type_ctrl == NULL)
    {
    MVL_LOG_ERR0 ("Cannot create variable (type_id invalid).");
    return (NULL);
    }
  va->proc = &varProcFuns;
  if (service == MMSOP_READ)
    {
    intPtr = (ST_INT16 *) va->data;
    *intPtr = lastStr1S++;
    }
  return (va);
  }

/************************************************************************/
/*                       get_struct1_l_va				*/
/************************************************************************/

static MVL_VAR_ASSOC *get_struct1_l_va (ST_INT service)
  {
MVL_VAR_ASSOC *va;
ST_INT32 *longPtr;

  va = chk_calloc (1, sizeof (MVL_VAR_ASSOC));
  va->name = "struct1$l_dyn";
  va->data = chk_calloc (1, sizeof (ST_INT32));
  va->type_id = I32_TYPEID;
  va->type_ctrl = mvl_type_ctrl_find (va->type_id);
  if (va->type_ctrl == NULL)
    {
    MVL_LOG_ERR0 ("Cannot create variable (type_id invalid).");
    return (NULL);
    }
  va->proc = &varProcFuns;
  if (service == MMSOP_READ)
    {
    longPtr = (ST_INT32 *) va->data;
    *longPtr = lastStr1L++;
    }
  return (va);
  }

/************************************************************************/
/*				u_mvl_free_nvl				*/
/************************************************************************/

ST_VOID u_mvl_free_nvl (ST_INT service, MVL_NVLIST_CTRL *nvl, 
			     MVL_NET_INFO *netInfo)
  {
ST_INT i;

  for (i = 0; i < nvl->num_of_entries; ++i)
    u_mvl_free_va (service, nvl->entries[i], netInfo);

  chk_free (nvl->entries);
  chk_free (nvl->va_scope);
  chk_free (nvl);
  }

/************************************************************************/
/*				u_mvl_free_va				*/
/************************************************************************/

ST_VOID u_mvl_free_va (ST_INT service, MVL_VAR_ASSOC *va, 
			    MVL_NET_INFO *netInfo)
  {
  chk_free (va->data);
  chk_free (va);
  }

/************************************************************************/
/************************************************************************/
/* GET NAMELIST HELPER FUNCTIONS					*/
/* These functions are necessary because MVL does not know about our	*/
/* manufactured variables and variable lists. We will fill in part of 	*/
/* the namelist response data structure.				*/
/************************************************************************/
/* 			u_gnl_ind_vars				*/
/************************************************************************/

ST_CHAR *serverVars[] =
  {
  "Temperature",
  "Temperature_dyn",
  "arr1",
  "arr1_dyn",
  "struct1",
  "struct1$l",
  "struct1$l_dyn",
  "struct1$s",
  "struct1$s_dyn",
  "struct1_dyn",
  };
ST_INT numServerVars = sizeof(serverVars)/sizeof(ST_CHAR *);


ST_INT u_gnl_ind_vars (MVL_NET_INFO *net_info, NAMELIST_REQ_INFO *req_info, 
			     ST_CHAR **ptr, ST_BOOLEAN *moreFollowsOut, 
			     ST_INT maxNames)
  {
ST_INT v;
ST_INT i;
ST_INT j;

/* Take care of the 'continue after' business if necessary */
  i = 0;
  if (req_info->cont_after_pres)
    {
    while (i < numServerVars)
      {
      v = strcmp (req_info->continue_after, serverVars[i]);
      if (v == 0)
        {
        ++i;			/* Index to the next one		*/
        break;
        }
      if (v < 0) 
        break;

      ++i;			/* We have not found our place yet ...	*/
      }
    }

/* Now make the list for the response */
  for (j = 0; j < maxNames && i < numServerVars; ++i, ++j, ++ptr)
    *ptr = serverVars[i];

  if (i < numServerVars)
    *moreFollowsOut = SD_TRUE;

  return (j);
  }


/************************************************************************/
/*			u_gnl_ind_nvls				*/
/************************************************************************/

ST_CHAR *serverNvls[] =
  {
  "nvl1",
  "nvl1_dyn"
  };
ST_INT numServerNvls = sizeof(serverNvls)/sizeof(ST_CHAR *);


ST_INT u_gnl_ind_nvls (MVL_NET_INFO *net_info, NAMELIST_REQ_INFO *req_info, 
			     ST_CHAR **ptr, ST_BOOLEAN *moreFollowsOut, 
			     ST_INT maxNames)
  {
ST_INT v;
ST_INT i;
ST_INT j;

/* Take care of the 'continue after' business if necessary */
  i = 0;
  if (req_info->cont_after_pres)
    {
    while (i < numServerNvls)
      {
      v = strcmp (req_info->continue_after, serverNvls[i]);
      if (v == 0)
        {
        ++i;			/* Index to the next one		*/
        break;
        }
      if (v < 0) 
        break;

      ++i;			/* We have not found our place yet ...	*/
      }
    }

/* Now make the list for the response */
  for (j = 0; j < maxNames && i < numServerNvls; ++i, ++j, ++ptr)
    *ptr = serverNvls[i];

  if (i < numServerNvls)
    *moreFollowsOut = SD_TRUE;

  return (j);
  }

/************************************************************************/
/*			u_gnl_ind_doms				*/
/************************************************************************/

ST_CHAR *serverDoms[] =
  {
  "mvlLiteDomain"
  };
ST_INT numServerDoms = sizeof(serverDoms)/sizeof(ST_CHAR *);


ST_INT u_gnl_ind_doms (NAMELIST_REQ_INFO *req_info, 
			     ST_CHAR **ptr, ST_BOOLEAN *moreFollowsOut, 
			     ST_INT maxNames)
  {
ST_INT v;
ST_INT i;
ST_INT j;

/* Take care of the 'continue after' business if necessary */
  i = 0;
  if (req_info->cont_after_pres)
    {
    while (i < numServerDoms)
      {
      v = strcmp (req_info->continue_after, serverDoms[i]);
      if (v == 0)
        {
        ++i;			/* Index to the next one		*/
        break;
        }
      if (v < 0) 
        break;

      ++i;			/* We have not found our place yet ...	*/
      }
    }

/* Now make the list for the response */
  for (j = 0; j < maxNames && i < numServerDoms; ++i, ++j, ++ptr)
    *ptr = serverDoms[i];

  if (i < numServerDoms)
    *moreFollowsOut = SD_TRUE;

  return (j);
  }

/************************************************************************/
#endif /* USE_MANUFACTURED_OBJS */
/************************************************************************/

/************************************************************************/
/************************************************************************/
/* ASYNC. READ/WRITE RESPONSE SUPPORT					*/
/************************************************************************/

MVL_IND_PEND *pendIndCtrl;
ST_INT indCount;
time_t pendIndRxTime;

/************************************************************************/
/************************************************************************/

static ST_VOID pend_ind_service ()
  {

/* Any indications pending? */
  if (!pendIndCtrl)
    return;

/* We will hold onto every tenth indication for about 10 sec.		*/
  if (time (NULL) - pendIndRxTime >= asyncRespTime)
    {
#if (MMS_READ_EN & RESP_EN)
    if (pendIndCtrl->op == MMSOP_READ)
      mvlas_read_resp (pendIndCtrl);
#endif
#if (MMS_WRITE_EN & RESP_EN)
    if (pendIndCtrl->op == MMSOP_WRITE)
      mvlas_write_resp (pendIndCtrl);
#endif
    pendIndCtrl = NULL;
    }
  }


/************************************************************************/
/************************************************************************/

#if (MMS_READ_EN & RESP_EN)
ST_VOID u_mvl_read_ind (MVL_IND_PEND *indCtrl)
  {
MVLAS_READ_CTRL *rdCtrl;
MVLAS_RD_VA_CTRL *vaCtrl;
MVL_VAR_ASSOC *va;
ST_INT i;

  ++indCount;

/* We really don't have anything to do here, but just show how to 	*/
/* access the info of interest. 					*/
  indCtrl->usr = NULL;			/* We get to use this field 	*/
  rdCtrl = &indCtrl->u.rd;
  vaCtrl = rdCtrl->vaCtrlTbl;
  for (i = 0; i < rdCtrl->numVar; ++i, ++vaCtrl)
    {
    va = vaCtrl->va;
    if (va)		/* VA was resolved, we can look it over		*/
      {
      vaCtrl->acc_rslt_tag = ACC_RSLT_SUCCESS;
      }
    vaCtrl->usr = NULL;			/* We get to use this field 	*/
    }

/* Let's just decide to not respond immediately for one	of 10 		*/
/* indications. We will respond when we are good and ready!		*/
  if (asyncResponse == SD_TRUE && pendIndCtrl == NULL)
    {
    if (indCount % 10 == 0)
      {
      pendIndCtrl = indCtrl;
      pendIndRxTime = time (NULL);
      return;
      }
    }

/* OK, go ahead and respond */
  mvlas_read_resp (indCtrl);
  }
#endif	/* (MMS_READ_EN & RESP_EN)	*/

/************************************************************************/
/************************************************************************/

#if (MMS_WRITE_EN & RESP_EN)
ST_VOID u_mvl_write_ind (MVL_IND_PEND *indCtrl)
  {
MVLAS_WRITE_CTRL *wrCtrl;
MVLAS_WR_VA_CTRL *vaCtrl;
MVL_VAR_ASSOC *va;
ST_INT i;

  ++indCount;

/* We really don't have anything to do here, but just show how to 	*/
/* access the info of interest. 					*/
  indCtrl->usr = NULL;			/* We get to use this field 	*/
  wrCtrl = &indCtrl->u.wr;
  vaCtrl = wrCtrl->vaCtrlTbl;
  for (i = 0; i < wrCtrl->numVar; ++i, ++vaCtrl)
    {
    va = vaCtrl->va;
    if (va)		/* VA was resolved, we can look it over		*/
      {
#if 0
/* Here we can set the result code if we so desire, or this can be done	*/
/* later.								*/
      vaCtrl->resp_tag = WR_RSLT_SUCCESS;
      vaCtrl->resp_tag = WR_RSLT_FAILURE;
#endif
      }
    vaCtrl->usr = NULL;			/* We get to use this field 	*/
    }

/* Let's just decide to not respond immediately for one	of 10 		*/
/* indications. We will respond when we are good and ready!		*/
  if (asyncResponse == SD_TRUE && pendIndCtrl == NULL)
    {
    if (indCount % 10 == 0)
      {
      pendIndCtrl = indCtrl;
      pendIndRxTime = time (NULL);
      return;
      }
    }

/* OK, go ahead and respond */
  mvlas_write_resp (indCtrl);
  }
#endif	/* (MMS_WRITE_EN & RESP_EN)	*/

#if (MMS_INFO_EN & RESP_EN)
ST_VOID u_mvl_info_rpt_ind (MVL_COMM_EVENT *event)
  {
INFO_REQ_INFO *info_ptr;
ST_INT j;
OBJECT_NAME *vobj;
VARIABLE_LIST *vl;
VAR_ACC_SPEC *va_spec;
MVL_VAR_ASSOC **info_va;
MVL_VAR_ASSOC *va;
ST_INT num_va;
ST_CHAR *name;
ST_INT16 remTemperature;

/* We want to get a table of pointers to VARIABLE_ASSOCIATIONS so we	*/
/* can convert the received data to local format.		 	*/

  info_ptr = (INFO_REQ_INFO *) event->u.mms.dec_rslt.data_ptr;
  va_spec = &info_ptr->va_spec;
  num_va = info_ptr->num_of_acc_result;
  info_va = (MVL_VAR_ASSOC **) chk_calloc (num_va, 
					   sizeof (MVL_VAR_ASSOC *));

  if (va_spec->var_acc_tag == VAR_ACC_NAMEDLIST)
    {
    vobj = &va_spec->vl_name;
    name = vobj->obj_name.vmd_spec;
    }
  else if (va_spec->var_acc_tag == VAR_ACC_VARLIST)
    {
    vl = (VARIABLE_LIST *) (info_ptr + 1);
    for (j = 0; j < num_va; ++j, ++vl)
      {
      if (vl->var_spec.var_spec_tag == VA_SPEC_NAMED)
        {
        vobj = &vl->var_spec.vs.name;
        name = vobj->obj_name.vmd_spec;
        if (!strcmp (name, "Temperature") && vobj->object_tag == VMD_SPEC)
          {
          va = (MVL_VAR_ASSOC *) chk_calloc (1, sizeof (MVL_VAR_ASSOC));
          info_va[j] = va;
          va->type_id = I16_TYPEID;
          va->data = &remTemperature;
          }
        }
      else
        {
        MVL_LOG_NERR0 ("InfoRpt Ind : VA Spec not named");
        }
      }
    }

  mvl_info_data_to_local (event, num_va, info_va);
  for (j = 0; j < num_va; ++j)
    {
    va = info_va[j];
    if (va != NULL)
      {
#if 0
      printf ("\nInfo rpt received for 'Temperature', value %d", 
				(int) remTemperature);
#endif
      chk_free (va);
      }
    }
  chk_free (info_va);
  }
#endif

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
#if 0	/* enable this to print when Ind received		*/
  printf ("\n ReadJournal Indication received.");
#endif
  mvlas_jread_resp (indCtrl);	/* Call Virtual Machine response funct.	*/
  }
#endif /* #if (MMS_JREAD_EN & RESP_EN) */

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

#if (MMS_STATUS_EN & RESP_EN)
/************************************************************************/
/*			u_mvl_status_ind				*/
/************************************************************************/
ST_VOID u_mvl_status_ind (MVL_IND_PEND *indCtrl)
  {
#if 0	/* enable this to print when Ind received		*/
  printf ("\n Status Indication received.");
#endif
  indCtrl->u.status.resp_info = &status_resp_info;
  mplas_status_resp (indCtrl);
  }
#endif	/* #if (MMS_STATUS_EN & RESP_EN)	*/

#if (MMS_GETNAMES_EN & RESP_EN)
/************************************************************************/
/*			u_mvl_namelist_ind				*/
/************************************************************************/
ST_VOID u_mvl_namelist_ind (MVL_IND_PEND *indCtrl)
  {
#if 0	/* enable this to print when Ind received		*/
  printf ("\n GetNameList Indication received.");
#endif
  mvlas_namelist_resp (indCtrl);
  }
#endif	/* #if (MMS_NAMELIST_EN & RESP_EN)	*/

#if (MMS_GETVAR_EN & RESP_EN)
/************************************************************************/
/*			u_mvl_getvar_ind				*/
/************************************************************************/
ST_VOID u_mvl_getvar_ind (MVL_IND_PEND *indCtrl)
  {
#if 0	/* enable this to print when Ind received		*/
  printf ("\n GetVariableAccessAttributes Indication received.");
#endif
  mvlas_getvar_resp (indCtrl);
  }
#endif	/* #if (MMS_GETVAR_EN & RESP_EN)	*/

#if (MMS_GET_DOM_EN & RESP_EN)
/************************************************************************/
/*			u_mvl_getdom_ind				*/
/************************************************************************/
ST_VOID u_mvl_getdom_ind (MVL_IND_PEND *indCtrl)
  {
#if 0	/* enable this to print when Ind received		*/
  printf ("\n GetDomainAttributes Indication received.");
#endif
  mvlas_getdom_resp (indCtrl);
  }
#endif	/* #if (MMS_GET_DOM_EN & RESP_EN)	*/

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
ST_CHAR *dummy_name [5] = {"file1", "file2", "file3", "file4", "file5"};

ST_VOID u_mvl_fdir_ind (MVL_IND_PEND *indCtrl)
  {
#if defined(_WIN32)
  mvlas_fdir_resp (indCtrl);
#else
ST_INT j;
MVLAS_FDIR_CTRL *fdir_ctrl;
MVL_FDIR_RESP_INFO resp_info;
ST_CHAR *tmp_dir_ent_array;	/* ptr to temporary array of dir entries*/

  fdir_ctrl = &indCtrl->u.fdir;

#if 0	/* enable this to print when Ind received		*/
  printf ("\n FileDirectory Indication received.");
  printf ("\n   fs_filename = %s", fdir_ctrl->fs_filename);
  printf ("\n   ca_filename = %s", fdir_ctrl->ca_filename);
#endif

  resp_info.num_dir_ent = sizeof (dummy_name) / sizeof (ST_CHAR *);
  resp_info.more_follows = SD_FALSE;
  tmp_dir_ent_array = 
    (ST_CHAR *) chk_calloc (resp_info.num_dir_ent, sizeof(MVL_DIR_ENT));
  resp_info.dir_ent = (MVL_DIR_ENT *) tmp_dir_ent_array;

  for (j = 0; j < resp_info.num_dir_ent;  j++)
    {
    resp_info.dir_ent[j].fsize = 100 * j;
    resp_info.dir_ent[j].mtimpres = SD_TRUE;
    resp_info.dir_ent[j].mtime = time (NULL);
    strcpy (resp_info.dir_ent[j].filename, dummy_name [j]);
    }

  fdir_ctrl->resp_info = &resp_info;
  mplas_fdir_resp (indCtrl);
  /* NOTE: indCtrl freed by resp function, so don't use it after this point.*/
  chk_free (tmp_dir_ent_array);
#endif	/* !_WIN32	*/
  }
#endif	/* #if (MMS_FDIR_EN & RESP_EN)	*/

#if (MMS_FDELETE_EN & RESP_EN)
/************************************************************************/
/*			u_mvl_fdelete_ind				*/
/************************************************************************/
ST_VOID u_mvl_fdelete_ind (MVL_IND_PEND *indCtrl)
  {
MVLAS_FDELETE_CTRL *fdelete_ctrl = &indCtrl->u.fdelete;

#if 0	/* enable this to print when Ind received		*/
  printf ("\n FileDelete Indication received for file '%s'.",
          fdelete_ctrl->filename);
#endif

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

#if 0	/* enable this to print when Ind received		*/
  printf ("\n FileRename Indication received for current file name '%s' \n and new file name '%s' .\n",
           frename_ctrl->curfilename, frename_ctrl->newfilename);
#endif

  /* Use the ANSI "rename" function if available on your OS.		*/
  /* Otherwise, use the appropriate function for your OS.		*/
  if (rename (frename_ctrl->curfilename, frename_ctrl->newfilename))
    {
#if 0	/* enable this to print error info	*/
    printf ("\n Error (%d) renaming file: %s\n", errno, strerror(errno));
#endif
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
#if 0	/* enable this to print when Ind received		*/
  printf ("\n Obtainfile Indication received for source file '%s' \n and destination file '%s'.\n",
    indCtrl->u.obtfile.srcfilename, indCtrl->u.obtfile.destfilename);
#endif

  mvlas_obtfile_resp (indCtrl);      /* indicate state machine starting */
  }
#endif	/* #if (MMS_OBTAINFILE_EN & RESP_EN)	*/

#if (MMS_DEFVLIST_EN & RESP_EN)
/************************************************************************/
/*			u_mvl_defvlist_ind				*/
/************************************************************************/
ST_VOID u_mvl_defvlist_ind (MVL_IND_PEND *indCtrl)
  {
#if 0	/* enable this to print when Ind received		*/
  printf ("\n DefineNamedVariableList Indication received.");
#endif
  mvlas_defvlist_resp (indCtrl);
  }
#endif	/* #if (MMS_DEFVLIST_EN & RESP_EN)	*/

#if (MMS_GETVLIST_EN & RESP_EN)
/************************************************************************/
/*			u_mvl_getvlist_ind				*/
/************************************************************************/
ST_VOID u_mvl_getvlist_ind (MVL_IND_PEND *indCtrl)
  {
#if 0	/* enable this to print when Ind received		*/
  printf ("\n GetNamedVariableListAttributes Indication received.");
#endif
  mvlas_getvlist_resp (indCtrl);
  }
#endif	/* #if (MMS_GETVLIST_EN & RESP_EN)	*/

#if (MMS_DELVLIST_EN & RESP_EN)
/************************************************************************/
/*			u_mvl_delvlist_ind				*/
/************************************************************************/
ST_VOID u_mvl_delvlist_ind (MVL_IND_PEND *indCtrl)
  {
#if 0	/* enable this to print when Ind received		*/
  printf ("\n DeleteNamedVariableList Indication received.");
#endif
  mvlas_delvlist_resp (indCtrl);
  }
#endif	/* #if (MMS_DELVLIST_EN & RESP_EN)	*/

#if (MMS_JINIT_EN & RESP_EN)
/************************************************************************/
/*			u_mvl_jinit_ind					*/
/************************************************************************/
ST_VOID u_mvl_jinit_ind (MVL_IND_PEND *indCtrl)
  {
#if 0	/* enable if needed	*/
JINIT_REQ_INFO *req_info = indCtrl->u.jinit.req_info;
#endif
JINIT_RESP_INFO resp_info;

#if 0	/* enable this to print when Ind received		*/
  printf ("\n InitializeJournal Indication received.");
#endif

#if 1	/* DEBUG: user should replace this with code to Initialize Journal*/
  resp_info.del_entries = 5;
#endif

  indCtrl->u.jinit.resp_info = &resp_info;
  mplas_jinit_resp (indCtrl);
  }
#endif	/* #if (MMS_JINIT_EN & RESP_EN)	*/

#if (MMS_JSTAT_EN & RESP_EN)
/************************************************************************/
/*			u_mvl_jstat_ind					*/
/************************************************************************/
ST_VOID u_mvl_jstat_ind (MVL_IND_PEND *indCtrl)
  {
JSTAT_REQ_INFO *req_info = indCtrl->u.jstat.req_info;
JSTAT_RESP_INFO resp_info;
MVL_JOURNAL_CTRL *jCtrl;

#if 0	/* enable this to print when Ind received		*/
  printf ("\n ReportJournalStatus Indication received.");
#endif

#if 1	/* DEBUG: could all this be done automatically by MVL?	*/
/* First find our Journal Control element */
  jCtrl = mvl_vmd_find_jou (&mvl_vmd, &req_info->jou_name, indCtrl->event->net_info);

  if (jCtrl == NULL)
    {
    _mplas_err_resp (indCtrl, 2, 1); 	/* object undefined */
    MVL_LOG_NERR1 ("ReportJournalStatus - could not find journal '%s'", 
				req_info->jou_name.obj_name.vmd_spec);
    return;
    }  

  resp_info.cur_entries = jCtrl->numEntries;
  resp_info.mms_deletable = jCtrl->mms_deletable;
#endif

  indCtrl->u.jstat.resp_info = &resp_info;
  mplas_jstat_resp (indCtrl);
  }
#endif	/* #if (MMS_JSTAT_EN & RESP_EN)	*/

#if (MMS_GETCL_EN & RESP_EN)
/************************************************************************/
/*			u_mvl_getcl_ind					*/
/************************************************************************/
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
#endif	/* #if (MMS_GETCL_EN & RESP_EN)	*/

#if (MMS_CONCLUDE_EN & RESP_EN)
/************************************************************************/
/*			u_mvl_concl_ind					*/
/************************************************************************/
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
#endif	/* #if (MMS_CONCLUDE_EN & RESP_EN)	*/

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
MVL_AA_OBJ_CTRL *aa;
static INIT_INFO initRespInfo;

  printf ("\n Initiate indication for local AR '%s'", cc->bind_ctrl->ar_name);
  initRespInfo.mms_p_context_pres = SD_TRUE;
  initRespInfo.max_segsize_pres = SD_TRUE;
  initRespInfo.max_segsize = mvl_cfg_info->max_msg_size;
  initRespInfo.maxreq_calling = 5;
  initRespInfo.maxreq_called = 5;
  initRespInfo.max_nest_pres = SD_TRUE;
  initRespInfo.max_nest = 5;
  initRespInfo.mms_detail_pres = SD_TRUE;
  initRespInfo.version = 1;
  initRespInfo.num_cs_init = 0;
  initRespInfo.core_position = 0;
  initRespInfo.param_supp[0] = m_param[0];
  initRespInfo.param_supp[1] = m_param[1];
  memcpy (initRespInfo.serv_supp, m_service_resp,11);

  aa = (MVL_AA_OBJ_CTRL *) cc->aa_objs;

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
MVL_NET_FILE *open_file;
  printf ("\n Disconnect indication for local AR '%s'", cc->bind_ctrl->ar_name);
  cc->rem_vmd = NULL;
  /* this is a good place to clear up any outstanding indication	*/
  /* control structures.						*/
  mvl_clr_ind_que (cc);

  /* On Abort, close open files on this connection, and free up resources.*/
  if (discType == MVL_ACSE_ABORT_IND)
    {
    while ((open_file = list_get_first (&cc->file_list)) != NULL)
      {
      fclose (open_file->fp);
      chk_free (open_file);
      }
    }
  }

/************************************************************************/
/************************************************************************/
/* MISC. FUNCTIONS							*/
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
  
  /*  At initialization, install a SLOGIPC command handler. The      */
  /*  build in SLOGIPC handler just receives the command and put's   */
  /*  on a list to be handled by the application at it's leisure ... */
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
  spareMem = (ST_CHAR *) malloc (500);

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

