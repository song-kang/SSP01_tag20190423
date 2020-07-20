include /usr/src/uk/sbase.mk

SP_COMM_DIR:= /home/SoftwarePlatform/common/sp_common
SSP_DIR    := /home/SoftwarePlatform/SSP01

CC_INCLUDES := $(CC_INCLUDES) -I$(SP_COMM_DIR) -I$(SSP_DIR)/include/ssp_base -I/usr/include/mysql

CC_LINK     := $(CC_LINK) -L$(SSP_DIR)/lib/linux-centos -lssp_base -lsbase_mysql -lsbase_mdb -lsbase_escprint -lmysqlclient
CC_LINK_D   := $(CC_LINK_D) -L$(SSP_DIR)/lib/linux-centos -lssp_based -lsbase_mysqld -lsbase_mdbd -lsbase_escprintd -lmysqlclient
