##########################################################################
# SISCO SOFTWARE MODULE HEADER ###########################################
##########################################################################
#			       						 #
# MODULE NAME : mmsl.mak                       				 #
# PRODUCT(S)  : MMS-EASE-Lite						 #
#			       						 #
# MODULE DESCRIPTION :        						 #
# This makefile is used by the Linux 'make' utility to build the core	 #
# Lite library for MMS-EASE-Lite.  All paths including header 		 #
# files and object modules are relative to the current project directory #
# which makes this file as environment independent as possible.  Both 	 #
# the DEBUG and Non DEBUG versions of the Utilities are made with this 	 #
# script.  								 #
#									 #
# To make the MMSL_L.A:							 #
# 	make -f mmsl.mak     	    DFLAG=_l   DEFS=-DDEBUG_SISCO	 #
# To make the MMSL_N.A:							 #
# 	make -f mmsl.mak     	    DFLAG=_n				 #
# To make the MMSL_LD.A:						 #
# 	make -f mmsl.mak     OPT=-g DFLAG=_ld  DEFS=-DDEBUG_SISCO	 #
# To make the MMSL_ND.A:						 #
# 	make -f mmsl.mak     OPT=-g DFLAG=_nd				 #
#			       						 #
# MODIFICATION LOG :	       						 #
#  Date     Who   Rev			Comments			 #
# --------  ---  ------   -------------------------------------------	 #
# 02/03/10  JRB    07     Del INCDIR, LIBDIR (now defined in platform.mak).
# 08/04/05  EJV    06     Moved platform defs to platform.mak.
#			  Revised link & clean. Added -c in compile rule.
# 08/04/04  EJV    05     Add AIX OS support.
# 01/24/04  JRB    04     Add mmsdtext.c
# 12/10/03  JRB    03     Add LYNX OS support. Del "no-stack-check" option.
# 10/09/03  JRB    02     Chg CFLAGS & LDFLAGS to be "user-settable",
#			   use $(AR), as recommended by GNU Make.
#			  Add "$(OBJDIR)/" prefix before all objects & "%.o",
#			   del "$(OBJDIR)/" prefix before "$@",
#			   then use $(OBJECTS) in $(AR) cmd.
#			  Use ARFLAGS (for QNX = -n -q, default = -r).
# 04/24/03  JRB    02     Added ms_tdef4.c
# 12/09/02  CRM    01     Created from QNX makefile.			 #
##########################################################################

# Include common and platform-dependent definitions
include ./platform.mak

# set the library, object, and include file path names
DFLAG  =
DEFS   =
OPT    =
OBJDIR = obj$(DFLAG)
SRCDIR = ../../src
PROJ  = mmsl$(DFLAG)
LIST  = >> cc.lst 2>&1
VPATH = $(SRCDIR) $(OBJDIR) $(LIBDIR) $(INCDIR)

ALL_CFLAGS = $(PLATFORM_CFLAGS) \
	$(OPT) -I$(INCDIR) $(DEFS) -DMMS_LITE \
	$(CFLAGS)

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
	$(INCDIR)/mvl_defs.h \
	$(INCDIR)/mvl_log.h \
	$(INCDIR)/mloguser.h


# These are the objects that belong in the library
OBJECTS = \
	$(OBJDIR)/mmscancl.o \
	$(OBJDIR)/mmsconcl.o \
	$(OBJDIR)/mmsdata.o \
	$(OBJDIR)/mmsdatat.o \
	$(OBJDIR)/mmsdec.o \
	$(OBJDIR)/mmsdtext.o \
	$(OBJDIR)/mmsinit.o \
	$(OBJDIR)/mmsl_fin.o \
	$(OBJDIR)/mmslvar.o \
	$(OBJDIR)/mmsstat.o \
	$(OBJDIR)/mms_adl.o \
	$(OBJDIR)/mms_alta.o \
	$(OBJDIR)/mms_aref.o \
	$(OBJDIR)/mms_ced.o \
	$(OBJDIR)/mms_ced3.o \
	$(OBJDIR)/mms_ced4.o \
	$(OBJDIR)/mms_ced5.o \
	$(OBJDIR)/mms_ced6.o \
	$(OBJDIR)/mms_ced7.o \
	$(OBJDIR)/mms_err.o \
	$(OBJDIR)/mms_err2.o \
	$(OBJDIR)/mms_err3.o \
	$(OBJDIR)/mms_err4.o \
	$(OBJDIR)/mms_rej.o \
	$(OBJDIR)/mms_rtaa.o \
	$(OBJDIR)/mms_tdef.o \
	$(OBJDIR)/msdataas.o \
	$(OBJDIR)/ms_ext.o \
	$(OBJDIR)/ms_size.o \
	$(OBJDIR)/ms_tdef.o \
	$(OBJDIR)/ms_tdef2.o \
	$(OBJDIR)/ms_tdef4.o \
	$(OBJDIR)/rq_ackev.o \
	$(OBJDIR)/rq_cancl.o \
	$(OBJDIR)/rq_concl.o \
	$(OBJDIR)/rq_defee.o \
	$(OBJDIR)/rq_evnot.o \
	$(OBJDIR)/rq_getcl.o \
	$(OBJDIR)/rq_getdo.o \
	$(OBJDIR)/rq_getea.o \
	$(OBJDIR)/rq_getec.o \
	$(OBJDIR)/rq_getee.o \
	$(OBJDIR)/rq_getpi.o \
	$(OBJDIR)/rq_getty.o \
	$(OBJDIR)/rq_getva.o \
	$(OBJDIR)/rq_getvl.o \
	$(OBJDIR)/rq_ident.o \
	$(OBJDIR)/rq_info.o \
	$(OBJDIR)/rq_init.o \
	$(OBJDIR)/rq_namel.o \
	$(OBJDIR)/rq_read.o \
	$(OBJDIR)/rq_repea.o \
	$(OBJDIR)/rq_repec.o \
	$(OBJDIR)/rq_repee.o \
	$(OBJDIR)/rq_stat.o \
	$(OBJDIR)/rq_trige.o \
	$(OBJDIR)/rq_ustat.o \
	$(OBJDIR)/rq_write.o \
	$(OBJDIR)/rs_ackev.o \
	$(OBJDIR)/rs_cancl.o \
	$(OBJDIR)/rs_concl.o \
	$(OBJDIR)/rs_defee.o \
	$(OBJDIR)/rs_evnot.o \
	$(OBJDIR)/rs_getcl.o \
	$(OBJDIR)/rs_getdo.o \
	$(OBJDIR)/rs_getea.o \
	$(OBJDIR)/rs_getec.o \
	$(OBJDIR)/rs_getee.o \
	$(OBJDIR)/rs_getpi.o \
	$(OBJDIR)/rs_getty.o \
	$(OBJDIR)/rs_getva.o \
	$(OBJDIR)/rs_getvl.o \
	$(OBJDIR)/rs_ident.o \
	$(OBJDIR)/rs_info.o \
	$(OBJDIR)/rs_init.o \
	$(OBJDIR)/rs_namel.o \
	$(OBJDIR)/rs_read.o \
	$(OBJDIR)/rs_repea.o \
	$(OBJDIR)/rs_repec.o \
	$(OBJDIR)/rs_repee.o \
	$(OBJDIR)/rs_stat.o \
	$(OBJDIR)/rs_trige.o \
	$(OBJDIR)/rs_ustat.o \
	$(OBJDIR)/rs_write.o

LIBRARY = $(LIBDIR)/$(PROJ).a

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
$(OBJDIR)/mmscancl.o : mmscancl.c $(INCLUDES)
$(OBJDIR)/mmsconcl.o : mmsconcl.c $(INCLUDES)
$(OBJDIR)/mmsdata.o : mmsdata.c $(INCLUDES)
$(OBJDIR)/mmsdatat.o : mmsdatat.c $(INCLUDES)
$(OBJDIR)/mmsdec.o: mmsdec.c $(INCLUDES)
$(OBJDIR)/mmsdtext.o : mmsdtext.c $(INCLUDES)
$(OBJDIR)/mmsinit.o : mmsinit.c $(INCLUDES)
$(OBJDIR)/mmsl_fin.o : mmsl_fin.c $(INCLUDES)
$(OBJDIR)/mmslvar.o : mmslvar.c $(INCLUDES)
$(OBJDIR)/mmsstat.o : mmsstat.c $(INCLUDES)
$(OBJDIR)/mms_adl.o : mms_adl.c $(INCLUDES)
$(OBJDIR)/mms_alta.o : mms_alta.c $(INCLUDES)
$(OBJDIR)/mms_aref.o : mms_aref.c $(INCLUDES)
$(OBJDIR)/mms_ced.o : mms_ced.c $(INCLUDES)
$(OBJDIR)/mms_ced3.o : mms_ced3.c $(INCLUDES)
$(OBJDIR)/mms_ced4.o : mms_ced4.c $(INCLUDES)
$(OBJDIR)/mms_ced5.o : mms_ced5.c $(INCLUDES)
$(OBJDIR)/mms_ced6.o : mms_ced6.c $(INCLUDES)
$(OBJDIR)/mms_ced7.o : mms_ced7.c $(INCLUDES)
$(OBJDIR)/mms_err.o : mms_err.c $(INCLUDES)
$(OBJDIR)/mms_err2.o : mms_err2.c $(INCLUDES)
$(OBJDIR)/mms_err3.o : mms_err3.c $(INCLUDES)
$(OBJDIR)/mms_err4.o : mms_err4.c $(INCLUDES)
$(OBJDIR)/mms_rej.o : mms_rej.c $(INCLUDES)
$(OBJDIR)/mms_rtaa.o : mms_rtaa.c $(INCLUDES)
$(OBJDIR)/mms_tdef.o : mms_tdef.c $(INCLUDES)
$(OBJDIR)/msdataas.o : msdataas.c $(INCLUDES)
$(OBJDIR)/ms_ext.o : ms_ext.c $(INCLUDES)
$(OBJDIR)/ms_size.o : ms_size.c $(INCLUDES)
$(OBJDIR)/ms_tdef.o : ms_tdef.c $(INCLUDES)
$(OBJDIR)/ms_tdef2.o : ms_tdef2.c $(INCLUDES)
$(OBJDIR)/ms_tdef4.o : ms_tdef4.c $(INCLUDES)
$(OBJDIR)/rq_ackev.o : rq_ackev.c $(INCLUDES)
$(OBJDIR)/rq_cancl.o : rq_cancl.c $(INCLUDES)
$(OBJDIR)/rq_concl.o : rq_concl.c $(INCLUDES)
$(OBJDIR)/rq_defee.o : rq_defee.c $(INCLUDES)
$(OBJDIR)/rq_evnot.o : rq_evnot.c $(INCLUDES)
$(OBJDIR)/rq_getcl.o : rq_getcl.c $(INCLUDES)
$(OBJDIR)/rq_getdo.o : rq_getdo.c $(INCLUDES)
$(OBJDIR)/rq_getea.o : rq_getea.c $(INCLUDES)
$(OBJDIR)/rq_getec.o : rq_getec.c $(INCLUDES)
$(OBJDIR)/rq_getee.o : rq_getee.c $(INCLUDES)
$(OBJDIR)/rq_getpi.o : rq_getpi.c $(INCLUDES)
$(OBJDIR)/rq_getty.o : rq_getty.c $(INCLUDES)
$(OBJDIR)/rq_getva.o : rq_getva.c $(INCLUDES)
$(OBJDIR)/rq_getvl.o : rq_getvl.c $(INCLUDES)
$(OBJDIR)/rq_ident.o : rq_ident.c $(INCLUDES)
$(OBJDIR)/rq_info.o : rq_info.c $(INCLUDES)
$(OBJDIR)/rq_init.o : rq_init.c $(INCLUDES)
$(OBJDIR)/rq_namel.o : rq_namel.c $(INCLUDES)
$(OBJDIR)/rq_read.o : rq_read.c $(INCLUDES)
$(OBJDIR)/rq_repea.o : rq_repea.c $(INCLUDES)
$(OBJDIR)/rq_repec.o : rq_repec.c $(INCLUDES)
$(OBJDIR)/rq_repee.o : rq_repee.c $(INCLUDES)
$(OBJDIR)/rq_stat.o : rq_stat.c $(INCLUDES)
$(OBJDIR)/rq_trige.o : rq_trige.c $(INCLUDES)
$(OBJDIR)/rq_ustat.o : rq_ustat.c $(INCLUDES)
$(OBJDIR)/rq_write.o : rq_write.c $(INCLUDES)
$(OBJDIR)/rs_ackev.o : rs_ackev.c $(INCLUDES)
$(OBJDIR)/rs_cancl.o : rs_cancl.c $(INCLUDES)
$(OBJDIR)/rs_concl.o : rs_concl.c $(INCLUDES)
$(OBJDIR)/rs_defee.o : rs_defee.c $(INCLUDES)
$(OBJDIR)/rs_evnot.o : rs_evnot.c $(INCLUDES)
$(OBJDIR)/rs_getcl.o : rs_getcl.c $(INCLUDES)
$(OBJDIR)/rs_getdo.o : rs_getdo.c $(INCLUDES)
$(OBJDIR)/rs_getea.o : rs_getea.c $(INCLUDES)
$(OBJDIR)/rs_getec.o : rs_getec.c $(INCLUDES)
$(OBJDIR)/rs_getee.o : rs_getee.c $(INCLUDES)
$(OBJDIR)/rs_getpi.o : rs_getpi.c $(INCLUDES)
$(OBJDIR)/rs_getty.o : rs_getty.c $(INCLUDES)
$(OBJDIR)/rs_getva.o : rs_getva.c $(INCLUDES)
$(OBJDIR)/rs_getvl.o : rs_getvl.c $(INCLUDES)
$(OBJDIR)/rs_ident.o : rs_ident.c $(INCLUDES)
$(OBJDIR)/rs_info.o : rs_info.c $(INCLUDES)
$(OBJDIR)/rs_init.o : rs_init.c $(INCLUDES)
$(OBJDIR)/rs_namel.o : rs_namel.c $(INCLUDES)
$(OBJDIR)/rs_read.o : rs_read.c $(INCLUDES)
$(OBJDIR)/rs_repea.o : rs_repea.c $(INCLUDES)
$(OBJDIR)/rs_repec.o : rs_repec.c $(INCLUDES)
$(OBJDIR)/rs_repee.o : rs_repee.c $(INCLUDES)
$(OBJDIR)/rs_stat.o : rs_stat.c $(INCLUDES)
$(OBJDIR)/rs_trige.o : rs_trige.c $(INCLUDES)
$(OBJDIR)/rs_ustat.o : rs_ustat.c $(INCLUDES)
$(OBJDIR)/rs_write.o : rs_write.c $(INCLUDES)
