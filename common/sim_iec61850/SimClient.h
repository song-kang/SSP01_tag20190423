/**
 *
 * 文 件 名 : SimClient.h
 * 创建日期 : 2018-8-22 9:58
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : 仿真客户端程序
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2018-8-22	邵凯田　创建文件
 *
 **/

#ifndef __SIM_CLIENT_H__
#define __SIM_CLIENT_H__

#include "SimComm.h"
#include "STcpClientBase.h"
#include "ConfigFileMgr.h"

class view_home;
class CSimClient : public STcpClientBase
{
public:
	CSimClient();
	virtual ~CSimClient();

	////////////////////////////////////////////////////////////////////////
	// 描    述:  以服务形式启动客户端，不能与Connect函数同时使用
	// 作    者:  邵凯田
	// 创建时间:  2018-8-22 9:58
	// 参数说明:  @ip表示服务端IP地址，@port表示端口
	// 返 回 值:  true表示服务启动成功,false表示服务启动失败
	//////////////////////////////////////////////////////////////////////////
	virtual bool Start(SString module_name,SString module_desc,SString ip,int port);

	////////////////////////////////////////////////////////////////////////
	// 描    述:  停止服务
	// 作    者:  邵凯田
	// 创建时间:  2018-8-22 9:58
	// 参数说明:  void
	// 返 回 值:  true表示服务停止成功,false表示服务停止失败
	//////////////////////////////////////////////////////////////////////////
	virtual bool Stop();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  登录上服务端后的回调
	// 作    者:  邵凯田
	// 创建时间:  2018-8-22 9:58
	// 参数说明:  void
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	virtual void OnLogin();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  断开服务端后的回调
	// 作    者:  邵凯田
	// 创建时间:  2018-8-22 9:58
	// 参数说明:  void
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	virtual void OnClose();

	////////////////////////////////////////////////////////////////////////
	// 描    述:  会话线程接收到报文后的回调虚函数，派生类通过此函数处理即时消息
	// 作    者:  邵凯田
	// 创建时间:  2018-8-22 9:58
	// 参数说明:  @pSession为会话实例指针，可以通过指针向对端发送数据
	//            @pPackage为刚接收到的数据包
	// 返 回 值:  true表示已经处理完毕，此报文可以释放了，false表示未处理，此报文应放入接收队列
	//////////////////////////////////////////////////////////////////////////
	virtual bool OnRecvFrame(stuSTcpPackage *pPackage);

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
	virtual int RunStep(SString func,SString args);

	void setOptObj(view_home* pViewHome,ConfigFileMgr* pConfileMgr);
private:
	//服务端回复注册结果命令(PKG_REGISTER_RES=2)，携带参数为：cluster_id=集群编号;sim_code=仿真模块唯一代号;result=1/0; (1表示成功，0表示失败)
	bool ProcessSimPackageType2(SString& strHead);
	//服务端发送请求能力描述信息(PKG_ABILITY_REQ=3)
	bool ProcessSimPackageType3(SString& strHead);
	//服务端发送服务开启请求(PKG_START_REQ=5)
	bool ProcessSimPackageType5(SString& strHead);
	//服务端发送服务停止请求(PKG_STOP_REQ=7)
	bool ProcessSimPackageType7(SString& strHead);
	//服务端发送作业执行请求(PKG_JOB_REQ=9)
	bool ProcessSimPackageType9(SString& strHead);
	bool sendRegisterInfo();
private:
	ConfigFileMgr* m_pConfigFileMgr;
	view_home* m_pViewHome;
	SString m_strModuleName;
};

#endif//__SIM_CLIENT_H__
