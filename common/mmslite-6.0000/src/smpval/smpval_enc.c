/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,       */
/*	2006-2012, All Rights Reserved.					*/
/*									*/
/*                  PROPRIETARY AND CONFIDENTIAL                        */
/*									*/
/* MODULE NAME : smpval_enc.c						*/
/* PRODUCT(S)  : MMS-EASE Lite						*/
/*									*/
/* MODULE DESCRIPTION : IEC 61850-9-2 SMPVAL encode functions.		*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :                            */
/*									*/
/* MODIFICATION LOG :                                                   */
/*  Date     Who   Rev                  Comments                        */
/* --------  ---  ------   -------------------------------------------  */
/* 01/11/12  JRB	   smpval_asdu_data_update: fix to correctly use*/
/*			   DatSetPres & RefrTmPres args. Fix svID len chk.*/
/*			   Add smpval_asdu_data_update_ed2.		*/
/* 06/12/07  RKR     03    ported to RHEL5                              */
/* 12/01/06  JRB     02    Fix warnings.				*/
/* 03/20/06  JRB     01    Created					*/
/************************************************************************/
#include "glbtypes.h"
#include "sysincs.h"
#include "smpval.h"
#include "mmsdefs.h"		/* many basic MMS definitions	*/
#include "clnp_log.h"

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif
/************************************************************************/
/*			smpval_msg_create				*/
/* Allocate and initialize new SMPVAL msg info structure.		*/
/* RETURNS: ptr to new SMPVAL msg info structure (NULL on error)	*/
/************************************************************************/
SMPVAL_MSG *smpval_msg_create (
	ST_UINT numASDU)	/* Num of ASDU concatenated into one APDU*/
  {
SMPVAL_MSG *smpvalMsg;

  smpvalMsg = (SMPVAL_MSG *) chk_calloc (1, sizeof (SMPVAL_MSG));
  smpvalMsg->numASDU = numASDU;

  /* Allocate array of ASDU structs.	*/
  smpvalMsg->asduArray = chk_calloc (numASDU, sizeof (SMPVAL_ASDU));


  return (smpvalMsg);
  }

/************************************************************************/
/*			smpval_msg_destroy				*/
/* Free up all resources for a SMPVAL msg info structure.		*/
/************************************************************************/
ST_VOID smpval_msg_destroy (SMPVAL_MSG *smpvalMsg)
  {
  if (smpvalMsg)
    {
    chk_free (smpvalMsg->asduArray);
    chk_free (smpvalMsg);
    }
  }

/************************************************************************/
/*			reverse_bytes					*/
/* Copies data from source to destination but reverses the order of the	*/
/* bytes (i.e. converts Big-Endian to Little-Endian or vice versa).	*/
/************************************************************************/
ST_VOID reverse_bytes (ST_UINT8 *dst, ST_UINT8 *src, ST_INT numbytes)
  {
  src += numbytes;
  while (numbytes-- > 0)
    *dst++ = *(--src);
  }

/************************************************************************/
/*			smpval_asdu_data_update				*/
/* Updates data stored for one ASDU to be sent in a SMPVAL message		*/
/* NOTE: if this function is too slow, the data may also be updated	*/
/*   directly by accessing smpvalMsg->asduArray[asduIdx]. Also, a similar	*/
/*   function could be written to update only certain members of the	*/
/*   ASDU (i.e. the ones that are changing).				*/
/************************************************************************/
ST_RET smpval_asdu_data_update (
	SMPVAL_MSG *smpvalMsg,	/* SMPVAL msg info structure		*/
	ST_INT asduIdx,		/* index into array of ASDU for this SMPVAL msg*/
	ST_UINT8 *SamplePtr,	/* pointer to data			*/
	ST_INT SampleLen,	/* length of data in bytes		*/
	ST_CHAR *svID,
	ST_INT SmpCnt,
	ST_BOOLEAN DatSetPres,	/* SD_TRUE if optional DatSet should be sent*/
	ST_CHAR *DatSet,	/* Optional (chk DatSetPres arg)	*/
	ST_UINT32 ConfRev,
	ST_BOOLEAN SmpSynch,
	ST_BOOLEAN RefrTmPres,	/* SD_TRUE if optional RefrTm should be sent*/
	MMS_UTC_TIME *RefrTm,	/* Optional (chk_RefrTmPres arg)	*/
	ST_BOOLEAN SmpRatePres,	/* SD_TRUE if optional SmpRate should be sent*/
	ST_UINT16 SmpRate)	/* Optional (chk SmpRatePres arg)	*/
  {
SMPVAL_ASDU *asdu;
ST_RET retcode = SD_SUCCESS;	/* assume success until problem found	*/

  if (asduIdx < smpvalMsg->numASDU)
    {
    asdu = &smpvalMsg->asduArray[asduIdx];
    asdu->SamplePtr =  SamplePtr;
    asdu->SampleLen =  SampleLen;

    if (strlen (svID) <= MAX_SMPVAL_SVID_LEN_ED1)
      strcpy (asdu->svID, svID);
    else
      {
      CLNP_LOG_ERR0 ("smpval_asdu_data_put: smpvalID too long");
      retcode = SD_FAILURE;
      }
    if (DatSetPres)
      {
      asdu->DatSetPres = SD_TRUE;
      if (strlen (DatSet) < sizeof (asdu->DatSet))
        strcpy (asdu->DatSet, DatSet);
      else
        {
        CLNP_LOG_ERR0 ("smpval_asdu_data_put: DatSet too long");
        retcode = SD_FAILURE;
        }
      }
    asdu->SmpCnt = SmpCnt;
    asdu->ConfRev =  ConfRev;
    asdu->SmpSynch = SmpSynch;
    if (RefrTmPres)
      {
      asdu->RefrTmPres = SD_TRUE;
      asdu->RefrTm = *RefrTm;	/* NOTE: this copies whole structure*/
      }
    asdu->SmpRatePres = SmpRatePres;
    asdu->SmpRate = SmpRate;
    }
  else
    {
    CLNP_LOG_ERR2 ("smpval_asdu_data_put: ASDU index %d exceeds max %d", asduIdx, smpvalMsg->numASDU - 1);
    retcode = SD_FAILURE;
    }
    
  return (retcode);
  }
/************************************************************************/
/*			smpval_asdu_data_update_ed2			*/
/* Similar function for Edition 2.					*/
/* Differences from Ed 1:						*/
/*	svID is longer							*/
/*	SmpSynch changed from BOOLEAN to UINT8				*/
/*	SmpMod added.							*/
/************************************************************************/
ST_RET smpval_asdu_data_update_ed2 (
	SMPVAL_MSG *smpvalMsg,	/* SMPVAL msg info structure		*/
	ST_INT asduIdx,		/* index into array of ASDU for this SMPVAL msg*/
	ST_UINT8 *SamplePtr,	/* pointer to data			*/
	ST_INT SampleLen,	/* length of data in bytes		*/
	ST_CHAR *svID,
	ST_INT SmpCnt,
	ST_BOOLEAN DatSetPres,	/* SD_TRUE if optional DatSet should be sent*/
	ST_CHAR *DatSet,	/* Optional (chk DatSetPres arg)	*/
	ST_UINT32 ConfRev,
	ST_UINT8 SmpSynch,	/* 0-255				*/
	ST_BOOLEAN RefrTmPres,	/* SD_TRUE if optional RefrTm should be sent*/
	MMS_UTC_TIME *RefrTm,	/* Optional (chk_RefrTmPres arg)	*/
	ST_BOOLEAN SmpRatePres,	/* SD_TRUE if optional SmpRate should be sent*/
	ST_UINT16 SmpRate,	/* Optional (chk SmpRatePres arg)	*/
	/* These args are used for Ed 2 only	*/
	ST_BOOLEAN SmpModPres,	/* SD_TRUE if optional SmpMod should be sent*/
	ST_UINT16 SmpMod)	/* Optional (chk SmpModPres arg)	*/
  {
SMPVAL_ASDU *asdu;
ST_RET retcode = SD_SUCCESS;	/* assume success until problem found	*/

  if (asduIdx < smpvalMsg->numASDU)
    {
    asdu = &smpvalMsg->asduArray[asduIdx];
    asdu->SamplePtr =  SamplePtr;
    asdu->SampleLen =  SampleLen;

    if (strlen (svID) <= MAX_SMPVAL_SVID_LEN_ED2)
      strcpy (asdu->svID, svID);
    else
      {
      CLNP_LOG_ERR0 ("smpval_asdu_data_put: svID too long");
      retcode = SD_FAILURE;
      }
    if (DatSetPres)
      {
      asdu->DatSetPres = SD_TRUE;
      if (strlen (DatSet) < sizeof (asdu->DatSet))
        strcpy (asdu->DatSet, DatSet);
      else
        {
        CLNP_LOG_ERR0 ("smpval_asdu_data_put: DatSet too long");
        retcode = SD_FAILURE;
        }
      }
    asdu->SmpCnt = SmpCnt;
    asdu->ConfRev =  ConfRev;
    /* NOTE: SmpSynch is stored in ST_BOOLEAN (like Edition 1) but it	*/
    /*       is really ST_UINT8 (1-255). This is OK because both map to	*/
    /*       "unsigned char".						*/
    asdu->SmpSynch = SmpSynch;
    if (RefrTmPres)
      {
      asdu->RefrTmPres = SD_TRUE;
      asdu->RefrTm = *RefrTm;	/* NOTE: this copies whole structure*/
      }
    asdu->SmpRatePres = SmpRatePres;
    asdu->SmpRate = SmpRate;
    /* NOTE: SmpModPres & SmpMod used for Edition 2 only	*/
    asdu->SmpModPres = SmpModPres;
    asdu->SmpMod = SmpMod;
    }
  else
    {
    CLNP_LOG_ERR2 ("smpval_asdu_data_put: ASDU index %d exceeds max %d", asduIdx, smpvalMsg->numASDU - 1);
    retcode = SD_FAILURE;
    }
    
  return (retcode);
  }
/************************************************************************/
/*			smpval_asdu_encode					*/
/* Encode one SMPVAL ASDU in the middle of a SMPVAL message.			*/
/* RETURNS: SD_VOID							*/
/************************************************************************/
static ST_VOID smpval_asdu_encode (
	ASN1_ENC_CTXT *aCtx,
	ST_INT edition,		/* 1 for Edition 1, 2 for Edition 2	*/
	SMPVAL_ASDU *smpvalAsdu)
  {
#if (SD_BYTE_ORDER==SD_LITTLE_ENDIAN)
ST_UINT8 tmp_ostr [4];
#endif

  asn1r_strt_constr (aCtx); 	/* Start SEQ	*/

  if (edition == 2)
    {
    /* NOTE: "SmpMod" should only be present if this is Edition 2 encode.	*/
    /* "SmpMod" is optional. Chk "SmpModPres" flag.	*/
    if (smpvalAsdu->SmpModPres)
      {
      /* NOTE: ST_UINT16 encoded as Octet String.	*/
#if (SD_BYTE_ORDER==SD_LITTLE_ENDIAN)
      reverse_bytes (tmp_ostr, (ST_UINT8 *) &smpvalAsdu->SmpMod, 2);
      asn1r_wr_octstr (aCtx, tmp_ostr, 2);
#else
      asn1r_wr_octstr (aCtx, (ST_UINT8 *) &smpvalAsdu->SmpMod, 2);
#endif
      asn1r_fin_prim (aCtx,8,CTX);	/* finish "SmpMod"	*/
      }
    }

  asn1r_wr_octstr (aCtx, smpvalAsdu->SamplePtr, smpvalAsdu->SampleLen);	
  asn1r_fin_prim (aCtx,7,CTX);		/* finish "sample"	*/

  /* "SmpRate" is optional. Chk "SmpRatePres" flag.	*/
  if (smpvalAsdu->SmpRatePres)
    {
    /* NOTE: ST_UINT16 encoded as Octet String.	*/
#if (SD_BYTE_ORDER==SD_LITTLE_ENDIAN)
    reverse_bytes (tmp_ostr, (ST_UINT8 *) &smpvalAsdu->SmpRate, 2);
    asn1r_wr_octstr (aCtx, tmp_ostr, 2);
#else
    asn1r_wr_octstr (aCtx, (ST_UINT8 *) &smpvalAsdu->SmpRate, 2);
#endif
    asn1r_fin_prim (aCtx,6,CTX);		/* finish "SmpRate"	*/
    }

  /* NOTE: "SmpSynch" encoding is different for Edition 1 or 2.	*/
  if (edition == 2)
    {
    /* SmpSynch "Tag [5] IMPLICIT OCTET STRING 1".	*/
    asn1r_wr_octstr (aCtx, &smpvalAsdu->SmpSynch_ed2, 1);
    asn1r_fin_prim (aCtx,5,CTX);		/* finish "SmpSynch"	*/
    }
  else
    {	/* edition == 1	*/
    /* SmpSynch "Tag [5] IMPLICIT BOOLEAN DEFAULT FALSE".	*/
    /* DEFAULT is FALSE, so only encode this if it is TRUE	*/
    if (smpvalAsdu->SmpSynch)
      {
      asn1r_wr_bool (aCtx, smpvalAsdu->SmpSynch);
      asn1r_fin_prim (aCtx,5,CTX);		/* finish "SmpSynch"	*/
      }
    }

  /* "RefrTm" is optional. Chk "RefrTmPres" flag.	*/
  if (smpvalAsdu->RefrTmPres)
    {
    asn1r_wr_utc_time (aCtx, &smpvalAsdu->RefrTm);
    asn1r_fin_prim (aCtx,4,CTX);		/* finish "RefrTm"	*/
    }

    /* NOTE: ST_UINT32 encoded as Octet String.	*/
#if (SD_BYTE_ORDER==SD_LITTLE_ENDIAN)
  reverse_bytes (tmp_ostr, (ST_UINT8 *) &smpvalAsdu->ConfRev, 4); 
  asn1r_wr_octstr (aCtx, tmp_ostr, 4);
#else
  asn1r_wr_octstr (aCtx, (ST_UINT8 *) &smpvalAsdu->ConfRev, 4);
#endif
  asn1r_fin_prim (aCtx,3,CTX);		/* finish "ConfRev"	*/

    /* NOTE: ST_UINT16 encoded as Octet String.	*/
#if (SD_BYTE_ORDER==SD_LITTLE_ENDIAN)
  reverse_bytes (tmp_ostr, (ST_UINT8 *) &smpvalAsdu->SmpCnt, 2); 
  asn1r_wr_octstr (aCtx, tmp_ostr, 2);
#else
  asn1r_wr_octstr (aCtx, (ST_UINT8 *) &smpvalAsdu->SmpCnt, 2);
#endif
  asn1r_fin_prim (aCtx,2,CTX);		/* finish "SmpCnt"	*/

  /* "DatSet" is optional. Chk "DatSetPres" flag.	*/
  if (smpvalAsdu->DatSetPres)
    {
    asn1r_wr_vstr (aCtx, smpvalAsdu->DatSet);
    asn1r_fin_prim (aCtx,1,CTX);
    }

  asn1r_wr_vstr (aCtx, smpvalAsdu->svID);
  asn1r_fin_prim (aCtx,0,CTX);

  asn1r_fin_constr (aCtx,SEQ_CODE,UNI,DEF);	/* finish SEQ (UNIVERSAL SEQ)*/

  return;
  }
/************************************************************************/
/*			smpval_msg_encode					*/
/* Encodes complete SMPVAL message (APDU plus Ethertype header).	  	*/
/* RETURNS: ptr to start of encoded message (or NULL on error)		*/
/* NOTE: Ethertype header must be encoded in the same buffer as smpvalMsg,	*/
/*   so it is done here for efficiency.					*/
/************************************************************************/
ST_UCHAR *smpval_msg_encode (
	SMPVAL_MSG *smpvalMsg,	/* SMPVAL msg info structure		*/
	ETYPE_INFO *etypeInfo,	/* Ethertype info	*/
	ST_UCHAR *asn1Buf,	/* buffer to encode in	*/
	ST_INT asn1BufLen,	/* len of buffer	*/
	ST_INT *asn1LenOut)	/* ptr to len encoded	*/
  {
ST_RET retcode=SD_SUCCESS;	/* assume SUCCESS	*/
ASN1_ENC_CTXT aCtx;
ST_UCHAR *asn1Start = NULL;	/* ptr to start of ASN1 encoded data	*/
				/* init to NULL (i.e. failure)		*/
ST_INT asduIdx;			/* index into array of ASDU		*/

  asn1r_strt_asn1_bld (&aCtx, asn1Buf, asn1BufLen);

  asn1r_strt_constr (&aCtx);	/* Start "savPdu" (APPLICATION 0 IMPLICIT SEQUENCE)*/
  asn1r_strt_constr (&aCtx); 	/* Start "asdu"	(CONTEXT 2 IMPLICIT SEQUENCE)*/

  /* Encode each ASDU from data in smpvalMsg->asduArray.		*/
  /* CRITICAL: Encode ASDUs in reverse order (last ASDU first).	*/
  for (asduIdx = smpvalMsg->numASDU-1; asduIdx >= 0; asduIdx--)
    {
    smpval_asdu_encode (&aCtx, smpvalMsg->edition, &smpvalMsg->asduArray[asduIdx]);
    }

  asn1r_fin_constr (&aCtx,2,CTX,DEF);	/* Finish "asdu" (CONTEXT 2 IMPLICIT SEQ)*/

  /* Encode optional "security" (tag 1) if needed.	*/
  if (smpvalMsg->securityLen)
    {
    asn1r_wr_octstr (&aCtx, smpvalMsg->securityBuf, smpvalMsg->securityLen);
    asn1r_fin_prim (&aCtx,1,CTX);		/* finish "security"	*/
    }

  if (retcode == SD_SUCCESS)	/* encoding OK so far?	*/
    {
    /* do rest of encoding	*/
    asn1r_wr_u16 (&aCtx, smpvalMsg->numASDU);
    asn1r_fin_prim (&aCtx,0,CTX);		/* Finish "numASDU"	*/

    asn1r_fin_constr (&aCtx,0,APP,DEF);	/* Finish "savPdu" (APPLICATION 0 IMPLICIT SEQ)*/

    /* ASN.1 encoding done. Chk for overrun. */
    if (aCtx.asn1r_encode_overrun)
      {
      CLNP_LOG_ERR0 ("IEC SMPVAL encode overrun");
      }
    else
      {
      *asn1LenOut = ASN1_ENC_LEN(&aCtx);	/* len encoded	*/
      
      /* Finally, add the Ethertype information */
      /* This is the only place we set return to GOOD value.		*/
      asn1Start = etype_hdr_encode(
                    ASN1_ENC_PTR(&aCtx),	/* current position in encode buf*/
                    asn1BufLen,	/* len of encode buffer	*/
                    asn1LenOut,	/* ptr to len encoded	*/
                    etypeInfo);	/* Ethertype info struct	*/
      }
    }
  
  return (asn1Start);		/* ptr to start (or NULL on error)	*/
  }

/************************************************************************/
/*			     smpval_msg_send	     			*/
/* Encode and send SMPVAL message using information stored in		*/
/* SMPVAL_MSG struct.						*/
/* RETURNS: SD_SUCCESS or SD_FAILURE					*/
/************************************************************************/
ST_RET smpval_msg_send (
	SMPVAL_MSG *smpvalMsg,	/* SMPVAL msg info structure		*/
	ETYPE_INFO *etypeInfo,	/* Ethertype info	*/
	ST_UCHAR *dstMac)	/* Destination MAC address	*/
  {
  ST_UCHAR encBuf [ETHE_MAX_LEN_LSDU];	/* big enough to encode complete frame*/
  ST_UCHAR *encPtr;	/* ptr to data including 802.3 Length/Type	*/
  ST_UINT encLen;	/* len of data					*/
  ST_RET retcode;

#if defined(DEBUG_SISCO)
  if ((clnp_debug_sel & CLSNS_LOG_REQ) != 0)
    smpval_msg_log (smpvalMsg, "SampledValue Request:");
#endif

  encPtr = smpval_msg_encode (smpvalMsg,
              etypeInfo,
              encBuf,		/* buffer to encode in	*/
              sizeof(encBuf),	/* len of buffer	*/
              (ST_INT *)&encLen);		/* ptr to len encoded*/
  if (encPtr==NULL)
    {
    CLNP_LOG_ERR0 ("smpval_msg_encode failed");
    retcode = SD_FAILURE;
    }
  else
    {	/* send message	*/
    assert (encLen+12 <= sizeof (encBuf));
    memcpy (encPtr-12, dstMac, CLNP_MAX_LEN_MAC);		/* DST MAC*/
    memcpy (encPtr-6, clnp_param.loc_mac, CLNP_MAX_LEN_MAC);	/* SRC MAC*/
    retcode = clnp_snet_write_raw (encPtr-12, encLen+12);
    }
  return (retcode);
  }

