
/************************************************************************/
/*  This file created from input file '..\..\mvl\usr\gse_mgmt\uca_obj.odf'
    Leaf Access Parameter (LAP) File: '..\..\mvl\usr\gse_mgmt\leafmap.xml'
	Created Mon Feb 25 19:13:40 2019
*/

#include "glbtypes.h"
#include "sysincs.h"
#include "mmsdefs.h"
#include "mms_pvar.h"
#include "mms_vvar.h"
#include "mvl_acse.h"
#include "uca_obj.h"


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
#else	/* !USE_RT_TYPE_2	*/
SD_CONST ST_CHAR *SD_CONST foMvlRtNames[] =
  {
  NULL
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
  maxMvlRtNames = 0;
  numMvlRtNames = 0;
  mvlRtNames = foMvlRtNames;
#endif
  mvl_num_types = 0 + mvl_max_dyn.types;

  if (mvl_num_types)
    mvl_type_ctrl = (MVL_TYPE_CTRL *) M_CALLOC (MSMEM_STARTUP, mvl_num_types, sizeof(MVL_TYPE_CTRL));
#if defined USR_SUPPLIED_RT
  u_mvl_start_init_rt_tbl (0, 0);
#endif


#if defined USR_SUPPLIED_RT
  u_mvl_end_init_rt_tbl ();
#endif
  }


/************************************************************************/
/* RUNTIME TYPE DATA */


#if !defined USR_SUPPLIED_RT
#endif /* #if defined USR_SUPPLIED_RT */


/************************************************************************/
/* UCA READ INDICATION FUNCTIONS                                           */
/* ReadInd functions are to put the primitive data in the dest buffer.    */
/* The app is to call 'mvlu_rd_prim_done' to let MVLU know that the data   */
/* is available for encoding.                                              */
/* This can be done in this function or asynchronously.                    */

/* UCA WRITE INDICATION FUNCTIONS                                          */
/* WriteInd functions are to move the primitive data from the src buffer   */
/* and deal with it, then call 'mvlu_wr_prim_done' to let MVLU know        */
/* that we are done with it.                                               */
/* This can be done in this function or asynchronously.                    */
/************************************************************************/

/************************************************************************/
/* Leaf Access Function Pointer Tables                    */
MVLU_RD_FUN_INFO mvluRdFunInfoTbl[] =
  {
    {_MVLU_LEAF_FUN_INIT (mvlu_null_rd_ind)}	/* 0=MVLU_NULL_RD_IND_INDEX	*/
  };
ST_INT mvluNumRdFunEntries = sizeof(mvluRdFunInfoTbl)/sizeof(MVLU_RD_FUN_INFO);

MVLU_WR_FUN_INFO mvluWrFunInfoTbl[] =
  {
    {_MVLU_LEAF_FUN_INIT (mvlu_null_wr_ind)}	/* 0=MVLU_NULL_WR_IND_INDEX	*/
  };
ST_INT mvluNumWrFunEntries = sizeof(mvluWrFunInfoTbl)/sizeof(MVLU_WR_FUN_INFO);

/************************************************************************/
