#include "dlg_svg_dw_setup.h"

CDlgSvgDwSetup::CDlgSvgDwSetup(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	connect(ui.twDW,SIGNAL(cellDoubleClicked(int,int)),this,SLOT(cellDoubleClickedDW(int,int)));
	connect(ui.twSession,SIGNAL(cellDoubleClicked(int,int)),this,SLOT(cellDoubleClickedSession(int,int)));
	connect(ui.twDW,SIGNAL(itemChanged(QTableWidgetItem*)),this,SLOT(itemChangedDW(QTableWidgetItem*)));
	connect(ui.twSession,SIGNAL(itemChanged(QTableWidgetItem*)),this,SLOT(itemChangedSession(QTableWidgetItem*)));
	ui.twDW->setColumnCount(2);
	ui.twSession->setColumnCount(2);
	ui.twDW->setHorizontalHeaderLabels(QStringList() << "名称"<<"值");
	ui.twSession->setHorizontalHeaderLabels(QStringList() << "名称"<<"值");
}

CDlgSvgDwSetup::~CDlgSvgDwSetup()
{

}

//////////////////////////////////////////////////////////////////////////
// 描    述:  显示指定的数据集
// 作    者:  邵凯田
// 创建时间:  2017-12-14 10:24
// 参数说明:  @pDs为数据集指针
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CDlgSvgDwSetup::ShowDataset(CSsp_Dataset *pDs)
{
	SString sql = pDs->GetSql();
	SString name,val;
	SStringList slNames,slVals;
	//查询sql中的会话属性
	int pos=0;
	int i;
	int p1=0,p2;
	while(1)
	{
		p1 = sql.Find("{@SESSION:",p1);
		if(p1 < 0)
			break;
		p2 = sql.Find("@}",p1);
		if(p2 < 0)
			break;
		p1 += 10;
		name = sql.Mid(p1,p2-p1);
		//看是否已经存在了
		bool bFind = false;
		for(i=0;i<slNames.count();i++)
		{
			if(*slNames[i] == name)
			{
				bFind = true;
				break;
			}
		}
		p1 = p2+2;
		if(bFind)
			continue;

		val = SSP_BASE->GetSessionAttributeValue(name.data());
		slNames.append(name);
		slVals.append(val);
	}
	QString qstr;
	ui.twDW->setRowCount(slNames.count());
	for(i=0;i<slNames.count();i++)
	{
		qstr = slNames[i]->data();
		ui.twDW->setItem(i,0,new QTableWidgetItem(qstr));
		qstr = slVals[i]->data();
		ui.twDW->setItem(i,1,new QTableWidgetItem(qstr));
	}

	slNames.clear();
	slVals.clear();

	int cnt = SSP_BASE->GetSessionAttributeCount();
	for(i=0;i<cnt;i++)
	{
		if(!SSP_BASE->GetIdSessionAttribute(i,name,val))
			break;
		slNames.append(name);
		slVals.append(val);		
	}

	SStringList slSortNames,slSortVals;
	while(slNames.count() > 0)
	{
		int min = 0;
		for(i=1;i<slNames.count();i++)
		{
			if(*slNames[i] < *slNames[min])
				min = i;
		}
		slSortNames.append(*slNames[min]);
		slSortVals.append(*slVals[min]);
		slNames.remove(min);
		slVals.remove(min);
	}

	ui.twSession->setRowCount(slSortNames.count());
	for(i=0;i<slSortNames.count();i++)
	{
		qstr = slSortNames[i]->data();
		ui.twSession->setItem(i,0,new QTableWidgetItem(qstr));
		qstr = slSortVals[i]->data();
		ui.twSession->setItem(i,1,new QTableWidgetItem(qstr));
	}
}

void CDlgSvgDwSetup::cellDoubleClickedDW(int row,int col)
{
	QTableWidgetItem *item = ui.twDW->item(row,col);
	if(item == NULL)
		return;
	if(col != 1)
		return;
	ui.twDW->editItem(item);
}

void CDlgSvgDwSetup::cellDoubleClickedSession(int row,int col)
{
	QTableWidgetItem *item = ui.twSession->item(row,col);
	if(item == NULL)
		return;
	if(col != 1)
		return;
	ui.twSession->editItem(item);
}

void CDlgSvgDwSetup::itemChangedDW(QTableWidgetItem* item)
{
	SString name = ui.twDW->item(item->row(),0)->text().toStdString().data();
	SString val = item->text().toStdString().data();
	SSP_BASE->SetSessionAttributeValue(name.data(),val);
}

void CDlgSvgDwSetup::itemChangedSession(QTableWidgetItem* item)
{
	SString name = ui.twSession->item(item->row(),0)->text().toStdString().data();
	SString val = item->text().toStdString().data();
	SSP_BASE->SetSessionAttributeValue(name.data(),val);
}