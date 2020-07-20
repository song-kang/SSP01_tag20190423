#include "ArpManager.h"


CArpManager::CArpManager(void)
{
}


CArpManager::~CArpManager(void)
{
}

bool CArpManager::openNetCard()
{
	///* 打开设备*/
	//if ( (m_adhandle= pcap_open(d->name,          // 设备名
	//	65536,            // 65535保证能捕获到不同数据链路层上的每个数据包的全部内容
	//	PCAP_OPENFLAG_PROMISCUOUS,    // 混杂模式
	//	1000,             // 读取超时时间
	//	NULL,             // 远程机器验证
	//	errbuf            // 错误缓冲池
	//	) ) == NULL)
	//{
	//	fprintf(stderr,"\n无法读取该适配器. 适配器%s 不被WinPcap支持\n", d->name);
	//	/* 释放设备列表*/  
	//	pcap_freealldevs(alldevs);
	//	return -1;
	//}
	return true;

}
