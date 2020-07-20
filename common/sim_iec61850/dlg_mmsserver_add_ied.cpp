#include "dlg_mmsserver_add_ied.h"
#include <QCheckBox>
#include "SQt.h"
dlg_mmsserver_add_ied::dlg_mmsserver_add_ied(CSimConfig *pCfg,QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	Qt::WindowFlags flags=Qt::Dialog;
	flags |=Qt::WindowCloseButtonHint;
	setWindowFlags(flags);
	setWindowTitle("选择添加设备");
	m_pSimConfig = pCfg;
	initTable();
	connect(ui.pushButton_ok,SIGNAL(clicked()),this,SLOT(OnOk()));
	connect(ui.pushButton_cancel,SIGNAL(clicked()),this,SLOT(OnCancel()));
	connect(ui.selectAllBtn,SIGNAL(clicked()),this,SLOT(slotSelectAllBtn()));
	connect(ui.selectInvBtn,SIGNAL(clicked()),this,SLOT(slotSelectInvBtn()));
}

dlg_mmsserver_add_ied::~dlg_mmsserver_add_ied()
{

}

void dlg_mmsserver_add_ied::initTable()
{
	QStringList labels;
	labels << " " << "IED名称" << "IED描述" << "制造厂商" << "类型" << "版本" ; 
	ui.tableWidget->setColumnCount(labels.size());
	ui.tableWidget->setHorizontalHeaderLabels(labels);
	ui.tableWidget->setColumnWidth(0,20);
	ui.tableWidget->setColumnWidth(1,70);
	ui.tableWidget->setColumnWidth(2,200);
	ui.tableWidget->setColumnWidth(3,80);
	ui.tableWidget->setColumnWidth(4,70);
	ui.tableWidget->setColumnWidth(5,100);


	unsigned long pos1,pos2,pos3,pos4;
	char* ied_name;
	int no;	
	int cnt = 0;
	stuSclVtSubNetwork *pNet = m_pSimConfig->m_VirtualTerminal.m_SubNetwork.FetchFirst(pos1);
	while(pNet)
	{
		if(strstr(pNet->type,"MMS") != NULL || strstr(pNet->name,"MMS")  != NULL)
		{
			//MMS子网
			stuSclVtSubNetworkConnectAP *pAP = pNet->m_ConnectAP.FetchFirst(pos2);
			while(pAP)
			{
				stuSclVtIed *pIed = m_pSimConfig->m_VirtualTerminal.SearchIedByName((char*)pAP->ied_name);
				if(pIed != NULL)
				{
					int row = ui.tableWidget->rowCount();
					ui.tableWidget->insertRow(row);
					stuSclVtIed *pMmsServerIed = m_pSimConfig->m_MmsServerIed.FetchFirst(pos3);
					bool bThisUsed = false;
					while(pMmsServerIed)
					{
						if(SString::equals(pIed->name,pMmsServerIed->name))
						{
							bThisUsed = true;
							break;
						}
						pMmsServerIed = m_pSimConfig->m_MmsServerIed.FetchNext(pos3);
					}
					if (!bThisUsed)
					{
						QCheckBox *pBox = new QCheckBox();
						ui.tableWidget->setCellWidget(row,0,pBox);
						pBox->setCheckState(Qt::Unchecked);
						cnt++;
					}
					ui.tableWidget->setItem(row,1,new QTableWidgetItem(pIed->name));
					ui.tableWidget->setItem(row,2,new QTableWidgetItem(pIed->desc));
					ui.tableWidget->setItem(row,3,new QTableWidgetItem(pIed->manufacturer));
					ui.tableWidget->setItem(row,4,new QTableWidgetItem(pIed->type));
					ui.tableWidget->setItem(row,5,new QTableWidgetItem(pIed->configVersion));
					if(bThisUsed)
					{
						for(int j=1;j<=5;j++)
							ui.tableWidget->item(row,j)->setTextColor(Qt::darkGray);
					}

				}
				pAP = pNet->m_ConnectAP.FetchNext(pos2);
			}//end while(pAP)
		}//end if MMS
		pNet = m_pSimConfig->m_VirtualTerminal.m_SubNetwork.FetchNext(pos1);
	}

	/*
	stuSclVtIed *pIed = m_pSimConfig->m_VirtualTerminal.m_Ied.FetchFirst(pos1);
	while(pIed)
	{
		int row = ui.tableWidget->rowCount();
		ui.tableWidget->insertRow(row);
		stuSclVtIed *pMmsServerIed = m_pSimConfig->m_MmsServerIed.FetchFirst(pos2);
		bool bThisUsed = false;
		while(pMmsServerIed)
		{
			if(SString::equals(pIed->name,pMmsServerIed->name))
			{
					bThisUsed = true;
					break;
			}
			pMmsServerIed = m_pSimConfig->m_MmsServerIed.FetchNext(pos2);
		}
		if (!bThisUsed)
		{
			QCheckBox *pBox = new QCheckBox();
			ui.tableWidget->setCellWidget(row,0,pBox);
			pBox->setCheckState(Qt::Unchecked);
			cnt++;
		}
		ui.tableWidget->setItem(row,1,new QTableWidgetItem(pIed->name));
		ui.tableWidget->setItem(row,2,new QTableWidgetItem(pIed->desc));
		ui.tableWidget->setItem(row,3,new QTableWidgetItem(pIed->manufacturer));
		ui.tableWidget->setItem(row,4,new QTableWidgetItem(pIed->type));
		ui.tableWidget->setItem(row,5,new QTableWidgetItem(pIed->configVersion));
		if(bThisUsed)
		{
			for(int j=1;j<=5;j++)
				ui.tableWidget->item(row,j)->setTextColor(Qt::darkGray);
		}

		pIed = m_pSimConfig->m_VirtualTerminal.m_Ied.FetchNext(pos1);
	}
	*/
	if(cnt == 0)
		SQt::ShowInfoBox("提示","未发现可选用的对象!");
	SQt::SetTableWidgetStyle(ui.tableWidget);
	ui.tableWidget->resizeColumnsToContents();
}


void dlg_mmsserver_add_ied::OnOk()
{
	int i,cnt = ui.tableWidget->rowCount();
	QCheckBox *pBox;
	m_SelectedIed.clear();
	for(i=0;i<cnt;i++)
	{
		pBox = (QCheckBox*)ui.tableWidget->cellWidget(i,0);
		if(pBox == NULL || pBox->checkState()!=Qt::Checked)
			continue;
		stuSclVtIed *pIed = m_pSimConfig->m_VirtualTerminal.SearchIedByName((char*)ui.tableWidget->item(i,1)->text().toStdString().data());
		if(pIed != NULL)
		{
			m_SelectedIed.append(pIed);
		}
	}
	if(m_SelectedIed.count() <= 0)
	{
		SQt::ShowInfoBox("提示","尚未选中任何对象!");
		return ;
	}
	accept();
}

void dlg_mmsserver_add_ied::OnCancel()
{
	reject();
}

void dlg_mmsserver_add_ied::slotSelectAllBtn()
{
	QCheckBox *pBox;
	for (int i = 0; i < ui.tableWidget->rowCount(); i++)
	{
		pBox = (QCheckBox*)ui.tableWidget->cellWidget(i,0);
		if (pBox == NULL)
		{
			continue;
		}
		if(pBox->checkState()!=Qt::Checked)
		{
			pBox->setChecked(true);
		}
	}
}

void dlg_mmsserver_add_ied::slotSelectInvBtn()
{
	QCheckBox *pBox;
	for (int i = 0; i < ui.tableWidget->rowCount(); i++)
	{
		pBox = (QCheckBox*)ui.tableWidget->cellWidget(i,0);
		if (pBox == NULL)
		{
			continue;
		}
		if(pBox->checkState()!=Qt::Checked)
		{
			pBox->setChecked(true);
		} else {
			pBox->setChecked(false);
		}
	}
}