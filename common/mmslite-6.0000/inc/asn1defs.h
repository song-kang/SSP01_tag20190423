/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : asn1defs.h						*/
/* PRODUCT(S)  : ASN1DE 						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	Contains definitions and global variables useful to the ASN1DE	*/
/*	user.								*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 12/19/01  JRB     14    Del everything & use "asn1r.h".		*/
/* 11/14/01  EJV     13    Added MMS_UTC_TIME typedef.			*/
/*			   Added asn1_wr_utc_time, asn1_get_utc_time	*/
/* 06/22/00  DSF     12    Added asn1_max_bitcount			*/
/* 06/16/00  JRB     11    Add #if INT64_SUPP...			*/
/* 05/24/00  RKR     10    Deleted a an extra declaration of asn1_tzone	*/
/* 09/13/99  MDE     09    Added SD_CONST modifiers			*/
/* 10/08/98  MDE     08    Migrated to updated SLOG interface		*/
/* 03/18/98  JRB     07    Add ASN1E_* defines.				*/
/*			   Move ASN1 error codes from mmsintr2.h to here*/
/*			   Add 0x7000 to all ASN.1 error codes.		*/
/* 12/10/97  MDE     06    Changed get_i8 to take ST_INT8		*/
/* 09/29/97  MDE     05    Changed ASN1_LITE to MMS_LITE		*/
/* 08/21/97  MDE     04    Added ASN1_ARB_FLOAT define			*/
/* 08/13/97  MDE     03    Changed wr_i8 and wr_u8 to take sized int's	*/
/* 06/17/97  MDE     02    Removed old debug defines			*/
/* 05/20/97  MDE     01    Changes to time handling - renames, etc.	*/
/* 04/02/97	   7.00    MMSEASE 7.0 release. See ASN1ML70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#include "asn1r.h"		/* Simply use all new defs in this file	*/
