/**
 *
 * 文 件 名 : sim_manager.h
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

#ifndef __SIM_MANAGER_H__
#define __SIM_MANAGER_H__
#include "MUSimulator.h"
#include "GooseSimulator.h"
#include "sim_config.h"
#include "SimMmsServer.h"
#include "sp_virtual_terminal.h"
class CSimManager
{
public:
	CSimManager();
	~CSimManager();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  启动管理器
	// 作    者:  邵凯田
	// 创建时间:  2016-1-5 9:50
	// 参数说明:  void
	// 返 回 值:  true/false
	//////////////////////////////////////////////////////////////////////////
	bool Start();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  停止管理器
	// 作    者:  邵凯田
	// 创建时间:  2016-1-5 9:51
	// 参数说明:  void
	// 返 回 值:  true/false
	//////////////////////////////////////////////////////////////////////////
	bool Stop();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  触发指定的GOOSE状态变位
	// 作    者:  邵凯田
	// 创建时间:  2017-4-5 18:17
	// 参数说明:  @appid
	//         :  @vals
	// 返 回 值:  true/false
	//////////////////////////////////////////////////////////////////////////
	bool DoGooseChg(int appid,SString vals);

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
	bool DoSvChg(int appid,int ch_idx,float rms,int q);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  修改指定的叶子节点值内容
	// 作    者:  邵凯田
	// 创建时间:  2017-4-5 18:21
	// 参数说明:  @mms_path
	//         :  @vt
	//         :  @val
	// 返 回 值:  true/false
	//////////////////////////////////////////////////////////////////////////
	bool DoSetLeaf(SString ied_ld,SString leaf_path,int vt,SString val);

	CMUSimManager m_MuSimMgr;//MU模拟管理器
	CGooseSimManager m_GooseSimMGr;//GOOSE模拟管理器
	CSimConfig *m_pSimConfig;
	CSimMmsServer m_MmsServer;
};


#endif//__SIM_MANAGER_H__