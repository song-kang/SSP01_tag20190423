#include "dlg_operlogin.h"

dlg_operlogin::dlg_operlogin(bool bWithMonitor,SString sTip,QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	connect(ui.pushButtonOk,SIGNAL(clicked()),this,SLOT(OnOk()));
	connect(ui.pushButtonCancel,SIGNAL(clicked()),this,SLOT(OnCancel()));
	ui.plainTextTip->setPlainText(sTip.data());
	m_bWithMonitor = bWithMonitor;
	if(!m_bWithMonitor)
	{
		ui.lineEditMonUser->setEnabled(false);
		ui.lineEditMonPassword->setEnabled(false);
	}
	m_bOk = false;
}

dlg_operlogin::~dlg_operlogin()
{

}


void dlg_operlogin::OnOk()
{
	m_sUser = ui.lineEditUser->text().toStdString().data();
	SString pwd = ui.lineEditPassword->text().toStdString().data();
	SString sql;
	SRecordset rs;
	sql.sprintf("select usr_code,pwd from t_ssp_user where usr_code='%s' ",m_sUser.data());
	DB->Retrieve(sql,rs);
	if(rs.GetRows() <= 0)
	{
		SQt::ShowErrorBox(tr("错误"),tr("指定的操作人不存在!"));
		return;
	}
	if(rs.GetValue(0,0) != m_sUser || rs.GetValue(0,1) != pwd)
	{
		SQt::ShowErrorBox(tr("错误"),tr("操作人登录密码错误!"));
		return;
	}
	if(m_bWithMonitor)
	{
		if(m_sMonUser == m_sUser)
		{
			SQt::ShowErrorBox(tr("提示"),tr("监护人不能使用与操作人相同的账号!"));
			return;
		}
		m_sMonUser = ui.lineEditMonUser->text().toStdString().data();
		SString mon_pwd = ui.lineEditMonPassword->text().toStdString().data();
		sql.sprintf("select usr_code,pwd from t_ssp_user where usr_code='%s' ",m_sMonUser.data());
		DB->Retrieve(sql,rs);
		if(rs.GetRows() <= 0)
		{
			SQt::ShowErrorBox(tr("错误"),tr("指定的监护人不存在!"));
			return ;
		}
		if(rs.GetValue(0,0) != m_sMonUser || rs.GetValue(0,1) != mon_pwd)
		{
			SQt::ShowErrorBox(tr("错误"),tr("监护人登录密码错误!"));
			return ;
		}
	}


	m_bOk = true;
	accept();
}
void dlg_operlogin::OnCancel()
{
	m_bOk = false;
	reject();
}