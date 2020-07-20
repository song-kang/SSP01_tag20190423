/**
 *
 * 文 件 名 : ConfigMgr.cpp
 * 创建日期 : 2015-7-29 13:53
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : 配置文件管理器，管理所有配置文件，支持配置文件的动态加载（自动监视配置文件变化）
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2015-7-29	邵凯田　创建文件
 *
 **/

#include "sp_config_mgr.h"

static CConfigMgr* g_pConfigMgr = NULL;


//////////////////////////////////////////////////////////////////////////
// 名    称:  CConfigBase
// 作    者:  邵凯田
// 创建时间:  2015-7-29 13:56
// 描    述:  配置文件基类，定义配置文件加载、变更等公共接口，所有受控配置文件应以此接口
//////////////////////////////////////////////////////////////////////////

CConfigBase::CConfigBase()
{
	m_ConfigType = SPCFG_UNKNOWN;
	m_bAutoRelaod = false;
	if(g_pConfigMgr == NULL)
		g_pConfigMgr = new CConfigMgr();
	g_pConfigMgr->AddConfig(this);
}

CConfigBase::~CConfigBase()
{
	if(g_pConfigMgr != NULL)
		g_pConfigMgr->RemoveConfig(this);
}

//////////////////////////////////////////////////////////////////////////
// 名    称:  CConfigMgr
// 作    者:  邵凯田
// 创建时间:  2015-7-29 14:16
// 描    述:  配置文件管理器类
//////////////////////////////////////////////////////////////////////////
CConfigMgr::CConfigMgr()
{
	m_bQuit = true;
	m_iReloadSecs = 5;
	m_Configs.setAutoDelete(false);
	m_RemovedConfigs.setAutoDelete(true);
	m_ppConfig = new CConfigBase*[SPCFG_MAX_SIZE];//最多支持100个配置
	memset(m_ppConfig,0,sizeof(CConfigBase*)*SPCFG_MAX_SIZE);
}

CConfigMgr::~CConfigMgr()
{
	delete[] m_ppConfig;
	m_Configs.clear();
	m_RemovedConfigs.clear();
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  取唯一全局指针
// 作    者:  邵凯田
// 创建时间:  2017-8-4 17:06
// 参数说明:  void
// 返 回 值:  CConfigMgr*
//////////////////////////////////////////////////////////////////////////
CConfigMgr* CConfigMgr::GetPtr()
{
	return g_pConfigMgr;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  设备唯一指针（动态库中使用）
// 作    者:  邵凯田
// 创建时间:  2017-8-4 17:05
// 参数说明:  @ptr表示中唯一指针
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CConfigMgr::SetPtr(CConfigMgr *ptr)
{
	g_pConfigMgr = ptr;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  设置重新加载间隔秒数
// 作    者:  邵凯田
// 创建时间:  2015-7-29 14:27
// 参数说明:  @iSec为秒数
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CConfigMgr::SetReloadSeconds(int iSec)
{
	g_pConfigMgr->m_iReloadSecs = iSec;
};

//////////////////////////////////////////////////////////////////////////
// 描    述:  开启重新加载服务,将启动配置文件扫描线程，定时扫描文件修改时间，
//			  定时间隔默认5秒，可通过SetReloadSeconds修改
// 作    者:  邵凯田
// 创建时间:  2015-7-29 15:17
// 参数说明:  void
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CConfigMgr::StartReload()
{
	if(!g_pConfigMgr->m_bQuit)
		return;
	g_pConfigMgr->m_bQuit = false;
	S_CREATE_THREAD(ThreadReload,g_pConfigMgr);
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  退出重新加载服务线程，系统退出时须调用
// 作    者:  邵凯田
// 创建时间:  2015-7-29 15:19
// 参数说明:  void
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CConfigMgr::Quit()
{
	if(g_pConfigMgr != NULL)
	{
		if(g_pConfigMgr->m_bQuit == false)
		{
			g_pConfigMgr->m_bQuit = true;
			while(g_pConfigMgr->m_Configs.count() > 0)
				SApi::UsSleep(1000);
		}
		else
		{
			SApi::UsSleep(100000);
			g_pConfigMgr->m_Configs.clear();
		}
		delete g_pConfigMgr;
		g_pConfigMgr = NULL;
	}
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  添加配置文件类到管理器
// 作    者:  邵凯田
// 创建时间:  2015-7-29 15:24
// 参数说明:  @pCfg为配置文件实例指针
// 返 回 值:  true表示添加成功，false表示失败（指定类型配置文件已存在)
//////////////////////////////////////////////////////////////////////////
bool CConfigMgr::AddConfig(CConfigBase *pCfg)
{
	g_pConfigMgr->m_Configs.append(pCfg);
	return true;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  删除指定的配置文件
// 作    者:  邵凯田
// 创建时间:  2015-11-12 10:03
// 参数说明:  @pCfg
//         :  @bAddtoRemovedList是否将删除的配置文件添加到释放队列，退出应用时释放
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CConfigMgr::RemoveConfig(CConfigBase *pCfg,bool bAddtoRemovedList/*=false*/)
{
	g_pConfigMgr->m_Configs.remove(pCfg);
	if(bAddtoRemovedList)
		g_pConfigMgr->m_RemovedConfigs.append(pCfg);
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  取指定类型的配置文件指针
// 作    者:  邵凯田
// 创建时间:  2015-7-29 15:32
// 参数说明:  @cfgType表示配置文件类型
// 返 回 值:  CConfigBase*，NULL表示不存在
//////////////////////////////////////////////////////////////////////////
CConfigBase* CConfigMgr::GetConfig(eSpConfigType cfgType)
{
	unsigned long pos=0;
	CConfigBase *p ;
	if(cfgType <= 0 || cfgType >= SPCFG_MAX_SIZE || g_pConfigMgr == NULL)
		return NULL;
	p = g_pConfigMgr->m_ppConfig[(int)cfgType];
	if(p != NULL)
		return p;
	p = g_pConfigMgr->m_Configs.FetchFirst(pos);
	while(p)
	{
		if(p->GetConfigType() == cfgType)
		{
			g_pConfigMgr->m_ppConfig[(int)cfgType] = p;
			return p;
		}
		p = g_pConfigMgr->m_Configs.FetchNext(pos);
	}
	LOGWARN("指定配置文件类型[%d]不存在!",cfgType);
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  所有受控配置文件自动重新加载的扫描线程
// 作    者:  邵凯田
// 创建时间:  2015-7-29 15:26
// 参数说明:  @lp为this指针
// 返 回 值:  NULL
//////////////////////////////////////////////////////////////////////////
void* CConfigMgr::ThreadReload(void* lp)
{
	CConfigMgr *pThis = (CConfigMgr*)lp;
	unsigned long pos;
	CConfigBase *p;
	bool ret;
	S_INTO_THREAD;
	while(!pThis->m_bQuit)
	{
		p = pThis->m_Configs.FetchFirst(pos);
		while(p)
		{
			if(p->GetAutoReload())
			{
				if(SFile::filetime(p->GetPathFile()) != p->m_LastModifyTime)
				{
					ret = p->ReLoad();
					if(ret == false)
					{
						LOGERROR("重新加载配置文件[%s]时失败!",p->GetPathFile().data());
						p->m_LastModifyTime = SFile::filetime(p->GetPathFile());
					}
					else
					{
						LOGDEBUG("配置文件[%s]变化,将重新加载!",p->GetPathFile().data());
					}
				}
			}
			p = pThis->m_Configs.FetchNext(pos);
		}
		SApi::UsSleep(pThis->m_iReloadSecs*1000000);
	}
	pThis->m_Configs.clear();
	return NULL;
}
