#include "view_doubleuser.h"

view_doubleUser::view_doubleUser(QWidget *parent)
	: QWidget(parent),pwd_login(2)
{
	ui.setupUi(this);
	m_in[0]="";
	m_in[1]="";
}

view_doubleUser::~view_doubleUser()
{

}
void view_doubleUser::on_pushButton_pressed()
{

	SString sUser,sPwd,sUser1,sPwd1;
	sUser=ui.lineEdit->text().toStdString();
	sUser1=ui.lineEdit_4->text().toStdString();
	sPwd=ui.lineEdit_2->text().toStdString();
	sPwd1=ui.lineEdit_3->text().toStdString();
	if (sUser==sUser1&&sUser!="")
	{
		QMessageBox::information(NULL,"tip",tr("用户名重复！"));
		ui.lineEdit->clear();
		ui.lineEdit_4->clear();
		return;
	}
	if (sUser==""||sUser1=="")
	{
		QMessageBox::information(NULL,"tip",tr("请输入用户名!"));
		return;
	}
	if (sPwd==""||sPwd1=="")
	{
		QMessageBox::information(NULL,"tip",tr("请输入密码!"));
		return;
	}
	pwd_login::LOGSTATE lt;
	if (logInSys(DB,sUser,sPwd)==pwd_login::NOIN)
	{
		emit loginSucessed(NOIN,getLogInfo());
		return;
	}
	emit loginSucessed(ONEIN,sUser+","+getLogInfo());
	lt=logInSys(DB,sUser1,sPwd1);
	emit loginSucessed(lt,sUser1+","+getLogInfo());
}
//void view_doubleUser::on_pushButton_2_pressed()
//{
//	SString sUser,sPwd;
//	sUser=ui.lineEdit_3->text().toStdString();
//	sPwd=ui.lineEdit_4->text().toStdString();
//	if (sUser==m_in[0]&&sUser!="")
//	{
//		QMessageBox::information(NULL,"tip",tr("重复登录！"));
//		ui.lineEdit_3->clear();
//		ui.lineEdit_4->clear();
//		return;
//	}
//	pwd_login::LOGSTATE lt;
//	lt=logInSys(DB,sUser,sPwd);
//	if (lt!=pwd_login::NOIN)
//	{
//		m_in[1]=sUser;
//		ui.pushButton_2->setEnabled(false);
//	}
//	emit loginSucessed(lt,getLogInfo());
//}