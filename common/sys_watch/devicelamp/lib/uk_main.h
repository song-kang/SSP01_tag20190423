

#ifndef __UKMAIN_HEADER
#define __UKMAIN_HEADER

#include <fcntl.h>
#include <unistd.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/ioctl.h>
#include <sys/statfs.h>
#include <sys/types.h>
#include <sys/prctl.h>
#include <sys/mman.h>
#include <sys/mount.h>
#include "uklib.h"


//#define HEART 
#define MAX_FRAME_SIZE 	1518
#define MAX_DEVNAME 	255
#define MAX_DEVNUM  	16
#define MAX_DEVNUM_PERDEV (MAX_DEVNUM/2)
#define MAX_DEVNUM_PERDEV_EXT 2
#define MAX_UK_DEVICE   2
#define MAX_DEVNUM_EX   256

#define PKG_SIZE 			2048
#define TSEC_DATA_SIZE		(256*1024)	
#define TSEC_DATABUF_8M		64
#define TSEC_DATABUF_16M	128
#define TSEC_DATABUF_64M    512
#define DMASIZE_8M 			(8*1024*1024)
#define DMASIZE_16M 		(16*1024*1024)
#define DMASIZE_NEWZONE 	(64*1024*1024)



typedef struct _TsecDataStr
{
	unsigned int size;
	char buf[TSEC_DATA_SIZE];
}TsecDataStr;


typedef struct _Tsec_DataBuffer
{
	int  front;            //??芍D赤﹞????㏒?那?3??a??
	int  rear;			   //?車芍D?2????㏒?那?豕??a??
	int  flag;			   //0?a3??車芍D㏒?1?a豕??車芍D
	unsigned int maxSize;
	TsecDataStr *buffer;
}Tsec_DataBuffer;


typedef struct tagdeviceobject{
	int  i;
	long hdevice;
	void * devbuf; 
	unsigned long devbuflen;
	UK_NetCallBack func;
	void * pcontext;
	int timeout;
	pthread_mutex_t devlock;
	long readptr;
	long writeptr;
	unsigned char * databuff;
	Tsec_DataBuffer rx_buf;
	unsigned int dmaSize;
    unsigned long used;
}deviceobject,*pdeviceobject;


extern unsigned char gl_devname[MAX_DEVNAME];
extern deviceobject gl_devlist[MAX_DEVNUM];
extern unsigned long gl_devnum;

int getdevlist(void);

#endif
