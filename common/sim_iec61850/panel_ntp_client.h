#ifndef PANEL_NTP_CLIENT_H
#define PANEL_NTP_CLIENT_H

#include <QWidget>
#include "SQt.h"
#include "ui_panel_sim_ntp_client.h"
#include "SNtp.h"
#include "ssp_baseview.h"
#define NTP_SERVER 0
#define NTP_CLIENT 1
class panel_ntp_client : public CBaseView
{
	Q_OBJECT

public:
	panel_ntp_client(QWidget *parent, int iType);
	~panel_ntp_client();
	//void setIpAndPort(QString& strIp,QString& strPort){ m_strCurSetIp = strIp; m_strCurSetPort = strPort;}
	void setNtpType(int iType) {m_iNtpType = iType;}
	virtual void OnRefresh();
private:
	void refreshInfo();
	void init();
private:
	Ui::panel_ntp_client ui;
	SNtpClient m_ntpClent;
	SNtpServer m_ntpServer;
	QString m_strCurSetServerIp;
	QString m_strCurSetServerPort;
	QString m_strCurSetClientIp;
	QString m_strCurSetClientPort;
	int m_iNtpType;
private slots:
	void OnSlotTimeSyn();
	void OnSlotStartServer();
	void OnSlotStopServer();

};

#endif // PANEL_NTP_CLIENT_H
