/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1993 - 2005, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : mmsldefs.h						*/
/* PRODUCT(S)  : MMSEASE-LITE						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	Functions and variables below this point are used by the	*/
/*	application.							*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 01/11/12  JRB	   Updated copyright and version.		*/
/* 05/03/11  JRB	   Updated copyright and version.		*/
/* 01/04/11  RKR           Updated version.                             */
/* 09/16/10  JRB           Updated version.				*/
/* 06/08/10  RKR           Updated copyright and version.               */
/* 07/27/09  JRB     26    Updated version.				*/
/* 02/18/09  RKR     25    Updated version.				*/
/* 01/19/09  RKR     24    Updated version.				*/
/* 12/01/08  GLB     23    Updated version.				*/
/* 11/10/08  GLB     22    Updated version.				*/
/* 04/23/08  GLB     21    Updated version.				*/
/* 01/02/07  JRB     20    Updated version.				*/
/* 04/10/06  RKR     19    Updated copyright and version for SMV Beta   */
/* 10/28/05  EJV     18    Update ver to 5.0250 for MMS-LITE-SECURE rel.*/
/* 09/09/05  JRB     17    Update ver to 5.02 & Copyright info.		*/
/*			   BUILD_NUM is OBSOLETE.			*/
/* 04/28/05  PLM     16    Update ver to 5.01 Build 1			*/
/* 07/21/04  DWL     15    Update to 5.0052				*/
/* 02/20/04  ASK     14    Chg ver to 5.0051 for USPS release		*/
/* 01/26/04  DWL     13    Update copyright date to 2004.		*/
/* 09/10/03  EJV     12    Chg ver to 5.0050 (Security ready MMS-LITE)	*/
/* 04/29/03  DWL     11    Updated build number				*/
/* 04/08/03  DWL     10    Updated copyright/version/build number	*/
/* 12/20/02  DWL     09    Updated version/build number			*/
/* 03/22/02  DWL     08    Updated build number				*/
/* 03/12/02  DWL     07    Updated version				*/
/* 02/25/02  MDE     06    Removed mmsl_max_msg_size			*/
/* 10/09/01  JRB     05    Add MMSLITE_* product identification defines.*/
/* 02/20/98  MDE     04    Fixed __cplusplus problem			*/
/* 11/05/97  MDE     03    Changed mmsl_msg_start to ST_UCHAR *		*/
/* 08/24/97  MDE     02    Added 'mmsl_max_msg_size', 'ml_init_buffers' */
/* 11/10/93  DGE     01    Initial MMSEASE-LITE Release.  		*/
/************************************************************************/

#ifndef LMMSDEFS_INCLUDED
#define LMMSDEFS_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************/
/*		Defines for Product Identification.			*/
/* Write these parameters on screen and in log file at startup.		*/
/* Use MMSLITE_NAME & MMSLITE_VERSION in MMS Identify response.		*/
/************************************************************************/
#define MMSLITE_COPYRIGHT	"Copyright (c) 1986-2012 SISCO, Inc. All Rights Reserved."
#define MMSLITE_NAME		"MMS-LITE-80X-001"
#define MMSLITE_VERSION		"6.0000"
#define MMSLITE_BUILD_NUM	0	/* OBSOLETE: for backward compatibility only*/

/************************************************************************/
/************************************************************************/
/* MMSEASE-LITE specific Global Variables				*/
/************************************************************************/
#define MMS_PCI		0x0001

extern ST_INT    mmsl_enc_buf_size;
extern ST_UCHAR *mmsl_enc_buf;
extern ST_INT    mmsl_dec_info_size;

extern ST_INT16  mmsl_version;
extern ST_UINT32 mmsl_invoke_id;

extern ST_UCHAR  *mmsl_msg_start;
extern ST_INT    mmsl_msg_len;


/************************************************************************/
/* This function initializes the encode/decode buffers and the decode 	*/
/* object limitations, and is normally generated using the 'mbufcalc' 	*/
/* utility program.							*/

ST_VOID ml_init_buffers (ST_VOID);


#ifdef __cplusplus
}
#endif

#endif		/* MMSLDEFS_INCLUDED */
