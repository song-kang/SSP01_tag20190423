/**
 *
 * 文 件 名 : view_home.h
 * 创建日期 : 2015-12-22 16:05
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : 
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2015-12-22	邵凯田　创建文件
 *
 **/
#ifndef VIEW_HOME_H
#define VIEW_HOME_H

#include <QWidget>
#include "ui_view_home.h"
#include "ssp_baseview.h"
#include "sim_config.h"
#include "panel_sim_root.h"
#include "panel_sim_mmssvr.h"
#include "panel_sim_param_set.h"
#include "SimMmsServer.h"
#include "panel_sim_modify_dataset.h"
#include "panel_ntp_client.h"
#include "panel_sim_icmp.h"
#include "panel_sim_tcp_client.h"
#include "panel_sim_tcp_server.h"
#include "panel_sim_arp.h"
#include "panel_sim_udp.h"
#include "panel_sim_net_storm.h"
#include "SimClient.h"
#include "ConfigFileMgr.h"

enum eSimTreeNodeType
{
	SIM_TREE_HOME=1,
	SIM_TREE_SUBNET,
	SIM_TREE_SMV,
	SIM_TREE_SMV_ITEM,
	SIM_TREE_GOOSE,
	SIM_TREE_GOOSE_ITEM,
	SIM_TREE_MMSSVR,
	SIM_TREE_MMSSVR_ITEM,
	SIM_TREE_MMSCLIENT,
	SIM_TREE_MMSCLIENT_ITEM,
	SIM_TREE_NTPSVR,
	SIM_TREE_NTPSVR_ITEM,
	SIM_TREE_NTPCLIENT,
	SIM_TREE_NTPCLIENT_ITEM,
	SIM_TREE_TCPSVR,
	SIM_TREE_TCPSVR_ITEM,
	SIM_TREE_TCPCLIENT,
	SIM_TREE_TCPCLIENT_ITEM,
	SIM_TREE_UDPSEND,
	SIM_TREE_UDPSEND_ITEM,
	SIM_TREE_ICMPSEND,
	SIM_TREE_ICMPSEND_ITEM,
	SIM_TREE_ARPSEND,
	SIM_TREE_ARPSEND_ITEM,
	SIM_TREE_STORM,
	SIM_TREE_STORM_ITEM,
	SIM_TREE_MMSSVR_ITEM_DATASET,
};

class view_home : public CBaseView
{
	Q_OBJECT

public:
	view_home(QWidget *parent = 0);
	virtual ~view_home();
	virtual void OnPreShow();
	void Load();
	void LoadSubNet(stuSimSubnetwork *pSubNet,QTreeWidgetItem *pNetItem);
	void LoadSmvItem(stuSclVtIedSmvOut *pSmv,QTreeWidgetItem *pParent);
	void LoadGooseItem(stuSclVtIedGooseOut *pGoose,QTreeWidgetItem *pParent);
	void LoadMmsSvrItem(stuSimMmsSvr *pMmsSvr,QTreeWidgetItem *pParent);
	void LoadMmsClientItem(stuSimMmsClient *pMmsClient,QTreeWidgetItem *pParent);
	void LoadNtpSvrItem(stuSimNtpSvr *pNtpSvr,QTreeWidgetItem *pParent);
	void LoadNtpClientItem(stuSimNtpClient *pNtpClient,QTreeWidgetItem *pParent);
	void LoadTcpSvrItem(stuSimTcpSvr *pTcpSvr,QTreeWidgetItem *pParent);
	void LoadTcpClientItem(stuSimTcpClient *pTcpClient,QTreeWidgetItem *pParent);
	void LoadUdpSendItem(stuSimUdpSend *pUdpSend,QTreeWidgetItem *pParent);
	void LoadIcmpSendItem(stuSimIcmpSend *pIcmpSend,QTreeWidgetItem *pParent);
	void LoadArpSendItem(stuSimArpSend *pArpSend,QTreeWidgetItem *pParent);
	void LoadStormItem(stuSimStorm *pStorm,QTreeWidgetItem *pParent);
	void LoadIedItem(stuSclVtIed *pVtIed,QTreeWidgetItem *pParent);
	void LoadDataSetItem(stuSclVtIedDataset* pDataSet,QTreeWidgetItem* pParent);
	
	//LDZ 20181107 添加有关智能告警测试工具命令处理有关逻辑
	int doExecCmd(SString& args);
	bool startServer();
	bool stopServer();
	bool startSimClient();
	////////////////
public slots:
	void OnTreeCurrentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);
	void treeItemPressed(QTreeWidgetItem *item, int column);
	void OnNewNetPort();
	void OnDelNetPort();
	void OnNewSmvOut();
	void OnDelSelectedSMV();
	void OnNewGooseOut();
	void OnDelSelectedGooseOut();
	void OnNewMMSServer();

	void OnNewMmsClient();
signals:
	bool sigStartSever();
	bool sigStopSever();
private:
	QWidget* GetTreeWidgetItemPanel(QTreeWidgetItem* item, bool bShow=false);
private:
	Ui::view_home ui;
	CSimConfig *m_pSimConfig;
	CSimManager *m_pSimManager;
	QIcon m_iconHome,m_iconNet,m_iconSmv,m_iconGoose,m_iconMms,m_iconMmsClient,m_iconNtp,m_iconNtpClient,m_iconStorm,m_iconComm,m_iconMmsServerIed,m_iconDataSet;
	QTreeWidgetItem *m_pRoot;
	QTreeWidgetItem *m_pThisItem;
	panel_sim_root *m_pPanelSimRoot;
	panel_sim_mmssvr *m_pPanelSimMmsSvr;
	panel_sim_param_set* m_pPanelSimParamSet;
	panel_sim_modify_dataset* m_pPanelSimDataSet;
	panel_ntp_client*   m_pPanelNtpClient;
	panel_ntp_client*   m_pPanelNtpServer;
	panel_sim_icmp* m_pPanelSimIcmp;
	panel_sim_tcp_client* m_pPanelSimTcpClient;
	panel_sim_tcp_server* m_pPanelSimTcpServer;
	panel_sim_arp* m_pPanelSimArp;
	panel_sim_udp* m_pPanelSimUdp;
	panel_sim_net_storm* m_pPanelSimStorm;
	int m_iConfigEditSn;
	CSimClient m_simClient;
	ConfigFileMgr m_configFileMgr;

};

#endif // VIEW_HOME_H
