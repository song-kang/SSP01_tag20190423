/**
 *
 * 文 件 名 : SimClient.cpp
 * 创建日期 : 2018-8-22 9:59
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : 仿真客户端
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2018-8-22	邵凯田　创建文件
 *
 **/

#include "SimClient.h"
#include "view_home.h"
#include <QDebug>
CSimClient::CSimClient()
{
	SetStartWord(0x35E7);
	m_strModuleName = "sim_iec61850_server";
}
CSimClient::~CSimClient()
{

}

////////////////////////////////////////////////////////////////////////
// 描    述:  以服务形式启动客户端，不能与Connect函数同时使用
// 作    者:  邵凯田
// 创建时间:  2018-8-22 9:58
// 参数说明:  @ip表示服务端IP地址，@port表示端口
// 返 回 值:  true表示服务启动成功,false表示服务启动失败
//////////////////////////////////////////////////////////////////////////
bool CSimClient::Start(SString module_name,SString module_desc,SString ip,int port)
{
	SString str;
	str.sprintf("module_name=%s;module_desc=%s;",module_name.data(),module_desc.data());
	SetLoginInfo(str);
	qDebug() << str << "ip"<<ip.data()<<"port"<<port;
	return STcpClientBase::Start(ip,port);
}

////////////////////////////////////////////////////////////////////////
// 描    述:  停止服务
// 作    者:  邵凯田
// 创建时间:  2018-8-22 9:58
// 参数说明:  void
// 返 回 值:  true表示服务停止成功,false表示服务停止失败
//////////////////////////////////////////////////////////////////////////
bool CSimClient::Stop()
{
	qDebug() << "停止服务！";
	return STcpClientBase::Stop();
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  登录上服务端后的回调
// 作    者:  邵凯田
// 创建时间:  2018-8-22 9:58
// 参数说明:  void
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CSimClient::OnLogin()
{
	LOGDEBUG("登录服务端成功!");
	//发送注册命令
	sendRegisterInfo();
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  断开服务端后的回调
// 作    者:  邵凯田
// 创建时间:  2018-8-22 9:58
// 参数说明:  void
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CSimClient::OnClose()
{

}

////////////////////////////////////////////////////////////////////////
// 描    述:  会话线程接收到报文后的回调虚函数，派生类通过此函数处理即时消息
// 作    者:  邵凯田
// 创建时间:  2018-8-22 9:58
// 参数说明:  @pSession为会话实例指针，可以通过指针向对端发送数据
//            @pPackage为刚接收到的数据包
// 返 回 值:  true表示已经处理完毕，此报文可以释放了，false表示未处理，此报文应放入接收队列
//////////////////////////////////////////////////////////////////////////
bool CSimClient::OnRecvFrame(stuSTcpPackage *pPackage)
{
	if(pPackage->m_wFrameType == PKG_RUN_FUN_REQ)
	{
		SString func = SString::GetAttributeValue(pPackage->m_sHead,"func");
		SString args = SString::GetAttributeValue(pPackage->m_sHead,"args");

		int ret = RunStep(func,args);
		SString sHead = SString::toFormat("result=%d;",ret) + pPackage->m_sHead;
		qDebug() << "1111111111--:" << sHead.data();
		SendFrame(sHead,PKG_RUN_FUN_RES);
	}
#if 0
	else if (pPackage->m_wFrameType == PKG_REGISTER_RES)
	{
		return ProcessSimPackageType2(pPackage->m_sHead);
	} else if (pPackage->m_wFrameType == PKG_ABILITY_REQ)
	{
		return ProcessSimPackageType3(pPackage->m_sHead);
	}else if (pPackage->m_wFrameType == PKG_START_REQ)
	{
		return ProcessSimPackageType5(pPackage->m_sHead);
	}else if (pPackage->m_wFrameType == PKG_STOP_REQ)
	{
		return ProcessSimPackageType7(pPackage->m_sHead);
	}else if (pPackage->m_wFrameType == PKG_JOB_REQ)
	{
		return ProcessSimPackageType9(pPackage->m_sHead);
	}
#endif
	return true;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  执行指定的功能项，并返回执行结果
// 作    者:  邵凯田
// 创建时间:  2018-11-6 16:46
// 参数说明:  @func表示功能项名称
//         :  @args表示执行参数，多个参数使用逗号分隔
// 返 回 值:  >0表示执行失败：  1:功能项不存在
//								2:参数数量不足
//								3:参数格式不正确
//								4:参数无效
//								5:服务未启动
//								6:连接异常
//								7:执行失败
//			  =0表示执行成功
//////////////////////////////////////////////////////////////////////////
int CSimClient::RunStep(SString func,SString args)
{
	LOGDEBUG("RunStep(%s, %s)",func.data(),args.data());
	//TODO:在此进行步骤执行，并回复结果
	if (func == "61850_Ied_Param_Change")
	{
		return m_pViewHome->doExecCmd(args);
	}else if (func == "start")
	{
		return m_pViewHome->startServer()?0:7;
	} else if (func == "stop")
	{
		return m_pViewHome->stopServer()? 0:7;
	} else if (func == "func_info")
	{
		return ProcessSimPackageType3(args)?0:7;
	}

	return 1;
}


void CSimClient::setOptObj(view_home* pViewHome,ConfigFileMgr* pConfileMgr)
{
	m_pViewHome = pViewHome;
	m_pConfigFileMgr = pConfileMgr;
}


bool CSimClient::ProcessSimPackageType2(SString& strHead)
{
	//接受到服务端返回的注册结果，根据结果进行逻辑处理
	int iRest = SString::GetAttributeValue(strHead,"result").toInt();
	if (iRest == 0)
	{
		//socket是连接的，没有断开，则重新发送注册信息
		if (IsConnected())
		{
			return sendRegisterInfo();
		} else
		{
			return m_pViewHome->startSimClient();
		}
	} else
	{
		LOGDEBUG("注册成功!");
		return true;
	}
}

bool CSimClient::ProcessSimPackageType3(SString& strHead)
{
	//客户端回复自身能力描述信息(PKG_ABILITY_RES=4)
	stModule* pCurModule = m_pConfigFileMgr->getModuleByName(m_strModuleName);
	if (pCurModule == NULL)
	{
		return false;
	}
	//	sprintf(regCommand.name,"%s",pCurModule->strName.data());
	QVector<stuSimAbility> tmpList;

	for(int j = 0; j < pCurModule->vFuncList.count(); j++)
	{
		stFunc* pCurFunc = pCurModule->vFuncList.at(j);
		if (pCurFunc == NULL)
		{
			continue;
		}
		stuSimAbility tmp;
		tmp.ctrl_id = pCurFunc->strIdx.toInt();
		sprintf(tmp.ctrl_name,"%s",pCurFunc->strName.data());
		if (pCurFunc->strArgc.isEmpty())
		{
			tmp.param_num = 0;
			sprintf(tmp.param_names,"%s","");
		} else
		{
			tmp.param_num = pCurFunc->strName.find(";")+1;
			sprintf(tmp.param_names,"%s",pCurFunc->strArgcFmt.data());
		}
		tmpList.append(tmp);
	}

	int iNum = tmpList.count();
	stuSTcpPackage* pSend = new stuSTcpPackage;
	pSend->m_wFrameType = PKG_ABILITY_RES;
	pSend->m_sHead = strHead;
	pSend->m_iAsduLen = iNum;
	pSend->m_iAsduBufferLen = iNum * sizeof(stuSimAbility);
	char* pNewBuf = new char[pSend->m_iAsduBufferLen];
	memset(pNewBuf,0,pSend->m_iAsduBufferLen);
	char* pMemStart=pNewBuf;
	for (int i = 0; i < tmpList.count(); i++)
	{
		stuSimAbility tmp = tmpList.at(i);
		memcpy(pMemStart,&tmp,sizeof(tmp));
		pMemStart = pMemStart + sizeof(tmp);
	}
	return SendFrame(pSend);
}

bool CSimClient::ProcessSimPackageType5(SString& strHead)
{
	//客户端回复服务开启响应(PKG_START_RES=6)
	bool bRst = m_pViewHome->startServer();
	SString sRstHead;
	if (bRst)
	{
		sRstHead = SString::toFormat("result=%d;",1) + strHead;
	} else
	{
		sRstHead = SString::toFormat("result=%d;",0) + strHead;
	}
	return SendFrame(sRstHead,PKG_START_RES);
}

bool CSimClient::ProcessSimPackageType7(SString& strHead)
{
	//客户端回复服务停止响应(PKG_STOP_RES=8)
	bool bRst = m_pViewHome->stopServer();
	SString sRstHead;
	if (bRst)
	{
		sRstHead = SString::toFormat("result=%d;",1) + strHead;
	} else
	{
		sRstHead = SString::toFormat("result=%d;",0) + strHead;
	}
	return SendFrame(sRstHead,PKG_STOP_RES);
}

bool CSimClient::ProcessSimPackageType9(SString& strHead)
{
	//客户端执行完指定的作业后回复执行结果(PKG_JOB_RES=10)
	SString func = SString::GetAttributeValue(strHead,"func");
	SString args = SString::GetAttributeValue(strHead,"args");
	int ret = RunStep(func,args);
	SString sHead = SString::toFormat("result=%d;",ret) + strHead;
	return SendFrame(sHead,PKG_JOB_RES);
}

bool CSimClient::sendRegisterInfo()
{
	SString strHead;
	strHead.sprintf("cluster_id=%d,sim_code=%d,sim_name=%s",1,1,"sim_iec61850_server");
	return SendFrame(strHead,PKG_REGISTER_REQ);
}