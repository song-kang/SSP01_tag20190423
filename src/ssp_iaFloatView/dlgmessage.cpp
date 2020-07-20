#include "dlgmessage.h"

DlgMessage::DlgMessage(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	//setWindowFlags(Qt::WindowStaysOnTopHint | Qt::Tool);

	Init();

	connect(ui.btn_ok,SIGNAL(clicked()),this,SLOT(SlotBtnOkClicked()));
	connect(ui.btn_cancel,SIGNAL(clicked()),this,SLOT(close()));
}

DlgMessage::~DlgMessage()
{

}

void DlgMessage::Init()
{
	ui.label_critical->setVisible(false);
	ui.label_warn->setVisible(false);
	ui.label_information->setVisible(false);
	ui.label_question->setVisible(false);
	ui.widget->setVisible(false);
	ui.plainTextEdit->setReadOnly(true);
}

void DlgMessage::SetMessage(int type,QString text)
{
	switch (type)
	{
	case VIEW_CRITICAL:
		ui.label_critical->setVisible(true);
		break;
	case VIEW_WARN:
		ui.label_warn->setVisible(true);
		break;
	case VIEW_INFORMATION:
		ui.label_information->setVisible(true);
		break;
	case VIEW_QUESTION:
		ui.label_question->setVisible(true);
		ui.widget->setVisible(true);
		break;
	default:
		ui.label_information->setVisible(true);
		break;
	}

	ui.plainTextEdit->setPlainText(text);
}

void DlgMessage::SlotBtnOkClicked()
{
	accept();
}
