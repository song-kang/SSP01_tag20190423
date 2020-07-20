#ifndef DLG_SET_VOLTAGE_CURRENT_H
#define DLG_SET_VOLTAGE_CURRENT_H

#include <QDialog>
#include "ui_dlg_set_voltage_current.h"
class dlg_set_voltage_current : public QDialog
{
	Q_OBJECT

public:
	dlg_set_voltage_current(QWidget *parent);
	~dlg_set_voltage_current();
	QString getPrimaryVoltage() {return m_strPrimaryVoltage;}
	void  setPrimaryVoltage(QString& str) {m_strPrimaryVoltage = str;}

	QString getSecondaryVoltage() {return m_strSecondaryVoltage;}
	void  setSecondaryVoltage(QString& str) {m_strSecondaryVoltage = str;}

	QString getPrimaryCurrent() {return m_strPrimaryCurrent;}
	void  setPrimaryCurrent(QString& str) {m_strPrimaryCurrent = str;}

	QString getSecondaryCurrent() {return m_strSecondaryCurrent;}
	void  setSecondaryCurrent(QString& str) {m_strSecondaryCurrent = str;}
private:
	void initDlg();
private:
	Ui::set_voltage_current_dlg ui;
	QString m_strPrimaryVoltage;
	QString m_strSecondaryVoltage;
	QString m_strPrimaryCurrent;
	QString m_strSecondaryCurrent;

private slots:
	void slotOkBtn();
};

#endif // DLG_SET_VOLTAGE_CURRENT_H
