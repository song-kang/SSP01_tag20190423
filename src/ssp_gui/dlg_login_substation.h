#ifndef DLG_LOGIN_SUBSTATION_H
#define DLG_LOGIN_SUBSTATION_H

#include <QDialog>
#include "ui_dlg_login_substation.h"
#include "ssp_gui.h"

class dlg_login_substation : public QDialog
{
	Q_OBJECT

public:
	//forOper == true,操作员，false, 监护人
	dlg_login_substation(bool forOper, QWidget *parent = 0);
	~dlg_login_substation();

	void initLoginUser();
public slots:
		void OnOk();
		void OnCancel();
		void showErrorLabel(QString text);
public:
	SString m_sUser;
	bool m_bOk;
	bool for_oper;
private:
	Ui::dlg_login_substation ui;
};

#endif // DLG_LOGIN_BLUESTYLE_H
