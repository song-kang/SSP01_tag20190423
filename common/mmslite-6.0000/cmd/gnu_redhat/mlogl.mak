#########################################################################
# SISCO SOFTWARE MODULE HEADER ###########################################
##########################################################################
#			       						 #
# MODULE NAME : mlogl.mak                       			 #
# PRODUCT(S)  : MMS-EASE-Lite						 #
#			       						 #
# MODULE DESCRIPTION :        						 #
#	This makefile is used by the Linux 'make' utility to build the	 #
# MLOG library for MMS-EASE-Lite.  All paths including header 		 #
# files and object modules are relative to the current project directory #
# which makes this file as environment independent as possible.  Both 	 #
# the DEBUG and Non DEBUG versions of the library are made with this 	 #
# script.  								 #
#									 #
# To make the MLOGL_L.A:						 #
# 	make -f mlogl.mak     	    DFLAG=_l   DEFS=-DDEBUG_SISCO	 #
# To make the MLOGL_N.A:						 #
# 	make -f mlogl.mak     	    DFLAG=_n				 #
# To make the MLOGL_LD.A:						 #
# 	make -f mlogl.mak    OPT=-g DFLAG=_ld  DEFS=-DDEBUG_SISCO	 #
# To make the MLOGL_ND.A:						 #
# 	make -f mlogl.mak    OPT=-g DFLAG=_nd				 #
#									 #
# MODIFICATION LOG :	       						 #
#  Date     Who   Rev			Comments			 #
# --------  ---  ------   -------------------------------------------	 #
# 02/03/10  JRB    08     Del INCDIR, LIBDIR (now defined in platform.mak).
# 02/15/08  JRB    07     Del suicacse.h.
# 08/04/05  EJV    06     Moved platform defs to platform.mak.
#			  Revised link & clean. Added -c in compile rule.
# 02/23/05  EJV    05     Updated INCLUDES.
# 08/04/04  EJV    04     Add AIX OS support.
# 12/10/03  JRB    03     Add LYNX OS support. Del "no-stack-check" option.
# 10/09/03  JRB    02     Chg CFLAGS & LDFLAGS to be "user-settable",
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
OBJDIR = obj$(DFLAG)
SRCDIR = ../../src
PROJ  = mlogl$(DFLAG)
LIST  = >> cc.lst 2>&1
VPATH = $(SRCDIR) $(OBJDIR) $(LIBDIR) $(INCDIR)

ALL_CFLAGS = $(PLATFORM_CFLAGS) \
	$(OPT) -I$(INCDIR) $(DEFS) -DMMS_LITE \
	$(CFLAGS)

LIBRARY = $(LIBDIR)/$(PROJ).a

# setup the dependencies for the source files
INCLUDES = \
	$(INCDIR)/acseauth.h \
	$(INCDIR)/asn1defs.h \
	$(INCDIR)/asn1r.h    \
	$(INCDIR)/dibmatch.h \
	$(INCDIR)/gen_list.h \
	$(INCDIR)/glberror.h \
	$(INCDIR)/glbopt.h   \
	$(INCDIR)/glbsem.h   \
	$(INCDIR)/glbtypes.h \
	$(INCDIR)/mem_chk.h  \
	$(INCDIR)/mloguser.h \
	$(INCDIR)/mms_def2.h \
	$(INCDIR)/mmsldefs.h \
	$(INCDIR)/mms_dfun.h \
	$(INCDIR)/mms_err.h  \
	$(INCDIR)/mms_log.h  \
	$(INCDIR)/mms_mp.h   \
	$(INCDIR)/mms_pcon.h \
	$(INCDIR)/mms_pdom.h \
	$(INCDIR)/mms_perr.h \
	$(INCDIR)/mms_pevn.h \
	$(INCDIR)/mms_pfil.h \
	$(INCDIR)/mms_pjou.h \
	$(INCDIR)/mms_pocs.h \
	$(INCDIR)/mms_pprg.h \
	$(INCDIR)/mms_psem.h \
	$(INCDIR)/mms_pvar.h \
	$(INCDIR)/mms_pvmd.h \
	$(INCDIR)/mms_vvar.h \
	$(INCDIR)/mmslog.h   \
	$(INCDIR)/slog.h     \
	$(INCDIR)/ssec.h     \
	$(INCDIR)/stime.h    \
	$(INCDIR)/mmsdefs.h  \
	$(INCDIR)/mmsintr2.h \
	$(INCDIR)/mmsefun.h  \
	$(INCDIR)/sx_arb.h   \
	$(INCDIR)/sx_defs.h  \
	$(INCDIR)/sysincs.h

# These are the objects that belong in the library
OBJECTS = \
	$(OBJDIR)/mlogl.o \
	$(OBJDIR)/mlogaevn.o \
	$(OBJDIR)/mlogafil.o \
	$(OBJDIR)/mlogavar.o \
	$(OBJDIR)/mlogmisc.o \
	$(OBJDIR)/ml_ackev.o \
	$(OBJDIR)/ml_altec.o \
	$(OBJDIR)/ml_altee.o \
	$(OBJDIR)/ml_cancl.o \
	$(OBJDIR)/ml_concl.o \
	$(OBJDIR)/ml_crepi.o \
	$(OBJDIR)/ml_defea.o \
	$(OBJDIR)/ml_defec.o \
	$(OBJDIR)/ml_defee.o \
	$(OBJDIR)/ml_defsc.o \
	$(OBJDIR)/ml_defse.o \
	$(OBJDIR)/ml_defty.o \
	$(OBJDIR)/ml_defva.o \
	$(OBJDIR)/ml_defvl.o \
	$(OBJDIR)/ml_deldo.o \
	$(OBJDIR)/ml_delea.o \
	$(OBJDIR)/ml_delec.o \
	$(OBJDIR)/ml_delee.o \
	$(OBJDIR)/ml_delpi.o \
	$(OBJDIR)/ml_delse.o \
	$(OBJDIR)/ml_delty.o \
	$(OBJDIR)/ml_delva.o \
	$(OBJDIR)/ml_delvl.o \
	$(OBJDIR)/ml_downl.o \
	$(OBJDIR)/ml_evnot.o \
	$(OBJDIR)/ml_fclos.o \
	$(OBJDIR)/ml_fdele.o \
	$(OBJDIR)/ml_frena.o \
	$(OBJDIR)/ml_fdir.o \
	$(OBJDIR)/ml_fopen.o \
	$(OBJDIR)/ml_fread.o \
	$(OBJDIR)/ml_getae.o \
	$(OBJDIR)/ml_getas.o \
	$(OBJDIR)/ml_getcl.o \
	$(OBJDIR)/ml_getdo.o \
	$(OBJDIR)/ml_getea.o \
	$(OBJDIR)/ml_getec.o \
	$(OBJDIR)/ml_getee.o \
	$(OBJDIR)/ml_getpi.o \
	$(OBJDIR)/ml_getsc.o \
	$(OBJDIR)/ml_getty.o \
	$(OBJDIR)/ml_getva.o \
	$(OBJDIR)/ml_getvl.o \
	$(OBJDIR)/ml_ident.o \
	$(OBJDIR)/ml_info.o \
	$(OBJDIR)/ml_init.o \
	$(OBJDIR)/ml_initd.o \
	$(OBJDIR)/ml_initu.o \
	$(OBJDIR)/ml_input.o \
	$(OBJDIR)/ml_jcrea.o \
	$(OBJDIR)/ml_jdele.o \
	$(OBJDIR)/ml_jinit.o \
	$(OBJDIR)/ml_jread.o \
	$(OBJDIR)/ml_jstat.o \
	$(OBJDIR)/ml_jwrit.o \
	$(OBJDIR)/ml_kill.o \
	$(OBJDIR)/ml_loadd.o \
	$(OBJDIR)/ml_namel.o \
	$(OBJDIR)/ml_obtfi.o \
	$(OBJDIR)/ml_outpu.o \
	$(OBJDIR)/ml_rddwn.o \
	$(OBJDIR)/ml_rdupl.o \
	$(OBJDIR)/ml_read.o \
	$(OBJDIR)/ml_relct.o \
	$(OBJDIR)/ml_renam.o \
	$(OBJDIR)/ml_repea.o \
	$(OBJDIR)/ml_repec.o \
	$(OBJDIR)/ml_repee.o \
	$(OBJDIR)/ml_reset.o \
	$(OBJDIR)/ml_resum.o \
	$(OBJDIR)/ml_rsent.o \
	$(OBJDIR)/ml_rspoo.o \
	$(OBJDIR)/ml_rssta.o \
	$(OBJDIR)/ml_start.o \
	$(OBJDIR)/ml_stat.o \
	$(OBJDIR)/ml_stop.o \
	$(OBJDIR)/ml_store.o \
	$(OBJDIR)/ml_takec.o \
	$(OBJDIR)/ml_termd.o \
	$(OBJDIR)/ml_termu.o \
	$(OBJDIR)/ml_trige.o \
	$(OBJDIR)/ml_uploa.o \
	$(OBJDIR)/ml_ustat.o \
	$(OBJDIR)/ml_write.o

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
$(OBJDIR)/mlogl.o : mlogl.c $(INCLUDES)
$(OBJDIR)/mlogaevn.o : mlogaevn.c $(INCLUDES)
$(OBJDIR)/mlogafil.o : mlogafil.c $(INCLUDES)
$(OBJDIR)/mlogavar.o : mlogavar.c $(INCLUDES)
$(OBJDIR)/mlogmisc.o : mlogmisc.c $(INCLUDES)
$(OBJDIR)/ml_ackev.o : ml_ackev.c $(INCLUDES)
$(OBJDIR)/ml_altec.o : ml_altec.c $(INCLUDES)
$(OBJDIR)/ml_altee.o : ml_altee.c $(INCLUDES)
$(OBJDIR)/ml_cancl.o : ml_cancl.c $(INCLUDES)
$(OBJDIR)/ml_concl.o : ml_concl.c $(INCLUDES)
$(OBJDIR)/ml_crepi.o : ml_crepi.c $(INCLUDES)
$(OBJDIR)/ml_defea.o : ml_defea.c $(INCLUDES)
$(OBJDIR)/ml_defec.o : ml_defec.c $(INCLUDES)
$(OBJDIR)/ml_defee.o : ml_defee.c $(INCLUDES)
$(OBJDIR)/ml_defsc.o : ml_defsc.c $(INCLUDES)
$(OBJDIR)/ml_defse.o : ml_defse.c $(INCLUDES)
$(OBJDIR)/ml_defty.o : ml_delse.c $(INCLUDES)
$(OBJDIR)/ml_delty.o : ml_delty.c $(INCLUDES)
$(OBJDIR)/ml_delva.o : ml_delva.c $(INCLUDES)
$(OBJDIR)/ml_delvl.o : ml_delvl.c $(INCLUDES)
$(OBJDIR)/ml_downl.o : ml_downl.c $(INCLUDES)
$(OBJDIR)/ml_evnot.o : ml_evnot.c $(INCLUDES)
$(OBJDIR)/ml_fclos.o : ml_fclos.c $(INCLUDES)
$(OBJDIR)/ml_fdele.o : ml_fdele.c $(INCLUDES)
$(OBJDIR)/ml_frena.o : ml_frena.c $(INCLUDES)
$(OBJDIR)/ml_fdir.o : ml_fdir.c $(INCLUDES)
$(OBJDIR)/ml_fopen.o : ml_fopen.c $(INCLUDES)
$(OBJDIR)/ml_fread.o : ml_fread.c $(INCLUDES)
$(OBJDIR)/ml_getae.o : ml_getae.c $(INCLUDES)
$(OBJDIR)/ml_getas.o : ml_getas.c $(INCLUDES)
$(OBJDIR)/ml_getcl.o : ml_getcl.c $(INCLUDES)
$(OBJDIR)/ml_getdo.o : ml_getdo.c $(INCLUDES)
$(OBJDIR)/ml_getea.o : ml_getea.c $(INCLUDES)
$(OBJDIR)/ml_getec.o : ml_getec.c $(INCLUDES)
$(OBJDIR)/ml_getee.o : ml_getee.c $(INCLUDES)
$(OBJDIR)/ml_getpi.o : ml_getpi.c $(INCLUDES)
$(OBJDIR)/ml_getsc.o : ml_getsc.c $(INCLUDES)
$(OBJDIR)/ml_getty.o : ml_getty.c $(INCLUDES)
$(OBJDIR)/ml_getva.o : ml_getva.c $(INCLUDES)
$(OBJDIR)/ml_getvl.o : ml_getvl.c $(INCLUDES)
$(OBJDIR)/ml_ident.o : ml_ident.c $(INCLUDES)
$(OBJDIR)/ml_info.o : ml_info.c $(INCLUDES)
$(OBJDIR)/ml_init.o : ml_init.c $(INCLUDES)
$(OBJDIR)/ml_initd.o : ml_initd.c $(INCLUDES)
$(OBJDIR)/ml_initu.o : ml_initu.c $(INCLUDES)
$(OBJDIR)/ml_input.o : ml_input.c $(INCLUDES)
$(OBJDIR)/ml_jcrea.o : ml_jcrea.c $(INCLUDES)
$(OBJDIR)/ml_jdele.o : ml_jdele.c $(INCLUDES)
$(OBJDIR)/ml_jinit.o : ml_jinit.c $(INCLUDES)
$(OBJDIR)/ml_jread.o : ml_jread.c $(INCLUDES)
$(OBJDIR)/ml_jstat.o : ml_jstat.c $(INCLUDES)
$(OBJDIR)/ml_jwrit.o : ml_jwrit.c $(INCLUDES)
$(OBJDIR)/ml_kill.o : ml_kill.c $(INCLUDES)
$(OBJDIR)/ml_loadd.o : ml_loadd.c $(INCLUDES)
$(OBJDIR)/ml_namel.o : ml_namel.c $(INCLUDES)
$(OBJDIR)/ml_obtfi.o : ml_obtfi.c $(INCLUDES)
$(OBJDIR)/ml_outpu.o : ml_outpu.c $(INCLUDES)
$(OBJDIR)/ml_rddwn.o : ml_rddwn.c $(INCLUDES)
$(OBJDIR)/ml_rdupl.o : ml_rdupl.c $(INCLUDES)
$(OBJDIR)/ml_read.o : ml_read.c $(INCLUDES)
$(OBJDIR)/ml_relct.o : ml_relct.c $(INCLUDES)
$(OBJDIR)/ml_renam.o : ml_renam.c $(INCLUDES)
$(OBJDIR)/ml_repea.o : ml_repea.c $(INCLUDES)
$(OBJDIR)/ml_repec.o : ml_repec.c $(INCLUDES)
$(OBJDIR)/ml_repee.o : ml_repee.c $(INCLUDES)
$(OBJDIR)/ml_reset.o : ml_reset.c $(INCLUDES)
$(OBJDIR)/ml_resum.o : ml_resum.c $(INCLUDES)
$(OBJDIR)/ml_rsent.o : ml_rsent.c $(INCLUDES)
$(OBJDIR)/ml_rspoo.o : ml_rspoo.c $(INCLUDES)
$(OBJDIR)/ml_rssta.o : ml_rssta.c $(INCLUDES)
$(OBJDIR)/ml_start.o : ml_start.c $(INCLUDES)
$(OBJDIR)/ml_stat.o : ml_stat.c $(INCLUDES)
$(OBJDIR)/ml_stop.o : ml_stop.c $(INCLUDES)
$(OBJDIR)/ml_store.o : ml_store.c $(INCLUDES)
$(OBJDIR)/ml_takec.o : ml_takec.c $(INCLUDES)
$(OBJDIR)/ml_termd.o : ml_termd.c $(INCLUDES)
$(OBJDIR)/ml_termu.o : ml_termu.c $(INCLUDES)
$(OBJDIR)/ml_trige.o : ml_trige.c $(INCLUDES)
$(OBJDIR)/ml_uploa.o : ml_uploa.c $(INCLUDES)
$(OBJDIR)/ml_ustat.o : ml_ustat.c $(INCLUDES)
$(OBJDIR)/ml_write.o : ml_write.c $(INCLUDES)
