/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*   (c) Copyright Systems Integration Specialists Company, Inc.,	*/
/*      	1994 - 2002, All Rights Reserved		        */
/*									*/
/* MODULE NAME : mvl_var.c    						*/
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
/* 11/23/11  KCR           Added casting for proper 64bit compile       */
/* 10/30/06  JRB     31    Use new mvl_vmd_* object handling functions.	*/
/*			   _mvl_objname_to_va: add args.		*/
/*			   u_mvl_get_va_aa: add args.			*/
/*			   Elim use of global var "_mvl_curr_net_info".	*/
/* 01/30/06  GLB     30    Integrated porting changes for VMS           */
/* 02/27/04  MDE     29    Log message correct				*/
/* 03/13/03  JRB     28    Add mvl_scope_set function.			*/
/* 12/12/02  JRB     27    Add net_info arg to mvl_get_va_asn1... and	*/
/*			   elim it's use of global _mvl_curr_net...	*/
/* 02/25/02  MDE     26    Now get max PDU size from mvl_cfg_info	*/
/* 01/02/02  JRB     25    Converted to use ASN1R (re-entrant ASN1)	*/
/* 10/25/00  JRB     24    Del u_mvl funct ptrs. Call functs directly.	*/
/*			   Control with #ifdefs.			*/
/* 08/17/00  JRB     23    Don't clear va_to_free. Need value later.	*/
/* 03/08/00  JRB     22    Del #ifdef MVL_REQ_BUF_ENABLED: not used.	*/
/* 01/21/00  MDE     21    Now use MEM_SMEM for dynamic memory		*/
/* 01/21/00  MDE     20    Add '_mvl_get_req_buf' for MVL_REQ_BUF_EN..	*/
/* 11/19/99  NAV     19	   Add #ifdef MVL_REQ_BUF_ENABLED		*/
/* 10/22/99  NAV     18    Add a few utilities to copy structs		*/
/* 09/13/99  MDE     17    Added SD_CONST modifiers			*/
/* 09/07/99  MDE     16    Changed MVL_VA_SCOPE to MVL_SCOPE		*/
/* 01/20/99  JRB     15    Del mvl_find_*. Use new "bsearch" versions.	*/
/*			   Del _mvl_skip_uca_prefix (use va->flags).	*/
/*			   Del _mvl_destroy_nvl_ent.. (see mvl_nvl_de..)*/
/* 12/11/98  MDE     14    Removed scope references from VA		*/
/* 12/08/98  MDE     13    Added client alternate access support	*/
/* 11/16/98  MDE     12    Added '_mvl_destroy_nvl_entries'		*/
/* 11/16/98  MDE     11    Renamed internal functions (prefix '_')	*/
/* 11/10/98  MDE     10    Added 'mvl_get_va_asn1_data' (was in s_read)	*/
/* 09/21/98  MDE     09    Minor lint cleanup				*/
/* 07/21/98  MDE     08    Moved 'mvl_insert_nvlist' to mvl_dnvl.c	*/
/* 07/13/98  MDE     07    Fixed mvl_insert_nvlist bug when adding 1st	*/
/* 06/30/98  MDE     06    Corrected insert order for "_UCA_" names	*/
/* 06/15/98  MDE     05    Changes to allow compile under C++		*/
/* 06/05/98  MDE     04    Added 'mvl_insert_nvlist'			*/
/* 01/06/98  RWM     03    Added Binary Search Capability               */
/* 12/12/97  MDE     02    Moved mvl_vmd to rt_types.c			*/
/* 09/10/97  MDE     01    MMS-LITE V4.0 Release			*/
/************************************************************************/

#include "glbtypes.h"
#include "sysincs.h"

#include "mmsdefs.h"
#include "mms_pvar.h"
#include "mms_vvar.h"
#include "mvl_acse.h"
#include "mvl_log.h"


/************************************************************************/
/* For debug version, use a static pointer to avoid duplication of 	*/
/* __FILE__ strings.							*/
/************************************************************************/

#ifdef DEBUG_SISCO
SD_CONST static ST_CHAR *SD_CONST thisFileName = __FILE__;
#endif

/************************************************************************/
/************************************************************************/
/*			_mvl_objname_to_va				*/
/************************************************************************/

MVL_VAR_ASSOC *_mvl_objname_to_va (MVL_VMD_CTRL *vmd_ctrl,
	MVL_NET_INFO *net_info,
	ST_INT service,
	OBJECT_NAME *obj, 
	MVL_SCOPE *va_scope_out,
	ST_BOOLEAN alt_access_pres,
	ALT_ACCESS *alt_acc,
	ST_BOOLEAN *alt_access_done_out)
  {    
MVL_VAR_ASSOC *va;

/* By default, we don't modify the VA to handle alternate access 	*/
  if (alt_access_done_out != NULL)
    *alt_access_done_out = SD_FALSE;

/* Let's find the variable association */
  va = mvl_vmd_find_var (vmd_ctrl, obj, net_info);
  if ((!va) || (va->flags & MVL_VAR_FLAG_UCA))
    {
  /* Could not find variable association, see if we can manufacture it	*/
#if defined(MVL_UCA) || defined(USE_MANUFACTURED_OBJS)
    /* May be able to manufacture it	*/
    va = u_mvl_get_va_aa (vmd_ctrl, service, obj, net_info, 
    				 alt_access_pres, alt_acc, 
    				 alt_access_done_out);
    if (va != NULL)		/* Manufactured it, set flag ...	*/
      va->va_to_free = va;
#endif	/* defined(MVL_UCA) || defined(USE_MANUFACTURED_OBJS)	*/
    }

/* OK, assuming we have the VA, set it's scope appropriately		*/
  if (va && va_scope_out)
    {
    va_scope_out->scope = obj->object_tag;
    if (va_scope_out->scope == DOM_SPEC)
      {
      va_scope_out->dom = mvl_vmd_find_dom (vmd_ctrl, obj->domain_id);
      if (va_scope_out->dom == NULL)
        {
        MVL_LOG_NERR1 ("Warning : Variable object references non-existant domain '%s'", 
						obj->domain_id);
        }
      }
    }
  return (va);
  }

/************************************************************************/
/*			mvl_scope_set					*/
/* Use the OBJECT_NAME info to set the scope for any MMS object.	*/
/* This function fills in the struct pointed to by mvl_scope_out.	*/
/************************************************************************/
ST_VOID mvl_scope_set (OBJECT_NAME *obj, MVL_SCOPE *mvl_scope_out)
  {
  mvl_scope_out->scope = obj->object_tag;
  if (mvl_scope_out->scope == DOM_SPEC)
    {
    mvl_scope_out->dom = mvl_vmd_find_dom (&mvl_vmd, obj->domain_id);
    if (mvl_scope_out->dom == NULL)
      MVL_LOG_NERR1 ("Warning : MMS object references nonexistent domain '%s'", 
					obj->domain_id);
    }
  }

/************************************************************************/
/*			mvl_get_va_asn1_data				*/
/************************************************************************/

ST_RET mvl_get_va_asn1_data (MVL_NET_INFO *net_info, MVL_VAR_ASSOC *va,
			 ST_BOOLEAN alt_access_pres,
			 ALT_ACCESS *alt_acc,
                         ST_UCHAR *asn1_dest_buffer,
                         ST_INT asn1_buffer_len,
                         ST_INT *asn1_len_out)
  {
RUNTIME_TYPE *rt;
ST_INT num_rt;
ST_RET rc;
ST_UCHAR *asn1_start;
ST_INT asn1_len;
ST_INT aa_mode;
ASN1_ENC_CTXT localEncCtx;	/* For readability, use "aCtx" to access this.*/
ASN1_ENC_CTXT *aCtx = &localEncCtx;

  aa_mode = MVL_USE_UNPACKED_AA;
  if (va->proc && va->proc->pre_read_aa)
    {
    rc = (*va->proc->pre_read_aa)(&va, alt_access_pres, alt_acc, 
  			    net_info, &aa_mode);

    if (rc != SD_SUCCESS)
      {
      MVL_LOG_NERR1 ("Read: pre_read_aa returned %d", rc);
      return (SD_FAILURE);
      }
    }

  rc = mvl_get_runtime (va->type_id, &rt, &num_rt);
  if (rc != SD_SUCCESS)
    {
    MVL_LOG_NERR1 ("Read: mvl_get_runtime failed, rc = %x", rc);
    return (rc);			     /* bad type id		*/
    }

/* If we got here, we have a valid runtime and have resolved local	*/
/* data pointer and size. Go ahead and build the ASN.1 data element.	*/

/* First we need to initialize the ASN.1 encode tools, then do the 	*/
/* encode							   	*/
  asn1r_strt_asn1_bld (aCtx, asn1_dest_buffer, asn1_buffer_len);

#if defined(MVL_AA_SUPP)
  if (alt_access_pres == SD_FALSE || aa_mode == MVL_USE_NO_AA)
    {
    rc = ms_local_to_asn1 (aCtx, rt, num_rt, (ST_CHAR *) va->data);
    }
  else
    {
    if (aa_mode == MVL_USE_PACKED_AA)
      m_alt_acc_packed = SD_TRUE;
    else
      m_alt_acc_packed = SD_FALSE;

    rc = ms_local_to_asn1_aa (aCtx, rt, num_rt, alt_acc, (ST_CHAR *) va->data);
    }
#else
  rc = ms_local_to_asn1 (aCtx, rt, num_rt, (ST_CHAR *) va->data);
#endif

/* When the ASN.1 encode is complete, 'asn1_field_ptr' points 1 byte ahead 	*/
/* of the start of the message. The ASN.1 message is build from back to	*/
/* front, so that is now we calculate the length of the PDU		*/
  if (rc == SD_SUCCESS)
    {
    asn1_start = aCtx->asn1r_field_ptr+1;
    asn1_len = (int) ((asn1_dest_buffer + asn1_buffer_len) - asn1_start);
    memmove (asn1_dest_buffer,asn1_start,asn1_len);
    *asn1_len_out = asn1_len;
  
    if (va->proc && va->proc->post_read_aa)
      {
      (*va->proc->post_read_aa)(va, alt_access_pres, alt_acc, 
    			             net_info);
      }
    }
  else
    {
    MVL_LOG_NERR1 ("Local to ASN1 data conversion error: 0x%04x", rc);
    }
  return (rc);
  }


/************************************************************************/
/************************************************************************/
#if defined(MVL_AA_SUPP)
/************************************************************************/

/************************************************************************/
/************************************************************************/
/*			_mvl_get_asn1_aa 				*/
/************************************************************************/

ST_RET _mvl_get_asn1_aa (ALT_ACCESS *alt_acc, VARIABLE_LIST *vl)
  {
ST_UCHAR *aa_buf;
ST_INT aa_buf_size;
ST_UCHAR *asn1_start;
ST_INT asn1_len;
ASN1_ENC_CTXT localEncCtx;	/* For readability, use "aCtx" to access this.*/
ASN1_ENC_CTXT *aCtx = &localEncCtx;

  aa_buf = _m_get_aa_asn1_buf (&aa_buf_size);
  asn1r_strt_asn1_bld (aCtx, aa_buf, aa_buf_size);
  if (ms_aa_to_asn1 (aCtx, alt_acc))
    {
    _m_free_aa_asn1_buf ();
    return (SD_FAILURE);
    }
  else
    {
    asn1_start = aCtx->asn1r_field_ptr+1;
    asn1_len = (int) ((aa_buf + aa_buf_size) - asn1_start);
    vl->alt_access.len = asn1_len;
    vl->alt_access.data = asn1_start;
    vl->alt_access_pres = SD_TRUE;
    _m_set_aa_asn1_buf_end (asn1_start -1);
    }
  return (SD_SUCCESS);
  }

/************************************************************************/

static ST_UCHAR *aa_buffer_head;
static ST_UCHAR *aa_buffer_end;

/************************************************************************/
/*			_m_get_aa_asn1_buf 				*/
/************************************************************************/

ST_UCHAR *_m_get_aa_asn1_buf (ST_INT *size_out)
  {
ST_UCHAR *ret;

  if (!aa_buffer_head)
    {
    aa_buffer_head = (ST_UCHAR *) M_MALLOC (MSMEM_AA_ENCODE, mvl_cfg_info->max_msg_size);
    aa_buffer_end = aa_buffer_head + mvl_cfg_info->max_msg_size -1;
    }

  *size_out = (int) (aa_buffer_end - aa_buffer_head + 1);
  ret = aa_buffer_head;
  return (ret);
  }

/************************************************************************/
/*			_m_set_aa_asn1_buf_end 				*/
/************************************************************************/

ST_VOID _m_set_aa_asn1_buf_end (ST_UCHAR *new_end)
  {
  aa_buffer_end = new_end;
  }

/************************************************************************/
/*			_m_free_aa_asn1_buf 				*/
/************************************************************************/

ST_VOID _m_free_aa_asn1_buf (ST_VOID)
  {
  if (aa_buffer_head)
    {
    M_FREE (MSMEM_AA_ENCODE, aa_buffer_head);
    aa_buffer_head = NULL;
    }
  }

#endif /* #if defined(MVL_AA_SUPP) */


/************************************************************************/
/*			mvl_get_descr_local_addr			*/
/* DESCRIBED VARIABLE ASSOCIATION					*/
/* Addresses are in the form :						*/
/*              "baseaddr:xxx"						*/
/* where xxx is the element offset from the base address.		*/
/************************************************************************/

/************************************************************************/
#if defined (MVL_DESCR_SUPP)
/************************************************************************/

ST_RET mvl_get_descr_local_addr (VAR_ACC_ADDR *addr,
                            RUNTIME_TYPE *rt,
                            ST_INT num_rt,
                            ST_CHAR **data_out,
                            ST_INT *data_size_out)
  {
ST_INT i;
MVL_DESCR_ADDR_ASSOC *d;
ST_INT base_name_len;
ST_INT offset;
ST_INT num_offset;
ST_INT num_elmnts;

/* We only support symbolic addresses					*/
  if (addr->addr_tag != SYM_ADDR)
    {
    MVL_LOG_NERR0 ("Get Described Local Address : Only Symbolic Addresses are supported");
    return (MVL_ERR_NOT_SYM_ADDR);
    }

/* Find the matching described variable association			*/
  d = mvl_vmd.descr_addr_assoc_tbl;
  for (i = 0; i < mvl_vmd.num_descr_addr; ++i, ++d)
    {
    base_name_len = strlen (d->base_name);
    if (!strncmp (d->base_name, addr->addr.sym_addr, base_name_len))
      {
      /* Check for offset from base element				*/
      if (base_name_len == strlen (addr->addr.sym_addr))
        offset = 0;
      else
        {
        num_offset = atoi (&addr->addr.sym_addr[base_name_len + 1]);
        if (num_offset)         /* :1 is first element			*/
          num_offset--;
        offset = d->data_size * num_offset;
        }

      if (rt->el_tag != RT_ARR_START) /* if not array			*/
        {
        *data_out = (ST_CHAR *) d->data + offset;
        *data_size_out = d->data_size;
        return (SD_SUCCESS);
        }
      else      /* this is an array					*/
        {
        num_elmnts = rt->u.arr.num_elmnts; /* get number of elements	*/
        if (num_elmnts + num_offset > d->max_elmnts)
	  {
	  MVL_LOG_NERR0 ("Get Described Local Address : Invalid array element count");
	  return (MVL_ERR_ARRAY_ELEMENT_CNT);
	  }

        *data_out = (ST_CHAR *) d->data + offset;
        *data_size_out = d->data_size * num_elmnts;
        return (SD_SUCCESS);
        }
      break;
      }
    }
  MVL_LOG_NERR0 ("Get Described Local Address : Could not get Local Address");
  return (MVL_ERR_LOCAL_ADDRESS);
  }
/************************************************************************/
#endif /* #if defined (MVL_DESCR_SUPP) */
/************************************************************************/

