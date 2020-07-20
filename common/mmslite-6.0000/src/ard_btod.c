/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : ard_btod.c 						*/
/* PRODUCT(S)  : ASN1DE 						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 03/11/04  GLB     05    Remove "thisFileName"                    	*/
/* 03/05/02  JRB     04    Eliminate warnings.				*/
/* 12/20/01  JRB     03    Chg ASN1_CTXT to ASN1_DEC_CTXT.		*/
/* 09/13/99  MDE     02    Added SD_CONST modifiers			*/
/* 07/26/99  MDE     01    New module, derived from ard_btod.c		*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "asn1r.h"
#include "asn1log.h"

/************************************************************************/
/************************************************************************/
/*			    get_btod					*/
/************************************************************************/

ST_RET asn1r_get_btod (ASN1_DEC_CTXT *ac, MMS_BTOD *out)
  {
ST_INT32 ms;		/* ms time component			*/
ST_INT32 day = 0;	/* day component of TimeOfDay value	*/

#ifdef	DEBUG_ASN1_DECODE
  if (!out)
    {
    slogCallStack (sLogCtrl,
                    "get_btod: attempt to reference through a NULL pointer");
    return(SD_FAILURE);
    }
#endif

  /* Read Time Component (first 4 bytes)	*/
  ms =  (((ST_INT32) *(ac->asn1r_field_ptr++)) << 24) & 0xFF000000L;
  ms |= (((ST_INT32) *(ac->asn1r_field_ptr++)) << 16) & 0x00FF0000L;
  ms |= (((ST_INT32) *(ac->asn1r_field_ptr++)) << 8)  & 0x0000FF00L;
  ms |=  ((ST_INT32) *(ac->asn1r_field_ptr++))        & 0x000000FFL;

  /* Check to see if TimeOfDay is in long or short form  */
  if (ac->asn1r_elmnt_len == 4)
    {
    out->form = MMS_BTOD4;
    }
  else
    {
    out->form = MMS_BTOD6;
    day = (((ST_INT32) *(ac->asn1r_field_ptr++)) << 8) & 0xFF00;
    day |= ((ST_INT32) *(ac->asn1r_field_ptr++))       & 0x00FF;
    }

  out->ms = ms;
  out->day = day;
  return (SD_SUCCESS);
  }

