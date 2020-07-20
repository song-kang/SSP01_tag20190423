#ifndef DLGMESSAGE_H
#define DLGMESSAGE_H

#include <QDialog>
#include "ui_dlgmessage.h"

enum viewType {
	VIEW_CRITICAL = 1,
	VIEW_WARN,
	VIEW_INFORMATION,
	VIEW_QUESTION,
};

class DlgMessage : public QDialog
{
	Q_OBJECT

public:
	DlgMessage(QWidget *parent = 0);
	~DlgMessage();

	void SetMessage(int viewType,QString text);

private:
	Ui::DlgMessage ui;

private:
	void Init();

private slots:
	void SlotBtnOkClicked();

};

#endif // DLGMESSAGE_H
