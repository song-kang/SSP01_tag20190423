#include "dlgconfigure.h"
#include "ssp_iafloatview.h"
#include "dlgmessage.h"

DlgConfigure::DlgConfigure(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	ssp_floatwnd = (ssp_iaFloatView *)parent;

	//setWindowFlags(this->windowFlags() | Qt::WindowStaysOnTopHint | Qt::Tool);

	int curMonitor = QApplication::desktop()->screenNumber(this);
	QRect rect = QApplication::desktop()->availableGeometry(curMonitor);
	move((rect.width()-this->width())/2,(rect.height()-this->height())/2);

	Init();

	connect(ui.btn_color_critical,SIGNAL(clicked()),this,SLOT(SlotBtnColorCriticalClicked()));
	connect(ui.btn_color_important,SIGNAL(clicked()),this,SLOT(SlotBtnColorImportantClicked()));
	connect(ui.btn_color_secondary,SIGNAL(clicked()),this,SLOT(SlotBtnColorSecondaryClicked()));
	connect(ui.btn_color_general,SIGNAL(clicked()),this,SLOT(SlotBtnColorGeneralClicked()));
	connect(ui.btn_ok,SIGNAL(clicked()),this,SLOT(SlotBtnOkClicked()));
	connect(ui.btn_cancel,SIGNAL(clicked()),this,SLOT(close()));

	connect(ui.checkBox_none,SIGNAL(stateChanged(int)),this,SLOT(SlotCheckboxNone(int)));
	connect(ui.checkBox_critical,SIGNAL(stateChanged(int)),this,SLOT(SlotCheckboxCritical(int)));
	connect(ui.checkBox_important,SIGNAL(stateChanged(int)),this,SLOT(SlotCheckboxImportant(int)));
	connect(ui.checkBox_secondary,SIGNAL(stateChanged(int)),this,SLOT(SlotCheckboxSecondary(int)));
	connect(ui.checkBox_general,SIGNAL(stateChanged(int)),this,SLOT(SlotCheckboxGeneral(int)));
}

DlgConfigure::~DlgConfigure()
{

}

void DlgConfigure::Init()
{
	if (ssp_floatwnd->ReadSettings("/pop/none").toInt())
		ui.checkBox_none->setCheckState(Qt::Checked);
	else
		ui.checkBox_none->setCheckState(Qt::Unchecked);
	if (ssp_floatwnd->ReadSettings("/pop/critical").toInt())
		ui.checkBox_critical->setCheckState(Qt::Checked);
	else
		ui.checkBox_critical->setCheckState(Qt::Unchecked);
	if (ssp_floatwnd->ReadSettings("/pop/important").toInt())
		ui.checkBox_important->setCheckState(Qt::Checked);
	else
		ui.checkBox_important->setCheckState(Qt::Unchecked);
	if (ssp_floatwnd->ReadSettings("/pop/secondary").toInt())
		ui.checkBox_secondary->setCheckState(Qt::Checked);
	else
		ui.checkBox_secondary->setCheckState(Qt::Unchecked);
	if (ssp_floatwnd->ReadSettings("/pop/general").toInt())
		ui.checkBox_general->setCheckState(Qt::Checked);
	else
		ui.checkBox_general->setCheckState(Qt::Unchecked);

	ui.btn_color_critical->setStyleSheet(tr("QPushButton{border-style:solid;border-radius:2px;background:%2;border-width:1;border-color:#000000;}")
		.arg(m_iColorCritical = ssp_floatwnd->ReadSettings("/color/critical")));
	ui.btn_color_important->setStyleSheet(tr("QPushButton{border-style:solid;border-radius:2px;background:%2;border-width:1;border-color:#000000;}")
		.arg(m_iColorImportant = ssp_floatwnd->ReadSettings("/color/important")));
	ui.btn_color_secondary->setStyleSheet(tr("QPushButton{border-style:solid;border-radius:2px;background:%2;border-width:1;border-color:#000000;}")
		.arg(m_iColorSecondary = ssp_floatwnd->ReadSettings("/color/secondary")));
	ui.btn_color_general->setStyleSheet(tr("QPushButton{border-style:solid;border-radius:2px;background:%2;border-width:1;border-color:#000000;}")
		.arg(m_iColorGeneral = ssp_floatwnd->ReadSettings("/color/general")));

	ui.spinBox_restore->setRange(10,36000);
	ui.spinBox_restore->setSingleStep(10);
	ui.spinBox_restore->setSuffix(" 秒");
	ui.spinBox_restore->setFixedWidth(100);
	ui.spinBox_restore->setValue(ssp_floatwnd->ReadSettings("/count/restoretime").toInt());
}

void DlgConfigure::SlotBtnColorCriticalClicked()
{
	QColor color = QColorDialog::getColor(m_iColorCritical,this,tr("选择颜色"));
	if (color.isValid())
	{
		m_iColorCritical = color.name();
		ui.btn_color_critical->setStyleSheet(
			tr("QPushButton{border-style:solid;border-radius:2px;background:%2;border-width:1;border-color:#000000;}").arg(m_iColorCritical));
	}
}

void DlgConfigure::SlotBtnColorImportantClicked()
{
	QColor color = QColorDialog::getColor(m_iColorImportant,this,tr("选择颜色"));
	if (color.isValid())
	{
		m_iColorImportant = color.name();
		ui.btn_color_important->setStyleSheet(
			tr("QPushButton{border-style:solid;border-radius:2px;background:%2;border-width:1;border-color:#000000;}").arg(m_iColorImportant));
	}
}

void DlgConfigure::SlotBtnColorSecondaryClicked()
{
	QColor color = QColorDialog::getColor(m_iColorSecondary,this,tr("选择颜色"));
	if (color.isValid())
	{
		m_iColorSecondary = color.name();
		ui.btn_color_secondary->setStyleSheet(
			tr("QPushButton{border-style:solid;border-radius:2px;background:%2;border-width:1;border-color:#000000;}").arg(m_iColorSecondary));
	}
}

void DlgConfigure::SlotBtnColorGeneralClicked()
{
	QColor color = QColorDialog::getColor(m_iColorGeneral,this,tr("选择颜色"));
	if (color.isValid())
	{
		m_iColorGeneral = color.name();
		ui.btn_color_general->setStyleSheet(
			tr("QPushButton{border-style:solid;border-radius:2px;background:%2;border-width:1;border-color:#000000;}").arg(m_iColorGeneral));
	}
}

void DlgConfigure::SlotBtnOkClicked()
{
	if (ui.checkBox_none->checkState() == Qt::Unchecked && 
		ui.checkBox_critical->checkState() == Qt::Unchecked && 
		ui.checkBox_important->checkState() == Qt::Unchecked && 
		ui.checkBox_secondary->checkState() == Qt::Unchecked && 
		ui.checkBox_general->checkState() == Qt::Unchecked)
	{
		DlgMessage dlg(this);
		dlg.SetMessage(VIEW_WARN,"没有完成告警窗自动弹出设置。\n\n请选择其中一个或多个选项！");
		dlg.exec();
		return;
	}

	if (ui.checkBox_none->checkState() == Qt::Checked)
	{
		ssp_floatwnd->WriteSettings("/pop/none","1");
		ssp_floatwnd->WriteSettings("/pop/critical","0");
		ssp_floatwnd->WriteSettings("/pop/important","0");
		ssp_floatwnd->WriteSettings("/pop/secondary","0");
		ssp_floatwnd->WriteSettings("/pop/general","0");
	}
	else
	{
		ssp_floatwnd->WriteSettings("/pop/none","0");
		if (ui.checkBox_critical->checkState() == Qt::Checked)
			ssp_floatwnd->WriteSettings("/pop/critical","1");
		else
			ssp_floatwnd->WriteSettings("/pop/critical","0");

		if (ui.checkBox_important->checkState() == Qt::Checked)
			ssp_floatwnd->WriteSettings("/pop/important","1");
		else
			ssp_floatwnd->WriteSettings("/pop/important","0");

		if (ui.checkBox_secondary->checkState() == Qt::Checked)
			ssp_floatwnd->WriteSettings("/pop/secondary","1");
		else
			ssp_floatwnd->WriteSettings("/pop/secondary","0");

		if (ui.checkBox_general->checkState() == Qt::Checked)
			ssp_floatwnd->WriteSettings("/pop/general","1");
		else
			ssp_floatwnd->WriteSettings("/pop/general","0");
	}

	ssp_floatwnd->WriteSettings("/color/critical",m_iColorCritical);
	ssp_floatwnd->WriteSettings("/color/important",m_iColorImportant);
	ssp_floatwnd->WriteSettings("/color/secondary",m_iColorSecondary);
	ssp_floatwnd->WriteSettings("/color/general",m_iColorGeneral);
	ssp_floatwnd->WriteSettings("/count/restoretime",tr("%1").arg(ui.spinBox_restore->value()));

	SString sql;
	SRecordset rs;
	sql.sprintf("update t_ia2_rule_severity set backcolor='%s' where rule_severity=4",m_iColorCritical.toStdString().data());
	DB->Execute(sql);
	sql.sprintf("update t_ia2_rule_severity set backcolor='%s' where rule_severity=3",m_iColorImportant.toStdString().data());
	DB->Execute(sql);
	sql.sprintf("update t_ia2_rule_severity set backcolor='%s' where rule_severity=2",m_iColorSecondary.toStdString().data());
	DB->Execute(sql);
	sql.sprintf("update t_ia2_rule_severity set backcolor='%s' where rule_severity=1",m_iColorGeneral.toStdString().data());
	DB->Execute(sql);

	accept();
}

void DlgConfigure::SlotCheckboxNone(int state)
{
	if (state == Qt::Checked)
	{
		ui.checkBox_critical->setCheckState(Qt::Unchecked);
		ui.checkBox_important->setCheckState(Qt::Unchecked);
		ui.checkBox_secondary->setCheckState(Qt::Unchecked);
		ui.checkBox_general->setCheckState(Qt::Unchecked);
	}
}

void DlgConfigure::SlotCheckboxCritical(int state)
{
	if (state == Qt::Checked)
		ui.checkBox_none->setCheckState(Qt::Unchecked);
}

void DlgConfigure::SlotCheckboxImportant(int state)
{
	if (state == Qt::Checked)
		ui.checkBox_none->setCheckState(Qt::Unchecked);
}

void DlgConfigure::SlotCheckboxSecondary(int state)
{
	if (state == Qt::Checked)
		ui.checkBox_none->setCheckState(Qt::Unchecked);
}

void DlgConfigure::SlotCheckboxGeneral(int state)
{
	if (state == Qt::Checked)
		ui.checkBox_none->setCheckState(Qt::Unchecked);
}
