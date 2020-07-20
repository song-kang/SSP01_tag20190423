#if defined(UCA_SMP)
/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1997 - 2000, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : smp_mst.c						*/
/* PRODUCT(S)  : UCA Station Management Protocol (UCA SMP).		*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module implements the MASTER side of the UCA Station	*/
/*	Management Protocol.						*/
/* Implementation notes:						*/
/*	If the SMP modules are incorporated within a Master application	*/
/*	using the SISCO's Reduced Stack then this module needs to be	*/
/*	compiled with the REDUCED_STACK define.				*/
/*	If the SMP modules are incorporated within a Master application	*/
/*	using the SISCO's Trim7 Stack then no define is needed, this	*/
/*	is the default implementation of SMP.				*/
/*	In addition, the SMP modules may be incorporated within a Master*/
/*	application which does not link to any SISCO's stack. In this	*/
/*	case the Application must interface to one of subnetwork modules*/
/*	and process the received messages in the way one of the stack	*/
/*	would.								*/
/*									*/
/*	Implemented Services:						*/
/*		Time Synchronization					*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 10/04/07  MDE    12     Tweaked LOGCFG_VALUE_GROUP/LOGCFGX_VALUE_MAP	*/
/* 05/23/05  EJV    11     Add smpLogMaskMapCtrl for parsing logcfg.xml	*/
/* 04/27/00  MDE    10     Lint cleanup					*/
/* 01/24/00  EJV    09     PHARLAP_ETS: time is recorded in the driver.	*/
/* 12/20/99  EJV    08     Added SD_CONST modifiers			*/
/* 02/05/99  JRB    07     ALWAYS set subnet_type in smp_init.		*/
/*			   In smp_time_sync_req & smp_decode_uca_pdu,	*/
/*			   chk that init is done by checking subnet_type*/
/* 12/28/98  EJV    06     Migrated to updated SLOG interface		*/
/* 12/22/98  EJV    05     Reorganized logging macros.			*/
/*			   Do not use global vars in smp_set_mac_str.	*/
/*			   Merged smp_process_timesync_pdu to other fun.*/
/*			   Deleted some globals or moved them into funs.*/
/*			   Use |= to set error logging.			*/
/* 02/18/98  EJV    04     Moved initialization of glb vars into func.	*/
/* 11/17/97  EJV    03     Added hex logging				*/
/* 10/20/97  EJV    02     Moved user func to smp_usr.c			*/
/* 09/23/97  EJV    01     Created					*/
/************************************************************************/
#include "glbtypes.h"
#include "sysincs.h"

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;  /* Define for SLOG mechanism    */
#endif

#include "smp_log.h"
#include "smp_usr.h"
#include "smp.h"
#if defined(REDUCED_STACK)
#include "adlc_usr.h"
#else
#include "clnp_usr.h"
#include "clnp_sne.h"
#endif  /* !REDUCED_STACK */

	/*--------------------------------------*/
	/* Logging vars				*/
	/*--------------------------------------*/

/* smp_debug_sel global var is used to turn on/off different types of logging.	*/

ST_UINT smp_debug_sel;

#ifdef DEBUG_SISCO
SD_CONST ST_CHAR *SD_CONST _smp_err_logstr = "SMP_LOG_ERR";
SD_CONST ST_CHAR *SD_CONST _smp_req_logstr = "SMP_LOG_REQ";
SD_CONST ST_CHAR *SD_CONST _smp_ind_logstr = "SMP_LOG_IND";
SD_CONST ST_CHAR *SD_CONST _smp_deb_logstr = "SMP_LOG_DEB";
SD_CONST ST_CHAR *SD_CONST _smp_hex_logstr = "SMP_LOG_HEX";

LOGCFGX_VALUE_MAP smpLogMaskMaps[] =
  {
    {"SMP_LOG_ERR",	SMP_LOG_ERR,	&smp_debug_sel,	_LOGCFG_DATATYPE_UINT_MASK, NULL, "Error"},
    {"SMP_LOG_REQ",	SMP_LOG_REQ,	&smp_debug_sel,	_LOGCFG_DATATYPE_UINT_MASK, NULL, "Request"},
    {"SMP_LOG_IND",	SMP_LOG_IND,	&smp_debug_sel,	_LOGCFG_DATATYPE_UINT_MASK, NULL, "Indication"},
    {"SMP_LOG_DEB",	SMP_LOG_DEB,	&smp_debug_sel,	_LOGCFG_DATATYPE_UINT_MASK, NULL, "Debug"},
    {"SMP_LOG_HEX",	SMP_LOG_HEX,	&smp_debug_sel,	_LOGCFG_DATATYPE_UINT_MASK, NULL, "Hex"}
  };

LOGCFG_VALUE_GROUP smpLogMaskMapCtrl =
  {
  {NULL,NULL},
  "SmpLogMasks",	       /* Parent Tag */
  sizeof(smpLogMaskMaps)/sizeof(LOGCFGX_VALUE_MAP),
  smpLogMaskMaps
  };
#endif /* DEBUG_SISCO */

	/*--------------------------------------------------------------*/
	/* This is the authentication for the Time Synchronization	*/
	/*--------------------------------------------------------------*/

static ST_UCHAR smp_auth [SMP_SIZE_OF_AUTH];	/* unused, all 0's	*/


	/*------------------------------------------------------*/
	/* Pre-encoded PDUs to send to the Slave		*/
	/* First 3 bytes is LLC encoding (no ASN1 encoding)	*/
	/*------------------------------------------------------*/

static ST_UCHAR  smp_err_pdu [6];
static ST_UCHAR *smp_err_code_ptr;		/* error code ptr	*/
	 /* error code in UCA ErrorPDU must be set before sending	*/

static ST_UCHAR smp_measure_req_pdu [7];

static ST_UCHAR smp_sync_req_pdu [23];
	/* needs ASN1 encoding, max buf size needed:	*/
	/* 3 +				LLC		*/
	/* 2 + 2					*/
	/* 2+SMP_SIZE_OF_TIMES(6) +	request-time	*/
	/* 2+SMP_SIZE_OF_TIMES(6) +	response-time	*/
	/* 0 +				request-msec	*/
	/* 0 +				response-msec	*/
	/* 0				authentication	*/
	/*-------------------				*/
	/* 23						*/
static ST_UCHAR *smp_a1_ptr;			/* reqest time ptr  	*/
static ST_UCHAR *smp_a2_ptr;			/* response time ptr	*/

static ST_UCHAR  smp_sync_err_pdu [8];
static ST_UCHAR *smp_sync_err_code_ptr;		/* sync error code ptr	*/
	 /* error code in Sync-ErrorPDU must be set before sending	*/

static ST_UCHAR smp_prepare_sync_pdu [7];


	/*----------------------------------------------*/
	/* Time Synchronization	State Machine		*/
	/* states, struct,...				*/
	/*----------------------------------------------*/

#define SMP_SYNC_STATE_IDLE			0
#define SMP_SYNC_STATE_SEND_PREPARE		1
#define SMP_SYNC_STATE_SEND_MEASURE_REQ		2
#define SMP_SYNC_STATE_WAIT_MEASURE_RSP		3
#define SMP_SYNC_STATE_SEND_SYNC_REQ		4
#define SMP_SYNC_STATE_WAIT_SYNC_RSP		5


typedef struct
  {
  ST_UINT  state;

  ST_UCHAR loc_mac  [SMP_MAX_LEN_MAC];	/* Local MAC address		*/
  ST_UCHAR rem_mac  [SMP_MAX_LEN_MAC];	/* Remote MAC address		*/

  ST_UCHAR rec_time1 [SMP_SIZE_OF_REC_TIME]; /* send time of Measure-Request  PDU	*/
  ST_UCHAR rec_time2 [SMP_SIZE_OF_REC_TIME]; /* recv time of Measure-Response PDU	*/
					
  ST_LONG  user_id;			/* for user to use		*/
  ST_VOID  (*callBackFun)(ST_LONG user_id, ST_RET result, SMP_TIME *delta_time);
  } SMP_SYNC_STATE_MACH;


/* We set up to handle multiple state machines which represents the	*/
/* ability to synchronize multiple slaves.				*/
		

#define SMP_MAX_STATE_MACH 32

static SMP_SYNC_STATE_MACH smp_sync_state_mach [SMP_MAX_STATE_MACH];


	/*----------------------------------------------*/
	/* Vars used during PDU processing 		*/
	/*----------------------------------------------*/

#if defined(REDUCED_STACK)
#define SUBNET_ADLC	1
#endif  /* !REDUCED_STACK */
static ST_INT subnet_type;
static ST_INT max_len_mac;


	/*----------------------------------------------*/
	/* Local function declarations			*/
	/*----------------------------------------------*/
	
static ST_VOID smp_set_mac_str (ST_UCHAR *mac, ST_UCHAR *mac_str);

static ST_RET smp_process_err_pdu         (ST_UCHAR err_code,    ST_UCHAR *loc_mac, ST_UCHAR *rem_mac);
static ST_RET smp_process_measure_rsp_pdu (ST_UCHAR *rec_time,   ST_UCHAR *loc_mac, ST_UCHAR *rem_mac);
static ST_RET smp_process_sync_rsp_pdu    (SMP_TIME *delta_time, ST_UCHAR *loc_mac, ST_UCHAR *rem_mac);
static ST_RET smp_process_sync_err_pdu    (ST_UCHAR err_code,    ST_UCHAR *loc_mac, ST_UCHAR *rem_mac);

static ST_RET  smp_send_err_pdu          (ST_UCHAR err_code, ST_UCHAR *loc_mac, ST_UCHAR *rem_mac);
static ST_RET  smp_send_prepare_sync_pdu (SMP_SYNC_STATE_MACH *state_mach);
static ST_RET  smp_send_measure_req_pdu  (SMP_SYNC_STATE_MACH *state_mach);
static ST_RET  smp_send_sync_req_pdu     (SMP_SYNC_STATE_MACH *state_mach);
static ST_RET  smp_send_sync_err_pdu     (ST_UCHAR err_code, ST_UCHAR *loc_mac, ST_UCHAR *rem_mac);
static ST_RET  smp_send_pdu              (ST_LONG user_id, ST_UCHAR  *pdu_buf, ST_UINT16  pdu_len, ST_UCHAR  *loc_mac, ST_UCHAR  *rem_mac);
       ST_VOID smp_send_pdu_callback     (ST_RET result, ST_LONG user_id, ST_UCHAR *rec_time, ST_UINT rec_time_len);

static SMP_SYNC_STATE_MACH *smp_find_machine   (ST_UCHAR *loc_mac, ST_UCHAR *rem_mac);
static SMP_SYNC_STATE_MACH *smp_create_machine (ST_UCHAR *loc_mac, ST_UCHAR *rem_mac);
static ST_VOID              smp_reset_machine  (SMP_SYNC_STATE_MACH *state_mach);


/************************************************************************/
/*			smp_init					*/
/*----------------------------------------------------------------------*/
/* This function is called from the user code to initialize all global	*/
/* variables used in SMP modules.					*/
/*									*/
/* Return:								*/
/*	ST_RET		SD_SUCCESS or SD_FAILURE.			*/
/************************************************************************/
ST_RET smp_init (ST_VOID)
{
  smp_debug_sel |= SMP_LOG_ERR;
  memset (smp_auth, 0, sizeof (smp_auth));	/* unused, all 0's	*/

  /* Pre-encoded PDUs to send to the Slave	*/
  /* First 3 bytes is LLC encoding.		*/

  smp_err_pdu [0] = LLC_LSAP_SMP;
  smp_err_pdu [1] = LLC_LSAP_SMP;
  smp_err_pdu [2] = LLC_CONTROL_SMP;
  smp_err_pdu [3] = 0x80;			/* ASN1 code, ErrorPDU	*/
  smp_err_pdu [4] = 0x01;			/* length		*/
  smp_err_pdu [5] = 0x00;			/* error code		*/

  smp_err_code_ptr = &smp_err_pdu [5];		/* set error code ptr	*/


  smp_measure_req_pdu [0] = LLC_LSAP_SMP;
  smp_measure_req_pdu [1] = LLC_LSAP_SMP;
  smp_measure_req_pdu [2] = LLC_CONTROL_SMP;
  smp_measure_req_pdu [3] = 0xA1;		/* ASN1 code, TimeSyncPDU	*/
  smp_measure_req_pdu [4] = 0x02;		/* length			*/
  smp_measure_req_pdu [5] = 0x80;		/* ASN1 code, Measure-RequestPDU*/
  smp_measure_req_pdu [6] = 0x00;		/* length			*/


  smp_sync_req_pdu  [0] = LLC_LSAP_SMP;
  smp_sync_req_pdu  [1] = LLC_LSAP_SMP;
  smp_sync_req_pdu  [2] = LLC_CONTROL_SMP;
  smp_sync_req_pdu  [3] = 0xA1;			/* ASN1 code, TimeSyncPDU	*/
  smp_sync_req_pdu  [4] = 0x12;			/* length			*/
  smp_sync_req_pdu  [5] = 0xA2;			/* ASN1 code, Sync-RequestPDU	*/
  smp_sync_req_pdu  [6] = 0x10;			/* length			*/
  smp_sync_req_pdu  [7] = 0x80;			/* ASN1 code, TimeOfDay	*/
  smp_sync_req_pdu  [8] = 0x06;			/* length		*/
  smp_sync_req_pdu  [9] = 0x00;			/* request time		*/
  smp_sync_req_pdu [10] = 0x00;
  smp_sync_req_pdu [11] = 0x00;
  smp_sync_req_pdu [12] = 0x00;
  smp_sync_req_pdu [13] = 0x00;
  smp_sync_req_pdu [14] = 0x00;
  smp_sync_req_pdu [15] = 0x81;			/* ASN1 code, TimeOfDay	*/
  smp_sync_req_pdu [16] = 0x06;			/* length		*/
  smp_sync_req_pdu [17] = 0x00;			/* response time	*/
  smp_sync_req_pdu [18] = 0x00;
  smp_sync_req_pdu [19] = 0x00;
  smp_sync_req_pdu [20] = 0x00;
  smp_sync_req_pdu [21] = 0x00;
  smp_sync_req_pdu [22] = 0x00;

  smp_a1_ptr = &smp_sync_req_pdu [9];		/* set req time ptr	*/
  smp_a2_ptr = &smp_sync_req_pdu [17];		/* set rsp time ptr	*/

  smp_sync_err_pdu [0] = LLC_LSAP_SMP;
  smp_sync_err_pdu [1] = LLC_LSAP_SMP;
  smp_sync_err_pdu [2] = LLC_CONTROL_SMP;
  smp_sync_err_pdu [3] = 0xA1;			/* ASN1 code, TimeSyncPDU	*/
  smp_sync_err_pdu [4] = 0x03;			/* length			*/
  smp_sync_err_pdu [5] = 0x84;			/* ASN1 code, Sync-ErrorPDU	*/
  smp_sync_err_pdu [6] = 0x01;			/* length			*/
  smp_sync_err_pdu [7] = 0x00;			/* error code			*/

  smp_sync_err_code_ptr =  &smp_sync_err_pdu [7];

  smp_prepare_sync_pdu [0] = LLC_LSAP_SMP;
  smp_prepare_sync_pdu [1] = LLC_LSAP_SMP;
  smp_prepare_sync_pdu [2] = LLC_CONTROL_SMP;
  smp_prepare_sync_pdu [3] = 0xA1;		/* ASN1 code, TimeSyncPDU	*/
  smp_prepare_sync_pdu [4] = 0x02;		/* length			*/
  smp_prepare_sync_pdu [5] = 0x85;		/* ASN1 code, Prepare-SyncPDU	*/
  smp_prepare_sync_pdu [6] = 0x00;		/* length			*/

  memset (smp_sync_state_mach, 0, sizeof(smp_sync_state_mach));

#if defined(REDUCED_STACK)
  subnet_type = SUBNET_ADLC;
  max_len_mac = 2;
#else
  subnet_type = clnp_snet_get_type ();
  switch (subnet_type)
    {
    case SUBNET_ADLC:
      max_len_mac = 2;
    break;

    case SUBNET_ETHE:
      max_len_mac = 6;
    break;

    default:
      SMP_LOG_ERR1 ("SMP-ERROR: Subnetwork type=%d not supported", subnet_type);
      return (SD_FAILURE);	/* Init failed.  CANNOT use SMP.	*/
    }
#endif  /* !REDUCED_STACK */

  return (SD_SUCCESS);
}


/************************************************************************/
/*			smp_set_mac_str					*/
/************************************************************************/
static ST_VOID smp_set_mac_str (ST_UCHAR *mac, ST_UCHAR *mac_str)
{
  mac_str[0]=0;

  if (subnet_type == SUBNET_ADLC)
    {
    sprintf (mac_str,"0x%X (%u)", *((ST_UINT16 *) mac), *((ST_UINT16 *) mac));
    }
#if !defined(REDUCED_STACK)
  else if (subnet_type == SUBNET_ETHE)
    {
    sprintf(mac_str,"%02X%02X%02X%02X%02X%02X",
	  (ST_UINT) mac[0],
	  (ST_UINT) mac[1],
	  (ST_UINT) mac[2],
	  (ST_UINT) mac[3],
	  (ST_UINT) mac[4],
	  (ST_UINT) mac[5]);
    }
#endif
}
  

/************************************************************************/
/*			smp_time_sync_req				*/
/*----------------------------------------------------------------------*/
/* Return:								*/
/*	ST_RET		SD_SUCCESS or SD_FAILURE.			*/
/************************************************************************/
ST_RET smp_time_sync_req (
		ST_LONG	  user_id,	/* for user to use		*/
		ST_UCHAR *loc_mac,	/* Ptr to Local MAC address	*/
		ST_UCHAR *rem_mac,	/* Ptr to Remote MAC address	*/
		ST_VOID	(*callBackFun)(ST_LONG user_id, ST_RET result, SMP_TIME *delta_time))
{
SMP_SYNC_STATE_MACH	*state_mach;
ST_CHAR			lmac [SMP_MAX_LEN_MAC];

  if (subnet_type == 0)		/* Check that init has been done.	*/
    {	/* NOTE: use ALWAYS log here because ERR may not be enabled.	*/
    SLOGALWAYS0 ("SMP-ERROR: SMP not initialized (must call smp_init)");
    return (SD_FAILURE);
    }

  if (loc_mac == NULL)
    {
#if defined(REDUCED_STACK)
    SMP_LOG_ERR0 ("SMP-ERROR: Local MAC address must be specified");
    return (SD_FAILURE);
#else
    /* Local MAC may be setup during initialization time */
    if (clnp_snet_get_local_mac (lmac) != SD_SUCCESS)
      {
      SMP_LOG_ERR0 ("SMP-ERROR: Lookup local MAC address failed (not initialized)");
      return (SD_FAILURE);
      }
#endif  /* !REDUCED_STACK */
    }
  else
    memcpy (lmac, loc_mac, SMP_MAX_LEN_MAC);

  /* reset existing state machine or create new one */
  state_mach = smp_create_machine (lmac, rem_mac);
  if (state_mach == NULL)
    {
    /* There are no more state machines to be used. */
    SMP_LOG_ERR0 ("SMP-ERROR: Creating state machine failed (out of resources)");
    return (SD_FAILURE);
    }
  state_mach->user_id = user_id;
  memcpy (state_mach->loc_mac, lmac, max_len_mac);
  memcpy (state_mach->rem_mac, rem_mac, max_len_mac);
  state_mach->callBackFun = callBackFun;

  /* send the unconfirmed Prepare-SyncPDU */
  if (smp_send_prepare_sync_pdu (state_mach) != SD_SUCCESS)
    {
    smp_reset_machine (state_mach);
    return (SD_FAILURE);
    }

  switch (subnet_type)
    {
    case SUBNET_ADLC:
      /* wait until the PDU is actually sent by subnetwork */
      state_mach->state = SMP_SYNC_STATE_SEND_PREPARE;
    break;

#if !defined(REDUCED_STACK)
    case SUBNET_ETHE:
      /* Prepare-SyncPDU was sent, this is an unconfirmed pdu,	*/
      /* we can now send the Measure-RequestPDU 		*/
      if (smp_send_measure_req_pdu (state_mach) != SD_SUCCESS)
        {
        smp_reset_machine (state_mach);
        return (SD_FAILURE);
        }
      /* at this point we consider the PDU sent */
      if (u_smp_record_time (state_mach->rec_time1, sizeof (state_mach->rec_time1))
        == SD_SUCCESS)
        {
        state_mach->state = SMP_SYNC_STATE_WAIT_MEASURE_RSP;	
        }
      else
        {
        SMP_LOG_ERR0 ("SMP-ERROR: Recording local time of Measure-RequestPDU failed");
        smp_reset_machine (state_mach);
        return (SD_FAILURE);
        }
    break;
#endif  /* !REDUCED_STACK */

    default:
      /* invalid subnet */
      SMP_LOG_ERR1 ("SMP-ERROR: Subnetwork type=%d not supported", subnet_type);
      smp_reset_machine (state_mach);
      return (SD_FAILURE);
    }

  return (SD_SUCCESS);  
}


/************************************************************************/
/*			smp_decode_uca_pdu				*/
/*----------------------------------------------------------------------*/
/* This function is called from the CLNP layer right after the LLC LSAP	*/
/* decode. It is used to decode a UCA Station Management Protocol PDU.	*/
/************************************************************************/
ST_RET smp_decode_uca_pdu (SMP_DATAUNIT *smp_du)
{
ST_RET   ret;
ST_UCHAR err_code;
ST_UCHAR loc_mac_str[20];
ST_UCHAR rem_mac_str[20];
ST_BOOLEAN sv_rec_time_valid;	               /* SD_TRUE if sv_rec_time valid	*/
ST_UCHAR   sv_rec_time [SMP_SIZE_OF_REC_TIME]; /* save PDU arrival time	*/
SMP_SYNC_STATE_MACH	*state_mach;

  if (subnet_type == 0)		/* Check that init has been done.	*/
    {	/* NOTE: use ALWAYS log here because ERR may not be enabled.	*/
    SLOGALWAYS0 ("SMP-ERROR: SMP not initialized (must call smp_init)");
    return (SD_FAILURE);
    }

  switch (subnet_type)
    {
    case  SUBNET_ADLC:
      /* currently ADLC is recording time on all received SMP PDUs 	*/
      memcpy (sv_rec_time, smp_du->rec_time, sizeof (sv_rec_time));
      sv_rec_time_valid = SD_TRUE;
    break;

#if !defined(REDUCED_STACK)
    case SUBNET_ETHE:
      sv_rec_time_valid = SD_TRUE;
#if defined(PHARLAP_ETS)
      /* The Pharlap Ethernet driver will record time in the clnp_tnt.c */
      memcpy (sv_rec_time, &smp_du->pdu_buf[smp_du->pdu_len], sizeof(SYSTEMTIME));
#else
      if (u_smp_record_time (sv_rec_time, sizeof (sv_rec_time)) != SD_SUCCESS)
        {
        sv_rec_time_valid = SD_FALSE;
        SMP_LOG_ERR0 ("SMP-ERROR: Recording local time failed");
        }
#endif  /* PHARLAP_ETS */
    break;
#endif  /* !REDUCED_STACK */

    default:
      /* other subnetworks not supported */
      SMP_LOG_ERR1 ("SMP-ERROR: Ignoring received UCA SMP PDU (subnetwork=%d not supported)",
        subnet_type);
      return (SD_FAILURE);
    }
    
  smp_set_mac_str (smp_du->loc_mac, loc_mac_str);
  smp_set_mac_str (smp_du->rem_mac, rem_mac_str);

  /* ! REDUCED_STACK has different loc addr for each conn! do not check	*/

#if !defined(REDUCED_STACK)
  switch (subnet_type)
    {
    case  SUBNET_ADLC:
      /* Check if local MAC is ours, ignore PDU not addressed to us	*/
      if (memcmp (smp_du->loc_mac, clnp_param.loc_mac, max_len_mac) != 0)
        {
        SMP_LOG_IND1 ("SMP: Ignoring PDU for dest mac=%s", loc_mac_str);
        return (SD_SUCCESS);
	}
    break;

    case SUBNET_ETHE:
      /* Check if local MAC is ours, ignore PDU not addressed to us	*/
      if (memcmp (smp_du->loc_mac, clnp_param.loc_mac, max_len_mac) != 0)
        {
        SMP_LOG_IND1 ("SMP: Ignoring PDU for dest mac=%s", loc_mac_str);
        return (SD_SUCCESS);
	}
    break;
    }
#endif  /* !REDUCED_STACK */

  /* decode received PDU */
  if ((ret = smp_dec_uca_pdu (smp_du->pdu_buf, smp_du->pdu_len)) != SD_SUCCESS)
    {
    SMP_LOG_IND2 ("SMP-PDU.ind   loc_mac=%s  rem_mac=%s", loc_mac_str, rem_mac_str);
    SMP_LOGH_IND (smp_du->pdu_len, smp_du->pdu_buf);

    /* PDU decoding error */
    if ((err_code = smp_err_code) == 0)
      err_code = SMP_ERR_PDU_ERROR;
    /* may loop if ASN1 decoding bad on both sides */
    /*    smp_send_err_pdu (err_code, smp_du->loc_mac, smp_du->rem_mac); */

    /* the Time Synchronization failed */
    state_mach = smp_find_machine (smp_du->loc_mac, smp_du->rem_mac);
    if (state_mach != NULL)
      {
      if (state_mach->callBackFun)
        (*state_mach->callBackFun)(state_mach->user_id, SD_FAILURE, NULL);
      smp_reset_machine (state_mach);
      }

    return (SD_FAILURE);
    }

  /* process received PDU */
  switch (smp_uca_pdu_type)
    {
    case SMP_UCA_PDU_ERROR:
      SMP_LOG_IND3  ("SMP-ErrorPDU.ind  loc_mac=%s  rem_mac=%s  error_code=%u",
        loc_mac_str, rem_mac_str, (unsigned) smp_err_code);
      SMP_LOGH_IND (smp_du->pdu_len, smp_du->pdu_buf);
      ret = smp_process_err_pdu (smp_err_code, smp_du->loc_mac, smp_du->rem_mac);
    break;

    case SMP_UCA_PDU_TIMESYNC:
      switch (smp_timesync_pdu_type)
        {
        case SMP_TIMESYNC_PDU_MEASURE_RSP:
          SMP_LOG_IND2 ("SMP-Measure-ResponsePDU.ind  loc_mac=%s  rem_mac=%s", loc_mac_str, rem_mac_str);
          SMP_LOGH_IND (smp_du->pdu_len, smp_du->pdu_buf);

          /* check if arrival time recorded */
          if (sv_rec_time_valid)
            ret = smp_process_measure_rsp_pdu (sv_rec_time, smp_du->loc_mac, smp_du->rem_mac);
          else
            {
            err_code = SMP_SYNC_ERR_NOT_MEASURED;
            ret = smp_send_sync_err_pdu (err_code, smp_du->loc_mac, smp_du->rem_mac);
            }
        break;

        case SMP_TIMESYNC_PDU_SYNC_RSP:
          SMP_LOG_IND2  ("SMP-Sync-ResponsePDU.ind     loc_mac=%s  rem_mac=%s", loc_mac_str, rem_mac_str);
          SMP_LOGC_IND6 ("  Delta (days=%ld msec=%ld usec=%ld) %c%c%c",
            smp_delta_time.days, smp_delta_time.msec, smp_delta_time.usec, ' ', ' ',' ');
          SMP_LOGH_IND (smp_du->pdu_len, smp_du->pdu_buf);
          ret = smp_process_sync_rsp_pdu (&smp_delta_time, smp_du->loc_mac, smp_du->rem_mac);
        break;

        case SMP_TIMESYNC_PDU_ERROR:
          SMP_LOG_IND3  ("SMP-Sync-ErrorPDU.ind  loc_mac=%s  rem_mac=%s  error_code=%u",
            loc_mac_str, rem_mac_str, (unsigned) smp_sync_err_code);
          SMP_LOGH_IND (smp_du->pdu_len, smp_du->pdu_buf);
          ret = smp_process_sync_err_pdu (smp_sync_err_code, smp_du->loc_mac, smp_du->rem_mac);
        break;

        default:
          SMP_LOG_IND2 ("SMP-PDU.ind   loc_mac=%s  rem_mac=%s", loc_mac_str, rem_mac_str);
          SMP_LOGH_IND (smp_du->pdu_len, smp_du->pdu_buf);
          SMP_LOG_ERR1 ("SMP-ERROR: Invalid TimeSyncPDU type=%u", (unsigned) smp_timesync_pdu_type);
          err_code = SMP_SYNC_ERR_SEQUENCE_PROBLEM;
          smp_send_sync_err_pdu (err_code, smp_du->loc_mac, smp_du->rem_mac);
          return (SD_FAILURE);
        }
    break;

    default:
      SMP_LOG_IND2 ("SMP-PDU.ind   loc_mac=%s  rem_mac=%s", loc_mac_str, rem_mac_str);
      SMP_LOGH_IND (smp_du->pdu_len, smp_du->pdu_buf);
      SMP_LOG_ERR1 ("SMP-ERROR: Invalid UCA-Mgmt PDU type=%u.", (unsigned) smp_uca_pdu_type);
      err_code = SMP_ERR_UNRECOGNIZED_SERVICE;
      smp_send_err_pdu (err_code, smp_du->loc_mac, smp_du->rem_mac);
      return (SD_FAILURE);
    }

  return (ret);
}


/************************************************************************/
/*			smp_process_err_pdu				*/
/*----------------------------------------------------------------------*/
/* ErrorPDU received.							*/
/************************************************************************/
static ST_RET smp_process_err_pdu (ST_UCHAR err_code,
                                   ST_UCHAR *loc_mac, ST_UCHAR *rem_mac)
{
SMP_SYNC_STATE_MACH	*state_mach;

  /* the Time Synchronization failed */
  state_mach = smp_find_machine (loc_mac, rem_mac);
  if (state_mach != NULL)
    {
    if (state_mach->callBackFun)
      (*state_mach->callBackFun)(state_mach->user_id, SD_FAILURE, NULL);
    smp_reset_machine (state_mach);
    }

  return (SD_SUCCESS);
}

/************************************************************************/
/*			smp_process_measure_rsp_pdu			*/
/*----------------------------------------------------------------------*/
/* TimeSyncPDU: Measure-ResponsePDU received.				*/
/************************************************************************/
static ST_RET smp_process_measure_rsp_pdu (ST_UCHAR *rec_time,
                                           ST_UCHAR *loc_mac, ST_UCHAR *rem_mac)
{
ST_UCHAR		 err_code = 0;
SMP_SYNC_STATE_MACH	*state_mach;

  state_mach = smp_find_machine (loc_mac, rem_mac);
  if (state_mach == NULL)
    {
    /* We probably got error and reset the state machine */
    SMP_LOG_ERR0 ("SMP-ERROR: Valid state machine not found, Measure-ResponsePDU not processed");
    err_code = (ST_UCHAR) SMP_SYNC_ERR_SEQUENCE_PROBLEM;
    smp_send_sync_err_pdu (err_code, loc_mac, rem_mac);
    return (SD_FAILURE);
    }

  if (state_mach->state != SMP_SYNC_STATE_WAIT_MEASURE_RSP)
    {
    /* invalid state */
    SMP_LOG_ERR1 ("SMP-ERROR: Invalid state=%d for Measure-ResponsePDU", state_mach->state);
    err_code = (ST_UCHAR) SMP_SYNC_ERR_SEQUENCE_PROBLEM;
    smp_send_sync_err_pdu (err_code, loc_mac, rem_mac);
    return (SD_FAILURE);
    }

  memcpy (state_mach->rec_time2, rec_time, sizeof (state_mach->rec_time2));

  /* send the Sync-RequestPDU */
  if (smp_send_sync_req_pdu (state_mach) != SD_SUCCESS)
    {
    /* unable to send Sync-RequestPDU, try to send Sync-ErrorPDU */
    err_code = (ST_UCHAR) SMP_SYNC_ERR_NOT_MEASURED;
    smp_send_sync_err_pdu (err_code, loc_mac, rem_mac);
    return (SD_FAILURE);
    }

  switch (subnet_type)
    {
    case  SUBNET_ADLC:
      /* we need to wait until the PDU has been actually sent sub-net	*/
      state_mach->state = SMP_SYNC_STATE_SEND_SYNC_REQ;
    break;

#if !defined(REDUCED_STACK)
    case SUBNET_ETHE:
      /* at this point we consider the PDU sent */
      state_mach->state = SMP_SYNC_STATE_WAIT_SYNC_RSP;
    break;
#endif  /* !REDUCED_STACK */

    default:
      /* other subnetworks not supported */
      SMP_LOG_ERR1 ("SMP-ERROR: subnetwork=%d not supported.", subnet_type);
      return (SD_FAILURE);
    }

  return (SD_SUCCESS);
}


/************************************************************************/
/*			smp_process_sync_rsp_pdu			*/
/*----------------------------------------------------------------------*/
/* TimeSyncPDU: Sync-ResponsePDU received.				*/
/************************************************************************/
static ST_RET smp_process_sync_rsp_pdu (SMP_TIME *delta_time,
                                        ST_UCHAR *loc_mac, ST_UCHAR *rem_mac)
{
ST_RET			 ret;
SMP_SYNC_STATE_MACH	*state_mach;
ST_RET			 result;

  state_mach = smp_find_machine (loc_mac, rem_mac);
  if (state_mach == NULL)
    {
    /* probably old PDU, do not send Sync-ErrorPDU, slave already cleaned state table	*/
    return (SD_FAILURE);
    }

  switch (state_mach->state)
    {
    case SMP_SYNC_STATE_WAIT_SYNC_RSP:
      /* the Time synchronization completed */
      result = SD_SUCCESS;
    break;

    default:
      /* invalid state */
      SMP_LOG_ERR1 ("SMP-ERROR: Invalid state=%d for Sync-ResponsePDU", state_mach->state);
      result = SD_FAILURE;
    }
  ret = result;

  if (state_mach->callBackFun)
    (*state_mach->callBackFun)(state_mach->user_id, result, delta_time);
  smp_reset_machine (state_mach);

  return (ret);
}


/************************************************************************/
/*			smp_process_sync_err_pdu			*/
/*----------------------------------------------------------------------*/
/* Sync-ErrorPDU received.						*/
/************************************************************************/
static ST_RET smp_process_sync_err_pdu (ST_UCHAR err_code,
                                        ST_UCHAR *loc_mac, ST_UCHAR *rem_mac)
{
SMP_SYNC_STATE_MACH	*state_mach;

  /* the Time Synchronization failed */
  state_mach = smp_find_machine (loc_mac, rem_mac);
  if (state_mach != NULL)
    {
    if (state_mach->callBackFun)
      (*state_mach->callBackFun)(state_mach->user_id, SD_FAILURE, NULL);
    smp_reset_machine (state_mach);
    }

  return (SD_SUCCESS);
}

	/*======================================================*/
	/* 		Send PDU functions			*/
	/*======================================================*/


/************************************************************************/
/*			smp_send_err_pdu				*/
/************************************************************************/
static ST_RET smp_send_err_pdu (ST_UCHAR err_code,
				ST_UCHAR *loc_mac, ST_UCHAR *rem_mac)
{
ST_RET			 ret;
ST_UCHAR                 loc_mac_str[20];
ST_UCHAR                 rem_mac_str[20];
SMP_SYNC_STATE_MACH	*state_mach;

  smp_set_mac_str (loc_mac, loc_mac_str);
  smp_set_mac_str (rem_mac, rem_mac_str);

  SMP_LOG_REQ3 ("SMP-ErrorPDU.req  loc_mac=%s  rem_mac=%s  error_code=%u",
    loc_mac_str, rem_mac_str, (unsigned) err_code);

  *smp_err_code_ptr = err_code;

  ret = smp_send_pdu ((ST_LONG) 0, smp_err_pdu, sizeof (smp_err_pdu), loc_mac, rem_mac);
  if (ret != SD_SUCCESS)
    {
    SMP_LOG_ERR1 ("SMP-ERROR: Send ErrorPDU.req failed (ret=%d)", ret);
    }

  /* the Time Synchronization failed */
  state_mach = smp_find_machine (loc_mac, rem_mac);
  if (state_mach != NULL)
    {
    if (state_mach->callBackFun)
      (*state_mach->callBackFun)(state_mach->user_id, SD_FAILURE, NULL);
    smp_reset_machine (state_mach);
    }

  return (ret);
}


/************************************************************************/
/*			smp_send_sync_err_pdu				*/
/************************************************************************/
static ST_RET smp_send_sync_err_pdu (ST_UCHAR err_code,
				     ST_UCHAR *loc_mac, ST_UCHAR *rem_mac)
{
ST_RET			 ret;
ST_UCHAR                 loc_mac_str[20];
ST_UCHAR                 rem_mac_str[20];
SMP_SYNC_STATE_MACH	*state_mach;

  smp_set_mac_str (loc_mac, loc_mac_str);
  smp_set_mac_str (rem_mac, rem_mac_str);

  SMP_LOG_REQ3 ("SMP-Sync-ErrorPDU.req  loc_mac=%s  rem_mac=%s  error_code=%u",
    loc_mac_str, rem_mac_str, (unsigned) err_code);

  *smp_sync_err_code_ptr = err_code;

  ret = smp_send_pdu ((ST_LONG) 0, smp_sync_err_pdu, sizeof (smp_sync_err_pdu), loc_mac, rem_mac);
  if (ret != SD_SUCCESS)
    {
    SMP_LOG_ERR1 ("SMP-ERROR: Send Sync-ErrorPDU.req failed (ret=%d)", ret);
    }

  /* the Time Synchronization failed */
  state_mach = smp_find_machine (loc_mac, rem_mac);
  if (state_mach != NULL)
    {
    if (state_mach->callBackFun)
      (*state_mach->callBackFun)(state_mach->user_id, SD_FAILURE, NULL);
    smp_reset_machine (state_mach);
    }

  return (ret);
}


/************************************************************************/
/*			smp_send_prepare_sync_pdu			*/
/************************************************************************/
static ST_RET smp_send_prepare_sync_pdu (SMP_SYNC_STATE_MACH *state_mach)
{
ST_RET   ret;
ST_UCHAR loc_mac_str[20];
ST_UCHAR rem_mac_str[20];

  smp_set_mac_str (state_mach->loc_mac, loc_mac_str);
  smp_set_mac_str (state_mach->rem_mac, rem_mac_str);

  SMP_LOG_REQ2 ("SMP-Prepare-SyncPDU.req      loc_mac=%s  rem_mac=%s",
    loc_mac_str, rem_mac_str);

  ret = smp_send_pdu ((ST_LONG) state_mach, smp_prepare_sync_pdu, sizeof (smp_prepare_sync_pdu),
		      state_mach->loc_mac, state_mach->rem_mac);
  if (ret != SD_SUCCESS)
    {
    SMP_LOG_ERR1 ("SMP-ERROR: Send Prepare-SyncPDU.req failed (ret=%d)", ret);
    }

  return (ret);
}


/************************************************************************/
/*			smp_send_measure_req_pdu			*/
/************************************************************************/
static ST_RET smp_send_measure_req_pdu (SMP_SYNC_STATE_MACH *state_mach)
{
ST_RET ret;
ST_UCHAR loc_mac_str[20];
ST_UCHAR rem_mac_str[20];

  smp_set_mac_str (state_mach->loc_mac, loc_mac_str);
  smp_set_mac_str (state_mach->rem_mac, rem_mac_str);

  SMP_LOG_REQ2 ("SMP-Measure-RequestPDU.req   loc_mac=%s  rem_mac=%s",
    loc_mac_str, rem_mac_str);

  ret = smp_send_pdu ((ST_LONG) state_mach, smp_measure_req_pdu, sizeof (smp_measure_req_pdu),
		      state_mach->loc_mac, state_mach->rem_mac);
  if (ret != SD_SUCCESS)
    {
    SMP_LOG_ERR1 ("SMP-ERROR: Send Measure-RequestPDU.req failed (ret=%d)", ret);
    }

  return (ret);
}


/************************************************************************/
/*			smp_send_sync_req_pdu				*/
/************************************************************************/
static ST_RET smp_send_sync_req_pdu (SMP_SYNC_STATE_MACH *state_mach)
{
ST_RET		ret;
ST_INT		enc_len = 0;
SMP_TIME	a1;	/* converted time of Measure-RequestPDU.req  */
SMP_TIME	a2;	/* converted time of Measure-ResponsePDU.ind */
ST_UCHAR        loc_mac_str[20];
ST_UCHAR        rem_mac_str[20];

  smp_set_mac_str (state_mach->loc_mac, loc_mac_str);
  smp_set_mac_str (state_mach->rem_mac, rem_mac_str);

  SMP_LOG_REQ2 ("SMP-Sync-RequestPDU.req      loc_mac=%s  rem_mac=%s",
    loc_mac_str, rem_mac_str);

  /* convert the recorded times to struct we want to see */
  memset (&a1, 0, sizeof (SMP_TIME));
  memset (&a2, 0, sizeof (SMP_TIME));
  if (u_smp_convert_time (state_mach->rec_time1, sizeof (state_mach->rec_time1),
                          &a1) != SD_SUCCESS
      ||
      u_smp_convert_time (state_mach->rec_time2, sizeof (state_mach->rec_time2),
                          &a2) != SD_SUCCESS)
    {
    SMP_LOG_ERR0 ("SMP-ERROR: Convert time function failed");
    return (SD_FAILURE);
    }

  SMP_LOGC_REQ6 ("  a1(days=%ld msec=%ld usec=%ld)  a2(days=%ld msec=%ld usec=%ld)",
    a1.days, a1.msec, a1.usec, a2.days, a2.msec, a2.usec);

  /* time A1 */
  smp_a1_ptr[0] =  (ST_UCHAR)((a1.msec >> 24) & 0x000000ff);
  smp_a1_ptr[1] =  (ST_UCHAR)((a1.msec >> 16) & 0x000000ff);
  smp_a1_ptr[2] =  (ST_UCHAR)((a1.msec >> 8)  & 0x000000ff);
  smp_a1_ptr[3] =  (ST_UCHAR) (a1.msec        & 0x000000ff);
  /* days A1 */
  smp_a1_ptr[4] =  (ST_UCHAR)((a1.days >> 8) & 0x000000ff);
  smp_a1_ptr[5] =  (ST_UCHAR) (a1.days       & 0x000000ff);

  /* time A2 */
  smp_a2_ptr[0] =  (ST_UCHAR)((a2.msec >> 24) & 0x000000ff);
  smp_a2_ptr[1] =  (ST_UCHAR)((a2.msec >> 16) & 0x000000ff);
  smp_a2_ptr[2] =  (ST_UCHAR)((a2.msec >> 8)  & 0x000000ff);
  smp_a2_ptr[3] =  (ST_UCHAR) (a2.msec        & 0x000000ff);
  /* days A2 */
  smp_a2_ptr[4] =  (ST_UCHAR)((a2.days >> 8) & 0x000000ff);
  smp_a2_ptr[5] =  (ST_UCHAR) (a2.days       & 0x000000ff);


  ret = smp_send_pdu ((ST_LONG) state_mach, smp_sync_req_pdu, sizeof (smp_sync_req_pdu),
		      state_mach->loc_mac, state_mach->rem_mac);
  if (ret != SD_SUCCESS)
    {
    SMP_LOG_ERR1 ("SMP-ERROR: Send Sync-RequestPDU.req failed (ret=%d)", ret);
    }

  return (ret);
}


/************************************************************************/
/*			smp_send_pdu					*/
/* Note:  The LLC is already encoded 					*/
/************************************************************************/
static ST_RET smp_send_pdu (ST_LONG user_id,
			    ST_UCHAR *pdu_buf, ST_UINT16  pdu_len,
			    ST_UCHAR *loc_mac, ST_UCHAR  *rem_mac)
{
ST_RET ret = SD_FAILURE;

#if defined(REDUCED_STACK)

ADLC_EXT_WRITE_PACKET adlcExtPacket;

  SMP_LOGH_REQ (pdu_len, pdu_buf);

  adlcExtPacket.i.localAddr = *((ST_INT16 *) loc_mac);
  adlcExtPacket.i.remoteAddr = *((ST_INT16 *) rem_mac);
  adlcExtPacket.i.dataSize = pdu_len;
  adlcExtPacket.i.data = pdu_buf;
  adlcExtPacket.i.userId = user_id;
  ret = adlcExtWritePacket (&adlcExtPacket);

#else

SN_UNITDATA sn_req;

  SMP_LOGH_REQ (pdu_len, pdu_buf);

  memset (&sn_req, 0, SN_UNITDATA_LEN);
  memcpy (sn_req.loc_mac, loc_mac, max_len_mac);
  memcpy (sn_req.rem_mac, rem_mac, max_len_mac);
  sn_req.lpdu_len = pdu_len;
  sn_req.lpdu = pdu_buf;
  ret = clnp_snet_ext_write (&sn_req, user_id);

#endif  /* !REDUCED_STACK */

  return (ret);
}


/************************************************************************/
/*			smp_send_pdu_callback				*/
/*----------------------------------------------------------------------*/
/* This callback function called by ADLC sub-network layer when a PDU	*/
/* has been sent. Saves time stamp of Measure-Response PDU only.	*/
/************************************************************************/
ST_VOID smp_send_pdu_callback (ST_RET result, ST_LONG user_id,
			       ST_UCHAR *rec_time, ST_UINT rec_time_len)
{
SMP_SYNC_STATE_MACH	*state_mach;
ST_UCHAR        	loc_mac_str[20];
ST_UCHAR        	rem_mac_str[20];

  if ((state_mach = (SMP_SYNC_STATE_MACH *) user_id) == NULL)
    return; /* just ignore, user_id=0 used when error PDU sent */

  smp_set_mac_str (state_mach->loc_mac, loc_mac_str);
  smp_set_mac_str (state_mach->rem_mac, rem_mac_str);

  if (result != SD_SUCCESS)
    {
    SMP_LOG_ERR2 ("SMP-ERROR: Send PDU failed, loc_mac=%s  rem_mac=%s",
      loc_mac_str, rem_mac_str);
    smp_reset_machine (state_mach);
    return;
    }

  switch (state_mach->state)
    {
    case SMP_SYNC_STATE_SEND_PREPARE:
      /* now send the Measure-RequestPDU and wait for the PDU send done call */
      if (smp_send_measure_req_pdu (state_mach) == SD_SUCCESS)
        state_mach->state = SMP_SYNC_STATE_SEND_MEASURE_REQ;
      else
        {
        if (state_mach->callBackFun)
          (*state_mach->callBackFun)(state_mach->user_id, SD_FAILURE, NULL);
        smp_reset_machine (state_mach);
        }
    break;

    case SMP_SYNC_STATE_SEND_MEASURE_REQ:
      /* save the recorded time and wait for Measure-ResponsePDU */
      memcpy (state_mach->rec_time1, rec_time, sizeof (state_mach->rec_time1));
      state_mach->state = SMP_SYNC_STATE_WAIT_MEASURE_RSP;
    break;

    case SMP_SYNC_STATE_SEND_SYNC_REQ:
      /* Sync-RequestPDU sent, wait for response */
      state_mach->state = SMP_SYNC_STATE_WAIT_SYNC_RSP;
    break;

    default:	/* ignore any other state */
    break;
    }
}


	/*======================================================*/
	/* 	Time Sync State Machine local functions		*/
	/*======================================================*/


/************************************************************************/
/*			smp_find_machine				*/
/*----------------------------------------------------------------------*/
/* This routine searches the available sync state machines and returns	*/
/* one for which the MAC adresses match.				*/
/*									*/
/* Parameters:								*/
/*									*/
/* Return:								*/
/*	SD_SUCCESS (0)		if encoding successful			*/
/*	NULL			otherwise				*/
/************************************************************************/
static SMP_SYNC_STATE_MACH *smp_find_machine (ST_UCHAR *loc_mac, ST_UCHAR *rem_mac)
{
ST_INT i;
SMP_SYNC_STATE_MACH *state_mach = NULL;

  for (i=0; i<SMP_MAX_STATE_MACH; ++i)
    {
    if (smp_sync_state_mach [i].state != SMP_SYNC_STATE_IDLE &&
      memcmp (loc_mac, smp_sync_state_mach[i].loc_mac, max_len_mac) == 0 &&
      memcmp (rem_mac, smp_sync_state_mach[i].rem_mac, max_len_mac) == 0)
      {
      state_mach = &smp_sync_state_mach [i];
      break;
      }
    }
  return (state_mach);
}


/************************************************************************/
/*			smp_create_machine				*/
/*----------------------------------------------------------------------*/
/* This routine finds the state of an existing machine or starts the	*/
/* use of a new state machine.						*/
/************************************************************************/
static SMP_SYNC_STATE_MACH *smp_create_machine (
				ST_UCHAR *loc_mac, ST_UCHAR *rem_mac)
{
ST_INT i;
SMP_SYNC_STATE_MACH *state_mach = NULL;

  /* Find existing state machine for given rem_mac or create new one	*/
  if ((state_mach = smp_find_machine (loc_mac, rem_mac)) == NULL)
    {
    /* Find an unused state machine */
    for (i=0; i<SMP_MAX_STATE_MACH; ++i)
      {
      if (smp_sync_state_mach [i].state == SMP_SYNC_STATE_IDLE)
        {
	state_mach = &smp_sync_state_mach [i];
        break;
	}
      }
    }

  /* set the state machine */
  if (state_mach != NULL)
    {
    smp_reset_machine (state_mach);
    state_mach->state = SMP_SYNC_STATE_SEND_PREPARE;
    }

  return (state_mach);
}


/************************************************************************/
/*			smp_reset_machine				*/
/*----------------------------------------------------------------------*/
/* Reset state machine to IDLE state.					*/
/************************************************************************/
static ST_VOID smp_reset_machine (SMP_SYNC_STATE_MACH *state_mach)
{
  if (state_mach != NULL)
    {
    memset (state_mach, 0, sizeof (SMP_SYNC_STATE_MACH));
    state_mach->state = SMP_SYNC_STATE_IDLE;
    }
}


#endif  /* UCA_SMP */

