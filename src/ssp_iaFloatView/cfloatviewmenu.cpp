#include "cfloatviewmenu.h"
#include "ssp_iafloatview.h"
#include "cia2alarmpopwnd.h"
#include "dlgconfigure.h"

CFloatViewMenu::CFloatViewMenu(QWidget *parent)
	: QMenu(parent)
{
	ssp_floatwnd = (ssp_iaFloatView *)parent;

#ifdef WIN32
	setStyleSheet(
		"QMenu{ background-color:white; border:1px solid rgb(214,214,214); }"
		"QMenu::item{ height:28px; padding:0px 20px 0px 40px; font-size:12px; color:rgb(0, 0, 0); font-family: Microsoft Yahei; }"
		"QMenu::item:focus{ padding: -1; }"
		"QMenu::item:selected:enabled{ background:rgb(22, 154, 243); color:white; }"
		"QMenu::item:selected:!enabled{ color:transparent; }"
		"QMenu::icon{ position:absolute;left: 12px; }"
		"QMenu::separator{ height:1px; background:rgb(209,209,209); margin:4px 0px 4px 0px; }");
#else
	setStyleSheet(
		"QMenu{ background-color:white; border:1px solid rgb(214,214,214); }"
		"QMenu::item{ height:28px; padding:0px 20px 0px 40px; font-size:12px; color:rgb(0, 0, 0); font-family: 宋体; }"
		"QMenu::item:focus{ padding: -1; }"
		"QMenu::item:selected:enabled{ background:rgb(22, 154, 243); color:white; }"
		"QMenu::item:selected:!enabled{ color:transparent; }"
		"QMenu::icon{ position:absolute;left: 12px; }"
		"QMenu::separator{ height:1px; background:rgb(209,209,209); margin:4px 0px 4px 0px; }");
#endif
	
	 setContentsMargins(2,4,2,4);
	 Init();
}

CFloatViewMenu::~CFloatViewMenu()
{

}

void CFloatViewMenu::Init()
{

}

void CFloatViewMenu::CreateMenuItem()
{
	act_show = new QAction(QIcon(":/images/display"),tr("显示告警"),this);
	addAction(act_show);
	connect(act_show, SIGNAL(triggered()),this,SLOT(SlotShow()));

	addSeparator();

	act_push_no = new QAction(tr("不弹出告警"),this);
	if (ssp_floatwnd->ReadSettings("/pop/none").toInt() == 1)
		act_push_no->setIcon(QIcon(":/images/select"));
	addAction(act_push_no);
	connect(act_push_no, SIGNAL(triggered()),this,SLOT(SlotPushNo()));

	act_push_critical = new QAction(tr("弹出紧急告警"),this);
	if (ssp_floatwnd->ReadSettings("/pop/critical").toInt() == 1) 
		act_push_critical->setIcon(QIcon(":/images/select"));
	addAction(act_push_critical);
	connect(act_push_critical, SIGNAL(triggered()),this,SLOT(SlotPushCritical()));

	act_push_important = new QAction(tr("弹出重要告警"),this);
	if (ssp_floatwnd->ReadSettings("/pop/important").toInt() == 1) 
		act_push_important->setIcon(QIcon(":/images/select"));
	addAction(act_push_important);
	connect(act_push_important, SIGNAL(triggered()),this,SLOT(SlotPushImportant()));

	act_push_secondary = new QAction(tr("弹出次要告警"),this);
	if (ssp_floatwnd->ReadSettings("/pop/secondary").toInt() == 1) 
		act_push_secondary->setIcon(QIcon(":/images/select"));
	addAction(act_push_secondary);
	connect(act_push_secondary, SIGNAL(triggered()),this,SLOT(SlotPushSecondary()));

	act_push_general = new QAction(tr("弹出一般告警"),this);
	if (ssp_floatwnd->ReadSettings("/pop/general").toInt() == 1) 
		act_push_general->setIcon(QIcon(":/images/select"));
	addAction(act_push_general);
	connect(act_push_general, SIGNAL(triggered()),this,SLOT(SlotPushGeneral()));

	addSeparator();

	act_setting = new QAction(QIcon(":/images/setting"),tr("参数设置"),this);
	addAction(act_setting);
	connect(act_setting, SIGNAL(triggered()),this,SLOT(SlotSetting()));
}

void CFloatViewMenu::SlotShow()
{
	((CIa2AlarmPopWnd*)ssp_floatwnd->m_iAlarmWnd)->show();
	ssp_floatwnd->hide();
}

void CFloatViewMenu::SlotPushNo()
{
	ssp_floatwnd->WriteSettings("/pop/none","1");
	ssp_floatwnd->WriteSettings("/pop/critical","0");
	ssp_floatwnd->WriteSettings("/pop/important","0");
	ssp_floatwnd->WriteSettings("/pop/secondary","0");
	ssp_floatwnd->WriteSettings("/pop/general","0");
}

void CFloatViewMenu::SlotPushCritical()
{
	ssp_floatwnd->WriteSettings("/pop/none","0");
	ssp_floatwnd->WriteSettings("/pop/critical","1");
}

void CFloatViewMenu::SlotPushImportant()
{
	ssp_floatwnd->WriteSettings("/pop/none","0");
	ssp_floatwnd->WriteSettings("/pop/important","1");
}

void CFloatViewMenu::SlotPushSecondary()
{
	ssp_floatwnd->WriteSettings("/pop/none","0");
	ssp_floatwnd->WriteSettings("/pop/secondary","1");
}

void CFloatViewMenu::SlotPushGeneral()
{
	ssp_floatwnd->WriteSettings("/pop/none","0");
	ssp_floatwnd->WriteSettings("/pop/general","1");
}

void CFloatViewMenu::SlotSetting()
{
	DlgConfigure dlg(ssp_floatwnd);
	if (dlg.exec() == QDialog::Accepted)
	{
		((CIa2AlarmPopWnd*)ssp_floatwnd->m_iAlarmWnd)->OnRefreshTableWidget();
		((ssp_iaFloatView*)ssp_floatwnd)->RefreshFloatWnd();
	}
}

