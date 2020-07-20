/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,       */
/*               1997 - 2001 All Rights Reserved                        */
/*                                                                      */
/* MODULE NAME : foundry.h                                              */
/* PRODUCT(S)  :                                                        */
/*                                                                      */
/* MODULE DESCRIPTION :                                                 */
/*                                                                      */
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :                            */
/*              main                                                    */
/*                                                                      */
/* MODIFICATION LOG :                                                   */
/*  Date     Who   Rev                  Comments                        */
/* --------  ---  ------   -------------------------------------------  */
/* 07/12/04  JRB     20    Replace el_tag_defines w/ get_el_tag_text.	*/
/* 01/21/04  JRB     19    Add logUnusedLeafAccessInfo funct.		*/
/* 12/11/03  MDE     18    Added addIncPath global			*/
/* 10/08/03  JRB     17    Chg dummy fun names to "mvlu_null_rd(wr)_ind"*/
/* 09/22/03  JRB     16    Del leafTblFileName global. Add addDynLeafFuns.*/
/* 03/13/03  JRB     15    Use RUNTIME_CTRL *rt_ctrl in TYPE_CTRL.	*/
/* 01/06/03  MDE     14    Put leaf_tbl.c and foundry.log in output dir	*/
/* 12/12/02  JRB     13    Use "mvl_uca.h"				*/
/* 11/27/02  MDE     12    Support for CSV, XML, etc..			*/
/* 02/02/01  EJV     11    Removed extern that are static in c files:	*/
/*			   hdrHead, pathCtrl, rdIndFunCtrlHead,		*/
/*			   wrIndFunCtrlHead, refCtrlHead.		*/
/* 12/21/00  MDE     10    Added 'noMmsLite' support			*/
/* 06/21/00  MDE     09    Now can emit type list w/TDL (-l)		*/
/* 04/26/00  RKR     08    Added caseSensStrIdxs			*/
/* 03/07/00  MDE     07    Added default read/write function feature	*/
/* 05/26/99  MDE     06    Updated TFN processing			*/
/* 10/01/98  MDE     05    Tweaks for 16 bit compilers			*/
/* 09/21/98  MDE     04    Minor lint cleanup				*/
/* 09/16/98  MDE     05    Now create Journals too			*/
/* 09/10/98  MDE     04    Minor fixes for handling UCA/non-UCA types	*/
/* 08/12/98  MDE     03    Broke up foundry.c, added array support, etc	*/
/* 07/13/98  MDE     02    Misc, added 'wantTypedef' flag for type	*/
/* 02/27/98  MDE     01    New module					*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "gen_list.h"

#if defined (_WIN32) || defined(MSDOS)
#include <io.h>
#include <conio.h>
#include <ctype.h>
#endif
#include <limits.h>

#include "mmsdefs.h"
#include "mms_pvar.h"


#include "mms_vvar.h"
#include "slog.h"

#include "mms_pvmd.h"
#include "mms_err.h"
#include "mms_pcon.h"
#include "mmsdefs.h"
#include "asn1defs.h"
#include "mvl_uca.h"
#include "mem_chk.h"

/************************************************************************/
/************************************************************************/
/* DEFINES								*/
/************************************************************************/
#define MAX_CFG_LINE            512
#define MAX_STRING_SIZE         256

#if defined(PLAIN_DOS)	/* 16 bit integer platforms) */
#define MAX_COMMON_STRINGS    10000
#define ASN1_SIZE             32767
#define MAX_TDL_SIZE          10000
#define MAX_TYPEDEF_SIZE      32767
#else
#define MAX_COMMON_STRINGS    50000
#define ASN1_SIZE             250000
#define MAX_TDL_SIZE          50000
#define MAX_TYPEDEF_SIZE      250000
#endif


/************************************************************************/
/* EXTRACTED OBJECT DEFINITION CONTROL */
/************************************************************************/
/************************************************************************/
/* Object Definition File Inclusion */
typedef struct fil_ctrl
  {
  DBL_LNK l;
  ST_CHAR *hdrFile;
  } FILE_CTRL;

/************************************************************************/
/* Object Definition Include Paths */
#define MAX_NUM_PATH 25
typedef struct path_ctrl
  {
  ST_INT numPath;
  ST_CHAR *pathText[MAX_NUM_PATH];
  } PATH_CTRL;

/************************************************************************/
/* User Specified Header Files */
#define MAX_INC 25
typedef struct inc_ctrl
  {
  ST_INT numInc;
  ST_CHAR *incText[MAX_INC];
  } INC_CTRL;
extern INC_CTRL incCtrl;


/************************************************************************/
/* Types */

typedef struct mvluFoundryTypeStruct 
  {
  ST_VOID *typeCtrl;
  ST_CHAR *ucaName;
  ST_INT sortedNum;
  ST_BOOLEAN arr_comp;
  RUNTIME_TYPE *rt;
  } MVLU_FOUNDRY_TYPE_INFO;

typedef struct type_ctrl
  {
  DBL_LNK l;
  ST_CHAR *label;               /* define to be used, per user          */
  ST_CHAR *tdl;                 /* TDL, as supplied by the user         */
  ST_CHAR *desc;                /* desc, as supplied by the user        */

  RUNTIME_CTRL *rt_ctrl;	/* Runtime type table			*/
  MVLU_FOUNDRY_TYPE_INFO *mvluFoundryInfo;

  ST_UCHAR *asn1;               /* ASN.1 type                           */
  ST_INT asn1_len;

  ST_BOOLEAN unique;            /* Do not optimize away                 */
  struct type_ctrl *twin;

  ST_BOOLEAN keep;	        /* Do not delete this type		*/
  ST_BOOLEAN transient;         /* Delete when done                     */

  ST_BOOLEAN wantTypedef;       /* Always produce typedef		*/

  ST_BOOLEAN base;              /* Delete if not referenced             */
  ST_INT baseRefCount;          /* Used with base types only            */

  ST_BOOLEAN deleteIfUnused;    /* Delete if not referenced by var      */
  ST_INT varRefCount;           /* Number of vars that use this type    */

  ST_BOOLEAN ucaType;

  struct fo_read_write_ind *rdWrIndFunSel;
  } TYPE_CTRL;

extern TYPE_CTRL *typeHead;
extern ST_INT typeCount;
extern ST_INT totalRtCount;


/************************************************************************/
/* VMD Variables */
typedef struct var_ctrl
  {
  DBL_LNK l;
  ST_CHAR *name;
  TYPE_CTRL *typeCtrl;

  ST_CHAR *initText;
  ST_CHAR *procText;
  ST_CHAR *usrText;

  ST_CHAR *ref;                 /* Used in writing code, address ref.   */
  ST_CHAR *domRef;              /* Used in writing code, address ref.   */
  ST_INT scope;			/* used in UCA NVL's			*/
  ST_BOOLEAN nvlOnly;
  } VAR_CTRL;
extern ST_INT numVmdVars;
extern VAR_CTRL *vmdVarHead;


/************************************************************************/
/* VMD Variables Lists */
#define MAX_VARS_PER_VARLIST    200 
typedef struct varlist_ctrl
  {
  DBL_LNK l;
  ST_CHAR *name;
  ST_INT numVars;
  VAR_CTRL **varTable;
  } VARLIST_CTRL;
extern ST_INT numVmdVarLists;
extern VARLIST_CTRL *vmdVarListHead;


/************************************************************************/
/* Domains */
typedef struct domain_ctrl
  {
  DBL_LNK l;
  ST_CHAR *name;
  ST_INT numVars;
  VAR_CTRL *varHead;
  ST_INT numVarLists;
  VARLIST_CTRL *varListHead;
  } DOMAIN_CTRL;
extern DOMAIN_CTRL *domHead;
extern ST_INT numDoms;

/************************************************************************/
/* AA Scope */
typedef struct aa_ctrl
  {
  ST_INT numVars;
  VAR_CTRL *varHead;
  ST_INT numVarLists;
  VARLIST_CTRL *varListHead;
  } AA_CTRL;
extern AA_CTRL aaObjs;


/************************************************************************/

/* For VMD_SPEC Journals */
typedef struct jou_ctrl
  {
  DBL_LNK l;
  ST_CHAR *name;
  } JOU_CTRL;
extern ST_INT numJou;
extern JOU_CTRL *jouHead;
/* For DOM_SPEC Journals */
typedef struct dom_jou_ctrl
  {
  DBL_LNK l;
  ST_CHAR *domName;
  ST_CHAR *name;
  ST_BOOLEAN processed;
  } DOM_JOU_CTRL;
extern ST_INT dnumJou;		/* num of DOM_SPEC journals	*/
extern DOM_JOU_CTRL *djouHead;

/************************************************************************/
/* UCA Name Generation */
typedef struct name_ctrl
  {
  DBL_LNK l;
  ST_CHAR *base;
  TYPE_CTRL *typeCtrl;
  } NAME_CTRL;
extern ST_INT numNames;
extern NAME_CTRL *nameListHead;


/************************************************************************/
/************************************************************************/

/* Define statements from the template file */
typedef struct 
  {
  DBL_LNK l;
  ST_CHAR *rValue;
  ST_CHAR *lValue;
  } DEFINE_CTRL;

/************************************************************************/
/************************************************************************/
/* Functions from the template file */

/* funType defines */
#define FUNTYPE_IN_TFN 		0
#define FUNTYPE_EXTERN		1
#define FUNTYPE_REPLACED	2
#define FUNTYPE_NOT_FOUND	3

typedef struct 
  {
  DBL_LNK l;
  ST_CHAR *funName;		/* The function name			*/ 
  ST_INT funType;		/* See above for values 		*/
  ST_CHAR *functiondata;	/* used for IN_TFN,EXTERN		*/
  ST_CHAR *replacementFunName;  /* Used for REPLACED			*/
  } FUN_CTRL;


/* Template Define and Function Extraction Functions  */
ST_VOID processTfnFile (FILE *fp);
ST_VOID logTfnProcessResults (ST_VOID);
DEFINE_CTRL *findRefDefine (ST_CHAR *lValue);
FUN_CTRL *findTfnFun (ST_CHAR *funName);

extern ST_BOOLEAN noTfnCache;

/************************************************************************/
/* UCA Template Function Control */
typedef struct 
  {
  DBL_LNK l;
  ST_CHAR *funName;
  ST_CHAR *funIndexDefine;
  ST_CHAR *ucaName;
  RUNTIME_TYPE *rt;
  ST_INT rt_index;
  FUN_CTRL *funCtrl;
  ST_BOOLEAN arr_comp;
  } IND_FUN_CTRL;

/************************************************************************/
/* UCA Template Define Control */
typedef struct 
  {
  DBL_LNK l;
  ST_CHAR *refDef;
  ST_BOOLEAN dummyRef;
  } REF_CTRL;

/************************************************************************/

typedef struct fo_read_write_ind
  {
  struct
    {
    ST_CHAR rd[33];
    ST_CHAR wr[33];
    } x[15];
  } FO_READ_WRITE_IND;

extern FO_READ_WRITE_IND *currRdWrIndFuns;

/************************************************************************/
/************************************************************************/
/* FUNCTIONS								*/
/************************************************************************/

ST_VOID exitApp (ST_VOID);
ST_RET processSource (ST_VOID);
ST_VOID readAlgnTable (ST_VOID);

ST_VOID deriveUcaNames (ST_VOID);
ST_RET optimizeTypes (ST_VOID);
ST_VOID writeOutput (ST_VOID);
ST_VOID writeRtData (ST_VOID);
ST_BOOLEAN isRtPrim (RUNTIME_TYPE *rt);

ST_INT getCommonStringIndex (ST_CHAR *str);
ST_CHAR *stringIndexToDefine (ST_INT stringIndex);


ST_VOID deriveUcaRtInfo (TYPE_CTRL *typeCtrl, 
                        RUNTIME_TYPE *rt_el_ptr, 
			MVLU_FOUNDRY_TYPE_INFO *mvluFoundryInfo,
                        ST_INT rt_index);
ST_VOID write_uca_code (ST_VOID);
ST_VOID writeUcaRtInfo (TYPE_CTRL *typeCtrl, 
                        RUNTIME_TYPE *rt_el_ptr, 
                        ST_INT rt_index);
ST_VOID writeNameList (ST_VOID);
ST_VOID writeLeafInit (ST_VOID);
ST_VOID writeCodeBreak (ST_VOID);
ST_VOID usage (ST_VOID);
ST_VOID trimFileName (ST_CHAR *dest, ST_CHAR *fullName);
ST_VOID calc_offset_to_last (RUNTIME_TYPE *rt_el_ptr);
ST_CHAR *get_el_tag_text (RUNTIME_TYPE *rt_el_ptr);

/************************************************************************/
/* VARIABLES 								*/
/************************************************************************/
/************************************************************************/
/* EXTRACTED OBJECT DEFINITION CONTROL */

/* User Specified Header Files */
extern INC_CTRL incCtrl;

/* Types */
extern ST_INT typeCount;
extern ST_INT totalRtCount;
extern TYPE_CTRL *typeHead;
extern TYPE_CTRL *typedefOnlyHead;

/* VMD Variables */
extern ST_INT numVmdVars;
extern VAR_CTRL *vmdVarHead;

/* VMD Variables Lists */
extern ST_INT numVmdVarLists;
extern VARLIST_CTRL *vmdVarListHead;

/* Domains */
extern ST_INT numDoms;
extern DOMAIN_CTRL *domHead;

/* AA Scope */
extern AA_CTRL aaObjs;

/* UCA Name Generation */
extern ST_INT numNames;
extern NAME_CTRL *nameListHead;


/************************************************************************/
/* Input and Output File Names */
#define _MAX_FILE_NAME  200

extern ST_CHAR cfgFile[_MAX_FILE_NAME+1];
extern ST_CHAR hFileName[_MAX_FILE_NAME+1];
extern ST_CHAR inFileName[_MAX_FILE_NAME+1];
extern ST_CHAR cFileName[_MAX_FILE_NAME+1];
extern ST_CHAR xmlFileName[_MAX_FILE_NAME+1];
extern ST_CHAR leafTblFileName[_MAX_FILE_NAME+1];
extern ST_CHAR newFileName[_MAX_FILE_NAME+1];
extern ST_CHAR templateInFileName[_MAX_FILE_NAME+1];

/* File handles */
extern FILE *sourceFp;
extern FILE *templateFp;
extern FILE *headerFp;
extern FILE *typeFunFp;
extern FILE *codeFp;
extern FILE *newFp;
extern FILE *typeListFp;

/************************************************************************/
/* Flags, misc. */
extern ST_BOOLEAN fileOverwrite;
extern ST_BOOLEAN extractNames;
extern ST_BOOLEAN verbose;
extern ST_BOOLEAN createVa;
extern ST_BOOLEAN debug;
extern ST_BOOLEAN useTemplate;
extern ST_BOOLEAN mvlu_use_leaf_file;
extern ST_BOOLEAN mvl_uca;
extern ST_BOOLEAN mvlu_use_ref;
extern ST_BOOLEAN useRtInitCode;
extern ST_BOOLEAN caseSensStrIdxs;
extern ST_BOOLEAN noMmsLite;

#define _MVLU_NULL_READ_FUN_NAME	"mvlu_null_rd_ind"
#define _MVLU_NULL_WRITE_FUN_NAME	"mvlu_null_wr_ind"
extern ST_BOOLEAN mvlu_no_dummy;

extern ST_BOOLEAN ucaType;

extern ST_BOOLEAN createTypeListFile;

extern ST_CHAR *typeSuffix;
extern ST_BOOLEAN noTypedefs;
extern ST_CHAR *commentBar;

/************************************************************************/
/* Processing Control */

extern ST_INT lineCount;
#define MAX_FILE_NESTING     100
extern ST_INT fileNestLevel;
extern ST_CHAR *fileName[MAX_FILE_NESTING];
extern ST_INT fileLine[MAX_FILE_NESTING];

extern time_t rightnow;

/* Our very own data alignment table */
extern ST_INT data_algn_tbl[NUM_ALGN_TYPES];

extern ST_CHAR *mvl_rt_names[MAX_COMMON_STRINGS];
extern ST_INT num_mvl_rt_names;


ST_RET loadXmlLeafAccessInfo (ST_CHAR *fileName);
ST_VOID logUnusedLeafAccessInfo (ST_VOID);

ST_VOID addDynLeafFuns (ST_VOID);
ST_VOID addIncPath (ST_CHAR *pathText);

