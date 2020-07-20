#ifndef PANEL_SIM_UDP_H
#define PANEL_SIM_UDP_H

#include <QWidget>
#include "SQt.h"
#include "ui_panel_sim_udp.h"
#include "SSocket.h"
#include "ssp_baseview.h"
#include "panel_sim_tcp_recv_handle.h"

struct stUdpMsgHead
{
	int	 iLen;
	//int  iType;
	char buf[TCP_CLIENT_RECV_BUF];
};
class panel_sim_udp : public CBaseView
{
	Q_OBJECT

public:
	panel_sim_udp(QWidget *parent);
	~panel_sim_udp();
signals:
	void doExitRecvThread();
private:
	bool IsQuit(){return m_bQuit;}
	QByteArray hexStringtoByteArray(QString hex);
	void formatString(QString &org, int n=2, const QChar &ch=QChar(' '));
	void getRandStr(char* pCH, int iLen);

	bool BindPort();
	void init();
	void initTable();
	void doUnconnect();
	void addRow(QString& strText);
	void addRow(char* pBuf, int len,struct sockaddr_in& remote_addr);
	bool doCheckControlVaild();
	bool CreateUdpSocket();
	void setSocketRecvSwitchState(bool bState);
	bool doRepaly(struct sockaddr_in &remote_addr);
	static void* ThreadUdpRecv(void *lp);
	static void* ThreadUdpSend(void *lp);
private:
	Ui::panel_sim_udp ui;
	SSocket m_udpsocket;
	bool  m_bQuit;
	int   m_iSendLen;
	int   m_iSendCount;
	int   m_iIntervalValue;
	//发送内容内存有关对象
	char* m_chSendBuf;
	int   m_iSendBufSize;
	QString m_strSendCommonInfo;
	QByteArray m_byteSendHexInfo;
	char  m_chSendRandInfo[TCP_CLIENT_SEND_BUF];
	QString m_strDestIp;
	QString m_strPort;
	stUdpMsgHead m_stUdpMsgHead;
	bool    m_bIsSocketStart;
private slots:
	void OnSlotStart();
	void OnSlotSend();
	void OnSlotStop();
	void OnSlotRandRadioButton();
	void OnSlotHexRadioButton();
	void OnSlotCommonRadioButton();
	void OnSlotStringShowRadioButton();
	void OnSlotHexShowRadioButton();
	void OnSlotExitRecvThread();
};

#endif // PANEL_SIM_UDP_H
