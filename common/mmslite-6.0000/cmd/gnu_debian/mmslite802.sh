##########################################################################
# mmslite802.sh                                                          #
# Build file for making MMS-Lite for LINUX, QNX, etc.
# Use the first command line arg ($1) to specify the platform.
# Use the second command line arg ($2) to pass param to make (like clean).
# if $1=AIX     Build for the AIX platform
# if $1=LYNX    Build for the LYNX platform
# if $1=LINUX   Build for the LINUX platform (default if $1 is empty)
# if $1=QNX     Build for the QNX platform
#                                                                        #
# Examples:
#   ./mmslite802.sh LINUX
#   ./mmslite802.sh LINUX clean
#                                                                        #
# NOTE: ositpxs.mak only works on LINUX.
#       However, it should be usable on other platforms if
#       "clnp_linux.c" is replaced with a file ported to each platform.
#                                                                        #
##########################################################################
# MODIFICATION LOG :                                                     #
#  Date     Who   Rev                   Comments                         #
# --------  ---  ------   -----------------------------------------------#
# 02/03/11  JRB		  Del uositcps0, uositpxs0.
# 06/25/10  JRB		  Del obsolete ositp4e library.
# 12/15/09  JRB    18     Del ssec0 library.
#			  Make object directories only if not present.
# 03/17/08  JRB    17     Del old ositcpe, ositpxe libraries
#                         and sreadd, slistend executables.
# 01/18/08  RKR    16     Removed the line that ran mbufcalc
# 05/23/07  LWP    15     Ported to bash Posix
# 03/08/07  JRB    14     Add sositpxs0, uositpxs0.
# 12/01/06  JRB    13     Add ositpxs, smpval, cositpxs0, scl_tpxs0.
# 11/17/06  JRB    12     Add gse_mgmt, gse_mgmt_test
# 11/13/06  JRB    11     Del client, server, uca_srvr sample executables
#                         (they all link to ositcpe lib).
#                         Del "*ositp4e", "*ositpxe" sample executables.
# 08/16/05  RKR    10     Renamed
# 08/15/05  JRB    09     Changed name to make802.sh
# 08/08/05  EJV    08     Added arg $2, and test for correct args
#                         Rem setting AIX compiler mode (see platform.mak)
#                         Export PLATFORM variable once for all makefiles.
# 08/03/05  JRB    07     Add *ositp4e.mak, *ositpxe.mak, iecgoose.mak.
# 08/16/04  JRB    06     Use "X_$1" in "if test" to work w/ NO arg on cmdline
# 08/04/04  EJV    05     Added AIX support.
# 07/19/04  JRB    04     Add scl_srvr.mak.
# 03/15/04  GLB    03     Copy sockets executables to "/usr/bin".
# 11/18/03  JRB    02     Add ositcps_*.a, *ositcps0_ld, ssec0_*.a
#                         Pass PLATFORM=$1 to each make command
#                          to allow use on QNX, etc.
# 12/09/02  CRM    01     Created mkall.sh from QNX script file          #
##########################################################################
##########################################################################
#  MAKE ALL LIBRARIES                                                    #
##########################################################################
# ------------------------------------------------------------------------
#  NOTE: The platform.mak included in every makefile specifies pthreads
#        support if available. There is small performance penalty when
#        multithreading support is enabled.
#        The platform.mak shows how to disable multithreading support.
# ------------------------------------------------------------------------
# check if correct parameters were passed
if [ -z "$1" ]
then
  echo "No command-line arguments. Must specify platform (LINUX, QNX, etc.)"
  exit 0
fi

if [ "$1" = "clean" ]
then
  echo "Must specify platform in first argument (LINUX, QNX, etc.)"
  exit 0
fi
if [ "$1" = "LINUX" -o "$1" = "LYNX" -o "$1" = "QNX" -o "$1" = "AIX" ]
then
  if [ -z "$2" ]
  then
    echo "==============================="
    echo "= Making platform: $1"
    echo "==============================="
  else
    if [ "$2" = "clean" ]
    then
      rm -f cc.lst
      rm -f foundry.tmp
      echo "==============================="
      echo "= Cleaning platform: $1"
      echo "==============================="
    else
      echo "ERROR: invalid second argument: $2"
      exit 0
    fi
  fi
else
  echo "ERROR: unsupported platform: $1"
  exit 0
fi
export PLATFORM=$1


# make directories
if [ ! -d ../../lib_debian ]
then
  mkdir ../../lib_debian
fi
if [ ! -d ../../bin ]
then
  mkdir ../../bin
fi

# Make ALL object directories.
if [ ! -d obj_l ]; then
    mkdir obj_l
fi
if [ ! -d obj_n ]; then
    mkdir obj_n
fi
if [ ! -d obj_ld ]; then
    mkdir obj_ld
fi
if [ ! -d obj_nd ]; then
    mkdir obj_nd
fi

if [ ! -d obj_mvlu_l ]; then
    mkdir obj_mvlu_l
fi
if [ ! -d obj_mvlu_n ]; then
    mkdir obj_mvlu_n
fi
if [ ! -d obj_mvlu_ld ]; then
    mkdir obj_mvlu_ld
fi
if [ ! -d obj_mvlu_nd ]; then
    mkdir obj_mvlu_nd
fi

if [ ! -d obj_ositcps_l ]; then
    mkdir obj_ositcps_l
fi
if [ ! -d obj_ositcps_n ]; then
    mkdir obj_ositcps_n
fi
if [ ! -d obj_ositcps_ld ]; then
    mkdir obj_ositcps_ld
fi
if [ ! -d obj_ositcps_nd ]; then
    mkdir obj_ositcps_nd
fi

if [ ! -d obj_foundry_ld ]; then
    mkdir obj_foundry_ld
fi
if [ ! -d obj_mmslog_ld ]; then
    mkdir obj_mmslog_ld
fi

if [ ! -d obj_cositcps0_ld ]; then
    mkdir obj_cositcps0_ld
fi
if [ ! -d obj_sositcps0_ld ]; then
    mkdir obj_sositcps0_ld
fi
if [ ! -d obj_scl_srvr_ld ]; then
    mkdir obj_scl_srvr_ld
fi

if [ "$1" != "QNX" ]
then
if [ ! -d obj_ositpxs_l ]; then
    mkdir obj_ositpxs_l
fi
if [ ! -d obj_ositpxs_n ]; then
    mkdir obj_ositpxs_n
fi
if [ ! -d obj_ositpxs_ld ]; then
    mkdir obj_ositpxs_ld
fi
if [ ! -d obj_ositpxs_nd ]; then
    mkdir obj_ositpxs_nd
fi

if [ ! -d obj_cositpxs0_ld ]; then
    mkdir obj_cositpxs0_ld
fi
if [ ! -d obj_sositpxs0_ld ]; then
    mkdir obj_sositpxs0_ld
fi
if [ ! -d obj_scl_tpxs0_ld ]; then
    mkdir obj_scl_tpxs0_ld
fi

if [ ! -d obj_iecgoose_ld ]; then
    mkdir obj_iecgoose_ld
fi
if [ ! -d obj_gse_mgmt_ld ]; then
    mkdir obj_gse_mgmt_ld
fi

# end !QNX section
fi

# set path to GNU make
if [ "$1" = "AIX" ]
then
  alias make='/usr/local/bin/make'
fi


echo STARTING MAKE OF meml_l.a LIBRARY for $1  >> cc.lst 2>&1
make -f meml.mak $2             DFLAG=_l   DEFS=-DDEBUG_SISCO   # logging    / no debug
echo STARTING MAKE OF meml_n.a LIBRARY for $1  >> cc.lst 2>&1
make -f meml.mak $2             DFLAG=_n                        # no logging / no debug
echo STARTING MAKE OF meml_ld.a LIBRARY for $1  >> cc.lst 2>&1
make -f meml.mak $2      OPT=-g DFLAG=_ld  DEFS=-DDEBUG_SISCO   # logging    / debug
echo STARTING MAKE OF meml_nd.a LIBRARY for $1  >> cc.lst 2>&1
make -f meml.mak $2      OPT=-g DFLAG=_nd                       # no logging / debug

echo STARTING MAKE OF mem_l.a LIBRARY for $1  >> cc.lst 2>&1
make -f mem.mak $2              DFLAG=_l   DEFS=-DDEBUG_SISCO   # logging    / no debug
echo STARTING MAKE OF mem_n.a LIBRARY for $1  >> cc.lst 2>&1
make -f mem.mak $2              DFLAG=_n                        # no logging / no debug
echo STARTING MAKE OF mem_ld.a LIBRARY for $1  >> cc.lst 2>&1
make -f mem.mak $2       OPT=-g DFLAG=_ld  DEFS=-DDEBUG_SISCO   # logging    / debug
echo STARTING MAKE OF mem_nd.a LIBRARY for $1  >> cc.lst 2>&1
make -f mem.mak $2       OPT=-g DFLAG=_nd                       # no logging / debug

echo STARTING MAKE OF smem_l.a LIBRARY for $1  >> cc.lst 2>&1
make -f smem.mak $2             DFLAG=_l   DEFS=-DDEBUG_SISCO   # logging    / no debug
echo STARTING MAKE OF smem_n.a LIBRARY for $1  >> cc.lst 2>&1
make -f smem.mak $2             DFLAG=_n                        # no logging / no debug
echo STARTING MAKE OF smem_ld.a LIBRARY for $1  >> cc.lst 2>&1
make -f smem.mak $2      OPT=-g DFLAG=_ld  DEFS=-DDEBUG_SISCO   # logging    / debug
echo STARTING MAKE OF smem_nd.a LIBRARY for $1  >> cc.lst 2>&1
make -f smem.mak $2      OPT=-g DFLAG=_nd                       # no logging / debug

echo STARTING MAKE OF slog_l.a LIBRARY for $1  >> cc.lst 2>&1
make -f slog.mak $2             DFLAG=_l   DEFS=-DDEBUG_SISCO   # logging    / no debug
echo STARTING MAKE OF slog_n.a LIBRARY for $1  >> cc.lst 2>&1
make -f slog.mak $2             DFLAG=_n                        # no logging / no debug
echo STARTING MAKE OF slog_ld.a LIBRARY for $1  >> cc.lst 2>&1
make -f slog.mak $2      OPT=-g DFLAG=_ld  DEFS=-DDEBUG_SISCO   # logging    / debug
echo STARTING MAKE OF slog_nd.a LIBRARY for $1  >> cc.lst 2>&1
make -f slog.mak $2      OPT=-g DFLAG=_nd                       # no logging / debug

echo STARTING MAKE OF asn1l_l.a LIBRARY for $1  >> cc.lst 2>&1
make -f asn1l.mak $2            DFLAG=_l   DEFS=-DDEBUG_SISCO   # logging    / no debug
echo STARTING MAKE OF asn1l_n.a LIBRARY for $1  >> cc.lst 2>&1
make -f asn1l.mak $2            DFLAG=_n                        # no logging / no debug
echo STARTING MAKE OF asn1l_ld.a LIBRARY for $1  >> cc.lst 2>&1
make -f asn1l.mak $2     OPT=-g DFLAG=_ld  DEFS=-DDEBUG_SISCO   # logging    / debug
echo STARTING MAKE OF asn1l_nd.a LIBRARY for $1  >> cc.lst 2>&1
make -f asn1l.mak $2     OPT=-g DFLAG=_nd                       # no logging / debug

echo STARTING MAKE OF mmsl_l.a LIBRARY for $1  >> cc.lst 2>&1
make -f mmsl.mak $2             DFLAG=_l   DEFS=-DDEBUG_SISCO   # logging    / no debug
echo STARTING MAKE OF mmsl_n.a LIBRARY for $1  >> cc.lst 2>&1
make -f mmsl.mak $2             DFLAG=_n                        # no logging / no debug
echo STARTING MAKE OF mmsl_ld.a LIBRARY for $1  >> cc.lst 2>&1
make -f mmsl.mak $2      OPT=-g DFLAG=_ld  DEFS=-DDEBUG_SISCO   # logging    / debug
echo STARTING MAKE OF mmsl_nd.a LIBRARY for $1  >> cc.lst 2>&1
make -f mmsl.mak $2      OPT=-g DFLAG=_nd                       # no logging / debug

echo STARTING MAKE OF mmsle_l.a LIBRARY for $1  >> cc.lst 2>&1
make -f mmsle.mak $2            DFLAG=_l   DEFS=-DDEBUG_SISCO   # logging    / no debug
echo STARTING MAKE OF mmsle_n.a LIBRARY for $1  >> cc.lst 2>&1
make -f mmsle.mak $2            DFLAG=_n                        # no logging / no debug
echo STARTING MAKE OF mmsle_ld.a LIBRARY for $1  >> cc.lst 2>&1
make -f mmsle.mak $2     OPT=-g DFLAG=_ld  DEFS=-DDEBUG_SISCO   # logging    / debug
echo STARTING MAKE OF mmsle_nd.a LIBRARY for $1  >> cc.lst 2>&1
make -f mmsle.mak $2     OPT=-g DFLAG=_nd                       # no logging / debug

echo STARTING MAKE OF ositcps_l.a LIBRARY for $1  >> cc.lst 2>&1
make -f ositcps.mak $2          DFLAG=_l   DEFS=-DDEBUG_SISCO   # logging    / no debug
echo STARTING MAKE OF ositcps_n.a LIBRARY for $1  >> cc.lst 2>&1
make -f ositcps.mak $2          DFLAG=_n                        # no logging / no debug
echo STARTING MAKE OF ositcps_ld.a LIBRARY for $1  >> cc.lst 2>&1
make -f ositcps.mak $2   OPT=-g DFLAG=_ld  DEFS=-DDEBUG_SISCO   # logging    / debug
echo STARTING MAKE OF ositcps_nd.a LIBRARY for $1  >> cc.lst 2>&1
make -f ositcps.mak $2   OPT=-g DFLAG=_nd                       # no logging / debug

# CANNOT MAKE THESE ON QNX BECAUSE 'TP4' TRANPORT NOT PORTED
if [ "$1" != "QNX" ]
then

echo STARTING MAKE OF ositpxs_l.a LIBRARY for $1  >> cc.lst 2>&1
make -f ositpxs.mak $2        DFLAG=_l   DEFS=-DDEBUG_SISCO   # logging    / no debug
echo STARTING MAKE OF ositpxs_n.a LIBRARY for $1  >> cc.lst 2>&1
make -f ositpxs.mak $2        DFLAG=_n                        # no logging / no debug
echo STARTING MAKE OF ositpxs_ld.a LIBRARY for $1  >> cc.lst 2>&1
make -f ositpxs.mak $2 OPT=-g DFLAG=_ld  DEFS=-DDEBUG_SISCO   # logging    / debug
echo STARTING MAKE OF ositpxs_nd.a LIBRARY for $1  >> cc.lst 2>&1
make -f ositpxs.mak $2 OPT=-g DFLAG=_nd                       # no logging / debug

echo STARTING MAKE OF gse_mgmt_l.a LIBRARY for $1  >> cc.lst 2>&1
make -f gse_mgmt.mak $2        DFLAG=_l   DEFS=-DDEBUG_SISCO   # logging    / no debug
echo STARTING MAKE OF gse_mgmt_n.a LIBRARY for $1  >> cc.lst 2>&1
make -f gse_mgmt.mak $2        DFLAG=_n                        # no logging / no debug
echo STARTING MAKE OF gse_mgmt_ld.a LIBRARY for $1  >> cc.lst 2>&1
make -f gse_mgmt.mak $2 OPT=-g DFLAG=_ld  DEFS=-DDEBUG_SISCO   # logging    / debug
echo STARTING MAKE OF gse_mgmt_nd.a LIBRARY for $1  >> cc.lst 2>&1
make -f gse_mgmt.mak $2 OPT=-g DFLAG=_nd                       # no logging / debug

echo STARTING MAKE OF smpval_l.a LIBRARY for $1  >> cc.lst 2>&1
make -f smpval.mak $2        DFLAG=_l   DEFS=-DDEBUG_SISCO   # logging    / no debug
echo STARTING MAKE OF smpval_n.a LIBRARY for $1  >> cc.lst 2>&1
make -f smpval.mak $2        DFLAG=_n                        # no logging / no debug
echo STARTING MAKE OF smpval_ld.a LIBRARY for $1  >> cc.lst 2>&1
make -f smpval.mak $2 OPT=-g DFLAG=_ld  DEFS=-DDEBUG_SISCO   # logging    / debug
echo STARTING MAKE OF smpval_nd.a LIBRARY for $1  >> cc.lst 2>&1
make -f smpval.mak $2 OPT=-g DFLAG=_nd                       # no logging / debug

fi

echo STARTING MAKE OF mvl_l.a LIBRARY for $1  >> cc.lst 2>&1
make -f mvl.mak $2              DFLAG=_l   DEFS=-DDEBUG_SISCO   # logging    / no debug
echo STARTING MAKE OF mvl_n.a LIBRARY for $1  >> cc.lst 2>&1
make -f mvl.mak $2              DFLAG=_n                        # no logging / no debug
echo STARTING MAKE OF mvl_ld.a LIBRARY for $1  >> cc.lst 2>&1
make -f mvl.mak $2       OPT=-g DFLAG=_ld  DEFS=-DDEBUG_SISCO   # logging    / debug
echo STARTING MAKE OF mvl_nd.a LIBRARY for $1  >> cc.lst 2>&1
make -f mvl.mak $2       OPT=-g DFLAG=_nd                       # no logging / debug

echo STARTING MAKE OF mvlu_l.a LIBRARY for $1  >> cc.lst 2>&1
make -f mvlu.mak $2             DFLAG=_l   DEFS=-DDEBUG_SISCO   # logging    / no debug
echo STARTING MAKE OF mvlu_n.a LIBRARY for $1  >> cc.lst 2>&1
make -f mvlu.mak $2             DFLAG=_n                        # no logging / no debug
echo STARTING MAKE OF mvlu_ld.a LIBRARY for $1  >> cc.lst 2>&1
make -f mvlu.mak $2      OPT=-g DFLAG=_ld  DEFS=-DDEBUG_SISCO   # logging    / debug
echo STARTING MAKE OF mvlu_nd.a LIBRARY for $1  >> cc.lst 2>&1
make -f mvlu.mak $2      OPT=-g DFLAG=_nd                       # no logging / debug

echo STARTING MAKE OF util_l.a LIBRARY for $1  >> cc.lst 2>&1
make -f util.mak $2             DFLAG=_l   DEFS=-DDEBUG_SISCO   # logging    / no debug
echo STARTING MAKE OF util_n.a LIBRARY for $1  >> cc.lst 2>&1
make -f util.mak $2             DFLAG=_n                        # no logging / no debug
echo STARTING MAKE OF util_ld.a LIBRARY for $1  >> cc.lst 2>&1
make -f util.mak $2      OPT=-g DFLAG=_ld  DEFS=-DDEBUG_SISCO   # logging    / debug
echo STARTING MAKE OF util_nd.a LIBRARY for $1  >> cc.lst 2>&1
make -f util.mak $2      OPT=-g DFLAG=_nd                       # no logging / debug

echo STARTING MAKE OF mlogl_l.a LIBRARY for $1  >> cc.lst 2>&1
make -f mlogl.mak $2            DFLAG=_l   DEFS=-DDEBUG_SISCO   # logging    / no debug
echo STARTING MAKE OF mlogl_n.a LIBRARY for $1  >> cc.lst 2>&1
make -f mlogl.mak $2            DFLAG=_n                        # no logging / no debug
echo STARTING MAKE OF mlogl_ld.a LIBRARY for $1  >> cc.lst 2>&1
make -f mlogl.mak $2     OPT=-g DFLAG=_ld  DEFS=-DDEBUG_SISCO   # logging    / debug
echo STARTING MAKE OF mlogl_nd.a LIBRARY for $1  >> cc.lst 2>&1
make -f mlogl.mak $2     OPT=-g DFLAG=_nd                       # no logging / debug

echo STARTING MAKE OF findalgn_ld EXECUTABLE for $1 >> cc.lst 2>&1
make -f findalgn.mak $2  OPT=-g DFLAG=_ld  DEFS=-DDEBUG_SISCO   # logging    / debug

echo STARTING MAKE OF foundry_ld EXECUTABLE for $1 >> cc.lst 2>&1
make -f foundry.mak $2   OPT=-g DFLAG=_ld  DEFS=-DDEBUG_SISCO   # logging    / debug

echo STARTING MAKE OF mmslog_ld EXECUTABLE for $1 >> cc.lst 2>&1
make -f mmslog.mak $2    OPT=-g DFLAG=_ld  DEFS=-DDEBUG_SISCO   # logging    / debug

#
# The following executables link to the "ositcps" library.
#
echo STARTING MAKE OF cositcps0_ld EXECUTABLE for $1 >> cc.lst 2>&1
make -f cositcps0.mak $2    OPT=-g DFLAG=_ld  DEFS=-DDEBUG_SISCO   # logging    / debug

echo STARTING MAKE OF sositcps0_ld EXECUTABLE for $1 >> cc.lst 2>&1
make -f sositcps0.mak $2    OPT=-g DFLAG=_ld  DEFS=-DDEBUG_SISCO   # logging    / debug


echo STARTING MAKE OF scl_srvr_ld EXECUTABLE for $1 >> cc.lst 2>&1
make -f scl_srvr.mak $2     OPT=-g DFLAG=_ld  DEFS=-DDEBUG_SISCO   # logging    / debug

# CANNOT MAKE THESE ON QNX BECAUSE 'TP4' TRANPORT NOT PORTED
if [ "$1" != "QNX" ]
then

#
# The following executables link to the "ositpxs" library.
#
echo STARTING MAKE OF cositpxs0_ld EXECUTABLE for $1 >> cc.lst 2>&1
make -f cositpxs0.mak $2    OPT=-g DFLAG=_ld  DEFS=-DDEBUG_SISCO   # logging    / debug

echo STARTING MAKE OF sositpxs0_ld EXECUTABLE for $1 >> cc.lst 2>&1
make -f sositpxs0.mak $2    OPT=-g DFLAG=_ld  DEFS=-DDEBUG_SISCO   # logging    / debug

echo STARTING MAKE OF scl_tpxs0_ld EXECUTABLE for $1 >> cc.lst 2>&1
make -f scl_tpxs0.mak $2     OPT=-g DFLAG=_ld  DEFS=-DDEBUG_SISCO   # logging    / debug

#
# The following executable tests the IEC-61850 GOOSE Framework.
#
echo STARTING MAKE OF iecgoose_ld EXECUTABLE for $1 >> cc.lst 2>&1
make -f iecgoose.mak $2  OPT=-g DFLAG=_ld  DEFS=-DDEBUG_SISCO   # logging    / debug

#
# The following executable tests the IEC 61850 GSE Management.
#
echo STARTING MAKE OF gse_mgmt_ld EXECUTABLE for $1 >> cc.lst 2>&1
make -f gse_mgmt_test.mak $2  OPT=-g DFLAG=_ld  DEFS=-DDEBUG_SISCO   # logging    / debug

fi
