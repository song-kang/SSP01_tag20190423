/**
 *
 * 文 件 名 : GuiApplication.h
 * 创建日期 : 2015-7-30 15:51
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : 图形界面应用程序类
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2015-7-30	邵凯田　创建文件
 *
 **/

#ifndef __TEST_GUI_APPLICATION_H__
#define __TEST_GUI_APPLICATION_H__

#include <QtGui/QApplication>
#include <QtGui/QMainWindow>
#include "SApplication.h"
#include "GuiFrame.h"
#include "ssp_gui.h"
#include "ssp_database.h"
#include "ssp_funpoint.h"
#include "ssp_datawindow.h"
#include "sp_svglib_config.h"

class CGuiApplication : public SApplication
{
public:
	CGuiApplication(void);
	virtual ~CGuiApplication(void);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  取APP应用实例指针
	// 作    者:  邵凯田
	// 创建时间:  2015-7-30 15:59
	// 参数说明:  void
	// 返 回 值:  CHmiApplication*
	//////////////////////////////////////////////////////////////////////////
	static CGuiApplication* GetApp();
	static QApplication* GetQApp();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  设备QT应用类指针
	// 作    者:  邵凯田
	// 创建时间:  2015-7-30 15:59
	// 参数说明:  @pApp
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void SetQApplication(QApplication *pApp);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  启动服务，开启所有应用的服务，该函数必须由派生类实现，且启动服务完成后必须返回
	// 作    者:  邵凯田
	// 创建时间:  2015-7-30 15:59
	// 参数说明:  void
	// 返 回 值:  true/false
	//////////////////////////////////////////////////////////////////////////
	virtual bool Start();

	////////////////////////////////////////////////////////////////////////
	// 描    述:  停止服务，关闭所有应用的服务
	// 作    者:  邵凯田
	// 创建时间:  2015-7-30 15:59
	// 参数说明:  void
	// 返 回 值:  true/false
	//////////////////////////////////////////////////////////////////////////
	virtual bool Stop();

	////////////////////////////////////////////////////////////////////////
	// 描    述:  取当前模块的运行状态信息，对应接入模块应列出所有接入设备的状态明细
	// 作    者:  邵凯田
	// 创建时间:  2015-7-30 15:59
	// 参数说明:  void
	// 返 回 值:  SString
	//////////////////////////////////////////////////////////////////////////
	virtual SString GetNowModInfo();

	bool CSpAgentClient::ProcessAgentMsg(WORD,stuSpUnitAgentMsgHead *,SString &,BYTE *,int){return false;};

protected:
	QApplication *m_pQApp;//QT应用指针
	CGuiFrame *m_pMainWnd;//主窗口

	CSsp_Database *m_pDatabase;			//数据库配置文件
	CSsp_DatawindowMgr *m_pDatawindow;	//数据窗口配置文件
	CSsp_FunPointConfig *m_pFunpoint;	//功能点配置文件
	CSp_SvgLibConfig	*m_pSvgLibCfg;	//SVG图元库
};

#endif//__TEST_GUI_APPLICATION_H__
