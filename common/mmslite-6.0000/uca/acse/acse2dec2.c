/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	1995-2009, All Rights Reserved					*/
/*									*/
/* MODULE NAME : acse2dec2.c						*/
/* PRODUCT(S)  :							*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*	ACSE Decode Functions using "asn1r_get_tag". These functions	*/
/*	avoid the use of any global variables.				*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*			acse_dec_aarq					*/
/*			acse_dec_aare					*/
/*			acse_dec_rlrq					*/
/*			acse_dec_rlre					*/
/*			acse_dec_abrt					*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev     Comments					*/
/* --------  ---  ------   -------------------------------------------  */
/* 11/17/09  JRB    62     New ACSE decode functions.			*/
/************************************************************************/
#include "glbtypes.h"
#include "sysincs.h"
#include "mem_chk.h"
#include "asn1r.h"
#include "asn1log.h"	/* for ASN.1 decode logging	*/
#include "acseauth.h"
#include "acse2.h"
#include "copp.h"	/* for copp_dec_pdv_entry, PDV_INFO	*/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/* ASN.1 tags for ACSE APDUs						*/
/* NOTE: decoder needs TAG defs like this (class & constr masks included).*/
#define TAGDEC_AARQ	(ASN1MASK_APP | ASN1MASK_CONSTR | 0)
#define TAGDEC_AARE	(ASN1MASK_APP | ASN1MASK_CONSTR | 1)
#define TAGDEC_RLRQ	(ASN1MASK_APP | ASN1MASK_CONSTR | 2)
#define TAGDEC_RLRE	(ASN1MASK_APP | ASN1MASK_CONSTR | 3)
#define TAGDEC_ABRT	(ASN1MASK_APP | ASN1MASK_CONSTR | 4)
#define TAGDEC_ADT    	(ASN1MASK_APP | ASN1MASK_CONSTR | 5)

/* ASN.1 tags for elements of AARQ APDU					*/
#define TAGDEC_AARQ_protocol_version		(ASN1MASK_CTX | 0)
#define TAGDEC_AARQ_ASO_context_name		(ASN1MASK_CTX | ASN1MASK_CONSTR | 1)
#define TAGDEC_AARQ_CALLED_AP_title		(ASN1MASK_CTX | ASN1MASK_CONSTR | 2)
#define TAGDEC_AARQ_CALLED_AE_qual		(ASN1MASK_CTX | ASN1MASK_CONSTR | 3)
#define TAGDEC_AARQ_CALLED_AP_inv_id		(ASN1MASK_CTX | ASN1MASK_CONSTR | 4)
#define TAGDEC_AARQ_CALLED_AE_inv_id		(ASN1MASK_CTX | ASN1MASK_CONSTR | 5)
#define TAGDEC_AARQ_CALLING_AP_title		(ASN1MASK_CTX | ASN1MASK_CONSTR | 6)
#define TAGDEC_AARQ_CALLING_AE_qual		(ASN1MASK_CTX | ASN1MASK_CONSTR | 7)
#define TAGDEC_AARQ_CALLING_AP_inv_id		(ASN1MASK_CTX | ASN1MASK_CONSTR | 8)
#define TAGDEC_AARQ_CALLING_AE_inv_id		(ASN1MASK_CTX | ASN1MASK_CONSTR | 9)
#define TAGDEC_AARQ_acse_requirements		(ASN1MASK_CTX | 10)
#define TAGDEC_AARQ_mechanism_name		(ASN1MASK_CTX | 11)
#define TAGDEC_AARQ_CALLING_auth_value		(ASN1MASK_CTX | ASN1MASK_CONSTR | 12)
#define TAGDEC_AARQ_user_info			(ASN1MASK_CTX | ASN1MASK_CONSTR | 30)

/* ASN.1 tags for elements of AARE APDU					*/
#define TAGDEC_AARE_protocol_version		(ASN1MASK_CTX | 0)
#define TAGDEC_AARE_ASO_context_name		(ASN1MASK_CTX | ASN1MASK_CONSTR | 1)
#define TAGDEC_AARE_result			(ASN1MASK_CTX | ASN1MASK_CONSTR | 2)
#define TAGDEC_AARE_result_source_diagnostic	(ASN1MASK_CTX | ASN1MASK_CONSTR | 3)
#define TAGDEC_AARE_RESPONDING_AP_title		(ASN1MASK_CTX | ASN1MASK_CONSTR | 4)
#define TAGDEC_AARE_RESPONDING_AE_qual		(ASN1MASK_CTX | ASN1MASK_CONSTR | 5)
#define TAGDEC_AARE_RESPONDING_AP_inv_id	(ASN1MASK_CTX | ASN1MASK_CONSTR | 6)
#define TAGDEC_AARE_RESPONDING_AE_inv_id	(ASN1MASK_CTX | ASN1MASK_CONSTR | 7)
#define TAGDEC_AARE_RESPONDER_acse_requirements	(ASN1MASK_CTX | 8)
#define TAGDEC_AARE_mechanism_name		(ASN1MASK_CTX | 9)
#define TAGDEC_AARE_RESPONDING_auth_value	(ASN1MASK_CTX | ASN1MASK_CONSTR | 10)
#define TAGDEC_AARE_user_info			(ASN1MASK_CTX | ASN1MASK_CONSTR | 30)


/* ASN.1 tags for elements of RLRQ APDU					*/
#define TAGDEC_RLRQ_reason			(ASN1MASK_CTX | 0)
#define TAGDEC_RLRQ_user_info			(ASN1MASK_CTX | ASN1MASK_CONSTR | 30)

/* ASN.1 tags for elements of RLRE APDU					*/
#define TAGDEC_RLRE_reason			(ASN1MASK_CTX | 0)
#define TAGDEC_RLRE_user_info			(ASN1MASK_CTX | ASN1MASK_CONSTR | 30)

/* ASN.1 tags for elements of ABRT APDU					*/
#define TAGDEC_ABRT_source			(ASN1MASK_CTX | 0)
#define TAGDEC_ABRT_diagnostic			(ASN1MASK_CTX | 1)
#define TAGDEC_ABRT_user_info			(ASN1MASK_CTX | ASN1MASK_CONSTR | 30)

/* ASN.1 tags for common ACSE elements					*/
#define TAG_INT32		(ASN1MASK_UNI | INT_CODE)
#define TAG_OBJ_ID		(ASN1MASK_UNI | OBJ_ID_CODE)
#define TAG_EXTERN		(ASN1MASK_UNI | ASN1MASK_CONSTR | EXTERN_CODE)
#define TAG_RESULT_SOURCE_1	(ASN1MASK_CTX | ASN1MASK_CONSTR | 1)
#define TAG_RESULT_SOURCE_2	(ASN1MASK_CTX | ASN1MASK_CONSTR | 2)

#define TAG_AUTH_GRAPHICSTRING	(ASN1MASK_CTX | 0)
#define TAG_AUTH_EXTERNAL	(ASN1MASK_CTX | ASN1MASK_CONSTR | 2)

#define TAG_EXTERNAL_SINGLEASN1	(ASN1MASK_CTX | ASN1MASK_CONSTR | 0)

/************************************************************************/
/*			acse_dec_user_info				*/
/* NOTE: almost same as copp_dec_pdv_list but looks for EXTERN tag	*/
/*       instead of SEQUENCE tag.					*/
/************************************************************************/
static ST_RET acse_dec_user_info (ASN1_DEC_CTXT *aCtx,
	ST_INT8 expected_pci,		/* expected PCI			*/
	PDV_INFO *pdv_info)		/* out: decoded PDV data	*/
  {
ST_UINT16 tagnum;
ST_RET rc = SD_SUCCESS;

  /* Initialize the pdv_info structure.	*/
  memset (pdv_info, 0, sizeof (PDV_INFO));	/* CRITICAL: start clean*/

  pdv_info->expected_pci = expected_pci;

  /* Loop decoding tags until error or end of this constructor.*/
  while (rc == SD_SUCCESS	/* no error on last loop iteration	*/
         && (rc = asn1r_get_tag (aCtx, &tagnum)) == SD_SUCCESS
         && tagnum != 0)	/* 0 indicates end of constructor	*/
    {
    /* Handle each possible TAG.	*/
    switch (tagnum)
      {
      case TAG_EXTERN:	/* only TAG allowed	*/
        rc = copp_dec_pdv_entry (aCtx, pdv_info);
        break;
  
      default:				/* Unexpected tag	*/
        ACSELOG_ERR1 ("Decode error: unexpected tag 0x%X.", tagnum);
        rc = ASN1E_UNEXPECTED_TAG;
        break;
      }	/* end "switch (tagnum)"	*/
    }	/* end "while" loop	*/
  return (rc);
  }
/************************************************************************/
/*			acse_dec_constr_int32				*/
/* Decode constructor containing one INT32 value.			*/
/************************************************************************/
static ST_RET acse_dec_constr_int32 (ASN1_DEC_CTXT *aCtx,
	ST_INT32 *int32)	/* out: decoded data	*/
  {
ST_UINT16 tagnum;
ST_RET rc = SD_SUCCESS;

  /* Loop decoding tags until error or end of this constructor.*/
  while (rc == SD_SUCCESS	/* no error on last loop iteration	*/
         && (rc = asn1r_get_tag (aCtx, &tagnum)) == SD_SUCCESS
         && tagnum != 0)	/* 0 indicates end of constructor	*/
    {
    /* Handle each possible TAG.	*/
    switch (tagnum)
      {
      case TAG_INT32:	/* only TAG allowed	*/
        rc = asn1r_get_i32 (aCtx, int32);
        break;
  
      default:				/* Unexpected tag	*/
        ACSELOG_ERR1 ("Decode error: unexpected tag 0x%X.", tagnum);
        rc = ASN1E_UNEXPECTED_TAG;
        break;
      }	/* end "switch (tagnum)"	*/
    }	/* end "while" loop	*/
  return (rc);
  }
/************************************************************************/
/*			acse_dec_constr_obj_id				*/
/* Decode constructor containing one OBJ_ID value.			*/
/************************************************************************/
static ST_RET acse_dec_constr_obj_id (ASN1_DEC_CTXT *aCtx,
	MMS_OBJ_ID *obj_id)	/* out: decoded data	*/
  {
ST_UINT16 tagnum;
ST_RET rc = SD_SUCCESS;

  /* Loop decoding tags until error or end of this constructor.*/
  while (rc == SD_SUCCESS	/* no error on last loop iteration	*/
         && (rc = asn1r_get_tag (aCtx, &tagnum)) == SD_SUCCESS
         && tagnum != 0)	/* 0 indicates end of constructor	*/
    {
    /* Handle each possible TAG.	*/
    switch (tagnum)
      {
      case TAG_OBJ_ID:	/* only TAG allowed	*/
        rc = asn1r_get_objid (aCtx, obj_id->comps,&obj_id->num_comps);
        break;
  
      default:				/* Unexpected tag	*/
        ACSELOG_ERR1 ("Decode error: unexpected tag 0x%X.", tagnum);
        rc = ASN1E_UNEXPECTED_TAG;
        break;
      }	/* end "switch (tagnum)"	*/
    }	/* end "while" loop	*/
  return (rc);
  }
/************************************************************************/
/*			acse_dec_constr_diag				*/
/* Decode constructor containing diag (tag value indicates source).	*/
/************************************************************************/
static ST_RET acse_dec_constr_diag (ASN1_DEC_CTXT *aCtx,
	AARE_APDU *aare_apdu)	/* out: decoded data	*/
  {
ST_UINT16 tagnum;
ST_RET rc = SD_SUCCESS;
ST_INT32 int32;			/* tmp var for decoded integer	*/

  /* Loop decoding tags until error or end of this constructor.*/
  while (rc == SD_SUCCESS	/* no error on last loop iteration	*/
         && (rc = asn1r_get_tag (aCtx, &tagnum)) == SD_SUCCESS
         && tagnum != 0)	/* 0 indicates end of constructor	*/
    {
    /* Handle each possible TAG.	*/
    switch (tagnum)
      {
      case TAG_RESULT_SOURCE_1:
        aare_apdu->result_source = 1;
        rc = acse_dec_constr_int32 (aCtx, &int32);
        if (rc==SD_SUCCESS)
          aare_apdu->result_diag = (ST_INT16) int32;
        break;
  
      case TAG_RESULT_SOURCE_2:
        aare_apdu->result_source = 2;
        rc = acse_dec_constr_int32 (aCtx, &int32);
        if (rc==SD_SUCCESS)
          aare_apdu->result_diag = (ST_INT16) int32;
        break;
  
      default:				/* Unexpected tag	*/
        ACSELOG_ERR1 ("Decode error: unexpected tag 0x%X.", tagnum);
        rc = ASN1E_UNEXPECTED_TAG;
        break;
      }	/* end "switch (tagnum)"	*/
    }	/* end "while" loop	*/
  return (rc);
  }

/************************************************************************/
/*			acse_dec_auth_value_external			*/
/************************************************************************/
ST_RET acse_dec_auth_value_external (
	ASN1_DEC_CTXT *aCtx,
	ACSE_AUTH_INFO *authInfo)		/* out	*/
  {
ST_UINT16 tagnum;
ST_RET rc = SD_SUCCESS;
ST_UCHAR *auth_ptr;
ST_INT auth_len;

  /* Loop decoding tags until error or end of this constructor.*/
  while (rc == SD_SUCCESS	/* no error on last loop iteration	*/
         && (rc = asn1r_get_tag (aCtx, &tagnum)) == SD_SUCCESS
         && tagnum != 0)	/* 0 indicates end of constructor	*/
    {
    /* Handle each possible TAG.	*/
    switch (tagnum)
      {
      case TAG_EXTERNAL_SINGLEASN1:
        /* Don't want to decode it. Just get ptr & len & copy the data.*/
        rc = asn1r_get_contents (aCtx, &auth_ptr, &auth_len);
        if (rc == SD_SUCCESS)
          {
          authInfo->u.other_auth.auth_value.len = auth_len;
          authInfo->u.other_auth.auth_value.buf = authInfo->u.other_auth.auth_value.ptr =
                 (ST_UCHAR *) chk_malloc (auth_len);
          memcpy (authInfo->u.other_auth.auth_value.ptr, auth_ptr, auth_len);
          }
        break;
  
      default:				/* Unexpected tag	*/
        ACSELOG_ERR1 ("Decode error: unexpected tag 0x%X.", tagnum);
        rc = ASN1E_UNEXPECTED_TAG;
        break;
      }	/* end "switch (tagnum)"	*/
    }	/* end "while" loop	*/
  return (rc);
  }

/************************************************************************/
/*			acse_dec_authentication				*/
/************************************************************************/
ST_RET acse_dec_authentication (
	ASN1_DEC_CTXT *aCtx,
	ACSE_AUTH_INFO *authInfo)		/* out	*/
  {
ST_UINT16 tagnum;
ST_RET rc = SD_SUCCESS;
ST_INT password_len;
ST_UCHAR *contents_ptr;		/* ptr to ASN.1 contents octets	*/
ST_INT contents_len;		/* len of ASN.1 contents octets	*/

  /* Loop decoding tags until error or end of this constructor.*/
  while (rc == SD_SUCCESS	/* no error on last loop iteration	*/
         && (rc = asn1r_get_tag (aCtx, &tagnum)) == SD_SUCCESS
         && tagnum != 0)	/* 0 indicates end of constructor	*/
    {
    /* Handle each possible TAG.	*/
    switch (tagnum)
      {
      case TAG_AUTH_GRAPHICSTRING:
        /* Mech type must be Password. */
        if (authInfo->mech_type != ACSE_AUTH_MECH_PASSWORD)
          {
          rc = ASN1E_UNEXPECTED_TAG;
          break;
          }
        password_len = ASN1_DEC_LEN(aCtx);
        if (password_len <= ACSE_MAX_LEN_PASSWORD)
          {
          memcpy (authInfo->u.pw_auth.password, ASN1_DEC_PTR(aCtx), password_len);
          authInfo->u.pw_auth.password[password_len] = 0; /* NULL terminate */
          }
        else
          {
          ACSELOG_ERR1 ("_a_dec_auth_value: password too long (%d bytes)", password_len);
          authInfo->u.pw_auth.password[0] = 0;	/* NULL password	*/
          rc = ASN1E_INVALID_LENGTH;
          }
        /* Skip over this element now.	*/
        if (rc == SD_SUCCESS)
          rc = asn1r_get_contents (aCtx, &contents_ptr, &contents_len);
        break;
  
      case TAG_AUTH_EXTERNAL:
        /* Mech type must NOT be Password. */
        if (authInfo->mech_type == ACSE_AUTH_MECH_PASSWORD)
          rc = ASN1E_UNEXPECTED_TAG;
        else
          rc = acse_dec_auth_value_external (aCtx, authInfo);
        break;
  
      default:				/* Unexpected tag	*/
        ACSELOG_ERR1 ("Decode error: unexpected tag 0x%X.", tagnum);
        rc = ASN1E_UNEXPECTED_TAG;
        break;
      }	/* end "switch (tagnum)"	*/
    }	/* end "while" loop	*/
  return (rc);
  }
/************************************************************************/
/*			acse_dec_aarq_2					*/
/* AARQ constructor found. Parse to end of this constructor.		*/
/************************************************************************/
static ST_RET acse_dec_aarq_2 (ASN1_DEC_CTXT *aCtx,
	ST_CHAR pcimms,		/* PCI negotiated for MMS data	*/
	AARQ_APDU *aarq_apdu)	/* out: decoded data	*/
  {
ST_UCHAR bit_str;
MMS_OBJ_ID mech_name;	/* Authentication mechanism name	*/
MMS_OBJ_ID maceMechId = MACE_MECH_ID;
ST_UINT16 tagnum;
ST_RET rc = SD_SUCCESS;
PDV_INFO pdv_info;
ST_UCHAR *contents_ptr;		/* ptr to ASN.1 contents octets	*/
ST_INT contents_len;		/* len of ASN.1 contents octets	*/

  /* Loop decoding tags until error or end of this constructor.*/
  while (rc == SD_SUCCESS	/* no error on last loop iteration	*/
         && (rc = asn1r_get_tag (aCtx, &tagnum)) == SD_SUCCESS
         && tagnum != 0)	/* 0 indicates end of constructor	*/
    {
    /* DEBUG: Should use "asn1r_chk_tag" to check sequence, but	old	*/
    /*        decoder didn't check, so do like old decoder for now.	*/
    switch (tagnum)
      {
      case TAGDEC_AARQ_protocol_version:
        /* CRITICAL: asn1r_get_bitstr doesn't set unused bits, so init all bits=0*/
        bit_str = 0;
        asn1r_get_bitstr (aCtx, &bit_str, 8);
        if ((bit_str & BIT_VERSION1) == 0)		/* NOT Version1	*/
          rc = ASN1E_INVALID_BITSTR;	/* Illegal ACSE version		*/
        break;

      case TAGDEC_AARQ_ASO_context_name:
        aarq_apdu->ASO_context_name_pres = SD_TRUE;
        rc = acse_dec_constr_obj_id (aCtx, &aarq_apdu->ASO_context_name);
        break;

      case TAGDEC_AARQ_CALLED_AP_title:
        aarq_apdu->called_ae_title.AP_title_pres = SD_TRUE;
        rc = acse_dec_constr_obj_id (aCtx, &aarq_apdu->called_ae_title.AP_title);
        break;

      case TAGDEC_AARQ_CALLED_AE_qual:
        aarq_apdu->called_ae_title.AE_qual_pres = SD_TRUE;
        rc = acse_dec_constr_int32 (aCtx, &aarq_apdu->called_ae_title.AE_qual);
        break;

      case TAGDEC_AARQ_CALLED_AP_inv_id:
        aarq_apdu->called_ae_title.AP_inv_id_pres = SD_TRUE;
        rc = acse_dec_constr_int32 (aCtx, &aarq_apdu->called_ae_title.AP_inv_id);
        break;

      case TAGDEC_AARQ_CALLED_AE_inv_id:
        aarq_apdu->called_ae_title.AE_inv_id_pres = SD_TRUE;
        rc = acse_dec_constr_int32 (aCtx, &aarq_apdu->called_ae_title.AE_inv_id);
        break;

      case TAGDEC_AARQ_CALLING_AP_title:
        aarq_apdu->calling_ae_title.AP_title_pres = SD_TRUE;
        rc = acse_dec_constr_obj_id (aCtx, &aarq_apdu->calling_ae_title.AP_title);
        break;

      case TAGDEC_AARQ_CALLING_AE_qual:
        aarq_apdu->calling_ae_title.AE_qual_pres = SD_TRUE;
        rc = acse_dec_constr_int32 (aCtx, &aarq_apdu->calling_ae_title.AE_qual);
        break;

      case TAGDEC_AARQ_CALLING_AP_inv_id:
        aarq_apdu->calling_ae_title.AP_inv_id_pres = SD_TRUE;
        rc = acse_dec_constr_int32 (aCtx, &aarq_apdu->calling_ae_title.AP_inv_id);
        break;

      case TAGDEC_AARQ_CALLING_AE_inv_id:
        aarq_apdu->calling_ae_title.AE_inv_id_pres = SD_TRUE;
        rc = acse_dec_constr_int32 (aCtx, &aarq_apdu->calling_ae_title.AE_inv_id);
        break;

      case TAGDEC_AARQ_acse_requirements:
        /* CRITICAL: asn1r_get_bitstr doesn't set unused bits, so init all bits=0*/
        bit_str = 0;
        asn1r_get_bitstr (aCtx, &bit_str, 8);
        /* DEBUG: some stacks incorrectly set the low bit (0x01) in the	*/
        /* bitstring. To be nice, we will let them get away with it.	*/
        if ((bit_str & BIT_AUTHENTICATION) || (bit_str & 0x01))
          /* This indicates the "Authentication functional unit" is present */
          aarq_apdu->auth_info.auth_pres = SD_TRUE;
        /* Don't support AP context negotiation, so ignore Bit 1.	*/
        /* We will never set Bit 1 in the response.			*/
        break;

    case TAGDEC_AARQ_mechanism_name:
        if (aarq_apdu->auth_info.auth_pres)
          rc = asn1r_get_objid (aCtx, mech_name.comps, &mech_name.num_comps);
        else
          {
          ACSELOG_ERR0 ("AARQ: unexpected 'mechanism-name'");
          rc = ASN1E_INVALID_BITSTR;
          }
        if (rc == SD_SUCCESS)
          {
          /* Save the mechanism name for the user */
          if (asn1_objidcmp (&mech_name, &a_auth_password_obj_id) == 0)
            aarq_apdu->auth_info.mech_type = ACSE_AUTH_MECH_PASSWORD;
          else  
            {
            if (asn1_objidcmp (&mech_name, &maceMechId) == 0)
              aarq_apdu->auth_info.mech_type = ACSE_AUTH_MECH_MACE_CERT;
            else
              aarq_apdu->auth_info.mech_type = ACSE_AUTH_MECH_OTHER;
            memcpy(&aarq_apdu->auth_info.u.other_auth.mech_id, &mech_name, sizeof(MMS_OBJ_ID));
            }
          }
        break;

      case TAGDEC_AARQ_CALLING_auth_value:
        if (aarq_apdu->auth_info.auth_pres)
          rc = acse_dec_authentication (aCtx, &aarq_apdu->auth_info);
        else
          {
          ACSELOG_ERR0 ("AARQ: unexpected 'Calling-authentication-value'");
          rc = ASN1E_INVALID_BITSTR;
          }
        break;

      case TAGDEC_AARQ_user_info:
        rc = acse_dec_user_info (aCtx, pcimms, &pdv_info);
        /* Save decoded user_info.	*/
        aarq_apdu->user_info.ptr = pdv_info.pdv_data_ptr;
        aarq_apdu->user_info.len = pdv_info.pdv_data_len;
        break;
      
      default:				/* Don't know how to decode.	*/
        ACSELOG_DEC1 ("AARQ unrecognized tag %d. Ignored.", tagnum);
        /* Ignore this element.	*/
        rc = asn1r_get_contents (aCtx, &contents_ptr, &contents_len);
        break;
      }	/* end "switch (tagnum)"	*/
    }	/* end "while" loop	*/
  return (rc);
  }

/************************************************************************/
/*			acse_dec_aare_2					*/
/* AARE constructor found. Parse to end of this constructor.		*/
/************************************************************************/
static ST_RET acse_dec_aare_2 (
	ASN1_DEC_CTXT *aCtx,
	ST_CHAR pcimms,		/* PCI negotiated for MMS data	*/
	AARE_APDU *aare_apdu)	/* out: decoded data	*/
  {
ST_UCHAR bit_str;
MMS_OBJ_ID mech_name;	/* Authentication mechanism name	*/
MMS_OBJ_ID maceMechId = MACE_MECH_ID;
ST_UINT16 tagnum;
ST_RET rc = SD_SUCCESS;
PDV_INFO pdv_info;
ST_INT32 int32;			/* tmp var for decoded integer	*/
ST_UCHAR *contents_ptr;		/* ptr to ASN.1 contents octets	*/
ST_INT contents_len;		/* len of ASN.1 contents octets	*/

  /* Loop decoding tags until error or end of this constructor.*/
  while (rc == SD_SUCCESS	/* no error on last loop iteration	*/
         && (rc = asn1r_get_tag (aCtx, &tagnum)) == SD_SUCCESS
         && tagnum != 0)	/* 0 indicates end of constructor	*/
    {
    /* DEBUG: Should use "asn1r_chk_tag" to check sequence, but	old	*/
    /*        decoder didn't check, so do like old decoder for now.	*/
    switch (tagnum)
      {
      case TAGDEC_AARE_protocol_version:
        /* CRITICAL: asn1r_get_bitstr doesn't set unused bits, so init all bits=0*/
        bit_str = 0;
        asn1r_get_bitstr (aCtx, &bit_str, 8);
        if ((bit_str & BIT_VERSION1) == 0)		/* NOT Version1	*/
          rc = ASN1E_INVALID_BITSTR;	/* Illegal ACSE version		*/
        break;

      case TAGDEC_AARE_ASO_context_name:
        aare_apdu->ASO_context_name_pres = SD_TRUE;
        rc = acse_dec_constr_obj_id (aCtx, &aare_apdu->ASO_context_name);
        break;

      case TAGDEC_AARE_result:
        rc = acse_dec_constr_int32 (aCtx, &int32);
        if (rc==SD_SUCCESS)
          aare_apdu->result = (ST_INT16) int32;
        break;

    case TAGDEC_AARE_result_source_diagnostic:
        rc = acse_dec_constr_diag (aCtx, aare_apdu);
      break;

      case TAGDEC_AARE_RESPONDING_AP_title:
        aare_apdu->responding_ae_title.AP_title_pres = SD_TRUE;
        rc = acse_dec_constr_obj_id (aCtx, &aare_apdu->responding_ae_title.AP_title);
        break;

      case TAGDEC_AARE_RESPONDING_AE_qual:
        aare_apdu->responding_ae_title.AE_qual_pres = SD_TRUE;
        rc = acse_dec_constr_int32 (aCtx, &aare_apdu->responding_ae_title.AE_qual);
        break;

      case TAGDEC_AARE_RESPONDING_AP_inv_id:
        aare_apdu->responding_ae_title.AP_inv_id_pres = SD_TRUE;
        rc = acse_dec_constr_int32 (aCtx, &aare_apdu->responding_ae_title.AP_inv_id);
        break;

      case TAGDEC_AARE_RESPONDING_AE_inv_id:
        aare_apdu->responding_ae_title.AE_inv_id_pres = SD_TRUE;
        rc = acse_dec_constr_int32 (aCtx, &aare_apdu->responding_ae_title.AE_inv_id);
        break;

      case TAGDEC_AARE_RESPONDER_acse_requirements:
        /* CRITICAL: asn1r_get_bitstr doesn't set unused bits, so init all bits=0*/
        bit_str = 0;
        asn1r_get_bitstr (aCtx, &bit_str, 8);
        /* DEBUG: some stacks incorrectly set the low bit (0x01) in the	*/
        /* bitstring. To be nice, we will let them get away with it.	*/
        if ((bit_str & BIT_AUTHENTICATION) || (bit_str & 0x01))
          /* This indicates the "Authentication functional unit" is present */
          aare_apdu->auth_info.auth_pres = SD_TRUE;
        /* Don't support AP context negotiation, so ignore Bit 1.		*/
        /* We will never set Bit 1 in the response.			*/
        break;

   
      case TAGDEC_AARE_mechanism_name:
        if (aare_apdu->auth_info.auth_pres)
          rc = asn1r_get_objid (aCtx, mech_name.comps, &mech_name.num_comps);
        else
          {
          ACSELOG_ERR0 ("AARE: unexpected 'mechanism-name'");
          rc = ASN1E_INVALID_BITSTR;
          }
        if (rc == SD_SUCCESS)
          {
          /* Save the mechanism name for the user */
          if (asn1_objidcmp (&mech_name, &a_auth_password_obj_id) == 0)
            aare_apdu->auth_info.mech_type = ACSE_AUTH_MECH_PASSWORD;
          else  
            {
            if (asn1_objidcmp (&mech_name, &maceMechId) == 0)
              aare_apdu->auth_info.mech_type = ACSE_AUTH_MECH_MACE_CERT;
            else
              aare_apdu->auth_info.mech_type = ACSE_AUTH_MECH_OTHER;
            memcpy(&aare_apdu->auth_info.u.other_auth.mech_id, &mech_name, sizeof(MMS_OBJ_ID));
            }
          }
        break;

    case TAGDEC_AARE_RESPONDING_auth_value:
        if (aare_apdu->auth_info.auth_pres)
          rc = acse_dec_authentication (aCtx, &aare_apdu->auth_info);
        else
          {
          ACSELOG_ERR0 ("AARE: unexpected 'Responding-authentication-value'");
          rc = ASN1E_INVALID_BITSTR;
          }
        break;

      case TAGDEC_AARE_user_info:
        rc = acse_dec_user_info (aCtx, pcimms, &pdv_info);
        /* Save decoded user_info.	*/
        aare_apdu->user_info.ptr = pdv_info.pdv_data_ptr;
        aare_apdu->user_info.len = pdv_info.pdv_data_len;
        break;
      
      default:				/* Don't know how to decode.	*/
        ACSELOG_DEC1 ("AARE unrecognized tag %d. Ignored.", tagnum);
        /* Ignore this element.	*/
        rc = asn1r_get_contents (aCtx, &contents_ptr, &contents_len);
        break;
      }	/* end "switch (tagnum)"	*/
    }	/* end "while" loop	*/
  return (rc);
  }

/************************************************************************/
/*			acse_dec_rlrq_2					*/
/* RLRQ constructor found. Parse to end of this constructor.		*/
/************************************************************************/
static ST_RET acse_dec_rlrq_2 (
	ASN1_DEC_CTXT *aCtx,
	ST_CHAR pcimms,		/* PCI negotiated for MMS data	*/
	RLRQ_APDU *rlrq_apdu)	/* out: decoded data	*/
  {
ST_UINT16 tagnum;
ST_RET rc = SD_SUCCESS;
PDV_INFO pdv_info;
ST_UCHAR *contents_ptr;		/* ptr to ASN.1 contents octets	*/
ST_INT contents_len;		/* len of ASN.1 contents octets	*/

  /* Loop decoding tags until error or end of this constructor.*/
  while (rc == SD_SUCCESS	/* no error on last loop iteration	*/
         && (rc = asn1r_get_tag (aCtx, &tagnum)) == SD_SUCCESS
         && tagnum != 0)	/* 0 indicates end of constructor	*/
    {
    /* DEBUG: Should use "asn1r_chk_tag" to check sequence, but	old	*/
    /*        decoder didn't check, so do like old decoder for now.	*/
    switch (tagnum)
      {
      case TAGDEC_RLRQ_reason:
        rlrq_apdu->reason_pres = SD_TRUE;
        rc = asn1r_get_i16 (aCtx, &rlrq_apdu->reason);
        break;

      case TAGDEC_RLRQ_user_info:	/* usually not received	*/
        rc = acse_dec_user_info (aCtx, pcimms, &pdv_info);
        /* Save decoded user_info.	*/
        rlrq_apdu->user_info.ptr = pdv_info.pdv_data_ptr;
        rlrq_apdu->user_info.len = pdv_info.pdv_data_len;
        break;
      
      default:				/* Don't know how to decode.	*/
        ACSELOG_DEC1 ("RLRQ unrecognized tag %d. Ignored.", tagnum);
        /* Ignore this element.	*/
        rc = asn1r_get_contents (aCtx, &contents_ptr, &contents_len);
        break;
      }	/* end "switch (tagnum)"	*/
    }	/* end "while" loop	*/
  return (rc);
  }

/************************************************************************/
/*			acse_dec_rlre_2					*/
/* RLRE constructor found. Parse to end of this constructor.		*/
/************************************************************************/
static ST_RET acse_dec_rlre_2 (
	ASN1_DEC_CTXT *aCtx,
	ST_CHAR pcimms,		/* PCI negotiated for MMS data	*/
	RLRE_APDU *rlre_apdu)	/* out: decoded data	*/
  {
ST_UINT16 tagnum;
ST_RET rc = SD_SUCCESS;
PDV_INFO pdv_info;
ST_UCHAR *contents_ptr;		/* ptr to ASN.1 contents octets	*/
ST_INT contents_len;		/* len of ASN.1 contents octets	*/

  /* Loop decoding tags until error or end of this constructor.*/
  while (rc == SD_SUCCESS	/* no error on last loop iteration	*/
         && (rc = asn1r_get_tag (aCtx, &tagnum)) == SD_SUCCESS
         && tagnum != 0)	/* 0 indicates end of constructor	*/
    {
    /* DEBUG: Should use "asn1r_chk_tag" to check sequence, but	old	*/
    /*        decoder didn't check, so do like old decoder for now.	*/
    switch (tagnum)
      {
      case TAGDEC_RLRE_reason:
        rlre_apdu->reason_pres = SD_TRUE;
        rc = asn1r_get_i16 (aCtx, &rlre_apdu->reason);
        break;

      case TAGDEC_RLRE_user_info:	/* usually not received	*/
        rc = acse_dec_user_info (aCtx, pcimms, &pdv_info);
        /* Save decoded user_info.	*/
        rlre_apdu->user_info.ptr = pdv_info.pdv_data_ptr;
        rlre_apdu->user_info.len = pdv_info.pdv_data_len;
        break;
      
      default:				/* Don't know how to decode.	*/
        ACSELOG_DEC1 ("RLRE unrecognized tag %d. Ignored.", tagnum);
        /* Ignore this element.	*/
        rc = asn1r_get_contents (aCtx, &contents_ptr, &contents_len);
        break;
      }	/* end "switch (tagnum)"	*/
    }	/* end "while" loop	*/
  return (rc);
  }

/************************************************************************/
/*			acse_dec_abrt_2					*/
/* ABRT constructor found. Parse to end of this constructor.		*/
/************************************************************************/
static ST_RET acse_dec_abrt_2 (
	ASN1_DEC_CTXT *aCtx,
	ST_CHAR pcimms,		/* PCI negotiated for MMS data	*/
	ABRT_APDU *abrt_apdu)	/* out: decoded data	*/
  {
ST_UINT16 tagnum;
ST_RET rc = SD_SUCCESS;
PDV_INFO pdv_info;
ST_UCHAR *contents_ptr;		/* ptr to ASN.1 contents octets	*/
ST_INT contents_len;		/* len of ASN.1 contents octets	*/

  /* Loop decoding tags until error or end of this constructor.*/
  while (rc == SD_SUCCESS	/* no error on last loop iteration	*/
         && (rc = asn1r_get_tag (aCtx, &tagnum)) == SD_SUCCESS
         && tagnum != 0)	/* 0 indicates end of constructor	*/
    {
    /* DEBUG: Should use "asn1r_chk_tag" to check sequence, but	old	*/
    /*        decoder didn't check, so do like old decoder for now.	*/
    switch (tagnum)
      {
      case TAGDEC_ABRT_source:
        rc = asn1r_get_i16 (aCtx, &abrt_apdu->source);
        break;

      case TAGDEC_ABRT_diagnostic:	/* usually not received	*/
        abrt_apdu->diagnostic_pres = SD_TRUE;
        rc = asn1r_get_i16 (aCtx, &abrt_apdu->diagnostic);
        break;

      case TAGDEC_ABRT_user_info:	/* usually not received	*/
        rc = acse_dec_user_info (aCtx, pcimms, &pdv_info);
        /* Save decoded user_info.	*/
        abrt_apdu->user_info.ptr = pdv_info.pdv_data_ptr;
        abrt_apdu->user_info.len = pdv_info.pdv_data_len;
        break;
      
      default:				/* Don't know how to decode.	*/
        ACSELOG_DEC1 ("ABRT unrecognized tag %d. Ignored.", tagnum);
        /* Ignore this element.	*/
        rc = asn1r_get_contents (aCtx, &contents_ptr, &contents_len);
        break;
      }	/* end "switch (tagnum)"	*/
    }	/* end "while" loop	*/
  return (rc);
  }

	/*======================================================*/
	/* ACSE USER decode functions below.			*/
	/* User can call these to completely parse any ACSE PDU.*/
	/*======================================================*/

/************************************************************************/
/*			acse_dec_aarq					*/
/************************************************************************/
ST_RET acse_dec_aarq (
	ST_UCHAR *pdu_ptr,	/* PDU pointer		*/
	ST_INT pdu_len,		/* PDU length		*/
	ST_INT8 pcimms,		/* PCI negotiated for MMS data	*/
	AARQ_APDU *aarq_apdu)	/* out: decoded data	*/
  {
ASN1_DEC_CTXT aCtx;
ST_RET rc;
ST_UINT16 tagnum;

  memset (aarq_apdu, 0, sizeof (AARQ_APDU));	/* CRITICAL: start clean*/

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
    case TAGDEC_AARQ:	/* only TAG allowed	*/
      rc = acse_dec_aarq_2 (&aCtx, pcimms, aarq_apdu);
      break;
  
    default:				/* Unexpected tag	*/
      ACSELOG_ERR1 ("Decode error: unexpected tag 0x%X.", tagnum);
      rc = ASN1E_UNEXPECTED_TAG;
      break;
    }	/* end "switch (tagnum)"	*/
  return (rc);
  }

/************************************************************************/
/*			acse_dec_aare					*/
/************************************************************************/
ST_RET acse_dec_aare (
	ST_UCHAR *pdu_ptr,	/* PDU pointer		*/
	ST_INT pdu_len,		/* PDU length		*/
	ST_INT8 pcimms,		/* PCI negotiated for MMS data	*/
	AARE_APDU *aare_apdu)	/* out: decoded data	*/
  {
ASN1_DEC_CTXT aCtx;
ST_RET rc;
ST_UINT16 tagnum;

  memset (aare_apdu, 0, sizeof (AARE_APDU));	/* CRITICAL: start clean*/

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
    case TAGDEC_AARE:	/* only TAG allowed	*/
      rc = acse_dec_aare_2 (&aCtx, pcimms, aare_apdu);
      break;
  
    default:				/* Unexpected tag	*/
      ACSELOG_ERR1 ("Decode error: unexpected tag 0x%X.", tagnum);
      rc = ASN1E_UNEXPECTED_TAG;
      break;
    }	/* end "switch (tagnum)"	*/
  return (rc);
  }

/************************************************************************/
/*			acse_dec_rlrq					*/
/************************************************************************/
ST_RET acse_dec_rlrq (
	ST_UCHAR *pdu_ptr,	/* PDU pointer		*/
	ST_INT pdu_len,		/* PDU length		*/
	ST_INT8 pcimms,		/* PCI negotiated for MMS data	*/
	RLRQ_APDU *rlrq_apdu)	/* out: decoded data	*/
  {
ASN1_DEC_CTXT aCtx;
ST_RET rc;
ST_UINT16 tagnum;

  memset (rlrq_apdu, 0, sizeof (RLRQ_APDU));	/* CRITICAL: start clean*/

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
    case TAGDEC_RLRQ:	/* only TAG allowed	*/
      rc = acse_dec_rlrq_2 (&aCtx, pcimms, rlrq_apdu);
      break;
  
    default:				/* Unexpected tag	*/
      ACSELOG_ERR1 ("Decode error: unexpected tag 0x%X.", tagnum);
      rc = ASN1E_UNEXPECTED_TAG;
      break;
    }	/* end "switch (tagnum)"	*/
  return (rc);
  }

/************************************************************************/
/*			acse_dec_rlre					*/
/************************************************************************/
ST_RET acse_dec_rlre (
	ST_UCHAR *pdu_ptr,	/* PDU pointer		*/
	ST_INT pdu_len,		/* PDU length		*/
	ST_INT8 pcimms,		/* PCI negotiated for MMS data	*/
	RLRE_APDU *rlre_apdu)	/* out: decoded data	*/
  {
ASN1_DEC_CTXT aCtx;
ST_RET rc;
ST_UINT16 tagnum;

  memset (rlre_apdu, 0, sizeof (RLRE_APDU));	/* CRITICAL: start clean*/

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
    case TAGDEC_RLRE:	/* only TAG allowed	*/
      rc = acse_dec_rlre_2 (&aCtx, pcimms, rlre_apdu);
      break;
  
    default:				/* Unexpected tag	*/
      ACSELOG_ERR1 ("Decode error: unexpected tag 0x%X.", tagnum);
      rc = ASN1E_UNEXPECTED_TAG;
      break;
    }	/* end "switch (tagnum)"	*/
  return (rc);
  }

/************************************************************************/
/*			acse_dec_abrt					*/
/************************************************************************/
ST_RET acse_dec_abrt (
	ST_UCHAR *pdu_ptr,	/* PDU pointer		*/
	ST_INT pdu_len,		/* PDU length		*/
	ST_INT8 pcimms,		/* PCI negotiated for MMS data	*/
	ABRT_APDU *abrt_apdu)	/* out: decoded data	*/
  {
ASN1_DEC_CTXT aCtx;
ST_RET rc;
ST_UINT16 tagnum;

  memset (abrt_apdu, 0, sizeof (ABRT_APDU));	/* CRITICAL: start clean*/

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
    case TAGDEC_ABRT:	/* only TAG allowed	*/
      rc = acse_dec_abrt_2 (&aCtx, pcimms, abrt_apdu);
      break;
  
    default:				/* Unexpected tag	*/
      ACSELOG_ERR1 ("Decode error: unexpected tag 0x%X.", tagnum);
      rc = ASN1E_UNEXPECTED_TAG;
      break;
    }	/* end "switch (tagnum)"	*/
  return (rc);
  }

