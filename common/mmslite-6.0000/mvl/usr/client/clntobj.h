/*  This file created from input file '..\..\mvl\usr\client\clntobj.odf'
    Leaf Access Parameter (LAP) File: 'Not Used'
	Created Mon Feb 25 19:13:22 2019
*/

#ifndef FOUNDRY_OUTPUT_HEADER_INCLUDED
#define FOUNDRY_OUTPUT_HEADER_INCLUDED

#include "mvl_uca.h"

#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************/
/* MVL Type Control Information */
extern MVL_TYPE_CTRL *mvl_type_ctrl;
extern ST_INT mvl_num_types;

/************************************************************************/


/* MVL Type ID's */
#define RTYP_BOOL_TYPEID		0
#define RTYP_BTIME6_TYPEID		1
#define RTYP_BSTR6_TYPEID		2
#define RTYP_BSTR8_TYPEID		3
#define RTYP_BSTR9_TYPEID		4
#define RTYP_BVSTR6_TYPEID		5
#define RTYP_BVSTR8_TYPEID		6
#define RTYP_BVSTR10_TYPEID		7
#define RTYP_INT8U_TYPEID		8
#define RTYP_INT16U_TYPEID		9
#define RTYP_OSTR8_TYPEID		10
#define RTYP_VSTR32_TYPEID		11
#define RTYP_VSTR65_TYPEID		12
#define RTYP_OBJREF_TYPEID		13
#define RTYP_INT32U_TYPEID		14
#define I16_TYPEID		15
#define UTF8VSTRING13_TYPEID		16
#define phv_type_TYPEID		17
#define phsx_type_TYPEID		18


/************************************************************************/

/************************************************************************/


/*	Common Strings Index Defines	*/

#ifndef USE_RT_TYPE_2

#define FO_PHSA_STRING_INDEX	 1
#define FO_INSTCVAL_STRING_INDEX	 2
#define FO_MAG_STRING_INDEX	 3
#define FO_I_STRING_INDEX	 4
#define FO_F_STRING_INDEX	 5
#define FO_ANG_STRING_INDEX	 6
#define FO_CVAL_STRING_INDEX	 7
#define FO_RANGE_STRING_INDEX	 8
#define FO_Q_STRING_INDEX	 9
#define FO_T_STRING_INDEX	 10
#define FO_PHSB_STRING_INDEX	 11
#define FO_PHSC_STRING_INDEX	 12
#define FO_NEUT_STRING_INDEX	 13
#define FO_NET_STRING_INDEX	 14
#define FO_RES_STRING_INDEX	 15
#endif


/************************************************************************/
/* TYPEDEFS for MMS TYPES	                                              */
/************************************************************************/

/* Use ":CF", "-notypedefs" in the ODF file to not include this line */
#define USE_MMS_TYPEDEFS

#if defined (USE_MMS_TYPEDEFS)

typedef ST_BOOLEAN RTYP_BOOL_TDEF;

typedef MMS_BTIME6 RTYP_BTIME6_TDEF;

typedef ST_UCHAR RTYP_BSTR6_TDEF[1];  /* Bitstring */

typedef ST_UCHAR RTYP_BSTR8_TDEF[1];  /* Bitstring */

typedef ST_UCHAR RTYP_BSTR9_TDEF[2];  /* Bitstring */

typedef struct     /* BVstring */
  {
  ST_INT16 len_1;
  ST_UCHAR data_1[1];
  }  RTYP_BVSTR6_TDEF;  /* Bitstring */

typedef struct     /* BVstring */
  {
  ST_INT16 len_1;
  ST_UCHAR data_1[1];
  }  RTYP_BVSTR8_TDEF;  /* Bitstring */

typedef struct     /* BVstring */
  {
  ST_INT16 len_1;
  ST_UCHAR data_1[2];
  }  RTYP_BVSTR10_TDEF;  /* Bitstring */

typedef ST_UINT8 RTYP_INT8U_TDEF;

typedef ST_UINT16 RTYP_INT16U_TDEF;

typedef ST_UCHAR RTYP_OSTR8_TDEF[8];  /* Octetstring */

typedef ST_CHAR RTYP_VSTR32_TDEF[33];  /* Visible String */

typedef ST_CHAR RTYP_VSTR65_TDEF[66];  /* Visible String */

typedef ST_CHAR RTYP_OBJREF_TDEF[130];  /* Visible String */

typedef ST_UINT32 RTYP_INT32U_TDEF;

typedef ST_INT16 I16_TDEF;

typedef ST_CHAR UTF8VSTRING13_TDEF[28];

typedef struct
  {
  struct
    {
    struct
      {
      struct
        {
        ST_INT32 i;
        ST_FLOAT f;
        } mag;
      struct
        {
        ST_INT32 i;
        ST_FLOAT f;
        } ang;
      } instCVal;
    struct
      {
      struct
        {
        ST_INT32 i;
        ST_FLOAT f;
        } mag;
      struct
        {
        ST_INT32 i;
        ST_FLOAT f;
        } ang;
      } cVal;
    ST_INT8 range;
    struct     /* BVstring */
      {
      ST_INT16 len_1;
      ST_UCHAR data_1[2];
      } q;
    MMS_UTC_TIME t;
    } phsA;
  struct
    {
    struct
      {
      struct
        {
        ST_INT32 i;
        ST_FLOAT f;
        } mag;
      struct
        {
        ST_INT32 i;
        ST_FLOAT f;
        } ang;
      } instCVal;
    struct
      {
      struct
        {
        ST_INT32 i;
        ST_FLOAT f;
        } mag;
      struct
        {
        ST_INT32 i;
        ST_FLOAT f;
        } ang;
      } cVal;
    ST_INT8 range;
    struct     /* BVstring */
      {
      ST_INT16 len_1;
      ST_UCHAR data_1[2];
      } q;
    MMS_UTC_TIME t;
    } phsB;
  struct
    {
    struct
      {
      struct
        {
        ST_INT32 i;
        ST_FLOAT f;
        } mag;
      struct
        {
        ST_INT32 i;
        ST_FLOAT f;
        } ang;
      } instCVal;
    struct
      {
      struct
        {
        ST_INT32 i;
        ST_FLOAT f;
        } mag;
      struct
        {
        ST_INT32 i;
        ST_FLOAT f;
        } ang;
      } cVal;
    ST_INT8 range;
    struct     /* BVstring */
      {
      ST_INT16 len_1;
      ST_UCHAR data_1[2];
      } q;
    MMS_UTC_TIME t;
    } phsC;
  struct
    {
    struct
      {
      struct
        {
        ST_INT32 i;
        ST_FLOAT f;
        } mag;
      struct
        {
        ST_INT32 i;
        ST_FLOAT f;
        } ang;
      } instCVal;
    struct
      {
      struct
        {
        ST_INT32 i;
        ST_FLOAT f;
        } mag;
      struct
        {
        ST_INT32 i;
        ST_FLOAT f;
        } ang;
      } cVal;
    ST_INT8 range;
    struct     /* BVstring */
      {
      ST_INT16 len_1;
      ST_UCHAR data_1[2];
      } q;
    MMS_UTC_TIME t;
    } neut;
  struct
    {
    struct
      {
      struct
        {
        ST_INT32 i;
        ST_FLOAT f;
        } mag;
      struct
        {
        ST_INT32 i;
        ST_FLOAT f;
        } ang;
      } instCVal;
    struct
      {
      struct
        {
        ST_INT32 i;
        ST_FLOAT f;
        } mag;
      struct
        {
        ST_INT32 i;
        ST_FLOAT f;
        } ang;
      } cVal;
    ST_INT8 range;
    struct     /* BVstring */
      {
      ST_INT16 len_1;
      ST_UCHAR data_1[2];
      } q;
    MMS_UTC_TIME t;
    } net;
  struct
    {
    struct
      {
      struct
        {
        ST_INT32 i;
        ST_FLOAT f;
        } mag;
      struct
        {
        ST_INT32 i;
        ST_FLOAT f;
        } ang;
      } instCVal;
    struct
      {
      struct
        {
        ST_INT32 i;
        ST_FLOAT f;
        } mag;
      struct
        {
        ST_INT32 i;
        ST_FLOAT f;
        } ang;
      } cVal;
    ST_INT8 range;
    struct     /* BVstring */
      {
      ST_INT16 len_1;
      ST_UCHAR data_1[2];
      } q;
    MMS_UTC_TIME t;
    } res;
  } phv_type_TDEF;

typedef struct
  {
  struct
    {
    struct
      {
      ST_INT32 i;
      ST_FLOAT f;
      } mag;
    struct
      {
      ST_INT32 i;
      ST_FLOAT f;
      } ang;
    } instCVal;
  struct
    {
    struct
      {
      ST_INT32 i;
      ST_FLOAT f;
      } mag;
    struct
      {
      ST_INT32 i;
      ST_FLOAT f;
      } ang;
    } cVal;
  ST_INT8 range;
  struct     /* BVstring */
    {
    ST_INT16 len_1;
    ST_UCHAR data_1[2];
    } q;
  MMS_UTC_TIME t;
  } phsx_type_TDEF;

/************************************************************************/
#endif /* if defined (USE_MMS_TYPEDEFS) */
/************************************************************************/
#ifdef __cplusplus
}
#endif

#endif /* #ifndef FOUNDRY_OUTPUT_HEADER_INCLUDED */
