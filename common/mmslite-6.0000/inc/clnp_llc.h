/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1996-2002, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : clnp_llc.h						*/
/* PRODUCT(S)  : Lean-T Stack						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*		This header file contains defines for the encoding and	*/
/*		decoding of Logical Link Control (LLC) which is		*/
/*		implemented with the CLNP layer.			*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 08/12/04  JRB    04     Del LLC_LSAP_IEC_GOOSE 0xF5 (obsolete now).	*/
/* 01/10/02  JRB    03     Add LLC_LSAP_IEC_GOOSE.			*/
/* 09/09/97  EJV    02     Added support for UCA SMP (Station Mgt Prot)	*/
/* 05/27/97  JRB   7.00    MMSEASE 7.0 release.				*/
/* 06/04/96  EJV    01     Created					*/
/************************************************************************/

#ifndef CLNP_LLC_INCLUDED
#define CLNP_LLC_INCLUDED


#ifdef __cplusplus
extern "C" {
#endif


#define LLC_LSAP	(ST_UCHAR) 0xFE
#define LLC_CONTROL	(ST_UCHAR) 0x03

#define LLC_LSAP_SMP	(ST_UCHAR) 0xFB	/* For UCA SMP (Station Mgt Prot) */
#define LLC_CONTROL_SMP	(ST_UCHAR) 0x03 /* For UCA SMP (Station Mgt Prot) */

typedef struct
  {
  ST_UCHAR		dest;		/* destination SAP		*/
  ST_UCHAR		src;		/* source SAP			*/
  ST_UCHAR		control;	/* control field		*/
  }LLC_HDR;

#define LLC_HDR_LEN	sizeof (LLC_HDR)


/* Function prototypes from llc.c module */

ST_RET clnpl_llc_encode (ST_UCHAR *enc_buf, ST_UCHAR src_lsap, ST_UCHAR dest_lsap);
ST_RET clnpl_llc_decode (ST_UCHAR *dec_buf, ST_UCHAR *dest_lsap);


#ifdef __cplusplus
}
#endif

#endif  /* end of 'already included' 	*/


