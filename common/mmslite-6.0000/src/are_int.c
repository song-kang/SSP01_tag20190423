/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2002, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : are_int.c 						*/
/* PRODUCT(S)  : ASN1DE 						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 03/11/04  GLB     08    Remove "thisFileName"                    	*/
/* 01/14/03  JRB     07    Avoid redundant logging of encode overrun.	*/
/* 01/22/02  JRB     06    Chg asn1r_end_of_buffer to asn1r_buf_start.	*/
/* 12/20/01  JRB     05    Chg ASN1_CTXT to ASN1_ENC_CTXT.		*/
/* 11/12/01  EJV     04    Fixed function name asn1r_wr_i64		*/
/* 09/26/00  JRB     03    Add another #ifdef INT64_SUPPORT		*/
/* 09/13/99  MDE     02    Added SD_CONST modifiers			*/
/* 07/26/99  MDE     01    New module, derived from ae_int.c		*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "asn1r.h"
#include "asn1log.h"


/************************************************************************/
/* Local functions in this module					*/
/* These are not static because we may want to use them in macro's	*/
/* to speed things up some time in the future.				*/

ST_VOID asn1r_wr_int32 (ASN1_ENC_CTXT *ac, ST_INT32 data, ST_BOOLEAN positive);
#ifdef INT64_SUPPORT
ST_VOID asn1r_wr_int64 (ASN1_ENC_CTXT *ac, ST_INT64 data, ST_BOOLEAN positive);
#endif

/************************************************************************/
/*				wr_i8					*/
/************************************************************************/

ST_VOID asn1r_wr_i8 (ASN1_ENC_CTXT *ac, ST_INT8 data)
  {
  asn1r_wr_int32 (ac, (ST_INT32) data, (ST_BOOLEAN) (data >= 0 ? SD_TRUE : SD_FALSE));
  }

/************************************************************************/
/*				wr_i16					*/
/* Function to write a 16-bit integer into message being constructed.	*/
/* Can handle values to 16 bits.  Sets asn1_field_ptr to next position.	*/
/************************************************************************/

ST_VOID asn1r_wr_i16 (ASN1_ENC_CTXT *ac, ST_INT16 data)
  {
  asn1r_wr_int32 (ac, (ST_INT32) data, (ST_BOOLEAN) (data >= 0 ? SD_TRUE : SD_FALSE));
  }

/************************************************************************/
/*				wr_i32					*/
/************************************************************************/

ST_VOID asn1r_wr_i32 (ASN1_ENC_CTXT *ac, ST_INT32 data)
  {
  asn1r_wr_int32 (ac, data, (ST_BOOLEAN) (data >= 0 ? SD_TRUE : SD_FALSE));
  }

/************************************************************************/
/*				wr_u8					*/
/************************************************************************/

ST_VOID asn1r_wr_u8 (ASN1_ENC_CTXT *ac, ST_UINT8 data)
  {
  asn1r_wr_int32 (ac, (ST_INT32) data, SD_TRUE);
  }

/************************************************************************/
/*				wr_u16					*/
/************************************************************************/

ST_VOID asn1r_wr_u16 (ASN1_ENC_CTXT *ac, ST_UINT16 data)
  {
  asn1r_wr_int32 (ac, (ST_INT32) data, SD_TRUE);
  }

/************************************************************************/
/*				wr_u32					*/
/************************************************************************/

ST_VOID asn1r_wr_u32 (ASN1_ENC_CTXT *ac, ST_UINT32 data)
  {
  asn1r_wr_int32 (ac, (ST_INT32) data, SD_TRUE);
  }

/************************************************************************/
/*				_wr_int32				*/
/************************************************************************/

ST_VOID asn1r_wr_int32 (ASN1_ENC_CTXT *ac, ST_INT32 data, ST_BOOLEAN positive)
  {
ST_INT i;

  if (ac->asn1r_field_ptr - 5 < ac->asn1r_buf_start)
    {
    ac->asn1r_encode_overrun = SD_TRUE;
    return;
    }

/* First just write all 32 bits into the message */
  *(ac->asn1r_field_ptr--) = (ST_UCHAR) data;		
  *(ac->asn1r_field_ptr--) = (ST_UCHAR)(data >>  8);	
  *(ac->asn1r_field_ptr--) = (ST_UCHAR)(data >> 16);	
  *(ac->asn1r_field_ptr--) = (ST_UCHAR)(data >> 24);	


  if (positive)
    {
  /* ASN.1 requires us to eliminate redundant leading zero's, and 	*/
  /* we need to make sure the MSB is not set in the high order byte	*/

    if (*(ac->asn1r_field_ptr + 1) & 0x80)
      *(ac->asn1r_field_ptr--) = 0x00;
    else
      {
      for (i = 0; i < 3; ++i)
	{
        if ((*(ac->asn1r_field_ptr + 1) == 0x00) &&
            (!(*(ac->asn1r_field_ptr + 2) & 0x80)))
	  {
          ++ac->asn1r_field_ptr;
	  }
        else
          break;
	}
      } 
    }
  else	/* The data value is negative */
    {
  /* ASN.1 requires us to eliminate redundant leading 0xFF's, and	*/
  /* we need to make sure that the MSB is set in the high order byte	*/
    for (i = 0; i < 3; ++i)
      {
      if ((*(ac->asn1r_field_ptr + 1) == 0xFF) &&
          (*(ac->asn1r_field_ptr + 2) & 0x80))
        {
        ++ac->asn1r_field_ptr;
        }
      else
        break;
      } 
    }
  }

/************************************************************************/
/************************************************************************/
#ifdef INT64_SUPPORT
/************************************************************************/
/*				wr_i64					*/
/************************************************************************/

ST_VOID asn1r_wr_i64 (ASN1_ENC_CTXT *ac, ST_INT64 data)
  {
  asn1r_wr_int64 (ac, (ST_INT64) data, (ST_BOOLEAN) (data >= 0 ? SD_TRUE : SD_FALSE));
  }

ST_VOID asn1r_wr_u64 (ASN1_ENC_CTXT *ac, ST_UINT64 data)
  {
  asn1r_wr_int64 (ac, (ST_INT64) data, SD_TRUE);
  }

/************************************************************************/
/*				_wr_int64				*/
/************************************************************************/

ST_VOID asn1r_wr_int64 (ASN1_ENC_CTXT *ac, ST_INT64 data, ST_BOOLEAN positive)
  {
ST_INT i;

  if (ac->asn1r_field_ptr - 9 < ac->asn1r_buf_start)
    {
    ac->asn1r_encode_overrun = SD_TRUE;
    return;
    }

  *(ac->asn1r_field_ptr--) = (ST_UCHAR) data;		
  *(ac->asn1r_field_ptr--) = (ST_UCHAR)(data >>  8);	
  *(ac->asn1r_field_ptr--) = (ST_UCHAR)(data >> 16);	
  *(ac->asn1r_field_ptr--) = (ST_UCHAR)(data >> 24);	
  *(ac->asn1r_field_ptr--) = (ST_UCHAR)(data >> 32);	
  *(ac->asn1r_field_ptr--) = (ST_UCHAR)(data >> 40);	
  *(ac->asn1r_field_ptr--) = (ST_UCHAR)(data >> 48);	
  *(ac->asn1r_field_ptr--) = (ST_UCHAR)(data >> 56);

  if (positive)
    {
  /* ASN.1 requires us to eliminate redundant leading zero's, and 	*/
  /* we need to make sure the MSB is not set in the high order byte	*/

    if (*(ac->asn1r_field_ptr + 1) & 0x80)
      *(ac->asn1r_field_ptr--) = 0x00;
    else
      {
      for (i = 0; i < 7; ++i)
	{
        if ((*(ac->asn1r_field_ptr + 1) == 0x00) &&
            (!(*(ac->asn1r_field_ptr + 2) & 0x80)))
	  {
          ++ac->asn1r_field_ptr;
	  }
        else
          break;
	}
      } 
    }
  else	/* The data value is negative */
    {
  /* ASN.1 requires us to eliminate redundant leading 0xFF's, and	*/
  /* we need to make sure that the MSB is set in the high order byte	*/
    for (i = 0; i < 7; ++i)
      {
      if ((*(ac->asn1r_field_ptr + 1) == 0xFF) &&
          (*(ac->asn1r_field_ptr + 2) & 0x80))
        {
        ++ac->asn1r_field_ptr;
        }
      else
        break;
      } 
    }
  }

/************************************************************************/
#endif /* #ifdef INT64_SUPPORT */
/************************************************************************/

