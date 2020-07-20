/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	      1998 - 2001, All Rights Reserved		        */
/*									*/
/* MODULE NAME : server.h    						*/
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
/* 04/23/03  JRB     05    Add sample UTF8Vstring var (UTF8Vstring13test).*/
/* 04/18/02  JRB     04    Increase some MVL_NUM_DYN_* values.		*/
/* 02/04/02  JRB     03    Don't use MVL_NUM_CALLED.			*/
/*			   Add MVL_NUM_DYN_* defines.			*/ 
/* 11/26/01  EJV     02    Added UtcTimeNow variable.			*/
/* 12/31/97  MDE     01    New file					*/
/************************************************************************/

typedef struct struct1	/* STRUCT1_TYPE	TDL="{(s)Short,(l)Long}"	*/
  {
  ST_INT16 s;
  ST_INT32 l;
  } STRUCT1;

extern ST_INT16 arr1[10];
extern ST_INT16 Temperature;
extern STRUCT1 struct1;
extern MMS_UTC_TIME UtcTimeNow;	/* New MMS Type: Utctime		*/
#if (UNICODE_LOCAL_FORMAT==UNICODE_UTF16)
extern ST_UINT16 UTF8Vstring13test [13+1];	/* local representation is UTF16*/
#else
extern ST_CHAR UTF8Vstring13test [13+1];	/* local representation is UTF8*/
#endif

extern ST_INT16 domArr1[10];
extern ST_INT16 domTemperature;
extern STRUCT1 domStruct1;

extern MVL_VAR_PROC varProcFuns;

extern ST_INT16 rptCtrl[];

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
#define MVL_NUM_DYN_JOUS	0
#define MVL_NUM_DYN_DOM_VARS	100
#define MVL_NUM_DYN_DOM_NVLS	10
#define MVL_NUM_DYN_AA_VARS	0	/* do not use foundry generated array*/
#define MVL_NUM_DYN_AA_NVLS	0
#define MVLU_NUM_DYN_TYPES	0
