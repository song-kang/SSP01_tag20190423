/****************************************************************************
				
		Copyright 2015 UK Hardware Team  

			     UK Co., Ltd.

		   UK ZIP api library for Linux

File Name:
	ukzip_api.h
Abstract:
	api library header file for start high-speed zip example
Version history
	06/27/2015		Create by zhangmingming

************************************************************************/	
#include <sys/select.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#ifdef OFFICIAL_VERSION
#include "dre_api.h"
#endif

#define UKZIP_IOC_CMD 	   'c'
#define UKZIP_DMA_START    _IO(UKZIP_IOC_CMD, 1)
#define UKZIP_DMA_STOP     _IO(UKZIP_IOC_CMD, 2)
#define UKZIP_CHECK_EVENT  _IO(UKZIP_IOC_CMD, 3)
#define UKZIP_RESET	       _IO(UKZIP_IOC_CMD, 4)

#define BUF_ALIGNMENT 32
#define EVT_TIMEOUT   (1000 * 10)
#define OFFICIAL_VERSION
#undef 	OFFICIAL_VERSION

typedef struct _ukzip_data {
	/* intput buffer */
	void *			inp_buf_addr;
	unsigned int 	inp_buf_len;

	/* output buffer */
	void *			outp_buf_addr;
	unsigned int *	outp_buf_len;

	unsigned int cmd; /* 0:encode, 1:decode */
} ukzip_data;

#ifdef __cplusplus
extern "C"{
#endif
int  UKZip_OpenDev();
int  UKZip_CloseDev(int fd);
int  UKZip_Start(int fd, ukzip_data *user_data);
#ifdef __cplusplus
}
#endif

