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
#include "GuiApplication.h"
CGuiApplication* g_pGuiApplication=NULL;

CGuiApplication::CGuiApplication(void)
{
	m_pMainWnd = NULL;
	m_pQApp = NULL;
	g_pGuiApplication = this;
	m_pDatabase = new CSsp_Database();			//数据库配置文件
	m_pDatawindow = new CSsp_DatawindowMgr();	//数据窗口配置文件
	m_pFunpoint = new CSsp_FunPointConfig();	//功能点配置文件
	m_pSvgLibCfg = new CSp_SvgLibConfig();
}


CGuiApplication::~CGuiApplication(void)
{
	if(m_pMainWnd != NULL)
	{
		delete m_pMainWnd;
		m_pMainWnd = NULL;
	}
	m_pDatawindow->Clear();
	Stop();	
	if(m_pDatawindow != NULL)
		delete m_pDatawindow;
	if(m_pDatabase != NULL)
		delete m_pDatabase;
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
// 	if(!SApplication::Start())
// 		return false;
	//TODO:添加启动窗口

	if(!m_pDatabase->Load(GetConfPath()+"sys_database.xml"))
	{
		LOGFAULT("数据库配置文件打开失败!file:%s", m_pDatabase->GetPathFile().data());
		SQt::ShowErrorBox(QObject::tr("错误"),QObject::tr("数据库配置文件打开失败!将无法使用部分在线功能!"));
		//return false;
	}
	m_pDatawindow->SetDbOper(DB);
	m_pDatawindow->SetMdbOper(MDB);
	//if(!m_pDatawindow->Load(GetConfPath()+"sys_datawindow.xml"))
	if(!m_pDatawindow->LoadByDb())
	{
		LOGFAULT("数据窗口配置文件打开失败!file:%s", m_pDatawindow->GetPathFile().data());
		//SQt::ShowErrorBox(QObject::tr("错误"),QObject::tr("数据窗口配置文件打开失败!"));
		//return false;
	}
#if 0
	if(!m_pFunpoint->LoadByDb(DB,""))
	{
		LOGFAULT("从数据库加载功能点配置时失败!");
		SQt::ShowErrorBox(QObject::tr("错误"),QObject::tr("从数据库加载功能点配置时失败!"));
		return false;
	}
#else
	SString sText = "<?xml version='1.0' encoding='GBK'?>\r\n"
					"<fun_point key='' name='SSP平台开发助手' desc='根功能节点，表示系统名称'>\r\n"
					"	<fun_point key='code' name='代码生成' desc=''>\r\n"
					"		<fun_point key='code.db' name='数据库助手' desc='' >\r\n"
					"			<fun_point key='code.mysql.helper' name='MySQL数据库助手' desc='' />\r\n"
					"			<fun_point key='code.oracle.helper' name='Oracle数据库助手' desc='' />\r\n"
					"			<fun_point key='code.db.mdbsql' name='内存表SQL语句生成' desc='' />\r\n"
					"		</fun_point>\r\n"
					"		<fun_point key='code.common' name='常用功能' desc='' >\r\n"
					"			<fun_point key='code.xml2saxex' name='XML转SaxEx代码' desc='' />\r\n"
					"			<fun_point key='code.file2cpp' name='文件转C++数组定义' desc='' />\r\n"
					"			<fun_point key='code.cpp2file' name='C++数组定义转文件' desc='' />\r\n"
					"		</fun_point>\r\n"
					"	</fun_point>\r\n"
					"	<fun_point key='prj' name='软件项目' desc=''>\r\n"
					"		<fun_point key='prj.helper' name='项目助手' desc='' >\r\n"
					"			<fun_point key='prj.new.plugin' name='HMI插件项目创建' desc='' />\r\n"
					"			<fun_point key='prj.new.cmd' name='控制台项目创建' desc='' />\r\n"
					"			<fun_point key='prj.new.gui' name='人机界面项目创建' desc='' />\r\n"
					"		</fun_point>\r\n"
					"	</fun_point>\r\n"
					"\r\n"
					"\r\n"
					"\r\n"
					"	<fun_point key='hlp' name='帮助' desc=''>\r\n"
					"		<fun_point key='hlp.hlp' name='帮助' desc='' >\r\n"
					"			<fun_point key='sys.about' name='帮助' desc='' />\r\n"
					"		</fun_point>\r\n"
					"	</fun_point>\r\n"
					"</fun_point>";
	m_pFunpoint->SetFunPointXmlText(sText);
// 	if(!m_pFunpoint->Load(GetConfPath()+"sys_funpoint.xml"))
// 	{
// 		LOGFAULT("功能点配置文件打开失败!file:%s", m_pFunpoint->GetPathFile().data());
// 		SQt::ShowErrorBox(QObject::tr("错误"),QObject::tr("功能点配置文件打开失败!"));
// 		return false;
// 	}
#endif
// 	if(!m_pSvgLibCfg->Load(GetConfPath()+"sys_svg_library.xml"))
// 	{
// 		LOGFAULT("SVG图元库配置文件打开失败!file:%s", m_pSvgLibCfg->GetPathFile().data());
// 		SQt::ShowErrorBox(QObject::tr("错误"),QObject::tr("SVG图元库配置文件打开失败!"));
// 		//return false;
// 	}

	StartAgent(false);//以非阻塞方式运行代理客户端
	if(m_pMainWnd == NULL)
	{
		m_pMainWnd = new CGuiFrame(NULL);
		m_pMainWnd->SetStartSysName("SSP01平台开发助手V1.0.0");
		m_pMainWnd->Start();
	}

	m_pMainWnd->InitModule(m_pFunpoint->GetFunPointXmlText());
	m_pMainWnd->showMaximized();
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
