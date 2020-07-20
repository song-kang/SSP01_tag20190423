#ifndef PANEL_SIM_TCP_RECV_HANDLE_H
#define PANEL_SIM_TCP_RECV_HANDLE_H

#include <QWidget>
#include "ui_panel_sim_tcp_recv_sock.h"
#include "SQt.h"
#include "SSocket.h"
#include "ssp_baseview.h"
#define  MSG_TYPE_HEX 0
#define  MSG_TYPE_STR 1
#define TCP_CLIENT_RECV_BUF 1460
#define TCP_CLIENT_SEND_BUF 1460
struct stTCPMsgHead
{
	int	 iLen;//消息内容长度
	//int  iType;//消息类型：0表示随机字符串，1表示16进制，2表示自定义字符串
};

class panel_sim_tcp_server;
class panel_sim_tcp_recv_handle : public QWidget
{
	Q_OBJECT

public:
	panel_sim_tcp_recv_handle(QWidget *parent=NULL);
	panel_sim_tcp_recv_handle(panel_sim_tcp_server* pTcpServer,SSocket* pCurSocket,QWidget *parent=NULL);
	~panel_sim_tcp_recv_handle();

	void CloseSocket();
signals:
	void doSocketCloseHandle(QString strText);
private:
	void init();
	void initTable();
	void addRow(QString& strText);
	void addRow(char* pBuf, int len);
	static void* ThreadTcpRecv(void *lp);
	static void* ThreadTcpSend(void *lp);
	bool IsQuit(){return m_bQuit;}
	QByteArray hexStringtoByteArray(QString hex);
	void formatString(QString &org, int n=2, const QChar &ch=QChar(' '));
	void getRandStr(char* pCH, int iLen);
	static void* ThreadRecv(void *lp);
	bool doRepaly();
	bool doCheckControlVaild();
	bool doSend(char* pSendBuf, int len,int iType);

private:
	Ui::panel_sim_tcp_recv ui;
	panel_sim_tcp_server* m_pParentPanel;
	SSocket* m_pCurSocket;
	QString m_strCurSocketIp;
	//char*  m_sendRand;
	bool  m_bConnectOK;
	bool  m_bQuit;
	int   m_iSendLen;
	int   m_iSendCount;
	int   m_iIntervalValue;

	char*   m_pSendBuf;
	int m_iSendBufSize;
	char  m_sendRandBuf[TCP_CLIENT_SEND_BUF];
	
	stTCPMsgHead m_stSendMsgHead;
	QString m_strSendCommonInfo;
	QByteArray m_byteSendHexInfo;
private slots:
	void OnSlotUnconnect();
	void OnSlotSend();
	void OnSlotHexShowRadioButton();
	void OnSlotStringShowRadioButton();

	void OnSlotRandRadioButton();
	void OnSlotHexRadioButton();
	void OnSlotCommonRadioButton();
};

#endif // PANEL_SIM_TCP_RECV_HANDLE_H
