#pragma once
#ifdef WIN32
#include <stdio.h> 
#define HAVE_REMOTE
#define WPCAP
#include <pcap.h>
#pragma comment(lib,"wpcap.lib")
#else
#include <stdio.h> 
#include <stdlib.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h> 
#include <netinet/ip.h> 
#include <string.h> 
#include <netdb.h> 
#include <netinet/tcp.h> 
#include <netinet/udp.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <signal.h> 
#include <net/if.h> 
#include <sys/ioctl.h> 
#include <sys/stat.h> 
#include <fcntl.h> 
#include <linux/if_ether.h> 
#include <net/ethernet.h> 

/* ssl_ifindex表示网卡编号*/
//struct sockaddr_ll 
//{ 
//	u_short sll_family; 
//	u_short sll_protocol; 
//	int sll_ifindex; //网络字节序，（3为eth1,5为eth3）
//	u_short sll_hatype; 
//	u_char sll_pkttype; 
//	u_char sll_halen; 
//	u_char sll_addr[8]; 
//};

#endif
//字节对齐必须是1 
#pragma pack(push)  
#pragma pack(1)
#define IPTOSBUFFERS    12
#define ETH_ARP         0x0806  //以太网帧类型表示后面数据的类型，对于ARP请求或应答来说，该字段的值为x0806
#define ARP_HARDWARE    1  //硬件类型字段值为表示以太网地址
#define ETH_IP          0x0800  //协议类型字段表示要映射的协议地址类型值为x0800表示IP地址
#define ARP_REQUEST     1
#define ARP_REPLY       2
#define HOSTNUM         255

struct ethernet_head 
{ 
	unsigned char dest_mac[6]; //目标主机MAC地址 
	unsigned char source_mac[6]; //源端MAC地址 
	unsigned short eh_type; //以太网类型 
}; 

struct arp_head 
{ 
	unsigned short hardware_type; //硬件类型：以太网接口类型为1 
	unsigned short protocol_type; //协议类型：IP协议类型为0X0800 
	unsigned char hardware_add_len; //硬件地址长度：MAC地址长度为6B 
	unsigned char protocol_add_len; //协议地址长度：IP地址长度为4B 
	unsigned short operation_field; //操作：ARP请求为1，ARP应答为2 
	unsigned char source_mac_add[6]; //源MAC地址：发送方的MAC地址 
	unsigned long source_ip_add; //源IP地址：发送方的IP地址 
	unsigned char dest_mac_add[6]; //目的MAC地址：ARP请求中该字段没有意义；ARP响应中为接收方的MAC地址 
	unsigned long dest_ip_add; //目的IP地址：ARP请求中为请求解析的IP地址；ARP响应中为接收方的IP地址
	unsigned char padding[18]; 
}; 

struct arp_packet //最终arp包结构 
{ 
	ethernet_head eth; //以太网头部 
	arp_head arp; //arp数据包头部 
}; 

#pragma pack()
#pragma pack(pop)  

class CArpManager
{
public:
	CArpManager(void);
	~CArpManager(void);
private:
	bool openNetCard();
private:
	pcap_t *m_adhandle;
};

