/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2002, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : are_btod.c 						*/
/* PRODUCT(S)  : ASN1DE 						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 03/11/04  GLB     06    Remove "thisFileName"                    	*/
/* 01/14/03  JRB     05    Avoid redundant logging of encode overrun.	*/
/* 01/22/02  JRB     04    Chg asn1r_end_of_buffer to asn1r_buf_start.	*/
/* 12/20/01  JRB     03    Chg ASN1_CTXT to ASN1_ENC_CTXT.		*/
/* 09/13/99  MDE     02    Added SD_CONST modifiers			*/
/* 07/26/99  MDE     01    New module, derived from ae_btod.c		*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "asn1r.h"
#include "asn1log.h"

/************************************************************************/
/*			    wr_btod					*/
/************************************************************************/

ST_VOID asn1r_wr_btod (ASN1_ENC_CTXT *ac, MMS_BTOD *data)
  {
  /* check for overun of buffer */
  if (ac->asn1r_field_ptr - 6 < ac->asn1r_buf_start)
    {
    ac->asn1r_encode_overrun = SD_TRUE;
    return;
    }

  if (data->form == MMS_BTOD6)
    {
    *(ac->asn1r_field_ptr--) = (ST_UCHAR) (data->day & 0xFF);       /* low byte  */
    *(ac->asn1r_field_ptr--) = (ST_UCHAR) ((data->day >> 8) & 0xFF);/* high byte */
    }

  /* In any case, calculate and write the time component of the TimeOfDay */
  /* value.								*/
  *(ac->asn1r_field_ptr--) = (ST_UCHAR)  (data->ms & 0xFF);
  *(ac->asn1r_field_ptr--) = (ST_UCHAR) ((data->ms >> 8) & 0xFF);
  *(ac->asn1r_field_ptr--) = (ST_UCHAR) ((data->ms >> 16) & 0xFF);
  *(ac->asn1r_field_ptr--) = (ST_UCHAR) ((data->ms >> 24) & 0xFF);
  }

