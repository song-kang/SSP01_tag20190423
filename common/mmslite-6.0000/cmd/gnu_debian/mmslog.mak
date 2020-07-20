##########################################################################
# SISCO SOFTWARE MODULE HEADER ###########################################
##########################################################################
#			       						 #
# MODULE NAME : mmslog.mak                       			 #
# PRODUCT(S)  : MMS-EASE-Lite						 #
#			       						 #
# MODULE DESCRIPTION :        						 #
#   This makefile is used by the Linux 'make' utility to build the Named #
# Type generator for MMS-EASE-Lite.  All paths including header 	 #
# files and object modules are relative to the current project directory #
# which makes this file as environment independent as possible.  	 #
# each switch used in the process.  					 #
#									 #
# To make the MMSLOG_LD:						 #
# 	make -f mmslog.mak     OPT=-g DFLAG=_ld  DEFS=-DDEBUG_SISCO	 #
#									 #
# MODIFICATION LOG :	       						 #
#  Date     Who   Rev			Comments			 #
# --------  ---  ------   -------------------------------------------	 #
# 02/03/10  JRB    06     Del INCDIR, LIBDIR (now defined in platform.mak).
# 08/04/05  EJV    05     Moved platform defs to platform.mak.
#			  Revised link & clean. Added -c in compile rule.
# 08/04/04  EJV    04     Add AIX OS support.
# 12/10/03  JRB    03     Add LYNX OS support. Del "no-stack-check" option.
# 10/21/03  JRB    02     Chg CFLAGS & LDFLAGS to be "user-settable",
#			  as recommended by GNU Make.
#			  Add "$(OBJDIR)/" prefix before all objects & "%.o",
#			   del "$(OBJDIR)/" prefix before "$@",
#			   then use $(OBJECTS) in link cmd.
#			  Del "LIBS" var, then chg "LIBSPATH" to "LIBS"
# 12/19/02  CRM    01     Created from QNX makefile.			 #
##########################################################################

# Include common and platform-dependent definitions
include ./platform.mak

# set the library, object, and include file path names
DEFS   =
OPT    =
SRCDIR = ../../mvl/util/mmslog
MMSSRCDIR = ../../src
PROJ  = mmslog$(DFLAG)
OBJDIR = obj_$(PROJ)
EXEDIR = ../../bin
LIST  = >> cc.lst 2>&1
VPATH = $(SRCDIR) $(MMSSRCDIR) $(OBJDIR) $(LIBDIR) $(INCDIR)

ALL_CFLAGS = $(PLATFORM_CFLAGS) \
	$(OPT) -I$(INCDIR) \
	-I$(SRCDIR) $(DEFS) -DFOUNDRY -DMMS_LITE -DNO_MVL -DMLOG_ENABLE \
	$(CFLAGS)

LIBS = $(LIBDIR)/mlogl$(DFLAG).a $(LIBDIR)/mmsle$(DFLAG).a $(LIBDIR)/mmsl$(DFLAG).a \
           $(LIBDIR)/asn1l$(DFLAG).a $(LIBDIR)/mem$(DFLAG).a $(LIBDIR)/slog$(DFLAG).a \
           $(LIBDIR)/util$(DFLAG).a  

EXE = $(EXEDIR)/$(PROJ)

# setup the dependencies for the source files
INCLUDES = $(INCDIR)/glbtypes.h \
	$(INCDIR)/mvl_defs.h \
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
	$(SRCDIR)/mmsop_en.h

#  object files
OBJECTS = \
	$(OBJDIR)/mmslog.o \
	$(OBJDIR)/arg.o \
	$(OBJDIR)/mms_alta.o \
	$(OBJDIR)/mms_arb.o \
	$(OBJDIR)/mms_rtaa.o \
	$(OBJDIR)/mms_tdef.o \
	$(OBJDIR)/mmsdata.o \
	$(OBJDIR)/mmsdataa.o \
	$(OBJDIR)/mmsdatat.o \
	$(OBJDIR)/mmslvar.o \
	$(OBJDIR)/mmsop_en.o \
	$(OBJDIR)/ms_tdef.o \
	$(OBJDIR)/ms_tdef2.o \
	$(OBJDIR)/ms_tdef3.o \
	$(OBJDIR)/readhex.o \
	$(OBJDIR)/sx_data.o


# Main target is the executable
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
	rm -f $(EXE)
	rm -rf $(OBJDIR)
	echo "Finished clean" $(LIST)

# Pattern rule for all object files
# NOTE: the $@ is the complete target name including the "$(OBJDIR)/" prefix
$(OBJDIR)/%.o : %.c
#	Enable next line to log name of file being compiled (may help clarify error messages).
#	echo Compiling: $< $(LIST)
	$(CC) -c $(ALL_CFLAGS) $< -o $@ $(LIST)

# Object file target dependencies. All use the pattern rule above.
$(OBJDIR)/mmslog.o : mmslog.c $(INCLUDES)
$(OBJDIR)/arg.o : arg.c $(INCLUDES)
$(OBJDIR)/mms_alta.o : mms_alta.c $(INCLUDES)
$(OBJDIR)/mms_arb.o : mms_arb.c $(INCLUDES)
$(OBJDIR)/mms_rtaa.o : mms_rtaa.c $(INCLUDES)
$(OBJDIR)/mms_tdef.o : mms_tdef.c $(INCLUDES)
$(OBJDIR)/mmsdata.o : mmsdata.c $(INCLUDES)
$(OBJDIR)/mmsdataa.o : mmsdataa.c $(INCLUDES)
$(OBJDIR)/mmsdatat.o : mmsdatat.c $(INCLUDES)
$(OBJDIR)/mmslvar.o : mmslvar.c $(INCLUDES)
$(OBJDIR)/mmsop_en.o : mmsop_en.c $(INCLUDES)
$(OBJDIR)/ms_tdef.o : ms_tdef.c $(INCLUDES)
$(OBJDIR)/ms_tdef2.o : ms_tdef2.c $(INCLUDES)
$(OBJDIR)/ms_tdef3.o : ms_tdef3.c $(INCLUDES)
$(OBJDIR)/readhex.o : readhex.c $(INCLUDES)
$(OBJDIR)/sx_data.o : sx_data.c $(INCLUDES)

