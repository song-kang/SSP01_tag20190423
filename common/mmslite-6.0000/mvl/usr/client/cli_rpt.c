/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	2003 - 2005, All Rights Reserved				*/
/*									*/
/* MODULE NAME : cli_rpt.c						*/
/* PRODUCT(S)  : MMSEASE-LITE						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*	Functions to perform "client" processing of IEC-61850 Reports	*/
/*	and UCA Reports received from "servers".			*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*	rpt_typeids_find						*/
/*	rcb_info_create							*/
/*	rcb_info_destroy						*/
/*	u_iec_rpt_ind							*/
/*	u_iec_rpt_ind_data (user modify or replace)			*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 01/12/10  JRB     11    Fix log messages.				*/
/* 03/05/08  JRB     10    Allow AA_SPEC or VMD_SPEC DatSet.		*/
/*                         Allow 129 char DatSet (MVL61850_MAX_OBJREF_LEN)*/
/*			   Use MVL61850_MAX_.. for RptID (still 65 char)*/
/*			   Add rcb_disable.				*/
/* 10/15/07  JRB     09    Add RptID to log msg.			*/
/* 07/22/05  JRB     08    Allow one conn to control multiple RCBs	*/
/*		 	   (user_info must point to ALL_RCB_INFO).	*/
/*			   Save varNameArray in rcb_info to use later.	*/
/*			   Chg u_iec_rpt_ind_data 4th arg to (RCB_INFO *).*/
/* 12/03/04  JRB     07    Extract domain name of NVL from dataSetName.	*/
/*			   Increase RptID length to vstring65.		*/
/* 08/04/04  EJV     06    rcb_info_create: added typecast for Inclusion*/
/* 06/29/04  JRB     05    Del mvl_tdl_to_type_id & instead use new	*/
/*			   mvl_type_id_create_from_tdl.			*/
/* 05/13/04  JRB     04    Chg SqNum to INT16U to match 61850-7-2.	*/
/* 01/23/04  JRB     03    Use ms_local_to_text.			*/
/* 12/17/03  JRB     02    61850-8-1 FDIS changes:			*/
/*			   Decode ConfRev in rpt if enabled by OptFlds.	*/
/*			   Move SubSeqNum, MoreSegmentsFollow to just	*/
/*			   before inclusion bitstring.			*/
/*			   Chg OptFlds from bvstring9 to bvstring10.	*/
/* 10/24/03  JRB     01    New						*/
/************************************************************************/
#include "glbtypes.h"
#include "sysincs.h"
#include "mvl_acse.h"
#include "mvl_log.h"
#include "mvl_uca.h"	/* only need OPTFLD_* defines	*/
#include "client.h"

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/
/************************************************************************/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/
/* Global variables.							*/
/************************************************************************/
/* NONE	*/

/************************************************************************/
/* Static function prototypes.						*/
/************************************************************************/
static OBJECT_NAME **nvl_var_name_array_create (MVL_NET_INFO *net_info, 
	OBJECT_NAME *nvl_oname,
	ST_INT *numVarOut,
	ST_INT timeOut);
static ST_VOID nvl_var_name_array_destroy (OBJECT_NAME **varNameArray, ST_INT numVar);
static ST_INT var_type_create (MVL_NET_INFO *net_info, OBJECT_NAME *varObj,
		ST_INT timeOut);
static ST_RET rcb_info_var_create (RCB_INFO *rcb_info, RPT_TYPEIDS *rpt_typeids);
static ST_VOID rcb_info_var_destroy (RCB_INFO *rcb_info);
static ST_VOID log_var_data (MVL_VAR_ASSOC *var);

/* DEBUG: move these functions to library modules?	*/
OBJECT_NAME *object_name_clone_create (OBJECT_NAME *srcobj);
ST_VOID object_name_clone_destroy (OBJECT_NAME *obj);

/************************************************************************/
/*			rpt_typeids_find				*/
/* Find ALL types needed for controlling & decoding IEC/UCA reports.	*/
/* These types must be defined in the ODF file and created by Foundry.	*/
/* RETURN: SD_SUCCESS or SD_FAILURE (if ANY type is NOT found)		*/
/* NOTE: this function based on "mvlu_rpt_find_typeids" in "mvlu_rpt.c".*/
/************************************************************************/
ST_RET rpt_typeids_find (RPT_TYPEIDS *rpt_typeids)
  {
ST_RET retCode = SD_FAILURE;	/* assume FAILURE for now	*/
ST_CHAR *type_name;		/* name of type to be found	*/

  /* stop on any error	*/
  do
    {	/* "one-time" loop: just to have something to break out of	*/
    if ((rpt_typeids->mmsbool = mvl_typename_to_typeid (type_name = "RTYP_BOOL")) < 0)
      break;
    if ((rpt_typeids->bstr6 = mvl_typename_to_typeid (type_name = "RTYP_BSTR6")) < 0)
      break;
    if ((rpt_typeids->bstr8 = mvl_typename_to_typeid (type_name = "RTYP_BSTR8")) < 0)
      break;
    if ((rpt_typeids->bstr9 = mvl_typename_to_typeid (type_name = "RTYP_BSTR9")) < 0)
      break;
    if ((rpt_typeids->bvstring6 = mvl_typename_to_typeid (type_name = "RTYP_BVSTR6")) < 0)
      break;
    if ((rpt_typeids->bvstring8 = mvl_typename_to_typeid (type_name = "RTYP_BVSTR8")) < 0)
      break;
    if ((rpt_typeids->bvstring10 = mvl_typename_to_typeid (type_name = "RTYP_BVSTR10")) < 0)
      break;
    if ((rpt_typeids->btime6 = mvl_typename_to_typeid (type_name = "RTYP_BTIME6")) < 0)
      break;
    if ((rpt_typeids->int8u = mvl_typename_to_typeid (type_name = "RTYP_INT8U")) < 0)
      break;
    if ((rpt_typeids->int16u = mvl_typename_to_typeid (type_name = "RTYP_INT16U")) < 0)
      break;
    if ((rpt_typeids->int32u = mvl_typename_to_typeid (type_name = "RTYP_INT32U")) < 0)
      break;
    if ((rpt_typeids->ostring8 = mvl_typename_to_typeid (type_name = "RTYP_OSTR8")) < 0)
      break;
    if ((rpt_typeids->vstring32 = mvl_typename_to_typeid (type_name = "RTYP_VSTR32")) < 0)
      break;
    if ((rpt_typeids->vstring65 = mvl_typename_to_typeid (type_name = "RTYP_VSTR65")) < 0)
      break;
    if ((rpt_typeids->objref = mvl_typename_to_typeid (type_name = "RTYP_OBJREF")) < 0)
      break;
    
    retCode = SD_SUCCESS;  /* If we get here, all were successful	*/
    } while (0);	/* end of "one-time" loop	*/

  if (retCode)
    MVL_LOG_ERR1 ("Can't find type '%s'", type_name);
  return (retCode);	/* If ANY find failed, SD_FAILURE is returned	*/
  }

/************************************************************************/
/*			rcb_info_create					*/
/************************************************************************/
RCB_INFO *rcb_info_create (MVL_NET_INFO *net_info, ST_CHAR *domName,
	ST_CHAR *rcbName, RPT_TYPEIDS *rpt_typeids, ST_INT timeOut)
  {
RCB_INFO *rcb_info;
ST_CHAR varName [MAX_IDENT_LEN+1];
ST_CHAR datSetName [MVL61850_MAX_OBJREF_LEN+1];	/* data set name	*/
ST_CHAR tmpbuf     [MVL61850_MAX_OBJREF_LEN+1];
ST_CHAR RptID [MVL61850_MAX_RPTID_LEN+1];	/* RptID		*/
ST_INT numDsVar;		/* num variables in NVL		*/
OBJECT_NAME **varNameArray;	/* array of variable names in NVL	*/
ST_INT j;
size_t extended_size;
ST_CHAR *extended_ptr;
ST_CHAR InclusionTdl [30];	/* place to create TDL for inclusion bstr*/
ST_RET status = SD_SUCCESS;	/* set to SD_FAILURE if anything fails.	*/
ST_INT rcb_type;
OBJECT_NAME nvl_oname;		/* MMS ObjectName for NVL.	*/

  assert (rpt_typeids->mmsbool || rpt_typeids->int8u);	/* make sure global struct initialized*/

  /* Read "RptID" from the server & save in "RptID" local variable.	*/
  /* Copy to "rcb_info" struct later.					*/
  strcpy (varName, rcbName);
  strcat (varName, "$RptID");
  if (named_var_read (net_info, varName, DOM_SPEC, domName, rpt_typeids->vstring65, RptID, timeOut))
    {
    SLOGALWAYS2 ("Error reading RptID '%s' in domain '%s'", varName, domName);
    return (NULL);
    }

  /* Figure out RCB type from 'rcbName'.	*/
  if (strstr (rcbName, "$BR$") != NULL)
    rcb_type = RCB_TYPE_IEC_BRCB;
  else if (strstr (rcbName, "$RP$") != NULL)
    {
    /* This could be IEC URCB or UCA URCB. Only IEC contains "Resv" attr.*/
    /* Try to read "Resv". If successful, RCB is IEC_URCB, else it is UCA.*/
    /* NOTE: "RptID" read was successful, so we know this RCB exists.	*/
    ST_BOOLEAN tmpResv;	
    strcpy (varName, rcbName);
    strcat (varName, "$Resv");
    if (named_var_read (net_info, varName, DOM_SPEC, domName, rpt_typeids->mmsbool, &tmpResv, timeOut)==SD_SUCCESS)
      rcb_type = RCB_TYPE_IEC_URCB;
    else
      rcb_type = RCB_TYPE_UCA;
    }
  else
    {
    SLOGALWAYS1 ("RCB name '%s' invalid. Must contain 'BR' or 'RP'.", rcbName);
    return (NULL);
    }

  /* Read "DatSet" from the server.	*/
  strcpy (varName, rcbName);
  strcat (varName, "$DatSet");
  if (named_var_read (net_info, varName, DOM_SPEC, domName, rpt_typeids->objref, datSetName, timeOut))
    {	/* can't read data set name	*/
    SLOGALWAYS2 ("Error reading data set name ('%s' in domain '%s')", varName, domName);
    return (NULL);
    }

  /* datSetName should now be set	*/
  /* strtok modifies input buffer, so copy to temporary buffer first.	*/
  strcpy (tmpbuf, datSetName);
  if (tmpbuf[0] == '@')
    {	/* AA-Specific DatSet	*/
    nvl_oname.obj_name.vmd_spec = &tmpbuf[1];	/*extract NVL name*/
    nvl_oname.object_tag = AA_SPEC;
    }
  else if (tmpbuf[0] == '/')
    {	/* VMD-Specific DatSet	*/
    nvl_oname.obj_name.vmd_spec = &tmpbuf[1];	/*extract NVL name*/
    nvl_oname.object_tag = VMD_SPEC;
    }
  else
    {	/* Domain-Specific DatSet	*/
    nvl_oname.domain_id = strtok (tmpbuf, "/");		/*extract dom name*/
    nvl_oname.obj_name.vmd_spec = strtok (NULL, "");	/*extract NVL name*/
    nvl_oname.object_tag = DOM_SPEC;
    if (nvl_oname.obj_name.vmd_spec == NULL)	/* "/" not found	*/
      {
      SLOGALWAYS1 ("DatSet '%s' invalid.", datSetName);
      return (NULL);
      }
    }

  /* Get NVL Attributes from the server	*/
  /* NOTE: Resources allocated by this funct are freed by calling 	*/
  /*   nvl_var_name_array_destroy (called from rcb_info_destroy).	*/
  varNameArray = nvl_var_name_array_create (net_info, &nvl_oname, &numDsVar, timeOut);
  if (varNameArray == NULL)
    {
    SLOGALWAYS1 ("GetNamedVariableListAttributes request failed for '%s'",
                 nvl_oname.obj_name.vmd_spec);
    return (NULL);
    }
  /* Allocate RCB_INFO structure plus extra room for additional data
   * that depends on "numDsVar". This effectively combines 8 allocations into one.
   * Set pointers in RCB_INFO structure to point into the extra space.
   * CRITICAL: use chk_calloc to start with clean structure
   */
  extended_size = sizeof (RCB_INFO)
      + numDsVar * sizeof (ST_INT)		/* rcb_info->typeIdArr		*/
      + numDsVar * sizeof (MVL_VAR_ASSOC *)	/* rcb_info->rcb_var.dataRefName*/
      + numDsVar * sizeof (MVL_VAR_ASSOC *)	/* rcb_info->rcb_var.dataValue	*/
      + numDsVar * sizeof (MVL_VAR_ASSOC *)	/* rcb_info->rcb_var.Reason	*/
      + numDsVar * 66				/* rcb_info->rcb_data.dataRefName*/
      + numDsVar * sizeof (MMS_BVSTRING)	/* rcb_info->rcb_data.Reason	*/
      + BSTR_NUMBITS_TO_NUMBYTES(numDsVar);	/* rcb_info->rcb_data.Inclusion	*/

  rcb_info = chk_calloc (1, extended_size);

  SLOGALWAYS4 ("Client RCB info " S_FMT_PTR " created: Domain=%s, RCB=%s, RptID=%s",
               rcb_info, domName, rcbName, RptID);
  extended_ptr = (ST_CHAR *)(rcb_info + 1);	/* point after RCB_INFO struct	*/

  rcb_info->typeIdArr = (ST_INT *) extended_ptr;
  extended_ptr += numDsVar * sizeof (ST_INT);	/* point after rcb_info->typeIdArr*/

  rcb_info->rcb_var.dataRefName = (MVL_VAR_ASSOC **) extended_ptr;
  extended_ptr += numDsVar * sizeof (MVL_VAR_ASSOC *);	/* rcb_info->rcb_var.dataRefName*/

  rcb_info->rcb_var.dataValue = (MVL_VAR_ASSOC **) extended_ptr;
  extended_ptr += numDsVar * sizeof (MVL_VAR_ASSOC *);	/* rcb_info->rcb_var.dataValue	*/

  rcb_info->rcb_var.Reason = (MVL_VAR_ASSOC **) extended_ptr;
  extended_ptr += numDsVar * sizeof (MVL_VAR_ASSOC *);	/* rcb_info->rcb_var.Reason	*/

  rcb_info->rcb_data.dataRefName = extended_ptr;
  extended_ptr += numDsVar * 66;			/* rcb_info->rcb_data.dataRefName*/

  rcb_info->rcb_data.Reason = (MMS_BVSTRING *) extended_ptr;
  extended_ptr += numDsVar * sizeof (MMS_BVSTRING);	/* rcb_info->rcb_data.Reason	*/

  rcb_info->rcb_data.Inclusion = (ST_UINT8 *) extended_ptr;
  extended_ptr += BSTR_NUMBITS_TO_NUMBYTES(numDsVar);	/* rcb_info->rcb_data.Inclusion	*/

  /* Make sure we computed sizes correctly.	*/
  assert (extended_ptr == (ST_CHAR *) rcb_info + extended_size);

  /* Fill in other rcb_info structure members.	*/
  rcb_info->rcb_type = rcb_type;
  strcpy (rcb_info->RptID, RptID);	/* save RptID to compare when RPTs received*/
  rcb_info->varNameArray = varNameArray;
  rcb_info->numDsVar = numDsVar;

  /* Create all necessary types.					*/
  /* Try to create all types even if one fails.				*/
  /* This way, we can later destroy all without remembering which	*/
  /* were successfully created.						*/

  /* Create a Type for the Inclusion bitstring. Save it in "rcb_info->InclusionTypeid" */
  sprintf (InclusionTdl, "Bstring%d", rcb_info->numDsVar);
  rcb_info->InclusionTypeid = mvl_type_id_create_from_tdl (NULL, InclusionTdl);
  if (rcb_info->InclusionTypeid < 0)
    {
    SLOGALWAYS0 ("Error - could not add type for 'Inclusion'.");
    status = SD_FAILURE;
    }

  /* Create types and fill in rcb_info->typeIdArr	*/
  /* Start with all set to invalid, so if some fail, cleanup is easier.	*/
  for (j=0; j<numDsVar; j++)
    rcb_info->typeIdArr [j] = -1;	/* invalid type_id	*/
  for (j=0; j<numDsVar; j++)
    {
    if ((rcb_info->typeIdArr [j] = var_type_create (net_info, varNameArray[j], timeOut))
        < 0)
      {	/* couldn't create this type, so stop	*/
      SLOGALWAYS1 ("Error creating type for variable '%s'",
                   varNameArray[j]->obj_name.vmd_spec);
      status = SD_FAILURE;
      break;
      }
    }

  if (status == SD_SUCCESS)
    {
    /* Types created OK, so create variables	*/
    status = rcb_info_var_create (rcb_info, rpt_typeids);	/* returns SD_SUCCESS or SD_FAILURE*/
    }

  if (status != SD_SUCCESS)
    {	/* something failed. Destroy anything created.	*/
    rcb_info_destroy (rcb_info);
    rcb_info = NULL;
    }

  return (rcb_info);
  }

/************************************************************************/
/*			rcb_info_destroy				*/
/************************************************************************/
ST_VOID rcb_info_destroy (RCB_INFO *rcb_info)
  {
ST_INT j;

  /* Free up array of variable names saved in rcb_info_create	*/
  nvl_var_name_array_destroy (rcb_info->varNameArray, rcb_info->numDsVar);

  /* Destroy all variables.	*/
  rcb_info_var_destroy (rcb_info);


  /* Destroy "Inclusion" Type (this should be AFTER destroying "Inclusion" Variable).*/
  if (rcb_info->InclusionTypeid > 0)	/* 0=never created. -1=error	*/
    mvl_type_id_destroy (rcb_info->InclusionTypeid);

  /* Destroy "dataValue" Types (this should be AFTER destroying "dataValue" variables).	*/
  for (j=0; j<rcb_info->numDsVar; j++)
    {
    /* All types were initialized to -1. They are also set to -1 if create failed.*/
    /* Only destroy types that are NOT (-1).				*/
    if (rcb_info->typeIdArr [j] >= 0)
      mvl_type_id_destroy (rcb_info->typeIdArr [j]);
    }

  chk_free (rcb_info);
  SLOGALWAYS1 ("Client RCB info " S_FMT_PTR " destroyed", rcb_info);
  }
/************************************************************************/
/*			nvl_var_name_array_create			*/
/* Send a GetNamedVariableListAttributes request to get a list of	*/
/* variable names in a NamedVariableList (NVL).				*/
/* Assume the NVL is domain-specific					*/
/* RETURN: ptr to array of ptrs to variable names (OBJECT_NAME structs).*/
/*	   Also the variable pointed to by numVarOut contains the number*/
/*         of variables in the NVL.					*/
/* NOTE: this function allocates memory for the (OBJECT_NAME *) array.	*/
/*	Call "nvl_var_name_array_destroy" to free the memory.		*/
/************************************************************************/
static OBJECT_NAME **nvl_var_name_array_create (MVL_NET_INFO *net_info,
	OBJECT_NAME *nvl_oname,
	ST_INT *numVarOut,
	ST_INT timeOut)
  {
MVL_REQ_PEND *reqCtrl;
GETVLIST_REQ_INFO getvlist_req;
GETVLIST_RESP_INFO *getvlist_resp;	/* set to reqCtrl->u.getvlist_resp_info*/
VARIABLE_LIST *variable_list;
OBJECT_NAME **varNameArray;	/* Ptr to array of ptrs to var names	*/
ST_RET ret;			/* general purpose return code	*/
ST_INT j;

  getvlist_req.vl_name = *nvl_oname;

  ret = mvla_getvlist (net_info, &getvlist_req, &reqCtrl);
  if (ret == SD_SUCCESS)
    ret = waitReqDone (reqCtrl, timeOut);
  if (ret != SD_SUCCESS)
    {
    SLOGALWAYS1 ("mvla_getvlist Error = 0x%X.", ret);
    varNameArray = NULL;
    }
  else
    {
    getvlist_resp = reqCtrl->u.getvlist.resp_info;
    variable_list = (VARIABLE_LIST *) (getvlist_resp + 1);
    /* Allocate array of variable names.	*/
    *numVarOut = getvlist_resp->num_of_variables;
    varNameArray = chk_malloc (getvlist_resp->num_of_variables * sizeof (OBJECT_NAME *));
    /* Copy info from response to allocated array	*/
    for (j = 0; j < getvlist_resp->num_of_variables;  j++, variable_list++)
      {
      /* Assume all variables are named.	*/
      assert (variable_list->var_spec.var_spec_tag == VA_SPEC_NAMED);
      varNameArray[j] = object_name_clone_create (&variable_list->var_spec.vs.name); 
      }
    }

  mvl_free_req_ctrl (reqCtrl);	/* CRITICAL:		*/
  return (varNameArray);
  }
/************************************************************************/
/*			nvl_var_name_array_destroy			*/
/* Free up all resources allocated by "nvl_var_name_array_create".	*/
/************************************************************************/
static ST_VOID nvl_var_name_array_destroy (OBJECT_NAME **varNameArray, ST_INT numVar)
  {
ST_INT j;
  for (j = 0; j < numVar; j++)
    object_name_clone_destroy (varNameArray[j]);
  chk_free (varNameArray);
  }

/************************************************************************/
/*			var_type_create					*/
/* Send GetVariableAccessAttributes request. Wait for response. When	*/
/* response received, create new type. Pass NULL as type_name arg to	*/
/* "mvl_type_id_create" (this avoids the problem of duplicate names).	*/
/* RETURN: Type ID							*/
/* NOTE: Call "mvl_type_id_destroy" to destroy the type created here.	*/
/************************************************************************/
static ST_INT var_type_create (MVL_NET_INFO *net_info, OBJECT_NAME *varObj,
	ST_INT timeOut)
  {
MVL_REQ_PEND *reqCtrl;
GETVAR_REQ_INFO getvar_req;
VAR_ACC_TSPEC *type_spec;
ST_INT type_id = -1;	/* invalid. If anything fails, this value returned*/
ST_RET ret;

  getvar_req.req_tag = GETVAR_NAME;
  getvar_req.name = *varObj;
  
  /* Send GetVariableAccessAttributes request & wait for response.	*/
  ret = mvla_getvar (net_info, &getvar_req, &reqCtrl);
  if (ret == SD_SUCCESS)
    ret = waitReqDone (reqCtrl, timeOut);
  if (ret != SD_SUCCESS)
    SLOGALWAYS1 ("GetVariableAccessAttributes error = 0x%x.", ret);
  else
    {
    type_spec = &reqCtrl->u.getvar.resp_info->type_spec;
    /* First arg (type_name) is NULL to avoid duplicate names.	*/
    type_id = mvl_type_id_create (NULL, type_spec->data, type_spec->len);
    }
  mvl_free_req_ctrl (reqCtrl);
  return (type_id);
  }

/************************************************************************/
/* 			rcb_info_var_create				*/
/* Create dummy variables to be used later when a report is received.	*/
/* These variables are needed to decode the data received in a report.	*/
/* This function fills in "rcb_info->rcb_data" and "rcb_info->rcb_var".	*/
/************************************************************************/
static ST_RET rcb_info_var_create (RCB_INFO *rcb_info, RPT_TYPEIDS *rpt_typeids)
  {
OBJECT_NAME dummyvar_objname;	/* dummy variable object name	*/
ST_INT j;
ST_INT incSize;		/* num bytes for inclusion bitstring	*/

/* CRITICAL: "mvl_var_create" is used (NOT "mvl_var_add") to create
 * local variables to store data received later in IEC/UCA Reports.
 * These are NOT real variables, so remote MMS nodes cannot access them.
 * All variables use the same name (dummyvar): that's OK because
 * "mvl_var_create" does NOT add them to the database and nothing needs
 * to use the name.
 */ 

/* NOTE: last arg to mvl_var_create is always SD_FALSE, so the name is
 * NOT copied. This is OK because all names here are fixed strings.
 */

  /* Set up one OBJECT_NAME to use for all variables.	*/
  dummyvar_objname.object_tag = VMD_SPEC;
  dummyvar_objname.obj_name.vmd_spec = "dummyvar";	/* all vars use same name	*/

  if ((rcb_info->rcb_var.RptID = mvl_var_create (&dummyvar_objname, rpt_typeids->vstring65,
			&rcb_info->rcb_data.RptID, NULL, SD_FALSE)) == NULL)
    return (SD_FAILURE);

  if ((rcb_info->rcb_var.OptFlds = mvl_var_create (&dummyvar_objname, rpt_typeids->bvstring10,
			&rcb_info->rcb_data.OptFlds, NULL, SD_FALSE)) == NULL)
    return (SD_FAILURE);

  if ((rcb_info->rcb_var.SqNum = mvl_var_create (&dummyvar_objname, rpt_typeids->int16u,
			&rcb_info->rcb_data.SqNum, NULL, SD_FALSE)) == NULL)
    return (SD_FAILURE);

  if ((rcb_info->rcb_var.TimeOfEntry = mvl_var_create (&dummyvar_objname, rpt_typeids->btime6,
			&rcb_info->rcb_data.TimeOfEntry, NULL, SD_FALSE)) == NULL)
    return (SD_FAILURE);

  if ((rcb_info->rcb_var.DatSetNa = mvl_var_create (&dummyvar_objname, rpt_typeids->objref,
			&rcb_info->rcb_data.DatSetNa, NULL, SD_FALSE)) == NULL)
    return (SD_FAILURE);

  if ((rcb_info->rcb_var.BufOvfl = mvl_var_create (&dummyvar_objname, rpt_typeids->mmsbool,
			&rcb_info->rcb_data.BufOvfl, NULL, SD_FALSE)) == NULL)
    return (SD_FAILURE);

  if ((rcb_info->rcb_var.SubSeqNum = mvl_var_create (&dummyvar_objname, rpt_typeids->int16u,
			&rcb_info->rcb_data.SubSeqNum, NULL, SD_FALSE)) == NULL)
    return (SD_FAILURE);

  if ((rcb_info->rcb_var.MoreSegmentsFollow = mvl_var_create (&dummyvar_objname, rpt_typeids->mmsbool,
			&rcb_info->rcb_data.MoreSegmentsFollow, NULL, SD_FALSE)) == NULL)
    return (SD_FAILURE);

  if ((rcb_info->rcb_var.EntryID = mvl_var_create (&dummyvar_objname, rpt_typeids->ostring8,
			&rcb_info->rcb_data.EntryID, NULL, SD_FALSE)) == NULL)
    return (SD_FAILURE);

  if ((rcb_info->rcb_var.ConfRev = mvl_var_create (&dummyvar_objname, rpt_typeids->int32u,
			&rcb_info->rcb_data.ConfRev, NULL, SD_FALSE)) == NULL)
    return (SD_FAILURE);

  incSize = BSTR_NUMBITS_TO_NUMBYTES(rcb_info->numDsVar);
  /* Note: rcb_info->rcb_data.Inclusion is a "ptr" to data, so don't add "&".	*/
  if ((rcb_info->rcb_var.Inclusion = mvl_var_create (&dummyvar_objname, rcb_info->InclusionTypeid,
			rcb_info->rcb_data.Inclusion, NULL, SD_FALSE)) == NULL)
    return (SD_FAILURE);

  /* Allocate array of DataRefName (vstring65)	*/
  /* & array of (MVL_VAR_ASSOC *).	*/
  /* Can't alloc array of strings, so we just allocate one big buffer	*/
  /* and use [j*66] to find postion for each string in the buffer.	*/
  /* Create separate variable for each array entry.	*/
  for (j=0; j<rcb_info->numDsVar; j++)
    {
    if ((rcb_info->rcb_var.dataRefName[j] = mvl_var_create (&dummyvar_objname, rpt_typeids->vstring65,
			&rcb_info->rcb_data.dataRefName[j*66], NULL, SD_FALSE)) == NULL)
      return (SD_FAILURE);
    }

  /* Create variable for each DataSet variable.	*/
  for (j=0; j<rcb_info->numDsVar; j++)
    {
    ST_VOID *remote_data;	/* buffer to store remote data	*/
    MVL_TYPE_CTRL *type_ctrl;

    type_ctrl = mvl_type_ctrl_find (rcb_info->typeIdArr[j]);
    assert (type_ctrl);	/* find should never fail	*/
    /* Allocate buffer to store remote data.		*/
    remote_data = chk_malloc (type_ctrl->data_size);
    
    if ((rcb_info->rcb_var.dataValue[j] = mvl_var_create (&dummyvar_objname, 
		rcb_info->typeIdArr[j],	/* Use type just created	*/
		remote_data,	/* buffer for data	*/ 
		NULL,		/* proc funs	*/
		SD_FALSE))	/* DON'T copy name	*/
        == NULL)
      {	/* couldn't create this variable, so stop	*/
      return (SD_FAILURE);
      }
    }

  /* Allocate array of Reason (bstr6)(one byte for each Reason)	*/
  /* & array of (MVL_VAR_ASSOC *).	*/
  /* Create separate variable for each array entry.	*/
  for (j=0; j<rcb_info->numDsVar; j++)
    {
    /* Use bvstring8 because IEC sends bstr6 and UCA sends bstr8.	*/
    if ((rcb_info->rcb_var.Reason[j] = mvl_var_create (&dummyvar_objname, rpt_typeids->bvstring8,
			&rcb_info->rcb_data.Reason[j], NULL, SD_FALSE)) == NULL)
      return (SD_FAILURE);
    }

  /* If we get this far, everything was successful.	*/
  return (SD_SUCCESS);
  }

/************************************************************************/
/* 			rcb_info_var_destroy				*/
/* Destroy everything created in "rcb_info_var_create".			*/
/* Check that each was successfully creating before destroying.	Any one	*/
/* of the "mvl_var_create" calls in "rcb_info_var_create" may have failed.*/
/************************************************************************/
static ST_VOID rcb_info_var_destroy (RCB_INFO *rcb_info)
  {
ST_INT j;

  if (rcb_info->rcb_var.RptID)
    mvl_var_destroy (rcb_info->rcb_var.RptID);
  
  if (rcb_info->rcb_var.OptFlds)
    mvl_var_destroy (rcb_info->rcb_var.OptFlds);
  
  if (rcb_info->rcb_var.SqNum)
    mvl_var_destroy (rcb_info->rcb_var.SqNum);

  if (rcb_info->rcb_var.TimeOfEntry)
    mvl_var_destroy (rcb_info->rcb_var.TimeOfEntry);

  if (rcb_info->rcb_var.DatSetNa)
    mvl_var_destroy (rcb_info->rcb_var.DatSetNa);

  if (rcb_info->rcb_var.BufOvfl)
    mvl_var_destroy (rcb_info->rcb_var.BufOvfl);

  if (rcb_info->rcb_var.SubSeqNum)
    mvl_var_destroy (rcb_info->rcb_var.SubSeqNum);

  if (rcb_info->rcb_var.MoreSegmentsFollow)
    mvl_var_destroy (rcb_info->rcb_var.MoreSegmentsFollow);

  if (rcb_info->rcb_var.EntryID)
    mvl_var_destroy (rcb_info->rcb_var.EntryID);

  if (rcb_info->rcb_var.ConfRev)
    mvl_var_destroy (rcb_info->rcb_var.ConfRev);

  if (rcb_info->rcb_var.Inclusion)
    mvl_var_destroy (rcb_info->rcb_var.Inclusion);

  /* Destroy "dataRefName" variables.	*/
  for (j=0; j<rcb_info->numDsVar; j++)
    {
    if (rcb_info->rcb_var.dataRefName[j])
      mvl_var_destroy (rcb_info->rcb_var.dataRefName[j]);
    }

  /* Destroy "dataValue" variables.	*/
  for (j=0; j<rcb_info->numDsVar; j++)
    {
    if (rcb_info->rcb_var.dataValue[j]!=NULL)
      {
      chk_free (rcb_info->rcb_var.dataValue[j]->data);
      mvl_var_destroy (rcb_info->rcb_var.dataValue[j]);
      }
    }

  /* Destroy "Reason" variables.	*/
  for (j=0; j<rcb_info->numDsVar; j++)
    {
    if (rcb_info->rcb_var.Reason[j])
      mvl_var_destroy (rcb_info->rcb_var.Reason[j]);
    }
  }

/************************************************************************/
/* This sample function sets some options and enables the RCB.		*/
/* If any write fails, stop.						*/
/* If more options needed, add more arguments to this function.		*/
/************************************************************************/
ST_RET rcb_enable (MVL_NET_INFO *netInfo, ST_CHAR *domName,
	ST_CHAR *rcbName, ST_UCHAR *OptFlds, ST_UCHAR *TrgOps,
	ST_UINT32 IntgPd, RPT_TYPEIDS *rpt_typeids, ST_INT timeOut)
  {
ST_BOOLEAN RptEna = 1;		/* 1 = enable the report	*/
ST_RET ret = SD_SUCCESS;
ST_CHAR varName [MAX_IDENT_LEN + 1];

  if (ret == SD_SUCCESS)
    {
    sprintf (varName, "%s$IntgPd", rcbName);
    ret = named_var_write (netInfo, varName, DOM_SPEC, domName, rpt_typeids->int32u, (ST_CHAR *) &IntgPd, timeOut);
    }

  if (ret == SD_SUCCESS)
    {
    /* NOTE: only write 9 bits. 10th bit (segmentation) is read-only.	*/
    sprintf (varName, "%s$OptFlds", rcbName);
    ret = named_var_write (netInfo, varName, DOM_SPEC, domName, rpt_typeids->bstr9, OptFlds, timeOut);
    }

  if (ret == SD_SUCCESS)
    {
    sprintf (varName, "%s$TrgOps", rcbName);
    ret = named_var_write (netInfo, varName, DOM_SPEC, domName, rpt_typeids->bstr8, TrgOps, timeOut);
    }

#if 0	/* Add something like this if other options needed.	*/
  if (ret == SD_SUCCESS)
    {
    sprintf (varName, "%s$Trgs", rcbName);
    ret = named_var_write (netInfo, varName, DOM_SPEC, domName, rpt_typeids->int16u, (ST_CHAR *) &Trgs, timeOut);
    }
#endif

  if (ret == SD_SUCCESS)
    {
    sprintf (varName, "%s$RptEna", rcbName);
    ret = named_var_write (netInfo, varName, DOM_SPEC, domName, rpt_typeids->mmsbool, (ST_CHAR *) &RptEna, timeOut);
    }

  return (ret);
  }

/************************************************************************/
/*			rcb_disable					*/
/************************************************************************/
ST_RET rcb_disable (MVL_NET_INFO *netInfo, ST_CHAR *domName,
	ST_CHAR *rcbName, RPT_TYPEIDS *rpt_typeids, ST_INT timeOut)
  {
ST_BOOLEAN RptEna = 0;		/* 0 = disable the report	*/
ST_BOOLEAN Resv = 0;		/* 0 = unreserve the URCB	*/
ST_RET ret = SD_SUCCESS;
ST_CHAR varName [MAX_IDENT_LEN + 1];

  if (ret == SD_SUCCESS)
    {
    sprintf (varName, "%s$RptEna", rcbName);
    ret = named_var_write (netInfo, varName, DOM_SPEC, domName, rpt_typeids->mmsbool, (ST_CHAR *) &RptEna, timeOut);
    }
  /* If this is URCB, unreserve it too (i.e. set Resv=0).	*/
  if (ret == SD_SUCCESS && strstr (rcbName, "$RP$") != NULL)
    {
    sprintf (varName, "%s$Resv", rcbName);
    ret = named_var_write (netInfo, varName, DOM_SPEC, domName, rpt_typeids->mmsbool, (ST_CHAR *) &Resv, timeOut);
    }
  return (ret);
  }

/************************************************************************/
/************************************************************************/
static ST_VOID log_var_data (MVL_VAR_ASSOC *var)
  {
  ST_CHAR tdl_buf [500];	/* increase size if complex TDL expected*/
  MVL_TYPE_CTRL *type_ctrl;
  ST_CHAR *data_text;		/* var data converted to text	*/
  ST_CHAR text_buf [30000];	/* increase size if ms_local_to_text fails	*/

  type_ctrl = mvl_type_ctrl_find (var->type_id);
  if (type_ctrl)
    {
    /* If the TDL produced is longer than max_tdl_len, this function	*/
    /* "gracefully" fails (i.e. returns 0).					*/
    if (ms_runtime_to_tdl (type_ctrl->rt, type_ctrl->num_rt, tdl_buf, sizeof(tdl_buf))>0)
      SLOGCALWAYS1 ("  TYPE: %s", tdl_buf);
    else
      SLOGCALWAYS0 ("  TYPE: unknown");

    data_text = ms_local_to_text (var->data,
                 type_ctrl->rt,
                 type_ctrl->num_rt, text_buf, sizeof (text_buf));
    if (data_text)
      SLOGCALWAYS1 ("  DATA: %s", data_text);
    else
      SLOGCALWAYS0 ("  DATA: cannot be converted to text");
    }
  else
    SLOGCALWAYS0 ("  ERR: type_id is invalid");
  }

/************************************************************************/
/*			u_iec_rpt_ind_data				*/
/* User function to process the received report data. This example	*/
/* function simply writes the data to the log file.			*/
/*									*/
/* It should be easy to modify or rewrite this function to do whatever	*/
/* is appropriate for your application.					*/
/************************************************************************/
ST_VOID u_iec_rpt_ind_data (MVL_VAR_ASSOC **info_va,
	ST_UINT8 *OptFldsData,	/* ptr to data part of OptFlds bvstring	*/
	ST_UINT8 *InclusionData,	/* ptr to Inclusion bstring	*/
	RCB_INFO *rcb_info,
	ST_INT va_total)
  {
ST_INT va_num=0;
ST_INT j;

  SLOGALWAYS0 ("Data received in Information Report");
  SLOGCALWAYS1("Var# %02d:    RptId", va_num);
  log_var_data (info_va[va_num]);
  va_num++;
  SLOGCALWAYS1("Var# %02d:    OptFlds", va_num);
  log_var_data (info_va[va_num]);
  va_num++;

  /* NOTE: Don't change initial entries in "info_va". Add these right after "OptFlds".*/
  if (BSTR_BIT_GET(OptFldsData, OPTFLD_BITNUM_SQNUM))
    {
    SLOGCALWAYS1("Var# %02d:    SqNum", va_num);
    log_var_data (info_va[va_num]);
    va_num++;
    }

  if (BSTR_BIT_GET(OptFldsData, OPTFLD_BITNUM_TIMESTAMP))
    {
    SLOGCALWAYS1("Var# %02d:    TimeOfEntry", va_num);
    log_var_data (info_va[va_num]);
    va_num++;
    }

  if (BSTR_BIT_GET(OptFldsData, OPTFLD_BITNUM_DATSETNAME))
    {
    SLOGCALWAYS1("Var# %02d:    DatSetNa", va_num);
    log_var_data (info_va[va_num]);
    va_num++;
    }

  if (BSTR_BIT_GET (OptFldsData, OPTFLD_BITNUM_BUFOVFL))
    {
    SLOGCALWAYS1("Var# %02d:    BufOvfl", va_num);
    log_var_data (info_va[va_num]);
    va_num++;
    }

  if (BSTR_BIT_GET(OptFldsData, OPTFLD_BITNUM_ENTRYID))
    {
    SLOGCALWAYS1("Var# %02d:    EntryID", va_num);
    log_var_data (info_va[va_num]);
    va_num++;
    }

  if (BSTR_BIT_GET(OptFldsData, OPTFLD_BITNUM_CONFREV))
    {
    SLOGCALWAYS1("Var# %02d:    ConfRev", va_num);
    log_var_data (info_va[va_num]);
    va_num++;
    }

  if (BSTR_BIT_GET (OptFldsData,OPTFLD_BITNUM_SUBSEQNUM))
    {
    SLOGCALWAYS1("Var# %02d:    SubSeqNum", va_num);
    log_var_data (info_va[va_num]);
    va_num++;
    SLOGCALWAYS1("Var# %02d:    MoreSegmentsFollow", va_num);
    log_var_data (info_va[va_num]);
    va_num++;
    }

  SLOGCALWAYS1("Var# %02d:    Inclusion", va_num);
  log_var_data (info_va[va_num]);
  va_num++;

  /* If data-Ref enabled, check "Inclusion" to figure out what is being received.*/
  if (BSTR_BIT_GET(OptFldsData, OPTFLD_BITNUM_DATAREF))
    {
    for (j = 0; j < rcb_info->numDsVar; ++j)
      {
      if (BSTR_BIT_GET (InclusionData, j))
        {
        SLOGCALWAYS2("Var# %02d:    DataRefName# %d", va_num, j);
        log_var_data (info_va[va_num]);
        va_num++;
        }
      }  
    }

  /* HERE'S THE DATA. Check "Inclusion" to figure out what is being received.*/
  for (j = 0; j < rcb_info->numDsVar; ++j)
    {
    if (BSTR_BIT_GET (InclusionData, j))
      {
      SLOGCALWAYS3("Var# %02d:    DataSet Var# %d: Name=%s", va_num, j,
                   rcb_info->varNameArray[j]->obj_name.vmd_spec);
      log_var_data (info_va[va_num]);
      va_num++;
      }
    }  

  /* If "reason" enabled, check "Inclusion" to figure out what is being received.*/
  if (BSTR_BIT_GET(OptFldsData, OPTFLD_BITNUM_REASON))
    {
    for (j = 0; j < rcb_info->numDsVar; ++j)
      {
      if (BSTR_BIT_GET (InclusionData, j))
        {
        SLOGCALWAYS2("Var# %02d:    Reason# %d", va_num, j);
        log_var_data (info_va[va_num]);
        va_num++;
        }
      }  
    }
  assert (va_num==va_total);	/* Did we count right?	*/
  }

/************************************************************************/
/*			object_name_clone_create			*/
/* Create clone of OBJECT_NAME struct. Can't just copy struct because	*/
/* struct contains pointers to strings.					*/
/* The function "object_name_clone_destroy" should be called to destroy	*/
/* the clone.								*/
/************************************************************************/
OBJECT_NAME *object_name_clone_create (OBJECT_NAME *srcobj)
  {
OBJECT_NAME *dstobj;
size_t extended_size;
ST_CHAR *extended_ptr;

  /* Allocate OBJECT_NAME structure plus extra room for additional data.
   * This effectively combines 3 allocations into one.
   * Set pointers in OBJECT_NAME structure to point into the extra space.
   */
  extended_size = sizeof (OBJECT_NAME)
      + MAX_IDENT_LEN + 1			/* dstobj->obj_name.vmd_spec	*/
      + MAX_IDENT_LEN + 1;			/* dstobj->domain_id		*/

  dstobj = chk_malloc (extended_size);

  /* Copy old struct to new struct (before setting ptrs in new struct).	*/
  memcpy (dstobj, srcobj, sizeof (OBJECT_NAME));

  /* Fix ptrs to strings in new struct	*/
  extended_ptr = (ST_CHAR *)(dstobj + 1);	/* point after dstobj struct	*/

  dstobj->obj_name.vmd_spec = extended_ptr;
  extended_ptr += (MAX_IDENT_LEN + 1);		/* point after dstobj->obj_name.vmd_spec*/

  dstobj->domain_id = extended_ptr;

  /* Copy strings to the new struct.	*/
  strcpy (dstobj->obj_name.vmd_spec, srcobj->obj_name.vmd_spec);
  if (dstobj->object_tag == DOM_SPEC)
    strcpy (dstobj->domain_id, srcobj->domain_id);
  return (dstobj);
  }

/************************************************************************/
/*			object_name_clone_destroy			*/
/* Destroy OBJECT_NAME clone created by "object_name_clone_create".	*/
/************************************************************************/
ST_VOID object_name_clone_destroy (OBJECT_NAME *obj)
  {
  /* allocated by object_name_clone_create using chk_malloc, so use chk_free.*/
  chk_free (obj);
  }

/************************************************************************/
/*			u_iec_rpt_ind					*/
/* This function processes ONLY IEC-61850 and UCA Reports. If any	*/
/* other InformationReport is received, it logs an error message and	*/
/* ignores it.								*/  
/* CRITICAL: this function assumes that a pointer to an ALL_RCB_INFO	*/
/*	structure has been saved in the					*/
/*	user_info member of the MVL_NET_INFO structure for this conn.	*/
/************************************************************************/
ST_RET u_iec_rpt_ind (MVL_COMM_EVENT *event)
  {
INFO_REQ_INFO *info_ptr;
ST_INT j, va_num, va_total;
VAR_ACC_SPEC *va_spec;
MVL_VAR_ASSOC **info_va;
RCB_INFO *rcb_info;		
ST_UINT8 *OptFldsData;		/* ptr to data part of OptFlds bvstring	*/
ST_UINT8 *InclusionData;	/* ptr to Inclusion bstring		*/
ST_CHAR saveRptID [MVL61850_MAX_RPTID_LEN+1];
ALL_RCB_INFO *all_rcb_info;
ST_RET retcode = SD_SUCCESS;

  info_ptr = (INFO_REQ_INFO *) event->u.mms.dec_rslt.data_ptr;
  va_spec = &info_ptr->va_spec;
  va_total = info_ptr->num_of_acc_result;

  /* An IEC-61850 or UCA report must be a NamedVariableList named "RPT".
   * Ignore any other InformationReport.
   */
  if (va_spec->var_acc_tag != VAR_ACC_NAMEDLIST
      || strcmp (va_spec->vl_name.obj_name.vmd_spec, "RPT") != 0)
    {
    SLOGALWAYS0 ("Received InformationReport is not a IEC-61850 Report or UCA Report. Ignored.");
    return (SD_FAILURE);
    }

  /* Get "all_rcb_info" from "user_info". User must set "user_info" when conn established.*/
  all_rcb_info = event->net_info->user_info;

  /* Check "all_rcb_info" to see if any RCB has been enabled.	*/
  if (all_rcb_info == NULL || all_rcb_info->rcb_info_list == NULL)
    {
    SLOGALWAYS0 ("Received InformationReport. No IEC-61850 or UCA RCB enabled. Ignored.");
    return (SD_FAILURE);
    }

  /* Create array of (MVL_VAR_ASSOC *) needed for converting the received
   * data to local format.
   * Use variables created earlier to fill in the array. These variables will
   * hold the decoded data.
   * NOTE: any return after this must free info_va (see CLEANUP label).
   */
  info_va = (MVL_VAR_ASSOC **) chk_calloc (va_total, 
				   sizeof (MVL_VAR_ASSOC *));

  /* Must decode the RptID first to find the correct RCB_INFO. Could decode into 
   * any "vstring65" variable. We just use the RptID variable from the first
   * rcb_info on the list.
   */
  rcb_info = all_rcb_info->rcb_info_list;	/* use first rcb_info on list*/
  va_num=0;
  info_va[va_num++] = rcb_info->rcb_var.RptID;

  rcb_info->rcb_data.RptID[0] = '\0';	/* start with empty string	*/
  mvl_info_data_to_local (event, va_num, info_va);
  /* NOTE: decoded RptID is now in "rcb_info->rcb_data.RptID". Save it.*/
  strcpy (saveRptID, rcb_info->rcb_data.RptID);

  /* Search list of "rcb_info" to find one with matching RptID.	*/
  /* If not found, rcb_info will be == NULL.			*/
  for (rcb_info = all_rcb_info->rcb_info_list;
       rcb_info != NULL;
       rcb_info = (RCB_INFO *) list_get_next (all_rcb_info->rcb_info_list, rcb_info))
    {
    if (strcmp (rcb_info->RptID, saveRptID) == 0)
      break;	/* rcb_info now points to right structure	*/
    }

  if (!rcb_info)
    {
    SLOGALWAYS1 ("RptID '%s' not recognized on this connection. Received report ignored.", saveRptID);
    retcode = SD_FAILURE;
    goto CLEANUP;
    }

  va_num=0;
  info_va[va_num++] = rcb_info->rcb_var.RptID;
  info_va[va_num++] = rcb_info->rcb_var.OptFlds;

  /* Perform 1st decode (up through "OptFlds"). Need "OptFlds" to figure
   * out what comes next.
   */
  mvl_info_data_to_local (event, va_num, info_va);

  /* Examine "OptFlds", and set up 2nd decode to decode all options	*/
  OptFldsData = rcb_info->rcb_data.OptFlds.data_1;	/* use local var	*/
  printf ("OptFlds = 0x%02x 0x%02x\n", OptFldsData[0],OptFldsData[1]);	/* 9 bit bstr (2 bytes)	*/

  /* NOTE: Don't change initial entries in "info_va". Add these right after "OptFlds".*/
  if (BSTR_BIT_GET(OptFldsData, OPTFLD_BITNUM_SQNUM))
    info_va[va_num++] = rcb_info->rcb_var.SqNum;

  if (BSTR_BIT_GET(OptFldsData, OPTFLD_BITNUM_TIMESTAMP))
    info_va[va_num++] = rcb_info->rcb_var.TimeOfEntry;

  if (BSTR_BIT_GET(OptFldsData, OPTFLD_BITNUM_DATSETNAME))
    info_va[va_num++] = rcb_info->rcb_var.DatSetNa;

  /* The following optional vars are supported by IEC-61850 but they are
   * NOT supported by UCA. This client must NOT set these OptFlds bits
   * when connected to a UCA server, and these variables will NOT be
   * included in a report received from a UCA server.
   */

  if (BSTR_BIT_GET (OptFldsData, OPTFLD_BITNUM_BUFOVFL))
    info_va[va_num++] = rcb_info->rcb_var.BufOvfl;

  if (BSTR_BIT_GET(OptFldsData, OPTFLD_BITNUM_ENTRYID))
    info_va[va_num++] = rcb_info->rcb_var.EntryID;

  if (BSTR_BIT_GET(OptFldsData, OPTFLD_BITNUM_CONFREV))
    info_va[va_num++] = rcb_info->rcb_var.ConfRev;

  if (BSTR_BIT_GET (OptFldsData,OPTFLD_BITNUM_SUBSEQNUM))
    {
    info_va[va_num++] = rcb_info->rcb_var.SubSeqNum;
    info_va[va_num++] = rcb_info->rcb_var.MoreSegmentsFollow;
    }

  info_va[va_num++] = rcb_info->rcb_var.Inclusion;

  assert (va_num < va_total);

  /* Perform 2nd decode (up through "Inclusion").	*/
  mvl_info_data_to_local (event, va_num, info_va);

  /* Examine "Inclusion", and set up 3rd decode to decode all data.	*/
  InclusionData = rcb_info->rcb_data.Inclusion;	/* use local var	*/
  printf ("Inclusion = 0x%02X\n", InclusionData[0]);	/* Just print 1st byte	*/

  /* NOTE: Don't change initial entries in "info_va". Add these right after "Inclusion".*/

  /* If "data-Ref" enabled, check "Inclusion" to figure out what is being received.*/
  if (BSTR_BIT_GET(OptFldsData, OPTFLD_BITNUM_DATAREF))
    {
    for (j = 0; j < rcb_info->numDsVar; ++j)
      {
      if (BSTR_BIT_GET (InclusionData, j))
        info_va[va_num++] = rcb_info->rcb_var.dataRefName[j];
      }  
    }

  /* HERE'S THE DATA. Check "Inclusion" to figure out what is being received.*/
  for (j = 0; j < rcb_info->numDsVar; ++j)
    {
    if (BSTR_BIT_GET (InclusionData, j))
      info_va[va_num++] = rcb_info->rcb_var.dataValue[j];
    }  

  /* If "reason" enabled, check "Inclusion" to figure out what is being received.*/
  if (BSTR_BIT_GET(OptFldsData, OPTFLD_BITNUM_REASON))
    {
    for (j = 0; j < rcb_info->numDsVar; ++j)
      {
      if (BSTR_BIT_GET (InclusionData, j))
        info_va[va_num++] = rcb_info->rcb_var.Reason[j];
      }  
    }

  /* Does num of variables received match expected num.*/
  if (va_num!=va_total)
    {
    SLOGALWAYS2 ("Num of var received in RPT (%d) does not match expected (%d). Possibly incorrect OptFlds or inclusion bitstring", va_total, va_num);
    printf      ("Num of var received in RPT (%d) does not match expected (%d). Possibly incorrect OptFlds or inclusion bitstring\n", va_total, va_num);
    }
  else
    {
    /* Perform 3rd decode (everything).	*/
    mvl_info_data_to_local (event, va_num, info_va);

    u_iec_rpt_ind_data (info_va, OptFldsData, InclusionData, rcb_info, va_total);
    }

CLEANUP:
  chk_free (info_va);
  return (retcode);
  }
