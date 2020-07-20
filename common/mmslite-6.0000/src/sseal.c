/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  2008 - 2009, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : sseal.c						*/
/* PRODUCT(S)  : 							*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 02/03/10  NAV     07    OpenVMS port					*/
/* 05/15/09  EJV     06    Elim warn: mv thisFileName to _S_TEST_PROBES.*/
/* 04/18/09  MDE     05    Added thisFileName				*/
/* 01/18/09  MDE     04    Made seal time 32 bit			*/
/* 01/05/09  RKR     03    removed thisFileName                         */
/* 12/31/08  MDE     02    Linux compile corrections			*/
/* 09/23/08  MDE     01    New						*/
/************************************************************************/


#include "glbtypes.h"
#include "sysincs.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <stdlib.h>
#include <ctype.h>

#include "sseal.h"
#include "slog.h"

#if defined(_S_TEST_PROBES)
#include "stestprb.h"

#ifdef DEBUG_SISCO
static ST_CHAR *thisFileName = __FILE__;
#endif
#endif

/************************************************************************/
/************************************************************************/
/* SKIPJACK Encryption/Decryption 					*/
/************************************************************************/

#define SSEAL_NUM_INTERNAL_KEYS	10
static SJ_KEY sSealInternalKeys[SSEAL_NUM_INTERNAL_KEYS] = 
  {
    {0xd7,0x7b,0x83,0x9d,0x4e,0x3c,0x3f,0xba,0xbe,0x7f},
    {0x05,0xcf,0x81,0x02,0x89,0x4f,0x5a,0x88,0x9a,0xa5},
    {0x31,0xee,0x99,0x26,0x02,0xc4,0xa4,0xc8,0x8f,0x69},
    {0x54,0xde,0x6c,0xcf,0x06,0xe8,0x81,0x42,0x21,0x23},
    {0x80,0x4c,0xe2,0xfa,0xfd,0xb9,0x73,0x09,0x30,0xc1},
    {0x79,0xba,0xea,0x47,0xe2,0xc1,0xea,0x56,0x83,0x07},
    {0x15,0x38,0x5c,0x1d,0x1a,0xd0,0x23,0x09,0xbd,0x99},
    {0xd4,0xd4,0x30,0xeb,0x6d,0x30,0xa9,0xf6,0x98,0x33},
    {0xc5,0x81,0xf3,0xf3,0xc8,0x68,0x26,0x7f,0x4a,0xf3},
    {0xf1,0xf1,0xf0,0x69,0x14,0x3e,0xc0,0x7d,0xf5,0x55},
  };

typedef unsigned char	BYTE; /* 8 bits */
typedef unsigned short	WORD; /* 16 bits */

static void skip32 (SJ_KEY key, BYTE buf[4], int encrypt);

/************************************************************************/
/************************************************************************/
/*			sMakeSeal					*/
/************************************************************************/

ST_RET sMakeSeal  (ST_INT32 keySel, ST_INT numKeys, SJ_KEY *keyArray, 
		   ST_UINT32 dataCrc, S_SEAL *destSeal)
  {
ST_UINT32 crc32;

/* See if using the internal keys */
  if (keyArray == NULL)
    {
    numKeys = SSEAL_NUM_INTERNAL_KEYS;
    keyArray = sSealInternalKeys;
    }

/* Make sure the key is in range */
  if (keySel >= numKeys)
    return (S_SEAL_KEYSEL);

/* Use the current time + user supplied kelSel & dataCrc */
  destSeal->currTime = (ST_INT32) time (NULL);
  destSeal->keySel = keySel;
  destSeal->dataCrc = dataCrc;

#if defined(_S_TEST_PROBES)
  if (_sTestProbeEnabledS("SSEAL_SendExpiredTimeSeal"))
    {
    SLOGALWAYS0 ("TEST: Generating expired time seal");
    destSeal->currTime += 10000; /* Should be enough! */
    }
#endif

/* Do forward & reverse 32 bit CRC's */
  crc32 = SSEAL_LIB_INITIAL_CRC32;
  sBufCrc32 (&destSeal->dataCrc, sizeof (destSeal->dataCrc), &crc32, SD_TRUE);
  sBufCrc32 (&destSeal->currTime, sizeof (destSeal->currTime), &crc32, SD_TRUE);
  sBufCrc32 (&destSeal->keySel, sizeof (destSeal->keySel), &crc32, SD_TRUE);
  destSeal->crc[0] = crc32;

  crc32 = SSEAL_LIB_INITIAL_CRC32;
  sBufCrc32 (&destSeal->dataCrc, sizeof (destSeal->dataCrc), &crc32, SD_FALSE);
  sBufCrc32 (&destSeal->currTime, sizeof (destSeal->currTime), &crc32, SD_FALSE);
  sBufCrc32 (&destSeal->keySel, sizeof (destSeal->keySel), &crc32, SD_FALSE);
  destSeal->crc[1] = crc32;

/* Now encrypt the two CRC's */
  sSkipjackBuffer (keySel, numKeys, keyArray, sizeof (destSeal->crc), destSeal->crc, destSeal->eCrc, 1);
  return (SD_SUCCESS);
  }

/************************************************************************/
/*			sCheckSeal					*/
/************************************************************************/

ST_RET sCheckSeal (S_SEAL *srcSeal, ST_INT numKeys, SJ_KEY *keyArray, ST_UINT32 dataCrc, ST_INT32 timeWindow)
  {
ST_INT32 currTime;
ST_INT32 et;
ST_UINT32 decryptedCrc[2];   
ST_UINT32 calculatedCrc[2];

/* See if using the internal keys */
  if (keyArray == NULL)
    {
    numKeys = SSEAL_NUM_INTERNAL_KEYS;
    keyArray = sSealInternalKeys;
    }

/* Make sure the key is in range */
  if (srcSeal->keySel >= numKeys)
    return (S_SEAL_KEYSEL);

/* Decrypt the CRC, make sure it matches */
  sSkipjackBuffer (srcSeal->keySel, numKeys, keyArray, sizeof (srcSeal->crc), srcSeal->eCrc, decryptedCrc, 0);
  if ((srcSeal->crc[0] != decryptedCrc[0]) || (srcSeal->crc[1] != decryptedCrc[1]))
    return (S_SEAL_ECRC_MISMATCH);

/* OK, now do our own CRC checks */
  calculatedCrc[0] = SSEAL_LIB_INITIAL_CRC32;
  sBufCrc32 (&srcSeal->dataCrc, sizeof (srcSeal->dataCrc), &calculatedCrc[0], SD_TRUE);
  sBufCrc32 (&srcSeal->currTime, sizeof (srcSeal->currTime), &calculatedCrc[0], SD_TRUE);
  sBufCrc32 (&srcSeal->keySel, sizeof (srcSeal->keySel), &calculatedCrc[0], SD_TRUE);

  calculatedCrc[1] = SSEAL_LIB_INITIAL_CRC32;
  sBufCrc32 (&srcSeal->dataCrc, sizeof (srcSeal->dataCrc), &calculatedCrc[1], SD_FALSE);
  sBufCrc32 (&srcSeal->currTime, sizeof (srcSeal->currTime), &calculatedCrc[1], SD_FALSE);
  sBufCrc32 (&srcSeal->keySel, sizeof (srcSeal->keySel), &calculatedCrc[1], SD_FALSE);

  if ((srcSeal->crc[0] != calculatedCrc[0]) || (srcSeal->crc[1] != calculatedCrc[1]))
    return (S_SEAL_CRC_MISMATCH);

/* Check the dataCrc */
  if (srcSeal->dataCrc != dataCrc)
    return (S_SEAL_DATA_CRC_MISMATCH);

/* OK, is the time OK? */
  currTime = (ST_INT32) time (NULL);
  et = currTime - srcSeal->currTime;
  if (et < 0)
    et *= -1;

  if (et > timeWindow)
    return (S_SEAL_TIME_WINDOW);

  return (SD_SUCCESS);
  }

/************************************************************************/
/*			sSealKeyStir					*/
/************************************************************************/

ST_VOID sSealKeyStir (ST_INT numKeys, SJ_KEY *keyArray, ST_INT sx, ST_INT sy)
  {
ST_UCHAR x;
ST_INT i,j;

  if (sx == 0)
    sx = numKeys > 4 ? 3 : numKeys-1;

  if (sy == 0)
    sy = 2;

  for (i = 0; i < numKeys; ++i)
    {
    for (j = 0; j < 10; ++j)
      {
      x = keyArray[sx][sy];
      keyArray[i][j] ^= x;
      ++sx;
      if (sx >= 10)
        {
        sx = 0;
	++sy;
        if (sy >= numKeys)
          sy = 0;
        }
      }
    }
  }

/************************************************************************/
/************************************************************************/
/*			sSkipjack32					*/
/************************************************************************/

ST_RET sSkipjack32 (ST_INT keySel, ST_INT numKeys, SJ_KEY *keyArray, 
		    ST_INT *dataIn, ST_INT *dataOut, ST_BOOLEAN encrypt)
  {
ST_INT workingData;
ST_UCHAR *key;

/* See if using the internal keys */
  if (keyArray == NULL)
    {
    numKeys = SSEAL_NUM_INTERNAL_KEYS;
    keyArray = sSealInternalKeys;
    }
  if (keySel >= numKeys)
    return (S_SEAL_KEYSEL);

/* Easier if encrypting/decrypting in place */
  key = keyArray[keySel];
  if (dataIn == dataOut)
    skip32 (key, (BYTE *) dataIn, encrypt);
  else
    {
    workingData = *dataIn;
    skip32 (key, (BYTE *) &workingData, encrypt);
    *dataOut = workingData;
    }
  return (SD_SUCCESS);
  }

/************************************************************************/
/*			sSkipjackBuffer					*/
/************************************************************************/

ST_RET sSkipjackBuffer (ST_INT keySel, ST_INT numKeys, SJ_KEY *keyArray, 
			ST_INT dataLen, ST_VOID *inBuf, ST_VOID *outBuf, ST_BOOLEAN encrypt)
  {
ST_INT *dataIn; 
ST_INT *dataOut;
ST_INT numInt; 
ST_INT i;
ST_RET rc;

/* See if using the internal keys */
  if (keyArray == NULL)
    {
    numKeys = SSEAL_NUM_INTERNAL_KEYS;
    keyArray = sSealInternalKeys;
    }

  dataIn = (ST_INT *) inBuf; 
  dataOut = (ST_INT *) outBuf;

/* We need 4 byte chunks */
  if (dataLen % 4)
    return (S_SEAL_BUFSIZE_REMAINDER);

  numInt = dataLen/sizeof(int); 
  for (i = 0; i < numInt; ++i, ++dataIn, ++dataOut)
    {
    rc = sSkipjack32 (keySel, numKeys, keyArray, dataIn, dataOut, encrypt);
    if (rc != SD_SUCCESS)
      return (rc);

  /* Rotate through keys */
    ++keySel;
    if (keySel >= numKeys)
      keySel = 0;
    }

  return (SD_SUCCESS);
  }


/************************************************************************/
/************************************************************************/
/* 
   SKIP32 -- 32 bit block cipher based on SKIPJACK.
   Written by Greg Rose, QUALCOMM Australia, 1999/04/27.

   In common: F-table, G-permutation, key schedule.
   Different: 24 round feistel structure.
   Based on:  Unoptimized test implementation of SKIPJACK algorithm 
              Panu Rissanen <bande@lut.fi>

   SKIPJACK and KEA Algorithm Specifications 
   Version 2.0 
   29 May 1998

   Not copyright, no rights reserved.
*/

static const BYTE ftable[256] = { 
0xa3,0xd7,0x09,0x83,0xf8,0x48,0xf6,0xf4,0xb3,0x21,0x15,0x78,0x99,0xb1,0xaf,0xf9,
0xe7,0x2d,0x4d,0x8a,0xce,0x4c,0xca,0x2e,0x52,0x95,0xd9,0x1e,0x4e,0x38,0x44,0x28,
0x0a,0xdf,0x02,0xa0,0x17,0xf1,0x60,0x68,0x12,0xb7,0x7a,0xc3,0xe9,0xfa,0x3d,0x53,
0x96,0x84,0x6b,0xba,0xf2,0x63,0x9a,0x19,0x7c,0xae,0xe5,0xf5,0xf7,0x16,0x6a,0xa2,
0x39,0xb6,0x7b,0x0f,0xc1,0x93,0x81,0x1b,0xee,0xb4,0x1a,0xea,0xd0,0x91,0x2f,0xb8,
0x55,0xb9,0xda,0x85,0x3f,0x41,0xbf,0xe0,0x5a,0x58,0x80,0x5f,0x66,0x0b,0xd8,0x90,
0x35,0xd5,0xc0,0xa7,0x33,0x06,0x65,0x69,0x45,0x00,0x94,0x56,0x6d,0x98,0x9b,0x76,
0x97,0xfc,0xb2,0xc2,0xb0,0xfe,0xdb,0x20,0xe1,0xeb,0xd6,0xe4,0xdd,0x47,0x4a,0x1d,
0x42,0xed,0x9e,0x6e,0x49,0x3c,0xcd,0x43,0x27,0xd2,0x07,0xd4,0xde,0xc7,0x67,0x18,
0x89,0xcb,0x30,0x1f,0x8d,0xc6,0x8f,0xaa,0xc8,0x74,0xdc,0xc9,0x5d,0x5c,0x31,0xa4,
0x70,0x88,0x61,0x2c,0x9f,0x0d,0x2b,0x87,0x50,0x82,0x54,0x64,0x26,0x7d,0x03,0x40,
0x34,0x4b,0x1c,0x73,0xd1,0xc4,0xfd,0x3b,0xcc,0xfb,0x7f,0xab,0xe6,0x3e,0x5b,0xa5,
0xad,0x04,0x23,0x9c,0x14,0x51,0x22,0xf0,0x29,0x79,0x71,0x7e,0xff,0x8c,0x0e,0xe2,
0x0c,0xef,0xbc,0x72,0x75,0x6f,0x37,0xa1,0xec,0xd3,0x8e,0x62,0x8b,0x86,0x10,0xe8,
0x08,0x77,0x11,0xbe,0x92,0x4f,0x24,0xc5,0x32,0x36,0x9d,0xcf,0xf3,0xa6,0xbb,0xac,
0x5e,0x6c,0xa9,0x13,0x57,0x25,0xb5,0xe3,0xbd,0xa8,0x3a,0x01,0x05,0x59,0x2a,0x46
};

/************************************************************************/

static WORD g(SJ_KEY key, int k, WORD w)
{
    BYTE g1, g2, g3, g4, g5, g6;

    g1 = (w>>8)&0xff;
    g2 = w&0xff;

    g3 = ftable[g2 ^ key[(4*k)%10]] ^ g1;
    g4 = ftable[g3 ^ key[(4*k+1)%10]] ^ g2;
    g5 = ftable[g4 ^ key[(4*k+2)%10]] ^ g3;
    g6 = ftable[g5 ^ key[(4*k+3)%10]] ^ g4;

    return ((g5<<8) + g6);
}

/************************************************************************/

static void skip32 (SJ_KEY key, BYTE buf[4], int encrypt)
{
    int		k; /* round number */
    int		i; /* round counter */
    int		kstep;
    WORD	wl, wr;

    /* sort out direction */
    if (encrypt)
	kstep = 1, k = 0;
    else
	kstep = -1, k = 23;

    /* pack into words */
    wl = (buf[0] << 8) + buf[1];
    wr = (buf[2] << 8) + buf[3];

    /* 24 feistel rounds, doubled up */
    for (i = 0; i < 24/2; ++i) {
	wr ^= g(key, k, wl) ^ k;
	k += kstep;
	wl ^= g(key, k, wr) ^ k;
	k += kstep;
    }

    /* implicitly swap halves while unpacking */
    buf[0] = wr >> 8;	buf[1] = wr & 0xFF;
    buf[2] = wl >> 8;	buf[3] = wl & 0xFF;
}


/************************************************************************/
/************************************************************************/
/*			sBufCrc32					*/
/************************************************************************/
/*
  IEEE-802.3 32bit CRC
  --------------------
*/

static const unsigned int crc32table[256] = {
                   /* 0x00 */ 0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA,
                   /* 0x04 */ 0x076DC419, 0x706AF48F, 0xE963A535, 0x9E6495A3,
                   /* 0x08 */ 0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988,
                   /* 0x0C */ 0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91,
                   /* 0x10 */ 0x1DB71064, 0x6AB020F2, 0xF3B97148, 0x84BE41DE,
                   /* 0x14 */ 0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7,
                   /* 0x18 */ 0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC,
                   /* 0x1C */ 0x14015C4F, 0x63066CD9, 0xFA0F3D63, 0x8D080DF5,
                   /* 0x20 */ 0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172,
                   /* 0x24 */ 0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B,
                   /* 0x28 */ 0x35B5A8FA, 0x42B2986C, 0xDBBBC9D6, 0xACBCF940,
                   /* 0x2C */ 0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,
                   /* 0x30 */ 0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116,
                   /* 0x34 */ 0x21B4F4B5, 0x56B3C423, 0xCFBA9599, 0xB8BDA50F,
                   /* 0x38 */ 0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924,
                   /* 0x3C */ 0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D,
                   /* 0x40 */ 0x76DC4190, 0x01DB7106, 0x98D220BC, 0xEFD5102A,
                   /* 0x44 */ 0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433,
                   /* 0x48 */ 0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818,
                   /* 0x4C */ 0x7F6A0DBB, 0x086D3D2D, 0x91646C97, 0xE6635C01,
                   /* 0x50 */ 0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E,
                   /* 0x54 */ 0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457,
                   /* 0x58 */ 0x65B0D9C6, 0x12B7E950, 0x8BBEB8EA, 0xFCB9887C,
                   /* 0x5C */ 0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,
                   /* 0x60 */ 0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2,
                   /* 0x64 */ 0x4ADFA541, 0x3DD895D7, 0xA4D1C46D, 0xD3D6F4FB,
                   /* 0x68 */ 0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0,
                   /* 0x6C */ 0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9,
                   /* 0x70 */ 0x5005713C, 0x270241AA, 0xBE0B1010, 0xC90C2086,
                   /* 0x74 */ 0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
                   /* 0x78 */ 0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4,
                   /* 0x7C */ 0x59B33D17, 0x2EB40D81, 0xB7BD5C3B, 0xC0BA6CAD,
                   /* 0x80 */ 0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A,
                   /* 0x84 */ 0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683,
                   /* 0x88 */ 0xE3630B12, 0x94643B84, 0x0D6D6A3E, 0x7A6A5AA8,
                   /* 0x8C */ 0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1,
                   /* 0x90 */ 0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE,
                   /* 0x94 */ 0xF762575D, 0x806567CB, 0x196C3671, 0x6E6B06E7,
                   /* 0x98 */ 0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC,
                   /* 0x9C */ 0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5,
                   /* 0xA0 */ 0xD6D6A3E8, 0xA1D1937E, 0x38D8C2C4, 0x4FDFF252,
                   /* 0xA4 */ 0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,
                   /* 0xA8 */ 0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60,
                   /* 0xAC */ 0xDF60EFC3, 0xA867DF55, 0x316E8EEF, 0x4669BE79,
                   /* 0xB0 */ 0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236,
                   /* 0xB4 */ 0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F,
                   /* 0xB8 */ 0xC5BA3BBE, 0xB2BD0B28, 0x2BB45A92, 0x5CB36A04,
                   /* 0xBC */ 0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,
                   /* 0xC0 */ 0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A,
                   /* 0xC4 */ 0x9C0906A9, 0xEB0E363F, 0x72076785, 0x05005713,
                   /* 0xC8 */ 0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38,
                   /* 0xCC */ 0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21,
                   /* 0xD0 */ 0x86D3D2D4, 0xF1D4E242, 0x68DDB3F8, 0x1FDA836E,
                   /* 0xD4 */ 0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777,
                   /* 0xD8 */ 0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C,
                   /* 0xDC */ 0x8F659EFF, 0xF862AE69, 0x616BFFD3, 0x166CCF45,
                   /* 0xE0 */ 0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2,
                   /* 0xE4 */ 0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB,
                   /* 0xE8 */ 0xAED16A4A, 0xD9D65ADC, 0x40DF0B66, 0x37D83BF0,
                   /* 0xEC */ 0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
                   /* 0xF0 */ 0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6,
                   /* 0xF4 */ 0xBAD03605, 0xCDD70693, 0x54DE5729, 0x23D967BF,
                   /* 0xF8 */ 0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94,
                   /* 0xFC */ 0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D,
                   };

ST_VOID sBufCrc32 (ST_VOID *vBuf, ST_INT numBytes, ST_UINT32 *crcInOut, ST_BOOLEAN backwards)
  {
unsigned int crc;
unsigned char *buf;

  buf = (ST_UCHAR *) vBuf;
  crc = *crcInOut;

  if (!backwards)
    {
    for (; (numBytes > 0); numBytes--)
      {
      crc = (((crc >> 8) & 0x00FFFFFF) ^
             crc32table[(crc ^ *buf++) & 0x000000FF]);
      }
    }
  else
    {
    buf += numBytes-1;
    for (; (numBytes > 0); numBytes--)
      {
      crc = (((crc >> 8) & 0x00FFFFFF) ^
             crc32table[(crc ^ *buf--) & 0x000000FF]);
      }
    }
  *crcInOut = crc;
  }

/************************************************************************/

ST_VOID sIntCrc32 (ST_INT val, ST_UINT32 *crcInOut)
  {
  sBufCrc32 (&val, sizeof(int), crcInOut, SD_FALSE);
  }

ST_VOID sStrCrc32 (ST_CHAR *str, ST_UINT32 *crcInOut)
  {
  sBufCrc32 (str, (ST_INT) strlen (str), crcInOut, SD_FALSE);
  }


/************************************************************************/
/************************************************************************/
#ifdef SSEAL_CRC64_SUPPORT
/************************************************************************/
/************************************************************************/
/*			sBufCrc64					*/
/************************************************************************/

#define POLY64REV       0x95AC9329AC4BC9B5ULL

ST_VOID sBufCrc64 (ST_VOID *vBuf, ST_INT numBytes, ST_UINT64 *crcInOut, ST_BOOLEAN backwards)
{
ST_INT i, j;
unsigned long long crc;
unsigned long long part;
static int init = 0;
static unsigned long long CRCTable[256];
unsigned char *buf;

  crc = *crcInOut;    
  if (!init)
    {
    init = 1;
    for (i = 0; i < 256; i++)
      {
      part = i;
      for (j = 0; j < 8; j++)
        {
        if (part & 1)
          part = (part >> 1) ^ POLY64REV;
        else
          part >>= 1;
        }
        CRCTable[i] = part;
      }
    }

  buf = (ST_UCHAR *) vBuf;

  if (!backwards)
    {
    for (; (numBytes > 0); numBytes--)
      crc = CRCTable[(crc ^ *buf++) & 0xff] ^ (crc >> 8);
    }
  else
    {
    buf += numBytes-1;
    for (; (numBytes > 0); numBytes--)
      crc = CRCTable[(crc ^ *buf--) & 0xff] ^ (crc >> 8);
    }

  *crcInOut = crc;    
  }

/************************************************************************/
#endif /* #ifdef SSEAL_CRC64_SUPPORT */
/************************************************************************/

