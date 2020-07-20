/**
 *
 * 文 件 名 : GooseSimulator.cpp
 * 创建日期 : 2016-1-6 9:18
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : GOOSE模拟器
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2016-1-6	邵凯田　创建文件
 *
 **/
#include "GooseSimulator.h"
 #include <QDebug>
 #include <QStringList>
 #include <QString>
//////////////////////////////////////////////////////////////////////////
// 名    称:  CGooseSimulator
// 作    者:  邵凯田
// 创建时间:  2012-10-31 11:03
// 描    述:  GOOSE模拟器
//////////////////////////////////////////////////////////////////////////

CGooseSimulator::CGooseSimulator(stuSclVtIedGooseOut *pGooseOut,SString srcmac)
{
	m_pGooseOut = pGooseOut;
	m_Chns.setShared(true);
	SString::MacStrToHex(pGooseOut->mac,m_DstMac);
	SString::MacStrToHex(srcmac,m_SrcMac);
	m_iVLanId = pGooseOut->vlan;
	m_iVLanP = pGooseOut->vlan_priority;
	m_iReserver1 = m_iReserver2 = 0;
	m_iConfRev = pGooseOut->confRev;
	//TODO:gocb/dataset可能生成的不规范
	//<GSEControl name="GoCB_In" datSet="dsGOOSE2" confRev="1" appID="IL2201ARPIT/LLN0.GoCB_In" type="GOOSE"/>
	//goID: IL2201ARPIT/LLN0.GoCB_In
	//gocbRef: IL2201ARPIT/LLN0$GO$GoCB_In
	//datSet: IL2201ARPIT/LLN0$dsGOOSE2

	m_goID = pGooseOut->gocb;//appID="IL2201ARPIT/LLN0.GoCB_In"
	SString lln0 = m_goID.left(m_goID.find("/LLN0.")+5);
	m_gocb = lln0+"$GO$"+pGooseOut->name;
	m_dataset = lln0+"$"+pGooseOut->ds_name;
	m_iAppid = pGooseOut->appid;
	m_iMaxSendMsec = pGooseOut->max_time;
	m_iTTL = pGooseOut->max_time*2;
	m_iST = 0;
	m_iSQ = 0;
	m_bTest = false;
	m_iNdsCom = 0;
	m_pBuffer = new BYTE[C_MAX_GOOSE_FRAME_LEN];//一包数据
	memset(m_pBuffer, 0, C_MAX_GOOSE_FRAME_LEN);
	m_iChns = 0;
	int val1,q;
	SString chnname,val;
	unsigned long pos;
	stuSclVtIedDatasetFcda *pFcda = pGooseOut->m_pDataset->m_Fcda.FetchFirst(pos);
	while(pFcda)
	{
		m_iChns++;
		chnname.sprintf("$%s$%s$",pFcda->doi_desc,pFcda->dai_desc);
		//int vtype;//0:未知  1:字符串  2：整数  3：浮点数  4：bitstring  5:utctime  6:bool  7:品质  8:Struct ...
		val = pFcda->value;//SString::toInt(pFcda->value);//SString::GetAttributeValueI(pFcda->value,"value");
		if(pFcda->vtype == 1)
			AddChannel(chnname, GOOSE_CHN_STRING,val);
		else if(pFcda->vtype == 2)
			AddChannel(chnname, GOOSE_CHN_INT,val);
		else if(pFcda->vtype == 3)
			AddChannel(chnname, GOOSE_CHN_FLOAT,val);
		else if(pFcda->vtype == 4)
			AddChannel(chnname, GOOSE_CHN_BITSTR,val);
		else if(pFcda->vtype == 5)
			AddChannel(chnname, GOOSE_CHN_UTC,val);
		else if(pFcda->vtype == 6 || pFcda->vtype == 8)
			AddChannel(chnname, GOOSE_CHN_BOOL,val);
		else if(pFcda->vtype == 7)
			AddChannel(chnname, GOOSE_CHN_Q,val);
		else
		{
			m_iChns--;
			LOGWARN("未知的GOOSE通道类型:%d",pFcda->vtype);
		}
		pFcda = pGooseOut->m_pDataset->m_Fcda.FetchNext(pos);
	}
	m_iReSendTimes = 10;
	RebuildBuffer();
}

CGooseSimulator::~CGooseSimulator()
{
	if(m_pBuffer != NULL)
		delete[] m_pBuffer;
}

CGooseSimulator::CChannel::CChannel()
{
	m_ValType = GOOSE_CHN_BOOL;
	
}

CGooseSimulator::CChannel::~CChannel()
{

}


//////////////////////////////////////////////////////////////////////////
// 描    述:  重新构建缓冲区
// 作    者:  邵凯田
// 创建时间:  2012-10-31 16:42
// 参数说明:  void
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CGooseSimulator::RebuildBuffer()
{
	int i,j;
	BYTE *pRow = m_pBuffer;
	BYTE *p;
	WORD w,wLenApp,wLenPdu,wLenAsdu,wLenData,wSeqLen;
	stuGoose_Head1 *pHead1 = (stuGoose_Head1*)pRow;
	int val;
	float fval;
	int *pi;
	m_iFrameLen = sizeof(stuGoose_Head1);
	SDateTime::getSystemTime(m_iNextSendTimeSoc,m_iNextSendTimeUSec);
	SDateTime::getSystemTime(m_iEventTimeSoc,m_iEventTimeNSec);
	if(m_iReSendTimes == 0)
	{
		m_iST ++;
		m_iSQ = 0;
	}
	else
	{
		m_iST=m_iSQ=1;
	}
	m_Chns.lock();
	//dst mac
	memcpy(pHead1->DstMac,m_DstMac,6);
	//src mac
	memcpy(pHead1->SrcMac,m_SrcMac,6);
	pHead1->VLan[0] = 0x81;
	pHead1->VLan[1] = 0x00;
	pHead1->VLan_Val[0] = (m_iVLanP<<5)|((m_iVLanId>>8)&0x0f);
	pHead1->VLan_Val[1] = (BYTE)m_iVLanId;
	//88b8
	pHead1->Type = htons(0x88b8);
	//appid
	pHead1->AppID = htons(m_iAppid);
	//length
	//pHead1->AppLen = htons(wLenApp);
	//r1
	pHead1->Res1 = htons(m_iReserver1);
	//r2
	pHead1->Res2 = htons(m_iReserver2);
	//pdu length
	pHead1->PDU_Len[0] = 0x61;
	pHead1->PDU_Len[1] = 0x82;
	//pHead1->PDU_Len[2] = (BYTE)(wLenPdu>>8);
	//pHead1->PDU_Len[3] = (BYTE)(wLenPdu&0xff);

	pRow += sizeof(stuGoose_Head1);
	BYTE *pPduBegin = pRow;

	//gocbRef
	pRow[0] = 0x80;
	pRow[1] = m_gocb.length();
	pRow+=2;
	memcpy(pRow,m_gocb.data(),m_gocb.length());
	pRow += m_gocb.length();

	//timeAllowedtoLive
	pRow[0] = 0x81;
	pRow[1] = 0x02;
	pRow[2] = (BYTE)(m_iTTL>>8);
	pRow[3] = (BYTE)(m_iTTL&0xFF);
	pRow+=4;

	//datSet
	pRow[0] = 0x82;
	pRow[1] = m_dataset.length();
	pRow+=2;
	memcpy(pRow,m_dataset.data(),m_dataset.length());
	pRow += m_dataset.length();

	//goid
	pRow[0] = 0x83;
	pRow[1] = m_goID.length();
	pRow+=2;
	memcpy(pRow,m_goID.data(),m_goID.length());
	pRow += m_goID.length();

	//t
	pRow[0] = 0x84;
	pRow[1] = 0x08;
	pRow += 2;
	m_iEventTimePos = pRow-m_pBuffer;
	int soc = htonl(m_iEventTimeSoc);
	int fraction = htonl((int)((((double)m_iEventTimeNSec)/1000000)*0x01000000));
	//usec = (int)((((double)fraction)/0x01000000) * 1000000)
	memcpy(pRow,&soc,4);
	memcpy(pRow+4,&fraction,4);
	pRow+=8;

	//stNum
	pRow[0] = 0x85;
	pRow[1] = 0x02;
	pRow[2] = (BYTE)(m_iST>>8);
	pRow[3] = (BYTE)(m_iST&0xff);
	pRow += 2;
	m_iStPos = pRow-m_pBuffer;
	pRow += 2;

	//sqNum
	pRow[0] = 0x86;
	pRow[1] = 0x02;
	pRow[2] = (BYTE)(m_iSQ>>8);
	pRow[3] = (BYTE)(m_iSQ&0xff);
	pRow += 2;
	m_iSqPos = pRow-m_pBuffer;
	pRow += 2;

	//test
	pRow[0] = 0x87;
	pRow[1] = 0x01;
	pRow[2] = (BYTE)(m_bTest?1:0);
	pRow += 2;
	m_iTestPos = pRow-m_pBuffer;
	pRow += 1;

	//confRev
	pRow[0] = 0x88;
	pRow[1] = 0x01;
	pRow[2] = (BYTE)(m_iConfRev);
	pRow += 3;

	//ndsCom
	pRow[0] = 0x89;
	pRow[1] = 0x01;
	pRow[2] = (BYTE)(m_iNdsCom);
	pRow += 3;

	//numdatseentries
	pRow[0] = 0x8a;
	pRow[1] = 0x01;
	pRow[2] = (BYTE)(m_iChns);
	pRow += 3;

	//alldata
	pRow[0] = 0xab;
	pRow[1] = 0x82;
	pRow += 2;
	BYTE *pAllDatLenBegin = pRow;
	pRow[2] = (BYTE)(0);
	pRow[3] = (BYTE)(0);
	pRow += 2;
	
	SString stval;
	unsigned long pos;
	CChannel *pChn = m_Chns.FetchFirst(pos);
	while(pChn)
	{
		if(pChn->m_ValType == GOOSE_CHN_BOOL)
		{
			pRow[0] = 0x80+pChn->m_ValType;
			pRow[1] = 0x01;
			pRow+=2;
			pChn->m_iOffsetInFrameBuffer = pRow - m_pBuffer;
			stval = pChn->m_sValue;
			if(stval == "true")
				stval = "1";
			else
				stval = "0";
			pRow[0] = (BYTE)stval.toInt();
			pRow++;
		}
		else if(pChn->m_ValType == GOOSE_CHN_BITSTR)
		{
			//固定认为是双点类型，两个BIT
			pRow[0] = 0x80+pChn->m_ValType;
			pRow[1] = 0x02;
			pRow+=2;
			pChn->m_iOffsetInFrameBuffer = pRow - m_pBuffer;
			pRow[0] = (BYTE)6;//padding
			stval = pChn->m_sValue;
			if(stval == "true")
				pRow[1] = 0x80;//stval = "2";
			else
				pRow[1] = 0x40;//stval = "1";
			//pRow[1] = (BYTE)(stval.toInt());
			pRow+=2;
		}
		else if(pChn->m_ValType == GOOSE_CHN_UTC)
		{
			pRow[0] = 0x80+pChn->m_ValType;
			pRow[1] = 0x08;
			pRow+=2;
			pChn->m_iOffsetInFrameBuffer = pRow - m_pBuffer;
#if 1
			SString t = SString::GetAttributeValue(pChn->m_sValue,"t");
			int soc=0;
			if(t.length() == 19)
				soc = SDateTime::makeDateTime("yyyy-MM-dd hh:mm:ss",t).soc();
			soc = htonl(soc);
			int usec=SString::GetAttributeValueI(pChn->m_sValue,"usec");
#endif
#if 0
			int soc = 0;
			int usec = 0;
			if (!pChn->m_sValue.isEmpty())
			{
				QString strUtc = pChn->m_sValue.data();
				QStringList strList = strUtc.split(" ");
				if (strList.size() >=2)
				{
					QString strDateTime = strList.at(0) + " " + strList.at(1);
					soc = SDateTime::makeDateTime("yyyy-MM-dd hh:mm:ss",strDateTime.toStdString().data()).soc();
					soc = htonl(soc);
					if (strList.size()== 3)
					{
						usec = strList.at(2).toInt();
					}
				} else
				{
					soc = strUtc.toInt();
				}
				
			} else
			{
				soc = htonl(soc);
			}
			
		
#endif
			int itmp = (int)((((double)usec)/1000000)*0x01000000);
			int fraction = htonl((int)((((double)usec)/1000000)*0x01000000));
			//usec = (int)((((double)fraction)/0x01000000) * 1000000)
			memcpy(pRow,&soc,4);
			memcpy(pRow+4,&fraction,4);
			pRow+=8;
		}
		else if(pChn->m_ValType == GOOSE_CHN_FLOAT)
		{
			pRow[0] = 0x80+pChn->m_ValType;
			pRow[1] = 0x04;
			pRow+=2;
			pChn->m_iOffsetInFrameBuffer = pRow - m_pBuffer;
			float f = pChn->m_sValue.toFloat();
			SKT_SWAP_FLOAT_REV(f);			
			memcpy(pRow,&f,4);
			pRow+=4;
		}
		else if(pChn->m_ValType == GOOSE_CHN_INT || pChn->m_ValType == GOOSE_CHN_UINT)
		{
			pRow[0] = 0x80+pChn->m_ValType;
			pRow[1] = 0x04;
			pRow+=2;
			pChn->m_iOffsetInFrameBuffer = pRow - m_pBuffer;
			int f = pChn->m_sValue.toInt();
			SKT_SWAP_DWORD_REV(f);			
			memcpy(pRow,&f,4);
			pRow+=4;
		}
		else if(pChn->m_ValType == GOOSE_CHN_STRING)
		{
			pRow[0] = 0x80+pChn->m_ValType;
			BYTE len = (BYTE)pChn->m_sValue.length();
			pRow[1] = len;
			pRow+=2;
			memcpy(pRow,pChn->m_sValue.data(),len);
			pRow+=len;
		}
		else if(pChn->m_ValType == GOOSE_CHN_Q)
		{
			pRow[0] = 0x80+GOOSE_CHN_BITSTR;
			//固定认为是32位类型，32个BIT
			pRow[1] = 0x05;
			pRow+=2;
			pChn->m_iOffsetInFrameBuffer = pRow - m_pBuffer;
			pRow[0] = 0x00;
			int f = pChn->m_sValue.toInt();
			SKT_SWAP_DWORD_REV(f);			
			memcpy(pRow+1,&f,4);
			pRow+=5;
		}
		else
		{
			LOGWARN("不支持的GOOSE通道类型:%d,将当成BOOLEAN处理",pChn->m_ValType);
			pRow[0] = 0x80+GOOSE_CHN_BOOL;
			pRow[1] = 0x01;
			pRow+=2;
			pChn->m_iOffsetInFrameBuffer = pRow - m_pBuffer;
			pRow[0] = (BYTE)pChn->m_sValue.toInt();
			pRow++;
		}
		pChn = m_Chns.FetchNext(pos);
	}
	m_iFrameLen = pRow - m_pBuffer;

	WORD len;
	len = pRow-pAllDatLenBegin-2;
	pAllDatLenBegin[0] = (BYTE)(len>>8);
	pAllDatLenBegin[1] = (BYTE)(len&0xff);

	wLenPdu = pRow-pPduBegin;
	pHead1->PDU_Len[2] = (BYTE)(wLenPdu>>8);
	pHead1->PDU_Len[3] = (BYTE)(wLenPdu&0xff);

	len = m_iFrameLen-18;
	pHead1->AppLen = htons(len);
	m_Chns.unlock();
	return;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  添加一个新的通道
// 作    者:  邵凯田
// 创建时间:  2016-01-07 09:42
// 参数说明:  @eType为类型
//            @iValue为当前值
//            @q为品质
// 返 回 值:  新通道指针
//////////////////////////////////////////////////////////////////////////
CGooseSimulator::CChannel* CGooseSimulator::AddChannel(SString chnname,eGooseValType vtype,SString sValue)
{
	CChannel *pChn = new CChannel();
	pChn->m_sChnName = chnname;
	pChn->m_ValType = vtype;
	pChn->m_sValue = sValue;
	m_Chns.append(pChn);
	return pChn;
}


//////////////////////////////////////////////////////////////////////////
// 名    称:  CGooseSimPort
// 作    者:  邵凯田
// 创建时间:  2016-01-07 09:57
// 描    述:  GOOSE模拟器端口，一个端口可以发送多个GOOSE
//////////////////////////////////////////////////////////////////////////
CGooseSimPort::CGooseSimPort(SString sDevName)
{
	m_Gooses.setAutoDelete(true);
	m_Gooses.setShared(true);
	m_sDevName = sDevName;
}

CGooseSimPort::~CGooseSimPort()
{
}

////////////////////////////////////////////////////////////////////////
// 描    述:  启动服务,虚函数，派生函数中必须先调用此函数
// 作    者:  邵凯田
// 创建时间:  2016-01-07 09:57
// 参数说明:  void
// 返 回 值:  true表示服务启动成功,false表示服务启动失败
//////////////////////////////////////////////////////////////////////////
bool CGooseSimPort::Start()
{
	if(!SService::Start())
		return false;
	if(!m_Sender.Create(m_sDevName))
	{
		LOGERROR("在设备[%s]上创建原始发送套接字失败!",m_sDevName.data());
		return false;
	}
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
// 创建时间:  2016-01-07 09:57
// 参数说明:  void
// 返 回 值:  true表示服务停止成功,false表示服务停止失败
//////////////////////////////////////////////////////////////////////////
bool CGooseSimPort::Stop()
{
	SService::Stop();
	m_Sender.Close();
	return true;
}

void* CGooseSimPort::ThreadSend(void *lp)
{
	CGooseSimPort* pThis = (CGooseSimPort*)lp;
	int soc1,usec1;
	int usecs;
	pThis->BeginThread();
	CGooseSimulator *pGoose;
	unsigned long pos;
	while(!pThis->IsQuit())
	{
		SDateTime::getSystemTime(soc1,usec1);
		pThis->m_Gooses.lock();
		pGoose = pThis->m_Gooses.FetchFirst(pos);
		while(pGoose && !pThis->IsQuit())
		{
			if(pGoose->m_iReSendTimes == 0 || soc1 > pGoose->m_iNextSendTimeSoc || (soc1 == pGoose->m_iNextSendTimeSoc && usec1 >= pGoose->m_iNextSendTimeUSec))
			{
				SDateTime::getSystemTime(soc1,usec1);
				pGoose->m_Chns.lock();
				pThis->m_Sender.SendRaw(pGoose->m_pBuffer,pGoose->m_iFrameLen);
				pGoose->m_Chns.unlock();
				pGoose->m_iNextSendTimeSoc = soc1;
				pGoose->m_iNextSendTimeUSec = usec1;
				if(pGoose->m_iReSendTimes == 0)
				{
					pGoose->AddNextSendMs(2);
				}
				else if(pGoose->m_iReSendTimes == 1)
				{
					pGoose->AddNextSendMs(2);
				}
				else if(pGoose->m_iReSendTimes == 2)
				{
					pGoose->AddNextSendMs(4);
				}
				else if(pGoose->m_iReSendTimes == 3)
				{
					pGoose->AddNextSendMs(8);
				}
				else
					pGoose->AddNextSendMs(pGoose->m_iMaxSendMsec);
				pGoose->m_iReSendTimes++;
				pGoose->m_iSQ++;
				if(pGoose->m_iSQ >= 0xfff0)
					pGoose->m_iSQ = 1;
				pGoose->m_pBuffer[pGoose->m_iSqPos] = (BYTE)(pGoose->m_iSQ>>8);
				pGoose->m_pBuffer[pGoose->m_iSqPos+1] = (BYTE)(pGoose->m_iSQ&0xff);
			}
			pGoose = pThis->m_Gooses.FetchNext(pos);
		}
		pThis->m_Gooses.unlock();
		SApi::UsSleep(100);
	}
	pThis->EndThread();
	return NULL;
}
bool CGooseSimPort::CheckGooseOutIsExist(stuSclVtIedGooseOut* pCur)
{
	if (pCur == NULL)
	{//当指针为空时，返回true，防止外部调用该函数判断将对象添加对模拟器列表中
		return true;
	}
	for(int i = 0; i < m_Gooses.count(); i ++)
	{
		CGooseSimulator* pCurGooseSim = m_Gooses.at(i);
		if (pCurGooseSim == NULL)
		{
			continue;
		}
		if (pCur == pCurGooseSim->m_pGooseOut)
		{
			m_Gooses.remove(i);
			return true;
		}
	}
	return false;
}
//////////////////////////////////////////////////////////////////////////
// 名    称:  CGooseSimManager
// 作    者:  邵凯田
// 创建时间:  2016-01-07 10:01
// 描    述:  GOOSE模拟器管理器
//////////////////////////////////////////////////////////////////////////
CGooseSimManager::CGooseSimManager()
{

}

CGooseSimManager::~CGooseSimManager()
{

}

//////////////////////////////////////////////////////////////////////////
// 描    述:  查找指针网卡名的端口实例，不存在则新创建
// 作    者:  邵凯田
// 创建时间:  2016-01-07 10:01
// 参数说明:  @sDevName为网卡名
// 返 回 值:  CGooseSimPort*
//////////////////////////////////////////////////////////////////////////
CGooseSimPort* CGooseSimManager::SearchPort(SString sDevName)
{
	unsigned long pos;
	CGooseSimPort *pPort = m_Ports.FetchFirst(pos);
	while(pPort)
	{
		if(pPort->m_sDevName == sDevName)
			break;
		pPort = m_Ports.FetchNext(pos);
	}
	if(pPort == NULL)
	{
		pPort = new CGooseSimPort(sDevName);
		pPort->Start();
		m_Ports.append(pPort);
	}
	return pPort;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  添加一个新的GOOSE
// 作    者:  邵凯田
// 创建时间:  2016-01-07 10:01
// 参数说明:  sDevName为网卡名
//            @pGoose为初始化后的GOOSE实例
// 返 回 值:  CGooseSimulator*
//////////////////////////////////////////////////////////////////////////
CGooseSimulator* CGooseSimManager::AddGoose(SString sDevName,CGooseSimulator *pGoose)
{
	CGooseSimPort *pPort = SearchPort(sDevName);
	pPort->m_Gooses.append(pGoose);
	return pGoose;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  停止全部发送，释放对象
// 作    者:  邵凯田
// 创建时间:  2016-01-07 10:01
// 参数说明:  void
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CGooseSimManager::Clear()
{
	unsigned long pos;
	CGooseSimPort *pPort = m_Ports.FetchFirst(pos);
	while(pPort)
	{
		pPort->StopAndWait();
		pPort = m_Ports.FetchNext(pos);
	}
	m_Ports.clear();
}


