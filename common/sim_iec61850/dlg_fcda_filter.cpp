#include "dlg_fcda_filter.h"

dlg_fcda_filter::dlg_fcda_filter(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
#ifdef WIN32
	setWindowFlags( Qt::Dialog | Qt::Tool | Qt::WindowStaysOnTopHint);
#else
	setWindowFlags( Qt::Dialog );
#endif
	QStringList labels;
	ui.twResult->setColumnCount(6);
	labels << "IED名称" << "IED描述" << "数据集" << "信号名称" << "MMS路径" << "当前值"; 
	ui.twResult->setHorizontalHeaderLabels(labels);
	ui.twResult->setColumnWidth(0,70);
	ui.twResult->setColumnWidth(1,200);
	ui.twResult->setColumnWidth(2,100);
	ui.twResult->setColumnWidth(3,200);
	ui.twResult->setColumnWidth(4,100);
	ui.twResult->setColumnWidth(5,200);
	m_pTextEdit = NULL;
	connect(ui.edtName,SIGNAL(textChanged(const QString &)),this,SLOT(OnNameChanged(const QString &)));
	connect(ui.twResult,SIGNAL(cellDoubleClicked(int,int)),this,SLOT(OnTableDbClicked(int,int)));

	SQt::SetTableWidgetStyle(ui.twResult);
}

dlg_fcda_filter::~dlg_fcda_filter()
{

}

void dlg_fcda_filter::OnNameChanged(const QString &str)
{
	SString name = str.toStdString().data();
	ui.twResult->setRowCount(0);
	unsigned long pos,pos2,pos3;
	SString strValue;
	int row;
	stuSclVtIed *pIed = m_pSimConfig->m_MmsServerIed.FetchFirst(pos);
	while(pIed)
	{
		stuSclVtIedDataset *pDataset = pIed->m_Dataset.FetchFirst(pos2);
		while(pDataset)
		{
			stuSclVtIedDatasetFcda *pFcda = pDataset->m_Fcda.FetchFirst(pos3);
			while(pFcda)
			{
				if(strstr(pFcda->dai_desc,name.data()) != NULL || strstr(pFcda->doi_desc,name.data()) != NULL)
				{
					//match it
					row = ui.twResult->rowCount();
					ui.twResult->insertRow(row);
					ui.twResult->setItem(row,0,new QTableWidgetItem(pIed->name));
					ui.twResult->setItem(row,1,new QTableWidgetItem(pIed->desc));
					ui.twResult->setItem(row,2,new QTableWidgetItem(pDataset->desc));
					if(SString::equals(pFcda->doi_desc,pFcda->dai_desc))
						ui.twResult->setItem(row,3,new QTableWidgetItem(pFcda->doi_desc));
					else
						ui.twResult->setItem(row,3,new QTableWidgetItem(SString::toFormat("DOI:%s  DAI:%s",pFcda->doi_desc,pFcda->dai_desc).data()));
					ui.twResult->setItem(row,4,new QTableWidgetItem(SString::toFormat("%s%s",pIed->name,pFcda->path.data()).data()));
					if(m_pSimMmsServer->IsRuning())
						strValue = m_pSimMmsServer->GetLeafVarValueByPath(SString::toFormat("%s%s",pIed->name,pFcda->path.data()));
					ui.twResult->setItem(row,5,new QTableWidgetItem(strValue.data()));
				}
				pFcda = pDataset->m_Fcda.FetchNext(pos3);
			}
			pDataset = pIed->m_Dataset.FetchNext(pos2);
		}
		pIed = m_pSimConfig->m_MmsServerIed.FetchNext(pos);
	}
	//m_pTextEdit insertPlainText(QSTRING)

}

void dlg_fcda_filter::OnTableDbClicked(int row,int col)
{
	if(row < 0)
		return;
	SString sText;
	//$SetLeaf("PM2201PROT","GGIO5$ST$Op01",1,"{ 1 , 0 , 0 , 0 , 0 ,  , $(now)}");
	SString path,ld,leaf,val,desc;
	path = ui.twResult->item(row,4)->text().toStdString().data();
	int p1 = path.find("/");
	if(p1 < 0)
		return;
	ld = path.left(p1);
	leaf = path.mid(p1+1);
	if(m_pSimMmsServer->IsRuning())
		val = m_pSimMmsServer->GetLeafVarValueByPath(path);
	else
		val = "";
	desc = ui.twResult->item(row,0)->text().toStdString().data();
	desc += "-";
	desc += ui.twResult->item(row,1)->text().toStdString().data();
	desc += " ";
	desc += ui.twResult->item(row,2)->text().toStdString().data();
	desc += " ";
	desc += ui.twResult->item(row,3)->text().toStdString().data();
	sText.sprintf("  //%s\r\n  $SetLeaf(\"%s\",\"%s\",1,\"%s\");\r\n",
		desc.data(),ld.data(),leaf.data(),val.data());

	QString str = sText.data();
	m_pTextEdit->insertPlainText(str);
}
