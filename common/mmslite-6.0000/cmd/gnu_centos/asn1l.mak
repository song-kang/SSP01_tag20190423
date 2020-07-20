##########################################################################
# SISCO SOFTWARE MODULE HEADER ###########################################
##########################################################################
#			       						 #
# MODULE NAME : asn1l.mak                       			 #
# PRODUCT(S)  : MMS-EASE-Lite						 #
#			       						 #
# MODULE DESCRIPTION :        						 #
#     This makefile is used by the Linux 'make' utility to build the ASN1#
# Lite library for MMS-EASE-Lite.  All paths including header 		 #
# files and object modules are relative to the current project directory #
# which makes this file as environment independent as possible.  Both 	 #
# the DEBUG and Non DEBUG versions of the Utilities are made with this 	 #
# script								 #
#									 #
# To make the ASN1L_L.A:						 #
# 	make -f asn1l.mak     	    DFLAG=_l   DEFS=-DDEBUG_SISCO	 #
# To make the ASN1L_N.A:						 #
# 	make -f asn1l.mak     	    DFLAG=_n				 #
# To make the ASN1L_LD.A:						 #
# 	make -f asn1l.mak    OPT=-g DFLAG=_ld  DEFS=-DDEBUG_SISCO	 #
# To make the ASN1L_ND.A:						 #
# 	make -f asn1l.mak    OPT=-g DFLAG=_nd				 #
#			       						 #
# MODIFICATION LOG :	       						 #
#  Date     Who   Rev			Comments			 #
# --------  ---  ------   -------------------------------------------	 #
# 02/03/10  JRB    07     Del INCDIR, LIBDIR (now defined in platform.mak).
# 08/04/05  EJV    06     Moved platform defs to platform.mak.
#			  Revised link & clean. Added -c in compile rule.
# 02/23/05  EJV    05     Updated INCLUDES.
# 08/04/04  EJV    04     Add AIX OS support.
# 12/10/03  JRB    03     Add LYNX OS support. Del "no-stack-check" option.
# 10/21/03  JRB    02     Chg CFLAGS & LDFLAGS to be "user-settable",
#			   use $(AR), as recommended by GNU Make.
#			  Add "$(OBJDIR)/" prefix before all objects & "%.o",
#			   del "$(OBJDIR)/" prefix before "$@",
#			   then use $(OBJECTS) in $(AR) cmd.
#			  Use ARFLAGS (for QNX = -n -q, default = -r).
#			  Del -v (verbose) option to $(AR).
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
PROJ  = asn1l$(DFLAG)
LIST  = >> cc.lst 2>&1
VPATH = $(SRCDIR) $(OBJDIR) $(LIBDIR) $(INCDIR)


ALL_CFLAGS = $(PLATFORM_CFLAGS) \
	$(OPT) -I$(INCDIR) $(DEFS) -DMMS_LITE \
	$(CFLAGS)

LIBRARY = $(LIBDIR)/$(PROJ).a

# setup the dependencies for the source files
INCLUDES = \
	$(INCDIR)/asn1defs.h \
	$(INCDIR)/asn1log.h  \
	$(INCDIR)/asn1r.h    \
	$(INCDIR)/gen_list.h \
	$(INCDIR)/glbopt.h   \
	$(INCDIR)/glbtypes.h \
	$(INCDIR)/mem_chk.h  \
	$(INCDIR)/mms_def2.h \
	$(INCDIR)/slog.h     \
	$(INCDIR)/sysincs.h

# These are the objects that belong in the library
OBJECTS = \
	$(OBJDIR)/asn1r.o   \
	$(OBJDIR)/ard_btod.o  \
	$(OBJDIR)/ard_bool.o  \
	$(OBJDIR)/ard_bstr.o  \
	$(OBJDIR)/ard_delm.o  \
	$(OBJDIR)/ard_flt.o   \
	$(OBJDIR)/ard_idnt.o  \
	$(OBJDIR)/ard_int.o   \
	$(OBJDIR)/ard_objd.o  \
	$(OBJDIR)/ard_ostr.o  \
	$(OBJDIR)/ard_strn.o  \
	$(OBJDIR)/ard_time.o  \
	$(OBJDIR)/ard_utc.o   \
	$(OBJDIR)/ard_vstr.o  \
	$(OBJDIR)/are_btod.o  \
	$(OBJDIR)/are_bool.o  \
	$(OBJDIR)/are_bstr.o  \
	$(OBJDIR)/are_delm.o  \
	$(OBJDIR)/are_flt.o   \
	$(OBJDIR)/are_int.o   \
	$(OBJDIR)/are_objd.o  \
	$(OBJDIR)/are_ostr.o  \
	$(OBJDIR)/are_time.o  \
	$(OBJDIR)/are_utc.o   \
	$(OBJDIR)/are_vstr.o

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
$(OBJDIR)/asn1r.o : asn1r.c $(INCLUDES)
$(OBJDIR)/ard_btod.o : ard_btod.c $(INCLUDES)
$(OBJDIR)/ard_bool.o : ard_bool.c $(INCLUDES)
$(OBJDIR)/ard_bstr.o : ard_bstr.c $(INCLUDES)
$(OBJDIR)/ard_delm.o : ard_delm.c $(INCLUDES)
$(OBJDIR)/ard_flt.o : ard_flt.c $(INCLUDES)
$(OBJDIR)/ard_idnt.o : ard_idnt.c $(INCLUDES)
$(OBJDIR)/ard_int.o : ard_int.c $(INCLUDES)
$(OBJDIR)/ard_objd.o : ard_objd.c $(INCLUDES)
$(OBJDIR)/ard_ostr.o : ard_ostr.c $(INCLUDES)
$(OBJDIR)/ard_strn.o :ard_strn.c $(INCLUDES)
$(OBJDIR)/ard_time.o : ard_time.c $(INCLUDES)
$(OBJDIR)/ard_utc.o : ard_utc.c $(INCLUDES)
$(OBJDIR)/ard_vstr.o : ard_vstr.c $(INCLUDES)
$(OBJDIR)/are_btod.o : are_btod.c $(INCLUDES)
$(OBJDIR)/are_bool.o : are_bool.c $(INCLUDES)
$(OBJDIR)/are_bstr.o : are_bstr.c $(INCLUDES)
$(OBJDIR)/are_delm.o : are_delm.c $(INCLUDES)
$(OBJDIR)/are_flt.o : are_flt.c $(INCLUDES)
$(OBJDIR)/are_int.o : are_int.c $(INCLUDES)
$(OBJDIR)/are_objd.o : are_objd.c $(INCLUDES)
$(OBJDIR)/are_ostr.o : are_ostr.c $(INCLUDES)
$(OBJDIR)/are_time.o : are_time.c $(INCLUDES)
$(OBJDIR)/are_utc.o : are_utc.c $(INCLUDES)
$(OBJDIR)/are_vstr.o :  are_vstr.c $(INCLUDES)
