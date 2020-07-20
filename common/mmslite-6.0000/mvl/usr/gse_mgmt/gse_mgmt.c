/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	2006 - 2010, All Rights Reserved		        */
/*									*/
/* MODULE NAME : gse_mgmt.c    						*/
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
/* 06/18/10  JRB	   Repl mvl_comm_serve with subnet_serve.	*/
/*			   Repl mvl_start_acse with clnp_init, add clnp_end.*/
/*			   Del call to obsolete clnp_snet_read_hook_add	*/
/*			   and related code (handled by new subnet_serve).*/
/*			   Del #ifdef MOSI.				*/
/* 02/26/09  JRB     07    Add function to satisfy linker.		*/
/* 06/12/08  GLB     06    Add slog_start                               */
/* 05/14/08  GLB     05    Added slog_ipc_std_cmd_service		*/
/* 03/25/08  JRB     04    Fix more linux warnings.			*/
/* 02/16/07  JRB     03    Clarify some log messages.			*/
/* 11/27/06  CRM     02    Fix linux warnings.				*/
/* 11/03/06  CRM     01    Created.					*/
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
#include "mvl_log.h"
#include "uca_obj.h"
#include "tp4api.h"

#include "goose.h"
#include "glbsem.h"
#include "gse_mgmt.h"
#include "subnet.h"

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



/* Test Data */

ST_CHAR *test_str[] = 
	  { "1111111111",
	    "22222222222222222222",
	    "333333333333333333333333333333",
	    "4444444444444444444444444444444444444444",
	    "55555555555555555555555555555555555555555555555555",
	    "666666666666666666666666666666666666666666666666666666666666",
	    "77777777777777777777777777777777777777777777777777777777777777777"
	  };

/************************************************************************/
/*			gse_mgmt_ref_req_create				*/
/************************************************************************/
GSE_REF_REQ *gse_mgmt_ref_req_create (ST_UINT32 stateID, ST_CHAR *ident, ST_INT numIntegers)
  {
GSE_REF_REQ *ctrl;
ST_INT index;

  ctrl = (GSE_REF_REQ *) chk_calloc (1, sizeof (GSE_REF_REQ));
  memset (ctrl, 0, sizeof(GSE_REF_REQ));

  ctrl->stateID = stateID;
  strncpy( ctrl->ident, ident, MAX_VSTRING_BUF);
  ctrl->numIntegers = numIntegers;
  ctrl->offset = (ST_UINT32 *) chk_calloc (numIntegers, sizeof (ST_UINT32));

  for (index = numIntegers - 1; index >= 0; index--)
    ctrl->offset[index] = 1 + index;

  return (ctrl);
  }

/************************************************************************/
/*			gse_mgmt_ref_req_destroy			*/
/************************************************************************/
ST_VOID gse_mgmt_ref_req_destroy (GSE_REF_REQ *gseRefReq)
  {

  if (gseRefReq)
    {
    if (gseRefReq->offset)
      chk_free (gseRefReq->offset);

    chk_free (gseRefReq);
    }
  }
/************************************************************************/
/*			gse_mgmt_ref_rsp_create				*/
/************************************************************************/
GSE_REF_RSP *gse_mgmt_ref_rsp_create (ST_UINT32 stateID, ST_CHAR *ident, ST_INT numResults)
  {
GSE_REF_RSP *ctrl;
ST_INT index;

  ctrl = (GSE_REF_RSP *) chk_calloc (1, sizeof (GSE_REF_RSP));
  memset (ctrl, 0, sizeof(GSE_REF_RSP));

  ctrl->stateID = stateID;
  ctrl->confRev = numResults + 1;
  strncpy (ctrl->ident, ident, MAX_VSTRING_BUF);
  strcpy (ctrl->datSet, "Positive Response datSet String");
  ctrl->numResults = numResults;
  ctrl->result = (REF_REQ_RESULTS *) chk_calloc (numResults, sizeof (REF_REQ_RESULTS));

  for (index = 0; index < numResults; index++)
    {
    ctrl->result[index].error = RSP_OTHER;
    strcpy(ctrl->result[index].reference, "Positive Response Reference String");
    ctrl->result[index].rsp_type = SD_SUCCESS;

    if (index == 0 || index == 2)
      ctrl->result[index].rsp_type = SD_FAILURE;
    }

  return (ctrl);
  }


/************************************************************************/
/*			  gse_mgmt_ref_rsp_destroy			*/
/************************************************************************/
ST_VOID gse_mgmt_ref_rsp_destroy (GSE_REF_RSP *refRsp)
  {

  if (refRsp)
    {
    if (refRsp->result)
      chk_free (refRsp->result);

    chk_free (refRsp);
    }
  }

/************************************************************************/
/*			gse_mgmt_off_req_create				*/
/************************************************************************/
GSE_OFFSET_REQ *gse_mgmt_off_req_create (ST_UINT32 stateID, ST_CHAR *ident, ST_INT numVStrings, ST_CHAR **references)
  {
GSE_OFFSET_REQ *ctrl;

  ctrl = (GSE_OFFSET_REQ *) chk_calloc (1, sizeof (GSE_OFFSET_REQ));
  memset (ctrl, 0, sizeof(GSE_OFFSET_REQ));

  ctrl->stateID = stateID;
  strncpy( ctrl->ident, ident, MAX_VSTRING_BUF);
  ctrl->numVStrings = numVStrings;
  
  ctrl->references = references;
  return (ctrl);
  }

/************************************************************************/
/*			gse_mgmt_off_req_destroy			*/
/************************************************************************/
ST_VOID gse_mgmt_off_req_destroy (GSE_OFFSET_REQ *gseOffReq)
  {
  if (gseOffReq)
    chk_free (gseOffReq);
  }

/************************************************************************/
/*			gse_mgmt_off_rsp_create				*/
/************************************************************************/
GSE_OFFSET_RSP *gse_mgmt_off_rsp_create (ST_UINT32 stateID, ST_CHAR *ident, ST_INT numResults)
  {
GSE_OFFSET_RSP *ctrl;
ST_INT index;

  ctrl = (GSE_OFFSET_RSP *) chk_calloc (1, sizeof (GSE_OFFSET_RSP));
  memset (ctrl, 0, sizeof(GSE_OFFSET_RSP));

  ctrl->stateID = stateID;
  ctrl->confRev = numResults + 1;
  strncpy (ctrl->ident, ident, MAX_VSTRING_BUF);
  strcpy (ctrl->datSet, "Positive Response datSet String");
  ctrl->numResults = numResults;
  ctrl->result = (OFFSET_REQ_RESULTS *) chk_calloc (numResults, sizeof (OFFSET_REQ_RESULTS));

  for (index = 0; index < numResults; index++)
    {
    ctrl->result[index].offset = index + 10;
    ctrl->result[index].error = RSP_OTHER;
    ctrl->result[index].rsp_type = SD_SUCCESS;

    if (index == 0 || index == 2)
      ctrl->result[index].rsp_type = SD_FAILURE;
    }

  return (ctrl);
  }


/************************************************************************/
/*			  gse_mgmt_off_rsp_destroy			*/
/************************************************************************/
ST_VOID gse_mgmt_off_rsp_destroy (GSE_OFFSET_RSP *offRsp)
  {

  if (offRsp)
    {
    if (offRsp->result)
      chk_free (offRsp->result);

    chk_free (offRsp);
    }
  }

/************************************************************************/
/*			gse_mgmt_err_rsp_create				*/
/************************************************************************/
GSE_GLB_ERR_RSP *gse_mgmt_err_rsp_create (ST_UINT32 stateID, ST_CHAR *ident, ST_INT rsp_type, ST_INT numResults)
  {
GSE_GLB_ERR_RSP *ctrl;

  ctrl = (GSE_GLB_ERR_RSP *) chk_calloc (1, sizeof (GSE_GLB_ERR_RSP));
  memset (ctrl, 0, sizeof(GSE_GLB_ERR_RSP));

  ctrl->stateID = stateID;
  strncpy (ctrl->ident, ident, MAX_VSTRING_BUF);
  ctrl->glbError = rsp_type;

  return (ctrl);
  }


/************************************************************************/
/*			  gse_mgmt_err_rsp_destroy			*/
/************************************************************************/
ST_VOID gse_mgmt_err_rsp_destroy (GSE_GLB_ERR_RSP *errRsp)
  {

  if (errRsp)
    chk_free (errRsp);
  }

/************************************************************************/
/*				doCommService 				*/
/************************************************************************/
ST_VOID doCommService ()
  {
    wait_any_event (1000);	/* Wait 1000 milliseconds		*/
    
    /* Call subnet_serve to receive and process GSE Management messages.*/
    /* NOTE: mvl_comm_serve now processes ONLY MMS messages (not needed)*/
    while (subnet_serve ())	/* Perform communications service 	*/
      {				/* Loop until it returns 0		*/
      }

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

/************************************************************************/
/*			kbService ()					*/
/************************************************************************/
static ST_VOID kbService (ST_VOID)
  {
ST_CHAR c;

  if (kbhit ())		/* Report test keyboard input */
    {
#if defined(_WIN32) || defined(__QNX__)
    c = getch ();
#else
    c = (ST_CHAR) getchar ();	/* works only if term_init called first	*/
#endif

    if (c == 'x')
      doIt = SD_FALSE;	/* This triggers graceful exit	*/

    if (c == '?')
      {
      printf ("\n x : Exit");
      }
    }
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
ST_UCHAR *gse_mgmt_pdu;
ST_INT gse_mgmt_pdu_len;
GSE_REF_REQ *refReqCtrl;
GSE_OFFSET_REQ *offReqCtrl;
ST_UCHAR gse_enc_buf[MAX_GSE_MGMT_ENCODE_BUF];
ETYPE_INFO etype; 
ST_CHAR *str = "0123456789";
GSE_GLB_ERR_RSP *errRspCtrl;
GSE_OFFSET_RSP *offRspCtrl;
GSE_REF_RSP *refRspCtrl;
ST_UCHAR dstMac [] = {1,1,1,1,1,1};	/* Use same DST MAC for all messages	*/
ST_RET ret;


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

#if defined(UCA_SMP)
  smp_init ();
#endif

  doIt = SD_TRUE;

  SLOGALWAYS0 ("Initializing ...");

  init_acse ();		/* Start the lower layer subsystem		*/

  marker = chk_malloc (1);		/* Marker			*/

  /* CRITICAL: to receive Multicast packets (for GOOSE, SMPVAL, etc.),	*/
  /* must set multicast filter using clnp_snet_set_multicast_filter	*/
  /* or clnp_snet_rx_all_multicast_start (accept ALL multicast packets).*/
  /* NOTE: this is AFTER goose_init to override filter set by goose_init.*/

  if ((ret = clnp_snet_rx_all_multicast_start()) != SD_SUCCESS)
    {
    SLOGALWAYS1 ("Error 0x%04X setting multicast filter.", ret);
    printf      ("Error 0x%04X setting multicast filter.\n", ret);
    }

  /* Set the ^c catcher */
  signal (SIGINT, ctrlCfun);

#if !defined(NO_KEYBOARD) && !defined(_WIN32) && !defined(__QNX__)
  term_init ();	/* makes getchar work right	*/
#endif

  etype.tci     = ETYPE_TCI_GSE;
  etype.etypeID = ETYPE_TYPE_GSE;
  etype.appID   = 0x4000;	

  printf ("\n Sending GSE Management messages ... ");

  SLOGALWAYS0 ("GSE Management Get Go Ref Request Encode ... ");
  refReqCtrl = gse_mgmt_ref_req_create (1, str, 27);
  gse_mgmt_pdu = getGoRefReqEncode (refReqCtrl, gse_enc_buf, MAX_GSE_MGMT_ENCODE_BUF, &gse_mgmt_pdu_len, 
		    &etype, dstMac, clnp_param.loc_mac); 
  gse_mgmt_ref_req_destroy (refReqCtrl);  
  SLOGALWAYSH (gse_mgmt_pdu_len, gse_mgmt_pdu);
  clnp_snet_write_raw (gse_mgmt_pdu, gse_mgmt_pdu_len);

  SLOGALWAYS0 ("GSE Management Get GOOSE Ele Num Request Encode ... ");
  offReqCtrl = gse_mgmt_off_req_create (2, str, 7, test_str);
  gse_mgmt_pdu = getGOOSEEleNumReqEncode (offReqCtrl, gse_enc_buf, MAX_GSE_MGMT_ENCODE_BUF, &gse_mgmt_pdu_len, 
		    &etype, dstMac, clnp_param.loc_mac); 
  gse_mgmt_off_req_destroy (offReqCtrl);  
  SLOGALWAYSH (gse_mgmt_pdu_len, gse_mgmt_pdu);
  clnp_snet_write_raw (gse_mgmt_pdu, gse_mgmt_pdu_len);

  SLOGALWAYS0 ("GSE Management Get Gs Ref Request Encode ... ");
  refReqCtrl = gse_mgmt_ref_req_create (3, str, 40);
  gse_mgmt_pdu = getGsRefReqEncode (refReqCtrl, gse_enc_buf, MAX_GSE_MGMT_ENCODE_BUF, &gse_mgmt_pdu_len, 
		    &etype, dstMac, clnp_param.loc_mac); 
  gse_mgmt_ref_req_destroy (refReqCtrl);  
  SLOGALWAYSH (gse_mgmt_pdu_len, gse_mgmt_pdu);
  clnp_snet_write_raw (gse_mgmt_pdu, gse_mgmt_pdu_len);

  SLOGALWAYS0 ("GSE Management Get GSSE Data Offset Request Encode ... ");
  offReqCtrl = gse_mgmt_off_req_create (4, str, 4, test_str);
  gse_mgmt_pdu = getGSSEDataOffsetReqEncode (offReqCtrl, gse_enc_buf, MAX_GSE_MGMT_ENCODE_BUF, &gse_mgmt_pdu_len, 
		    &etype, dstMac, clnp_param.loc_mac); 
  gse_mgmt_off_req_destroy (offReqCtrl);  
  SLOGALWAYSH (gse_mgmt_pdu_len, gse_mgmt_pdu);
  clnp_snet_write_raw (gse_mgmt_pdu, gse_mgmt_pdu_len);

  SLOGALWAYS0 ("GSE Management Get Go Reference Response Encode ... ");
  refRspCtrl = gse_mgmt_ref_rsp_create (5, str, 5);
  gse_mgmt_pdu = getGoRefRspEncode (refRspCtrl, gse_enc_buf, MAX_GSE_MGMT_ENCODE_BUF, &gse_mgmt_pdu_len, 
		    &etype, dstMac, clnp_param.loc_mac); 
  gse_mgmt_ref_rsp_destroy (refRspCtrl);
  SLOGALWAYSH (gse_mgmt_pdu_len, gse_mgmt_pdu);
  clnp_snet_write_raw (gse_mgmt_pdu, gse_mgmt_pdu_len);

  SLOGALWAYS0 ("GSE Management Get GOOSE Element Number Response Encode ... ");
  offRspCtrl = gse_mgmt_off_rsp_create (6, str, 6);
  gse_mgmt_pdu = getGOOSEEleNumRspEncode (offRspCtrl, gse_enc_buf, MAX_GSE_MGMT_ENCODE_BUF, &gse_mgmt_pdu_len, 
		    &etype, dstMac, clnp_param.loc_mac); 
  gse_mgmt_off_rsp_destroy (offRspCtrl);
  SLOGALWAYSH (gse_mgmt_pdu_len, gse_mgmt_pdu);
  clnp_snet_write_raw (gse_mgmt_pdu, gse_mgmt_pdu_len);

  SLOGALWAYS0 ("GSE Management Get Gs Ref Response Encode ... ");
  refRspCtrl = gse_mgmt_ref_rsp_create (7, str, 7);
  gse_mgmt_pdu = getGsRefRspEncode (refRspCtrl, gse_enc_buf, MAX_GSE_MGMT_ENCODE_BUF, &gse_mgmt_pdu_len, 
		    &etype, dstMac, clnp_param.loc_mac); 
  gse_mgmt_ref_rsp_destroy (refRspCtrl);
  SLOGALWAYSH (gse_mgmt_pdu_len, gse_mgmt_pdu);
  clnp_snet_write_raw (gse_mgmt_pdu, gse_mgmt_pdu_len);

  SLOGALWAYS0 ("GSE Management Get GSSE Data Offset Response Encode ... ");
  offRspCtrl = gse_mgmt_off_rsp_create (8, str, 8);
  gse_mgmt_pdu = getGSSEDataOffsetRspEncode (offRspCtrl, gse_enc_buf, MAX_GSE_MGMT_ENCODE_BUF, &gse_mgmt_pdu_len, 
		    &etype, dstMac, clnp_param.loc_mac); 
  gse_mgmt_off_rsp_destroy (offRspCtrl);
  SLOGALWAYSH (gse_mgmt_pdu_len, gse_mgmt_pdu);
  clnp_snet_write_raw (gse_mgmt_pdu, gse_mgmt_pdu_len);

  SLOGALWAYS0 ("GSE Management Get Gs Ref Response Encode ... ");
  refRspCtrl = gse_mgmt_ref_rsp_create (7, str, 6);
  gse_mgmt_pdu = getGsRefRspEncode (refRspCtrl, gse_enc_buf, MAX_GSE_MGMT_ENCODE_BUF, &gse_mgmt_pdu_len, 
		    &etype, dstMac, clnp_param.loc_mac); 
  gse_mgmt_ref_rsp_destroy (refRspCtrl);
  SLOGALWAYSH (gse_mgmt_pdu_len, gse_mgmt_pdu);
  clnp_snet_write_raw (gse_mgmt_pdu, gse_mgmt_pdu_len);

  SLOGALWAYS0 ("GSE Management Global Error (GLB_ERR_CTRL_BLK_CONFIG_ERR) Response Encode ... ");
  errRspCtrl = gse_mgmt_err_rsp_create (5, str, GLB_ERR_CTRL_BLK_CONFIG_ERR, 4);
  gse_mgmt_pdu = getGlbErrorRspEncode (errRspCtrl, gse_enc_buf, MAX_GSE_MGMT_ENCODE_BUF, &gse_mgmt_pdu_len, 
		    &etype, dstMac, clnp_param.loc_mac); 
  gse_mgmt_err_rsp_destroy (errRspCtrl);
  SLOGALWAYSH (gse_mgmt_pdu_len, gse_mgmt_pdu);
  clnp_snet_write_raw (gse_mgmt_pdu, gse_mgmt_pdu_len);

  SLOGALWAYS0 ("GSE Management Global Error (GLB_ERR_RSP_TOO_LARGE) Response Encode ... ");
  errRspCtrl = gse_mgmt_err_rsp_create (6, str, GLB_ERR_RSP_TOO_LARGE, 5);
  gse_mgmt_pdu = getGlbErrorRspEncode (errRspCtrl, gse_enc_buf, MAX_GSE_MGMT_ENCODE_BUF, &gse_mgmt_pdu_len, 
		    &etype, dstMac, clnp_param.loc_mac); 
  gse_mgmt_err_rsp_destroy (errRspCtrl);
  SLOGALWAYSH (gse_mgmt_pdu_len, gse_mgmt_pdu);
  clnp_snet_write_raw (gse_mgmt_pdu, gse_mgmt_pdu_len);

  SLOGALWAYS0 ("GSE Management Global Error (GLB_ERR_UNKNOWN_CTRL_BLK) Response Encode ... ");
  errRspCtrl = gse_mgmt_err_rsp_create (7, str, GLB_ERR_UNKNOWN_CTRL_BLK, 6);
  gse_mgmt_pdu = getGlbErrorRspEncode (errRspCtrl, gse_enc_buf, MAX_GSE_MGMT_ENCODE_BUF, &gse_mgmt_pdu_len, 
		    &etype, dstMac, clnp_param.loc_mac); 
  gse_mgmt_err_rsp_destroy (errRspCtrl);
  SLOGALWAYSH (gse_mgmt_pdu_len, gse_mgmt_pdu);
  clnp_snet_write_raw (gse_mgmt_pdu, gse_mgmt_pdu_len);

  SLOGALWAYS0 ("GSE Management Global Error (GLB_ERR_OTHER) Response Encode ... ");
  errRspCtrl = gse_mgmt_err_rsp_create (8, str, GLB_ERR_OTHER, 7);
  gse_mgmt_pdu = getGlbErrorRspEncode (errRspCtrl, gse_enc_buf, MAX_GSE_MGMT_ENCODE_BUF, &gse_mgmt_pdu_len, 
		    &etype, dstMac, clnp_param.loc_mac); 
  gse_mgmt_err_rsp_destroy (errRspCtrl);
  SLOGALWAYSH (gse_mgmt_pdu_len, gse_mgmt_pdu);
  clnp_snet_write_raw (gse_mgmt_pdu, gse_mgmt_pdu_len);

  printf ("\n\n Entering Receive mode, hit ^c or 'x' to exit ...\n");

  while (doIt)
    {
    doCommService ();
    kbService ();
    }

  /* clnp_init called at startup, so must call clnp_end to clean up.*/
  clnp_end ();

#if !defined(NO_KEYBOARD) && !defined(_WIN32) && !defined(__QNX__)
  term_rest ();	/* Must be called before exit if term_init used.	*/
#endif

  /* Log only buffers allocated AFTER startup & not yet freed.	*/
  dyn_mem_ptr_status2 (marker);
#if 1	/* add this only if problems detected	*/
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
    printf ("\r Received GOOSE Message %ld", rxCount);
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
  /* clnp_init instead of mvl_start_acse here (much less code).		*/
  ret = clnp_init (1200);
  if (ret != SD_SUCCESS)
    {
    printf ("clnp_init () failed, err = %d.\n", ret );
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
  
  /* At initialization, install a SLOGIPC command handler. The      */
  /* build in SLOGIPC handler just receives the command and put's   */
  /* on a list to be handled by the application at it's leisure ... */
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
/* Needed to satisfy the linker, but should NEVER be called.		*/ 
/************************************************************************/
ST_RET u_mvlu_resolve_leaf_ref (ST_CHAR *leafName, ST_INT *setFlagsIo, 
				ST_CHAR *refText, ST_RTREF *refOut)
  {
  return (SD_FAILURE);	/* should never be called	*/
  }

