/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 2001, All Rights Reserved.		        */
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : ard_objid.c						*/
/* PRODUCT(S)  : ASN1DE 						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 12/20/01  JRB     03    Chg ASN1_CTXT to ASN1_DEC_CTXT.		*/
/* 09/13/99  MDE     02    Added SD_CONST modifiers			*/
/* 07/26/99  MDE     01    New module, derived from ard_objid.c		*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "asn1r.h"
#include "asn1log.h"

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/
/************************************************************************/
/*			    get_objid					*/
/* Function to read an object identifier data element from an ASN.1 msg */
/* and put sub-identifiers into integer array.				*/
/************************************************************************/

#define  MAX_OBJ_ID_COMPONENTS 		16

ST_RET asn1r_get_objid (ASN1_DEC_CTXT *ac, ST_INT16 component_list[], ST_INT *num_components)
 {
ST_UCHAR c; 		/* Temporary storage for octets.		*/
ST_UCHAR *last_pos; 	/* One past last position of this data element. */
ST_UINT16 j;		/* Temporarily holds value encoded in 2 octets. */
ST_INT i;		/* Counter for looping through sub-identifiers. */

#ifdef	DEBUG_ASN1_DECODE
  if (!num_components || !component_list)
    {
    slogCallStack (sLogCtrl,
                    "get_objid: attempt to reference through a NULL pointer");
    return(SD_FAILURE);
    }
#endif

 last_pos = ac->asn1r_field_ptr + ac->asn1r_elmnt_len;
 c = *(ac->asn1r_field_ptr++);		/* First 2 subIDs reside in 1st octet.	*/

 if (c & 0x80) 
   {
   ALOG_NERR0 ("ASN.1 decode: Object ID first octet error");
   return (SD_FAILURE);
   }

 if ((component_list[0] = c/40) > 2)
   {
   ALOG_NERR0 ("ASN.1 decode: Object ID component 0 too big");
   return (SD_FAILURE);
   }

 if ((component_list[1] = c%40) > 39)
   {
   ALOG_NERR0 ("ASN.1 decode: Object ID component 1 too big");
   return (SD_FAILURE);
   }

 *num_components = 2;

/* Decode remaining subIDs in contents of object identifier data el't.  */

 for (i=2; ac->asn1r_field_ptr < last_pos; i++,(*num_components)++)
   {
   if (i >= MAX_OBJ_ID_COMPONENTS)	/* avoid overwrite		*/
     {
     ALOG_NERR0 ("ASN.1 decode: Object ID too many components");
     return (SD_FAILURE);
     }

   if ((c = *(ac->asn1r_field_ptr++)) & 0x80)	/* if next octet is not last -	*/
     {
     if (*ac->asn1r_field_ptr & 0x80)		/* reject if more than 2 octets */
       {
       ALOG_NERR0 ("ASN.1 decode: Object ID more than 2 octets for value");
       return (SD_FAILURE);
       } 

     j = (ST_UINT16) (c & 0x7F) << 7;	/* support two octets per subID */
     j |= (*(ac->asn1r_field_ptr++) & 0x7F);	/* merge the 14 ID bits 	*/
     component_list[i] = j;
     }
   else 				/* only one octet		*/
     component_list[i] = (ST_UINT16) c;	/* write masked ID code 	*/
   }

 return(SD_SUCCESS);
 }

