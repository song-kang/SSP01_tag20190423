

#ifndef UKLIB_HEADER
#define UKLIB_HEADER

#ifdef __cplusplus
extern "C"{
#endif
typedef long UK_STATUS;
typedef struct UK_TIMESTAMP
{
	unsigned long sec;
	unsigned long nsec;
}UK_TIMESTAMP;
typedef struct UK_SENSOR
{
	double temperature;
	double humidity;
}UK_SENSOR;



typedef unsigned long u32;

typedef struct UK_FPOWER
{
	unsigned short tx_power;
	unsigned short rx_power;
}UK_FPOWER;

typedef struct tagNetStat{
	unsigned long      linkstate;
}UK_NetStat,*PUK_NetStat;

typedef struct tagEthStat{
	unsigned long linkstatus;
	unsigned long fincage;
	UK_FPOWER power;
}UK_EthStat,*PUK_EthStat;

//扩展单元的版本
typedef struct _ex_version_{
	unsigned long date;
	unsigned long version;
	unsigned long hasdev;
}ex_version;


#define K_PCIE_MAGIC  'K'

#define UK_GETDEVLIST  	 	     _IOR(K_PCIE_MAGIC,	 	0, char *)
#define UK_DEVSTART  		     _IO(K_PCIE_MAGIC,	 	1)
#define UK_DEVSTOP   		     _IO(K_PCIE_MAGIC,    	2)
#define UK_GETSTATINFO           _IOR(K_PCIE_MAGIC,   	3, int *)
#define UK_RDMODE			     _IOWR(K_PCIE_MAGIC,  	4, int *)
#define UK_WRMODE			     _IOWR(K_PCIE_MAGIC,  	5, int *)
#define UK_RDWRITEPTR			 _IOR(K_PCIE_MAGIC,   	6, int *)
#define UK_PCIEEXIST			 _IO(K_PCIE_MAGIC,    	7)
#define UK_ADDCNT        		 _IOWR(K_PCIE_MAGIC,	8, int *)
#define UK_READCNT	         	 _IOWR(K_PCIE_MAGIC,	9, int *)
#define UK_READNETSTAT	         _IOR(K_PCIE_MAGIC,		10, int *)
#define UK_DO_W					 _IOWR(K_PCIE_MAGIC, 	11, int *)
#define UK_DO_R  	 			 _IOWR(K_PCIE_MAGIC, 	12, int *)
#define UK_DI  	 			     _IOWR(K_PCIE_MAGIC, 	13, int *)
#define UK_LED_W  	 		     _IOWR(K_PCIE_MAGIC, 	14, int *)
#define UK_LED_R  	 		     _IOWR(K_PCIE_MAGIC, 	15, int *)
#define UK_SEC_W  	 		     _IOWR(K_PCIE_MAGIC, 	16, int *)
#define UK_SEC_R  	 		     _IOWR(K_PCIE_MAGIC, 	17, int *)
#define UK_NSEC_R  	 		     _IOWR(K_PCIE_MAGIC, 	18, int *)
#define UK_SYNCT		         _IOWR(K_PCIE_MAGIC, 	19, int *)
#define UK_IGB_STAT			     _IOWR(K_PCIE_MAGIC, 	20, int *)
#define UK_GETEVENT			     _IOWR(K_PCIE_MAGIC, 	21, int *)
#define UK_SYNC_STATUS		     _IOWR(K_PCIE_MAGIC, 	22, int *)
#define UK_RESETTIME             _IOWR(K_PCIE_MAGIC, 	23, int *)
#define UK_WDT_EN        	     _IOWR(K_PCIE_MAGIC, 	24, int *)
#define UK_WDT_DIS        	     _IOWR(K_PCIE_MAGIC, 	25, int *)	
#define UK_WDT_FEED        	     _IOWR(K_PCIE_MAGIC, 	26, int *)
#define UK_POWERSTAT        	 _IOWR(K_PCIE_MAGIC, 	27, int *)
#define UK_GPIO_SET_LED        	 _IOWR(K_PCIE_MAGIC, 	28, int *)
#define UK_GPIO_GET_LED        	 _IOWR(K_PCIE_MAGIC, 	29, int *)
#define UK_GET_BIOSSEL           _IOWR(K_PCIE_MAGIC, 	30, int *)
#define UK_SET_BIOSSEL           _IOWR(K_PCIE_MAGIC, 	31, int *)
#define UK_IGB_INTEN			 _IOWR(K_PCIE_MAGIC, 	32, int *)
#define UK_IGB_PAR				 _IOWR(K_PCIE_MAGIC, 	33, int *)
#define UK_IGB_INTDIS			 _IOWR(K_PCIE_MAGIC, 	34, int *)
#define UK_POWEROFF				 _IOWR(K_PCIE_MAGIC, 	35, int *)
#define UK_INT_ENA_W			 _IOWR(K_PCIE_MAGIC, 	36, int *)
#define UK_INT_EDGE_W			 _IOWR(K_PCIE_MAGIC, 	37, int *)
#define UK_POWERON				 _IOWR(K_PCIE_MAGIC, 	38, int *)
#define UK_RDPPS                 _IOWR(K_PCIE_MAGIC,    39, int *)
#define K_PCIE_MAXNR             40

#define UK_GPIO_MAGIC  'G'
#define UK_GPIO_SET     _IOWR(UK_GPIO_MAGIC,1, unsigned long *)


#define UK_ERR_SUCCESS       0
#define UK_ERR_NODEVS       -1
#define UK_ERR_EXISTS       -2
#define UK_ERR_NONEXISTS    -3
#define UK_ERR_NOMEM        -4
#define UK_ERR_DEVSTART     -5
#define UK_ERR_DEVSTOP      -6
#define UK_ERR_THREAD_FAIL  -7
#define UK_ERR_THREAD_EXIT  -8
#define UK_ERR_INPROGRESS   -9
#define UK_ERR_INVALIDCMD   -10
#define UK_ERR_FAIL         -11
#define UK_ERR_INVHANDLE    -12
#define UK_ERR_FILEFULL     -13
#define UK_ERR_FILESEEK     -14
#define UK_ERR_FILEMAP      -15
#define UK_ERR_INVARG       -16
#define UK_ERR_VERSION      -17

#define UK_TRUE  1
#define UK_FALSE 0
#define LED_OFF     0
#define LED_ON      1
#define LED_TOGGLE  2
#define DO_OFF   1
#define DO_ON    0



/*************************************
函数名	: UK_getNetList
描述    	: 得到光口的设备列表
参数        
pName	: 存储使用":"隔开的设备列表的buffer
buflen	: buffer的长度
返回值	
UK_ERR_NODEVS	: 没有设备
大于0的值		: 设备数量
 **************************************/
int UK_getNetList(char *pName,int buflen);


/*************************************
函数名	: UK_regNetCallback
描述     	: 注册一个设备回调函数，该函数会在光口有数据时被回调
参数 
devname			: 设备名
UK_NetCallBack	: 设备回调函数，原型如下
UK_STATUS (*UK_NetCallBack)(unsigned char *pNetBuf,int* plen,UK_STATUS status,void *pContext);
pContext		      	: 要传递给回调函数的数据
timeout			: 超时时间
返回值     
UK_ERR_EXISTS   : 该设备已经被注册过了
UK_ERR_SUCCESS: 注册成功 
 **************************************/
typedef UK_STATUS (*UK_NetCallBack)(int totallen,UK_STATUS status,void *pContext);
UK_STATUS UK_regNetCallback(char *devname,UK_NetCallBack pFunc,void *pContext,unsigned long timeout);


/*************************************
函数名	: UK_unregNetCallback
描述		: 取消原来的注册设备回调函数
参数 
devname			: 设备名
UK_NetCallBack	: 要取消的设备回调函数，原型如下
UK_STATUS (*UK_NetCallBack)(unsigned char *pNetBuf,int* plen,UK_STATUS status,void *pContext);
pContext		      	: 要传递给回调函数的数据
返回值
UK_ERR_NONEXISTS	: 该设备没有注册此设备回调函数
UK_ERR_SUCCESS	: 取消成功 
 **************************************/
UK_STATUS UK_unregNetCallback(char *devname,UK_NetCallBack pFunc,void *pContext);


/*************************************
函数名	: UK_NetCaptureStart
描述		: 启动收包	
返回值:     
-1					:  启动失败
UK_ERR_SUCCESS	:  启动成功
 **************************************/
UK_STATUS UK_NetCaptureStart(void);


/*************************************
函数名	: UK_ReadFile
描述		: 读光口数据
参数
devname	: 要读取的设备名
buffer	: 存放读到的数据的位置
size		: buffer的长度
return:     
0	: 没读到数据
>0  	: 返回值为读到的数据长度
 **************************************/
long  UK_ReadFile(char *devname,void * buffer,int size);


/*************************************
函数名	: UK_NetCaptureStop
描述	 	: 停止收包
返回值     
UK_ERR_SUCCESS: 关闭成功 
 **************************************/
UK_STATUS UK_NetCaptureStop(void);


/*************************************
函数名	: UK_dido_setdo
描述	 	: 设置开出的状态
参数
val	: 将开出状态设置成val(当val等于DO_OFF时，开出将断开；当val等于DO_ON时，开出将闭合)
num	: 要设置的开出
返回值   
UK_ERR_SUCCESS: 设置成功
 **************************************/
int UK_dido_setdo(char val,char num);


/*************************************
函数名	: UK_read_di
描述	 	: 读取开入的状态
参数
val	: 指针，开入的状态将放入它指向的内存中
返回值   
UK_ERR_SUCCESS: 读取成功
 **************************************/
int UK_read_di(char *val);


/*************************************
函数名	: UK_EnableEvent
描述	 	: 使能/禁止开入中断，同时在使能的时候设备是上升沿还是下降沿有效
参数
ena		: 0:禁止；1:使能
edge	: 0:正边沿有效；1:负边沿有效
返回值   
UK_ERR_SUCCESS: 操作成功
 **************************************/
int UK_EnableEvent(int ena,int edge);


/*************************************
函数名	: UK_CheckDIEvent
描述	 	: 阻塞进程直到开入中断发生或者超时
参数
event	: 存储中断类型的buffer
timeout	: 超时时间
返回值   
UK_ERR_SUCCESS: 成功
 **************************************/
int UK_CheckDIEvent(void * event,int timeout);


/*************************************
函数名	: UK_UsrLed
描述	 	: 设置自定义灯
参数
bit		:0~7 表示指定索引的灯， -1 表示所有灯
status	:当 bit为 0~7时， status 有三个值LED_ON表示打开， LED_OFF表示关闭，
LED_TOGGLE 表示取反，当 bit 为-1 时，每一个 bit 表示对应的状态
返回值
UK_ERR_INVARG 		: 非法参数
UK_ERR_SUCCESS	: 成功
 **************************************/
UK_STATUS UK_UsrLed(unsigned int bit,unsigned long status);

#define LED_FAULT 0
#define LED_RUN   1
/*************************************
函数名  : UK_UsrLed_GPIO
描述        : 设置由GPIO控制的两个自定义灯
参数
bit     :
LED_FAULT	故障灯
LED_RUN		运行灯
status  :
LED_ON表示打开
LED_OFF表示关闭
返回值
UK_ERR_INVARG       : 非法参数
UK_ERR_SUCCESS  : 成功
 **************************************/
UK_STATUS UK_UsrLed_GPIO(int bit,unsigned long status);



/*************************************
函数名	: IRIGB_DeviceOpen
描述	 	: 打开B码设备
返回值
大于0的值表示文件句柄
等于0的值表示打开失败
 **************************************/
int IRIGB_DeviceOpen();


/*************************************
函数名	: IRIGB_DeviceClose
描述	 	: 关闭B码设备
参数
fd	: 通过IRIGB_DeviceOpen获取的文件句柄
 **************************************/
void IRIGB_DeviceClose(int fd);


/*************************************
函数名	: IRIGB_EnableDevice
描述	 	: 使能B码设备
参数
fd	: 通过IRIGB_DeviceOpen获取的文件句柄
 **************************************/
int IRIGB_EnableDevice(int fd);


/*************************************
函数名	: IRIGB_DisableDevice
描述	 	: 禁止B码设备
参数
fd	: 通过IRIGB_DeviceOpen获取的文件句柄
 **************************************/
int IRIGB_DisableDevice(int fd);


/*************************************
函数名	: IRIGB_SyncTime
描述	 	: 获取对时状态
参数
fd	: 通过IRIGB_DeviceOpen获取的文件句柄
返回值
当时的对时状态
 **************************************/
int IRIGB_SyncTime(int fd);

/*************************************
函数名	: IRIGB_GetPTPState
描述	 	: 获取信号状态
参数
fd	: 通过IRIGB_DeviceOpen获取的文件句柄
返回值
当时的信号状态
 **************************************/
int IRIGB_GetPTPState(int fd);


/*************************************
函数名	: IRIGB_GetTime
描述	 	: 从对时的fpga中获取当时的精确时间
参数
fd	: 通过IRIGB_DeviceOpen获取的文件句柄
返回值
当时的精确时间
 **************************************/
UK_TIMESTAMP IRIGB_GetTime(int fd);

/*************************************
函数名	: IRIGB_SetTime
描述	 	: 设置对时fpga的时间
参数
fd	: 通过IRIGB_DeviceOpen获取的文件句柄
sec	: 要设置的时间
返回值
UK_ERR_FAIL		: 设置失败
UK_ERR_SUCCESS 	: 设置成功
 **************************************/
UK_STATUS IRIGB_SetTime(int fd,unsigned long sec);


/*************************************
函数名	: IRIGB_ResetTime
描述	 	: 对对时进行人工复位
参数
fd	: 通过IRIGB_DeviceOpen获取的文件句柄
返回值
UK_ERR_FAIL		: 设置失败
UK_ERR_SUCCESS 	: 设置成功
 **************************************/
UK_STATUS IRIGB_ResetTime(int fd);


//add by caowei
//time 5s 10s 15s 20s 25s 30s
/*************************************
函数名	: UK_Wdt_Enable
描述	 	: 使能看门狗并设置超时时间
参数
time	: 看门狗的超时时间
返回值
UK_ERR_FAIL		: 设置失败
UK_ERR_SUCCESS 	: 设置成功
 **************************************/
UK_STATUS UK_Wdt_Enable(unsigned int time);


/*************************************
函数名	: UK_Wdt_Disable
描述	 	: 禁止看门狗
返回值
UK_ERR_FAIL		: 设置失败
UK_ERR_SUCCESS 	: 设置成功
 **************************************/
UK_STATUS UK_Wdt_Disable();


/*************************************
函数名	: UK_Wdt_FeedDog
描述	 	: 喂狗
返回值
UK_ERR_FAIL		: 喂狗失败
UK_ERR_SUCCESS 	: 喂狗成功
 **************************************/
UK_STATUS UK_Wdt_FeedDog();


/*************************************
函数名	: UK_getOptions
描述	 	: 读取光口的统计信息(光口的连接状态和光功率)
参数		
idx			: 光口号
pBuf		: 存储获取到的信息的内存单元
plen			: pBuf的长度
返回值
UK_ERR_FAIL	 	: 获取失败
UK_ERR_SUCCESS  	: 获取成功

 **************************************/
UK_STATUS UK_getOptions(int idx,unsigned char *pBuf,int *plen);


/*************************************
函数名	: UK_Read_Ethstat
描述	 	: 读取光口的统计信息(现在只有光口的连接状态)
参数		
idx			: 光口号
返回值
UK_NetStat	 	: 获取到的状态值

 **************************************/
UK_NetStat UK_Read_Ethstat(int idx);


/*************************************
函数名	: UK_Read_Digital_Sensor
描述	 	: 读取温湿度传感器的值
参数		
sensor		: UK_SENSOR类型的指针，用来返回当时的温湿度值
返回值
UK_ERR_FAIL	 	: 获取失败
UK_ERR_SUCCESS  	: 获取成功

 **************************************/
UK_STATUS UK_Read_Digital_Sensor(UK_SENSOR * sensor);


/*************************************
函数名	: UKPower_regCallback
描述     	: 注册一个回调函数，该函数会在设备掉电的情况下回调
参数 
fd					: 文件句柄
UKPower_CallBack	: 回调函数，原型如下
UK_STATUS (*UKPower_CallBack)(void *pContext);
pContext		      	: 要传递给回调函数的数据
返回值     
UK_ERR_EXISTS   : 该设备已经被注册过了
UK_ERR_SUCCESS	: 注册成功 
 **************************************/
typedef UK_STATUS (*UKPower_CallBack)(void *pContext);
UK_STATUS UKPower_regCallback(int fd,UKPower_CallBack pFunc,void *pContext);

/*************************************
函数名  : 	UK_ReadPowerState
描述        : 读取当前的电源状态 
返回值     
-1   		: 读取失败 
0,1,2,3  	: 电源状态 
**************************************/ 
int UK_ReadPowerState();

/*************************************
函数名	: UKPower_unregCallback
描述		: 取消原来注册的回调函数
参数 
fd					: 文件句柄
返回值
UK_ERR_NONEXISTS	: 该设备没有注册此设备回调函数
UK_ERR_SUCCESS		: 取消成功 
 **************************************/
UK_STATUS UKPower_unregCallback(int fd);


/*************************************
函数名	: UK_Read_Ex_Version
描述		: 取消扩展板的版本号
参数 
index					: 扩展口号
val						: 用来返回扩展板的FPGA版本信息
返回值
UK_ERR_NONEXISTS	 	: 没有这个扩展板的FPGA版本信息
UK_ERR_FAIL			: 参数二的地址为NULL
UK_ERR_SUCCESS 	 	: 取得成功 
 **************************************/
UK_STATUS UK_Read_Ex_Version(int index,ex_version * val);


UK_STATUS UK_Read_BIOSSEL(int fd,int * sel);

UK_STATUS UK_Write_BIOSSEL(int fd,int sel);

int IRIGB_Parity(int fd, int en);

UK_STATUS UK_WriteMode(unsigned long serial,unsigned long trsconf);

UK_STATUS UK_ReadMode(unsigned long * serial,unsigned long * trsconf);

UK_STATUS UK_ReadPPSTIME(int * t);

UK_STATUS UK_SetGPIO(int value);


#ifdef __cplusplus
}
#endif
#endif
