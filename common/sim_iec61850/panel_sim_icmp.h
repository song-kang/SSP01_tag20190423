#ifndef PANEL_SIM_ICMP_H
#define PANEL_SIM_ICMP_H

#include <QWidget>
#include "ui_panel_sim_icmp.h"
//#include "sim_manager.h"
#include "ssp_baseview.h"
#include "sim_config.h"
#include "SQt.h"
#include "sim_manager.h"
#include "SICMP.h"
class panel_sim_icmp : public CBaseView
{
	Q_OBJECT

public:
	panel_sim_icmp(QWidget *parent);
	~panel_sim_icmp();
	QString getDescByTypeAndCode(int iType, int iCode=0);
signals:
	void doExitSendThread();
private:
	void init();
	int countFlowValue();
	bool checkControlValid();
	static void* ThreadSendRequest(void *lp);
private:
	Ui::panel_sim_icmp ui;
	SICMP m_sIcmp;

	//SSocket m_curSocket;
	//char  m_chSendBuf[1024];
	int   m_iSendLen;
	int m_iSendCount;
	int   m_iIntervalValue;
	QString m_strDestIp;
	bool  m_bSendTreadExit;
private slots:
	void OnSlotSend();
	void OnSlotStop();
	void OnDoExitSendThread();
};

#endif // PANEL_SIM_ICMP_H
