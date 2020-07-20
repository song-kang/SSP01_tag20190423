#
#!/bin/bash
##########################################################################
# MODIFICATION LOG :
#  Date     Who  Comments
# --------  ---  -----------------------------------------------
# 05/23/07  LWP  Ported to bash Posix 
# 08/04/05  EJV  Revised to work with platform.mak
# 08/20/04  JRB  Use "X_$1" in "if test" to work w/ NO arg on cmdline
##########################################################################

mkdir obj_ld

# NOTE: ! Make sure that the platform.mak specifies multhithread support
#       on the platform being compiled.

# set path to GNU make
if [ "$1" = "AIX" ]
then
  alias make='/usr/local/bin/make'
fi

export PLATFORM=$1

echo STARTING mem_ld.a LIBRARY  >> cc.lst 2>&1
make -f mem.mak $2       OPT=-g  DFLAG=_ld  DEFS=-DDEBUG_SISCO    # logging    / debug

echo STARTING slog_ld.a LIBRARY  >> cc.lst 2>&1
make -f slog.mak $2      OPT=-g  DFLAG=_ld  DEFS=-DDEBUG_SISCO    # logging    / debug

echo STARTING util_ld.a LIBRARY  >> cc.lst 2>&1
make -f util.mak $2      OPT=-g  DFLAG=_ld  DEFS=-DDEBUG_SISCO    # logging    / debug

echo STARTING gsemtest_ld EXECUTABLE >> cc.lst 2>&1
make -f gsemtest.mak $2  OPT=-g  DFLAG=_ld  DEFS=-DDEBUG_SISCO    # logging    / debug

