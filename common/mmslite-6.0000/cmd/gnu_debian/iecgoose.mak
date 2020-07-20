##########################################################################
#  SISCO SOFTWARE MODULE HEADER ##########################################
##########################################################################
#			       						 #
# MODULE NAME : iecgoose.mak                       			 #
# PRODUCT(S)  : MMS-EASE-Lite						 #
#			       						 #
# MODULE DESCRIPTION :        						 #
# This makefile is used by the Linux 'make' utility to build the IEC GOOSE #
# example for MMS-EASE-Lite. All paths including header 		 #
# files and object modules are relative to the current project directory #
# which makes this file as environment independent as possible.  	 #
# Usage instructions are given below as well as a brief description of   #
# each switch used in the process.  					 #
#			       						 #
# To make the iecgoose_ld executable:					 #
# 	make -f iecgoose.mak    OPT=-g DFLAG=_ld  DEFS=-DDEBUG_SISCO	 #
#									 #
# MODIFICATION LOG :	       						 #
#  Date     Who   Rev			Comments			 #
# --------  ---  ------   -------------------------------------------	 #
# 06/23/10  JRB           Replace ositp4e lib with ositpxs.
# 02/03/10  JRB    10     Del INCDIR, LIBDIR (now defined in platform.mak).
# 08/12/08  RKR    09     Added slogipc.c
# 04/15/08  JRB    08     Add -d to foundry_ld command.
# 03/17/08  JRB    07     Added foundry_ld dependency.
# 10/23/06  JRB    06     Add startup.c
#			  Add ..../scl_srvr to VPATH (for startup.c)
# 09/16/05  JRB    05     Add "-t$(SRCDIR)/leafmap.xml" on foundry cmds.
#			  Del unused last arg on foundry cmds. 
# 09/09/05  JRB    04     Added "reject.c".
# 08/04/05  EJV    03     Moved platform defs to platform.mak.
#			  Revised link & clean. Added -c in compile rule.
# 12/10/03  JRB    02     Add LYNX OS support. Del "no-stack-check" option.
# 11/03/03  JRB    01     New
##########################################################################

# Include common and platform-dependent definitions
include ./platform.mak

OPT   =
DEFS  =

# Define directories here
SRCDIR = ../../mvl/usr/iecgoose
MMSOPDIR = ../../mvl/usr/iecgoose
MMSSRCDIR = ../../src
MVLSRCDIR = ../../mvl/src
FOUNDRYDIR = ../../bin
USRSRCDIR = ../../mvl/usr
ACSESRCDIR = ../../mvl/src/acse
EXEDIR = ../../mvl/usr/iecgoose
# Added "../../mvl/usr/scl_srvr" to VPATH to find "startup.c"
VPATH = $(SRCDIR) $(MVLSRCDIR) $(MVLSRCDIR)/acse $(MMSSRCDIR) $(USRSRCDIR) \
	$(LIBDIR) $(OBJDIR) $(INCDIR) ../../mvl/usr/scl_srvr

PROJ  = iecgoose$(DFLAG)
OBJDIR = obj_$(PROJ)
LIST  = >> cc.lst 2>&1

ALL_CFLAGS = $(PLATFORM_CFLAGS) \
	$(OPT) -I$(INCDIR) $(DEFS) \
	-DMMS_LITE -DMOSI -DLEAN_T -I$(MMSOPDIR) -DMVL_UCA \
	$(CFLAGS)

LIBS = \
	$(LIBDIR)/mvlu$(DFLAG).a \
	$(LIBDIR)/mlogl$(DFLAG).a \
	$(LIBDIR)/mmsle$(DFLAG).a \
	$(LIBDIR)/mmsl$(DFLAG).a \
	$(LIBDIR)/mem$(DFLAG).a \
	$(LIBDIR)/slog$(DFLAG).a \
	$(LIBDIR)/util$(DFLAG).a \
	$(LIBDIR)/ositpxs$(DFLAG).a \
	$(LIBDIR)/asn1l$(DFLAG).a

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
	$(MMSOPDIR)/uca_obj.h 

# Define objects used in the exectable
OBJECTS = \
	$(OBJDIR)/iec_comn.o \
	$(OBJDIR)/iec_demo.o \
	$(OBJDIR)/iec_main.o \
	$(OBJDIR)/iec_rx.o \
	$(OBJDIR)/iec_tx.o \
	$(OBJDIR)/mms_arb.o \
	$(OBJDIR)/mmsop_en.o \
	$(OBJDIR)/mvlop_en.o \
	$(OBJDIR)/mvl_acse.o \
	$(OBJDIR)/event.o \
	$(OBJDIR)/logcfgx.o \
	$(OBJDIR)/rdwrind.o \
	$(OBJDIR)/slogipc.o \
	$(OBJDIR)/reject.o \
	$(OBJDIR)/startup.o \
	$(OBJDIR)/uca_obj.o 

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
$(OBJDIR)/iec_comn.o : iec_comn.c $(INCLUDES)
$(OBJDIR)/iec_demo.o : iec_demo.c $(INCLUDES)
$(OBJDIR)/iec_main.o : iec_main.c $(INCLUDES)
$(OBJDIR)/iec_rx.o : iec_rx.c $(INCLUDES)
$(OBJDIR)/iec_tx.o : iec_tx.c $(INCLUDES)
$(OBJDIR)/mms_arb.o : mms_arb.c $(INCLUDES)
$(OBJDIR)/mmsop_en.o : mmsop_en.c $(INCLUDES)
$(OBJDIR)/mvlop_en.o : mvlop_en.c $(INCLUDES)
$(OBJDIR)/mvl_acse.o : mvl_acse.c $(INCLUDES)
$(OBJDIR)/event.o : event.c $(INCLUDES)
$(OBJDIR)/logcfgx.o : logcfgx.c $(INCLUDES)
$(OBJDIR)/rdwrind.o : rdwrind.c $(INCLUDES)
$(OBJDIR)/slogipc.o : slogipc.c $(INCLUDES)
$(OBJDIR)/uca_obj.o : uca_obj.c $(INCLUDES)
$(OBJDIR)/reject.o : reject.c $(INCLUDES)
$(OBJDIR)/startup.o : startup.c $(INCLUDES)
