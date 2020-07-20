/**
 *
 * 文 件 名 : BridgeSvr.cpp
 * 创建日期 : 2018-10-12 16:38
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : 
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2018-10-12	邵凯田　创建文件
 *
 **/

#include "BridgeSvr.h"

CSession::CSession(STcpServerSession *pServerSession)
{
	m_pSession = pServerSession;
	m_ChannelType = CHN_NONE;
	m_TcpSvrParams.setAutoDelete(true);
	m_TcpSvrParams.setShared(true);
}

CSession::~CSession()
{

}

////////////////////////////////////////////////////////////////////////
// 描    述:  启动服务,虚函数，派生函数中必须先调用此函数
// 作    者:  邵凯田
// 创建时间:  2018-10-12 18:38
// 参数说明:  void
// 返 回 值:  true表示服务启动成功,false表示服务启动失败
//////////////////////////////////////////////////////////////////////////
bool CSession::Start()
{
	if(!SService::Start())
		return false;

	return true;
}

////////////////////////////////////////////////////////////////////////
// 描    述:  停止服务，派生函数中必须先调用此函数
// 作    者:  邵凯田
// 创建时间:  2018-10-12 18:38
// 参数说明:  void
// 返 回 值:  true表示服务停止成功,false表示服务停止失败
//////////////////////////////////////////////////////////////////////////
bool CSession::Stop()
{
	if(!SService::Stop())
		return false;
	m_ListenSock.Close();
	return true;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  创建TCP服务通道
// 作    者:  邵凯田
// 创建时间:  2018-10-12 18:38
// 参数说明:  
// 返 回 值:  
//////////////////////////////////////////////////////////////////////////
bool CSession::CreateTcpSvr(SString &sExtAttr)
{
	int port = SString::GetAttributeValueI(sExtAttr,"port");
	if(!m_ListenSock.CreateTcp() || !m_ListenSock.Bind(port) || !m_ListenSock.Listen())
	{
		LOGERROR("无效侦听TCP端口:%d",port);
		return false;
	}
	LOGDEBUG("在端口%d创建桥接通道成功!",port);
	m_ChannelType = CHN_TCP_SERVER;
	SKT_CREATE_THREAD(ThreadTcpSvrListen,this);
	return true;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  处理从客户端接收到的数据包
// 作    者:  邵凯田
// 创建时间:  2018-10-12 20:51
// 参数说明:  @pPaakage
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CSession::ProcessClientPkg(stuSTcpPackage *pPackage)
{
	if(pPackage->m_wFrameType == PKG_CONTROL)
	{
		//控制命令
		LOGDEBUG("收到控制命令:%s",pPackage->m_sHead.data());
		SString act = SString::GetAttributeValue(pPackage->m_sHead,"act");
		if(act == "create_tcpsvr")
		{
			bool ret = CreateTcpSvr(pPackage->m_sHead);
			SString sHead;
			sHead.sprintf("act=create_tcpsvr;result=%d;",ret?1:0);
			m_pSession->AddToSend(m_pSession->NewPackage(sHead,PKG_CONTROL));
			LOGWARN("发送控制命令:%s",sHead.data());
		}
		else if(act == "pipbroken")
		{
			SString session_id = SString::GetAttributeValue(pPackage->m_sHead,"session_id");
			unsigned long pos;
			m_TcpSvrParams.lock();
			stuTcpSvrParam *p = m_TcpSvrParams.FetchFirst(pos);
			while(p)
			{
				if(p->session_id == session_id)
					break;
				p = m_TcpSvrParams.FetchNext(pos);
			}
			m_TcpSvrParams.unlock();
			if(p == NULL)
				return;
			p->pSessionSock->Close();
		}
	}
	else if(pPackage->m_wFrameType == PKG_DATA && pPackage->m_pAsduBuffer != NULL)
	{
		//LOGDEBUG("收到数据包(len=%d):%s",pPackage->m_iAsduLen,SString::HexToStr(pPackage->m_pAsduBuffer,pPackage->m_iAsduLen>500?500:pPackage->m_iAsduLen).data());
		SString session_id = SString::GetAttributeValue(pPackage->m_sHead,"session_id");
		if(m_ChannelType == CHN_TCP_SERVER)
		{
			unsigned long pos;
			m_TcpSvrParams.lock();
			stuTcpSvrParam *p = m_TcpSvrParams.FetchFirst(pos);
			while(p)
			{
				if(p->session_id == session_id)
					break;
				p = m_TcpSvrParams.FetchNext(pos);
			}
			m_TcpSvrParams.unlock();
			if(p == NULL)
				return;
			int ret = p->pSessionSock->Send(pPackage->m_pAsduBuffer,pPackage->m_iAsduLen);
			if(ret != pPackage->m_iAsduLen)
			{
				SString sHead;
				sHead.sprintf("act=pipbroken;session_id=%ld;",(long)p->pSessionSock);
				m_pSession->AddToSend(m_pSession->NewPackage(sHead,PKG_CONTROL));
				LOGWARN("发送控制命令:%s",sHead.data());
			}
		}
	}
}


//////////////////////////////////////////////////////////////////////////
// 描    述:  TCP转发服务通道侦听线程
// 作    者:  邵凯田
// 创建时间:  2018-10-12 19:05
// 参数说明:  
// 返 回 值:  
//////////////////////////////////////////////////////////////////////////
void* CSession::ThreadTcpSvrListen(void *lp)
{
	CSession *pThis = (CSession*)lp;
	SSocket *pSock = NULL;
	SString sHead;
	pThis->BeginThread();
	while(pThis->IsQuit() == false)
	{
		if(pSock == NULL)
			pSock = new SSocket();
		if(!pThis->m_ListenSock.Accept(*pSock))
			break;
		stuTcpSvrParam *p = new stuTcpSvrParam();
		pThis->m_TcpSvrParams.append(p);
		p->pThis = pThis;
		p->pSessionSock = pSock;
		p->session_id.sprintf("%ld",(long)p->pSessionSock);
		pSock = NULL;
		sHead.sprintf("act=connect;session_id=%ld;",(long)p->pSessionSock);
		pThis->m_pSession->AddToSend(pThis->m_pSession->NewPackage(sHead,PKG_CONTROL));
		LOGWARN("发送控制命令:%s",sHead.data());
		stuSTcpPackage *pRecvPackage = NULL;
		for(int i=0;i<500;i++)
		{
			SApi::UsSleep(10000);
			pRecvPackage = pThis->m_pSession->SearchRecvPackage("connect");
			if(pRecvPackage != NULL)
				break;
		}
		if(pRecvPackage == NULL || SString::GetAttributeValueI(pRecvPackage->m_sHead,"result") != 1)
		{
			LOGWARN("无法连接到指定的桥接目标!通道创建失败!");
			continue;
		}
		
		SKT_CREATE_THREAD(ThreadTcpSvrSession,p);
		SApi::UsSleep(100000);
	}
	pThis->EndThread();
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  TCP转发服务通道收发线程
// 作    者:  邵凯田
// 创建时间:  2018-10-12 19:05
// 参数说明:  
// 返 回 值:  
//////////////////////////////////////////////////////////////////////////
void* CSession::ThreadTcpSvrSession(void *lp)
{
	stuTcpSvrParam *p = (stuTcpSvrParam*)lp;
	CSession *pThis = p->pThis;
	SSocket *pSock = p->pSessionSock;
	pSock->SetTimeout(10,5000);
	int ret;
	BYTE buf[1600];
	SString sHead;
	pThis->BeginThread();
	while(pThis->IsQuit() == false)
	{
		//转发服务
		ret = pSock->CheckForRecv();
		if(ret == 0)
			continue;
		if(ret < 0)
		{
			//外网侧通信中断，需要转发通信中断给内网侧
			sHead.sprintf("act=pipbroken;session_id=%ld;",(long)pSock);
			pThis->m_pSession->AddToSend(pThis->m_pSession->NewPackage(sHead,PKG_CONTROL));
			LOGWARN("发送控制命令:%s",sHead.data());
		}
		ret = pSock->ReceiveOnce(buf,1600);
		if(ret <= 0)
		{
			//外网侧通信中断，需要转发通信中断给内网侧
			sHead = "act=pipbroken;";
			sHead.sprintf("act=pipbroken;session_id=%ld;",(long)pSock);
			pThis->m_pSession->AddToSend(pThis->m_pSession->NewPackage(sHead,PKG_CONTROL));
			LOGWARN("发送控制命令:%s",sHead.data());
			break;
		}
		//将收到的内容发出去
		sHead.sprintf("session_id=%s;",p->session_id.data());
		pThis->m_pSession->AddToSend(pThis->m_pSession->NewPackage(sHead,PKG_DATA,buf,ret));
	}
	pThis->EndThread();
	pThis->m_TcpSvrParams.remove(p);
	return NULL;
}

CBridgeSvr::CBridgeSvr()
{
	SetStartWord(0xE35B);
}

CBridgeSvr::~CBridgeSvr()
{

}

////////////////////////////////////////////////////////////////////////
// 描    述:  登录回调虚函数
// 作    者:  邵凯田
// 创建时间:  2018-10-12 16:37
// 参数说明:  @ip登录客户端的IP
//            @port登录客户端的端口
//            @sLoginHead登录字符串
// 返 回 值:  true表示允许登录,false表示拒绝登录
//////////////////////////////////////////////////////////////////////////
bool CBridgeSvr::OnLogin(SString ip,int port,SString &sLoginHead)
{
	return true;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  新会话回调虚函数
// 作    者:  邵凯田
// 创建时间:  2018-10-12 16:37
// 参数说明:  @pSession为新创建的会话实例
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CBridgeSvr::OnNewSession(STcpServerSession *pSession)
{
	CSession *p = new CSession(pSession);
	p->Start();
	pSession->SetUserPtr(p);
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  删除会话的回调虚函数
// 作    者:  邵凯田
// 创建时间:  2018-10-12 16:37
// 参数说明:  @pSession为即将将删除的会话实例
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CBridgeSvr::OnDeleteSession(STcpServerSession *pSession)
{
	CSession *p = (CSession*)pSession->GetUserPtr();
	if(p != NULL)
	{
		p->StopAndWait(10);
		pSession->SetUserPtr(NULL);
		delete p;
	}
}

////////////////////////////////////////////////////////////////////////
// 描    述:  会话线程接收到报文后的回调虚函数，派生类通过此函数处理即时消息
// 作    者:  邵凯田
// 创建时间:  2018-10-12 16:37
// 参数说明:  @pSession为会话实例指针，可以通过指针向对端发送数据
//            @pPackage为刚接收到的数据包
// 返 回 值:  true表示已经处理完毕，此报文可以释放了，false表示未处理，此报文应放入接收队列
//////////////////////////////////////////////////////////////////////////
bool CBridgeSvr::OnRecvFrame(STcpServerSession *pSession,stuSTcpPackage *pPackage)
{
	((CSession*)pSession->GetUserPtr())->ProcessClientPkg(pPackage);

	if(pPackage->m_wFrameType == PKG_CONTROL && SString::GetAttributeValue(pPackage->m_sHead,"act") == "connect")
		return false;
	return true;
}

