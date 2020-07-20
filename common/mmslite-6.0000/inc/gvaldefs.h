/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*              1986-1997 All Rights Reserved             		*/
/*									*/
/* MODULE NAME : gvaldefs.h						*/
/* PRODUCT(S)  : MMSEASE						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	This include file contains common declarations used throughout	*/
/*	the user application code.					*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#ifndef GVALDEFS_INCLUDED
#define GVALDEFS_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#define STRGETSTRINGSIZE 256

ST_BOOLEAN  floatget (ST_FLOAT *dest);
ST_BOOLEAN  doubleget( ST_DOUBLE *dest);
ST_BOOLEAN  intget  (ST_INT *dest);	/* int get function		*/
ST_BOOLEAN  hexget  (ST_INT *dest);	/* int get function		*/
ST_BOOLEAN  longget (ST_LONG *dest); 	/* long int get function	*/
ST_BOOLEAN  longhexget (ST_LONG *dest);	/* long int get function	*/
ST_BOOLEAN  strget  (ST_CHAR *dest); 	/* string get function		*/
ST_INT  input_hex (ST_UCHAR *dest_str, ST_INT maxbytes);

ST_VOID entry1 (ST_INT maxlen, ST_CHAR *str_ptr);

#ifdef __cplusplus
}
#endif

#endif

