#ifndef PANEL_SIM_TCP_SERVER_H
#define PANEL_SIM_TCP_SERVER_H

#include <QWidget>
#include "SQt.h"
#include "ui_panel_sim_tcp_server.h"
#include "SSocket.h"
#include "ssp_baseview.h"
#include "panel_sim_tcp_recv_handle.h"

class panel_sim_tcp_server : public CBaseView
{
	Q_OBJECT

public:
	panel_sim_tcp_server(QWidget *parent);
	~panel_sim_tcp_server();
private:
	void init();
	bool createServerSocket();
	static void* ThreadTcpListen(void *lp);
	static void* ThreadHandleClientRecv(void *lp);
	static void* ThreadSocketRecvService(void *lp);
signals:
	void doAddHandle(SSocket* pCur);
private:
	Ui::panel_sim_tcp_server ui;
	SSocket m_socket;
	//SPtrList<SSocket> m_clientSocketList;
	SPtrList<panel_sim_tcp_recv_handle> m_recvPanelList;
	QString m_strIP;
	QString m_strPort;
	//bool m_bExitHandleClientRecvThread;
	bool m_bExitTcpListenThread;
	
private slots:
	void OnSlotStart();
	void OnSlotStop();
	void OnSlotSocketClose(QString strText);
	void OnSlotADD(SSocket* pCur);
};

#endif // PANEL_SIM_TCP_SERVER_H
