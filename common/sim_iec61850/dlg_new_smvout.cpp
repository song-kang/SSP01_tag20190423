#include "dlg_new_smvout.h"
#include <QCheckBox>
#include "SQt.h"
dlg_new_smvout::dlg_new_smvout(CSimConfig *pCfg,stuSimSubnetwork *pNet,QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	QStringList labels;
	ui.tableWidget->setColumnCount(7);
	labels << " " << "IED名称" << "IED描述" << "控制块序号" << "AppID" << "smvID" << "关联端口"; 
	ui.tableWidget->setHorizontalHeaderLabels(labels);
	ui.tableWidget->setColumnWidth(0,20);
	ui.tableWidget->setColumnWidth(1,70);
	ui.tableWidget->setColumnWidth(2,200);
	ui.tableWidget->setColumnWidth(3,80);
	ui.tableWidget->setColumnWidth(4,70);
	ui.tableWidget->setColumnWidth(5,100);
	ui.tableWidget->setColumnWidth(6,150);

	m_pSimConfig = pCfg;
	m_pThisNet = pNet;
	unsigned long pos1,pos2,pos3,pos4;
	char* ied_name;
	int no;
	int cnt = 0;
	stuSclVtIed *pIed = m_pSimConfig->m_VirtualTerminal.m_Ied.FetchFirst(pos1);
	while(pIed)
	{
		stuSclVtIedSmvOut *pOut = pIed->m_SmvOut.FetchFirst(pos2);
		while(pOut)
		{
			int row = ui.tableWidget->rowCount();
			ui.tableWidget->insertRow(row);
			ied_name = pOut->ied_name.data();
			no = pOut->smv_no;
			bool bThisUsed = false;
			SString sUsedPort;
			stuSimSubnetwork *pSubNet = m_pSimConfig->m_SubNet.FetchFirst(pos3);
			while(pSubNet)
			{
				stuSclVtIedSmvOut *pOut2 = pSubNet->m_Smv.FetchFirst(pos4);
				while(pOut2)
				{
					if(SString::equals(pOut2->ied_name.data(),ied_name) && pOut2->smv_no == no)
					{
						if(pSubNet == m_pThisNet)
							bThisUsed = true;
						if(sUsedPort.length() > 0)
							sUsedPort += ",";
						sUsedPort += SString::toFormat("[%s]",pSubNet->name.data());
					}
					pOut2 = pSubNet->m_Smv.FetchNext(pos4);
				}
				pSubNet = m_pSimConfig->m_SubNet.FetchNext(pos3);
			}
			if(!bThisUsed)
			{
				QCheckBox *pBox = new QCheckBox();
				ui.tableWidget->setCellWidget(row,0,pBox);
				pBox->setCheckState(Qt::Unchecked);
				cnt++;
			}

			ui.tableWidget->setItem(row,1,new QTableWidgetItem(pOut->ied_name.data()));
			ui.tableWidget->setItem(row,2,new QTableWidgetItem(pIed->desc));
			ui.tableWidget->setItem(row,3,new QTableWidgetItem(SString::toFormat("%d",pOut->smv_no).data()));
			ui.tableWidget->setItem(row,4,new QTableWidgetItem(SString::toFormat("0x%04X",pOut->appid).data()));
			ui.tableWidget->setItem(row,5,new QTableWidgetItem(pOut->smvID));
			ui.tableWidget->setItem(row,6,new QTableWidgetItem(sUsedPort.data()));
			if(bThisUsed)
			{
				for(int j=1;j<=6;j++)
					ui.tableWidget->item(row,j)->setTextColor(Qt::darkGray);
			}
			pOut = pIed->m_SmvOut.FetchNext(pos2);
		}
		pIed = m_pSimConfig->m_VirtualTerminal.m_Ied.FetchNext(pos1);
	}
	
	if(cnt == 0)
		SQt::ShowInfoBox("提示","未发现可选用的对象!");
	connect(ui.pushButton_ok,SIGNAL(clicked()),this,SLOT(OnOk()));
	connect(ui.pushButton_cancel,SIGNAL(clicked()),this,SLOT(OnCancel()));
	SQt::SetTableWidgetStyle(ui.tableWidget);
	ui.tableWidget->resizeColumnsToContents();
}

dlg_new_smvout::~dlg_new_smvout()
{

}


void dlg_new_smvout::OnOk()
{
	int i,cnt = ui.tableWidget->rowCount();
	QCheckBox *pBox;
	m_SelectedSmv.clear();
	for(i=0;i<cnt;i++)
	{
		pBox = (QCheckBox*)ui.tableWidget->cellWidget(i,0);
		if(pBox == NULL || pBox->checkState()!=Qt::Checked)
			continue;
		stuSclVtIed *pIed = m_pSimConfig->m_VirtualTerminal.SearchIedByName((char*)ui.tableWidget->item(i,1)->text().toStdString().data());
		if(pIed != NULL)
		{
			stuSclVtIedSmvOut *pOut = pIed->SearchSmvOutByNo(atoi(ui.tableWidget->item(i,3)->text().toStdString().data()));
			if(pOut != NULL)
			{
				m_SelectedSmv.append(pOut);
			}
		}
	}
	if(m_SelectedSmv.count() <= 0)
	{
		SQt::ShowInfoBox("提示","尚未选中任何对象!");
		return ;
	}
	accept();
}

void dlg_new_smvout::OnCancel()
{
	reject();
}
