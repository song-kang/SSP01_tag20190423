##########################################################################
# SISCO SOFTWARE MODULE HEADER ###########################################
##########################################################################
#			       						 #                                
# MODULE NAME : ositpxs.mak                   			 	 #            
# PRODUCT(S)  : MMS-EASE-Lite						 #                
#			       						 #                
# MODULE DESCRIPTION :        						 #                
# This makefile is used by the GNU 'make' utility to build a library     #
# for MMS-EASE-Lite.  All paths including header 		         #
# files and object modules are relative to the current project 		 #
# directory which makes this file as environment independent as          #
# possible.  								 #
#									 #
# To make the 'ositpxs_l.a' library:					 #
# 	make -f ositpxs.mak         DFLAG=_l   DEFS=-DDEBUG_SISCO	 #
# To make the 'ositpxs_n.a' library:					 #
# 	make -f ositpxs.mak   	    DFLAG=_n				 #
# To make the 'ositpxs_ld.a' library:					 #
# 	make -f ositpxs.mak  OPT=-g DFLAG=_ld  DEFS=-DDEBUG_SISCO	 #
# To make the 'ositpxs_nd.a' library:					 #
# 	make -f ositpxs.mak  OPT=-g DFLAG=_nd				 #
#								   	 #
# MODIFICATION LOG :	       						 #
#  Date     Who   Rev			Comments			 #
# --------  ---  ------   -------------------------------------------	 #
# 06/25/10  JRB		  Del tp4_dec, tp4_hc, tp4calld, tp4callg,
#			  tp4main, tp4send, clnp_thr, and -DTP4_ENABLED.
# 02/03/10  JRB    04     Del INCDIR, LIBDIR (now defined in platform.mak).
# 12/15/09  JRB    03     Add acse2dec2.c, acse2dib_arr.c, copp_dec2.c,
#			  and coppdcpa.c.
#			  Del unneeded lean_cfg.c
# 10/05/07  JRB    02     Del gensock2.c (added to util.mak).
# 12/01/06  JRB    01     Created.
##########################################################################

# Include common and platform-dependent definitions
include ./platform.mak

# set the library, object, and include file path names
DFLAG  =
DEFS   =
OPT    =
SRCDIR = ../../uca/leant ../../uca/acse ../../uca/goose ../../src
PROJ  = ositpxs$(DFLAG)
OBJDIR = obj_$(PROJ)
LIST  = >> cc.lst 2>&1
VPATH = $(SRCDIR) $(OBJDIR) $(LIBDIR) $(INCDIR)
 
ALL_CFLAGS = $(PLATFORM_CFLAGS) \
	$(OPT) -I$(INCDIR) \
	$(DEFS) -DMMS_LITE -DLEAN_T -DMOSI -DTP0_ENABLED \
	$(CFLAGS)

LIBRARY = $(LIBDIR)/$(PROJ).a

# setup the dependencies for the source files
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
	$(INCDIR)/mvl_acse.h \
	$(INCDIR)/mvl_defs.h \
	$(INCDIR)/mvl_log.h

# These are the objects that belong in the library
OBJECTS = \
	$(OBJDIR)/acse2dec.o \
	$(OBJDIR)/acse2dec2.o \
	$(OBJDIR)/acse2dib.o \
	$(OBJDIR)/acse2dib_arr.o \
	$(OBJDIR)/acse2enc.o \
	$(OBJDIR)/acse2var.o \
	$(OBJDIR)/audt_dec.o \
	$(OBJDIR)/audt_enc.o \
	$(OBJDIR)/checksum.o \
	$(OBJDIR)/clnp_dec.o \
	$(OBJDIR)/clnp_enc.o \
	$(OBJDIR)/clnp_esh.o \
	$(OBJDIR)/clnp_eth.o \
	$(OBJDIR)/clnp_hc.o \
        $(OBJDIR)/clnp_llc.o \
	$(OBJDIR)/clnp_sta.o \
	$(OBJDIR)/clnp_usr.o \
	$(OBJDIR)/clnp_linux.o \
	$(OBJDIR)/copp_dec.o \
	$(OBJDIR)/copp_dec2.o \
	$(OBJDIR)/copp_enc.o \
	$(OBJDIR)/coppdaru.o \
	$(OBJDIR)/coppdcp.o \
	$(OBJDIR)/coppdcpa.o \
	$(OBJDIR)/coppdcpr.o \
	$(OBJDIR)/cosp_dec.o \
	$(OBJDIR)/cosp_enc.o \
	$(OBJDIR)/cospmain.o \
	$(OBJDIR)/ethertyp.o \
	$(OBJDIR)/goose.o \
	$(OBJDIR)/gsei_dec.o \
	$(OBJDIR)/gsei_enc.o \
	$(OBJDIR)/lean_var.o \
	$(OBJDIR)/osicfgx.o \
	$(OBJDIR)/smp_dec.o \
	$(OBJDIR)/smp_slv.o \
	$(OBJDIR)/tp0_dec.o \
	$(OBJDIR)/tp0_socks.o \
	$(OBJDIR)/tp0calld.o \
	$(OBJDIR)/tp0callg.o \
	$(OBJDIR)/tp0main.o \
	$(OBJDIR)/tp4_enc.o \
	$(OBJDIR)/tp4port.o \
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
$(OBJDIR)/acse2dec.o : acse2dec.c  $(INCLUDES)
$(OBJDIR)/acse2dec2.o: acse2dec2.c $(INCLUDES)
$(OBJDIR)/acse2dib.o : acse2dib.c  $(INCLUDES)
$(OBJDIR)/acse2dib_arr.o: acse2dib_arr.c $(INCLUDES)
$(OBJDIR)/acse2enc.o : acse2enc.c  $(INCLUDES)
$(OBJDIR)/acse2var.o : acse2var.c  $(INCLUDES)
$(OBJDIR)/audt_dec.o : audt_dec.c  $(INCLUDES)
$(OBJDIR)/audt_enc.o : audt_enc.c  $(INCLUDES)
$(OBJDIR)/checksum.o : checksum.c  $(INCLUDES)
$(OBJDIR)/clnp_dec.o : clnp_dec.c  $(INCLUDES)
$(OBJDIR)/clnp_enc.o : clnp_enc.c  $(INCLUDES)
$(OBJDIR)/clnp_esh.o : clnp_esh.c  $(INCLUDES)
$(OBJDIR)/clnp_eth.o : clnp_eth.c  $(INCLUDES)
$(OBJDIR)/clnp_hc.o : clnp_hc.c  $(INCLUDES)
$(OBJDIR)/clnp_llc.o : clnp_llc.c  $(INCLUDES)
$(OBJDIR)/clnp_sta.o : clnp_sta.c  $(INCLUDES)
$(OBJDIR)/clnp_usr.o : clnp_usr.c  $(INCLUDES)
$(OBJDIR)/clnp_linux.o : clnp_linux.c  $(INCLUDES)
$(OBJDIR)/copp_dec.o : copp_dec.c  $(INCLUDES)
$(OBJDIR)/copp_dec2.o: copp_dec2.c $(INCLUDES)
$(OBJDIR)/copp_enc.o : copp_enc.c  $(INCLUDES)
$(OBJDIR)/coppdaru.o : coppdaru.c  $(INCLUDES)
$(OBJDIR)/coppdcp.o : coppdcp.c  $(INCLUDES)
$(OBJDIR)/coppdcpa.o : coppdcpa.c $(INCLUDES)
$(OBJDIR)/coppdcpr.o : coppdcpr.c  $(INCLUDES)
$(OBJDIR)/cosp_dec.o : cosp_dec.c  $(INCLUDES)
$(OBJDIR)/cosp_enc.o : cosp_enc.c  $(INCLUDES)
$(OBJDIR)/cospmain.o : cospmain.c  $(INCLUDES)
$(OBJDIR)/ethertyp.o : ethertyp.c  $(INCLUDES)
$(OBJDIR)/goose.o : goose.c  $(INCLUDES)
$(OBJDIR)/gsei_dec.o : gsei_dec.c  $(INCLUDES)
$(OBJDIR)/gsei_enc.o : gsei_enc.c  $(INCLUDES)
$(OBJDIR)/lean_var.o : lean_var.c $(INCLUDES)
$(OBJDIR)/osicfgx.o : osicfgx.c  $(INCLUDES)
$(OBJDIR)/smp_dec.o : smp_dec.c  $(INCLUDES)
$(OBJDIR)/smp_slv.o : smp_slv.c  $(INCLUDES)
$(OBJDIR)/tp0_dec.o : tp0_dec.c  $(INCLUDES)
$(OBJDIR)/tp0_socks.o : tp0_socks.c  $(INCLUDES)
$(OBJDIR)/tp0calld.o : tp0calld.c  $(INCLUDES)
$(OBJDIR)/tp0callg.o : tp0callg.c  $(INCLUDES)
$(OBJDIR)/tp0main.o : tp0main.c  $(INCLUDES)
$(OBJDIR)/tp4_enc.o : tp4_enc.c  $(INCLUDES)
$(OBJDIR)/tp4port.o : tp4port.c  $(INCLUDES)
$(OBJDIR)/tpx_dec.o : tpx_dec.c  $(INCLUDES)
