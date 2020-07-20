#include "view_login.h"
#include "view_singleuser.h"
#include "view_doubleuser.h"
view_login::view_login(LOGINWINDOWTYPE usertype,QWidget *parent)
	: QDialog(parent)
{
	m_bLogin=false;
	//BOOLCHECK(pdb->TestConnect(),tr("数据库连接失败！"));
	switch(usertype)
	{
	case LOGIN_WND_TYPE_LOGIN:
		m_logWidget=new view_singleUser(this);
		m_bSingle=true;
		break;
	case LOGIN_WND_TYPE_CHECK:
		m_logWidget=new view_doubleUser(this);
		m_bSingle=false;
		break;
	default:
		break;
	}
	QObject::connect(m_logWidget,SIGNAL(loginSucessed(pwd_login::LOGSTATE,SString)),this,SLOT(onLoginSucessed(pwd_login::LOGSTATE,SString)));
	//this->setWindowModality(Qt::ApplicationModal);
	setWindowTitle(tr("登录"));
	//QGridLayout * gridlayout=new QGridLayout(this);
	//gridlayout->addWidget(m_logWidget);
}
void view_login::onLoginSucessed(pwd_login::LOGSTATE state,SString sInfo)
{
	switch(state)
	{
	case pwd_login::ALLIN:
		m_bLogin=true;
		if (m_bSingle)
		{
			m_sUser=SString::GetIdAttribute(1,sInfo,",");
		}else
		{
			m_sMonitor=SString::GetIdAttribute(1,sInfo,",");
		}
		this->close();
		break;
	case pwd_login::ONEIN:
		m_bLogin=false;
		m_sUser=SString::GetIdAttribute(1,sInfo,",");
		break;
	case pwd_login::NOIN:
		m_bLogin=false;
		QMessageBox::information(NULL,"tip",SString::GetIdAttribute(2,sInfo,",").data());
		break;
	}
}
view_login::~view_login()
{
	if (m_logWidget!=NULL)
	{
		delete m_logWidget;
		m_logWidget=NULL;
	}
}
