/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	2011 - 2011, All Rights Reserved				*/
/*									*/
/* MODULE NAME : dib_list.c						*/
/* PRODUCT(S)  : MMSEASE-LITE						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*	DIB management functions (for storing and finding addresses).	*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*			find_dib_entry					*/
/*			dib_entry_create				*/
/*			dib_entry_destroy				*/
/*			dib_entry_destroy_all				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who	   Comments					*/
/* --------  ---  ------   -------------------------------------------	*/
/* 09/15/11  JRB	   New						*/
/************************************************************************/
#include "glbtypes.h"
#include "sysincs.h"
#include "lean_a.h"

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/
/************************************************************************/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/* Global DIB linked list. Only accessed by functions in this file.	*/
DIB_ENTRY *dib_entry_list;	/* linked list of all DIB entries.	*/

/************************************************************************/
/*			find_dib_entry					*/
/************************************************************************/
DIB_ENTRY *find_dib_entry (ST_CHAR *arname)
  {
DIB_ENTRY *dib_entry;
  for (dib_entry = dib_entry_list;
       dib_entry != NULL;
       dib_entry = list_get_next (dib_entry_list, dib_entry))
    {
    if (strcmp (dib_entry->name, arname) == 0)
      break;	/* found a match	*/
    }
  return (dib_entry);
  }

/************************************************************************/
/*			dib_entry_create				*/
/* Allocate a struct, copy dib_entry to it, add it to global linked list.*/
/************************************************************************/
ST_VOID dib_entry_create (DIB_ENTRY *dib_entry)
  {
DIB_ENTRY *new_dib_entry;
  new_dib_entry = chk_malloc (sizeof (DIB_ENTRY));
  memcpy (new_dib_entry, dib_entry, sizeof (DIB_ENTRY));
  list_add_last (&dib_entry_list, new_dib_entry);
  }

/************************************************************************/
/*			dib_entry_destroy				*/
/* Remove dib_entry from global linked list and free it.		*/
/************************************************************************/
ST_VOID dib_entry_destroy (DIB_ENTRY *dib_entry)
  {
  list_unlink (&dib_entry_list, dib_entry);	/* remove from list	*/
  chk_free (dib_entry);
  }

/************************************************************************/
/*			dib_entry_destroy_all				*/
/* Destroy all DIB entries.						*/
/************************************************************************/
ST_VOID dib_entry_destroy_all ()
  {
DIB_ENTRY *dib_entry;
  /* Destroy first entry on the list, until list is empty (NULL).	*/
  while ((dib_entry = dib_entry_list) != NULL)
    {
    dib_entry_destroy (dib_entry);	/* remove from list AND free it*/
    }
  }

