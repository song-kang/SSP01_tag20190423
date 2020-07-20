/**
 *
 * 文 件 名 : BridgeClientApp.cpp
 * 创建日期 : 2018-10-12 21:44
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

#include "BridgeClientApp.h"

CBridgeClientApp::CBridgeClientApp()
{

}

CBridgeClientApp::~CBridgeClientApp()
{

}

void usage()
{
	printf("\n");
	printf("Usage: ssp_bridge_client -conf=XML配置文件名(使用配置生成桥接配置)\n");
	printf("                         -bridge_ip=<桥接服务器的IP>(使用指定配置,下同,不可与conf同时使用)\n");
	printf("                         -bridge_port=<桥接服务器的端口>\n");
	printf("                         -outside_port=<服务器侧的桥接端口>\n");
	printf("                         -inside_ip=<客户端侧的桥接IP>\n");
	printf("                         -inside_port=<客户端侧的桥接端口>\n");
	printf("\n");
	SApi::UsSleep(1000000);
}
bool CBridgeClientApp::Start()
{
	SString sConf = GetParam("conf");
	if(sConf.length() > 0)
	{
		if(!SFile::exists(sConf))
		{
			LOGERROR("conf对应配置文件[%s]不存在!",sConf.data());
			usage();
			return false;
		}
		SXmlConfig xml;
		if(!xml.ReadConfig(sConf))
		{
			LOGERROR("配置文件[%s]格式无效!",sConf.data());
			usage();
			return false;
		}
		SBaseConfig *tcp_bridges = xml.SearchChild("tcp_bridges");
		int i,cnt = tcp_bridges->GetChildCount("bridge");
		if(tcp_bridges == NULL || cnt <= 0)
		{
			LOGERROR("配置文件[%s]缺少有效的配置项:tcp_bridges->bridge!",sConf.data());
			usage();
			return false;
		}
		m_Clients.setAutoDelete(true);
		for(i=0;i<cnt;i++)
		{
			SBaseConfig *bridge = tcp_bridges->GetChildNode(i,"bridge");
			if(bridge == NULL)
				continue;
			SString sBridgeIp = bridge->GetAttribute("bridge_ip");
			int iBridgePort = bridge->GetAttributeI("bridge_port");
			int outside_port = bridge->GetAttributeI("outside_port");
			SString inside_ip = bridge->GetAttribute("inside_ip");
			int inside_port = bridge->GetAttributeI("inside_port");
			SString desc = bridge->GetAttribute("desc");
			CBridgeClient *pClient = new CBridgeClient();
			m_Clients.append(pClient);
			pClient->SetConnectMode(true);
			pClient->m_sName = desc;
			if(!pClient->Start(sBridgeIp,iBridgePort,outside_port,inside_ip,inside_port))
			{
				LOGERROR("无法连接服务器%s:%d!",sBridgeIp.data(),iBridgePort);				
			}
		}
		ProcCmd();
	}
	SString sBridgeIp = this->GetParam("bridge_ip");
	int iBridgePort = this->GetParamI("bridge_port");

	int outside_port = this->GetParamI("outside_port");
	SString inside_ip = this->GetParam("inside_ip");
	int inside_port = this->GetParamI("inside_port");

	if(sBridgeIp.length() == 0)
	{
		LOGERROR("缺少有效的参数:bridge_ip!");
		usage();
		return false;
	}
	if(iBridgePort == 0)
	{
		LOGERROR("缺少有效的参数:bridge_port!");
		usage();
		return false;
	}

	if(outside_port == 0)
	{
		LOGERROR("缺少有效的参数:outside_port!");
		usage();
		return false;
	}
	if(inside_ip.length() == 0)
	{
		LOGERROR("缺少有效的参数:inside_ip!");
		usage();
		return false;
	}
	if(inside_port == 0)
	{
		LOGERROR("缺少有效的参数:inside_port!");
		usage();
		return false;
	}
	CBridgeClient *pClient = new CBridgeClient();
	m_Clients.append(pClient);
	pClient->SetConnectMode(true);
	pClient->m_sName = "";
	if(!pClient->Start(sBridgeIp,iBridgePort,outside_port,inside_ip,inside_port))
	{
		LOGERROR("无法连接服务器%s:%d!",sBridgeIp.data(),iBridgePort);				
	}

// 	m_Client.SetConnectMode(true);
// 	if(!m_Client.Start(sBridgeIp,iBridgePort,outside_port,inside_ip,inside_port))
// 	{
// 		LOGERROR("无法连接服务器%s:%d!",sBridgeIp.data(),iBridgePort);
// 		//return false;
// 	}
	ProcCmd();
	return true;
}

void CBridgeClientApp::ProcCmd()
{
	SString cmd;
	while(1)
	{
		cmd = CBridgeClient::GetInput();
		if(cmd == "")
		{
			printf("list   -查看当前桥接通道\n");
		}
		else if(cmd == "list")
		{
			int i,cnt = m_Clients.count();
			for(i=0;i<cnt;i++)
			{
				CBridgeClient *pClient = m_Clients[i];
				if(pClient == NULL)
					continue;
				printf("桥接通道( %s:%d -> %s:%d  %s) 活动会话数:%d \n",
					pClient->m_sBrgIp.data(),pClient->m_iBrgPort,
					pClient->m_sInnerIp.data(),pClient->m_iInnerPort,
					pClient->m_sName.data(),pClient->m_TcpClientParams.count());

				int j,cnt2 = pClient->m_TcpClientParams.count();
				for(j=0;j<cnt2;j++)
				{
					CBridgeClient::stuTcpClientParam *p = pClient->m_TcpClientParams[j];
					if(p == NULL)
						continue;
					printf("\tidx:%02d session_id:%s in_bytes:%u out_bytes:%u \n",
						j+1,p->session_id.data(),p->in_bytes,p->out_bytes);
				}
			}
		}
	}
}

bool CBridgeClientApp::Stop()
{
	//m_Client.Stop();
	return true;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  处理文本命令
// 作    者:  邵凯田
// 创建时间:  2016-2-13 15:12
// 参数说明:  @sCmd为命令内容，空字符串表示取表命令列表
//         :  @sResult为返回结果，纯文本
// 返 回 值:  true表示成功，false表示失败
//////////////////////////////////////////////////////////////////////////
bool CBridgeClientApp::ProcessTxtCmd(SString &sCmd,SString &sResult)
{
	SString sText;
//	int i;
	if(sCmd == "" || sCmd == " ")
	{
		sResult = "cmd                - help\r\n"
				  "cmd info           - client information\r\n"
				  "cmd units          - unit information\r\n"
				  "cmd procs          - process infomation\r\n";
	}
	if(sCmd == "info")
	{
// 		sResult += SString::toFormat("共%d个数据库客户端已连接!\r\n", m_MdbService.GetSessionCount());
// 		for (i = 0; i < m_MdbService.GetSessionCount(); i++)
// 		{
// 			STcpServerSession *p = m_MdbService.GetSession(i);
// 			if (p == NULL)
// 				continue;
// 			sResult += SString::toFormat("    第%d个连接:%s:%d -> MDB\r\n", i + 1, p->GetSessionSock()->GetPeerIp().data(), p->GetSessionSock()->GetPeerPort());
// 		}
// 		sResult += m_MdbService.GetMdbMgrPtr()->GetMdbMgrInfo();
	}

	return true;
}

////////////////////////////////////////////////////////////////////////
// 描    述:  代理消息处理接口，由派生类实现，处理函数必须尽量短小，快速返回
// 作    者:  邵凯田
// 创建时间:  2016-2-3 15:20
// 参数说明:  @wMsgType表示消息类型
//         :  @pMsgHead为消息头
//         :  @sHeadStr消息头字符串
//         :  @pBuffer为消息携带的二进制数据内容，NULL表示无二进制数据
//         :  @iLength为二进制数据长度
// 返 回 值:  true表示处理成功，false表示处理失败或未处理
//////////////////////////////////////////////////////////////////////////
bool CBridgeClientApp::ProcessAgentMsg(WORD wMsgType,stuSpUnitAgentMsgHead *pMsgHead,SString &sHeadStr,BYTE* pBuffer,int iLength)
{
	return false;
}

