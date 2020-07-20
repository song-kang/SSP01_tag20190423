#ifndef DLGCONFIGURE_H
#define DLGCONFIGURE_H

#include <QDialog>
#include "ui_dlgconfigure.h"

class ssp_iaFloatView;
class DlgConfigure : public QDialog
{
	Q_OBJECT

public:
	DlgConfigure(QWidget *parent = 0);
	~DlgConfigure();

private:
	Ui::DlgConfigure ui;

	ssp_iaFloatView *ssp_floatwnd;

	QString m_iColorCritical;
	QString m_iColorImportant;
	QString m_iColorSecondary;
	QString m_iColorGeneral;

private:
	void Init();

private slots:
	void SlotBtnColorCriticalClicked();
	void SlotBtnColorImportantClicked();
	void SlotBtnColorSecondaryClicked();
	void SlotBtnColorGeneralClicked();
	void SlotBtnOkClicked();
	void SlotCheckboxNone(int state);
	void SlotCheckboxCritical(int state);
	void SlotCheckboxImportant(int state);
	void SlotCheckboxSecondary(int state);
	void SlotCheckboxGeneral(int state);

};

#endif // DLGCONFIGURE_H
