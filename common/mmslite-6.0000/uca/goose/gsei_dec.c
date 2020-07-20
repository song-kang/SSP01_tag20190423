/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,       */
/*	2002-2002, All Rights Reserved.					*/
/*									*/
/*                  PROPRIETARY AND CONFIDENTIAL                        */
/*									*/
/* MODULE NAME : gsei_dec.c						*/
/* PRODUCT(S)  : MMS-EASE Lite						*/
/*									*/
/* MODULE DESCRIPTION :  IEC GOOSE decode functions.			*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :                            */
/*									*/
/* MODIFICATION LOG :                                                   */
/*  Date     Who   Rev                  Comments                        */
/* --------  ---  ------   -------------------------------------------  */
/* 11/23/11  KCR           Added casting for proper 64bit compile       */
/* 09/25/07  JRB     10    Use asn1r_get_u32 to decode confRev.		*/
/* 06/13/07  RKR     09    Ported to RHEL5                              */
/* 05/13/05  JRB     08    Fail decode if numDataEntries <= 0.		*/
/* 10/21/03  JRB     07    Del unused parse_next_..			*/
/* 06/20/03  JRB     06    Fix first ASN1 constructor: APP 1 IMPLICIT SEQ.*/
/* 01/09/03  JRB     05    Del unused proto.				*/
/* 12/11/02  JRB     04    Del mvl_acse.h include.			*/
/* 12/03/02  ASK     03    Changes for new IEC Goose encodings		*/
/* 10/14/02  JRB     02    Fix "allData" write past end of dataEntries arr*/
/*			   Call gse_iec_decode_done to clean up on err.	*/
/*			   Return error if numDataEntries incorrect.	*/
/* 02/11/02  JRB     01    Created					*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "asn1r.h"
#include "mem_chk.h"
#include "asn1log.h"
#include "clnp_usr.h"
#include "clnp_sne.h"
#include "clnp.h"	/* for clnpl_decode	*/
#include "goose.h"

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

static ST_VOID START_iec_goose (ASN1_DEC_CTXT *ac);
static ST_VOID decode_gcRef (ASN1_DEC_CTXT *ac);
static ST_VOID decode_timeToLive (ASN1_DEC_CTXT *ac);
static ST_VOID decode_dataSetRef (ASN1_DEC_CTXT *ac);
static ST_VOID decode_appId (ASN1_DEC_CTXT *ac);
static ST_VOID decode_utcTime (ASN1_DEC_CTXT *ac);

static ST_VOID decode_stNum (ASN1_DEC_CTXT *ac);
static ST_VOID decode_sqNum (ASN1_DEC_CTXT *ac);
static ST_VOID decode_test (ASN1_DEC_CTXT *ac);
static ST_VOID decode_confRev (ASN1_DEC_CTXT *ac);
static ST_VOID decode_needsCommissioning (ASN1_DEC_CTXT *ac);
static ST_VOID decode_numDataSetEntries (ASN1_DEC_CTXT *ac);
static ST_VOID END_data (ASN1_DEC_CTXT *ac);
static ST_VOID END_data2 (ASN1_DEC_CTXT *ac);
static ST_VOID START_allData (ASN1_DEC_CTXT *ac);
static ST_VOID END_allData (ASN1_DEC_CTXT *ac);
static ST_VOID END_iec_goose (ASN1_DEC_CTXT *ac);
static ST_VOID END_decode (ASN1_DEC_CTXT *ac);


/************************************************************************/
/*			gse_iec_hdr_decode				*/
/************************************************************************/
GSE_IEC_HDR *gse_iec_hdr_decode (ST_UCHAR *apdu, ST_INT apdu_len)
  {
ASN1_DEC_CTXT ac;
GSE_IEC_HDR *gseIecHdr;
  gseIecHdr = (GSE_IEC_HDR *) chk_calloc (1, sizeof (GSE_IEC_HDR));	/* allocate "clean" struct	*/

  /* Assume no APPID */
  gseIecHdr->appID_pres = SD_FALSE;
  gseIecHdr->appID = NULL;
  
  memset (&ac, 0, sizeof (ASN1_DEC_CTXT));
  ac.usr_info[0] = gseIecHdr;	/* Save in ASN1 Decode Context	*/
  ac.asn1r_decode_method = ASN1_TAG_METHOD;		/* Use asn1_tag method for decoding. 	*/
  ASN1R_TAG_ADD ((&ac), APP|CONSTR,1,START_iec_goose);

  ac.asn1r_decode_done_fun = asn1r_done_err; /* done function = error fun 	*/
  ac.asn1r_err_fun = NULL;		   /* set up error detected fun 	*/

  /* Parse the ASN.1 msg. */
  asn1r_decode_asn1 (&ac, apdu, apdu_len);
  if (ac.asn1r_pdu_dec_err != NO_DECODE_ERR)
    {
    gse_iec_decode_done (gseIecHdr);	/* this frees up hdr & dataEntries*/
    gseIecHdr = NULL; 			/* ERROR return       */
    }
  return (gseIecHdr);
  }
/************************************************************************/
/* START_iec_goose							*/
/************************************************************************/
static ST_VOID START_iec_goose (ASN1_DEC_CTXT *ac)
  {
  ASN1R_TAG_ADD (ac, CTX,0,decode_gcRef);
  ac->asn1r_c_done_fun [ac->asn1r_msg_level] = END_iec_goose;
  }
/************************************************************************/
/* decode_gcRef							*/
/************************************************************************/
static ST_VOID decode_gcRef (ASN1_DEC_CTXT *ac)
  {
GSE_IEC_HDR *gseIecHdr = (GSE_IEC_HDR *) ac->usr_info[0];
  /* WARNING: this code modifies the ASN.1 decode buffer. It moves the	*/
  /* string forward one byte (overwriting the length field),		*/
  /* and adds NULL at the end of the string.		 		*/

  gseIecHdr->gcRef = (ST_CHAR *)ac->asn1r_field_ptr - 1;	/* set ptr	*/
  asn1r_get_vstr (ac, (ST_CHAR *)ac->asn1r_field_ptr - 1);	/* WARNING: modifies decode buffer*/
  ASN1R_TAG_ADD (ac,CTX,1,decode_timeToLive);
  }
/************************************************************************/
/* decode_timeToLive							*/
/************************************************************************/
static ST_VOID decode_timeToLive (ASN1_DEC_CTXT *ac)
  {
GSE_IEC_HDR *gseIecHdr = (GSE_IEC_HDR *) ac->usr_info[0];
  asn1r_get_u32 (ac, &gseIecHdr->timeToLive);
  ASN1R_TAG_ADD (ac,CTX,2,decode_dataSetRef);
  }
/************************************************************************/
/* decode_dataSetRef							*/
/************************************************************************/
static ST_VOID decode_dataSetRef (ASN1_DEC_CTXT *ac)
  {
GSE_IEC_HDR *gseIecHdr = (GSE_IEC_HDR *) ac->usr_info[0];
  /* WARNING: this code modifies the ASN.1 decode buffer. It moves the	*/
  /* string forward one byte (overwriting the length field),		*/
  /* and adds NULL at the end of the string.		 		*/

  gseIecHdr->dataSetRef = (ST_CHAR *)ac->asn1r_field_ptr - 1;	/* set ptr	*/
  asn1r_get_vstr (ac, (ST_CHAR *)ac->asn1r_field_ptr - 1);	/* WARNING: modifies decode buffer*/

  ASN1R_TAG_ADD (ac,CTX,3,decode_appId);
  ASN1R_TAG_ADD (ac,CTX,4,decode_utcTime);
  }
/************************************************************************/
/* decode_appId							*/
/************************************************************************/
static ST_VOID decode_appId (ASN1_DEC_CTXT *ac)
  {
GSE_IEC_HDR *gseIecHdr = (GSE_IEC_HDR *) ac->usr_info[0];
  
  gseIecHdr->appID_pres = SD_TRUE;
  
  /* WARNING: this code modifies the ASN.1 decode buffer. It moves the	*/
  /* string forward one byte (overwriting the length field),		*/
  /* and adds NULL at the end of the string.		 		*/
  gseIecHdr->appID = (ST_CHAR *)ac->asn1r_field_ptr - 1;	/* set ptr	*/
  asn1r_get_vstr (ac, (ST_CHAR *)ac->asn1r_field_ptr - 1);	/* WARNING: modifies decode buffer*/

  ASN1R_TAG_ADD (ac,CTX,4,decode_utcTime);
  }
/************************************************************************/
/* decode_utcTime							*/
/************************************************************************/
static ST_VOID decode_utcTime (ASN1_DEC_CTXT *ac)
  {
GSE_IEC_HDR *gseIecHdr = (GSE_IEC_HDR *) ac->usr_info[0];
  asn1r_get_utc_time (ac, &gseIecHdr->utcTime);
  ASN1R_TAG_ADD (ac,CTX,5,decode_stNum);
  }
/************************************************************************/
/* decode_stNum								*/
/************************************************************************/
static ST_VOID decode_stNum (ASN1_DEC_CTXT *ac)
  {
GSE_IEC_HDR *gseIecHdr = (GSE_IEC_HDR *) ac->usr_info[0];
  asn1r_get_u32 (ac, &gseIecHdr->stNum);
  ASN1R_TAG_ADD (ac,CTX,6,decode_sqNum);
  }
/************************************************************************/
/* decode_sqNum								*/
/************************************************************************/
static ST_VOID decode_sqNum (ASN1_DEC_CTXT *ac)
  {
GSE_IEC_HDR *gseIecHdr = (GSE_IEC_HDR *) ac->usr_info[0];
  asn1r_get_u32 (ac, &gseIecHdr->sqNum);
  ASN1R_TAG_ADD (ac,CTX,7,decode_test);		/* OPTIONAL	*/
  ASN1R_TAG_ADD (ac,CTX,8,decode_confRev);
  }
/************************************************************************/
/* decode_test								*/
/************************************************************************/
static ST_VOID decode_test (ASN1_DEC_CTXT *ac)
  {
GSE_IEC_HDR *gseIecHdr = (GSE_IEC_HDR *) ac->usr_info[0];
  asn1r_get_bool (ac, &gseIecHdr->test);
  ASN1R_TAG_ADD (ac,CTX,8,decode_confRev);
  }
/************************************************************************/
/* decode_confRev [8]							*/
/************************************************************************/
static ST_VOID decode_confRev (ASN1_DEC_CTXT *ac)
  {
GSE_IEC_HDR *gseIecHdr = (GSE_IEC_HDR *) ac->usr_info[0];
  asn1r_get_u32 (ac, &gseIecHdr->confRev);
  ASN1R_TAG_ADD (ac,CTX,9,decode_needsCommissioning);	/* OPTIONAL	*/
  ASN1R_TAG_ADD (ac,CTX,10,decode_numDataSetEntries);
  }
/************************************************************************/
/* decode_needsCommissioning [9]					*/
/************************************************************************/
static ST_VOID decode_needsCommissioning (ASN1_DEC_CTXT *ac)
  {
GSE_IEC_HDR *gseIecHdr = (GSE_IEC_HDR *) ac->usr_info[0];
  asn1r_get_bool (ac, &gseIecHdr->needsCommissioning);
  ASN1R_TAG_ADD (ac,CTX,10,decode_numDataSetEntries);
  }
/************************************************************************/
/* decode_numDataSetEntries [10]					*/
/************************************************************************/
static ST_VOID decode_numDataSetEntries (ASN1_DEC_CTXT *ac)
  {
GSE_IEC_HDR *gseIecHdr = (GSE_IEC_HDR *) ac->usr_info[0];
  asn1r_get_i32 (ac, &gseIecHdr->numDataEntries);
  if (gseIecHdr->numDataEntries <= 0)
    {
    ALOG_ERR1 ("IEC GOOSE decode error: invalid num of data entries '%d'.", gseIecHdr->numDataEntries);
    asn1r_set_dec_err (ac, REQ_BAD_VALUE);
    }
  else
    {
    /* CRITICAL: allocate array of data entry structures now.		*/
    gseIecHdr->dataEntries = chk_calloc (gseIecHdr->numDataEntries,
                                         sizeof (GSE_IEC_DATA_ENTRY_RX));
    ASN1R_TAG_ADD (ac,CTX|CONSTR,11,START_allData);
    }
  }
/************************************************************************/
/*			END_data						*/
/************************************************************************/
static ST_VOID END_data (ASN1_DEC_CTXT *ac)
  {
ST_INT asn1Len;
ST_UINT8 *startPtr;
GSE_IEC_HDR *gseIecHdr = (GSE_IEC_HDR *) ac->usr_info[0];
  startPtr = gseIecHdr->dataEntries[gseIecHdr->tmpIndex].asn1Ptr;

  if (ac->asn1r_indef_flag)			/* indefinite length encoding	*/
    asn1Len = (int) (ac->asn1r_field_ptr - startPtr - 2);	/* -2 strips 00 00*/
  else
    asn1Len = (int) (ac->asn1r_field_ptr - startPtr);

  gseIecHdr->dataEntries[gseIecHdr->tmpIndex].asn1Len = asn1Len;
  }
/************************************************************************/
/* START_allData [11]							*/
/************************************************************************/
static ST_VOID START_allData (ASN1_DEC_CTXT *ac)
  {
GSE_IEC_HDR *gseIecHdr = (GSE_IEC_HDR *) ac->usr_info[0];
  /* Should now point to data. Save until decode done.		*/
  gseIecHdr->dataEntries[gseIecHdr->tmpIndex].asn1Ptr = ac->asn1r_field_ptr;

  asn1r_parse_next (ac, END_data2);
  ac->asn1r_c_done_fun [ac->asn1r_msg_level] = END_allData;
  }
/************************************************************************/
/*			END_data2					*/
/************************************************************************/
static ST_VOID END_data2 (ASN1_DEC_CTXT *ac)
  {
GSE_IEC_HDR *gseIecHdr = (GSE_IEC_HDR *) ac->usr_info[0];
  END_data (ac);
  /* Set up for next data entry.	*/
  if (++gseIecHdr->tmpIndex < gseIecHdr->numDataEntries)
    {
    gseIecHdr->dataEntries[gseIecHdr->tmpIndex].asn1Ptr = ac->asn1r_field_ptr;
    asn1r_parse_next (ac, END_data2);
    }	/* else must be done. END_allData will be called next.	*/
  }
/************************************************************************/
/* END_allData								*/
/************************************************************************/
static ST_VOID END_allData (ASN1_DEC_CTXT *ac)
  {
GSE_IEC_HDR *gseIecHdr = (GSE_IEC_HDR *) ac->usr_info[0];
  if (gseIecHdr->tmpIndex != gseIecHdr->numDataEntries)
    {
    ALOG_ERR0 ("IEC GOOSE decode error: num of data entries does not match value in header");
    asn1r_set_dec_err (ac, REQ_BAD_VALUE);    
    }
  return;
  }
/************************************************************************/
/* END_iec_goose						*/
/************************************************************************/
static ST_VOID END_iec_goose (ASN1_DEC_CTXT *ac)
  {
  ac->asn1r_decode_done_fun = END_decode;
  }
/************************************************************************/
/* END_decode							*/
/************************************************************************/
static ST_VOID END_decode (ASN1_DEC_CTXT *ac)
  {
  return;
  }

/************************************************************************/
/*			gse_iec_decode_done				*/
/************************************************************************/
ST_RET gse_iec_decode_done (GSE_IEC_HDR *gseIecHdr)
  {
  if (gseIecHdr == NULL)
    return (SD_FAILURE);
  if (gseIecHdr->dataEntries)			/* If dataEntries allocated,	*/
    chk_free (gseIecHdr->dataEntries);	/* then free them.		*/
  chk_free (gseIecHdr);
  return (SD_SUCCESS);
  }
