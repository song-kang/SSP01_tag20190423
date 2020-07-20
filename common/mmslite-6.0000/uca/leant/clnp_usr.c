/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	1996-2005, All Rights Reserved.			        	*/
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : clnp_usr.c						*/
/* PRODUCT(S)  : Lean-T Stack						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*		This module implements the interface between the	*/
/*		Connectionless-mode Network Protocol (CLNP) and the	*/
/*		CLNP-user in order to exchange Network Service data	*/
/*		units (NSDUs).						*/
/*									*/
/* Note: The CLNP is implemented as non-segmenting subset of the full	*/
/*	 protocol. 							*/
/*									*/
/*  For information see the:						*/
/*	ISO 8473 "Information processing systems - Data communication -	*/
/*	  Protocol providing the connectionless-mode  network service";	*/
/*	ISO 8348 "Information processing systems - Data communication -	*/
/*	  Network service definition. Addendum 1: Connectionless-mode	*/
/*	  transmission".						*/
/*	ISO 9542 "Information processing systems - Telecommunications	*/
/*	  and information exchange beetween systems - End system to	*/
/*	  Intermediate system routing exchange protocol for use with	*/
/*	  ISO 8473.							*/
/*									*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*			clnp_init					*/
/*			clnp_end					*/
/*			clnp_alloc					*/
/*			clnp_free					*/
/*			clnp_write					*/
/*			clnp_timer_tick					*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 06/22/10  JRB	   Del clnp_read_* (use new subnet_serve).	*/
/*			   Move clnp_param to lean_var.c so always avail*/
/*			   Remove static from clnpl_log_nsdu.		*/
/* 02/27/07  JRB    36     Chg a ..LOG_NERR to ..LOG_IND.		*/
/* 03/20/06  JRB    35     Use new clnp_snet_write_raw.			*/
/* 12/05/05  JRB    34     BUG: move clnp_snet_free call to fix memory leak*/
/* 02/08/05  JRB    33     Call new clnp_snet_read_hook_process.	*/
/*			   Fix NERR log msg.				*/
/* 			   Don't set "ret" if not used.			*/
/* 01/24/05  JRB    32     Move "*_logstr" to lean_var.c		*/
/* 12/12/03  JRB    31     clnp_read: discard packet if len is illegal.	*/
/* 10/23/03  JRB    30     Move clnp_debug_sel to slog.c, slogl.c.	*/
/* 06/09/03  JRB    29     Del unused clnp_always_logstr.		*/
/* 10/29/01  JRB    28     Make clnpl_log_snsdu non-static.		*/
/* 03/12/01  JRB    27     Use new SMEM allocation functions.		*/
/* 03/12/01  JRB    26     clnp_write: set sn_req.lpdu pointing to static*/
/*			    buffer. Not allocated, so do NOT free it.	*/
/*			    clnpl_encode_dt no longer sets sn_req.lpdu.	*/
/* 03/12/01  JRB    25     Del memset in clnp_read/write. Not needed.	*/
/* 04/27/00  MDE    24     Lint cleanup					*/
/* 04/06/00  JRB    23     Use new subnet function "clnp_snet_free"	*/
/*			   instead of "free_lpdu" flag. More flexible.	*/
/* 11/17/99  JRB    22     Added clnp_status function.			*/
/* 09/13/99  MDE    21     Added SD_CONST modifiers			*/
/* 08/05/99  JRB    20     More cleanup of multithreading.		*/
/* 08/02/99  JRB    19     Move some globals from clnp_thr.c to here.	*/
/* 08/02/99  JRB    18     Support "multithreading" in "clnp_write"	*/
/*			   if S_MT_SUPPORT defined.			*/
/* 10/20/98  JRB    17     If rem_nsap not routable, or in same routing	*/
/*			   domain, send to All-ES MAC.			*/ 
/*			   Chg LOG_ALWAYS macros to LOG_REQ.		*/
/* 10/08/98  MDE    16     Migrated to updated SLOG interface		*/
/* 08/13/98  JRB    15     Lint cleanup.				*/
/* 07/17/97  EJV    14     Disable sending ESH if esh_cfg_timer=0	*/
/* 05/28/97  EJV    13     Deleted absolete clnp_err.h include.		*/
/* 05/27/97  JRB   7.00    MMSEASE 7.0 release.				*/
/* 05/07/97  JRB    12     Changed non-ANSI "%hu" to "%u".		*/
/* 03/03/97  EJV    11     Removed CLNP_LOG_ALWAYS			*/
/* 01/23/97  JRB    10     Init clnp_debug_sel in clnp_init.		*/
/* 11/13/96  JRB    09     Don't init globals unless they are constant.	*/
/* 11/13/96  JRB    08     Add "sysincs.h".				*/
/* 10/17/96  EJV    07	   changed condition to != SNET_ADLC to call	*/
/*			   function clnp_snet_get_local_mac.		*/
/* 10/03/96  JRB    06     Clarify Remote MAC logging.			*/
/* 09/25/96  JRB    05     Do not init slog. Let user do it.		*/
/* 07/25/96  EJV    04     Commented out call to clnp_config.		*/
/* 07/22/96  EJV    03     Added check for invalid MAC (for ADLC)	*/
/* 07/19/96  EJV    02     Added ns_userdata_len to clnp_init and	*/
/*			   clnpl_check_param and removed it from	*/
/*			   clnp_param.					*/
/* 05/24/96  EJV    01     Created					*/
/************************************************************************/
#include "glbtypes.h"
#include "sysincs.h"

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;  /* Define for SLOG mechanism    */
#endif

#include "mem_chk.h"
#include "glbsem.h"
#include "clnp_usr.h"
#include "clnp_log.h"
#include "clnp_sta.h"
#include "clnp_llc.h"
#include "clnp_sne.h"
#include "clnp.h"

#if defined(UCA_SMP)
#include "smp_usr.h"
#endif

/* CLNP is using clnp_debug_sel global variable to turn on/off different*/
/* types of logging.							*/
				/* if 0 nothing will be logged		*/

/*----------------------------------------------------------------------*/
/* Static variables and functions in this module			*/
/*----------------------------------------------------------------------*/

static ST_UCHAR	clnpl_initialized;


static ST_RET	clnpl_check_param (ST_UINT ns_userdata_len);



/************************************************************************/
/*			clnp_init					*/
/*----------------------------------------------------------------------*/
/* This function will initialize the operation of CLNP and the interface*/
/* to underlying sub-network.						*/
/* The CLNP-user should set the parameters in the clnp_param structure	*/
/* before calling this function. THis can be done by reading Lean-T	*/
/* Stack configuration file or by calling the clnp_config function (hard*/
/* coded).								*/
/*									*/
/* Parameters:								*/
/*	ST_UINT	ns_userdata_len		Max size of NS-USERDATA		*/
/*									*/
/* Return:								*/
/*	SD_SUCCESS (0)		if initialization successful		*/
/*	error code		otherwise				*/
/*				Note: The CLNP may recover from most	*/
/*				errors in clnp_param, but it will return*/
/*				error code for the last parsed parameter*/
/*				in error.				*/
/************************************************************************/
ST_RET clnp_init (ST_UINT ns_userdata_len)
{
ST_RET ret, ret_param;

  ret = ret_param = SD_SUCCESS;

#if defined(DEBUG_SISCO)
  clnp_debug_sel |= CLNP_LOG_ERR;
#endif

  if (!clnpl_initialized)
    {
    /*--------------------------------------------------*/
    /* NOTE:						*/
    /* Set the CLNP parameters (hard coded in clnp_hc.c)*/
    /* in case when the parameters can't be accessed	*/
    /* from a configuration file.			*/
    /* This func can be called from another module.	*/
    /*							*/
    /* ret = clnp_config ();				*/
    /*--------------------------------------------------*/


    /*--------------------------------------------------*/
    /* check the parameters in clnp_param, if any are	*/
    /* wrong reset to default values			*/
    /*--------------------------------------------------*/

    ret_param = clnpl_check_param (ns_userdata_len);

    if (ret_param == CLNP_ERR_NSAP_LEN ||	/* Invalid or missing NSAP */
        ret_param == CLNP_ERR_MAC_ADDR)		/* Missing MAC for ADLC	*/
      {
      ret = ret_param;		/* Can not recover from this errors.	*/
      }
      /* Note: We recovering from other parameters	*/
      /* errors by supplying default values.		*/

    /*--------------------------------------------------*/
    /* initialize interface with underlying sub-network */
    /* and vars for managing ES Hello.			*/
    /*--------------------------------------------------*/

    if (ret == SD_SUCCESS)
      {
      if ((ret = clnp_snet_init (&clnp_param)) == SD_SUCCESS)
        {
        /* Read local MAC addresses from sub-net interface (for ADLC it	*/
	/* is set by NS_USER)						*/

        if (clnp_snet_get_type () != SUBNET_ADLC)
          if ((ret = clnp_snet_get_local_mac (clnp_param.loc_mac)) != SD_SUCCESS)
            CLNP_LOG_ERR0 ("CLNP-ERROR: Read Local MAC address from sub-network interface failed");

	/* initialize vars needed for sending ES Hello */

        ret = clnpl_init_esh (clnp_param.esh_delay);

        if (ret != SD_SUCCESS)
          clnp_snet_term ();
	}
      }

    /*--------------------------------------------------*/
    /* Set the CLNP initialized flag and		*/
    /* call the timer function to send first ES Hello	*/
    /*--------------------------------------------------*/

    if (ret == SD_SUCCESS)
      {
      clnpl_initialized = SD_TRUE;
      clnp_timer_tick ();
      CLNP_LOG_REQ0 ("CLNP initialized");
      }
    else
      CLNP_LOG_ERR1 ("CLNP-ERROR: initialization failed ret=%d", ret);
    }

  /* If we initialized OK but there was recoverable parameter error	*/
  /* then pass the parameter error code to NS-USER.			*/
  /* If the ret_param = CLNP_ERR_UDATA_LEN the Transport should		*/
  /* terminate or lower it's packet size, otherwise some packets will	*/
  /* not be sent. This error is relevant only because CLNP does not	*/
  /* implement segmenting.						*/
  /* If we could not initialize from other reasons pass the real error	*/
  /* to NS-USER.							*/

  if (ret == SD_SUCCESS)
    ret = ret_param;

  return (ret);
}


/************************************************************************/
/*			clnp_end					*/
/*----------------------------------------------------------------------*/
/* This function will terminate the operation of the CLNP and cleanup	*/
/* the interface to underlying subnetwork.				*/
/*									*/
/* Parameters:								*/
/*	ST_VOID			none					*/
/*									*/
/* Return:								*/
/*	SD_SUCCESS (0)		if termination successful		*/
/*	error code		otherwise				*/
/************************************************************************/
ST_RET clnp_end (ST_VOID)
{
ST_RET ret;

  ret = SD_SUCCESS;

  if (clnpl_initialized)
    {
    /* terminate interface with underlying sub-network */
    ret = clnp_snet_term ();

    CLNP_LOG_REQ0 ("CLNP terminated");

    clnpl_initialized = SD_FALSE;
    }

  return (ret);
}


/************************************************************************/
/*			clnp_alloc					*/
/*----------------------------------------------------------------------*/
/* This function will allocate a buffer for N_UNITDATA to pass data	*/
/* between the CLNP and the CLNP-user.					*/
/*									*/
/* Parameters:								*/
/*	ST_UINT	data_len	Length of data in bytes. The length	*/
/*				of CLNP-user data has to be:		*/
/*				0 < data_len <= CLNP_MAX_LEN_UDATA	*/
/*									*/
/* Return:								*/
/*	N_UNITDATA *		pointer to allocated memory		*/
/*	NULL			if function fails to alloc memory	*/
/************************************************************************/
N_UNITDATA *clnp_alloc (ST_UINT data_len)
{
N_UNITDATA	*req;

  req = NULL;	/* assume invalid size requested */

  /* if requested data len falls outside correct range return NULL ptr	*/ 
  if (data_len > 0)
    {
    req = (N_UNITDATA *) M_CALLOC (MSMEM_N_UNITDATA, 1, 
    	(ST_UINT) (N_UNITDATA_LEN - CLNP_MIN_LEN_UDATA + data_len));
    }
  else
    CLNP_LOG_ERR1 ("CLNP-ERROR: alloc failed (invalid data_len=%u)", data_len);

  return (req);
}


/************************************************************************/
/*			clnp_free					*/
/*----------------------------------------------------------------------*/
/* This function will free the buffer allocated for N_UNITDATA passed	*/
/* between the CLNP and the CLNP-user.					*/
/*									*/
/* Parameters:								*/
/*	N_UNITDATA *req		memory pointer to free			*/
/*									*/
/* Return:								*/
/*	ST_VOID								*/
/************************************************************************/
ST_VOID clnp_free (N_UNITDATA *req)
{
  M_FREE (MSMEM_N_UNITDATA, req);
}


/************************************************************************/
/*			clnp_write					*/
/*----------------------------------------------------------------------*/
/* This function is used by the CLNP-user to write N_UNITDATA.request	*/
/* to the CLNP.								*/
/* The CLNP will free the req pointer.					*/
/*									*/
/* Parameters:								*/
/*	N_UNITDATA *req		ptr to N_UNITDATA.request to send.	*/
/*				This pointer should be allocated using	*/
/*				the clnp_alloc function.		*/
/*									*/
/* Return:								*/
/*	SD_SUCCESS (0)		if N_UNITDATA.request sent successfully	*/
/*	error code		otherwise				*/
/* NOTE: this function uses a "static" buffer for encoding, so it must	*/
/*       be protected by a semaphore if it is called from multiple threads.*/
/************************************************************************/
ST_RET clnp_write (N_UNITDATA *req)
{
ST_RET		ret;
SN_UNITDATA	sn_req;
static ST_UCHAR raw_packet [ETHE_MAX_LEN_LSDU];	/* raw packet buffer	*/

#if defined (S_MT_SUPPORT)
  gs_get_sem ();
#endif

  ret = SD_SUCCESS;

  /* Set "sn_req.lpdu" pointing to "data part" of raw packet buffer.	*/
  sn_req.lpdu = raw_packet + 14;

  if (clnpl_initialized)
    {
    /* make sure the pointer is not NULL */
    if (req != NULL)
      {
      /* log N_UNITDATA.request to be send */
      clnpl_log_nsdu (req, CLNP_LOG_REQ);

      /* set the local MAC address in sn_req */
      memcpy (sn_req.loc_mac, clnp_param.loc_mac, CLNP_MAX_LEN_MAC);

      /* set the remote MAC address in sn_req */
      if (req->rem_mac_valid == SD_TRUE)
        /* valid MAC addr supplied in req */
        memcpy (sn_req.rem_mac, req->rem_mac, CLNP_MAX_LEN_MAC);
      else
        {
	/* see if we can find the MAC addr in NSAP-MAC table */
        if (clnp_snet_lookup_es (req->rem_nsap, sn_req.rem_mac) != SD_SUCCESS)
          {
          /* MAC address not found in table, check if we know IS MAC addr */
          /* If no router, OR NSAP is NOT routable (i.e. < 12 bytes),	*/
          /* OR NSAP is in same Routing domain, send to All-ES MAC.	*/
          if (clnp_snet_lookup_is (sn_req.rem_mac) != SD_SUCCESS  ||
              req->rem_nsap [0] < 12  ||  clnp_param.loc_nsap [0] < 12  ||
              memcmp (&req->rem_nsap [1], &clnp_param.loc_nsap [1], 10) == 0)
            {			/* Must send to All ES MAC address	*/
            ret = clnp_snet_get_all_es_mac (sn_req.rem_mac);
            }
          /* else send to router (rem_mac already set by "..lookup_is" call).*/
          }
        }

      /* encode the PDU (LLC and NPDU encoding) */
      if ((ret = clnpl_encode_dt (&sn_req, req)) == SD_SUCCESS)
        {
        /* log the data send to sub-network */
        clnpl_log_snsdu (&sn_req, CLSNS_LOG_REQ);

        /* encoding OK, send the PDU */
        assert (sn_req.lpdu_len + 14 <= sizeof (raw_packet));
        memcpy (raw_packet, sn_req.rem_mac, CLNP_MAX_LEN_MAC);	/* DST MAC*/
        memcpy (&raw_packet[6], sn_req.loc_mac, CLNP_MAX_LEN_MAC);/* SRC MAC*/
        /* Set the 802.3 length/type field (bytes 12 and 13).	*/
        raw_packet[12] = (ST_UCHAR) (sn_req.lpdu_len >> 8);	
        raw_packet[13] = (ST_UCHAR) (sn_req.lpdu_len & 0xff);
        /* Data part already encoded in raw_packet (see sn_req.lpdu above).*/
        if ((ret = clnp_snet_write_raw (raw_packet, sn_req.lpdu_len+14)) == SD_SUCCESS)
          { 
          CLNP_STAT_INC_DT_SENT;
          CLNP_STAT_INC_PDU_SENT;
	  }
	else
          CLNP_LOG_ERR0 ("CLNP-ERROR: write to sub-network failed");
	}
      }
    else
      {
      ret = CLNP_ERR_NULL_PTR;
      CLNP_LOG_ERR0 ("CLNP-ERROR: write failed (req=NULL)");
      }
    }
  else
    {
    ret = CLNP_ERR_NOT_INIT;
    CLNP_LOG_ERR0 ("CLNP-ERROR: write failed (CLNP not initialized)");
    }

  /* we are done with this req, free the pointer */
  if (req != NULL)
    clnp_free (req);

#if defined (S_MT_SUPPORT)
  gs_free_sem ();
#endif

  return (ret);
}


/************************************************************************/
/*			clnp_timer_tick					*/
/*----------------------------------------------------------------------*/
/* This function is called on 1 second intervals by the CLNP-user (TP4)	*/
/* When this function is called, CLNP decrements the Configuration	*/
/* Timer (used to trigger sending of ES-Hellos), and the Holding Timers	*/
/* (used to trigger clearing the IS or ES {NSAP,MAC} table entries).	*/
/*									*/
/* Parameters:								*/
/*	ST_VOID			none				       	*/
/*									*/
/* Return:								*/
/*	ST_VOID			none					*/
/************************************************************************/
ST_VOID clnp_timer_tick (ST_VOID)
{
  if (clnpl_initialized)
    {
    /* See if ESH should be sent */

    clnpl_check_cfg_timer ();

    /* Let Sub-network interface decrement the Holding Times in IS and	*/
    /* ES tables.							*/

    clnp_snet_timer_tick ();
    }
}

/************************************************************************/
/*			clnp_status					*/
/*----------------------------------------------------------------------*/
/* This function checks to see if CLNP is running.			*/
/*									*/
/* Parameters:								*/
/*	ST_VOID		none					       	*/
/*									*/
/* Return:								*/
/*	ST_RET		SD_SUCCESS if running, else error code.		*/
/************************************************************************/
ST_RET clnp_status (ST_VOID)
  {
  if (!clnpl_initialized)
    return (SD_FAILURE);
  return (SD_SUCCESS);
  }

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/*			Local functions in this module			*/
/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/


/************************************************************************/
/*			clnpl_check_param				*/
/*----------------------------------------------------------------------*/
/* This function will check the parameters set by the CLNP-user.	*/
/* If any parameter is incorrect it will be reset to default value.	*/
/*									*/
/* Parameters:								*/
/*	ST_UINT	ns_userdata_len		Max size of NS-USERDATA		*/
/*									*/
/* Return:								*/
/*	SD_SUCCESS (0)		if parameters correct			*/
/*	error code		otherwise				*/
/************************************************************************/
static ST_RET clnpl_check_param (ST_UINT ns_userdata_len)
{
ST_RET ret;
ST_UINT16  sn_udata_len;
ST_UCHAR   void_mac [CLNP_MAX_LEN_MAC];


  ret = SD_SUCCESS;

  /* the PDU which we send should have non-zero lifetime */

  if (clnp_param.pdu_lifetime == 0)
    {
    clnp_param.pdu_lifetime = CLNP_DEF_PDU_LIFETIME;
    CLNP_LOG_NERR2 ("CLNP-NERR: Invalid PDU lifetime %u, resetting to %u",
      (ST_UINT) clnp_param.pdu_lifetime, (ST_UINT) CLNP_DEF_PDU_LIFETIME);
    ret = CLNP_ERR_LIFETIME;
    }

  /* the decrement units for received PDU lifetime should be at least 1	*/

  if (clnp_param.pdu_lifetime_dec == 0)
    {
    clnp_param.pdu_lifetime_dec = CLNP_DEF_PDU_LIFETIME_DEC;
    CLNP_LOG_NERR2 ("CLNP-NERR: Invalid PDU lifetime decrement %u, resetting to %u",
      (ST_UINT) clnp_param.pdu_lifetime_dec, (ST_INT) CLNP_DEF_PDU_LIFETIME_DEC);
    ret = CLNP_ERR_LIFETIME_DEC;
    }

  /* ES Hello Configuration Timer (how often system reports it's	*/
  /* presence to other systems on the network (in seconds)) should be	*/
  /* less than MAX defined						*/
  /* Note that we added additional option: if the esh_cfg_timer=0 then	*/
  /* sending of ESHs will be disabled.					*/

  if (clnp_param.esh_cfg_timer > CLNP_MAX_ESH_CFG_TIMER)
    {
    clnp_param.esh_cfg_timer = CLNP_DEF_ESH_CFG_TIMER;
    CLNP_LOG_NERR2 ("CLNP-NERR: Invalid ESH Configuration Timer %u, resetting to %u",
      clnp_param.esh_cfg_timer, CLNP_DEF_ESH_CFG_TIMER);
    ret = CLNP_ERR_ESH_CFG_TIMER;
    }

  /* check the delay time for first ES Hello, can't be larger then max	*/
  /* Configuration Timer defined					*/

  if (clnp_param.esh_delay > CLNP_MAX_ESH_CFG_TIMER)
    {
    clnp_param.esh_delay = CLNP_DEF_ESH_DELAY;
    CLNP_LOG_NERR2 ("CLNP-NERR: Invalid first ESH delay %u, resetting to %u",
      clnp_param.esh_delay, CLNP_DEF_ESH_DELAY);
    ret = CLNP_ERR_ESH_DELAY;
    }

  /* the MAC address has to be set for ADLC sub-net */
  if (clnp_snet_get_type () == SUBNET_ADLC)
    {
    memset (void_mac, 0, sizeof (void_mac)); /* assume all 0's -> invalid MAC */
    if (memcmp(clnp_param.loc_mac, void_mac, CLNP_MAX_LEN_MAC) == 0)
      ret = CLNP_ERR_MAC_ADDR;
    }

  /* if the length in the local NSAP is 0 or more then MAX defined	*/
  /* then the address is invalid					*/
  if (clnp_param.loc_nsap[0] == 0)
    {
    ret = CLNP_ERR_NSAP_LEN;
    CLNP_LOG_ERR0 ("CLNP-ERROR: Local NSAP missing in configuration (file)");
    }
  else if (clnp_param.loc_nsap[0] > CLNP_MAX_LEN_NSAP)
    {
    ret = CLNP_ERR_NSAP_LEN;
    CLNP_LOG_ERR1 ("CLNP-ERROR: Invalid local NSAP length %u", (ST_UINT) clnp_param.loc_nsap[0]);
    }

  /* the PDU size can not be greater then max which underlying sub-net	*/
  /* can handle.							*/

  sn_udata_len = clnp_snet_get_max_udata_len ();

  if (ns_userdata_len == 0 ||
      ns_userdata_len + CLNP_DEF_LEN_HDR + LLC_HDR_LEN > sn_udata_len)
    {
    CLNP_LOG_NERR2 ("CLNP-NERR: Invalid SN-USERDATA Length (%u), max allowed %u",
      ns_userdata_len, sn_udata_len-CLNP_DEF_LEN_HDR-LLC_HDR_LEN);
    ret = CLNP_ERR_UDATA_LEN;
    }

  return (ret);
}				
				
/************************************************************************/
/*			clnpl_log_nsdu					*/
/*----------------------------------------------------------------------*/
/* This function will log the Network Services data unit send/received	*/
/* to/from Transport layer.						*/
/*									*/
/* Parameters:								*/
/*	N_UNITDATA  *req	Pointer to data structure		*/
/*	ST_UINT	     log_mask	Log mask specifying if request or	*/
/*				indication should be logged.		*/
/*									*/
/* Return:								*/
/*	ST_VOID			none					*/
/************************************************************************/
ST_VOID clnpl_log_nsdu (N_UNITDATA *req, ST_ULONG log_mask)
{
#if defined(DEBUG_SISCO)
  if (log_mask == CLNP_LOG_IND)
    {
    CLNP_LOG_IND0  ("N-UNITDATA.ind:");
    }
  else if (log_mask == CLNP_LOG_REQ)
    {
    CLNP_LOG_REQ0  ("N-UNITDATA.req:");
    }
  else
    return;

  if (req->rem_mac_valid)
    {
    CLNP_LOGC_RI0 (log_mask,"Remote MAC:");
    CLNP_LOGH_RI (log_mask,(ST_UINT) CLNP_MAX_LEN_MAC, req->rem_mac);
    }
  else
    CLNP_LOGC_RI0 (log_mask,"Remote MAC UNKNOWN");
  CLNP_LOGC_RI0 (log_mask,"Remote NSAP:");
  CLNP_LOGH_RI (log_mask,(ST_UINT) req->rem_nsap [0], &req->rem_nsap [1]);
  CLNP_LOGC_RI1 (log_mask,"Data length: %u", req->data_len);
  CLNP_LOGH_RI (log_mask,(ST_UINT) req->data_len, req->data_buf);
#endif	/* defined(DEBUG_SISCO)	*/
}

#if defined(DEBUG_SISCO)
/************************************************************************/
/*			clnpl_log_snsdu					*/
/*----------------------------------------------------------------------*/
/* This function will log the Sub-Network Services data unit		*/
/* send/received to/from subnetwork.					*/
/*									*/
/* Parameters:								*/
/*	SN_UNITDATA *sn_req	Pointer to data structure		*/
/*	ST_UINT	     log_mask	Log mask specifying if request or	*/
/*				indication should be logged.		*/
/*									*/
/* Return:								*/
/*	ST_VOID			none					*/
/************************************************************************/
ST_VOID clnpl_log_snsdu (SN_UNITDATA *sn_req, ST_ULONG log_mask)
{
  if (log_mask == CLSNS_LOG_IND)
    {
    CLSNS_LOG_IND0  ("SN-UNITDATA.ind:");
    }
  else if (log_mask == CLSNS_LOG_REQ)
    {
    CLSNS_LOG_REQ0  ("SN-UNITDATA.req:");
    }
  else
    return;

  CLSNS_LOGC_RI0 (log_mask,"Local  MAC:");
  CLSNS_LOGH_RI (log_mask,(ST_UINT) CLNP_MAX_LEN_MAC, sn_req->loc_mac);
  CLSNS_LOGC_RI0 (log_mask,"Remote MAC:");
  CLSNS_LOGH_RI (log_mask,(ST_UINT) CLNP_MAX_LEN_MAC, sn_req->rem_mac);
  CLSNS_LOGC_RI1 (log_mask,"Data length: %u", sn_req->lpdu_len);
  CLSNS_LOGH_RI (log_mask,(ST_UINT) sn_req->lpdu_len, sn_req->lpdu);
}
#else	/* function if DEBUG_SISCO not defined */
ST_VOID clnpl_log_snsdu (SN_UNITDATA *sn_req, ST_ULONG log_mask)
{
  return;
}
#endif

