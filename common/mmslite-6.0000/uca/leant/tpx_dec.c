/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	1996-2008, All Rights Reserved.					*/
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : tpx_dec.c						*/
/* PRODUCT(S)  : Lean-T Stack						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*		This module implements the decoding of the TPDUs.	*/
/*									*/
/*  For information see the:						*/
/*	ISO 8073 "Information processing systems - Open Systems		*/
/*	Interconnections - Connection oriented transport protocol	*/
/*	specification.							*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/*			tp_decode_nsdu					*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 10/23/08  JRB    11     Add casts to fix warnings.			*/
/* 10/22/08  JRB    10     Add many length checks & return immediately	*/
/*			   on any error.				*/
/*			   Allow any class in CR, CC (caller checks it).*/
/*			   Chg decode of DC, DT variable part like others*/
/*			   Chg tp_decode_er to really decode ER TPDU.	*/
/*			   Delete tp_decode_rj (not used in TP0 or TP4)	*/
/*			   Fix byte order in logging of dst_ref, src_ref*/
/* 11/01/07  EJV    09     Chg TSAP to TSEL in slogs.			*/
/*			   tp_log_tsdu: added mutex.			*/
/* 08/29/00  JRB    08     Don't return error if checksum NOT received	*/
/*			   in DR or DC (maybe not negotiated yet).	*/
/* 04/25/00  JRB    07     Lint cleanup.				*/
/* 06/08/99  JRB    06     Chg src_ref, dst_ref logs to HEX.		*/
/* 08/13/98  JRB    05     Lint cleanup.				*/
/* 01/16/98  JRB    04     Chg unrecognixed param logs to "IND" logs.	*/
/* 12/04/97  JRB    03     Clarify casts. Log unrecognized params.	*/
/* 07/31/97  JRB    02     tp_decode_cx: pass default max_tpdu_len_enc	*/
/*			   as arg because TP0/RFC1006 and TP4 need	*/
/*			   different defaults.				*/
/* 07/31/97  JRB    01     Created. Moved all tp_decode_* functions	*/
/*			   from tp4_dec.c to here and changed arguments	*/
/*			   so TP0 can use them too.			*/
/************************************************************************/
#include "glbtypes.h"
#include "sysincs.h"
#include "checksum.h"
#include "tp4.h"
#include "tp4_encd.h"
#include "tp4_log.h"

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/* local functions prototypes */

static ST_BOOLEAN tp_checksum_verified (ST_UCHAR *buf, ST_UINT16 buf_len, ST_UINT16 par_pos);

/************************************************************************/
/*			get_hdr_len					*/
/* Get the total length of the TPDU header.				*/
/************************************************************************/
static ST_UINT16 get_hdr_len (ST_UCHAR *pdu_ptr, ST_UINT16 pdu_len, ST_UINT16 min_len)
  {
ST_UINT16 hdr_len;
  if (pdu_len < min_len)
    return (0);	/* error	*/
  /* First byte is always header len but doesn't include itself, so add 1.*/
  hdr_len = (ST_UINT16) pdu_ptr[0] + 1;
  if (hdr_len < min_len || hdr_len > pdu_len)
    return (0);	/* error	*/
  return (hdr_len);
  } 
/************************************************************************/
/*			get_next_param					*/
/* Must have at least 2 bytes to get param len. If 2 bytes available,	*/
/* get param len (2nd byte) and make sure it doesn't go past end of	*/
/* header.								*/
/* WARNING: Assumes (len_proc < hdr_len). Caller must check.		*/
/************************************************************************/
static TP_PAR *get_next_param (ST_UCHAR *pdu_ptr,
	ST_UINT16 len_proc,	/* num bytes already processed	*/
	ST_UINT16 hdr_len)	/* num bytes in header		*/
  {
ST_UCHAR *cur_ptr;
ST_UINT16 len_left;		/* num bytes remaining in header	*/

  /* len_proc < hdr_len, so this calc can't be messed up by underflow	*/
  len_left = hdr_len - len_proc;
  cur_ptr = pdu_ptr + len_proc;
  if (len_left < 2 ||
      (ST_UINT16) cur_ptr[1] + 2 > len_left)	/* cur_ptr[1]=param len*/
    return (NULL);
  return ((TP_PAR *) cur_ptr);
  }

/************************************************************************/
/*			tp_decode_cx					*/
/*----------------------------------------------------------------------*/
/* Function used to decode a received CR or CC TPDU. Because n_unitdata	*/
/* may contain multiple TPDUs, the offset parameter will tell us where	*/
/* the CR or CC TPDU starts in the data_buf (n_unitdata).		*/
/*									*/
/* NOTE: Default value of "max_tpdu_len_enc" is passed as arg because	*/
/*       TP4 and TP0/RFC1006 need different defaults.			*/
/*									*/
/* Parameters:								*/
/*	TPDU_CX    *tpdu_cx_rx	Struct to fill in with decoded info.	*/
/*	ST_UCHAR   *pdu_ptr	Pointer to TPDU.			*/
/*	ST_UINT16  pdu_len	Len of TPDU.				*/
/*	ST_UCHAR   pdu_type	TP_PDU_TYPE_CR or TP_PDU_TYPE_CC	*/
/*	ST_UCHAR   max_tpdu_len_enc	Binary encoded Max TPDU len	*/
/*									*/
/* Return:								*/
/*	ST_UINT16		Length of CR or CC TPDU.		*/
/*	0			if decoding failed			*/
/* NOTE: many ways to fail so return 0 IMMEDIATELY after any error.	*/
/************************************************************************/
ST_UINT16 tp_decode_cx (TPDU_CX *tpdu_cx_rx, ST_UCHAR *pdu_ptr,
            ST_UINT16 pdu_len, ST_UCHAR pdu_type, ST_UCHAR def_max_tpdu_len_enc)
{
TP_FHDR_CX	*pdu;
ST_UINT16		 len_proc;	/* length of CR or CC TPDU  processed	*/
TP_PAR		*param;
ST_BOOLEAN		 checksum_present;
ST_UINT16  hdr_len;

  hdr_len = get_hdr_len (pdu_ptr, pdu_len, TP_FHDR_CX_LEN);
  if (hdr_len == 0)
    {
    TP_LOG_ERR0 ("TPDU-CR.ind or CC.ind invalid header len. TPDU Ignored.");
    return (0);
    }
  
  checksum_present = SD_FALSE;	/* it is error if checksum is not	*/
				/* present in CR or TPDU			*/

  /* make sure the struct is cleared */
  memset (tpdu_cx_rx, 0, sizeof (TPDU_CX));

  pdu = (TP_FHDR_CX *) pdu_ptr;

  /* everything in data_buf starting from offset should be the CR or CC	*/

  /* Set TPDU size to default in case not encoded.	*/
  tpdu_cx_rx->max_tpdu_len_enc = def_max_tpdu_len_enc;

  /*----------------------------------------------------*/
  /* Process the fixed part of CR or CC TPDU header	*/
  /*----------------------------------------------------*/

  tpdu_cx_rx->cdt = (ST_UCHAR) (pdu->type_cdt & TP_PDU_MASK_TYPE);
						/* remote CDT		*/
  tpdu_cx_rx->loc_ref = *((ST_UINT16 *) pdu->dest_ref);	/* local conn reference	*/
  tpdu_cx_rx->rem_ref = *((ST_UINT16 *) pdu->src_ref); 	/* remote conn reference*/

  /* get the preferred class option */
  /* Class is in high nibble of class_opt.	*/
  tpdu_cx_rx->preferred_class = pdu->class_opt >> 4;

  /*----------------------------------------------------*/
  /* Process the variable part of CR or CC TPDU header.	*/
  /* Extract parameters in which we are interested into	*/
  /* the tpdu_cx_rx.					*/
  /* The ISO 8073 states that is if a parameter is not	*/
  /* present in CR or CC and the standard defines a 	*/
  /* default value for it, then the default value should*/
  /* be used (equivalent to present parameter).		*/
  /* Following parameters have default value:		*/
  /* 	TPDU size:		128 bytes		*/
  /*    TP version		0x01			*/
  /*	Additional options	0x01 (checksum used and	*/
  /*				      support expedited	*/
  /*				      data transfer)	*/	
  /*----------------------------------------------------*/

  len_proc = TP_FHDR_CX_LEN;	/* we processed already the fixed part	*/

  while (len_proc < hdr_len)
    {
    if ((param = get_next_param (pdu_ptr, len_proc, hdr_len)) == NULL)
      {
      TP_LOG_ERR0 ("TPDU-CR.ind or CC.ind invalid param len. TPDU Ignored.");
      return (0);
      }

    switch (param->code)
      {
      case TP_PAR_CODE_TSAP_CALLING:	/* copy the len & remote TSAP */
	if (param->len <= MAX_TSEL_LEN)
	  {
	  if (pdu_type == TP_PDU_TYPE_CR)
	    /* CR TPDU, we are called (remote TSAP in this field) */
            memcpy (tpdu_cx_rx->rem_tsap, (ST_CHAR *) &param->len, 1 + param->len);
	  else
	    /* CC TPDU, we are calling (local TSAP in this field) */
            memcpy (tpdu_cx_rx->loc_tsap, (ST_CHAR *) &param->len, 1 + param->len);
	  }
	else
	  {
          TP_LOG_ERR1 ("TP-ERROR: decode CR or CC TPDU failed (TSEL length %u not supported)",
	    (unsigned) param->len);
          return (0);		/* bad TSAP length, ignore this TPDU	*/
	  }
      break;

      case TP_PAR_CODE_TSAP_CALLED:
        /* copy the length and local TSAP */
	if (param->len <= MAX_TSEL_LEN)
	  {
	  if (pdu_type == TP_PDU_TYPE_CR)
	    /* CR TPDU, we are called (local TSAP in this field) */
            memcpy (tpdu_cx_rx->loc_tsap, (ST_CHAR *) &param->len, 1 + param->len);
	  else
	    /* CC TPDU, we are calling (remote TSAP in this field) */
            memcpy (tpdu_cx_rx->rem_tsap, (ST_CHAR *) &param->len, 1 + param->len);
	  }
	else
	  {
          TP_LOG_ERR1 ("TP-ERROR: decode CR or CC TPDU failed (TSEL length %u not supported)",
	    (unsigned) param->len);
          return (0);		/* bad TSAP length, ignore this TPDU	*/
	  }
      break;

      case TP_PAR_CODE_TPDU_SIZE:
        if (param->len != 1)
          {
          TP_LOG_ERR0 ("TPDU-CR.ind or CC.ind invalid param len. TPDU Ignored.");
          return (0);
          }
        if (param->value [0] < TP_PDU_MAX_SIZE_128  ||
            param->value [0] > TP_PDU_MAX_SIZE_8192)
	  {
          TP_LOG_ERR1 ("TP-ERROR: decode CR TPDU failed (invalid TPDU size %u)",
            (unsigned) param->value[0]);
          return (0);	/* invalid TPDU size, ignore this TPDU	*/
	  }
        else
          tpdu_cx_rx->max_tpdu_len_enc = param->value [0];
      break;

      case TP_PAR_CODE_VERSION:
        if (param->len != 1)
          {
          TP_LOG_ERR0 ("TPDU-CR.ind or CC.ind invalid param len. TPDU Ignored.");
          return (0);
          }
        if (param->value [0] != TP_VERSION)
	  {
          TP_LOG_ERR1 ("TP-ERROR: decode CR TPDU failed (invalid TPDU version %u)",
	    (unsigned) param->value[0]);
          return (0);		/* invalid TP version, ignore this TPDU	*/
	  }
      break;

      case TP_PAR_CODE_ADD_OPTIONS:
        /* Following proposed protocol parameters can be in	*/
	/* additional options (for class 4):			*/
	/* non-use of checksum, use/non-use of expedited data.	*/
	/* We will reject non-use of checksum and we will not	*/
	/* support expedited data service (see encoding of CC).	*/
	/* There is no processing needed of this parameter.	*/
      break;

      case TP_PAR_CODE_CHECKSUM:
        checksum_present = SD_TRUE;
        if (!tp_checksum_verified (pdu_ptr, pdu_len, len_proc))
          return (0);		/* bad checksum, ignore this TPDU	*/
      break;

      default:
        /* ignore this parameter */
        TP_LOG_IND1 ("TP-WARNING: decode CR or CC TPDU: Unrecognized param code (0x%X) ignored",
          param->code);
      break;  
      } /* end switch */

    /* add len of currently processed parameter to len_proc */
    len_proc += (ST_UINT16) (TP_PAR_FIX_LEN + param->len);
    } /* end while */

  if (checksum_present==SD_FALSE  &&  tpdu_cx_rx->preferred_class==4)
    {
    TP_LOG_ERR0 ("TP-ERROR: decode CR TPDU failed (checksum parameter not present)");
    return (0);
    }

  /*----------------------------------------------------*/
  /* If decoding of header successful copy user data	*/
  /* and call proceess function.			*/
  /*----------------------------------------------------*/

  /* check if user data are present */

  if (hdr_len < pdu_len)
    {
    ST_UINT16 udata_len = pdu_len - hdr_len;

    if ( udata_len <= TP_MAX_CONN_UDATA)
      {
      memcpy (tpdu_cx_rx->udata_buf, &pdu_ptr [hdr_len], udata_len);
      tpdu_cx_rx->udata_len = udata_len;
      }
    else
      TP_LOG_ERR1 ("TP-ERROR: invalid user data length (%u) in received CR or CC TPDU",
	  udata_len);
    }

  return (pdu_len);
}
 
 
/************************************************************************/
/*			tp_decode_dr					*/
/*----------------------------------------------------------------------*/
/* Function used to decode a received DR TPDU. Because the n_unitdata	*/
/* may contain multiple TPDUs, the offset parameter will tell us where	*/
/* the DR TPDU starts in the data_buf (n_unitdata).			*/
/*    This function does NOT return a decode error if the checksum is	*/
/* not present. Use of checksum may not have been negotiated yet.	*/
/*									*/
/* Parameters:								*/
/*	TPDU_DR    *tpdu_dr_rx	Struct to fill in with decoded info.	*/
/*	ST_UCHAR   *pdu_ptr	Pointer to TPDU.			*/
/*	ST_UINT16  pdu_len	Len of TPDU.				*/
/*									*/
/* Return:								*/
/*	ST_UINT16		Length of DR TPDU.			*/
/*	0			if decoding failed			*/
/* NOTE: many ways to fail so return 0 IMMEDIATELY after any error.	*/
/************************************************************************/
ST_UINT16 tp_decode_dr (TPDU_DR *tpdu_dr_rx, ST_UCHAR *pdu_ptr,
              ST_UINT16 pdu_len)
{
TP_FHDR_DR	*pdu;
ST_UINT16		 len_proc;	/* length of DR TPDU already processed	*/
TP_PAR		*param;
ST_UINT16  hdr_len;

  hdr_len = get_hdr_len (pdu_ptr, pdu_len, TP_FHDR_DR_LEN);
  if (hdr_len == 0)
    {
    TP_LOG_ERR0 ("TPDU-DR.ind invalid header len. TPDU Ignored.");
    return (0);
    }

  /* make sure the struct is cleared */
  memset (tpdu_dr_rx, 0, sizeof (TPDU_DR));

  pdu = (TP_FHDR_DR *) pdu_ptr;

  /* everything in data_buf starting from offset should be the DR TPDU	*/

  /*--------------------------------------------*/
  /* Process the fixed part of DR TPDU header	*/
  /*--------------------------------------------*/

  tpdu_dr_rx->loc_ref = *((ST_UINT16 *) pdu->dest_ref);	/* local conn reference	*/
  tpdu_dr_rx->rem_ref = *((ST_UINT16 *) pdu->src_ref);  	/* remote conn reference*/
  tpdu_dr_rx->reason = pdu->reason;			/* reson for disconnect	*/

  /*----------------------------------------------------*/
  /* Process the variable part of DR TPDU header.	*/
  /*----------------------------------------------------*/

  len_proc = TP_FHDR_DR_LEN;	/* we processed already the fixed part	*/

  while (len_proc < hdr_len)
    {
    if ((param = get_next_param (pdu_ptr, len_proc, hdr_len)) == NULL)
      {
      TP_LOG_ERR0 ("TPDU-DR.ind invalid param len. TPDU Ignored.");
      return (0);
      }

    switch (param->code)
      {
      case TP_PAR_CODE_CHECKSUM:
        if (!tp_checksum_verified (pdu_ptr, pdu_len, len_proc))
          return (0);		/* bad checksum, ignore this TPDU	*/
      break;

      default:
        /* ignore other parameters */
        TP_LOG_IND1 ("TP-WARNING: decode DR TPDU: Unrecognized param code (0x%X) ignored",
          param->code);
      break;
      } /* end switch */

    /* add len of currently processed parameter to len_proc */
    len_proc += (ST_UINT16) (TP_PAR_FIX_LEN + param->len);
    } /* end while */

  return (pdu_len);
}


/************************************************************************/
/*			tp_decode_dc					*/
/*----------------------------------------------------------------------*/
/* Function used to decode a received DC TPDU.				*/
/*    This function does NOT return a decode error if the checksum is	*/
/* not present. Use of checksum may not have been negotiated yet.	*/
/*									*/
/* Parameters:								*/
/*	TPDU_DC    *tpdu_dc_rx	Struct to fill in with decoded info.	*/
/*	ST_UCHAR   *pdu_ptr	Pointer to TPDU.			*/
/*									*/
/* Return:								*/
/*	ST_UINT16		Length of DC TPDU.			*/
/*	0			if decoding failed			*/
/* NOTE: many ways to fail so return 0 IMMEDIATELY after any error.	*/
/************************************************************************/
ST_UINT16 tp_decode_dc (TPDU_DC *tpdu_dc_rx,
	ST_UCHAR *pdu_ptr,
	ST_UINT16 pdu_len)
{
TP_FHDR_DC *pdu;
TP_PAR *param;
ST_UINT16 len_proc;	/* length of TPDU already processed	*/
ST_UINT16 hdr_len;

  hdr_len = get_hdr_len (pdu_ptr, pdu_len, TP_FHDR_DC_LEN);
  if (hdr_len == 0)
    {
    TP_LOG_ERR0 ("TPDU-DC.ind invalid header len. TPDU Ignored.");
    return (0);
    }

  /* make sure the struct is cleared */
  memset (tpdu_dc_rx, 0, sizeof (TPDU_DC));

  pdu = (TP_FHDR_DC *) pdu_ptr;

  /*--------------------------------------------*/
  /* Process the fixed part of DC TPDU header	*/
  /*--------------------------------------------*/

  tpdu_dc_rx->loc_ref = *((ST_UINT16 *) pdu->dest_ref);	/* local conn reference	*/
  tpdu_dc_rx->rem_ref = *((ST_UINT16 *) pdu->src_ref);	/* remote conn reference*/

  /*----------------------------------------------------*/
  /* Process the variable part of DC TPDU header.	*/
  /*----------------------------------------------------*/

  len_proc = TP_FHDR_DC_LEN;	/* we processed already the fixed part	*/

  while (len_proc < hdr_len)
    {
    if ((param = get_next_param (pdu_ptr, len_proc, hdr_len)) == NULL)
      {
      TP_LOG_ERR0 ("TPDU-DC.ind invalid param len. TPDU Ignored.");
      return (0);
      }

    switch (param->code)
      {
      case TP_PAR_CODE_CHECKSUM:
        if (!tp_checksum_verified (pdu_ptr, pdu_len, len_proc))
          return (0);		/* bad checksum, ignore this TPDU	*/
      break;

      default:
        /* Other parameters not allowed.	*/
        TP_LOG_ERR1 ("TPDU-DC.ind unrecognized param code (0x%X). Protocol error.",
                     param->code);
        return (0);
      break;
      } /* end switch */

    /* add len of currently processed parameter to len_proc */
    len_proc += (ST_UINT16) (TP_PAR_FIX_LEN + param->len);
    } /* end while */

  return (pdu_len);
}


/************************************************************************/
/*			tp_decode_dt					*/
/*----------------------------------------------------------------------*/
/* Function used to decode a received DT TPDU.				*/
/*									*/
/* Parameters:								*/
/*	TPDU_DT    *tpdu_dt_rx	Struct to fill in with decoded info.	*/
/*	ST_UCHAR   *pdu_ptr	Pointer to TPDU.			*/
/*	ST_UINT16  pdu_len	Len of TPDU.				*/
/*									*/
/* Return:								*/
/*	ST_UINT16		Length of DT TPDU.			*/
/*	0			if decoding failed			*/
/* NOTE: many ways to fail so return 0 IMMEDIATELY after any error.	*/
/************************************************************************/
ST_UINT16 tp_decode_dt (TPDU_DT *tpdu_dt_rx, ST_UCHAR *pdu_ptr,
              ST_UINT16 pdu_len)
{
TP_FHDR_DT	*pdu;
ST_BOOLEAN		 checksum_present;
ST_UINT16  hdr_len;
ST_UINT16  len_proc;	/* length of TPDU already processed	*/
TP_PAR    *param;

  hdr_len = get_hdr_len (pdu_ptr, pdu_len, TP_FHDR_DT_LEN);
  if (hdr_len == 0)
    {
    TP_LOG_ERR0 ("TPDU-DT.ind invalid header len. TPDU Ignored.");
    return (0);
    }

  checksum_present = SD_FALSE;	/* it is error if checksum is missing	*/

  /* make sure the struct is cleared */
  memset (tpdu_dt_rx, 0, sizeof (TPDU_DT));

  pdu = (TP_FHDR_DT *) pdu_ptr;

  /*--------------------------------------------*/
  /* Process the fixed part of DT TPDU header	*/
  /*--------------------------------------------*/

  /* udata is everything after the header	*/
  tpdu_dt_rx->udata_ptr = (ST_CHAR *) pdu_ptr + hdr_len;
  tpdu_dt_rx->udata_len = pdu_len - hdr_len;

  if (pdu->eot_sn & TP_PDU_MASK_SN)
    tpdu_dt_rx->eot = SD_TRUE;			/* set the EOF flag */
  else
    tpdu_dt_rx->eot = SD_FALSE;

  tpdu_dt_rx->dst_ref = *((ST_UINT16 *) pdu->dest_ref);	/* local conn reference	*/
  tpdu_dt_rx->sn = (ST_UCHAR) (pdu->eot_sn & TP_PDU_MASK_EOF); /* sn of received TPDU */

  /*----------------------------------------------------*/
  /* Process the variable part of DT TPDU header.	*/
  /*----------------------------------------------------*/

  len_proc = TP_FHDR_DT_LEN;	/* we processed already the fixed part	*/

  while (len_proc < hdr_len)
    {
    if ((param = get_next_param (pdu_ptr, len_proc, hdr_len)) == NULL)
      {
      TP_LOG_ERR0 ("TPDU-DT.ind invalid param len. TPDU Ignored.");
      return (0);
      }
    switch (param->code)
      {
      /* Checksum is the only allowed parameter*/
      case TP_PAR_CODE_CHECKSUM:
        checksum_present = SD_TRUE;
        if (!tp_checksum_verified (pdu_ptr, pdu_len, len_proc))
          return (0);		/* bad checksum, ignore this TPDU	*/
      break;

      default:
        /* Other parameters not allowed.	*/
        TP_LOG_ERR1 ("TPDU-DT.ind unrecognized param code (0x%X). Protocol error.",
                     param->code);
        return (0);
      break;  
      } /* end switch */

    /* add len of currently processed parameter to len_proc */
    len_proc += (ST_UINT16) (TP_PAR_FIX_LEN + param->len);
    } /* end while */

  if (!checksum_present)
    {
    TP_LOG_ERR0 ("TP-ERROR: decode DT TPDU failed (checksum parameter not present)");
    return (0);
    }

  return (pdu_len);
}


/************************************************************************/
/*			tp0_decode_dt					*/
/*----------------------------------------------------------------------*/
/* This function is used to decode a Class 0 DT TPDU.			*/
/* Same as tp_decode_dt except dest_ref and Checksum NOT encoded.	*/
/* NOTE: many ways to fail so return 0 IMMEDIATELY after any error.	*/
/************************************************************************/
ST_UINT16 tp0_decode_dt (TPDU_DT *tpdu_dt_rx, ST_UCHAR *pdu_ptr,
              ST_UINT16 pdu_len)
{
TP0_FHDR_DT	*tp0_fhdr_dt;
ST_UINT16  hdr_len;

  hdr_len = get_hdr_len (pdu_ptr, pdu_len, TP0_FHDR_DT_LEN);
  if (hdr_len == 0)
    {
    TP_LOG_ERR0 ("TPDU-DT.ind invalid header len. TPDU Ignored.");
    return (0);
    }
  if (hdr_len != TP0_FHDR_DT_LEN)
    {
    /* Variable part not allowed.	*/
    TP_LOG_ERR1 ("TPDU-DT.ind header len (%u) != 3. Protocol error.",
                 hdr_len);
    return (0);
    }

  /* make sure the struct is cleared */
  memset (tpdu_dt_rx, 0, sizeof (TPDU_DT));

  tp0_fhdr_dt = (TP0_FHDR_DT *) pdu_ptr;

  /*--------------------------------------------*/
  /* Process the fixed part of DT TPDU header	*/
  /*--------------------------------------------*/

  /* udata is everything after the header	*/
  tpdu_dt_rx->udata_ptr = (ST_CHAR *) pdu_ptr + hdr_len;
  tpdu_dt_rx->udata_len = pdu_len - hdr_len;

  if (tp0_fhdr_dt->eot_sn & TP_PDU_MASK_SN)
    tpdu_dt_rx->eot = SD_TRUE;			/* set the EOF flag */
  else
    tpdu_dt_rx->eot = SD_FALSE;

  tpdu_dt_rx->sn = (ST_UCHAR) (tp0_fhdr_dt->eot_sn & TP_PDU_MASK_EOF); /* sn of received TPDU */

  return (pdu_len);
}


/************************************************************************/
/*			tp_decode_ak					*/
/*----------------------------------------------------------------------*/
/* Function used to decode a received AK TPDU. Because the n_unitdata	*/
/* may contain multiple TPDUs, the offset parameter will tell us where	*/
/* the AK TPDU starts in the data_buf (n_unitdata).			*/
/*									*/
/* Parameters:								*/
/*	TPDU_AK    *tpdu_ak_rx	Struct to fill in with decoded info.	*/
/*	ST_UCHAR   *pdu_ptr	Pointer to TPDU.			*/
/*									*/
/* Return:								*/
/*	ST_UINT16		Length of AK TPDU.			*/
/*	0			if decoding failed			*/
/* NOTE: many ways to fail so return 0 IMMEDIATELY after any error.	*/
/************************************************************************/
ST_UINT16 tp_decode_ak (TPDU_AK *tpdu_ak_rx,
	ST_UCHAR *pdu_ptr,
	ST_UINT16 pdu_len)
{
TP_FHDR_AK	*pdu;
ST_UINT16		 len_proc;	/* length of AK TPDU already processed	*/
TP_PAR		*param;
ST_BOOLEAN		 checksum_present;
ST_UINT16  hdr_len;

  hdr_len = get_hdr_len (pdu_ptr, pdu_len, TP_FHDR_AK_LEN);
  if (hdr_len == 0)
    {
    TP_LOG_ERR0 ("TPDU-AK.ind invalid header len. TPDU Ignored.");
    return (0);
    }

  checksum_present = SD_FALSE;	/* it is error if checksum is missing	*/

  /* make sure the struct is cleared */
  memset (tpdu_ak_rx, 0, sizeof (TPDU_AK));

  pdu = (TP_FHDR_AK *) pdu_ptr;

  /*--------------------------------------------*/
  /* Process the fixed part of AK TPDU header	*/
  /*--------------------------------------------*/

  tpdu_ak_rx->dst_ref = *((ST_UINT16 *) pdu->dest_ref); 	/* local conn reference	*/
  tpdu_ak_rx->sn = (ST_UCHAR) (pdu->sn & TP_PDU_MASK_BIT8); /* next expected DT sn	*/
  tpdu_ak_rx->cdt = (ST_UCHAR) (pdu->type_cdt & TP_PDU_MASK_TYPE); /* remote CDT	*/

  /*----------------------------------------------------*/
  /* Process the variable part of AK TPDU.		*/
  /*----------------------------------------------------*/

  len_proc = TP_FHDR_AK_LEN;	/* we processed already the fixed part	*/

  while (len_proc < hdr_len)
    {
    if ((param = get_next_param (pdu_ptr, len_proc, hdr_len)) == NULL)
      {
      TP_LOG_ERR0 ("TPDU-AK.ind invalid param len. TPDU Ignored.");
      return (0);
      }

    switch (param->code)
      {
      case TP_PAR_CODE_CHECKSUM:
        checksum_present = SD_TRUE;
        if (!tp_checksum_verified (pdu_ptr, pdu_len, len_proc))
          return (0);		/* bad checksum, ignore this TPDU	*/
      break;

      default:
        /* ignore other parameters */
        TP_LOG_IND1 ("TP-WARNING: decode AK TPDU: Unrecognized param code (0x%X) ignored",
          param->code);
      break;
      } /* end switch */

    /* add len of currently processed parameter to len_proc */
    len_proc += (ST_UINT16) (TP_PAR_FIX_LEN + param->len);
    } /* end while */

  if (!checksum_present)
    {
    TP_LOG_ERR0 ("TP-ERROR: decode AK TPDU failed (checksum parameter not present)");
    return (0);
    }

  /* NOTE: len_proc may be less than pdu_len if multiple TPDU in NSDU.	*/
  return (len_proc);
}


/************************************************************************/
/*			tp_decode_er					*/
/*----------------------------------------------------------------------*/
/* Function used to decode a received ER TPDU. Because the n_unitdata	*/
/* may contain multiple TPDUs, the offset parameter will tell us where	*/
/* the ER TPDU starts in the data_buf (n_unitdata).			*/
/*									*/
/* Parameters:								*/
/*	TPDU_ER    *tpdu_er_rx	Struct to fill in with decoded info.	*/
/*	ST_UCHAR   *pdu_ptr	Pointer to TPDU.			*/
/*	ST_UINT16  pdu_len	Len of TPDU.				*/
/*	ST_INT     tp_classs	Transport class (must be 0 or 4).	*/
/*									*/
/* Return:								*/
/*	ST_UINT16		Length of ER TPDU.			*/
/*	0			if decoding failed			*/
/* NOTE: many ways to fail so return 0 IMMEDIATELY after any error.	*/
/************************************************************************/
ST_UINT16 tp_decode_er (TPDU_ER *tpdu_er_rx,
	ST_UCHAR *pdu_ptr,
	ST_UINT16 pdu_len,
	ST_INT tp_class)	/* must be 0 or 4	*/
{
TP_FHDR_ER *pdu;
ST_UINT16 len_proc;	/* length of TPDU already processed	*/
TP_PAR *param;
ST_UINT16 hdr_len;
ST_BOOLEAN checksum_present;

  hdr_len = get_hdr_len (pdu_ptr, pdu_len, TP_FHDR_ER_LEN);
  if (hdr_len == 0)
    {
    TP_LOG_ERR0 ("TPDU-ER.ind invalid header len. TPDU Ignored.");
    return (0);
    }

  /* If class 4 (tp_class=4), checksum must be present.	*/
  checksum_present = SD_FALSE;

  /* make sure the struct is cleared */
  memset (tpdu_er_rx, 0, sizeof (TPDU_ER));

  pdu = (TP_FHDR_ER *) pdu_ptr;

  /*--------------------------------------------*/
  /* Process the fixed part of ER TPDU header	*/
  /*--------------------------------------------*/
  tpdu_er_rx->dst_ref = *((ST_UINT16 *) pdu->dest_ref);	/* local conn reference	*/
  tpdu_er_rx->reject_cause = pdu->reason;

  /*----------------------------------------------------*/
  /* Process the variable part of ER TPDU header.	*/
  /*----------------------------------------------------*/

  len_proc = TP_FHDR_ER_LEN;	/* we processed already the fixed part	*/

  while (len_proc < hdr_len)
    {
    if ((param = get_next_param (pdu_ptr, len_proc, hdr_len)) == NULL)
      {
      TP_LOG_ERR0 ("TPDU-ER.ind invalid param len. TPDU Ignored.");
      return (0);
      }

    switch (param->code)
      {
      case TP_PAR_CODE_ER_INVAL_TPDU:	/* Invalid TPDU	*/
        /* Set ptr to Invalid TPDU part of message & set len.	*/
        tpdu_er_rx->invalid_tpdu_ptr = &param->value[0];
        tpdu_er_rx->invalid_tpdu_len = param->len;
      break;

      case TP_PAR_CODE_CHECKSUM:
        checksum_present = SD_TRUE;
        if (!tp_checksum_verified (pdu_ptr, pdu_len, len_proc))
          return (0);		/* bad checksum, ignore this TPDU	*/
      break;

      default:
        /* Other parameters not allowed.	*/
        TP_LOG_ERR1 ("TPDU-ER.ind unrecognized param code (0x%X). Protocol error.",
                     param->code);
        return (0);
      break;  
      } /* end switch */

    /* add len of currently processed parameter to len_proc */
    len_proc += (ST_UINT16) (TP_PAR_FIX_LEN + param->len);
    } /* end while */

  /* For Class 0, invalid_tpdu_ptr must be set during decode.	*/
  if (tpdu_er_rx->invalid_tpdu_ptr == NULL && tp_class == 0)
    {
    TP_LOG_ERR0 ("TPDU-ER.ind 'Invalid TPDU' not present for Class 0");
    return (0);
    }

  /* For Class 4, checksum must be found during decode.	*/
  if (!checksum_present && tp_class == 4)
    {
    TP_LOG_ERR0 ("TPDU-ER.ind checksum not present for Class 4");
    return (0);
    }

  /* NOTE: len_proc may be less than pdu_len if multiple TPDU in NSDU.	*/
  return (len_proc);
}


/************************************************************************/
/*			tp_checksum_verified				*/
/*----------------------------------------------------------------------*/
/* This function is used to decode and verify the checksum parameter in	*/
/* TPDU.								*/
/*									*/
/* Parameters:								*/
/*	ST_UCHAR	*buf		Ptr to TPDU buffer			*/
/*	ST_UINT16	buf_len		Length of the buffer (TPDU).		*/
/*	ST_UINT16	par_pos		Position of checksum parameter in TPDU.	*/
/*									*/
/* Return:								*/
/*	ST_BOOLEAN			SD_TRUE if checksum verified.		*/
/*				SD_FALSE otherwise				*/
/************************************************************************/
static ST_BOOLEAN tp_checksum_verified (ST_UCHAR *buf, ST_UINT16 buf_len, ST_UINT16 par_pos)
{
ST_BOOLEAN verified;
TP_PAR *param;

  /* NOTE: len checked before calling, so we know param fits in TPDU,	*/
  /*       but here we make sure param len == 2 before continuing.	*/

  verified = SD_FALSE;

  param = (TP_PAR *) (&buf [par_pos]);	/* point to start of param*/

  if (param->code == TP_PAR_CODE_CHECKSUM)
    {
    if (param->len == 2)
      {
      /* compute the checksum of the entire TPDU, the position of the	*/
      /* checksum first byte in the buf is computed as the difference	*/
      /* between pointers							*/

      verified = checksum_verified (buf, buf_len,
		(ST_UINT16) (param->value - buf), SD_FALSE);

      if (!verified)
        TP_LOG_ERR0 ("TP-ERROR: decode TPDU failed (verification of checksum failed)");
      }
    else
      TP_LOG_ERR1 ("TP-ERROR: decode TPDU failed (invalid length=%u for checksum parameter)",
        param->len);
    }
  else
    TP_LOG_ERR0 ("TP-ERROR: decode TPDU failed (checksum parameter missing)");

  return (verified);
}

#if defined(DEBUG_SISCO)
/************************************************************************/
/*			tp_log_tsdu					*/
/*----------------------------------------------------------------------*/
/* Function used to log Transport Service data unit.			*/
/*									*/
/* Parameters:								*/
/*	ST_VOID	 *tsdu		Pointer to data unit struct		*/
/*	ST_UCHAR type		TSDU type (TP_PDU_TYPE_CR, etc.)	*/
/*	ST_UCHAR mask		logging  mask:				*/
/*				TP_IND (indication) or TP_REQ (request)	*/
/*									*/
/* Return:								*/
/*	ST_VOID			none					*/
/* NOTE: all src_ref & dst_ref stored in network byte order. Converted	*/
/*       to "host byte order" using "ntohs" only in this function.	*/
/************************************************************************/
ST_VOID tp_log_tsdu (ST_VOID *tsdu, ST_UCHAR type, ST_UCHAR mask)
{
char *buf;	/* ptr to msg header (i.e. TPDU type)	*/
		/* set before passing to log function	*/

  S_LOCK_UTIL_RESOURCES ();
  switch (type)
    {
    case TP_PDU_TYPE_CR:
    case TP_PDU_TYPE_CC:
      {  /*block*/
      TPDU_CX *tpdu = (TPDU_CX *) tsdu;

      if (mask == TP_IND)
        {
        if (type == TP_PDU_TYPE_CR)
          buf = "TPDU-CR.ind:";
        else
          buf = "TPDU-CC.ind:";

        /* For IND, dst_ref is loc_ref	*/
        TP_LOG_RI6 (mask,"%s  dst_ref=0x%04X src_ref=0x%04X max_len=%4u class=%u cdt=%2u",
          buf, ntohs(tpdu->loc_ref), ntohs(tpdu->rem_ref), 1 << tpdu->max_tpdu_len_enc,
          (unsigned) tpdu->preferred_class, (unsigned) tpdu->cdt);
        }
      else
        {
        if (type == TP_PDU_TYPE_CR)
          buf = "TPDU-CR.req:";
        else
          buf = "TPDU-CC.req:";

        /* For REQ, dst_ref is rem_ref	*/
        TP_LOG_RI6 (mask,"%s  dst_ref=0x%04X src_ref=0x%04X max_len=%4u class=%u cdt=%2u",
          buf, ntohs(tpdu->rem_ref), ntohs(tpdu->loc_ref), 1 << tpdu->max_tpdu_len_enc,
          (unsigned) tpdu->preferred_class, (unsigned) tpdu->cdt);
        }

      TP_LOGC_RI0 (mask,"Local TSEL:");
      TP_LOGHC_RI (mask,(ST_UINT) tpdu->loc_tsap [0], &tpdu->loc_tsap [1]);
      TP_LOGC_RI0 (mask,"Remote TSEL:");
      TP_LOGHC_RI (mask,(ST_UINT) tpdu->rem_tsap [0], &tpdu->rem_tsap [1]);

#if 0
      TP_LOGC_RI0 (mask,"Remote MAC:");
      TP_LOGHC_RI (mask,(ST_UINT) CLNP_MAX_LEN_MAC, tpdu->rem_mac);
      TP_LOGC_RI0 (mask,"Remote NSAP:");
      TP_LOGHC_RI (mask,(ST_UINT) tpdu->rem_nsap [0], &tpdu->rem_nsap [1]);
#endif
      TP_LOGC_RI1 (mask,"User data length: %u", tpdu->udata_len);
      if (tpdu->udata_len > 0)
        TP_LOGHC_RI (mask,(ST_UINT) tpdu->udata_len, tpdu->udata_buf);
      }
    break;

    case TP_PDU_TYPE_DR:
    case TP_PDU_TYPE_DC:
      {  /*block*/
      TPDU_DR *tpdu = (TPDU_DR *) tsdu;	/* we use DR struct for both */

      if (mask == TP_IND)
        {
        if (type == TP_PDU_TYPE_DR)
          buf = "TPDU-DR.ind:";
        else
          buf = "TPDU-DC.ind:";

        /* For IND, dst_ref is loc_ref	*/
        TP_LOG_RI3 (mask,"%s  dst_ref=0x%04X src_ref=0x%04X",
          buf, ntohs(tpdu->loc_ref), ntohs(tpdu->rem_ref));
        }
      else
        {
        if (type == TP_PDU_TYPE_DR)
          buf = "TPDU-DR.req:";
        else
          buf = "TPDU-DC.req:";

        /* For REQ, dst_ref is rem_ref	*/
        TP_LOG_RI3 (mask,"%s  dst_ref=0x%04X src_ref=0x%04X",
          buf, ntohs(tpdu->rem_ref), ntohs(tpdu->loc_ref));
        }

      if (type == TP_PDU_TYPE_DR)
        TP_LOGC_RI1 (mask,"Disconnect reason: 0x%2.2X", (unsigned) tpdu->reason);
#if 0
      TP_LOGC_RI0 (mask,"Remote MAC:");
      TP_LOGHC_RI (mask,(ST_UINT) CLNP_MAX_LEN_MAC, tpdu->rem_mac);
      TP_LOGC_RI0 (mask,"Remote NSAP:");
      TP_LOGHC_RI (mask,(ST_UINT) tpdu->rem_nsap [0], &tpdu->rem_nsap [1]);
#endif
      }
    break;

    case TP_PDU_TYPE_DT:
      {  /*block*/
      TPDU_DT *tpdu = (TPDU_DT *) tsdu;

      if (mask == TP_IND)
        buf = "TPDU-DT.ind:";
      else
        buf = "TPDU-DT.req:";

      TP_LOG_RI4 (mask,"%s  dst_ref=0x%04X sn=%3u eot=%u",
        buf, ntohs(tpdu->dst_ref), (unsigned) tpdu->sn, (unsigned) tpdu->eot);

#if 0
      TP_LOGC_RI0 (mask,"Remote MAC:");
      TP_LOGHC_RI (mask,(ST_UINT) CLNP_MAX_LEN_MAC, tpdu->rem_mac);
      TP_LOGC_RI0 (mask,"Remote NSAP:");
      TP_LOGHC_RI (mask,(ST_UINT) tpdu->rem_nsap [0], &tpdu->rem_nsap [1]);
#endif
      TP_LOGC_RI1 (mask,"User data length: %u", tpdu->udata_len);
      TP_LOGHC_RI (mask,(ST_UINT) tpdu->udata_len, tpdu->udata_ptr);
      }
    break;

    case TP_PDU_TYPE_AK:
      {  /*block*/
      TPDU_AK *tpdu = (TPDU_AK *) tsdu;

      if (mask == TP_IND)
        buf = "TPDU-AK.ind:";
      else
        buf = "TPDU-AK.req:";

      TP_LOG_RI4 (mask,"%s  dst_ref=0x%04X sn=%3u cdt=%2u",
        buf, ntohs(tpdu->dst_ref), (unsigned) tpdu->sn, (unsigned) tpdu->cdt);
#if 0
      TP_LOGC_RI0 (mask,"Remote MAC:");
      TP_LOGHC_RI (mask,(ST_UINT) CLNP_MAX_LEN_MAC, tpdu->rem_mac);
      TP_LOGC_RI0 (mask,"Remote NSAP:");
      TP_LOGHC_RI (mask,(ST_UINT) tpdu->rem_nsap [0], &tpdu->rem_nsap [1]);
#endif
      }
    break;
    default:
      TP_LOG_ERR1 ("TP-ERROR: unknown TPDU type %d", type);
    break;
    }

  S_UNLOCK_UTIL_RESOURCES ();
}
#else	/* function if DEBUG_SISCO not defined */
ST_VOID tp_log_tsdu (ST_VOID *tsdu, ST_UCHAR type, ST_UCHAR mask)
{
  return;
}
#endif
