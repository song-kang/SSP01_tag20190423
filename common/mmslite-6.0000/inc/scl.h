#ifndef SCL_INCLUDED
#define SCL_INCLUDED
/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	2004-2004 All Rights Reserved					*/
/*									*/
/* MODULE NAME : scl.h							*/
/* PRODUCT(S)  : MMS-EASE-LITE						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*	SCL main header file.						*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev     Comments					*/
/* --------  ---  ------   -------------------------------------------  */
/* 02/20/12  JRB	   Many Ed 2 changes (see V6.0000 release notes).*/
/* 08/12/11  JRB	   Add scl_parse_scd, etc.			*/
/* 02/23/11  JRB           Repl sGroup with sgValHead (linked list).	*/
/*			   Add "*_sg_val_add" functions.		*/
/* 12/08/10  JRB	   u_set_all_leaf_functions: add (SCL_LNTYPE *) arg*/
/* 12/02/10  JRB	   Chg EnumVal to ptr & add EnumValBuf.		*/
/* 09/15/10  JRB	   Fix rptID max len (chg from 64 to 65 char).	*/
/* 10/01/08  JRB    18     Add SCL_ADDRESS, scl_address_find.		*/
/* 07/24/08  JRB    17     Fix len of appID in SCL_GCB (maps to GoID).	*/
/* 04/23/08  JRB    16     Add SCL_SGCB, scl_sgcb_add.			*/
/* 07/03/07  JRB    15     Add scl_gse_find.				*/
/* 06/12/07  JRB    14     Add VLANID in SCL_GSE & SCL_SMV.		*/
/* 05/21/07  RKR    13     Changed MAC type to ST_UCHAR for GSE and SMV */
/* 10/30/06  JRB    12     scl2_datatype_create_all: all args changed.	*/
/*			   scl2_ld_create_all: add vmd_ctrl, is_client args.*/
/*			   Add type_id to SCL_LN, SCL_LNTYPE.		*/
/*			   Chg SCL2_BTYPE.				*/
/* 08/04/06  MDE    11     Added scl_debug_mode, scl2_add_btype, etc.	*/
/* 07/26/06  JRB    10     Add structs,functs for parsing Communication.*/
/* 04/19/06  JRB    09	   Add maxClient to SCL_RCB for "RptEnabled max".*/
/* 03/16/06  JRB    08	   Add SCL_SVCB struct & scl_svcb_add proto.	*/
/* 03/15/06  JRB    07     Chg almost all function prototypes.		*/
/*			   Del scl_dotype_add_da_val,			*/
/*			    scl_datype_add_bda_val, scl_header_save.	*/
/*			   Add sGroup to SCL_DAI.			*/
/*			   Chg desc to ptr (allocated during parse).	*/
/*			   Increase MAX_CDC_LEN for user-defined CDCs.	*/
/* 07/25/05  JRB    12     scl2_ld_create_all: Add brcb_bufsize arg.	*/
/*			   SCL_INFO: add brcb_bufsize element.		*/
/* 06/28/05  JRB    11     Del scl2_ld_destroy_all. Use mvl_vmd_destroy.*/
/* 06/24/05  JRB    10     Chg "ord" from unsigned to signed value.	*/
/* 05/27/05  CRM    09	   Add scl_info_destroy & scl2_ld_destroy_all.	*/
/* 03/22/05  JRB    08     Add ifdef __cplusplus to work with C++ code.	*/
/* 02/15/05  JRB    07     Add iedName to SCL_INFO.			*/
/*			   Add domName to SCL_LD & SCL_FCDA.		*/
/*			   Add varName to SCL_LN.			*/
/*			   Define SCL_HEADER struct & add it to SCL_INFO.*/
/*			   Add scl_header_save.				*/
/* 08/06/04  JRB    06     Add scl_parse.				*/
/*			   Move mapping functions to user header.	*/
/* 07/19/04  JRB    05     Add out_filename arg to datamap_cfg_read.	*/
/*			   Add any_cfg_count proto.			*/
/* 07/15/04  JRB    04     Clean up SCL_GCB, scl_gcb_add.		*/
/* 07/09/04  JRB    03     scl2_ld_create_all: add reportScanRate arg.	*/
/* 07/02/04  JRB    02     Add SCL_LCB, SCL_GCB, scl_lcb_add, scl_gcb_add.*/
/* 06/08/04  JRB    01     Initial Revision.				*/
/************************************************************************/
#include "gen_list.h"
#include "mms_def2.h"		/* need MAX_IDENT_LEN	*/
#include "mvl_defs.h"		/* need MVL_VAR_ASSOC, etc.	*/
#include "sx_defs.h"		/* need SX_DEC_CTRL	*/
#ifdef __cplusplus
extern "C" {
#endif

#define MAX_VALKIND_LEN	4	/* Spec, Conf, RO, or Set		*/
#define MAX_CDC_LEN	50	/* SPS, DPS, etc. (CURVE is longest	*/
				/* predefined CDC but user may define others)*/
#define MAX_FC_LEN	2	/* ST, MX, etc.				*/

/* These defines used in SCL_DA struct to differentiate between structs	*/
/* containing DA info and structs containing SDO info.			*/
#define SCL_OBJTYPE_DA	0
#define SCL_OBJTYPE_SDO	1

/* This def used for flattened leaf names (longer to allow array indices)*/
/* Allow 7 extra char for 5 digit array index & brackets, like [10000]	*/
#define MAX_FLAT_LEN	(MAX_IDENT_LEN+7)

#define SCL_ATTR_OPTIONAL 	0	/* attribute is optional	*/
#define SCL_ATTR_REQUIRED 	1	/* attribute is required	*/

/* Values allowed for "parseMode" in SCL_DEC_CTRL.	*/
#define SCL_PARSE_MODE_CID		0
#define SCL_PARSE_MODE_SCD_ALL		1
#define SCL_PARSE_MODE_SCD_FILTERED	2

/************************************************************************/
/* Structures to contain information from "Header" section of SCL.	*/
/************************************************************************/
typedef struct
  {
  /* NOTE: only required elements included here. Add optional elements as needed.*/
  ST_CHAR id            [MAX_IDENT_LEN+1];

  /* Defined values for "nameStructure" attribute	*/
  #define SCL_NAMESTRUCTURE_IEDNAME	0	/* value="IEDName"	*/
  #define SCL_NAMESTRUCTURE_FUNCNAME	1	/* value="FuncName"	*/
  ST_INT nameStructure;
  } SCL_HEADER;

/************************************************************************/
/* Structures to contain information from "Communications" section of SCL.*/
/************************************************************************/
/* Data from "Address" element (inside "ConnectedAP" element)	*/
typedef struct scl_address
  {
  /* NOTE: there is only one Address allowed, so this is never put on 	*/
  /* a linked list.							*/
  AE_TITLE ae_title;	/* includes AP title, AE qualifier, etc.	*/

  ST_UINT psel_len;
  ST_UCHAR psel[MAX_PSEL_LEN];
  ST_UINT ssel_len;
  ST_UCHAR ssel[MAX_SSEL_LEN];
  ST_UINT tsel_len;
  ST_UCHAR tsel[MAX_TSEL_LEN];
  ST_ULONG ip;			/* IP Addr (network byte order)	*/
  } SCL_ADDRESS;

/* Data from "GSE" element (inside "ConnectedAP" element)	*/
typedef struct scl_gse
  {
  /* CRITICAL: First 2 parameters used to add this struct to linked	*/
  /* lists using list_add_last, etc.					*/
  struct scl_gse *next;		/* CRITICAL: DON'T MOVE.	*/
  struct scl_gse *prev;		/* CRITICAL: DON'T MOVE.	*/
  ST_CHAR ldInst[MAX_IDENT_LEN+1];
  ST_CHAR cbName[MAX_IDENT_LEN+1];
  ST_UCHAR MAC[CLNP_MAX_LEN_MAC];	/* Multicast MAC address	*/
  ST_UINT APPID;
  ST_UINT VLANPRI;
  ST_UINT VLANID;
  ST_UINT MinTime;	/* Minimum GOOSE retrans time (ms)	*/
  ST_UINT MaxTime;	/* Maximum GOOSE retrans time (ms)	*/
  } SCL_GSE;

/* Data from "SMV" element (inside "ConnectedAP" element)	*/
typedef struct scl_smv
  {
  /* CRITICAL: First 2 parameters used to add this struct to linked	*/
  /* lists using list_add_last, etc.					*/
  struct scl_smv *next;		/* CRITICAL: DON'T MOVE.	*/
  struct scl_smv *prev;		/* CRITICAL: DON'T MOVE.	*/
  ST_CHAR ldInst[MAX_IDENT_LEN+1];
  ST_CHAR cbName[MAX_IDENT_LEN+1];
  ST_UCHAR MAC[CLNP_MAX_LEN_MAC];	/* Multicast MAC address	*/
  ST_UINT APPID;
  ST_UINT VLANPRI;
  ST_UINT VLANID;
  } SCL_SMV;

/* Data from "ConnectedAP" element	*/
typedef struct scl_cap
  {
  /* CRITICAL: First 2 parameters used to add this struct to linked	*/
  /* lists using list_add_last, etc.					*/
  struct scl_cap *next;		/* CRITICAL: DON'T MOVE.	*/
  struct scl_cap *prev;		/* CRITICAL: DON'T MOVE.	*/
  ST_CHAR iedName[MAX_IDENT_LEN+1];
  ST_CHAR *desc;		/* description (optional)*/
				/* may be long so allocate if present*/
  ST_CHAR apName[MAX_IDENT_LEN+1];
  SCL_ADDRESS address;		/* one address. NO LINKED LIST.	*/
  SCL_GSE *gseHead;      /* head of list of GSE defs	*/
  SCL_SMV *smvHead;      /* head of list of SMV defs	*/
  } SCL_CAP;

/* Data from "Subnetwork" element	*/
typedef struct scl_subnet
  {
  /* CRITICAL: First 2 parameters used to add this struct to linked	*/
  /* lists using list_add_last, etc.					*/
  struct scl_subnet *next;		/* CRITICAL: DON'T MOVE.	*/
  struct scl_subnet *prev;		/* CRITICAL: DON'T MOVE.	*/
  ST_CHAR name[MAX_IDENT_LEN+1];
  ST_CHAR *desc;		/* description (optional)*/
				/* may be long so allocate if present*/
  ST_CHAR type[MAX_IDENT_LEN+1];
  SCL_CAP *capHead;		/* head of list of ConnectedAP defs	*/
  } SCL_SUBNET;

/************************************************************************/
/* Structures to contain information from "AccessPoint" section of SCL.*/
/************************************************************************/

/* "scl_fcda_add" allocates this struct, fills it in,		*/
/* and adds it to the linked list "fcdaHead" in SCL_DATASET.		*/
typedef struct
  {
  DBL_LNK l;
  ST_CHAR domName[MAX_IDENT_LEN+1];	/* domain name (constructed)	*/
  ST_CHAR ldInst  [MAX_IDENT_LEN+1];
  ST_CHAR prefix  [MAX_IDENT_LEN+1];
  ST_CHAR lnInst  [MAX_IDENT_LEN+1];
  ST_CHAR lnClass [MAX_IDENT_LEN+1];
  ST_CHAR doName  [MAX_IDENT_LEN+1];
  ST_CHAR daName  [MAX_IDENT_LEN+1];
  ST_CHAR fc      [MAX_FC_LEN+1];	/* ST, MX, etc.			*/
  ST_CHAR ix      [5+1];		/* array index (5 digits max)	*/
  } SCL_FCDA;

/* Structure to store <Val> elements with "sGroup" attribute.		*/
/* Must be linked list to store multiple elements.			*/
typedef struct scl_sg_val
  {
  /* CRITICAL: First 2 parameters used to add this struct to linked	*/
  /* lists using list_add_last, etc.					*/
  struct scl_sg_val *next;		/* CRITICAL: DON'T MOVE.	*/
  struct scl_sg_val *prev;		/* CRITICAL: DON'T MOVE.	*/
  ST_UINT sGroup;			/* Setting Group for this Val	*/
  ST_CHAR *Val;				/* Val text			*/
					/* allocate appropriate size buffer*/
  } SCL_SG_VAL;

/* "scl_dai_add" allocates this struct, fills it in,			*/
/* and adds it to the linked list "daiHead" in SCL_LN.			*/
/* The "flattened" name must be constructed from the "name" & "ix"	*/
/* attribute of the DOI and DAI and possibly the intervening SDI,	*/
/* where "ix" is an array index (we'll need some new flattened name	*/
/* syntax to handle the array index).					*/
/* The "accessControl" attr of DOI is ignored (don't know what it means).*/
/* The "desc" attr of DOI, SDI, & DAI are ignored (not useful).		*/
typedef struct
  {
  DBL_LNK l;
  ST_CHAR flattened[MAX_FLAT_LEN+1];	/* flattened attribute name	*/
					/* constructed from "name" & "ix"*/
					/* from DOI, SDI, & DAI		*/
  ST_CHAR *Val;				/* attribute value text		*/
					/* allocate appropriate size buffer*/
  SCL_SG_VAL *sgValHead;		/* linked list of Setting Group	*/
					/* initial values		*/
  ST_CHAR sAddr[MAX_IDENT_LEN+1];	/* from DAI			*/
  ST_CHAR valKind[MAX_VALKIND_LEN+1];	/* from DAI			*/
  } SCL_DAI;

/* "scl_dataset_add" allocates this struct,			*/
/* and adds it to the linked list "datasetHead" in SCL_LN.	*/
typedef struct
  {
  DBL_LNK l;
  ST_CHAR name[MAX_IDENT_LEN+1];	/* dataset name		*/
  ST_CHAR *desc;			/* description (optional)*/
					/* may be long so allocate if present*/
  SCL_FCDA *fcdaHead;			/* head of list of FCDA	*/
  } SCL_DATASET;

/* "scl_rcb_add" allocates this struct, fills it in,		*/
/* and adds it to the linked list "rcbHead" in SCL_LN.		*/
typedef struct
  {
  DBL_LNK l;
  ST_CHAR name[MAX_IDENT_LEN+1];
  ST_CHAR *desc;		/* description (optional)*/
				/* may be long so allocate if present*/
  ST_CHAR datSet[MAX_IDENT_LEN+1];
  ST_UINT intgPd;
  ST_CHAR rptID[MVL61850_MAX_RPTID_LEN+1];
  ST_UINT confRev;
  ST_BOOLEAN buffered;		/* TRUE if this is buffered RCB	*/
  ST_UINT bufTime;
  ST_UINT8 TrgOps [1];		/* 8-bit bitstring			*/
				/* Boolean vals from SCL file		*/
				/* (dchg, qchg, dupd, & period)		*/
				/* used to set bits in TrgOps bitstring	*/
  ST_UINT8 OptFlds [2];		/* 9-bit bitstring			*/
				/* Boolean vals from SCL file		*/
				/* (seqNum, timeStamp, dataSet,		*/
				/* reasonCode, dataRef, bufOvfl,	*/
				/* entryID, configRef)			*/
				/* segmentation boolean is ignored	*/
				/* used to set bits in OptFlds bitstring*/
  ST_UINT maxClient;		/* value of "RptEnabled max" attr.	*/
  } SCL_RCB;			/* Report Control Block	*/

typedef struct
  {
  /* Counters of different types of BRCB.	*/
  ST_UINT brcb_count_complex;
  ST_UINT brcb_count_ST;
  ST_UINT brcb_count_MX;
  ST_UINT brcb_count_CO;
  ST_UINT brcb_count_SP;
  ST_UINT brcb_count_SG;
  ST_UINT brcb_count_SE;
  ST_UINT brcb_count_SV;
  ST_UINT brcb_count_CF;
  ST_UINT brcb_count_DC;
  ST_UINT brcb_count_EX;

  /* Counters of different types of URCB.	*/
  ST_UINT urcb_count_complex;
  ST_UINT urcb_count_ST;
  ST_UINT urcb_count_MX;
  ST_UINT urcb_count_CO;
  ST_UINT urcb_count_SP;
  ST_UINT urcb_count_SG;
  ST_UINT urcb_count_SE;
  ST_UINT urcb_count_SV;
  ST_UINT urcb_count_CF;
  ST_UINT urcb_count_DC;
  ST_UINT urcb_count_EX;
  } SCL_RCB_COUNTERS;

/************************************************************************/
/*			LOG Control Block				*/
/* "scl_lcb_add" allocates this struct, fills it in,			*/
/* and adds it to the linked list "lcbHead" in SCL_LN.			*/
typedef struct
  {
  DBL_LNK l;
  ST_CHAR name[MAX_IDENT_LEN+1];
  ST_CHAR *desc;		/* description (optional)*/
				/* may be long so allocate if present*/
  ST_CHAR datSet[MAX_IDENT_LEN+1];
  ST_UINT intgPd;
  ST_CHAR logName[MAX_IDENT_LEN+1];
  ST_BOOLEAN logEna;
  ST_BOOLEAN reasonCode;
  ST_UINT8 TrgOps [1];		/* 8-bit bitstring			*/
				/* Boolean vals from SCL file		*/
				/* (dchg, qchg, dupd, & period)		*/
				/* used to set bits in TrgOps bitstring	*/
  } SCL_LCB;

/************************************************************************/
/*			GOOSE Control Block				*/
/* "scl_gcb_add" allocates this struct, fills it in,			*/
/* and adds it to the linked list "gcbHead" in SCL_LN.			*/
typedef struct
  {
  DBL_LNK l;
  ST_CHAR name[MAX_IDENT_LEN+1];	/* Name of CB. Used to construct*/
					/* GoCBRef or GsCBRef		*/
  ST_CHAR *desc;			/* description (optional)*/
					/* may be long so allocate if present*/
  ST_CHAR datSet[MAX_IDENT_LEN+1];	/* for GOOSE only	*/
					/* used to construct GOOSE DatSet*/
  ST_UINT confRev;			/* for GOOSE only	*/
  ST_BOOLEAN isGoose;	/* SD_TRUE if "GOOSE", SD_FALSE if "GSSE"*/
  ST_CHAR appID[MVL61850_MAX_RPTID_LEN+1];	/* for GOOSE only	*/
					/* maps to GoID in 61850-7-2	*/
  ST_BOOLEAN subscribed;		/* user subscribed to this GCB	*/
  /* The SCL file may also contain one or more "IEDName" elements to	*/
  /* indicate IEDs that should subscribe for GOOSE data. We have no	*/
  /* way to use this information, so it is ignored.			*/
  } SCL_GCB;

/************************************************************************/
/*			Sampled Value Control Block			*/
/* "scl_parse" allocates this struct and fills it in.			*/
/* "scl_svcb_add" adds it to the linked list "svcbHead" in SCL_LN.	*/
typedef struct
  {
  DBL_LNK l;
  ST_CHAR name[MAX_IDENT_LEN+1];
  ST_CHAR *desc;		/* description (optional)*/
				/* may be long so allocate if present*/
  ST_CHAR datSet[MAX_IDENT_LEN+1];
  /* "smvID" big enough for Edition 2, but only 65 char allowed for Edition 1*/ 
  ST_CHAR smvID[MVL61850_MAX_OBJREF_LEN+1];
  ST_UINT smpRate;
  ST_UINT nofASDU;
  ST_UINT confRev;
  ST_BOOLEAN multicast;		/* TRUE if this is MsvCB		*/
  ST_UINT8 OptFlds [1];		/* 8-bit bitstring			*/
				/* Boolean vals from "SmvOpts" in SCL	*/
				/* (sampleRate, etc.)			*/
				/* used to set bits in this bitstring	*/
  ST_BOOLEAN securityPres;	/* SmvOpts security flag	*/
  ST_BOOLEAN dataRefPres;	/* SmvOpts dataRef flag		*/
  /* For Edition 2 only	*/
  ST_INT8 smpMod;		/* SmpPerPeriod, SmpPerSec, or SecPerSmp*/
			      	/* converted to Enumerated value	*/
  } SCL_SVCB;			/* Sampled Value Control Block	*/

/************************************************************************/
/*			Setting Group Control Block			*/
typedef struct
  {
  /* NOTE: no DBL_LNK here. Only 2 allowed so never put on a linked list.*/
  ST_CHAR *desc;		/* description (optional)		*/
				/* may be long so allocate if present	*/
  ST_UINT numOfSGs;		/* mandatory	*/
  ST_UINT actSG;		/* optional	*/
  } SCL_SGCB;

/************************************************************************/
typedef struct scl_extref
  {
  /* CRITICAL: First 2 parameters used to add this struct to linked	*/
  /* lists using list_add_last, etc.					*/
  struct scl_extref *next;		/* CRITICAL: DON'T MOVE.	*/
  struct scl_extref *prev;		/* CRITICAL: DON'T MOVE.	*/
  /* NOTE: ALL ATTRIBUTES BELOW ARE OPTIONAL.			*/
  /* Pointers are first just to reduce structure padding.	*/
  ST_CHAR *desc;		/* description				*/
				/* may be long so allocate if present	*/
  ST_CHAR *intAddr;		/* internal address			*/
		       		/* may be long so allocate if present	*/
  /* Max lengths of these attributes set by SCL Schema.	*/
  ST_CHAR iedName [MAX_IDENT_LEN+1];
  ST_CHAR ldInst  [MAX_IDENT_LEN+1];
  ST_CHAR prefix  [11+1];
  ST_CHAR lnClass [5+1];
  ST_CHAR lnInst  [12+1];
  ST_CHAR doName  [MAX_IDENT_LEN+1];
  ST_CHAR daName  [MAX_IDENT_LEN+1];
  ST_CHAR serviceType [6+1];
  ST_CHAR srcLDInst  [MAX_IDENT_LEN+1];
  ST_CHAR srcPrefix  [11+1];
  ST_CHAR srcLNClass [5+1];
  ST_CHAR srcLNInst  [12+1];
  ST_CHAR srcCBName  [32+1];
  } SCL_EXTREF;

/************************************************************************/
/*			Logical Node structure 				*/
/* "scl_ln_add" allocates this struct, fills it in,		*/
/* and adds it to the linked list "lnHead" in SCL_LD.		*/
typedef struct
  {
  DBL_LNK l;
  ST_CHAR varName[MAX_IDENT_LEN+1];	/* variable name (constructed)	*/
  ST_CHAR *desc;			/* description (optional)*/
					/* may be long so allocate if present*/
  ST_CHAR lnType[MAX_IDENT_LEN+1];	/* LN Type name		*/
  ST_CHAR lnClass[MAX_IDENT_LEN+1];	/* LN Class name	*/
					/* for LN0, must be "LLN0"	*/
  ST_CHAR inst[MAX_IDENT_LEN+1];	/* LN inst name			*/
					/* for LN0, must be "" (empty string)*/
  ST_CHAR prefix[MAX_IDENT_LEN+1];	/* LN prefix name	*/
					/* for LNO, ignored	*/
  SCL_DAI     *daiHead;		/* head of list of DAI	*/
  SCL_DATASET *datasetHead;	/* head of list of DataSet	*/
  SCL_RCB     *rcbHead;		/* head of list of RCB (Report Control)	*/
  SCL_LCB     *lcbHead;		/* head of list of LCB (Log Control)	*/
  SCL_GCB     *gcbHead;		/* head of list of GCB (GOOSE Control)	*/
  SCL_SVCB    *svcbHead;	/* head of list of SVCB (Sampled Value Control)*/
  SCL_SGCB    *sgcb;		/* SGCB (Setting Group Control)(only 1 allowed)*/
  SCL_EXTREF  *extrefHead;	/* head of list of ExtRef (in Inputs)	*/
  /* NOTE: In LN or LN0: Inputs ignored		*/
  /* NOTE: In LN0: SCLControl ignored		*/

  ST_INT type_id;		/* Initialized by "scl2_datatype_create_all"*/
  MVL_VAR_ASSOC *mvl_var_assoc;	/* MVL Variable Association created from LN info*/
  } SCL_LN;			/* Logical Node (LN or LN0 in SCL)	*/

/************************************************************************/
/*			Logical Device structure 			*/
/* "scl_ld_create" allocates this struct			*/
/* and adds it to the linked list "ldHead" in SCL_INFO.		*/
typedef struct
  {
  DBL_LNK l;
  ST_CHAR domName[MAX_IDENT_LEN+1];	/* domain name (constructed)	*/
  ST_CHAR *desc;			/* description (optional)*/
					/* may be long so allocate if present*/
  ST_CHAR inst[MAX_IDENT_LEN+1];	/* LD inst name		*/
  SCL_LN *lnHead;			/* head of list of LN	*/
  /* NOTE: AccessControl in LDevice is ignored	*/
  } SCL_LD;			/* Logical Device (LDevice in SCL)*/

/************************************************************************/
/* Structures to contain information from "DataTypeTemplates" section of SCL.*/
/************************************************************************/
/* This structure should be allocated and filled in by the function	*/
/* "scl_lntype_add_do".							*/
typedef struct scl_do
  {
  /* CRITICAL: First 2 parameters used to add this struct to linked	*/
  /* lists using list_add_last, etc.					*/
  struct scl_do *next;			/* CRITICAL: DON'T MOVE.	*/
  struct scl_do *prev;			/* CRITICAL: DON'T MOVE.	*/
  ST_CHAR name[MAX_IDENT_LEN+1];	/* data object name		*/
  ST_CHAR type[MAX_IDENT_LEN+1];	/* data object type		*/
  } SCL_DO;

typedef struct scl_lntype
  {
  /* CRITICAL: First 2 parameters used to add this struct to linked	*/
  /* lists using list_add_last, etc.					*/
  struct scl_lntype *next;		/* CRITICAL: DON'T MOVE.	*/
  struct scl_lntype *prev;		/* CRITICAL: DON'T MOVE.	*/
  ST_CHAR id[MAX_IDENT_LEN+1];			/* name used to reference this LN Type*/
  ST_CHAR lnClass[MAX_IDENT_LEN+1];		/* logical node class		*/
  SCL_DO *doHead;		/* head of list of DO	*/
				/* scl_lntype_add_do adds to list	*/
  ST_INT type_id;		/* Initialized by "scl2_datatype_create_all"*/
  } SCL_LNTYPE;


/* This structure should be allocated and filled in by the function	*/
/* "scl_dotype_add_da" OR "scl_dotype_add_sdo", and possibly modified by the optional	*/
/* function "scl_dotype_add_da_val".					*/
/* NOTE: the same structure must be used for DA or SDO because each must	*/
/* be put on the same linked list in the order they are read from the SCL file.*/
/* Most of the parameters are relevant only for DA elements. They are	*/
/* ignored if this is an SDO (i.e. objtype=SCL_OBJTYPE_SDO).		*/
typedef struct
  {
  DBL_LNK l;
  ST_INT objtype;			/* SCL_OBJTYPE_DA or SCL_OBJTYPE_SDO	*/
  ST_CHAR name[MAX_IDENT_LEN+1];	/* DA or SDO name		*/
  ST_CHAR *desc;			/* description (optional)*/
					/* may be long so allocate if present*/
  ST_CHAR sAddr[MAX_IDENT_LEN+1];	/* for DA only: DA sAddr	*/
  ST_CHAR bType[MAX_IDENT_LEN+1];	/* for DA only: DA bType	*/
  ST_CHAR valKind[MAX_VALKIND_LEN+1];	/* for DA only: Spec, Conf, RO, or Set	*/
  ST_CHAR type[MAX_IDENT_LEN+1];	/* for DA: needed if bType="Struct" or "Enum"*/
					/* for SDO: required		*/
  ST_UINT count;			/* for DA only: num array entries*/
  ST_CHAR fc[MAX_FC_LEN+1];		/* for DA only: functional constraint	*/
  ST_BOOLEAN dchg;			/* for DA only: TrgOp (data change)	*/
  ST_BOOLEAN qchg;			/* for DA only: TrgOp (quality change)	*/
  ST_BOOLEAN dupd;			/* for DA only: TrgOp (data update)	*/

  /* The "Val" and "sGroup" parameters are only set if the SCL file contains the
   * optional "Val" element, in which case "scl_dotype_add_da_val" is called.
   */
  ST_CHAR *Val;				/* for DA only: attribute value text	*/
					/* allocate appropriate size buffer*/
  SCL_SG_VAL *sgValHead;		/* for DA only: linked list of	*/
					/* Setting Group initial values	*/
  } SCL_DA;

typedef struct scl_dotype
  {
  /* CRITICAL: First 2 parameters used to add this struct to linked	*/
  /* lists using list_add_last, etc.					*/
  struct scl_dotype *next;		/* CRITICAL: DON'T MOVE.	*/
  struct scl_dotype *prev;		/* CRITICAL: DON'T MOVE.	*/
  ST_CHAR id[MAX_IDENT_LEN+1];	/* name used to reference this DO Type	*/
  ST_CHAR cdc[MAX_CDC_LEN+1];	/* CDC name				*/
  SCL_DA *daHead;		/* head of list of DA or SDO		*/
				/* scl_dotype_add_da OR			*/
				/* scl_dotype_add_sdo adds to list	*/
  } SCL_DOTYPE;

/* This structure should be allocated and filled in by the function	*/
/* "scl_datype_add_bda".						*/
typedef struct
  {
  DBL_LNK l;
  ST_CHAR name[MAX_IDENT_LEN+1];		/* data attribute name		*/
  ST_CHAR *desc;			/* description (optional)*/
					/* may be long so allocate if present*/
  ST_CHAR sAddr[MAX_IDENT_LEN+1];		/* for DA only: DA sAddr	*/
  ST_CHAR bType[MAX_IDENT_LEN+1];		/* data attribute type		*/
  ST_CHAR valKind[MAX_VALKIND_LEN+1];		/* Spec, Conf, RO, or Set	*/
  ST_CHAR type[MAX_IDENT_LEN+1];		/* only used if btype="Struct" or "Enum"*/
  ST_UINT count;	/* for DA only: num array entries*/

  /* The "Val" and "sGroup" parameters are only set if the SCL file contains the
   * optional "Val" element, in which case "scl_datype_add_bda_val" is called.
   */
  ST_CHAR *Val;				/* attribute value text		*/
					/* allocate appropriate size buffer*/
  SCL_SG_VAL *sgValHead;		/* linked list of Setting Group	*/
					/* initial values		*/
  } SCL_BDA;			/* Basic Data Attribute		*/
typedef struct
  {
  DBL_LNK l;
  ST_CHAR id[MAX_IDENT_LEN+1];			/* name used to reference this DA Type*/
  SCL_BDA *bdaHead;		/* head of list of BDA	*/
				/* scl_datype_add_bda adds to list	*/
  } SCL_DATYPE;


/* This structure should be allocated and filled in by the function	*/
/* "scl_enumtype_add_enumval".						*/
typedef struct
  {
  DBL_LNK l;
  ST_INT ord;				/* ord attribute	*/
  /* If string fits in EnumValBuf, it is copied there & EnumVal is set	*/
  /* to point to it. Else, EnumVal is allocated & string is copied to it.*/
  ST_CHAR *EnumVal;			/* EnumVal pointer		*/
  ST_CHAR EnumValBuf[MAX_IDENT_LEN+1];	/* EnumVal buffer		*/
  } SCL_ENUMVAL;
typedef struct
  {
  DBL_LNK l;
  ST_CHAR id[MAX_IDENT_LEN+1];			/* name used to reference this DA Type*/
  SCL_ENUMVAL *enumvalHead;	/* head of list of EnumVal	*/
				/* scl_enumtype_add_enumval adds to list*/
  } SCL_ENUMTYPE;


/************************************************************************/
/*			SCL_SERV_CFG					*/
/* Used only when parseMode == SCL_PARSE_MODE_SCD_FILTERED.		*/
/************************************************************************/
typedef struct
  {
  ST_CHAR iedName [MAX_IDENT_LEN+1];	/* IED name to find		*/
  ST_CHAR apName  [MAX_IDENT_LEN+1];	/* AccessPoint name to find	*/
  } SCL_SERV_CFG;		/* Config info for one Server		*/

/************************************************************************/
/*			SCL_REPORTSETTINGS				*/
/* Data from "ReportSettings" in "Services" element.			*/
/* Only one, so linked list pointers are not included.			*/
/************************************************************************/
typedef struct
  {
  /* These may be "Dyn", "Conf", or "Fix". No other values allowed.	*/
  ST_CHAR cbName [4+1];
  ST_CHAR datSet [4+1];
  ST_CHAR rptID [4+1];
  ST_CHAR optFields [4+1];
  ST_CHAR bufTime [4+1];
  ST_CHAR trgOps [4+1];
  ST_CHAR intgPd [4+1];
  ST_BOOLEAN resvTms;
  ST_BOOLEAN owner; /* proposed in Tissue 807 */
  } SCL_REPORTSETTINGS;

/************************************************************************/
/*			SCL_SERVICE_WITH_MAX				*/
/************************************************************************/
typedef struct
  {
  ST_BOOLEAN enabled;
  ST_UINT max;
  } SCL_SERVICE_WITH_MAX;

/************************************************************************/
/*			SCL_SERVICES					*/
/* Only one, so linked list pointers are not included.			*/
/************************************************************************/
typedef struct
  {
  /* Simple entries just map to booleans.	*/
  ST_BOOLEAN GetDirectory;
  ST_BOOLEAN GetDataObjectDefinition;
  ST_BOOLEAN DataObjectDirectory;
  ST_BOOLEAN GetDataSetValue;
  ST_BOOLEAN SetDataSetValue;
  ST_BOOLEAN DataSetDirectory;
  ST_BOOLEAN ReadWrite;
  ST_BOOLEAN TimerActivatedControl;
  ST_BOOLEAN GetCBValues;
  ST_BOOLEAN GSEDir;
  ST_BOOLEAN FileHandling;
  ST_BOOLEAN ConfLdName;
  /* More complicated entries need special mappings.	*/
  SCL_SERVICE_WITH_MAX ConfLogControl;
  SCL_SERVICE_WITH_MAX GOOSE;
  SCL_SERVICE_WITH_MAX GSSE;
  SCL_SERVICE_WITH_MAX SMVsc;
  SCL_SERVICE_WITH_MAX SupSubscription;
  SCL_SERVICE_WITH_MAX ConfSigRef;

  SCL_REPORTSETTINGS ReportSettings;
  } SCL_SERVICES;


/************************************************************************/
/*			SCL_SERV_OPT					*/
/* Options for a Server (NOT configured by SCL).			*/
/************************************************************************/
typedef struct
  {
  /* Report configuration parameters.		*/
  ST_UINT reportScanRateMs;	/* Report scan rate (millisec)		*/
  ST_INT brcb_bufsize;		/* BRCB buffer size			*/
  /* Log configuration parameters.		*/
  ST_UINT logScanRateMs;	/* Log scan rate (millisec)		*/
  ST_UINT logMaxEntries;	/* Max number of Log entries allowed	*/
  } SCL_SERV_OPT;

/************************************************************************/
/*			SCL_SERVER					*/
/************************************************************************/
typedef struct scl_server
  {
  /* CRITICAL: First 2 parameters used to add this struct to linked	*/
  /* lists using list_add_last, etc.					*/
  struct scl_server *next;		/* CRITICAL: DON'T MOVE.	*/
  struct scl_server *prev;		/* CRITICAL: DON'T MOVE.	*/
  SCL_SERV_OPT serv_opt;	/* options not configured by SCL	*/
  SCL_LD *ldHead;		/* head of list of LDevice defs		*/
				/* for this Server			*/
  ST_CHAR iedName [MAX_IDENT_LEN+1];	/* IED name for this Server	*/
  ST_CHAR apName  [MAX_IDENT_LEN+1];	/* AccessPoint name for this Server*/
  MVL_VMD_CTRL *vmd_ctrl;	/* VMD created for this Server		*/
  SCL_SERVICES scl_services;	/* Info from "Services" section of SCL	*/
  } SCL_SERVER;

/************************************************************************/
/************************************************************************/
typedef struct
  {
  ST_INT forceEdition;		/* 0 = use edition detected in SCL file	*/
				/* 1 = force Edition 1 parsing		*/
				/* 2 = force Edition 2 parsing		*/
  /* NOTE: "includeOwner" should NOT be used if Tissue 807 is approved.	*/
  /*       It should only be used as an alternative way to control	*/
  /*       inclusion of "Owner" until Tissue 807 is resolved.		*/
  ST_BOOLEAN includeOwner;	/* control inclusion of "Owner" in RCBs.*/
  } SCL_OPTIONS;

/************************************************************************/
/*			SCL_INFO					*/
/* This structure contains all information extracted from the SCL file	*/
/* to be used for MMS-EASE Lite initialization.				*/
/************************************************************************/
typedef struct
  {
  ST_INT edition;		/* 0 (default) means Edition 1		*/
			      	/* 2 means Edition 2			*/
			      	/* Other editions not supported.	*/
  SCL_OPTIONS options;		/* parser options passed by user	*/

  SCL_HEADER Header;		/* Info from "Header" section of SCL file*/

  /* SubNetwork definitions from (from Communication section)		*/
  SCL_SUBNET *subnetHead;	/* head of list of SubNetwork defs	*/

  /* Logical Node Type definitions (from DataTypeTemplates section)	*/
  SCL_LNTYPE *lnTypeHead;	/* head of list	of LNodeType defs	*/
  SCL_DOTYPE *doTypeHead;	/* head of list of DOType defs		*/
  SCL_DATYPE *daTypeHead;	/* head of list of DAType defs		*/
  SCL_ENUMTYPE *enumTypeHead;	/* head of list of EnumType defs	*/

  /* Server definitions (from "Server" section)				*/
  SCL_SERVER *serverHead;	/* head of list of Server defs		*/

  ST_BOOLEAN datatype_create_done;	/* flag set by scl2_datatype_create_all*/
  ST_BOOLEAN ld_create_done;	/* flag set by scl2_ld_create_all*/

  /* Parameters below used only when pareMode==SCL_PARSE_MODE_SCD_FILTERED.*/
  SCL_SERV_CFG *scl_serv_cfg_arr;	/* array of Servers to configure*/
  ST_UINT       scl_serv_cfg_num;	/* number of Servers in array	*/
  ST_CHAR **scl_iedtype_cfg_arr;	/* array of iedTypes to configure*/
  ST_UINT     scl_iedtype_cfg_num;	/* number of iedTypes in array	*/
  } SCL_INFO;

/************************************************************************/
/*			SCL_DEC_CTRL					*/
/* This structure contains data saved during the parsing process.	*/
/************************************************************************/
typedef struct scl_dec_ctrl
  {
  ST_CHAR *iedName;
  ST_CHAR *accessPointName;
  ST_BOOLEAN accessPointFound;	/* SD_TRUE if IED and AccessPoint found	*/
  ST_BOOLEAN iedNameMatched;	
  ST_BOOLEAN accessPointMatched;
  SCL_INFO *sclInfo;
  SCL_GSE *scl_gse;	/* Used for "GSE" in "Communication" section	*/
  SCL_SMV *scl_smv;	/* Used for "SMV" in "Communication" section	*/
  SCL_LD *scl_ld;	/* Used for "LDevice"				*/
  SCL_LN *scl_ln;	/* Used for "LN" (Logical Node)			*/
  SCL_RCB *scl_rcb;	/* alloc to store ReportControl info		*/
  SCL_LCB *scl_lcb;	/* alloc to store LogControl info		*/
  ST_UINT8 TrgOps[1];	/* Used for ReportControl or LogControl.	*/
			/* Copied to SCL_RCB or SCL_LCB.		*/
  SCL_SVCB *scl_svcb;	/* Used for "SampledValueControl".	*/
  SCL_ENUMVAL *scl_enumval;	/* Used for "EnumVal".			*/
  SCL_DAI *scl_dai;	/* Used for "DAI".				*/
  SCL_DA *scl_da;	/* Used for "DA".				*/
  SCL_BDA *scl_bda;	/* Used for "BDA".				*/
  ST_CHAR flattened[MAX_FLAT_LEN + 1];	/* Created by concatenating values*/
					/* from DOI, SDI, and DAI elements*/
  ST_UINT sGroupTmp;	/* temporary "sGroup" value: set when element	*/
			/* start proc'd, used when element end proc'd.	*/
  /* Parameters below used for special parsing modes.			*/
  ST_INT parseMode;		/* one of "SCL_PARSE_MODE_*" defines.	*/
				/* Set by main parse functions.		*/
  ST_BOOLEAN iedTypeMatched;	/* SD_TRUE if iedType matches one requested*/
				/* used for LNodeType, DOType, DAType	*/
  ST_CHAR iedNameProc [MAX_IDENT_LEN+1]; /* iedName being processed	*/

  SCL_SERVICES scl_services;	/* Info from "Services" section		*/
				/* Copied to SCL_SERVER when created.	*/
  } SCL_DEC_CTRL;

/************************************************************************/
/************************************************************************/
/* FUNCTIONS to store SCL info in "SCL_INFO" structure.			*/
/************************************************************************/
/************************************************************************/

SCL_LNTYPE *scl_lntype_create (
	SCL_INFO *scl_info);

SCL_DO *scl_lntype_add_do (
	SCL_INFO *scl_info);

SCL_DOTYPE *scl_dotype_create (
	SCL_INFO *scl_info);

SCL_DA *scl_dotype_add_da (
	SCL_INFO *scl_info);

SCL_DA *scl_dotype_add_sdo (
	SCL_INFO *scl_info);

SCL_DATYPE *scl_datype_create (
	SCL_INFO *scl_info);

SCL_BDA *scl_datype_add_bda (
	SCL_INFO *scl_info);

SCL_ENUMTYPE *scl_enumtype_create (
	SCL_INFO *scl_info);

SCL_ENUMVAL *scl_enumtype_add_enumval (
	SCL_INFO *scl_info);

SCL_FCDA *scl_fcda_add (
	SCL_INFO *scl_info);

SCL_DAI *scl_dai_add (
	SCL_INFO *scl_info);

SCL_DATASET *scl_dataset_add (
	SCL_INFO *scl_info);

SCL_RCB *scl_rcb_add (
	SCL_INFO *scl_info);

SCL_LCB *scl_lcb_add (
	SCL_INFO *scl_info);

SCL_GCB *scl_gcb_add (
	SCL_INFO *scl_info);

SCL_SGCB *scl_sgcb_add (
	SCL_INFO *scl_info);

SCL_SVCB *scl_svcb_add (
	SCL_INFO *scl_info);

SCL_LN *scl_ln_add (
	SCL_INFO *scl_info);

SCL_LD *scl_ld_create (
	SCL_INFO *scl_info);

SCL_SUBNET *scl_subnet_add (
	SCL_INFO *scl_info);

SCL_CAP *scl_cap_add (
	SCL_INFO *scl_info);

SCL_GSE *scl_gse_add (
	SCL_INFO *scl_info);

SCL_SMV *scl_smv_add (
	SCL_INFO *scl_info);

SCL_SG_VAL *scl_dai_sg_val_add (
	SCL_DAI *scl_dai);	/* ptr to current DAI element	*/

SCL_SG_VAL *scl_da_sg_val_add (
	SCL_DA *scl_da);	/* ptr to current DA element	*/

SCL_SG_VAL *scl_bda_sg_val_add (
	SCL_BDA *scl_bda);	/* ptr to current BDA element	*/

SCL_EXTREF *scl_extref_add (
	SCL_INFO *scl_info);

SCL_SERVER *scl_server_add (
	SCL_INFO *scl_info);

/************************************************************************/
/*			scl_parse					*/
/* Parses SCL file and stores extracted info in SCL_INFO structure.	*/
/************************************************************************/
ST_RET scl_parse (ST_CHAR *xmlFileName, ST_CHAR *iedName, 
		  ST_CHAR *accessPointName, SCL_INFO *sclInfo);
ST_RET scl_parse_cid (ST_CHAR *xmlFileName,
	ST_CHAR *iedName, 
	ST_CHAR *accessPointName,
	SCL_OPTIONS *options,	/* miscellaneous parser options		*/
				/* may be NULL if no options needed	*/
	SCL_INFO *sclInfo);	/* main struct where all SCL info stored*/
ST_VOID scl_log_all (SCL_INFO *scl_info);

/************************************************************************/
/*			scl_info_destroy				*/
/* Destroy all info stored in the SCL_INFO structure by "scl_parse".	*/
/************************************************************************/
ST_VOID scl_info_destroy (SCL_INFO *scl_info);

ST_VOID scl_ld_destroy (SCL_LD *scl_ld);	/* destroy just one LD	*/

/************************************************************************/
/*			scl2_datatype_create_all			*/
/* Create MMS Data types for all Logical Node Types (LNodeType)		*/
/* defined in SCL.							*/
/* RETURNS:	SD_SUCCESS or error code				*/
/************************************************************************/
ST_RET scl2_datatype_create_all (
	MVL_VMD_CTRL *vmd_ctrl,	/* VMD in which to add types.		*/
	SCL_INFO *sclInfo,	/* main struct where all SCL info stored*/
	ST_INT max_rt_num,	/* max num of RUNTIME_TYPE for each LNodeType*/
	ST_BOOLEAN use_names,	/* if SD_TRUE, generate a name for each type*/
	ST_CHAR *name_prefix);	/* unique prefix to add to each type name*/
				/* only used if "use_names==SD_TRUE".	*/
ST_INT scl2_datatype_count_scd (
	SCL_INFO *scl_info);	/* main struct where all SCL info stored*/

/************************************************************************/
/*			scl2_ld_create_all			*/
/* Create all Logical Devices from SCL info saved in "sclInfo".		*/
/* RETURNS:	SD_SUCCESS or error code				*/
/************************************************************************/
ST_RET scl2_ld_create_all (
	MVL_VMD_CTRL *vmd_ctrl,	/* VMD in which to add Logical Devices.	*/
	SCL_INFO *sclInfo,	/* main struct where all SCL info stored*/
	ST_UINT reportScanRate,	/* report scan rate (millisec)	*/
	ST_INT brcb_bufsize,	/* BRCB buffer size			*/
	ST_BOOLEAN is_client);	/* If this flag is set, Client model is	*/
				/* created (i.e. Control Blocks NOT created).*/

/************************************************************************/
/*			u_set_all_leaf_functions			*/
/* User customizable function to set all leaf functions for a data type.*/
/************************************************************************/
ST_RET u_set_all_leaf_functions (RUNTIME_CTRL *rt_ctrl,
	SCL_LNTYPE *scl_lntype);	/* LNodeType info from SCL file.*/


/************************************************************************/
/************************************************************************/

extern ST_BOOLEAN scl_debug_mode;
extern ST_INT scl_debug_mode_error_count;

typedef struct
  {
  DBL_LNK l;
  ST_CHAR *btype;
  RUNTIME_CTRL *rt_ctrl;
  } SCL2_BTYPE;
extern SCL2_BTYPE *scl2_btype_list;
SCL2_BTYPE *scl2_add_btype (ST_CHAR *btype, ST_CHAR *tdl);



typedef struct
  {
  SCL_INFO *scl_info;
  SCL_LD *scl_ld;
  SCL_LN *scl_ln;
  ST_CHAR *valText;
  ST_CHAR *attrib;
  ST_VOID *dest;
  ST_INT numRt;
  SD_CONST RUNTIME_TYPE *rtHead;
  } SCL2_IV_TRANSLATE_CTRL;

ST_RET u_mvl_scl_set_initial_value (SCL2_IV_TRANSLATE_CTRL *sclXlateIv);

/************************************************************************/
/* General purpose functions to get attributes from XML.		*/
/************************************************************************/
ST_RET scl_get_attr_ptr (SX_DEC_CTRL *sxDecCtrl, ST_CHAR *name,
	ST_CHAR **value, ST_BOOLEAN required);
ST_RET scl_get_attr_copy (SX_DEC_CTRL *sxDecCtrl, ST_CHAR *name,
	ST_CHAR *value, ST_UINT maxValueLen, ST_BOOLEAN required);
ST_RET scl_get_int_attr (SX_DEC_CTRL *sxDecCtrl, ST_CHAR *name,
	ST_INT *value, ST_BOOLEAN required);
ST_RET scl_get_uint_attr (SX_DEC_CTRL *sxDecCtrl, ST_CHAR *name,
	ST_UINT *value, ST_BOOLEAN required);

ST_VOID scl_stop_parsing (SX_DEC_CTRL *sxDecCtrl, ST_RET errCode);

/************************************************************************/
/* Functions for parsing "Services" element (see sclparse_serv.c).	*/
/************************************************************************/
ST_VOID scl_services_init (SCL_SERVICES *scl_services);
ST_VOID Services_SEFun (SX_DEC_CTRL *sxDecCtrl);

/************************************************************************/
/* Functions to find objects in SCL_INFO.				*/
/************************************************************************/
SCL_GSE *scl_gse_find (SCL_INFO *scl_info,
	ST_CHAR *iedName,	/* IED of GSE address		*/
	ST_CHAR *apName,	/* AccessPoint of GSE address	*/
	ST_CHAR *ldInst,	/* ldInst of GSE address	*/
	ST_CHAR *cbName);	/* cbName of GSE address	*/

SCL_ADDRESS *scl_address_find (SCL_INFO *scl_info,
	ST_CHAR *iedName,	/* IED for which to find address	*/
	ST_CHAR *apName);	/* AccessPoint for which to find address*/

SCL_SERVER *scl_server_find (SCL_INFO *scl_info,
	ST_CHAR *iedName,
	ST_CHAR *apName);

SCL_SMV *scl_smv_find (SCL_INFO *scl_info,
	ST_CHAR *iedName,	/* IED of SMV address		*/
	ST_CHAR *apName,	/* AccessPoint of SMV address	*/
	ST_CHAR *ldInst,	/* ldInst of SMV address	*/
	ST_CHAR *cbName);	/* cbName of SMV address	*/

SCL_GCB *scl_gcb_find (SCL_SERVER *scl_server,
	ST_CHAR *ldInst,	/* LDevice inst from SCL	*/
	ST_CHAR *lnName,	/* Logical Node name		*/
				/* Must be LLN0			*/
	ST_CHAR *gcbName,	/* GCB name			*/
	SCL_LD **ldOut,		/* LD where CB found		*/
	SCL_LN **lnOut);	/* LN where CB found		*/

SCL_SVCB *scl_svcb_find (SCL_SERVER *scl_server,
	ST_CHAR *ldInst,	/* LDevice inst from SCL	*/
	ST_CHAR *lnName,	/* Logical Node name		*/
				/* Must be LLN0			*/
	ST_CHAR *svcbName,	/* SVCB name			*/
	SCL_LD **ldOut,		/* LD where CB found		*/
	SCL_LN **lnOut);	/* LN where CB found		*/


/************************************************************************/
/* Functions for SCD parse mode.					*/
/************************************************************************/
ST_RET scl_parse_scd_all (ST_CHAR *xmlFileName,
	SCL_OPTIONS *options,	/* miscellaneous parser options		*/
				/* may be NULL if no options needed	*/
	SCL_INFO *sclInfo);	/* main struct where all SCL info stored*/

ST_RET scl_parse_scd_filtered (ST_CHAR *xmlFileName,
	SCL_OPTIONS *options,	/* miscellaneous parser options		*/
				/* may be NULL if no options needed	*/
	SCL_INFO *sclInfo,	/* main struct where all SCL info stored*/
	SCL_SERV_CFG *scl_serv_cfg_arr,	/* array of Servers to configure*/
	ST_UINT scl_serv_cfg_num,	/* number of Servers in array	*/
	ST_CHAR **scl_iedtype_cfg_arr,	/* array of iedTypes to configure*/
	ST_UINT scl_iedtype_cfg_num);	/* number of iedTypes in array	*/

ST_RET scl2_ld_create_all_scd (
	MVL_VMD_CTRL *vmd_ctrl,	/* VMD in which to add Logical Devices.	*/
	SCL_INFO *scl_info,	/* main struct where all SCL info stored*/
	SCL_SERVER *scl_server,	/* Server info used to create all MMS objects*/
	SCL_SERV_OPT *serv_opt,	/* Options for this Server		*/
	ST_BOOLEAN is_client);	/* If this flag is set, Client model is	*/
				/* created (i.e. Control Blocks NOT created).*/

MVL_VMD_CTRL **scl2_vmd_create_all (
	SCL_INFO *scl_info,	/* SCL info from parser		*/
	ST_CHAR *iedName,	/* IED name of Server		*/
				/* If NULL, no Server configured*/
	ST_CHAR *apName,	/* AccessPoint name of Server	*/
				/* If NULL, no Server configured*/
	SCL_SERV_OPT *serv_opt,	/* Server options		*/
	ST_UINT *vmd_count);	/* OUT: number of VMDs created	*/
				/* (i.e. size of array returned)*/

/************************************************************************/
/* Miscellaneous functions						*/
/************************************************************************/
/* Call this after ALL objects created by "scl2_*" functions.		*/
/* It frees the "reserved_1" member of all RUNTIME_CTRL structures.	*/
ST_VOID scl2_reserved_free_all (MVL_VMD_CTRL *vmd_ctrl);

ST_INT scl_dib_entry_save (SCL_INFO *scl_info);	/* see "scl_dib.c"	*/

#ifdef __cplusplus
}
#endif

#endif	/* !SCL_INCLUDED	*/
