#ifndef DLG_LOGIN_NEW_H
#define DLG_LOGIN_NEW_H

#include <QDialog>
#include "ui_dlg_login_new.h"
#include "ssp_gui.h"

class dlg_login_new : public QDialog
{
	Q_OBJECT

public:
	dlg_login_new(QWidget *parent = 0);
	~dlg_login_new();
public slots:
		void OnOk();
		void OnCancel();
		void showErrorLabel(QString text);
public:
	SString m_sUser;
	bool m_bOk;
private:
	Ui::dlg_login_new ui;
};

#endif // DLG_LOGIN_NEW_H
