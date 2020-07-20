/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	2009-2009, All Rights Reserved					*/
/*									*/
/*			PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : copp_dec2.c						*/
/* PRODUCT(S)  : MOSI Stack						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	COPP decode functions using new ASN.1 decode scheme		*/
/*	(asn1r_decode_init, asn1r_get_tag).				*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*			copp_dec_pdv_entry				*/
/*			copp_dec_user_data_only				*/
/*			copp_dec_cp					*/
/*			copp_dec_cpa					*/
/*			copp_dec_cpr					*/
/*			copp_dec_arx					*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 08/02/10  JRB	   copp_dec_pc_def_list_entry: init pci=0.	*/
/* 11/17/09  JRB    01     Created.					*/
/************************************************************************/

/************************************************************************/
/* Common COPP protocol abbreviations:					*/
/*	PCI:	Presentation-context-identifier				*/
/*	ASN:	Abstract-syntax-name					*/
/*	TSN:	Transfer-syntax-name					*/
/************************************************************************/
/* CRITICAL: any function that parses a constructor must check for	*/
/*    tagnum==0 which indicates the constructor is complete.		*/
/************************************************************************/
#include "glbtypes.h"
#include "sysincs.h"
#include "mem_chk.h"
#include "asn1r.h"
#include "acse2.h"

#include "copp.h"
#include "cosp_usr.h"

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/* All code dealing with "*_psel" (here and in other modules) assumes  	*/
/* length fits in first byte of PSEL.					*/
/* NOTE: Longer PSELs should never be needed, and if another app sends	*/
/*       one, this code gracefully refuses the connection.		*/
#if MAX_PSEL_LEN > 127
#error This code will not work because (MAX_PSEL_LEN > 127). Length must fit in first byte of psel.
#endif

/************************************************************************/
/************************************************************************/
/* NOTE: We only support "Fully-encoded" User-data (not Simply-encoded).*/
#define TAG_USER_DATA_FULLY	(ASN1MASK_APP | ASN1MASK_CONSTR | 1)
#define TAG_PDV			(ASN1MASK_CTX | ASN1MASK_CONSTR | 0)

/* UNIVERSAL tags used in multiple places.				*/
#define TAG_SET			(ASN1MASK_UNI | ASN1MASK_CONSTR | SET_CODE)
#define TAG_SEQUENCE		(ASN1MASK_UNI | ASN1MASK_CONSTR | SEQ_CODE)
#define TAG_PCI			(ASN1MASK_UNI | INT_CODE)
#define TAG_TSN			(ASN1MASK_UNI | OBJ_ID_CODE)
#define TAG_ASN			(ASN1MASK_UNI | OBJ_ID_CODE)

/* CP tags	*/
/*      TAG_SET			also used	*/
#define TAG_MODE_CSTR		(ASN1MASK_CTX | ASN1MASK_CONSTR | 0)
#define TAG_MODE		(ASN1MASK_CTX | 0)
#define TAG_CP_SEQ		(ASN1MASK_CTX | ASN1MASK_CONSTR | 2)

#define TAG_VERSION		(ASN1MASK_CTX | 0)       
#define TAG_CALLING_PSEL	(ASN1MASK_CTX | 1)     
#define TAG_CALLED_PSEL		(ASN1MASK_CTX | 2)      
#define TAG_PC_DEF_LIST		(ASN1MASK_CTX | ASN1MASK_CONSTR | 4)
#define TAG_DEF_CTXT_NAME	(ASN1MASK_CTX | ASN1MASK_CONSTR | 6)
#define TAG_PRES_REQ		(ASN1MASK_CTX | 8)        
#define TAG_USER_SESS_REQ	(ASN1MASK_CTX | 9)
/*      TAG_USER_DATA_FULLY	also used	*/

/* CPA tags	*/
#define TAG_CPA_SEQ		(ASN1MASK_CTX | ASN1MASK_CONSTR | 2)
/*      TAG_VERSION		also used	*/
#define TAG_RESPONDING_PSEL	(ASN1MASK_CTX | 3)      
#define TAG_PC_DEF_RESULT_LIST	(ASN1MASK_CTX | ASN1MASK_CONSTR | 5)
/*      TAG_PRES_REQ		also used	*/
/*      TAG_USER_SESS_REQ	also used	*/
/*      TAG_USER_DATA_FULLY	also used	*/

/* Result-list tags (for pc_def_result_list)*/
#define TAG_RESULT_LIST_RESULT	(ASN1MASK_CTX | 0)
#define TAG_RESULT_LIST_TSN	(ASN1MASK_CTX | 1)
#define TAG_RESULT_LIST_REASON	(ASN1MASK_CTX | 2)

/* CPR tags	*/
/*      TAG_VERSION		also used	*/
/*      TAG_RESPONDING_PSEL	also used	*/
/*      TAG_PC_DEF_RESULT_LIST	also used	*/
#define TAG_DEFAULT_CONTEXT_RESULT	(ASN1MASK_CTX | 7)
#define TAG_PROVIDER_REASON		(ASN1MASK_CTX | 10)
/*      TAG_USER_DATA_FULLY	also used	*/

/* ARX tags (User or Provider Abort)	*/
#define TAG_ARU			(ASN1MASK_CTX | ASN1MASK_CONSTR | 0)
#define TAG_ARP			(ASN1MASK_UNI | ASN1MASK_CONSTR | SEQ_CODE)

/* ARU tags (User Abort)	*/
#define TAG_ARU_PC_IDENTIFIER_LIST	(ASN1MASK_CTX | ASN1MASK_CONSTR | 0)
/*      TAG_USER_DATA_FULLY	also used	*/

/* ARP tags (Provider Abort)	*/
#define TAG_ARP_ABORT_REASON	(ASN1MASK_CTX | 0)
#define TAG_ARP_EVENT_ID	(ASN1MASK_CTX | 1)

/*======================================================================*/
/*======================================================================*/
/*		Common decode functions					*/
/*======================================================================*/
/*======================================================================*/

/************************************************************************/
/*			copp_dec_pdv_entry				*/
/* NOTE: If we ever receive more than 1 pdv, we could change this to	*/
/*       just ignore ones that don't match "pdv_info->expected_pci".	*/
/************************************************************************/
ST_RET copp_dec_pdv_entry (
	ASN1_DEC_CTXT *aCtx,	/* ASN.1 decode context		*/
	PDV_INFO *pdv_info)	/* in/out: decoded PDV data	*/
				/* set pdv_info->expected_pci before calling*/
  {
ST_RET rc = SD_SUCCESS;
ST_UINT16 tagnum;
MMS_OBJ_ID tsn;		/* TSN rcvd	*/
/* This TAG array created from info in Presentation spec.	*/
TAG_INFO tag_arr [] = {
	{TAG_TSN,	TAG_OPTIONAL},
	{TAG_PCI,	TAG_MANDATORY},
	{TAG_PDV,	TAG_MANDATORY} };
int tag_idx = 0;	/* index into tag_arr array (next to look for)	*/

  /* Loop decoding tags until error or end of this constructor.*/
  while (rc == SD_SUCCESS	/* no error on last loop iteration	*/
         && (rc = asn1r_get_tag (aCtx, &tagnum)) == SD_SUCCESS
         && tagnum != 0)	/* 0 indicates end of constructor	*/
    {
    rc = asn1r_chk_tag (tagnum, tag_arr, (sizeof(tag_arr)/sizeof(TAG_INFO)), &tag_idx);
    if (rc)
      {
      break;	/* error message already logged	*/
      }

    /* Handle each possible TAG.	*/
    switch (tagnum)
      {
      case TAG_TSN:
        rc = asn1r_get_objid (aCtx, tsn.comps,&tsn.num_comps);
        if (!rc && asn1_objidcmp (&tsn, &asn1_obj_id) != 0)
          rc = ASN1E_INVALID_BITSTR;	/* TSN better be ASN1	*/
        break;
  
      case TAG_PCI:
        rc = asn1r_get_i8 (aCtx, &pdv_info->pci);
        if (!rc && pdv_info->pci != pdv_info->expected_pci)
          rc = ASN1E_INVALID_BITSTR;	/* Not the PCI we're expecting.	*/
        break;

      case TAG_PDV:
        /* Process PDV constructor. Do NOT want to waste time decoding	*/
        /* it now. Just save the pointer to the PDV and length.		*/
        rc = asn1r_get_contents (aCtx, &pdv_info->pdv_data_ptr, &pdv_info->pdv_data_len);
        break;

      default:				/* Unexpected tag	*/
        /* NOTE: Should NEVER get here, "asn1r_chk_tag" catches this.	*/
        rc = ASN1E_UNEXPECTED_TAG;
        break;
      }	/* end "switch (tagnum)"	*/
    }	/* end "while" loop	*/
  return (rc);
  }

/************************************************************************/
/*			copp_dec_pdv_list				*/
/************************************************************************/
ST_RET copp_dec_pdv_list (ASN1_DEC_CTXT *aCtx,
	PDV_INFO *pdv_info)	/* in/out: PDV data	*/
				/* set pdv_info->expected_pci before calling*/
  {
ST_RET rc = SD_SUCCESS;
ST_UINT16 tagnum;

  /* Loop decoding tags until error or end of this constructor.*/
  while (rc == SD_SUCCESS	/* no error on last loop iteration	*/
         && (rc = asn1r_get_tag (aCtx, &tagnum)) == SD_SUCCESS
         && tagnum != 0)	/* 0 indicates end of constructor	*/
    {
    /* Handle each possible TAG.	*/
    switch (tagnum)
      {
      case TAG_SEQUENCE:
        rc = copp_dec_pdv_entry (aCtx, pdv_info);
        break;
  
      default:				/* Unexpected tag	*/
        COPP_LOG_ERR1 ("Decode error: unexpected tag 0x%X.", tagnum);
        rc = ASN1E_UNEXPECTED_TAG;
        break;
      }	/* end "switch (tagnum)"	*/
    }	/* end "while" loop	*/
  return (rc);
  }

/************************************************************************/
/*			copp_dec_user_data_only				*/
/* Decode User Data if it's the top level constructor (not nested).	*/
/************************************************************************/
ST_RET copp_dec_user_data_only (
	ST_UCHAR *pdu_ptr,		/* PDU pointer			*/
	ST_INT pdu_len,			/* PDU length			*/
	ST_INT8 expected_pci,		/* expected PCI			*/
	PDV_INFO *pdv_info)		/* out: decoded PDV data	*/
  {
ASN1_DEC_CTXT aCtx;
ST_RET rc = SD_SUCCESS;
ST_UINT16 tagnum;

  /* Initialize the pdv_info structure.	*/
  memset (pdv_info, 0, sizeof (PDV_INFO));	/* CRITICAL: start clean*/

  pdv_info->expected_pci = expected_pci;

  /* Initialize the ASN.1 decode context.	*/
  rc = asn1r_decode_init (&aCtx, pdu_ptr, pdu_len);

  if (rc == SD_SUCCESS)
    {
    /* Get the first tag. MUST BE "User-data".	*/
    rc = asn1r_get_tag (&aCtx, &tagnum);
    if (!rc)
      {
      /* Handle each possible TAG.	*/
      switch (tagnum)
        {
        case TAG_USER_DATA_FULLY:
          rc = copp_dec_pdv_list (&aCtx, pdv_info);
          break;
  
        default:				/* Unexpected tag	*/
          COPP_LOG_ERR1 ("Decode error: unexpected tag 0x%X.", tagnum);
          rc = ASN1E_UNEXPECTED_TAG;
          break;
        }	/* end "switch (tagnum)"	*/
      }
    }
  return (rc);
  }

/************************************************************************/
/*			copp_dec_mode					*/
/************************************************************************/
ST_RET copp_dec_mode (ASN1_DEC_CTXT *aCtx,
	ST_INT8 *mode)	/* in/out: decoded data	*/
  {
ST_RET rc = SD_SUCCESS;
ST_UINT16 tagnum;

  /* Loop decoding tags until error or end of this constructor.*/
  while (rc == SD_SUCCESS	/* no error on last loop iteration	*/
         && (rc = asn1r_get_tag (aCtx, &tagnum)) == SD_SUCCESS
         && tagnum != 0)	/* 0 indicates end of constructor	*/
    {
    /* Handle each possible TAG.	*/
    switch (tagnum)
      {
      case TAG_MODE:
        rc = asn1r_get_i8 (aCtx, mode);
        if (!rc && *mode != 1)	/* Not "normal" mode. Give up.	*/
          rc = ASN1E_INVALID_BOOLEAN;
        break;
  
      default:				/* Unexpected tag	*/
        COPP_LOG_ERR1 ("Decode error: unexpected tag 0x%X.", tagnum);
        rc = ASN1E_UNEXPECTED_TAG;
        break;
      }	/* end "switch (tagnum)"	*/

    }	/* end "while" loop	*/
  return (rc);
  }

/************************************************************************/
/*			copp_dec_pc_def_result_list_entry		*/
/************************************************************************/
ST_RET copp_dec_pc_def_result_list_entry (ASN1_DEC_CTXT *aCtx,
	COPP_DEC_PC_DEF_RESULT_LIST *pc_def_result_list)	/* in/out: decoded data	*/
  {
ST_RET rc = SD_SUCCESS;
ST_UINT16 tagnum;
ST_INT8 result;			/* result rcvd	*/
MMS_OBJ_ID tsn;			/* TSN rcvd	*/
ST_INT8 result_list_prov_reas;	/* reason rcvd	*/

/* This TAG array created from info in Presentation spec.	*/
TAG_INFO tag_arr [] = {
	{TAG_RESULT_LIST_RESULT,	TAG_MANDATORY},
	{TAG_RESULT_LIST_TSN,		TAG_OPTIONAL},
	{TAG_RESULT_LIST_REASON,	TAG_OPTIONAL} };
int tag_idx = 0;	/* index into tag_arr array (next to look for)	*/

  /* Loop decoding tags until error or end of this constructor.*/
  while (rc == SD_SUCCESS	/* no error on last loop iteration	*/
         && (rc = asn1r_get_tag (aCtx, &tagnum)) == SD_SUCCESS
         && tagnum != 0)	/* 0 indicates end of constructor	*/
    {
    rc = asn1r_chk_tag (tagnum, tag_arr, (sizeof(tag_arr)/sizeof(TAG_INFO)), &tag_idx);
    if (rc)
      {
      break;	/* error message already logged	*/
      }

    /* Handle each possible TAG.	*/
    /* NOTE: usually more than one list entry. Don't want to save the	*/
    /*       decoded data, so just set pc_def_result_list->bad_pc_deflist*/
    /*       on any error.						*/
    switch (tagnum)
      {
      case TAG_RESULT_LIST_RESULT:
        rc = asn1r_get_i8 (aCtx, &result);
        if (!rc && result != 0)
          {	/* P-Context rejected.	*/
          pc_def_result_list->bad_pc_deflist = SD_TRUE;
          }
        break;
  
      case TAG_RESULT_LIST_TSN:
        rc = asn1r_get_objid (aCtx, tsn.comps,&tsn.num_comps);
        if (!rc && asn1_objidcmp (&tsn, (MMS_OBJ_ID *) &asn1_obj_id) != 0)
          {	/* TSN is not ASN1. Not allowed.	*/
          pc_def_result_list->bad_pc_deflist = SD_TRUE;
          }
        break;

      case TAG_RESULT_LIST_REASON:
        /* NOTE: Decode but don't save value, because no real use for it.*/
        rc = asn1r_get_i8 (aCtx, &result_list_prov_reas);
        break;

      default:				/* Unexpected tag	*/
        /* NOTE: Should NEVER get here, "asn1r_chk_tag" catches this.	*/
        rc = ASN1E_UNEXPECTED_TAG;
        break;
      }	/* end "switch (tagnum)"	*/
    }	/* end "while" loop	*/
  pc_def_result_list->num_result++;	/* increment count of pc_def_result*/
  return (rc);
  }

/************************************************************************/
/*			copp_dec_pc_def_result_list			*/
/************************************************************************/
ST_RET copp_dec_pc_def_result_list (ASN1_DEC_CTXT *aCtx,
	COPP_DEC_PC_DEF_RESULT_LIST *pc_def_result_list)	/* in/out: decoded data	*/
  {
ST_RET rc = SD_SUCCESS;
ST_UINT16 tagnum;

  /* Loop decoding tags until error or end of this constructor.*/
  while (rc == SD_SUCCESS	/* no error on last loop iteration	*/
         && (rc = asn1r_get_tag (aCtx, &tagnum)) == SD_SUCCESS
         && tagnum != 0)	/* 0 indicates end of constructor	*/
    {
    /* Handle each possible TAG.	*/
    switch (tagnum)
      {
      case TAG_SEQUENCE:
        /* decode one entry on list	*/
        rc = copp_dec_pc_def_result_list_entry (aCtx, pc_def_result_list);
        break;
  
      default:				/* Unexpected tag	*/
        COPP_LOG_ERR1 ("Decode error: unexpected tag 0x%X.", tagnum);
        rc = ASN1E_UNEXPECTED_TAG;
        break;
      }	/* end "switch (tagnum)"	*/
    }	/* end "while" loop	*/
  return (rc);
  }

/*======================================================================*/
/*======================================================================*/
/*		CP decode functions					*/
/*======================================================================*/
/*======================================================================*/

/************************************************************************/
/*			copp_dec_tsn_seq				*/
/* NOTE: This is only used during CP decode.				*/
/************************************************************************/
ST_RET copp_dec_tsn_seq (ASN1_DEC_CTXT *aCtx,
	COPP_DEC_CP *cp_info)	/* in/out: decoded data	*/
  {
ST_RET rc = SD_SUCCESS;
ST_UINT16 tagnum;
MMS_OBJ_ID tsn;		/* TSN rcvd	*/

  /* Loop decoding tags until error or end of this constructor.*/
  while (rc == SD_SUCCESS	/* no error on last loop iteration	*/
         && (rc = asn1r_get_tag (aCtx, &tagnum)) == SD_SUCCESS
         && tagnum != 0)	/* 0 indicates end of constructor	*/
    {
    switch (tagnum)
      {
      case TAG_TSN:
        rc = asn1r_get_objid (aCtx, tsn.comps,&tsn.num_comps);
        if (!rc && asn1_objidcmp (&tsn, (MMS_OBJ_ID *) &asn1_obj_id) != 0)
          cp_info->bad_pc_deflist = SD_TRUE;	/* TSN better be ASN1	*/
        break;
  
      default:				/* Unexpected tag	*/
        COPP_LOG_ERR1 ("Decode error: unexpected tag 0x%X.", tagnum);
        rc = ASN1E_UNEXPECTED_TAG;
        break;
      }	/* end "switch (tagnum)"	*/
    }	/* end "while" loop	*/
  return (rc);
  }

/************************************************************************/
/*			copp_dec_pc_def_list_entry			*/
/* NOTE: This is only used during CP decode.				*/
/************************************************************************/
ST_RET copp_dec_pc_def_list_entry (ASN1_DEC_CTXT *aCtx,
	COPP_DEC_CP *cp_info)	/* in/out: decoded data	*/
  {
ST_RET rc = SD_SUCCESS;
ST_UINT16 tagnum;
MMS_OBJ_ID asn;		/* ASN received (must be ACSE or MMS)	*/
ST_INT8 pci = 0;	/* PCI rcvd	*/
/* This TAG array created from info in Presentation spec.	*/
TAG_INFO tag_arr [] = {
	{TAG_PCI,	TAG_MANDATORY},
	{TAG_ASN,	TAG_MANDATORY},
	{TAG_SEQUENCE,	TAG_MANDATORY} };
int tag_idx = 0;	/* index into tag_arr array (next to look for)	*/

  /* Loop decoding tags until error or end of this constructor.*/
  while (rc == SD_SUCCESS	/* no error on last loop iteration	*/
         && (rc = asn1r_get_tag (aCtx, &tagnum)) == SD_SUCCESS
         && tagnum != 0)	/* 0 indicates end of constructor	*/
    {
    rc = asn1r_chk_tag (tagnum, tag_arr, (sizeof(tag_arr)/sizeof(TAG_INFO)), &tag_idx);
    if (rc)
      {
      break;	/* error message already logged	*/
      }

    /* Handle each possible TAG.	*/
    switch (tagnum)
      {
      case TAG_PCI:
        rc = asn1r_get_i8 (aCtx, &pci);
        break;
  
      case TAG_ASN:
        if (asn1r_get_objid (aCtx, asn.comps,&asn.num_comps))
          {
          return (ASN1E_INVALID_BOOLEAN);
          }       
        if (asn1_objidcmp (&asn, (MMS_OBJ_ID *) &mms_obj_id) == 0)
          cp_info->pcimms = pci;
        else if (asn1_objidcmp (&asn, (MMS_OBJ_ID *) &acse_obj_id) == 0)
          cp_info->pciacse = pci;
        else
          {	/* Unrecognized Abstract Syntax.  Must be "ACSE" or "ISO MMS".	*/
          cp_info->bad_pc_deflist = SD_TRUE;
          }
        break;

      case TAG_SEQUENCE:	/* this is beginning of TSN seq	*/
        /* decode one entry on list	*/
        rc = copp_dec_tsn_seq (aCtx, cp_info);
        break;

      default:				/* Unexpected tag	*/
        /* NOTE: Should NEVER get here, "asn1r_chk_tag" catches this.	*/
        rc = ASN1E_UNEXPECTED_TAG;
        break;
      }	/* end "switch (tagnum)"	*/
    }	/* end "while" loop	*/
  cp_info->num_ctxt++;	/* increment count of pc_def	*/
  return (rc);
  }

/************************************************************************/
/*			copp_dec_pc_def_list				*/
/* NOTE: This is only used during CP decode.				*/
/************************************************************************/
ST_RET copp_dec_pc_def_list (ASN1_DEC_CTXT *aCtx,
	COPP_DEC_CP *cp_info)	/* in/out: decoded data	*/
  {
ST_RET rc = SD_SUCCESS;
ST_UINT16 tagnum;

  /* Loop decoding tags until error or end of this constructor.*/
  while (rc == SD_SUCCESS	/* no error on last loop iteration	*/
         && (rc = asn1r_get_tag (aCtx, &tagnum)) == SD_SUCCESS
         && tagnum != 0)	/* 0 indicates end of constructor	*/
    {
    /* Handle each possible TAG.	*/
    switch (tagnum)
      {
      case TAG_SEQUENCE:
        /* decode one entry on list	*/
        rc = copp_dec_pc_def_list_entry (aCtx, cp_info);
        break;
  
      default:				/* Unexpected tag	*/
        COPP_LOG_ERR1 ("Decode error: unexpected tag 0x%X.", tagnum);
        rc = ASN1E_UNEXPECTED_TAG;
        break;
      }	/* end "switch (tagnum)"	*/
    }	/* end "while" loop	*/
  return (rc);
  }

/************************************************************************/
/*			copp_dec_cp_seq					*/
/************************************************************************/
ST_RET copp_dec_cp_seq (ASN1_DEC_CTXT *aCtx,
	COPP_DEC_CP *cp_info)	/* in/out: decoded data	*/
  {
ST_RET rc = SD_SUCCESS;
ST_UINT16 tagnum;
ST_UCHAR *contents_ptr;		/* ptr to ASN.1 contents octets	*/
ST_INT contents_len;		/* len of ASN.1 contents octets	*/
/* This TAG array created from info in Presentation spec.	*/
TAG_INFO tag_arr [] = {
	{TAG_VERSION,		TAG_OPTIONAL},
	{TAG_CALLING_PSEL,	TAG_OPTIONAL},
	{TAG_CALLED_PSEL,	TAG_OPTIONAL},
	{TAG_PC_DEF_LIST,	TAG_OPTIONAL},
	{TAG_DEF_CTXT_NAME,	TAG_OPTIONAL},
	{TAG_PRES_REQ,		TAG_OPTIONAL},
	{TAG_USER_SESS_REQ,	TAG_OPTIONAL},
	{TAG_USER_DATA_FULLY,	TAG_OPTIONAL} };
int tag_idx = 0;	/* index into tag_arr array (next to look for)	*/

  /* Loop decoding tags until error or end of this constructor.*/
  while (rc == SD_SUCCESS	/* no error on last loop iteration	*/
         && (rc = asn1r_get_tag (aCtx, &tagnum)) == SD_SUCCESS
         && tagnum != 0)	/* 0 indicates end of constructor	*/
    {
    rc = asn1r_chk_tag (tagnum, tag_arr, (sizeof(tag_arr)/sizeof(TAG_INFO)), &tag_idx);
    if (rc)
      {
      break;	/* error message already logged	*/
      }

    /* Handle each possible TAG.	*/
    switch (tagnum)
      {
      case TAG_VERSION:
        rc = asn1r_get_bitstr (aCtx, &cp_info->version_bit_str, 8);
        break;

      case TAG_CALLING_PSEL:
        rc = asn1r_get_contents (aCtx, &contents_ptr, &contents_len);
        if (rc == SD_SUCCESS)
          {
          if (contents_len <= MAX_PSEL_LEN)
            {
            cp_info->calling_psel [0] = contents_len;	/* first byte is len*/
            memcpy (&cp_info->calling_psel [1], contents_ptr, contents_len);
            }
          else
            {
            COPP_LOG_ERR2 ("P-CONNECT.ind: Calling PSEL len=%d exceeds max=%d",
                contents_len, MAX_PSEL_LEN);
            cp_info->send_negative_resp = SD_TRUE;	/* send negative response*/
            cp_info->negative_resp_reason = PROV_REASON_LOCAL_LIMIT;
            }
          }
        break;

      case TAG_CALLED_PSEL:
        rc = asn1r_get_contents (aCtx, &contents_ptr, &contents_len);
        if (rc == SD_SUCCESS)
          {
          if (contents_len <= MAX_PSEL_LEN)
            {
            cp_info->called_psel [0] = contents_len;	/* first byte is len*/
            memcpy (&cp_info->called_psel [1], contents_ptr, contents_len);
            }
          else
            {
            COPP_LOG_ERR2 ("P-CONNECT.ind: Called PSEL len=%d exceeds max=%d",
                contents_len, MAX_PSEL_LEN);
            cp_info->send_negative_resp = SD_TRUE;	/* send negative response*/
            cp_info->negative_resp_reason = PROV_REASON_LOCAL_LIMIT;
            }
          }
        break;

      case TAG_PC_DEF_LIST:
        rc = copp_dec_pc_def_list (aCtx, cp_info);	/* decode PC def_list constructor*/
        break;

      case TAG_DEF_CTXT_NAME:
        /* Default-context-name. If this is present, we will refuse	*/
        /* this connection so don't bother decoding it.			*/
        cp_info->def_ctxt_proposed = SD_TRUE;
        /* This skips over it, but we never use the contents.		*/
        rc = asn1r_get_contents (aCtx, &contents_ptr, &contents_len);
        break;

      case TAG_PRES_REQ:
        cp_info->pres_req_present = SD_TRUE;
        rc = asn1r_get_bitstr (aCtx, cp_info->pres_req, 8);
        break;

      case TAG_USER_SESS_REQ:
        cp_info->user_ses_req_present = SD_TRUE;
        rc = asn1r_get_bitstr (aCtx, cp_info->user_sess_req, 16);
        break;

      case TAG_USER_DATA_FULLY:
        /* This decodes User-data into "cp_info->pdv_info".		*/
        /* CRITICAL: must set expected_pci before calling copp_dec_pdv..*/
        cp_info->pdv_info.expected_pci = cp_info->pciacse;
        rc = copp_dec_pdv_list (aCtx, &cp_info->pdv_info);
        break;

      default:				/* Unexpected tag	*/
        /* NOTE: Should NEVER get here, "asn1r_chk_tag" catches this.	*/
        rc = ASN1E_UNEXPECTED_TAG;
        break;
      }	/* end "switch (tagnum)"	*/
    }	/* end "while" loop	*/
  return (rc);
  }

/************************************************************************/
/*			copp_dec_cp2					*/
/* Found CP constructor. Now decode the rest of it.			*/
/************************************************************************/
ST_RET copp_dec_cp2 (ASN1_DEC_CTXT *aCtx,
	COPP_DEC_CP *cp_info)	/* in/out: decoded data	*/
  {
ST_RET rc = SD_SUCCESS;
ST_UINT16 tagnum;

  /* Loop decoding tags until error or end of this constructor.*/
  while (rc == SD_SUCCESS	/* no error on last loop iteration	*/
         && (rc = asn1r_get_tag (aCtx, &tagnum)) == SD_SUCCESS
         && tagnum != 0)	/* 0 indicates end of constructor	*/
    {
    /* NOTE: do NOT need to call asn1r_chk_tag to check order of tags.	*/
    /*       This is a SET (not SEQUENCE), so order does not matter.	*/

    /* Handle each possible TAG.	*/
    switch (tagnum)
      {
      case TAG_MODE_CSTR:
        /* decode Mode constructor	*/
        rc = copp_dec_mode (aCtx, &cp_info->mode);
        break;
  
      case TAG_CP_SEQ:	/* same tag for CP or CPA	*/
        /* decode CP Sequence constructor*/
        rc = copp_dec_cp_seq (aCtx, cp_info);
        break;

      default:				/* Unexpected tag	*/
        COPP_LOG_ERR1 ("Decode error: unexpected tag 0x%X.", tagnum);
        return (ASN1E_UNEXPECTED_TAG);
      }	/* end "switch (tagnum)"	*/
    }	/* end "while" loop	*/
  return (rc);
  }

/************************************************************************/
/*			copp_dec_cp					*/
/* Decode Presentation "CP" PDU.					*/
/* NOTE: Initial memset of "cp_info" sets PSEL lengths to 0		*/
/*       (important in case they are NOT received in PDU).		*/
/************************************************************************/
ST_RET copp_dec_cp (
	ST_UCHAR *pdu_ptr,	/* PDU pointer		*/
	ST_INT pdu_len,		/* PDU length		*/
	COPP_DEC_CP *cp_info)	/* out: decoded data	*/
  {
ASN1_DEC_CTXT aCtx;
ST_RET rc = SD_SUCCESS;
ST_UINT16 tagnum;

  /* Initialize the cp_info structure.	*/
  memset (cp_info, 0, sizeof (COPP_DEC_CP));	/* CRITICAL: start clean*/

  /* Set default protocol version (used if this optional param not encoded).*/
  cp_info->version_bit_str = 0x80;	/* Default to "version1".*/

  /* Initialize the ASN.1 decode context.	*/
  rc = asn1r_decode_init (&aCtx, pdu_ptr, pdu_len);
  if (rc)
    return (rc);

  /* Get first tag.	*/
  rc = asn1r_get_tag (&aCtx, &tagnum);
  if (rc)
    return (rc);

  /* Handle each possible TAG.	*/
  switch (tagnum)
    {
    case TAG_SET:	/* only TAG allowed	*/
      rc = copp_dec_cp2 (&aCtx, cp_info);
      break;
  
    default:				/* Unexpected tag	*/
      COPP_LOG_ERR1 ("Decode error: unexpected tag 0x%X.", tagnum);
      rc = ASN1E_UNEXPECTED_TAG;
      break;
    }	/* end "switch (tagnum)"	*/
  return (rc);
  }

/*======================================================================*/
/*======================================================================*/
/*		CPA decode functions					*/
/*======================================================================*/
/*======================================================================*/

/************************************************************************/
/*			copp_dec_cpa_seq				*/
/************************************************************************/
ST_RET copp_dec_cpa_seq (ASN1_DEC_CTXT *aCtx,
	COPP_DEC_CPA *cpa_info)	/* in/out: decoded data	*/
  {
ST_RET rc = SD_SUCCESS;
ST_UINT16 tagnum;
ST_UCHAR *contents_ptr;		/* ptr to ASN.1 contents octets	*/
ST_INT contents_len;		/* len of ASN.1 contents octets	*/
/* This TAG array created from info in Presentation spec.	*/
TAG_INFO tag_arr [] = {
	{TAG_VERSION,		TAG_OPTIONAL},
	{TAG_RESPONDING_PSEL,	TAG_OPTIONAL},
	{TAG_PC_DEF_RESULT_LIST,TAG_OPTIONAL},
	{TAG_PRES_REQ,		TAG_OPTIONAL},
	{TAG_USER_SESS_REQ,	TAG_OPTIONAL},
	{TAG_USER_DATA_FULLY,	TAG_OPTIONAL} };
int tag_idx = 0;	/* index into tag_arr array (next to look for)	*/

  /* Loop decoding tags until error or end of this constructor.*/
  while (rc == SD_SUCCESS	/* no error on last loop iteration	*/
         && (rc = asn1r_get_tag (aCtx, &tagnum)) == SD_SUCCESS
         && tagnum != 0)	/* 0 indicates end of constructor	*/
    {
    rc = asn1r_chk_tag (tagnum, tag_arr, (sizeof(tag_arr)/sizeof(TAG_INFO)), &tag_idx);
    if (rc)
      {
      break;	/* error message already logged	*/
      }

    /* Handle each possible TAG.	*/
    switch (tagnum)
      {
      case TAG_VERSION:
        rc = asn1r_get_bitstr (aCtx, &cpa_info->version_bit_str, 8);
        break;

      case TAG_RESPONDING_PSEL:
        rc = asn1r_get_contents (aCtx, &contents_ptr, &contents_len);
        if (rc == SD_SUCCESS)
          {
          if (contents_len <= MAX_PSEL_LEN)
            {
            cpa_info->responding_psel [0] = contents_len;	/* first byte is len*/
            memcpy (&cpa_info->responding_psel [1], contents_ptr, contents_len);
            }
          else
            {
            /* Log it but ignore problem. Don't really care about Responding PSEL.*/
            COPP_LOG_ERR2 ("P-CONNECT.cnf+: Responding PSEL len=%d exceeds max=%d",
                contents_len, MAX_PSEL_LEN);
            }
          }
        break;

      case TAG_PC_DEF_RESULT_LIST:
        rc = copp_dec_pc_def_result_list (aCtx, &cpa_info->pc_def_result_list);
        break;

      case TAG_PRES_REQ:
        cpa_info->pres_req_present = SD_TRUE;
        rc = asn1r_get_bitstr (aCtx, cpa_info->pres_req, 8);
        break;

      case TAG_USER_SESS_REQ:
        cpa_info->user_ses_req_present = SD_TRUE;
        rc = asn1r_get_bitstr (aCtx, cpa_info->user_sess_req, 16);
        break;

      case TAG_USER_DATA_FULLY:
        /* This decodes User-data into "cpa_info->pdv_info".		*/
        /* NOTE: cpa_info->pdv_info.expected_pci set at start of decode.*/
        rc = copp_dec_pdv_list (aCtx, &cpa_info->pdv_info);
        break;

      default:				/* Unexpected tag	*/
        /* NOTE: Should NEVER get here, "asn1r_chk_tag" catches this.	*/
        rc = ASN1E_UNEXPECTED_TAG;
        break;
      }	/* end "switch (tagnum)"	*/
    }	/* end "while" loop	*/
  return (rc);
  }

/************************************************************************/
/*			copp_dec_cpa2					*/
/* Found CPA constructor. Now decode the rest of it.			*/
/* NOTE: almost identical to "copp_dec_cp2", but filling COPP_DEC_CPA.	*/
/************************************************************************/
ST_RET copp_dec_cpa2 (ASN1_DEC_CTXT *aCtx,
	COPP_DEC_CPA *cpa_info)	/* in/out: decoded data	*/
  {
ST_RET rc = SD_SUCCESS;
ST_UINT16 tagnum;

  /* Loop decoding tags until error or end of this constructor.*/
  while (rc == SD_SUCCESS	/* no error on last loop iteration	*/
         && (rc = asn1r_get_tag (aCtx, &tagnum)) == SD_SUCCESS
         && tagnum != 0)	/* 0 indicates end of constructor	*/
    {
    /* NOTE: do NOT need to call asn1r_chk_tag to check order of tags.	*/
    /*       This is a SET (not SEQUENCE), so order does not matter.	*/

    /* Handle each possible TAG.	*/
    switch (tagnum)
      {
      case TAG_MODE_CSTR:
        /* decode Mode constructor	*/
        rc = copp_dec_mode (aCtx, &cpa_info->mode);
        break;
  
      case TAG_CPA_SEQ:
        /* decode CPA Sequence constructor*/
        rc = copp_dec_cpa_seq (aCtx, cpa_info);
        break;

      default:				/* Unexpected tag	*/
        COPP_LOG_ERR1 ("Decode error: unexpected tag 0x%X.", tagnum);
        return (ASN1E_UNEXPECTED_TAG);
      }	/* end "switch (tagnum)"	*/
    }	/* end "while" loop	*/
  return (rc);
  }

/************************************************************************/
/*			copp_dec_cpa					*/
/* Decode Presentation "CPA" PDU.					*/
/* NOTE: Initial memset of "cpa_info" sets PSEL lengths to 0		*/
/*       (important in case they are NOT received in PDU).		*/
/************************************************************************/
ST_RET copp_dec_cpa (
	ST_UCHAR *pdu_ptr,		/* PDU pointer			*/
	ST_INT pdu_len,			/* PDU length			*/
	ST_INT8 expected_pci,		/* expected PCI			*/
	COPP_DEC_CPA *cpa_info)		/* out: decoded data		*/
  {
ASN1_DEC_CTXT aCtx;
ST_RET rc = SD_SUCCESS;
ST_UINT16 tagnum;

  /* Initialize the cpa_info structure.	*/
  memset (cpa_info, 0, sizeof (COPP_DEC_CPA));	/* CRITICAL: start clean*/

  /* Set default protocol version (used if this optional param not encoded).*/
  cpa_info->version_bit_str = 0x80;	/* Default to "version1".*/

  cpa_info->pdv_info.expected_pci = expected_pci;

  /* Initialize the ASN.1 decode context.	*/
  rc = asn1r_decode_init (&aCtx, pdu_ptr, pdu_len);
  if (rc)
    return (rc);

  /* Get first tag.	*/
  rc = asn1r_get_tag (&aCtx, &tagnum);
  if (rc)
    return (rc);

  /* Handle each possible TAG.	*/
  switch (tagnum)
    {
    case TAG_SET:	/* only TAG allowed	*/
      rc = copp_dec_cpa2 (&aCtx, cpa_info);
      break;
  
    default:				/* Unexpected tag	*/
      COPP_LOG_ERR1 ("Decode error: unexpected tag 0x%X.", tagnum);
      rc = ASN1E_UNEXPECTED_TAG;
      break;
    }	/* end "switch (tagnum)"	*/
  return (rc);
  }

/*======================================================================*/
/*======================================================================*/
/*		CPR decode functions					*/
/*======================================================================*/
/*======================================================================*/

/************************************************************************/
/*			copp_dec_cpr_seq				*/
/* Found CPR Sequence constructor. Now decode the rest of it.		*/
/************************************************************************/
ST_RET copp_dec_cpr_seq (ASN1_DEC_CTXT *aCtx,
	COPP_DEC_CPR *cpr_info)	/* in/out: decoded data	*/
  {
ST_RET rc = SD_SUCCESS;
ST_UINT16 tagnum;
ST_UCHAR *contents_ptr;		/* ptr to ASN.1 contents octets	*/
ST_INT contents_len;		/* len of ASN.1 contents octets	*/
/* This TAG array created from info in Presentation spec.	*/
TAG_INFO tag_arr [] = {
	{TAG_VERSION,			TAG_OPTIONAL},
	{TAG_RESPONDING_PSEL,		TAG_OPTIONAL},
	{TAG_PC_DEF_RESULT_LIST,	TAG_OPTIONAL},
	{TAG_DEFAULT_CONTEXT_RESULT,	TAG_OPTIONAL},
	{TAG_PROVIDER_REASON,		TAG_OPTIONAL},
	{TAG_USER_DATA_FULLY,		TAG_OPTIONAL} };
int tag_idx = 0;	/* index into tag_arr array (next to look for)	*/

  /* Loop decoding tags until error or end of this constructor.*/
  while (rc == SD_SUCCESS	/* no error on last loop iteration	*/
         && (rc = asn1r_get_tag (aCtx, &tagnum)) == SD_SUCCESS
         && tagnum != 0)	/* 0 indicates end of constructor	*/
    {
    rc = asn1r_chk_tag (tagnum, tag_arr, (sizeof(tag_arr)/sizeof(TAG_INFO)), &tag_idx);
    if (rc)
      {
      break;	/* error message already logged	*/
      }

    /* Handle each possible TAG.	*/
    switch (tagnum)
      {
      case TAG_VERSION:
        rc = asn1r_get_bitstr (aCtx, &cpr_info->version_bit_str, 8);
        break;

      case TAG_RESPONDING_PSEL:
        rc = asn1r_get_contents (aCtx, &contents_ptr, &contents_len);
        if (rc == SD_SUCCESS)
          {
          if (contents_len <= MAX_PSEL_LEN)
            {
            cpr_info->responding_psel [0] = contents_len;	/* first byte is len*/
            memcpy (&cpr_info->responding_psel [1], contents_ptr, contents_len);
            }
          else
            {
            /* Log it but ignore problem. Don't really care about Responding PSEL.*/
            COPP_LOG_ERR2 ("P-CONNECT.cnf-: Responding PSEL len=%d exceeds max=%d",
                contents_len, MAX_PSEL_LEN);
            }
          }
        break;

      case TAG_PC_DEF_RESULT_LIST:
        rc = copp_dec_pc_def_result_list (aCtx, &cpr_info->pc_def_result_list);
        break;

      case TAG_DEFAULT_CONTEXT_RESULT:
        rc = asn1r_get_i8 (aCtx, &cpr_info->default_context_result);
        break;

      case TAG_PROVIDER_REASON:
        cpr_info->provider_reason_present = SD_TRUE;
        rc = asn1r_get_i8 (aCtx, &cpr_info->provider_reason);
        break;

      case TAG_USER_DATA_FULLY:
        /* This decodes User-data into "cpr_info->pdv_info".		*/
        /* NOTE: cpr_info->pdv_info.expected_pci set at start of decode.*/
        rc = copp_dec_pdv_list (aCtx, &cpr_info->pdv_info);
        break;

      default:				/* Unexpected tag	*/
        /* NOTE: Should NEVER get here, "asn1r_chk_tag" catches this.	*/
        rc = ASN1E_UNEXPECTED_TAG;
        break;
      }	/* end "switch (tagnum)"	*/
    }	/* end "while" loop	*/
  return (rc);
  }

/************************************************************************/
/*			copp_dec_cpr					*/
/* Decode Presentation "CPR" PDU.					*/
/************************************************************************/
ST_RET copp_dec_cpr (
	ST_UCHAR *pdu_ptr,		/* PDU pointer			*/
	ST_INT pdu_len,			/* PDU length			*/
	ST_INT8 expected_pci,		/* expected PCI			*/
	COPP_DEC_CPR *cpr_info)		/* out: decoded data		*/
  {
ASN1_DEC_CTXT aCtx;
ST_RET rc = SD_SUCCESS;
ST_UINT16 tagnum;

  /* Initialize the cpr_info structure.	*/
  memset (cpr_info, 0, sizeof (COPP_DEC_CPR));	/* CRITICAL: start clean*/

  /* Set default protocol version (used if this optional param not encoded).*/
  cpr_info->version_bit_str = 0x80;	/* Default to "version1".*/

  cpr_info->pdv_info.expected_pci = expected_pci;

  /* Initialize the ASN.1 decode context.	*/
  rc = asn1r_decode_init (&aCtx, pdu_ptr, pdu_len);
  if (rc)
    return (rc);

  /* Get first tag.	*/
  rc = asn1r_get_tag (&aCtx, &tagnum);
  if (rc)
    return (rc);

  /* Handle each possible TAG.	*/
  switch (tagnum)
    {
    case TAG_SEQUENCE:	/* only TAG allowed	*/
      rc = copp_dec_cpr_seq (&aCtx, cpr_info);
      break;
  
    default:				/* Unexpected tag	*/
      COPP_LOG_ERR1 ("Decode error: unexpected tag 0x%X.", tagnum);
      rc = ASN1E_UNEXPECTED_TAG;
      break;
    }	/* end "switch (tagnum)"	*/
  return (rc);
  }

/*======================================================================*/
/*======================================================================*/
/*		ARU, ARP decode functions				*/
/*======================================================================*/
/*======================================================================*/

/************************************************************************/
/*			copp_dec_pc_identifier_list_entry		*/
/* NOTE: used only during ARU decode.					*/
/************************************************************************/
ST_RET copp_dec_pc_identifier_list_entry (ASN1_DEC_CTXT *aCtx,
	COPP_DEC_ARX *arx_info)	/* in/out: decoded data	*/
  {
ST_RET rc = SD_SUCCESS;
ST_UINT16 tagnum;
MMS_OBJ_ID tsn;		/* TSN rcvd	*/
ST_INT8 pci;		/* PCI rcvd	*/
/* This TAG array created from info in Presentation spec.	*/
TAG_INFO tag_arr [] = {
	{TAG_PCI,	TAG_MANDATORY},
	{TAG_TSN,	TAG_MANDATORY} };
int tag_idx = 0;	/* index into tag_arr array (next to look for)	*/

  /* Loop decoding tags until error or end of this constructor.*/
  while (rc == SD_SUCCESS	/* no error on last loop iteration	*/
         && (rc = asn1r_get_tag (aCtx, &tagnum)) == SD_SUCCESS
         && tagnum != 0)	/* 0 indicates end of constructor	*/
    {
    rc = asn1r_chk_tag (tagnum, tag_arr, (sizeof(tag_arr)/sizeof(TAG_INFO)), &tag_idx);
    if (rc)
      {
      break;	/* error message already logged	*/
      }

    /* Handle each possible TAG.	*/
    switch (tagnum)
      {
      case TAG_PCI:
        rc = asn1r_get_i8 (aCtx, &pci);
        /* Should be ACSE PCI, but we're aborting, so we don't really care.*/
        break;
  
      case TAG_TSN:
        rc = asn1r_get_objid (aCtx, tsn.comps, &tsn.num_comps);
        /* Should be ASN.1 TSN, but we're aborting, so we don't really care.*/
        break;

      default:				/* Unexpected tag	*/
        /* NOTE: Should NEVER get here, "asn1r_chk_tag" catches this.	*/
        rc = ASN1E_UNEXPECTED_TAG;
        break;
      }	/* end "switch (tagnum)"	*/
    }	/* end "while" loop	*/
  arx_info->num_identifier++;	/* increment count of pc_identifier	*/
  return (rc);
  }

/************************************************************************/
/*			copp_dec_pc_identifier_list			*/
/* NOTE: used only during ARU decode.					*/
/************************************************************************/
ST_RET copp_dec_pc_identifier_list (ASN1_DEC_CTXT *aCtx,
	COPP_DEC_ARX *arx_info)	/* in/out: decoded data	*/
  {
ST_RET rc = SD_SUCCESS;
ST_UINT16 tagnum;

  /* Loop decoding tags until error or end of this constructor.*/
  while (rc == SD_SUCCESS	/* no error on last loop iteration	*/
         && (rc = asn1r_get_tag (aCtx, &tagnum)) == SD_SUCCESS
         && tagnum != 0)	/* 0 indicates end of constructor	*/
    {
    /* Handle each possible TAG.	*/
    switch (tagnum)
      {
      case TAG_SEQUENCE:
        /* decode one entry on list	*/
        rc = copp_dec_pc_identifier_list_entry (aCtx, arx_info);
        break;
  
      default:				/* Unexpected tag	*/
        COPP_LOG_ERR1 ("Decode error: unexpected tag 0x%X.", tagnum);
        rc = ASN1E_UNEXPECTED_TAG;
        break;
      }	/* end "switch (tagnum)"	*/
    }	/* end "while" loop	*/
  return (rc);
  }

/************************************************************************/
/*			copp_dec_aru_seq				*/
/* Found ARU Sequence constructor. Now decode the rest of it.		*/
/************************************************************************/
ST_RET copp_dec_aru_seq (ASN1_DEC_CTXT *aCtx,
	COPP_DEC_ARX *arx_info)	/* in/out: decoded data	*/
  {
ST_RET rc = SD_SUCCESS;
ST_UINT16 tagnum;
/* This TAG array created from info in Presentation spec.	*/
TAG_INFO tag_arr [] = {
	{TAG_ARU_PC_IDENTIFIER_LIST,	TAG_OPTIONAL},
	{TAG_USER_DATA_FULLY,		TAG_OPTIONAL} };
int tag_idx = 0;	/* index into tag_arr array (next to look for)	*/

  /* Loop decoding tags until error or end of this constructor.*/
  while (rc == SD_SUCCESS	/* no error on last loop iteration	*/
         && (rc = asn1r_get_tag (aCtx, &tagnum)) == SD_SUCCESS
         && tagnum != 0)	/* 0 indicates end of constructor	*/
    {
    rc = asn1r_chk_tag (tagnum, tag_arr, (sizeof(tag_arr)/sizeof(TAG_INFO)), &tag_idx);
    if (rc)
      {
      break;	/* error message already logged	*/
      }

    /* Handle each possible TAG.	*/
    switch (tagnum)
      {
      case TAG_ARU_PC_IDENTIFIER_LIST:
        rc = copp_dec_pc_identifier_list (aCtx, arx_info);
        break;

      case TAG_USER_DATA_FULLY:
        /* This decodes User-data into "arx_info->pdv_info".		*/
        /* NOTE: arx_info->pdv_info.expected_pci set at start of decode.*/
        rc = copp_dec_pdv_list (aCtx, &arx_info->pdv_info);
        break;

      default:				/* Unexpected tag	*/
        /* NOTE: Should NEVER get here, "asn1r_chk_tag" catches this.	*/
        rc = ASN1E_UNEXPECTED_TAG;
        break;
      }	/* end "switch (tagnum)"	*/
    }	/* end "while" loop	*/
  return (rc);
  }

/************************************************************************/
/*			copp_dec_arp_seq				*/
/* Found ARP Sequence constructor. Now decode the rest of it.		*/
/************************************************************************/
ST_RET copp_dec_arp_seq (ASN1_DEC_CTXT *aCtx,
	COPP_DEC_ARX *arx_info)	/* in/out: decoded data	*/
  {
ST_RET rc = SD_SUCCESS;
ST_UINT16 tagnum;
/* This TAG array created from info in Presentation spec.	*/
TAG_INFO tag_arr [] = {
	{TAG_ARP_ABORT_REASON,	TAG_OPTIONAL},
	{TAG_ARP_EVENT_ID,	TAG_OPTIONAL} };
int tag_idx = 0;	/* index into tag_arr array (next to look for)	*/

  /* Loop decoding tags until error or end of this constructor.*/
  while (rc == SD_SUCCESS	/* no error on last loop iteration	*/
         && (rc = asn1r_get_tag (aCtx, &tagnum)) == SD_SUCCESS
         && tagnum != 0)	/* 0 indicates end of constructor	*/
    {
    rc = asn1r_chk_tag (tagnum, tag_arr, (sizeof(tag_arr)/sizeof(TAG_INFO)), &tag_idx);
    if (rc)
      {
      break;	/* error message already logged	*/
      }

    /* Handle each possible TAG.	*/
    switch (tagnum)
      {
      case TAG_ARP_ABORT_REASON:
        rc = asn1r_get_i8 (aCtx, &arx_info->abort_reason);
        break;

      case TAG_ARP_EVENT_ID:
        rc = asn1r_get_i8 (aCtx, &arx_info->event_id);
        break;

      default:				/* Unexpected tag	*/
        /* NOTE: Should NEVER get here, "asn1r_chk_tag" catches this.	*/
        rc = ASN1E_UNEXPECTED_TAG;
        break;
      }	/* end "switch (tagnum)"	*/
    }	/* end "while" loop	*/
  return (rc);
  }

/************************************************************************/
/*			copp_dec_arx					*/
/* Decode Presentation "ARU" or "ARP" PDU.				*/
/************************************************************************/
ST_RET copp_dec_arx (
	ST_UCHAR *pdu_ptr,		/* PDU pointer			*/
	ST_INT pdu_len,			/* PDU length			*/
	ST_INT8 expected_pci,		/* expected PCI			*/
	COPP_DEC_ARX *arx_info)		/* out: decoded data		*/
  {
ASN1_DEC_CTXT aCtx;
ST_RET rc = SD_SUCCESS;
ST_UINT16 tagnum;

  /* Initialize the arx_info structure.	*/
  memset (arx_info, 0, sizeof (COPP_DEC_ARX));	/* CRITICAL: start clean*/

  /* Need this later when decoding the PDV.	*/
  arx_info->pdv_info.expected_pci = expected_pci;

  /* Initialize the ASN.1 decode context.	*/
  rc = asn1r_decode_init (&aCtx, pdu_ptr, pdu_len);
  if (rc)
    return (rc);

  /* Get first tag.	*/
  rc = asn1r_get_tag (&aCtx, &tagnum);
  if (rc)
    return (rc);

  /* Handle each possible TAG. Must be ARU or ARP.	*/
  switch (tagnum)
    {
    case TAG_ARU:
      arx_info->abort_type = COPP_U_ABORT;
      rc = copp_dec_aru_seq (&aCtx, arx_info);
      break;
  
    case TAG_ARP:	/* this is beginning of ARP seq	*/
      arx_info->abort_type = COPP_P_ABORT;
      rc = copp_dec_arp_seq (&aCtx, arx_info);
      break;

    default:				/* Unexpected tag	*/
      COPP_LOG_ERR1 ("Decode error: unexpected tag 0x%X.", tagnum);
      rc = ASN1E_UNEXPECTED_TAG;
      break;
    }	/* end "switch (tagnum)"	*/
  return (rc);
  }

