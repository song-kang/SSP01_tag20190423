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

#include "BridgeClient.h"

CBridgeClient::CBridgeClient()
{
	SetStartWord(0xE35B);
	m_TcpClientParams.setAutoDelete(true);
	m_TcpClientParams.setShared(true);
}

CBridgeClient::~CBridgeClient()
{

}

////////////////////////////////////////////////////////////////////////
// 描    述:  以服务形式启动客户端，不能与Connect函数同时使用
// 作    者:  邵凯田
// 创建时间:  2018-10-12 22:05
// 参数说明:  @ip表示服务端IP地址，@port表示端口
// 返 回 值:  true表示服务启动成功,false表示服务启动失败
//////////////////////////////////////////////////////////////////////////
bool CBridgeClient::Start(SString ip,int port,int outside_port,SString inside_ip,int inside_port)
{
// 	int brg_port = GetInput("请输入您想要桥接的TCP端口号","2000").toInt();
// 	SString inner_ip = GetInput("请输入您想要桥接到的内网IP","127.0.0.1");
// 	int inner_port = GetInput("请输入您想要桥接的内网TCP端口号",SString::toString(brg_port)).toInt();
	m_sBrgIp = ip;
	m_iBrgPort = outside_port;
	m_sInnerIp = inside_ip;
	m_iInnerPort = inside_port;

	if(!STcpClientBase::Start(ip,port))
		return false;

	return true;
}

////////////////////////////////////////////////////////////////////////
// 描    述:  停止服务
// 作    者:  邵凯田
// 创建时间:  2018-10-12 22:05
// 参数说明:  void
// 返 回 值:  true表示服务停止成功,false表示服务停止失败
//////////////////////////////////////////////////////////////////////////

bool CBridgeClient::Stop()
{
	if(!STcpClientBase::Stop())
		return false;
	return true;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  登录上服务端后的回调
// 作    者:  邵凯田
// 创建时间:  2018-10-12 22:05
// 参数说明:  void
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CBridgeClient::OnLogin()
{
	stuSTcpPackage *pRecvPackage = NULL;
	SString sHead;
	sHead.sprintf("act=create_tcpsvr;port=%d;",m_iBrgPort);
	if(!SendAndRecv(pRecvPackage,sHead,PKG_CONTROL,NULL,0,3,PKG_CONTROL) || pRecvPackage == NULL || pRecvPackage->m_wFrameType != PKG_CONTROL)
	{
		LOGERROR("创建TCP桥接通道失败!");
		return ;
	}
	sHead = pRecvPackage->m_sHead;
	if(SString::GetAttributeValue(sHead,"act") != "create_tcpsvr" || SString::GetAttributeValue(sHead,"result") != "1")
	{
		LOGERROR("桥接服务创建失败!桥接通道为: %s:%d --> %s:%d",m_sBrgIp.data(),m_iBrgPort,m_sInnerIp.data(),m_iInnerPort);
		return ;
	}
	LOGDEBUG("桥接服务创建成功!桥接通道为: %s:%d --> %s:%d",m_sBrgIp.data(),m_iBrgPort,m_sInnerIp.data(),m_iInnerPort);

}

////////////////////////////////////////////////////////////////////////
// 描    述:  会话线程接收到报文后的回调虚函数，派生类通过此函数处理即时消息
// 作    者:  邵凯田
// 创建时间:  2018-10-12 22:05
// 参数说明:  @pSession为会话实例指针，可以通过指针向对端发送数据
//            @pPackage为刚接收到的数据包
// 返 回 值:  true表示已经处理完毕，此报文可以释放了，false表示未处理，此报文应放入接收队列
//////////////////////////////////////////////////////////////////////////
bool CBridgeClient::OnRecvFrame(stuSTcpPackage *pPackage)
{
	if(pPackage->m_wFrameType == PKG_CONTROL)
	{
		LOGDEBUG("收到控制命令:%s",pPackage->m_sHead.data());
		SString act = SString::GetAttributeValue(pPackage->m_sHead,"act");
		if(act == "connect")
		{
			SString sHead;
			SSocket *pSock = new SSocket();
			pSock->CreateTcp();
			bool bConn = pSock->Connect(m_sInnerIp,m_iInnerPort);
			SString session_id = SString::GetAttributeValue(pPackage->m_sHead,"session_id");
			sHead.sprintf("act=connect;session_id=%s;result=%d;",session_id.data(),bConn?1:0);
			AddToSend(NewPackage(sHead,PKG_CONTROL));
			LOGWARN("连接桥接目标(%s:%d)%s!发送控制命令:%s",m_sInnerIp.data(),m_iInnerPort,bConn?"成功":"失败",sHead.data());
			if(!bConn)
			{
				delete pSock;
				return true;
			}
			stuTcpClientParam *p = new stuTcpClientParam();
			p->pSock = pSock;
			p->pThis = this;;
			p->session_id = session_id;
			SKT_CREATE_THREAD(ThreadTcpSvrInnerClient,p);
			m_TcpClientParams.append(p);
		}
		else if(act == "pipbroken")
		{
			SString session_id = SString::GetAttributeValue(pPackage->m_sHead,"session_id");
			m_TcpClientParams.lock();
			unsigned long pos;
			stuTcpClientParam *p = m_TcpClientParams.FetchFirst(pos);
			while(p)
			{
				if(p->session_id == session_id)
					break;
				p = m_TcpClientParams.FetchNext(pos);
			}
			m_TcpClientParams.unlock();
			if(p == NULL)
				return true;
			p->pSock->Close();
		}
	}
	else if(pPackage->m_wFrameType == PKG_DATA && pPackage->m_iAsduLen > 0)
	{
		//LOGDEBUG("收到数据包(len=%d):%s",pPackage->m_iAsduLen,SString::HexToStr(pPackage->m_pAsduBuffer,pPackage->m_iAsduLen>500?500:pPackage->m_iAsduLen).data());
		unsigned long pos;
		SString sHead;
		SString session_id = SString::GetAttributeValue(pPackage->m_sHead,"session_id");
		m_TcpClientParams.lock();
		stuTcpClientParam *p = m_TcpClientParams.FetchFirst(pos);
		while(p)
		{
			if(p->session_id == session_id)
				break;
			p = m_TcpClientParams.FetchNext(pos);
		}
		m_TcpClientParams.unlock();
		if(p == NULL)
			return true;
		int ret = p->pSock->Send(pPackage->m_pAsduBuffer,pPackage->m_iAsduLen);
		if(ret != pPackage->m_iAsduLen)
		{
			sHead.sprintf("act=pipbroken;session_id=%s;",p->session_id.data());
			LOGWARN("发送控制命令:%s",sHead.data());
			AddToSend(NewPackage(sHead,PKG_CONTROL));
		}
		p->in_bytes += pPackage->m_iAsduLen;
	}
	return true;
}

SString CBridgeClient::GetInput(SString tip,SString def)
{
	char buf[1025];
	if(tip.length() > 0)
	{
		printf(tip.data());
		if(def.length() > 0)
			printf("(%s)",def.data());
		printf(":");
	}
	memset(buf,0,sizeof(buf));
	gets(buf);
	SString str = buf;
	if(str.length() == 0)
		return def;
	return str;
}

void* CBridgeClient::ThreadTcpSvrInnerClient(void *lp)
{
	stuTcpClientParam *p = (stuTcpClientParam*)lp;
	CBridgeClient *pThis = p->pThis;
	SSocket *pSock = p->pSock;
	int ret;
	SString sHead;
	BYTE buf[1600];
	pSock->SetTimeout(10,5000);
	pThis->BeginThread();
	while(pThis->IsQuit() == false)
	{
		ret = pSock->CheckForRecv();
		if(ret == 0)
			continue;
		if(ret < 0)
		{
			sHead.sprintf("act=pipbroken;session_id=%s;",p->session_id.data());
			pThis->AddToSend(pThis->NewPackage(sHead,PKG_CONTROL));
			LOGWARN("发送控制命令:%s",sHead.data());
			break;
		}
		ret = pSock->ReceiveOnce(buf,1600);
		if(ret <= 0)
		{
			sHead.sprintf("act=pipbroken;session_id=%s;",p->session_id.data());
			pThis->AddToSend(pThis->NewPackage(sHead,PKG_CONTROL));
			LOGWARN("发送控制命令:%s",sHead.data());
			break;
		}
		sHead.sprintf("session_id=%s;",p->session_id.data());
		p->out_bytes += ret;
		pThis->AddToSend(pThis->NewPackage(sHead,PKG_DATA,buf,ret));
	}
	pThis->EndThread();
	pThis->m_TcpClientParams.remove(p);
	return NULL;
}