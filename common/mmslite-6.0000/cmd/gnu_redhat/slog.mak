##########################################################################
# SISCO SOFTWARE MODULE HEADER ###########################################
##########################################################################
#			       						 #
# MODULE NAME : slog.mak                       				 #
# PRODUCT(S)  : MMS-EASE-Lite						 #
#			       						 #
# MODULE DESCRIPTION :        						 #
# This makefile is used by the Linux 'make' utility to build the SLOG	 #
# library for MMS-EASE-Lite.  All paths including header 		 #
# files and object modules are relative to the current project directory #
# which makes this file as environment independent as possible.  	 #
#									 #
# To make the SLOG_L.A:							 #
# 	make -f slog.mak     	    DFLAG=_l   DEFS=-DDEBUG_SISCO	 #
# To make the SLOG_N.A:							 #
# 	make -f slog.mak     	    DFLAG=_n				 #
# To make the SLOG_LD.A:						 #
# 	make -f slog.mak     OPT=-g DFLAG=_ld  DEFS=-DDEBUG_SISCO	 #
# To make the SLOG_ND.A:						 #
# 	make -f slog.mak     OPT=-g DFLAG=_nd				 #
#									 #
# MODIFICATION LOG :	       						 #
#  Date     Who   Rev			Comments			 #
# --------  ---  ------   -------------------------------------------	 #
# 02/03/10  JRB    09     Del INCDIR, LIBDIR (now defined in platform.mak).
# 08/04/05  EJV    08     Moved platform defs to platform.mak.
#			  Revised link & clean. Added -c in compile rule.
# 04/28/05  GLB    07     Removed mistyped "q"s
# 02/23/05  EJV    06     Updated INCLUDES.
# 08/04/04  EJV    05     Add AIX OS support.
# 07/23/04  JRB    04     Add slogipcs.c
# 12/10/03  JRB    03     Add LYNX OS support. Del "no-stack-check" option.
# 10/09/03  JRB    02     Chg CFLAGS & LDFLAGS to be "user-settable",
#			   use $(AR), as recommended by GNU Make.
#			  Add "$(OBJDIR)/" prefix before all objects & "%.o",
#			   del "$(OBJDIR)/" prefix before "$@",
#			   then use $(OBJECTS) in $(AR) cmd.
#			  Use ARFLAGS (for QNX = -n -q, default = -r).
# 12/11/02  CRM    01     Created from QNX makefile.			 #
##########################################################################

# Include common and platform-dependent definitions
include ./platform.mak

# set the library, object, and include file path names
DFLAG  =
DEFS   =
OPT    =
OBJDIR = obj$(DFLAG)
SRCDIR = ../../src
PROJ  = slog$(DFLAG)
LIST  = >> cc.lst 2>&1
VPATH = $(SRCDIR) $(OBJDIR) $(LIBDIR) $(INCDIR)

ALL_CFLAGS = $(PLATFORM_CFLAGS) \
	$(OPT) -I$(INCDIR) $(DEFS) -DMMS_LITE \
	$(CFLAGS)

# setup the dependencies for the source files
INCLUDES = \
	$(INCDIR)/gen_list.h \
	$(INCDIR)/gensock2.h \
	$(INCDIR)/glbopt.h   \
	$(INCDIR)/glbsem.h   \
	$(INCDIR)/glbtypes.h \
	$(INCDIR)/mem_chk.h  \
	$(INCDIR)/qmem.h     \
	$(INCDIR)/slog.h     \
	$(INCDIR)/sock_log.h \
	$(INCDIR)/stime.h    \
	$(INCDIR)/sysincs.h

# These are the objects that belong in the library
OBJECTS = \
	$(OBJDIR)/sloghex.o \
	$(OBJDIR)/slog.o \
	$(OBJDIR)/slogfil.o \
	$(OBJDIR)/slogipcs.o \
	$(OBJDIR)/slogmem.o \
	$(OBJDIR)/qmem.o

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
$(OBJDIR)/sloghex.o : sloghex.c $(INCLUDES)
$(OBJDIR)/slog.o : slog.c $(INCLUDES)
$(OBJDIR)/slogfil.o : slogfil.c $(INCLUDES)
$(OBJDIR)/slogipcs.o : slogipcs.c $(INCLUDES)
$(OBJDIR)/slogmem.o : slogmem.c $(INCLUDES)
$(OBJDIR)/qmem.o : qmem.c $(INCLUDES)
