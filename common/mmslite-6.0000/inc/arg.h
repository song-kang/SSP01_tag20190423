/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*              1991 All Rights Reserved       				*/
/*									*/
/* MODULE NAME : arg.h							*/
/* PRODUCT(S)  : 							*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 02/25/98  MDE    02     Changed numArgStrings to ST_INT		*/
/* 06/23/94  MDE    01     Created					*/
/************************************************************************/
/************************************************************************/


ST_INT process_args (ST_INT argc, ST_CHAR **argv);
ST_VOID listValidArgs (ST_VOID);

/************************************************************************/

struct arg_ctrl 
  {
  ST_CHAR *str;					/* text to be matched 	*/
  ST_CHAR *help;					/* Help text	      	*/
  ST_VOID (*fun)(ST_INT argNum, ST_CHAR *str);	/* processing function	*/
  };

extern ST_INT argRetVal;
extern ST_INT argStopProc;
extern ST_INT argCurrArg;
extern ST_CHAR    **argArgv;


/* User must define these variables */
extern struct arg_ctrl argCtrl[];
extern ST_INT numArgStrings;

