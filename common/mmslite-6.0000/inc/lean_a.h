#ifndef LEAN_A_H
#define LEAN_A_H
/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	1996 - 2009 All Rights Reserved					*/
/*									*/
/* MODULE NAME : lean_a.h						*/
/* PRODUCT(S)  : Lean-T Stack						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*	NONE								*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 09/01/11  JRB	   Add dib_entry_create/destroy.		*/
/* 11/17/09  JRB     13    Del _WIN32, __OS2__ includes (use sysincs.h)	*/
/* 11/01/09  JRB     12    Merge these changes by EJV:			*/
/*			   dib_entry: made 'name' an arr instead of ptr,*/
/*			   added list ptrs. Rpl fields with AE_TITLE.	*/
/*			   Move SUIC stuff to SUIC files (chanctrl,	*/
/*			   s_chan_ctrl, s_reset_ctrl, MAX_SUIC_CHAN).	*/
/*			   Del obsolete init_dirser, *_clt7 prototypes.	*/
/*			   Del MMSEASE_MOSI code.			*/
/* 05/13/05  EJV     11    MMSEASE_MOSI: added extern domsock_listener	*/
/* 03/15/05  EJV     10    MMSEASE_MOSI: set MAX_SUIC_CHAN to 1000	*/
/*			     Added mms_event_fd.			*/
/* 02/08/02  JRB     09    Move osicfgx prototype to mvl_acse.h.	*/
/* 11/27/00  JRB     08    Add osicfgx proto.				*/
/* 12/10/97  JRB     07    Add MMSEventName and hMMSEvent for OS2.	*/
/* 08/14/97  JRB     06    #include "acse2usr.h" and use PRES_ADDR in	*/
/*			   DIB_ENTRY.					*/
/* 05/27/97  JRB   7.00    MMSEASE 7.0 release.				*/
/* 05/02/97  JRB     05    Del MAX_?_SEL defines.  Use suicacse.h defs.	*/
/* 05/01/97  JRB     04    Move MAX_SUIC_CHAN define to here.		*/
/* 01/02/96  JRB     03    Add MMSEventName and hMMSEvent.		*/
/* 10/04/96  JRB     02    #ifdef MCGW, add adlcAddr to DIB_ENTRY.	*/
/*			   Add *_clt7 function prototypes.		*/
/* 05/29/96  MDE     01    Changed DIB_ENTRY - name is now a pointer	*/
/* 03/27/96  MDE           Created	                              	*/
/************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#include "asn1defs.h"		/* for MMS_OBJ_ID definition		*/
#include "acse2usr.h"		/* for PRES_ADDR definition		*/
#include "mms_def2.h"		/* for MAX_AR_LEN definition		*/

/************************************************************************/
/* The structure below is used to save Directory and other information	*/
/* about local and remote AE's.						*/
/************************************************************************/

#define DIB_ENTRY_DEFINED

struct dib_entry
{
  /* CRITICAL: First 2 parameters used to add this struct to linked	*/
  /* lists using list_add_last, etc.					*/
  struct dib_entry *next;		/* CRITICAL: DON'T MOVE.	*/
  struct dib_entry *prev;		/* CRITICAL: DON'T MOVE.	*/
  /* NOTE: MMS Lite doesn't use "reserved". MMS-EASE saves bind_id there.*/
  ST_VOID *reserved;		/* reserved field			*/
  ST_CHAR name[MAX_AR_LEN + 1];	/* AR Name				*/
  ST_CHAR local;		/* AR Name role: 1 - local or 0 - remote*/
  AE_TITLE ae_title;		/* AE Title (ACSE address parameters)	*/
  PRES_ADDR pres_addr;		/* Presentation address.		*/
} ;
typedef struct dib_entry DIB_ENTRY;

/************************************************************************/
/* Structure to store incoming associate indication in case no listen	*/
/* channel is available							*/
/************************************************************************/

extern ST_INT num_loc_dib_entries;
extern ST_INT num_rem_dib_entries;
extern DIB_ENTRY *loc_dib_table;
extern DIB_ENTRY *rem_dib_table;

/************************************************************************/
/* Event handling variables for WIN32 & OS2.				*/
/************************************************************************/
#ifdef _WIN32
extern ST_CHAR *MMSEventName;	/* Pointer to the globally unique name of Event Object */
extern HANDLE hMMSEvent;	/* Handle to Event Object to be signalled when */
				/* s_serve () needs to be called */
#endif	/* _WIN32	*/
#ifdef __OS2__
extern ST_CHAR *MMSEventName;	/* Pointer to the globally unique name of Event Object */
extern HEV      hMMSEvent;	/* Handle to Event Object to be signalled when */
				/* s_serve () needs to be called */
#endif	/* __OS2__	*/

/************************************************************************/
/* Prototypes								*/
/************************************************************************/
/* find_dib_entry searches combined list of local AND remote entries.	*/
DIB_ENTRY *find_dib_entry (ST_CHAR *ar_name);
DIB_ENTRY *find_loc_dib_entry (ST_CHAR *ar_name);
DIB_ENTRY *find_rem_dib_entry (ST_CHAR *ar_name);

ST_VOID dib_entry_create (DIB_ENTRY *dib_entry);
ST_VOID dib_entry_destroy (DIB_ENTRY *dib_entry);
ST_VOID dib_entry_destroy_all (ST_VOID);

#ifdef __cplusplus
}
#endif

#endif	/* !LEAN_A_H	*/


