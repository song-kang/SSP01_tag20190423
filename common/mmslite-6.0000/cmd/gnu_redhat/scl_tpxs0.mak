##########################################################################
#  SISCO SOFTWARE MODULE HEADER ##########################################
##########################################################################
#			       						 #
# MODULE NAME : scl_tpxs0.mak                       			 #
# PRODUCT(S)  : MMS-EASE-Lite						 #
#			       						 #
# MODULE DESCRIPTION :        						 #
# This makefile is used by the GNU 'make' utility to build the Server    #
# example for MMS-EASE-Lite. All paths including header 		 #
# files and object modules are relative to the current project directory #
# which makes this file as environment independent as possible.  	 #
# Usage instructions are given below as well as a brief description of   #
# each switch used in the process.  					 #
#									 #
# To make the scl_tpxs0_ld executable:					 #
# 	make -f scl_tpxs0.mak    OPT=-g DFLAG=_ld  DEFS=-DDEBUG_SISCO	 #
#									 #
# MODIFICATION LOG :	       						 #
#  Date     Who   Rev			Comments			 #
# --------  ---  ------   -------------------------------------------	 #
# 03/16/12  JRB		  Replace "subnet.c" with "subnet_scl.c".
# 02/17/12  JRB		  Add USE_DIB_LIST, GOOSE_RX_SUPPORT defines                          
#			  Add dib_list.c, scl_dib.c, iec_comn.c, iec_rx.c, iec_sub.c, subnet.c
#			  Add src & inc directory "../../mvl/usr/iecgoose".                     
# 03/24/11  JRB		  Add userleaf_beh.c userleaf_health.c
#			  userwrite.c and -DUSE_RECURSIVE_READ
# 09/14/10  JRB		  Add -DMLOG_ENABLE & link to mlogl library.
# 02/03/10  JRB    11     Del INCDIR, LIBDIR (now defined in platform.mak).
# 01/12/10  JRB    10     Del ssec0 library.
# 12/19/08  JRB    09     Add db_61850.c, userleaf2.c
# 04/15/08  JRB    08     Add -d to foundry_ld command.
# 03/17/08  JRB    07     Added foundry_ld dependency.
# 02/06/07  GLB    06     Added "slogipc" for the Log Viewer
# 12/01/06  JRB    02     Added SMPVAL_SUPPORT and smpval.a
# 12/01/06  JRB    01     Created from scl_srvr.mak by replacing
#			  ositcps.a with ositpxs.a
##########################################################################

# Include common and platform-dependent definitions
include ./platform.mak

# Define directories here

OPT   =
DEFS  =
SRCDIR = ../../mvl/usr/scl_srvr
GOOSEDIR = ../../mvl/usr/iecgoose
MMSOPDIR = ../../mvl/usr/scl_srvr
MMSSRCDIR = ../../src
MVLSRCDIR = ../../mvl/src
USRSRCDIR = ../../mvl/usr
ACSESRCDIR = ../../mvl/src/acse
FOUNDRYDIR = ../../bin
EXEDIR = ../../mvl/usr/scl_srvr
VPATH = $(SRCDIR) $(MVLSRCDIR) $(ACSESRCDIR) $(MMSSRCDIR) $(USRSRCDIR) \
	$(GOOSEDIR) \
	$(LIBDIR) $(OBJDIR) $(INCDIR)

PROJ  = scl_tpxs0$(DFLAG)
OBJDIR = obj_$(PROJ)
LIST  = >> cc.lst 2>&1

# set compiler/linker flags

ifeq "$(PLATFORM)" "QNX"
# "-N" linker option increases stack size to avoid stack overflow
LDFLAGS = -N 100000
endif

ALL_CFLAGS = $(PLATFORM_CFLAGS) \
	$(OPT) -I$(INCDIR) -I$(MMSOPDIR) -I$(GOOSEDIR) $(DEFS) \
	-DMMS_LITE -DMOSI -DLEAN_T -DMVL_UCA -DSMPVAL_SUPPORT \
	-DMLOG_ENABLE \
	-DUSE_RECURSIVE_READ \
	-DUSE_DIB_LIST \
	-DGOOSE_RX_SUPP \
	$(CFLAGS)

LIBS = \
	$(LIBDIR)/smpval$(DFLAG).a \
	$(LIBDIR)/ositpxs$(DFLAG).a \
	$(LIBDIR)/mvlu$(DFLAG).a \
	$(LIBDIR)/mlogl$(DFLAG).a \
	$(LIBDIR)/mmsle$(DFLAG).a \
	$(LIBDIR)/mmsl$(DFLAG).a \
	$(LIBDIR)/asn1l$(DFLAG).a \
	$(LIBDIR)/mem$(DFLAG).a \
	$(LIBDIR)/slog$(DFLAG).a \
	$(LIBDIR)/util$(DFLAG).a

EXE = $(EXEDIR)/$(PROJ)

# Define includes dependancies
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
	$(MMSOPDIR)/mmsop_en.h \
	$(MMSOPDIR)/uca_srvr.h \
	$(MMSOPDIR)/uca_obj.h

# Define objects used in the exectable
OBJECTS = \
	$(OBJDIR)/db_61850.o \
	$(OBJDIR)/scl_srvr.o \
	$(OBJDIR)/startup.o \
	$(OBJDIR)/mmsop_en.o \
	$(OBJDIR)/mvlop_en.o \
	$(OBJDIR)/mvl_acse.o \
	$(OBJDIR)/event2.o \
	$(OBJDIR)/logcfgx.o \
	$(OBJDIR)/uca_obj.o \
	$(OBJDIR)/userleaf.o \
	$(OBJDIR)/userleaf2.o \
	$(OBJDIR)/userleaf_beh.o \
	$(OBJDIR)/userleaf_health.o \
	$(OBJDIR)/userwrite.o \
	$(OBJDIR)/reject.o \
	$(OBJDIR)/slogipc.o \
	$(OBJDIR)/dib_list.o \
	$(OBJDIR)/scl_dib.o \
	$(OBJDIR)/iec_comn.o \
	$(OBJDIR)/iec_rx.o \
	$(OBJDIR)/iec_sub.o \
	$(OBJDIR)/subnet_scl.o \
	$(OBJDIR)/usermap.o


# The main target is the executable
$(EXE):	$(OBJECTS) $(LIBS)
	rm -f $(EXE)
	$(PLATFORM_LINKER) $(PLATFORM_LDFLAGS) $(LDFLAGS) $(PLATFORM_CFLAGS) $(CFLAGS) \
	  -o$(EXE) $(OBJECTS) \
	  $(LIBS) \
	  $(PLATFORM_LIBS) $(LIST)
	echo "FINISHED GENERATING $(EXE) EXECUTABLE" $(LIST)
	echo "-----------------------------------------------------" $(LIST)


clean:
	rm -f $(OBJECTS)
	rm -f $(SRCDIR)/uca_obj.c
	rm -f $(SRCDIR)/uca_obj.h
	rm -f $(EXE)
	rm -rf $(OBJDIR)
	echo "Finished clean" $(LIST)

# uca_obj.c (and uca_obj.h) are created using the foundry utility.
$(SRCDIR)/uca_obj.c : uca_obj.odf $(FOUNDRYDIR)/foundry_ld
	$(FOUNDRYDIR)/foundry_ld -d -o -c$(ALIGNDIR)/align.cfg -t$(SRCDIR)/leafmap.xml $(SRCDIR)/uca_obj.odf $(LIST)

$(SRCDIR)/uca_obj.h : uca_obj.odf $(FOUNDRYDIR)/foundry_ld
	$(FOUNDRYDIR)/foundry_ld -d -o -c$(ALIGNDIR)/align.cfg -t$(SRCDIR)/leafmap.xml $(SRCDIR)/uca_obj.odf $(LIST)


# Pattern rule for all object files
# NOTE: the $@ is the complete target name including the "$(OBJDIR)/" prefix
$(OBJDIR)/%.o : %.c
#	Enable next line to log name of file being compiled (may help clarify error messages).
#	echo Compiling: $< $(LIST)
	$(CC) -c $(ALL_CFLAGS) $< -o $@ $(LIST)

# Object file target dependencies. All use the pattern rule above.
$(OBJDIR)/db_61850.o:	db_61850.c $(INCLUDES)
$(OBJDIR)/scl_srvr.o:	scl_srvr.c $(INCLUDES)
$(OBJDIR)/startup.o:	startup.c $(INCLUDES)
$(OBJDIR)/mmsop_en.o:	mmsop_en.c $(INCLUDES)
$(OBJDIR)/mvlop_en.o:	mvlop_en.c $(INCLUDES)
$(OBJDIR)/mvl_acse.o:	mvl_acse.c $(INCLUDES)
$(OBJDIR)/event2.o:	event2.c $(INCLUDES)
$(OBJDIR)/logcfgx.o:	logcfgx.c $(INCLUDES)
$(OBJDIR)/uca_obj.o:	uca_obj.c $(INCLUDES)
$(OBJDIR)/userleaf.o:	userleaf.c $(INCLUDES)
$(OBJDIR)/userleaf2.o:	userleaf2.c $(INCLUDES)
$(OBJDIR)/userleaf_beh.o:	userleaf_beh.c $(INCLUDES)
$(OBJDIR)/userleaf_health.o:	userleaf_health.c $(INCLUDES)
$(OBJDIR)/usermap.o:	usermap.c $(INCLUDES)
$(OBJDIR)/userwrite.o:	userwrite.c $(INCLUDES)
$(OBJDIR)/reject.o : reject.c $(INCLUDES)
$(OBJDIR)/slogipc.o : $(MMSSRCDIR)/slogipc.c $(INCLUDES)
$(OBJDIR)/dib_list.o:	dib_list.c $(INCLUDES)
$(OBJDIR)/scl_dib.o:	scl_dib.c $(INCLUDES)
$(OBJDIR)/iec_comn.o:	iec_comn.c $(INCLUDES)
$(OBJDIR)/iec_rx.o:	iec_rx.c $(INCLUDES)
$(OBJDIR)/iec_sub.o:	iec_sub.c $(INCLUDES)
$(OBJDIR)/subnet_scl.o:	subnet_scl.c $(INCLUDES)

