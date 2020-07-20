#include "dlg_new_net.h"
#include <QCheckBox>
#include "SQt.h"

#ifdef WIN32
#ifdef _DEBUG
#pragma comment(lib,"sbase_rawsockd.lib")
#else
#pragma comment(lib,"sbase_rawsock.lib")
#endif
#endif

dlg_new_net::dlg_new_net(CSimConfig *pCfg,QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	m_SelectedNetDev.setAutoDelete(true);

	QStringList labels;
	ui.tableWidget->setColumnCount(7);
	labels << " " << "名称" << "描述" << "IP" << "子网IP地址" << "子网掩码" << "MAC地址"; 
	ui.tableWidget->setHorizontalHeaderLabels(labels);
	ui.tableWidget->setColumnWidth(0,20);
#ifdef WIN32
	ui.tableWidget->setColumnWidth(1,200);
	ui.tableWidget->setColumnWidth(2,150);
#else
	ui.tableWidget->setColumnWidth(1,70);
	ui.tableWidget->setColumnWidth(2,100);
#endif
	ui.tableWidget->setColumnWidth(3,120);
	ui.tableWidget->setColumnWidth(4,120);
	ui.tableWidget->setColumnWidth(5,120);
	ui.tableWidget->setColumnWidth(6,120);

	m_pSimConfig = pCfg;
	SPtrList<SRawSocketRecv::stuPcapDevice> list;
	SRawSocketRecv::FindAllPcapDevices(list);
#ifndef WIN32
	if(list.count() == 0)
	{
		SRawSocketRecv::SetEthNamePrex("em");
		SRawSocketRecv::FindAllPcapDevices(list);
	}
#endif
	ui.tableWidget->setRowCount(list.count());
	int cnt = 0;
	for(int row=0;row<list.count();row++)
	{
		SRawSocketRecv::stuPcapDevice *p = list[row];
		//判断当前网口是否已经添加过?
		unsigned long pos=0;
		stuSimSubnetwork *pSubNet = m_pSimConfig->m_SubNet.FetchFirst(pos);
		while(pSubNet)
		{
			if(pSubNet->name == p->name)
				break;
			pSubNet = m_pSimConfig->m_SubNet.FetchNext(pos);
		}
		if(pSubNet == NULL)
		{
			QCheckBox *pBox = new QCheckBox();
			ui.tableWidget->setCellWidget(row,0,pBox);
			pBox->setCheckState(Qt::Unchecked);
			cnt ++;
			//pBox->setCheckState(Qt::Checked);
		}
		// 		else
		// 		{
		// 			pBox->setCheckState(Qt::Unchecked);
		// 			pBox->setEnabled(false);
		// 		}
		ui.tableWidget->setItem(row,1,new QTableWidgetItem(p->name.data()));
		ui.tableWidget->setItem(row,2,new QTableWidgetItem(p->desc.data()));
		ui.tableWidget->setItem(row,3,new QTableWidgetItem(p->ip.data()));
		ui.tableWidget->setItem(row,4,new QTableWidgetItem(p->subnetip.data()));
		ui.tableWidget->setItem(row,5,new QTableWidgetItem(p->netmask.data()));
		ui.tableWidget->setItem(row,6,new QTableWidgetItem(p->mac.data()));

	}
	if(cnt == 0)
		SQt::ShowInfoBox("提示","所有网口均已添加到模拟器中!");
	connect(ui.pushButton_ok,SIGNAL(clicked()),this,SLOT(OnOk()));
	connect(ui.pushButton_cancel,SIGNAL(clicked()),this,SLOT(OnCancel()));
	SQt::SetTableWidgetStyle(ui.tableWidget);
	ui.tableWidget->resizeColumnsToContents();
}

dlg_new_net::~dlg_new_net()
{

}

void dlg_new_net::OnOk()
{
	int i,cnt = ui.tableWidget->rowCount();
	QCheckBox *pBox;
	m_SelectedNetDev.clear();
	for(i=0;i<cnt;i++)
	{
		pBox = (QCheckBox*)ui.tableWidget->cellWidget(i,0);
		if(pBox == NULL || pBox->checkState()!=Qt::Checked)
			continue;
		SRawSocketRecv::stuPcapDevice *pNew = new SRawSocketRecv::stuPcapDevice();
		pNew->name = ui.tableWidget->item(i,1)->text().toStdString().data();
		pNew->desc = ui.tableWidget->item(i,2)->text().toStdString().data();
		pNew->ip = ui.tableWidget->item(i,3)->text().toStdString().data();
		pNew->subnetip = ui.tableWidget->item(i,4)->text().toStdString().data();
		pNew->netmask = ui.tableWidget->item(i,5)->text().toStdString().data();
		pNew->mac = ui.tableWidget->item(i,6)->text().toStdString().data();
		m_SelectedNetDev.append(pNew);
	}
	if(m_SelectedNetDev.count() <= 0)
	{
		SQt::ShowInfoBox("提示","尚未选中任何网口!");
		return ;
	}
	accept();
}

void dlg_new_net::OnCancel()
{
	reject();
}