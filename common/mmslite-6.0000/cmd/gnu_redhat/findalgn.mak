##########################################################################
# SISCO SOFTWARE MODULE HEADER ###########################################
##########################################################################
#			       						 #
#			       						 #
# MODULE NAME : findalgn.mak                       			 #
# PRODUCT(S)  : MMS-EASE-Lite						 #
#			       						 #
# This makefile is used by the Linux 'make' utility to build the Find	 #
# Alignment Utility for MMS-EASE-Lite.  All paths including header 	 #
# files and object modules are relative to the current project directory # 
# which makes this file as environment independent as possible.  	 #
# Usage instructions are given below as well as a brief description of   #
# each switch used in the process.  					 #
#									 #
# To make the FINDALGN_LD:						 #
# 	make -f findalgn.mak   OPT=-g DFLAG=_ld  DEFS=-DDEBUG_SISCO	 #
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
# 12/17/02  CRM    01     Created from QNX makefile.			 #
##########################################################################

# Include common and platform-dependent definitions
include ./platform.mak

OPT   =
DEFS  =
OBJDIR = obj$(DFLAG)
SRCDIR = ../../mvl/util/foundry
VPATH = $(SRCDIR) $(OBJDIR) $(LIBDIR) $(INCDIR)
PROJ  = findalgn$(DFLAG)
LIST  = >> cc.lst 2>&1

ALL_CFLAGS = $(PLATFORM_CFLAGS) \
	$(OPT) -I$(INCDIR) \
	$(DEFS) -DMMS_LITE \
	$(CFLAGS)

LIBS = $(LIBDIR)/slog$(DFLAG).a $(LIBDIR)/mem$(DFLAG).a $(LIBDIR)/slog$(DFLAG).a \
           $(LIBDIR)/util$(DFLAG).a

EXE = ../../bin/$(PROJ)

# setup the dependencies for the source files
INCLUDES = $(INCDIR)/glbtypes.h \
		$(INCDIR)/sysincs.h \
		$(INCDIR)/mem_chk.h

#  object files
OBJECTS = $(OBJDIR)/findalgn.o

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
	echo "Finished clean" $(LIST)

# Pattern rule for all object files
# NOTE: the $@ is the complete target name including the "$(OBJDIR)/" prefix
$(OBJDIR)/%.o : %.c
#	Enable next line to log name of file being compiled (may help clarify error messages).
#	echo Compiling: $< $(LIST)
	$(CC) -c $(ALL_CFLAGS) $< -o $@ $(LIST)

# Object file target dependencies. All use the pattern rule above.
$(OBJDIR)/findalgn.o : findalgn.c $(INCLUDES)
