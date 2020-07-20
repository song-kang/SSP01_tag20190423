#DEBUG = y
ifeq ($(DEBUG),y)
      CPPFLAGS += -O -g
else
      CPPFLAGS += -O3
endif



CC := gcc
CPPFLAGS += -fPIC  -shared -I ../include -lpthread 
LINKNAME := libuk5500_Phri.so
MAJOR := 1
MINOR := 6.1
SONAME := ${LINKNAME}.${MAJOR}
VERSION := ${MAJOR}.${MINOR}
OBJS := ${LINKNAME}.${VERSION}

WORKDIR = $(shell pwd)/../..
SOURCE := ukzip_api.c uk_main.c uk_subfun.c 
all:
	${CC} ${CPPFLAGS} -Wl,-soname,${SONAME} -o ${OBJS} ${SOURCE}
clean:
	rm -f *.o *.so.*
#ftp:
#	echo "open 193.100.100.1" > ftp.tmp
#	echo "user root 666666" >> ftp.tmp
#	echo "passive" >> ftp.tmp
#	echo "send ${OBJS} /home/uk/lib/${OBJS}" >> ftp.tmp
#	echo "bye" >> ftp.tmp
#	ftp -i -in < ftp.tmp
#	rm -rf ftp.tmp
