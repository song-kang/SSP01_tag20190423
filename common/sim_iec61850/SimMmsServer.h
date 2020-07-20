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
#ifndef __SIM_MMS_SERVER_H__
#define __SIM_MMS_SERVER_H__

#include "sim_config.h"
#include "SMmsServer.h"
#include "ssp_gui.h"
class CSimMmsServer : public SMmsServer
{
public:
	CSimMmsServer();
	virtual ~CSimMmsServer();

	////////////////////////////////////////////////////////////////////////
	// 描    述:  启动服务,虚函数，派生函数中必须先调用此函数
	// 作    者:  邵凯田
	// 创建时间:  2016-9-2 14:27
	// 参数说明:  void
	// 返 回 值:  true表示服务启动成功,false表示服务启动失败
	//////////////////////////////////////////////////////////////////////////
	virtual bool Start();


	//////////////////////////////////////////////////////////////////////////
	// 描    述:  进度文本虚函数，加载服务时使用
	// 作    者:  邵凯田
	// 创建时间:  2017-4-13 15:10
	// 参数说明:  @sTipText
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	virtual void OnProgressText(SString sTipText)
	{
		SSP_GUI->SetStatusTip(sTipText);
		SQt::ProcessUiEvents();
	};

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  映射叶子节点用户
	// 作    者:  邵凯田
	// 创建时间:  2016-9-2 14:27
	// 参数说明:  @sIedLdName表示IED+LD名称
	//         :  @sLeafPath表示叶子节点路径
	//         :  @ppLeafMap表示叶子节点的用户映射指针的地址，映射指针为空，用户层决定是否需要映射，需要时创建stuLeafMap对象并填写指针，MMS服务内部不负责释放
	// 返 回 值:  true表示成功映射，false表示无法映射
	//////////////////////////////////////////////////////////////////////////
	virtual bool OnMapLeafToUser(char* sIedLdName, char* sLeafPath, stuLeafMap **ppLeafMap);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  添加访问点IP地址
	// 作    者:  邵凯田
	// 创建时间:  2016-11-12 14:42
	// 参数说明:  void
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void AddApIpAddr();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  删除访问点IP
	// 作    者:  邵凯田
	// 创建时间:  2016-11-12 14:42
	// 参数说明:  void
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void RemoveApIpAddr();

	SPtrList<stuLeafMap> m_StateLeafMap;//状态量叶子映射
	SPtrList<stuLeafMap> m_AnalogLeafMap;//模拟量叶子映射

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  模拟线程，定时进行信号或测量值的变化
	// 作    者:  邵凯田
	// 创建时间:  2016-9-2 14:39
	// 参数说明:  @lp
	// 返 回 值:  NULL
	//////////////////////////////////////////////////////////////////////////
	static void* ThreadSim(void *lp);
	CSimConfig *m_pSimConfig;
};
#endif
