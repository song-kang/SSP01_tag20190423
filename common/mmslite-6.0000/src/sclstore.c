/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	2004-2012 All Rights Reserved					*/
/*									*/
/* MODULE NAME : sclstore.c						*/
/* PRODUCT(S)  : MMS-EASE-LITE						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*	Functions to store information parsed from SCL file.		*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*			scl_lntype_create				*/
/*			scl_lntype_add_do				*/
/*			scl_dotype_create				*/
/*			scl_dotype_add_da				*/
/*			scl_dotype_add_sdo				*/
/*			scl_datype_create				*/
/*			scl_datype_add_bda				*/
/*			scl_enumtype_create				*/
/*			scl_enumtype_add_enumval			*/
/*			scl_fcda_add					*/
/*			scl_dai_add					*/
/*			scl_dataset_add					*/
/*			scl_rcb_add					*/
/*			scl_lcb_add					*/
/*			scl_gcb_add					*/
/*			scl_ln_add					*/
/*			scl_ld_create					*/
/*			scl_info_destroy				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev     Comments					*/
/* --------  ---  ------   -------------------------------------------  */
/* 01/11/12  JRB	   Many Ed 2 changes (see V6.0000 release notes).*/
/* 02/23/11  JRB	   Add "*_sg_val_add" functions.		*/
/*			   scl_info_destroy: free sgValHead lists.	*/
/* 12/02/10  JRB	   Chg EnumVal to ptr & add EnumValBuf.		*/
/* 04/23/08  JRB    11     Add scl_sgcb_add.				*/
/* 07/26/06  JRB    10     Add scl_subnet_add, scl_cap_add, scl_gse_add,*/
/*			   scl_smv_add.					*/
/* 04/04/06  JRB    09	   Add scl_svcb_add: alloc SCL_SVCB & add to list.*/
/* 03/15/06  JRB    08     Simplify functions: Do not copy data.	*/
/*			    Most functions take only (SCL_INFO *) arg &	*/
/*			    just alloc struct and add to linked list.	*/
/*			   Del scl_dotype_add_da_val,			*/
/*			    scl_datype_add_bda_val, scl_header_save.	*/
/* 01/30/06  GLB    07     Integrated porting changes for VMS           */
/* 07/25/05  JRB    06     If nameStructure!=IEDName, ignore it and	*/
/*			   assume nameStructure=IEDName anyway.		*/
/* 06/29/05  CRM    05     Add scl_info_destroy.			*/
/* 06/24/05  JRB    04     Chg "ord" from unsigned to signed value.	*/
/* 02/15/05  JRB    03     Chg scl_ld_create to generate scl_ld->domName.*/
/*			   Chg scl_fcda_add to generate scl_fcda->domName.*/
/*			   Chg scl_ln_add to generate scl_ln->varName.	*/
/*			   Add scl_header_save.				*/
/* 07/19/04  JRB    02     Add scl_lcb_add, scl_gcb_add.		*/
/* 06/10/04  JRB    01     Initial Revision.				*/
/************************************************************************/
#include "glbtypes.h"
#include "sysincs.h"
#include "scl.h"
#include "sx_log.h"
#include "str_util.h"

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif


/* NOTE:
 * Functions with "scl_" prefix used while parsing SCL file to
 * store data in linked lists.
 * Functions with "scl2_" prefix (see sclproc.c) used to read info from
 * linked lists and create MMS objects (Domains, variables, NVLs, etc.).
 */

/************************************************************************/
/*			scl_lntype_create				*/
/* Begin creation of a Logical Node Type (LNodeType).			*/
/* RETURNS:	SD_SUCCESS or error code				*/
/************************************************************************/
SCL_LNTYPE *scl_lntype_create (
		SCL_INFO *scl_info)	/* struct to store all SCL info	*/
  {
SCL_LNTYPE *scl_lntype;

  scl_lntype = (SCL_LNTYPE *) chk_calloc (1, sizeof (SCL_LNTYPE));
  /* Add LNType to front of LNType List.	*/
  list_add_first (&scl_info->lnTypeHead, scl_lntype);
  return (scl_lntype);
  }

/************************************************************************/
/*			scl_lntype_add_do				*/
/* Add a Data Object (DO) to a Logical Node Type (LNodeType).		*/
/* Adds to lntype created by most recent call to "scl_lntype_create".	*/
/* RETURNS:	SD_SUCCESS or error code				*/
/************************************************************************/
SCL_DO *scl_lntype_add_do (
	SCL_INFO *scl_info)	/* main struct where all SCL info stored*/
  {
SCL_DO *scl_do = NULL;	/* assume failure	*/

  /* All higher level linked lists must be initialized.	*/
  if (scl_info->lnTypeHead)
    {
    scl_do = (SCL_DO *) chk_calloc (1, sizeof (SCL_DO));
    /* Add DO to front of DO List in first entry of LNType list	*/
    list_add_first (&scl_info->lnTypeHead->doHead, scl_do);
    }
  else
    SXLOG_ERR0 ("Cannot add DO to NULL LNTYPE");
  return (scl_do);
  }

/************************************************************************/
/*			scl_dotype_create				*/
/* Begin creation of a Data Object Type (DOType).			*/
/* RETURNS:	SD_SUCCESS or error code				*/
/************************************************************************/
SCL_DOTYPE *scl_dotype_create (
	SCL_INFO *scl_info)	/* main struct where all SCL info stored*/
  {
SCL_DOTYPE *scl_dotype;

  scl_dotype = (SCL_DOTYPE *) chk_calloc (1, sizeof (SCL_DOTYPE));
  /* Add DOType to front of DOType List.	*/
  list_add_first (&scl_info->doTypeHead, scl_dotype);
  return (scl_dotype);
  }

/************************************************************************/
/*			scl_dotype_add_da				*/
/* Add a Data Attribute (DA) to a Data Object Type (DOType).		*/
/* RETURNS:	SD_SUCCESS or error code				*/
/************************************************************************/
SCL_DA *scl_dotype_add_da (
	SCL_INFO *scl_info)	/* main struct where all SCL info stored*/
  {
SCL_DA *scl_da = NULL;	/* assume failure	*/

  /* All higher level linked lists must be initialized.	*/
  if (scl_info->doTypeHead)
    {
    scl_da = (SCL_DA *) chk_calloc (1, sizeof (SCL_DA));
    /* CRITICAL: DA and SDO use same struct, "objtype" tells which one it is.*/
    scl_da->objtype = SCL_OBJTYPE_DA;
    /* Add DA to front of DA List in first entry of DOType list	*/
    list_add_first (&scl_info->doTypeHead->daHead, scl_da);
    }
  else
    SXLOG_ERR0 ("Cannot add DA to NULL DO");
  return (scl_da);
  }

/************************************************************************/
/*			scl_dotype_add_sdo				*/
/* Add a (SDO) to a Data Object Type (DOType).				*/
/* RETURNS:	SD_SUCCESS or error code				*/
/************************************************************************/
SCL_DA *scl_dotype_add_sdo (
	SCL_INFO *scl_info)	/* main struct where all SCL info stored*/
  {
SCL_DA *scl_da = NULL;	/* assume failure	*/

  /* All higher level linked lists must be initialized.	*/
  if (scl_info->doTypeHead)
    {
    scl_da = (SCL_DA *) chk_calloc (1, sizeof (SCL_DA));
    /* CRITICAL: DA and SDO use same struct, "objtype" tells which one it is.
     * NOTE: Because this is SDO, the sAddr, bType, and valKind members of
     * the scl_da structure are NOT used.
     */
    scl_da->objtype = SCL_OBJTYPE_SDO;
    /* Add DA to front of DA List in first entry of DOType list	*/
    list_add_first (&scl_info->doTypeHead->daHead, scl_da);
    }
  else
    SXLOG_ERR0 ("Cannot add SDO to NULL DO");
  return (scl_da);
  }

/************************************************************************/
/*			scl_datype_create				*/
/* Begin creation of a Data Attribute Type (DAType).			*/
/* RETURNS:	SD_SUCCESS or error code				*/
/************************************************************************/
SCL_DATYPE *scl_datype_create (
	SCL_INFO *scl_info)	/* main struct where all SCL info stored*/
  {
SCL_DATYPE *scl_datype;

  scl_datype = (SCL_DATYPE *) chk_calloc (1, sizeof (SCL_DATYPE));
  /* Add DAType to front of DAType List.	*/
  list_add_first (&scl_info->daTypeHead, scl_datype);
  return (scl_datype);
  }
/************************************************************************/
/*			scl_datype_add_bda				*/
/* Add a Basic Data Attribute (BDA) to a Data Attribute Type (DAType).	*/
/* RETURNS:	SD_SUCCESS or error code				*/
/************************************************************************/
SCL_BDA *scl_datype_add_bda (
	SCL_INFO *scl_info)	/* main struct where all SCL info stored*/
  {
SCL_BDA *scl_bda = NULL;	/* assume failure	*/

  /* All higher level linked lists must be initialized.	*/
  if (scl_info->daTypeHead)
    {
    scl_bda = (SCL_BDA *) chk_calloc (1, sizeof (SCL_BDA));
    /* Add BDA to front of BDA List in first entry of DAType list	*/
    list_add_first (&scl_info->daTypeHead->bdaHead, scl_bda);
    }
  else
    SXLOG_ERR0 ("Cannot add BDA to NULL DATYPE");
  return (scl_bda);
  }

/************************************************************************/
/*			scl_enumtype_create				*/
/* Begin creation of an EnumType.					*/
/* RETURNS:	SD_SUCCESS or error code				*/
/************************************************************************/
SCL_ENUMTYPE *scl_enumtype_create (
	SCL_INFO *scl_info)	/* main struct where all SCL info stored*/
  {
SCL_ENUMTYPE *scl_enumtype;

  scl_enumtype = (SCL_ENUMTYPE *) chk_calloc (1, sizeof (SCL_ENUMTYPE));
  /* Add EnumType to front of EnumType List.	*/
  list_add_first (&scl_info->enumTypeHead, scl_enumtype);
  return (scl_enumtype);
  }


/************************************************************************/
/*			scl_enumtype_add_enumval			*/
/* Add an EnumVal to an EnumType.					*/
/* Add to EnumType created by most recent call to scl_enumtype_create.	*/
/* RETURNS:	SD_SUCCESS or error code				*/
/************************************************************************/
SCL_ENUMVAL *scl_enumtype_add_enumval (
	SCL_INFO *scl_info)	/* main struct where all SCL info stored*/
  {
SCL_ENUMVAL *scl_enumval = NULL;	/* assume failure	*/

  /* All higher level linked lists must be initialized.	*/
  if (scl_info->enumTypeHead)
    {
    scl_enumval = (SCL_ENUMVAL *) chk_calloc (1, sizeof (SCL_ENUMVAL));
    /* Add EnumVal to front of EnumVal List in first entry of EnumType list	*/
    list_add_first (&scl_info->enumTypeHead->enumvalHead, scl_enumval);
    }
  else
    SXLOG_ERR0 ("Cannot add ENUMVAL to NULL ENUMTYPE");
  return (scl_enumval);
  }


/************************************************************************/
/*			scl_fcda_add				*/
/* Allocates a SCL_FCDA struct						*/
/* and adds it to the linked list "fcdaHead" in SCL_DATASET.		*/
/************************************************************************/
SCL_FCDA *scl_fcda_add (
	SCL_INFO *scl_info)	/* main struct where all SCL info stored*/
  {
SCL_FCDA *scl_fcda = NULL;	/* assume failure	*/

  /* All higher level linked lists must be initialized.	*/
  if (scl_info->serverHead
      && scl_info->serverHead->ldHead
      && scl_info->serverHead->ldHead->lnHead
      && scl_info->serverHead->ldHead->lnHead->datasetHead)
    {
    scl_fcda = (SCL_FCDA *) chk_calloc (1, sizeof (SCL_FCDA));
    /* Add FCDA to front of FCDA List.	*/
    list_add_first (&scl_info->serverHead->ldHead->lnHead->datasetHead->fcdaHead, scl_fcda);
    }
  else
    SXLOG_ERR0 ("Cannot add FCDA to NULL DATASET");

  return (scl_fcda);
  }

/************************************************************************/
/*			scl_dai_add					*/
/* Allocates a SCL_DAI struct						*/
/* and adds it to the linked list "daiHead" in SCL_LN.			*/
/************************************************************************/
SCL_DAI *scl_dai_add (
	SCL_INFO *scl_info)	/* main struct where all SCL info stored*/
  {
SCL_DAI *scl_dai = NULL;	/* assume failure	*/

  /* All higher level linked lists must be initialized.	*/
  if (scl_info->serverHead
      && scl_info->serverHead->ldHead
      && scl_info->serverHead->ldHead->lnHead)
    {
    scl_dai = (SCL_DAI *) chk_calloc (1, sizeof (SCL_DAI));
    /* Add DAI to front of DAI List.	*/
    list_add_first (&scl_info->serverHead->ldHead->lnHead->daiHead, scl_dai);
    }
  else
    SXLOG_ERR0 ("Cannot add DAI to NULL LN");
  return (scl_dai);
  }

/************************************************************************/
/*			scl_extref_add					*/
/* Allocates a SCL_EXTREF struct					*/
/* and adds it to the linked list "extrefHead" in SCL_LN.		*/
/************************************************************************/
SCL_EXTREF *scl_extref_add (
	SCL_INFO *scl_info)	/* main struct where all SCL info stored*/
  {
SCL_EXTREF *scl_extref = NULL;	/* assume failure	*/

  /* All higher level linked lists must be initialized.	*/
  if (scl_info->serverHead
      && scl_info->serverHead->ldHead
      && scl_info->serverHead->ldHead->lnHead)
    {
    scl_extref = (SCL_EXTREF *) chk_calloc (1, sizeof (SCL_EXTREF));
    /* Add EXTREF to front of EXTREF List.	*/
    list_add_first (&scl_info->serverHead->ldHead->lnHead->extrefHead, scl_extref);
    }
  else
    SXLOG_ERR0 ("Cannot add EXTREF to NULL LN");
  return (scl_extref);
  }

/************************************************************************/
/*			scl_dataset_add				*/
/* Allocates a SCL_DATASET struct					*/
/* and adds it to the linked list "datasetHead" in SCL_LN.		*/
/************************************************************************/
SCL_DATASET *scl_dataset_add (
	SCL_INFO *scl_info)	/* main struct where all SCL info stored*/
				/* TRUNCATED if longer than buffer	*/
  {
SCL_DATASET *scl_dataset = NULL;	/* assume failure	*/

  /* All higher level linked lists must be initialized.	*/
  if (scl_info->serverHead
      && scl_info->serverHead->ldHead
      && scl_info->serverHead->ldHead->lnHead)
    {
    scl_dataset = (SCL_DATASET *) chk_calloc (1, sizeof (SCL_DATASET));
    /* Add DATASET to front of DATASET List.	*/
    list_add_first (&scl_info->serverHead->ldHead->lnHead->datasetHead, scl_dataset);
    }
  else
    SXLOG_ERR0 ("Cannot add DATASET to NULL LN");
  return (scl_dataset);
  }

/************************************************************************/
/*			scl_rcb_add					*/
/* Alloc & add SCL_RCB struct to the linked list "rcbHead" in SCL_LN.	*/
/* NOTE: struct is not filled in yet.					*/
/* NOTE: the RptEnabled element of the SCL file is ignored. SISCO software*/
/*	does not need to know which clients may access the RCB.		*/
/************************************************************************/
SCL_RCB *scl_rcb_add (
	SCL_INFO *scl_info)	/* main struct where all SCL info stored*/
  {
SCL_RCB *scl_rcb = NULL;

  /* All higher level linked lists must be initialized.	*/
  if (scl_info->serverHead
      && scl_info->serverHead->ldHead
      && scl_info->serverHead->ldHead->lnHead)
    {
    scl_rcb = (SCL_RCB *) chk_calloc (1, sizeof (SCL_RCB));
    /* Add RCB to front of RCB List.	*/
    list_add_first (&scl_info->serverHead->ldHead->lnHead->rcbHead, scl_rcb);
    }
  else
    SXLOG_ERR0 ("Cannot add RCB to NULL LN");
  return (scl_rcb);
  }

/************************************************************************/
/*			scl_lcb_add					*/
/* Alloc & add a SCL_LCB struct to the linked list "lcbHead" in SCL_LN.	*/
/* NOTE: struct is not filled in yet.					*/
/************************************************************************/
SCL_LCB *scl_lcb_add (
	SCL_INFO *scl_info)	/* main struct where all SCL info stored*/
  {
SCL_LCB *scl_lcb = NULL;

  /* All higher level linked lists must be initialized.	*/
  if (scl_info->serverHead
      && scl_info->serverHead->ldHead
      && scl_info->serverHead->ldHead->lnHead)
    {
    scl_lcb = (SCL_LCB *) chk_calloc (1, sizeof (SCL_LCB));
    /* Add LCB to front of LCB List.	*/
    list_add_first (&scl_info->serverHead->ldHead->lnHead->lcbHead, scl_lcb);
    }
  else
    SXLOG_ERR0 ("Cannot add LCB to NULL LN");
  return (scl_lcb);
  }

/************************************************************************/
/*			scl_gcb_add					*/
/* Add a GOOSE Control Block (GCB).					*/
/* Allocates a SCL_GCB struct						*/
/* and adds it to the linked list "gcbHead" in SCL_LN.			*/
/* NOTE: The SCL file may also contain one or more "IEDName" elements to*/
/*       indicate IEDs that should subscribe for GOOSE data. We have no	*/
/*       way to use this information, so it is ignored.			*/
/************************************************************************/
SCL_GCB *scl_gcb_add (
	SCL_INFO *scl_info)	/* main struct where all SCL info stored*/
  {
SCL_GCB *scl_gcb = NULL;	/* assume failure	*/

  /* All higher level linked lists must be initialized.	*/
  if (scl_info->serverHead
      && scl_info->serverHead->ldHead
      && scl_info->serverHead->ldHead->lnHead)
    {
    scl_gcb = (SCL_GCB *) chk_calloc (1, sizeof (SCL_GCB));
    /* Add GCB to front of GCB List.	*/
    list_add_first (&scl_info->serverHead->ldHead->lnHead->gcbHead, scl_gcb);
    }
  else
    SXLOG_ERR0 ("Cannot add GCB (GOOSE Control Block) to NULL LN");
  return (scl_gcb);
  }

/************************************************************************/
/*			scl_sgcb_add					*/
/* Add a Setting Group Control Block (SGCB).				*/
/* Allocates a SCL_SGCB struct, stores ptr in "sgcb" in SCL_LN.		*/
/************************************************************************/
SCL_SGCB *scl_sgcb_add (
	SCL_INFO *scl_info)	/* main struct where all SCL info stored*/
  {
SCL_SGCB *scl_sgcb = NULL;	/* assume failure	*/

  /* All higher level linked lists must be initialized.	*/
  if (scl_info->serverHead
      && scl_info->serverHead->ldHead
      && scl_info->serverHead->ldHead->lnHead)
    {
    /* Only one SGCB allowed. Make sure not already set.	*/
    if (scl_info->serverHead->ldHead->lnHead->sgcb != NULL)
      {
      SXLOG_ERR0 ("Duplicate SGCB (Setting Group Control Block) not allowed");
      return (NULL);
      }
    else  
      scl_info->serverHead->ldHead->lnHead->sgcb = scl_sgcb = (SCL_SGCB *) chk_calloc (1, sizeof (SCL_SGCB));
    }
  else
    SXLOG_ERR0 ("Cannot add SGCB (Setting Group Control Block) to NULL LN");
  return (scl_sgcb);
  }

/************************************************************************/
/*			scl_svcb_add					*/
/* Alloc & add SCL_SVCB struct to the linked list "svcbHead" in SCL_LN.	*/
/* NOTE: struct is not filled in yet.					*/
/************************************************************************/
SCL_SVCB *scl_svcb_add (
	SCL_INFO *scl_info)	/* main struct where all SCL info stored*/
  {
SCL_SVCB *scl_svcb = NULL;	/* assume failure	*/

  /* All higher level linked lists must be initialized.	*/
  if (scl_info->serverHead
      && scl_info->serverHead->ldHead
      && scl_info->serverHead->ldHead->lnHead)
    {
    scl_svcb = (SCL_SVCB *) chk_calloc (1, sizeof(SCL_SVCB));
    /* Add to front of list.	*/
    list_add_first (&scl_info->serverHead->ldHead->lnHead->svcbHead, scl_svcb);
    }
  else
    SXLOG_ERR0 ("Cannot add SVCB to NULL LN");
  return (scl_svcb);
  }

/************************************************************************/
/*			scl_ln_add					*/
/* Allocates a SCL_LN struct						*/
/* and adds it to the linked list "lnHead" in SCL_LD.			*/
/************************************************************************/
SCL_LN *scl_ln_add (
	SCL_INFO *scl_info)	/* main struct where all SCL info stored*/
  {
SCL_LN *scl_ln = NULL;	/* assume failure	*/

  /* All higher level linked lists must be initialized.	*/
  if (scl_info->serverHead
      && scl_info->serverHead->ldHead)
    {
    scl_ln = (SCL_LN *) chk_calloc (1, sizeof (SCL_LN));
    /* Add LN to front of LN List.	*/
    list_add_first (&scl_info->serverHead->ldHead->lnHead, scl_ln);
    }
  else
    SXLOG_ERR0 ("Cannot add LN to NULL LD");
  return (scl_ln);
  }

/************************************************************************/
/*			scl_ld_create					*/
/* Allocates SCL_LD struct						*/
/* and adds it to the linked list "ldHead" in SCL_INFO.			*/
/************************************************************************/
SCL_LD *scl_ld_create (
	SCL_INFO *scl_info)	/* main struct where all SCL info stored*/
  {
SCL_LD *scl_ld = NULL;	/* assume failure	*/

  /* All higher level linked lists must be initialized.	*/
  if (scl_info->serverHead)
    {
    scl_ld = (SCL_LD *) chk_calloc (1, sizeof (SCL_LD));
    /* Add LD to front of LD List.	*/
    list_add_first (&scl_info->serverHead->ldHead, scl_ld);
    }
  else
    SXLOG_ERR0 ("Cannot add LD to NULL Server");

  return (scl_ld);
  }

/************************************************************************/
/*			scl_subnet_add					*/
/* Allocates SCL_SUBNET struct						*/
/* and adds it to the linked list "subnetHead" in SCL_INFO.		*/
/************************************************************************/
SCL_SUBNET *scl_subnet_add (
	SCL_INFO *scl_info)	/* main struct where all SCL info stored*/
  {
SCL_SUBNET *scl_subnet = NULL;	/* assume failure	*/

  scl_subnet = (SCL_SUBNET *) chk_calloc (1, sizeof (SCL_SUBNET));
  /* Add to front of list.	*/
  list_add_first (&scl_info->subnetHead, scl_subnet);

  return (scl_subnet);
  }

/************************************************************************/
/*			scl_cap_add					*/
/* Allocates a SCL_CAP struct						*/
/* and adds it to the linked list "capHead" in SCL_SUBNET.		*/
/************************************************************************/
SCL_CAP *scl_cap_add (
	SCL_INFO *scl_info)	/* main struct where all SCL info stored*/
  {
SCL_CAP *scl_cap = NULL;	/* assume failure	*/

  /* All higher level linked lists must be initialized.	*/
  if (scl_info->subnetHead)
    {
    scl_cap = (SCL_CAP *) chk_calloc (1, sizeof (SCL_CAP));
    /* Add to front of list.	*/
    list_add_first (&scl_info->subnetHead->capHead, scl_cap);
    }
  else
    SXLOG_ERR0 ("Cannot add CAP to NULL SUBNET");
  return (scl_cap);
  }

/************************************************************************/
/*			scl_gse_add					*/
/* Allocates a SCL_GSE struct						*/
/* and adds it to the linked list "gseHead" in SCL_CAP.			*/
/************************************************************************/
SCL_GSE *scl_gse_add (
	SCL_INFO *scl_info)	/* main struct where all SCL info stored*/
  {
SCL_GSE *scl_gse = NULL;	/* assume failure	*/

  /* All higher level linked lists must be initialized.	*/
  if (scl_info->subnetHead->capHead)
    {
    scl_gse = (SCL_GSE *) chk_calloc (1, sizeof (SCL_GSE));
    /* Add to front of list.	*/
    list_add_first (&scl_info->subnetHead->capHead->gseHead, scl_gse);
    }
  else
    SXLOG_ERR0 ("Cannot add GSE to NULL CAP");
  return (scl_gse);
  }

/************************************************************************/
/*			scl_smv_add					*/
/* Allocates a SCL_SMV struct						*/
/* and adds it to the linked list "smvHead" in SCL_CAP.			*/
/************************************************************************/
SCL_SMV *scl_smv_add (
	SCL_INFO *scl_info)	/* main struct where all SCL info stored*/
  {
SCL_SMV *scl_smv = NULL;	/* assume failure	*/

  /* All higher level linked lists must be initialized.	*/
  if (scl_info->subnetHead->capHead)
    {
    scl_smv = (SCL_SMV *) chk_calloc (1, sizeof (SCL_SMV));
    /* Add to front of list.	*/
    list_add_first (&scl_info->subnetHead->capHead->smvHead, scl_smv);
    }
  else
    SXLOG_ERR0 ("Cannot add SMV to NULL CAP");
  return (scl_smv);
  }

/************************************************************************/
/*			scl_dai_sg_val_add				*/
/* NOTE: scl_dai arg should equal scl_info->ldHead->lnHead->daiHead	*/
/************************************************************************/
SCL_SG_VAL *scl_dai_sg_val_add (
	SCL_DAI *scl_dai)	/* ptr to current DAI element	*/
  {
SCL_SG_VAL *scl_sg_val;

  scl_sg_val = (SCL_SG_VAL *) chk_calloc (1, sizeof (SCL_SG_VAL));
  /* Add to front of List.	*/
  list_add_first (&scl_dai->sgValHead, scl_sg_val);
  return (scl_sg_val);
  }

/************************************************************************/
/*			scl_da_sg_val_add				*/
/* NOTE: scl_da arg should equal scl_info->doTypeHead->daHead		*/
/************************************************************************/
SCL_SG_VAL *scl_da_sg_val_add (
	SCL_DA *scl_da)		/* ptr to current DA element	*/
  {
SCL_SG_VAL *scl_sg_val;

  scl_sg_val = (SCL_SG_VAL *) chk_calloc (1, sizeof (SCL_SG_VAL));
  /* Add to front of List.	*/
  list_add_first (&scl_da->sgValHead, scl_sg_val);
  return (scl_sg_val);
  }

/************************************************************************/
/*			scl_bda_sg_val_add				*/
/* NOTE: scl_bda arg should equal scl_info->daTypeHead->bdaHead		*/
/************************************************************************/
SCL_SG_VAL *scl_bda_sg_val_add (
	SCL_BDA *scl_bda)	/* ptr to current BDA element	*/
  {
SCL_SG_VAL *scl_sg_val;

  scl_sg_val = (SCL_SG_VAL *) chk_calloc (1, sizeof (SCL_SG_VAL));
  /* Add to front of List.	*/
  list_add_first (&scl_bda->sgValHead, scl_sg_val);
  return (scl_sg_val);
  }

/************************************************************************/
/*			scl_server_add					*/
/************************************************************************/
SCL_SERVER *scl_server_add (
	SCL_INFO *scl_info)	/* main struct where all SCL info stored*/
  {
SCL_SERVER *scl_server;

  scl_server = (SCL_SERVER *) chk_calloc (1, sizeof (SCL_SERVER));
  /* Add to front of List.	*/
  list_add_first (&scl_info->serverHead, scl_server);
  return (scl_server);
  }

/************************************************************************/
/*			scl_info_destroy				*/
/* Destroy all info stored in the SCL_INFO structure by "scl_parse".	*/
/* NOTE: most buffers were allocated by functions in this module. The	*/
/*   elements "Val" and "desc" in several structures were allocated	*/
/*   by functions in "sclparse.c".					*/
/************************************************************************/
ST_VOID scl_info_destroy (SCL_INFO *scl_info)
  {
SCL_LNTYPE *lnType;	
SCL_DO *scl_do;
SCL_LD *scl_ld;
SCL_DATYPE * scl_daType;
SCL_BDA *scl_bda;
SCL_ENUMTYPE *scl_enum;
SCL_ENUMVAL *scl_enumval;
SCL_DOTYPE *scl_doType;	
SCL_DA *scl_da;	
SCL_SUBNET *scl_subnet;
SCL_CAP *scl_cap;
SCL_GSE *scl_gse;
SCL_SMV *scl_smv;
SCL_SG_VAL *scl_sg_val;
SCL_SERVER *scl_server;

  while ((scl_subnet = (SCL_SUBNET *) list_get_first(&scl_info->subnetHead)) != NULL)
    {
    while ((scl_cap = (SCL_CAP *) list_get_first(&scl_subnet->capHead)) != NULL)
      {
      while ((scl_gse = (SCL_GSE *) list_get_first(&scl_cap->gseHead)) != NULL)
        {
        chk_free(scl_gse);
        }
      while ((scl_smv = (SCL_SMV *) list_get_first(&scl_cap->smvHead)) != NULL)
        {
        chk_free(scl_smv);
        }
      if (scl_cap->desc)
        chk_free (scl_cap->desc);
      chk_free (scl_cap);
      }
    if (scl_subnet->desc)
      chk_free (scl_subnet->desc);
    chk_free (scl_subnet);
    }

  while ((scl_daType = (SCL_DATYPE *) list_get_first (&scl_info->daTypeHead)) != NULL)
    {	
    while ((scl_bda = (SCL_BDA *) list_get_first (&scl_daType->bdaHead)) != NULL)
      {	
      while ((scl_sg_val = (SCL_SG_VAL *) list_get_first (&scl_bda->sgValHead)) != NULL)
        {
        chk_free (scl_sg_val->Val);
        chk_free (scl_sg_val);
        }
      if (scl_bda->desc)
        chk_free (scl_bda->desc);
      if (scl_bda->Val)
        chk_free (scl_bda->Val);
      chk_free (scl_bda);
      }
    chk_free (scl_daType);
    }

  while ((lnType = (SCL_LNTYPE *) list_get_first (&scl_info->lnTypeHead)) != NULL)
    {	
    while ((scl_do = (SCL_DO *) list_get_first (&lnType->doHead)) != NULL)
      {	
      chk_free (scl_do);
      }
    chk_free (lnType);
    }
  while ((scl_doType = (SCL_DOTYPE *) list_get_first (&scl_info->doTypeHead)) != NULL)
    {	
    while ((scl_da = (SCL_DA *) list_get_first (&scl_doType->daHead)) != NULL)
      {	
      while ((scl_sg_val = (SCL_SG_VAL *) list_get_first (&scl_da->sgValHead)) != NULL)
        {
        chk_free (scl_sg_val->Val);
        chk_free (scl_sg_val);
        }
      if (scl_da->desc)
        chk_free (scl_da->desc);
      if (scl_da->Val)
        chk_free (scl_da->Val);
      chk_free (scl_da);
      }
    chk_free (scl_doType);
    }
  while ((scl_enum = (SCL_ENUMTYPE *) list_get_first (&scl_info->enumTypeHead)) != NULL)
    {	
    while ((scl_enumval = (SCL_ENUMVAL *) list_get_first (&scl_enum->enumvalHead)) != NULL)
      {	
      /* If EnumVal DOES NOT point to EnumValBuf (fixed buffer),	*/
      /* then it was allocated & it must be freed.			*/
      if (scl_enumval->EnumVal != NULL && scl_enumval->EnumVal != scl_enumval->EnumValBuf)
        chk_free (scl_enumval->EnumVal);
      chk_free (scl_enumval);
      }
    chk_free (scl_enum);
    }

  /* CRITICAL: if "SCD" parse mode was used, the SCL_LD list is nested	*/
  /*     in the	SCL_SERVER structure (must have nested loops).		*/
  while ((scl_server = (SCL_SERVER *) list_get_first (&scl_info->serverHead)) != NULL)
    {
    while ((scl_ld = (SCL_LD *) list_get_first (&scl_server->ldHead)) != NULL)
      {	
      scl_ld_destroy (scl_ld);
      }
    chk_free (scl_server);
    }
  }

/************************************************************************/
/*			scl_ld_destroy					*/
/************************************************************************/
ST_VOID scl_ld_destroy (SCL_LD *scl_ld)
  {
SCL_LN *scl_ln;
SCL_RCB *scl_rcb;
SCL_LCB *scl_lcb;
SCL_GCB *scl_gcb;
SCL_SVCB *scl_svcb;
SCL_DAI *scl_dai;
SCL_DATASET *scl_dataset;
SCL_FCDA *scl_fcda;
SCL_SG_VAL *scl_sg_val;
SCL_EXTREF *scl_extref;

  while ((scl_ln = (SCL_LN *) list_get_first (&scl_ld->lnHead)) != NULL)
    {
    while ((scl_dai = (SCL_DAI *) list_get_first (&scl_ln->daiHead)) != NULL)
      {
      while ((scl_sg_val = (SCL_SG_VAL *) list_get_first (&scl_dai->sgValHead)) != NULL)
        {
        chk_free (scl_sg_val->Val);
        chk_free (scl_sg_val);
        }
      if (scl_dai->Val)
        chk_free (scl_dai->Val);
      chk_free (scl_dai);
      }

    while ((scl_dataset = (SCL_DATASET *) list_get_first (&scl_ln->datasetHead)) != NULL)
      {
      while ((scl_fcda = (SCL_FCDA *) list_get_first (&scl_dataset->fcdaHead)) != NULL)
        {
        chk_free (scl_fcda);
        }
      if (scl_dataset->desc)
        chk_free (scl_dataset->desc);
      chk_free (scl_dataset);
      }

    while ((scl_rcb = (SCL_RCB *) list_get_first (&scl_ln->rcbHead)) != NULL)
      {
      if (scl_rcb->desc)
        chk_free (scl_rcb->desc);
      chk_free (scl_rcb);
      }

    while ((scl_lcb = (SCL_LCB *) list_get_first (&scl_ln->lcbHead)) != NULL)
      {
      if (scl_lcb->desc)
        chk_free (scl_lcb->desc);
      chk_free (scl_lcb);
      }

    while ((scl_gcb = (SCL_GCB *) list_get_first (&scl_ln->gcbHead)) != NULL)
      {
      if (scl_gcb->desc)
        chk_free (scl_gcb->desc);
      chk_free (scl_gcb);
      }
    while ((scl_svcb = (SCL_SVCB *) list_get_first (&scl_ln->svcbHead)) != NULL)
      {
      if (scl_svcb->desc)
        chk_free (scl_svcb->desc);
      chk_free (scl_svcb);
      }
    /* Free all scl_extref in this LN.	*/    
    while ((scl_extref = (SCL_EXTREF *) list_get_first (&scl_ln->extrefHead)) != NULL)
      {
      if (scl_extref->desc)
        chk_free (scl_extref->desc);
      if (scl_extref->intAddr)
        chk_free (scl_extref->intAddr);
      chk_free (scl_extref);
      }
    /* Only one SGCB allowed (no linked list)	*/
    if (scl_ln->sgcb)
      {
      if (scl_ln->sgcb->desc)
        chk_free (scl_ln->sgcb->desc);
      chk_free (scl_ln->sgcb);
      }
    if (scl_ln->desc)
      chk_free (scl_ln->desc);
    chk_free (scl_ln);
    }
  if (scl_ld->desc)
    chk_free (scl_ld->desc);
  chk_free (scl_ld);
  }

