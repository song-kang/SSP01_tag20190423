/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2002, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : are_objid.c						*/
/* PRODUCT(S)  : ASN1DE 						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 03/11/04  GLB     07    Remove "thisFileName"                    	*/
/* 01/14/03  JRB     06    Avoid redundant logging of encode overrun.	*/
/* 03/05/02  JRB     05    Eliminate warnings.				*/
/* 01/22/02  JRB     04    Chg asn1r_end_of_buffer to asn1r_buf_start.	*/
/* 12/20/01  JRB     03    Chg ASN1_CTXT to ASN1_ENC_CTXT.		*/
/* 09/13/99  MDE     02    Added SD_CONST modifiers			*/
/* 07/26/99  MDE     01    New module, derived from ae_objid.c		*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "asn1r.h"
#include "asn1log.h"

/************************************************************************/
/* Static functions in this module					*/

static ST_VOID _wr_component (ASN1_ENC_CTXT *ac, ST_INT16);

/************************************************************************/
/*			     wr_objid					*/
/*  Function to write an object identifier data element to a message	*/
/*  being built.							*/
/************************************************************************/

ST_VOID asn1r_wr_objid (ASN1_ENC_CTXT *ac, ST_INT16 component_list[], ST_INT num_components)
  {
ST_INT i;
ST_INT16 icomp;

  for (i = num_components - 1; i > 1; i--)
    _wr_component(ac, component_list[i]);

  icomp = component_list[1] + component_list[0]*40;
  _wr_component(ac, icomp);
  }

/************************************************************************/
/*			     _wr_component				*/
/*  Function to write an integer to a field of octets in the following	*/
/*  format:	[1xxxxxxx] ...	0xxxxxxx   , where all but the last	*/
/*  octet starts with a 1 bit, and where the x's are the bits of the    */
/*  binary integer input to the function (distributed 7 bits per octet).*/
/************************************************************************/

static ST_VOID _wr_component (ASN1_ENC_CTXT *ac, ST_INT16 icomp)
  {
  if (ac->asn1r_field_ptr -2 < ac->asn1r_buf_start)
    {
    ac->asn1r_encode_overrun = SD_TRUE;
    return;
    }

  if (icomp < 0x80)
    *(ac->asn1r_field_ptr--) = (ST_UCHAR) icomp;
  else
    {
    *(ac->asn1r_field_ptr--) = (ST_UCHAR) (icomp & 0x7F);
    *(ac->asn1r_field_ptr--) = (ST_UCHAR) (((icomp >> 7) & 0x7F) | 0x80);
    }
  }

