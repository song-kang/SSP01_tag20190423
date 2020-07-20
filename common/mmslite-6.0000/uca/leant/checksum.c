/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1996, All Rights Reserved.		        	*/
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : checksum.c						*/
/* PRODUCT(S)  : Lean-T Stack						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*		This module implements the computation of checksum	*/
/*		using the algorithm defined in Annex B of the ISO 8073	*/
/*		or in the ISO 8473.					*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/*			checksum_insert					*/
/*			checksum_verified				*/
/*									*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 05/27/97  JRB   7.00    MMSEASE 7.0 release.				*/
/* 05/12/97  JRB    03     ch*_verified chk c0. Was checking c1 twice.	*/
/*			   Clean up data types.				*/
/*			   Clarify casting computing x,y in ch*_insert.	*/
/* 05/07/97  JRB    02     Changed non-ANSI "%hu" to "%u".		*/
/* 06/17/96  EJV    01     Created					*/
/************************************************************************/
#if 0
/* this file is not neccessary when printf will be replaced with slog	*/
#include <stdio.h>
#endif

#include "glbtypes.h"
#include "checksum.h"


static ST_VOID get_checksum_c0_c1 (ST_UCHAR *buf, ST_UINT16 buf_len,
				ST_UCHAR *pC0, ST_UCHAR *pC1);


/************************************************************************/
/*			checksum_insert					*/
/*----------------------------------------------------------------------*/
/* This function computes and inserts the 2 byte checksum into a PDU	*/
/* buffer at specified position (and position+1).			*/
/* The checksum algorithm is described in ISO 8473 (CLNP) and		*/
/* ISO 8073 (Transport protocol).					*/
/* Note that the maximum len of TPDU in ISO 8073 is 8192 bytes.		*/
/* 									*/
/* Parameters:								*/
/*	ST_UCHAR	*buf		Ptr to encoding buffer on which checksum*/
/*				will be computed.			*/
/*	ST_UINT16	buf_len		Length of the buffer (PDU).		*/
/*	ST_UINT16	position	Position of checksum's first byte in buf*/
/*				(position=0 if checksum is the first	*/
/*				byte in buffer).			*/
/*									*/
/* Return:								*/
/*	ST_VOID			none					*/
/************************************************************************/
ST_VOID checksum_insert (ST_UCHAR *buf, ST_UINT16 buf_len, ST_UINT16 position)
{
ST_UCHAR  c0, c1;
ST_LONG   x, y;		/* the checksum's first and second byte		*/
ST_UINT16 sum_pos;		/* this is the position of checksum first byte	*/
			/* in the PDU as specified by algorithm, first	*/
			/* byte position is 1 not 0.			*/

  sum_pos = position+1;

  /* make sure the checksum bytes are cleared */ 
  buf [position] = buf [position+1] = 0;

  get_checksum_c0_c1 (buf, buf_len, &c0, &c1); 

#if 0
  /* LIZ substitute the printf with slogging */
  printf ("\n Checksum computation: c0=0x%2.2X=%u, c1=0x%2.2X=%u",
    (ST_UINT) c0, (ST_UINT) c0, (ST_UINT) c1, (ST_UINT) c1);
#endif


  x = ((ST_LONG) (buf_len-sum_pos) * (ST_LONG) c0 - (ST_LONG) c1) % 255L;
  y = ((ST_LONG) c1 - (ST_LONG) (buf_len-sum_pos+1) * (ST_LONG) c0) % 255L;

  buf [position]   = (ST_UCHAR) (x <= 0L ? x+255L : x);
  buf [position+1] = (ST_UCHAR) (y <= 0L ? y+255L : y);

#if 0
  /* LIZ substitute the printf with slogging */
  printf ("\n Checksum computation: buf[%u]=%u, buf[%u]=%u",
		position,  (ST_UINT) buf[position],
		position+1,(ST_UINT) buf[position+1]);
#endif
}


/************************************************************************/
/*			checksum_verified				*/
/*----------------------------------------------------------------------*/
/* This function verifies the checksum of a PDU.			*/
/* The checksum algorithm is described in ISO 8473 (CLNP) and		*/
/* ISO 8073 (Transport protocol).					*/
/* Note that the maximum len of TPDU in ISO 8073 is 8192 bytes.		*/
/* 									*/
/* Parameters:								*/
/*	ST_UCHAR	*buf		Ptr to encoding buffer on which checksum*/
/*				will be computed.			*/
/*	ST_UINT16	buf_len		Length of the buffer (PDU).		*/
/*	ST_UINT16	position	Position of checksum's first byte in buf*/
/*				(position=0 if checksum is the first	*/
/*				byte in buffer), used only for CLNP.	*/
/*	ST_BOOLEAN	clnp_csum	If SD_TRUE the CLNP checksum will be	*/
/*				verified, if SD_FALSE the Transport Prot.	*/
/*				checksum will be verified.		*/
/*									*/
/* Return:								*/
/*	ST_BOOLEAN			SD_TRUE if verification of checksum is	*/
/*				successful;				*/
/*				SD_FALSE if verification failed.		*/
/************************************************************************/
ST_BOOLEAN checksum_verified (ST_UCHAR *buf, ST_UINT16 buf_len, ST_UINT16 position,
			ST_BOOLEAN clnp_csum)
{
ST_BOOLEAN checksum_correct;
ST_UCHAR   c0, c1;

  checksum_correct = SD_FALSE;

  if (clnp_csum)
    {
    if (buf [position] != 0 && buf [position+1] != 0)
      {
      /* if both bytes in checksum are not 0 then we have to compute the	*/
      /* checksum								*/

      get_checksum_c0_c1 (buf, buf_len, &c0, &c1);

#if 0
      /* LIZ substitute the printf with slogging */
      printf ("\n Checksum verification: c0=0x%2.2X=%u, c1=0x%2.2X=%u",
        (ST_UINT) c0, (ST_UINT) c0, (ST_UINT) c1, (ST_UINT) c1);
#endif

      if (c0 == 0 && c1 == 0)
        checksum_correct = SD_TRUE;
      }
    else if (buf [position] == 0 && buf [position+1] == 0)
      {
      /* if both bytes in checksum are 0 then the checksum should be	*/
      /* ignored, and is considered as correct.				*/

      checksum_correct = SD_TRUE;
      }

    /* if one byte (but not both bytes) in checksum is 0 then the	*/
    /* checksum is invalid.						*/
    }
  else
    {
    /* transport protocol checksum */

    get_checksum_c0_c1 (buf, buf_len, &c0, &c1);

    if (c0 == 0 && c1 == 0)
      checksum_correct = SD_TRUE;
    }

  return (checksum_correct);
}


/************************************************************************/
/*			get_checksum_c0_c1				*/
/*----------------------------------------------------------------------*/
/* This function computes the checksum values C0 and C1 described in	*/
/* ISO 8473 (CLNP) and ISO 8073 (Transport protocol).			*/
/* Note that the maximum len of TPDU in ISO 8073 is 8192 bytes.		*/
/* To avoid overflow of long values c0 or c1 when the buffer len is	*/
/* greater then 4096 the values c0 and c1 are computed in two stages.	*/
/* 									*/
/* Parameters:								*/
/*	ST_UCHAR	*buf		Ptr to encoding buffer on which checksum*/
/*				will be computed.			*/
/*	ST_UINT16	buf_len		Length of the buffer (TPDU).		*/
/*	ST_UCHAR	*pC0		Pointer where to return the computed C0.*/
/*	ST_UCHAR	*pC1		Pointer where to return the computed C1.*/
/*									*/
/* Return:								*/
/*	ST_VOID			none					*/
/************************************************************************/
static ST_VOID get_checksum_c0_c1 (ST_UCHAR *buf, ST_UINT16 buf_len,
				ST_UCHAR *pC0, ST_UCHAR *pC1)
{
ST_ULONG  c0, c1;

  c0 = c1 = 0L;

  while (buf_len-- > 0)
    {
    c0 = c0 + (ST_ULONG) *(buf++);
    c1 = c1 + c0;

    /* if the buf_len > 4096 there is risk of overflowing, so we take	*/
    /* modulus at 4096 bytes to bring the c0 and c1 numbers down	*/

    if (buf_len == 4096)
      {
      c0 = c0 % 255L;
      c1 = c1 % 255L;
      }
    }
  *pC0 = (ST_UCHAR) (c0 % 255L);
  *pC1 = (ST_UCHAR) (c1 % 255L);

  /* the code below can substitute the code above if long 32-bit values	*/
  /* are not available with given compiler.				*/
  #if 0
  ST_UINT16 c0,c1;
  c0 = c1 = 0;
  while (buf_len-- > 0)
    {
    c0 += (ST_UINT16) *(buf++);
    if (c0 >= 255)
      {
      ++c0;		/* this two lines are equvalen to mod 255 */
      c0 &= 0x00ff;
      }
    c1 += c0;
    if (c1 >= 255)
      {
      ++c1;	
      c1 &= 0x00ff;
      }
    }
  *pC0 = (ST_UCHAR) c0;
  *pC1 = (ST_UCHAR) c1;
  #endif
}


