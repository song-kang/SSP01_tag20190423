/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	1997-2009, All Rights Reserved					*/
/*									*/
/*			PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : cosp_enc.c						*/
/* PRODUCT(S)  : MOSI Stack (over TP4)					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*		This file implements the encoding of COSP SPDUs.	*/
/*									*/
/* NOTE: Implementation restrictions:					*/
/*		Non-segmenting COSP,					*/
/*		Transport connections are not reused.			*/
/*									*/
/*  For information see the:						*/
/*	ISO 8326 "Information processing systems - Open Systems		*/
/*	  Interconnection - Basic connection oriented session service	*/
/*	  definition.							*/
/*	ISO 8327 "Information processing systems - Open Systems		*/
/*	  Interconnection - Basic connection oriented session protocol	*/
/*	  specification.						*/
/*	ISO 8327/ADD.2 (Draft for Version2).				*/
/*									*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/*			cosp_envelope_len 				*/
/*			cosp_enc_cn					*/
/*			cosp_enc_ac					*/
/*			cosp_enc_rf					*/
/*			cosp_enc_fn_dn					*/
/*			cosp_enc_ab					*/
/*			cosp_enc_dt					*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 11/17/09  JRB    11     Repl cosp_enc_cn_ac with 2 separate functs.	*/
/*			   cosp_envelope_len: add loc_ssel_len arg.	*/
/* 06/02/04  JRB    10     cosp_enc_dt: do NOT restrict user data len.	*/
/* 07/16/02  JRB    09     Clean up & fix comments in cosp_enc_cn_ac.	*/
/* 06/05/02  JRB    08     Send Calling SSEL on connect request.	*/
/* 09/12/01  JRB    07     Reverse last change (not needed).		*/
/* 08/01/01  JRB    06     Del rem_addr arg from cosp_enc_cn_ac.	*/
/* 04/12/00  JRB    05     Lint cleanup.				*/
/* 09/13/99  MDE    04     Added SD_CONST modifiers			*/
/* 08/13/98  JRB    03     Lint cleanup.				*/
/* 05/27/97  JRB   7.00    MMSEASE 7.0 release.				*/
/* 03/20/97  EJV    02     Enhanced logging.				*/
/* 01/17/97  EJV    01     Created					*/
/************************************************************************/
#include "glbtypes.h"
#include "sysincs.h"

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;	/* Define for SLOG mechanism	*/
#endif

#include "acse2.h"

#include "cosp_log.h"
#include "cosp.h"
#include "cosp_usr.h"


	/*----------------------------------------------*/
	/* Fixed encoding for CONNECT / ACCEPT SPDU:	*/
	/* Connect/Accept PGI:		    		*/
	/*	- extended concatination not supported,	*/
	/*	- COSP Version2 supported		*/
	/* Session User Requirements PI: 		*/
	/*	- duplex functional unit (FU) supported	*/
	/*----------------------------------------------*/

static ST_UCHAR cosp_cn_ac_fixed [12] = {
	5,		/*   CN-AC PGI code				*/
	6,		/*   CN-AC PGI len				*/
			/* Protocol Option PI:				*/
	19,		/*   PI code					*/
	1,		/*   PI len					*/
	0,		/*   0 if rcv extended concatinat. not supported*/
			/* Version Number PI:				*/
	22,		/*   PI code					*/
	1,		/*   PI len					*/
	COSP_VER2,	/*   we supporting Version2			*/
			/* Session User Requirements PI:		*/
	20,		/*   PI code					*/
	2,		/*   PI len					*/
	0,		/*   bits 9-16 are 0				*/
	2};		/*   bits 1-8 (only bit2=1 duplex FU supported)	*/



	/*----------------------------------------------*/
	/* Fixed encoding for REFUSE SPDU:		*/
	/* Session User Requirements PI:	 	*/
	/*	- duplex functional unit (FU) supported	*/
	/* COSP Version PI:				*/
	/*	- Version2 supported			*/
	/*----------------------------------------------*/

static ST_UCHAR cosp_rf_fixed [7] = {
			/* Session User Requirements PI:		*/
	20,		/*   PI code					*/
	2,		/*   PI len					*/
	0,		/*   bits 9-16 are 0				*/
	2,		/*   bits 1-8 (only bit2=1 duplex FU supported)	*/
			/* Version Number PI:				*/
	22,		/*   PI code					*/
	1,		/*   PI len					*/
	COSP_VER2};	/*   we supporting Version2			*/


static ST_UINT cosp_enc_len (ST_UINT16 len, ST_UCHAR *buf);


/************************************************************************/
/*			cosp_envelope_len 				*/
/*----------------------------------------------------------------------*/
/* Function to compute the number of envelope bytes needed to encode	*/
/* a SPDU. The returned length does not include the SS-user data length.*/
/* 									*/
/* Parameters:								*/
/*  ACSE_CONN   *con		Connection info parameters		*/
/*  ST_UINT      loc_ssel_len 	Local SSEL length			*/
/*  ST_UINT      rem_ssel_len 	Remote SSEL length			*/
/*  ST_UCHAR	 spdu_type	One of COSP_SI_... from cosp.h		*/
/*	  								*/
/* Return:								*/
/*  ST_UINT     		Length of SPDU envelope encoding	*/
/************************************************************************/
ST_UINT cosp_envelope_len (
	ACSE_CONN *con,
	ST_UINT loc_ssel_len,
	ST_UINT rem_ssel_len,
	ST_UCHAR spdu_type)
{
ST_UINT enc_len;
ST_UINT udata_len;

  udata_len = con->ppdu_len;
  enc_len = 0;

  switch (spdu_type)
    {
    case COSP_SI_CONNECT:
      enc_len =
        2+6					/* Connect/Accept PGI	*/
      + 2+2					/* Ses User Requirem. PI*/
      + (loc_ssel_len != 0 ? 2+loc_ssel_len : 0)/* Calling SSEL		*/
      + (rem_ssel_len != 0 ? 2+rem_ssel_len : 0)/* Called  SSEL		*/
      + (udata_len > 0     ? 2 : 0)		/* 1 PI code, 1 len	*/
      + (udata_len > 254   ? 2 : 0);		/* add 2 bytes for len	*/
    break;

    case COSP_SI_ACCEPT:
      enc_len =
        2+6					/* Connect/Accept PGI	*/
      + 2+2					/* Ses User Requirem. PI*/
      + (rem_ssel_len != 0 ? 2+rem_ssel_len : 0)/* Calling  SSEL	*/
      + (loc_ssel_len != 0 ? 2+loc_ssel_len : 0)/* Responding SSEL	*/
      + (udata_len > 0    ? 2 : 0)		/* 1 PI code, 1 len	*/
      + (udata_len > 254  ? 2 : 0);		/* add 2 bytes for len	*/
    break;

    case COSP_SI_REFUSE:
      enc_len = 
        2+2					/* Ses User Requirem. PI*/
      + 2+1					/* Supported Version PI	*/
      + 2+1					/* 1 PI code, 1 len, 1 Reason Code*/
      + (udata_len > 254  ? 2 : 0);		/* add 2 bytes for len */
    break;

    case COSP_SI_FINISH:
    case COSP_SI_DISCON:
      enc_len =
        (udata_len > 0    ? 2 : 0)		/* 1 PI code, 1 len	*/
      + (udata_len > 254  ? 2 : 0);		/* addl 2 bytes for len	*/
    break;

    case COSP_SI_ABORT:
      enc_len =
        2+1					/* TP Disconnect PI	*/
      + (udata_len > 0    ? 2 : 0)		/* 1 PI code, 1 len	*/
      + (udata_len > 254  ? 2 : 0);		/* add 2 bytes for len	*/
    break;

    case COSP_SI_DATA:
      /* We send Give Token SPDU concatinated with the DT (basic concatination)	*/
      enc_len = 2;	/* (SI=01,LI=0) token functionality not used	*/
    break;

    default:
      COSP_LOG_ERR1 ("COSP-ERROR: Encoding: Invalid PDU type = %d", spdu_type);
    break;
    }

  /* add bytes for params len (none in DT) */
  if (spdu_type == COSP_SI_DATA)
    /* in DT we do not send any parameters but we have to send the LI=0	*/
    enc_len += 1;			/* 1 byte for len	*/
  else
    {
    /* all udata is encoded in a parameter */
    if (enc_len + udata_len > 254)
      enc_len += 3;			/* 3 bytes for len	*/
    else
      enc_len += 1;			/* 1 byte for len	*/
    }

  /* add byte for SPDU code */
  enc_len += 1;

  return (enc_len);
}


/************************************************************************/
/*			cosp_enc_cn					*/
/*----------------------------------------------------------------------*/
/* Function to encode a CONNECT SPDU.					*/
/* This function encodes into the global encode buffer "cosp_buf".	*/
/* It assumes the PPDU has already been encoded in the buffer, and it	*/
/* adds the Session encoding in front of it. The function		*/
/* cosp_envelope_len computes the exact size of the Session encoding.	*/
/* 									*/
/* Return:								*/
/*	SD_SUCCESS (0)		if successful				*/
/*	error code		otherwise				*/
/************************************************************************/
ST_RET cosp_enc_cn (
	ACSE_CONN *con,		/* Connection info			*/
	char **spdu_ptr,	/* Pointer where to return pointer to SPDU*/
	ST_UINT *spdu_len)	/* Pointer where to return the SPDU len	*/
{
ST_UINT	envelope_len;
ST_UINT	udata_len;
ST_UINT	udata_max_len;
ST_UCHAR	       *enc_buf;
ST_UINT	enc_len;
ST_UINT  	idx;
ST_UINT copy_len;	/* num bytes to copy	*/

  /*------------------------------------------------------------*/
  /* make sure data len is OK (User Data presence is mandatory)	*/
  /*------------------------------------------------------------*/
  udata_len = con->ppdu_len;
  envelope_len  = cosp_envelope_len (con, (ST_UINT)con->loc_ssel[0], (ST_UINT)con->rem_ssel[0], COSP_SI_CONNECT);

  udata_max_len = COSP_MAX_UDATA_CON;
  if (udata_len == 0 || udata_len > udata_max_len)
    {
    COSP_LOG_ERR2 ("COSP-ERROR: Encoding CN: Invalid User Data length=%u (0<length<=%u).",
      udata_len, udata_max_len);
    return (COSP_ERR_INV_UDATA_LEN);
    }

  /* set the pointer to beginning of encoding in the buffer (global buf)*/
  enc_buf = con->ppdu_ptr - envelope_len;
  if (enc_buf < cosp_buf)
    {
    COSP_LOG_ERR0 ("COSP-ERROR: Encoding CN: Invalid pointer to Global Encoding Buffer.");
    return (COSP_ERR_INV_POINTER);
    }
  idx = 0;

  /*--------------------------------------------*/
  /* Encode CN code and total SPDU len		*/
  /*--------------------------------------------*/
  /* SPDU length to encode does not include 1 byte for SPDU code and	*/
  /* 1 or 3 bytes for total len 					*/

  if (envelope_len-2 + udata_len <= 254)
    enc_len = envelope_len-2 + udata_len;	/* len enc on 1 byte	*/
  else
    enc_len = envelope_len-4 + udata_len;	/* len enc on 3 bytes	*/

  enc_buf [idx++] = COSP_SI_CONNECT;
  idx += cosp_enc_len ((ST_UINT16) enc_len, &enc_buf [idx]);

  /*--------------------------------------------*/
  /* no Connection Identifier PGI encoded 	*/
  /*--------------------------------------------*/

  /*--------------------------------------------*/
  /* encode Connect/Accept PGI:			*/
  /*	- extended concatination not supported,	*/
  /*	- COSP Version2 supported		*/
  /* encode Session User Requirements PI: 	*/
  /*	- duplex functional unit (FU) supported	*/
  /*--------------------------------------------*/
  memcpy (&enc_buf [idx], cosp_cn_ac_fixed, sizeof (cosp_cn_ac_fixed));
  idx += sizeof (cosp_cn_ac_fixed);

  /*--------------------------------------------*/
  /* encode Calling SSEL (if SSEL len > 0)	*/
  /*--------------------------------------------*/
  if (con->loc_ssel [0] > 0)	/* first byte is len	*/
    {
    enc_buf [idx++] = 51;			/* encode PI code	*/
    copy_len = con->loc_ssel[0] + 1;	/* copy len plus data together	*/
    memcpy (&enc_buf [idx], con->loc_ssel, copy_len);
    idx += copy_len;
    }

  /*--------------------------------------------*/
  /* encode Called SSEL (if SSEL len > 0)	*/
  /*--------------------------------------------*/
  if (con->rem_ssel [0] > 0)	/* first byte is len	*/
    {
    enc_buf [idx++] = 52;			/* encode PI code	*/
    copy_len = con->rem_ssel[0] + 1;	/* copy len plus data together	*/
    memcpy (&enc_buf [idx], con->rem_ssel, copy_len);
    idx += copy_len;
    }

  /*--------------------------------------------------------------------*/
  /* encode User Data (length only), SS-user Data should be already	*/
  /* in the enc_buf (in proper position).				*/
  /*--------------------------------------------------------------------*/
  if (udata_len <= 512)
    enc_buf [idx++] = 193;		/* encode PGI code		*/
  else
    enc_buf [idx++] = 194;		/* if 512 < udata_len <= 10240)*/
  idx += cosp_enc_len ((ST_UINT16) udata_len, &enc_buf [idx]);
  /* udata should be already in enc_buf */


  /* Make sure byte count computed correctly. Should NEVER fail.	*/
  if (idx != envelope_len)
    {
    COSP_LOG_ERR0 ("COSP-ERROR: Encoding CN: header length incorrect.");
    return (COSP_ERR_INV_POINTER);
    }

  /*-----------------------------------------------*/
  /* set values to be returned to calling function */
  /*-----------------------------------------------*/
  *spdu_len = envelope_len + udata_len;
  *spdu_ptr = (char *) enc_buf;

  return (SD_SUCCESS);
}

/************************************************************************/
/*			cosp_enc_ac					*/
/*----------------------------------------------------------------------*/
/* Function to encode a ACCEPT SPDU.					*/
/* This function encodes into the global encode buffer "cosp_buf".	*/
/* It assumes the PPDU has already been encoded in the buffer, and it	*/
/* adds the Session encoding in front of it. The function		*/
/* cosp_envelope_len computes the exact size of the Session encoding.	*/
/* 									*/
/* NOTE: Calling or Responding SSEL are NOT encoded. These should never	*/
/*       be needed and it reduces the bytes on the wire.		*/
/*									*/
/* Return:								*/
/*	SD_SUCCESS (0)		if successful				*/
/*	error code		otherwise				*/
/************************************************************************/
ST_RET cosp_enc_ac (
	ACSE_CONN *con,		/* Connection info			*/
        char **spdu_ptr,	/* Pointer where to return pointer to SPDU*/
	ST_UINT *spdu_len)	/* Pointer where to return the SPDU len	*/
{
ST_UINT envelope_len;
ST_UINT udata_len;
ST_UINT udata_max_len;
ST_UCHAR *enc_buf;
ST_UINT enc_len;
ST_UINT idx;

  /*------------------------------------------------------------*/
  /* make sure data len is OK (User Data presence is mandatory)	*/
  /*------------------------------------------------------------*/
  udata_len = con->ppdu_len;
  envelope_len  = cosp_envelope_len (con, 0, 0, COSP_SI_ACCEPT);

  udata_max_len = COSP_MAX_UDATA;		/* for ACCEPT SPDU	*/
  if (udata_len == 0 || udata_len > udata_max_len)
    {
    COSP_LOG_ERR2 ("COSP-ERROR: Encoding AC: Invalid User Data length=%u (0<length<=%u).",
      udata_len, udata_max_len);
    return (COSP_ERR_INV_UDATA_LEN);
    }

  /* set the pointer to beginning of encoding in the buffer (global buf)*/
  enc_buf = con->ppdu_ptr - envelope_len;
  if (enc_buf < cosp_buf)
    {
    COSP_LOG_ERR0 ("COSP-ERROR: Encoding AC: Invalid pointer to Global Encoding Buffer.");
    return (COSP_ERR_INV_POINTER);
    }
  idx = 0;

  /*--------------------------------------------*/
  /* Encode AC code and total SPDU len		*/
  /*--------------------------------------------*/
  /* SPDU length to encode does not include 1 byte for SPDU code and	*/
  /* 1 or 3 bytes for total len 					*/

  if (envelope_len-2 + udata_len <= 254)
    enc_len = envelope_len-2 + udata_len;	/* len enc on 1 byte	*/
  else
    enc_len = envelope_len-4 + udata_len;	/* len enc on 3 bytes	*/

  enc_buf [idx++] = COSP_SI_ACCEPT;
  idx += cosp_enc_len ((ST_UINT16) enc_len, &enc_buf [idx]);

  /*--------------------------------------------*/
  /* no Connection Identifier PGI encoded 	*/
  /*--------------------------------------------*/

  /*--------------------------------------------*/
  /* encode Connect/Accept PGI:			*/
  /*	- extended concatination not supported,	*/
  /*	- COSP Version2 supported		*/
  /* encode Session User Requirements PI: 	*/
  /*	- duplex functional unit (FU) supported	*/
  /*--------------------------------------------*/
  memcpy (&enc_buf [idx], cosp_cn_ac_fixed, sizeof (cosp_cn_ac_fixed));
  idx += sizeof (cosp_cn_ac_fixed);

  /*--------------------------------------------*/
  /* Calling/Responding SSEL not encoded in AC	*/
  /*--------------------------------------------*/

  /*--------------------------------------------------------------------*/
  /* encode User Data (length only), SS-user Data should be already	*/
  /* in the enc_buf (in proper position).				*/
  /*--------------------------------------------------------------------*/
  enc_buf [idx++] = 193;		/* encode PGI code	*/
  idx += cosp_enc_len ((ST_UINT16) udata_len, &enc_buf [idx]);
  /* udata should be already in enc_buf */

  /* Make sure byte count computed correctly. Should NEVER fail.	*/
  if (idx != envelope_len)
    {
    COSP_LOG_ERR0 ("COSP-ERROR: Encoding AC: header length incorrect.");
    return (COSP_ERR_INV_POINTER);
    }

  /*-----------------------------------------------*/
  /* set values to be returned to calling function */
  /*-----------------------------------------------*/
  *spdu_len = envelope_len + udata_len;
  *spdu_ptr = (char *) enc_buf;

  return (SD_SUCCESS);
}

/************************************************************************/
/*			cosp_enc_rf					*/
/*----------------------------------------------------------------------*/
/* Function to encode a REFUSE SPDU.					*/
/* This function assumed that there is enough place in the enc_buf for	*/
/* the RF SPDU encoding. The function cosp_envelope_len can be used	*/
/* to compute exact envelope encoding length.				*/
/* Note that this function assumes that the SS-user data (refuse reason)*/
/* has been already copied into the enc_buf in proper position.		*/
/* 									*/
/* Parameters:								*/
/*  ACSE_CONN	*con		Connection info parameters		*/
/*  charE      **spdu_ptr	Pointer where to return pointer to SPDU	*/
/*  ST_UINT	*spdu_len	Pointer where to return the SPDU len	*/
/*  ST_UCHAR	 reason		Reason  code for refuse			*/
/*	  								*/
/* Return:								*/
/*	SD_SUCCESS (0)		if successful				*/
/*	error code		otherwise				*/
/************************************************************************/
ST_RET cosp_enc_rf (ACSE_CONN *con, char **spdu_ptr, ST_UINT *spdu_len,
                     ST_UCHAR reason)
{
ST_UINT	envelope_len;
ST_UINT	udata_len;
ST_UCHAR	       *enc_buf;
ST_UINT	enc_len;
ST_UINT  	idx;

  /*--------------------------------------------*/
  /* make sure data len is OK			*/
  /*--------------------------------------------*/
  udata_len = con->ppdu_len;
  envelope_len  = cosp_envelope_len (con, 0, 0, COSP_SI_REFUSE);

  if (udata_len > COSP_MAX_UDATA)
    {
    COSP_LOG_ERR2 ("COSP-ERROR: Encoding RF: Invalid User Data length=%u (length<=%u).",
      udata_len, COSP_MAX_UDATA);
    return (COSP_ERR_INV_UDATA_LEN);
    }
  /* set the pointer to beginning of encoding in the buffer (global buf)*/
  enc_buf = con->ppdu_ptr - envelope_len;
  if (enc_buf < cosp_buf)
    {
    COSP_LOG_ERR0 ("COSP-ERROR: Encoding RF: Invalid pointer to Global Encoding Buffer.");
    return (COSP_ERR_INV_POINTER);
    }

  idx = 0;

  /*------------------------------------*/
  /* Encode RF code and total SPDU len	*/
  /*------------------------------------*/
  /* SPDU length to encode does not include 1 byte for SPDU code and	*/
  /* 1 or 3 bytes for total len 					*/

  if (envelope_len-2 + udata_len <= 254)
    enc_len = envelope_len-2 + udata_len;	/* len enc on 1 byte	*/
  else
    enc_len = envelope_len-4 + udata_len;	/* len enc on 3 bytes	*/

  enc_buf [idx++] = COSP_SI_REFUSE;
  idx += cosp_enc_len ((ST_UINT16) enc_len, &enc_buf [idx]);

  /*--------------------------------------------*/
  /* no Connection Identifier PGI encoded 	*/
  /*--------------------------------------------*/

  /*--------------------------------------------*/
  /* encode Session User Requirements PI: 	*/
  /*	- duplex functional unit (FU) supported	*/
  /* encode COSP Version PI:			*/
  /*	- Version2 supported			*/
  /*--------------------------------------------*/
  memcpy (&enc_buf [idx], cosp_rf_fixed, sizeof (cosp_rf_fixed));
  idx += sizeof (cosp_rf_fixed);

  /*--------------------------------------------------------------------*/
  /* encode Reason Code. SS-user Data (if any) should be already in the	*/
  /* enc_buf (in proper position).					*/
  /*--------------------------------------------------------------------*/
  enc_buf [idx++] = 50;					/* PI code	*/
  idx += cosp_enc_len ((ST_UINT16) (udata_len+1), &enc_buf [idx]);	/* len	*/
  enc_buf [idx++] = reason;				/* Reason code	*/
  /* udata may follow if reason=COSP_RF_REASON_U_REJECT	*/

  /*-----------------------------------------------*/
  /* set values to be returned to calling function */
  /*-----------------------------------------------*/
  *spdu_len = envelope_len + udata_len;
  *spdu_ptr = (char *) enc_buf;

  return (SD_SUCCESS);
}


/************************************************************************/
/*			cosp_enc_fn_dn					*/
/*----------------------------------------------------------------------*/
/* Function to encode a FINISH or DISCONNECT SPDU.			*/
/* This function assumed that there is enough place in the enc_buf for	*/
/* the FN or DN SPDU encoding. The function cosp_envelope_len can be	*/
/* used to compute exact envelope encoding length.			*/
/* Note that this function assumes that the SS-user data (RLRQ or RLRE	*/
/* APDU) have been already copied into the enc_buf in proper position.	*/
/* 									*/
/* Parameters:								*/
/*  ACSE_CONN	*con		Connection info parameters		*/
/*  char       **spdu_ptr	Pointer where to return pointer to SPDU	*/
/*  ST_UINT	*spdu_len	Pointer where to return the SPDU len	*/
/*  ST_UCHAR	 spdu_type	COSP_SI_FINISH or COSP_SI_DISCON	*/
/*	  								*/
/* Return:								*/
/*	SD_SUCCESS (0)		if successful				*/
/*	error code		otherwise				*/
/************************************************************************/
ST_RET cosp_enc_fn_dn (ACSE_CONN *con, char **spdu_ptr, ST_UINT *spdu_len,
			ST_UCHAR spdu_type)
{
ST_UINT	envelope_len;
ST_UINT	udata_len;
ST_UCHAR	       *enc_buf;
ST_UINT	enc_len;
ST_UINT  	idx;

  /*------------------------------------------------------------*/
  /* make sure data len is OK (User Data mandatory)		*/
  /*------------------------------------------------------------*/
  udata_len = con->ppdu_len;
  envelope_len  = cosp_envelope_len (con, 0, 0, spdu_type);

  if (udata_len == 0 || udata_len > COSP_MAX_UDATA)
    {
    COSP_LOG_ERR2 ("COSP-ERROR: Encoding FN/DN: Invalid User Data length=%u (0<length<=%u).",
      udata_len, COSP_MAX_UDATA);
    return (COSP_ERR_INV_UDATA_LEN);
    }

  /* set the pointer to beginning of encoding in the buffer (global buf)*/
  enc_buf = con->ppdu_ptr - envelope_len;
  if (enc_buf < cosp_buf)
    {
    COSP_LOG_ERR0 ("COSP-ERROR: Encoding FN/DN: Invalid pointer to Global Encoding Buffer.");
    return (COSP_ERR_INV_POINTER);
    }

  idx = 0;

  /*------------------------------------------*/
  /* Encode FN or DN code and total SPDU len  */
  /*------------------------------------------*/
  /* SPDU length to encode does not include 1 byte for FN or DN code	*/
  /* and 1 or 3 bytes for total len 					*/

  if (envelope_len-2 + udata_len <= 254)
    enc_len = envelope_len-2 + udata_len;	/* len enc on 1 byte	*/
  else
    enc_len = envelope_len-4 + udata_len;	/* len enc on 3 bytes	*/

  enc_buf [idx++] = spdu_type;
  idx += cosp_enc_len ((ST_UINT16) enc_len, &enc_buf [idx]);

  /*--------------------------------------------------------------------*/
  /* encode User Data (length only), SS-user Data should be already in	*/
  /* the enc_buf (in proper position).					*/
  /*--------------------------------------------------------------------*/
   enc_buf [idx++] = 193;				/* PI code	*/
   idx += cosp_enc_len ((ST_UINT16) udata_len, &enc_buf [idx]);

  /*-----------------------------------------------*/
  /* set values to be returned to calling function */
  /*-----------------------------------------------*/
  *spdu_len = envelope_len + udata_len;
  *spdu_ptr = (char *) enc_buf;

  return (SD_SUCCESS);
}


/************************************************************************/
/*			cosp_enc_u_ab					*/
/*----------------------------------------------------------------------*/
/* Function to encode a SS-user generated  ABORT SPDU			*/
/* This function assumed that there is enough place in the enc_buf for	*/
/* the AB SPDU encoding. The function cosp_envelope_len can be used	*/
/* to compute exact envelope encoding length.				*/
/* Note that this function assumes that the SS-user data (ARP /ATU PPDU)*/
/* has been already copied into the enc_buf in proper position.		*/
/* 									*/
/* Parameters:								*/
/*  ACSE_CON	*con		Connection info parameters		*/
/*  char      **spdu_ptr	Pointer where to return pointer to SPDU	*/
/*  ST_UINT	*spdu_len	Pointer where to return the SPDU len	*/
/*	  								*/
/* Return:								*/
/*	SD_SUCCESS (0)		if successful				*/
/*	error code		otherwise				*/
/************************************************************************/
ST_RET cosp_enc_u_ab (ACSE_CONN *con, char **spdu_ptr, ST_UINT *spdu_len)
{
ST_UINT	envelope_len;
ST_UINT	udata_len;
ST_UCHAR	       *enc_buf;
ST_UINT	enc_len;
ST_UINT  	idx;

  /*------------------------------------------------------------*/
  /* make sure data len is OK (depending on the reason the User	*/
  /* Data or the Reflect Parameter presence is mandatory)	*/
  /*------------------------------------------------------------*/
  udata_len = con->ppdu_len;
  envelope_len  = cosp_envelope_len (con, 0, 0, COSP_SI_ABORT);

  if (udata_len == 0 || udata_len > COSP_MAX_UDATA)
    {
    COSP_LOG_ERR2 ("COSP-ERROR: Encoding U-AB: Invalid User Data length=%u (0<length<=%u).",
      udata_len, COSP_MAX_UDATA);
    return (COSP_ERR_INV_UDATA_LEN);
    }

  /* set the pointer to beginning of encoding in the buffer (global buf)*/
  enc_buf = con->ppdu_ptr - envelope_len;
  if (enc_buf < cosp_buf)
    {
    COSP_LOG_ERR0 ("COSP-ERROR: Encoding U-AB: Invalid pointer to Global Encoding Buffer.");
    return (COSP_ERR_INV_POINTER);
    }

  idx = 0;

  /*------------------------------------*/
  /* Encode AB code and total SPDU len  */
  /*------------------------------------*/
  /* SPDU length to encode does not include 1 byte for AB code and	*/
  /* 1 or 3 bytes for total len 					*/

  if (envelope_len-2 + udata_len <= 254)
    enc_len = envelope_len-2 + udata_len;	/* len enc on 1 byte	*/
  else
    enc_len = envelope_len-4 + udata_len;	/* len enc on 3 bytes	*/

  enc_buf [idx++] = COSP_SI_ABORT;
  idx += cosp_enc_len ((ST_UINT16) enc_len, &enc_buf [idx]);

  /*----------------------------------------------------*/
  /* Encode Transport Disonnect PI (release TP conn)	*/
  /*----------------------------------------------------*/
  enc_buf [idx++] = 17;					/* PI code	*/
  enc_buf [idx++] = 1;					/* PI len	*/
  enc_buf [idx++] = COSP_TCONN_RELEASE | COSP_AB_REASON_USER_ABORT;

  /*--------------------------------------------------------------------*/
  /* encode User Data (length only), SS-user Data should be already	*/
  /* in the enc_buf (in proper position).				*/
  /*--------------------------------------------------------------------*/
  if (udata_len > 0)
    {
    /* encode User Data	*/
    enc_buf [idx++] = 193;				/* PGI code	*/
    idx += cosp_enc_len ((ST_UINT16) udata_len, &enc_buf [idx]);
    /* udata should be already in enc_buf */
    }

  /*-----------------------------------------------*/
  /* set values to be returned to calling function */
  /*-----------------------------------------------*/
  *spdu_len = envelope_len + udata_len;
  *spdu_ptr = (char *) enc_buf;

  return (SD_SUCCESS);
}

/************************************************************************/
/*			cosp_enc_p_ab					*/
/*----------------------------------------------------------------------*/
/* Function to encode a SS-provider generated  ABORT SPDU		*/
/* This function assumed that there is enough place in the enc_buf for	*/
/* the AB SPDU encoding (COSP_P_AB_LEN bytes).				*/
/* 									*/
/* Parameters:								*/
/*  char       *spdu_ptr	Pointer to encode buffer for SPDU	*/
/*  ST_INT	err_code	This value will be passed for AB reason	*/
/*	  								*/
/* Return:								*/
/*	SD_SUCCESS (0)		if successful				*/
/*	error code		otherwise				*/
/************************************************************************/
ST_RET cosp_enc_p_ab (char *spdu_ptr, ST_INT err_code)
{

  /*------------------------------------*/
  /* Encode AB code and total SPDU len  */
  /*------------------------------------*/

  spdu_ptr [0] = COSP_SI_ABORT;
  spdu_ptr [1] = 3 + 4;

  /*----------------------------------------------------*/
  /* Encode Transport Disonnect PI (release TP conn)	*/
  /*----------------------------------------------------*/
  spdu_ptr [2] = 17;					/* PI code	*/
  spdu_ptr [3] = 1;					/* PI len	*/
  spdu_ptr [4] = COSP_TCONN_RELEASE | COSP_AB_REASON_PROT_ERROR;

  /* encode the Reflect Parameter PI */
  spdu_ptr [5] = 49;					/* PI code	*/
  spdu_ptr [6] = (ST_UCHAR) 2;				/* PI len (0-9)	*/
  spdu_ptr [7] = (ST_UCHAR) ((err_code >> 8) & 0x00FF);
  spdu_ptr [8] = (ST_UCHAR)  (err_code       & 0x00FF);

  /* !!! if number of encoded bytes changes make sure that the define	*/
  /* !!! COSP_P_AB_SPDU_LEN is changed accordingly.			*/

  return (SD_SUCCESS);
}


/************************************************************************/
/*			cosp_enc_dt					*/
/*----------------------------------------------------------------------*/
/* Function to encode a DATA SPDU					*/
/* This function assumed that there is enough place in the enc_buf for	*/
/* the GIVE-TOKEN ans DT SPDU encoding.					*/
/* The function cosp_envelope_len can be used to compute exact envelope	*/
/* encoding length.							*/
/* Note that this function assumes that the SS-user data have been	*/
/* already copied into the enc_buf in proper position.			*/
/* 									*/
/* Parameters:								*/
/*  ACSE_CON	*con		Connection info parameters		*/
/*  char       **spdu_ptr	Pointer where to return pointer to SPDU	*/
/*  ST_UINT	*spdu_len	Pointer where to return the SPDU len	*/
/*	  								*/
/* Return:								*/
/*	SD_SUCCESS (0)		if successful				*/
/*	error code		otherwise				*/
/************************************************************************/
ST_RET cosp_enc_dt (ACSE_CONN *con, char **spdu_ptr, ST_UINT *spdu_len)
{
ST_UINT	envelope_len;
ST_UINT	udata_len;
ST_UCHAR	       *enc_buf;
ST_UINT  	idx;

  /*------------------------------------------------------------*/
  /* make sure data len is OK 					*/
  /*------------------------------------------------------------*/
  udata_len     = con->ppdu_len;
  envelope_len  = cosp_envelope_len (con, 0, 0, COSP_SI_DATA);

  if (udata_len == 0)
    {
    COSP_LOG_ERR2 ("COSP-ERROR: Encoding DT: Invalid User Data length=%u (0<length<=%u).",
      udata_len, COSP_MAX_UDATA);
    return (COSP_ERR_INV_UDATA_LEN);
    }

  /* set the pointer to beginning of encoding in the buffer (global buf)*/
  enc_buf = con->ppdu_ptr - envelope_len;
  if (enc_buf < cosp_buf)
    {
    COSP_LOG_ERR0 ("COSP-ERROR: Encoding DT: Invalid pointer to Global Encoding Buffer.");
    return (COSP_ERR_INV_POINTER);
    }

  idx = 0;

  /*--------------------------------------------*/
  /* Encode GIVE-TOKEN code and params len LI=0	*/
  /*--------------------------------------------*/

  enc_buf [idx++] = COSP_SI_GIVE_TOKEN;
  enc_buf [idx++] = 0;			/* no parameters present LI=0	*/

  /*------------------------------------*/
  /* Encode DT code and params len LI=0	*/
  /*------------------------------------*/

  enc_buf [idx++] = COSP_SI_DATA;
  enc_buf [idx++] = 0;			/* no parameters present LI=0	*/
  /* udata should be already in enc_buf */

  /*-----------------------------------------------*/
  /* set values to be returned to calling function */
  /*-----------------------------------------------*/
  *spdu_len = envelope_len + udata_len;
  *spdu_ptr = (char *) enc_buf;

  return (SD_SUCCESS);
}


/************************************************************************/
/*			cosp_enc_len					*/
/*----------------------------------------------------------------------*/
/* Function to encode a parameter's len. If len<=254 then 1 byte	*/
/* will be used to encode the length. If len>254 then 3 bytes will	*/
/* be used to encode the length.					*/
/* 									*/
/* Parameters:								*/
/*  ST_UINT16	  len		Parameter's length to encode		*/
/*  ST_UCHAR        *buf		Pointer where to encode the len		*/
/* 									*/
/* Return:								*/
/*  ST_UINT		Number of bytes used in encoding of len		*/
/************************************************************************/
static ST_UINT cosp_enc_len (ST_UINT16 len, ST_UCHAR *buf)
{
ST_UINT bytes;

  if (len <= 254)
    {
    buf [0] = (ST_UCHAR) len;
    bytes = 1;
    }
  else
    {
    buf [0] = (ST_UCHAR) 0xFF;			/* indicates len>254	*/
    buf [1] = (ST_UCHAR)((len >> 8) & 0x00FF);	/* high order byte	*/
    buf [2] = (ST_UCHAR) (len       & 0x00FF);	/* low order byte	*/
    bytes = 3;
    }

  return (bytes);
}

