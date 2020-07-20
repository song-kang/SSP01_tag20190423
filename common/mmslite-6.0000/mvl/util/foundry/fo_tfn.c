/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,       */
/*               1997, 1998, All Rights Reserved                        */
/*                                                                      */
/* MODULE NAME : fo_tfn.c                                               */
/* PRODUCT(S)  :                                                        */
/*                                                                      */
/* MODULE DESCRIPTION :                                                 */
/*                                                                      */
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :                            */
/*              	                                                */
/* DEFINE_CTRL *findDefine (ST_CHAR *lValue, FILE *fp)			*/
/* FUN_CTRL *findFun (ST_CHAR *funName, FILE *fp)			*/
/* 									*/
/*                                                                      */
/* MODIFICATION LOG :                                                   */
/*  Date     Who   Rev                  Comments                        */
/* --------  ---  ------   -------------------------------------------  */
/* 02/02/10  NAV     09    OpenVMS Port					*/
/* 05/21/07  RKR     08    ported strpbrk call to gcc 4.1.1             */
/* 01/30/06  GLB     07    Integrated porting changes for VMS           */
/* 04/14/03  JRB     06    Eliminate compiler warnings.			*/
/* 11/26/02  JRB     05    Handle case when leaf fun contains "_REF".	*/
/* 05/17/00  JRB     04    strpbrk in parseExtern chk for "(" too.	*/
/* 05/19/00  JRB     03    Lint cleanup.				*/
/* 05/17/00  JRB     02    strpbrk chk for tab ('\t') as well as space.	*/
/*			   Log line if parse fails.			*/
/* 04/11/99  DWL     01    New module					*/
/************************************************************************/

#include <stdio.h>    
#include <stdlib.h>   
#include <string.h>      
#include <ctype.h>
#include "glbtypes.h"
#include "gen_list.h"
#include "mem_chk.h"

#include "foundry.h"

/************************************************************************/
#define S_SEARCH_START_OF_FUNC_NAME	0
#define S_SEARCH_START_OF_FUNC_ARG      1
#define S_SEARCH_END_OF_FUNC_ARG	2
#define S_SEARCH_START_OF_FUNC_BODY	3
#define S_SEARCH_END_OF_FUNC_BODY	4
#define S_SEARCH_END_OF_FUNC_PTYPE	5
#define S_SEARCH_START_OF_FUNC_DEF	6

#define MAX_FUN_SIZE  1024
/************************************************************************/
/* Static variables							*/
/************************************************************************/

SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
static ST_BOOLEAN bBuildingRValue = SD_FALSE;
static ST_BOOLEAN bBuildingFunction = SD_FALSE;

static ST_CHAR alphabet[] =
               {"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"};

/************************************************************************/
DEFINE_CTRL *parseDefine (ST_CHAR *ptrBuf);
FUN_CTRL *parseReplacedFun (ST_CHAR *ptrBuf);
FUN_CTRL *parseFun (FILE *tfnFp, ST_CHAR *ptrBuf);
FUN_CTRL *parseExtern (ST_CHAR *ptrBuf);
static ST_CHAR *pointToNextNonSpaceChar(ST_CHAR *ptr);
static ST_RET buildMyString (ST_CHAR *dstStr, 
				ST_CHAR **ppDstStr, 
				ST_INT dstStrSize, 
				const ST_CHAR *srcStr, 
				ST_INT count);
static ST_VOID initFindFunVars(ST_INT *state, 
			       ST_INT *numLines, 
			       ST_INT *braceCounter, 
			       ST_CHAR *funPrototype,
			       ST_CHAR **pFunPr,	
			       ST_INT funPrSize,
			       ST_RET *ret);
static ST_BOOLEAN isEndOfFunc(ST_CHAR *buf, ST_INT *bCount);

/************************************************************************/
/* This control structure is used only for defines of the form '*_REF'	*/

DEFINE_CTRL *tfnRefDefineList;

/* Use this to speed the search for defines ... quicksort, bsearch 	*/
ST_INT numRefDefines;
DEFINE_CTRL **sortedRefDefineTable;

/************************************************************************/
/* This control structure is to store information about read/write 	*/
/* functions, external declarations, and 'function replacement' defines	*/

FUN_CTRL *tfnFunList;

/* Use this to speed the search for functions ... quicksort, bsearch 	*/
ST_INT numTfnFuns;
FUN_CTRL **sortedTfnTable;

/************************************************************************/
/************************************************************************/
/* 		          processTfnFile				*/
/* Process the TFN file and create the lists shown above, then sort	*/
/* them using quicksort (actually, sort the pointer tables).		*/
/************************************************************************/

ST_VOID processTfnFile (FILE *tfnFp)
  {
DEFINE_CTRL *ctrlptr;
FUN_CTRL *funptr;
ST_CHAR buf[256];
ST_CHAR *pBuf;
ST_UINT i;
ST_CHAR *tmp;
ST_BOOLEAN goAhead;
ST_BOOLEAN parseError;
static ST_BOOLEAN bRefDefine = SD_FALSE;
static ST_BOOLEAN bRepFun = SD_FALSE;
static ST_BOOLEAN bExternFun = SD_FALSE;
static ST_BOOLEAN bInFun = SD_FALSE;

  while ( fgets(buf, sizeof(buf) - 1, tfnFp ) != NULL)
    {
    pBuf = buf;
    parseError = SD_FALSE;
    /* Check for blank line */
    goAhead = SD_FALSE;
    tmp = pBuf;
    for (i=0;i<strlen(buf)-1;i++,tmp++)
      {
      if (isprint (*tmp))
	{
	goAhead = SD_TRUE;
	break;
	}
      }
    if (!goAhead)
      continue;
    /* Establish type */
    if (strstr (pBuf, "#define") == NULL && strstr (pBuf, "ST_VOID") == NULL &&
        strstr (pBuf, "extern") == NULL && bBuildingRValue != SD_TRUE && 
        bBuildingFunction != SD_TRUE)
      continue;
    /* Need to preserve spacing if we are already in a function or ref define */
    if (bBuildingRValue == SD_FALSE && bBuildingFunction == SD_FALSE)
      pBuf = pointToNextNonSpaceChar(pBuf);
    if (bRefDefine == SD_TRUE || bRepFun == SD_TRUE || strncmp(pBuf,"#define",7) == 0)
      {
      /* Chk for REF & NOT rd_ind_fun or wr_ind_fun (funs may contain _REF)*/
      if ((strstr (pBuf, "_REF") != NULL
           && strstr (pBuf, "_rd_ind_fun") == NULL 
           && strstr (pBuf, "_wr_ind_fun") == NULL) || bRefDefine == SD_TRUE)
        {
	/* This is a reference define */
        ctrlptr = parseDefine (pBuf);
	if (ctrlptr != NULL)
	  {
	  list_add_last ((ST_VOID **) &tfnRefDefineList, ctrlptr);
	  bRefDefine = SD_FALSE;
	  }
	else
          {
          parseError = SD_TRUE;
	  bRefDefine = SD_TRUE;
          }
	}
      if (strstr (pBuf, "_ind_fun") != NULL || bRepFun == SD_TRUE)
        {
	/* This is a replaced function */
        funptr = parseReplacedFun (pBuf);
	if (funptr != NULL)
	  {
	  list_add_last ((ST_VOID **) &tfnFunList, funptr);
	  bRepFun = SD_FALSE;
	  }
	else
          {
          parseError = SD_TRUE;
	  bRepFun = SD_TRUE;
          }
	}
      }
    else if (bExternFun == SD_TRUE || strncmp(pBuf,"extern",6) == 0)
      {
      /* This is an extern */
      funptr = parseExtern (pBuf);
      if (funptr != NULL)
	{
        list_add_last ((ST_VOID **) &tfnFunList, funptr);
	bExternFun = SD_FALSE;
	}
      else
        {
        parseError = SD_TRUE;
	bExternFun = SD_TRUE;
        }
      }
    else if (bInFun == SD_TRUE || strncmp(pBuf,"ST_VOID",7) == 0)
      {
      /* This is an inline function */
      funptr = parseFun (tfnFp, pBuf);
      if (funptr != NULL)
	{
	list_add_last ((ST_VOID **) &tfnFunList, funptr);
	bInFun = SD_FALSE;
	}
      else
        {
        parseError = SD_TRUE;
	bInFun = SD_TRUE;
        }
      }
    if (parseError)
      SLOGALWAYS1 ("ERROR parsing line: %s", buf);
    }
  }

/************************************************************************/

ST_VOID logTfnProcessResults ()
  {
DEFINE_CTRL *ctrlptr;
FUN_CTRL *funptr;
FILE *output;

  /* This is to check output */
  output = fopen ("data.lst", "w");

  fprintf (output, "\n\n****** REFERENCE DEFINES ");
  ctrlptr = tfnRefDefineList;
  while (ctrlptr)
    {
    fprintf (output, "\n%s\t%s", ctrlptr->lValue, ctrlptr->rValue);
    ctrlptr = (DEFINE_CTRL *) list_get_next (tfnRefDefineList, ctrlptr);
    }

  fprintf (output, "\n\n****** READ/WRITE INDICATION FUNCTIONS");
  funptr = tfnFunList;
  while (funptr)
    {				   
    fprintf (output, "\n\n%s: ", funptr->funName);
    if (funptr->funType == FUNTYPE_IN_TFN)
      fprintf (output, "FUNTYPE_IN_TFN\n%s", funptr->functiondata);
    else if (funptr->funType == FUNTYPE_EXTERN)
      fprintf (output, "FUNTYPE_EXTERN\n%s", funptr->functiondata);
    else if (funptr->funType == FUNTYPE_REPLACED)
      fprintf (output, "FUNTYPE_REPLACED\n%s", funptr->replacementFunName);
    else
      fprintf (output, "FUNTYPE_NOT_FOUND");
      
    funptr = (FUN_CTRL *) list_get_next (tfnFunList, funptr);
    }

  fclose (output);
  }


/************************************************************************/
/* 		          findRefDefine					*/
/* Search for the reference define. These are always of the form *_REF,	*/
/* and have been put in the list 'tfnRefDefineList'. Just search the 	*/
/* list and return the element, or NULL for failure. 			*/
/************************************************************************/
DEFINE_CTRL *findRefDefine (ST_CHAR *lValue)
  {  
DEFINE_CTRL *ctrlptr;

  ctrlptr = tfnRefDefineList;
  while (ctrlptr)
    {
    if (strcmp (ctrlptr->lValue, lValue) == 0)
      return (ctrlptr);
    ctrlptr = (DEFINE_CTRL *) list_get_next (tfnRefDefineList, ctrlptr);
    }

  return (NULL); /* not found */
  }  

/************************************************************************/
/* 		          findTfnFun					*/
/* Search for the selected function. Note that this function may have	*/
/* been 'replaced' by another function; no matter, just search the 	*/
/* list and return the element, or FUN_NOT_FOUND for failure.		*/
/************************************************************************/
FUN_CTRL *findTfnFun (ST_CHAR *funName)
  {
FUN_CTRL *funptr;

  funptr = tfnFunList;
  while (funptr)
    {
    if (strcmp (funptr->funName, funName) == 0)
      return (funptr);
    funptr = (FUN_CTRL *) list_get_next (tfnFunList, funptr);
    }
  /* Fun not found, we return a FUN_CTRL with FUN_NOT_FOUND */
  funptr = (FUN_CTRL *) chk_calloc (1, sizeof(FUN_CTRL));
  funptr->funType = FUNTYPE_NOT_FOUND;
  return (funptr); /* not found */
  }

/************************************************************************/
/*				parseDefine				*/
/* Parse define statement and store pertinent info in structure		*/
/************************************************************************/

DEFINE_CTRL *parseDefine (ST_CHAR *ptrBuf)
  {  
static ST_CHAR lValue[256];
static ST_CHAR rValue[256];
static ST_CHAR *pRValue;
ST_BOOLEAN bDone;
ST_CHAR seps[] = " \t";
DEFINE_CTRL *pDefineCtrl;
ST_RET ret;
static ST_CHAR *tmpPtr;
ST_CHAR *pBuf;

  /* initialize local variable */
  bDone = SD_FALSE;
  pDefineCtrl = NULL;
  pRValue = rValue;
  if (bBuildingRValue == SD_FALSE)
    memset (rValue, 0x00, sizeof(rValue));
  pBuf = ptrBuf;
  ret = SD_SUCCESS;

  /* process line from the source file				*/
    if (bBuildingRValue == SD_FALSE)
      {
      pBuf += 7;
      if (isspace(*pBuf) == 0) /* should have a white space character */
        return (NULL);
      pBuf = pointToNextNonSpaceChar(pBuf);
      pBuf = strtok(pBuf,seps);
      if (pBuf == NULL)
        return (NULL);
      strcpy (lValue, pBuf);
      pBuf += strlen(pBuf) + 1;	/* we have parsed until lValue */
      pBuf = pointToNextNonSpaceChar(pBuf);
      if (pBuf == NULL) /* did we reach end of string */
        return (NULL);
      }
    bBuildingRValue = SD_FALSE;
    tmpPtr = pBuf;
    pBuf = strpbrk(pBuf,"\n\\");
    if (pBuf == NULL)
      return (NULL);
    if (*pBuf == '\n')
      {
      if (*(pBuf-1) == 0x0D)
	*(pBuf-1) = 0x00;
      else
	*pBuf = 0x00;
      ret |= buildMyString (rValue, &pRValue, sizeof(rValue), tmpPtr, strlen(tmpPtr));
      }
    else if (*pBuf == '\\')
      {
      bBuildingRValue = SD_TRUE;
      ret |= buildMyString (rValue, &pRValue, sizeof(rValue), tmpPtr, strlen(tmpPtr));
      }

  if (bBuildingRValue != SD_TRUE)
    {
    /* we have lValue and rValue so copy and return it */		
    pDefineCtrl = (DEFINE_CTRL *) chk_calloc (1, sizeof(DEFINE_CTRL));
    pDefineCtrl->lValue = chk_strdup (lValue);
    pDefineCtrl->rValue = chk_strdup (rValue);
    return(pDefineCtrl);
    }

  return (NULL);
  }  

/************************************************************************/
/*				parseReplacedFun			*/
/* Parse line and store info related to replaced function		*/
/* Information retrieved in similar manner as parseDefine, we just have	*/
/* a different return type.						*/
/************************************************************************/

FUN_CTRL *parseReplacedFun (ST_CHAR *ptrBuf)
  {  
static ST_CHAR lValue[256];
static ST_CHAR rValue[256];
ST_CHAR *pRValue;
ST_BOOLEAN bDone;
ST_CHAR seps[] = " \t";
FUN_CTRL *pFunCtrl;
ST_RET ret;
ST_CHAR *tmpPtr;
ST_CHAR *pBuf;

  /* initialize local variable */
  bDone = SD_FALSE;
  pFunCtrl = NULL;
  pRValue = rValue;
  if (bBuildingRValue == SD_FALSE)
    {
    memset (rValue, 0x00, sizeof(rValue));
    }
  pBuf = ptrBuf;
  ret = SD_SUCCESS;

  /* process line from the source file				*/
    if (bBuildingRValue == SD_FALSE)
      {
      pBuf += 7;
      if (isspace(*pBuf) == 0) /* should have a white space character */
        return (NULL);
      pBuf = pointToNextNonSpaceChar(pBuf);
      pBuf = strtok(pBuf,seps);
      if (pBuf == NULL)
        return (NULL);
      strcpy (lValue, pBuf);
      pBuf += strlen(pBuf) + 1;	/* we have parsed until lValue */
      pBuf = pointToNextNonSpaceChar(pBuf);
      if (pBuf == NULL) /* did we reach end of string */
        return (NULL);
      }
    bBuildingRValue = SD_FALSE;
    pBuf = pointToNextNonSpaceChar(pBuf);
    tmpPtr = pBuf;
    pBuf = strpbrk(pBuf,"\n\t\\");
    if (pBuf == NULL)
      return (NULL);
    if (*pBuf == '\n' || *pBuf == '\t')
      {
      if (*(pBuf-1) == 0x0D)
	*(pBuf-1) = 0x00;
      else
	*pBuf = 0x00;
      strcpy (rValue, tmpPtr);
      }
    else if (*pBuf == '\\')
      {
      bBuildingRValue = SD_TRUE;
      }

  if (bBuildingRValue != SD_TRUE)
    {
    /* we have lValue and rValue so copy and return it */		
    pFunCtrl = (FUN_CTRL *) chk_calloc (1, sizeof(FUN_CTRL));
    pFunCtrl->funName = chk_strdup (lValue);
    pFunCtrl->replacementFunName = chk_strdup (rValue);
    pFunCtrl->funType = FUNTYPE_REPLACED;
    return(pFunCtrl);
    }

  return (NULL);
  }  


/************************************************************************/
/* 				parseFun				*/
/* This function searches a function name in a file and copies the 	*/
/* whole function into a control block.					*/
/************************************************************************/
FUN_CTRL *parseFun (FILE *tfnFp, ST_CHAR *ptrBuf)
  {
ST_BOOLEAN bCallStateMachine; /* to parse more without getting new line */
ST_BOOLEAN bDone;	      /* to flag that we have found a function	*/
static ST_BOOLEAN bNewline;   /* New line read from file		*/
static ST_INT bCount;	      /* to count parenthesis and pilow mark	*/
static ST_INT state;	      /* to derive the state machine		*/	
ST_CHAR *pBuf;		      /* a pointer to the above buffer		*/	
ST_RET ret;		      /* to hold a return value			*/	
FUN_CTRL *pFunCtrl;	      /* retun value pointer of this function	*/	
ST_INT numLines;	      /* for one of the output of this function */	
ST_INT count;		      /* used in bldFunprototype		*/	
ST_CHAR *tmpPtr;	      /* to hold a character pointer temporarily*/
static ST_CHAR *pBeginFunName;/* Beginning of function name		*/
static ST_CHAR *pEndFunName;  /* End of function name			*/
static ST_INT funNameLen;     /* Function name length			*/
static ST_CHAR funName[128];  /* Function name				*/
static ST_CHAR *fun = NULL;   /* Function text				*/
static ST_INT funSize;		      
static ST_INT bufSize;		      
		
  /* initialize local variable */
  bNewline = SD_TRUE;
  bDone = SD_FALSE;
  ret = SD_SUCCESS;
  if (fun == NULL)
    {
    fun = (ST_CHAR *) chk_calloc (MAX_FUN_SIZE, sizeof (ST_CHAR)); /* This will be realloc'd as needed */
    bufSize = MAX_FUN_SIZE;
    }

  if (bBuildingFunction == SD_FALSE)
    {
    state = S_SEARCH_START_OF_FUNC_DEF;
    bCount = 0;
    numLines = 0;
    }
  pFunCtrl = NULL;
  pBuf = ptrBuf;

  /* got one line from the source file, if need to parse more then do not      */
  /* return, rather set bCallStateMachine to be TRUE, and run the state        */
  /* machine again, return when end of file or job done			       */	

  numLines ++;
  bCallStateMachine = SD_TRUE;
  while (bCallStateMachine)
    {
    bBuildingFunction = SD_TRUE;
    bCallStateMachine = SD_FALSE;
    switch (state)
      {
      case S_SEARCH_START_OF_FUNC_DEF:
        tmpPtr = pBuf;
	strcpy (fun, tmpPtr);
	bNewline = SD_FALSE;
	funSize = strlen (fun);
        pBuf = pointToNextNonSpaceChar(pBuf);
        pBuf += 7;
        if (isspace(*pBuf) != 0) /* should have a white space character */
	  {
	  pBuf = strpbrk (pBuf, alphabet);
	  if (pBuf != NULL) /* Function name is on this line */
	    bCallStateMachine = SD_TRUE;
	  state = S_SEARCH_START_OF_FUNC_NAME; 
	  }
	else
	  {
	  state = S_SEARCH_START_OF_FUNC_NAME; 
	  return (NULL);
	  }
      break;
      case S_SEARCH_START_OF_FUNC_NAME :
        tmpPtr = pBuf;
	if (bNewline == SD_TRUE) /* new line read in */
	  {
	  while (funSize + (ST_INT) strlen (tmpPtr) > bufSize)
	    {
	    fun = (ST_CHAR *) chk_realloc (fun, bufSize + (MAX_FUN_SIZE * sizeof (ST_CHAR)));
	    bufSize = bufSize + MAX_FUN_SIZE;
	    }
	  strcat (fun, tmpPtr);
	  funSize = strlen (fun);
	  bNewline = SD_FALSE;
	  }
        pBuf = pointToNextNonSpaceChar(pBuf); /* start of func name	*/
	pBeginFunName = pBuf;
	pBuf = strpbrk (pBuf, " \t");
	pEndFunName = pBuf;
	funNameLen = pEndFunName - pBeginFunName;
	memset (funName, '\0', 128);
	strncpy (funName, pBeginFunName, funNameLen);
	count = pBuf - tmpPtr;	
        pBuf = strpbrk (pBuf, "(");
	if (pBuf == NULL) /* Args on next line */
	  {
	  state = S_SEARCH_START_OF_FUNC_ARG;
	  return (NULL);
	  }
	state = S_SEARCH_START_OF_FUNC_ARG;
        bCallStateMachine = SD_TRUE;
	numLines = 1;	
      break;

      case S_SEARCH_START_OF_FUNC_ARG :
	tmpPtr = pBuf;
	if (bNewline == SD_TRUE) /* new line read in */
	  {
	  while (funSize + (ST_INT) strlen (tmpPtr) > bufSize)
	    {
	    fun = (ST_CHAR *) chk_realloc (fun, bufSize + (MAX_FUN_SIZE * sizeof (ST_CHAR)));
	    bufSize = bufSize + MAX_FUN_SIZE;
	    }
	  strcat (fun, tmpPtr);
	  funSize = strlen (fun);
	  bNewline = SD_FALSE;
	  }
	pBuf = pointToNextNonSpaceChar(pBuf);
	if (pBuf)
	  {		
	  if  (*pBuf == '(') /* start of function arguments */
	    {
	    pBuf ++;
	    bCount ++;
	    state = S_SEARCH_END_OF_FUNC_ARG;
	    bCallStateMachine = SD_TRUE;
	    }
          else if (*pBuf == '/' && *(pBuf+1) == '*') 
	    {				/* comment is followed by func name */
	    pBuf = strstr(pBuf,"*/");	/* move ptr to end of comment	    */	
	    pBuf += 2;			
	    bCallStateMachine = SD_TRUE;
	    }
	  else   /* func arguments were expected starting with l curly brace */
	    {
	    if (numLines > 1) /* parse again only if function name was not found in this line */
	      bCallStateMachine = SD_TRUE;
	    }	 	
          }
      break;

      case S_SEARCH_END_OF_FUNC_ARG :
	tmpPtr = pBuf;
	if (bNewline == SD_TRUE) /* new line read in */
	  {
	  while (funSize + (ST_INT) strlen (tmpPtr) > bufSize)
	    {
	    fun = (ST_CHAR *) chk_realloc (fun, bufSize + (MAX_FUN_SIZE * sizeof (ST_CHAR)));
	    bufSize = bufSize + MAX_FUN_SIZE;
	    }
	  strcat (fun, tmpPtr);
	  funSize = strlen (fun);
	  bNewline = SD_FALSE;
	  }
	pBuf = strpbrk(pBuf, "()");
	if (pBuf) /* make sure function body follows after the brace */
	  {
	  if (*pBuf == ')')
	    bCount --;
	  else 
	    bCount ++;
	  pBuf++; 
	  if (bCount <= 0)
	    state = S_SEARCH_START_OF_FUNC_BODY;
	  }
        if (pBuf) 
          count = pBuf - tmpPtr;
	else
	  count = strlen (tmpPtr);	
	if (strstr (pBuf, "{") != NULL)
	  bCallStateMachine = SD_TRUE;
      break;

      case S_SEARCH_START_OF_FUNC_BODY :
	tmpPtr = pBuf;
	if (bNewline == SD_TRUE) /* new line read in */
	  {
	  while (funSize + (ST_INT) strlen (tmpPtr) > bufSize)
	    {
	    fun = (ST_CHAR *) chk_realloc (fun, bufSize + (MAX_FUN_SIZE * sizeof (ST_CHAR)));
	    bufSize = bufSize + MAX_FUN_SIZE;
	    }
	  strcat (fun, tmpPtr);
	  funSize = strlen (fun);
	  bNewline = SD_FALSE;
	  }
	pBuf = pointToNextNonSpaceChar(pBuf);
	if (pBuf)
	  {
	  if  (*pBuf == '{') /* start of function defn */
	    {
	    pBuf ++;
	    bCount ++;
	    state = S_SEARCH_END_OF_FUNC_BODY;		
            if (pBuf) 
              count = pBuf - tmpPtr;
	    else
	      count = strlen (tmpPtr);	
	    }	
	  else if (*pBuf == '/' && *(pBuf+1) == '*') 
	    {			/* comment is followed by end of args */
	    pBuf = strstr(pBuf,"*/");	
	    pBuf += 2;		/* move ptr at the end of comment */
            if (pBuf) 
              count = pBuf - tmpPtr;
	    else
	      count = strlen (tmpPtr);	
	    }
	  }
      break;

      case S_SEARCH_END_OF_FUNC_BODY : 
	tmpPtr = pBuf;
	if (bNewline == SD_TRUE) /* new line read in */
	  {
	  while (funSize + (ST_INT) strlen (tmpPtr) > bufSize)
	    {
	    fun = (ST_CHAR *) chk_realloc (fun, bufSize + (MAX_FUN_SIZE * sizeof (ST_CHAR)));
	    bufSize = bufSize + MAX_FUN_SIZE;
	    }
	  strcat (fun, tmpPtr);
	  funSize = strlen (fun);
	  bNewline = SD_FALSE;
	  }
	pBuf =(ST_CHAR *) strpbrk (pBuf, "");
        if (pBuf) 
          count = pBuf - tmpPtr;
        else
          count = strlen (tmpPtr);	
	if (isEndOfFunc(tmpPtr,&bCount))                                                      
	  {
	  bDone = SD_TRUE;
	  bBuildingFunction = SD_FALSE;
	  }
	break;

      } /* end of switch (state) */
    } /* bCallStateMachine */


  if (bBuildingFunction == SD_TRUE)
    return (NULL);
  else if (bDone == SD_TRUE && bCount == 0 && ret == SD_SUCCESS)
    {
    pFunCtrl = (FUN_CTRL *) chk_calloc (1, sizeof(FUN_CTRL));
    pFunCtrl->funName = chk_strdup (funName);
    pFunCtrl->functiondata = chk_strdup (fun);
    pFunCtrl->funType = FUNTYPE_IN_TFN;
    }
  else
    return (NULL);

  return (pFunCtrl);
  }

/************************************************************************/
/************************************************************************/
/* 				parseExtern				*/
/* This function searches a function name in a file and copies the 	*/
/* extern function info into a control block.				*/
/************************************************************************/
FUN_CTRL *parseExtern (ST_CHAR *ptrBuf)
  {
ST_BOOLEAN bCallStateMachine; /* to parse more without getting new line */
ST_BOOLEAN bDone;	      /* to flag that we have found a function  */	
ST_INT bCount;		      /* to count parenthesis and pillow mark	*/
static ST_INT state;	      /* to derive the state machine		*/	
ST_CHAR *pBuf;		      /* a pointer to the above buffer		*/	
static ST_CHAR funPrototype[512];    /* to hold the function prototype	*/
ST_CHAR *pFunPr;	      /* pointer to the above buffer		*/
ST_INT funPrSize;	      /* sizeof (funPrototype)			*/	
ST_RET ret;		      /* to hold a return value			*/	
FUN_CTRL *pFunCtrl;	      /* retun value pointer of this function	*/	
static ST_INT numLines;	      /* for one of the output of this function */	
ST_INT count;		      /* used in bldFunprototype		*/	
ST_CHAR *tmpPtr;	      /* to hold a character pointer temporarily*/
ST_CHAR *pBeginFunName;
static ST_CHAR funName[128];
		
  /* initialize local variable */
  bDone = SD_FALSE;
  ret = SD_SUCCESS;
  bCount = 0;
  funPrSize = sizeof(funPrototype);
  if (bBuildingRValue == SD_FALSE)
    {
    state = S_SEARCH_START_OF_FUNC_NAME;
    memset (funPrototype, 0x00, funPrSize);
    memset (funName, 0x00, sizeof(funName));
    numLines = 0;
    }
  pFunPr = funPrototype;
  pFunCtrl = NULL;
  pBuf = ptrBuf;

  numLines ++;
  bCallStateMachine = SD_TRUE;
  while (bCallStateMachine)
    {
    bCallStateMachine = SD_FALSE; /* break this loop until some sets it TRUE */
    switch (state)
      {
      case S_SEARCH_START_OF_FUNC_NAME :
        tmpPtr = pBuf;
        pBuf = pointToNextNonSpaceChar(pBuf);
        if (strncmp(pBuf,"extern",6) != 0)
          continue;
        pBuf += 6;
        if (isspace(*pBuf) == 0) /* should have a white space character */
          continue;
        pBuf = pointToNextNonSpaceChar(pBuf);
        if (strncmp(pBuf,"ST_VOID",6) != 0)
          continue;
        pBuf += 7;
        if (isspace(*pBuf) == 0) /* should have a white space character */
          continue;
        pBuf = pointToNextNonSpaceChar(pBuf); /* move ptr at the start of func name */
	pBeginFunName = pBuf;
	pBuf = strpbrk (pBuf, " \t("); 	/* move ptr at the end of func name */
	strncpy (funName, pBeginFunName, pBuf - pBeginFunName);
	count = pBuf - tmpPtr;	
	ret |= buildMyString (funPrototype, &pFunPr, funPrSize, tmpPtr, count);
	state = S_SEARCH_START_OF_FUNC_ARG;
        bCallStateMachine = SD_TRUE;
	bBuildingRValue = SD_TRUE;
	numLines = 1;	
      break;

      case S_SEARCH_START_OF_FUNC_ARG :
        tmpPtr = pBuf;
        pBuf = pointToNextNonSpaceChar(pBuf);
	if (pBuf)
	  {		
	  if  (*pBuf == '(') /* start of function arguments */
	    {
	    pBuf ++;
	    bCount ++;
	    state = S_SEARCH_END_OF_FUNC_ARG;
	    bCallStateMachine = SD_TRUE;
	    }
          else if (*pBuf == '/' && *(pBuf+1) == '*') 
	    {				/* comment is followed by func name */
	    pBuf = strstr(pBuf,"*/");	/* move ptr to end of comment	    */	
	    pBuf += 2;			
	    bCallStateMachine = SD_TRUE;
	    }
	  else   /* func arguments were expected starting with l curly brace */
	    {
	    initFindFunVars(&state, &numLines, &bCount, funPrototype, &pFunPr,funPrSize, &ret);
	    }	 	
          }
	if (funPrototype[0]) /* if initFindVars was not called	*/
	  {
	  if (pBuf) 
            count = pBuf - tmpPtr;
	  else
	    count = strlen (tmpPtr);	
	  ret |= buildMyString (funPrototype, &pFunPr, funPrSize, tmpPtr, count);
         }
      break;

      case S_SEARCH_END_OF_FUNC_ARG :
	tmpPtr = pBuf;
        pBuf = strpbrk(pBuf, "()");
	if (pBuf) /* make sure function body follows after the brace */
	  {
	  if (*pBuf == ')')
	    bCount --;
	  else 
	    bCount ++;
	  pBuf++; 
	  if (bCount <= 0)
 	    state = S_SEARCH_END_OF_FUNC_PTYPE;
	  bCallStateMachine = SD_TRUE;
	  }
        if (pBuf) 
          count = pBuf - tmpPtr;
	else
	  count = strlen (tmpPtr);	
	ret |= buildMyString (funPrototype, &pFunPr, funPrSize, tmpPtr, count);
      break;

      case S_SEARCH_END_OF_FUNC_PTYPE :
        tmpPtr = pBuf;
        pBuf = pointToNextNonSpaceChar(pBuf);
        if (pBuf)
          {
          if  (*pBuf == ';') /* end of function proto type */
            {
            ret |= buildMyString (funPrototype, &pFunPr, funPrSize, ";", 1);
	    bBuildingRValue = SD_FALSE;
	    bDone = SD_TRUE;
	    }	
	  else if (*pBuf == '/' && *(pBuf+1) == '*') 
	    {			/* comment is followed by end of args */
	    pBuf = strstr(pBuf,"*/");	
	    pBuf += 2;		/* move ptr at the end of comment */
	    bCallStateMachine = SD_TRUE;
	    }
	  else /* function defn was expected starting with l brace */
	    {
	    initFindFunVars(&state, &numLines, &bCount, funPrototype, &pFunPr,funPrSize, &ret);
	    }	 	
	  }
	break;
	} /* end of switch (state) */
    } /* bCallStateMachine */

  if (bBuildingRValue == SD_TRUE)
    return (NULL);
  else if (bDone == SD_TRUE && bCount <= 0 && ret == SD_SUCCESS)
    {
    pFunCtrl = (FUN_CTRL *) chk_calloc (1, sizeof(FUN_CTRL));
    pFunCtrl->funName = chk_strdup (funName);
    pFunCtrl->functiondata = chk_strdup (funPrototype);
    pFunCtrl->funType = FUNTYPE_EXTERN;
    return (pFunCtrl);
    }
 
  return (NULL);
  }



/************************************************************************/
/* 			pointToNextNonSpaceChar				*/
/* It skips the spaces and points to next character, if end of string	*/
/* returns NULL								*/
/************************************************************************/

static ST_CHAR *pointToNextNonSpaceChar(ST_CHAR *ptr)
  {
  while (*ptr)
    {
    if (isspace(*ptr))
      ptr ++;
    else 
      break;
    }
  if (*ptr)        /* character was found */
    return (ptr);  /* location of that character */
  else 		  /* end of string */
    return (NULL);
  }

/************************************************************************/
/*			buildMyString					*/
/* This function copies from srcStr into ppDstStr bytes equal to 'count'*/
/* Before copying it also checks for the overflow of dstStr		*/
/************************************************************************/

static ST_RET buildMyString (ST_CHAR *dstStr, 
				ST_CHAR **ppDstStr, 
				ST_INT dstStrSize, 
				const ST_CHAR *srcStr, 
				ST_INT count)
  {
ST_INT dstStrLen = strlen (dstStr);

  /* make sure the strcpy will not overflow the dstStr */
  if (count + dstStrLen + 1 >= dstStrSize)
    return (SD_FAILURE);
  if (dstStrLen == 0)
    strncpy (*ppDstStr, srcStr, count);
  else
    strncat (*ppDstStr, srcStr, count);
  *ppDstStr += count;
  return (SD_SUCCESS);
  }

/************************************************************************/
/*				isEndOfFunc				*/
/* It looks for end of function by counting the pillow marks		*/
/************************************************************************/

static ST_BOOLEAN isEndOfFunc(ST_CHAR *buf, ST_INT *bCount)
  {
ST_CHAR *ptr;

  ptr = buf;
  while (*ptr)
    {
    switch (*ptr)
      {
      case '{' :
        (*bCount) ++;
      break;
      case '}' :
        (*bCount) --;
      break;
      default :
      break;
      }
    ptr ++;
    }
  if (*bCount <= 0)
    return (SD_TRUE);
  return (SD_FALSE);
  }

/************************************************************************/
/*			initFindFunVars					*/
/* Reinitialization routine						*/
/************************************************************************/

static ST_VOID initFindFunVars(ST_INT *state, 
			       ST_INT *numLines, 
			       ST_INT *bCount, 
			       ST_CHAR *funPrototype,
			       ST_CHAR **pFunPr,
			       ST_INT funPrSize,
			       ST_RET *ret)
  {
  *state = S_SEARCH_START_OF_FUNC_NAME;
  *numLines = 0;
  *bCount = 0;
  *pFunPr = funPrototype;
  memset (funPrototype, 0x00, funPrSize);
  *ret = SD_SUCCESS;
  }
