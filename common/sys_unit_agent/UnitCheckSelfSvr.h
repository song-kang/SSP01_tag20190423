/**
 *
 * 文 件 名 : UnitCheckSelfSvr.h
 * 创建日期 : 2015-7-21 13:25
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : 单元自检服务
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2015-7-21	邵凯田　创建文件
 *
 **/

#ifndef __SP_UNIT_CHECKSELF_SVR_H__
#define __SP_UNIT_CHECKSELF_SVR_H__

#include "sp_unit_checkself.h"
#include "SService.h"
#include "SHash.h"

typedef void (*ChkSelfAnalogChg)(void *cbParam,stuUnitCheckSelfAnalog *pAnalog);
typedef void (*ChkSelfStateChg)(void *cbParam,stuUnitCheckSelfState *pState);


class CUnitCheckSelfSvr : public SService
{
public:
	CUnitCheckSelfSvr();
	virtual ~CUnitCheckSelfSvr();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  启动单元自检服务
	// 作    者:  邵凯田
	// 创建时间:  2015-7-21 13:33
	// 参数说明:  void
	// 返 回 值:  true/false
	//////////////////////////////////////////////////////////////////////////
	virtual bool Start();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  设置单元配置指针
	// 作    者:  邵凯田
	// 创建时间:  2015-7-21 13:32
	// 参数说明:  
	// 返 回 值:  
	//////////////////////////////////////////////////////////////////////////
	void SetUnitConfig(CUnitConfig *p){m_pUnitConfig = p;};

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  模拟量值改变的突发处理
	// 作    者:  邵凯田
	// 创建时间:  2015-7-21 13:40
	// 参数说明:  @pAnalog
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void AnalogChg(stuUnitCheckSelfAnalog *pAnalog);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  状态量值突发改变的处理
	// 作    者:  邵凯田
	// 创建时间:  2015-7-21 13:40
	// 参数说明:  @pState
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void StateChk(stuUnitCheckSelfState *pState);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  搜索模拟量
	// 作    者:  邵凯田
	// 创建时间:  2015-7-21 13:40
	// 参数说明:  @list为引用返回的列表
	//         :  @iFromId为起始ID，-1表示不限制起始ID
	//         :  @iToId为结束ID，-1表示不限制结束ID
	// 返 回 值:  搜索到的模拟量数据,<0表示失败
	//////////////////////////////////////////////////////////////////////////
	int SearchAnalog(SPtrList<stuUnitCheckSelfAnalog> &list,int iFromId=-1,int iToId=-1);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  搜索状态量
	// 作    者:  邵凯田
	// 创建时间:  2015-7-21 13:40
	// 参数说明:  @list为引用返回的列表
	//         :  @iFromId为起始ID，-1表示不限制起始ID
	//         :  @iToId为结束ID，-1表示不限制结束ID
	// 返 回 值:  搜索到的状态量数据,<0表示失败
	//////////////////////////////////////////////////////////////////////////
	int SearchState(SPtrList<stuUnitCheckSelfState> &list,int iFromId=-1,int iToId=-1);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  设置突发单元自检模拟量、状态量回调函数指针和参数
	// 作    者:  邵凯田
	// 创建时间:  2015-7-21 16:12
	// 参数说明:  
	// 返 回 值:  
	//////////////////////////////////////////////////////////////////////////
	void SetChkSeflCallbackFun(ChkSelfAnalogChg pFunAna,ChkSelfStateChg pFunSta,void* cb)
	{
		m_pFunAnalogChg = pFunAna;
		m_pFunStateChg = pFunSta;
		m_pFunCbParam = cb;
	}

private:
	//////////////////////////////////////////////////////////////////////////
	// 描    述:  自检主线程
	// 作    者:  邵凯田
	// 创建时间:  2015-7-21 13:51
	// 参数说明:  @lp为this
	// 返 回 值:  NULL
	//////////////////////////////////////////////////////////////////////////
	static void* ThreadCheckSelf(void *lp);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  开机自检，单元管理程序运行时一次性检测项目
	// 作    者:  邵凯田
	// 创建时间:  2015-7-21 13:53
	// 参数说明:  void
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void StartupCheckSelf();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  实时自检，即单元管理程序运行后定时执行的自检
	// 作    者:  邵凯田
	// 创建时间:  2015-7-21 13:54
	// 参数说明:  void
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void RealtimeCheckSelf();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  信息点散列值计算函数
	// 作    者:  邵凯田
	// 创建时间:  2015-7-21 14:44
	// 参数说明:  
	// 返 回 值:  
	//////////////////////////////////////////////////////////////////////////
	static int HashKey_Analog(void *cbParam,void *pItem);
	static int HashKey_State(void *cbParam,void *pItem);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  添加新的信息点到列表和散列
	// 作    者:  邵凯田
	// 创建时间:  2015-7-21 14:44
	// 参数说明:  @p
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void NewAnalog(stuUnitCheckSelfAnalog *p);
	void NewState(stuUnitCheckSelfState *p);

	CUnitConfig *m_pUnitConfig;					//单元配置指针
	SPtrList<stuUnitCheckSelfAnalog> m_Analogs;	//单元自检模拟量列表
	SPtrList<stuUnitCheckSelfState> m_States;	//单元自检状态量列表
	SIntHash<stuUnitCheckSelfAnalog,HashKey_Analog> m_HashAnalog;
	SIntHash<stuUnitCheckSelfState,HashKey_State> m_HashState;
	ChkSelfAnalogChg m_pFunAnalogChg;
	ChkSelfStateChg m_pFunStateChg;
	void* m_pFunCbParam;
	
};

#endif//__SP_UNIT_CHECKSELF_SVR_H__
