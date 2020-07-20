/*  This file created from input file '..\..\mvl\usr\server\srvrobj.odf'
    Leaf Access Parameter (LAP) File: 'Not Used'
	Created Mon Feb 25 19:13:34 2019
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
#define I16_TYPEID		0
#define I32_TYPEID		1
#define U32_TYPEID		2
#define STRUCT1_TYPEID		3
#define ARR1_TYPEID		4
#define LARGE_ARR_TYPEID		5
#define UTC_TM_TYPEID		6
#define UTF8VSTRING13_TYPEID		7


/************************************************************************/

/************************************************************************/


/*	Common Strings Index Defines	*/

#ifndef USE_RT_TYPE_2

#define FO_S_STRING_INDEX	 1
#define FO_L_STRING_INDEX	 2
#endif


/************************************************************************/
/* TYPEDEFS for MMS TYPES	                                              */
/************************************************************************/

/* Use ":CF", "-notypedefs" in the ODF file to not include this line */
#define USE_MMS_TYPEDEFS

#if defined (USE_MMS_TYPEDEFS)

typedef ST_INT16 I16_TDEF;

typedef ST_INT32 I32_TDEF;

typedef ST_UINT32 U32_TDEF;

typedef struct
  {
  ST_INT16 s;
  ST_INT32 l;
  } STRUCT1_TDEF;

typedef ST_INT16 ARR1_TDEF[10];

typedef ST_INT16 LARGE_ARR_TDEF[400];

typedef MMS_UTC_TIME UTC_TM_TDEF;

typedef ST_CHAR UTF8VSTRING13_TDEF[28];

/************************************************************************/
#endif /* if defined (USE_MMS_TYPEDEFS) */
/************************************************************************/
#ifdef __cplusplus
}
#endif

#endif /* #ifndef FOUNDRY_OUTPUT_HEADER_INCLUDED */
