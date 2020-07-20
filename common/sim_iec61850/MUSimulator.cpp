/**
 *
 * 文 件 名 : MUSimulator.cpp
 * 创建日期 : 2012-10-31 11:03
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : MU模拟器，只针对9-2协议
 * 修改记录 : 
 *            $Log: $
 *
 **/

#include "MUSimulator.h"

//////////////////////////////////////////////////////////////////////////
// 名    称:  CMUSimulator
// 作    者:  邵凯田
// 创建时间:  2012-10-31 11:03
// 描    述:  MU模拟器
//////////////////////////////////////////////////////////////////////////

CMUSimulator::CMUSimulator(stuSclVtIedSmvOut *pSmvOut,SString srcmac)
{
	SString::MacStrToHex(pSmvOut->mac,m_DstMac);
	SString::MacStrToHex(srcmac,m_SrcMac);
	m_iVLanId = pSmvOut->vlan;
	m_iVLanP = pSmvOut->vlan_priority;
	m_iReserver1 = m_iReserver2 = 0;
	m_dwConfRev = pSmvOut->confRev;
	m_bSync = pSmvOut->sampleSynchronized;
	m_iAsduNum = pSmvOut->nofASDU;
	m_iAppid = pSmvOut->appid;
	m_sSvId = pSmvOut->smvID;
	m_iSampRate = pSmvOut->smpRate;
	if(m_iSampRate == 4000)
		m_iSampRate = 80;
	m_pBuffer = new BYTE[C_MAX_FRAME_LEN*m_iSampRate];//每周期80点，最大包512字节
	memset(m_pBuffer,0,C_MAX_FRAME_LEN*m_iSampRate);
	m_iChns = 0;
	int val,q;
	unsigned long pos;
	stuSclVtIedDatasetFcda *pFcda = pSmvOut->m_pDataset->m_Fcda.FetchFirst(pos);
	while(pFcda)
	{		
		if(pFcda->path.find("$q") > 0)
		{
			pFcda = pSmvOut->m_pDataset->m_Fcda.FetchNext(pos);
			continue;
		}
		m_iChns++;
		val = SString::GetAttributeValueI(pFcda->value,"val");
		if(pFcda->value == "")
			val = 0;//6553600;
		q = SString::GetAttributeValueI(pFcda->value,"q");
#if 0
		if(strstr(pFcda->doi_desc,"额定延") != NULL)
			AddChannel(MU_DELAY,val,q);
		else if(strstr(pFcda->doi_desc,"电流") != NULL)
			AddChannel(MU_CHN_AI,val,q);
		else if(strstr(pFcda->doi_desc,"电压") != NULL)
			AddChannel(MU_CHN_AU,val,q);
		else
			AddChannel(MU_CHN_D,val,q);
#endif
		if (pFcda->chn_type == MU_DELAY)
		{
			
			AddChannel(MU_DELAY,val,q);
		} else if (pFcda->chn_type == MU_CHN_AI)
		{
			float fValue;
			if (pFcda->primary_rated == 0 ||
				pFcda->secondary_rated == 0)
			{
				fValue = 0;
			} else
			{
				fValue = pFcda->primary_rated/pFcda->secondary_rated*((float)val)*1000;
			}
			val = floor(fValue);
			AddChannel(MU_CHN_AI,val,q);
		} else if (pFcda->chn_type == MU_CHN_AU)
		{
			float fValue;
			if (pFcda->primary_rated == 0 ||
				pFcda->secondary_rated == 0)
			{
				fValue = 0;
			} else
			{
				fValue = pFcda->primary_rated/pFcda->secondary_rated*((float)val)*100;
			}
			val = floor(fValue);
			AddChannel(MU_CHN_AU,val,q);
		} else 
		{
			AddChannel(MU_CHN_D,val,q);
		}
				
		pFcda = pSmvOut->m_pDataset->m_Fcda.FetchNext(pos);
	}

	m_iSampCount = m_iSampPos = 0;
	m_iDistrub = 0;
	m_iBadLevel = 0;
	RebuildBuffer();
}

CMUSimulator::~CMUSimulator()
{
	if(m_pBuffer != NULL)
		delete[] m_pBuffer;
}

CMUSimulator::CChannel::CChannel()
{
	m_iType = MU_CHN_AU;
	m_iValue = m_iNewValue = 1000000;
	m_q = 0;
}

CMUSimulator::CChannel::~CChannel()
{

}

//////////////////////////////////////////////////////////////////////////
// 描    述:  使用通道数量初始化模拟MU
// 作    者:  邵凯田
// 创建时间:  2012-10-31 16:43
// 参数说明:  @chns为通道数量
// 返 回 值:  true/false
//////////////////////////////////////////////////////////////////////////
bool CMUSimulator::Init(int chns)
{
	if(m_pBuffer != NULL)
		return false;
	m_pBuffer = new BYTE[C_MAX_FRAME_LEN*m_iSampRate];//每周期80点，最大包512字节
	memset(m_pBuffer,0,C_MAX_FRAME_LEN*m_iSampRate);
	m_iChns = chns;
	for(int i=0;i<chns;i++)
	{
		AddChannel(MU_CHN_AU,1000000,0);
	}
	RebuildBuffer();
	return true;
}


//////////////////////////////////////////////////////////////////////////
// 描    述:  重新构建缓冲区
// 作    者:  邵凯田
// 创建时间:  2012-10-31 16:42
// 参数说明:  void
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CMUSimulator::RebuildBuffer()
{
	int i,j;
	BYTE *pRow = m_pBuffer;
	BYTE *p;
	WORD w,wLenApp,wLenPdu,wLenAsdu,wLenData,wSeqLen;
	stuSV92_Head1 *pHead1;
	stuSV92_Head2 *pHead2;
	CChannel *pChn;
	int val;
	float fval;
	int *pi;

	//计算PDU长度
	wLenData = m_iChns * 8;
	wLenAsdu = wLenData + sizeof(stuSV92_Head2) + m_sSvId.length() + 2;
	wSeqLen = wLenAsdu+4;
	wLenPdu = wLenAsdu + 11;//10
	wLenApp = wLenPdu + 12;
	m_iFrameLen = wLenApp + 18;//一包数据的长度
	m_iSeqPos = m_iDatPos = 0;
	for(i=0;i<m_iSampRate;i++,pRow+=C_MAX_FRAME_LEN)
	{
		pHead1 = (stuSV92_Head1*)pRow;
		//dst mac
		memcpy(pHead1->DstMac,m_DstMac,6);
		//src mac
		memcpy(pHead1->SrcMac,m_SrcMac,6);
		pHead1->VLan[0] = 0x81;
		pHead1->VLan[1] = 0x00;
		pHead1->VLan_Val[0] = (m_iVLanP<<5)|((m_iVLanId>>8)&0x0f);
		pHead1->VLan_Val[1] = (BYTE)m_iVLanId;

		//88ba
		pHead1->Type = htons(0x88ba);
		//appid
		pHead1->AppID = htons(m_iAppid);
		//length
		pHead1->AppLen = htons(wLenApp);
		//r1
		pHead1->Res1 = htons(m_iReserver1);
		//r2
		pHead1->Res2 = htons(m_iReserver2);
		//pdu length
		pHead1->PDU_Len[0] = 0x60;
		//pHead1->PDU_Len[1] = 0x82;
		//w =htons(wLenPdu);
		//memcpy(&pHead1->PDU_Len[2],&w,2);
		pHead1->PDU_Len[1] = 0x82;
 		pHead1->PDU_Len[2] = (BYTE)(wLenPdu>>8);
 		pHead1->PDU_Len[3] = (BYTE)(wLenPdu&0xff);
//		pHead1->PDU_Len[2] = (BYTE)wLenPdu;

		//number of asdu
		pHead1->NumberOfAsdu[0] = 0x80;
		pHead1->NumberOfAsdu[1] = 0x01;
		pHead1->NumberOfAsdu[2] = 0x01;
		//sequence of asdu
		pHead1->SeqOfAsdu[0] = 0xA2;
		pHead1->SeqOfAsdu[1] = 0x82;		
		pHead1->SeqOfAsdu[2] = (BYTE)(wSeqLen>>8);
		pHead1->SeqOfAsdu[3] = (BYTE)(wSeqLen&0xff);
		//pHead1->SeqOfAsdu[2] = (BYTE)(wSeqLen);

		//asdu head
		pHead1->Asdu_Head[0] = 0x30;
		//pHead1->Asdu_Head[1] = 0x82;
		//w =htons(wLenAsdu);
		//memcpy(&pHead1->Asdu_Head[2],&w,2);
		pHead1->Asdu_Head[1] = 0x82;
 		pHead1->Asdu_Head[2] = (BYTE)(wLenAsdu>>8);
 		pHead1->Asdu_Head[3] = (BYTE)(wLenAsdu&0xff);
//		pHead1->Asdu_Head[2] = (BYTE)(wLenAsdu);
		//svid
		pHead1->SvId_Empty[0] = 0x80;
		pHead1->SvId_Empty[1] = (BYTE)m_sSvId.length();
		memcpy(&pHead1->SvId_Empty[2],m_sSvId.data(),m_sSvId.length());

		//SVID后
		//sample count
		pHead2 = (stuSV92_Head2 *)(((BYTE*)&pHead1->SvId_Empty[2]) + m_sSvId.length());
		pHead2->SampCnt[0] = 0x82;
		pHead2->SampCnt[1] = 0x02;
		//smpcnt不写，发送时生成
		//if(m_iSeqPos == 0)
			m_iSeqPos = (&pHead2->SampCnt[2]-pRow);

		//configure rev
		pHead2->ConfRev[0] = 0x83;
		pHead2->ConfRev[1] = 0x04;
		*((DWORD*)&pHead2->ConfRev[2]) = ntohl(m_dwConfRev);
		//sync
		pHead2->Sync[0] = 0x85;
		pHead2->Sync[1] = 0x01;
		pHead2->Sync[2] = m_bSync?1:0;
		
		//data header
		pHead2->Data_Head[0] = 0x87;
		//pHead2->Data_Head[1] = 0x82;
		//w = htons(wLenData);
		//memcpy(&pHead2->Data_Head[2],&w,2);
		pHead2->Data_Head[1] = 0x82;
		pHead2->Data_Head[2] = (BYTE)(wLenData>>8);
		pHead2->Data_Head[3] = (BYTE)(wLenData&0xff);
//		pHead2->Data_Head[2] = (BYTE)(wLenData);

		pi = (int*)(&pHead2->Data_Head[4]);
		//后面是数据
		unsigned long pos;
		pChn = m_Chns.FetchFirst(pos);
		for(j=0;pChn && j<m_iChns;j++,pChn = m_Chns.FetchNext(pos))
		{
			pChn->m_iValue = pChn->m_iNewValue;
			
			if(pChn->m_iType == MU_CHN_D)//直流
			{
				val = pChn->m_iValue;
				if(m_iDistrub != 0)
					val += (int)(((float)val)*((0-rand()%2)*(float)(rand()%m_iDistrub))/10000.0f);
			}
			else//交流
			{
				val = (int)(((float)sin(3.14159*2*(float)i/(float)m_iSampRate) * 1.414f * pChn->m_iValue));
				if(j == 0)
				{
					val = val;
				}
				if(m_iDistrub != 0)
					val += (int)(((float)val)*((0-rand()%2)*(float)(rand()%m_iDistrub))/10000.0f);
			}
			*pi = htonl(val);
			pi++;
			*pi = htonl(pChn->m_q);
			pi++;
		}
	}
	return;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  添加一个新的通道
// 作    者:  邵凯田
// 创建时间:  2012-10-31 15:30
// 参数说明:  @eType为类型
//            @iValue为当前值
//            @q为品质
// 返 回 值:  新通道指针
//////////////////////////////////////////////////////////////////////////
CMUSimulator::CChannel* CMUSimulator::AddChannel(eMuChnType eType,int iValue,DWORD q)
{
	CChannel *pChn = new CChannel();
	pChn->m_iType = eType;
	pChn->m_iNewValue = iValue;
	pChn->m_q = q;
	m_Chns.append(pChn);
	return pChn;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  更新目标MAC地址
// 作    者:  邵凯田
// 创建时间:  2012-10-31 18:54
// 参数说明:  @pMac为目标MAC地址
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CMUSimulator::UpdateDstMac(BYTE *pMac)
{
	memcpy(m_DstMac,pMac,6);
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  更新源MAC地址
// 作    者:  邵凯田
// 创建时间:  2012-10-31 18:54
// 参数说明:  @pMac为源MAC地址
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CMUSimulator::UpdateSrcMac(BYTE *pMac)
{
	memcpy(m_SrcMac,pMac,6);
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  更新APPID
// 作    者:  邵凯田
// 创建时间:  2012-10-31 16:19
// 参数说明:  @appid
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CMUSimulator::UpdateAppid(WORD appid)
{
	m_iAppid = appid;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  更新保留字
// 作    者:  邵凯田
// 创建时间:  2012-10-31 16:19
// 参数说明:  @r1/r2
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CMUSimulator::UpdateReserver(WORD r1,WORD r2)
{
	m_iReserver1 = r1;
	m_iReserver2 = r2;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  更新SVID
// 作    者:  邵凯田
// 创建时间:  2012-10-31 16:19
// 参数说明:  @svid
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CMUSimulator::UpdateSvID(SString svid)
{
	m_sSvId = svid;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  更新同步标识
// 作    者:  邵凯田
// 创建时间:  2012-10-31 16:19
// 参数说明:  @sync=true表示同步，false表示失步
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CMUSimulator::UpdateSync(bool sync)
{
	m_bSync = sync;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  更新通道类型
// 作    者:  邵凯田
// 创建时间:  2012-10-31 16:59
// 参数说明:  @pChn为通道
//            @eType为类型
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CMUSimulator::UpdateChnType(CChannel *pChn,eMuChnType eType)
{
	pChn->m_iType = eType;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  更新通道品质
// 作    者:  邵凯田
// 创建时间:  2012-10-31 16:20
// 参数说明:  @pChn为对应通道
//            @q为品质字
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CMUSimulator::UpdateChnQ(CChannel* pChn, DWORD q)
{
	pChn->m_q = q;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  更新通道值
// 作    者:  邵凯田
// 创建时间:  2012-10-31 16:21
// 参数说明:  @pChn为对应通道
//            @iNewValue为新的值
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CMUSimulator::UpdateChnData(CChannel *pChn,int iNewValue)
{
	pChn->m_iNewValue = iNewValue;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  取一个新的采样点
// 作    者:  邵凯田
// 创建时间:  2012-11-1 10:59
// 参数说明:  void
// 返 回 值:  BYTE*
//////////////////////////////////////////////////////////////////////////
BYTE* CMUSimulator::GetNewSamplePoint()
{
	BYTE *pBuf = &m_pBuffer[m_iSampPos*C_MAX_FRAME_LEN];
	WORD w = htons(m_iSampCount);
	memcpy(pBuf+m_iSeqPos,&w,2);

	m_iSampPos++;
	if(m_iSampPos >= m_iSampRate)
		m_iSampPos = 0;
	m_iSampCount++;
	if(m_iSampCount >= m_iSampRate*50)
		m_iSampCount = 0;

	return pBuf;
}


//////////////////////////////////////////////////////////////////////////
// 名    称:  CMUSimPort
// 作    者:  邵凯田
// 创建时间:  2012-11-1 11:29
// 描    述:  MU模拟器端口，一个端口可以发送多个MU
//////////////////////////////////////////////////////////////////////////
CMUSimPort::CMUSimPort(SString sDevName)
{
	m_MUs.setAutoDelete(true);
	m_sDevName = sDevName;
}

CMUSimPort::~CMUSimPort()
{
}

////////////////////////////////////////////////////////////////////////
// 描    述:  启动服务,虚函数，派生函数中必须先调用此函数
// 作    者:  邵凯田
// 创建时间:  2012-11-1 11:29
// 参数说明:  void
// 返 回 值:  true表示服务启动成功,false表示服务启动失败
//////////////////////////////////////////////////////////////////////////
bool CMUSimPort::Start()
{
	if(!SService::Start())
		return false;
#ifdef WIN32
	m_pSendQueue = pcap_sendqueue_alloc(C_MAX_FRAME_LEN*4000);
	memset(&m_PktHdr,0,sizeof(m_PktHdr));
	int soc,usec;
	SDateTime::getSystemTime(soc,usec);
	m_PktHdr.ts.tv_sec = soc;
	m_PktHdr.ts.tv_usec = usec;
	if ( (m_pCap = pcap_open(m_sDevName.data(), 1600, PCAP_OPENFLAG_PROMISCUOUS, 1, NULL, m_ErrBuf) ) == NULL)  
		// 	if ((m_pCap = pcap_open_live(sDevName.data(),		// name of the device
		// 							 65536,			// portion of the packet to capture. It doesn't matter in this case 
		// 							 1,				// promiscuous mode (nonzero means promiscuous)
		// 							 1,			// read timeout
		// 							 m_ErrBuf			// error buffer
		// 							 )) == NULL)
	{
		LOGERROR("无法打开网卡. '%s' 不是WinPcap支持的有效设备名!\n", m_sDevName.data());
		return false;
	}
#else
	if(!m_Sender.Create(m_sDevName))
	{
		LOGERROR("在设备[%s]上创建原始发送套接字失败!",m_sDevName.data());
		return false;
	}
#endif
#ifdef WIN32
	HANDLE h; 
	h = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)(int)ThreadSend, (LPVOID)this, 0, 0); 
	::SetThreadPriority(h,THREAD_PRIORITY_HIGHEST);  
	CloseHandle(h);
#else
	SKT_CREATE_THREAD(ThreadSend,this);
#endif
	return true;
}

////////////////////////////////////////////////////////////////////////
// 描    述:  停止服务，派生函数中必须先调用此函数
// 作    者:  邵凯田
// 创建时间:  2012-11-1 11:29
// 参数说明:  void
// 返 回 值:  true表示服务停止成功,false表示服务停止失败
//////////////////////////////////////////////////////////////////////////
bool CMUSimPort::Stop()
{
	SService::Stop();
#ifdef WIN32
	if(m_pCap != NULL)
	{
		pcap_sendqueue_destroy(m_pSendQueue);
		pcap_close(m_pCap);
		m_pCap = NULL;
	}
#else
	m_Sender.Close();
#endif
	return true;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  发送全部MU一个周波的数据，使用队列发送
// 作    者:  邵凯田
// 创建时间:  2012-11-1 11:40
// 参数说明:  void
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CMUSimPort::SendAllMu()
{
	unsigned long pos;
	int len;
	BYTE *pBuf;
	int soc,usec;

#ifdef WIN32
	SDateTime::getSystemTime(soc,usec);
	m_PktHdr.ts.tv_sec = soc;
	m_PktHdr.ts.tv_usec = usec;
	CMUSimulator *pMu;
	m_pSendQueue->len = 0;
	if(m_pCap == NULL || m_MUs.count() == 0)
		return;
	float fusec;
	fusec = usec;
	float fsep = 250/m_MUs.count();
	for(int i=0;i<80&&!IsQuit();i++)
	{
		pMu = m_MUs.FetchFirst(pos);
		while(pMu)
		{
			pBuf = pMu->GetNewSamplePoint();
			len = pMu->GetFrameLen();
			m_PktHdr.caplen = len;//(len&0x03)==0?len:(((len>>2)<<2)+4);
			m_PktHdr.len = len;
			if(pcap_sendqueue_queue(m_pSendQueue, &m_PktHdr, pBuf) == -1)  
				return ;
			fusec += fsep;
			if(fusec >= 1000000)
			{
				fusec -= 1000000;
				soc +=1;
			}
			m_PktHdr.ts.tv_sec = (int)soc;
			m_PktHdr.ts.tv_usec = (int)fusec;
			pMu = m_MUs.FetchNext(pos);
		}
	}
	if(( pcap_sendqueue_transmit(m_pCap, m_pSendQueue, 1)) < m_pSendQueue->len)
		return ;
	m_pSendQueue->len = 0;

	return;
#else
	CMUSimulator *pMu = m_MUs.FetchFirst(pos);
	while(pMu)
	{
		pBuf = pMu->GetNewSamplePoint();
		len = pMu->GetFrameLen();
		if(pMu->m_iBadLevel == 1)
		{
			if(rand()%1000 < 2)
			{
				if(rand()%100 < 3)
					pBuf += 4;
				len -= (rand()%(len/2));
			}
		}
		else if(pMu->m_iBadLevel == 2)
		{
			if(rand()%100 < 5)
			{
				if(rand()%10 < 3)
					pBuf += 4;
				len -= (rand()%(len/2));
			}
		}
		m_Sender.SendRaw(pBuf,len);
		pMu = m_MUs.FetchNext(pos);
	}
#endif
}



void* CMUSimPort::ThreadSend(void *lp)
{
	CMUSimPort* pThis = (CMUSimPort*)lp;
	int soc1,usec1,soc2,usec2;
	int usecs;
	pThis->BeginThread();

#ifdef WIN32
	LARGE_INTEGER freq;  
	LARGE_INTEGER start, end;  
	QueryPerformanceFrequency(&freq);
	unsigned int us = 20000;  
	LONGLONG count = (us * freq.QuadPart) / (1000000);
	LONGLONG startQuadPart;
	QueryPerformanceCounter(&start);
	startQuadPart = end.QuadPart = start.QuadPart;
#else
	pthread_attr_t attr;
	int rs;
	rs = pthread_attr_init( &attr );
	pthread_attr_setschedpolicy( &attr, SCHED_RR );
	struct sched_param param;
	param.sched_priority = 1;
	pthread_attr_setschedparam(&attr, &param);
	SDateTime::getSystemTimeNs(soc1,usec1);
	double nsecs;
#endif

	while(!pThis->IsQuit())
	{
//		SDateTime::getSystemTime(soc1,usec1);
		pThis->SendAllMu();
// 		SDateTime::getSystemTime(soc2,usec2);
// 		usecs = ((soc2-soc1)*1000000+(usec2-usec1));
// 		if(usecs > 16000)
// 		{
// 			usecs++;
// 		}
#ifdef WIN32
		startQuadPart += count;
		//startQuadPart = end.QuadPart + count;
		do  
		{
			//SApi::ThreadYield();
			QueryPerformanceCounter(&end);  
		}while(end.QuadPart< startQuadPart);
#if 0
		while(1)
		{
			SDateTime::getSystemTime(soc2,usec2);
			usecs = 250 - ((soc2-soc1)*1000000+(usec2-usec1));
			if(usecs<=6)
				break;
			else if(usecs < 100)
				continue;
			else if(usecs > 250)
				continue;
			SApi::ThreadYield();
		}
#endif
#else
		while(1)
		{
			SDateTime::getSystemTimeNs(soc2,usec2);
			nsecs = 250000 - (((double)(soc2-soc1))*1000000000+(usec2-usec1));
			if(nsecs<=3000)
				break;
// 			if(nsecs > 100000)
// 			{
// 				SApi::UsSleep(2);
// 				continue;
// 			}
// 			else if(nsecs > 50000)
// 			{
// 				SApi::UsSleep(1);
// 				continue;
// 			}
			SApi::ThreadYield();
		}
		soc1 = soc2;
		usec1 = usec2;
#endif
	}

	pThis->EndThread();
	return NULL;
}
bool CMUSimPort::CheckSmvIsExist(stuSclVtIedSmvOut* pCur)
{
	if (pCur == NULL)
	{//当指针为空时，返回true，防止外部调用该函数判断将对象添加对模拟器列表中
		return true;
	}
	for(int i = 0; i < m_MUs.count(); i ++)
	{
		CMUSimulator* pCurMusSim = m_MUs.at(i);
		if (pCurMusSim == NULL)
		{
			continue;
		}
		if (pCur == pCurMusSim->m_pSmvOut)
		{
			m_MUs.remove(i);
			return true;
		}
	}
	return false;
}
//////////////////////////////////////////////////////////////////////////
// 名    称:  CMUSimManager
// 作    者:  邵凯田
// 创建时间:  2012-10-31 14:21
// 描    述:  MU模拟器管理器
//////////////////////////////////////////////////////////////////////////
CMUSimManager::CMUSimManager()
{

}

CMUSimManager::~CMUSimManager()
{

}

//////////////////////////////////////////////////////////////////////////
// 描    述:  查找指针网卡名的端口实例，不存在则新创建
// 作    者:  邵凯田
// 创建时间:  2012-11-1 13:22
// 参数说明:  @sDevName为网卡名
// 返 回 值:  CMUSimPort*
//////////////////////////////////////////////////////////////////////////
CMUSimPort* CMUSimManager::SearchPort(SString sDevName)
{
	unsigned long pos;
	CMUSimPort *pPort = m_Ports.FetchFirst(pos);
	while(pPort)
	{
		if(pPort->m_sDevName == sDevName)
			break;
		pPort = m_Ports.FetchNext(pos);
	}
	if(pPort == NULL)
	{
		pPort = new CMUSimPort(sDevName);
		pPort->Start();
		m_Ports.append(pPort);
	}
	return pPort;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  添加一个新的MU
// 作    者:  邵凯田
// 创建时间:  2012-11-1 11:59
// 参数说明:  sDevName为网卡名
//            @pMU为初始化后的MU实例
// 返 回 值:  CMUSimulator*
//////////////////////////////////////////////////////////////////////////
CMUSimulator* CMUSimManager::AddMU(SString sDevName,CMUSimulator *pMU)
{
	CMUSimPort *pPort = SearchPort(sDevName);
	pPort->m_MUs.append(pMU);
	return pMU;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  停止全部发送，释放对象
// 作    者:  邵凯田
// 创建时间:  2012-11-1 11:59
// 参数说明:  void
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CMUSimManager::Clear()
{
	unsigned long pos;
	CMUSimPort *pPort = m_Ports.FetchFirst(pos);
	while(pPort)
	{
		pPort->StopAndWait();
		pPort = m_Ports.FetchNext(pos);
	}
	m_Ports.clear();
}


