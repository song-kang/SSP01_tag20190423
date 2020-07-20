/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,       */
/*               1997-2001, All Rights Reserved                         */
/*                                                                      */
/* MODULE NAME : fo_uca.c                                               */
/* PRODUCT(S)  :                                                        */
/*                                                                      */
/* MODULE DESCRIPTION :                                                 */
/*  Program to convert type list to C code which sets up runtime table  */
/*                                                                      */
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :                            */
/*              main                                                    */
/*                                                                      */
/* MODIFICATION LOG :                                                   */
/*  Date     Who   Rev                  Comments                        */
/* --------  ---  ------   -------------------------------------------  */
/* 01/30/06  GLB     35    Integrated porting changes for VMS           */
/* 07/12/04  JRB     34    Don't log if "$dynamic" leaf entry not used.	*/
/* 01/21/04  JRB     33    Add logUnusedLeafAccessInfo funct.		*/
/*			   Add more error logs & printfs.		*/
/* 01/06/04  JRB     32    Print warning if XML input file not found.	*/
/* 12/11/03  JRB     31    If XML leaf input used & rd/wr leaf funct	*/
/*			   is NULL funct, add to lefttodo.txt.		*/
/* 11/24/03  JRB     30    Del "type twinning" code. Duplicate types	*/
/*			   should be avoided in "input" ODF files.	*/
/* 10/08/03  JRB     29    Do NOT automatically create dummy functions:	*/
/*			   _mvlu_null_read(write)_ind.			*/
/* 09/22/03  JRB     28    Add addDynLeafFuns to allow Name="$dynamic"	*/
/*			   in XML input file for supplying "extra" leaf.*/
/*			   Del "#if !defined MVLU_USR_FUN_TBLS" around	*/
/*			   leaf tables in output ".c" file.		*/
/*			   Del "leaf_tbl.c" output file. Not needed.	*/
/* 09/17/03  JRB     27	   Allow array of structures.			*/
/*			   Move makeNameDerivs call inside "if".	*/
/* 05/05/03  JRB     26    Add asserts to make sure ucaName!=NULL.	*/
/* 04/29/03  JRB     25    Chg fprintf format from %-32.32s to %-64s	*/
/*			   (uses minimum 64 spaces, but never trucates).*/
/*			   Exit if mvlu_proc_rt_type fails.		*/
/* 04/14/03  JRB     24    Eliminate compiler warnings.			*/
/* 04/01/03  JRB     23    Add UTF8string support (see RT_UTF8_STRING).	*/
/* 03/13/03  JRB     22    Use RUNTIME_CTRL *rt_ctrl in TYPE_CTRL.	*/
/*			   mvlu_proc_rt_type: Chg to use RUNTIME_CTRL.	*/
/*			   Chg ucaName in MVLU_UCA_NAME_CTRL from ptr	*/
/*			   to array (saves allocs).			*/
/* 01/06/03  MDE     21    Put leaf_tbl.c in output directory		*/
/* 12/18/02  JRB     20    Del findRtOffset, no longer used.		*/
/* 12/17/02  JRB     19    Fix embedded comment in output files.	*/
/* 12/17/02  MDE     18    Chk for MVLU_STRING_REF_PREFIX.		*/
/* 11/27/02  MDE     17    Support for CSV, XML, etc..			*/
/* 08/07/02  JRB     16    Fix dummy UTC time leaf function.		*/
/*			   Make sure dummy int values are within range.	*/
/* 11/19/01  EJV     15    Added support for new MMS type UtcTime:	*/
/*			   isRtPrim: added  case for RT_UTC_TIME;	*/
/*			   writeUcaDataInit: added code for RT_UTC_TIME;*/
/*			   saveRtUcaNames: added case for TR_UTC_TIME;	*/
/* 08/22/00  JRB     14    Add dummy code for B(V)string, Btime4(6).	*/
/*			   Add pragma & printf if dummy funcs generated.*/
/*			   Improve generated comments.			*/
/* 07/13/00  JRB     13    Fix Visible string dummy rd_ind_fun.		*/
/* 04/26/00  RKR     12    Added caseSensStrIdxs			*/
/* 04/10/00  JRB     11    Fix SD_CONST for ??IndFunTbl.		*/
/* 03/07/00  MDE     10    Added default read/write function feature	*/
/* 01/21/00  MDE     09    Added 'useShortenedNames'			*/ 
/* 09/10/99  MDE     08    Now set 'curr_index' for all read types	*/
/* 05/26/99  MDE     07    Cleaned up and upgraded template file proc.	*/
/* 05/26/99  MDE     06    Now use chk_strdup				*/
/* 03/03/99  MDE     05    Fixed bug in name ordering for arrays	*/
/* 09/21/98  MDE     04    Minor lint cleanup				*/
/* 09/10/98  MDE     03    Minor fixes for handling UCA/non-UCA types	*/
/* 07/28/98  MDE     02    Corrections to extern rd/wr ind fun handling	*/
/* 07/21/98  MDE     01    New module, extracted from foundry.c		*/
/************************************************************************/

#include "foundry.h"
#include "str_util.h"	/* for stricmp	*/
/************************************************************************/
/************************************************************************/

SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;

ST_BOOLEAN useShortenedNames = SD_FALSE;

/************************************************************************/
/* STATIC VARIABLES							*/

static IND_FUN_CTRL *rdIndFunCtrlHead;
static IND_FUN_CTRL *wrIndFunCtrlHead;
static REF_CTRL *refCtrlHead;

/************************************************************************/
/* STATIC FUNCTIONS							*/

static ST_VOID makeNameDerivs (ST_CHAR *ucaVarName, ST_CHAR *nameDestU,
                               ST_CHAR *nameDestL, ST_CHAR *nameDestN);
static ST_VOID writeUcaDataInit (IND_FUN_CTRL *indFunCtrl, FILE *fp);
static ST_VOID writeCompName (RUNTIME_TYPE *rt, 
		       MVLU_FOUNDRY_TYPE_INFO *mvluFoundryInfo,
		       ST_CHAR *baseName);
static ST_VOID writeRdIndFun (IND_FUN_CTRL *indFunCtrl);
static ST_VOID writeWrIndFun (IND_FUN_CTRL *indFun);
static IND_FUN_CTRL *findRdIndFun (ST_CHAR *funName);
static IND_FUN_CTRL *findWrIndFun (ST_CHAR *funName);
static ST_VOID writeRdIndFunNewCode (IND_FUN_CTRL *indFunCtrl, FILE *fp);
static ST_VOID writeWrIndFunNewCode (IND_FUN_CTRL *indFun, FILE *fp);
static ST_VOID writeLeafTbls (FILE *fp);

static IND_FUN_CTRL *createIndFunCtrl (ST_CHAR *funName);
/************************************************************************/
/************************************************************************/

#define MAX_LEAF_NAME	200
#define MAX_FUN_NAME	200
#define MAX_REF_NAME	200


typedef struct
  {
  DBL_LNK l;
  ST_CHAR leafName[MAX_LEAF_NAME+1];
  ST_CHAR rdIndFunName[MAX_FUN_NAME+1];
  ST_CHAR wrIndFunName[MAX_FUN_NAME+1];
  ST_CHAR refString[MAX_REF_NAME+1];
  ST_BOOLEAN leafUsed;	/* indicate if leaf actually used in object model*/
  } LEAF_ACCESS_INFO;
LEAF_ACCESS_INFO *leafAccessHead;
LEAF_ACCESS_INFO *findLeafAccessInfo (ST_CHAR *leafName);

static ST_VOID _getAttribVal (ST_CHAR *xmlBuf, ST_CHAR *attribName, 
		       ST_CHAR *dest, ST_INT destSize);


/************************************************************************/
/************************************************************************/

ST_VOID deriveUcaRtInfo (TYPE_CTRL *typeCtrl, 
                        RUNTIME_TYPE *rt, 
			MVLU_FOUNDRY_TYPE_INFO *mvluFoundryInfo,
                        ST_INT rt_index)
  {
ST_CHAR nameBufU[100];
ST_CHAR nameBufL[100];
ST_CHAR nameBufN[100];
ST_INT i;
ST_INT nameLen;
DEFINE_CTRL *dc;
FUN_CTRL *rdFunCtrl;
FUN_CTRL *wrFunCtrl;
ST_CHAR refBuf[300];
ST_CHAR rdIndexBuf[100];
ST_CHAR wrIndexBuf[100];
ST_CHAR funNameBuf[300];
ST_CHAR *funName;
IND_FUN_CTRL *indFunCtrl;
REF_CTRL *newRef;
ST_BOOLEAN dummyRead = SD_FALSE;
ST_BOOLEAN dummyWrite = SD_FALSE;
ST_BOOLEAN useRef;
LEAF_ACCESS_INFO *la;

/* Here we come up with the Read and Write indication handler funs      */
/* for this primitive element, as well as the REF define.		*/
  if (ms_is_rt_prim (rt) == SD_TRUE)
    {
    assert (mvluFoundryInfo->ucaName);	/* this code assumes it is not NULL*/

  /* Make upper and lower case versions of the var name */ 
    makeNameDerivs (mvluFoundryInfo->ucaName, nameBufU, nameBufL, nameBufN);

    la = findLeafAccessInfo (mvluFoundryInfo->ucaName);

    /* set flag to indicate that this leaf actually used by the object model*/
    if (la)
      la->leafUsed = SD_TRUE;

  /* Take care of the REF define generation */
    if (mvlu_use_ref == SD_TRUE)
      {
      if (mvlu_use_leaf_file)
        {
        if (la && la->refString[0])
          {
          newRef = (REF_CTRL *) chk_calloc (1, sizeof (REF_CTRL));
          newRef->refDef = la->refString;
          list_add_last ((ST_VOID **)&refCtrlHead, newRef);
          useRef = SD_TRUE;
          }
	else
          useRef = SD_FALSE;
        }
      else
        {
        sprintf (refBuf, "%s_REF", nameBufU);
        if (useTemplate == SD_TRUE)
          dc = findRefDefine (refBuf);
        else
          dc = NULL;

        useRef = SD_TRUE;
        if (dc == NULL)
          {
          if (mvlu_no_dummy == SD_FALSE)
            sprintf (refBuf, "#define %s_REF \t\t((ST_RTREF) 0)", nameBufU);
  	  else
            useRef = SD_FALSE;
          }
        else
          sprintf (refBuf, "#define %s_REF \t\t%s", nameBufU, dc->rValue);

        if (useRef)
          {
          newRef = (REF_CTRL *) chk_calloc (1, sizeof (REF_CTRL));
          if (dc == NULL)
            newRef->dummyRef = SD_TRUE;
        
          newRef->refDef = chk_strdup (refBuf);
          list_add_last ((ST_VOID **)&refCtrlHead, newRef);
          }
        }
      }

  /* Take care of the read indication function. Determine the function	*/
  /* name, and see if it is already included.				*/
    if (mvlu_use_leaf_file)
      {
      if (la && la->rdIndFunName[0])
        funName = la->rdIndFunName;
      else
        funName = _MVLU_NULL_READ_FUN_NAME;

      rdFunCtrl = (FUN_CTRL *) chk_calloc (1, sizeof(FUN_CTRL));
      rdFunCtrl->funType = FUNTYPE_EXTERN;
      }
    else
      {
      if (useShortenedNames == SD_TRUE)
        sprintf (funNameBuf, "u%sRif", nameBufN);
      else
        sprintf (funNameBuf, "u_%s_rd_ind_fun", nameBufL);
      rdFunCtrl = findTfnFun (funNameBuf);
      switch (rdFunCtrl->funType)
        {
        case FUNTYPE_NOT_FOUND :
          funName = funNameBuf;
          if (typeCtrl->rdWrIndFunSel != NULL &&
              typeCtrl->rdWrIndFunSel->x[rt->el_tag].rd[0] != 0)
            {
            funName = typeCtrl->rdWrIndFunSel->x[rt->el_tag].rd;
            rdFunCtrl = findTfnFun (funName);
            }
        break;
  
        case FUNTYPE_IN_TFN :
        case FUNTYPE_EXTERN :
          funName = funNameBuf;
        break;
  
        case FUNTYPE_REPLACED :
          funName = rdFunCtrl->replacementFunName;
          rdFunCtrl = findTfnFun (funName);
        break;
  
        default:
        break;
        }
      }

    /* If XML leaf input used, chk if name matches dummy (null) function name*/
    if (mvlu_use_leaf_file && strcmp (funName, _MVLU_NULL_READ_FUN_NAME) == 0)
      dummyRead = SD_TRUE;

  /* See if the selected function is already on our list		*/
    indFunCtrl = findRdIndFun (funName);

  /* If not, see if we are to generate dummy functions */
    if (!indFunCtrl && !mvlu_use_leaf_file && 
         rdFunCtrl->funType == FUNTYPE_NOT_FOUND && mvlu_no_dummy)
      {
      funName = _MVLU_NULL_READ_FUN_NAME;
      rdFunCtrl->funType = FUNTYPE_EXTERN;
      indFunCtrl = findRdIndFun (funName);
      dummyRead = SD_TRUE;
      }

    if (indFunCtrl)	/* Yep, it is on the list, so we can share  */
      strcpy (rdIndexBuf, indFunCtrl->funIndexDefine);
    else
      {
      indFunCtrl = (IND_FUN_CTRL *) chk_calloc (1, sizeof (IND_FUN_CTRL));
      list_add_last ((ST_VOID **)&rdIndFunCtrlHead, indFunCtrl);

      indFunCtrl->funName = chk_strdup (funName);
      indFunCtrl->arr_comp = mvluFoundryInfo->arr_comp;
      indFunCtrl->funCtrl = rdFunCtrl;
      indFunCtrl->rt = rt;
      indFunCtrl->rt_index = rt_index;
      indFunCtrl->ucaName = chk_strdup (mvluFoundryInfo->ucaName);

    /* Now we need the  read index define to be used */
      strcpy (rdIndexBuf, funName);
      strcat (rdIndexBuf, "_INDEX");
      nameLen = (ST_INT) strlen (rdIndexBuf);
      if (caseSensStrIdxs == SD_FALSE)
        {
        for (i = 0; i < nameLen; ++i)
          {
          if (islower (rdIndexBuf[i]))
            rdIndexBuf[i] = toupper (rdIndexBuf[i]);
          }
        }
      indFunCtrl->funIndexDefine = chk_strdup (rdIndexBuf);
      }


  /* Take care of the write indication function. Determine the function	*/
  /* name, and see if it is already included.				*/
    if (mvlu_use_leaf_file)
      {
      if (la && la->wrIndFunName[0])
        funName = la->wrIndFunName;
      else
        funName = _MVLU_NULL_WRITE_FUN_NAME;

      wrFunCtrl = (FUN_CTRL *) chk_calloc (1, sizeof(FUN_CTRL));
      wrFunCtrl->funType = FUNTYPE_EXTERN;
      }
    else
      {
      if (useShortenedNames == SD_TRUE)
        sprintf (funNameBuf, "u%sWif", nameBufN);
      else
        sprintf (funNameBuf, "u_%s_wr_ind_fun", nameBufL);
      wrFunCtrl = findTfnFun (funNameBuf);
      switch (wrFunCtrl->funType)
        {
        case FUNTYPE_NOT_FOUND :
          funName = funNameBuf;
          if (typeCtrl->rdWrIndFunSel != NULL &&
              typeCtrl->rdWrIndFunSel->x[rt->el_tag].wr[0] != 0)
            {
            funName = typeCtrl->rdWrIndFunSel->x[rt->el_tag].wr;
            wrFunCtrl = findTfnFun (funName);
            }
        break;
  
        case FUNTYPE_IN_TFN :
        case FUNTYPE_EXTERN :
          funName = funNameBuf;
        break;
  
        case FUNTYPE_REPLACED :
          funName = wrFunCtrl->replacementFunName;
          wrFunCtrl = findTfnFun (funName);
        break;
  
        default:
        break;
        }
      }

    /* If XML leaf input used, chk if name matches dummy (null) function name*/
    if (mvlu_use_leaf_file && strcmp (funName, _MVLU_NULL_WRITE_FUN_NAME) == 0)
      dummyWrite = SD_TRUE;

  /* See if the selected function is already on our list		*/
    indFunCtrl = findWrIndFun (funName);

  /* If not, see if we are to generate dummy functions */
    if (!indFunCtrl  && !mvlu_use_leaf_file && wrFunCtrl->funType == FUNTYPE_NOT_FOUND && mvlu_no_dummy)
      {
      funName = _MVLU_NULL_WRITE_FUN_NAME;
      indFunCtrl = findWrIndFun (funName);
      wrFunCtrl->funType = FUNTYPE_EXTERN;
      dummyWrite = SD_TRUE;
      }

    if (indFunCtrl)	/* Yep, it is on the list, so we can share  */
      strcpy (wrIndexBuf, indFunCtrl->funIndexDefine);
    else
      {
      indFunCtrl = (IND_FUN_CTRL *) chk_calloc (1, sizeof (IND_FUN_CTRL));
      list_add_last ((ST_VOID **)&wrIndFunCtrlHead, indFunCtrl);

      indFunCtrl->funName = chk_strdup (funName);
      indFunCtrl->arr_comp = mvluFoundryInfo->arr_comp;
      indFunCtrl->funCtrl = wrFunCtrl;
      indFunCtrl->rt = rt;
      indFunCtrl->rt_index = rt_index;
      indFunCtrl->ucaName = chk_strdup (mvluFoundryInfo->ucaName);

    /* Now we need the  write index define to be used */
      strcpy (wrIndexBuf, funName);
      strcat (wrIndexBuf, "_INDEX");
      nameLen = (ST_INT) strlen (wrIndexBuf);
      if (caseSensStrIdxs == SD_FALSE)
        {
        for (i = 0; i < nameLen; ++i)
          {
          if (islower (wrIndexBuf[i]))
            wrIndexBuf[i] = toupper (wrIndexBuf[i]);
          }
        }
      indFunCtrl->funIndexDefine = chk_strdup (wrIndexBuf);
      }

  /* Save the generated string defines for later use ... */

    if (useRef == SD_TRUE)
      {
      if (mvlu_use_leaf_file)
        rt->refBuf = la->refString;
      else
        {
        sprintf (refBuf, "%s_REF", nameBufU);
        rt->refBuf = chk_strdup (refBuf);
        }
      }
    else
      rt->refBuf = NULL;

    rt->rdIndexBuf = chk_strdup (rdIndexBuf);
    rt->wrIndexBuf = chk_strdup (wrIndexBuf);

    if (dummyRead || dummyWrite || !useRef)
      {
      fprintf (newFp, "\n%-64s(%s%s%s)", mvluFoundryInfo->ucaName,
    			dummyRead ? "Rd": "  ",
    			dummyWrite ? "Wr": "  ",
    			!useRef ? "Ref": "");
      }
    }
  }

/************************************************************************/
/* WRITING RT INITIALIZATION CODE					*/
/************************************************************************/
/*                      writeUcaRtInfo                                  */
/************************************************************************/

ST_VOID writeUcaRtInfo (TYPE_CTRL *typeCtrl, 
                        RUNTIME_TYPE *rt, 
                        ST_INT rt_index)
  {
FILE *fp;
ST_INT rtOffset;
ST_CHAR *p;

  fp = codeFp;
  fprintf (fp, "\n    rt_types[%d].mvluTypeInfo.sortedNum = %d;",
                                rt_index, rt->mvluTypeInfo.sortedNum);

  rtOffset = rt->mvluTypeInfo.offSet;
  fprintf (fp, "\n    rt_types[%d].mvluTypeInfo.offSet = %d;",
                                  rt_index, rtOffset);

  if (ucaType && ms_is_rt_prim (rt) == SD_TRUE)
    {
    fprintf (fp, "\n    rt_types[%d].mvluTypeInfo.rdIndFunIndex = %s;",
                                  rt_index, rt->rdIndexBuf);
    fprintf (fp, "\n    rt_types[%d].mvluTypeInfo.wrIndFunIndex = %s;",
                                  rt_index, rt->wrIndexBuf);
    if (mvlu_use_ref == SD_TRUE && rt->refBuf != NULL)
      {
      if (!strncmp (rt->refBuf, MVLU_STRING_REF_PREFIX, strlen (MVLU_STRING_REF_PREFIX)))
        {
        p = rt->refBuf + strlen(MVLU_STRING_REF_PREFIX);
        fprintf (fp, "\n    rt_types[%d].mvluTypeInfo.ref = (ST_RTREF)\"%s\";", rt_index, p);
	}
      else /* Not a SBO reference */
        {
        fprintf (fp, "\n    rt_types[%d].mvluTypeInfo.ref = %s;",
                                  rt_index, rt->refBuf);
        }
      }
    }
  }

/************************************************************************/
/*                       findRdIndFun					*/
/************************************************************************/

static IND_FUN_CTRL *findRdIndFun (ST_CHAR *funName)
  {
IND_FUN_CTRL *ic;

  ic = rdIndFunCtrlHead;
  while (ic != NULL)
    {
    if (!strcmp (ic->funName, funName))
      return (ic);
    ic = (IND_FUN_CTRL *) list_get_next (rdIndFunCtrlHead, ic);
    }
  return (NULL);
  }

/************************************************************************/
/*                       findWrIndFun					*/
/************************************************************************/

static IND_FUN_CTRL *findWrIndFun (ST_CHAR *funName)
  {
IND_FUN_CTRL *ic;

  ic = wrIndFunCtrlHead;
  while (ic != NULL)
    {
    if (!strcmp (ic->funName, funName))
      return (ic);
    ic = (IND_FUN_CTRL *) list_get_next (wrIndFunCtrlHead, ic);
    }
  return (NULL);
  }

/************************************************************************/
/************************************************************************/
/* WRITING UCA CODE							*/
/************************************************************************/

ST_CHAR *ucaSrcCommentStrings[] =
  {
  "\n/* UCA READ INDICATION FUNCTIONS                                           */",
  "\n/* ReadInd functions are to put the primitive data in the dest buffer.    */",
  "\n/* The app is to call 'mvlu_rd_prim_done' to let MVLU know that the data   */",
  "\n/* is available for encoding.                                              */",
  "\n/* This can be done in this function or asynchronously.                    */",
  "\n",
  "\n/* UCA WRITE INDICATION FUNCTIONS                                          */",
  "\n/* WriteInd functions are to move the primitive data from the src buffer   */",
  "\n/* and deal with it, then call 'mvlu_wr_prim_done' to let MVLU know        */",
  "\n/* that we are done with it.                                               */",
  "\n/* This can be done in this function or asynchronously.                    */",
  };
ST_INT numUcaSrcCommentStrings = sizeof(ucaSrcCommentStrings)/sizeof(ST_CHAR *);

/************************************************************************/
/*                       write_uca_code					*/
/************************************************************************/

ST_VOID write_uca_code ()
  {
IND_FUN_CTRL *ic;
REF_CTRL *rc;
ST_INT indFunIndex;
FILE *fp;
ST_INT i;
ST_BOOLEAN dummyPresent;

  fp = codeFp;

/* Check to see if we will be making up template functions ... */
  if (mvlu_no_dummy == SD_FALSE)
    {
    dummyPresent = SD_FALSE;
    ic = rdIndFunCtrlHead;
    while (!dummyPresent && ic != NULL && dummyPresent == SD_FALSE)
      {
      if (ic->funCtrl->funType == FUNTYPE_NOT_FOUND && 
          strcmp (ic->funName, _MVLU_NULL_READ_FUN_NAME))
        {
        dummyPresent = SD_TRUE;
        }
      ic = (IND_FUN_CTRL *) list_get_next (rdIndFunCtrlHead, ic);
      }
    ic = wrIndFunCtrlHead;
    while (!dummyPresent && ic != NULL && dummyPresent == SD_FALSE)
      {
      if (ic->funCtrl->funType == FUNTYPE_NOT_FOUND && 
          strcmp (ic->funName, _MVLU_NULL_WRITE_FUN_NAME))
        {
        dummyPresent = SD_TRUE;
        }
      ic = (IND_FUN_CTRL *) list_get_next (wrIndFunCtrlHead, ic);
      }
    
    if (dummyPresent == SD_TRUE)
      {
      fprintf (newFp, "\n%s", commentBar);
      fprintf (newFp, "\n/* These functions were NOT supplied in the input template file         */");
      fprintf (newFp, "\n/* and probably should be ...                                           */");
      fprintf (newFp, "\n");
      printf ("\nWARNING: Some functions were not supplied in the input template file");
      printf ("\n         so dummy functions were generated. See '%s'.", newFileName);
      }
    }

  fprintf (fp, "\n%s", commentBar);
  for (i = 0; i < numUcaSrcCommentStrings; ++i)
    fprintf (fp, "%s", ucaSrcCommentStrings[i]);
  fprintf (fp, "\n%s\n", commentBar);

/* Write the Read Indication Functions */
  ic = rdIndFunCtrlHead;
  while (ic != NULL)
    {
    writeRdIndFun (ic);
    ic = (IND_FUN_CTRL *) list_get_next (rdIndFunCtrlHead, ic);
    }

/* Now write the Write Indication Functions */
  ic = wrIndFunCtrlHead;
  while (ic != NULL)
    {
    writeWrIndFun (ic);
    ic = (IND_FUN_CTRL *) list_get_next (wrIndFunCtrlHead, ic);
    }

  fprintf (fp, "\n%s", commentBar);
  writeLeafTbls (fp);

  fprintf (fp, "\n%s", commentBar);
  fprintf (fp, "\n");	/* LINUX likes newline at end of file	*/
  if (mvlu_no_dummy == SD_FALSE)
    {
    dummyPresent = SD_FALSE;
    rc = refCtrlHead;
    while (rc != NULL)
      {
      if (rc->dummyRef == SD_TRUE)
        {
        dummyPresent = SD_TRUE;
        break;
        }
      rc = (REF_CTRL *) list_get_next (refCtrlHead, rc);
      }
    if (dummyPresent == SD_TRUE)
      {
      fprintf (newFp, "\n%s", commentBar);
      fprintf (newFp, "\n/* These reference defines were NOT supplied in the input template file */");
      fprintf (newFp, "\n/* and probably should be ...                                           */");
      fprintf (newFp, "\n");
      }
    }

/* Write the REFERENCE Defines */
  if (!mvlu_use_leaf_file)
    {
    fprintf (headerFp, "\n\n%s", commentBar);
    fprintf (headerFp, "\n/* UCA Primitive Node Reference Defines                                 */");
    fprintf (headerFp, "\n/* These should be supplied to Foundry via a template file              */");
    fprintf (headerFp, "\n");
    rc = refCtrlHead;
    while (rc != NULL)
      {
      fprintf (headerFp, "\n%s", rc->refDef);
      if (mvlu_no_dummy == SD_FALSE && rc->dummyRef == SD_TRUE)
        fprintf (newFp, "\n%s", rc->refDef);

      rc = (REF_CTRL *) list_get_next (refCtrlHead, rc);
      }
    }

/* Write the Read Indication Function Index Defines */
  fprintf (headerFp, "\n\n%s", commentBar);
  fprintf (headerFp, "\n/* UCA Runtime Type Function Indexes                                    */");
  fprintf (headerFp, "\n/* These are generated by Foundry and should not be changed             */");
  fprintf (headerFp, "\n");
  indFunIndex = 0;
  ic = rdIndFunCtrlHead;
  while (ic != NULL)
    {
    fprintf (headerFp, "\n#define %s\t%d", ic->funIndexDefine, indFunIndex);
    ++indFunIndex;
    ic = (IND_FUN_CTRL *) list_get_next (rdIndFunCtrlHead, ic);
    }

/* Write the Write Indication Function Index Defines */
  fprintf (headerFp, "\n\n");
  indFunIndex = 0;
  ic = wrIndFunCtrlHead;
  while (ic != NULL)
    {
    fprintf (headerFp, "\n#define %s\t%d", ic->funIndexDefine, indFunIndex);
    ++indFunIndex;
    ic = (IND_FUN_CTRL *) list_get_next (wrIndFunCtrlHead, ic);
    }


/* Write the Read Indication Function External Declarations */
  fprintf (headerFp, "\n\n%s", commentBar);
  fprintf (headerFp, "\n/* UCA Read/Write Indication function external declarations */");
  fprintf (headerFp, "\n/* These functions must be provided externally              */");
  fprintf (headerFp, "\n");
  ic = rdIndFunCtrlHead;
  while (ic != NULL)
    {
    if (ic->funCtrl->funType == FUNTYPE_EXTERN)
      {
      if (ic->funCtrl->functiondata != NULL)
        fprintf (headerFp, "\n%s", ic->funCtrl->functiondata);
      else
        fprintf (headerFp, "\nST_VOID %s (MVLU_RD_VA_CTRL *mvluRdVaCtrl);", ic->funName);
      }
    ic = (IND_FUN_CTRL *) list_get_next (rdIndFunCtrlHead, ic);
    }
  fprintf (headerFp, "\n");
  ic = wrIndFunCtrlHead;
  while (ic != NULL)
    {
    if (ic->funCtrl->funType == FUNTYPE_EXTERN)
      {
      if (ic->funCtrl->functiondata != NULL)
        fprintf (headerFp, "\n%s", ic->funCtrl->functiondata);
      else
        fprintf (headerFp, "\nST_VOID %s (MVLU_WR_VA_CTRL *mvluWrVaCtrl);", ic->funName);
      }
    ic = (IND_FUN_CTRL *) list_get_next (wrIndFunCtrlHead, ic);
    }
  fprintf (headerFp, "\n");
  }

/************************************************************************/
/************************************************************************/
/* WRITE READ/WRITE INDICATION FUNCTIONS				*/
/************************************************************************/
/*                       writeRdIndFun					*/
/************************************************************************/

static ST_VOID writeRdIndFun (IND_FUN_CTRL *indFunCtrl)
  {
FILE *fp;

  fp = codeFp;

  /* Was there anything in the LAP file for this function? */
  if (indFunCtrl->funCtrl->funType == FUNTYPE_NOT_FOUND)
    {
    writeRdIndFunNewCode (indFunCtrl, codeFp);
    writeRdIndFunNewCode (indFunCtrl, newFp);
    }
  else if (indFunCtrl->funCtrl->funType == FUNTYPE_IN_TFN)
    {
    fprintf (fp, "\n\n%s", commentBar);
    fprintf (fp, "\n/*\t\t\t%s\t\t\t\t\t*/", indFunCtrl->funName);
    fprintf (fp, "\n%s\n\n", commentBar);
    fprintf (fp, "\n%s", indFunCtrl->funCtrl->functiondata);
    }
  }

/************************************************************************/
/*                       writeWrIndFun					*/
/************************************************************************/

static ST_VOID writeWrIndFun (IND_FUN_CTRL *indFunCtrl)
  {
FILE *fp;

  fp = codeFp;

  /* Was there anything in the template file for this function? */
  if (indFunCtrl->funCtrl->funType == FUNTYPE_NOT_FOUND)
    {
    writeWrIndFunNewCode (indFunCtrl, codeFp);
    writeWrIndFunNewCode (indFunCtrl, newFp);
    }
  else if (indFunCtrl->funCtrl->funType == FUNTYPE_IN_TFN)
    {
    fprintf (fp, "\n\n%s", commentBar);
    fprintf (fp, "\n/*\t\t\t%s\t\t\t\t\t*/", indFunCtrl->funName);
    fprintf (fp, "\n%s\n\n", commentBar);
    fprintf (fp, "\n%s", indFunCtrl->funCtrl->functiondata);
    }
  }





/************************************************************************/
/************************************************************************/
/* WRITE DUMMY READ INDICATION HANDLER					*/
/************************************************************************/
/*                       writeRdIndFunNewCode				*/
/************************************************************************/

static ST_VOID writeRdIndFunNewCode (IND_FUN_CTRL *indFunCtrl, FILE *fp)
  {
ST_CHAR nameBufU[100];
ST_CHAR nameBufL[100];
ST_CHAR nameBufN[100];

  assert (indFunCtrl->ucaName);	/* this code assumes it is not NULL	*/

  makeNameDerivs (indFunCtrl->ucaName, nameBufU, nameBufL, nameBufN);

  fprintf (fp, "\n\n%s", commentBar);
  fprintf (fp, "\n/*\t\t\t%s\t\t\t\t\t*/", indFunCtrl->funName);
  fprintf (fp, "\n%s", commentBar);
  if (useShortenedNames == SD_TRUE)
    {
    fprintf (fp, "\n\nST_VOID u%sRif (MVLU_RD_VA_CTRL *mvluRdVaCtrl)", 
                      nameBufN);
    }
  else
    {
    fprintf (fp, "\n#pragma message(\"Dummy function '%s' generated by Foundry.\")", 
                      indFunCtrl->funName);
    fprintf (fp, "\n\nST_VOID %s (MVLU_RD_VA_CTRL *mvluRdVaCtrl)", 
                      indFunCtrl->funName);
    }
  fprintf (fp, "\n  {");

  writeUcaDataInit (indFunCtrl, fp);
  fprintf (fp, "\n\n/* When the data buffer has valid read data, send the response */");
  fprintf (fp, "\n  mvlu_rd_prim_done (mvluRdVaCtrl, SD_SUCCESS);");
  fprintf (fp, "\n  }");
  }
  
/************************************************************************/
/*  			writeUcaDataInit				*/
/************************************************************************/

static ST_VOID writeUcaDataInit (IND_FUN_CTRL *indFunCtrl, FILE *fp)
  {
RUNTIME_TYPE *rt_el_ptr;
ST_INT rt_index;
ST_CHAR *ucaName;
ST_CHAR *tempPtr;

  assert (indFunCtrl->ucaName);	/* this code assumes it is not NULL	*/

  rt_index = indFunCtrl->rt_index;
  ucaName = indFunCtrl->ucaName;
  rt_el_ptr = indFunCtrl->rt;

  if (indFunCtrl->arr_comp == SD_TRUE)
    fprintf (fp, "\nST_RTINT curr_index;");

  if (rt_el_ptr->el_tag == RT_VISIBLE_STRING)
    {
    tempPtr = ucaName;
    while (strlen (tempPtr) > (ST_UINT) abs (rt_el_ptr->u.p.el_len))
      ++tempPtr;
    fprintf (fp, "\nST_CHAR *dest;");
    fprintf (fp, "\n");
    if (indFunCtrl->arr_comp == SD_TRUE)
      {
      fprintf (fp, "\n/* This is an element of an array, get the index ... */");
      fprintf (fp, "\n  curr_index = mvluRdVaCtrl->rdVaCtrl->va->arrCtrl.curr_index;\n");
      }
    fprintf (fp, "\n/* primData points to a 'Vstring%d'. Copy dummy data.\t*/",
               -rt_el_ptr->u.p.el_len);	/* el_len always negative.	*/
    fprintf (fp, "\n  dest = mvluRdVaCtrl->primData;");
    fprintf (fp, "\n  strcpy (dest, \"%s\");", tempPtr);
    }
  else if (rt_el_ptr->el_tag == RT_INTEGER)
    {
    switch (rt_el_ptr->u.p.el_len)
      {
      case 1:
        fprintf (fp, "\nST_INT8 *dest;");
        fprintf (fp, "\n");
        if (indFunCtrl->arr_comp == SD_TRUE)
          {
          fprintf (fp, "\n/* This is an element of an array, get the index ... */");
          fprintf (fp, "\n  curr_index = mvluRdVaCtrl->rdVaCtrl->va->arrCtrl.curr_index;\n");
          }
        fprintf (fp, "\n/* primData points to a 'Int8'. Copy dummy data.\t*/");
        fprintf (fp, "\n  dest = (ST_INT8 *) mvluRdVaCtrl->primData;");
        fprintf (fp, "\n  if (*dest == 0)");
        fprintf (fp, "\n    *dest = %d;", rt_index & 0x7F);
      break;
      case 2:
        fprintf (fp, "\nST_INT16 *dest;");
        fprintf (fp, "\n");
        if (indFunCtrl->arr_comp == SD_TRUE)
          {
          fprintf (fp, "\n/* This is an element of an array, get the index ... */");
          fprintf (fp, "\n  curr_index = mvluRdVaCtrl->rdVaCtrl->va->arrCtrl.curr_index;\n");
          }
        fprintf (fp, "\n/* primData points to a 'Int16'. Copy dummy data.\t*/");
        fprintf (fp, "\n  dest = (ST_INT16 *) mvluRdVaCtrl->primData;");
        fprintf (fp, "\n  if (*dest == 0)");
        fprintf (fp, "\n    *dest = %d;", rt_index & 0x7FFF);
      break;
      case 4:
        fprintf (fp, "\nST_INT32 *dest;");
        fprintf (fp, "\n");
        if (indFunCtrl->arr_comp == SD_TRUE)
          {
          fprintf (fp, "\n/* This is an element of an array, get the index ... */");
          fprintf (fp, "\n  curr_index = mvluRdVaCtrl->rdVaCtrl->va->arrCtrl.curr_index;\n");
          }
        fprintf (fp, "\n/* primData points to a 'Int32'. Copy dummy data.\t*/");
        fprintf (fp, "\n  dest = (ST_INT32 *) mvluRdVaCtrl->primData;");
        fprintf (fp, "\n  if (*dest == 0)");
        fprintf (fp, "\n    *dest = %d;", rt_index);
      break;
      }
    }
  else if (rt_el_ptr->el_tag == RT_UNSIGNED)
    {
    switch (rt_el_ptr->u.p.el_len)
      {
      case 1:
        fprintf (fp, "\nST_UINT8 *dest;");
        fprintf (fp, "\n");
        if (indFunCtrl->arr_comp == SD_TRUE)
          {
          fprintf (fp, "\n/* This is an element of an array, get the index ... */");
          fprintf (fp, "\n  curr_index = mvluRdVaCtrl->rdVaCtrl->va->arrCtrl.curr_index;\n");
          }
        fprintf (fp, "\n/* primData points to a 'Uint8'. Copy dummy data.\t*/");
        fprintf (fp, "\n  dest = (ST_UINT8 *) mvluRdVaCtrl->primData;");
        fprintf (fp, "\n  if (*dest == 0)");
        fprintf (fp, "\n    *dest = %d;", rt_index & 0xFF);
      break;
      case 2:
        fprintf (fp, "\nST_UINT16 *dest;");
        fprintf (fp, "\n");
        if (indFunCtrl->arr_comp == SD_TRUE)
          {
          fprintf (fp, "\n/* This is an element of an array, get the index ... */");
          fprintf (fp, "\n  curr_index = mvluRdVaCtrl->rdVaCtrl->va->arrCtrl.curr_index;\n");
          }
        fprintf (fp, "\n/* primData points to a 'Uint16'. Copy dummy data.\t*/");
        fprintf (fp, "\n  dest = (ST_UINT16 *) mvluRdVaCtrl->primData;");
        fprintf (fp, "\n  if (*dest == 0)");
        fprintf (fp, "\n    *dest = %d;", rt_index & 0xFFFF);
      break;
      case 4:
        fprintf (fp, "\nST_UINT32 *dest;");
        fprintf (fp, "\n");
        if (indFunCtrl->arr_comp == SD_TRUE)
          {
          fprintf (fp, "\n/* This is an element of an array, get the index ... */");
          fprintf (fp, "\n  curr_index = mvluRdVaCtrl->rdVaCtrl->va->arrCtrl.curr_index;\n");
          }
        fprintf (fp, "\n/* primData points to a 'Uint32'. Copy dummy data.\t*/");
        fprintf (fp, "\n  dest = (ST_UINT32 *) mvluRdVaCtrl->primData;");
        fprintf (fp, "\n  if (*dest == 0)");
        fprintf (fp, "\n    *dest = %d;", rt_index);
      break;
      }
    }
  else if (rt_el_ptr->el_tag == RT_FLOATING_POINT)
    {
    if (rt_el_ptr->u.p.el_len == sizeof(ST_FLOAT))
      {
      fprintf (fp, "\nST_FLOAT *dest;");
      fprintf (fp, "\n");
      fprintf (fp, "\n/* primData points to a 'Float'. Copy dummy data.\t*/");
        if (indFunCtrl->arr_comp == SD_TRUE)
          {
          fprintf (fp, "\n/* This is an element of an array, get the index ... */");
          fprintf (fp, "\n  curr_index = mvluRdVaCtrl->rdVaCtrl->va->arrCtrl.curr_index;\n");
          }
      fprintf (fp, "\n  dest = (ST_FLOAT *) mvluRdVaCtrl->primData;");
      fprintf (fp, "\n  *dest = (ST_FLOAT) %.3f;", (ST_FLOAT) rt_index + 0.2345);
      }
    else
      {
      fprintf (fp, "\nST_DOUBLE *dest;");
      fprintf (fp, "\n");
        if (indFunCtrl->arr_comp == SD_TRUE)
          {
          fprintf (fp, "\n/* This is an element of an array, get the index ... */");
          fprintf (fp, "\n  curr_index = mvluRdVaCtrl->rdVaCtrl->va->arrCtrl.curr_index;\n");
          }
        fprintf (fp, "\n/* primData points to a 'Double'. Copy dummy data.\t*/");
      fprintf (fp, "\n  dest = (ST_DOUBLE *) mvluRdVaCtrl->primData;");
      fprintf (fp, "\n  *dest = %.3f;", (ST_FLOAT) rt_index + 100.2345);
      }
    }
  else if (rt_el_ptr->el_tag == RT_BIT_STRING)
    {
    if (rt_el_ptr->u.p.el_len >= 0)	/* BStringxxx len in bits	*/
      {
      fprintf (fp, "\nST_INT8 *dest;");
      fprintf (fp, "\n");
        if (indFunCtrl->arr_comp == SD_TRUE)
          {
          fprintf (fp, "\n/* This is an element of an array, get the index ... */");
          fprintf (fp, "\n  curr_index = mvluRdVaCtrl->rdVaCtrl->va->arrCtrl.curr_index;\n");
          }
      fprintf (fp, "\n/* primData points to a 'Bstring%d'. Copy dummy data.\t*/",
               rt_el_ptr->u.p.el_len);
      fprintf (fp, "\n  dest = (ST_INT8 *) mvluRdVaCtrl->primData;");
      fprintf (fp, "\n  memcpy(dest,\"\\x55\\x55\\x55\\x55\\x55\",%d);", (rt_el_ptr->u.p.el_len+7)/8);
      }
    else		/* BVStringxx ; 2 byte header then len in bits	*/
      {
      fprintf (fp, "\nST_INT16 *dest;");
      fprintf (fp, "\n");
      fprintf (fp, "\n/* primData points to a 'BVstring%d'. Copy dummy data.\t*/",
               -rt_el_ptr->u.p.el_len);
        if (indFunCtrl->arr_comp == SD_TRUE)
          {
          fprintf (fp, "\n/* This is an element of an array, get the index ... */");
          fprintf (fp, "\n  curr_index = mvluRdVaCtrl->rdVaCtrl->va->arrCtrl.curr_index;\n");
          }
      fprintf (fp, "\n  dest = (ST_INT16 *) mvluRdVaCtrl->primData;");
      fprintf (fp, "\n  *dest = %d;\t/* Just set len.\t*/", -rt_el_ptr->u.p.el_len);
      }
    }
  else if (rt_el_ptr->el_tag == RT_BINARY_TIME)
    {
    if (rt_el_ptr->u.p.el_len == 4)	/* BTIME4	*/
      {
      fprintf (fp, "\nST_INT32 *dest;");
      fprintf (fp, "\n");
        if (indFunCtrl->arr_comp == SD_TRUE)
          {
          fprintf (fp, "\n/* This is an element of an array, get the index ... */");
          fprintf (fp, "\n  curr_index = mvluRdVaCtrl->rdVaCtrl->va->arrCtrl.curr_index;\n");
          }
      fprintf (fp, "\n/* primData points to a 'Btime4'. Copy dummy data.\t*/");
      fprintf (fp, "\n  dest = (ST_INT32 *) mvluRdVaCtrl->primData;");
      fprintf (fp, "\n  *dest = 0x43200000l;");
      }
    else				/* BTIME6	*/
      {
      fprintf (fp, "\nST_INT8 *dest;");
      fprintf (fp, "\n");
      fprintf (fp, "\n/* primData points to a 'Btime6'. Copy dummy data.\t*/");
        if (indFunCtrl->arr_comp == SD_TRUE)
          {
          fprintf (fp, "\n/* This is an element of an array, get the index ... */");
          fprintf (fp, "\n  curr_index = mvluRdVaCtrl->rdVaCtrl->va->arrCtrl.curr_index;\n");
          }
      fprintf (fp, "\n  dest = (ST_INT8 *) mvluRdVaCtrl->primData;");
      fprintf (fp, "\n  memcpy(dest,\"\\x00\\x2e\\x32\\x29\\x86\\x17\",6);");
      }
    }
  else if (rt_el_ptr->el_tag == RT_UTC_TIME)
    {
    fprintf (fp, "\nMMS_UTC_TIME *dest;");
    fprintf (fp, "\n");
    if (indFunCtrl->arr_comp == SD_TRUE)
      {
      fprintf (fp, "\n/* This is an element of an array, get the index ... */");
      fprintf (fp, "\n  curr_index = mvluRdVaCtrl->rdVaCtrl->va->arrCtrl.curr_index;\n");
      }
    fprintf (fp, "\n/* primData points to a 'Utctime'. Copy dummy data.\t*/");
    fprintf (fp, "\n  dest = (MMS_UTC_TIME *) mvluRdVaCtrl->primData;");
    fprintf (fp, "\n  dest->secs=time(NULL);");
    fprintf (fp, "\n  dest->fraction=0;");
    fprintf (fp, "\n  dest->qflags=0x0E;");
    }
  else if (rt_el_ptr->el_tag == RT_UTF8_STRING)
    {
    tempPtr = ucaName;
    while (strlen (tempPtr) > (ST_UINT) abs (rt_el_ptr->u.p.el_len))
      ++tempPtr;
    fprintf (fp, "\nST_CHAR *dest;");
    fprintf (fp, "\n");
    if (indFunCtrl->arr_comp == SD_TRUE)
      {
      fprintf (fp, "\n/* This is an element of an array, get the index ... */");
      fprintf (fp, "\n  curr_index = mvluRdVaCtrl->rdVaCtrl->va->arrCtrl.curr_index;\n");
      }
    if (rt_el_ptr->u.p.el_len > 0)
      fprintf (fp, "\n/* primData points to a 'UTF8string%d'.\t*/",
               rt_el_ptr->u.p.el_len);
    else
      fprintf (fp, "\n/* primData points to a 'UTF8Vstring%d'.\t*/",
               abs (rt_el_ptr->u.p.el_len));
    fprintf (fp, "\n  dest = mvluRdVaCtrl->primData;");
    fprintf (fp, "\n/* Add code here to copy UTF8string to dest.\t*/");
    }
  else
    {
    if (indFunCtrl->arr_comp == SD_TRUE)
      {
      fprintf (fp, "\n/* This is an element of an array, get the index ... */");
      fprintf (fp, "\n  curr_index = mvluRdVaCtrl->rdVaCtrl->va->arrCtrl.curr_index;\n");
      }
    }
  }

/************************************************************************/
/************************************************************************/
/* WRITE DUMMY READ INDICATION HANDLER					*/
/************************************************************************/
/*                       writeWrIndFunNewCode				*/
/************************************************************************/

static ST_VOID writeWrIndFunNewCode (IND_FUN_CTRL *indFunCtrl, FILE *fp)
  {
ST_CHAR nameBufU[100];
ST_CHAR nameBufL[100];
ST_CHAR nameBufN[100];

  assert (indFunCtrl->ucaName);	/* this code assumes it is not NULL	*/

  makeNameDerivs (indFunCtrl->ucaName, nameBufU, nameBufL, nameBufN);

 /* Add the function to the type function temp file */
  fprintf (fp, "\n\n%s", commentBar);
  fprintf (fp, "\n/*\t\t\t%s\t\t\t\t\t*/", indFunCtrl->funName);
  fprintf (fp, "\n%s", commentBar);
  fprintf (fp, "\n/* UCA variable name: \"%s\" */", indFunCtrl->ucaName);
  if (useShortenedNames == SD_TRUE)
    {
    fprintf (fp, "\n\nST_VOID u%sWif (MVLU_WR_VA_CTRL *mvluWrVaCtrl)", 
                      nameBufN);
    }
  else
    {
    fprintf (fp, "\n\nST_VOID %s (MVLU_WR_VA_CTRL *mvluWrVaCtrl)", 
                      indFunCtrl->funName);
    }

  fprintf (fp, "\n  {");
  if (indFunCtrl->arr_comp == SD_TRUE)
    fprintf (fp, "\nST_RTINT curr_index;");
  fprintf (fp, "\nST_CHAR *src;");
  fprintf (fp, "\n");
  if (indFunCtrl->arr_comp == SD_TRUE)
    {
    fprintf (fp, "\n/* This is an element of an array, get the index ... */");
    fprintf (fp, "\n  curr_index = mvluWrVaCtrl->wrVaCtrl->va->arrCtrl.curr_index;\n");
    }
  fprintf (fp, "\n/* Get the pointer to the source for this primitive data element   */");
  fprintf (fp, "\n  src = mvluWrVaCtrl->primData;");
  fprintf (fp, "\n  mvlu_wr_prim_done (mvluWrVaCtrl, SD_SUCCESS);");
  fprintf (fp, "\n  }");
  }
  

/************************************************************************/
/************************************************************************/
/* WRITE UCA VARIABLE NAME LIST						*/
/************************************************************************/

#define MAX_PREFIX_LEN  200
#define MAX_NEST_LEVEL  50
ST_CHAR *namePrefix[MAX_NEST_LEVEL];
ST_INT nestLevel;
NAME_CTRL *currName;

/************************************************************************/
/*                       writeNameList                                  */
/************************************************************************/

ST_VOID writeNameList ()
  {
FILE *fp;
RUNTIME_TYPE *rt;
ST_INT i;
MVLU_FOUNDRY_TYPE_INFO *mvluFoundryInfo;

  fp = codeFp;
  writeCodeBreak (); 
  fprintf (fp, "/*  This file created from input file '%s'\n", inFileName);
  fprintf (fp, "    Leaf Access Parameter (LAP) File: '%s'\n", 
      useTemplate || mvlu_use_leaf_file ? templateInFileName : "Not Used");

  fprintf (fp, "\tCreated %s", ctime (&rightnow));
  fprintf (fp, "*/\n\n");

  currName = nameListHead;
  while (currName != NULL)
    {
    if (debug == SD_TRUE)
      {
      SLOGCALWAYS2 ("  Extracting names for type '%s', base '%s'", 
                        currName->typeCtrl->label, currName->base);
      }

    fprintf (codeFp, "\n\n/* Type '%s' : Base '%s' */", 
                        currName->typeCtrl->label, currName->base);

    mvluFoundryInfo = currName->typeCtrl->mvluFoundryInfo;
    rt = currName->typeCtrl->rt_ctrl->rt_first;
    for (i = 0; i < currName->typeCtrl->rt_ctrl->rt_num; ++i, ++rt, ++mvluFoundryInfo)
      {
      writeCompName (rt, mvluFoundryInfo, currName->base);
      if (rt->el_tag == RT_ARR_START)
        { 
        i += (rt->u.arr.num_rt_blks + 1);
        rt += (rt->u.arr.num_rt_blks + 1);
        mvluFoundryInfo += (rt->u.arr.num_rt_blks + 1);
        }
      }
    currName = (NAME_CTRL *) list_get_next (nameListHead, currName);
    }
  }

/************************************************************************/
/*                       writeCompName                                  */
/************************************************************************/

static ST_VOID writeCompName (RUNTIME_TYPE *rt, 
		       MVLU_FOUNDRY_TYPE_INFO *mvluFoundryInfo,
		       ST_CHAR *baseName)
  {
TYPE_CTRL *typeCtrl;
ST_CHAR ucaName[300];
ST_CHAR *p;

  if (mvluFoundryInfo->ucaName == NULL)
    return;

  p = strstr (mvluFoundryInfo->ucaName, "$");
  if (!p)
    {
    SLOGALWAYS1 ("Internal error, could not find '$' in '%s'",
                              mvluFoundryInfo->ucaName);
    exitApp ();
    }
  strcpy (ucaName, baseName);
  strcat (ucaName, p);
  
  typeCtrl = (TYPE_CTRL *) mvluFoundryInfo->typeCtrl;
  if (ms_comp_name_pres(rt))
    {
    if (createVa == SD_TRUE)
      {
      if (typeCtrl != NULL)
        {
        fprintf (codeFp, "\n\":V\",\"%s\",\"%s\"", ucaName, typeCtrl->label);
        }
      else
        {
        fprintf (codeFp, "\n\":V\",\"%s\",\"%s\"", 
                		ucaName, "Must Use Type ID, not TDL");
        SLOGALWAYS3 ("Error at input line %d (%d in file '%s')", 
                        lineCount, fileLine[fileNestLevel], fileName[fileNestLevel]);
        SLOGCALWAYS0 ("Must Use Type ID, not TDL");
        }
      }
    else        /* Just names */
      fprintf (codeFp, "\n%s", ucaName);
    }
  }


/************************************************************************/
/************************************************************************/
/************************************************************************/
/*                       writeLeafRtInit                                  */
/************************************************************************/

extern DEFINE_CTRL *tfnRefDefineList;

static ST_VOID writeRtLeafInit (FILE *xmlFp, RUNTIME_TYPE *rt, 
		       MVLU_FOUNDRY_TYPE_INFO *mvluFoundryInfo,
		       ST_CHAR *baseName)
  {
TYPE_CTRL *typeCtrl;
LEAF_ACCESS_INFO *la;
ST_CHAR ucaName[300];
ST_CHAR *p;
IND_FUN_CTRL *ic;
DEFINE_CTRL *ctrlptr;
ST_CHAR *rdFun;
ST_CHAR *wrFun;
ST_CHAR *refText;

  if (mvluFoundryInfo->ucaName == NULL)
    return;

  if (mvlu_use_leaf_file)
    la = findLeafAccessInfo (mvluFoundryInfo->ucaName);
  else
    la = NULL;

  p = strstr (mvluFoundryInfo->ucaName, "$");
  if (!p)
    {
    SLOGALWAYS1 ("Internal error, could not find '$' in '%s'",
                              mvluFoundryInfo->ucaName);
    exitApp ();
    }
  strcpy (ucaName, baseName);
  strcat (ucaName, p);
  
  typeCtrl = (TYPE_CTRL *) mvluFoundryInfo->typeCtrl;
  
  /* Find the read function */
  ic = rdIndFunCtrlHead;
  while (ic != NULL)
    {
    if (!strcmp (rt->rdIndexBuf, ic->funIndexDefine))
      break;
    ic = (IND_FUN_CTRL *) list_get_next (rdIndFunCtrlHead, ic);
    }
  if (ic != NULL)
    rdFun = ic->funName;
  else
    rdFun = "_Unknown";
  
  /* Find the write function */
  ic = wrIndFunCtrlHead;
  while (ic != NULL)
    {
    if (!strcmp (rt->wrIndexBuf, ic->funIndexDefine))
      break;
    ic = (IND_FUN_CTRL *) list_get_next (wrIndFunCtrlHead, ic);
    }
  if (ic != NULL)
    wrFun = ic->funName;
  else
    wrFun = "_Unknown";

  
  refText = "";
  if (mvlu_use_leaf_file)
    {
    if (la != NULL)
      {
      if (la->refString[0])
        refText = la->refString;
      }
    }
  else /* Not using leaf file, see if using TFN */
    {    
    if (rt->refBuf != NULL)
      {
      ctrlptr = tfnRefDefineList;
      while (ctrlptr)
        {
        if (strcmp (ctrlptr->lValue, rt->refBuf) == 0)
          {
          refText = ctrlptr->rValue;
          break;
	  }
        ctrlptr = (DEFINE_CTRL *) list_get_next (tfnRefDefineList, ctrlptr);
        }
      }
    }
  

  fprintf (xmlFp, "\t<Leaf Name=\"%s\" RdIndFun=\"%s\" WrIndFun=\"%s\" Ref=\"%s\"/>\n", 
  		ucaName, rdFun, wrFun, refText);
  }


/************************************************************************/
/*                       writeLeafInit                                  */
/************************************************************************/

ST_VOID writeLeafInit ()
  {
FILE *xmlFp;
RUNTIME_TYPE *rt;
ST_INT i;
MVLU_FOUNDRY_TYPE_INFO *mvluFoundryInfo;
TYPE_CTRL *typeCtrl;

/* Now write the XML file */
  if ((xmlFp = fopen (xmlFileName, "w")) == NULL)
    return;
  fprintf (xmlFp, "<Leafmap>\n");

  typeCtrl = typeHead;
  i = 0;
  while (typeCtrl != NULL)
    {
#ifdef OBSOLETE_TWINNING_ENABLE		/* DEBUG: delete later?	*/
    if (typeCtrl->ucaType && !typeCtrl->twin)
#else
    if (typeCtrl->ucaType)
#endif
      {
      if (typeCtrl->twin)
        fprintf (xmlFp, "\t<!--WARNING: Type '%s' is a duplicate of '%s'. The following set of leaf functions can be eliminated if type '%s' is eliminated.-->\n",
                         typeCtrl->label, typeCtrl->twin->label, typeCtrl->label);
        
      mvluFoundryInfo = typeCtrl->mvluFoundryInfo;

      rt = typeCtrl->rt_ctrl->rt_first;
      for (i = 0; i < typeCtrl->rt_ctrl->rt_num; ++i, ++rt, ++mvluFoundryInfo)
        {
        if (ms_is_rt_prim (rt) == SD_TRUE)
          {
          writeRtLeafInit (xmlFp, rt, mvluFoundryInfo, typeCtrl->label);
          if (rt->el_tag == RT_ARR_START)
            { 
            i += (rt->u.arr.num_rt_blks + 1);
            rt += (rt->u.arr.num_rt_blks + 1);
            mvluFoundryInfo += (rt->u.arr.num_rt_blks + 1);
            }
          }
        }
      }
    typeCtrl = (TYPE_CTRL *) list_get_next (typeHead, typeCtrl);
    }
  fprintf (xmlFp, "</Leafmap>\n");
  fclose (xmlFp);
  }


/************************************************************************/
/*			writeLeafTbls					*/
/************************************************************************/

ST_VOID writeLeafTbls (FILE *fp)
  {
IND_FUN_CTRL *ic;
IND_FUN_CTRL *nextIc;
ST_INT indFunIndex;


  fprintf (fp, "\n");
  fprintf (fp, "/* Leaf Access Function Pointer Tables                    */\n");
  fprintf (fp, "MVLU_RD_FUN_INFO mvluRdFunInfoTbl[] =\n");
  fprintf (fp, "  {\n");
  indFunIndex = 0;
  ic = rdIndFunCtrlHead;
  while (ic != NULL)
    {
    fprintf (fp, "    {_MVLU_LEAF_FUN_INIT (%s)}", ic->funName);
    nextIc = (IND_FUN_CTRL *) list_get_next (rdIndFunCtrlHead, ic);
    if (nextIc)
      fprintf (fp, ",");
    fprintf (fp, "\t/* %d=%s\t*/\n", indFunIndex, ic->funIndexDefine);
    ++indFunIndex;
    ic = nextIc;
    }
  fprintf (fp, "  };\n");
  fprintf (fp, "ST_INT mvluNumRdFunEntries = sizeof(mvluRdFunInfoTbl)/sizeof(MVLU_RD_FUN_INFO);\n");

  fprintf (fp, "\n");
  fprintf (fp, "MVLU_WR_FUN_INFO mvluWrFunInfoTbl[] =\n");
  fprintf (fp, "  {\n");
  indFunIndex = 0;
  ic = wrIndFunCtrlHead;
  while (ic != NULL)
    {
    fprintf (fp, "    {_MVLU_LEAF_FUN_INIT (%s)}", ic->funName);
    nextIc = (IND_FUN_CTRL *) list_get_next (wrIndFunCtrlHead, ic);
    if (nextIc)
      fprintf (fp, ",");
    fprintf (fp, "\t/* %d=%s\t*/\n", indFunIndex, ic->funIndexDefine);
    ++indFunIndex;
    ic = nextIc;
    }
  fprintf (fp, "  };\n");
  fprintf (fp, "ST_INT mvluNumWrFunEntries = sizeof(mvluWrFunInfoTbl)/sizeof(MVLU_WR_FUN_INFO);\n");
  }

/************************************************************************/
/************************************************************************/
/* DERIVE UCA NAMES FROM TYPES						*/
/************************************************************************/
/*                       deriveUcaNames                                 */
/************************************************************************/

ST_VOID deriveUcaNames ()
  {
TYPE_CTRL *current_type;
MVLU_UCA_NAME_CTRL *ucaNames;
MVLU_UCA_NAME_CTRL *uc;
RUNTIME_TYPE *rt;
ST_INT i;

  namePrefix[0] = (ST_CHAR *) chk_calloc (1, MAX_PREFIX_LEN);
  current_type = typeHead;
  while (current_type != NULL)
    {
    if (mvlu_proc_rt_type (current_type->label, 
    		       current_type->rt_ctrl, &ucaNames))
      {
      SLOGALWAYS1 ("FATAL: IEC/UCA post-processing of RUNTIME_TYPE failed for '%s'.",
                   current_type->label);
      exitApp ();
      }

    uc = ucaNames;
    for (i = 0; i < current_type->rt_ctrl->rt_num; ++i, ++uc)
      {
      if (strlen (uc->ucaName))
        {
        rt = uc->rt;
        current_type->mvluFoundryInfo[uc->rtIndex].ucaName = chk_strdup (uc->ucaName);

        /* NOTE: for el_tag==RT_ARR_START, next "rt"			*/
        /* has the same "ucaName" (see _mvlu_build_uca_name_tbl).	*/
	}
      }
    current_type = (TYPE_CTRL *) list_get_next (typeHead, current_type);
    }
  }


/************************************************************************/
/************************************************************************/
/* MISC. UTILITY FUNCTIONS						*/
/************************************************************************/
/*                       makeNameDerivs					*/
/************************************************************************/
static ST_VOID makeNameDerivs (ST_CHAR *ucaVarName, ST_CHAR *nameDestU,
                               ST_CHAR *nameDestL, ST_CHAR *nameDestN)
  {
ST_INT i, j;
ST_INT nameLen;

  assert (ucaVarName);		/* this code assumes it is not NULL*/
  strcpy (nameDestU, ucaVarName);
  nameLen = (ST_INT) strlen (nameDestU);
  for (i = 0; i < nameLen; ++i)
    {
    if (((caseSensStrIdxs == SD_FALSE))&&(islower (nameDestU[i])))
      nameDestU[i] = toupper (nameDestU[i]);
    else if (nameDestU[i] == '$')
      nameDestU[i] = '_';
    }

  strcpy (nameDestL, ucaVarName);
  for (i = 0; i < nameLen; ++i)
    {
    if (((caseSensStrIdxs == SD_FALSE)) && (isupper (nameDestL[i])))
      nameDestL[i] = tolower (nameDestL[i]);
    else if (nameDestL[i] == '$')
      nameDestL[i] = '_';
    }

  for (j = 0, i = 0; i < nameLen;)
    {
    if (ucaVarName[i] == '$')
      {
      ++i;
      nameDestN[j++] = toupper(ucaVarName[i++]);
      }
    else if (i == 0)
      nameDestN[j++] = toupper(ucaVarName[i++]);
    else 
      nameDestN[j++] = tolower(ucaVarName[i++]);
    }
  nameDestN[j] = 0;
  }


/************************************************************************/
/************************************************************************/

ST_RET loadXmlLeafAccessInfo (ST_CHAR *fileName)
  {
FILE *fp;
LEAF_ACCESS_INFO *la;
ST_CHAR buf[1000];
ST_CHAR *p;
ST_RET ret;

  fp = fopen (fileName, "r");
  if (fp == NULL)
    {
    SLOGALWAYS1 ("Could not open leaf map XML file '%s'", fileName);
    printf ("\nWARNING: Could not open leaf map XML file '%s'", fileName);
    return (SD_FAILURE);
    }

/* Now read in the file and set the leaf parameters */
  ret = SD_SUCCESS;
  while (SD_TRUE)
    {
    la = (LEAF_ACCESS_INFO *) chk_calloc (1, sizeof (LEAF_ACCESS_INFO));
    la->leafName[0] = 0;
    la->rdIndFunName[0] = 0;
    la->wrIndFunName[0] = 0;
    la->refString[0] = 0;

    if (fgets (buf, sizeof(buf) - 1, fp) == NULL)
      break;
    if (strstr (buf, "<Leafmap>") != NULL)
      continue;
    if (strstr (buf, "</Leafmap>") != NULL)
      break;
    p = strstr (buf, "<Leaf ");
    if (!p)
      continue;

    _getAttribVal (p, "Name", la->leafName, sizeof (la->leafName));
    _getAttribVal (p, "RdIndFun", la->rdIndFunName, sizeof (la->rdIndFunName));
    _getAttribVal (p, "WrIndFun", la->wrIndFunName, sizeof (la->wrIndFunName));
    _getAttribVal (p, "Ref", la->refString, sizeof (la->refString));
     
    if (la->leafName[0] == 0)
      {
      SLOGALWAYS1 ("ERROR: 'Name' attribute not found in '%s'. XML input file processing STOPPED.", p);
      printf      ("ERROR: 'Name' attribute not found in '%s'. XML input file processing STOPPED.\n", p);
      ret = SD_FAILURE;
      break;
      }
    list_add_last ((ST_VOID **) &leafAccessHead, la);
    }

  fclose (fp);
  return (ret);
  }


/************************************************************************/

static ST_VOID _getAttribVal (ST_CHAR *xmlBuf, ST_CHAR *attribName, 
		       ST_CHAR *dest, ST_INT destSize)
  {
ST_CHAR *p;
ST_CHAR look[50];

  *dest = 0;
  strcpy (look, attribName);
  strcat (look, "=\"");
  p = strstr (xmlBuf, look);
  if (!p)
    {
    SLOGALWAYS2 ("WARNING: '%s' attribute not found in this XML leaf input: %s", attribName, xmlBuf);
    printf      ("WARNING: '%s' attribute not found in this XML leaf input: %s\n", attribName, xmlBuf);
    return;
    }

  p += strlen (look);
  while (*p != 0 && *p != '"' && destSize > 0)
    {
    *(dest++) = *(p++);
    --destSize;
    }
  *dest = 0;
  }



/************************************************************************/

LEAF_ACCESS_INFO *findLeafAccessInfo (ST_CHAR *leafName)
  {
LEAF_ACCESS_INFO *la;

  la = leafAccessHead;
  while (la != NULL)
    {
    if (!strcmp (leafName, la->leafName))
      return (la);
    la = (LEAF_ACCESS_INFO *) list_get_next (leafAccessHead, la); 
    }
  return (NULL);
  }


/************************************************************************/
/*		logUnusedLeafAccessInfo					*/
/* Log any leaf names configured in the input XML file that are NOT	*/
/* used by the Object Model. This should help users that may have	*/
/* misspelled some leaf names or used the wrong input file.		*/
/************************************************************************/
ST_VOID logUnusedLeafAccessInfo ()
  {
LEAF_ACCESS_INFO *la;

  la = leafAccessHead;
  while (la != NULL)
    {
    /* Log if leaf entry not used, but only if leaf is NOT "$dynamic".	*/
    /* "&dynamic" leafs are NEVER used until runtime.			*/
    if (!la->leafUsed && strcmp (la->leafName, "$dynamic") != 0)
      {
      SLOGALWAYS2 ("'%s' entry from input file '%s' NOT used",
          la->leafName, templateInFileName);
      printf      ("'%s' entry from input file '%s' NOT used\n",
          la->leafName, templateInFileName);
      }
    la = (LEAF_ACCESS_INFO *) list_get_next (leafAccessHead, la); 
    }
  return;
  }

/************************************************************************/
/*			addDynLeafFuns					*/
/* Search the input XML file data (saved in leafAccessHead linked list)	*/
/* for leafs named "$dynamic" and add the corresponding read and write	*/
/* leaf functions to the "rdIndFunCtrlHead" & "wrIndFunCtrlHead"	*/
/* linked lists, respectively. These leaf functions may be used at	*/
/* runtime for dynamic type creation.					*/
/* NOTE: the "reference" in the XML input file is ignored.		*/
/* NOTE: this function only works if leaf input file is XML (not TFN).	*/
/************************************************************************/
ST_VOID addDynLeafFuns ()
  {
ST_CHAR *funName;
IND_FUN_CTRL *indFunCtrl;
LEAF_ACCESS_INFO *la;		/* from XML leaf input file	*/
ST_BOOLEAN foundUnused = SD_FALSE;

  assert (mvlu_use_leaf_file);	/* only works if leaf input file is XML	*/

  /* Search "leafAccessHead" linked list for all leafs named "$dynamic".*/
  for (la = leafAccessHead;
       la != NULL;
       la = (LEAF_ACCESS_INFO *) list_get_next (leafAccessHead, la))
    {
    if (!stricmp ("$dynamic", la->leafName))
      {
      /* Process la->rdIndFunName	*/
      if (la && la->rdIndFunName[0])
        funName = la->rdIndFunName;
      else
        funName = _MVLU_NULL_READ_FUN_NAME;

      /* If func NOT already on RD list, create & add to RD list.	*/
      if (!findRdIndFun (funName))
        {
        indFunCtrl = createIndFunCtrl (funName);
        list_add_last ((ST_VOID **)&rdIndFunCtrlHead, indFunCtrl);
        }

      /* Process la->wrIndFunName (similar)	*/
      if (la && la->wrIndFunName[0])
        funName = la->wrIndFunName;
      else
        funName = _MVLU_NULL_WRITE_FUN_NAME;

      /* If func NOT already on WR list, create & add to WR list.	*/
      if (!findWrIndFun (funName))
        {
        indFunCtrl = createIndFunCtrl (funName);
        list_add_last ((ST_VOID **)&wrIndFunCtrlHead, indFunCtrl);
        }
      }	/* end if "$dynamic"	*/
    else if (la->leafUsed == SD_FALSE)
      {
      /* This extra bit of code is just to make "lefttodo.txt" a little
       * more informative. If this leaf name is NOT "$dynamic" and it is
       * NOT used by object model, log it to "lefttodo.txt".
       */
      if (!foundUnused)
        {	/* write header when first unused leaf found	*/
        foundUnused = SD_TRUE;
        fprintf (newFp, "\n\n# The following leafs were defined in the input file\n# '%s', but were NOT used by the object model.\n# Check spelling.\n",
               templateInFileName);
        }
      fprintf (newFp, "%s\n", la->leafName); 
      }
    }	/* end main loop	*/

  if (!foundUnused)
    fprintf (newFp, "\n\n# ALL leafs defined in the input file '%s' were used by the object model.\n",
             templateInFileName);
  return;
  }

/************************************************************************/
/*			createIndFunCtrl				*/
/* Alloc and initialize IND_FUN_CTRL struct.				*/
/* Called ONLY by "addDynLeafFuns" above.				*/
/* NOTE: this function is only used for "dynamic" leaf functions. For	*/
/*    these, we don't need to set all params in indFunCtrl.		*/
/*    ONLY funName, funCtrl, and funIndexDefine are set in the struct.	*/
/************************************************************************/
static IND_FUN_CTRL *createIndFunCtrl (ST_CHAR *funName)
  {
IND_FUN_CTRL *indFunCtrl;
ST_CHAR tmpBuf[100];	/* tmp buf to create funIndexDefine string	*/
ST_INT nameLen, i;

  indFunCtrl = (IND_FUN_CTRL *) chk_calloc (1, sizeof (IND_FUN_CTRL));

  indFunCtrl->funName = chk_strdup (funName);
  indFunCtrl->funCtrl = (FUN_CTRL *) chk_calloc (1, sizeof(FUN_CTRL));
  indFunCtrl->funCtrl->funType = FUNTYPE_EXTERN;

  /* Create the function index define to be used.	*/
  strcpy (tmpBuf, funName);
  strcat (tmpBuf, "_INDEX");
  nameLen = (ST_INT) strlen (tmpBuf);
  if (caseSensStrIdxs == SD_FALSE)
    {	/* convert to upper case	*/
    for (i = 0; i < nameLen; ++i)
      tmpBuf[i] = toupper (tmpBuf[i]);
    }
  indFunCtrl->funIndexDefine = chk_strdup (tmpBuf);
  return (indFunCtrl);
  }
