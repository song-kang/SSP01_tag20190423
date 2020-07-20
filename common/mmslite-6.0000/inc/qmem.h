/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*		1997, All Rights Reserved 				*/
/*									*/
/* MODULE NAME : qmem.h							*/
/* PRODUCT(S)  : Quick Memory Allocator					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

ST_RET qMemInit (ST_INT NumElem);
ST_VOID *qMemAlloc (ST_INT size);
ST_VOID qMemFree (ST_VOID *ptr);

extern ST_INT qMemElemSize;		/* default = 126 (must be power	*/
					/* of 2 - 2)			*/


#ifdef __cplusplus
}
#endif
