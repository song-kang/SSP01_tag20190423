include /usr/src/uk/ssp_base.mk

CC_TARGET   = libssnmp.so
CC_TARGET_D   = libssnmpd.so
INSTALL_PATH= ../../lib/linux
CC_INCLUDES := $(CC_INCLUDES) -I./ -Iinclude/ -I/home/afang/skt/openssl-1.0.1l/include -D_USE_OPENSSL -D_LIB -DHAVE_CONFIG_H -D_THREADS

RM = rm -rf
LN = ln
CP = cp -f

SRCS =  *.cpp src/*.cpp
OBJS =  *.o 

all:
		$(RM) debug
		$(CC) -c $(SRCS) $(CC_INCLUDES) $(CC_COMPLIE)
		$(CC) -shared $(OBJS) -o $(CC_TARGET)
		$(RM) *.o
		$(CP) $(CC_TARGET) $(INSTALL_PATH)/

debug:
		$(RM) debug
		$(CC) -c $(SRCS) $(CC_INCLUDES) $(CC_COMPLIE_D)
		$(CC) -shared $(OBJS) -o $(CC_TARGET_D)
		$(RM) *.o
		$(CP) $(CC_TARGET_D) $(INSTALL_PATH)/


clean:
		$(RM) *.o
		$(RM) *.d

install:
		$(CP) $(CC_TARGET) $(INSTALL_PATH)/$(CC_TARGET)
		$(CP) $(CC_TARGETD) $(INSTALL_PATH)/$(CC_TARGET)
