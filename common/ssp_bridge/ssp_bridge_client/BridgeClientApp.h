/**
 *
 * 文 件 名 : BridgeClientApp.h
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


#ifndef __BRIDGE_CLIENT_APPLICATION_H__
#define __BRIDGE_CLIENT_APPLICATION_H__

#include "SApplication.h"
#include "BridgeClient.h"

class CBridgeClientApp : public SApplication 
{
public:
	CBridgeClientApp();
	virtual ~CBridgeClientApp();


	////////////////////////////////////////////////////////////////////////
	// 描    述:  启动服务，开启所有应用的服务，该函数必须由派生类实现，且启动服务完成后必须返回
	// 作    者:  邵凯田
	// 创建时间:  2018-10-12 21:44
	// 参数说明:  void
	// 返 回 值:  true/false
	//////////////////////////////////////////////////////////////////////////
	virtual bool Start();

	////////////////////////////////////////////////////////////////////////
	// 描    述:  停止服务，关闭所有应用的服务
	// 作    者:  邵凯田
	// 创建时间:  2018-10-12 21:44
	// 参数说明:  void
	// 返 回 值:  true/false
	//////////////////////////////////////////////////////////////////////////
	virtual bool Stop();

	void ProcCmd();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  处理文本命令
	// 作    者:  邵凯田
	// 创建时间:  2018-10-12 21:44
	// 参数说明:  @sCmd为命令内容，空字符串表示取表命令列表
	//         :  @sResult为返回结果，纯文本
	// 返 回 值:  true表示成功，false表示失败
	//////////////////////////////////////////////////////////////////////////
	virtual bool ProcessTxtCmd(SString &sCmd,SString &sResult);

	////////////////////////////////////////////////////////////////////////
	// 描    述:  代理消息处理接口，由派生类实现，处理函数必须尽量短小，快速返回
	// 作    者:  邵凯田
	// 创建时间:  2018-10-12 21:44
	// 参数说明:  @wMsgType表示消息类型
	//         :  @pMsgHead为消息头
	//         :  @sHeadStr消息头字符串
	//         :  @pBuffer为消息携带的二进制数据内容，NULL表示无二进制数据
	//         :  @iLength为二进制数据长度
	// 返 回 值:  true表示处理成功，false表示处理失败或未处理
	//////////////////////////////////////////////////////////////////////////
	bool ProcessAgentMsg(WORD wMsgType,stuSpUnitAgentMsgHead *pMsgHead,SString &sHeadStr,BYTE* pBuffer=NULL,int iLength=0);


	//CBridgeClient m_Client;
	SPtrList<CBridgeClient> m_Clients;
};


#endif//__BRIDGE_CLIENT_APPLICATION_H__
