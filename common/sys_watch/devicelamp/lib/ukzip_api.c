/****************************************************************************
				
		Copyright 2015 UK Hardware Team

			       UK Co., Ltd.

		   UK ZIP api library for Linux

File Name:
	ukzip_api.c
Abstract:
	api library for start high-speed zip/unzip example
Version history
	01/25/2015		Create by zhangmingming

************************************************************************/	
#include "ukzip_api.h"

#ifndef OFFICIAL_VERSION
static int check_event(int fd, int timeout, void *event)
{
	struct timeval tv_val;
	fd_set evt_set;
	int ret;

	memset(&tv_val, 0, sizeof(tv_val));
	do{
		tv_val.tv_sec = (signed long) (timeout / 1000);
		tv_val.tv_usec = (signed long) (timeout % 1000 * 1000);
		FD_ZERO(&evt_set);
		FD_SET(fd, &evt_set);

		ret = select(fd + 1, &evt_set, NULL, NULL, &tv_val);
		if (ret == 0) {
			*((int *) event) = 0;
			return 0;
		} else if (ret < 0&&errno!=EINTR) {
			return -1;
		}
	}while(ret==-1&&errno==EINTR);

	ret = ioctl(fd, UKZIP_CHECK_EVENT, event);
	if (ret < 0) {
		return -1;
	}
	return 1;
}
#endif



 int UKZip_OpenDev()
{
#ifdef OFFICIAL_VERSION
	if (DRE_IS_RESULT_ERR(DRE_apiSysInit())) {
		printf("initialization failed.\n");
		return -1;
	}
	return 0;
#else
	char szbuf[256];
	FILE *fp;
	int major;
	int fd;
	char * ptmp;

	fd = open("/dev/e8021ib", O_RDWR);
	if (fd < 0) {
		fp = popen("awk '$2 == \"e8021ib\" {print $1}' /proc/devices", "r");
		if (!fp) {
			return -1;
		}
		memset(szbuf, 0 ,sizeof(szbuf));
		ptmp = fgets(szbuf,256,fp);
		pclose(fp);

		major = atoi(szbuf);
		if (!major) {
			return -1;
		}

		fp = popen("find /dev/ -name \"e8021ib\"", "r");
		if (!fp) {
			return -1;
		}
		memset(szbuf, 0, sizeof(szbuf));
		ptmp = fgets(szbuf, 256, fp);
		pclose(fp);
		
		if (!strstr(szbuf, "/dev/e8021ib")) {
			sprintf(szbuf,"mknod /dev/e8021ib c %d 0", major);
			fd = system(szbuf);
		}

		fd = open("/dev/e8021ib", O_RDWR);
		if (fd < 0) {
			return -1;
		}
	}
	return fd;
#endif
}

int UKZip_CloseDev(int fd)
{
#ifdef OFFICIAL_VERSION
    	DRE_apiSysExit();
	return 0;
#else
	int ret;

	ret = close(fd);
	if (ret < 0) {
		return ret;
	}
	return 0;
#endif
}

int UKZip_Start(int fd, ukzip_data *user_data)
{
#ifdef OFFICIAL_VERSION
    	DRE_dataDesc *srcBuf = NULL;
	DRE_dataDesc *dstBuf = NULL;
        DRE_rawSessHandle sessID = 0;
        DRE_rawSessCompParam cmpParam = {0}; 
    	DRE_compAlgoParam cmpAlgParam = {{0}}; 
    	DRE_rawSyncOpData opData = {{0}};
    	DRE_status retCode = DRE_OK;

	if (!user_data) {
		return -1;
	}

	/* open session */
	memset(&cmpAlgParam, 0, sizeof(DRE_compAlgoParam));
	cmpAlgParam.gzip.mode = DRE_GZIP_MODE_STATIC;    
	memset(&cmpParam, 0, sizeof(cmpParam));
	cmpParam.compAlgo = DRE_GZIP;
	cmpParam.compAlgoParam = &cmpAlgParam;
	cmpParam.stateful = DRE_FALSE;

	retCode = DRE_rawSessOpen(&sessID, 
                          	  0, 
                              	  !user_data->cmd,
                                  0,
                              	  NULL,
                              	  &cmpParam,
                              	  NULL,
                              	  NULL,
                              	  NULL);
	if (DRE_IS_RESULT_ERR(retCode)) {
		printf("Failed to open session, err code:%x\n", retCode);
		return -1;          
	}

    	/* prepare data */
    	/* source data */
	srcBuf = malloc(sizeof(DRE_dataDesc));
	if (srcBuf == NULL) {
		printf("Failed to alloc memory, %d.\n", __LINE__);
        	goto out;
	}
	memset(srcBuf, 0, sizeof(DRE_dataDesc));
    
	srcBuf->ptr = user_data->inp_buf_addr;
    	if (srcBuf->ptr == NULL) {
        	printf("Failed to alloc memory, %d.\n", __LINE__);
        	goto out;
    	}
    	srcBuf->len = user_data->inp_buf_len;
	if (!srcBuf->len) {
		printf("Invalid input buffer length, %d.\n", __LINE__);
	}

	/* destination data */
	dstBuf = malloc(sizeof(DRE_dataDesc));
	if (dstBuf == NULL) {
		printf("Failed to alloc memory, %d.\n", __LINE__);
		goto out;
	}
	memset(dstBuf, 0, sizeof(DRE_dataDesc));

	dstBuf->ptr = user_data->outp_buf_addr;
	if (dstBuf->ptr == NULL) {
	    printf("Failed to alloc memory, %d.\n", __LINE__);
	    goto out;
	}
	dstBuf->len = *user_data->outp_buf_len;
	if ((!user_data->outp_buf_len) || (!dstBuf->len)) {
	    	printf("Invalid output buffer length, %d.\n", __LINE__);
		goto out;
	}

	opData.dstLen = user_data->outp_buf_len;
	opData.compCount.countMode = 0;
	opData.compCount.sourceCount = user_data->inp_buf_len;
	opData.compCount.headCount = 0;

	/* submit data */
	retCode = DRE_rawSessSubmitSync(sessID, 
	                                &opData,
	                                srcBuf,
	                                1, 
	                                dstBuf,
	                                1, 
	                                NULL);
	                                //(DRE_uptr)&usrInfo);
	if (DRE_IS_RESULT_ERR(retCode)) {
	    printf("failed to submit data in line %d, err code:%x\n",
	            __LINE__, retCode);
	    goto out;
	}
out:
	if (sessID) {
		DRE_rawSessClose(sessID);
	}
 
	/* release memories */
	if (srcBuf != NULL) {
		free(srcBuf);
	}

	if (dstBuf != NULL) {
		free(dstBuf);
	}
	return 0; 
#else
	int event;
	int ret;
	int dst_len = *user_data->outp_buf_len;
	int try_cnt = 0;
	if(user_data->inp_buf_len==0&&*(user_data->outp_buf_len)==0)return 0;
retry:
	ret = ioctl(fd, UKZIP_DMA_START, (void *) user_data);
	if (ret < 0) {
		printf("UKZip: Dma start failed.\n");
		return -1;
	}

	ret = check_event(fd, EVT_TIMEOUT, &event);
	if ((ret < 0) || (ret == 0)) {
		printf("UKZip: Check event failed.\n");
		if (try_cnt) {
			return -1;
		}

		ret = ioctl(fd, UKZIP_RESET, NULL);
		if (ret < 0) {
			printf("UKZip: Reset failed.\n");
			return -1;
		}

		printf("UKZip: Reset zip device successed.\n");
		*user_data->outp_buf_len = dst_len;
		try_cnt = 1;
		goto retry;
	}

	ret = ioctl(fd, UKZIP_DMA_STOP, NULL);
	if (ret < 0) {
		printf("UKZip: Dma stop failed.\n");
		return -1;
	}

	/* found a bad last command */
	if (!event) {
		if (try_cnt) {
			return -1;
		}
		printf("UKZip: Found an exception stopping caused by user application at last round.\n");
	
		*user_data->outp_buf_len = dst_len;
		try_cnt = 1;
		goto retry;
	}

	return 0;
#endif
}

