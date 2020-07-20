#include <fcntl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <sys/shm.h>
#include "uk_main.h"
#include "uklib.h"
#include "ukzip_api.h"


int i2c_file;
int i2c_file_hts221;
pthread_mutex_t i2c_devlock;
pthread_mutexattr_t i2c_mutAttr;


#define I2C_SMBUS_READ  1
#define I2C_SMBUS_WRITE 0

static void initlibrary(void) __attribute__ ((constructor));
static void leavelibrary(void) __attribute__ ((destructor));

//#define DEBUG
#define TIMEOUT 1000

#define MAX_POWEROFF_THREAD 10

enum{
	F_VERSION_DATE=20,
	F_VERSION=24,
	F_LINKS=28,
	F_IN_CAGE=32,
	F_POWER_BASE=40,
};

typedef struct _UKPower_{
	int fd;
	int tid;
	UKPower_CallBack pFunc;
	void *pContext;
	int used;
}UKPower;

struct shared_use_st
{
	ex_version version_ex[17];
};
struct shared_use_st *shared;
void *shm = NULL;
int shmid; 

UKPower gl_power[MAX_POWEROFF_THREAD];

unsigned char gl_devname[MAX_DEVNAME];
deviceobject  gl_devlist[MAX_DEVNUM];
unsigned long gl_devnum = 0;
long 		  gl_hdev_mgr = 0;
//供自定义灯和开入开出使用的文件句柄
unsigned long gl_phri_fd;
//对时状态
unsigned long gl_sync_state=0;
//开始读入光口数据的标志
unsigned long gl_read_start=1;
//回调线程开始工作的标志
unsigned long gl_start_process=1;


//用于统计扩展口的信息的
UK_NetStat gl_realtime_netstat[MAX_DEVNUM_EX];
UK_FPOWER gl_realtime_power[MAX_DEVNUM_EX];
unsigned long gl_realtime_fincage[MAX_DEVNUM_EX];

//对时线程
pthread_t th_synctime_id;
//回调函数的线程
pthread_t th_process_id[4];
//将数据读取第二层的缓冲区的线程
pthread_t th_read_id;
pthread_t th_addcnt_id;


#ifdef DBG
struct timeval gl_dbg_tv_last[16],gl_dbg_tv_this[16];
#endif


static inline int i2c_read_byte(int fd, int addr, int reg, unsigned char * data)
{
	struct i2c_smbus_ioctl_data args;
	int ret;
	if (ioctl(fd,I2C_SLAVE,addr) < 0){
		fprintf(stderr,
			"Error: Could not set address to 0x%02x: %s\n",
			addr, strerror(errno));
		return -errno;
	}
    args.read_write = I2C_SMBUS_READ;
    args.command	= reg;
    args.size		= I2C_SMBUS_BYTE_DATA;
    args.data		= (union i2c_smbus_data *)data;
    ret=ioctl(fd,I2C_SMBUS,&args);
	return ret;
}

static inline int i2c_write_byte(int fd, int addr, int reg, unsigned char * data)
{
	struct i2c_smbus_ioctl_data args;
	int ret;
	if (ioctl(fd,I2C_SLAVE,addr) < 0){
		fprintf(stderr,
			"Error: Could not set address to 0x%02x: %s\n",
			addr, strerror(errno));
		return -errno;
	}
	args.read_write = I2C_SMBUS_WRITE;
    args.command = reg;
    args.size = I2C_SMBUS_BYTE_DATA;
    args.data = (union i2c_smbus_data *)data;
    ret=ioctl(fd,I2C_SMBUS,&args);
	return ret;
}


static void *threadprocess0(void);
static void *threadprocess1(void);
static void *threadprocess2(void);
static void *threadprocess3(void);



/**
  * @brief  init the usart buffer
  * @param  None
  * @retval None
  */
void Tsec_DataBufInit (Tsec_DataBuffer *buf,unsigned int maxSize) 
{
	buf->rear  = 0;
	buf->front = 0;
	buf->flag  = 0;
	buf->maxSize = maxSize;	
	buf->buffer = (TsecDataStr *)malloc(sizeof(TsecDataStr)*(buf->maxSize));
	if(buf->buffer == NULL)
	{
		printf("Tsec_DataBuf Init Fail\n");
	}
}

/**
  * @brief   get the buffer size
  * @param  buffer
  * @retval None
  */
unsigned int Tsec_IsFull (Tsec_DataBuffer *buf) 
{
	if(buf->front==buf->rear && buf->flag==1)
		return UK_TRUE;
	else 
		return UK_FALSE;
}

/**
  * @brief   get the buffer size
  * @param  buffer
  * @retval None
  */
unsigned int Tsec_IsEmpty (Tsec_DataBuffer *buf) 
{
	if(buf->front==buf->rear && buf->flag==0)
		return UK_TRUE;
	else 
		return UK_FALSE;

}


/**
  * @brief   put a character into the buffer
  * @param  buffer and character
  * @retval None
  */
inline UK_STATUS Tsec_DataBufIn (Tsec_DataBuffer *buf, TsecDataStr *data) 
{
	unsigned int next;
	if(Tsec_IsFull(buf)==UK_TRUE)
	{
		return UK_ERR_FAIL;
	}
	buf->buffer[buf->rear] = *data;
	buf->rear = (buf->rear+1)%buf->maxSize;
	buf->flag=1;
	return UK_ERR_SUCCESS;
}

/**
  * @brief   get a character out of  the buffer
  * @param  buffer and character
  * @retval None
  */
inline UK_STATUS Tsec_DataBufOut (Tsec_DataBuffer *buf, TsecDataStr *data) 
{
	unsigned int next;

	if(Tsec_IsEmpty(buf)==UK_TRUE)
	{
		return UK_ERR_FAIL;
	}
	*data = buf->buffer[buf->front];
	buf->front = (buf->front+1)%buf->maxSize;
	buf->flag=0;
	return UK_ERR_SUCCESS;
}

/**
  * @brief   get the buffer size
  * @param  buffer
  * @retval None
  */
unsigned int Tsec_GetDataBufSize (Tsec_DataBuffer *buf) 
{
	unsigned int used = 0;
	if(buf->front==buf->rear && buf->flag==1)
	{
		return buf->maxSize;
	}
	used = (buf->maxSize + buf->rear - buf->front) % (buf->maxSize);
	return (used);
}


void * threadforread()
{
	unsigned int  idx;
	unsigned int  hidx_ex;
	unsigned int  hidx_ex_2;
	unsigned int  f_incage;
	unsigned int  linkstat;
	UK_FPOWER 	  power;
	unsigned int  hidx = 0;
	int           i = 0;
	int           j = 0;
	long          readptr;
	long          writeptr=0;
	long          needsize=0;
	long          leftSize = 0;
	unsigned long tempcount1 = 0;
	unsigned long addrOffset=0;
	unsigned long bdval;
	unsigned long bdLen;
	unsigned char * dmabase;
	unsigned long buffsize;
	TsecDataStr * pDataStr;
	unsigned int  rear = 0;
	//推送的标志
	unsigned long bdval_heart=0;
	//临时存放bd的值，用来判断是否是推送报文
	int 		  read_count=0;
	unsigned long used;
	unsigned long max_hidx;
	power.tx_power=0;
	power.rx_power=0;
	if(gl_devnum>2)
		max_hidx=MAX_DEVNUM;
	else
		max_hidx=2;
	prctl(PR_SET_NAME, "threadforread");
	while(gl_read_start)
	{
		read_count++;
		if(read_count>100)
		{
			ioctl(gl_hdev_mgr,UK_PCIEEXIST);
			read_count=0;
		}
		for(hidx=0;hidx<max_hidx;hidx++)
		{
			//pthread_mutex_lock(&(gl_devlist[hidx].devlock));
			if( gl_devlist[hidx].hdevice >0)
			{	
				dmabase = gl_devlist[hidx].databuff;
				if(dmabase == NULL)
				{
					//pthread_mutex_unlock(&(gl_devlist[hidx].devlock));
					continue;
				}
				readptr = gl_devlist[hidx].readptr;
#if 1
				ioctl(gl_devlist[hidx].hdevice,UK_RDWRITEPTR,&writeptr);
				if(writeptr>=readptr)
				{
					needsize=writeptr-readptr;
				}
				else
				{
					needsize=(gl_devlist[hidx].dmaSize/PKG_SIZE)+writeptr-readptr;
				}
#else
				needsize=gl_devlist[hidx].dmaSize/PKG_SIZE;
#endif
				
				for(j=0;j<needsize;j++)
				{
					pthread_mutex_lock(&(gl_devlist[hidx].devlock));
					rear = gl_devlist[hidx].rx_buf.rear;
					pDataStr = &(gl_devlist[hidx].rx_buf.buffer[rear]);
					leftSize = TSEC_DATA_SIZE - pDataStr->size;

					addrOffset = ((readptr+j) % (gl_devlist[hidx].dmaSize/PKG_SIZE));
					
					bdval = *((unsigned long  *)(dmabase + addrOffset*PKG_SIZE));
					bdval = ntohl(bdval);
					
					if(gl_devnum>2)
						used = bdval;
					else
						used = *((unsigned long  *)(dmabase + addrOffset*PKG_SIZE + 2044));
					if((used & 0x1) != 0)
					{
						
						bdLen = ((bdval >> 16) & 0x7FF);
						//有效内容的长度
						bdLen -= 4;
						//去掉BD本身的长度
						if(bdLen <= 0||bdLen>PKG_SIZE)
						{
							pthread_mutex_unlock(&(gl_devlist[hidx].devlock));
							printf("hidx:%d,bdLen:%d\n",hidx,bdLen);
							continue;
						}

//特殊报文的处理(开始)						
						if(gl_devnum>2){
							bdval_heart = *((unsigned long  *)(dmabase + addrOffset*PKG_SIZE + 4));
							bdval_heart = ntohl(bdval_heart);
							if( bdval_heart&0x80000000 ){
#ifdef HEART

								if(leftSize>=76)
								{
									memcpy((char *)(pDataStr->buf+pDataStr->size),(char *)(dmabase + addrOffset*PKG_SIZE+4),64);
									pDataStr->size += 76;
								}
								if(pDataStr->size)
								{
									gl_devlist[hidx].rx_buf.rear=(rear+1)%(gl_devlist[hidx].rx_buf.maxSize);
									gl_devlist[hidx].rx_buf.flag = 1;
								}
								*((unsigned long *)(dmabase + addrOffset*PKG_SIZE)) = 0;
								pthread_mutex_unlock(&(gl_devlist[hidx].devlock));
								continue;
#else
								if(pDataStr->size)
								{
									gl_devlist[hidx].rx_buf.rear=(rear+1)%(gl_devlist[hidx].rx_buf.maxSize);
									gl_devlist[hidx].rx_buf.flag = 1;
								}
								*((unsigned long *)(dmabase + addrOffset*PKG_SIZE)) = 0;
								pthread_mutex_unlock(&(gl_devlist[hidx].devlock));
								continue;
#endif
							}
						}
						else if(bdval&0x80000000){
							idx=0;
							hidx_ex=(bdval>>12)&0xf;
							//printf("hidx_ex:%d\n",hidx_ex);
							hidx_ex_2=(bdval>>8)&0xf;
							//if(hidx_ex>0&&hidx_ex<=8)
							{	
								shared->version_ex[hidx_ex].hasdev=5;
								if(shared->version_ex[hidx_ex].date!=*((unsigned long  *)(dmabase + addrOffset*PKG_SIZE+F_VERSION_DATE)))
									shared->version_ex[hidx_ex].date=*((unsigned long  *)(dmabase + addrOffset*PKG_SIZE+F_VERSION_DATE));
								if(shared->version_ex[hidx_ex].version!=*((unsigned long  *)(dmabase + addrOffset*PKG_SIZE+F_VERSION)))
									shared->version_ex[hidx_ex].version=*((unsigned long  *)(dmabase + addrOffset*PKG_SIZE+F_VERSION));
							}
							idx+=MAX_DEVNUM;
							idx+=MAX_DEVNUM_PERDEV*(hidx_ex-1);
							idx+=hidx_ex_2;
							for(i=0;i<MAX_DEVNUM_PERDEV;i++,idx++)
							{
								linkstat = 0;
								linkstat = *((unsigned long  *)(dmabase + addrOffset*PKG_SIZE+F_LINKS));
								linkstat = (linkstat>>i)&0x1;
								f_incage = 0;
								f_incage = *((unsigned long  *)(dmabase + addrOffset*PKG_SIZE+F_IN_CAGE));
								f_incage = (f_incage>>i)&0x1;
								power.tx_power=*((unsigned short  *)(dmabase + addrOffset*PKG_SIZE+F_POWER_BASE+i*4+2));
								power.rx_power=*((unsigned short  *)(dmabase + addrOffset*PKG_SIZE+F_POWER_BASE+i*4));
								gl_realtime_fincage[idx]=f_incage;
								gl_realtime_power[idx].tx_power=power.tx_power;
								gl_realtime_power[idx].rx_power=power.rx_power;
								gl_realtime_netstat[idx].linkstate= linkstat;
							}
							*((unsigned long  *)(dmabase + addrOffset*PKG_SIZE + 2044))=0;
							pthread_mutex_unlock(&(gl_devlist[hidx].devlock));
							continue;
						}
						else;
//特殊报文的处理(结束)
						
						if(leftSize<bdLen)
						{
							if(Tsec_IsFull(&(gl_devlist[hidx].rx_buf))==UK_FALSE)
							{
								//缓冲区还没有用完就开辟下一个缓冲
								gl_devlist[hidx].rx_buf.rear = (rear+1)%(gl_devlist[hidx].rx_buf.maxSize);
								j--;
								gl_devlist[hidx].rx_buf.flag = 1;
								pthread_mutex_unlock(&(gl_devlist[hidx].devlock));
								continue;
							}
							else
							{
								if(gl_devnum>2)
									*((unsigned long *)(dmabase + addrOffset*PKG_SIZE)) = 0;
								else
									*((unsigned long  *)(dmabase + addrOffset*PKG_SIZE + 2044))=0;
								pthread_mutex_unlock(&(gl_devlist[hidx].devlock));
								continue;
							}
						}
						else
						{
							memcpy((char *)(pDataStr->buf+pDataStr->size),(char *)(dmabase + addrOffset*PKG_SIZE+4),bdLen);
							pDataStr->size+=bdLen;
							if(gl_devnum>2)
								*((unsigned long *)(dmabase + addrOffset*PKG_SIZE)) = 0;
							else
								*((unsigned long  *)(dmabase + addrOffset*PKG_SIZE + 2044))=0;
							if(j==needsize-1)
							{
								gl_devlist[hidx].rx_buf.rear = (rear+1)%(gl_devlist[hidx].rx_buf.maxSize);
								gl_devlist[hidx].rx_buf.flag = 1;
							}
						}
					}
					else
					{
						if(pDataStr->size)
						{
							gl_devlist[hidx].rx_buf.rear = (rear+1)%(gl_devlist[hidx].rx_buf.maxSize);
							gl_devlist[hidx].rx_buf.flag = 1;
						}
						pthread_mutex_unlock(&(gl_devlist[hidx].devlock));
						break;
					}
					pthread_mutex_unlock(&(gl_devlist[hidx].devlock));
				}
				gl_devlist[hidx].readptr = ((readptr+j) % (gl_devlist[hidx].dmaSize/PKG_SIZE));

			}
			//pthread_mutex_unlock(&(gl_devlist[hidx].devlock));			
		}
		
		usleep(10*TIMEOUT);
	}
}


void * addcnt()
{
	while(gl_read_start)
	{
		ioctl(gl_hdev_mgr,UK_ADDCNT,0);
		sleep(1);
	}
}

static void initlibrary(void){
	int i;
	int ret;
	char szbuf[250],*ptmp;
	FILE *fp;
	int drivermajor;
	unsigned char data;
	printf("init library\n");
	shmid = shmget((key_t)1234, sizeof(struct shared_use_st), 0777|IPC_CREAT); 
	if(shmid == -1)  
    {  
        fprintf(stderr, "shmget failed\n");  
        exit(EXIT_FAILURE);  
    }
	shm = shmat(shmid, 0, 0); 
	 if(shm == (void*)-1)  
    {  
        fprintf(stderr, "shmat failed\n");  
        exit(EXIT_FAILURE);  
    }
	  
    shared = (struct shared_use_st*)shm;  	

#ifdef I2C_DEV
	i2c_file=open("/dev/i2c-0",O_RDWR);
	i2c_file_hts221=i2c_file;
	data=0x82;
	i2c_write_byte(i2c_file,0x5f,0x20,&data);
	pthread_mutexattr_settype(&i2c_mutAttr, PTHREAD_MUTEX_RECURSIVE_NP);
	pthread_mutex_init(&i2c_devlock,&i2c_mutAttr);	
#endif

	gl_hdev_mgr = open("/dev/ukrecvdata254",O_RDWR | O_NONBLOCK);

	if( gl_hdev_mgr <= 0 ){
		fp = popen("awk '$2 == \"UK_REC\" {print $1}' /proc/devices","r");
		if(fp == NULL)
			goto errleave;
		memset(szbuf, 0 ,sizeof(szbuf));
		ptmp = fgets(szbuf,250,fp);
		pclose(fp);
		if( (drivermajor = atoi(szbuf)) == 0)
			goto errleave;
		fp = popen("find /dev/ -name \"ukrecvdata254\"","r");
		if( fp == NULL)
			goto errleave;
		memset(szbuf, 0 ,sizeof(szbuf));
		ptmp = fgets(szbuf,250,fp);
		pclose(fp);
		
		if( strstr(szbuf,"/dev/ukrecvdata254") == NULL){
			for( i = 0;i<MAX_DEVNUM;i++){
				sprintf(szbuf,"mknod /dev/ukrecvdata%d c %d %d",i,drivermajor,i);
				ret = system(szbuf);
			}
			sprintf(szbuf,"mknod /dev/ukrecvdata254 c %d 254",drivermajor);
			ret = system(szbuf);
		}
	
		gl_hdev_mgr = open("/dev/ukrecvdata254",O_RDWR | O_NONBLOCK);
errleave:
		if( gl_hdev_mgr <= 0 ){
			gl_hdev_mgr = 0;
			return;
		}
	}
	gl_phri_fd=gl_hdev_mgr;
	gl_devnum =  getdevlist();
	memset(gl_devlist,0,sizeof(gl_devlist));
	for( i=0;i<MAX_DEVNUM;i++)
	{
		if(i<gl_devnum/2)
			gl_devlist[i].i=0;
		else
			gl_devlist[i].i=1;
		pthread_mutex_init(&gl_devlist[i].devlock,NULL);
	}
}




static void leavelibrary(void)
{
	int i;
	printf("leave library\n");
	gl_read_start=0;
	if(th_read_id!=0)
	{
		pthread_join(th_read_id,NULL);
		ioctl(gl_hdev_mgr,UK_DEVSTOP,0);
	}
	gl_start_process=0;
	
	for(i=0;i<4;i++)
	{
		if(th_process_id[i]!=0)
		{
			pthread_join(th_process_id[i],NULL);
			th_process_id[i]=0;
		}
	}

	for(i=0;i<MAX_DEVNUM;i++)
	{
		if(gl_devlist[i].rx_buf.buffer != NULL)
		{
			free(gl_devlist[i].rx_buf.buffer);
		}
	}
	
	if(gl_hdev_mgr){
	  for( i = 0 ; i<MAX_DEVNUM; i++){
		  if(gl_devlist[i].hdevice){
			  close(gl_devlist[i].hdevice);
		  }	
		  pthread_mutex_destroy(&gl_devlist[i].devlock);
	  }
	  close(gl_hdev_mgr);
	}
#ifdef I2C_DEV
	pthread_mutex_destroy(&i2c_devlock);
#endif
	if(gl_phri_fd)
		close(gl_phri_fd);

}



UK_STATUS UK_UsrLed(unsigned int bit,unsigned long status)
{
	int ret;
	unsigned long curstatus = 0;
	if(gl_phri_fd == 0){
		return UK_ERR_INVHANDLE;
	}
	if( bit != -1 && !(bit >= 0 && bit <= 8)){
		return UK_ERR_INVARG;
	} 
	if( bit == -1){
		status&=0xff;
		if( (ret = ioctl(gl_phri_fd,UK_LED_W, &status)) < 0){
			return ret;
		}
	}else{
		if( (ret = ioctl(gl_phri_fd,UK_LED_R, &curstatus)) < 0){
			return ret;
		}

		switch(status){
			case LED_ON:
				curstatus |= 1ul << bit;
				break;
			case LED_OFF:
				curstatus &= ~(1ul << bit);
				break;
			case LED_TOGGLE:
				curstatus ^= (1ul << bit);
				break;
			default:
				return UK_ERR_INVARG;
		}
		curstatus&=0xff;
		if( (ret = ioctl(gl_phri_fd,UK_LED_W, &curstatus)) < 0){
			return ret;
		}
	}
	return UK_ERR_SUCCESS; 

}

UK_STATUS UK_UsrLed_GPIO(int bit,unsigned long status)
{
	unsigned long old_status;
	unsigned long new_status;
	if(gl_phri_fd == 0 || gl_phri_fd < 0){
        return UK_ERR_INVHANDLE;
    }
    if(bit < 0 || bit > 1){
        return UK_ERR_INVARG;
    } 
	ioctl(gl_phri_fd,UK_GPIO_GET_LED,&old_status);
	switch(status){
		case LED_ON:
			new_status=old_status&(~(1<<(bit+8)));
			ioctl(gl_phri_fd,UK_GPIO_SET_LED,&new_status);
			break;
		case LED_OFF:
			new_status=old_status|(1<<(bit+8));
			ioctl(gl_phri_fd,UK_GPIO_SET_LED,&new_status);
			break;
		default:
            return UK_ERR_INVARG;
	}
}


int UK_getNetList(char *pName,int buflen)
{
	if(buflen < gl_devnum * 7){
		return -ENOMEM;
	}
	if( gl_devnum){ 
		memcpy(pName,gl_devname,strlen(gl_devname) + 1);
	}
	return gl_devnum;
}


int UK_ReadPowerState()
{
	int ret;
	int state;
	ret=ioctl(gl_phri_fd,UK_POWERSTAT,&state);
	if(ret!=0)
		return -1;
	if(state>=0)
		return state;
	else
		return -1;
}

UK_STATUS UK_unregNetCallback(char *devname,UK_NetCallBack pFunc,void *pContext)
{
	char cardname[10];
	long hidx = opendevice(devname);
	if( hidx < 0){
		return UK_ERR_NODEVS;
	}
	gl_devlist[hidx].func = NULL;
	return UK_ERR_SUCCESS;
}


UK_STATUS UK_regNetCallback(char *devname,UK_NetCallBack pFunc,void *pContext,unsigned long timeout)
{
	long hidx = opendevice(devname);
	char cardname[10];
	int ret;
	if( hidx < 0){
		return UK_ERR_NODEVS;
	}
	if( gl_devlist[hidx].func != NULL ){
		return UK_ERR_EXISTS;
	}
	gl_devlist[hidx].func =       pFunc;
	gl_devlist[hidx].pcontext =   pContext;
	gl_devlist[hidx].timeout =    timeout;
	return UK_ERR_SUCCESS;	
}



void * th_powercallback(UKPower * ppower)
{
	int poweroff;
	int fd=ppower->fd;
	void * pContext;
	UKPower_CallBack pFunc=ppower->pFunc;
	pContext=ppower->pContext;
	unsigned long count=0;
	while(ppower->used){
		ioctl(fd,UK_POWEROFF,&poweroff);
		if(poweroff==1)
		{
			if(pFunc)
				(pFunc)(pContext);
		}
		if(count>100){
			poweroff=0;
			ioctl(fd,UK_POWERON,&poweroff);
			count=0;
		}
		count++;
		usleep(10000);
	}
}

UK_STATUS UKPower_unregCallback(int fd)
{
	int i;
	UKPower * ppower;
	for(i=0;i<MAX_POWEROFF_THREAD;i++)
		if(gl_power[i].fd==fd)
			break;
	if(i==MAX_POWEROFF_THREAD)
		return UK_ERR_FAIL;
	ppower=&gl_power[i];
	ppower->used=0;
	pthread_join(ppower->tid,NULL);
	return UK_ERR_SUCCESS;
}


UK_STATUS UKPower_regCallback(int fd,UKPower_CallBack pFunc,void *pContext)
{
	int i;
	int tid;
	UKPower * ppower;
	for(i=0;i<MAX_POWEROFF_THREAD;i++)
		if(gl_power[i].used==0)
			break;
	if(i==MAX_POWEROFF_THREAD)
		return UK_ERR_FAIL;
	ppower=&gl_power[i];
	ppower->fd=fd;
	ppower->pFunc=pFunc;
	ppower->pContext=pContext;
	ppower->used=1;
	pthread_create(&tid,NULL,th_powercallback,ppower);
	ppower->tid=tid;
	return UK_ERR_SUCCESS;	
}

static inline long _uk_readfile(int hidx,unsigned char * pbuf,int len)
{
	int bufSize = 0;
	int i = 0;
	int rc = 0;
	int front = 0;
	TsecDataStr *pData;
	bufSize = Tsec_GetDataBufSize(&(gl_devlist[hidx].rx_buf));
	
	for(i=0;i<bufSize;i++)
	{
		if (Tsec_IsEmpty(&(gl_devlist[hidx].rx_buf))==UK_TRUE) 
		{					
			break;
		}
		front =	gl_devlist[hidx].rx_buf.front;	
		pData = &(gl_devlist[hidx].rx_buf.buffer[front]);
		if(pData->size <= len && pData->size>0)
		{
			//拷贝数据至应用层
			memcpy(pbuf,pData->buf,pData->size);
			rc += (pData->size);
			len -= (pData->size);
			pbuf += (pData->size);
			pData->size = 0;
			front = (front + 1) % (gl_devlist[hidx].rx_buf.maxSize);
			gl_devlist[hidx].rx_buf.front = front;
			gl_devlist[hidx].rx_buf.flag=0;
		}
		else
			break;
	}
	return rc;
}


long  UK_ReadFile(char *devname,void * buffer,int size)
{
	int rc;
	long hidx = opendevice(devname);
	if( hidx < 0){
		return UK_ERR_NODEVS;
	}
    if(gl_devlist[hidx].used==0)
        return 0;
	pthread_mutex_lock(&(gl_devlist[hidx].devlock));
	rc=_uk_readfile(hidx,buffer,size);
	pthread_mutex_unlock(&(gl_devlist[hidx].devlock));
	return rc;
}


static void *threadprocess0(void)
{
	fd_set readset;	
	struct timeval timeout;
	int ret;
	int hidx;
	long fd = 0;
	int i;
	timeout.tv_sec  = 1;
	timeout.tv_usec = 0;
	int fd_ready=0;
	int maxfd=0;
	long costtime=0;
	if(gl_devlist[0].func==NULL&&gl_devlist[4].func==NULL&&gl_devlist[8].func==NULL&&gl_devlist[12].func==NULL)
		return;
	prctl(PR_SET_NAME,"threadprocess0");
	while(gl_start_process)
	{
			for(i=0;i<MAX_DEVNUM;i+=4)
			{
				if( gl_devlist[i].hdevice > 0 && gl_devlist[i].func && Tsec_IsEmpty(&(gl_devlist[i].rx_buf))==UK_FALSE)
				{
					(gl_devlist[i].func)(NULL,UK_ERR_SUCCESS,gl_devlist[i].pcontext);
				}
#ifdef DBG
				gl_dbg_tv_last[i]=gl_dbg_tv_this[i];
				gettimeofday(&(gl_dbg_tv_this[i]),NULL);
				costtime=(gl_dbg_tv_this[i].tv_sec-gl_dbg_tv_last[i].tv_sec)*1000000+(gl_dbg_tv_this[i].tv_usec-gl_dbg_tv_last[i].tv_usec);
				if(costtime>800000||costtime<0)
					printf("i:%02d,costtime:%d,now:%d:%06d,old:%d:%06d\n",i,costtime,gl_dbg_tv_this[i].tv_sec,gl_dbg_tv_this[i].tv_usec,gl_dbg_tv_last[i].tv_sec,gl_dbg_tv_last[i].tv_usec);
#endif
			}
			usleep(TIMEOUT);
	}
	printf("stop the thread!\n");
	pthread_exit(NULL);
	return NULL;
}

static void *threadprocess1(void)
{
	fd_set readset;	
	struct timeval timeout;
	int ret;
	int hidx;
	long fd = 0;
	int i;
	timeout.tv_sec  = 1;
	timeout.tv_usec = 0;
	int fd_ready=0;
	int maxfd=0;
	long costtime=0;
	if(gl_devlist[1].func==NULL&&gl_devlist[5].func==NULL&&gl_devlist[9].func==NULL&&gl_devlist[13].func==NULL)
		return;
	prctl(PR_SET_NAME, "threadprocess1");	
	while(gl_start_process)
	{
			for(i=1;i<MAX_DEVNUM;i+=4)
			{
				if( gl_devlist[i].hdevice > 0 && gl_devlist[i].func && Tsec_IsEmpty(&(gl_devlist[i].rx_buf))==UK_FALSE)
				{
					(gl_devlist[i].func)(NULL,UK_ERR_SUCCESS,gl_devlist[i].pcontext);
				}
#ifdef DBG
				gl_dbg_tv_last[i]=gl_dbg_tv_this[i];
				gettimeofday(&(gl_dbg_tv_this[i]),NULL);
				costtime=(gl_dbg_tv_this[i].tv_sec-gl_dbg_tv_last[i].tv_sec)*1000000+(gl_dbg_tv_this[i].tv_usec-gl_dbg_tv_last[i].tv_usec);
				if(costtime>800000||costtime<0)
					printf("i:%02d,costtime:%d,now:%d:%06d,old:%d:%06d\n",i,costtime,gl_dbg_tv_this[i].tv_sec,gl_dbg_tv_this[i].tv_usec,gl_dbg_tv_last[i].tv_sec,gl_dbg_tv_last[i].tv_usec);
#endif
			}
			usleep(TIMEOUT);
	}
	printf("stop the thread!\n");
	pthread_exit(NULL);
	return NULL;
}

static void *threadprocess2(void)
{
	fd_set readset;	
	struct timeval timeout;
	int ret;
	int hidx;
	long fd = 0;
	int i;
	timeout.tv_sec  = 1;
	timeout.tv_usec = 0;
	int fd_ready=0;
	int maxfd=0;
	long costtime=0;
	if(gl_devlist[2].func==NULL&&gl_devlist[6].func==NULL&&gl_devlist[10].func==NULL&&gl_devlist[14].func==NULL)
		return;
	prctl(PR_SET_NAME, "threadprocess2");	
	while(gl_start_process)
	{
			for(i=2;i<MAX_DEVNUM;i+=4)
			{
				if( gl_devlist[i].hdevice > 0 && gl_devlist[i].func && Tsec_IsEmpty(&(gl_devlist[i].rx_buf))==UK_FALSE)
				{
					(gl_devlist[i].func)(NULL,UK_ERR_SUCCESS,gl_devlist[i].pcontext);
				}
#ifdef DBG
				gl_dbg_tv_last[i]=gl_dbg_tv_this[i];
				gettimeofday(&(gl_dbg_tv_this[i]),NULL);
				costtime=(gl_dbg_tv_this[i].tv_sec-gl_dbg_tv_last[i].tv_sec)*1000000+(gl_dbg_tv_this[i].tv_usec-gl_dbg_tv_last[i].tv_usec);
				if(costtime>800000||costtime<0)
					printf("i:%02d,costtime:%d,now:%d:%06d,old:%d:%06d\n",i,costtime,gl_dbg_tv_this[i].tv_sec,gl_dbg_tv_this[i].tv_usec,gl_dbg_tv_last[i].tv_sec,gl_dbg_tv_last[i].tv_usec);
#endif
			}
			usleep(TIMEOUT);
	}
	printf("stop the thread!\n");
	pthread_exit(NULL);
	return NULL;
}



static void *threadprocess3(void)
{
	fd_set readset;	
	struct timeval timeout;
	int ret;
	int hidx;
	long fd = 0;
	int i;
	timeout.tv_sec  = 1;
	timeout.tv_usec = 0;
	int fd_ready=0;
	int maxfd=0;
	long costtime=0;
	if(gl_devlist[3].func==NULL&&gl_devlist[7].func==NULL&&gl_devlist[11].func==NULL&&gl_devlist[15].func==NULL)
		return;
	prctl(PR_SET_NAME, "threadprocess3");	
	while(gl_start_process)
	{
			for(i=3;i<MAX_DEVNUM;i+=4)
			{
				if( gl_devlist[i].hdevice > 0 && gl_devlist[i].func && Tsec_IsEmpty(&(gl_devlist[i].rx_buf))==UK_FALSE)
				{
					(gl_devlist[i].func)(NULL,UK_ERR_SUCCESS,gl_devlist[i].pcontext);
				}
#ifdef DBG
				gl_dbg_tv_last[i]=gl_dbg_tv_this[i];
				gettimeofday(&(gl_dbg_tv_this[i]),NULL);
				costtime=(gl_dbg_tv_this[i].tv_sec-gl_dbg_tv_last[i].tv_sec)*1000000+(gl_dbg_tv_this[i].tv_usec-gl_dbg_tv_last[i].tv_usec);
				if(costtime>800000||costtime<0)
					printf("i:%02d,costtime:%d,now:%d:%06d,old:%d:%06d\n",i,costtime,gl_dbg_tv_this[i].tv_sec,gl_dbg_tv_this[i].tv_usec,gl_dbg_tv_last[i].tv_sec,gl_dbg_tv_last[i].tv_usec);
#endif
			}
			usleep(TIMEOUT);
	}
	printf("stop the thread!\n");
	pthread_exit(NULL);
	return NULL;
}




UK_STATUS UK_NetCaptureStart(void)
{
	int i;
	long hidx=0;
	char devname[100];
	unsigned int cnt_val_1,cnt_val_2;
	ioctl(gl_hdev_mgr,UK_READCNT,&cnt_val_1);
	sleep(2);
	ioctl(gl_hdev_mgr,UK_READCNT,&cnt_val_2);
	if(cnt_val_1!=cnt_val_2)
	{
		return UK_ERR_FAIL;
	}
	gl_read_start=0;
	
	ioctl(gl_hdev_mgr,UK_DEVSTOP,0);
	
	for(i=0;i<MAX_DEVNUM;i++)
	{
		sprintf(devname,"TNET%02d",i);
		hidx=opendevice(devname);
		if( hidx < 0){
			continue;
		}
		if(gl_devlist[hidx].hdevice)
		{
			ioctl(gl_devlist[hidx].hdevice,UK_RDWRITEPTR,&(gl_devlist[hidx].readptr));
		}
		switch(hidx)
		{
			case 6:
			case 7:
			case 14:
			case 15:
				Tsec_DataBufInit(&(gl_devlist[hidx].rx_buf),TSEC_DATABUF_16M);
				gl_devlist[hidx].dmaSize = (DMASIZE_16M);
				break;
			default:
				if(gl_devnum>2)
				{
					Tsec_DataBufInit(&(gl_devlist[hidx].rx_buf),TSEC_DATABUF_8M);
					gl_devlist[hidx].dmaSize = DMASIZE_8M;
				}
				else
				{
					Tsec_DataBufInit(&(gl_devlist[hidx].rx_buf),TSEC_DATABUF_64M);
					gl_devlist[hidx].dmaSize = DMASIZE_NEWZONE;
				}
				break;
		}

		gl_devlist[hidx].databuff=mmap(NULL,gl_devlist[hidx].dmaSize,PROT_READ|PROT_WRITE,MAP_SHARED|MAP_LOCKED,gl_devlist[hidx].hdevice,0);
		if (gl_devlist[hidx].databuff == MAP_FAILED) 
		{
			printf("MMAP port:%d failed\n",hidx);
    		return -1;
  		}
		memset(gl_devlist[hidx].databuff,0,gl_devlist[hidx].dmaSize);
        gl_devlist[hidx].used=1;
	}
    
	gl_read_start=1;
	int ret=0;
#if 0
	pthread_attr_t attr;
	struct sched_param param;
	pthread_attr_init(&attr);
	pthread_attr_setschedpolicy(&attr,SCHED_RR);
	pthread_attr_getschedparam(&attr,&param);
	param.sched_priority = 90;
	pthread_attr_setschedparam(&attr,&param);
	pthread_create(&th_process_id[0],&attr,(void *)threadprocess0,NULL);
	pthread_create(&th_process_id[1],&attr,(void *)threadprocess1,NULL);
	pthread_create(&th_process_id[2],&attr,(void *)threadprocess2,NULL);
	pthread_create(&th_process_id[3],&attr,(void *)threadprocess3,NULL);
	pthread_attr_destroy(&attr);
#endif
	pthread_create(&th_read_id,NULL,threadforread,NULL);
	pthread_create(&th_addcnt_id,NULL,addcnt,NULL);
	ret = ioctl(gl_hdev_mgr,UK_DEVSTART,0);	
	if(ret < 0){
			return UK_ERR_DEVSTART;  
	}
	return UK_ERR_SUCCESS;
}

UK_STATUS UK_NetCaptureStop(void)
{
	int i;
	int hidx;
	char devname[100];
	int ret;

	ret = ioctl(gl_hdev_mgr,UK_DEVSTOP,0);	
	if(ret < 0){
			return UK_ERR_DEVSTART;  
	}
	gl_read_start=0;
	pthread_join(th_addcnt_id,NULL);
	th_addcnt_id=0;
	pthread_join(th_read_id,NULL);
	th_read_id=0;
	gl_start_process=0;
	pthread_join(th_process_id[0],NULL);	
	pthread_join(th_process_id[1],NULL);	
	pthread_join(th_process_id[2],NULL);
	pthread_join(th_process_id[3],NULL);	
	th_process_id[0]=0;
	th_process_id[1]=0;
	th_process_id[2]=0;
	th_process_id[3]=0;
    for(i=0;i<MAX_DEVNUM;i++)
    {
    	sprintf(devname,"TNET%02d",i);
		hidx=opendevice(devname);
		if( hidx < 0){
			continue;
		}
		gl_devlist[hidx].used=0;
		munmap(gl_devlist[hidx].databuff,gl_devlist[hidx].dmaSize);
	}
	return UK_ERR_SUCCESS;
}


int IRIGB_DeviceOpen()
{
	int fd;
	fd = open("/dev/ukrecvdata254", O_RDWR);
	if ( fd < 0 )
		return 0;
	else
		return fd;
	return 0;
}

void IRIGB_DeviceClose(int fd)
{
	close(fd);
}


int gl_loop_sync=0;
void* thsync(int fd)
{
	prctl(PR_SET_NAME, "thsync");
	while(gl_loop_sync)
	{
		if ( ioctl( fd, UK_SYNCT, &gl_sync_state) < 0 ){
			return -1;	
		}
		sleep(5);
		usleep(200000);
	}
}

int IRIGB_EnableDevice(int fd)
{
	int ret;
	gl_loop_sync=1;
	ret=ioctl(fd,UK_IGB_INTEN,NULL);
	pthread_create(&th_synctime_id,NULL,thsync,fd);
	return ret;
}

int IRIGB_Parity(int fd,int en)
{
	int ret=ioctl(fd,UK_IGB_PAR,&en);
	return ret;
}

int IRIGB_DisableDevice(int fd)
{
	gl_loop_sync=0;
	ioctl(fd,UK_IGB_INTDIS,NULL);
	if(th_synctime_id)
		pthread_join(th_synctime_id,NULL);
	return 0;
}


int IRIGB_SyncTime(int fd)
{
	unsigned long val,val1,val2;
	int ret;
	ret=ioctl(fd,UK_IGB_STAT,&val);
	val1=val&0xff;
	val2=val&0x7000;
	val1<<=8;
	val2>>=12;
	if(ret==-1)
		return -1;
	else 
		return val1+val2;
}


int IRIGB_GetPTPState(int fd)
{
	int state;
	int ret;
	ret=ioctl(fd, UK_IGB_STAT, &state);
	if (ret < 0) {
		return -1;
	}
	state&=0xf00;
	state>>=8;
	return state;
}


UK_STATUS IRIGB_ResetTime(int fd)
{
	int val;
	ioctl(fd,UK_RESETTIME,&val);
	return UK_ERR_SUCCESS;
}


UK_TIMESTAMP IRIGB_GetTime(int fd)
{
	UK_TIMESTAMP ts;
	ioctl(fd,UK_NSEC_R,&(ts.nsec));
	while((ts.nsec>998000000||ts.nsec<2000000)&&ts.nsec!=-1)
	{
		usleep(5000);
		ioctl(fd,UK_NSEC_R,&(ts.nsec));	
	}
	ioctl(fd,UK_SEC_R,&(ts.sec));
	return ts;
}


UK_STATUS IRIGB_SetTime(int fd, unsigned long sec)
{
	int ret;
	ret=ioctl(fd,UK_SEC_W,&sec);
	if(ret!=0)
		return UK_ERR_FAIL;
	return UK_ERR_SUCCESS;
}



int UK_dido_setdo(char val, char num)
{
	int ret;
	int curstatus = 0;
	int fd;
	fd=gl_phri_fd;
	if(fd == 0){
		return UK_ERR_INVHANDLE;
	}
	if( (ret = ioctl(fd,UK_DO_R,curstatus)) < 0){
			return ret;
	}
	switch(val){
		case DO_OFF:
			curstatus |= (1ul << num);
			break;
		case DO_ON:
			curstatus &= ~(1ul << num);
			break;
		default:
			return UK_ERR_INVARG;
	}
	if( (ret = ioctl(fd,UK_DO_W, curstatus)) < 0){
			return ret;
	}
	return UK_ERR_SUCCESS; 
}

int UK_read_di(char *val)
{
	int ret;
	int fd;
	fd=gl_phri_fd;
	ret=ioctl(fd,UK_DI,val);
	if(ret!=0)
		return UK_ERR_FAIL;
	return UK_ERR_SUCCESS;
}



int UK_EnableEvent(int ena,int edge)
{
	int ret;
	int fd;
	fd=gl_phri_fd;
	ret = ioctl(fd,UK_INT_ENA_W,&ena);
	ret = ioctl(fd,UK_INT_EDGE_W,&edge);
	if( ret < 0 ){
		return 0xff;
	}
	return 0;
}
int UK_CheckDIEvent (void * event,int timeout)
{
	struct timeval tv_val;
	fd_set evt_set;
	int ret;
	int fd;
	fd=gl_phri_fd;
	memset(&tv_val, 0, sizeof(tv_val));
	tv_val.tv_sec = (signed long) (timeout / 1000);
	tv_val.tv_usec = (signed long) (timeout % 1000 * 1000);
	FD_ZERO(&evt_set);
	FD_SET(fd, &evt_set);
	if( timeout == 0)
		ret = select(fd + 1, &evt_set, NULL, NULL, NULL);
	else
		ret = select(fd + 1, &evt_set, NULL, NULL, &tv_val);
	if (ret == 0) {
		printf("checkdievent timeout\n");
		*((int *) event) = 0;
		return 0;
	} else if (ret < 0) {
		return -1;
	}	
	ret = ioctl(fd,UK_GETEVENT,event);
	if( ret < 0)
		return -1;
	return 0;

}



//add by caowei for wdt
#define 		WDT_TIMEOUT 	1    //单位s

pthread_t 		wdt_tid;
unsigned int 	wdt_enable;




UK_STATUS UK_Wdt_Enable(unsigned int t)
{
	int ret;
	unsigned int time;
	int i=0;

	if(gl_phri_fd == 0){
		return UK_ERR_INVHANDLE;
	}
	
	wdt_enable = 1; 
    	
	if(t)
        time = ((t -1)/ 5) & 0x7f;
    do{          
        ret = ioctl(gl_phri_fd,UK_WDT_EN,&time);
        if(i!=0)
            printf("enable wdt failed\n");
        i++; 
    }while(i<10&&ret!=0);
    if (ret < 0) { 
        return -1;
    }    	

	return 0;
}


UK_STATUS UK_Wdt_Disable()
{

	unsigned int temp;
	
	if(gl_phri_fd == 0){
		return UK_ERR_INVHANDLE;
	}

  	int i;
    int ret; 

    do{               
        ret=ioctl(gl_phri_fd,UK_WDT_DIS,&temp);
        if(i!=0)
            printf("disable wdt failed\n");
        i++; 
    }while(i<10&&ret!=0);

    if (ret < 0) { 
        return -1;
    }    

	return 0;
}

UK_STATUS UK_Wdt_FeedDog()
{
	unsigned int temp=1;
	
	if(gl_phri_fd == 0){
		return UK_ERR_INVHANDLE;
	}
	int ret;
	ret=ioctl(gl_phri_fd,UK_WDT_FEED,&temp);
	if (ret < 0)
		return -1;
	return ret;
}

UK_NetStat UK_Read_Ethstat(int idx)
{
	int i;
	int hidx;
	UK_NetStat stat[MAX_UK_DEVICE];
	char devname[100];
	if(idx<MAX_DEVNUM){
		for(i=0;i<MAX_UK_DEVICE;i++)
		{
			if(gl_devnum>MAX_UK_DEVICE)
				sprintf(devname,"TNET%02d",i*MAX_DEVNUM_PERDEV);
			else
				sprintf(devname,"TNET%02d",i);
			hidx=opendevice(devname);
			if(gl_devlist[hidx].hdevice)
			{
				ioctl(gl_devlist[hidx].hdevice,UK_READNETSTAT,&stat[i]);
			}
		}
		gl_realtime_netstat[idx].linkstate=(stat[idx/(MAX_DEVNUM_PERDEV+MAX_DEVNUM_PERDEV_EXT)].linkstate>>idx)&0x1;
	}
	else
		return gl_realtime_netstat[idx];
	return gl_realtime_netstat[idx];
}

unsigned long UK_Read_Fincage(int idx)
{
#ifdef I2C_DEV
	int ret;
	FILE * fp;
	unsigned char addr;
	char ulTemp[100]={0};
	char result[100]={0};
	unsigned char data;
	unsigned char flag;
	if(gl_devnum<=2){
		if(idx<MAX_DEVNUM_PERDEV){
			data=1<<idx;
			i2c_write_byte(i2c_file,0x70,0xE0,&data);
			i2c_read_byte(i2c_file,0x50,0x14,&flag);
			if(flag==0x41||flag==0x59)
				return 1;
			return 0;
		}
		else
			return gl_realtime_fincage[idx];
	}
#endif
	return -1;
}

UK_FPOWER UK_Read_Realtime_Power(int idx)
{

	int i;
	FILE * fp;
	UK_FPOWER power;
	unsigned short addr;
	unsigned char value[4]={0};
	char ulTemp[100]={0};
	char result[100]={0};
	char szbuf[2][100]={0};
	power.rx_power=0;
	power.tx_power=0;
	unsigned char data;
	unsigned char flag;
#ifdef I2C_DEV
	if(gl_devnum<=2){
		if(idx<MAX_DEVNUM_PERDEV){
			data=1<<idx;
			i2c_write_byte(i2c_file,0x70,0xE0,&data);
			addr=0x66;
			i2c_read_byte(i2c_file,0x50,0x14,&flag);
			if(flag!=0x41)
				return power;
			for(i=0;i<4;i++)
			{
				data=0;
				i2c_read_byte(i2c_file,0x5f,addr+i,&data);
				value[i]=data;
			}
			power.tx_power=value[0]*16*16+value[1];
			power.rx_power=value[2]*16*16+value[3];
			return power;
		}
		else{
			return gl_realtime_power[idx];
		}
	}
#endif
	return power;
}


UK_STATUS UK_getOptions(int idx,unsigned char *pBuf,int *plen)
{
	UK_NetStat rt;
	int needsize;
	needsize=sizeof(UK_EthStat);
	if(idx<0||idx>MAX_DEVNUM_EX)
		return UK_ERR_FAIL;
	else if(pBuf==NULL)
		return UK_ERR_FAIL;
	else if(*plen<needsize)
		return UK_ERR_FAIL;
	else{
		*plen=sizeof(UK_EthStat);
		rt=UK_Read_Ethstat(idx);
		((PUK_EthStat)pBuf)->linkstatus=rt.linkstate;
#ifdef I2C_DEV
		pthread_mutex_lock(&i2c_devlock);
#endif
		((PUK_EthStat)pBuf)->fincage=UK_Read_Fincage(idx);
		((PUK_EthStat)pBuf)->power=UK_Read_Realtime_Power(idx);
#ifdef I2C_DEV
		pthread_mutex_unlock(&i2c_devlock);
#endif
	}
	return UK_ERR_SUCCESS;
}


UK_STATUS UK_Read_hts221(unsigned char addr,unsigned len,char *data)
{
#ifdef I2C_DEV
	if(len==1){
		i2c_read_byte(i2c_file,0x5f,addr,data);
	}	
	else if(len==2){
		i2c_read_byte(i2c_file,0x5f,addr,data);
		i2c_read_byte(i2c_file,0x5f,addr+1,data+1);
	}
	else
		return UK_ERR_FAIL;
	return UK_ERR_SUCCESS;
#else
	return UK_ERR_FAIL;
#endif
}



UK_STATUS UK_Read_Digital_Sensor(UK_SENSOR * sensor)
{
	unsigned char isReady;
	unsigned char T0_degC_x8,T1_degC_x8,T1_T0msb;
	short T_OUT=0,T0_OUT=0,T1_OUT=0;
	double T_DegC;
	double T0_degC;
	double T1_degC;
	unsigned char H0_rH_x2,H1_rH_x2;
	short H_OUT,H0_OUT,H1_OUT;
	unsigned char H0_rH;
	unsigned char H1_rH;
	double H_rH;
	unsigned char whoami;
	UK_STATUS ret;
#ifdef I2C_DEV
	if(i2c_file_hts221==-1){
		return UK_ERR_NODEVS;
	}

	pthread_mutex_lock(&i2c_devlock);	
	UK_Read_hts221(0x0f,1,&whoami);
	if(whoami!=0xbc){
		i2c_file_hts221=-1;
		pthread_mutex_unlock(&i2c_devlock);
		return UK_ERR_NODEVS;
	}

	ret=UK_Read_hts221(0x27,1,&isReady);
	if(ret!=UK_ERR_SUCCESS||isReady!=0x3){
		pthread_mutex_unlock(&i2c_devlock);
		return UK_ERR_FAIL;
	}
	//printf("isReady:%d\n",isReady);

//取温度相关的值

	ret=UK_Read_hts221(0x32,1,&T0_degC_x8);
	if(ret!=UK_ERR_SUCCESS){
		pthread_mutex_unlock(&i2c_devlock);
		return UK_ERR_FAIL;
	}

	ret=UK_Read_hts221(0x33,1,&T1_degC_x8);
	if(ret!=UK_ERR_SUCCESS){
		pthread_mutex_unlock(&i2c_devlock);
		return UK_ERR_FAIL;
	}

	ret=UK_Read_hts221(0x35,1,&T1_T0msb);
	if(ret!=UK_ERR_SUCCESS){
		pthread_mutex_unlock(&i2c_devlock);
		return UK_ERR_FAIL;
	}
	ret=UK_Read_hts221(0x3C,2,&T0_OUT);
	if(ret!=UK_ERR_SUCCESS){
		pthread_mutex_unlock(&i2c_devlock);
		return UK_ERR_FAIL;
	}
	ret=UK_Read_hts221(0x3E,2,&T1_OUT);
	if(ret!=UK_ERR_SUCCESS){
		pthread_mutex_unlock(&i2c_devlock);
		return UK_ERR_FAIL;	
	}

	ret=UK_Read_hts221(0x2A,2,&T_OUT);
	if(ret!=UK_ERR_SUCCESS){
		pthread_mutex_unlock(&i2c_devlock);
		return UK_ERR_FAIL;	
	}
	if(T1_OUT==T0_OUT){
		pthread_mutex_unlock(&i2c_devlock);
		return UK_ERR_FAIL;
	}

	T0_degC = (T0_degC_x8 + (1 << 8) * (T1_T0msb & 0x03)) / 8.0;
	T1_degC = (T1_degC_x8 + (1 << 6) * (T1_T0msb & 0x0C)) / 8.0; 
	T_DegC  = T0_degC + (T_OUT - T0_OUT) * (T1_degC - T0_degC) / (T1_OUT - T0_OUT); 
//取湿度相关的值
	
	ret=UK_Read_hts221(0x30,1,&H0_rH_x2);
	if(ret!=UK_ERR_SUCCESS){
		pthread_mutex_unlock(&i2c_devlock);
		return UK_ERR_FAIL;
	}
	
	ret=UK_Read_hts221(0x31,1,&H1_rH_x2);
	if(ret!=UK_ERR_SUCCESS){
		pthread_mutex_unlock(&i2c_devlock);
		return UK_ERR_FAIL;
	}
	
	ret=UK_Read_hts221(0x36,2,&H0_OUT);
	if(ret!=UK_ERR_SUCCESS){
		pthread_mutex_unlock(&i2c_devlock);
		return UK_ERR_FAIL;
	}
	
	ret=UK_Read_hts221(0x3A,2,&H1_OUT);
	if(ret!=UK_ERR_SUCCESS){
		pthread_mutex_unlock(&i2c_devlock);
		return UK_ERR_FAIL; 
	}

	ret=UK_Read_hts221(0x28,2,&H_OUT);
	if(ret!=UK_ERR_SUCCESS){
		pthread_mutex_unlock(&i2c_devlock);
		return UK_ERR_FAIL;
	}

	if(H1_OUT==H0_OUT){
		pthread_mutex_unlock(&i2c_devlock);
		return UK_ERR_FAIL;
	}

	H0_rH = H0_rH_x2/2.0; 
	H1_rH = H1_rH_x2/2.0; 
	H_rH  = H0_rH + (H_OUT - H0_OUT) * (H1_rH - H0_rH) / (H1_OUT - H0_OUT); 

	sensor->temperature=T_DegC;
	sensor->humidity=H_rH;
	pthread_mutex_unlock(&i2c_devlock);
	return UK_ERR_SUCCESS;
#else
	sensor->temperature=0;
	sensor->humidity=0;
	return UK_ERR_SUCCESS;
#endif
}

UK_STATUS UK_Read_Ex_Version(int index,ex_version * val)
{
	if(val==NULL)
		return UK_ERR_FAIL;
	if(shared->version_ex[index].date==0||shared->version_ex[index].version==0)
		return UK_ERR_NONEXISTS;
	if(shared->version_ex[index].hasdev<=0){
		shared->version_ex[index].date=0;
		shared->version_ex[index].version=0;	
	}else
		shared->version_ex[index].hasdev--;
	val->date=shared->version_ex[index].date;
	val->version=shared->version_ex[index].version;
	return UK_ERR_SUCCESS;
} 



UK_STATUS UK_Read_BIOSSEL(int fd,int * sel)
{
	int ret;
	ret=ioctl(fd,UK_GET_BIOSSEL,sel);
	if(ret!=0)
		return UK_ERR_FAIL;		
	return UK_ERR_SUCCESS;
}

UK_STATUS UK_Write_BIOSSEL(int fd,int sel)
{
	int ret;
	ret=ioctl(fd,UK_SET_BIOSSEL,&sel);
	if(ret!=0)
		return UK_ERR_FAIL;	
	return UK_ERR_SUCCESS;
}

UK_STATUS UK_WriteMode(unsigned long serial,unsigned long trsconf)
{
	int ret;
	unsigned long val;
	val=serial;
	val+=trsconf<<16;
	ret=ioctl(gl_hdev_mgr,UK_WRMODE,&val);
	return ret;
}

UK_STATUS UK_ReadMode(unsigned long * serial,unsigned long * trsconf)
{
    int ret; 
    unsigned long val; 
    ret=ioctl(gl_hdev_mgr,UK_RDMODE,&val);
	*serial=val&0xffff;
	*trsconf=(val>>16)&0xffff;
    return ret; 
}

UK_STATUS UK_ReadPPSTIME(int * t)
{
	int ret;
	if(t==NULL)
		return UK_ERR_FAIL;
	ret=ioctl(gl_hdev_mgr,UK_RDPPS,t);
	return ret;	
}


UK_STATUS UK_SetGPIO(int value)
{
	int ret;
	int fd=open("/dev/ukgpio",O_RDWR);
	if(fd<=0)
		return UK_ERR_FAIL;
	ret=ioctl(fd,UK_GPIO_SET,&value);
	return ret;
}

