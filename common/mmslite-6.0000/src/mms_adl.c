/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	  1986 - 1997, All Rights Reserved.		        */			
/*									*/
/*		    PROPRIETARY AND CONFIDENTIAL			*/
/*									*/
/* MODULE NAME : mms_adl.c						*/
/* PRODUCT(S)  : MMSEASE						*/
/*									*/
/* MODULE DESCRIPTION : 						*/
/* Function to convert ADL (Alternate Access Definition Language) into 	*/
/* alternate access structure						*/
/*									*/
/* GLOBAL FUNCTIONS DEFINED IN THIS MODULE :				*/
/*									*/
/* MODIFICATION LOG :							*/
/*  Date     Who   Rev			Comments			*/
/* --------  ---  ------   -------------------------------------------	*/
/* 03/09/12  JRB	   Verify array index >= 0 BEFORE using it.	*/
/* 11/23/11  KCR           Added casting for proper 64bit compile       */
/* 08/26/09  JRB     08    Cast sprintf args to avoid warnings.		*/
/* 08/09/06  JRB     07    Added ms_adl_to_asn1_2.			*/
/* 09/13/99  MDE     06    Added SD_CONST modifiers			*/
/* 06/15/98  MDE     05    Changes to allow compile under C++		*/
/* 02/20/98  MDE     04    Minor variable initialization changes	*/
/* 12/29/97  MDE     03    Corrected LOCK/UNLOCK problems		*/
/* 12/10/97  MDE     02    Added logging for invalid sel_type		*/
/* 05/21/97  IKE     01	   Used '[' instead of ':' for nesting for an	*/
/* 			   array of structures				*/
/* 04/02/97  DTL   7.00    MMSEASE 7.0 release. See MODL70.DOC for	*/
/*			   history.					*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"
#include "glbsem.h"
#include <ctype.h>
#include "mmsdefs.h"
#include "mms_pvar.h"
#include "mms_vvar.h"

/************************************************************************/

#define MAX_NESTING_LEVEL	50
#define MAX_NUM_AA	       	100	

#define AA_STACK_EMPTY(a)	(!((a)->brace_number)) /* is stack empty*/
#define AA_STACK_FULL(a)	((a)->brace_number == MAX_NESTING_LEVEL)

#define L_ANGLE_BRACKET		'<'
#define R_ANGLE_BRACKET		'>'
#define	L_BRACE			'['
#define	R_BRACE			']'
#define	COMMA			','
#define	COLON			':'
#define	ASTERISK		'*'
#define NULL_CHARACTER		'\0'

#define ARRAY_ACCESS		0
#define STRUCTURE_ACCESS	1
#define L_BRACE_PARSE		2
#define R_BRACE_PARSE		3
#define COMMA_PARSE		4
#define COLON_PARSE		5
#define FINISH_PARSE		6

/* The following data structure is used to keep track of information	*/
/* needed to handle ADL string internally. 				*/

struct adl_info
  {
  ST_BOOLEAN comp_name_exists; /* is it a named alternate acces exist ?	*/
  ST_INT named_index;	    /* where exactly is the index of the name ?	*/
  ST_CHAR comp_name[MAX_IDENT_LEN+1]; /* what is the name of the component?*/
  ST_INT nest_count[MAX_NESTING_LEVEL]; /* nesting level per brace	*/
  ST_INT comp_count[MAX_NESTING_LEVEL]; /* comp nesting level		*/
  ST_INT brace_number;
  ST_INT access_state;		/* what to be parsed next ?  		*/
  ST_BOOLEAN started_with_structure_access;/*start could be array or struct*/
  ST_BOOLEAN named_comp_allowed;  /* are named components allowed ?  	*/
  ST_BOOLEAN left_brace_allowed;  /* is left brace allowed ?		*/
  };
typedef struct adl_info ADL_INFO;


/************************************************************************/
/* static variables used in this module					*/
/************************************************************************/

SD_CONST static ST_CHAR seps[] = " []<>\t\n:,";
SD_CONST static ST_CHAR ch_seps[] = "[]<>:,";

/************************************************************************/
/* Internal functions for this module					*/
/************************************************************************/
static ST_RET get_alternate_access (ST_CHAR *pString,
			           ALT_ACCESS *alt_acc,
				   ADL_INFO *info);
static ST_RET get_structure_access (ST_CHAR **ppString,
				   ALT_ACCESS *alt_acc,
			           ADL_INFO *info);
static ST_RET get_array_access (ST_CHAR **ppString,
			       ALT_ACCESS *alt_acc,
			       ADL_INFO *info);
static ST_RET process_r_brace (ST_CHAR **ppString,
			      ALT_ACCESS *alt_acc,
			      ADL_INFO *info);
static ST_RET process_comma (ST_CHAR **ppString,
			      ALT_ACCESS *alt_acc,
			      ADL_INFO *info);
static ST_RET process_l_brace (ST_CHAR **ppString,ADL_INFO *info);
static ST_RET process_colon (ST_CHAR **ppString,ADL_INFO *info);
static ST_RET end_of_parsing (ALT_ACCESS *alt_acc,ADL_INFO *info);
static ST_RET get_named_component (ST_CHAR **ppString,ST_CHAR *token);
static ST_RET get_index_identifier (ST_CHAR **ppString,ALT_ACCESS *alt_acc);
static ST_RET get_sel_type (ST_CHAR **ppString,ST_INT type);
static ST_VOID copy_component_name (ALT_ACCESS *alt_acc,ADL_INFO *info);
static ST_RET isnumber (ST_CHAR *pString);
static ST_RET is_iso_identifier (ST_CHAR *pString);
static ST_VOID point_to_next_character (ST_INT length,ST_CHAR **ppString);
static ST_RET token_copy (ST_CHAR *token,ST_CHAR *pString,ST_INT max_token_length);
static ST_RET traverse_array (ALT_ACCESS *, ST_INT, ST_CHAR *,ST_INT);
static ST_RET get_alt_acc (ALT_ACCESS *);
static ST_RET separator_copy (ST_CHAR *adl,ST_INT *i,ST_CHAR separator, ST_INT max_adl_len);
static ST_RET comp_copy (ST_CHAR *adl,ST_INT *i,ST_CHAR *comp, ST_INT max_adl_len);

/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/
/************************************************************************/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/
/*			ms_adl_to_aa					*/
/* Function to parse ADL (Alternate Access Definition Language) and	*/
/* put into alternate structure						*/
/* Input : adl_str User allocates and frees it				*/
/* Output : alt_acc User allocate and frees it				*/
/* Output : alt_acc->aa : This function allocates and user frees it	*/
/* The grammar of ADL is as follows					*/
/*									*/
/* <Alternate Access> ::= <Structure Access> | <Array Access>		*/
/* <Structure Access> ::= <Named Struct. Access> | 			*/
/*			  <Unnamed Struct. Access>			*/
/* <Named Struct. Access> ::= <Named Component><Unnamed Struct. Access>	*/
/* <Unnamed Struct. Access> ::=<Component Access> |			*/
/*		        <Component Access><Comma><Structure Access>	*/
/* <Component Access> ::= <Component> | <Component><Alternate Access>	*/
/* <Array Access> ::="["<Index Identifier>{<Colon><Structure Access>}"]"*/ 
/*			| "["<Index Identifier>{<Array Access>}"]"	*/
/* <Component> ::= ISO Identifier					*/
/* <Named Component> ::= "<"<Component>">"				*/
/* <Index Identifier> ::= <Index> | <Index Range> | <All Elements>	*/
/* <Index Range> ::= <Low Index><Comma><Num of Elements>		*/
/* <Low Index> ::= <Index>						*/
/* <Num of Elements> ::= <Index>					*/
/* <Index> ::= <Digit> | <Digit><Index>					*/
/* <Digit> ::= "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9"*/
/* <All Elements> ::= "*"						*/
/* <Comma> ::= ","							*/
/* <Colon> ::= ":"							*/
/************************************************************************/

ST_RET ms_adl_to_aa (ST_CHAR *adl_str,ALT_ACCESS *alt_acc)
  {
ST_RET ret_value;
ST_CHAR *string;
ST_INT str_length;
ADL_INFO *info;

  S_LOCK_COMMON_RESOURCES ();

/* initialize local variables			*/
  ret_value = MVE_INVALID_ADL;
  str_length = (ST_INT) strlen (adl_str);
  string = (ST_CHAR *) chk_calloc (1,str_length+1);
  info = (ADL_INFO *) chk_calloc (1,sizeof (ADL_INFO));

/* initialize alt_acc 				*/
  alt_acc->num_aa = 0;
  alt_acc->aa = (ALT_ACC_EL *) chk_calloc (MAX_NUM_AA,sizeof (ALT_ACC_EL));

/* make the duplicate copy of the adl_string and use it in the function	*/
/* calls in case any function corrupts the string			*/
  strcpy (string,adl_str);

/* initialize some state control variables				*/
  info->comp_name_exists = SD_FALSE;
  info->named_comp_allowed = SD_FALSE;
  info->left_brace_allowed = SD_TRUE;

/* start parsing							*/
  ret_value = get_alternate_access (string,alt_acc,info);

/* free what we allocated in this function				*/
  chk_free (info);
  chk_free (string);
  if (ret_value != SD_SUCCESS)
    chk_free (alt_acc->aa);

/* Take care of housekeeping						*/
  if (ret_value == SD_SUCCESS)
    {
  /* Log the alternate access, if selected				*/
    if (mms_debug_sel & MMS_LOG_AA)
      ms_log_alt_access (alt_acc);
  /* save the high water mark						*/
    if (alt_acc->num_aa > m_hw_dec_aa)
      m_hw_dec_aa = alt_acc->num_aa;
    }
  S_UNLOCK_COMMON_RESOURCES ();
  return (ret_value);
  }

/************************************************************************/
/*                           ms_aa_to_adl                         	*/
/* Function to extract information from alternate access structure and	*/
/* make ADL string as output						*/	
/* Input : alt_acc user allocates and frees it				*/
/* Output : adl_str : user is responsible and frees it			*/
/************************************************************************/
ST_RET ms_aa_to_adl (ALT_ACCESS *alt_acc, ST_CHAR *adl_str,ST_INT max_adl_len)
  {
ST_INT num_of_els;
ST_RET ret;

  S_LOCK_COMMON_RESOURCES ();
  num_of_els = get_alt_acc (alt_acc);
  if (num_of_els == 0)
    ret = MVE_INVALID_ADL; 
  else
    ret = traverse_array (alt_acc, num_of_els, adl_str,max_adl_len);

  S_UNLOCK_COMMON_RESOURCES ();
    return (ret);
  }

/************************************************************************/
/* 			get_alternate_access				*/
/* This function parses the passed string pString sequentially. While	*/
/* parsing, this function works on state basis and calls the appropriate*/
/* function on the basis. The called functions set the next state	*/
/************************************************************************/

static ST_RET get_alternate_access (ST_CHAR *pString,
				   ALT_ACCESS *alt_acc,
				   ADL_INFO *info)
  {
ST_INT ret_value;
ST_BOOLEAN all_done;

  ret_value = SD_SUCCESS;
  all_done = SD_FALSE;
  point_to_next_character (0,&pString);
  if (*pString == L_BRACE) 
    {
    info->brace_number++;
    info->access_state = ARRAY_ACCESS;
    info->started_with_structure_access = SD_FALSE;
    point_to_next_character (1,&pString);
    }
  else
    {
    info->access_state = STRUCTURE_ACCESS;
    info->started_with_structure_access = SD_TRUE;
    }

  while ((all_done==SD_FALSE) && (ret_value == SD_SUCCESS))
    {
    switch (info->access_state)
      {
      case STRUCTURE_ACCESS :	
        ret_value = get_structure_access (&pString,alt_acc,info);
      break;

      case ARRAY_ACCESS :
        ret_value = get_array_access (&pString,alt_acc,info);
      break;	

      case L_BRACE_PARSE :
	ret_value = process_l_brace (&pString,info);
      break;   
	
      case R_BRACE_PARSE :
	ret_value = process_r_brace (&pString,alt_acc,info);	
      break;

      case COMMA_PARSE :
	ret_value = process_comma (&pString,alt_acc,info);
      break;		

      case COLON_PARSE :
	ret_value = process_colon (&pString,info);
      break;	

      case FINISH_PARSE :
	ret_value = end_of_parsing (alt_acc,info);	
        all_done = SD_TRUE;
      break;

      default:
	MLOG_NERR1("Unexpected %s",pString);
        ret_value = MVE_INVALID_ADL;
      break;	

      }
    }

  if (ret_value != SD_SUCCESS)
    return (MVE_INVALID_ADL);

/* Stack should be empty because all braces should be matched by now	*/
  if (info->brace_number)
    {
    MLOG_NERR0("Braces not matched");
    return (MVE_INVALID_ADL);
    }

  return (SD_SUCCESS);
  }

/************************************************************************/
/*			get_structure_access				*/
/* This function expects the syntax like <name1>comp1 or comp1 in the 	*/
/* beginning. It parses until the component ,checks the next character	*/
/* and sets the next state						*/
/************************************************************************/

static ST_RET get_structure_access (ST_CHAR **ppString,
				   ALT_ACCESS *alt_acc,
				   ADL_INFO *info)
  {
ST_CHAR token[MAX_IDENT_LEN+1];
ST_INT token_length;
ST_RET ret_value;
ST_CHAR *pString;
ALT_ACC_EL *aa;

  ret_value = SD_SUCCESS;
  pString = *ppString;
  aa = alt_acc->aa+alt_acc->num_aa;
  if (*pString == L_ANGLE_BRACKET) /* alternate access has a name	*/
    {
    if (info->comp_name_exists == SD_TRUE) /* name already defined	*/
      {
      MLOG_NERR1("component name is allowed only once.Error at %s",pString);
      return (MVE_INVALID_ADL);
      }
    info->comp_name_exists = SD_TRUE;
    if (get_named_component (&pString,info->comp_name) != SD_SUCCESS)
      return (MVE_INVALID_ADL);
    }
  if (!(token_length = token_copy (token,pString,MAX_IDENT_LEN)))
    {
    MLOG_NERR1("Expected component at %s",pString);
    return (MVE_INVALID_ADL);
    }
  if (is_iso_identifier (token) == SD_FALSE)
    {
    MLOG_NERR1("Component %s is not a valid identifier",token);
    return (MVE_INVALID_ADL);
    }
/* set the aa pointer to the current structure to be filled in		*/
  aa = alt_acc->aa+alt_acc->num_aa; 

/* start filling the structure						*/
  aa->comp_name_pres = info->comp_name_exists;
  if (aa->comp_name_pres)
    strcpy (aa->comp_name,info->comp_name);
  strcpy (aa->u.component,token);
  aa->sel_type = get_sel_type (&pString,AA_COMP);

  if ((info->comp_name_exists == SD_TRUE) && (aa->sel_type == AA_COMP || aa->sel_type == AA_COMP_NEST))
    info->named_index = alt_acc->num_aa;
  if (++alt_acc->num_aa > MAX_NUM_AA)
    {
    MLOG_NERR0("Number of elements in the structure exceeded the limit");	
    return (MVE_INVALID_ADL);
    }
  point_to_next_character (token_length,&pString);  				

/* Setting the next access state					*/
  switch (*pString)
    {
    case L_BRACE :
      info->access_state = L_BRACE_PARSE;
    break;

    case R_BRACE : 
      info->access_state = R_BRACE_PARSE;
    break;

    case COMMA :  
      info->access_state = COMMA_PARSE;
    break;	

    case NULL_CHARACTER :
      info->access_state = FINISH_PARSE;
    break;		

    default :
      MLOG_NERR1("Unexpected %s",pString);
      ret_value = MVE_INVALID_ADL;		
      info->access_state = -1;
    break;
    }
/* update argumet with the current position of the pointer to string	*/
  *ppString = pString;
  return (ret_value);
  }	

/************************************************************************/
/*			get_array_access				*/
/* This function expects the syntax like 2:comp1 or 2[3 or 2,3 in the	*/
/* beginning. It parses to get the index identifier, checks the next 	*/
/* character and sets the next state					*/
/************************************************************************/

static ST_RET get_array_access (ST_CHAR **ppString,
				   ALT_ACCESS *alt_acc,
				   ADL_INFO *info)
 {
ST_CHAR *pString;
ST_RET ret_value;

  ret_value = SD_SUCCESS;
  pString = *ppString;
  if (info->left_brace_allowed == SD_FALSE)
    {
    MLOG_NERR1("Syntax error at %s",pString);
    return (MVE_INVALID_ADL);
    }
  if (get_index_identifier (&pString,alt_acc) != SD_SUCCESS)
    return (MVE_INVALID_ADL);
  if (++alt_acc->num_aa > MAX_NUM_AA)
    {
    MLOG_NERR0("Number of elements in the structure exceeded the limit");	
    return (MVE_INVALID_ADL);
    }
/* Setting the next access  state		*/						

  switch (*pString)
    {
    case L_BRACE :
      info->access_state = L_BRACE_PARSE;
    break;

    case R_BRACE : 
      info->access_state = R_BRACE_PARSE;
    break;

    case COLON :
      info->access_state = COLON_PARSE;	
    break;

    default :
      MLOG_NERR1("Unexpected %s",pString);
      ret_value = MVE_INVALID_ADL;	
      info->access_state = -1;
    break;
    }
/* update argumet with the current position of the pointer to string	*/
  *ppString = pString;
  return (ret_value);
  }

/************************************************************************/
/*			process_l_brace					*/
/* If the left brace is encountered then we expect array access.We also	*/
/* keep track of nesting associated with this brace 			*/
/************************************************************************/

static ST_RET process_l_brace (ST_CHAR **ppString,ADL_INFO *info)
  {
ST_CHAR *pString;

  pString = *ppString;
  if (AA_STACK_FULL (info))
    {
    MLOG_NERR0("Nesting Level exceeded");
    return (MVE_INVALID_ADL);
    }
/* we are nesting one level now	associated with this brace		*/
  info -> nest_count[info->brace_number++] = 1;
  point_to_next_character (1,&pString);	
  if (isdigit (*pString) || (*pString == ASTERISK))
    info->access_state = ARRAY_ACCESS;
  else 
    info->access_state = STRUCTURE_ACCESS;
  *ppString = pString;
  return (SD_SUCCESS);
  }

/************************************************************************/
/*			process_r_brace					*/
/* Right brace is used to end the array access. This function removes 	*/
/* the right brace from the passed string, checks the next character	*/
/* and sets the next state						*/
/************************************************************************/

static ST_RET process_r_brace (ST_CHAR **ppString,
			      ALT_ACCESS *alt_acc,
			      ADL_INFO *info)
  {
ST_CHAR *pString;
ST_RET ret_value;
ST_INT i;
ST_INT brace_number;
ALT_ACC_EL *aa;

  ret_value = SD_SUCCESS;
  aa = alt_acc->aa+alt_acc->num_aa;
  pString = *ppString;

  info->left_brace_allowed = SD_FALSE;
  if (AA_STACK_EMPTY (info))
    {
    MLOG_NERR1("Syntax Error due to right brace at %s",pString);
    return (MVE_INVALID_ADL);
    }
  brace_number = -- info->brace_number;
  for (i=0;i<info->nest_count[brace_number];i++,aa++)
    {
    aa->sel_type = AA_END_NEST;
    if (++alt_acc->num_aa > MAX_NUM_AA)
      {
      MLOG_NERR0("Number of elements in the structure exceeded the limit");	
      return (MVE_INVALID_ADL);
      }
    }
  point_to_next_character (1,&pString);	
  switch (*pString)
    {
    case R_BRACE : 
      info->access_state = R_BRACE_PARSE;
    break;

    case COMMA :  
      /* comma is used to separate between different alternate accesses	*/
      /* it is invalid between between two array accesses like [2],[3]	*/
      if ((info->started_with_structure_access == SD_TRUE) || (info->brace_number))	
	{
        info->access_state = COMMA_PARSE;
	}
      else
	{
	info->access_state = -1;	
	MLOG_NERR1("Comma invalid at %s",pString);
	ret_value = MVE_INVALID_ADL;
	}
    break;	

    case NULL_CHARACTER :
      info->access_state = FINISH_PARSE;
    break;

    default :
      info->access_state = -1;
      MLOG_NERR1("Unexpected %s",pString);
      ret_value = MVE_INVALID_ADL;
    break;
    }
  *ppString = pString;
  return (ret_value);
  }

/************************************************************************/
/*				process_comma				*/
/* If the comma is encountered then we expect structure access		*/
/************************************************************************/

static ST_RET process_comma (ST_CHAR **ppString,
			      ALT_ACCESS *alt_acc,
			      ADL_INFO *info)
  {
  info->named_comp_allowed = SD_TRUE;
  info->access_state = STRUCTURE_ACCESS;
  info->left_brace_allowed = SD_TRUE;
/* it is time to copy the name of the alternate access if there is any	*/
  if (info->comp_name_exists)
    copy_component_name (alt_acc,info);
  point_to_next_character (1,ppString);
  if (**ppString == NULL_CHARACTER)
    {
    MLOG_NERR0("Comma unexpected at the end");
    return (MVE_INVALID_ADL);
    }
  return (SD_SUCCESS);
  }

/************************************************************************/
/*				process_colon				*/		
/* Colon means array index is nested with structure elements so we	*/
/* increment the nesting level of the brace before colon by one		*/
/************************************************************************/

static ST_RET process_colon (ST_CHAR **ppString,ADL_INFO *info)
  {
ST_RET brace_number;

  if (AA_STACK_EMPTY (info))
    {
    MLOG_NERR1("Syntax Error due to colon at %s",*ppString);
    return (MVE_INVALID_ADL);
    }
  brace_number = info->brace_number - 1;
  info -> nest_count[brace_number] ++;
  info->access_state = STRUCTURE_ACCESS;
  point_to_next_character (1,ppString);	
  return (SD_SUCCESS);
  }

/************************************************************************/
/*			end_of_parsing					*/
/* At the end of parsing it checks if the name of the alternate access	*/
/* needs to be copied into the structure				*/
/************************************************************************/

static ST_RET end_of_parsing (ALT_ACCESS *alt_acc,ADL_INFO *info)
  {
ALT_ACC_EL *aa;

  aa = alt_acc->aa+alt_acc->num_aa;
  if ((info->comp_name_exists) && (info->named_comp_allowed))
    copy_component_name (alt_acc,info);
  return (SD_SUCCESS);
  }

/************************************************************************/
/*			get_named_component				*/
/* This function extracts "name1" from the syntax <name1> and points	*/
/* to first non white space character after ">"				*/
/************************************************************************/

static ST_RET get_named_component (ST_CHAR **ppString,ST_CHAR *token)
  {
ST_INT token_length;
ST_CHAR *pString;

  pString = *ppString;
  point_to_next_character (1,&pString);  				
  if (!(token_length = token_copy (token,pString,MAX_IDENT_LEN)))
    {
    MLOG_NERR1("Expected name for the Alternate Access at %s",pString);
    return (MVE_INVALID_ADL);
    }
  if (is_iso_identifier (token) == SD_FALSE)
    {
    MLOG_NERR1("not a valid ISO identifier %s",token);
    return (MVE_INVALID_ADL);
    }
  point_to_next_character (token_length,&pString); 	
  if (*pString != R_ANGLE_BRACKET)/* expect R_ANGLE_BRACKET	*/
    {
    MLOG_NERR1("Right Angle Bracket expected at %s",pString);
    return (MVE_INVALID_ADL);
    }
  point_to_next_character (1,&pString);  /* remove white spaces between R_ANGLE_BRACKET*/
  *ppString = pString;
  return (SD_SUCCESS);
  }

/************************************************************************/
/*			get_index_identifier				*/
/* This function is called by the function get_array_access		*/
/* Index Identifier could be one of these three types		 	*/
/* <Index> 								*/
/* <Index Range> ::= <Low Index><Comma><Num of Elements>		*/
/* <All Elements> ::= "*"						*/
/* where <Index>, <Low Index> and <Num of Elements> are decimal numbers	*/  
/************************************************************************/

static ST_RET get_index_identifier (ST_CHAR **ppString,ALT_ACCESS *alt_acc)
  {
ST_CHAR *pString;
ST_INT token_length;
ST_CHAR token[MAX_IDENT_LEN+1];
ALT_ACC_EL *aa;

  aa = alt_acc->aa+alt_acc->num_aa;
  pString = *ppString;
  if (!(token_length = token_copy (token,pString,MAX_IDENT_LEN)))
    {
    MLOG_NERR1("Expected an index after [ at %s",pString);
    return (MVE_INVALID_ADL);
    }
  point_to_next_character (token_length,&pString);
  switch (*pString)
    {
    case L_BRACE :
    case R_BRACE :
    case COLON	 :
      if ((*token == ASTERISK) && (token_length == 1))
        {
        aa->sel_type = get_sel_type (&pString,AA_ALL);
        }
      else 
        {
        if (isnumber (token) == SD_FALSE)
	  {
          MLOG_NERR1("Expected a number or '*' at %s",token);
          return (MVE_INVALID_ADL);
	  }
	aa->u.index = atol (token);
        aa->sel_type = get_sel_type (&pString,AA_INDEX);
        }
    break;

    case COMMA :
      if (isnumber (token) == SD_FALSE)
	{
        MLOG_NERR1("Expected a number at %s",token);
        return (MVE_INVALID_ADL);
	}
      aa->u.ir.low_index = atol (token);
      point_to_next_character (1,&pString);
      if (!(token_length = token_copy (token,pString,MAX_IDENT_LEN+1)))
        {
        MLOG_NERR1("Expected an index at %s",pString);
        return (MVE_INVALID_ADL);
        }
      point_to_next_character (token_length,&pString);
      if ((*pString != L_BRACE) && (*pString != R_BRACE)
				   && (*pString != COLON))
	{
        MLOG_NERR2("Unexpected character %c at %s", *pString,pString);
        MLOG_NERR0("Expected characters  []:");
        return (MVE_INVALID_ADL);
	}
      if (isnumber (token) == SD_FALSE) 
	{
        MLOG_NERR1("Expected a number at %s",token);
        return (MVE_INVALID_ADL);
	}
      aa->u.ir.num_elmnts = atol (token);
      aa->sel_type = get_sel_type (&pString,AA_INDEX_RANGE);
    break;

    default : 
      MLOG_NERR2("Unexpected character %c at %s ",*pString,pString);
      MLOG_NERR0("Expected characters :  [],:");
      return (MVE_INVALID_ADL);
    break;
    }
  *ppString = pString;
  return (SD_SUCCESS);
  }

/************************************************************************/
/*				get_sel_type				*/
/* This function determines sel_type for Alternate Access if it nested	*/
/* or not depending on the next separator. If type is nested it		*/
/* increments a static variable nesting_count which is used to fill the	*/
/* structure with sel_type of AA_END_NEST at the end of parsing. If type*/
/* is not nested, it returns the passed type. This function does not 	*/
/* alter the passed string						*/
/************************************************************************/

static ST_RET get_sel_type (ST_CHAR **ppString, ST_INT type)
  {
ST_CHAR *next_separator;

  next_separator = strpbrk (*ppString,ch_seps);
  if (!next_separator)
    return (type);

  switch (type)
    {
    case AA_COMP :
      if (*next_separator == L_BRACE)
        type = AA_COMP_NEST;
    break;

    case AA_INDEX :
      if ((*next_separator == L_BRACE) || (*next_separator == COLON))
        type = AA_INDEX_NEST;	
    break;

    case AA_INDEX_RANGE :
      if ((*next_separator == L_BRACE) || (*next_separator == COLON))
        type = AA_INDEX_RANGE_NEST;
    break;

    case AA_ALL :
      if ((*next_separator == L_BRACE) || (*next_separator == COLON))
        type = AA_ALL_NEST;
    break;
    }
  return (type);
  }

/************************************************************************/
/*			copy_component_name				*/
/* This function copies the name of the alternate access to the 	*/
/* structure at the right place						*/
/************************************************************************/

static ST_VOID copy_component_name (ALT_ACCESS *alt_acc,ADL_INFO *info)
  {
ALT_ACC_EL *aa;
  aa = alt_acc -> aa + info->named_index;
  strcpy (aa->comp_name,info->comp_name);
  aa->comp_name_pres = SD_TRUE;
  info->comp_name_exists = SD_FALSE;
  }

/************************************************************************/
/*				isnumber				*/
/* This function checks if the passed string contains only digits	*/
/************************************************************************/

static ST_RET isnumber (ST_CHAR *pString)
  {
  while (*pString)
    {
    if (isdigit (*pString++)==0)
      return (SD_FALSE);
    }
  return (SD_TRUE);
  }

/************************************************************************/
/*			is_iso_identifier				*/
/* This function checks the health of ISO identifier. ISO identifier	*/
/* can not start with a number and it can have alphanumeric characters	*/
/* underscore '_' and dollar sign '$'					*/
/************************************************************************/

static ST_RET is_iso_identifier (ST_CHAR *pString)
  {
  if (isdigit (*pString))	 /* the first letter may not be a digit  */
    return (SD_FALSE);
  while (*pString)
    {
    if ((!(isalnum (*pString))) &&
	 (!(*pString == '_')) &&
	 (!(*pString == '$')))
      return (SD_FALSE);
    pString++;
    }
  return (SD_TRUE);
  }  
	
/************************************************************************/
/*			point_to_next_character				*/
/* This function wacks all the white spaces and pointer will  be	*/
/* pointing to the next character in the string or null character if 	*/
/* there are no more characters in the string				*/
/************************************************************************/

static ST_VOID point_to_next_character (ST_INT length,ST_CHAR **ppString)
  {
  *ppString += length;
  while (isspace (**ppString))
   (*ppString)++;
  }

/************************************************************************/
/*			token_copy					*/
/* This funtion gets the token from the string and returns the strlen	*/
/************************************************************************/

static ST_RET token_copy (ST_CHAR *token,ST_CHAR *pString,ST_INT max_token_length)
  {
ST_INT counter;

  counter = 0;
  while ((strchr (seps,*pString)==NULL) && (counter < max_token_length))
    {
    *token++ = *pString++;
    counter++;
    }
  *token = '\0';
  return (counter);
  }
  
/************************************************************************/
/*			get_alt_acc					*/
/* This function is called by ms_adl_to_string				*/
/************************************************************************/
static ST_RET get_alt_acc (ALT_ACCESS *alt_acc)
  {
ALT_ACC_EL *aa;
ST_INT el_count;
ST_INT error;
ST_INT index_count;	     /*keep track of nesting */
ST_INT comp_nest_count[MAX_NESTING_LEVEL];
ST_INT x;

  error = SD_FALSE;
  x = -1;
  index_count = 0;
  aa = alt_acc->aa;
  for (el_count = 0; el_count < alt_acc->num_aa; ++el_count, ++aa)
    {
    switch (aa->sel_type)
      {
      case AA_COMP_NEST :
	  index_count++;
	  comp_nest_count[++x] = index_count;
      case AA_COMP :
      break;

      case AA_INDEX_NEST :
        index_count++;
      case AA_INDEX :
      break;

      case AA_INDEX_RANGE_NEST :
        index_count++;
      case AA_INDEX_RANGE :
      break;

      case AA_ALL_NEST :
        index_count++;
      case AA_ALL :
      break;
      case AA_END_NEST :
	if (x >= 0 && index_count == comp_nest_count[x])
	  comp_nest_count[x--] = 0;
        index_count--;
      break;

      default : 			/* should not be any other tag	*/
	MLOG_NERR0("Bad sel_type");
        error = SD_TRUE; 
      break;
      }
    if (error)
      break;
    }

  if (!error && el_count && !index_count)
    {
    alt_acc->num_aa = el_count;
    return (el_count);
    }
  return (0);
  }

/************************************************************************/
/*                         traverse_array                               */
/*  Interprets each element of AA structure and puts correct ADL syntax */
/*  into an array.  This function is called through aa_to_adl.		*/ 
/************************************************************************/
static ST_RET traverse_array (ALT_ACCESS *alt_acc, 
			    ST_INT count, 
			    ST_CHAR *adl,
			    ST_INT max_adl_len)
  {
ADL_INFO info;
ALT_ACC_EL *aa;
ST_INT loop_count;
ST_INT temp_count;
ST_INT prev_sel_type;   /* Initialize to meaningless value	*/
ST_INT lbrace_count;
ST_INT rbrace_count;
ST_INT x;
ST_INT y;
ST_INT len, len2;
ST_INT i;
ST_CHAR ch[MAX_IDENT_LEN];		    /* Max length for unsigned long integer */
ST_CHAR *pCount;
ST_CHAR *temp_ptr;

  prev_sel_type = 99;   /* Initialize to meaningless value	*/
  x = -1;
  y = 0;
  i = 0;

  pCount = adl;
  temp_ptr = adl;
  memset (&info, 0, sizeof (ADL_INFO));
  aa = alt_acc->aa;
  info.nest_count[0] = 0;
  info.comp_count[0] = 0;
  
  switch (aa->sel_type)
    {
    case AA_INDEX :
    case AA_INDEX_RANGE	:
    case AA_ALL	:
    case AA_ALL_NEST :
    case AA_INDEX_NEST :
    case AA_INDEX_RANGE_NEST :
      info.access_state = ARRAY_ACCESS;
      info.started_with_structure_access = SD_FALSE;
      if (separator_copy (adl, &i, L_BRACE, max_adl_len) != SD_SUCCESS)
        return (MVE_INVALID_ADL);
      break;
    default:  
      info.access_state = STRUCTURE_ACCESS;
      info.started_with_structure_access = SD_TRUE;
      break;
    }

  for (loop_count = 0; loop_count < count; loop_count++, aa++)
    {
    switch (aa->sel_type)
      {
      case AA_COMP:
	if ((prev_sel_type == AA_COMP) && (info.nest_count[0]))
          {
	  i--;
          if (separator_copy (adl, &i, COMMA, max_adl_len) != SD_SUCCESS)
            return (MVE_INVALID_ADL);
	  }
        else if ((prev_sel_type == AA_END_NEST) || (prev_sel_type == AA_COMP))
	  {
          if (separator_copy (adl, &i, COMMA, max_adl_len) != SD_SUCCESS)
            return (MVE_INVALID_ADL);
	  }
	if ((prev_sel_type == AA_INDEX_RANGE_NEST) || (prev_sel_type == AA_INDEX_NEST))
	  {
	  i--;
          if (separator_copy (adl, &i, L_BRACE, max_adl_len) != SD_SUCCESS)
            return (MVE_INVALID_ADL);
	  }
	else if ((prev_sel_type == AA_INDEX) || (prev_sel_type == AA_INDEX_RANGE)
                || (prev_sel_type == AA_ALL_NEST))
	  {
          if (separator_copy (adl, &i, L_BRACE, max_adl_len) != SD_SUCCESS)
            return (MVE_INVALID_ADL);
	  }
	if (prev_sel_type == AA_COMP_NEST)
	  {
          if (separator_copy (adl, &i, L_BRACE, max_adl_len) != SD_SUCCESS)
            return (MVE_INVALID_ADL);
	  }
	if ((aa->comp_name_pres) && (!info.comp_count[0]))
	  {
          if (separator_copy (adl, &i, L_ANGLE_BRACKET, max_adl_len) != SD_SUCCESS)
            return (MVE_INVALID_ADL);
          if (comp_copy (adl, &i, aa->comp_name, max_adl_len) != SD_SUCCESS)
	    return (MVE_INVALID_ADL);
          if (separator_copy (adl, &i, R_ANGLE_BRACKET, max_adl_len) != SD_SUCCESS)
            return (MVE_INVALID_ADL);
	  }
        if (comp_copy (adl, &i, aa->u.component, max_adl_len) != SD_SUCCESS)
	   return (MVE_INVALID_ADL);
	len = (ST_INT) strlen (aa->u.component);
        len2 = (ST_INT) strlen (aa->comp_name);
	if (adl[i - len - 1] == L_BRACE || prev_sel_type == AA_COMP_NEST ||
	   (aa->comp_name_pres && adl[i - len - len2 - 3] == L_BRACE))
	  if (separator_copy (adl, &i, R_BRACE, max_adl_len) != SD_SUCCESS)
            return (MVE_INVALID_ADL);

        info.comp_name_exists = SD_TRUE;
	prev_sel_type = AA_COMP;
        break;

      case AA_COMP_NEST:
        if (prev_sel_type == AA_END_NEST)
	  {
          if (separator_copy (adl, &i, COMMA, max_adl_len) != SD_SUCCESS)
            return (MVE_INVALID_ADL);
	  }

	if ((prev_sel_type == AA_INDEX_NEST) && adl[i - 1] == R_BRACE)
	  {
	  i--;
          if (separator_copy (adl, &i, L_BRACE, max_adl_len) != SD_SUCCESS)
            return (MVE_INVALID_ADL);
	  }
        else if ((prev_sel_type == AA_INDEX_NEST) || (prev_sel_type == AA_INDEX_RANGE_NEST)
                      || (prev_sel_type == AA_ALL_NEST))
	  {
          if (separator_copy (adl, &i, L_BRACE, max_adl_len) != SD_SUCCESS)
            return (MVE_INVALID_ADL);
	  }

	if (prev_sel_type == AA_COMP)
	  {
          if (separator_copy (adl, &i, COMMA, max_adl_len) != SD_SUCCESS)
            return (MVE_INVALID_ADL);
	  }

	if (prev_sel_type == AA_COMP_NEST)
	  {
          if (separator_copy (adl, &i, L_BRACE, max_adl_len) != SD_SUCCESS)
            return (MVE_INVALID_ADL);
	  }

	if ((aa->comp_name_pres) && (!info.comp_count[0]))
	  {
          if (separator_copy (adl, &i, L_ANGLE_BRACKET, max_adl_len) != SD_SUCCESS)
            return (MVE_INVALID_ADL);
          if (comp_copy (adl, &i, aa->comp_name, max_adl_len) != SD_SUCCESS)
	   return (MVE_INVALID_ADL);
          if (separator_copy (adl, &i, R_ANGLE_BRACKET, max_adl_len) != SD_SUCCESS)
            return (MVE_INVALID_ADL);
	  }
        if (comp_copy (adl, &i, aa->u.component, max_adl_len) != SD_SUCCESS)
	   return (MVE_INVALID_ADL);
        info.comp_name_exists = SD_TRUE;
	prev_sel_type = AA_COMP_NEST;
        info.nest_count[y] = AA_COMP_NEST;
	info.comp_count[++x] = info.nest_count[y++];
        break;


      case AA_INDEX_NEST:
        info.nest_count[y++] = AA_INDEX_NEST;
        if (prev_sel_type == AA_END_NEST)
	  {
          if (separator_copy (adl, &i, COMMA, max_adl_len) != SD_SUCCESS)
            return (MVE_INVALID_ADL);
	  }

	if ((prev_sel_type == AA_INDEX_RANGE_NEST) || (prev_sel_type == AA_COMP_NEST) ||
	                  (prev_sel_type == AA_ALL_NEST))
	  {
          if (separator_copy (adl, &i, L_BRACE, max_adl_len) != SD_SUCCESS)
            return (MVE_INVALID_ADL);
	  }

	if (prev_sel_type == AA_INDEX_NEST)
          {
	  i--;
          if (separator_copy (adl, &i, L_BRACE, max_adl_len) != SD_SUCCESS)
            return (MVE_INVALID_ADL);
	  }
	sprintf (ch, "%lu", (ST_ULONG) aa->u.index);
	if (strlen (ch) > MAX_IDENT_LEN)
	  return (MVE_INVALID_ADL);
        if (comp_copy (adl, &i, ch, max_adl_len) != SD_SUCCESS)
          return (MVE_INVALID_ADL);
        if (separator_copy (adl, &i, R_BRACE, max_adl_len) != SD_SUCCESS)
          return (MVE_INVALID_ADL);
	prev_sel_type = AA_INDEX_NEST;
        break;

      case AA_INDEX:
        if (prev_sel_type == AA_END_NEST)
	  {
          if (separator_copy (adl, &i, COMMA, max_adl_len) != SD_SUCCESS)
            return (MVE_INVALID_ADL);
	  }

	if (prev_sel_type == AA_INDEX_RANGE_NEST)
	  {
	  --i;
          if (separator_copy (adl, &i, L_BRACE, max_adl_len) != SD_SUCCESS)
            return (MVE_INVALID_ADL);
	  }
	if ((prev_sel_type == AA_COMP_NEST) ||(prev_sel_type == AA_ALL_NEST))
	  {
          if (separator_copy (adl, &i, L_BRACE, max_adl_len) != SD_SUCCESS)
            return (MVE_INVALID_ADL);
	  }

	if (prev_sel_type == AA_INDEX_NEST)
          {
	  i--;
          if (separator_copy (adl, &i, L_BRACE, max_adl_len) != SD_SUCCESS)
            return (MVE_INVALID_ADL);
	  }
	sprintf (ch, "%lu", (ST_ULONG) aa->u.index);
	if (strlen (ch) > MAX_IDENT_LEN)
	  return (MVE_INVALID_ADL);
        if (comp_copy (adl, &i, ch, max_adl_len) != SD_SUCCESS)
          return (MVE_INVALID_ADL);
        if (separator_copy (adl, &i, R_BRACE, max_adl_len) != SD_SUCCESS)
          return (MVE_INVALID_ADL);
	prev_sel_type = AA_INDEX_NEST;
        break;

      case AA_INDEX_RANGE_NEST:
        info.nest_count[y++] = AA_INDEX_RANGE_NEST;
      case AA_INDEX_RANGE:
	if (prev_sel_type == AA_INDEX_NEST)
          {
	  i--;
          if (separator_copy (adl, &i, L_BRACE, max_adl_len) != SD_SUCCESS)
            return (MVE_INVALID_ADL);
	  }
        if (prev_sel_type == AA_END_NEST)
	  {
          if (separator_copy (adl, &i, COMMA, max_adl_len) != SD_SUCCESS)
            return (MVE_INVALID_ADL);
	  }
        if (prev_sel_type == AA_COMP_NEST) 
	  {
          if (separator_copy (adl, &i, L_BRACE, max_adl_len) != SD_SUCCESS)
            return (MVE_INVALID_ADL);
	  }
	if (prev_sel_type == AA_INDEX_RANGE_NEST)
	  {
	  i--;
          if (separator_copy (adl, &i, L_BRACE, max_adl_len) != SD_SUCCESS)
            return (MVE_INVALID_ADL);
	  }
	sprintf (ch, "%lu", (ST_ULONG) aa->u.ir.low_index);
	if (strlen (ch) > MAX_IDENT_LEN)
	  return (MVE_INVALID_ADL);
        if (comp_copy (adl, &i, ch, max_adl_len) != SD_SUCCESS)
          return (MVE_INVALID_ADL);
        if (separator_copy (adl, &i, COMMA, max_adl_len) != SD_SUCCESS)
          return (MVE_INVALID_ADL);
	sprintf (ch, "%lu", (ST_ULONG) aa->u.ir.num_elmnts);
	if (strlen (ch) > MAX_IDENT_LEN)
	  return (MVE_INVALID_ADL);
        if (comp_copy (adl, &i, ch, max_adl_len) != SD_SUCCESS)
          return (MVE_INVALID_ADL);
        if (separator_copy (adl, &i, R_BRACE, max_adl_len) != SD_SUCCESS)
          return (MVE_INVALID_ADL);
	prev_sel_type = AA_INDEX_RANGE_NEST;
        break;	

      case AA_ALL_NEST:
        info.nest_count[y++] = AA_ALL_NEST;
      case AA_ALL:
        if (prev_sel_type == AA_END_NEST)
	  {
          if (separator_copy (adl, &i, COMMA, max_adl_len) != SD_SUCCESS)
            return (MVE_INVALID_ADL);
	  }
        if ((prev_sel_type == AA_COMP_NEST) || (prev_sel_type == AA_ALL_NEST))
	  {
          if (separator_copy (adl, &i, L_BRACE, max_adl_len) != SD_SUCCESS)
            return (MVE_INVALID_ADL);
	  }

        if (separator_copy (adl, &i, ASTERISK, max_adl_len) != SD_SUCCESS)
          return (MVE_INVALID_ADL);
	prev_sel_type = AA_ALL_NEST;
        break;

      case AA_END_NEST:
	if ((prev_sel_type != AA_END_NEST) && (adl[i - 1] == R_BRACE))
	  {
	  i--;
	  adl[i] = 0;
	  }
	y--;
	if (x >= 0 && y >= 0 && info.nest_count[y] == info.comp_count[x])
	  info.comp_count[x--] = 0;

        lbrace_count = rbrace_count = 0;

        if (separator_copy (adl, &i, R_BRACE, max_adl_len) != SD_SUCCESS)
          return (MVE_INVALID_ADL);

	if (y == 0)
	  {
	  for (temp_count = 0; (temp_count <= (&adl[i] - temp_ptr)) && 
	                       (*pCount != 0); temp_count++, pCount++)
            { 
            if (*pCount == L_BRACE)
              lbrace_count++;
            if (*pCount == R_BRACE)
              rbrace_count++;
            }
	  }

	adl[i] = 0;
        prev_sel_type = AA_END_NEST;
        break;
      default:
        MLOG_NERR1("Invalid sel_type : %d", aa->sel_type);
        return (MVE_INVALID_ADL);
	break;
      }
    }

  pCount = adl;              
  lbrace_count = rbrace_count = 0;
 
  for (temp_count = 0; (temp_count <= i) && (*pCount != 0); temp_count++, pCount++)
    { 
    if (*pCount == L_BRACE)
      lbrace_count++;
    if (*pCount == R_BRACE)
      rbrace_count++;
    }
  rbrace_count = lbrace_count - rbrace_count;
  for (temp_count = 0; temp_count < rbrace_count; temp_count++)
    {
    *pCount = R_BRACE;
    pCount++;
    }
  *pCount = 0;  
  return (SD_SUCCESS);
  }

/************************************************************************/
/* 			separator_copy					*/
/* This function takes start of string pointer, current string pointer	*/
/* the string we want to copy and max len allowed, Before we copy str	*/
/* into adl[*i] we validate for the overrun				*/
/************************************************************************/
static ST_RET separator_copy (ST_CHAR *adl,ST_INT *i,ST_CHAR sep, ST_INT max_adl_len)
  { 
  if (*i + 1 >= max_adl_len)
    return (SD_FAILURE);
  adl[*i] = sep;
  *i += 1;
  adl[*i] = 0;	/* Insure end of string is recognized */
  return (SD_SUCCESS);
  }
/************************************************************************/

/************************************************************************/
static ST_RET comp_copy (ST_CHAR *adl,ST_INT *i,ST_CHAR *comp, ST_INT max_adl_len)
  { 
ST_INT comp_len;

  comp_len = (ST_INT) strlen (comp);
  if (*i + comp_len + 1 >= max_adl_len)
    return (SD_FAILURE);
  strcpy (&adl[*i], comp);
  *i += comp_len;
  return (SD_SUCCESS);
  }

/************************************************************************/
/*			ms_adl_to_asn1_2				*/
/* Function to parse ADL (Alternate Access Definition Language) and	*/
/* generate the ASN.1 encoding of the Alternate Access Definition.	*/
/* NOTE: Suffix (_2) is just to avoid naming conflict with older funct.	*/
/* NOTE: this function is thread-safe as long as the encode buffer	*/
/*   passed as arg (buf_ptr) is not being used by another thread.	*/
/************************************************************************/
ST_RET ms_adl_to_asn1_2 (ST_CHAR *pAdl,	/* ADL string to be encoded	*/
	ST_UCHAR *buf_ptr,		/* buffer in which to encode	*/
	ST_INT buf_len,			/* buffer length		*/
	ST_UCHAR **asn1_ptr_out,	/* ptr to ptr to encoded data	*/
	ST_INT *asn1_len_out)		/* ptr to encoded length	*/
  {
ALT_ACCESS stAltAcc;
ST_RET retcode;

  /* First, convert from ADL string to ALT_ACCESS structure.		*/
  /* NOTE: if successful, this allocates "stAltAcc.aa". Be sure to free it.*/
  retcode = ms_adl_to_aa(pAdl, &stAltAcc); 	/* get ALT_ACCESS 	*/
  if (retcode == SD_SUCCESS)
    {
    /* Next, convert from ALT_ACCESS structure to ASN.1 encoding.	*/
    retcode = ms_aa_to_asn1_2 (&stAltAcc, buf_ptr, buf_len,
              asn1_ptr_out, asn1_len_out);
    chk_free (stAltAcc.aa);	/* free array allocated by ms_adl_to_aa	*/
    }
  return (retcode);
  }
