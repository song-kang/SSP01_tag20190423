/**
 *
 * 文 件 名 : sim_manager.cpp
 * 创建日期 : 2016-1-5 9:47
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : 模拟管理器类
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2016-1-5	邵凯田　创建文件
 *
 **/

#include "sim_manager.h"

CSimManager::CSimManager()
{
	m_pSimConfig = NULL;
}

CSimManager::~CSimManager()
{
	m_MuSimMgr.Clear();
	m_GooseSimMGr.Clear();
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  启动管理器
// 作    者:  邵凯田
// 创建时间:  2016-1-5 9:50
// 参数说明:  void
// 返 回 值:  true/false
//////////////////////////////////////////////////////////////////////////
bool CSimManager::Start()
{
	m_MuSimMgr.Clear();
	m_GooseSimMGr.Clear();
	unsigned long pos1,pos2;

	//mu
	stuSimSubnetwork *pNet = m_pSimConfig->m_SubNet.FetchFirst(pos1);
	while(pNet)
	{
		if(pNet->m_Smv.count() > 0)
		{
			CMUSimPort *pPort = m_MuSimMgr.SearchPort(pNet->name);
			stuSclVtIedSmvOut *pOut = pNet->m_Smv.FetchFirst(pos2);
			while(pOut)
			{
				CMUSimulator *pMu = new CMUSimulator(pOut,pNet->mac);
				m_MuSimMgr.AddMU(pNet->name,pMu);
				pOut = pNet->m_Smv.FetchNext(pos2);
			}
			pPort->Start();
		}
		pNet = m_pSimConfig->m_SubNet.FetchNext(pos1);
	}

	//goose
	pNet = m_pSimConfig->m_SubNet.FetchFirst(pos1);
	while(pNet)
	{
		if(pNet->m_Goose.count() > 0)
		{
			CGooseSimPort *pPort = m_GooseSimMGr.SearchPort(pNet->name);
			stuSclVtIedGooseOut *pOut = pNet->m_Goose.FetchFirst(pos2);
			while(pOut)
			{
				if(pOut->m_pDataset != NULL)
				{
					CGooseSimulator *pGoose = new CGooseSimulator(pOut,pNet->mac);
					m_GooseSimMGr.AddGoose(pNet->name,pGoose);
				}
				pOut = pNet->m_Goose.FetchNext(pos2);
			}
			pPort->Start();
		}
		pNet = m_pSimConfig->m_SubNet.FetchNext(pos1);
	}

	
	return true;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  停止管理器
// 作    者:  邵凯田
// 创建时间:  2016-1-5 9:51
// 参数说明:  void
// 返 回 值:  true/false
//////////////////////////////////////////////////////////////////////////
bool CSimManager::Stop()
{
	m_MuSimMgr.Clear();
	m_GooseSimMGr.Clear();
	return true;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  触发指定的GOOSE状态变位
// 作    者:  邵凯田
// 创建时间:  2017-4-5 18:17
// 参数说明:  @appid
//         :  @vals
// 返 回 值:  true/false
//////////////////////////////////////////////////////////////////////////
bool CSimManager::DoGooseChg(int appid,SString vals)
{
	unsigned long pos1,pos2,pos3;
	stuSimSubnetwork *pNet = m_pSimConfig->m_SubNet.FetchFirst(pos1);
	while(pNet)
	{
		CGooseSimPort *pPort = m_GooseSimMGr.SearchPort(pNet->name);
		if(pPort != NULL)
		{
			CGooseSimulator *pGoose = pPort->m_Gooses.FetchFirst(pos2);
			while(pGoose)
			{
				if(pGoose->m_iAppid == appid)
				{
					//匹配到当前的APPID
					//ch_name1=val1;ch_name2=val2;
					SString sLine,sName,sVal;
					int i,cnt = SString::GetAttributeCount(vals,";");
					for(i=1;i<=cnt;i++)
					{
						sLine = SString::GetIdAttribute(i,vals,";");
						if(sLine.length() == 0)
							continue;
						sName = "$"+SString::GetIdAttribute(1,sLine,"=")+"$";
						sVal  = SString::GetIdAttribute(2,sLine,"=");
						//查找sName
						CGooseSimulator::CChannel *pChn = pGoose->m_Chns.FetchFirst(pos3);
						while(pChn)
						{
							if(pChn->m_sChnName.find(sName)>=0)
							{
								//find it
								pChn->m_sValue = sVal;
							}
							pChn = pGoose->m_Chns.FetchNext(pos3);
						}
					}
					pGoose->m_iReSendTimes = 0;
					pGoose->RebuildBuffer();
				}
				pGoose = pPort->m_Gooses.FetchNext(pos2);
			}
		}
		pNet = m_pSimConfig->m_SubNet.FetchNext(pos1);
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  触发指定的SV值变化
// 作    者:  邵凯田
// 创建时间:  2017-4-5 18:17
// 参数说明:  @appid
//         :  @ch_idx
//         :  @rms
//         :  @q
// 返 回 值:  true/false
//////////////////////////////////////////////////////////////////////////
bool CSimManager::DoSvChg(int appid,int ch_idx,float rms,int q)
{
	unsigned long pos1,pos2;
	stuSimSubnetwork *pNet = m_pSimConfig->m_SubNet.FetchFirst(pos1);
	while(pNet)
	{
		CMUSimPort *pPort = m_MuSimMgr.SearchPort(pNet->name);
		if(pPort != NULL)
		{
			CMUSimulator *pMu = pPort->m_MUs.FetchFirst(pos2);
			while(pMu)
			{
				if(pMu->m_iAppid == appid)
				{
					//find it
					CMUSimulator::CChannel *pChn = pMu->m_Chns[ch_idx];
					if(pChn != NULL)
					{
						pMu->UpdateChnData(pChn,(int)rms);
						pMu->RebuildBuffer();
					}
				}
				pMu = pPort->m_MUs.FetchNext(pos2);
			}
		}
		pNet = m_pSimConfig->m_SubNet.FetchNext(pos1);
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  修改指定的叶子节点值内容
// 作    者:  邵凯田
// 创建时间:  2017-4-5 18:21
// 参数说明:  @mms_path
//         :  @vt
//         :  @val
// 返 回 值:  true/false
//////////////////////////////////////////////////////////////////////////
bool CSimManager::DoSetLeaf(SString ied_ld,SString leaf_path,int vt,SString val)
{
	stuLeafMap *pMap = m_MmsServer.SearchLeafMap(ied_ld.data(), leaf_path.data());
	SString tip;
	if(pMap != NULL)
	{
		tip.sprintf("映射节点不存在[%s/%s]!",ied_ld.data(), leaf_path.data());
		m_MmsServer.PushNewReport(pMap);
	}
	//vt为值类型(1-bool,2-int,3-float,4-utctime)
	bool ret = false;
	if(/*vt == 4 && */val.find("$(now)") >= 0)
	{
		SDateTime now = SDateTime::currentDateTime();
		val = val.replace("$(now)",now.toString("yyyy-MM-dd hh:mm:ss.")+SString::toFormat("%06d",now.msec()*1000+now.usec()));
		//val = now.toString("yyyy-MM-dd hh:mm:ss.")+SString::toFormat("%06d",now.msec()*1000+now.usec());
	}
	ret = m_MmsServer.SetLeafVarValueByPath(ied_ld+"/"+leaf_path,val);
	tip += SString::toFormat("赋值[%s]%s",val.data(),ret?"成功":"失败");
	SSP_GUI->SetStatusTip(tip);
	/*
	switch(vt)
	{
	case 1:
	case 2:
		ret = m_MmsServer.SetLeafVarValue(pMap,val.toInt());
		break;
	case 3:
		ret = m_MmsServer.SetLeafVarValue(pMap,val.toFloat());
		break;
	case 4:
		if(val == "now")
		{
			int soc,usec;
			SDateTime::getSystemTime(soc,usec);
			ret = m_MmsServer.SetLeafVarValue(pMap,soc,usec);
		}
		else
		{
			ret = m_MmsServer.SetLeafVarValue(pMap,SString::GetIdAttributeI(1,val,","),SString::GetIdAttributeI(2,val,","));
		}
		break;
	}
	*/
	return ret;
}
