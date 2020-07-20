/**
 *
 * 文 件 名 : ssp_plugin_mgr.h
 * 创建日期 : 2017-8-4 9:59
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : SSP_GUI插件管理器类
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2017-8-4	邵凯田　创建文件
 *
 **/
#ifndef __SSP_PLUGIN_MGR_H__
#define __SSP_PLUGIN_MGR_H__

#include "ssp_plugin.h"
#include "SString.h"
#include "SList.h"
#include "ssp_baseview.h"

class CSspPlugin
{
public:
	CSspPlugin();
	~CSspPlugin();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  加载插件
	// 作    者:  邵凯田
	// 创建时间:  2017-8-4 10:52
	// 参数说明:  @sPluginFile为插件动态库文件全路径文件名
	// 返 回 值:  true表示加载成功，false表示加载失败
	//////////////////////////////////////////////////////////////////////////
	bool Load(SString sPluginFile);

	SString m_sPluginFile;//插件文件名（全路径）
#ifdef WIN32
	HMODULE m_hInstance;
#else
	void* m_hInstance;
#endif
	pFun_SSP_Init				m_pSSP_Init;
	pFun_SSP_Exit				m_pSSP_Exit;
	pFun_SSP_GetPluginName		m_pSSP_GetPluginName;
	pFun_SSP_GetPluginVer		m_pSSP_GetPluginVer;
	pFun_SSP_FunPointSupported	m_pSSP_FunPointSupported;
	//pFun_SSP_SetGlobalPtr		m_pSSP_SetGlobalPtr;
	pFun_SSP_NewView			m_pSSP_NewView;
	pFun_SSP_GActSupported		m_pSSP_GActSupported;
};

class CSspPluginMgr
{
public:
	CSspPluginMgr();
	~CSspPluginMgr();
	
	//////////////////////////////////////////////////////////////////////////
	// 描    述:  初始化插件管理器，加载所有插件
	// 作    者:  邵凯田
	// 创建时间:  2017-8-4 11:16
	// 参数说明:  void
	// 返 回 值:  true表示初始化成功，false表示失败
	//////////////////////////////////////////////////////////////////////////
	bool Init();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  退出所有插件
	// 作    者:  邵凯田
	// 创建时间:  2017-9-14 13:46
	// 参数说明:  void
	// 返 回 值:  true/false
	//////////////////////////////////////////////////////////////////////////
	bool Exit();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  创建新的窗口
	// 作    者:  邵凯田
	// 创建时间:  2017-8-4 13:05
	// 参数说明:  @sFunName为功能点名称
	//         :  @parent为父窗口名称
	// 返 回 值:  CBaseView*表示新打开的窗口指针, NULL表示插件中没有对应的功能点窗口
	//////////////////////////////////////////////////////////////////////////
	CBaseView* NewView(SString sFunName,QWidget* parent);

	SPtrList<CSspPlugin> m_Plugins;
};

#endif//__SSP_PLUGIN_MGR_H__
