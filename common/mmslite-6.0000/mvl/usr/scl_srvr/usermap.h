#ifndef USERMAP_INCLUDED
#define USERMAP_INCLUDED
/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	2004-2009 All Rights Reserved					*/
/*									*/
/* MODULE NAME : usermap.h						*/
/* PRODUCT(S)  : MMS-EASE-LITE						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*	Data mapping structures and functions used by SCL Server	*/
/*	sample application.						*/
/*									*/
/* CUSTOMIZATION:							*/
/*	Defining USERMAP_CUSTOM disables the default DATA_MAP definition*/
/*	and the default versions of the functions "datamap_leaf",	*/
/*	"datamap_find_leaf", and "datamap_save_line". If it is defined,	*/
/*	you must provide "usermap_custom.h" containing the DATA_MAP	*/
/*	definition, and customized versions of these functions.		*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev     Comments					*/
/* --------  ---  ------   -------------------------------------------  */
/* 03/02/12  JRB	   Increase "leaf" size to MAX_FLAT_LEN to avoid*/
/*			   potential buffer overflow in "usermap.c".	*/
/* 04/25/11  JRB	   Add "*CheckRange".				*/
/* 06/02/10  JRB	   Repl IEC_61850_SERVER.. with USERMAP_CUSTOM.	*/
/*			   Add datamap_save_line (customizable).	*/
/*			   Add datamap_bsearch.				*/
/* 06/02/09  JRB    12     Add DATA_MAP_LD struct; chg functs to use it.*/
/*			   Moved 'ldevice' from DATA_MAP struct to	*/
/*			   DATA_MAP_HEAD (avoids many duplicates).	*/
/*			   Del any_cfg_count.				*/
/* 12/22/08  JRB    11     DATA_MAP: add deadband_info.			*/
/*			   Add set_rpt_reason.				*/
/*			   Add datamap_leaf proto (customizable)	*/
/*			   & delete datamap_user_leaf (don't need both).*/
/*			   Add datamap_find_leaf.			*/
/*			   Del datamap_leaf_opc_client, def where used.	*/
/* 04/08/08  JRB    10     Chg () to (ST_VOID) in prototype.		*/
/* 07/10/07  CRM    09     Added opcInitialValue to DATA_MAP typedef	*/
/* 04/16/07  CRM    08     Added "writable" flag to DATA_MAP typedef.	*/
/* 03/05/07  JRB    07     Chg opcItemName from array to ptr.		*/
/* 03/20/06  JRB    06     Del all_obj_create/destroy (now in scl_srvr.c)*/
/* 09/22/05  JRB    05     Del opc protos & opcServerInfoHead.		*/
/* 07/13/05  JRB    04     Chg datamap_cfg_read args.			*/
/*			   Add datamap_cfg_destroy.			*/
/*			   Add all_obj_create/destroy.			*/
/*			   Add datamap_user_oper, datamap_user_leaf.	*/
/* 03/17/05  JRB    03     Add ifdef __cplusplus to work with C++ code.	*/
/*			   Cleanup IEC_61850_SER.. sections.		*/
/* 01/18/05  JRB    02     Add opcGroup for IEC_61850_SER..		*/
/*			   Add TRACK_INFO & datamap_leaf_opc_client.	*/
/* 08/05/04  JRB    01     Initial Revision.				*/
/************************************************************************/
#include "scl.h"	/* need SCL_INFO	*/
#include "mvl_uca.h"	/* need MVLU_RD_VA_CTRL, etc.	*/

#ifdef __cplusplus
extern "C" {
#endif

#if defined(USERMAP_CUSTOM)

#include "usermap_custom.h"	/* define "customized" DATA_MAP struct here*/

#else	/* !defined(USERMAP_CUSTOM)	*/

/************************************************************************/
/*			DATA_MAP structure				*/
/* This structure is used to store each entry from a data mapping	*/
/* configuration file.							*/
/* Each structure represents 1 line from the file.			*/
/************************************************************************/
typedef struct
  {
  /* NOTE: "ldevice" deleted. Get "ldevice" from DATA_MAP_HEAD.		*/
  ST_CHAR leaf [MAX_FLAT_LEN+1];	/* "leaf" name		*/
  /* This is a user-defined parameter to help the user to map MMS data	*/
  /* to real user data.  This may be replaced with any			*/
  /* parameter that may be useful for mapping data.			*/
  /* In this example it is just a string read from the mapping file.	*/
  ST_CHAR usr_data_info [MAX_IDENT_LEN+1];

  ST_VOID *dataPtr;	/* Pointer to data storage for this	*/
			/* leaf. Also used as unique handle	*/
			/* for sorting/finding			*/
  RUNTIME_TYPE *dataType;	/* data type of this leaf	*/

  ST_VOID *deadband_info;	/* deadband info for this attr		*/
				/* NULL if this attr not deadbanded	*/
				/* Points to DEADBAND_INT32 or		*/
				/* DEADBAND_FLOAT struct (cast as needed)*/
  } DATA_MAP;
#endif	/* !defined(USERMAP_CUSTOM)	*/

/************************************************************************/
/*			DATA_MAP_HEAD structure				*/
/************************************************************************/
typedef struct
  {
  DATA_MAP **map_arr;	/* Array of ptrs to DATA_MAP structures	*/
			/* allocated and initialized when cfg file read	*/
  ST_UINT map_count;	/* num of entries in array		*/
  ST_CHAR ldevice [MAX_IDENT_LEN+1];	/* Logical Device name	*/
  } DATA_MAP_HEAD;

/************************************************************************/
/*			DATA_MAP_LD structure				*/
/* Holds mappings for just one Logical Device.				*/
/************************************************************************/
typedef struct _data_map_ld
  {
  /* CRITICAL: First 2 parameters used to add this struct to linked	*/
  /* lists using list_add_last, etc.					*/
  struct _data_map_ld *next;	/* CRITICAL: DON'T MOVE.	*/
  struct _data_map_ld *prev;	/* CRITICAL: DON'T MOVE.	*/
  ST_CHAR ldevice [MAX_IDENT_LEN+1];	/* Logical Device name	*/
  DATA_MAP **map_arr;	/* Array of ptrs to DATA_MAP structures	*/
			/* allocated and initialized when cfg file read	*/
  ST_UINT map_count;	/* num of entries in array		*/
  ST_UINT tmp_count;	/* temporary count of mappings (used	*/
			/* only while filling in mapping array)	*/
  } DATA_MAP_LD;

#define MAX_NEST_LEVEL	50

/************************************************************************/
/*			TRACK_INFO structure				*/
/************************************************************************/
typedef struct
  {
  ST_UINT primdata_num;	/* # of primitive data elements in variable	*/
			/* must account for arrays of structs.	*/
  ST_INT data_offset;
  ST_INT str_nest;			/* current struct nest level	*/
  /* rt_type of start of structs containing this leaf. First entry is 	*/
  /* top level struct, next is lower level struct, etc.			*/
  RUNTIME_TYPE *rt_type_str_start [MAX_NEST_LEVEL];
  ST_INT data_offset_str_start [MAX_NEST_LEVEL];
  } TRACK_INFO;

/************************************************************************/
/*			datamap_cfg_read				*/
/* Reads data mapping configuration file & maps the data for all	*/
/* variables in the global VMD "mvl_vmd".				*/
/* RETURNS:	SD_SUCCESS or error code				*/
/************************************************************************/
ST_RET datamap_cfg_read (
	ST_CHAR *in_filename,		/* input cfg file name	*/
	ST_CHAR *out_filename);		/* output cfg file name	*/

/************************************************************************/
/*			datamap_cfg_destroy				*/
/* Unmap all data mapped by datamap_cfg_read and free associated buffers.*/
/* CRITITAL: must be called BEFORE variables removed, or it will be	*/
/*   impossible to remove the mapping.					*/
/************************************************************************/
ST_VOID datamap_cfg_destroy (ST_VOID);

/************************************************************************/
/*			datamap_leaf					*/
/* CUSTOMIZABLE: Sample function provided but can easily be replaced	*/
/*               with user customized function.				*/
/************************************************************************/
ST_RET datamap_leaf (
	DATA_MAP_LD *map_ld,	/* Logical Device mappings to set	*/
	MVL_VAR_ASSOC *var,	/* Variable (Logical Node) info		*/
	ST_CHAR *dom_name,	/* Name of domain (Logical Device)	*/
				/* containing this variable (Logical Node)*/
	ST_CHAR *branch_name,
	ST_CHAR *comp_name,
	RUNTIME_TYPE *rt_type,	/* type of this "leaf"			*/
	TRACK_INFO *track_info,
	FILE *fp_out_map);

/************************************************************************/
/*			datamap_find_leaf				*/
/* CUSTOMIZABLE: Sample function provided but can easily be replaced	*/
/*               with user customized function.				*/
/************************************************************************/
DATA_MAP *datamap_find_leaf (
	DATA_MAP_LD *map_ld,	/* Logical Device mappings to search	*/
	ST_CHAR *dom_name,	/* Name of domain (Logical Device)	*/
				/* containing this variable (Logical Node)*/
	ST_CHAR *leaf_name,	/* "Flattened" leaf name		*/
	ST_CHAR *comp_name);	/* Name of this component		*/

/************************************************************************/
/*			datamap_save_line				*/
/* CUSTOMIZABLE: Sample function provided but can easily be replaced	*/
/*               with user customized function.				*/
/************************************************************************/
ST_RET datamap_save_line (
	ST_CHAR *datamap_cfg_filename,	/* filename for log messages	*/
	ST_INT line_num,	/* current line num for log messages	*/
	ST_CHAR *in_buf,	/* current line text for log messages	*/
	DATA_MAP *map_entry,    /* save data in this struct		*/
	ST_CHAR *curptr,	/* ptr to current pos in token buffer	*/
	ST_CHAR *seps);		/* list of separator chars		*/

/************************************************************************/
/* If usermap.c is compiled with DATAMAP_USER_ENABLED defined, these	*/
/* user functions are called to do extra mapping or initialization.	*/
/************************************************************************/
ST_VOID datamap_user_oper (ST_CHAR *dom_name, ST_CHAR *oper_name);

/************************************************************************/
/* Set the IEC 61850 Report reason. May be called from any leaf function*/
/************************************************************************/
ST_VOID set_rpt_reason (MVLU_RD_VA_CTRL *mvluRdVaCtrl);

/************************************************************************/
/* This function finds a (DATA_MAP *) in a sorted array of (DATA_MAP *).*/
/************************************************************************/
DATA_MAP *datamap_bsearch (
	DATA_MAP **map_arr,		/* array of objects to search	*/
	ST_UINT map_count,		/* num elem in array		*/
	ST_CHAR *leaf_name);		/* leaf name to search for	*/

/* Functions to check range of Enumerated values.	*/
ST_RET hvRefCheckRange      (ST_INT8 value);
ST_RET cmdQualCheckRange    (ST_INT8 value);
ST_RET orCatCheckRange      (ST_INT8 value);
ST_RET SIUnitCheckRange     (ST_INT8 value);
ST_RET multiplierCheckRange (ST_INT8 value);
ST_RET angRefCheckRange     (ST_INT8 value);
ST_RET angRefWYECheckRange  (ST_INT8 value);
ST_RET phsRefCheckRange     (ST_INT8 value);
ST_RET setCharactCheckRange (ST_INT8 value);

#ifdef __cplusplus
}
#endif

#endif	/* !USERMAP_INCLUDED	*/
