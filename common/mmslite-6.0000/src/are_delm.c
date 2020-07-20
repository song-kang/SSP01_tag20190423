/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2002, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : are_delmn.c						*/
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
/* 01/22/02  JRB     04    Chg asn1r_field_start to asn1r_field_end.	*/
/* 12/20/01  JRB     03    Chg ASN1_CTXT to ASN1_ENC_CTXT.		*/
/* 09/13/99  MDE     02    Added SD_CONST modifiers			*/
/* 07/26/99  MDE     01    New module, derived from ae_delmn.c		*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "asn1r.h"
#include "asn1log.h"


/************************************************************************/
/*			     wr_delmnt					*/
/* Function to write an entire data element to a message being built.	*/
/* Useful when another layer of software encodes a data element to be	*/
/* inserted into the current message.					*/
/************************************************************************/

ST_VOID asn1r_wr_delmnt (ASN1_ENC_CTXT *ac, ST_UCHAR *bufPtr, ST_INT bufLen)
  {
  asn1r_wr_octstr (ac, bufPtr, bufLen);
  ac->asn1r_field_end = ac->asn1r_field_ptr;
  }
