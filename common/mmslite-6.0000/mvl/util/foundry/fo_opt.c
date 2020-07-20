/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,       */
/*               1997-2001, All Rights Reserved                         */
/*                                                                      */
/* MODULE NAME : fo_opt.c                                              	*/
/* PRODUCT(S)  :                                                        */
/*                                                                      */
/* MODULE DESCRIPTION :                                                 */
/*  Program to convert type list to C code which sets up runtime table  */
/*                                                                      */
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :                            */
/*              main                                                    */
/*                                                                      */
/* MODIFICATION LOG :                                                   */
/*  Date     Who   Rev                  Comments                        */
/* --------  ---  ------   -------------------------------------------  */
/* 12/04/03  MDE     08    Changed duplicate printf text		*/
/* 11/24/03  JRB     07    Del "type twinning" code. Duplicate types	*/
/*			   should be avoided in "input" ODF files.	*/
/* 04/01/03  JRB     06    Add UTF8string support (see RT_UTF8_STRING).	*/
/* 03/13/03  JRB     05    Use RUNTIME_CTRL *rt_ctrl in TYPE_CTRL.	*/
/* 11/11/02  JRB     04    Use ms_runtime_destroy.			*/
/* 11/19/01  EJV     03    Added support for new MMS type UtcTime:	*/
/*			   rtCmp: added case for RT_UTC_TIME;		*/
/* 09/21/98  MDE     02    Minor lint cleanup				*/
/* 07/21/98  MDE     01    New module, extracted from foundry.c		*/
/************************************************************************/

#include "foundry.h"

/************************************************************************/

SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;

/************************************************************************/

static ST_BOOLEAN typeCmp (TYPE_CTRL *this_type, TYPE_CTRL *next_type);
static ST_BOOLEAN rtCmp (RUNTIME_TYPE *rt1, RUNTIME_TYPE *rt2);
static ST_VOID freeType (TYPE_CTRL *t);

/************************************************************************/
/************************************************************************/
/*                       optimizeTypes                                  */
/************************************************************************/

ST_RET optimizeTypes ()
  {
TYPE_CTRL *current_type;
TYPE_CTRL *this_type;
TYPE_CTRL *next_type;

  if (verbose == SD_TRUE)
    {
    printf ("\rRemoving transient and unreferenced base types ...");
    }

/* Remove transient and unreferenced base types ... */
  if (debug == SD_TRUE)
    {
    SLOGALWAYS0 ("Removing transient and unreferenced base types ...");
    }

  current_type = typeHead;
  while (current_type != NULL)
    {
    next_type = (TYPE_CTRL *) list_get_next (typeHead, current_type);

  /* See if we should delete this type from the list */
    if (current_type->varRefCount == 0 && current_type->keep == SD_FALSE)
      {
      if (current_type->transient)
        {
        if (debug == SD_TRUE)
          {
          SLOGCALWAYS2 ("  Removing transient type '%s' (%d ref)", 
                          current_type->label, current_type->baseRefCount);
          }
        freeType (current_type);
        }
      else if (current_type->base && 
               current_type->baseRefCount == 0)
        {
        if (debug == SD_TRUE)
          {
          SLOGCALWAYS1 ("  Removing unreferenced base type '%s'", 
                                          current_type->label);
          }
        freeType (current_type);
        }
      else if (current_type->deleteIfUnused)
        {
        if (debug == SD_TRUE)
          {
          SLOGCALWAYS1 ("  Removing unreferenced variable type '%s'", 
                                          current_type->label);
          }
        freeType (current_type);
        }
      }

    current_type = next_type;
    }

/* Optimize by eliminating duplicate types */
  if (debug == SD_TRUE)
    {
    SLOGALWAYS0 ("Eliminating Twins"); 
    }
  this_type = typeHead;
  while (this_type != NULL)
    {
    next_type = (TYPE_CTRL *) list_get_next (typeHead, this_type);
    if (this_type->twin == NULL &&              /* We are not a twin    */
        this_type->unique != SD_TRUE)           /* We may have some ... */
      {
      while (next_type != NULL)                 /* Find our twins       */
        {
        if (next_type->twin == NULL &&          /* next not twin'd      */
            next_type->unique != SD_TRUE)       /* and can be ...       */
          {
          if (typeCmp (this_type, next_type) == SD_TRUE)
            {
            if (debug == SD_TRUE)
              {
              SLOGCALWAYS2 ("  Type '%s' found twin '%s'", 
                                this_type->label, next_type->label);
              }
            next_type->twin = this_type;
#ifdef OBSOLETE_TWINNING_ENABLE		/* DEBUG: delete later?	*/
            --typeCount;
            totalRtCount -= next_type->rt_ctrl->rt_num;
#else
            printf ("  Note: Type '%-24s' is identical to '%-24s'\n",
                                next_type->label, this_type->label);
            SLOGALWAYS2 ("!!! WARNING: Type '%s' is a duplicate of '%s'. WASTES MEMORY.",
                                next_type->label, this_type->label);
#endif
            }
          }
        next_type = (TYPE_CTRL *) list_get_next (typeHead, next_type);
        }
      }
    this_type = (TYPE_CTRL *) list_get_next (typeHead, this_type);
    }
  return (SD_SUCCESS);
  }  

/************************************************************************/
/*                      typeCmp                                         */
/************************************************************************/

static ST_BOOLEAN typeCmp (TYPE_CTRL *this_type, TYPE_CTRL *next_type)
  {
ST_INT i;

/* Simple comparision first */
  if (!strcmp (this_type->tdl, next_type->tdl))
    return (SD_TRUE);

/* OK, now look at the runtime type for a more in depth comparison      */
  if (this_type->rt_ctrl->rt_num != next_type->rt_ctrl->rt_num)
    return (SD_FALSE);

  for (i = 0; i < this_type->rt_ctrl->rt_num; ++i)
    {
    if (rtCmp (&this_type->rt_ctrl->rt_first[i], &next_type->rt_ctrl->rt_first[i]) != SD_TRUE)
      return (SD_FALSE);
    }

  return (SD_TRUE);
  }

/************************************************************************/
/*                      rtCmp                                           */
/************************************************************************/

static ST_BOOLEAN rtCmp (RUNTIME_TYPE *rt1, RUNTIME_TYPE *rt2)
  {
  if (rt1->el_tag != rt2->el_tag)
    return (SD_FALSE);

  if (rt1->el_size != rt2->el_size)
    return (SD_FALSE);

  if (strcmp (rt1->name, rt2->name))
    return (SD_FALSE);

/* OK, rt type tag specific checks */
  switch(rt1->el_tag)
    {
  /*  Primitive types.                                                  */
    case RT_BOOL:
    case RT_BIT_STRING:
    case RT_INTEGER:
    case RT_UNSIGNED:
    case RT_FLOATING_POINT:
    case RT_OCTET_STRING:
    case RT_VISIBLE_STRING:
    case RT_GENERAL_TIME:
    case RT_BINARY_TIME:
    case RT_BCD:
    case RT_BOOLEANARRAY:
    case RT_UTC_TIME:
    case RT_UTF8_STRING:
      if (rt1->u.p.el_len != rt2->u.p.el_len)
        return (SD_FALSE);
    break;

    case RT_STR_START:
    case RT_STR_END:
      if (rt1->u.str.num_rt_blks != rt2->u.str.num_rt_blks)
        return (SD_FALSE);
    break;

    case RT_ARR_START:
    case RT_ARR_END:
      if (rt1->u.arr.num_elmnts != rt2->u.arr.num_elmnts)
        return (SD_FALSE);

      if (rt1->u.arr.num_rt_blks != rt2->u.arr.num_rt_blks)
        return (SD_FALSE);
    break;

    default:
      SLOGALWAYS1 ("Internal error, unknown runtime type asn1_tag 0x%02x!\n",
                                rt1->el_tag);
      exitApp ();
    break;
    }

  return (SD_TRUE);
  }


/************************************************************************/
/*                      freeType                                        */
/************************************************************************/

static ST_VOID freeType (TYPE_CTRL *t)
  {
NAME_CTRL *currName;

  --typeCount;
  totalRtCount -= t->rt_ctrl->rt_num;

  list_unlink ((ST_VOID **) &typeHead, t);

/* Before we free this type, see of the user wants the typedef anyway .. */
  if (t->wantTypedef == SD_TRUE)
    {
    list_add_last ((ST_VOID **) &typedefOnlyHead, t);
    return;
    }

/* First verify that this was not listed in the name extraction section */
  currName = nameListHead;
  while (currName != NULL)
    {
    if (currName->typeCtrl == t)
      {
      SLOGCALWAYS1 ("Warning: Type '%s' optimized away, used for name extraction", t->label);
      return;
      }
    currName = (NAME_CTRL *) list_get_next (nameListHead, currName);
    }

  chk_free (t->desc);
  chk_free (t->label);
  chk_free (t->tdl);
  ms_runtime_destroy (t->rt_ctrl);
  chk_free (t->asn1);
  chk_free (t);
  }


