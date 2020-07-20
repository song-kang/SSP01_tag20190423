/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1996, All Rights Reserved.		        	*/
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : tp4_enc.c						*/
/* PRODUCT(S)  : Lean-T Stack						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*		This module implements the encoding of the TPDUs.	*/
/*									*/
/*  For information see the:						*/
/*	ISO 8073 "Information processing systems - Open Systems		*/
/*	Interconnections - Connection oriented transport protocol	*/
/*	specification.							*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/*			tp_encode_cx					*/
/*			tp_encode_dr					*/
/*			tp_encode_dc					*/
/*			tp_encode_dt					*/
/*			tp0_encode_dt					*/
/*			tp_encode_ak					*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 08/13/98  JRB    04     Fix indenting.				*/
/* 08/26/97  JRB    03     RFC1006 changes:				*/
/*			   - add TP0 encode				*/
/*			   - tp_encode_cx: if TPDU size = 65531, don't	*/
/*			     encode it. Peer should default to 65531.	*/
/*			   - Use TP_PDU_TYPE_* everywhere.		*/
/* 05/27/97  JRB   7.00    MMSEASE 7.0 release.				*/
/* 10/04/96  JRB    02     Use max_tpdu_len_enc (binary encoded value).	*/
/* 06/20/96  EJV    01     Created					*/
/************************************************************************/
static char *thisFileName = __FILE__;	/* For TP4_EXCEPT and slog macros*/

#include "glbtypes.h"
#include "sysincs.h"
#include "checksum.h"
#include "tp4api.h"		/* User definitions for tp4	*/
#include "tp4.h"
#include "tp4_encd.h"
#include "tp4_log.h"



/* local functions prototypes */

static ST_VOID tp_enc_par_checksum (ST_UCHAR *buf, ST_UINT16 buf_len, ST_UINT16 par_pos);



/************************************************************************/
/*			tp_encode_cx					*/
/*----------------------------------------------------------------------*/
/* This function is used to encode a CR or CC TPDU.			*/
/*									*/
/* Parameters:								*/
/*	ST_UCHAR	*enc_buf	Pointer where to encode the CX TPDU	*/
/*	TPDU_CX *tpdu_cx	Pointer to info struct for CX TPDU	*/
/*	ST_UCHAR  pdu_type	TP_PDU_TYPE_CR or  TP_PDU_TYPE_CC	*/
/*									*/
/* Return:								*/
/*	ST_UINT16			The actual length of encoded TPDU	*/
/************************************************************************/
ST_UINT16 tp_encode_cx (ST_UCHAR *enc_buf, TPDU_CX *tpdu_cx, ST_UCHAR pdu_type)
{
ST_UINT16		enc_len;	/* The actual length of encoded TPDU	*/
TP_FHDR_CX	*pdu;
TP_PAR		*param;		/* generic struct for a parameter	*/
ST_UINT16		 idx;		/* points to current offset into enc_buf*/

  enc_len = 0;

  /*------------------------------------*/
  /* encode TPDU header, fixed part	*/
  /*------------------------------------*/

  pdu = (TP_FHDR_CX *) enc_buf;

  /* compute actual header length (do not count the hdr_len byte)	*/
  if (tpdu_cx->preferred_class == 0)
    pdu->hdr_len = 
     (ST_UCHAR) (TP_FHDR_CX_LEN - sizeof(pdu->hdr_len)+	/* fixed part of header	*/
	    TP_PAR_FIX_LEN + tpdu_cx->loc_tsap [0] +	/* local TSAP parameter	*/
	    TP_PAR_FIX_LEN + tpdu_cx->rem_tsap [0]);	/* remote TSAP parameter*/
  else
    pdu->hdr_len =
     (ST_UCHAR) (TP_FHDR_CX_LEN - sizeof(pdu->hdr_len)+	/* fixed part of header	*/
	    TP_PAR_FIX_LEN + tpdu_cx->loc_tsap [0] +	/* local TSAP parameter	*/
	    TP_PAR_FIX_LEN + tpdu_cx->rem_tsap [0] +	/* remote TSAP parameter*/
	    TP_PAR_LEN_VERSION +      			/* TP version parameter	*/
	    TP_PAR_LEN_ADD_OPTIONS +			/* additional opt param	*/
	    TP_PAR_LEN_CHECKSUM);     			/* checksum parameter	*/
  /* NOTE: if TPDU size gets encoded hdr_len must be increased.	*/

  /* set TPDU type */
  if (pdu_type == TP_PDU_TYPE_CR  ||  pdu_type == TP_PDU_TYPE_CC)
    pdu->type_cdt = pdu_type;
  else
    {
    TP_LOG_ERR1 ("TP-ERROR: Encode CR or CC failed (invalid pdu_type %u passed to func)",
      pdu_type);
    TP4_EXCEPT ();
    return (enc_len); 		/* valid types are CR and CC		*/
    }

  /* set CDT, if specified CDT is greater then max default to max */
  if (tpdu_cx->cdt <= TP_MAX_CDT)
    pdu->type_cdt |= tpdu_cx->cdt;
  else
    pdu->type_cdt |= TP_MAX_CDT;

  /* copy to dest ref remote connection reference */
  if (pdu_type == TP_PDU_TYPE_CR)
    pdu->dest_ref [0] = pdu->dest_ref [1] = 0;		/* always 0	*/
  else
    memcpy (pdu->dest_ref, (ST_UCHAR *) &tpdu_cx->rem_ref, sizeof (pdu->dest_ref));

  /* copy to src ref local connection reference	*/
  memcpy (pdu->src_ref, (ST_UCHAR *) &tpdu_cx->loc_ref, sizeof (pdu->src_ref));

  if (tpdu_cx->preferred_class == 4)
    pdu->class_opt = TP_CLASS_4_NORMAL;	/* set the TP class		*/
  else if (tpdu_cx->preferred_class == 0)
    pdu->class_opt = TP_CLASS_0;	/* set the TP class		*/
  else
    {
    /* only class 4 (normal format) implemented */
    TP_LOG_ERR1 ("TP-ERROR: Encode CR or CC failed (not implemented class %u)",
      tpdu_cx->preferred_class);
    TP4_EXCEPT ();
    return (enc_len);
    }

  idx = TP_FHDR_CX_LEN;

  /*------------------------------------*/
  /* encode calling TSAP parameter	*/
  /*------------------------------------*/

  param = (TP_PAR *) &enc_buf [idx];

  param->code = TP_PAR_CODE_TSAP_CALLING;  	/* parameter code	*/
  if (pdu_type == TP_PDU_TYPE_CR)
    {
    /* Copy len and local TSAP to enc_buf */
    memcpy (&param->len, tpdu_cx->loc_tsap, 1 + tpdu_cx->loc_tsap [0]);
    idx += (ST_UINT16) (TP_PAR_FIX_LEN + tpdu_cx->loc_tsap [0]);
    }
  else
    {
    /* for CC copy len and remote TSAP to enc_buf */
    memcpy (&param->len, tpdu_cx->rem_tsap, 1 + tpdu_cx->rem_tsap [0]);
    idx += (ST_UINT16) (TP_PAR_FIX_LEN + tpdu_cx->rem_tsap [0]);
    }

  /*------------------------------------*/
  /* encode called TSAP parameter	*/
  /*------------------------------------*/

  param = (TP_PAR *) &enc_buf [idx];

  param->code = TP_PAR_CODE_TSAP_CALLED;  	/* parameter code	*/
  if (pdu_type == TP_PDU_TYPE_CR)
    {
    /* Copy len and remote TSAP to enc_buf */
    memcpy (&param->len, tpdu_cx->rem_tsap, 1 + tpdu_cx->rem_tsap [0]);
    idx += (ST_UINT16) (TP_PAR_FIX_LEN + tpdu_cx->rem_tsap [0]);
    }
  else
    {
    /* for CC copy len and local TSAP to enc_buf	*/
    memcpy (&param->len, tpdu_cx->loc_tsap, 1 + tpdu_cx->loc_tsap [0]);
    idx += (ST_UINT16) (TP_PAR_FIX_LEN + tpdu_cx->loc_tsap [0]);
    }

  /*------------------------------------*/
  /* encode TPDU size parameter		*/
  /*------------------------------------*/

  /* Can't encode 65531. If not encoded, other side defaults to 65531.	*/
  if (tpdu_cx->max_tpdu_len_enc != TP_PDU_MAX_SIZE_65531)
    {
    pdu->hdr_len += TP_PAR_LEN_TPDU_SIZE;	/* Adjust "hdr_len"	*/

    param = (TP_PAR *) &enc_buf [idx];

    param->code = TP_PAR_CODE_TPDU_SIZE;  /* parameter code	*/
    param->len = 1;			/* parameter len	*/

    if (tpdu_cx->max_tpdu_len_enc < TP_PDU_MAX_SIZE_128  ||
        tpdu_cx->max_tpdu_len_enc > TP_PDU_MAX_SIZE_8192)
      {
      TP_LOG_ERR1 ("TP-ERROR: Encode CR or CC failed (invalid TPDU size %u)",
        tpdu_cx->max_tpdu_len_enc);
      TP4_EXCEPT ();
      return (enc_len);
      }
    else
      param->value[0] = tpdu_cx->max_tpdu_len_enc;

    idx += TP_PAR_LEN_TPDU_SIZE;
    }

  /*------------------------------------*/
  /* encode TP version			*/
  /*------------------------------------*/

  if (tpdu_cx->preferred_class != 0)
    {
    param = (TP_PAR *) &enc_buf [idx];

    param->code = TP_PAR_CODE_VERSION;  	/* parameter code	*/
    param->len = 1;			/* parameter len	*/
    param->value [0] = TP_VERSION; 	/* version number	*/

    idx += TP_PAR_LEN_VERSION;
    }

  /*------------------------------------*/
  /* encode TP additional options	*/
  /*------------------------------------*/

  if (tpdu_cx->preferred_class != 0)
    {
    param = (TP_PAR *) &enc_buf [idx];

    param->code = TP_PAR_CODE_ADD_OPTIONS;/* parameter code	*/
    param->len = 1;			/* parameter len	*/
    param->value [0] = TP_OPT_CHECKSUM_USE | TP_OPT_EXPEDITED_NON_USE;
  			/* this param is 0 for our selected options */
    idx += TP_PAR_LEN_ADD_OPTIONS;
    /* next param is checksum	*/
    }

  /*-----------------------------------------------------*/
  /* copy the user data (if any) after the last parameter*/
  /* (in our case the last param will be checksum param) */
  /*-----------------------------------------------------*/

  enc_len = (ST_UINT16) (pdu->hdr_len+1);	/* set the TPDU size and add to it	*/
					/* user data len (if any); add 1 for the*/
					/* len byte we didn't counted in hdr_len*/
  if (tpdu_cx->udata_len > 0)
    {
    if (tpdu_cx->preferred_class == 4)	/* leave room for checksum	*/
      memcpy (&enc_buf [idx + TP_PAR_LEN_CHECKSUM], tpdu_cx->udata_buf, tpdu_cx->udata_len);
    else
      memcpy (&enc_buf [idx], tpdu_cx->udata_buf, tpdu_cx->udata_len);

    enc_len += tpdu_cx->udata_len;
    }

  /*------------------------------------*/
  /* encode the checksum parameter	*/
  /*------------------------------------*/

  if (tpdu_cx->preferred_class == 4)
    {
    tp_enc_par_checksum (enc_buf, enc_len, idx);
    }
  return (enc_len);
}


/************************************************************************/
/*			tp_encode_dr					*/
/*----------------------------------------------------------------------*/
/* This function is used to encode a DR TPDU.				*/
/*									*/
/* Parameters:								*/
/*	ST_UCHAR	*enc_buf	Pointer where to encode the DR TPDU	*/
/*	TPDU_DR *tpdu_dr	Pointer to info struct for DR TPDU	*/
/*									*/
/* Return:								*/
/*	ST_UINT16			The actual length of encoded TPDU	*/
/************************************************************************/
ST_UINT16 tp_encode_dr (ST_UCHAR *enc_buf, TPDU_DR *tpdu_dr)
{
ST_UINT16		 enc_len;	/* The actual length of encoded TPDU	*/
TP_FHDR_DR	*pdu;

  enc_len = TP_MAX_LEN_DR;

  /*------------------------------------*/
  /* encode the TPDU header, fixed part	*/
  /*------------------------------------*/

  pdu = (TP_FHDR_DR *) enc_buf;

  pdu->hdr_len = TP_MAX_LEN_DR - sizeof (pdu->hdr_len);
  					/* header length		*/
					/* don't count the hdr_len byte	*/
  pdu->type = TP_PDU_TYPE_DR;		/* TPDU type			*/
  memcpy (pdu->dest_ref, (ST_UCHAR *) &tpdu_dr->rem_ref, sizeof (pdu->dest_ref));
					/* Remote connection reference	*/
  memcpy (pdu->src_ref, (ST_UCHAR *) &tpdu_dr->loc_ref, sizeof (pdu->src_ref));
					/* Local connection reference	*/
  pdu->reason = tpdu_dr->reason;	/* reason for DR		*/

  /*------------------------------------*/
  /* encode the checksum parameter	*/
  /*------------------------------------*/

  tp_enc_par_checksum (enc_buf,		/* buf on which checksum is computed	*/
  		       enc_len,		/* len of buffer (length of TPDU)	*/
		       TP_FHDR_DR_LEN);	/* position of checksum param in enc_buf*/

  return (enc_len);
}


/************************************************************************/
/*			tp_encode_dc					*/
/*----------------------------------------------------------------------*/
/* This function is used to encode a DC TPDU.				*/
/*									*/
/* Parameters:								*/
/*	ST_UCHAR	*enc_buf	Pointer where to encode the DC TPDU	*/
/*	TPDU_DR *tpdu_dc	Pointer to info struct for DC TPDU	*/
/*									*/
/* Return:								*/
/*	ST_UINT16			The actual length of encoded TPDU	*/
/************************************************************************/
ST_UINT16 tp_encode_dc (ST_UCHAR *enc_buf, TPDU_DC *tpdu_dc)
{
ST_UINT16		 enc_len;	/* The actual length of encoded TPDU	*/
TP_FHDR_DC 	*pdu;

  enc_len = TP_MAX_LEN_DC;

  /*------------------------------------*/
  /* encode the TPDU header, fixed part	*/
  /*------------------------------------*/

  pdu = (TP_FHDR_DC *) enc_buf;

  pdu->hdr_len = TP_MAX_LEN_DC - sizeof (pdu->hdr_len);
  					/* header length		*/
					/* don't count the hdr_len byte	*/
  pdu->type = TP_PDU_TYPE_DC;		/* TPDU type			*/
  memcpy (pdu->dest_ref, (ST_UCHAR *) &tpdu_dc->rem_ref, sizeof (pdu->dest_ref));	
					/* Remote connection reference	*/
  memcpy (pdu->src_ref, (ST_UCHAR *) &tpdu_dc->loc_ref, sizeof (pdu->src_ref));
					/* Local connection reference	*/

  /*------------------------------------*/
  /* encode the checksum parameter	*/
  /*------------------------------------*/

  tp_enc_par_checksum (enc_buf,		/* buf on which checksum is computed	*/
  		       enc_len,		/* len of buffer (length of TPDU)	*/
		       TP_FHDR_DC_LEN);	/* position of checksum param in enc_buf*/
  return (enc_len);
}


/************************************************************************/
/*			tp_encode_dt					*/
/*----------------------------------------------------------------------*/
/* This function is used to encode a DT TPDU.				*/
/*									*/
/* Parameters:								*/
/*	ST_UCHAR	*enc_buf	Pointer to buffer where the TPDU should	*/
/*				be encoded.				*/
/*	TPDU_DT *tpdu_dt	Pointer data info structure		*/
/*									*/
/* Return:								*/
/*	ST_UINT16			The actual length of encoded TPDU	*/
/************************************************************************/
ST_UINT16 tp_encode_dt (ST_UCHAR *enc_buf, TPDU_DT *tpdu_dt)
{
ST_UINT16		 enc_len;	/* The actual length of encoded TPDU	*/
TP_FHDR_DT 	*pdu;

  enc_len = (ST_UINT16) (TP_HEAD_LEN_DT + tpdu_dt->udata_len);

  /*------------------------------------*/
  /* encode the TPDU header, fixed part	*/
  /*------------------------------------*/

  pdu = (TP_FHDR_DT *) enc_buf;

  pdu->hdr_len = TP_HEAD_LEN_DT - sizeof (pdu->hdr_len);
  					/* header length		*/
					/* don't count the hdr_len byte	*/
  pdu->type = TP_PDU_TYPE_DT;		/* TPDU type			*/
  memcpy (pdu->dest_ref, (ST_UCHAR *) &tpdu_dt->dst_ref, sizeof (pdu->dest_ref));
					/* Remote connection reference	*/

  /* set sequence number (make sure that bit 8 is not set) */
  pdu->eot_sn = (ST_UCHAR) (tpdu_dt->sn & TP_PDU_MASK_EOF);
  if (tpdu_dt->eot)
    pdu->eot_sn |= TP_PDU_EOF;		/* set EOT flag			*/

  /*------------------------------------*/
  /* copy the udata to enc_buf		*/
  /*------------------------------------*/

  memcpy (&enc_buf [TP_HEAD_LEN_DT], tpdu_dt->udata_ptr, tpdu_dt->udata_len);

  /*------------------------------------*/
  /* encode the checksum parameter	*/
  /*------------------------------------*/

  tp_enc_par_checksum (enc_buf,		/* buf on which checksum is computed	*/
  		       enc_len,		/* len of buffer (length of TPDU)	*/
		       TP_FHDR_DT_LEN);	/* position of checksum param in enc_buf*/
  return (enc_len);
}

/************************************************************************/
/*			tp0_encode_dt					*/
/* This function is used to encode a Class 0 DT TPDU.			*/
/* Same as tp_encode_dt except dest_ref and Checksum NOT encoded.	*/
/************************************************************************/
ST_UINT16 tp0_encode_dt (ST_UCHAR *enc_buf, TPDU_DT *tpdu_dt)
{
ST_UINT16	enc_len;	/* The actual length of encoded TPDU	*/
TP0_FHDR_DT 	*tp0_fhdr_dt;

  enc_len = (ST_UINT16) (TP0_HEAD_LEN_DT + tpdu_dt->udata_len);

  /*------------------------------------*/
  /* encode the TPDU header, fixed part	*/
  /*------------------------------------*/

  tp0_fhdr_dt = (TP0_FHDR_DT *) enc_buf;

  tp0_fhdr_dt->hdr_len = TP0_HEAD_LEN_DT - 1;	/* header length	*/
					/* don't count the hdr_len byte	*/
  tp0_fhdr_dt->type = TP_PDU_TYPE_DT;	/* TPDU type			*/

  /* set sequence number (make sure that bit 8 is not set) */
  tp0_fhdr_dt->eot_sn = (ST_UCHAR) 0;	/* sequence number always 0	*/
  if (tpdu_dt->eot)
    tp0_fhdr_dt->eot_sn |= TP_PDU_EOF;	/* set EOT flag			*/

  /*------------------------------------*/
  /* copy the udata to enc_buf		*/
  /*------------------------------------*/

  memcpy (&enc_buf [TP0_HEAD_LEN_DT], tpdu_dt->udata_ptr, tpdu_dt->udata_len);
  return (enc_len);
}

/************************************************************************/
/*			tp_encode_ak					*/
/*----------------------------------------------------------------------*/
/* This function is used to encode an AK TPDU.				*/
/*									*/
/* Parameters:								*/
/*	ST_UCHAR	*enc_buf	Pointer to buffer where the TPDU should	*/
/*				be encoded.				*/
/*	TPDU_AK *tpdu_ak	Pointer to AK info struct		*/
/*									*/
/* Return:								*/
/*	ST_UINT16			The actual length of encoded TPDU	*/
/************************************************************************/
ST_UINT16 tp_encode_ak (ST_UCHAR *enc_buf, TPDU_AK *tpdu_ak)
{
ST_UINT16		 enc_len;	/* The actual length of encoded TPDU	*/
TP_FHDR_AK 	*pdu;

  enc_len = TP_MAX_LEN_AK;

  /*------------------------------------*/
  /* encode the TPDU header, fixed part	*/
  /*------------------------------------*/

  pdu = (TP_FHDR_AK *) enc_buf;

  pdu->hdr_len = TP_MAX_LEN_AK - sizeof (pdu->hdr_len);
  					/* header length		*/
					/* don't count the hdr_len byte	*/
  pdu->type_cdt = TP_PDU_TYPE_AK;	/* TPDU type			*/

  /* encode the CDT (from cfg struct) */
  if (tpdu_ak->cdt <= TP_MAX_CDT)
    pdu->type_cdt |= tpdu_ak->cdt;
  else
    pdu->type_cdt |= TP_MAX_CDT;
  memcpy (pdu->dest_ref, (ST_UCHAR *) &tpdu_ak->dst_ref, sizeof (pdu->dest_ref));
					/* Remote connection reference	*/
  pdu->sn = tpdu_ak->sn;		/* set next expected seq number	*/

  /*------------------------------------*/
  /* encode the checksum parameter	*/
  /*------------------------------------*/

  tp_enc_par_checksum (enc_buf,		/* buf on which checksum is computed	*/
  		       enc_len,		/* len of buffer (length of TPDU)	*/
		       TP_FHDR_AK_LEN);	/* position of checksum param in enc_buf*/
  return (enc_len);
}


/*======================================================================*/
/*		LOCAL FUNCTIONS for ENCODING				*/
/*======================================================================*/


/************************************************************************/
/*			tp_enc_par_checksum				*/
/*----------------------------------------------------------------------*/
/* This function is used to encode the checksum parameter in TPDU.	*/
/*									*/
/* Parameters:								*/
/*	ST_UCHAR	*buf		Ptr to encoding buffer on which checksum*/
/*				will be computed.			*/
/*	ST_UINT16	buf_len		Length of the buffer (TPDU).		*/
/*	ST_UINT16	par_pos		Position of checksum parameter in buf.	*/
/*									*/
/* Return:								*/
/*	ST_VOID			none					*/
/************************************************************************/
static ST_VOID tp_enc_par_checksum (ST_UCHAR *buf, ST_UINT16 buf_len, ST_UINT16 par_pos)
{
TP_PAR *param;

  param = (TP_PAR *) &buf [par_pos];

  param->code = TP_PAR_CODE_CHECKSUM;		/* parameter code	*/
  param->len = 2;				/* Parameter length	*/

  /* compute the checksum of the entire TPDU, the position of the	*/
  /* checksum first byte in the buf is computed as the difference	*/
  /* between pointers							*/

  checksum_insert (buf, buf_len, (ST_UINT16) (param->value - buf));
}
