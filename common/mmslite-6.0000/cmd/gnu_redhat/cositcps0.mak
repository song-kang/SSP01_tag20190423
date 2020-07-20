##########################################################################
#  SISCO SOFTWARE MODULE HEADER ##########################################
##########################################################################
#			       						 #
# MODULE NAME : cositcps0.mak                       			 #
# PRODUCT(S)  : MMS-EASE-Lite						 #
#			       						 #
# MODULE DESCRIPTION :        						 #
# This makefile is used by the Linux 'make' utility to build the Client  #
# example for MMS-EASE-Lite. All paths including header 		 #
# files and object modules are relative to the current project directory #
# which makes this file as environment independent as possible.  	 #
# Usage instructions are given below as well as a brief description of   #
# each switch used in the process.  					 #
#			       						 #
# To make the cositcps0_ld executable:					 #
# 	make -f cositcps0.mak    OPT=-g DFLAG=_ld  DEFS=-DDEBUG_SISCO	 #
#									 #
#									 #
# MODIFICATION LOG :	       						 #
#  Date     Who   Rev			Comments			 #
# --------  ---  ------   -------------------------------------------	 #
# 02/17/12  JRB		  Add USE_DIB_LIST define.
#			  Add dib_list.c, scl_dib.c
# 09/14/10  JRB		  Add -DMLOG_ENABLE.
# 02/03/10  JRB    12     Del INCDIR, LIBDIR (now defined in platform.mak).
# 01/12/10  JRB    11     Del ssec0 library.
# 04/15/08  JRB    10     Add -d to foundry_ld command.
# 03/17/08  JRB    09     Added foundry_ld dependency.
# 02/06/07  GLB    08     Added "slogipc" for the Log Viewer
# 11/28/06  JRB    07     Chg mlog lib to mlogl to match other makefiles.
# 11/10/06  JRB    06     Del mvl lib. Add mvlu and mlog libs.
# 09/09/05  JRB    05     Added "reject.c".
# 08/04/05  EJV    04     Moved platform defs to platform.mak.
#			  Revised link & clean. Added -c in compile rule.
# 08/04/04  EJV    03     Add AIX OS support.
# 12/10/03  JRB    02     Add LYNX OS support. Del "no-stack-check" option.
# 10/17/03  JRB    01     Created from client.mak by replacing
#                         event.c with event2.c &
#			  ositcpe.a with ositcps.a
##########################################################################

# Include common and platform-dependent definitions
include ./platform.mak

OPT   =
DEFS  =

# Define directories here
SRCDIR = ../../mvl/usr/client
MMSOPDIR = ../../mvl/usr/client
MMSSRCDIR = ../../src
MVLSRCDIR = ../../mvl/src
FOUNDRYDIR = ../../bin
USRSRCDIR = ../../mvl/usr
ACSESRCDIR = ../../mvl/src/acse
EXEDIR = ../../mvl/usr/client
VPATH = $(SRCDIR) $(MVLSRCDIR) $(MVLSRCDIR)/acse $(MMSSRCDIR) $(USRSRCDIR) \
	$(LIBDIR) $(OBJDIR) $(INCDIR)

PROJ  = cositcps0$(DFLAG)
OBJDIR = obj_$(PROJ)
LIST  = >> cc.lst 2>&1

ALL_CFLAGS = $(PLATFORM_CFLAGS) \
	$(OPT) -I$(INCDIR) $(DEFS) \
	-DMMS_LITE -DMOSI -DLEAN_T -I$(MMSOPDIR) \
	-DMLOG_ENABLE \
	-DUSE_DIB_LIST \
	$(CFLAGS)

LIBS = \
	$(LIBDIR)/mvlu$(DFLAG).a \
	$(LIBDIR)/mlogl$(DFLAG).a \
	$(LIBDIR)/mmsle$(DFLAG).a \
	$(LIBDIR)/mmsl$(DFLAG).a \
	$(LIBDIR)/asn1l$(DFLAG).a \
	$(LIBDIR)/mem$(DFLAG).a \
	$(LIBDIR)/slog$(DFLAG).a \
	$(LIBDIR)/util$(DFLAG).a \
	$(LIBDIR)/ositcps$(DFLAG).a

EXE = $(EXEDIR)/$(PROJ)


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
	$(MMSOPDIR)/clntobj.h

# Define objects used in the exectable
OBJECTS = \
	$(OBJDIR)/client.o \
	$(OBJDIR)/cli_rpt.o \
	$(OBJDIR)/mmsop_en.o \
	$(OBJDIR)/mvlop_en.o \
	$(OBJDIR)/mvl_acse.o \
	$(OBJDIR)/event2.o \
	$(OBJDIR)/logcfgx.o \
	$(OBJDIR)/reject.o \
	$(OBJDIR)/slogipc.o \
	$(OBJDIR)/dib_list.o \
	$(OBJDIR)/scl_dib.o \
	$(OBJDIR)/clntobj.o

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
	rm -f $(SRCDIR)/clntobj.c
	rm -f $(SRCDIR)/clntobj.h
	rm -f $(EXE)
	rm -rf $(OBJDIR)
	echo "Finished clean" $(LIST)

# clntobj.c (and clntobj.h) are created using the foundry utility.
$(SRCDIR)/clntobj.c : clntobj.odf $(FOUNDRYDIR)/foundry_ld
	$(FOUNDRYDIR)/foundry_ld -d -o -c$(ALIGNDIR)/align.cfg $(SRCDIR)/clntobj.odf $(SRCDIR)/clntobj.c $(LIST)

$(SRCDIR)/clntobj.h : clntobj.odf $(FOUNDRYDIR)/foundry_ld
	$(FOUNDRYDIR)/foundry_ld -d -o -c$(ALIGNDIR)/align.cfg $(SRCDIR)/clntobj.odf $(SRCDIR)/clntobj.h $(LIST)

# Pattern rule for all object files
# NOTE: the $@ is the complete target name including the "$(OBJDIR)/" prefix
$(OBJDIR)/%.o : %.c
#	Enable next line to log name of file being compiled (may help clarify error messages).
#	echo Compiling: $< $(LIST)
	$(CC) -c $(ALL_CFLAGS) $< -o $@ $(LIST)

# Object file target dependencies. All use the pattern rule above.
$(OBJDIR)/client.o : client.c $(INCLUDES)
$(OBJDIR)/cli_rpt.o : cli_rpt.c $(INCLUDES)
$(OBJDIR)/mmsop_en.o : mmsop_en.c $(INCLUDES)
$(OBJDIR)/mvlop_en.o : mvlop_en.c $(INCLUDES)
$(OBJDIR)/mvl_acse.o : mvl_acse.c $(INCLUDES)
$(OBJDIR)/event2.o : event2.c $(INCLUDES)
$(OBJDIR)/logcfgx.o : logcfgx.c $(INCLUDES)
$(OBJDIR)/clntobj.o : clntobj.c $(INCLUDES)
$(OBJDIR)/reject.o : reject.c $(INCLUDES)
$(OBJDIR)/slogipc.o : $(MMSSRCDIR)/slogipc.c $(INCLUDES)
$(OBJDIR)/dib_list.o:	dib_list.c $(INCLUDES)
$(OBJDIR)/scl_dib.o:	scl_dib.c $(INCLUDES)

