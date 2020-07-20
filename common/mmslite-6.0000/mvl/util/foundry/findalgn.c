/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  	1997-2001, All Rights Reserved.		        */
/*									*/
/* MODULE NAME : findalgn.c						*/
/* PRODUCT(S)  : MMSEASE						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 03/31/10  JRB	   Fix STREND_ALGN value for M_STREND_MODE_NONE.*/
/*			   Eliminate double ";;".			*/
/* 04/14/03  JRB     06    Eliminate compiler warnings.			*/
/* 02/20/03  JRB     05    Check if SD_BYTE_ORDER define is correct.	*/
/* 01/09/03  CRM     04    Deleted #define DEBUG_SISCO			*/
/*			   Add #if defined(DEBUG_SISCO) to mSetLogCfg.	*/
/* 10/05/01  EJV     03    Added ifdef for not defined ST_INT64		*/
/* 09/21/98  MDE     02    Minor lint cleanup				*/
/* 12/10/97  MDE     01    Minor cleanups				*/
/* 04/15/97  DSF   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"

#if !defined(MMS_LITE)
#include "mms_usr.h"
#else
#include "mmsdefs.h"
#endif
#include "mms_vvar.h"

#include "slog.h"
#include "mem_chk.h"

/************************************************************************/

SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
ST_BOOLEAN debug_en;

/************************************************************************/
static ST_VOID mSetLogCfg (ST_VOID);
ST_VOID m_find_algn_values (ST_INT *algn_table_out,
			    ST_INT *strstart_mode_out, 
			    ST_INT *strend_mode_out);
ST_INT8 is_little_endian();
/************************************************************************/

ST_INT main (int argc, char *argv[])
  {
ST_INT strstart_mode;
ST_INT strend_mode;
ST_INT data_algn_tbl[NUM_ALGN_TYPES];

  if (argc > 1 && !strcmp (argv[1], "-d"))
    debug_en = SD_TRUE;

  mSetLogCfg ();
  m_find_algn_values (data_algn_tbl, &strstart_mode, &strend_mode);
  printf ("\n  data_algn_tbl[ARRSTRT_ALGN] : 0x%04x", data_algn_tbl[ARRSTRT_ALGN]);
  printf ("\n  data_algn_tbl[ARREND_ALGN]  : 0x%04x", data_algn_tbl[ARREND_ALGN]);
  printf ("\n  data_algn_tbl[STRSTRT_ALGN] : 0x%04x", data_algn_tbl[STRSTRT_ALGN]);
  printf ("\n  data_algn_tbl[STREND_ALGN]  : 0x%04x", data_algn_tbl[STREND_ALGN]);
  printf ("\n  data_algn_tbl[INT8_ALGN]    : 0x%04x", data_algn_tbl[INT8_ALGN]);
  printf ("\n  data_algn_tbl[INT16_ALGN]   : 0x%04x", data_algn_tbl[INT16_ALGN]);
  printf ("\n  data_algn_tbl[INT32_ALGN]   : 0x%04x", data_algn_tbl[INT32_ALGN]);
  printf ("\n  data_algn_tbl[INT64_ALGN]   : 0x%04x", data_algn_tbl[INT64_ALGN]);
  printf ("\n  data_algn_tbl[FLOAT_ALGN]   : 0x%04x", data_algn_tbl[FLOAT_ALGN]);
  printf ("\n  data_algn_tbl[DOUBLE_ALGN]  : 0x%04x", data_algn_tbl[DOUBLE_ALGN]);
  printf ("\n  data_algn_tbl[OCT_ALGN]     : 0x%04x", data_algn_tbl[OCT_ALGN]);
  printf ("\n  data_algn_tbl[BOOL_ALGN]    : 0x%04x", data_algn_tbl[BOOL_ALGN]);
  printf ("\n  data_algn_tbl[BCD1_ALGN]    : 0x%04x", data_algn_tbl[BCD1_ALGN]);
  printf ("\n  data_algn_tbl[BCD2_ALGN]    : 0x%04x", data_algn_tbl[BCD2_ALGN]);
  printf ("\n  data_algn_tbl[BCD4_ALGN]    : 0x%04x", data_algn_tbl[BCD4_ALGN]);
  printf ("\n  data_algn_tbl[BIT_ALGN]     : 0x%04x", data_algn_tbl[BIT_ALGN]);
  printf ("\n  data_algn_tbl[VIS_ALGN]     : 0x%04x", data_algn_tbl[VIS_ALGN]);

  if (strstart_mode == M_STRSTART_MODE_NONE)
    {
    printf ("\n  Structure start mode = M_STRSTART_MODE_NONE");
    }
  else if (strstart_mode == M_STRSTART_MODE_LARGEST)
    {
    printf ("\n  Structure start mode = M_STRSTART_MODE_LARGEST");
    }
  else if (strstart_mode == M_STRSTART_MODE_FIRST)
    {
    printf ("\n  Structure start mode = M_STRSTART_MODE_FIRST");
    }

  if (strend_mode == M_STREND_MODE_NONE)
    {
    printf ("\n  Structure end mode = M_STREND_MODE_NONE");
    }
  else if (strend_mode == M_STREND_MODE_LARGEST)
    {
    printf ("\n  Structure end mode = M_STREND_MODE_LARGEST");
    }

  printf ("\n\n");

#if SD_BYTE_ORDER==SD_BIG_ENDIAN
  if (is_little_endian())
    printf ("\nWARNING: SD_BYTE_ORDER==SD_BIG_ENDIAN. Should be SD_LITTLE_ENDIAN. Check 'glbtypes.h'.");
  else
    printf ("\nSD_BYTE_ORDER==SD_BIG_ENDIAN. Appears to be CORRECT.");
#else
  if (is_little_endian())
    printf ("\nSD_BYTE_ORDER==SD_LITTLE_ENDIAN. Appears to be CORRECT.");
  else
    printf ("\nWARNING: SD_BYTE_ORDER==SD_LITTLE_ENDIAN. Should be SD_BIG_ENDIAN. Check 'glbtypes.h'.");
#endif
  return(0);
  }

/************************************************************************/
/************************************************************************/
/************************************************************************/
/************************************************************************/

#ifndef offsetof
#define offsetof(s,m)   (size_t)((char *)&(((s *)0)->m) - (char *)0)
#endif

/************************************************************************/

ST_INT _getAlgnVal (ST_INT o);

/************************************************************************/
/************************************************************************/

typedef struct algn_Byte_test
  {
  ST_CHAR x;
  ST_CHAR y;
  }ALGN_BYTE_TEST;

typedef struct algn_Short_test
  {
  ST_CHAR x;
  ST_INT16 y;
  }ALGN_SHORT_TEST;

typedef struct algn_Long_test
  {
  ST_CHAR x;
  ST_INT32 y;
  }ALGN_LONG_TEST;

#if defined(INT64_SUPPORT)
typedef struct algn_Xlong_test
  {
  ST_CHAR x;
  ST_INT64 y;
  }ALGN_XLONG_TEST;
#endif

typedef struct algn_Float_test
  {
  ST_CHAR x;
  ST_FLOAT y;
  }ALGN_FLOAT_TEST;

typedef struct algn_Double_test
  {
  ST_CHAR x;
  ST_DOUBLE y;
  }ALGN_DOUBLE_TEST;


typedef struct algn_arrstart_test
  {
  ST_CHAR x;
  ST_CHAR y[5];
  } ALGN_ARRSTART_TEST;


/******** Structure alignment test structures ********/

/* >>> MODE DETECTION CASES */
typedef struct algn_strstart_test0
  {
  ST_INT8 x;
  struct
    {
    ST_INT8 y;
    } s;
  } ALGN_STRSTART_TEST0;

typedef struct algn_strstart_test1
  {
  ST_INT8 x;
  struct
    {
    ST_INT16 y;
    } s;
  } ALGN_STRSTART_TEST1;

typedef struct algn_strstart_test2
  {
  ST_INT8 x;
  struct
    {
    ST_INT32 y;
    } s;
  } ALGN_STRSTART_TEST2;

typedef struct algn_strstart_test3
  {
  ST_INT8 x;
  struct
    {
    ST_INT8 y;
    ST_INT32 z;
    } s;
  } ALGN_STRSTART_TEST3;

/* >>> MODE CONSISTANCY VERIFICATION CASES */
typedef struct algn_strstart_test4
  {
  ST_INT8 x;
  struct
    {
    ST_INT8 y;
    ST_INT32 z;
    } s;
  } ALGN_STRSTART_TEST4;

typedef struct algn_strstart_test5
  {
  ST_INT8 x;
  struct
    {
    ST_INT16 y;
    ST_INT32 z;
    } s;
  } ALGN_STRSTART_TEST5;
		       
typedef struct algn_strstart_test6
  {
  ST_INT8 x;
  struct
    {
    ST_INT8 y;
    } s;
  ST_INT8 z;
  } ALGN_STRSTART_TEST6;

typedef struct algn_strstart_test7
  {
  ST_INT8 x;
  struct
    {
    ST_INT16 y;
    } s;
  ST_INT8 z;
  } ALGN_STRSTART_TEST7;

typedef struct algn_strstart_test8
  {
  ST_INT8 x;
  struct
    {
    ST_INT32 y;
    } s;
  ST_INT8 z;
  } ALGN_STRSTART_TEST8;


/* >>> STRUCTURE END MODE DETECTION CASES */

typedef struct algn_strend_test0
  {
  ST_INT8 x;
  } ALGN_STREND_TEST0;

typedef struct algn_strend_test1
  {
  struct
    {
    ST_INT16 y;
    } s;
  } ALGN_STREND_TEST1;

typedef struct algn_strend_test2
  {
  struct
    {
    ST_INT32 y;
    } s;
  ST_INT8 x;
  } ALGN_STREND_TEST2;

typedef struct algn_strend_test3
  {
  struct
    {
    ST_INT8 y;
    ST_INT32 z;
    } s;
  ST_INT8 x;
  } ALGN_STREND_TEST3;

typedef struct algn_strend_test4
  {
  ST_INT8 y;
  struct
    {
    ST_INT32 z;
    } s;
  ST_INT8 x;
  } ALGN_STREND_TEST4;

typedef struct algn_strend_test5
  {
  struct
    {
    ST_INT16 y;
    ST_INT32 z;
    } s;
  ST_INT8 x;
  } ALGN_STREND_TEST5;
		       
typedef struct algn_strend_test6
  {
  ST_INT8 x;
  struct
    {
    ST_INT8 y;
    } s;
  ST_INT8 z;
  } ALGN_STREND_TEST6;

typedef struct algn_strend_test7
  {
  ST_INT8 x;
  struct
    {
    ST_INT16 y;
    } s;
  ST_INT8 z;
  } ALGN_STREND_TEST7;

typedef struct algn_strend_test8
  {
  ST_INT8 x;
  struct
    {
    ST_INT32 y;
    } s;
  ST_INT8 z;
  } ALGN_STREND_TEST8;

/************************************************************************/
/************************************************************************/


ST_VOID m_find_algn_values (ST_INT *algn_tbl_out,
			    ST_INT *strstart_mode_out, 
			    ST_INT *strend_mode_out)

  {
ST_INT o;
ST_INT byte_algn_val;
ST_INT short_algn_val;
ST_INT long_algn_val;
ST_INT xlong_algn_val;
ST_INT float_algn_val;
ST_INT double_algn_val;
ST_INT arrstart_algn_val;
ST_INT arrend_algn_val;
ST_INT strstart_algn_val;
ST_INT strend_algn_val;
ST_INT strstart_algn0;
ST_INT strstart_algn1;
ST_INT strstart_algn2;
ST_INT strstart_algn3;
ST_INT strstart_algn4;
ST_INT strstart_algn5;
ST_INT strstart_algn6;
ST_INT strstart_algn7;
ST_INT strstart_algn8;
ST_INT str0_size;
ST_INT str1_size;
ST_INT str2_size;
ST_INT str3_size;
ST_INT str4_size;
ST_INT str5_size;
ST_INT str6_size;
ST_INT str7_size;
ST_INT str8_size;
ST_INT strstart_mode;
ST_INT strend_mode;

/* Find 8 bit element alignment */
  o = offsetof (ALGN_BYTE_TEST, y);
  byte_algn_val = _getAlgnVal (o);

/* Find 16 bit integer element alignment */
  o = offsetof (ALGN_SHORT_TEST, y);
  short_algn_val = _getAlgnVal (o);

/* Find 32 bit integer element alignment */
  o = offsetof (ALGN_LONG_TEST, y);
  long_algn_val = _getAlgnVal (o);

#ifdef INT64_SUPPORT
  o = offsetof (ALGN_XLONG_TEST, y);
  xlong_algn_val = _getAlgnVal (o);
#else
  xlong_algn_val = long_algn_val;
#endif

/* Find float element alignment */
  o = offsetof (ALGN_FLOAT_TEST, y);
  float_algn_val = _getAlgnVal (o);

/* Find double element alignment */
  o = offsetof (ALGN_DOUBLE_TEST, y);
  double_algn_val = _getAlgnVal (o);


/*************************************************/
/* So much for the easy ones, now do structures */

/* Find structure start element alignment */
  o = offsetof (ALGN_STRSTART_TEST0, s);
  strstart_algn0 = _getAlgnVal (o);
  str0_size = sizeof (ALGN_STREND_TEST0);

  o = offsetof (ALGN_STRSTART_TEST1, s);
  strstart_algn1 = _getAlgnVal (o);
  str1_size = sizeof (ALGN_STREND_TEST1);

  o = offsetof (ALGN_STRSTART_TEST2, s);
  strstart_algn2 = _getAlgnVal (o);
  str2_size = sizeof (ALGN_STREND_TEST2);

  o = offsetof (ALGN_STRSTART_TEST3, s);
  strstart_algn3 = _getAlgnVal (o);
  str3_size = sizeof (ALGN_STREND_TEST3);

  o = offsetof (ALGN_STRSTART_TEST4, s);
  strstart_algn4 = _getAlgnVal (o);
  str4_size = sizeof (ALGN_STREND_TEST4);

  o = offsetof (ALGN_STRSTART_TEST5, s);
  strstart_algn5 = _getAlgnVal (o);
  str5_size = sizeof (ALGN_STREND_TEST5);

  o = offsetof (ALGN_STRSTART_TEST6, s);
  strstart_algn6 = _getAlgnVal (o);
  str6_size = sizeof (ALGN_STREND_TEST6);

  o = offsetof (ALGN_STRSTART_TEST7, s);
  strstart_algn7 = _getAlgnVal (o);
  str7_size = sizeof (ALGN_STREND_TEST7);

  o = offsetof (ALGN_STRSTART_TEST8, s);
  strstart_algn8 = _getAlgnVal (o);
  str8_size = sizeof (ALGN_STREND_TEST8);

  if (debug_en == SD_TRUE)
    {
    SLOGCALWAYS2 ("byte_algn_val\t= 0x%04x\t (offset %d)", byte_algn_val, o);
    SLOGCALWAYS2 ("short_algn_val\t= 0x%04x\t (offset %d)", short_algn_val, o);
    SLOGCALWAYS2 ("long_algn_val\t= 0x%04x\t (offset %d)", long_algn_val, o);
    SLOGCALWAYS2 ("xlong_algn_val\t= 0x%04x\t (offset %d)", xlong_algn_val, o);
    SLOGCALWAYS2 ("float_algn_val\t= 0x%04x\t (offset %d)", float_algn_val, o);
    SLOGCALWAYS2 ("dbl_algn_val\t= 0x%04x\t (offset %d)", double_algn_val, o);
    SLOGCALWAYS3 ("str0 algn\t= %d (offset %d), size\t= %d", 
    		strstart_algn0, o, str0_size);
    SLOGCALWAYS3 ("str1 algn\t= %d (offset %d), size\t= %d", 
    		strstart_algn1, o, str1_size);
    SLOGCALWAYS3 ("str2 algn\t= %d (offset %d), size\t= %d", 
    		strstart_algn2, o, str2_size);
    SLOGCALWAYS3 ("str3 algn\t= %d (offset %d), size\t= %d", 
    		strstart_algn3, o, str3_size);
    SLOGCALWAYS3 ("str4 algn\t= %d (offset %d), size\t= %d", 
    		strstart_algn4, o, str4_size);
    SLOGCALWAYS3 ("str5 algn\t= %d (offset %d), size\t= %d", 
    		strstart_algn5, o, str5_size);
    SLOGCALWAYS3 ("str6 algn\t= %d (offset %d), size\t= %d", 
    		strstart_algn6, o, str6_size);
    SLOGCALWAYS3 ("str7 algn\t= %d (offset %d), size\t= %d", 
    		strstart_algn7, o, str7_size);
    SLOGCALWAYS3 ("str8 algn\t= %d (offset %d), size\t= %d", 
    		strstart_algn8, o, str8_size);
    }  

/* First check to see if all structures are aligned the same regardless	*/
/* of their members. If so, we don't consider the members at all.	*/
/* If this is not the case, determine whether the first or largest 	*/
/* element controls the alignment.					*/

  if (strstart_algn0 == strstart_algn1 &&
      strstart_algn0 == strstart_algn2 &&
      strstart_algn0 == strstart_algn3)
    {
    strstart_mode = M_STRSTART_MODE_NONE;
    strstart_algn_val = strstart_algn0;
    }
  else if (strstart_algn0 == byte_algn_val &&
           strstart_algn1 == short_algn_val &&
           strstart_algn2 == long_algn_val &&
           strstart_algn3 == long_algn_val)
    {
    strstart_mode = M_STRSTART_MODE_LARGEST;
    strstart_algn_val = 0;
    }
  else if (strstart_algn0 == byte_algn_val &&
           strstart_algn1 == short_algn_val &&
           strstart_algn2 == long_algn_val &&
           strstart_algn3 == byte_algn_val)
    {
    strstart_mode = M_STRSTART_MODE_FIRST;
    strstart_algn_val = 0;
    }
  else
    {
    SLOGALWAYS0 ("Warning: Structure Start alignment consistancy problem");
    strstart_mode = M_STRSTART_MODE_NONE;
    strstart_algn_val = strstart_algn0;
    }

/* Now do verification checks */
  if (strstart_mode == M_STRSTART_MODE_NONE)
    {
    if (strstart_algn4 != strstart_algn0 ||
        strstart_algn5 != strstart_algn0 ||
        strstart_algn6 != strstart_algn0 ||
        strstart_algn7 != strstart_algn0 ||
        strstart_algn8 != strstart_algn0)
      {
      SLOGALWAYS0 ("Warning: Structure Start alignment consistancy problem");
      strstart_mode = M_STRSTART_MODE_NONE;
      strstart_algn_val = strstart_algn0;
      }
    }
  else if (strstart_mode == M_STRSTART_MODE_LARGEST)
    {
    if (strstart_algn4 != long_algn_val ||
        strstart_algn5 != long_algn_val ||
        strstart_algn6 != byte_algn_val ||
        strstart_algn7 != short_algn_val ||
        strstart_algn8 != long_algn_val)
      {
      SLOGALWAYS0 ("Warning: Structure Start alignment consistancy problem");
      strstart_mode = M_STRSTART_MODE_NONE;
      strstart_algn_val = strstart_algn0;
      }
    }
  else if (strstart_mode == M_STRSTART_MODE_FIRST)
    {
    if (strstart_algn4 != byte_algn_val ||
        strstart_algn5 != short_algn_val ||
        strstart_algn6 != byte_algn_val ||
        strstart_algn7 != short_algn_val ||
        strstart_algn8 != long_algn_val)
      {
      SLOGALWAYS0 ("Warning: Structure Start alignment consistancy problem");
      strstart_mode = M_STRSTART_MODE_NONE;
      strstart_algn_val = strstart_algn0;
      }
    }

/* OK, now do structure end alignment detection */
  if (str0_size == 1 &&
      str1_size == 2 &&
      str2_size == 8 &&
      str3_size == 12 &&
      str4_size % 4 == 0 &&
      str5_size % 4 == 0 &&
      str6_size == 3 &&
      str7_size % 2 == 0 &&
      str8_size % 4 == 0)
    {
    strend_mode = M_STREND_MODE_LARGEST;
    strend_algn_val = 0;
    }
  else
    {
    strend_mode = M_STREND_MODE_NONE;
    strend_algn_val = str0_size - 1;
    }

  o = offsetof (ALGN_ARRSTART_TEST, y);
  arrstart_algn_val = _getAlgnVal (o);
  arrend_algn_val = 0;

  algn_tbl_out[ARRSTRT_ALGN] = arrstart_algn_val;
  algn_tbl_out[ARREND_ALGN]  = arrend_algn_val;
  algn_tbl_out[STRSTRT_ALGN] = strstart_algn_val;
  algn_tbl_out[STREND_ALGN]  = strend_algn_val;
  algn_tbl_out[INT8_ALGN]    = byte_algn_val;
  algn_tbl_out[INT16_ALGN]   = short_algn_val;
  algn_tbl_out[INT32_ALGN]   = long_algn_val;
  algn_tbl_out[INT64_ALGN]   = xlong_algn_val;
  algn_tbl_out[FLOAT_ALGN]   = float_algn_val;
  algn_tbl_out[DOUBLE_ALGN]  = double_algn_val;
  algn_tbl_out[OCT_ALGN]     = byte_algn_val;
  algn_tbl_out[BOOL_ALGN]    = byte_algn_val;
  algn_tbl_out[BCD1_ALGN]    = byte_algn_val;
  algn_tbl_out[BCD2_ALGN]    = short_algn_val;
  algn_tbl_out[BCD4_ALGN]    = long_algn_val;
  algn_tbl_out[BIT_ALGN]     = byte_algn_val;
  algn_tbl_out[VIS_ALGN]     = byte_algn_val;

  *strstart_mode_out = strstart_mode;
  *strend_mode_out = strend_mode;


  SLOGALWAYS0 ("MMS-EASE DATA ALIGNMENT TABLE");
  SLOGALWAYS0 ("");
  SLOGCALWAYS0 ("ST_INT m_def_data_algn_tbl[NUM_ALGN_TYPES] =");
  SLOGCALWAYS0 ("  {");
  SLOGCALWAYS1 ("  0x%04x,\t /* ARRSTRT_ALGN 00\t*/", algn_tbl_out[ARRSTRT_ALGN]);
  SLOGCALWAYS1 ("  0x%04x,\t /* ARREND_ALGN 01\t*/", algn_tbl_out[ARREND_ALGN]);
  SLOGCALWAYS1 ("  0x%04x,\t /* STRSTRT_ALGN 02\t*/", algn_tbl_out[STRSTRT_ALGN]);
  SLOGCALWAYS1 ("  0x%04x,\t /* STREND_ALGN 03\t*/", algn_tbl_out[STREND_ALGN]);
  SLOGCALWAYS1 ("  0x%04x,\t /* INT8_ALGN 04\t*/", algn_tbl_out[INT8_ALGN]);
  SLOGCALWAYS1 ("  0x%04x,\t /* INT16_ALGN 05\t*/", algn_tbl_out[INT16_ALGN]);
  SLOGCALWAYS1 ("  0x%04x,\t /* INT32_ALGN 06\t*/", algn_tbl_out[INT32_ALGN]);
  SLOGCALWAYS1 ("  0x%04x,\t /* INT64_ALGN 07\t*/", algn_tbl_out[INT64_ALGN]);
  SLOGCALWAYS1 ("  0x%04x,\t /* FLOAT_ALGN 08\t*/", algn_tbl_out[FLOAT_ALGN]);
  SLOGCALWAYS1 ("  0x%04x,\t /* DOUBLE_ALGN 09\t*/", algn_tbl_out[DOUBLE_ALGN]);
  SLOGCALWAYS1 ("  0x%04x,\t\t /* OCT_ALGN 10\t*/", algn_tbl_out[OCT_ALGN]);
  SLOGCALWAYS1 ("  0x%04x,\t /* BOOL_ALGN 11\t*/", algn_tbl_out[BOOL_ALGN]);
  SLOGCALWAYS1 ("  0x%04x,\t /* BCD1_ALGN 12\t*/", algn_tbl_out[BCD1_ALGN]);
  SLOGCALWAYS1 ("  0x%04x,\t /* BCD2_ALGN 13\t*/", algn_tbl_out[BCD2_ALGN]);
  SLOGCALWAYS1 ("  0x%04x,\t /* BCD4_ALGN 14\t*/", algn_tbl_out[BCD4_ALGN]);
  SLOGCALWAYS1 ("  0x%04x,\t /* BIT_ALGN 15\t*/", algn_tbl_out[BIT_ALGN]);
  SLOGCALWAYS1 ("  0x%04x \t /* VIS_ALGN 16\t*/", algn_tbl_out[VIS_ALGN]);
  SLOGCALWAYS0 ("  };");
  SLOGCALWAYS0 ("");

  if (strstart_mode == M_STRSTART_MODE_NONE)
    {
    SLOGCALWAYS0 ("#define M_STRSTART_MODE\tM_STRSTART_MODE_NONE");
    }
  else if (strstart_mode == M_STRSTART_MODE_LARGEST)
    {
    SLOGCALWAYS0 ("#define M_STRSTART_MODE\tM_STRSTART_MODE_LARGEST");
    }
  else if (strstart_mode == M_STRSTART_MODE_FIRST)
    {
    SLOGCALWAYS0 ("#define M_STRSTART_MODE\tM_STRSTART_MODE_FIRST");
    }

  if (strend_mode == M_STREND_MODE_NONE)
    {
    SLOGCALWAYS0 ("#define M_STREND_MODE\t\tM_STREND_MODE_NONE");
    }
  else if (strend_mode == M_STREND_MODE_LARGEST)
    {
    SLOGCALWAYS0 ("#define M_STREND_MODE\t\tM_STREND_MODE_LARGEST");
    }
  }

/************************************************************************/
/************************************************************************/

ST_INT _getAlgnVal (ST_INT o)
  {
ST_INT algn;

  switch (o)
    {
    case 1 :
      algn = 0x0000;
    break;
    case 2 :
      algn = 0x0001;
    break;
    case 4 :
      algn = 0x0003;
    break;
    case 8 :
      algn = 0x0007;
    break;
    default :
      algn = -1;
    break;
    }
  return (algn);
  }


/************************************************************************/
/************************************************************************/
/* AUXILIARY FUNCTIONS 							*/
/************************************************************************/
/*				mSetLogCfg 				*/
/************************************************************************/
/* Set up the SLOG subsystem, so we can log all MMS activity. This is 	*/
/* very useful for system level debugging, and can easily be extended 	*/
/* to log user events as well.						*/

static ST_VOID mSetLogCfg (ST_VOID)
  {
#if defined(DEBUG_SISCO)
LOG_CTRL *lc;

/* Initialize the logging for MMS-EASE					*/
  lc = (LOG_CTRL *) chk_calloc (1,sizeof (LOG_CTRL));
  sLogCtrl = lc;

  lc->mc.ctrl = MEM_CTRL_MSG_HDR_EN;
  lc->logCtrl = LOG_FILE_EN;

/* Use differential time log						*/
  lc->logCtrl |= LOG_TIME_EN;

/* File Logging Control defaults 					*/
  lc->fc.fileName = "findalgn.log";
  lc->fc.maxSize = 1000000L;
  lc->fc.ctrl = (FIL_CTRL_WIPE_EN | 
                 FIL_CTRL_WRAP_EN | 
                 FIL_CTRL_NO_APPEND | 
                 FIL_CTRL_MSG_HDR_EN);
#endif
  }

/************************************************************************/
/*			is_little_endian				*/
/* Check byte order (big-endian or little-endian).			*/
/* NOTE: Intel processors are little-endian.				*/
/* Returns: 1 (TRUE) if processor is little-endian			*/
/*	    0 (FALSE) if processor is big-endian			*/
/************************************************************************/
ST_INT8 is_little_endian()
  {
ST_INT endian_test_int = 1;

  return (*((ST_INT8 *) &endian_test_int));
  }
