##########################################################################
# SISCO SOFTWARE MODULE HEADER ###########################################
##########################################################################
#			       						 #
# MODULE NAME : mmsle.mak                       			 #
# PRODUCT(S)  : MMS-EASE-Lite						 #
#			       						 #
# MODULE DESCRIPTION :        						 #
# 	This makefile is used by the Linux 'make' utility to build the 	 #
# MMS-EASE-Lite exensions library.   All paths including header 	 #
# files and object modules are relative to the current project directory #
# which makes this file as environment independent as possible.  Both    #
# the DEBUG and Non DEBUG versions of the Utilities are made with this   #
# script.  								 #
#									 #
# To make the MMSLE_L.A:						 #
# 	make -f mmsle.mak     	    DFLAG=_l   DEFS=-DDEBUG_SISCO	 #
# To make the MMSLE_N.A:						 #
# 	make -f mmsle.mak     	    DFLAG=_n				 #
# To make the MMSLE_LD.A:						 #
# 	make -f mmsle.mak     OPT=-g DFLAG=_ld  DEFS=-DDEBUG_SISCO	 #
# To make the MMSLE_ND.A:						 #
# 	make -f mmsle.mak     OPT=-g DFLAG=_nd				 #
#			       						 #
# MODIFICATION LOG :	       						 #
#  Date     Who   Rev			Comments			 #
# --------  ---  ------   -------------------------------------------	 #
# 02/03/10  JRB    06     Del INCDIR, LIBDIR (now defined in platform.mak).
# 08/04/05  EJV    05     Moved platform defs to platform.mak.
#			  Revised link & clean. Added -c in compile rule.
# 08/04/04  EJV    04     Add AIX OS support.
# 12/10/03  JRB    03     Add LYNX OS support. Del "no-stack-check" option.
# 10/21/03  JRB    02     Chg CFLAGS & LDFLAGS to be "user-settable",
#			   use $(AR), as recommended by GNU Make.
#			  Add "$(OBJDIR)/" prefix before all objects & "%.o",
#			   del "$(OBJDIR)/" prefix before "$@",
#			   then use $(OBJECTS) in $(AR) cmd.
#			  Use ARFLAGS (for QNX = -n -q, default = -r).
# 12/18/02  CRM    01     Created from QNX makefile.			 #
##########################################################################

# Include common and platform-dependent definitions
include ./platform.mak

# set the library, object, and include file path names
DFLAG  =
DEFS   =
OPT    =
OBJDIR = obj$(DFLAG)
SRCDIR = ../../src
PROJ  = mmsle$(DFLAG)
LIST  = >> cc.lst 2>&1
VPATH = $(SRCDIR) $(OBJDIR) $(LIBDIR) $(INCDIR)

ALL_CFLAGS = $(PLATFORM_CFLAGS) \
	$(OPT) -I$(INCDIR) $(DEFS) -DMMS_LITE \
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
	$(INCDIR)/mmsefun.h

# These are the objects that belong in the library
OBJECTS = \
	$(OBJDIR)/mms_ced1.o \
	$(OBJDIR)/mms_ced2.o \
	$(OBJDIR)/rq_altec.o \
	$(OBJDIR)/rq_altee.o \
	$(OBJDIR)/rq_crepi.o \
	$(OBJDIR)/rq_defea.o \
	$(OBJDIR)/rq_defec.o \
	$(OBJDIR)/rq_defsc.o \
	$(OBJDIR)/rq_defse.o \
	$(OBJDIR)/rq_defty.o \
	$(OBJDIR)/rq_defva.o \
	$(OBJDIR)/rq_defvl.o \
	$(OBJDIR)/rq_deldo.o \
	$(OBJDIR)/rq_delea.o \
	$(OBJDIR)/rq_delec.o \
	$(OBJDIR)/rq_delee.o \
	$(OBJDIR)/rq_delpi.o \
	$(OBJDIR)/rq_delse.o \
	$(OBJDIR)/rq_delty.o \
	$(OBJDIR)/rq_delva.o \
	$(OBJDIR)/rq_delvl.o \
	$(OBJDIR)/rq_downl.o \
	$(OBJDIR)/rq_fclos.o \
	$(OBJDIR)/rq_fdele.o \
	$(OBJDIR)/rq_frena.o \
	$(OBJDIR)/rq_fdir.o  \
	$(OBJDIR)/rq_fopen.o \
	$(OBJDIR)/rq_fread.o \
	$(OBJDIR)/rq_getae.o \
	$(OBJDIR)/rq_getas.o \
	$(OBJDIR)/rq_getsc.o \
	$(OBJDIR)/rq_initd.o \
	$(OBJDIR)/rq_initu.o \
	$(OBJDIR)/rq_input.o \
	$(OBJDIR)/rq_jcrea.o \
	$(OBJDIR)/rq_jdele.o \
	$(OBJDIR)/rq_jinit.o \
	$(OBJDIR)/rq_jread.o \
	$(OBJDIR)/rq_jstat.o \
	$(OBJDIR)/rq_jwrit.o \
	$(OBJDIR)/rq_kill.o  \
	$(OBJDIR)/rq_loadd.o \
	$(OBJDIR)/rq_obtfi.o \
	$(OBJDIR)/rq_outpu.o \
	$(OBJDIR)/rq_rddwn.o \
	$(OBJDIR)/rq_rdupl.o \
	$(OBJDIR)/rq_relct.o \
	$(OBJDIR)/rq_renam.o \
	$(OBJDIR)/rq_reset.o \
	$(OBJDIR)/rq_resum.o \
	$(OBJDIR)/rq_rsent.o \
	$(OBJDIR)/rq_rspoo.o \
	$(OBJDIR)/rq_rssta.o \
	$(OBJDIR)/rq_start.o \
	$(OBJDIR)/rq_stop.o  \
	$(OBJDIR)/rq_store.o \
	$(OBJDIR)/rq_takec.o \
	$(OBJDIR)/rq_termd.o \
	$(OBJDIR)/rq_termu.o \
	$(OBJDIR)/rq_uploa.o \
	$(OBJDIR)/rs_altec.o \
	$(OBJDIR)/rs_altee.o \
	$(OBJDIR)/rs_crepi.o \
	$(OBJDIR)/rs_defea.o \
	$(OBJDIR)/rs_defec.o \
	$(OBJDIR)/rs_defsc.o \
	$(OBJDIR)/rs_defse.o \
	$(OBJDIR)/rs_defty.o \
	$(OBJDIR)/rs_defva.o \
	$(OBJDIR)/rs_defvl.o \
	$(OBJDIR)/rs_deldo.o \
	$(OBJDIR)/rs_delea.o \
	$(OBJDIR)/rs_delec.o \
	$(OBJDIR)/rs_delee.o \
	$(OBJDIR)/rs_delpi.o \
	$(OBJDIR)/rs_delse.o \
	$(OBJDIR)/rs_delty.o \
	$(OBJDIR)/rs_delva.o \
	$(OBJDIR)/rs_delvl.o \
	$(OBJDIR)/rs_downl.o \
	$(OBJDIR)/rs_fclos.o \
	$(OBJDIR)/rs_fdele.o \
	$(OBJDIR)/rs_frena.o \
	$(OBJDIR)/rs_fdir.o  \
	$(OBJDIR)/rs_fopen.o \
	$(OBJDIR)/rs_fread.o \
	$(OBJDIR)/rs_getae.o \
	$(OBJDIR)/rs_getas.o \
	$(OBJDIR)/rs_getsc.o \
	$(OBJDIR)/rs_initd.o \
	$(OBJDIR)/rs_initu.o \
	$(OBJDIR)/rs_input.o \
	$(OBJDIR)/rs_jcrea.o \
	$(OBJDIR)/rs_jdele.o \
	$(OBJDIR)/rs_jinit.o \
	$(OBJDIR)/rs_jread.o \
	$(OBJDIR)/rs_jstat.o \
	$(OBJDIR)/rs_jwrit.o \
	$(OBJDIR)/rs_kill.o  \
	$(OBJDIR)/rs_loadd.o \
	$(OBJDIR)/rs_obtfi.o \
	$(OBJDIR)/rs_outpu.o \
	$(OBJDIR)/rs_rddwn.o \
	$(OBJDIR)/rs_rdupl.o \
	$(OBJDIR)/rs_relct.o \
	$(OBJDIR)/rs_renam.o \
	$(OBJDIR)/rs_reset.o \
	$(OBJDIR)/rs_resum.o \
	$(OBJDIR)/rs_rsent.o \
	$(OBJDIR)/rs_rspoo.o \
	$(OBJDIR)/rs_rssta.o \
	$(OBJDIR)/rs_start.o \
	$(OBJDIR)/rs_stop.o  \
	$(OBJDIR)/rs_store.o \
	$(OBJDIR)/rs_takec.o \
	$(OBJDIR)/rs_termd.o \
	$(OBJDIR)/rs_termu.o \
	$(OBJDIR)/rs_uploa.o

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
$(OBJDIR)/mms_ced1.o : mms_ced1.c $(INCLUDES)
$(OBJDIR)/mms_ced2.o : mms_ced2.c $(INCLUDES)
$(OBJDIR)/rq_altec.o : rq_altec.c $(INCLUDES)
$(OBJDIR)/rq_altee.o : rq_altee.c $(INCLUDES)
$(OBJDIR)/rq_crepi.o : rq_crepi.c $(INCLUDES)
$(OBJDIR)/rq_defea.o : rq_defea.c $(INCLUDES)
$(OBJDIR)/rq_defec.o : rq_defec.c $(INCLUDES)
$(OBJDIR)/rq_defsc.o : rq_defsc.c $(INCLUDES)
$(OBJDIR)/rq_defse.o : rq_defse.c $(INCLUDES)
$(OBJDIR)/rq_defty.o : rq_defty.c $(INCLUDES)
$(OBJDIR)/rq_defva.o : rq_defva.c $(INCLUDES)
$(OBJDIR)/rq_defvl.o : rq_defvl.c $(INCLUDES)
$(OBJDIR)/rq_deldo.o : rq_deldo.c $(INCLUDES)
$(OBJDIR)/rq_delea.o : rq_delea.c $(INCLUDES)
$(OBJDIR)/rq_delec.o : rq_delec.c $(INCLUDES)
$(OBJDIR)/rq_delee.o : rq_delee.c $(INCLUDES)
$(OBJDIR)/rq_delpi.o : rq_delpi.c $(INCLUDES)
$(OBJDIR)/rq_delse.o : rq_delse.c $(INCLUDES)
$(OBJDIR)/rq_delty.o : rq_delty.c $(INCLUDES)
$(OBJDIR)/rq_delva.o : rq_delva.c $(INCLUDES)
$(OBJDIR)/rq_delvl.o : rq_delvl.c $(INCLUDES)
$(OBJDIR)/rq_downl.o : rq_downl.c $(INCLUDES)
$(OBJDIR)/rq_fclos.o : rq_fclos.c $(INCLUDES)
$(OBJDIR)/rq_fdele.o : rq_fdele.c $(INCLUDES)
$(OBJDIR)/rq_fdir.o : rq_fdir.c $(INCLUDES)
$(OBJDIR)/rq_fopen.o : rq_fopen.c $(INCLUDES)
$(OBJDIR)/rq_fread.o : rq_fread.c $(INCLUDES)
$(OBJDIR)/rq_frena.o : rq_frena.c $(INCLUDES)
$(OBJDIR)/rq_getae.o : rq_getae.c $(INCLUDES)
$(OBJDIR)/rq_getas.o : rq_getas.c $(INCLUDES)
$(OBJDIR)/rq_getsc.o : rq_getsc.c $(INCLUDES)
$(OBJDIR)/rq_initd.o : rq_initd.c $(INCLUDES)
$(OBJDIR)/rq_initu.o : rq_initu.c $(INCLUDES)
$(OBJDIR)/rq_input.o : rq_input.c $(INCLUDES)
$(OBJDIR)/rq_jcrea.o : rq_jcrea.c $(INCLUDES)
$(OBJDIR)/rq_jdele.o : rq_jdele.c $(INCLUDES)
$(OBJDIR)/rq_jinit.o : rq_jinit.c $(INCLUDES)
$(OBJDIR)/rq_jread.o : rq_jread.c $(INCLUDES)
$(OBJDIR)/rq_jstat.o : rq_jstat.c $(INCLUDES)
$(OBJDIR)/rq_jwrit.o : rq_jwrit.c $(INCLUDES)
$(OBJDIR)/rq_kill.o : rq_kill.c $(INCLUDES)
$(OBJDIR)/rq_loadd.o : rq_loadd.c $(INCLUDES)
$(OBJDIR)/rq_obtfi.o : rq_obtfi.c $(INCLUDES)
$(OBJDIR)/rq_outpu.o : rq_outpu.c $(INCLUDES)
$(OBJDIR)/rq_rddwn.o : rq_rddwn.c $(INCLUDES)
$(OBJDIR)/rq_rdupl.o : rq_rdupl.c $(INCLUDES)
$(OBJDIR)/rq_relct.o : rq_relct.c $(INCLUDES)
$(OBJDIR)/rq_renam.o : rq_renam.c $(INCLUDES)
$(OBJDIR)/rq_reset.o : rq_reset.c $(INCLUDES)
$(OBJDIR)/rq_resum.o : rq_resum.c $(INCLUDES)
$(OBJDIR)/rq_rsent.o : rq_rsent.c $(INCLUDES)
$(OBJDIR)/rq_rspoo.o : rq_rspoo.c $(INCLUDES)
$(OBJDIR)/rq_rssta.o : rq_rssta.c $(INCLUDES)
$(OBJDIR)/rq_start.o : rq_start.c $(INCLUDES)
$(OBJDIR)/rq_stop.o : rq_stop.c $(INCLUDES)
$(OBJDIR)/rq_store.o : rq_store.c $(INCLUDES)
$(OBJDIR)/rq_takec.o : rq_takec.c $(INCLUDES)
$(OBJDIR)/rq_termd.o : rq_termd.c $(INCLUDES)
$(OBJDIR)/rq_termu.o : rq_termu.c $(INCLUDES)
$(OBJDIR)/rq_uploa.o : rq_uploa.c $(INCLUDES)
$(OBJDIR)/rs_altec.o : rs_altec.c $(INCLUDES)
$(OBJDIR)/rs_altee.o : rs_altee.c $(INCLUDES)
$(OBJDIR)/rs_crepi.o : rs_crepi.c $(INCLUDES)
$(OBJDIR)/rs_defea.o : rs_defea.c $(INCLUDES)
$(OBJDIR)/rs_defec.o : rs_defec.c $(INCLUDES)
$(OBJDIR)/rs_defsc.o : rs_defsc.c $(INCLUDES)
$(OBJDIR)/rs_defse.o : rs_defse.c $(INCLUDES)
$(OBJDIR)/rs_defty.o : rs_defty.c $(INCLUDES)
$(OBJDIR)/rs_defva.o : rs_defva.c $(INCLUDES)
$(OBJDIR)/rs_defvl.o : rs_defvl.c $(INCLUDES)
$(OBJDIR)/rs_deldo.o : rs_deldo.c $(INCLUDES)
$(OBJDIR)/rs_delea.o : rs_delea.c $(INCLUDES)
$(OBJDIR)/rs_delec.o : rs_delec.c $(INCLUDES)
$(OBJDIR)/rs_delee.o : rs_delee.c $(INCLUDES)
$(OBJDIR)/rs_delpi.o : rs_delpi.c $(INCLUDES)
$(OBJDIR)/rs_delse.o : rs_delse.c $(INCLUDES)
$(OBJDIR)/rs_delty.o : rs_delty.c $(INCLUDES)
$(OBJDIR)/rs_delva.o : rs_delva.c $(INCLUDES)
$(OBJDIR)/rs_delvl.o : rs_delvl.c $(INCLUDES)
$(OBJDIR)/rs_downl.o : rs_downl.c $(INCLUDES)
$(OBJDIR)/rs_fclos.o : rs_fclos.c $(INCLUDES)
$(OBJDIR)/rs_fdele.o : rs_fdele.c $(INCLUDES)
$(OBJDIR)/rs_fdir.o :  rs_fdir.c $(INCLUDES)
$(OBJDIR)/rs_fopen.o : rs_fopen.c $(INCLUDES)
$(OBJDIR)/rs_fread.o : rs_fread.c $(INCLUDES)
$(OBJDIR)/rs_frena.o : rs_frena.c $(INCLUDES)
$(OBJDIR)/rs_getae.o : rs_getae.c $(INCLUDES)
$(OBJDIR)/rs_getas.o : rs_getas.c $(INCLUDES)
$(OBJDIR)/rs_getsc.o : rs_getsc.c $(INCLUDES)
$(OBJDIR)/rs_initd.o : rs_initd.c $(INCLUDES)
$(OBJDIR)/rs_initu.o : rs_initu.c $(INCLUDES)
$(OBJDIR)/rs_input.o : rs_input.c $(INCLUDES)
$(OBJDIR)/rs_jcrea.o : rs_jcrea.c $(INCLUDES)
$(OBJDIR)/rs_jdele.o : rs_jdele.c $(INCLUDES)
$(OBJDIR)/rs_jinit.o : rs_jinit.c $(INCLUDES)
$(OBJDIR)/rs_jread.o : rs_jread.c $(INCLUDES)
$(OBJDIR)/rs_jstat.o : rs_jstat.c $(INCLUDES)
$(OBJDIR)/rs_jwrit.o : rs_jwrit.c $(INCLUDES)
$(OBJDIR)/rs_kill.o : rs_kill.c $(INCLUDES)
$(OBJDIR)/rs_loadd.o : rs_loadd.c $(INCLUDES)
$(OBJDIR)/rs_obtfi.o : rs_obtfi.c $(INCLUDES)
$(OBJDIR)/rs_outpu.o : rs_outpu.c $(INCLUDES)
$(OBJDIR)/rs_rddwn.o : rs_rddwn.c $(INCLUDES)
$(OBJDIR)/rs_rdupl.o : rs_rdupl.c $(INCLUDES)
$(OBJDIR)/rs_relct.o : rs_relct.c $(INCLUDES)
$(OBJDIR)/rs_renam.o : rs_renam.c $(INCLUDES)
$(OBJDIR)/rs_reset.o : rs_reset.c $(INCLUDES)
$(OBJDIR)/rs_resum.o : rs_resum.c $(INCLUDES)
$(OBJDIR)/rs_rsent.o : rs_rsent.c $(INCLUDES)
$(OBJDIR)/rs_rspoo.o : rs_rspoo.c $(INCLUDES)
$(OBJDIR)/rs_rssta.o : rs_rssta.c $(INCLUDES)
$(OBJDIR)/rs_start.o : rs_start.c $(INCLUDES)
$(OBJDIR)/rs_stop.o : rs_stop.c $(INCLUDES)
$(OBJDIR)/rs_store.o : rs_store.c $(INCLUDES)
$(OBJDIR)/rs_takec.o : rs_takec.c $(INCLUDES)
$(OBJDIR)/rs_termd.o : rs_termd.c $(INCLUDES)
$(OBJDIR)/rs_termu.o : rs_termu.c $(INCLUDES)
$(OBJDIR)/rs_uploa.o : rs_uploa.c $(INCLUDES)
