/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	2011 - 2011, All Rights Reserved				*/
/*									*/
/* MODULE NAME : scl_dib.c						*/
/* PRODUCT(S)  : MMSEASE-LITE						*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*	Functions to fill DIB_ENTRY list from SCL informaion.		*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*			scl_dib_entry_save				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who	   Comments					*/
/* --------  ---  ------   -------------------------------------------	*/
/* 09/15/11  JRB	   New						*/
/************************************************************************/
#include "glbtypes.h"
#include "sysincs.h"
#include "scl.h"
#include "lean_a.h"
#include "str_util.h" /* for strncpy_safe, etc.	*/

#ifdef USE_DIB_LIST	/* this function only works in this mode	*/
/************************************************************************/
/*			scl_dib_entry_save				*/
/* Find all addresses in the SCL file. Fill in a DIB_ENTRY		*/
/* structure for each address and then use "dib_entry_create" to save	*/
/* the address in a global linked list.					*/
/* Generate unique ARName by concatenating iedName, "/", and apName.	*/
/************************************************************************/
ST_INT scl_dib_entry_save (SCL_INFO *scl_info)
  {
SCL_SUBNET *scl_subnet;
SCL_CAP *scl_cap;
ST_INT addr_count = 0;
  
  /* Communication section may contain multiple Subnetwork sections.	*/
  /* Use only the first Subnetwork found with type="8-MMS".		*/
  for (scl_subnet = (SCL_SUBNET *) list_find_last ((DBL_LNK *) scl_info->subnetHead);
       scl_subnet != NULL;
       scl_subnet = (SCL_SUBNET *) list_find_prev ((DBL_LNK *) scl_info->subnetHead, (DBL_LNK *) scl_subnet))
    {
    for (scl_cap = (SCL_CAP *) list_find_last ((DBL_LNK *) scl_subnet->capHead);
         scl_cap != NULL;
         scl_cap = (SCL_CAP *) list_find_prev ((DBL_LNK *) scl_subnet->capHead, (DBL_LNK *) scl_cap))
      {
      /* If the IP address is 0, assume scl_cap->address was not set.	*/
      /* Maybe only GSE or SMV address was set.				*/
      if (scl_cap->address.ip != 0)
        {
        DIB_ENTRY dib_entry;	/* fill in this temporary struct	*/
        memset (&dib_entry, 0, sizeof (DIB_ENTRY));	/* start clean	*/

        /* Generate unique ARName by concatenating iedName, "/", and apName.*/
        strncpy_safe      (dib_entry.name, scl_cap->iedName, sizeof(dib_entry.name)-1);
        strncat_maxstrlen (dib_entry.name, "/", sizeof(dib_entry.name)-1);
        strncat_maxstrlen (dib_entry.name, scl_cap->apName, sizeof(dib_entry.name)-1);

        /* Copy info from scl_cap->address to DIB_ENTRY.	*/
        memcpy (&dib_entry.ae_title, &scl_cap->address.ae_title, sizeof (AE_TITLE));

        dib_entry.pres_addr.psel_len = scl_cap->address.psel_len;
        memcpy (dib_entry.pres_addr.psel, scl_cap->address.psel, scl_cap->address.psel_len);

        dib_entry.pres_addr.ssel_len = scl_cap->address.ssel_len;
        memcpy (dib_entry.pres_addr.ssel, scl_cap->address.ssel, scl_cap->address.ssel_len);

        dib_entry.pres_addr.tsel_len = scl_cap->address.tsel_len;
        memcpy (dib_entry.pres_addr.tsel, scl_cap->address.tsel, scl_cap->address.tsel_len);

        dib_entry.pres_addr.netAddr.ip = scl_cap->address.ip;
        /* This allocates a struct, copies to it, and adds it to global linked list.*/
        dib_entry_create (&dib_entry);
        addr_count++;
        }		/* end scl_cap loop	*/
      }
    }		/* end scl_subnet loop	*/

  return (addr_count);
  }
#endif	/* USE_DIB_LIST	*/

