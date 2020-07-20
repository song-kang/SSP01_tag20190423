#ifndef CHECKSUM_INCLUDED
#define CHECKSUM_INCLUDED
/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1996, All Rights Reserved.		        	*/
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : checksum.h						*/
/* PRODUCT(S)  : Lean-T Stack						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*		This header file contain functionprototypes for		*/
/*		implemented computation and verification of checksum	*/
/*		using the algorithm defined in Annex B of the ISO 8073	*/
/*		or in the ISO 8473.					*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/*			checksum_insert					*/
/*			checksum_verified				*/
/*									*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 05/27/97  JRB   7.00    MMSEASE 7.0 release.				*/
/* 06/19/96  EJV    01     Created					*/
/************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif


ST_VOID    checksum_insert   (ST_UCHAR *buf, ST_UINT16 buf_len, ST_UINT16 position);
ST_BOOLEAN checksum_verified (ST_UCHAR *buf, ST_UINT16 buf_len, ST_UINT16 position, ST_BOOLEAN clnp_csum);



#ifdef __cplusplus
}
#endif

#endif  /* end of 'already included' 	*/
