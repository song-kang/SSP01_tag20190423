/**
 *
 * 文 件 名 : viewmoduleinfo.h
 * 创建日期 : 2013-9-13 12:58
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : 执行模块的版本信息
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2013-9-13	邵凯田　创建文件
 *
 **/

#ifndef VIEWMODULEINFO_H
#define VIEWMODULEINFO_H

#include <QWidget>
#include "ui_viewmoduleinfo.h"
#include "ssp_baseview.h"
#include "SApplication.h"
#include "ssp_database.h"

class CViewModuleInfo : public CBaseView
{
	Q_OBJECT

public:
	struct stuRunUnit
	{
		stuRunUnit()
		{
			m_ProcInfos.setAutoDelete(true);
		}
		~stuRunUnit()
		{
			m_ProcInfos.clear();
		}
		stuSpUnitAgentUnitInfo UnitInfo;
		SPtrList<stuSpUnitAgentProcInfo> m_ProcInfos;
	};
	struct stuRealModInfo
	{
		SString unit_name;
		SString mod_name;
		SString desc;
		SString create_time;
		SString version;
		SString crc;
		SString run_state;
	};

	CViewModuleInfo(QWidget *parent = 0);
	virtual ~CViewModuleInfo();

	////////////////////////////////////////////////////////////////////////
	// 描    述:  当窗口准备显示前调用虚函数，重新加载数据
	// 作    者:  邵凯田
	// 创建时间:  2013:9:13 13:03
	// 参数说明:  void
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	virtual void OnPreShow();

	////////////////////////////////////////////////////////////////////////
	// 描    述:  重新加载实时模块信息
	// 作    者:  邵凯田
	// 创建时间:  2013:10:23 10:03
	// 参数说明:  
	// 返 回 值:  
	//////////////////////////////////////////////////////////////////////////
	void ReLoad();

	virtual void OnRefresh(){OnPreShow();};
signals:
	void sigInfoUpdated();
public slots:
	////////////////////////////////////////////////////////////////////////
	// 描    述:  定时触发，用于显示线程查询出来的记录
	// 作    者:  邵凯田
	// 创建时间:  2013:10:23 10:29
	// 参数说明:  
	// 返 回 值:  
	//////////////////////////////////////////////////////////////////////////
	void OnTimerUI();

	void OnQuery();
	void OnSave();
	void OnPrint();

private:
	////////////////////////////////////////////////////////////////////////
	// 描    述:  修改加载线程
	// 作    者:  邵凯田
	// 创建时间:  2013:10:23 10:02
	// 参数说明:  
	// 返 回 值:  
	//////////////////////////////////////////////////////////////////////////
	static void* ThreadLoadInfo(void *lp);

	bool m_bLoading;//是否正在加载
	SPtrList<stuRunUnit> m_RunUnits;//所有的运行单元，以及各运行单元内的应用进程
	Ui::CViewModuleInfo ui;
};

#endif // VIEWMODULEINFO_H
