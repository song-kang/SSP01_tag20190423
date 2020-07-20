/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,       */
/*          1995-2002, All Rights Reserved                              */
/*                                                                      */
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*                                                                      */
/* MODULE NAME : adlc_enc.c						*/
/* PRODUCT(S)  : ADLC			                             	*/
/*                                                                      */
/* MODULE DESCRIPTION :                                                 */
/* 	This module contains functions to encod the ADLC frame and	*/
/* send it to the serial device						*/
/*                                                                      */
/* GLOBAL VARIABLES DEFINED IN THIS MODULE :                            */
/*                                                                      */
/* MODIFICATION LOG :                                                   */
/*  Date     Who   Rev                     Comments                     */
/* --------  ---  ------   -------------------------------------------  */
/* 09/23/02  KCR    24     Cleaned up general PClint errors             */
/* 09/23/02  KCR    23     Removed extra include files                  */
/* 09/23/02  KCR    22     Moved thisFileName before includes           */
/* 09/13/99  MDE    21     Added SD_CONST modifiers         		*/
/* 10/07/97  IKE    20     Added support for UI frames for UCA_SMP	*/
/* 07/16/97  IKE    19     Added device statistics and device filt log	*/
/* 05/12/97  IKE    18     Changed to MMSEASE 7.0 Data Types		*/
/* 02/21/97  IKE    17     Changed preamble character from 0x55 to 0x00 */
/* 02/03/97  IKE    16     adlcCfg per port basis			*/
/* 01/17/97  IKE    15     Logging Enhancements				*/
/* 12/03/96  IKE    14     WIN32 Support, header files cleanup		*/ 
/* 09/27/96  IKE    13     Used sm_putc instead of sm_put in preamble   */
/* 09/17/96  IKE    12     Added premable chars				*/
/* 08/22/96  IKE    11     Support for contention avoidance		*/
/* 08/16/96  IKE    10     Changed the name from _ui_frame to mult_frame*/
/* 06/21/96  IKE    09     Changed ADLC_LOG_IO to ADLC_LOG_INFO_FRAME	*/
/* 06/26/96  IKE    08     Added GET_COM_NO to logging and use of 	*/
/*			   getLocalAddr and getRemoteAddr as member 	*/
/*			   function of class ADLC_CON_INFO		*/
/* 06/21/96  IKE    07     Changed ADLC_LOG_IO to ADLC_LOG_INFO_FRAME	*/
/* 06/11/96  IKE    06     Changed sm_put to SM_PUT 			*/
/* 05/22/96  IKE    05     Header file cleanup. Added generateErrors 	*/
/* 04/05/96  IKE    04     Change Logging from device osi terminology	*/
/* 			   to local remote terminology			*/
/* 04/05/96  rkr    02     Header file and SAVE_CALLING cleanup 	*/
/* 04/04/96  rkr    01	   Cleanup					*/
/* 10/25/95  IKE   1.00    Initial release                             	*/
/************************************************************************/
static char *thisFileName = __FILE__;
/* first glbtypes.h and sysincs.h and any other ANSI C header files	*/
#include "glbtypes.h"
#include "sysincs.h"

/* Some other SISCO files						*/
#include "cfg_util.h"
#include "memlog.h"

/* ADLC files								*/
#include "adlc.h"


#ifdef ADLC_TESTING
#include "fkeydefs.h"
#endif

/************************************************************************/
/*      Static Function Declarations                           		*/
/************************************************************************/
static ST_UCHAR getFrameType(ST_INT runMode,ST_UCHAR ctrlByte);
static ST_RET send_preamble_chars(ADLC_PORT_INFO *pptr);
#if defined(ADLC_ERROR)
static ST_VOID generateErrors(ST_UCHAR *frame, ST_INT len);
#endif
/************************************************************************/
/*		             send_info_frame				*/
/************************************************************************/
ST_RET send_info_frame(ADLC_FRAME *fptr,ST_BOOLEAN pfBit,ST_BOOLEAN eosBit)
  {
ST_UCHAR *frame = fptr->u.infoBuf;
ST_INT infoLen = fptr->infoLen;
ST_INT frameLen = fptr->frameLen;
ST_UINT16 frameFormat;
ST_UCHAR ctrlByte = 0x00;
ST_UCHAR frameType;
ADLC_CON_INFO *cptr = fptr->cptr;
ADLC_PORT_INFO *pptr = cptr->pptr;
ST_UCHAR vR = cptr->vR;
ST_UCHAR vS = cptr->vS;
ST_UINT16 srcAddr = getLocalAddr(cptr);
ST_UINT16 dstAddr = getRemoteAddr(cptr);
ST_CHAR tmpStr[20];

  adlcStats.txPackets ++;
  if (fptr->status == STAT_STANDBY)
    sprintf(tmpStr,"%s","Sending");
  else
    {	
    sprintf(tmpStr,"%s","Retransmitting");
    adlcStats.retransmits ++;
#if defined(ADLC_LM)
    cptr->dptr->devStats.retransmits ++;
#endif
    }
/* Prepare Control Byte							*/
  ctrlByte = fptr->cmd;
  if (pfBit)
    PF_SET(ctrlByte);
  INFO_NR_SET(ctrlByte,vR);
  INFO_NS_SET(ctrlByte,vS);

/* Prepare Frame Format		      */
  frameType = getFrameType(pptr->runMode,ctrlByte);
  frameFormat = getFrameFormat(frameLen,eosBit,frameType);

/* All the needed information is ready so do encoding			*/
  enc_adlc_frame(frame,srcAddr,dstAddr,ctrlByte,frameFormat,infoLen,pptr);

/* do logging								*/
  ADLC_LOG_IO4("%s %s frame from Local Addr = 0x%x to Remote Addr = 0x%x", 
  		tmpStr, getCommandName(ctrlByte),srcAddr,dstAddr);
  ADLC_LOG_CIO4("Ns = %d Nr = %d P/F Bit = %x EOS Bit = %x",INFO_NS_GET(ctrlByte),
  		 INFO_NR_GET(ctrlByte),pfBit ? 1 : 0,eosBit ? 1 : 0);
  ADLC_LOG_CIO2("Frame Type = 0x%x Port = COM%d",frameType,GET_COM_NO(pptr->port));

  ADLC_LOG_IO_HEX (frameLen,frame);
#if defined(ADLC_LM)
  ADLC_LOG_DEV_FILT4(cptr->dptr->address,"%s %s frame from Local Addr = 0x%x to Remote Addr = 0x%x", 
  		tmpStr, getCommandName(ctrlByte),srcAddr,dstAddr);
  ADLC_LOG_CDEV_FILT4(cptr->dptr->address,"Ns = %d Nr = %d P/F Bit = %x EOS Bit = %x",INFO_NS_GET(ctrlByte),
  		 INFO_NR_GET(ctrlByte),pfBit ? 1 : 0,eosBit ? 1 : 0);
  ADLC_LOG_CDEV_FILT2(cptr->dptr->address,"Frame Type = 0x%x Port = COM%d",frameType,GET_COM_NO(pptr->port));

  ADLC_LOG_DEV_FILT_HEX (cptr->dptr->address,frameLen,frame);
#endif
  if (send_preamble_chars(pptr) == SD_SUCCESS)
    return (SM_PUT (pptr->port,frameLen,(ST_CHAR *)frame));
  return (SD_FAILURE);
  }

/************************************************************************/
/*		             send_supr_frame				*/
/* Used for sending supervisory commands.  This is identical to send	*/
/* unum command except that this function will set the command type to	*/
/* include the proper value of R in the command.			*/
/*									*/
/************************************************************************/

ST_RET send_supr_frame(ADLC_FRAME *fptr)
  {
ADLC_CON_INFO *cptr = fptr->cptr;
ADLC_PORT_INFO *pptr = cptr->pptr;
ST_UCHAR *frame = fptr->u.suprBuf;
ST_INT frameLen = fptr->frameLen;
ST_UINT16 frameFormat;
ST_UCHAR ctrlByte = 0x00;
ST_UCHAR frameType;
ST_UCHAR vR = cptr->vR;
ST_UINT16 srcAddr = getLocalAddr(cptr); 
ST_UINT16 dstAddr = getRemoteAddr(cptr);

  memset(frame,0x00,MIN_ADLC_FRAME_LEN);

/* Prepare Control Byte							*/
  ctrlByte = fptr->cmd;
  SUPR_NR_SET(ctrlByte,vR);

/* Prepare Frame Format							*/
  frameType = getFrameType(pptr->runMode,ctrlByte);
  frameFormat = getFrameFormat(MIN_ADLC_FRAME_LEN,0,frameType);

/* All the needed information is ready so start encoding byte by byte	*/
  enc_adlc_frame(frame,srcAddr,dstAddr,ctrlByte,frameFormat,0,pptr);
  ADLC_LOG_IO3("Sending %s frame from Local Addr = 0x%x to Remote Addr = 0x%x",
  		getCommandName(ctrlByte),srcAddr,dstAddr);
  ADLC_LOG_CIO4("Nr = %d  P/F Bit = %x Frame Type = 0x%x Port = COM%d",
		 SUPR_NR_GET(ctrlByte),PF_GET(ctrlByte) ? 1 : 0,frameType,GET_COM_NO(pptr->port));
  ADLC_LOG_IO_HEX (frameLen,frame);

#if defined(ADLC_LM)
  ADLC_LOG_DEV_FILT3(cptr->dptr->address,"Sending %s frame from Local Addr = 0x%x to Remote Addr = 0x%x",
  		getCommandName(ctrlByte),srcAddr,dstAddr);
  ADLC_LOG_CDEV_FILT4(cptr->dptr->address,"Nr = %d  P/F Bit = %x Frame Type = 0x%x Port = COM%d",
		 SUPR_NR_GET(ctrlByte),PF_GET(ctrlByte) ? 1 : 0,frameType,GET_COM_NO(pptr->port));
  ADLC_LOG_DEV_FILT_HEX (cptr->dptr->address,frameLen,frame);
#endif
  if (send_preamble_chars(pptr) == SD_SUCCESS)
    return (SM_PUT (pptr->port,frameLen,(ST_CHAR *)frame));
  return (SD_FAILURE);
  }

/************************************************************************/
/*		              send_unum_frame				*/
/* Used for sending supervisory commands.  This is identical to send	*/
/* unum command except that this function will set the command type to	*/
/* include the proper value of R in the command.			*/
/*									*/
/************************************************************************/
ST_RET send_unum_frame(ADLC_FRAME *fptr)
  {
ADLC_CON_INFO *cptr = fptr->cptr;
ADLC_PORT_INFO *pptr = cptr->pptr;
ST_UCHAR *frame = fptr->u.unumBuf;
ST_INT frameLen = fptr->frameLen;
ST_UINT16 frameFormat;
ST_UCHAR ctrlByte = 0x00;
ST_UCHAR frameType;
ST_UINT16 srcAddr = getLocalAddr(cptr);
ST_UINT16 dstAddr = getRemoteAddr(cptr);

  memset(frame,0x00,MIN_ADLC_FRAME_LEN);

/* Prepare Control Byte							*/
  ctrlByte = fptr->cmd;

/* Prepare Frame Format							*/
  frameType = getFrameType(pptr->runMode,ctrlByte);
  frameFormat = getFrameFormat(MIN_ADLC_FRAME_LEN,0,frameType);

  enc_adlc_frame(frame,srcAddr,dstAddr,ctrlByte,frameFormat,0,pptr);
  ADLC_LOG_IO3("Sending %s frame from Local Addr = 0x%x to Remote Addr = 0x%x",
  		getCommandName(ctrlByte),srcAddr,dstAddr);
  ADLC_LOG_CIO3("P/F Bit = %x Frame Type = 0x%x Port = COM%d",PF_GET(ctrlByte) ? 1 : 0,
		 frameType,GET_COM_NO(pptr->port));
  ADLC_LOG_IO_HEX (frameLen,frame);

#if defined(ADLC_LM)
  ADLC_LOG_DEV_FILT3(cptr->dptr->address,"Sending %s frame from Local Addr = 0x%x to Remote Addr = 0x%x",
  		getCommandName(ctrlByte),srcAddr,dstAddr);
  ADLC_LOG_DEV_FILT3(cptr->dptr->address,"P/F Bit = %x Frame Type = 0x%x Port = COM%d",PF_GET(ctrlByte) ? 1 : 0,
		 frameType,GET_COM_NO(pptr->port));
  ADLC_LOG_DEV_FILT_HEX (cptr->dptr->address,frameLen,frame);
#endif
  if (send_preamble_chars(pptr) == SD_SUCCESS)
    return (SM_PUT (pptr->port,frameLen,(ST_CHAR *)frame));
  return (SD_FAILURE);
  }

/************************************************************************/
/*		              send_redr_frame				*/
/* redirect frames are those frames which came to the master but they	*/
/* were intended to go to other slaves so master will transmit back	*/
/* Warning : Do not do fptr->cptr					*/
/* We do not have connection for these frames so we can not extract cptr*/
/************************************************************************/
#if defined(ADLC_MASTER)
ST_RET send_redr_frame(ADLC_FRAME *fptr,ADLC_PORT_INFO *pptr)
  {
ST_UCHAR *frame = fptr->u.redrBuf;
ST_INT frameLen = fptr->frameLen;
ST_UCHAR ctrlByte;
ST_UCHAR frameType;
ST_UINT16 srcAddr;
ST_UINT16 dstAddr;

  ctrlByte = CTRL_BYTE_GET(frame);
  srcAddr = SRC_ADDR_GET(frame);
  dstAddr = DST_ADDR_GET(frame);
  frameType = FRAME_TYPE_GET(frame);
  ADLC_LOG_IO3("Sending Redirect %s frame from  Src Addr = 0x%x to Dst Addr = 0x%x",
  		getCommandName(ctrlByte),srcAddr,dstAddr);
  ADLC_LOG_CIO2("Frame Type = 0x%x Port = COM%d",frameType,GET_COM_NO(pptr->port));
  ADLC_LOG_IO_HEX (frameLen,frame);

  if (send_preamble_chars(pptr) == SD_SUCCESS)
    return (SM_PUT (pptr->port,frameLen,(ST_CHAR *)frame));
  return (SD_FAILURE);
  }
#endif /* ADLC_MASTER */

/************************************************************************/
/*		             send_mult_frame				*/
/* These frames are for broadcast and multicast addresses		*/
/* Warning : Do not do fptr->cptr					*/
/* We do not have connection for these frames so we can not extract cptr*/
/************************************************************************/
ST_RET send_mult_frame(ADLC_FRAME *fptr,ADLC_PORT_INFO *pptr)
  {
ST_UCHAR *frame = fptr->u.infoBuf;
ST_INT frameLen = fptr->frameLen;
ST_UCHAR ctrlByte;
ST_UINT16 srcAddr;
ST_UINT16 dstAddr;
ST_UCHAR frameType;
ST_UINT16 frameFormat;
ST_INT infoLen = fptr->infoLen;

  ctrlByte = CTRL_BYTE_GET(frame);
  srcAddr = SRC_ADDR_GET(frame);
  dstAddr = DST_ADDR_GET(frame);
  frameFormat = FRAME_FORMAT_GET(frame);
  frameType = getFrameType(pptr->runMode,ctrlByte);
  FRAME_TYPE_SET(frameFormat,frameType);

  enc_adlc_frame(frame,srcAddr,dstAddr,ctrlByte,frameFormat,infoLen,pptr);

  ADLC_LOG_IO3("Sending %s frame from  Local Addr = 0x%x to Remote Addr = 0x%x",
  		getCommandName(ctrlByte),srcAddr,dstAddr);
  ADLC_LOG_CIO4("Ns = %d Nr = %d P/F Bit = %x EOS Bit = %x",INFO_NS_GET(ctrlByte),
  		 INFO_NR_GET(ctrlByte),PF_GET(ctrlByte) ? 1 : 0,EOS_GET(frame) ? 1 : 0);
  ADLC_LOG_CIO2("Frame Type = 0x%x Port = COM%d",frameType,GET_COM_NO(pptr->port));
  ADLC_LOG_IO_HEX (frameLen,frame);

  if (send_preamble_chars(pptr) == SD_SUCCESS)
    return (SM_PUT (pptr->port,frameLen,(ST_CHAR *)frame));
  return (SD_FAILURE);
  }

/************************************************************************/
/*			enc_adlc_frame					*/
/* All the information we need to encode ADLC frame in the arguments	*/
/* Valid Frame Format for supervisory and unnumbered is following	*/
/* SOF(1) TYPE+EOS+LEN(2) DST_ADDR(2) SRC_ADDR(2) CTRL(1) CRC(2) EOF(1)	*/
/* Valid Frame Format for Info frame is following			*/
/* SOF(1) TYPE+EOS+LEN(2) DST_ADDR(2) SRC_ADDR(2) CTRL(1) CRC(2) INFO(?)*/
/* CRC(2) EOF(1)							*/
/************************************************************************/
ST_VOID enc_adlc_frame(ST_UCHAR *frame,ST_UINT16 srcAddr,ST_UINT16 dstAddr,ST_UCHAR ctrlByte,
		    ST_UINT16 frameFormat,ST_INT infoLen, ADLC_PORT_INFO *pptr)
  {
ST_UINT16 crc1 = 0x0000;
ST_UINT16 crc2 = 0x0000;
ST_INT len;

/* All the needed information is available so start encoding byte by byte*/
  len = 0;
  frame[len++] = ADLC_SOF(pptr);
  frame[len++] = (ST_UCHAR) (frameFormat >> 8);	      	/* high byte	*/
  frame[len++] = (ST_UCHAR) frameFormat;	      		/* low byte	*/
  frame[len++] = (ST_UCHAR) (dstAddr >> 8);		/* high byte	*/
  frame[len++] = (ST_UCHAR) dstAddr;	  		/* low byte	*/
  frame[len++] = (ST_UCHAR) (srcAddr >> 8);		/* high byte	*/
  frame[len++] = (ST_UCHAR) srcAddr;	  		/* low byte	*/
  frame[len++] = ctrlByte;
  crc1 = calcCrc(&frame[FORMAT_FIELD_OFFSET], (ST_UINT) len - SOF_FIELD_LEN);
  frame[len++] = (ST_UCHAR) (crc1 >> 8);			/* high byte	*/
  frame[len++] = (ST_UCHAR) crc1;				/* low byte	*/
  if (infoLen)	/* it is info frame data is already copied to proper	*/
    {		/* place in the frame. and we need one more crc		*/
    len += infoLen;
    crc2 = calcCrc(&frame[FORMAT_FIELD_OFFSET], (ST_UINT) len - SOF_FIELD_LEN);
    frame[len++] = (ST_UCHAR) (crc2 >> 8);			/* high byte	*/
    frame[len++] = (ST_UCHAR) crc2;			/* low byte	*/
    }
  frame[len++] = ADLC_EOF(pptr);

#if defined(ADLC_ERROR)
  generateErrors(frame,len);
#endif /* ADLC_ERROR */
  }

/************************************************************************/
/*			getFrameFormat					*/
/* Prepare Frame Format							*/
/* frame format consists of 16 bits					*/
/* frame format = ttttelllllllllll					*/
/* where t = type, e = eosBit, l = len					*/
/************************************************************************/
ST_UINT16 getFrameFormat(ST_INT frameLen,ST_BOOLEAN eosBit,ST_UCHAR frameType)
  {
ST_UINT16 frameFormat = 0x0800; /* by default not end of segment	*/

  FRAME_TYPE_SET(frameFormat,frameType);
  FRAME_LEN_SET(frameFormat,frameLen - EOF_FIELD_LEN); 
  if (eosBit == 0)
    EOS_SET(frameFormat);
  return (frameFormat);  
  }
/************************************************************************/
/* runMode is used for slave and ctrlbyte is used for master		*/
/************************************************************************/
static ST_UCHAR getFrameType(ST_INT runMode,ST_UCHAR ctrlByte)
  {
#if defined(ADLC_SLAVE)

  if (runMode == CONTENTION_MODE)
    return (FRAME_FORMAT_TYPE4);
  return (FRAME_FORMAT_TYPE3);

#endif /* ADLC_SLAVE */

#if defined(ADLC_MASTER)

ST_UCHAR ret = FRAME_FORMAT_TYPE3;

  switch (eventTypeOf(ctrlByte)) 
    {
    case RR :
    case RNR :
    case REJ :
      if (PF_GET(ctrlByte) == SD_FALSE)
        ret = FRAME_FORMAT_TYPE4;
    break;

    default :
    break;
    }
  return (ret);
#endif /* ADLC_MASTER */
  }
/************************************************************************/

/************************************************************************/
static ST_RET send_preamble_chars(ADLC_PORT_INFO *pptr)
  {
#if defined(ADLC_MASTER)
ST_CHAR packet[100];
ST_INT len;

  if (ADLC_PREAMBLE_LEN(pptr) == 0)
    return(SD_SUCCESS);

  if (ADLC_PREAMBLE_LEN(pptr) < 100)
    len = ADLC_PREAMBLE_LEN(pptr);
  else
    len = 100;

  memset(packet,0x00,len);
  return SM_PUT(pptr->port,len,packet);
#endif /* ADLC_MASTER */

#if defined(ADLC_SLAVE)
register ST_INT i;

  if (ADLC_PREAMBLE_LEN(pptr) == 0)
    return(SD_SUCCESS);

  for (i=0;i<ADLC_PREAMBLE_LEN(pptr);i++)
    sm_putc(pptr->port,0x00);

  return (SD_SUCCESS);
#endif /* ADLC_SLAVE */
  }
#if defined(UCA_SMP)
/************************************************************************/
/*		             send_ui_frame				*/
/* for ui frames 							*/
/************************************************************************/
ST_RET send_ui_frame(ADLC_FRAME *fptr)
  {
ST_UCHAR *frame = fptr->u.infoBuf;
ST_INT infoLen = fptr->infoLen;
ST_INT frameLen = fptr->frameLen;
ST_UINT16 frameFormat;
ST_UCHAR ctrlByte = 0x00;
ST_UCHAR frameType;
ADLC_CON_INFO *cptr = fptr->cptr;
ADLC_PORT_INFO *pptr = cptr->pptr;
ST_UINT16 srcAddr = getLocalAddr(cptr);
ST_UINT16 dstAddr = getRemoteAddr(cptr);

/* Prepare Control Byte							*/
  ctrlByte = fptr->cmd;

/* Prepare Frame Format							*/
  frameType = getFrameType(pptr->runMode,ctrlByte);
  frameFormat = getFrameFormat(frameLen,0,frameType);

  enc_adlc_frame(frame,srcAddr,dstAddr,ctrlByte,frameFormat,infoLen,pptr);
  ADLC_LOG_IO3("Sending %s frame from Local Addr = 0x%x to Remote Addr = 0x%x",
  		getCommandName(ctrlByte),srcAddr,dstAddr);
  ADLC_LOG_CIO3("P/F Bit = %x Frame Type = 0x%x Port = COM%d",PF_GET(ctrlByte) ? 1 : 0,
		 frameType,GET_COM_NO(pptr->port));
  ADLC_LOG_IO_HEX (frameLen,frame);

#if defined(ADLC_LM)
  ADLC_LOG_DEV_FILT3(cptr->dptr->address,"Sending %s frame from Local Addr = 0x%x to Remote Addr = 0x%x",
  		getCommandName(ctrlByte),srcAddr,dstAddr);
  ADLC_LOG_DEV_FILT3(cptr->dptr->address,"P/F Bit = %x Frame Type = 0x%x Port = COM%d",PF_GET(ctrlByte) ? 1 : 0,
		 frameType,GET_COM_NO(pptr->port));
  ADLC_LOG_DEV_FILT_HEX (cptr->dptr->address,frameLen,frame);
#endif
  if (send_preamble_chars(pptr) == SD_SUCCESS)
    return (SM_PUT (pptr->port,frameLen,(ST_CHAR *)frame));
  return (SD_FAILURE);
  }
#endif /* UCA_SMP */

