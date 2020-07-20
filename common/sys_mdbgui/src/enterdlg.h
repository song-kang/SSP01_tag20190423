#ifndef ENTERDLG_H
#define ENTERDLG_H

#include <QWidget>
#include <QDialog>
#include "ui_enterdlg.h"
#include "ManConnect.h"
class enterDlg : public QDialog
{
	Q_OBJECT

public:
	enterDlg(QWidget *parent = 0);
	~enterDlg();
	int exec();

private:
	bool event(QEvent *e);
	Ui::enterDlg ui;
	SString ip;
	int port;
	SString sParam;
	int i_ret;
private slots:
	void on_pushButton_ok_pressed();
	void on_pushButton_cancel_pressed();
signals:
	void ok_pressed(SString sParam);
};

#endif // ENTERDLG_H
