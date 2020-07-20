/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 1997, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : glberror.h						*/
/* PRODUCT(S)  : MMSEASE						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/* 	This module contains defines for all the entities that may 	*/
/*	generate error code.                                            */
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 10/30/98  DSF     02    Added E_ACTIVEMMS				*/
/* 03/18/98  JRB     01    Added E_ASN1_DEC.				*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/


/*** NOTE : These defines are for reference only, and are not used in	*/
/* 	    creating the actual error code defines.			*/

#ifndef GBLERROR_INCLUDED
#define GBLERROR_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#define E_SUBSYS        0x0     /* underlying MAP subsystem:            */
                                /* Board or Board Driver (non-SISCO)    */

#define E_DRIVER        0x100   /* SISCO MAP Board Driver               */
#define E_DH            0x200   /* SISCO Data Highway Driver            */

#define EX_DRIVER       0x500   /* SISCO MAP Board Driver Exceptions    */
#define EX_DH           0x600   /* SISCO Data Highway Driver Exceptions */

#define E_CASESUIC      0xA00   /* CASE SUIC                            */
#define E_ACSESUIC      0xB00   /* ACSE SUIC                            */

#define EX_CASESUIC     0xE00   /* CASE SUIC Exceptions                 */
#define EX_ACSESUIC     0xF00   /* ACSE SUIC Exceptions                 */

#define E_TP4		0x1200	/* TP4 errors (see tp4api.h)		*/

#define E_CASESUIC_IS   0x1400   /* CASE SUIC Implementation Specific   */
#define E_ACSESUIC_IS   0x1500   /* ACSE SUIC Implementation Specific   */

#define EX_CASESUIC_IS  0x1900   /* CASE SUIC Implementation Specific Exceptions        */
#define EX_ACSESUIC_IS  0x1A00   /* ACSE SUIC Implementation Specific Exceptions        */

#define E_SESSIF        0x1E00  /* Session Interface                    */
#define E_CASE          0x1F00  /* SISCO CASE Layer Provider            */

#define EX_SESSIF       0x2400  /* Session Interface Exceptions         */
#define EX_CASE         0x2500  /* SISCO CASE Layer Provider Exceptions */

#define E_SESSIF_IS     0x2800  /* Session IF Implementation Spec 	*/
#define E_CASE_IS       0x2900  /* SISCO CASE Layer Provider Imp. Spec  */

#define EX_SESSIF_IS    0x2D00  /* Session IF Implementation Spec Exceptions            */
#define EX_CASE_IS      0x2E00  /* SISCO CASE Layer Provider Imp. Spec Exceptions       */

#define E_ACSE2		0x3000	/* ACSE2 errors (see acse2usr.h)	*/
#define E_COPP		0x3100	/* COPP errors (see copp_usr.h)		*/
#define E_COSP		0x3200	/* COSP errors (see cosp_usr.h)		*/
#define E_CLNP		0x3400	/* CLNP errors (see clnp_usr.h)		*/
#define E_SUBNET	0x3500	/* SUBNET errors (see clnp_usr.h)	*/
#define E_ADLC		0x3600	/* ADLC errors (see adlc_usr.h)		*/
#define E_RLI_IPC	0x3700	/* RLI_IPC errors (see rli_ipce.h)	*/

#define	E_CFG_UTIL	0x4000	/* CFG UTIL codes			*/
#define	E_HLM		0x4100	/* HLM codes				*/
#define	E_HLM_MMS	0x4200	/* HLM MMS code				*/

#define E_ACTIVEMMS	0x4A00	/* ActiveMMS codes			*/

#define	E_CM		0x5100	/* CM error codes base			*/
#define	E_CRM		0x5200	/* CRM error codes base			*/

#define E_MMS_GEN	0x6400	/* General provider 			*/
#define E_MMS_PPI	0X6500 	/* Paired Primitive Interface 		*/
#define	E_MMS_RESP	0x6600	/* Request Control Block (resp_err)	*/
#define	E_MMS_VMI	0x6700	/* Virtual Machine Interface		*/
#define	E_MMS_EXCPT	0x6800	/* MMS exceptions			*/
#define	E_LLP_ACSE	0x6900	/* ACSE LLP codes			*/
#define	E_LLP_CASE	0x6A00	/* CASE LLP codes			*/
#define	E_LLP_LLC	0x6B00	/* LLC LLP codes			*/
#define	E_MMS_LITE	0x6A00	/* MMS-LITE codes			*/

#define E_MMS_DS	0x6B00	/* MMS Directory Services		*/

#define E_ASN1_DEC	0x7000	/* ASN.1 decode errors			*/

/* The following define indicates the beginning of a range for		*/
/* User-Defined error codes.  Users may define error codes above this	*/
/* value without conflicting with SISCO defined errors.			*/

#define E_USER_APP	0x7E00	/* User Application errors		*/

/* ******** DO NOT DEFINE ANY ERROR CODES ABOVE "0x7E00" ********	*/

#ifdef __cplusplus
}
#endif

#endif

