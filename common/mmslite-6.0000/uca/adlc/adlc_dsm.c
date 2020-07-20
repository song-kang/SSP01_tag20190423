/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,       */
/*          1995-2002, All Rights Reserved                              */
/*                                                                      */
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*                                                                      */
/* MODULE NAME : adlc_dsm.c						*/
/* PRODUCT(S)  : ADLC			                             	*/
/*                                                                      */
/* MODULE DESCRIPTION :                                                 */
/* This module contains functions that provide for serial port communi-	*/
/* cations 								*/
/*                                                                      */
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :                            */
/*	ST_RET sm_init(port,baud,parity,data,stop			*/
/*	ST_RET sm_exit(port)						*/
/*      ST_RET sm_put (port,len,buffer)                             	*/
/* 	ST_RET sm_putc (port,ch)					*/
/*	ST_INT sm_rx_cnt (port)						*/
/*	ST_RET sm_get (port,packet,toRead,bytesRead)			*/
/*      ST_INT sm_getc (port)                                       	*/
/*      ST_VOID sm_rx_flush(port)					*/
/*	ST_RET sm_set_signals (port)					*/
/*	ST_RET sm_clear_signals (port)					*/
/*	ST_RET sm_set_rts (port)					*/
/*	ST_RET sm_clear_rts (port)					*/
/*	ST_RET sm_set_dtr (port)					*/
/*	ST_RET sm_clear_dtr (port)					*/
/*	ST_RET sm_set_rts_clear_dtr(port)				*/
/*	ST_RET sm_clear_rts_set_dtr(port)				*/
/*	ST_INT sm_get_rts (port)					*/
/*	ST_INT sm_get_dtr (port)					*/
/*	ST_INT sm_get_cts (port)					*/
/*	ST_INT sm_get_dsr (port)					*/
/*	ST_INT sm_get_dcd (port)					*/
/*                                                                      */
/* MODIFICATION LOG :                                                   */
/*  Date     Who   Rev      Comments                                    */
/* --------  ---  ------   -------------------------------------------  */
/* 09/23/02  KCR    16     Moved thisFileName before includes           */
/* 09/13/99  MDE    15     Added SD_CONST modifiers         		*/
/* 05/12/97  IKE    14     Changed to MMSEASE 7.0 Data Types		*/
/* 02/21/97  IKE    13     Changed COM_TEST to ESSENTIAL_COM to make	*/
/*			   more sense, some cleanup			*/
/* 02/03/97  IKE    12     adlcCfg per port basis			*/
/* 09/19/96  IKE    11     Rearranged sm_get and sm_set functions	*/
/*			   so they are in same order as in adlc_osm.c	*/	
/* 08/22/96  IKE    10     Added sm_get_dcd, check for contention 	*/
/*			   avoidance mode for master, clear_rts for 	*/
/*			   slave when exiting				*/
/* 07/17/96  IKE    09     Removed some lint and C6.00 warnings 	*/
/* 06/21/96  IKE    08     Changed logging to ADLC_LOG_SM for Serial M.	*/
/* 06/18/96  IKE    07     Removed stimeInit() function call		*/
/* 06/11/96  IKE    06     Changed all int16 calls to D16rmInterrupt for*/
/*			   DOS16M. Changed sm_get. Added some more 	*/
/*			   functions					*/
/* 05/30/96  IKE    05     Added prefix adlc_ to real_mem_buf_ptr to be	*/
/*			   able to link with DGW			*/
/* 05/22/96  IKE    04     Header file cleanup. Added time_delay and 	*/
/*			   time_elapsed. Changed sm_put for slave	*/
/*			   to deal with RS232 control signals		*/
/*			   Added some functions to control the RS232 	*/
/*			   signals					*/
/* 04/05/96  rkr    03     Header file and SAVE_CALLING cleanup 	*/
/* 04/05/96  rkr    02	   Created from split up of adlc_sm.c		*/
/* 02/15/96  IKE    01	   Merged DOS adlc_sm.c and rearrangement	*/
/* 12/15/95  IKE  1.00     Created                                      */
/************************************************************************/

static char *thisFileName = __FILE__;
#include <dos.h>
#include "glbtypes.h"
#include "sysincs.h"
#include "adlc_dsm.h"
#include "adlc_log.h"
#include "adlc.h"

#ifdef ESSENTIAL_COM
static ST_BOOLEAN xc_initialized = SD_FALSE;

#ifdef DOS16M
#undef DOS16M 
#endif /* DOS16M */
#endif /* ESSENTIAL_COM */

#ifdef DOS16M
#include "dos16.h"

ST_CHAR *adlc_real_mem_buf_ptr=NULL;
ST_CHAR *adlc_prot_mem_buf_ptr=NULL;
#endif

/************************************************************************/
/*                      	sm_init                                 */
/************************************************************************/

ST_RET sm_init (ST_INT port,ST_INT baud, ST_INT parity,
		 ST_INT data, ST_INT stop)
  {
ST_RET ret = SD_SUCCESS;
#ifdef DOS16M
ST_INT16 old_strategy;
#endif /* DOS16M */

#if defined(ESSENTIAL_COM)
  if (port > COM2)
#else
  if (port < COM5)
#endif
    {	
    ADLC_LOG_ERR1("ADLC ERROR initializing Communication for COM%d",GET_COM_NO(port));
    return (SD_FAILURE);
    }
#ifdef DOS16M
  if(adlc_real_mem_buf_ptr==NULL)
    {
    old_strategy = D16MemStrategy(MForceLow);
    adlc_prot_mem_buf_ptr = D16MemAlloc(2000);
    adlc_real_mem_buf_ptr = D16RealPtr(adlc_prot_mem_buf_ptr);
    D16MemStrategy(old_strategy);
    if (!adlc_prot_mem_buf_ptr || !adlc_real_mem_buf_ptr)
      ret = SD_FAILURE;
    }
#endif /* DOS16M */

#ifdef ESSENTIAL_COM 

  if (xc_initialized == SD_FALSE)
    {
    if (xc_entr (8)  == 0)
      {
      xc_link (port, 0);
      xc_init (port, baud, parity, data, stop);
      }
    else
      {
      ADLC_LOG_ERR1("ADLC ERROR initializing Communication for COM%d",GET_COM_NO(port));
      return (SD_FAILURE);
      }
    xc_initialized = SD_TRUE;
    }
#endif /* ESSENTIAL_COM */
  return (ret);
  }

/************************************************************************/
/*                                  sm_exit                             */
/************************************************************************/

ST_RET sm_exit (ST_INT port)
  {
ST_INT ret = SD_SUCCESS;
#ifdef ESSENTIAL_COM 
  if (xc_initialized == SD_TRUE)
    {
#if defined(ADLC_SLAVE)
  ret = sm_clear_rts(port);
  ret = sm_set_dtr(port);
#endif
    xc_unlk (port);
    xc_exit ();
    xc_initialized = SD_FALSE;
    }
#endif /* ESSENTIAL_COM */
  return (SD_SUCCESS);
  }


/************************************************************************/
/*                               sm_put                                 */
/* This function is used to write a buffer to a comm port		*/
/************************************************************************/

ST_RET sm_put (ST_INT port,ST_INT len,ST_CHAR *packet)
  {
ST_INT ret = 0;
#ifdef DOS16M
ST_INT16 old_strategy;
D16REGS inregs,outregs;
#else
#if !defined(ESSENTIAL_COM)	/* using a digiboard */
union REGS inregs,outregs;
struct SREGS sregs;
#endif
#endif

  ADLC_LOG_SM2 ("Sending Packet, Port %d, Len %d",port,len);
  ADLC_LOG_SM_HEX (len,packet);

#ifdef DOS16M
  if(adlc_real_mem_buf_ptr==NULL)
    {
    old_strategy = D16MemStrategy(MForceLow);
    adlc_prot_mem_buf_ptr = D16MemAlloc(2000);
    adlc_real_mem_buf_ptr = D16RealPtr(adlc_prot_mem_buf_ptr);
    D16MemStrategy(old_strategy);
    }

  memcpy(adlc_prot_mem_buf_ptr,packet,len);

  inregs.ax = 0x0e00;
  inregs.bx = FP_OFF (adlc_real_mem_buf_ptr);
  inregs.es = FP_SEG (adlc_real_mem_buf_ptr);
  inregs.cx = len;
  inregs.dx = port;
  D16rmInterrupt(0x14,&inregs,&outregs);
  if (outregs.ax != (ST_UINT16) len)
    ret = -1;

#else /* DOS16M is not defined */

#if defined(ESSENTIAL_COM)
    ret = xc_put (port, (ST_CHAR*) packet, &len);
#endif

#if !defined(ESSENTIAL_COM)	/* a digiboard in real mem */
  inregs.h.ah = 14;
  inregs.x.bx = FP_OFF (packet);
  sregs.es = FP_SEG (packet);
  inregs.x.cx = (ST_UINT16) len;
  inregs.x.dx = (ST_UINT16) port;
  int86x (0x14,&inregs,&outregs,&sregs);
  if (outregs.x.ax != (ST_UINT16) len)
    ret = -1;
#endif /* !ESSENTIAL_COM */

#endif /* DOS16M not defined */
  return (ret);
  }

/************************************************************************/
/*	                          sm_putc				*/
/************************************************************************/
ST_RET sm_putc(ST_INT port, ST_CHAR ch)
  {
ST_INT ret = SD_SUCCESS;

#ifdef ESSENTIAL_COM
  ret = xc_putc (port, ch);
#else

#ifdef DOS16M /* digiboard in protected mode */
D16REGS inregs,outregs;
  inregs.ax = 0x0100;
  inregs.ax = inregs.ax | ch;
  inregs.dx = port;
  D16rmInterrupt(0x14,&inregs,&outregs);
  if (outregs.ax & 0x8000)
    ret = -1;

#else 	/* digiboard in real mode	    */
union REGS inregs,outregs;

  inregs.h.ah = 1;
  inregs.h.al = ch;
  inregs.x.dx = (ST_UINT16) port;
  int86 (0x14,&inregs,&outregs);

  if (outregs.h.ah & 0x80)
    ret = -1;
#endif /* DOS16M */
#endif /* ESSENTIAL_COM */

  return (ret);
  }
/************************************************************************/
/*                               sm_rx_cnt                                 */
/* This function is get the number of characters in the input buffer	*/
/************************************************************************/

ST_INT sm_rx_cnt (ST_INT port)
  {
ST_INT ret = 0;

#ifdef ESSENTIAL_COM
  ret = (ST_INT) xc_test (port);
#else

#ifdef DOS16M /* digiboard in protected mode */
D16REGS inregs,outregs;
  inregs.ax = 0x1500;
  inregs.dx = port;
  D16rmInterrupt(0x14,&inregs,&outregs);
  ret = outregs.ax;

#else 	/* digiboard in real mode	    */
union REGS inregs,outregs;

  inregs.h.ah = 0x15;
  inregs.x.dx = (ST_UINT16) port;
  int86 (0x14,&inregs,&outregs);

  ret = (ST_INT) outregs.x.ax;

#endif /* DOS16M */
#endif /* ESSENTIAL_COM */

  return (ret);
  }

/************************************************************************/
/*                               sm_tx_cnt                              */
/* This function is to get the number of characters in the output buffer*/
/************************************************************************/
ST_INT sm_tx_cnt(ST_INT port)
  {
ST_INT ret = 0;

#ifdef ESSENTIAL_COM
  ret = (ST_INT) xc_utest (port);
#else

#ifdef DOS16M /* digiboard in protected mode */
D16REGS inregs,outregs;
  inregs.ax = 0xfd01;
  inregs.dx = port;
  D16rmInterrupt(0x14,&inregs,&outregs);
  ret = outregs.cx;

#else 	/* digiboard in real mode	    */
union REGS inregs,outregs;

  inregs.h.ah = 0xfd;
  inregs.x.dx = (ST_UINT16) port;
  inregs.h.al = 1;   /* for transmit buffer */
  int86 (0x14,&inregs,&outregs);

  ret = (ST_INT) outregs.x.cx;

#endif /* DOS16M */
#endif /* ESSENTIAL_COM */

  return (ret);
  }
/************************************************************************/
/*                      sm_get                                          */
/* This function is used to read a buffer from a comm port		*/
/************************************************************************/
ST_RET sm_get (ST_INT port,ST_CHAR *packet, ST_INT toRead, ST_INT *bytesRead)
  {
ST_INT ret = SD_SUCCESS;
#if defined(ESSENTIAL_COM)
ST_INT ret_len;

  ret_len = toRead;
  ret = xc_get (port, (ST_CHAR*) packet, &ret_len);
  *bytesRead = ret_len;
  if (*bytesRead == 0)
    ret = SD_FAILURE;

#else
#ifdef DOS16M
ST_INT16 old_strategy;
D16REGS inregs,outregs;

  if(adlc_real_mem_buf_ptr==NULL)
    {
    old_strategy = D16MemStrategy(MForceLow);
    adlc_prot_mem_buf_ptr = D16MemAlloc(2000);
    adlc_real_mem_buf_ptr = D16RealPtr(adlc_prot_mem_buf_ptr);
    D16MemStrategy(old_strategy);
    }

  inregs.ax = 0x0f00;
  inregs.bx = FP_OFF (adlc_real_mem_buf_ptr);
  inregs.es = FP_SEG (adlc_real_mem_buf_ptr);
  inregs.cx = toRead;
  inregs.dx = port;
  D16rmInterrupt(0x14,&inregs,&outregs);
  *bytesRead = outregs.ax;
  if (*bytesRead == 0)
    ret = SD_FAILURE;
  else
    memcpy(packet,adlc_prot_mem_buf_ptr,*bytesRead);

#else /* DOS16M is not defined */
union REGS inregs,outregs;
struct SREGS sregs;

  inregs.h.ah = 0x0F;
  inregs.x.bx = FP_OFF (packet);
  inregs.x.cx = (ST_UINT16) toRead;
  inregs.x.dx = (ST_UINT16) port;
  sregs.es = FP_SEG (packet);
  int86x (0x14,&inregs,&outregs,&sregs);
  *bytesRead = outregs.x.ax;
  if (*bytesRead == 0)
    ret = SD_FAILURE;
#endif /* !ESSENTIAL_COM */
#endif /* DOS16M not defined */

  return (ret);
  }

/************************************************************************/
/*                      sm_getc                                         */
/* This function is used to receive a character from a comm port	*/
/************************************************************************/

ST_INT sm_getc (ST_INT port)
  {
ST_INT ret;

#ifdef ESSENTIAL_COM
  ret = xc_getc (port);
#else
#ifdef DOS16M /* digiboard in protected mode */
D16REGS inregs,outregs;

  inregs.ax = 0x0200;
  inregs.dx = port;
  D16rmInterrupt(0x14,&inregs,&outregs);
  if (outregs.ax & 0x8000)
    ret = -1;
  else
    ret = outregs.ax;
#else 	/* digiboard in real mode	    */
union REGS inregs,outregs;

  inregs.h.ah = 2;
  inregs.x.dx = (ST_UINT16) port;
  int86 (0x14,&inregs,&outregs);

  if (outregs.h.ah & 0x80)
    ret = -1;
  else
    ret = (ST_INT) (outregs.h.al);

#endif /* DOS16M not defined */
#endif /* ESSENTIAL_COM */

  return (ret);
  }


/************************************************************************/
/*                      sm_rx_flush                                     */
/* This function is used to flush out a comm port before transmitting   */
/************************************************************************/

ST_VOID sm_rx_flush (ST_INT port)
  {
ST_INT16 len;
ST_UCHAR packet[102];

  len = 0;
  while (sm_rx_cnt (port) && (len < 100))
    packet[len++] = (ST_UCHAR) sm_getc (port);

  if (len)
    {
    ADLC_LOG_SM1 ("DIGI Flush Com Port #%d:", port);
    ADLC_LOG_SM_HEX (len,packet);
    }
  }

/*-----------------------------------------------------------------------
 * Following are the functions to set or clear different control signals
 *----------------------------------------------------------------------*/

/************************************************************************/
/*                      sm_set_signals                                  */
/* This function sets the DTR and RTS signals high                  	*/
/************************************************************************/

ST_RET sm_set_signals (ST_INT port)
  {
ST_INT ret = SD_FAILURE;

#ifdef ESSENTIAL_COM
int ret1, ret2;

  ret1 = (ST_INT16) xc_dtr (port, 1);
  ret2 = (ST_INT16) xc_rts (port, 1);
  ret = SD_SUCCESS;  /* For control values 0 and 1 return value not applicable */
#else

#ifdef DOS16M /* digiboard in protected mode */
D16REGS inregs,outregs;
  inregs.ax = 0x1300;
  inregs.dx = port;
  D16rmInterrupt(0x14,&inregs,&outregs);
  if ((outregs.ax & 0xFF00) == 0)
    {
    ret = SD_SUCCESS;
    }

#else 	/* digiboard in real mode	    */
union REGS inregs,outregs;

  inregs.h.ah = 0x13;
  inregs.x.dx = (ST_UINT16) port;
  int86 (0x14,&inregs,&outregs);

  if (outregs.h.ah == 0)
    {
    ret = SD_SUCCESS;
    }
#endif /* DOS16M not defined */
#endif /* ESSENTIAL_COM */

  return (ret);
  }


/************************************************************************/
/*                      sm_clear_signals                                */
/* This function sets the DTR and RTS signals low                   	*/
/************************************************************************/

ST_RET sm_clear_signals (ST_INT port)
  {
ST_INT ret = SD_FAILURE;

#ifdef ESSENTIAL_COM
int ret1, ret2;

  ret1 = (ST_INT16) xc_dtr (port, 0);
  ret2 = (ST_INT16) xc_rts (port, 0);
  ret = SD_SUCCESS;  /* For control values 0 and 1 return value not applicable */
#else
#ifdef DOS16M /* digiboard in protected mode */
D16REGS inregs,outregs;
  inregs.ax = 0x0b00;
  inregs.dx = port;
  D16rmInterrupt(0x14,&inregs,&outregs);
  if ((outregs.ax & 0xFF00) == 0)
    {
    ret = SD_SUCCESS;
    }

#else 	/* digiboard in real mode	    */
union REGS inregs,outregs;

  inregs.h.ah = 0x0b;
  inregs.x.dx = (ST_UINT16) port;
  int86 (0x14,&inregs,&outregs);

  if (outregs.h.ah == 0)
    {
    ret = SD_SUCCESS;
    }
#endif /* DOS16M not defined */
#endif /* ESSENTIAL_COM */

  return (ret);
  }

/************************************************************************/
/*                      sm_set_rts                                  	*/
/* This function sets the RTS signal to raise                  		*/
/************************************************************************/

ST_RET sm_set_rts (ST_INT port)
  {
ST_INT ret = SD_FAILURE;

#ifdef ESSENTIAL_COM
int ret1;

  ret1 = (ST_INT16) xc_rts (port, 1);
  ret = SD_SUCCESS;  /* For control values 0 and 1 return value not applicable */
#else

#ifdef DOS16M /* digiboard in protected mode */
D16REGS inregs,outregs;
  inregs.ax = 0x0501;
  inregs.dx = port;
  inregs.bx = 0x0002; /* Set bit 1 of BL to 1 to raise RTS		*/
  D16rmInterrupt(0x14,&inregs,&outregs);
  if (outregs.bx & 0x0002) 
    {
    ret = SD_SUCCESS;
    }
#else
union REGS inregs,outregs;

  inregs.h.ah = 0x05;   /* Function Number				*/
  inregs.x.dx = (ST_UINT16) port;
  inregs.h.al = 0x01;   /* To write modem control register		*/
  inregs.h.bl = 0x02; /* Set bit 1 of BL to 1 to raise RTS		*/
  
  int86 (0x14,&inregs,&outregs);

  if (outregs.h.bl & 0x02)
    ret = SD_SUCCESS;
#endif /* DOS16M */
#endif /* ESSENTIAL_COM */

  return (ret);
  }

/************************************************************************/
/*                      sm_clear_rts                                  	*/
/* This function clears the RTS signal to drop                 		*/
/************************************************************************/

ST_RET sm_clear_rts (ST_INT port)
  {
ST_INT ret = SD_FAILURE;

#ifdef ESSENTIAL_COM
int ret1;

  ret1 = (ST_INT16) xc_rts (port, 0);
  ret = SD_SUCCESS;  /* For control values 0 and 1 return value not applicable */
#else

#ifdef DOS16M /* digiboard in protected mode */
D16REGS inregs,outregs;
  inregs.ax = 0x0501;
  inregs.dx = port;
  inregs.bx = 0x0000; 
  D16rmInterrupt(0x14,&inregs,&outregs);
  if ((outregs.bx & 0x0002) == 0x0000)
    {
    ret = SD_SUCCESS;
    }
#else
union REGS inregs,outregs;

  inregs.h.ah = 0x05;   /* Function Number				*/
  inregs.x.dx = (ST_UINT16) port;
  inregs.h.al = 0x01;   /* To write modem control register		*/
  inregs.h.bl = 0x00;	/* setting bit 1 of BL to 0 will drop RTS;	*/
  
  int86 (0x14,&inregs,&outregs);

  if ((outregs.h.bl & 0x02) == 0x00) /* Bit 1 should be set to 0	*/
    {
    ret = SD_SUCCESS;
    }
#endif /* DOS16M */
#endif /* ESSENTIAL_COM */

  return (ret);
  }

/************************************************************************/
/*                      sm_set_dtr                                  	*/
/* This function sets the DTR  signal to raise                 		*/
/************************************************************************/

ST_RET sm_set_dtr (ST_INT port)
  {
ST_INT ret = SD_FAILURE;

#ifdef ESSENTIAL_COM
ST_INT ret1;

  ret1 = (ST_INT16) xc_dtr (port, 1);
  ret = SD_SUCCESS;  /* For control values 0 and 1 return value not applicable */
#else

#ifdef DOS16M /* digiboard in protected mode */
D16REGS inregs,outregs;
  inregs.ax = 0x0501;
  inregs.dx = port;
  inregs.bx = 0x0001; 	/* Set bit 0 of BL to 1 to raise DTR		*/
  D16rmInterrupt(0x14,&inregs,&outregs);
  if (outregs.bx & 0x0001)
    {
    ret = SD_SUCCESS;
    }

#else
union REGS inregs,outregs;

  inregs.h.ah = 0x05;   /* Function Number				*/
  inregs.x.dx = (ST_UINT16) port;
  inregs.h.al = 0x01;   /* To write modem control register		*/
  inregs.h.bl = 0x01;	/* Set bit 0 of BL to 1 to raise DTR		*/
  
  int86 (0x14,&inregs,&outregs);

  if (outregs.h.bl & 0x01)
    ret = SD_SUCCESS;
#endif /* DOS16M */
#endif /* ESSENTIAL_COM */

  return (ret);
  }
/************************************************************************/
/*                      sm_clear_dtr                                  	*/
/* This function clears the DTR  signal to drop                		*/
/************************************************************************/

ST_RET sm_clear_dtr (ST_INT port)
  {
ST_INT ret = SD_FAILURE;

#ifdef ESSENTIAL_COM
ST_INT ret1;

  ret1 = (ST_INT16) xc_dtr (port, 0);
  ret = SD_SUCCESS;  /* For control values 0 and 1 return value not applicable */
#else
#ifdef DOS16M /* digiboard in protected mode */
D16REGS inregs,outregs;
  inregs.ax = 0x0501;
  inregs.dx = port;
  inregs.bx = 0x0000;	/* setting bit 0 of BL to 0 will drop DTR;	*/
  D16rmInterrupt(0x14,&inregs,&outregs);
  if ((outregs.bx & 0x0001) == 0x0000)
    {
    ret = SD_SUCCESS;
    }

#else
union REGS inregs,outregs;

  inregs.h.ah = 0x05;   /* Function Number				*/
  inregs.x.dx = (ST_UINT16) port;
  inregs.h.al = 0x01;   /* To write modem control register		*/
  inregs.h.bl = 0x00;	/* setting bit 0 of BL to 0 will drop DTR;	*/
  
  int86 (0x14,&inregs,&outregs);

  if ((outregs.h.bl & 0x01) == 0x00) /* Bit 0 should be set to 0		*/
    {
    ret = SD_SUCCESS;
    }
#endif /* DOS16M */
#endif /* ESSENTIAL_COM */

  return (ret);
  }
/************************************************************************/

/************************************************************************/
ST_RET sm_set_rts_clear_dtr(ST_INT port)
  {
#ifdef ESSENTIAL_COM
ST_INT ret1;
ST_INT ret2;

  ret1 = sm_set_rts(port);
  ret2 = sm_clear_dtr(port);
  return (ret1 || ret2);

#else  /* digiboard  */
  return (sm_set_rts(port));
#endif /* ESSENTIAL_COM */
  }
/************************************************************************/

/************************************************************************/
ST_RET sm_clear_rts_set_dtr(ST_INT port)
  {
#ifdef ESSENTIAL_COM
ST_INT ret1;
ST_INT ret2;

  ret1 = sm_clear_rts(port);
  ret2 = sm_set_dtr(port);
  return (ret1 || ret2);

#else  /* digiboard  */
  return (sm_set_dtr(port));
#endif /* ESSENTIAL_COM */
  }
/*-----------------------------------------------------------------------
 * Following are the functions to get status of different control signals
 *----------------------------------------------------------------------*/

/************************************************************************/

/************************************************************************/
ST_INT sm_get_rts (ST_INT port)
  {
ST_INT ret;
#if defined(ESSENTIAL_COM)
ST_INT ret1;
  ret1 = xc_rts(port,2);
  if (ret1 == -1)
    ret = HIGH;	
  else
    ret = LOW;

#else
#ifdef DOS16M /* digiboard in protected mode */
D16REGS inregs,outregs;
  inregs.ax = 0x0500;
  inregs.dx = port;
  D16rmInterrupt(0x14,&inregs,&outregs);
  if (outregs.bx & 0x0002)  /* Bit 1 should be set to 1 for RTS HIGH	*/
    ret = HIGH;
  else 
    ret = LOW;
#else
union REGS inregs,outregs;

  inregs.h.ah = 0x05;	/* Function Number				*/
  inregs.x.dx = (ST_UINT16) port;
  inregs.h.al = 0x00;	/* Read Modem Control Register			*/
  int86 (0x14,&inregs,&outregs);
  if (outregs.h.bl & 0x02) /* Bit 1 should be set to 1 for RTS HIGH	*/
    ret = HIGH;
  else 
    ret = LOW;
#endif /* DOS16M */
#endif /* ESSENTIAL_COM */
  return (ret);
  }
/************************************************************************/

/************************************************************************/
ST_INT sm_get_dtr (ST_INT port)
  {
ST_INT ret;
#if defined(ESSENTIAL_COM)
ST_INT ret1;
  ret1 = xc_dtr(port,2);
  if (ret1 == -1)
    ret = HIGH;	
  else
    ret = LOW;

#else
#ifdef DOS16M /* digiboard in protected mode */
D16REGS inregs,outregs;
  inregs.ax = 0x0500;
  inregs.dx = port;
  D16rmInterrupt(0x14,&inregs,&outregs);
  if (outregs.bx & 0x0001)
    ret = HIGH;
  else 
    ret = LOW;

#else
union REGS inregs,outregs;

  inregs.h.ah = 0x05;	/* Function Number				*/
  inregs.x.dx = (ST_UINT16) port;
  inregs.h.al = 0x00;	/* Read Modem Control Register			*/
  int86 (0x14,&inregs,&outregs);
  if (outregs.h.bl & 0x01)
    ret = HIGH;
  else 
    ret = LOW;
#endif
#endif /* ESSENTIAL_COM */
  return (ret);
  }
/************************************************************************/

/************************************************************************/
ST_INT sm_get_cts (ST_INT port)
  {
ST_INT ret = SD_FAILURE;
#ifdef ESSENTIAL_COM
ST_INT ret1;
    ret1 = xc_cts(port);
    if (ret1 == -1)
      ret = HIGH;
    else 
      ret = LOW;
#else
#ifdef DOS16M 	/* digiboard in protected mode */
D16REGS inregs,outregs;
  inregs.ax = 0x0300;
  inregs.dx = port;
  D16rmInterrupt(0x14,&inregs,&outregs);
  if (outregs.ax & 0x0010) 
    ret = HIGH;
  else
    ret = LOW;

#else 		/* digiboard in real mode 	*/
union REGS inregs,outregs;

  inregs.h.ah = 0x03; /* Function Number	*/
  inregs.x.dx = (ST_UINT16) port;
  int86(0x14,&inregs,&outregs);
  if (outregs.h.al & 0x10)
    ret = HIGH;
  else 
    ret = LOW;
#endif /* DOS16M */
#endif /* ESSENTIAL_COM */  
  return (ret);
  }
/************************************************************************/

/************************************************************************/
ST_INT sm_get_dsr (ST_INT port)
  {
ST_INT ret = SD_FAILURE;
#ifdef ESSENTIAL_COM
ST_INT ret1;
    ret1 = xc_dsr(port);
    if (ret1 == -1)
      ret = HIGH;
    else
      ret = LOW;
#else
#ifdef DOS16M /* digiboard in protected mode */
D16REGS inregs,outregs;
  inregs.ax = 0x0300;
  inregs.dx = port;
  D16rmInterrupt(0x14,&inregs,&outregs);
  if (outregs.ax & 0x0020) 
    ret = HIGH;
  else
    ret = LOW;

#else
union REGS inregs,outregs;

  inregs.h.ah = 0x03; /* Function Number	*/
  inregs.x.dx = (ST_UINT16) port;
  int86 (0x14,&inregs,&outregs);
  if (outregs.h.al & 0x20)
    ret = HIGH;
  else
    ret = LOW;
#endif /* DOS16M */
#endif /* ESSENTIAL_COM */  
  return (ret);
  }
/************************************************************************/

/************************************************************************/
ST_INT sm_get_dcd (ST_INT port)
  {
ST_INT ret = SD_FAILURE;
#ifdef ESSENTIAL_COM
ST_INT ret1;
    ret1 = xc_dcd(port);
    if (ret1 == -1)
      ret = HIGH;
    else
      ret = LOW;
#else
#ifdef DOS16M /* digiboard in protected mode */
D16REGS inregs,outregs;
  inregs.ax = 0x0300;
  inregs.dx = port;
  D16rmInterrupt(0x14,&inregs,&outregs);
  if (outregs.ax & 0x0080) 
    ret = HIGH;
  else
    ret = LOW;

#else
union REGS inregs,outregs;

  inregs.h.ah = 0x03; /* Function Number	*/
  inregs.x.dx = (ST_UINT16) port;
  int86 (0x14,&inregs,&outregs);
  if (outregs.h.al & 0x80)
    ret = HIGH;
  else
    ret = LOW;
#endif /* DOS16M */
#endif /* ESSENTIAL_COM */  
  return (ret);
  }
