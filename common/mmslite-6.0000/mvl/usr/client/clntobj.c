
/************************************************************************/
/*  This file created from input file '..\..\mvl\usr\client\clntobj.odf'
    Leaf Access Parameter (LAP) File: 'Not Used'
	Created Mon Feb 25 19:13:22 2019
*/

#include "glbtypes.h"
#include "sysincs.h"
#include "mmsdefs.h"
#include "mms_pvar.h"
#include "mms_vvar.h"
#include "mvl_acse.h"
#include "clntobj.h"

#include "client.h"	/* User Specified */

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__; 
#endif


/************************************************************************/
MVL_TYPE_CTRL *mvl_type_ctrl;
ST_INT mvl_num_types;
MVL_VMD_CTRL mvl_vmd;

/************************************************************************/

/* Init alignment table according to 'align.cfg'	*/
ST_INT m_lite_data_algn_tbl [NUM_ALGN_TYPES] = /* Data Alignment Table*/
  {
  0x0000,  /* ARRSTRT_ALGN   00  */
  0x0000,  /* ARREND_ALGN    01  */
  0x0000,  /* STRSTRT_ALGN   02  */
  0x0000,  /* STREND_ALGN    03  */
  0x0000,  /* INT8_ALGN      04  */
  0x0001,  /* INT16_ALGN     05  */
  0x0003,  /* INT32_ALGN     06  */
  0x0007,  /* INT64_ALGN     07  */
  0x0003,  /* FLOAT_ALGN     08  */
  0x0007,  /* DOUBLE_ALGN    09  */
  0x0000,  /* OCT_ALGN       10  */
  0x0000,  /* BOOL_ALGN      11  */
  0x0000,  /* BCD1_ALGN      12  */
  0x0001,  /* BCD2_ALGN      13  */
  0x0003,  /* BCD4_ALGN      14  */
  0x0000,  /* BIT_ALGN       15  */
  0x0000   /* VIS_ALGN       16  */
  };

/* Init structure alignment mode variables according to 'align.cfg'	*/
ST_INT m_struct_start_algn_mode = M_STRSTART_MODE_LARGEST;	/* value of M_STRSTART_MODE from align.cfg*/
ST_INT m_struct_end_algn_mode = M_STREND_MODE_LARGEST;	/* value of M_STREND_MODE from align.cfg*/
/************************************************************************/


/*	Common Strings Table	*/
ST_INT numMvlRtNames;
#ifdef USE_RT_TYPE_2
ST_CHAR mvlCompName_ [] = "";
ST_CHAR mvlCompName_phsA [] = "phsA";
ST_CHAR mvlCompName_instCVal [] = "instCVal";
ST_CHAR mvlCompName_mag [] = "mag";
ST_CHAR mvlCompName_i [] = "i";
ST_CHAR mvlCompName_f [] = "f";
ST_CHAR mvlCompName_ang [] = "ang";
ST_CHAR mvlCompName_cVal [] = "cVal";
ST_CHAR mvlCompName_range [] = "range";
ST_CHAR mvlCompName_q [] = "q";
ST_CHAR mvlCompName_t [] = "t";
ST_CHAR mvlCompName_phsB [] = "phsB";
ST_CHAR mvlCompName_phsC [] = "phsC";
ST_CHAR mvlCompName_neut [] = "neut";
ST_CHAR mvlCompName_net [] = "net";
ST_CHAR mvlCompName_res [] = "res";
#else	/* !USE_RT_TYPE_2	*/
SD_CONST ST_CHAR *SD_CONST foMvlRtNames[] =
  {
  "",
  "phsA",
  "instCVal",
  "mag",
  "i",
  "f",
  "ang",
  "cVal",
  "range",
  "q",
  "t",
  "phsB",
  "phsC",
  "neut",
  "net",
  "res"
  };

ST_CHAR **mvlRtNames;
ST_INT maxMvlRtNames;
#endif	/* !USE_RT_TYPE_2	*/

/************************************************************************/

/* MMS OBJECT INITIALIZATION */

ST_VOID mvl_init_type_ctrl (ST_VOID);
static ST_VOID mvl_init_vmd_vars (ST_VOID);
static ST_VOID mvl_init_dom_vars (ST_VOID);
static ST_VOID mvl_init_aa_vars (ST_VOID);
static ST_VOID mvl_init_vmd_varLists (ST_VOID);
static ST_VOID mvl_init_dom_varLists (ST_VOID);
static ST_VOID mvl_init_aa_varLists (ST_VOID);
static ST_VOID mvl_init_journals (ST_VOID);

/* mvl_init_mms_objs may be called more than once, but only first call	*/
/* has any effect.							*/
ST_VOID mvl_init_mms_objs ()
  {
#if defined(OBSOLETE_AA_OBJ_INIT)
ST_INT i;
#endif	/*#if defined(OBSOLETE_AA_OBJ_INIT)*/
static ST_BOOLEAN _mvlInitMmsObjsCalled = SD_FALSE;

/* If already called once, do NOTHING.	*/
  if (_mvlInitMmsObjsCalled)
    return;
  _mvlInitMmsObjsCalled = SD_TRUE;

  if (mvl_max_dyn.aa_nvls == 0)
    mvl_max_dyn.aa_nvls = MVL_NUM_DYN_AA_NVLS;

  if (mvl_max_dyn.aa_vars == 0)
    mvl_max_dyn.aa_vars = MVL_NUM_DYN_AA_VARS;

  if (mvl_max_dyn.doms == 0)
    mvl_max_dyn.doms = MVL_NUM_DYN_DOMS;

  if (mvl_max_dyn.dom_nvls == 0)
    mvl_max_dyn.dom_nvls = MVL_NUM_DYN_DOM_NVLS;

  if (mvl_max_dyn.dom_vars == 0)
    mvl_max_dyn.dom_vars = MVL_NUM_DYN_DOM_VARS;

  if (mvl_max_dyn.journals == 0)
    mvl_max_dyn.journals = MVL_NUM_DYN_JOUS;

  if (mvl_max_dyn.types == 0)
    mvl_max_dyn.types = MVLU_NUM_DYN_TYPES;

  if (mvl_max_dyn.vmd_nvls == 0)
    mvl_max_dyn.vmd_nvls = MVL_NUM_DYN_VMD_NVLS;

  if (mvl_max_dyn.vmd_vars == 0)
    mvl_max_dyn.vmd_vars = MVL_NUM_DYN_VMD_VARS;


#if defined(OBSOLETE_AA_OBJ_INIT)
/* Make sure conn_ctrl allocated (by mvl_start_acse)	*/
  assert (mvl_calling_conn_ctrl || mvl_called_conn_ctrl);

/* Set up the AA Control structures */
  for (i = 0; i < mvl_cfg_info->num_called; ++i)
    mvl_called_conn_ctrl[i].aa_objs = (MVL_AA_OBJ_CTRL *) M_CALLOC (MSMEM_STARTUP, 1, sizeof(MVL_AA_OBJ_CTRL));
  for (i = 0; i < mvl_cfg_info->num_calling; ++i)
    mvl_calling_conn_ctrl[i].aa_objs = (MVL_AA_OBJ_CTRL *) M_CALLOC (MSMEM_STARTUP, 1, sizeof(MVL_AA_OBJ_CTRL));
#endif	/*#if defined(OBSOLETE_AA_OBJ_INIT)*/

  mvl_init_type_ctrl ();
  mvl_init_vmd_vars ();
  mvl_init_dom_vars ();
  mvl_init_aa_vars ();
  mvl_init_vmd_varLists ();
  mvl_init_dom_varLists ();
  mvl_init_aa_varLists ();
  mvl_init_journals ();
  }
/************************************************************************/

/* VMD WIDE NAMED VARIABLE ASSOCIATION INITIALIZATION */

static ST_VOID mvl_init_vmd_vars ()
  {
MVL_VAR_ASSOC **ppva;

  mvl_vmd.max_num_var_assoc = 0 + mvl_max_dyn.vmd_vars;
  mvl_vmd.num_var_assoc = 0;
  if (mvl_vmd.max_num_var_assoc)
    mvl_vmd.var_assoc_tbl = ppva = (MVL_VAR_ASSOC **) M_CALLOC (MSMEM_STARTUP, mvl_vmd.max_num_var_assoc, sizeof (MVL_VAR_ASSOC *));
  }
/************************************************************************/

/* DOMAIN VARIABLE INITIALIZATION */
static ST_VOID mvl_init_dom_vars ()
  {
MVL_DOM_CTRL **ppdom;

  mvl_vmd.num_dom = 0;
  mvl_vmd.max_num_dom = 0 + mvl_max_dyn.doms;
  if (mvl_vmd.max_num_dom)
    mvl_vmd.dom_tbl = ppdom = (MVL_DOM_CTRL **) M_CALLOC (MSMEM_STARTUP, mvl_vmd.max_num_dom, sizeof (MVL_DOM_CTRL *));
  }

/************************************************************************/

/* AA VARIABLE INITIALIZATION */
static ST_VOID mvl_init_aa_vars ()
  {
#if defined(OBSOLETE_AA_OBJ_INIT)
MVL_AA_OBJ_CTRL *aa;
MVL_VAR_ASSOC **ppva;
ST_INT i;
ST_INT j;

/* Do AA specific variables */
  i = 0;
  for (j = 0; j < mvl_cfg_info->num_called; ++j, ++i)
    {
    aa = (MVL_AA_OBJ_CTRL *) mvl_called_conn_ctrl[j].aa_objs;
    aa->foundry_objects = SD_TRUE;
    aa->max_num_var_assoc = 0 + mvl_max_dyn.aa_vars;
    aa->num_var_assoc = 0;
    if (aa->max_num_var_assoc)
      aa->var_assoc_tbl = ppva = (MVL_VAR_ASSOC **) M_CALLOC (MSMEM_STARTUP, aa->max_num_var_assoc, sizeof (MVL_VAR_ASSOC *));
    }
  for (j = 0; j < mvl_cfg_info->num_calling; ++j, ++i)
    {
    aa = (MVL_AA_OBJ_CTRL *) mvl_calling_conn_ctrl[j].aa_objs;
    aa->foundry_objects = SD_TRUE;
    aa->max_num_var_assoc = 0 + mvl_max_dyn.aa_vars;
    aa->num_var_assoc = 0;
    if (aa->max_num_var_assoc)
      aa->var_assoc_tbl = ppva = (MVL_VAR_ASSOC **) M_CALLOC (MSMEM_STARTUP, aa->max_num_var_assoc, sizeof (MVL_VAR_ASSOC *));
    }
#endif	/*#if defined(OBSOLETE_AA_OBJ_INIT)*/
  }

/************************************************************************/

/* NAMED VARIABLE LIST INITIALIZATION */

static ST_VOID mvl_init_vmd_varLists ()
  {
MVL_NVLIST_CTRL **ppvl;

/* Do VMD wide variable lists */
  mvl_vmd.max_num_nvlist = 0 + mvl_max_dyn.vmd_nvls;
  mvl_vmd.num_nvlist = 0;
  if (mvl_vmd.max_num_nvlist)
    mvl_vmd.nvlist_tbl = ppvl = (MVL_NVLIST_CTRL **) M_CALLOC (MSMEM_STARTUP, mvl_vmd.max_num_nvlist, sizeof (MVL_NVLIST_CTRL *));
  }
/************************************************************************/

/* DOMAIN VARIABLE LIST INITIALIZATION */
static ST_VOID mvl_init_dom_varLists ()
  {
  }

/************************************************************************/

/* AA VARIABLE LIST INITIALIZATION */
static ST_VOID mvl_init_aa_varLists ()
  {
#if defined(OBSOLETE_AA_OBJ_INIT)
MVL_AA_OBJ_CTRL *aa;
MVL_NVLIST_CTRL **ppvl;
ST_INT i;


/* Now do AA specific Variable Lists */
  for (i = 0; i < mvl_cfg_info->num_called; ++i)
    {
    aa = (MVL_AA_OBJ_CTRL *) mvl_called_conn_ctrl[i].aa_objs;
    aa->foundry_objects = SD_TRUE;
    aa->max_num_nvlist = 0 + mvl_max_dyn.aa_nvls;
    aa->num_nvlist = 0;
    if (aa->max_num_nvlist)
      aa->nvlist_tbl = ppvl = (MVL_NVLIST_CTRL **) M_CALLOC (MSMEM_STARTUP, aa->max_num_nvlist, sizeof (MVL_NVLIST_CTRL *));
    }
  for (i = 0; i < mvl_cfg_info->num_calling; ++i)
    {
    aa = (MVL_AA_OBJ_CTRL *) mvl_calling_conn_ctrl[i].aa_objs;
    aa->foundry_objects = SD_TRUE;
    aa->max_num_nvlist = 0 + mvl_max_dyn.aa_nvls;
    aa->num_nvlist = 0;
    if (aa->max_num_nvlist)
      aa->nvlist_tbl = ppvl = (MVL_NVLIST_CTRL **) M_CALLOC (MSMEM_STARTUP, aa->max_num_nvlist, sizeof (MVL_NVLIST_CTRL *));
    }
#endif	/*#if defined(OBSOLETE_AA_OBJ_INIT)*/
  }

/************************************************************************/

/* JOURNAL INITIALIZATION */
static ST_VOID mvl_init_journals ()
  {
MVL_JOURNAL_CTRL **ppjou;

  /* initialize VMD_SPEC journals */

  mvl_vmd.max_num_jou = 0 + mvl_max_dyn.journals;
  mvl_vmd.num_jou = 0;
  if (mvl_vmd.max_num_jou)
    mvl_vmd.jou_tbl = ppjou = (MVL_JOURNAL_CTRL **) M_CALLOC (MSMEM_STARTUP, mvl_vmd.max_num_jou, sizeof (MVL_JOURNAL_CTRL *));
  }

/************************************************************************/

ST_VOID mvl_init_type_ctrl ()
  {
static ST_BOOLEAN _mvlInitTypeCtrlCalled = SD_FALSE;

  if (_mvlInitTypeCtrlCalled == SD_TRUE)
    return;
  _mvlInitTypeCtrlCalled = SD_TRUE;

#ifndef USE_RT_TYPE_2
  maxMvlRtNames = 16;
  numMvlRtNames = 16;
  mvlRtNames = foMvlRtNames;
#endif
  mvl_num_types = 19 + mvl_max_dyn.types;

  if (mvl_num_types)
    mvl_type_ctrl = (MVL_TYPE_CTRL *) M_CALLOC (MSMEM_STARTUP, mvl_num_types, sizeof(MVL_TYPE_CTRL));
#if defined USR_SUPPLIED_RT
  u_mvl_start_init_rt_tbl (19, 194);
#endif

/* RTYP_BOOL : UCAreporttypes */
/*
  mvl_type_ctrl[RTYP_BOOL_TYPEID].tdl = 
  "<BOOL>";
*/
  mvl_type_ctrl[RTYP_BOOL_TYPEID].num_rt = 1;
  mvl_type_ctrl[RTYP_BOOL_TYPEID].data_size = 1;
#if defined USR_SUPPLIED_RT
  mvl_type_ctrl[RTYP_BOOL_TYPEID].rt = u_mvl_get_rt_tbl (RTYP_BOOL_TYPEID, 1);
#else
  mvl_type_ctrl[RTYP_BOOL_TYPEID].rt = mvl_rt_tables[rt_table_index++];
#endif /* #if defined USR_SUPPLIED_RT */
  strcpy (mvl_type_ctrl[RTYP_BOOL_TYPEID].type_name, "RTYP_BOOL");

/* RTYP_BTIME6 : UCAreporttypes */
/*
  mvl_type_ctrl[RTYP_BTIME6_TYPEID].tdl = 
  "<BTIME6>";
*/
  mvl_type_ctrl[RTYP_BTIME6_TYPEID].num_rt = 1;
  mvl_type_ctrl[RTYP_BTIME6_TYPEID].data_size = 8;
#if defined USR_SUPPLIED_RT
  mvl_type_ctrl[RTYP_BTIME6_TYPEID].rt = u_mvl_get_rt_tbl (RTYP_BTIME6_TYPEID, 1);
#else
  mvl_type_ctrl[RTYP_BTIME6_TYPEID].rt = mvl_rt_tables[rt_table_index++];
#endif /* #if defined USR_SUPPLIED_RT */
  strcpy (mvl_type_ctrl[RTYP_BTIME6_TYPEID].type_name, "RTYP_BTIME6");

/* RTYP_BSTR6 : UCAreporttypes */
/*
  mvl_type_ctrl[RTYP_BSTR6_TYPEID].tdl = 
  "Bstring6";
*/
  mvl_type_ctrl[RTYP_BSTR6_TYPEID].num_rt = 1;
  mvl_type_ctrl[RTYP_BSTR6_TYPEID].data_size = 1;
#if defined USR_SUPPLIED_RT
  mvl_type_ctrl[RTYP_BSTR6_TYPEID].rt = u_mvl_get_rt_tbl (RTYP_BSTR6_TYPEID, 1);
#else
  mvl_type_ctrl[RTYP_BSTR6_TYPEID].rt = mvl_rt_tables[rt_table_index++];
#endif /* #if defined USR_SUPPLIED_RT */
  strcpy (mvl_type_ctrl[RTYP_BSTR6_TYPEID].type_name, "RTYP_BSTR6");

/* RTYP_BSTR8 : UCAreporttypes */
/*
  mvl_type_ctrl[RTYP_BSTR8_TYPEID].tdl = 
  "Bstring8";
*/
  mvl_type_ctrl[RTYP_BSTR8_TYPEID].num_rt = 1;
  mvl_type_ctrl[RTYP_BSTR8_TYPEID].data_size = 1;
#if defined USR_SUPPLIED_RT
  mvl_type_ctrl[RTYP_BSTR8_TYPEID].rt = u_mvl_get_rt_tbl (RTYP_BSTR8_TYPEID, 1);
#else
  mvl_type_ctrl[RTYP_BSTR8_TYPEID].rt = mvl_rt_tables[rt_table_index++];
#endif /* #if defined USR_SUPPLIED_RT */
  strcpy (mvl_type_ctrl[RTYP_BSTR8_TYPEID].type_name, "RTYP_BSTR8");

/* RTYP_BSTR9 : Client needs for writing OptFlds */
/*
  mvl_type_ctrl[RTYP_BSTR9_TYPEID].tdl = 
  "Bstring9";
*/
  mvl_type_ctrl[RTYP_BSTR9_TYPEID].num_rt = 1;
  mvl_type_ctrl[RTYP_BSTR9_TYPEID].data_size = 2;
#if defined USR_SUPPLIED_RT
  mvl_type_ctrl[RTYP_BSTR9_TYPEID].rt = u_mvl_get_rt_tbl (RTYP_BSTR9_TYPEID, 1);
#else
  mvl_type_ctrl[RTYP_BSTR9_TYPEID].rt = mvl_rt_tables[rt_table_index++];
#endif /* #if defined USR_SUPPLIED_RT */
  strcpy (mvl_type_ctrl[RTYP_BSTR9_TYPEID].type_name, "RTYP_BSTR9");

/* RTYP_BVSTR6 : UCAreporttypes */
/*
  mvl_type_ctrl[RTYP_BVSTR6_TYPEID].tdl = 
  "Bvstring6";
*/
  mvl_type_ctrl[RTYP_BVSTR6_TYPEID].num_rt = 1;
  mvl_type_ctrl[RTYP_BVSTR6_TYPEID].data_size = 4;
#if defined USR_SUPPLIED_RT
  mvl_type_ctrl[RTYP_BVSTR6_TYPEID].rt = u_mvl_get_rt_tbl (RTYP_BVSTR6_TYPEID, 1);
#else
  mvl_type_ctrl[RTYP_BVSTR6_TYPEID].rt = mvl_rt_tables[rt_table_index++];
#endif /* #if defined USR_SUPPLIED_RT */
  strcpy (mvl_type_ctrl[RTYP_BVSTR6_TYPEID].type_name, "RTYP_BVSTR6");

/* RTYP_BVSTR8 : UCAreporttypes */
/*
  mvl_type_ctrl[RTYP_BVSTR8_TYPEID].tdl = 
  "Bvstring8";
*/
  mvl_type_ctrl[RTYP_BVSTR8_TYPEID].num_rt = 1;
  mvl_type_ctrl[RTYP_BVSTR8_TYPEID].data_size = 4;
#if defined USR_SUPPLIED_RT
  mvl_type_ctrl[RTYP_BVSTR8_TYPEID].rt = u_mvl_get_rt_tbl (RTYP_BVSTR8_TYPEID, 1);
#else
  mvl_type_ctrl[RTYP_BVSTR8_TYPEID].rt = mvl_rt_tables[rt_table_index++];
#endif /* #if defined USR_SUPPLIED_RT */
  strcpy (mvl_type_ctrl[RTYP_BVSTR8_TYPEID].type_name, "RTYP_BVSTR8");

/* RTYP_BVSTR10 : UCAreporttypes */
/*
  mvl_type_ctrl[RTYP_BVSTR10_TYPEID].tdl = 
  "Bvstring10";
*/
  mvl_type_ctrl[RTYP_BVSTR10_TYPEID].num_rt = 1;
  mvl_type_ctrl[RTYP_BVSTR10_TYPEID].data_size = 4;
#if defined USR_SUPPLIED_RT
  mvl_type_ctrl[RTYP_BVSTR10_TYPEID].rt = u_mvl_get_rt_tbl (RTYP_BVSTR10_TYPEID, 1);
#else
  mvl_type_ctrl[RTYP_BVSTR10_TYPEID].rt = mvl_rt_tables[rt_table_index++];
#endif /* #if defined USR_SUPPLIED_RT */
  strcpy (mvl_type_ctrl[RTYP_BVSTR10_TYPEID].type_name, "RTYP_BVSTR10");

/* RTYP_INT8U : UCAreporttypes */
/*
  mvl_type_ctrl[RTYP_INT8U_TYPEID].tdl = 
  "<INT8U>";
*/
  mvl_type_ctrl[RTYP_INT8U_TYPEID].num_rt = 1;
  mvl_type_ctrl[RTYP_INT8U_TYPEID].data_size = 1;
#if defined USR_SUPPLIED_RT
  mvl_type_ctrl[RTYP_INT8U_TYPEID].rt = u_mvl_get_rt_tbl (RTYP_INT8U_TYPEID, 1);
#else
  mvl_type_ctrl[RTYP_INT8U_TYPEID].rt = mvl_rt_tables[rt_table_index++];
#endif /* #if defined USR_SUPPLIED_RT */
  strcpy (mvl_type_ctrl[RTYP_INT8U_TYPEID].type_name, "RTYP_INT8U");

/* RTYP_INT16U : UCAreporttypes */
/*
  mvl_type_ctrl[RTYP_INT16U_TYPEID].tdl = 
  "<INT16U>";
*/
  mvl_type_ctrl[RTYP_INT16U_TYPEID].num_rt = 1;
  mvl_type_ctrl[RTYP_INT16U_TYPEID].data_size = 2;
#if defined USR_SUPPLIED_RT
  mvl_type_ctrl[RTYP_INT16U_TYPEID].rt = u_mvl_get_rt_tbl (RTYP_INT16U_TYPEID, 1);
#else
  mvl_type_ctrl[RTYP_INT16U_TYPEID].rt = mvl_rt_tables[rt_table_index++];
#endif /* #if defined USR_SUPPLIED_RT */
  strcpy (mvl_type_ctrl[RTYP_INT16U_TYPEID].type_name, "RTYP_INT16U");

/* RTYP_OSTR8 : UCAreporttypes */
/*
  mvl_type_ctrl[RTYP_OSTR8_TYPEID].tdl = 
  "Ostring8";
*/
  mvl_type_ctrl[RTYP_OSTR8_TYPEID].num_rt = 1;
  mvl_type_ctrl[RTYP_OSTR8_TYPEID].data_size = 8;
#if defined USR_SUPPLIED_RT
  mvl_type_ctrl[RTYP_OSTR8_TYPEID].rt = u_mvl_get_rt_tbl (RTYP_OSTR8_TYPEID, 1);
#else
  mvl_type_ctrl[RTYP_OSTR8_TYPEID].rt = mvl_rt_tables[rt_table_index++];
#endif /* #if defined USR_SUPPLIED_RT */
  strcpy (mvl_type_ctrl[RTYP_OSTR8_TYPEID].type_name, "RTYP_OSTR8");

/* RTYP_VSTR32 : UCAreporttypes */
/*
  mvl_type_ctrl[RTYP_VSTR32_TYPEID].tdl = 
  "<VSTR32>";
*/
  mvl_type_ctrl[RTYP_VSTR32_TYPEID].num_rt = 1;
  mvl_type_ctrl[RTYP_VSTR32_TYPEID].data_size = 33;
#if defined USR_SUPPLIED_RT
  mvl_type_ctrl[RTYP_VSTR32_TYPEID].rt = u_mvl_get_rt_tbl (RTYP_VSTR32_TYPEID, 1);
#else
  mvl_type_ctrl[RTYP_VSTR32_TYPEID].rt = mvl_rt_tables[rt_table_index++];
#endif /* #if defined USR_SUPPLIED_RT */
  strcpy (mvl_type_ctrl[RTYP_VSTR32_TYPEID].type_name, "RTYP_VSTR32");

/* RTYP_VSTR65 : UCAreporttypes */
/*
  mvl_type_ctrl[RTYP_VSTR65_TYPEID].tdl = 
  "<VSTR65>";
*/
  mvl_type_ctrl[RTYP_VSTR65_TYPEID].num_rt = 1;
  mvl_type_ctrl[RTYP_VSTR65_TYPEID].data_size = 66;
#if defined USR_SUPPLIED_RT
  mvl_type_ctrl[RTYP_VSTR65_TYPEID].rt = u_mvl_get_rt_tbl (RTYP_VSTR65_TYPEID, 1);
#else
  mvl_type_ctrl[RTYP_VSTR65_TYPEID].rt = mvl_rt_tables[rt_table_index++];
#endif /* #if defined USR_SUPPLIED_RT */
  strcpy (mvl_type_ctrl[RTYP_VSTR65_TYPEID].type_name, "RTYP_VSTR65");

/* RTYP_OBJREF : UCAreporttypes */
/*
  mvl_type_ctrl[RTYP_OBJREF_TYPEID].tdl = 
  "Vstring129";
*/
  mvl_type_ctrl[RTYP_OBJREF_TYPEID].num_rt = 1;
  mvl_type_ctrl[RTYP_OBJREF_TYPEID].data_size = 130;
#if defined USR_SUPPLIED_RT
  mvl_type_ctrl[RTYP_OBJREF_TYPEID].rt = u_mvl_get_rt_tbl (RTYP_OBJREF_TYPEID, 1);
#else
  mvl_type_ctrl[RTYP_OBJREF_TYPEID].rt = mvl_rt_tables[rt_table_index++];
#endif /* #if defined USR_SUPPLIED_RT */
  strcpy (mvl_type_ctrl[RTYP_OBJREF_TYPEID].type_name, "RTYP_OBJREF");

/* RTYP_INT32U : UCAreporttypes */
/*
  mvl_type_ctrl[RTYP_INT32U_TYPEID].tdl = 
  "<INT32U>";
*/
  mvl_type_ctrl[RTYP_INT32U_TYPEID].num_rt = 1;
  mvl_type_ctrl[RTYP_INT32U_TYPEID].data_size = 4;
#if defined USR_SUPPLIED_RT
  mvl_type_ctrl[RTYP_INT32U_TYPEID].rt = u_mvl_get_rt_tbl (RTYP_INT32U_TYPEID, 1);
#else
  mvl_type_ctrl[RTYP_INT32U_TYPEID].rt = mvl_rt_tables[rt_table_index++];
#endif /* #if defined USR_SUPPLIED_RT */
  strcpy (mvl_type_ctrl[RTYP_INT32U_TYPEID].type_name, "RTYP_INT32U");

/* I16 : Single 16-bit signed integer */
/*
  mvl_type_ctrl[I16_TYPEID].tdl = 
  "Short";
*/
  mvl_type_ctrl[I16_TYPEID].num_rt = 1;
  mvl_type_ctrl[I16_TYPEID].data_size = 2;
#if defined USR_SUPPLIED_RT
  mvl_type_ctrl[I16_TYPEID].rt = u_mvl_get_rt_tbl (I16_TYPEID, 1);
#else
  mvl_type_ctrl[I16_TYPEID].rt = mvl_rt_tables[rt_table_index++];
#endif /* #if defined USR_SUPPLIED_RT */
  strcpy (mvl_type_ctrl[I16_TYPEID].type_name, "I16");

/* UTF8VSTRING13 : variable len Unicode UTF8 string */
/*
  mvl_type_ctrl[UTF8VSTRING13_TYPEID].tdl = 
  "UTF8Vstring13";
*/
  mvl_type_ctrl[UTF8VSTRING13_TYPEID].num_rt = 1;
  mvl_type_ctrl[UTF8VSTRING13_TYPEID].data_size = 28;
#if defined USR_SUPPLIED_RT
  mvl_type_ctrl[UTF8VSTRING13_TYPEID].rt = u_mvl_get_rt_tbl (UTF8VSTRING13_TYPEID, 1);
#else
  mvl_type_ctrl[UTF8VSTRING13_TYPEID].rt = mvl_rt_tables[rt_table_index++];
#endif /* #if defined USR_SUPPLIED_RT */
  strcpy (mvl_type_ctrl[UTF8VSTRING13_TYPEID].type_name, "UTF8VSTRING13");

/* phv_type :  */
/*
  mvl_type_ctrl[phv_type_TYPEID].tdl = 
  "{(phsA){(instCVal){(mag){(i)Long,(f)Float,},(ang){(i)Long,(f)Float,},},(\
cVal){(mag){(i)Long,(f)Float,},(ang){(i)Long,(f)Float,},},(range)Byte,(q\
)BVstring13,(t)Utctime,},(phsB){(instCVal){(mag){(i)Long,(f)Float,},(ang\
){(i)Long,(f)Float,},},(cVal){(mag){(i)Long,(f)Float,},(ang){(i)Long,(f)\
Float,},},(range)Byte,(q)BVstring13,(t)Utctime,},(phsC){(instCVal){(mag)\
{(i)Long,(f)Float,},(ang){(i)Long,(f)Float,},},(cVal){(mag){(i)Long,(f)F\
loat,},(ang){(i)Long,(f)Float,},},(range)Byte,(q)BVstring13,(t)Utctime,}\
,(neut){(instCVal){(mag){(i)Long,(f)Float,},(ang){(i)Long,(f)Float,},},(\
cVal){(mag){(i)Long,(f)Float,},(ang){(i)Long,(f)Float,},},(range)Byte,(q\
)BVstring13,(t)Utctime,},(net){(instCVal){(mag){(i)Long,(f)Float,},(ang)\
{(i)Long,(f)Float,},},(cVal){(mag){(i)Long,(f)Float,},(ang){(i)Long,(f)F\
loat,},},(range)Byte,(q)BVstring13,(t)Utctime,},(res){(instCVal){(mag){(\
i)Long,(f)Float,},(ang){(i)Long,(f)Float,},},(cVal){(mag){(i)Long,(f)Flo\
at,},(ang){(i)Long,(f)Float,},},(range)Byte,(q)BVstring13,(t)Utctime,},}\
";
*/
  mvl_type_ctrl[phv_type_TYPEID].num_rt = 152;
  mvl_type_ctrl[phv_type_TYPEID].data_size = 312;
#if defined USR_SUPPLIED_RT
  mvl_type_ctrl[phv_type_TYPEID].rt = u_mvl_get_rt_tbl (phv_type_TYPEID, 152);
#else
  mvl_type_ctrl[phv_type_TYPEID].rt = mvl_rt_tables[rt_table_index++];
#endif /* #if defined USR_SUPPLIED_RT */
  strcpy (mvl_type_ctrl[phv_type_TYPEID].type_name, "phv_type");

/* phsx_type :  */
/*
  mvl_type_ctrl[phsx_type_TYPEID].tdl = 
  "{(instCVal){(mag){(i)Long,(f)Float,},(ang){(i)Long,(f)Float,},},(cVal){(\
mag){(i)Long,(f)Float,},(ang){(i)Long,(f)Float,},},(range)Byte,(q)BVstri\
ng13,(t)Utctime,}";
*/
  mvl_type_ctrl[phsx_type_TYPEID].num_rt = 25;
  mvl_type_ctrl[phsx_type_TYPEID].data_size = 52;
#if defined USR_SUPPLIED_RT
  mvl_type_ctrl[phsx_type_TYPEID].rt = u_mvl_get_rt_tbl (phsx_type_TYPEID, 25);
#else
  mvl_type_ctrl[phsx_type_TYPEID].rt = mvl_rt_tables[rt_table_index++];
#endif /* #if defined USR_SUPPLIED_RT */
  strcpy (mvl_type_ctrl[phsx_type_TYPEID].type_name, "phsx_type");


#if defined USR_SUPPLIED_RT
  u_mvl_end_init_rt_tbl ();
#endif
  }


/************************************************************************/
/* RUNTIME TYPE DATA */


#if !defined USR_SUPPLIED_RT

/************************************************************************/
/* RTYP_BOOL : UCAreporttypes */

SD_CONST static RUNTIME_TYPE mvl_rt_table_0[1] =
  {
    {				/* rt[0] init data ...			*/
    RT_BOOL,		/* el_tag				*/
    1,				/* el_size				*/
    1,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      1,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    NULL,				/* comp_name_ptr 				*/
#else	/* !USE_RT_TYPE_2	*/
    0,				/* name_index 				*/
#endif	/* !USE_RT_TYPE_2	*/
    }
  };

/************************************************************************/
/* RTYP_BTIME6 : UCAreporttypes */

SD_CONST static RUNTIME_TYPE mvl_rt_table_1[1] =
  {
    {				/* rt[0] init data ...			*/
    RT_BINARY_TIME,		/* el_tag				*/
    8,				/* el_size				*/
    8,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      6,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    NULL,				/* comp_name_ptr 				*/
#else	/* !USE_RT_TYPE_2	*/
    0,				/* name_index 				*/
#endif	/* !USE_RT_TYPE_2	*/
    }
  };

/************************************************************************/
/* RTYP_BSTR6 : UCAreporttypes */

SD_CONST static RUNTIME_TYPE mvl_rt_table_2[1] =
  {
    {				/* rt[0] init data ...			*/
    RT_BIT_STRING,		/* el_tag				*/
    1,				/* el_size				*/
    1,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      6,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    NULL,				/* comp_name_ptr 				*/
#else	/* !USE_RT_TYPE_2	*/
    0,				/* name_index 				*/
#endif	/* !USE_RT_TYPE_2	*/
    }
  };

/************************************************************************/
/* RTYP_BSTR8 : UCAreporttypes */

SD_CONST static RUNTIME_TYPE mvl_rt_table_3[1] =
  {
    {				/* rt[0] init data ...			*/
    RT_BIT_STRING,		/* el_tag				*/
    1,				/* el_size				*/
    1,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      8,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    NULL,				/* comp_name_ptr 				*/
#else	/* !USE_RT_TYPE_2	*/
    0,				/* name_index 				*/
#endif	/* !USE_RT_TYPE_2	*/
    }
  };

/************************************************************************/
/* RTYP_BSTR9 : Client needs for writing OptFlds */

SD_CONST static RUNTIME_TYPE mvl_rt_table_4[1] =
  {
    {				/* rt[0] init data ...			*/
    RT_BIT_STRING,		/* el_tag				*/
    2,				/* el_size				*/
    2,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      9,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    NULL,				/* comp_name_ptr 				*/
#else	/* !USE_RT_TYPE_2	*/
    0,				/* name_index 				*/
#endif	/* !USE_RT_TYPE_2	*/
    }
  };

/************************************************************************/
/* RTYP_BVSTR6 : UCAreporttypes */

SD_CONST static RUNTIME_TYPE mvl_rt_table_5[1] =
  {
    {				/* rt[0] init data ...			*/
    RT_BIT_STRING,		/* el_tag				*/
    4,				/* el_size				*/
    4,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      -6,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    NULL,				/* comp_name_ptr 				*/
#else	/* !USE_RT_TYPE_2	*/
    0,				/* name_index 				*/
#endif	/* !USE_RT_TYPE_2	*/
    }
  };

/************************************************************************/
/* RTYP_BVSTR8 : UCAreporttypes */

SD_CONST static RUNTIME_TYPE mvl_rt_table_6[1] =
  {
    {				/* rt[0] init data ...			*/
    RT_BIT_STRING,		/* el_tag				*/
    4,				/* el_size				*/
    4,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      -8,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    NULL,				/* comp_name_ptr 				*/
#else	/* !USE_RT_TYPE_2	*/
    0,				/* name_index 				*/
#endif	/* !USE_RT_TYPE_2	*/
    }
  };

/************************************************************************/
/* RTYP_BVSTR10 : UCAreporttypes */

SD_CONST static RUNTIME_TYPE mvl_rt_table_7[1] =
  {
    {				/* rt[0] init data ...			*/
    RT_BIT_STRING,		/* el_tag				*/
    4,				/* el_size				*/
    4,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      -10,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    NULL,				/* comp_name_ptr 				*/
#else	/* !USE_RT_TYPE_2	*/
    0,				/* name_index 				*/
#endif	/* !USE_RT_TYPE_2	*/
    }
  };

/************************************************************************/
/* RTYP_INT8U : UCAreporttypes */

SD_CONST static RUNTIME_TYPE mvl_rt_table_8[1] =
  {
    {				/* rt[0] init data ...			*/
    RT_UNSIGNED,		/* el_tag				*/
    1,				/* el_size				*/
    1,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      1,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    NULL,				/* comp_name_ptr 				*/
#else	/* !USE_RT_TYPE_2	*/
    0,				/* name_index 				*/
#endif	/* !USE_RT_TYPE_2	*/
    }
  };

/************************************************************************/
/* RTYP_INT16U : UCAreporttypes */

SD_CONST static RUNTIME_TYPE mvl_rt_table_9[1] =
  {
    {				/* rt[0] init data ...			*/
    RT_UNSIGNED,		/* el_tag				*/
    2,				/* el_size				*/
    2,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      2,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    NULL,				/* comp_name_ptr 				*/
#else	/* !USE_RT_TYPE_2	*/
    0,				/* name_index 				*/
#endif	/* !USE_RT_TYPE_2	*/
    }
  };

/************************************************************************/
/* RTYP_OSTR8 : UCAreporttypes */

SD_CONST static RUNTIME_TYPE mvl_rt_table_10[1] =
  {
    {				/* rt[0] init data ...			*/
    RT_OCTET_STRING,		/* el_tag				*/
    8,				/* el_size				*/
    8,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      8,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    NULL,				/* comp_name_ptr 				*/
#else	/* !USE_RT_TYPE_2	*/
    0,				/* name_index 				*/
#endif	/* !USE_RT_TYPE_2	*/
    }
  };

/************************************************************************/
/* RTYP_VSTR32 : UCAreporttypes */

SD_CONST static RUNTIME_TYPE mvl_rt_table_11[1] =
  {
    {				/* rt[0] init data ...			*/
    RT_VISIBLE_STRING,		/* el_tag				*/
    33,				/* el_size				*/
    33,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      -32,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    NULL,				/* comp_name_ptr 				*/
#else	/* !USE_RT_TYPE_2	*/
    0,				/* name_index 				*/
#endif	/* !USE_RT_TYPE_2	*/
    }
  };

/************************************************************************/
/* RTYP_VSTR65 : UCAreporttypes */

SD_CONST static RUNTIME_TYPE mvl_rt_table_12[1] =
  {
    {				/* rt[0] init data ...			*/
    RT_VISIBLE_STRING,		/* el_tag				*/
    66,				/* el_size				*/
    66,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      -65,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    NULL,				/* comp_name_ptr 				*/
#else	/* !USE_RT_TYPE_2	*/
    0,				/* name_index 				*/
#endif	/* !USE_RT_TYPE_2	*/
    }
  };

/************************************************************************/
/* RTYP_OBJREF : UCAreporttypes */

SD_CONST static RUNTIME_TYPE mvl_rt_table_13[1] =
  {
    {				/* rt[0] init data ...			*/
    RT_VISIBLE_STRING,		/* el_tag				*/
    130,				/* el_size				*/
    130,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      -129,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    NULL,				/* comp_name_ptr 				*/
#else	/* !USE_RT_TYPE_2	*/
    0,				/* name_index 				*/
#endif	/* !USE_RT_TYPE_2	*/
    }
  };

/************************************************************************/
/* RTYP_INT32U : UCAreporttypes */

SD_CONST static RUNTIME_TYPE mvl_rt_table_14[1] =
  {
    {				/* rt[0] init data ...			*/
    RT_UNSIGNED,		/* el_tag				*/
    4,				/* el_size				*/
    4,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      4,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    NULL,				/* comp_name_ptr 				*/
#else	/* !USE_RT_TYPE_2	*/
    0,				/* name_index 				*/
#endif	/* !USE_RT_TYPE_2	*/
    }
  };

/************************************************************************/
/* I16 : Single 16-bit signed integer */

SD_CONST static RUNTIME_TYPE mvl_rt_table_15[1] =
  {
    {				/* rt[0] init data ...			*/
    RT_INTEGER,		/* el_tag				*/
    2,				/* el_size				*/
    2,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      2,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    NULL,				/* comp_name_ptr 				*/
#else	/* !USE_RT_TYPE_2	*/
    0,				/* name_index 				*/
#endif	/* !USE_RT_TYPE_2	*/
    }
  };

/************************************************************************/
/* UTF8VSTRING13 : variable len Unicode UTF8 string */

SD_CONST static RUNTIME_TYPE mvl_rt_table_16[1] =
  {
    {				/* rt[0] init data ...			*/
    RT_UTF8_STRING,		/* el_tag				*/
    28,				/* el_size				*/
    28,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      -13,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    NULL,				/* comp_name_ptr 				*/
#else	/* !USE_RT_TYPE_2	*/
    0,				/* name_index 				*/
#endif	/* !USE_RT_TYPE_2	*/
    }
  };

/************************************************************************/
/* phv_type :  */

SD_CONST static RUNTIME_TYPE mvl_rt_table_17[152] =
  {
    {				/* rt[0] init data ...			*/
    RT_STR_START,		/* el_tag				*/
    0,				/* el_size				*/
    312,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      150,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    NULL,				/* comp_name_ptr 				*/
#else	/* !USE_RT_TYPE_2	*/
    0,				/* name_index 				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[1] init data ...			*/
    RT_STR_START,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      23,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_phsA, 	/* comp_name_ptr 'phsA'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_PHSA_STRING_INDEX, 	/* name_index 'phsA'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[2] init data ...			*/
    RT_STR_START,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      8,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_instCVal, 	/* comp_name_ptr 'instCVal'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_INSTCVAL_STRING_INDEX, 	/* name_index 'instCVal'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[3] init data ...			*/
    RT_STR_START,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      2,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_mag, 	/* comp_name_ptr 'mag'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_MAG_STRING_INDEX, 	/* name_index 'mag'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[4] init data ...			*/
    RT_INTEGER,		/* el_tag				*/
    4,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      4,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_i, 	/* comp_name_ptr 'i'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_I_STRING_INDEX, 	/* name_index 'i'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[5] init data ...			*/
    RT_FLOATING_POINT,		/* el_tag				*/
    4,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      4,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_f, 	/* comp_name_ptr 'f'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_F_STRING_INDEX, 	/* name_index 'f'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[6] init data ...			*/
    RT_STR_END,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      2,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    NULL,				/* comp_name_ptr 				*/
#else	/* !USE_RT_TYPE_2	*/
    0,				/* name_index 				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[7] init data ...			*/
    RT_STR_START,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      2,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_ang, 	/* comp_name_ptr 'ang'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_ANG_STRING_INDEX, 	/* name_index 'ang'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[8] init data ...			*/
    RT_INTEGER,		/* el_tag				*/
    4,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      4,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_i, 	/* comp_name_ptr 'i'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_I_STRING_INDEX, 	/* name_index 'i'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[9] init data ...			*/
    RT_FLOATING_POINT,		/* el_tag				*/
    4,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      4,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_f, 	/* comp_name_ptr 'f'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_F_STRING_INDEX, 	/* name_index 'f'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[10] init data ...			*/
    RT_STR_END,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      2,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    NULL,				/* comp_name_ptr 				*/
#else	/* !USE_RT_TYPE_2	*/
    0,				/* name_index 				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[11] init data ...			*/
    RT_STR_END,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      8,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    NULL,				/* comp_name_ptr 				*/
#else	/* !USE_RT_TYPE_2	*/
    0,				/* name_index 				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[12] init data ...			*/
    RT_STR_START,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      8,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_cVal, 	/* comp_name_ptr 'cVal'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_CVAL_STRING_INDEX, 	/* name_index 'cVal'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[13] init data ...			*/
    RT_STR_START,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      2,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_mag, 	/* comp_name_ptr 'mag'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_MAG_STRING_INDEX, 	/* name_index 'mag'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[14] init data ...			*/
    RT_INTEGER,		/* el_tag				*/
    4,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      4,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_i, 	/* comp_name_ptr 'i'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_I_STRING_INDEX, 	/* name_index 'i'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[15] init data ...			*/
    RT_FLOATING_POINT,		/* el_tag				*/
    4,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      4,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_f, 	/* comp_name_ptr 'f'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_F_STRING_INDEX, 	/* name_index 'f'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[16] init data ...			*/
    RT_STR_END,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      2,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    NULL,				/* comp_name_ptr 				*/
#else	/* !USE_RT_TYPE_2	*/
    0,				/* name_index 				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[17] init data ...			*/
    RT_STR_START,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      2,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_ang, 	/* comp_name_ptr 'ang'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_ANG_STRING_INDEX, 	/* name_index 'ang'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[18] init data ...			*/
    RT_INTEGER,		/* el_tag				*/
    4,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      4,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_i, 	/* comp_name_ptr 'i'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_I_STRING_INDEX, 	/* name_index 'i'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[19] init data ...			*/
    RT_FLOATING_POINT,		/* el_tag				*/
    4,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      4,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_f, 	/* comp_name_ptr 'f'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_F_STRING_INDEX, 	/* name_index 'f'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[20] init data ...			*/
    RT_STR_END,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      2,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    NULL,				/* comp_name_ptr 				*/
#else	/* !USE_RT_TYPE_2	*/
    0,				/* name_index 				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[21] init data ...			*/
    RT_STR_END,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      8,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    NULL,				/* comp_name_ptr 				*/
#else	/* !USE_RT_TYPE_2	*/
    0,				/* name_index 				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[22] init data ...			*/
    RT_INTEGER,		/* el_tag				*/
    2,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      1,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_range, 	/* comp_name_ptr 'range'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_RANGE_STRING_INDEX, 	/* name_index 'range'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[23] init data ...			*/
    RT_BIT_STRING,		/* el_tag				*/
    6,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      -13,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_q, 	/* comp_name_ptr 'q'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_Q_STRING_INDEX, 	/* name_index 'q'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[24] init data ...			*/
    RT_UTC_TIME,		/* el_tag				*/
    12,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      8,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_t, 	/* comp_name_ptr 't'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_T_STRING_INDEX, 	/* name_index 't'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[25] init data ...			*/
    RT_STR_END,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      23,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    NULL,				/* comp_name_ptr 				*/
#else	/* !USE_RT_TYPE_2	*/
    0,				/* name_index 				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[26] init data ...			*/
    RT_STR_START,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      23,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_phsB, 	/* comp_name_ptr 'phsB'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_PHSB_STRING_INDEX, 	/* name_index 'phsB'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[27] init data ...			*/
    RT_STR_START,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      8,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_instCVal, 	/* comp_name_ptr 'instCVal'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_INSTCVAL_STRING_INDEX, 	/* name_index 'instCVal'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[28] init data ...			*/
    RT_STR_START,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      2,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_mag, 	/* comp_name_ptr 'mag'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_MAG_STRING_INDEX, 	/* name_index 'mag'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[29] init data ...			*/
    RT_INTEGER,		/* el_tag				*/
    4,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      4,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_i, 	/* comp_name_ptr 'i'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_I_STRING_INDEX, 	/* name_index 'i'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[30] init data ...			*/
    RT_FLOATING_POINT,		/* el_tag				*/
    4,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      4,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_f, 	/* comp_name_ptr 'f'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_F_STRING_INDEX, 	/* name_index 'f'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[31] init data ...			*/
    RT_STR_END,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      2,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    NULL,				/* comp_name_ptr 				*/
#else	/* !USE_RT_TYPE_2	*/
    0,				/* name_index 				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[32] init data ...			*/
    RT_STR_START,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      2,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_ang, 	/* comp_name_ptr 'ang'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_ANG_STRING_INDEX, 	/* name_index 'ang'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[33] init data ...			*/
    RT_INTEGER,		/* el_tag				*/
    4,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      4,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_i, 	/* comp_name_ptr 'i'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_I_STRING_INDEX, 	/* name_index 'i'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[34] init data ...			*/
    RT_FLOATING_POINT,		/* el_tag				*/
    4,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      4,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_f, 	/* comp_name_ptr 'f'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_F_STRING_INDEX, 	/* name_index 'f'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[35] init data ...			*/
    RT_STR_END,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      2,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    NULL,				/* comp_name_ptr 				*/
#else	/* !USE_RT_TYPE_2	*/
    0,				/* name_index 				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[36] init data ...			*/
    RT_STR_END,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      8,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    NULL,				/* comp_name_ptr 				*/
#else	/* !USE_RT_TYPE_2	*/
    0,				/* name_index 				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[37] init data ...			*/
    RT_STR_START,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      8,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_cVal, 	/* comp_name_ptr 'cVal'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_CVAL_STRING_INDEX, 	/* name_index 'cVal'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[38] init data ...			*/
    RT_STR_START,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      2,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_mag, 	/* comp_name_ptr 'mag'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_MAG_STRING_INDEX, 	/* name_index 'mag'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[39] init data ...			*/
    RT_INTEGER,		/* el_tag				*/
    4,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      4,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_i, 	/* comp_name_ptr 'i'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_I_STRING_INDEX, 	/* name_index 'i'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[40] init data ...			*/
    RT_FLOATING_POINT,		/* el_tag				*/
    4,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      4,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_f, 	/* comp_name_ptr 'f'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_F_STRING_INDEX, 	/* name_index 'f'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[41] init data ...			*/
    RT_STR_END,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      2,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    NULL,				/* comp_name_ptr 				*/
#else	/* !USE_RT_TYPE_2	*/
    0,				/* name_index 				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[42] init data ...			*/
    RT_STR_START,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      2,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_ang, 	/* comp_name_ptr 'ang'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_ANG_STRING_INDEX, 	/* name_index 'ang'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[43] init data ...			*/
    RT_INTEGER,		/* el_tag				*/
    4,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      4,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_i, 	/* comp_name_ptr 'i'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_I_STRING_INDEX, 	/* name_index 'i'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[44] init data ...			*/
    RT_FLOATING_POINT,		/* el_tag				*/
    4,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      4,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_f, 	/* comp_name_ptr 'f'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_F_STRING_INDEX, 	/* name_index 'f'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[45] init data ...			*/
    RT_STR_END,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      2,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    NULL,				/* comp_name_ptr 				*/
#else	/* !USE_RT_TYPE_2	*/
    0,				/* name_index 				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[46] init data ...			*/
    RT_STR_END,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      8,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    NULL,				/* comp_name_ptr 				*/
#else	/* !USE_RT_TYPE_2	*/
    0,				/* name_index 				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[47] init data ...			*/
    RT_INTEGER,		/* el_tag				*/
    2,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      1,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_range, 	/* comp_name_ptr 'range'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_RANGE_STRING_INDEX, 	/* name_index 'range'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[48] init data ...			*/
    RT_BIT_STRING,		/* el_tag				*/
    6,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      -13,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_q, 	/* comp_name_ptr 'q'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_Q_STRING_INDEX, 	/* name_index 'q'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[49] init data ...			*/
    RT_UTC_TIME,		/* el_tag				*/
    12,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      8,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_t, 	/* comp_name_ptr 't'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_T_STRING_INDEX, 	/* name_index 't'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[50] init data ...			*/
    RT_STR_END,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      23,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    NULL,				/* comp_name_ptr 				*/
#else	/* !USE_RT_TYPE_2	*/
    0,				/* name_index 				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[51] init data ...			*/
    RT_STR_START,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      23,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_phsC, 	/* comp_name_ptr 'phsC'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_PHSC_STRING_INDEX, 	/* name_index 'phsC'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[52] init data ...			*/
    RT_STR_START,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      8,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_instCVal, 	/* comp_name_ptr 'instCVal'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_INSTCVAL_STRING_INDEX, 	/* name_index 'instCVal'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[53] init data ...			*/
    RT_STR_START,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      2,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_mag, 	/* comp_name_ptr 'mag'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_MAG_STRING_INDEX, 	/* name_index 'mag'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[54] init data ...			*/
    RT_INTEGER,		/* el_tag				*/
    4,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      4,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_i, 	/* comp_name_ptr 'i'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_I_STRING_INDEX, 	/* name_index 'i'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[55] init data ...			*/
    RT_FLOATING_POINT,		/* el_tag				*/
    4,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      4,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_f, 	/* comp_name_ptr 'f'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_F_STRING_INDEX, 	/* name_index 'f'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[56] init data ...			*/
    RT_STR_END,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      2,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    NULL,				/* comp_name_ptr 				*/
#else	/* !USE_RT_TYPE_2	*/
    0,				/* name_index 				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[57] init data ...			*/
    RT_STR_START,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      2,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_ang, 	/* comp_name_ptr 'ang'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_ANG_STRING_INDEX, 	/* name_index 'ang'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[58] init data ...			*/
    RT_INTEGER,		/* el_tag				*/
    4,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      4,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_i, 	/* comp_name_ptr 'i'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_I_STRING_INDEX, 	/* name_index 'i'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[59] init data ...			*/
    RT_FLOATING_POINT,		/* el_tag				*/
    4,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      4,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_f, 	/* comp_name_ptr 'f'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_F_STRING_INDEX, 	/* name_index 'f'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[60] init data ...			*/
    RT_STR_END,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      2,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    NULL,				/* comp_name_ptr 				*/
#else	/* !USE_RT_TYPE_2	*/
    0,				/* name_index 				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[61] init data ...			*/
    RT_STR_END,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      8,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    NULL,				/* comp_name_ptr 				*/
#else	/* !USE_RT_TYPE_2	*/
    0,				/* name_index 				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[62] init data ...			*/
    RT_STR_START,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      8,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_cVal, 	/* comp_name_ptr 'cVal'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_CVAL_STRING_INDEX, 	/* name_index 'cVal'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[63] init data ...			*/
    RT_STR_START,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      2,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_mag, 	/* comp_name_ptr 'mag'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_MAG_STRING_INDEX, 	/* name_index 'mag'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[64] init data ...			*/
    RT_INTEGER,		/* el_tag				*/
    4,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      4,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_i, 	/* comp_name_ptr 'i'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_I_STRING_INDEX, 	/* name_index 'i'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[65] init data ...			*/
    RT_FLOATING_POINT,		/* el_tag				*/
    4,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      4,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_f, 	/* comp_name_ptr 'f'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_F_STRING_INDEX, 	/* name_index 'f'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[66] init data ...			*/
    RT_STR_END,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      2,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    NULL,				/* comp_name_ptr 				*/
#else	/* !USE_RT_TYPE_2	*/
    0,				/* name_index 				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[67] init data ...			*/
    RT_STR_START,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      2,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_ang, 	/* comp_name_ptr 'ang'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_ANG_STRING_INDEX, 	/* name_index 'ang'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[68] init data ...			*/
    RT_INTEGER,		/* el_tag				*/
    4,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      4,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_i, 	/* comp_name_ptr 'i'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_I_STRING_INDEX, 	/* name_index 'i'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[69] init data ...			*/
    RT_FLOATING_POINT,		/* el_tag				*/
    4,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      4,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_f, 	/* comp_name_ptr 'f'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_F_STRING_INDEX, 	/* name_index 'f'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[70] init data ...			*/
    RT_STR_END,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      2,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    NULL,				/* comp_name_ptr 				*/
#else	/* !USE_RT_TYPE_2	*/
    0,				/* name_index 				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[71] init data ...			*/
    RT_STR_END,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      8,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    NULL,				/* comp_name_ptr 				*/
#else	/* !USE_RT_TYPE_2	*/
    0,				/* name_index 				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[72] init data ...			*/
    RT_INTEGER,		/* el_tag				*/
    2,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      1,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_range, 	/* comp_name_ptr 'range'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_RANGE_STRING_INDEX, 	/* name_index 'range'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[73] init data ...			*/
    RT_BIT_STRING,		/* el_tag				*/
    6,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      -13,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_q, 	/* comp_name_ptr 'q'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_Q_STRING_INDEX, 	/* name_index 'q'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[74] init data ...			*/
    RT_UTC_TIME,		/* el_tag				*/
    12,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      8,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_t, 	/* comp_name_ptr 't'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_T_STRING_INDEX, 	/* name_index 't'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[75] init data ...			*/
    RT_STR_END,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      23,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    NULL,				/* comp_name_ptr 				*/
#else	/* !USE_RT_TYPE_2	*/
    0,				/* name_index 				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[76] init data ...			*/
    RT_STR_START,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      23,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_neut, 	/* comp_name_ptr 'neut'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_NEUT_STRING_INDEX, 	/* name_index 'neut'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[77] init data ...			*/
    RT_STR_START,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      8,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_instCVal, 	/* comp_name_ptr 'instCVal'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_INSTCVAL_STRING_INDEX, 	/* name_index 'instCVal'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[78] init data ...			*/
    RT_STR_START,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      2,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_mag, 	/* comp_name_ptr 'mag'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_MAG_STRING_INDEX, 	/* name_index 'mag'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[79] init data ...			*/
    RT_INTEGER,		/* el_tag				*/
    4,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      4,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_i, 	/* comp_name_ptr 'i'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_I_STRING_INDEX, 	/* name_index 'i'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[80] init data ...			*/
    RT_FLOATING_POINT,		/* el_tag				*/
    4,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      4,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_f, 	/* comp_name_ptr 'f'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_F_STRING_INDEX, 	/* name_index 'f'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[81] init data ...			*/
    RT_STR_END,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      2,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    NULL,				/* comp_name_ptr 				*/
#else	/* !USE_RT_TYPE_2	*/
    0,				/* name_index 				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[82] init data ...			*/
    RT_STR_START,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      2,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_ang, 	/* comp_name_ptr 'ang'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_ANG_STRING_INDEX, 	/* name_index 'ang'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[83] init data ...			*/
    RT_INTEGER,		/* el_tag				*/
    4,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      4,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_i, 	/* comp_name_ptr 'i'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_I_STRING_INDEX, 	/* name_index 'i'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[84] init data ...			*/
    RT_FLOATING_POINT,		/* el_tag				*/
    4,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      4,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_f, 	/* comp_name_ptr 'f'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_F_STRING_INDEX, 	/* name_index 'f'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[85] init data ...			*/
    RT_STR_END,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      2,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    NULL,				/* comp_name_ptr 				*/
#else	/* !USE_RT_TYPE_2	*/
    0,				/* name_index 				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[86] init data ...			*/
    RT_STR_END,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      8,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    NULL,				/* comp_name_ptr 				*/
#else	/* !USE_RT_TYPE_2	*/
    0,				/* name_index 				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[87] init data ...			*/
    RT_STR_START,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      8,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_cVal, 	/* comp_name_ptr 'cVal'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_CVAL_STRING_INDEX, 	/* name_index 'cVal'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[88] init data ...			*/
    RT_STR_START,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      2,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_mag, 	/* comp_name_ptr 'mag'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_MAG_STRING_INDEX, 	/* name_index 'mag'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[89] init data ...			*/
    RT_INTEGER,		/* el_tag				*/
    4,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      4,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_i, 	/* comp_name_ptr 'i'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_I_STRING_INDEX, 	/* name_index 'i'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[90] init data ...			*/
    RT_FLOATING_POINT,		/* el_tag				*/
    4,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      4,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_f, 	/* comp_name_ptr 'f'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_F_STRING_INDEX, 	/* name_index 'f'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[91] init data ...			*/
    RT_STR_END,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      2,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    NULL,				/* comp_name_ptr 				*/
#else	/* !USE_RT_TYPE_2	*/
    0,				/* name_index 				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[92] init data ...			*/
    RT_STR_START,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      2,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_ang, 	/* comp_name_ptr 'ang'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_ANG_STRING_INDEX, 	/* name_index 'ang'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[93] init data ...			*/
    RT_INTEGER,		/* el_tag				*/
    4,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      4,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_i, 	/* comp_name_ptr 'i'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_I_STRING_INDEX, 	/* name_index 'i'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[94] init data ...			*/
    RT_FLOATING_POINT,		/* el_tag				*/
    4,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      4,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_f, 	/* comp_name_ptr 'f'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_F_STRING_INDEX, 	/* name_index 'f'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[95] init data ...			*/
    RT_STR_END,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      2,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    NULL,				/* comp_name_ptr 				*/
#else	/* !USE_RT_TYPE_2	*/
    0,				/* name_index 				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[96] init data ...			*/
    RT_STR_END,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      8,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    NULL,				/* comp_name_ptr 				*/
#else	/* !USE_RT_TYPE_2	*/
    0,				/* name_index 				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[97] init data ...			*/
    RT_INTEGER,		/* el_tag				*/
    2,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      1,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_range, 	/* comp_name_ptr 'range'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_RANGE_STRING_INDEX, 	/* name_index 'range'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[98] init data ...			*/
    RT_BIT_STRING,		/* el_tag				*/
    6,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      -13,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_q, 	/* comp_name_ptr 'q'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_Q_STRING_INDEX, 	/* name_index 'q'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[99] init data ...			*/
    RT_UTC_TIME,		/* el_tag				*/
    12,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      8,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_t, 	/* comp_name_ptr 't'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_T_STRING_INDEX, 	/* name_index 't'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[100] init data ...			*/
    RT_STR_END,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      23,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    NULL,				/* comp_name_ptr 				*/
#else	/* !USE_RT_TYPE_2	*/
    0,				/* name_index 				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[101] init data ...			*/
    RT_STR_START,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      23,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_net, 	/* comp_name_ptr 'net'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_NET_STRING_INDEX, 	/* name_index 'net'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[102] init data ...			*/
    RT_STR_START,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      8,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_instCVal, 	/* comp_name_ptr 'instCVal'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_INSTCVAL_STRING_INDEX, 	/* name_index 'instCVal'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[103] init data ...			*/
    RT_STR_START,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      2,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_mag, 	/* comp_name_ptr 'mag'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_MAG_STRING_INDEX, 	/* name_index 'mag'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[104] init data ...			*/
    RT_INTEGER,		/* el_tag				*/
    4,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      4,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_i, 	/* comp_name_ptr 'i'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_I_STRING_INDEX, 	/* name_index 'i'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[105] init data ...			*/
    RT_FLOATING_POINT,		/* el_tag				*/
    4,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      4,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_f, 	/* comp_name_ptr 'f'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_F_STRING_INDEX, 	/* name_index 'f'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[106] init data ...			*/
    RT_STR_END,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      2,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    NULL,				/* comp_name_ptr 				*/
#else	/* !USE_RT_TYPE_2	*/
    0,				/* name_index 				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[107] init data ...			*/
    RT_STR_START,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      2,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_ang, 	/* comp_name_ptr 'ang'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_ANG_STRING_INDEX, 	/* name_index 'ang'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[108] init data ...			*/
    RT_INTEGER,		/* el_tag				*/
    4,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      4,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_i, 	/* comp_name_ptr 'i'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_I_STRING_INDEX, 	/* name_index 'i'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[109] init data ...			*/
    RT_FLOATING_POINT,		/* el_tag				*/
    4,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      4,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_f, 	/* comp_name_ptr 'f'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_F_STRING_INDEX, 	/* name_index 'f'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[110] init data ...			*/
    RT_STR_END,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      2,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    NULL,				/* comp_name_ptr 				*/
#else	/* !USE_RT_TYPE_2	*/
    0,				/* name_index 				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[111] init data ...			*/
    RT_STR_END,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      8,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    NULL,				/* comp_name_ptr 				*/
#else	/* !USE_RT_TYPE_2	*/
    0,				/* name_index 				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[112] init data ...			*/
    RT_STR_START,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      8,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_cVal, 	/* comp_name_ptr 'cVal'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_CVAL_STRING_INDEX, 	/* name_index 'cVal'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[113] init data ...			*/
    RT_STR_START,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      2,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_mag, 	/* comp_name_ptr 'mag'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_MAG_STRING_INDEX, 	/* name_index 'mag'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[114] init data ...			*/
    RT_INTEGER,		/* el_tag				*/
    4,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      4,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_i, 	/* comp_name_ptr 'i'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_I_STRING_INDEX, 	/* name_index 'i'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[115] init data ...			*/
    RT_FLOATING_POINT,		/* el_tag				*/
    4,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      4,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_f, 	/* comp_name_ptr 'f'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_F_STRING_INDEX, 	/* name_index 'f'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[116] init data ...			*/
    RT_STR_END,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      2,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    NULL,				/* comp_name_ptr 				*/
#else	/* !USE_RT_TYPE_2	*/
    0,				/* name_index 				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[117] init data ...			*/
    RT_STR_START,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      2,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_ang, 	/* comp_name_ptr 'ang'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_ANG_STRING_INDEX, 	/* name_index 'ang'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[118] init data ...			*/
    RT_INTEGER,		/* el_tag				*/
    4,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      4,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_i, 	/* comp_name_ptr 'i'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_I_STRING_INDEX, 	/* name_index 'i'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[119] init data ...			*/
    RT_FLOATING_POINT,		/* el_tag				*/
    4,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      4,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_f, 	/* comp_name_ptr 'f'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_F_STRING_INDEX, 	/* name_index 'f'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[120] init data ...			*/
    RT_STR_END,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      2,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    NULL,				/* comp_name_ptr 				*/
#else	/* !USE_RT_TYPE_2	*/
    0,				/* name_index 				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[121] init data ...			*/
    RT_STR_END,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      8,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    NULL,				/* comp_name_ptr 				*/
#else	/* !USE_RT_TYPE_2	*/
    0,				/* name_index 				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[122] init data ...			*/
    RT_INTEGER,		/* el_tag				*/
    2,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      1,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_range, 	/* comp_name_ptr 'range'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_RANGE_STRING_INDEX, 	/* name_index 'range'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[123] init data ...			*/
    RT_BIT_STRING,		/* el_tag				*/
    6,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      -13,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_q, 	/* comp_name_ptr 'q'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_Q_STRING_INDEX, 	/* name_index 'q'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[124] init data ...			*/
    RT_UTC_TIME,		/* el_tag				*/
    12,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      8,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_t, 	/* comp_name_ptr 't'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_T_STRING_INDEX, 	/* name_index 't'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[125] init data ...			*/
    RT_STR_END,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      23,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    NULL,				/* comp_name_ptr 				*/
#else	/* !USE_RT_TYPE_2	*/
    0,				/* name_index 				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[126] init data ...			*/
    RT_STR_START,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      23,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_res, 	/* comp_name_ptr 'res'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_RES_STRING_INDEX, 	/* name_index 'res'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[127] init data ...			*/
    RT_STR_START,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      8,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_instCVal, 	/* comp_name_ptr 'instCVal'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_INSTCVAL_STRING_INDEX, 	/* name_index 'instCVal'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[128] init data ...			*/
    RT_STR_START,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      2,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_mag, 	/* comp_name_ptr 'mag'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_MAG_STRING_INDEX, 	/* name_index 'mag'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[129] init data ...			*/
    RT_INTEGER,		/* el_tag				*/
    4,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      4,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_i, 	/* comp_name_ptr 'i'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_I_STRING_INDEX, 	/* name_index 'i'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[130] init data ...			*/
    RT_FLOATING_POINT,		/* el_tag				*/
    4,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      4,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_f, 	/* comp_name_ptr 'f'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_F_STRING_INDEX, 	/* name_index 'f'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[131] init data ...			*/
    RT_STR_END,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      2,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    NULL,				/* comp_name_ptr 				*/
#else	/* !USE_RT_TYPE_2	*/
    0,				/* name_index 				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[132] init data ...			*/
    RT_STR_START,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      2,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_ang, 	/* comp_name_ptr 'ang'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_ANG_STRING_INDEX, 	/* name_index 'ang'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[133] init data ...			*/
    RT_INTEGER,		/* el_tag				*/
    4,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      4,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_i, 	/* comp_name_ptr 'i'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_I_STRING_INDEX, 	/* name_index 'i'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[134] init data ...			*/
    RT_FLOATING_POINT,		/* el_tag				*/
    4,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      4,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_f, 	/* comp_name_ptr 'f'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_F_STRING_INDEX, 	/* name_index 'f'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[135] init data ...			*/
    RT_STR_END,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      2,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    NULL,				/* comp_name_ptr 				*/
#else	/* !USE_RT_TYPE_2	*/
    0,				/* name_index 				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[136] init data ...			*/
    RT_STR_END,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      8,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    NULL,				/* comp_name_ptr 				*/
#else	/* !USE_RT_TYPE_2	*/
    0,				/* name_index 				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[137] init data ...			*/
    RT_STR_START,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      8,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_cVal, 	/* comp_name_ptr 'cVal'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_CVAL_STRING_INDEX, 	/* name_index 'cVal'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[138] init data ...			*/
    RT_STR_START,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      2,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_mag, 	/* comp_name_ptr 'mag'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_MAG_STRING_INDEX, 	/* name_index 'mag'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[139] init data ...			*/
    RT_INTEGER,		/* el_tag				*/
    4,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      4,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_i, 	/* comp_name_ptr 'i'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_I_STRING_INDEX, 	/* name_index 'i'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[140] init data ...			*/
    RT_FLOATING_POINT,		/* el_tag				*/
    4,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      4,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_f, 	/* comp_name_ptr 'f'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_F_STRING_INDEX, 	/* name_index 'f'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[141] init data ...			*/
    RT_STR_END,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      2,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    NULL,				/* comp_name_ptr 				*/
#else	/* !USE_RT_TYPE_2	*/
    0,				/* name_index 				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[142] init data ...			*/
    RT_STR_START,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      2,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_ang, 	/* comp_name_ptr 'ang'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_ANG_STRING_INDEX, 	/* name_index 'ang'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[143] init data ...			*/
    RT_INTEGER,		/* el_tag				*/
    4,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      4,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_i, 	/* comp_name_ptr 'i'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_I_STRING_INDEX, 	/* name_index 'i'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[144] init data ...			*/
    RT_FLOATING_POINT,		/* el_tag				*/
    4,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      4,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_f, 	/* comp_name_ptr 'f'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_F_STRING_INDEX, 	/* name_index 'f'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[145] init data ...			*/
    RT_STR_END,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      2,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    NULL,				/* comp_name_ptr 				*/
#else	/* !USE_RT_TYPE_2	*/
    0,				/* name_index 				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[146] init data ...			*/
    RT_STR_END,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      8,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    NULL,				/* comp_name_ptr 				*/
#else	/* !USE_RT_TYPE_2	*/
    0,				/* name_index 				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[147] init data ...			*/
    RT_INTEGER,		/* el_tag				*/
    2,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      1,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_range, 	/* comp_name_ptr 'range'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_RANGE_STRING_INDEX, 	/* name_index 'range'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[148] init data ...			*/
    RT_BIT_STRING,		/* el_tag				*/
    6,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      -13,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_q, 	/* comp_name_ptr 'q'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_Q_STRING_INDEX, 	/* name_index 'q'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[149] init data ...			*/
    RT_UTC_TIME,		/* el_tag				*/
    12,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      8,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_t, 	/* comp_name_ptr 't'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_T_STRING_INDEX, 	/* name_index 't'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[150] init data ...			*/
    RT_STR_END,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      23,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    NULL,				/* comp_name_ptr 				*/
#else	/* !USE_RT_TYPE_2	*/
    0,				/* name_index 				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[151] init data ...			*/
    RT_STR_END,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      150,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    NULL,				/* comp_name_ptr 				*/
#else	/* !USE_RT_TYPE_2	*/
    0,				/* name_index 				*/
#endif	/* !USE_RT_TYPE_2	*/
    }
  };

/************************************************************************/
/* phsx_type :  */

SD_CONST static RUNTIME_TYPE mvl_rt_table_18[25] =
  {
    {				/* rt[0] init data ...			*/
    RT_STR_START,		/* el_tag				*/
    0,				/* el_size				*/
    52,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      23,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    NULL,				/* comp_name_ptr 				*/
#else	/* !USE_RT_TYPE_2	*/
    0,				/* name_index 				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[1] init data ...			*/
    RT_STR_START,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      8,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_instCVal, 	/* comp_name_ptr 'instCVal'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_INSTCVAL_STRING_INDEX, 	/* name_index 'instCVal'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[2] init data ...			*/
    RT_STR_START,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      2,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_mag, 	/* comp_name_ptr 'mag'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_MAG_STRING_INDEX, 	/* name_index 'mag'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[3] init data ...			*/
    RT_INTEGER,		/* el_tag				*/
    4,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      4,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_i, 	/* comp_name_ptr 'i'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_I_STRING_INDEX, 	/* name_index 'i'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[4] init data ...			*/
    RT_FLOATING_POINT,		/* el_tag				*/
    4,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      4,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_f, 	/* comp_name_ptr 'f'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_F_STRING_INDEX, 	/* name_index 'f'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[5] init data ...			*/
    RT_STR_END,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      2,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    NULL,				/* comp_name_ptr 				*/
#else	/* !USE_RT_TYPE_2	*/
    0,				/* name_index 				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[6] init data ...			*/
    RT_STR_START,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      2,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_ang, 	/* comp_name_ptr 'ang'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_ANG_STRING_INDEX, 	/* name_index 'ang'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[7] init data ...			*/
    RT_INTEGER,		/* el_tag				*/
    4,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      4,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_i, 	/* comp_name_ptr 'i'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_I_STRING_INDEX, 	/* name_index 'i'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[8] init data ...			*/
    RT_FLOATING_POINT,		/* el_tag				*/
    4,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      4,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_f, 	/* comp_name_ptr 'f'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_F_STRING_INDEX, 	/* name_index 'f'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[9] init data ...			*/
    RT_STR_END,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      2,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    NULL,				/* comp_name_ptr 				*/
#else	/* !USE_RT_TYPE_2	*/
    0,				/* name_index 				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[10] init data ...			*/
    RT_STR_END,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      8,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    NULL,				/* comp_name_ptr 				*/
#else	/* !USE_RT_TYPE_2	*/
    0,				/* name_index 				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[11] init data ...			*/
    RT_STR_START,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      8,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_cVal, 	/* comp_name_ptr 'cVal'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_CVAL_STRING_INDEX, 	/* name_index 'cVal'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[12] init data ...			*/
    RT_STR_START,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      2,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_mag, 	/* comp_name_ptr 'mag'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_MAG_STRING_INDEX, 	/* name_index 'mag'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[13] init data ...			*/
    RT_INTEGER,		/* el_tag				*/
    4,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      4,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_i, 	/* comp_name_ptr 'i'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_I_STRING_INDEX, 	/* name_index 'i'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[14] init data ...			*/
    RT_FLOATING_POINT,		/* el_tag				*/
    4,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      4,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_f, 	/* comp_name_ptr 'f'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_F_STRING_INDEX, 	/* name_index 'f'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[15] init data ...			*/
    RT_STR_END,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      2,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    NULL,				/* comp_name_ptr 				*/
#else	/* !USE_RT_TYPE_2	*/
    0,				/* name_index 				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[16] init data ...			*/
    RT_STR_START,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      2,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_ang, 	/* comp_name_ptr 'ang'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_ANG_STRING_INDEX, 	/* name_index 'ang'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[17] init data ...			*/
    RT_INTEGER,		/* el_tag				*/
    4,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      4,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_i, 	/* comp_name_ptr 'i'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_I_STRING_INDEX, 	/* name_index 'i'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[18] init data ...			*/
    RT_FLOATING_POINT,		/* el_tag				*/
    4,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      4,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_f, 	/* comp_name_ptr 'f'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_F_STRING_INDEX, 	/* name_index 'f'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[19] init data ...			*/
    RT_STR_END,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      2,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    NULL,				/* comp_name_ptr 				*/
#else	/* !USE_RT_TYPE_2	*/
    0,				/* name_index 				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[20] init data ...			*/
    RT_STR_END,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      8,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    NULL,				/* comp_name_ptr 				*/
#else	/* !USE_RT_TYPE_2	*/
    0,				/* name_index 				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[21] init data ...			*/
    RT_INTEGER,		/* el_tag				*/
    2,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      1,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_range, 	/* comp_name_ptr 'range'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_RANGE_STRING_INDEX, 	/* name_index 'range'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[22] init data ...			*/
    RT_BIT_STRING,		/* el_tag				*/
    6,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      -13,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_q, 	/* comp_name_ptr 'q'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_Q_STRING_INDEX, 	/* name_index 'q'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[23] init data ...			*/
    RT_UTC_TIME,		/* el_tag				*/
    12,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      8,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_t, 	/* comp_name_ptr 't'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_T_STRING_INDEX, 	/* name_index 't'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[24] init data ...			*/
    RT_STR_END,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      23,			/*   num_rt_blks				*/
      0			/*   pad					*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    NULL,				/* comp_name_ptr 				*/
#else	/* !USE_RT_TYPE_2	*/
    0,				/* name_index 				*/
#endif	/* !USE_RT_TYPE_2	*/
    }
  };

/************************************************************************/


ST_INT rt_table_index;

SD_CONST RUNTIME_TYPE * SD_CONST mvl_rt_tables[] =
  {
  mvl_rt_table_0,
  mvl_rt_table_1,
  mvl_rt_table_2,
  mvl_rt_table_3,
  mvl_rt_table_4,
  mvl_rt_table_5,
  mvl_rt_table_6,
  mvl_rt_table_7,
  mvl_rt_table_8,
  mvl_rt_table_9,
  mvl_rt_table_10,
  mvl_rt_table_11,
  mvl_rt_table_12,
  mvl_rt_table_13,
  mvl_rt_table_14,
  mvl_rt_table_15,
  mvl_rt_table_16,
  mvl_rt_table_17,
  mvl_rt_table_18
  };


#endif /* #if defined USR_SUPPLIED_RT */

