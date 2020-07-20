#ifndef DLG_LOGIN_BLUESTYLE_H
#define DLG_LOGIN_BLUESTYLE_H

#include <QDialog>
#include "ui_dlg_login_bluestyle.h"
#include "ssp_gui.h"

class dlg_login_blueStyle : public QDialog
{
	Q_OBJECT

public:
	dlg_login_blueStyle(QWidget *parent = 0);
	~dlg_login_blueStyle();
public slots:
		void OnOk();
		void OnCancel();
		void showErrorLabel(QString text);
public:
	SString m_sUser;
	bool m_bOk;
private:
	Ui::dlg_login_blueStyle ui;
};

#endif // DLG_LOGIN_BLUESTYLE_H
