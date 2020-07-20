/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	1997 - 2009, All Rights Reserved				*/
/*									*/
/*			PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : cosp_dec.c						*/
/* PRODUCT(S)  : MOSI Stack (over TP4)					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*		This file implements the decoding of COSP SPDUs.	*/
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
/*			cosp_dec_cn_ac					*/
/*			cosp_dec_rf					*/
/*			cosp_dec_fn_dn					*/
/*			cosp_dec_ab					*/
/*			cosp_dec_dt					*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 11/17/09  JRB    09     Del SSEL chks from cosp_validate_cn_ac, now	*/
/*                         chked in "cosp_process_connect" (more portable)*/
/* 08/19/03  EJV    08     Allow for ACCEPT  User Data <= COSP_MAX_UDATA*/
/* 06/05/02  JRB    07     Allow Called SSEL=NULL on connect if		*/
/*			   configured SSEL=NULL.			*/
/* 04/26/01  EJV    06     Correction to version check in CN/AC.	*/
/*			   Corrected spelling.				*/
/* 09/13/99  MDE    05     Added SD_CONST modifiers			*/
/* 01/08/99  EJV    04     Added decoding of param 17 in FINISH SPDU.	*/
/*			   Make sure param 17 is extracted properly.	*/
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


	/*--------------------------------------*/
	/*	Local function prototypes	*/
	/*--------------------------------------*/

static ST_UINT16  cosp_dec_len   (ST_UCHAR *buf, ST_UINT *bytes);
static ST_RET cosp_validate_cn_ac (COSP_CN_AC *dec_par, ST_UCHAR spdu_type);


/************************************************************************/
/*			cosp_dec_cn_ac					*/
/*----------------------------------------------------------------------*/
/* Function to decode a CONNECT or ACCEPT SPDU.				*/
/* Note if User Data are present in received SPDU then the length in the*/
/* dec_par->udata_len will be > 0. The dec_par->udata_ptr points into	*/
/* the spdu_buf (it is not an allocated pointer).			*/
/* 									*/
/* Parameters:								*/
/*  COSP_CN_AC		*dec_par	ptr to decoded parameters struct*/
/*  char        	*spdu_buf	Pointer to received SPDU buf	*/
/*  ST_UINT     	 spdu_len	Length of received SPDU		*/
/*  ST_UCHAR		 spdu_type	SPDU type to decode (CN, AC)	*/
/*	  								*/
/* Return:								*/
/*	SD_SUCCESS (0)		if successful				*/
/*	error code		otherwise				*/
/************************************************************************/
ST_RET cosp_dec_cn_ac (COSP_CN_AC *dec_par, char *spdu_buf, ST_UINT spdu_len, 
			ST_UCHAR spdu_type)
{
ST_RET		ret;
ST_UCHAR	       *dec_buf;
ST_UINT	dec_len;
ST_UCHAR		param_code;
ST_UINT	param_len;
ST_UINT	group_len;
ST_UINT	len_bytes;	/* number of bytes for encoded len (1 or 3) */
ST_UINT	idx;

  ret = SD_SUCCESS;

  dec_buf = (ST_UCHAR *) spdu_buf;
  dec_len = 0;
  idx = 0;

  /*-----------------------------------------------------*/
  /* set defaults in case params are not present in SPDU */
  /*-----------------------------------------------------*/

  memset (dec_par, 0, sizeof (COSP_CN_AC));
  dec_par->ver_num        = COSP_VER1;		/*!default ver is Version1 */
  dec_par->ses_urequir [0]= 0x03;		/* bits 9-16: 9 and 10 set */
  dec_par->ses_urequir [1]= 0x49;		/* bits 1-8: 1,4 and 7 set */

  /* in addition memset is equivalent to following con params settings */
  /* dec_par->prot_option         = 0;   rcv extended concatination not supported	*/
  /* dec_par->initiator_tsdu_size = 0;   SSDU segmenting from initiator not supported	*/
  /* dec_par->responder_tsdu_size = 0;   SSDU segmenting from responder not supported	*/
  /* dec_par->loc_ssel [0]        = 0;   called SSEL is NULL				*/
  /* dec_par->rem_ssel [0]        = 0;   calling SSEL is NULL				*/
  /* dec_par->udata_len           = 0;   User Data length  				*/
  /* dec_par->udata_ptr           = 0;   User Data pointer (points into spdu_buf!)	*/

  /* we will use the first while loop to avoid excesive nesting of if...else... */

  while (SD_TRUE)
    {
    /* NOTE: we should never return back from bottom of the loop here!!! */

    /* the decoded len should be comparable with spdu_len passed to this func */
    dec_len = (ST_UINT) cosp_dec_len (&dec_buf [idx+1], &len_bytes);
    if (dec_len + 1+len_bytes != spdu_len)
      {
      ret = COSP_ERR_DEC_INV_LEN;
      COSP_LOG_ERR2 ("COSP-ERROR: Decoding CN/AC: invalid SPDU len=%u or decoded len=%u.",
        spdu_len, dec_len + 1+len_bytes);
      break;
      }
    idx += 1+len_bytes;		/* 1 for SPDU code */

    /* decode all connect parameters */
    while (dec_len > 0)
      {
      param_code = dec_buf [idx];
      param_len  = (ST_UINT) cosp_dec_len (&dec_buf [idx+1], &len_bytes);

      /* signal error if param_len is greater then decoding length left */
      if (param_len + 1+len_bytes > dec_len)
        {
        ret = COSP_ERR_DEC_INV_LEN;
        COSP_LOG_ERR2 ("COSP-ERROR: Decoding CN/AC: invalid parameter len=%u (length left to decode len=%u).",
          param_len + 1+len_bytes, dec_len);
	break;
	}
      dec_len -= 1+len_bytes;
      idx     += 1+len_bytes;

      /* do not try to decode if param_len is 0 (param is empty)	*/
      if (param_len == 0)
        continue;

      switch (param_code)
        {
	case  1:		/* PGI - Connection Identifier group 	*/
	  /* we will ignore this group (skip all decoding) */
	break;

	case  5:		/* PGI - Connect/Accept Item group	*/
          group_len = param_len;
	  while (group_len > 0)
	    {
            param_code = dec_buf [idx];
            param_len  = (ST_UINT) cosp_dec_len (&dec_buf [idx+1], &len_bytes);

            /* signal error if param_len is greater then decoding length left */
            if (param_len + 1+len_bytes > group_len)
              {
              ret = COSP_ERR_DEC_INV_LEN;
              COSP_LOG_ERR2 ("COSP-ERROR: Decoding CN/AC: invalid parameter len=%u (PGI-5 group length left to decode len=%u).",
                param_len + 1+len_bytes, group_len);
              break;
              }
            dec_len   -= 1+len_bytes;
            group_len -= 1+len_bytes;
            idx       += 1+len_bytes;

            /* do not try to decode if param_len is 0 (param is empty)	*/
            if (param_len == 0)
              continue;

            switch (param_code)
              {
              case  19:			/* PI - Protocol Options	*/
		if (param_len == 1)
                  dec_par->prot_option = dec_buf [idx];
		else
                  {
                  ret = COSP_ERR_DEC_INV_LEN;
                  COSP_LOG_ERR1 ("COSP-ERROR: Decoding CN/AC: invalid PI-19 parameter len=%u (should be len=1).",
                    param_len);
                  }
	      break;

	      case  21:			/* PI - TSDU Max Size		*/
		if (param_len == 4)
		  {
                  dec_par->initiator_tsdu_size = 
		    ((ST_UINT16) dec_buf [idx]   << 8) | (ST_UINT16) dec_buf [idx+1];
                  dec_par->responder_tsdu_size =
		    ((ST_UINT16) dec_buf [idx+2] << 8) | (ST_UINT16) dec_buf [idx+3];
		  }
		else
                  {
                  ret = COSP_ERR_DEC_INV_LEN;
                  COSP_LOG_ERR1 ("COSP-ERROR: Decoding CN/AC: invalid PI-21 parameter len=%u (should be len=4).",
                    param_len);
                  }
	      break;

	      case  22:			/* PI - Version Number		*/
		if (param_len == 1)
                  dec_par->ver_num = dec_buf [idx];
		else
                  {
                  ret = COSP_ERR_DEC_INV_LEN;
                  COSP_LOG_ERR1 ("COSP-ERROR: Decoding CN/AC: invalid PI-22 parameter len=%u (should be len=1)",
                    param_len);
                  }
	      break;

	      case  23:			/* PI - Initial Serial Number	*/
                /* we will ignore this param (skip all decoding) */
	      break;

	      case  26:			/* PI - Token Setting Item	*/
                /* we will ignore this param (skip all decoding) */
	      break;

	      default:
	        ret = COSP_ERR_DEC_INV_PI_CODE;
                COSP_LOG_ERR1 ("COSP-ERROR: Decoding CN/AC: invalid PI code=%d in PGI-5",
                  param_code);
	      } /* end of switch */

	    if (ret == SD_SUCCESS)
	      {
	      /* adjust processing vars by the length of param processed */
              dec_len   -= param_len;
              group_len -= param_len;
              idx       += param_len;
	      }
	    else
	      break;	/* error, exit this loop */
	    } /* end of while (group_len > 0) */

	  param_len = 0;	/* dec_len & idx already adjusted */
	break;

        case  16:			/* PI - Token Item	*/
	  if (spdu_type == COSP_SI_ACCEPT)
            /* we will ignore this param (skip all decoding) */
            ;
	  else
            {
	    ret = COSP_ERR_DEC_INV_PI_CODE;
            COSP_LOG_ERR1 ("COSP-ERROR: Decoding CN/AC: invalid PI code=%d", param_code);
            }
	break;

	case  20:	/* PI -  Session User Requirements param */
	  if (param_len == 2)
	    {
            dec_par->ses_urequir [0]= dec_buf [idx];	/* bits 9-16	*/
            dec_par->ses_urequir [1]= dec_buf [idx+1];	/* bits 1-8	*/
	    }
	  else
            {
            ret = COSP_ERR_DEC_INV_LEN;
            COSP_LOG_ERR1 ("COSP-ERROR: Decoding CN/AC: invalid PI-20 parameter len=%u (should be len=2).",
              param_len);
            }
	break;

	case  51:	/* PI -  Calling Session Selector param */
          if (param_len <= MAX_SSEL_LEN) /* 16 */
            {
            if (spdu_type == COSP_SI_CONNECT)
              {
              dec_par->rem_ssel [0] = (ST_UCHAR) param_len;
              memcpy (&dec_par->rem_ssel [1], &dec_buf [idx], param_len);
              }
            else
              {
              /* if COSP_SI_ACCEPT then same as Calling in S-CONNECT */
              dec_par->loc_ssel [0] = (ST_UCHAR) param_len;
              memcpy (&dec_par->loc_ssel [1], &dec_buf [idx], param_len);
	      }
	    }
	  else
            {
            ret = COSP_ERR_DEC_INV_LEN;
            COSP_LOG_ERR2 ("COSP-ERROR: Decoding CN/AC: invalid PI-51 parameter len=%u (should be len<=%u).",
              param_len, MAX_SSEL_LEN);
            }
	break;

	case  52:	/* PI - Called(CN)/Responding(AC) Session Selector param */
	  if (param_len <= MAX_SSEL_LEN) /* 16 */
	    {
            if (spdu_type == COSP_SI_CONNECT)
              {
              dec_par->loc_ssel [0] = (ST_UCHAR) param_len;
	      memcpy (&dec_par->loc_ssel [1], &dec_buf [idx], param_len);
	      }
	    else
	      {
              /* if COSP_SI_ACCEPT then Responder SSEL */
              dec_par->rem_ssel [0] = (ST_UCHAR) param_len;
	      memcpy (&dec_par->rem_ssel [1], &dec_buf [idx], param_len);
	      }
	    }
	  else
            {
            ret = COSP_ERR_DEC_INV_LEN;
            COSP_LOG_ERR2 ("COSP-ERROR: Decoding CN/AC: invalid PI-52 parameter len=%u (should be len<=%u).",
              param_len, MAX_SSEL_LEN);
            }
	break;

	case  193:	/* PGI - User Data - used only if len <=512 in CONNECT and	*/
                        /* len up to 65539 total SPDU size in ACCEPT (ISO 8327/DAD2)	*/
          if ((spdu_type == COSP_SI_CONNECT && param_len <= 512) ||
	      (spdu_type == COSP_SI_ACCEPT  && param_len <= COSP_MAX_UDATA))
            {
            dec_par->udata_len = param_len;
            dec_par->udata_ptr = &dec_buf [idx];
            }
          else
            {
            ret = COSP_ERR_DEC_INV_LEN;
            COSP_LOG_ERR2 ("COSP-ERROR: Decoding CN/AC: invalid PI-193 (User Data) parameter len=%u (max should be len<=%u).",
              param_len, (spdu_type == COSP_SI_CONNECT) ? 512 : COSP_MAX_UDATA);
            }
	break;

	case  194:	/* PGI - Extended User Data - used if len>512	*/
          if (param_len <= COSP_MAX_UDATA_CON)	/* and len<=10240 	*/
            {
            dec_par->udata_len = param_len;
            dec_par->udata_ptr = &dec_buf [idx];
            }
          else
            {
            ret = COSP_ERR_DEC_INV_LEN;
            COSP_LOG_ERR2 ("COSP-ERROR: Decoding CN/AC: invalid PI-194 (User Data) parameter len=%u (should be len<=%u).",
              param_len, COSP_MAX_UDATA_CON);
            }
	break;

	default:
	  ret = COSP_ERR_DEC_INV_PI_CODE;
          COSP_LOG_ERR1 ("COSP-ERROR: Decoding CN/AC: invalid PI code=%d", param_code);
	} /* end of switch */

      /* adjust processing vars by the length of param processed */
      if (ret == SD_SUCCESS)
        {
        dec_len -= param_len;
        idx     += param_len;
	}
      else
        break;
      } /* end while len to decode */

    break;	/* this is end of decoding, exit loop unconditionally */
    }  /* end of while (SD_TRUE) loop */

  if (ret == SD_SUCCESS)
    /* validate the received SPDU */
    ret = cosp_validate_cn_ac (dec_par, spdu_type);

  return (ret);
}


/************************************************************************/
/*			cosp_dec_rf					*/
/*----------------------------------------------------------------------*/
/* Function to decode a REFUSE SPDU.					*/
/* Note if User Data are present in received SPDU then the length in the*/
/* dec_par->udata_len will be > 0. The dec_par->udata_ptr points into	*/
/* the spdu_buf (it is not an allocated pointer).			*/
/* 									*/
/* Parameters:								*/
/*  COSP_RF		*dec_par	ptr to decoded parameters struct*/
/*  char        	*spdu_buf	Pointer to received SPDU buf	*/
/*  ST_UINT     	 spdu_len	Length of received SPDU		*/
/*	  								*/
/* Return:								*/
/*	SD_SUCCESS (0)		if successful				*/
/*	error code		otherwise				*/
/************************************************************************/
ST_RET cosp_dec_rf (COSP_RF *dec_par, char *spdu_buf, ST_UINT spdu_len)
{
ST_RET		ret;
ST_UCHAR	       *dec_buf;
ST_UINT	dec_len;
ST_UCHAR		param_code;
ST_UINT	param_len;
ST_UINT	len_bytes;	/* number of bytes for encoded len (1 or 3) */
ST_UINT  	idx;

  ret = SD_SUCCESS;

  dec_buf = (ST_UCHAR *) spdu_buf;
  dec_len = 0;
  idx = 0;

  /*-----------------------------------------------------*/
  /* set defaults in case params are not present in SPDU */
  /*-----------------------------------------------------*/

  memset (dec_par, 0, sizeof (COSP_RF));
  dec_par->disconnect          = 1;		/* default disconnect transport	*/
  dec_par->ver_num             = COSP_VER1;	/* default is Version1		*/
  dec_par->ses_urequir [0]     = 0x03;		/* bits 9-16: 9 and 10 set	*/
  dec_par->ses_urequir [1]     = 0x49;		/* bits 1-8: 1,4 and 7 set	*/

  /* in addition memset is equivalent to following con params settings	*/
  /* dec_par->reason	     = 0;	rejected by called SS (SS-user?)*/
  /* dec_par->udata_len      = 0;	Length of User Data		*/
  /* dec_par->udata_ptr      = 0;       Pointer to User Data (points into spdu_buf!)	*/

  /* we will use the first while loop to avoid excesive nesting of if...else... */

  while (SD_TRUE)
    {
    /* NOTE: we should never return back from bottom of the loop here!!! */

    /* the decoded len should be comparable with spdu_len passed to this func */
    dec_len = (ST_UINT) cosp_dec_len (&dec_buf [idx+1], &len_bytes);
    if (dec_len + 1+len_bytes != spdu_len)
      {
      ret = COSP_ERR_DEC_INV_LEN;
      COSP_LOG_ERR2 ("COSP-ERROR: Decoding RF: invalid SPDU len=%u or decoded len=%u.",
        spdu_len, dec_len + 1+len_bytes);
      break;
      }
    idx += 1+len_bytes;		/* 1 for RF SPDU code */

    /* decode all REFUSE parameters */
    while (dec_len > 0)
      {
      param_code = dec_buf [idx];
      param_len  = (ST_UINT) cosp_dec_len (&dec_buf [idx+1], &len_bytes);

      /* signal error if param_len is greater then decoding length left */
      if (param_len + 1+len_bytes > dec_len)
        {
        ret = COSP_ERR_DEC_INV_LEN;
        COSP_LOG_ERR2 ("COSP-ERROR: Decoding RF: invalid parameter len=%u (length left to decode len=%u).",
          param_len + 1+len_bytes, dec_len);
	break;
	}
      dec_len -= 1+len_bytes;
      idx     += 1+len_bytes;

      /* do not try to decode if param_len is 0 (param is empty)	*/
      if (param_len == 0)
        continue;

      switch (param_code)
        {
	case  1:		/* PGI - Connection Identifier group 	*/
	  /* we will ignore this group (skip all decoding) */
	break;

        case  17:			/* PI - Transport Disconnect	*/
	  if (param_len == 1)
            dec_par->disconnect = (ST_BOOLEAN)(dec_buf [idx] & 0x01);
	  else
            {
            ret = COSP_ERR_DEC_INV_LEN;
            COSP_LOG_ERR1 ("COSP-ERROR: Decoding RF: invalid PI-17 parameter len=%u (should be len=1).",
              param_len);
            }
	break;

	case  20:	/* PI -  Session User Requirements param */
	  if (param_len == 2)
	    {
            dec_par->ses_urequir [0]= dec_buf [idx];	/* bits 9-16	*/
            dec_par->ses_urequir [1]= dec_buf [idx+1]; /* bits 1-8	*/
	    }
	  else
            {
            ret = COSP_ERR_DEC_INV_LEN;
            COSP_LOG_ERR1 ("COSP-ERROR: Decoding RF: invalid PI-20 parameter len=%u (should be len=2).",
              param_len);
            }
	break;

        case  22:			/* PI - Version Number		*/
	  if (param_len == 1)
            dec_par->ver_num = dec_buf [idx];
	  else
            {
            ret = COSP_ERR_DEC_INV_LEN;
            COSP_LOG_ERR1 ("COSP-ERROR: Decoding RF: invalid PI-22 parameter len=%u (should be len=1).",
              param_len);
            }
	break;

	case  50:			/* PI -  Reason Code param */
          dec_par->reason = dec_buf [idx];	/* refuse reason code	*/

          if (param_len > 1)
            {
            dec_par->udata_len = param_len-1;
            dec_par->udata_ptr = &dec_buf [idx+1];
            }
	break;

	default:
	  ret = COSP_ERR_DEC_INV_PI_CODE;
          COSP_LOG_ERR1 ("COSP-ERROR: Decoding RF: invalid PI code=%d", param_code);
	} /* end of switch */

      /* adjust processing vars by the length of param processed */
      if (ret == SD_SUCCESS)
        {
        dec_len -= param_len;
        idx     += param_len;
	}
      else
        break;
      } /* end while len to decode */

    break;	/* this is end of decoding, exit loop unconditionally */
    }  /* end of while (SD_TRUE) loop */

  /* validate the received SPDU */
  if (ret == SD_SUCCESS)
    {
    /* User Data may be present only if refuse reason=2	*/
    if (dec_par->reason != COSP_RF_REASON_U_REJECT)
      if (dec_par->udata_len > 0)
        {
        ret = COSP_ERR_DEC_INV_RF_UDATA;
        COSP_LOG_ERR1 ("COSP-ERROR: Decoding RF: User Data present (len=%u) when Refuse Reason!=2",
          dec_par->udata_len);
        }
    }

  return (ret);
}


/************************************************************************/
/*			cosp_dec_fn_dn					*/
/*----------------------------------------------------------------------*/
/* Function to decode a FINISH or DISCONNECT SPDU.			*/
/* Note if User Data are present in received SPDU then the length in the*/
/* dec_par->udata_len will be > 0. The dec_par->udata_ptr points into	*/
/* the spdu_buf (it is not an allocated pointer).			*/
/* 									*/
/* Parameters:								*/
/*  COSP_FN_DN		*dec_par	Ptr to decoded parameters struct*/
/*  char        	*spdu_buf	Pointer to received SPDU buf	*/
/*  ST_UINT     	 spdu_len	Length of received SPDU		*/
/*  ST_UCHAR		 spdu_type	SPDU type to decode (FN, DN)	*/
/*	  								*/
/* Return:								*/
/*	SD_SUCCESS (0)		if successful				*/
/*	error code		otherwise				*/
/************************************************************************/
ST_RET cosp_dec_fn_dn (COSP_FN_DN *dec_par, char *spdu_buf, ST_UINT spdu_len,
			ST_UCHAR spdu_type)
{
ST_RET		ret;
ST_UCHAR	       *dec_buf;
ST_UINT	dec_len;
ST_UCHAR		param_code;
ST_UINT	param_len;
ST_UINT	len_bytes;	/* number of bytes for encoded len (1 or 3) */
ST_UINT  	idx;

  ret = SD_SUCCESS;

  dec_buf = (ST_UCHAR *) spdu_buf;
  dec_len = 0;
  idx = 0;

  /*-----------------------------------------------------*/
  /* set defaults in case params are not present in SPDU */
  /*-----------------------------------------------------*/

  memset (dec_par, 0, sizeof (COSP_FN_DN));
  if (spdu_type == COSP_SI_FINISH)
    dec_par->disconnect        = 1;	/* default disconnect transport	*/

  /* in addition memset is equivalent to following con params settings	*/
  /* dec_par->udata_len      = 0;	Length of User Data		*/
  /* dec_par->udata_ptr      = 0;       Pointer to User Data (points into spdu_buf!)	*/

  /* we will use the first while loop to avoid excesive nesting of if...else... */

  while (SD_TRUE)
    {
    /* NOTE: we should never return back from bottom of the loop here!!! */

    /* the decoded len should be comparable with spdu_len passed to this func */
    dec_len = (ST_UINT) cosp_dec_len (&dec_buf [idx+1], &len_bytes);
    if (dec_len + 1+len_bytes != spdu_len)
      {
      ret = COSP_ERR_DEC_INV_LEN;
      COSP_LOG_ERR2 ("COSP-ERROR: Decoding FN/DN: invalid SPDU len=%u or decoded len=%u.",
        spdu_len, dec_len + 1+len_bytes);
      break;
      }
    idx += 1+len_bytes;		/* 1 for FN/DN SPDU code */

    /* decode all FINISH / DISCONNECT parameters */
    while (dec_len > 0)
      {
      param_code = dec_buf [idx];
      param_len  = (ST_UINT) cosp_dec_len (&dec_buf [idx+1], &len_bytes);

      /* signal error if param_len is greater then decoding length left */
      if (param_len + 1+len_bytes > dec_len)
        {
        ret = COSP_ERR_DEC_INV_LEN;
        COSP_LOG_ERR2 ("COSP-ERROR: Decoding FN/DN: invalid parameter len=%u (length left to decode len=%u).",
          param_len + 1+len_bytes, dec_len);
	break;
	}
      dec_len -= 1+len_bytes;
      idx     += 1+len_bytes;

      /* do not try to decode if param_len is 0 (param is empty)	*/
      if (param_len == 0)
        continue;

      switch (param_code)
        {
	case  17:	/* PI - Transport Disconnect */
          /* this param is valid only in FINISH SPDU */
          if (spdu_type == COSP_SI_FINISH)
            {
            dec_par->disconnect = (ST_BOOLEAN)(dec_buf [idx] & 0x01);
            if (dec_par->disconnect != 1)
              {
              /* we do NOT support reusing transport connection */
              ret = COSP_ERR_DEC_INV_PI_CODE;
              COSP_LOG_ERR1 ("COSP-ERROR: Decoding FN: PI code=%d (keep transport connection not supported)", param_code);
              }
            }
          else
            {
            ret = COSP_ERR_DEC_INV_PI_CODE;
            COSP_LOG_ERR1 ("COSP-ERROR: Decoding DN: invalid PI code=%d", param_code);
            }
	break;

	case  193:	/* PGI - User Data group */
          dec_par->udata_len = param_len;
          dec_par->udata_ptr = &dec_buf [idx];
	break;

	default:
	  ret = COSP_ERR_DEC_INV_PI_CODE;
          COSP_LOG_ERR1 ("COSP-ERROR: Decoding FN/DN: invalid PI code=%d", param_code);
	} /* end of switch */

      /* adjust processing vars by the length of param processed */
      if (ret == SD_SUCCESS)
        {
        dec_len -= param_len;
        idx     += param_len;
	}
      else
        break;
      } /* end while len to decode */

    break;	/* this is end of decoding, exit loop unconditionally */
    }  /* end of while (SD_TRUE) loop */

  /* validate the received SPDU */
  if (ret == SD_SUCCESS)
    {
    if (dec_par->udata_len == 0)
      {
      ret = COSP_ERR_INV_UDATA_LEN;
      COSP_LOG_ERR0 ("COSP-ERROR: Decoding FN/DN: invalid User Data len=0");
      }
    }

  return (ret);
}


/************************************************************************/
/*			cosp_dec_ab					*/
/*----------------------------------------------------------------------*/
/* Function to decode an ABORT SPDU.					*/
/* Note if User Data are present in received SPDU then the length in the*/
/* dec_par->udata_len will be > 0. The dec_par->udata_ptr points into	*/
/* the spdu_buf (it is not an allocated pointer).			*/
/* 									*/
/* Parameters:								*/
/*  COSP_AB		*dec_par	Ptr to decoded parameters struct*/
/*  char        	*spdu_buf	Pointer to received SPDU buf	*/
/*  ST_UINT     	 spdu_len	Length of received SPDU		*/
/*	  								*/
/* Return:								*/
/*	SD_SUCCESS (0)		if successful				*/
/*	error code		otherwise				*/
/************************************************************************/
ST_RET cosp_dec_ab (COSP_AB *dec_par, char *spdu_buf, ST_UINT spdu_len)
{
ST_RET		ret;
ST_UCHAR	       *dec_buf;
ST_UINT	dec_len;
ST_UCHAR		param_code;
ST_UINT	param_len;
ST_UINT	len_bytes;	/* number of bytes for encoded len (1 or 3) */
ST_UINT  	idx;

  ret = SD_SUCCESS;

  dec_buf = (ST_UCHAR *) spdu_buf;
  dec_len = 0;
  idx = 0;

  /*-----------------------------------------------------*/
  /* set defaults in case params are not present in SPDU */
  /*-----------------------------------------------------*/

  memset (dec_par, 0, sizeof (COSP_AB));
  dec_par->disconnect        = 1;	/* default disconnect transport	*/

  /* in addition memset is equivalent to following con params settings	*/
  /* dec_par->reason	     = 0;	undefined reason		*/
  /* dec_par->reflect_par_len= 0;	Len of  implementation defined protocol err code*/
  /* dec_par->reflect_par [9]= 0;	Buf for implementation defined protocol err code*/
  /* dec_par->udata_len      = 0;	Length of User Data		*/
  /* dec_par->udata_ptr      = 0;       Pointer to User Data (points into spdu_buf!)	*/

  /* we will use the first while loop to avoid excesive nesting of if...else... */

  while (SD_TRUE)
    {
    /* NOTE: we should never return back from bottom of the loop here!!! */

    /* the decoded len should be comparable with spdu_len passed to this func */
    dec_len = (ST_UINT) cosp_dec_len (&dec_buf [idx+1], &len_bytes);
    if (dec_len + 1+len_bytes != spdu_len)
      {
      ret = COSP_ERR_DEC_INV_LEN;
      COSP_LOG_ERR2 ("COSP-ERROR: Decoding AB: invalid SPDU len=%u or decoded len=%u.",
        spdu_len, dec_len + 1+len_bytes);
      break;
      }
    idx += 1+len_bytes;		/* 1 for AB SPDU code */

    /* decode all ABORT parameters */
    while (dec_len > 0)
      {
      param_code = dec_buf [idx];
      param_len  = (ST_UINT) cosp_dec_len (&dec_buf [idx+1], &len_bytes);

      /* signal error if param_len is greater then decoding length left */
      if (param_len + 1+len_bytes > dec_len)
        {
        ret = COSP_ERR_DEC_INV_LEN;
        COSP_LOG_ERR2 ("COSP-ERROR: Decoding AB: invalid parameter len=%u (length left to decode len=%u).",
          param_len + 1+len_bytes, dec_len);
	break;
	}
      dec_len -= 1+len_bytes;
      idx     += 1+len_bytes;

      /* do not try to decode if param_len is 0 (param is empty)	*/
      if (param_len == 0)
        continue;

      switch (param_code)
        {
	case  17:		/* PI - Transport Disconnect */
	  if (param_len == 1)
	    {
            dec_par->disconnect = (ST_BOOLEAN) (dec_buf [idx] & 0x01);
            dec_par->reason     = (ST_UCHAR)   (dec_buf [idx] & 0x1E);
            }
	  else
            {
            ret = COSP_ERR_DEC_INV_LEN;
            COSP_LOG_ERR1 ("COSP-ERROR: Decoding AB: invalid PI-17 parameter len=%u (should be len=1).",
              param_len);
            }
	break;

	case  49:			/* PI -  Reflect param */
	  if (param_len <= 9)
	    {
            dec_par->reflect_par_len = param_len;
            memcpy (dec_par->reflect_par, &dec_buf [idx], param_len);
	    }
	  else
            {
            ret = COSP_ERR_DEC_INV_LEN;
            COSP_LOG_ERR1 ("COSP-ERROR: Decoding AB: invalid PI-49 parameter len=%u (should be len<=9).",
              param_len);
            }
	break;

	case  193:	/* PGI - User Data group */
          dec_par->udata_len = param_len;
          dec_par->udata_ptr = &dec_buf [idx];
	break;

	default:
	  ret = COSP_ERR_DEC_INV_PI_CODE;
          COSP_LOG_ERR1 ("COSP-ERROR: Decoding AB: invalid PI code=%d", param_code);
	} /* end of switch */

      /* adjust processing vars by the length of param processed */
      if (ret == SD_SUCCESS)
        {
        dec_len -= param_len;
        idx     += param_len;
	}
      else
        break;
      } /* end while len to decode */

    break;	/* this is end of decoding, exit loop unconditionally */
    }  /* end of while (SD_TRUE) loop */

  /* validate the received SPDU */
  if (ret == SD_SUCCESS)
    {
    if (dec_par->reason == COSP_AB_REASON_PROT_ERROR)
      if (dec_par->reflect_par_len == 0)
        {
        ret = COSP_ERR_DEC_INV_AB_RP;
        COSP_LOG_ERR0 ("COSP-ERROR: Decoding AB: invalid Reflect Parameter len=0");
        }

    if (dec_par->reason == COSP_AB_REASON_USER_ABORT)
      if (dec_par->udata_len == 0)
        {
        ret = COSP_ERR_INV_UDATA_LEN;
        COSP_LOG_ERR0 ("COSP-ERROR: Decoding AB: invalid User Data len=0");
        }
    }

  return (ret);
}


/************************************************************************/
/*			cosp_dec_dt					*/
/*----------------------------------------------------------------------*/
/* Function to decode an DATA SPDU.					*/
/* Note if User Data are present in received SPDU then the length in the*/
/* dec_par->udata_len will be > 0. The dec_par->udata_ptr points into	*/
/* the spdu_buf (it is not an allocated pointer).			*/
/* 									*/
/* Parameters:								*/
/*  COSP_DT		*dec_par	Ptr to decoded parameters struct*/
/*  char        	*spdu_buf	Pointer to received SPDU buf	*/
/*  ST_UINT     	 spdu_len	Length of received SPDU		*/
/*	  								*/
/* Return:								*/
/*	SD_SUCCESS (0)		if successful				*/
/*	error code		otherwise				*/
/************************************************************************/
ST_RET cosp_dec_dt (COSP_DT *dec_par, char *spdu_buf, ST_UINT spdu_len)
{
ST_RET		ret;
ST_UCHAR	       *dec_buf;
ST_UINT	dec_len;
ST_UINT	len_bytes;	/* number of bytes for encoded len (1 or 3) */
ST_UINT  	idx;

  ret = SD_SUCCESS;

  dec_buf = (ST_UCHAR *) spdu_buf;
  dec_len = 0;
  idx = 0;

  /*-----------------------------------------------------*/
  /* set defaults in case params are not present in SPDU */
  /*-----------------------------------------------------*/

  memset (dec_par, 0, sizeof (COSP_DT));

  /* memset is equivalent to following con params settings	*/
  /* dec_par->udata_len      = 0;	Length of User Data		*/
  /* dec_par->udata_ptr      = 0;       Pointer to User Data (points into spdu_buf!)	*/

  /*--------------------------------------------------------------------*/
  /* the protocol calls for GIVE-TOKEN and DT SPDU to be concatinated	*/
  /* even if we do not use the token mechanism.				*/
  /*--------------------------------------------------------------------*/

  dec_len = (ST_UINT) cosp_dec_len (&dec_buf [idx+1], &len_bytes);
  if (dec_len == 0)
    {
    idx += 1+len_bytes;
    /* token mechanism not used, decode DT */
    if (dec_buf [idx] == COSP_SI_DATA)
      {
      /*------------------------------------------------------------*/
      /* note that Enclosure Item PI should not be present because	*/
      /* we do not support segmenting (see CONNECT)			*/
      /*------------------------------------------------------------*/

      /* get the params length, should be 0, we do not send/rcv any params in DT */
      dec_len = (ST_UINT) cosp_dec_len (&dec_buf [idx+1], &len_bytes);
      if (dec_len == 0)
        {
        idx += 1+len_bytes;  /* 1 for DT SPDU code and 1 byte for params len LI */

        dec_par->udata_len = spdu_len - (2 + 1+len_bytes);
        dec_par->udata_ptr = &dec_buf [idx];
        }
      else
        {
        ret = COSP_ERR_DEC_INV_LEN;
        COSP_LOG_ERR0 ("COSP-ERROR: Decoding DT: Decoding of parameters in DATA SPDU not supported.");
        }
      }
    else
      {
      ret = COSP_ERR_DEC_INV_SPDU;
      COSP_LOG_ERR1 ("COSP-ERROR: Decoding DT: Invalid SPDU code=%u detected after Token SPDU.",
        (ST_UINT) dec_buf [idx]);
      }
    }
  else
    {
    ret = COSP_ERR_DEC_INV_LEN;
    COSP_LOG_ERR0 ("COSP-ERROR: Decoding DT: Parameters in Token SPDU not supported.");
    }

  return (ret);
}


/************************************************************************/
/*			cosp_dec_len					*/
/*----------------------------------------------------------------------*/
/* Function to decode a parameter's length (8-bit or 16-bit value).	*/
/* 									*/
/* Parameters:								*/
/*  ST_UCHAR	*buf		Pointer to len in received SPDU buf	*/
/*  ST_UINT	*bytes		Pointer where to return number of	*/
/*				bytes for decoded field			*/
/*	  								*/
/* Return:								*/
/*	ST_UINT16			the length of a parameter		*/
/************************************************************************/
static ST_UINT16 cosp_dec_len (ST_UCHAR *buf, ST_UINT *bytes)
{
ST_UINT16   len;

  if (buf [0] == 0xFF)
    {
    /* len >254      high order byte          low order byte		*/
    len  = ((ST_UINT16) buf [1] << 8) | (ST_UINT16) buf [2];	
    *bytes = 3;
    }
  else
    {
    /* len <=254 */
    len  = (ST_UINT16) buf [0];
    *bytes = 1;
    }

  return (len);
}


/************************************************************************/
/*			cosp_validate_cn_ac				*/
/*----------------------------------------------------------------------*/
/* Function to validate parameters in a CONNECT or ACCEPT SPDU.		*/
/* 									*/
/* Parameters:								*/
/*  COSP_CN_AC		*dec_par	ptr to decoded parameters struct*/
/*  ST_UCHAR		 spdu_type	SPDU type to validate (CN, AC)	*/
/*	  								*/
/* Return:								*/
/*	SD_SUCCESS (0)		if validation successful		*/
/*	error code		otherwise				*/
/************************************************************************/
static ST_RET cosp_validate_cn_ac (COSP_CN_AC *dec_par, ST_UCHAR spdu_type)
{
  /* NOTE: SSEL now checked in "cosp_process_connect" (more portable).	*/

  /* currently we support Version 2 of the Session Protocol		*/
  if (spdu_type == COSP_SI_CONNECT)
    {
    /* CONNECT, check if one of choices is COSP_VER2 */
    if ((dec_par->ver_num & COSP_VER2) == 0)
      {
      COSP_LOG_ERR2 ("COSP-ERROR: Decoding CN/AC: Session Protocol Version '%d' not supported (supported ver=%d).",
        dec_par->ver_num, COSP_VER2);
      return (COSP_ERR_DEC_INV_PROT_VER);
      }
    }
  else
    {
    /* ACCEPT, make sure COSP_VER2 was accepted	*/
    if (dec_par->ver_num != COSP_VER2)
      {
      COSP_LOG_ERR2 ("COSP-ERROR: Decoding CN/AC: Session Protocol Version '%d' not supported (supported ver=%d).",
        dec_par->ver_num, COSP_VER2);
      return (COSP_ERR_DEC_INV_PROT_VER);
      }
    }

  /* make sure extended concatination is not set (we do not support it)*/
  if (dec_par->prot_option != 0)
    {
    COSP_LOG_ERR0 ("COSP-ERROR: Decoding CN/AC: Extended Concatination not supported.");
    return (COSP_ERR_DEC_INV_PROT_OPT);
    }

  /* make sure that segmenting of SSDUs was not specified */
  if (dec_par->initiator_tsdu_size != 0 ||
      dec_par->responder_tsdu_size != 0)
    {
    COSP_LOG_ERR0 ("COSP-ERROR: Decoding CN/AC: SSDUs segmenting not supported.");
    return (COSP_ERR_DEC_INV_SEG);
    }

  if (spdu_type == COSP_SI_CONNECT)
    {
    /* we support only Duplex Functional Units, make sure it is available*/
    if (!(dec_par->ses_urequir [1] & 0x02))	/* is bit2 set?	*/
      {
      COSP_LOG_ERR0 ("COSP-ERROR: Decoding CN/AC: Duplex Functional Unit not proposed (Session User Requirements).");
      return (COSP_ERR_DEC_INV_FUN_UNITS);
      }
    }
  else
    {
    /* ACCEPT, make sure only Duplex Functional Units (FU) has been selected */
    if (dec_par->ses_urequir [0] != 0 ||	/* bits 9-16: must be 0	*/
        dec_par->ses_urequir [1] != 0x02)	/* bits 1-8:  bit2 set	*/
      {
      COSP_LOG_ERR0 ("COSP-ERROR: Decoding CN/AC: Only Duplex Functional Unit supported (Session User Requirements).");
      return (COSP_ERR_DEC_INV_FUN_UNITS);
      }
    }

  return (SD_SUCCESS);
}


