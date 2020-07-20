##########################################################################
# SISCO SOFTWARE MODULE HEADER ###########################################
##########################################################################
#			       						 #
# MODULE NAME : util.mak                       			 	 #
# PRODUCT(S)  : MMS-EASE-Lite						 #
#			       						 #
# MODULE DESCRIPTION :        						 #
# This makefile is used by the Linux 'make' utility to build the Memory	 #
# allocation library for MMS-EASE-Lite.  All paths including header 	 #
# files and object modules are relative to the current project directory #
# which makes this file as environment independent as possible.  Both    #
# the DEBUG and Non DEBUG versions of the Utilities are made with this   #
# script								 #
#									 #
# To make the UTIL_L.A:							 #
# 	make -f util.mak     	    DFLAG=_l   DEFS=-DDEBUG_SISCO	 #
# To make the UTIL_N.A:							 #
# 	make -f util.mak     	    DFLAG=_n				 #
# To make the UTIL_LD.A:						 #
# 	make -f util.mak     OPT=-g DFLAG=_ld  DEFS=-DDEBUG_SISCO	 #
# To make the UTIL_ND.A:						 #
# 	make -f util.mak     OPT=-g DFLAG=_nd				 #
#			       						 #
# MODIFICATION LOG :	       						 #
#  Date     Who   Rev			Comments			 #
# --------  ---  ------   -------------------------------------------	 #
# 02/03/12  JRB		  Add sclparse_serv.c
# 02/03/10  JRB    16     Del INCDIR, LIBDIR (now defined in platform.mak).
# 12/21/09  JRB    15     Added ipc_utl.c, crc_util.c
# 11/07/08  RKR    14     Added sseal.c
# 03/07/08  JRB    13     Del sfile.c, stdtime_iccp_timestamp.c 
# 10/05/07  JRB    12     Add gensock2.c so any app can use it.
# 02/15/07  RKR    11     Added STDTIME
# 08/04/05  EJV    10     Moved platform defs to platform.mak.
#			  Revised link & clean. Added -c in compile rule.
# 05/23/05  JRB    09     Add ukey.c and UTIL_LIB.
# 04/28/05  GLB    08     Removed "mms_mv.h" from INCLUDES.
# 04/28/05  GLB    07     Removed "mms_defs.h" from INCLUDES.
# 02/23/05  EJV    06     Updated INCLUDES.
# 08/04/04  EJV    05     Add AIX OS support.
# 07/19/04  JRB    04     Add scl*.c
# 12/10/03  JRB    03     Add LYNX OS support. Del "no-stack-check" option.
# 10/21/03  JRB    02     Chg CFLAGS & LDFLAGS to be "user-settable",
#			   use $(AR), as recommended by GNU Make.
#			  Add "$(OBJDIR)/" prefix before all objects & "%.o",
#			   del "$(OBJDIR)/" prefix before "$@",
#			   then use $(OBJECTS) in $(AR) cmd.
#			  Add $(PLATFORM) check to allow QNX build too.
#			  Use ARFLAGS (for QNX = -n -q, default = -r).
#			  Add glbsem_unix.c. Add -D_GNU_SOURCE
#			   to enable extra prototypes in "pthread.h".
#			  Add more files to INCLUDES.
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
PROJ  = util$(DFLAG)
LIST  = >> cc.lst 2>&1
VPATH = $(SRCDIR) $(OBJDIR) $(LIBDIR) $(INCDIR)

ALL_CFLAGS = $(PLATFORM_CFLAGS) \
	$(OPT) -I$(INCDIR) \
	$(DEFS) -DMMS_LITE -DUTIL_LIB \
	$(CFLAGS)

LIBRARY = $(LIBDIR)/$(PROJ).a

# setup the dependancies for the source files
INCLUDES = \
	$(INCDIR)/asn1defs.h \
	$(INCDIR)/asn1r.h    \
	$(INCDIR)/cfg_util.h \
	$(INCDIR)/cfglog.h   \
	$(INCDIR)/gen_list.h \
	$(INCDIR)/gensock2.h \
	$(INCDIR)/glberror.h \
	$(INCDIR)/glbopt.h   \
	$(INCDIR)/glbsem.h   \
	$(INCDIR)/glbtypes.h \
	$(INCDIR)/mem_chk.h  \
	$(INCDIR)/mms_def2.h \
	$(INCDIR)/mms_mp.h   \
	$(INCDIR)/mms_pvar.h \
	$(INCDIR)/mms_vvar.h \
	$(INCDIR)/slog.h     \
	$(INCDIR)/sock_log.h \
	$(INCDIR)/stime.h    \
	$(INCDIR)/str_util.h \
	$(INCDIR)/sx_arb.h   \
	$(INCDIR)/sx_defs.h  \
	$(INCDIR)/sx_log.h   \
	$(INCDIR)/sysincs.h  \
	$(INCDIR)/time_str.h

# These are the objects that belong in the library
ifeq "$(PLATFORM)" "QNX"
# QNX can't handle glbsem*.c
OBJECTS = \
	$(OBJDIR)/genlists.o \
	$(OBJDIR)/gensock2.o \
	$(OBJDIR)/cfg_util.o \
	$(OBJDIR)/scl_log.o \
	$(OBJDIR)/sclparse.o \
	$(OBJDIR)/sclparse_serv.o \
	$(OBJDIR)/sclstore.o \
	$(OBJDIR)/stime.o \
	$(OBJDIR)/sseal.o \
	$(OBJDIR)/str_util.o \
	$(OBJDIR)/sx_dec.o \
	$(OBJDIR)/sx_enc.o \
	$(OBJDIR)/sx_data.o \
	$(OBJDIR)/sx_axs4d.o \
	$(OBJDIR)/stdtime.o \
	$(OBJDIR)/stdtime_mms_btime.o \
	$(OBJDIR)/stdtime_mms_utctime.o \
	$(OBJDIR)/stdtime_quadlib.o \
	$(OBJDIR)/stdtime_w32.o \
	$(OBJDIR)/time_str.o
else
OBJECTS = \
	$(OBJDIR)/cfg_util.o \
	$(OBJDIR)/crc_util.o \
	$(OBJDIR)/genlists.o \
	$(OBJDIR)/gensock2.o \
	$(OBJDIR)/glbsem.o \
	$(OBJDIR)/glbsem_unix.o \
	$(OBJDIR)/ipc_utl.o \
	$(OBJDIR)/scl_log.o \
	$(OBJDIR)/sclparse.o \
	$(OBJDIR)/sclparse_serv.o \
	$(OBJDIR)/sclstore.o \
	$(OBJDIR)/stime.o \
	$(OBJDIR)/sseal.o \
	$(OBJDIR)/str_util.o \
	$(OBJDIR)/sx_dec.o \
	$(OBJDIR)/sx_enc.o \
	$(OBJDIR)/sx_data.o \
	$(OBJDIR)/sx_axs4d.o \
	$(OBJDIR)/stdtime.o \
	$(OBJDIR)/stdtime_mms_btime.o \
	$(OBJDIR)/stdtime_mms_utctime.o \
	$(OBJDIR)/stdtime_quadlib.o \
	$(OBJDIR)/stdtime_w32.o \
	$(OBJDIR)/time_str.o \
	$(OBJDIR)/ukey.o
endif
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
$(OBJDIR)/cfg_util.o : cfg_util.c $(INCLUDES)
$(OBJDIR)/crc_util.o : crc_util.c $(INCLUDES)
$(OBJDIR)/genlists.o : genlists.c $(INCLUDES)
$(OBJDIR)/gensock2.o : gensock2.c $(INCLUDES)
$(OBJDIR)/ipc_utl.o : ipc_utl.c $(INCLUDES)
$(OBJDIR)/scl_log.o : scl_log.c $(INCLUDES)
$(OBJDIR)/sclparse.o : sclparse.c $(INCLUDES)
$(OBJDIR)/sclparse_serv.o : sclparse_serv.c $(INCLUDES)
$(OBJDIR)/sclstore.o : sclstore.c $(INCLUDES)
$(OBJDIR)/stime.o : stime.c $(INCLUDES)
$(OBJDIR)/str_util.o : str_util.c $(INCLUDES)
$(OBJDIR)/glbsem.o : glbsem.c $(INCLUDES)
$(OBJDIR)/sseal.o : sseal.c $(INCLUDES)
$(OBJDIR)/glbsem_unix.o : glbsem_unix.c $(INCLUDES)
$(OBJDIR)/sx_dec.o : sx_dec.c $(INCLUDES)
$(OBJDIR)/sx_enc.o : sx_enc.c $(INCLUDES)
$(OBJDIR)/sx_data.o : sx_data.c $(INCLUDES)
$(OBJDIR)/sx_axs4d.o : sx_axs4d.c $(INCLUDES)
$(OBJDIR)/stdtime.o : stdtime.c $(INCLUDES)
$(OBJDIR)/stdtime_mms_btime.o : stdtime_mms_btime.c $(INCLUDES)
$(OBJDIR)/stdtime_mms_utctime.o : stdtime_mms_utctime.c $(INCLUDES)
$(OBJDIR)/stdtime_quadlib.o : stdtime_quadlib.c $(INCLUDES)
$(OBJDIR)/stdtime_w32.o : stdtime_w32.c $(INCLUDES)
$(OBJDIR)/time_str.o : time_str.c $(INCLUDES)
$(OBJDIR)/ukey.o : ukey.c $(INCLUDES)
