#ifndef DLG_OPERLOGIN_H
#define DLG_OPERLOGIN_H

#include <QDialog>
#include "ui_dlg_operlogin.h"
#include "ssp_gui.h"
class dlg_operlogin : public QDialog
{
	Q_OBJECT

public:
	dlg_operlogin(bool bWithMonitor,SString sTip,QWidget *parent = 0);
	~dlg_operlogin();

public slots:
	void OnOk();
	void OnCancel();
public:
	bool m_bWithMonitor;
	SString m_sUser;
	SString m_sMonUser;
	bool m_bOk;
private:
	Ui::dlg_operlogin ui;
};

#endif // DLG_OPERLOGIN_H
