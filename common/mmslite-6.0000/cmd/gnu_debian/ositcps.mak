##########################################################################
# SISCO SOFTWARE MODULE HEADER ###########################################
##########################################################################
#			       						 #
# MODULE NAME : ositcps.mak                       			 #
# PRODUCT(S)  : MMS-EASE-Lite						 #
#			       						 #
# MODULE DESCRIPTION :        						 #
# This makefile is used by the Linux 'make' utility to build the         #
# LEANT-MOSI TP0 (RFC 1006) library for MMS-EASE-Lite.All paths including#
# header files and object modules are relative to the current project	 #
# directory which makes this file as environment independent as 	 #
# possible.  Both the DEBUG and Non DEBUG versions of the Utilities are  #
# made with this script.  						 #
#									 #
# To make the 'ositcps_l.a' library:					 #
# 	make -f ositcps.mak         DFLAG=_l   DEFS=-DDEBUG_SISCO	 #
# To make the 'ositcps_n.a' library:					 #
# 	make -f ositcps.mak   	    DFLAG=_n				 #
# To make the 'ositcps_ld.a' library:					 #
# 	make -f ositcps.mak  OPT=-g DFLAG=_ld  DEFS=-DDEBUG_SISCO	 #
# To make the 'ositcps_nd.a' library:					 #
# 	make -f ositcps.mak  OPT=-g DFLAG=_nd				 #
#			       						 #
# MODIFICATION LOG :	       						 #
#  Date     Who   Rev			Comments			 #
# --------  ---  ------   -------------------------------------------	 #
# 02/03/10  JRB    09     Del INCDIR, LIBDIR (now defined in platform.mak).
# 12/15/09  JRB    08     Add acse2dec2.c, acse2dib_arr.c, copp_dec2.c,
#			  and coppdcpa.c.
#			  Del unneeded lean_cfg.c
# 03/17/08  JRB    07     Del unneeded tp4call*.c
# 10/05/07  JRB    06     Del gensock2.c (added to util.mak).
# 08/04/05  EJV    05     Moved platform defs to platform.mak.
#			  Revised link & clean. Added -c in compile rule.
# 01/24/05  JRB    04     Add lean_var.c
# 08/04/04  EJV    03     Add AIX OS support.
#			  Del tp4main.c
# 12/10/03  JRB    02     Add LYNX OS support. Del "no-stack-check" option.
# 10/17/03  JRB    01     New
#			  Same as ositcpe.mak but use gensock2.c, tp0_socks.c
##########################################################################

# Include common and platform-dependent definitions
include ./platform.mak

DFLAG  =
DEFS   =
OPT    =
SRCDIR = ../../uca/leant ../../uca/acse ../../src
PROJ  = ositcps$(DFLAG)
OBJDIR = obj_$(PROJ)
LIST  = >> cc.lst 2>&1
VPATH = $(SRCDIR) $(OBJDIR) $(LIBDIR) $(INCDIR)

ALL_CFLAGS = $(PLATFORM_CFLAGS) \
	$(OPT) -I$(INCDIR) $(DEFS) -DMMS_LITE \
	-DMOSI -DLEAN_T -DTP0_ENABLED \
	$(CFLAGS)

LIBRARY = $(LIBDIR)/$(PROJ).a

# setup the dependencies for the source files
INCLUDES = $(INCDIR)/glbtypes.h \
	$(INCDIR)/sysincs.h \
	$(INCDIR)/stime.h \
	$(INCDIR)/slog.h \
	$(INCDIR)/mem_chk.h \
	$(INCDIR)/memlog.h \
	$(INCDIR)/gen_list.h \
	$(INCDIR)/stime.h \
	$(INCDIR)/lean_a.h \
	$(INCDIR)/acse2log.h \
	$(INCDIR)/asn1defs.h \
	$(INCDIR)/acse2usr.h \
	$(INCDIR)/acse2.h \
	$(INCDIR)/tp0_sock.h \
	$(INCDIR)/tp4.h \
	$(INCDIR)/tp4_encd.h \
	$(INCDIR)/tp4_log.h \
	$(INCDIR)/tp4api.h \
	$(INCDIR)/cfglog.h \
	$(INCDIR)/checksum.h \
	$(INCDIR)/cfg_util.h \
	$(INCDIR)/clnp_log.h \
	$(INCDIR)/clnp_usr.h

# These are the objects that belong in the library
OBJECTS = \
	$(OBJDIR)/acse2dec.o \
	$(OBJDIR)/acse2dec2.o \
	$(OBJDIR)/acse2dib.o \
	$(OBJDIR)/acse2dib_arr.o \
	$(OBJDIR)/acse2enc.o \
	$(OBJDIR)/acse2var.o \
	$(OBJDIR)/checksum.o \
	$(OBJDIR)/coppdaru.o \
	$(OBJDIR)/coppdcp.o \
	$(OBJDIR)/coppdcpa.o \
	$(OBJDIR)/coppdcpr.o \
	$(OBJDIR)/copp_dec.o \
	$(OBJDIR)/copp_dec2.o \
	$(OBJDIR)/copp_enc.o \
	$(OBJDIR)/cospmain.o \
	$(OBJDIR)/cosp_dec.o \
	$(OBJDIR)/cosp_enc.o \
	$(OBJDIR)/lean_var.o \
	$(OBJDIR)/osicfgx.o \
	$(OBJDIR)/tp0calld.o \
	$(OBJDIR)/tp0callg.o \
	$(OBJDIR)/tp0main.o \
	$(OBJDIR)/tp0_dec.o \
	$(OBJDIR)/tp0_socks.o \
	$(OBJDIR)/tp4port.o \
	$(OBJDIR)/tp4_enc.o \
	$(OBJDIR)/tpx_dec.o


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
	rm -rf $(OBJDIR)
	echo "Finished clean" $(LIST)

# Pattern rule for all object files
# NOTE: the $@ is the complete target name including the "$(OBJDIR)/" prefix
$(OBJDIR)/%.o : %.c
#	Enable next line to log name of file being compiled (may help clarify error messages).
#	echo Compiling: $< $(LIST)
	$(CC) -c $(ALL_CFLAGS) $< -o $@ $(LIST)

# Object file target dependencies. All use the pattern rule above.
$(OBJDIR)/acse2dec.o	: acse2dec.c $(INCLUDES)
$(OBJDIR)/acse2dec2.o	: acse2dec2.c $(INCLUDES)
$(OBJDIR)/acse2dib.o	: acse2dib.c $(INCLUDES)
$(OBJDIR)/acse2dib_arr.o: acse2dib_arr.c $(INCLUDES)
$(OBJDIR)/acse2enc.o	: acse2enc.c $(INCLUDES)
$(OBJDIR)/acse2var.o	: acse2var.c $(INCLUDES)
$(OBJDIR)/checksum.o	: checksum.c $(INCLUDES)
$(OBJDIR)/coppdaru.o	: coppdaru.c $(INCLUDES)
$(OBJDIR)/coppdcp.o	: coppdcp.c $(INCLUDES)
$(OBJDIR)/coppdcpa.o	: coppdcpa.c $(INCLUDES)
$(OBJDIR)/coppdcpr.o	: coppdcpr.c $(INCLUDES)
$(OBJDIR)/copp_dec.o	: copp_dec.c $(INCLUDES)
$(OBJDIR)/copp_dec2.o	: copp_dec2.c $(INCLUDES)
$(OBJDIR)/copp_enc.o	: copp_enc.c $(INCLUDES)
$(OBJDIR)/cospmain.o	: cospmain.c $(INCLUDES)
$(OBJDIR)/cosp_dec.o	: cosp_dec.c $(INCLUDES)
$(OBJDIR)/cosp_enc.o	: cosp_enc.c $(INCLUDES)
$(OBJDIR)/lean_var.o : lean_var.c $(INCLUDES)
$(OBJDIR)/osicfgx.o	: osicfgx.c $(INCLUDES)
$(OBJDIR)/tp0calld.o	: tp0calld.c $(INCLUDES)
$(OBJDIR)/tp0callg.o	: tp0callg.c $(INCLUDES)
$(OBJDIR)/tp0main.o	: tp0main.c $(INCLUDES)
$(OBJDIR)/tp0_dec.o	: tp0_dec.c $(INCLUDES)
$(OBJDIR)/tp0_socks.o	: tp0_socks.c $(INCLUDES)
$(OBJDIR)/tp4port.o	: tp4port.c $(INCLUDES)
$(OBJDIR)/tp4send.o	: tp4send.c $(INCLUDES)
$(OBJDIR)/tp4_dec.o	: tp4_dec.c $(INCLUDES)
$(OBJDIR)/tp4_enc.o	: tp4_enc.c $(INCLUDES)
$(OBJDIR)/tpx_dec.o	: tpx_dec.c $(INCLUDES)
