#include "dlg_login_substation.h"

dlg_login_substation::dlg_login_substation(bool forOper, QWidget *parent)
	: QDialog(parent), for_oper(forOper)
{
	ui.setupUi(this);
	setWindowModality(Qt::ApplicationModal);
//#ifdef WIN32
	setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint|Qt::FramelessWindowHint);
//#else
//#endif
	connect(ui.toolButtonOK,SIGNAL(clicked()),this,SLOT(OnOk()));
	connect(ui.toolButtonCancel,SIGNAL(clicked()),this,SLOT(OnCancel()));
	//connect(ui.lineEditName,SIGNAL(textChanged(QString)),this,SLOT(showErrorLabel(QString)));
	connect(ui.lineEditPwd,SIGNAL(textChanged(QString)),this,SLOT(showErrorLabel(QString)));
	m_bOk = false;

	initLoginUser();

	//设置关于窗体为圆角

	QBitmap bmp(this->size());

	bmp.fill();

	QPainter p(&bmp);

	p.setPen(Qt::NoPen);

	p.setBrush(Qt::black);

	p.drawRoundedRect(bmp.rect(),10,10);

	setMask(bmp);

	//ui.lineEditName->setFocus();
	ui.labelError->setHidden(true);

	ui.toolButtonOK->setFocus(); //设置默认焦点

	ui.toolButtonOK->setShortcut( QKeySequence::InsertParagraphSeparator ); //设置快捷键为键盘的“回车”键

	ui.toolButtonOK->setShortcut(Qt::Key_Enter); //设置快捷键为enter键

	ui.toolButtonOK->setShortcut(Qt::Key_Return); //设置快捷键为小键盘上的enter键

	//linux上标题栏没有隐藏，设置窗口标题为登录
	setWindowTitle("Login");
}

dlg_login_substation::~dlg_login_substation()
{

}

void dlg_login_substation::initLoginUser() {
	QString grpCode("admin");
	if (!for_oper) {
		grpCode = "mon";
	}
	SString sql;
	//sql.sprintf("select usr_code, name from t_ssp_user where grp_code='%s' ",grpCode.toStdString().data());
	sql.sprintf("select usr_code, name from t_ssp_user",grpCode.toStdString().data());
	SRecordset rs;
	DB->Retrieve(sql,rs);
	if(rs.GetRows() <= 0)
	{
		//SQt::ShowErrorBox(tr("错误"),tr("指定的用户名不存在!"));
		ui.labelError->setHidden(false);
		return;
	}

	for (int i=0; i<rs.GetRows(); i++) {
		ui.cbxUser->addItem(rs.GetValue(i, 1).data(), QString(rs.GetValue(i, 0).data()));
	}
}

void dlg_login_substation::OnOk()
{
	m_sUser = ui.cbxUser->itemData(ui.cbxUser->currentIndex(), Qt::UserRole).toString().toStdString().c_str();
	SString pwd = ui.lineEditPwd->text().toStdString().data();
	SString sql;
	sql.sprintf("select usr_code,pwd from t_ssp_user where usr_code='%s' ",m_sUser.data());
	SRecordset rs;
	DB->Retrieve(sql,rs);
	if(rs.GetRows() <= 0)
	{
		//SQt::ShowErrorBox(tr("错误"),tr("指定的用户名不存在!"));
		ui.labelError->setHidden(false);
		return ;
	}
	if(rs.GetValue(0,0) != m_sUser || rs.GetValue(0,1) != pwd)
	{
		//SQt::ShowErrorBox(tr("错误"),tr("登录密码错误!"));
		ui.labelError->setHidden(false);
		return ;
	}
	m_bOk = true;
	accept();
}

void dlg_login_substation::OnCancel()
{
	m_bOk = false;
	reject();
}

void dlg_login_substation::showErrorLabel( QString text )
{
	ui.labelError->setHidden(true);
}
