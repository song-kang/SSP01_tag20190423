/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : ard_strn.c						*/
/* PRODUCT(S)  : ASN1DE 						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 03/11/04  GLB     04    Remove "thisFileName"                    	*/
/* 12/20/01  JRB     03    Chg ASN1_CTXT to ASN1_DEC_CTXT.		*/
/* 09/13/99  MDE     02    Added SD_CONST modifiers			*/
/* 07/26/99  MDE     01    New module, derived from ad_strn.c		*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "asn1r.h"
#include "asn1log.h"

/************************************************************************/
/*			     get_strn					*/
/*   Reads character data from message into selected buffer. No error	*/
/*   checking is done. The calling routine must verify that the buffer	*/
/*   is big enough. Conversion from ASCII to local character represen-	*/
/*   tation is not necessary for MS-DOS.				*/
/************************************************************************/

ST_VOID asn1r_get_strn (ASN1_DEC_CTXT *ac, ST_CHAR *ptr)
  {
ST_INT i;

#ifdef	DEBUG_ASN1_DECODE
  if (!ptr)
    {
    slogCallStack (sLogCtrl,
                    "get_strn: attempt to reference through a NULL pointer");
    return;
    }
#endif

  for (i = ac->asn1r_elmnt_len; i; --i)
    *(ptr++) = (ST_CHAR) *(ac->asn1r_field_ptr++);
  }

