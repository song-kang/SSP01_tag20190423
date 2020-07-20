#ifndef CLNP_TNT_INCLUDED
#define CLNP_TNT_INCLUDED
/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*	(c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*	1998 - 2000 All Rights Reserved					*/
/*									*/
/* MODULE NAME : clnp_tnt.h						*/
/* PRODUCT(S)  : Lean-T Stack (Phar Lap TNT ETS port only)		*/
/*									*/
/* MODULE DESCRIPTION :							*/
/*	Stub function prototypes for Phar Lap TNT ETS. Ethernet drivers	*/
/*	must be modified to call these functions instead of the normal	*/
/*	functions in the Phar Lap TCP/IP stack.  This allows the SISCO	*/
/*	OSI stack to intercept these calls and and do any necessary OSI	*/
/*	processing before passing them on to the Phar Lap TCP/IP stack.	*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev     Comments					*/
/* --------  ---  ------   -------------------------------------------  */
/* 05/05/00  JRB    03     Remove last chg. No longer needed.		*/
/* 03/19/99  EJV    02     pfXmit has 4 parameters in Phar Lap ver 9.1	*/
/*			   (with NE2K patch)				*/
/*			   Introduced the _PHARLAP91_PATCHED define	*/
/* 10/30/98  JRB    01     Created					*/
/************************************************************************/
int    __cdecl stubEtsTCPSetDeviceEthernetInfo(DEVHANDLE hDriver,
					   char *pEnetStationAddr);
int    __cdecl stubEtsTCPRegisterDeviceFuncs(DEVHANDLE hDevice,
			int (__cdecl *pfUpDown)(DEVHANDLE hDevice,
						unsigned short goingUp,
						char *options),
			int (__cdecl *pfIoctl)(DEVHANDLE hDevice, int mode),
			int (__cdecl *pfXmit)(DEVHANDLE hDevice,
					      long *pPacket, int packetLen),
			int (__cdecl *pfSend)(void *pPacketHdr));

void __cdecl stubEtsTCPQueueRecvBuffer(DEVHANDLE hDriver, void *hMsg,
                         int pkt_len,
                         char *pRecvBuf);

#endif	/* !CLNP_TNT_INCLUDED	*/
