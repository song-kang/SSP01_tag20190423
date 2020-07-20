/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	      1998 - 2001, All Rights Reserved		        */
/*									*/
/* MODULE NAME : uca_srvr.h   						*/
/* PRODUCT(S)  : MMSEASE-LITE						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*    This file contains declarations needed by rt_types.c to resolve	*/
/*    variable association initiailzation statements.			*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*	NONE								*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 03/23/11  JRB	   Add u_mvl_write_ind_61850			*/
/* 09/01/04  RKR     13    Changed MVLU_NUM_DYN_TYPES to 200		*/
/* 09/19/03  JRB     12    Del MVLU_USR_FUN_TBLS define (OBSOLETE).	*/
/* 04/16/03  JRB     11    Add u_int32_offset_rd(wr)_ind_fun.		*/
/*			   Define MVLU_USR_FUN_TBLS.			*/
/* 03/21/02  JRB     10	   Add MVL_NUM_DYN_* defines, defaults don't work*/
/* 11/21/01  EJV     09    Added UtcTimeNow var.			*/
/* 03/16/00  RKR     08    Changed (Q) and (SIT) models in the UCA RPT	*/
/* 09/07/99  MDE     07    Upgraded to use new UCA report mechanisms	*/
/* 09/07/99  MDE     06    Migrated to GOMSGE .9			*/
/* 10/21/98  MDE     05    Fixed nested comment				*/
/* 09/16/98  MDE     04    Deleted 'gRptCtrlPtrs', use new mvlu_rpt.c	*/
/* 08/12/98  MDE     03    Report handling changes			*/
/* 07/13/98  MDE     02    Report, other cleanup & fixup		*/
/* 12/31/97  MDE     01    New file					*/
/************************************************************************/

#ifndef offsetof
#define offsetof(s,m)   (size_t)((char *)&(((s *)0)->m) - (char *)0)
#endif

/************************************************************************/

#define GLOBE_RP_BRCBMX_RPT_ID	1
#define GLOBE_RP_BRCBST_RPT_ID	2

/* Defines required by Foundry generated code.				*/
/* Default values defined in "mvl_defs.h" but we don't like the defaults*/
/* so we undefine and redefine them here.				*/

#undef MVL_NUM_DYN_DOMS
#undef MVL_NUM_DYN_VMD_VARS
#undef MVL_NUM_DYN_VMD_NVLS
#undef MVL_NUM_DYN_JOUS
#undef MVL_NUM_DYN_DOM_VARS
#undef MVL_NUM_DYN_DOM_NVLS
#undef MVL_NUM_DYN_AA_VARS
#undef MVL_NUM_DYN_AA_NVLS
#undef MVLU_NUM_DYN_TYPES

#define MVL_NUM_DYN_DOMS	10
#define MVL_NUM_DYN_VMD_VARS	100
#define MVL_NUM_DYN_VMD_NVLS	10
#define MVL_NUM_DYN_JOUS	10
#define MVL_NUM_DYN_DOM_VARS	100
#define MVL_NUM_DYN_DOM_NVLS	10
#define MVL_NUM_DYN_AA_VARS	10
#define MVL_NUM_DYN_AA_NVLS	10
#define MVLU_NUM_DYN_TYPES	200	/* important for UCA	*/

/************************************************************************/
/* Function prototypes.							*/
/************************************************************************/
ST_VOID u_int32_offset_rd_ind_fun (MVLU_RD_VA_CTRL *mvluRdVaCtrl);
ST_VOID u_int32_offset_wr_ind_fun (MVLU_WR_VA_CTRL *mvluWrVaCtrl);

ST_VOID u_mvl_write_ind_61850 (MVL_IND_PEND *indCtrl);

