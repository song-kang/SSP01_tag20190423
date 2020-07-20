#ifndef PANEL_SIM_TCP_CLIENT_H
#define PANEL_SIM_TCP_CLIENT_H

#include <QWidget>
#include "SQt.h"
#include "ui_panel_sim_tcp_client.h"
#include "SSocket.h"
#include "ssp_baseview.h"
#include "panel_sim_tcp_recv_handle.h"

class panel_sim_tcp_client : public CBaseView
{
	Q_OBJECT

public:
	panel_sim_tcp_client(QWidget *parent);
	~panel_sim_tcp_client();


	//////////////////////////////////////////////////////////////////////////
	// 描    述:  模拟线程，处理recv事件
	// 作    者:  
	// 创建时间:  2016-12-22 14:39
	// 参数说明:  @lp
	// 返 回 值:  NULL
	//////////////////////////////////////////////////////////////////////////
	static void* ThreadTcpRecv(void *lp);
	static void* ThreadTcpSend(void *lp);
private:
	bool IsQuit(){return m_bQuit;}
	QByteArray hexStringtoByteArray(QString hex);
	void formatString(QString &org, int n=2, const QChar &ch=QChar(' '));
	void getRandStr(char* pCH, int iLen);
	bool doSend(char* pSendBuf, int len,int iType);
signals:
	void doSocketUnconnect();
private:
	void init();
	void initTable();
	void doUnconnect();
	void addRow(QString& strText);
	void addRow(char* pBuf, int len);
	bool doCheckControlVaild();
private:
	Ui::panel_sim_tcp_client ui;
	SSocket m_socket;
	bool  m_bQuit;
	char  m_sendRandomBuf[TCP_CLIENT_SEND_BUF];
	bool  m_bConnectOK;
	int   m_iSendLen;
	int   m_iSendCount;
	int   m_iIntervalValue;
	char* m_pSendBuf;
	int   m_iSendBufSize;
	stTCPMsgHead m_stSendMsgHead;
	QString m_strSendCommonInfo;
	QByteArray m_byteSendHexInfo;
private slots:
	void OnSlotConnect();
	void OnSlotSend();
	void OnSlotUnconnect();
	void OnSlotRandRadioButton();
	void OnSlotHexRadioButton();
	void OnSlotCommonRadioButton();
	void OnSlotStringShowRadioButton();
	void OnSlotHexShowRadioButton();
};

#endif // PANEL_SIM_TCP_CLIENT_H
