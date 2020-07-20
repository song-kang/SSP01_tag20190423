#ifndef PANEL_SIM_NET_STORM_H
#define PANEL_SIM_NET_STORM_H

#include <QWidget>
#include "SQt.h"
#include "ui_panel_sim_storm.h"
#include "SSocket.h"
#include "ssp_baseview.h"
#include "sim_config.h"
#include "panel_sim_tcp_recv_handle.h"
#include "SRawSocket.h"
class panel_sim_net_storm : public CBaseView,public SService
{
	Q_OBJECT

public:
	panel_sim_net_storm(QWidget *parent);
	~panel_sim_net_storm();
	static void* ThreadSend(void *lp);
	virtual void OnRefresh();
	void setCurTreeItem(QTreeWidgetItem* curItem){m_pCurTreeItem = curItem;}
signals:
	void doExistThread();
private:
	void init();
	bool doCheckControlVaild();
	bool IsQuit(){return m_bThreadQuit;}
	QByteArray hexStringtoByteArray(QString hex);
	void formatString(QString &org, int n=2, const QChar &ch=QChar(' '));
	void getRandStr(char* pCH, int iLen);

	////////////////////////////////////////////////////////////////////////
	// 描    述:  启动服务,虚函数，派生函数中必须先调用此函数
	// 作    者:  邵凯田
	// 创建时间:  2016-01-07 09:57
	// 参数说明:  void
	// 返 回 值:  true表示服务启动成功,false表示服务启动失败
	//////////////////////////////////////////////////////////////////////////
	virtual bool Start();

	////////////////////////////////////////////////////////////////////////
	// 描    述:  停止服务，派生函数中必须先调用此函数
	// 作    者:  邵凯田
	// 创建时间:  2016-01-07 09:57
	// 参数说明:  void
	// 返 回 值:  true表示服务停止成功,false表示服务停止失败
	//////////////////////////////////////////////////////////////////////////
	virtual bool Stop();


private:
	Ui::panel_sim_storm ui;

	QTreeWidgetItem* m_pCurTreeItem;
	QTreeWidgetItem* m_pOldTreeItem;
	stuSimSubnetwork* m_pCurSubnetwork;
	bool  m_bThreadQuit;
	char  m_sendRandomBuf[TCP_CLIENT_SEND_BUF];
	bool  m_bConnectOK;
	int   m_iSendLen;
	int   m_iSendCount;
	int   m_iIntervalValue;
	char* m_pSendBuf;
	int   m_iSendBufSize;
	QString m_strSendCommonInfo;
	QByteArray m_byteSendHexInfo;
	SString m_sDevName;//发送网卡名
	SRawSocketSend m_Sender;
private slots:
	void OnSlotStart();
	void OnSlotStop();
	void OnExistThread();
};

#endif // PANEL_SIM_NET_STORM_H
