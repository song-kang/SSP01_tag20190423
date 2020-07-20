/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,       */
/*	2002-2002, All Rights Reserved.					*/
/*									*/
/*                  PROPRIETARY AND CONFIDENTIAL                        */
/*									*/
/* MODULE NAME : gse_mgmt_dec.c						*/
/* PRODUCT(S)  : MMS-EASE Lite						*/
/*									*/
/* MODULE DESCRIPTION :  GSE MGMT decode functions.			*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :                            */
/*									*/
/* MODIFICATION LOG :                                                   */
/*  Date     Who   Rev                  Comments                        */
/* --------  ---  ------   -------------------------------------------  */
/* 11/27/06  JRB     02    Fix initializaton warning.			*/
/* 11/03/06  CRM     01    Created					*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "asn1r.h"
#include "mem_chk.h"
#include "asn1log.h"
#include "clnp_usr.h"
#include "clnp_sne.h"
#include "gse_mgmt.h"
#include "clnp_log.h"


#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif


/* ASN.1 tags at top level (APPLICATION)				*/
#define TAG_APP_PDU			(ASN1MASK_APP | ASN1MASK_CONSTR | 0)

/* ASN.1 tags at top level inside MGMT PDU				*/
#define TAG_STATE_ID			(ASN1MASK_CTX | 0)
#define TAG_MGMT_REQ			(ASN1MASK_CTX | ASN1MASK_CONSTR | 1)
#define TAG_MGMT_RSP			(ASN1MASK_CTX | ASN1MASK_CONSTR | 2)

/* ASN.1 tags inside MGMT REQ						*/
#define TAG_GO_REF_REQ			(ASN1MASK_CTX | ASN1MASK_CONSTR |1)
#define TAG_GOOSE_ELE_REQ		(ASN1MASK_CTX | ASN1MASK_CONSTR |2)
#define TAG_GS_REF_REQ			(ASN1MASK_CTX | ASN1MASK_CONSTR |3)
#define TAG_GSSE_DATA_OFF_REQ		(ASN1MASK_CTX | ASN1MASK_CONSTR |4)

#define TAG_REF_REQ_ID			(ASN1MASK_CTX | 0)
#define TAG_REF_REQ_OFF			(ASN1MASK_CTX | ASN1MASK_CONSTR |1)

#define TAG_ELE_REQ_ID			(ASN1MASK_CTX | 0)
#define TAG_ELE_REQ_REF			(ASN1MASK_CTX | ASN1MASK_CONSTR | 1)

#define TAG_REQ_OFF			(ASN1MASK_UNI | INT_CODE)
#define TAG_REQ_REF			(ASN1MASK_UNI | VISTR_CODE)

/* ASN.1 tags inside MGMT RSP						*/
#define TAG_GO_REF_RSP			(ASN1MASK_CTX | ASN1MASK_CONSTR |1)
#define TAG_GOOSE_ELE_RSP		(ASN1MASK_CTX | ASN1MASK_CONSTR |2)
#define TAG_GS_REF_RSP			(ASN1MASK_CTX | ASN1MASK_CONSTR |3)
#define TAG_GSSE_DATA_OFF_RSP		(ASN1MASK_CTX | ASN1MASK_CONSTR |4)


#define TAG_RSP_ID			(ASN1MASK_CTX | 0)
#define TAG_RSP_CONF_REV		(ASN1MASK_CTX | 1)
#define TAG_RSP_POSITIVE		(ASN1MASK_CTX | ASN1MASK_CONSTR | 2)
#define TAG_RSP_NEGATIVE		(ASN1MASK_CTX | 3)

#define TAG_RSP_DAT_SET			(ASN1MASK_CTX | 0)
#define TAG_RSP_RESULT			(ASN1MASK_CTX | ASN1MASK_CONSTR | 1)

#define TAG_RSP_OFF			(ASN1MASK_CTX | 0)
#define TAG_RSP_REF			(ASN1MASK_CTX | 1)
#define TAG_RSP_ERR			(ASN1MASK_CTX | 2)


/* Decode state defines (i.e. what are we waiting for next)		*/
/* CRITICAL: must be in this sequence.					*/
#define STATE_WAIT_APP_PDU	0	
#define STATE_WAIT_SID		1

#define STATE_WAIT_MGMT_REQ	2
#define STATE_WAIT_REF_REQ	3	
#define STATE_WAIT_REF_REQ_ID	4
#define STATE_WAIT_REF_REQ_OFF	5

#define STATE_WAIT_ELE_REQ	3	
#define STATE_WAIT_ELE_REQ_REF	5

#define STATE_WAIT_REQ_OFF	6
#define STATE_WAIT_REQ_REF	6

#define STATE_WAIT_MGMT_RSP	2
#define STATE_WAIT_REF_RSP	3	
#define STATE_WAIT_ELE_RSP	3	

#define STATE_WAIT_RSP_ID	4
#define STATE_WAIT_RSP_CONV_REV	5
#define STATE_WAIT_RSP_POS	6
#define STATE_WAIT_RSP_NEG	6
#define STATE_WAIT_RSP_DAT_SET	7
#define STATE_WAIT_RSP_RESULT	8
#define STATE_WAIT_RSP_OFF	9
#define STATE_WAIT_RSP_REF	9
#define STATE_WAIT_RSP_ERR	9

typedef struct
  {
  ST_INT state;		
  } GSE_MGMT_DEC_CTXT;


/************************************************************************/
/* END_gse_mgmt								*/
/* Done with parse. Clear asn1r_decode_done_fun function pointer.	*/
/************************************************************************/
static ST_VOID END_gse_mgmt (ASN1_DEC_CTXT *aCtx)
  {
  aCtx->asn1r_decode_done_fun = NULL;				/* replaces error funct		    */
  }

/************************************************************************/
/* decode_tag_ref_req							*/
/* Called for all 'context-specific' class tags at GSE  (lower) level.	*/
/************************************************************************/
static ST_VOID decode_tag_ref_req (ASN1_DEC_CTXT *aCtx, ST_UINT16 tagnum)
  {
GSE_MGMT_MSG *gseMgmt = (GSE_MGMT_MSG *) aCtx->usr_info[0];
GSE_MGMT_DEC_CTXT *gseMgmtCtx = (GSE_MGMT_DEC_CTXT *) aCtx->usr_info[1];


  /* This whole function is just one big switch to handle each possible id			    */
  switch (tagnum)
    {
    case TAG_REF_REQ_ID:
      if (gseMgmtCtx->state == STATE_WAIT_REF_REQ_ID)
        {
        if (asn1r_get_vstr (aCtx, gseMgmt->msg.refReq.ident) == SD_SUCCESS)
          gseMgmtCtx->state++;	
        else
          asn1r_set_dec_err (aCtx, ASN1E_DECODE_OTHER);
        }
      else
        asn1r_set_dec_err (aCtx, ASN1E_UNEXPECTED_TAG);		/* out of sequence		    */
      break;
    
    case TAG_REF_REQ_OFF:
      if (gseMgmtCtx->state == STATE_WAIT_REF_REQ_OFF)
        {
        if (aCtx->asn1r_constr_elmnt)				/* must be constructor		    */
          {
          aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = NULL;/* do nothing when constr done	    */
	  gseMgmt->msg.refReq.numIntegers = 0;
          gseMgmt->msg.refReq.offset = (ST_UINT32 *) chk_calloc (NUM_ELEMENTS, sizeof(ST_UINT32));
	  gseMgmtCtx->state++;
          }
        else
          asn1r_set_dec_err (aCtx, ASN1E_UNEXPECTED_FORM);	/* expected constructor		    */
        }
      else
        asn1r_set_dec_err (aCtx, ASN1E_UNEXPECTED_TAG);		/* out of sequence		    */
      break;

    case TAG_REQ_OFF:
      if (gseMgmtCtx->state == STATE_WAIT_REQ_OFF)
	{
	if (gseMgmt->msg.refReq.numIntegers && (gseMgmt->msg.refReq.numIntegers % NUM_ELEMENTS) == 0)
          gseMgmt->msg.refReq.offset = (ST_UINT32 *) chk_realloc (gseMgmt->msg.refReq.offset,
								 (gseMgmt->msg.refReq.numIntegers * sizeof(ST_UINT32)) + 
								 (NUM_ELEMENTS * sizeof(ST_UINT32)));
        
        if (asn1r_get_u32 (aCtx, &gseMgmt->msg.refReq.offset[gseMgmt->msg.refReq.numIntegers++]) != SD_SUCCESS)
          asn1r_set_dec_err (aCtx, ASN1E_DECODE_OTHER);
        }
      else
        asn1r_set_dec_err (aCtx, ASN1E_UNEXPECTED_TAG);		/* out of sequence		    */
      break;

    default:							/* Don't know how to decode.	    */
      CLNP_LOG_ERR1 ("Decode error: unexpected tag 0x%X.", tagnum);
      asn1r_set_dec_err (aCtx, ASN1E_UNEXPECTED_TAG);
      break;
    }								/* End "switch (tagnum)"	    */
  }

/************************************************************************/
/* decode_tag_ele_req							*/
/* Called for all 'context-specific' class tags at GSE  (lower) level.	*/
/************************************************************************/
static ST_VOID decode_tag_ele_req (ASN1_DEC_CTXT *aCtx, ST_UINT16 tagnum)
  {
GSE_MGMT_MSG *gseMgmt = (GSE_MGMT_MSG *) aCtx->usr_info[0];
GSE_MGMT_DEC_CTXT *gseMgmtCtx = (GSE_MGMT_DEC_CTXT *) aCtx->usr_info[1];


  /* This whole function is just one big switch to handle each possible id			    */
  switch (tagnum)
    {
    case TAG_ELE_REQ_ID:
      if (gseMgmtCtx->state == STATE_WAIT_REF_REQ_ID)
        {
        if (asn1r_get_vstr (aCtx, gseMgmt->msg.offReq.ident) == SD_SUCCESS)
          gseMgmtCtx->state++;	
        else
          asn1r_set_dec_err (aCtx, ASN1E_DECODE_OTHER);
        }
      else
        asn1r_set_dec_err (aCtx, ASN1E_UNEXPECTED_TAG);		/* out of sequence		    */
      break;
    
    case TAG_ELE_REQ_REF:
      if (gseMgmtCtx->state == STATE_WAIT_ELE_REQ_REF)
        {
        if (aCtx->asn1r_constr_elmnt)				/* must be constructor		    */
          {
          aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = NULL;/* do nothing when constr done	    */
	  gseMgmt->msg.offReq.numVStrings = 0;
          gseMgmt->msg.offReq.references = (ST_CHAR **) chk_calloc (NUM_ELEMENTS, sizeof(ST_CHAR *));
          gseMgmtCtx->state++;
          }
        else
          asn1r_set_dec_err (aCtx, ASN1E_UNEXPECTED_FORM);	/* expected constructor		    */
        }
      else
        asn1r_set_dec_err (aCtx, ASN1E_UNEXPECTED_TAG);		/* out of sequence		    */
      break;
    
    case TAG_REQ_REF:
      if (gseMgmtCtx->state == STATE_WAIT_REQ_REF)
	{
	if (gseMgmt->msg.offReq.numVStrings && (gseMgmt->msg.offReq.numVStrings % NUM_ELEMENTS) == 0)
	  gseMgmt->msg.offReq.references = (ST_CHAR **) chk_realloc (gseMgmt->msg.offReq.references, 
								    (gseMgmt->msg.offReq.numVStrings * sizeof (ST_CHAR *)) + 
								    (NUM_ELEMENTS * sizeof (ST_CHAR *)));
        gseMgmt->msg.offReq.references[gseMgmt->msg.offReq.numVStrings] = 
							 (ST_CHAR *) chk_calloc (aCtx->asn1r_elmnt_len+1, sizeof(ST_CHAR));
        
	if (asn1r_get_vstr (aCtx, gseMgmt->msg.offReq.references[gseMgmt->msg.offReq.numVStrings++]) != SD_SUCCESS)
          asn1r_set_dec_err (aCtx, ASN1E_DECODE_OTHER);
        }
      else
        asn1r_set_dec_err (aCtx, ASN1E_UNEXPECTED_TAG);		/* out of sequence		    */
      break;

    default:							/* Don't know how to decode.	    */
      CLNP_LOG_ERR1 ("Decode error: unexpected tag 0x%X.", tagnum);
      asn1r_set_dec_err (aCtx, ASN1E_UNEXPECTED_TAG);
      break;
    }								/* End "switch (tagnum)"	    */
  }

/************************************************************************/
/* decode_tag_mgmt_req							*/
/* Called for all 'context-specific' class tags at Mgmt (top) level.	*/
/************************************************************************/
static ST_VOID decode_tag_mgmt_req (ASN1_DEC_CTXT *aCtx, ST_UINT16 tagnum)
  {
GSE_MGMT_MSG *gseMgmt = (GSE_MGMT_MSG *) aCtx->usr_info[0];
GSE_MGMT_DEC_CTXT *gseMgmtCtx = (GSE_MGMT_DEC_CTXT *) aCtx->usr_info[1];


  /* This whole function is just one big switch to handle each possible id			    */
  switch (tagnum)
    {
    case TAG_GO_REF_REQ:
      if (gseMgmtCtx->state == STATE_WAIT_REF_REQ)
        {
        if (aCtx->asn1r_constr_elmnt)				/* must be constructor		    */
          {
	  gseMgmt->msg.refReq.stateID = gseMgmt->stateID;
	  gseMgmt->msgType = GSE_MSG_TYPE_GO_REF_REQ;
          aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = NULL;/* do nothing when constr done	    */
	  aCtx->asn1r_anytag_fun = decode_tag_ref_req;		/* decode all context-specific tags */
          gseMgmtCtx->state++;
          }
        else
          asn1r_set_dec_err (aCtx, ASN1E_UNEXPECTED_FORM);	/* expected constructor		    */
        }
      else
        asn1r_set_dec_err (aCtx, ASN1E_UNEXPECTED_TAG);		/* out of sequence		    */
      break;

    case TAG_GOOSE_ELE_REQ:
      if (gseMgmtCtx->state == STATE_WAIT_ELE_REQ)
        {
        if (aCtx->asn1r_constr_elmnt)				/* must be constructor		    */
          {
	  gseMgmt->msg.offReq.stateID = gseMgmt->stateID;
	  gseMgmt->msgType = GSE_MSG_TYPE_GOOSE_ELE_REQ;
          aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = NULL;/* do nothing when constr done	    */
	  aCtx->asn1r_anytag_fun = decode_tag_ele_req;		/* decode all context-specific tags */
          gseMgmtCtx->state++;
          }
        else
          asn1r_set_dec_err (aCtx, ASN1E_UNEXPECTED_FORM);	/* expected constructor		    */
        }
      else
        asn1r_set_dec_err (aCtx, ASN1E_UNEXPECTED_TAG);		/* out of sequence		    */
      break;

    case TAG_GS_REF_REQ:
      if (gseMgmtCtx->state == STATE_WAIT_REF_REQ)
        {
        if (aCtx->asn1r_constr_elmnt)				/* must be constructor		    */
          {
	  gseMgmt->msg.refReq.stateID = gseMgmt->stateID;
	  gseMgmt->msgType = GSE_MSG_TYPE_GS_REF_REQ;
          aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = NULL;/* do nothing when constr done	    */
	  aCtx->asn1r_anytag_fun = decode_tag_ref_req;		/* decode all context-specific tags */
          gseMgmtCtx->state++;
          }
        else
          asn1r_set_dec_err (aCtx, ASN1E_UNEXPECTED_FORM);	/* expected constructor		    */
        }
      else
        asn1r_set_dec_err (aCtx, ASN1E_UNEXPECTED_TAG);		/* out of sequence		    */
      break;

    case TAG_GSSE_DATA_OFF_REQ:
      if (gseMgmtCtx->state == STATE_WAIT_ELE_REQ)
        {
        if (aCtx->asn1r_constr_elmnt)				/* must be constructor		    */
          {
	  gseMgmt->msg.offReq.stateID = gseMgmt->stateID;
	  gseMgmt->msgType = GSE_MSG_TYPE_GSSE_DATA_OFF_REQ;
          aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = NULL;/* do nothing when constr done	    */
	  aCtx->asn1r_anytag_fun = decode_tag_ele_req;		/* decode all context-specific tags */
          gseMgmtCtx->state++;
          }
        else
          asn1r_set_dec_err (aCtx, ASN1E_UNEXPECTED_FORM);	/* expected constructor		    */
        }
      else
        asn1r_set_dec_err (aCtx, ASN1E_UNEXPECTED_TAG);		/* out of sequence		    */
      break;

    default:							/* Don't know how to decode.	    */
      CLNP_LOG_ERR1 ("Decode error: unrecognized 'context-specific' class tag %d.", tagnum);
      asn1r_set_dec_err (aCtx, ASN1E_UNEXPECTED_TAG);
      break;
    }								/* End "switch (tagnum)"	    */
  }
/************************************************************************/
/* decode_tag_off_req_result						*/
/* Called for all 'context-specific' class tags at GSE  (lower) level.	*/
/************************************************************************/
static ST_VOID decode_tag_off_req_result (ASN1_DEC_CTXT *aCtx, ST_UINT16 tagnum)
  {
GSE_MGMT_MSG *gseMgmt = (GSE_MGMT_MSG *) aCtx->usr_info[0];
GSE_MGMT_DEC_CTXT *gseMgmtCtx = (GSE_MGMT_DEC_CTXT *) aCtx->usr_info[1];


  if (gseMgmt->msg.offRsp.numResults && (gseMgmt->msg.offRsp.numResults % NUM_ELEMENTS) == 0)
    gseMgmt->msg.offRsp.result = (OFFSET_REQ_RESULTS *) chk_realloc (gseMgmt->msg.offRsp.result, 
					      (gseMgmt->msg.offRsp.numResults * sizeof (OFFSET_REQ_RESULTS)) + 
					      (NUM_ELEMENTS * sizeof (OFFSET_REQ_RESULTS)));

  /* This whole function is just one big switch to handle each possible id			    */
  switch (tagnum)
    {
    case TAG_RSP_OFF:
      if (gseMgmtCtx->state == STATE_WAIT_RSP_OFF)
	{
        if (asn1r_get_u32 (aCtx, 
		  &gseMgmt->msg.offRsp.result[gseMgmt->msg.offRsp.numResults++].offset) != SD_SUCCESS)
          asn1r_set_dec_err (aCtx, ASN1E_DECODE_OTHER);
        }
      else
        asn1r_set_dec_err (aCtx, ASN1E_UNEXPECTED_TAG);		/* out of sequence		    */
      break;

    case TAG_RSP_ERR:
      if (gseMgmtCtx->state == STATE_WAIT_RSP_ERR)
	{
        if (asn1r_get_u32 (aCtx, 
		  &gseMgmt->msg.offRsp.result[gseMgmt->msg.offRsp.numResults++].error) != SD_SUCCESS)
          asn1r_set_dec_err (aCtx, ASN1E_DECODE_OTHER);
        }
      else
        asn1r_set_dec_err (aCtx, ASN1E_UNEXPECTED_TAG);		/* out of sequence		    */
      break;

    default:							/* Don't know how to decode.	    */
      CLNP_LOG_ERR1 ("Decode error: unexpected tag 0x%X.", tagnum);
      asn1r_set_dec_err (aCtx, ASN1E_UNEXPECTED_TAG);
      break;
    }								/* End "switch (tagnum)"	    */
  }

/************************************************************************/
/* decode_tag_ref_req_result						*/
/* Called for all 'context-specific' class tags at GSE  (lower) level.	*/
/************************************************************************/
static ST_VOID decode_tag_ref_req_result (ASN1_DEC_CTXT *aCtx, ST_UINT16 tagnum)
  {
GSE_MGMT_MSG *gseMgmt = (GSE_MGMT_MSG *) aCtx->usr_info[0];
GSE_MGMT_DEC_CTXT *gseMgmtCtx = (GSE_MGMT_DEC_CTXT *) aCtx->usr_info[1];


  if (gseMgmt->msg.refRsp.numResults && (gseMgmt->msg.refRsp.numResults % NUM_ELEMENTS) == 0)
    gseMgmt->msg.refRsp.result = (REF_REQ_RESULTS *) chk_realloc (gseMgmt->msg.refRsp.result, 
					      (gseMgmt->msg.refRsp.numResults * sizeof (REF_REQ_RESULTS)) + 
					      (NUM_ELEMENTS * sizeof (REF_REQ_RESULTS)));

  /* This whole function is just one big switch to handle each possible id			    */
  switch (tagnum)
    {
    case TAG_RSP_REF:
      if (gseMgmtCtx->state == STATE_WAIT_RSP_REF)
	{
	if (asn1r_get_vstr (aCtx, 
		    gseMgmt->msg.refRsp.result[gseMgmt->msg.refRsp.numResults++].reference) != SD_SUCCESS)
          asn1r_set_dec_err (aCtx, ASN1E_DECODE_OTHER);
        }
      else
        asn1r_set_dec_err (aCtx, ASN1E_UNEXPECTED_TAG);		/* out of sequence		    */
      break;

    case TAG_RSP_ERR:
      if (gseMgmtCtx->state == STATE_WAIT_RSP_ERR)
	{
        if (asn1r_get_u32 (aCtx, 
		  &gseMgmt->msg.refRsp.result[gseMgmt->msg.refRsp.numResults++].error) != SD_SUCCESS)
          asn1r_set_dec_err (aCtx, ASN1E_DECODE_OTHER);
        }
      else
        asn1r_set_dec_err (aCtx, ASN1E_UNEXPECTED_TAG);		/* out of sequence		    */
      break;

    default:							/* Don't know how to decode.	    */
      CLNP_LOG_ERR1 ("Decode error: unexpected tag 0x%X.", tagnum);
      asn1r_set_dec_err (aCtx, ASN1E_UNEXPECTED_TAG);
      break;
    }								/* End "switch (tagnum)"	    */
  }

/************************************************************************/
/* decode_tag_off_rsp_pos							*/
/* Called for all 'context-specific' class tags at GSE  (lower) level.	*/
/************************************************************************/
static ST_VOID decode_tag_off_rsp_pos (ASN1_DEC_CTXT *aCtx, ST_UINT16 tagnum)
  {
GSE_MGMT_MSG *gseMgmt = (GSE_MGMT_MSG *) aCtx->usr_info[0];
GSE_MGMT_DEC_CTXT *gseMgmtCtx = (GSE_MGMT_DEC_CTXT *) aCtx->usr_info[1];


  /* This whole function is just one big switch to handle each possible id			    */
  switch (tagnum)
    {
    case TAG_RSP_DAT_SET:
      if (gseMgmtCtx->state == STATE_WAIT_RSP_DAT_SET)
	{
	if (asn1r_get_vstr (aCtx, gseMgmt->msg.offRsp.datSet) == SD_SUCCESS)
          gseMgmtCtx->state++;	
        else
          asn1r_set_dec_err (aCtx, ASN1E_DECODE_OTHER);
        }
      else
        asn1r_set_dec_err (aCtx, ASN1E_UNEXPECTED_TAG);		/* out of sequence		    */
      break;

    case TAG_RSP_RESULT:
      if (gseMgmtCtx->state == STATE_WAIT_RSP_RESULT)
	{
        if (aCtx->asn1r_constr_elmnt)				/* must be constructor		    */
          {
          aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = NULL;/* do nothing when constr done	    */
	  gseMgmt->msg.offRsp.numResults = 0;
          gseMgmt->msg.offRsp.result = (OFFSET_REQ_RESULTS *) chk_calloc (NUM_ELEMENTS, sizeof(OFFSET_REQ_RESULTS));
	  aCtx->asn1r_anytag_fun = decode_tag_off_req_result;	/* decode all context-specific tags */
	  gseMgmtCtx->state++;
          }
        else
          asn1r_set_dec_err (aCtx, ASN1E_UNEXPECTED_FORM);	/* expected constructor		    */
        }
      else
        asn1r_set_dec_err (aCtx, ASN1E_UNEXPECTED_TAG);		/* out of sequence		    */
      break;

    default:							/* Don't know how to decode.	    */
      CLNP_LOG_ERR1 ("Decode error: unexpected tag 0x%X.", tagnum);
      asn1r_set_dec_err (aCtx, ASN1E_UNEXPECTED_TAG);
      break;
    }								/* End "switch (tagnum)"	    */
  }

/************************************************************************/
/* decode_tag_ref_rsp_pos							*/
/* Called for all 'context-specific' class tags at GSE  (lower) level.	*/
/************************************************************************/
static ST_VOID decode_tag_ref_rsp_pos (ASN1_DEC_CTXT *aCtx, ST_UINT16 tagnum)
  {
GSE_MGMT_MSG *gseMgmt = (GSE_MGMT_MSG *) aCtx->usr_info[0];
GSE_MGMT_DEC_CTXT *gseMgmtCtx = (GSE_MGMT_DEC_CTXT *) aCtx->usr_info[1];


  /* This whole function is just one big switch to handle each possible id			    */
  switch (tagnum)
    {
    case TAG_RSP_DAT_SET:
      if (gseMgmtCtx->state == STATE_WAIT_RSP_DAT_SET)
	{
	if (asn1r_get_vstr (aCtx, gseMgmt->msg.refRsp.datSet) == SD_SUCCESS)
          gseMgmtCtx->state++;	
        else
          asn1r_set_dec_err (aCtx, ASN1E_DECODE_OTHER);
        }
      else
        asn1r_set_dec_err (aCtx, ASN1E_UNEXPECTED_TAG);		/* out of sequence		    */
      break;

    case TAG_RSP_RESULT:
      if (gseMgmtCtx->state == STATE_WAIT_RSP_RESULT)
	{
        if (aCtx->asn1r_constr_elmnt)				/* must be constructor		    */
          {
          aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = NULL;/* do nothing when constr done	    */
	  gseMgmt->msg.refRsp.numResults = 0;
          gseMgmt->msg.refRsp.result = (REF_REQ_RESULTS *) chk_calloc (NUM_ELEMENTS, sizeof(REF_REQ_RESULTS));
	  aCtx->asn1r_anytag_fun = decode_tag_ref_req_result;	/* decode all context-specific tags */
	  gseMgmtCtx->state++;
          }
        else
          asn1r_set_dec_err (aCtx, ASN1E_UNEXPECTED_FORM);	/* expected constructor		    */
        }
      else
        asn1r_set_dec_err (aCtx, ASN1E_UNEXPECTED_TAG);		/* out of sequence		    */
      break;

    default:							/* Don't know how to decode.	    */
      CLNP_LOG_ERR1 ("Decode error: unexpected tag 0x%X.", tagnum);
      asn1r_set_dec_err (aCtx, ASN1E_UNEXPECTED_TAG);
      break;
    }								/* End "switch (tagnum)"	    */
  }

/************************************************************************/
/* decode_tag_off_rsp							*/
/* Called for all 'context-specific' class tags at GSE  (lower) level.	*/
/************************************************************************/
static ST_VOID decode_tag_off_rsp (ASN1_DEC_CTXT *aCtx, ST_UINT16 tagnum)
  {
GSE_MGMT_MSG *gseMgmt = (GSE_MGMT_MSG *) aCtx->usr_info[0];
GSE_MGMT_DEC_CTXT *gseMgmtCtx = (GSE_MGMT_DEC_CTXT *) aCtx->usr_info[1];


  /* This whole function is just one big switch to handle each possible id			    */
  switch (tagnum)
    {
    case TAG_RSP_ID:
      if (gseMgmtCtx->state == STATE_WAIT_RSP_ID)
        {
        if (asn1r_get_vstr (aCtx, gseMgmt->msg.offReq.ident) == SD_SUCCESS)
          gseMgmtCtx->state++;	
        else
          asn1r_set_dec_err (aCtx, ASN1E_DECODE_OTHER);
        }
      else
        asn1r_set_dec_err (aCtx, ASN1E_UNEXPECTED_TAG);		/* out of sequence		    */
      break;

    case TAG_RSP_CONF_REV:
      if (gseMgmtCtx->state == STATE_WAIT_RSP_CONV_REV)
        {
        if (asn1r_get_u32 (aCtx, &gseMgmt->msg.offRsp.confRev) == SD_SUCCESS)
          gseMgmtCtx->state++;	
        else
          asn1r_set_dec_err (aCtx, ASN1E_DECODE_OTHER);
        }
      else
        asn1r_set_dec_err (aCtx, ASN1E_UNEXPECTED_TAG);		/* out of sequence		    */
      break;

    case TAG_RSP_POSITIVE:
      if (gseMgmtCtx->state == STATE_WAIT_RSP_POS)
        {
        if (aCtx->asn1r_constr_elmnt)				/* must be constructor		    */
          {
          aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = NULL;/* do nothing when constr done	    */
	  aCtx->asn1r_anytag_fun = decode_tag_off_rsp_pos;		/* decode all context-specific tags */
	  gseMgmtCtx->state++;
          }
        else
          asn1r_set_dec_err (aCtx, ASN1E_UNEXPECTED_FORM);	/* expected constructor		    */
        }
      else
        asn1r_set_dec_err (aCtx, ASN1E_UNEXPECTED_TAG);		/* out of sequence		    */
      break;

    case TAG_RSP_NEGATIVE:
      if (gseMgmtCtx->state == STATE_WAIT_RSP_NEG)
        {
        if (asn1r_get_u32 (aCtx, &gseMgmt->msg.glbErrRsp.glbError) != SD_SUCCESS)
          asn1r_set_dec_err (aCtx, ASN1E_DECODE_OTHER);
	else
	  gseMgmt->msgType = GSE_MSG_TYPE_GLOBAL_ERROR_RSP;
        }
      else
        asn1r_set_dec_err (aCtx, ASN1E_UNEXPECTED_TAG);		/* out of sequence		    */
      break;

    default:							/* Don't know how to decode.	    */
      CLNP_LOG_ERR1 ("Decode error: unexpected tag 0x%X.", tagnum);
      asn1r_set_dec_err (aCtx, ASN1E_UNEXPECTED_TAG);
      break;
    }								/* End "switch (tagnum)"	    */
  }

/************************************************************************/
/* decode_tag_ref_rsp							*/
/* Called for all 'context-specific' class tags at GSE  (lower) level.	*/
/************************************************************************/
static ST_VOID decode_tag_ref_rsp (ASN1_DEC_CTXT *aCtx, ST_UINT16 tagnum)
  {
GSE_MGMT_MSG *gseMgmt = (GSE_MGMT_MSG *) aCtx->usr_info[0];
GSE_MGMT_DEC_CTXT *gseMgmtCtx = (GSE_MGMT_DEC_CTXT *) aCtx->usr_info[1];


  /* This whole function is just one big switch to handle each possible id			    */
  switch (tagnum)
    {
    case TAG_RSP_ID:
      if (gseMgmtCtx->state == STATE_WAIT_RSP_ID)
        {
        if (asn1r_get_vstr (aCtx, gseMgmt->msg.refReq.ident) == SD_SUCCESS)
          gseMgmtCtx->state++;	
        else
          asn1r_set_dec_err (aCtx, ASN1E_DECODE_OTHER);
        }
      else
        asn1r_set_dec_err (aCtx, ASN1E_UNEXPECTED_TAG);		/* out of sequence		    */
      break;

    case TAG_RSP_CONF_REV:
      if (gseMgmtCtx->state == STATE_WAIT_RSP_CONV_REV)
        {
        if (asn1r_get_u32 (aCtx, &gseMgmt->msg.refRsp.confRev) == SD_SUCCESS)
          gseMgmtCtx->state++;	
        else
          asn1r_set_dec_err (aCtx, ASN1E_DECODE_OTHER);
        }
      else
        asn1r_set_dec_err (aCtx, ASN1E_UNEXPECTED_TAG);		/* out of sequence		    */
      break;

    case TAG_RSP_POSITIVE:
      if (gseMgmtCtx->state == STATE_WAIT_RSP_POS)
        {
        if (aCtx->asn1r_constr_elmnt)				/* must be constructor		    */
          {
          aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = NULL;/* do nothing when constr done	    */
	  aCtx->asn1r_anytag_fun = decode_tag_ref_rsp_pos;		/* decode all context-specific tags */
	  gseMgmtCtx->state++;
          }
        else
          asn1r_set_dec_err (aCtx, ASN1E_UNEXPECTED_FORM);	/* expected constructor		    */
        }
      else
        asn1r_set_dec_err (aCtx, ASN1E_UNEXPECTED_TAG);		/* out of sequence		    */
      break;

    case TAG_RSP_NEGATIVE:
      if (gseMgmtCtx->state == STATE_WAIT_RSP_NEG)
        {
        if (asn1r_get_u32 (aCtx, &gseMgmt->msg.glbErrRsp.glbError) != SD_SUCCESS)
          asn1r_set_dec_err (aCtx, ASN1E_DECODE_OTHER);
	else
	  gseMgmt->msgType = GSE_MSG_TYPE_GLOBAL_ERROR_RSP;
        }
      else
        asn1r_set_dec_err (aCtx, ASN1E_UNEXPECTED_TAG);		/* out of sequence		    */
      break;

    default:							/* Don't know how to decode.	    */
      CLNP_LOG_ERR1 ("Decode error: unexpected tag 0x%X.", tagnum);
      asn1r_set_dec_err (aCtx, ASN1E_UNEXPECTED_TAG);
      break;
    }								/* End "switch (tagnum)"	    */
  }
  
/************************************************************************/
/* decode_tag_mgmt_rsp							*/
/* Called for all 'context-specific' class tags at Mgmt (top) level.	*/
/************************************************************************/
static ST_VOID decode_tag_mgmt_rsp (ASN1_DEC_CTXT *aCtx, ST_UINT16 tagnum)
  {
GSE_MGMT_MSG *gseMgmt = (GSE_MGMT_MSG *) aCtx->usr_info[0];
GSE_MGMT_DEC_CTXT *gseMgmtCtx = (GSE_MGMT_DEC_CTXT *) aCtx->usr_info[1];


  /* This whole function is just one big switch to handle each possible id			    */
  switch (tagnum)
    {
    case TAG_GO_REF_RSP:
      if (gseMgmtCtx->state == STATE_WAIT_REF_RSP)
        {
        if (aCtx->asn1r_constr_elmnt)				/* must be constructor		    */
          {
	  gseMgmt->msg.refRsp.stateID = gseMgmt->stateID;
	  gseMgmt->msgType = GSE_MSG_TYPE_GO_REF_RSP;
          aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = NULL;/* do nothing when constr done	    */
	  aCtx->asn1r_anytag_fun = decode_tag_ref_rsp;		/* decode all context-specific tags */
          gseMgmtCtx->state++;
          }
        else
          asn1r_set_dec_err (aCtx, ASN1E_UNEXPECTED_FORM);	/* expected constructor		    */
        }
      else
        asn1r_set_dec_err (aCtx, ASN1E_UNEXPECTED_TAG);		/* out of sequence		    */
      break;

    case TAG_GOOSE_ELE_RSP:
      if (gseMgmtCtx->state == STATE_WAIT_ELE_RSP)
        {
        if (aCtx->asn1r_constr_elmnt)				/* must be constructor		    */
          {
	  gseMgmt->msg.offRsp.stateID = gseMgmt->stateID;
	  gseMgmt->msgType = GSE_MSG_TYPE_GOOSE_ELE_RSP;
          aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = NULL;/* do nothing when constr done	    */
	  aCtx->asn1r_anytag_fun = decode_tag_off_rsp;		/* decode all context-specific tags */
          gseMgmtCtx->state++;
          }
        else
          asn1r_set_dec_err (aCtx, ASN1E_UNEXPECTED_FORM);	/* expected constructor		    */
        }
      else
        asn1r_set_dec_err (aCtx, ASN1E_UNEXPECTED_TAG);		/* out of sequence		    */
      break;

    case TAG_GS_REF_RSP:
      if (gseMgmtCtx->state == STATE_WAIT_REF_RSP)
        {
        if (aCtx->asn1r_constr_elmnt)				/* must be constructor		    */
          {
	  gseMgmt->msg.refRsp.stateID = gseMgmt->stateID;
	  gseMgmt->msgType = GSE_MSG_TYPE_GS_REF_RSP;
          aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = NULL;/* do nothing when constr done	    */
	  aCtx->asn1r_anytag_fun = decode_tag_ref_rsp;		/* decode all context-specific tags */
          gseMgmtCtx->state++;
          }
        else
          asn1r_set_dec_err (aCtx, ASN1E_UNEXPECTED_FORM);	/* expected constructor		    */
        }
      else
        asn1r_set_dec_err (aCtx, ASN1E_UNEXPECTED_TAG);		/* out of sequence		    */
      break;

    case TAG_GSSE_DATA_OFF_RSP:
      if (gseMgmtCtx->state == STATE_WAIT_ELE_RSP)
        {
        if (aCtx->asn1r_constr_elmnt)				/* must be constructor		    */
          {
	  gseMgmt->msg.offRsp.stateID = gseMgmt->stateID;
	  gseMgmt->msgType = GSE_MSG_TYPE_GSSE_DATA_OFF_RSP;
          aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = NULL;/* do nothing when constr done	    */
	  aCtx->asn1r_anytag_fun = decode_tag_off_rsp;		/* decode all context-specific tags */
          gseMgmtCtx->state++;
          }
        else
          asn1r_set_dec_err (aCtx, ASN1E_UNEXPECTED_FORM);	/* expected constructor		    */
        }
      else
        asn1r_set_dec_err (aCtx, ASN1E_UNEXPECTED_TAG);		/* out of sequence		    */
      break;

    default:							/* Don't know how to decode.	    */
      CLNP_LOG_ERR1 ("Decode error: unrecognized 'context-specific' class tag %d.", tagnum);
      asn1r_set_dec_err (aCtx, ASN1E_UNEXPECTED_TAG);
      break;
    }								/* End "switch (tagnum)"	    */
  }

/************************************************************************/
/* decode_tag_mgmt_pdu							*/
/* Called for all 'context-specific' class tags at Mgmt (top) level.	*/
/************************************************************************/
static ST_VOID decode_tag_mgmt_pdu (ASN1_DEC_CTXT *aCtx, ST_UINT16 tagnum)
  {
GSE_MGMT_MSG *gseMgmt = (GSE_MGMT_MSG *) aCtx->usr_info[0];
GSE_MGMT_DEC_CTXT *gseMgmtCtx = (GSE_MGMT_DEC_CTXT *) aCtx->usr_info[1];


  /* This whole function is just one big switch to handle each possible id			    */
  switch (tagnum)
    {
    case TAG_STATE_ID:
      if (gseMgmtCtx->state == STATE_WAIT_SID)
        {
        if (asn1r_get_u32 (aCtx, &gseMgmt->stateID) == SD_SUCCESS)
          gseMgmtCtx->state++;
        else
          asn1r_set_dec_err (aCtx, ASN1E_DECODE_OTHER);
        }
      else
        asn1r_set_dec_err (aCtx, ASN1E_UNEXPECTED_TAG);		/* out of sequence		    */
      break;

    case TAG_MGMT_REQ:
      if (gseMgmtCtx->state == STATE_WAIT_MGMT_REQ)
        {
        if (aCtx->asn1r_constr_elmnt)				/* must be constructor		    */
          {
          aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = NULL;/* do nothing when constr done	    */	
	  aCtx->asn1r_anytag_fun = decode_tag_mgmt_req;		/* decode all context-specific tags */
          gseMgmtCtx->state++;
          }
        else
          asn1r_set_dec_err (aCtx, ASN1E_UNEXPECTED_FORM);	/* expected constructor		    */	
        }
      else
        asn1r_set_dec_err (aCtx, ASN1E_UNEXPECTED_TAG);		/* out of sequence		    */
      break;

    case TAG_MGMT_RSP:
      if (gseMgmtCtx->state == STATE_WAIT_MGMT_RSP)
        {
        if (aCtx->asn1r_constr_elmnt)				/* must be constructor		    */
          {
          aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = NULL;/* do nothing when constr done	    */	
	  aCtx->asn1r_anytag_fun = decode_tag_mgmt_rsp;		/* decode all context-specific tags */
          gseMgmtCtx->state++;
          }
        else
          asn1r_set_dec_err (aCtx, ASN1E_UNEXPECTED_FORM);	/* expected constructor		    */	
        }
      else
        asn1r_set_dec_err (aCtx, ASN1E_UNEXPECTED_TAG);		/* out of sequence		    */
      break;
    default:							/* Don't know how to decode.	    */
      CLNP_LOG_ERR1 ("Decode error: unrecognized 'context-specific' class tag %d.", tagnum);
      asn1r_set_dec_err (aCtx, ASN1E_UNEXPECTED_TAG);
      break;
    }								/* End "switch (tagnum)"	    */
  }


/************************************************************************/
/* decode_tag_app							*/
/* Called for all 'application' class tags.				*/
/************************************************************************/
static ST_VOID decode_tag_app (ASN1_DEC_CTXT *aCtx, ST_UINT16 tagnum)
  {
GSE_MGMT_DEC_CTXT *gseMgmtCtx = (GSE_MGMT_DEC_CTXT *) aCtx->usr_info[1];

  
  /* This whole function is just one big switch to handle each possible id			    */
  /* Only one id allowed in this GSE_MGMT protocol.						    */
  switch (tagnum)
    {
    case TAG_APP_PDU:
      if (gseMgmtCtx->state == STATE_WAIT_APP_PDU)
        {	  
        if (aCtx->asn1r_constr_elmnt)				/* must be constructor		    */		    
          {
          aCtx->asn1r_c_done_fun [aCtx->asn1r_msg_level] = END_gse_mgmt;
	  aCtx->asn1r_anytag_fun = decode_tag_mgmt_pdu;		/* decode all context-specific tags */
          gseMgmtCtx->state++;
          }
        else
          asn1r_set_dec_err (aCtx, ASN1E_UNEXPECTED_FORM);	/* expected constructor		    */
        }
      else
        asn1r_set_dec_err (aCtx, ASN1E_UNEXPECTED_TAG);		/* out of sequence		    */
      break;
 
    default:							/* Don't know how to decode.	    */
      CLNP_LOG_ERR1 ("Decode error: unrecognized 'application' class tag %d.", tagnum);
      asn1r_set_dec_err (aCtx, ASN1E_UNEXPECTED_TAG);
      break;
    }
  }


/************************************************************************/
/*			gse_mgmt_error					*/
/************************************************************************/
static ST_VOID gse_mgmt_error (ASN1_DEC_CTXT *aCtx, ST_RET err)
  {
  CLNP_LOG_ERR1 ("GSE_MGMT decode error = 0x%X", err);
  }

/************************************************************************/
/*			gse_mgmt_msg_decode_asn1				*/
/* Decode the ASN.1 part of a GSE_MGMT message.				*/
/************************************************************************/
static ST_RET gse_mgmt_msg_decode_asn1 (GSE_MGMT_MSG *gseMgmtMsg, ST_UCHAR *apdu, ST_INT apdu_len)
  {
ASN1_DEC_CTXT aCtx;
GSE_MGMT_DEC_CTXT gseMgmtCtx;
ST_RET retcode = SD_SUCCESS;

  memset (&aCtx, 0, sizeof(ASN1_DEC_CTXT));		/* start clean	*/
  memset (&gseMgmtCtx, 0, sizeof(GSE_MGMT_DEC_CTXT));	/* start clean	*/

  /* Save gseMgmtMsg and gseMgmtCtx in ASN1 Decode Context					    */
  aCtx.usr_info[0] = gseMgmtMsg;
  aCtx.usr_info[1] = &gseMgmtCtx;

  /* CLASS_METHOD decode method is much simpler and uses fewer callback functions.		    */
  aCtx.asn1r_decode_method = ASN1_ANYTAG_METHOD;
  aCtx.asn1r_anytag_fun = decode_tag_app;			/* decode all context-specific tags */
  aCtx.asn1r_p_id_fun = asn1r_class_err; 			/* no privates to start		    */

  /* If parse finishes before expected, this error funct is called.				    */
  /* When parse is done, this funct ptr is set to NULL.						    */
  aCtx.asn1r_decode_done_fun = asn1r_done_err; /* done function = error fun			    */

  /* this function called if any error detected during parse.					    */
  aCtx.asn1r_err_fun = gse_mgmt_error;

  /* Parse the ASN.1 msg.									    */
  asn1r_decode_asn1 (&aCtx, apdu, apdu_len);

  if (aCtx.asn1r_pdu_dec_err != NO_DECODE_ERR)
    retcode = SD_FAILURE;
  return (retcode);
  }

/************************************************************************/
/*			gse_mgmt_msg_decode				*/
/* Completely decode GSE_MGMT message (including Ethertype header).	*/
/* This function allocates the GSE_MGMT_MSG structure, fills it in, and	*/
/* returns a pointer to it. It also fills in the ETYPE_INFO struct	*/
/* referenced by the "etypeInfo" argument.				*/
/* CRITICAL: The GSE_MGMT_MSG struct contains pointers to data in the 	*/
/*   SN_UNITDATA struct. Therefore, the SN_UNITDATA struct must not be	*/
/*   freed until after the GSE_MGMT_MSG struct is freed.		*/
/* NOTE: The Ethertype ID should be checked before calling this funct,	*/
/*   but it is checked again here to be sure this is GSE_MGMT message.	*/
/************************************************************************/
GSE_MGMT_MSG *gse_mgmt_msg_decode (
	SN_UNITDATA *sn_req,					/* message to decode		    */
	ETYPE_INFO *etypeInfo)					/* store decoded Ethertype info	    */
  {
  GSE_MGMT_MSG *gseMgmt = NULL;
  ST_UCHAR *apdu;
  ST_INT apdu_len;

  
  /* Decode the Ethertype header.								    */
  /* Returns a ptr to the data. Fills in etypeInfo struct.					    */
  apdu = etype_hdr_decode(sn_req, etypeInfo, &apdu_len);
  
  if (apdu != NULL && etypeInfo->etypeID == ETYPE_TYPE_GSE)
   {
    /* Decode complete GSE_MGMT message								    */
    /* Use calloc to allocate "clean" GSE_MGMT_MSG struct					    */
    gseMgmt = (GSE_MGMT_MSG *) chk_calloc (1, sizeof (GSE_MGMT_MSG));
    
    if (gse_mgmt_msg_decode_asn1 (gseMgmt, apdu, apdu_len) != SD_SUCCESS)
      {
      chk_free (gseMgmt);					/* free struct just allocated.	    */
      gseMgmt = NULL;						/* error return			    */
      }
    }

  return (gseMgmt);
  }

/************************************************************************/
/*			gse_mgmt_msg_free				*/
/* Free resources for allocated for recieved GSE_MGMT message.		*/
/************************************************************************/
ST_VOID gse_mgmt_msg_free (GSE_MGMT_MSG *gseMgmt)
  {
ST_INT i;

  if (gseMgmt)
    {
    switch (gseMgmt->msgType)
      {
      case GSE_MSG_TYPE_GOOSE_ELE_REQ:
      case GSE_MSG_TYPE_GSSE_DATA_OFF_REQ:
	if (gseMgmt->msg.offReq.references)
	  {
	  for (i = 0; i < gseMgmt->msg.offReq.numVStrings; i++)
	    if (gseMgmt->msg.offReq.references[i])
	      chk_free (gseMgmt->msg.offReq.references[i]);

	  chk_free (gseMgmt->msg.offReq.references);
	  }
	break;

      case GSE_MSG_TYPE_GO_REF_REQ:
      case GSE_MSG_TYPE_GS_REF_REQ:
	if (gseMgmt->msg.refReq.offset)
	  chk_free (gseMgmt->msg.refReq.offset);
	break;

      case GSE_MSG_TYPE_GOOSE_ELE_RSP:
      case GSE_MSG_TYPE_GSSE_DATA_OFF_RSP:
	if (gseMgmt->msg.offRsp.result)
	  chk_free (gseMgmt->msg.offRsp.result);
	break;
      case GSE_MSG_TYPE_GO_REF_RSP:
      case GSE_MSG_TYPE_GS_REF_RSP:
	if (gseMgmt->msg.refRsp.result)
	  chk_free (gseMgmt->msg.refRsp.result);
	break;
      }
    chk_free (gseMgmt);
    }
  }
