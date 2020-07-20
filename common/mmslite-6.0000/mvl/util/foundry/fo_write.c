/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,       */
/*               1997-2006, All Rights Reserved				*/
/*                                                                      */
/* MODULE NAME : fo_write.c                                             */
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
/* 02/26/10  JRB	   Write "va->type_ctrl" so it's valid for	*/
/*			   static (foundry generated) variables too.	*/
/* 08/26/09  JRB     63    Cast fprintf args to avoid warnings.		*/
/* 08/29/08  JRB     62    If var->ref=NULL, don't log it (caused crash)*/
/* 10/30/06  JRB     61    Add VMD arg to u_mvl_get_va_aa.		*/
/*			   Use new mvl_vmd_find_dom.			*/
/*			   Del "#if !defined(MVL_DYN_ASN1_TYPES)" & asn1*/
/*			   generated in output of mvl_type_ctrl.	*/
/* 05/24/06  JRB     60    Fix NVL creation so vars completely resolved.*/
/*			   Fix code generated if DOM_SPEC journals	*/
/*			   configured without VMD_SPEC journals.	*/
/* 01/30/06  GLB     59    Integrated porting changes for VMS           */
/* 07/27/05  JRB     58    If mvl_uca!=0 & va->data is initialized,	*/
/*			   also set va->use_static_data = SD_TRUE.	*/
/* 05/10/05  JRB     57    Add "#if defined(OBSOLETE_AA_OBJ_INIT)" to	*/
/*			   output "C" file.				*/
/*			   Chg generated mvl_init_mms_objs to just	*/
/* 			   do NOTHING if called again (do not assert).	*/
/* 03/14/05  CRM     56    Replaced MVL_NUM_DYN defines with mvl_max_dyn*/
/*                         structure variables.                         */
/* 02/21/05  JRB     55    Create (don't just init) m_struct_* global vars*/
/*			   in output 'c' file (removed from mms_tdef.c).*/
/* 10/29/04  JRB     54    Write "m_lite_data_algn_tbl" struct,		*/
/*			   m_struct_start_algn_mode, m_struct_end_..	*/
/*			   in output 'c' file to use at runtime.	*/
/* 07/12/04  JRB     53    Add get_el_tag_text to map el_tag defines to	*/
/*			   text, so define changes don't break the code.*/
/* 06/29/04  JRB     52    Alloc "mvl_vmd.dom_tbl" even if numDom==0.	*/
/* 11/24/03  JRB     51    Del "type twinning" code. Duplicate types	*/
/*			   should be avoided in "input" ODF files.	*/
/* 10/20/03  JRB     50    Fix print formats.				*/
/* 09/22/03  JRB     49    Call new addDynLeafFuns.			*/
/* 05/05/03  JRB     48    Chk that var,NVL,dom,jou name < MAX_IDENT_LEN.*/
/* 04/14/03  JRB     47    Eliminate compiler warnings.			*/
/* 04/01/03  JRB     46    Add UTF8string support (see RT_UTF8_STRING).	*/
/*			   Del obsolete RT_NESTED_RT.			*/
/* 03/13/03  JRB     45    Use RUNTIME_CTRL *rt_ctrl in TYPE_CTRL.	*/
/*			   mvlu_proc_rt_type: Chg to use RUNTIME_CTRL.	*/
/*			   MVL_TYPE_CTRL: type_name is now array, copy to it*/
/* 12/17/02  JRB     44    Fix embedded comment in output .c & .h files.*/
/* 12/13/02  MDE     43    Add some "#ifndef USE_RT_TYPE_2"		*/
/* 12/11/02  JRB     42    Added #include "mvl_uca.h" to output H file.	*/
/* 12/09/02  MDE     41    Allow replacement of name table		*/
/* 12/02/02  MDE     40    XML LAP, other enhancements			*/
/* 04/16/02  MDE     39    Allow mvl_init_type_ctrl to be called early	*/
/* 02/25/02  MDE     38    Now get max PDU size from mvl_cfg_info	*/
/* 02/13/02  JRB     37    Del ml_init_buffers call (no more mbufcalc).	*/
/* 02/07/02  JRB     36    Assert if mvl_init_mms_objs called twice.	*/
/* 01/30/02  JRB     35    In mvl_init_mms_objs: don't call mvl_init	*/
/*			   (called by mvl_start_acse), don't check	*/
/*			   MVL_NUM_CALL* defs or set mvl_num_call* vars,*/
/*			   assert if mvl_*_conn_ctrl not set.		*/
/* 01/24/02  EJV     34    Added support for DOM_SPEC journals.		*/
/*			   Added slog in writeTypeCtrl ()		*/
/* 11/19/01  EJV     33    Added support for new MMS type UtcTime:	*/
/*			   el_tag_defines[]: added RT_UTC_TIME,mv others*/
/*			   writeRtEl: added code for RT_UTC_TIME	*/
/* 07/11/01  JRB     32    All generated code use M_CALLOC with context	*/
/*			   = MSMEM_STARTUP.				*/
/* 05/21/01  MDE     31    Cleaned up twin handling			*/
/* 01/04/00  EJV     30    checkExist: eliminated non-ANSI kb interface.*/
/* 12/21/00  MDE     29    Added 'noMmsLite' support			*/
/* 10/06/00  MDE     28    Now setup AA Vars, VarLists for calling too	*/
/* 10/06/00  MDE     27    Added _mvlu_num_dyn_types			*/
/* 07/13/00  JRB     26    Generate "#ifdef USE_RT_TYPE_2" code.	*/
/* 06/21/00  MDE     25    Now can emit type list w/TDL (-l)		*/
/* 05/01/00  JRB     24    Chg "if (..)" to "#if (..)" for Lint.	*/
/* 04/26/00  JRB     23    Add mvl_init_type_ctrl prototype.		*/
/* 04/26/00  RKR     22    Added caseSensStrIdxs			*/
/* 04/14/00  JRB     21    mvl_init_aa*: use mvl_num_called (lower case)*/
/* 04/07/00  JRB     20    Fix code if 0 VMD vars, VMD varlists,	*/
/*			   AA vars, AA varlists, or journals.		*/
/* 03/07/00  MDE     19    Added 'included' and C++ header & footer	*/
/* 03/07/00  MDE     18   Added type_id to/from string code		*/
/* 12/20/99  GLB     17    Added static to mvl_init_type_ctrl           */
/* 11/19/99  MDE     16    Corrected problem with variables of UCA type	*/
/*		           w/o _UCA_ prefix.				*/
/* 09/07/99  MDE     15    Added scope to MVL_NVLIST_CTRL		*/
/* 05/26/99  MDE     14    Now use chk_strdup				*/
/* 05/12/99  MDE     13    Open all files in text mode			*/
/* 03/22/99  RKR     12    fixed aa init in writeAAVarListInit	  	*/
/* 03/18/99  MDE     11    Fixes to avoid chk_calloc (0) errors	when	*/
/*			   the MVL_NUM_DYN_xxxx defines are chaned to 0	*/
/* 03/18/99  MDE     10    Changes to support dynamic object subsystem	*/
/* 03/18/99  MDE     09    Changes to support runtime connection limits */
/* 01/08/99  JRB     08    Chg for bsearch. Strip off "_UCA_" on var.	*/
/* 12/08/98  MDE     07    Added dynamic object handling		*/
/* 11/11/98  MDE     06    Fixed serious problem with multiple NVL's	*/
/* 09/21/98  MDE     05    Minor lint cleanup				*/
/* 09/16/98  MDE     04    Now create Journals too			*/
/* 09/10/98  MDE     03    Minor fixes for handling UCA/non-UCA types	*/
/* 07/23/98  MDE     02    Discard unused strings from the string table	*/
/* 07/21/98  MDE     01    New module, extracted from foundry.c		*/
/************************************************************************/

#include "foundry.h"

/* OBSOLETE_TWINNING_ENABLE: This define enables obsolete "type twinning"*/
/*	code. Enable this code only if absolutely necessary.		*/
/*	Using this code is very confusing, and there are much safer	*/
/*	ways to achieve the same results.				*/
/*	This code may be deleted in future releases.			*/

/************************************************************************/
/************************************************************************/

SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;

/************************************************************************/
/************************************************************************/

static ST_VOID writeHeader (ST_VOID);
static ST_VOID writeFooter (ST_VOID);
static ST_VOID writeCommonStrings (ST_VOID);
static ST_VOID writeCommonStringDefines (ST_VOID);
static ST_VOID writeCodeLeader (ST_VOID);
static ST_VOID writeObjInit (ST_VOID);
static ST_VOID writeVarInitCode (VAR_CTRL *var, ST_CHAR *prefix,
			  ST_CHAR *scopeText);
static ST_VOID writeVarListInitCode (VARLIST_CTRL *varList, ST_CHAR *prefix,
			     ST_CHAR *scopeString);
static ST_VOID writeTypeCtrl (ST_VOID);
static ST_VOID writeRtEl (TYPE_CTRL *typeCtrl, 
                   RUNTIME_TYPE *rt_el_ptr, 
		   MVLU_FOUNDRY_TYPE_INFO *mvluFoundryInfo,
		   ST_INT rt_index, ST_INT first);

static ST_VOID createMvlRtNames ();
static ST_VOID addCommonString (ST_CHAR *str);

static ST_VOID writeVmdVarInit (ST_VOID);
static ST_VOID writeDomVarInit (ST_VOID);
static ST_VOID writeAAVarInit (ST_VOID);
static ST_VOID writeVmdVarListInit (ST_VOID);
static ST_VOID writeDomVarListInit (ST_VOID);
static ST_VOID writeAAVarListInit (ST_VOID);
static ST_VOID writeJournalInit (ST_VOID);
static ST_VOID writeTypedefs (ST_VOID);
static ST_VOID checkExist (ST_CHAR *filename);

/************************************************************************/
/************************************************************************/
/*                       writeOutput                                    */
/************************************************************************/

ST_VOID writeOutput ()
  {
  if (verbose == SD_TRUE)
    printf ("\rWriting output files ...                           ");

  if (debug == SD_TRUE)
    {
    SLOGALWAYS0 ("Writing output files ...");
    }

/*  Open output C and H files                                        */
  checkExist (cFileName);
  if ((codeFp = fopen (cFileName, "w")) == NULL)
    {
    printf ("Error, cannot create output file '%s'\n", cFileName);
    usage();
    }

  /* Open unresolved template item file */
  if (mvl_uca == SD_TRUE)
    {
    if ((newFp = fopen (newFileName, "w")) == NULL)
      {
      printf ("Error, cannot create new item file '%s'\n", newFileName);
      usage();
      }
    fprintf (newFp, "\n/* Incomplete Leaf Handlers:           */");
    }

  if (extractNames == SD_FALSE)
    {
    checkExist (hFileName);
    if ((headerFp = fopen (hFileName, "w")) == NULL)
      {
      printf ("Error, cannot create output file '%s'\n", hFileName);
      usage();
      }

    if (verbose == SD_TRUE)
      printf ("\rWriting header ...                           ");
    writeHeader ();
    if (verbose == SD_TRUE)
      printf ("\rWriting code ...                           ");

    createMvlRtNames ();
    writeCodeLeader ();
    writeObjInit ();

    if (!noMmsLite)
      {
      writeVmdVarInit ();
      writeDomVarInit ();
      writeAAVarInit ();
      writeVmdVarListInit ();
      writeDomVarListInit ();
      writeAAVarListInit ();
      writeJournalInit ();
      }
    writeTypeCtrl ();
    writeCommonStringDefines ();
    writeTypedefs ();
    if (!noMmsLite)
      {
      if (mvl_uca == SD_TRUE)
        {
        fprintf (newFp, "\n\n");
        write_uca_code ();
        }
      }
    writeFooter ();
    }
  else
    {
    if (verbose == SD_TRUE)
      printf ("\rWriting UCA names ...                           ");

    writeNameList ();
    }

  writeLeafInit ();

  fclose (codeFp);
  if (headerFp != NULL)
    {
    fclose (headerFp);
    }
  if (newFp != NULL)
    fclose (newFp);

  if (createTypeListFile == SD_TRUE && typeListFp != NULL)
    fclose (typeListFp);
  }

/************************************************************************/
/*                       writeCodeLeader                                        */
/************************************************************************/
/*      Write the beginning of the output .c file                       */

static ST_VOID writeCodeLeader ()
  {
FILE *fp;
ST_INT i;
ST_CHAR trimedFileName[256]; 
ST_CHAR *mode_string;	/* mode converted to string to write to "c" file*/

  fp = codeFp;
  writeCodeBreak (); 
  fprintf (fp, "/*  This file created from input file '%s'\n", inFileName);
  fprintf (fp, "    Leaf Access Parameter (LAP) File: '%s'\n", 
      useTemplate || mvlu_use_leaf_file ? templateInFileName : "Not Used");
  fprintf (fp, "\tCreated %s", ctime (&rightnow));
  fprintf (fp, "*/\n\n");
  trimFileName (trimedFileName, hFileName);
  if (!noMmsLite)
    {
    fprintf (fp, "#include \"glbtypes.h\"\n");
    fprintf (fp, "#include \"sysincs.h\"\n");
    fprintf (fp, "#include \"mmsdefs.h\"\n");
    fprintf (fp, "#include \"mms_pvar.h\"\n");
    fprintf (fp, "#include \"mms_vvar.h\"\n");
    fprintf (fp, "#include \"mvl_acse.h\"\n");
    fprintf (fp, "#include \"%s\"\n\n", trimedFileName); 
    }

  for (i = 0; i < incCtrl.numInc; ++i)
    fprintf (fp, "#include \"%s\"\t/* User Specified */\n", incCtrl.incText[i]);

  if (noMmsLite)
    fprintf (fp, "#include \"%s\"\n\n", trimedFileName); 

  fprintf (fp, "\n#ifdef DEBUG_SISCO");
  fprintf (fp, "\nSD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__; ");
  fprintf (fp, "\n#endif");
  fprintf (fp, "\n\n");


  writeCodeBreak ();
  fprintf (fp, "MVL_TYPE_CTRL *mvl_type_ctrl;\n");
  fprintf (fp, "ST_INT mvl_num_types;\n");
  if (!noMmsLite)
    fprintf (fp, "MVL_VMD_CTRL mvl_vmd;\n");

  writeCodeBreak ();
  /* Generate data alignment table to use at runtime from 'align.cfg' info.*/
  fprintf (fp, "\n/* Init alignment table according to 'align.cfg'	*/");
  fprintf (fp, "\nST_INT m_lite_data_algn_tbl [NUM_ALGN_TYPES] = /* Data Alignment Table*/");
  fprintf (fp, "\n  {");
  fprintf (fp, "\n  0x%04x,  /* ARRSTRT_ALGN   00  */", data_algn_tbl[0]);
  fprintf (fp, "\n  0x%04x,  /* ARREND_ALGN    01  */", data_algn_tbl[1]);
  fprintf (fp, "\n  0x%04x,  /* STRSTRT_ALGN   02  */", data_algn_tbl[2]);
  fprintf (fp, "\n  0x%04x,  /* STREND_ALGN    03  */", data_algn_tbl[3]);
  fprintf (fp, "\n  0x%04x,  /* INT8_ALGN      04  */", data_algn_tbl[4]);
  fprintf (fp, "\n  0x%04x,  /* INT16_ALGN     05  */", data_algn_tbl[5]);
  fprintf (fp, "\n  0x%04x,  /* INT32_ALGN     06  */", data_algn_tbl[6]);
  fprintf (fp, "\n  0x%04x,  /* INT64_ALGN     07  */", data_algn_tbl[7]);
  fprintf (fp, "\n  0x%04x,  /* FLOAT_ALGN     08  */", data_algn_tbl[8]);
  fprintf (fp, "\n  0x%04x,  /* DOUBLE_ALGN    09  */", data_algn_tbl[9]);
  fprintf (fp, "\n  0x%04x,  /* OCT_ALGN       10  */", data_algn_tbl[10]);
  fprintf (fp, "\n  0x%04x,  /* BOOL_ALGN      11  */", data_algn_tbl[11]);
  fprintf (fp, "\n  0x%04x,  /* BCD1_ALGN      12  */", data_algn_tbl[12]);
  fprintf (fp, "\n  0x%04x,  /* BCD2_ALGN      13  */", data_algn_tbl[13]);
  fprintf (fp, "\n  0x%04x,  /* BCD4_ALGN      14  */", data_algn_tbl[14]);
  fprintf (fp, "\n  0x%04x,  /* BIT_ALGN       15  */", data_algn_tbl[15]);
  fprintf (fp, "\n  0x%04x   /* VIS_ALGN       16  */", data_algn_tbl[16]);
  fprintf (fp, "\n  };\n");

  /* Generate m_struct_start(end)_algn_mode. These are also read from	*/
  /* 'align.cfg' and belong with the data alignment table.		*/
  fprintf (fp, "\n/* Init structure alignment mode variables according to 'align.cfg'	*/");
  if (m_struct_start_algn_mode == M_STRSTART_MODE_NONE)
    mode_string = "M_STRSTART_MODE_NONE";
  else if (m_struct_start_algn_mode == M_STRSTART_MODE_FIRST)
    mode_string = "M_STRSTART_MODE_FIRST";
  else if (m_struct_start_algn_mode == M_STRSTART_MODE_LARGEST)
    mode_string = "M_STRSTART_MODE_LARGEST";
  else
    {
    SLOGALWAYS1 ("Structure start alignment value %d illegal", m_struct_start_algn_mode);
    exitApp ();
    }
  fprintf (fp, "\nST_INT m_struct_start_algn_mode = %s;\t/* value of M_STRSTART_MODE from align.cfg*/", mode_string);

  if (m_struct_end_algn_mode == M_STREND_MODE_NONE)
    mode_string = "M_STREND_MODE_NONE";
  else if (m_struct_end_algn_mode == M_STREND_MODE_LARGEST)
    mode_string = "M_STREND_MODE_LARGEST";
  else
    {
    SLOGALWAYS1 ("Structure end alignment value %d illegal", m_struct_end_algn_mode);
    exitApp ();
    }
  fprintf (fp, "\nST_INT m_struct_end_algn_mode = %s;\t/* value of M_STREND_MODE from align.cfg*/", mode_string);

  writeCommonStrings ();
  }


/************************************************************************/
/*                             trimFileName                             */
/************************************************************************/

ST_VOID trimFileName (ST_CHAR *dest, ST_CHAR *fullName)
  {
ST_INT i,s;
ST_CHAR c;

  s = 0;
  i = 0;
  while ((c = fullName[i]))
    {
    if (c == '\\' || c == '/' || c == ':' || c == ']' )
      s = i+1;  /* remember the last path seperator character position  */
    i++; 
    }
  strcpy (dest,&fullName[s]);
  }

/************************************************************************/
/*                       writeCommonStrings                             */
/************************************************************************/

static ST_VOID writeCommonStrings ()
  {
FILE *fp;
ST_INT i;

  fp = codeFp;
  writeCodeBreak ();
  fprintf (fp, "\n\n");
  fprintf (fp, "/*\tCommon Strings Table\t*/\n");
  fprintf (fp, "ST_INT numMvlRtNames;\n");
  fprintf (fp, "#ifdef USE_RT_TYPE_2\n");
  for (i = 0; i < num_mvl_rt_names; ++i)
    {
    fprintf (fp, "ST_CHAR mvlCompName_%s [] = \"%s\";\n", mvl_rt_names[i], mvl_rt_names[i]);
    }
  fprintf (fp, "#else\t/* !USE_RT_TYPE_2\t*/\n");
  fprintf (fp, "SD_CONST ST_CHAR *SD_CONST foMvlRtNames[] =\n");
  fprintf (fp, "  {\n");
  for (i = 0; i < num_mvl_rt_names; ++i)
    {
    fprintf (fp, "  \"%s\"", mvl_rt_names[i]);
    if (i < num_mvl_rt_names-1)
       fprintf (fp, ",\n");
    }
  if (num_mvl_rt_names == 0)
    fprintf (fp, "  NULL");
  fprintf (fp, "\n  };\n\n");
  fprintf (fp, "ST_CHAR **mvlRtNames;\n");
  fprintf (fp, "ST_INT maxMvlRtNames;\n");
  fprintf (fp, "#endif\t/* !USE_RT_TYPE_2\t*/\n");
  }


/************************************************************************/
/*                       writeCommonStringDefines                       */
/************************************************************************/

ST_VOID writeCommonStringDefines ()
  {
FILE *fp;
ST_INT i;

  fp = headerFp;
  fprintf (fp, "\n");
  fprintf (fp, "/************************************************************************/");
  fprintf (fp, "\n");
  fprintf (fp, "\n\n");
  fprintf (fp, "/*\tCommon Strings Index Defines\t*/\n");
  fprintf (fp, "\n");
  fprintf (fp, "#ifndef USE_RT_TYPE_2\n");
  for (i = 1; i < num_mvl_rt_names; ++i)
    fprintf (fp, "\n#define %s\t %d", stringIndexToDefine(i), i);
  fprintf (fp, "\n#endif\n");
  fprintf (fp, "\n");
  }

/************************************************************************/
/*                       writeObjInit                                   */
/************************************************************************/

static ST_VOID writeObjInit ()
  {
  writeCodeBreak ();
  fprintf (codeFp, "\n/* MMS OBJECT INITIALIZATION */");

  fprintf (codeFp, "\n");
  fprintf (codeFp, "\nST_VOID mvl_init_type_ctrl (ST_VOID);");
  if (!noMmsLite)
    {
    fprintf (codeFp, "\nstatic ST_VOID mvl_init_vmd_vars (ST_VOID);");
    fprintf (codeFp, "\nstatic ST_VOID mvl_init_dom_vars (ST_VOID);");
    fprintf (codeFp, "\nstatic ST_VOID mvl_init_aa_vars (ST_VOID);");
    fprintf (codeFp, "\nstatic ST_VOID mvl_init_vmd_varLists (ST_VOID);");
    fprintf (codeFp, "\nstatic ST_VOID mvl_init_dom_varLists (ST_VOID);");
    fprintf (codeFp, "\nstatic ST_VOID mvl_init_aa_varLists (ST_VOID);");
    fprintf (codeFp, "\nstatic ST_VOID mvl_init_journals (ST_VOID);");
    }
  fprintf (codeFp, "\n");

  fprintf (codeFp, "\n/* mvl_init_mms_objs may be called more than once, but only first call	*/");
  fprintf (codeFp, "\n/* has any effect.							*/");
  fprintf (codeFp, "\nST_VOID mvl_init_mms_objs ()");
  fprintf (codeFp, "\n  {");

  if (!noMmsLite)
    {
    fprintf (codeFp, "\n#if defined(OBSOLETE_AA_OBJ_INIT)");	/*disabled by default*/
    fprintf (codeFp, "\nST_INT i;");
    fprintf (codeFp, "\n#endif	/*#if defined(OBSOLETE_AA_OBJ_INIT)*/");
    fprintf (codeFp, "\nstatic ST_BOOLEAN _mvlInitMmsObjsCalled = SD_FALSE;");
    fprintf (codeFp, "\n");
    fprintf (codeFp, "\n/* If already called once, do NOTHING.	*/");
    fprintf (codeFp, "\n  if (_mvlInitMmsObjsCalled)");
    fprintf (codeFp, "\n    return;");
    fprintf (codeFp, "\n  _mvlInitMmsObjsCalled = SD_TRUE;");
    fprintf (codeFp, "\n\n");

    fprintf (codeFp, "  if (mvl_max_dyn.aa_nvls == 0)\n");
    fprintf (codeFp, "    mvl_max_dyn.aa_nvls = MVL_NUM_DYN_AA_NVLS;\n");
    fprintf (codeFp, "\n");

    fprintf (codeFp, "  if (mvl_max_dyn.aa_vars == 0)\n");
    fprintf (codeFp, "    mvl_max_dyn.aa_vars = MVL_NUM_DYN_AA_VARS;\n");
    fprintf (codeFp, "\n");

    fprintf (codeFp, "  if (mvl_max_dyn.doms == 0)\n");
    fprintf (codeFp, "    mvl_max_dyn.doms = MVL_NUM_DYN_DOMS;\n");
    fprintf (codeFp, "\n");

    fprintf (codeFp, "  if (mvl_max_dyn.dom_nvls == 0)\n");
    fprintf (codeFp, "    mvl_max_dyn.dom_nvls = MVL_NUM_DYN_DOM_NVLS;\n");
    fprintf (codeFp, "\n");

    fprintf (codeFp, "  if (mvl_max_dyn.dom_vars == 0)\n");
    fprintf (codeFp, "    mvl_max_dyn.dom_vars = MVL_NUM_DYN_DOM_VARS;\n");
    fprintf (codeFp, "\n");

    fprintf (codeFp, "  if (mvl_max_dyn.journals == 0)\n");
    fprintf (codeFp, "    mvl_max_dyn.journals = MVL_NUM_DYN_JOUS;\n");
    fprintf (codeFp, "\n");

    fprintf (codeFp, "  if (mvl_max_dyn.types == 0)\n");
    fprintf (codeFp, "    mvl_max_dyn.types = MVLU_NUM_DYN_TYPES;\n");
    fprintf (codeFp, "\n");

    fprintf (codeFp, "  if (mvl_max_dyn.vmd_nvls == 0)\n");
    fprintf (codeFp, "    mvl_max_dyn.vmd_nvls = MVL_NUM_DYN_VMD_NVLS;\n");
    fprintf (codeFp, "\n");

    fprintf (codeFp, "  if (mvl_max_dyn.vmd_vars == 0)\n");
    fprintf (codeFp, "    mvl_max_dyn.vmd_vars = MVL_NUM_DYN_VMD_VARS;\n");
    fprintf (codeFp, "\n");

    fprintf (codeFp, "\n#if defined(OBSOLETE_AA_OBJ_INIT)");	/*disabled by default*/
    fprintf (codeFp, "\n/* Make sure conn_ctrl allocated (by mvl_start_acse)	*/");
    fprintf (codeFp, "\n  assert (mvl_calling_conn_ctrl || mvl_called_conn_ctrl);");
    fprintf (codeFp, "\n");
    fprintf (codeFp, "\n/* Set up the AA Control structures */");
    fprintf (codeFp, "\n  for (i = 0; i < mvl_cfg_info->num_called; ++i)");
    fprintf (codeFp, "\n    mvl_called_conn_ctrl[i].aa_objs = (MVL_AA_OBJ_CTRL *) M_CALLOC (MSMEM_STARTUP, 1, sizeof(MVL_AA_OBJ_CTRL));");
    fprintf (codeFp, "\n  for (i = 0; i < mvl_cfg_info->num_calling; ++i)");
    fprintf (codeFp, "\n    mvl_calling_conn_ctrl[i].aa_objs = (MVL_AA_OBJ_CTRL *) M_CALLOC (MSMEM_STARTUP, 1, sizeof(MVL_AA_OBJ_CTRL));");
    fprintf (codeFp, "\n#endif	/*#if defined(OBSOLETE_AA_OBJ_INIT)*/");
    fprintf (codeFp, "\n");
  
    if (numVmdVars || numVmdVarLists || numDoms)
      fprintf (codeFp, "\n  mvl_vmd.foundry_objects = SD_TRUE;");
    }
  
  fprintf (codeFp, "\n  mvl_init_type_ctrl ();");

  if (!noMmsLite)
    {
    fprintf (codeFp, "\n  mvl_init_vmd_vars ();");
    fprintf (codeFp, "\n  mvl_init_dom_vars ();");
    fprintf (codeFp, "\n  mvl_init_aa_vars ();");
    fprintf (codeFp, "\n  mvl_init_vmd_varLists ();");
    fprintf (codeFp, "\n  mvl_init_dom_varLists ();");
    fprintf (codeFp, "\n  mvl_init_aa_varLists ();");
    fprintf (codeFp, "\n  mvl_init_journals ();");
    }
  fprintf (codeFp, "\n  }");
  }

/************************************************************************/
/*                       writeDomVarInit                                */
/************************************************************************/

ST_VOID writeDomVarInit ()
  {
DOMAIN_CTRL *dom;
VAR_CTRL *var;
ST_INT varIndex;
ST_INT domIndex;
ST_CHAR refBuf[100];
ST_UINT namelen;	/* len of domain name	*/

  if (debug == SD_TRUE)
    {
    SLOGALWAYS0 ("WRITING DOMAIN INITIALIZATION CODE");
    }

  writeCodeBreak ();
  fprintf (codeFp, "\n/* DOMAIN VARIABLE INITIALIZATION */");
  fprintf (codeFp, "\nstatic ST_VOID mvl_init_dom_vars ()");
  fprintf (codeFp, "\n  {");
  fprintf (codeFp, "\nMVL_DOM_CTRL **ppdom;");
  if (numDoms)
    {
    fprintf (codeFp, "\nMVL_DOM_CTRL *dom;");
    fprintf (codeFp, "\nMVL_VAR_ASSOC *va;");
    fprintf (codeFp, "\nMVL_VAR_ASSOC **ppva;");
    }
  fprintf (codeFp, "\n");

  fprintf (codeFp, "\n  mvl_vmd.num_dom = %d;", numDoms);
  fprintf (codeFp, "\n  mvl_vmd.max_num_dom = %d + mvl_max_dyn.doms;", numDoms);
  fprintf (codeFp, "\n  if (mvl_vmd.max_num_dom)");
  fprintf (codeFp, "\n    mvl_vmd.dom_tbl = ppdom = (MVL_DOM_CTRL **) M_CALLOC (MSMEM_STARTUP, mvl_vmd.max_num_dom, sizeof (MVL_DOM_CTRL *));");
  if (numDoms)
    {
    fprintf (codeFp, "\n  dom = (MVL_DOM_CTRL *) M_CALLOC (MSMEM_STARTUP, %d, sizeof (MVL_DOM_CTRL));", 
                                        numDoms);
  
    dom = domHead;
    domIndex = 0;
    while (dom != NULL) 
      {
      if (debug == SD_TRUE)
        {
        SLOGCALWAYS1 ("  Domain: '%s", dom->name);
        }
      if ((namelen = strlen (dom->name)) > MAX_IDENT_LEN)
        {
        SLOGALWAYS2 ("Dom name '%s' len=%d exceeds maximum len", dom->name, namelen);
        exitApp ();
        }
      fprintf (codeFp, "\n\n/* Domain: '%s'\t*/", dom->name);
      fprintf (codeFp, "\n  dom->name = \"%s\";", dom->name);
  
      fprintf (codeFp, "\n  dom->max_num_var_assoc = %d + mvl_max_dyn.dom_vars;", dom->numVars);
      fprintf (codeFp, "\n  dom->num_var_assoc = %d;", dom->numVars);
      if (dom->numVars || dom->numVarLists)
        fprintf (codeFp, "\n  dom->foundry_objects = SD_TRUE;");

      fprintf (codeFp, "\n  if (dom->max_num_var_assoc)");
      fprintf (codeFp, "\n    dom->var_assoc_tbl = ppva = (MVL_VAR_ASSOC **) M_CALLOC (MSMEM_STARTUP, dom->max_num_var_assoc, sizeof (MVL_VAR_ASSOC *));");
      fprintf (codeFp, "\n");
      if (dom->numVars)
        {
        fprintf (codeFp, "\n  va = (MVL_VAR_ASSOC *) M_CALLOC (MSMEM_STARTUP, %d, sizeof (MVL_VAR_ASSOC));", 
                                                dom->numVars);

        varIndex = 0;
        var = dom->varHead;
        while (var != NULL) 
          {
          if (debug == SD_TRUE)
            {
            SLOGCALWAYS1 ("  Variable: '%s'", var->name);
            }
          writeVarInitCode (var,"", "DOM_SPEC");
          fprintf (codeFp, "\n  *ppva++ = va++;");
  
          sprintf (refBuf, "mvl_vmd.dom_tbl[%d]->var_assoc_tbl[%d]", domIndex, varIndex);
          var->ref = chk_strdup (refBuf);
  
          var = (VAR_CTRL *) list_get_next (dom->varHead, var);
          ++varIndex;
          }
        }
      fprintf (codeFp, "\n  *ppdom++ = dom++;");
      dom = (DOMAIN_CTRL *) list_get_next (domHead, dom);
      ++domIndex;
      }
    }	/* end "if (numDoms)"	*/

  fprintf (codeFp, "\n  }");
  fprintf (codeFp, "\n");
  }

/************************************************************************/
/*                       writeDomVlistinit                              */
/************************************************************************/

ST_VOID writeDomVarListInit ()
  {
DOMAIN_CTRL *dom;
VARLIST_CTRL *varList;
ST_INT domIndex;

  if (debug == SD_TRUE)
    {
    SLOGALWAYS0 ("WRITING DOMAIN INITIALIZATION CODE");
    }

  writeCodeBreak ();
  fprintf (codeFp, "\n/* DOMAIN VARIABLE LIST INITIALIZATION */");
  fprintf (codeFp, "\nstatic ST_VOID mvl_init_dom_varLists ()");
  fprintf (codeFp, "\n  {");
  if (numDoms)
    {
    fprintf (codeFp, "\nMVL_DOM_CTRL *dom;");
    fprintf (codeFp, "\nMVL_NVLIST_CTRL *vl;");
    fprintf (codeFp, "\nMVL_NVLIST_CTRL **ppvl;");
    fprintf (codeFp, "\n");

    fprintf (codeFp, "\n  dom = mvl_vmd.dom_tbl [0];\t/* allocated by mvl_init_dom_vars\t*/");
    domIndex = 0;
    dom = domHead;
    while (dom != NULL) 
      {
      if (debug == SD_TRUE)
        {
        SLOGCALWAYS1 ("  Domain: '%s", dom->name);
        }
      fprintf (codeFp, "\n\n/* Domain: '%s'\t*/", dom->name);
      fprintf (codeFp, "\n\n  dom->max_num_nvlist = %d + mvl_max_dyn.dom_nvls;", dom->numVarLists);
      fprintf (codeFp, "\n  dom->num_nvlist = %d;", dom->numVarLists);

      fprintf (codeFp, "\n  if (dom->max_num_nvlist)");
      fprintf (codeFp, "\n    dom->nvlist_tbl = ppvl = (MVL_NVLIST_CTRL **) M_CALLOC (MSMEM_STARTUP, dom->max_num_nvlist, sizeof (MVL_NVLIST_CTRL *));");

      if (dom->numVarLists)
        {
        fprintf (codeFp, "\n  vl = (MVL_NVLIST_CTRL *) M_CALLOC (MSMEM_STARTUP, %d, sizeof (MVL_NVLIST_CTRL));", 
                                        dom->numVarLists);
        varList = dom->varListHead;
        while (varList != NULL) 
          {
          if (debug == SD_TRUE)
            {
            SLOGCALWAYS1 ("  Variable List: '%s'", varList->name);
            }
          writeVarListInitCode (varList,"", "DOM_SPEC");
          fprintf (codeFp, "\n  *ppvl++ = vl++;");
          varList = (VARLIST_CTRL *) list_get_next (dom->varListHead, varList);
          }
        }
      fprintf (codeFp, "\n  ++dom;");
      dom = (DOMAIN_CTRL *) list_get_next (domHead, dom);
      }
    }
  fprintf (codeFp, "\n  }");
  fprintf (codeFp, "\n");
  }


/************************************************************************/
/*                       writeAAVarInit                                 */
/************************************************************************/

ST_VOID writeAAVarInit ()
  {
VAR_CTRL *var;
ST_INT varIndex;
ST_CHAR refBuf[100];

  if (debug == SD_TRUE)
    {
    SLOGALWAYS0 ("WRITING AA INITIALIZATION CODE");
    }

  writeCodeBreak ();
  fprintf (codeFp, "\n/* AA VARIABLE INITIALIZATION */");
  fprintf (codeFp, "\nstatic ST_VOID mvl_init_aa_vars ()");
  fprintf (codeFp, "\n  {");

  fprintf (codeFp, "\n#if defined(OBSOLETE_AA_OBJ_INIT)");	/*whole funct disabled by default*/

  fprintf (codeFp, "\nMVL_AA_OBJ_CTRL *aa;");
  if (aaObjs.numVars)
    {
    fprintf (codeFp, "\nMVL_VAR_ASSOC *va;");
    }
  fprintf (codeFp, "\nMVL_VAR_ASSOC **ppva;");
  fprintf (codeFp, "\nST_INT i;");
  fprintf (codeFp, "\nST_INT j;");
  fprintf (codeFp, "\n");
  fprintf (codeFp, "\n/* Do AA specific variables */");
  fprintf (codeFp, "\n  i = 0;");
  fprintf (codeFp, "\n  for (j = 0; j < mvl_cfg_info->num_called; ++j, ++i)");
  fprintf (codeFp, "\n    {");
  fprintf (codeFp, "\n    aa = (MVL_AA_OBJ_CTRL *) mvl_called_conn_ctrl[j].aa_objs;");
  fprintf (codeFp, "\n    aa->foundry_objects = SD_TRUE;");

  fprintf (codeFp, "\n    aa->max_num_var_assoc = %d + mvl_max_dyn.aa_vars;", aaObjs.numVars);
  fprintf (codeFp, "\n    aa->num_var_assoc = %d;", aaObjs.numVars);

  fprintf (codeFp, "\n    if (aa->max_num_var_assoc)");
  fprintf (codeFp, "\n      aa->var_assoc_tbl = ppva = (MVL_VAR_ASSOC **) M_CALLOC (MSMEM_STARTUP, aa->max_num_var_assoc, sizeof (MVL_VAR_ASSOC *));");

  if (aaObjs.numVars)
    {
    fprintf (codeFp, "\n    va = (MVL_VAR_ASSOC *) M_CALLOC (MSMEM_STARTUP, %d, sizeof (MVL_VAR_ASSOC));", 
                              aaObjs.numVars);
    varIndex = 0;
    var = aaObjs.varHead;
    while (var != NULL) 
      {
      if (debug == SD_TRUE)
        {
        SLOGCALWAYS1 ("  Variable: '%s'", var->name);
        }
      writeVarInitCode (var,"  ", "AA_SPEC");
      fprintf (codeFp, "\n    *ppva++ = va++;");
  
      sprintf (refBuf, "aa->var_assoc_tbl[%d]", varIndex);
      var->ref = chk_strdup (refBuf);
  
      var = (VAR_CTRL *) list_get_next (aaObjs.varHead, var);
      ++varIndex;
      }
    }
  fprintf (codeFp, "\n    }");


  fprintf (codeFp, "\n  for (j = 0; j < mvl_cfg_info->num_calling; ++j, ++i)");
  fprintf (codeFp, "\n    {");
  fprintf (codeFp, "\n    aa = (MVL_AA_OBJ_CTRL *) mvl_calling_conn_ctrl[j].aa_objs;");
  fprintf (codeFp, "\n    aa->foundry_objects = SD_TRUE;");

  fprintf (codeFp, "\n    aa->max_num_var_assoc = %d + mvl_max_dyn.aa_vars;", aaObjs.numVars);
  fprintf (codeFp, "\n    aa->num_var_assoc = %d;", aaObjs.numVars);

  fprintf (codeFp, "\n    if (aa->max_num_var_assoc)");
  fprintf (codeFp, "\n      aa->var_assoc_tbl = ppva = (MVL_VAR_ASSOC **) M_CALLOC (MSMEM_STARTUP, aa->max_num_var_assoc, sizeof (MVL_VAR_ASSOC *));");

  if (aaObjs.numVars)
    {
    fprintf (codeFp, "\n    va = (MVL_VAR_ASSOC *) M_CALLOC (MSMEM_STARTUP, %d, sizeof (MVL_VAR_ASSOC));", 
                              aaObjs.numVars);
    varIndex = 0;
    var = aaObjs.varHead;
    while (var != NULL) 
      {
      if (debug == SD_TRUE)
        {
        SLOGCALWAYS1 ("  Variable: '%s'", var->name);
        }
      writeVarInitCode (var,"  ", "AA_SPEC");
      fprintf (codeFp, "\n    *ppva++ = va++;");
  
      sprintf (refBuf, "aa->var_assoc_tbl[%d]", varIndex);
      var->ref = chk_strdup (refBuf);
  
      var = (VAR_CTRL *) list_get_next (aaObjs.varHead, var);
      ++varIndex;
      }
    }
  fprintf (codeFp, "\n    }");
  
  fprintf (codeFp, "\n#endif	/*#if defined(OBSOLETE_AA_OBJ_INIT)*/");
  fprintf (codeFp, "\n  }");
  fprintf (codeFp, "\n");
  }


/************************************************************************/
/*                       writeAAVarListInit                             */
/************************************************************************/

ST_VOID writeAAVarListInit ()
  {
VARLIST_CTRL *varList;

  if (debug == SD_TRUE)
    {
    SLOGALWAYS0 ("WRITING AA VARIABLE LIST INITIALIZATION CODE");
    }

  writeCodeBreak ();
  fprintf (codeFp, "\n/* AA VARIABLE LIST INITIALIZATION */");
  fprintf (codeFp, "\nstatic ST_VOID mvl_init_aa_varLists ()");
  fprintf (codeFp, "\n  {");

  fprintf (codeFp, "\n#if defined(OBSOLETE_AA_OBJ_INIT)");	/*whole funct disabled by default*/

  fprintf (codeFp, "\nMVL_AA_OBJ_CTRL *aa;");
  if (aaObjs.numVarLists)
    fprintf (codeFp, "\nMVL_NVLIST_CTRL *vl;");
  fprintf (codeFp, "\nMVL_NVLIST_CTRL **ppvl;");
  fprintf (codeFp, "\nST_INT i;");
  fprintf (codeFp, "\n");

  fprintf (codeFp, "\n\n/* Now do AA specific Variable Lists */");
  fprintf (codeFp, "\n  for (i = 0; i < mvl_cfg_info->num_called; ++i)");
  fprintf (codeFp, "\n    {");
  fprintf (codeFp, "\n    aa = (MVL_AA_OBJ_CTRL *) mvl_called_conn_ctrl[i].aa_objs;");
  fprintf (codeFp, "\n    aa->foundry_objects = SD_TRUE;");
  fprintf (codeFp, "\n    aa->max_num_nvlist = %d + mvl_max_dyn.aa_nvls;", aaObjs.numVarLists);
  fprintf (codeFp, "\n    aa->num_nvlist = %d;", aaObjs.numVarLists);
  fprintf (codeFp, "\n    if (aa->max_num_nvlist)");
  fprintf (codeFp, "\n      aa->nvlist_tbl = ppvl = (MVL_NVLIST_CTRL **) M_CALLOC (MSMEM_STARTUP, aa->max_num_nvlist, sizeof (MVL_NVLIST_CTRL *));");

  if (aaObjs.numVarLists)
    {
    fprintf (codeFp, "\n    vl = (MVL_NVLIST_CTRL *) M_CALLOC (MSMEM_STARTUP, %d, sizeof (MVL_NVLIST_CTRL));", 
                      aaObjs.numVarLists);
    varList = aaObjs.varListHead;
    while (varList != NULL) 
      {
      if (debug == SD_TRUE)
        {
        SLOGCALWAYS1 ("  Variable List: '%s'", varList->name);
        }
      writeVarListInitCode (varList,"  ", "AA_SPEC");
      fprintf (codeFp, "\n    *ppvl++ = vl++;");
      varList = (VARLIST_CTRL *) list_get_next (aaObjs.varListHead, varList);
      }
    }
  fprintf (codeFp, "\n    }");

  fprintf (codeFp, "\n  for (i = 0; i < mvl_cfg_info->num_calling; ++i)");
  fprintf (codeFp, "\n    {");
  fprintf (codeFp, "\n    aa = (MVL_AA_OBJ_CTRL *) mvl_calling_conn_ctrl[i].aa_objs;");
  fprintf (codeFp, "\n    aa->foundry_objects = SD_TRUE;");
  fprintf (codeFp, "\n    aa->max_num_nvlist = %d + mvl_max_dyn.aa_nvls;", aaObjs.numVarLists);
  fprintf (codeFp, "\n    aa->num_nvlist = %d;", aaObjs.numVarLists);
  fprintf (codeFp, "\n    if (aa->max_num_nvlist)");
  fprintf (codeFp, "\n      aa->nvlist_tbl = ppvl = (MVL_NVLIST_CTRL **) M_CALLOC (MSMEM_STARTUP, aa->max_num_nvlist, sizeof (MVL_NVLIST_CTRL *));");

  if (aaObjs.numVarLists)
    {
    fprintf (codeFp, "\n    vl = (MVL_NVLIST_CTRL *) M_CALLOC (MSMEM_STARTUP, %d, sizeof (MVL_NVLIST_CTRL));", 
                      aaObjs.numVarLists);
    varList = aaObjs.varListHead;
    while (varList != NULL) 
      {
      if (debug == SD_TRUE)
        {
        SLOGCALWAYS1 ("  Variable List: '%s'", varList->name);
        }
      writeVarListInitCode (varList,"  ", "AA_SPEC");
      fprintf (codeFp, "\n    *ppvl++ = vl++;");
      varList = (VARLIST_CTRL *) list_get_next (aaObjs.varListHead, varList);
      }
    }
  fprintf (codeFp, "\n    }");

  fprintf (codeFp, "\n#endif	/*#if defined(OBSOLETE_AA_OBJ_INIT)*/");
  fprintf (codeFp, "\n  }");
  fprintf (codeFp, "\n");
  }


/************************************************************************/
/*                       writeVmdVarInit                                        */
/************************************************************************/

ST_VOID writeVmdVarInit ()
  {
VAR_CTRL *var;
ST_INT varIndex;
ST_CHAR refBuf[100];

  if (debug == SD_TRUE)
    {
    SLOGALWAYS0 ("WRITING VARIABLE INITIALIZATION CODE");
    }

  writeCodeBreak ();
  fprintf (codeFp, "\n/* VMD WIDE NAMED VARIABLE ASSOCIATION INITIALIZATION */");
  fprintf (codeFp, "\n\nstatic ST_VOID mvl_init_vmd_vars ()");
  fprintf (codeFp, "\n  {");
  if (numVmdVars)
    fprintf (codeFp, "\nMVL_VAR_ASSOC *va;");
  fprintf (codeFp, "\nMVL_VAR_ASSOC **ppva;");
  fprintf (codeFp, "\n");

  if (numVmdVars)
    fprintf (codeFp, "\n  va = (MVL_VAR_ASSOC *) M_CALLOC (MSMEM_STARTUP, %d, sizeof (MVL_VAR_ASSOC));", 
                                                numVmdVars);

  fprintf (codeFp, "\n  mvl_vmd.max_num_var_assoc = %d + mvl_max_dyn.vmd_vars;", numVmdVars);
  fprintf (codeFp, "\n  mvl_vmd.num_var_assoc = %d;", numVmdVars);
  fprintf (codeFp, "\n  if (mvl_vmd.max_num_var_assoc)");
  fprintf (codeFp, "\n    mvl_vmd.var_assoc_tbl = ppva = (MVL_VAR_ASSOC **) M_CALLOC (MSMEM_STARTUP, mvl_vmd.max_num_var_assoc, sizeof (MVL_VAR_ASSOC *));");

  varIndex = 0;
  var = vmdVarHead;
  while (var != NULL) 
    {
    if (debug == SD_TRUE)
      {
      SLOGCALWAYS1 ("  Variable: '%s'", var->name);
      }
    writeVarInitCode (var,"", "VMD_SPEC");
    fprintf (codeFp, "\n  *ppva++ = va++;");

    sprintf (refBuf, "mvl_vmd.var_assoc_tbl[%d]", varIndex);
    var->ref = chk_strdup (refBuf);

    var = (VAR_CTRL *) list_get_next (vmdVarHead, var);
    ++varIndex;
    }
  
  fprintf (codeFp, "\n  }");
  }


/************************************************************************/
/*                       writeVarInitCode                               */
/************************************************************************/

static ST_VOID writeVarInitCode (VAR_CTRL *var, ST_CHAR *prefix,
			  ST_CHAR *scopeText)
  {
ST_CHAR *realname;	/* MMS variable name (_UCA_ prefix stripped off)*/
ST_UINT realnamelen;
  realname = var->name;
  if (var->typeCtrl->ucaType && (!strncmp (var->name, "_UCA_", 5)))
    realname += strlen ("_UCA_");	/* skip over prefix	*/
  if ((realnamelen = strlen (realname)) > MAX_IDENT_LEN)
    {
    SLOGALWAYS2 ("Var name '%s' len=%d exceeds maximum len", realname, realnamelen);
    exitApp ();
    }

  if (var->typeCtrl->ucaType == SD_FALSE)
    fprintf (codeFp, "\n%s  va->name = \"%s\";", prefix, var->name);
  else
    {
    if (!strncmp (var->name, "_UCA_", 5))
      fprintf (codeFp, "\n%s  va->name = \"%s\";", prefix, &var->name[5]);
    else
      fprintf (codeFp, "\n%s  va->name = \"%s\";", prefix, var->name);
    fprintf (codeFp, "\n%s  va->flags = MVL_VAR_FLAG_UCA;", prefix);
    }

  fprintf (codeFp, "\n%s  va->type_id = %s%s;", prefix, 
				var->typeCtrl->label, typeSuffix);
  fprintf (codeFp, "\n%s  va->type_ctrl = &mvl_type_ctrl[%s%s];", prefix, 
				var->typeCtrl->label, typeSuffix);

  if (var->initText)
    {
    fprintf (codeFp, "\n%s  va->data = %s;", prefix, var->initText);
    if (mvl_uca)
      fprintf (codeFp, "\n%s  va->use_static_data = SD_TRUE;", prefix);
    }

  if (var->procText)
    fprintf (codeFp, "\n%s  va->proc = %s;", prefix, var->procText);

  if (var->usrText)
    fprintf (codeFp, "\n%s  va->user_info = (ST_VOID *) %s;", prefix, var->usrText);
  }


/************************************************************************/
/*                       writeVmdVarListInit                                    */
/************************************************************************/

ST_VOID writeVmdVarListInit ()
  {
VARLIST_CTRL *varList;

   if (debug == SD_TRUE)
     {
     SLOGALWAYS0 ("WRITING VARIABLE LIST INITIALIZATION CODE");
     }

  writeCodeBreak ();
  fprintf (codeFp, "\n/* NAMED VARIABLE LIST INITIALIZATION */");
  fprintf (codeFp, "\n\nstatic ST_VOID mvl_init_vmd_varLists ()");
  fprintf (codeFp, "\n  {");
  if (numVmdVarLists)
    fprintf (codeFp, "\nMVL_NVLIST_CTRL *vl;");
  fprintf (codeFp, "\nMVL_NVLIST_CTRL **ppvl;");
  fprintf (codeFp, "\n");

  fprintf (codeFp, "\n/* Do VMD wide variable lists */");
  if (numVmdVarLists)
    fprintf (codeFp, "\n  vl = (MVL_NVLIST_CTRL *) M_CALLOC (MSMEM_STARTUP, %d, sizeof (MVL_NVLIST_CTRL));", 
                                                numVmdVarLists);

  fprintf (codeFp, "\n  mvl_vmd.max_num_nvlist = %d + mvl_max_dyn.vmd_nvls;", numVmdVarLists);
  fprintf (codeFp, "\n  mvl_vmd.num_nvlist = %d;", numVmdVarLists);
  fprintf (codeFp, "\n  if (mvl_vmd.max_num_nvlist)");
  fprintf (codeFp, "\n    mvl_vmd.nvlist_tbl = ppvl = (MVL_NVLIST_CTRL **) M_CALLOC (MSMEM_STARTUP, mvl_vmd.max_num_nvlist, sizeof (MVL_NVLIST_CTRL *));");

  varList = vmdVarListHead;
  while (varList != NULL) 
    {
    if (debug == SD_TRUE)
      {
      SLOGCALWAYS1 ("  Variable List: '%s'", varList->name);
      }
    writeVarListInitCode (varList,"", "VMD_SPEC");
    fprintf (codeFp, "\n  *ppvl++ = vl++;");
    varList = (VARLIST_CTRL *) list_get_next (vmdVarListHead, varList);
    }

  fprintf (codeFp, "\n  }");
  }


/************************************************************************/
/*                       writeVarListInitCode                           */
/************************************************************************/

static ST_VOID writeVarListInitCode (VARLIST_CTRL *varList, ST_CHAR *prefix,
			     ST_CHAR *nvlScopeString)

  {
VAR_CTRL *var;
ST_INT i;
ST_CHAR *vaScopeString;
ST_UINT namelen;	/* len of NVL name	*/

  if ((namelen = strlen (varList->name)) > MAX_IDENT_LEN)
    {
    SLOGALWAYS2 ("NVL name '%s' len=%d exceeds maximum len", varList->name, namelen);
    exitApp ();
    }
  fprintf (codeFp, "\n%s  vl->name = \"%s\";", prefix, varList->name);
    
  fprintf (codeFp, "\n%s  vl->num_of_entries = %d;", prefix, varList->numVars);
  fprintf (codeFp, "\n%s  vl->entries = (MVL_VAR_ASSOC **) M_CALLOC (MSMEM_STARTUP, %d, sizeof (MVL_VAR_ASSOC *));", 
                                                prefix, varList->numVars);
  fprintf (codeFp, "\n%s  vl->va_scope = (MVL_SCOPE *) M_CALLOC (MSMEM_STARTUP, %d, sizeof (MVL_SCOPE));", 
                                                prefix, varList->numVars);

  fprintf (codeFp, "\n%s  vl->nvl_scope.scope = %s;", prefix, nvlScopeString);
  if (!strcmp (nvlScopeString, "DOM_SPEC"))
    fprintf (codeFp, "\n%s  vl->nvl_scope.dom = dom;", prefix);

  for (i = 0; i < varList->numVars; ++i)
    {
    var = varList->varTable[i];

    if (var->scope == VMD_SPEC)
      vaScopeString = "VMD_SPEC";
    else if (var->scope == DOM_SPEC)
      vaScopeString = "DOM_SPEC";
    else 
      vaScopeString = "AA_SPEC";

    if (var->nvlOnly == SD_FALSE)
      {
      assert (var->ref != NULL);	/* Can't be NULL in this case	*/
      if (debug)
        SLOGCALWAYS2 ("    Variable: '%s', reference '%s'", var->name, var->ref);

      fprintf (codeFp, "\n%s  vl->entries[%d] = %s;\t/* %s\t*/", 
                                prefix, i, var->ref, var->name);
      }
    else /* This is not a real VA, create one! */
      {
      /* NOTE: In this case, var->ref == NULL, so don't try to log it.	*/
      if (debug)
        SLOGCALWAYS1 ("    Variable: '%s'", var->name);

      /* Generate code to fill in "vl->entries".	*/
      fprintf (codeFp, "\n%s  {",
               prefix);
      fprintf (codeFp, "\n%s  OBJECT_NAME varObjName;",
               prefix);
      fprintf (codeFp, "\n%s  varObjName.obj_name.vmd_spec = \"%s\";",
               prefix, var->name);
      fprintf (codeFp, "\n%s  varObjName.object_tag = %s;",
               prefix, vaScopeString);
      if (var->scope == DOM_SPEC)
        fprintf (codeFp, "\n%s  varObjName.domain_id = %s->name;",
                 prefix, var->domRef);
      fprintf (codeFp, "\n%s  vl->entries [%d] = u_mvl_get_va_aa (&mvl_vmd, MMSOP_INFO_RPT, &varObjName, NULL, SD_FALSE, NULL, NULL);",
               prefix, i);
      fprintf (codeFp, "\n%s  assert (vl->entries[%d]!=NULL);",
               prefix, i);
      fprintf (codeFp, "\n%s  }",
               prefix);
      }

  /* Write the scope information element */
    fprintf (codeFp, "\n%s  vl->va_scope[%d].scope = %s;", 
                              prefix, i, vaScopeString);
    if (var->domRef != NULL)
      fprintf (codeFp, "\n%s  vl->va_scope[%d].dom = %s;", prefix, i, var->domRef);
    }
  }

/************************************************************************/
/*                       writeTypeCtrl                                  */
/************************************************************************/

static ST_VOID writeTypeCtrl ()
  {
TYPE_CTRL *current_type;
ST_INT mms_index;
ST_INT rt_index;
ST_INT i;
ST_INT col;
ST_INT tdlLen;
MVLU_FOUNDRY_TYPE_INFO *mvluFoundryInfo;
RUNTIME_TYPE *rt;

  writeCodeBreak ();

#ifdef USE_RUNTIME_CTRL
/* NOTE: may use this code someday if "rt" & "num_rt" replaced with	*/
/*      "RUNTIME_CTRL *rt_ctrl" in "MVL_TYPE_CTRL".			*/
/* NOTE: Need forward refs because RUNTIME_TYPE array generated AFTER mvl_init_type_ctrl*/
  fprintf (codeFp, "\n/* forward references	*/");
  for (i=0; i<typeCount; i++)
    {
    fprintf (codeFp, "\nSD_CONST static RUNTIME_CTRL mvl_rt_ctrl_%d;",i);
    }
#endif	/* USE_RUNTIME_CTRL*/
    
  fprintf (codeFp, "\nST_VOID mvl_init_type_ctrl ()\n  {\n");
  /* Dummy } to fool dumb brace-matchers and match previous brace. */

  fprintf (codeFp, "static ST_BOOLEAN _mvlInitTypeCtrlCalled = SD_FALSE;\n");
  if (useRtInitCode == SD_TRUE)
    fprintf (codeFp, "RUNTIME_TYPE *rt_types;\n\n");

  fprintf (codeFp, "\n");
  fprintf (codeFp, "  if (_mvlInitTypeCtrlCalled == SD_TRUE)\n");
  fprintf (codeFp, "    return;\n");
  fprintf (codeFp, "  _mvlInitTypeCtrlCalled = SD_TRUE;\n");
  fprintf (codeFp, "\n");


  fprintf (codeFp, "#ifndef USE_RT_TYPE_2\n");
  fprintf (codeFp, "  maxMvlRtNames = %d;\n", num_mvl_rt_names);
  fprintf (codeFp, "  numMvlRtNames = %d;\n", num_mvl_rt_names);
  fprintf (codeFp, "  mvlRtNames = foMvlRtNames;\n");
  fprintf (codeFp, "#endif\n");
  fprintf (codeFp, "  mvl_num_types = %d + mvl_max_dyn.types;\n", typeCount);
  fprintf (codeFp, "\n  if (mvl_num_types)");
  fprintf (codeFp, "\n    mvl_type_ctrl = (MVL_TYPE_CTRL *) M_CALLOC (MSMEM_STARTUP, mvl_num_types, sizeof(MVL_TYPE_CTRL));\n");

  fprintf (codeFp,"#if defined USR_SUPPLIED_RT\n");
  fprintf (codeFp,"  u_mvl_start_init_rt_tbl (%d, %d);\n", typeCount, totalRtCount);
  if (useRtInitCode == SD_TRUE)
    {
    fprintf (codeFp,"#else\n");
    fprintf (codeFp,"  rt_types = chk_calloc(%d, sizeof(RUNTIME_TYPE));\n", totalRtCount);
    }
  fprintf (codeFp,"#endif\n");

  fprintf (codeFp, "\n");

/* Now, start at the beginning of the chain of defined types       */
/* elements and print code to initialize MVL_TYPE_CTRL array       */
/* entries to the values defined by the user input.                */

  SLOGALWAYS0 ("WRITING RUNTIME TYPES");
  rt_index = 0;
  mms_index = 0;
  current_type = typeHead;
  while (current_type != NULL)
    {
    if (current_type->ucaType && current_type->rt_ctrl->rt_first->el_tag != RT_STR_START)
      {
      printf ("!!! WARNING: IEC/UCA type named '%s' is not a struct. ILLEGAL.\n",
              current_type->label);
      SLOGALWAYS1 ("!!! WARNING: IEC/UCA type named '%s' is not a struct. ILLEGAL.",
              current_type->label);
      }

    ucaType = current_type->ucaType;
    if (debug == SD_TRUE)
      {
      SLOGCALWAYS2 ("  Writing type '%s' (%d ref)", 
                        current_type->label, current_type->baseRefCount);
      }

    fprintf (codeFp, "/* %s : %s */\n",current_type->label, current_type->desc);
#ifdef OBSOLETE_TWINNING_ENABLE		/* DEBUG: delete later?	*/
    if (current_type->twin)
      {
      if (debug == SD_TRUE)
        {
        SLOGCALWAYS0 ("    Twin! No code"); 
        }
      fprintf (codeFp, "/* %s is our twin */\n\n",current_type->twin->label);
      current_type = (TYPE_CTRL *) list_get_next (typeHead, current_type);
      continue;
      }
#endif

    tdlLen = strlen (current_type->tdl);
    fprintf (codeFp, "/*\n");
    fprintf (codeFp, "  mvl_type_ctrl[%s%s].tdl = \n", 
			current_type->label, typeSuffix);
    fprintf (codeFp, "  \"");
    col = 1;
    for (i = 0; i < tdlLen; ++i, ++col)
      {
      fprintf (codeFp, "%c", current_type->tdl[i]);
      if (col > 71)
        {
        fprintf (codeFp, "\\\n");
        col = 0;
        }
      }
    fprintf (codeFp, "\";\n");
    fprintf (codeFp, "*/\n");

#ifdef USE_RUNTIME_CTRL
/* NOTE: may use this code someday if "rt" & "num_rt" replaced with	*/
/*      "RUNTIME_CTRL *rt_ctrl" in "MVL_TYPE_CTRL".			*/
    fprintf (codeFp, "  mvl_type_ctrl[%s%s].rt_ctrl = &mvl_rt_ctrl_%d;\n",
                 current_type->label, typeSuffix, mms_index);
#endif	/* USE_RUNTIME_CTRL*/

    fprintf (codeFp, "  mvl_type_ctrl[%s%s].num_rt = %d;\n",
                 current_type->label, typeSuffix, current_type->rt_ctrl->rt_num);
  
    fprintf (codeFp, "  mvl_type_ctrl[%s%s].data_size = %ld;\n",
                           current_type->label, typeSuffix,
                           (ST_LONG) current_type->rt_ctrl->rt_first->offset_to_last);

    fprintf (codeFp, "#if defined USR_SUPPLIED_RT\n");
    fprintf (codeFp, "  mvl_type_ctrl[%s%s].rt = u_mvl_get_rt_tbl (%s%s, %d);\n",
              	current_type->label, typeSuffix, 
		current_type->label, typeSuffix, 
		current_type->rt_ctrl->rt_num);
    fprintf (codeFp,"#else\n");


#if 0
  /* For UCA types: sort names, set offset, calc size */
    if (mvl_uca == SD_TRUE)
      mvlu_proc_rt_type (current_type->label, 
      			 current_type->rt_ctrl, NULL);
#endif

    rt = current_type->rt_ctrl->rt_first;
    mvluFoundryInfo = current_type->mvluFoundryInfo;
    for (i = 0; i < current_type->rt_ctrl->rt_num; ++i, ++rt, ++mvluFoundryInfo)
      {
      if (mvl_uca == SD_TRUE && ucaType && ms_comp_name_pres(rt))
        deriveUcaRtInfo (current_type, rt, mvluFoundryInfo, rt_index+i);
      }


    if (useRtInitCode == SD_TRUE)
      {
      fprintf (codeFp, "  mvl_type_ctrl[%s%s].rt = rt_types + %d;\n",
                        current_type->label, typeSuffix, rt_index);
      fprintf (codeFp, "    {");

      mvluFoundryInfo = current_type->mvluFoundryInfo;
      rt = current_type->rt_ctrl->rt_first;
      for (i = 0; i < current_type->rt_ctrl->rt_num; ++i, ++rt_index, ++rt,
					    ++mvluFoundryInfo)
        {
        writeRtEl (current_type, rt, mvluFoundryInfo, rt_index, (i==0));
        }
      fprintf (codeFp, "\n    }");
      }
    else
      {
      fprintf (codeFp, "  mvl_type_ctrl[%s%s].rt = mvl_rt_tables[rt_table_index++];",
              	current_type->label, typeSuffix); 
      }
    fprintf (codeFp, "\n#endif /* #if defined USR_SUPPLIED_RT */\n");

    fprintf (codeFp, "  strcpy (mvl_type_ctrl[%s%s].type_name, \"%s\");\n",
		current_type->label, typeSuffix, current_type->label);
    fprintf (codeFp, "\n");	/* Newline before next entry.	*/

    current_type = (TYPE_CTRL *) list_get_next (typeHead, current_type);
    ++mms_index;
    }		     

  fprintf (codeFp,"\n#if defined USR_SUPPLIED_RT\n");
  fprintf (codeFp,"  u_mvl_end_init_rt_tbl ();\n");
  fprintf (codeFp,"#endif\n");

/* We are all done printing the runtime table.                          */
/* Dummy { to fool dumb brace-matchers and match following brace        */

  fprintf (codeFp, "  }\n");

/* Add "dynamic leaf functions" (see addDynLeafFuns description for 	*/
/* details).								*/
/* NOTE: only works if leaf input file is XML (not TFN).		*/
  if (mvlu_use_leaf_file)
    addDynLeafFuns ();
 
/* Generate the RUNTIME_TYPE tables (mvl_rt_table_*).			*/
/* NOTE: this comes AFTER generating mvl_init_type_ctrl because some	*/
/*       RUNTIME_TYPE info is filled in by deriveUcaRtInfo call above.	*/
  if (useRtInitCode == SD_FALSE)
    writeRtData ();
  }

/************************************************************************/
/*                       writeJournalInit                             	*/
/************************************************************************/

static ST_VOID writeJournalInit ()
  {
JOU_CTRL *jou;
DOM_JOU_CTRL *djou;
DOMAIN_CTRL *dom;
ST_INT curDomNumJou;
ST_UINT namelen;	/* len of Journal name	*/

  if (debug == SD_TRUE)
    {
    SLOGALWAYS0 ("WRITING JOURNAL INITIALIZATION CODE");
    }

  writeCodeBreak ();
  fprintf (codeFp, "\n/* JOURNAL INITIALIZATION */");
  fprintf (codeFp, "\nstatic ST_VOID mvl_init_journals ()");
  fprintf (codeFp, "\n  {");
  if (numJou)
    fprintf (codeFp, "\nMVL_JOURNAL_CTRL *jou;");
  fprintf (codeFp, "\nMVL_JOURNAL_CTRL **ppjou;");
  if (numDoms)
    fprintf (codeFp, "\nMVL_DOM_CTRL *dom;");
  fprintf (codeFp, "\n");

  fprintf (codeFp, "\n  /* initialize VMD_SPEC journals */\n");
  if (numJou)
    fprintf (codeFp, "\n  jou = (MVL_JOURNAL_CTRL *) M_CALLOC (MSMEM_STARTUP, %d, sizeof (MVL_JOURNAL_CTRL));", 
                                        numJou);
  
  fprintf (codeFp, "\n  mvl_vmd.max_num_jou = %d + mvl_max_dyn.journals;", numJou);
  fprintf (codeFp, "\n  mvl_vmd.num_jou = %d;", numJou);
  fprintf (codeFp, "\n  if (mvl_vmd.max_num_jou)");
  fprintf (codeFp, "\n    mvl_vmd.jou_tbl = ppjou = (MVL_JOURNAL_CTRL **) M_CALLOC (MSMEM_STARTUP, mvl_vmd.max_num_jou, sizeof (MVL_JOURNAL_CTRL *));");
  
  jou = jouHead;
  while (jou != NULL) 
    {
    if (debug == SD_TRUE)
      {
      SLOGCALWAYS1 ("  Journal: '%s'", jou->name);
      }
    if ((namelen = strlen (jou->name)) > MAX_IDENT_LEN)
      {
      SLOGALWAYS2 ("Journal name '%s' len=%d exceeds maximum len", jou->name, namelen);
      exitApp ();
      }
    fprintf (codeFp, "\n  jou->name = \"%s\";", jou->name);
    fprintf (codeFp, "\n  *ppjou++ = jou++;");
    jou = (JOU_CTRL *) list_get_next (jouHead, jou);
    }

  /* now handle the configured DOM_SPEC journals */
  /* for each domain we need to allocate jou_tbl[n+mvl_max_dyn.journals]	*/

  if (numDoms)
    fprintf (codeFp, "\n\n  /* initialize DOM_SPEC journals */");
  dom = domHead;
  while (dom != NULL) 
    {
    fprintf (codeFp, "\n");
    /* count the number of journals for current domain */
    curDomNumJou = 0;
    djou = djouHead;
    while (djou != NULL)
      {
      if (strcmp(djou->domName, dom->name) == 0)
        {
        ++curDomNumJou;
        djou->processed = SD_TRUE;
        }
      djou = (DOM_JOU_CTRL *) list_get_next (djouHead, djou);
      }

    fprintf (codeFp, "\n  dom = mvl_vmd_find_dom (&mvl_vmd, \"%s\");", dom->name);
    fprintf (codeFp, "\n  if (dom != NULL)");
    fprintf (codeFp, "\n    {");
    if (curDomNumJou)
      fprintf (codeFp, "\n    MVL_JOURNAL_CTRL *djou = (MVL_JOURNAL_CTRL *) M_CALLOC (MSMEM_STARTUP, %d, sizeof (MVL_JOURNAL_CTRL));", 
                       curDomNumJou);
    fprintf (codeFp, "\n    dom->max_num_jou = %d + mvl_max_dyn.journals;", curDomNumJou);
    fprintf (codeFp, "\n    dom->num_jou = %d;", curDomNumJou);
    fprintf (codeFp, "\n    if (dom->max_num_jou)");
    fprintf (codeFp, "\n      dom->jou_tbl = ppjou = (MVL_JOURNAL_CTRL **) M_CALLOC (MSMEM_STARTUP, dom->max_num_jou, sizeof (MVL_JOURNAL_CTRL *));");

    djou = djouHead;
    while (djou != NULL) 
      {
      /* take only the journals that are for current domain */
      if (strcmp(djou->domName, dom->name) == 0)
        {
        if (debug == SD_TRUE)
          {
          SLOGCALWAYS2 ("  DOM_SPEC Journal: '%s:%s'", djou->domName, djou->name);
          }
        fprintf (codeFp, "\n    djou->name = \"%s\";", djou->name);
        fprintf (codeFp, "\n    *ppjou++ = djou++;");
        }
      djou = (DOM_JOU_CTRL *) list_get_next (djouHead, djou);
      }  
    fprintf (codeFp, "\n    }");
    dom = (DOMAIN_CTRL *) list_get_next (domHead, dom);
    }

  fprintf (codeFp, "\n  }");
  fprintf (codeFp, "\n");

  /* check for unprocesses DOM_SPEC journals (when domain not found) */
  djou = djouHead;
  while (djou != NULL)
    {
    if (djou->processed == SD_FALSE)
      {
      SLOGALWAYS2 ("Error: domain not found for journal '%s:%s'", 
                    djou->domName, djou->name);
      exitApp ();
      }
    djou = (DOM_JOU_CTRL *) list_get_next (djouHead, djou);
    }

  }

/************************************************************************/
/************************************************************************/
/*                       writeTypedefs					*/
/************************************************************************/

ST_VOID writeTypedefs ()
  {
TYPE_CTRL *current_type;
ST_CHAR *typedefBuf;
ST_RET rc;
ST_CHAR typedefName[200];

  typedefBuf = (ST_CHAR *) chk_calloc (1, MAX_TYPEDEF_SIZE);
  fprintf (headerFp, "\n%s",commentBar);
  fprintf (headerFp, "\n/* TYPEDEFS for MMS TYPES	                                              */");
  fprintf (headerFp, "\n%s",commentBar);
  fprintf (headerFp, "\n");

  if (noTypedefs == SD_FALSE)
    {
    fprintf (headerFp, "\n/* Use \":CF\", \"-notypedefs\" in the ODF file to not include this line */");
    fprintf (headerFp, "\n#define USE_MMS_TYPEDEFS");
    fprintf (headerFp, "\n");
    }

  fprintf (headerFp, "\n#if defined (USE_MMS_TYPEDEFS)");
  fprintf (headerFp, "\n");

  current_type = typeHead;
  while (current_type != NULL)
    {
    strcpy (typedefName, current_type->label);
    strcat (typedefName, "_TDEF");

/* Cover bug ... */
    typedefBuf[0] = 0;
    rc = ms_rt_to_typedef (typedefName, current_type->rt_ctrl->rt_first,
		     current_type->rt_ctrl->rt_num, typedefBuf, MAX_TYPEDEF_SIZE);
    if (rc == SD_SUCCESS)
      {
      fprintf (headerFp, "\n%s\n", typedefBuf); 
      }
    current_type = (TYPE_CTRL *) list_get_next (typeHead, current_type);
    }

/* Take care of 'typedef only types */
  current_type = typedefOnlyHead;
  while (current_type != NULL)
    {
    strcpy (typedefName, current_type->label);
    strcat (typedefName, "_TDEF");

/* Cover bug ... */
    typedefBuf[0] = 0;
    rc = ms_rt_to_typedef (typedefName, current_type->rt_ctrl->rt_first,
		     current_type->rt_ctrl->rt_num, typedefBuf, MAX_TYPEDEF_SIZE);
    if (rc == SD_SUCCESS)
      {
      fprintf (headerFp, "\n%s\n", typedefBuf); 
      }
    current_type = (TYPE_CTRL *) list_get_next (typedefOnlyHead, current_type);
    }


  chk_free (typedefBuf);

  fprintf (headerFp, "\n%s",commentBar);
  fprintf (headerFp, "\n#endif /* if defined (USE_MMS_TYPEDEFS) */");
  fprintf (headerFp, "\n%s",commentBar);

  }

/************************************************************************/
/*                       writeHeader                                    */
/************************************************************************/
/*      Write the .h file for the C function, to define the indices into*/
/*      the table for the various user labels and the other external    */
/*      access information required by user programs.  Notice that the  */
/*      tables can be allocated either statically or dynamically.       */

static ST_VOID writeHeader ()
  {
ST_INT i;
TYPE_CTRL *type_ptr;
FILE *fp;

  fp = headerFp;
  fprintf (fp, "/*  This file created from input file '%s'\n", inFileName);
  fprintf (fp, "    Leaf Access Parameter (LAP) File: '%s'\n", 
      useTemplate || mvlu_use_leaf_file ? templateInFileName : "Not Used");
  fprintf (fp, "\tCreated %s", ctime (&rightnow));
  fprintf (fp, "*/\n");

  fprintf (fp, "\n#ifndef FOUNDRY_OUTPUT_HEADER_INCLUDED");
  fprintf (fp, "\n#define FOUNDRY_OUTPUT_HEADER_INCLUDED");
  fprintf (fp, "\n");
  fprintf (fp, "\n#include \"mvl_uca.h\"");
  fprintf (fp, "\n");
  fprintf (fp, "\n#ifdef __cplusplus");
  fprintf (fp, "\nextern \"C\" {");
  fprintf (fp, "\n#endif");


  fprintf (fp, "\n\n%s", commentBar);
  fprintf (fp, "\n/* MVL Type Control Information */");
  fprintf (fp, "\nextern MVL_TYPE_CTRL *mvl_type_ctrl;");
  fprintf (fp, "\nextern ST_INT mvl_num_types;");

  fprintf (fp, "\n\n%s", commentBar);
  fprintf (fp, "\n");

  fprintf (fp, "\n");
  fprintf (fp, "\n/* MVL Type ID's */");
  fprintf (fp, "\n");

  type_ptr = typeHead;
  i = 0;
  while (type_ptr != NULL)
    {
#ifdef OBSOLETE_TWINNING_ENABLE		/* DEBUG: delete later?	*/
    if (type_ptr->twin)
      {
      fprintf (fp, "#define %s%s\t\t%s%s\n", type_ptr->label, typeSuffix,  
				   type_ptr->twin->label, typeSuffix);
      }
    else
#endif
      {
      fprintf (fp, "#define %s%s\t\t%d\n", type_ptr->label, typeSuffix, i);
      ++i;
      }
    type_ptr = (TYPE_CTRL *) list_get_next (typeHead, type_ptr);
    }

  fprintf (fp, "\n");
  fprintf (fp, "\n%s", commentBar);
  fprintf (fp, "\n");
  }


/************************************************************************/
/*			writeFooter 					*/
/************************************************************************/

static ST_VOID writeFooter ()
  {
FILE *fp;

  fp = headerFp;

  fprintf (fp, "\n#ifdef __cplusplus");
  fprintf (fp, "\n}");
  fprintf (fp, "\n#endif");
  fprintf (fp, "\n");
  fprintf (fp, "\n#endif /* #ifndef FOUNDRY_OUTPUT_HEADER_INCLUDED */");
  fprintf (fp, "\n");	/* LINUX likes newline at end of file	*/
  }

/************************************************************************/
/*			get_el_tag_text					*/
/************************************************************************/
/* Array of structs to map element tag defines to text.	*/
typedef struct
  {
  ST_INT define_val;
  ST_CHAR *text;
  } EL_TAG_MAP;
EL_TAG_MAP el_tag_map[] = 
  {
  {RT_ARR_START,      "RT_ARR_START"},
  {RT_STR_START,      "RT_STR_START"},
  {RT_BOOL,           "RT_BOOL"},
  {RT_BIT_STRING,     "RT_BIT_STRING"},
  {RT_INTEGER,        "RT_INTEGER"},
  {RT_UNSIGNED,       "RT_UNSIGNED"},
  {RT_FLOATING_POINT, "RT_FLOATING_POINT"},
  {RT_OCTET_STRING,   "RT_OCTET_STRING"},
  {RT_VISIBLE_STRING, "RT_VISIBLE_STRING"},
  {RT_GENERAL_TIME,   "RT_GENERAL_TIME"},
  {RT_BINARY_TIME,    "RT_BINARY_TIME"},
  {RT_BCD,            "RT_BCD"},
  {RT_BOOLEANARRAY,   "RT_BOOLEANARRAY"},
  {RT_UTC_TIME,       "RT_UTC_TIME"},
  {RT_UTF8_STRING,    "RT_UTF8_STRING"},
  {RT_STR_END,        "RT_STR_END"},
  {RT_ARR_END,        "RT_ARR_END"},
  };

ST_CHAR *get_el_tag_text (RUNTIME_TYPE *rt_el_ptr)
  {
ST_CHAR *el_tag_text = NULL;
ST_INT j;

  /* Convert el_tag to text. If conversion fails, log and exit.	*/
  for (j = 0;  j < (sizeof(el_tag_map)/sizeof(EL_TAG_MAP));  j++)
    {
    if (rt_el_ptr->el_tag == el_tag_map[j].define_val)
      el_tag_text = el_tag_map[j].text;
    }
  return (el_tag_text);
  }

/************************************************************************/
/*                       writeRtEl                                      */
/************************************************************************/
/*      Print the code to initialize a runtime element                  */

static ST_VOID writeRtEl (TYPE_CTRL *typeCtrl, 
                   RUNTIME_TYPE *rt_el_ptr, 
		   MVLU_FOUNDRY_TYPE_INFO *mvluFoundryInfo,
		   ST_INT rt_index, ST_INT first)
  {
FILE *fp;
ST_INT stringIndex;
ST_CHAR *el_tag_text;

  /* Convert el_tag to text. If conversion fails, log and exit.	*/
  el_tag_text = get_el_tag_text (rt_el_ptr);
  if (el_tag_text == NULL)
    {
    SLOGALWAYS1 ("Internal error, unknown runtime type asn1_tag (%d)!\n",
                 rt_el_ptr->el_tag);
    exitApp ();
    }
  
  fp = codeFp;
  fprintf (fp, "\n    rt_types[%d].el_tag = %s;", 
				rt_index,
        			el_tag_text);

  if (rt_el_ptr->el_size == 0)
    fprintf (fp, "\n /* rt_types[%d].el_size = %d; */",
                              rt_index, rt_el_ptr->el_size);
  else        
    fprintf (fp, "\n    rt_types[%d].el_size = %d;",
                          rt_index, rt_el_ptr->el_size);

  if (first || mvl_uca == SD_TRUE)
    {
    fprintf (fp, "\n    rt_types[%d].offset_to_last = %ld;",
        rt_index, (ST_LONG) rt_el_ptr->offset_to_last);
    }

  switch (rt_el_ptr->el_tag)
    {
    case RT_ARR_START:
    case RT_ARR_END:
      fprintf (fp, "\n    rt_types[%d].u.arr.num_elmnts = %d;", rt_index,
        rt_el_ptr->u.arr.num_elmnts);
      fprintf (fp, "\n    rt_types[%d].u.arr.num_rt_blks = %d;", rt_index,
        rt_el_ptr->u.arr.num_rt_blks);
    break;

    case RT_STR_START:
    case RT_STR_END:
      fprintf (fp, "\n    rt_types[%d].u.str.num_rt_blks = %d;", rt_index,
        (int)rt_el_ptr->u.str.num_rt_blks);
    break;

  /*  Primitive types.                                                  */
    case RT_BOOL:
    case RT_BIT_STRING:
    case RT_INTEGER:
    case RT_UNSIGNED:
    case RT_FLOATING_POINT:
    case RT_OCTET_STRING:
    case RT_VISIBLE_STRING:
    case RT_GENERAL_TIME:
    case RT_BINARY_TIME:
    case RT_BCD:
    case RT_BOOLEANARRAY:
    case RT_UTC_TIME:
    case RT_UTF8_STRING:
      fprintf (fp, "\n    rt_types[%d].u.p.el_len = %d;", 
                          rt_index, rt_el_ptr->u.p.el_len);
    break;

    default:
      SLOGALWAYS1 ("Internal error, unknown runtime type asn1_tag 0x%02x!\n",
                                rt_el_ptr->el_tag);
      exitApp ();
    break;
    }


/* Take care of the named component ...                                 */
  if (rt_el_ptr->name[0])
    {
    stringIndex = getCommonStringIndex (rt_el_ptr->name);
    if (stringIndex != -1)
      {
      fprintf (fp, "\n    rt_types[%d].name_index = %s; /* \"%s\" */",
                                rt_index, 
    			    	stringIndexToDefine (stringIndex),
    			    	mvl_rt_names[stringIndex]);
      }
    else
      {
      SLOGALWAYS0 ("Internal error, string index not found");
      exitApp ();
      }
    }

  if (mvl_uca == SD_TRUE)
    {
    if (rt_el_ptr->name[0])
      writeUcaRtInfo (typeCtrl, rt_el_ptr, rt_index);
    }
  }

/************************************************************************/
/*                      writeCodeBreak                                  */
/************************************************************************/

ST_VOID writeCodeBreak ()
  {
  fprintf (codeFp, "\n");
  fprintf (codeFp, "/************************************************************************/");
  fprintf (codeFp, "\n");
  }



/************************************************************************/
/************************************************************************/
/* COMMON STRING MANAGEMENT                                             */
/************************************************************************/
/************************************************************************/

static ST_VOID createMvlRtNames ()
  {
TYPE_CTRL *current_type;
RUNTIME_TYPE *rt;
ST_INT i;

  current_type = typeHead;
  while (current_type != NULL)
    {
    rt = current_type->rt_ctrl->rt_first;
    for (i = 0; i < current_type->rt_ctrl->rt_num; ++i, ++rt)
      {
      if (ms_comp_name_pres(rt))
        addCommonString (ms_comp_name_find(rt));
      }
    current_type = (TYPE_CTRL *) list_get_next (typeHead, current_type);
    }
  }

/************************************************************************/
/*                       addCommonString                             	*/
/************************************************************************/

static ST_VOID addCommonString (ST_CHAR *str)
  {
ST_INT i;

  for (i = 0; i < num_mvl_rt_names; ++i)
    {
    if (!strcmp (str, mvl_rt_names[i]))
      return;
    }

/* We need to create a new common string entry */
  if (num_mvl_rt_names >= MAX_COMMON_STRINGS)
    {
    SLOGALWAYS0 ("Error: common string buffer overrun");
    exitApp ();
    }

  if (num_mvl_rt_names == 0)	/* Dummy placeholder */
    {
    mvl_rt_names[num_mvl_rt_names] = (ST_CHAR *) chk_calloc (1, 1);
    ++num_mvl_rt_names;
    }

  mvl_rt_names[num_mvl_rt_names] = chk_strdup (str);
  ++num_mvl_rt_names;
  }

/************************************************************************/
/*                       getCommonStringIndex                           */
/************************************************************************/

ST_INT getCommonStringIndex (ST_CHAR *str)
  {
ST_INT i;

  for (i = 0; i < num_mvl_rt_names; ++i)
    {
    if (!strcmp (str, mvl_rt_names[i]))
      return (i);
    }
  return (-1);
  }

/************************************************************************/
/*			stringIndexToDefine				*/
/************************************************************************/

ST_CHAR *stringIndexToDefine (ST_INT stringIndex)
  {
static ST_CHAR defineBuf[200];
ST_INT nameLen;
ST_INT i;

  strcpy (defineBuf, "FO_");
  strcat (defineBuf, mvl_rt_names[stringIndex]);
  nameLen = (ST_INT) strlen (defineBuf);

  if (caseSensStrIdxs == SD_FALSE)
    {
    for (i = 0; i < nameLen; ++i)
      {
      if (islower (defineBuf[i]))
        defineBuf[i] = toupper (defineBuf[i]);
      }
    }
  strcat (defineBuf, "_STRING_INDEX");
  return (defineBuf);
  }


/************************************************************************/
/************************************************************************/
/*                       checkExist                                     */
/************************************************************************/

static ST_VOID checkExist (ST_CHAR *fn)
  {
FILE *handle;

  if (fileOverwrite == SD_TRUE) 
    return;

  handle = fopen (fn, "r");		/* Attempt to open for reading  */
  if (handle == NULL)
    return;

  fclose (handle);
  printf ("Output file '%s' exists, to overwrite the file Foundry must be called with '-o' option.", fn);
  exit (1);
  }


