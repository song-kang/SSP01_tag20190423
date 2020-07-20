#########################################################################
# SISCO SOFTWARE MODULE HEADER ##########################################
#########################################################################
#
# MODULE NAME : stackcfg.mak
# PRODUCT(S)  : MMS-EASE-Lite
#
# MODULE DESCRIPTION :
# This makefile is used by the 'make' utility to build the "stackcfg" lib.
# Relative paths used for maximum portability.
#
# MODIFICATION LOG :
#  Date     Who   Rev			Comments
# --------  ---  ------   -------------------------------------------
# 12/30/10  EJV           Revised to keep mak files uniform.
#                         Chg OBJDIR to depend on PROJ name.
#			  Updated 'clean' section.
# 01/12/10  JRB    01     Created.
#########################################################################

# Include common and platform-dependent definitions
include ./platform.mak

#---------------------------------------------------------------
# set the include, source, object, and  library files path names
#---------------------------------------------------------------
DFLAG        =
DEFS         =
OPT          =
SRCDIR       = ../../security/ssec
PROJ         = stackcfg$(DFLAG)
OBJDIR       = obj_$(PROJ)
LIST         = >> ./cc.lst 2>&1

VPATH        = $(INCDIR) $(SRCDIR) $(OBJDIR) $(LIBDIR) 

LIBRARY      = $(LIBDIR)/$(PROJ).a

#-------------------
# set compiler flags
#-------------------
ALL_CFLAGS = $(PLATFORM_CFLAGS) $(CFLAGS) $(OPT) \
	-I$(INCDIR) \
	$(DEFS) -DMMS_LITE 

#--------------------------------------------
# setup the dependencies for the source files
#--------------------------------------------
INCLUDES =

# These are the objects that belong in the library
OBJECTS = \
	$(OBJDIR)/seccfg_int.o  \
	$(OBJDIR)/sstackcfg_mvl.o \
	$(OBJDIR)/sstackcfg.o    \
	$(OBJDIR)/sstackcfgxml.o

#---------------------------
# Main target is the library
#---------------------------
# If any objects have changed, delete and recreate it.
$(LIBRARY): $(OBJECTS)
	$(AR) $(ARFLAGS) $(LIBRARY) $(OBJECTS) $(LIST)
	echo "FINISHED CREATING $(LIBRARY) LIBRARY" $(LIST)
	echo "-----------------------------------------------------" $(LIST)

clean:
	rm -f $(OBJECTS)
	rm -f $(LIBRARY)
	rm -rf $(OBJDIR)
	echo "Finished clean" $(LIST)

#----------------------------------
# Pattern rule for all object files
#----------------------------------
# NOTE: the $@ is the complete target name including the "$(OBJDIR)/" prefix
$(OBJDIR)/%.o : %.c
#	Enable next line to log name of file being compiled (may help clarify error messages).
#	echo Compiling: $< $(LIST)
	$(CC) -c $(ALL_CFLAGS) $< -o $@ $(LIST)

#-----------------------------------------------------------------
# Object file target dependencies. All use the pattern rule above.
#-----------------------------------------------------------------
$(OBJDIR)/seccfg_int.o		: seccfg_int.c   $(INCLUDES)
$(OBJDIR)/sstackcfg_mvl.o	: sstackcfg_mvl.c $(INCLUDES)
$(OBJDIR)/sstackcfg.o		: sstackcfg.c    $(INCLUDES)
$(OBJDIR)/sstackcfgxml.o	: sstackcfgxml.c $(INCLUDES)
