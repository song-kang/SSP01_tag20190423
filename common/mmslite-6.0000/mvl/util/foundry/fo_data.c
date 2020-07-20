/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	1986 - 1997, All Rights Reserved		        */
/*									*/
/* MODULE NAME : fo_data.c    						*/
/* PRODUCT(S)  : MMSEASE-LITE						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*	NONE								*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 08/26/09  JRB     17    Cast fprintf args to avoid warnings.		*/
/* 04/16/07  JRB     16    Add prim_count to output file.		*/
/* 08/09/06  JRB     15    Write mvl_rt_tables only if typeCount>0.	*/
/* 07/12/04  JRB     14    Use get_el_tag_text.				*/
/* 11/24/03  JRB     13    Del "type twinning" code. Duplicate types	*/
/*			   should be avoided in "input" ODF files.	*/
/* 10/20/03  JRB     12    Fix print formats.				*/
/* 09/17/03  JRB     11    Set rdIndFunIndex, wrIndFunIndex=-1 if not used.*/
/*			   assert if rdIndexBuf or wrIndexBuf == NULL.	*/
/* 04/14/03  JRB     10    Eliminate compiler warnings.			*/
/* 03/13/03  JRB     09    Use RUNTIME_CTRL *rt_ctrl in TYPE_CTRL.	*/
/* 12/17/02  MDE     08    Chk for MVLU_STRING_REF_PREFIX.		*/
/* 11/27/02  MDE     07    Support for XML, etc..			*/
/* 07/13/00  JRB     06    Generate "#ifdef USE_RT_TYPE_2" code.	*/
/* 04/26/00  JRB     05    In RUNTIME_TYPE init, Add {} inside union.	*/
/* 10/13/99  JRB     04    Remove extra commas in writeRtEl.    */
/* 09/21/98  MDE     03    Minor lint cleanup				*/
/* 09/10/98  MDE     02    Added ST_CONST, minor fixes for handling 	*/
/*			   UCA/non-UCA types, twinned types		*/
/* 02/24/98  MDE     01    New						*/
/************************************************************************/

#include "foundry.h"

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/
/************************************************************************/

#if 0	/* DEBUG: add when needed	*/
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/

ST_VOID writeTypeHeader (ST_INT tCount, TYPE_CTRL *tc);
ST_VOID writeRtEl (RUNTIME_TYPE *rt);
ST_VOID writeTypeTrailer (TYPE_CTRL *tc);
ST_VOID writeMvlRtTables (ST_VOID);

/************************************************************************/

ST_INT rtCount;
ST_INT numRtInType;

/************************************************************************/

ST_VOID writeRtData ()
  {
ST_INT j;
RUNTIME_TYPE *rt;
TYPE_CTRL *current_type;
ST_INT tCount;

  fprintf (codeFp, "\n\n%s", commentBar);
  fprintf (codeFp, "\n/* RUNTIME TYPE DATA */\n");
  fprintf (codeFp, "\n\n#if !defined USR_SUPPLIED_RT");
  tCount = 0;
  current_type = typeHead;
  while (current_type != NULL)
    {
    ucaType = current_type->ucaType;
#ifdef OBSOLETE_TWINNING_ENABLE		/* DEBUG: delete later?	*/
    if (current_type->twin)
      {
      current_type = (TYPE_CTRL *) list_get_next (typeHead, current_type);
      continue;
      }
#endif
    if (current_type->rt_ctrl->rt_num == 0)
      break;
    rt = current_type->rt_ctrl->rt_first;
    writeTypeHeader (tCount, current_type);
    for (j = 0; j < current_type->rt_ctrl->rt_num; ++j, ++rt)
      writeRtEl (rt);

    writeTypeTrailer (current_type);
#ifdef USE_RUNTIME_CTRL
/* NOTE: may use this code someday if "rt" & "num_rt" replaced with	*/
/*      "RUNTIME_CTRL *rt_ctrl" in "MVL_TYPE_CTRL".			*/
    fprintf (codeFp, "\nSD_CONST static RUNTIME_CTRL mvl_rt_ctrl_%d = {mvl_rt_table_%d,%d};", 
				tCount, tCount, current_type->rt_ctrl->rt_num);
#endif	/* USE_RUNTIME_CTRL*/
    ++tCount;
    current_type = (TYPE_CTRL *) list_get_next (typeHead, current_type);
    }
  writeMvlRtTables ();
  fprintf (codeFp, "\n#endif /* #if defined USR_SUPPLIED_RT */\n\n");
  }

/************************************************************************/
/*                       writeTypeHeader				*/
/************************************************************************/

ST_VOID writeTypeHeader (ST_INT tCount, TYPE_CTRL *tc)
  {
  fprintf (codeFp, "\n\n%s\n", commentBar);
  fprintf (codeFp, "/* %s : %s */\n", tc->label, tc->desc);
  fprintf (codeFp, "\nSD_CONST static RUNTIME_TYPE mvl_rt_table_%d[%d] =", 
				tCount, tc->rt_ctrl->rt_num);
  fprintf (codeFp, "\n  {");
  rtCount = 0;
  numRtInType = tc->rt_ctrl->rt_num;
  }

/************************************************************************/
/*                       writeRtEl					*/
/************************************************************************/

ST_VOID writeRtEl (RUNTIME_TYPE *rt)
  {
ST_INT stringIndex;
ST_CHAR *p;
ST_CHAR *el_tag_text;
  el_tag_text = get_el_tag_text (rt);
  assert (el_tag_text != NULL);		/* Must always get valid text.	*/

  fprintf (codeFp, "\n    {\t\t\t\t/* rt[%d] init data ...\t\t\t*/", rtCount);
  fprintf (codeFp, "\n    %s,\t\t/* el_tag\t\t\t\t*/", el_tag_text);
  fprintf (codeFp, "\n    %d,\t\t\t\t/* el_size\t\t\t\t*/", rt->el_size);
  fprintf (codeFp, "\n    %ld,\t\t\t\t/* offset_to_last\t\t\t*/", (ST_LONG) rt->offset_to_last);

  fprintf (codeFp, "\n      {\t\t\t\t/* u\t\t\t\t\t*/");
  fprintf (codeFp, "\n      {\t\t\t\t/* p\t\t\t\t\t*/");
  if (rt->el_tag == RT_STR_START || rt->el_tag == RT_STR_END)
    {
    fprintf (codeFp, "\n      %d,\t\t\t/*   num_rt_blks\t\t\t\t*/", rt->u.str.num_rt_blks);
    fprintf (codeFp, "\n      0\t\t\t/*   pad\t\t\t\t\t*/");
    }
  else if (rt->el_tag == RT_ARR_START || rt->el_tag == RT_ARR_END)
    {
    fprintf (codeFp, "\n      %d,\t\t\t/*   num_elmnts\t\t\t\t*/", rt->u.arr.num_elmnts);
    fprintf (codeFp, "\n      %d\t\t\t/*   num_rt_blks\t\t\t\t*/", rt->u.arr.num_rt_blks);
    }
  else
    {
    fprintf (codeFp, "\n      %d,\t\t\t/*   el_len\t\t\t\t*/", rt->u.p.el_len);
    fprintf (codeFp, "\n      0\t\t\t/*   pad\t\t\t\t*/");
    }
  fprintf (codeFp, "\n      }\t\t\t\t/* end 'p'\t\t\t\t*/");
  fprintf (codeFp, "\n      },\t\t\t/* end 'u'\t\t\t\t*/");

  if (ms_comp_name_pres(rt))
    {
    stringIndex = getCommonStringIndex (ms_comp_name_find(rt));
    if (stringIndex != -1)
      {
      fprintf (codeFp, "\n#ifdef USE_RT_TYPE_2");
      fprintf (codeFp, "\n    mvlCompName_%s, \t/* comp_name_ptr '%s'\t\t\t\t*/", 
				mvl_rt_names[stringIndex],
				mvl_rt_names[stringIndex]);
      fprintf (codeFp, "\n#else\t/* !USE_RT_TYPE_2\t*/");
      fprintf (codeFp, "\n    %s, \t/* name_index '%s'\t\t\t\t*/", 
				stringIndexToDefine (stringIndex),
				mvl_rt_names[stringIndex]);
      fprintf (codeFp, "\n#endif\t/* !USE_RT_TYPE_2\t*/");
      }
    else
      printf ("\n Internal error: common string not found");
    }
  else
    {
    fprintf (codeFp, "\n#ifdef USE_RT_TYPE_2");
    fprintf (codeFp, "\n    NULL,\t\t\t\t/* comp_name_ptr \t\t\t\t*/"); 
    fprintf (codeFp, "\n#else\t/* !USE_RT_TYPE_2\t*/");
    fprintf (codeFp, "\n    0,\t\t\t\t/* name_index \t\t\t\t*/"); 
    fprintf (codeFp, "\n#endif\t/* !USE_RT_TYPE_2\t*/");
    }

  if (mvl_uca == SD_TRUE)
    {
    fprintf (codeFp, "\n      {\t\t\t\t/* mvluTypeInfo\t\t\t\t*/");
    fprintf (codeFp, "\n      %d,\t\t\t/*   prim_count\t\t\t\t*/", rt->mvluTypeInfo.prim_count);
    fprintf (codeFp, "\n      %d,\t\t\t/*   sortedNum\t\t\t\t*/", rt->mvluTypeInfo.sortedNum);
    fprintf (codeFp, "\n      %ld,\t\t\t/*   offSet\t\t\t\t*/", (ST_LONG) rt->mvluTypeInfo.offSet);

    if (ucaType && ms_comp_name_pres(rt) && ms_is_rt_prim (rt) == SD_TRUE)
      {
      assert (rt->rdIndexBuf);	/* buffer must be valid	*/
      assert (rt->wrIndexBuf);	/* buffer must be valid	*/
      fprintf (codeFp, "\n      %s,\t/*   rdIndFunIndex\t\t*/", rt->rdIndexBuf);
      fprintf (codeFp, "\n      %s,\t/*   wrIndFunIndex\t\t*/", rt->wrIndexBuf);

      if (rt->refBuf != NULL)
        {
        if (!strncmp (rt->refBuf, MVLU_STRING_REF_PREFIX, strlen (MVLU_STRING_REF_PREFIX)))
          {	/* Write everything after PREFIX as quoted string.	*/
          p = rt->refBuf + strlen(MVLU_STRING_REF_PREFIX);
          fprintf (codeFp, "\n      (ST_RTREF)\"%s\"\t\t\t/*   ref\t\t\t*/", p);
	  }
        else	/* Write entire string WITHOUT quotes.	*/
          {
          fprintf (codeFp, "\n      %s\t\t\t/*   ref\t\t\t*/", rt->refBuf);
          }
	}
      else
        fprintf (codeFp, "\n      0\t\t\t\t/*   ref (not defined)\t*/");
      }
    else
      {
      fprintf (codeFp, "\n      -1,\t\t\t/*   rdIndFunIndex\t\t\t*/");
      fprintf (codeFp, "\n      -1,\t\t\t/*   wrIndFunIndex\t\t\t*/");
      fprintf (codeFp, "\n      0\t\t\t/*   ref\t\t\t*/");
      }
    fprintf (codeFp, "\n      }");
    }

  ++rtCount;
  if (rtCount == numRtInType)
    fprintf (codeFp, "\n    }");
  else
    fprintf (codeFp, "\n    },");
  }

/************************************************************************/
/*                       writeTypeTrailer				*/
/************************************************************************/

ST_VOID writeTypeTrailer (TYPE_CTRL *tc)
  {
  fprintf (codeFp, "\n  };");
  }

/************************************************************************/
/*                       writeMvlRtTables				*/
/************************************************************************/

ST_VOID writeMvlRtTables ()
  {
ST_INT i;

  /* If any entries, create "mvl_rt_tables" array	*/
  if (typeCount > 0)
    {
    fprintf (codeFp, "\n\n%s\n", commentBar);
    fprintf (codeFp, "\n\nST_INT rt_table_index;");

    fprintf (codeFp, "\n\nSD_CONST RUNTIME_TYPE * SD_CONST mvl_rt_tables[] =");
    fprintf (codeFp, "\n  {");
    for (i = 0; i < typeCount; ++i)
      {
      fprintf (codeFp, "\n  mvl_rt_table_%d", i);
      if (i < typeCount-1)
        fprintf (codeFp, ",");
      }
    fprintf (codeFp, "\n  };\n\n");
    }
  }

