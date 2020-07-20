#pragma once
#include "SSocket.h"
#include "ssp_baseview.h"
#define TIMEOUT 1000

#pragma pack(push)  
#pragma pack(1)

#define TYPE_ICMP_ECHOREPLY	0
#define TYPE_ICMP_ECHOREQ	8
#define TYPE_ICMP_DESTINATION_UNREACHABLE 3
#define TYPE_ICMP_SOURCE_QUENCH 4
#define TYPE_ICMP_REDIRECT 5
#define TYPE_ICMP_ROUTE 10
#define TYPE_ICMP_TIME_EXCEEDED 11
#define TYPE_ICMP_PARAMETER_PROBLEM 12
#define TYPE_ICMP_TIMESTAMP   13
#define TYPE_ICMP_TIMESTAMP_REPLY 14
#define TYPE_ICMP_INFORMATION_REQUEST 15
#define TYPE_ICMP_INFORMATION_REPLY 16
#define TYPE_ICMP_MASK_REQUEST 17
#define TYPE_ICMP_MASK_REPLY 18

#define ICMP_DEF_TIMEOUT    1000
#define ICMP_MAX_PACKET_SIZE 1024
#define ICMP_REQ_DATASIZE 32
#define ICMP_DEF_MAX_HOP 30
//定义IP首部
typedef struct _iphdr{
	unsigned char h_lenver; //4 位IP版本号+4位首部长度
	unsigned char tos; //8位服务类型TOS
	unsigned short total_len; //16位IP包总长度（字节）
	unsigned short ident; //1 6位标识, 用于辅助IP包的拆装
	unsigned short frag_and_flags; //3位标志位+13位偏移位, 也是用于IP包的拆装
	unsigned char ttl; //8位IP包生存时间 TTL
	unsigned char proto; //8位协议 (TCP, UDP 或其他)
	unsigned short checksum; //16位IP首部校验和,最初置零,等所有包头都填写正确后,计算并替换.
	unsigned int sourceIP; //32位源IP地址
	unsigned int destIP; //32位目的IP地址

	//struct	in_addr iaSrc;	// Internet Address - Source
	//struct	in_addr iaDst;	// Internet Address - Destination
}IP_HEADER;  

//定义ICMP首部
typedef struct icmp_hdr{
	unsigned char Type; //8位类型
	unsigned char Code; //8位代码
	unsigned short Checksum; //16位校验和, 从TYPE开始,直到最后一位用户数据,如果为字节数为奇数则补充一位
	unsigned short ID; //识别号（一般用进程号作为识别号）, 用于匹配ECHO和ECHO REPLY包
	unsigned short Seq; //报文序列号, 用于标记ECHO报文顺序
	//unsigned int timestamp; //时间戳
}ICMP_HEADER;

//定义ICMP请求回显或是回显应答（Echo or Echo Reply Message)
//Type(8)是请求回显报文(Echo)；Type(0)是回显应答报文(Echo Reply)。
//请求回显或回显应答报文属于查询报文。Ping就是用这种报文进行查询和回应。
typedef struct _icmp_echo_quest{
	ICMP_HEADER icmpHdr;
	//unsigned int dwTime; //时间戳
	//char	cData[ICMP_REQ_DATASIZE];
}ICMP_ECHOQUEST;

// ICMP Echo Reply
typedef struct _icmp_echoreply
{
	IP_HEADER	ipHdr;
	ICMP_ECHOQUEST	echoRequest;
	char    cFiller[256];
}ICMP_ECHO_REPLY,*ICMP_PECHO_REPLY;

//时间戳或时间戳请求（Timestamp or Timeestamp Reply Message)
typedef struct _icmp_timestamp
{
	ICMP_HEADER icmpHdr;
	unsigned int originate_time_stamp; //起始时间戳
	unsigned int receive_time_stamp; //接受时间戳
	unsigned int transmit_time_stamp; //回显时间戳
	//char	cData[ICMP_REQ_DATASIZE];
}ICMP_TIMESTAMP;


//时间戳或时间戳应答（Timestamp or Timeestamp Reply Message)
typedef struct _icmp_timestamp_reply
{
	IP_HEADER	ipHdr;
	ICMP_TIMESTAMP	timestamp;
	char    cFiller[256];
}ICMP_TIMESTAMP_REPLY;

//地址掩码请求（Timestamp or Timeestamp Reply Message)
typedef struct _icmp_mask
{
	ICMP_HEADER icmpHdr;
	unsigned int imask; //掩码
}ICMP_MASK_QUEST;


//地址掩码请求返回
typedef struct _icmp_mask_reply
{
	IP_HEADER	ipHdr;
	ICMP_MASK_QUEST	maskquest;
	char    cFiller[256];
}ICMP_MASK_REPLY;
//路由询问和查询
typedef struct _icmp_route_reply
{
	IP_HEADER	ipHdr;
	ICMP_HEADER icmp_header;
	unsigned ch[ICMP_REQ_DATASIZE];
}ICMP_ROUTE_Reply;

//解码结果
typedef struct
{
	USHORT usSeqNo;   //包序列号
	DWORD dwRoundTripTime; //往返时间
	in_addr dwIPaddr;  //对端IP地址
} DECODE_RESULT;

#pragma pack()
#pragma pack(pop)  


unsigned short checksum(unsigned short* buffer,int size);

class SICMP
{
public:
	SICMP(void);
	~SICMP(void);

	bool initSocket();
	void setIpAndPort(SString strIP, SString strPort)
	{
		m_strDestIp = strIP;
		m_strDestPort = strPort;
	}
	
	int doPing(char *sIp,int &ms,int &ttl);
	int testPing(char *sIp,int &ms,int &ttl);
	u_short in_cksum(u_short *addr, int len);
	int doTimestamp(char *sIp,int &ms,int &ttl);
	int doMask(char *sIp,int &ms,int &ttl);
	int doroute(char *sIp,int &ms,int &ttl);
	int doTraceRoute(char* sIP,int &ms,int &ttl);
	bool DecodeIcmpResponse(char* pBuf, int iPacketSize, DECODE_RESULT& stDecodeResult);
	bool DecodeIcmpEchoResponse(ICMP_ECHO_REPLY* pReply, int iPacketSize, DECODE_RESULT& stDecodeResult);
	bool DecodeIcmpTimestampResponse(ICMP_TIMESTAMP_REPLY* pReply, int iPacketSize, DECODE_RESULT& stDecodeResult);
	bool DecodeIcmpMaskResponse(ICMP_MASK_REPLY* pReply, int iPacketSize, DECODE_RESULT& stDecodeResult);
	bool DecodeIcmpRouteResponse(ICMP_ROUTE_Reply* pReply, int iPacketSize, DECODE_RESULT& stDecodeResult);
	BOOL DecodeIcmpTraceRouteResponse(char* pBuf, int iPacketSize, DECODE_RESULT& stDecodeResult);

	bool doIcmpPingSend();
	bool doInitICMPBase(SString strDestIp);
	bool doInitICMPInfo(SString strDestIp,int iThreadId,int iLen,int iSeq);
private:
	bool CreateSocket(int af, int type, int protocol);
	int CloseSocket();
private:
	SSocket m_socket;
	sockaddr_in m_sendDest;
	char  m_chSendBuf[1024];
	int   m_iSendLen;
	//SOCKET  m_socket;
	SString m_strDestIp;
	SString m_strDestPort;
	SOCKET m_hSocket;
	int m_iAF;
	bool m_bTcp;
	int m_iSocketType;
	int m_iLastError;

	char m_recvBuf[ICMP_MAX_PACKET_SIZE];
	static int nId;
	static int nSeq;
public:
	QString m_strResultInfo;
};

