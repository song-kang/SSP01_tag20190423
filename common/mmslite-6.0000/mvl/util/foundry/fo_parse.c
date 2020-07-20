/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,       */
/*               1997-2003, All Rights Reserved				*/
/*                                                                      */
/* MODULE NAME : fo_parse.c                                              */
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
/* 01/30/06  GLB     25    Integrated porting changes for VMS           */
/* 09/09/05  JRB     24    Log & exit on unrecognized commands, old code*/
/*			   assumed unrecognized cmd was ":T" cmd.	*/
/* 05/12/05  JRB     23    Print warning if AA_SCOPE obj in ODF file.	*/
/* 02/21/05  JRB     22    Fix checking of control strings (:CC, :CP,	*/
/*			   etc) & fix "Bad control string..." log msg.	*/
/* 12/11/03  MDE     21    Made addIncPath global			*/
/* 03/13/03  JRB     20    Use RUNTIME_CTRL *rt_ctrl in TYPE_CTRL.	*/
/*			   u_ml_get_rt_type: chg from "func ptr" to func.*/
/* 01/01/03  JRB     19    Add findRtType arg, avoid global m_calc_rt..	*/
/* 12/02/02  MDE     18    XML LAP, other enhancements			*/
/* 11/11/02  JRB     17    Del mkRtFromAsn1, use ms_runtime_create	*/
/* 01/29/02  JRB     16    Fix compile err: chg VOID to ST_VOID.	*/
/* 01/24/02  EJV     15    Added support for DOM_SPEC journals.		*/
/* 01/08/02  JRB     14    Converted to use ASN1R (re-entrant ASN1)	*/
/* 06/21/00  MDE     13    Now can emit type list w/TDL (-l)		*/
/* 04/26/00  RKR     12    Added caseSensStrIdxs			*/
/* 03/07/00  MDE     11    Added default read/write function feature	*/
/* 05/26/99  MDE     10    Use updated TFN file processing		*/
/* 05/26/99  MDE     09    Now use chk_strdup				*/
/* 04/22/99  MDE     08    Fixed crash w/o TFN, added 'X' flag to types	*/
/* 04/12/99  MDE     07    Open all files in text mode			*/
/* 01/08/99  JRB     06    Chg for bsearch. Don't need prefix on nvlist.*/
/* 10/01/98  MDE     05    Tweaks for 16 bit compilers			*/
/* 09/21/98  MDE     04    Minor lint cleanup				*/
/* 09/16/98  MDE     03    Now create Journals too			*/
/* 09/10/98  MDE     02    Minor fixes for handling UCA/non-UCA types	*/
/* 07/21/98  MDE     01    New module, extracted from foundry.c		*/
/************************************************************************/

#include "foundry.h"

/************************************************************************/
/************************************************************************/

SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;

/************************************************************************/

/* Object Definition File Inclusion */
static FILE_CTRL *hdrHead;

/* Object Definition Include Paths */
static PATH_CTRL pathCtrl;

/* Default object attribute flags */
static ST_BOOLEAN varDefaultData;
static ST_BOOLEAN varDefaultProc;
static ST_BOOLEAN typeDefaultUnique;
static ST_BOOLEAN typeDefaultTransient;
static ST_BOOLEAN typeDefaultBase;
static ST_BOOLEAN typeDefaultDeleteIfUnused;
static ST_BOOLEAN typeDefaultUca;

static ST_BOOLEAN tempFileUsed;

/* Temporary storage for parser to use */
static ST_CHAR userFlagsIn[MAX_STRING_SIZE+1];
static ST_CHAR userLabelIn[MAX_STRING_SIZE+1];

/************************************************************************/

static ST_VOID processIncludes (ST_VOID);
static FILE_CTRL *findIncluded (ST_CHAR *hdrFile);
static ST_VOID addIncluded (ST_CHAR *incName);
static ST_VOID handleFileName (ST_VOID);
static ST_VOID handleControl (ST_VOID);
static ST_VOID handleVar (ST_VOID);
static ST_VOID handleDom (ST_VOID);
static ST_VOID handleVarList (ST_VOID);
static ST_VOID handleType (ST_VOID);
static ST_VOID handleInc (ST_VOID);
static ST_VOID handleJou (ST_VOID);
static ST_VOID handleNameExtract (ST_VOID);
static ST_VOID getQuotedString (ST_CHAR *dest, ST_BOOLEAN ws_allowed_in_string);
static ST_VOID expandSource (FILE *dest, FILE *src);
static ST_VOID incLineCount (ST_VOID);
static TYPE_CTRL *findType (ST_CHAR *typeName);
static DOMAIN_CTRL *findDom (ST_CHAR *domName, ST_INT *domIndexOut);
static VAR_CTRL *findVar (ST_CHAR *varName, VAR_CTRL *listHead);
static VARLIST_CTRL *findVarList (ST_CHAR *varListName, VARLIST_CTRL *listHead);
static ST_VOID *findJou (ST_CHAR *jouName, ST_CHAR *domName, ST_INT jouType);

ST_VOID loadDefaultFunSet (ST_CHAR *funFileName);

/************************************************************************/
/************************************************************************/
/*                       processSource                                  */
/************************************************************************/
/* Parse the input file and create the in-memory data structures in     */
/* preparation for writing the output.                                  */

ST_RET processSource ()
  {
ST_INT i;

  if (createTypeListFile == SD_TRUE)
    {
    typeListFp = fopen ("stdDataTypes.txt", "w");
    if (typeListFp == NULL)
      {
      printf ("\n  WARNING: could not open type list file");
      SLOGALWAYS0 ("WARNING: could not open type list file");
      }
    }

/*  Open our input files first						*/
  if ((sourceFp = fopen (inFileName, "r")) == NULL)
    {
    printf ("Error, cannot open input file '%s'\n", inFileName);
    usage();
    }

  if (useTemplate)
    {
    if ((templateFp = fopen (templateInFileName, "r")) == NULL)
      {
      printf ("Error, cannot open input file '%s'\n", templateInFileName);
      usage();
      }
    processTfnFile (templateFp);
    }
  if (mvlu_use_leaf_file)
    loadXmlLeafAccessInfo (templateInFileName);

  if (extractNames == SD_FALSE)
    readAlgnTable ();

  m_max_rt_aa_ctrl = 20000;
  fileName[0] = inFileName;

/* Here we will scan the input file for #include statements and if so   */
/* we will create a new temporary file that has the includes resolved.  */
  processIncludes ();

/* Now go through the input file and deal with each input record.       */
  if (debug == SD_TRUE)
    {
    SLOGALWAYS0 ("Searching for object definitions: ");
    }

  while ((i = getc (sourceFp)) != EOF) 
    {
    if (i == '\n')
      {
      incLineCount ();
      continue;
      }
    if (i == '#')               /* Comment line, skip to EOL            */
      {
      while ((i = getc (sourceFp)) != '\n' && (i != EOF))
        ;
      if (i == '\n')
        incLineCount ();
      continue;                 /* Re-start while loop.                 */
      }
    if (i != '"')               /* INteresting things start with a "    */
      continue;                 /* Re-start while loop.                 */

    ungetc (i, sourceFp);         /* Put the first character back.      */

  /* OK, we are at the start of something interesting, get a string ... */
  /* The first thing will usually be the flags string.                  */
    getQuotedString (userFlagsIn, SD_FALSE);
    if (userFlagsIn[0] == ':')
      {
      if (userFlagsIn[1] == 'C')
        handleControl ();
      else if (userFlagsIn[1] == 'I')
        handleInc ();
      else if (userFlagsIn[1] == 'T')
        {                  /* The type handler expects flags and label  */
        getQuotedString (userLabelIn, SD_FALSE);
        handleType ();
        }
      else if (userFlagsIn[1] == 'D')
        handleDom ();
      else if (userFlagsIn[1] == 'V')
        handleVar ();
      else if (userFlagsIn[1] == 'L')
        handleVarList ();
      else if (userFlagsIn[1] == 'N')
        handleNameExtract ();
      else if (userFlagsIn[1] == 'F')
        handleFileName ();
      else if (userFlagsIn[1] == 'J')
        handleJou ();
      else
        {
        SLOGALWAYS1 ("Fatal: Command '%s' not supported.", userFlagsIn);
        exitApp ();
        }
      }
    else        /* No flags, default is type */
      {
      SLOGALWAYS1 ("Fatal: Command '%s' illegal (must start with ':').", userFlagsIn);
      exitApp ();
      }
    }
  return (SD_SUCCESS);
  }

/************************************************************************/
/************************************************************************/
/* INCLUDE DIRECTIVE PROCESSING						*/
/************************************************************************/
/*                       processIncludes                                */
/************************************************************************/
/* Check the main file for include directives. If so, open a temp file  */
/* and resolve all includes by using expandSource.                      */

static ST_VOID processIncludes ()
  {
ST_CHAR lineBuf[MAX_CFG_LINE+1];
ST_CHAR *p;
ST_BOOLEAN includes;
FILE *fp;

/* Scan the main file for include directives */
  includes = SD_FALSE;
  while (SD_TRUE)
    {
    if (fgets (lineBuf, MAX_CFG_LINE, sourceFp) == NULL)
      break;

  /* remove all text after a comment, if present */
    p = strstr (lineBuf, "#");
    if (p != NULL)
      *p = 0;

    if (!strncmp (lineBuf,"include ", 8) ||
        strstr (lineBuf, "\":CI\"") != NULL)
      {
      includes = SD_TRUE;
      break;
      }
    }

  fseek (sourceFp, 0, SEEK_SET);
  if (includes == SD_FALSE)
    {
    tempFileUsed = SD_FALSE;
    return;
    }

/* OK, we have a nested situation here */
  fp = fopen ("foundry.tmp", "w");
  if (!fp)
    {
    SLOGALWAYS0 ("Could not open temporary file (foundry.tmp)");
    exitApp ();
    }
  expandSource (fp, sourceFp);
  fclose (sourceFp);
  fclose (fp);
  tempFileUsed = SD_TRUE;
  sourceFp = fopen ("foundry.tmp", "r");
  }

/************************************************************************/
/*                       expandSource                                   */
/************************************************************************/
/* Recursive function to copy the contents of the source file to the    */
/* destination. When an include directive is found, expand that file    */
/* by recursive call.                                                   */

static ST_VOID expandSource (FILE *dest, FILE *src )
  {
ST_CHAR lineBuf[MAX_CFG_LINE+1];
ST_CHAR lineBuf2[MAX_CFG_LINE+1];
ST_CHAR pathBuf[256];
ST_CHAR lastChar;
FILE *newSrc;
ST_BOOLEAN includeOk;
ST_INT i;
ST_INT j;
ST_CHAR *p;
ST_CHAR *p2;

  while (SD_TRUE)
    {
    if (fgets (lineBuf, MAX_CFG_LINE, src) == NULL)
      break;

  /* Path handling ... */
    if (!strncmp (lineBuf,"path ", 5) ||
        !strncmp (lineBuf,"\":CP\"", 5))
      {
      strcpy (lineBuf2, lineBuf);
      for (i = 5; i < MAX_CFG_LINE; ++i)
        {
        if (lineBuf2[i] && 			/* Start of path? */
	    !isspace (lineBuf2[i]) && 
            lineBuf2[i] != ',' && 		
            lineBuf2[i] != ' ' && 		
            lineBuf2[i] != '"') 		
          {
          for (j = i; j < MAX_CFG_LINE - i; ++j) /* Find end of path */
            {
            if (lineBuf2[j] == 0 || 
                lineBuf2[j] == ' ' ||
                lineBuf2[j] == '\t' ||
                lineBuf2[j] == '\n' ||
                lineBuf2[j] == '#' ||
                lineBuf2[j] == '"')
              {
              lineBuf2[j] = 0;
              addIncPath (&lineBuf2[i]);
              break;
              }
            }
          break;
          }
        }
      }

  /* OK, now check for an include directive */
    
    p = strstr (lineBuf, "\":CI\"");
    if (p != NULL)
      {
      p2 = strstr (lineBuf, "#");
      if (p2 != NULL && p2 < p)
        p = NULL;
      }

    if (p != NULL || !strncmp (lineBuf,"include ", 8))
      {
      includeOk = SD_FALSE;

    /* Find the end of the 'include' indicator */
      if (p == NULL)
        i = 7;
      else
        i = (p - lineBuf) + 5;

    /* Start looking for the filename to include */
      for (; i < MAX_CFG_LINE; ++i)
        {
        if (isalpha(lineBuf[i]))
          {
          for (j = i; j < MAX_CFG_LINE - i; ++j)
            {
            if (lineBuf[j] == 0 || 
                lineBuf[j] == ' ' ||
                lineBuf[j] == '\t' ||
                lineBuf[j] == '\n' ||
                lineBuf[j] == '"' ||
                lineBuf[j] == '#')
              {
              lineBuf[j] = 0;
              if (findIncluded (&lineBuf[i]) == NULL)
                {
                addIncluded (&lineBuf[i]);
                newSrc = fopen (&lineBuf[i], "r");
                if (!newSrc)
                  {
                  for (j = 0; j < pathCtrl.numPath && !newSrc; ++j)
                    {
                    strcpy (pathBuf, pathCtrl.pathText[j]);
                    lastChar = pathBuf[strlen(pathBuf)-1];
                    if (lastChar != '/' && lastChar != '\\')
                      strcat (pathBuf, "/");
                    strcat (pathBuf, &lineBuf[i]);
                    newSrc = fopen (pathBuf, "r");
                    }
                  }

                if (!newSrc)
                  {
                  SLOGALWAYS1 ("Error: Could not open include file '%s'", 
                                                                &lineBuf[i]);
                  exitApp ();
                  }
                SLOGCALWAYS1 ("Including file    : '%s'", &lineBuf[i]);
                fprintf (dest,"\":FS\",\"%s\"\n",&lineBuf[i]);
                expandSource (dest, newSrc);
                fprintf (dest,"\":FE\",\"%s\"\n",&lineBuf[i]);
                fclose (newSrc);
                includeOk = SD_TRUE;
                break;
                }
              else
                {
                SLOGCALWAYS1 ("File : '%s' already included", &lineBuf[i]);
                fprintf (dest,"\n### Include '%s' already included\n",&lineBuf[i]);
                includeOk = SD_TRUE;
                break;
                }
              }
            }
          break;
          }
        }
      if (includeOk == SD_FALSE)
        {
        SLOGALWAYS0 ("Error: include file problem");
        exitApp ();
        }
      }
    else if (fputs (lineBuf, dest) == EOF)
      {
      SLOGALWAYS0 ("Error: Could not write to temporary file");
      exitApp ();
      }
    }
  }

/************************************************************************/
/*                      addIncPath                                      */
/************************************************************************/

ST_VOID addIncPath (ST_CHAR *pathText)
  {
  if (pathCtrl.numPath >= MAX_NUM_PATH)
    {
    SLOGCALWAYS0 ("Too many path statements");
    exitApp ();
    }

  pathCtrl.pathText[pathCtrl.numPath] = chk_strdup (pathText);
  ++pathCtrl.numPath;
  }

/************************************************************************/
/*                       findIncluded                                   */
/************************************************************************/

static FILE_CTRL *findIncluded (ST_CHAR *hdrFile)
  {
FILE_CTRL *hdr;

  hdr = hdrHead;
  while (hdr != NULL) 
    {      
    if (!strcmp (hdr->hdrFile, hdrFile))
      return (hdr);
    hdr = (FILE_CTRL *) list_get_next (hdrHead, hdr);
    }

  return (NULL);
  }

/************************************************************************/
/*                       addIncluded                                    */
/************************************************************************/

static ST_VOID addIncluded (ST_CHAR *hdrFile)
  {
FILE_CTRL *hdr;

  hdr = (FILE_CTRL *) chk_calloc (1, sizeof (FILE_CTRL));
  hdr->hdrFile = chk_strdup (hdrFile);
  list_add_last ((ST_VOID **) &hdrHead, hdr);
  }


/************************************************************************/
/************************************************************************/
/* OBJECT HANDLING 							*/
/************************************************************************/
/*                      handleControl                                   */
/************************************************************************/

static ST_VOID handleControl ()
  {
ST_CHAR string2[MAX_STRING_SIZE+1];

  if (!strcmp (userFlagsIn, ":CS"))  /* case sensitivity for	*/
    {					  /* String Indexes 		*/
    caseSensStrIdxs = SD_TRUE;
    }
  else
    {
    getQuotedString (string2, SD_TRUE);

    if (debug == SD_TRUE)
      {
      SLOGCALWAYS1 ("String 2 = '%s'", string2);
      }

    if (!strcmp (userFlagsIn, ":CC"))     /* Config file          */
      {
      strcpy (cfgFile, string2);
      if (debug == SD_TRUE)
         {
        SLOGCALWAYS1 ("Config file = '%s'", cfgFile);
        }
      readAlgnTable ();
      }
    else if (!strcmp (userFlagsIn, ":CP"))     /* path          */
      {
	/* Just ignore a 'path' line */
      }
    else if (!strcmp (userFlagsIn, ":CFUN"))     /* path          */
      {
      loadDefaultFunSet (string2);
      }
    else if (!strcmp (userFlagsIn, ":CF"))        /* Flags                */
      {
      if (!strncmp (string2, ":V",2))               /* Variable flags     */
        {
        if (strchr (&string2[2], 'D') != NULL)
          varDefaultData = SD_TRUE;
        else
          varDefaultData = SD_FALSE;
        if (debug == SD_TRUE)
          {
          SLOGCALWAYS1 ("Variable Data Flag = '%s'", 
                                varDefaultData ? "TRUE" : "FALSE");
          }

        if (strchr (&string2[2], 'P') != NULL)      
          varDefaultProc = SD_TRUE;
        else
          varDefaultProc = SD_FALSE;
        if (debug == SD_TRUE)
          {
          SLOGCALWAYS1 ("Variable Proc Flag = '%s'", 
                                varDefaultProc ? "TRUE" : "FALSE");
          }
        }
      else if (!strncmp (string2, ":T",2))          /* Type flags         */ 
        {
        if (strchr (&string2[2], 'U') != NULL)
          typeDefaultUnique = SD_TRUE;
        else
          typeDefaultUnique = SD_FALSE;
        if (debug == SD_TRUE)
          {
          SLOGCALWAYS1 ("Type Unique Flag = '%s'", 
                                typeDefaultUnique ? "TRUE" : "FALSE");
          }

        if (strchr (&string2[2], 'T') != NULL)
          typeDefaultTransient = SD_TRUE;
        else
          typeDefaultTransient = SD_FALSE;
        if (debug == SD_TRUE)
          {
          SLOGCALWAYS1 ("Type Unique Flag = '%s'", 
                                typeDefaultTransient ? "TRUE" : "FALSE");
          }

        if (strchr (&string2[2], 'B') != NULL)
          typeDefaultBase = SD_TRUE;
        else
          typeDefaultBase = SD_FALSE;
        if (debug == SD_TRUE)
          {
          SLOGCALWAYS1 ("Type Base Flag = '%s'", 
                                  typeDefaultBase ? "TRUE" : "FALSE");
          }

        if (strchr (&string2[2], 'V') != NULL)
          typeDefaultDeleteIfUnused = SD_TRUE;
        else
          typeDefaultDeleteIfUnused = SD_FALSE;
        if (debug == SD_TRUE)
          {
          SLOGCALWAYS1 ("Type Delete Unused Flag = '%s'", 
                        typeDefaultDeleteIfUnused ? "TRUE" : "FALSE");
          }
        if (strchr (&string2[2], 'X') != NULL)
          typeDefaultUca = SD_TRUE;
        else
          typeDefaultUca = SD_FALSE;
        if (debug == SD_TRUE)
          {
          SLOGCALWAYS1 ("Type UCA Flag = '%s'", 
                                typeDefaultUca ? "TRUE" : "FALSE");
          }
        }
      else if (!strcmp (string2, "-notypedefs"))
        {
        if (debug == SD_TRUE)
          {
          SLOGCALWAYS0 ("No Typedefs"); 
          }
        noTypedefs = SD_TRUE;
        }
      else if (!strcmp (string2, "-nosuffix"))
        typeSuffix = "";
      }
    else if (!strcmp (userFlagsIn, ":CU"))        /* Flags                */
      {
      if (!strcmp (string2, "MVL_UCA"))
        {
        mvl_uca = SD_TRUE;
        }
      if (!strcmp (string2, "MVLU_USE_REF"))
        {
        mvlu_use_ref = SD_TRUE;
        }
      if (!strcmp (string2, "MVLU_NO_DUMMY"))
        {
        mvlu_no_dummy = SD_TRUE;
        }

      }
    else
      {
      SLOGCALWAYS1 ("Bad control string: '%s'", userFlagsIn);
      exitApp ();
      }

    if (debug == SD_TRUE)
      {
      SLOGCALWAYS0 ("");
      SLOGCALWAYS1 ("  currFile = '%s'", fileName[fileNestLevel]);
      }
    }
  }

/************************************************************************/
/*                      handleFileName                                  */
/************************************************************************/

static ST_VOID handleFileName ()
  {
ST_CHAR currFile[MAX_STRING_SIZE+1];
FILE_CTRL *fil;

  getQuotedString (currFile, SD_TRUE);
  if (strchr (&userFlagsIn[2], 'S') != NULL) 
    {
    if (!strlen (currFile))
      exitApp ();
    
    fil = findIncluded (currFile);
    if (!fil)
      {
      SLOGCALWAYS0 ("Internal Error!");
      exitApp ();
      }
    
    ++fileNestLevel;
    fileLine[fileNestLevel] = 0;
    fileName[fileNestLevel] = fil->hdrFile;
    }
  else if (strchr (&userFlagsIn[2], 'E') != NULL) 
    {
    --fileNestLevel;
    }
  else
    {
    SLOGCALWAYS0 ("Internal Error 2!");
    exitApp ();
    }

  if (debug == SD_TRUE)
    {
    SLOGCALWAYS0 ("");
    SLOGCALWAYS1 ("  currFile = '%s'", fileName[fileNestLevel]);
    }
  }


/************************************************************************/
/*                      handleInc                                       */
/************************************************************************/

static ST_VOID handleInc ()
  {
ST_CHAR userIncIn[MAX_STRING_SIZE+1];

  if (incCtrl.numInc >= MAX_INC)
    {
    SLOGCALWAYS0 ("Too many include statements");
    exitApp ();
    }

  getQuotedString (userIncIn, SD_TRUE);
  if (debug == SD_TRUE)
    {
    SLOGCALWAYS0 ("");
    SLOGCALWAYS1 ("  userIncIn = '%s'", userIncIn);
    }

  if (!strlen (userIncIn))
    exitApp ();

  incCtrl.incText[incCtrl.numInc] = chk_strdup (userIncIn);
  ++incCtrl.numInc;
  }

/************************************************************************/
/*                      handleType                                      */
/************************************************************************/

static ST_VOID handleType ()
  {
ST_CHAR userDescIn[MAX_STRING_SIZE+1];
ST_CHAR *userTdlIn;
TYPE_CTRL *newType;
ST_UCHAR *asn1_buf;
ST_UCHAR *new_asn1;
ST_UCHAR *asn1_start;
ST_INT asn1_len;
RUNTIME_CTRL *rt_ctrl;
ST_RET ret;
ST_INT i;
ASN1_ENC_CTXT aCtx;

/* TDL buffer is large, calloc as needed rathter than use stack */
  userTdlIn = (ST_CHAR *) chk_calloc (1, MAX_TDL_SIZE+1);

/* Allocate the working ASN.1 type buffer */
  asn1_buf = (ST_UCHAR *) chk_calloc (1, ASN1_SIZE);

/* Label and Flags are already read, read TDL and Desc */
  getQuotedString (userTdlIn, SD_FALSE);
  getQuotedString (userDescIn, SD_TRUE);

  if (debug == SD_TRUE)
    {
    SLOGCALWAYS0 ("");
    SLOGCALWAYS1 ("  userLabelIn = '%s'", userLabelIn);
    SLOGCALWAYS1 ("  userFlagsIn = '%s'", userFlagsIn);
    SLOGCALWAYS1 ("  userTdlIn   = '%s'", userTdlIn);
    SLOGCALWAYS1 ("  userDescIn  = '%s'", userDescIn);
    }

  if (!strlen (userLabelIn) || !strlen (userTdlIn))
    {
    SLOGALWAYS0 ("Fatal:  format error on input");
    exitApp ();
    }

  if (findType (userLabelIn))
    {
    SLOGALWAYS1 ("Fatal:  Duplicate type name '%s'", userLabelIn);
    exitApp ();
    }

  if (createTypeListFile == SD_TRUE)
    {
    fprintf (typeListFp, "%s\t%s\t%s\n", 
				&userFlagsIn[1], userLabelIn, userTdlIn);
    }

  asn1_len = ASN1_SIZE;
  asn1_start = ms_mk_asn1_type (&asn1_len, asn1_buf, userTdlIn);

  if (!asn1_start)
    {
    SLOGALWAYS2 ("Type '%s' : Bad TDL (0x%04x)", 
                        userTdlIn, mms_op_err);
    exitApp ();
    }
  else                /* ASN1 string now in asn1_buf, make run-time.  */
    {
    /* User function "u_ml_get_rt_type" will be called to find referenced types	*/
    rt_ctrl = ms_runtime_create (asn1_start, asn1_len,
                               (UINT_MAX/sizeof(RUNTIME_TYPE))-100);

    if (!rt_ctrl)		/* Create failed	*/
      {
      SLOGALWAYS1 ("Fatal error!  ASN1->RT conversion err, type '%s'",
                                        userLabelIn);
      exitApp ();
      }

    totalRtCount += rt_ctrl->rt_num;
    newType = (TYPE_CTRL *) chk_calloc (1, sizeof (TYPE_CTRL));
    list_add_last ((ST_VOID **)&typeHead, newType);

    newType->tdl = chk_strdup (userTdlIn);
    newType->desc = chk_strdup (userDescIn);
    newType->label = chk_strdup (userLabelIn);
    newType->ucaType = SD_FALSE;

  /* Check for user flags for this type */
    if (typeDefaultUnique || 
        (strlen (userFlagsIn) >= 3 && 
         strchr (&userFlagsIn[2], 'U') != NULL))
      {
      newType->unique = SD_TRUE;
      }
    if (typeDefaultTransient || 
        (strlen (userFlagsIn) >= 3 &&
         strchr (&userFlagsIn[2], 'T') != NULL))
      {
      newType->transient = SD_TRUE;
      }
    if (typeDefaultBase ||   
        (strlen (userFlagsIn) >= 3 &&
         strchr (&userFlagsIn[2], 'B') != NULL))
      {
      newType->base = SD_TRUE;
      }
    if (typeDefaultDeleteIfUnused || 
        (strlen (userFlagsIn) >= 3 && 
         strchr (&userFlagsIn[2], 'V') != NULL))
      {
      newType->deleteIfUnused = SD_TRUE;
      }
    if (strlen (userFlagsIn) >= 3 && 
        strchr (&userFlagsIn[2], 'K') != NULL)
      {
      newType->keep = SD_TRUE;
      }
    if (strlen (userFlagsIn) >= 3 && 
        strchr (&userFlagsIn[2], 'D') != NULL)
      {
      newType->wantTypedef = SD_TRUE;
      }
    if (typeDefaultUca || 
        (strlen (userFlagsIn) >= 3 &&
         strchr (&userFlagsIn[2], 'X') != NULL))
      {
      newType->ucaType = SD_TRUE;
      }
    if (strlen (userFlagsIn) >= 3 && strchr (&userFlagsIn[2], 'C') != NULL)
      {
      newType->rdWrIndFunSel = currRdWrIndFuns;
      }

  /* Now we are deriving the ASN.1 from the runtime, so that it will    */
  /* not contain embedded types                                         */
    asn1r_strt_asn1_bld (&aCtx, asn1_buf, ASN1_SIZE);
    ret = ms_runtime_to_asn1 (&aCtx, rt_ctrl->rt_first, rt_ctrl->rt_num);
    if (ret != SD_SUCCESS)
      {
      SLOGALWAYS0 ("ERROR: Could not create ASN.1 from RUNTIME");
      exitApp ();
      }
    asn1_start = aCtx.asn1r_field_ptr+1;
    asn1_len = (asn1_buf + ASN1_SIZE) - asn1_start;

    new_asn1 = (ST_UCHAR *) chk_malloc (asn1_len);
    memcpy (new_asn1, asn1_start, asn1_len); 
    newType->asn1 = new_asn1;
    newType->asn1_len = asn1_len;

    newType->rt_ctrl = rt_ctrl;
    newType->mvluFoundryInfo = 
	(MVLU_FOUNDRY_TYPE_INFO *) chk_calloc (rt_ctrl->rt_num, 
					   sizeof(MVLU_FOUNDRY_TYPE_INFO));
    
    for (i = 0; i < rt_ctrl->rt_num; ++i)
      {
      newType->mvluFoundryInfo[i].rt = &newType->rt_ctrl->rt_first[i];
      newType->mvluFoundryInfo[i].typeCtrl = newType;
      }
    typeCount++;
    }
  chk_free (asn1_buf);          /* All done with this one */
  chk_free (userTdlIn);
  }

/************************************************************************/
/*                       handleDom                                      */
/************************************************************************/

static ST_VOID handleDom ()
  {
ST_CHAR domName[MAX_STRING_SIZE+1];
DOMAIN_CTRL *domCtrl;
DOMAIN_CTRL *dPrev;
DOMAIN_CTRL *dNext;
ST_INT domIndex;

  getQuotedString (domName, SD_FALSE);
  if (domName[0] == 0)
    {
    SLOGALWAYS0 ("Fatal:  Could not get domain name");
    exitApp ();
    }

  if (debug == SD_TRUE)
    {
    SLOGCALWAYS0 ("");
    SLOGCALWAYS1 ("  domName = '%s'", domName);
    }

  if (findDom (domName, &domIndex) != NULL)
    {
    SLOGALWAYS1 ("Fatal:  Duplicate domain name '%s'",domName);
    exitApp ();
    }

  domCtrl = (DOMAIN_CTRL *) chk_calloc (1, sizeof (DOMAIN_CTRL));
  domCtrl->name = chk_strdup (domName);

/* OK this Domain is ready to be added to the list.                     */
/* Find the insert point.                                               */
  dPrev = NULL;
  dNext = domHead;
  while (dNext != NULL) 
    {                   /* if new name < next name              */
    if (strcmp (domCtrl->name, dNext->name) < 0)
      break;
    dPrev = dNext;
    dNext = (DOMAIN_CTRL *) list_get_next (domHead, dNext);
    }

/*  If 'next' == NULL, add to end of  list. If 'prev' = NULL add to     */
/* head of list. If neither, insert.                                    */

  if ((domHead == NULL) || dPrev == NULL)
    list_add_first ((ST_VOID **) &domHead, domCtrl);
  else 
    list_add_node_after (dPrev, domCtrl);
  ++numDoms;
  }

/************************************************************************/
/*                       handleVar                                      */
/************************************************************************/

static ST_VOID handleVar ()
  {
ST_CHAR domName[MAX_STRING_SIZE+1];
ST_CHAR varName[MAX_STRING_SIZE+1];
ST_CHAR tempVarName[MAX_STRING_SIZE+1];
ST_CHAR typeName[MAX_STRING_SIZE+1];
ST_CHAR initText[MAX_STRING_SIZE+1];
ST_CHAR procText[MAX_STRING_SIZE+1];
ST_CHAR usrText[MAX_STRING_SIZE+1];
ST_CHAR refBuf[100];
DOMAIN_CTRL *domCtrl;
TYPE_CTRL *typeCtrl;
VAR_CTRL *varCtrl;
VAR_CTRL *vPrev;
VAR_CTRL *vNext;
VAR_CTRL **pVarListHead;
ST_CHAR *domP;
ST_CHAR *name;
ST_CHAR *nextName;
ST_BOOLEAN aaScope;
ST_BOOLEAN domScope;
ST_INT domIndex;

  getQuotedString (varName, SD_FALSE);

/* Check to see if it is domain specific */
  aaScope = SD_FALSE;
  domScope = SD_FALSE;
  domP = strchr (varName, ':');
  domName[0] = 0;
  if (domP != NULL)
    {
    *domP = 0;
    if (!strcmp (varName, "AA_SCOPE"))
      aaScope = SD_TRUE;
    else
      {
      domScope = SD_TRUE;
      strcpy (domName, varName);
      domCtrl = findDom (domName, &domIndex);
      if (domCtrl == NULL)
        {
        SLOGALWAYS1 ("Fatal:  Nonexistant domain '%s'", domName);
        exitApp ();
        }
      }
    ++domP;
    strcpy (tempVarName, domP);
    strcpy (varName, tempVarName);
    if (aaScope)
      printf ("\nWARNING: AA_SCOPE variable '%s' not supported unless output file compiled with 'OBSOLETE_AA_OBJ_INIT' defined.",
              varName);
    }

  getQuotedString (typeName, SD_FALSE);

  if (varDefaultData || strchr (&userFlagsIn[2], 'D') != NULL)
    getQuotedString (initText, SD_TRUE);
  else
    initText[0] = 0;

  if (varDefaultData || strchr (&userFlagsIn[2], 'P') != NULL)
    getQuotedString (procText, SD_TRUE);
  else
    procText[0] = 0;

  if (strchr (&userFlagsIn[2], 'U') != NULL)
    getQuotedString (usrText, SD_TRUE);
  else
    usrText[0] = 0;

  if (varName[0] == 0)
    {
    SLOGALWAYS0 ("Fatal:  Could not get var name");
    exitApp ();
    }

  if (debug == SD_TRUE)
    {
    SLOGCALWAYS0 ("");
    SLOGCALWAYS1 ("  varName = '%s'", varName);
    SLOGCALWAYS1 ("  domName = '%s'", domName);
    SLOGCALWAYS1 ("  typeName = '%s'", typeName);
    SLOGCALWAYS1 ("  initText = '%s'", initText);
    SLOGCALWAYS1 ("  procText = '%s'", procText);
    }

  if (domScope == SD_TRUE)
    {
    ++domCtrl->numVars;
    pVarListHead = &domCtrl->varHead;
    }
  else if (aaScope == SD_TRUE)
    {
    ++aaObjs.numVars;
    pVarListHead = &aaObjs.varHead;
    }
  else
    {
    ++numVmdVars;
    pVarListHead = &vmdVarHead;
    }

  if (findVar (varName, *pVarListHead) != NULL)
    {
    SLOGALWAYS1 ("Fatal:  Duplicate var name '%s'", varName);
    exitApp ();
    }

  typeCtrl = findType (typeName);
  if (typeCtrl == NULL)
    {
    SLOGALWAYS1 ("Could not find type name '%s'", typeName);
    exitApp ();
    }
  if (!strncmp (varName, "_UCA_", 5))
    typeCtrl->ucaType = SD_TRUE;

  varCtrl = (VAR_CTRL *) chk_calloc (1, sizeof (VAR_CTRL));
  varCtrl->name = chk_strdup (varName);
  varCtrl->typeCtrl = typeCtrl;

  if (initText[0] != 0)
    varCtrl->initText = chk_strdup (initText);

  if (procText[0] != 0)
    varCtrl->procText = chk_strdup (procText);

  if (usrText[0] != 0)
    varCtrl->usrText = chk_strdup (usrText);

  if (domScope == SD_TRUE)
    {
    sprintf (refBuf, "mvl_vmd.dom_tbl[%d]", domIndex);
    varCtrl->domRef = chk_strdup (refBuf);
    varCtrl->scope = DOM_SPEC;
    }
  else if (aaScope == SD_TRUE)
    varCtrl->scope = AA_SPEC;
  else
    varCtrl->scope = VMD_SPEC;


/* OK this variable is ready to be added to the list.                   */
/* Find the insert point.                                               */
  vPrev = NULL;
  vNext = *pVarListHead;
  if (!strncmp (varCtrl->name, "_UCA_", 5))
    name = &varCtrl->name[5];
  else
    name = varCtrl->name;
  while (vNext != NULL) 
    {                   /* if new name < next name              */
    if (!strncmp (vNext->name, "_UCA_", 5))
      nextName = &vNext->name[5];
    else
      nextName = vNext->name;

    if (strcmp (name, nextName) < 0)
      break;
    vPrev = vNext;
    vNext = (VAR_CTRL *) list_get_next (*pVarListHead, vNext);
    }

/*  If 'next' == NULL, add to end of  list. If 'prev' = NULL add to     */
/* head of list. If neither, insert.                                    */

  if ((*pVarListHead == NULL) || vPrev == NULL)
    list_add_first ((ST_VOID **) pVarListHead, varCtrl);
  else 
    list_add_node_after (vPrev, varCtrl);

  ++typeCtrl->varRefCount;
  }

/************************************************************************/
/*                       handleVarList                                  */
/************************************************************************/

static ST_VOID handleVarList ()
  {
ST_CHAR domName[MAX_STRING_SIZE+1];
ST_CHAR varListName[MAX_STRING_SIZE+1];
ST_CHAR varName[MAX_STRING_SIZE+1];
ST_CHAR tempVarListName[MAX_STRING_SIZE+1];
ST_CHAR tempVarName[MAX_STRING_SIZE+1];
ST_CHAR refBuf[100];
DOMAIN_CTRL *domCtrl;
ST_INT domIndex;
VARLIST_CTRL *varListCtrl;
VARLIST_CTRL *vPrev;
VARLIST_CTRL *vNext;
VARLIST_CTRL **pVarListListHead;
VAR_CTRL **pVarListHead;
ST_CHAR *domP;
ST_CHAR *name;
ST_CHAR *nextName;
VAR_CTRL *var;
ST_INT numVars;
ST_BOOLEAN aaScope;
ST_BOOLEAN domScope;

  getQuotedString (varListName, SD_FALSE);

/* Check to see if it is domain specific */
  aaScope = SD_FALSE;
  domScope = SD_FALSE;
  domP = strchr (varListName, ':');
  if (domP != NULL)
    {
    *domP = 0;
    if (!strcmp (varListName, "AA_SCOPE"))
      aaScope = SD_TRUE;
    else
      {
      domScope = SD_TRUE;
      strcpy (domName, varListName);
      domCtrl = findDom (domName, &domIndex);
      if (domCtrl == NULL)
        {
        SLOGALWAYS1 ("Fatal:  Nonexistant domain '%s'", domName);
        exitApp ();
        }
      }
    ++domP;
    strcpy (tempVarListName, domP);
    strcpy (varListName, tempVarListName);
    if (aaScope)
      printf ("\nWARNING: AA_SCOPE NamedVariableList '%s' not supported unless output file compiled with 'OBSOLETE_AA_OBJ_INIT' defined.",
              varListName);
    }
  else
    {
    domName[0] = 0;
    domCtrl = NULL;
    }

  if (varListName[0] == 0)
    {
    SLOGALWAYS0 ("Fatal:  Could not get varList name");
    exitApp ();
    }

  if (debug == SD_TRUE)
    {
    SLOGCALWAYS1 ("  varListName = '%s'", varListName);
    SLOGCALWAYS1 ("  domName = '%s'", domName);
    }

  if (domScope == SD_TRUE)
    {
    ++domCtrl->numVarLists;
    pVarListListHead = &domCtrl->varListHead;
    }
  else if (aaScope == SD_TRUE)
    {
    ++aaObjs.numVarLists;
    pVarListListHead = &aaObjs.varListHead;
    }
  else
    {
    ++numVmdVarLists;
    pVarListListHead = &vmdVarListHead;
    }

  if (findVarList (varListName, *pVarListListHead) != NULL)
    {
    SLOGALWAYS1 ("Duplicate varList name '%s", varListName);
    exitApp ();
    }

  varListCtrl = (VARLIST_CTRL *) chk_calloc (1, sizeof (VARLIST_CTRL));
  varListCtrl->name = chk_strdup (varListName);
  varListCtrl->varTable = (VAR_CTRL **) chk_calloc (MAX_VARS_PER_VARLIST, 
                                      sizeof (VAR_CTRL *));

/* OK this variable list is ready to be added to the list.              */
/* Find the insert point.                                               */
  vPrev = NULL;
  vNext = *pVarListListHead;
  name = varListCtrl->name;
  while (vNext != NULL) 
    {                   /* if new name < next name              */
    nextName = vNext->name;

    if (strcmp (name, nextName) < 0)
      break;
    vPrev = vNext;
    vNext = (VARLIST_CTRL *) list_get_next (*pVarListListHead, vNext);
    }

/*  If 'next' == NULL, add to end of  list. If 'prev' = NULL add to     */
/* head of list. If neither, insert.                                    */

  if ((*pVarListListHead == NULL) || vPrev == NULL)
    list_add_first ((ST_VOID **) pVarListListHead, varListCtrl);
  else 
    list_add_node_after (vPrev, varListCtrl);

/* OK, now get the variables in the list */
  numVars = 0;
  while (SD_TRUE)
    {
    getQuotedString (varName, SD_FALSE);
    if (!strcmp (varName, ":S"))
      break;

    aaScope = SD_FALSE;
    domScope = SD_FALSE;
    domP = strchr (varName, ':');
    if (domP != NULL)
      {
      *domP = 0;
      if (!strcmp (varName, "AA_SCOPE"))
        aaScope = SD_TRUE;
      else
        {
        domScope = SD_TRUE;
        strcpy (domName, varName);
        domCtrl = findDom (domName, &domIndex);
        if (domCtrl == NULL)
          {
          SLOGALWAYS1 ("Fatal:  Nonexistant domain '%s'", domName);
          exitApp ();
          }
        }
      ++domP;
      strcpy (tempVarName, domP);
      strcpy (varName, tempVarName);
      if (aaScope)
        printf ("\nWARNING: AA_SCOPE variable '%s' in NamedVariableList not supported unless output file compiled with 'OBSOLETE_AA_OBJ_INIT' defined.",
                varName);
      }

    if (domScope == SD_TRUE)
      pVarListHead = &domCtrl->varHead;
    else if (aaScope == SD_TRUE)
      pVarListHead = &aaObjs.varHead;
    else
      pVarListHead = &vmdVarHead;

    var = findVar (varName, *pVarListHead);
    if (var == NULL)
      {
      if (mvl_uca == SD_TRUE)
        {
        var = (VAR_CTRL *) chk_calloc (1, sizeof (VAR_CTRL));
        var->name = chk_strdup (varName);

        if (domScope == SD_TRUE)
          {
          sprintf (refBuf, "mvl_vmd.dom_tbl[%d]", domIndex);
          var->domRef = chk_strdup (refBuf);
          var->scope = DOM_SPEC;
          }
        else if (aaScope == SD_TRUE)
          var->scope = AA_SPEC;
        else 
          var->scope = VMD_SPEC;

        var->nvlOnly = SD_TRUE;
        }
      else
        {
        SLOGALWAYS2 ("VarList '%s': could not find var name '%s'", 
					varListName, varName);
        exitApp ();
        }
      }

    if (numVars >= MAX_VARS_PER_VARLIST)
      {
      SLOGALWAYS1 ("Too many variables for variable list '%s", varListName);
      exitApp ();
      }

    varListCtrl->varTable[numVars] = var;
    ++numVars;
    }

  varListCtrl->numVars = numVars;
  varListCtrl->varTable = (VAR_CTRL **) chk_realloc (varListCtrl->varTable, 
                                       numVars * sizeof (VAR_CTRL *));
  }

/************************************************************************/
/*                       handleJou                                      */
/************************************************************************/

static ST_VOID handleJou ()
  {
ST_CHAR jouNameBuf[2*MAX_STRING_SIZE+1];/* "jouName" or "domainName:jouName"	*/
ST_INT	  jouType;
ST_CHAR  *jouName;
ST_CHAR  *domName;

  getQuotedString (jouNameBuf, SD_FALSE);
  if (debug == SD_TRUE)
    {
    SLOGCALWAYS0 ("");
    SLOGCALWAYS1 ("  jouName = '%s'", jouNameBuf);
    }

  jouName = strchr (jouNameBuf, ':');
  if (jouName != NULL)
    {
    jouType = DOM_SPEC;
    *jouName = 0;			/* separate domain from journal	*/
    ++jouName;				/* points to journal name str	*/
    domName = jouNameBuf;		/* points to domain name string	*/
    ++dnumJou;
    }
  else
    {
    jouType = VMD_SPEC;
    jouName = jouNameBuf;
    domName = NULL;
    ++numJou;
    }

  if (findJou (jouName, domName, jouType) != NULL)
    {
    SLOGALWAYS1 ("Fatal:  Duplicate Journal name '%s'", jouName);
    exitApp ();
    }

  if (jouType == DOM_SPEC)
    {
    DOM_JOU_CTRL *jouCtrl;
    DOM_JOU_CTRL *jPrev;
    DOM_JOU_CTRL *jNext;

    jouCtrl = (DOM_JOU_CTRL *) chk_calloc (1, sizeof (DOM_JOU_CTRL));
    jouCtrl->name = chk_strdup (jouName);
    jouCtrl->domName = chk_strdup (domName);
    jouCtrl->processed = SD_FALSE;

    /* OK this journal is ready to be added to the list.                   */
    /* Find the insert point.                                               */
    jPrev = NULL;
    jNext = djouHead;
    while (jNext != NULL) 
      {                   /* if new name < next name              */
      if (strcmp (jouCtrl->domName, jNext->domName) <= 0 &&
          strcmp (jouCtrl->name, jNext->name) < 0)
        break;
      jPrev = jNext;
      jNext = (DOM_JOU_CTRL *) list_get_next (djouHead, jNext);
      }

    /* If 'next' == NULL, add to end of  list. If 'prev' = NULL add to     */
    /* head of list. If neither, insert.                                    */

    if ((djouHead == NULL) || jPrev == NULL)
      list_add_first ((ST_VOID **) &djouHead, jouCtrl);
    else 
      list_add_node_after (jPrev, jouCtrl);
    }
  else
    {
    JOU_CTRL *jouCtrl;
    JOU_CTRL *jPrev;
    JOU_CTRL *jNext;

    /* VMD_SPEC journals */
    jouCtrl = (JOU_CTRL *) chk_calloc (1, sizeof (JOU_CTRL));
    jouCtrl->name = chk_strdup (jouName);

    /* OK this journal is ready to be added to the list.                   */
    /* Find the insert point.                                               */
    jPrev = NULL;
    jNext = jouHead;
    while (jNext != NULL) 
      {                   /* if new name < next name              */
      if (strcmp (jouCtrl->name, jNext->name) < 0)
        break;
      jPrev = jNext;
      jNext = (JOU_CTRL *) list_get_next (jouHead, jNext);
      }

    /* If 'next' == NULL, add to end of  list. If 'prev' = NULL add to     */
    /* head of list. If neither, insert.                                    */

    if ((jouHead == NULL) || jPrev == NULL)
      list_add_first ((ST_VOID **) &jouHead, jouCtrl);
    else 
      list_add_node_after (jPrev, jouCtrl);
    }
  }

/************************************************************************/
/*                       handleNameExtract                              */
/************************************************************************/

static ST_VOID handleNameExtract ()
  {
ST_CHAR baseName[MAX_STRING_SIZE+1];
ST_CHAR typeName[MAX_STRING_SIZE+1];
TYPE_CTRL *typeCtrl;
NAME_CTRL *nameCtrl;

  getQuotedString (baseName, SD_FALSE);
  getQuotedString (typeName, SD_FALSE);

  if (debug == SD_TRUE)
    {
    SLOGCALWAYS0 ("");
    SLOGCALWAYS1 ("  baseName = '%s'", baseName);
    SLOGCALWAYS1 ("  typeName = '%s'", typeName);
    }

  ++numNames;
  typeCtrl = findType (typeName);
  if (typeCtrl == NULL)
    {
    SLOGALWAYS1 ("Could not find type name '%s'", typeName);
    exitApp ();
    }

  nameCtrl = (NAME_CTRL *) chk_calloc (1, sizeof (NAME_CTRL));
  nameCtrl->base = chk_strdup (baseName);
  nameCtrl->typeCtrl = typeCtrl;
  list_add_last ((ST_VOID **)&nameListHead, nameCtrl);
  }

/************************************************************************/
/************************************************************************/
/* RUNTIME TYPE CREATION                                                */
/************************************************************************/
/*			u_ml_get_rt_type				*/
/************************************************************************/
/* mms_tdef.c will call this function to find a referenced type.        */

ST_RET u_ml_get_rt_type (ASN1_DEC_CTXT *aCtx, OBJECT_NAME *type_name,  RUNTIME_TYPE **rt_out, 
                                              ST_INT *num_rt_out)
  {
TYPE_CTRL *current_type;
ASN1_TO_RT_DEC_INFO *decInfo = (ASN1_TO_RT_DEC_INFO *) aCtx->usr_info[0];

  if (debug == SD_TRUE)
    {
    SLOGCALWAYS1 ("Inserting nested type '%s'", type_name->obj_name.vmd_spec);
    }

  current_type = typeHead;
  while (current_type != NULL)
    {
    if (!strcmp (current_type->label, type_name->obj_name.vmd_spec))
      {
      *rt_out = current_type->rt_ctrl->rt_first;
      *num_rt_out = current_type->rt_ctrl->rt_num;

      if (decInfo->calc_only == SD_FALSE)
        {
        ++current_type->baseRefCount;
        if (debug == SD_TRUE)
          {
          SLOGCALWAYS1 ("  References    '%s'",current_type->label);
          }
        }
      return (SD_SUCCESS);
      }
    current_type = (TYPE_CTRL *) list_get_next (typeHead, current_type);
    }
  SLOGALWAYS1 ("Could not find type '%s'",type_name->obj_name.vmd_spec);
  return (SD_FAILURE);
  }

/************************************************************************/
/************************************************************************/
/* OBJECT FIND FUNCTIONS						*/
/************************************************************************/
/*                       findType                                       */
/************************************************************************/

static TYPE_CTRL *findType (ST_CHAR *typeName)
  {
TYPE_CTRL *type;

  type = typeHead;
  while (type != NULL) 
    {                   /* if new name < next name              */
    if (!strcmp (type->label, typeName))
      return (type);
    type = (TYPE_CTRL *) list_get_next (typeHead, type);
    }
  return (NULL);
  }

/************************************************************************/
/*                       findDom                                        */
/************************************************************************/

static DOMAIN_CTRL *findDom (ST_CHAR *domName, ST_INT *domIndexOut)
  {
DOMAIN_CTRL *dom;
ST_INT domIndex;

  domIndex = 0;
  dom = domHead;
  while (dom != NULL) 
    {                   /* if new name < next name              */
    if (!strcmp (dom->name, domName))
      {
      *domIndexOut = domIndex;
      return (dom);
      }
    ++domIndex;
    dom = (DOMAIN_CTRL *) list_get_next (domHead, dom);
    }
  return (NULL);
  }

/************************************************************************/
/*                       findVar                                        */
/************************************************************************/

static VAR_CTRL *findVar (ST_CHAR *varName, VAR_CTRL *listHead)
  {
VAR_CTRL *var;

  var = listHead;
  while (var != NULL) 
    {                   /* if new name < next name              */
    if (!strcmp (var->name, varName))
      return (var);
    var = (VAR_CTRL *) list_get_next (listHead, var);
    }
  return (NULL);
  }

/************************************************************************/
/*                       findVarList                                    */
/************************************************************************/

static VARLIST_CTRL *findVarList (ST_CHAR *varListName, 
				  VARLIST_CTRL *listHead)
  {
VARLIST_CTRL *varList;

  varList = listHead;
  while (varList != NULL) 
    {                   /* if new name < next name              */
    if (!strcmp (varList->name, varListName))
      return (varList);
    varList = (VARLIST_CTRL *) list_get_next (listHead, varList);
    }
  return (NULL);
  }

/************************************************************************/
/*                       findJou                                        */
/************************************************************************/

static ST_VOID *findJou (ST_CHAR *jouName, ST_CHAR *domName, ST_INT jouType)
  {
JOU_CTRL *jou;
DOM_JOU_CTRL *djou;

  if (jouType == DOM_SPEC)
    {
    /* use DOM_SPEC journal list */
    djou = djouHead;
    while (djou != NULL) 
      {   
      if (strcmp (djou->name, jouName) == 0 && strcmp (djou->domName, domName) == 0)
        return (djou);
      djou = (DOM_JOU_CTRL *) list_get_next (djouHead, djou);
      }
    }
  else
    {
    /* use VMD_SPEC journal list */
    jou = jouHead;
    while (jou != NULL) 
      {   
      if (!strcmp (jou->name, jouName))
        return (jou);
      jou = (JOU_CTRL *) list_get_next (jouHead, jou);
      }
    }
  return (NULL);
  }

/************************************************************************/
/************************************************************************/
/* MISC FUNCTIONS							*/
/************************************************************************/
/*                       getQuotedString                                */
/************************************************************************/

static ST_VOID getQuotedString (ST_CHAR *dest, ST_BOOLEAN ws_allowed_in_string)
  {
ST_INT i,j;

/* Find opening " for the string */
  while (SD_TRUE)
    {
    i = getc (sourceFp);
    if (i == '"')
      break;
    if (i == EOF)
      return;
    if (i == '\n')
      incLineCount ();
    if (i == '#')               /* Comment line, skip to EOL            */
      {
      while ((i = getc (sourceFp)) != '\n' && (i != EOF))
        ;
      if (i == '\n')
        incLineCount ();
      }
    }

/* Now get characters until we find the closing " */
  j = 0;
  while (SD_TRUE)
    {
    i = getc (sourceFp);
    if (i == '"')       /* End of string */
      break;
    if (i == EOF)       /* End of file */
      break;
    if (i == '\n')      /* End of line */
      {
      incLineCount ();
      continue;
      }
    if (i == '#')               /* Comment line, skip to EOL            */
      {
      while ((i = getc (sourceFp)) != '\n' && (i != EOF))
        ;
      if (i == '\n')
        incLineCount ();
      continue;
      }

  /* Check for whitespace and handle it */
    if (ws_allowed_in_string == SD_FALSE && (i == ' ' || i == '\t'))
      continue;

  /* OK, we like this character, accept it */
    dest[j] = i;
    ++j;
    }
  dest[j] = 0;
  }

/************************************************************************/
/*				incLineCount				*/
/************************************************************************/

static ST_VOID incLineCount ()
  {
  ++fileLine[fileNestLevel];
  ++lineCount;
  if (verbose == SD_TRUE)
    printf ("\rProcessing Line:% 6d", lineCount);
  }

/************************************************************************/
/************************************************************************/

/************************************************************************/

ST_VOID loadDefaultFunSet (ST_CHAR *funFileName)
  {
FILE *fp;
ST_CHAR lineBuf[255];
ST_INT i;
ST_INT c;
ST_CHAR pathBuf[256];
ST_CHAR lastChar;

  if (strlen (funFileName) == 0)
    {
    currRdWrIndFuns = NULL;
    return;
    }

  fp = fopen (funFileName, "r");
  if (fp == NULL)
    {
    for (i = 0; i < pathCtrl.numPath && !fp; ++i)
      {
      strcpy (pathBuf, pathCtrl.pathText[i]);
      lastChar = pathBuf[strlen(pathBuf)-1];
      if (lastChar != '/' && lastChar != '\\')
        strcat (pathBuf, "/");
      strcat (pathBuf, funFileName);
      SLOGCALWAYS1 ("Trying to open '%s'", pathBuf);
      fp = fopen (pathBuf, "r");
      }

    if (fp == NULL)
      {
      SLOGALWAYS1 ("Error, cannot open input file '%s'", funFileName);
      exitApp ();
      }
    }

  currRdWrIndFuns = (FO_READ_WRITE_IND *) chk_calloc (1, sizeof (FO_READ_WRITE_IND));
  for (i = 3; i < 15; ++i)
    {
    if (fgets (lineBuf, MAX_CFG_LINE, fp) == NULL)
      {
      SLOGALWAYS1 ("Error reading default function file %s", funFileName);
      exitApp ();
      }
    c = sscanf (lineBuf, "%s%s", currRdWrIndFuns->x[i].rd, 
				 currRdWrIndFuns->x[i].wr);	
    if (c != 2)
      {
      SLOGALWAYS1 ("Error reading default function file %s", funFileName);
      exitApp ();
      }
    }
  fclose (fp);
  }

