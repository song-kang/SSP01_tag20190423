#ifndef CFLOATVIEWMENU_H
#define CFLOATVIEWMENU_H

#include <QMenu>

class ssp_iaFloatView;
class CFloatViewMenu : public QMenu
{
	Q_OBJECT

public:
	CFloatViewMenu(QWidget *parent = 0);
	~CFloatViewMenu();

	void CreateMenuItem();

private:
	ssp_iaFloatView *ssp_floatwnd;

	QAction *act_show;
	QAction *act_push_no;
	QAction *act_push_critical;
	QAction *act_push_important;
	QAction *act_push_secondary;
	QAction *act_push_general;
	QAction *act_setting;

private:
	void Init();

private slots:
	void SlotShow();
	void SlotPushNo();
	void SlotPushCritical();
	void SlotPushImportant();
	void SlotPushSecondary();
	void SlotPushGeneral();
	void SlotSetting();

};

#endif // CFLOATVIEWMENU_H
