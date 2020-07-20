# ***********************************************************************
# MODIFICATION LOG :                                                    #
#  Date     Who   Rev                   Comments                        #
# --------  ---  ------   -------------------------------------------   #
# 05/23/05  LWP    02     Ported to bash Posix 
# 09/27/05  EJV    05     Saved in ASCII mode.                          #
# 09/16/05  EJV    04     Pass optionally 801 or 802 through argument.  #
# 02/24/04  DWL    03     Use gzip on both platforms.                   #
# 02/18/04  DWL    02     This script can now build on QNX or Linux.    #
# 05/25/01  GLB    01     Created to build compressed mmslite files     #
# ***********************************************************************

# Assumption:  this script is being run from the mmslite/cmd/gnu directory

cd ../../..

# create archive of mmslite files
tar -cvf ./mmslite$1.tar ./mmslite

# create compressed mmslite files from archived files
gzip ./mmslite$1.tar

