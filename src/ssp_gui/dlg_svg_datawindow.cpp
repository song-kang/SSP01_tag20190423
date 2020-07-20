#include "dlg_svg_datawindow.h"
#include "dlg_svg_dw_setup.h"

CDlgSvgDataWindow::CDlgSvgDataWindow(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	//setWindowModality(Qt::WindowModal);
	setWindowFlags( Qt::Dialog/*| Qt::Tool*/);
// #ifdef WIN32
// 	setWindowFlags( Qt::Dialog | Qt::Tool | Qt::WindowStaysOnTopHint/* | Qt::MSWindowsFixedSizeDialogHint*/);
// #else
// 	setWindowFlags( Qt::Dialog | Qt::Tool | Qt::WindowStaysOnTopHint );
// #endif
	connect(ui.cmbClsName,SIGNAL(currentIndexChanged(int)),this,SLOT(currentIndexChangedClsName(int)));
	connect(ui.cmbName,SIGNAL(currentIndexChanged(int)),this,SLOT(currentIndexChangedName(int)));
	connect(ui.edtFilter,SIGNAL(textChanged(const QString &)),this,SLOT(textChangedFilter(const QString &)));
	connect(ui.twDataset,SIGNAL(cellDoubleClicked(int,int)),this,SLOT(cellDoubleClickedDs(int,int)));
	connect(ui.btnCfg,SIGNAL(clicked()),this,SLOT(onBtnCfg()));
}

CDlgSvgDataWindow::~CDlgSvgDataWindow()
{
	
}

void CDlgSvgDataWindow::ShowDlg()
{
	CSsp_DatawindowMgr *pDW = GET_DW_CFG;
	if(pDW == NULL)
	{
		SQt::ShowErrorBox("错误","无效的数据窗口配置");
		return;
	}
	SString sClsName;
	SStringList slClsName;
	unsigned long pos;
	int i,cnt;
	sClsName = "全部";
	slClsName.append(sClsName);
	CSsp_Dataset *pDS = pDW->GetDatasetPtr()->FetchFirst(pos);
	while(pDS)
	{
		sClsName = pDS->GetClsName();
		bool bFind = false;
		for(i=0;i<slClsName.count();i++)
		{
			if(*slClsName.at(i) == sClsName)
			{
				bFind = true;
				break;
			}
		}
		if(!bFind)
			slClsName.append(sClsName);
		pDS = pDW->GetDatasetPtr()->FetchNext(pos);
	}
	
	SString sOldClsName = ui.cmbClsName->currentIndex()<0?"":ui.cmbClsName->itemText(ui.cmbClsName->currentIndex()).toStdString().data();
	if(sOldClsName.length() == 0)
		sOldClsName = "全部";
	ui.cmbClsName->clear();
	QString qstr;
	bool bClsSelected = false;
	for(i=0;i<slClsName.count();i++)
	{
		qstr = slClsName[i]->data();
		ui.cmbClsName->addItem(qstr);
		if(sOldClsName == slClsName[i]->data())
		{
			ui.cmbClsName->setCurrentIndex(i);
			bClsSelected = true;
		}
	}
	if(!bClsSelected)
		ui.cmbClsName->setCurrentIndex(0);
	show();
}

void CDlgSvgDataWindow::currentIndexChangedClsName(int index)
{
	if(ui.cmbClsName->currentIndex() < 0)
		return;
	SString sFilter = ui.edtFilter->text().toStdString().data();
	SString sClsName = ui.cmbClsName->itemText(ui.cmbClsName->currentIndex()).toStdString().data();
	SString sOldName = ui.cmbName->currentIndex()<0?"":ui.cmbName->itemText(ui.cmbName->currentIndex()).toStdString().data();
	sOldName = SString::GetIdAttribute(1,sOldName," - ");

	m_sLastDsName = sOldName;
	SString str;
	QString qstr;
	int i=0;
	unsigned long pos;
	CSsp_DatawindowMgr *pDW = GET_DW_CFG;
	ui.cmbName->clear();
	CSsp_Dataset *pDS = pDW->GetDatasetPtr()->FetchFirst(pos);
	while(pDS)
	{
		str = pDS->GetClsName();
		if((sClsName != "全部" && str != sClsName) ||
			(sFilter.length() > 0 && pDS->GetName().Find(sFilter)<0 && pDS->GetDesc().Find(sFilter)<0))
		{
			pDS = pDW->GetDatasetPtr()->FetchNext(pos);
			continue;
		}
		qstr = pDS->GetName().data();
		qstr += " - ";
		qstr += pDS->GetDesc().data();
		//qstr.sprintf("%s - %s",pDS->GetName().data(), pDS->GetDesc().data());
		ui.cmbName->addItem(qstr);
		if(sOldName == pDS->GetName())
			ui.cmbName->setCurrentIndex(i);
		i++;
		pDS = pDW->GetDatasetPtr()->FetchNext(pos);
	}
}

void CDlgSvgDataWindow::textChangedFilter(const QString &text)
{
	
	if(ui.cmbClsName->currentIndex() < 0)
		return;
	currentIndexChangedClsName(ui.cmbClsName->currentIndex());
}

void CDlgSvgDataWindow::ShowDataset(CSsp_Dataset *pDS)
{
	SSP_GUI->ShowWaiting();
	QString qstr;
	int i,j,cnt;
	SRecordset *pRs = pDS->GetInnerRecordset();
	ui.twDataset->setColumnCount(pDS->GetColumnCount());
	cnt = pRs->GetRows();
	ui.twDataset->setRowCount(cnt);
	//写列名
	QStringList labels;
	for(i=0;i<pDS->GetColumnCount();i++)
	{
		CSsp_DwColumn *pCol = pDS->GetColumn(i);
		if(pCol == NULL)
			continue;
		qstr = pCol->m_sName.data();
		labels.append(qstr);
	}
	ui.twDataset->setHorizontalHeaderLabels(labels);
	SRecord *pRec;
	for(i=0;i<cnt;i++)
	{
		pRec = pRs->GetRecord(i);
		if(pRec == NULL)
			continue;
		for(j=0;j<pRs->GetColumns();j++)
		{
			qstr = pRec->GetValueStr(j);
			ui.twDataset->setItem(i,j,new QTableWidgetItem(qstr));
		}
	}
	ui.twDataset->resizeColumnsToContents();
	SSP_GUI->HideWaiting();
}

void CDlgSvgDataWindow::currentIndexChangedName(int index)
{
	if(ui.cmbName->currentIndex() < 0)
		return;
	SString sName = ui.cmbName->itemText(ui.cmbName->currentIndex()).toStdString().data();
	sName = SString::GetIdAttribute(1,sName," - ");
	if(sName.length() > 0 && m_sLastDsName == sName)
		return;
	CSsp_DatawindowMgr *pDW = GET_DW_CFG;
	CSsp_Dataset *pDS = pDW->SearchDataset(sName);
	if(pDS == NULL)
	{
		SQt::ShowErrorBox("提示",SString::toFormat("数据窗口[%s]不存在!",sName.data()).data());
		return;
	}
	ShowDataset(pDS);
}

void CDlgSvgDataWindow::cellDoubleClickedDs(int row, int column)
{
	if(ui.cmbName->currentIndex() < 0 || row<0 || column <0)
		return;
	SString sName = ui.cmbName->itemText(ui.cmbName->currentIndex()).toStdString().data();
	sName = SString::GetIdAttribute(1,sName," - ");
	QString qstr = sName.data();
	emit sigDatasetCell(qstr,row,column);
}

void CDlgSvgDataWindow::onBtnCfg()
{
	SString sName = ui.cmbName->itemText(ui.cmbName->currentIndex()).toStdString().data();
	sName = SString::GetIdAttribute(1,sName," - ");
	CSsp_DatawindowMgr *pDW = GET_DW_CFG;
	CSsp_Dataset *pDS = pDW->SearchDataset(sName);
	if(pDS == NULL)
	{
		return;
	}
	CDlgSvgDwSetup dlg(this);
	dlg.ShowDataset(pDS);
	dlg.exec();
	pDS->SetInnerRsIsDirty();
	ShowDataset(pDS);
}
