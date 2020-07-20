/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	2006-2011, All Rights Reserved.					*/
/*									*/
/*			PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : smpval_dec.c						*/
/* PRODUCT(S)  : MMS-EASE Lite						*/
/*									*/
/* MODULE DESCRIPTION : IEC 61850-9-2 SMPVAL decode functions.		*/
/* This module uses a newer decode scheme based on asn1r_get_tag with	*/
/* the following advantages:						*/
/* 1. Don't need "usr_info" to pass info through parser. Instead, just	*/
/*    pass what's needed to lower functs.				*/
/* 2. Returns immediately on any error. Doesn't call asn1r_set_dec_err.	*/
/* 3. Uses asn1r_chk_tag to manage sequence.				*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*			smpval_msg_decode				*/
/*			smpval_msg_free					*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev     Comments					*/
/* --------  ---  ------   -------------------------------------------  */
/* 12/14/11  JRB	   Rewritten using asn1r_get_tag (faster).	*/
/*			   smpval_msg_decode: added "edition" arg.	*/
/* 12/01/06  JRB     02    Fix warnings.				*/
/* 03/20/06  JRB     01    Created					*/
/************************************************************************/
#include "glbtypes.h"
#include "sysincs.h"
#include "asn1r.h"
#include "mem_chk.h"
#include "asn1log.h"
#include "clnp_sne.h"
#include "smpval.h"
#include "clnp_log.h"

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/* ASN.1 tags at top level (APPLICATION)	*/
#define TAG_SAVPDU	(ASN1MASK_APP | ASN1MASK_CONSTR | 0)

/* ASN.1 tags at top level inside SAVPDU 	*/
#define TAG_NOASDU	(ASN1MASK_CTX | 0)
#define TAG_SECURITY	(ASN1MASK_CTX | 1)
/* ASN.1 constructor tag for start of "Sequence of ASDU".	*/
#define TAG_ASDU_SEQ	(ASN1MASK_CTX | ASN1MASK_CONSTR | 2)

/* ASN.1 constructor tag for start of "ASDU".	*/
#define TAG_ASDU	(ASN1MASK_UNI | ASN1MASK_CONSTR | SEQ_CODE)
/* ASN.1 tags inside ASDU			*/
#define TAG_SVID	(ASN1MASK_CTX |0)
#define TAG_DATSET	(ASN1MASK_CTX |1)
#define TAG_SMPCNT	(ASN1MASK_CTX |2)
#define TAG_CONFREV	(ASN1MASK_CTX |3)
#define TAG_REFRTM	(ASN1MASK_CTX |4)
#define TAG_SMPSYNCH	(ASN1MASK_CTX |5)
#define TAG_SMPRATE	(ASN1MASK_CTX |6)
#define TAG_SAMPLE	(ASN1MASK_CTX |7)
#define TAG_SMPMOD	(ASN1MASK_CTX |8)	/* added in Edition 2*/

/************************************************************************/
/*			decode_asdu					*/
/* NOTE: decoding is slightly different for Edition 1 and Edition 2	*/
/*       (smpvalMsg->edition is used to select behavior).		*/
/************************************************************************/
static ST_RET decode_asdu (ASN1_DEC_CTXT *aCtx,
	SMPVAL_MSG *smpvalMsg,
	SMPVAL_ASDU *asdu)	/* ptr to array elem to fill in	*/

  {
ST_RET rc = SD_SUCCESS;
ST_UINT16 tagnum;
ST_UINT8 tmp_ostr [4];
/* Edition 1 tags (from IEC 61850-9-2 Edition 1).	*/
/* This never changes so a static variable is more efficient.		*/
static TAG_INFO tag_arr_ed1 [] = {		/* Edition 1 tags	*/
	{TAG_SVID,    TAG_MANDATORY},
	{TAG_DATSET,  TAG_OPTIONAL},
	{TAG_SMPCNT,  TAG_MANDATORY},
	{TAG_CONFREV, TAG_MANDATORY},
	{TAG_REFRTM,  TAG_OPTIONAL},
	{TAG_SMPSYNCH,TAG_OPTIONAL},	/* default=FALSE	*/
	/* 9-2 Encoding shows MANDATORY, but SmpRate must be OPTIONAL based on OptFlds.*/
	{TAG_SMPRATE, TAG_OPTIONAL},
	{TAG_SAMPLE,  TAG_MANDATORY} };

/* Edition 2 tags (from IEC 61850-9-2 Edition 2).	*/
/* This never changes so a static variable is more efficient.		*/
static TAG_INFO tag_arr_ed2 [] = {
	{TAG_SVID,    TAG_MANDATORY},
	{TAG_DATSET,  TAG_OPTIONAL},
	{TAG_SMPCNT,  TAG_MANDATORY},
	{TAG_CONFREV, TAG_MANDATORY},
	{TAG_REFRTM,  TAG_OPTIONAL},
	{TAG_SMPSYNCH,TAG_MANDATORY},
	{TAG_SMPRATE, TAG_OPTIONAL},
	{TAG_SAMPLE,  TAG_MANDATORY},
	{TAG_SMPMOD,  TAG_OPTIONAL} };	/* this tag ONLY for Edition 2	*/
int tag_idx = 0;	/* index into tag_arr array (next to look for)	*/

  /* Loop decoding tags until error or end of this constructor.*/
  while (rc == SD_SUCCESS	/* no error on last loop iteration	*/
         && (rc = asn1r_get_tag (aCtx, &tagnum)) == SD_SUCCESS
         && tagnum != 0)	/* 0 indicates end of constructor	*/
    {
    if (smpvalMsg->edition == 2)
      rc = asn1r_chk_tag (tagnum, tag_arr_ed2, (sizeof(tag_arr_ed2)/sizeof(TAG_INFO)), &tag_idx);
    else
      rc = asn1r_chk_tag (tagnum, tag_arr_ed1, (sizeof(tag_arr_ed1)/sizeof(TAG_INFO)), &tag_idx);
    if (rc != SD_SUCCESS)
      {
      CLNP_LOG_ERR1 ("IEC 61850-9-2 decode: unexpected tag 0x%X", tagnum);
      break;
      }

    /* This switch handles each possible tag.*/
    switch (tagnum)
      {
      case TAG_SVID:					/* svID	*/
        if (asn1r_get_vstr_maxlen (aCtx, asdu->svID, sizeof(asdu->svID)-1) != SD_SUCCESS)
          return (ASN1E_DECODE_OTHER);
        break;
  
      case TAG_DATSET:					/* DatSet (Optional)	*/
        asdu->DatSetPres = SD_TRUE;
        if (asn1r_get_vstr_maxlen (aCtx, asdu->DatSet, sizeof(asdu->DatSet)-1) != SD_SUCCESS)
          return (ASN1E_DECODE_OTHER);
        break;
  
      case TAG_SMPCNT:					/* SmpCnt	*/
        if (asn1r_get_octstr_fixlen (aCtx, tmp_ostr, 2) == SD_SUCCESS)
          {
#if (SD_BYTE_ORDER==SD_LITTLE_ENDIAN)
          reverse_bytes ((ST_UINT8 *) &asdu->SmpCnt, tmp_ostr, 2);
#else
          asdu->SmpCnt = *(ST_UINT16 *) tmp_ostr;
#endif
          }
        else
          return (ASN1E_DECODE_OTHER);
        break;
  
      case TAG_CONFREV:					/* ConfRev	*/
        if (asn1r_get_octstr_fixlen (aCtx, tmp_ostr, 4) == SD_SUCCESS)
          {
#if (SD_BYTE_ORDER==SD_LITTLE_ENDIAN)
          reverse_bytes ((ST_UINT8 *) &asdu->ConfRev, tmp_ostr, 4);
#else
          asdu->ConfRev = *(ST_UINT32 *) tmp_ostr;
#endif
          }
        else
          return (ASN1E_DECODE_OTHER);
        break;
  
      case TAG_REFRTM:					/* RefrTm (optional)	*/
        asdu->RefrTmPres = SD_TRUE;
        if (asn1r_get_utc_time (aCtx, &asdu->RefrTm) != SD_SUCCESS)
          return (ASN1E_DECODE_OTHER);
        break;
  
      case TAG_SMPSYNCH:					/* SmpSynch (optional)	*/
        /* This is BOOLEAN in Edition 1, UINT8  in Edition 2.	*/
        if (smpvalMsg->edition == 2)
          /* UINT8 encoded as 1 byte octetstring (reverse_bytes not needed)*/
          rc = asn1r_get_octstr_fixlen (aCtx, &asdu->SmpSynch_ed2, 1);
        else
          rc = asn1r_get_bool (aCtx, &asdu->SmpSynch);
        if (rc != SD_SUCCESS)
          return (ASN1E_DECODE_OTHER);
        break;
  
      case TAG_SMPRATE:					/* SmpRate (optional)	*/
        if (asn1r_get_octstr_fixlen (aCtx, tmp_ostr, 2) == SD_SUCCESS)
          {
          asdu->SmpRatePres = SD_TRUE;
#if (SD_BYTE_ORDER==SD_LITTLE_ENDIAN)
          reverse_bytes ((ST_UINT8 *) &asdu->SmpRate, tmp_ostr, 2);
#else
          asdu->SmpRate = *(ST_UINT16 *) tmp_ostr;
#endif
          }
        else
          return (ASN1E_DECODE_OTHER);
        break;
  
      case TAG_SAMPLE:					/* sample	*/
        /* Set SampleLen and SamplePtr (points within ASN1 decode buffer).*/
        asdu->SampleLen = ASN1_DEC_LEN(aCtx);
        asdu->SamplePtr = ASN1_DEC_PTR(aCtx);
        if (asn1r_skip_elmnt (aCtx) != SD_SUCCESS)	/* Skip over this ASN.1 element.*/
          return (ASN1E_DECODE_OTHER);
        break;
  
      /* This case should only execute for Edition 2.	*/
      case TAG_SMPMOD:					/* SmpMod (optional)*/
        if (asn1r_get_octstr_fixlen (aCtx, tmp_ostr, 2) == SD_SUCCESS)
          {
          asdu->SmpModPres = SD_TRUE;
#if (SD_BYTE_ORDER==SD_LITTLE_ENDIAN)
          reverse_bytes ((ST_UINT8 *) &asdu->SmpMod, tmp_ostr, 2);
#else
          asdu->SmpMod = *(ST_UINT16 *) tmp_ostr;
#endif
           }
        else
          return (ASN1E_DECODE_OTHER);
        break;

      default:				/* Don't know how to decode.	*/
        /* NOTE: asn1r_chk_tag should catch this before we ever get here.*/
        CLNP_LOG_ERR1 ("Decode error: unexpected tag 0x%X.", tagnum);
        return (ASN1E_UNEXPECTED_TAG);
      }	/* End "switch (tagnum)"	*/
    }	/* end "while" loop	*/
  /* Decoded to end of ASDU (or error).	*/
  return (rc);
  }
/************************************************************************/
/*			decode_asdu_seq					*/
/************************************************************************/
static ST_RET decode_asdu_seq (ASN1_DEC_CTXT *aCtx,
	SMPVAL_MSG *smpvalMsg)
  {
ST_RET rc = SD_SUCCESS;
ST_UINT16 tagnum;
SMPVAL_ASDU *asdu;
ST_INT asduIdx = 0;		/* index into array of ASDU	*/

/* NOTE: only one mandatory tag expected here (TAG_ASDU).		*/
/* Just check it in loop. Don't need "tag_arr" like other functions.	*/

  /* Loop decoding tags until error or end of this constructor.*/
  while (rc == SD_SUCCESS	/* no error on last loop iteration	*/
         && (rc = asn1r_get_tag (aCtx, &tagnum)) == SD_SUCCESS
         && tagnum != 0)	/* 0 indicates end of constructor	*/
    {
    /* NOTE: We're decoding a SEQUENCE of SEQUENCE of ASDU now. We expect*/
    /* to find the same constructor (start of ASDU) each time through	*/
    /* this loop.							*/
    if (tagnum != TAG_ASDU)
      {
      CLNP_LOG_ERR1 ("IEC 61850-9-2 decode: unexpected tag 0x%X", tagnum);
      break;
      }

    /* NOTE: asduIdx starts at 0, and is incremented after each ASDU decoded.*/
    asdu = &smpvalMsg->asduArray[asduIdx];

    /* This switch handles each possible tag.*/
    switch (tagnum)
      {
      case TAG_ASDU:
        /* Decoded "ASDU" constructor. Decode to end of this constructor.*/
        rc = decode_asdu (aCtx, smpvalMsg, asdu);
        if (rc)
          return (rc);	/* return on first error	*/
        asduIdx++;	/* get ready for next ASDU	*/
        break;

      default:				/* Don't know how to decode.	*/
        /* NOTE: asn1r_chk_tag should catch this before we ever get here.*/
        CLNP_LOG_ERR1 ("Decode error: unexpected tag 0x%X.", tagnum);
        return (ASN1E_UNEXPECTED_TAG);
      }	/* End "switch (tagnum)"	*/
    }	/* end "while" loop	*/
  /* Decoded to end of "Sequence of ASDU" (or error).	*/
  return (rc);
  }
/************************************************************************/
/*			decode_apdu					*/
/************************************************************************/
static ST_RET decode_apdu (ASN1_DEC_CTXT *aCtx,
	SMPVAL_MSG *smpvalMsg)
  {
/* This TAG array created from info in IEC 61850-9-2.	*/
/* This never changes so a static variable is more efficient.		*/
static TAG_INFO tag_arr [] = {
	{TAG_NOASDU, TAG_MANDATORY},
	{TAG_SECURITY, TAG_OPTIONAL},
	{TAG_ASDU_SEQ, TAG_MANDATORY} };
ST_RET rc = SD_SUCCESS;
ST_UINT16 tagnum;
int tag_idx = 0;	/* index into tag_arr array (next to look for)	*/

  /* Loop decoding tags until error or end of this constructor.*/
  while (rc == SD_SUCCESS	/* no error on last loop iteration	*/
         && (rc = asn1r_get_tag (aCtx, &tagnum)) == SD_SUCCESS
         && tagnum != 0)	/* 0 indicates end of constructor	*/
    {
    rc = asn1r_chk_tag (tagnum, tag_arr, (sizeof(tag_arr)/sizeof(TAG_INFO)), &tag_idx);
    if (rc != SD_SUCCESS)
      {
      return (rc);	/* error message already logged	*/
      }

    /* This switch handles each possible tag.*/
    switch (tagnum)
      {
      case TAG_NOASDU:
        if (asn1r_get_u16 (aCtx, &smpvalMsg->numASDU) == SD_SUCCESS)
          {
          /* CRITICAL: allocate asduArray now.	*/
          smpvalMsg->asduArray = chk_calloc (smpvalMsg->numASDU, sizeof(SMPVAL_ASDU));
          /* NOTE: because of calloc, "SmpSynch" is correctly	*/
          /*   set to default value. No need to change it if not present in PDU.*/ 
          }
        else
          return (ASN1E_DECODE_OTHER);
        break;

      case TAG_SECURITY:
        /* Just copy encoded "security" into smpvalMsg->securityBuf.	*/
        if (asn1r_get_octstr_varlen (aCtx, smpvalMsg->securityBuf, MAX_SMPVAL_SECURITY_LEN, &smpvalMsg->securityLen) != SD_SUCCESS)
          return (ASN1E_DECODE_OTHER);
        break;

      case TAG_ASDU_SEQ:
        /* Decoded "Sequence of ASDU" constructor. Decode to end of this constructor.*/
        rc = decode_asdu_seq (aCtx, smpvalMsg);
        if (rc)
          return (rc);	/* return on first error	*/
        break;

      default:				/* Don't know how to decode.	*/
        /* NOTE: asn1r_chk_tag should catch this before we ever get here.*/
        CLNP_LOG_ERR1 ("Decode error: unexpected tag 0x%X.", tagnum);
        return (ASN1E_UNEXPECTED_TAG);
      }	/* End "switch (tagnum)"	*/
    }	/* end "while" loop	*/
  /* Decoded to end of APDU (or error).	*/
  return (rc);
  }

/************************************************************************/
/*			smpval_msg_decode_asn1				*/
/* Decode the ASN.1 part of a SMPVAL message.				*/
/************************************************************************/
ST_RET smpval_msg_decode_asn1 (SMPVAL_MSG *smpvalMsg, ST_UCHAR *apdu, ST_INT apdu_len)
  {
ASN1_DEC_CTXT aCtx;
ST_RET retcode = SD_SUCCESS;
ST_UINT16 tag;

  retcode = asn1r_decode_init (&aCtx, apdu, apdu_len);

  /* Get the first tag.	*/
  if (retcode == SD_SUCCESS)
    retcode = asn1r_get_tag (&aCtx, &tag);

  if (retcode == SD_SUCCESS)
    {
    if (tag==TAG_SAVPDU)
      {
      /* Constructor we expected. Call function to decode it.	*/
      retcode = decode_apdu (&aCtx, smpvalMsg);
      }
    else
      {
      CLNP_LOG_ERR1 ("Decode error: unexpected tag 0x%X.", tag);
      retcode = SD_FAILURE;
      }
    }

  return (retcode);
  }

/************************************************************************/
/*			smpval_msg_decode					*/
/* Completely decode SMPVAL message (including Ethertype header).		*/
/* This function allocates the SMPVAL_MSG structure, fills it in, and	*/
/* returns a pointer to it. It also fills in the ETYPE_INFO struct	*/
/* referenced by the "etypeInfo" argument.				*/
/* CRITICAL: The SMPVAL_MSG struct contains pointers to data in the 	*/
/*   SN_UNITDATA struct. Therefore, the SN_UNITDATA struct must not be	*/
/*   freed until after the SMPVAL_MSG struct is freed.			*/
/* NOTE: The Ethertype ID should be checked before calling this funct,	*/
/*   but it is checked again here to be sure this is SMPVAL message.	*/
/************************************************************************/
SMPVAL_MSG *smpval_msg_decode (
	SN_UNITDATA *sn_req,	/* message to decode			*/
	ST_INT edition,		/* 61850 edition (1 or 2).		*/
	ETYPE_INFO *etypeInfo)	/* where to store decoded Ethertype info*/
  {
  SMPVAL_MSG *smpvalMsg = NULL;
  ST_UCHAR *apdu;
  ST_INT apdu_len;

  /* Decode the Ethertype header.	*/
  /* Returns a ptr to the data. Fills in etypeInfo struct. */
  apdu = etype_hdr_decode(sn_req, etypeInfo, &apdu_len);
  if (apdu != NULL && etypeInfo->etypeID == ETYPE_TYPE_SV)
    {
    /* Decode complete SMPVAL message	*/
    /* Use calloc to allocate "clean" SMPVAL_MSG struct	*/
    smpvalMsg = (SMPVAL_MSG *) chk_calloc (1, sizeof (SMPVAL_MSG));
    smpvalMsg->edition = edition;	/* lower functions use this*/
    if (smpval_msg_decode_asn1 (smpvalMsg, apdu, apdu_len) != SD_SUCCESS)
      {
      chk_free (smpvalMsg);	/* free struct just allocated.	*/
      smpvalMsg = NULL;		/* error return	*/
      }
    }
  return (smpvalMsg);
  }

/************************************************************************/
/*			smpval_msg_free					*/
/* Free resources for allocated for recieved SMPVAL message.		*/
/************************************************************************/
ST_VOID smpval_msg_free (SMPVAL_MSG *smpvalMsg)
  {
  if (smpvalMsg->asduArray)
    chk_free (smpvalMsg->asduArray);
  chk_free (smpvalMsg);
  }

