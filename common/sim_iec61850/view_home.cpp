/**
 *
 * 文 件 名 : view_home.cpp
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
#include "view_home.h"
#include "GuiApplication.h"
#include "dlg_new_net.h"
#include "dlg_new_smvout.h"
#include "dlg_new_gseout.h"
#include "SRawSocket.h"
#include "dlg_mmsserver_add_ied.h"

view_home::view_home(QWidget *parent)
	: CBaseView(parent)
{
	ui.setupUi(this);
	CGuiApplication *pApp = (CGuiApplication*)SApplication::GetPtr();
	m_pSimConfig = &pApp->m_SimConfig;
	m_pRoot = NULL;
	m_iconHome = QIcon(":/sim_iec61850/Resources/home.gif");
	m_iconNet = QIcon(":/sim_iec61850/Resources/NET01.ICO");
	m_iconSmv = QIcon(":/sim_iec61850/Resources/view_inst.png");
	m_iconGoose = QIcon(":/sim_iec61850/Resources/switch.png");
	m_iconMms = QIcon(":/sim_iec61850/Resources/earth.gif");
	m_iconMmsClient = QIcon(":/sim_iec61850/Resources/pc.gif");
	m_iconNtp = QIcon(":/sim_iec61850/Resources/TIMER01.ICO");
	m_iconNtpClient = QIcon(":/sim_iec61850/Resources/CLOCK04.ICO");
	m_iconStorm = QIcon(":/sim_iec61850/Resources/fault.gif");
	m_iconComm = QIcon(":/sim_iec61850/Resources/NET08.ICO");
	m_iconMmsServerIed = QIcon(":/sim_iec61850/Resources/DRIVE.ICO");
	m_iconDataSet = QIcon(":/sim_iec61850/Resources/20070402165933835.gif");
	m_pSimManager = &CGuiApplication::GetApp()->m_SimManager;
	m_pPanelSimRoot = new panel_sim_root(m_pSimManager,ui.widget);
	ui.gridLayout_2->addWidget(m_pPanelSimRoot);
	m_pPanelSimMmsSvr = new panel_sim_mmssvr(ui.widget);
	m_pPanelSimMmsSvr->m_pSimManager = m_pSimManager;
	m_pPanelSimMmsSvr->hide();
	m_pPanelSimMmsSvr->Load();
	ui.gridLayout_2->addWidget(m_pPanelSimMmsSvr);

	m_pPanelSimParamSet = new panel_sim_param_set(ui.widget);
	m_pPanelSimParamSet->hide();
	ui.gridLayout_2->addWidget(m_pPanelSimParamSet);

	m_pPanelSimDataSet = new panel_sim_modify_dataset(ui.widget);
	m_pPanelSimDataSet->hide();
	ui.gridLayout_2->addWidget(m_pPanelSimDataSet);
#if 1
	m_pPanelNtpClient = new panel_ntp_client(ui.widget,NTP_CLIENT);
	m_pPanelNtpClient->hide();
	ui.gridLayout_2->addWidget(m_pPanelNtpClient);

	m_pPanelNtpServer = new panel_ntp_client(ui.widget,NTP_SERVER);
	m_pPanelNtpServer->hide();
	ui.gridLayout_2->addWidget(m_pPanelNtpServer);

	m_pPanelSimIcmp = new panel_sim_icmp(ui.widget);
	m_pPanelSimIcmp->hide();
	ui.gridLayout_2->addWidget(m_pPanelSimIcmp);

	m_pPanelSimTcpClient = new panel_sim_tcp_client(ui.widget);
	m_pPanelSimTcpClient->hide();
	ui.gridLayout_2->addWidget(m_pPanelSimTcpClient);

	m_pPanelSimTcpServer = new panel_sim_tcp_server(ui.widget);
	m_pPanelSimTcpServer->hide();
	ui.gridLayout_2->addWidget(m_pPanelSimTcpServer);
#if 1
	m_pPanelSimArp = new panel_sim_arp(ui.widget);
	m_pPanelSimArp->hide();
	ui.gridLayout_2->addWidget(m_pPanelSimArp);
#endif
	m_pPanelSimUdp = new panel_sim_udp(ui.widget);
	m_pPanelSimUdp->hide();
	ui.gridLayout_2->addWidget(m_pPanelSimUdp);

	m_pPanelSimStorm = new panel_sim_net_storm(ui.widget);
	m_pPanelSimStorm->hide();
	ui.gridLayout_2->addWidget(m_pPanelSimStorm);
#endif
	QObject::connect(ui.treeWidget, SIGNAL(itemPressed(QTreeWidgetItem*, int)), this, SLOT(treeItemPressed(QTreeWidgetItem*, int)));
	connect(ui.treeWidget,SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)),this,SLOT(OnTreeCurrentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)));
	m_iConfigEditSn = 0;
	Load();
	CGuiApplication::GetApp()->NewEvent("系统",0,0,"","提示","系统启动!");

#if 1
	m_simClient.setOptObj(this,&m_configFileMgr);
	SString strPath = CGuiApplication::GetApp()->GetConfPath()+"module.xml";
	if(!m_configFileMgr.readConfigFile(strPath))
	{
		LOGERROR("模块命令配置文件读取失败，请检查配置文件是否正确！");
		CGuiApplication::GetApp()->NewEvent("系统",0,0,"","告警","读取module.xml文件失败!");
	} else
	{
		startSimClient();
	}
	connect(this,SIGNAL(sigStartSever()),m_pPanelSimRoot,SLOT(CommandToStart()));
	connect(this,SIGNAL(sigStopSever()),m_pPanelSimRoot,SLOT(CommandToStop()));
#endif
}

view_home::~view_home()
{

}
void view_home::OnPreShow()
{
	if(m_iConfigEditSn != m_pSimConfig->m_iEditSn)
	{
		Load();
		m_iConfigEditSn = m_pSimConfig->m_iEditSn;
	}
}
void view_home::Load()
{
	ui.treeWidget->clear();
	m_pRoot = new QTreeWidgetItem();
	m_pRoot->setData(0,Qt::UserRole,QVariant(SIM_TREE_HOME));
	m_pRoot->setData(0,Qt::UserRole+1,QVariant("0"));
	m_pRoot->setText(0,"IEC61850 Simulator V1.0.0");
	m_pRoot->setIcon(0,m_iconHome);
	ui.treeWidget->addTopLevelItem(m_pRoot);
	ui.treeWidget->setCurrentItem(m_pRoot);

	//添加MMSSERVER
	QTreeWidgetItem *pMmsNode = new QTreeWidgetItem();
	pMmsNode->setData(0,Qt::UserRole,QVariant(SIM_TREE_MMSSVR));
	SString desc = "MMS-Server";
	pMmsNode->setText(0,desc.data());
	pMmsNode->setToolTip(0,desc.data());
	pMmsNode->setIcon(0,m_iconMms);
	m_pRoot->addChild(pMmsNode);

	unsigned long pos1 = 0;
	stuSclVtIed *pVtIed = m_pSimConfig->m_MmsServerIed.FetchFirst(pos1);
	while(pVtIed)
	{
		LoadIedItem(pVtIed,pMmsNode);
		pVtIed = m_pSimConfig->m_MmsServerIed.FetchNext(pos1);
	}
	pMmsNode->setExpanded(true);
	unsigned long pos = 0;
	stuSimSubnetwork *pSubNet = m_pSimConfig->m_SubNet.FetchFirst(pos);
	while(pSubNet)
	{
		QTreeWidgetItem *pSubNetNode = new QTreeWidgetItem();
		pSubNetNode->setData(0,Qt::UserRole,QVariant(SIM_TREE_SUBNET));
		pSubNetNode->setData(0,Qt::UserRole+1,QVariant(SString::toFormat("%ld",(unsigned long)pSubNet).data()));
		SString desc = SString::toFormat("%s(%s)",pSubNet->desc.data(),pSubNet->ip.data());
		pSubNetNode->setText(0,desc.data());
		pSubNetNode->setToolTip(0,desc.data());
		pSubNetNode->setIcon(0,m_iconNet);
		m_pRoot->addChild(pSubNetNode);
		LoadSubNet(pSubNet,pSubNetNode);
		pSubNetNode->setExpanded(true);
		pSubNet = m_pSimConfig->m_SubNet.FetchNext(pos);
	}
	m_pRoot->setExpanded(true);
	ui.treeWidget->resizeColumnToContents(0);
}

void view_home::LoadSubNet(stuSimSubnetwork *pSubNet,QTreeWidgetItem *pNetItem)
{
	QTreeWidgetItem *p;
	unsigned long pos = 0;

	//SMV
	p = new QTreeWidgetItem();
	p->setData(0,Qt::UserRole,QVariant(SIM_TREE_SMV));
	p->setData(0,Qt::UserRole+1,QVariant(SString::toFormat("%ld",(unsigned long)pSubNet).data()));
	p->setText(0,"SMV");
	p->setIcon(0,m_iconSmv);
	pNetItem->addChild(p);
	stuSclVtIedSmvOut *pSmv = pSubNet->m_Smv.FetchFirst(pos);
	while(pSmv)
	{
		LoadSmvItem(pSmv,p);		
		pSmv = pSubNet->m_Smv.FetchNext(pos);
	}
	p->setExpanded(true);

	//GOOSE
	p = new QTreeWidgetItem();
	p->setData(0,Qt::UserRole,QVariant(SIM_TREE_GOOSE));
	p->setData(0,Qt::UserRole+1,QVariant(SString::toFormat("%ld",(unsigned long)pSubNet).data()));
	p->setText(0,"GOOSE");
	p->setIcon(0,m_iconGoose);
	pNetItem->addChild(p);
	stuSclVtIedGooseOut *pGoose = pSubNet->m_Goose.FetchFirst(pos);
	while(pGoose)
	{
		LoadGooseItem(pGoose,p);
		pGoose = pSubNet->m_Goose.FetchNext(pos);
	}
	p->setExpanded(true);

// 	//MMSSVR
// 	p = new QTreeWidgetItem();
// 	p->setData(0,Qt::UserRole,QVariant(SIM_TREE_MMSSVR));
// 	p->setData(0,Qt::UserRole+1,QVariant(SString::toFormat("%ld",(unsigned long)pSubNet).data()));
// 	p->setText(0,"MMS-Server");
// 	p->setIcon(0,m_iconMms);
// 	pNetItem->addChild(p);
// 	stuSimMmsSvr *pMmsSvr = pSubNet->m_MmsSvr.FetchFirst(pos);
// 	while(pMmsSvr)
// 	{
// 		LoadMmsSvrItem(pMmsSvr,p);
// 		pMmsSvr = pSubNet->m_MmsSvr.FetchNext(pos);
// 	}
// 	p->setExpanded(true);

	//MMSCLIENT
	p = new QTreeWidgetItem();
	p->setData(0,Qt::UserRole,QVariant(SIM_TREE_MMSCLIENT));
	p->setData(0,Qt::UserRole+1,QVariant(SString::toFormat("%ld",(unsigned long)pSubNet).data()));
	p->setText(0,"MMS-Client");
	p->setIcon(0,m_iconMmsClient);
	pNetItem->addChild(p);
	stuSimMmsClient *pMmsClient = pSubNet->m_MmsClient.FetchFirst(pos);
	while(pMmsClient)
	{
		LoadMmsClientItem(pMmsClient,p);
		pMmsClient = pSubNet->m_MmsClient.FetchNext(pos);
	}
	p->setExpanded(true);

	//NTPSVR
	p = new QTreeWidgetItem();
	p->setData(0,Qt::UserRole,QVariant(SIM_TREE_NTPSVR));
	p->setData(0,Qt::UserRole+1,QVariant(SString::toFormat("%ld",(unsigned long)pSubNet).data()));
	p->setText(0,"NTP-Server");
	p->setIcon(0,m_iconNtp);
	pNetItem->addChild(p);
	stuSimNtpSvr *pNtpSvr = pSubNet->m_NtpSvr.FetchFirst(pos);
	while(pNtpSvr)
	{
		LoadNtpSvrItem(pNtpSvr,p);
		pNtpSvr = pSubNet->m_NtpSvr.FetchNext(pos);
	}
	p->setExpanded(true);

	//NTPCLIENT
	p = new QTreeWidgetItem();
	p->setData(0,Qt::UserRole,QVariant(SIM_TREE_NTPCLIENT));
	p->setData(0,Qt::UserRole+1,QVariant(SString::toFormat("%ld",(unsigned long)pSubNet).data()));
	p->setText(0,"NTP-Client");
	p->setIcon(0,m_iconNtpClient);
	pNetItem->addChild(p);
	stuSimNtpClient *pNtpClient = pSubNet->m_NtpClient.FetchFirst(pos);
	while(pNtpClient)
	{
		LoadNtpClientItem(pNtpClient,p);
		pNtpClient = pSubNet->m_NtpClient.FetchNext(pos);
	}
	p->setExpanded(true);

	//TCPSVR
	p = new QTreeWidgetItem();
	p->setData(0,Qt::UserRole,QVariant(SIM_TREE_TCPSVR));
	p->setData(0,Qt::UserRole+1,QVariant(SString::toFormat("%ld",(unsigned long)pSubNet).data()));
	p->setText(0,"TCP-Server");
	p->setIcon(0,m_iconComm);
	pNetItem->addChild(p);
	stuSimTcpSvr *pTcpSvr = pSubNet->m_TcpSvr.FetchFirst(pos);
	while(pTcpSvr)
	{
		LoadTcpSvrItem(pTcpSvr,p);
		pTcpSvr = pSubNet->m_TcpSvr.FetchNext(pos);
	}
	p->setExpanded(true);

	//TCTPCLIENT
	p = new QTreeWidgetItem();
	p->setData(0,Qt::UserRole,QVariant(SIM_TREE_TCPCLIENT));
	p->setData(0,Qt::UserRole+1,QVariant(SString::toFormat("%ld",(unsigned long)pSubNet).data()));
	p->setText(0,"TCP-Client");
	p->setIcon(0,m_iconComm);
	pNetItem->addChild(p);
	stuSimTcpClient *pTcpClient = pSubNet->m_TcpClient.FetchFirst(pos);
	while(pTcpClient)
	{
		LoadTcpClientItem(pTcpClient,p);
		pTcpClient = pSubNet->m_TcpClient.FetchNext(pos);
	}
	p->setExpanded(true);

	//UDPSEND
	p = new QTreeWidgetItem();
	p->setData(0,Qt::UserRole,QVariant(SIM_TREE_UDPSEND));
	p->setData(0,Qt::UserRole+1,QVariant(SString::toFormat("%ld",(unsigned long)pSubNet).data()));
	p->setText(0,"Udp");
	p->setIcon(0,m_iconComm);
	pNetItem->addChild(p);
	stuSimUdpSend *pUdpSend = pSubNet->m_UdpSend.FetchFirst(pos);
	while(pUdpSend)
	{
		LoadUdpSendItem(pUdpSend,p);
		pUdpSend = pSubNet->m_UdpSend.FetchNext(pos);
	}
	p->setExpanded(true);

	//ICMPSEND
	p = new QTreeWidgetItem();
	p->setData(0,Qt::UserRole,QVariant(SIM_TREE_ICMPSEND));
	p->setData(0,Qt::UserRole+1,QVariant(SString::toFormat("%ld",(unsigned long)pSubNet).data()));
	p->setText(0,"Icmp");
	p->setIcon(0,m_iconComm);
	pNetItem->addChild(p);
	stuSimIcmpSend *pIcmpSend = pSubNet->m_IcmpSend.FetchFirst(pos);
	while(pIcmpSend)
	{
		LoadIcmpSendItem(pIcmpSend,p);
		pIcmpSend = pSubNet->m_IcmpSend.FetchNext(pos);
	}
	p->setExpanded(true);

	//ARPSEND
	p = new QTreeWidgetItem();
	p->setData(0,Qt::UserRole,QVariant(SIM_TREE_ARPSEND));
	p->setData(0,Qt::UserRole+1,QVariant(SString::toFormat("%ld",(unsigned long)pSubNet).data()));
	p->setText(0,"Arp");
	p->setIcon(0,m_iconComm);
	pNetItem->addChild(p);
	stuSimArpSend *pArpSend = pSubNet->m_ArpSend.FetchFirst(pos);
	while(pArpSend)
	{
		LoadArpSendItem(pArpSend,p);
		pArpSend = pSubNet->m_ArpSend.FetchNext(pos);
	}
	p->setExpanded(true);

	//STORM
	p = new QTreeWidgetItem();
	p->setData(0,Qt::UserRole,QVariant(SIM_TREE_STORM));
	p->setData(0,Qt::UserRole+1,QVariant(SString::toFormat("%ld",(unsigned long)pSubNet).data()));
	p->setText(0,"网络风暴");
	p->setIcon(0,m_iconStorm);
	pNetItem->addChild(p);
	stuSimStorm *pStorm = pSubNet->m_Storm.FetchFirst(pos);
	while(pStorm)
	{
		LoadStormItem(pStorm,p);
		pStorm = pSubNet->m_Storm.FetchNext(pos);
	}
	p->setExpanded(true);
}

void view_home::LoadSmvItem(stuSclVtIedSmvOut *pSmv,QTreeWidgetItem *pParent)
{
	QTreeWidgetItem *p;
	p = new QTreeWidgetItem();
	p->setData(0,Qt::UserRole,QVariant(SIM_TREE_SMV_ITEM));
	//p->setData(0,Qt::UserRole+1,QVariant("0"));
	p->setData(0,Qt::UserRole+1,QVariant(SString::toFormat("%ld",(unsigned long)pSmv).data()));
	SString ied_desc;
	stuSclVtIed *pIed = m_pSimConfig->m_VirtualTerminal.SearchIedByName(pSmv->ied_name.data());
	if(pIed != NULL)
		ied_desc = pIed->desc;
	SString desc = SString::toFormat("[0x%04X] %s %s",pSmv->appid,pSmv->ied_name.data(),ied_desc.data());	
	p->setText(0,desc.data());
	p->setToolTip(0,desc.data());
	p->setIcon(0,m_iconSmv);
	pParent->addChild(p);
}
void view_home::LoadGooseItem(stuSclVtIedGooseOut *pGoose,QTreeWidgetItem *pParent)
{
	QTreeWidgetItem *p;
	p = new QTreeWidgetItem();
	p->setData(0,Qt::UserRole,QVariant(SIM_TREE_GOOSE_ITEM));
	SString ied_desc;
	stuSclVtIed *pIed = m_pSimConfig->m_VirtualTerminal.SearchIedByName(pGoose->ied_name.data());
	if(pIed != NULL)
		ied_desc = pIed->desc;
	//p->setData(0,Qt::UserRole+1,QVariant("0"));
	p->setData(0,Qt::UserRole+1,QVariant(SString::toFormat("%ld",(unsigned long)pGoose).data()));
	SString desc;
	desc.sprintf("[0x%04X] %s %s",pGoose->appid,pGoose->ied_name.data(),ied_desc.data());
	p->setText(0,desc.data());
	p->setToolTip(0,desc.data());
	p->setIcon(0,m_iconGoose);
	pParent->addChild(p);
}
void view_home::LoadMmsSvrItem(stuSimMmsSvr *pMmsSvr,QTreeWidgetItem *pParent)
{

}
void view_home::LoadMmsClientItem(stuSimMmsClient *pMmsClient,QTreeWidgetItem *pParent)
{

}
void view_home::LoadNtpSvrItem(stuSimNtpSvr *pNtpSvr,QTreeWidgetItem *pParent)
{

}
void view_home::LoadNtpClientItem(stuSimNtpClient *pNtpClient,QTreeWidgetItem *pParent)
{

}
void view_home::LoadTcpSvrItem(stuSimTcpSvr *pTcpSvr,QTreeWidgetItem *pParent)
{

}
void view_home::LoadTcpClientItem(stuSimTcpClient *pTcpClient,QTreeWidgetItem *pParent)
{

}
void view_home::LoadUdpSendItem(stuSimUdpSend *pUdpSend,QTreeWidgetItem *pParent)
{

}
void view_home::LoadIcmpSendItem(stuSimIcmpSend *pIcmpSend,QTreeWidgetItem *pParent)
{

}
void view_home::LoadArpSendItem(stuSimArpSend *pArpSend,QTreeWidgetItem *pParent)
{

}
void view_home::LoadStormItem(stuSimStorm *pStorm,QTreeWidgetItem *pParent)
{

}

QWidget* view_home::GetTreeWidgetItemPanel(QTreeWidgetItem* item,bool bShow)
{
	if(item == NULL)
		return NULL;
	eSimTreeNodeType node_type = (eSimTreeNodeType)item->data(0,Qt::UserRole).toInt();
	switch(node_type)
	{
	case SIM_TREE_HOME:
		return m_pPanelSimRoot;
		break;
	case SIM_TREE_SUBNET:
		break;
	case SIM_TREE_SMV:
		break;
	case SIM_TREE_SMV_ITEM:
		if (bShow)
		{
			m_pPanelSimParamSet->setCurTreeItem(item);
			m_pPanelSimParamSet->OnRefresh();
		}
		return m_pPanelSimParamSet;
		break;
	case SIM_TREE_GOOSE:
		break;
	case SIM_TREE_GOOSE_ITEM:
		if (bShow)
		{
			m_pPanelSimParamSet->setCurTreeItem(item);
			m_pPanelSimParamSet->OnRefresh();
		}
		return m_pPanelSimParamSet;
		break;
	case SIM_TREE_MMSSVR:
		return m_pPanelSimMmsSvr;
		break;
	case SIM_TREE_MMSSVR_ITEM:
		
		break;
	case SIM_TREE_MMSCLIENT:
	
		break;
	case SIM_TREE_MMSCLIENT_ITEM:
		break;
	case SIM_TREE_NTPSVR:
		return m_pPanelNtpServer;
		break;
	case SIM_TREE_NTPSVR_ITEM:
		break;
	case SIM_TREE_NTPCLIENT:
		return m_pPanelNtpClient;
		break;
	case SIM_TREE_NTPCLIENT_ITEM:
		break;
	case SIM_TREE_TCPSVR:
		return m_pPanelSimTcpServer;
		break;
	case SIM_TREE_TCPSVR_ITEM:
		break;
	case SIM_TREE_TCPCLIENT:
		return m_pPanelSimTcpClient;
		break;
	case SIM_TREE_TCPCLIENT_ITEM:
		break;
	case SIM_TREE_UDPSEND:
		return m_pPanelSimUdp;
		break;
	case SIM_TREE_UDPSEND_ITEM:
		break;
	case SIM_TREE_ICMPSEND:
		return m_pPanelSimIcmp;
		break;
	case SIM_TREE_ICMPSEND_ITEM:
		break;
	case SIM_TREE_ARPSEND:
		if (bShow)
		{
			m_pPanelSimArp->setCurTreeItem(item);
			m_pPanelSimArp->OnRefresh();
		}
		return m_pPanelSimArp;
		break;
	case SIM_TREE_ARPSEND_ITEM:
		break;
	case SIM_TREE_STORM:
		if (bShow)
		{
			m_pPanelSimStorm->setCurTreeItem(item);
			m_pPanelSimStorm->OnRefresh();
		}
		return m_pPanelSimStorm;
		break;
	case SIM_TREE_STORM_ITEM:
		break;
	case SIM_TREE_MMSSVR_ITEM_DATASET:
		{
			if (bShow)
			{
				m_pPanelSimDataSet->m_sMmsDsIedName = item->data(0,Qt::UserRole+2).toString().toStdString().data();
				m_pPanelSimDataSet->setCurTreeItem(item);
				m_pPanelSimDataSet->OnRefresh();
			}
			return m_pPanelSimDataSet;
		}
		break;
	}
	return NULL;
}

void view_home::OnTreeCurrentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous)
{
	if(previous != NULL)
	{
		QWidget *pOld = GetTreeWidgetItemPanel(previous);
		if(pOld != NULL)
		{
			pOld->hide();
		}
	}
	QWidget *pNow = GetTreeWidgetItemPanel(current,true);
	if(pNow != NULL)
	{
		pNow->show();
	}
	//p->setData(0,Qt::UserRole+1,QVariant(SString::toFormat("%ld",(unsigned long)pSubNet).data()));
}


void view_home::treeItemPressed( QTreeWidgetItem *item, int column )
{
	if (qApp->mouseButtons() == Qt::RightButton && item != NULL) 
	{
		m_pThisItem = item;
		eSimTreeNodeType node_type = (eSimTreeNodeType)item->data(0,Qt::UserRole).toInt();
		switch(node_type)
		{
		case SIM_TREE_HOME:
			{
				QMenu menu(this);
				menu.addAction(tr("添加模拟网口"),this,SLOT(OnNewNetPort()));
				menu.exec(QCursor::pos());
			}
			break;
		case SIM_TREE_SUBNET:
			{
				QMenu menu(this);
				menu.addAction(tr("删除模拟网口"),this,SLOT(OnDelNetPort()));
				menu.exec(QCursor::pos());
			}
			break;
		case SIM_TREE_SMV:
			{
				QMenu menu(this);
				menu.addAction(tr("添加SMV发送到当前端口"),this,SLOT(OnNewSmvOut()));
				menu.exec(QCursor::pos());
			}
			break;
		case SIM_TREE_SMV_ITEM:
			{
#if 0
				QMenu menu(this);
				menu.addAction(tr("删除当前选中SMV"),this,SLOT(OnDelSelectedSMV()));
				menu.exec(QCursor::pos());
#endif
			}
			break;
		case SIM_TREE_GOOSE:
			{
				QMenu menu(this);
				menu.addAction(tr("添加GOOSE发送到当前端口"),this,SLOT(OnNewGooseOut()));
				menu.exec(QCursor::pos());
			}
			break;
		case SIM_TREE_GOOSE_ITEM:
			{
#if 0
				QMenu menu(this);
				menu.addAction(tr("删除当前选中GOOSE"),this,SLOT(OnDelSelectedGooseOut()));
				menu.exec(QCursor::pos());
#endif
			}
			break;
		case SIM_TREE_MMSSVR:
			{
				QMenu menu(this);
				menu.addAction(tr("添加设备发送到当前端口"),this,SLOT(OnNewMMSServer()));
				menu.exec(QCursor::pos());
			}
			break;
		case SIM_TREE_MMSSVR_ITEM:
			break;
		case SIM_TREE_MMSCLIENT:
			{
				QMenu menu(this);
				menu.addAction(tr("添加MMSClient发送到当前端口"),this,SLOT(OnNewMmsClient()));
				menu.exec(QCursor::pos());
			}
			break;
		case SIM_TREE_MMSCLIENT_ITEM:
			break;
		case SIM_TREE_NTPSVR:
			break;
		case SIM_TREE_NTPSVR_ITEM:
			break;
		case SIM_TREE_NTPCLIENT:
			break;
		case SIM_TREE_NTPCLIENT_ITEM:
			break;
		case SIM_TREE_TCPSVR:
			break;
		case SIM_TREE_TCPSVR_ITEM:
			break;
		case SIM_TREE_TCPCLIENT:
			break;
		case SIM_TREE_TCPCLIENT_ITEM:
			break;
		case SIM_TREE_UDPSEND:
			break;
		case SIM_TREE_UDPSEND_ITEM:
			break;
		case SIM_TREE_ICMPSEND:
			break;
		case SIM_TREE_ICMPSEND_ITEM:
			break;
		case SIM_TREE_ARPSEND:
			break;
		case SIM_TREE_ARPSEND_ITEM:
			break;
		case SIM_TREE_STORM:
			break;
		case SIM_TREE_STORM_ITEM:
			break;
		}
	}
}

void view_home::OnNewNetPort()
{
	dlg_new_net dlg(m_pSimConfig);
	dlg.exec();
	if(dlg.m_SelectedNetDev.count() == 0)
		return;
	unsigned long pos = 0;
	SRawSocketRecv::stuPcapDevice *pDev = dlg.m_SelectedNetDev.FetchFirst(pos);
	while(pDev)
	{
		stuSimSubnetwork *pNet = new stuSimSubnetwork();
		pNet->name = pDev->name;
		pNet->desc = pDev->desc;
		pNet->ip = pDev->ip;
		pNet->mac = pDev->mac;
		m_pSimConfig->m_SubNet.append(pNet);

		QTreeWidgetItem *pSubNetNode = new QTreeWidgetItem();
		pSubNetNode->setData(0,Qt::UserRole,QVariant(SIM_TREE_SUBNET));
		pSubNetNode->setData(0,Qt::UserRole+1,QVariant(SString::toFormat("%ld",(unsigned long)pNet).data()));
		pSubNetNode->setText(0,SString::toFormat("%s(%s)",pNet->desc.data(),pNet->ip.data()).data());
		pSubNetNode->setIcon(0,m_iconNet);
		m_pRoot->addChild(pSubNetNode);
		LoadSubNet(pNet,pSubNetNode);
		pSubNetNode->setExpanded(true);

		pDev = dlg.m_SelectedNetDev.FetchNext(pos);
	}

	m_pSimConfig->Save();
	ui.treeWidget->resizeColumnToContents(0);
}

void view_home::OnDelNetPort()
{
	QString qstr = m_pThisItem->data(0,Qt::UserRole+1).toString();
	stuSimSubnetwork *pNet = (stuSimSubnetwork *)atol(qstr.toStdString().data());
	if(pNet == NULL)
		return;
	if(SQt::ShowQuestionBox("提示",SString::toFormat("您是否确认删除网口'%s:%s'?",pNet->name.data(),pNet->desc.data()).data()) == false)
		return;
	m_pSimConfig->m_SubNet.remove(pNet);
	m_pSimConfig->Save();
	m_pRoot->removeChild(m_pThisItem);
	ui.treeWidget->resizeColumnToContents(0);
}

void view_home::OnNewSmvOut()
{
	QString qstr = m_pThisItem->data(0,Qt::UserRole+1).toString();
	stuSimSubnetwork *pNet = (stuSimSubnetwork *)atol(qstr.toStdString().data());
	if(pNet == NULL)
		return;
	dlg_new_smvout dlg(m_pSimConfig,pNet);
	dlg.exec();
	if(dlg.m_SelectedSmv.count() == 0)
		return;
	unsigned long pos;
	stuSclVtIedSmvOut *pOut = dlg.m_SelectedSmv.FetchFirst(pos);
	while(pOut)
	{
		pNet->m_Smv.append(pOut);
		LoadSmvItem(pOut,m_pThisItem);
		pOut = dlg.m_SelectedSmv.FetchNext(pos);
	}
	m_pSimConfig->Save();
	ui.treeWidget->resizeColumnToContents(0);
}
void view_home::OnDelSelectedSMV()
{
	QTreeWidgetItem* pCurParent = m_pThisItem->parent();
	if (pCurParent == NULL)
	{
		return;
	}
	QString qstr = pCurParent->data(0,Qt::UserRole+1).toString();
	stuSimSubnetwork *pNet = (stuSimSubnetwork *)atol(qstr.toStdString().data());
	if(pNet == NULL)
		return;
	qstr = m_pThisItem->data(0,Qt::UserRole+1).toString();
	stuSclVtIedSmvOut *pSmvOut = (stuSclVtIedSmvOut *)atol(qstr.toStdString().data());
	if(pSmvOut == NULL)
		return;
	pNet->delSmvOut(pSmvOut);
	pCurParent->removeChild(m_pThisItem);
	m_pSimConfig->Save();
}

void view_home::OnNewGooseOut()
{
	QString qstr = m_pThisItem->data(0,Qt::UserRole+1).toString();
	stuSimSubnetwork *pNet = (stuSimSubnetwork *)atol(qstr.toStdString().data());
	if(pNet == NULL)
		return;
	dlg_new_gseout dlg(m_pSimConfig,pNet);
	dlg.exec();
	if(dlg.m_SelectedGoose.count() == 0)
		return;
	unsigned long pos;
	stuSclVtIedGooseOut *pOut = dlg.m_SelectedGoose.FetchFirst(pos);
	while(pOut)
	{
		pNet->m_Goose.append(pOut);
		LoadGooseItem(pOut,m_pThisItem);
		pOut = dlg.m_SelectedGoose.FetchNext(pos);
	}
	m_pSimConfig->Save();
	ui.treeWidget->resizeColumnToContents(0);
}

void view_home::OnDelSelectedGooseOut()
{
	QTreeWidgetItem* pCurParent = m_pThisItem->parent();
	if (pCurParent == NULL)
	{
		return;
	}
	QString qstr = pCurParent->data(0,Qt::UserRole+1).toString();
	stuSimSubnetwork *pNet = (stuSimSubnetwork *)atol(qstr.toStdString().data());
	if(pNet == NULL)
		return;
	qstr = m_pThisItem->data(0,Qt::UserRole+1).toString();
	stuSclVtIedGooseOut *pGooseOut = (stuSclVtIedGooseOut *)atol(qstr.toStdString().data());
	if(pGooseOut == NULL)
		return;
	pNet->delGooseOut(pGooseOut);
	pCurParent->removeChild(m_pThisItem);
	m_pSimConfig->Save();
	
}

void view_home::OnNewMMSServer()
{
	dlg_mmsserver_add_ied dlg(m_pSimConfig,this);
	dlg.exec();
	if(dlg.m_SelectedIed.count() == 0)
		return;
	unsigned long pos;
	stuSclVtIed *pVtIed = dlg.m_SelectedIed.FetchFirst(pos);
	while(pVtIed)
	{
		m_pSimConfig->m_MmsServerIed.append(pVtIed);
		LoadIedItem(pVtIed,m_pThisItem);
		pVtIed = dlg.m_SelectedIed.FetchNext(pos);
	}
	m_pThisItem->setExpanded(true);
	m_pSimConfig->Save();
	ui.treeWidget->resizeColumnToContents(0);
}

void view_home::LoadIedItem(stuSclVtIed *pVtIed,QTreeWidgetItem *pParent)
{
	if (pVtIed == NULL || pParent == NULL)
	{
		return;
	}
	QTreeWidgetItem *p;
	p = new QTreeWidgetItem();
	p->setData(0,Qt::UserRole,QVariant(SIM_TREE_MMSSVR_ITEM));
	p->setData(0,Qt::UserRole+1,QVariant(SString::toFormat("%ld",(unsigned long)pVtIed).data()));
	SString desc;
	desc.sprintf("%s %s",pVtIed->name,pVtIed->desc);
	p->setText(0,desc.data());
	p->setToolTip(0,desc.data());
	p->setIcon(0,m_iconMmsServerIed);
	pParent->addChild(p);
	for(int i = 0; i < pVtIed->m_Dataset.count(); i ++)
	{
		stuSclVtIedDataset* pCur = pVtIed->m_Dataset.at(i);
		if (pCur == NULL)
		{
			continue;
		}
		LoadDataSetItem(pCur,p);
	}
	//p->setExpanded(true);
}

void view_home::LoadDataSetItem(stuSclVtIedDataset* pDataSet,QTreeWidgetItem* pParent)
{
	if (pDataSet == NULL || pParent == NULL)
	{
		return;
	}
	QTreeWidgetItem *p;
	p = new QTreeWidgetItem();
	p->setData(0,Qt::UserRole,QVariant(SIM_TREE_MMSSVR_ITEM_DATASET));
	p->setData(0,Qt::UserRole+1,QVariant(SString::toFormat("%ld",(unsigned long)pDataSet).data()));
	p->setData(0,Qt::UserRole+2,QVariant(SString::toFormat("%s",pDataSet==NULL?"":pDataSet->iedname).data()));
	SString desc;
	desc.sprintf("[%d] %s %s",pDataSet->ds_no,pDataSet->name,pDataSet->desc);
	p->setText(0,desc.data());
	p->setToolTip(0,desc.data());
	p->setIcon(0,m_iconDataSet);
	pParent->addChild(p);
	
}
void view_home::OnNewMmsClient()
{

}

//bool view_home::doHandleServerCommand()
int view_home::doExecCmd(SString& args)
{
	SString strIedName = SString::GetAttributeValue(args,"ied_name","=",",");
	SString strSignalMmsPath = SString::GetAttributeValue(args,"signal_mms_path","=",",");
	//SString strVal = SString::GetAttributeValue(args,"val","=",",");
	int iPosStart = args.find("{");
	int iPosEnd = args.find("}");
	if (iPosStart < 0 || iPosEnd < 0 || iPosEnd < iPosStart)
	{
		return 3;
	}
	SString strVal = args.substr(iPosStart,iPosEnd-iPosStart+1);
	
	//stuSclVtIedDatasetFcda* pCurFcda = NULL;//(stuSclVtIedDatasetFcda*)strAddr.toLong();

	//QString strCurText = "";//item->text();	
	CSimMmsServer * m_pSimMmsServer = &(m_pSimManager->m_MmsServer);//&pApp->m_SimManager.m_MmsServer;
#if 0
	if(!m_pSimMmsServer->IsRuning())
	{
		//SQt::ShowErrorBox("提示","MMS服务尚未启动!");
		return 5;
	}
#endif
	//QString mmspath = strSignalMmsPath;//ui.tableWidget->item(row,10)->text();
	SString m_sMmsDsIedName = strIedName;//表示设备名称也就是设备的MMS_PATH
	SString path=m_sMmsDsIedName+strSignalMmsPath;//mmspath.toStdString().data();
	//stuSclVtIed* pFindIed = m_pSimConfig->m_VirtualTerminal.SearchIedByName(m_sMmsDsIedName.data());
	unsigned long pos1;
	stuSclVtIed *pMmsServerIed = m_pSimConfig->m_MmsServerIed.FetchFirst(pos1);
	bool bThisUsed = false;
	while(pMmsServerIed)
	{
		if(SString::equals(m_sMmsDsIedName,pMmsServerIed->name))
		{
			bThisUsed = true;
			break;
		}
		pMmsServerIed = m_pSimConfig->m_MmsServerIed.FetchNext(pos1);
	}
	if (!bThisUsed)
	{
		//没有对应的设备
		return 4;
	}
	SString sVal = strVal;//strCurText.toStdString().data();
	//pCurFcda->value = sVal;
	if(!m_pSimMmsServer->SetLeafVarValueByPath(path,sVal))
	{
		//SQt::ShowErrorBox("提示","写入MMS服务节点时失败!");
		return 7;
	}
	return 0;
}

bool view_home::startServer()
{
	//return m_pPanelSimRoot->CommandToStart();
	CSimMmsServer * m_pSimMmsServer = &(m_pSimManager->m_MmsServer);
	if(m_pSimMmsServer->IsRuning())
	{
		return true;
	}else
	{
		emit sigStartSever();
	}
	 return true;
}
bool view_home::stopServer()
{
	 emit sigStopSever();
	 return true;
}

bool view_home::startSimClient()
{
	SString strModuleName="sim_iec61850_server";
	stModule* pCurModule = m_configFileMgr.getModuleByName(strModuleName);
	if (pCurModule == NULL)
	{
		LOGERROR("获取模块对象失败，请检查配置文件是否正确！");
		return false;
	}
	SString strSeverIp = pCurModule->stConServer.strIp;
	SString strServerPort = pCurModule->stConServer.strPort;
	bool bRst = m_simClient.Start(pCurModule->strName,pCurModule->strModule,strSeverIp,strServerPort.toInt());
	if (!bRst)
	{
		LOGERROR("连接智能告警测控工具IP[%s]端口[%s]失败！",strSeverIp.data(),strServerPort.data());
		return false;
	}
	return true;
}