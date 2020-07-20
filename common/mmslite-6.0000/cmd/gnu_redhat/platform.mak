#########################################################################
# SISCO SOFTWARE MODULE HEADER ##########################################
#########################################################################
#			       					        #
# MODULE NAME : platform.mak                       			#
# PRODUCT(S)  : MMS-LITE, MMS-LITE-SECURE				#
#		and other products.					#
#			       					        #
# MODULE DESCRIPTION :        					        #
#      This makefile is included by all makefiles to provide platform	#
#      dependent defines.						#
#			       						#
#									#
# MODIFICATION LOG :	       						#
#  Date     Who   Rev			Comments			#
# --------  ---  ------   -------------------------------------------	#
# 05/09/11  EJV           SUN:chg to -xtarget=generic and -m32 or -m64
#                            (after update to Solaris Studio 12).
# 04/20/11  JRB		  LINUX, WIN32: del -fsigned-char (unneeded)
# 04/06/11  EJV           SUN: added check for HARDWARE.
# 04/01/11  EJV           SUN: changed _XOPEN_SOURCE=500 to 600.
#                              Added __EXTENSIONS__ for strcasecmp.
# 03/01/11  EJV           SUN: moved -g option to mk*.sh scripts.
#                           Added -xmemalign=1s add -mt options.
#                           Added $(CC_MODE) for 64-bit.
#                         LIBDIR: added $(CC_MODE) for 64-bit compile.
# 02/23/11  EJV           Split conditional statements to compile on Sun
#			  with gmake v3.80, preset PLATFORM_CFLAGS to error.
# 12/15/10  EJV           Added INCDIR_OPENSSL, LIBDIR_OPENSSL. 
#                         Added SUN platform. Moved lines. Chg few comments.
# 02/03/10  JRB    05     Add INCDIR, LIBDIR (deleted from other makefiles).
#			  Use "else ifeq" and if PLATFORM is not
#			  supported, use $(error ....).
#			  Add -m32 on LINUX.
# 09/16/08  LWP	   04	  Ported to QNX Neutrino			#
# 02/16/07  RKR    03     Added -lrt for STDLIB				#
# 09/20/05  EJV    02     Added comment for QNX flags			#
# 08/04/05  EJV    01     Extracted from MMS-LITE makefiles & added	#
#			  multithread support.				#
#########################################################################

# Use these directories for ALL platforms
INCDIR=../../inc
LIBDIR=../../lib_redhat$(CC_MODE)

# set this define to detect missing PLATFORM
PLATFORM_CFLAGS = $(error Unsupported PLATFORM=$(PLATFORM))

# ------------------------------------------------------------------------
#  NOTE: If available, the platform.mak included in every makefile
#        specifies pthreads support. There is small performance penalty
#        when multithread support is enabled.
#        See each platform for note how to disable the multithread support.
#
#  The MMS-LITE-SECURE needs to be compiled with multithread support!
# ------------------------------------------------------------------------


# ------------------------------------------------------------------------
# default settings for LINUX
# ------------------------------------------------------------------------

ifeq "$(PLATFORM)" "LINUX"

  # - - - - - - - - - - - - - - - - - - - - -
  # Description of GNU compiler options used:
  # - - - - - - - - - - - - - - - - - - - - -
  #  -g     Produce symbolic info in the object file for debugger
  #  -O0    Generate non-optimized code sequences
  #  -wall                 Warning level
  #  -D_GNU_SOURCE         Enable extra prototypes in "pthread.h".
  #  -pthread              Compile for multithreaded programs, equivalent
  #                          to -D_REENTRANT and -lpthread
  #  -D_THREAD_SAFE	 Compile for multithreaded programs
  #   Note: on Suse LINUX the _THREAD_SAFE or _REENTRANT is not being defined
  #       when -pthread option is present. To compile glbsem.h properly we
  #       need to define one of these defines explicitly here.
  #  -m32   This flag compiles 32-bit software, even on
  #         64-bit Linux Kernel.

# set compiler
PLATFORM_CFLAGS = -O0 -Wall -D_GNU_SOURCE -D_THREAD_SAFE -pthread -m64 -fPIC -D__x86_64__ -D__LP64__
# set linker
# on LINUX g++ linker is needed to link C++ libraries
PLATFORM_LINKER  = $(CC)
PLATFORM_LDFLAGS =
PLATFORM_LIBS    = -lpthread -lrt

# set archive
ARFLAGS = -r

# set MMS-LITE
ALIGNDIR = ../../mvl/util/foundry/linux

# path to OpenSSL
INCDIR_OPENSSL = /usr/include
LIBDIR_OPENSSL = /lib

# NOTE: To disable multithread support: remove -D_THREAD_SAFE -pthread from
#       PLATFORM_CFLAGS and -lpthread from PLATFORM_LIBS.

endif

# ------------------------------------------------------------------------
# settings for QNX
# ------------------------------------------------------------------------

ifeq "$(PLATFORM)" "QNX"

  # - - - - - - - - - - - - - - - - - - - - -
  # Description of QNX compiler options used:
  # - - - - - - - - - - - - - - - - - - - - -
  #
  #  -c     Suppress the link phase and retain object files
  #  -g     Produce symbolic info in the object file for debugger
  #  -Od    Generate non-optimized code sequences
  #  -Wc,-s No stack overflow checking (Watcom compiler)
  #  -w3    Warning level
  #  -j     Change char default from unsigned to signed

PLATFORM_CFLAGS = -j -w3 -Od

PLATFORM_LINKER  = $(CC)
PLATFORM_LDFLAGS =
PLATFORM_LIBS    =

ARFLAGS = -n -q

ALIGNDIR = ../../mvl/util/foundry/qnx

endif

# ------------------------------------------------------------------------
# settings for QNX Neutrino
# ------------------------------------------------------------------------

ifeq "$(PLATFORM)" "QNXNTO"

  # - - - - - - - - - - - - - - - - - - - - -
  # Description of QNX compiler options used:
  # - - - - - - - - - - - - - - - - - - - - -
  #
  #  -c     Suppress the link phase and retain object files
  #  -g     Produce symbolic info in the object file for debugger
  #  -Od    Generate non-optimized code sequences
  #  -Wc,-s No stack overflow checking (Watcom compiler)
  #  -w3    Warning level
  
# PLATFORM_CFLAGS = -w9 -l socket -o0 -fsigned-char 
PLATFORM_CFLAGS = -O0 -Wall -fsigned-char -D_GNU_SOURCE -D_THREAD_SAFE -l socket
PLATFORM_LINKER  = $(CC)
PLATFORM_LDFLAGS =
PLATFORM_LIBS    = 

ARFLAGS = -r

ALIGNDIR = ../../mvl/util/foundry/qnxnto

endif

# ------------------------------------------------------------------------
# settings for LYNX
# ------------------------------------------------------------------------

ifeq "$(PLATFORM)" "LYNX"

PLATFORM_CFLAGS = -O0 -Wall -fsigned-char -D__LYNX

PLATFORM_LINKER  = $(CC)
PLATFORM_LDFLAGS =
PLATFORM_LIBS    = -lbsd

ARFLAGS = -r

ALIGNDIR = ../../mvl/util/foundry/lynx

endif

# ------------------------------------------------------------------------
# settings for SUN Solaris
# ------------------------------------------------------------------------

ifeq "$(PLATFORM)" "SUN"

# Description of Sun compiler options used:
# -Xa               Compiler mode (ISO C plus K&R C compatibility extensions).
# -xchar=signed     Default char type is signed.
# -xO3 -g           includes symbolic information with almost full optimization
# -xtarget=generic  get best performance for generic architecture, chipset, and cache
#                   (equivalent to -xarch=generic -xchip=generic -xcache=generic)
#		    Produce 32-bit object binaries for good performance on most systems.
# -errtags=yes      Display the tag for a warning message
# _XOPEN_SOURCE=600 Define for POSIX threads and for domain socket support (min XPG4v2).
# __EXTENSIONS__    Extended system interfaces are explicitly requested.
# -xmemalign=1i     When using #pragma pack on a SPARC platform the -xmemalign option must
#                   be specified for the compilation and the linking of the application.
#                   The '1i' means that if non-aligned memory access is detected the
#                   a handler is invoked to properly access the momory.
# -mt               Multithread support, expands to -D_REENTRANT -lthread (Sun Studio 11).
# -m32 or -m64      This flag compiles 32-bit or 64-bit software.

# set compiler options
ifeq "$(CC_MODE)" "64"
  ifeq "$(HARDWARE)" "sparc"
    PLATFORM_CFLAGS  = -xtarget=generic -m64 -mt -errtags=yes -D_XOPEN_SOURCE=600 -D__EXTENSIONS__ -xmemalign=1i
  else
    # Sun Solaris x86
    PLATFORM_CFLAGS  = -xtarget=generic -m64 -mt -errtags=yes -D_XOPEN_SOURCE=600 -D__EXTENSIONS__
  endif
else
  # 32-bit compilation
  ifeq "$(HARDWARE)" "sparc"
    PLATFORM_CFLAGS  = -xtarget=generic -m32 -mt -errtags=yes -D_XOPEN_SOURCE=600 -D__EXTENSIONS__ -xmemalign=1i
  else
    # Sun Solaris x86
    PLATFORM_CFLAGS  = -xtarget=generic -m32 -mt -errtags=yes -D_XOPEN_SOURCE=600 -D__EXTENSIONS__
  endif
endif

# set linker
PLATFORM_LINKER  = $(CC)
PLATFORM_LDFLAGS =
PLATFORM_LIBS    = -lpthread -lrt -lxnet -lsocket -lnsl -lm

# set archive
AR = /usr/ccs/bin/ar
ARFLAGS = -r

# set MMS-LITE
ALIGNDIR = ../../mvl/util/foundry/sun

# path to OpenSSL
INCDIR_OPENSSL = /usr/sfw/include
LIBDIR_OPENSSL = /usr/sfw/lib/$(CC_MODE)

endif

# ------------------------------------------------------------------------
# settings for AIX
# ------------------------------------------------------------------------

ifeq "$(PLATFORM)" "AIX"

  #- - - - - - - - - - - - - - - - - - - - - -  - - - - - - - - - - - - - - - -
  # Compiler mode xlC_r:
  # "Invokes the compiler so that source files are compiled as C++ language
  # source code. Files with .c suffixes, assuming you have not used
  # the -+ compiler option, are compiled as C language source code with a
  # default language level of ansi, and compiler option -qansialias to allow
  # type-based aliasing. All _r-suffixed invocations additionally set the macro
  # names -D_THREAD_SAFE and add the libraries -L/usr/lib/threads,
  #  -lc_r and -lpthreads. The compiler option -qthreaded is also added."
  #- - - - - - - - - - - - - - - - - - - - - -  - - - - - - - - - - - - - - - -
CC=xlC_r

# set compiler
PLATFORM_CFLAGS = -qchars=signed -flag=w:e -qcpluscmt -qmaxmem=16384 -qOPT=0

# set linker
PLATFORM_LINKER  = xlC_r
PLATFORM_LDFLAGS =
PLATFORM_LIBS    = -lpthreads -lm

ARFLAGS = -r

ALIGNDIR = ../../mvl/util/foundry/aix

# NOTE: To disable multithread support: change xlC_r to xlC and remove -lpthread -lm
#       from PLATFORM_LIBS.

endif

# ------------------------------------------------------------------------
# settings for WIN32 (with GNU compiler)
# ------------------------------------------------------------------------

ifeq "$(PLATFORM)" "WIN32"

PLATFORM_CFLAGS = -m32 -O0 -Wall -D_GNU_SOURCE -D_THREAD_SAFE
ARFLAGS = -r
ALIGNDIR = ../../mvl/util/foundry/win32

endif

# ------------------------------------------------------------------------
# add other platforms here as needed using (ifeq "$(PLATFORM)" "....")
# ------------------------------------------------------------------------

