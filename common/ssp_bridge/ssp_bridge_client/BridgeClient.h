/**
 *
 * 文 件 名 : BridgeSvr.h
 * 创建日期 : 2018-10-12 16:37
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : 桥接服务程序
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2018-10-12	邵凯田　创建文件
 *
 **/

#ifndef __SSP_BRIDGE_SERVER_H__
#define __SSP_BRIDGE_SERVER_H__

#include "SService.h"
#include "SApi.h"
#include "SSocket.h"
#include "SString.h"
#include "STcpClientBase.h"
#include "../public/bridge_inc.h"

class CBridgeClient : public STcpClientBase
{
public:
	struct stuTcpClientParam
	{
		stuTcpClientParam()
		{
			pSock = NULL;
			in_bytes = out_bytes = 0;
		}
		~stuTcpClientParam()
		{
			if(pSock != NULL)
				delete pSock;
		}
		CBridgeClient *pThis;
		SSocket *pSock;
		SString session_id;
		unsigned int in_bytes,out_bytes;
	};
	CBridgeClient();
	virtual ~CBridgeClient();

	////////////////////////////////////////////////////////////////////////
	// 描    述:  以服务形式启动客户端，不能与Connect函数同时使用
	// 作    者:  邵凯田
	// 创建时间:  2018-10-12 22:05
	// 参数说明:  @ip表示服务端IP地址，@port表示端口
	// 返 回 值:  true表示服务启动成功,false表示服务启动失败
	//////////////////////////////////////////////////////////////////////////
	virtual bool Start(SString ip,int port,int outside_port,SString inside_ip,int inside_port);

	////////////////////////////////////////////////////////////////////////
	// 描    述:  停止服务
	// 作    者:  邵凯田
	// 创建时间:  2018-10-12 22:05
	// 参数说明:  void
	// 返 回 值:  true表示服务停止成功,false表示服务停止失败
	//////////////////////////////////////////////////////////////////////////
	virtual bool Stop();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  登录上服务端后的回调
	// 作    者:  邵凯田
	// 创建时间:  2018-10-12 22:05
	// 参数说明:  void
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	virtual void OnLogin();

	////////////////////////////////////////////////////////////////////////
	// 描    述:  会话线程接收到报文后的回调虚函数，派生类通过此函数处理即时消息
	// 作    者:  邵凯田
	// 创建时间:  2018-10-12 22:05
	// 参数说明:  @pSession为会话实例指针，可以通过指针向对端发送数据
	//            @pPackage为刚接收到的数据包
	// 返 回 值:  true表示已经处理完毕，此报文可以释放了，false表示未处理，此报文应放入接收队列
	//////////////////////////////////////////////////////////////////////////
	virtual bool OnRecvFrame(stuSTcpPackage *pPackage);

	static SString GetInput(SString tip="",SString def="");

	static void* ThreadTcpSvrInnerClient(void *lp);

	SString m_sBrgIp;
	int m_iBrgPort;
	SString m_sInnerIp;
	int m_iInnerPort;
	SString m_sName;
	SPtrList<stuTcpClientParam> m_TcpClientParams;
};

#endif//__SSP_BRIDGE_SERVER_H__
