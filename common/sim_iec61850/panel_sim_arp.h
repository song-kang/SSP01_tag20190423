#ifndef PANEL_SIM_ARP_H
#define PANEL_SIM_ARP_H

#include <QWidget>
#include "SQt.h"
#include "ui_panel_sim_arp.h"
#include "SSocket.h"
#include "ssp_baseview.h"
#include "sim_config.h"
#include "ArpManager.h"

struct stAddrListInSelNetCard
{
	unsigned long ip; //IPµØÖ· 
	unsigned long netmask; //×ÓÍøÑÚÂë 
};
class panel_sim_arp : public CBaseView
{
	Q_OBJECT

public:
	panel_sim_arp(QWidget *parent);
	~panel_sim_arp();
	virtual void OnRefresh();
	void setCurTreeItem(QTreeWidgetItem* curItem){m_pCurTreeItem = curItem;}
signals:
	void doExitSendThread();
private:
	void initControl();
	bool checkControlValid();
	int countFlowValue();
	void setButtonState(bool bFlag);
	//void initTable();
	//void addRow(QString strIp, QString strMac);
	bool openNetCard();
	bool closeNetCard();
	bool initArpPacket(unsigned char* source_mac,unsigned long& srcIP,unsigned long& destIP);
	bool getCurNetcardIpList();
	//bool checkIPIsExist(QString strIp);
	static void* ThreadSendArpRequest(void *lp);
	static void* ThreadWaitForArpRepeatPacket(void *lp);
private:
	Ui::panel_sim_arp ui;
	QTreeWidgetItem* m_pCurTreeItem;
	QTreeWidgetItem* m_pOldTreeItem;
	stuSimSubnetwork* m_pCurSubnetwork;
private:
	pcap_t *m_adhandle;
	arp_packet m_curArpPacket;
	bool  m_bSendTreadExit;
	bool  m_bRecvTreadExit;
	SPtrList<stAddrListInSelNetCard> m_addrList;
	char  m_chSendBuf[1024];
	int   m_iSendLen;
	int   m_iSendCount;
	int   m_iIntervalValue;
	unsigned char  m_chCurMac[6];
	QString m_strLocalIp;
	QString m_strDestStartIp;
	//QString m_strDestEndIp;
private slots:
	void OnSlotStart();
	void OnSlotStop();
	void OnSlotSetting();
	void OnDoExitSendThread();
};

#endif // PANEL_SIM_ARP_H
