/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,       */
/*               1997 - 2004, All Rights Reserved                       */
/*                                                                      */
/* MODULE NAME : fo_main.c                                              */
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
/* 07/23/08  GLB     23    Update Copyright.                            */
/* 01/30/06  GLB     22    Integrated porting changes for VMS           */
/* 02/21/05  JRB     21    Add m_struct_* globals (removed from mms_tdef)*/
/*			   Chg exit to exitApp.				*/
/* 11/17/04  JRB     20    Use new slog_max_msg_size_set.		*/
/* 11/17/04  JRB     19    Exit if align file not found.		*/
/* 02/09/04  JRB     18    Update Copyright.				*/
/* 01/21/04  JRB     17    Call new logUnusedLeafAccessInfo funct.	*/
/* 12/11/03  MDE     16    Added -i option				*/
/* 09/22/03  JRB     15    Del leafTblFileName global.			*/
/* 04/14/03  JRB     14    Eliminate compiler warnings.			*/
/* 01/06/03  MDE     13    Put leaf_tbl.c and foundry.log in output dir	*/
/* 12/16/02  JRB     12    Del C++ style comments			*/
/* 11/27/02  MDE     11    Support for CSV, XML, etc..			*/
/* 01/24/02  EJV     10    Added support for DOM_SPEC journals.		*/
/*			    Changed copyright year in welcome().	*/
/* 12/21/00  MDE     09    Added 'noMmsLite' support			*/
/* 06/21/00  MDE     08    Now can emit type list w/TDL (-l)		*/
/* 04/26/00  RKR     07    Added caseSensStrIdxs			*/
/* 06/04/99  MDE     06    Removed the 'nocache' option; no longer need	*/
/* 05/12/99  MDE     05    Open all files in text mode			*/
/* 09/21/98  MDE     04    Minor lint cleanup				*/
/* 09/16/98  MDE     03    Now create Journals too			*/
/* 09/10/98  MDE     02    Minor fixes for handling UCA/non-UCA types	*/
/* 07/21/98  MDE     01    New module, extracted from foundry.c		*/
/************************************************************************/

#include "foundry.h"
#include "arg.h"

/************************************************************************/
/************************************************************************/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/
/* GLOBAL VARIABLES							*/
/************************************************************************/
/* For LITE, Foundry must set these. Values written to output 'c' file.	*/
ST_INT m_struct_start_algn_mode=-1;	/* illegal value. Foundry must set it*/
ST_INT m_struct_end_algn_mode=-1;	/* illegal value. Foundry must set it*/

/************************************************************************/
/* EXTRACTED OBJECT DEFINITION CONTROL */

/* User Specified Header Files */
INC_CTRL incCtrl;

/* Types */
ST_INT typeCount;
ST_INT totalRtCount;
TYPE_CTRL *typeHead;
TYPE_CTRL *typedefOnlyHead;

/* VMD Variables */
ST_INT numVmdVars;
VAR_CTRL *vmdVarHead;

/* VMD Variables Lists */
ST_INT numVmdVarLists;
VARLIST_CTRL *vmdVarListHead;

/* Domains */
ST_INT numDoms;
DOMAIN_CTRL *domHead;

/* AA Scope */
AA_CTRL aaObjs;

/* Journals */
ST_INT numJou;				/* num of VMD_SPEC journals	*/
JOU_CTRL *jouHead;
ST_INT dnumJou;				/* num of DOM_SPEC journals	*/
DOM_JOU_CTRL *djouHead;

/* UCA Name Generation */
ST_INT numNames;
NAME_CTRL *nameListHead;

/* Common String storage */
ST_CHAR *mvl_rt_names[MAX_COMMON_STRINGS];
ST_INT num_mvl_rt_names;

/* Input and Output File Names */
ST_CHAR cfgFile[_MAX_FILE_NAME+1] = "align.cfg";
ST_CHAR hFileName[_MAX_FILE_NAME+1];
ST_CHAR inFileName[_MAX_FILE_NAME+1];
ST_CHAR cFileName[_MAX_FILE_NAME+1];
ST_CHAR xmlFileName[_MAX_FILE_NAME+1];
ST_CHAR newFileName[_MAX_FILE_NAME+1] = "lefttodo.txt";
ST_CHAR templateInFileName[_MAX_FILE_NAME+1];
ST_CHAR logFileName[_MAX_FILE_NAME+1];

static ST_CHAR outFileName[_MAX_FILE_NAME+1];

/* File handles */
FILE *sourceFp;
FILE *templateFp;
FILE *headerFp;
FILE *typeFunFp;
FILE *codeFp;
FILE *newFp;
FILE *typeListFp;

/* Flags, misc. */
ST_BOOLEAN fileOverwrite;
ST_BOOLEAN extractNames;
ST_BOOLEAN verbose;
ST_BOOLEAN createVa;
ST_BOOLEAN debug;
ST_BOOLEAN useTemplate;
ST_BOOLEAN mvlu_use_leaf_file;
ST_BOOLEAN mvl_uca;
ST_BOOLEAN mvlu_use_ref;
ST_BOOLEAN useRtInitCode;
ST_BOOLEAN caseSensStrIdxs;
ST_BOOLEAN noMmsLite;
ST_BOOLEAN mvlu_no_dummy;

ST_BOOLEAN createTypeListFile;

ST_BOOLEAN ucaType;

ST_CHAR *typeSuffix = "_TYPEID";

ST_BOOLEAN noTypedefs;

ST_CHAR *commentBar = 
  "/************************************************************************/";

/* Processing Control */

ST_INT lineCount;
#define MAX_FILE_NESTING     100
ST_INT fileNestLevel;
ST_CHAR *fileName[MAX_FILE_NESTING];
ST_INT fileLine[MAX_FILE_NESTING];

time_t rightnow;

/* Our very own data alignment table */
ST_INT data_algn_tbl[NUM_ALGN_TYPES];

FO_READ_WRITE_IND *currRdWrIndFuns;

/************************************************************************/
/* STATIC FUNCTIONS							*/

static ST_VOID welcome (ST_VOID);
static ST_VOID initLogCfg (ST_VOID);
static ST_VOID init_mem (ST_VOID);
static ST_VOID setOverwrite (ST_INT argNum, ST_CHAR *arg);
static ST_VOID getAlignFile (ST_INT argNum, ST_CHAR *arg);
static ST_VOID getTfnFile (ST_INT argNum, ST_CHAR *arg);
static ST_VOID getOdfFile (ST_INT argNum, ST_CHAR *arg);
static ST_VOID setUcaVa (ST_INT argNum, ST_CHAR *arg);
static ST_VOID setUcaName (ST_INT argNum, ST_CHAR *arg);
static ST_VOID setDebug (ST_INT argNum, ST_CHAR *arg);
static ST_VOID setRtInit (ST_INT argNum, ST_CHAR *arg);
static ST_VOID setTypeList (ST_INT argNum, ST_CHAR *arg);
static ST_VOID setNoMms (ST_INT argNum, ST_CHAR *arg);
static ST_VOID setPrint (ST_INT argNum, ST_CHAR *arg);
static ST_VOID printHelp (ST_INT argNum, ST_CHAR *arg);
static ST_VOID getIncludePath (ST_INT argNum, ST_CHAR *arg);

/************************************************************************/

struct arg_ctrl argCtrl[] =
  {
    {"-?", "List Valid Arguments", printHelp},
    {"-c", "Alignment control file", getAlignFile},
    {"-t", "UCA Template Function File", getTfnFile},
    {"-o", "Overwrite target", setOverwrite},
    {"-d", "Debug mode", setDebug},
    {"-v", "Create UCA Variable Names & Associations", setUcaVa},
    {"-n", "Extract UCA Variable Names", setUcaName},
    {"-p", "Print line numbers being processed", setPrint},
    {"-r", "Use code to initialize Runtime Type", setRtInit},
    {"-l", "List all types", setTypeList},
    {"-i", "Include path", getIncludePath},
    {"-x", "No MMS-Lite Required", setNoMms},
    {"*", "Object Definition File Name", getOdfFile}
  };
ST_INT numArgStrings = sizeof(argCtrl)/sizeof(struct arg_ctrl);


/************************************************************************/
/*                       main                                           */
/************************************************************************/

int main (ST_INT argc, ST_CHAR **argv)
  {
ST_INT j;

/* WARNING: Do not log until after initLogCfg call */

  welcome ();
  init_mem ();          /* Set up memory allocation tools               */

#if defined(NO_GLB_VAR_INIT)
  m_init_glb_vars ();
#endif

  if (process_args (argc, argv) != SD_SUCCESS)
    exit (1);

/*  Figure out the output C and H file names				*/
/* We will look for the extension and wipe it out then append .c, .h	*/
  strcpy (cFileName, outFileName);
  j = strlen (cFileName) - 1;
  while (j >= 0 && cFileName[j] != '.')
    --j;
  if (j < 0)            /* No '.' (and so no extention) in output name  */
    j = strlen (cFileName);
  else
    {                   /* Check for relative path wo/extension         */
    if (j < (ST_INT)(strlen(cFileName) - 2) && 
        (cFileName[j+1] == '\\' || cFileName[j+1] == '/'))
      {
      j = strlen (cFileName);
      }
    }
  cFileName[j] = 0;
  strcpy (hFileName, cFileName);
  strcpy (xmlFileName, cFileName);
  strcat (cFileName, ".c");
  strcat (hFileName, ".h");

  j = strlen (xmlFileName) - 1;
  while (j >= 0 && 
         xmlFileName[j] != '\\' && 
         xmlFileName[j] != '/' && 
         xmlFileName[j] != ':')
    {
    --j;
    }
  if (j < 0)            /* Path in output name  */
    xmlFileName[0] = 0;		/* Nope */
  else
    xmlFileName[j+1] = 0; 	/* Yep */
  strcpy (newFileName, xmlFileName);
  strcpy (logFileName, xmlFileName);
  strcat (xmlFileName, "lap_out.xml");
  strcat (newFileName, "lefttodo.txt");
  strcat (logFileName, "foundry.log");

  initLogCfg ();
  SLOGALWAYS1 ("Executing '%s'", argv[0]);

  SLOGCALWAYS1 ("Input TDL file         : '%s'", inFileName);
  SLOGCALWAYS1 ("Configuration file     : '%s'", cfgFile);
  SLOGCALWAYS1 ("Output 'C' file        : '%s'", cFileName);
  SLOGCALWAYS1 ("Output 'H' file        : '%s'", hFileName);
  SLOGCALWAYS1 ("Output 'LAP XML' file  : '%s'", xmlFileName);
  SLOGCALWAYS1 ("Output 'Lefttodo' file : '%s'", newFileName);

  rightnow = time (NULL);
  processSource ();
  optimizeTypes ();
  if (mvl_uca == SD_TRUE)
    deriveUcaNames ();
  writeOutput ();
  logUnusedLeafAccessInfo ();	/* log unused info in XML input file	*/
  if (debug == SD_TRUE)
    dyn_mem_ptr_status ();      /* Log memory allocation usage          */
  printf ("\nDone\n");
  return (0);
  }

/************************************************************************/
/************************************************************************/
/* MISC. FUNCTIONS                                                      */
/************************************************************************/
/************************************************************************/
/*                       readAlgnTable                          	*/
/************************************************************************/
/* We want to allow 'natural' access to data, rather than packed        */
/* This needs to be done prior to creating runtime types.               */

ST_VOID readAlgnTable ()
  {
ST_CHAR lineBuf[MAX_CFG_LINE+1];
ST_INT algnCount;
ST_INT line;
ST_INT numScan;
ST_CHAR *valStart;
FILE *fp;
ST_BOOLEAN algnTblDone;
ST_BOOLEAN strStartModeDone;
ST_BOOLEAN strEndModeDone;

  fp = fopen (cfgFile, "r");

  if (fp == NULL)
    {
    printf ("\n  ERROR: alignment file '%s' not found. Quitting now.\n", cfgFile);
    SLOGALWAYS1 ("ERROR: alignment file '%s' not found. Quitting now.", cfgFile);
    exitApp ();
    }

  algnTblDone = SD_FALSE;
  strStartModeDone = SD_FALSE;
  strEndModeDone = SD_FALSE;

  algnCount = 0;
  line = 0;
  while (SD_TRUE)
    {
    ++line;
    if (fgets (lineBuf, MAX_CFG_LINE, fp) == NULL)
      {
      if (algnTblDone == SD_FALSE)
        {
        SLOGALWAYS0 ("Alignment Control File error, not enough alignment entries");
        exitApp ();
        }
      if (strStartModeDone == SD_FALSE)
        {
        SLOGALWAYS0 ("Alignment Control File error, no structure start entry");
        exitApp ();
        }
      if (strEndModeDone == SD_FALSE)
        {
        SLOGALWAYS0 ("Alignment Control File error, no structure end entry");
        exitApp ();
        }
      break;	/* we are done, OK */
      }

    if (strstr (lineBuf, "M_STRSTART_MODE"))
      {
      if (strstr (lineBuf, "M_STRSTART_MODE_LARGEST"))
	m_struct_start_algn_mode = M_STRSTART_MODE_LARGEST;
      else if (strstr (lineBuf, "M_STRSTART_MODE_FIRST"))
	m_struct_start_algn_mode = M_STRSTART_MODE_FIRST;
      else if (strstr (lineBuf, "M_STRSTART_MODE_NONE"))
	m_struct_start_algn_mode = M_STRSTART_MODE_NONE;
      else
        {
        SLOGALWAYS0 ("Structure start alignment error");
        exitApp ();
        }
      strStartModeDone = SD_TRUE;
      continue;
      }

    if (strstr (lineBuf, "M_STREND_MODE"))
      {
      if (strstr (lineBuf, "M_STREND_MODE_LARGEST"))
	m_struct_end_algn_mode = M_STREND_MODE_LARGEST;
      else if (strstr (lineBuf, "M_STREND_MODE_NONE"))
	m_struct_end_algn_mode = M_STREND_MODE_NONE;
      else
        {
        SLOGALWAYS0 ("Structure end alignment error");
        exitApp ();
        }
      strEndModeDone = SD_TRUE;
      continue;
      }

    valStart = lineBuf;
    while (*valStart && (*valStart == ' ' || *valStart == '\t'))
      ++valStart;
    if (*valStart && !strncmp (valStart, "0x00", 4))
      {
      if (algnCount == NUM_ALGN_TYPES)
        {
        SLOGALWAYS0 ("Type alignment table file error: too many values ");
        exitApp ();
        }
      valStart[6] = 0;
      numScan = sscanf (valStart, "%x", &data_algn_tbl[algnCount]);
      if (numScan != 1)
        {
        SLOGALWAYS0 ("Type alignment table file error");
        exitApp ();
        }
      ++algnCount;
      if (algnCount == NUM_ALGN_TYPES)
        algnTblDone = SD_TRUE;
      }
    }
  fclose (fp);
  m_data_algn_tbl = data_algn_tbl;
  }


/************************************************************************/
/*                       welcome                                                */
/************************************************************************/

static ST_VOID welcome ()
  {
  printf ("\n\n                  MMS-EASE Lite Object Foundry");
  printf ("\n(c) Copyright Systems Integration Specialists Company, Inc.,");
  printf ("\n                  1998 - 2008, All Rights Reserved\n");
  }

/************************************************************************/
/*                       exitApp                                        */
/************************************************************************/

ST_VOID exitApp ()
  {
  printf ("\n Fatal error at line %d in file '%s'. See '%s' for detail\n", 
               fileLine[fileNestLevel]+1, fileName[fileNestLevel], logFileName);
  SLOGCALWAYS3 ("Error at input line %d (%d in file '%s'). Terminated.", 
                lineCount+1, fileLine[fileNestLevel]+1, fileName[fileNestLevel]);
  exit (2);
  }

/************************************************************************/
/************************************************************************/
/* LOGGING CONTROL							*/
/************************************************************************/
/*                      initLogCfg                                      */
/************************************************************************/

static ST_VOID initLogCfg (ST_VOID)
  {
/* Allow large log messages */
  slog_max_msg_size_set (sLogCtrl, 10000);

/* Use File logging                                                     */
  sLogCtrl->logCtrl = LOG_FILE_EN;

/* Use time/date time log                                               */
  sLogCtrl->logCtrl |= LOG_TIME_EN;

/* File Logging Control defaults                                        */
  sLogCtrl->fc.fileName = logFileName;
  sLogCtrl->fc.maxSize = 1000000L;
  sLogCtrl->fc.ctrl = (FIL_CTRL_WIPE_EN | 
                 FIL_CTRL_NO_APPEND |
                 FIL_CTRL_MSG_HDR_EN);

  mms_debug_sel |= MMS_LOG_NERR;
  }

/************************************************************************/
/************************************************************************/
/* MEMORY ALLOCATION							*/
/************************************************************************/
/*                              init_mem                                */
/************************************************************************/

static ST_VOID mem_chk_error_detected (ST_VOID);
static ST_VOID *my_malloc_err (ST_UINT size);
static ST_VOID *my_calloc_err (ST_UINT num, ST_UINT size);
static ST_VOID *my_realloc_err (ST_VOID *old, ST_UINT size);

static ST_CHAR *spareMem;

ST_VOID init_mem ()
  {
#if defined(NO_GLB_VAR_INIT)
  mvl_init_glb_vars ();
#endif

/* Allocate spare memory to allow logging/printing memory errors        */
  spareMem = (ST_CHAR *) malloc (500);

/* trap mem_chk errors                                                  */
  mem_chk_err = mem_chk_error_detected;
  m_memerr_fun = my_malloc_err;
  c_memerr_fun = my_calloc_err;
  r_memerr_fun = my_realloc_err;

  }

/************************************************************************/
/* This function is called from the DEBUG version of the mem library    */
/* when an error of any type is detected.                               */
/************************************************************************/

static ST_INT memErrDetected;

static ST_VOID mem_chk_error_detected (ST_VOID)
  {
  if (!memErrDetected)
    {
    free (spareMem);
    memErrDetected = SD_TRUE;
    printf ("\n Memory Error Detected! Check log file");
    dyn_mem_ptr_status ();
    }
  }

/************************************************************************/
/* Memory Allocation Error Handling Functions.                          */
/* These functions are called from mem_chk when it is unable to         */
/* perform the requested operation. These functions must either return  */
/* a valid buffer or not return at all.                                 */
/************************************************************************/

static ST_VOID *my_malloc_err (ST_UINT size)
  {
  mem_chk_error_detected ();
  exitApp ();
  return (NULL);   
  }

static ST_VOID *my_calloc_err (ST_UINT num, ST_UINT size)
  {
  mem_chk_error_detected ();
  exitApp ();
  return (NULL);   
  }

static ST_VOID *my_realloc_err (ST_VOID *old, ST_UINT size)
  {
  mem_chk_error_detected ();
  exitApp ();
  return (NULL);   
  }

/************************************************************************/
/************************************************************************/
/* COMMAND LINE ARGUMENT PROCESSING					*/
/************************************************************************/

static ST_VOID setOverwrite (ST_INT argNum, ST_CHAR *arg)
  {
  fileOverwrite = SD_TRUE;
  }

static ST_VOID getAlignFile (ST_INT argNum, ST_CHAR *arg)
  {
  strcpy (cfgFile, &arg[2]);
  }

static ST_VOID getIncludePath (ST_INT argNum, ST_CHAR *arg)
  {
  addIncPath (&arg[2]);
  }

static ST_VOID getTfnFile (ST_INT argNum, ST_CHAR *arg)
  {
  strcpy (templateInFileName, &arg[2]);
  if (strstr (templateInFileName, ".xml"))
    {	/* New fangled leaf file */
    useTemplate = SD_FALSE;
    mvlu_use_leaf_file = SD_TRUE;
    }
  else	/* Old fashioned TFN file */
    {
    useTemplate = SD_TRUE;
    mvlu_use_leaf_file = SD_FALSE;
    }
  }

static ST_VOID getOdfFile (ST_INT argNum, ST_CHAR *arg)
  {
  if (!strlen (inFileName))
    {
    strcpy (inFileName, arg);
    strcpy (outFileName, arg);
    }
  else
    strcpy (outFileName, arg);
  }

static ST_VOID setUcaVa (ST_INT argNum, ST_CHAR *arg)
  {
  extractNames = SD_TRUE;
  createVa = SD_TRUE;
  }

static ST_VOID setUcaName (ST_INT argNum, ST_CHAR *arg)
  {
  extractNames = SD_TRUE;
  createVa = SD_FALSE;
  }

static ST_VOID setDebug (ST_INT argNum, ST_CHAR *arg)
  {
  debug = SD_TRUE;
  m_check_list_enable = SD_TRUE;
  m_fill_en = SD_TRUE;
  m_mem_debug = SD_TRUE;
  }

static ST_VOID setRtInit (ST_INT argNum, ST_CHAR *arg)
  {
  useRtInitCode = SD_TRUE;
  }

static ST_VOID setTypeList (ST_INT argNum, ST_CHAR *arg)
  {
  createTypeListFile = SD_TRUE;
  }

static ST_VOID setPrint (ST_INT argNum, ST_CHAR *arg)
  {
  verbose = SD_TRUE;
  }

static ST_VOID setNoMms (ST_INT argNum, ST_CHAR *arg)
  {
  noMmsLite = SD_TRUE;
  }

static ST_VOID printHelp (ST_INT argNum, ST_CHAR *arg)
  {
  usage ();
  }

/************************************************************************/
/*				usage					*/
/************************************************************************/

ST_VOID usage ()
  {
  printf ("\n\nUsage:  foundry [-o] [-ccfgFile] [-ttemplateFile] infile[.ext] [outfile]");
  listValidArgs ();
  exit (1);
  }
