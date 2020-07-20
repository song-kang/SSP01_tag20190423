/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	1997-2002, All Rights Reserved.					*/
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : smp_slv.c						*/
/* PRODUCT(S)  : UCA Station Management Protocol (UCA SMP).		*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This module implements the SLAVE side of the UCA Station	*/
/*	Management Protocol.						*/
/* Implementation notes:						*/
/*	If the SMP modules are incorporated within a Slave application	*/
/*	using the SISCO's Reduced Stack then this module needs to be	*/
/*	compiled with the REDUCED_STACK define.				*/
/*	If the SMP modules are incorporated within a Slave application	*/
/*	using the SISCO's Trim7 Stack then no defines are needed, this	*/
/*	is the default implementation of SMP.				*/
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
/* 11/23/11  KCR           Added casting for proper 64bit compile       */
/* 10/04/07  MDE    17     Tweaked LOGCFG_VALUE_GROUP/LOGCFGX_VALUE_MAP	*/
/* 06/13/07  RKR    16     ported to RHEL5                              */
/* 05/23/05  EJV    15     Add smpLogMaskMapCtrl for parsing logcfg.xml	*/
/* 10/21/03  JRB    14     Lint.					*/
/* 01/02/02  JRB    13     Converted to use ASN1R (re-entrant ASN1)	*/
/* 10/11/01  JRB    12     Use ASN1 tools to encode sync resp so that	*/
/*			   integer lengths are correct, include "usec".	*/
/*			   Del #ifdef UCA_SMP, let linker figure it out.*/
/* 04/27/00  MDE    11     Lint cleanup					*/
/* 01/24/00  EJV    10     PHARLAP_ETS: time is recorded in the driver.	*/
/* 09/13/99  MDE    09     Added SD_CONST modifiers			*/
/* 02/05/99  JRB    08     ALWAYS set subnet_type in smp_init.		*/
/*			   In smp_decode_uca_pdu,			*/
/*			   chk that init is done by checking subnet_type*/
/* 12/28/98  EJV    07     Reorganized logging macros.			*/
/*			   Do not use global vars in smp_set_mac_str.	*/
/*			   Merged smp_process_timesync_pdu to other fun.*/
/*			   Deleted some globals or moved them into funs.*/
/*			   Use |= to set error logging.			*/
/* 10/08/98  MDE    06     Migrated to updated SLOG interface		*/
/* 08/11/98  JRB    05     Lint cleanup.				*/
/* 02/18/98  EJV    04     Moved initialization of glb vars into func.	*/
/* 11/18/97  EJV    03     Added hex logging				*/
/* 11/14/97  EJV    02     Changed the encoding of Sync-ResponsePDU	*/
/*			   from IMPLICIT INTEGER to INTEGERS		*/
/* 09/09/97  EJV    01     Created from Herb's draft.			*/
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
#include "asn1defs.h"

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
	/* SLAVE application may disallow Time Synchronization from 	*/
	/* Master by setting the smp_time_locally_sync to SD_TRUE.	*/
	/*--------------------------------------------------------------*/

ST_BOOLEAN smp_time_locally_sync;


	/*--------------------------------------------------------------*/
	/* This is the authentication for the Time Synchronization	*/
	/* Must be set by user at startup. If not set, assum all 0's.	*/
	/*--------------------------------------------------------------*/

ST_UCHAR smp_auth_const [SMP_SIZE_OF_AUTH];	/* unused, all 0's	*/


	/*------------------------------------------------------*/
	/* Pre-encoded PDUs to respond to the Master		*/
	/* First 3 bytes is LLC encoding.			*/
	/* (no ASN1 encode required).				*/
	/*------------------------------------------------------*/

static ST_UCHAR  smp_err_pdu [6];
static ST_UCHAR *smp_err_code_ptr;		/* error code ptr	*/
	 /* error code in UCA ErrorPDU must be set before sending	*/

static ST_UCHAR smp_measure_rsp_pdu [7];

static ST_UCHAR smp_sync_rsp_pdu [25];
	/* needs ASN1 encoding, max buf size needed:	*/
	/* 3 +				LLC		*/
	/* 2 + 2					*/
	/* 2+INTEGER32(4) +		delta_days	*/
	/* 2+INTEGER32(4) +		delta_msec	*/
	/* 2+INTEGER32(4) +		delta_usec	*/
	/*-------------------				*/
	/* 25						*/

static ST_UCHAR  smp_sync_err_pdu [8];
static ST_UCHAR *smp_sync_err_code_ptr;		/* sync error code ptr	*/
	 /* error code in Sync-ErrorPDU must be set before sending	*/


	/*----------------------------------------------*/
	/* Time Synchronization	State Machine		*/
	/* states, struct,...				*/
	/*----------------------------------------------*/

#define SMP_SYNC_STATE_IDLE			0
#define SMP_SYNC_STATE_WAIT_MEASURE_REQ		1
#define SMP_SYNC_STATE_SEND_MEASURE_RSP		2
#define SMP_SYNC_STATE_WAIT_SYNC_REQ		3
#define SMP_SYNC_STATE_SEND_SYNC_RSP		4


typedef struct
  {
  ST_UINT  state;

  ST_UCHAR loc_mac [SMP_MAX_LEN_MAC];
  ST_UCHAR rem_mac [SMP_MAX_LEN_MAC];

  ST_UCHAR rec_time1 [SMP_SIZE_OF_REC_TIME];/* recv time of Measure-Request  PDU	*/
  ST_UCHAR rec_time2 [SMP_SIZE_OF_REC_TIME];/* send time of Measure-Response PDU	*/

  SMP_TIME delta_time;
  } SMP_SYNC_STATE_MACH;


/* We set up to handle multiple state machines which represents the	*/
/* ability to be synced by multiple masters.				*/
		

#define SMP_MAX_STATE_MACH 1

static SMP_SYNC_STATE_MACH smp_sync_state_mach [SMP_MAX_STATE_MACH];


	/*----------------------------------------------*/
	/* Vars used during PDU processing 		*/
	/*----------------------------------------------*/

#if defined(REDUCED_STACK)
#define SUBNET_ADLC	1
#endif  /* !REDUCED_STACK */
static ST_INT subnet_type;
static ST_UINT max_len_mac;


	/*----------------------------------------------*/
	/* Local function declarations			*/
	/*----------------------------------------------*/

static ST_VOID smp_set_mac_str (ST_UCHAR *mac, ST_UCHAR *mac_str);

static ST_RET smp_process_err_pdu         (ST_UCHAR err_code, ST_UCHAR *loc_mac, ST_UCHAR *rem_mac);
static ST_RET smp_process_prepare_pdu     (ST_UCHAR *loc_mac, ST_UCHAR *rem_mac);
static ST_RET smp_process_measure_req_pdu (ST_UCHAR *rec_time, ST_UCHAR *loc_mac, ST_UCHAR *rem_mac);
static ST_RET smp_process_sync_req_pdu    (SMP_TIME *a1, SMP_TIME *a2, ST_UCHAR *auth, ST_UCHAR *loc_mac, ST_UCHAR *rem_mac);
static ST_RET smp_process_sync_err_pdu    (ST_UCHAR err_code, ST_UCHAR *loc_mac, ST_UCHAR *rem_mac);

static ST_UINT  smp_encode_sync_rsp_pdu	(SMP_TIME *delta_time);
#ifdef UCA_SMP_TEST	/* Enable this function ONLY for TESTING.	*/
static ST_VOID test_smp_encode_sync_rsp_pdu ();
#endif

static ST_RET  smp_send_err_pdu         (ST_UCHAR err_code, ST_UCHAR *loc_mac, ST_UCHAR *rem_mac);
static ST_RET  smp_send_sync_err_pdu    (ST_UCHAR err_code, ST_UCHAR *loc_mac, ST_UCHAR *rem_mac);
static ST_RET  smp_send_measure_rsp_pdu (SMP_SYNC_STATE_MACH *state_mach);
static ST_RET  smp_send_sync_rsp_pdu    (SMP_SYNC_STATE_MACH *state_mach,
					SMP_TIME *delta_time,
					ST_UINT msg_len);
static ST_RET  smp_send_pdu             (ST_LONG user_id, ST_UCHAR  *pdu_buf, ST_UINT16  pdu_len, ST_UCHAR  *loc_mac, ST_UCHAR  *rem_mac);

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
  smp_time_locally_sync = SD_FALSE;

  /* Pre-encoded PDUs to respond to the Master	*/
  /* First 3 bytes is LLC encoding.		*/

  smp_err_pdu [0] = LLC_LSAP_SMP;
  smp_err_pdu [1] = LLC_LSAP_SMP;
  smp_err_pdu [2] = LLC_CONTROL_SMP;
  smp_err_pdu [3] = 0x80;			/* ASN1 code, ErrorPDU	*/
  smp_err_pdu [4] = 0x01;			/* length		*/
  smp_err_pdu [5] = 0x00;			/* error code		*/

  smp_err_code_ptr = &smp_err_pdu [5];		/* set error code ptr	*/

  smp_measure_rsp_pdu [0] = LLC_LSAP_SMP;
  smp_measure_rsp_pdu [1] = LLC_LSAP_SMP;
  smp_measure_rsp_pdu [2] = LLC_CONTROL_SMP;
  smp_measure_rsp_pdu [3] = 0xA1;		/* ASN1 code, TimeSyncPDU	*/
  smp_measure_rsp_pdu [4] = 0x02;		/* length			*/
  smp_measure_rsp_pdu [5] = 0x81;		/* ASN1 code, Measure-ResponsePDU */
  smp_measure_rsp_pdu [6] = 0x00;		/* length			*/

  smp_sync_rsp_pdu  [0] = LLC_LSAP_SMP;
  smp_sync_rsp_pdu  [1] = LLC_LSAP_SMP;
  smp_sync_rsp_pdu  [2] = LLC_CONTROL_SMP;
  /* The rest of this PDU encoded in "smp_encode_sync_rsp_pdu" function.*/

  smp_sync_err_pdu [0] = LLC_LSAP_SMP;
  smp_sync_err_pdu [1] = LLC_LSAP_SMP;
  smp_sync_err_pdu [2] = LLC_CONTROL_SMP;
  smp_sync_err_pdu [3] = 0xA1;			/* ASN1 code, TimeSyncPDU	*/
  smp_sync_err_pdu [4] = 0x03;			/* length			*/
  smp_sync_err_pdu [5] = 0x84;			/* ASN1 code, Sync-ErrorPDU	*/
  smp_sync_err_pdu [6] = 0x01;			/* length			*/
  smp_sync_err_pdu [7] = 0x00;			/* error code			*/

  smp_sync_err_code_ptr =  &smp_sync_err_pdu [7];

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

#ifdef UCA_SMP_TEST	/* Enable this ONLY for TESTING.	*/
  test_smp_encode_sync_rsp_pdu ();	/* test encode function	*/
#endif

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
    sprintf ((ST_CHAR *)mac_str,"0x%X (%u)", *((ST_UINT16 *) mac), *((ST_UINT16 *) mac));
    }
#if !defined(REDUCED_STACK)
  else if (subnet_type == SUBNET_ETHE)
    {
    sprintf((ST_CHAR *)mac_str,"%02X%02X%02X%02X%02X%02X",
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

  /* ! REDUCED_STACK local address not known (must get from stack or ADLC) */

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

    default:
      SMP_LOG_ERR1 ("SMP-ERROR: Ignoring received UCA SMP PDU (subnetwork=%d not supported)",
        subnet_type);
      return (SD_FAILURE);
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
    smp_send_err_pdu (err_code, smp_du->loc_mac, smp_du->rem_mac);
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
        case SMP_TIMESYNC_PDU_PREPARE:
          SMP_LOG_IND2 ("SMP-Prepare-SyncPDU.ind      loc_mac=%s  rem_mac=%s", loc_mac_str, rem_mac_str);
          SMP_LOGH_IND (smp_du->pdu_len, smp_du->pdu_buf);
          ret = smp_process_prepare_pdu (smp_du->loc_mac, smp_du->rem_mac);
        break;

        case SMP_TIMESYNC_PDU_MEASURE_REQ:
          SMP_LOG_IND2 ("SMP-Measure-RequestPDU.ind   loc_mac=%s  rem_mac=%s", loc_mac_str, rem_mac_str);
          SMP_LOGH_IND (smp_du->pdu_len, smp_du->pdu_buf);

          /* check if arrival time recorded */
          if (sv_rec_time_valid)
            ret = smp_process_measure_req_pdu (sv_rec_time, smp_du->loc_mac, smp_du->rem_mac);
          else
            {
            /* the arrival time was not recorded */
            SMP_LOG_ERR0 ("SMP-ERROR: Recording local time of Measure-RequestPDU failed");
            err_code = (ST_UCHAR) SMP_SYNC_ERR_NOT_MEASURED;
            ret = smp_send_sync_err_pdu (err_code, smp_du->loc_mac, smp_du->rem_mac);
            }
        break;

        case SMP_TIMESYNC_PDU_SYNC_REQ:
          SMP_LOG_IND2 ("SMP-Sync-RequestPDU.ind      loc_mac=%s  rem_mac=%s", loc_mac_str, rem_mac_str);
          SMP_LOGC_IND6 ("  a1(days=%ld msec=%ld usec=%ld)  a2(days=%ld msec=%ld usec=%ld)",
            smp_time_req.days, smp_time_req.msec, smp_time_req.usec,
            smp_time_rsp.days, smp_time_rsp.msec, smp_time_rsp.usec);
          ret = smp_process_sync_req_pdu (&smp_time_req, &smp_time_rsp, smp_auth, smp_du->loc_mac, smp_du->rem_mac);
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
  /* reset the state machine if found */
  smp_reset_machine (smp_find_machine (loc_mac, rem_mac));

  return (SD_SUCCESS);
}


/************************************************************************/
/*			smp_process_prepare_pdu				*/
/*----------------------------------------------------------------------*/
/* TimeSyncPDU: Prepare-SyncPDU received.				*/
/************************************************************************/
static ST_RET smp_process_prepare_pdu (ST_UCHAR *loc_mac, ST_UCHAR *rem_mac)
{
ST_UCHAR		 err_code;
SMP_SYNC_STATE_MACH	*state_mach;

  /* reset existing state machine or create new one */
  state_mach = smp_create_machine (loc_mac, rem_mac);
  if (state_mach == NULL)
    {
    /* There are no more state machines to be used. Send an ErrorPDU	*/
    SMP_LOG_ERR0 ("SMP-ERROR: Process Prepare-SyncPDU failed (out of resources)");

    err_code = (ST_UCHAR) SMP_ERR_OUT_OF_RESOURCES;
    smp_send_err_pdu (err_code, loc_mac, rem_mac);
    return (SD_FAILURE);
    }
  memcpy (state_mach->loc_mac, loc_mac, max_len_mac);
  memcpy (state_mach->rem_mac, rem_mac, max_len_mac);

  /* the state machine is now in SMP_SYNC_STATE_WAIT_MEASURE_REQ state	*/

  return (SD_SUCCESS);
}


/************************************************************************/
/*			smp_process_measure_req_pdu			*/
/*----------------------------------------------------------------------*/
/* TimeSyncPDU: Measure-RequestPDU received.				*/
/************************************************************************/
static ST_RET smp_process_measure_req_pdu (ST_UCHAR *rec_time,
                                           ST_UCHAR *loc_mac, ST_UCHAR *rem_mac)
{
ST_UCHAR		 err_code = 0;
SMP_SYNC_STATE_MACH	*state_mach;

  state_mach = smp_find_machine (loc_mac, rem_mac);
  if (state_mach == NULL)
    {
    /* We probably did not received Prepare-Sync PDU. Send Sync-ErrorPDU */
    SMP_LOG_ERR0 ("SMP-ERROR: Valid state machine not found, Measure-RequestPD not processed");
    err_code = (ST_UCHAR) SMP_SYNC_ERR_SEQUENCE_PROBLEM;
    smp_send_sync_err_pdu (err_code, loc_mac, rem_mac);
    return (SD_FAILURE);
    }

  if (state_mach->state != SMP_SYNC_STATE_WAIT_MEASURE_REQ)
    {
    /* invalid state */
    SMP_LOG_ERR1 ("SMP-ERROR: Invalid state=%d for Measure-RequestPDU", state_mach->state);
    err_code = (ST_UCHAR) SMP_SYNC_ERR_SEQUENCE_PROBLEM;
    smp_send_sync_err_pdu (err_code, loc_mac, rem_mac);
    return (SD_FAILURE);
    }

  /* save the recorded arrival time of the Measure-Request PDU 		*/
  memcpy (state_mach->rec_time1, rec_time, sizeof (state_mach->rec_time1));

  /* send the Measure-Response PDU */
  if (smp_send_measure_rsp_pdu (state_mach) != SD_SUCCESS)
    {
    /* unable to send Measure-ResponsePDU */
    /* try to send Sync-ErrorPDU */
    err_code = (ST_UCHAR) SMP_SYNC_ERR_NOT_MEASURED;
    smp_send_sync_err_pdu (err_code, loc_mac, rem_mac);
    return (SD_FAILURE);
    }

  switch (subnet_type)
    {
    case  SUBNET_ADLC:
      /* we need to wait until the message has been actually sent by	*/
      /* the subnetwork layer to record the sent time.			*/

      state_mach->state = SMP_SYNC_STATE_SEND_MEASURE_RSP;
    break;

#if !defined(REDUCED_STACK)
    case SUBNET_ETHE:
      /* at this point we consider the PDU sent */
      if (u_smp_record_time (state_mach->rec_time2, sizeof (state_mach->rec_time2))
        == SD_SUCCESS)
        {
        state_mach->state = SMP_SYNC_STATE_WAIT_SYNC_REQ;
        }
      else
        {
        SMP_LOG_ERR0 ("SMP-ERROR: Recording local time of Measure-ResponsePDU failed");
        err_code = (ST_UCHAR) SMP_SYNC_ERR_NOT_MEASURED;
        smp_send_sync_err_pdu (err_code, loc_mac, rem_mac);
        }
    break;
#endif  /* !REDUCED_STACK */

    default:
      /* other subnetworks not supported */
      SMP_LOG_ERR1 ("SMP-ERROR: subnetwork=%d not supported.", subnet_type);
      err_code = (ST_UCHAR) SMP_SYNC_ERR_NOT_MEASURED;
      smp_send_sync_err_pdu (err_code, loc_mac, rem_mac);
      return (SD_FAILURE);
    }

  return (SD_SUCCESS);
}


/************************************************************************/
/*			smp_process_sync_req_pdu			*/
/*----------------------------------------------------------------------*/
/* TimeSyncPDU, Sync-Request PDU received.				*/
/************************************************************************/
static ST_RET smp_process_sync_req_pdu (SMP_TIME *a1, SMP_TIME *a2, ST_UCHAR *auth,
                                        ST_UCHAR *loc_mac, ST_UCHAR *rem_mac)
{
ST_RET			 ret;
ST_UCHAR		 err_code;
SMP_SYNC_STATE_MACH	*state_mach;
SMP_TIME		 b1;	/* converted time of Measure-RequestPDU.ind  */
SMP_TIME		 b2;	/* converted time of Measure-ResponsePDU.rsp */
SMP_TIME		 delta_time;
ST_UINT			 msg_len;

  state_mach = smp_find_machine (loc_mac, rem_mac);
  if (state_mach == NULL)
    {
    /* probably old PDU. Send Sync-ErrorPDU	*/
    err_code = (ST_UCHAR) SMP_SYNC_ERR_SEQUENCE_PROBLEM;
    smp_send_sync_err_pdu (err_code, loc_mac, rem_mac);
    return (SD_FAILURE);
    }

  switch (state_mach->state)
    {
    case SMP_SYNC_STATE_WAIT_SYNC_REQ:
      /* ok */
    break;

    default:
      /* invalid state */
      SMP_LOG_ERR1 ("SMP-ERROR: Invalid state=%d for Sync-RequestPDU", state_mach->state);
      err_code = (ST_UCHAR) SMP_SYNC_ERR_SEQUENCE_PROBLEM;
      smp_send_sync_err_pdu (err_code, loc_mac, rem_mac);
      return (SD_FAILURE);
    }

  /* check the authentication */
  if (memcmp (auth, smp_auth_const, sizeof (smp_auth_const)) != 0)
    {
    SMP_LOG_ERR0 ("SMP-ERROR: Invalid autorization in Sync-RequestPDU");
    err_code = (ST_UCHAR) SMP_SYNC_ERR_NOT_AUTHORIZED;
    smp_send_sync_err_pdu (err_code, loc_mac, rem_mac);
    return (SD_FAILURE);
    }

  /* check if remote synchronization allowed */
  if (smp_time_locally_sync)
    {
    SMP_LOG_ERR0 ("SMP-ERROR: Sync-RequestPDU not executed (locally synchronized)");
    err_code = (ST_UCHAR) SMP_SYNC_ERR_LOCALLY_SYNCED;
    smp_send_sync_err_pdu (err_code, loc_mac, rem_mac);
    return (SD_FAILURE);
    }

  /* convert the recorded times to struct we want to see */
  memset (&b1, 0, sizeof (SMP_TIME));
  memset (&b2, 0, sizeof (SMP_TIME));
  if (u_smp_convert_time (state_mach->rec_time1, sizeof (state_mach->rec_time1),
                          &b1) != SD_SUCCESS
      ||
      u_smp_convert_time (state_mach->rec_time2, sizeof (state_mach->rec_time2),
                          &b2) != SD_SUCCESS)
    {
    SMP_LOG_ERR0 ("SMP-ERROR: Processing of Sync-RequestPDU failed (time conversion function failed)");
    err_code = (ST_UCHAR) SMP_SYNC_ERR_NOT_MEASURED;
    smp_send_sync_err_pdu (err_code, loc_mac, rem_mac);
    return (SD_FAILURE);
    }

  /* call user function to sync time */
  if ((ret = u_smp_time_sync_ind (a1, a2, &b1, &b2, &delta_time)) != SD_SUCCESS)
    {
    SMP_LOG_ERR0 ("SMP-ERROR: Execution of Time Synchronization failed (user function failed)");
    err_code = (ST_UCHAR) SMP_SYNC_ERR_NOT_MEASURED;
    smp_send_sync_err_pdu (err_code, loc_mac, rem_mac);
    return (SD_FAILURE);
    }

  /* encode and send Sync-ResponsePDU */
  if ((msg_len = smp_encode_sync_rsp_pdu (&delta_time)) == 0)
    {
    /* try to send Sync-Error PDU */
    err_code = (ST_UCHAR) SMP_SYNC_ERR_NOT_MEASURED;
    smp_send_sync_err_pdu (err_code, loc_mac, rem_mac);
    return (SD_FAILURE);
    }

  if ((ret = smp_send_sync_rsp_pdu (state_mach, &delta_time, msg_len)) != SD_SUCCESS)
    {
    /* try to send Sync-Error PDU */
    err_code = (ST_UCHAR) SMP_SYNC_ERR_NOT_MEASURED;
    smp_send_sync_err_pdu (err_code, loc_mac, rem_mac);
    return (SD_FAILURE);
    }

  switch (subnet_type)
    {
    case SUBNET_ADLC:
      /* wait until the PDU is actually sent by subnetwork */
      state_mach->state = SMP_SYNC_STATE_SEND_SYNC_RSP;	
    break;

#if !defined(REDUCED_STACK)
    case SUBNET_ETHE:
      /* Time synchronization completed, clean the state machine */
      smp_reset_machine (state_mach);
    break;
#endif  /* !REDUCED_STACK */

    default:	/* Just ignore it	*/
    break;
    }

  return (SD_SUCCESS);
}


/************************************************************************/
/*			smp_process_sync_err_pdu			*/
/*----------------------------------------------------------------------*/
/* Sync-ErrorPDU received.						*/
/************************************************************************/
static ST_RET smp_process_sync_err_pdu (ST_UCHAR err_code,
                                        ST_UCHAR *loc_mac, ST_UCHAR *rem_mac)
{
  /* clear the state machine if found */
  smp_reset_machine (smp_find_machine (loc_mac, rem_mac));

  return (SD_SUCCESS);
}

	/*======================================================*/
	/* 		Encode PDU functions			*/
	/*======================================================*/

/************************************************************************/
/*			smp_encode_sync_rsp_pdu				*/
/* Encode Sync Response into "smp_sync_rsp_pdu" buffer.						*/
/* Encode values from "delta_time" into temporary ASN1 buffer.		*/
/* Then copy ASN1 buffer to global "smp_sync_rsp_pdu" AFTER LLC header.	*/
/*									*/
/* NOTE: this function writes to global "smp_sync_rsp_pdu" buffer.	*/
/* RETURNS: len of encoded msg (0 if error)				*/
/************************************************************************/
static ST_UINT smp_encode_sync_rsp_pdu (SMP_TIME *delta_time)
  {
ST_UCHAR enc_buf [32];	/* Max size of encoded data = 22 bytes.		*/
			/* Allow 10 bytes extra for ASN1 overhead.	*/
ST_UCHAR *msg_ptr;	/* ptr to beginning of encoded PDU.		*/
ST_UINT msg_len;
ASN1_ENC_CTXT localEncCtx;	/* For readability, use "aCtx" to access this.*/
ASN1_ENC_CTXT *aCtx = &localEncCtx;

  asn1r_strt_asn1_bld (aCtx, enc_buf, sizeof (enc_buf));

  asn1r_strt_constr (aCtx);			/* start TimeSyncPDU	  	*/
  asn1r_strt_constr (aCtx);			/* start Sync-ResponsePDU	*/

  /* only encode delta microseconds if not 0.		*/
  if (delta_time->usec)
    {
    asn1r_wr_i32 (aCtx, delta_time->usec);		/* delta microseconds		*/
    asn1r_fin_prim (aCtx, INT_CODE,UNI);
    }
  asn1r_wr_i32 (aCtx, delta_time->msec);		/* delta milliseconds		*/
  asn1r_fin_prim (aCtx, INT_CODE,UNI);
  asn1r_wr_i32 (aCtx, delta_time->days);		/* delta days			*/
  asn1r_fin_prim (aCtx, INT_CODE,UNI);

  asn1r_fin_constr (aCtx, 3,CTX,DEF);		/* finish Sync-ResponsePDU	*/
  asn1r_fin_constr (aCtx, 1,CTX,DEF);		/* finish TimeSyncPDU	  	*/

  msg_ptr = aCtx->asn1r_field_ptr + 1;		/* point to beginning of PDU.	*/
  msg_len = (unsigned int) (enc_buf + sizeof (enc_buf) - msg_ptr);

  if (aCtx->asn1r_encode_overrun || msg_len > 22)
    {
    SMP_LOG_ERR0 ("SMP-ERROR: Encode SyncPDU.resp failed");
    return (0);		/* error	*/
    }

  /* Copy ASN1 encoded data AFTER LLC header (3 bytes) in smp_sync_rsp_pdu.*/
  memcpy (&smp_sync_rsp_pdu [3], msg_ptr, msg_len);

  return (msg_len+3);	/* len of ASN1 encoding + LLC header (3)	*/
  }

#ifdef UCA_SMP_TEST	/* Enable this function ONLY for TESTING.	*/
/************************************************************************/
/*		test_smp_encode_sync_rsp_pdu				*/
/* Call this function to check the encode function.			*/
/************************************************************************/
static ST_VOID test_smp_encode_sync_rsp_pdu ()
  {
SMP_TIME		 delta_time;
ST_UINT msg_len;

  delta_time.usec = 0x77889900;
  delta_time.msec = 0x55667788;
  delta_time.days = 0x11223344;
  msg_len = smp_encode_sync_rsp_pdu (&delta_time);
  SLOGALWAYS0 ("SMP Sync Resp PDU");
  SLOGALWAYSH (msg_len, smp_sync_rsp_pdu);

  delta_time.usec = 0x7788;
  delta_time.msec = 0x5566;
  delta_time.days = 0x1122;
  msg_len = smp_encode_sync_rsp_pdu (&delta_time);
  SLOGALWAYS0 ("SMP Sync Resp PDU");
  SLOGALWAYSH (msg_len, smp_sync_rsp_pdu);

  delta_time.usec = 0;		/* If 0 (default), will NOT be encoded	*/
  delta_time.msec = 0x55;
  delta_time.days = 0x11;
  msg_len = smp_encode_sync_rsp_pdu (&delta_time);
  SLOGALWAYS0 ("SMP Sync Resp PDU");
  SLOGALWAYSH (msg_len, smp_sync_rsp_pdu);
  }
#endif	/* UCA_SMP_TEST	*/

	/*======================================================*/
	/* 		Send PDU functions			*/
	/*======================================================*/


/************************************************************************/
/*			smp_send_err_pdu				*/
/************************************************************************/
static ST_RET smp_send_err_pdu (ST_UCHAR err_code,
				ST_UCHAR *loc_mac, ST_UCHAR *rem_mac)
{
ST_RET   ret;
ST_UCHAR loc_mac_str[20];
ST_UCHAR rem_mac_str[20];

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

  /* clear the state machine if found */
  smp_reset_machine (smp_find_machine (loc_mac, rem_mac));

  return (ret);
}


/************************************************************************/
/*			smp_send_sync_err_pdu				*/
/************************************************************************/
static ST_RET smp_send_sync_err_pdu (ST_UCHAR err_code,
				     ST_UCHAR *loc_mac, ST_UCHAR *rem_mac)
{
ST_RET   ret;
ST_UCHAR loc_mac_str[20];
ST_UCHAR rem_mac_str[20];

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

  /* clear the state machine if found */  
  smp_reset_machine (smp_find_machine (loc_mac, rem_mac));

  return (ret);
}


/************************************************************************/
/*			smp_send_measure_rsp_pdu			*/
/************************************************************************/
static ST_RET smp_send_measure_rsp_pdu (SMP_SYNC_STATE_MACH *state_mach)
{
ST_RET   ret;
ST_UCHAR loc_mac_str[20];
ST_UCHAR rem_mac_str[20];

  smp_set_mac_str (state_mach->loc_mac, loc_mac_str);
  smp_set_mac_str (state_mach->rem_mac, rem_mac_str);

  SMP_LOG_REQ2 ("SMP-Measure-ResponsePDU.req  loc_mac=%s  rem_mac=%s",
    loc_mac_str, rem_mac_str);

  ret = smp_send_pdu ((ST_LONG) state_mach, smp_measure_rsp_pdu, sizeof (smp_measure_rsp_pdu),
		      state_mach->loc_mac, state_mach->rem_mac);
  if (ret != SD_SUCCESS)
    {
    SMP_LOG_ERR1 ("SMP-ERROR: Send Measure-ResponsePDU.req failed (ret=%d)", ret);
    }

  return (ret);
}


/************************************************************************/
/*			smp_send_sync_rsp_pdu				*/
/************************************************************************/
static ST_RET smp_send_sync_rsp_pdu (SMP_SYNC_STATE_MACH *state_mach,
                                     SMP_TIME *delta_time, ST_UINT msg_len)
{
ST_RET   ret;
ST_UCHAR loc_mac_str[20];
ST_UCHAR rem_mac_str[20];

  smp_set_mac_str (state_mach->loc_mac, loc_mac_str);
  smp_set_mac_str (state_mach->rem_mac, rem_mac_str);

  SMP_LOG_REQ2 ("SMP-Sync-ResponsePDU.req     loc_mac=%s  rem_mac=%s",
    loc_mac_str, rem_mac_str);
  SMP_LOGC_REQ6 ("  Delta (days=%ld msec=%ld usec=%ld) %c%c%c",
    delta_time->days, delta_time->msec, delta_time->usec, ' ',' ',' ');

  ret = smp_send_pdu ((ST_LONG) state_mach, smp_sync_rsp_pdu, (ST_UINT16) msg_len,
		      state_mach->loc_mac, state_mach->rem_mac);
  if (ret != SD_SUCCESS)
    {
    SMP_LOG_ERR1 ("SMP-ERROR: Send Sync-ResponsePDU.req failed (ret=%d)", ret);
    }

  return (ret);
}


/************************************************************************/
/*			smp_send_pdu					*/
/* Note:  The LLC is already encoded					*/
/************************************************************************/
static ST_RET smp_send_pdu (ST_LONG user_id,
			    ST_UCHAR  *pdu_buf, ST_UINT16  pdu_len,
			    ST_UCHAR  *loc_mac, ST_UCHAR  *rem_mac)
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
/* has been sent. Save time stamp of Measure-Response PDU only.		*/
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
    case SMP_SYNC_STATE_SEND_MEASURE_RSP:
      /* save the recorded time of sending Measure-Response PDU */
      memcpy (state_mach->rec_time2, rec_time, sizeof (state_mach->rec_time2));
      state_mach->state = SMP_SYNC_STATE_WAIT_SYNC_REQ;
    break;

    case SMP_SYNC_STATE_SEND_SYNC_RSP:
      /* the Time synchronization completed, clean the state machine */
      smp_reset_machine (state_mach);
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
/* one for which the MAC matches.					*/
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
    state_mach->state = SMP_SYNC_STATE_WAIT_MEASURE_REQ;
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
