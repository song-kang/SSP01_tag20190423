/**
 *
 * 文 件 名 : GuiApplication.cpp
 * 创建日期 : 2015-7-30 15:55
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
#include <io.h>
#include <fcntl.h>
#include <stdio.h>
#include "GuiApplication.h"

CGuiApplication* g_pGuiApplication=NULL;

CGuiApplication::CGuiApplication(void)
{
	m_pMainWnd = NULL;
	m_pQApp = NULL;
	g_pGuiApplication = this;
	m_SimManager.m_pSimConfig = &m_SimConfig;
	SetApplicationId(101);
	m_pWndRealEvent = NULL;
	m_pDatabase = NULL;
#if 0 
	m_pDatabase = new CSsp_Database();			//数据库配置文件
	m_pDatabase->SetPoolSize(2);
	//LoadLibrary("plugin_scl_importd.dll");
//林德智 20181112 添加有关SCD导入功能
	int db_pools = GetParamI("db_pools");
	if(db_pools == 0)
		db_pools = 10;
	if(db_pools >= 3 && db_pools < 100)
		m_pDatabase->SetPoolSize(db_pools);

	//加载数据库配置
	if(!m_pDatabase->Load(GetConfPath()+"sys_database.xml"))
	{
		LOGFAULT("数据库配置文件打开失败!file:%s", m_pDatabase->GetPathFile().data());
		return;
	}
	//closeScdToMDB();
#endif
}


CGuiApplication::~CGuiApplication(void)
{
	if(m_pMainWnd != NULL)
	{
		delete m_pMainWnd;
		m_pMainWnd = NULL;
	}
	if(m_pWndRealEvent != NULL)
		delete m_pWndRealEvent;
	Stop();
	if(m_pFunpoint != NULL)
		delete m_pFunpoint;
	CConfigMgr::Quit();

}

//////////////////////////////////////////////////////////////////////////
// 描    述:  取APP应用实例指针
// 作    者:  邵凯田
// 创建时间:  2015-7-30 16:05
// 参数说明:  void
// 返 回 值:  CGuiApplication*
//////////////////////////////////////////////////////////////////////////
CGuiApplication* CGuiApplication::GetApp()
{
	return g_pGuiApplication;
}

QApplication* CGuiApplication::GetQApp()
{
	return g_pGuiApplication->m_pQApp;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  设备QT应用类指针
// 作    者:  邵凯田
// 创建时间:  2015-7-30 16:05
// 参数说明:  @pApp
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CGuiApplication::SetQApplication(QApplication *pApp)
{
	m_pQApp = pApp;

	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GBK"));
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));
	QTextCodec::setCodecForTr(QTextCodec::codecForName("GBK"));

}

//////////////////////////////////////////////////////////////////////////
// 描    述:  启动服务，开启所有应用的服务，该函数必须由派生类实现，且启动服务完成后必须返回
// 作    者:  邵凯田
// 创建时间:  2015-7-30 16:05
// 参数说明:  void
// 返 回 值:  true/false
//////////////////////////////////////////////////////////////////////////
bool CGuiApplication::Start()
{
	//TODO:添加启动窗口
	m_pFunpoint = new CSsp_FunPointConfig();	//功能点配置文件

	SString sText = "<?xml version='1.0' encoding='GBK'?>\r\n"
					"<fun_point key='' name='UK-9000变电站智能告警分析功能测试平台' desc='根功能节点，表示系统名称'>\r\n"
					"	<fun_point key='home' name='首页' desc=''>\r\n"
					"		<fun_point key='home' name='首页' desc='' >\r\n"
					"			<fun_point key='home' name='首页' desc='' />\r\n"
					"		</fun_point>\r\n"
					"	</fun_point>\r\n"
					"	<fun_point key='mod' name='模型管理' desc=''>\r\n"
					"		<fun_point key='mod.scd' name='SCL模型' desc='' >\r\n"
					"			<fun_point key='mod.scd.edit' name='SCL模型编辑' desc='' />\r\n"
					"			<fun_point key='mod.scd.import' name='SCL模型导入' desc='' />\r\n"
					//"			<fun_point key='plugin_scl_import' name='SCD模型导入' desc='' />\r\n"
					"		</fun_point>\r\n"
					"		<fun_point key='mod.scd' name='模拟试验' desc='' >\r\n"
					"			<fun_point key='pcap.replay' name='PCAP文件回放' desc='' />\r\n"
					"		</fun_point>\r\n"
					"	</fun_point>\r\n"
// 					"	<fun_point key='prj' name='模型管理' desc=''>\r\n"
// 					"		<fun_point key='prj.helper' name='项目助手' desc='' >\r\n"
// 					"			<fun_point key='prj.new.cmd' name='控制台项目创建' desc='' />\r\n"
// 					"			<fun_point key='prj.new.gui' name='人机界面项目创建' desc='' />\r\n"
// 					"		</fun_point>\r\n"
// 					"		<fun_point key='code.db' name='数据库代码生成' desc='' >\r\n"
// 					"			<fun_point key='code.db.mdbstu' name='内存库C结构生成' desc='' />\r\n"
// 					"			<fun_point key='code.db.mdbsql' name='内存表SQL语句生成' desc='' />\r\n"
// 					"		</fun_point>\r\n"
// 					"	</fun_point>\r\n"
					"\r\n"
					"\r\n"
					"\r\n"
					"	<fun_point key='vis' name='可视化' desc=''>\r\n"
					"		<fun_point key='vis.vis' name='可视化' desc='' >\r\n"
					"			<fun_point key='vis.ap' name='访问点' desc='' />\r\n"
					"		</fun_point>\r\n"
					"	</fun_point>\r\n"
					"	<fun_point key='hlp' name='帮助' desc=''>\r\n"
					"		<fun_point key='hlp.hlp' name='关于' desc='' >\r\n"
					"			<fun_point key='sys.about' name='关于' desc='' />\r\n"
					"			<fun_point key='sys.help' name='帮助' desc='' />\r\n"
					"		</fun_point>\r\n"
					"	</fun_point>\r\n"
					"</fun_point>";
	m_pFunpoint->SetFunPointXmlText(sText);
	if(!m_SimConfig.Load(GetConfPath()+"sim_iec61850.xml"))
	{
		LOGERROR("加载配置文件时失败!");
		SQt::ShowErrorBox("提示",SString::toFormat("加载配置文件(%ssim_iec61850.xml)时失败",GetConfPath().data()).data());
//		return false;
	}

	StartAgent(false);//以非阻塞方式运行代理客户端
	if(m_pMainWnd == NULL)
		m_pMainWnd = new CGuiFrame(NULL);
	m_pMainWnd->Start();
	m_pMainWnd->InitModule(m_pFunpoint->GetFunPointXmlText());
	m_pMainWnd->showMaximized();
	m_pWndRealEvent = new CFloatWnd_RealEvent();
#ifdef WIN32
	m_pWndRealEvent->setWindowFlags( Qt::Dialog | Qt::Tool | Qt::WindowStaysOnTopHint);
#else
	m_pWndRealEvent->setWindowFlags( Qt::Dialog );
#endif
	//m_pWndRealEvent->hide();
	m_pWndRealEvent->resize(1000,300);

	m_pMainWnd->SetStartTipText("启动完成!");
	m_pMainWnd->HideStartWnd();
	m_pQApp->exec();
	return true;
}

////////////////////////////////////////////////////////////////////////
// 描    述:  停止服务，关闭所有应用的服务
// 作    者:  邵凯田
// 创建时间:  2015-7-30 16:05
// 参数说明:  void
// 返 回 值:  true/false
//////////////////////////////////////////////////////////////////////////
bool CGuiApplication::Stop()
{
	SLog::quitLog();
	m_pQApp->quit();
	return true;
}

////////////////////////////////////////////////////////////////////////
// 描    述:  取当前模块的运行状态信息，对应接入模块应列出所有接入设备的状态明细
// 作    者:  邵凯田
// 创建时间:  2015-7-30 16:05
// 参数说明:  void
// 返 回 值:  SString
//////////////////////////////////////////////////////////////////////////
SString CGuiApplication::GetNowModInfo()
{
	return "";
}

void CGuiApplication::NewEvent(SString sEventType,int soc,int usec,SString sIedDesc,SString sEventLevel,SString sEventInfo)
{
	if(m_pWndRealEvent == NULL)
		return;
	stuRealEvent *pEvent = new stuRealEvent();
	pEvent->sEventType = sEventType;
	pEvent->soc = soc;
	pEvent->usec = usec;
	if(pEvent->soc == 0)
		SDateTime::getSystemTime(pEvent->soc,pEvent->usec);
	pEvent->sIedDesc = sIedDesc;
	pEvent->sEventLevel = sEventLevel;
	pEvent->sEventInfo = sEventInfo;
	
	m_pWndRealEvent->m_RealEvents.append(pEvent);
}

void InitConsoleWindow()
{
	int nCrt = 0;
	FILE* fp;
	AllocConsole();
	nCrt = _open_osfhandle((long)GetStdHandle(STD_OUTPUT_HANDLE), _O_TEXT);
	fp = _fdopen(nCrt, "w");
	*stdout = *fp;
	setvbuf(stdout, NULL, _IONBF, 0);
}

#include <QDebug>
void CGuiApplication::closeScdToMDB()
{
	SString sql="select count(*) as num from t_ssp_setup where set_key='scl.mdb'";
	int iNum = DB->SelectIntoI(sql);
	bool bRst = false;
	if(iNum == 0)
	{
		sql = "insert into t_ssp_setup(set_key,type_id,set_desc,set_default,set_value,set_dime,set_class,val_type,val_fmt) "
			"value('scl.mdb',1,'SCL文件不同步导入内存库(0否1是)','0','1','',0,3,'')";
		bRst = DB->ExecuteSQL(sql);
	} else
	{
		sql = "update t_ssp_setup set set_value='1' where set_key='scl.mdb'";//强制将内存库操作关闭
		bRst = DB->ExecuteSQL(sql);
	}

	if (!bRst)
	{
		LOGERROR("数据库操作失败：%s!",sql.data());
	}
}