/**
 *
 * 文 件 名 : BridgeSvrApp.cpp
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

#include "BridgeSvrApp.h"

CBridgeSvrApp::CBridgeSvrApp()
{

}

CBridgeSvrApp::~CBridgeSvrApp()
{

}

bool CBridgeSvrApp::Start()
{
	int tcp_port = 0;
	if(this->IsParam("port"))
		tcp_port = this->GetParamI("port");
	if(tcp_port == 0)
		tcp_port = 6001;
	LOGDEBUG("桥接端口为:%d, 如果需要更改端口，启动时加参数: -port=端口号",tcp_port);
	m_Server.Start(tcp_port);
	return true;
}

bool CBridgeSvrApp::Stop()
{
	m_Server.Stop();
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
bool CBridgeSvrApp::ProcessTxtCmd(SString &sCmd,SString &sResult)
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
bool CBridgeSvrApp::ProcessAgentMsg(WORD wMsgType,stuSpUnitAgentMsgHead *pMsgHead,SString &sHeadStr,BYTE* pBuffer,int iLength)
{
	return false;
}