
/************************************************************************/
/*  This file created from input file '..\..\mvl\usr\server\srvrobj.odf'
    Leaf Access Parameter (LAP) File: 'Not Used'
	Created Mon Feb 25 19:13:34 2019
*/

#include "glbtypes.h"
#include "sysincs.h"
#include "mmsdefs.h"
#include "mms_pvar.h"
#include "mms_vvar.h"
#include "mvl_acse.h"
#include "srvrobj.h"

#include "server.h"	/* User Specified */

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
ST_CHAR mvlCompName_s [] = "s";
ST_CHAR mvlCompName_l [] = "l";
#else	/* !USE_RT_TYPE_2	*/
SD_CONST ST_CHAR *SD_CONST foMvlRtNames[] =
  {
  "",
  "s",
  "l"
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

  mvl_vmd.foundry_objects = SD_TRUE;
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
MVL_VAR_ASSOC *va;
MVL_VAR_ASSOC **ppva;

  va = (MVL_VAR_ASSOC *) M_CALLOC (MSMEM_STARTUP, 7, sizeof (MVL_VAR_ASSOC));
  mvl_vmd.max_num_var_assoc = 7 + mvl_max_dyn.vmd_vars;
  mvl_vmd.num_var_assoc = 7;
  if (mvl_vmd.max_num_var_assoc)
    mvl_vmd.var_assoc_tbl = ppva = (MVL_VAR_ASSOC **) M_CALLOC (MSMEM_STARTUP, mvl_vmd.max_num_var_assoc, sizeof (MVL_VAR_ASSOC *));
  va->name = "Temperature";
  va->type_id = I16_TYPEID;
  va->type_ctrl = &mvl_type_ctrl[I16_TYPEID];
  va->data = &Temperature;
  va->proc = &varProcFuns;
  *ppva++ = va++;
  va->name = "UTF8Vstring13test";
  va->type_id = UTF8VSTRING13_TYPEID;
  va->type_ctrl = &mvl_type_ctrl[UTF8VSTRING13_TYPEID];
  va->data = &UTF8Vstring13test;
  *ppva++ = va++;
  va->name = "UtcTimeNow";
  va->type_id = UTC_TM_TYPEID;
  va->type_ctrl = &mvl_type_ctrl[UTC_TM_TYPEID];
  va->data = &UtcTimeNow;
  va->proc = &varProcFuns;
  *ppva++ = va++;
  va->name = "arr1";
  va->type_id = ARR1_TYPEID;
  va->type_ctrl = &mvl_type_ctrl[ARR1_TYPEID];
  va->data = arr1;
  va->proc = &varProcFuns;
  *ppva++ = va++;
  va->name = "struct1";
  va->type_id = STRUCT1_TYPEID;
  va->type_ctrl = &mvl_type_ctrl[STRUCT1_TYPEID];
  va->data = &struct1;
  va->proc = &varProcFuns;
  *ppva++ = va++;
  va->name = "struct1$l";
  va->type_id = I32_TYPEID;
  va->type_ctrl = &mvl_type_ctrl[I32_TYPEID];
  va->data = &struct1.l;
  va->proc = &varProcFuns;
  *ppva++ = va++;
  va->name = "struct1$s";
  va->type_id = I16_TYPEID;
  va->type_ctrl = &mvl_type_ctrl[I16_TYPEID];
  va->data = &struct1.s;
  va->proc = &varProcFuns;
  *ppva++ = va++;
  }
/************************************************************************/

/* DOMAIN VARIABLE INITIALIZATION */
static ST_VOID mvl_init_dom_vars ()
  {
MVL_DOM_CTRL **ppdom;
MVL_DOM_CTRL *dom;
MVL_VAR_ASSOC *va;
MVL_VAR_ASSOC **ppva;

  mvl_vmd.num_dom = 2;
  mvl_vmd.max_num_dom = 2 + mvl_max_dyn.doms;
  if (mvl_vmd.max_num_dom)
    mvl_vmd.dom_tbl = ppdom = (MVL_DOM_CTRL **) M_CALLOC (MSMEM_STARTUP, mvl_vmd.max_num_dom, sizeof (MVL_DOM_CTRL *));
  dom = (MVL_DOM_CTRL *) M_CALLOC (MSMEM_STARTUP, 2, sizeof (MVL_DOM_CTRL));

/* Domain: 'mvlLiteDom'	*/
  dom->name = "mvlLiteDom";
  dom->max_num_var_assoc = 5 + mvl_max_dyn.dom_vars;
  dom->num_var_assoc = 5;
  dom->foundry_objects = SD_TRUE;
  if (dom->max_num_var_assoc)
    dom->var_assoc_tbl = ppva = (MVL_VAR_ASSOC **) M_CALLOC (MSMEM_STARTUP, dom->max_num_var_assoc, sizeof (MVL_VAR_ASSOC *));

  va = (MVL_VAR_ASSOC *) M_CALLOC (MSMEM_STARTUP, 5, sizeof (MVL_VAR_ASSOC));
  va->name = "domArr1";
  va->type_id = ARR1_TYPEID;
  va->type_ctrl = &mvl_type_ctrl[ARR1_TYPEID];
  va->data = domArr1;
  va->proc = &varProcFuns;
  *ppva++ = va++;
  va->name = "domStruct1";
  va->type_id = STRUCT1_TYPEID;
  va->type_ctrl = &mvl_type_ctrl[STRUCT1_TYPEID];
  va->data = &domStruct1;
  va->proc = &varProcFuns;
  *ppva++ = va++;
  va->name = "domStruct1$l";
  va->type_id = I32_TYPEID;
  va->type_ctrl = &mvl_type_ctrl[I32_TYPEID];
  va->data = &domStruct1.l;
  va->proc = &varProcFuns;
  *ppva++ = va++;
  va->name = "domStruct1$s";
  va->type_id = I16_TYPEID;
  va->type_ctrl = &mvl_type_ctrl[I16_TYPEID];
  va->data = &domStruct1.s;
  va->proc = &varProcFuns;
  *ppva++ = va++;
  va->name = "domTemperature";
  va->type_id = I16_TYPEID;
  va->type_ctrl = &mvl_type_ctrl[I16_TYPEID];
  va->data = &domTemperature;
  va->proc = &varProcFuns;
  *ppva++ = va++;
  *ppdom++ = dom++;

/* Domain: 'mvlLiteDom1'	*/
  dom->name = "mvlLiteDom1";
  dom->max_num_var_assoc = 0 + mvl_max_dyn.dom_vars;
  dom->num_var_assoc = 0;
  if (dom->max_num_var_assoc)
    dom->var_assoc_tbl = ppva = (MVL_VAR_ASSOC **) M_CALLOC (MSMEM_STARTUP, dom->max_num_var_assoc, sizeof (MVL_VAR_ASSOC *));

  *ppdom++ = dom++;
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
MVL_NVLIST_CTRL *vl;
MVL_NVLIST_CTRL **ppvl;

/* Do VMD wide variable lists */
  vl = (MVL_NVLIST_CTRL *) M_CALLOC (MSMEM_STARTUP, 2, sizeof (MVL_NVLIST_CTRL));
  mvl_vmd.max_num_nvlist = 2 + mvl_max_dyn.vmd_nvls;
  mvl_vmd.num_nvlist = 2;
  if (mvl_vmd.max_num_nvlist)
    mvl_vmd.nvlist_tbl = ppvl = (MVL_NVLIST_CTRL **) M_CALLOC (MSMEM_STARTUP, mvl_vmd.max_num_nvlist, sizeof (MVL_NVLIST_CTRL *));
  vl->name = "allButAaVars";
  vl->num_of_entries = 10;
  vl->entries = (MVL_VAR_ASSOC **) M_CALLOC (MSMEM_STARTUP, 10, sizeof (MVL_VAR_ASSOC *));
  vl->va_scope = (MVL_SCOPE *) M_CALLOC (MSMEM_STARTUP, 10, sizeof (MVL_SCOPE));
  vl->nvl_scope.scope = VMD_SPEC;
  vl->entries[0] = mvl_vmd.var_assoc_tbl[0];	/* Temperature	*/
  vl->va_scope[0].scope = VMD_SPEC;
  vl->entries[1] = mvl_vmd.var_assoc_tbl[3];	/* arr1	*/
  vl->va_scope[1].scope = VMD_SPEC;
  vl->entries[2] = mvl_vmd.var_assoc_tbl[4];	/* struct1	*/
  vl->va_scope[2].scope = VMD_SPEC;
  vl->entries[3] = mvl_vmd.var_assoc_tbl[5];	/* struct1$l	*/
  vl->va_scope[3].scope = VMD_SPEC;
  vl->entries[4] = mvl_vmd.var_assoc_tbl[6];	/* struct1$s	*/
  vl->va_scope[4].scope = VMD_SPEC;
  vl->entries[5] = mvl_vmd.dom_tbl[0]->var_assoc_tbl[4];	/* domTemperature	*/
  vl->va_scope[5].scope = DOM_SPEC;
  vl->va_scope[5].dom = mvl_vmd.dom_tbl[0];
  vl->entries[6] = mvl_vmd.dom_tbl[0]->var_assoc_tbl[0];	/* domArr1	*/
  vl->va_scope[6].scope = DOM_SPEC;
  vl->va_scope[6].dom = mvl_vmd.dom_tbl[0];
  vl->entries[7] = mvl_vmd.dom_tbl[0]->var_assoc_tbl[1];	/* domStruct1	*/
  vl->va_scope[7].scope = DOM_SPEC;
  vl->va_scope[7].dom = mvl_vmd.dom_tbl[0];
  vl->entries[8] = mvl_vmd.dom_tbl[0]->var_assoc_tbl[2];	/* domStruct1$l	*/
  vl->va_scope[8].scope = DOM_SPEC;
  vl->va_scope[8].dom = mvl_vmd.dom_tbl[0];
  vl->entries[9] = mvl_vmd.dom_tbl[0]->var_assoc_tbl[3];	/* domStruct1$s	*/
  vl->va_scope[9].scope = DOM_SPEC;
  vl->va_scope[9].dom = mvl_vmd.dom_tbl[0];
  *ppvl++ = vl++;
  vl->name = "nvl1";
  vl->num_of_entries = 3;
  vl->entries = (MVL_VAR_ASSOC **) M_CALLOC (MSMEM_STARTUP, 3, sizeof (MVL_VAR_ASSOC *));
  vl->va_scope = (MVL_SCOPE *) M_CALLOC (MSMEM_STARTUP, 3, sizeof (MVL_SCOPE));
  vl->nvl_scope.scope = VMD_SPEC;
  vl->entries[0] = mvl_vmd.var_assoc_tbl[3];	/* arr1	*/
  vl->va_scope[0].scope = VMD_SPEC;
  vl->entries[1] = mvl_vmd.var_assoc_tbl[4];	/* struct1	*/
  vl->va_scope[1].scope = VMD_SPEC;
  vl->entries[2] = mvl_vmd.var_assoc_tbl[0];	/* Temperature	*/
  vl->va_scope[2].scope = VMD_SPEC;
  *ppvl++ = vl++;
  }
/************************************************************************/

/* DOMAIN VARIABLE LIST INITIALIZATION */
static ST_VOID mvl_init_dom_varLists ()
  {
MVL_DOM_CTRL *dom;
MVL_NVLIST_CTRL *vl;
MVL_NVLIST_CTRL **ppvl;

  dom = mvl_vmd.dom_tbl [0];	/* allocated by mvl_init_dom_vars	*/

/* Domain: 'mvlLiteDom'	*/

  dom->max_num_nvlist = 1 + mvl_max_dyn.dom_nvls;
  dom->num_nvlist = 1;
  if (dom->max_num_nvlist)
    dom->nvlist_tbl = ppvl = (MVL_NVLIST_CTRL **) M_CALLOC (MSMEM_STARTUP, dom->max_num_nvlist, sizeof (MVL_NVLIST_CTRL *));
  vl = (MVL_NVLIST_CTRL *) M_CALLOC (MSMEM_STARTUP, 1, sizeof (MVL_NVLIST_CTRL));
  vl->name = "nvl1";
  vl->num_of_entries = 3;
  vl->entries = (MVL_VAR_ASSOC **) M_CALLOC (MSMEM_STARTUP, 3, sizeof (MVL_VAR_ASSOC *));
  vl->va_scope = (MVL_SCOPE *) M_CALLOC (MSMEM_STARTUP, 3, sizeof (MVL_SCOPE));
  vl->nvl_scope.scope = DOM_SPEC;
  vl->nvl_scope.dom = dom;
  vl->entries[0] = mvl_vmd.dom_tbl[0]->var_assoc_tbl[0];	/* domArr1	*/
  vl->va_scope[0].scope = DOM_SPEC;
  vl->va_scope[0].dom = mvl_vmd.dom_tbl[0];
  vl->entries[1] = mvl_vmd.dom_tbl[0]->var_assoc_tbl[1];	/* domStruct1	*/
  vl->va_scope[1].scope = DOM_SPEC;
  vl->va_scope[1].dom = mvl_vmd.dom_tbl[0];
  vl->entries[2] = mvl_vmd.dom_tbl[0]->var_assoc_tbl[4];	/* domTemperature	*/
  vl->va_scope[2].scope = DOM_SPEC;
  vl->va_scope[2].dom = mvl_vmd.dom_tbl[0];
  *ppvl++ = vl++;
  ++dom;

/* Domain: 'mvlLiteDom1'	*/

  dom->max_num_nvlist = 0 + mvl_max_dyn.dom_nvls;
  dom->num_nvlist = 0;
  if (dom->max_num_nvlist)
    dom->nvlist_tbl = ppvl = (MVL_NVLIST_CTRL **) M_CALLOC (MSMEM_STARTUP, dom->max_num_nvlist, sizeof (MVL_NVLIST_CTRL *));
  ++dom;
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
MVL_JOURNAL_CTRL *jou;
MVL_JOURNAL_CTRL **ppjou;
MVL_DOM_CTRL *dom;

  /* initialize VMD_SPEC journals */

  jou = (MVL_JOURNAL_CTRL *) M_CALLOC (MSMEM_STARTUP, 1, sizeof (MVL_JOURNAL_CTRL));
  mvl_vmd.max_num_jou = 1 + mvl_max_dyn.journals;
  mvl_vmd.num_jou = 1;
  if (mvl_vmd.max_num_jou)
    mvl_vmd.jou_tbl = ppjou = (MVL_JOURNAL_CTRL **) M_CALLOC (MSMEM_STARTUP, mvl_vmd.max_num_jou, sizeof (MVL_JOURNAL_CTRL *));
  jou->name = "SampleJournal";
  *ppjou++ = jou++;

  /* initialize DOM_SPEC journals */

  dom = mvl_vmd_find_dom (&mvl_vmd, "mvlLiteDom");
  if (dom != NULL)
    {
    dom->max_num_jou = 0 + mvl_max_dyn.journals;
    dom->num_jou = 0;
    if (dom->max_num_jou)
      dom->jou_tbl = ppjou = (MVL_JOURNAL_CTRL **) M_CALLOC (MSMEM_STARTUP, dom->max_num_jou, sizeof (MVL_JOURNAL_CTRL *));
    }

  dom = mvl_vmd_find_dom (&mvl_vmd, "mvlLiteDom1");
  if (dom != NULL)
    {
    MVL_JOURNAL_CTRL *djou = (MVL_JOURNAL_CTRL *) M_CALLOC (MSMEM_STARTUP, 1, sizeof (MVL_JOURNAL_CTRL));
    dom->max_num_jou = 1 + mvl_max_dyn.journals;
    dom->num_jou = 1;
    if (dom->max_num_jou)
      dom->jou_tbl = ppjou = (MVL_JOURNAL_CTRL **) M_CALLOC (MSMEM_STARTUP, dom->max_num_jou, sizeof (MVL_JOURNAL_CTRL *));
    djou->name = "MyJournal";
    *ppjou++ = djou++;
    }
  }

/************************************************************************/

ST_VOID mvl_init_type_ctrl ()
  {
static ST_BOOLEAN _mvlInitTypeCtrlCalled = SD_FALSE;

  if (_mvlInitTypeCtrlCalled == SD_TRUE)
    return;
  _mvlInitTypeCtrlCalled = SD_TRUE;

#ifndef USE_RT_TYPE_2
  maxMvlRtNames = 3;
  numMvlRtNames = 3;
  mvlRtNames = foMvlRtNames;
#endif
  mvl_num_types = 8 + mvl_max_dyn.types;

  if (mvl_num_types)
    mvl_type_ctrl = (MVL_TYPE_CTRL *) M_CALLOC (MSMEM_STARTUP, mvl_num_types, sizeof(MVL_TYPE_CTRL));
#if defined USR_SUPPLIED_RT
  u_mvl_start_init_rt_tbl (8, 15);
#endif

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

/* I32 : Single 32-bit signed integer */
/*
  mvl_type_ctrl[I32_TYPEID].tdl = 
  "Long";
*/
  mvl_type_ctrl[I32_TYPEID].num_rt = 1;
  mvl_type_ctrl[I32_TYPEID].data_size = 4;
#if defined USR_SUPPLIED_RT
  mvl_type_ctrl[I32_TYPEID].rt = u_mvl_get_rt_tbl (I32_TYPEID, 1);
#else
  mvl_type_ctrl[I32_TYPEID].rt = mvl_rt_tables[rt_table_index++];
#endif /* #if defined USR_SUPPLIED_RT */
  strcpy (mvl_type_ctrl[I32_TYPEID].type_name, "I32");

/* U32 : Single 32-bit unsigned integer */
/*
  mvl_type_ctrl[U32_TYPEID].tdl = 
  "ULong";
*/
  mvl_type_ctrl[U32_TYPEID].num_rt = 1;
  mvl_type_ctrl[U32_TYPEID].data_size = 4;
#if defined USR_SUPPLIED_RT
  mvl_type_ctrl[U32_TYPEID].rt = u_mvl_get_rt_tbl (U32_TYPEID, 1);
#else
  mvl_type_ctrl[U32_TYPEID].rt = mvl_rt_tables[rt_table_index++];
#endif /* #if defined USR_SUPPLIED_RT */
  strcpy (mvl_type_ctrl[U32_TYPEID].type_name, "U32");

/* STRUCT1 : Basic simple structure */
/*
  mvl_type_ctrl[STRUCT1_TYPEID].tdl = 
  "{(s)Short,(l)Long}";
*/
  mvl_type_ctrl[STRUCT1_TYPEID].num_rt = 4;
  mvl_type_ctrl[STRUCT1_TYPEID].data_size = 8;
#if defined USR_SUPPLIED_RT
  mvl_type_ctrl[STRUCT1_TYPEID].rt = u_mvl_get_rt_tbl (STRUCT1_TYPEID, 4);
#else
  mvl_type_ctrl[STRUCT1_TYPEID].rt = mvl_rt_tables[rt_table_index++];
#endif /* #if defined USR_SUPPLIED_RT */
  strcpy (mvl_type_ctrl[STRUCT1_TYPEID].type_name, "STRUCT1");

/* ARR1 : Array Of 10 16-bit signed integers */
/*
  mvl_type_ctrl[ARR1_TYPEID].tdl = 
  "[10:Short]";
*/
  mvl_type_ctrl[ARR1_TYPEID].num_rt = 3;
  mvl_type_ctrl[ARR1_TYPEID].data_size = 20;
#if defined USR_SUPPLIED_RT
  mvl_type_ctrl[ARR1_TYPEID].rt = u_mvl_get_rt_tbl (ARR1_TYPEID, 3);
#else
  mvl_type_ctrl[ARR1_TYPEID].rt = mvl_rt_tables[rt_table_index++];
#endif /* #if defined USR_SUPPLIED_RT */
  strcpy (mvl_type_ctrl[ARR1_TYPEID].type_name, "ARR1");

/* LARGE_ARR : Array Of 400 16-bit signed integers */
/*
  mvl_type_ctrl[LARGE_ARR_TYPEID].tdl = 
  "[400:Short]";
*/
  mvl_type_ctrl[LARGE_ARR_TYPEID].num_rt = 3;
  mvl_type_ctrl[LARGE_ARR_TYPEID].data_size = 800;
#if defined USR_SUPPLIED_RT
  mvl_type_ctrl[LARGE_ARR_TYPEID].rt = u_mvl_get_rt_tbl (LARGE_ARR_TYPEID, 3);
#else
  mvl_type_ctrl[LARGE_ARR_TYPEID].rt = mvl_rt_tables[rt_table_index++];
#endif /* #if defined USR_SUPPLIED_RT */
  strcpy (mvl_type_ctrl[LARGE_ARR_TYPEID].type_name, "LARGE_ARR");

/* UTC_TM : UTC Time */
/*
  mvl_type_ctrl[UTC_TM_TYPEID].tdl = 
  "Utctime";
*/
  mvl_type_ctrl[UTC_TM_TYPEID].num_rt = 1;
  mvl_type_ctrl[UTC_TM_TYPEID].data_size = 12;
#if defined USR_SUPPLIED_RT
  mvl_type_ctrl[UTC_TM_TYPEID].rt = u_mvl_get_rt_tbl (UTC_TM_TYPEID, 1);
#else
  mvl_type_ctrl[UTC_TM_TYPEID].rt = mvl_rt_tables[rt_table_index++];
#endif /* #if defined USR_SUPPLIED_RT */
  strcpy (mvl_type_ctrl[UTC_TM_TYPEID].type_name, "UTC_TM");

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


#if defined USR_SUPPLIED_RT
  u_mvl_end_init_rt_tbl ();
#endif
  }


/************************************************************************/
/* RUNTIME TYPE DATA */


#if !defined USR_SUPPLIED_RT

/************************************************************************/
/* I16 : Single 16-bit signed integer */

SD_CONST static RUNTIME_TYPE mvl_rt_table_0[1] =
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
/* I32 : Single 32-bit signed integer */

SD_CONST static RUNTIME_TYPE mvl_rt_table_1[1] =
  {
    {				/* rt[0] init data ...			*/
    RT_INTEGER,		/* el_tag				*/
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
/* U32 : Single 32-bit unsigned integer */

SD_CONST static RUNTIME_TYPE mvl_rt_table_2[1] =
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
/* STRUCT1 : Basic simple structure */

SD_CONST static RUNTIME_TYPE mvl_rt_table_3[4] =
  {
    {				/* rt[0] init data ...			*/
    RT_STR_START,		/* el_tag				*/
    0,				/* el_size				*/
    8,				/* offset_to_last			*/
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
    {				/* rt[1] init data ...			*/
    RT_INTEGER,		/* el_tag				*/
    4,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      2,			/*   el_len				*/
      0			/*   pad				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    mvlCompName_s, 	/* comp_name_ptr 's'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_S_STRING_INDEX, 	/* name_index 's'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[2] init data ...			*/
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
    mvlCompName_l, 	/* comp_name_ptr 'l'				*/
#else	/* !USE_RT_TYPE_2	*/
    FO_L_STRING_INDEX, 	/* name_index 'l'				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[3] init data ...			*/
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
    }
  };

/************************************************************************/
/* ARR1 : Array Of 10 16-bit signed integers */

SD_CONST static RUNTIME_TYPE mvl_rt_table_4[3] =
  {
    {				/* rt[0] init data ...			*/
    RT_ARR_START,		/* el_tag				*/
    0,				/* el_size				*/
    20,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      10,			/*   num_elmnts				*/
      1			/*   num_rt_blks				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    NULL,				/* comp_name_ptr 				*/
#else	/* !USE_RT_TYPE_2	*/
    0,				/* name_index 				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[1] init data ...			*/
    RT_INTEGER,		/* el_tag				*/
    2,				/* el_size				*/
    0,				/* offset_to_last			*/
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
    },
    {				/* rt[2] init data ...			*/
    RT_ARR_END,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      10,			/*   num_elmnts				*/
      1			/*   num_rt_blks				*/
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
/* LARGE_ARR : Array Of 400 16-bit signed integers */

SD_CONST static RUNTIME_TYPE mvl_rt_table_5[3] =
  {
    {				/* rt[0] init data ...			*/
    RT_ARR_START,		/* el_tag				*/
    0,				/* el_size				*/
    800,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      400,			/*   num_elmnts				*/
      1			/*   num_rt_blks				*/
      }				/* end 'p'				*/
      },			/* end 'u'				*/
#ifdef USE_RT_TYPE_2
    NULL,				/* comp_name_ptr 				*/
#else	/* !USE_RT_TYPE_2	*/
    0,				/* name_index 				*/
#endif	/* !USE_RT_TYPE_2	*/
    },
    {				/* rt[1] init data ...			*/
    RT_INTEGER,		/* el_tag				*/
    2,				/* el_size				*/
    0,				/* offset_to_last			*/
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
    },
    {				/* rt[2] init data ...			*/
    RT_ARR_END,		/* el_tag				*/
    0,				/* el_size				*/
    0,				/* offset_to_last			*/
      {				/* u					*/
      {				/* p					*/
      400,			/*   num_elmnts				*/
      1			/*   num_rt_blks				*/
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
/* UTC_TM : UTC Time */

SD_CONST static RUNTIME_TYPE mvl_rt_table_6[1] =
  {
    {				/* rt[0] init data ...			*/
    RT_UTC_TIME,		/* el_tag				*/
    12,				/* el_size				*/
    12,				/* offset_to_last			*/
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
/* UTF8VSTRING13 : variable len Unicode UTF8 string */

SD_CONST static RUNTIME_TYPE mvl_rt_table_7[1] =
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
  mvl_rt_table_7
  };


#endif /* #if defined USR_SUPPLIED_RT */

