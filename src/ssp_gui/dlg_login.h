#ifndef DLG_LOGIN_H
#define DLG_LOGIN_H

#include <QDialog>
#include "ui_dlg_login.h"
#include "ssp_gui.h"

class dlg_login : public QDialog
{
	Q_OBJECT

public:
	dlg_login(QWidget *parent = 0);
	~dlg_login();
public slots:
	void OnOk();
	void OnCancel();
public:
	SString m_sUser;
	bool m_bOk;
private:
	Ui::dlg_login ui;
};

#endif // DLG_LOGIN_H
