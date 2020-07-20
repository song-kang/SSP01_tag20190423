/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : ard_bool.c						*/
/* PRODUCT(S)  : ASN1DE 						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 12/20/01  JRB     03    Chg ASN1_CTXT to ASN1_DEC_CTXT.		*/
/* 09/13/99  MDE     02    Added SD_CONST modifiers			*/
/* 07/26/99  MDE     01    New module, derived from ard_bool.c		*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "asn1r.h"
#include "asn1log.h"

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/
/************************************************************************/
/************************************************************************/
/*				get_bool				*/
/* Function to read an ASN.1 ST_BOOLEAN from a message being decoded	*/
/* Returns 0 if OK, non-zero if error.					*/
/************************************************************************/

ST_RET asn1r_get_bool (ASN1_DEC_CTXT *ac, ST_BOOLEAN *ptr)
  {
#ifdef	DEBUG_ASN1_DECODE
  if (!ptr)
    {
    slogCallStack (sLogCtrl,
                    "get_bool: attempt to reference through a NULL pointer");
    return(SD_FAILURE);
    }
#endif

  if (ac->asn1r_elmnt_len != 1)
    {
    ALOG_NERR0 ("ASN.1 decode: Boolean length not == 1");
    return (SD_FAILURE);
    }

  *ptr = (ST_BOOLEAN) *(ac->asn1r_field_ptr++);
  return (SD_SUCCESS);
  }

