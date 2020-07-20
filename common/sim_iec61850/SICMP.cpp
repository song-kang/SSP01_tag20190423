#include "SICMP.h"
#include "SLog.h"

int SICMP::nId = 1;
int SICMP::nSeq = 0; 
unsigned short checksum(unsigned short* buffer, int size)
{
	unsigned long cksum = 0;
	while(size>1)
	{
		cksum += *buffer++;
		size -= sizeof(u_short);
	}
	if(size)
	{
		cksum += *(UCHAR*)buffer;
	}
	cksum = (cksum>>16) + (cksum&0xffff); 
	cksum += (cksum>>16); 
	return (u_short)(~cksum);

} 
SICMP::SICMP(void)
{
	
}


SICMP::~SICMP(void)
{
}


bool SICMP::initSocket()
{
	SSocket skt;
	if(!skt.Create(AF_INET, SOCK_RAW, IPPROTO_ICMP))
	{
		LOGWARN("create socket failed");
		return false;
	}
	skt.SetTimeout(TIMEOUT,TIMEOUT);
#if 0
	// Send the echo request  				
	struct    sockaddr_in saDest;
	saDest.sin_addr.s_addr = inet_addr(sIp);
	saDest.sin_family = AF_INET;
	saDest.sin_port = 0;
	nRet = skt.SendTo(saDest,&echoReq,sizeof(ECHOREQUEST));
#endif
	return true;
}

int SICMP::doPing(char *sIp,int &ms,int &ttl)
{
	//Create(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	SSocket skt;
	if(!skt.Create(AF_INET, SOCK_RAW, IPPROTO_ICMP))
		return 1001;
	//QMessageBox(this,"警告","create ok");
	printf("create ok-------------\n");
	skt.SetTimeout(1000,1000);
	ICMP_ECHOQUEST echoReq;
	//static int nId = 1;
	//static int nSeq = 1;
	int nRet;

	// Fill in echo request
	echoReq.icmpHdr.Type		= TYPE_ICMP_ECHOREQ;
	echoReq.icmpHdr.Code		= 0;
	echoReq.icmpHdr.Checksum	= 0;
	echoReq.icmpHdr.ID			= nId++;
	echoReq.icmpHdr.Seq			= nSeq++;
#if 0
	// Fill in some data to send
	for (nRet = 0; nRet < ICMP_REQ_DATASIZE; nRet++)
		echoReq.cData[nRet] = ' '+nRet;
#endif
	// Save tick count when sent
	int soc1,usec1,soc2,usec2;
	SDateTime::getSystemTime(soc1,usec1);
	//echoReq.dwTime				= usec1;

	// Put data in packet and compute checksum
	echoReq.icmpHdr.Checksum = in_cksum((u_short *)&echoReq, sizeof(ICMP_ECHOQUEST));

	// Send the echo request  				
	struct    sockaddr_in saDest;
	saDest.sin_addr.s_addr = inet_addr(sIp);
	saDest.sin_family = AF_INET;
	saDest.sin_port = 0;
	nRet = skt.SendTo(saDest,&echoReq,sizeof(ICMP_ECHOQUEST));
	if (nRet != sizeof(ICMP_ECHOQUEST)) 
	{
		return 1002;
	}
	//QMessageBox(this,"警告","send ok");
	printf("send ok-------------\n");
	if(skt.CheckForRecv() < 1)
	{
		return 1003;
	}
	ICMP_ECHO_REPLY echoReply;
	sockaddr_in in;
	int ireplysize = sizeof(ICMP_ECHO_REPLY);
	nRet = skt.RecvFrom(&echoReply,sizeof(ICMP_ECHO_REPLY),in);
	if(nRet <= 0)
	{
		return 1004;
	}
	//QMessageBox(this,"警告","recv ok");
	printf("recv ok-------------\n");
	DECODE_RESULT dst;
	//DecodeIcmpResponse((char*)&echoReply,sizeof(ICMP_ECHO_REPLY),dst);
	DecodeIcmpEchoResponse(&echoReply,sizeof(ICMP_ECHO_REPLY),dst);
	SDateTime::getSystemTime(soc2,usec2);
	ttl = echoReply.ipHdr.ttl;
	ms = (soc2-soc1)*1000+(usec2-usec1)/1000;
	//QMessageBox(this,"警告","doping end");
	printf("doping ok-------------\n");
	return 0;
}

u_short SICMP::in_cksum(u_short *addr, int len)
{
	register int nleft = len;
	register u_short *w = addr;
	register u_short answer;
	register int sum = 0;

	/*
	 *  Our algorithm is simple, using a 32 bit accumulator (sum),
	 *  we add sequential 16 bit words to it, and at the end, fold
	 *  back all the carry bits from the top 16 bits into the lower
	 *  16 bits.
	 */
	while( nleft > 1 )  {
		sum += *w++;
		nleft -= 2;
	}

	/* mop up an odd byte, if necessary */
	if( nleft == 1 ) {
		u_short	u = 0;

		*(u_char *)(&u) = *(u_char *)w ;
		sum += u;
	}

	/*
	 * add back carry outs from top 16 bits to low 16 bits
	 */
	sum = (sum >> 16) + (sum & 0xffff);	/* add hi 16 to low 16 */
	sum += (sum >> 16);			/* add carry */
	answer = ~sum;				/* truncate to 16 bits */
	return (answer);
}

int SICMP::doTimestamp(char *sIp,int &ms,int &ttl)
{
	SSocket skt;
	if(!skt.Create(AF_INET, SOCK_RAW, IPPROTO_ICMP))
		return 1001;
	skt.SetTimeout(1000,1000);
	ICMP_TIMESTAMP timestamp = {0};
	//static int nId = 1;
	//static int nSeq = 1;
	int nRet;

	// Fill in echo request
	timestamp.icmpHdr.Type		= TYPE_ICMP_TIMESTAMP;
	timestamp.icmpHdr.Code		= 0;
	timestamp.icmpHdr.Checksum	= 0;
	timestamp.icmpHdr.ID			= nId++;
	timestamp.icmpHdr.Seq			= nSeq++;
#if 0
	// Fill in some data to send
	for (nRet = 0; nRet < ICMP_REQ_DATASIZE; nRet++)
		timestamp.cData[nRet] = ' '+nRet;
#endif
	// Save tick count when sent
	int soc1,usec1,soc2,usec2;
	SDateTime::getSystemTime(soc1,usec1);
	//echoReq.dwTime				= usec1;
	timestamp.originate_time_stamp = soc1;
	
	// Put data in packet and compute checksum
	timestamp.icmpHdr.Checksum = in_cksum((u_short *)&timestamp, sizeof(ICMP_TIMESTAMP));
	//timestamp.originate_time_stamp = 0;
	//timestamp.receive_time_stamp = 0;
	//timestamp.transmit_time_stamp = 0;
#if 0
	STime curTime = SDateTime::currentDateTime().time();
	unsigned int ISec = (unsigned int)(curTime.hour()*60*60*1000+curTime.minute()*60*1000+ curTime.usec()*1000+curTime.msec());
	timestamp.originate_time_stamp = ISec;
#endif
#if 0
	SYSTEMTIME stUTC;  
	::GetSystemTime(&stUTC); 
	//timestamp.originate_time_stamp = ((stUTC.wHour*60 + stUTC.wMinute)*60 + stUTC.wSecond)*1000+stUTC.wMilliseconds;
	unsigned int ISec = stUTC.wHour*60*60*1000+stUTC.wMinute*60*1000+ stUTC.wSecond*1000+stUTC.wMilliseconds;
	timestamp.originate_time_stamp = ISec;
#endif
	// Send the echo request  				
	struct    sockaddr_in saDest;
	saDest.sin_addr.s_addr = inet_addr(sIp);
	saDest.sin_family = AF_INET;
	saDest.sin_port = 0;
	nRet = skt.SendTo(saDest,&timestamp,sizeof(ICMP_TIMESTAMP));
	if (nRet != sizeof(ICMP_TIMESTAMP)) 
	{
		return 1002;
	}
	if(skt.CheckForRecv() < 1)
	{
		return 1003;
	}
	ICMP_TIMESTAMP_REPLY timestampReply;
	sockaddr_in in;
	nRet = skt.RecvFrom(&timestampReply,sizeof(ICMP_TIMESTAMP_REPLY),in);
	if(nRet <= 0)
	{
		return 1004;
	}
	DECODE_RESULT dst;
	//DecodeIcmpResponse((char*)&timestampReply,sizeof(ICMP_TIMESTAMP_REPLY),dst);
	DecodeIcmpTimestampResponse(&timestampReply,sizeof(ICMP_TIMESTAMP_REPLY),dst);
	SDateTime::getSystemTime(soc2,usec2);
	ttl = timestampReply.ipHdr.ttl;
	ms = (soc2-soc1)*1000+(usec2-usec1)/1000;	
	return 0;
}

//解码得到的数据报
bool SICMP::DecodeIcmpResponse(char* pBuf, int iPacketSize, DECODE_RESULT& stDecodeResult)
{
	//检查数据报大小的合法性
	IP_HEADER* pIpHdr = (IP_HEADER*)pBuf;
	int iIpHdrLen = pIpHdr->h_lenver * 4;
	if (iPacketSize < (int)(iIpHdrLen+sizeof(ICMP_HEADER)))
		return false;
	//按照ICMP包类型检查id字段和序列号以确定是否是程序应接收的Icmp包
	ICMP_HEADER* pIcmpHdr = (ICMP_HEADER*)(pBuf+iIpHdrLen);
	USHORT usID, usSquNo;
	unsigned short iiType = (int)pIcmpHdr->Type;
	if (pIcmpHdr->Type == TYPE_ICMP_ECHOREPLY)
	{
		usID = pIcmpHdr->ID;
		usSquNo = pIcmpHdr->Seq;
	}
	else if(pIcmpHdr->Type == TYPE_ICMP_TIME_EXCEEDED)//传输超时 
	{
		char* pInnerIpHdr = pBuf+iIpHdrLen+sizeof(ICMP_HEADER);  //载荷中的IP头
		int iInnerIPHdrLen = ((IP_HEADER*)pInnerIpHdr)->h_lenver * 4;//载荷中的IP头长
		ICMP_HEADER* pInnerIcmpHdr = (ICMP_HEADER*)(pInnerIpHdr+iInnerIPHdrLen);//载荷中的ICMP头
		usID = pInnerIcmpHdr->ID;
		usSquNo = pInnerIcmpHdr->Seq;
	}
	else
		return false;
#if 0
	if (usID != (USHORT)GetCurrentProcessId() || usSquNo !=stDecodeResult.usSeqNo) 
		return FALSE;
#endif
	if (pIcmpHdr->Type == TYPE_ICMP_ECHOREPLY)
	{
		ICMP_ECHO_REPLY* pechoReply = (ICMP_ECHO_REPLY*)pBuf;
		//unsigned int iTime = pechoReply->echoRequest.dwTime;
	} else if (pIcmpHdr->Type == TYPE_ICMP_TIME_EXCEEDED)
	{
		ICMP_TIMESTAMP_REPLY* timeReply = (ICMP_TIMESTAMP_REPLY*)pBuf;
		unsigned int iTime = timeReply->timestamp.originate_time_stamp;
		unsigned int iTime1 = timeReply->timestamp.receive_time_stamp;
		unsigned int iTime2 = timeReply->timestamp.transmit_time_stamp;
	}
	//处理正确收到的ICMP数据报
	if (pIcmpHdr->Type == TYPE_ICMP_ECHOREPLY ||
		pIcmpHdr->Type == TYPE_ICMP_TIME_EXCEEDED)
	{
		//返回解码结果
		stDecodeResult.dwIPaddr.s_addr = pIpHdr->sourceIP;
		stDecodeResult.dwRoundTripTime = GetTickCount()-stDecodeResult.dwRoundTripTime;
#if 0
		//打印屏幕信息
		if (stDecodeResult.dwRoundTripTime)
			cout << setw(6) << stDecodeResult.dwRoundTripTime << " ms" << flush;
		else
			cout << setw(6) << "<1" << " ms" << flush;
		return TRUE;
#endif
	}
	return FALSE;
}
bool SICMP::DecodeIcmpEchoResponse(ICMP_ECHO_REPLY* pReply, int iPacketSize, DECODE_RESULT& stDecodeResult)
{
	char destIP[64];
	in_addr destAddr;
	destAddr.s_addr = pReply->ipHdr.destIP;
	//u_long longDest = (u_long)pReply->ipHdr.destIP;
	//destAddr = *(in_addr*)&longDest;
	strcpy( destIP, inet_ntoa(destAddr));
	char sourceIP[64];
	in_addr sourceAddr;
	u_long longSource = htonl((u_long)pReply->ipHdr.sourceIP);
	sourceAddr = *(in_addr*)&longSource;
	strcpy( sourceIP, (char*)inet_ntoa( sourceAddr ));
	//strcpy( sourceIP, (char*)inet_ntoa( htonl( (u_long)pReply->ipHdr.sourceIP ) ) );

	//unsigned int iTime = pReply->echoRequest.dwTime;
	int soc1,usec1,soc2,usec2;
	SDateTime::getSystemTime(soc1,usec1);
	time_t socNow = SDateTime::getNowSoc();
	long lTime = socNow;
	return true;
}

bool SICMP::DecodeIcmpTimestampResponse(ICMP_TIMESTAMP_REPLY* pReply, int iPacketSize, DECODE_RESULT& stDecodeResult)
{
	char destIP[64];
	in_addr destAddr;
	destAddr.s_addr = pReply->ipHdr.destIP;
	//u_long longDest = (u_long)pReply->ipHdr.destIP;
	//destAddr = *(in_addr*)&longDest;
	strcpy( destIP, inet_ntoa(destAddr));
	char sourceIP[64];
	in_addr sourceAddr;
	u_long longSource = htonl((u_long)pReply->ipHdr.sourceIP);
	sourceAddr = *(in_addr*)&longSource;
	strcpy( sourceIP, (char*)inet_ntoa( sourceAddr ));
	//strcpy( sourceIP, (char*)inet_ntoa( htonl( (u_long)pReply->ipHdr.sourceIP ) ) );
	if (pReply->timestamp.icmpHdr.Type == TYPE_ICMP_TIMESTAMP_REPLY)
	{
		printf("返回的消息类型正确");
	}
	unsigned int ioTime = pReply->timestamp.originate_time_stamp;
	unsigned int irTime = pReply->timestamp.receive_time_stamp;
	unsigned int itTime = pReply->timestamp.transmit_time_stamp;

	int soc1,usec1,soc2,usec2;
	SDateTime::getSystemTime(soc1,usec1);
	STime;
	time_t socNow = SDateTime::getNowSoc();
	long lTime = socNow;
	return FALSE;
}
#if 1
#ifdef _WIN32
static bool g_bWinSockInit=false;
#endif
bool SICMP::CreateSocket(int af, int type, int protocol)
{
	LOGBASEDEBUG("into SSocket's Create(), this=%p",this);
	m_iLastError = 0;
	CloseSocket();
	if(type != SOCK_STREAM)
		m_bTcp = false;
#ifdef _WIN32
	if(!g_bWinSockInit)
	{
		WSADATA WSAData;
		WSAStartup( MAKEWORD( 1, 1 ), &WSAData );
		g_bWinSockInit = true;
	}
#endif
	m_iAF = af;
	m_iSocketType = type;
	m_hSocket = ::socket(af,type,protocol);

	if(m_hSocket == INVALID_SOCKET)
	{
#ifdef WIN32
		m_iLastError = WSAGetLastError();
#else
		m_iLastError = errno;
#endif
		LOGWARN("error in SSocket's Create() when socket, this=%p, errno=%d",this,m_iLastError);
		return false;
	}
	if(type != SOCK_STREAM)
		return true;
	int bNodelay = 1;  
	int err = setsockopt(m_hSocket,IPPROTO_TCP,TCP_NODELAY,(char *)&bNodelay,sizeof(bNodelay));//不采用延时算法 
	if ( err == SOCKET_ERROR ) 
	{
#ifdef WIN32
		m_iLastError = WSAGetLastError();
#else
		m_iLastError = errno;
#endif
		LOGWARN("error in SSocket's Create() when setsockopt, this=%p, handle=%X,errno=%d",
			this, m_hSocket, m_iLastError);
		CloseSocket();
		return false;       
	}

	int   keepAlive   =   1;//设定KeepAlive   
	//	int   keepIdle   =   5;//开始首次KeepAlive探测前的TCP空闭时间   
	//	int   keepInterval   =   5;//两次KeepAlive探测间的时间间隔   
	//	int   keepCount   =   3;//判定断开前的KeepAlive探测次数   
	err = setsockopt(m_hSocket,SOL_SOCKET,SO_KEEPALIVE,(char*)&keepAlive,sizeof(keepAlive));
	if(err == SOCKET_ERROR)
	{   
#ifdef WIN32
		m_iLastError = WSAGetLastError();
#else
		m_iLastError = errno;
#endif
		LOGWARN("error in SSocket's Create() when SO_KEEPALIVE, this=%p, handle=%X,errno=%d",
			this, m_hSocket, m_iLastError);
		CloseSocket();
	}   
	//设置端口属性
	int iTimeout = 1000;
	if (setsockopt(m_hSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&iTimeout, sizeof(iTimeout)) == SOCKET_ERROR)
	{
#ifdef WIN32
		m_iLastError = WSAGetLastError();
#else
		m_iLastError = errno;
#endif
		LOGWARN("error in SSocket's Create() when SO_KEEPALIVE, this=%p, handle=%X,errno=%d",
			this, m_hSocket, m_iLastError);
		CloseSocket();
	}
	return true;
}

int SICMP::CloseSocket()
{
	LOGBASEDEBUG("into SSocket's Close(),this=%p,handle=%X",this,m_hSocket);

	int ret=0;
	if(m_hSocket != 0)
	{
		ret=shutdown(m_hSocket, 2);
#ifdef WIN32
		closesocket(m_hSocket);
#else
		close(m_hSocket);
#endif
		m_hSocket = 0;
	}
	return ret;
}
#endif

int SICMP::doMask(char *sIp,int &ms,int &ttl)
{
	SSocket skt;
#if 1
	if (!skt.CreateBroadCast())
	{
			return 1001;
	}
#endif
	/*if(!skt.Create(AF_INET, SOCK_RAW, IPPROTO_ICMP))
		return 1001;*/
	skt.SetTimeout(1000,1000);
	ICMP_MASK_QUEST icmpmask = {0};
	//static int nId = 1;
	//static int nSeq = 1;
	int nRet;

	// Fill in echo request
	icmpmask.icmpHdr.Type		= TYPE_ICMP_MASK_REQUEST;
	icmpmask.icmpHdr.Code		= 0;
	icmpmask.icmpHdr.Checksum	= 0;
	icmpmask.icmpHdr.ID			= nId++;
	icmpmask.icmpHdr.Seq			= nSeq++;

	// Put data in packet and compute checksum
	icmpmask.icmpHdr.Checksum = in_cksum((u_short *)&icmpmask, sizeof(ICMP_MASK_QUEST));

	// Send the echo request  				
	struct    sockaddr_in saDest;
	saDest.sin_addr.s_addr = inet_addr(sIp);
	saDest.sin_family = AF_INET;
	saDest.sin_port = htonl(4001);
	nRet = skt.SendTo(saDest,&icmpmask,sizeof(ICMP_MASK_QUEST));
	if (nRet != sizeof(ICMP_MASK_QUEST)) 
	{
		return 1002;
	}
#if 0
	if(skt.CheckForRecv() < 1)
	{
		return 1003;
	}
#endif
	ICMP_MASK_REPLY maskReply;
	sockaddr_in in;
	int i = 10;
	while( i> 0)
	{
		nRet = skt.RecvFrom(&maskReply,sizeof(ICMP_MASK_REPLY),in);
		if(nRet <= 0)
		{
			return 1004;
		}
		DECODE_RESULT dst;
		//DecodeIcmpResponse((char*)&timestampReply,sizeof(ICMP_TIMESTAMP_REPLY),dst);
		DecodeIcmpMaskResponse(&maskReply,sizeof(ICMP_MASK_REPLY),dst);
		i--;
	}
#if 0
	nRet = skt.RecvFrom(&maskReply,sizeof(ICMP_MASK_REPLY),in);
	if(nRet <= 0)
	{
		return 1004;
	}
	DECODE_RESULT dst;
	//DecodeIcmpResponse((char*)&timestampReply,sizeof(ICMP_TIMESTAMP_REPLY),dst);
	DecodeIcmpMaskResponse(&maskReply,sizeof(ICMP_MASK_REPLY),dst);
#endif
	/*SDateTime::getSystemTime(soc2,usec2);
	ttl = timestampReply.ipHdr.ttl;
	ms = (soc2-soc1)*1000+(usec2-usec1)/1000;*/	
	return 0;
}

bool SICMP::DecodeIcmpMaskResponse(ICMP_MASK_REPLY* pReply, int iPacketSize, DECODE_RESULT& stDecodeResult)
{
	char destIP[64];
	in_addr destAddr;
	destAddr.s_addr = pReply->ipHdr.destIP;
	//u_long longDest = (u_long)pReply->ipHdr.destIP;
	//destAddr = *(in_addr*)&longDest;
	strcpy( destIP, inet_ntoa(destAddr));
	char sourceIP[64];
	in_addr sourceAddr;
	u_long longSource = htonl((u_long)pReply->ipHdr.sourceIP);
	sourceAddr = *(in_addr*)&longSource;
	strcpy( sourceIP, (char*)inet_ntoa( sourceAddr ));

	char maskIP[64];
	in_addr maskAddr;
	maskAddr.s_addr = pReply->maskquest.imask;
	//u_long longDest = (u_long)pReply->ipHdr.destIP;
	//destAddr = *(in_addr*)&longDest;
	strcpy( maskIP, inet_ntoa(maskAddr));
	//strcpy( sourceIP, (char*)inet_ntoa( htonl( (u_long)pReply->ipHdr.sourceIP ) ) );
	if (pReply->maskquest.icmpHdr.Type == TYPE_ICMP_MASK_REPLY)
	{
		printf("返回的消息类型正确");
	}
	
	int soc1,usec1,soc2,usec2;
	SDateTime::getSystemTime(soc1,usec1);
	STime;
	time_t socNow = SDateTime::getNowSoc();
	long lTime = socNow;
	return FALSE;
}

int SICMP::doroute(char *sIp,int &ms,int &ttl)
{
	SSocket skt;
	if(!skt.Create(AF_INET, SOCK_RAW, IPPROTO_ICMP))
		return 1001;
	skt.SetTimeout(1000,1000);
	ICMP_HEADER header = {0};
	//static int nId = 1;
	//static int nSeq = 1;
	int nRet;

	// Fill in echo request
	header.Type		= TYPE_ICMP_ROUTE;
	header.Code		= 0;
	header.Checksum	= 0;
	header.ID			= nId++;
	header.Seq			= nSeq++;

	// Save tick count when sent
	int soc1,usec1,soc2,usec2;
	SDateTime::getSystemTime(soc1,usec1);
	
	// Put data in packet and compute checksum
	header.Checksum = in_cksum((u_short *)&header, sizeof(ICMP_HEADER));
	//timestamp.originate_time_stamp = 0;
	//timestamp.receive_time_stamp = 0;
	//timestamp.transmit_time_stamp = 0;

	// Send the echo request  				
	struct    sockaddr_in saDest;
	saDest.sin_addr.s_addr = inet_addr(sIp);
	saDest.sin_family = AF_INET;
	saDest.sin_port = 0;
	nRet = skt.SendTo(saDest,&header,sizeof(ICMP_HEADER));
	if (nRet != sizeof(ICMP_HEADER)) 
	{
		return 1002;
	}
	if(skt.CheckForRecv() < 1)
	{
		return 1003;
	}
	ICMP_ROUTE_Reply routeReply;
	sockaddr_in in;
	nRet = skt.RecvFrom(&routeReply,sizeof(ICMP_ROUTE_Reply),in);
	if(nRet <= 0)
	{
		return 1004;
	}
	DECODE_RESULT dst;
	//DecodeIcmpResponse((char*)&timestampReply,sizeof(ICMP_TIMESTAMP_REPLY),dst);
	//DecodeIcmpTimestampResponse(&routeReply,sizeof(ICMP_ROUTE_Reply),dst);
	SDateTime::getSystemTime(soc2,usec2);
	ttl = routeReply.ipHdr.ttl;
	ms = (soc2-soc1)*1000+(usec2-usec1)/1000;	
	return 0;
}

bool SICMP::DecodeIcmpRouteResponse(ICMP_ROUTE_Reply* pReply, int iPacketSize, DECODE_RESULT& stDecodeResult)
{

	return true;
}

int SICMP::doTraceRoute(char* sIP,int &ms,int &ttl)
{
	if (!CreateSocket(AF_INET, SOCK_RAW, IPPROTO_ICMP))
	{
		return 1001;
	}

	//设置端口属性
	int iTimeout = 20000;//ICMP_DEF_TIMEOUT;
	if (setsockopt(m_hSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&iTimeout, sizeof(iTimeout)) == SOCKET_ERROR)
	{
#ifdef WIN32
		m_iLastError = WSAGetLastError();
#else
		m_iLastError = errno;
#endif
		LOGWARN("Failed to set recv timeout, this=%p, handle=%X,errno=%d",
			this, m_hSocket, m_iLastError);
		CloseSocket();
		return -1;
	}
	//填充目的Socket地址
	sockaddr_in destSockAddr;
	ZeroMemory(&destSockAddr, sizeof(sockaddr_in));
	destSockAddr.sin_addr.s_addr = inet_addr(sIP);
	destSockAddr.sin_family = AF_INET;
	destSockAddr.sin_port = 0;

	memset(m_recvBuf, 0, sizeof(m_recvBuf));

	ICMP_ECHOQUEST echoReq;
	//static int nId = 1;
	//static int nSeq = 1;
	int nRet;

	// Fill in echo request
	echoReq.icmpHdr.Type		= TYPE_ICMP_ECHOREQ;
	echoReq.icmpHdr.Code		= 0;
	echoReq.icmpHdr.Checksum	= 0;
	echoReq.icmpHdr.ID			= nId++;
	echoReq.icmpHdr.Seq			= nSeq++;
#if 0
	// Fill in some data to send
	for (nRet = 0; nRet < ICMP_REQ_DATASIZE; nRet++)
		echoReq.cData[nRet] = ' '+nRet;
	//memset(echoReq.cData, 'E', ICMP_REQ_DATASIZE);
#endif
	DECODE_RESULT stDecodeResult;
	BOOL bReachDestHost = FALSE;
	USHORT usSeqNo = 0;
	int iTTL = 1;
	int iMaxHop = 50;//ICMP_DEF_MAX_HOP;
	m_strResultInfo.clear();
	while (!bReachDestHost && iMaxHop--)
	{
		//设置IP数据报头的ttl字段
		setsockopt(m_hSocket, IPPROTO_IP, IP_TTL, (char*)&iTTL, sizeof(iTTL));
		//输出当前跳站数作为路由信息序号
		//cout << setw(3) << iTTL << flush;
		//填充ICMP数据报剩余字段
		
		echoReq.icmpHdr.Seq = htons(usSeqNo++);
		echoReq.icmpHdr.Checksum = in_cksum((u_short *)&echoReq, sizeof(ICMP_ECHOQUEST));
		

		//记录序列号和当前时间
		stDecodeResult.usSeqNo = echoReq.icmpHdr.Seq;
		stDecodeResult.dwRoundTripTime = GetTickCount();

		//发送ICMP的EchoRequest数据报
		if (sendto(m_hSocket, (char*)&echoReq, sizeof(echoReq), 0, 
			(sockaddr*)&destSockAddr, sizeof(destSockAddr)) == SOCKET_ERROR)
		{
			//如果目的主机不可达则直接退出
			if (WSAGetLastError() == WSAEHOSTUNREACH)
			{
#ifdef WIN32
				m_iLastError = WSAGetLastError();
#else
				m_iLastError = errno;
#endif
				LOGWARN("Destination host unreachable--Trace complete, this=%p, handle=%X,errno=%d",
					this, m_hSocket, m_iLastError);
			}
			CloseSocket();
			return 0;
		}
		//接收ICMP的EchoReply数据报
		//因为收到的可能并非程序所期待的数据报，所以需要循环接收直到收到所要数据或超时
		sockaddr_in from;
		int iFromLen = sizeof(from);
		int iReadDataLen;
		
		while (1)
		{
			//等待数据到达
			iReadDataLen = recvfrom(m_hSocket, m_recvBuf, ICMP_MAX_PACKET_SIZE, 
				0, (sockaddr*)&from, &iFromLen);
			if (iReadDataLen != SOCKET_ERROR) //有数据包到达
			{
				//解码得到的数据包，如果解码正确则跳出接收循环发送下一个EchoRequest包
				if (DecodeIcmpTraceRouteResponse(m_recvBuf, iReadDataLen, stDecodeResult))
				{
					if (stDecodeResult.dwIPaddr.s_addr == destSockAddr.sin_addr.s_addr)
						bReachDestHost = TRUE;
					QString strIp = inet_ntoa(stDecodeResult.dwIPaddr);
					QString strInfo = QString::number(iTTL) + " 时间： IP:" + strIp + "\n";
					cout << '/t' << inet_ntoa(stDecodeResult.dwIPaddr) << endl;
					printf("\n ip is %s,ittl is %d", strIp.toStdString().data(),iTTL);
					m_strResultInfo += strInfo; 
					break;
				}
			}
			else if (WSAGetLastError() == WSAETIMEDOUT) //接收超时，打印星号
			{
#ifdef WIN32
				m_iLastError = WSAGetLastError();
#else
				m_iLastError = errno;
#endif
				LOGWARN("Request timed out, this=%p, handle=%X,errno=%d",
					this, m_hSocket, m_iLastError);
				QString strInfo = QString::number(iTTL) + " *  Request timed out\n";
				m_strResultInfo += strInfo;
				break;
			}
			else
			{
#ifdef WIN32
				m_iLastError = WSAGetLastError();
#else
				m_iLastError = errno;
#endif
				LOGWARN("Failed to call recvfrom, this=%p, handle=%X,errno=%d,ittl=%d",
					this, m_hSocket, m_iLastError,iTTL);
				printf("\n ip is ,ittl is %d",iTTL);
				CloseSocket();
				return -1;
			}
		}
		//TTL值加1
		iTTL++;
	}

	
	return 0;
}

//解码得到的数据报
BOOL SICMP::DecodeIcmpTraceRouteResponse(char* pBuf, int iPacketSize, DECODE_RESULT& stDecodeResult)
{
	//检查数据报大小的合法性
	IP_HEADER* pIpHdr = (IP_HEADER*)pBuf;
	int iIpHdrLen = pIpHdr->h_lenver * 4;
#if 0
	if (iPacketSize < (int)(iIpHdrLen+sizeof(ICMP_HEADER)))
		return FALSE;
#endif
	//按照ICMP包类型检查id字段和序列号以确定是否是程序应接收的Icmp包
	ICMP_HEADER* pIcmpHdr = (ICMP_HEADER*)(pBuf+iIpHdrLen);
	USHORT usID, usSquNo;
	if (pIcmpHdr->Type == TYPE_ICMP_ECHOREPLY)
	{
		usID = pIcmpHdr->ID;
		usSquNo = pIcmpHdr->Seq;
	}
	else if(pIcmpHdr->Type == TYPE_ICMP_TIME_EXCEEDED)//=TIMEOUT
	{
		char* pInnerIpHdr = pBuf+iIpHdrLen+sizeof(ICMP_HEADER);  //载荷中的IP头
		int iInnerIPHdrLen = ((IP_HEADER*)pInnerIpHdr)->h_lenver * 4;//载荷中的IP头长
		ICMP_HEADER* pInnerIcmpHdr = (ICMP_HEADER*)(pInnerIpHdr+iInnerIPHdrLen);//载荷中的ICMP头
		usID = pInnerIcmpHdr->ID;
		usSquNo = pInnerIcmpHdr->Seq;
	}
	else
		return FALSE;
#if 0
	if (usID != (USHORT)GetCurrentProcessId() || usSquNo !=stDecodeResult.usSeqNo) 
		return FALSE;
#endif
	//处理正确收到的ICMP数据报
	if (pIcmpHdr->Type == TYPE_ICMP_ECHOREPLY ||
		pIcmpHdr->Type == TYPE_ICMP_TIME_EXCEEDED)
	{
		//返回解码结果
		stDecodeResult.dwIPaddr.s_addr = pIpHdr->sourceIP;
		stDecodeResult.dwRoundTripTime = GetTickCount()-stDecodeResult.dwRoundTripTime;
#if 0
		//打印屏幕信息
		if (stDecodeResult.dwRoundTripTime)
			cout << setw(6) << stDecodeResult.dwRoundTripTime << " ms" << flush;
		else
			cout << setw(6) << "<1" << " ms" << flush;
#endif
		return TRUE;
	}
	return FALSE;
}

bool SICMP::doIcmpPingSend()
{
	int nRet;
	nRet = m_socket.SendTo(m_sendDest,m_chSendBuf,m_iSendLen);
	if (nRet != m_iSendLen) 
	{
		return false;
	}
	
	return true;
}

bool SICMP::doInitICMPInfo(SString strDestIp,int iThreadId,int iLen,int iSeq)
{
#if 0
	//m_socket.SetTimeout(1000,1000);
	ICMP_ECHOQUEST echoReq;
	int nRet;
	//nSeq = 1;
	//nId = 1;
	// Fill in echo request
	echoReq.icmpHdr.Type		= TYPE_ICMP_ECHOREQ;
	echoReq.icmpHdr.Code		= 0;
	echoReq.icmpHdr.Checksum	= 0;
	echoReq.icmpHdr.ID			= iThreadId;//nId++;//iThreadId;
	echoReq.icmpHdr.Seq			= nSeq++;

	
#if 0
	// Fill in some data to send
	char* pData = m_chSendBuf+sizeof(echoReq);
	for (nRet = 0; nRet < (iLen - sizeof(echoReq)); nRet++)
	{
		*pData = ' '+nRet;
		pData++;
	}
#endif


	// Put data in packet and compute checksum
	//echoReq.icmpHdr.Checksum = in_cksum((u_short *)&echoReq, sizeof(ICMP_ECHOQUEST));
	echoReq.icmpHdr.Checksum = in_cksum((u_short *)m_chSendBuf, iLen);
#endif
	memset(m_chSendBuf,0,sizeof(m_chSendBuf));
	ICMP_ECHOQUEST* pIcmpQuest = (ICMP_ECHOQUEST*)m_chSendBuf;
	
	pIcmpQuest->icmpHdr.Type		= TYPE_ICMP_ECHOREQ;
	pIcmpQuest->icmpHdr.Code		= 0;
	pIcmpQuest->icmpHdr.Checksum	= 0;
	pIcmpQuest->icmpHdr.ID			= iThreadId;//nId++;//iThreadId;
	pIcmpQuest->icmpHdr.Seq			= iSeq;
	// Fill in some data to send
	char* pData = m_chSendBuf+sizeof(ICMP_ECHOQUEST);
	for (int nRet = 0; nRet < (iLen - sizeof(ICMP_ECHOQUEST)); nRet++)
	{
		*pData = ' '+nRet;
		pData++;
	}
	pIcmpQuest->icmpHdr.Checksum = in_cksum((u_short *)m_chSendBuf, iLen);
	m_iSendLen = iLen;
	return true;
}

bool SICMP::doInitICMPBase(SString strDestIp)
{
	if(!m_socket.Create(AF_INET, SOCK_RAW, IPPROTO_ICMP))
		return false;
	// Send the echo request  				
	m_sendDest.sin_addr.s_addr = inet_addr(strDestIp.data());
	m_sendDest.sin_family = AF_INET;
	m_sendDest.sin_port = 0;
	return true;
}


int SICMP::testPing(char *sIp,int &ms,int &ttl)
{
	SSocket skt;
	if(!skt.Create(AF_INET, SOCK_RAW, IPPROTO_ICMP))
		return 1001;
	skt.SetTimeout(1000,1000);
	ICMP_ECHOQUEST echoReq;
	//static int nId = 1;
	//static int nSeq = 1;
	int nRet;

	// Fill in echo request
	echoReq.icmpHdr.Type		= TYPE_ICMP_ECHOREQ;
	echoReq.icmpHdr.Code		= 0;
	echoReq.icmpHdr.Checksum	= 0;
	echoReq.icmpHdr.ID			= nId++;
	echoReq.icmpHdr.Seq			= nSeq++;
#if 0
	// Fill in some data to send
	for (nRet = 0; nRet < ICMP_REQ_DATASIZE; nRet++)
		echoReq.cData[nRet] = ' '+nRet;
#endif
	// Save tick count when sent
	int soc1,usec1,soc2,usec2;
	SDateTime::getSystemTime(soc1,usec1);
	//echoReq.dwTime				= usec1;

	// Put data in packet and compute checksum
	echoReq.icmpHdr.Checksum = in_cksum((u_short *)&echoReq, sizeof(ICMP_ECHOQUEST));

	// Send the echo request  				
	struct    sockaddr_in saDest;
	saDest.sin_addr.s_addr = inet_addr(sIp);
	saDest.sin_family = AF_INET;
	saDest.sin_port = 0;
	nRet = skt.SendTo(saDest,&echoReq,sizeof(ICMP_ECHOQUEST));
	if (nRet != sizeof(ICMP_ECHOQUEST)) 
	{
		return 1002;
	}
	return 0;
}