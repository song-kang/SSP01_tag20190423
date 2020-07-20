#include "dlg_login_new.h"

dlg_login_new::dlg_login_new(QWidget *parent)
	: QDialog(parent)
{;
	ui.setupUi(this);	
	setWindowModality(Qt::ApplicationModal);
#ifdef WIN32
	setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
#else
#endif
	connect(ui.toolButtonOK,SIGNAL(clicked()),this,SLOT(OnOk()));
	connect(ui.toolButtonCancel,SIGNAL(clicked()),this,SLOT(OnCancel()));
	connect(ui.lineEditName,SIGNAL(textChanged(QString)),this,SLOT(showErrorLabel(QString)));
	connect(ui.lineEditPwd,SIGNAL(textChanged(QString)),this,SLOT(showErrorLabel(QString)));
	m_bOk = false;

	//设置关于窗体为圆角

	QBitmap bmp(this->size());

	bmp.fill();

	QPainter p(&bmp);

	p.setPen(Qt::NoPen);

	p.setBrush(Qt::black);

	p.drawRoundedRect(bmp.rect(),10,10);

	setMask(bmp);

	ui.lineEditName->setFocus();
	ui.labelError->setHidden(true);
	
}

dlg_login_new::~dlg_login_new()
{
	
}

void dlg_login_new::OnOk()
{
	m_sUser = ui.lineEditName->text().toStdString().data();
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

void dlg_login_new::OnCancel()
{
	m_bOk = false;
	reject();
}

void dlg_login_new::showErrorLabel(QString text)
{
	ui.labelError->setHidden(true);
}
