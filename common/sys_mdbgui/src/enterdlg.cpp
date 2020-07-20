#include "enterdlg.h"
#include <QKeyEvent>
enterDlg::enterDlg(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
}
bool enterDlg::event(QEvent *e)
{
////////////ÆÁ±Îesc¼ü///////////////
	if ( e->type() == QEvent::KeyPress )
	{
		QKeyEvent* ke = (QKeyEvent*) e;
		if (ke->key()==Qt::Key_Escape) 
		{
			return TRUE;
		}        
	}
////////////ÆÁ±Îesc¼ü///////////////
	return QDialog::event(e);
}
void enterDlg::on_pushButton_ok_pressed()
{
	ip =ui.lineEdit_ip->text().toStdString();
	port = ui.lineEdit_port->text().toInt();
	sParam = SString::toFormat("hostaddr=%s;port=%d;user=skt_mdb;password=skt001@163.com;", ip.data(), port);
	this->close();
	emit ok_pressed(sParam);
	i_ret = 0;
}
void enterDlg::on_pushButton_cancel_pressed()
{
	i_ret = -1;
	this->close();
}
int enterDlg::exec()
{
	QDialog::exec();
	return i_ret;
}
enterDlg::~enterDlg()
{

}
