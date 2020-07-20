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
/* 02/15/05  JRB     03    Eliminate //.				*/
/* 01/06/04  JRB     02    Del unused vars pbro_di, lun0_di.		*/
/* 04/18/03  JRB     01    New file based on uca_srvr project.		*/
/************************************************************************/

extern MMS_UTC_TIME UtcTimeNow;

extern ST_UINT16 Pbro_SG_Pu_Phsi_val;
extern ST_UINT16 Pbro_SG_Pu_neutrali_val;
extern ST_UINT16 Pbro_SG_Pu_hzi_val;

typedef struct
  {
  ST_BOOLEAN Out;
  ST_UINT8 Tar;
  struct
    {
    ST_UCHAR b2[1];
    ST_UCHAR q[2];
    ST_INT32 t_ms;  /* Btime6 is two ST_INT32 */
    ST_INT32 t_days;
    } FctDS;
    ST_UCHAR AuxIn1[2];
  ST_UINT8 PuGrp;
  } PBRO_ST;
extern PBRO_ST pbro_st;

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
#define MVLU_NUM_DYN_TYPES	100	/* important for UCA	*/

/************************************************************************/
/* Function prototypes.							*/
/************************************************************************/
ST_VOID u_int32_offset_rd_ind_fun (MVLU_RD_VA_CTRL *mvluRdVaCtrl);
ST_VOID u_int32_offset_wr_ind_fun (MVLU_WR_VA_CTRL *mvluWrVaCtrl);
