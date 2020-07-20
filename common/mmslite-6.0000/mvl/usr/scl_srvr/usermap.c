/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	2004-2009 All Rights Reserved					*/
/*									*/
/* MODULE NAME : usermap.c						*/
/* PRODUCT(S)  : MMS-EASE-LITE						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*	Functions to map from MMS "leafs" to user data.			*/
/*									*/
/* NOTE: If USERMAP_CUSTOM is defined, user must supply customized	*/
/*       versions of these functions (preferably in a separate file):	*/
/*			datamap_save_line				*/
/*			datamap_find_leaf				*/
/*			datamap_leaf					*/
/*									*/
/* NOTE: compile with "DATAMAP_USER_ENABLE" defined to cause extra user	*/
/*       functions to be called.					*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*			datamap_cfg_read				*/
/*			datamap_cfg_destroy				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev     Comments					*/
/* --------  ---  ------   -------------------------------------------  */
/* 03/02/12  JRB	   Fix Klocwork warnings.			*/
/* 03/31/11  JRB	   datamap_cleanup: free map_ld_array when done.*/
/*			   Fail if leaf name or user string too long.	*/
/* 06/02/10  JRB	   Move all customized code to other files.	*/
/*			   Use "USERMAP_CUSTOM" to enable customizing.	*/
/* 06/02/09  JRB    23     Chg functions to use new DATA_MAP_LD struct.	*/
/*			   Replace any_cfg_count with datamap_cfg_alloc.*/
/*			   Use bsearch to find (DATA_MAP *) (much faster*/
/*			   for big files).				*/
/*			   Moved 'ldevice' from DATA_MAP struct to	*/
/*			   DATA_MAP_HEAD (avoids many duplicates).	*/
/* 01/05/09  JRB    22     datamap_leaf: OVERHAUL to automatically set	*/
/*			   map_entry->deadband_info. Allow "customized"	*/
/*			   datamap_leaf & del datamap_user_leaf.	*/
/*			   Move more ..OPC_CLIENT code to separate file.*/
/* 10/29/07  JRB    21     strncat_safe obsolete, use strncat_maxstrlen.*/
/* 08/06/07  JRB    20     Allow only comma or tab as delimiters.	*/
/*			   Check for empty tokens.			*/
/* 07/20/07  JRB    19     Generate opcInitialValue if missing in input.*/
/* 07/12/07  CRM    18     opcItemName parameter MUST be present	*/
/*			   (if not known, must enter "").		*/
/* 07/10/07  CRM    17     Added opcInitialValue to input/output.	*/
/*			   Column 7 (type) now required in input.	*/
/* 06/19/07  JRB    16     Ignore illegal mappings in input.		*/
/* 04/30/07  JRB    15     Fix to work with RUNTIME_TYPE change.	*/
/* 03/29/07  CRM    14     Added quotes to opcmapout.cfg dummy entries. */
/* 03/05/07  JRB    13     Allow longer lines in input.			*/
/*			   Don't map Beh$stVal to data, it is computed.	*/
/*			   Chg opcItemName to ptr, alloc before using.	*/
/* 09/09/05  JRB    12     Don't assert (just log) if any var is not struct.*/
/* 07/22/05  JRB    11     Recognize RCB leafs by looking for $RP$ or	*/
/*			   $BR$ (looking for RP$urcb, BR$brcb didn't work).*/
/* 07/13/05  JRB    10     If DATAMAP_USER_ENABLE defined, call user fcts*/
/*			   datamap_user_leaf, datamap_user_oper.	*/
/* 07/13/05  JRB    09     datamap_cfg_read: Del (SCL_INFO *) arg	*/
/*			    (allows mapping vars NOT configured by SCL);*/
/*			    del (DATA_MAP_HEAD *) arg & use local var.	*/
/*			   Add datamap_cfg_destroy funct.		*/
/*			   Alloc and copy DATA_MAP structs so unneeded	*/
/*			    structs can be freed in datamap_cfg_read.	*/
/* 03/30/05  JRB    08     Chg ERR log to FLOW log if leaf not mapped.	*/
/* 03/15/05  JRB    07     Replace strtok with get_next_string to handle*/
/*			   extra delimiters and quoted strings.		*/
/*			   Add quotes around strings in output cfg file.*/
/*			   Add "type" column in output cfg file.	*/
/* 02/15/05  JRB    06     Use new generated scl_ld->domName, not	*/
/*			   scl_ld->inst as domain name.			*/
/* 01/19/05  JRB    05     Move most IEC_61850_SERVER_OPC_CLIENT code	*/
/*			    to new module, call datamap_leaf_opc_client.*/
/*			   For IEC_61850_SER.., allow missing "ItemName"*/
/*			   Move TRACK_INFO to usermap.h.		*/
/*			   Chg rt_type_str_start & data_offset_str_start*/
/*			    in TRACK_INFO to arrays to save multiple	*/
/*			    nesting levels.				*/
/*			   Use mvlu_trim_branch_name from mvl_uca.c	*/
/*			   Fix data_offset calc, do all in datamap_loop.*/
/* 08/05/04  JRB    04     Use new user header file.			*/
/* 07/23/04  JRB    03     Fixed logs. Shortened static function names.	*/
/* 07/21/04  JRB    02     Add out_filename arg to datamap_cfg_read.	*/
/*			   Add #if (IEC_61850_SERVER_OPC_CLIENT)	*/
/* 06/08/04  JRB    01     Initial Revision.				*/
/************************************************************************/
#include "glbtypes.h"
#include "sysincs.h"
#include "scl.h"	/* for SCL_INFO, SCL_LD, etc.	*/
#include "usermap.h"	/* for DATA_MAP, etc.	*/
#include "sx_log.h"
#include "mvl_defs.h"
#include "mvl_uca.h"
#include "str_util.h"	/* for strn..._safe	protos	*/
#include "db_61850.h"	/* "deadband" structures & functions	*/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

#define LINE_BUF_LEN	1024	/* max len of line read from file.	*/

#if 0
/************************************************************************/
/* DEBUG: if special user mapping is required, DATAMAP_USER_ENABLED	*/
/* must be defined and the user must write the functions		*/
/* "datamap_user_oper" to do user mapping.				*/
/* These functions may be placed here or in any other module.		*/
/************************************************************************/
#define DATAMAP_USER_ENABLE
/* Example of "datamap_user_oper" function (does nothing).	*/
ST_VOID datamap_user_oper (ST_CHAR *dom_name, ST_CHAR *oper_name)
  {
  }
#endif

/* CRITICAL: the "primdata_num" element of the TRACK_INFO struct
 * is used in this module as the index into the array of (DATA_MAP *).
 * The same value must also be passed to the leaf functions so they can
 * find the right (DATA_MAP *).
 * NOTE: dataPtr passed to the leaf function should ALWAYS 
 *       match the dataPtr in the DATA_MAP struct.
 */

/************************************************************************/
/* Static functions.							*/
/************************************************************************/
static ST_RET datamap_loop (
	DATA_MAP_LD *map_ld,	/* Logical Device mappings to search	*/
				/* NOTE: NULL if this LD has no mappings*/
	ST_CHAR *dom_name,	/* Name of domain (Logical Device)	*/
				/* containing this variable (Logical Node)*/
	MVL_VAR_ASSOC *var,	/* Variable (Logical Node) info		*/
	RUNTIME_TYPE *rt_first,	
	ST_INT rt_num,
	RUNTIME_TYPE *rt_type_last,	/* to make sure don't go past end of RUNTIME_TYPE arr*/
	TRACK_INFO *track_info,
	ST_CHAR *branch_name,
	ST_BOOLEAN array_flag,	/* SD_TRUE if this type is in array	*/
	FILE *fp_out_map);
static ST_VOID datamap_loop_count_prim (
	RUNTIME_TYPE *rt_first,	
	ST_INT rt_num,
	TRACK_INFO *track_info);

/* Callback function used by "qsort" & "bsearch".	*/
static int datamap_compare (const void *obj1, const void *obj2);

/************************************************************************/
/*			datamap_cfg_alloc				*/
/* For each Logical Device (LD), count the number of "useful" lines in	*/
/* the data mapping CFG file.						*/
/* NOTE: Ignore empty lines or lines beginning with '#'.		*/
/* Create linked list of Logical Device (LD) mapping structures. Each	*/
/* structure will contain all mappings for that LD.			*/
/* Then copy linked list to array (easier to access).			*/
/* RETURNS: pointer to array of pointers, NULL on error.		*/
/************************************************************************/
DATA_MAP_LD **datamap_cfg_alloc (ST_CHAR *in_filename,
	ST_UINT *p_ld_count)	/* out: number of LD found in file	*/
  {
FILE *in_fp;
ST_CHAR in_buf[LINE_BUF_LEN];	/* buffer for one line read from file	*/
ST_CHAR *curptr;		/* ptr to current position in token_buf	*/
char seps[] = ",\t\n";
ST_CHAR *ldevice;		/* first token on line	*/
DATA_MAP_LD *map_ld_list;	/* linked list of ptrs to LD mappings	*/
DATA_MAP_LD *map_ld;		/* mappings for one LD		*/
DATA_MAP_LD **map_ld_array;	/* array of ptrs to LD mappings	*/
				/* created from array		*/
ST_UINT ld_count = 0;		/* # of different LD found in input file*/
ST_UINT ld_idx;			/* array index	*/

  map_ld_list = NULL;		/* start with empty list	*/
  map_ld_array = NULL;		/* init. Normally allocated later	*/

  in_fp = fopen (in_filename, "r");
  if (in_fp == NULL)
    {
    SXLOG_ERR1 ("Error opening input file '%s'", in_filename);
    *p_ld_count = 0;
    return (NULL);
    }

  /* Read one line at a time from the input file	*/
  while (fgets (in_buf, sizeof(in_buf) - 1, in_fp) != NULL)
    {
    /* If string is too close to max len, we probably did not read	*/
    /* complete line. DO NOT allow that.				*/
    if (strlen (in_buf) > (sizeof (in_buf) - 3))
      {
      /* If this happens, increase value of LINE_BUF_LEN.		*/
      SXLOG_ERR2 ("Line exceeds maximum length '%d' in '%s'. Cannot parse.",
                  LINE_BUF_LEN, in_filename);
      /* Cleanup and break out of loop.	*/
      while ((map_ld = (DATA_MAP_LD *) list_get_first (&map_ld_list)) != NULL)
        chk_free (map_ld);
      ld_count = 0;	/* array won't be allocated & NULL will be returned*/
      break;
      }
    /* NOTE: get_next_string modifies the input buffer.			*/
    /*       Here we're just counting lines, so that is OK.		*/
    curptr = in_buf;	/* init "curptr"	*/
    ldevice = get_next_string(&curptr, seps);	/* "curptr" adjusted by this funct	*/

    /* If NULL, this is empty line. If first char is '#', this is comment line.*/
    if (ldevice == NULL || ldevice[0] == '#')
      continue;		/* Ignore empty lines & comment lines	*/
    if (ldevice [0] == '\0')
      continue;	/* First token is empty. This is probably empty line. Ignore it.*/

    /* If "ldevice" matches one on list, use it. Else, alloc & add to list.	*/
    for (map_ld = map_ld_list;
         map_ld != NULL;
         map_ld = (DATA_MAP_LD *) list_get_next (map_ld_list, map_ld))
      {
      if (strcmp (map_ld->ldevice, ldevice) == 0)
        {
        break;	/* map_ld points to struct found. */
        }
      }
    if (!map_ld)	/* struct NOT found	*/
      {
      /* Allocate new entry for this "ldevice" and add to list.*/
      map_ld = chk_calloc (1, sizeof (DATA_MAP_LD));
      list_add_last (&map_ld_list, map_ld);
      strcpy (map_ld->ldevice, ldevice);
      ld_count++;
      }
    /* Either found list entry or created new entry.	*/
    assert (map_ld != NULL);
    map_ld->tmp_count++;	/* incr num entries for this LD	*/
    }	/* end "while (fgets..)"	*/

  /* Convert Linked List to array, then use array from here on.	*/
  /* DEBUG: Could use "qsort" and "bsearch" on this array, but number	*/
  /*        of Logical Devices is usually small (maybe later).		*/
  if (ld_count)
    {
    /* Allocate array of ptrs, copy each list entry to array.		*/
    map_ld_array = (DATA_MAP_LD **) chk_malloc (ld_count*sizeof(DATA_MAP_LD *));
    ld_idx = 0;
    for (map_ld = map_ld_list;
         map_ld != NULL;
         map_ld = (DATA_MAP_LD *) list_get_next (map_ld_list, map_ld))
      {
      map_ld_array[ld_idx] = map_ld;
      ld_idx++;
      }
    assert (ld_idx==ld_count);
    }

  /* Loop through array and allocate "map_arr" for each LD.	*/
  for (ld_idx = 0; ld_idx < ld_count; ld_idx++)
    {
    ST_UINT map_idx;	/* inner loop counter	*/
    map_ld = map_ld_array[ld_idx];
    map_ld->map_count = map_ld->tmp_count;	/* save count	*/
    /* Alloc array of DATA_MAP pointers. */
    /* NOTE: need array of pointers to allow sorting later with "qsort".*/
    map_ld->map_arr = chk_calloc (map_ld->map_count, sizeof (DATA_MAP *));
    for (map_idx = 0; map_idx < map_ld->map_count; map_idx++)
      {
      map_ld->map_arr[map_idx] = chk_calloc (1, sizeof (DATA_MAP));
      }
    }

  fclose (in_fp);

  *p_ld_count = ld_count;
  return (map_ld_array);
  }

/************************************************************************/
/*			datamap_cfg_rd2					*/
/* Read each line in the "Mapping Cfg" file. Copy data from one line	*/
/* to one entry in the "map_arr" array in a DATA_MAP_LD structure.	*/
/* Find the correct DATA_MAP_LD structure by searching the 'map_ld_array'*/
/* for a matching "ldevice".						*/
/* When done reading, sort the "map_arr" array in each DATA_MAP_LD.	*/
/* NOTE: Ignore empty lines or lines beginning with '#'.		*/
/************************************************************************/
static ST_RET datamap_cfg_rd2 (ST_CHAR *datamap_cfg_filename,
	DATA_MAP_LD **map_ld_array,	/* array of ptrs to LD mappings	*/
	ST_UINT ld_count)		/* num array elements		*/
  {
FILE *in_fp;
ST_CHAR in_buf[LINE_BUF_LEN];	/* buffer for one line read from file	*/
ST_CHAR token_buf[LINE_BUF_LEN];	/* copy of "in_buf". Modified by get_next_string.*/
ST_CHAR *curptr;		/* ptr to current position in token_buf	*/
char seps[] = ",\t\n";
DATA_MAP *map_entry;
ST_INT line_num;		/* number of lines in file	*/
ST_UINT entry_num;		/* number of valid entries in file	*/
ST_RET retcode = SD_SUCCESS;
ST_CHAR *ldevice;		/* first token on line	*/
ST_CHAR *leaf;			/* 2nd token	*/
DATA_MAP_LD *map_ld;
ST_UINT ld_idx;			/* current LD array index	*/

  in_fp = fopen (datamap_cfg_filename, "r");
  if (in_fp == NULL)
    {
    SXLOG_ERR1 ("Error opening input file '%s'", datamap_cfg_filename);
    return (SD_FAILURE);
    }

  /* CRITICAL: reset "tmp_count" for each LD before 2nd pass.	*/
  for (ld_idx = 0; ld_idx < ld_count; ld_idx++)
    {
    map_ld_array[ld_idx]->tmp_count = 0;
    }

  /* Read one line at a time from the input file	*/
  /* NOTE: length of every line checked on first pass, so no need to recheck here.*/
  entry_num = 0;	
  line_num = 0;
  while (fgets (in_buf, sizeof(in_buf) - 1, in_fp) != NULL)
    {
    line_num++;

    /* Make a copy of input buffer to be used by "get_next_string", which will
     * modify the copied buffer (token_buf). Keep input buffer (in_buf)
     * intact.
     * NOTE: could use "strtok" here, but it doesn't nicely handle extra
     *       delimiters or quoted strings. 
     */
    strcpy (token_buf, in_buf);
    curptr = token_buf;	/* init "curptr"	*/
    ldevice = get_next_string(&curptr, seps);	/* "curptr" adjusted by this funct	*/

    /* If NULL, this is empty line. If first char is '#', this is comment line.*/
    if (ldevice == NULL || ldevice[0] == '#')
      continue;		/* Ignore empty lines & comment lines	*/
    if (ldevice [0] == '\0')
      {
      /* First token is empty. This is probably empty line.	*/
      /* Ignore this line, but log error if more tokens found. 	*/
      if ((leaf = get_next_string (&curptr, seps)) != NULL)
        SXLOG_ERR3 ("Input ignored because first token is empty at line %d in '%s'. Second token='%s'",
                    line_num, datamap_cfg_filename, leaf);
      continue;
      }

    /* Find map_ld with same "ldevice" as this leaf.		*/
    /* Set "map_entry" to next available ptr in the array.	*/
    map_entry = NULL;
    for (ld_idx = 0; ld_idx < ld_count; ld_idx++)
      {
      map_ld = map_ld_array[ld_idx];
      if (strcmp (map_ld->ldevice, ldevice) == 0)
        {
        /* Set map_entry to next available ptr in the array.	*/
        map_entry = map_ld->map_arr[map_ld->tmp_count];
        map_ld->tmp_count++;	/* increment counter	*/
        break;
        }
      }
    if (map_entry==NULL)
      {
      SXLOG_ERR2 ("LD='%s' found only on 2nd pass reading '%s'. File changed?",
                 ldevice, datamap_cfg_filename);
      retcode = SD_FAILURE;
      break;	/* break out of main loop	*/
      }

    /* Save data from this line in "map_entry".	*/
    retcode = datamap_save_line (
              datamap_cfg_filename,
              line_num,
              in_buf,
              map_entry,	/* save data in this struct	*/
              curptr,
              seps);

    /* If ANYTHING failed so far, stop looping.	*/
    if (retcode)
      break;	/* get out of loop	*/
    entry_num++;
    }	/* end main "while" loop	*/

  /* Now, for each LD, sort all entries alphabetically by leaf name.	*/
  if (retcode == SD_SUCCESS)
    {
    for (ld_idx = 0; ld_idx < ld_count; ld_idx++)
      {
      map_ld = map_ld_array[ld_idx];
      /* Both counts must match (i.e. we counted the same both times).	*/
      if (map_ld->tmp_count != map_ld->map_count)
        {	/* should only happen if file modified between passes*/
        SXLOG_ERR2 ("Entry count for LD='%s' changed on 2nd pass reading '%s'",
                  map_ld->ldevice, datamap_cfg_filename);
        retcode = SD_FAILURE;
        break;	/* stop on first error	*/
        }

      qsort (map_ld->map_arr,
           map_ld->map_count,
           sizeof (DATA_MAP *),
           datamap_compare);	/* compares leaf name*/
      }
    }

  /* Log if successful. Otherwise, error already logged.	*/
  if (retcode == SD_SUCCESS)
    SXLOG_FLOW3 ("Done reading file '%s', %d lines, %d valid entries",
                 datamap_cfg_filename, line_num, entry_num);

  fclose (in_fp);
  return (retcode);
  }

#if !defined(USERMAP_CUSTOM)	/* CUSTOMIZABLE FUNCTIONS		*/
/* If USERMAP_CUSTOM is defined, customized versions of these functions	*/
/* must be supplied in a separate module.				*/

/************************************************************************/
/*			datamap_save_line				*/
/* Save data from one line of the file in a DATA_MAP structure.		*/
/************************************************************************/
ST_RET datamap_save_line (
	ST_CHAR *datamap_cfg_filename,	/* filename for log messages	*/
	ST_INT line_num,	/* current line num for log messages	*/
	ST_CHAR *in_buf,	/* current line text for log messages	*/
	DATA_MAP *map_entry,    /* save data in this struct		*/
	ST_CHAR *curptr,	/* ptr to current pos in token buffer	*/
	ST_CHAR *seps)		/* list of separator chars		*/
  {
ST_CHAR *leaf;
ST_CHAR *token3;
ST_RET retcode = SD_SUCCESS;  

  /* NOTE: if one get_next_string returns NULL, subsequent calls should also.*/
  leaf   = get_next_string (&curptr, seps);
  token3 = get_next_string (&curptr, seps);
  /* NOTE: If more columns of input are needed, add a call to	*/
  /*       "get_next_string" for each additional column.	*/

  if (leaf && token3)
    {
    /* Fail if "leaf name" too long.	*/
    if (strlen(leaf) < sizeof(map_entry->leaf))
      strcpy (map_entry->leaf, leaf);
    else 
      {
      SXLOG_ERR3 ("Leaf name '%s' too long at line %d in '%s'.",
                  leaf, line_num, datamap_cfg_filename);
      retcode = SD_FAILURE;
      }

    /* Fail if "user string" too long.	*/
    if (strlen(token3) < sizeof(map_entry->usr_data_info))
      strcpy (map_entry->usr_data_info, token3);
    else 
      {
      SXLOG_ERR3 ("User string '%s' too long at line %d in '%s'.",
                  token3, line_num, datamap_cfg_filename);
      retcode = SD_FAILURE;
      }
    }
  else
    {
    SXLOG_ERR2 ("Invalid input at line %d in '%s'. Must contain 3 parameters.", line_num, datamap_cfg_filename);
    SXLOG_CERR1 ("%s", in_buf);
    retcode = SD_FAILURE;
    }
  return (retcode);
  }

/************************************************************************/
/*			datamap_find_leaf				*/
/* Find the mapping (DATA_MAP *) for this leaf.				*/
/************************************************************************/
DATA_MAP *datamap_find_leaf (
	DATA_MAP_LD *map_ld,	/* Logical Device mappings to search	*/
				/* NOTE: NULL if this LD has no mappings*/
	ST_CHAR *dom_name,	/* Name of domain (Logical Device)	*/
				/* containing this variable (Logical Node)*/
	ST_CHAR *leaf_name,	/* "Flattened" leaf name		*/
	ST_CHAR *comp_name)	/* Name of this component		*/
  {
DATA_MAP *map_entry = NULL;	/* assume mapping NOT found		*/

  assert (comp_name != NULL);

  /* map_ld points to LD we're currently mapping.	*/
  /* If map_ld==NULL, there are no mappings for this LD, so skip search.*/
  if (map_ld)
    map_entry = datamap_bsearch (map_ld->map_arr, map_ld->map_count,
                leaf_name);
  return (map_entry);
  }

/************************************************************************/
/*			datamap_leaf					*/
/* Sets the "dataPtr", "dataType"					*/
/* parameters in JUST ONE entry of the "map_ld->map_arr".		*/ 
/* RETURNS:	SD_SUCCESS or SD_FAILURE				*/
/************************************************************************/
ST_RET datamap_leaf (
	DATA_MAP_LD *map_ld,	/* Logical Device mappings to search	*/
				/* NOTE: NULL if this LD has no mappings*/
	MVL_VAR_ASSOC *var,	/* Variable (Logical Node) info		*/
	ST_CHAR *dom_name,	/* Name of domain (Logical Device)	*/
				/* containing this variable (Logical Node)*/
	ST_CHAR *branch_name,
	ST_CHAR *comp_name,
	RUNTIME_TYPE *rt_type,	/* type of this "leaf"			*/
	TRACK_INFO *track_info,
	FILE *fp_out_map)
  {
DATA_MAP_HEAD *map_head_sorted;	/* head of array of (DATA_MAP *) for this var.*/
DATA_MAP *map_entry;	/* map entry for this leaf	*/
ST_CHAR leaf_name[MAX_FLAT_LEN+1];	/* build flattened leaf name here*/
ST_CHAR tdl_buf[80];			/* TDL for type of this leaf	*/
ST_INT  tdl_len;			/* len of TDL			*/
RUNTIME_TYPE tmp_rt_type;
DATA_MAP tmp_map_entry; /* temporary entry. Copied to map_head_sorted	*/
ST_VOID *deadband_info;	/* deadband info for this leaf (NULL if not found)*/
ST_RET retcode;  

  assert (comp_name);	/* assume "comp_name" is NEVER NULL	*/
  if (strlen (comp_name))
    {	/* not empty, so append to branch name	*/
    strcpy (leaf_name, branch_name);
    /* New "strncat_maxstrlen" returns error if string is truncated.	*/
    strncat_maxstrlen (leaf_name, "$",       MAX_FLAT_LEN);
    retcode = strncat_maxstrlen (leaf_name, comp_name, MAX_FLAT_LEN);
    if (retcode)
      {
      SXLOG_ERR1 ("flattened name '%s' was truncated", leaf_name);
      return (SD_FAILURE);
      }
    }
  else
    {   /* comp_name is empty, so don't append to branch_name	*/
    strcpy (leaf_name, branch_name);
    }

  /* Get TDL for this type for writing to output file.
   * Don't want component name in the TDL, so just copy rt_type to temp struct
   * and set comp_name_ptr = NULL.
   * Always simple type so 2nd arg to "ms_runtime_to_tdl" is always 1.
   */ 
  memcpy (&tmp_rt_type, rt_type, sizeof (RUNTIME_TYPE));
  tmp_rt_type.comp_name_ptr = NULL;
  tdl_len = ms_runtime_to_tdl (&tmp_rt_type, 1, tdl_buf, sizeof(tdl_buf)-1);
  if (tdl_len == 0)	/* make sure tdl_buf set if error occurred	*/
    sprintf (tdl_buf, "UNKNOWN_TYPE(tag=%d, len=%d)",rt_type->el_tag, rt_type->u.p.el_len);

  map_head_sorted = (DATA_MAP_HEAD *) var->user_info;

  /* Use dom_name, leaf_name to find the map_entry.		*/
  map_entry = datamap_find_leaf (map_ld, dom_name, leaf_name, comp_name);

  /* If this attribute requires deadband calculations, it SHOULD NOT be	*/
  /* mapped (i.e. map_entry==NULL). Save deadband info in static	*/
  /* temporary entry.							*/
  /* NOTE: No need to alloc because copied later to map_head_sorted.	*/
  deadband_info = deadband_create (var, leaf_name, rt_type);
  if (deadband_info)
    {	/* this attribute requires deadband calc	*/
    if (map_entry == NULL)
      {
      /* GOOD. Leaf SHOULD NOT be mapped and it isn't.			*/
      /* Point to temporary entry & save deadband_info there.		*/
      /* Don't care about other elements. Just save deadband_info.	*/
      /* NOTE: deadband_info freed in datamap_cfg_var_cleanup.		*/
      map_entry = &tmp_map_entry;	/* copied later so tmp is fine*/
      memset (map_entry, 0, sizeof (DATA_MAP));	/* start clean	*/
      /* Save leaf_name for logging, debugging, etc.	*/
      strcpy (map_entry->leaf, leaf_name);
      map_entry->deadband_info = deadband_info;	/* save db info here	*/
      SXLOG_FLOW2 ("Leaf name '%s' in domain '%s': deadband parameters automatically mapped",
                   leaf_name, dom_name);
      }
    else
      {
      /* BAD. Leaf SHOULD NOT be mapped but it is.			*/
      /* Log error and ignore deadband (i.e. cfg overrides auto deadband).*/
      /* NOTE: if you want the automatic deadband config to override 	*/
      /*       the input config file, remove this code			*/
      /*       and save deadband_info in map_entry.			*/
      SXLOG_ERR2 ("ERROR: Leaf name '%s' in domain '%s' SHOULD NOT BE MAPPED.\n  Disables automatic deadband calculation.",
                  leaf_name, dom_name);
      chk_free (deadband_info);
      deadband_info = NULL;	/* Don't use deadband info.	*/
      }
    }

  /* Now write to output cfg file "mapped" entry or "dummy" entry	*/
  /* but only if this leaf DOES NOT contain deadband_info & leaf_name	*/
  /* DOES NOT contain "$Beh$stVal", etc.				*/
  /* These should NEVER be mapped.					*/
  if (deadband_info == NULL			/* does not contain deadband*/
      &&  strstr (leaf_name, "$Beh$stVal") == NULL	/* does not contain "$Beh$stVal"*/
      &&  strstr (leaf_name, "$RP$") == NULL	/* does not contain "$RP$"*/
      &&  strstr (leaf_name, "$BR$") == NULL)	/* does not contain "$BR$"*/
    {    
    if (map_entry)
      {		/* Mapped. Copy entry to output file.	*/
      if (fp_out_map)
        fprintf (fp_out_map, "%s\t%s\t\"%s\"\ttype=%s\n", 
               map_ld->ldevice,
               map_entry->leaf,
               map_entry->usr_data_info,
               tdl_buf);
      }
    else
      {		/* Not Mapped. Write dummy entry to output file.	*/
      SXLOG_FLOW2 ("Leaf name '%s' in domain '%s' not found in data mapping configuration file",
                 leaf_name, dom_name);
      /* Write dummy "Mapping Cfg" entry to output file.	*/
      /* Put "" around dummy string to match format of  real entry.	*/
      if (fp_out_map)
        fprintf (fp_out_map, "%s\t%s\t\"dummy_data_mapping_string\"\ttype=%s\n", 
               dom_name, leaf_name,
               tdl_buf);
      }
    }

  if (map_entry != NULL)
    {				/* this leaf is mapped	*/
    /* Set pointer to data storage for this leaf.	*/
    map_entry->dataPtr = (ST_CHAR *) var->data + track_info->data_offset;
    map_entry->dataType = rt_type;	/* Save type	*/

    /* NOTE: map_entry is completely filled in now.
     * Save map_entry to appropriate array entry.
     * Use primdata_num as index into array.
     * NOTE: array initialized to all NULL, so only need to save if map_entry!=NULL.
     * CRITICAL: the value of "primdata_num" used here as index to array
     * must be passed to "leaf" function. The "leaf" function needs it
     * to find this map_entry.
     * CRITICAL: The DATA_MAP struct is allocated and copied so that
     *   everything allocated when the file was read can be freed when
     *   config is complete (see datamap_cleanup), but the sorted DATA_MAP_HEAD
     *   struct created here (i.e. map_head_sorted) is still valid and usable.
     */
    map_head_sorted->map_arr [track_info->primdata_num] = chk_malloc (sizeof (DATA_MAP));
    memcpy (map_head_sorted->map_arr [track_info->primdata_num], map_entry, sizeof (DATA_MAP));
    }

  /* CRITICAL: update primdata_num (ONLY here).	*/
  track_info->primdata_num++;
  return (SD_SUCCESS);
  }
#endif	/* !defined(USERMAP_CUSTOM)	*/

/************************************************************************/
/*  				datamap_array				*/
/* WARNING: datamap_array & datamap_loop may call each other		*/
/*	recursively.							*/
/* Add text for array, like "...$array[0]$etc".				*/
/* NOTE: because array index is included in mapping text, there is no	*/
/* need for "var->arrCtrl".						*/
/************************************************************************/
static ST_RET datamap_array (
	DATA_MAP_LD *map_ld,	/* Logical Device mappings to search	*/
				/* NOTE: NULL if this LD has no mappings*/
	ST_CHAR *dom_name,	/* Name of domain (Logical Device)	*/
				/* containing this variable (Logical Node)*/
	MVL_VAR_ASSOC *var,	/* Variable (Logical Node) info		*/
	RUNTIME_TYPE *rt_type,
	RUNTIME_TYPE *rt_type_last,	/* to make sure don't go past end of RUNTIME_TYPE arr*/
	TRACK_INFO *track_info,
	ST_CHAR *branch_name,	/* array name without index		*/
	FILE *fp_out_map)
  {
ST_INT j;
ST_INT rt_num;
ST_INT num_arr_elements;
ST_CHAR tmp_branch_name[MAX_FLAT_LEN+1];	/* copy branch_name here & add index*/
ST_RET retcode = SD_FAILURE;
ST_CHAR *comp_name;

  comp_name = ms_comp_name_find (rt_type);
  /* ifdef USE_RT_TYPE_2, ms_comp_name_find macro NEVER returns NULL.	*/
  assert (comp_name != NULL);

  rt_num = rt_type->u.arr.num_rt_blks;	/* num RUNTIME_TYPE to define one element of array*/

  num_arr_elements = rt_type->u.arr.num_elmnts;	/* num of elements in array		*/

  assert (rt_type->el_tag == RT_ARR_START);

  /* Make sure room for 10 digit integer value in sprintf of tmp_branch_name.*/
  if (strlen(branch_name) > (MAX_FLAT_LEN-12))
    {
    SXLOG_ERR1 ("flattened name too long to add more: %s", branch_name);
    /* retcode already set = SD_FAILURE	*/
    }
  else
    {
    for (j = 0; j < num_arr_elements; ++j)
      {
      /* Create temporary branch_name for this array element.	*/
      sprintf (tmp_branch_name, "%s[%d]", branch_name, j);
      assert (strlen(tmp_branch_name) <= MAX_FLAT_LEN);	/* should never fail with chk above*/
      /* Recursive call to "datamap_loop".	*/
      retcode = datamap_loop (map_ld, dom_name, var, rt_type+1, rt_num,
                  rt_type_last, track_info, tmp_branch_name, SD_TRUE, fp_out_map);
      if (retcode)
        break;	/* error, stop looping	*/
      }
    }

  /* NOTE: don't do anything for RT_ARR_END here. Handled in datamap_loop.*/

  return (retcode);
  }
/************************************************************************/
/*			datamap_loop					*/
/* WARNING: datamap_array & datamap_loop may call each other		*/
/*	recursively.							*/
/* RETURNS:	SD_SUCCESS or error code				*/
/* NOTE: usually (track_info->data_offset == rt_type->mvluTypeInfo.offSet)*/
/*       but not when inside an array.					*/
/************************************************************************/
static ST_RET datamap_loop (
	DATA_MAP_LD *map_ld,	/* Logical Device mappings to search	*/
				/* NOTE: NULL if this LD has no mappings*/
	ST_CHAR *dom_name,	/* Name of domain (Logical Device)	*/
				/* containing this variable (Logical Node)*/
	MVL_VAR_ASSOC *var,	/* Variable (Logical Node) info		*/
	RUNTIME_TYPE *rt_first,	
	ST_INT rt_num,
	RUNTIME_TYPE *rt_type_last,	/* to make sure don't go past end of RUNTIME_TYPE arr*/
	TRACK_INFO *track_info,
	ST_CHAR *branch_name,
	ST_BOOLEAN array_flag,	/* SD_TRUE if this type is in array	*/
	FILE *fp_out_map)
  {
ST_RTINT num_rt_blks;
ST_CHAR *comp_name;
RUNTIME_TYPE *rt_type;		/* ptr to current array entry.	*/
ST_INT rt_index;		/* index into array of RUNTIME_TYPE	*/
ST_RET retcode = SD_SUCCESS;

  /* Loop through array of RUNTIME_TYPE structs.			*/
  /* NOTE: On RT_ARR_START, rt_index & rt_type get extra incrementing.	*/
  for (rt_index = 0, rt_type = rt_first; rt_index < rt_num; ++rt_index, ++rt_type)
    {
    assert (rt_type < rt_type_last);

    if (!array_flag)
      {	/* must have been called from "datamap_ln" (not datamap_array)	*/
      /* Make sure data_offset computed correctly	*/
      if (track_info->data_offset != rt_type->mvluTypeInfo.offSet)
        {
        SXLOG_ERR2 ("data_offset=%d does not match expected (%d)",
                    track_info->data_offset, rt_type->mvluTypeInfo.offSet);
        retcode = SD_FAILURE;	/* give up	*/
        break;
        }
      }

    comp_name = ms_comp_name_find (rt_type);
    /* ifdef USE_RT_TYPE_2, ms_comp_name_find macro NEVER returns NULL.	*/
    assert (comp_name != NULL);
    
    switch (rt_type->el_tag)
      {
      case RT_STR_START:
        if (track_info->str_nest >= MAX_NEST_LEVEL)
          {
          SXLOG_ERR1 ("Structure nesting exceeds max %d. Can't continue processing.", MAX_NEST_LEVEL);
          retcode = SD_FAILURE;	/* give up	*/
          break;
          }
        track_info->rt_type_str_start[track_info->str_nest] = rt_type;	/* save this ptr (lowest level struct found)*/
        track_info->data_offset_str_start[track_info->str_nest] = track_info->data_offset;
        track_info->str_nest++;

        if (strlen (comp_name))
          {
          if (array_flag && rt_index == 0)
            {	/* this struct is first elem of array. DO NOT add comp_name*/
            }
          else
            {
            strncat_maxstrlen (branch_name, "$", MAX_FLAT_LEN);
            strncat_maxstrlen (branch_name, comp_name, MAX_FLAT_LEN);
            }
          }
#if defined(DATAMAP_USER_ENABLE)
        /* If this struct is "Oper", call extra user function to perform special mapping or initialization.*/
        if (strcmp (comp_name, "Oper") == 0) 
          datamap_user_oper (dom_name, branch_name);	/* user function	*/
#endif
        break;
      case RT_STR_END:
        track_info->str_nest--;		/* decrement nest level	*/
        track_info->rt_type_str_start[track_info->str_nest] = NULL;	/* clear ptr*/
        track_info->data_offset_str_start[track_info->str_nest] = 0;	/* clear offset*/

        assert (strlen(comp_name)==0);	/* Shound NEVER have component name*/
        mvlu_trim_branch_name (branch_name);
        break;
      case RT_ARR_END:
        assert (0);		/* should never get here. Code below for*/
				/* RT_ARR_START skips past this RUNTIME_TYPE.	*/
        break;
      case RT_ARR_START:
        assert (strlen(comp_name));	/* MUST have component name*/
        strncat_maxstrlen (branch_name, "$", MAX_FLAT_LEN);
        strncat_maxstrlen (branch_name, comp_name, MAX_FLAT_LEN);
        /* Recursive call to "datamap_array".	*/
        datamap_array (map_ld, dom_name, var, rt_type,
                         rt_type_last, track_info, branch_name, fp_out_map);
        mvlu_trim_branch_name (branch_name);	/* remove comp_name just added	*/

        track_info->data_offset += rt_type->el_size;	/* SPECIAL CASE: do this before incrementing rt_type*/

        /* Increment to point at RT_ARR_END	*/
        num_rt_blks = rt_type->u.arr.num_rt_blks;
        rt_index += (num_rt_blks + 1);
        rt_type += (num_rt_blks + 1);
        assert (rt_type->el_tag==RT_ARR_END);
        break;
      default:			/* Primitive	*/
        if (rt_num == 1)
          {   /* this prim is only element in the RT table.	*/
          /* don't want datamap_leaf to add comp_name, so overwrite	*/
          /* it with empty string.					*/
          comp_name = "";
          }
        retcode = datamap_leaf (
                  map_ld,
                  var,
                  dom_name,		/* Name of domain (Logical Device)	*/
					/* containing this variable (Logical Node)*/
                  branch_name,
                  comp_name,
                  rt_type,		/* type of this "leaf"			*/
                  track_info,
                  fp_out_map);
        /* NOTE: if this fails, retcode chk below will break out of "for" loop.*/
        break;
      }	/* end "switch"	*/
    if (retcode)
      break;		/* error so stop looping	*/

    /* Update data_offset for any rt_type. Notice the SPECIAL CASE for	*/
    /* RT_ARR_START where data_offset also updated before incrementing rt_type.*/
    track_info->data_offset += rt_type->el_size;
    }	/* end "for" loop	*/

  if (!array_flag)
    {	/* must have been called from "datamap_ln" (not datamap_array)	*/
    /* rt_type should be pointing at last struct in RUNTIME_TYPE array.	*/
    assert (rt_type == rt_type_last);
    }
  return (retcode);
  }
/************************************************************************/
/*  				datamap_array_count_prim		*/
/* WARNING: datamap_array_count_prim & datamap_loop_count_prim may	*/
/*	call each other	recursively.					*/
/* Add text for array, like "...$array[0]$etc".				*/
/* RETURNS:	ST_VOID		Cannot fail				*/
/************************************************************************/
static ST_VOID datamap_array_count_prim (
	RUNTIME_TYPE *rt_type,
	TRACK_INFO *track_info)
  {
ST_INT j;
ST_INT rt_num;		/* num RUNTIME_TYPE to define one element of array*/
ST_INT num_arr_elements;	/* num elements in array*/

  rt_num = rt_type->u.arr.num_rt_blks;
  num_arr_elements = rt_type->u.arr.num_elmnts;	/* num elements in array*/

  for (j = 0; j < num_arr_elements; ++j)
    {
    /* Recursive call to "datamap_loop_count_prim".	*/
    datamap_loop_count_prim (rt_type+1, rt_num, track_info);
    }
  return;
  }
/************************************************************************/
/*			datamap_loop_count_prim				*/
/* WARNING: datamap_array_count_prim & datamap_loop_count_prim may	*/
/*	call each other	recursively.					*/
/* RETURNS:	ST_VOID		Cannot fail				*/
/************************************************************************/
static ST_VOID datamap_loop_count_prim (
	RUNTIME_TYPE *rt_first,	
	ST_INT rt_num,
	TRACK_INFO *track_info)
  {
ST_RTINT num_rt_blks;
RUNTIME_TYPE *rt_type;		/* ptr to current array entry.	*/
ST_INT rt_index;		/* index into array of RUNTIME_TYPE	*/

  for (rt_index = 0, rt_type = rt_first; rt_index < rt_num; ++rt_index, ++rt_type)
    {
    switch (rt_type->el_tag)
      {
      case RT_STR_START:
      case RT_STR_END:
      case RT_ARR_END:
        break;			/* Do nothing	*/
      case RT_ARR_START:
        /* Recursive call to "datamap_array_count_prim".	*/
        datamap_array_count_prim (rt_type, track_info);
        num_rt_blks = rt_type->u.arr.num_rt_blks;
        rt_index += (num_rt_blks + 1);
        rt_type += (num_rt_blks + 1);
        break;
      default:	/* Must be primitive	*/
        track_info->primdata_num++;	/* CRITICAL: increment ONLY here.*/
        break;
      }	/* end "switch"	*/
    }	/* end "for" loop	*/

  return;
  }
/************************************************************************/
/*			datamap_ln					*/
/* Sets the "dataPtr" and "dataType"					*/
/* parameters in EVERY entry of the "map_ld->map_arr".		*/
/* This function allocates an array of (DATA_MAP *), one for each	*/
/* primitive data element. It would be easier to alloc one (DATA_MAP *)	*/
/* for each element of RUNTIME_CTRL, but that would not work if the type*/
/* contains arrays. This approach allows a unique (DATA_MAP *) for each	*/
/* element of an array.							*/ 
/* RETURNS:	SD_SUCCESS or error code				*/
/************************************************************************/
ST_RET datamap_ln (
	DATA_MAP_LD *map_ld,	/* Logical Device mappings to search	*/
				/* NOTE: NULL if this LD has no mappings*/
	ST_CHAR *dom_name,	/* Name of domain (Logical Device)	*/
				/* containing this variable (Logical Node)*/
	MVL_VAR_ASSOC *var,	/* Variable (Logical Node) info		*/
	FILE *fp_out_map)
  {
ST_CHAR branch_name[MAX_FLAT_LEN+1];	/* allow extra space for array indices	*/
RUNTIME_CTRL *rt_ctrl;
MVL_TYPE_CTRL *type_ctrl;
ST_RET retcode;
TRACK_INFO track_info;
DATA_MAP_HEAD *map_head_sorted;
RUNTIME_TYPE *rt_type_last;	/* to make sure don't go past end of RUNTIME_TYPE arr*/

  if ((strstr (var->name,"$")))
    {
    assert (0);	/* should always be top level var. No '$' in name	*/
    }

  /* "var" is top-level var. Should never be alternate access.*/
  assert (var->arrCtrl.arrAltAccPres == SD_FALSE);

  /* Find runtime type (RUNTIME_CTRL) for this Variable (Logical Node).	*/
  if ((type_ctrl = mvl_type_ctrl_find (var->type_id)) != NULL)
    rt_ctrl = type_ctrl->rt_ctrl;
  else
    {
    SXLOG_ERR2 ("Invalid type_id=%d for Variable '%s'", var->type_id, var->name);
    return (SD_FAILURE);
    }

  /* Assuming Logical Node is always a struct.	*/
  if (rt_ctrl->rt_first->el_tag != RT_STR_START)
    {
    SXLOG_ERR1 ("Variable '%s' is not a structure. Cannot be mapped using datamap_cfg_read", var->name);
    return (SD_FAILURE);
    }

  memset (&track_info, 0, sizeof (TRACK_INFO));	/* CRITICAL: initialize track_info*/
  strcpy (branch_name,var->name);

  rt_type_last = rt_ctrl->rt_first + rt_ctrl->rt_num;

  /* Process var once to find number of primitive data elements using "track_info->primdata_num".*/
  datamap_loop_count_prim (rt_ctrl->rt_first, rt_ctrl->rt_num, &track_info);

  /* Allocate DATA_MAP_HEAD struct plus array of (DATA_MAP *).	*/
  /* Save allocated ptr in "var->user_info".			*/
  map_head_sorted = chk_calloc (1, sizeof (DATA_MAP_HEAD) + track_info.primdata_num * sizeof (DATA_MAP *));
  map_head_sorted->map_arr = (DATA_MAP **)(map_head_sorted + 1);
  map_head_sorted->map_count = track_info.primdata_num;
  strcpy (map_head_sorted->ldevice, dom_name);

  assert (var->user_info == NULL);	/* CRITICAL: user_info must not be used for something else*/
  /* If assert fails, then all code in this module may need to avoid	*/
  /* using "user_info". Perhaps a new element could be added to "var".	*/
  var->user_info = map_head_sorted;

  /* Process var again to complete mapping.	*/
  memset (&track_info, 0, sizeof (TRACK_INFO));	/* CRITICAL: re-init track_info*/
  retcode =  datamap_loop (map_ld, dom_name, var,
                              rt_ctrl->rt_first, rt_ctrl->rt_num, rt_type_last,
                              &track_info, branch_name, SD_FALSE, fp_out_map);
  if (retcode == SD_SUCCESS)
    {
    assert (track_info.str_nest == 0);	/* finished processing top level struct*/
    /* CRITICAL: must get same count as datamap_loop_count_prim (saved in map_count)*/
    assert (track_info.primdata_num == map_head_sorted->map_count);
    /* Make sure data_offset was computed right.	*/
    /* Log before assert so easier to debug if assertion ever fails.	*/
    if (track_info.data_offset != rt_ctrl->rt_first->offset_to_last)
      SXLOG_ERR2 ("track_info.data_offset = %d  rt_ctrl->rt_first->offset_to_last = %d",
            track_info.data_offset, rt_ctrl->rt_first->offset_to_last);
    assert (track_info.data_offset == rt_ctrl->rt_first->offset_to_last);
    SXLOG_FLOW2 ("Number of primitive data elements in LN '%s' = %d", var->name, track_info.primdata_num);
    }
  return (retcode);
  }
/************************************************************************/
/*			datamap_cleanup					*/
/* Free up unnecessary mapping info.					*/
/* NOTE: this function does not free the DATA_MAP_HEAD struct saved in	*/
/*   the "user_info" member of MVL_VAR_ASSOC (i.e. "sorted" entries).	*/
/*   See "datamap_cfg_var_cleanup" for MVL_VAR_ASSOC cleanup.		*/
/************************************************************************/
static ST_VOID datamap_cleanup (
	DATA_MAP_LD **map_ld_array,	/* array of ptrs to LD mappings*/
	ST_UINT ld_count)			/* num array elements	*/
  {
ST_UINT ld_idx, leaf_idx;
DATA_MAP_LD *map_ld;
  for (ld_idx = 0;  ld_idx < ld_count;  ld_idx++)
    {
    map_ld = map_ld_array[ld_idx];
    for (leaf_idx = 0;  leaf_idx < map_ld->map_count;  leaf_idx++)
      {
      chk_free (map_ld->map_arr [leaf_idx]);
      }
    chk_free (map_ld->map_arr);
    chk_free (map_ld);
    }
  if (map_ld_array)
    chk_free (map_ld_array);
  }
/************************************************************************/
/*			datamap_cfg_read				*/
/* Reads data mapping configuration file & maps the data for all	*/
/* variables in the global VMD object "mvl_vmd".			*/
/* RETURNS:	SD_SUCCESS or error code				*/
/************************************************************************/
ST_RET datamap_cfg_read (
	ST_CHAR *in_filename,		/* input cfg file name	*/
	ST_CHAR *out_filename)		/* output cfg file name	*/
  {
ST_RET retcode = SD_SUCCESS;	/* assume SUCCESS	*/
FILE *fp_out_map;
MVL_DOM_CTRL *dom;
MVL_VAR_ASSOC *var;
ST_INT didx, vidx;	/* indices to domain & variable arrays	*/
DATA_MAP_LD **map_ld_array;	/* array of ptrs to LD mappings	*/
ST_UINT ld_count;		/* num array elements		*/
ST_UINT ld_idx;			/* current array index		*/
DATA_MAP_LD *map_ld;		/* mappings from input file for one LD	*/

  /* Read "Mapping Cfg" file once to count entries & set up LD array.	*/
  map_ld_array = datamap_cfg_alloc (in_filename, &ld_count);
  if (map_ld_array)	/* may be NULL if file is empty	*/
    {
    /* Read "Mapping Cfg" again and fill in all info in LD array.*/
    if (datamap_cfg_rd2 (in_filename, map_ld_array, ld_count) != SD_SUCCESS)
      {		/* ERROR already logged	*/
      datamap_cleanup (map_ld_array, ld_count);	/* free everything just allocated*/
      return (SD_FAILURE);	/* Give up now.	*/
      }
    }
  else
    ld_count = 0;	/* no mappings, so make sure count is 0	*/

  fp_out_map = fopen (out_filename, "w");
  if (fp_out_map == NULL)
    SXLOG_ERR1 ("WARNING: could not open output file '%s'", out_filename);

  /* Loop through all "Logical Devices" (domains).	*/
  for (didx = 0; didx < mvl_vmd.num_dom; didx++)
    {
    dom = mvl_vmd.dom_tbl [didx];

    /* Find map_ld (mappings from input file for this LD).		*/
    /* NOTE: map_ld == NULL if no mappings in input file for this LD.	*/
    map_ld = NULL;	/* assume LD not found	*/
    for (ld_idx = 0; ld_idx < ld_count; ld_idx++)
      {
      DATA_MAP_LD *tmp_map_ld = map_ld_array[ld_idx];
      if (strcmp (tmp_map_ld->ldevice, dom->name) == 0)
        {
        map_ld = tmp_map_ld;	/* found match	*/
        break;
        }
      }

    /* Log if no mappings in input file for this LD, but continue anyway.*/
    /* Dummy entries will be generated for this LD in output file.	*/
    if (map_ld == NULL)
      SXLOG_ERR3 ("No mapping entries found for LD='%s' in input file '%s'.\nDummy entries will be generated in output file '%s'.",
                  dom->name, in_filename, out_filename);

    /* Loop through all "Logical Nodes" (variables).	*/
    for (vidx = 0; vidx < dom->num_var_assoc; vidx++)
      {
      var = dom->var_assoc_tbl[vidx];
      retcode = datamap_ln (map_ld, dom->name, var, fp_out_map);
      if (retcode)
        break;		/* stop on first error	*/
      }
    if (retcode)
      break;		/* stop on first error	*/
    }
  if (fp_out_map)
    fclose (fp_out_map);

  /* Free up unnecessary buffers. All necessary information has already	*/
  /* been copied to appropriate objects.				*/
  datamap_cleanup (map_ld_array, ld_count);
  return (retcode);
  }

/************************************************************************/
/*			datamap_cfg_var_cleanup				*/
/* Clean up data mapping info in this var.				*/
/************************************************************************/

static ST_VOID datamap_cfg_var_cleanup (MVL_VAR_ASSOC *var)
  {
DATA_MAP_HEAD *map_head_sorted;
ST_UINT j;
DATA_MAP *map_entry;	/* current array entry	*/

  /* var->user_info points to DATA_MAP_HEAD struct	*/
  map_head_sorted = var->user_info;
  if (map_head_sorted)
    {
    for (j = 0; j < map_head_sorted->map_count; j++)
      {	/* map_arr has many NULL entries for non-primitive elements*/
      map_entry = map_head_sorted->map_arr [j];
      if (map_entry != NULL)
        {
        if (map_entry->deadband_info)
          chk_free (map_entry->deadband_info);
        chk_free (map_entry);
        }
      }
    chk_free (map_head_sorted);
    var->user_info = NULL;
    }
  }

/************************************************************************/
/*			datamap_cfg_destroy				*/
/* Unmap all data mapped by datamap_cfg_read and free associated buffers.*/
/* CRITITAL: must be called BEFORE variables removed, or it will be	*/
/*   impossible to remove the mapping.					*/
/************************************************************************/
ST_VOID datamap_cfg_destroy ()
  {
ST_INT didx, vidx;	/* indices to domain & variable arrays	*/
MVL_DOM_CTRL *dom;
MVL_VAR_ASSOC *var;

  for (didx = 0; didx < mvl_vmd.num_dom; didx++)
    {
    dom = mvl_vmd.dom_tbl [didx];
    for (vidx = 0; vidx < dom->num_var_assoc; vidx++)
      {
      var = dom->var_assoc_tbl[vidx];
      /* Clean up data mapping info in this var.	*/
      datamap_cfg_var_cleanup (var);
      }
    }
  }

/*======================================================================*/
/*======================================================================*/
/* Functions needed for using "qsort" and "bsearch".			*/
/*======================================================================*/
/*======================================================================*/

/************************************************************************/
/*			datamap_compare				*/
/* Compare function used by "qsort" and "bsearch".			*/
/* Sorts by "leaf" name for one LD.					*/
/************************************************************************/
static int datamap_compare (const void *obj1, const void *obj2)
  {
const DATA_MAP **datamap1 = (const DATA_MAP **) obj1;
const DATA_MAP **datamap2 = (const DATA_MAP **) obj2;
  return (strcmp ((*datamap1)->leaf, (*datamap2)->leaf));
  }

/************************************************************************/
/*			datamap_bsearch_key				*/
/* This function finds a pointer to a (DATA_MAP *) in a sorted array of	*/
/* (DATA_MAP *) (i.e a pointer to a pointer to DATA_MAP).		*/
/************************************************************************/
static DATA_MAP **datamap_bsearch_key (
	DATA_MAP **map_arr,		/* array of objects to search	*/
	ST_UINT map_count,		/* num elem in array		*/
	ST_CHAR *leaf_name)
  {
DATA_MAP **key;
DATA_MAP TmpObj;		/* Temporary object to search for.	*/
DATA_MAP *pTmpObj;		/* ptr to temp object.	*/

  if (map_count == 0)
    return (NULL);

  strcpy (TmpObj.leaf, leaf_name);	/* Copy info to temp object	*/

  /* Set ptr to object. Need this to pass "pointer pointer" to bsearch.	*/
  pTmpObj = &TmpObj;

  /* Already checked (map_count != 0) above.	*/
  key = (DATA_MAP **) bsearch (&pTmpObj, map_arr, map_count, 
        sizeof (DATA_MAP *),
        datamap_compare);	/* compares leaf name*/
  return (key);
  }

/************************************************************************/
/*			datamap_bsearch					*/
/* This function finds a (DATA_MAP *) in a sorted array of (DATA_MAP *).*/
/************************************************************************/
DATA_MAP *datamap_bsearch (
	DATA_MAP **map_arr,		/* array of objects to search	*/
	ST_UINT map_count,		/* num elem in array		*/
	ST_CHAR *leaf_name)		/* leaf name to search for	*/
  {
DATA_MAP **key;

  /* First find the "key".				*/
  key = datamap_bsearch_key (map_arr, map_count, leaf_name);

  /* If key != NULL, return "*key", else return NULL.	*/
  if (key)		/* "key" is pointer to pointer to structure.	*/
    return (*key);	/* "*key" is pointer to structure.		*/

  return (NULL);	/* Not found!!	*/
  }


