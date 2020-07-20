
#include "uk_main.h"
extern int gl_hdev_mgr;		//zw long -> int


int getdevlist()
{
	int ret;
	if(gl_hdev_mgr == 0){
		return 0;
	}
	memset(gl_devname,0,sizeof(gl_devname));
	if( (ret = ioctl(gl_hdev_mgr,UK_GETDEVLIST,gl_devname)) < 0){
		printf("Get Device List Error:%d\n",errno);
		return ret;
	}
	if(ret >= 0){
		ret = (strlen(gl_devname) + 1) / 7;
	}
	return ret;
}


long opendevice(char *devname)
{
	int idx,hdev;
	unsigned long ultemp;
	void *devbuf;
	int err;
	unsigned char sztemp[50];
	if(gl_hdev_mgr == 0 || devname == NULL || strlen(devname) != 6 || memcmp(devname,"TNET",4) || !isdigit(devname[4]) || !isdigit(devname[5])){
		return -ENODEV;
 	}
	
	sscanf(devname,"TNET%02d",&idx);
	if(idx < 0 || idx > MAX_DEVNUM ){
		return -ENODEV;
	}
	pthread_mutex_lock(&gl_devlist[idx].devlock);
	if( gl_devlist[idx].hdevice == 0){
		sprintf(sztemp,"/dev/ukrecvdata%d",idx);
		hdev = open(sztemp,O_RDWR | O_NONBLOCK);
		if( hdev < 0){
			pthread_mutex_unlock(&gl_devlist[idx].devlock);
			return hdev;
		}
		gl_devlist[idx].hdevice = hdev;
	}
	pthread_mutex_unlock(&gl_devlist[idx].devlock);
	return idx;
}


