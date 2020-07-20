/**
 *
 * 文 件 名 : viewabout.cpp
 * 创建日期 : 2013-9-13 8:52
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : 关于窗口
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2013-9-13	邵凯田　创建文件
 *
 **/

#include "viewabout.h"
#include "SApplication.h"
#include "ssp_database.h"

CAboutPic::CAboutPic(QWidget *parent) : QWidget(parent)
{
	if(!m_bmpAbout.load(":/ssp_gui/Resources/about.png"))
	{
		LOGWARN("load Resources/about.png error!");
	}

}

CAboutPic::~CAboutPic()
{

}

void CAboutPic::paintEvent(QPaintEvent *event)
{
	S_UNUSED(event);
	QPainter painter(this);
	
	painter.drawPixmap(0,0,m_bmpAbout.width(),m_bmpAbout.height(),m_bmpAbout,0,0,m_bmpAbout.width(),m_bmpAbout.height() );

}


CViewAbout::CViewAbout(QWidget *parent) : CBaseView(parent)
{
	ui.setupUi(this);
	m_pAboutPic = new CAboutPic(ui.widget);
	m_pAboutPic->move(0,0);
	m_pAboutPic->resize(ui.widget->width(),ui.widget->height());
	//setStyleSheet(QString::fromUtf8("background-color: rgb(220, 230, 240);"));
}

CViewAbout::~CViewAbout()
{

}

extern SString g_sSystemName;
void CViewAbout::OnPreShow()
{
	SApplication *pApp =SApplication::GetPtr();
	ui.label->setText(g_sSystemName.data());
	SString str;
	if(pApp != NULL)
	{
		ui.labelVer->setText(SString::toFormat(Q2S(tr("HMI程序版本:V%s")).data(),pApp->GetModuleVer().data()).data());
		if(GET_DB_CFG != NULL)
		{
			str = DB->SelectInto("select db_ver from t_ssp_db_info order by db_sn desc");
			ui.labelDbVer->setText(SString::toFormat(Q2S(tr("数据库版本:V%s")).data(),str.data()).data());
		}
		else
			ui.labelDbVer->setText("");
		ui.labelCRC->setText(SString::toFormat(Q2S(tr("HMI程序校验码:%s")).data(),pApp->GetModuleCrc().data()).data());
		ui.labelBuild->setText(SString::toFormat(Q2S(tr("HMI程序生成日期:%s")).data(),pApp->GetModuleTime().data()).data());
		str = Q2S(tr("无"));
		if(GET_DB_CFG == NULL)
		{
			ui.labelDbInst->setText("");
			ui.labelDbInst_2->setText("");
			ui.labelMDbInst->setText("");
		}
		else
		{
			if(DB->GetDatabasePool() != NULL && DB->GetDatabasePool()->GetPoolSize() > 0)
			{
				SString sParams = DB->GetDatabasePool()->GetParams();
				str.sprintf("%s:%s / %s",
					SString::GetAttributeValue(sParams,"hostaddr").data(),
					SString::GetAttributeValue(sParams,"port").data(),
					SString::GetAttributeValue(sParams,"dbname").data());
			}
			ui.labelDbInst->setText(SString::toFormat(Q2S(tr("历史主数据库实例:%s")).data(),str.data()).data());
			str = Q2S(tr("无"));
			if(DB->GetSlaveDatabasePool() != NULL && DB->GetSlaveDatabasePool()->GetPoolSize() > 0)
			{
				SString sParams = DB->GetSlaveDatabasePool()->GetParams();
				str.sprintf("%s:%s / %s",
					SString::GetAttributeValue(sParams,"hostaddr").data(),
					SString::GetAttributeValue(sParams,"port").data(),
					SString::GetAttributeValue(sParams,"dbname").data());
			}
			ui.labelDbInst_2->setText(SString::toFormat(Q2S(tr("历史备数据库实例:%s")).data(),str.data()).data());
			str = Q2S(tr("无"));
			if(MDB->GetDatabasePool() != NULL && MDB->GetDatabasePool()->GetPoolSize() > 0)
			{
				SString sParams = MDB->GetDatabasePool()->GetParams();
				str.sprintf("%s:%s / %s",
					SString::GetAttributeValue(sParams,"hostaddr").data(),
					SString::GetAttributeValue(sParams,"port").data(),
					SString::GetAttributeValue(sParams,"dbname").data());
			}
			ui.labelMDbInst->setText(SString::toFormat(Q2S(tr("内存数据库实例:%s")).data(),str.data()).data());
		}
	}
// 	ui.labelVer->setText(SString::toFormat("HMI程序版本：V%s",BaseStationModule::GetPublicInfo()->m_sVersion.data()).data());
// 	//ui.labelVer->setText(SString::toFormat("程序版本：V1.2").data());
// 	ui.labelDbVer->setText(SString::toFormat("数据库版本：V%s",SelectInto("select ver_code from t_version").data()).data());
// 	//ui.labelBuildTime->setText(SString::toFormat("HMI程序生成时间：%s %s",__DATE__,__TIME__).data());
// 	ui.labelCRC->setText(SString::toFormat("HMI程序校验码：%s",BaseStationModule::GetPublicInfo()->m_sModuleDesc.data()).data());
// 	ui.labelDbInst->setText(SString::toFormat("当前库实例：%s / %s",m_sDbIp_p.data(),m_sDbInst_p.data()).data());
// 	ui.labelDbInst_2->setText(SString::toFormat("当前库实例：%s / %s",m_sDbIp_p.data(),m_sDbInst_p.data()).data());

// 	// 版本
// 	SString displayString;
// 	SString str = BaseStationModule::GetPublicInfo()->m_sVersion.data();
// 	int count = SString::GetAttributeCount(str, ".");
// 	for (int i = 1; i <= 3; ++i)
// 	{
// 		SString stemp = SString::GetIdAttribute(i, str, ".");
// 		stemp.sprintf("%02X", stemp.toInt());
// 		displayString += stemp;
// 	}

//	ui.label_info->setText(displayString.data());

// 	if(m_sDbIp_s.length() == 0)
// 		ui.labelDbInst_2->setText("网存库实例：未使用");
// 	else
// 		ui.labelDbInst_2->setText(SString::toFormat("网存库实例：%s / %s",m_sDbIp_s.data(),m_sDbInst_s.data()).data());

}

void CViewAbout::OnRefresh()
{
	OnPreShow();
}


CViewHelp::CViewHelp(QWidget *parent) : CBaseView(parent)
{
	m_pWebView = new QWebView(this);
	m_pWebView->showMaximized();

	m_gridLayout = new QGridLayout(this);
	m_gridLayout->setSpacing(6);
	m_gridLayout->setContentsMargins(0, 0, 0, 0);
	m_gridLayout->setObjectName(QString::fromUtf8("gridLayout"));

	m_gridLayout->addWidget(m_pWebView);

#ifdef WIN32
	SString url = SApplication::GetPtr()->GetHomePath()+"help\\help.html";
#else
	SString url = SApplication::GetPtr()->GetHomePath()+"help/help.html";
#endif
	m_pWebView->load(QUrl::fromUserInput(url.data()));
	
}

CViewHelp::~CViewHelp()
{
	
	if(m_pWebView != NULL)
	{
		//m_gridLayout->removeWidget(m_pWebView);
		//delete m_pWebView;
	}
	//m_pWebView->stop();
	//delete m_pWebView;
}

void CViewHelp::OnPreShow()
{

}

void CViewHelp::OnRefresh()
{

}

