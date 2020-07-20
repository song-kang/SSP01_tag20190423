/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	2010-2010 All Rights Reserved					*/
/*									*/
/* MODULE NAME : subnet.c						*/
/* PRODUCT(S)  : MMSEASE-LITE						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*	Subnetwork service functions for receiving and processing	*/
/*	subnetwork packets (basically IEEE 802.3 MAC frames).		*/
/*									*/
/* NOTE: Compile this with the main application (not in library).	*/
/*	Enable the desired protocols by defining any of the following	*/
/*	in the makefile:						*/
/*			GOOSE_RX_SUPP					*/
/*			GSSE_RX_SUPP					*/
/*			GSE_MGMT_RX_SUPP				*/
/*			SMPVAL_RX_SUPP					*/
/*			SUBNET_THREADS					*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*			subnet_serve					*/
/*	These user functions should be customized as needed:		*/
/*			u_goose_ind					*/
/*			u_mmsl_goose_received (for IEC GSSE, UCA GOOSE)	*/
/*			u_gse_mgmt_ind					*/
/*			u_smpval_ind					*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who	   Comments					*/
/* --------  ---  ------   -------------------------------------------  */
/* 01/11/12  JRB           smpval_msg_decode: added "edition" arg.	*/
/*			   Add call to smpval_msg_log.			*/
/* 06/22/10  JRB	   Created.					*/
/************************************************************************/
#include "glbtypes.h"
#include "sysincs.h"
#include "mem_chk.h"
#include "subnet.h"

#include "clnp_sne.h"
#include "clnp_llc.h"	/* for LLC_LSAP	*/
#include "clnp_log.h"

#include "tp4_encd.h"	/* for TP_PDU_TYPE_UD	*/
#include "acse2usr.h"	/* for cltp_decode_nsdu	*/
#include "clnp.h"	/* for clnpl_decode	*/
#include "gse_mgmt.h"
#include "smpval.h"
#include "goose.h"	/* definitions for IEC 61850 GSSE (UCA GOOSE)	*/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;  /* Define for SLOG mechanism    */
#endif


#if defined(SUBNET_THREADS)
/* Global queues of received messages used only if threads enabled.	*/
SN_UNITDATA *smpval_ind_list;	/* queue of received SMPVAL messages	*/
SN_UNITDATA *goose_ind_list;	/* queue of received GOOSE messages	*/
SN_UNITDATA *gse_mgmt_ind_list;	/* queue of received GSE MGMT messages	*/
N_UNITDATA *cltp_ind_list;	/* queue of received CLTP messages	*/
#endif
  
#if defined(GOOSE_RX_SUPP)
/************************************************************************/
/*			u_goose_ind	 				*/
/* User callback function called when an IEC 61850 GOOSE		*/
/* message is received.							*/
/* CUSTOMIZE: This is sample only. Should be customized as needed.	*/
/************************************************************************/

ST_VOID u_goose_ind (SN_UNITDATA *sn_udt)
  {
  printf ("\n Received IEC 61850 GOOSE Message.");
#if 0
  /* NOTE: We could do this. If so, we must include "iec_demo.h" and	*/
  /*       link in code from the "iecgoose" directory.			*/
  retcode = chk_for_goose_msg (sn_udt);
#endif
  }
#endif	/* defined(GOOSE_RX_SUPP)	*/

#if defined(GSSE_RX_SUPP)
/************************************************************************/
/*			u_mmsl_goose_received 				*/
/* User callback function called when an IEC 61850 GSSE (UCA GOOSE)	*/
/* message is received and decoded.					*/
/* CUSTOMIZE: This is sample only. Should be customized as needed.	*/
/************************************************************************/

ST_VOID u_mmsl_goose_received (GOOSE_INFO *goose_info)
  {
  printf ("\n Received IEC 61850 GSSE Message.");
  }
#endif	/* defined(GSSE_RX_SUPP)	*/

#if defined(GSE_MGMT_RX_SUPP)
/************************************************************************/
/*			u_gse_mgmt_ind					*/
/* User function to perform additional processing of GSE Management	*/
/* indication.								*/
/* CUSTOMIZE: This is sample only. Should be customized as needed.	*/
/*            This function just logs the decoded message.		*/
/************************************************************************/
ST_VOID u_gse_mgmt_ind (GSE_MGMT_MSG *msg,
	ETYPE_INFO *etypeInfo,	/* Ethertype header info		*/
	SN_UNITDATA *sn_udt)	/* includes MAC addresses		*/
  {
ST_INT j;
  printf ("\n Received GSE Management Message. Type = %d", msg->msgType);

  SLOGALWAYS0 ("GSE Management Message Decode ... ");
 
  switch (msg->msgType)
    {
    case GSE_MSG_TYPE_GLOBAL_ERROR_RSP:
      SLOGCALWAYS1 ("Message Type = %d, GSE_MSG_TYPE_GLOBAL_ERROR_RSP", msg->msgType);
      SLOGCALWAYS1 ("State ID: %d", msg->msg.glbErrRsp.stateID);
      SLOGCALWAYS1 ("Ident   : %s", msg->msg.glbErrRsp.ident);
      SLOGCALWAYS1 ("ConfRev : %d", msg->msg.glbErrRsp.confRev);
      SLOGCALWAYS1 ("GLOBAL ERROR: %d", msg->msg.glbErrRsp.glbError);
      break;

    case GSE_MSG_TYPE_GO_REF_REQ:
    case GSE_MSG_TYPE_GS_REF_REQ:
      if (msg->msgType == GSE_MSG_TYPE_GO_REF_REQ)
        SLOGCALWAYS1 ("Message Type = %d, GSE_MSG_TYPE_GO_REF_REQ", msg->msgType);
      else
        SLOGCALWAYS1 ("Message Type = %d, GSE_MSG_TYPE_GS_REF_REQ", msg->msgType);
      SLOGCALWAYS1 ("State ID: %d", msg->msg.refReq.stateID);
      SLOGCALWAYS1 ("Ident   : %s", msg->msg.refReq.ident);
      if (msg->msg.refReq.offset)
        {
        for (j = 0; j < msg->msg.refReq.numIntegers; j++)
          SLOGCALWAYS2 ("Offset #%2d:  %2d", j, msg->msg.refReq.offset[j]);
        }
      break;

    case GSE_MSG_TYPE_GOOSE_ELE_REQ:
    case GSE_MSG_TYPE_GSSE_DATA_OFF_REQ:
      if (msg->msgType == GSE_MSG_TYPE_GOOSE_ELE_REQ)
        SLOGCALWAYS1 ("Message Type = %d, GSE_MSG_TYPE_GOOSE_ELE_REQ", msg->msgType);
      else
        SLOGCALWAYS1 ("Message Type = %d, GSE_MSG_TYPE_GSSE_DATA_OFF_REQ", msg->msgType);
      SLOGCALWAYS1 ("State ID: %d", msg->msg.offReq.stateID);
      SLOGCALWAYS1 ("Ident   : %s", msg->msg.offReq.ident);
      if (msg->msg.offReq.references)
        {
        for (j = 0; j < msg->msg.offReq.numVStrings; j++)
          SLOGCALWAYS2 ("Reference #%2d:  %s", j, msg->msg.offReq.references[j]);
        }
     break;

    case GSE_MSG_TYPE_GO_REF_RSP:
    case GSE_MSG_TYPE_GS_REF_RSP:
      if (msg->msgType == GSE_MSG_TYPE_GO_REF_RSP)
        SLOGCALWAYS1 ("Message Type = %d, GSE_MSG_TYPE_GO_REF_RSP", msg->msgType);
      else
        SLOGCALWAYS1 ("Message Type = %d, GSE_MSG_TYPE_GS_REF_RSP", msg->msgType);
      SLOGCALWAYS1 ("State ID: %d", msg->msg.refRsp.stateID);
      SLOGCALWAYS1 ("Ident   : %s", msg->msg.refRsp.ident);
      SLOGCALWAYS1 ("ConfRev : %d", msg->msg.refRsp.confRev);
      SLOGCALWAYS1 ("Dataset:  %s", msg->msg.refRsp.datSet);
      if (msg->msg.refRsp.result)
        {
        for (j = 0; j < msg->msg.refRsp.numResults; j++)
          {
          if (*msg->msg.refRsp.result[j].reference)
            SLOGCALWAYS2 ("Reference #%2d:  %s", j, msg->msg.refRsp.result[j].reference);
          else
            SLOGCALWAYS2 ("Error     #%2d:  %d", j, msg->msg.refRsp.result[j].error);
          }
        }
      break;

    case GSE_MSG_TYPE_GOOSE_ELE_RSP:
    case GSE_MSG_TYPE_GSSE_DATA_OFF_RSP:
      if (msg->msgType == GSE_MSG_TYPE_GOOSE_ELE_RSP)
        SLOGCALWAYS1 ("Message Type = %d, GSE_MSG_TYPE_GOOSE_ELE_RSP", msg->msgType);
      else
        SLOGCALWAYS1 ("Message Type = %d, GSE_MSG_TYPE_GSSE_DATA_OFF_RSP", msg->msgType);
      SLOGCALWAYS1 ("State ID: %d", msg->msg.offRsp.stateID);
      SLOGCALWAYS1 ("Ident   : %s", msg->msg.offRsp.ident);
      SLOGCALWAYS1 ("ConfRev : %d", msg->msg.offRsp.confRev);
      SLOGCALWAYS1 ("DataSet:  %s", msg->msg.offRsp.datSet);
      if (msg->msg.offRsp.result)
        {
        for (j = 0; j < msg->msg.offRsp.numResults; j++)
          {
          if (msg->msg.offRsp.result[j].offset)
            SLOGCALWAYS2 ("Offset #%2d:  %d", j, msg->msg.offRsp.result[j].offset);
          else
            SLOGCALWAYS2 ("Error  #%2d:  %d", j, msg->msg.offRsp.result[j].error);
          }
        }
      break;
    }	/* end "switch"	*/
  }
#endif	/* defined(GSE_MGMT_RX_SUPP)	*/

#if defined(SMPVAL_RX_SUPP)
/************************************************************************/
/*			u_smpval_ind					*/
/* User function to perform additional processing of SMPVAL indication.	*/
/* DEBUG: customize as needed.						*/
/************************************************************************/
ST_VOID u_smpval_ind (SMPVAL_MSG *smpvalMsg,
	ETYPE_INFO *etypeInfo,	/* Ethertype header info		*/
	SN_UNITDATA *sn_udt)	/* includes MAC addresses		*/
  {
ST_UINT j;  
  printf ("SampledValue message received, numASDU = %d, smpCnt values:", smpvalMsg->numASDU);
  for (j = 0; j < smpvalMsg->numASDU; j++)
    printf (" %d", smpvalMsg->asduArray[j].SmpCnt);
  printf ("\n  securityLen = %u\n", smpvalMsg->securityLen);
  /* If enabled, log decoded data.	*/
  if ((clnp_debug_sel & CLSNS_LOG_IND) != 0)
    smpval_msg_log (smpvalMsg, "SampledValue Indication:");
  }
#endif	/* defined(SMPVAL_RX_SUPP)	*/

/************************************************************************/
/*			sn_unitdata_clone				*/
/* Allocate and initialize a clone of a SN_UNITDATA structure.		*/
/* NOTE: sn_udt->lpdu points to allocated buffer. Just the pointer is	*/
/*   copied to the clone. The "lpdu" must not be freed until the	*/
/*   message is processed.						*/
/************************************************************************/
SN_UNITDATA *sn_unitdata_clone (SN_UNITDATA *sn_udt)
  {
SN_UNITDATA *sn_udt_clone;	/* clone of structure passed in	*/
  sn_udt_clone = chk_malloc (sizeof (SN_UNITDATA));
  memcpy (sn_udt_clone, sn_udt, sizeof (SN_UNITDATA));
  return (sn_udt_clone);
  }

#if defined(SMPVAL_RX_SUPP)
/************************************************************************/
/*			smpval_ind_process				*/
/* Process received SMPVAL message.					*/
/* CRITICAL: Must call "clnp_snet_free" to free SN_UNITDATA when done.	*/
/* RETURNS:	ST_VOID							*/
/************************************************************************/
ST_VOID smpval_ind_process (SN_UNITDATA *sn_udt)
  {
ETYPE_INFO etypeInfo;		/* Ethertype header info	*/
SMPVAL_MSG *smpvalMsg;		/* SMPVAL msg info structure	*/
ST_INT edition = 2;		/* DEBUG: change to 1 to do Edition 1	*/
				/* SampledValue decode			*/

  /* Decode IEC 61850 SMPVAL message. Also fill in etypeInfo.	*/
  smpvalMsg = smpval_msg_decode (sn_udt, edition, &etypeInfo);
  if (smpvalMsg)
    {
    u_smpval_ind (smpvalMsg, &etypeInfo, sn_udt);	/* process decoded info	*/

    smpval_msg_free (smpvalMsg);	/* done with "smpvalMsg", so free it	*/
    }

  clnp_snet_free (sn_udt);
  return;
  }
#endif	/* defined(SMPVAL_RX_SUPP)	*/

#if defined(GOOSE_RX_SUPP)
/************************************************************************/
/*			goose_ind_process				*/
/* Process received GOOSE message.					*/
/* CRITICAL: Must call "clnp_snet_free" to free SN_UNITDATA when done.	*/
/* RETURNS:	ST_VOID							*/
/************************************************************************/
ST_VOID goose_ind_process (SN_UNITDATA *sn_udt)
  {
  u_goose_ind (sn_udt);
  clnp_snet_free (sn_udt);
  return;
  }
#endif	/* defined(GOOSE_RX_SUPP)	*/

#if defined(GSE_MGMT_RX_SUPP)
/************************************************************************/
/*			gse_mgmt_ind_process				*/
/* Process received GSE Management message.				*/
/* CRITICAL: Must call "clnp_snet_free" to free SN_UNITDATA when done.	*/
/* RETURNS:	ST_VOID							*/
/************************************************************************/
ST_VOID gse_mgmt_ind_process (SN_UNITDATA *sn_udt)
  {
ETYPE_INFO etypeInfo; 
GSE_MGMT_MSG *msg;

  /* Decode IEC 61850 SMPVAL message. Also fill in etypeInfo.	*/
  msg = gse_mgmt_msg_decode (sn_udt, &etypeInfo);

  if (msg)
    {
    u_gse_mgmt_ind (msg, &etypeInfo, sn_udt);	/* pass decoded info to user*/

    gse_mgmt_msg_free (msg);
    }

  clnp_snet_free (sn_udt);
  return;
  }
#endif	/* defined(GSE_MGMT_RX_SUPP)	*/

#if defined(GSSE_RX_SUPP)	/* GSSE is only protocol using CLTP	*/
/************************************************************************/
/*			cltp_ind_process				*/
/* Process received CLTP (Connectionless OSI Transport) message.	*/
/* RETURNS:	ST_VOID							*/
/************************************************************************/
ST_VOID cltp_ind_process (N_UNITDATA *n_udt)
  {
  cltp_decode_nsdu (n_udt);	/* this decodes & processes it	*/
  }
#endif	/* defined(GSSE_RX_SUPP)	*/

#if defined(SUBNET_THREADS)
/* NOTE: the following put & get functions only needed with threads	*/
/*   One thread puts packets on list, another thread gets packets from list.*/

#if defined(GOOSE_RX_SUPP)
/************************************************************************/
/*			goose_ind_put					*/
/* Save packet on linked list.						*/
/* Applications should call "goose_ind_get" to get packet		*/
/* from linked list and "goose_ind_process" to process it.		*/
/************************************************************************/
ST_RET goose_ind_put (SN_UNITDATA *sn_udt)
  {
SN_UNITDATA *sn_clone;
  /* CRITICAL: must clone to put on list so data is valid later.	*/
  sn_clone = sn_unitdata_clone (sn_udt);
  S_LOCK_UTIL_RESOURCES();
  list_add_last (&goose_ind_list, sn_clone);
  S_UNLOCK_UTIL_RESOURCES();
  return (SD_SUCCESS); 	/* linked list function never fails	*/
  }
/************************************************************************/
/*			goose_ind_get					*/
/* Get packet from linked list.						*/
/************************************************************************/
SN_UNITDATA *goose_ind_get ()
  {
SN_UNITDATA *sn_udt;
  S_LOCK_UTIL_RESOURCES();
  sn_udt = (SN_UNITDATA *) list_get_first (&goose_ind_list);
  S_UNLOCK_UTIL_RESOURCES();
  return (sn_udt);
  }
#endif	/* defined(GOOSE_RX_SUPP)	*/

#if defined(GSSE_RX_SUPP)
/************************************************************************/
/*			cltp_ind_put					*/
/* Save packet on linked list.						*/
/* Applications should call "cltp_ind_get" to get packet		*/
/* from linked list and "cltp_ind_process" to process it.		*/
/************************************************************************/
ST_RET cltp_ind_put (N_UNITDATA *n_udt)
  {
  S_LOCK_UTIL_RESOURCES();
  list_add_last (&cltp_ind_list, n_udt);
  S_UNLOCK_UTIL_RESOURCES();
  return (SD_SUCCESS); 	/* linked list function never fails	*/
  }
/************************************************************************/
/*			cltp_ind_get					*/
/* Get packet from linked list.						*/
/************************************************************************/
N_UNITDATA *cltp_ind_get ()
  {
N_UNITDATA *n_udt;
  S_LOCK_UTIL_RESOURCES();
  n_udt = (N_UNITDATA *) list_get_first (&cltp_ind_list);
  S_UNLOCK_UTIL_RESOURCES();
  return (n_udt);
  }
#endif	/* defined(GSSE_RX_SUPP)	*/

#if defined(GSE_MGMT_RX_SUPP)
/************************************************************************/
/*			gse_mgmt_ind_put				*/
/* Save packet on linked list.						*/
/* Applications should call "gse_mgmt_ind_get" to get packet		*/
/* from linked list and "gse_mgmt_ind_process" to process it.		*/
/************************************************************************/
ST_RET gse_mgmt_ind_put (SN_UNITDATA *sn_udt)
  {
SN_UNITDATA *sn_clone;
  /* CRITICAL: must clone to put on list so data is valid later.	*/
  sn_clone = sn_unitdata_clone (sn_udt);
  S_LOCK_UTIL_RESOURCES();
  list_add_last (&gse_mgmt_ind_list, sn_clone);
  S_UNLOCK_UTIL_RESOURCES();
  return (SD_SUCCESS); 	/* linked list function never fails	*/
  }
/************************************************************************/
/*			gse_mgmt_ind_get				*/
/* Get packet from linked list.						*/
/************************************************************************/
SN_UNITDATA *gse_mgmt_ind_get ()
  {
SN_UNITDATA *sn_udt;
  S_LOCK_UTIL_RESOURCES();
  sn_udt = (SN_UNITDATA *) list_get_first (&gse_mgmt_ind_list);
  S_UNLOCK_UTIL_RESOURCES();
  return (sn_udt);
  }
#endif	/* defined(GSE_MGMT_RX_SUPP)	*/

#if defined(SMPVAL_RX_SUPP)
/************************************************************************/
/*			smpval_ind_put					*/
/* Save packet on linked list.						*/
/* Applications should call "smpval_ind_get" to get packet		*/
/* from linked list and "smpval_ind_process" to process it.		*/
/************************************************************************/
ST_RET smpval_ind_put (SN_UNITDATA *sn_udt)
  {
SN_UNITDATA *sn_clone;
  /* CRITICAL: must clone to put on list so data is valid later.	*/
  sn_clone = sn_unitdata_clone (sn_udt);
  S_LOCK_UTIL_RESOURCES();
  list_add_last (&smpval_ind_list, sn_clone);
  S_UNLOCK_UTIL_RESOURCES();
  return (SD_SUCCESS); 	/* linked list function never fails	*/
  }
/************************************************************************/
/*			smpval_ind_get					*/
/* Get packet from linked list.						*/
/************************************************************************/
SN_UNITDATA *smpval_ind_get ()
  {
SN_UNITDATA *sn_udt;
  S_LOCK_UTIL_RESOURCES();
  sn_udt = (SN_UNITDATA *) list_get_first (&smpval_ind_list);
  S_UNLOCK_UTIL_RESOURCES();
  return (sn_udt);
  }
#endif	/* defined(SMPVAL_RX_SUPP)	*/

#endif	/* defined(SUBNET_THREADS)	*/

#if defined(GSSE_RX_SUPP)
/************************************************************************/
/*			llc_ind						*/
/* Default processing for any received MAC frame where the		*/
/* "Length/Type" field indicates "Length". We assume it must be LLC.	*/
/* This may be TP4, IEC 61850 GSSE, etc.				*/
/* NOTE: Currently, only CLTP PDUs are supported, for IEC 61850 GSSE.	*/
/* DEBUG: If TP4 support is needed, add a function cotp_ind		*/
/*        which calls tp_decode_nsdu and call cotp_ind from 2nd switch.	*/
/* RETURNS:	ST_VOID							*/
/************************************************************************/
ST_VOID llc_ind (SN_UNITDATA *sn_udt)
  {
ST_RET		 retcode;
N_UNITDATA	*n_udt;
ST_UCHAR	 dest_lsap;	/* From LLC decoding			*/

  /* log the data received from subnetwork */
  clnpl_log_snsdu (sn_udt, CLSNS_LOG_IND);

  /* First decode the LLC layer.					*/
  retcode = clnpl_llc_decode (sn_udt->lpdu, &dest_lsap);
  if (retcode == SD_SUCCESS)
    {
    /* LLC decoding was OK.	*/
    switch (dest_lsap)
      {
      case LLC_LSAP:		/* Normal LSAP */
      /* Decode to get CLNP NPDU. This sets n_udt pointing to allocated	*/
      /* struct. Must call "clnp_free" later to free n_udt.		*/
      retcode = clnpl_decode (sn_udt, &n_udt);
      if (retcode == SD_SUCCESS)
        {
        clnpl_log_nsdu (n_udt, CLNP_LOG_IND);	/* log it	*/
 
        if (n_udt->data_len >= 2)	/* need at least 2 bytes to decode*/
          {
          /* Check TPDU type in 2nd byte.	*/
          switch (n_udt->data_buf[1])
            {
            case TP_PDU_TYPE_UD:	/* this is CLTP PDU	*/
#if defined(GSSE_RX_SUPP)	/* GSSE is only protocol using CLTP	*/
#if defined(SUBNET_THREADS)
            cltp_ind_put (n_udt);	/* queue to process later*/
            /* NOTE: must call clnp_free later after this is processed.	*/
#else
            cltp_ind_process (n_udt);	/* process now		*/
            clnp_free (n_udt);		/* free it	*/
#endif
#endif	/* GSSE_RX_SUPP	*/
            break;

            /* DEBUG: Add cases here to support other protocols like TP4.*/

            default:
            break;	/* protocol not supported. Ignore this frame.	*/
            }	/* end "inner" switch	*/
          }
        else
          {	/* NPDU too short to use. Ignore it.	*/
          clnp_free (n_udt);
          }
        }
      break;

      /* DEBUG: Add cases here to support other protocols.	*/

      default:
      break;	/* protocol not supported. Ignore this frame.	*/
      }		/* end "outer" switch	*/
    }
  /* Done with "sn_udt"	*/
  clnp_snet_free (sn_udt);
  return;
  }
#endif	/* defined(GSSE_RX_SUPP)	*/

/************************************************************************/
/*			subnet_serve					*/
/* Service the MAC interface. Call this periodically from main loop.	*/
/* This function receives a IEEE 802.3 MAC frame and processes it	*/
/* (if the appropriate protocol is enabled).				*/
/* NOTE: sn_udt->lpdu_len contains the 802.3 "Length/Type" field.	*/
/*   If (lpdu_len >= 0x600), it represents "Type" (e.g. ETYPE_TYPE_SV).	*/
/*   If (lpdu_len <  0x600), it represents "Length".			*/
/* NOTE: This function should only be called if one or more protocols	*/
/*       are enabled to receive IEEE 802.3 MAC frames.			*/
/* NOTE: Each processing function must free "sn_udt" by calling		*/
/*       clnp_snet_free when all processing is done. If no processing	*/
/*       function is called, this function frees "sn_udt".		*/
/* RETURNS:	SD_TRUE		if a MAC frame was received		*/
/*		SD_FALSE	if a MAC frame was NOT received.	*/
/************************************************************************/
ST_BOOLEAN subnet_serve (ST_VOID)
  {
SN_UNITDATA sn_udt;	/* MAC frame data (if clnp_snet_read succeeds)	*/
ST_RET read_ret;

  read_ret = clnp_snet_read (&sn_udt);
  if (read_ret != SD_SUCCESS)
    return (SD_FALSE);	/* MAC frame NOT received	*/

  /* MAC frame received. If appropriate protocol is enabled, process it.*/
  switch (sn_udt.lpdu_len)
    {
#if defined(SMPVAL_RX_SUPP)
    case ETYPE_TYPE_SV:		/* IEC 61850 SMPVAL msg.	*/

#if defined(SUBNET_THREADS)
      smpval_ind_put (&sn_udt);		/* queue to process later*/
#else
      smpval_ind_process (&sn_udt);	/* process it now	*/
#endif
      break;
#endif	/* SMPVAL_RX_SUPP	*/

#if defined(GOOSE_RX_SUPP)
    case ETYPE_TYPE_GOOSE:	/* IEC 61850 GOOSE msg.		*/

#if defined(SUBNET_THREADS)
      goose_ind_put (&sn_udt);		/* queue to process later*/
#else
      goose_ind_process (&sn_udt);	/* process it now	*/
#endif
      break;
#endif	/* GOOSE_RX_SUPP	*/

#if defined(GSE_MGMT_RX_SUPP)
    case ETYPE_TYPE_GSE:	/* IEC 61850 GSE Management msg.*/

#if defined(SUBNET_THREADS)
      gse_mgmt_ind_put (&sn_udt);	/* queue to process later*/
#else
      gse_mgmt_ind_process (&sn_udt);	/* process it now	*/
#endif
      break;
#endif	/* GSE_MGMT_RX_SUPP		*/

    default:
      /* Handle any other lpdu_len value.	*/
      /* Param contains "Length", not "Type", OR "Type" isn't recognized.*/
#if defined(GSSE_RX_SUPP)
      if (sn_udt.lpdu_len < 0x600)
        {
        /* lpdu_len represents "Length" (packet could be GSSE, TP4, etc).*/
        /* Do default processing.					*/
        llc_ind (&sn_udt);
        }
      else
#endif	/* GSSE_RX_SUPP		*/
        {
        /* CRITICAL: not processed, must free it now.	*/
        clnp_snet_free (&sn_udt);
        }
        
      break;
    }	/* end "switch"	*/

  return (SD_TRUE);	/* MAC frame received	*/
  }

