#ifndef ETHSUB_INCLUDED
#define ETHSUB_INCLUDED
/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	2000-2006 All Rights Reserved					*/
/*									*/
/* MODULE NAME : ethsub.h						*/
/* PRODUCT(S)  : Lean-T Stack for Windows 95/98/NT			*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*	Definitions to allow Subnetwork interface to work with OSI LLC	*/
/*	Ethernet driver.						*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev     Comments					*/
/* --------  ---  ------   -------------------------------------------  */
/* 07/21/06  JRB    04     Chg to use SISCO naming conventions.		*/
/* 04/14/06  JRB    03     Add sysincs header.				*/
/* 11/20/02  ASK    02     Add ethdown_etype proto			*/
/* 04/06/00  JRB    01     Created					*/
/************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************/
/*		Include files required to interface with	*/
/*		OSI LLC NDIS Ethernet Driver			*/
/*                                                              */
/****************************************************************/

#include "conpack.h"
#include "osillc.h"

#include "glbtypes.h"
#include "sysincs.h"
#include "slog.h"
#include "stime.h"
#include "clnp_usr.h"	/* clnp_sne.h needs this	*/
#include "clnp_log.h"
#include "clnp_sne.h"	/* for SN_UNITDATA, etc.	*/

/****************************************************************/
/****************************************************************/

typedef struct
{
/** ----- Following fields are specific to this Access Method */
 HANDLE hFile;		/* LLC device driver descriptor */

 unsigned long seq;	/* sequence number to expect */
 void *readlst[10];	/* list of pointers to out of sequence packet blocks */

 /* ----- Following fields are required for 95/98 */
 struct rd_wr_block *pWriteEvents;	/* list of write event structures */
 struct rd_wr_block *pReadEvents;	/* list of read event structures */
} LLC_CONTEXT;

/************************************************************************/
/* OSI Event structure - Modified for MMS_LITE				*/
/************************************************************************/

typedef struct
  {
  void * nxevt;
  SN_UNITDATA sn_req;	/* event data in format needed by LEANT CLNP.	*/
  } SN_EVENT;

typedef struct rd_wr_block
  {
  int inuse;
  HANDLE hEvent;
  OVERLAPPED *ovlp;
  ST_UCHAR *framep;	/* pointer to raw Ethernet frame	*/
  void *pb;
  LLC_CONTEXT *gccp;
  } RD_WR_BLOCK;

/************************************************************************/
/* MMS_LITE function to Queue received packet for Lite CLNP.		*/
/************************************************************************/
void QueueRxPacket (ST_UCHAR *framep);

/************************************************************************/
/* Functions that may be called from CLNP.				*/
/************************************************************************/
void ethdown (SN_UNITDATA *sn_req);
void ethdown_etype (SN_UNITDATA *sn_req);
void ethsapoff (void);
void ethsapon (ST_CHAR *drvname);
int llcsetup (void);

BOOL QueryOid (HANDLE hDriver, ULONG ulOid, PBYTE pBuffer, ULONG ulLength);
BOOL SetOid   (HANDLE hDriver, ULONG ulOid, PBYTE pBuffer, ULONG ulLength);

void osifreeevt (SN_EVENT *evt);
SN_EVENT *osiallocevt ();
void osiputevt (SN_EVENT *evt);
SN_EVENT *osigetevt ();

void OsiReadThread (LPDWORD lpdwParam);
void OsiReadThreadNT (LPDWORD lpdwParam);
void CALLBACK WriteCompletion (DWORD error, DWORD length, OVERLAPPED *ovlp);
void CALLBACK ReadCompletion (DWORD error, DWORD length, OVERLAPPED *ovlp);
void ProcessReadBlock (OVERLAPPED *ovlp);

/************************************************************************/
/* Global variables.							*/
/************************************************************************/
extern LLC_CONTEXT GCC;		/* General Current Context */
extern LLC_CONTEXT *GCCP;	/* General Current Context Pointer */

extern int MaxWriteOut;
extern int MaxReadOut;
extern int FramesPerBlock;
extern int ReceptionMode;
extern int DriverID;
extern int NumWriteOut;
extern CRITICAL_SECTION csOsiList;   /* Critical Section object for OSI Event List */
extern BOOL bServiceControl;
extern BOOL bNT;
extern BOOL bOsiRequired;
extern DWORD Priority;

#ifdef __cplusplus
}
#endif

#endif	/* !ETHSUB_INCLUDED	*/

