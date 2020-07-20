include /usr/src/uk/ssp_base.mk

CC_LINK     := $(CC_LINK)
CC_LINK_D   := $(CC_LINK_D)

CC_TARGET   = sys_ntp
CC_TARGET_D = sys_ntpd
INSTALL_PATH= ../../../lib/linux

SRCS =  *.cpp
OBJS =  *.o

all:
		$(CC) -c $(SRCS) $(CC_INCLUDES) -I../../../include/ssp_base -I/mnt/SoftwarePlateform/sbase/include/db/oracle -I/mnt/SoftwarePlateform/sbase/include/db/oracle/oci/include
		$(CC) -o $(CC_TARGET) -g $(OBJS) $(CC_LINK) -lsbase_oracle -lclntsh -lclntshcore -lnnz12 -locci -lociei -locijdbc12 -lons -loramysql12 
		$(RM) *.o
		$(CP) $(CC_TARGET) $(INSTALL_PATH)/
debug:
		$(CC) -c $(SRCS) $(CC_INCLUDES) -I../../../include/ssp_base -I/mnt/SoftwarePlateform/sbase/include/db/oracle -I/mnt/SoftwarePlateform/sbase/include/db/oracle/oci/include
		$(CC) -o $(CC_TARGET_D) -g $(OBJS) $(CC_LINK_D) -lsbase_oracled -lclntsh -lclntshcore -lnnz12 -locci -lociei -locijdbc12 -lons -loramysql12 
		$(RM) *.o
		$(CP) $(CC_TARGET_D) $(INSTALL_PATH)/

clean:
		$(RM) *.o
		$(RM) *.d
		$(RM) $(CC_TARGET)

install:
		$(CP) $(CC_TARGET) $(INSTALL_PATH)/$(CC_TARGET)
