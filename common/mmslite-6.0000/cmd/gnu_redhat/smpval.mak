##########################################################################
# SISCO SOFTWARE MODULE HEADER ###########################################
##########################################################################
#
# MODULE NAME : smpval.mak
# PRODUCT(S)  : MMS-EASE-Lite
#
# MODULE DESCRIPTION :
# This makefile is used by the GNU 'make' utility to build the
# "smpval" library for MMS-EASE-Lite.  All paths including header
# files and object modules are relative to the current project directory
# which makes this file as environment independent as possible.  Both
# the DEBUG and Non DEBUG versions of the library are made with this
# script.
#
# To make the smpval_l.a:
# 	make -f smpval.mak DFLAG=_l         DEFS=-DDEBUG_SISCO
# To make the smpval_n.a:
# 	make -f smpval.mak DFLAG=_n
# To make the smpval_ld.a:
# 	make -f smpval.mak DFLAG=_ld OPT=-g DEFS=-DDEBUG_SISCO
# To make the smpval_nd.a:
# 	make -f smpval.mak DFLAG=_nd OPT=-g
#
# MODIFICATION LOG :
#  Date     Who   Rev			Comments
# --------  ---  ------   -------------------------------------------
# 02/03/11  JRB		  Add smpval_log.c
# 02/03/10  JRB    02     Del INCDIR, LIBDIR (now defined in platform.mak).
# 11/30/06  JRB    01     Created.
##########################################################################

# Include common and platform-dependent definitions
include ./platform.mak

# set the library, object, and include file path names
DFLAG  =
DEFS   =
OPT    =
OBJDIR = obj$(DFLAG)
SRCDIR = ../../src/smpval
PROJ  = smpval$(DFLAG)
LIST  = >> cc.lst 2>&1
VPATH = $(SRCDIR) $(OBJDIR) $(LIBDIR) $(INCDIR)

ALL_CFLAGS = $(PLATFORM_CFLAGS) \
	$(OPT) -I$(INCDIR) $(DEFS) -DMMS_LITE \
	$(CFLAGS)

# setup the dependencies for the source files
INCLUDES = $(INCDIR)/glbtypes.h \
	$(INCDIR)/sysincs.h \
	$(INCDIR)/mem_chk.h \
	$(INCDIR)/slog.h \
	$(INCDIR)/smpval.h

# These are the objects that belong in the library
OBJECTS = \
	$(OBJDIR)/smpval_dec.o \
	$(OBJDIR)/smpval_enc.o \
	$(OBJDIR)/smpval_log.o

LIBRARY = $(LIBDIR)/$(PROJ).a

# Main target is the library
# Make sure all objects are up to date
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
$(OBJDIR)/smpval_dec.o : smpval_dec.c $(INCLUDES)
$(OBJDIR)/smpval_enc.o : smpval_enc.c $(INCLUDES)
$(OBJDIR)/smpval_log.o : smpval_log.c $(INCLUDES)
