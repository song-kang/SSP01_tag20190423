#ifndef RS_A_H
#define RS_A_H

/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*		1993, 1994, All Rights Reserved				*/
/*									*/
/* MODULE NAME : rs_a.h							*/
/* PRODUCT(S)  : REDUCED STACK SUIC 					*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*	NONE								*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 03/09/98  JRB     02    Chg MAX_SUIC_CHAN from 256 to 1024.		*/
/* 10/20/97  JRB     01    Added #include "asn1defs.h" for MMS_OBJ_ID.	*/
/* 05/27/97  JRB   7.00    MMSEASE 7.0 release.				*/
/* 05/01/97  JRB     03    Move MAX_SUIC_CHAN define to here.		*/
/* 01/02/96  JRB     02    Add MMSEventName and hMMSEvent.		*/
/* 05/29/96  MDE     01    Changed DIB_ENTRY - name is now a pointer	*/
/* 03/27/96  MDE           Created	                              	*/
/************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif
									  
#if defined (_WIN32)
#include <windows.h>
#endif	/* _WIN32	*/

#if defined (__OS2__)
#define INCL_BASE
#include <os2.h>
#endif	/* __OS2__	*/

#include "asn1defs.h"		/* for MMS_OBJ_ID definition		*/

#define MAX_SUIC_CHAN 1024	/* number of channels			*/

/************************************************************************/
/* The structure below is used to save Directory and other information	*/
/* about local and remote AE's read in from SUIC.DIB			*/
/************************************************************************/

#define DIB_ENTRY_DEFINED

struct dib_entry
{
  ST_LONG reserved;		/* reserved field			*/
  ST_CHAR *name;			/* user-defined ASCII character string	*/
  ST_CHAR local;			/* local or remote name			*/
  ST_UCHAR AP_title_pres;		/* present flag				*/
  MMS_OBJ_ID AP_title; 		/* AP title    				*/
  ST_UCHAR AP_inv_id_pres;		/* present flag				*/
  ST_INT32 AP_invoke_id;		/* AP invocation ID    			*/
  ST_UCHAR AE_qual_pres;		/* present flag				*/
  ST_INT32 AE_qual;			/* AE qualifier    			*/
  ST_UCHAR AE_inv_id_pres;		/* present flag				*/
  ST_INT32 AE_invoke_id;		/* AE invocation ID    			*/
  ST_UINT16 adlcAddr;
} ;
typedef struct dib_entry DIB_ENTRY;

/************************************************************************/
/* Channel-oriented control structure used to hold connection-specific	*/
/* information								*/
/************************************************************************/

#define MAX_TX_OUT	10	/* max. outstanding TX requests allowed	*/

struct chanctrl
  {
  DIB_ENTRY *loc_de;		/* Pointer to Local DIB Entry */
  DIB_ENTRY *rem_de;		/* Pointer to Remote DIB Entry */
  } ;
extern struct chanctrl *s_chan_ctrl;

/************************************************************************/
/* Structure to store incoming associate indication in case no listen	*/
/* channel is available							*/
/************************************************************************/

extern ST_INT num_loc_dib_entries;
extern ST_INT num_rem_dib_entries;
extern DIB_ENTRY *loc_dib_table;
extern DIB_ENTRY *rem_dib_table;

/************************************************************************/
/* Event handling variables for WIN32.					*/
/************************************************************************/
#ifdef _WIN32
extern ST_CHAR *MMSEventName;	/* Pointer to the globally unique name of Event Object */
extern HANDLE hMMSEvent;	/* Handle to Event Object to be signalled when */
				/* s_serve () needs to be called */
#endif	/* _WIN32	*/

/************************************************************************/
/* Prototypes								*/
/************************************************************************/
ST_VOID s_reset_ctrl (ST_INT);
ST_RET init_dirser (ST_VOID);
DIB_ENTRY *find_dib_entry (ST_CHAR *);
DIB_ENTRY *find_loc_dib_entry (ST_CHAR *ar_name);
DIB_ENTRY *find_rem_dib_entry (ST_CHAR *ar_name);

#ifdef __cplusplus
}
#endif

#endif


