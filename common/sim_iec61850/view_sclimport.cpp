/**
 *
 * 文 件 名 : view_sclimport.cpp
 * 创建日期 : 2015-12-23 13:17
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : SCL文件导入
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2015-12-23	邵凯田　创建文件
 *
 **/

#include "view_sclimport.h"
#include "GuiApplication.h"
#include "dlg_set_voltage_current.h"
view_sclimport::view_sclimport(QWidget *parent)
	: CBaseView(parent)
{
	ui.setupUi(this);
	connect(ui.pushButton_open,SIGNAL(clicked()),this,SLOT(OnOpen()));
	connect(ui.pushButton_sel_all,SIGNAL(clicked()),this,SLOT(OnSelAll()));
	connect(ui.pushButton_sel_none,SIGNAL(clicked()),this,SLOT(OnSelNone()));
	connect(ui.pushButton_sel_rev,SIGNAL(clicked()),this,SLOT(OnSelRev()));
	connect(ui.pushButton_import,SIGNAL(clicked()),this,SLOT(OnImport()));

	ui.pushButton_sel_all->setEnabled(false);
	ui.pushButton_sel_none->setEnabled(false);
	ui.pushButton_sel_rev->setEnabled(false);
	ui.pushButton_import->setEnabled(false);
	ui.progressBar->setEnabled(false);

	QStringList labels;
	ui.tableWidget->setColumnCount(6);
	labels << " " << "名称" << "描述" << "型号" << "制造商" << "访问点" ;//<< "数据集数量" << "GOOSE输出" << "SMV输出" << "GOOSE输入" << "SMV输入"; 
	ui.tableWidget->setHorizontalHeaderLabels(labels);
	ui.tableWidget->setColumnWidth(0,20);
	ui.tableWidget->setColumnWidth(1,70);
	ui.tableWidget->setColumnWidth(2,200);
	ui.tableWidget->setColumnWidth(3,80);
	ui.tableWidget->setColumnWidth(4,80);
	ui.tableWidget->setColumnWidth(5,150);
// 	ui.tableWidget->setColumnWidth(5,80);
// 	ui.tableWidget->setColumnWidth(6,80);
// 	ui.tableWidget->setColumnWidth(7,80);
// 	ui.tableWidget->setColumnWidth(8,80);
// 	ui.tableWidget->setColumnWidth(9,80);
	//设置样式
	ui.progressBar->setValue(0);
	SQt::SetTableWidgetStyle(ui.tableWidget);
	m_pSimConfig = &CGuiApplication::GetApp()->m_SimConfig;
	m_pScl = NULL;
}

view_sclimport::~view_sclimport()
{
	if(m_pScl != NULL)
		delete m_pScl;
}

void view_sclimport::OnOpen()
{

	QString filter = tr("SCL 文件(*.scd *.cid *.icd);;SCD 文件(*.scd);;CID 文件(*.cid);;ICD 文件(*.icd);;XML 文件(*.xml);;所有文件(*.*)");
	QString fileName = QFileDialog::getOpenFileName(this,tr("打开文件"),QString::null,filter);
	if (fileName.isEmpty())
		return;
	if(m_pScl != NULL)
	{
		delete m_pScl;
		m_pScl = NULL;
	}
	ui.pushButton_open->setEnabled(false);
	m_sSclFile = fileName.toStdString().data();
	m_iSclLoadState = 0;
	m_pScl = new CSpSclParserEx();
	ui.progressBar->setEnabled(true);
	ui.progressBar->setValue(0);
	SKT_CREATE_THREAD(ThreadLoadScl,this);
	while(m_iSclLoadState == 0)
	{
		ui.progressBar->setValue(m_pScl->GetProgressPecent());
		SApi::UsSleep(100000);
	}
	ui.progressBar->setValue(m_pScl->GetProgressPecent());
	ui.pushButton_open->setEnabled(true);
	ui.progressBar->setEnabled(false);
	if(m_iSclLoadState == 2 || m_pScl->m_IED.count()==0)
	{
		SQt::ShowErrorBox("错误","解析指定的SCL文件时失败,请检查文件的有效性!");
		return;
	}
	ui.tableWidget->setRowCount(m_pScl->m_IED.count());
	unsigned long pos = 0;
	int row = 0;
	SString sAp;
	QCheckBox *pBox;;
	CSpSclParserEx::stuSCL_IED *pIed = m_pScl->m_IED.FetchFirst(pos);
	while(pIed)
	{
		pBox = (QCheckBox*)ui.tableWidget->cellWidget(row,0);
		if(pBox == NULL)
		{
			pBox = new QCheckBox();
			ui.tableWidget->setCellWidget(row,0,pBox);
		}
		pBox->setCheckState(Qt::Checked);
		ui.tableWidget->setItem(row,1,new QTableWidgetItem(pIed->name));
		ui.tableWidget->setItem(row,2,new QTableWidgetItem(pIed->desc));
		ui.tableWidget->setItem(row,3,new QTableWidgetItem(pIed->type));
		ui.tableWidget->setItem(row,4,new QTableWidgetItem(pIed->manufacturer));
		sAp = "";
		unsigned long pos2;
		CSpSclParserEx::stuSCL_IED_AccessPoint *p = pIed->m_AccessPoint.FetchFirst(pos2);
		while(p)
		{
			if(sAp.length() == 0)
				sAp = p->name;
			else
				sAp += SString::toFormat(" / %s",p->name);
			p = pIed->m_AccessPoint.FetchNext(pos2);
		}

		ui.tableWidget->setItem(row,5,new QTableWidgetItem(sAp.data()));
		pIed = m_pScl->m_IED.FetchNext(pos);
		row++;
	}
	ui.tableWidget->resizeColumnsToContents();

	ui.pushButton_sel_all->setEnabled(true);
	ui.pushButton_sel_none->setEnabled(true);
	ui.pushButton_sel_rev->setEnabled(true);
	ui.pushButton_import->setEnabled(true);

}

void* view_sclimport::ThreadLoadScl(void *lp)
{
	view_sclimport *pThis = (view_sclimport*)lp;

	if(!pThis->m_pScl->ParseXmlFile(pThis->m_sSclFile.data()))
	{
		pThis->m_iSclLoadState = 2;		
		return NULL;
	}
	pThis->m_pScl->ProcessDataTypeTemplates();
	pThis->m_iSclLoadState = 1;
	return NULL;
}

void view_sclimport::OnSelAll()
{
	int i,cnt = ui.tableWidget->rowCount();
	QCheckBox *pBox;
	for(i=0;i<cnt;i++)
	{
		pBox = (QCheckBox*)ui.tableWidget->cellWidget(i,0);
		if(pBox == NULL)
			continue;
		pBox->setCheckState(Qt::Checked);
	}
}
void view_sclimport::OnSelNone()
{
	int i,cnt = ui.tableWidget->rowCount();
	QCheckBox *pBox;
	for(i=0;i<cnt;i++)
	{
		pBox = (QCheckBox*)ui.tableWidget->cellWidget(i,0);
		if(pBox == NULL)
			continue;
		pBox->setCheckState(Qt::Unchecked);
	}
}
void view_sclimport::OnSelRev()
{
	int i,cnt = ui.tableWidget->rowCount();
	QCheckBox *pBox;
	for(i=0;i<cnt;i++)
	{
		pBox = (QCheckBox*)ui.tableWidget->cellWidget(i,0);
		if(pBox == NULL)
			continue;
		pBox->setCheckState(pBox->checkState()==Qt::Checked?(Qt::Unchecked):(Qt::Checked));
	}

}

void* view_sclimport::ThreadImportScl(void *lp)
{
	view_sclimport *pThis = (view_sclimport*)lp;
	unsigned long pos,pos2;
	stuSclVtIed *pVtIed;
	CSpSclParserEx::stuSCL_Substation *pSubStation = pThis->m_pScl->m_Substation[0];
	if(pSubStation != NULL)
		pThis->m_pSimConfig->m_VirtualTerminal.m_sStationName = SString::safeChar(pSubStation->desc);
	if(pThis->m_pSimConfig->m_VirtualTerminal.m_sStationName.length() == 0)
	{
		CSpSclParserEx::stuSCL_Header *pHeader = pThis->m_pScl->m_Header[0];
		if(pHeader != NULL)
			pThis->m_pSimConfig->m_VirtualTerminal.m_sStationName = SString::safeChar(pHeader->id);
	}
	pThis->m_pSimConfig->m_VirtualTerminal.LoadCommunicationBySclParser(pThis->m_pScl);
	CSpSclParserEx::stuSCL_IED *pIed = pThis->m_ImportIed.FetchFirst(pos);
	int i=0;
	while(pIed)
	{
		pThis->m_iSclImportPercent = (i*200)/pThis->m_ImportIed.count();i++;
		pVtIed = new stuSclVtIed();
		pVtIed->name		= (char*)pIed->name;
		pVtIed->desc		= SString::safeChar(pIed->desc);
		pVtIed->type		= SString::safeChar(pIed->type);
		pVtIed->manufacturer= SString::safeChar(pIed->manufacturer);
		pVtIed->configVersion=SString::safeChar(pIed->configVersion);

		if(pThis->m_pSimConfig->m_VirtualTerminal.LoadIedBySclParser(pThis->m_pScl,pVtIed,pIed))
			pThis->m_pSimConfig->m_VirtualTerminal.m_Ied.append(pVtIed);
		else
			delete pVtIed;
		pIed = pThis->m_ImportIed.FetchNext(pos);
	}

	pIed = pThis->m_ImportIed.FetchFirst(pos);
	pVtIed = pThis->m_pSimConfig->m_VirtualTerminal.m_Ied.FetchFirst(pos2);
	i=0;
	while(pIed && pVtIed)
	{
		pThis->m_iSclImportPercent = 50+(i*200)/pThis->m_ImportIed.count();i++;
		pThis->m_pSimConfig->m_VirtualTerminal.LoadIedInputsBySclParser(pThis->m_pScl,pVtIed,pIed);
		pIed = pThis->m_ImportIed.FetchNext(pos);
		pVtIed = pThis->m_pSimConfig->m_VirtualTerminal.m_Ied.FetchNext(pos2);
	}
	pThis->m_iSclImportPercent = 100;
	pThis->m_iSclImportState = 1;
	return NULL;
}

void view_sclimport::OnImport()
{
	if(m_pScl == NULL || m_pScl->m_IED.count() == 0)
	{
		SQt::ShowInfoBox("提示","尚未加载有效的SCL文件!");
		return;
	}
	dlg_set_voltage_current dlg(this);
	if (dlg.exec() == QDialog::Rejected)
	{
		QMessageBox::warning(this,"警告","请设置额定电压电流值！");
		return;
	} else 
	{
		m_fPrimaryVoltage = dlg.getPrimaryVoltage().toFloat();
		m_fSecondaryVoltage = dlg.getSecondaryVoltage().toFloat();
		m_fPriaryCurrent = dlg.getPrimaryCurrent().toFloat();
		m_fSecondaryCurrent = dlg.getSecondaryCurrent().toFloat();
		m_pSimConfig->m_VirtualTerminal.m_fPrimaryVoltage = m_fPrimaryVoltage;
		m_pSimConfig->m_VirtualTerminal.m_fSecondaryVoltage = m_fSecondaryVoltage;
		m_pSimConfig->m_VirtualTerminal.m_fPriaryCurrent = m_fPriaryCurrent;
		m_pSimConfig->m_VirtualTerminal.m_fSecondaryCurrent = m_fSecondaryCurrent;

	}
	int i,cnt = ui.tableWidget->rowCount();
	QCheckBox *pBox;
	unsigned long pos = 0;
	CSpSclParserEx::stuSCL_IED *pIed;
	SString iedname;
	m_ImportIed.clear();
	for(i=0;i<cnt;i++)
	{
		pBox = (QCheckBox*)ui.tableWidget->cellWidget(i,0);
		if(pBox == NULL)
			continue;
		iedname = ui.tableWidget->item(i,1)->text().toStdString().data();
		if(pBox->checkState()==Qt::Checked)
		{
			pIed = m_pScl->SearchIedByName(iedname.data());
			if(pIed != NULL)
				m_ImportIed.append(pIed);
		}
	}
	if(m_ImportIed.count() == 0)
	{
		SQt::ShowInfoBox("提示","尚未选中任何IED!");
		return;
	}
	if(!SQt::ShowQuestionBox("提示","导入操作将导致清除所有与SCL相关的配置数据,\r\n您确定要这么做吗?"))
		return;
	SQt::ProcessUiEvents();
	m_pSimConfig->m_MmsServerIed.clear();
	stuSimSubnetwork *pSubNet = m_pSimConfig->m_SubNet.FetchFirst(pos);
	while(pSubNet)
	{
		pSubNet->m_Goose.clear();
		pSubNet->m_Smv.clear();
		pSubNet->m_MmsSvr.clear();
		pSubNet->m_MmsClient.clear();
		pSubNet = m_pSimConfig->m_SubNet.FetchNext(pos);
	}
	m_pSimConfig->m_VirtualTerminal.Clear();
	m_pSimConfig->m_VirtualTerminal.m_sStationName = "";
	m_iSclImportState = 0;
	m_iSclImportPercent = 0;
	ui.progressBar->setEnabled(true);
	SKT_CREATE_THREAD(ThreadImportScl,this);
	while(m_iSclImportState == 0)
	{
		ui.progressBar->setValue(m_iSclImportPercent);
		SApi::UsSleep(100000);
	}
	ui.progressBar->setValue(m_iSclImportPercent);
	ui.progressBar->setEnabled(false);
	if(!m_pSimConfig->Save())
	{
		SQt::ShowErrorBox("提示","保存配置文件时失败!");
	}
	else
	{
		SQt::ShowInfoBox("提示","SCL模型导入成功!");
	}
}
