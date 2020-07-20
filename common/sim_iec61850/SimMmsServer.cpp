/**
 *
 * 文 件 名 : SimMmsServer.h
 * 创建日期 : 2016-9-2 14:27
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : MMS服务端实现类，使用SMmsServer库
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2016-9-2	邵凯田　创建文件
 *
 **/
#include "SimMmsServer.h"
#include <math.h>

CSimMmsServer::CSimMmsServer()
{

}

CSimMmsServer::~CSimMmsServer()
{

}

////////////////////////////////////////////////////////////////////////
// 描    述:  启动服务,虚函数，派生函数中必须先调用此函数
// 作    者:  邵凯田
// 创建时间:  2016-9-2 14:27
// 参数说明:  void
// 返 回 值:  true表示服务启动成功,false表示服务启动失败
//////////////////////////////////////////////////////////////////////////
bool CSimMmsServer::Start()
{
	if(!SMmsServer::Start())
		return false;
	//SKT_CREATE_THREAD(ThreadSim,this);
	return true;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  映射叶子节点用户
// 作    者:  邵凯田
// 创建时间:  2016-9-2 14:27
// 参数说明:  @sIedLdName表示IED+LD名称
//         :  @sLeafPath表示叶子节点路径
//         :  @ppLeafMap表示叶子节点的用户映射指针的地址，映射指针为空，用户层决定是否需要映射，需要时创建stuLeafMap对象并填写指针，MMS服务内部不负责释放
// 返 回 值:  true表示成功映射，false表示无法映射
//////////////////////////////////////////////////////////////////////////
bool CSimMmsServer::OnMapLeafToUser(char* sIedLdName, char* sLeafPath, stuLeafMap **ppLeafMap)
{
	if(strstr(sLeafPath,"$stVal") != NULL || strstr(sLeafPath,"$t") != NULL)
	{
		*ppLeafMap = new stuLeafMap();
		m_StateLeafMap.append(*ppLeafMap);
		return true;
	}
	else if(strstr(sLeafPath,"$f") != NULL)
	{
		*ppLeafMap = new stuLeafMap();
		m_AnalogLeafMap.append(*ppLeafMap);
		return true;
	}
	return false;
}


//////////////////////////////////////////////////////////////////////////
// 描    述:  添加访问点IP地址
// 作    者:  邵凯田
// 创建时间:  2016-11-12 14:42
// 参数说明:  void
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CSimMmsServer::AddApIpAddr()
{
	unsigned long pos,pos2,pos3;
	int idx = 10;
	SString local_netcard;
	stuSclVtSubNetwork *p = m_pSimConfig->m_VirtualTerminal.m_SubNetwork.FetchFirst(pos);
	while(p)
	{
		local_netcard = SString::GetAttributeValue(m_pSimConfig->m_sMmsExtAttribute,p->name);
		if(strstr(p->type,"MMS") != NULL && local_netcard.length() > 0)
		{			
			stuSclVtSubNetworkConnectAP *pAP = p->m_ConnectAP.FetchFirst(pos2);
			while(pAP)
			{
				stuSclVtSubNetworkConnectAPAddress *pAddr = pAP->m_Address.FetchFirst(pos3);
				while(pAddr)
				{
					if(strlen(pAddr->ip) > 0 && strlen(pAddr->ip_subnet) > 0)
					{
						::system(SString::toFormat("ifconfig %s:%d %s netmask %s",local_netcard.data(),idx++,pAddr->ip,pAddr->ip_subnet).data());
					}
					pAddr = pAP->m_Address.FetchNext(pos3);
				}
				pAP = p->m_ConnectAP.FetchNext(pos2);
			}
		}
		p = m_pSimConfig->m_VirtualTerminal.m_SubNetwork.FetchNext(pos);
	}
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  删除访问点IP
// 作    者:  邵凯田
// 创建时间:  2016-11-12 14:42
// 参数说明:  void
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CSimMmsServer::RemoveApIpAddr()
{
	unsigned long pos,pos2,pos3;
	int idx = 10;
	SString local_netcard;
	stuSclVtSubNetwork *p = m_pSimConfig->m_VirtualTerminal.m_SubNetwork.FetchFirst(pos);
	while(p)
	{
		local_netcard = SString::GetAttributeValue(m_pSimConfig->m_sMmsExtAttribute,p->name);
		if(strstr(p->type,"MMS") != NULL && local_netcard.length() > 0)
		{			
			stuSclVtSubNetworkConnectAP *pAP = p->m_ConnectAP.FetchFirst(pos2);
			while(pAP)
			{
				stuSclVtSubNetworkConnectAPAddress *pAddr = pAP->m_Address.FetchFirst(pos3);
				while(pAddr)
				{
					if(strlen(pAddr->ip) > 0 && strlen(pAddr->ip_subnet) > 0)
					{
						::system(SString::toFormat("ifconfig %s:%d down",local_netcard.data(),idx++).data());
					}
					pAddr = pAP->m_Address.FetchNext(pos3);
				}
				pAP = p->m_ConnectAP.FetchNext(pos2);
			}
		}
		p = m_pSimConfig->m_VirtualTerminal.m_SubNetwork.FetchNext(pos);
	}
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  模拟线程，定时进行信号或测量值的变化
// 作    者:  邵凯田
// 创建时间:  2016-9-2 14:39
// 参数说明:  @lp
// 返 回 值:  NULL
//////////////////////////////////////////////////////////////////////////
void* CSimMmsServer::ThreadSim(void *lp)
{
	CSimMmsServer* pThis = (CSimMmsServer*)lp;
	unsigned long pos;
	int st=0;
	float f;
	char *ld_name;
	int soc,usec;
	pThis->BeginThread();
	SApi::UsSleep(60000000);
	while(!pThis->IsQuit())
	{
		st = st==0?1:0;
		int cnt = 0;
		if(pThis->m_pSimConfig->m_iMmsStTimes > 0)
		{
			stuLeafMap *pLeaf = pThis->m_StateLeafMap.FetchFirst(pos);
			while(!pThis->IsQuit() && pLeaf && cnt < pThis->m_pSimConfig->m_iMmsStTimes)
			{
				//if(rand()%10 == 0 && cnt < pThis->m_pSimConfig->m_iMmsStTimes /*&& strstr(pThis->GetLeafMapIedLdName(pLeaf),"PT1001A") != NULL*/)
				{
					SDateTime::getSystemTime(soc,usec);
					if(strstr(pThis->GetLeafMapLeaf(pLeaf),"$t")!= NULL)
					{
						pThis->NewReportPush(pLeaf,soc,usec);
					}
					else
					{
						pThis->NewReportPush(pLeaf,st);
					}
					cnt++;
				}
				pLeaf = pThis->m_StateLeafMap.FetchNext(pos);
			}
			pThis->NewReportProc();

		}
		if(pThis->m_pSimConfig->m_iMmsMxTimes > 0)
		{
			cnt = 0;
			stuLeafMap *pLeaf = pThis->m_AnalogLeafMap.FetchFirst(pos);
			while(!pThis->IsQuit() && pLeaf && cnt < pThis->m_pSimConfig->m_iMmsMxTimes)
			{
				//if(rand()%10 == 0 && cnt < pThis->m_pSimConfig->m_iMmsMxTimes /* && strstr(pThis->GetLeafMapIedLdName(pLeaf),"PT1001A") != NULL*/)
				{
					int st = rand()%1000;
					if(st == 0)
						st = 10;
					f = (rand()%10000)/st;
					pThis->NewReportPush(pLeaf,f);
					cnt ++;
				}
				pLeaf = pThis->m_AnalogLeafMap.FetchNext(pos);
			}
			pThis->NewReportProc();

		}
		SApi::UsSleep(1000000);
	}
	pThis->EndThread();
	return NULL;
}


