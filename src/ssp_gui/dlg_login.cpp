#include "dlg_login.h"

dlg_login::dlg_login(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	setWindowModality(Qt::ApplicationModal);
	#ifdef WIN32
		setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
	#else
	#endif
	connect(ui.pushButtonOk,SIGNAL(clicked()),this,SLOT(OnOk()));
	connect(ui.pushButtonCancel,SIGNAL(clicked()),this,SLOT(OnCancel()));
	m_bOk = false;
}

dlg_login::~dlg_login()
{

}

void dlg_login::OnOk()
{
	m_sUser = ui.lineEditName->text().toStdString().data();
	SString pwd = ui.lineEditPassword->text().toStdString().data();
	SString sql;
	sql.sprintf("select usr_code,pwd from t_ssp_user where usr_code='%s' ",m_sUser.data());
	SRecordset rs;
	DB->Retrieve(sql,rs);
	if(rs.GetRows() <= 0)
	{
		SQt::ShowErrorBox(tr("错误"),tr("指定的用户名不存在!"));
		return ;
	}
	if(rs.GetValue(0,0) != m_sUser || rs.GetValue(0,1) != pwd)
	{
		SQt::ShowErrorBox(tr("错误"),tr("登录密码错误!"));
		return ;
	}
	m_bOk = true;
	accept();
}
void dlg_login::OnCancel()
{
	m_bOk = false;
	reject();
}