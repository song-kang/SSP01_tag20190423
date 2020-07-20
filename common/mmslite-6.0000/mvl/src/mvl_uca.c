/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	       1998-2005, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : mvl_uca.c						*/
/* PRODUCT(S)  : MMSEASE						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*	Special read/write processing functions for UCA and		*/
/*	IEC 61850 objects.						*/
/*									*/
/* NOTE: define MVL61850_CTL_DISABLE to avoid calling user functions	*/
/*	(u_mvl61850_ctl_oper_*) if 61850 Controls not needed.		*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 01/11/12  JRB           Chg all prim_num from ST_RTINT to ST_UINT for big vars.*/
/*			   Fix klocwork warnings w/ strncat_maxstrlen,etc.*/
/* 11/23/11  KCR           Added casting for proper 64bit compile       */
/* 04/01/11  JRB	   u_mvl_get_va_aa: find base var first,	*/
/*			   free arrCtrl.. on ANY error.			*/
/* 03/18/11  JRB	   mvlu_find_component: add arg, make external,	*/
/*			   handle array index in flatname, chg last 4	*/
/*			   args to output only, chk arr_index in range,	*/
/*			   avoid using uninitialized comp_name.		*/
/*			   Repl init_prim_info calls w/ inline code.	*/
/*			   Repl mvl_get_runtime calls w/ inline code.	*/
/*			   startElWrites: chk for Oper BEFORE loop.	*/
/* 12/08/10  JRB	   Add mvlu_flat_var_create/destroy functions.	*/
/* 11/19/10  JRB	   mvlu_find_component: make sure alt access	*/
/*			   array index is not out of range.		*/
/*			   Chg arr_index to unsigned to avoid conversion*/
/*			   from unsigned to signed.			*/
/* 09/28/10  JRB	   Don't allow AA_INDEX_RANGE, AA_ALL.		*/
/* 			   Repl asserts with error returns.		*/
/*			   mvlu_handle_alt_acc: ret err if processing fails*/
/* 02/25/10  JRB	   Chk mvl_var_create_derived return.		*/
/*			   Chk alt_access_done_out before using.	*/
/*			   startElWrites: init sboName="".		*/
/*			   More portable way to initialize objClone.	*/
/* 08/08/09  MDE     73    Fixed Linux warning				*/
/* 08/08/09  MDE     72    Fixed 'Disable chk_var_writable for IOS'	*/
/* 04/07/09  MDE     71    Disable chk_var_writable for IOS		*/
/* 01/27/09  JRB     70    Don't send write resp until ind is done.	*/
/*			   Don't allow write if any leaf unwritable.	*/
/*			   Call user func at start/end of complex write.*/
/* 01/07/09  JRB     69    Del S_LOCK for OPC_CLIENT in u_mvl_read_ind	*/
/*			   (already locked by mvl_comm_serve).		*/
/* 06/30/08  JRB     68    Repl cmd_executing w/ ctlState (more flexible)*/
/*			   Do SBO Ctrl checks only if Oper/Cancel is	*/
/*			   ONLY structure being written.		*/
/*			   Fix sboName buffer size.			*/
/* 05/06/08  JRB     67	   Set new cmd_executing flag.			*/
/* 03/03/08  JRB     66    Call mvl61850_ctl_req_done to do what's needed*/
/*			   when IEC 61850 control request completes.	*/
/* 02/18/08  JRB     65    Use new mvl_var_create_derived function.	*/
/* 02/07/08  JRB     64    Fix alignment of ptr passed to mvlu_clone..	*/
/* 12/18/07  MDE     63	   Fixed lastDollar crash for IOS		*/
/* 11/12/07  MDE     62	   Changed MMSOP_RDWR_USR_HANDLED to _RD_	*/
/* 07/20/07  JRB     61    getGnlVarNames: fix crash when flatname	*/
/*			   too long; add return code & check it.	*/
/* 05/09/07  JRB     60    Fix alt_acc arg passed to mvlu_find_component*/
/*			   Fix log macro args.				*/
/* 04/30/07  JRB     59    Fix to work with RUNTIME_TYPE change.	*/
/* 04/17/07  JRB     58	   Fix calc of offset_from_base & prim_num	*/
/*			   to work for Alt Access on arrays (corrects	*/
/*			   primData, prim_num, and prim_offset_base	*/
/*			   passed to leaf functions).			*/
/*			   mvlu_find_struct_comp: add args to compute	*/
/*			   offset & prim_num.				*/
/* 03/07/07  JRB     57    Add mvlu_get_leaf_val_int_any.		*/
/* 11/21/06  JRB     56    Send LastApplError if write of Oper, etc fails.*/
/* 10/30/06  JRB     55    Use new mvl_vmd_* object handling functions.	*/
/*			   mvlu_find_base_va: add args.			*/
/*			   u_mvl_get_va_aa: add args.			*/
/*			   u_gnl_ind_*: add net_info arg to		*/
/*			   elim use of global var "_mvl_curr_net_info".	*/
/*			   u_mvl_get_va_aa: do not set va->usr_ind_ctrl	*/
/*			    (done by calling functions).		*/
/* 10/26/06  JRB     54    Del unused local vars.			*/
/* 09/27/06  MDE     53	   Added MMSOP_RD_USR_HANDLED for IOS		*/
/* 09/13/06  JRB     52    mvlu_find_comp_type: allow non-dynamic types.*/
/* 08/09/06  JRB     51    Del u_mvl_get_nvl, u_mvl_free_nvl functions	*/
/*			   (not needed with new improved Foundry).	*/
/*			   Use "mvl_var_create/destroy" so all variables*/
/*			   created/destroyed in one place.		*/
/* 07/24/06  JRB     50    Chg some common ERR logs to FLOW.		*/
/* 03/27/06  JRB     49    Add more mvlu_get_leaf_* functions.		*/
/* 07/29/05  MDE     48    Fixed static data use for write handling	*/
/* 07/11/05  JRB     47    Call user fcts (u_mvl61850_ctl_oper_*) if	*/
/*			   !defined (MVL61850_CTL_DISABLE).		*/
/* 01/19/05  JRB     46	   u_gnl_ind_* return (-1) on error.		*/
/* 12/09/04  JRB     45    init_prim_info_recursive: fix ARR_END handling.*/
/*			   init_prim_info_arr: simplify & ret ST_VOID.	*/
/*			   Add mvlu_find_comp_type, mvlu_get_leaf_val*.	*/
/*			   Chg trim_branch_name to global mvlu_trim_..	*/
/*			   & simplify it using strrchr.			*/
/* 09/20/04  JRB     44    startElWrites: if writing "Oper" or "Cancel"	*/
/*			   struct, check SBO state.			*/
/*			   mvlu_wr_prim_done call mvlu_sbo_ctrl_free.	*/
/* 06/29/04  JRB     43    Del global var mvluUseStaticData, instead use*/
/*			   use_static_data flag in MVL_VAR_ASSOC for Read.*/
/*			   NEVER use static data for Write.		*/
/*			   startElReads, etc: add prim_info arg.	*/
/*			   Del elmntOffset, use prim_info->prim_offset.	*/
/* 11/24/03  JRB     42    getGnlVarNames: fix prefix len by using	*/
/*			   MAX_IDENT_LEN, chk overflow BEFORE writing,	*/
/*			   & add logging & asserts.			*/
/* 09/18/03  JRB     41    Allow alt acc on array of "nested" structures*/
/*			   Add some debug logging & extra comments.	*/
/* 05/02/03  JRB     40    switch(rt->el_tag): Use default for most cases*/
/* 04/04/03  JRB     39    Fix integrity/GI scan code so multiple	*/
/*			   concurrent scans don't corrupt one another.	*/
/* 12/20/02  JRB     38    Moved mvlu_set_leaf_param to mvluleaf.c	*/
/* 12/12/02  JRB     37    Use usr_resp_fun ptr to call scan done funcs.*/
/* 12/09/02  MDE     36    Made mvlu_find_uca_var global		*/
/* 11/27/02  MDE     35    Addded leaf indication handlers		*/
/* 			   Addded mvlu_find_rt_leaf			*/
/* 			   Addded mvlu_set_leaf_param 			*/
/* 11/29/01  MDE     34    Added GOOSE function pointer			*/
/* 11/14/01  EJV     33    Added support for new MMS type UtcTime:  	*/
/* 11/13/01  MDE     32    Added GOOSE scan support			*/
/* 05/21/01  MDE     31    Cleaned up memory allocation for SMEM	*/
/* 10/25/00  JRB     30    Del u_mvl & u_gnl funct ptrs. Call directly.	*/
/*			   Del mvlu_install (no longer needed).		*/
/* 08/18/00  JRB     29    Don't clear va_to_free. Need value later.	*/
/*			   mvlu_free_nvl free va->va_to_free only if	*/
/*			   it was allocated by mvlu_get_nvl.		*/
/* 08/18/00  RKR     28    Added rt fields to MVLU_ typedefs		*/
/* 07/13/00  JRB     27    Cleanup ms_comp_na.. chg for MVL_XNAME.	*/
/* 07/13/00  JRB     26    Use new ms_comp_name_find to get comp names.	*/
/* 07/13/00  JRB     25	   Move these functs to mvl_type.c:		*/
/*			     mvlu_add_rt_type, mvlu_free_rt_type.	*/
/* 06/21/00  MDE     24    Now copy base VA user_info to new VA		*/
/* 05/15/00  MDE     23    Now filder out too-long variable names	*/
/* 04/14/00  JRB     22    Lint cleanup.				*/
/* 04/05/00  RKR     21    Made MVL_XNAME a compile time option		*/
/* 04/03/00  RKR     20    Added the xName to UCA Rd and Wr Ind funs	*/
/* 03/30/00  RKR     19    Passed the expanded UCA var name to ind fun  */
/* 01/21/00  MDE     18    Now use MEM_SMEM for dynamic memory		*/
/* 12/20/99  MDE     17    Fix getArrAARtType to return SUCCESS/FAIL	*/
/* 11/03/99  JRB     16    Fix GetNameList if CA name = base var name.	*/ 
/* 09/30/99  EJV     15    Added slog macro to mvlu_rd_prim_done 	*/
/* 09/13/99  MDE     14    Added SD_CONST modifiers			*/
/* 09/07/99  MDE     13    Revised and enhanced the UCA report system	*/
/* 06/04/99  MDE     12    Now allow arrays as base VA type, other	*/
/* 			   minor changes to VA processing		*/
/* 06/04/99  MDE     11    Fixed memory leak for nested array alt acc	*/
/* 04/07/99  MDE     10    Logging improvements (fixed wrong AA log too)*/
/* 03/09/99  JRB     12    Fix illegal free of gnlNameBuf.		*/
/* 02/22/99  JRB     11    BUG FIX: Always start with clean "arrCtrl".	*/
/* 01/08/99  JRB     10    Use new "bsearch" object model. Don't use	*/
/*			   "_UCA_" prefix on va and nvl names.		*/
/* 12/11/98  MDE     09    Removed scope references from VA		*/
/* 11/17/98  MDE     08    Made mvlu_get_va_aa alloc space for name	*/
/* 11/16/98  MDE     07    Renamed internal functions (prefix '_')	*/
/* 09/21/98  MDE     06    Uninitialized ptr fix, Minor lint cleanup	*/
/* 08/11/98  MDE     05    Added UCA variable array support		*/
/* 07/13/98  MDE     04    Mixed scope NVL fixes, data alignment fix	*/
/* 06/29/98  MDE     03    Added report function pointers 		*/
/* 06/15/98  MDE     02    Changes to allow compile under C++		*/
/* 01/02/98  MDE     01    New						*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "glbsem.h"
#include "mmsdefs.h"
#include "mms_pvar.h"
#include "mms_vvar.h"
#include "mvl_uca.h"
#include "mvl_log.h"
#include "str_util.h"
#include "scl.h"	/* Need MAX_FLAT_LEN	*/

#if defined(MVL_UCA)	/* This entire module is only valid for UCA.	*/


/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/* PRIM_INFO struct:  extra info about primitive.	*/
typedef struct
  {
  ST_UINT prim_num;		/* index to data			*/
  ST_UINT prim_offset;		/* mem offset from start of var		*/
  ST_UINT prim_offset_base;	/* mem offset from start of "base" var	*/
  } PRIM_INFO;			/* extra info about primitive	*/

/************************************************************************/

static ST_VOID mvluDefGetVaDataBufFun (ST_INT service, MVL_VAR_ASSOC *va, 
				       ST_INT size);
static ST_VOID mvluDefFreeVaDataBufFun (ST_INT service, MVL_VAR_ASSOC *va);

/************************************************************************/
/* Read/Write leaf indication override handlers */

ST_VOID (*u_mvlu_leaf_rd_ind_fun)(MVLU_RD_VA_CTRL *mvluRdVaCtrl);
ST_VOID (*u_mvlu_leaf_wr_ind_fun)(MVLU_WR_VA_CTRL *mvluWrVaCtrl);

/************************************************************************/
/* STATIC VARIABLES, LOCAL DEFINES					*/

/* GNL Variables */
static ST_CHAR *gnlNameBuf;
static ST_CHAR *currGnlNamePos;

#if !defined(USPS_IOS)
/* Index to "u_no_write_allowed" function. It is set to correct value	*/
/* at startup by calling "mvlu_find_wr_ind_fun_index" (one time).	*/
static ST_RTINT idx_u_no_write_allowed = -2;	/* start with illegal val*/
#endif

/************************************************************************/
/* STATIC FUNCTIONS							*/

static ST_RET mvlu_find_struct_comp (ST_CHAR *compName,
	RUNTIME_TYPE **rtIo, ST_INT *numRtIo,
	ST_INT *offset_io,
	ST_UINT *prim_num_io); 

static ST_RET mvlu_handle_alt_acc (OBJECT_NAME *obj, ALT_ACCESS *alt_acc, 
				    MVL_ARR_CTRL *arrCtrl);
static ST_RET getArrAARtType (MVL_ARR_CTRL *arrCtrl, 
			RUNTIME_TYPE **pRt, ST_INT *pNumRt);
static ST_VOID cloneArrAA (ALT_ACC_EL *arrAa,  ALT_ACCESS *dest);

static ST_VOID mvlu_find_base_va (MVL_VMD_CTRL *vmd_ctrl, OBJECT_NAME *obj, MVL_NET_INFO *net_info, MVL_VAR_ASSOC **vaOut);

ST_RET getGnlVarNames (MVL_VAR_ASSOC *va, ST_CHAR *caPtr, 
			ST_CHAR **dest, ST_INT maxNames, 
			ST_INT *numNames, ST_BOOLEAN *moreFollowsOut);
static ST_VOID mvlu_clone_objname (OBJECT_NAME *dest, OBJECT_NAME *src, ST_CHAR *oname, ST_CHAR *dname);

static ST_VOID startArrRds (MVL_IND_PEND *indCtrl, MVLAS_READ_CTRL *rdCtrl,
		     MVLAS_RD_VA_CTRL *rdVaCtrl, RUNTIME_TYPE *rt, PRIM_INFO *prim_info);
static ST_VOID startElReads (MVL_IND_PEND *indCtrl, MVLAS_READ_CTRL *rdCtrl,
		     MVLAS_RD_VA_CTRL *rdVaCtrl, 
		     RUNTIME_TYPE *rt, ST_INT rt_num, PRIM_INFO *prim_info);
static ST_VOID startArrWrs (MVL_IND_PEND *indCtrl, MVLAS_WRITE_CTRL *wrCtrl,
		     MVLAS_WR_VA_CTRL *wrVaCtrl, RUNTIME_TYPE *rt, PRIM_INFO *prim_info);
static ST_VOID startElWrites (MVL_IND_PEND *indCtrl, MVLAS_WRITE_CTRL *wrCtrl,
	  	      MVLAS_WR_VA_CTRL *wrVaCtrl, 
		      RUNTIME_TYPE *rt, ST_INT rt_num, PRIM_INFO *prim_info);
static ST_INT countPrimEl (MVL_VAR_ASSOC *va, RUNTIME_TYPE *rt, ST_INT rt_num);

static ST_RET chk_var_writable (RUNTIME_TYPE *rt, ST_INT rt_num);
static ST_RET chk_write_resp_ready (MVL_IND_PEND *indCtrl);
static ST_VOID set_write_resp_dae (MVLAS_WR_VA_CTRL *wrVaCtrl,
	ST_INT16 dae);	/* DataAccessError code	*/

/************************************************************************/
/* Global function pointers. May be set by user.			*/
/************************************************************************/
/* This function pointer called BEFORE all write leaf functions.	*/
/* NOTE: called only for complex variables (struct or array).		*/
ST_RET (*u_mvl_wr_ind_var_start)(MVL_IND_PEND *indCtrl,
	MVLAS_WR_VA_CTRL *wrVaCtrl);	/* current var in list of var	*/

/* This function pointer called AFTER all write leaf functions.		*/
/* NOTE: called only for complex variables (struct or array).		*/
ST_RET (*u_mvl_wr_ind_var_end)(MVL_IND_PEND *indCtrl,
	MVLAS_WR_VA_CTRL *wrVaCtrl);	/* current var in list of var	*/

/************************************************************************/
/************************************************************************/
/* MANUFACTURED VARIABLE RESOLUTION FUNCTIONS				*/
/************************************************************************/
/*			u_mvl_get_va_aa					*/
/************************************************************************/
/* This function is called from MVL when it is unable to find a 	*/
/* configured MMS server variable for a READ, WRITE, or GET VARIABLE 	*/
/* ACCESS ATTRIBUTES indication. 					*/

MVL_VAR_ASSOC *u_mvl_get_va_aa (MVL_VMD_CTRL *vmd_ctrl, ST_INT service, OBJECT_NAME *obj, 
			   MVL_NET_INFO *netInfo,
		      	   ST_BOOLEAN alt_access_pres,
		      	   ALT_ACCESS *alt_acc,
		      	   ST_BOOLEAN *alt_access_done_out)
  {
ST_CHAR *name;
MVL_VAR_ASSOC *va;
MVL_VAR_ASSOC *baseVa;
ST_RET rc;
OBJECT_NAME objClone;		/* clone of "obj"		*/
ST_CHAR oname[MAX_IDENT_LEN+1];	/* objClone will include ptr to this	*/
ST_CHAR dname[MAX_IDENT_LEN+1];	/* objClone will include ptr to this	*/
RUNTIME_TYPE *ucaRt;
ST_INT numUcaRt;
ST_CHAR *subStart;
ST_INT subTypeId;
MVL_ARR_CTRL arrCtrl;
ST_CHAR *flatname;	/* flattened variable name (for mvlu_find_component)*/
RUNTIME_TYPE *comp_rt;		/* set by mvlu_find_component	*/
ST_INT comp_num_rt;		/* set by mvlu_find_component	*/
ST_INT offset_from_base;	/* set by mvlu_find_component	*/
ST_UINT prim_num;		/* set by mvlu_find_component	*/

/* We need to find the base variable for this variable 			*/
/* No point continuing if base variable can't be found.			*/
  mvlu_find_base_va (vmd_ctrl, obj, netInfo, &baseVa);
  if (baseVa == NULL)
    {
    MVL_LOG_NERR1 ("Could not find base variable from '%s'", obj->obj_name.vmd_spec);
    return (NULL);
    }

/* Make a working copy of the variable name (mvlu_handle_alt_acc may change it)	*/
  mvlu_clone_objname (&objClone, obj, oname, dname);

/* We will handle any alternate access here, by creating the UCA from 	*/
/* variable name.							*/
  memset (&arrCtrl, 0, sizeof (MVL_ARR_CTRL));	/* start with clean "arrCtrl"*/
  if (alt_access_pres)
    {
    rc = mvlu_handle_alt_acc (&objClone, alt_acc, &arrCtrl);
    if (rc)
      return (NULL);	/* can't process AA. error msg already logged	*/
    /* If caller's ptr is valid, set value to which it points.	*/
    if (alt_access_done_out)
      *alt_access_done_out = SD_TRUE;
    }

  /* Use mvlu_find_component to compute offset_from_base & prim_num.	*/
  /* Look for '$' in name. If found, point after it, else use NULL.	*/
  flatname = strstr (obj->obj_name.vmd_spec, "$");
  if (flatname != NULL)
    flatname++;	/* point after the '$'	*/
  /* NOTE: mvlu_find_component changes comp_rt, comp_num_rt,	*/
  /*       offset_from_base, prim_num.				*/
  /* NOTE: offset_from_base, prim_num different for each array elem.*/
  /* CRITICAL: mvlu_find_component expects alt_acc=NULL if Alt Access	*/
  /*           Spec NOT present. Make sure "alt_acc" is set correctly.	*/
  if (!alt_access_pres)
    alt_acc = NULL;	/* CRITICAL	*/
  rc = mvlu_find_component (baseVa, flatname, alt_acc,
           &comp_rt, &comp_num_rt, &offset_from_base, &prim_num);
  if (rc)
    {
    /* CRITICAL: if mvlu_handle_alt_acc allocated "arrCtrl.alt_acc.aa"	*/
    /*           (through cloneArrAA), it must be freed now.		*/
    if (arrCtrl.alt_acc.aa)
      M_FREE (MSMEM_MVLU_AA, arrCtrl.alt_acc.aa);
    return (NULL);	/* if this fails, stop now.	*/
    }

/* Now resolve this variable's type given it's name and the base	*/
/* type.								*/
  ucaRt    = baseVa->type_ctrl->rt;
  numUcaRt = baseVa->type_ctrl->num_rt;

/* Check to see if this is for base var... name has no embedded '$'	*/
/* If is a derived variable we need to create from the variable name	*/
/* 'path' and runtime type.						*/
  name = objClone.obj_name.vmd_spec;	/* We know this is a union ...	*/
  subStart = strstr (name,"$");		/* Skip the outer (base) name 	*/
  if (subStart)
    {
  /* This is a derived variable, and we have the base UCA variable	*/
  /* Using the base runtime table and the variable name, we can get the	*/
  /* subset type for this variable					*/

    ++subStart;
    rc = mvlu_find_uca_var (&ucaRt, &numUcaRt, subStart);
    if (rc != SD_SUCCESS)
      {
      MVL_LOG_NERR1 ("Error - could not find subcomponent '%s'", subStart);
      if (arrCtrl.alt_acc.aa)
        M_FREE (MSMEM_MVLU_AA, arrCtrl.alt_acc.aa);
      return (NULL);
      }
    }


/* If this is an alternate access on an array, we need to copy the	*/
/* RT type so we can modify the number of elements and total size	*/
  if (arrCtrl.arrAltAccPres == SD_TRUE)
    {
    rc = getArrAARtType (&arrCtrl, &ucaRt, &numUcaRt);
    if (rc != SD_SUCCESS)
      {
      M_FREE (MSMEM_MVLU_AA, arrCtrl.alt_acc.aa);
      return (NULL);
      }
    }

  /* Make sure type computed matches what mvlu_find_component computed.	*/
  if (comp_num_rt != numUcaRt ||
      comp_rt->el_tag != ucaRt->el_tag ||
      comp_rt->mvluTypeInfo.prim_count != ucaRt->mvluTypeInfo.prim_count)
    {
    MVL_LOG_ERR1 ("Error creating temporary type definition for variable '%s'", obj->obj_name.vmd_spec);
    if (arrCtrl.alt_acc.aa)
      M_FREE (MSMEM_MVLU_AA, arrCtrl.alt_acc.aa);
    return (NULL);
    }

/* OK, now we have the sub-runtime type, go ahead and create a temp 	*/
/* RT type and variable association.					*/
  rc = mvlu_add_rt_type (ucaRt, numUcaRt, &subTypeId);
  if (rc != SD_SUCCESS)
    {
    MVL_LOG_NERR0 ("Error - could not add temp RT type");
    if (arrCtrl.alt_acc.aa)
      M_FREE (MSMEM_MVLU_AA, arrCtrl.alt_acc.aa);
    return (NULL);
    }

/* Create "derived" variable from baseVa.				*/
/* NOTE: This var is NOT added to list so its name is NOT sent		*/
/*       in GetNameList responses.					*/
  va = mvl_var_create_derived (objClone.obj_name.vmd_spec, subTypeId,
              baseVa,
              offset_from_base,
              prim_num);

  if (va!=NULL)
    {
/* CRITICAL: If use_static_data==SD_FALSE, mvl_var_create_derived could	*/
/*    not set "va->data". Must call this function to allocate it.	*/
/* NOTE: Since this code executes every time a variable is accessed,	*/
/*    it requires MANY more allocations when use_static_data==SD_FALSE.	*/
    if (baseVa->use_static_data==SD_FALSE)
      mvluDefGetVaDataBufFun (service, va, ucaRt->offset_to_last);

#if defined(MVLU_USE_REF)
    va->ref = ucaRt->mvluTypeInfo.ref;
#endif

/* Copy the array AA information */
    memcpy (&va->arrCtrl, &arrCtrl, sizeof (MVL_ARR_CTRL));
    va->arrCtrl.curr_index = arrCtrl.low_index;
    }	/* va!=NULL	*/
  else
    {	/* va==NULL (error)	*/
    if (arrCtrl.alt_acc.aa)
      M_FREE (MSMEM_MVLU_AA, arrCtrl.alt_acc.aa);
    }

/* Good work, we are done here.	*/
  return (va);
  }

/************************************************************************/
/*			getArrAARtType					*/
/************************************************************************/

static ST_RET getArrAARtType (MVL_ARR_CTRL *arrCtrl, 
			RUNTIME_TYPE **pRt, ST_INT *pNumRt)
    {
ST_INT numRt;
ST_INT i, j;
RUNTIME_TYPE *rt;
RUNTIME_TYPE *endRt;
RUNTIME_TYPE *newRt;
ALT_ACCESS *alt_acc;
ALT_ACC_EL *alt_acc_el;	/* ptr to current entry in alt acc array	*/
ST_INT nest_level = 0;

  rt = *pRt;
  numRt = *pNumRt;

/* Check to see if further nesting with this AA selection ...	*/
  if (arrCtrl->nested == SD_FALSE)
    {
  /* If just one element, need to lose the outer array */
    if (arrCtrl->num_elmnts == 1)
      {
      numRt -= 2;
      ++rt;
      }
    }
  else	/* Further nesting is requested, let's do it!	*/
    {
  /* We only support a single drill down AA	*/
    if (arrCtrl->num_elmnts > 1)
      {
      MVL_LOG_NERR0 ("AA resolution problem - multiple elements and nested");
      return (SD_FAILURE);
      }
    numRt -= 2;
    ++rt;

  /* OK, we now need to find the type of the component ... */  
    alt_acc = &arrCtrl->alt_acc;
    /* Only support AA_COMP and AA_COMP_NEST for now.		*/
    /* If AA_COMP_NEST is used, this is recursive process.	*/
    for (j=0; j < alt_acc->num_aa; j++)
      {
      alt_acc_el = &alt_acc->aa[j];
      if (alt_acc_el->sel_type == AA_COMP || alt_acc_el->sel_type == AA_COMP_NEST)
        {
        if (alt_acc_el->sel_type == AA_COMP_NEST)
          nest_level++;
        /* find this component name within the current RUNTIME_TYPE array*/
        for (i = 0; i < numRt; ++i, ++rt)
          {
          if (!strcmp (alt_acc_el->u.component, ms_comp_name_find (rt)))
            break;
          }
        if (i >= numRt)
          {
          MVL_LOG_NERR1 ("AA resolution problem - could not find component '%s'", 
					alt_acc_el->u.component);
          return (SD_FAILURE);
	  }
        if (rt->el_tag == RT_STR_START)
          numRt = rt->u.str.num_rt_blks+2;
        else if (rt->el_tag == RT_ARR_START)
          numRt = rt->u.arr.num_rt_blks+2;
        else
          numRt = 1;
        }
      else if (alt_acc_el->sel_type == AA_END_NEST)
        nest_level--;
      else
        {
        MVL_LOG_NERR0 ("AA resolution problem - complex nested AA on array");
        return (SD_FAILURE);
        }
      }		/* end "for" loop	*/
    if (nest_level != 0)
      {
      MVL_LOG_NERR0 ("AA resolution problem - invalid nesting");
      return (SD_FAILURE);
      }
    }

/* OK, now copy the runtime type elements into a new one so we can fool	*/
/* with it.								*/
  newRt = (RUNTIME_TYPE *) M_MALLOC (MSMEM_DYN_RT, numRt * sizeof (RUNTIME_TYPE));
  memcpy (newRt, rt, numRt * sizeof (RUNTIME_TYPE));

/* Adjust the total size of the type, using scaling 			*/
  if (arrCtrl->num_elmnts > 1)
    {
    newRt->offset_to_last = rt[1].offset_to_last * arrCtrl->num_elmnts;

/* Now set the number of elements as desired				*/
    newRt->u.arr.num_elmnts = arrCtrl->num_elmnts;
    endRt = newRt + (newRt->u.arr.num_rt_blks + 1);
    endRt->u.arr.num_elmnts = arrCtrl->num_elmnts;

/* We don't want end of array padding */
    endRt->el_size = 0;
    }

  *pRt = newRt;
  *pNumRt = numRt;
  return (SD_SUCCESS);
  }

/************************************************************************/
/*                       mvlu_handle_alt_acc				*/
/************************************************************************/
/* Here we deal with an alternate access. We will simply add the 	*/
/* sub-name components to the base name to create a fully qualified UCA */
/* sub-var name								*/
/* Of course, this only handles the most simple form of alternate	*/
/* access correctly ...							*/

static ST_RET mvlu_handle_alt_acc (OBJECT_NAME *obj, ALT_ACCESS *alt_acc,
				    MVL_ARR_CTRL *arrCtrl)
  {
ST_INT i;
ST_CHAR *name;
ST_BOOLEAN done;

  name = obj->obj_name.vmd_spec;	/* We know this is a union ...	*/
  arrCtrl->nested = SD_FALSE;
  done = SD_FALSE;
  for (i = 0; i < alt_acc->num_aa && !done; ++i)
    {
    switch (alt_acc->aa[i].sel_type)
      {
      case AA_COMP :
      case AA_COMP_NEST :
        strcat (name, "$");
        strcat (name, alt_acc->aa[i].u.component);
      break;

      case AA_INDEX_NEST :
        arrCtrl->nested = SD_TRUE;
	cloneArrAA (&alt_acc->aa[i],  &arrCtrl->alt_acc);
	/* Lets fall through into common code ... */
      case AA_INDEX :
        arrCtrl->arrAltAccPres = SD_TRUE;
        /* Make sure index doesn't exceed max supported.	*/
        if (alt_acc->aa[i].u.index > 0x7FFF)
          {
          MVL_LOG_ERR1 ("Array index = %lu exceeds max supported.", (ST_ULONG) alt_acc->aa[i].u.index);
          return (SD_FAILURE);
          }
        arrCtrl->low_index = (ST_RTINT) alt_acc->aa[i].u.index;
        arrCtrl->num_elmnts = 1;
        done = SD_TRUE;
      break;

#if 0	/* DEBUG: DO NOT enable this code unless mvlu_find_component supports these cases.*/
      case AA_INDEX_RANGE_NEST :
        arrCtrl->nested = SD_TRUE;
	cloneArrAA (&alt_acc->aa[i],  &arrCtrl->alt_acc);
	/* Lets fall through into common code ... */
      case AA_INDEX_RANGE :
        arrCtrl->arrAltAccPres = SD_TRUE;
        arrCtrl->low_index = (ST_RTINT) alt_acc->aa[i].u.ir.low_index;
        arrCtrl->num_elmnts = (ST_RTINT) alt_acc->aa[i].u.ir.num_elmnts;
        done = SD_TRUE;
      break;

      case AA_ALL:
      case AA_ALL_NEST :
        arrCtrl->nested = SD_TRUE;
	cloneArrAA (&alt_acc->aa[i],  &arrCtrl->alt_acc);
        arrCtrl->arrAltAccPres = SD_TRUE;
        arrCtrl->low_index = 0;
        arrCtrl->num_elmnts = 0;	/* 'all' flag */
        done = SD_TRUE;
      break;
#endif

      case AA_END_NEST :
        done = SD_TRUE;
      break;

      default:
        MVL_LOG_NERR1 ("Error: Invalid alt access sel_type = %d", 
					alt_acc->aa[i].sel_type);
      return (SD_FAILURE);
      }
    }
  return (SD_SUCCESS);
  }

/************************************************************************/
/*				cloneArrAA				*/
/************************************************************************/

static ST_VOID cloneArrAA (ALT_ACC_EL *arrAa,  ALT_ACCESS *dest)
  {
ST_INT elCount;
ST_INT nestLevel;
ALT_ACC_EL *aaEl;

/* First let's count how many we need */
  nestLevel = 0;
  elCount = 0;
  aaEl = arrAa + 1;
  while (SD_TRUE)
    {
    if (nestLevel == 0 && aaEl->sel_type == AA_END_NEST)
      break;

    ++elCount;
    if (aaEl->sel_type == AA_COMP_NEST ||
        aaEl->sel_type == AA_INDEX_NEST ||
        aaEl->sel_type == AA_INDEX_RANGE_NEST ||
        aaEl->sel_type == AA_ALL_NEST)
      {
      ++nestLevel;
      } 
    if (aaEl->sel_type == AA_END_NEST)
      {
      --nestLevel;
      } 
    ++aaEl;
    } 

/* OK, now just calloc and copy the nested alternate access */
  if (elCount)
    {
    dest->num_aa = elCount;
    dest->aa = (ALT_ACC_EL *) M_MALLOC (MSMEM_MVLU_AA, elCount * sizeof (ALT_ACC_EL));
    memcpy (dest->aa, arrAa+1, elCount * sizeof (ALT_ACC_EL));
    }
  else
    {
    MVL_LOG_NERR0 ("Nested AA construction problem");
    }
  }

/************************************************************************/
/*                       mvlu_find_base_va				*/
/************************************************************************/
/* This function takes a MMS variable name and determines the type ID	*/
/* for the base type for the variable. This is done here by 		*/
/* extracting the name root then looking for a configured variable	*/
/* of that name.							*/
static ST_VOID mvlu_find_base_va (MVL_VMD_CTRL *vmd_ctrl, OBJECT_NAME *obj, MVL_NET_INFO *net_info, MVL_VAR_ASSOC **vaOut)
  {
MVL_VAR_ASSOC *va;
ST_CHAR *p;
ST_CHAR *name;
OBJECT_NAME objClone;		/* clone of "obj"		*/
ST_CHAR oname[MAX_IDENT_LEN+1];	/* objClone will include ptr to this	*/
ST_CHAR dname[MAX_IDENT_LEN+1];	/* objClone will include ptr to this	*/

  mvlu_clone_objname (&objClone, obj, oname, dname);
  name = objClone.obj_name.vmd_spec;	/* We know this is a union ...	*/

/* See if this variable name has embedded '$', and if so wack it so	*/
/* that we have the base name to work with.				*/
  p = strstr (name,"$");
  if (p)
    *p = 0;

  va = mvl_vmd_find_var (vmd_ctrl, &objClone, net_info);
  if (va && (va->flags & MVL_VAR_FLAG_UCA) == 0)
    va = NULL;	/* Found va but not UCA variable, so don't return it.	*/
  *vaOut = va;
  }

/************************************************************************/
/*                       mvlu_find_uca_var				*/
/************************************************************************/
/* This function searches a runtime type for the given UCA variable 	*/
/* name. It does this by breaking the UCA name into its components	*/
/* and then finding the name in the current level of the runtime type.	*/

ST_RET mvlu_find_uca_var (RUNTIME_TYPE **rtIo, ST_INT *numRtIo, 
				 ST_CHAR *varName)
  {
ST_CHAR nameBuf[MAX_IDENT_LEN+1];
ST_CHAR *nameToFind;
ST_CHAR *compEnd;
ST_BOOLEAN nameDone;
ST_RET ret;
ST_INT offset=0;	/* mvlu_find_struct_comp needs it but not used	*/
ST_UINT prim_num=0;	/* mvlu_find_struct_comp needs it but not used	*/

/* Note that varName does not have the base name prefix */
  strcpy (nameBuf, varName);	
  nameToFind = nameBuf;
  nameDone = SD_FALSE;
  while (nameDone == SD_FALSE)
    {
  /* Isolate the component name for this level, removing subcomp names	*/
    compEnd = strstr (nameToFind, "$");
    if (compEnd != NULL)
      *compEnd = 0;
    else			/* This is the last nest level		*/
      nameDone = SD_TRUE;

  /* Find the component name in the current runtime type nest level	*/
    ret = mvlu_find_struct_comp (nameToFind, rtIo, numRtIo, &offset, &prim_num);
    if (ret == SD_FAILURE)
      {
      /* Many things can cause this so just use FLOW Logging.	*/
      MVLU_LOG_FLOW2 ("Could not find name component %s from name %s",
				nameToFind, varName);
      return (SD_FAILURE);
      }

  /* OK, we now have found the component in the runtime type, and our	*/
  /* runtime pointer and numRt reflect the sub-runtime type.		*/
  /* next component.							*/

  /* Prepare to find the next level component name			*/
    nameToFind = compEnd+1;
    }
  return (SD_SUCCESS);
  }

/************************************************************************/
/*                       mvlu_find_struct_comp				*/
/************************************************************************/
/* This function searches a structure runtime type for the given  	*/
/* component name at the outer level.					*/
/* NOTE: modifies (*rtIo), (*numRtIo), (*offset_io), (*prim_num_io).	*/

static ST_RET mvlu_find_struct_comp (ST_CHAR *compName,
	RUNTIME_TYPE **rtIo, ST_INT *numRtIo,
	ST_INT *offset_io,
	ST_UINT *prim_num_io) 
  {
RUNTIME_TYPE *rt;
RUNTIME_TYPE *endRt;
ST_BOOLEAN foundRt;

  foundRt = SD_FALSE;
  rt = *rtIo;
  endRt = rt + *numRtIo;
  if (rt->el_tag != RT_STR_START)
    {
    MVL_LOG_NERR0 ("Find struct comp: First RT is not structure start");
    return (SD_FAILURE);
    }
  ++rt;		/* Skip the structure start */

  while (rt < endRt)
    {
    if (!strcmp (compName, ms_comp_name_find (rt)))
      {
      *rtIo = rt;
      foundRt = SD_TRUE;
      }
    switch (rt->el_tag)
      {
      case RT_STR_START :
        if (foundRt == SD_TRUE)
          {
          *numRtIo = rt->u.str.num_rt_blks+2;
          return (SD_SUCCESS);
          }
        /* Not found yet.	*/
        *offset_io += rt->offset_to_last;	/* adjust offset	*/
        *prim_num_io += rt->mvluTypeInfo.prim_count;	/* adjust prim_num*/
        rt += rt->u.str.num_rt_blks+2;	/* Skip the structure contents	*/
      break;

      case RT_ARR_START :
        if (foundRt == SD_TRUE)
          {
          *numRtIo = rt->u.arr.num_rt_blks+2;
          return (SD_SUCCESS);
          }
        /* Not found yet.	*/
        *offset_io += rt->offset_to_last;	/* adjust offset	*/
        *prim_num_io += rt->mvluTypeInfo.prim_count;	/* adjust prim_num*/
        rt += rt->u.arr.num_rt_blks+1; /* Skip the array contents	*/
      break;

      case RT_STR_END :
      case RT_ARR_END :
        ++rt;
      break;

      default:
        if (foundRt == SD_TRUE)
          {
          *numRtIo = 1;
          return (SD_SUCCESS);
          }
        /* Not found yet.	*/
        *offset_io += rt->offset_to_last;	/* adjust offset	*/
        *prim_num_io += rt->mvluTypeInfo.prim_count;	/* adjust prim_num*/
        ++rt;
      break;
      }
    }
  return (SD_FAILURE);
  }

/************************************************************************/
/*				u_mvl_free_va				*/
/************************************************************************/
/* MVL calls this function when it is through with it. We will free	*/
/* the data buffer, and then the VA (unless it was a base VA).		*/

ST_VOID u_mvl_free_va (ST_INT service, MVL_VAR_ASSOC *va, 
			    MVL_NET_INFO *netInfo)
  {
/* If "va->data" was allocated, free it now.	*/
  if (va->base_va->use_static_data==SD_FALSE)
    mvluDefFreeVaDataBufFun(service, va);

/* If this was not a 'base' VA, free the derived type */
  if ( (va->flags & MVL_VAR_FLAG_UCA) == 0)
    {

  /* See if we allocated the runtime type ... */
    if (va->arrCtrl.arrAltAccPres == SD_TRUE)
      {
      M_FREE (MSMEM_DYN_RT, va->type_ctrl->rt);
      if (va->arrCtrl.nested == SD_TRUE)
        M_FREE (MSMEM_MVLU_AA, va->arrCtrl.alt_acc.aa);
      }

    mvlu_free_rt_type (va->type_id);
    mvl_var_destroy (va);
    }
  }

/************************************************************************/
/************************************************************************/
/* MANUFACTURED VARIABLE_LIST RESOLUTION FUNCTIONS			*/
/************************************************************************/
/* u_mvl_get_nvl, u_mvl_free_nvl functions deleted (no longer needed).	*/
/* They were only needed because Foundry could not find the variables	*/
/* for the NVL to set "vl->entries". But new improved Foundry		*/
/* generates code like the following (only done once at startup,	*/
/* so it is much more efficient):					*/
/*
 *  varObjName.obj_name.vmd_spec = "DI$Name";
 *  varObjName.object_tag = DOM_SPEC;
 *  varObjName.domain_id = mvl_vmd.dom_tbl[6]->name;
 *  vl->entries [0] = u_mvl_get_va_aa (MMSOP_INFO_RPT, &varObjName, NULL, SD_FALSE, NULL, NULL);
 */

/************************************************************************/
/************************************************************************/
/* GET NAMELIST HELPER FUNCTIONS					*/
/* These functions are necessary because MVL does not know about our	*/
/* manufactured variables and variable lists. We will fill in part of 	*/
/* the namelist response data structure.				*/
/************************************************************************/
/************************************************************************/
/*			u_gnl_ind_vars					*/
/************************************************************************/

ST_INT u_gnl_ind_vars (MVL_NET_INFO *net_info, NAMELIST_REQ_INFO *req_info, 
			     ST_CHAR **ptr, ST_BOOLEAN *moreFollowsOut, 
			     ST_INT maxNames)
  {
ST_INT i;
ST_INT numRespNames;
ST_CHAR caBuf[100];
ST_CHAR *caPtr;
ST_CHAR *p;
ST_INT numVa;
ST_INT v;
MVL_VAR_ASSOC **va;
MVL_AA_OBJ_CTRL *aa;
MVL_DOM_CTRL *domCtrl;


  gnlNameBuf = (ST_CHAR *) M_CALLOC (MSMEM_MVLU_GNL, 1, maxNames * (MAX_IDENT_LEN +1));

/* Start by finding the VA that we should start with 			*/
/* Get the variable associations in the selected scope			*/
  numVa = 0;
  if (req_info->objscope == VMD_SPEC)
    {
    numVa = mvl_vmd.num_var_assoc;
    va = mvl_vmd.var_assoc_tbl;
    }
  else if (req_info->objscope == DOM_SPEC)
    {
    domCtrl = mvl_vmd_find_dom (&mvl_vmd, req_info->dname);
    if (domCtrl)
      {
      numVa = domCtrl->num_var_assoc;
      va = domCtrl->var_assoc_tbl;
      }
    else
      {
      MVL_LOG_NERR1 ("GetNameList variables: Domain '%s' not found", req_info->dname);
      *moreFollowsOut = SD_FALSE;
      return (-1);	/* error. This triggers error response	*/
      }
    }
  else  /* AA_SPEC */
    {
    aa = (MVL_AA_OBJ_CTRL *) net_info->aa_objs;
    if (aa)
      {
      numVa = aa->num_var_assoc;
      va = aa->var_assoc_tbl;
      }
    else
      {
      *moreFollowsOut = SD_FALSE;
      return (-1);	/* error. This triggers error response	*/
      }
    }

/* Take care of the 'continue after' business as necessary. Note that 	*/
/* we will only look at the base name.					*/
  i = 0;
  if (req_info->cont_after_pres)
    {
    caPtr = req_info->continue_after;
    strcpy (caBuf, req_info->continue_after);
    p = strstr (caBuf,"$");
    if (p)
      *p = 0;

    while (i < numVa)
      {
      p = va[i]->name;
      v = strcmp (caBuf, p);
      if (v == 0)	  	/* Exact match				*/
        break;

      if (v < 0) 		/* CA is less than real VA name		*/
        {
        if (i > 0)		/* Start with the previous VA		*/
          --i;
        break;
        }
      ++i;			/* We have not found our place yet ...	*/
      }
    }
  else
    caPtr = NULL;

  *moreFollowsOut = SD_FALSE;

  currGnlNamePos = gnlNameBuf;

/* OK, va[i] is where we start putting together our names		*/
  for (numRespNames = 0; numRespNames < maxNames && i < numVa; ++i)
    {
    ST_INT j;
    ST_INT numRespNamesStart = numRespNames;	/* save count modified by fct*/
    if (getGnlVarNames (va[i], caPtr, &ptr[numRespNames], 
		  maxNames, &numRespNames, moreFollowsOut) != SD_SUCCESS)
      return (-1);	/* error response will be sent	*/

    MVLU_LOG_DEBUG0 ("Names returned from getGnlVarNames");
    for (j = numRespNamesStart; j<numRespNames; j++)
      MVLU_LOG_CDEBUG2 ("[%d]%s", j, ptr[j]);

    caPtr = NULL;
    }
  if (i < numVa)
    *moreFollowsOut = SD_TRUE;
    
  return (numRespNames);
  }

/************************************************************************/
/*			getGnlVarNames					*/
/************************************************************************/

/* GNL Variables defines */
#define MAX_NEST_LEVEL	10

ST_RET getGnlVarNames (MVL_VAR_ASSOC *va, ST_CHAR *caPtr, 
			ST_CHAR **dest, ST_INT maxNames, 
			ST_INT *numNames, ST_BOOLEAN *moreFollowsOut)
  {
RUNTIME_TYPE *ucaRt;
ST_INT numUcaRt;
ST_INT i;
ST_CHAR nameBuf[MAX_IDENT_LEN+1];
ST_INT nameCount;
ST_INT maxSortedNum;
ST_INT sortedNum;
ST_INT startSortedNum;
ST_CHAR *subStart;
ST_RET rc;
ST_INT baseIndexOffset;
ST_INT maxRetNames;
/* prefix len should never reach MAX_IDENT_LEN but could get very close*/
ST_CHAR namePrefix[MAX_NEST_LEVEL][MAX_IDENT_LEN+1];
ST_INT nestLevel;
size_t strLen;
ST_INT get;
ST_INT put;
ST_INT numNewNames;
ST_BOOLEAN compress;
ST_CHAR *comp_name;		/* component name	*/

  if ( (va->flags & MVL_VAR_FLAG_UCA) == 0)
    {  
    ++(*numNames);
    dest[0] = va->name;
    return (SD_SUCCESS);
    }

  memset (namePrefix, 0, sizeof(namePrefix));
  strcpy (namePrefix[0], va->name);

/* First we need to find the starting 'sortedNum', based on the CA name	*/
  nameCount = *numNames;
  startSortedNum = 1;
  if (caPtr != NULL)
    {
    baseIndexOffset = 0;
    /* CA name could be base name, or derived name.			*/
    if (strcmp (caPtr, va->name) == 0)
      {	/* CA name equals base name. Start with next name AFTER base.	*/
      startSortedNum = 1;
      }
    else if ((subStart = strstr (caPtr,"$")) == NULL)	/* Skip the base name*/
      {
      MVL_LOG_ERR1 ("Problem finding sub-type in '%s'", caPtr);
      return (SD_FAILURE);
      }
    else
      {
      ucaRt    = va->type_ctrl->rt;
      numUcaRt = va->type_ctrl->num_rt;
      ++subStart; 
      rc = mvlu_find_uca_var (&ucaRt, &numUcaRt, subStart);
      if (rc == SD_SUCCESS)
        startSortedNum = ucaRt->mvluTypeInfo.sortedNum + 1;
      }
    }
  else	/* No continue after, we need to include the base name */
    {
    dest[0] = va->name;
    ++nameCount;
    baseIndexOffset = 1;
    --maxNames;
    }
  maxRetNames = maxNames - *numNames;
  maxSortedNum = startSortedNum + maxRetNames - 1;

/* OK, now we start doing the real thing. Derive names for this 	*/
/* type, put them into the dest array.					*/
/* We will save those elements with 'sortedNum' between 		*/
/* 'startSortedNum' and 'maxSortedNum'					*/

  compress = SD_FALSE;
  ucaRt    = va->type_ctrl->rt;
  numUcaRt = va->type_ctrl->num_rt;

  /* This code assumes all UCA vars are structs, so first tag must be RT_STR_START*/
  /* DEBUG: change this to "assert"?					*/
  if (ucaRt->el_tag != RT_STR_START)
    MVL_LOG_ERR1 ("IEC/UCA type (type_id=%d) is not a struct. Cannot derive variable names for GetNameList response.",
                  va->type_id);

  nestLevel = 0;
  for (i = 0; i < numUcaRt; ++i, ++ucaRt)
    {
    sortedNum = ucaRt->mvluTypeInfo.sortedNum;
    comp_name = ms_comp_name_find (ucaRt);
    if (strlen (comp_name)  &&
        sortedNum >= startSortedNum && sortedNum <= maxSortedNum)
      {
      /* Construct flattened variable name. */
      strcpy (nameBuf, namePrefix[nestLevel]);
      strncat_maxstrlen (nameBuf, "$", sizeof(nameBuf)-1);
      rc = strncat_maxstrlen (nameBuf, comp_name, sizeof(nameBuf)-1);
      if (rc == SD_SUCCESS)
        {
        strLen = strlen (nameBuf);
        dest[sortedNum-startSortedNum+baseIndexOffset] = currGnlNamePos;
        strcpy (currGnlNamePos, nameBuf);
        currGnlNamePos += strLen +1;
        ++nameCount;
        }
      else
        {
        /* NOTE: should never happen if len checked in _mvlu_save_rt_uca_name at startup.*/
        MVL_LOG_ERR2 ("GetNameList error: Derived variable name '%s$%s' too long",
                       namePrefix[nestLevel], comp_name);
        compress = SD_TRUE;
        }
      }

    if (ucaRt->el_tag == RT_STR_START)
      {
      comp_name = ms_comp_name_find (ucaRt);
      if (strlen (comp_name))
        {
        /* Chk len is legal BEFORE writing (need room for 2 strings + '$').*/
        if (strlen (namePrefix[nestLevel]) + strlen (comp_name) + 1 <= MAX_IDENT_LEN)
	  {
          ++nestLevel;
          if (nestLevel <= 0 || nestLevel >= MAX_NEST_LEVEL)
            {
            MVL_LOG_ERR1 ("GetNameList error: structure nest level %d not supported.", nestLevel);
            return (SD_FAILURE);
            }
          strcpy (namePrefix[nestLevel], namePrefix[nestLevel-1]);
          strncat_maxstrlen (namePrefix[nestLevel], "$", MAX_IDENT_LEN);
          strncat_maxstrlen (namePrefix[nestLevel], comp_name, MAX_IDENT_LEN);
          }
        else
          {
          /* NOTE: should never happen if len checked in _mvlu_save_rt_uca_name at startup.*/
          MVL_LOG_ERR2 ("GetNameList error: Flattened variable name too long for struct '%s$%s'",
                       namePrefix[nestLevel], comp_name);
          MVL_LOG_CERR0 ("Flattened name of struct and all lower components cannot be returned");
          compress=SD_TRUE;
          /* Skip to end of struct	*/
          i += (ucaRt->u.str.num_rt_blks + 1);
          ucaRt += (ucaRt->u.str.num_rt_blks + 1);
          }
        }
      }
    else if (ucaRt->el_tag == RT_STR_END)
      --nestLevel;
    else if (ucaRt->el_tag == RT_ARR_START)
      {					     /* Skip the array elements	*/
      /* There is no way to create UCA names for objects inside an	*/
      /* array, so skip over the array.					*/
      i += (ucaRt->u.arr.num_rt_blks + 1);
      ucaRt += (ucaRt->u.arr.num_rt_blks + 1);
      }

    if (sortedNum > maxSortedNum)
      *moreFollowsOut = SD_TRUE;

    }

  /* DEBUG: at this point loop, 'nestLevel' should equal (-1). This may	*/
  /* sound strange, but nestLevel NOT incremented on first RT_STR_START,*/
  /* so last RT_STR_END makes it -1. Add assert here?			*/

  /* If we had to skip one or more names, we need to eliminate the	*/
  /* holes in the name pointer table.					*/
  if (compress == SD_TRUE)
    {
    get = 0;
    put = 0;
    numNewNames = nameCount - *numNames;
    while (put < numNewNames)
      {
      if (dest[get] != NULL)
        dest[put++] = dest[get];
      ++get;
      }
    }

  *numNames = nameCount;
  return (SD_SUCCESS);
  }

/************************************************************************/
/*                       u_gnl_done					*/
/************************************************************************/
/* This function is called after MVL has send the MMS response to a 	*/
/* GET_NAME_LIST indication. We will clean up memory resources.		*/

ST_VOID u_gnl_done (ST_INT16 mms_class,
			      NAMELIST_RESP_INFO *resp_info)
  {
  if (mms_class == MMS_CLASS_VAR)
    M_FREE (MSMEM_MVLU_GNL, gnlNameBuf);
  }

/************************************************************************/
/************************************************************************/
/*			u_gnl_ind_nvls					*/
/************************************************************************/

ST_INT u_gnl_ind_nvls (MVL_NET_INFO *net_info, NAMELIST_REQ_INFO *req_info, 
			     ST_CHAR **ptr, ST_BOOLEAN *moreFollowsOut, 
			     ST_INT maxNames)
  {
ST_INT v;
ST_INT i;
ST_INT num_obj;
MVL_AA_OBJ_CTRL *aa;
MVL_NVLIST_CTRL **vl;
MVL_DOM_CTRL *domCtrl;
ST_INT startIndex;
ST_INT numObjLeft;
ST_INT numRetNames;
ST_CHAR *name;

/* First get the number of objects and pointer to the object table	*/
  num_obj = 0;
  if (req_info->objscope == VMD_SPEC)
    {
    num_obj = mvl_vmd.num_nvlist;
    vl = mvl_vmd.nvlist_tbl;
    }
  else if (req_info->objscope == DOM_SPEC)
    {
    domCtrl = mvl_vmd_find_dom (&mvl_vmd, req_info->dname);
    if (domCtrl)
      {
      num_obj = domCtrl->num_nvlist;
      vl = domCtrl->nvlist_tbl;
      }
    else
      {
      MVL_LOG_NERR1 ("GetNameList NVL: Domain '%s' not found", req_info->dname);
      *moreFollowsOut = SD_FALSE;
      return (-1);	/* error. This triggers error response	*/
      }
    }
  else  /* AA_SPEC */
    {
    aa = (MVL_AA_OBJ_CTRL *) net_info->aa_objs;
    if (aa)
      {
      num_obj = aa->num_nvlist;
      vl = aa->nvlist_tbl;
      }
    else
      {
      *moreFollowsOut = SD_FALSE;
      return (-1);	/* error. This triggers error response	*/
      }
    }

  if (num_obj)
    {
  /* Take care of the 'continue after' business if necessary */
    startIndex = 0;
    if (req_info->cont_after_pres)
      {
      while (startIndex < num_obj)
        {
        name = vl[startIndex]->name;
        v = strcmp (req_info->continue_after, name);
        if (v == 0)
          {
          ++startIndex;		/* Index to the next one		*/
          break;
          }
        if (v < 0) 
          break;
  
        ++startIndex;		/* We have not found our place yet ...	*/
        }
      }

    numObjLeft = num_obj-startIndex;
    numRetNames = min(numObjLeft, maxNames);
    if (numRetNames < numObjLeft)
      *moreFollowsOut = SD_TRUE;

  /* Now make the list for the response */
    for (i = 0; i < numRetNames; ++i, ++ptr)
      *ptr = vl[startIndex+i]->name;
  
    return (numRetNames);
    }
  return (0);
  }

/************************************************************************/
/************************************************************************/
/*                       mvlu_clone_objname				*/
/************************************************************************/
/* This function 'clones' a MMS OBJECT_NAME structure, which means that	*/
/* it must allocate the various storage elements as required. Note	*/
/* that the name storage allocation is 65, to give calling routines	*/
/* room to work.							*/ 

static ST_VOID mvlu_clone_objname (OBJECT_NAME *dest, OBJECT_NAME *src, ST_CHAR *oname, ST_CHAR *dname)
  {
  memset (dest, 0, sizeof (OBJECT_NAME));
  dest->object_tag = src->object_tag;
  dest->obj_name.vmd_spec = oname;	/* set pointer	*/
  strcpy (dest->obj_name.vmd_spec, src->obj_name.vmd_spec);
  if (src->object_tag == DOM_SPEC)
    {
    dest->domain_id = dname;		/* set pointer	*/
    strcpy (dest->domain_id, src->domain_id);
    }
  }

/************************************************************************/
/************************************************************************/
/* MVLU READ/WRITE HANDLERS						*/
/************************************************************************/

static ST_VOID mvluDefAsyncWrIndFun (struct mvlu_wr_va_ctrl *mvluWrVaCtrl);
static ST_VOID mvluDefAsyncRdIndFun (struct mvlu_rd_va_ctrl *mvluRdVaCtrl); 

/* Function pointers for non-UCA variable handling */
ST_VOID(*mvluAsyncRdIndFun)(struct mvlu_rd_va_ctrl *mvluRdVaCtrl) = 
				mvluDefAsyncRdIndFun;
ST_VOID(*mvluAsyncWrIndFun)(struct mvlu_wr_va_ctrl *mvluWrVaCtrl) =
				mvluDefAsyncWrIndFun;

/************************************************************************/
/************************************************************************/
/************************************************************************/
/*			mvl_read_ind					*/
/************************************************************************/

ST_VOID u_mvl_read_ind (MVL_IND_PEND *indCtrl)
  {
MVLAS_READ_CTRL *rdCtrl;
MVLAS_RD_VA_CTRL *rdVaCtrl;
MVL_VAR_ASSOC *va;
MVLU_RD_VA_CTRL *mvluRdVaCtrl;
ST_INT i;
ST_INT numVar;
RUNTIME_TYPE *rt;
ST_INT numRt;
PRIM_INFO prim_info;

  rdCtrl = &indCtrl->u.rd;
  numVar = rdCtrl->numVar;

/* First we will go through each variable being read and count the	*/
/* primitive elelemts.							*/

  rdVaCtrl = rdCtrl->vaCtrlTbl;
  for (i = 0; i < numVar; ++i, ++rdVaCtrl)
    {
    va = rdVaCtrl->va;
    if (va)	  /* VA was resolved, we can deal with it		*/
      {
      rt    = va->type_ctrl->rt;
      numRt = va->type_ctrl->num_rt;

      rdVaCtrl->acc_rslt_tag = ACC_RSLT_SUCCESS;
      rdVaCtrl->numPrimDataDone = 0;
      if (va->base_va != NULL)	/* UCA variable handling ... */
        {
        rdVaCtrl->numPrimData = countPrimEl (va, rt, numRt); 
        }
      else
        rdVaCtrl->numPrimData = 1;
      }
    else
      rdVaCtrl->numPrimData = 1;
    }

/* Now we will go through each var being read and invoke the rdInd	*/
/* function for it.							*/

  rdVaCtrl = rdCtrl->vaCtrlTbl;
  for (i = 0; i < numVar; ++i, ++rdVaCtrl)
    {
    va = rdVaCtrl->va;
    if (va)	  /* VA was resolved, we can deal with it		*/
      {

    /* The VA's data pointer is valid, as is the type ID. 		*/
    /* We want to call the handlers for all primitive level functions 	*/
    /* for this data type						*/

      if (va->base_va != NULL)	/* UCA variable handling ... */
        {
        rt    = va->type_ctrl->rt;
        numRt = va->type_ctrl->num_rt;

	/* Initialize prim_info struct.	*/
        prim_info.prim_num         = va->prim_num;
        prim_info.prim_offset      = 0;
        prim_info.prim_offset_base = va->offset_from_base;

	startElReads (indCtrl, rdCtrl, rdVaCtrl, rt, numRt, &prim_info); 
        }
      else		/* Non-UCA variable handling ... */
        {
        mvluRdVaCtrl = (MVLU_RD_VA_CTRL *) M_CALLOC (MSMEM_MVLU_VA_CTRL, 1, 
					sizeof (MVLU_RD_VA_CTRL));
        mvluRdVaCtrl->primData = (ST_CHAR *) va->data;
        mvluRdVaCtrl->indCtrl = indCtrl;
        mvluRdVaCtrl->rdVaCtrl = rdVaCtrl;
        (*mvluAsyncRdIndFun)(mvluRdVaCtrl);
        }
      }
    else		/* VA not found, let it be done */
      {
      mvluRdVaCtrl = (MVLU_RD_VA_CTRL *) M_CALLOC (MSMEM_MVLU_VA_CTRL, 1, 
					sizeof (MVLU_RD_VA_CTRL));
      mvluRdVaCtrl->indCtrl = indCtrl;
      mvluRdVaCtrl->rdVaCtrl = rdVaCtrl;
      mvlu_rd_prim_done (mvluRdVaCtrl, SD_SUCCESS);
      }
    }
  }

/************************************************************************/
/*			mvlu_trim_branch_name				*/
/* Find last '$' in name and replace it with NULL.			*/
/************************************************************************/

ST_VOID mvlu_trim_branch_name (ST_CHAR *branch_name)
  {
ST_CHAR *ptr;
  if ((ptr = strrchr (branch_name, '$')) != NULL)	/* find last '$'*/
    *ptr = 0;	/* replace '$' with NULL	*/
  return;
  }

/************************************************************************/
/*			startElReads					*/
/************************************************************************/

static ST_VOID startElReads (MVL_IND_PEND *indCtrl, MVLAS_READ_CTRL *rdCtrl,
		     MVLAS_RD_VA_CTRL *rdVaCtrl, 
		     RUNTIME_TYPE *rt, ST_INT rt_num, PRIM_INFO *prim_info)
  {
ST_INT i;
MVLU_RD_VA_CTRL *mvluRdVaCtrl;
ST_RTINT rdIndFunIndex;
ST_UCHAR el_tag;
ST_INT el_size;
ST_RTINT num_rt_blks;
#if defined(MVL_XNAME)
ST_CHAR element_name[MAX_IDENT_LEN+1];  
ST_CHAR branch_name[MAX_IDENT_LEN+1];  
MVL_VAR_ASSOC *va;
ST_CHAR *comp_name;
#endif

#if defined(MVL_XNAME)
  va = rdVaCtrl->va;
  element_name[0]=0;
  strcpy (branch_name,va->name);

/* if this is a substructure we need to trim back one level of the name */
/* because the first rt element will be the name of the component 	*/
/* already part of the va->name						*/

  if ((rt_num > 1) && (strstr (branch_name,"$")))
    {
    mvlu_trim_branch_name (branch_name);
    }
#endif

  for (i = 0; i < rt_num; ++i, ++rt)
    {

#if defined(MVL_XNAME)
    if (rt_num > 1)
      {
      comp_name = ms_comp_name_find (rt);
      if (strlen (comp_name))
        {
        if ((rt->el_tag == RT_STR_START))
          {
          strcat (branch_name, "$");
          strcat (branch_name, comp_name);
          strcpy (element_name, branch_name);
          }
        else
          {
	  element_name[0]=0;
	  strcat (element_name, branch_name);
	  strcat (element_name, "$");
	  strcat (element_name, comp_name);
	  }
        }

      if (rt->el_tag == RT_STR_END)
        {
        strcpy (element_name, branch_name);
        mvlu_trim_branch_name (branch_name);
        }
      }
    else
      {   /* only one element in the RT table assume it's a primitive	*/
      strcpy (element_name, branch_name);
      }
#endif

    el_tag = rt->el_tag;
    num_rt_blks = rt->u.arr.num_rt_blks;
    el_size = rt->el_size;

    if (ms_is_rt_prim (rt) == SD_TRUE)
      {
      rdIndFunIndex = rt->mvluTypeInfo.rdIndFunIndex;
      if (u_mvlu_leaf_rd_ind_fun != NULL || 
          (rdIndFunIndex >= 0 && rdIndFunIndex < mvluNumRdFunEntries))
        {
        mvluRdVaCtrl = (MVLU_RD_VA_CTRL *) M_CALLOC (MSMEM_MVLU_VA_CTRL, 1, 
					     sizeof (MVLU_RD_VA_CTRL));
        mvluRdVaCtrl->rt = rt;
#if defined(MVL_XNAME)
    	strcpy (mvluRdVaCtrl->xName, element_name);
#endif
        mvluRdVaCtrl->primData = (ST_CHAR *) rdVaCtrl->va->data + 
      		   			     prim_info->prim_offset;
        mvluRdVaCtrl->indCtrl = indCtrl;
        mvluRdVaCtrl->rdVaCtrl = rdVaCtrl;
#if defined(MVLU_USE_REF)
        mvluRdVaCtrl->primRef = rt->mvluTypeInfo.ref;
#endif
        mvluRdVaCtrl->prim_num = prim_info->prim_num;
        mvluRdVaCtrl->prim_offset_base = prim_info->prim_offset_base;
        if (u_mvlu_leaf_rd_ind_fun == NULL)
          (*mvluRdFunInfoTbl[rdIndFunIndex].fun_ptr)(mvluRdVaCtrl);
	else
          (*u_mvlu_leaf_rd_ind_fun)(mvluRdVaCtrl);
        }
      else
        rdVaCtrl->acc_rslt_tag = ACC_RSLT_FAILURE;
      prim_info->prim_num++;
      }

    if (el_tag == RT_ARR_START)
      {
      startArrRds (indCtrl, rdCtrl, rdVaCtrl, rt, prim_info);
      i += (num_rt_blks + 1);
      rt += (num_rt_blks + 1);
      }
    else
      {
      prim_info->prim_offset      += el_size;
      prim_info->prim_offset_base += el_size;
      }
    }
  }

/************************************************************************/
/*  				startArrRds				*/
/************************************************************************/

static ST_VOID startArrRds (MVL_IND_PEND *indCtrl, MVLAS_READ_CTRL *rdCtrl,
		     MVLAS_RD_VA_CTRL *rdVaCtrl, RUNTIME_TYPE *rt, PRIM_INFO *prim_info)
  {
ST_RTINT i;
MVL_VAR_ASSOC *va;
ST_INT numRt;
/* DEBUG: To allow bigger arrays, must chg type of these two vars	*/
/*        & va->arrCtrl.low_index & va->arrCtrl.curr_index.		*/
ST_RTINT low_index;
ST_RTINT num_elmnts;

  numRt = rt->u.arr.num_rt_blks+2;
  va = rdVaCtrl->va;

  if (va->arrCtrl.arrAltAccPres == SD_TRUE)
    {
    low_index = va->arrCtrl.low_index;
    num_elmnts = va->arrCtrl.num_elmnts;
    }
  else
    {
    low_index = 0;
    num_elmnts = (ST_RTINT) rt->u.arr.num_elmnts;
    }

/* Let's check to see if the client is selecting a sub-object ... 	*/
  prim_info->prim_offset      += rt->el_size;
  prim_info->prim_offset_base += rt->el_size;
  for (i = 0; i < num_elmnts; ++i)
    {
    va->arrCtrl.curr_index = low_index + i;
    startElReads (indCtrl, rdCtrl, rdVaCtrl, rt+1, numRt-2, prim_info);
    }
  prim_info->prim_offset      += (rt+numRt-1)->el_size;
  prim_info->prim_offset_base += (rt+numRt-1)->el_size;
  }

/************************************************************************/
/*			mvlu_rd_prim_done				*/
/************************************************************************/
/* The user calls this function acynchronously when the primitive data	*/
/* has been put in the 'primData' buffer.				*/

ST_VOID mvlu_rd_prim_done (MVLU_RD_VA_CTRL *mvluRdVaCtrl, ST_RET rc)
  {
MVL_IND_PEND *indCtrl;
MVLAS_READ_CTRL *rdCtrl;
MVLAS_RD_VA_CTRL *rdVaCtrl;
ST_INT i;

  rdVaCtrl = mvluRdVaCtrl->rdVaCtrl;

  if (rc != SD_SUCCESS)
    {
/* DEBUG LIZ: added logging why read failed */
    MVL_LOG_NERR2 ("Read failed for UCA variable '%s' rc=%d",
      rdVaCtrl->va->name, rc);
    rdVaCtrl->acc_rslt_tag = ACC_RSLT_FAILURE;
    }
    
  ++rdVaCtrl->numPrimDataDone;
  if (rdVaCtrl->numPrimDataDone == rdVaCtrl->numPrimData)
    {
    /* All primitives for "one" variable are complete.		*/
    indCtrl = mvluRdVaCtrl->indCtrl;

    if (indCtrl->scan_va_done_fun)
      (*indCtrl->scan_va_done_fun)(indCtrl, rdVaCtrl->va);

  /* If all primitives for all variables are complete, respond now */
    rdCtrl = &indCtrl->u.rd;
    rdVaCtrl = rdCtrl->vaCtrlTbl;
    for (i = 0; i < rdCtrl->numVar; ++i, ++rdVaCtrl)
      {
      if (rdVaCtrl->numPrimDataDone != rdVaCtrl->numPrimData)
        break;
      }
    if (i == rdCtrl->numVar)
      {
      if (indCtrl->op != MMSOP_READ)
        {	/* Not normal read. Do special processing.	*/
        if (indCtrl->usr_resp_fun)
          {	/* If user set custom resp fun, call it.	*/
          (*indCtrl->usr_resp_fun) (indCtrl);
          }
        }
      else
        mvlas_read_resp (indCtrl);
      }
    }
  M_FREE (MSMEM_MVLU_VA_CTRL, mvluRdVaCtrl);
  }

/************************************************************************/
/*			mvluDefAsyncRdIndFun				*/
/************************************************************************/

static ST_VOID mvluDefAsyncRdIndFun (struct mvlu_rd_va_ctrl *mvluRdVaCtrl)
  {
  mvlu_rd_prim_done (mvluRdVaCtrl, SD_SUCCESS);
  }

/************************************************************************/
/************************************************************************/
/*			u_mvl_write_ind					*/
/* NOTE: "indCtrl" is freed when "mvlas_write_resp" is called to send	*/
/* response. To assure "indCtrl" is valid until the end of this function,*/
/* the "ind_funct_done" flag is used to indicate if this function	*/
/* is done. It is accessible in all lower level functions.		*/
/* At end of this function, if response ready, "mvlas_write_resp" is	*/
/* called & indCtrl is freed (all done). If response NOT ready, this	*/
/* flag is set to (SD_TRUE), then "mvlu_wr_prim_done" will call		*/
/* "mvlas_write_resp"  later when response is ready.			*/
/************************************************************************/

ST_VOID u_mvl_write_ind (MVL_IND_PEND *indCtrl)
  {
MVLAS_WRITE_CTRL *wrCtrl;
MVLAS_WR_VA_CTRL *wrVaCtrl;
MVL_VAR_ASSOC *va;
MVLU_WR_VA_CTRL *mvluWrVaCtrl;
ST_INT i;
ST_INT numVar;
RUNTIME_TYPE *rt;
ST_INT numRt;
PRIM_INFO prim_info;

  /* NOTE: (indCtrl->u.wr.ind_funct_done==SD_FALSE) because indCtrl calloced.*/
  /*       Changed to SD_TRUE only when this function completes.	*/
  /*       Checked in "mvlu_wr_prim_done" funct.			*/

  wrCtrl = &indCtrl->u.wr;
  numVar = wrCtrl->numVar;

/* First we will go through each variable being written and count the	*/
/* primitive elelemts.							*/
  wrVaCtrl = wrCtrl->vaCtrlTbl;

  for (i = 0; i < numVar; ++i, ++wrVaCtrl)
    {
    va = wrVaCtrl->va;
    if (va)		/* VA was resolved, we can look it over		*/
      {
      rt    = va->type_ctrl->rt;
      numRt = va->type_ctrl->num_rt;
      wrVaCtrl->resp_tag = WR_RSLT_SUCCESS;
      wrVaCtrl->numPrimDataDone = 0;
      if (va->base_va != NULL)	/* UCA variable handling ... */
        wrVaCtrl->numPrimData = countPrimEl (va, rt, numRt);
      else
        wrVaCtrl->numPrimData = 1;
      }
    else
      wrVaCtrl->numPrimData = 1;
    }

/* Now we will go through each var being written and invoke the wrInd	*/
/* function for it.							*/

  wrVaCtrl = wrCtrl->vaCtrlTbl;
  for (i = 0; i < numVar; ++i, ++wrVaCtrl)
    {
    va = wrVaCtrl->va;
    if (va)		/* VA was resolved, we can look it over		*/
      {
    /* The VA's data pointer is valid, as is the type ID. 		*/
    /* We want to call the handlers for all primitive level functions 	*/
    /* for this data type						*/
      if (va->base_va != NULL)	/* UCA variable handling ... */
        {
        rt    = va->type_ctrl->rt;
        numRt = va->type_ctrl->num_rt;

        if (chk_var_writable (rt, numRt))
          {	/* This var NOT writable. Set error & skip to next var.	*/
          set_write_resp_dae (wrVaCtrl, ARE_OBJ_ACCESS_DENIED);
          continue;	/* skip to next variable	*/
          }

        /* Initialize prim_info struct.	*/
        prim_info.prim_num         = va->prim_num;
        prim_info.prim_offset      = 0;
        prim_info.prim_offset_base = va->offset_from_base;

        /* If this is struct (or array), user function pointer !=NULL,	*/
        /* and function returns error, do not allow write.		*/
        if (numRt > 1		/* must be struct or array	*/
            && u_mvl_wr_ind_var_start != NULL
            && (*u_mvl_wr_ind_var_start)(indCtrl, wrVaCtrl) != SD_SUCCESS)
          {	/* This var NOT writable. Set error & skip to next var.	*/
          set_write_resp_dae (wrVaCtrl, ARE_OBJ_ACCESS_DENIED);
          continue;	/* skip to next variable	*/
          }

        startElWrites (indCtrl, wrCtrl,wrVaCtrl, rt, numRt, &prim_info); 

        /* If this is struct (or array), user function pointer !=NULL,	*/
        /* and function returns error, do not allow write.		*/
        /* CAUTION: If error is detected here, must make sure that leaf	*/
        /*          functions did NOT write any data.			*/
        if (numRt > 1		/* must be struct or array	*/
            && u_mvl_wr_ind_var_end != NULL
            && (*u_mvl_wr_ind_var_end)(indCtrl, wrVaCtrl) != SD_SUCCESS)
          {	/* This var NOT writable. Set error & skip to next var.	*/
          set_write_resp_dae (wrVaCtrl, ARE_OBJ_ACCESS_DENIED);
          continue;	/* skip to next variable	*/
          }
        }
      else		/* Non-UCA variable handling ... */
        {
        mvluWrVaCtrl = (MVLU_WR_VA_CTRL *) M_CALLOC (MSMEM_MVLU_VA_CTRL, 1, 
					sizeof (MVLU_WR_VA_CTRL));
        mvluWrVaCtrl->primData = (ST_CHAR *) va->data;
        mvluWrVaCtrl->indCtrl = indCtrl;
        mvluWrVaCtrl->wrVaCtrl = wrVaCtrl;
        (*mvluAsyncWrIndFun)(mvluWrVaCtrl);
        }
      }
    else 	
      { 
      mvluWrVaCtrl = (MVLU_WR_VA_CTRL *) M_CALLOC (MSMEM_MVLU_VA_CTRL, 1, 
					sizeof (MVLU_WR_VA_CTRL));
      mvluWrVaCtrl->indCtrl = indCtrl;
      mvluWrVaCtrl->wrVaCtrl = wrVaCtrl;
      mvlu_wr_prim_done (mvluWrVaCtrl, SD_SUCCESS);
      }
    }
  /* If response is ready (i.e. all synchronous leafs), send it now.	*/
  /* If not, just set flag (SD_TRUE) & mvlu_wr_prim_done will send resp.*/
  if (chk_write_resp_ready (indCtrl) == SD_SUCCESS)
    mvlas_write_resp (indCtrl);		/* sends resp & frees indCtrl	*/
  else
    indCtrl->u.wr.ind_funct_done = SD_TRUE; /* this funct is done	*/  	
  }

/************************************************************************/
/*			startElWrites					*/
/* NOTE: Resp NOT sent while in this function (see ind_funct_done flag).*/
/*       Therefore, indCtrl is valid throughout this function		*/
/*       (i.e. it is NOT freed).					*/
/************************************************************************/

static ST_VOID startElWrites (MVL_IND_PEND *indCtrl, MVLAS_WRITE_CTRL *wrCtrl,
	  	      MVLAS_WR_VA_CTRL *wrVaCtrl, 
		      RUNTIME_TYPE *rt, ST_INT rt_num, PRIM_INFO *prim_info)
  {
ST_INT i;
MVLU_WR_VA_CTRL *mvluWrVaCtrl;
ST_RTINT wrIndFunIndex;
ST_UCHAR el_tag;
ST_INT el_size;
ST_RTINT num_rt_blks;
#if defined(MVL_XNAME)
ST_CHAR element_name[MAX_IDENT_LEN+1];  
ST_CHAR branch_name[MAX_IDENT_LEN+1];  
MVL_VAR_ASSOC *va;
ST_CHAR *comp_name;
#endif
ST_CHAR sboName[MVL61850_MAX_OBJREF_LEN+1]="";	/* init to empty string	*/

#if !defined(MVL61850_CTL_DISABLE)
  ST_INT oper_nest_level = 0;
#endif

#if defined(MVL_XNAME)
  va = wrVaCtrl->va;
  element_name[0]=0;
  strcpy (branch_name,va->name);

/* if this is a substructure we need to trim back one level of the name */
/* because the first rt element will be the name of the component 	*/
/* already part of the va->name		    				*/

  if ((rt_num > 1) && (strstr (branch_name, "$")))
    {
    mvlu_trim_branch_name (branch_name);
    }
#endif

  /* Do some special stuff for IEC 61850 SBO controls.		*/
  /* If this var is a struct named "Oper" or "Cancel", check SBO state.	*/
  /* NOTE: must be writing "Oper" or "Cancel" struct and ONLY the struct.*/
  if (rt->el_tag == RT_STR_START  &&  /* it is a struct		*/
      (strcmp (ms_comp_name_find(rt), "Oper") == 0  ||  strcmp (ms_comp_name_find(rt), "Cancel") == 0))
    {
    /* Check "Select" timeouts for ALL SBO controls.	*/
    mvlu_sbo_chk_timers ();
    /* Chk SBO state. Save in wrVaCtrl->sboCtrl (NULL if "Select" not done).*/
    mvl61850_sbo_create_sboname (wrVaCtrl->va, &wrVaCtrl->va_scope, sboName);
    wrVaCtrl->sboCtrl = mvlu_sbo_chk_state (sboName, indCtrl->event->net_info);
    }

  for (i = 0; i < rt_num; ++i, ++rt)
    {
#if defined(MVL_XNAME)
    if (rt_num > 1)  /* is this a collection of elements? */
      {
      comp_name = ms_comp_name_find (rt);
      if (strlen (comp_name))
        {
        if ((rt->el_tag == RT_STR_START))
          {
          strcat (branch_name, "$");
          strcat (branch_name, comp_name);
          strcpy (element_name, branch_name);
          }
        else
          {
	  element_name[0]=0;
	  strcat (element_name, branch_name);
	  strcat (element_name, "$");
	  strcat (element_name, comp_name);
	  }
        }

      if (rt->el_tag == RT_STR_END)
        {
        strcpy (element_name, branch_name);
        mvlu_trim_branch_name (branch_name);
        }
      }
    else
      {   /* only one element in the RT table assume it's a primitive */
      strcpy (element_name, branch_name);
      }
#endif

    el_tag = rt->el_tag;
    num_rt_blks = rt->u.arr.num_rt_blks;
    el_size = rt->el_size;

#if !defined(MVL61850_CTL_DISABLE)
    if (rt->el_tag == RT_STR_START)
      {
      /* only increment this if this is "Oper" or already inside "Oper".	*/
      if (oper_nest_level>0)
        oper_nest_level++;
      else if (strcmp (ms_comp_name_find(rt), "Oper") == 0)
        {
        if (wrVaCtrl->sboCtrl)
          wrVaCtrl->sboCtrl->ctlState = MVL61850_CTLSTATE_READY;
        u_mvl61850_ctl_oper_begin (sboName);
        oper_nest_level++;
        }
      }
    if (rt->el_tag == RT_STR_END)
      {
      if (oper_nest_level>0)
        {
        if (--oper_nest_level==0)
          {
          /* All leaf functs have been called for "Oper" struct.*/
          u_mvl61850_ctl_oper_end (indCtrl->event->net_info, sboName, wrVaCtrl->va->base_va);
          }
        }
      }
#endif	/* !defined(MVL61850_CTL_DISABLE)	*/

    if (ms_is_rt_prim (rt) == SD_TRUE)
      {
      wrIndFunIndex = rt->mvluTypeInfo.wrIndFunIndex;
      if (u_mvlu_leaf_wr_ind_fun != NULL || 
          (wrIndFunIndex >= 0 && wrIndFunIndex < mvluNumWrFunEntries))
        {
        mvluWrVaCtrl = (MVLU_WR_VA_CTRL *) M_CALLOC (MSMEM_MVLU_VA_CTRL, 1,
                                             sizeof (MVLU_WR_VA_CTRL));
#if defined(MVL_XNAME)
    	strcpy (mvluWrVaCtrl->xName, element_name);
#endif
        mvluWrVaCtrl->primData = (ST_CHAR *) wrVaCtrl->va->data + 
      		   			     prim_info->prim_offset;
        mvluWrVaCtrl->indCtrl = indCtrl;
        mvluWrVaCtrl->wrVaCtrl = wrVaCtrl;
        mvluWrVaCtrl->rt = rt;
#if defined(MVLU_USE_REF)
        mvluWrVaCtrl->primRef = rt->mvluTypeInfo.ref;
#endif
        mvluWrVaCtrl->prim_num = prim_info->prim_num;
        mvluWrVaCtrl->prim_offset_base = prim_info->prim_offset_base;
        if (u_mvlu_leaf_wr_ind_fun == NULL)
          (*mvluWrFunInfoTbl[wrIndFunIndex].fun_ptr)(mvluWrVaCtrl);
	else
          (*u_mvlu_leaf_wr_ind_fun)(mvluWrVaCtrl);
        }
      else
        wrVaCtrl->resp_tag = WR_RSLT_FAILURE;
      prim_info->prim_num++;
      }

    if (el_tag == RT_ARR_START)
      {
      startArrWrs (indCtrl, wrCtrl, wrVaCtrl, rt, prim_info);
      i += (num_rt_blks + 1);
      rt += (num_rt_blks + 1);
      }
    else
      {
      prim_info->prim_offset      += el_size;
      prim_info->prim_offset_base += el_size;
      }
    }
  }

/************************************************************************/
/* 			startArrWrs					*/
/************************************************************************/

static ST_VOID startArrWrs (MVL_IND_PEND *indCtrl, MVLAS_WRITE_CTRL *wrCtrl,
		     MVLAS_WR_VA_CTRL *wrVaCtrl, RUNTIME_TYPE *rt, PRIM_INFO *prim_info)
  {
ST_RTINT i;
MVL_VAR_ASSOC *va;
ST_INT numRt;
/* DEBUG: To allow bigger arrays, must chg type of these two vars	*/
/*        & va->arrCtrl.low_index & va->arrCtrl.curr_index.		*/
ST_RTINT low_index;
ST_RTINT num_elmnts;

  numRt = rt->u.arr.num_rt_blks+2;
  va = wrVaCtrl->va;

  if (va->arrCtrl.arrAltAccPres == SD_TRUE)
    {
    low_index = va->arrCtrl.low_index;
    num_elmnts = va->arrCtrl.num_elmnts;
    }
  else
    {
    low_index = 0;
    num_elmnts = (ST_RTINT) rt->u.arr.num_elmnts;
    }

/* Let's check to see if the client is selecting a sub-object ... 	*/
  prim_info->prim_offset      += rt->el_size;
  prim_info->prim_offset_base += rt->el_size;
  for (i = 0; i < num_elmnts; ++i)
    {
    va->arrCtrl.curr_index = low_index + i;
    startElWrites (indCtrl, wrCtrl, wrVaCtrl, rt+1, numRt-2, prim_info);
    }
  prim_info->prim_offset      += (rt+numRt-1)->el_size;
  prim_info->prim_offset_base += (rt+numRt-1)->el_size;
  }

/************************************************************************/
/*			 mvlu_wr_prim_done				*/
/************************************************************************/
/* The user calls this function acynchronously when the primitive data	*/
/* has been put in the 'primData' buffer.				*/

ST_VOID mvlu_wr_prim_done (MVLU_WR_VA_CTRL *mvluWrVaCtrl, ST_RET rc)
  {
MVLAS_WR_VA_CTRL *wrVaCtrl;

  wrVaCtrl = mvluWrVaCtrl->wrVaCtrl;
  if (rc != SD_SUCCESS)
    wrVaCtrl->resp_tag = WR_RSLT_FAILURE;

  ++wrVaCtrl->numPrimDataDone;
  if (wrVaCtrl->numPrimDataDone == wrVaCtrl->numPrimData)
    {	/* all prims done for this variable	*/
    MVL_IND_PEND *indCtrl    = mvluWrVaCtrl->indCtrl;

#if !defined(MVL61850_CTL_DISABLE)
    mvl61850_ctl_req_done (wrVaCtrl, indCtrl->event->net_info);
#endif	/* !defined(MVL61850_CTL_DISABLE)	*/

    /* Response "usually" sent from u_mvl_write_ind. If it finished	*/
    /* without sending resp AND resp is ready now, send it.		*/
    if (indCtrl->u.wr.ind_funct_done		/* ind funct completed	*/
        && chk_write_resp_ready(indCtrl)==SD_SUCCESS)	/* resp ready	*/
      mvlas_write_resp (indCtrl);		/* send resp		*/
    }
  M_FREE (MSMEM_MVLU_VA_CTRL, mvluWrVaCtrl);
  }


/************************************************************************/
/*			mvluDefAsyncWrIndFun				*/
/************************************************************************/

static ST_VOID mvluDefAsyncWrIndFun (struct mvlu_wr_va_ctrl *mvluWrVaCtrl)
  {
  mvlu_wr_prim_done (mvluWrVaCtrl, SD_SUCCESS);
  }

/************************************************************************/
/************************************************************************/
/*			countPrimEl					*/
/************************************************************************/

static ST_INT countPrimEl (MVL_VAR_ASSOC *va, RUNTIME_TYPE *rt, ST_INT rt_num)
  {
ST_INT i;
ST_INT num_elmnts;
ST_INT numPrimData;
ST_INT subElCount;

  numPrimData = 0;
  for (i = 0; i < rt_num; ++i, ++rt)
    {
    if (ms_is_rt_prim (rt) == SD_TRUE)
      ++numPrimData;
    if (rt->el_tag == RT_ARR_START)
      {
      subElCount = countPrimEl (va, rt+1, rt->u.arr.num_rt_blks);

      if (va->arrCtrl.arrAltAccPres == SD_FALSE)
        num_elmnts = rt->u.arr.num_elmnts;
      else
        num_elmnts = va->arrCtrl.num_elmnts;

      numPrimData += (subElCount * num_elmnts);

      i += (rt->u.arr.num_rt_blks + 1);
      rt += (rt->u.arr.num_rt_blks + 1);
      }
    }
  return (numPrimData);
  }

/************************************************************************/
/*			mvluDefGetVaDataBufFun				*/
/* NOTE: only called when va->base_va->use_static_data==SD_FALSE.	*/
/************************************************************************/

static ST_VOID mvluDefGetVaDataBufFun (ST_INT service, 
				       MVL_VAR_ASSOC *va, ST_INT size)
  {
  if (service == MMSOP_READ || service == MMSOP_WRITE || 
      service == MMSOP_INFO_RPT || service == MMSOP_RD_USR_HANDLED)
    {
    va->data = M_MALLOC (MSMEM_MVLU_VA_DATA, size);
    }
  }

/************************************************************************/
/*			mvluDefFreeVaDataBufFun				*/
/* NOTE: only called when va->base_va->use_static_data==SD_FALSE.	*/
/************************************************************************/

static ST_VOID mvluDefFreeVaDataBufFun (ST_INT service, MVL_VAR_ASSOC *va)
  {
  if (service == MMSOP_READ || service == MMSOP_WRITE ||
      service == MMSOP_INFO_RPT || service == MMSOP_RD_USR_HANDLED)
    {
    M_FREE (MSMEM_MVLU_VA_DATA, va->data);
    }
  }

/************************************************************************/
/*			mvlu_find_rt_leaf				*/
/************************************************************************/

RUNTIME_TYPE *mvlu_find_rt_leaf (ST_INT type_id, ST_CHAR *leafName)
  {
RUNTIME_TYPE *rt;
ST_INT numRt; 
ST_RET ret;

  ret = mvl_get_runtime (type_id, &rt, &numRt);
  if (ret != SD_SUCCESS)
    {
    MVL_LOG_NERR1 ("Could not get RT type for type id %d", type_id);
    return (NULL);
    }

  ret = mvlu_find_uca_var (&rt, &numRt, leafName);
  if (ret != SD_SUCCESS)
    {
    MVL_LOG_NERR1 ("Could not find leaf '%s'", leafName);
    return (NULL);
    }

  if (numRt != 1)
    {	/* See if this is an array with a single primitive element */
    if (rt->el_tag == RT_ARR_START && numRt == 3) 
      return (rt+1);

    MVL_LOG_NERR1 ("'%s' is a branch, not a leaf", leafName);
    return (NULL);
    }


  return (rt);
  }

/************************************************************************/
/*			mvlu_find_comp_type				*/
/* Arguments:								*/
/*  base_type_id	type ID for top level variable			*/
/*  flatname		flattened component name with top level var	*/
/*			name (Logical Node) stripped off.		*/
/*  sub_rt_type		Ptr to (RUNTIME_TYPE *) to be filled in.	*/
/*  sub_rt_num		Ptr to "count" to be filled in.			*/
/************************************************************************/
ST_RET mvlu_find_comp_type (ST_INT base_type_id, ST_CHAR *flatname,
			RUNTIME_TYPE **sub_rt_type,		/* out	*/
			ST_INT *sub_rt_num)			/* out	*/
  {
MVL_TYPE_CTRL *base_type_ctrl;
ST_RET ret;

  base_type_ctrl = mvl_type_ctrl_find (base_type_id);
  if (base_type_ctrl)
    {
    *sub_rt_type = base_type_ctrl->rt;
    *sub_rt_num = base_type_ctrl->num_rt;
    ret = mvlu_find_uca_var (sub_rt_type, sub_rt_num, flatname);
    }
  else
    ret = SD_FAILURE;

  /* NOTE: don't log error here. Caller may often pass invalid names.	*/
  return (ret);
  }

/************************************************************************/
/*			mvlu_get_leaf_val_int8			*/
/* Get leaf data value for type ST_INT8.				*/ 
/* Arguments:								*/
/*   base_va	base variable (i.e. logical node)			*/
/*   flatname	flattened leaf name (e.g. ST$Mod$stVal)			*/
/*   data	ptr to data written by this function			*/
/* RETURNS:	SD_SUCCESS or SD_FAILURE				*/
/************************************************************************/
ST_RET mvlu_get_leaf_val_int8 (MVL_VAR_ASSOC *base_va, ST_CHAR *flatname, ST_INT8 *data)
  {
RUNTIME_TYPE *comp_rt_type;	/* first rt_type of component	*/
ST_INT comp_rt_num;		/* num of rt_types in component	*/
ST_RET ret = SD_FAILURE;

  /* Find the attribute type.	*/
  if (mvlu_find_comp_type (base_va->type_id,
           flatname,	/* flattened name	*/
	   &comp_rt_type,	/* ptr to val set by function	*/
	   &comp_rt_num)	/* ptr to val set by function	*/
      == SD_SUCCESS)
    {
    if (comp_rt_type->el_tag == RT_INTEGER && comp_rt_type->u.p.el_len == 1)
      {	/* must be INT8	*/
      *data = *(ST_INT8 *)((ST_CHAR *) base_va->data + comp_rt_type->mvluTypeInfo.offSet);
      ret = SD_SUCCESS;
      }
    else
      MVL_LOG_ERR2 ("Invalid type for '%s' attribute in LN '%s'. Cannot use it.", flatname, base_va->name);
    }
  else
    MVLU_LOG_FLOW2 ("Cannot find '%s' attribute in this variable '%s'",
                  flatname, base_va->name);
  return (ret);
  }
/************************************************************************/
/*			mvlu_get_leaf_val_int32			*/
/* Get leaf data value for type ST_INT32.				*/ 
/* Arguments:								*/
/*   base_va	base variable (i.e. logical node)			*/
/*   flatname	flattened leaf name (e.g. ST$Mod$stVal)			*/
/*   data	ptr to data written by this function			*/
/* RETURNS:	SD_SUCCESS or SD_FAILURE				*/
/************************************************************************/
ST_RET mvlu_get_leaf_val_int32 (MVL_VAR_ASSOC *base_va, ST_CHAR *flatname, ST_INT32 *data)
  {
RUNTIME_TYPE *comp_rt_type;	/* first rt_type of component	*/
ST_INT comp_rt_num;		/* num of rt_types in component	*/
ST_RET ret = SD_FAILURE;

  /* Find the attribute type.	*/
  if (mvlu_find_comp_type (base_va->type_id,
           flatname,	/* flattened name	*/
	   &comp_rt_type,	/* ptr to val set by function	*/
	   &comp_rt_num)	/* ptr to val set by function	*/
      == SD_SUCCESS)
    {
    if (comp_rt_type->el_tag == RT_INTEGER && comp_rt_type->u.p.el_len == 4)
      {	/* must be INT32	*/
      *data = *(ST_INT32 *)((ST_CHAR *) base_va->data + comp_rt_type->mvluTypeInfo.offSet);
      ret = SD_SUCCESS;
      }
    else
      MVL_LOG_ERR2 ("Invalid type for '%s' attribute in LN '%s'. Cannot use it.", flatname, base_va->name);
    }
  else
    MVLU_LOG_FLOW2 ("Cannot find '%s' attribute in this variable '%s'",
                  flatname, base_va->name);
  return (ret);
  }
/************************************************************************/
/*			mvlu_get_leaf_val_int_any			*/
/* Get leaf value for any signed integer. Cast value to ST_INT32.	*/
/************************************************************************/
ST_RET mvlu_get_leaf_val_int_any (MVL_VAR_ASSOC *base_va, ST_CHAR *flatname, ST_INT32 *data)
  {  
RUNTIME_TYPE *comp_rt_type;	/* first rt_type of component	*/
ST_INT comp_rt_num;		/* num of rt_types in component	*/
ST_CHAR *raw_data;	/* ptr to data. May be any integer size	*/
ST_INT8 tmp_int8;	/* Used to get val as INT8, then cast to INT32	*/
ST_INT16 tmp_int16;	/* Used to get val as INT16, then cast to INT32	*/
ST_RET retcode = SD_SUCCESS;

  /* Find the attribute type.	*/
  if (mvlu_find_comp_type (base_va->type_id,
          flatname,		/* flattened name	*/
          &comp_rt_type,	/* ptr to val set by function	*/
          &comp_rt_num)		/* ptr to val set by function	*/
      == SD_SUCCESS)
    {
    raw_data = (ST_CHAR *) base_va->data + comp_rt_type->mvluTypeInfo.offSet;
    if (comp_rt_type->el_tag == RT_INTEGER)
      {
      if (comp_rt_type->u.p.el_len == 1)	/* INT8	*/
        {
        tmp_int8 = *(ST_INT8 *) raw_data;
        *data = (ST_INT32) tmp_int8;
        }
      else if (comp_rt_type->u.p.el_len == 2)	/* INT16	*/
        {
        tmp_int16 = *(ST_INT16 *) raw_data;
        *data = (ST_INT32) tmp_int16;
        }
      else if (comp_rt_type->u.p.el_len == 4)	/* INT32	*/
        {
        *data = *(ST_INT32 *) raw_data;
        }
      else
        {  
        MVL_LOG_ERR2 ("Unsupported integer size for '%s' attribute in variable '%s'",
                  flatname, base_va->name);
        retcode = SD_FAILURE;
        }
      }
    else
      {  
      MVL_LOG_ERR2 ("Invalid type for '%s' attribute in variable '%s'. Cannot get value.",
                  flatname, base_va->name);
      retcode = SD_FAILURE;
      }
    }
  else
    {  
    MVLU_LOG_FLOW2 ("Cannot find '%s' attribute in variable '%s'",
                  flatname, base_va->name);
    retcode = SD_FAILURE;
    }
  return (retcode);
  }
/************************************************************************/
/*			mvlu_get_leaf_val_uint32			*/
/* Get leaf data value for type ST_UINT32.				*/ 
/* Arguments:								*/
/*   base_va	base variable (i.e. logical node)			*/
/*   flatname	flattened leaf name (e.g. ST$Mod$stVal)			*/
/*   data	ptr to data written by this function			*/
/* RETURNS:	SD_SUCCESS or SD_FAILURE				*/
/************************************************************************/
ST_RET mvlu_get_leaf_val_uint32 (MVL_VAR_ASSOC *base_va, ST_CHAR *flatname, ST_UINT32 *data)
  {
RUNTIME_TYPE *comp_rt_type;	/* first rt_type of component	*/
ST_INT comp_rt_num;		/* num of rt_types in component	*/
ST_RET ret = SD_FAILURE;

  /* Find the attribute type.	*/
  if (mvlu_find_comp_type (base_va->type_id,
           flatname,		/* flattened name	*/
	   &comp_rt_type,	/* ptr to val set by function	*/
	   &comp_rt_num)	/* ptr to val set by function	*/
      == SD_SUCCESS)
    {
    if (comp_rt_type->el_tag == RT_UNSIGNED && comp_rt_type->u.p.el_len == 4)
      {	/* must be UINT32	*/
      *data = *(ST_UINT32 *)((ST_CHAR *) base_va->data + comp_rt_type->mvluTypeInfo.offSet);
      ret = SD_SUCCESS;
      }
    else
      MVL_LOG_ERR2 ("Invalid type for '%s' attribute in LN '%s'. Cannot use it.", flatname, base_va->name);
    }
  else
    MVLU_LOG_FLOW2 ("Cannot find '%s' attribute in this variable '%s'",
                  flatname, base_va->name);
  return (ret);
  }
/************************************************************************/
/*			mvlu_get_leaf_val_boolean			*/
/* Get leaf data value for type ST_BOOLEAN.				*/ 
/* Arguments:								*/
/*   base_va	base variable (i.e. logical node)			*/
/*   flatname	flattened leaf name (e.g. ST$Mod$stVal)			*/
/*   data	ptr to data written by this function			*/
/* RETURNS:	SD_SUCCESS or SD_FAILURE				*/
/************************************************************************/
ST_RET mvlu_get_leaf_val_boolean (MVL_VAR_ASSOC *base_va, ST_CHAR *flatname, ST_BOOLEAN *data)
  {
RUNTIME_TYPE *comp_rt_type;	/* first rt_type of component	*/
ST_INT comp_rt_num;		/* num of rt_types in component	*/
ST_RET ret = SD_FAILURE;

  /* Find the attribute type.	*/
  if (mvlu_find_comp_type (base_va->type_id,
           flatname,	/* flattened name	*/
	   &comp_rt_type,	/* ptr to val set by function	*/
	   &comp_rt_num)	/* ptr to val set by function	*/
      == SD_SUCCESS)
    {
    if (comp_rt_type->el_tag == RT_BOOL)
      {	/* must be BOOLEAN	*/
      *data = *(ST_BOOLEAN *)((ST_CHAR *) base_va->data + comp_rt_type->mvluTypeInfo.offSet);
      ret = SD_SUCCESS;
      }
    else
      MVL_LOG_ERR2 ("Invalid type for '%s' attribute in LN '%s'. Cannot use it.", flatname, base_va->name);
    }
  else
    MVLU_LOG_FLOW2 ("Cannot find '%s' attribute in this variable '%s'",
                  flatname, base_va->name);
  return (ret);
  }
/************************************************************************/
/*			mvlu_get_leaf_val_bvstring			*/
/* Get leaf data value for type MMS_BVSTRING.				*/ 
/* Arguments:								*/
/*   base_va	base variable (i.e. logical node)			*/
/*   flatname	flattened leaf name (e.g. ST$Mod$stVal)			*/
/*   data	ptr to data written by this function			*/
/* RETURNS:	SD_SUCCESS or SD_FAILURE				*/
/************************************************************************/
ST_RET mvlu_get_leaf_val_bvstring (MVL_VAR_ASSOC *base_va, ST_CHAR *flatname,
                                   MMS_BVSTRING *data, ST_INT max_num_bits)
  {
RUNTIME_TYPE *comp_rt_type;	/* first rt_type of component	*/
ST_INT comp_rt_num;		/* num of rt_types in component	*/
ST_RET ret = SD_FAILURE;
ST_INT num_bytes;

  /* Find the attribute type.	*/
  if (mvlu_find_comp_type (base_va->type_id,
           flatname,	/* flattened name	*/
	   &comp_rt_type,	/* ptr to val set by function	*/
	   &comp_rt_num)	/* ptr to val set by function	*/
      == SD_SUCCESS)
    {
    /* NOTE: for Bvstring, el_len is negative.	*/
    if (comp_rt_type->el_tag == RT_BIT_STRING && comp_rt_type->u.p.el_len < 0
        && abs (comp_rt_type->u.p.el_len) <= max_num_bits)
      {
      num_bytes = 2 + (abs (comp_rt_type->u.p.el_len) + 7)/8;
      memcpy (data, (ST_CHAR *)base_va->data + comp_rt_type->mvluTypeInfo.offSet, num_bytes);
      ret = SD_SUCCESS;
      }
    else
      MVL_LOG_ERR2 ("Invalid type for '%s' attribute in LN '%s'. Cannot use it.", flatname, base_va->name);
    }
  else
    MVLU_LOG_FLOW2 ("Cannot find '%s' attribute in this variable '%s'",
                  flatname, base_va->name);
  return (ret);
  }
/************************************************************************/
/*			mvlu_get_leaf_data_ptr				*/
/* Get leaf data pointer (for any type of leaf).			*/
/* Arguments:								*/
/*   base_va	base variable (i.e. logical node)			*/
/*   flatname	flattened leaf name (e.g. ST$Mod$stVal)			*/
/*   rt_type	ptr to ptr to type (function sets "*rt_type")		*/
/* RETURNS:	pointer to leaf data (NULL on error)			*/
/************************************************************************/
ST_VOID *mvlu_get_leaf_data_ptr (MVL_VAR_ASSOC *base_va, ST_CHAR *flatname, RUNTIME_TYPE **rt_type)
  {
RUNTIME_TYPE *comp_rt_type;	/* first rt_type of component	*/
ST_INT comp_rt_num;		/* num of rt_types in component	*/
ST_VOID *data;

  /* Find the attribute type.	*/
  if (mvlu_find_comp_type (base_va->type_id,
           flatname,	/* flattened name	*/
	   &comp_rt_type,	/* ptr to val set by function	*/
	   &comp_rt_num)	/* ptr to val set by function	*/
      == SD_SUCCESS)
    {
    data = ((ST_CHAR *) base_va->data + comp_rt_type->mvluTypeInfo.offSet);
    *rt_type = comp_rt_type;
    }
  else
    {
    data = NULL;
    MVLU_LOG_FLOW2 ("Cannot find '%s' attribute in this variable '%s'",
                  flatname, base_va->name);
    }
  return (data);
  }

/************************************************************************/
/*                       mvlu_find_component				*/
/* Find a lower level variable by processing "flattened" variable name	*/
/* AND/OR Alternate Access.						*/
/* Last 4 args are for "output" only (i.e. caller passes pointer, this	*/
/* function sets value at that address).				*/
/* NOTE: Similar to "mvlu_find_uca_var" but handles Alternate Access too.*/
/*       Also handles array index in flattened name.			*/
/* NOTE: rt_type->u.arr.num_elmnts <= max ST_RTINT and is never		*/
/*       negative (checked when type created) so cast to ST_UINT is safe.*/
/************************************************************************/
ST_RET mvlu_find_component (
	MVL_VAR_ASSOC *var,	/* base variable (Logical Node)	*/
	ST_CHAR *flatname, 	/* flattened name (e.g. ST$Mod$stVal)	*/
	ALT_ACCESS *alt_acc,	/* NULL if Alt Access Spec not present	*/
	RUNTIME_TYPE **rt_out,	/* if successful, *rt_out is set	*/
	ST_INT *num_rt_out,	/* if successful, *num_rt_out is set	*/
	ST_INT *offset_out,	/* if successful, *offset_out is set	*/
	ST_UINT *prim_num_out)	/* if successful, *prim_num_out is set	*/
  {
ST_RET ret;
ST_INT aa_idx;	/* index to next alternate access element	*/
RUNTIME_TYPE *rt_type;	/* used to set "*rt_out"       before return	*/
ST_INT num_rt;		/* used to set "*num_rt_out"   before return	*/
ST_INT offset;		/* used to set "*offset_out"   before return	*/
ST_UINT prim_num;	/* used to set "*prim_num_out" before return	*/
/* nameBuf should be big enough for leaf name + array index.	*/
ST_CHAR nameBuf[MAX_FLAT_LEN+1];
ST_CHAR *nameToFind;
ST_CHAR *compEnd;
ST_BOOLEAN nameDone;
ST_UINT arr_index;	/* array index if component is array	*/
ST_CHAR *openbracket;	/* ptr to '[' in flatname	*/
ST_CHAR *closebracket;	/* ptr to ']' in flatname	*/

  /* Initialize local variables. These are used to set caller's "*rt_out", etc.*/
  rt_type = var->type_ctrl->rt;
  num_rt  = var->type_ctrl->num_rt;
  offset  = 0;
  prim_num = 0;

  /* Deal with "flattened" naming first (i.e. fake Alternate Access).	*/
  if (flatname != NULL)
    {
    strncpy_safe (nameBuf, flatname, sizeof(nameBuf)-1);	/* copy so we can modify it*/
    nameToFind = nameBuf;
    nameDone = SD_FALSE;
    while (nameDone == SD_FALSE)
      {
      /* Isolate the component name for this level, removing subcomp names	*/
      compEnd = strstr (nameToFind, "$");
      if (compEnd != NULL)
	*compEnd = 0;
      else			/* This is the last nest level		*/
	nameDone = SD_TRUE;
  
      /* If array index present, save it in "arr_index" & remove it from nameToFind.*/
      openbracket = strchr (nameToFind, '[');	/* look for '['	*/
      if (openbracket)
        {
        *openbracket = '\0';			/* replace '[' with NULL*/
        closebracket = strchr (openbracket+1, ']');	/* look for ']'	*/
        if (closebracket)
          {
          *closebracket = '\0';			/* replace ']' with NULL*/
          ret = asciiToUint (openbracket+1, &arr_index);	/* convert array index*/
          }
        else
          ret = SD_FAILURE;	/* found '[' but couldn't find ']'	*/
        if (ret)
          {
          MVL_LOG_ERR1 ("Illegal array index in '%s'", flatname);
          return (ret);
          }
        }

      /* Find the component name in the current runtime type nest level	*/
      /* NOTE: this modifies rt_type, num_rt, offset, prim_num.		*/
      ret = mvlu_find_struct_comp (nameToFind, &rt_type, &num_rt, &offset, &prim_num);
      if (ret == SD_FAILURE)
	{
	/* Many things can cause this so just use FLOW Logging.	*/
	MVLU_LOG_FLOW2 ("Could not find name component %s from name %s",
			nameToFind, flatname);
	return (SD_FAILURE);
	}

      /* If array index was found, do array processing now.	*/
      if (openbracket)
        {	/* array index was found	*/
        if (rt_type->el_tag != RT_ARR_START)
          {
          MVL_LOG_ERR1 ("Array index specified for non-array component in flat name %s",
              flatname);
          return (SD_FAILURE);
          }
        /* CRITICAL: make sure array index is not out of range.	*/
        if (arr_index >= (ST_UINT) rt_type->u.arr.num_elmnts)
          {
          MVL_LOG_ERR3 ("Array index %u out of bounds in flat name '%s'. Array size = %u",
                arr_index, flatname, (ST_UINT) rt_type->u.arr.num_elmnts);
          return (SD_FAILURE);
          }
        offset += rt_type->el_size;	/* ARR_START size (usually 0)*/

        rt_type++;		/* point to array element	*/
        num_rt -= 2;
        offset   += rt_type->offset_to_last * arr_index;
        prim_num += rt_type->mvluTypeInfo.prim_count * arr_index;
        }

      /* OK, we now have found the component in the runtime type, and our	*/
      /* runtime pointer and numRt reflect the sub-runtime type.		*/
      /* next component.							*/

      /* Prepare to find the next level component name			*/
      nameToFind = compEnd+1;
      }	/* end while	*/
    }	/* end if (flatname != NULL)*/

  /* Now deal with real alternate access.	*/
  if (alt_acc != NULL)
    {
    for (aa_idx = 0; aa_idx < alt_acc->num_aa; aa_idx++)
      {
      ALT_ACC_EL *next_el;
      ST_CHAR *comp_name;	/* component name (used only if AA_COMP..)*/

      next_el = &alt_acc->aa[aa_idx];
      switch (next_el->sel_type)
        {
        case AA_COMP :
        case AA_COMP_NEST :
          comp_name = next_el->u.component;
          /* Find the component name in the current runtime type nest level	*/
          ret = mvlu_find_struct_comp (comp_name, &rt_type, &num_rt, &offset, &prim_num);
          if (ret == SD_FAILURE)
            {
            /* Many things can cause this so just use FLOW Logging.	*/
            MVLU_LOG_FLOW1 ("Could not find name component %s", comp_name);
            return (SD_FAILURE);
            }
          break;
        case AA_INDEX_NEST :
        case AA_INDEX :
          comp_name = ms_comp_name_find (rt_type);	/* this macro never returns NULL*/
          if (rt_type->el_tag != RT_ARR_START)
            {
            MVL_LOG_ERR1 ("Specified array index on non-array component %s", comp_name);
            return (SD_FAILURE);
            }
          arr_index = next_el->u.index;

          /* CRITICAL: make sure array index is not out of range.	*/
          if (arr_index >= (ST_UINT) rt_type->u.arr.num_elmnts)
            {
            MVL_LOG_NERR3 ("Array index %u out of bounds for array size %u on component '%s'",
                arr_index, (ST_UINT) rt_type->u.arr.num_elmnts, comp_name);
            return (SD_FAILURE);
            }

          offset += rt_type->el_size;	/* ARR_START size (usually 0)*/

          rt_type++;	/* point to array elem	*/
          num_rt -= 2;

          /* Multiply array element size by array index.		*/
          offset   += rt_type->offset_to_last * arr_index;
          prim_num += rt_type->mvluTypeInfo.prim_count * arr_index;
          break;
	case AA_END_NEST :
          break;	/* just ignore these	*/
        default:
          MVL_LOG_ERR1 ("Alternate access type '%d' not supported", next_el->sel_type);
          return (SD_FAILURE);
        }

      /* OK, we now have found the component in the runtime type, and our	*/
      /* runtime pointer and numRt reflect the sub-runtime type.		*/
      }
    }

  /* Change caller's values only if everything successful.*/
  *rt_out = rt_type;
  *num_rt_out = num_rt;
  *offset_out = offset;
  *prim_num_out = prim_num;
  return (SD_SUCCESS);
  }

/************************************************************************/
/*			chk_var_writable				*/
/* Check if EVERY leaf in this variable is writable. If so, variable	*/
/* is writable.								*/
/* RETURNS:	SD_SUCCESS if variable is writable			*/
/*		SD_FAILURE if variable is NOT writable			*/
/************************************************************************/
static ST_RET chk_var_writable (RUNTIME_TYPE *rt, ST_INT rt_num)
  {
#if !defined(USPS_IOS)
ST_INT j;

  /* DEBUG: This mode is almost never used.	*/
  /* 1 leaf function for all leafs. Assume all leafs writable.	*/
  if (u_mvlu_leaf_wr_ind_fun)
    {
    return (SD_SUCCESS);	/* var is writable	*/
    }

  /* Find index to "u_no_write_allowed" function & save in static var.	*/
  /* This calls "mvlu_find.." only once, the first time through here.	*/
  /* NOTE: if not found, idx will be -1. If found, idx will be >= 0.	*/
  if (idx_u_no_write_allowed == -2)	/* not yet initialized	*/
    idx_u_no_write_allowed = mvlu_find_wr_ind_fun_index ("u_no_write_allowed");
  if (idx_u_no_write_allowed < 0)
    return (SD_SUCCESS);	/* funct not found. Assume var is writable*/

  /* Loop through type def to see if ANY leaf is unwritable.	*/
  for (j = 0; j < rt_num; ++j, ++rt)
    {
    if (ms_is_rt_prim (rt)
        && rt->mvluTypeInfo.wrIndFunIndex == idx_u_no_write_allowed)
      {
      return (SD_FAILURE);	/* var not writable	*/
      }
    }
#endif
  return (SD_SUCCESS);	/* var is writable	*/
  }

/************************************************************************/
/*			chk_write_resp_ready				*/
/* Check if everything ready to send Write response.			*/
/* RETURNS:	SD_SUCCESS if ready					*/
/*		SD_FAILURE if NOT ready					*/
/************************************************************************/
static ST_RET chk_write_resp_ready (MVL_IND_PEND *indCtrl)
  {
MVLAS_WRITE_CTRL *wrCtrl;
MVLAS_WR_VA_CTRL *wrVaCtrl;	/* current var in list of var	*/
ST_INT i;
ST_BOOLEAN retcode;
    
  wrCtrl = &indCtrl->u.wr;

  /* If all primitives for all variables are complete, ready to send resp.*/
  wrVaCtrl = wrCtrl->vaCtrlTbl;
  for (i = 0; i < wrCtrl->numVar; ++i, ++wrVaCtrl)
    {
    if (wrVaCtrl->numPrimDataDone != wrVaCtrl->numPrimData)
      break;	/* prims NOT done for this var	*/
    }
  if (i == wrCtrl->numVar)
    retcode = SD_SUCCESS;	/* ready to send response.	*/
  else 
    retcode = SD_FAILURE;	/* not ready	*/
  return (retcode);
  }
/************************************************************************/
/*			set_write_resp_dae				*/
/* Set MMS Write Response DataAccessError for this variable.		*/
/************************************************************************/
static ST_VOID set_write_resp_dae (MVLAS_WR_VA_CTRL *wrVaCtrl,
	ST_INT16 dae)	/* MMS DataAccessError code	*/
  {
  wrVaCtrl->resp_tag = WR_RSLT_FAILURE;
  wrVaCtrl->failure = dae;
  wrVaCtrl->numPrimDataDone = wrVaCtrl->numPrimData;	/* pretend like all prim done*/
  }

/************************************************************************/
/*			mvlu_flat_var_create				*/
/* Create a "derived" UCA/IEC 61850 variable from a base variable by	*/
/* specifying a "flattened" name.					*/
/* NOTE: use mvlu_flat_var_destroy to free this var.			*/
/************************************************************************/
MVL_VAR_ASSOC *mvlu_flat_var_create (
	MVL_VAR_ASSOC *base_va,	/* base variable (Logical Node)		*/
	ST_CHAR *flatname)	/* name of attribute inside base var	*/
  {
RUNTIME_TYPE *comp_rt_type;	/* set by mvlu_find_component	*/
ST_INT comp_rt_num;		/* set by mvlu_find_component	*/

ST_INT offset_from_base;	/* set by mvlu_find_component	*/
ST_UINT prim_num;		/* set by mvlu_find_component	*/

ST_INT sub_type_id;	/* type_id for this component. Create it.	*/
MVL_VAR_ASSOC *var;
ST_RET rc;

  /* Find the attribute type.	*/
  /* Use mvlu_find_component to compute offset_from_base & prim_num.	*/
  /* NOTE: mvlu_find_component sets comp_rt_type, comp_rt_num,		*/
  /*       offset_from_base, prim_num.				*/
  /* NOTE: offset_from_base, prim_num different for each array elem.*/
  /* CRITICAL: mvlu_find_component expects alt_acc=NULL if Alt Access	*/
  /*           Spec NOT present. Make sure "alt_acc" is set correctly.	*/
  rc = mvlu_find_component (base_va, flatname, NULL,	/* alt_acc	*/
           &comp_rt_type, &comp_rt_num, &offset_from_base, &prim_num);
  if (rc)
    return (NULL);	/* if this fails, stop now.	*/

  /* comp_rt_type and comp_rt_num now represent a sub-type.	*/

  /* Create a temporary type.			*/
  rc = mvlu_add_rt_type (comp_rt_type, comp_rt_num, &sub_type_id);
  if (rc != SD_SUCCESS)
    {
    MVL_LOG_NERR0 ("Error - could not add temp RT type");
    return (NULL);
    }

  /* Create "derived" variable from base_va.				*/
  /* NOTE: var name is not important. Just use flatname to ease debugging.*/
  var = mvl_var_create_derived (flatname, sub_type_id,
              base_va,
              offset_from_base,
              prim_num);
  if (var == NULL)
    {	/* var create failed, must free tmp type just created.	*/
    mvlu_free_rt_type (sub_type_id);
    }
  return (var);
  }

/************************************************************************/
/*			mvlu_flat_var_destroy				*/
/* Free type and variable created by "mvlu_flat_var_create".		*/
/************************************************************************/
ST_VOID mvlu_flat_var_destroy (
	MVL_VAR_ASSOC *var)	/* var created by mvlu_flat_var_create	*/
  {
  mvlu_free_rt_type (var->type_id);	/* free tmp type	*/
  mvl_var_destroy (var);
  }

/************************************************************************/
#endif	/* defined(MVL_UCA)	*/
/************************************************************************/

