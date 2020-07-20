/**
 *
 * 文 件 名 : ssp_plugin_mgr.cpp
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
#include "ssp_plugin_mgr.h"
#include "SApplication.h"
#include "SFile.h"
#include "sp_config_mgr.h"
#include "ssp_base.h"

CSspPlugin::CSspPlugin()
{

}
CSspPlugin::~CSspPlugin()
{

}
//////////////////////////////////////////////////////////////////////////
// 描    述:  加载插件
// 作    者:  邵凯田
// 创建时间:  2017-8-4 10:52
// 参数说明:  @sPluginFile为插件动态库文件全路径文件名
// 返 回 值:  true表示加载成功，false表示加载失败
//////////////////////////////////////////////////////////////////////////
bool CSspPlugin::Load(SString sPluginFile)
{
	m_sPluginFile = sPluginFile;
	bool err = false;
#ifdef WIN32
	m_hInstance = LoadLibrary(sPluginFile.data());
	if(m_hInstance == NULL)
	{
		LOGWARN("加载插件%s失败!",sPluginFile.data());
		return false;
	}

	m_pSSP_Init = (pFun_SSP_Init)GetProcAddress(m_hInstance, "SSP_Init");
	m_pSSP_Exit = (pFun_SSP_Exit)GetProcAddress(m_hInstance, "SSP_Exit");
	m_pSSP_GetPluginName = (pFun_SSP_GetPluginName)GetProcAddress(m_hInstance, "SSP_GetPluginName");
	m_pSSP_GetPluginVer = (pFun_SSP_GetPluginVer)GetProcAddress(m_hInstance, "SSP_GetPluginVer");
	m_pSSP_FunPointSupported = (pFun_SSP_FunPointSupported)GetProcAddress(m_hInstance, "SSP_FunPointSupported");
	//m_pSSP_SetGlobalPtr = (pFun_SSP_SetGlobalPtr)GetProcAddress(m_hInstance, "SSP_SetGlobalPtr");
	m_pSSP_NewView = (pFun_SSP_NewView)GetProcAddress(m_hInstance, "SSP_NewView");
	m_pSSP_GActSupported = (pFun_SSP_GActSupported)GetProcAddress(m_hInstance, "SSP_GActSupported");

#else
	m_hInstance = dlopen(sPluginFile.data(),RTLD_LAZY);
	char *pErr;

	if ((pErr = dlerror()) != NULL)
	{
		err = true;
		LOGWARN("dlopen error:%s\n",pErr);
	}
	if(m_hInstance == NULL)
	{
		LOGWARN("加载插件%s失败!",sPluginFile.data());
		return false;
	}
	m_pSSP_Init = (pFun_SSP_Init)dlsym(m_hInstance, "SSP_Init");
	if ((pErr = dlerror()) != NULL)
	{
		err = true;
		LOGWARN("dlsym error:%s\n",pErr);
	}
	m_pSSP_Exit = (pFun_SSP_Exit)dlsym(m_hInstance, "SSP_Exit");
	m_pSSP_GetPluginName = (pFun_SSP_GetPluginName)dlsym(m_hInstance, "SSP_GetPluginName");
	if ((pErr = dlerror()) != NULL)
	{
		err = true;
		LOGWARN("dlsym error:%s\n",pErr);
	}
	m_pSSP_GetPluginVer = (pFun_SSP_GetPluginVer)dlsym(m_hInstance, "SSP_GetPluginVer");
	if ((pErr = dlerror()) != NULL)
	{
		err = true;
		LOGWARN("dlsym error:%s\n",pErr);
	}
	m_pSSP_FunPointSupported = (pFun_SSP_FunPointSupported)dlsym(m_hInstance, "SSP_FunPointSupported");
	if ((pErr = dlerror()) != NULL)
	{
		err = true;
		LOGWARN("dlsym error:%s\n",pErr);
	}
	m_pSSP_NewView = (pFun_SSP_NewView)dlsym(m_hInstance, "SSP_NewView");
	if ((pErr = dlerror()) != NULL)
	{
		err = true;
		LOGWARN("dlsym error:%s\n",pErr);
	}
	m_pSSP_GActSupported = (pFun_SSP_GActSupported)dlsym(m_hInstance, "SSP_GActSupported");
// 	if ((pErr = dlerror()) != NULL)
// 	{
// 		err = true;
// 		LOGWARN("dlsym error:%s\n",pErr);
// 	}

#endif
	if(err || m_pSSP_Init == NULL || m_pSSP_GetPluginName == NULL || m_pSSP_GetPluginVer == NULL || m_pSSP_FunPointSupported == NULL || m_pSSP_NewView == NULL)
	{
		if(m_pSSP_Init == NULL && m_pSSP_GetPluginName == NULL && m_pSSP_GetPluginVer == NULL && m_pSSP_FunPointSupported == NULL && m_pSSP_NewView == NULL)
		{
			LOGDEBUG("加载插件%s失败!忽略所有入口均不具备的DLL，完全不是HMI插件!",sPluginFile.data());
			return false;//忽略所有入口均不具备的DLL，完全不是插件
		}
		LOGWARN("加载插件%s失败!",sPluginFile.data());
		return false;
	}
#ifdef _DEBUG
	int ret = m_pSSP_Init("debug=true;");
#else
	int ret = m_pSSP_Init("");
#endif
	if(ret != 0)
	{
		if(ret == 1 || ret == 2)
		{
			SString text;
#ifdef _DEBUG
			text.sprintf("不能加载非DEBUG版本插件[%s]!",sPluginFile.data());
#else
			text.sprintf("不能加载DEBUG版本插件[%s]!",sPluginFile.data());
#endif
			//SQt::ShowErrorBox("错误",text);
			LOGDEBUG("插件%s初始化失败!ret=%d, DEBUG版本不一致，忽略",sPluginFile.data(),ret);
		}
		else
		{
			LOGWARN("插件%s初始化失败!ret=%d",sPluginFile.data(),ret);
		}
		return false;
	}
	//m_pSSP_SetGlobalPtr(1,CConfigMgr::GetPtr());
	//m_pSSP_SetGlobalPtr(2,ssp_base::GetPtr());
	LOGDEBUG("插件%s加载成功!",sPluginFile.data());
	return true;
}


CSspPluginMgr::CSspPluginMgr()
{
	m_Plugins.setAutoDelete(true);
}

CSspPluginMgr::~CSspPluginMgr()
{

}

//////////////////////////////////////////////////////////////////////////
// 描    述:  初始化插件管理器，加载所有插件
// 作    者:  邵凯田
// 创建时间:  2017-8-4 11:16
// 参数说明:  void
// 返 回 值:  true表示初始化成功，false表示失败
//////////////////////////////////////////////////////////////////////////
bool CSspPluginMgr::Init()
{
	SString sPath = SBASE_SAPP->GetBinPath();
	SStringList pathList;
	pathList.append(sPath);					// 兼容当前目录

#ifndef WIN32
	pathList.append(sPath + "../lib/");		// 新程序改为在lib目录下(LINUX)
#endif

	LOGDEBUG("准备加载目录[%s]HMI插件",sPath.data());

	for (int dirIdx = 0; dirIdx < pathList.count(); ++dirIdx)
	{
		sPath = *(pathList.at(dirIdx));
#ifdef WIN32
		SDir dir(sPath,"*.dll");
#else
		SDir dir(sPath,"*.so");
#endif
		int i,cnt = dir.count();
		for(i=0;i<cnt;i++)
		{
			SString sFile = dir[i];
			printf("dir %d = %s  ",i,sFile.data());
#ifdef WIN32
			if(sFile == "." || sFile == ".." || sFile.left(7) != "plugin_" || sFile.right(4) != ".dll")
#else
			if(sFile == "." || sFile == ".." || sFile.left(10) != "libplugin_" || sFile.right(3) != ".so")
#endif
			{
				printf("忽略! \n");
				continue;
			}
			printf("加载...\n");
			SString attr = dir.attribute(i);
			if(SString::GetAttributeValueI(attr,"isdir") != 1)
			{
				CSspPlugin *pPlugin = new CSspPlugin();
				if(!pPlugin->Load(sPath + sFile))
				{
					delete pPlugin;
				}
				else
					m_Plugins.append(pPlugin);
			}
		}
	}
	
	return true;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  退出所有插件
// 作    者:  邵凯田
// 创建时间:  2017-9-14 13:46
// 参数说明:  void
// 返 回 值:  true/false
//////////////////////////////////////////////////////////////////////////
bool CSspPluginMgr::Exit()
{
	unsigned long pos;
	CSspPlugin *p = m_Plugins.FetchFirst(pos);
	while(p)
	{
		if(p->m_pSSP_Exit)
			p->m_pSSP_Exit();
		p = m_Plugins.FetchNext(pos);
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  创建新的窗口
// 作    者:  邵凯田
// 创建时间:  2017-8-4 13:05
// 参数说明:  @sFunName为功能点名称
//         :  @parent为父窗口名称
// 返 回 值:  CBaseView*表示新打开的窗口指针, NULL表示插件中没有对应的功能点窗口
//////////////////////////////////////////////////////////////////////////
CBaseView* CSspPluginMgr::NewView(SString sFunName,QWidget* parent)
{
	unsigned long pos;
	CBaseView *pView;
	CSspPlugin *p = m_Plugins.FetchFirst(pos);
	while(p)
	{
		pView = (CBaseView*)p->m_pSSP_NewView(sFunName.data(),parent);
		if(pView != NULL)
			return pView;
		p = m_Plugins.FetchNext(pos);
	}
	return NULL;
}
