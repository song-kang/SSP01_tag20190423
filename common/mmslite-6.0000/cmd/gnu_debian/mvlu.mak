##########################################################################
# SISCO SOFTWARE MODULE HEADER ###########################################
##########################################################################
#			       						 #                                
# MODULE NAME : mvlu.mak        	               			 #            
# PRODUCT(S)  : MMS-EASE-Lite						 #            
#			       					         #            
# MODULE DESCRIPTION :        						 #           
#     This makefile is used by the Linux 'make' utility to build the MVLU#    
# libraries for MMS-EASE-Lite.  All paths including header 		 #        
# files and object modules are relative to the current project 		 #   
# directory which makes this file as environment independent as          #
# possible.  								 #
#								         #                                
# To make the MVLU_L.A:							 #
# 	make -f mvlu.mak     	    DFLAG=_l   DEFS=-DDEBUG_SISCO	 #
# To make the MVLU_N.A:							 #
# 	make -f mvlu.mak     	    DFLAG=_n				 #
# To make the MVLU_LD.A:						 #
# 	make -f mvlu.mak     OPT=-g DFLAG=_ld  DEFS=-DDEBUG_SISCO	 #
# To make the MVLU_ND.A:						 #
# 	make -f mvlu.mak     OPT=-g DFLAG=_nd				 #
#									 #
# MODIFICATION LOG :	       						 #
#  Date     Who   Rev			Comments			 #
# --------  ---  ------   -------------------------------------------	 #
# 02/21/12  JRB		  Add mvl61850_log.c, s_jinit2.c
# 03/24/11  JRB		  Add mvlu_scan.c
# 02/03/10  JRB    10     Del INCDIR, LIBDIR (now defined in platform.mak).
# 10/23/06  JRB    09     Add mvl61850_rpt.c
# 08/04/05  EJV    08     Moved platform defs to platform.mak.
#			  Revised link & clean. Added -c in compile rule.
# 07/25/05  JRB    07     Del unused include path ../../inc/mmsop_en.
# 01/19/05  JRB    06     Add mvl61860_ctl.c
# 08/04/04  EJV    05     Add AIX OS support.
# 07/19/04  JRB    04     Add sclproc.c
# 12/10/03  JRB    03     Add LYNX OS support. Del "no-stack-check" option.
# 10/21/03  JRB    02     Chg CFLAGS & LDFLAGS to be "user-settable",
#			   use $(AR), as recommended by GNU Make.
#			  Add "$(OBJDIR)/" prefix before all objects & "%.o",
#			   del "$(OBJDIR)/" prefix before "$@",
#			   then use $(OBJECTS) in $(AR) cmd.
#			  Use ARFLAGS (for QNX = -n -q, default = -r).
# 12/19/02  CRM    01     Created from QNX makefile.			 #
##########################################################################

# Include common and platform-dependent definitions
include ./platform.mak

# set the library, object, and include file path names
DFLAG  =
DEFS   =
OPT    =
SRCDIR = ../../mvl/src
PROJ  = mvlu$(DFLAG)
OBJDIR = obj_$(PROJ)
LIST  = >> cc.lst 2>&1
VPATH = $(SRCDIR) $(OBJDIR) $(LIBDIR) $(INCDIR)

ALL_CFLAGS = $(PLATFORM_CFLAGS) \
	$(OPT) -I$(INCDIR) \
	$(DEFS) -DMMS_LITE -DMVL_UCA \
	$(CFLAGS)

LIBRARY = $(LIBDIR)/$(PROJ).a

# setup the dependencies for the source files
INCLUDES = $(INCDIR)/glbtypes.h \
	$(INCDIR)/sysincs.h \
	$(INCDIR)/mmsdefs.h \
	$(INCDIR)/glberror.h \
	$(INCDIR)/gen_list.h \
	$(INCDIR)/mem_chk.h \
	$(INCDIR)/mms_pvar.h \
	$(INCDIR)/mms_mp.h \
	$(INCDIR)/asn1defs.h \
	$(INCDIR)/slog.h \
	$(INCDIR)/mms_vvar.h \
	$(INCDIR)/mms_err.h \
	$(INCDIR)/mms_def2.h \
	$(INCDIR)/mmsintr2.h \
	$(INCDIR)/mmsldefs.h \
	$(INCDIR)/mms_dfun.h \
	$(INCDIR)/mms_perr.h \
	$(INCDIR)/mms_log.h \
	$(INCDIR)/mmslog.h \
	$(INCDIR)/mms_pcon.h \
	$(INCDIR)/mms_pdom.h \
	$(INCDIR)/mms_pevn.h \
	$(INCDIR)/mms_pfil.h \
	$(INCDIR)/mms_pjou.h \
	$(INCDIR)/mms_pocs.h \
	$(INCDIR)/mms_pprg.h \
	$(INCDIR)/mms_psem.h \
	$(INCDIR)/mms_pvmd.h \
	$(INCDIR)/mmsefun.h \
	$(INCDIR)/mvl_acse.h \
	$(INCDIR)/mvl_defs.h \
	$(INCDIR)/mvl_log.h

# These are the objects that belong in the library
OBJECTS = \
	$(OBJDIR)/c_concl.o \
	$(OBJDIR)/c_defvl.o \
	$(OBJDIR)/c_delvl.o \
	$(OBJDIR)/c_fclose.o \
	$(OBJDIR)/c_fdel.o \
	$(OBJDIR)/c_fdir.o \
	$(OBJDIR)/c_fget.o \
	$(OBJDIR)/c_fopen.o \
	$(OBJDIR)/c_fread.o \
        $(OBJDIR)/c_fren.o \
	$(OBJDIR)/c_getdom.o \
	$(OBJDIR)/c_getnam.o \
	$(OBJDIR)/c_getvar.o \
	$(OBJDIR)/c_getvla.o \
	$(OBJDIR)/c_ident.o \
	$(OBJDIR)/c_info.o \
	$(OBJDIR)/c_jinit.o \
	$(OBJDIR)/c_jread.o \
	$(OBJDIR)/c_jstat.o \
	$(OBJDIR)/c_read.o \
        $(OBJDIR)/c_obtfi.o \
	$(OBJDIR)/c_status.o \
	$(OBJDIR)/c_write.o \
	$(OBJDIR)/mvlu_sbo.o \
	$(OBJDIR)/mvl_buf.o \
	$(OBJDIR)/mvl_dtyp.o \
	$(OBJDIR)/mvl_obj.o \
	$(OBJDIR)/mvl_objb.o \
	$(OBJDIR)/mvl_ijou.o \
	$(OBJDIR)/mvl_send.o \
	$(OBJDIR)/mvl_serv.o \
	$(OBJDIR)/mvl_typ2.o \
	$(OBJDIR)/mvl_type.o \
	$(OBJDIR)/mvl_uca.o \
	$(OBJDIR)/mvl_var.o \
	$(OBJDIR)/mvl61850.o \
	$(OBJDIR)/mvl61850_ctl.o \
	$(OBJDIR)/mvl61850_log.o \
	$(OBJDIR)/mvl61850_rpt.o \
	$(OBJDIR)/mvlu_rpt.o \
	$(OBJDIR)/mvlu_rt.o \
	$(OBJDIR)/mvlu_scan.o \
	$(OBJDIR)/mvluleaf.o \
	$(OBJDIR)/s_cancel.o \
	$(OBJDIR)/s_concl.o \
	$(OBJDIR)/s_defvl.o \
	$(OBJDIR)/s_delvl.o \
	$(OBJDIR)/s_fclose.o \
	$(OBJDIR)/s_fdel.o \
	$(OBJDIR)/s_fren.o \
	$(OBJDIR)/s_fdir.o \
	$(OBJDIR)/s_fopen.o \
	$(OBJDIR)/s_fread.o \
	$(OBJDIR)/s_getcl.o \
	$(OBJDIR)/s_getdom.o \
	$(OBJDIR)/s_getnam.o \
	$(OBJDIR)/s_getvaa.o \
	$(OBJDIR)/s_getvla.o \
	$(OBJDIR)/s_ident.o \
	$(OBJDIR)/s_info.o \
	$(OBJDIR)/s_jinit.o \
	$(OBJDIR)/s_jinit2.o \
	$(OBJDIR)/s_jread.o \
	$(OBJDIR)/s_jstat.o \
        $(OBJDIR)/s_obtfi.o \
	$(OBJDIR)/s_read.o \
	$(OBJDIR)/s_status.o \
	$(OBJDIR)/s_write.o \
	$(OBJDIR)/sclproc.o

# Main target is the library
# If any objects have changed, delete and recreate it.
$(LIBRARY): $(OBJECTS)
	rm -f $(LIBRARY)
	$(AR) $(ARFLAGS) $(LIBRARY) $(OBJECTS) $(LIST)
	echo "FINISHED CREATING $(LIBRARY) LIBRARY" $(LIST)
	echo "-----------------------------------------------------" $(LIST)

clean:
	rm -f $(OBJECTS)
	rm -f $(LIBRARY)
	echo "Finished clean" $(LIST)

# Pattern rule for all object files
# NOTE: the $@ is the complete target name including the "$(OBJDIR)/" prefix
$(OBJDIR)/%.o : %.c
#	Enable next line to log name of file being compiled (may help clarify error messages).
#	echo Compiling: $< $(LIST)
	$(CC) -c $(ALL_CFLAGS) $< -o $@ $(LIST)

# Object file target dependencies. All use the pattern rule above.
$(OBJDIR)/c_concl.o : c_concl.c $(INCLUDES)
$(OBJDIR)/c_defvl.o : c_defvl.c $(INCLUDES)
$(OBJDIR)/c_delvl.o : c_delvl.c $(INCLUDES)
$(OBJDIR)/c_ident.o : c_ident.c $(INCLUDES)
$(OBJDIR)/c_info.o : c_info.c $(INCLUDES)
$(OBJDIR)/c_fclose.o : c_fclose.c $(INCLUDES)
$(OBJDIR)/c_fdel.o : c_fdel.c $(INCLUDES)
$(OBJDIR)/c_fdir.o : c_fdir.c $(INCLUDES)
$(OBJDIR)/c_fget.o : c_fget.c $(INCLUDES)
$(OBJDIR)/c_fopen.o : c_fopen.c $(INCLUDES)
$(OBJDIR)/c_fread.o : c_fread.c $(INCLUDES)
$(OBJDIR)/c_fren.o : c_fren.c $(INCLUDES)
$(OBJDIR)/c_getdom.o : c_getdom.c $(INCLUDES)
$(OBJDIR)/c_getnam.o : c_getnam.c $(INCLUDES)
$(OBJDIR)/c_getvar.o : c_getvar.c $(INCLUDES)
$(OBJDIR)/c_getvla.o : c_getvla.c $(INCLUDES)
$(OBJDIR)/c_jinit.o : c_jinit.c $(INCLUDES)
$(OBJDIR)/c_jread.o : c_jread.c $(INCLUDES)
$(OBJDIR)/c_jstat.o : c_jstat.c $(INCLUDES)
$(OBJDIR)/c_read.o : c_read.c $(INCLUDES)
$(OBJDIR)/c_obtfi.o : c_obtfi.c $(INCLUDES)
$(OBJDIR)/c_status.o : c_status.c $(INCLUDES)
$(OBJDIR)/c_write.o : c_write.c $(INCLUDES)
$(OBJDIR)/mvl_buf.o : mvl_buf.c $(INCLUDES)
$(OBJDIR)/mvl_dtyp.o : mvl_dtyp.c $(INCLUDES)
$(OBJDIR)/mvl_obj.o : mvl_obj.c $(INCLUDES)
$(OBJDIR)/mvl_objb.o : mvl_objb.c $(INCLUDES)
$(OBJDIR)/mvl_ijou.o : mvl_ijou.c $(INCLUDES)
$(OBJDIR)/mvlu_sbo.o : mvlu_sbo.c $(INCLUDES)
$(OBJDIR)/mvl_send.o : mvl_send.c $(INCLUDES)
$(OBJDIR)/mvl_uca.o : mvl_uca.c $(INCLUDES)
$(OBJDIR)/mvl_serv.o : mvl_serv.c $(INCLUDES)
$(OBJDIR)/mvl_typ2.o : mvl_typ2.c $(INCLUDES)
$(OBJDIR)/mvl_type.o : mvl_type.c $(INCLUDES)
$(OBJDIR)/mvl_var.o : mvl_var.c $(INCLUDES)
$(OBJDIR)/mvlu_rt.o : mvlu_rt.c $(INCLUDES)
$(OBJDIR)/mvlu_scan.o : mvlu_scan.c $(INCLUDES)
$(OBJDIR)/mvluleaf.o : mvluleaf.c $(INCLUDES)
$(OBJDIR)/mvl61850.o : mvl61850.c $(INCLUDES)
$(OBJDIR)/mvl61850_ctl.o : mvl61850_ctl.c $(INCLUDES)
$(OBJDIR)/mvl61850_log.o : mvl61850_log.c $(INCLUDES)
$(OBJDIR)/mvl61850_rpt.o : mvl61850_rpt.c $(INCLUDES)
$(OBJDIR)/mvlu_rpt.o : mvlu_rpt.c $(INCLUDES)
$(OBJDIR)/s_cancel.o : s_cancel.c $(INCLUDES)
$(OBJDIR)/s_concl.o : s_concl.c $(INCLUDES)
$(OBJDIR)/s_defvl.o : s_defvl.c $(INCLUDES)
$(OBJDIR)/s_delvl.o : s_delvl.c $(INCLUDES)
$(OBJDIR)/s_fclose.o : s_fclose.c $(INCLUDES)
$(OBJDIR)/s_fdel.o : s_fdel.c $(INCLUDES)
$(OBJDIR)/s_fren.o : s_fren.c $(INCLUDES)
$(OBJDIR)/s_fdir.o : s_fdir.c $(INCLUDES)
$(OBJDIR)/s_fopen.o : s_fopen.c $(INCLUDES)
$(OBJDIR)/s_fread.o : s_fread.c $(INCLUDES)
$(OBJDIR)/s_jinit.o : s_jinit.c $(INCLUDES)
$(OBJDIR)/s_jinit2.o : s_jinit2.c $(INCLUDES)
$(OBJDIR)/s_jread.o : s_jread.c $(INCLUDES)
$(OBJDIR)/s_getcl.o : s_getcl.c $(INCLUDES)
$(OBJDIR)/s_getnam.o : s_getnam.c $(INCLUDES)
$(OBJDIR)/s_getdom.o : s_getdom.c $(INCLUDES)
$(OBJDIR)/s_getvaa.o : s_getvaa.c $(INCLUDES)
$(OBJDIR)/s_getvla.o : s_getvla.c $(INCLUDES)
$(OBJDIR)/s_ident.o : s_ident.c $(INCLUDES)
$(OBJDIR)/s_info.o : s_info.c $(INCLUDES)
$(OBJDIR)/s_jstat.o : s_jstat.c $(INCLUDES)
$(OBJDIR)/s_read.o : s_read.c $(INCLUDES)
$(OBJDIR)/s_obtfi.o : s_obtfi.c $(INCLUDES)
$(OBJDIR)/s_status.o : s_status.c $(INCLUDES)
$(OBJDIR)/s_write.o : s_write.c $(INCLUDES)
$(OBJDIR)/sclproc.o : sclproc.c $(INCLUDES)
















