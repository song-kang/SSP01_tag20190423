#include "view_singleuser.h"

view_singleUser::view_singleUser(QWidget *parent)
	: QWidget(parent),pwd_login(1)
{
	ui.setupUi(this);
}

view_singleUser::~view_singleUser()
{

}
void view_singleUser::on_pushButton_pressed()
{
	pwd_login::LOGSTATE lt;
	lt=logInSys(DB,ui.lineEdit->text().toStdString().data(),ui.lineEdit_2->text().toStdString().data());
	emit loginSucessed(lt,ui.lineEdit->text().toStdString().data()+(","+getLogInfo()));
}