/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*              1991 All Rights Reserved       				*/
/*									*/
/* MODULE NAME : arg.c							*/
/* PRODUCT(S)  : 							*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 04/14/03  JRB    04     Eliminate compiler warnings.			*/
/* 04/13/98  MDE    03     Minor change to * arg handling		*/
/* 07/12/94  RWM    02     Inserted a string pointer pointer in order to*/
/*                         pass the rest of the arguments after the -p  */
/*                         parameter has been processed.  Also revised  */
/*                         was the spacing of the printf statement on   */
/*                         line 98                                      */
/* 06/23/94  MDE    01     Created					*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "arg.h"
#include "str_util.h"

/************************************************************************/

ST_VOID listValidArgs (ST_VOID);

/************************************************************************/

/* Return value from process_args, can set in arg process funs		*/
ST_INT argRetVal;	

/* Set SD_TRUE to halt argument progrssing					*/
ST_INT argStopProc;

/* Current argument being processed, can be set in arg process funs	*/
/* to skip arguments							*/
ST_INT argCurrArg;

/* Global pointer to argument list					*/
ST_CHAR **argArgv;

/************************************************************************/
/************************************************************************/

ST_INT process_args (ST_INT argc, ST_CHAR **argv)
  {
ST_INT j;
ST_INT cmpLen;

  argRetVal = SD_SUCCESS;
  argStopProc = SD_FALSE;
  argArgv = argv;

/* process all arguments, unless bad arg or told to stop		*/
  for (argCurrArg = 1; argCurrArg < argc && !argStopProc; ++argCurrArg)
    {
    for (j = 0; j < numArgStrings; ++j)
      {
      cmpLen = strlen (argCtrl[j].str);
      if (argv[argCurrArg][0] == '-' || argv[argCurrArg][0] == '/')
        argv[argCurrArg][0] = '-';

      if (argCtrl[j].str[0] == '*' ||
          !strnicmp (argv[argCurrArg],argCtrl[j].str,cmpLen))
        {     		/* found a match				*/
        (*argCtrl[j].fun)(argCurrArg, argv[argCurrArg]);
        break; 
        }

      } /* looking for match						*/

    if (j == numArgStrings)			/* if not found		*/
      {
      printf ("Invalid Argument : '%s'", argv[argCurrArg]);
      listValidArgs ();
      return (SD_FAILURE);
      }
    } /* for each argument passed in					*/
  return (argRetVal);
  }

/************************************************************************/

ST_VOID listValidArgs ()
  {
ST_INT j;

/* print all arguments and help						*/
  printf ("\n\n Valid Arguments :");
  for (j = 0; j < numArgStrings; ++j)
    {
    printf ("\n\t%-5.5s\t: %s", argCtrl[j].str, argCtrl[j].help);
    } 
  }


