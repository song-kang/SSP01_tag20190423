#include "dlg_set_voltage_current.h"
#include <QMessageBox>
dlg_set_voltage_current::dlg_set_voltage_current(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	Qt::WindowFlags flags=Qt::Dialog;
	flags |=Qt::WindowCloseButtonHint;
	setWindowFlags(flags);
	initDlg();
	connect(ui.OKPushButton,SIGNAL(clicked()),this,SLOT(slotOkBtn()));
	connect(ui.canelPushButton,SIGNAL(clicked()),this,SLOT(close()));
}

dlg_set_voltage_current::~dlg_set_voltage_current()
{

}

void dlg_set_voltage_current::initDlg()
{
	//QLineEdit * lineEdit = new QLineEdit();
	bool bOk;
	float fMin = 0;//pCurParaInfo->strMinValue.toFloat(&bOk);
	float fMax = 99999999;//pCurParaInfo->strMaxValue.toFloat(&bOk);
	QDoubleValidator *validatorPortPrimaryV=new QDoubleValidator(fMin,fMax,3, ui.primaryVoltageLineEdit);
	validatorPortPrimaryV->setNotation(QDoubleValidator::StandardNotation);
	ui.primaryVoltageLineEdit->setValidator(validatorPortPrimaryV);

	QDoubleValidator *validatorPortSecondaryV=new QDoubleValidator(fMin,fMax,3, ui.secondaryVoltageLineEdit);
	validatorPortSecondaryV->setNotation(QDoubleValidator::StandardNotation);
	ui.secondaryVoltageLineEdit->setValidator(validatorPortSecondaryV);

	QDoubleValidator *validatorPortPrimaryC=new QDoubleValidator(fMin,fMax,3, ui.PrimaryCurrentLineEdit);
	validatorPortPrimaryC->setNotation(QDoubleValidator::StandardNotation);
	ui.PrimaryCurrentLineEdit->setValidator(validatorPortPrimaryC);

	QDoubleValidator *validatorPortSeondaryC=new QDoubleValidator(fMin,fMax,3, ui.secondaryCurrentLineEdit);
	validatorPortSeondaryC->setNotation(QDoubleValidator::StandardNotation);
	ui.secondaryCurrentLineEdit->setValidator(validatorPortSeondaryC);
}

void dlg_set_voltage_current::slotOkBtn()
{
	if (ui.primaryVoltageLineEdit->text().isEmpty() ||
		ui.secondaryVoltageLineEdit->text().isEmpty() ||
		ui.PrimaryCurrentLineEdit->text().isEmpty() ||
		ui.secondaryCurrentLineEdit->text().isEmpty())
	{
		QMessageBox::warning(this,"警告","额定电压电流不能为空，请设置！");
		return;
	}
	m_strPrimaryVoltage = ui.primaryVoltageLineEdit->text();
	m_strSecondaryVoltage = ui.secondaryVoltageLineEdit->text();
	m_strPrimaryCurrent = ui.PrimaryCurrentLineEdit->text();
	m_strSecondaryCurrent = ui.secondaryCurrentLineEdit->text();
	accept();
	return;
}